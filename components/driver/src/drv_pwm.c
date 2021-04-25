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
#include "hwregs.h"
#include "drv_names.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "drv_pwm.h"
#include "osi_log.h"
#include "osi_api.h"
#include "hal_chip.h"
#include "hal_iomux.h"
#include "osi_compiler.h"
#include "quec_proj_config.h"
#include "quec_pin_index.h"
#include "ql_gpio.h"
#include "ql_api_osi.h"

static osiClockConstrainRegistry_t gPwmCLK = {.tag = HAL_NAME_PWM};

struct drvPwm
{
    bool opened;
    bool clk_lock;
    bool pwt_start;
    HWP_PWM_T *hw_pwm;
    osiPmSource_t *pm_source;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
	bool lgp_strat;
#endif
};

#ifndef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG
static drvPwm_t gDrvPWMCtx = {
    .opened = false,
    .clk_lock = true,
    .pwt_start = false,
    .hw_pwm = (HWP_PWM_T *)hwp_pwm,
    .pm_source = NULL,
};
#else
static drvPwm_t gDrvPWTCtx = {
    .opened = false,
    .clk_lock = true,
    .pwt_start = false,
    .hw_pwm = (HWP_PWM_T *)hwp_pwm,
    .pm_source = NULL,
};

static drvPwm_t gDrvLPGCtx = {
    .opened = false,
    .clk_lock = false,
    .pwt_start = false,
    .hw_pwm = (HWP_PWM_T *)hwp_pwm,
    .pm_source = NULL,
};

static drvPwm_t gDrvPWL0Ctx = {
    .opened = false,
    .clk_lock = true,
    .pwt_start = false,
    .hw_pwm = (HWP_PWM_T *)hwp_pwm,
    .pm_source = NULL,
};

static drvPwm_t gDrvPWL1Ctx = {
    .opened = false,
    .clk_lock = true,
    .pwt_start = false,
    .hw_pwm = (HWP_PWM_T *)hwp_pwm,
    .pm_source = NULL,
};
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
ql_timer_t pwm_sleep_timer = NULL;

static void pwm_sleep_timer_handler(void *ctx)
{
	drvPwm_t *d = (drvPwm_t *)ctx;

	if (d != NULL && d->opened)
    {
		if(d->lgp_strat || d->pwt_start)
		{
			REG_SYS_CTRL_CLK_AON_APB_MODE_T clkApbMode = {hwp_sysCtrl->clk_aon_apb_mode};
			clkApbMode.b.mode_aon_apb_clk_id_pwm = 1;
			hwp_sysCtrl->clk_aon_apb_mode = clkApbMode.v;
			ql_pin_set_func(QUEC_PIN_NET_STATUS, 1);
		}
    }
	if(pwm_sleep_timer){
		ql_rtos_timer_delete(pwm_sleep_timer);
		pwm_sleep_timer = NULL;
	}
}
#endif

static void prvPwtSuspend(drvPwm_t *d)
{
    OSI_LOGI(0x10007dbd, "pwm suspend");
    REG_PWM_PWT_CONFIG_T pwtcfg;
    if (d != NULL && d->opened)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
		(void)pwtcfg;
		if(!pwm_sleep_timer){
			ql_rtos_timer_create(&pwm_sleep_timer, QL_TIMER_IN_SERVICE, pwm_sleep_timer_handler, (void *)d);
		}
		else{
			ql_rtos_timer_stop(pwm_sleep_timer);
		}
		if(d->lgp_strat || d->pwt_start)
		{
			REG_SYS_CTRL_CLK_AON_APB_MODE_T clkApbMode = {hwp_sysCtrl->clk_aon_apb_mode};
			clkApbMode.b.mode_aon_apb_clk_id_pwm = 0;
			hwp_sysCtrl->clk_aon_apb_mode = clkApbMode.v;
			ql_pin_set_func(QUEC_PIN_NET_STATUS, 0);
			ql_gpio_set_direction(QUEC_GPIO_NET_STATUS, 0);
		}
#else
        if (d->pwt_start)
        {
            pwtcfg.v = d->hw_pwm->pwt_config;
            pwtcfg.b.pwt_enable = 0;
            d->hw_pwm->pwt_config = pwtcfg.v;
        }
#endif
    }
}

static void prvPwtResume(drvPwm_t *d)
{
    OSI_LOGI(0x10007dbe, "pwm Resume");
    REG_PWM_PWT_CONFIG_T pwtcfg;
    if (d != NULL && d->opened)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
	(void)pwtcfg;
	if(pwm_sleep_timer){
		ql_rtos_timer_start_relaxed(pwm_sleep_timer, 2000, 0, QL_WAIT_FOREVER); //2秒内无再进休眠，重新打开pwm
	}
	else{
		pwm_sleep_timer_handler((void *)d);
	}
#else
        if (d->pwt_start)
        {
            pwtcfg.v = d->hw_pwm->pwt_config;
            pwtcfg.b.pwt_enable = 1;
            d->hw_pwm->pwt_config = pwtcfg.v;
        }
#endif		
    }
}

static void _pwmSuspend(void *ctx, osiSuspendMode_t mode)
{
    drvPwm_t *d = (drvPwm_t *)ctx;
    prvPwtSuspend(d);
}

static void _pwmResume(void *ctx, osiSuspendMode_t mode, uint32_t source)
{
    drvPwm_t *d = (drvPwm_t *)ctx;
    prvPwtResume(d);
}

static const osiPmSourceOps_t _pwmPmOps = {
    .suspend = _pwmSuspend,
    .resume = _pwmResume,
};

static inline drvPwm_t *prvPWMGetByName(uint32_t name)
{
#ifndef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG
    if (name == DRV_NAME_PWM1)
        return &gDrvPWMCtx;
#else
    if (name == DRV_NAME_PWT)
    {
        return &gDrvPWTCtx;    
    }
	else if (name == DRV_NAME_LPG)
    {
        return &gDrvLPGCtx;     
    }
	else if (name == DRV_NAME_PWL0)
    {
        return &gDrvPWL0Ctx;     
    }
	else if (name == DRV_NAME_PWL1)
    {
        return &gDrvPWL1Ctx;     
    }
#endif	
    return NULL;
}

drvPwm_t *
drvPwmAcquire(uint32_t name)
{
    drvPwm_t *d = prvPWMGetByName(name);
    if (d == NULL)
        return NULL;

    uint32_t critical = osiEnterCritical();

    if (!d->opened)
    {
        d->pm_source = osiPmSourceCreate(name, &_pwmPmOps, d);
        if (d->pm_source != NULL)
        {
            d->opened = true;
            if (d->clk_lock)
            {
                OSI_LOGI(0x10007dbf, "pwm  lock");

#ifndef CONFIG_QUEC_PROJECT_FEATURE
                osiPmWakeLock(d->pm_source);
#else
				osiPmWakeUnlock(d->pm_source);
#endif

                osiRequestSysClkActive(&gPwmCLK);
            }
        }
        else
            return NULL;
    }
    osiExitCritical(critical);
    return d;
}

void drvPwtSetPeriodDuty(drvPwm_t *d, uint8_t clk_div, uint32_t pwm_count, uint32_t duty)
{
    REG_PWM_PWT_CONFIG_T pwtcfg;
    REG_SYS_CTRL_CFG_CLK_PWM_T pwm_divider = {hwp_sysCtrl->cfg_clk_pwm};
    OSI_ASSERT(pwm_count < (1 << 11), "PWM pwt pwt_period set fail");
    OSI_ASSERT(duty < (1 << 11), "PWM pwt duty set fail");
    if (d != NULL && d->opened)
    {
        if (pwm_count == 0)
        {
            hwp_pwm->pwt_config = 0;
        }
        else
        {
            pwm_divider.b.pwm_divider = clk_div;
            pwtcfg.v = d->hw_pwm->pwt_config;
            pwtcfg.b.pwt_period = pwm_count;
            pwtcfg.b.pwt_duty = duty;
            d->hw_pwm->pwt_config = pwtcfg.v;
            hwp_sysCtrl->cfg_clk_pwm = pwm_divider.v;
        }
    }
}

bool drvPwtStart(drvPwm_t *d)
{
    REG_PWM_PWT_CONFIG_T pwtcfg;
    REG_SYS_CTRL_CLK_AON_APB_MODE_T clkApbMode = {hwp_sysCtrl->clk_aon_apb_mode};
    if (d != NULL && d->opened)
    {
        pwtcfg.v = d->hw_pwm->pwt_config;
        clkApbMode.b.mode_aon_apb_clk_id_pwm = 1;
        hwp_sysCtrl->clk_aon_apb_mode = clkApbMode.v;
        pwtcfg.b.pwt_enable = 1;
        d->hw_pwm->pwt_config = pwtcfg.v;
        d->pwt_start = true;
    }
    else
    {
        return false;
    }
    return true;
}

bool drvPwtStop(drvPwm_t *d)
{
    REG_SYS_CTRL_CLK_AON_APB_MODE_T clkApbMode = {hwp_sysCtrl->clk_aon_apb_mode};
    if (d != NULL && d->opened)
    {
        clkApbMode.b.mode_aon_apb_clk_id_pwm = 0;
        hwp_sysCtrl->clk_aon_apb_mode = clkApbMode.v;
        d->hw_pwm->pwt_config = 0;
        d->pwt_start = false;
    }
    else
    {
        return false;
    }
    return true;
}

bool drvLgpStart(drvPwm_t *d, drvLpgPer_t period, drvLpgOntime_t OnTime)
{
    REG_SYS_CTRL_CLK_AON_APB_MODE_T clkApbMode = {hwp_sysCtrl->clk_aon_apb_mode};
    REG_PWM_LPG_CONFIG_T LgpConfig;
    if (d != NULL && d->opened)
    {
        clkApbMode.b.mode_aon_apb_clk_id_pwm = 1;
        LgpConfig.v = d->hw_pwm->lpg_config;
        LgpConfig.b.lpg_reset_l = 1;
        LgpConfig.b.lpg_ontime = OnTime;
        LgpConfig.b.lpg_period = period;
        hwp_sysCtrl->clk_aon_apb_mode = clkApbMode.v;
        d->hw_pwm->lpg_config = LgpConfig.v;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
		d->lgp_strat = 1;
#endif
    }
    else
    {
        return false;
    }
    return true;
}

bool drvLgpStop(drvPwm_t *d)
{
    REG_SYS_CTRL_CLK_AON_APB_MODE_T clkApbMode = {hwp_sysCtrl->clk_aon_apb_mode};
    if (d != NULL && d->opened)
    {
        clkApbMode.b.mode_aon_apb_clk_id_pwm = 0;
        hwp_sysCtrl->clk_aon_apb_mode = clkApbMode.v;
        d->hw_pwm->lpg_config = 0;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
		d->lgp_strat = 0;
#endif
    }
    else
    {
        return false;
    }
    return true;
}

bool drvPwlStart(drvPwm_t *d, drvPwlId_t pwl_id, uint8_t level)
{
    if (d != NULL && d->opened)
    {
        if (pwl_id != PWL_ID_0 && pwl_id != PWL_ID_1)
        {
            OSI_LOGI(0x10007dc0, "PWM pwl id error");
            return false;
        }
        if (pwl_id == PWL_ID_0)
        {
            REG_PWM_PWL0_CONFIG_T pwl0;
            pwl0.v = 0;
            pwl0.b.pwl0_set_oe = 1;
            if (level == 0)
            {
                pwl0.b.pwl0_force_l = 1;
            }
            else if (level == 0xff)
            {
                pwl0.b.pwl0_force_h = 1;
            }
            else
            {
                pwl0.b.pwl0_en_h = 1;
                pwl0.b.pwl_min = level;
            }
            d->hw_pwm->pwl0_config = pwl0.v;
        }
        else
        {
            REG_PWM_PWL1_CONFIG_T pwl1;
            pwl1.v = 0;
            pwl1.b.pwl1_set_oe = 1;
            if (level == 0)
            {
                pwl1.b.pwl1_force_l = 1;
            }
            else if (level == 0xff)
            {
                pwl1.b.pwl1_force_h = 1;
            }
            else
            {
                pwl1.b.pwl1_en_h = 1;
                pwl1.b.pwl1_threshold = level;
            }
            d->hw_pwm->pwl1_config = pwl1.v;
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool drvPwlStop(drvPwm_t *d, drvPwlId_t pwl_id)
{
    if (d != NULL && d->opened)
    {
        if (pwl_id != PWL_ID_0 && pwl_id != PWL_ID_1)
        {
            OSI_LOGI(0x10007dc0, "PWM pwl id error");
            return false;
        }
        if (pwl_id == PWL_ID_0)
        {
            d->hw_pwm->pwl0_config = PWM_PWL0_CLR_OE | PWM_PWL0_FORCE_L;
        }
        else
        {
            d->hw_pwm->pwl1_config = PWM_PWL1_CLR_OE | PWM_PWL1_FORCE_L;
        }
    }
    else
    {
        return false;
    }
    return true;
}

void drvPwmRelease(drvPwm_t *d)
{
    if (d != NULL)
    {
        if (d->opened)
        {
            if (d->pm_source)
            {
                if (d->clk_lock)
                {
                    OSI_LOGI(0x10007dc1, "pwm release lock");
                    osiPmWakeUnlock(d->pm_source);
                    osiReleaseClk(&gPwmCLK);
                }
                osiPmSourceDelete(d->pm_source);
            }
            d->opened = false;
            d->pm_source = NULL;
        }
    }
}
