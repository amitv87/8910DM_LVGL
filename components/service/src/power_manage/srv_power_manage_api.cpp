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

#include "srv_pm.hpp"
#include "osi_log.h"

static srvpm::SrvPm *gSrvPm = nullptr;

bool srvPmInit(srvPmPowerOn_t pon_func, void *arg)
{
    auto pm = new srvpm::SrvPm();
    if (pm == nullptr)
    {
        OSI_LOGE(0, "pm srv create fail");
        return false;
    }

    if (!pm->init(pon_func, arg))
    {
        OSI_LOGE(0, "pm init fail");
        delete pm;
        return false;
    }

    gSrvPm = pm;
    return true;
}

void srvPmRun()
{
    return gSrvPm->runInitCheck();
}

bool srvPmAddEventNotfiy(srvPmEvent_t event, uint32_t caller, srvPmEventNotify_t cb, void *ctx)
{
    return gSrvPm->addEventNotify(event, caller, cb, ctx);
}

void srvPmRemoveEventNotify(srvPmEvent_t event, uint32_t caller)
{
    gSrvPm->removeEventNotify(event, caller);
}
