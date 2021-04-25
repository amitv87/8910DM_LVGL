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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('B', 'U', 'D', 'C')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "boot_udc.h"
#include "boot_platform.h"
#include "drv_names.h"
#include "hwregs.h"
#include "usb/usb_ch9.h"
#include "osi_log.h"
#include "osi_api.h"
#include <sys/errno.h>
#include <stdlib.h>
#include <string.h>

typedef REG_USBC_DIEPCTL1_T usbcDiepctl_t;
typedef REG_USBC_DIEPINT1_T usbcDiepint_t;
typedef REG_USBC_DIEPTSIZ1_T usbcDieptsiz_t;
typedef REG_USBC_DTXFSTS1_T usbcDtxfsts_t;

typedef REG_USBC_DOEPCTL1_T usbcDoepctl_t;
typedef REG_USBC_DOEPINT1_T usbcDoepint_t;
typedef REG_USBC_DOEPTSIZ1_T usbcDoeptsiz_t;

typedef enum
{
    EP0_SETUP = 0,
#define EP0_STATE_DIRIN_MASK 0b10
#define EP0_STATE_DATA_MASK 0b1
    EP0_DATA_IN = 0b111,
    EP0_DATA_OUT = 0b101,
    EP0_STATUS_IN = 0b110,
    EP0_STATUS_OUT = 0b100,
} ep0State_t;

typedef struct
{
    REG32 epctl;
    REG32 reserved_1_io;
    REG32 epint;
    REG32 reserved_2_io;
    REG32 eptsiz;
    REG32 epdma;
    REG32 dtxfsts; // in ep only
    REG32 epdmab;
} epReg_t;

#ifdef CONFIG_SOC_8910
#define IEP_CNT (13) // include ep 0
#define OEP_CNT (13) // include ep 0
#define ROM_SERIAL_OEP (0x02)
#define ROM_SERIAL_IEP (0x81)
#define ROM_SERIAL_NOTIFY_EP (0x83)
#define TX_FIFO_CNT (8)
#endif

typedef struct udc_xfer
{
    bootUsbXfer_t xfer;
    uint32_t xfer_size;
    uint8_t zero : 1;
    uint8_t running : 1;
} udcXfer_t;

typedef struct udc_ep
{
    bootUsbEp_t ep;
    udcXfer_t *cur_xfer;
    uint32_t dma_max;
    epReg_t *hwep;
    uint8_t fifo_index : 4;
    uint8_t type : 2;
    uint8_t periodic : 1;
} udcEp_t;

struct boot_udc
{
    HWP_USBC_T *hw;
    udcEp_t iep[IEP_CNT];
    udcEp_t oep[IEP_CNT];
    uint16_t tx_fifo_map;
    ep0State_t ep0_state;
    uint8_t inited : 1;
    uint8_t connected : 1;
    uint8_t reserved_fifo : 1;
};

static bootUdc_t gUdc20 = {
    .hw = hwp_usbc,
};
static void _giveBackComplete(bootUdc_t *udc, udcEp_t *ep, udcXfer_t *x);
static uint32_t _epXferStart(bootUdc_t *udc, udcEp_t *ep, void *buf, uint32_t length);
static inline void _txFifoFlush(bootUdc_t *udc, uint8_t index);
static inline void _txFifoFlushAll(bootUdc_t *udc);

static void _setHwEpMps(bootUdc_t *udc, udcEp_t *ep, uint16_t mps)
{
    // max_dma_size = min(mps * max(EPTSIZ->npkt), max(EPTSIZE->xferSize))
    uint32_t max_dma_size = 0xffc0;
    uint32_t max_pkt_size = mps;
    if (ep->ep.num == 0)
    {
        switch (max_pkt_size)
        {
        case 64:
            mps = 0;
            break;

        case 32:
            mps = 1;
            break;

        case 16:
            mps = 2;
            break;

        case 8:
        default:
            mps = 3;
            break;
        }
        if (ep->ep.dirin)
            max_dma_size = 0x7f;
        else
            max_dma_size = 0x40;
    }

    max_dma_size -= (max_dma_size % max_pkt_size);

    ep->ep.mps = max_pkt_size;
    ep->dma_max = max_dma_size;

    usbcDiepctl_t epctl = {};
    epctl.b.mps = mps;
    ep->hwep->epctl |= epctl.v;
}

static void _initUdcCommonConfig(bootUdc_t *udc)
{
    memset(&udc->iep[0], 0, sizeof(udcEp_t) * IEP_CNT);
    memset(&udc->oep[0], 0, sizeof(udcEp_t) * OEP_CNT);
    for (uint8_t i = 0; i < IEP_CNT; ++i)
    {
        udc->iep[i].hwep = (epReg_t *)&udc->hw->diepctl0 + i;
        udc->iep[i].ep.num = i;
        udc->iep[i].ep.dirin = 1;
    }

    for (uint8_t i = 0; i < OEP_CNT; ++i)
    {
        udc->oep[i].hwep = (epReg_t *)&udc->hw->doepctl0 + i;
        udc->oep[i].ep.num = i;
    }
    udc->tx_fifo_map |= (1 << 0); // ep 0x80 use
}

static void _initUdcRomConfig(bootUdc_t *udc)
{
#ifdef CONFIG_SOC_8910
    _initUdcCommonConfig(udc);
    uint16_t ep0_mps = 64;
    uint16_t ep_mps = 64;
    _setHwEpMps(udc, &udc->iep[0], ep0_mps);
    _setHwEpMps(udc, &udc->oep[0], ep0_mps);
    for (uint8_t i = 1; i < IEP_CNT; ++i)
        _setHwEpMps(udc, &udc->iep[i], ep_mps);
    for (uint8_t i = 1; i < OEP_CNT; ++i)
        _setHwEpMps(udc, &udc->oep[i], ep_mps);

    _txFifoFlushAll(udc);

    // notify ep
    udc->iep[1].fifo_index = 1;
    udc->tx_fifo_map |= (1 << 1);

    // data in ep
    udc->iep[1].fifo_index = 1;
    udc->tx_fifo_map |= (1 << 1);

    // set ep int mask
    uint16_t oepmsk = (1 << 0) | (1 << 2);
    uint16_t iepmsk = (1 << 0) | (1 << 1) | (1 << 3);
    udc->hw->daintmsk = (oepmsk << 16) | iepmsk;

    udc->connected = 1;
    udc->inited = 1;
    udc->reserved_fifo = 1;
#endif
}

static void _initUdcConfig(bootUdc_t *udc)
{
    _initUdcCommonConfig(udc);
    osiPanic(); // TODO
}

bootUdc_t *bootUdcOpen(uint32_t name, bool rom)
{
    if (name != DRV_NAME_USB20)
        return NULL;

    bootUdc_t *udc = &gUdc20;
    if (udc->inited == 1)
        return udc;

    if (rom)
        _initUdcRomConfig(udc);
    else
        _initUdcConfig(udc);

    return udc;
}

void bootUdcClose(bootUdc_t *udc)
{
}

static void _enqueueSetup(bootUdc_t *udc)
{
    osiPanic(); // TODO
}

static void _epInIsrDone(bootUdc_t *udc, udcEp_t *ep)
{
    if (udc->reserved_fifo == 0)
        _txFifoFlush(udc, ep->fifo_index);

    if (ep->ep.num == 0)
    {
        OSI_LOGE(0, "ep0 in done (TODO)");
        osiPanic();
        if (!(udc->ep0_state & EP0_STATE_DIRIN_MASK))
        {
            OSI_LOGW(0, "ep0 in done, state mismatch (%d)", udc->ep0_state);
            return;
        }

        if (udc->ep0_state == EP0_STATUS_IN)
        {
            udcEp_t *ep0_out = &udc->oep[0];
            if (ep0_out->cur_xfer)
                _giveBackComplete(udc, ep0_out, ep0_out->cur_xfer);

            // TODO test mode here
            _enqueueSetup(udc);
            return;
        }
    }

    udcXfer_t *x = ep->cur_xfer;
    if (!x)
    {
        OSI_LOGW(0, "ep %x in done, no transfer", ep->ep.address);
        return;
    }

    uint32_t size_left, size_done;
    if (ep->ep.num == 0)
    {
        REG_USBC_DIEPTSIZ0_T dieptsiz0 = {ep->hwep->eptsiz};
        size_left = dieptsiz0.b.xfersize;
    }
    else
    {
        usbcDieptsiz_t dieptsiz = {ep->hwep->eptsiz};
        size_left = dieptsiz.b.xfersize;
    }

    if (size_left > x->xfer_size)
    {
        OSI_LOGE(0, "ep %x in done, overflow, size left %u, xfer size %u",
                 ep->ep.address, size_left, x->xfer_size);
    }

    size_done = x->xfer_size - size_left;
    x->xfer.actual += size_done;

    bool transfer_done = (x->xfer.actual == x->xfer.length);
    if (!transfer_done)
    {
        uint32_t size = x->xfer.length - x->xfer.actual;
        x->xfer_size = _epXferStart(udc, ep, x->xfer.buf + x->xfer.actual, size);
        return;
    }
    else if (x->zero)
    {
        x->zero = 0;
        x->xfer_size = _epXferStart(udc, ep, NULL, 0);
        return;
    }

    if (transfer_done)
    {
        x->xfer.status = 0;
        if (ep->ep.num == 0 && udc->ep0_state == EP0_DATA_IN)
        {
            udc->ep0_state = EP0_STATUS_OUT;
            osiPanic(); // TODO
            return;
        }
        _giveBackComplete(udc, ep, x);
    }
}

static void _epInIsr(bootUdc_t *udc, udcEp_t *ep, uint32_t epint_)
{
    usbcDiepint_t epint = {epint_};
    ep->hwep->epint = epint_;

    if (epint.b.xfercompl)
        _epInIsrDone(udc, ep);
}

static void _epOutIsrDone(bootUdc_t *udc, udcEp_t *ep)
{
    if (ep->ep.num == 0)
    {
        OSI_LOGE(0, "ep0 out done (TODO)");
        osiPanic();
        if (udc->ep0_state & EP0_STATE_DIRIN_MASK)
        {
            OSI_LOGW(0, "ep0 out done, state mismatch (%d)", udc->ep0_state);
            return;
        }

        if (udc->ep0_state == EP0_STATUS_OUT)
        {
            udcEp_t *ep0_in = &udc->iep[0];
            if (ep0_in->cur_xfer)
                _giveBackComplete(udc, ep0_in, ep0_in->cur_xfer);
            _enqueueSetup(udc);
            return;
        }
    }

    udcXfer_t *x = ep->cur_xfer;
    if (x == NULL)
    {
        OSI_LOGW(0, "ep %x out done, no transfer", ep->ep.address);
        return;
    }

    uint32_t size_left, size_done;
    if (ep->ep.num == 0)
    {
        REG_USBC_DOEPTSIZ0_T doeptsiz0 = {ep->hwep->eptsiz};
        size_left = doeptsiz0.b.xfersize;
    }
    else
    {
        usbcDoeptsiz_t doeptsiz = {ep->hwep->eptsiz};
        size_left = doeptsiz.b.xfersize;
    }

    size_done = x->xfer_size - size_left;
    x->xfer.actual += size_done;

    bool transfer_done = x->xfer.length == x->xfer.actual || size_left != 0;
    if (!transfer_done)
    {
        uint32_t size = x->xfer.length - x->xfer.actual;
        if (size > ep->dma_max)
            size = ep->dma_max;
        x->xfer_size = _epXferStart(udc, ep, x->xfer.buf + x->xfer.actual, size);
    }
    else
    {
        x->xfer.status = 0;
        if (ep->ep.num == 0 && udc->ep0_state == EP0_DATA_OUT)
        {
            OSI_LOGE(0, "ep0 data phase done, swith in status");
            udc->ep0_state = EP0_STATUS_IN;
            osiPanic(); // TODO
            return;
        }
        _giveBackComplete(udc, ep, x);
    }
}

static void _epOutIsr(bootUdc_t *udc, udcEp_t *ep, uint32_t epint_)
{
    usbcDoepint_t epint = {epint_};
    ep->hwep->epint = epint_;

    /**
     * ignore complete bit when in SETUP stage, and process
     * out handler by setup bit.
     *
     * 1. clear xfercompl bit if has any [setup, stuppktrcvd] bit
     * 2. clear xfercompl bit if in SETUP stage (may not have setup interrupt bits)
     */
    if (epint.b.setup || epint.b.stuppktrcvd)
        epint.b.xfercompl = 0;

    if (ep->ep.num == 0 && udc->ep0_state == EP0_SETUP && !(epint.b.setup))
        epint.b.xfercompl = 0;

    if (epint.b.xfercompl || (ep->ep.num == 0 && epint.b.setup))
        _epOutIsrDone(udc, ep);
}

static inline void _epIsr(bootUdc_t *udc, udcEp_t *ep)
{
    uint32_t epint = {ep->hwep->epint};
    OSI_LOGV(0, "ep %x ep isr %x", ep->ep.address, epint);

    ep->hwep->epint = epint; // write to clear
    if (ep->ep.dirin)
    {
        epint &= udc->hw->diepmsk;
        _epInIsr(udc, ep, epint);
    }
    else
    {
        epint &= udc->hw->doepmsk;
        _epOutIsr(udc, ep, epint);
    }
}

bool bootUdcConnected(bootUdc_t *udc)
{
    REG_USBC_DCFG_T dcfg = {udc->hw->dcfg};
    REG_USBC_DSTS_T dsts = {udc->hw->dsts};
    return dcfg.b.devaddr != 0 && dsts.b.suspsts == 0;
}

bool bootUdcIsrPoll(bootUdc_t *udc)
{
    REG_USBC_GINTSTS_T gintsts = {udc->hw->gintsts};
    udc->hw->gintsts = gintsts.v;
    gintsts.v &= udc->hw->gintmsk;

    if (gintsts.b.usbrst || gintsts.b.resetdet)
    {
        OSI_LOGE(0, "usb reset");
        osiPanic(); // TODO
    }

    if (gintsts.b.enumdone)
    {
        OSI_LOGE(0, "usb enumdone");
        osiPanic();
    }

    if (gintsts.b.usbsusp)
    {
        OSI_LOGE(0, "usb suspend");
    }

    if (gintsts.b.wkupint)
    {
        OSI_LOGE(0, "usb wakeup");
    }

    uint32_t daint = udc->hw->daint;
    daint &= udc->hw->daintmsk;
    if (gintsts.b.oepint)
    {
        uint16_t ointmap = daint >> 16;
        OSI_LOGV(0, "usb out ep int %x/%x", ointmap, udc->hw->daint);
        for (uint8_t i = 0; i < OEP_CNT; ++i)
        {
            if (ointmap & (1 << i))
                _epIsr(udc, &udc->oep[i]);
        }
    }

    if (gintsts.b.iepint)
    {
        OSI_LOGV(0, "usb in ep int %x/%x", (daint & 0xffff), udc->hw->daint);
        for (uint8_t i = 0; i < IEP_CNT; ++i)
        {
            if (daint & (1 << i))
                _epIsr(udc, &udc->iep[i]);
        }
    }

    return true;
}

static void _giveBackComplete(bootUdc_t *udc, udcEp_t *ep, udcXfer_t *x)
{
    if (x)
    {
        x->running = 0;
        if (x->xfer.complete)
            x->xfer.complete(&ep->ep, &x->xfer);
    }
    ep->cur_xfer = NULL;
}

static int8_t _txFifoAlloc(bootUdc_t *udc, udcEp_t *ep)
{
    REG_USBC_DIEPTXF1_T txf_1_2;
    REG_USBC_DIEPTXF2_T txf;
    int8_t best = -1;
    uint32_t best_fifo_size = 0xffffffff;
    REG32 *dieptxf = (REG32 *)&udc->hw->dieptxf1;
    for (uint16_t i = TX_FIFO_CNT; i > 0; --i)
    {
        if ((udc->tx_fifo_map & (1 << i)) == 0)
        {
            uint32_t fifo_size;
            if (i <= 2)
            {
                txf_1_2.v = dieptxf[i - 1];
                fifo_size = txf_1_2.b.inepntxfdep * 4;
            }
            else
            {
                txf.v = dieptxf[i - 1];
                fifo_size = txf.b.inepntxfdep * 4;
            }

            if (fifo_size >= ep->ep.mps && fifo_size < best_fifo_size)
            {
                best_fifo_size = fifo_size;
                best = i;
            }
        }
    }

    if (best != -1)
        udc->tx_fifo_map |= (1 << best);
    return best;
}

static void _txFifoFree(bootUdc_t *udc, udcEp_t *ep)
{
    if (ep->fifo_index == 0)
        return;
    udc->tx_fifo_map &= ~(1 << ep->fifo_index);
    ep->fifo_index = 0;
}

static inline void _rxFifoFlush(bootUdc_t *udc)
{
    REG_USBC_GRSTCTL_T grstctl = {};
    grstctl.b.rxfflsh = 1;
    udc->hw->grstctl = grstctl.v;
    do
    {
        grstctl.v = udc->hw->grstctl;
    } while (grstctl.b.rxfflsh == 1);
}

static inline void _txFifoFlush(bootUdc_t *udc, uint8_t index)
{
    REG_USBC_GRSTCTL_T grstctl = {};
    grstctl.b.txfnum = index;
    grstctl.b.txfflsh = 1;
    udc->hw->grstctl = grstctl.v;
    do
    {
        grstctl.v = udc->hw->grstctl;
    } while (grstctl.b.txfflsh == 1);
}

static inline void _txFifoFlushAll(bootUdc_t *udc)
{
    _txFifoFlush(udc, 0x10);
}

static void _epXferStop(bootUdc_t *udc, udcEp_t *ep)
{
    epReg_t *hep = ep->hwep;
    REG_USBC_DCTL_T dctl = {};
    usbcDiepctl_t epctl = {hep->epctl};
    if (!(epctl.b.epena))
        return;
    if (ep->ep.dirin)
    {
        epctl.v = 0;
        if (ep->periodic)
        {
            epctl.b.snak = 1;
            hep->epctl |= epctl.v;
        }
        else
        {
            dctl.b.sgnpinnak = 1;
            udc->hw->dctl = dctl.v;
        }
    }
    else
    {
        dctl.b.sgoutnak = 1;
        udc->hw->dctl |= dctl.v;
    }

    // set epdisable
    epctl.v = 0;
    epctl.b.epdis = 1;
    hep->epctl |= epctl.v;

    dctl.v = 0;
    if (ep->ep.dirin)
    {
        _txFifoFlush(udc, ep->fifo_index);
        if (udc->reserved_fifo == 0)
            _txFifoFree(udc, ep);

        if (!ep->periodic)
            dctl.b.cgnpinnak = 1;
    }
    else
    {
        dctl.b.cgoutnak = 1;
    }
    udc->hw->dctl |= dctl.v;
}

static uint32_t _epXferStart(bootUdc_t *udc, udcEp_t *ep, void *buf, uint32_t length)
{
    epReg_t *hep = ep->hwep;
    uint32_t epctl, eptsiz;
    if (buf == NULL)
        length = 0;
    if (length > ep->dma_max)
        length = ep->dma_max;

    uint32_t npkt = 1;
    if (length != 0)
        npkt = (length + ep->ep.mps - 1) / ep->ep.mps;

    if (ep->ep.dirin)
    {
        if (ep->ep.num == 0)
        {
            REG_USBC_DIEPTSIZ0_T dieptsiz0 = {};
            dieptsiz0.b.xfersize = length;
            dieptsiz0.b.pktcnt = npkt;
            eptsiz = dieptsiz0.v;
        }
        else
        {
            usbcDieptsiz_t dieptsiz = {};
            dieptsiz.b.xfersize = length;
            dieptsiz.b.pktcnt = npkt;
            if (ep->type == UE_ISOCHRONOUS)
                dieptsiz.b.mc = npkt;
            else
                dieptsiz.b.mc = 1;
            eptsiz = dieptsiz.v;
            if (udc->reserved_fifo == 0)
            {
                int8_t index = _txFifoAlloc(udc, ep);
                if (index < 0)
                {
                    OSI_LOGE(0, "fail to alloc tx fifo (ep %x)", ep->ep.address);
                    if (ep->periodic == 1)
                        osiPanic();
                    ep->fifo_index = 0;
                }
                else
                {
                    ep->fifo_index = index;
                }
            }
        }

        usbcDiepctl_t diepctl = {hep->epctl};
        diepctl.b.epdis = 0;
        diepctl.b.cnak = 1;
        diepctl.b.epena = 1;
        diepctl.b.txfnum = ep->fifo_index;
        epctl = diepctl.v;
    }
    else
    {
        if (ep->ep.num == 0)
        {
            REG_USBC_DOEPTSIZ0_T doeptsiz0 = {};
            doeptsiz0.b.xfersize = length;
            doeptsiz0.b.pktcnt = 1;
            eptsiz = doeptsiz0.v;
        }
        else
        {
            usbcDoeptsiz_t doeptsiz = {};
            doeptsiz.b.xfersize = length;
            doeptsiz.b.pktcnt = npkt;
            eptsiz = doeptsiz.v;
        }

        usbcDoepctl_t doepctl = {hep->epctl};
        if (!(ep->ep.num == 0 && udc->ep0_state == EP0_SETUP))
            doepctl.b.cnak = 1;
        doepctl.b.epena = 1;
        epctl = doepctl.v;
    }

    OSI_LOGV(0, "udc xfer start, %x/%x/%p/%u", epctl, eptsiz, buf, length);
    hep->eptsiz = eptsiz;
    if (buf && length != 0)
    {
        // TODO cache
        hep->epdma = (uint32_t)buf;
    }
    hep->epctl = epctl;
    return length;
}

bool bootUdcXferEnqueue(bootUdc_t *udc, bootUsbEp_t *ep_, bootUsbXfer_t *xfer_)
{
    if (udc == NULL || ep_ == NULL || xfer_ == NULL)
        return false;

    if (udc->connected == 0)
        return false;

    if (ep_->num == 0)
    {
        OSI_LOGE(0, "udc enqueue ep0 %x (TODO)", ep_->address);
        osiPanic(); // TODO
    }

    udcEp_t *ep = (udcEp_t *)ep_;
    if (ep->cur_xfer)
    {
        OSI_LOGE(0, "udc enqueue busy, ep %x", ep_->address);
        return false;
    }

    xfer_->status = -EINPROGRESS;
    xfer_->actual = 0;
    udcXfer_t *xfer = (udcXfer_t *)xfer_;
    if (ep->ep.dirin && xfer_->zlp)
        xfer->zero = (xfer_->length % ep_->mps) == 0 ? 1 : 0;

    xfer->running = 1;
    ep->cur_xfer = xfer;
    xfer->xfer_size = _epXferStart(udc, ep, xfer->xfer.buf, xfer->xfer.length);
    return true;
}

void bootUdcXferDequeue(bootUdc_t *udc, bootUsbEp_t *ep_, bootUsbXfer_t *xfer_)
{
    udcXfer_t *x = (udcXfer_t *)xfer_;
    if (x->running == 0)
    {
        OSI_LOGW(0, "udc dequeue with not running transfer, ep - %x", ep_->address);
        return;
    }

    udcEp_t *ep = (udcEp_t *)ep_;
    if (x != ep->cur_xfer)
    {
        OSI_LOGW(0, "udc dequeue xfer %p ep %x mismatch, ep->cur_xfer %p", x, ep_->address, ep->cur_xfer);
        return;
    }
    else
    {
        _epXferStop(udc, ep);
        ep->cur_xfer = NULL;
    }

    x->xfer.status = -ECANCELED;
    _giveBackComplete(udc, ep, x);
}

bootUsbXfer_t *bootUdcXferAlloc(bootUdc_t *udc, bootUsbEp_t *ep)
{
    udcXfer_t *x = (udcXfer_t *)malloc(sizeof(udcXfer_t));
    if (x == NULL)
    {
        OSI_LOGE(0, "boot udc fail allocate xfer");
        return NULL;
    }
    return &x->xfer;
}

void bootUdcXferFree(bootUdc_t *udc, bootUsbXfer_t *xfer)
{
    udcXfer_t *x = (udcXfer_t *)xfer;
    free(x);
}

#ifdef CONFIG_SOC_8910

bootUsbEp_t *bootUdcRomSerialInEp(bootUdc_t *udc)
{
    if (udc == NULL)
        return NULL;
    uint8_t num = ROM_SERIAL_IEP & 0x7f;
    return &udc->iep[num].ep;
}

bootUsbEp_t *bootUdcRomSerialOutEp(bootUdc_t *udc)
{
    if (udc == NULL)
        return NULL;
    return &udc->oep[ROM_SERIAL_OEP].ep;
}

#endif
