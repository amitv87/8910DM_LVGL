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

#ifndef _RF_RTC_H_
#define _RF_RTC_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_RF_RTC_BASE (0x05034000)
#else
#define REG_RF_RTC_BASE (0x50034000)
#endif

typedef volatile struct
{
    uint32_t __0[40];    // 0x00000000
    uint32_t reg_00_reg; // 0x000000a0
    uint32_t __164[2];   // 0x000000a4
    uint32_t reg_0c_reg; // 0x000000ac
    uint32_t reg_10_reg; // 0x000000b0
    uint32_t reg_14_reg; // 0x000000b4
    uint32_t reg_18_reg; // 0x000000b8
    uint32_t reg_1c_reg; // 0x000000bc
    uint32_t reg_c0_reg; // 0x000000c0
    uint32_t reg_c4_reg; // 0x000000c4
    uint32_t reg_c8_reg; // 0x000000c8
    uint32_t reg_cc_reg; // 0x000000cc
    uint32_t reg_d0_reg; // 0x000000d0
    uint32_t reg_d4_reg; // 0x000000d4
    uint32_t reg_d8_reg; // 0x000000d8
} HWP_RF_RTC_T;

#define hwp_rfRtc ((HWP_RF_RTC_T *)REG_ACCESS_ADDRESS(REG_RF_RTC_BASE))

// reg_00_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_a1_bit : 8; // [7:0]
        uint32_t reg_a0_bit : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_RF_RTC_REG_00_REG_T;

// reg_0c_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_a7_bit : 8; // [7:0]
        uint32_t reg_a6_bit : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_RF_RTC_REG_0C_REG_T;

// reg_10_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_a9_bit : 8; // [7:0]
        uint32_t reg_a8_bit : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_RF_RTC_REG_10_REG_T;

// reg_14_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_ab_bit : 8; // [7:0]
        uint32_t reg_aa_bit : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_RF_RTC_REG_14_REG_T;

// reg_18_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_ad_bit : 8; // [7:0]
        uint32_t reg_ac_bit : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_RF_RTC_REG_18_REG_T;

// reg_1c_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_af_bit : 8; // [7:0]
        uint32_t reg_ae_bit : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_RF_RTC_REG_1C_REG_T;

// reg_c0_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dig_afc_bit_reg : 14; // [13:0]
        uint32_t enable_clk_6p5m : 1;  // [14]
        uint32_t __31_15 : 17;         // [31:15]
    } b;
} REG_RF_RTC_REG_C0_REG_T;

// reg_c4_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xtal_osc_ibit_n : 4;   // [3:0]
        uint32_t xtal_osc_ibit_l : 4;   // [7:4]
        uint32_t xtal26m_pwadc_en : 1;  // [8]
        uint32_t xtal_reg_bit : 4;      // [12:9]
        uint32_t xtal26m_tsxadc_en : 1; // [13]
        uint32_t xtal26m_plls1_en : 1;  // [14]
        uint32_t xtal26m_plls2_en : 1;  // [15]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RF_RTC_REG_C4_REG_T;

// reg_c8_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xtal_fixi_bit_n : 5;   // [4:0]
        uint32_t xtal_fixi_bit_l : 5;   // [9:5]
        uint32_t xtal26m_ts_en : 1;     // [10]
        uint32_t xtal26m_pllcal_en : 1; // [11]
        uint32_t __31_12 : 20;          // [31:12]
    } b;
} REG_RF_RTC_REG_C8_REG_T;

// reg_cc_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xdrv_reg_bit : 4;         // [3:0]
        uint32_t xdrv_aux1_power_bit : 3;  // [6:4]
        uint32_t xdrv_aux2_power_bit : 3;  // [9:7]
        uint32_t xdrv_pmic_power_bit : 3;  // [12:10]
        uint32_t xtal26m_interface_en : 1; // [13]
        uint32_t xtal26m_pmic_en : 1;      // [14]
        uint32_t __31_15 : 17;             // [31:15]
    } b;
} REG_RF_RTC_REG_CC_REG_T;

// reg_d0_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xtal_capbank_bit_n : 7; // [6:0]
        uint32_t xtal_hsel_n : 1;        // [7]
        uint32_t xtal_capbank_bit_l : 7; // [14:8]
        uint32_t xtal_hsel_l : 1;        // [15]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RF_RTC_REG_D0_REG_T;

// reg_d4_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_reser_l : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_RF_RTC_REG_D4_REG_T;

// reg_d8_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_reser_n : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_RF_RTC_REG_D8_REG_T;

// reg_00_reg
#define RF_RTC_REG_A1_BIT(n) (((n)&0xff) << 0)
#define RF_RTC_REG_A0_BIT(n) (((n)&0xff) << 8)

// reg_0c_reg
#define RF_RTC_REG_A7_BIT(n) (((n)&0xff) << 0)
#define RF_RTC_REG_A6_BIT(n) (((n)&0xff) << 8)

// reg_10_reg
#define RF_RTC_REG_A9_BIT(n) (((n)&0xff) << 0)
#define RF_RTC_REG_A8_BIT(n) (((n)&0xff) << 8)

// reg_14_reg
#define RF_RTC_REG_AB_BIT(n) (((n)&0xff) << 0)
#define RF_RTC_REG_AA_BIT(n) (((n)&0xff) << 8)

// reg_18_reg
#define RF_RTC_REG_AD_BIT(n) (((n)&0xff) << 0)
#define RF_RTC_REG_AC_BIT(n) (((n)&0xff) << 8)

// reg_1c_reg
#define RF_RTC_REG_AF_BIT(n) (((n)&0xff) << 0)
#define RF_RTC_REG_AE_BIT(n) (((n)&0xff) << 8)

// reg_c0_reg
#define RF_RTC_DIG_AFC_BIT_REG(n) (((n)&0x3fff) << 0)
#define RF_RTC_ENABLE_CLK_6P5M (1 << 14)

// reg_c4_reg
#define RF_RTC_XTAL_OSC_IBIT_N(n) (((n)&0xf) << 0)
#define RF_RTC_XTAL_OSC_IBIT_L(n) (((n)&0xf) << 4)
#define RF_RTC_XTAL26M_PWADC_EN (1 << 8)
#define RF_RTC_XTAL_REG_BIT(n) (((n)&0xf) << 9)
#define RF_RTC_XTAL26M_TSXADC_EN (1 << 13)
#define RF_RTC_XTAL26M_PLLS1_EN (1 << 14)
#define RF_RTC_XTAL26M_PLLS2_EN (1 << 15)

// reg_c8_reg
#define RF_RTC_XTAL_FIXI_BIT_N(n) (((n)&0x1f) << 0)
#define RF_RTC_XTAL_FIXI_BIT_L(n) (((n)&0x1f) << 5)
#define RF_RTC_XTAL26M_TS_EN (1 << 10)
#define RF_RTC_XTAL26M_PLLCAL_EN (1 << 11)

// reg_cc_reg
#define RF_RTC_XDRV_REG_BIT(n) (((n)&0xf) << 0)
#define RF_RTC_XDRV_AUX1_POWER_BIT(n) (((n)&0x7) << 4)
#define RF_RTC_XDRV_AUX2_POWER_BIT(n) (((n)&0x7) << 7)
#define RF_RTC_XDRV_PMIC_POWER_BIT(n) (((n)&0x7) << 10)
#define RF_RTC_XTAL26M_INTERFACE_EN (1 << 13)
#define RF_RTC_XTAL26M_PMIC_EN (1 << 14)

// reg_d0_reg
#define RF_RTC_XTAL_CAPBANK_BIT_N(n) (((n)&0x7f) << 0)
#define RF_RTC_XTAL_HSEL_N (1 << 7)
#define RF_RTC_XTAL_CAPBANK_BIT_L(n) (((n)&0x7f) << 8)
#define RF_RTC_XTAL_HSEL_L (1 << 15)

// reg_d4_reg
#define RF_RTC_RTC_RESER_L(n) (((n)&0xffff) << 0)

// reg_d8_reg
#define RF_RTC_RTC_RESER_N(n) (((n)&0xffff) << 0)

#endif // _RF_RTC_H_
