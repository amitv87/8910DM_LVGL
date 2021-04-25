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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('U', 'S', 'B', 'C')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/queue.h>

#include <hwregs.h>
#include <osi_api.h>
#include <osi_log.h>
#include <osi_compiler.h>
#include <usb/usb_device.h>

#include "udc_platform_config.h"
#include "usb_utils.h"

// #define FORCE_FULL_SPEED

#define DWC_EP_MPS_FULL (64)
#define DWC_EP_MPS_HIGH (512)
#define DWC_EP0_MPS (64)

#define ISRAM(n) (OSI_IS_IN_REGION(unsigned, n, CONFIG_RAM_PHY_ADDRESS, CONFIG_RAM_SIZE) || \
                  OSI_IS_IN_REGION(unsigned, n, CONFIG_SRAM_PHY_ADDRESS, CONFIG_SRAM_SIZE))

typedef REG_USBC_DIEPCTL1_T usbcDiepctl_t;
typedef REG_USBC_DIEPINT1_T usbcDiepint_t;
typedef REG_USBC_DIEPTSIZ1_T usbcDieptsiz_t;
typedef REG_USBC_DTXFSTS1_T usbcDtxfsts_t;

typedef REG_USBC_DOEPCTL1_T usbcDoepctl_t;
typedef REG_USBC_DOEPINT1_T usbcDoepint_t;
typedef REG_USBC_DOEPTSIZ1_T usbcDoeptsiz_t;

enum BURST_TYPE
{
    BURST_SINGLE = 0,
    BURST_INCR = 1,
    BURST_INCR4 = 3,
    BURST_INCR8 = 5,
    BURST_INCR16 = 7,
};

enum dwc_speed
{
    DWC_SPEED_HIGH_20 = 0,
    DWC_SPEED_FULL_20 = 1,
    DWC_SPEED_LOW_11 = 2,
    DWC_SPEED_FULL48_11 = 3,
};

enum dwc_state
{
    DWC_NORMAL = 0,
    DWC_SUSPEND,
    DWC_OFF,
};

typedef struct dwc_ep_regs
{
    REG32 epctl;
    REG32 reserved_1_io;
    REG32 epint;
    REG32 reserved_2_io;
    REG32 eptsiz;
    REG32 epdma;
    REG32 dtxfsts; // in ep only
    REG32 epdmab;
} dwcEpReg_t;

typedef TAILQ_ENTRY(dwc_xfer) dwc_xfer_entry_t;
typedef TAILQ_HEAD(, dwc_xfer) dwc_xfer_head_t;
typedef struct dwc_xfer
{
    usbXfer_t xfer; ///< keep first member
    uint32_t xfer_size;
    dwc_xfer_entry_t anchor;
    uint8_t zero : 1;
    uint8_t occupied : 1;
} dwcXfer_t;

typedef struct dwc_tx_fifo dwcTxFifo_t;
typedef TAILQ_ENTRY(dwc_tx_fifo) dwcTxFifoIter_t;
typedef TAILQ_HEAD(dwc_tx_fifo_head, dwc_tx_fifo) dwcTxFifoHead_t;
struct dwc_tx_fifo
{
    dwcTxFifoIter_t iter;
    uint16_t bytes;
    uint8_t index : 4;
    uint8_t using_ep : 4;
    uint8_t last_ep : 4;
};

typedef struct dwc_ep dwcEp_t;
typedef STAILQ_ENTRY(dwc_ep) dwcEpIter_t;
typedef STAILQ_HEAD(, dwc_ep) dwcEpHead_t;
struct dwc_ep
{
    usbEp_t ep;
    dwcEpReg_t *hep;
    uint8_t type : 2;
    uint8_t periodic : 1;
    uint8_t used : 1;
    uint8_t halted : 1;
    uint8_t dynamic_fifo : 1;
    uint8_t pending : 1;
    uint32_t dma_max;
    dwcTxFifo_t *fifo;
    dwcXfer_t *xfer_schedule;
    dwc_xfer_head_t xfer_q;
    dwcEpIter_t iter;
};

typedef struct
{
    udc_t *gadget;
    HWP_USBC_T *hw;
    dwcEp_t iep[UDC_IEP_COUNT + 1];
    dwcEp_t oep[UDC_OEP_COUNT + 1];
    uint8_t iep_count; ///< include ep0
    uint8_t oep_count; ///< include ep0
    uint8_t test_mode;
    uint8_t dedicated_fifo : 1; ///< reserved fifo for each enabled in endpoint
    uint8_t connected : 1;
    uint8_t disable_schedule : 1;
#define DWC_TXFIFO_COUNT (8)
    uint16_t tx_fifo_map;
    ep0State_t ep0_state;
    enum dwc_state pm_level;
    dwcXfer_t ep0_xfer;
    dwcXfer_t setup_xfer;
    dwcEpHead_t pending_eps;
    dwcTxFifoHead_t avail_txfifo;
    dwcTxFifo_t txfifo_room[DWC_TXFIFO_COUNT];
    uint8_t *ep0_buf;   ///< make sure 8 bytes is enough
    uint8_t *setup_buf; ///< make sure 8 bytes is enough
} dwcUdc_t;

static dwcUdc_t _the_controller = {};

#define EP2DWCEP(ep) (dwcEp_t *)(ep)

static inline dwcUdc_t *prvGadgetGetDwc(udc_t *udc)
{
    return (dwcUdc_t *)udc->controller_priv;
}

static inline dwcXfer_t *prvXferGetDwcXfer(usbXfer_t *xfer)
{
    return (dwcXfer_t *)xfer;
}

static inline dwcEp_t *prvAddressGetEndpoint(dwcUdc_t *dwc, uint8_t address)
{
    uint8_t num = UE_GET_ADDR(address);
    if (UE_GET_DIR(address) == UE_DIR_IN)
    {
        if (num > dwc->iep_count)
            return NULL;
        return &dwc->iep[num];
    }
    else
    {
        if (num > dwc->oep_count)
            return NULL;
        return &dwc->oep[num];
    }
}

static inline dwcEp_t *prvGetDwcEp0(dwcUdc_t *dwc)
{
    if (dwc->ep0_state & EP0_STATE_DIRIN_MASK)
        return &dwc->iep[0];
    else
        return &dwc->oep[0];
}

static inline void prvChangeEp0State(dwcUdc_t *dwc, ep0State_t st)
{
    dwc->ep0_state = st;
}

static inline void prvSetAddress(dwcUdc_t *dwc, uint8_t addr)
{
    REG_USBC_DCFG_T dcfg = {dwc->hw->dcfg};
    dcfg.b.devaddr = addr;
    dwc->hw->dcfg = dcfg.v;
}

static void prvQueueSetup(dwcUdc_t *dwc);
static void prvEp0CompleteCB(usbEp_t *ep_, usbXfer_t *xfer);
static int prvEpQueueLocked(dwcUdc_t *dwc, dwcEp_t *ep, dwcXfer_t *xfer);
static void prvEpStopLocked(dwcUdc_t *dwc, dwcEp_t *ep);
static void prvStallEp0(dwcUdc_t *dwc, dwcEp_t *ep);
static int prvStallEpLocked(dwcUdc_t *dwc, dwcEp_t *ep_, bool halt);
static void prvEpDisableLocked(dwcUdc_t *dwc, dwcEp_t *ep);
static void prvEpCancelXferInQueue(dwcEp_t *ep);
static inline void prvRxFifoFlush(dwcUdc_t *dwc);
static inline void prvTxFifoFlushAll(dwcUdc_t *dwc);
static void prvHwInit(dwcUdc_t *dwc, bool reset_int);
static void prvExitHibernation(dwcUdc_t *dwc);
static inline void prvTxFifoFreeLocked(dwcUdc_t *dwc, dwcEp_t *ep);

static void prvDisconnectISR(dwcUdc_t *dwc)
{
    uint32_t critical = osiEnterCritical();
    dwc->gadget->speed = USB_SPEED_VARIABLE;
    dwc->test_mode = 0;
    dwc->connected = 0;
    for (unsigned i = 0; i < dwc->iep_count; ++i)
    {
        prvEpDisableLocked(dwc, &dwc->iep[i]);
        osiExitCritical(critical);
        prvEpCancelXferInQueue(&dwc->iep[i]);
        critical = osiEnterCritical();
    }

    for (unsigned i = 0; i < dwc->oep_count; ++i)
    {
        prvEpDisableLocked(dwc, &dwc->oep[i]);
        osiExitCritical(critical);
        prvEpCancelXferInQueue(&dwc->oep[i]);
        critical = osiEnterCritical();
    }

    osiExitCritical(critical);
    udcDisconnect(dwc->gadget);
    prvRxFifoFlush(dwc);
    prvTxFifoFlushAll(dwc);
}

static inline void prvRxFifoFlush(dwcUdc_t *dwc)
{
    REG_USBC_GRSTCTL_T grstctl = {};
    grstctl.b.rxfflsh = 1;
    dwc->hw->grstctl = grstctl.v;
    REG_WAIT_FIELD_EQZ(grstctl, dwc->hw->grstctl, rxfflsh);
}

static inline void prvTxFifoFlush(dwcUdc_t *dwc, uint8_t num)
{
    REG_USBC_GRSTCTL_T grstctl = {};
    grstctl.b.txfnum = num;
    grstctl.b.txfflsh = 1;
    dwc->hw->grstctl = grstctl.v;
    REG_WAIT_FIELD_EQZ(grstctl, dwc->hw->grstctl, txfflsh);
}

static void prvEpXferStopLocked(dwcUdc_t *dwc, dwcEp_t *ep)
{
    dwcEpReg_t *hep = ep->hep;
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
            dwc->hw->dctl |= dctl.v;
        }
    }
    else
    {
        dctl.b.sgoutnak = 1;
        dwc->hw->dctl |= dctl.v;
    }

    // set epdisable
    epctl.v = 0;
    epctl.b.epdis = 1;
    epctl.b.snak = 1;
    hep->epctl |= epctl.v;

    // wait ep disable
    osiDelayUS(6);

    // clear ep disable
    usbcDiepint_t epint = {.b.epdisbld = 1};
    hep->epint = epint.v;

    dctl.v = 0;
    if (ep->ep.dirin)
    {
        if (ep->ep.num == 0)
        {
            prvTxFifoFlush(dwc, 0);
        }
        else if (ep->fifo)
        {
            prvTxFifoFlush(dwc, ep->fifo->index);
        }

        if (!ep->periodic)
            dctl.b.cgnpinnak = 1;

        if (ep->dynamic_fifo)
            prvTxFifoFreeLocked(dwc, ep);
    }
    else
    {
        dctl.b.cgoutnak = 1;
    }
    dwc->hw->dctl |= dctl.v;
}

static void prvEpProgramZlpLocked(dwcUdc_t *dwc, dwcEp_t *ep)
{
    usbcDieptsiz_t eptsiz = {};
    eptsiz.b.mc = 1;
    eptsiz.b.pktcnt = 1;
    eptsiz.b.xfersize = 0;

    usbcDiepctl_t epctl = {};
    epctl.b.usbactep = 1;
    epctl.b.cnak = 1;
    epctl.b.epena = 1;

    ep->hep->epdma = 0;
    ep->hep->eptsiz = eptsiz.v;
    ep->hep->epctl |= epctl.v;
}

static uint32_t prvEpXferStartLocked(dwcUdc_t *dwc, dwcEp_t *ep, void *buf, uint32_t length, bool cached)
{
    dwcEpReg_t *hep = ep->hep;
    uint32_t epctl, eptsiz;
    if (length == 0)
    {
        prvEpProgramZlpLocked(dwc, ep);
        return 0;
    }

    // these conditional judgements contribute a little time delay
    if (buf == NULL || !OSI_IS_ALIGNED(buf, 4) || !ISRAM(buf) || !ISRAM((uint32_t)buf + length - 1))
    {
        OSI_LOGE(0, "ep %x dma buf %p invalid", ep->ep.address, buf);
        osiPanic();
    }

    uint32_t npkt = (length + ep->ep.mps - 1) / ep->ep.mps;
    if (cached)
    {
        if (ep->ep.dirin)
            osiDCacheClean(buf, length);
        else
            osiDCacheInvalidate(buf, length);
    }

    if (ep->ep.dirin)
    {
        usbcDiepctl_t diepctl = {hep->epctl};
        if (ep->ep.num == 0)
        {
            REG_USBC_DIEPTSIZ0_T dieptsiz0 = {};
            dieptsiz0.b.xfersize = length;
            dieptsiz0.b.pktcnt = npkt;
            eptsiz = dieptsiz0.v;

            diepctl.b.txfnum = 0;
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

            diepctl.b.txfnum = ep->fifo->index;
        }

        diepctl.b.cnak = 1;
        diepctl.b.epena = 1;
        epctl = diepctl.v;
    }
    else
    {
        usbcDoepctl_t doepctl = {hep->epctl};
        if (!(ep->ep.num == 0 && dwc->ep0_state == EP0_SETUP))
            doepctl.b.cnak = 1;
        doepctl.b.epena = 1;
        epctl = doepctl.v;

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
    }

    hep->eptsiz = eptsiz;
    hep->epdma = (uint32_t)buf;
    hep->epctl = epctl;
    return length;
}

static void prvEp0Zlp(dwcUdc_t *dwc)
{
    uint32_t critical = osiEnterCritical();
    dwcEp_t *ep = NULL;
    if (dwc->ep0_state == EP0_STATUS_OUT)
        ep = &dwc->oep[0];
    else if (dwc->ep0_state == EP0_STATUS_IN)
        ep = &dwc->iep[0];

    if (ep == NULL)
    {
        OSI_LOGE(0, "ep0 zlp status %d mismatch", dwc->ep0_state);
        osiPanic();
    }
    else
    {
        prvEpProgramZlpLocked(dwc, ep);
    }
    osiExitCritical(critical);
}

/**
 * @brief process get status
 *
 * @return
 *      - (-1)  fail
 *      - 0     need to process by the driver
 *      - other complete
 */
static int prvProcessGetStatus(dwcUdc_t *dwc, const usb_device_request_t *ctrl)
{
    if (ctrl->wLength != 2)
        return -EINVAL;

    uint16_t reply;
    uint8_t ep_address;
    dwcEp_t *ep = NULL;
    switch (ctrl->bmRequestType & UT_RECIP_MASK)
    {
    case UT_RECIP_DEVICE:
        /*
        * bit 0 => self powered
        * bit 1 => remote wakeup
        */
        reply = cpu_to_le16(0b1);
        break;

    case UT_RECIP_INTERFACE:
        /* currently, the data result should be zero */
        reply = cpu_to_le16(0);
        break;

    case UT_RECIP_ENDPOINT:
        ep_address = le16_to_cpu(ctrl->wIndex);
        ep = prvAddressGetEndpoint(dwc, ep_address);
        if (!ep)
            return -ENOENT;
        reply = cpu_to_le16(ep->halted ? 1 : 0);
        break;

    default:
        return 0;
    }

    memcpy(dwc->ep0_buf, &reply, 2);

    uint32_t critical = osiEnterCritical();
    dwcXfer_t *dx = &dwc->ep0_xfer;
    dx->xfer.buf = dwc->ep0_buf;
    dx->xfer.length = 2;
    dx->xfer.param = dwc;
    dx->xfer.complete = NULL;

    prvEpQueueLocked(dwc, &dwc->iep[0], dx);
    osiExitCritical(critical);

    return 2;
}

/**
 * @brief process set/clear feature
 *
 * @return
 *      - (-1)  fail
 *      - 0     need process by the bind driver
 *      - other process complete
 */
static int prvProcessCtrlFeature(dwcUdc_t *dwc, const usb_device_request_t *ctrl)
{
    bool set = ctrl->bRequest == UR_SET_FEATURE;
    uint8_t recip = (ctrl->bmRequestType & UT_RECIP_MASK);
    uint16_t value = ctrl->wValue;
    uint16_t index = ctrl->wIndex;
    uint16_t length = ctrl->wLength;

    uint32_t critical;
    int ret;
    dwcEp_t *ep;
    switch (recip)
    {
    case UT_RECIP_DEVICE:
        switch (value)
        {
        case UF_TEST_MODE:
            if ((index & 0xff) != 0)
                return -EINVAL;
            if (!set)
                return -EINVAL;
            if (length != 0)
                return -EINVAL;

            dwc->test_mode = (index >> 8);
            return 1;

        case UF_DEVICE_REMOTE_WAKEUP:
            return 1;

        default:
            return -EINVAL;
        }
        break;

    case UT_RECIP_ENDPOINT:
        ep = prvAddressGetEndpoint(dwc, index);
        if (ep == NULL)
            return -ENOENT;

        critical = osiEnterCritical();
        ret = prvStallEpLocked(dwc, ep, (value != 0));
        if (ret == 0 && ep->halted)
        {
            prvEpStopLocked(dwc, ep);
            osiExitCritical(critical);
            prvEpCancelXferInQueue(ep);
            return 1;
        }
        osiExitCritical(critical);
        return ret < 0 ? ret : 1;

    default:
        return -EINVAL;
    }

    return -EINVAL;
}

static void prvProcessControl(dwcUdc_t *dwc, const usb_device_request_t *ctrl)
{
    ep0State_t next_state;
    if (ctrl->wLength == 0)
        next_state = EP0_STATUS_IN;
    else if (UT_GET_DIR(ctrl->bmRequestType) == UT_DIR_IN)
        next_state = EP0_DATA_IN;
    else
        next_state = EP0_DATA_OUT;
    prvChangeEp0State(dwc, next_state);
    dwcEp_t *ep0 = (dwc->ep0_state & EP0_STATE_DIRIN_MASK) ? &dwc->iep[0] : &dwc->oep[0];

    int r = 0;
    if ((ctrl->bmRequestType & UT_MASK) == UT_STANDARD)
    {
        switch (ctrl->bRequest)
        {
        case UR_SET_ADDRESS:
            OSI_LOGI(0, "usb set address %u", ctrl->wValue);
            dwc->connected = 1;
            prvSetAddress(dwc, ctrl->wValue);
            udcConnect(dwc->gadget);
            r = 1;
            break;

        case UR_GET_STATUS:
            r = prvProcessGetStatus(dwc, ctrl);
            break;

        case UR_CLEAR_FEATURE:
        case UR_SET_FEATURE:
            r = prvProcessCtrlFeature(dwc, ctrl);
            break;

        default:
            break;
        }
    }

    if (r == 0)
    {
        udevDrv_t *drv = udcGetDriver(dwc->gadget);
        if (drv)
            r = udevDriverSetup(drv, ctrl);
        else
            r = -1;
    }

    if (r < 0)
        goto stall;

    if (!(dwc->ep0_state & EP0_STATE_DATA_MASK))
        prvEp0Zlp(dwc);
    return;

stall:
    OSI_LOGE(0, "usb control fail, stall ep. (%x/%x/%x/%x/%x)",
             ctrl->bmRequestType, ctrl->bRequest, ctrl->wValue,
             ctrl->wIndex, ctrl->wLength);

    prvStallEp0(dwc, ep0);
}

static void prvEp0CompleteCB(usbEp_t *ep_, usbXfer_t *xfer)
{
    dwcUdc_t *dwc = (dwcUdc_t *)xfer->param;
    dwcEp_t *ep = EP2DWCEP(ep_);

    xfer->complete = NULL; // ep0 xfer is shared
    if (dwc->ep0_state != EP0_SETUP || ep != &dwc->oep[0])
    {
        OSI_LOGE(0, "ep0 complete failed, state mismatch or ep mismatch."
                    "(ep: %x, ep0_state: %d)",
                 ep->ep.address, dwc->ep0_state);
        return;
    }

    if (xfer->status != 0)
    {
        OSI_LOGE(0, "ep0 complete failed. (errno %d)", xfer->status);
        return;
    }

    if (xfer->actual == 8)
    {
        uint8_t *buf = (uint8_t *)xfer->buf;
        OSI_LOGD(0, "ep0 complete, %x %x %x %x %x %x %x %x",
                 buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
        prvProcessControl(dwc, (usb_device_request_t *)xfer->buf);
    }
    else
    {
        OSI_LOGW(0, "ep0 complete error, (transfer size %d)", xfer->actual);
        prvQueueSetup(dwc);
    }
}

static void prvDwcRmtWakeup_(dwcUdc_t *dwc)
{
    uint32_t critical = osiEnterCritical();
    if (dwc->pm_level == DWC_SUSPEND)
    {
        REG_USBC_DCTL_T dctl = {dwc->hw->dctl};
        dctl.b.rmtwkupsig = 1;
        dwc->hw->dctl = dctl.v;
        prvExitHibernation(dwc);
        dctl.b.rmtwkupsig = 0;
        dwc->hw->dctl = dctl.v;
        dwc->pm_level = DWC_NORMAL;
    }
    osiExitCritical(critical);
}

static void prvPendingEpLocked(dwcUdc_t *dwc, dwcEp_t *ep)
{
    if (ep->pending == 0)
    {
        ep->pending = 1;
        STAILQ_INSERT_TAIL(&dwc->pending_eps, ep, iter);
    }
}

// allocate tx fifo use LRU strategy
static bool prvTxFifoAllocateLocked(dwcUdc_t *dwc, dwcEp_t *ep)
{
    const dwcTxFifo_t *fifo = ep->fifo;
    if (fifo && fifo->using_ep == 0 && fifo->last_ep == ep->ep.num)
    {
        ep->fifo->using_ep = ep->ep.num;
        TAILQ_REMOVE(&dwc->avail_txfifo, ep->fifo, iter);
        return true;
    }
    else
    {
        dwcTxFifo_t *f, *tmp;
        // must traverse from tail (LRU)
        TAILQ_FOREACH_REVERSE_SAFE(f, &dwc->avail_txfifo, dwc_tx_fifo_head, iter, tmp)
        {
            if (f->bytes >= ep->ep.mps)
            {
                f->using_ep = ep->ep.num;
                f->last_ep = ep->ep.num;
                TAILQ_REMOVE(&dwc->avail_txfifo, f, iter);
                ep->fifo = f;
                return true;
            }
        }
    }
    return false;
}

static inline void prvTxFifoFreeLocked(dwcUdc_t *dwc, dwcEp_t *ep)
{
    // give back fifo to the queue head (LRU), but do not set
    // ep->fifo to NULL and not set fifo->last_ep for quick allocating
    if (ep->fifo)
    {
        ep->fifo->using_ep = 0;
        TAILQ_INSERT_HEAD(&dwc->avail_txfifo, ep->fifo, iter);
    }
}

static void prvScheduleEpLocked(dwcUdc_t *dwc, dwcEp_t *ep)
{
    if (ep->xfer_schedule)
        return;

    dwcXfer_t *dx = TAILQ_FIRST(&ep->xfer_q);
    if (dx == NULL)
        return;

    if (dwc->disable_schedule && ep->ep.num != 0)
    {
        prvPendingEpLocked(dwc, ep);
        return;
    }

    if (ep->dynamic_fifo)
    {
        if (!prvTxFifoAllocateLocked(dwc, ep))
        {
            OSI_LOGW(0, "EP %x/%u fail to allocate fifo, pending", ep->ep.address, ep->type);
            prvPendingEpLocked(dwc, ep);
            return;
        }
        OSI_LOGD(0, "EP %x/%u use fifo %u", ep->ep.address, ep->type, ep->fifo->index);
    }

    if (dwc->pm_level == DWC_SUSPEND && ep->ep.dirin && (dwc->gadget->feature & UDC_FEATURE_WAKEUP_ON_WRITE))
        prvDwcRmtWakeup_(dwc);

    TAILQ_REMOVE(&ep->xfer_q, dx, anchor);
    ep->xfer_schedule = dx;

    uint32_t size = dx->xfer.length;
    if (size > ep->dma_max)
        size = ep->dma_max;

    dx->xfer_size = prvEpXferStartLocked(dwc, ep, dx->xfer.buf, size, !dx->xfer.uncached);
}

static int prvEpQueueLocked(dwcUdc_t *dwc, dwcEp_t *ep, dwcXfer_t *dx)
{
    if (OSI_UNLIKELY(dx->occupied))
    {
        OSI_LOGW(0, "ep queue busy. (ep: %x, ep0_state: %d)", ep->ep.address, dwc->ep0_state);
        return -EBUSY;
    }

    if (ep->ep.dirin && dx->xfer.zlp)
        dx->zero = (dx->xfer.length % ep->ep.mps) == 0 ? 1 : 0;

    dx->xfer.status = -EINPROGRESS;
    dx->xfer.actual = 0;
    dx->occupied = 1;

    TAILQ_INSERT_TAIL(&ep->xfer_q, dx, anchor);
    prvScheduleEpLocked(dwc, ep);

    return 0;
}

static void prvQueueSetup(dwcUdc_t *dwc)
{
    uint32_t critical = osiEnterCritical();
    prvChangeEp0State(dwc, EP0_SETUP);

    dwcXfer_t *dx = &dwc->setup_xfer;
    dx->xfer.buf = dwc->setup_buf;
    dx->xfer.length = 64; // more space for back2back package
    dx->xfer.param = dwc;
    dx->xfer.complete = prvEp0CompleteCB;

    prvEpQueueLocked(dwc, &dwc->oep[0], dx);
    osiExitCritical(critical);
}

static void prvStallEp0(dwcUdc_t *dwc, dwcEp_t *ep)
{
    // these two bits are both for in/out
    usbcDiepctl_t epctl = {};
    epctl.b.stall = 1;
    epctl.b.cnak = 1;
    ep->hep->epctl |= epctl.v;

    prvQueueSetup(dwc);
}

static int prvStallEpLocked(dwcUdc_t *dwc, dwcEp_t *ep, bool halt)
{
    if (ep->ep.num == 0)
    {
        if (halt) // can not clear ep0
            prvStallEp0(dwc, ep);

        return 0;
    }

    if (ep->type == UE_ISOCHRONOUS)
        return -EINVAL;

    uint8_t type;
    if (ep->ep.dirin)
    {
        usbcDiepctl_t epctl = {ep->hep->epctl};
        if (halt)
        {
            epctl.b.stall = 1;
            epctl.b.snak = 1;
            if (epctl.b.epena)
                epctl.b.epdis = 1;
        }
        else
        {
            epctl.b.stall = 0;
            type = epctl.b.eptype;
            if (type == UE_BULK || type == UE_INTERRUPT)
                epctl.b.setd0pid = 1;
        }
        ep->hep->epctl = epctl.v;
    }
    else
    {
        usbcDoepctl_t epctl = {ep->hep->epctl};
        if (halt)
            epctl.b.stall = 1;
        else
        {
            epctl.b.stall = 0;
            type = epctl.b.eptype;
            if (type == UE_BULK || type == UE_INTERRUPT)
                epctl.b.setd0pid = 1;
        }
        ep->hep->epctl = epctl.v;
    }

    ep->halted = halt;
    return 0;
}

static void prvDwcEpHalt(udc_t *udc, usbEp_t *ep_, bool halt)
{
    uint32_t critical = osiEnterCritical();
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    dwcEp_t *ep = EP2DWCEP(ep_);
    prvStallEpLocked(dwc, ep, halt);
    osiExitCritical(critical);
}

static void prvSetEpMps(dwcUdc_t *dwc, dwcEp_t *ep, uint16_t mps)
{
    // max_dma_size = min(mps * max(EPTSIZ->npkt), max(EPTSIZE->xferSize))
    uint32_t max_dma_size = 0xffc0;
    uint16_t max_pkt_size = mps;
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
        }
        if (ep->ep.dirin)
            max_dma_size = 0x7f;
        else
            max_dma_size = 0x40;
    }

    max_dma_size -= (max_dma_size % max_pkt_size);

    // EPn mps bits is enough for EP0
    // Ep out and Ep in have the same mps bits
    ep->ep.mps = max_pkt_size;
    ep->dma_max = max_dma_size;

    usbcDiepctl_t epctl = {ep->hep->epctl};
    epctl.b.mps = mps;
    ep->hep->epctl = epctl.v;
}

static void prvEpCancelXferInQueue(dwcEp_t *ep)
{
    uint32_t critical = osiEnterCritical();
    dwcXfer_t *dx;
    while ((dx = TAILQ_FIRST(&ep->xfer_q)))
    {
        TAILQ_REMOVE(&ep->xfer_q, dx, anchor);
        dx->occupied = 0;
        usbXfer_t *x = &dx->xfer;
        x->status = -ECANCELED;
        osiExitCritical(critical);
        if (x->complete)
            x->complete(&ep->ep, x);
        critical = osiEnterCritical();
    }
    osiExitCritical(critical);
}

static void prvEpStopLocked(dwcUdc_t *dwc, dwcEp_t *ep)
{
    usbcDiepctl_t epctl = {ep->hep->epctl};
    if (epctl.b.epena)
        prvEpXferStopLocked(dwc, ep);

    if (ep->xfer_schedule)
    {
        TAILQ_INSERT_HEAD(&ep->xfer_q, ep->xfer_schedule, anchor);
        ep->xfer_schedule = NULL;
    }
}

static void prvEpDisableLocked(dwcUdc_t *dwc, dwcEp_t *ep)
{
    if (ep->ep.dirin)
        dwc->hw->daintmsk &= ~(1 << ep->ep.num);
    else
        dwc->hw->daintmsk &= ~((1 << ep->ep.num) << 16);
    prvEpStopLocked(dwc, ep);
    // always free in ep tx fifo
    if (ep->ep.dirin)
    {
        prvTxFifoFreeLocked(dwc, ep);
        ep->fifo = NULL;
    }

    if (ep->ep.num != 0)
    {
        usbcDiepctl_t epctl = {ep->hep->epctl};
        epctl.b.usbactep = 0;
        epctl.b.eptype = 0;
        epctl.b.snak = 1;
        ep->hep->epctl = epctl.v;
    }
}

static int prvEpEnableLocked(dwcUdc_t *dwc, dwcEp_t *ep)
{
    dwcEpReg_t *hep = ep->hep;
    usbcDiepctl_t epctl = {hep->epctl};
    if (!epctl.b.usbactep)
    {
        if (ep->dynamic_fifo == 0 && ep->ep.dirin)
        {
            uint32_t critical = osiEnterCritical();
            if (!prvTxFifoAllocateLocked(dwc, ep))
            {
                OSI_LOGE(0, "EP %x/%u fail allocate txfifo", ep->ep.address, ep->type);
                osiPanic();
                return -1;
            }
            osiExitCritical(critical);
            OSI_LOGI(0, "EP %x/%u allocate fifo %d", ep->ep.address, ep->type, ep->fifo->index);
        }
        epctl.b.usbactep = 1;
        epctl.b.eptype = ep->type;
        epctl.b.epdis = 0;
        hep->epctl = epctl.v;
    }

    if (ep->ep.dirin)
        dwc->hw->daintmsk |= (1 << ep->ep.num);
    else
        dwc->hw->daintmsk |= ((1 << ep->ep.num) << 16);

    return 0;
}

static void prvEnumdoneISR(dwcUdc_t *dwc)
{
    uint32_t critical = osiEnterCritical();
    udc_t *udc = dwc->gadget;
    REG_USBC_DSTS_T dsts = {dwc->hw->dsts};
    uint16_t ep0_mps = 0;
    uint16_t ep_mps = 8;

    switch (dsts.b.enumspd)
    {
    case DWC_SPEED_FULL_20:
    case DWC_SPEED_FULL48_11:
        ep0_mps = DWC_EP0_MPS;
        ep_mps = DWC_EP_MPS_FULL;
        udc->speed = USB_SPEED_FULL;
        break;

    case DWC_SPEED_LOW_11:
        ep0_mps = 8;
        ep_mps = 8;
        udc->speed = USB_SPEED_LOW;
        break;

    case DWC_SPEED_HIGH_20:
        ep0_mps = DWC_EP0_MPS;
        ep_mps = DWC_EP_MPS_HIGH;
        udc->speed = USB_SPEED_HIGH;
        break;
    }

    OSI_LOGI(0, "enumdone speed %u", udc->speed);

    prvSetEpMps(dwc, &dwc->iep[0], ep0_mps);
    prvSetEpMps(dwc, &dwc->oep[0], ep0_mps);
    for (uint8_t i = 1; i < dwc->iep_count; ++i)
        prvSetEpMps(dwc, &dwc->iep[i], ep_mps);
    for (uint8_t i = 1; i < dwc->oep_count; ++i)
        prvSetEpMps(dwc, &dwc->oep[i], ep_mps);

    prvRxFifoFlush(dwc);
    prvTxFifoFlushAll(dwc);

    dwc->iep[0].used = 1;
    dwc->oep[0].used = 1;
    prvEpEnableLocked(dwc, &dwc->iep[0]);
    prvEpEnableLocked(dwc, &dwc->oep[0]);
    prvQueueSetup(dwc);
    osiExitCritical(critical);
}

static inline void prvGiveBackComplete_(dwcUdc_t *dwc, dwcEp_t *ep, dwcXfer_t *dx, bool schedule)
{
    uint32_t critical = osiEnterCritical();
    if (dx)
    {
        dx->occupied = 0;
        usbXfer_t *xfer = &dx->xfer;
        osiExitCritical(critical);
        if (xfer->complete)
            xfer->complete(&ep->ep, xfer);
        critical = osiEnterCritical();
    }

    if (schedule)
    {
        prvScheduleEpLocked(dwc, ep);
    }
    osiExitCritical(critical);
}

static inline void prvGiveBackComplete(dwcUdc_t *dwc, dwcEp_t *ep, dwcXfer_t *dx)
{
    return prvGiveBackComplete_(dwc, ep, dx, true);
}

static inline void prvGiveBackEpCurrent(dwcUdc_t *dwc, dwcEp_t *ep)
{
    uint32_t critical = osiEnterCritical();
    dwcXfer_t *dx = ep->xfer_schedule;
    ep->xfer_schedule = NULL;
    if (!ep->ep.dirin && !dx->xfer.uncached)
        osiDCacheInvalidate(dx->xfer.buf, dx->xfer.actual);
    osiExitCritical(critical);
    prvGiveBackComplete(dwc, ep, dx);
}

static int prvSetTestMode(dwcUdc_t *dwc, uint8_t test_mode)
{
    REG_USBC_DCTL_T dctl = {dwc->hw->dctl};
    switch (test_mode)
    {
    case TEST_J:
    case TEST_K:
    case TEST_SE0_NAK:
    case TEST_PACKET:
    case TEST_FORCE_EN:
        dctl.b.tstctl = test_mode;
        break;

    default:
        return -EINVAL;
    }

    dwc->hw->dctl |= dctl.v;
    return 0;
}

static void prvEpInDoneISR(dwcUdc_t *dwc, dwcEp_t *ep)
{
    uint32_t size_left;
    uint32_t critical = osiEnterCritical();
    if (ep->ep.num == 0)
    {
        // complete the ctrl transfer
        if (dwc->ep0_state == EP0_STATUS_IN)
        {
            osiExitCritical(critical);
            // last packet must be out or setup (if setup dx must be NULL)
            dwcEp_t *ep0_out = &dwc->oep[0];
            if (ep0_out->xfer_schedule)
                prvGiveBackEpCurrent(dwc, ep0_out);

            if (dwc->test_mode)
            {
                int r = prvSetTestMode(dwc, dwc->test_mode);
                if (r < 0)
                {
                    OSI_LOGE(0, "set usb test mode fail");
                    prvStallEp0(dwc, &dwc->oep[0]);
                    return;
                }
            }

            prvQueueSetup(dwc);
            return;
        }

        if (OSI_UNLIKELY(!(dwc->ep0_state & EP0_STATE_DIRIN_MASK)))
        {
            osiExitCritical(critical);
            // must be something wrong
            OSI_LOGW(0, "ep0 in done, state mismatch (%d)", dwc->ep0_state);
            return;
        }
        size_left = REGTYPE_FIELD_GET(REG_USBC_DIEPTSIZ0_T, ep->hep->eptsiz, xfersize);
    }
    else
    {
        size_left = REGTYPE_FIELD_GET(usbcDieptsiz_t, ep->hep->eptsiz, xfersize);
    }

    dwcXfer_t *dx = ep->xfer_schedule;
    if (dx == NULL)
    {
        osiExitCritical(critical);
        OSI_LOGE(0, "ep %x done no transfer. (ctrl stage %d)", ep->ep.address, dwc->ep0_state);
        osiPanic();
    }

    if (size_left > dx->xfer_size)
    {
        OSI_LOGE(0, "ep%x xfer size overflow, reg size left %u, xfer size %u. (ctrl stage %d)",
                 ep->ep.num, size_left, dx->xfer_size, dwc->ep0_state);
        osiPanic();
    }

    uint32_t size_done = dx->xfer_size - size_left;
    dx->xfer.actual += size_done;

    bool transfer_done = (dx->xfer.actual == dx->xfer.length);
    if (!transfer_done)
    {
        uint32_t size = dx->xfer.length - dx->xfer.actual;
        if (size > ep->dma_max)
            size = ep->dma_max;
        dx->xfer_size = prvEpXferStartLocked(dwc, ep, dx->xfer.buf + dx->xfer.actual, size, !dx->xfer.uncached);
    }
    else
    {
        if (ep->dynamic_fifo)
            prvTxFifoFreeLocked(dwc, ep);

        if (dx->zero)
        {
            dx->zero = 0;
            dx->xfer_size = 0;
            prvEpProgramZlpLocked(dwc, ep);
        }
        else
        {
            dx->xfer.status = 0;
            dx->xfer_size = 0;
            if (ep->ep.num == 0 && dwc->ep0_state == EP0_DATA_IN)
            {
                prvChangeEp0State(dwc, EP0_STATUS_OUT);
                prvEp0Zlp(dwc);
            }
            else
            {
                osiExitCritical(critical);
                prvGiveBackEpCurrent(dwc, ep);
                return;
            }
        }
    }
    osiExitCritical(critical);
}

static void prvEpOutDoneISR(dwcUdc_t *dwc, dwcEp_t *ep)
{
    uint32_t size_left;
    uint32_t critical = osiEnterCritical();
    if (ep->ep.num == 0)
    {
        if (dwc->ep0_state == EP0_STATUS_OUT)
        {
            osiExitCritical(critical);
            // last packet must be in
            dwcEp_t *ep0_in = &dwc->iep[0];
            if (ep0_in->xfer_schedule)
                prvGiveBackEpCurrent(dwc, ep0_in);

            prvQueueSetup(dwc);
            return;
        }

        if (OSI_UNLIKELY(dwc->ep0_state & EP0_STATE_DIRIN_MASK))
        {
            osiExitCritical(critical);
            // must be something wrong
            OSI_LOGW(0, "ep0 out done, state mismatch (%d)", dwc->ep0_state);
            return;
        }
        size_left = REGTYPE_FIELD_GET(REG_USBC_DOEPTSIZ0_T, ep->hep->eptsiz, xfersize);
    }
    else
    {
        size_left = REGTYPE_FIELD_GET(usbcDoeptsiz_t, ep->hep->eptsiz, xfersize);
    }

    dwcXfer_t *dx = ep->xfer_schedule;
    if (dx == NULL)
    {
        osiExitCritical(critical);
        OSI_LOGE(0, "ep %x done no transfer. (ctrl stage %d)", ep->ep.address, dwc->ep0_state);
        return;
    }

    if (size_left > dx->xfer_size)
    {
        OSI_LOGE(0, "ep%x xfer size overflow, reg size left %u, xfer size %u. (ctrl stage %d)",
                 ep->ep.num, size_left, dx->xfer_size, dwc->ep0_state);
        osiPanic();
    }

    uint32_t size_done = dx->xfer_size - size_left;
    dx->xfer.actual += size_done;

    bool transfer_done = dx->xfer.length == dx->xfer.actual || size_left != 0;
    if (!transfer_done)
    {
        uint32_t size = dx->xfer.length - dx->xfer.actual;
        if (size > ep->dma_max)
            size = ep->dma_max;
        dx->xfer_size = prvEpXferStartLocked(dwc, ep, dx->xfer.buf + dx->xfer.actual, size, !dx->xfer.uncached);
    }
    else
    {
        dx->xfer.status = 0;
        dx->xfer_size = 0;
        if (ep->ep.num == 0 && dwc->ep0_state == EP0_DATA_OUT)
        {
            prvChangeEp0State(dwc, EP0_STATUS_IN);
            prvEp0Zlp(dwc);
        }
        else
        {
            osiExitCritical(critical);
            prvGiveBackEpCurrent(dwc, ep);
            return;
        }
    }
    osiExitCritical(critical);
}

static void prvEpISR(dwcUdc_t *dwc, dwcEp_t *ep)
{
    uint32_t epint = ep->hep->epint;
    if (!(ep->hep->epctl & REGTYPE_FIELD_MASK(usbcDiepctl_t, usbactep)))
    {
        ep->hep->epint = epint;
        OSI_LOGW(0, "ep %x intr %x, not actep", ep->ep.address, epint);
        return;
    }

    if (ep->ep.dirin)
    {
        epint &= dwc->hw->diepmsk;
        ep->hep->epint = epint;
        if (epint & REGTYPE_FIELD_MASK(usbcDiepint_t, xfercompl))
        {
            prvEpInDoneISR(dwc, ep);
            return;
        }

        if (epint & REGTYPE_FIELD_MASK(usbcDiepint_t, timeout))
        {
            OSI_LOGW(0, "ep %x timeout", ep->ep.address);
        }
    }
    else
    {
        epint &= (dwc->hw->doepmsk | REGTYPE_FIELD_MASK(usbcDoepint_t, stuppktrcvd));
        ep->hep->epint = epint;

        usbcDoepint_t doepint = {epint};
        if (ep->ep.num == 0 && (doepint.b.setup || doepint.b.stuppktrcvd))
        {
            doepint.b.xfercompl = 0;
        }

        if (doepint.b.xfercompl)
        {
            prvEpOutDoneISR(dwc, ep);
            return;
        }

        if (ep->ep.num == 0 && doepint.b.setup)
        {
            if (OSI_UNLIKELY(dwc->ep0_state != EP0_SETUP))
            {
                OSI_LOGW(0, "out isr setup invalid stage %d", dwc->ep0_state);
            }
            prvEpOutDoneISR(dwc, ep);
            return;
        }

        if (doepint.b.stsphsercvd)
        {
            OSI_LOGD(0, "ep %x stsphsercvd", ep->ep.address);
        }
    }

    if (epint & REGTYPE_FIELD_MASK(usbcDiepint_t, ahberr))
    {
        OSI_LOGW(0, "ep %x ahberr", ep->ep.address);
    }

    if (epint & REGTYPE_FIELD_MASK(usbcDiepint_t, txfifoundrn))
    {
        OSI_LOGW(0, "ep %x txfifoundrn/outpkterrmsk", ep->ep.address);
    }
}

static void prvEnterHibernation(dwcUdc_t *dwc)
{
    // clear any pending interrupts, since dwc2 will not able to
    // clear them after entering hibernation
    dwc->hw->gintsts = 0xffffffff;

    osiDelayUS(10);

    REG_USBC_PCGCCTL_T pcgcctl = {dwc->hw->pcgcctl};
    pcgcctl.b.stoppclk = 1;
    dwc->hw->pcgcctl = pcgcctl.v;

    udcPlatSuspend(dwc->gadget);
}

static void prvExitHibernation(dwcUdc_t *dwc)
{
    udcPlatResume(dwc->gadget);

    REG_USBC_PCGCCTL_T pcgcctl = {dwc->hw->pcgcctl};
    pcgcctl.b.stoppclk = 0;
    dwc->hw->pcgcctl = pcgcctl.v;

    osiDelayUS(10);
}

static inline void prvSuspendISR(dwcUdc_t *dwc)
{
    dwc->pm_level = DWC_SUSPEND;
    prvEnterHibernation(dwc);
}

static inline void prvResumeISR(dwcUdc_t *dwc)
{
    prvExitHibernation(dwc);
    dwc->pm_level = DWC_NORMAL;
}

static void prvDwcISR(void *p)
{
    dwcUdc_t *dwc = (dwcUdc_t *)p;
    dwcEp_t *ep;
    uint32_t critical;
    bool transfer_ready = true;
    unsigned retry_cnt = 3;
    REG_USBC_GINTSTS_T gintsts = {dwc->hw->gintsts};

    // check device mode
    if (gintsts.b.curmod == 1)
        return;

isr_retry:
    dwc->hw->gintsts = gintsts.v;
    gintsts.v &= dwc->hw->gintmsk;

    if (gintsts.b.usbrst || gintsts.b.resetdet)
    {
        bool connect = dwc->connected == 1;
        OSI_LOGI(0, "usb irq, usbrst %d/%x/%x", connect, dwc->hw->gotgctl, gintsts.v);
        if (dwc->pm_level == DWC_SUSPEND)
        {
            prvExitHibernation(dwc);
            dwc->pm_level = DWC_NORMAL;
        }

        gintsts.b.usbsusp = 0; // ignore suspend if reset
        prvDisconnectISR(dwc);
        transfer_ready = false;
        REG_USBC_GOTGCTL_T otgctl = {dwc->hw->gotgctl};
        if (connect && otgctl.b.bsesvld)
        {
            OSI_LOGE(0, "usb irq, usbrst when connect %x", gintsts.v);
            prvHwInit(dwc, true);
            dwc->pm_level = DWC_NORMAL;
            return;
        }
    }

    if (gintsts.b.enumdone)
    {
        OSI_LOGI(0, "usb irq, enum done");
        prvSetAddress(dwc, 0);
        prvEnumdoneISR(dwc);
        transfer_ready = true;
    }

    if (gintsts.b.usbsusp)
    {
        OSI_LOGI(0, "usb irq, usbsusp, %d/%x", dwc->connected, dwc->hw->dsts);
        if (!!dwc->connected)
            prvSuspendISR(dwc);
        transfer_ready = false;
        return;
    }

    if (gintsts.b.wkupint)
    {
        OSI_LOGI(0, "usb irq, wkupint, %x/%x", dwc->hw->pcgcctl, dwc->hw->dsts);
        if (dwc->pm_level == DWC_SUSPEND)
        {
            prvResumeISR(dwc);
        }
        else
        {
            // abnormal state, reset the controller
            prvHwInit(dwc, false);
            return;
        }
        transfer_ready = true;
    }

    dwc->disable_schedule = 1;
    // do not process endpoints if udc may be disconnected
    if (OSI_LIKELY(transfer_ready))
    {
        uint32_t daint = (dwc->hw->daint & dwc->hw->daintmsk);
        if (daint & 0x1) // IEP0
            prvEpISR(dwc, &dwc->iep[0]);

        if (daint & 0x10000) // OEP0
            prvEpISR(dwc, &dwc->oep[0]);

        if (daint & 0xfffe) // IEPx
        {
            for (uint8_t i = 1; i < dwc->iep_count; ++i)
            {
                if (daint & (1 << i))
                    prvEpISR(dwc, &dwc->iep[i]);
            }
        }

        if (daint & 0xfffe0000) // OEPx
        {
            uint16_t ointmap = (daint >> 16);
            for (uint8_t i = 1; i < dwc->oep_count; ++i)
            {
                if (ointmap & (1 << i))
                    prvEpISR(dwc, &dwc->oep[i]);
            }
        }
    }

    critical = osiEnterCritical();
    dwc->disable_schedule = 0;
    while ((ep = STAILQ_FIRST(&dwc->pending_eps)) != NULL)
    {
        ep->pending = 0;
        STAILQ_REMOVE_HEAD(&dwc->pending_eps, iter);
        prvScheduleEpLocked(dwc, ep);
        osiExitCritical(critical);
        OSI_NOP; // a while
        critical = osiEnterCritical();
    }
    osiExitCritical(critical);

    if (gintsts.b.sessreqint)
    {
        OSI_LOGE(0, "usb irq, sessreqint");
    }

    if (gintsts.b.fetsusp)
    {
        OSI_LOGE(0, "usb irq, fetsusp");
    }

    gintsts.v = dwc->hw->gintsts;
    if ((gintsts.v & dwc->hw->gintmsk) && retry_cnt-- > 0)
        goto isr_retry;
}

static bool prvDwcRmtWakeup(udc_t *udc)
{
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    prvDwcRmtWakeup_(dwc);
    return true;
}

static inline void prvTxFifoFlushAll(dwcUdc_t *dwc)
{
    prvTxFifoFlush(dwc, 0x10);
}

static void prvHwFifoInit(dwcUdc_t *dwc)
{
    const uint16_t rx_fifo_sz = 1024;
    const uint16_t np_tx_fifo_sz = 256;
    uint16_t start_addr = rx_fifo_sz + np_tx_fifo_sz;

    REG_USBC_GRXFSIZ_T grxfsiz;
    grxfsiz.b.rxfdep = rx_fifo_sz;
    dwc->hw->grxfsiz = grxfsiz.v;

    REG_USBC_GNPTXFSIZ_T gnptxfsiz;
    gnptxfsiz.b.nptxfstaddr = rx_fifo_sz;
    gnptxfsiz.b.nptxfdep = np_tx_fifo_sz;
    dwc->hw->gnptxfsiz = gnptxfsiz.v;

    TAILQ_INIT(&dwc->avail_txfifo);
    dwcTxFifo_t *txfifo = &dwc->txfifo_room[0];
    const uint16_t ptx_fifo_1_2_sz = 256;
    const uint16_t ptx_fifo_sz = 256;
    REG32 *dieptxf = (REG32 *)&dwc->hw->dieptxf1;
    REG_USBC_DIEPTXF1_T txf_1_2;
    for (uint8_t i = 0; i < 2; ++i)
    {
        txf_1_2.b.inepntxfdep = ptx_fifo_1_2_sz;
        txf_1_2.b.inepntxfstaddr = start_addr;
        dieptxf[i] = txf_1_2.v;
        start_addr += ptx_fifo_1_2_sz;

        txfifo->index = i + 1;
        txfifo->bytes = ptx_fifo_1_2_sz * 4;
        txfifo->using_ep = 0;
        txfifo->last_ep = 0;
        TAILQ_INSERT_TAIL(&dwc->avail_txfifo, txfifo, iter);
        txfifo++;
    }

    dieptxf = (REG32 *)&dwc->hw->dieptxf3;
    REG_USBC_DIEPTXF3_T txf;
    for (uint8_t i = 0; i < 6; ++i)
    {
        txf.b.inepntxfdep = ptx_fifo_sz;
        txf.b.inepntxfstaddr = start_addr;
        dieptxf[i] = txf.v;
        start_addr += ptx_fifo_sz;

        txfifo->index = i + 3;
        txfifo->bytes = ptx_fifo_sz * 4;
        txfifo->using_ep = 0;
        txfifo->last_ep = 0;
        TAILQ_INSERT_TAIL(&dwc->avail_txfifo, txfifo, iter);
        txfifo++;
    }

    REG_USBC_GDFIFOCFG_T OSI_UNUSED gdfifocfg = {dwc->hw->gdfifocfg};
    OSI_ASSERT(start_addr < gdfifocfg.b.gdfifocfg, "hw fifo config overflow");

    REG_USBC_GRSTCTL_T grstctl = {.b.ahbidle = 1};
    dwc->hw->grstctl = grstctl.v;
    grstctl.b.ahbidle = 0;
    grstctl.b.txfnum = 0x10; // flush all
    grstctl.b.txfflsh = 1;
    grstctl.b.rxfflsh = 1;
    dwc->hw->grstctl = grstctl.v;
    while (grstctl.b.txfflsh || grstctl.b.rxfflsh)
    {
        grstctl.v = dwc->hw->grstctl;
    }
}

static void inline prvGlobalSoftReset(dwcUdc_t *dwc)
{
    REG_USBC_GRSTCTL_T grstctl = {dwc->hw->grstctl};
    grstctl.b.csftrst = 1;
    dwc->hw->grstctl = grstctl.v;

    REG_WAIT_FIELD_EQZ(grstctl, dwc->hw->grstctl, csftrst);
    REG_WAIT_FIELD_NEZ(grstctl, dwc->hw->grstctl, ahbidle);
}

static void prvHwInit(dwcUdc_t *dwc, bool reset_int)
{
    dwc->hw->pcgcctl = 0;

    // global reset the usb controller
    if (!reset_int)
        prvGlobalSoftReset(dwc);

    REG_USBC_GAHBCFG_T gahbcfg = {};
    dwc->hw->gahbcfg = gahbcfg.v;

    REG_USBC_DCFG_T dcfg = {};
#ifdef FORCE_FULL_SPEED
    dcfg.b.devspd = DWC_SPEED_FULL_20;
#else
    dcfg.b.devspd = DWC_SPEED_HIGH_20;
#endif
    dwc->hw->dcfg = dcfg.v;

    if (!reset_int)
    {
        // soft disconnect to the host
        REG_USBC_DCTL_T dctl = {dwc->hw->dctl};
        dctl.b.sftdiscon = 1;
        dwc->hw->dctl = dctl.v;

        osiDelayUS(100);

        dctl.b.sftdiscon = 0;
        dwc->hw->dctl = dctl.v;

        prvGlobalSoftReset(dwc);
    }

    REG_USBC_GUSBCFG_T gusbcfg = {};
    gusbcfg.b.forcedevmode = 1;
    gusbcfg.b.toutcal = 7;
    gusbcfg.b.srpcap = 0;
    gusbcfg.b.hnpcap = 0;
    gusbcfg.b.phyif = 0;
    gusbcfg.b.usbtrdtim = 9;
    dwc->hw->gusbcfg = gusbcfg.v;

    prvHwFifoInit(dwc);

    dwc->hw->gotgint = 0xffffffff;

    REG_USBC_GINTMSK_T gintmsk = {};
    gintmsk.b.oepintmsk = 1;
    gintmsk.b.iepintmsk = 1;
    gintmsk.b.wkupintmsk = 1;
    gintmsk.b.enumdonemsk = 1;
    gintmsk.b.usbrstmsk = 1;
    gintmsk.b.usbsuspmsk = 1;
    gintmsk.b.sessreqintmsk = 1;
    gintmsk.b.fetsuspmsk = 1;
    dwc->hw->gintmsk = gintmsk.v;

    dwcEpReg_t *hep;
    // Set nak for all endpoints
    usbcDiepctl_t epctl = {};
    epctl.b.snak = 1;
    for (uint8_t i = 0; i < dwc->iep_count; ++i)
    {
        hep = dwc->iep[i].hep;
        hep->epctl |= epctl.v;
    }

    for (uint8_t i = 0; i < dwc->oep_count; ++i)
    {
        hep = dwc->oep[i].hep;
        hep->epctl |= epctl.v;
    }

    REG_USBC_DOEPMSK_T doepmsk = {};
    doepmsk.b.setupmsk = 1;
    doepmsk.b.ahberrmsk = 1;
    doepmsk.b.xfercomplmsk = 1;
    doepmsk.b.stsphsercvdmsk = 1;
    doepmsk.b.back2backsetup = 1;
    doepmsk.b.stsphsercvdmsk = 1;
    doepmsk.b.outpkterrmsk = 1;
    dwc->hw->doepmsk = doepmsk.v;

    REG_USBC_DIEPMSK_T diepmsk = {};
    diepmsk.b.xfercomplmsk = 1;
    diepmsk.b.ahberrmsk = 1;
    diepmsk.b.timeoutmsk = 1;
    diepmsk.b.txfifoundrnmsk = 1;
    dwc->hw->diepmsk = diepmsk.v;

    // enable in/out ep0 interrupt
    REG_USBC_DAINTMSK_T daintmsk = {};
    daintmsk.b.inepmsk0 = 1;
    daintmsk.b.outepmsk0 = 1;
    dwc->hw->daintmsk = daintmsk.v;

    // clear all pending interrupt
    dwc->hw->gintsts = 0xffffffff;

    gahbcfg.b.dmaen = 1;
    gahbcfg.b.hbstlen = BURST_INCR16;
    gahbcfg.b.glblintrmsk = 1;
    dwc->hw->gahbcfg = gahbcfg.v;
}

static void prvHwExit(dwcUdc_t *dwc)
{
    uint32_t critical = osiEnterCritical();
    for (uint8_t i = 0; i < dwc->iep_count; ++i)
    {
        prvEpDisableLocked(dwc, &dwc->iep[i]);
        osiExitCritical(critical);
        prvEpCancelXferInQueue(&dwc->iep[i]);
        critical = osiEnterCritical();
    }
    for (uint8_t i = 0; i < dwc->oep_count; ++i)
    {
        prvEpDisableLocked(dwc, &dwc->oep[i]);
        osiExitCritical(critical);
        prvEpCancelXferInQueue(&dwc->oep[i]);
        critical = osiEnterCritical();
    }
    osiExitCritical(critical);
}

static inline void prvPlatformInit(udc_t *udc)
{
    udcPlatSetPower(udc, true);
    udcPlatSetClock(udc, true);
    udcPlatEnable(udc);
}

static inline void prvPlatformExit(udc_t *udc)
{
    udcPlatDisable(udc);
    udcPlatSetClock(udc, false);
    udcPlatSetPower(udc, false);
}

static void prvDwcStart(udc_t *udc)
{
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    prvPlatformInit(udc);
    osiIrqSetHandler(HAL_SYSIRQ_NUM(SYS_IRQ_ID_USBC), prvDwcISR, dwc);
    osiIrqSetPriority(HAL_SYSIRQ_NUM(SYS_IRQ_ID_USBC), SYS_IRQ_PRIO_USBC);
    osiIrqEnable(HAL_SYSIRQ_NUM(SYS_IRQ_ID_USBC));
    prvHwInit(dwc, false);
    dwc->pm_level = DWC_NORMAL;
}

static void prvDwcStop(udc_t *udc)
{
    osiIrqDisable(HAL_SYSIRQ_NUM(SYS_IRQ_ID_USBC));
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    if (dwc == NULL)
        return;

    dwc->pm_level = DWC_OFF;
    prvDisconnectISR(dwc);
    prvHwExit(dwc);
    prvPlatformExit(udc);
}

static int prvDwcEpQueue(udc_t *udc, usbEp_t *ep_, usbXfer_t *xfer)
{
    uint32_t critical = osiEnterCritical();
    dwcXfer_t *dx = prvXferGetDwcXfer(xfer);
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    dwcEp_t *ep = EP2DWCEP(ep_);
    if (ep_->num == 0)
        ep = prvGetDwcEp0(dwc);

    if (!(ep->hep->epctl & REGTYPE_FIELD_MASK(usbcDiepctl_t, usbactep)))
    {
        osiExitCritical(critical);
        OSI_LOGE(0, "ep queue not ready. (ep: %x)", ep_->address);
        return -ENOENT;
    }

    int r = prvEpQueueLocked(dwc, ep, dx);
    osiExitCritical(critical);
    return r;
}

static void prvDwcEpDequeue(udc_t *udc, usbEp_t *ep_, usbXfer_t *xfer)
{
    uint32_t critical = osiEnterCritical();
    dwcXfer_t *dx = prvXferGetDwcXfer(xfer);
    if (!dx->occupied)
    {
        osiExitCritical(critical);
        return;
    }

    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    dwcEp_t *ep = EP2DWCEP(ep_);
    if (ep_->num == 0)
        ep = prvGetDwcEp0(dwc);

    if (dx == ep->xfer_schedule)
        prvEpStopLocked(dwc, ep);

    dx->occupied = 0;
    TAILQ_REMOVE(&ep->xfer_q, dx, anchor);
    xfer->status = -ECANCELED;
    osiExitCritical(critical);

    prvGiveBackComplete(dwc, ep, dx);
}

static void prvDwcEpDequeueAll(udc_t *udc, usbEp_t *ep_)
{
    uint32_t critical = osiEnterCritical();
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    dwcEp_t *ep = EP2DWCEP(ep_);
    if (ep_->num == 0)
        ep = prvGetDwcEp0(dwc);

    if (ep->xfer_schedule != NULL)
        prvEpStopLocked(dwc, ep);

    dwcXfer_t *x, *tx;
    TAILQ_FOREACH_SAFE(x, &ep->xfer_q, anchor, tx)
    {
        x->occupied = 0;
        x->xfer.status = -ECANCELED;
        TAILQ_REMOVE(&ep->xfer_q, x, anchor);
        osiExitCritical(critical);

        prvGiveBackComplete_(dwc, ep, x, false);

        critical = osiEnterCritical();
    }

    osiExitCritical(critical);
}

static int prvDwcEpEnable(udc_t *udc, usbEp_t *ep_)
{
    OSI_LOGI(0, "DWC ep %x enable", ep_->address);
    uint32_t critical = osiEnterCritical();
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    dwcEp_t *ep = EP2DWCEP(ep_);
    int result = prvEpEnableLocked(dwc, ep);
    osiExitCritical(critical);
    return result;
}

static void prvDwcEpDisable(udc_t *udc, usbEp_t *ep_)
{
    OSI_LOGI(0, "DWC ep %x disable", ep_->address);
    uint32_t critical = osiEnterCritical();
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    dwcEp_t *ep = EP2DWCEP(ep_);
    prvEpDisableLocked(dwc, ep);
    osiExitCritical(critical);
    prvEpCancelXferInQueue(ep);
}

static usbEp_t *prvDwcEpAllocate(udc_t *udc, usb_endpoint_descriptor_t *desc)
{
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    bool dirin = UE_GET_DIR(desc->bEndpointAddress) == UE_DIR_IN;
    uint8_t type = UE_GET_XFERTYPE(desc->bmAttributes);
    uint8_t epcount;
    dwcEp_t *eps;
    dwcEp_t *ep = NULL;
    uint32_t sc = osiEnterCritical();
    if (dirin)
    {
        eps = dwc->iep;
        epcount = dwc->iep_count;
    }
    else
    {
        eps = dwc->oep;
        epcount = dwc->oep_count;
    }

    for (uint8_t i = 1; i < epcount; ++i)
    {
        ep = &eps[i];
        if (!ep->used)
        {
            ep->used = 1;
            ep->type = type & 0b11;
            ep->halted = 0;
            ep->periodic = 0;
            ep->dynamic_fifo = 0;
            if (type == UE_ISOCHRONOUS || type == UE_INTERRUPT)
                ep->periodic = dirin ? 1 : 0;
            if (dirin && dwc->dedicated_fifo == 0 && ep->periodic == 0)
                ep->dynamic_fifo = 1;
            break;
        }
    }
    osiExitCritical(sc);
    if (ep == NULL)
        return NULL;

    return &ep->ep;
}

static void prvDwcEpFree(udc_t *udc, usbEp_t *ep_)
{
    uint32_t critical = osiEnterCritical();
    dwcEp_t *ep = EP2DWCEP(ep_);
    ep->used = 0;
    osiExitCritical(critical);
}

static usbXfer_t *prvDwcXferAllocate(udc_t *udc)
{
    dwcXfer_t *dx = (dwcXfer_t *)calloc(1, sizeof(*dx));
    return dx != NULL ? &dx->xfer : NULL;
}

static void prvDwcXferFree(udc_t *udc, usbXfer_t *xfer)
{
    dwcXfer_t *dx = (dwcXfer_t *)xfer;
    free(dx);
}

static void prvDwcGdbPollIntr(udc_t *udc)
{
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);
    if (dwc->connected)
        prvDwcISR(dwc);
}

static void prvDwcInit(udc_t *udc, dwcUdc_t *dwc)
{
    static uint8_t _ep0_dma[96] OSI_CACHE_LINE_ALIGNED OSI_SECTION_SRAM_BSS;

    dwc->iep_count = UDC_IEP_COUNT + 1;
    dwc->oep_count = UDC_OEP_COUNT + 1;
    dwc->ep0_buf = _ep0_dma;        // 32 bytes
    dwc->setup_buf = &_ep0_dma[32]; // 64 bytes
    dwc->gadget = udc;
    dwc->dedicated_fifo = 0;
    dwc->connected = 0;
    dwc->hw = hwp_usbc;
    dwc->tx_fifo_map |= (1 << 0); // ep 0x80 use fifo 0

    STAILQ_INIT(&dwc->pending_eps);
    memset(&dwc->iep[0], 0, sizeof(dwcEp_t) * dwc->iep_count);
    memset(&dwc->oep[0], 0, sizeof(dwcEp_t) * dwc->oep_count);
    for (uint8_t i = 0; i < dwc->iep_count; ++i)
    {
        dwc->iep[i].hep = (dwcEpReg_t *)&dwc->hw->diepctl0 + i;
        dwc->iep[i].xfer_schedule = NULL;
        dwc->iep[i].ep.num = i;
        dwc->iep[i].ep.dirin = 1;

        TAILQ_INIT(&dwc->iep[i].xfer_q);
    }

    for (uint8_t i = 0; i < dwc->oep_count; ++i)
    {
        dwc->oep[i].hep = (dwcEpReg_t *)&dwc->hw->doepctl0 + i;
        dwc->oep[i].xfer_schedule = NULL;
        dwc->oep[i].ep.num = i;
        dwc->oep[i].ep.dirin = 0;

        TAILQ_INIT(&dwc->oep[i].xfer_q);
    }

    dwc->iep[0].periodic = 0;

    udc->controller_priv = (unsigned long)dwc;
}

void dwcUdcInit(udc_t *udc)
{
    if (udc == NULL)
        return;
    dwcUdc_t *dwc = &_the_controller;

    uint32_t sc = osiEnterCritical();
    prvDwcInit(udc, dwc);
    udc->speed = USB_SPEED_VARIABLE;
    udc->ep0 = &dwc->oep[0].ep;
    udc->ops.start = prvDwcStart;
    udc->ops.stop = prvDwcStop;
    udc->ops.remote_wakeup = prvDwcRmtWakeup;
    udc->ops.enqueue = prvDwcEpQueue;
    udc->ops.dequeue = prvDwcEpDequeue;
    udc->ops.dequeue_all = prvDwcEpDequeueAll;
    udc->ops.ep_enable = prvDwcEpEnable;
    udc->ops.ep_disable = prvDwcEpDisable;
    udc->ops.ep_stall = prvDwcEpHalt;
    udc->ops.ep_alloc = prvDwcEpAllocate;
    udc->ops.ep_free = prvDwcEpFree;
    udc->ops.xfer_alloc = prvDwcXferAllocate;
    udc->ops.xfer_free = prvDwcXferFree;
    // gdb mode
    udc->ops.gdb_poll = prvDwcGdbPollIntr;
    osiExitCritical(sc);

    osiRegisterBlueScreenHandler(NULL, (osiCallback_t)prvDwcGdbPollIntr, udc);
}

void dwcUdcExit(udc_t *udc)
{
    if (!udc)
        return;

    prvDwcStop(udc);
    dwcUdc_t *dwc = prvGadgetGetDwc(udc);

    if (dwc)
    {
        udc->controller_priv = 0;
        memset(dwc, 0, sizeof(*dwc));
    }
}
