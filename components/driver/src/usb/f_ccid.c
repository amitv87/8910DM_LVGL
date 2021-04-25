/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('C', 'C', 'I', 'D')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "usb/usb_ch9.h"
#include "usb/ccid.h"
#include "usb/ccid_message.h"
#include "usb/usb_device.h"
#include "usb/usb_composite_device.h"
#include "drv_ccid.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hal_config.h"
#include <sys/errno.h>
#include <sys/queue.h>
#include <string.h>
#include <stdlib.h>

// NOTE: not support multi-instances

#define CCID_NOTIFY_FS_INTERVAL 5
#define CCID_NOTIFY_HS_INTERVAL 16
#define CCID_NOTIFY_MAXPACKET 4
#define CCID_TX_CNT 8
#define CCID_RXBUF_SIZE OSI_ALIGN_UP(sizeof(struct ccid_bulk_out_header), CONFIG_CACHE_LINE_SIZE)
#define CCID_TXBUF_SIZE OSI_ALIGN_UP(sizeof(struct ccid_bulk_in_header), CONFIG_CACHE_LINE_SIZE)
#define CCID_NOTIFY_SIZE sizeof(struct ccid_notification)

struct ccid_xfer;
typedef STAILQ_ENTRY(ccid_xfer) ccidXferIter_t;
typedef STAILQ_HEAD(ccid_xfer_head, ccid_xfer) ccidXferHead_t;

struct f_ccid;
typedef struct ccid_xfer
{
    usbXfer_t *xfer;
    ccidXferIter_t iter;
    struct f_ccid *ccid;
} ccidXfer_t;

typedef struct
{
    usbEp_t *in;
    usbEp_t *out;
    usbXfer_t *rx_xfer;
    ccidXferHead_t tx_idles;
    osiSemaphore_t *tx_xfer_sema;
    osiSemaphore_t *rx_xfer_sema;
} ccidData_t;

typedef struct
{
    usbEp_t *notify;
    usbXfer_t *notify_xfer;
    osiWork_t *notify_work;
    osiSemaphore_t *notify_sema;
    struct ccid_notification pending_notify;
} ccidCtrl_t;

struct drv_ccid
{
    osiMutex_t *rx_lock;
    osiMutex_t *tx_lock;
    ccidLineCallback_t line_callback;
    void *line_cb_ctx;
    bool devopen;
};

typedef struct f_ccid
{
    copsFunc_t func;
    drvCCID_t drv;
    void *extramem;
    osiWorkQueue_t *wq;
    ccidData_t data;
    ccidCtrl_t ctrl;
    uint8_t slotmap;
    bool online;
    bool broken;
} f_ccid_t;

static f_ccid_t *gCCID = NULL;

static usb_interface_descriptor_t s_ccid_intf_desc = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = UDESC_INTERFACE,
    // .bInterfaceNumber = DYNAMIC ALLOCATE,
    .bNumEndpoints = 3,
    .bInterfaceClass = UDCLASS_CCID,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    // .iInterface = DYNAMIC, or 0
};

static ccidClassDesc_t s_ccid_class_desc = {
    .bLength = CCID_DESCRIPTOR_LENGTH,
    .bDescriptorType = CCID_DESCRIPTOR_TYPE,
    .bcdCCID = CCID_0110,
    .bMaxSlotIndex = 0,
    .bVoltageSupport = CCID_VOLTAGE_3V,
    .dwProtocols = CCID_PROTOCOL_T0,
    .dwDefaultClock = 3580,
    .dwMaximumClock = 3580,
    .bNumClockSupported = 0,
    // Deafult ICC I/O data rate in bps
    .dwDataRate = 9600,
    // Maximum supported ICC I/O in bps
    .dwMaxDataRate = 9600,
    .bNumDataRatesSupported = 0,
    .dwMaxIFSD = 0,
    .dwSynchProtocols = 0,
    .dwMechanical = 0,
    .dwFeatures = CCID_FEATURE_EXC_SAPDU |
                  CCID_FEATURE_AUTO_PNEGO |
                  CCID_FEATURE_AUTO_BAUD |
                  CCID_FEATURE_AUTO_CLOCK |
                  CCID_FEATURE_AUTO_VOLT |
                  CCID_FEATURE_AUTO_ACTIV |
                  CCID_FEATURE_AUTO_PCONF,
    .dwMaxCCIDMessageLength = 0x200,
    .bClassGetResponse = 0,
    .bClassEnvelope = 0,
    .wLcdLayout = 0,
    .bPINSupport = 0,
    .bMaxCCIDBusySlots = 1,
};

static usb_endpoint_descriptor_t s_ccid_notify_desc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_IN,
    .bmAttributes = UE_INTERRUPT,
    .wMaxPacketSize = CCID_NOTIFY_MAXPACKET,
    .bInterval = CCID_NOTIFY_HS_INTERVAL,
};

static usb_endpoint_descriptor_t s_ccid_in_ep_desc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_IN,
    .bmAttributes = UE_BULK,
    .wMaxPacketSize = 512,
};

static usb_endpoint_descriptor_t s_ccid_out_ep_desc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_OUT,
    .bmAttributes = UE_BULK,
    .wMaxPacketSize = 512,
};

static usb_descriptor_t *s_ccid_hs_func[] = {
    (usb_descriptor_t *)&s_ccid_intf_desc,
    (usb_descriptor_t *)&s_ccid_class_desc,
    (usb_descriptor_t *)&s_ccid_notify_desc,
    (usb_descriptor_t *)&s_ccid_in_ep_desc,
    (usb_descriptor_t *)&s_ccid_out_ep_desc,
    NULL,
};

static inline f_ccid_t *f2ccid(copsFunc_t *f)
{
    return (f_ccid_t *)f;
}

static inline f_ccid_t *drv2ccid(drvCCID_t *d)
{
    return OSI_CONTAINER_OF(d, f_ccid_t, drv);
}

static uint32_t _ccidFillDesc(copsFunc_t *f, void *buf_, uint32_t size)
{
    OSI_LOGD(0, "ccid fill descriptor");
    uint16_t mps = 8;
    uint8_t notify_interval = CCID_NOTIFY_FS_INTERVAL;
    if (f->controller->speed >= USB_SPEED_HIGH)
    {
        notify_interval = CCID_NOTIFY_HS_INTERVAL;
        mps = 512;
    }
    else if (f->controller->speed == USB_SPEED_FULL)
    {
        mps = 64;
    }

    s_ccid_notify_desc.bInterval = notify_interval;
    s_ccid_in_ep_desc.wMaxPacketSize = mps;
    s_ccid_out_ep_desc.wMaxPacketSize = mps;
    uint8_t *buf = (uint8_t *)buf_;
    uint32_t fill_size = 0;
    usb_descriptor_t *h = s_ccid_hs_func[0];
    for (unsigned i = 1; h != NULL && size != 0; ++i)
    {
        uint32_t len = OSI_MIN(uint32_t, h->bLength, size);
        memcpy(buf, h, len);
        size -= len;
        buf += len;
        fill_size += len;
        h = s_ccid_hs_func[i];
    }
    return fill_size;
}

static void _ccidDisable(copsFunc_t *f);

static void _ccidDestroy(copsFunc_t *f)
{
    f_ccid_t *c = f2ccid(f);
    if (c->drv.devopen)
    {
        // later destroy
        c->broken = true;
    }
    else
    {
        gCCID = NULL;
        free(c);
    }
}

static int _ccidSetup(copsFunc_t *f, const usb_device_request_t *ctrl)
{
    f_ccid_t *c = f2ccid(f);
    copsCtrl_t *cc = copsGetCtrl(f->cops);
    usbXfer_t *xfer = cc->xfer;
    uint16_t w_length = ctrl->wLength;

    if (!c->online)
        return -ENOTCONN;

    xfer->zlp = 0;
    xfer->param = c;
    xfer->length = 0;
    xfer->buf = cc->buffer;
    xfer->complete = NULL;

    int ret = -EOPNOTSUPP;
    switch ((ctrl->bRequest << 8) | ctrl->bmRequestType)
    {
    case ((UT_DIR_OUT | UT_CLASS | UT_RECIP_INTERFACE) | (CCIDGENERICREQ_ABORT << 8)):
        if (w_length != 0)
            goto invalid;

        // TODO
        return 0;

    case ((UT_DIR_IN | UT_CLASS | UT_RECIP_INTERFACE) |
          (CCIDGENERICREQ_GET_CLOCK_FREQUENCIES << 8)):
        if (w_length > cc->bufsize)
            goto invalid;
        *(uint32_t *)xfer->buf = s_ccid_class_desc.dwDefaultClock;
        xfer->length = OSI_MIN(uint16_t, w_length, sizeof(uint32_t));
        break;

    case ((UT_DIR_IN | UT_CLASS | UT_RECIP_INTERFACE) |
          (CCIDGENERICREQ_GET_DATA_RATES << 8)):
        if (w_length > cc->bufsize)
            goto invalid;
        *(uint32_t *)xfer->buf = s_ccid_class_desc.dwDataRate;
        xfer->length = OSI_MIN(uint16_t, w_length, sizeof(uint32_t));
        break;

    default:
    invalid:
        OSI_LOGE(0, "ccid invalid control req: %x/%x/%x/%x/%x",
                 ctrl->bmRequestType, ctrl->bRequest, ctrl->wValue,
                 ctrl->wIndex, ctrl->wLength);
        return ret;
    }

    if (xfer->length > 0)
    {
        ret = udcEpQueue(f->controller, f->controller->ep0, xfer);
        if (ret < 0)
        {
            OSI_LOGE(0, "ccid setup fail");
            goto invalid;
        }
    }

    return 0;
}

static ccidXfer_t *_getTxCCIDXfer(f_ccid_t *c)
{
    uint32_t critical = osiEnterCritical();
    ccidXfer_t *cx = STAILQ_FIRST(&c->data.tx_idles);
    STAILQ_REMOVE_HEAD(&c->data.tx_idles, iter);
    if (cx && STAILQ_EMPTY(&c->data.tx_idles))
        osiSemaphoreAcquire(c->data.tx_xfer_sema);
    osiExitCritical(critical);
    return cx;
}

static void _putTxCCIDXfer(f_ccid_t *c, ccidXfer_t *cx)
{
    uint32_t critical = osiEnterCritical();
    STAILQ_INSERT_TAIL(&c->data.tx_idles, cx, iter);
    osiSemaphoreRelease(c->data.tx_xfer_sema);
    osiExitCritical(critical);
}

static void _bulkInComplete(usbEp_t *ep, struct usb_xfer *xfer)
{
    switch (xfer->status)
    {
    case 0:
    case -ECANCELED:
        break;

    default:
        OSI_LOGE(0, "ccid tx complete fail %d", xfer->status);
        break;
    }
    ccidXfer_t *cx = (ccidXfer_t *)xfer->param;
    _putTxCCIDXfer(cx->ccid, cx);
}

static void _bulkOutComplete(usbEp_t *ep, struct usb_xfer *xfer)
{
    switch (xfer->status)
    {
    case -ECANCELED:
    case 0:
        break;

    default:
        OSI_LOGE(0, "ccid rx complete fail %d", xfer->status);
    }
    f_ccid_t *c = (f_ccid_t *)xfer->param;
    osiSemaphoreRelease(c->data.rx_xfer_sema);
}

static void _notifyComplete(usbEp_t *ep, struct usb_xfer *xfer)
{
    OSI_LOGD(0, "notify complete %d", xfer->status);
    switch (xfer->status)
    {
    case -ECANCELED:
    case 0:
        break;

    default:
        OSI_LOGE(0, "notify complete error %d", xfer->status);
        break;
    }
    f_ccid_t *c = (f_ccid_t *)xfer->param;
    osiSemaphoreRelease(c->ctrl.notify_sema);
}

static void _ccidNotifyWork(void *arg)
{
    f_ccid_t *c = (f_ccid_t *)arg;
    osiSemaphoreAcquire(c->ctrl.notify_sema);
    if (!c->online)
    {
        OSI_LOGW(0, "ccid notify offline");
        osiSemaphoreRelease(c->ctrl.notify_sema);
        return;
    }

    const uint8_t *data = (uint8_t *)&c->ctrl.pending_notify;
    uint8_t *buf = c->ctrl.notify_xfer->buf;
    memcpy(buf, data, sizeof(struct ccid_notification));
    OSI_LOGD(0, "ccid notify work, %p: %x %x %x %x", buf, buf[0], buf[1], buf[2], buf[3]);
    int r = udcEpQueue(c->func.controller, c->ctrl.notify, c->ctrl.notify_xfer);
    if (r < 0)
    {
        OSI_LOGE(0, "ccid queue notify fail %d", r);
    }
}

static int _ccidBind(copsFunc_t *f, cops_t *cops, udc_t *udc)
{
    OSI_LOGD(0, "ccid bind");
    f_ccid_t *c = f2ccid(f);
    bool ret = -ENOMEM;

    unsigned memalloc = CCID_RXBUF_SIZE + CCID_NOTIFY_SIZE + CONFIG_CACHE_LINE_SIZE +
                        CCID_TX_CNT * (CCID_TXBUF_SIZE + sizeof(ccidXfer_t));
    c->extramem = malloc(memalloc);
    if (c->extramem == NULL)
        return ret;

    ccidXfer_t *cx = (ccidXfer_t *)c->extramem;
    uint8_t *xferbuf = (uint8_t *)OSI_ALIGN_UP(
        (uint32_t)c->extramem + CCID_TX_CNT * sizeof(ccidXfer_t),
        CONFIG_CACHE_LINE_SIZE);

    int intf_num = copsAssignInterface(f->cops, f);
    if (intf_num < 0)
        goto intf_assign_fail;

    c->wq = osiWorkQueueCreate("ccid", 1, OSI_PRIORITY_NORMAL, 512);
    if (c->wq == NULL)
        goto base_fail;

    // data channel
    c->data.in = udcEpAlloc(f->controller, &s_ccid_in_ep_desc);
    if (c->data.in == NULL)
        goto data_fail;

    c->data.out = udcEpAlloc(f->controller, &s_ccid_out_ep_desc);
    if (c->data.out == NULL)
        goto data_fail;

    c->data.tx_xfer_sema = osiSemaphoreCreate(1, 1);
    if (c->data.tx_xfer_sema == NULL)
        goto data_fail;

    c->data.rx_xfer_sema = osiSemaphoreCreate(1, 1);
    if (c->data.rx_xfer_sema == NULL)
        goto data_fail;

    STAILQ_INIT(&c->data.tx_idles);
    for (unsigned i = 0; i < CCID_TX_CNT; ++i)
    {
        cx->xfer = udcXferAlloc(f->controller);
        if (cx->xfer == NULL)
            goto data_fail;
        cx->xfer->param = cx;
        cx->xfer->complete = _bulkInComplete;
        cx->xfer->buf = xferbuf;
        cx->xfer->length = CCID_TXBUF_SIZE;
        cx->ccid = c;
        STAILQ_INSERT_TAIL(&c->data.tx_idles, cx, iter);
        xferbuf += CCID_TXBUF_SIZE;
        cx += 1;
    }

    c->data.rx_xfer = udcXferAlloc(f->controller);
    if (c->data.rx_xfer == NULL)
        goto data_fail;
    c->data.rx_xfer->param = c;
    c->data.rx_xfer->complete = _bulkOutComplete;
    c->data.rx_xfer->buf = xferbuf;
    c->data.rx_xfer->length = CCID_RXBUF_SIZE;
    xferbuf += CCID_RXBUF_SIZE;

    // ctrl channel
    c->ctrl.notify = udcEpAlloc(f->controller, &s_ccid_notify_desc);
    if (c->ctrl.notify == NULL)
        goto ctrl_fail;

    c->ctrl.notify_xfer = udcXferAlloc(f->controller);
    if (c->ctrl.notify_xfer == NULL)
        goto ctrl_fail;
    c->ctrl.notify_xfer->param = c;
    c->ctrl.notify_xfer->complete = _notifyComplete;
    c->ctrl.notify_xfer->buf = xferbuf;
    c->ctrl.notify_xfer->length = sizeof(struct ccid_notification);
    xferbuf += CCID_NOTIFY_SIZE;

    c->ctrl.notify_work = osiWorkCreate(_ccidNotifyWork, NULL, c);
    if (c->ctrl.notify_work == NULL)
        goto ctrl_fail;

    c->ctrl.notify_sema = osiSemaphoreCreate(1, 1);
    if (c->ctrl.notify_sema == NULL)
        goto ctrl_fail;

    s_ccid_intf_desc.bInterfaceNumber = intf_num;
    s_ccid_notify_desc.bEndpointAddress = c->ctrl.notify->address;
    s_ccid_in_ep_desc.bEndpointAddress = c->data.in->address;
    s_ccid_out_ep_desc.bEndpointAddress = c->data.out->address;

    return 0;

ctrl_fail:
    osiSemaphoreDelete(c->ctrl.notify_sema);
    osiWorkDelete(c->ctrl.notify_work);
    udcXferFree(f->controller, c->ctrl.notify_xfer);
    udcEpFree(f->controller, c->ctrl.notify);
    memset(&c->ctrl, 0, sizeof(c->ctrl));

data_fail:
    udcXferFree(f->controller, c->data.rx_xfer);
    while ((cx = STAILQ_FIRST(&c->data.tx_idles)) != NULL)
    {
        STAILQ_REMOVE_HEAD(&c->data.tx_idles, iter);
        udcXferFree(f->controller, cx->xfer);
    }
    udcEpFree(f->controller, c->data.in);
    udcEpFree(f->controller, c->data.out);
    osiSemaphoreDelete(c->data.rx_xfer_sema);
    osiSemaphoreDelete(c->data.tx_xfer_sema);
    memset(&c->data, 0, sizeof(c->data));
    STAILQ_INIT(&c->data.tx_idles);

base_fail:
    osiWorkQueueDelete(c->wq);
    c->wq = NULL;
    copsRemoveInterface(f->cops, intf_num);

intf_assign_fail:
    free(c->extramem);
    c->extramem = NULL;

    return ret;
}

static void _ccidUnbind(copsFunc_t *f)
{
    OSI_LOGD(0, "ccid unbind");
    f_ccid_t *c = f2ccid(f);
    _ccidDisable(f);
    ccidXfer_t *cx;
    while ((cx = STAILQ_FIRST(&c->data.tx_idles)) != NULL)
    {
        STAILQ_REMOVE_HEAD(&c->data.tx_idles, iter);
        udcXferFree(f->controller, cx->xfer);
    }

    udcXferFree(f->controller, c->data.rx_xfer);
    udcEpFree(f->controller, c->data.in);
    udcEpFree(f->controller, c->data.out);
    osiSemaphoreDelete(c->data.rx_xfer_sema);
    osiSemaphoreDelete(c->data.tx_xfer_sema);
    memset(&c->data, 0, sizeof(c->data));
    STAILQ_INIT(&c->data.tx_idles);

    udcXferFree(f->controller, c->ctrl.notify_xfer);
    udcEpFree(f->controller, c->ctrl.notify);
    osiWorkDelete(c->ctrl.notify_work);
    osiSemaphoreDelete(c->ctrl.notify_sema);
    memset(&c->ctrl, 0, sizeof(c->ctrl));

    osiWorkQueueDelete(c->wq);
    free(c->extramem);
    c->wq = NULL;
    c->extramem = NULL;

    uint8_t intf_num = s_ccid_intf_desc.bInterfaceNumber;
    s_ccid_intf_desc.bInterfaceNumber = 0;
    copsRemoveInterface(f->cops, intf_num);
}

// notify all inserted slot
static void _ccidReadyNotify(f_ccid_t *c)
{
    uint32_t critical = osiEnterCritical();
    uint16_t notify_change = 0;
    for (uint8_t i = 0; i < sizeof(c->slotmap); ++i)
    {
        if (c->slotmap & (1 << i))
            notify_change |= ((1 << i) | (1 << (i * 2)));
    }
    c->ctrl.pending_notify.bMessageType = CCID_RDRTOPC_NOTIFY_SLOT_CHANGE;
    memcpy(c->ctrl.pending_notify.buf, &notify_change, sizeof(notify_change));
    osiExitCritical(critical);
    osiWorkEnqueue(c->ctrl.notify_work, c->wq);
}

static int _ccidSetAlt(copsFunc_t *f, uint8_t intf, uint8_t alt)
{
    OSI_LOGD(0, "ccid set alt");
    f_ccid_t *c = f2ccid(f);
    if (intf != s_ccid_intf_desc.bInterfaceNumber)
        return -EINVAL;

    int ret = udcEpEnable(f->controller, c->ctrl.notify);
    if (ret < 0)
        return ret;

    ret = udcEpEnable(f->controller, c->data.in);
    if (ret < 0)
        goto epin_fail;

    ret = udcEpEnable(f->controller, c->data.out);
    if (ret < 0)
        goto epout_fail;

    c->online = true;
    osiSemaphoreRelease(c->data.rx_xfer_sema);
    osiSemaphoreRelease(c->data.tx_xfer_sema);
    osiSemaphoreRelease(c->ctrl.notify_sema);

    _ccidReadyNotify(c);

    if (c->drv.line_callback)
        c->drv.line_callback(c->drv.line_cb_ctx, true);
    return 0;

epout_fail:
    udcEpDisable(f->controller, c->data.in);

epin_fail:
    udcEpDisable(f->controller, c->ctrl.notify);

    return ret;
}

static void _ccidDisable(copsFunc_t *f)
{
    OSI_LOGD(0, "ccid disable");
    f_ccid_t *c = f2ccid(f);
    uint32_t critical = osiEnterCritical();
    if (c->online)
    {
        c->online = false;
        udcEpDisable(f->controller, c->ctrl.notify);
        udcEpDisable(f->controller, c->data.in);
        udcEpDisable(f->controller, c->data.out);
    }
    osiExitCritical(critical);
    if (c->drv.line_callback)
        c->drv.line_callback(c->drv.line_cb_ctx, false);
}

copsFunc_t *createCCIDFunc(uint32_t name)
{
    f_ccid_t *c = (f_ccid_t *)calloc(1, sizeof(f_ccid_t));
    if (c == NULL)
        return NULL;

    STAILQ_INIT(&c->data.tx_idles);
    c->func.name = name;
    c->func.ops.fill_desc = _ccidFillDesc;
    c->func.ops.destroy = _ccidDestroy;
    c->func.ops.setup = _ccidSetup;
    c->func.ops.bind = _ccidBind;
    c->func.ops.unbind = _ccidUnbind;
    c->func.ops.set_alt = _ccidSetAlt;
    c->func.ops.disable = _ccidDisable;

    gCCID = c;

    return &c->func;
}

// driver api

drvCCID_t *drvCCIDOpen(uint32_t ignore)
{
    if (gCCID == NULL)
        return NULL;

    uint32_t critical = osiEnterCritical();
    drvCCID_t *d = &gCCID->drv;
    bool ok = !d->devopen;
    if (ok)
    {
        d->rx_lock = osiMutexCreate();
        d->tx_lock = osiMutexCreate();
        if (d->rx_lock && d->tx_lock)
        {
            d->devopen = true;
        }
        else
        {
            osiMutexDelete(d->rx_lock);
            osiMutexDelete(d->tx_lock);
            d->rx_lock = NULL;
            d->tx_lock = NULL;
            ok = false;
        }
    }
    osiExitCritical(critical);
    return d;
}

void drvCCIDClose(drvCCID_t *d)
{
    if (d == NULL || !d->devopen)
        return;

    f_ccid_t *c = drv2ccid(d);
    if (c != gCCID)
        osiPanic();

    uint32_t critical = osiEnterCritical();
    d->devopen = false;
    osiMutexDelete(d->rx_lock);
    osiMutexDelete(d->tx_lock);
    d->rx_lock = NULL;
    d->tx_lock = NULL;
    d->line_callback = NULL;
    d->line_cb_ctx = NULL;
    if (c->broken)
    {
        gCCID = NULL;
        free(c);
    }
    osiExitCritical(critical);
}

void drvCCIDSetLineChangeCallback(drvCCID_t *d, ccidLineCallback_t cb, void *ctx)
{
    uint32_t critical = osiEnterCritical();
    if (d != NULL)
    {
        d->line_callback = cb;
        d->line_cb_ctx = ctx;
    }
    osiExitCritical(critical);
}

bool drvCCIDOnline(drvCCID_t *d)
{
    if (d == NULL)
        return false;
    f_ccid_t *c = drv2ccid(d);
    return c->online;
}

int drvCCIDSlotCheck(drvCCID_t *d, uint8_t *slotmap)
{
    if (d == NULL || slotmap == NULL)
        return -EINVAL;
    f_ccid_t *c = drv2ccid(d);
    *slotmap = c->slotmap;
    return 0;
}

int drvCCIDSlotChange(drvCCID_t *d, uint8_t index, bool insert)
{
    if (d == NULL)
        return -EINVAL;

    if (index > s_ccid_class_desc.bMaxSlotIndex)
    {
        OSI_LOGE(0, "invalid slot %u, max %u", index, s_ccid_class_desc.bMaxSlotIndex);
        return -ERANGE;
    }

    f_ccid_t *c = drv2ccid(d);
    bool attached = (c->slotmap & (1 << index));
    if (insert == attached)
    {
        OSI_LOGW(0, "slot not change %d", attached);
        return attached ? -EBUSY : 0;
    }

    int result = 0;
    uint32_t critical = osiEnterCritical();
    c->slotmap |= (1 << index);
    if (c->online)
    {
        uint16_t notify_change = (1 << (index * 2 + 1));
        for (unsigned i = 0; i < sizeof(c->slotmap); ++i)
            notify_change |= (c->slotmap & (1 << i)) ? (1 << (i * 2)) : 0;

        c->ctrl.pending_notify.bMessageType = CCID_RDRTOPC_NOTIFY_SLOT_CHANGE;
        memcpy(c->ctrl.pending_notify.buf, &notify_change, sizeof(notify_change));
        if (!osiWorkEnqueue(c->ctrl.notify_work, c->wq))
            result = -ENOEXEC;
    }
    osiExitCritical(critical);
    return result;
}

int drvCCIDRead(drvCCID_t *d, struct ccid_bulk_out_header *buf, unsigned size)
{
    if (d == NULL)
        return -EINVAL;

    f_ccid_t *c = drv2ccid(d);
    if (!c->online)
        return -ENOTCONN;

    if (size == 0)
        return 0;

    if (size > sizeof(struct ccid_bulk_out_header))
        return -ENOTCONN;

    if (buf == NULL)
        return -ENOMEM;

    osiMutexLock(d->rx_lock);
    osiSemaphoreAcquire(c->data.rx_xfer_sema);
    if (!c->online)
    {
        osiSemaphoreRelease(c->data.rx_xfer_sema);
        osiMutexUnlock(d->rx_lock);
        return -ENOTCONN;
    }

    uint32_t critical = osiEnterCritical();
    int ret = udcEpQueue(c->func.controller, c->data.out, c->data.rx_xfer);
    if (ret < 0)
    {
        osiSemaphoreRelease(c->data.rx_xfer_sema);
        if (ret == -EBUSY)
            ret = -EIO;
    }
    osiExitCritical(critical);
    if (ret < 0)
    {
        osiMutexUnlock(d->rx_lock);
        OSI_LOGE(0, "ccid rx start fail %d", ret);
        return ret;
    }

    osiSemaphoreAcquire(c->data.rx_xfer_sema);
    ret = c->data.rx_xfer->status;
    if (ret == 0)
    {
        unsigned len = OSI_MIN(unsigned, c->data.rx_xfer->actual, size);
        memcpy(buf, c->data.rx_xfer->buf, len);
    }
    osiSemaphoreRelease(c->data.rx_xfer_sema);
    osiMutexUnlock(d->rx_lock);

    return ret;
}

int drvCCIDWrite(drvCCID_t *d, const struct ccid_bulk_in_header *data, unsigned size)
{
    if (d == NULL)
        return -EINVAL;

    f_ccid_t *c = drv2ccid(d);
    if (!c->online)
        return -ENOTCONN;

    if (size == 0)
        return 0;

    if (size > sizeof(struct ccid_bulk_in_header))
        return -ENOMEM;

    if (data == NULL)
        return -ENOMEM;

    osiMutexLock(d->tx_lock);
    ccidXfer_t *cx = _getTxCCIDXfer(c);
    if (cx == NULL)
    {
        osiSemaphoreAcquire(c->data.tx_xfer_sema);
        if (!c->online)
        {
            osiSemaphoreRelease(c->data.tx_xfer_sema);
            osiMutexUnlock(d->tx_lock);
            return -ENOTCONN;
        }

        cx = _getTxCCIDXfer(c);
        if (cx == NULL)
        {
            // should never happen
            OSI_LOGW(0, "tx empty");
            osiPanic();
        }
    }

    memcpy(cx->xfer->buf, data, size);
    uint32_t critical = osiEnterCritical();
    cx->xfer->length = size;
    int ret = udcEpQueue(c->func.controller, c->data.in, cx->xfer);
    if (ret < 0)
    {
        // overlap error code
        if (ret == -EBUSY)
            ret = -EIO;
        _putTxCCIDXfer(c, cx);
    }
    osiExitCritical(critical);

    if (ret < 0)
        OSI_LOGE(0, "ccid tx start fail %d", ret);

    osiMutexUnlock(d->tx_lock);

    return ret;
}
