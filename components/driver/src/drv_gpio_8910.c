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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('G', 'P', 'I', 'O')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_gpio.h"
#include "drv_names.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hwregs.h"
#include "hal_iomux.h"
#include <string.h>
#include <stdlib.h>

#if defined(CONFIG_SOC_8910)

#define GPIO_COUNT (32)

struct drvGpio
{
    uint32_t id;

    drvGpioMode_t mode;
    bool intr_enabled;
    bool intr_level;
    bool debounce;
    bool rising;
    bool falling;

    drvGpioIntrCB_t cb;
    void *cb_ctx;
};

typedef struct
{
    uint32_t oen;
    uint32_t val;
    uint32_t int_ctrl_r;
    uint32_t int_ctrl_f;
    uint32_t int_mode;
    uint32_t dbn;
} drvGpioSuspend_t;

typedef struct
{
    drvGpio_t *gpios[GPIO_COUNT];
    drvGpioSuspend_t suspend;
    osiPmSource_t *pm_source;
} drvGpioContext_t;

static drvGpioContext_t gDrvGpioCtx;

static void _gpioSuspend(void *ctx, osiSuspendMode_t mode)
{
    drvGpioContext_t *p = &gDrvGpioCtx;
    p->suspend.oen = hwp_gpio1->gpio_oen_val;
    p->suspend.val = hwp_gpio1->gpio_val_reg;
    p->suspend.int_ctrl_r = hwp_gpio1->gpint_ctrl_r_set_reg;
    p->suspend.int_ctrl_f = hwp_gpio1->gpint_ctrl_f_set_reg;
    p->suspend.int_mode = hwp_gpio1->gpint_mode_set_reg;
    p->suspend.dbn = hwp_gpio1->dbn_en_set_reg;
}

static void _gpioResume(void *ctx, osiSuspendMode_t mode, uint32_t source)
{
    if (source & OSI_RESUME_ABORT)
        return;

    drvGpioContext_t *p = &gDrvGpioCtx;
    hwp_gpio1->gpint_ctrl_r_clr_reg = 0xffffffff;
    hwp_gpio1->gpint_ctrl_f_clr_reg = 0xffffffff;
    hwp_gpio1->dbn_en_clr_reg = 0xffffffff;
    hwp_gpio1->gpint_mode_clr_reg = 0xffffffff;

    hwp_gpio1->gpio_oen_val = p->suspend.oen;
    hwp_gpio1->gpio_val_reg = p->suspend.val;
    hwp_gpio1->dbn_en_set_reg = p->suspend.dbn;
    hwp_gpio1->gpint_mode_set_reg = p->suspend.int_mode;
    hwp_gpio1->gpint_ctrl_r_set_reg = p->suspend.int_ctrl_r;
    hwp_gpio1->gpint_ctrl_f_set_reg = p->suspend.int_ctrl_f;
}

static const osiPmSourceOps_t _gpioPmOps = {
    .suspend = _gpioSuspend,
    .resume = _gpioResume,
};

static bool _isConfigValid(uint32_t id, drvGpioConfig_t *cfg)
{
    if (id >= GPIO_COUNT || cfg == NULL)
        return false;

    return true;
}

static void _gpioISR(void *ctx)
{
    drvGpioContext_t *p = &gDrvGpioCtx;
    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t status = hwp->int_status;
    hwp->int_clr = status;

    OSI_LOGD(0, "GPIO int status 0x%x", status);
    while (status != 0)
    {
        uint32_t nclz = __builtin_clz(status);
        uint32_t id = 31 - nclz;
        status &= ~(1 << id);

        drvGpio_t *gpio = p->gpios[id];
        if (gpio != NULL && gpio->cb != NULL)
            gpio->cb(gpio->cb_ctx);
    }
}

void drvGpioInit(void)
{
    drvGpioContext_t *p = &gDrvGpioCtx;
    memset(p->gpios, 0, sizeof(p->gpios));

    hwp_gpio1->gpio_clr_reg = 0xffffffff;
    osiIrqSetPriority(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GPIO_1), SYS_IRQ_PRIO_GPIO_1);
    osiIrqSetHandler(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GPIO_1), _gpioISR, (void *)0);
    osiIrqEnable(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GPIO_1));

    p->pm_source = osiPmSourceCreate(DRV_NAME_GPIO, &_gpioPmOps, NULL);
}

drvGpio_t *drvGpioOpen(uint32_t id, drvGpioConfig_t *cfg, drvGpioIntrCB_t cb, void *cb_ctx)
{
    drvGpioContext_t *p = &gDrvGpioCtx;

    uint32_t critical = osiEnterCritical();
    if (!_isConfigValid(id, cfg))
    {
        osiExitCritical(critical);
        return NULL;
    }

    if (p->gpios[id] != NULL)
    {
        osiExitCritical(critical);
        return p->gpios[id];
    }

    drvGpio_t *d = (drvGpio_t *)calloc(1, sizeof(drvGpio_t));
    if (d == NULL)
    {
        osiExitCritical(critical);
        return NULL;
    }

    d->id = id;
    d->cb = cb;
    d->cb_ctx = cb_ctx;
    p->gpios[id] = d;
    drvGpioReconfig(d, cfg);

#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPIO
    // GPIO must be contiguous
    halIomuxSetFunction(HAL_IOMUX_FUN_GPIO_0 + id);
#endif

    osiExitCritical(critical);
    return d;
}

bool drvGpioReconfig(drvGpio_t *d, drvGpioConfig_t *cfg)
{
    if (d == NULL || !_isConfigValid(d->id, cfg))
        return false;

    uint32_t critical = osiEnterCritical();

    d->mode = cfg->mode;
    d->intr_enabled = cfg->intr_enabled;
    d->intr_level = cfg->intr_level;
    d->debounce = cfg->debounce;
    d->rising = cfg->rising;
    d->falling = cfg->falling;

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;

    hwp->gpint_ctrl_r_clr_reg = (1 << lid);
    hwp->gpint_ctrl_f_clr_reg = (1 << lid);
    hwp->dbn_en_clr_reg = (1 << lid);
    hwp->gpint_mode_clr_reg = (1 << lid);

    if (cfg->mode == DRV_GPIO_INPUT)
    {
        hwp->gpio_oen_set_in = (1 << lid);
    }
    else
    {
        if (cfg->out_level)
            hwp->gpio_set_reg = (1 << lid);
        else
            hwp->gpio_clr_reg = (1 << lid);
        hwp->gpio_oen_set_out = (1 << lid);
    }

    if (cfg->intr_enabled)
    {
        if (cfg->debounce)
            hwp->dbn_en_set_reg = (1 << lid);
        if (cfg->intr_level)
            hwp->gpint_mode_set_reg = (1 << lid);
        if (cfg->rising)
            hwp->gpint_ctrl_r_set_reg = (1 << lid);
        if (cfg->falling)
            hwp->gpint_ctrl_f_set_reg = (1 << lid);
    }

    osiExitCritical(critical);
    return true;
}

void drvGpioClose(drvGpio_t *d)
{
    drvGpioContext_t *p = &gDrvGpioCtx;
    uint32_t critical = osiEnterCritical();

    if (d == NULL || d->id >= GPIO_COUNT || d != p->gpios[d->id])
    {
        osiExitCritical(critical);
        return;
    }

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;

    hwp->gpint_ctrl_r_clr_reg = (1 << lid);
    hwp->gpint_ctrl_f_clr_reg = (1 << lid);
    hwp->dbn_en_clr_reg = (1 << lid);
    hwp->gpint_mode_clr_reg = (1 << lid);

    hwp->gpio_oen_set_in = (1 << lid);
    free(d);
    p->gpios[lid] = NULL;
    osiExitCritical(critical);
}

bool drvGpioRead(drvGpio_t *d)
{
    drvGpioContext_t *p = &gDrvGpioCtx;
    uint32_t critical = osiEnterCritical();

    if (d == NULL || d->id >= GPIO_COUNT || d != p->gpios[d->id])
    {
        osiExitCritical(critical);
        return false;
    }

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPIO
    bool res = hwp->gpio_val_reg & (1 << lid);
#else
    bool res;
    if (d->mode == DRV_GPIO_INPUT)
    {
        res = hwp->gpio_val_reg & (1 << lid);
    }
    else
    {
        res = hwp->gpio_set_reg & (1 << lid);
    }
#endif
    osiExitCritical(critical);
    return res;
}

void drvGpioWrite(drvGpio_t *d, bool level)
{
    drvGpioContext_t *p = &gDrvGpioCtx;
    uint32_t critical = osiEnterCritical();

    if (d == NULL || d->id >= GPIO_COUNT || d != p->gpios[d->id])
    {
        osiExitCritical(critical);
        return;
    }

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;

    if (level)
        hwp->gpio_set_reg = (1 << lid);
    else
        hwp->gpio_clr_reg = (1 << lid);
    osiExitCritical(critical);
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO
void quec_drvGpiodirset(drvGpio_t *d, bool dir)
{
    drvGpioContext_t *p = &gDrvGpioCtx;
    uint32_t critical = osiEnterCritical();

    if (d == NULL || d->id >= GPIO_COUNT || d != p->gpios[d->id])
    {
        osiExitCritical(critical);
        return;
    }

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;
    d->mode = dir;

    if (dir)
        hwp->gpio_oen_set_out = (1 << lid);
    else
        hwp->gpio_oen_set_in = (1 << lid);
    osiExitCritical(critical);
}

bool quec_drvGpiodirget(drvGpio_t *d)
{
    drvGpioContext_t *p = &gDrvGpioCtx;
    uint32_t critical = osiEnterCritical();

    if (d == NULL || d->id >= GPIO_COUNT || d != p->gpios[d->id])
    {
        osiExitCritical(critical);
        return false;
    }

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;

    bool res = hwp->gpio_oen_val & (1 << lid);
    osiExitCritical(critical);
    /* gpio_oen_val: 1:INPUT  */
    /*               0:OUTPUT */
    return (!res);
}

bool quec_drvGpioRecallback(drvGpio_t *d, drvGpioIntrCB_t cb, void *cb_ctx)
{
    if (d == NULL || cb == NULL)
        return false;

    drvGpioContext_t *p = &gDrvGpioCtx;

    uint32_t critical = osiEnterCritical();

    d->cb = cb;
    d->cb_ctx = cb_ctx;
    p->gpios[d->id] = d;

    osiExitCritical(critical);
    return true;
}

void quec_GPIOintrenset(drvGpio_t *d, bool intr_en)
{
    d->intr_enabled = intr_en;
}

drvGpioMode_t quec_GPIOdirget(drvGpio_t *d)
{
    return d->mode;
}

bool quec_GPIOintrlvlget(drvGpio_t *d)
{
    return d->intr_level;
}

bool quec_GPIOdbnget(drvGpio_t *d)
{
    return d->debounce;
}

bool quec_GPIOrisingget(drvGpio_t *d)
{
    return d->rising;
}

bool quec_GPIOfallingget(drvGpio_t *d)
{
    return d->falling;
}

drvGpioIntrCB_t quec_GPIOcbget(drvGpio_t *d)
{
    return d->cb;
}

void *quec_GPIOcbctxget(drvGpio_t *d)
{
    return d->cb_ctx;
}

void quec_drvGpioIntrClose(drvGpio_t *d)
{
    drvGpioContext_t *p = &gDrvGpioCtx;
    uint32_t critical = osiEnterCritical();

    if (d == NULL || d->id >= GPIO_COUNT || d != p->gpios[d->id])
    {
        osiExitCritical(critical);
        return;
    }

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;

    hwp->gpint_ctrl_r_clr_reg = (1 << lid);
    hwp->gpint_ctrl_f_clr_reg = (1 << lid);
    hwp->dbn_en_clr_reg = (1 << lid);
    hwp->gpint_mode_clr_reg = (1 << lid);

    osiExitCritical(critical);
}
#endif

#endif
