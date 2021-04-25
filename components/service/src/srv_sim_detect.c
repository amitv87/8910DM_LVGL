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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('S', 'I', 'M', 'D')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "srv_sim_detect.h"
#include "hal_config.h"
#include "hal_iomux.h"
#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPIO
#include "drv_gpio.h"
#endif
#include "osi_api.h"
#include "osi_log.h"
#include "osi_sysnv.h"
#include <stdio.h>

#include "quec_pin_cfg.h"
#include "ql_gpio_internal.h"

#define CONFIG_SIM_DETECT_DEBOUNCE_TIMEOUT (400) // ms
#define CONFIG_SIM_DETECT_DROPOUT_TIMEOUT (2000) // ms

#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPIO
typedef struct
{
    int sim_id;
    simDetectCB_t cb;
    bool connected;
    bool connect_debounce;
    osiTimer_t *debounce_timer;
    drvGpio_t *gpio;
} simDetect_t;
#endif

static void _debounceCB(void *sd_)
{
    simDetect_t *sd = (simDetect_t *)sd_;

    if (sd->connect_debounce == false)
    {
        srvSimDetectSwitch(sd->sim_id, true); //switch on interrupt
    }
    else
    {
        sd->connect_debounce = false;
    }
    if (!((drvGpioRead(sd->gpio) ^ gSysnvSimVoltTrigMode)))
    {
        OSI_LOGI(0, "SIM %d Plug In", sd->sim_id);
        if (sd->cb)
            sd->cb(sd->sim_id, true);
    }
}

static void _simDetectCB(void *sd_)
{
    simDetect_t *sd = (simDetect_t *)sd_;
    bool connect = drvGpioRead(sd->gpio);
    OSI_LOGI(0, "SIM %d interrupt occurred %d %d", sd->sim_id, connect, sd->connect_debounce);
    if (connect == sd->connected)
        return;

#ifdef CONFIG_APP_SSIM_SUPPORT
    OSI_LOGI(0, "Soft sim hot plug");
    osiShutdown(OSI_SHUTDOWN_RESET);
    return;
#endif

    sd->connected = connect;
    if (!(sd->connected ^ gSysnvSimVoltTrigMode))
    {
        sd->connect_debounce = true;
        osiTimerStart(sd->debounce_timer, CONFIG_SIM_DETECT_DEBOUNCE_TIMEOUT);
    }
    else
    {
        if (sd->connect_debounce)
            osiTimerStop(sd->debounce_timer);
        else
        {
            OSI_LOGI(0, "SIM %d Plug Out", sd->sim_id);
            srvSimDetectSwitch(sd->sim_id, false); //switch off interrupt
            osiTimerStart(sd->debounce_timer, CONFIG_SIM_DETECT_DROPOUT_TIMEOUT);
            if (sd->cb)
                sd->cb(sd->sim_id, false);
        }
        sd->connect_debounce = false;
    }
}
static bool _simDetectInit(simDetect_t *sd, int gpio_id)
{
    drvGpioConfig_t cfg = {
        .mode = DRV_GPIO_INPUT,
        .intr_enabled = false,
        .intr_level = false,
        .rising = true,
        .falling = true,
        .debounce = true,
    };

    sd->debounce_timer = osiTimerCreate(NULL, _debounceCB, sd);
    if (sd->debounce_timer == NULL)
        return false;

    sd->gpio = drvGpioOpen(gpio_id, &cfg, _simDetectCB, sd);
    if (sd->gpio == NULL)
        goto gpio_open_fail;

    // Force pull none is needed for sim detect. Though it should be
    // configured by iomux function properties, call it again here.
#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPIO
    halIomuxSetPadPull(HAL_IOMUX_FUN_GPIO(gpio_id), HAL_IOMUX_FORCE_PULL_NONE);
#else
    if (true == gSysnvSimVoltTrigMode)
    {
        ql_gpio_set_pull(gpio_id, PULL_DOWN);
    }
    else
    {
        ql_gpio_set_pull(gpio_id, PULL_UP);
    }
#endif

    sd->connected = drvGpioRead(sd->gpio);
    (gSysnvSimHotPlug == true) ? (cfg.intr_enabled = true) : (cfg.intr_enabled = false);
    if (drvGpioReconfig(sd->gpio, &cfg))
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO
        extern drvGpio_t *quec_GPIO[GPIO_MAX];
        quec_GPIO[gpio_id] = sd->gpio;
#endif
        return true;
    }

    drvGpioClose(sd->gpio);
    sd->gpio = NULL;

gpio_open_fail:
    osiTimerDelete(sd->debounce_timer);
    return false;
}

static simDetect_t gSim[2] = {};
bool srvSimDetectRegister(int id, int gpio_id, simDetectCB_t cb)
{
    if (id != 1 && id != 2)
        return false;

    simDetect_t *sim = &gSim[id - 1];
    if (sim->gpio != NULL)
    {
        drvGpioClose(sim->gpio);
        osiTimerDelete(sim->debounce_timer);
    }
    sim->debounce_timer = NULL;
    sim->gpio = NULL;
    sim->sim_id = id;
    sim->cb = cb;
    return _simDetectInit(sim, gpio_id);
}

bool srvSimDetectSwitch(int id, bool int_status)
{
    if (id != 1 && id != 2)
        return false;

    simDetect_t *sim = &gSim[id - 1];
    if (sim->gpio == NULL)
    {
        return false;
    }
    drvGpioConfig_t cfg = {
        .mode = DRV_GPIO_INPUT,
        .intr_enabled = false,
        .intr_level = false,
        .rising = true,
        .falling = true,
        .debounce = true,
    };
    (int_status == true) ? (cfg.intr_enabled = true) : (cfg.intr_enabled = false);
    sim->connected = drvGpioRead(sim->gpio);
    return drvGpioReconfig(sim->gpio, &cfg);
}
