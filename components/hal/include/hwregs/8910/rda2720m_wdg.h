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

#ifndef _RDA2720M_WDG_H_
#define _RDA2720M_WDG_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_WDG_BASE (0x50308040)

typedef volatile struct
{
    uint32_t wdg_load_low;      // 0x00000000
    uint32_t wdg_load_high;     // 0x00000004
    uint32_t wdg_ctrl;          // 0x00000008
    uint32_t wdg_int_clr;       // 0x0000000c
    uint32_t wdg_int_raw;       // 0x00000010
    uint32_t wdg_int_mask;      // 0x00000014
    uint32_t wdg_cnt_low;       // 0x00000018
    uint32_t wdg_cnt_high;      // 0x0000001c
    uint32_t wdg_lock;          // 0x00000020
    uint32_t wdg_cnt_rd_low;    // 0x00000024
    uint32_t wdg_cnt_rd_high;   // 0x00000028
    uint32_t wdg_irqvalue_low;  // 0x0000002c
    uint32_t wdg_irqvalue_high; // 0x00000030
} HWP_RDA2720M_WDG_T;

#define hwp_rda2720mWdg ((HWP_RDA2720M_WDG_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_WDG_BASE))

// wdg_load_low
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_load_low : 16; // [15:0]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_RDA2720M_WDG_WDG_LOAD_LOW_T;

// wdg_load_high
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_load_high : 16; // [15:0]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_WDG_WDG_LOAD_HIGH_T;

// wdg_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_irq_en : 1; // [0]
        uint32_t wdg_run : 1;    // [1]
        uint32_t wdg_new : 1;    // [2]
        uint32_t wdg_rst_en : 1; // [3]
        uint32_t __31_4 : 28;    // [31:4]
    } b;
} REG_RDA2720M_WDG_WDG_CTRL_T;

// wdg_int_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_int_clr : 1; // [0]
        uint32_t __2_1 : 2;       // [2:1]
        uint32_t wdg_rst_clr : 1; // [3]
        uint32_t __31_4 : 28;     // [31:4]
    } b;
} REG_RDA2720M_WDG_WDG_INT_CLR_T;

// wdg_int_raw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_int_raw : 1; // [0], read only
        uint32_t __2_1 : 2;       // [2:1]
        uint32_t wdg_rst_raw : 1; // [3], read only
        uint32_t wdg_ld_busy : 1; // [4], read only
        uint32_t __31_5 : 27;     // [31:5]
    } b;
} REG_RDA2720M_WDG_WDG_INT_RAW_T;

// wdg_int_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_int_mask : 1; // [0], read only
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_RDA2720M_WDG_WDG_INT_MASK_T;

// wdg_cnt_low
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_cnt_low : 16; // [15:0], read only
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_RDA2720M_WDG_WDG_CNT_LOW_T;

// wdg_cnt_high
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_cnt_high : 16; // [15:0], read only
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_RDA2720M_WDG_WDG_CNT_HIGH_T;

// wdg_lock
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_lock : 16; // [15:0]
        uint32_t __31_16 : 16;  // [31:16]
    } b;
} REG_RDA2720M_WDG_WDG_LOCK_T;

// wdg_cnt_rd_low
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_cnt_rd_low : 16; // [15:0], read only
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_RDA2720M_WDG_WDG_CNT_RD_LOW_T;

// wdg_cnt_rd_high
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_cnt_rd_high : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_RDA2720M_WDG_WDG_CNT_RD_HIGH_T;

// wdg_irqvalue_low
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_irqvalue_low : 16; // [15:0]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RDA2720M_WDG_WDG_IRQVALUE_LOW_T;

// wdg_irqvalue_high
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_irqvalue_high : 16; // [15:0]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_WDG_WDG_IRQVALUE_HIGH_T;

// wdg_load_low
#define RDA2720M_WDG_WDG_LOAD_LOW(n) (((n)&0xffff) << 0)

// wdg_load_high
#define RDA2720M_WDG_WDG_LOAD_HIGH(n) (((n)&0xffff) << 0)

// wdg_ctrl
#define RDA2720M_WDG_WDG_IRQ_EN (1 << 0)
#define RDA2720M_WDG_WDG_RUN (1 << 1)
#define RDA2720M_WDG_WDG_NEW (1 << 2)
#define RDA2720M_WDG_WDG_RST_EN (1 << 3)

// wdg_int_clr
#define RDA2720M_WDG_WDG_INT_CLR (1 << 0)
#define RDA2720M_WDG_WDG_RST_CLR (1 << 3)

// wdg_int_raw
#define RDA2720M_WDG_WDG_INT_RAW (1 << 0)
#define RDA2720M_WDG_WDG_RST_RAW (1 << 3)
#define RDA2720M_WDG_WDG_LD_BUSY (1 << 4)

// wdg_int_mask
#define RDA2720M_WDG_WDG_INT_MASK (1 << 0)

// wdg_cnt_low
#define RDA2720M_WDG_WDG_CNT_LOW(n) (((n)&0xffff) << 0)

// wdg_cnt_high
#define RDA2720M_WDG_WDG_CNT_HIGH(n) (((n)&0xffff) << 0)

// wdg_lock
#define RDA2720M_WDG_WDG_LOCK(n) (((n)&0xffff) << 0)

// wdg_cnt_rd_low
#define RDA2720M_WDG_WDG_CNT_RD_LOW(n) (((n)&0xffff) << 0)

// wdg_cnt_rd_high
#define RDA2720M_WDG_WDG_CNT_RD_HIGH(n) (((n)&0xffff) << 0)

// wdg_irqvalue_low
#define RDA2720M_WDG_WDG_IRQVALUE_LOW(n) (((n)&0xffff) << 0)

// wdg_irqvalue_high
#define RDA2720M_WDG_WDG_IRQVALUE_HIGH(n) (((n)&0xffff) << 0)

#endif // _RDA2720M_WDG_H_
