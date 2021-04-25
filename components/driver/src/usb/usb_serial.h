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

#ifndef _USB_SERIAL_H_
#define _USB_SERIAL_H_

#include <usb/usb_device.h>
#include <usb/usb_composite_device.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct usbSerial;

typedef struct
{
    int (*bind)(uint32_t name, struct usbSerial *serial);
    void (*unbind)(struct usbSerial *serial);
    int (*enable)(struct usbSerial *serial);
    void (*disable)(struct usbSerial *serial);
    bool (*open)(struct usbSerial *serial);
    void (*close)(struct usbSerial *serial);
} usbSerialDriverOps_t;

/**
 * @brief struct of usb serial
 */
typedef struct usbSerial
{
    usb_endpoint_descriptor_t *epin_desc;  ///< usb in endpoint descriptor
    usb_endpoint_descriptor_t *epout_desc; ///< usb out endpoint descriptor
    copsFunc_t *func;                      ///< the composite function
    const usbSerialDriverOps_t *ops;       ///< driver implementation ops
    unsigned long priv;                    ///< private data for usb serial;
} usbSerial_t;

const usbSerialDriverOps_t *usbSerialGetOps(void);
const usbSerialDriverOps_t *usbDebugSerialGetOps(void);

#ifdef __cplusplus
}
#endif

#endif
