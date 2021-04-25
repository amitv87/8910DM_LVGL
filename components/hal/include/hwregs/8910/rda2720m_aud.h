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

#ifndef _RDA2720M_AUD_H_
#define _RDA2720M_AUD_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_AUD_BASE (0x50308800)

typedef volatile struct
{
    uint32_t aud_cfga_clr;             // 0x00000000
    uint32_t aud_cfga_hid_cfg0;        // 0x00000004
    uint32_t aud_cfga_hid_cfg1;        // 0x00000008
    uint32_t aud_cfga_hid_cfg2;        // 0x0000000c
    uint32_t aud_cfga_hid_cfg3;        // 0x00000010
    uint32_t aud_cfga_hid_cfg4;        // 0x00000014
    uint32_t aud_cfga_hid_cfg5;        // 0x00000018
    uint32_t aud_cfga_hid_sts0;        // 0x0000001c
    uint32_t aud_cfga_prt_cfg_0;       // 0x00000020
    uint32_t aud_cfga_prt_cfg_1;       // 0x00000024
    uint32_t aud_cfga_rd_sts;          // 0x00000028
    uint32_t aud_cfga_int_module_ctrl; // 0x0000002c
    uint32_t aud_cfga_lp_module_ctrl;  // 0x00000030
    uint32_t ana_et2;                  // 0x00000034
    uint32_t clk_en;                   // 0x00000038
    uint32_t soft_rst;                 // 0x0000003c
} HWP_RDA2720M_AUD_T;

#define hwp_rda2720mAud ((HWP_RDA2720M_AUD_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_AUD_BASE))

// aud_cfga_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aud_int_clr : 8;      // [7:0], write clear
        uint32_t pa_shutdown_clr : 1;  // [8], write clear
        uint32_t hp_shutdown_clr : 1;  // [9], write clear
        uint32_t ear_shutdown_clr : 1; // [10], write clear
        uint32_t __31_11 : 21;         // [31:11]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_CLR_T;

// aud_cfga_hid_cfg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hid_en : 1;      // [0]
        uint32_t aud_dbnc_en : 2; // [2:1]
        uint32_t __31_3 : 29;     // [31:3]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_HID_CFG0_T;

// aud_cfga_hid_cfg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hid_low_dbnc_thd0 : 8;  // [7:0]
        uint32_t hid_high_dbnc_thd0 : 8; // [15:8]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_HID_CFG1_T;

// aud_cfga_hid_cfg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hid_tmr_t0 : 5;        // [4:0]
        uint32_t hid_tmr_t1t2_step : 5; // [9:5]
        uint32_t __31_10 : 22;          // [31:10]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_HID_CFG2_T;

// aud_cfga_hid_cfg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hid_tmr_t1 : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_HID_CFG3_T;

// aud_cfga_hid_cfg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hid_tmr_t2 : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_HID_CFG4_T;

// aud_cfga_hid_cfg5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hid_low_dbnc_thd1 : 8;  // [7:0]
        uint32_t hid_high_dbnc_thd1 : 8; // [15:8]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_HID_CFG5_T;

// aud_cfga_hid_sts0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aud_dbnc_sts0 : 3;         // [2:0], read only
        uint32_t aud_dbnc_sts1 : 3;         // [5:3], read only
        uint32_t audio_head_button_out : 1; // [6], read only
        uint32_t audio_head_insert_out : 1; // [7], read only
        uint32_t pa_shutdown : 1;           // [8], read only
        uint32_t hp_shutdown : 1;           // [9], read only
        uint32_t ear_shutdown : 1;          // [10], read only
        uint32_t __31_11 : 21;              // [31:11]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_HID_STS0_T;

// aud_cfga_prt_cfg_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t otp_pd_thd : 3;      // [2:0]
        uint32_t aud_prt_en : 1;      // [3]
        uint32_t aud_clk_sel : 1;     // [4]
        uint32_t pa_shutdown_en0 : 1; // [5]
        uint32_t pa_shutdown_en1 : 1; // [6]
        uint32_t pa_shutdown_en2 : 1; // [7]
        uint32_t hp_shutdown_en : 1;  // [8]
        uint32_t ear_shutdown_en : 1; // [9]
        uint32_t __31_10 : 22;        // [31:10]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_PRT_CFG_0_T;

// aud_cfga_prt_cfg_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ocp_precis : 3; // [2:0]
        uint32_t ocp_pd_thd : 3; // [5:3]
        uint32_t ovp_precis : 3; // [8:6]
        uint32_t ovp_pd_thd : 3; // [11:9]
        uint32_t otp_precis : 3; // [14:12]
        uint32_t __31_15 : 17;   // [31:15]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_PRT_CFG_1_T;

// aud_cfga_rd_sts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aud_irq_msk : 8; // [7:0], read only
        uint32_t aud_irq_raw : 8; // [15:8], read only
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_RD_STS_T;

// aud_cfga_int_module_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aud_int_en : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_INT_MODULE_CTRL_T;

// aud_cfga_lp_module_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t audio_adie_loop_en : 1; // [0]
        uint32_t audio_loop_map_sel : 1; // [1]
        uint32_t dac_en_l : 1;           // [2]
        uint32_t adc_en_l : 1;           // [3]
        uint32_t dac_en_r : 1;           // [4]
        uint32_t adc_en_r : 1;           // [5]
        uint32_t __31_6 : 26;            // [31:6]
    } b;
} REG_RDA2720M_AUD_AUD_CFGA_LP_MODULE_CTRL_T;

// ana_et2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_das_mix_sel : 2;  // [1:0]
        uint32_t rg_aud_dalr_mix_sel : 2; // [3:2]
        uint32_t dem_bypass : 1;          // [4]
        uint32_t __31_5 : 27;             // [31:5]
    } b;
} REG_RDA2720M_AUD_ANA_ET2_T;

// clk_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_aud_32k_en : 1;      // [0]
        uint32_t clk_aud_1k_en : 1;       // [1]
        uint32_t clk_aud_hid_en : 1;      // [2]
        uint32_t clk_aud_loop_en : 1;     // [3]
        uint32_t clk_aud_6p5m_en : 1;     // [4]
        uint32_t clk_aud_loop_inv_en : 1; // [5]
        uint32_t __31_6 : 26;             // [31:6]
    } b;
} REG_RDA2720M_AUD_CLK_EN_T;

// soft_rst
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aud_32k_soft_rst : 1;      // [0], write clear
        uint32_t aud_1k_soft_rst : 1;       // [1], write clear
        uint32_t aud_hid_soft_rst : 1;      // [2], write clear
        uint32_t aud_dig_loop_soft_rst : 1; // [3], write clear
        uint32_t aud_dig_6p5m_soft_rst : 1; // [4], write clear
        uint32_t aud_dac_post_soft_rst : 1; // [5], write clear
        uint32_t __31_6 : 26;               // [31:6]
    } b;
} REG_RDA2720M_AUD_SOFT_RST_T;

// aud_cfga_clr
#define RDA2720M_AUD_AUD_INT_CLR(n) (((n)&0xff) << 0)
#define RDA2720M_AUD_PA_SHUTDOWN_CLR (1 << 8)
#define RDA2720M_AUD_HP_SHUTDOWN_CLR (1 << 9)
#define RDA2720M_AUD_EAR_SHUTDOWN_CLR (1 << 10)

// aud_cfga_hid_cfg0
#define RDA2720M_AUD_HID_EN (1 << 0)
#define RDA2720M_AUD_AUD_DBNC_EN(n) (((n)&0x3) << 1)

// aud_cfga_hid_cfg1
#define RDA2720M_AUD_HID_LOW_DBNC_THD0(n) (((n)&0xff) << 0)
#define RDA2720M_AUD_HID_HIGH_DBNC_THD0(n) (((n)&0xff) << 8)

// aud_cfga_hid_cfg2
#define RDA2720M_AUD_HID_TMR_T0(n) (((n)&0x1f) << 0)
#define RDA2720M_AUD_HID_TMR_T1T2_STEP(n) (((n)&0x1f) << 5)

// aud_cfga_hid_cfg3
#define RDA2720M_AUD_HID_TMR_T1(n) (((n)&0xffff) << 0)

// aud_cfga_hid_cfg4
#define RDA2720M_AUD_HID_TMR_T2(n) (((n)&0xffff) << 0)

// aud_cfga_hid_cfg5
#define RDA2720M_AUD_HID_LOW_DBNC_THD1(n) (((n)&0xff) << 0)
#define RDA2720M_AUD_HID_HIGH_DBNC_THD1(n) (((n)&0xff) << 8)

// aud_cfga_hid_sts0
#define RDA2720M_AUD_AUD_DBNC_STS0(n) (((n)&0x7) << 0)
#define RDA2720M_AUD_AUD_DBNC_STS1(n) (((n)&0x7) << 3)
#define RDA2720M_AUD_AUDIO_HEAD_BUTTON_OUT (1 << 6)
#define RDA2720M_AUD_AUDIO_HEAD_INSERT_OUT (1 << 7)
#define RDA2720M_AUD_PA_SHUTDOWN (1 << 8)
#define RDA2720M_AUD_HP_SHUTDOWN (1 << 9)
#define RDA2720M_AUD_EAR_SHUTDOWN (1 << 10)

// aud_cfga_prt_cfg_0
#define RDA2720M_AUD_OTP_PD_THD(n) (((n)&0x7) << 0)
#define RDA2720M_AUD_AUD_PRT_EN (1 << 3)
#define RDA2720M_AUD_AUD_CLK_SEL (1 << 4)
#define RDA2720M_AUD_PA_SHUTDOWN_EN0 (1 << 5)
#define RDA2720M_AUD_PA_SHUTDOWN_EN1 (1 << 6)
#define RDA2720M_AUD_PA_SHUTDOWN_EN2 (1 << 7)
#define RDA2720M_AUD_HP_SHUTDOWN_EN (1 << 8)
#define RDA2720M_AUD_EAR_SHUTDOWN_EN (1 << 9)

// aud_cfga_prt_cfg_1
#define RDA2720M_AUD_OCP_PRECIS(n) (((n)&0x7) << 0)
#define RDA2720M_AUD_OCP_PD_THD(n) (((n)&0x7) << 3)
#define RDA2720M_AUD_OVP_PRECIS(n) (((n)&0x7) << 6)
#define RDA2720M_AUD_OVP_PD_THD(n) (((n)&0x7) << 9)
#define RDA2720M_AUD_OTP_PRECIS(n) (((n)&0x7) << 12)

// aud_cfga_rd_sts
#define RDA2720M_AUD_AUD_IRQ_MSK(n) (((n)&0xff) << 0)
#define RDA2720M_AUD_AUD_IRQ_RAW(n) (((n)&0xff) << 8)

// aud_cfga_int_module_ctrl
#define RDA2720M_AUD_AUD_INT_EN(n) (((n)&0xff) << 0)

// aud_cfga_lp_module_ctrl
#define RDA2720M_AUD_AUDIO_ADIE_LOOP_EN (1 << 0)
#define RDA2720M_AUD_AUDIO_LOOP_MAP_SEL (1 << 1)
#define RDA2720M_AUD_DAC_EN_L (1 << 2)
#define RDA2720M_AUD_ADC_EN_L (1 << 3)
#define RDA2720M_AUD_DAC_EN_R (1 << 4)
#define RDA2720M_AUD_ADC_EN_R (1 << 5)

// ana_et2
#define RDA2720M_AUD_RG_AUD_DAS_MIX_SEL(n) (((n)&0x3) << 0)
#define RDA2720M_AUD_RG_AUD_DALR_MIX_SEL(n) (((n)&0x3) << 2)
#define RDA2720M_AUD_DEM_BYPASS (1 << 4)

// clk_en
#define RDA2720M_AUD_CLK_AUD_32K_EN (1 << 0)
#define RDA2720M_AUD_CLK_AUD_1K_EN (1 << 1)
#define RDA2720M_AUD_CLK_AUD_HID_EN (1 << 2)
#define RDA2720M_AUD_CLK_AUD_LOOP_EN (1 << 3)
#define RDA2720M_AUD_CLK_AUD_6P5M_EN (1 << 4)
#define RDA2720M_AUD_CLK_AUD_LOOP_INV_EN (1 << 5)

// soft_rst
#define RDA2720M_AUD_AUD_32K_SOFT_RST (1 << 0)
#define RDA2720M_AUD_AUD_1K_SOFT_RST (1 << 1)
#define RDA2720M_AUD_AUD_HID_SOFT_RST (1 << 2)
#define RDA2720M_AUD_AUD_DIG_LOOP_SOFT_RST (1 << 3)
#define RDA2720M_AUD_AUD_DIG_6P5M_SOFT_RST (1 << 4)
#define RDA2720M_AUD_AUD_DAC_POST_SOFT_RST (1 << 5)

#endif // _RDA2720M_AUD_H_
