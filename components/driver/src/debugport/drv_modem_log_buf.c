/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#include "drv_modem_log_buf.h"
#include "hal_shmem_region.h"

#define MODEM_USB_TX_MAX_SIZE (4096)
#define MODEM_PORT_IPC_UART (4)
#define MODEM_PORT_IPC_USB (5)
#define MODEM_PORT_IPC_FLASH (6)
#define MODEM_LOG_BUFFER_OK (0x4F4B)
#define MODEM_MAX_BUF_LEN (512 * 1024) // for sanity check
#define MODEM_MIN_BUF_LEN (1024)       // for sanity check

#define TRACE_CONT_SIZE_MIN (1024)
#define TRACE_TX_POLL_INTERVAL (20)
#define SEND_PACKET_BY_TRACE_TIMEOUT (200)
#define SEND_PACKET_DIRECT_TIMEOUT (50)

typedef volatile struct
{
    uint32_t head;            // log buffer base address
    uint32_t length;          // log buffer total length (2^n BYTES)
    uint32_t rd_index;        // log buffer read offset
    uint32_t wr_index;        // log buffer write offset
    uint32_t cur_wr_index;    // log buffer current write offset
    uint16_t port;            // 0 -> modem uart; 2 -> close(do not send); 4 -> IPC+UART; 5 -> IPC+USB; 6 -> IPC+FLASH
    uint16_t status;          // buffer available: 0K(0x4F4B)
    uint32_t overflow_index;  // write index of protect area
    uint32_t remain_send_len; // remain the send length
    uint32_t cur_send_len;    // current the send length
    uint32_t protect_len;     // log buffer protect length
} modemLogBuf_t;

typedef struct
{
    modemLogBuf_t *buf;
    bool is_zsp;
} drvModemLogBufImp_t;

bool drvModemLogBufInitCp(drvModemLogBuf_t *p)
{
    drvModemLogBufImp_t *d = (drvModemLogBufImp_t *)p;
    const halShmemRegion_t *region = halShmemGetRegion(MEM_CP_DEBUG_NAME);
    if (region == NULL)
        return false;

    d->buf = (modemLogBuf_t *)region->address;
    d->is_zsp = false;
    return true;
}

bool drvModemLogBufInitZsp(drvModemLogBuf_t *p)
{
    drvModemLogBufImp_t *d = (drvModemLogBufImp_t *)p;
    const halShmemRegion_t *region = halShmemGetRegion(MEM_ZSP_DEBUG_NAME);
    if (region == NULL)
        return false;

    d->buf = (modemLogBuf_t *)region->address;
    d->is_zsp = true;
    return true;
}

bool drvModemLogBufInitMos(drvModemLogBuf_t *p)
{
    drvModemLogBufImp_t *d = (drvModemLogBufImp_t *)p;
    const halShmemRegion_t *region = halShmemGetRegion(MEM_MOS_DEBUG_NAME);
    if (region == NULL)
        return false;

    d->buf = (modemLogBuf_t *)region->address;
    d->is_zsp = false;
    return true;
}

osiBuffer_t drvModemLogLinearBuf(drvModemLogBuf_t *p, bool *is_tail)
{
    drvModemLogBufImp_t *d = (drvModemLogBufImp_t *)p;
    osiBuffer_t out = {.ptr = 0, .size = 0};

    modemLogBuf_t *buf = d->buf;
    if (buf == NULL)
        return out;
    if (buf->port != MODEM_PORT_IPC_USB &&
        buf->port != MODEM_PORT_IPC_UART &&
        buf->port != MODEM_PORT_IPC_FLASH)
        return out;
    if (buf->status != MODEM_LOG_BUFFER_OK)
        return out;

    unsigned head = d->is_zsp ? (buf->head * 2) : buf->head;
    unsigned buf_len = buf->length;
    unsigned rd_index = buf->rd_index;
    unsigned wr_index = buf->wr_index;
    unsigned overflow_index = buf->overflow_index;

    // sanity check
    if (!OSI_IS_POW2(buf_len) || buf_len > MODEM_MAX_BUF_LEN || buf_len < MODEM_MIN_BUF_LEN)
        return out;

    // 4 bytes alignment is required
    rd_index = OSI_ALIGN_DOWN(rd_index, 4);
    wr_index = OSI_ALIGN_DOWN(wr_index, 4);
    overflow_index = OSI_ALIGN_DOWN(overflow_index, 4);

    unsigned wsize1 = (wr_index < rd_index) ? buf->overflow_index - rd_index : 0;
    if (wsize1 > 0)
    {
        out.ptr = (uintptr_t)((char *)head + rd_index);
        out.size = wsize1;
        *is_tail = true;
        return out;
    }

    unsigned wsize2 = (wr_index > rd_index) ? wr_index - rd_index : 0;
    if (wsize2 > 0)
    {
        out.ptr = (uintptr_t)((char *)head + rd_index);
        out.size = wsize2;
        *is_tail = false;
        return out;
    }

    return out;
}

void drvModemLogBufAdvance(drvModemLogBuf_t *p, unsigned size)
{
    drvModemLogBufImp_t *d = (drvModemLogBufImp_t *)p;
    if (d->buf == NULL)
        return;

    unsigned buf_len = d->buf->length;
    unsigned rd_index = d->buf->rd_index;
    rd_index = OSI_ALIGN_DOWN(rd_index, 4);
    size = OSI_ALIGN_DOWN(size, 4);
    rd_index = (rd_index + size) & (buf_len - 1);
    d->buf->rd_index = rd_index;
}
