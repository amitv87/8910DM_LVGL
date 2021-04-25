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

#include "drv_serial_imp.h"
#include "usb_serial.h"
#include "drv_usb.h"
#include "drv_names.h"
#include "osi_blocked_fifo.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <malloc.h>
#include <errno.h>
#include <osi_api.h>
#include <osi_log.h>
#include <osi_fifo.h>
#include <osi_compiler.h>
#include <sys/queue.h>
#include "quec_proj_config.h"

// NOTE: when USB trace is enabled, prvWriteDirect will be called to
// output trace data. It is needed to avoid trace during output.

#define TX_TIMEOUT_THRESHOLD (1000) // ms

typedef struct
{
    drvSerialImpl_t port;
    usbSerial_t *cdc;
    usbXfer_t *tx_xfer;
    usbXfer_t *rx_xfer;
    usbEp_t *tx_ep;
    usbEp_t *rx_ep;
    osiWorkQueue_t *work_queue;

    osiWork_t *work_rx;           ///< work for RX done
    osiSemaphore_t *rx_avail_sem; ///< sema to notify fifo avail
    osiBlockedFifo_t *rx_fifo;    ///< fifo for RX

    osiWork_t *work_tx;            ///< work for TX done
    osiSemaphore_t *tx_avail_sem;  ///< sema to notify fifo avail
    osiSemaphore_t *tx_finish_sem; ///< sema to nitify all done
    uint64_t tx_timestamp;         ///< time stamp for the last TX transfer
    const void *tx_buf_in_trans;   ///< buffer from TX fifo in transfer
    osiBlockedFifo_t *tx_fifo;     ///< blocked fifo for TX, it can be NULL
    uint32_t pending_event;        ///< not notified event
    void *allocated;               ///< dynamic allocated buffer on open, release on close
    bool inited;                   ///< a caller open the port
    bool ready;                    ///< the usb serial is ready to work (enum as serial)
    bool open;                     ///< open by the other peer
    bool async_later_free;         ///< free async resource later
    uint8_t async_res_ref;         ///< async resource ref count
} usbSerialPriv_t;

static inline usbSerialPriv_t *SERIAL_PRIV(usbSerial_t *serial) { return (usbSerialPriv_t *)serial->priv; }
static inline bool SERIAL_RUNNING(usbSerialPriv_t *priv) { return priv->inited && priv->ready; }
static inline usbSerialPriv_t *PORT_PRIV(drvSerialImpl_t *port) { return (usbSerialPriv_t *)port; }
static inline uint32_t PORT_NAME(drvSerialImpl_t *port) { return port->info ? port->info->name : 0x30303030; }
static inline const drvSerialCfg_t *PORT_CFG(drvSerialImpl_t *port) { return &port->info->cfg; } // caller make sure port info non-null
static bool prvIsTxFinishedLocked(usbSerialPriv_t *priv);

static inline void prvSemaTryAcquireLocked(usbSerialPriv_t *priv,
                                           osiSemaphore_t *sema,
                                           uint32_t timeout)
{
    priv->async_res_ref += 1;
    osiSemaphoreTryAcquire(sema, timeout);
    priv->async_res_ref -= 1;
}

static void prvNotifyPendingEvent(usbSerialPriv_t *priv)
{
    uint32_t critical = osiEnterCritical();
    uint32_t event = priv->pending_event;
    priv->pending_event = 0;
    osiExitCritical(critical);

    drvSerialImpl_t *port = &priv->port;
    if (port->info == NULL)
        return;

    const drvSerialCfg_t *cfg = PORT_CFG(port);
    event &= cfg->event_mask;
    if (event != 0 && cfg->event_cb != NULL)
    {
        cfg->event_cb(event, cfg->param);
    }
}

static void prvRxDoneCb(usbEp_t *ep, usbXfer_t *xfer)
{
    usbSerialPriv_t *priv = (usbSerialPriv_t *)xfer->param;
    drvSerialImpl_t *port = &priv->port;
    if (xfer->status != 0)
    {
        OSI_LOGW(0x100056c3, "CDC serial %4c rx fail, status/%d, size/%d",
                 PORT_NAME(port), xfer->status, xfer->actual);
        if (xfer->status == -ECANCELED)
            return;
    }
    else
    {
        OSI_LOGD(0x100056c4, "CDC serial %4c rx done, status/%d, size/%d",
                 PORT_NAME(port), xfer->status, xfer->actual);
        priv->pending_event |= DRV_SERIAL_EVENT_RX_ARRIVED;
        if (priv->rx_fifo != NULL)
            osiBlockedFifoPutDone(priv->rx_fifo, xfer->actual);

        // In case host doesn't send UCDC_SET_CONTROL_LINE_STATE,
        // "open" should be set forcedly to enabled write.
        priv->open = true;

        // In case someone is waiting rx avail
        osiSemaphoreRelease(priv->rx_avail_sem);
    }

    osiWorkEnqueue(priv->work_rx, priv->work_queue);
}

static void prvRxStartLocked(usbSerialPriv_t *priv)
{
    drvSerialImpl_t *port = &priv->port;
    void *data = osiBlockedFifoPutRequest(priv->rx_fifo, true);
    if (data == NULL)
    {
        OSI_LOGE(0, "serial %4c rx no enough space", PORT_NAME(port));
        return;
    }

    usbXfer_t *x = priv->rx_xfer;
    x->buf = data;
    x->length = PORT_CFG(port)->rx_dma_size;
    x->param = priv;
    x->complete = prvRxDoneCb;

    int ret = udcEpQueue(priv->cdc->func->controller, priv->rx_ep, x);
    OSI_LOGD(0x100056c1, "CDC serial %4c rx start 0x%08x/%d return/%d",
             PORT_NAME(port), data, x->length, ret);

    if (ret < 0)
    {
        OSI_LOGE(0x10005652, "CDC serial %4c rx start failed. enqueue xfer return (%d)",
                 PORT_NAME(port), ret);

        // failure except disconnected are not permitted
        if (ret != -ENOENT)
            osiPanic();
    }
}

static void prvRxDoneWork(void *param)
{
    uint32_t critical = osiEnterCritical();
    usbSerialPriv_t *priv = (usbSerialPriv_t *)param;
    if (SERIAL_RUNNING(priv))
        prvRxStartLocked(priv);
    osiExitCritical(critical);
    prvNotifyPendingEvent(priv);
}

static void prvTxDoneCb(usbEp_t *ep, usbXfer_t *xfer)
{
    usbSerialPriv_t *priv = (usbSerialPriv_t *)xfer->param;
    drvSerialImpl_t *port = &priv->port;
    if (xfer->status != 0)
    {
        OSI_LOGW(0x100056c7, "CDC serial %4c tx fail, status (%d)",
                 PORT_NAME(port), xfer->status);
    }
    else
    {
        OSI_LOGD(0x100056c8, "CDC serial %4c tx done, status (%d)",
                 PORT_NAME(port), xfer->status);
    }

    uint32_t critical = osiEnterCritical();
#ifdef CONFIG_QUEC_PROJECT_FEATURE	
	priv->pending_event |= DRV_SERIAL_EVENT_TX_COMPLETE;
#endif
    if (priv->tx_fifo != NULL)
    {
        if (xfer->buf == priv->tx_buf_in_trans)
        {
            osiBlockedFifoGetBlockDone(priv->tx_fifo);
            priv->tx_buf_in_trans = NULL;

            osiSemaphoreRelease(priv->tx_avail_sem);
        }
    }

    if (priv->tx_fifo && xfer->status != -ECANCELED)
    {
        osiWorkEnqueue(priv->work_tx, priv->work_queue);
    }
    else
    {
        if (prvIsTxFinishedLocked(priv))
            osiSemaphoreRelease(priv->tx_finish_sem);
    }
    osiExitCritical(critical);
}

static void prvTxTransBlockLocked_(usbSerialPriv_t *priv, const void *data, unsigned size, bool cached)
{
    drvSerialImpl_t *port = &priv->port;
    usbXfer_t *x = priv->tx_xfer;
    x->buf = (void *)data;
    x->zlp = 1;
    x->length = size;
    x->param = priv;
    x->status = 0; // though it will set to -EINPROGRESS at queue
    x->uncached = cached ? 0 : 1;
    x->complete = prvTxDoneCb;
    priv->tx_timestamp = osiUpTime();

    int ret = udcEpQueue(priv->cdc->func->controller, priv->tx_ep, x);
    OSI_LOGD(0x100056c5, "CDC serial %4c tx start 0x%08x/%d return/%d",
             PORT_NAME(port), data, x->length, ret);

    if (ret < 0)
    {
        OSI_LOGE(0x10005656, "CDC serial %4c tx start failed. size (%u), enqueue return (%d)",
                 PORT_NAME(port), size, ret);

        // failure except disconnected are not permitted
        if (ret != -ENOENT)
            osiPanic();
    }
}

static void prvTxTransBlockLocked(usbSerialPriv_t *priv, const void *data, unsigned size)
{
    return prvTxTransBlockLocked_(priv, data, size, true);
}

static bool prvTxPollLocked(usbSerialPriv_t *priv)
{
    usbXfer_t *x = priv->tx_xfer;
    if (x->status == -EINPROGRESS) // only one xfer for tx ep
        return false;

    if (priv->tx_fifo != NULL)
    {
        unsigned size = 0;
        const void *data = osiBlockedFifoGetRequest(priv->tx_fifo, &size);
        if (data != NULL && size > 0)
        {
            priv->tx_buf_in_trans = data;
            prvTxTransBlockLocked(priv, data, size);
            return true;
        }
    }

    return false;
}

static bool prvIsTxFinishedLocked(usbSerialPriv_t *priv)
{
    usbXfer_t *xfer = priv->tx_xfer;
    if (xfer->status == -EINPROGRESS)
        return false;

    if (priv->tx_fifo != NULL && !osiBlockedFifoIsEmpty(priv->tx_fifo))
        return false;

    return true;
}

static void prvTxDoneWork(void *priv_)
{
    uint32_t critical = osiEnterCritical();
    usbSerialPriv_t *priv = (usbSerialPriv_t *)priv_;
    if (priv->tx_fifo != NULL)
    {
        // try to send more
        unsigned size = 0;
        const void *data = osiBlockedFifoGetRequest(priv->tx_fifo, &size);
        if (data != NULL && size > 0)
        {
            priv->tx_buf_in_trans = data;
            prvTxTransBlockLocked(priv, data, size);
        }
    }

    if (prvIsTxFinishedLocked(priv))
        osiSemaphoreRelease(priv->tx_finish_sem);

    osiExitCritical(critical);
#ifdef CONFIG_QUEC_PROJECT_FEATURE	
	prvNotifyPendingEvent(priv);
#endif
}

static void prvAsyncResFreeLocked(usbSerialPriv_t *priv)
{
    osiSemaphoreDelete(priv->tx_avail_sem);
    osiSemaphoreDelete(priv->tx_finish_sem);
    osiSemaphoreDelete(priv->rx_avail_sem);
    free(priv);
}

/**
 * \true for resource deleted, stop any thing
 * \false resource not delete, continue going
 */
static inline bool prvAsyncResCheckFreeLocked(usbSerialPriv_t *priv)
{
    if (priv->async_later_free)
    {
        if (priv->async_res_ref == 0)
            prvAsyncResFreeLocked(priv);
        return true;
    }
    return false;
}

static bool prvWaitWriteFinishLocked(usbSerialPriv_t *priv, unsigned timeout)
{
    if (!SERIAL_RUNNING(priv) || !priv->open || priv->async_later_free)
        return false;

    prvSemaTryAcquireLocked(priv, priv->tx_finish_sem, 0);
    if (prvIsTxFinishedLocked(priv))
        return true;

    if (osiUpTime() - priv->tx_timestamp > TX_TIMEOUT_THRESHOLD)
        return false;

    prvSemaTryAcquireLocked(priv, priv->tx_finish_sem, timeout);
    if (prvAsyncResCheckFreeLocked(priv))
        return false;

    if (!SERIAL_RUNNING(priv) || !priv->open)
        return false;

    return prvIsTxFinishedLocked(priv);
}

static bool prvWaitWriteAvailLocked(usbSerialPriv_t *priv, unsigned timeout)
{
    if (!SERIAL_RUNNING(priv) || !priv->tx_fifo ||
        !priv->open || priv->async_later_free)
        return false;

    prvSemaTryAcquireLocked(priv, priv->tx_avail_sem, 0);
    if (!osiBlockedFifoIsFull(priv->tx_fifo))
        return true;

    if (osiUpTime() - priv->tx_timestamp > TX_TIMEOUT_THRESHOLD)
        return false;

    prvSemaTryAcquireLocked(priv, priv->tx_avail_sem, timeout);
    if (prvAsyncResCheckFreeLocked(priv))
        return false;

    if (!SERIAL_RUNNING(priv) || !priv->tx_fifo || !priv->open)
        return false;

    return !osiBlockedFifoIsFull(priv->tx_fifo);
}

static bool prvWaitReadAvailLocked(usbSerialPriv_t *priv, unsigned timeout)
{
    if (!SERIAL_RUNNING(priv) || !priv->rx_fifo || priv->async_later_free)
        return false;

    prvSemaTryAcquireLocked(priv, priv->rx_avail_sem, 0);
    if (!osiBlockedFifoIsEmpty(priv->rx_fifo))
        return true;

    prvSemaTryAcquireLocked(priv, priv->rx_avail_sem, timeout);
    if (prvAsyncResCheckFreeLocked(priv))
        return false;

    if (!SERIAL_RUNNING(priv) || !priv->rx_fifo)
        return false;

    return !osiBlockedFifoIsEmpty(priv->rx_fifo);
}

static int prvSerialStartLocked(usbSerialPriv_t *priv)
{
    if (priv->rx_fifo)
        osiBlockedFifoReset(priv->rx_fifo);
    if (priv->tx_fifo)
        osiBlockedFifoReset(priv->tx_fifo);
    priv->tx_buf_in_trans = NULL;
    OSI_ASSERT(priv->rx_fifo != NULL, "usb serial open rxfifo null");
    prvRxStartLocked(priv);

    return 0;
}

static void prvSerialStopLocked(usbSerialPriv_t *priv)
{
    udc_t *udc = priv->cdc->func->controller;
    udcEpDequeue(udc, priv->rx_ep, priv->rx_xfer);
    udcEpDequeue(udc, priv->tx_ep, priv->tx_xfer);
}

static int prvOpen(drvSerialImpl_t *port)
{
    usbSerialPriv_t *prv = PORT_PRIV(port);
    if (prv->inited)
        return true;

    const drvSerialCfg_t *kcfg = PORT_CFG(port);
    OSI_LOGD(0, "usb serial %4c open. %u/%u/%u/%u/%p", PORT_NAME(port),
             kcfg->rx_dma_size, kcfg->rx_buf_size, kcfg->tx_dma_size,
             kcfg->tx_buf_size, prv->allocated);

    if (kcfg->rx_dma_size == 0 || kcfg->rx_buf_size == 0 ||
        (kcfg->tx_dma_size == 0 && kcfg->tx_buf_size != 0) ||
        !OSI_IS_ALIGNED(kcfg->rx_dma_size, CONFIG_CACHE_LINE_SIZE) ||
        !OSI_IS_ALIGNED(kcfg->tx_dma_size, CONFIG_CACHE_LINE_SIZE) ||
        prv->allocated != NULL)
    {
        OSI_LOGE(0, "usb serial %4c invalid cfg.", PORT_NAME(port));
        return -EINVAL;
    }

    prv->tx_fifo = NULL;
    prv->rx_fifo = NULL;
    void *ptr = NULL;
    osiBlockedFifo_t *rx_fifo = NULL;
    osiBlockedFifo_t *tx_fifo = NULL;
    unsigned allocate_size = kcfg->rx_buf_size + kcfg->tx_buf_size;
    if (allocate_size != 0)
    {
        allocate_size += CONFIG_CACHE_LINE_SIZE;
        ptr = malloc(allocate_size);
        if (ptr == NULL)
            return -ENOMEM;

        uintptr_t pextra = OSI_ALIGN_UP(ptr, CONFIG_CACHE_LINE_SIZE);
        if (kcfg->rx_buf_size)
        {
            uint8_t *rxbuf = (uint8_t *)OSI_PTR_INCR_POST(pextra, kcfg->rx_buf_size);
            rx_fifo = osiBlockedFifoCreate(rxbuf, kcfg->rx_buf_size,
                                           CONFIG_CACHE_LINE_SIZE, kcfg->rx_dma_size,
                                           2 * kcfg->rx_buf_size / kcfg->rx_dma_size);
            if (rx_fifo == NULL)
            {
                OSI_LOGE(0, "usb serial %4c init rxfifo fail", PORT_NAME(port));
                free(ptr);
                return -EINVAL;
            }
        }

        if (kcfg->tx_buf_size)
        {
            uint8_t *txbuf = (uint8_t *)OSI_PTR_INCR_POST(pextra, kcfg->tx_buf_size);
            tx_fifo = osiBlockedFifoCreate(txbuf, kcfg->tx_buf_size,
                                           CONFIG_CACHE_LINE_SIZE, kcfg->tx_dma_size,
                                           kcfg->tx_buf_size / kcfg->tx_dma_size + 2);
            if (tx_fifo == NULL)
            {
                OSI_LOGE(0, "usb serial %4c init txfifo fail", PORT_NAME(port));
                osiBlockedFifoDelete(rx_fifo);
                free(ptr);
                return -EINVAL;
            }
        }
    }

    uint32_t critical = osiEnterCritical();
    prv->allocated = ptr;
    prv->tx_fifo = tx_fifo;
    prv->rx_fifo = rx_fifo;
    prv->inited = true;
    if (prv->ready)
        prvSerialStartLocked(prv);
    osiExitCritical(critical);
    return 0;
}

static void prvClose(drvSerialImpl_t *port)
{
    usbSerialPriv_t *prv = PORT_PRIV(port);
    if (!prv->inited)
        return;

    uint32_t critical = osiEnterCritical();
    prv->inited = false;
    prvSerialStopLocked(prv);

    osiBlockedFifo_t *rx_fifo = prv->rx_fifo;
    osiBlockedFifo_t *tx_fifo = prv->tx_fifo;
    void *ptr = prv->allocated;
    prv->rx_fifo = NULL;
    prv->tx_fifo = NULL;
    prv->allocated = NULL;
    osiExitCritical(critical);

    if (rx_fifo)
        osiBlockedFifoDelete(rx_fifo);
    if (tx_fifo)
        osiBlockedFifoDelete(tx_fifo);

    if (ptr)
        free(ptr);
}

static bool prvReady(drvSerialImpl_t *port)
{
    usbSerialPriv_t *priv = PORT_PRIV(port);
    return priv->ready;
}

static int prvWrite(drvSerialImpl_t *port, const void *data, size_t size)
{
    if (data == NULL || size == 0)
        return 0;

    int send = -1;
    uint32_t critical = osiEnterCritical();
    usbSerialPriv_t *priv = PORT_PRIV(port);
    if (SERIAL_RUNNING(priv) && priv->open && priv->tx_fifo != NULL)
    {
        send = osiBlockedFifoPut(priv->tx_fifo, data, size);
        if (send > 0)
            prvTxPollLocked(priv);
    }
    osiExitCritical(critical);
    return send;
}

static bool prvWriteDirect(drvSerialImpl_t *port, const void *data, size_t size, size_t toms, uint32_t flags)
{
    if (data == NULL || size == 0)
        return true;

    bool sent_done = false;
    uint32_t critical = osiEnterCritical();
    usbSerialPriv_t *priv = PORT_PRIV(port);
    usbXfer_t *x = priv->tx_xfer;
    if (x->status != -EINPROGRESS && SERIAL_RUNNING(priv) && priv->open)
    {
        prvTxTransBlockLocked_(priv, data, size, !(flags & DRV_SERIAL_FLAG_TXBUF_UNCACHED));
        sent_done = prvWaitWriteFinishLocked(priv, toms);
    }

    // It is by design not to cancel the timeout transfer. So, the
    // next transfer will fail immediately. The pitfall is when PC
    // connect the serial, PC may receive garbage data due to caller
    // may free or reuse the memory.
    osiExitCritical(critical);
    return sent_done;
}

static int prvRead(drvSerialImpl_t *port, void *buffer, size_t size)
{
    usbSerialPriv_t *priv = PORT_PRIV(port);
    if (!SERIAL_RUNNING(priv))
        return -1;

    if (!buffer || size == 0)
        return 0;

    return osiBlockedFifoGet(priv->rx_fifo, buffer, size);
}

static int prvReadAvail(drvSerialImpl_t *port)
{
    usbSerialPriv_t *priv = PORT_PRIV(port);
    if (!SERIAL_RUNNING(priv))
        return -1;
    return osiBlockedFifoBytes(priv->rx_fifo);
}

static int prvWriteAvail(drvSerialImpl_t *port)
{
    usbSerialPriv_t *priv = PORT_PRIV(port);
    if (!SERIAL_RUNNING(priv))
        return -1;
    return priv->tx_fifo == NULL ? 0 : osiBlockedFifoSpace(priv->tx_fifo);
}

static bool prvWaitWriteAvail(drvSerialImpl_t *port, unsigned timeout)
{
    uint32_t critical = osiEnterCritical();
    usbSerialPriv_t *priv = PORT_PRIV(port);
    bool result = prvWaitWriteAvailLocked(priv, timeout);
    osiExitCritical(critical);
    return result;
}

static bool prvWaitReadAvail(drvSerialImpl_t *port, unsigned timeout)
{
    uint32_t critical = osiEnterCritical();
    usbSerialPriv_t *priv = PORT_PRIV(port);
    bool result = prvWaitReadAvailLocked(priv, timeout);
    osiExitCritical(critical);
    return result;
}

static bool prvWaitWriteFinish(drvSerialImpl_t *port, unsigned timeout)
{
    uint32_t critical = osiEnterCritical();
    usbSerialPriv_t *priv = PORT_PRIV(port);
    bool result = prvWaitWriteFinishLocked(priv, timeout);
    osiExitCritical(critical);
    return result;
}

static void prvFreePrivLocked(usbSerialPriv_t *priv, udc_t *controller, bool async_check)
{
    priv->async_later_free = true;
    priv->inited = false;
    priv->open = false;
    // NULL pointers can be handled
    osiWorkDelete(priv->work_rx);
    osiWorkDelete(priv->work_tx);
    udcXferFree(controller, priv->tx_xfer);
    udcXferFree(controller, priv->rx_xfer);
    udcEpFree(controller, priv->tx_ep);
    udcEpFree(controller, priv->rx_ep);
    if (priv->tx_fifo)
        osiBlockedFifoDelete(priv->tx_fifo);
    if (priv->rx_fifo)
        osiBlockedFifoDelete(priv->rx_fifo);
    if (priv->allocated)
        free(priv->allocated);

    if (!async_check || priv->async_res_ref == 0)
    {
        prvAsyncResFreeLocked(priv);
        return;
    }

    // clear all free member if priv still not release
    priv->work_rx = NULL;
    priv->work_tx = NULL;
    priv->tx_xfer = NULL;
    priv->rx_xfer = NULL;
    priv->tx_ep = NULL;
    priv->rx_ep = NULL;
    priv->tx_fifo = NULL;
    priv->rx_fifo = NULL;
    priv->allocated = NULL;
}

int usbSerialBind(uint32_t name, usbSerial_t *cdc)
{
    if (!cdc || !cdc->func || !cdc->func->controller ||
        !cdc->epin_desc || !cdc->epout_desc)
        return -1;

    OSI_LOGI(0x10005657, "CDC serial %4c bind", name);

    osiWorkQueue_t *wq = drvUsbWorkQueue();
    if (wq == NULL)
        return -1;

    usbSerialPriv_t *priv = (usbSerialPriv_t *)calloc(1, sizeof(usbSerialPriv_t));
    if (priv == NULL)
    {
        OSI_LOGE(0, "usb serial %4c bind not enough memory", name);
        return -1;
    }

    udc_t *controller = cdc->func->controller;
    priv->tx_ep = udcEpAlloc(controller, cdc->epin_desc);
    if (priv->tx_ep == NULL)
    {
        OSI_LOGE(0, "usb serial %4c bind tx_ep allocate fail", name);
        goto failed;
    }

    priv->rx_ep = udcEpAlloc(controller, cdc->epout_desc);
    if (priv->rx_ep == NULL)
    {
        OSI_LOGE(0, "usb serial %4c bind rx_ep allocate fail", name);
        goto failed;
    }

    priv->tx_xfer = udcXferAlloc(controller);
    if (priv->tx_xfer == NULL)
        goto failed;

    priv->rx_xfer = udcXferAlloc(controller);
    if (priv->rx_xfer == NULL)
        goto failed;

    priv->tx_avail_sem = osiSemaphoreCreate(1, 1);
    if (priv->tx_avail_sem == NULL)
        goto failed;

    priv->rx_avail_sem = osiSemaphoreCreate(1, 1);
    if (priv->rx_avail_sem == NULL)
        goto failed;

    priv->tx_finish_sem = osiSemaphoreCreate(1, 1);
    if (priv->tx_finish_sem == NULL)
        goto failed;

    priv->work_queue = wq;
    priv->work_tx = osiWorkCreate(prvTxDoneWork, NULL, priv);
    if (priv->work_tx == NULL)
        goto failed;

    priv->work_rx = osiWorkCreate(prvRxDoneWork, NULL, priv);
    if (priv->work_rx == NULL)
        goto failed;

    priv->cdc = cdc;
    priv->tx_timestamp = 0;
    priv->tx_buf_in_trans = NULL;
    priv->tx_fifo = NULL;

    priv->port.ops.open = prvOpen;
    priv->port.ops.close = prvClose;
    priv->port.ops.ready = prvReady;
    priv->port.ops.write = prvWrite;
    priv->port.ops.write_direct = prvWriteDirect;
    priv->port.ops.read = prvRead;
    priv->port.ops.read_avail = prvReadAvail;
    priv->port.ops.write_avail = prvWriteAvail;
    priv->port.ops.wait_write_avail = prvWaitWriteAvail;
    priv->port.ops.wait_read_avail = prvWaitReadAvail;
    priv->port.ops.wait_write_finish = prvWaitWriteFinish;
    bool r = drvSerialRegisterImpl(name, &priv->port);
    if (!r)
    {
        OSI_LOGE(0, "usb serial %4c bind fail to register serial impl", name);
        goto failed;
    }

    cdc->epin_desc->bEndpointAddress = priv->tx_ep->address;
    cdc->epout_desc->bEndpointAddress = priv->rx_ep->address;
    cdc->priv = (unsigned long)priv;
    return 0;

failed:
    // error handle: ignore critical section here
    prvFreePrivLocked(priv, controller, false);
    return -1;
}

void usbSerialUnbind(usbSerial_t *cdc)
{
    if (!cdc || !cdc->func || !cdc->func->controller)
        return;

    usbSerialPriv_t *priv = SERIAL_PRIV(cdc);
    if (priv == NULL)
        return;

    OSI_LOGI(0x10005658, "CDC serial %4c unbind", PORT_NAME(&priv->port));
    drvSerialUnregisterImpl(&priv->port);

    uint32_t critical = osiEnterCritical();
    udc_t *controller = cdc->func->controller;
    cdc->priv = (unsigned long)NULL;
    prvFreePrivLocked(priv, controller, true);
    osiExitCritical(critical);
}

int usbSerialEnable(usbSerial_t *cdc)
{
    if (!cdc || !cdc->func || !cdc->func->controller)
        return -EINVAL;

    usbSerialPriv_t *priv = SERIAL_PRIV(cdc);
    if (priv == NULL)
        return -EINVAL;

    uint32_t critical = osiEnterCritical();
    priv->open = false;
    priv->ready = true;

    udc_t *udc = priv->cdc->func->controller;
    int result = udcEpEnable(udc, priv->tx_ep);
    if (result < 0)
    {
        osiExitCritical(critical);
        OSI_LOGE(0, "%x enable fail", priv->tx_ep->address);
        return result;
    }

    result = udcEpEnable(udc, priv->rx_ep);
    if (result < 0)
    {
        udcEpDisable(udc, priv->tx_ep);
        osiExitCritical(critical);
        OSI_LOGE(0, "%x enable fail", priv->rx_ep->address);
        return result;
    }

    if (priv->inited)
        prvSerialStartLocked(priv);
    osiExitCritical(critical);

    drvSerialImpl_t *port = &priv->port;
    OSI_LOGI(0x10005659, "CDC serial %4c enable", PORT_NAME(port));
    priv->pending_event |= DRV_SERIAL_EVENT_READY;
    prvNotifyPendingEvent(priv);
    return 0;
}

void usbSerialDisable(usbSerial_t *cdc)
{
    if (!cdc || !cdc->func || !cdc->func->controller)
        return;

    usbSerialPriv_t *priv = SERIAL_PRIV(cdc);
    if (priv == NULL)
        return;

    uint32_t critical = osiEnterCritical();
    priv->ready = false;
    priv->open = false;
    prvSerialStopLocked(priv);

    udc_t *udc = priv->cdc->func->controller;
    udcEpDisable(udc, priv->rx_ep);
    udcEpDisable(udc, priv->tx_ep);

    osiExitCritical(critical);

    drvSerialImpl_t *port = &priv->port;
    OSI_LOGI(0x1000565a, "CDC serial %4c disable", PORT_NAME(port));
    priv->pending_event |= DRV_SERIAL_EVENT_BROKEN;
    prvNotifyPendingEvent(priv);
}

bool usbSerialOpen(usbSerial_t *cdc)
{
    if (cdc->priv == 0)
        return false;

    uint32_t critical = osiEnterCritical();
    usbSerialPriv_t *priv = SERIAL_PRIV(cdc);
    if (!priv->open)
    {
        priv->open = true;
        priv->pending_event |= DRV_SERIAL_EVENT_OPEN;
    }
    osiExitCritical(critical);
    OSI_LOGI(0, "usb serial %4c open %x", PORT_NAME(&priv->port), priv->pending_event);
    prvNotifyPendingEvent(priv);
    return true;
}

void usbSerialClose(usbSerial_t *cdc)
{
    if (cdc->priv == 0)
        return;

    uint32_t critical = osiEnterCritical();
    usbSerialPriv_t *priv = SERIAL_PRIV(cdc);
    if (priv->open)
    {
        priv->open = false;
        priv->pending_event |= DRV_SERIAL_EVENT_CLOSE;
    }
    osiExitCritical(critical);
    OSI_LOGI(0, "usb serial %4c close %x", PORT_NAME(&priv->port), priv->pending_event);
    prvNotifyPendingEvent(priv);
}

static const usbSerialDriverOps_t gUserialOps = {
    .bind = usbSerialBind,
    .unbind = usbSerialUnbind,
    .enable = usbSerialEnable,
    .disable = usbSerialDisable,
    .open = usbSerialOpen,
    .close = usbSerialClose,
};

const usbSerialDriverOps_t *usbSerialGetOps(void)
{
    return &gUserialOps;
}
