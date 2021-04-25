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

#ifndef _USB_RNDIS_DATA_H_
#define _USB_RNDIS_DATA_H_

#include <usb/usb_device.h>
#include <usb/usb_composite_device.h>
#include <stdint.h>
#include <stddef.h>
#include "osi_api.h"
#include "osi_compiler.h"

#include "usb_utils.h"
#include "usb_ether.h"

OSI_EXTERN_C_BEGIN

/**
 * @brief struct of cdc ethernet
 */
typedef struct
{
    void (*rndis_open)(copsFunc_t *func);
    void (*rndis_close)(copsFunc_t *func);
    usbEther_t *usbe;                      ///< usb ether instance
    usb_endpoint_descriptor_t *epin_desc;  ///< usb in endpoint descriptor
    usb_endpoint_descriptor_t *epout_desc; ///< usb out endpoint descriptor
    copsFunc_t *func;                      ///< the composite function
    uint16_t cdc_filter;                   ///< cdc filter
    unsigned long priv;                    ///< private data for cdc ether
    const uint8_t *host_mac;
    const uint8_t *dev_mac;
} rndisData_t;

int rndisEtherBind(rndisData_t *ether);
void rndisEtherUnbind(rndisData_t *ether);
int rndisEtherEnable(rndisData_t *ether);
void rndisEtherDisable(rndisData_t *ether);

OSI_EXTERN_C_END

#endif
