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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('U', 'P', 'D', 'L')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "boot_fdl_channel.h"
#include "boot_platform.h"
#include "boot_udc.h"
#include "osi_fifo.h"
#include "drv_names.h"
#include "hal_config.h"
#include "osi_log.h"
#include "osi_api.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define RX_BUFFER_SIZE_DEFAULT (8 * 1024)
#define RX_DMA_SIZE_DEFAULT (2048)
#define TX_DMA_SIZE_DEFAULT (2048)

#define DATA_BUFFER_SIZE (RX_BUFFER_SIZE_DEFAULT + RX_DMA_SIZE_DEFAULT + TX_DMA_SIZE_DEFAULT)

typedef enum
{
    RX_IDLE = 0,
    RX_START,
    RX_DONE,
} rxState_t;

typedef struct
{
    fdlChannel_t ops;
    osiFifo_t rx_fifo;
    bootUdc_t *udc;
    uint8_t *rx_dma_buffer;
    uint8_t *tx_dma_buffer;
    uint32_t rx_dma_size;
    uint32_t tx_dma_size;
    bootUsbEp_t *in_ep;
    bootUsbEp_t *out_ep;
    bootUsbXfer_t *in_xfer;
    bootUsbXfer_t *out_xfer;
    bool rx_running;
    bool tx_running;
} fdlUsbChannel_t;

static void _rxComp(bootUsbEp_t *ep, bootUsbXfer_t *xfer)
{
    fdlUsbChannel_t *ch = (fdlUsbChannel_t *)xfer->param;
    if (xfer->status == 0)
    {
        osiDCacheInvalidate(xfer->buf, OSI_ALIGN_UP(xfer->actual, CONFIG_CACHE_LINE_SIZE));
        int len = osiFifoPut(&ch->rx_fifo, xfer->buf, xfer->actual);
        if (len != xfer->actual)
        {
            OSI_LOGE(0, "usb fdl rx overflow, actual %u, saved %d", xfer->actual, len);
            osiPanic();
        }
    }
    else
    {
        OSI_LOGE(0, "usb fdl rx done not ok %d", xfer->status);
        osiPanic();
    }
    ch->rx_running = false;
}

static bool _startRx(fdlUsbChannel_t *ch)
{
    bootUsbXfer_t *xfer = ch->out_xfer;
    xfer->buf = ch->rx_dma_buffer;
    xfer->length = ch->rx_dma_size;
    xfer->actual = 0;
    xfer->param = (void *)ch;
    xfer->complete = _rxComp;
    ch->rx_running = true;
    osiDCacheInvalidate(xfer->buf, xfer->length);
    bool r = bootUdcXferEnqueue(ch->udc, ch->out_ep, xfer);
    if (!r)
    {
        ch->rx_running = false;
        OSI_LOGE(0, "usb fdl start rx fail");
        osiPanic();
    }
    return r;
}

static void _poll(fdlUsbChannel_t *ch)
{
    bool r = true;
    if (!ch->rx_running)
        r = _startRx(ch);

    if (r)
        r = bootUdcIsrPoll(ch->udc);

    if (!r)
    {
        OSI_LOGE(0, "usb fdl rx poll fail");
        osiPanic();
    }
}

static int _avail(fdlChannel_t *ch_)
{
    fdlUsbChannel_t *ch = (fdlUsbChannel_t *)ch_;
    _poll(ch);
    return osiFifoBytes(&ch->rx_fifo);
}

static int _read(fdlChannel_t *ch_, void *data, size_t size)
{
    fdlUsbChannel_t *ch = (fdlUsbChannel_t *)ch_;
    _poll(ch);
    return osiFifoGet(&ch->rx_fifo, data, size);
}

static void _txComp(bootUsbEp_t *ep, bootUsbXfer_t *xfer)
{
    fdlUsbChannel_t *ch = (fdlUsbChannel_t *)xfer->param;
    if (xfer->status != 0)
    {
        OSI_LOGE(0, "usb fdl tx done fail, %d", xfer->status);
        osiPanic();
    }
    ch->tx_running = false;
}

static int _txStart(fdlUsbChannel_t *ch, const void *data, size_t size)
{
    if (size > ch->tx_dma_size)
        size = ch->tx_dma_size;
    memcpy(ch->tx_dma_buffer, data, size);
    bootUsbXfer_t *xfer = ch->in_xfer;
    xfer->buf = ch->tx_dma_buffer;
    xfer->length = size;
    xfer->actual = 0;
    xfer->zlp = 1;
    xfer->param = ch;
    xfer->complete = _txComp;
    ch->tx_running = true;
    osiDCacheClean(xfer->buf, xfer->length);
    bool r = bootUdcXferEnqueue(ch->udc, ch->in_ep, xfer);
    if (!r)
        return -1;
    while (bootUdcIsrPoll(ch->udc) && ch->tx_running)
        ;
    return ch->tx_running ? -1 : size;
}

static int _write(fdlChannel_t *ch_, const void *data, size_t size)
{
    fdlUsbChannel_t *ch = (fdlUsbChannel_t *)ch_;
    if (ch->tx_running)
    {
        OSI_LOGE(0, "usb fdl write but tx busy");
        osiPanic();
        return -1;
    }

    size_t sent = 0;
    while (sent < size)
    {
        int len = _txStart(ch, data + sent, size - sent);
        if (len < 0)
        {
            OSI_LOGE(0, "usb fdl tx start fail, sent %u", sent);
            osiPanic();
        }
        sent += len;
    }
    return sent;
}

static void _flushInput(fdlChannel_t *ch_)
{
    fdlUsbChannel_t *ch = (fdlUsbChannel_t *)ch_;
    _poll(ch);
    osiFifoReset(&ch->rx_fifo);
}

static void _destroy(fdlChannel_t *ch_)
{
    fdlUsbChannel_t *ch = (fdlUsbChannel_t *)ch_;
    // Rom ep do not need to release
    bootUdcXferFree(ch->udc, ch->in_xfer);
    bootUdcXferFree(ch->udc, ch->out_xfer);
    bootUdcClose(ch->udc);
    free(ch);
}

static bool _initUsbInfo(fdlUsbChannel_t *ch)
{
    bootUdc_t *udc = bootUdcOpen(DRV_NAME_USB20, true);
    if (udc == NULL)
        return false;

    bootUsbEp_t *in_ep = bootUdcRomSerialInEp(udc);
    bootUsbEp_t *out_ep = bootUdcRomSerialOutEp(udc);
    if (!in_ep || !out_ep)
        osiPanic();

    bootUsbXfer_t *in_xfer = bootUdcXferAlloc(udc, in_ep);
    bootUsbXfer_t *out_xfer = bootUdcXferAlloc(udc, out_ep);
    if (!in_xfer || !out_xfer)
        goto fail;

    ch->udc = udc;
    ch->in_ep = in_ep;
    ch->out_ep = out_ep;
    ch->in_xfer = in_xfer;
    ch->out_xfer = out_xfer;
    return true;

fail:
    if (in_xfer)
        bootUdcXferFree(udc, in_xfer);
    if (out_xfer)
        bootUdcXferFree(udc, out_xfer);
    bootUdcClose(udc);
    return false;
}

static bool _connected(fdlChannel_t *ch_)
{
    fdlUsbChannel_t *ch = (fdlUsbChannel_t *)ch_;
    return bootUdcConnected(ch->udc);
}

static bool _setBaud(fdlChannel_t *ch_, uint32_t baud) { return true; }
static void _flush(fdlChannel_t *ch) {}

fdlChannel_t *fdlOpenUsbSerial()
{

    uint32_t alloc_s = DATA_BUFFER_SIZE + sizeof(fdlUsbChannel_t) + CONFIG_CACHE_LINE_SIZE;
    fdlUsbChannel_t *ch = (fdlUsbChannel_t *)malloc(alloc_s);
    if (ch == NULL)
        return NULL;

    memset(ch, 0, alloc_s);
    if (!_initUsbInfo(ch))
    {
        free(ch);
        return NULL;
    }

    uintptr_t pextra = (uintptr_t)ch + sizeof(fdlUsbChannel_t);
    uint8_t *rx_buffer = (uint8_t *)OSI_PTR_INCR_POST(pextra, RX_BUFFER_SIZE_DEFAULT);
    osiFifoInit(&ch->rx_fifo, rx_buffer, RX_BUFFER_SIZE_DEFAULT);
    pextra = OSI_ALIGN_UP(pextra, CONFIG_CACHE_LINE_SIZE);
    ch->rx_dma_buffer = (uint8_t *)OSI_PTR_INCR_POST(pextra, RX_DMA_SIZE_DEFAULT);
    ch->tx_dma_buffer = (uint8_t *)OSI_PTR_INCR_POST(pextra, TX_DMA_SIZE_DEFAULT);
    ch->rx_dma_size = RX_DMA_SIZE_DEFAULT;
    ch->tx_dma_size = TX_DMA_SIZE_DEFAULT;

    ch->ops.set_baud = _setBaud;
    ch->ops.avail = _avail;
    ch->ops.write = _write;
    ch->ops.read = _read;
    ch->ops.flush_input = _flushInput;
    ch->ops.flush = _flush;
    ch->ops.connected = _connected;
    ch->ops.destroy = _destroy;

    if (!_startRx(ch))
    {
        _destroy(&ch->ops);
        return NULL;
    }

    return &ch->ops;
}
