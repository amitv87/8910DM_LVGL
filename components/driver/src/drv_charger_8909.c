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
#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('C', 'H', 'A', 'G')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_charger.h"
#include "drv_pmic_intr.h"
#include "drv_adc.h"
#include "hal_adi_bus.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hwregs.h"
#include "drv_config.h"
#include <string.h>

void drvChargerGetInfo(uint8_t *nBcs, uint8_t *nBcl)
{
    *nBcs = 0;
    *nBcl = 100;
}

void drvChargerSetNoticeCB(drvChargerNoticeCB_t cb) {}

drvChargerType_t drvChargerGetType(void)
{
    return DRV_CHARGER_TYPE_NONE;
}

void drvChargerSetCB(drvChargerPlugCB_t cb, void *ctx) {}

void drvChargerInit(void) {}

uint16_t drvChargerGetVbatRT()
{
    return 4000;
}
