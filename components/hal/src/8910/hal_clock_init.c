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
#include "hal_chip.h"
#include "hal_ram_cfg.h"
#include "hal_hwspinlock.h"
#include "drv_config.h"
#include "osi_api.h"
#include "connectivity_config.h"
#include "osi_tick_unit.h"

#define MR0 (0)
#define MR1 (1)
#define MR2 (2)
#define MR3 (3)
#define MR4 (4)
#define MR5 (5)
#define MR6 (6)
#define MR7 (7)
#define MR8 (8)
#define DMC_MR_WRITE_CMD(mr, val) (DMC400_DIRECT_CMD_MRS | ((val) << 8) | (mr))
#define DMC_MR_READ_CMD(mr, val) (DMC400_DIRECT_CMD_MRR | (mr))

#define QOS_CPA5 10
#define QOS_F8 9
#define QOS_LTE 8
#define QOS_ZSP_IBUS 7
#define QOS_ZSP_DBUS 7
#define QOS_ZSP_DMA 7
#define QOS_GGE 8
#define QOS_WCN 4
#define QOS_APA5 2
#define QOS_AXIDMA 1
#define QOS_LCDC 5
#define QOS_LZMA 1
#define QOS_GOUDA 5
#define QOS_USB 3
#define QOS_MERGEMEM 5
#define WRITE_PRIO 6
#define READ_FILL_PRIO 6

typedef struct
{
    uint32_t dll_cnt_ac;
    uint32_t dll_cnt_ds0;
    uint32_t dll_cnt_ds1;
} dllTrain_t;

static void prvDelayUS(uint32_t us)
{
    uint32_t start = HAL_TIMER_CURVAL_LO;
    uint32_t ticks = OSI_US_TO_HWTICK_U32(us);
    OSI_LOOP_WAIT(((unsigned)(HAL_TIMER_CURVAL_LO - start)) > ticks);
}

static inline uint32_t _pllDividerByFreq(uint32_t freq)
{
    return freq * (1ULL << 25) / 52000000ULL;
}

static inline uint8_t _sysDividerRegByFreq(uint32_t pll_freq, uint32_t sub_freq)
{
#define DIVIDER_CHECK(divx2, n, v) \
    if ((divx2) <= (n))            \
        return (v);

    unsigned divx2 = (pll_freq * 2 + sub_freq / 2) / sub_freq;
    DIVIDER_CHECK(divx2, HAL_CLK_DIV1P0, 15);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV1P5, 14);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV2P0, 13);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV2P5, 12);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV3P0, 11);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV3P5, 10);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV4P0, 9);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV4P5, 8);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV5P0, 7);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV5P5, 6);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV6P0, 5);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV7P0, 4);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV8P0, 3);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV12P0, 2);
    DIVIDER_CHECK(divx2, HAL_CLK_DIV16P0, 1);
    return 0; // HAL_CLK_DIV24P0
}

static void _dmc400QosConfig(void)
{
    REG_SYS_CTRL_CLK_AON_AHB_ENABLE_T clk_aon_ahb_enable = {.b.enable_aon_ahb_clk_id_bb_sysctrl = 1};
    hwp_sysCtrl->clk_aon_ahb_enable = clk_aon_ahb_enable.v;

    REG_CP_CLKRST_CLKEN_BB_SYSCTRL_SET_T clken_bb_sysctrl_set = {.b.clken_bb_sysctrlset = 1};
    hwp_clkrst->clken_bb_sysctrl_set = clken_bb_sysctrl_set.v;

    REG_SYS_CTRL_CFG_QOS_WCN_A5_GGE_T cfg_qos_wcn_a5_gge = {
        .b.wcn_mem_arqos = QOS_WCN,
        .b.wcn_mem_awqos = QOS_WCN,
        .b.gge_arqos = QOS_GGE,
        .b.gge_awqos = QOS_GGE,
        .b.a5_arqos = QOS_APA5,
        .b.a5_awqos = QOS_APA5,
    };
    hwp_sysCtrl->cfg_qos_wcn_a5_gge = cfg_qos_wcn_a5_gge.v;

    REG_SYS_CTRL_CFG_QOS_AXIDMA_CPA5_F8_T cfg_qos_axidma_cpa5_f8 = {
        .b.axidma_arqos = QOS_AXIDMA,
        .b.axidma_awqos = QOS_AXIDMA,
        .b.cp_a5_arqos = QOS_CPA5,
        .b.cp_a5_awqos = QOS_CPA5,
        .b.f8_arqos = QOS_F8,
        .b.f8_awqos = QOS_F8,
    };
    hwp_sysCtrl->cfg_qos_axidma_cpa5_f8 = cfg_qos_axidma_cpa5_f8.v;

    REG_SYS_CTRL_CFG_QOS_LCDC_LZMA_GOUDA_T cfg_qos_lcdc_lzma_gouda = {
        .b.lcdc_arqos = QOS_LCDC,
        .b.lcdc_awqos = QOS_LCDC,
        .b.lzma_arqos = QOS_LZMA,
        .b.lzma_awqos = QOS_LZMA,
        .b.gouda_arqos = QOS_GOUDA,
        .b.gouda_awqos = QOS_GOUDA,
    };
    hwp_sysCtrl->cfg_qos_lcdc_lzma_gouda = cfg_qos_lcdc_lzma_gouda.v;

    REG_SYS_CTRL_CFG_QOS_LTE_USB_T cfg_qos_lte_usb = {
        .b.lte_arqos = QOS_LTE,
        .b.lte_awqos = QOS_LTE,
        .b.usb_arqos = QOS_USB,
        .b.usb_awqos = QOS_USB,
    };
    hwp_sysCtrl->cfg_qos_lte_usb = cfg_qos_lte_usb.v;

    REG_SYS_CTRL_CFG_QOS_MERGE_MEM_T cfg_qos_merge_mem = {
        .b.merge_mem_awqos = QOS_MERGEMEM,
        .b.merge_mem_arqos = QOS_MERGEMEM,
    };
    hwp_sysCtrl->cfg_qos_merge_mem = cfg_qos_merge_mem.v;

    REG_CP_SYSREG_ZSP_QOS_T zsp_qos = {
        .b.arqos_zsp_dbus = QOS_ZSP_DBUS,
        .b.awqos_zsp_dbus = QOS_ZSP_DBUS,
        .b.arqos_zsp_ibus = QOS_ZSP_IBUS,
        .b.awqos_zsp_ibus = QOS_ZSP_IBUS,
        .b.arqos_zsp_axidma = QOS_ZSP_DMA,
        .b.awqos_zsp_axidma = QOS_ZSP_DMA,
    };
    hwp_sysreg->zsp_qos = zsp_qos.v;

    uint8_t qos_timeout = 15;

    REG_DMC400_TIMEOUT_CONTROL_T timeout_control = {.b.timeout_prescalar = 2};
    hwp_dmcCtrl->timeout_control = timeout_control.v;

    REG_DMC400_QOS0_CONTROL_T qos0_control = {
        .b.qos0_priority = 0,
        .b.qos0_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos0_control = qos0_control.v;

    REG_DMC400_QOS1_CONTROL_T qos1_control = {
        .b.qos1_priority = 1,
        .b.qos1_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos1_control = qos1_control.v;

    REG_DMC400_QOS2_CONTROL_T qos2_control = {
        .b.qos2_priority = 2,
        .b.qos2_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos2_control = qos2_control.v;

    REG_DMC400_QOS3_CONTROL_T qos3_control = {
        .b.qos3_priority = 3,
        .b.qos3_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos3_control = qos3_control.v;

    REG_DMC400_QOS4_CONTROL_T qos4_control = {
        .b.qos4_priority = 4,
        .b.qos4_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos4_control = qos4_control.v;

    REG_DMC400_QOS5_CONTROL_T qos5_control = {
        .b.qos5_priority = 5,
        .b.qos5_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos5_control = qos5_control.v;

    REG_DMC400_QOS6_CONTROL_T qos6_control = {
        .b.qos6_priority = 6,
        .b.qos6_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos6_control = qos6_control.v;

    REG_DMC400_QOS7_CONTROL_T qos7_control = {
        .b.qos7_priority = 7,
        .b.qos7_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos7_control = qos7_control.v;

    REG_DMC400_QOS8_CONTROL_T qos8_control = {
        .b.qos8_priority = 8,
        .b.qos8_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos8_control = qos8_control.v;

    REG_DMC400_QOS9_CONTROL_T qos9_control = {
        .b.qos9_priority = 9,
        .b.qos9_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos9_control = qos9_control.v;

    REG_DMC400_QOS10_CONTROL_T qos10_control = {
        .b.qos10_priority = 10,
        .b.qos10_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos10_control = qos10_control.v;

    REG_DMC400_QOS11_CONTROL_T qos11_control = {
        .b.qos11_priority = 11,
        .b.qos11_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos11_control = qos11_control.v;

    REG_DMC400_QOS12_CONTROL_T qos12_control = {
        .b.qos12_priority = 12,
        .b.qos12_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos12_control = qos12_control.v;

    REG_DMC400_QOS13_CONTROL_T qos13_control = {
        .b.qos13_priority = 13,
        .b.qos13_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos13_control = qos13_control.v;

    REG_DMC400_QOS14_CONTROL_T qos14_control = {
        .b.qos14_priority = 14,
        .b.qos14_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos14_control = qos14_control.v;

    REG_DMC400_QOS15_CONTROL_T qos15_control = {
        .b.qos15_priority = 15,
        .b.qos15_timeout = qos_timeout,
    };
    hwp_dmcCtrl->qos15_control = qos15_control.v;

    REG_DMC400_WRITE_PRIORITY_CONTROL_T write_priority_control = {
        .b.write_threshold_en = 1,
        .b.write_fill_priority_1_16ths = 0,
        .b.write_fill_priority_2_16ths = 0,
        .b.write_fill_priority_3_16ths = 0,
        .b.write_fill_priority_4_16ths = 0,
        .b.write_fill_priority_5_16ths = 0,
        .b.write_fill_priority_6_16ths = 0,
        .b.write_fill_priority_7_16ths = 0,
    };
    hwp_dmcCtrl->write_priority_control = write_priority_control.v;

    REG_DMC400_WRITE_PRIORITY_CONTROL2_T write_priority_control2 = {
        .b.write_fill_priority_8_16ths = WRITE_PRIO,
        .b.write_fill_priority_9_16ths = WRITE_PRIO,
        .b.write_fill_priority_10_16ths = WRITE_PRIO,
        .b.write_fill_priority_11_16ths = WRITE_PRIO,
        .b.write_fill_priority_12_16ths = WRITE_PRIO,
        .b.write_fill_priority_13_16ths = WRITE_PRIO,
        .b.write_fill_priority_14_16ths = WRITE_PRIO,
        .b.write_fill_priority_15_16ths = WRITE_PRIO,
    };
    hwp_dmcCtrl->write_priority_control2 = write_priority_control2.v;

    REG_DMC400_READ_PRIORITY_CONTROL_T read_priority_control = {
        .b.read_escalation = 0,
        .b.read_in_burst_prioritisation = 1,
        .b.read_fill_priority_1_16ths = 0,
        .b.read_fill_priority_2_16ths = 0,
        .b.read_fill_priority_3_16ths = 0,
        .b.read_fill_priority_4_16ths = 0,
        .b.read_fill_priority_5_16ths = 0,
        .b.read_fill_priority_6_16ths = 0,
        .b.read_fill_priority_7_16ths = 0,
    };
    hwp_dmcCtrl->read_priority_control = read_priority_control.v;

    REG_DMC400_READ_PRIORITY_CONTROL2_T read_priority_control2 = {
        .b.read_fill_priority_8_16ths = READ_FILL_PRIO,
        .b.read_fill_priority_9_16ths = READ_FILL_PRIO,
        .b.read_fill_priority_10_16ths = READ_FILL_PRIO,
        .b.read_fill_priority_11_16ths = READ_FILL_PRIO,
        .b.read_fill_priority_12_16ths = READ_FILL_PRIO,
        .b.read_fill_priority_13_16ths = READ_FILL_PRIO,
        .b.read_fill_priority_14_16ths = READ_FILL_PRIO,
        .b.read_fill_priority_15_16ths = READ_FILL_PRIO,
    };
    hwp_dmcCtrl->read_priority_control2 = read_priority_control2.v;

    REG_DMC400_HIT_PRIORITY_T hit_priority = {
        .b.hit_priority = 0,
        .b.hit_limit = 1,
    };
    hwp_dmcCtrl->hit_priority = hit_priority.v;

    REG_DMC400_TURNAROUND_PRIORITY_T turnaround_priority = {
        .b.turnaround_priority = 0,
        .b.turnaround_limit = 1,
    };
    hwp_dmcCtrl->turnaround_priority = turnaround_priority.v;
}

#ifdef CONFIG_USE_PSRAM

static uint32_t halPsramPllTrainAds0(void)
{
    uint32_t dll_lock, dll_cnt;

    //disable dll to clear status
    REG_PSRAM_PHY_PSRAM_RF_CFG_DLL_ADS0_T rf_cfg_dll_ads0 = {hwp_psramPhy->psram_rf_cfg_dll_ads0};
    rf_cfg_dll_ads0.b.rf_dll_en_ads0 = 0x0;
    hwp_psramPhy->psram_rf_cfg_dll_ads0 = rf_cfg_dll_ads0.v;

    // drf_dll_clk_mode_f0 : 0 - clk_dmc_2x_fg; 1 - clk_dmc_fg
    // drf_dll_half_mode_f0 : 1 - dll lock cnt = 1/2 T of dl_in
    hwp_psramPhy->psram_drf_cfg_dll_mode_f0 = 0x02;
    rf_cfg_dll_ads0.b.rf_dll_lock_wait_ads0 = 0x06;
    rf_cfg_dll_ads0.b.rf_dll_pd_cnt_ads0 = 0x06;
    // enable dll
    rf_cfg_dll_ads0.b.rf_dll_en_ads0 = 0x1;
    hwp_psramPhy->psram_rf_cfg_dll_ads0 = rf_cfg_dll_ads0.v;

    // wait dll lock
    REG_PSRAM_PHY_PSRAM_RFDLL_STATUS_DLL_ADS0_T rfdll_status_ads0 = {};
    do
    {
        rfdll_status_ads0.v = hwp_psramPhy->psram_rfdll_status_dll_ads0;
        dll_lock = rfdll_status_ads0.b.rfdll_locked_ads0;
        dll_cnt = rfdll_status_ads0.b.rfdll_cnt_ads0;
    } while ((dll_lock == 0) && (dll_cnt != 255));

    return dll_cnt;
}

static uint32_t halPsramPllTrainAds1()
{
    uint32_t dll_lock, dll_cnt;

    // disable dll to clear status
    REG_PSRAM_PHY_PSRAM_RF_CFG_DLL_ADS1_T rf_cfg_dll_ads1 = {hwp_psramPhy->psram_rf_cfg_dll_ads1};
    rf_cfg_dll_ads1.b.rf_dll_en_ads1 = 0x0;
    hwp_psramPhy->psram_rf_cfg_dll_ads1 = rf_cfg_dll_ads1.v;

    // drf_dll_clk_mode_f0 : 0 - clk_dmc_2x_fg; 1 - clk_dmc_fg
    // drf_dll_half_mode_f0 : 1 - dll lock cnt = 1/2 T of dl_in
    hwp_psramPhy->psram_drf_cfg_dll_mode_f0 = 0x02;
    rf_cfg_dll_ads1.b.rf_dll_lock_wait_ads1 = 0x06;
    rf_cfg_dll_ads1.b.rf_dll_pd_cnt_ads1 = 0x06;

    // enable dll
    rf_cfg_dll_ads1.b.rf_dll_en_ads1 = 0x1;
    hwp_psramPhy->psram_rf_cfg_dll_ads1 = rf_cfg_dll_ads1.v;

    //wait dll lock
    REG_PSRAM_PHY_PSRAM_RFDLL_STATUS_DLL_ADS1_T rfdll_status_ads1 = {};
    do
    {
        rfdll_status_ads1.v = hwp_psramPhy->psram_rfdll_status_dll_ads1;
        dll_lock = rfdll_status_ads1.b.rfdll_locked_ads1;
        dll_cnt = rfdll_status_ads1.b.rfdll_cnt_ads1;
    } while ((dll_lock == 0) && (dll_cnt != 255));

    return dll_cnt;
}

#else

static uint32_t halDdrPllTrainAc()
{
    uint32_t dll_lock, dll_cnt;
    REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_AC_T lpddr_rf_cfg_dll_ac =
        {hwp_lpddrPhy->lpddr_rf_cfg_dll_ac};
    //disable dll to clear status
    lpddr_rf_cfg_dll_ac.b.rf_dll_en_ac = 0x0;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_ac = lpddr_rf_cfg_dll_ac.v;
    //drf_dll_clk_mode_f0 : 0 - clk_dmc_2x_fg; 1 - clk_dmc_fg
    //drf_dll_half_mode_f0 : 1 - dll lock cnt = 1/2 T of dl_in
    hwp_lpddrPhy->lpddr_drf_cfg_dll_mode_f0 = 0x02;

    lpddr_rf_cfg_dll_ac.b.rf_dll_lock_wait_ac = 0x06;
    lpddr_rf_cfg_dll_ac.b.rf_dll_pd_cnt_ac = 0x06;
    //enable dll
    lpddr_rf_cfg_dll_ac.b.rf_dll_en_ac = 0x1;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_ac = lpddr_rf_cfg_dll_ac.v;
    //wait dll lock
    REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_DLL_AC_T rfdll_status_dll_ac = {};
    do
    {
        rfdll_status_dll_ac.v = hwp_lpddrPhy->lpddr_rfdll_status_dll_ac;
        dll_lock = rfdll_status_dll_ac.b.rfdll_locked_ac;
        dll_cnt = rfdll_status_dll_ac.b.rfdll_cnt_ac;
    } while ((dll_lock == 0) && (dll_cnt != 127));

    return dll_cnt;
}

static uint32_t halDdrPllTrainDs0()
{
    uint32_t dll_lock, dll_cnt;
    // disable dll to clear status
    REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DS0_T lpddr_rf_cfg_dll_ds0 = {hwp_lpddrPhy->lpddr_rf_cfg_dll_ds0};
    lpddr_rf_cfg_dll_ds0.b.rf_dll_en_ds0 = 0x0;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_ds0 = lpddr_rf_cfg_dll_ds0.v;

    // drf_dll_clk_mode_f0 : 0 - clk_dmc_2x_fg; 1 - clk_dmc_fg
    // drf_dll_half_mode_f0 : 1 - dll lock cnt = 1/2 T of dl_in
    hwp_lpddrPhy->lpddr_drf_cfg_dll_mode_f0 = 0x02;

    lpddr_rf_cfg_dll_ds0.b.rf_dll_lock_wait_ds0 = 0x06;
    lpddr_rf_cfg_dll_ds0.b.rf_dll_pd_cnt_ds0 = 0x06;
    // enable dll
    lpddr_rf_cfg_dll_ds0.b.rf_dll_en_ds0 = 0x1;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_ds0 = lpddr_rf_cfg_dll_ds0.v;
    // wait dll lock
    REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_DLL_DS0_T rfdll_status_dll_ds0 = {};
    do
    {
        rfdll_status_dll_ds0.v = hwp_lpddrPhy->lpddr_rfdll_status_dll_ds0;
        dll_lock = rfdll_status_dll_ds0.b.rfdll_locked_ds0;
        dll_cnt = rfdll_status_dll_ds0.b.rfdll_cnt_ds0;
    } while ((dll_lock == 0) && (dll_cnt != 127));

    return dll_cnt;
}

static uint32_t halDdrPllTrainDs1()
{
    uint32_t dll_lock, dll_cnt;
    // disable dll to clear status
    REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DS1_T lpddr_rf_cfg_dll_ds1 = {hwp_lpddrPhy->lpddr_rf_cfg_dll_ds1};
    lpddr_rf_cfg_dll_ds1.b.rf_dll_en_ds1 = 0x0;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_ds1 = lpddr_rf_cfg_dll_ds1.v;

    // drf_dll_clk_mode_f0 : 0 - clk_dmc_2x_fg; 1 - clk_dmc_fg
    // drf_dll_half_mode_f0 : 1 - dll lock cnt = 1/2 T of dl_in
    hwp_lpddrPhy->lpddr_drf_cfg_dll_mode_f0 = 0x02;

    lpddr_rf_cfg_dll_ds1.b.rf_dll_lock_wait_ds1 = 0x06;
    lpddr_rf_cfg_dll_ds1.b.rf_dll_pd_cnt_ds1 = 0x06;
    // enable dll
    lpddr_rf_cfg_dll_ds1.b.rf_dll_en_ds1 = 0x1;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_ds1 = lpddr_rf_cfg_dll_ds1.v;
    // wait dll lock
    REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_DLL_DS1_T rfdll_status_dll_ds1 = {};
    do
    {
        rfdll_status_dll_ds1.v = hwp_lpddrPhy->lpddr_rfdll_status_dll_ds1;
        dll_lock = rfdll_status_dll_ds1.b.rfdll_locked_ds1;
        dll_cnt = rfdll_status_dll_ds1.b.rfdll_cnt_ds1;
    } while ((dll_lock == 0) && (dll_cnt != 127));

    return dll_cnt;
}

#endif

static void halRamDllTrain(dllTrain_t *d)
{
#ifdef CONFIG_USE_PSRAM
    hwp_psramPhy->psram_rf_cfg_phy = 0x01;
    hwp_psramPhy->psram_rf_cfg_clock_gate = 0x03;

    d->dll_cnt_ds0 = halPsramPllTrainAds0();
    d->dll_cnt_ds1 = halPsramPllTrainAds1();
    d->dll_cnt_ac = 0;
#else
    hwp_lpddrPhy->lpddr_rf_cfg_phy = 0x01;
    hwp_lpddrPhy->lpddr_rf_cfg_clock_gate = 0x03;

    d->dll_cnt_ac = halDdrPllTrainAc();
    d->dll_cnt_ds0 = halDdrPllTrainDs0();
    d->dll_cnt_ds1 = halDdrPllTrainDs1();
#endif
}

static inline void halWriteDllTrain(dllTrain_t *d)
{
#ifdef CONFIG_USE_PSRAM
    hwp_psramPhy->psram_rf_cfg_dll_dl_0_wr_ads0 = d->dll_cnt_ds0;
    hwp_psramPhy->psram_rf_cfg_dll_dl_1_wr_ads0 = d->dll_cnt_ds0 - 10;
    hwp_psramPhy->psram_rf_cfg_dll_dl_2_wr_ads0 = d->dll_cnt_ds0 - 10;
    hwp_psramPhy->psram_rf_cfg_dll_dl_3_wr_ads0 = d->dll_cnt_ds0;
    hwp_psramPhy->psram_rf_cfg_dll_dl_0_wr_ads1 = d->dll_cnt_ds1;
    hwp_psramPhy->psram_rf_cfg_dll_dl_1_wr_ads1 = d->dll_cnt_ds1 - 10;
    hwp_psramPhy->psram_rf_cfg_dll_dl_2_wr_ads1 = d->dll_cnt_ds1 - 10;
    hwp_psramPhy->psram_rf_cfg_dll_dl_3_wr_ads1 = d->dll_cnt_ds1;
#else
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_0_wr_ac = d->dll_cnt_ac;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_0_wr_ds0 = d->dll_cnt_ds0;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_1_wr_ds0 = d->dll_cnt_ds0 - 10;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_2_wr_ds0 = d->dll_cnt_ds0 - 10;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_3_wr_ds0 = d->dll_cnt_ds0;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_0_wr_ds1 = d->dll_cnt_ds1;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_1_wr_ds1 = d->dll_cnt_ds1 - 10;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_2_wr_ds1 = d->dll_cnt_ds1 - 10;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_3_wr_ds1 = d->dll_cnt_ds1;
#endif
}

void halRamInit(void)
{
    hwp_sysreg->ddr_slp_ctrl_enable = 0;

    dllTrain_t dll_train_value;
    halRamDllTrain(&dll_train_value);

    prvRamPhyPadCfg();
    halWriteDllTrain(&dll_train_value);

    REG_DMC400_MEMC_STATUS_T memc_status = {hwp_dmcCtrl->memc_status};
    REG_DMC400_MEMC_CMD_T memc_cmd = {};
    if (memc_status.b.memc_status != 0)
    {
        memc_cmd.b.memc_cmd = 0; // CONFIG
        prvDelayUS(10);
        hwp_dmcCtrl->memc_cmd = memc_cmd.v;
    }

    _dmc400QosConfig();
    prvRamDmcCfg();
    prvRamBootCfg();

    hwp_sysreg->ddr_slp_wait_number = 200;
    hwp_sysreg->ddr_slp_ctrl_enable = 1;
}

/**
 * PSRAM exit half sleep, a low pulse of CE with width tXHPS, and wait tXHS.
 */
static inline void prvPsramExitHalfSleep(void)
{
    // select iomux for software
    hwp_psramPhy->psram_rf_cfg_phy_iomux_oe_wr_ads0 = 0x00110000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_oe_wr_ads1 = 0x00110000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_out_wr_ads0 = 0x00100000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_out_wr_ads1 = 0x00100000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_sel_wr_ads0 = 0x00110000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_sel_wr_ads1 = 0x00110000;

    // exit half sleep
    hwp_psramPhy->psram_rf_cfg_phy_iomux_out_wr_ads0 = 0x00000000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_out_wr_ads1 = 0x00000000;

    prvDelayUS(4); // tXPHS

    // release clock
    hwp_psramPhy->psram_rf_cfg_phy_iomux_out_wr_ads0 = 0x00100000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_out_wr_ads1 = 0x00100000;

    prvDelayUS(150); // tXHS

    // select iomux for hardware
    hwp_psramPhy->psram_rf_cfg_phy_iomux_sel_wr_ads0 = 0x00000000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_sel_wr_ads1 = 0x00000000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_oe_wr_ads0 = 0x00000000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_oe_wr_ads1 = 0x00000000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_out_wr_ads0 = 0x00000000;
    hwp_psramPhy->psram_rf_cfg_phy_iomux_out_wr_ads1 = 0x00000000;

    prvDelayUS(2);
}

void halRamWakeInit(void)
{
    hwp_sysreg->ddr_slp_ctrl_enable = 0;

#if defined(CONFIG_USE_PSRAM) && defined(CONFIG_PSRAM_LP_HALF_SLEEP)
    hwp_pwrctrl->ddr_hold_ctrl = 0;
    hwp_idle->ddr_latch = 0;
    hwp_psramPhy->psram_drf_t_phywrlat = 0;
#endif

    dllTrain_t dll_train_value;
    halRamDllTrain(&dll_train_value);

    prvRamPhyPadCfg();
    halWriteDllTrain(&dll_train_value);

    REG_DMC400_MEMC_STATUS_T memc_status = {hwp_dmcCtrl->memc_status};
    REG_DMC400_MEMC_CMD_T memc_cmd = {};
    if (memc_status.b.memc_status != 0)
    {
        memc_cmd.b.memc_cmd = 0; // CONFIG
        prvDelayUS(10);
        hwp_dmcCtrl->memc_cmd = memc_cmd.v;
    }

    _dmc400QosConfig();
    prvRamDmcCfg();

#if defined(CONFIG_USE_PSRAM) && defined(CONFIG_PSRAM_LP_HALF_SLEEP)
    prvPsramExitHalfSleep();

    hwp_dmcCtrl->direct_cmd = DMC_MR_WRITE_CMD(MR4, 0x80);
    prvDelayUS(2);

    hwp_dmcCtrl->memc_cmd = DMC400_MEMC_CMD_GO;
    prvDelayUS(2);

    hwp_pwrctrl->ddr_slp_req_hwen = 1;
#else
    prvRamWakeCfg();
#endif

    hwp_sysreg->ddr_slp_wait_number = 200;
    hwp_sysreg->ddr_slp_ctrl_enable = 1;
}

void halRamSuspend(void)
{
#if defined(CONFIG_USE_PSRAM) && defined(CONFIG_PSRAM_LP_HALF_SLEEP)
    // ddr sleep control may close dmc clock, and dmc register access won't
    // enable dmc clock automatically.
    hwp_sysreg->ddr_slp_ctrl_enable = 0;

    hwp_dmcCtrl->memc_cmd = DMC400_MEMC_CMD_CONFIG;
    OSI_LOOP_WAIT(hwp_dmcCtrl->memc_status == DMC400_MEMC_STATUS_CONFIG);

    hwp_psramPhy->psram_drf_t_phywrlat = 2;
    hwp_psramPhy->psram_drf_t_cph_wr = 1;
    hwp_psramPhy->psram_drf_t_cph_rd = 1;

    hwp_dmcCtrl->direct_cmd = DMC_MR_WRITE_CMD(MR4, 0x88);
    prvDelayUS(2);
    hwp_dmcCtrl->direct_cmd = DMC_MR_WRITE_CMD(MR6, 0xf0);
    prvDelayUS(2);

    hwp_psramPhy->psram_rf_cfg_clock_gate = 0;
    hwp_idle->ddr_latch = 3;

    // DDR lower power bypass. Without this, system can't sleep immediately.
    // It will try to get ACK from DMC, and DMC can't (maybe) return ACK
    // in config mode. Only after the timeout, system can sleep.
    //
    // Also, it should be set to 1 before normal work mode. It will try to
    // gate DDR clock when there are no transactions.
    hwp_pwrctrl->ddr_slp_req_hwen = 0;
#endif
}

void halRamSuspendAbort(void)
{
#if defined(CONFIG_USE_PSRAM) && defined(CONFIG_PSRAM_LP_HALF_SLEEP)
    hwp_dmcCtrl->memc_cmd = DMC400_MEMC_CMD_CONFIG;
    OSI_LOOP_WAIT(hwp_dmcCtrl->memc_status == DMC400_MEMC_STATUS_CONFIG);

    hwp_idle->ddr_latch = 0;
    hwp_psramPhy->psram_rf_cfg_clock_gate = 0x1f;

    prvPsramExitHalfSleep();

    hwp_dmcCtrl->direct_cmd = DMC_MR_WRITE_CMD(MR4, 0x80);
    prvDelayUS(2);

    hwp_psramPhy->psram_drf_t_cph_wr = 6;
    hwp_psramPhy->psram_drf_t_cph_rd = 0x12;
    hwp_psramPhy->psram_drf_t_phywrlat = 0;

    hwp_dmcCtrl->memc_cmd = DMC400_MEMC_CMD_GO;
    prvDelayUS(2);

    hwp_pwrctrl->ddr_slp_req_hwen = 1;
    hwp_sysreg->ddr_slp_ctrl_enable = 1;
#endif
}

void halClockInit(void)
{
    hwp_sysCtrl->reg_dbg = SYS_CTRL_PROTECT_UNLOCK;

    REG_SYS_CTRL_RESET_CAUSE_T reset_cause = {hwp_sysCtrl->reset_cause};
    reset_cause.b.boot_mode &= ~3;
    reset_cause.b.sw_boot_mode = 0;
    hwp_sysCtrl->reset_cause = reset_cause.v;

    REG_SYS_CTRL_SEL_CLOCK_T sel_clock = {hwp_sysCtrl->sel_clock};
    sel_clock.b.soft_sel_spiflash = 1;   // slow
    sel_clock.b.soft_sel_spiflash1 = 1;  // slow
    sel_clock.b.sys_sel_fast = 1;        // slow
    sel_clock.b.soft_sel_mem_bridge = 1; // slow

    REG_SPI_FLASH_SPI_CONFIG_T spi_config = {};
    spi_config.b.quad_mode = 1;
    spi_config.b.spi_wprotect_pin = 0;
    spi_config.b.spi_hold_pin = 0;
    spi_config.b.sample_delay = 0;
    spi_config.b.clk_divider = 2;
    spi_config.b.cmd_quad = 0;
    spi_config.b.tx_rx_size = 0;

    // change to slow clock, and change flash config for slow clock
    halApplyRegisters((uint32_t)&hwp_sysCtrl->sel_clock, sel_clock.v,
                      (uint32_t)&hwp_spiFlash->spi_config, spi_config.v,
                      REG_APPLY_END);

    // NOTE: it is assumed that PLL is enabled beforehand
    // set PLL divider
    hwp_analogReg->sdm_mempll_reg1 = _pllDividerByFreq(CONFIG_DEFAULT_MEMPLL_FREQ);
    hwp_analogReg->sdm_apll_reg1 = _pllDividerByFreq(CONFIG_DEFAULT_CPUPLL_FREQ);

    // wait a while for frequency stable
    prvDelayUS(300);

    // enable MEMPLL, in case it is not enabled for unknown reason
    REG_SYS_CTRL_CFG_MISC1_CFG_T cfg_misc1 = {hwp_sysCtrl->cfg_misc1_cfg};
    cfg_misc1.b.mempll_enable = 1;
    hwp_sysCtrl->cfg_misc1_cfg = cfg_misc1.v;

    REG_SYS_CTRL_CFG_PLL_AP_CPU_DIV_T ap_cpu_div = {};
    ap_cpu_div.b.cfg_pll_ap_cpu_div =
        _sysDividerRegByFreq(CONFIG_DEFAULT_CPUPLL_FREQ,
                             CONFIG_DEFAULT_CPU_FREQ);
    ap_cpu_div.b.cfg_pll_ap_cpu_div_update = 1;
    hwp_sysCtrl->cfg_pll_ap_cpu_div = ap_cpu_div.v;

    // CP CPU frequency should be the same as AXI frequency
    // Actually, this setting is ignored by hardware, and
    // just set a reasonable value not to confuse users.
    REG_SYS_CTRL_CFG_PLL_CP_CPU_DIV_T cp_cpu_div = {};
    cp_cpu_div.b.cfg_pll_cp_cpu_div =
        _sysDividerRegByFreq(CONFIG_DEFAULT_CPUPLL_FREQ,
                             CONFIG_DEFAULT_SYSAXI_FREQ);
    cp_cpu_div.b.cfg_pll_cp_cpu_div_update = 1;
    hwp_sysCtrl->cfg_pll_cp_cpu_div = cp_cpu_div.v;

    REG_SYS_CTRL_CFG_PLL_SYS_AXI_DIV_T sys_axi_div = {};
    sys_axi_div.b.cfg_pll_sys_axi_div =
        _sysDividerRegByFreq(CONFIG_DEFAULT_CPUPLL_FREQ,
                             CONFIG_DEFAULT_SYSAXI_FREQ);
    sys_axi_div.b.cfg_pll_sys_axi_div_update = 1;
    hwp_sysCtrl->cfg_pll_sys_axi_div = sys_axi_div.v;

    REG_SYS_CTRL_CFG_PLL_SYS_AHB_APB_DIV_T ahb_apb_div = {};
    ahb_apb_div.b.cfg_pll_ap_apb_div =
        _sysDividerRegByFreq(CONFIG_DEFAULT_SYSAXI_FREQ,
                             CONFIG_DEFAULT_SYSAHB_FREQ);
    ahb_apb_div.b.cfg_pll_ap_apb_div_update = 1;
    hwp_sysCtrl->cfg_pll_sys_ahb_apb_div = ahb_apb_div.v;

    // Attention: the divider should provide 2x MEMBUS_FREQ
    REG_SYS_CTRL_CFG_PLL_MEM_BRIDGE_DIV_T mem_div = {};
    mem_div.b.cfg_mem_bridge_div =
        _sysDividerRegByFreq(CONFIG_DEFAULT_MEMPLL_FREQ,
                             CONFIG_DEFAULT_MEMBUS_FREQ * 2);
    mem_div.b.cfg_pll_mem_bridge_div_update = 1;
    hwp_sysCtrl->cfg_pll_mem_bridge_div = mem_div.v;

    // FLASH controller clock, divided from AXI
    REG_SYS_CTRL_CFG_PLL_SYS_SPIFLASH_DIV_T sys_spiflash_div = {};
    sys_spiflash_div.b.cfg_pll_sys_spiflash_div =
        _sysDividerRegByFreq(CONFIG_DEFAULT_SYSAXI_FREQ,
                             CONFIG_DEFAULT_SPIFLASH_CTRL_FREQ);
    sys_spiflash_div.b.cfg_pll_sys_spiflash_div_update = 1;
    hwp_sysCtrl->cfg_pll_sys_spiflash_div = sys_spiflash_div.v;

    // FLASH device clock, divided from PLL
    REG_SYS_CTRL_CFG_PLL_SPIFLASH_DIV_T spiflash_div = {};
    spiflash_div.b.cfg_pll_spiflash_div =
        _sysDividerRegByFreq(CONFIG_DEFAULT_CPUPLL_FREQ,
                             CONFIG_DEFAULT_SPIFLASH_DEV_FREQ);
    spiflash_div.b.cfg_pll_spiflash_div_update = 1;
    hwp_sysCtrl->cfg_pll_spiflash_div = spiflash_div.v;

    REG_SYS_CTRL_CFG_AP_CPU_DBGEN_DIV_T cfg_ap_cpu_dbgen_div = {
        .b.cfg_ap_cpu_dbgen_div = 2, // APCPU_FREQ / 4
        .b.cfg_ap_cpu_dbgen_div_update = 1,
    };
    hwp_sysCtrl->cfg_ap_cpu_dbgen_div = cfg_ap_cpu_dbgen_div.v;

    REG_SYS_CTRL_CFG_CP_CPU_DBGEN_DIV_T cfg_cp_cpu_dbgen_div = {
        .b.cfg_cp_cpu_dbgen_div = 2, // CPCPU_FREQ / 4
        .b.cfg_cp_cpu_dbgen_div_update = 1,
    };
    hwp_sysCtrl->cfg_cp_cpu_dbgen_div = cfg_cp_cpu_dbgen_div.v;

    REG_SYS_CTRL_CFG_CORESIGHT_DIV_T cfg_coresight_div = {
        .b.cfg_coresight_div = 3, // CPUPLL_FREQ / 8
        .b.cfg_coresight_div_update = 1,
    };
    hwp_sysCtrl->cfg_coresight_div = cfg_coresight_div.v;

    // enable APLL low power mode
    REG_ANALOG_REG_APLL_REG0_T apll_reg0;
    REG_FIELD_CHANGE1(hwp_analogReg->apll_reg0, apll_reg0, apll_lp_mode_en, 1);

    // enable MEMPLL low power mode
    REG_ANALOG_REG_MEMPLL_REG0_T mempll_reg0;
    REG_FIELD_CHANGE1(hwp_analogReg->mempll_reg0, mempll_reg0, mempll_lp_mode_en, 1);

    spi_config.b.quad_mode = 1;
    spi_config.b.sample_delay = 2;
    spi_config.b.clk_divider = CONFIG_DEFAULT_SPIFLASH_DEV_FREQ / CONFIG_DEFAULT_SPIFLASH_INTF_FREQ;

    sel_clock.v = hwp_sysCtrl->sel_clock;
    sel_clock.b.sys_sel_fast = 0; // 0 is fast
    sel_clock.b.soft_sel_mem_bridge = 0;
    sel_clock.b.soft_sel_spiflash = 0;

    halApplyRegisters((uint32_t)&hwp_spiFlash->spi_config, spi_config.v,
                      (uint32_t)&hwp_sysCtrl->sel_clock, sel_clock.v,
                      REG_APPLY_TYPE_UDELAY, 10,
                      REG_APPLY_END);

    REG_SYS_CTRL_CFG_MISC1_CFG_T cfg_misc1_cfg;
    REG_FIELD_CHANGE1(hwp_sysCtrl->cfg_misc1_cfg, cfg_misc1_cfg,
                      apll_zsp_clk_sel, 1);

    REG_SYS_CTRL_CFG_CLK_494M_LTE_DIV_T cfg_clk_494m_lte_div;
    REG_FIELD_CHANGE6(hwp_sysCtrl->cfg_clk_494m_lte_div, cfg_clk_494m_lte_div,
                      cfg_clk_494m_lte_apll_div, 0xd,
                      cfg_clk_494m_lte_apll_div_update, 1,
                      cfg_clksw_494m_clk_sel, 1,
                      cfg_clk_96m_26m_sel, 1,
                      cfg_clk_96m_div, 0xD005,
                      cfg_clk_96m_div_update, 1);

    REG_CP_IDLE_AP_INTEN_T ap_inten = {
        .b.osw2_irq = 1,
        .b.sys_wak_irq = 1};
    hwp_idle->ap_inten_set = ap_inten.v;

    // close DSI PLL
    REG_CP_IDLE_IDLE_CG_SW_T idle_cg_sw = {.b.dsipll = 1};
    hwp_idle->idle_cg_sw_set = idle_cg_sw.v;

    REG_CP_IDLE_IDLE_CG_SEL_T idle_cg_sel = {.b.dsipll = 1};
    hwp_idle->idle_cg_sel_set = idle_cg_sel.v;

    REG_CP_IDLE_PD_PLL_SW_T pd_pll_sw = {.b.dsipll = 1};
    hwp_idle->pd_pll_sw_set = pd_pll_sw.v;

    REG_CP_IDLE_PD_PLL_SEL_T pd_pll_sel = {.b.dsipll = 1};
    hwp_idle->pd_pll_sel_set = pd_pll_sel.v;

    // enable wakeup sources
    REG_CP_IDLE_IDL_AWK_EN_T idl_awk_en = {};
    idl_awk_en.b.awk0_en = 1; // pmic
    // idl_awk_en.b.awk1_en = 1; // vad_int
    idl_awk_en.b.awk2_en = 1; // key
    idl_awk_en.b.awk3_en = 1; // gpio1
    idl_awk_en.b.awk4_en = 1; // uart1
    idl_awk_en.b.awk5_en = 1; // pad_uart1_rxd
#ifdef CONFIG_BLUEU_BT_ENABLE
    idl_awk_en.b.awk6_en = 1; // wcn2sys
    idl_awk_en.b.awk7_en = 1; // wcn_osc_en
#endif
    idl_awk_en.b.awk_osw1_en = 1;
    idl_awk_en.b.awk_osw2_en = 1;
    hwp_idle->idl_awk_en_set = idl_awk_en.v;

    REG_CP_IDLE_IDL_EN_T idl_en = {.b.idl_cp_en = 1, .b.idl_ap_en = 1};
    hwp_idle->idl_en = idl_en.v;

    hwp_idle->slow_sys_clk_sel_hwen = 1;
    hwp_idle->slow_clk_sel_hwen = 1;

    // care: pm1 deepsleep,must close AON_LP domain,avoid wakeup,bus error.
    REG_CP_PWRCTRL_PWR_HWEN_T pwr_hwen;
    REG_FIELD_CHANGE1(hwp_pwrctrl->pwr_hwen, pwr_hwen, aon_lp_pon_en, 1);

    // enable ddr low power function
    REG_CP_CLKRST_CLKEN_BB_SYSCTRL_T clken_bb_sysctrl = {.b.clken_sysreg = 1};
    hwp_clkrst->clken_bb_sysctrl_set = clken_bb_sysctrl.v;

    hwp_sysreg->ddr_slp_ctrl_enable = 0;

    REG_CP_SYSREG_A5_STANDBYWFI_EN_T a5_standbywfi_en = {
        .b.ap_a5_standby_en = 1,
        .b.cp_a5_standby_en = 1};
    hwp_sysreg->a5_standbywfi_en = a5_standbywfi_en.v;

    // enable lp mode
    REG_SYS_CTRL_CFG_FORCE_LP_MODE_LP_T force_lp_mode_lp = {
        .b.cfg_mode_lp_a5 = 1,
        .b.cfg_mode_lp_ahb = 1,
        .b.cfg_mode_lp_connect = 1,
        .b.cfg_mode_lp_cp_a5 = 1,
        .b.cfg_mode_lp_psram = 1,
        .b.cfg_mode_lp_spiflash = 1,
        .b.cfg_mode_lp_spiflash1 = 1,
        .b.cfg_mode_lp_ahb_merge = 1}; // 0x7f80
    hwp_sysCtrl->cfg_force_lp_mode_lp = force_lp_mode_lp.v;

    REG_RF_RTC_REG_C4_REG_T rfrtc_c4 = {
        .b.xtal_osc_ibit_n = 8,
        .b.xtal_osc_ibit_l = 0,
        .b.xtal26m_pwadc_en = 0,
        .b.xtal_reg_bit = 8,
        .b.xtal26m_tsxadc_en = 0,
        .b.xtal26m_plls1_en = 1,
        .b.xtal26m_plls2_en = 1}; // 0xd008
    hwp_rfRtc->reg_c4_reg = rfrtc_c4.v;

    REG_RF_RTC_REG_C8_REG_T rfrtc_c8 = {
        .b.xtal_fixi_bit_n = 16,
        .b.xtal_fixi_bit_l = 1,
        .b.xtal26m_ts_en = 1,
        .b.xtal26m_pllcal_en = 1}; // 0x0c30
    hwp_rfRtc->reg_c8_reg = rfrtc_c8.v;

    // enable zsp bus low power
    hwp_clkrst->zsp_buslpmc_set = 0xF;

    // clock autogate (WHY?)
    hwp_sysCtrl->clk_sys_axi_mode = 0x1F;
    hwp_sysCtrl->clk_sys_ahb_mode = 0x1E;
    hwp_sysCtrl->clk_ap_apb_mode = 0x782001;
    hwp_sysCtrl->clk_aif_apb_mode = 0xF01;
    hwp_sysCtrl->clk_aon_apb_mode = 0xFE6C1;
    hwp_sysCtrl->clk_others_mode = 0xF8;

    REG_SYS_CTRL_CLK_SYS_AXI_DISABLE_T clk_sys_axi_disable = {
        .b.disable_sys_axi_clk_id_sdmmc1 = 1};
    hwp_sysCtrl->clk_sys_axi_disable = clk_sys_axi_disable.v;

    REG_SYS_CTRL_CLK_AON_APB_DISABLE_T clk_aon_apb_disable = {
        .b.disable_aon_apb_clk_id_lvds = 1,
        .b.disable_aon_apb_clk_id_vad = 1};
    hwp_sysCtrl->clk_aon_apb_disable = clk_aon_apb_disable.v;

    REG_SYS_CTRL_CLK_OTHERS_1_DISABLE_T clk_others_1_disable = {
        .b.disable_oc_id_usb11_48m = 1};
    hwp_sysCtrl->clk_others_1_disable = clk_others_1_disable.v;

    REG_SYS_CTRL_CLK_AP_APB_DISABLE_T clk_ap_apb_disable = {
        .b.disable_ap_apb_clk_id_mod_camera = 1,
        .b.disable_ap_apb_clk_id_camera = 1,
        .b.disable_ap_apb_clk_id_pagespy = 1};
    hwp_sysCtrl->clk_ap_apb_disable = clk_ap_apb_disable.v;

    REG_SYS_CTRL_CLK_OTHERS_DISABLE_T clk_others_disable = {
        .b.disable_oc_id_pix = 1,
        .b.disable_oc_id_isp = 1,
        .b.disable_oc_id_spicam = 1,
        .b.disable_oc_id_cam = 1,
        .b.disable_oc_id_psram_pagespy = 1};
    hwp_sysCtrl->clk_others_disable = clk_others_disable.v;

    REG_SYS_CTRL_CLK_AIF_APB_DISABLE_T clk_aif_apb_disable = {
        .b.disable_aif_apb_clk_id_aud_2ad = 1};
    hwp_sysCtrl->clk_aif_apb_disable = clk_aif_apb_disable.v;

    // Sysmail registers doesn't have determinstic value on boot, and it is
    // needed to clear them manually. It is ok to write 0 to the hole
    // (reserved area) inside.
    for (uintptr_t address = (uintptr_t)&hwp_mailbox->sysmail0;
         address <= (uintptr_t)&hwp_mailbox->sysmail191; address += 4)
        *(volatile unsigned *)address = 0;

    // Release spinlock. Maybe in abnormal reset, the spinlock is hold by ap.
    halHwspinlockReleaseInCritical(HAL_HWSPINLOCK_ID_CPSLEEP);
}
