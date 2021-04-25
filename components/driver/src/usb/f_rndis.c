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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('N', 'D', 'I', 'S')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE

#include "usb/usb_ch9.h"
#include <usb/usb_cdc.h>
#include <usb/usb_composite_device.h>
#include <osi_compiler.h>
#include <osi_api.h>
#include <osi_log.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "usb_utils.h"
#include "drv_usb.h"
#include "drv_config.h"
#include "rndis_data.h"
#include "rndis.h"

typedef struct f_rndis
{
    copsFunc_t func;
    rndisData_t data_channel;
    uint8_t ctrl_id;
    uint8_t data_id;
    uint32_t vendorID;
    rndisParams_t *params;
    char manufacturer[16];
    usbEp_t *notify_ep;
    usbXfer_t *notify_xfer;
    uint32_t notify_count;
} rndis_t;

static const uint8_t kRndisHostMac[ETH_ALEN] = CONFIG_USB_ETHER_HOST_MAC;
static const uint8_t kRndisDevMac[ETH_ALEN] = CONFIG_USB_ETHER_DEV_MAC;

static inline rndis_t *_f2rndis(copsFunc_t *f)
{
    return (rndis_t *)f;
}

/* peak (theoretical) bulk transfer rate in bits-per-second */
static unsigned int bitrate(udc_t *udc)
{
    if (udc->speed >= USB_SPEED_HIGH)
        return 13 * 512 * 8 * 1000 * 8;
    else
        return 19 * 64 * 1 * 1000 * 8;
}

#define RNDIS_STATUS_INTERVAL_MS 16
#define STATUS_BYTECOUNT 8 /* 8 bytes data */

/* string descriptors: */

static struct usb_string rndis_string_defs[] = {
    [0].s = "RNDIS Communications Control",
    [1].s = "RNDIS Ethernet Data",
    [2].s = "RNDIS",
    {} /* end of list */
};

/* interface descriptor: */
static struct usb_interface_descriptor rndis_control_intf = {
    .bLength = sizeof rndis_control_intf,
    .bDescriptorType = UDESC_INTERFACE,

    /* .bInterfaceNumber = DYNAMIC */
    /* status endpoint is optional; this could be patched later */
    .bNumEndpoints = 1,
    .bInterfaceClass = UDCLASS_WIRELESS,
    .bInterfaceSubClass = 0x01,
    .bInterfaceProtocol = 0x03,
    /* .iInterface = DYNAMIC */
};

static struct usb_cdc_header_descriptor header_desc = {
    .bLength = sizeof header_desc,
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_HEADER,
    .bcdCDC = cpu_to_le16(0x0110),
};

static struct usb_cdc_cm_descriptor call_mgmt_descriptor = {
    .bLength = sizeof call_mgmt_descriptor,
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_CM,
    .bmCapabilities = 0x00,
    .bDataInterface = 0x01,
};

static struct usb_cdc_acm_descriptor rndis_acm_descriptor = {
    .bLength = sizeof rndis_acm_descriptor,
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_ACM,

    .bmCapabilities = 0x00,
};

static struct usb_cdc_union_descriptor rndis_union_desc = {
    .bLength = sizeof(rndis_union_desc),
    .bDescriptorType = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_UNION,
    /* .bMasterInterface0 =	DYNAMIC */
    /* .bSlaveInterface0 =	DYNAMIC */
};

/* the data interface has two bulk endpoints */

static struct usb_interface_descriptor rndis_data_intf = {
    .bLength = sizeof rndis_data_intf,
    .bDescriptorType = UDESC_INTERFACE,

    /* .bInterfaceNumber = DYNAMIC */
    .bNumEndpoints = 2,
    .bInterfaceClass = UDCLASS_CDC_DATA,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    /* .iInterface = DYNAMIC */
};

static struct usb_interface_assoc_descriptor
    rndis_iad_descriptor = {
        .bLength = sizeof rndis_iad_descriptor,
        .bDescriptorType = UDESC_IFACE_ASSOC,

        .bFirstInterface = 0, /* XXX, hardcoded */
        .bInterfaceCount = 2, // control + data
        .bFunctionClass = UDCLASS_WIRELESS,
        .bFunctionSubClass = 0x01,
        .bFunctionProtocol = 0x03,
        /* .iFunction = DYNAMIC */
};

/* high speed support: */
static struct usb_endpoint_descriptor rndis_notify_ep_desc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,

    .bEndpointAddress = UE_DIR_IN,
    .bmAttributes = UE_INTERRUPT,
    .wMaxPacketSize = cpu_to_le16(STATUS_BYTECOUNT),
    .bInterval = RNDIS_STATUS_INTERVAL_MS};

static struct usb_endpoint_descriptor rndis_ep_in_desc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,

    .bEndpointAddress = UE_DIR_IN,
    .bmAttributes = UE_BULK,
    .wMaxPacketSize = cpu_to_le16(512),
};

static struct usb_endpoint_descriptor rndis_ep_out_desc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,

    .bEndpointAddress = UE_DIR_OUT,
    .bmAttributes = UE_BULK,
    .wMaxPacketSize = cpu_to_le16(512),
};

static usb_descriptor_t *eth_hs_function[] = {
    (usb_descriptor_t *)&rndis_iad_descriptor,

    /* control interface matches ACM, not Ethernet */
    (usb_descriptor_t *)&rndis_control_intf,
    (usb_descriptor_t *)&header_desc,
    (usb_descriptor_t *)&call_mgmt_descriptor,
    (usb_descriptor_t *)&rndis_acm_descriptor,
    (usb_descriptor_t *)&rndis_union_desc,
    (usb_descriptor_t *)&rndis_notify_ep_desc,

    /* data interface has no altsetting */
    (usb_descriptor_t *)&rndis_data_intf,
    (usb_descriptor_t *)&rndis_ep_in_desc,
    (usb_descriptor_t *)&rndis_ep_out_desc,
    NULL,
};

static void prvRndisOpenCB(copsFunc_t *f)
{
    rndis_t *rndis = _f2rndis(f);
    OSI_LOGD(0, "rndis open cb");

    rndis_set_param_medium(rndis->params, RNDIS_MEDIUM_802_3,
                           bitrate(rndis->func.controller) / 100);
    rndis_signal_connect(rndis->params);
}

void prvRndisCloseCB(copsFunc_t *f)
{
    rndis_t *rndis = _f2rndis(f);
    OSI_LOGD(0, "rndis close cb");

    rndis_set_param_medium(rndis->params, RNDIS_MEDIUM_802_3, 0);
    rndis_signal_disconnect(rndis->params);
}

static void _rndisResponseAvailable(void *_rndis)
{
    uint32_t critical = osiEnterCritical();
    rndis_t *rndis = _rndis;
    usbXfer_t *xfer = rndis->notify_xfer;
    uint32_t *data = xfer->buf;

    rndis->notify_count++;
    if (rndis->notify_count != 1)
    {
        osiExitCritical(critical);
        return;
    }

    data[0] = cpu_to_le32(1);
    data[1] = cpu_to_le32(0);
    osiExitCritical(critical);

    int retval = udcEpQueue(rndis->func.controller, rndis->notify_ep, xfer);
    if (retval < 0)
    {
        rndis->notify_count--;
        OSI_LOGE(0, "Rndis tx start failed. size (%u), enqueue return (%d)",
                 xfer->length, retval);
    }
}

static void _rndisResponseComplete(usbEp_t *ep, usbXfer_t *xfer)
{
    rndis_t *rndis = xfer->param;
    int status = xfer->status;
    uint32_t critical;

    /* after TX:
	 *  - USB_CDC_GET_ENCAPSULATED_RESPONSE (ep0/control)
	 *  - RNDIS_RESPONSE_AVAILABLE (status/irq)
	 */
    switch (status)
    {
    case 0:
        if (ep != rndis->notify_ep)
            break;

        /* handle multiple pending RNDIS_RESPONSE_AVAILABLE
		 * notifications by resending until we're done
		 */
        critical = osiEnterCritical();
        rndis->notify_count--;
        if (rndis->notify_count == 0)
        {
            osiExitCritical(critical);
            break;
        }
        osiExitCritical(critical);

        status = udcEpQueue(rndis->func.controller, rndis->notify_ep, xfer);
        if (status)
        {
            rndis->notify_count--;
            OSI_LOGI(0, "notify/1 --> %d\n", status);
        }
        break;

    default:
        rndis->notify_count = 0;
        OSI_LOGE(0, "RNDIS %s response error %d, %d/%d\n",
                 ep->address, status, xfer->actual, xfer->length);
        break;
    }
}

static void _rndisCommandComplete(usbEp_t *ep, usbXfer_t *xfer)
{
    rndis_t *rndis = xfer->param;
    /* received RNDIS command from USB_CDC_SEND_ENCAPSULATED_COMMAND */
    int status = rndis_msg_parser(rndis->params, (uint8_t *)xfer->buf);
    if (status < 0)
    {
        OSI_LOGE(0, "RNDIS command error %d, %d/%d\n",
                 status, xfer->actual, xfer->length);
    }
}

static int _rndisBind(copsFunc_t *f, cops_t *cops, udc_t *udc)
{
    rndis_t *rndis = _f2rndis(f);

    rndis->data_channel.func = f;
    rndis->data_channel.epin_desc = &rndis_ep_in_desc;
    rndis->data_channel.epout_desc = &rndis_ep_out_desc;
    rndis->data_channel.rndis_open = prvRndisOpenCB;
    rndis->data_channel.rndis_close = prvRndisCloseCB;
    rndis->data_channel.host_mac = kRndisHostMac;
    rndis->data_channel.dev_mac = kRndisDevMac;

    int retval = rndisEtherBind(&rndis->data_channel);
    if (retval < 0)
    {
        OSI_LOGE(0, "rndisEtherBind failed\n");
        return retval;
    }

    for (uint8_t i = 0; rndis_string_defs[i].s != NULL; ++i)
        rndis_string_defs[i].id = copsAssignStringId(cops, &rndis_string_defs[i]);
    rndis_control_intf.iInterface = rndis_string_defs[0].id;
    rndis_data_intf.iInterface = rndis_string_defs[1].id;
    rndis_iad_descriptor.iFunction = rndis_string_defs[2].id;

    rndis->ctrl_id = copsAssignInterface(cops, f);
    if (rndis->ctrl_id == -1)
        goto intf_fail;

    rndis->data_id = copsAssignInterface(cops, f);
    if (rndis->data_id == -1)
    {
        copsRemoveInterface(cops, rndis->ctrl_id);
        rndis->ctrl_id = -1;
        goto intf_fail;
    }

    rndis_iad_descriptor.bFirstInterface = rndis->ctrl_id;
    rndis_control_intf.bInterfaceNumber = rndis->ctrl_id;
    rndis_union_desc.bMasterInterface = rndis->ctrl_id;

    rndis_data_intf.bInterfaceNumber = rndis->data_id;
    rndis_union_desc.bSlaveInterface[0] = rndis->data_id;

    rndis->notify_ep = udcEpAlloc(udc, &rndis_notify_ep_desc);
    if (rndis->notify_ep == NULL)
        goto notify_ep_fail;

    rndis->notify_xfer = udcXferAlloc(udc);
    if (!rndis->notify_xfer)
        goto notify_xfer_fail;

    rndis->notify_xfer->buf = malloc(STATUS_BYTECOUNT);
    if (!rndis->notify_xfer->buf)
        goto notify_buf_fail;

    rndis->notify_xfer->length = STATUS_BYTECOUNT;
    rndis->notify_xfer->param = rndis;
    rndis->notify_xfer->complete = _rndisResponseComplete;

    rndis_notify_ep_desc.bEndpointAddress = rndis->notify_ep->address;

    rndis_set_param_medium(rndis->params, RNDIS_MEDIUM_802_3, 0);

    if (rndis->manufacturer && rndis->vendorID &&
        rndis_set_param_vendor(rndis->params, rndis->vendorID, &rndis->manufacturer[0]))
    {
        retval = -EINVAL;
        free(rndis->notify_xfer->buf);
        goto fail;
    }

    return 0;

fail:
    free(rndis->notify_xfer->buf);

notify_buf_fail:
    udcXferFree(udc, rndis->notify_xfer);

notify_xfer_fail:
    udcEpFree(udc, rndis->notify_ep);

notify_ep_fail:
    copsRemoveInterface(cops, rndis->ctrl_id);
    copsRemoveInterface(cops, rndis->data_id);

intf_fail:
    rndisEtherUnbind(&rndis->data_channel);
    for (uint8_t i = 0; rndis_string_defs[i].s != NULL; ++i)
        copsRemoveString(cops, &rndis_string_defs[i]);

    return -1;
}

static void _rndisUnbind(copsFunc_t *f)
{
    rndis_t *rndis = _f2rndis(f);

    OSI_LOGI(0, "rndisUnbind\n");

    rndisEtherUnbind(&rndis->data_channel);

    if (rndis->notify_xfer)
    {
        free(rndis->notify_xfer->buf);
        udcXferFree(f->controller, rndis->notify_xfer);
    }

    if (rndis->notify_ep)
        udcEpFree(f->controller, rndis->notify_ep);

    if (rndis->ctrl_id != -1)
        copsRemoveInterface(f->cops, rndis->ctrl_id);
    if (rndis->data_id != -1)
        copsRemoveInterface(f->cops, rndis->data_id);

    for (uint8_t i = 0; rndis_string_defs[i].s != NULL; ++i)
        copsRemoveString(f->cops, &rndis_string_defs[i]);
}

static int _rndisSetup(copsFunc_t *f, const usb_device_request_t *ctrl)
{
    rndis_t *rndis = _f2rndis(f);
    uint16_t w_index = ctrl->wIndex;
    uint16_t w_value = ctrl->wValue;
    uint16_t w_length = ctrl->wLength;
    int32_t value = -EOPNOTSUPP;
    copsCtrl_t *cc = copsGetCtrl(f->cops);
    usbXfer_t *xfer = cc->xfer;

    xfer->zlp = 0;
    xfer->param = rndis;
    xfer->length = 0;
    xfer->buf = cc->buffer;
    xfer->complete = NULL;

    switch ((ctrl->bmRequestType << 8) | ctrl->bRequest)
    {
    case ((UT_DIR_OUT | UT_CLASS | UT_RECIP_INTERFACE) << 8) | UCDC_SEND_ENCAPSULATED_COMMAND:
        if (w_value || w_index != rndis->ctrl_id)
            goto invalid;

        value = w_length;
        xfer->complete = _rndisCommandComplete;
        xfer->param = rndis;
        break;

    case ((UT_DIR_IN | UT_CLASS | UT_RECIP_INTERFACE) << 8) | UCDC_GET_ENCAPSULATED_RESPONSE:
        if (w_value || w_index != rndis->ctrl_id)
            goto invalid;
        else
        {
            rndisResp_t *r = rndis_get_next_response(rndis->params);
            if (r)
            {
                memcpy(xfer->buf, r->buf, r->length);
                xfer->complete = _rndisResponseComplete;
                xfer->param = rndis;
                rndis_free_response(rndis->params, r);
                value = r->length;
            }
        }
        break;

    case ((UT_DIR_OUT | UT_CLASS | UT_RECIP_INTERFACE) << 8) |
        UCDC_NCM_SET_ETHERNET_PACKET_FILTER:
        if (w_length == 0 && w_index == rndis->ctrl_id)
        {
            rndis->data_channel.cdc_filter = w_value;
            OSI_LOGI(0, "rndis packet filter %x", w_value);
            value = 0;
            break;
        }

    default:
    invalid:
        OSI_LOGE(0, "invalid control %02x/%02x value/%04x index/%04x length/%u\n",
                 ctrl->bmRequestType, ctrl->bRequest,
                 w_value, w_index, w_length);
        return -1;
    }

    if (value > 0)
    {
        xfer->zlp = (value < w_length) && UT_GET_DIR(ctrl->bmRequestType) == UT_DIR_IN;
        xfer->length = value;
        if (xfer->length > cc->bufsize)
        {
            OSI_LOGE(0, "rndis ctrl buffer overflow %u/%u", xfer->length, cc->bufsize);
            osiPanic();
        }

        int r = udcEpQueue(f->controller, f->controller->ep0, xfer);
        if (r < 0)
        {
            OSI_LOGE(0, "rndis enqueue setup fail, %d", r);
            return -1;
        }
    }
    return 0;
}

static int _rndisSetAlt(copsFunc_t *f, uint8_t intf, uint8_t alt)
{
    rndis_t *rndis = _f2rndis(f);
    int retval;

    if (intf == rndis->ctrl_id)
    {
        rndis->notify_count = 0;
        retval = udcEpEnable(f->controller, rndis->notify_ep);
        if (retval < 0)
        {
            OSI_LOGE(0, "rndis ctrl intf enable notify fail");
            return retval;
        }
    }
    else if (intf == rndis->data_id)
    {
        retval = rndisEtherEnable(&rndis->data_channel);
        if (retval < 0)
        {
            OSI_LOGE(0, "rndis data intf enable fail %d", retval);
            return retval;
        }

        rndis_set_param_dev(rndis->params, &rndis->data_channel,
                            &rndis->data_channel.cdc_filter);
    }

    return 0;
}

static void _rndisDisable(copsFunc_t *f)
{
    rndis_t *rndis = _f2rndis(f);

    OSI_LOGI(0, "rndis disable, notify cnt %d", rndis->notify_count);
    rndis_uninit(rndis->params);
    udcEpDisable(f->controller, rndis->notify_ep);
    rndisEtherDisable(&rndis->data_channel);
    rndis->data_channel.cdc_filter = 0;
}

static void _rndisDestroy(copsFunc_t *f)
{
    if (!f)
        return;

    rndis_t *rndis = _f2rndis(f);
    OSI_LOGI(0, "rndisDestroy\n");
    rndis_deregister(rndis->params);
    free(rndis);
}

static uint32_t _rndisFillDesc(copsFunc_t *f, void *buf_, uint32_t size)
{
    uint8_t *buf = (uint8_t *)buf_;
    uint32_t total = size;

    if (f->controller->speed >= USB_SPEED_HIGH)
    {
        rndis_ep_in_desc.wMaxPacketSize = cpu_to_le16(512);
        rndis_ep_out_desc.wMaxPacketSize = cpu_to_le16(512);
    }
    else if (f->controller->speed == USB_SPEED_FULL)
    {
        rndis_ep_in_desc.wMaxPacketSize = cpu_to_le16(64);
        rndis_ep_out_desc.wMaxPacketSize = cpu_to_le16(64);
    }
    else
    {
        rndis_ep_in_desc.wMaxPacketSize = cpu_to_le16(8);
        rndis_ep_out_desc.wMaxPacketSize = cpu_to_le16(8);
    }

    usb_descriptor_t *h = eth_hs_function[0];
    for (uint8_t i = 1; h != NULL && size != 0; ++i)
    {
        uint32_t len = OSI_MIN(uint32_t, h->bLength, size);
        memcpy(buf, h, len);
        size -= len;
        buf += len;
        h = eth_hs_function[i];
    }

    return total - size;
}

copsFunc_t *createRndisFunc(void)
{
    struct f_rndis *rndis = (rndis_t *)calloc(1, sizeof(rndis_t));
    if (rndis == NULL)
    {
        OSI_LOGE(0, "alloc rndis_t failed\n");
        return NULL;
    }

    rndis->ctrl_id = -1;
    rndis->data_id = -1;
    rndis->vendorID = USB_VID_SPRD;
    memcpy(rndis->manufacturer, "UNISOC", sizeof("UNISOC"));

    rndis->func.name = OSI_MAKE_TAG('N', 'D', 'I', 'S');
    rndis->func.ops.bind = _rndisBind;
    rndis->func.ops.unbind = _rndisUnbind;
    rndis->func.ops.setup = _rndisSetup;
    rndis->func.ops.set_alt = _rndisSetAlt;
    rndis->func.ops.disable = _rndisDisable;
    rndis->func.ops.fill_desc = _rndisFillDesc;
    rndis->func.ops.destroy = _rndisDestroy;

    rndisParams_t *params = rndis_register(_rndisResponseAvailable, rndis);
    if (params == NULL)
    {
        OSI_LOGE(0, "rndis register failed");
        goto fail;
    }

    rndis->params = params;
    OSI_LOGI(0, "Rndis function create (rndis %p)", rndis);
    return &rndis->func;

fail:
    free(rndis);
    return NULL;
}
