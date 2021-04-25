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

#ifndef _TIMER_AP_H_
#define _TIMER_AP_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_TIMER4_BASE (0x0880e000)
#if defined(REG_ADDRESS_FOR_GGE)
#define REG_TIMER3_BASE (0x05105000)
#else
#define REG_TIMER3_BASE (0x50105000)
#endif

typedef volatile struct
{
    uint32_t ostimer_loadval_l;  // 0x00000000
    uint32_t ostimer_ctrl;       // 0x00000004
    uint32_t ostimer_curval_l;   // 0x00000008
    uint32_t ostimer_curval_h;   // 0x0000000c
    uint32_t ostimer_lockval_l;  // 0x00000010
    uint32_t ostimer_lockval_h;  // 0x00000014
    uint32_t hwtimer_ctrl;       // 0x00000018
    uint32_t hwtimer_curval_l;   // 0x0000001c
    uint32_t hwtimer_curval_h;   // 0x00000020
    uint32_t hwtimer_lockval_l;  // 0x00000024
    uint32_t hwtimer_lockval_h;  // 0x00000028
    uint32_t timer_irq_mask_set; // 0x0000002c
    uint32_t timer_irq_mask_clr; // 0x00000030
    uint32_t timer_irq_clr;      // 0x00000034
    uint32_t timer_irq_cause;    // 0x00000038
} HWP_TIMER_AP_T;

#define hwp_timer4 ((HWP_TIMER_AP_T *)REG_ACCESS_ADDRESS(REG_TIMER4_BASE))
#define hwp_timer3 ((HWP_TIMER_AP_T *)REG_ACCESS_ADDRESS(REG_TIMER3_BASE))

// ostimer_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t loadval_h : 24; // [23:0]
        uint32_t enable : 1;     // [24]
        uint32_t enabled : 1;    // [25], read only
        uint32_t cleared : 1;    // [26], read only
        uint32_t __27_27 : 1;    // [27]
        uint32_t repeat : 1;     // [28]
        uint32_t wrap : 1;       // [29]
        uint32_t load : 1;       // [30]
        uint32_t __31_31 : 1;    // [31]
    } b;
} REG_TIMER_AP_OSTIMER_CTRL_T;

// hwtimer_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t interval : 2;    // [1:0]
        uint32_t __7_2 : 6;       // [7:2]
        uint32_t interval_en : 1; // [8]
        uint32_t __31_9 : 23;     // [31:9]
    } b;
} REG_TIMER_AP_HWTIMER_CTRL_T;

// timer_irq_mask_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ostimer_mask : 1;      // [0], write set
        uint32_t hwtimer_wrap_mask : 1; // [1], write set
        uint32_t hwtimer_itv_mask : 1;  // [2], write set
        uint32_t __31_3 : 29;           // [31:3]
    } b;
} REG_TIMER_AP_TIMER_IRQ_MASK_SET_T;

// timer_irq_mask_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ostimer_mask : 1;      // [0], write clear
        uint32_t hwtimer_wrap_mask : 1; // [1], write clear
        uint32_t hwtimer_itv_mask : 1;  // [2], write clear
        uint32_t __31_3 : 29;           // [31:3]
    } b;
} REG_TIMER_AP_TIMER_IRQ_MASK_CLR_T;

// timer_irq_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ostimer_clr : 1;      // [0], write clear
        uint32_t hwtimer_wrap_clr : 1; // [1], write clear
        uint32_t hwtimer_itv_clr : 1;  // [2], write clear
        uint32_t __31_3 : 29;          // [31:3]
    } b;
} REG_TIMER_AP_TIMER_IRQ_CLR_T;

// timer_irq_cause
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ostimer_cause : 1;       // [0], read only
        uint32_t hwtimer_wrap_cause : 1;  // [1], read only
        uint32_t hwtimer_itv_cause : 1;   // [2], read only
        uint32_t __15_3 : 13;             // [15:3]
        uint32_t ostimer_status : 1;      // [16], read only
        uint32_t hwtimer_wrap_status : 1; // [17], read only
        uint32_t hwtimer_itv_status : 1;  // [18], read only
        uint32_t __31_19 : 13;            // [31:19]
    } b;
} REG_TIMER_AP_TIMER_IRQ_CAUSE_T;

// ostimer_loadval_l
#define TIMER_AP_LOADVAL_L(n) (((n)&0xffffffff) << 0)

// ostimer_ctrl
#define TIMER_AP_LOADVAL_H(n) (((n)&0xffffff) << 0)
#define TIMER_AP_ENABLE (1 << 24)
#define TIMER_AP_ENABLED (1 << 25)
#define TIMER_AP_CLEARED (1 << 26)
#define TIMER_AP_REPEAT (1 << 28)
#define TIMER_AP_WRAP (1 << 29)
#define TIMER_AP_LOAD (1 << 30)

// ostimer_curval_l
#define TIMER_AP_CURVAL_L(n) (((n)&0xffffffff) << 0)

// ostimer_curval_h
#define TIMER_AP_CURVAL_H(n) (((n)&0xffffffff) << 0)

// ostimer_lockval_l
#define TIMER_AP_LOCKVAL_L(n) (((n)&0xffffffff) << 0)

// ostimer_lockval_h
#define TIMER_AP_LOCKVAL_H(n) (((n)&0xffffffff) << 0)

// hwtimer_ctrl
#define TIMER_AP_INTERVAL(n) (((n)&0x3) << 0)
#define TIMER_AP_INTERVAL_EN (1 << 8)

// hwtimer_curval_l
#define TIMER_AP_CURVAL_L(n) (((n)&0xffffffff) << 0)

// hwtimer_curval_h
#define TIMER_AP_CURVAL_H(n) (((n)&0xffffffff) << 0)

// hwtimer_lockval_l
#define TIMER_AP_LOCKVAL_L(n) (((n)&0xffffffff) << 0)

// hwtimer_lockval_h
#define TIMER_AP_LOCKVAL_H(n) (((n)&0xffffffff) << 0)

// timer_irq_mask_set
#define TIMER_AP_OSTIMER_MASK (1 << 0)
#define TIMER_AP_HWTIMER_WRAP_MASK (1 << 1)
#define TIMER_AP_HWTIMER_ITV_MASK (1 << 2)

// timer_irq_mask_clr
#define TIMER_AP_OSTIMER_MASK (1 << 0)
#define TIMER_AP_HWTIMER_WRAP_MASK (1 << 1)
#define TIMER_AP_HWTIMER_ITV_MASK (1 << 2)

// timer_irq_clr
#define TIMER_AP_OSTIMER_CLR (1 << 0)
#define TIMER_AP_HWTIMER_WRAP_CLR (1 << 1)
#define TIMER_AP_HWTIMER_ITV_CLR (1 << 2)

// timer_irq_cause
#define TIMER_AP_OTHER_TIMS_IRQ(n) (((n)&0x3) << 1)
#define TIMER_AP_OSTIMER_CAUSE (1 << 0)
#define TIMER_AP_HWTIMER_WRAP_CAUSE (1 << 1)
#define TIMER_AP_HWTIMER_ITV_CAUSE (1 << 2)
#define TIMER_AP_OSTIMER_STATUS (1 << 16)
#define TIMER_AP_HWTIMER_WRAP_STATUS (1 << 17)
#define TIMER_AP_HWTIMER_ITV_STATUS (1 << 18)

#endif // _TIMER_AP_H_
