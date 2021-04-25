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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('U', 'S', 'R', 'L')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_debug_port.h"
#include "drv_debug_port_imp.h"
#include "drv_config.h"
#include "drv_usb.h"
#include "drv_names.h"
#include "usb/usb_serial.h"
#include "osi_trace.h"
#include "hal_shmem_region.h"
#include <osi_api.h>
#include <osi_log.h>
#include <osi_fifo.h>
#include <osi_compiler.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <malloc.h>
#include <errno.h>
#include <sys/queue.h>

/**
 * \brief usb serial for debugging
 *
 * There are several modes of userial debugging
 * - trace and cmd: either host protocol or diag protocol
 *   - rx will notify command engine through callback
 *   - tx will be mixed with trace
 *   - should poll at blue screen
 * - cmd only: either host protocol or diag protocol
 *   - rx will notify command engine through callback
 *   - tx will be sent directly
 *   - should poll at blue screen
 * - modem/mos trace: all inputs can be dropped
 *   - the difference between modem trace and mos trace is the shared memory
 *   - all rx data can be dropped
 *   - tx data is uncachable
 *   - should poll at blue screen
 *
 * The output of debugging usb serial should be self-flow-controlled. When
 * data is less than a threshold, the remaining data should be transfered
 * in next timer callback. Otherwise, the port will repeatedly transfer small
 * chunk of data, and affect the total usb performance.
 *
 * With the self-flow-control, the traces in the usb serial for trace can be
 * handled. However, it is needed to reduce the trace in trace port, and only
 * output trace for abnormal cases.
 *
 * \paragraph resource allocation
 *
 * The instances of \p drvDebugUserialPort_t are global variables.
 * - usb related resources ep/xfer are allocated at bind, and released
 *   at unbind.
 * - rx dma buffer and fifo memory are allocated at
 *   \p drvDebugUserialPortCreate, and never released. Before rx dma buffer
 *   is allocated, rx will use global receive-and-drop buffer.
 *
 * \paragraph low power and sleep
 *
 * It is a usb device, and use the usb device low power management.
 */

#define CDC_PTR_SANE(cdc) (cdc != NULL && cdc->func != NULL && cdc->func->controller != NULL)
#define CDC_PRIV_PTR_SANE(cdc) (cdc != NULL && cdc->priv != 0 && cdc->func != NULL && cdc->func->controller != NULL)

#define OUTPUT_AT_TXDONE (1)
#define OUTPUT_AT_TIMEOUT (2)
#define OUTPUT_AT_BLUE_SCREEN (3)

#define MODEM_USB_TX_MAX_SIZE (4096)
#define MODEM_PORT_AP_USB (5)
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
    drvDebugPort_t port;        // base debug port
    bool blue_screen_mode;      // whether in blue screen
    bool inited;                // whether initialized, just to avoid duplicated create
    bool trace_no_wait;         // not to wait at trace tx done
    osiMutex_t *lock;           // mutex for send packet
    osiTimer_t *tx_timer;       // timer to trigger tx again
    osiBuffer_t trace_buf;      // buffer fetch from trace
    unsigned rx_dma_size;       // rx dma size
    unsigned rx_fifo_size;      // rx fifo size
    void *rx_dma_buf;           // rx dma buffer, allocated at create
    osiFifo_t rx_fifo;          // rx fifo, initialized at create
    osiCallback_t rx_cb;        // rx callback
    void *rx_cb_ctx;            // rx callback context
    modemLogBuf_t *cp_logbuf;   // cp log buffer ctrl in shared memory
    modemLogBuf_t *zsp_logbuf;  // zsp log buffer ctrl in shared memory
    modemLogBuf_t *mos_logbuf;  // mos log buffer ctrl in shared memory
    modemLogBuf_t *xfer_logbuf; // cp/zsp/mos log buffer in transfer

    usbSerial_t *cdc;
    usbXfer_t *tx_xfer;
    usbXfer_t *rx_xfer;
    usbEp_t *tx_ep;
    usbEp_t *rx_ep;
} drvDebugUserialPort_t;

static void prvUserialRxStartLocked(drvDebugUserialPort_t *d);
static void prvUserialRxDoneCb(usbEp_t *ep, usbXfer_t *xfer);
static bool prvUserialStartTxLocked(drvDebugUserialPort_t *d, const void *data, unsigned size, bool uncached);
static void prvUserialTxDoneCb(usbEp_t *ep, usbXfer_t *xfer);
static void prvDummyRxCallback(void *param) {}

static drvDebugUserialPort_t gDuserials[] = {
    {.port.name = DRV_NAME_USRL_COM1},
    {.port.name = DRV_NAME_USRL_COM2},
    {.port.name = DRV_NAME_USRL_COM3},
    {.port.name = DRV_NAME_USRL_COM4},
};

/**
 * Find the predefined debugging usb serial by device name
 */
static drvDebugUserialPort_t *prvUserialFindByName(unsigned name)
{
    for (unsigned n = 0; n < OSI_ARRAY_SIZE(gDuserials); n++)
    {
        if (gDuserials[n].port.name == name)
            return &gDuserials[n];
    }
    return NULL;
}

/**
 * Start the tx timer
 */
static void prvUserialStartTxTimer(drvDebugUserialPort_t *d)
{
    osiTimerStartRelaxed(d->tx_timer, TRACE_TX_POLL_INTERVAL, OSI_DELAY_MAX);
}

/**
 * Start tx transfer.
 */
static bool prvUserialStartTxLocked(drvDebugUserialPort_t *d, const void *data, unsigned size, bool uncached)
{
    usbXfer_t *x = d->tx_xfer;
    x->buf = (void *)OSI_ALIGN_DOWN(data, 4); // though it should be aligned
    x->zlp = 1;
    x->length = size;
    x->param = d;
    x->status = 0; // though it will set to -EINPROGRESS at queue
    x->uncached = uncached;
    x->complete = prvUserialTxDoneCb;

    int ret = udcEpQueue(d->cdc->func->controller, d->tx_ep, x);
    if (!d->port.mode.trace_enable)
    {
        OSI_LOGD(0x100056c5, "CDC serial %4c tx start 0x%08x/%d return/%d",
                 d->port.name, data, size, ret);
    }

    if (ret < 0)
        return false;

    return true;
}

/**
 * Trace output in various modes
 */
static void prvUserialTraceOutputLocked(drvDebugUserialPort_t *d, unsigned whence)
{
    if (!d->port.mode.trace_enable || !d->port.status.usb_host_opened)
        return;

    if (whence == OUTPUT_AT_TXDONE)
    {
        if (!d->trace_no_wait && osiTraceDataSize() < TRACE_CONT_SIZE_MIN)
        {
            prvUserialStartTxTimer(d);
            return;
        }

        d->trace_buf = osiTraceBufFetch();
        if (d->trace_buf.size == 0)
        {
            prvUserialStartTxTimer(d);
            return;
        }

        prvUserialStartTxLocked(d, (void *)d->trace_buf.ptr, d->trace_buf.size, false);
        osiTimerStop(d->tx_timer);
    }
    else if (whence == OUTPUT_AT_BLUE_SCREEN)
    {
        if (d->trace_buf.size > 0) // on going
            return;

        d->trace_buf = osiTraceBufFetch();
        if (d->trace_buf.size == 0)
            return;

        prvUserialStartTxLocked(d, (void *)d->trace_buf.ptr, d->trace_buf.size, false);
    }
    else if (whence == OUTPUT_AT_TIMEOUT)
    {
        if (d->trace_buf.size > 0) // on going
            return;

        d->trace_buf = osiTraceBufFetch();
        if (d->trace_buf.size == 0)
        {
            prvUserialStartTxTimer(d);
            return;
        }

        prvUserialStartTxLocked(d, (void *)d->trace_buf.ptr, d->trace_buf.size, false);
    }
}

/**
 * Ouput cp/szp/mos trace, return the transfer size.
 */
static int prvUserialModemBufOutputLocked(drvDebugUserialPort_t *d, modemLogBuf_t *buf, bool check_size, bool is_zsp)
{
    if (buf == NULL || buf->port != MODEM_PORT_AP_USB || buf->status != MODEM_LOG_BUFFER_OK)
        return -1;

    unsigned head = is_zsp ? (buf->head * 2) : buf->head;
    unsigned buf_len = buf->length;
    unsigned rd_index = buf->rd_index;
    unsigned wr_index = buf->wr_index;
    unsigned overflow_index = buf->overflow_index;

    // sanity check
    if (!OSI_IS_POW2(buf_len) || buf_len > MODEM_MAX_BUF_LEN || buf_len < MODEM_MIN_BUF_LEN)
        return 0;

    // 4 bytes alignment is required for USB transfer
    rd_index = OSI_ALIGN_DOWN(rd_index, 4);
    wr_index = OSI_ALIGN_DOWN(wr_index, 4);
    overflow_index = OSI_ALIGN_DOWN(overflow_index, 4);

    unsigned wsize1 = (wr_index < rd_index) ? overflow_index - rd_index : 0;
    if (wsize1 > 0)
    {
        unsigned tsize = OSI_MIN(unsigned, wsize1, MODEM_USB_TX_MAX_SIZE);
        OSI_LOGD(0, "modem log (0x%x) write/%d rd/%d wr/%d",
                 buf, tsize, rd_index, wr_index);

        d->xfer_logbuf = buf;
        prvUserialStartTxLocked(d, (void *)head + rd_index, tsize, true);
        return tsize;
    }

    unsigned wsize2 = (wr_index > rd_index) ? wr_index - rd_index : 0;
    if (check_size && wsize2 < MODEM_USB_TX_MAX_SIZE)
        return 0;

    if (wsize2 > 0)
    {
        unsigned tsize = OSI_MIN(unsigned, wsize2, MODEM_USB_TX_MAX_SIZE);
        OSI_LOGD(0, "modem log (0x%x) write/%d rd/%d wr/%d",
                 buf, tsize, rd_index, wr_index);

        d->xfer_logbuf = buf;
        prvUserialStartTxLocked(d, (void *)head + rd_index, tsize, true);
        return tsize;
    }

    return 0;
}

/**
 * Output mos trace in various modes
 */
static void prvUserialMosLogOutputLocked(drvDebugUserialPort_t *d, unsigned whence)
{
    if (d->port.mode.protocol != DRV_DEBUG_PROTOCOL_MOS_LOG || !d->port.status.usb_host_opened)
        return;

    bool check_size = (whence == OUTPUT_AT_TXDONE);
    int tsize = prvUserialModemBufOutputLocked(d, d->mos_logbuf, check_size, false);
    if (tsize > 0)
        return;

    if (!d->blue_screen_mode)
        osiTimerStartRelaxed(d->tx_timer, TRACE_TX_POLL_INTERVAL, OSI_DELAY_MAX);
}

/**
 * Output modem trace in various modes
 */
static void prvUserialModemLogOutputLocked(drvDebugUserialPort_t *d, unsigned whence)
{
    if (d->port.mode.protocol != DRV_DEBUG_PROTOCOL_MODEM_LOG || !d->port.status.usb_host_opened)
        return;

    bool check_size = (whence == OUTPUT_AT_TXDONE);
    int tsize = prvUserialModemBufOutputLocked(d, d->cp_logbuf, check_size, false);
    if (tsize > 0)
        return;

    tsize = prvUserialModemBufOutputLocked(d, d->zsp_logbuf, check_size, true);
    if (tsize > 0)
        return;

    if (!d->blue_screen_mode)
        osiTimerStartRelaxed(d->tx_timer, TRACE_TX_POLL_INTERVAL, OSI_DELAY_MAX);
}

/**
 * TX done callback, may be called in usb isr, or blue screen poll
 */
static void prvUserialTxDoneCb(usbEp_t *ep, usbXfer_t *xfer)
{
    drvDebugUserialPort_t *d = (drvDebugUserialPort_t *)xfer->param;

    if (!d->port.mode.trace_enable)
    {
        if (xfer->status != 0)
        {
            OSI_LOGW(0x100056c7, "CDC serial %4c tx fail, status (%d)",
                     d->port.name, xfer->status);
        }
        else
        {
            OSI_LOGD(0x100056c8, "CDC serial %4c tx done, status (%d)",
                     d->port.name, xfer->status);
        }
    }

    if (d->port.mode.trace_enable)
    {
        if (d->trace_buf.size > 0)
            osiTraceBufHandled();

        d->trace_buf.ptr = 0;
        d->trace_buf.size = 0;
        prvUserialTraceOutputLocked(d, OUTPUT_AT_TXDONE);
    }
    else if (d->port.mode.protocol == DRV_DEBUG_PROTOCOL_MODEM_LOG)
    {
        // update rd_index for peer
        if (d->xfer_logbuf != NULL)
        {
            unsigned buf_len = d->xfer_logbuf->length;
            unsigned rd_index = d->xfer_logbuf->rd_index;
            rd_index = (rd_index + xfer->actual) & (buf_len - 1);
            d->xfer_logbuf->rd_index = rd_index;
            d->xfer_logbuf = NULL;
        }
        prvUserialModemLogOutputLocked(d, OUTPUT_AT_TXDONE);
    }
    else if (d->port.mode.protocol == DRV_DEBUG_PROTOCOL_MOS_LOG)
    {
        // update rd_index for peer
        if (d->xfer_logbuf != NULL)
        {
            unsigned buf_len = d->xfer_logbuf->length;
            unsigned rd_index = d->xfer_logbuf->rd_index;
            rd_index = (rd_index + xfer->actual) & (buf_len - 1);
            d->xfer_logbuf->rd_index = rd_index;
            d->xfer_logbuf = NULL;
        }
        prvUserialMosLogOutputLocked(d, OUTPUT_AT_TXDONE);
    }
}

/**
 * Debug usb serial output, called in timer isr or manual trigger
 */
static void prvUserialOutputTimeout(void *param)
{
    drvDebugUserialPort_t *d = (drvDebugUserialPort_t *)param;
    prvUserialTraceOutputLocked(d, OUTPUT_AT_TIMEOUT);
    prvUserialModemLogOutputLocked(d, OUTPUT_AT_TIMEOUT);
    prvUserialMosLogOutputLocked(d, OUTPUT_AT_TIMEOUT);
}

/**
 * RX done callback, may be called in usb isr, or blue screen poll
 */
static void prvUserialRxDoneCb(usbEp_t *ep, usbXfer_t *xfer)
{
    drvDebugUserialPort_t *d = (drvDebugUserialPort_t *)xfer->param;
    if (xfer->status != 0)
    {
        OSI_LOGW(0x100056c3, "CDC serial %4c rx fail, status/%d, size/%d",
                 d->port.name, xfer->status, xfer->actual);
    }
    else
    {
        OSI_LOGD(0x100056c4, "CDC serial %4c rx done, status/%d, size/%d",
                 d->port.name, xfer->status, xfer->actual);

        // In case host doesn't send UCDC_SET_CONTROL_LINE_STATE,
        // "open" should be set forcedly to enabled write.
        if (!d->port.status.usb_host_opened)
        {
            d->port.status.usb_host_opened = true;
            prvUserialOutputTimeout(d); // fake timeout to trigger output
        }

        if (d->rx_dma_buf != NULL)
        {
            // cache invalidate is called before callback
            osiFifoPut(&d->rx_fifo, xfer->buf, xfer->actual);
            d->rx_cb(d->rx_cb_ctx);
        }

        prvUserialRxStartLocked(d);
    }
}

/**
 * Start RX, called in rx done callback, or device enable
 */
static void prvUserialRxStartLocked(drvDebugUserialPort_t *d)
{
    if (!d->port.status.usb_enabled)
        return;

    usbXfer_t *x = d->rx_xfer;
    if (d->rx_dma_buf == NULL)
    {
        x->buf = gDummyUsbRxBuf;
        x->length = USB_DUMMY_RX_BUF_SIZE;
        x->uncached = true;
    }
    else
    {
        x->buf = d->rx_dma_buf;
        x->length = d->rx_dma_size;
        x->uncached = false;
    }
    x->param = d;
    x->complete = prvUserialRxDoneCb;

    int ret = udcEpQueue(d->cdc->func->controller, d->rx_ep, x);
    OSI_LOGD(0x100056c1, "CDC serial %4c rx start 0x%08x/%d return/%d",
             d->port.name, x->buf, x->length, ret);

    if (ret < 0)
    {
        OSI_LOGE(0x10005652, "CDC serial %4c rx start failed. enqueue xfer return (%d)",
                 d->port.name, ret);

        // failure except disconnected are not permitted
        if (ret != -ENOENT)
            osiPanic();
    }
}

/**
 * Start debug usb serial, return 0 on success.
 */
static int prvUserialStartLocked(drvDebugUserialPort_t *d)
{
    udc_t *udc = d->cdc->func->controller;
    int result;

    if ((result = udcEpEnable(udc, d->tx_ep)) < 0)
        return result;

    if ((result = udcEpEnable(udc, d->rx_ep)) < 0)
    {
        udcEpDisable(udc, d->tx_ep);
        return result;
    }
    return 0;
}

/**
 * Stop debug usb serial.
 */
static void prvUserialStopLocked(drvDebugUserialPort_t *d)
{
    udc_t *udc = d->cdc->func->controller;
    if (d->rx_ep)
    {
        udcEpDequeueAll(udc, d->rx_ep);
        udcEpDisable(udc, d->rx_ep);
    }
    if (d->tx_ep)
    {
        udcEpDequeueAll(udc, d->tx_ep);
        udcEpDisable(udc, d->tx_ep);
    }
}

/**
 * Send packet by trace, and wait finish
 */
static bool prvUserialSendPacketByTrace(drvDebugUserialPort_t *d, const void *data, unsigned size)
{
    if (!osiTraceBufPutPacket(data, size))
        return false;

    unsigned critical = osiEnterCritical();
    prvUserialTraceOutputLocked(d, d->blue_screen_mode ? OUTPUT_AT_BLUE_SCREEN : OUTPUT_AT_TIMEOUT);
    osiExitCritical(critical);

    osiElapsedTimer_t timeout;
    osiElapsedTimerStart(&timeout);

    while (osiElapsedTimeUS(&timeout) < SEND_PACKET_BY_TRACE_TIMEOUT * 1000)
    {
        if (osiTraceBufPacketFinished(data))
            return true;

        if (d->blue_screen_mode)
            udcGdbPollIntr(d->cdc->func->controller);
        else
            osiThreadSleepUS(2000);
    }

    return false;
}

/**
 * Wait direct tx transfer finish
 */
static bool prvUserialWaitDirectTxFinish(drvDebugUserialPort_t *d)
{
    return OSI_LOOP_WAIT_POST_TIMEOUT_US(d->tx_xfer->status != -EINPROGRESS,
                                         SEND_PACKET_DIRECT_TIMEOUT * 1000,
                                         osiThreadSleepUS(2000));
}

/**
 * Wait direct tx transfer finish in blue screen mode
 */
static bool prvUserialWaitDirectTxFinishBs(drvDebugUserialPort_t *d)
{
    return OSI_LOOP_WAIT_POST_TIMEOUT_US(d->tx_xfer->status != -EINPROGRESS,
                                         SEND_PACKET_DIRECT_TIMEOUT * 1000,
                                         udcGdbPollIntr(d->cdc->func->controller));
}

/**
 * Send packet through debug usb serial.
 */
static bool prvUserialSendPacket(drvDebugPort_t *p, const void *data, unsigned size)
{
    drvDebugUserialPort_t *d = OSI_CONTAINER_OF(p, drvDebugUserialPort_t, port);

    if (!d->port.mode.cmd_enable || !d->port.status.usb_host_opened)
        return false;

    if (d->blue_screen_mode)
    {
        return prvUserialWaitDirectTxFinishBs(d) &&
               prvUserialStartTxLocked(d, data, size, false) &&
               prvUserialWaitDirectTxFinishBs(d);
    }

    osiMutexLock(d->lock);

    bool ok = false;
    if (d->port.mode.trace_enable)
    {
        d->trace_no_wait = true;
        ok = prvUserialSendPacketByTrace(d, data, size);
        d->trace_no_wait = false;
    }
    else
    {
        ok = prvUserialWaitDirectTxFinish(d) &&
             prvUserialStartTxLocked(d, data, size, false) &&
             prvUserialWaitDirectTxFinish(d);
    }

    osiMutexUnlock(d->lock);
    return ok;
}

/**
 * Read from debug usb serial, return read bytes
 */
static int prvUserialRead(drvDebugPort_t *p, void *data, unsigned size)
{
    drvDebugUserialPort_t *d = OSI_CONTAINER_OF(p, drvDebugUserialPort_t, port);
    return (d->rx_dma_buf == NULL) ? 0 : osiFifoGet(&d->rx_fifo, data, size);
}

/**
 * Set trace enable or disable
 */
static void prvUserialSetTraceEnable(drvDebugPort_t *p, bool enable)
{
    if (enable)
    {
        drvDebugUserialPort_t *d = OSI_CONTAINER_OF(p, drvDebugUserialPort_t, port);
        unsigned critical = osiEnterCritical();
        prvUserialStartTxTimer(d);
        osiExitCritical(critical);
    }
}

/**
 * Set rx callback
 */
static void prvUserialSetRxCallback(drvDebugPort_t *p, osiCallback_t cb, void *param)
{
    drvDebugUserialPort_t *d = OSI_CONTAINER_OF(p, drvDebugUserialPort_t, port);
    d->rx_cb_ctx = param;
    d->rx_cb = (cb == NULL) ? prvDummyRxCallback : cb;
}

static const drvDebugPortOps_t gDuserialPortOps = {
    .send_packet = prvUserialSendPacket,
    .read = prvUserialRead,
    .set_trace_enable = prvUserialSetTraceEnable,
    .set_rx_callback = prvUserialSetRxCallback,
};

/**
 * Enter blue screen mode
 */
static void prvUserialBsEnter(void *param)
{
    drvDebugUserialPort_t *d = (drvDebugUserialPort_t *)param;
    d->blue_screen_mode = true;
    d->rx_cb = prvDummyRxCallback;

    if (d->port.mode.trace_enable)
    {
#ifdef CONFIG_KERNEL_HOST_TRACE
        // It will make USB easier to copy the data to RAM (stack).
        uint8_t event_data[GDB_EVENT_DATA_SIZE];
        memcpy(event_data, gBlueScreenEventData, GDB_EVENT_DATA_SIZE);

        prvUserialSendPacket(&d->port, event_data, GDB_EVENT_DATA_SIZE);
#endif
    }
}

/**
 * Enter blue screen mode
 */
static void prvUserialBsPoll(void *param)
{
    drvDebugUserialPort_t *d = (drvDebugUserialPort_t *)param;

    prvUserialTraceOutputLocked(d, OUTPUT_AT_BLUE_SCREEN);
    prvUserialModemLogOutputLocked(d, OUTPUT_AT_BLUE_SCREEN);
    prvUserialMosLogOutputLocked(d, OUTPUT_AT_BLUE_SCREEN);
}

/**
 * Release udc related resources
 */
static void prvUserialFreeUdcResources(drvDebugUserialPort_t *d, udc_t *controller)
{
    // NULL pointers can be handled
    udcXferFree(controller, d->tx_xfer);
    udcXferFree(controller, d->rx_xfer);
    udcEpFree(controller, d->tx_ep);
    udcEpFree(controller, d->rx_ep);

    d->tx_xfer = NULL;
    d->rx_xfer = NULL;
    d->tx_ep = NULL;
    d->rx_ep = NULL;
}

/**
 * USB serial bind
 */
static int prvUserialBind(uint32_t name, usbSerial_t *cdc)
{
    drvDebugUserialPort_t *d = prvUserialFindByName(name);
    if (d == NULL)
        return -1;

    if (!CDC_PTR_SANE(cdc) || cdc->epin_desc == NULL || cdc->epout_desc == NULL)
        return -1;

    OSI_LOGI(0x10005657, "CDC serial %4c bind", name);

    unsigned critical = osiEnterCritical();
    d->tx_xfer = NULL;
    d->rx_xfer = NULL;
    d->tx_ep = NULL;
    d->rx_ep = NULL;

    udc_t *controller = cdc->func->controller;
    d->cdc = cdc;
    d->tx_ep = udcEpAlloc(controller, cdc->epin_desc);
    if (d->tx_ep == NULL)
        goto failed;
    d->rx_ep = udcEpAlloc(controller, cdc->epout_desc);
    if (d->rx_ep == NULL)
        goto failed;
    d->tx_xfer = udcXferAlloc(controller);
    if (d->tx_xfer == NULL)
        goto failed;
    d->rx_xfer = udcXferAlloc(controller);
    if (d->rx_xfer == NULL)
        goto failed;

    cdc->epin_desc->bEndpointAddress = d->tx_ep->address;
    cdc->epout_desc->bEndpointAddress = d->rx_ep->address;
    cdc->priv = (unsigned long)d;
    osiExitCritical(critical);
    return 0;

failed:
    prvUserialFreeUdcResources(d, controller);
    osiExitCritical(critical);
    return -1;
}

/**
 * USB serial unbind
 */
static void prvUserialUnbind(usbSerial_t *cdc)
{
    if (!CDC_PRIV_PTR_SANE(cdc))
        return;

    drvDebugUserialPort_t *d = (drvDebugUserialPort_t *)cdc->priv;
    OSI_LOGI(0x10005658, "CDC serial %4c unbind", d->port.name);

    unsigned critical = osiEnterCritical();
    prvUserialFreeUdcResources(d, cdc->func->controller);
    cdc->priv = (unsigned long)NULL;
    osiExitCritical(critical);
}

/**
 * USB serial enable
 */
static int prvUserialEnable(usbSerial_t *cdc)
{
    if (!CDC_PRIV_PTR_SANE(cdc))
        return -EINVAL;

    drvDebugUserialPort_t *d = (drvDebugUserialPort_t *)cdc->priv;
    OSI_LOGI(0x10005659, "CDC serial %4c enable", d->port.name);

    uint32_t critical = osiEnterCritical();
    d->port.status.usb_host_opened = false;
    d->port.status.usb_enabled = (prvUserialStartLocked(d) == 0);
    prvUserialRxStartLocked(d);
    osiExitCritical(critical);
    return 0;
}

/**
 * USB serial disable
 */
static void prvUserialDisable(usbSerial_t *cdc)
{
    if (!CDC_PRIV_PTR_SANE(cdc))
        return;

    drvDebugUserialPort_t *d = (drvDebugUserialPort_t *)cdc->priv;
    OSI_LOGI(0x1000565a, "CDC serial %4c disable", d->port.name);

    uint32_t critical = osiEnterCritical();
    d->port.status.usb_enabled = false;
    d->port.status.usb_host_opened = false;
    prvUserialStopLocked(d);
    osiExitCritical(critical);
}

/**
 * USB serial (peer) open
 */
static bool prvUserialOpen(usbSerial_t *cdc)
{
    if (!CDC_PRIV_PTR_SANE(cdc))
        return false;

    drvDebugUserialPort_t *d = (drvDebugUserialPort_t *)cdc->priv;
    OSI_LOGI(0, "debug usb serial %4c open", d->port.name);

    uint32_t critical = osiEnterCritical();
    if (!d->port.status.usb_host_opened)
    {
        d->port.status.usb_host_opened = true;
        prvUserialStartTxTimer(d);
    }
    osiExitCritical(critical);
    return true;
}

/**
 * USB serial (peer) close
 */
static void prvUserialClose(usbSerial_t *cdc)
{
    if (!CDC_PRIV_PTR_SANE(cdc))
        return;

    drvDebugUserialPort_t *d = (drvDebugUserialPort_t *)cdc->priv;
    OSI_LOGI(0, "debug usb serial %4c close", d->port.name);

    uint32_t critical = osiEnterCritical();
    d->port.status.usb_host_opened = false;
    osiExitCritical(critical);
}

static const usbSerialDriverOps_t gDuserialUsbOps = {
    .bind = prvUserialBind,
    .unbind = prvUserialUnbind,
    .enable = prvUserialEnable,
    .disable = prvUserialDisable,
    .open = prvUserialOpen,
    .close = prvUserialClose,
};

const usbSerialDriverOps_t *usbDebugSerialGetOps(void)
{
    return &gDuserialUsbOps;
}

/**
 * Create debug usb serial. It may be call either the usb device is ready or not.
 */
drvDebugPort_t *drvDebugUserialPortCreate(unsigned name, drvDebugPortMode_t mode)
{
    drvDebugUserialPort_t *d = prvUserialFindByName(name);
    if (d == NULL || d->inited)
        return NULL;

    unsigned critical = osiEnterCritical();
    d->port.ops = &gDuserialPortOps;
    d->port.name = name;
    d->port.mode = mode;
    d->blue_screen_mode = false;
    d->rx_cb = prvDummyRxCallback;
    d->lock = osiMutexCreate();

    if (d->port.mode.cmd_enable && d->port.mode.protocol == DRV_DEBUG_PROTOCOL_HOST)
    {
        d->rx_dma_size = 512;
        d->rx_fifo_size = CONFIG_HOST_CMD_ENGINE_MAX_PACKET_SIZE;
    }
    else if (d->port.mode.cmd_enable && d->port.mode.protocol == DRV_DEBUG_PROTOCOL_DIAG)
    {
        d->rx_dma_size = 2048;
        d->rx_fifo_size = 64 * 1024;
    }

    if (d->rx_dma_size + d->rx_fifo_size > 0)
    {
        d->rx_dma_buf = (void *)memalign(CONFIG_CACHE_LINE_SIZE, d->rx_dma_size + d->rx_fifo_size);
        void *rx_fifo_buf = (char *)d->rx_dma_buf + d->rx_dma_size;
        osiFifoInit(&d->rx_fifo, rx_fifo_buf, d->rx_fifo_size);
    }

    if (d->port.mode.protocol == DRV_DEBUG_PROTOCOL_MODEM_LOG)
    {
        const halShmemRegion_t *region_cp = halShmemGetRegion(MEM_CP_DEBUG_NAME);
        const halShmemRegion_t *region_zsp = halShmemGetRegion(MEM_ZSP_DEBUG_NAME);
        if (region_cp != NULL)
            d->cp_logbuf = (modemLogBuf_t *)region_cp->address;
        if (region_zsp != NULL)
            d->zsp_logbuf = (modemLogBuf_t *)region_zsp->address;
    }

    if (d->port.mode.protocol == DRV_DEBUG_PROTOCOL_MOS_LOG)
    {
        const halShmemRegion_t *region = halShmemGetRegion(MEM_MOS_DEBUG_NAME);
        if (region != NULL)
            d->mos_logbuf = (modemLogBuf_t *)region->address;
    }

    if (d->port.mode.trace_enable ||
        d->port.mode.protocol == DRV_DEBUG_PROTOCOL_MODEM_LOG ||
        d->port.mode.protocol == DRV_DEBUG_PROTOCOL_MOS_LOG)
        d->tx_timer = osiTimerCreate(NULL, prvUserialOutputTimeout, d);

    if (d->port.mode.trace_enable)
        osiTraceSetDebugPort(&d->port);

    d->inited = true;

    if (d->port.mode.bs_enable)
        osiRegisterBlueScreenHandler(prvUserialBsEnter, prvUserialBsPoll, d);

    osiExitCritical(critical);

    prvUserialStartTxTimer(d);
    drvDebugPortRegister(&d->port);
    return &d->port;
}
