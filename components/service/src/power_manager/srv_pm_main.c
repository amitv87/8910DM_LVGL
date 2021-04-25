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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('S', 'V', 'P', 'M')

#include "srv_pm.h"
#include "osi_sysnv.h"
#include "osi_log.h"

static srvPm_t *gSrvPm = NULL;

bool srvPmInit(srvPmPonFunc_t pon_func, void *arg)
{
    srvPmConfig_t config = {
        .pon_func = pon_func,
        .pon_args = arg,
    };
    config.poff_charge_enable = gSysnvPoffChargeEnable;
    srvPm_t *p = srvPmCreateInstance(&config);
    if (p == NULL)
    {
        OSI_LOGE(0, "srvpm create fail");
        return false;
    }
    gSrvPm = p;
    return true;
}

void srvPmRun()
{
    srvPmRunInitCheck(gSrvPm);
}

srvPmEvToken_t *srvPmAddEvNotify(srvPmEv_t ev, void *func, void *arg)
{
    return srvPmAddEventCB(gSrvPm, ev, func, arg);
}

void srvPmRemoveEvNotify(srvPmEvToken_t *token)
{
    srvPmRmEventCB(gSrvPm, token);
}