/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('B', 'T', 'L', 'G')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "osi_log.h"
#include "osi_api.h"
#include "hal_shmem_region.h"
#include "drv_md_ipc.h"
#include "srv_trace.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define BT_THREAD_PRIORITY (OSI_PRIORITY_LOW)
#define BT_THREAD_STACK_SIZE (2048)

#define BT_LOG_RETRY_INTERVAL (10)
#define BT_USB_TX_MAX_SIZE (256)

#define BT_RISCV_LOG_TAG (0x00)
#define BT_RISCV_LOG_FLOWID (0x70)

#define BT_LOG_IDX_TYPE 2 //index step:  0->1 byte;  2->4 bytes(Dword)

typedef volatile struct
{
    uint32_t wr_index;       // log buffer write offset
    uint32_t rd_index;       // log buffer read offset
    uint32_t buff_base_addr; // log buffer base address
    uint32_t buff_size;      // log buffer total length(Dword)

    uint32_t status_flag;

    // BT log set nv_set
    // 0: disable
    // 1: UART1
    // 2: from ARM COM_Debug
    // 3: from ARM + UART1(only for BT debug use)
    uint32_t nv_set;
} logBuf_t;

typedef struct
{
    logBuf_t *bt_buf;
    uint32_t buf_addr;
    osiWorkQueue_t *work_queue;
    osiWork_t *write_work;
    uint32_t notify_cnt;
} btLog_t;

static int prvTraceWriteNOS(btLog_t *bt)
{
    logBuf_t *buf = bt->bt_buf;
    uint32_t total_size;
    uint32_t r_index;
    uint32_t w_index;
    uint32_t w_data_size;
    // Ensure the read index is 4 bytes aligned
    total_size = buf->buff_size << BT_LOG_IDX_TYPE;
    // Read index must +1, then point to the first data to be read
    r_index = ((buf->rd_index + 1) << BT_LOG_IDX_TYPE) % total_size;
    w_index = buf->wr_index << BT_LOG_IDX_TYPE;
    w_data_size = (w_index + total_size - r_index) % total_size;

    if (r_index + w_data_size > total_size)
    {
        w_data_size = total_size - r_index;
    }

    OSI_LOGD(0, "bt log  r_index %d, w_index %d, w_data_size %d", r_index, w_index, w_data_size);

    uint32_t wsize_ = OSI_MIN(uint32_t, w_data_size, BT_USB_TX_MAX_SIZE - 1);
    if (wsize_ == 0)
        return 0;

    uint32_t wsize = OSI_ALIGN_DOWN(wsize_, 4);
    if (wsize != 0)
    {
        const char *p = (char *)(bt->buf_addr + r_index);
        osiTraceRawSend(BT_RISCV_LOG_FLOWID, BT_RISCV_LOG_TAG, p, wsize);
    }

    buf->rd_index = ((r_index + wsize + total_size - 4) % total_size) >> BT_LOG_IDX_TYPE;

    return wsize;
}

static void prvTraceWriteWork(void *param)
{
    btLog_t *bt = (btLog_t *)param;
    // Send all data of one channel, and then the other
    OSI_LOOP_WAIT(prvTraceWriteNOS(bt) == 0);
}

static void prvIpcNotifyCb(uint32_t value, void *param)
{
    btLog_t *bt = (btLog_t *)param;
    bt->notify_cnt++;

    if (bt->bt_buf == NULL)
    {
        bt->bt_buf = (logBuf_t *)(CONFIG_RAM_PHY_ADDRESS | (value & 0xFFFFFF));
        bt->buf_addr = (uint32_t)(CONFIG_RAM_PHY_ADDRESS | (bt->bt_buf->buff_base_addr & 0xFFFFFF));
        OSI_LOGD(0, "bt base addr:0x%x, buf base value 0x%x", bt->bt_buf, bt->buf_addr);
    }

    osiWorkEnqueue(bt->write_work, bt->work_queue);
}

bool srvBtTraceInit(void)
{
    btLog_t *bt = (btLog_t *)calloc(1, sizeof(btLog_t));
    if (bt == NULL)
        return false;

    bt->work_queue = osiWorkQueueCreate("bt-log", 1, BT_THREAD_PRIORITY,
                                        BT_THREAD_STACK_SIZE);
    if (bt->work_queue == NULL)
        goto fail;

    bt->write_work = osiWorkCreate(prvTraceWriteWork, NULL, bt);
    if (bt->write_work == NULL)
        goto fail;

    bt->bt_buf = NULL;
    ipc_register_bt_log_notify(prvIpcNotifyCb, bt);
    OSI_LOGI(0, "bt log service init %p", bt);
    return true;

fail:
    osiWorkDelete(bt->write_work);
    osiWorkQueueDelete(bt->work_queue);
    free(bt);
    return false;
}
