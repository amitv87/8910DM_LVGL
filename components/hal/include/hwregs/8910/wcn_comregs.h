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

#ifndef _WCN_COMREGS_H_
#define _WCN_COMREGS_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_WCN_COMREGS_WCN_BASE (0x1500b000)
#define REG_WCN_COMREGS_AP_BASE (0x1500c000)

typedef volatile struct
{
    uint32_t snapshot;     // 0x00000000
    uint32_t snapshot_cfg; // 0x00000004
    uint32_t cause;        // 0x00000008
    uint32_t mask_set;     // 0x0000000c
    uint32_t mask_clr;     // 0x00000010
    uint32_t itreg_set;    // 0x00000014
    uint32_t itreg_clr;    // 0x00000018
} HWP_WCN_COMREGS_T;

#define hwp_wcnComregsWcn ((HWP_WCN_COMREGS_T *)REG_ACCESS_ADDRESS(REG_WCN_COMREGS_WCN_BASE))
#define hwp_wcnComregsAp ((HWP_WCN_COMREGS_T *)REG_ACCESS_ADDRESS(REG_WCN_COMREGS_AP_BASE))

// snapshot
typedef union {
    uint32_t v;
    struct
    {
        uint32_t snapshot : 2; // [1:0], read only
        uint32_t __31_2 : 30;  // [31:2]
    } b;
} REG_WCN_COMREGS_SNAPSHOT_T;

// snapshot_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t snapshot_cfg : 1;    // [0]
        uint32_t snapshot_cfg_rd : 1; // [1], read only
        uint32_t __31_2 : 30;         // [31:2]
    } b;
} REG_WCN_COMREGS_SNAPSHOT_CFG_T;

// cause
typedef union {
    uint32_t v;
    struct
    {
        uint32_t irq0_cause : 8; // [7:0], read only
        uint32_t irq1_cause : 8; // [15:8], read only
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_WCN_COMREGS_CAUSE_T;

// mask_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t irq0_mask_set : 8; // [7:0], read only
        uint32_t irq1_mask_set : 8; // [15:8], read only
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_WCN_COMREGS_MASK_SET_T;

// mask_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t irq0_mask_clr : 8; // [7:0], read only
        uint32_t irq1_mask_clr : 8; // [15:8], read only
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_WCN_COMREGS_MASK_CLR_T;

// itreg_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t irq0_set : 8; // [7:0], read only
        uint32_t irq1_set : 8; // [15:8], read only
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_WCN_COMREGS_ITREG_SET_T;

// itreg_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t irq0_clr : 8; // [7:0], read only
        uint32_t irq1_clr : 8; // [15:8], read only
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_WCN_COMREGS_ITREG_CLR_T;

// snapshot
#define WCN_COMREGS_SNAPSHOT(n) (((n)&0x3) << 0)

// snapshot_cfg
#define WCN_COMREGS_SNAPSHOT_CFG (1 << 0)
#define WCN_COMREGS_SNAPSHOT_CFG_RD (1 << 1)

// cause
#define WCN_COMREGS_IRQ0_CAUSE(n) (((n)&0xff) << 0)
#define WCN_COMREGS_IRQ1_CAUSE(n) (((n)&0xff) << 8)

// mask_set
#define WCN_COMREGS_IRQ0_MASK_SET(n) (((n)&0xff) << 0)
#define WCN_COMREGS_IRQ1_MASK_SET(n) (((n)&0xff) << 8)

// mask_clr
#define WCN_COMREGS_IRQ0_MASK_CLR(n) (((n)&0xff) << 0)
#define WCN_COMREGS_IRQ1_MASK_CLR(n) (((n)&0xff) << 8)

// itreg_set
#define WCN_COMREGS_IRQ0_SET(n) (((n)&0xff) << 0)
#define WCN_COMREGS_IRQ1_SET(n) (((n)&0xff) << 8)

// itreg_clr
#define WCN_COMREGS_IRQ0_CLR(n) (((n)&0xff) << 0)
#define WCN_COMREGS_IRQ1_CLR(n) (((n)&0xff) << 8)

#endif // _WCN_COMREGS_H_
