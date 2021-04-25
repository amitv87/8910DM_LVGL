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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('G', 'S', 'R', 'L')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "usb_serial.h"
#include "usb/usb_ch9.h"
#include <usb/usb_cdc.h>
#include <usb/usb_composite_device.h>
#include "osi_api.h"
#include "osi_log.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "usb_utils.h"
#include "quec_proj_config.h"

typedef struct
{
    copsFunc_t func;
    usbSerial_t serial;
    uint32_t serial_name;
    struct
    {
        usb_interface_descriptor_t intf;
        usb_endpoint_descriptor_t in_ep;
        usb_endpoint_descriptor_t out_ep;
    } desc;
    int intf_num;
} fSerial_t;

#define SPRD_OPEN_V (1)
#define SPRD_CLOSE_V (0)
#define OPTION_CHANGE_V (3)
#define OPTION_OPEN_V (3)
#define OPTION_CLOSE_V (0)

static_assert(SPRD_OPEN_V != OPTION_CLOSE_V, "serial port ctrl value illegal");
static_assert(SPRD_CLOSE_V != OPTION_OPEN_V, "serial port ctrl value illegal");
static_assert(SPRD_CLOSE_V != OPTION_CHANGE_V, "serial port ctrl value illegal");

static usbString_t generic_serial_str = {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    .s = "Mobile Generic Serial",
#else
    .s = "Unisoc Generic Serial",
#endif
};

static const usb_interface_descriptor_t generic_serial_intf_desc = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = UDESC_INTERFACE,
    /* .bInterfaceNumber = DYNAMIC, */
    .bNumEndpoints = 2,
    .bInterfaceClass = UDCLASS_VENDOR,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface = 0, // DYNAMIC
};

static const usb_endpoint_descriptor_t hs_data_in = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_IN,
    .bmAttributes = UE_BULK,
    .wMaxPacketSize = cpu_to_le16(512),
};

static const usb_endpoint_descriptor_t hs_data_out = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_OUT,
    .bmAttributes = UE_BULK,
    .wMaxPacketSize = cpu_to_le16(512),
};

static inline fSerial_t *_f2s(copsFunc_t *f) { return (fSerial_t *)f; }

static uint32_t _serialFillDesc(copsFunc_t *f, void *buf, uint32_t size)
{
    fSerial_t *s = _f2s(f);
    if (f->controller->speed >= USB_SPEED_HIGH)
    {
        s->desc.in_ep.wMaxPacketSize = cpu_to_le16(512);
        s->desc.out_ep.wMaxPacketSize = cpu_to_le16(512);
    }
    else if (f->controller->speed == USB_SPEED_FULL)
    {
        s->desc.in_ep.wMaxPacketSize = cpu_to_le16(64);
        s->desc.out_ep.wMaxPacketSize = cpu_to_le16(64);
    }
    else
    {
        s->desc.in_ep.wMaxPacketSize = cpu_to_le16(8);
        s->desc.out_ep.wMaxPacketSize = cpu_to_le16(8);
    }

    if (size > sizeof(s->desc))
        size = sizeof(s->desc);
    memcpy(buf, &s->desc, size);
    return size;
}

static int _serialSetAlt(copsFunc_t *f, uint8_t intf, uint8_t alt_must_0)
{
    fSerial_t *s = _f2s(f);
    return intf == s->intf_num ? s->serial.ops->enable(&s->serial) : -1;
}

static int _serialSetup(copsFunc_t *f, const usb_device_request_t *ctrl)
{
    fSerial_t *s = _f2s(f);
    const uint16_t w_value = ctrl->wValue;
    switch ((ctrl->bRequest << 8) | ctrl->bmRequestType)
    {
    case ((UCDC_SET_CONTROL_LINE_STATE << 8) | UT_CLASS | UT_RECIP_INTERFACE):
        OSI_LOGI(0, "GS %4c port change value %u", s->serial_name, w_value);
        if (w_value == SPRD_OPEN_V || w_value == OPTION_OPEN_V)
            s->serial.ops->open(&s->serial);
        else if (w_value == SPRD_CLOSE_V || w_value == OPTION_CLOSE_V)
            s->serial.ops->close(&s->serial);
        return 0;

    default:
        OSI_LOGE(0, "GS %4c unknown setup %x %x %x %x %x", s->serial_name,
                 ctrl->bmRequestType, ctrl->bRequest, ctrl->wValue,
                 ctrl->wIndex, ctrl->wLength);
        osiPanic();
    }
    return -1;
}

static int _serialBind(copsFunc_t *f, cops_t *cops, udc_t *udc)
{
    fSerial_t *s = _f2s(f);
    s->serial.func = f;
    s->serial.epin_desc = &s->desc.in_ep;
    s->serial.epout_desc = &s->desc.out_ep;
    int r = s->serial.ops->bind(s->serial_name, &s->serial);
    if (r < 0)
        return r;

    int intf = copsAssignInterface(cops, f);
    if (intf < 0)
        goto fail;

    s->intf_num = intf;
    s->desc.intf.bInterfaceNumber = intf;
    s->desc.intf.iInterface = copsAssignStringId(cops, &generic_serial_str);
    return 0;

fail:
    s->serial.ops->unbind(&s->serial);
    return -1;
}

static void _serialUnbind(copsFunc_t *f)
{
    fSerial_t *s = _f2s(f);
    s->serial.ops->unbind(&s->serial);

    if (s->intf_num != -1)
        copsRemoveInterface(f->cops, s->intf_num);
    s->intf_num = -1;
    copsRemoveString(f->cops, &generic_serial_str);
}

static void _serialDisable(copsFunc_t *f)
{
    fSerial_t *s = _f2s(f);
    s->serial.ops->disable(&s->serial);
}

static void _serialDestroy(copsFunc_t *f)
{
    if (f)
    {
        fSerial_t *s = _f2s(f);
        free(s);
    }
}

copsFunc_t *createSerialFunc(uint32_t name)
{
    fSerial_t *s = (fSerial_t *)calloc(1, sizeof(fSerial_t));
    if (s == NULL)
        return NULL;

    memcpy(&s->desc.intf, &generic_serial_intf_desc, USB_DT_INTERFACE_SIZE);
    memcpy(&s->desc.in_ep, &hs_data_in, USB_DT_ENDPOINT_SIZE);
    memcpy(&s->desc.out_ep, &hs_data_out, USB_DT_ENDPOINT_SIZE);

    s->intf_num = -1;
    s->serial_name = name;
    s->serial.ops = usbSerialGetOps();
    s->func.name = OSI_MAKE_TAG('G', 'S', 'R', 'L');
    s->func.ops.fill_desc = _serialFillDesc;
    s->func.ops.destroy = _serialDestroy;
    s->func.ops.setup = _serialSetup;
    s->func.ops.bind = _serialBind;
    s->func.ops.unbind = _serialUnbind;
    s->func.ops.set_alt = _serialSetAlt;
    s->func.ops.disable = _serialDisable;

    return &s->func;
}

copsFunc_t *createDebugSerialFunc(uint32_t name)
{
    fSerial_t *s = (fSerial_t *)calloc(1, sizeof(fSerial_t));
    if (s == NULL)
        return NULL;

    memcpy(&s->desc.intf, &generic_serial_intf_desc, USB_DT_INTERFACE_SIZE);
    memcpy(&s->desc.in_ep, &hs_data_in, USB_DT_ENDPOINT_SIZE);
    memcpy(&s->desc.out_ep, &hs_data_out, USB_DT_ENDPOINT_SIZE);

    s->intf_num = -1;
    s->serial_name = name;
    s->serial.ops = usbDebugSerialGetOps();
    s->func.name = OSI_MAKE_TAG('G', 'S', 'R', 'L');
    s->func.ops.fill_desc = _serialFillDesc;
    s->func.ops.destroy = _serialDestroy;
    s->func.ops.setup = _serialSetup;
    s->func.ops.bind = _serialBind;
    s->func.ops.unbind = _serialUnbind;
    s->func.ops.set_alt = _serialSetAlt;
    s->func.ops.disable = _serialDisable;

    return &s->func;
}
