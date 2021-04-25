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

#ifndef _ECM_DATA_H_
#define _ECM_DATA_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include "usb/usb_composite_device.h"
#include "usb/usb_device.h"
#include <stdbool.h>

typedef struct
{
    usb_endpoint_descriptor_t *epin_desc;
    usb_endpoint_descriptor_t *epout_desc;
    const uint8_t *host_mac;
    const uint8_t *dev_mac;
    copsFunc_t *func;
    void (*ecm_open)(copsFunc_t *func);
    void (*ecm_close)(copsFunc_t *func);
    void *priv;
} ecmData_t;

/**
 * @brief ecm data interface bind
 *
 * @param ecm   the ecm data struct, caller should fill\
 *              epin_desc, epout_desc and func.
 * @return true on success else fail
 */
bool ecmDataBind(ecmData_t *ecm);

/**
 * @brief ecm data interface unbind
 *
 * @param ecm   the ecm data struct
 */
void ecmDataUnbind(ecmData_t *ecm);

/**
 * @brief ecm data interface enable
 *
 * @param ecm   the ecm data struct
 * @return true on success else fail
 */
bool ecmDataEnable(ecmData_t *ecm);

/**
 * @brief ecm data interface disable
 *
 * @param ecm   the ecm data struct
 */
void ecmDataDisable(ecmData_t *ecm);

OSI_EXTERN_C_END

#endif