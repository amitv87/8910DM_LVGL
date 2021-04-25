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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('E', 'C', 'M', 'F')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE

#include "osi_api.h"
#include "osi_log.h"
#include "osi_compiler.h"
#include "usb/usb_ch9.h"
#include "usb/usb_composite_device.h"
#include "usb/usb_cdc.h"
#include "usb_utils.h"
#include "drv_if_ether.h"
#include "ecm_data.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/errno.h>
#include "drv_config.h"

#define ECM_STATUS_INTERVAL_MS 32
#define ECM_STATUS_BYTECOUNT 16 /* 8 byte header + data */

enum ecm_notify_state
{
    ECM_NOTIFY_NONE,    /* don't notify */
    ECM_NOTIFY_CONNECT, /* issue CONNECT next */
    ECM_NOTIFY_SPEED,   /* issue SPEED_CHANGE next */
};

struct f_ecm
{
    copsFunc_t func; // keep first member
    ecmData_t data_channel;
    usbEp_t *notify_ep;
    usbXfer_t *notify_xfer;
    int ctrl_id;
    int data_id;
    enum ecm_notify_state notify_state;
    bool is_open;
    bool is_data_enable;
    char host_mac_str[14];
    uint8_t *notify_buffer; // size: ECM_STATUS_BYTECOUNT
};

static const uint8_t kEcmHostMac[ETH_ALEN] = CONFIG_USB_ETHER_HOST_MAC;
static const uint8_t kEcmDevMac[ETH_ALEN] = CONFIG_USB_ETHER_DEV_MAC;

static usbString_t ecm_string[] = {
    [0] = {.s = "CDC Ethernet Control Modle (ECM)"},
    [1] = {.s = "CDC Ethernet Data"},
    [2] = {.s = "CDC ECM"},
    [3] = {.s = ""},
    {}};

static inline struct f_ecm *prvF2ECM(copsFunc_t *func) { return (struct f_ecm *)func; }

static usb_interface_assoc_descriptor_t ecm_iad_desc = {
    .bLength = sizeof(usb_interface_assoc_descriptor_t),
    .bDescriptorType = UDESC_IFACE_ASSOC,
    .bInterfaceCount = 2, // control + data
    .bFunctionClass = UDCLASS_COMM,
    .bFunctionSubClass = UDESCSUB_CDC_UNION, // on linux, it is USB_CDC_SUBCLASS_ETHERNET
    .bFunctionProtocol = UCDC_PROTO_NONE,
    .iFunction = 0, // dynamic or none
};

static usb_interface_descriptor_t ecm_control_intf_desc = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = UDESC_INTERFACE,
    .bInterfaceNumber = 0, // dynamic
    .bNumEndpoints = 1,
    .bInterfaceClass = UDCLASS_COMM,
    .bInterfaceSubClass = UDESCSUB_CDC_UNION, // ETHERNET
    .bInterfaceProtocol = UCDC_PROTO_NONE,
    .iInterface = 0, // dynamic or none
};

static struct usb_cdc_header_descriptor ecm_cdc_header_desc = {
    .bLength = sizeof(struct usb_cdc_header_descriptor),
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_HEADER,
    .bcdCDC = cpu_to_le16(0x0110),
};

static struct usb_cdc_union_descriptor ecm_union_desc = {
    .bLength = sizeof(struct usb_cdc_union_descriptor),
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_UNION,
    .bMasterInterface = 0,   // dynamic
    .bSlaveInterface[0] = 0, // dynamic
};

static struct usb_cdc_ethernet_descriptor ecm_desc = {
    .bLength = sizeof(struct usb_cdc_ethernet_descriptor),
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_ENF,
    .iMacAddress = 0,          // dynamic
    .bmEthernetStatistics = 0, // no statistics
    .wMaxSegmentSize = cpu_to_le16(ETH_FRAME_LEN),
    .wNumberMCFilters = 0,
    .bNumberPowerFilters = 0,
};

static usb_interface_descriptor_t ecm_data_none_intf_desc = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = UDESC_INTERFACE,
    .bInterfaceNumber = 0, // dynamic
    .bAlternateSetting = 0,
    .bNumEndpoints = 0,
    .bInterfaceClass = UDCLASS_CDC_DATA,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface = 0, // dynamic or none
};

static usb_interface_descriptor_t ecm_data_intf_desc = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = UDESC_INTERFACE,
    .bInterfaceNumber = 0, // dynamic
    .bAlternateSetting = 1,
    .bNumEndpoints = 2,
    .bInterfaceClass = UDCLASS_CDC_DATA,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface = 0, // dynamic or none
};

static usb_endpoint_descriptor_t ecm_ep_notify_desc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_IN,
    .bmAttributes = UE_INTERRUPT,
    .wMaxPacketSize = ECM_STATUS_BYTECOUNT,
    .bInterval = ECM_STATUS_INTERVAL_MS,
};

static usb_endpoint_descriptor_t ecm_ep_in_desc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_IN,
    .bmAttributes = UE_BULK,
    .wMaxPacketSize = 512,
};

static usb_endpoint_descriptor_t ecm_ep_out_desc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_OUT,
    .bmAttributes = UE_BULK,
    .wMaxPacketSize = 512,
};

static usb_descriptor_t *ecm_hs_function[] = {
    (usb_descriptor_t *)&ecm_iad_desc,
    (usb_descriptor_t *)&ecm_control_intf_desc,
    (usb_descriptor_t *)&ecm_cdc_header_desc,
    (usb_descriptor_t *)&ecm_union_desc,
    (usb_descriptor_t *)&ecm_desc,
    (usb_descriptor_t *)&ecm_ep_notify_desc,

    // data interface none (alt 0)
    (usb_descriptor_t *)&ecm_data_none_intf_desc,

    // data interface (alt 1)
    (usb_descriptor_t *)&ecm_data_intf_desc,
    (usb_descriptor_t *)&ecm_ep_in_desc,
    (usb_descriptor_t *)&ecm_ep_out_desc,
    NULL,
};

static inline unsigned prvEcmBitrate(struct f_ecm *ecm)
{
    switch (ecm->func.controller->speed)
    {
        // case USB_SPEED_SUPER:
        //     return 13 * 1024 * 8 * 1000 * 8;

    case USB_SPEED_HIGH:
        return 13 * 512 * 8 * 1000 * 8;

    case USB_SPEED_LOW:
    case USB_SPEED_FULL:
    default:
        return 19 * 64 * 1 * 1000 * 8;
    }
}

static void prvEcmDoNotify(struct f_ecm *ecm)
{
    usbXfer_t *x = ecm->notify_xfer;
    if (x == NULL)
        return;

    struct usb_cdc_notification *event = (struct usb_cdc_notification *)x->buf;
    uint32_t *data = (uint32_t *)event->data;

    switch (ecm->notify_state)
    {
    case ECM_NOTIFY_NONE:
        return;

    case ECM_NOTIFY_CONNECT:
        event->bNotification = UCDC_N_NETWORK_CONNECTION;
        event->wValue = ecm->is_open ? 1 : 0;
        event->wLength = 0;
        x->length = UCDC_NOTIFICATION_LENGTH;
        ecm->notify_state = ECM_NOTIFY_SPEED;
        break;

    case ECM_NOTIFY_SPEED:
        event->bNotification = UCDC_N_CONNECTION_SPEED_CHANGE;
        event->wValue = 0;
        event->wLength = 8;
        x->length = ECM_STATUS_BYTECOUNT;
        data[0] = prvEcmBitrate(ecm);
        data[1] = data[0];
        ecm->notify_state = ECM_NOTIFY_NONE;
        break;
    }
    event->bmRequestType = UCDC_NOTIFICATION;
    event->wIndex = ecm->ctrl_id;
    int r = udcEpQueue(ecm->func.controller, ecm->notify_ep, x);
    ecm->notify_xfer = NULL;
    if (r < 0)
    {
        ecm->notify_xfer = x;
        OSI_LOGE(0, "ecm notify %d", r);
    }
}

static inline void prvEcmNotifyConnect(struct f_ecm *ecm)
{
    ecm->notify_state = ECM_NOTIFY_CONNECT;
    prvEcmDoNotify(ecm);
}

static void prvEcmNotifyCompleteCB(usbEp_t *ep, usbXfer_t *xfer)
{
    struct f_ecm *ecm = (struct f_ecm *)xfer->param;
    struct usb_cdc_notification *event = (struct usb_cdc_notification *)xfer->buf;

    if (xfer->status != 0)
    {
        ecm->notify_state = ECM_NOTIFY_NONE;
        OSI_LOGE(0, "ecm notify complete event %02x fail %d", event->bNotification, xfer->status);
    }
    ecm->notify_xfer = xfer;
    prvEcmDoNotify(ecm);
}

static void prvEcmOpen(copsFunc_t *f)
{
    struct f_ecm *ecm = prvF2ECM(f);
    OSI_LOGI(0, "ecm open");
    ecm->is_open = true;
    prvEcmNotifyConnect(ecm);
}

static void prvEcmClose(copsFunc_t *f)
{
    struct f_ecm *ecm = prvF2ECM(f);
    OSI_LOGI(0, "ecm close");
    ecm->is_open = false;
    prvEcmNotifyConnect(ecm);
}

static int prvEcmFuncBind(copsFunc_t *f, cops_t *cops, udc_t *udc)
{
    struct f_ecm *ecm = prvF2ECM(f);
    int ctrl_id = copsAssignInterface(cops, f);
    if (ctrl_id < 0)
        return -1;

    int data_id = copsAssignInterface(cops, f);
    if (data_id < 0)
        goto fail_assign_data_id;

    ecm->notify_ep = udcEpAlloc(udc, &ecm_ep_notify_desc);
    if (ecm->notify_ep == NULL)
        goto fail_notify_ep;

    ecm->notify_xfer = udcXferAlloc(udc);
    if (ecm->notify_buffer == NULL)
        goto fail_notify_xfer;

    ecm->data_channel.func = f;
    ecm->data_channel.epin_desc = &ecm_ep_in_desc;
    ecm->data_channel.epout_desc = &ecm_ep_out_desc;
    ecm->data_channel.ecm_open = prvEcmOpen;
    ecm->data_channel.ecm_close = prvEcmClose;
    ecm->data_channel.host_mac = kEcmHostMac;
    ecm->data_channel.dev_mac = kEcmDevMac;
    if (!ecmDataBind(&ecm->data_channel))
        goto fail_data_intf;

    snprintf(ecm->host_mac_str, 14, "%02x%02x%02x%02x%02x%02x",
             kEcmHostMac[0], kEcmHostMac[1], kEcmHostMac[2],
             kEcmHostMac[3], kEcmHostMac[4], kEcmHostMac[5]);
    ecm_string[3].s = ecm->host_mac_str;

    copsAssignStringId(cops, &ecm_string[0]);
    copsAssignStringId(cops, &ecm_string[1]);
    copsAssignStringId(cops, &ecm_string[2]);
    copsAssignStringId(cops, &ecm_string[3]);

    ecm->notify_state = ECM_NOTIFY_NONE;
    ecm->notify_xfer->complete = prvEcmNotifyCompleteCB;
    ecm->notify_xfer->buf = ecm->notify_buffer;
    ecm->notify_xfer->param = (void *)ecm;

    ecm_ep_notify_desc.bEndpointAddress = ecm->notify_ep->address;

    ecm_iad_desc.iFunction = ecm_string[0].id;
    ecm_data_intf_desc.iInterface = ecm_string[1].id;
    ecm_control_intf_desc.iInterface = ecm_string[2].id;
    ecm_desc.iMacAddress = ecm_string[3].id;

    OSI_LOGV(0, "ECM control id %d, data id %d", ctrl_id, data_id);

    ecm->ctrl_id = ctrl_id;
    ecm_iad_desc.bFirstInterface = ctrl_id;
    ecm_control_intf_desc.bInterfaceNumber = ctrl_id;
    ecm_union_desc.bMasterInterface = ctrl_id;

    ecm->data_id = data_id;
    ecm_data_none_intf_desc.bInterfaceNumber = data_id;
    ecm_data_intf_desc.bInterfaceNumber = data_id;
    ecm_union_desc.bSlaveInterface[0] = data_id;
    return 0;

fail_data_intf:
    udcXferFree(udc, ecm->notify_xfer);
    ecm->notify_xfer = NULL;

fail_notify_xfer:
    udcEpFree(udc, ecm->notify_ep);
    ecm->notify_ep = NULL;

fail_notify_ep:
    copsRemoveInterface(cops, data_id);

fail_assign_data_id:
    copsRemoveInterface(cops, ctrl_id);

    return -1;
}

static void prvEcmFuncUnbind(copsFunc_t *f)
{
    struct f_ecm *ecm = prvF2ECM(f);

    ecmDataUnbind(&ecm->data_channel);

    udcEpFree(ecm->func.controller, ecm->notify_ep);
    udcXferFree(ecm->func.controller, ecm->notify_xfer);
    ecm->notify_ep = NULL;
    ecm->notify_xfer = NULL;

    ecm_iad_desc.iFunction = 0;
    ecm_data_intf_desc.iInterface = 0;
    ecm_control_intf_desc.iInterface = 0;
    ecm_desc.iMacAddress = 0;
    copsRemoveString(f->cops, &ecm_string[0]);
    copsRemoveString(f->cops, &ecm_string[1]);
    copsRemoveString(f->cops, &ecm_string[2]);
    copsRemoveString(f->cops, &ecm_string[3]);

    if (ecm->ctrl_id != -1)
    {
        copsRemoveInterface(f->cops, ecm->ctrl_id);
        ecm->ctrl_id = -1;
    }

    if (ecm->data_id != -1)
    {
        copsRemoveInterface(f->cops, ecm->data_id);
        ecm->data_id = -1;
    }
}

static int prvEcmFuncSetup(copsFunc_t *f, const usb_device_request_t *ctrl)
{
    struct f_ecm *ecm = prvF2ECM(f);
    uint16_t w_index = le16_to_cpu(ctrl->wIndex);
    uint16_t w_value = le16_to_cpu(ctrl->wValue);
    uint16_t w_length = le16_to_cpu(ctrl->wLength);
    int retval = -EINVAL;

    switch ((ctrl->bmRequestType << 8) | ctrl->bRequest)
    {
    case ((UT_DIR_OUT | UT_CLASS | UT_RECIP_INTERFACE) << 8) |
        UCDC_NCM_SET_ETHERNET_PACKET_FILTER:
        if (w_length != 0 || w_index != ecm->ctrl_id)
            goto invalid;
        OSI_LOGI(0, "ecm packet filter %x", w_value);
        retval = 0;
        break;

    default:
    invalid:
        OSI_LOGE(0, "ecm unknown setup: %x %x %x %x %x",
                 ctrl->bmRequestType, ctrl->bRequest,
                 w_value, w_index, w_length);
        break;
    }

    return retval;
}

static int prvEcmFuncSetAlt(copsFunc_t *f, uint8_t intf, uint8_t alt)
{
    struct f_ecm *ecm = prvF2ECM(f);
    int retval = 0;
    OSI_LOGI(0, "ECM set alt %u/%u/%d", intf, alt, ecm->is_data_enable);
    if (intf == ecm->ctrl_id)
    {
        retval = udcEpEnable(f->controller, ecm->notify_ep);
        if (retval < 0)
            OSI_LOGE(0, "ECM set ctrl intf fail");
        return retval;
    }
    else if (intf == ecm->data_id)
    {
        if (ecm->is_data_enable)
        {
            ecm->is_data_enable = false;
            ecmDataDisable(&ecm->data_channel);
        }

        if (alt == 1)
        {
            if (!ecmDataEnable(&ecm->data_channel))
                retval = -EPIPE;
            if (retval == 0)
            {
                ecm->is_data_enable = true;
                prvEcmNotifyConnect(ecm);
            }
        }

        OSI_LOGV(0, "ECM set data interface alt #%d (retval %d)", alt, retval);
        return retval;
    }
    return -EINVAL;
}

static void prvEcmFuncDisable(copsFunc_t *f)
{
    struct f_ecm *ecm = prvF2ECM(f);
    ecm->is_data_enable = false;
    ecmDataDisable(&ecm->data_channel);
    udcEpDisable(f->controller, ecm->notify_ep);
}

static uint32_t prvEcmFuncFillDesc(copsFunc_t *f, void *buf_, uint32_t size)
{
    uint32_t total = size;
    uint8_t *buf = (uint8_t *)buf_;

    if (f->controller->speed >= USB_SPEED_HIGH)
    {
        ecm_ep_in_desc.wMaxPacketSize = cpu_to_le16(512);
        ecm_ep_out_desc.wMaxPacketSize = cpu_to_le16(512);
    }
    else if (f->controller->speed == USB_SPEED_FULL)
    {
        ecm_ep_in_desc.wMaxPacketSize = cpu_to_le16(64);
        ecm_ep_out_desc.wMaxPacketSize = cpu_to_le16(64);
    }
    else
    {
        ecm_ep_in_desc.wMaxPacketSize = cpu_to_le16(8);
        ecm_ep_out_desc.wMaxPacketSize = cpu_to_le16(8);
    }

    usb_descriptor_t *h = ecm_hs_function[0];
    for (uint8_t i = 1; h != NULL && size != 0; ++i)
    {
        uint32_t len = OSI_MIN(uint32_t, h->bLength, size);
        memcpy(buf, h, len);
        size -= len;
        buf += len;
        h = ecm_hs_function[i];
    }
    return total - size;
}

static void prvEcmFuncDestroy(copsFunc_t *f)
{
    // TODO besure unbind
    struct f_ecm *ecm = prvF2ECM(f);
    free(ecm);
}

copsFunc_t *createEcmFunc()
{
    const uint32_t alloc_s = sizeof(struct f_ecm) + ECM_STATUS_BYTECOUNT + CONFIG_CACHE_LINE_SIZE;
    struct f_ecm *ecm = (struct f_ecm *)calloc(1, alloc_s);
    if (ecm == NULL)
    {
        OSI_LOGE(0, "ecm allocate fail");
        return NULL;
    }

    ecm->ctrl_id = -1;
    ecm->data_id = -1;
    ecm->notify_buffer = (uint8_t *)OSI_ALIGN_UP(
        (uint32_t)ecm + sizeof(struct f_ecm),
        CONFIG_CACHE_LINE_SIZE);

    ecm->func.name = OSI_MAKE_TAG('F', 'E', 'C', 'M');
    ecm->func.ops.bind = prvEcmFuncBind;
    ecm->func.ops.unbind = prvEcmFuncUnbind;
    ecm->func.ops.setup = prvEcmFuncSetup;
    ecm->func.ops.set_alt = prvEcmFuncSetAlt;
    ecm->func.ops.disable = prvEcmFuncDisable;
    ecm->func.ops.fill_desc = prvEcmFuncFillDesc;
    ecm->func.ops.destroy = prvEcmFuncDestroy;

    return &ecm->func;
}