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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('S', 'D', 'T', 'R')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "srv_dtr.h"
#include "drv_gpio.h"
#include "osi_log.h"
#include <string.h>

#include "quec_proj_config.h"
#include "quec_pin_cfg.h"
#include "ql_gpio_internal.h"
#include "hal_iomux.h"

#ifdef CONFIG_SRV_DTR_ENABLE

typedef struct
{
    bool started;
    uint8_t level;
    drvGpio_t *gpio;
    osiThread_t *thread;
    srvDtrCallback_t rising_cb;
    srvDtrCallback_t falling_cb;
    void *cb_ctx;
    osiWork_t *work;
    osiTimer_t *debounce_timer;
} srvDtrContext_t;

// In BSS, started will be false at initial
static srvDtrContext_t gSrvDtrCtx;

static void prvSendEvent(void *param)
{
    srvDtrContext_t *d = &gSrvDtrCtx;
    uint8_t level = drvGpioRead(d->gpio) ? 1 : 0;
    if (level == d->level)
        return;

    OSI_LOGI(0, "DTR GPIO %d->%d", d->level, level);

    d->level = level;
    if (d->level)
    {
        if (d->thread != NULL && d->rising_cb != NULL)
            osiThreadCallback(d->thread, d->rising_cb, d->cb_ctx);
    }
    else
    {
        if (d->thread != NULL && d->falling_cb != NULL)
            osiThreadCallback(d->thread, d->falling_cb, d->cb_ctx);
    }
}

static void prvTriggerWork(void *param)
{
    srvDtrContext_t *d = &gSrvDtrCtx;
    osiWorkEnqueue(d->work, osiSysWorkQueueHighPriority());
}

static void prvGpioCB(void *param)
{
#ifdef CONFIG_SRV_DTR_DEBOUNCE_ENABLE
    srvDtrContext_t *d = &gSrvDtrCtx;
    osiTimerStart(d->debounce_timer, CONFIG_SRV_DTR_DEBOUNCE_TIME);
#else
    prvTriggerWork(NULL);
#endif
}

bool srvDtrStart(osiThread_t *thread, srvDtrCallback_t rising_cb,
                 srvDtrCallback_t falling_cb, void *cb_ctx)
{
    srvDtrContext_t *d = &gSrvDtrCtx;
    if ((rising_cb != NULL || falling_cb != NULL) && thread == NULL)
        return false;

    if (d->started)
        return false;

#ifdef CONFIG_SRV_DTR_DEBOUNCE_ENABLE
    d->debounce_timer = osiTimerCreate(NULL, prvTriggerWork, NULL);
#endif

    d->level = 0xff;
    d->thread = thread;
    d->rising_cb = rising_cb;
    d->falling_cb = falling_cb;
    d->cb_ctx = cb_ctx;
    d->work = osiWorkCreate(prvSendEvent, NULL, NULL);

    drvGpioConfig_t cfg = {
        .mode = DRV_GPIO_INPUT,
        .intr_enabled = true,
        .intr_level = false,
        .debounce = true,
        .rising = true,
        .falling = true,
    };
    d->gpio = drvGpioOpen(CONFIG_SRV_DTR_GPIO, &cfg, prvGpioCB, NULL);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO
    extern drvGpio_t *quec_GPIO[GPIO_MAX];
    quec_GPIO[CONFIG_SRV_DTR_GPIO] = d->gpio;
#endif
    if (d->gpio == NULL)
    {
        d->started = false;
#ifdef CONFIG_SRV_DTR_DEBOUNCE_ENABLE
        osiTimerDelete(d->debounce_timer);
#endif
        osiWorkDelete(d->work);
        return false;
    }

    d->started = true;
    return true;
}

void srvDtrStop(void)
{
    srvDtrContext_t *d = &gSrvDtrCtx;
    if (!d->started)
        return;

#ifdef CONFIG_SRV_DTR_DEBOUNCE_ENABLE
    osiTimerDelete(d->debounce_timer);
#endif
    osiWorkDelete(d->work);
    drvGpioClose(d->gpio);

    memset(d, 0, sizeof(*d));
}

bool srvDtrIsHigh(void)
{
    srvDtrContext_t *d = &gSrvDtrCtx;
    if (d->started && d->gpio != NULL)
        return drvGpioRead(d->gpio);
    return false;
}

#endif
