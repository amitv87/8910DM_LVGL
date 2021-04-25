/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "boot_platform.h"
#include "hal_adi_bus.h"
#include "hwregs.h"
#include "osi_compiler.h"
#include "osi_api.h"
#include "osi_tick_unit.h"
#include "hal_config.h"
#include "hal_blue_screen.h"
#include "cmsis_core.h"

#define PSM_MODE_REG (hwp_rda2720mPsm->reserved_2) // should be sync with psm sleep
#define PSM_WR_PROT_MAGIC (0x454e)

void bootReset(bootResetMode_t mode)
{
    hwp_sysCtrl->reg_dbg = SYS_CTRL_PROTECT_UNLOCK;

    REG_SYS_CTRL_RESET_CAUSE_T reset_cause = {};
    if (mode == BOOT_RESET_FORCE_DOWNLOAD)
    {
        reset_cause.v = hwp_sysCtrl->reset_cause;
        reset_cause.b.boot_mode |= 1;
        reset_cause.b.sw_boot_mode = 0;
        hwp_sysCtrl->reset_cause = reset_cause.v;

        REG_SYS_CTRL_APCPU_RST_SET_T apcpu_rst = {};
        apcpu_rst.b.set_global_soft_rst = 1;
        hwp_sysCtrl->apcpu_rst_set = apcpu_rst.v;

        OSI_DEAD_LOOP;
    }
    else
    {
        REG_RDA2720M_GLOBAL_SWRST_CTRL0_T swrst_ctrl0;
        swrst_ctrl0.b.reg_rst_en = 1;
        halAdiBusWrite(&hwp_rda2720mGlobal->swrst_ctrl0, swrst_ctrl0.v);

        REG_RDA2720M_GLOBAL_SOFT_RST_HW_T soft_rst_hw;
        soft_rst_hw.b.reg_soft_rst = 1;
        halAdiBusWrite(&hwp_rda2720mGlobal->soft_rst_hw, soft_rst_hw.v);
    }
}

void _psmPrepare(void)
{
    REG_RDA2720M_GLOBAL_SOFT_RST0_T soft_rst0;
    REG_RDA2720M_GLOBAL_MODULE_EN0_T module_en0;
    REG_RDA2720M_PSM_PSM_CTRL_T psm_ctrl;
    REG_RDA2720M_PSM_PSM_RC_CLK_DIV_T psm_rc_clk_div;
    REG_RDA2720M_PSM_PSM_STATUS_T psm_status;
    REG_RDA2720M_PSM_PSM_32K_CAL_TH_T psm_32k_cal_th;
    REG_RDA2720M_PSM_RTC_PWR_OFF_TH1_T rtc_pwr_off_th1;
    REG_RDA2720M_PSM_RTC_PWR_OFF_TH2_T rtc_pwr_off_th2;
    REG_RDA2720M_PSM_RTC_PWR_OFF_TH3_T rtc_pwr_off_th3;
    REG_RDA2720M_PSM_RTC_PWR_ON_TH1_T rtc_pwr_on_th1;
    REG_RDA2720M_PSM_RTC_PWR_ON_TH2_T rtc_pwr_on_th2;
    REG_RDA2720M_PSM_RTC_PWR_ON_TH3_T rtc_pwr_on_th3;

    // psm pclk enable
    REG_ADI_CHANGE1(hwp_rda2720mGlobal->module_en0, module_en0, psm_en, 1);

    // psm apb soft_reset
    REG_ADI_CHANGE1(hwp_rda2720mGlobal->soft_rst0, soft_rst0, psm_soft_rst, 1);
    osiDelayUS(10);
    REG_ADI_CHANGE1(hwp_rda2720mGlobal->soft_rst0, soft_rst0, psm_soft_rst, 0);

    // open PSM protect register
    halAdiBusWrite(&hwp_rda2720mPsm->psm_reg_wr_protect, PSM_WR_PROT_MAGIC);

    // psm module soft_reset, auto clear
    REG_ADI_WRITE1(hwp_rda2720mPsm->psm_ctrl, psm_ctrl, psm_software_reset, 1);
    osiDelayUS(10);

    // open PSM protect register
    halAdiBusWrite(&hwp_rda2720mPsm->psm_reg_wr_protect, PSM_WR_PROT_MAGIC);

    // clear psm status
    REG_ADI_WRITE1(hwp_rda2720mPsm->psm_ctrl, psm_ctrl, psm_status_clr, 1);

    // configure psm clk divider
    REG_ADI_WRITE3(hwp_rda2720mPsm->psm_rc_clk_div, psm_rc_clk_div,
                   rc_32k_cal_cnt_p, 4,
                   clk_cal_64k_div_th, 7,
                   wdg_rst_clk_sel_en, 0);

    // enable psm cnt
    REG_ADI_CHANGE1(hwp_rda2720mPsm->psm_ctrl, psm_ctrl, psm_cnt_en, 1);
    osiDelayUS(10);

    // update cnt value enable
    REG_ADI_CHANGE1(hwp_rda2720mPsm->psm_ctrl, psm_ctrl, psm_cnt_update, 1);

    // check psm update is ok
    REG_ADI_WAIT_FIELD_NEZ(psm_status, hwp_rda2720mPsm->psm_status, psm_cnt_update_vld);

    // update cnt value disable
    REG_ADI_CHANGE1(hwp_rda2720mPsm->psm_ctrl, psm_ctrl, psm_cnt_update, 0);

    // configure psm cal N and pre_cal time
    REG_ADI_WRITE2(hwp_rda2720mPsm->psm_32k_cal_th, psm_32k_cal_th,
                   rc_32k_cal_cnt_n, 7,
                   rc_32k_cal_pre_th, 2);

    // configure psm cal dn filter
    halAdiBusWrite(&hwp_rda2720mPsm->psm_26m_cal_dn_th, 0);

    // configure psm cal up filter
    halAdiBusWrite(&hwp_rda2720mPsm->psm_26m_cal_up_th, 0xffff);

    // configure rtc power off clk_en ,hold,
    REG_ADI_WRITE2(hwp_rda2720mPsm->rtc_pwr_off_th1, rtc_pwr_off_th1,
                   rtc_pwr_off_clk_en_th, 0x10,
                   rtc_pwr_off_hold_th, 0x18);

    // configure rtc power off reset, pd
    REG_ADI_WRITE2(hwp_rda2720mPsm->rtc_pwr_off_th2, rtc_pwr_off_th2,
                   rtc_pwr_off_rstn_th, 0x20,
                   rtc_pwr_off_pd_th, 0x28);

    // configure rtc power off pd done,
    REG_ADI_WRITE1(hwp_rda2720mPsm->rtc_pwr_off_th3, rtc_pwr_off_th3,
                   rtc_pwr_off_done_th, 0x40);

    // configure rtc power on pd , reset,
    REG_ADI_WRITE2(hwp_rda2720mPsm->rtc_pwr_on_th1, rtc_pwr_on_th1,
                   rtc_pwr_on_pd_th, 0x04,
                   rtc_pwr_on_rstn_th, 0x08);

    // configure rtc power on clk_en, ,hold,
    REG_ADI_WRITE2(hwp_rda2720mPsm->rtc_pwr_on_th2, rtc_pwr_on_th2,
                   rtc_pwr_on_hold_th, 0x10,
                   rtc_pwr_on_clk_en_th, 0x06);

    // configure rtc power on pd done,timeout
    REG_ADI_WRITE2(hwp_rda2720mPsm->rtc_pwr_on_th3, rtc_pwr_on_th3,
                   rtc_pwr_on_done_th, 0x40,
                   rtc_pwr_on_timeout_th, 0x48);

    // configure psm cal interval
    halAdiBusWrite(&hwp_rda2720mPsm->psm_cnt_interval_th, 6);

    // configure psm cal phase, <=psm_cnt_interval_th
    halAdiBusWrite(&hwp_rda2720mPsm->psm_cnt_interval_phase, 6);
}

void bootPowerOff(void)
{
    bool pwrkey_en = false;
    bool wakeup_en = false;

#ifdef CONFIG_PWRKEY_POWERUP
    pwrkey_en = true;
#endif

#ifdef CONFIG_WAKEUP_PIN_POWERUP
    wakeup_en = true;
#endif

    REG_RDA2720M_PSM_PSM_CTRL_T psm_ctrl = {
        .b.psm_en = 1,
        .b.rtc_pwr_on_timeout_en = 0,
        .b.ext_int_pwr_en = wakeup_en ? 1 : 0,
        .b.pbint1_pwr_en = pwrkey_en ? 1 : 0,
        .b.pbint2_pwr_en = 0,
#ifdef CONFIG_CHARGER_POWERUP
        .b.charger_pwr_en = 1,
#else
        .b.charger_pwr_en = 0,
#endif
        .b.psm_cnt_alarm_en = 0,
        .b.psm_cnt_alm_en = 0,
        .b.psm_software_reset = 0,
        .b.psm_cnt_update = 1,
        .b.psm_cnt_en = 1,
        .b.psm_status_clr = 0,
        .b.psm_cal_en = 1,
        .b.rtc_32k_clk_sel = 0, // 32k less
    };

    _psmPrepare();

    halAdiBusWrite(&hwp_rda2720mPsm->psm_ctrl, psm_ctrl.v);

    REG_RDA2720M_PSM_PSM_STATUS_T psm_status;
    REG_ADI_WAIT_FIELD_NEZ(psm_status, hwp_rda2720mPsm->psm_status, psm_cnt_update_vld);

    // power off
    halAdiBusWrite(&hwp_rda2720mGlobal->power_pd_hw, 1);
}

bool bootIsFromPsmSleep(void)
{
    REG_RDA2720M_GLOBAL_MODULE_EN0_T module_en0;

    // enable PSM module
    module_en0.v = halAdiBusRead(&hwp_rda2720mGlobal->module_en0);
    module_en0.b.psm_en = 1;
    halAdiBusWrite(&hwp_rda2720mGlobal->module_en0, module_en0.v);

    uint32_t psm_magic = halAdiBusRead(&PSM_MODE_REG);

    // disable PSM module
    module_en0.b.psm_en = 0;
    halAdiBusWrite(&hwp_rda2720mGlobal->module_en0, module_en0.v);

    return (psm_magic == OSI_SHUTDOWN_PSM_SLEEP);
}

uint32_t bootSecondOffsetBytes(void)
{
    return 0;
}

int bootPowerOnCause()
{
    REG_RDA2720M_GLOBAL_SWRST_CTRL0_T swrst_ctrl0;
    swrst_ctrl0.v = halAdiBusRead(&hwp_rda2720mGlobal->swrst_ctrl0);
    if (swrst_ctrl0.b.reg_rst_en == 0)
    {
        REG_RDA2720M_GLOBAL_POR_SRC_FLAG_T por_src_flag;
        por_src_flag.v = halAdiBusRead(&hwp_rda2720mGlobal->por_src_flag);
        if (por_src_flag.v & (1 << 11))
            return OSI_BOOTCAUSE_PIN_RESET;

        if (por_src_flag.v & ((1 << 12) | (1 << 9) | (1 << 8)))
            return OSI_BOOTCAUSE_PWRKEY;

        if (por_src_flag.v & (1 << 6))
            return OSI_BOOTCAUSE_ALARM;

        if (por_src_flag.v & ((1 << 4) | (1 << 5) | (1 << 10)))
            return OSI_BOOTCAUSE_CHARGE;
    }

    return OSI_BOOTCAUSE_UNKNOWN;
}

void bootResetPinEnable(void)
{
    REG_RDA2720M_GLOBAL_POR_7S_CTRL_T por_7s_ctrl;
    REG_ADI_CHANGE2(hwp_rda2720mGlobal->por_7s_ctrl, por_7s_ctrl,
                    ext_rstn_mode, 1, key2_7s_rst_en, 1);

#ifdef CONFIG_BOOT_PB_7S_RESET_ENABLE
    REG_ADI_CHANGE1(hwp_rda2720mGlobal->por_7s_ctrl, por_7s_ctrl,
                    pbint_7s_rst_disable, 0);
#else
    REG_ADI_CHANGE1(hwp_rda2720mGlobal->por_7s_ctrl, por_7s_ctrl,
                    pbint_7s_rst_disable, 1);
#endif

    REG_RDA2720M_GLOBAL_SWRST_CTRL0_T swrst_ctrl0 = {halAdiBusRead(&hwp_rda2720mGlobal->swrst_ctrl0)};
    swrst_ctrl0.b.ext_rstn_pd_en = 1;
    swrst_ctrl0.b.pb_7s_rst_pd_en = 1;
    swrst_ctrl0.b.reg_rst_pd_en = 1;
    swrst_ctrl0.b.wdg_rst_pd_en = 0;
    halAdiBusWrite(&hwp_rda2720mGlobal->swrst_ctrl0, swrst_ctrl0.v);

    REG_RDA2720M_GLOBAL_SWRST_CTRL1_T swrst_ctrl1 = {};
    swrst_ctrl1.b.sw_rst_vio18_pd_en = 1;
    swrst_ctrl1.b.sw_rst_ddr12_pd_en = 1;
    swrst_ctrl1.b.sw_rst_mmc_pd_en = 1;
    swrst_ctrl1.b.sw_rst_usb33_pd_en = 1;
    swrst_ctrl1.b.sw_rst_rf15_pd_en = 1;
    swrst_ctrl1.b.sw_rst_ana_pd_en = 1;
    swrst_ctrl1.b.sw_rst_vdd28_pd_en = 1;
    swrst_ctrl1.b.sw_rst_dcxo_pd_en = 0;
    swrst_ctrl1.b.sw_rst_mem_pd_en = 1;
    swrst_ctrl1.b.sw_rst_dcdccore_pd_en = 1;
    swrst_ctrl1.b.sw_rst_dcdcgen_pd_en = 1;
    halAdiBusWrite(&hwp_rda2720mGlobal->swrst_ctrl1, swrst_ctrl1.v);
}

OSI_NO_RETURN OSI_WEAK void bootBlueScreen(void *ctx)
{
    halBlueScreenSaveContext(ctx);
    osiDCacheCleanInvalidateAll();
    OSI_DEAD_LOOP;
}
