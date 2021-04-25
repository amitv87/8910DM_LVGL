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

#ifndef _AUD_2AD_H_
#define _AUD_2AD_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_AUD_2AD_BASE (0x0880d000)

typedef volatile struct
{
    uint32_t aud_top_ctl;        // 0x00000000
    uint32_t aud_clr;            // 0x00000004
    uint32_t aud_iis_ctl;        // 0x00000008
    uint32_t dac_src_ctl;        // 0x0000000c
    uint32_t dac_sdm_ctl0;       // 0x00000010
    uint32_t dac_sdm_ctl1;       // 0x00000014
    uint32_t adc_src_ctl;        // 0x00000018
    uint32_t aud_loop_test;      // 0x0000001c
    uint32_t aud_sts0;           // 0x00000020
    uint32_t aud_int_clr;        // 0x00000024
    uint32_t aud_int_en;         // 0x00000028
    uint32_t audif_fifo_ctl;     // 0x0000002c
    uint32_t aud_dmic_ctl;       // 0x00000030
    uint32_t adc1_iis_ctl;       // 0x00000034
    uint32_t dac_sdm_dc_l;       // 0x00000038
    uint32_t dac_sdm_dc_h;       // 0x0000003c
    uint32_t audif_ctl0;         // 0x00000040
    uint32_t audif_adc_fifo_sts; // 0x00000044
    uint32_t audif_dac_fifo_sts; // 0x00000048
    uint32_t audif_sts;          // 0x0000004c
    uint32_t audif_sts_raw;      // 0x00000050
    uint32_t audif_sts_clr;      // 0x00000054
    uint32_t dac_src_step;       // 0x00000058
    uint32_t adc_dgain;          // 0x0000005c
    uint32_t dac_dgain0;         // 0x00000060
    uint32_t dac_dgain1;         // 0x00000064
    uint32_t dac_dgain2;         // 0x00000068
} HWP_AUD_2AD_T;

#define hwp_aud2ad ((HWP_AUD_2AD_T *)REG_ACCESS_ADDRESS(REG_AUD_2AD_BASE))

// aud_top_ctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_en_l : 1;         // [0]
        uint32_t adc_en_l : 1;         // [1]
        uint32_t dac_en_r : 1;         // [2]
        uint32_t adc_en_r : 1;         // [3]
        uint32_t dac_iis_sel : 2;      // [5:4]
        uint32_t adc_iis_sel : 2;      // [7:6]
        uint32_t adc_sinc_in_sel : 2;  // [9:8]
        uint32_t adc1_en_l : 1;        // [10]
        uint32_t adc1_en_r : 1;        // [11]
        uint32_t adc1_iis_sel : 2;     // [13:12]
        uint32_t adc1_sinc_in_sel : 2; // [15:14]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_AUD_2AD_AUD_TOP_CTL_T;

// aud_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc1_clr : 1; // [0]
        uint32_t dac_clr : 1;  // [1]
        uint32_t adc_clr : 1;  // [2]
        uint32_t __31_3 : 29;  // [31:3]
    } b;
} REG_AUD_2AD_AUD_CLR_T;

// aud_iis_ctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t iis_clkdiv_mode : 1;      // [0]
        uint32_t dac_lr_sel : 1;           // [1]
        uint32_t adc_lr_sel : 1;           // [2]
        uint32_t dac_io_mode : 2;          // [4:3]
        uint32_t adc_io_mode : 2;          // [6:5]
        uint32_t dac_iowl : 2;             // [8:7]
        uint32_t adc_iowl : 2;             // [10:9]
        uint32_t dac_sample_phase_sel : 1; // [11]
        uint32_t dac_bclk_pol : 1;         // [12]
        uint32_t adc_bclk_pol : 1;         // [13]
        uint32_t dac_iis_ckgate_en : 1;    // [14]
        uint32_t adc_iis_ckgate_en : 1;    // [15]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_AUD_2AD_AUD_IIS_CTL_T;

// dac_src_ctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_fs_mode : 4;       // [3:0]
        uint32_t dac_mute_div_ctl0 : 6; // [9:4]
        uint32_t dac_mute_div_ctl1 : 4; // [13:10]
        uint32_t dac_mute_ctl : 1;      // [14]
        uint32_t dac_mute_en : 1;       // [15]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_AUD_2AD_DAC_SRC_CTL_T;

// dac_sdm_ctl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_sdm_di : 2;    // [1:0]
        uint32_t dac_sdm_do : 2;    // [3:2]
        uint32_t dac_sdm_dilvl : 4; // [7:4]
        uint32_t dac_sdm_dolvl : 4; // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_AUD_2AD_DAC_SDM_CTL0_T;

// dac_sdm_ctl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_sdm_test : 8;       // [7:0]
        uint32_t dac_sdm_soft_rst_l : 1; // [8]
        uint32_t dac_sdm_soft_rst_r : 1; // [9]
        uint32_t __31_10 : 22;           // [31:10]
    } b;
} REG_AUD_2AD_DAC_SDM_CTL1_T;

// adc_src_ctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_src_n : 4;  // [3:0]
        uint32_t adc1_src_n : 4; // [7:4]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_AUD_2AD_ADC_SRC_CTL_T;

// aud_loop_test
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aud_loop_test : 1;     // [0]
        uint32_t loop_path_sel : 2;     // [2:1]
        uint32_t loop_fifo_af_lvl : 3;  // [5:3]
        uint32_t loop_fifo_ae_lvl : 3;  // [8:6]
        uint32_t loop_adc_path_sel : 1; // [9]
        uint32_t __31_10 : 22;          // [31:10]
    } b;
} REG_AUD_2AD_AUD_LOOP_TEST_T;

// aud_sts0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t st_mute : 2;      // [1:0], read only
        uint32_t aud_int_raw : 2;  // [3:2], read only
        uint32_t aud_int_mask : 2; // [5:4], read only
        uint32_t __31_6 : 26;      // [31:6]
    } b;
} REG_AUD_2AD_AUD_STS0_T;

// aud_int_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aud_int_clr : 1; // [0]
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_AUD_2AD_AUD_INT_CLR_T;

// aud_int_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aud_int_en : 2; // [1:0]
        uint32_t __31_2 : 30;    // [31:2]
    } b;
} REG_AUD_2AD_AUD_INT_EN_T;

// audif_fifo_ctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fifo_af_lvl : 3; // [2:0]
        uint32_t __31_3 : 29;         // [31:3]
    } b;
} REG_AUD_2AD_AUDIF_FIFO_CTL_T;

// aud_dmic_ctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_dmic_clk_mode : 2;  // [1:0]
        uint32_t clk_aud_26m_inv : 1;    // [2]
        uint32_t adc1_dmic_clk_mode : 2; // [4:3]
        uint32_t adc1_dmic_lr_sel : 1;   // [5]
        uint32_t clk_aud_26m_sel : 1;    // [6]
        uint32_t adc1_dmic_en : 1;       // [7]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_AUD_2AD_AUD_DMIC_CTL_T;

// adc1_iis_ctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc1_lr_sel : 1;        // [0]
        uint32_t adc1_io_mode : 2;       // [2:1]
        uint32_t adc1_iowl : 2;          // [4:3]
        uint32_t adc1_bclk_pol : 1;      // [5]
        uint32_t adc1_iis_ckgate_en : 1; // [6]
        uint32_t __31_7 : 25;            // [31:7]
    } b;
} REG_AUD_2AD_ADC1_IIS_CTL_T;

// dac_sdm_dc_l
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_sdm_dc_l : 16; // [15:0]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_AUD_2AD_DAC_SDM_DC_L_T;

// dac_sdm_dc_h
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_sdm_dc_h : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_AUD_2AD_DAC_SDM_DC_H_T;

// audif_ctl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fifo_af_lvl_r : 3; // [2:0]
        uint32_t ad_sync_sel : 3;       // [5:3]
        uint32_t audif_5p_mode : 1;     // [6]
        uint32_t __31_7 : 25;           // [31:7]
    } b;
} REG_AUD_2AD_AUDIF_CTL0_T;

// audif_adc_fifo_sts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t audif_adc_fifo_waddr_r : 4; // [3:0], read only
        uint32_t audif_adc_fifo_raddr_r : 4; // [7:4], read only
        uint32_t audif_adc_fifo_full_r : 1;  // [8], read only
        uint32_t audif_adc_fifo_empty_r : 1; // [9], read only
        uint32_t audif_adc_fifo_af_r : 1;    // [10], read only
        uint32_t __31_11 : 21;               // [31:11]
    } b;
} REG_AUD_2AD_AUDIF_ADC_FIFO_STS_T;

// audif_dac_fifo_sts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t audif_dac_fifo_addr_w : 4; // [3:0], read only
        uint32_t audif_dac_fifo_addr_r : 4; // [7:4], read only
        uint32_t audif_dac_fifo_full : 1;   // [8], read only
        uint32_t audif_dac_fifo_empty : 1;  // [9], read only
        uint32_t __31_10 : 22;              // [31:10]
    } b;
} REG_AUD_2AD_AUDIF_DAC_FIFO_STS_T;

// audif_sts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t audif_adc_rx_data_ready : 1; // [0], read only
        uint32_t __31_1 : 31;                 // [31:1]
    } b;
} REG_AUD_2AD_AUDIF_STS_T;

// audif_sts_raw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t audif_dac_fifo_ovfl_raw : 1;    // [0], read only
        uint32_t audif_adc_fifo_underfl_raw : 1; // [1], read only
        uint32_t __31_2 : 30;                    // [31:2]
    } b;
} REG_AUD_2AD_AUDIF_STS_RAW_T;

// audif_sts_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t underfl_sts_clr : 1; // [0]
        uint32_t ovfl_sts_clr : 1;    // [1]
        uint32_t __31_2 : 30;         // [31:2]
    } b;
} REG_AUD_2AD_AUDIF_STS_CLR_T;

// dac_src_step
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_src_step : 12; // [11:0]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_AUD_2AD_DAC_SRC_STEP_T;

// adc_dgain
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_l_dgain : 4;      // [3:0]
        uint32_t adc_r_dgain : 4;      // [7:4]
        uint32_t adc_dgain_update : 1; // [8]
        uint32_t __31_9 : 23;          // [31:9]
    } b;
} REG_AUD_2AD_ADC_DGAIN_T;

// dac_dgain0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_dgain_tone_sel : 1; // [0]
        uint32_t dac_dgain_update : 1;   // [1]
        uint32_t __31_2 : 30;            // [31:2]
    } b;
} REG_AUD_2AD_DAC_DGAIN0_T;

// dac_dgain1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_l_nor_dgain : 8; // [7:0]
        uint32_t dac_r_nor_dgain : 8; // [15:8]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_AUD_2AD_DAC_DGAIN1_T;

// dac_dgain2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_l_tone_dgain : 8; // [7:0]
        uint32_t dac_r_tone_dgain : 8; // [15:8]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_AUD_2AD_DAC_DGAIN2_T;

// aud_top_ctl
#define AUD_2AD_DAC_EN_L (1 << 0)
#define AUD_2AD_ADC_EN_L (1 << 1)
#define AUD_2AD_DAC_EN_R (1 << 2)
#define AUD_2AD_ADC_EN_R (1 << 3)
#define AUD_2AD_DAC_IIS_SEL(n) (((n)&0x3) << 4)
#define AUD_2AD_ADC_IIS_SEL(n) (((n)&0x3) << 6)
#define AUD_2AD_ADC_SINC_IN_SEL(n) (((n)&0x3) << 8)
#define AUD_2AD_ADC1_EN_L (1 << 10)
#define AUD_2AD_ADC1_EN_R (1 << 11)
#define AUD_2AD_ADC1_IIS_SEL(n) (((n)&0x3) << 12)
#define AUD_2AD_ADC1_SINC_IN_SEL(n) (((n)&0x3) << 14)

// aud_clr
#define AUD_2AD_ADC1_CLR (1 << 0)
#define AUD_2AD_DAC_CLR (1 << 1)
#define AUD_2AD_ADC_CLR (1 << 2)

// aud_iis_ctl
#define AUD_2AD_IIS_CLKDIV_MODE (1 << 0)
#define AUD_2AD_DAC_LR_SEL (1 << 1)
#define AUD_2AD_ADC_LR_SEL (1 << 2)
#define AUD_2AD_DAC_IO_MODE(n) (((n)&0x3) << 3)
#define AUD_2AD_ADC_IO_MODE(n) (((n)&0x3) << 5)
#define AUD_2AD_DAC_IOWL(n) (((n)&0x3) << 7)
#define AUD_2AD_ADC_IOWL(n) (((n)&0x3) << 9)
#define AUD_2AD_DAC_SAMPLE_PHASE_SEL (1 << 11)
#define AUD_2AD_DAC_BCLK_POL (1 << 12)
#define AUD_2AD_ADC_BCLK_POL (1 << 13)
#define AUD_2AD_DAC_IIS_CKGATE_EN (1 << 14)
#define AUD_2AD_ADC_IIS_CKGATE_EN (1 << 15)

// dac_src_ctl
#define AUD_2AD_DAC_FS_MODE(n) (((n)&0xf) << 0)
#define AUD_2AD_DAC_MUTE_DIV_CTL0(n) (((n)&0x3f) << 4)
#define AUD_2AD_DAC_MUTE_DIV_CTL1(n) (((n)&0xf) << 10)
#define AUD_2AD_DAC_MUTE_CTL (1 << 14)
#define AUD_2AD_DAC_MUTE_EN (1 << 15)

// dac_sdm_ctl0
#define AUD_2AD_DAC_SDM_DI(n) (((n)&0x3) << 0)
#define AUD_2AD_DAC_SDM_DO(n) (((n)&0x3) << 2)
#define AUD_2AD_DAC_SDM_DILVL(n) (((n)&0xf) << 4)
#define AUD_2AD_DAC_SDM_DOLVL(n) (((n)&0xf) << 8)

// dac_sdm_ctl1
#define AUD_2AD_DAC_SDM_TEST(n) (((n)&0xff) << 0)
#define AUD_2AD_DAC_SDM_SOFT_RST_L (1 << 8)
#define AUD_2AD_DAC_SDM_SOFT_RST_R (1 << 9)

// adc_src_ctl
#define AUD_2AD_ADC_SRC_N(n) (((n)&0xf) << 0)
#define AUD_2AD_ADC1_SRC_N(n) (((n)&0xf) << 4)

// aud_loop_test
#define AUD_2AD_AUD_LOOP_TEST (1 << 0)
#define AUD_2AD_LOOP_PATH_SEL(n) (((n)&0x3) << 1)
#define AUD_2AD_LOOP_FIFO_AF_LVL(n) (((n)&0x7) << 3)
#define AUD_2AD_LOOP_FIFO_AE_LVL(n) (((n)&0x7) << 6)
#define AUD_2AD_LOOP_ADC_PATH_SEL (1 << 9)

// aud_sts0
#define AUD_2AD_ST_MUTE(n) (((n)&0x3) << 0)
#define AUD_2AD_AUD_INT_RAW(n) (((n)&0x3) << 2)
#define AUD_2AD_AUD_INT_MASK(n) (((n)&0x3) << 4)

// aud_int_clr
#define AUD_2AD_AUD_INT_CLR (1 << 0)

// aud_int_en
#define AUD_2AD_AUD_INT_EN(n) (((n)&0x3) << 0)

// audif_fifo_ctl
#define AUD_2AD_ADC_FIFO_AF_LVL(n) (((n)&0x7) << 0)

// aud_dmic_ctl
#define AUD_2AD_ADC_DMIC_CLK_MODE(n) (((n)&0x3) << 0)
#define AUD_2AD_CLK_AUD_26M_INV (1 << 2)
#define AUD_2AD_ADC1_DMIC_CLK_MODE(n) (((n)&0x3) << 3)
#define AUD_2AD_ADC1_DMIC_LR_SEL (1 << 5)
#define AUD_2AD_CLK_AUD_26M_SEL (1 << 6)
#define AUD_2AD_ADC1_DMIC_EN (1 << 7)

// adc1_iis_ctl
#define AUD_2AD_ADC1_LR_SEL (1 << 0)
#define AUD_2AD_ADC1_IO_MODE(n) (((n)&0x3) << 1)
#define AUD_2AD_ADC1_IOWL(n) (((n)&0x3) << 3)
#define AUD_2AD_ADC1_BCLK_POL (1 << 5)
#define AUD_2AD_ADC1_IIS_CKGATE_EN (1 << 6)

// dac_sdm_dc_l
#define AUD_2AD_DAC_SDM_DC_L(n) (((n)&0xffff) << 0)

// dac_sdm_dc_h
#define AUD_2AD_DAC_SDM_DC_H(n) (((n)&0xff) << 0)

// audif_ctl0
#define AUD_2AD_ADC_FIFO_AF_LVL_R(n) (((n)&0x7) << 0)
#define AUD_2AD_AD_SYNC_SEL(n) (((n)&0x7) << 3)
#define AUD_2AD_AUDIF_5P_MODE (1 << 6)

// audif_adc_fifo_sts
#define AUD_2AD_AUDIF_ADC_FIFO_WADDR_R(n) (((n)&0xf) << 0)
#define AUD_2AD_AUDIF_ADC_FIFO_RADDR_R(n) (((n)&0xf) << 4)
#define AUD_2AD_AUDIF_ADC_FIFO_FULL_R (1 << 8)
#define AUD_2AD_AUDIF_ADC_FIFO_EMPTY_R (1 << 9)
#define AUD_2AD_AUDIF_ADC_FIFO_AF_R (1 << 10)

// audif_dac_fifo_sts
#define AUD_2AD_AUDIF_DAC_FIFO_ADDR_W(n) (((n)&0xf) << 0)
#define AUD_2AD_AUDIF_DAC_FIFO_ADDR_R(n) (((n)&0xf) << 4)
#define AUD_2AD_AUDIF_DAC_FIFO_FULL (1 << 8)
#define AUD_2AD_AUDIF_DAC_FIFO_EMPTY (1 << 9)

// audif_sts
#define AUD_2AD_AUDIF_ADC_RX_DATA_READY (1 << 0)

// audif_sts_raw
#define AUD_2AD_AUDIF_DAC_FIFO_OVFL_RAW (1 << 0)
#define AUD_2AD_AUDIF_ADC_FIFO_UNDERFL_RAW (1 << 1)

// audif_sts_clr
#define AUD_2AD_UNDERFL_STS_CLR (1 << 0)
#define AUD_2AD_OVFL_STS_CLR (1 << 1)

// dac_src_step
#define AUD_2AD_DAC_SRC_STEP(n) (((n)&0xfff) << 0)

// adc_dgain
#define AUD_2AD_ADC_L_DGAIN(n) (((n)&0xf) << 0)
#define AUD_2AD_ADC_R_DGAIN(n) (((n)&0xf) << 4)
#define AUD_2AD_ADC_DGAIN_UPDATE (1 << 8)

// dac_dgain0
#define AUD_2AD_DAC_DGAIN_TONE_SEL (1 << 0)
#define AUD_2AD_DAC_DGAIN_UPDATE (1 << 1)

// dac_dgain1
#define AUD_2AD_DAC_L_NOR_DGAIN(n) (((n)&0xff) << 0)
#define AUD_2AD_DAC_R_NOR_DGAIN(n) (((n)&0xff) << 8)

// dac_dgain2
#define AUD_2AD_DAC_L_TONE_DGAIN(n) (((n)&0xff) << 0)
#define AUD_2AD_DAC_R_TONE_DGAIN(n) (((n)&0xff) << 8)

#endif // _AUD_2AD_H_
