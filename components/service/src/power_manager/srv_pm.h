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

#ifndef _SRV_PM_H_
#define _SRV_PM_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include "srv_power_manager.h"

enum pm_state
{
    PM_POWER_OFF = -1,
    PM_POWER_ON = 0,
    PM_POWER_OFF_CHARGE = 1,
    PM_RUN_COUNT,
};

typedef struct
{
    srvPmPonFunc_t pon_func;
    void *pon_args;
    bool poff_charge_enable;
} srvPmConfig_t;

typedef struct srv_pm srvPm_t;

srvPm_t *srvPmCreateInstance(const srvPmConfig_t *config);

void srvPmRunInitCheck(srvPm_t *pm);

srvPmEvToken_t *srvPmAddEventCB(srvPm_t *pm, srvPmEv_t ev, void *func, void *ctx);

void srvPmRmEventCB(srvPm_t *pm, srvPmEvToken_t *token);

OSI_EXTERN_C_END

#endif