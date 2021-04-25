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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('F', 'A', 'C', 'M')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "osi_api.h"
#include "osi_log.h"
#include "usb_serial.h"
#include "usb/usb_ch9.h"
#include "usb/usb_cdc.h"
#include "usb/usb_device.h"
#include "usb/usb_composite_device.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/errno.h>

typedef struct
{
    usb_interface_assoc_descriptor_t iad;
    usb_interface_descriptor_t control_intf;
    struct usb_cdc_header_descriptor header;
    struct usb_cdc_cm_descriptor call_mgmt;
    struct usb_cdc_acm_descriptor acm;
    struct usb_cdc_union_descriptor cdc_union;
    usb_endpoint_descriptor_t notify_ep;
    usb_interface_descriptor_t data_intf;
    usb_endpoint_descriptor_t in_ep;
    usb_endpoint_descriptor_t out_ep;
} __attribute__((packed)) cdcAcmDesc_t;

typedef struct
{
    copsFunc_t func;
    cdcAcmDesc_t desc;
    usbSerial_t serial;
    struct usb_cdc_line_state port_coding;
    usbEp_t *notify;
    usbXfer_t *notify_xfer;
    usbString_t data_str;
    usbString_t iad_str;

#define ACM_CTRL_RTS (1 << 1)
#define ACM_CTRL_DTR (1 << 0) // ready for data transfer
    uint16_t handshake;

    int8_t ctrl_id;
    int8_t data_id;
} f_acm_t;

static usbString_t gAcmCtrlStr = {
    .s = "CDC Abstract Control Model (ACM)",
};

#define F2ACM(f) (f_acm_t *)(f)

#define ACM_NOTIFY_MAXPACKET 16
#define ACM_NOTIFY_INTERVAL_FS (1 << 5)
#define ACM_NOTIFY_INTERVAL_HS (10)

static const usb_interface_assoc_descriptor_t kAcmIadDescriptor = {
    .bLength = sizeof(kAcmIadDescriptor),
    .bDescriptorType = UDESC_IFACE_ASSOC,
    // .bFirstInterface = DYNAMIC,
    .bInterfaceCount = 2,
    .bFunctionClass = UDCLASS_COMM,
    .bFunctionSubClass = UDESCSUB_CDC_ACM,
    .bFunctionProtocol = UCDC_ACM_PROTO_AT_V25TER,
    // .iFunction = DYNAMIC,
};

static const usb_interface_descriptor_t kAcmCtrlIntfDesc = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = UDESC_INTERFACE,
    // .bInterfaceNumber = DYNAMIC,
    .bNumEndpoints = 1,
    .bInterfaceClass = UDCLASS_COMM,
    .bInterfaceSubClass = UDESCSUB_CDC_ACM,
    .bInterfaceProtocol = UCDC_ACM_PROTO_AT_V25TER,
    // .iInterface = DYNAMIC,
};

static const struct usb_cdc_header_descriptor kAcmHeaderDesc = {
    .bLength = sizeof(kAcmHeaderDesc),
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_HEADER,
    .bcdCDC = 0x0110,
};

static const struct usb_cdc_cm_descriptor kCdcCallMgmtDesc = {
    .bLength = sizeof(kCdcCallMgmtDesc),
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_CM,
    .bmCapabilities = 0,
    // .bDataInterface = DYNAMIC,
};

static const struct usb_cdc_acm_descriptor kAcmDesc = {
    .bLength = sizeof(kAcmDesc),
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_ACM,
    .bmCapabilities = USB_CDC_ACM_HAS_LINE,
};

static const struct usb_cdc_union_descriptor kCdcUnionDesc = {
    .bLength = sizeof(kCdcUnionDesc),
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_UNION,
    // .bMasterInterface0 = DYNAMIC,
    // .bSlaveInterface0 = DYNAMIC,
};

static const usb_interface_descriptor_t kAcmDataIntfDesc = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = UDESC_INTERFACE,
    // .bInterfaceNumber = DYNAMIC,
    .bNumEndpoints = 2,
    .bInterfaceClass = UDCLASS_CDC_DATA,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    // .iInterface = DYNAMIC,
};

// notify/in/out
static const usb_endpoint_descriptor_t kEpDesc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,
    // DYNAMIC
};

static uint32_t _acmFuncFillDesc(copsFunc_t *f, void *buf, uint32_t size)
{
    uint16_t mps = 8;
    uint8_t interval = ACM_NOTIFY_INTERVAL_FS;
    f_acm_t *acm = F2ACM(f);
    if (f->controller->speed >= USB_SPEED_HIGH)
    {
        mps = 512;
        interval = ACM_NOTIFY_INTERVAL_HS;
    }
    else if (f->controller->speed == USB_SPEED_FULL)
    {
        mps = 64;
    }
    else
    {
        mps = 8;
    }

    acm->desc.notify_ep.bInterval = interval;
    acm->desc.in_ep.wMaxPacketSize = mps;
    acm->desc.out_ep.wMaxPacketSize = mps;

    uint32_t len = OSI_MIN(uint32_t, size, sizeof(acm->desc));
    memcpy(buf, &acm->desc, len);
    return len;
}

static void _acmFuncDestroy(copsFunc_t *f)
{
    f_acm_t *acm = F2ACM(f);
    free(acm);
}

static void _acmCompSetLineCoding(usbEp_t *ep, usbXfer_t *xfer)
{
    f_acm_t *acm = (f_acm_t *)xfer->param;
    OSI_LOGE(0, "acm %4c set line coding, %d", acm->func.name, xfer->status);
    if (xfer->status != 0)
        return;

    if (xfer->actual != sizeof(struct usb_cdc_line_state))
    {
        OSI_LOGW(0, "acm set line coding, invalid length %u", xfer->actual);
        udcEpStall(acm->func.controller, acm->func.controller->ep0, true);
    }
    else
    {
        struct usb_cdc_line_state *linecoding = xfer->buf;
        acm->port_coding = *linecoding;
    }
}

static int _acmFuncSetup(copsFunc_t *f, const usb_device_request_t *ctrl)
{
    f_acm_t *acm = F2ACM(f);
    uint16_t w_value = ctrl->wValue;
    uint16_t w_length = ctrl->wLength;
    int value = -EOPNOTSUPP;
    copsCtrl_t *cc = copsGetCtrl(f->cops);
    usbXfer_t *xfer = cc->xfer;

    xfer->zlp = 0;
    xfer->param = acm;
    xfer->length = 0;
    xfer->buf = cc->buffer;
    xfer->complete = NULL;

    switch (ctrl->bmRequestType | (ctrl->bRequest << 8))
    {
    case ((UT_DIR_OUT | UT_CLASS | UT_RECIP_INTERFACE) | (UCDC_SET_LINE_CODING << 8)):
        if (w_length != sizeof(struct usb_cdc_line_state))
            goto invalid;

        value = w_length;
        xfer->complete = _acmCompSetLineCoding;
        break;

    case ((UT_DIR_IN | UT_CLASS | UT_RECIP_INTERFACE) | (UCDC_GET_LINE_CODING << 8)):
        OSI_LOGI(0, "acm %4c get line coding", acm->func.name);
        value = OSI_MIN(uint16_t, w_length, sizeof(struct usb_cdc_line_state));
        memcpy(xfer->buf, &acm->port_coding, value);
        break;

    case ((UT_DIR_OUT | UT_CLASS | UT_RECIP_INTERFACE) | (UCDC_SET_CONTROL_LINE_STATE << 8)):
        OSI_LOGI(0, "acm %4c set control line state %x/%x", acm->func.name, acm->handshake, w_value);
        value = 0;
        acm->handshake = w_value;
        if (acm->handshake & ACM_CTRL_DTR)
            acm->serial.ops->open(&acm->serial);
        else
            acm->serial.ops->close(&acm->serial);
        break;

    invalid:
    default:
        break;
    }

    if (value > 0)
    {
        xfer->length = OSI_MIN(uint32_t, value, cc->bufsize);
        value = udcEpQueue(f->controller, f->controller->ep0, xfer);
        if (value < 0)
            OSI_LOGE(0, "acm ctrl queue fail, %d", value);
    }

    return value;
}

static int _acmFuncBind(copsFunc_t *f, cops_t *cops, udc_t *udc)
{
    f_acm_t *acm = F2ACM(f);
    int r = acm->serial.ops->bind(f->name, &acm->serial);
    if (r < 0)
    {
        OSI_LOGE(0, "acm bind data port err %d", r);
        return r;
    }

    int result = -EINVAL;
    int8_t ctrl_intf, data_intf;
    ctrl_intf = copsAssignInterface(cops, f);
    if (ctrl_intf < 0)
        goto ctrl_intf_fail;

    data_intf = copsAssignInterface(cops, f);
    if (data_intf < 0)
        goto data_intf_fail;

    acm->desc.control_intf.iInterface = copsAssignStringId(cops, &gAcmCtrlStr);
    acm->desc.data_intf.iInterface = copsAssignStringId(cops, &acm->data_str);
    acm->desc.iad.iFunction = copsAssignStringId(cops, &acm->iad_str);

    do
    {
        acm->notify = udcEpAlloc(udc, &acm->desc.notify_ep);
        if (acm->notify == NULL)
            break;

        acm->notify_xfer = udcXferAlloc(udc);
        if (acm->notify_xfer == NULL)
            break;

        result = 0;
    } while (0);

    if (result < 0)
        goto notify_fail;

    acm->desc.notify_ep.bEndpointAddress = acm->notify->address;

    acm->desc.iad.bFirstInterface = ctrl_intf;
    acm->desc.control_intf.bInterfaceNumber = ctrl_intf;
    acm->desc.cdc_union.bMasterInterface = ctrl_intf;

    acm->desc.cdc_union.bSlaveInterface[0] = data_intf;
    acm->desc.data_intf.bInterfaceNumber = data_intf;
    acm->desc.call_mgmt.bDataInterface = data_intf;

    acm->ctrl_id = ctrl_intf;
    acm->data_id = data_intf;

    return 0;

notify_fail:
    acm->desc.control_intf.iInterface = copsRemoveString(cops, &gAcmCtrlStr);
    acm->desc.data_intf.iInterface = copsRemoveString(cops, &acm->data_str);
    acm->desc.iad.iFunction = copsRemoveString(cops, &acm->iad_str);

    udcXferFree(udc, acm->notify_xfer);
    udcEpFree(udc, acm->notify);
    acm->notify_xfer = NULL;
    acm->notify = NULL;

data_intf_fail:
    copsRemoveInterface(cops, ctrl_intf);

ctrl_intf_fail:
    acm->serial.ops->unbind(&acm->serial);

    return result;
}

static void _acmFuncUnbind(copsFunc_t *f)
{
    f_acm_t *acm = F2ACM(f);
    acm->handshake = 0;
    acm->serial.ops->unbind(&acm->serial);
    copsRemoveInterface(f->cops, acm->ctrl_id);
    copsRemoveInterface(f->cops, acm->data_id);
    udcEpDisable(f->controller, acm->notify);
    udcEpFree(f->controller, acm->notify);
    udcXferFree(f->controller, acm->notify_xfer);
    acm->desc.control_intf.iInterface = copsRemoveString(f->cops, &gAcmCtrlStr);
    acm->desc.data_intf.iInterface = copsRemoveString(f->cops, &acm->data_str);
    acm->desc.iad.iFunction = copsRemoveString(f->cops, &acm->iad_str);
    acm->ctrl_id = -1;
    acm->data_id = -1;
}

static int _acmFuncSetAlt(copsFunc_t *f, uint8_t intf, uint8_t alt)
{
    f_acm_t *acm = F2ACM(f);
    OSI_LOGI(0, "acm set alt %u/%u/%p", intf, alt, acm->notify_xfer->param);
    if (intf == (uint8_t)acm->ctrl_id)
    {
        if (acm->notify_xfer->param != NULL)
        {
            udcEpDisable(f->controller, acm->notify);
        }

        udcEpEnable(f->controller, acm->notify);
        acm->notify_xfer->param = acm;
    }
    else if (intf == (uint8_t)acm->data_id)
    {
        acm->serial.ops->disable(&acm->serial);
        int r = acm->serial.ops->enable(&acm->serial);
        if (r < 0)
        {
            OSI_LOGE(0, "acm enable data port fail %u/%d", intf, r);
            return -1;
        }
    }
    return 0;
}

static void _acmFuncDisable(copsFunc_t *f)
{
    f_acm_t *acm = F2ACM(f);
    OSI_LOGI(0, "acm disable");
    udcEpDisable(f->controller, acm->notify);
    acm->notify_xfer->param = NULL;
    acm->serial.ops->disable(&acm->serial);
    acm->handshake = 0;
}

copsFunc_t *createCdcAcmFunc(uint32_t name)
{
    const unsigned k_data_strlen = 32;
    const unsigned k_iad_strlen = 32;
    f_acm_t *acm = (f_acm_t *)calloc(1, sizeof(f_acm_t) + k_data_strlen + k_iad_strlen);
    if (acm == NULL)
        return NULL;

    // assign descriptor
    memcpy(&acm->desc.iad, &kAcmIadDescriptor, kAcmIadDescriptor.bLength);
    memcpy(&acm->desc.control_intf, &kAcmCtrlIntfDesc, kAcmCtrlIntfDesc.bLength);
    memcpy(&acm->desc.header, &kAcmHeaderDesc, kAcmHeaderDesc.bLength);
    memcpy(&acm->desc.call_mgmt, &kCdcCallMgmtDesc, kCdcCallMgmtDesc.bLength);
    memcpy(&acm->desc.acm, &kAcmDesc, kAcmDesc.bLength);
    memcpy(&acm->desc.cdc_union, &kCdcUnionDesc, kCdcUnionDesc.bLength);
    memcpy(&acm->desc.notify_ep, &kEpDesc, kEpDesc.bLength);
    memcpy(&acm->desc.data_intf, &kAcmDataIntfDesc, kAcmDataIntfDesc.bLength);
    memcpy(&acm->desc.in_ep, &kEpDesc, kEpDesc.bLength);
    memcpy(&acm->desc.out_ep, &kEpDesc, kEpDesc.bLength);

    // notify ep
    acm->desc.notify_ep.bEndpointAddress = UE_DIR_IN;
    acm->desc.notify_ep.bmAttributes = UE_INTERRUPT;
    acm->desc.notify_ep.wMaxPacketSize = ACM_NOTIFY_MAXPACKET;

    // in ep
    acm->desc.in_ep.bEndpointAddress = UE_DIR_IN;
    acm->desc.in_ep.bmAttributes = UE_BULK;

    // out ep
    acm->desc.out_ep.bEndpointAddress = UE_DIR_OUT;
    acm->desc.out_ep.bmAttributes = UE_BULK;

    acm->func.name = name;
    acm->func.ops.fill_desc = _acmFuncFillDesc;
    acm->func.ops.destroy = _acmFuncDestroy;
    acm->func.ops.setup = _acmFuncSetup;
    acm->func.ops.bind = _acmFuncBind;
    acm->func.ops.unbind = _acmFuncUnbind;
    acm->func.ops.set_alt = _acmFuncSetAlt;
    acm->func.ops.disable = _acmFuncDisable;

    acm->serial.epin_desc = &acm->desc.in_ep;
    acm->serial.epout_desc = &acm->desc.out_ep;
    acm->serial.func = &acm->func;
    acm->serial.ops = usbSerialGetOps();
    acm->serial.priv = (unsigned long)acm;

    acm->ctrl_id = -1;
    acm->data_id = -1;

    char *data_str = (char *)acm + sizeof(*acm);
    char *iad_str = data_str + k_data_strlen;

    snprintf(data_str, k_data_strlen, "ACM Data port (%lx)", name);
    snprintf(iad_str, k_iad_strlen, "ACM Serial (%lx)", name);

    acm->data_str.s = data_str;
    acm->iad_str.s = iad_str;

    return &acm->func;
}
