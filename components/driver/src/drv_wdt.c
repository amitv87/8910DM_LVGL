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

#include "osi_log.h"
#include "osi_api.h"
#include "drv_wdt.h"
#include "hal_adi_bus.h"
#include "hal_chip.h"
#include "hwregs.h"

#define AON_WDT_HZ (32768)
#define WDT_RESET_MIN (10)
#define WDT_THREAD_PRIORITY (OSI_PRIORITY_IDLE + 1)
#ifdef CONFIG_QUEC_PROJECT_FEATURE
#define WDT_THREAD_STACK_SIZE (1024)
#else
#define WDT_THREAD_STACK_SIZE (512) // enough size
#endif

typedef struct
{
    osiWorkQueue_t *wq;
    osiWork_t *feed_work;
    osiTimer_t *feed_timer;
    uint32_t reload_value;
} drvWdt_t;

static drvWdt_t gDrvWdt = {};

void prvWatchdogReload(uint32_t reload)
{
    halAdiBusBatchChange(
        &hwp_rda2720mWdg->wdg_load_high,
        HAL_ADI_BUS_OVERWITE(reload >> 16),
        &hwp_rda2720mWdg->wdg_load_low,
        HAL_ADI_BUS_OVERWITE(reload & 0xffff),
        HAL_ADI_BUS_CHANGE_END);
}

static void prvFeedWdt(void *p)
{
    drvWdt_t *d = (drvWdt_t *)p;
    prvWatchdogReload(d->reload_value);
}

static bool prvInit(drvWdt_t *d)
{
    bool res = false;
    do
    {
        d->wq = osiSysWorkQueueLowPriority();
        if (d->feed_work == NULL)
        {
            d->feed_work = osiWorkCreate(prvFeedWdt, NULL, d);
            if (d->feed_work == NULL)
                break;
        }

        if (d->feed_timer == NULL)
        {
            d->feed_timer = osiTimerCreateWork(d->feed_work, d->wq);
            if (d->feed_timer == NULL)
                break;
        }

        res = true;
    } while (0);

    if (!res)
    {
        osiTimerDelete(d->feed_timer);
        osiWorkDelete(d->feed_work);
        osiWorkQueueDelete(d->wq);
        d->feed_timer = NULL;
        d->feed_work = NULL;
        d->wq = NULL;
    }

    return res;
}

bool drvWatchdogStart(uint32_t reset_ms, uint32_t feed_ms)
{
    if (reset_ms < WDT_RESET_MIN)
        return false;

    drvWdt_t *d = &gDrvWdt;
    if (!prvInit(d))
        return false;

    if (feed_ms != OSI_WAIT_FOREVER)
    {
        if (reset_ms < feed_ms)
            return false;

        // should stop first
        if (d->reload_value != 0)
            return false;
    }

    uint32_t reload = (reset_ms * (uint64_t)AON_WDT_HZ) / 1000;
    d->reload_value = reload;

    REG_RDA2720M_GLOBAL_MODULE_EN0_T module_en0;
    REG_RDA2720M_GLOBAL_RTC_CLK_EN0_T rtc_clk_en0;
    REG_RDA2720M_WDG_WDG_CTRL_T wdg_ctrl;
    halAdiBusBatchChange(
        &hwp_rda2720mWdg->wdg_ctrl,
        HAL_ADI_BUS_OVERWITE(0), // wdg_run
        &hwp_rda2720mGlobal->module_en0,
        REG_FIELD_MASKVAL1(module_en0, wdg_en, 1),
        &hwp_rda2720mGlobal->rtc_clk_en0,
        REG_FIELD_MASKVAL1(rtc_clk_en0, rtc_wdg_en, 1),
        &hwp_rda2720mWdg->wdg_load_high,
        HAL_ADI_BUS_OVERWITE(reload >> 16),
        &hwp_rda2720mWdg->wdg_load_low,
        HAL_ADI_BUS_OVERWITE(reload & 0xffff),
        &hwp_rda2720mWdg->wdg_ctrl,
        REG_FIELD_MASKVAL2(wdg_ctrl, wdg_run, 1, wdg_rst_en, 1),
        HAL_ADI_BUS_CHANGE_END);

    if (feed_ms != OSI_WAIT_FOREVER)
        osiTimerStartPeriodicRelaxed(d->feed_timer, feed_ms, OSI_WAIT_FOREVER);

    return true;
}

void drvWatchdogStop(void)
{
    drvWdt_t *d = &gDrvWdt;
    if (d->feed_timer)
        osiTimerStop(d->feed_timer);
    d->reload_value = 0;
    REG_RDA2720M_GLOBAL_MODULE_EN0_T module_en0;
    REG_RDA2720M_GLOBAL_RTC_CLK_EN0_T rtc_clk_en0;
    halAdiBusBatchChange(
        &hwp_rda2720mWdg->wdg_ctrl,
        HAL_ADI_BUS_OVERWITE(0), // wdg_run
        &hwp_rda2720mGlobal->module_en0,
        REG_FIELD_MASKVAL1(module_en0, wdg_en, 0),
        &hwp_rda2720mGlobal->rtc_clk_en0,
        REG_FIELD_MASKVAL1(rtc_clk_en0, rtc_wdg_en, 0),
        HAL_ADI_BUS_CHANGE_END);
}
