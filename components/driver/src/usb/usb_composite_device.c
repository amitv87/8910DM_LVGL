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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('C', 'O', 'P', 'S')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "usb/usb_ch9.h"
#include <usb/usb_device.h>
#include <usb/usb_composite_device.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <osi_api.h>
#include <osi_log.h>
#include <sys/errno.h>

#include "drv_usb.h"
#include "usb_utils.h"

#define COPS_DEFAULT_VID USB_VID_RDA_DOWNLOAD // 0x0525
#define COPS_DEFAULT_PID USB_PID_RDA_DOWNLOAD // 0xa4a7

typedef TAILQ_HEAD(, composite_func_s) copsFuncHead_t;
struct composite_device_s
{
    udevDrv_t driver; ///< keep first member
    usb_device_descriptor_t device_desc;
    usbEp_t *ep0;
    bool configed;
    uint8_t n_intf;
    uint8_t n_func;
    uint16_t vid;
    uint16_t pid;
    copsFuncHead_t funcs;
#define COPS_MAX_FUNCTION_COUNT 10
#define COPS_MAX_INTERFACE_COUNT 64
#define COPS_MAX_STRING_COUNT 64
    copsFunc_t *intf_func_map[COPS_MAX_INTERFACE_COUNT];
    const usbString_t *string_map[COPS_MAX_STRING_COUNT];
#define COPS_CONFIG_MAX_SIZE 512
    uint8_t config_desc[COPS_CONFIG_MAX_SIZE];
#define COPS_CTRL_BUFFER_SIZE 512
    copsCtrl_t ctrl;
};

#define COPS_DEV_MANUFACTURER 0
#define COPS_DEV_PRODUCT 1
#ifdef CONFIG_QUEC_PROJECT_FEATURE
#define QUEC_COPS_DEV_SERIALNUMBER 2
#endif
static usbString_t cops_string_defs[] = {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    [COPS_DEV_MANUFACTURER].s = "Android",
    [COPS_DEV_PRODUCT].s = "Android",
    [QUEC_COPS_DEV_SERIALNUMBER].s = "",//"0000"
#else
    [COPS_DEV_MANUFACTURER].s = "UNISOC",
    [COPS_DEV_PRODUCT].s = "UNISOC-8910",
#endif
};

static inline cops_t *_getCops(udevDrv_t *drv)
{
    return (cops_t *)drv;
}

static inline uint8_t *_configBuffer(cops_t *c)
{
    return &c->config_desc[0];
}

static inline usb_config_descriptor_t *_configDesc(cops_t *c)
{
    return (usb_config_descriptor_t *)c->config_desc;
}

static int _copsSetupOther(udevDrv_t *driver, const usb_device_request_t *ctrl)
{
    OSI_LOGW(0, "set other, not support yet");
    return -1;
}

static uint32_t _copsGenConfigDesc(cops_t *c, void *buf, uint32_t size)
{
    usb_config_descriptor_t cfg, *config;

    cfg.bLength = USB_DT_CONFIG_SIZE;
    cfg.bDescriptorType = UDESC_CONFIG;
    cfg.wTotalLength = cfg.bLength;
    cfg.bNumInterface = c->n_intf;
    cfg.bConfigurationValue = 1;
    cfg.iConfiguration = 0;
    cfg.bmAttributes = (c->driver.udc->feature & UDC_FEATURE_ATTRIBUTE_MASK);
    cfg.bMaxPower = 0xC8; // 400 mA
    if (size <= USB_DT_CONFIG_SIZE)
    {
        memcpy(buf, &cfg, size);
        return size;
    }

    config = (usb_config_descriptor_t *)buf;
    *config = cfg;
    size -= config->wTotalLength;

    copsFunc_t *f;
    TAILQ_FOREACH(f, &c->funcs, anchor)
    {
        if (size <= 0)
            break;
        uint32_t ret = cfFillDesc(f, (uint8_t *)buf + config->wTotalLength, size);
        config->wTotalLength += ret;
        size -= ret;
    }

    return config->wTotalLength;
}

static void _copsResetConfig(cops_t *c)
{
    OSI_LOGI(0, "cops reset config");
    copsFunc_t *f;
    TAILQ_FOREACH(f, &c->funcs, anchor)
    {
        cfDisable(f);
    }
}

static int _copsSetConfig(cops_t *c, uint16_t num)
{
    OSI_LOGI(0, "cops set config %u", num);
    usb_config_descriptor_t *cfg = _configDesc(c);

    if (c->configed && num == cfg->bConfigurationValue)
        return 0;

    // Reset previous configurations then config current;
    // num == 0 may means no proper configuration
    if (num == 0 || num != cfg->bConfigurationValue)
    {
        if (c->configed)
            _copsResetConfig(c);
        c->configed = false;
        return 0;
    }

    for (uint8_t i = 0; i < COPS_MAX_INTERFACE_COUNT; ++i)
    {
        copsFunc_t *f = c->intf_func_map[i];
        if (f != NULL)
        {
            int r = cfSetAlt(f, i, 0);
            if (r < 0)
            {
                OSI_LOGE(0, "Cops set alt fail. f %p i %d", f, i);
                _copsResetConfig(c);
                goto fail;
            }
        }
    }

    c->configed = true;
    return 0;

fail:
    return -1;
}

static int _copsSetInterface(cops_t *c, uint16_t index, uint16_t value)
{
    OSI_LOGI(0, "cops set interface %u/%u", index, value);
    if (!c->configed || index > COPS_MAX_FUNCTION_COUNT)
    {
        OSI_LOGE(0, "cops set interface fail, invalid index %u", index);
        return -1;
    }

    copsFunc_t *f = c->intf_func_map[index];
    if (f == NULL)
    {
        OSI_LOGE(0, "cops set interface, not register %u", index);
        return -1;
    }

    int result = cfSetAlt(f, index, value);
    if (result < 0)
    {
        OSI_LOGE(0, "cops set interface fail %u/%u/%d", index, value, result);
        return result;
    }
    return 0;
}

static unsigned _ascii2Unicode(const char *ascii, char *unicode, unsigned unicode_len)
{
    unsigned i;
    unsigned len = strlen(ascii);
    memset(unicode, 0, unicode_len);
    for (i = 0; i < len && i < unicode_len / 2; ++i)
        unicode[i * 2] = ascii[i];
    return i * 2;
}

static const char *_copsIdString(cops_t *c, uint16_t language, uint8_t id)
{
    if (id > OSI_ARRAY_SIZE(c->string_map) || id <= 0)
        return NULL;

    return c->string_map[id - 1]->s;
}

static int _copsGetString(cops_t *c, uint16_t language, uint8_t id, char *buf, uint32_t length)
{
    if (length < 2)
        return 0;

    usb_string_descriptor_t *desc = (usb_string_descriptor_t *)buf;
    desc->bDescriptorType = UDESC_STRING;
    if (id == 0)
    {
        desc->bLength = OSI_MIN(uint32_t, length, 4);
        language = 0x0409;
        memcpy(desc->wData, &language, desc->bLength - 2);
        return desc->bLength;
    }

    const char *str = _copsIdString(c, language, id);
    if (str)
    {
        desc->bLength = strlen(str) * 2 + 2;
        int r = _ascii2Unicode(str, (char *)&desc->wData[0], length - 2);
        return (r + 2);
    }

    return -1;
}

static void _deviceQualifier(cops_t *c, void *buf, unsigned size)
{
    usb_device_qualifier_t desc = {
        .bLength = USB_DT_DEVICE_QUALIFIER_SIZE,
        .bDescriptorType = UDESC_DEVICE_QUALIFIER,
        .bcdUSB = c->device_desc.bcdUSB,
        .bDeviceClass = c->device_desc.bDeviceClass,
        .bDeviceSubClass = c->device_desc.bDeviceSubClass,
        .bDeviceProtocol = c->device_desc.bDeviceProtocol,
        .bMaxPacketSize0 = c->device_desc.bMaxPacketSize,
        .bNumConfigurations = c->device_desc.bNumConfigurations,
        .bReserved = 0,
    };
    memcpy(buf, &desc, size);
}

static void prvDumpSetup(const usb_device_request_t *ctrl)
{
    OSI_LOGE(0, "cops dump setup %x %x %x %x %x",
             ctrl->bmRequestType, ctrl->bRequest,
             ctrl->wValue, ctrl->wIndex, ctrl->wLength);
}

static int _copsSetupStandard(udevDrv_t *driver, const usb_device_request_t *ctrl)
{
    cops_t *c = _getCops(driver);
    int result;

    uint32_t w_length = ctrl->wLength;
    uint32_t w_value = ctrl->wValue;
    uint32_t w_index = ctrl->wIndex;

    usbXfer_t *x = c->ctrl.xfer;
    x->zlp = 1;
    x->param = c;
    x->length = 0;
    x->buf = c->ctrl.buffer;
    x->complete = NULL;

    switch (ctrl->bRequest)
    {
    case UR_GET_DESCRIPTOR:
        switch (w_value >> 8)
        {
        case UDESC_DEVICE:
            x->length = OSI_MIN(uint32_t, w_length, USB_DT_DEVICE_SIZE);
            memcpy(x->buf, &c->device_desc, x->length);
            break;

        case UDESC_DEVICE_QUALIFIER:
            x->length = OSI_MIN(uint32_t, w_length, USB_DT_DEVICE_QUALIFIER_SIZE);
            _deviceQualifier(c, x->buf, x->length);
            break;

        case UDESC_CONFIG:
        case UDESC_OTHER_SPEED_CONFIGURATION:
            x->length = OSI_MIN(uint32_t, w_length, _configDesc(c)->wTotalLength);
            memcpy(x->buf, _configBuffer(c), x->length);
            break;

        case UDESC_STRING:
            result = _copsGetString(c, w_index, w_value & 0xff, x->buf, w_length);
            if (result < 0)
                return -1;
            x->length = result;
            break;

        default:
            return -1;
        }
        break;

    case UR_SET_CONFIG:
        return _copsSetConfig(c, ctrl->wValue);

    case UR_SET_INTERFACE:
        if (ctrl->bmRequestType != UT_RECIP_INTERFACE)
        {
            OSI_LOGE(0, "cops set interface invalid");
            prvDumpSetup(ctrl);
            return -1;
        }
        return _copsSetInterface(c, ctrl->wIndex, ctrl->wValue);

    default:
        return -1;
    }

    if (x->length != 0)
    {
        if (x->length > c->ctrl.bufsize)
        {
            OSI_LOGE(0, "cops ctrl buffer overflow, %u/%u", x->length, c->ctrl.bufsize);
            osiPanic();
        }

        // do not set zlp if actual_send == setup->w_length
        if (x->length == w_length)
            x->zlp = 0;

        return udcEpQueue(c->driver.udc, c->driver.udc->ep0, x);
    }

    return 0;
}

static int _copsSetup(udevDrv_t *driver, const usb_device_request_t *ctrl)
{
    OSI_LOGD(0, "cops setup - bmRequestType: 0x%x, bRequest: 0x%x,"
                "wValue: 0x%x, wIndex: 0x%x, wLength: 0x%x",
             ctrl->bmRequestType,
             ctrl->bRequest, ctrl->wValue, ctrl->wIndex, ctrl->wLength);

    cops_t *c = _getCops(driver);
    copsFunc_t *f;
    uint8_t req_type = ctrl->bmRequestType & UT_MASK;
    uint8_t recip = ctrl->bmRequestType & UT_RECIP_MASK;
    uint16_t index = ctrl->wIndex;
    int retval = -EOPNOTSUPP;
    switch (req_type)
    {
    case UT_STANDARD:
        retval = _copsSetupStandard(driver, ctrl);
        break;

    case UT_CLASS:
        switch (recip)
        {
        case UT_RECIP_INTERFACE:
            if (index < COPS_MAX_INTERFACE_COUNT && c->intf_func_map[index])
            {
                f = c->intf_func_map[index];
                retval = cfSetup(f, ctrl);
                if (retval >= 0)
                {
                    break;
                }
            }

        default:
            prvDumpSetup(ctrl);
        }
        break;

    default:
        retval = _copsSetupOther(driver, ctrl);
        break;
    }

    return retval;
}

static int _copsBind(udevDrv_t *driver)
{
    OSI_LOGI(0, "cops bind");
    cops_t *c = _getCops(driver);
    if (c->ctrl.xfer == NULL)
        c->ctrl.xfer = udcXferAlloc(driver->udc);
    return c->ctrl.xfer != NULL ? 0 : -1;
}

static void _copsUnbind(udevDrv_t *driver)
{
    OSI_LOGI(0, "cops unbind");
    if (driver->udc)
    {
        cops_t *c = _getCops(driver);
        if (c->ctrl.xfer)
        {
            udcXferFree(driver->udc, c->ctrl.xfer);
            c->ctrl.xfer = NULL;
        }
    }
}

static void _copsDisconnect(udevDrv_t *driver)
{
    OSI_LOGI(0, "cops disconnect");
    cops_t *c = _getCops(driver);
    if (c->configed)
        _copsResetConfig(c);
    c->configed = false;
}

static void _copsConnect(udevDrv_t *driver)
{
    OSI_LOGI(0, "cops connect");
    cops_t *c = _getCops(driver);
    unsigned total = _copsGenConfigDesc(c, _configBuffer(c), COPS_CONFIG_MAX_SIZE);
    OSI_LOGI(0, "generate config descriptor: size %u/%u", total, COPS_CONFIG_MAX_SIZE);
}

static void _copsGadgetInit(udevDrv_t *drv)
{
    drv->ops.bind = _copsBind;
    drv->ops.unbind = _copsUnbind;
    drv->ops.setup = _copsSetup;
    drv->ops.connect = _copsConnect;
    drv->ops.disconnect = _copsDisconnect;
}

int copsAssignStringId(cops_t *c, usbString_t *ustr)
{
    if (c == NULL || ustr == NULL)
        return 0;

    uint32_t critical = osiEnterCritical();
    if (ustr->id == 0)
    {
        uint8_t id = 0;
        for (unsigned i = 0; i < OSI_ARRAY_SIZE(c->string_map); ++i)
        {
            if (c->string_map[i] == NULL)
            {
                c->string_map[i] = ustr;
                id = i + 1;
                break;
            }
        }
        ustr->id = id;
    }

    if (ustr->id)
        ustr->ref_count += 1;

    osiExitCritical(critical);

    return ustr->id;
}

int copsRemoveString(cops_t *c, usbString_t *ustr)
{
    if (c == NULL || ustr == NULL || ustr->id == 0)
        return 0;

    uint32_t critical = osiEnterCritical();
    if (ustr->ref_count == 0)
    {
        OSI_LOGW(0, "Remove usb string no ref count, id %x", ustr->id);
        ustr->id = 0;
    }
    else
    {
        ustr->ref_count -= 1;
    }

    if (ustr->ref_count == 0)
    {
        c->string_map[ustr->id - 1] = NULL;
        ustr->id = 0;
    }
    osiExitCritical(critical);
    return 0;
}

static void _copsDeviceInit(cops_t *c)
{
    usb_device_descriptor_t *dev = &c->device_desc;

    dev->bLength = USB_DT_DEVICE_SIZE;
    dev->bDescriptorType = UDESC_DEVICE;
    dev->bcdUSB = 0x0200;
    dev->bDeviceClass = UDCLASS_IN_INTERFACE;
    dev->bDeviceSubClass = 0;
    dev->bDeviceProtocol = 0;
    dev->bMaxPacketSize = 64;
    dev->idVendor = c->vid;
    dev->idProduct = c->pid;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    dev->bcdDevice = 0x0318;
#else
    dev->bcdDevice = 0x00;
#endif
    dev->iManufacturer = copsAssignStringId(c, &cops_string_defs[COPS_DEV_MANUFACTURER]);
    dev->iProduct = copsAssignStringId(c, &cops_string_defs[COPS_DEV_PRODUCT]);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    dev->iSerialNumber = copsAssignStringId(c, &cops_string_defs[QUEC_COPS_DEV_SERIALNUMBER]);
#else
    dev->iSerialNumber = 0;
#endif
    dev->bNumConfigurations = 1;
}

static inline void _updateDeviceDesc(cops_t *c)
{
    usb_device_descriptor_t *dev = &c->device_desc;
    dev->idVendor = c->vid;
    dev->idProduct = c->pid;
}

cops_t *copsCreate()
{
    const unsigned allocsize = sizeof(cops_t) + COPS_CTRL_BUFFER_SIZE + CONFIG_CACHE_LINE_SIZE;
    cops_t *c = (cops_t *)calloc(1, allocsize);
    if (c == NULL)
        return NULL;

    c->ctrl.buffer = (uint8_t *)OSI_ALIGN_UP((uint8_t *)c + sizeof(*c), CONFIG_CACHE_LINE_SIZE);
    c->ctrl.bufsize = COPS_CTRL_BUFFER_SIZE;
    c->vid = COPS_DEFAULT_VID;
    c->pid = COPS_DEFAULT_PID;
    TAILQ_INIT(&c->funcs);
    _configDesc(c)->wTotalLength = 0;
    _copsDeviceInit(c);
    _copsGadgetInit(&c->driver);
    return c;
}

void copsSetVendorId(cops_t *c, uint16_t vid)
{
    if (c)
    {
        c->vid = vid;
        _updateDeviceDesc(c);
    }
}

void copsSetProductId(cops_t *c, uint16_t pid)
{
    if (c)
    {
        c->pid = pid;
        _updateDeviceDesc(c);
    }
}

void copsDestroy(cops_t *c)
{
    if (c == NULL)
        return;
    copsRemoveAllFunction(c);
    free(c);
}

bool copsAddFunction(cops_t *c, copsFunc_t *f)
{
    if (c == NULL || f == NULL)
        return false;

    if (c->n_func == COPS_MAX_FUNCTION_COUNT)
    {
        OSI_LOGE(0, "function count had reached the maximum, %d", COPS_MAX_FUNCTION_COUNT);
        return false;
    }

    f->cops = c;
    f->controller = c->driver.udc;
    int r = cfBind(f, c, c->driver.udc);
    if (r < 0)
    {
        f->cops = NULL;
        f->controller = NULL;
        OSI_LOGE(0, "bind function %4c fail", f->name);
        return false;
    }

    TAILQ_INSERT_TAIL(&c->funcs, f, anchor);
    c->n_func += 1;
    return true;
}

bool copsAddFunctions(cops_t *c, copsFunc_t **funcs, unsigned count)
{
    if (c == NULL || funcs == NULL || count == 0)
        return false;

    if (c->n_func + count > COPS_MAX_FUNCTION_COUNT)
    {
        OSI_LOGE(0, "too many functions, %d/%d", c->n_func + count, COPS_MAX_FUNCTION_COUNT);
        return false;
    }

    copsFuncHead_t tmp_funcs;
    TAILQ_INIT(&tmp_funcs);
    bool result = true;
    for (unsigned i = 0; i < count; ++i)
    {
        copsFunc_t *f = funcs[i];
        f->cops = c;
        f->controller = c->driver.udc;
        int r = cfBind(f, c, c->driver.udc);
        if (r < 0)
        {
            OSI_LOGE(0, "bind function %4c fail", f->name);
            result = false;
            break;
        }
        else
        {
            TAILQ_INSERT_TAIL(&tmp_funcs, f, anchor);
        }
    }

    copsFunc_t *f, *tmp;
    TAILQ_FOREACH_SAFE(f, &tmp_funcs, anchor, tmp)
    {
        TAILQ_REMOVE(&tmp_funcs, f, anchor);
        if (result)
        {
            TAILQ_INSERT_TAIL(&c->funcs, f, anchor);
            c->n_func += 1;
        }
        else
        {
            cfUnbind(f);
            f->cops = NULL;
            f->controller = NULL;
        }
    }

    return result;
}

void copsRemoveAllFunction(cops_t *c)
{
    if (c == NULL)
        return;

    copsFunc_t *f;
    while ((f = TAILQ_FIRST(&c->funcs)))
    {
        TAILQ_REMOVE(&c->funcs, f, anchor);
        cfDisable(f);
        cfUnbind(f);
        cfDestroy(f);
    }

    c->n_intf = 0;
    c->n_func = 0;
    _configDesc(c)->bNumInterface = 0;
    _configDesc(c)->wTotalLength = USB_DT_CONFIG_SIZE;
}

udevDrv_t *copsGetGadgetDriver(cops_t *c)
{
    return c ? &c->driver : NULL;
}

int copsAssignInterface(cops_t *c, copsFunc_t *f)
{
    if (c == NULL)
        return -1;

    for (uint8_t i = 0; i < COPS_MAX_INTERFACE_COUNT; ++i)
    {
        if (c->intf_func_map[i] == NULL)
        {
            c->intf_func_map[i] = f;
            c->n_intf += 1;
            return i;
        }
    }

    return -1;
}

void copsRemoveInterface(cops_t *c, uint8_t index)
{
    if (!c || index >= 16 || !c->intf_func_map[index])
        return;

    c->intf_func_map[index] = NULL;
    c->n_intf -= 1;
}

copsCtrl_t *copsGetCtrl(cops_t *c)
{
    if (c && c->ctrl.xfer)
        return &c->ctrl;
    return NULL;
}
