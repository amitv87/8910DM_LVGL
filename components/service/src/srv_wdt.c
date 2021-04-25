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

#include "drv_wdt.h"
#include "osi_api.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t max_interval;
    uint32_t feed_interval;
} srvWdt_t;

static srvWdt_t gSrvWdt;

void srvWdtEnterDeepSleep(int64_t reset_ms)
{
    drvWatchdogStop();
    drvWatchdogStart((uint32_t)reset_ms, OSI_WAIT_FOREVER);
}

void srvWdtExitDeepSleep(void)
{
    srvWdt_t *wd = &gSrvWdt;
    drvWatchdogStop();
    drvWatchdogStart(wd->max_interval, wd->feed_interval);
}

bool srvWdtInit(uint32_t max_interval_ms, uint32_t feed_interval_ms)
{
    if (max_interval_ms < feed_interval_ms)
        return false;

    srvWdt_t *wd = &gSrvWdt;
    wd->max_interval = max_interval_ms;
    wd->feed_interval = feed_interval_ms;
    return true;
}

static void prvShutdownCB(void *ctx, osiShutdownMode_t mode)
{
    drvWatchdogStop();
}

bool srvWdtStart()
{
    srvWdt_t *wd = &gSrvWdt;
    bool r = drvWatchdogStart(wd->max_interval, wd->feed_interval);
    if (r)
        osiRegisterShutdownCallback(prvShutdownCB, NULL);
    return r;
}

void srvWdtStop()
{
    osiUnregisterShutdownCallback(prvShutdownCB, NULL);
    drvWatchdogStop();
}
