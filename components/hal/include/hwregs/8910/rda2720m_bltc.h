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

#ifndef _RDA2720M_BLTC_H_
#define _RDA2720M_BLTC_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_BLTC_BASE (0x50308180)

typedef volatile struct
{
    uint32_t bltc_ctrl;        // 0x00000000
    uint32_t bltc_r_prescl;    // 0x00000004
    uint32_t bltc_r_duty;      // 0x00000008
    uint32_t bltc_r_curve0;    // 0x0000000c
    uint32_t bltc_r_curve1;    // 0x00000010
    uint32_t bltc_g_prescl;    // 0x00000014
    uint32_t bltc_g_duty;      // 0x00000018
    uint32_t bltc_g_curve0;    // 0x0000001c
    uint32_t bltc_g_curve1;    // 0x00000020
    uint32_t bltc_b_prescl;    // 0x00000024
    uint32_t bltc_b_duty;      // 0x00000028
    uint32_t bltc_b_curve0;    // 0x0000002c
    uint32_t bltc_b_curve1;    // 0x00000030
    uint32_t bltc_sts;         // 0x00000034
    uint32_t rg_rgb_v0;        // 0x00000038
    uint32_t rg_rgb_v1;        // 0x0000003c
    uint32_t rg_rgb_v2;        // 0x00000040
    uint32_t rg_rgb_v3;        // 0x00000044
    uint32_t bltc_wled_prescl; // 0x00000048
    uint32_t bltc_wled_duty;   // 0x0000004c
    uint32_t bltc_wled_curve0; // 0x00000050
    uint32_t bltc_wled_curve1; // 0x00000054
    uint32_t bltc_pd_ctrl;     // 0x00000058
    uint32_t bltc_version;     // 0x0000005c
} HWP_RDA2720M_BLTC_T;

#define hwp_rda2720mBltc ((HWP_RDA2720M_BLTC_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_BLTC_BASE))

// bltc_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t r_run : 1;     // [0]
        uint32_t r_type : 1;    // [1]
        uint32_t r_sel : 1;     // [2]
        uint32_t r_sw : 1;      // [3]
        uint32_t g_run : 1;     // [4]
        uint32_t g_type : 1;    // [5]
        uint32_t g_sel : 1;     // [6]
        uint32_t g_sw : 1;      // [7]
        uint32_t b_run : 1;     // [8]
        uint32_t b_type : 1;    // [9]
        uint32_t b_sel : 1;     // [10]
        uint32_t b_sw : 1;      // [11]
        uint32_t wled_run : 1;  // [12]
        uint32_t wled_type : 1; // [13]
        uint32_t wled_sel : 1;  // [14]
        uint32_t wled_sw : 1;   // [15]
        uint32_t __31_16 : 16;  // [31:16]
    } b;
} REG_RDA2720M_BLTC_BLTC_CTRL_T;

// bltc_r_prescl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prescl : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_RDA2720M_BLTC_BLTC_R_PRESCL_T;

// bltc_r_duty
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mod : 8;      // [7:0]
        uint32_t duty : 8;     // [15:8]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_RDA2720M_BLTC_BLTC_R_DUTY_T;

// bltc_r_curve0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t trise : 6;    // [5:0]
        uint32_t __7_6 : 2;    // [7:6]
        uint32_t tfall : 6;    // [13:8]
        uint32_t __31_14 : 18; // [31:14]
    } b;
} REG_RDA2720M_BLTC_BLTC_R_CURVE0_T;

// bltc_r_curve1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t thigh : 8;    // [7:0]
        uint32_t tlow : 8;     // [15:8]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_RDA2720M_BLTC_BLTC_R_CURVE1_T;

// bltc_g_prescl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prescl : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_RDA2720M_BLTC_BLTC_G_PRESCL_T;

// bltc_g_duty
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mod : 8;      // [7:0]
        uint32_t duty : 8;     // [15:8]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_RDA2720M_BLTC_BLTC_G_DUTY_T;

// bltc_g_curve0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t trise : 6;    // [5:0]
        uint32_t __7_6 : 2;    // [7:6]
        uint32_t tfall : 6;    // [13:8]
        uint32_t __31_14 : 18; // [31:14]
    } b;
} REG_RDA2720M_BLTC_BLTC_G_CURVE0_T;

// bltc_g_curve1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t thigh : 8;    // [7:0]
        uint32_t tlow : 8;     // [15:8]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_RDA2720M_BLTC_BLTC_G_CURVE1_T;

// bltc_b_prescl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prescl : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_RDA2720M_BLTC_BLTC_B_PRESCL_T;

// bltc_b_duty
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mod : 8;      // [7:0]
        uint32_t duty : 8;     // [15:8]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_RDA2720M_BLTC_BLTC_B_DUTY_T;

// bltc_b_curve0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t trise : 6;    // [5:0]
        uint32_t __7_6 : 2;    // [7:6]
        uint32_t tfall : 6;    // [13:8]
        uint32_t __31_14 : 18; // [31:14]
    } b;
} REG_RDA2720M_BLTC_BLTC_B_CURVE0_T;

// bltc_b_curve1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t thigh : 8;    // [7:0]
        uint32_t tlow : 8;     // [15:8]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_RDA2720M_BLTC_BLTC_B_CURVE1_T;

// bltc_sts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bltc_r_busy : 1;    // [0], read only
        uint32_t bltc_g_busy : 1;    // [1], read only
        uint32_t bltc_b_busy : 1;    // [2], read only
        uint32_t bltc_wled_busy : 1; // [3], read only
        uint32_t __31_4 : 28;        // [31:4]
    } b;
} REG_RDA2720M_BLTC_BLTC_STS_T;

// rg_rgb_v0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_rgb_v0 : 6; // [5:0]
        uint32_t __31_6 : 26;   // [31:6]
    } b;
} REG_RDA2720M_BLTC_RG_RGB_V0_T;

// rg_rgb_v1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_rgb_v1 : 6; // [5:0]
        uint32_t __31_6 : 26;   // [31:6]
    } b;
} REG_RDA2720M_BLTC_RG_RGB_V1_T;

// rg_rgb_v2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_rgb_v2 : 6; // [5:0]
        uint32_t __31_6 : 26;   // [31:6]
    } b;
} REG_RDA2720M_BLTC_RG_RGB_V2_T;

// rg_rgb_v3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_rgb_v3 : 6; // [5:0]
        uint32_t __31_6 : 26;   // [31:6]
    } b;
} REG_RDA2720M_BLTC_RG_RGB_V3_T;

// bltc_wled_prescl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prescl : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_RDA2720M_BLTC_BLTC_WLED_PRESCL_T;

// bltc_wled_duty
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mod : 8;      // [7:0]
        uint32_t duty : 8;     // [15:8]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_RDA2720M_BLTC_BLTC_WLED_DUTY_T;

// bltc_wled_curve0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t trise : 6;    // [5:0]
        uint32_t __7_6 : 2;    // [7:6]
        uint32_t tfall : 6;    // [13:8]
        uint32_t __31_14 : 18; // [31:14]
    } b;
} REG_RDA2720M_BLTC_BLTC_WLED_CURVE0_T;

// bltc_wled_curve1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t thigh : 8;    // [7:0]
        uint32_t tlow : 8;     // [15:8]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_RDA2720M_BLTC_BLTC_WLED_CURVE1_T;

// bltc_pd_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sw_pd : 1;   // [0]
        uint32_t hw_pd : 1;   // [1]
        uint32_t __31_2 : 30; // [31:2]
    } b;
} REG_RDA2720M_BLTC_BLTC_PD_CTRL_T;

// bltc_version
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bltc_version : 16; // [15:0], read only
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_RDA2720M_BLTC_BLTC_VERSION_T;

// bltc_ctrl
#define RDA2720M_BLTC_R_RUN (1 << 0)
#define RDA2720M_BLTC_R_TYPE (1 << 1)
#define RDA2720M_BLTC_R_SEL (1 << 2)
#define RDA2720M_BLTC_R_SW (1 << 3)
#define RDA2720M_BLTC_G_RUN (1 << 4)
#define RDA2720M_BLTC_G_TYPE (1 << 5)
#define RDA2720M_BLTC_G_SEL (1 << 6)
#define RDA2720M_BLTC_G_SW (1 << 7)
#define RDA2720M_BLTC_B_RUN (1 << 8)
#define RDA2720M_BLTC_B_TYPE (1 << 9)
#define RDA2720M_BLTC_B_SEL (1 << 10)
#define RDA2720M_BLTC_B_SW (1 << 11)
#define RDA2720M_BLTC_WLED_RUN (1 << 12)
#define RDA2720M_BLTC_WLED_TYPE (1 << 13)
#define RDA2720M_BLTC_WLED_SEL (1 << 14)
#define RDA2720M_BLTC_WLED_SW (1 << 15)

// bltc_r_prescl
#define RDA2720M_BLTC_PRESCL(n) (((n)&0xff) << 0)

// bltc_r_duty
#define RDA2720M_BLTC_MOD(n) (((n)&0xff) << 0)
#define RDA2720M_BLTC_DUTY(n) (((n)&0xff) << 8)

// bltc_r_curve0
#define RDA2720M_BLTC_TRISE(n) (((n)&0x3f) << 0)
#define RDA2720M_BLTC_TFALL(n) (((n)&0x3f) << 8)

// bltc_r_curve1
#define RDA2720M_BLTC_THIGH(n) (((n)&0xff) << 0)
#define RDA2720M_BLTC_TLOW(n) (((n)&0xff) << 8)

// bltc_g_prescl
#define RDA2720M_BLTC_PRESCL(n) (((n)&0xff) << 0)

// bltc_g_duty
#define RDA2720M_BLTC_MOD(n) (((n)&0xff) << 0)
#define RDA2720M_BLTC_DUTY(n) (((n)&0xff) << 8)

// bltc_g_curve0
#define RDA2720M_BLTC_TRISE(n) (((n)&0x3f) << 0)
#define RDA2720M_BLTC_TFALL(n) (((n)&0x3f) << 8)

// bltc_g_curve1
#define RDA2720M_BLTC_THIGH(n) (((n)&0xff) << 0)
#define RDA2720M_BLTC_TLOW(n) (((n)&0xff) << 8)

// bltc_b_prescl
#define RDA2720M_BLTC_PRESCL(n) (((n)&0xff) << 0)

// bltc_b_duty
#define RDA2720M_BLTC_MOD(n) (((n)&0xff) << 0)
#define RDA2720M_BLTC_DUTY(n) (((n)&0xff) << 8)

// bltc_b_curve0
#define RDA2720M_BLTC_TRISE(n) (((n)&0x3f) << 0)
#define RDA2720M_BLTC_TFALL(n) (((n)&0x3f) << 8)

// bltc_b_curve1
#define RDA2720M_BLTC_THIGH(n) (((n)&0xff) << 0)
#define RDA2720M_BLTC_TLOW(n) (((n)&0xff) << 8)

// bltc_sts
#define RDA2720M_BLTC_BLTC_R_BUSY (1 << 0)
#define RDA2720M_BLTC_BLTC_G_BUSY (1 << 1)
#define RDA2720M_BLTC_BLTC_B_BUSY (1 << 2)
#define RDA2720M_BLTC_BLTC_WLED_BUSY (1 << 3)

// rg_rgb_v0
#define RDA2720M_BLTC_RG_RGB_V0(n) (((n)&0x3f) << 0)

// rg_rgb_v1
#define RDA2720M_BLTC_RG_RGB_V1(n) (((n)&0x3f) << 0)

// rg_rgb_v2
#define RDA2720M_BLTC_RG_RGB_V2(n) (((n)&0x3f) << 0)

// rg_rgb_v3
#define RDA2720M_BLTC_RG_RGB_V3(n) (((n)&0x3f) << 0)

// bltc_wled_prescl
#define RDA2720M_BLTC_PRESCL(n) (((n)&0xff) << 0)

// bltc_wled_duty
#define RDA2720M_BLTC_MOD(n) (((n)&0xff) << 0)
#define RDA2720M_BLTC_DUTY(n) (((n)&0xff) << 8)

// bltc_wled_curve0
#define RDA2720M_BLTC_TRISE(n) (((n)&0x3f) << 0)
#define RDA2720M_BLTC_TFALL(n) (((n)&0x3f) << 8)

// bltc_wled_curve1
#define RDA2720M_BLTC_THIGH(n) (((n)&0xff) << 0)
#define RDA2720M_BLTC_TLOW(n) (((n)&0xff) << 8)

// bltc_pd_ctrl
#define RDA2720M_BLTC_SW_PD (1 << 0)
#define RDA2720M_BLTC_HW_PD (1 << 1)

// bltc_version
#define RDA2720M_BLTC_BLTC_VERSION(n) (((n)&0xffff) << 0)

#endif // _RDA2720M_BLTC_H_
