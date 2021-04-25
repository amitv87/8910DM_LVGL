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

#ifndef _DRV_CHARGER_H_
#define _DRV_CHARGER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "quec_proj_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DRV_CHARGER_TYPE_NONE,
    DRV_CHARGER_TYPE_SDP,
    DRV_CHARGER_TYPE_DCP,
    DRV_CHARGER_TYPE_CDP,
    DRV_CHARGER_TYPE_UNKOWN //no use adapter
} drvChargerType_t;

typedef void (*drvChargerPlugCB_t)(void *ctx, bool plugged);

typedef void (*drvChargerNoticeCB_t)(void);

void drvChargerInit(void);

void drvChargerSetNoticeCB(drvChargerNoticeCB_t notice_cb);

void drvChargerSetCB(drvChargerPlugCB_t cb, void *ctx);

drvChargerType_t drvChargerGetType(void);

/**
 * @brief return the charger and battery info.
 *
 * @param nBcs  set the channel to measue
 * 0 No charging adapter is connected
 * 1 Charging adapter is connected
 * 2 Charging adapter is connected, charging in progress
 * 3 Charging adapter is connected, charging has finished
 * 4 Charging error, charging is interrupted
 * 5 False charging temperature, charging is interrupted while temperature is beyond allowed range
 * @param nBcl   percent of remaining capacity.
 */

void drvChargerGetInfo(uint8_t *nBcs, uint8_t *nBcl);

void drvChargeEnable(void);

void drvChargeDisable(void);

uint16_t drvChargerGetVbatRT();

#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE
uint32_t quec_get_vchg_vol();
uint32_t quec_get_vbat_vol();
uint32_t quec_get_bat_capicity();
uint32_t quec_get_vbat_cur();
uint32_t quec_get_chg_status();
bool quec_get_charger_state();
uint32_t quec_get_battery_temp();
bool quec_get_battery_state();
#endif


#ifdef __cplusplus
}
#endif
#endif
