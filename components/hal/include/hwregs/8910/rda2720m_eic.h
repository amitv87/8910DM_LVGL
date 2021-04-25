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

#ifndef _RDA2720M_EIC_H_
#define _RDA2720M_EIC_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_EIC_BASE (0x50308280)

typedef volatile struct
{
    uint32_t eicdata;   // 0x00000000
    uint32_t eicdmsk;   // 0x00000004
    uint32_t reserved1; // 0x00000008
    uint32_t reserved2; // 0x0000000c
    uint32_t reserved3; // 0x00000010
    uint32_t eiciev;    // 0x00000014
    uint32_t eicie;     // 0x00000018
    uint32_t eicris;    // 0x0000001c
    uint32_t eicmis;    // 0x00000020
    uint32_t eicic;     // 0x00000024
    uint32_t eictrig;   // 0x00000028
    uint32_t reserved4; // 0x0000002c
    uint32_t reserved5; // 0x00000030
    uint32_t reserved6; // 0x00000034
    uint32_t reserved7; // 0x00000038
    uint32_t reserved8; // 0x0000003c
    uint32_t eic0ctrl;  // 0x00000040
    uint32_t eic1ctrl;  // 0x00000044
    uint32_t eic2ctrl;  // 0x00000048
    uint32_t eic3ctrl;  // 0x0000004c
    uint32_t eic4ctrl;  // 0x00000050
    uint32_t eic5ctrl;  // 0x00000054
    uint32_t eic6ctrl;  // 0x00000058
    uint32_t eic7ctrl;  // 0x0000005c
    uint32_t eic8ctrl;  // 0x00000060
    uint32_t eic9ctrl;  // 0x00000064
    uint32_t eic10ctrl; // 0x00000068
    uint32_t eic11ctrl; // 0x0000006c
    uint32_t eic12ctrl; // 0x00000070
    uint32_t eic13ctrl; // 0x00000074
    uint32_t eic14ctrl; // 0x00000078
    uint32_t eic15ctrl; // 0x0000007c
} HWP_RDA2720M_EIC_T;

#define hwp_rda2720mEic ((HWP_RDA2720M_EIC_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_EIC_BASE))

// eicdata
typedef union {
    uint32_t v;
    struct
    {
        uint32_t eicdata : 13; // [12:0], read only
        uint32_t __31_13 : 19; // [31:13]
    } b;
} REG_RDA2720M_EIC_EICDATA_T;

// eicdmsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t eicdmsk : 13; // [12:0]
        uint32_t __31_13 : 19; // [31:13]
    } b;
} REG_RDA2720M_EIC_EICDMSK_T;

// eiciev
typedef union {
    uint32_t v;
    struct
    {
        uint32_t eiciev : 13;  // [12:0]
        uint32_t __31_13 : 19; // [31:13]
    } b;
} REG_RDA2720M_EIC_EICIEV_T;

// eicie
typedef union {
    uint32_t v;
    struct
    {
        uint32_t eicie : 13;   // [12:0]
        uint32_t __31_13 : 19; // [31:13]
    } b;
} REG_RDA2720M_EIC_EICIE_T;

// eicris
typedef union {
    uint32_t v;
    struct
    {
        uint32_t eicris : 13;  // [12:0], read only
        uint32_t __31_13 : 19; // [31:13]
    } b;
} REG_RDA2720M_EIC_EICRIS_T;

// eicmis
typedef union {
    uint32_t v;
    struct
    {
        uint32_t eicmis : 13;  // [12:0], read only
        uint32_t __31_13 : 19; // [31:13]
    } b;
} REG_RDA2720M_EIC_EICMIS_T;

// eicic
typedef union {
    uint32_t v;
    struct
    {
        uint32_t eicic : 13;   // [12:0]
        uint32_t __31_13 : 19; // [31:13]
    } b;
} REG_RDA2720M_EIC_EICIC_T;

// eictrig
typedef union {
    uint32_t v;
    struct
    {
        uint32_t eictrig : 13; // [12:0]
        uint32_t __31_13 : 19; // [31:13]
    } b;
} REG_RDA2720M_EIC_EICTRIG_T;

// eic0ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC0CTRL_T;

// eic1ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC1CTRL_T;

// eic2ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC2CTRL_T;

// eic3ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC3CTRL_T;

// eic4ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC4CTRL_T;

// eic5ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC5CTRL_T;

// eic6ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC6CTRL_T;

// eic7ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC7CTRL_T;

// eic8ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC8CTRL_T;

// eic9ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC9CTRL_T;

// eic10ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC10CTRL_T;

// eic11ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC11CTRL_T;

// eic12ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC12CTRL_T;

// eic13ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC13CTRL_T;

// eic14ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC14CTRL_T;

// eic15ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbnc_cnt : 12;      // [11:0]
        uint32_t __13_12 : 2;        // [13:12]
        uint32_t dbnc_en : 1;        // [14]
        uint32_t force_clk_dbnc : 1; // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EIC_EIC15CTRL_T;

// eicdata
#define RDA2720M_EIC_EICDATA(n) (((n)&0x1fff) << 0)

// eicdmsk
#define RDA2720M_EIC_EICDMSK(n) (((n)&0x1fff) << 0)

// eiciev
#define RDA2720M_EIC_EICIEV(n) (((n)&0x1fff) << 0)

// eicie
#define RDA2720M_EIC_EICIE(n) (((n)&0x1fff) << 0)

// eicris
#define RDA2720M_EIC_EICRIS(n) (((n)&0x1fff) << 0)

// eicmis
#define RDA2720M_EIC_EICMIS(n) (((n)&0x1fff) << 0)

// eicic
#define RDA2720M_EIC_EICIC(n) (((n)&0x1fff) << 0)

// eictrig
#define RDA2720M_EIC_EICTRIG(n) (((n)&0x1fff) << 0)

// eic0ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic1ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic2ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic3ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic4ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic5ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic6ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic7ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic8ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic9ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic10ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic11ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic12ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic13ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic14ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

// eic15ctrl
#define RDA2720M_EIC_DBNC_CNT(n) (((n)&0xfff) << 0)
#define RDA2720M_EIC_DBNC_EN (1 << 14)
#define RDA2720M_EIC_FORCE_CLK_DBNC (1 << 15)

#endif // _RDA2720M_EIC_H_
