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

#ifndef _RDA2720M_GLOBAL_H_
#define _RDA2720M_GLOBAL_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_GLOBAL_BASE (0x50308c00)

typedef volatile struct
{
    uint32_t chip_id_low;          // 0x00000000
    uint32_t chip_id_high;         // 0x00000004
    uint32_t module_en0;           // 0x00000008
    uint32_t arm_clk_en0;          // 0x0000000c
    uint32_t rtc_clk_en0;          // 0x00000010
    uint32_t soft_rst0;            // 0x00000014
    uint32_t soft_rst1;            // 0x00000018
    uint32_t power_pd_sw;          // 0x0000001c
    uint32_t power_pd_hw;          // 0x00000020
    uint32_t soft_rst_hw;          // 0x00000024
    uint32_t reserved_reg1;        // 0x00000028
    uint32_t reserved_reg2;        // 0x0000002c
    uint32_t reserved_reg3;        // 0x00000030
    uint32_t dcdc_clk_reg0;        // 0x00000034
    uint32_t dcdc_core_reg0;       // 0x00000038
    uint32_t dcdc_core_reg1;       // 0x0000003c
    uint32_t dcdc_core_reg2;       // 0x00000040
    uint32_t dcdc_core_vol;        // 0x00000044
    uint32_t dcdc_gen_reg0;        // 0x00000048
    uint32_t dcdc_gen_reg1;        // 0x0000004c
    uint32_t dcdc_gen_reg2;        // 0x00000050
    uint32_t dcdc_gen_vol;         // 0x00000054
    uint32_t dcdc_wpa_reg0;        // 0x00000058
    uint32_t dcdc_wpa_reg1;        // 0x0000005c
    uint32_t dcdc_wpa_reg2;        // 0x00000060
    uint32_t dcdc_wpa_vol;         // 0x00000064
    uint32_t dcdc_wpa_dcm_hw;      // 0x00000068
    uint32_t dcdc_ch_ctrl;         // 0x0000006c
    uint32_t reserved_reg4;        // 0x00000070
    uint32_t reserved_reg5;        // 0x00000074
    uint32_t reserved_reg6;        // 0x00000078
    uint32_t reserved_reg7;        // 0x0000007c
    uint32_t reserved_reg8;        // 0x00000080
    uint32_t ldo_ana_reg0;         // 0x00000084
    uint32_t ldo_ana_reg1;         // 0x00000088
    uint32_t reserved_reg9;        // 0x0000008c
    uint32_t reserved_reg10;       // 0x00000090
    uint32_t ldo_rf15_reg0;        // 0x00000094
    uint32_t ldo_rf15_reg1;        // 0x00000098
    uint32_t reserved_reg11;       // 0x0000009c
    uint32_t reserved_reg12;       // 0x000000a0
    uint32_t ldo_camd_reg0;        // 0x000000a4
    uint32_t ldo_camd_reg1;        // 0x000000a8
    uint32_t ldo_con_reg0;         // 0x000000ac
    uint32_t ldo_con_reg1;         // 0x000000b0
    uint32_t ldo_mem_reg0;         // 0x000000b4
    uint32_t ldo_mem_reg1;         // 0x000000b8
    uint32_t ldo_sim0_reg0;        // 0x000000bc
    uint32_t ldo_sim0_pd_reg;      // 0x000000c0
    uint32_t ldo_sim0_reg1;        // 0x000000c4
    uint32_t ldo_sim1_reg0;        // 0x000000c8
    uint32_t ldo_sim1_pd_reg;      // 0x000000cc
    uint32_t ldo_sim1_reg1;        // 0x000000d0
    uint32_t reserved_reg13;       // 0x000000d4
    uint32_t reserved_reg14;       // 0x000000d8
    uint32_t reserved_reg15;       // 0x000000dc
    uint32_t ldo_cama_reg0;        // 0x000000e0
    uint32_t ldo_cama_reg1;        // 0x000000e4
    uint32_t ldo_lcd_reg0;         // 0x000000e8
    uint32_t ldo_lcd_reg1;         // 0x000000ec
    uint32_t ldo_mmc_reg0;         // 0x000000f0
    uint32_t ldo_mmc_pd_reg;       // 0x000000f4
    uint32_t ldo_mmc_reg1;         // 0x000000f8
    uint32_t ldo_sd_reg0;          // 0x000000fc
    uint32_t ldo_sd_pd_reg;        // 0x00000100
    uint32_t ldo_sd_reg1;          // 0x00000104
    uint32_t ldo_ddr12_reg0;       // 0x00000108
    uint32_t ldo_ddr12_pd_reg;     // 0x0000010c
    uint32_t ldo_ddr12_reg1;       // 0x00000110
    uint32_t ldo_vdd28_reg0;       // 0x00000114
    uint32_t ldo_vdd28_reg1;       // 0x00000118
    uint32_t ldo_spimem_reg0;      // 0x0000011c
    uint32_t ldo_spimem_reg1;      // 0x00000120
    uint32_t ldo_dcxo_reg0;        // 0x00000124
    uint32_t ldo_dcxo_reg1;        // 0x00000128
    uint32_t ldo_usb_reg0;         // 0x0000012c
    uint32_t ldo_usb_pd_reg;       // 0x00000130
    uint32_t ldo_usb_reg1;         // 0x00000134
    uint32_t reserved_reg17;       // 0x00000138
    uint32_t reserved_reg18;       // 0x0000013c
    uint32_t reserved_reg19;       // 0x00000140
    uint32_t reserved_reg20;       // 0x00000144
    uint32_t ldo_trim_reg;         // 0x00000148
    uint32_t ldo_rtc_ctrl;         // 0x0000014c
    uint32_t ldo_ch_ctrl;          // 0x00000150
    uint32_t reserved_reg23;       // 0x00000154
    uint32_t reserved_reg24;       // 0x00000158
    uint32_t reserved_reg25;       // 0x0000015c
    uint32_t reserved_reg26;       // 0x00000160
    uint32_t reserved_reg27;       // 0x00000164
    uint32_t slp_ctrl;             // 0x00000168
    uint32_t slp_dcdc_pd_ctrl;     // 0x0000016c
    uint32_t slp_ldo_pd_ctrl0;     // 0x00000170
    uint32_t slp_ldo_pd_ctrl1;     // 0x00000174
    uint32_t slp_dcdc_lp_ctrl;     // 0x00000178
    uint32_t slp_ldo_lp_ctrl0;     // 0x0000017c
    uint32_t slp_ldo_lp_ctrl1;     // 0x00000180
    uint32_t dcdc_core_slp_ctrl0;  // 0x00000184
    uint32_t dcdc_core_slp_ctrl1;  // 0x00000188
    uint32_t dcdc_xtl_en0;         // 0x0000018c
    uint32_t reseved_dcdc_xtl_en4; // 0x00000190
    uint32_t ldo_xtl_en0;          // 0x00000194
    uint32_t ldo_xtl_en1;          // 0x00000198
    uint32_t ldo_xtl_en2;          // 0x0000019c
    uint32_t ldo_xtl_en3;          // 0x000001a0
    uint32_t ldo_xtl_en4;          // 0x000001a4
    uint32_t ldo_xtl_en5;          // 0x000001a8
    uint32_t ldo_xtl_en6;          // 0x000001ac
    uint32_t ldo_xtl_en7;          // 0x000001b0
    uint32_t ldo_xtl_en8;          // 0x000001b4
    uint32_t ldo_xtl_en9;          // 0x000001b8
    uint32_t ldo_xtl_en10;         // 0x000001bc
    uint32_t reserved_reg30;       // 0x000001c0
    uint32_t reserved_reg31;       // 0x000001c4
    uint32_t reserved_reg32;       // 0x000001c8
    uint32_t reserved_reg_core;    // 0x000001cc
    uint32_t reserved_reg_rtc;     // 0x000001d0
    uint32_t bg_ctrl;              // 0x000001d4
    uint32_t dcdc_vlg_sel;         // 0x000001d8
    uint32_t ldo_vlg_sel0;         // 0x000001dc
    uint32_t clk32kless_ctrl0;     // 0x000001e0
    uint32_t clk32kless_ctrl1;     // 0x000001e4
    uint32_t xtl_wait_ctrl0;       // 0x000001e8
    uint32_t rgb_ctrl;             // 0x000001ec
    uint32_t ib_ctrl;              // 0x000001f0
    uint32_t flash_ctrl;           // 0x000001f4
    uint32_t kpled_ctrl0;          // 0x000001f8
    uint32_t kpled_ctrl1;          // 0x000001fc
    uint32_t vibr_ctrl0;           // 0x00000200
    uint32_t vibr_ctrl1;           // 0x00000204
    uint32_t audio_ctrl0;          // 0x00000208
    uint32_t chgr_ctrl0;           // 0x0000020c
    uint32_t chgr_ctrl1;           // 0x00000210
    uint32_t chgr_status;          // 0x00000214
    uint32_t chgr_det_fgu_ctrl;    // 0x00000218
    uint32_t ovlo_ctrl;            // 0x0000021c
    uint32_t mixed_ctrl;           // 0x00000220
    uint32_t por_rst_monitor;      // 0x00000224
    uint32_t wdg_rst_monitor;      // 0x00000228
    uint32_t por_pin_rst_monitor;  // 0x0000022c
    uint32_t por_src_flag;         // 0x00000230
    uint32_t por_off_flag;         // 0x00000234
    uint32_t por_7s_ctrl;          // 0x00000238
    uint32_t hwrst_rtc;            // 0x0000023c
    uint32_t arch_en;              // 0x00000240
    uint32_t mcu_wr_prot_value;    // 0x00000244
    uint32_t pwr_wr_prot_value;    // 0x00000248
    uint32_t smpl_ctrl0;           // 0x0000024c
    uint32_t smpl_ctrl1;           // 0x00000250
    uint32_t rtc_rst0;             // 0x00000254
    uint32_t rtc_rst1;             // 0x00000258
    uint32_t rtc_rst2;             // 0x0000025c
    uint32_t rtc_clk_stop;         // 0x00000260
    uint32_t vbat_drop_cnt;        // 0x00000264
    uint32_t swrst_ctrl0;          // 0x00000268
    uint32_t swrst_ctrl1;          // 0x0000026c
    uint32_t otp_ctrl;             // 0x00000270
    uint32_t free_timer_low;       // 0x00000274
    uint32_t free_timer_high;      // 0x00000278
    uint32_t vol_tune_ctrl_core;   // 0x0000027c
} HWP_RDA2720M_GLOBAL_T;

#define hwp_rda2720mGlobal ((HWP_RDA2720M_GLOBAL_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_GLOBAL_BASE))

// chip_id_low
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chip_ip_low : 16; // [15:0], read only
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_CHIP_ID_LOW_T;

// chip_id_high
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chip_id_high : 16; // [15:0], read only
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_CHIP_ID_HIGH_T;

// module_en0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cal_en : 1;    // [0]
        uint32_t rtc_en : 1;    // [1]
        uint32_t wdg_en : 1;    // [2]
        uint32_t eic_en : 1;    // [3]
        uint32_t aud_en : 1;    // [4]
        uint32_t adc_en : 1;    // [5]
        uint32_t efs_en : 1;    // [6]
        uint32_t fgu_en : 1;    // [7]
        uint32_t pinreg_en : 1; // [8]
        uint32_t bltc_en : 1;   // [9]
        uint32_t psm_en : 1;    // [10]
        uint32_t __11_11 : 1;   // [11]
        uint32_t tmr_en : 1;    // [12]
        uint32_t __31_13 : 19;  // [31:13]
    } b;
} REG_RDA2720M_GLOBAL_MODULE_EN0_T;

// arm_clk_en0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_aud_if_en : 1;      // [0]
        uint32_t clk_aud_if_6p5m_en : 1; // [1]
        uint32_t clk_cal_en : 1;         // [2]
        uint32_t clk_cal_src_sel : 2;    // [4:3]
        uint32_t clk_auxadc_en : 1;      // [5]
        uint32_t clk_auxad_en : 1;       // [6]
        uint32_t __31_7 : 25;            // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_ARM_CLK_EN0_T;

// rtc_clk_en0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_arch_en : 1;  // [0]
        uint32_t rtc_rtc_en : 1;   // [1]
        uint32_t rtc_wdg_en : 1;   // [2]
        uint32_t rtc_eic_en : 1;   // [3]
        uint32_t __5_4 : 2;        // [5:4]
        uint32_t rtc_fgu_en : 1;   // [6]
        uint32_t rtc_bltc_en : 1;  // [7]
        uint32_t __10_8 : 3;       // [10:8]
        uint32_t rtc_efs_en : 1;   // [11]
        uint32_t rtc_flash_en : 1; // [12]
        uint32_t rtc_tmr_en : 1;   // [13]
        uint32_t __31_14 : 18;     // [31:14]
    } b;
} REG_RDA2720M_GLOBAL_RTC_CLK_EN0_T;

// soft_rst0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cal_soft_rst : 1;    // [0]
        uint32_t rtc_soft_rst : 1;    // [1]
        uint32_t wdg_soft_rst : 1;    // [2]
        uint32_t eic_soft_rst : 1;    // [3]
        uint32_t fgu_soft_rst : 1;    // [4]
        uint32_t psm_soft_rst : 1;    // [5]
        uint32_t adc_soft_rst : 1;    // [6]
        uint32_t efs_soft_rst : 1;    // [7]
        uint32_t aud_if_soft_rst : 1; // [8]
        uint32_t bltc_soft_rst : 1;   // [9]
        uint32_t __11_10 : 2;         // [11:10]
        uint32_t audtx_soft_rst : 1;  // [12]
        uint32_t audrx_soft_rst : 1;  // [13]
        uint32_t __31_14 : 18;        // [31:14]
    } b;
} REG_RDA2720M_GLOBAL_SOFT_RST0_T;

// soft_rst1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;        // [0]
        uint32_t tmr_soft_rst : 1; // [1]
        uint32_t __31_2 : 30;      // [31:2]
    } b;
} REG_RDA2720M_GLOBAL_SOFT_RST1_T;

// power_pd_sw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bg_pd : 1;        // [0]
        uint32_t ldo_vdd28_pd : 1; // [1]
        uint32_t ldo_ana_pd : 1;   // [2]
        uint32_t ldo_mem_pd : 1;   // [3]
        uint32_t osc3m_en : 1;     // [4]
        uint32_t dcdc_core_pd : 1; // [5]
        uint32_t __6_6 : 1;        // [6]
        uint32_t dcdc_gen_pd : 1;  // [7]
        uint32_t ldo_cp_pd : 1;    // [8]
        uint32_t ldo_emm_pd : 1;   // [9]
        uint32_t ldo_dcxo_pd : 1;  // [10]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_GLOBAL_POWER_PD_SW_T;

// power_pd_hw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pwr_off_seq_en : 1; // [0]
        uint32_t __31_1 : 31;        // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_POWER_PD_HW_T;

// soft_rst_hw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_soft_rst : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_SOFT_RST_HW_T;

// dcdc_clk_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t osc3m_freq : 4;       // [3:0]
        uint32_t dcdc_gen_clk_rst : 1; // [4]
        uint32_t __6_5 : 2;            // [6:5]
        uint32_t clkout_sel : 3;       // [9:7]
        uint32_t clkout_uniphase : 1;  // [10]
        uint32_t clk3m_out_en : 1;     // [11]
        uint32_t __31_12 : 20;         // [31:12]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_CLK_REG0_T;

// dcdc_core_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zx_offset_vcore : 2;  // [1:0]
        uint32_t sr_ls_vcore : 2;      // [3:2]
        uint32_t sr_hs_vcore : 2;      // [5:4]
        uint32_t slope_vcore : 2;      // [7:6]
        uint32_t rcomp_vcore : 2;      // [9:8]
        uint32_t pfm_vh_vcore : 2;     // [11:10]
        uint32_t curses_r_vcore : 2;   // [13:12]
        uint32_t curlimit_r_vcore : 2; // [15:14]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_CORE_REG0_T;

// dcdc_core_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t anriting_en_vcore : 1; // [0]
        uint32_t force_pwm_vcore : 1;   // [1]
        uint32_t zx_disable_vcore : 1;  // [2]
        uint32_t votrim_vcore : 3;      // [5:3]
        uint32_t __31_6 : 26;           // [31:6]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_CORE_REG1_T;

// dcdc_core_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t div_base_vcore : 6;   // [5:0]
        uint32_t phase_sel_vcore : 6;  // [11:6]
        uint32_t div_clk_vcore_en : 1; // [12]
        uint32_t __31_13 : 19;         // [31:13]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_CORE_REG2_T;

// dcdc_core_vol
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vosel_vcore : 9; // [8:0]
        uint32_t __31_9 : 23;     // [31:9]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_CORE_VOL_T;

// dcdc_gen_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zx_offset_vgen : 2;  // [1:0]
        uint32_t sr_ls_vgen : 2;      // [3:2]
        uint32_t sr_hs_vgen : 2;      // [5:4]
        uint32_t slope_vgen : 2;      // [7:6]
        uint32_t rcomp_vgen : 2;      // [9:8]
        uint32_t pfm_vh_vgen : 2;     // [11:10]
        uint32_t curses_r_vgen : 2;   // [13:12]
        uint32_t curlimit_r_vgen : 2; // [15:14]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_GEN_REG0_T;

// dcdc_gen_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t anriting_en_vgen : 1; // [0]
        uint32_t force_pwm_vgen : 1;   // [1]
        uint32_t maxduty_sel_vgen : 1; // [2]
        uint32_t zx_disable_vgen : 1;  // [3]
        uint32_t __31_4 : 28;          // [31:4]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_GEN_REG1_T;

// dcdc_gen_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t div_base_vgen : 6;   // [5:0]
        uint32_t phase_sel_vgen : 6;  // [11:6]
        uint32_t div_clk_vgen_en : 1; // [12]
        uint32_t __31_13 : 19;        // [31:13]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_GEN_REG2_T;

// dcdc_gen_vol
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vosel_vgen : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_GEN_VOL_T;

// dcdc_wpa_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sr_hs_vpa : 2;          // [1:0]
        uint32_t sawtooth_slope_vpa : 2; // [3:2]
        uint32_t rcomp3_vpa : 2;         // [5:4]
        uint32_t rcomp2_vpa : 2;         // [7:6]
        uint32_t pfm_threshold_vpa : 2;  // [9:8]
        uint32_t ccomp3_vpa : 2;         // [11:10]
        uint32_t curses_m_vpa : 2;       // [13:12]
        uint32_t curlimit_r_vpa : 2;     // [15:14]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_WPA_REG0_T;

// dcdc_wpa_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t maxduty_sel_vpa : 1;      // [0]
        uint32_t dvs_on_vpa : 1;           // [1]
        uint32_t bypass_threshold_vpa : 2; // [3:2]
        uint32_t bypass_forceon_vpa : 1;   // [4]
        uint32_t bypass_disable_vpa : 1;   // [5]
        uint32_t apc_ramp_sel_vpa : 1;     // [6]
        uint32_t apc_enable_vpa : 1;       // [7]
        uint32_t anriting_en_vpa : 1;      // [8]
        uint32_t zx_offset_vpa : 2;        // [10:9]
        uint32_t votrim_vpa : 3;           // [13:11]
        uint32_t sr_ls_vpa : 2;            // [15:14]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_WPA_REG1_T;

// dcdc_wpa_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pd_buck_vpa : 1;         // [0]
        uint32_t sawtoothcal_rst_vpa : 1; // [1]
        uint32_t zx_disable_vpa : 1;      // [2]
        uint32_t div_base_vpa : 6;        // [8:3]
        uint32_t phase_sel_vpa : 6;       // [14:9]
        uint32_t div_clk_vpa_en : 1;      // [15]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_WPA_REG2_T;

// dcdc_wpa_vol
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vosel_vpa : 7; // [6:0]
        uint32_t __31_7 : 25;   // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_WPA_VOL_T;

// dcdc_wpa_dcm_hw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t force_pwm_vpa : 1; // [0]
        uint32_t __31_1 : 31;       // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_WPA_DCM_HW_T;

// dcdc_ch_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dcdc_auxtrim_sel : 3; // [2:0]
        uint32_t __31_3 : 29;          // [31:3]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_CH_CTRL_T;

// ldo_ana_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;            // [0]
        uint32_t ldo_ana_cl_adj : 1;   // [1]
        uint32_t ldo_ana_shpt_adj : 1; // [2]
        uint32_t ldo_ana_stb : 2;      // [4:3]
        uint32_t ldo_ana_bp : 1;       // [5]
        uint32_t ldo_ana_cap_sel : 1;  // [6]
        uint32_t ldo_ana_shpt_pd : 1;  // [7]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_RDA2720M_GLOBAL_LDO_ANA_REG0_T;

// ldo_ana_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_ana_v : 6; // [5:0]
        uint32_t __31_6 : 26;   // [31:6]
    } b;
} REG_RDA2720M_GLOBAL_LDO_ANA_REG1_T;

// ldo_rf15_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_rf15_pd : 1;       // [0]
        uint32_t ldo_rf15_cl_adj : 1;   // [1]
        uint32_t ldo_rf15_shpt_adj : 1; // [2]
        uint32_t ldo_rf15_stb : 2;      // [4:3]
        uint32_t ldo_rf15_bp : 1;       // [5]
        uint32_t ldo_rf15_cap_sel : 1;  // [6]
        uint32_t ldo_rf15_shpt_pd : 1;  // [7]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_RDA2720M_GLOBAL_LDO_RF15_REG0_T;

// ldo_rf15_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_rf15_v : 6; // [5:0]
        uint32_t __31_6 : 26;    // [31:6]
    } b;
} REG_RDA2720M_GLOBAL_LDO_RF15_REG1_T;

// ldo_camd_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_camd_pd : 1;       // [0]
        uint32_t ldo_camd_cl_adj : 1;   // [1]
        uint32_t ldo_camd_shpt_adj : 1; // [2]
        uint32_t ldo_camd_stb : 2;      // [4:3]
        uint32_t ldo_camd_bp : 1;       // [5]
        uint32_t ldo_camd_cap_sel : 1;  // [6]
        uint32_t ldo_camd_shpt_pd : 1;  // [7]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_RDA2720M_GLOBAL_LDO_CAMD_REG0_T;

// ldo_camd_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_camd_v : 6; // [5:0]
        uint32_t __31_6 : 26;    // [31:6]
    } b;
} REG_RDA2720M_GLOBAL_LDO_CAMD_REG1_T;

// ldo_con_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_con_pd : 1;       // [0]
        uint32_t ldo_con_cl_adj : 1;   // [1]
        uint32_t ldo_con_shpt_adj : 1; // [2]
        uint32_t ldo_con_stb : 2;      // [4:3]
        uint32_t ldo_con_bp : 1;       // [5]
        uint32_t ldo_con_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;          // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_CON_REG0_T;

// ldo_con_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_con_v : 6; // [5:0]
        uint32_t __31_6 : 26;   // [31:6]
    } b;
} REG_RDA2720M_GLOBAL_LDO_CON_REG1_T;

// ldo_mem_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;            // [0]
        uint32_t ldo_mem_cl_adj : 1;   // [1]
        uint32_t ldo_mem_shpt_adj : 1; // [2]
        uint32_t ldo_mem_stb : 2;      // [4:3]
        uint32_t ldo_mem_bp : 1;       // [5]
        uint32_t ldo_mem_cap_sel : 1;  // [6]
        uint32_t ldo_mem_shpt_pd : 1;  // [7]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_RDA2720M_GLOBAL_LDO_MEM_REG0_T;

// ldo_mem_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_mem_v : 6; // [5:0]
        uint32_t __31_6 : 26;   // [31:6]
    } b;
} REG_RDA2720M_GLOBAL_LDO_MEM_REG1_T;

// ldo_sim0_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;             // [0]
        uint32_t ldo_sim0_cl_adj : 1;   // [1]
        uint32_t ldo_sim0_shpt_adj : 1; // [2]
        uint32_t ldo_sim0_stb : 2;      // [4:3]
        uint32_t ldo_sim0_cap_sel : 1;  // [5]
        uint32_t ldo_sim0_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;           // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SIM0_REG0_T;

// ldo_sim0_pd_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_sim0_pd : 1; // [0]
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SIM0_PD_REG_T;

// ldo_sim0_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_sim0_v : 7; // [6:0]
        uint32_t __31_7 : 25;    // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SIM0_REG1_T;

// ldo_sim1_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;             // [0]
        uint32_t ldo_sim1_cl_adj : 1;   // [1]
        uint32_t ldo_sim1_shpt_adj : 1; // [2]
        uint32_t ldo_sim1_stb : 2;      // [4:3]
        uint32_t ldo_sim1_cap_sel : 1;  // [5]
        uint32_t ldo_sim1_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;           // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SIM1_REG0_T;

// ldo_sim1_pd_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_sim1_pd : 1; // [0]
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SIM1_PD_REG_T;

// ldo_sim1_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_sim1_v : 7; // [6:0]
        uint32_t __31_7 : 25;    // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SIM1_REG1_T;

// ldo_cama_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_cama_pd : 1;       // [0]
        uint32_t ldo_cama_cl_adj : 1;   // [1]
        uint32_t ldo_cama_shpt_adj : 1; // [2]
        uint32_t ldo_cama_stb : 2;      // [4:3]
        uint32_t ldo_cama_cap_sel : 1;  // [5]
        uint32_t ldo_cama_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;           // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_CAMA_REG0_T;

// ldo_cama_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_cama_v : 7; // [6:0]
        uint32_t __31_7 : 25;    // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_CAMA_REG1_T;

// ldo_lcd_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_lcd_pd : 1;       // [0]
        uint32_t ldo_lcd_cl_adj : 1;   // [1]
        uint32_t ldo_lcd_shpt_adj : 1; // [2]
        uint32_t ldo_lcd_stb : 2;      // [4:3]
        uint32_t ldo_lcd_cap_sel : 1;  // [5]
        uint32_t ldo_lcd_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;          // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_LCD_REG0_T;

// ldo_lcd_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_lcd_v : 7; // [6:0]
        uint32_t __31_7 : 25;   // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_LCD_REG1_T;

// ldo_mmc_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;            // [0]
        uint32_t ldo_mmc_cl_adj : 1;   // [1]
        uint32_t ldo_mmc_shpt_adj : 1; // [2]
        uint32_t ldo_mmc_stb : 2;      // [4:3]
        uint32_t ldo_mmc_cap_sel : 1;  // [5]
        uint32_t ldo_mmc_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;          // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_MMC_REG0_T;

// ldo_mmc_pd_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_mmc_pd : 1; // [0]
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_LDO_MMC_PD_REG_T;

// ldo_mmc_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_mmc_v : 7; // [6:0]
        uint32_t __31_7 : 25;   // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_MMC_REG1_T;

// ldo_sd_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;              // [0]
        uint32_t ldo_vio18_cl_adj : 1;   // [1]
        uint32_t ldo_vio18_shpt_adj : 1; // [2]
        uint32_t ldo_vio18_stb : 2;      // [4:3]
        uint32_t ldo_vio18_cap_sel : 1;  // [5]
        uint32_t ldo_vio18_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;            // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SD_REG0_T;

// ldo_sd_pd_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vio18_pd : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SD_PD_REG_T;

// ldo_sd_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vio18_v : 7; // [6:0]
        uint32_t __31_7 : 25;     // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SD_REG1_T;

// ldo_ddr12_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;              // [0]
        uint32_t ldo_ddr12_cl_adj : 1;   // [1]
        uint32_t ldo_ddr12_shpt_adj : 1; // [2]
        uint32_t ldo_ddr12_stb : 2;      // [4:3]
        uint32_t ldo_ddr12_cap_sel : 1;  // [5]
        uint32_t ldo_ddr12_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;            // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_DDR12_REG0_T;

// ldo_ddr12_pd_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_ddr12_pd : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_LDO_DDR12_PD_REG_T;

// ldo_ddr12_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_ddr12_v : 7; // [6:0]
        uint32_t __31_7 : 25;     // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_DDR12_REG1_T;

// ldo_vdd28_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;              // [0]
        uint32_t ldo_vdd28_cl_adj : 1;   // [1]
        uint32_t ldo_vdd28_shpt_adj : 1; // [2]
        uint32_t ldo_vdd28_stb : 2;      // [4:3]
        uint32_t ldo_vdd28_cap_sel : 1;  // [5]
        uint32_t ldo_vdd28_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;            // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_VDD28_REG0_T;

// ldo_vdd28_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vdd28_v : 7; // [6:0]
        uint32_t __31_7 : 25;     // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_VDD28_REG1_T;

// ldo_spimem_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_spimem_pd : 1;       // [0]
        uint32_t ldo_spimem_cl_adj : 1;   // [1]
        uint32_t ldo_spimem_shpt_adj : 1; // [2]
        uint32_t ldo_spimem_stb : 2;      // [4:3]
        uint32_t ldo_spimem_cap_sel : 1;  // [5]
        uint32_t ldo_spimem_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;             // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SPIMEM_REG0_T;

// ldo_spimem_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_spimem_v : 7; // [6:0]
        uint32_t __31_7 : 25;      // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_SPIMEM_REG1_T;

// ldo_dcxo_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;             // [0]
        uint32_t ldo_dcxo_cl_adj : 1;   // [1]
        uint32_t ldo_dcxo_shpt_adj : 1; // [2]
        uint32_t ldo_dcxo_stb : 2;      // [4:3]
        uint32_t ldo_dcxo_cap_sel : 1;  // [5]
        uint32_t ldo_dcxo_shpt_pd : 1;  // [6]
        uint32_t ldo_dcxo_trim : 5;     // [11:7]
        uint32_t __31_12 : 20;          // [31:12]
    } b;
} REG_RDA2720M_GLOBAL_LDO_DCXO_REG0_T;

// ldo_dcxo_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_dcxo_v : 7; // [6:0]
        uint32_t __31_7 : 25;    // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_DCXO_REG1_T;

// ldo_usb_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;              // [0]
        uint32_t ldo_usb33_cl_adj : 1;   // [1]
        uint32_t ldo_usb33_shpt_adj : 1; // [2]
        uint32_t ldo_usb33_stb : 2;      // [4:3]
        uint32_t ldo_usb33_cap_sel : 1;  // [5]
        uint32_t ldo_usb33_shpt_pd : 1;  // [6]
        uint32_t __31_7 : 25;            // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_USB_REG0_T;

// ldo_usb_pd_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_usb33_pd : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_LDO_USB_PD_REG_T;

// ldo_usb_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_usb33_v : 7; // [6:0]
        uint32_t __31_7 : 25;     // [31:7]
    } b;
} REG_RDA2720M_GLOBAL_LDO_USB_REG1_T;

// ldo_trim_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_trim_a : 5; // [4:0]
        uint32_t ldo_trim_b : 5; // [9:5]
        uint32_t __31_10 : 22;   // [31:10]
    } b;
} REG_RDA2720M_GLOBAL_LDO_TRIM_REG_T;

// ldo_rtc_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vbatbk_v : 2;    // [1:0]
        uint32_t ldo_rtc_v : 2;   // [3:2]
        uint32_t ldo_rtc_cal : 5; // [8:4]
        uint32_t __31_9 : 23;     // [31:9]
    } b;
} REG_RDA2720M_GLOBAL_LDO_RTC_CTRL_T;

// ldo_ch_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_cal : 3;  // [2:0]
        uint32_t __4_3 : 2;    // [4:3]
        uint32_t ldo_cal1 : 3; // [7:5]
        uint32_t ldo_cal2 : 3; // [10:8]
        uint32_t __31_11 : 21; // [31:11]
    } b;
} REG_RDA2720M_GLOBAL_LDO_CH_CTRL_T;

// slp_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t slp_ldo_pd_en : 1; // [0]
        uint32_t slp_io_en : 1;     // [1]
        uint32_t ldo_xtl_en : 1;    // [2]
        uint32_t __31_3 : 29;       // [31:3]
    } b;
} REG_RDA2720M_GLOBAL_SLP_CTRL_T;

// slp_dcdc_pd_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;                   // [0]
        uint32_t slp_vddio1v8_pd_en : 1;      // [1]
        uint32_t slp_dcdcwpa_pd_en : 1;       // [2]
        uint32_t slp_dcdccore_drop_en : 1;    // [3]
        uint32_t slp_dcdccore_pd_en : 1;      // [4]
        uint32_t __5_5 : 1;                   // [5]
        uint32_t slp_dcdccore_pu_rstn_th : 6; // [11:6]
        uint32_t slp_dcdccore_pd_rstn_th : 4; // [15:12]
        uint32_t __31_16 : 16;                // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_SLP_DCDC_PD_CTRL_T;

// slp_ldo_pd_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t slp_ldosim1_pd_en : 1;   // [0]
        uint32_t __1_1 : 1;               // [1]
        uint32_t slp_ldocama_pd_en : 1;   // [2]
        uint32_t slp_ldocamd_pd_en : 1;   // [3]
        uint32_t slp_ldocamio_pd_en : 1;  // [4]
        uint32_t slp_ldolcd_pd_en : 1;    // [5]
        uint32_t slp_ldousb33_pd_en : 1;  // [6]
        uint32_t slp_ldoddr12_pd_en : 1;  // [7]
        uint32_t slp_ldovio18_pd_en : 1;  // [8]
        uint32_t slp_ldovdd28_pd_en : 1;  // [9]
        uint32_t slp_ldospimem_pd_en : 1; // [10]
        uint32_t slp_ldodcxo_pd_en : 1;   // [11]
        uint32_t slp_ldommc_pd_en : 1;    // [12]
        uint32_t __13_13 : 1;             // [13]
        uint32_t slp_ldorf15_pd_en : 1;   // [14]
        uint32_t __31_15 : 17;            // [31:15]
    } b;
} REG_RDA2720M_GLOBAL_SLP_LDO_PD_CTRL0_T;

// slp_ldo_pd_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t slp_ldomem_pd_en : 1;  // [0]
        uint32_t slp_ldoana_pd_en : 1;  // [1]
        uint32_t slp_ldosim0_pd_en : 1; // [2]
        uint32_t slp_ldocon_pd_en : 1;  // [3]
        uint32_t slp_ldocp_pd_en : 1;   // [4]
        uint32_t __31_5 : 27;           // [31:5]
    } b;
} REG_RDA2720M_GLOBAL_SLP_LDO_PD_CTRL1_T;

// slp_dcdc_lp_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t slp_dcdcwpa_lp_en : 1;  // [0]
        uint32_t slp_dcdcgen_lp_en : 1;  // [1]
        uint32_t __3_2 : 2;              // [3:2]
        uint32_t slp_dcdccore_lp_en : 1; // [4]
        uint32_t __31_5 : 27;            // [31:5]
    } b;
} REG_RDA2720M_GLOBAL_SLP_DCDC_LP_CTRL_T;

// slp_ldo_lp_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t slp_ldosim1_lp_en : 1;   // [0]
        uint32_t __1_1 : 1;               // [1]
        uint32_t slp_ldocama_lp_en : 1;   // [2]
        uint32_t slp_ldocamd_lp_en : 1;   // [3]
        uint32_t slp_ldocamio_lp_en : 1;  // [4]
        uint32_t slp_ldolcd_lp_en : 1;    // [5]
        uint32_t slp_ldousb33_lp_en : 1;  // [6]
        uint32_t slp_ldoddr12_lp_en : 1;  // [7]
        uint32_t slp_ldovio18_lp_en : 1;  // [8]
        uint32_t slp_ldovdd28_lp_en : 1;  // [9]
        uint32_t slp_ldospimem_lp_en : 1; // [10]
        uint32_t slp_ldodcxo_lp_en : 1;   // [11]
        uint32_t slp_ldommc_lp_en : 1;    // [12]
        uint32_t __13_13 : 1;             // [13]
        uint32_t slp_ldorf15_lp_en : 1;   // [14]
        uint32_t __31_15 : 17;            // [31:15]
    } b;
} REG_RDA2720M_GLOBAL_SLP_LDO_LP_CTRL0_T;

// slp_ldo_lp_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t slp_ldomem_lp_en : 1;  // [0]
        uint32_t slp_ldoana_lp_en : 1;  // [1]
        uint32_t slp_ldosim0_lp_en : 1; // [2]
        uint32_t slp_ldocon_lp_en : 1;  // [3]
        uint32_t __31_4 : 28;           // [31:4]
    } b;
} REG_RDA2720M_GLOBAL_SLP_LDO_LP_CTRL1_T;

// dcdc_core_slp_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dcdc_core_slp_step_en : 1;    // [0]
        uint32_t __2_1 : 2;                    // [2:1]
        uint32_t dcdc_core_slp_step_vol : 5;   // [7:3]
        uint32_t dcdc_core_slp_step_num : 4;   // [11:8]
        uint32_t dcdc_core_slp_step_delay : 2; // [13:12]
        uint32_t __31_14 : 18;                 // [31:14]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_CORE_SLP_CTRL0_T;

// dcdc_core_slp_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dcdc_core_vosel_ds_sw : 9; // [8:0]
        uint32_t __31_9 : 23;               // [31:9]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_CORE_SLP_CTRL1_T;

// dcdc_xtl_en0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dcdc_wpa_ext_xtl3_en : 1;  // [0]
        uint32_t dcdc_wpa_ext_xtl2_en : 1;  // [1]
        uint32_t dcdc_wpa_ext_xtl1_en : 1;  // [2]
        uint32_t dcdc_wpa_ext_xtl0_en : 1;  // [3]
        uint32_t dcdc_gen_ext_xtl3_en : 1;  // [4]
        uint32_t dcdc_gen_ext_xtl2_en : 1;  // [5]
        uint32_t dcdc_gen_ext_xtl1_en : 1;  // [6]
        uint32_t dcdc_gen_ext_xtl0_en : 1;  // [7]
        uint32_t __11_8 : 4;                // [11:8]
        uint32_t dcdc_core_ext_xtl3_en : 1; // [12]
        uint32_t dcdc_core_ext_xtl2_en : 1; // [13]
        uint32_t dcdc_core_ext_xtl1_en : 1; // [14]
        uint32_t dcdc_core_ext_xtl0_en : 1; // [15]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_XTL_EN0_T;

// ldo_xtl_en0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vdd28_ext_xtl3_en : 1; // [0]
        uint32_t ldo_vdd28_ext_xtl2_en : 1; // [1]
        uint32_t ldo_vdd28_ext_xtl1_en : 1; // [2]
        uint32_t ldo_vdd28_ext_xtl0_en : 1; // [3]
        uint32_t __11_4 : 8;                // [11:4]
        uint32_t ldo_dcxo_ext_xtl3_en : 1;  // [12]
        uint32_t ldo_dcxo_ext_xtl2_en : 1;  // [13]
        uint32_t ldo_dcxo_ext_xtl1_en : 1;  // [14]
        uint32_t ldo_dcxo_ext_xtl0_en : 1;  // [15]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN0_T;

// ldo_xtl_en1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __11_0 : 12;              // [11:0]
        uint32_t ldo_rf15_ext_xtl3_en : 1; // [12]
        uint32_t ldo_rf15_ext_xtl2_en : 1; // [13]
        uint32_t ldo_rf15_ext_xtl1_en : 1; // [14]
        uint32_t ldo_rf15_ext_xtl0_en : 1; // [15]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN1_T;

// ldo_xtl_en2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_sim1_ext_xtl3_en : 1; // [0]
        uint32_t ldo_sim1_ext_xtl2_en : 1; // [1]
        uint32_t ldo_sim1_ext_xtl1_en : 1; // [2]
        uint32_t ldo_sim1_ext_xtl0_en : 1; // [3]
        uint32_t __11_4 : 8;               // [11:4]
        uint32_t ldo_sim0_ext_xtl3_en : 1; // [12]
        uint32_t ldo_sim0_ext_xtl2_en : 1; // [13]
        uint32_t ldo_sim0_ext_xtl1_en : 1; // [14]
        uint32_t ldo_sim0_ext_xtl0_en : 1; // [15]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN2_T;

// ldo_xtl_en3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_mem_ext_xtl3_en : 1; // [0]
        uint32_t ldo_mem_ext_xtl2_en : 1; // [1]
        uint32_t ldo_mem_ext_xtl1_en : 1; // [2]
        uint32_t ldo_mem_ext_xtl0_en : 1; // [3]
        uint32_t __31_4 : 28;             // [31:4]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN3_T;

// ldo_xtl_en4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_camio_ext_xtl3_en : 1; // [0]
        uint32_t ldo_camio_ext_xtl2_en : 1; // [1]
        uint32_t ldo_camio_ext_xtl1_en : 1; // [2]
        uint32_t ldo_camio_ext_xtl0_en : 1; // [3]
        uint32_t __11_4 : 8;                // [11:4]
        uint32_t ldo_lcd_ext_xtl3_en : 1;   // [12]
        uint32_t ldo_lcd_ext_xtl2_en : 1;   // [13]
        uint32_t ldo_lcd_ext_xtl1_en : 1;   // [14]
        uint32_t ldo_lcd_ext_xtl0_en : 1;   // [15]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN4_T;

// ldo_xtl_en5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_camd_ext_xtl3_en : 1; // [0]
        uint32_t ldo_camd_ext_xtl2_en : 1; // [1]
        uint32_t ldo_camd_ext_xtl1_en : 1; // [2]
        uint32_t ldo_camd_ext_xtl0_en : 1; // [3]
        uint32_t __11_4 : 8;               // [11:4]
        uint32_t ldo_cama_ext_xtl3_en : 1; // [12]
        uint32_t ldo_cama_ext_xtl2_en : 1; // [13]
        uint32_t ldo_cama_ext_xtl1_en : 1; // [14]
        uint32_t ldo_cama_ext_xtl0_en : 1; // [15]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN5_T;

// ldo_xtl_en6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vio18_ext_xtl3_en : 1; // [0]
        uint32_t ldo_vio18_ext_xtl2_en : 1; // [1]
        uint32_t ldo_vio18_ext_xtl1_en : 1; // [2]
        uint32_t ldo_vio18_ext_xtl0_en : 1; // [3]
        uint32_t __11_4 : 8;                // [11:4]
        uint32_t ldo_ddr12_ext_xtl3_en : 1; // [12]
        uint32_t ldo_ddr12_ext_xtl2_en : 1; // [13]
        uint32_t ldo_ddr12_ext_xtl1_en : 1; // [14]
        uint32_t ldo_ddr12_ext_xtl0_en : 1; // [15]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN6_T;

// ldo_xtl_en7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_usb33_ext_xtl3_en : 1; // [0]
        uint32_t ldo_usb33_ext_xtl2_en : 1; // [1]
        uint32_t ldo_usb33_ext_xtl1_en : 1; // [2]
        uint32_t ldo_usb33_ext_xtl0_en : 1; // [3]
        uint32_t __11_4 : 8;                // [11:4]
        uint32_t ldo_mmc_ext_xtl3_en : 1;   // [12]
        uint32_t ldo_mmc_ext_xtl2_en : 1;   // [13]
        uint32_t ldo_mmc_ext_xtl1_en : 1;   // [14]
        uint32_t ldo_mmc_ext_xtl0_en : 1;   // [15]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN7_T;

// ldo_xtl_en8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vibr_ext_xtl3_en : 1;  // [0]
        uint32_t ldo_vibr_ext_xtl2_en : 1;  // [1]
        uint32_t ldo_vibr_ext_xtl1_en : 1;  // [2]
        uint32_t ldo_vibr_ext_xtl0_en : 1;  // [3]
        uint32_t __11_4 : 8;                // [11:4]
        uint32_t ldo_kpled_ext_xtl3_en : 1; // [12]
        uint32_t ldo_kpled_ext_xtl2_en : 1; // [13]
        uint32_t ldo_kpled_ext_xtl1_en : 1; // [14]
        uint32_t ldo_kpled_ext_xtl0_en : 1; // [15]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN8_T;

// ldo_xtl_en9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_ana_ext_xtl3_en : 1; // [0]
        uint32_t ldo_ana_ext_xtl2_en : 1; // [1]
        uint32_t ldo_ana_ext_xtl1_en : 1; // [2]
        uint32_t ldo_ana_ext_xtl0_en : 1; // [3]
        uint32_t __11_4 : 8;              // [11:4]
        uint32_t ldo_con_ext_xtl3_en : 1; // [12]
        uint32_t ldo_con_ext_xtl2_en : 1; // [13]
        uint32_t ldo_con_ext_xtl1_en : 1; // [14]
        uint32_t ldo_con_ext_xtl0_en : 1; // [15]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN9_T;

// ldo_xtl_en10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_spimem_ext_xtl3_en : 1; // [0]
        uint32_t ldo_spimem_ext_xtl2_en : 1; // [1]
        uint32_t ldo_spimem_ext_xtl1_en : 1; // [2]
        uint32_t ldo_spimem_ext_xtl0_en : 1; // [3]
        uint32_t __11_4 : 8;                 // [11:4]
        uint32_t ldo_cp_ext_xtl3_en : 1;     // [12]
        uint32_t ldo_cp_ext_xtl2_en : 1;     // [13]
        uint32_t ldo_cp_ext_xtl1_en : 1;     // [14]
        uint32_t ldo_cp_ext_xtl0_en : 1;     // [15]
        uint32_t __31_16 : 16;               // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_LDO_XTL_EN10_T;

// bg_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bg_chop_en : 1; // [0]
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_BG_CTRL_T;

// dcdc_vlg_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dcdc_core_nor_sw_sel : 1;    // [0]
        uint32_t dcdc_core_slp_sw_sel : 1;    // [1]
        uint32_t dcdc_core_votrim_sw_sel : 1; // [2]
        uint32_t dcdc_gen_sw_sel : 1;         // [3]
        uint32_t dcdc_wpa_sw_sel : 1;         // [4]
        uint32_t dcdc_wpa_votrim_sw_sel : 1;  // [5]
        uint32_t __31_6 : 26;                 // [31:6]
    } b;
} REG_RDA2720M_GLOBAL_DCDC_VLG_SEL_T;

// ldo_vlg_sel0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_trim_b_sw_sel : 1;  // [0]
        uint32_t ldo_trim_a_sw_sel : 1;  // [1]
        uint32_t ldo_dcxo_sw_sel : 1;    // [2]
        uint32_t ldo_rtc_cal_sw_sel : 1; // [3]
        uint32_t ldo_vibr_sw_sel : 1;    // [4]
        uint32_t ldo_kpled_sw_sel : 1;   // [5]
        uint32_t __31_6 : 26;            // [31:6]
    } b;
} REG_RDA2720M_GLOBAL_LDO_VLG_SEL0_T;

// clk32kless_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rc_32k_en : 1;               // [0]
        uint32_t rc_32k_sel : 1;              // [1]
        uint32_t __3_2 : 2;                   // [3:2]
        uint32_t rtc_mode : 1;                // [4], read only
        uint32_t __5_5 : 1;                   // [5]
        uint32_t ldo_dcxo_lp_en_rtcclr : 1;   // [6]
        uint32_t ldo_dcxo_lp_en_rtcset : 1;   // [7]
        uint32_t __9_8 : 2;                   // [9:8]
        uint32_t rc_mode_wr_ack_flag_clr : 1; // [10], write clear
        uint32_t __13_11 : 3;                 // [13:11]
        uint32_t rc_mode_wr_ack_flag : 1;     // [14], read only
        uint32_t __31_15 : 17;                // [31:15]
    } b;
} REG_RDA2720M_GLOBAL_CLK32KLESS_CTRL0_T;

// clk32kless_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rc_mode : 16; // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_CLK32KLESS_CTRL1_T;

// xtl_wait_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xtl_wait : 8;            // [7:0]
        uint32_t xtl_en : 1;              // [8]
        uint32_t slp_xtlbuf_pd_en : 1;    // [9]
        uint32_t ext_xtl0_for_26m_en : 1; // [10]
        uint32_t ext_xtl1_for_26m_en : 1; // [11]
        uint32_t ext_xtl2_for_26m_en : 1; // [12]
        uint32_t ext_xtl3_for_26m_en : 1; // [13]
        uint32_t __31_14 : 18;            // [31:14]
    } b;
} REG_RDA2720M_GLOBAL_XTL_WAIT_CTRL0_T;

// rgb_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rgb_pd_sw : 1;     // [0]
        uint32_t rgb_pd_hw_en : 1;  // [1]
        uint32_t slp_rgb_pd_en : 1; // [2]
        uint32_t __31_3 : 29;       // [31:3]
    } b;
} REG_RDA2720M_GLOBAL_RGB_CTRL_T;

// ib_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ib_rex_en : 1;      // [0]
        uint32_t ib_trim_em_sel : 1; // [1]
        uint32_t ib_trim : 7;        // [8:2]
        uint32_t batdet_cur_i : 3;   // [11:9]
        uint32_t __12_12 : 1;        // [12]
        uint32_t batdet_cur_en : 1;  // [13]
        uint32_t __31_14 : 18;       // [31:14]
    } b;
} REG_RDA2720M_GLOBAL_IB_CTRL_T;

// flash_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t flash_v_sw : 4;      // [3:0]
        uint32_t flash_v_hw_step : 2; // [5:4]
        uint32_t flash_v_hw_en : 1;   // [6]
        uint32_t __14_7 : 8;          // [14:7]
        uint32_t flash_pon : 1;       // [15]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_FLASH_CTRL_T;

// kpled_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_kpled_cl_adj : 1;   // [0]
        uint32_t ldo_kpled_shpt_adj : 1; // [1]
        uint32_t ldo_kpled_stb : 2;      // [3:2]
        uint32_t ldo_kpled_cap_sel : 1;  // [4]
        uint32_t __8_5 : 4;              // [8:5]
        uint32_t slp_ldokpled_pd_en : 1; // [9]
        uint32_t kpled_pulldown_en : 1;  // [10]
        uint32_t kpled_pd : 1;           // [11]
        uint32_t kpled_v : 4;            // [15:12]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_KPLED_CTRL0_T;

// kpled_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_kpled_shpt_pd : 1; // [0]
        uint32_t __1_1 : 1;             // [1]
        uint32_t ldo_kpled_reftrim : 5; // [6:2]
        uint32_t ldo_kpled_v : 3;       // [9:7]
        uint32_t __14_10 : 5;           // [14:10]
        uint32_t ldo_kpled_pd : 1;      // [15]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_KPLED_CTRL1_T;

// vibr_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vibr_v : 3;        // [2:0]
        uint32_t __7_3 : 5;             // [7:3]
        uint32_t ldo_vibr_pd : 1;       // [8]
        uint32_t slp_ldovibr_pd_en : 1; // [9]
        uint32_t ldo_vibr_shpt_pd : 1;  // [10]
        uint32_t ldo_vibr_cl_adj : 1;   // [11]
        uint32_t ldo_vibr_shpt_adj : 1; // [12]
        uint32_t ldo_vibr_stb : 2;      // [14:13]
        uint32_t ldo_vibr_cap_sel : 1;  // [15]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_VIBR_CTRL0_T;

// vibr_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vibr_eadbias_en : 1; // [0]
        uint32_t ldo_vibr_reftrim : 5;    // [5:1]
        uint32_t __31_6 : 26;             // [31:6]
    } b;
} REG_RDA2720M_GLOBAL_VIBR_CTRL1_T;

// audio_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_aud_if_6p5m_rx_inv_en : 1; // [0]
        uint32_t clk_aud_if_6p5m_tx_inv_en : 1; // [1]
        uint32_t clk_aud_if_rx_inv_en : 1;      // [2]
        uint32_t clk_aud_if_tx_inv_en : 1;      // [3]
        uint32_t __31_4 : 28;                   // [31:4]
    } b;
} REG_RDA2720M_GLOBAL_AUDIO_CTRL0_T;

// chgr_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chgr_pd : 1;    // [0]
        uint32_t chgr_iterm : 2; // [2:1]
        uint32_t chgr_end_v : 2; // [4:3]
        uint32_t chgr_cv_v : 6;  // [10:5]
        uint32_t __11_11 : 1;    // [11]
        uint32_t chgr_cc_en : 1; // [12]
        uint32_t chgr_dpm : 2;   // [14:13]
        uint32_t __31_15 : 17;   // [31:15]
    } b;
} REG_RDA2720M_GLOBAL_CHGR_CTRL0_T;

// chgr_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vchg_ovp_v : 2;          // [1:0]
        uint32_t __9_2 : 8;               // [9:2]
        uint32_t chgr_cc_i : 4;           // [13:10]
        uint32_t __14_14 : 1;             // [14]
        uint32_t chgr_expower_device : 1; // [15]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_CHGR_CTRL1_T;

// chgr_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vchg_ovi : 1;       // [0], read only
        uint32_t dcp_switch_en : 1;  // [1]
        uint32_t chgr_int : 1;       // [2], read only
        uint32_t chgr_on : 1;        // [3], read only
        uint32_t chgr_cv_status : 1; // [4], read only
        uint32_t cdp_int : 1;        // [5], read only
        uint32_t dcp_int : 1;        // [6], read only
        uint32_t sdp_int : 1;        // [7], read only
        uint32_t chg_det : 1;        // [8], read only
        uint32_t dcp_det : 1;        // [9], read only
        uint32_t dp_low : 1;         // [10], read only
        uint32_t chg_det_done : 1;   // [11], read only
        uint32_t non_dcp_int : 1;    // [12], read only
        uint32_t chgr_int_en : 1;    // [13]
        uint32_t __31_14 : 18;       // [31:14]
    } b;
} REG_RDA2720M_GLOBAL_CHGR_STATUS_T;

// chgr_det_fgu_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dp_dm_bc_enb : 1;     // [0]
        uint32_t dp_dm_aux_en : 1;     // [1]
        uint32_t sd_dcoffset_a_en : 1; // [2]
        uint32_t sd_dcoffset_b_en : 1; // [3]
        uint32_t sd_chop_en : 1;       // [4]
        uint32_t sd_adc0_rc_pd : 1;    // [5]
        uint32_t sd_clk_p : 2;         // [7:6]
        uint32_t sd_chop_cap_en : 1;   // [8]
        uint32_t chg_int_delay : 3;    // [11:9]
        uint32_t ldo_fgu_pd : 1;       // [12]
        uint32_t fgua_soft_rst : 1;    // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_RDA2720M_GLOBAL_CHGR_DET_FGU_CTRL_T;

// ovlo_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ovlo_t : 2;       // [1:0]
        uint32_t ovlo_v : 2;       // [3:2]
        uint32_t __8_4 : 5;        // [8:4]
        uint32_t ovlo_en : 1;      // [9]
        uint32_t vbat_crash_v : 2; // [11:10]
        uint32_t __31_12 : 20;     // [31:12]
    } b;
} REG_RDA2720M_GLOBAL_OVLO_CTRL_T;

// mixed_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t int_debug_en : 1; // [0]
        uint32_t all_int_deb : 1;  // [1]
        uint32_t gpi_debug_en : 1; // [2]
        uint32_t all_gpi_deb : 1;  // [3]
        uint32_t vbat_ok : 1;      // [4], read only
        uint32_t __7_5 : 3;        // [7:5]
        uint32_t batdet_ok : 1;    // [8], read only
        uint32_t vpp_5v_sel : 1;   // [9]
        uint32_t baton_t : 2;      // [11:10]
        uint32_t xosc32k_ctl : 1;  // [12]
        uint32_t __31_13 : 19;     // [31:13]
    } b;
} REG_RDA2720M_GLOBAL_MIXED_CTRL_T;

// por_rst_monitor
typedef union {
    uint32_t v;
    struct
    {
        uint32_t por_rst_monitor : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_POR_RST_MONITOR_T;

// wdg_rst_monitor
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_rst_monitor : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_WDG_RST_MONITOR_T;

// por_pin_rst_monitor
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pro_pin_rst_monitor : 16; // [15:0]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_POR_PIN_RST_MONITOR_T;

// por_src_flag
typedef union {
    uint32_t v;
    struct
    {
        uint32_t por_src_flag : 14;        // [13:0], read only
        uint32_t reg_soft_rst_flg_clr : 1; // [14], write clear
        uint32_t por_sw_force_on : 1;      // [15]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_POR_SRC_FLAG_T;

// por_off_flag
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;                    // [1:0]
        uint32_t otp_chip_pd_flag_clr : 1;     // [2], write clear
        uint32_t otp_chip_pd_flag : 1;         // [3], read only
        uint32_t hw_chip_pd_flag_clr : 1;      // [4], write clear
        uint32_t hw_chip_pd_flag : 1;          // [5], read only
        uint32_t sw_chip_pd_flag_clr : 1;      // [6], write clear
        uint32_t sw_chip_pd_flag : 1;          // [7], read only
        uint32_t hard_7s_chip_pd_flag_clr : 1; // [8], write clear
        uint32_t hard_7s_chip_pd_flag : 1;     // [9], read only
        uint32_t uvlo_chip_pd_flag_clr : 1;    // [10], write clear
        uint32_t uvlo_chip_pd_flag : 1;        // [11], read only
        uint32_t por_chip_pd_flag_clr : 1;     // [12], write clear
        uint32_t por_chip_pd_flag : 1;         // [13], read only
        uint32_t __31_14 : 18;                 // [31:14]
    } b;
} REG_RDA2720M_GLOBAL_POR_OFF_FLAG_T;

// por_7s_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pbint_7s_rst_mode : 1;      // [0]
        uint32_t pbint_7s_rst_disable : 1;   // [1]
        uint32_t pbint_7s_auto_on_en : 1;    // [2]
        uint32_t ext_rstn_mode : 1;          // [3]
        uint32_t pbint_7s_rst_threshold : 4; // [7:4]
        uint32_t pbint_7s_rst_swmode : 1;    // [8]
        uint32_t key2_7s_rst_en : 1;         // [9]
        uint32_t __10_10 : 1;                // [10]
        uint32_t pbint_flag_clr : 1;         // [11]
        uint32_t pbint2_flag_clr : 1;        // [12]
        uint32_t chgr_int_flag_clr : 1;      // [13]
        uint32_t ext_rstn_flag_clr : 1;      // [14]
        uint32_t pbint_7s_flag_clr : 1;      // [15]
        uint32_t __31_16 : 16;               // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_POR_7S_CTRL_T;

// hwrst_rtc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hwrst_rtc_reg_set : 8; // [7:0]
        uint32_t hwrst_rtc_reg_sts : 8; // [15:8], read only
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_HWRST_RTC_T;

// arch_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t arch_en : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_RDA2720M_GLOBAL_ARCH_EN_T;

// mcu_wr_prot_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mcu_wr_prot_value : 15; // [14:0]
        uint32_t mcu_wr_prot : 1;        // [15], read only
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_MCU_WR_PROT_VALUE_T;

// pwr_wr_prot_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pwr_wr_prot_value : 15; // [14:0]
        uint32_t pwr_wr_prot : 1;        // [15], read only
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_PWR_WR_PROT_VALUE_T;

// smpl_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t smpl_mode : 16; // [15:0]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_SMPL_CTRL0_T;

// smpl_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t smpl_en : 1;                   // [0], read only
        uint32_t __10_1 : 10;                   // [10:1]
        uint32_t smpl_pwr_on_set : 1;           // [11], read only
        uint32_t smpl_mode_wr_ack_flag_clr : 1; // [12], write clear
        uint32_t smpl_pwr_on_flag_clr : 1;      // [13], write clear
        uint32_t smpl_mode_wr_ack_flag : 1;     // [14], read only
        uint32_t smpl_pwr_on_flag : 1;          // [15], read only
        uint32_t __31_16 : 16;                  // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_SMPL_CTRL1_T;

// rtc_rst0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_clk_flag_set : 16; // [15:0]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_RTC_RST0_T;

// rtc_rst1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_clk_flag_clr : 16; // [15:0]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_RTC_RST1_T;

// rtc_rst2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_clk_flag_rtc : 16; // [15:0], read only
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_RTC_RST2_T;

// rtc_clk_stop
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_clk_stop_threshold : 7; // [6:0]
        uint32_t rtc_clk_stop_flag : 1;      // [7], read only
        uint32_t __31_8 : 24;                // [31:8]
    } b;
} REG_RDA2720M_GLOBAL_RTC_CLK_STOP_T;

// vbat_drop_cnt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vbat_drop_cnt : 12; // [11:0], read only
        uint32_t __31_12 : 20;       // [31:12]
    } b;
} REG_RDA2720M_GLOBAL_VBAT_DROP_CNT_T;

// swrst_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sw_rst_pd_threshold : 4; // [3:0]
        uint32_t reg_rst_en : 1;          // [4]
        uint32_t __6_5 : 2;               // [6:5]
        uint32_t wdg_rst_pd_en : 1;       // [7]
        uint32_t reg_rst_pd_en : 1;       // [8]
        uint32_t pb_7s_rst_pd_en : 1;     // [9]
        uint32_t ext_rstn_pd_en : 1;      // [10]
        uint32_t __31_11 : 21;            // [31:11]
    } b;
} REG_RDA2720M_GLOBAL_SWRST_CTRL0_T;

// swrst_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sw_rst_vio18_pd_en : 1;    // [0]
        uint32_t sw_rst_ddr12_pd_en : 1;    // [1]
        uint32_t sw_rst_mmc_pd_en : 1;      // [2]
        uint32_t sw_rst_usb33_pd_en : 1;    // [3]
        uint32_t sw_rst_rf15_pd_en : 1;     // [4]
        uint32_t sw_rst_ana_pd_en : 1;      // [5]
        uint32_t sw_rst_vdd28_pd_en : 1;    // [6]
        uint32_t sw_rst_dcxo_pd_en : 1;     // [7]
        uint32_t sw_rst_mem_pd_en : 1;      // [8]
        uint32_t sw_rst_dcdccore_pd_en : 1; // [9]
        uint32_t sw_rst_dcdcgen_pd_en : 1;  // [10]
        uint32_t __31_11 : 21;              // [31:11]
    } b;
} REG_RDA2720M_GLOBAL_SWRST_CTRL1_T;

// otp_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t otp_en : 1;  // [0]
        uint32_t otp_op : 2;  // [2:1]
        uint32_t __31_3 : 29; // [31:3]
    } b;
} REG_RDA2720M_GLOBAL_OTP_CTRL_T;

// free_timer_low
typedef union {
    uint32_t v;
    struct
    {
        uint32_t timer_low : 16; // [15:0], read only
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_FREE_TIMER_LOW_T;

// free_timer_high
typedef union {
    uint32_t v;
    struct
    {
        uint32_t timer_high : 16; // [15:0], read only
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_RDA2720M_GLOBAL_FREE_TIMER_HIGH_T;

// vol_tune_ctrl_core
typedef union {
    uint32_t v;
    struct
    {
        uint32_t core_vol_tune_en : 1;    // [0]
        uint32_t core_vol_tune_flag : 1;  // [1], read only
        uint32_t core_vol_tune_start : 1; // [2], write clear
        uint32_t core_step_vol : 5;       // [7:3]
        uint32_t core_step_num : 4;       // [11:8]
        uint32_t core_step_delay : 2;     // [13:12]
        uint32_t core_clk_sel : 1;        // [14]
        uint32_t __31_15 : 17;            // [31:15]
    } b;
} REG_RDA2720M_GLOBAL_VOL_TUNE_CTRL_CORE_T;

// chip_id_low
#define RDA2720M_GLOBAL_CHIP_IP_LOW(n) (((n)&0xffff) << 0)

// chip_id_high
#define RDA2720M_GLOBAL_CHIP_ID_HIGH(n) (((n)&0xffff) << 0)

// module_en0
#define RDA2720M_GLOBAL_CAL_EN (1 << 0)
#define RDA2720M_GLOBAL_RTC_EN (1 << 1)
#define RDA2720M_GLOBAL_WDG_EN (1 << 2)
#define RDA2720M_GLOBAL_EIC_EN (1 << 3)
#define RDA2720M_GLOBAL_AUD_EN (1 << 4)
#define RDA2720M_GLOBAL_ADC_EN (1 << 5)
#define RDA2720M_GLOBAL_EFS_EN (1 << 6)
#define RDA2720M_GLOBAL_FGU_EN (1 << 7)
#define RDA2720M_GLOBAL_PINREG_EN (1 << 8)
#define RDA2720M_GLOBAL_BLTC_EN (1 << 9)
#define RDA2720M_GLOBAL_PSM_EN (1 << 10)
#define RDA2720M_GLOBAL_TMR_EN (1 << 12)

// arm_clk_en0
#define RDA2720M_GLOBAL_CLK_AUD_IF_EN (1 << 0)
#define RDA2720M_GLOBAL_CLK_AUD_IF_6P5M_EN (1 << 1)
#define RDA2720M_GLOBAL_CLK_CAL_EN (1 << 2)
#define RDA2720M_GLOBAL_CLK_CAL_SRC_SEL(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_CLK_AUXADC_EN (1 << 5)
#define RDA2720M_GLOBAL_CLK_AUXAD_EN (1 << 6)

// rtc_clk_en0
#define RDA2720M_GLOBAL_RTC_ARCH_EN (1 << 0)
#define RDA2720M_GLOBAL_RTC_RTC_EN (1 << 1)
#define RDA2720M_GLOBAL_RTC_WDG_EN (1 << 2)
#define RDA2720M_GLOBAL_RTC_EIC_EN (1 << 3)
#define RDA2720M_GLOBAL_RTC_FGU_EN (1 << 6)
#define RDA2720M_GLOBAL_RTC_BLTC_EN (1 << 7)
#define RDA2720M_GLOBAL_RTC_EFS_EN (1 << 11)
#define RDA2720M_GLOBAL_RTC_FLASH_EN (1 << 12)
#define RDA2720M_GLOBAL_RTC_TMR_EN (1 << 13)

// soft_rst0
#define RDA2720M_GLOBAL_CAL_SOFT_RST (1 << 0)
#define RDA2720M_GLOBAL_RTC_SOFT_RST (1 << 1)
#define RDA2720M_GLOBAL_WDG_SOFT_RST (1 << 2)
#define RDA2720M_GLOBAL_EIC_SOFT_RST (1 << 3)
#define RDA2720M_GLOBAL_FGU_SOFT_RST (1 << 4)
#define RDA2720M_GLOBAL_PSM_SOFT_RST (1 << 5)
#define RDA2720M_GLOBAL_ADC_SOFT_RST (1 << 6)
#define RDA2720M_GLOBAL_EFS_SOFT_RST (1 << 7)
#define RDA2720M_GLOBAL_AUD_IF_SOFT_RST (1 << 8)
#define RDA2720M_GLOBAL_BLTC_SOFT_RST (1 << 9)
#define RDA2720M_GLOBAL_AUDTX_SOFT_RST (1 << 12)
#define RDA2720M_GLOBAL_AUDRX_SOFT_RST (1 << 13)

// soft_rst1
#define RDA2720M_GLOBAL_TMR_SOFT_RST (1 << 1)

// power_pd_sw
#define RDA2720M_GLOBAL_BG_PD (1 << 0)
#define RDA2720M_GLOBAL_LDO_VDD28_PD (1 << 1)
#define RDA2720M_GLOBAL_LDO_ANA_PD (1 << 2)
#define RDA2720M_GLOBAL_LDO_MEM_PD (1 << 3)
#define RDA2720M_GLOBAL_OSC3M_EN (1 << 4)
#define RDA2720M_GLOBAL_DCDC_CORE_PD (1 << 5)
#define RDA2720M_GLOBAL_DCDC_GEN_PD (1 << 7)
#define RDA2720M_GLOBAL_LDO_CP_PD (1 << 8)
#define RDA2720M_GLOBAL_LDO_EMM_PD (1 << 9)
#define RDA2720M_GLOBAL_LDO_DCXO_PD (1 << 10)

// power_pd_hw
#define RDA2720M_GLOBAL_PWR_OFF_SEQ_EN (1 << 0)

// soft_rst_hw
#define RDA2720M_GLOBAL_REG_SOFT_RST (1 << 0)

// dcdc_clk_reg0
#define RDA2720M_GLOBAL_OSC3M_FREQ(n) (((n)&0xf) << 0)
#define RDA2720M_GLOBAL_DCDC_GEN_CLK_RST (1 << 4)
#define RDA2720M_GLOBAL_CLKOUT_SEL(n) (((n)&0x7) << 7)
#define RDA2720M_GLOBAL_CLKOUT_UNIPHASE (1 << 10)
#define RDA2720M_GLOBAL_CLK3M_OUT_EN (1 << 11)

// dcdc_core_reg0
#define RDA2720M_GLOBAL_ZX_OFFSET_VCORE(n) (((n)&0x3) << 0)
#define RDA2720M_GLOBAL_SR_LS_VCORE(n) (((n)&0x3) << 2)
#define RDA2720M_GLOBAL_SR_HS_VCORE(n) (((n)&0x3) << 4)
#define RDA2720M_GLOBAL_SLOPE_VCORE(n) (((n)&0x3) << 6)
#define RDA2720M_GLOBAL_RCOMP_VCORE(n) (((n)&0x3) << 8)
#define RDA2720M_GLOBAL_PFM_VH_VCORE(n) (((n)&0x3) << 10)
#define RDA2720M_GLOBAL_CURSES_R_VCORE(n) (((n)&0x3) << 12)
#define RDA2720M_GLOBAL_CURLIMIT_R_VCORE(n) (((n)&0x3) << 14)

// dcdc_core_reg1
#define RDA2720M_GLOBAL_ANRITING_EN_VCORE (1 << 0)
#define RDA2720M_GLOBAL_FORCE_PWM_VCORE (1 << 1)
#define RDA2720M_GLOBAL_ZX_DISABLE_VCORE (1 << 2)
#define RDA2720M_GLOBAL_VOTRIM_VCORE(n) (((n)&0x7) << 3)

// dcdc_core_reg2
#define RDA2720M_GLOBAL_DIV_BASE_VCORE(n) (((n)&0x3f) << 0)
#define RDA2720M_GLOBAL_PHASE_SEL_VCORE(n) (((n)&0x3f) << 6)
#define RDA2720M_GLOBAL_DIV_CLK_VCORE_EN (1 << 12)

// dcdc_core_vol
#define RDA2720M_GLOBAL_VOSEL_VCORE(n) (((n)&0x1ff) << 0)

// dcdc_gen_reg0
#define RDA2720M_GLOBAL_ZX_OFFSET_VGEN(n) (((n)&0x3) << 0)
#define RDA2720M_GLOBAL_SR_LS_VGEN(n) (((n)&0x3) << 2)
#define RDA2720M_GLOBAL_SR_HS_VGEN(n) (((n)&0x3) << 4)
#define RDA2720M_GLOBAL_SLOPE_VGEN(n) (((n)&0x3) << 6)
#define RDA2720M_GLOBAL_RCOMP_VGEN(n) (((n)&0x3) << 8)
#define RDA2720M_GLOBAL_PFM_VH_VGEN(n) (((n)&0x3) << 10)
#define RDA2720M_GLOBAL_CURSES_R_VGEN(n) (((n)&0x3) << 12)
#define RDA2720M_GLOBAL_CURLIMIT_R_VGEN(n) (((n)&0x3) << 14)

// dcdc_gen_reg1
#define RDA2720M_GLOBAL_ANRITING_EN_VGEN (1 << 0)
#define RDA2720M_GLOBAL_FORCE_PWM_VGEN (1 << 1)
#define RDA2720M_GLOBAL_MAXDUTY_SEL_VGEN (1 << 2)
#define RDA2720M_GLOBAL_ZX_DISABLE_VGEN (1 << 3)

// dcdc_gen_reg2
#define RDA2720M_GLOBAL_DIV_BASE_VGEN(n) (((n)&0x3f) << 0)
#define RDA2720M_GLOBAL_PHASE_SEL_VGEN(n) (((n)&0x3f) << 6)
#define RDA2720M_GLOBAL_DIV_CLK_VGEN_EN (1 << 12)

// dcdc_gen_vol
#define RDA2720M_GLOBAL_VOSEL_VGEN(n) (((n)&0xff) << 0)

// dcdc_wpa_reg0
#define RDA2720M_GLOBAL_SR_HS_VPA(n) (((n)&0x3) << 0)
#define RDA2720M_GLOBAL_SAWTOOTH_SLOPE_VPA(n) (((n)&0x3) << 2)
#define RDA2720M_GLOBAL_RCOMP3_VPA(n) (((n)&0x3) << 4)
#define RDA2720M_GLOBAL_RCOMP2_VPA(n) (((n)&0x3) << 6)
#define RDA2720M_GLOBAL_PFM_THRESHOLD_VPA(n) (((n)&0x3) << 8)
#define RDA2720M_GLOBAL_CCOMP3_VPA(n) (((n)&0x3) << 10)
#define RDA2720M_GLOBAL_CURSES_M_VPA(n) (((n)&0x3) << 12)
#define RDA2720M_GLOBAL_CURLIMIT_R_VPA(n) (((n)&0x3) << 14)

// dcdc_wpa_reg1
#define RDA2720M_GLOBAL_MAXDUTY_SEL_VPA (1 << 0)
#define RDA2720M_GLOBAL_DVS_ON_VPA (1 << 1)
#define RDA2720M_GLOBAL_BYPASS_THRESHOLD_VPA(n) (((n)&0x3) << 2)
#define RDA2720M_GLOBAL_BYPASS_FORCEON_VPA (1 << 4)
#define RDA2720M_GLOBAL_BYPASS_DISABLE_VPA (1 << 5)
#define RDA2720M_GLOBAL_APC_RAMP_SEL_VPA (1 << 6)
#define RDA2720M_GLOBAL_APC_ENABLE_VPA (1 << 7)
#define RDA2720M_GLOBAL_ANRITING_EN_VPA (1 << 8)
#define RDA2720M_GLOBAL_ZX_OFFSET_VPA(n) (((n)&0x3) << 9)
#define RDA2720M_GLOBAL_VOTRIM_VPA(n) (((n)&0x7) << 11)
#define RDA2720M_GLOBAL_SR_LS_VPA(n) (((n)&0x3) << 14)

// dcdc_wpa_reg2
#define RDA2720M_GLOBAL_PD_BUCK_VPA (1 << 0)
#define RDA2720M_GLOBAL_SAWTOOTHCAL_RST_VPA (1 << 1)
#define RDA2720M_GLOBAL_ZX_DISABLE_VPA (1 << 2)
#define RDA2720M_GLOBAL_DIV_BASE_VPA(n) (((n)&0x3f) << 3)
#define RDA2720M_GLOBAL_PHASE_SEL_VPA(n) (((n)&0x3f) << 9)
#define RDA2720M_GLOBAL_DIV_CLK_VPA_EN (1 << 15)

// dcdc_wpa_vol
#define RDA2720M_GLOBAL_VOSEL_VPA(n) (((n)&0x7f) << 0)

// dcdc_wpa_dcm_hw
#define RDA2720M_GLOBAL_FORCE_PWM_VPA (1 << 0)

// dcdc_ch_ctrl
#define RDA2720M_GLOBAL_DCDC_AUXTRIM_SEL(n) (((n)&0x7) << 0)

// ldo_ana_reg0
#define RDA2720M_GLOBAL_LDO_ANA_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_ANA_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_ANA_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_ANA_BP (1 << 5)
#define RDA2720M_GLOBAL_LDO_ANA_CAP_SEL (1 << 6)
#define RDA2720M_GLOBAL_LDO_ANA_SHPT_PD (1 << 7)

// ldo_ana_reg1
#define RDA2720M_GLOBAL_LDO_ANA_V(n) (((n)&0x3f) << 0)

// ldo_rf15_reg0
#define RDA2720M_GLOBAL_LDO_RF15_PD (1 << 0)
#define RDA2720M_GLOBAL_LDO_RF15_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_RF15_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_RF15_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_RF15_BP (1 << 5)
#define RDA2720M_GLOBAL_LDO_RF15_CAP_SEL (1 << 6)
#define RDA2720M_GLOBAL_LDO_RF15_SHPT_PD (1 << 7)

// ldo_rf15_reg1
#define RDA2720M_GLOBAL_LDO_RF15_V(n) (((n)&0x3f) << 0)

// ldo_camd_reg0
#define RDA2720M_GLOBAL_LDO_CAMD_PD (1 << 0)
#define RDA2720M_GLOBAL_LDO_CAMD_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_CAMD_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_CAMD_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_CAMD_BP (1 << 5)
#define RDA2720M_GLOBAL_LDO_CAMD_CAP_SEL (1 << 6)
#define RDA2720M_GLOBAL_LDO_CAMD_SHPT_PD (1 << 7)

// ldo_camd_reg1
#define RDA2720M_GLOBAL_LDO_CAMD_V(n) (((n)&0x3f) << 0)

// ldo_con_reg0
#define RDA2720M_GLOBAL_LDO_CON_PD (1 << 0)
#define RDA2720M_GLOBAL_LDO_CON_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_CON_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_CON_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_CON_BP (1 << 5)
#define RDA2720M_GLOBAL_LDO_CON_SHPT_PD (1 << 6)

// ldo_con_reg1
#define RDA2720M_GLOBAL_LDO_CON_V(n) (((n)&0x3f) << 0)

// ldo_mem_reg0
#define RDA2720M_GLOBAL_LDO_MEM_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_MEM_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_MEM_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_MEM_BP (1 << 5)
#define RDA2720M_GLOBAL_LDO_MEM_CAP_SEL (1 << 6)
#define RDA2720M_GLOBAL_LDO_MEM_SHPT_PD (1 << 7)

// ldo_mem_reg1
#define RDA2720M_GLOBAL_LDO_MEM_V(n) (((n)&0x3f) << 0)

// ldo_sim0_reg0
#define RDA2720M_GLOBAL_LDO_SIM0_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_SIM0_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_SIM0_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_SIM0_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_SIM0_SHPT_PD (1 << 6)

// ldo_sim0_pd_reg
#define RDA2720M_GLOBAL_LDO_SIM0_PD (1 << 0)

// ldo_sim0_reg1
#define RDA2720M_GLOBAL_LDO_SIM0_V(n) (((n)&0x7f) << 0)

// ldo_sim1_reg0
#define RDA2720M_GLOBAL_LDO_SIM1_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_SIM1_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_SIM1_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_SIM1_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_SIM1_SHPT_PD (1 << 6)

// ldo_sim1_pd_reg
#define RDA2720M_GLOBAL_LDO_SIM1_PD (1 << 0)

// ldo_sim1_reg1
#define RDA2720M_GLOBAL_LDO_SIM1_V(n) (((n)&0x7f) << 0)

// ldo_cama_reg0
#define RDA2720M_GLOBAL_LDO_CAMA_PD (1 << 0)
#define RDA2720M_GLOBAL_LDO_CAMA_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_CAMA_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_CAMA_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_CAMA_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_CAMA_SHPT_PD (1 << 6)

// ldo_cama_reg1
#define RDA2720M_GLOBAL_LDO_CAMA_V(n) (((n)&0x7f) << 0)

// ldo_lcd_reg0
#define RDA2720M_GLOBAL_LDO_LCD_PD (1 << 0)
#define RDA2720M_GLOBAL_LDO_LCD_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_LCD_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_LCD_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_LCD_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_LCD_SHPT_PD (1 << 6)

// ldo_lcd_reg1
#define RDA2720M_GLOBAL_LDO_LCD_V(n) (((n)&0x7f) << 0)

// ldo_mmc_reg0
#define RDA2720M_GLOBAL_LDO_MMC_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_MMC_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_MMC_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_MMC_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_MMC_SHPT_PD (1 << 6)

// ldo_mmc_pd_reg
#define RDA2720M_GLOBAL_LDO_MMC_PD (1 << 0)

// ldo_mmc_reg1
#define RDA2720M_GLOBAL_LDO_MMC_V(n) (((n)&0x7f) << 0)

// ldo_sd_reg0
#define RDA2720M_GLOBAL_LDO_VIO18_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_VIO18_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_VIO18_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_VIO18_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_VIO18_SHPT_PD (1 << 6)

// ldo_sd_pd_reg
#define RDA2720M_GLOBAL_LDO_VIO18_PD (1 << 0)

// ldo_sd_reg1
#define RDA2720M_GLOBAL_LDO_VIO18_V(n) (((n)&0x7f) << 0)

// ldo_ddr12_reg0
#define RDA2720M_GLOBAL_LDO_DDR12_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_DDR12_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_DDR12_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_DDR12_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_DDR12_SHPT_PD (1 << 6)

// ldo_ddr12_pd_reg
#define RDA2720M_GLOBAL_LDO_DDR12_PD (1 << 0)

// ldo_ddr12_reg1
#define RDA2720M_GLOBAL_LDO_DDR12_V(n) (((n)&0x7f) << 0)

// ldo_vdd28_reg0
#define RDA2720M_GLOBAL_LDO_VDD28_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_VDD28_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_VDD28_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_VDD28_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_VDD28_SHPT_PD (1 << 6)

// ldo_vdd28_reg1
#define RDA2720M_GLOBAL_LDO_VDD28_V(n) (((n)&0x7f) << 0)

// ldo_spimem_reg0
#define RDA2720M_GLOBAL_LDO_SPIMEM_PD (1 << 0)
#define RDA2720M_GLOBAL_LDO_SPIMEM_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_SPIMEM_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_SPIMEM_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_SPIMEM_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_SPIMEM_SHPT_PD (1 << 6)

// ldo_spimem_reg1
#define RDA2720M_GLOBAL_LDO_SPIMEM_V(n) (((n)&0x7f) << 0)

// ldo_dcxo_reg0
#define RDA2720M_GLOBAL_LDO_DCXO_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_DCXO_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_DCXO_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_DCXO_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_DCXO_SHPT_PD (1 << 6)
#define RDA2720M_GLOBAL_LDO_DCXO_TRIM(n) (((n)&0x1f) << 7)

// ldo_dcxo_reg1
#define RDA2720M_GLOBAL_LDO_DCXO_V(n) (((n)&0x7f) << 0)

// ldo_usb_reg0
#define RDA2720M_GLOBAL_LDO_USB33_CL_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_USB33_SHPT_ADJ (1 << 2)
#define RDA2720M_GLOBAL_LDO_USB33_STB(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_LDO_USB33_CAP_SEL (1 << 5)
#define RDA2720M_GLOBAL_LDO_USB33_SHPT_PD (1 << 6)

// ldo_usb_pd_reg
#define RDA2720M_GLOBAL_LDO_USB33_PD (1 << 0)

// ldo_usb_reg1
#define RDA2720M_GLOBAL_LDO_USB33_V(n) (((n)&0x7f) << 0)

// ldo_trim_reg
#define RDA2720M_GLOBAL_LDO_TRIM_A(n) (((n)&0x1f) << 0)
#define RDA2720M_GLOBAL_LDO_TRIM_B(n) (((n)&0x1f) << 5)

// ldo_rtc_ctrl
#define RDA2720M_GLOBAL_VBATBK_V(n) (((n)&0x3) << 0)
#define RDA2720M_GLOBAL_LDO_RTC_V(n) (((n)&0x3) << 2)
#define RDA2720M_GLOBAL_LDO_RTC_CAL(n) (((n)&0x1f) << 4)

// ldo_ch_ctrl
#define RDA2720M_GLOBAL_LDO_CAL(n) (((n)&0x7) << 0)
#define RDA2720M_GLOBAL_LDO_CAL1(n) (((n)&0x7) << 5)
#define RDA2720M_GLOBAL_LDO_CAL2(n) (((n)&0x7) << 8)

// slp_ctrl
#define RDA2720M_GLOBAL_SLP_LDO_PD_EN (1 << 0)
#define RDA2720M_GLOBAL_SLP_IO_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_XTL_EN (1 << 2)

// slp_dcdc_pd_ctrl
#define RDA2720M_GLOBAL_SLP_VDDIO1V8_PD_EN (1 << 1)
#define RDA2720M_GLOBAL_SLP_DCDCWPA_PD_EN (1 << 2)
#define RDA2720M_GLOBAL_SLP_DCDCCORE_DROP_EN (1 << 3)
#define RDA2720M_GLOBAL_SLP_DCDCCORE_PD_EN (1 << 4)
#define RDA2720M_GLOBAL_SLP_DCDCCORE_PU_RSTN_TH(n) (((n)&0x3f) << 6)
#define RDA2720M_GLOBAL_SLP_DCDCCORE_PD_RSTN_TH(n) (((n)&0xf) << 12)

// slp_ldo_pd_ctrl0
#define RDA2720M_GLOBAL_SLP_LDOSIM1_PD_EN (1 << 0)
#define RDA2720M_GLOBAL_SLP_LDOCAMA_PD_EN (1 << 2)
#define RDA2720M_GLOBAL_SLP_LDOCAMD_PD_EN (1 << 3)
#define RDA2720M_GLOBAL_SLP_LDOCAMIO_PD_EN (1 << 4)
#define RDA2720M_GLOBAL_SLP_LDOLCD_PD_EN (1 << 5)
#define RDA2720M_GLOBAL_SLP_LDOUSB33_PD_EN (1 << 6)
#define RDA2720M_GLOBAL_SLP_LDODDR12_PD_EN (1 << 7)
#define RDA2720M_GLOBAL_SLP_LDOVIO18_PD_EN (1 << 8)
#define RDA2720M_GLOBAL_SLP_LDOVDD28_PD_EN (1 << 9)
#define RDA2720M_GLOBAL_SLP_LDOSPIMEM_PD_EN (1 << 10)
#define RDA2720M_GLOBAL_SLP_LDODCXO_PD_EN (1 << 11)
#define RDA2720M_GLOBAL_SLP_LDOMMC_PD_EN (1 << 12)
#define RDA2720M_GLOBAL_SLP_LDORF15_PD_EN (1 << 14)

// slp_ldo_pd_ctrl1
#define RDA2720M_GLOBAL_SLP_LDOMEM_PD_EN (1 << 0)
#define RDA2720M_GLOBAL_SLP_LDOANA_PD_EN (1 << 1)
#define RDA2720M_GLOBAL_SLP_LDOSIM0_PD_EN (1 << 2)
#define RDA2720M_GLOBAL_SLP_LDOCON_PD_EN (1 << 3)
#define RDA2720M_GLOBAL_SLP_LDOCP_PD_EN (1 << 4)

// slp_dcdc_lp_ctrl
#define RDA2720M_GLOBAL_SLP_DCDCWPA_LP_EN (1 << 0)
#define RDA2720M_GLOBAL_SLP_DCDCGEN_LP_EN (1 << 1)
#define RDA2720M_GLOBAL_SLP_DCDCCORE_LP_EN (1 << 4)

// slp_ldo_lp_ctrl0
#define RDA2720M_GLOBAL_SLP_LDOSIM1_LP_EN (1 << 0)
#define RDA2720M_GLOBAL_SLP_LDOCAMA_LP_EN (1 << 2)
#define RDA2720M_GLOBAL_SLP_LDOCAMD_LP_EN (1 << 3)
#define RDA2720M_GLOBAL_SLP_LDOCAMIO_LP_EN (1 << 4)
#define RDA2720M_GLOBAL_SLP_LDOLCD_LP_EN (1 << 5)
#define RDA2720M_GLOBAL_SLP_LDOUSB33_LP_EN (1 << 6)
#define RDA2720M_GLOBAL_SLP_LDODDR12_LP_EN (1 << 7)
#define RDA2720M_GLOBAL_SLP_LDOVIO18_LP_EN (1 << 8)
#define RDA2720M_GLOBAL_SLP_LDOVDD28_LP_EN (1 << 9)
#define RDA2720M_GLOBAL_SLP_LDOSPIMEM_LP_EN (1 << 10)
#define RDA2720M_GLOBAL_SLP_LDODCXO_LP_EN (1 << 11)
#define RDA2720M_GLOBAL_SLP_LDOMMC_LP_EN (1 << 12)
#define RDA2720M_GLOBAL_SLP_LDORF15_LP_EN (1 << 14)

// slp_ldo_lp_ctrl1
#define RDA2720M_GLOBAL_SLP_LDOMEM_LP_EN (1 << 0)
#define RDA2720M_GLOBAL_SLP_LDOANA_LP_EN (1 << 1)
#define RDA2720M_GLOBAL_SLP_LDOSIM0_LP_EN (1 << 2)
#define RDA2720M_GLOBAL_SLP_LDOCON_LP_EN (1 << 3)

// dcdc_core_slp_ctrl0
#define RDA2720M_GLOBAL_DCDC_CORE_SLP_STEP_EN (1 << 0)
#define RDA2720M_GLOBAL_DCDC_CORE_SLP_STEP_VOL(n) (((n)&0x1f) << 3)
#define RDA2720M_GLOBAL_DCDC_CORE_SLP_STEP_NUM(n) (((n)&0xf) << 8)
#define RDA2720M_GLOBAL_DCDC_CORE_SLP_STEP_DELAY(n) (((n)&0x3) << 12)

// dcdc_core_slp_ctrl1
#define RDA2720M_GLOBAL_DCDC_CORE_VOSEL_DS_SW(n) (((n)&0x1ff) << 0)

// dcdc_xtl_en0
#define RDA2720M_GLOBAL_DCDC_WPA_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_DCDC_WPA_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_DCDC_WPA_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_DCDC_WPA_EXT_XTL0_EN (1 << 3)
#define RDA2720M_GLOBAL_DCDC_GEN_EXT_XTL3_EN (1 << 4)
#define RDA2720M_GLOBAL_DCDC_GEN_EXT_XTL2_EN (1 << 5)
#define RDA2720M_GLOBAL_DCDC_GEN_EXT_XTL1_EN (1 << 6)
#define RDA2720M_GLOBAL_DCDC_GEN_EXT_XTL0_EN (1 << 7)
#define RDA2720M_GLOBAL_DCDC_CORE_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_DCDC_CORE_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_DCDC_CORE_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_DCDC_CORE_EXT_XTL0_EN (1 << 15)

// ldo_xtl_en0
#define RDA2720M_GLOBAL_LDO_VDD28_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_VDD28_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_VDD28_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_LDO_VDD28_EXT_XTL0_EN (1 << 3)
#define RDA2720M_GLOBAL_LDO_DCXO_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_LDO_DCXO_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_LDO_DCXO_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_LDO_DCXO_EXT_XTL0_EN (1 << 15)

// ldo_xtl_en1
#define RDA2720M_GLOBAL_LDO_RF15_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_LDO_RF15_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_LDO_RF15_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_LDO_RF15_EXT_XTL0_EN (1 << 15)

// ldo_xtl_en2
#define RDA2720M_GLOBAL_LDO_SIM1_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_SIM1_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_SIM1_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_LDO_SIM1_EXT_XTL0_EN (1 << 3)
#define RDA2720M_GLOBAL_LDO_SIM0_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_LDO_SIM0_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_LDO_SIM0_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_LDO_SIM0_EXT_XTL0_EN (1 << 15)

// ldo_xtl_en3
#define RDA2720M_GLOBAL_LDO_MEM_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_MEM_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_MEM_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_LDO_MEM_EXT_XTL0_EN (1 << 3)

// ldo_xtl_en4
#define RDA2720M_GLOBAL_LDO_CAMIO_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_CAMIO_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_CAMIO_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_LDO_CAMIO_EXT_XTL0_EN (1 << 3)
#define RDA2720M_GLOBAL_LDO_LCD_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_LDO_LCD_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_LDO_LCD_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_LDO_LCD_EXT_XTL0_EN (1 << 15)

// ldo_xtl_en5
#define RDA2720M_GLOBAL_LDO_CAMD_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_CAMD_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_CAMD_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_LDO_CAMD_EXT_XTL0_EN (1 << 3)
#define RDA2720M_GLOBAL_LDO_CAMA_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_LDO_CAMA_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_LDO_CAMA_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_LDO_CAMA_EXT_XTL0_EN (1 << 15)

// ldo_xtl_en6
#define RDA2720M_GLOBAL_LDO_VIO18_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_VIO18_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_VIO18_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_LDO_VIO18_EXT_XTL0_EN (1 << 3)
#define RDA2720M_GLOBAL_LDO_DDR12_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_LDO_DDR12_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_LDO_DDR12_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_LDO_DDR12_EXT_XTL0_EN (1 << 15)

// ldo_xtl_en7
#define RDA2720M_GLOBAL_LDO_USB33_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_USB33_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_USB33_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_LDO_USB33_EXT_XTL0_EN (1 << 3)
#define RDA2720M_GLOBAL_LDO_MMC_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_LDO_MMC_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_LDO_MMC_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_LDO_MMC_EXT_XTL0_EN (1 << 15)

// ldo_xtl_en8
#define RDA2720M_GLOBAL_LDO_VIBR_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_VIBR_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_VIBR_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_LDO_VIBR_EXT_XTL0_EN (1 << 3)
#define RDA2720M_GLOBAL_LDO_KPLED_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_LDO_KPLED_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_LDO_KPLED_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_LDO_KPLED_EXT_XTL0_EN (1 << 15)

// ldo_xtl_en9
#define RDA2720M_GLOBAL_LDO_ANA_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_ANA_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_ANA_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_LDO_ANA_EXT_XTL0_EN (1 << 3)
#define RDA2720M_GLOBAL_LDO_CON_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_LDO_CON_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_LDO_CON_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_LDO_CON_EXT_XTL0_EN (1 << 15)

// ldo_xtl_en10
#define RDA2720M_GLOBAL_LDO_SPIMEM_EXT_XTL3_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_SPIMEM_EXT_XTL2_EN (1 << 1)
#define RDA2720M_GLOBAL_LDO_SPIMEM_EXT_XTL1_EN (1 << 2)
#define RDA2720M_GLOBAL_LDO_SPIMEM_EXT_XTL0_EN (1 << 3)
#define RDA2720M_GLOBAL_LDO_CP_EXT_XTL3_EN (1 << 12)
#define RDA2720M_GLOBAL_LDO_CP_EXT_XTL2_EN (1 << 13)
#define RDA2720M_GLOBAL_LDO_CP_EXT_XTL1_EN (1 << 14)
#define RDA2720M_GLOBAL_LDO_CP_EXT_XTL0_EN (1 << 15)

// bg_ctrl
#define RDA2720M_GLOBAL_BG_CHOP_EN (1 << 0)

// dcdc_vlg_sel
#define RDA2720M_GLOBAL_DCDC_CORE_NOR_SW_SEL (1 << 0)
#define RDA2720M_GLOBAL_DCDC_CORE_SLP_SW_SEL (1 << 1)
#define RDA2720M_GLOBAL_DCDC_CORE_VOTRIM_SW_SEL (1 << 2)
#define RDA2720M_GLOBAL_DCDC_GEN_SW_SEL (1 << 3)
#define RDA2720M_GLOBAL_DCDC_WPA_SW_SEL (1 << 4)
#define RDA2720M_GLOBAL_DCDC_WPA_VOTRIM_SW_SEL (1 << 5)

// ldo_vlg_sel0
#define RDA2720M_GLOBAL_LDO_TRIM_B_SW_SEL (1 << 0)
#define RDA2720M_GLOBAL_LDO_TRIM_A_SW_SEL (1 << 1)
#define RDA2720M_GLOBAL_LDO_DCXO_SW_SEL (1 << 2)
#define RDA2720M_GLOBAL_LDO_RTC_CAL_SW_SEL (1 << 3)
#define RDA2720M_GLOBAL_LDO_VIBR_SW_SEL (1 << 4)
#define RDA2720M_GLOBAL_LDO_KPLED_SW_SEL (1 << 5)

// clk32kless_ctrl0
#define RDA2720M_GLOBAL_RC_32K_EN (1 << 0)
#define RDA2720M_GLOBAL_RC_32K_SEL (1 << 1)
#define RDA2720M_GLOBAL_RTC_MODE (1 << 4)
#define RDA2720M_GLOBAL_LDO_DCXO_LP_EN_RTCCLR (1 << 6)
#define RDA2720M_GLOBAL_LDO_DCXO_LP_EN_RTCSET (1 << 7)
#define RDA2720M_GLOBAL_RC_MODE_WR_ACK_FLAG_CLR (1 << 10)
#define RDA2720M_GLOBAL_RC_MODE_WR_ACK_FLAG (1 << 14)

// clk32kless_ctrl1
#define RDA2720M_GLOBAL_RC_MODE(n) (((n)&0xffff) << 0)

// xtl_wait_ctrl0
#define RDA2720M_GLOBAL_XTL_WAIT(n) (((n)&0xff) << 0)
#define RDA2720M_GLOBAL_XTL_EN (1 << 8)
#define RDA2720M_GLOBAL_SLP_XTLBUF_PD_EN (1 << 9)
#define RDA2720M_GLOBAL_EXT_XTL0_FOR_26M_EN (1 << 10)
#define RDA2720M_GLOBAL_EXT_XTL1_FOR_26M_EN (1 << 11)
#define RDA2720M_GLOBAL_EXT_XTL2_FOR_26M_EN (1 << 12)
#define RDA2720M_GLOBAL_EXT_XTL3_FOR_26M_EN (1 << 13)

// rgb_ctrl
#define RDA2720M_GLOBAL_RGB_PD_SW (1 << 0)
#define RDA2720M_GLOBAL_RGB_PD_HW_EN (1 << 1)
#define RDA2720M_GLOBAL_SLP_RGB_PD_EN (1 << 2)

// ib_ctrl
#define RDA2720M_GLOBAL_IB_REX_EN (1 << 0)
#define RDA2720M_GLOBAL_IB_TRIM_EM_SEL (1 << 1)
#define RDA2720M_GLOBAL_IB_TRIM(n) (((n)&0x7f) << 2)
#define RDA2720M_GLOBAL_BATDET_CUR_I(n) (((n)&0x7) << 9)
#define RDA2720M_GLOBAL_BATDET_CUR_EN (1 << 13)

// flash_ctrl
#define RDA2720M_GLOBAL_FLASH_V_SW(n) (((n)&0xf) << 0)
#define RDA2720M_GLOBAL_FLASH_V_HW_STEP(n) (((n)&0x3) << 4)
#define RDA2720M_GLOBAL_FLASH_V_HW_EN (1 << 6)
#define RDA2720M_GLOBAL_FLASH_PON (1 << 15)

// kpled_ctrl0
#define RDA2720M_GLOBAL_LDO_KPLED_CL_ADJ (1 << 0)
#define RDA2720M_GLOBAL_LDO_KPLED_SHPT_ADJ (1 << 1)
#define RDA2720M_GLOBAL_LDO_KPLED_STB(n) (((n)&0x3) << 2)
#define RDA2720M_GLOBAL_LDO_KPLED_CAP_SEL (1 << 4)
#define RDA2720M_GLOBAL_SLP_LDOKPLED_PD_EN (1 << 9)
#define RDA2720M_GLOBAL_KPLED_PULLDOWN_EN (1 << 10)
#define RDA2720M_GLOBAL_KPLED_PD (1 << 11)
#define RDA2720M_GLOBAL_KPLED_V(n) (((n)&0xf) << 12)

// kpled_ctrl1
#define RDA2720M_GLOBAL_LDO_KPLED_SHPT_PD (1 << 0)
#define RDA2720M_GLOBAL_LDO_KPLED_REFTRIM(n) (((n)&0x1f) << 2)
#define RDA2720M_GLOBAL_LDO_KPLED_V(n) (((n)&0x7) << 7)
#define RDA2720M_GLOBAL_LDO_KPLED_PD (1 << 15)

// vibr_ctrl0
#define RDA2720M_GLOBAL_LDO_VIBR_V(n) (((n)&0x7) << 0)
#define RDA2720M_GLOBAL_LDO_VIBR_PD (1 << 8)
#define RDA2720M_GLOBAL_SLP_LDOVIBR_PD_EN (1 << 9)
#define RDA2720M_GLOBAL_LDO_VIBR_SHPT_PD (1 << 10)
#define RDA2720M_GLOBAL_LDO_VIBR_CL_ADJ (1 << 11)
#define RDA2720M_GLOBAL_LDO_VIBR_SHPT_ADJ (1 << 12)
#define RDA2720M_GLOBAL_LDO_VIBR_STB(n) (((n)&0x3) << 13)
#define RDA2720M_GLOBAL_LDO_VIBR_CAP_SEL (1 << 15)

// vibr_ctrl1
#define RDA2720M_GLOBAL_LDO_VIBR_EADBIAS_EN (1 << 0)
#define RDA2720M_GLOBAL_LDO_VIBR_REFTRIM(n) (((n)&0x1f) << 1)

// audio_ctrl0
#define RDA2720M_GLOBAL_CLK_AUD_IF_6P5M_RX_INV_EN (1 << 0)
#define RDA2720M_GLOBAL_CLK_AUD_IF_6P5M_TX_INV_EN (1 << 1)
#define RDA2720M_GLOBAL_CLK_AUD_IF_RX_INV_EN (1 << 2)
#define RDA2720M_GLOBAL_CLK_AUD_IF_TX_INV_EN (1 << 3)

// chgr_ctrl0
#define RDA2720M_GLOBAL_CHGR_PD (1 << 0)
#define RDA2720M_GLOBAL_CHGR_ITERM(n) (((n)&0x3) << 1)
#define RDA2720M_GLOBAL_CHGR_END_V(n) (((n)&0x3) << 3)
#define RDA2720M_GLOBAL_CHGR_CV_V(n) (((n)&0x3f) << 5)
#define RDA2720M_GLOBAL_CHGR_CC_EN (1 << 12)
#define RDA2720M_GLOBAL_CHGR_DPM(n) (((n)&0x3) << 13)

// chgr_ctrl1
#define RDA2720M_GLOBAL_VCHG_OVP_V(n) (((n)&0x3) << 0)
#define RDA2720M_GLOBAL_CHGR_CC_I(n) (((n)&0xf) << 10)
#define RDA2720M_GLOBAL_CHGR_EXPOWER_DEVICE (1 << 15)

// chgr_status
#define RDA2720M_GLOBAL_VCHG_OVI (1 << 0)
#define RDA2720M_GLOBAL_DCP_SWITCH_EN (1 << 1)
#define RDA2720M_GLOBAL_CHGR_INT (1 << 2)
#define RDA2720M_GLOBAL_CHGR_ON (1 << 3)
#define RDA2720M_GLOBAL_CHGR_CV_STATUS (1 << 4)
#define RDA2720M_GLOBAL_CDP_INT (1 << 5)
#define RDA2720M_GLOBAL_DCP_INT (1 << 6)
#define RDA2720M_GLOBAL_SDP_INT (1 << 7)
#define RDA2720M_GLOBAL_CHG_DET (1 << 8)
#define RDA2720M_GLOBAL_DCP_DET (1 << 9)
#define RDA2720M_GLOBAL_DP_LOW (1 << 10)
#define RDA2720M_GLOBAL_CHG_DET_DONE (1 << 11)
#define RDA2720M_GLOBAL_NON_DCP_INT (1 << 12)
#define RDA2720M_GLOBAL_CHGR_INT_EN (1 << 13)

// chgr_det_fgu_ctrl
#define RDA2720M_GLOBAL_DP_DM_BC_ENB (1 << 0)
#define RDA2720M_GLOBAL_DP_DM_AUX_EN (1 << 1)
#define RDA2720M_GLOBAL_SD_DCOFFSET_A_EN (1 << 2)
#define RDA2720M_GLOBAL_SD_DCOFFSET_B_EN (1 << 3)
#define RDA2720M_GLOBAL_SD_CHOP_EN (1 << 4)
#define RDA2720M_GLOBAL_SD_ADC0_RC_PD (1 << 5)
#define RDA2720M_GLOBAL_SD_CLK_P(n) (((n)&0x3) << 6)
#define RDA2720M_GLOBAL_SD_CHOP_CAP_EN (1 << 8)
#define RDA2720M_GLOBAL_CHG_INT_DELAY(n) (((n)&0x7) << 9)
#define RDA2720M_GLOBAL_LDO_FGU_PD (1 << 12)
#define RDA2720M_GLOBAL_FGUA_SOFT_RST (1 << 13)

// ovlo_ctrl
#define RDA2720M_GLOBAL_OVLO_T(n) (((n)&0x3) << 0)
#define RDA2720M_GLOBAL_OVLO_V(n) (((n)&0x3) << 2)
#define RDA2720M_GLOBAL_OVLO_EN (1 << 9)
#define RDA2720M_GLOBAL_VBAT_CRASH_V(n) (((n)&0x3) << 10)

// mixed_ctrl
#define RDA2720M_GLOBAL_INT_DEBUG_EN (1 << 0)
#define RDA2720M_GLOBAL_ALL_INT_DEB (1 << 1)
#define RDA2720M_GLOBAL_GPI_DEBUG_EN (1 << 2)
#define RDA2720M_GLOBAL_ALL_GPI_DEB (1 << 3)
#define RDA2720M_GLOBAL_VBAT_OK (1 << 4)
#define RDA2720M_GLOBAL_BATDET_OK (1 << 8)
#define RDA2720M_GLOBAL_VPP_5V_SEL (1 << 9)
#define RDA2720M_GLOBAL_BATON_T(n) (((n)&0x3) << 10)
#define RDA2720M_GLOBAL_XOSC32K_CTL (1 << 12)

// por_rst_monitor
#define RDA2720M_GLOBAL_POR_RST_MONITOR(n) (((n)&0xffff) << 0)

// wdg_rst_monitor
#define RDA2720M_GLOBAL_WDG_RST_MONITOR(n) (((n)&0xffff) << 0)

// por_pin_rst_monitor
#define RDA2720M_GLOBAL_PRO_PIN_RST_MONITOR(n) (((n)&0xffff) << 0)

// por_src_flag
#define RDA2720M_GLOBAL_POR_SRC_FLAG(n) (((n)&0x3fff) << 0)
#define RDA2720M_GLOBAL_REG_SOFT_RST_FLG_CLR (1 << 14)
#define RDA2720M_GLOBAL_POR_SW_FORCE_ON (1 << 15)

// por_off_flag
#define RDA2720M_GLOBAL_OTP_CHIP_PD_FLAG_CLR (1 << 2)
#define RDA2720M_GLOBAL_OTP_CHIP_PD_FLAG (1 << 3)
#define RDA2720M_GLOBAL_HW_CHIP_PD_FLAG_CLR (1 << 4)
#define RDA2720M_GLOBAL_HW_CHIP_PD_FLAG (1 << 5)
#define RDA2720M_GLOBAL_SW_CHIP_PD_FLAG_CLR (1 << 6)
#define RDA2720M_GLOBAL_SW_CHIP_PD_FLAG (1 << 7)
#define RDA2720M_GLOBAL_HARD_7S_CHIP_PD_FLAG_CLR (1 << 8)
#define RDA2720M_GLOBAL_HARD_7S_CHIP_PD_FLAG (1 << 9)
#define RDA2720M_GLOBAL_UVLO_CHIP_PD_FLAG_CLR (1 << 10)
#define RDA2720M_GLOBAL_UVLO_CHIP_PD_FLAG (1 << 11)
#define RDA2720M_GLOBAL_POR_CHIP_PD_FLAG_CLR (1 << 12)
#define RDA2720M_GLOBAL_POR_CHIP_PD_FLAG (1 << 13)

// por_7s_ctrl
#define RDA2720M_GLOBAL_PBINT_7S_RST_MODE (1 << 0)
#define RDA2720M_GLOBAL_PBINT_7S_RST_DISABLE (1 << 1)
#define RDA2720M_GLOBAL_PBINT_7S_AUTO_ON_EN (1 << 2)
#define RDA2720M_GLOBAL_EXT_RSTN_MODE (1 << 3)
#define RDA2720M_GLOBAL_PBINT_7S_RST_THRESHOLD(n) (((n)&0xf) << 4)
#define RDA2720M_GLOBAL_PBINT_7S_RST_SWMODE (1 << 8)
#define RDA2720M_GLOBAL_KEY2_7S_RST_EN (1 << 9)
#define RDA2720M_GLOBAL_PBINT_FLAG_CLR (1 << 11)
#define RDA2720M_GLOBAL_PBINT2_FLAG_CLR (1 << 12)
#define RDA2720M_GLOBAL_CHGR_INT_FLAG_CLR (1 << 13)
#define RDA2720M_GLOBAL_EXT_RSTN_FLAG_CLR (1 << 14)
#define RDA2720M_GLOBAL_PBINT_7S_FLAG_CLR (1 << 15)

// hwrst_rtc
#define RDA2720M_GLOBAL_HWRST_RTC_REG_SET(n) (((n)&0xff) << 0)
#define RDA2720M_GLOBAL_HWRST_RTC_REG_STS(n) (((n)&0xff) << 8)

// arch_en
#define RDA2720M_GLOBAL_ARCH_EN (1 << 0)

// mcu_wr_prot_value
#define RDA2720M_GLOBAL_MCU_WR_PROT_VALUE(n) (((n)&0x7fff) << 0)
#define RDA2720M_GLOBAL_MCU_WR_PROT (1 << 15)

// pwr_wr_prot_value
#define RDA2720M_GLOBAL_PWR_WR_PROT_VALUE(n) (((n)&0x7fff) << 0)
#define RDA2720M_GLOBAL_PWR_WR_PROT (1 << 15)

// smpl_ctrl0
#define RDA2720M_GLOBAL_SMPL_MODE(n) (((n)&0xffff) << 0)

// smpl_ctrl1
#define RDA2720M_GLOBAL_SMPL_EN (1 << 0)
#define RDA2720M_GLOBAL_SMPL_PWR_ON_SET (1 << 11)
#define RDA2720M_GLOBAL_SMPL_MODE_WR_ACK_FLAG_CLR (1 << 12)
#define RDA2720M_GLOBAL_SMPL_PWR_ON_FLAG_CLR (1 << 13)
#define RDA2720M_GLOBAL_SMPL_MODE_WR_ACK_FLAG (1 << 14)
#define RDA2720M_GLOBAL_SMPL_PWR_ON_FLAG (1 << 15)

// rtc_rst0
#define RDA2720M_GLOBAL_RTC_CLK_FLAG_SET(n) (((n)&0xffff) << 0)

// rtc_rst1
#define RDA2720M_GLOBAL_RTC_CLK_FLAG_CLR(n) (((n)&0xffff) << 0)

// rtc_rst2
#define RDA2720M_GLOBAL_RTC_CLK_FLAG_RTC(n) (((n)&0xffff) << 0)

// rtc_clk_stop
#define RDA2720M_GLOBAL_RTC_CLK_STOP_THRESHOLD(n) (((n)&0x7f) << 0)
#define RDA2720M_GLOBAL_RTC_CLK_STOP_FLAG (1 << 7)

// vbat_drop_cnt
#define RDA2720M_GLOBAL_VBAT_DROP_CNT(n) (((n)&0xfff) << 0)

// swrst_ctrl0
#define RDA2720M_GLOBAL_SW_RST_PD_THRESHOLD(n) (((n)&0xf) << 0)
#define RDA2720M_GLOBAL_REG_RST_EN (1 << 4)
#define RDA2720M_GLOBAL_WDG_RST_PD_EN (1 << 7)
#define RDA2720M_GLOBAL_REG_RST_PD_EN (1 << 8)
#define RDA2720M_GLOBAL_PB_7S_RST_PD_EN (1 << 9)
#define RDA2720M_GLOBAL_EXT_RSTN_PD_EN (1 << 10)

// swrst_ctrl1
#define RDA2720M_GLOBAL_SW_RST_VIO18_PD_EN (1 << 0)
#define RDA2720M_GLOBAL_SW_RST_DDR12_PD_EN (1 << 1)
#define RDA2720M_GLOBAL_SW_RST_MMC_PD_EN (1 << 2)
#define RDA2720M_GLOBAL_SW_RST_USB33_PD_EN (1 << 3)
#define RDA2720M_GLOBAL_SW_RST_RF15_PD_EN (1 << 4)
#define RDA2720M_GLOBAL_SW_RST_ANA_PD_EN (1 << 5)
#define RDA2720M_GLOBAL_SW_RST_VDD28_PD_EN (1 << 6)
#define RDA2720M_GLOBAL_SW_RST_DCXO_PD_EN (1 << 7)
#define RDA2720M_GLOBAL_SW_RST_MEM_PD_EN (1 << 8)
#define RDA2720M_GLOBAL_SW_RST_DCDCCORE_PD_EN (1 << 9)
#define RDA2720M_GLOBAL_SW_RST_DCDCGEN_PD_EN (1 << 10)

// otp_ctrl
#define RDA2720M_GLOBAL_OTP_EN (1 << 0)
#define RDA2720M_GLOBAL_OTP_OP(n) (((n)&0x3) << 1)

// free_timer_low
#define RDA2720M_GLOBAL_TIMER_LOW(n) (((n)&0xffff) << 0)

// free_timer_high
#define RDA2720M_GLOBAL_TIMER_HIGH(n) (((n)&0xffff) << 0)

// vol_tune_ctrl_core
#define RDA2720M_GLOBAL_CORE_VOL_TUNE_EN (1 << 0)
#define RDA2720M_GLOBAL_CORE_VOL_TUNE_FLAG (1 << 1)
#define RDA2720M_GLOBAL_CORE_VOL_TUNE_START (1 << 2)
#define RDA2720M_GLOBAL_CORE_STEP_VOL(n) (((n)&0x1f) << 3)
#define RDA2720M_GLOBAL_CORE_STEP_NUM(n) (((n)&0xf) << 8)
#define RDA2720M_GLOBAL_CORE_STEP_DELAY(n) (((n)&0x3) << 12)
#define RDA2720M_GLOBAL_CORE_CLK_SEL (1 << 14)

#endif // _RDA2720M_GLOBAL_H_
