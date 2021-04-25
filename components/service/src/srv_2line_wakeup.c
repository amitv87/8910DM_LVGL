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
#include "drv_gpio.h"
#include "osi_sysnv.h"
#include "srv_2line_wakeup.h"

#ifdef CONFIG_TWOLINE_WAKEUP_ENABLE

#define PM_WAKEUP_OUT_NAME OSI_MAKE_TAG('P', 'W', 'O', 'U')
#define PM_WAKEUP_IN_NAME OSI_MAKE_TAG('P', 'W', 'I', 'N')
#define PM_WAKEUP_AT_CONTROL OSI_MAKE_TAG('P', 'W', 'A', 'T')
#define WAKEUP_OUTSLEEP_TIMEOUT_MS (120)

struct srv2LineWakeUp
{
    drvGpio_t *wakeup_out;
    drvGpio_t *wakeup_in;
    bool wakeup_out_state;
    osiSemaphore_t *wakeup_out_sema;
    osiPmSource_t *pm_wakeup_in;
    osiPmSource_t *pm_wakeup_out;
    osiPmSource_t *pm_wakeup_at_control;
    int wakeup_outsleep_timeout_ms;
    osiTimer_t *wakeup_out_sleep_timer;
    osiTimer_t *wakeup_in_sleep_timer;
    mcuNotifySleepMode_t mcu_notify_sleep_mode;
    uint32_t mcu_notify_sleep_delay_ms;
    bool init;
};

typedef struct srv2LineWakeUp srv2LineWakeUp_t;

static srv2LineWakeUp_t gSrv2LineWakeUp;

static void prvWakeUpOutSleepTimeout(void *ctx)
{
    srv2LineWakeUp_t *s = (srv2LineWakeUp_t *)ctx;
    osiPmWakeUnlock(s->pm_wakeup_out);
}

static void prvWakeUpOutPmSuspend(void *ctx, osiSuspendMode_t mode)
{
    srv2LineWakeUp_t *s = (srv2LineWakeUp_t *)ctx;
    drvGpioWrite(s->wakeup_out, false);
    s->wakeup_out_state = false;
}

static const osiPmSourceOps_t _wakeUpOutPmOps = {
    .suspend = prvWakeUpOutPmSuspend,
};

static void prvSrv2LineWakeUpCheckWakeUpOutStateFast(void)
{
    srv2LineWakeUp_t *s = &gSrv2LineWakeUp;

    if (s->wakeup_out_state)
        return;

    drvGpioWrite(s->wakeup_out, true);
    s->wakeup_out_state = true;
    osiPmWakeLock(s->pm_wakeup_out);
    srv2LineWakeUpOutSetSleepTimer();
}
static void prvWakeUpInSleepTimeout(void *ctx)
{
    srv2LineWakeUp_t *s = (srv2LineWakeUp_t *)ctx;

    if (!drvGpioRead(s->wakeup_in))
        osiPmWakeUnlock(s->pm_wakeup_in);
}

static void prvWakeUpInGpioIsr(void *ctx)
{
    srv2LineWakeUp_t *s = (srv2LineWakeUp_t *)ctx;
    if (drvGpioRead(s->wakeup_in))
    {
        osiPmWakeLock(s->pm_wakeup_in);
        prvSrv2LineWakeUpCheckWakeUpOutStateFast();
    }

    osiTimerStart(s->wakeup_in_sleep_timer, s->mcu_notify_sleep_delay_ms);
}

void srv2LineWakeUpCheckWakeUpOutState(void)
{
    srv2LineWakeUp_t *s = &gSrv2LineWakeUp;

    if (!s->init)
        return;

    if (s->wakeup_out_state)
        return;

    drvGpioWrite(s->wakeup_out, true);
    osiSemaphoreAcquire(s->wakeup_out_sema);
    if (!s->wakeup_out_state)
        osiThreadSleep(20);
    osiSemaphoreRelease(s->wakeup_out_sema);
    s->wakeup_out_state = true;
    osiPmWakeLock(s->pm_wakeup_out);
}

void srv2LineWakeUpOutSetSleepTimer(void)
{
    srv2LineWakeUp_t *s = &gSrv2LineWakeUp;

    if (!s->init)
        return;
    osiTimerStart(s->wakeup_out_sleep_timer, s->wakeup_outsleep_timeout_ms);
}

void srv2LineWakeUpMcuNotifySleep(int mode, uint32_t delay_ms)
{
    srv2LineWakeUp_t *s = &gSrv2LineWakeUp;

    if (!s->init)
        return;

    s->mcu_notify_sleep_mode = mode;
    s->mcu_notify_sleep_delay_ms = delay_ms;

    if (mode == 1)
        osiPmWakeLock(s->pm_wakeup_at_control);
    else
        osiPmWakeUnlock(s->pm_wakeup_at_control);
}

void srv2LineWakeUpInit(int mode, uint32_t delay_ms)
{
    srv2LineWakeUp_t *s = &gSrv2LineWakeUp;
    s->wakeup_out_state = false;
    s->wakeup_out_sema = osiSemaphoreCreate(1, 1);
    s->wakeup_outsleep_timeout_ms = WAKEUP_OUTSLEEP_TIMEOUT_MS;
    s->mcu_notify_sleep_mode = mode;
    s->mcu_notify_sleep_delay_ms = delay_ms;

    s->pm_wakeup_out = osiPmSourceCreate(PM_WAKEUP_OUT_NAME, &_wakeUpOutPmOps, s);
    s->pm_wakeup_in = osiPmSourceCreate(PM_WAKEUP_IN_NAME, NULL, NULL);
    s->pm_wakeup_at_control = osiPmSourceCreate(PM_WAKEUP_AT_CONTROL, NULL, NULL);
    s->wakeup_out_sleep_timer = osiTimerCreate(NULL, prvWakeUpOutSleepTimeout, s);
    s->wakeup_in_sleep_timer = osiTimerCreate(NULL, prvWakeUpInSleepTimeout, s);
    osiPmWakeLock(s->pm_wakeup_in);
    osiTimerStart(s->wakeup_in_sleep_timer, s->mcu_notify_sleep_delay_ms);

    drvGpioConfig_t cfg_irq = {
        .mode = DRV_GPIO_INPUT,
        .intr_enabled = true,
        .rising = true,
        .falling = true,
    };

    drvGpioConfig_t cfg_output = {0};
    cfg_output.mode = DRV_GPIO_OUTPUT;

    s->wakeup_in = drvGpioOpen(CONFIG_TWOLINE_WAKEUP_IN_GPIO, &cfg_irq, prvWakeUpInGpioIsr, s);
    s->wakeup_out = drvGpioOpen(CONFIG_TWOLINE_WAKEUP_OUT_GPIO, &cfg_output, NULL, NULL);
    s->init = true;
    OSI_LOGI(0, "srv2LineWakeUpInit wekeInGpioId = %d, wakeOutGpioId %d",
             CONFIG_TWOLINE_WAKEUP_IN_GPIO, CONFIG_TWOLINE_WAKEUP_OUT_GPIO);
}

void srv2LineWakeUpDelete(void)
{
    srv2LineWakeUp_t *s = &gSrv2LineWakeUp;
    if (!s->init)
        return;

    osiPmSourceDelete(s->pm_wakeup_out);
    osiPmSourceDelete(s->pm_wakeup_in);
    osiSemaphoreDelete(s->wakeup_out_sema);
    osiTimerDelete(s->wakeup_out_sleep_timer);
    osiTimerDelete(s->wakeup_in_sleep_timer);
    drvGpioClose(s->wakeup_in);
    drvGpioClose(s->wakeup_out);
    s->init = false;
}

#endif // CONFIG_TWOLINE_WAKEUP_ENABLE
