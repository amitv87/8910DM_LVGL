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

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hwregs.h"
#include "drv_names.h"
#include "hal_chip.h"
#include "osi_clock.h"
#include "osi_api.h"
#include "hal_adi_bus.h"

#include <usb/usb_device.h>

typedef struct
{
    bool clk_constrain;
    osiClockConstrainRegistry_t usb_clk;
    osiPmSource_t *pm_lock;
} udcPlat_t;

static inline void _usbClkLock(udcPlat_t *p)
{
    uint32_t critical = osiEnterCritical();
    if (!p->clk_constrain)
    {
        p->clk_constrain = true;
        osiRequestSysClkActive(&p->usb_clk);
    }
    osiExitCritical(critical);
}

static inline void _usbClkUnlock(udcPlat_t *p)
{
    uint32_t critical = osiEnterCritical();
    if (p->clk_constrain)
    {
        osiReleaseClk(&p->usb_clk);
        p->clk_constrain = false;
    }
    osiExitCritical(critical);
}

static void _usbEnable_8910(udc_t *udc)
{
    REG_ANALOG_REG_USB_REG0_T ana_usb_reg0 = {};
    ana_usb_reg0.b.usb_phase_sel = 2;
    ana_usb_reg0.b.usb_cdr_gain = 9;
    hwp_analogReg->usb_reg0 = ana_usb_reg0.v;

    REG_ANALOG_REG_USB_REG1_T ana_usb_reg1 = {};
    ana_usb_reg1.b.usb_vref_ibit = 2;
    ana_usb_reg1.b.usb_vref_vbit = 4;
    ana_usb_reg1.b.usb_v575m_sel_bit = 4;
    ana_usb_reg1.b.usb_v125m_sel_bit = 4;
    ana_usb_reg1.b.usb_hs_lvlout_bit = 12;
    hwp_analogReg->usb_reg1 = ana_usb_reg1.v;

    hwp_analogReg->usb_digphy_ana1 = 0x3f800;

    REG_ANALOG_REG_USB_REG2_T ana_usb_reg2 = {};
    ana_usb_reg2.b.usb_pu_otg = 1;
    ana_usb_reg2.b.usb_pu_usb_dev = 1;
    ana_usb_reg2.b.usb_pu_pkd = 1;
    ana_usb_reg2.b.usb_pu_1v8 = 1;
    ana_usb_reg2.b.usb_pu_1v2 = 1;
    ana_usb_reg2.b.usb_pwr_on = 1;
    ana_usb_reg2.b.usb_pu_iref = 1;
    ana_usb_reg2.b.usb_pu_hsrx = 1;
    ana_usb_reg2.b.usb_pu_hstx = 1;
    ana_usb_reg2.b.usb_pu_lptx = 1;
    hwp_analogReg->usb_reg2 = ana_usb_reg2.v;

    hwp_analogReg->usb_reg3 = 0;

    REG_ANALOG_REG_USB_REG4_T ana_usb_reg4 = {};
    ana_usb_reg4.b.usb_io33_enable = 1;
    ana_usb_reg4.b.usb_res_term_bit = 2;
    ana_usb_reg4.b.usb_lptx_drv_sel = 0xff;
    hwp_analogReg->usb_reg4 = ana_usb_reg4.v;

    REG_ANALOG_REG_USB_PLL1_T analog_usb_pll1 = {};
    analog_usb_pll1.b.usb_pll_vreg_bit = 8;
    analog_usb_pll1.b.usb_reg_res_bit = 2;
    analog_usb_pll1.b.usb_pll_cpbias_ibit = 4;
    analog_usb_pll1.b.usb_pll_cpc2_ibit = 4;
    analog_usb_pll1.b.usb_pll_cpr2_ibit = 4;
    hwp_analogReg->usb_pll1 = analog_usb_pll1.v;

    REG_ANALOG_REG_USB_PLL2_T analog_usb_pll2 = {};
    analog_usb_pll2.b.usb_refmulti2_en = 1;
    analog_usb_pll2.b.usb_pll_clk_960m_en = 1;
    hwp_analogReg->usb_pll2 = analog_usb_pll2.v;

    // USB UTMI+ Dig regs
    hwp_analogReg->usb_digphy_ana1 = 0x0;
    hwp_analogReg->usb_digphy_ana2 = 0x0;
    hwp_analogReg->usb_digphy_ana3 = (1 << 5) | (1 << 6);

    // USB PLL SDM Regs
    REG_ANALOG_REG_SDM_USBPLL_REG0_T sdm_usbpll_reg0 = {};
    sdm_usbpll_reg0.b.sdm_usbpll_pu = 1;
    sdm_usbpll_reg0.b.sdm_usbpll_sdm_clk_sel_rst = 1;
    hwp_analogReg->sdm_usbpll_reg0 = sdm_usbpll_reg0.v;

    hwp_analogReg->sdm_usbpll_reg1 = 0x24ec4ec4;

    REG_ANALOG_REG_USB_MON_T usb_mon = {};
    REG_ANALOG_REG_SDM_USBPLL_REG2_T sdm_usbpll_reg2 = {};
    sdm_usbpll_reg2.b.sdm_usbpll_dither_bypass = 1;
    sdm_usbpll_reg2.b.sdm_usbpll_sdm_reset_time_sel = 1;
    sdm_usbpll_reg2.b.sdm_usbpll_sdmclk_sel_time_sel = 1;
    sdm_usbpll_reg2.b.sdm_usbpll_clk_gen_en_reg = 1;
    sdm_usbpll_reg2.b.sdm_usbpll_clkout_en_counter_sel = 2;
    sdm_usbpll_reg2.b.sdm_usbpll_lock_counter_sel = 1;
    hwp_analogReg->sdm_usbpll_reg2 = sdm_usbpll_reg2.v;
    while (!REG_FIELD_GET(hwp_analogReg->usb_mon, usb_mon, usb_pll_lock))
    {
    }
}

static void _usbDisable_8910(udc_t *udc)
{
    hwp_analogReg->usb_suspend = 0;

    hwp_analogReg->usb_digphy_ana1 = 0;
    hwp_analogReg->usb_digphy_ana2 = 0;
    hwp_analogReg->usb_digphy_ana3 = 0;
    hwp_analogReg->usb_digphy_ana4 = 0;

    REG_ANALOG_REG_SDM_USBPLL_REG0_T sdm_usbpll_reg0 = {.b.sdm_usbpll_sdm_clk_sel_rst = 1};
    hwp_analogReg->sdm_usbpll_reg0 = sdm_usbpll_reg0.v;
    hwp_analogReg->sdm_usbpll_reg2 = 0;

    hwp_analogReg->usb_pll1 = 0;
    hwp_analogReg->usb_pll2 = 0;

    hwp_analogReg->usb_reg1 = 0;
    hwp_analogReg->usb_reg2 = 0;
    hwp_analogReg->usb_reg3 = 0;
    hwp_analogReg->usb_reg4 = 0;
}

static void _setpower_8910(udc_t *udc, bool on)
{
    udcPlat_t *p = (udcPlat_t *)udc->platform_priv;
    halPmuSwitchPower(HAL_POWER_USB, on, on);
    halPmuSwitchPower(HAL_POWER_ANALOG, on, on);

    if (!on)
    {
        _usbClkUnlock(p);
        osiPmWakeUnlock(p->pm_lock);
        osiClear32KSleepFlag(HAL_RESUME_SRC_USB_MON);
    }
    else
    {
        osiDelayUS(300); // wait for power on

        osiSet32KSleepFlag(HAL_RESUME_SRC_USB_MON);
        osiPmWakeLock(p->pm_lock);
        _usbClkLock(p);
    }
}

static void _suspend_8910(udc_t *udc)
{
    udcPlat_t *p = (udcPlat_t *)udc->platform_priv;
    _usbClkUnlock(p);
    osiPmWakeUnlock(p->pm_lock);
}

static void _resume_8910(udc_t *udc)
{
    udcPlat_t *p = (udcPlat_t *)udc->platform_priv;
    osiPmWakeLock(p->pm_lock);
    _usbClkLock(p);
}

static void _setclock_8910(udc_t *udc, bool on)
{
    uint32_t critical = osiEnterCritical();
    if (on)
    {
        REG_SYS_CTRL_SYS_AHB_RST_SET_T ahb_rst_set = {};
        ahb_rst_set.b.set_sys_ahb_rst_id_usbc = 1;
        hwp_sysCtrl->sys_ahb_rst_set = ahb_rst_set.v;

        hwp_idle->idle_cg_sw_clr = 0x10;
        hwp_idle->idle_cg_sel_clr = 0x10;
        hwp_idle->pd_pll_sw_clr = 0x10;
        hwp_idle->pd_pll_sel_clr = 0x10;

        REG_SYS_CTRL_CLK_SYS_AHB_ENABLE_T ahb_enable = {};
        ahb_enable.b.enable_sys_ahb_clk_id_usbc = 1;
        hwp_sysCtrl->clk_sys_ahb_enable = ahb_enable.v;

        REG_SYS_CTRL_CLK_OTHERS_ENABLE_T others_enable = {};
        others_enable.b.enable_oc_id_usbphy = 1;
        hwp_sysCtrl->clk_others_enable = others_enable.v;

        REG_SYS_CTRL_CLK_OTHERS_1_ENABLE_T others_1_enable = {};
        others_1_enable.b.enable_oc_id_usb_utmi_48m = 1;
        hwp_sysCtrl->clk_others_1_enable = others_1_enable.v;

        REG_SYS_CTRL_SYS_AHB_RST_CLR_T ahb_rst_clr = {};
        ahb_rst_clr.b.clr_sys_ahb_rst_id_usbc = 1;
        hwp_sysCtrl->sys_ahb_rst_clr = ahb_rst_clr.v;
    }
    else
    {
        REG_SYS_CTRL_CLK_OTHERS_DISABLE_T others_disable = {};
        others_disable.b.disable_oc_id_usbphy = 1;
        hwp_sysCtrl->clk_others_disable = others_disable.v;

        REG_SYS_CTRL_CLK_OTHERS_1_DISABLE_T others_1_disable = {};
        others_1_disable.b.disable_oc_id_usb_utmi_48m = 1;
        others_1_disable.b.disable_oc_id_usb11_48m = 1;
        hwp_sysCtrl->clk_others_1_disable = others_1_disable.v;

        REG_SYS_CTRL_CLK_SYS_AHB_DISABLE_T ahb_disable = {};
        ahb_disable.b.disable_sys_ahb_clk_id_usbc = 1;
        ahb_disable.b.disable_sys_ahb_clk_id_usb11 = 1;
        hwp_sysCtrl->clk_sys_ahb_disable = ahb_disable.v;

        hwp_idle->idle_cg_sw_set = 0x10;
        hwp_idle->idle_cg_sel_set = 0x10;
        hwp_idle->pd_pll_sw_set = 0x10;
        hwp_idle->pd_pll_sel_set = 0x10;
    }
    osiExitCritical(critical);
}

bool udcPlatInit_rda(udc_t *udc)
{
    udcPlat_t *p = (udcPlat_t *)calloc(1, sizeof(udcPlat_t));
    if (p == NULL)
        return false;

    uint32_t critical = osiEnterCritical();
    p->pm_lock = osiPmSourceCreate(DRV_NAME_USB20, NULL, NULL);
    if (p->pm_lock == NULL)
    {
        osiExitCritical(critical);
        free(p);
        return false;
    }

    p->clk_constrain = false;
    p->usb_clk.tag = HAL_NAME_USB;

    udc->plat_ops.setclk = _setclock_8910;
    udc->plat_ops.setpower = _setpower_8910;
    udc->plat_ops.enable = _usbEnable_8910;
    udc->plat_ops.disable = _usbDisable_8910;
    udc->plat_ops.suspend = _suspend_8910;
    udc->plat_ops.resume = _resume_8910;

    udc->platform_priv = (unsigned long)p;
    osiExitCritical(critical);
    return true;
}

void udcPlatExit_rda(udc_t *udc)
{
    if (udc->platform_priv == 0)
        return;
    uint32_t critical = osiEnterCritical();
    udcPlat_t *p = (udcPlat_t *)udc->platform_priv;
    udc->platform_priv = 0;
    osiExitCritical(critical);
    osiPmSourceDelete(p->pm_lock);
    free(p);
}
