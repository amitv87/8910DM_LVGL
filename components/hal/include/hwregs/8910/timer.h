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

#ifndef _TIMER_H_
#define _TIMER_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define NB_INTERVAL (1)
#define INT_TIMER_NB_BITS (24)
#define WD_TIMER_NB_BITS (24)
#define HW_TIMER_NB_BITS (32)
#define TIM_MAXVAL (0xffffff)

#define REG_TIMER1_BASE (0x08808000)
#define REG_TIMER2_BASE (0x08809000)

typedef volatile struct
{
    uint32_t ostimer_ctrl;       // 0x00000000
    uint32_t ostimer_curval;     // 0x00000004
    uint32_t wdtimer_ctrl;       // 0x00000008
    uint32_t wdtimer_loadval;    // 0x0000000c
    uint32_t hwtimer_ctrl;       // 0x00000010
    uint32_t hwtimer_curval;     // 0x00000014
    uint32_t timer_irq_mask_set; // 0x00000018
    uint32_t timer_irq_mask_clr; // 0x0000001c
    uint32_t timer_irq_clr;      // 0x00000020
    uint32_t timer_irq_cause;    // 0x00000024
} HWP_TIMER_T;

#define hwp_timer1 ((HWP_TIMER_T *)REG_ACCESS_ADDRESS(REG_TIMER1_BASE))
#define hwp_timer2 ((HWP_TIMER_T *)REG_ACCESS_ADDRESS(REG_TIMER2_BASE))

// ostimer_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t loadval : 24; // [23:0]
        uint32_t enable : 1;   // [24]
        uint32_t enabled : 1;  // [25], read only
        uint32_t cleared : 1;  // [26], read only
        uint32_t __27_27 : 1;  // [27]
        uint32_t repeat : 1;   // [28]
        uint32_t wrap : 1;     // [29]
        uint32_t load : 1;     // [30]
        uint32_t __31_31 : 1;  // [31]
    } b;
} REG_TIMER_OSTIMER_CTRL_T;

// wdtimer_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;     // [0], write set
        uint32_t __3_1 : 3;     // [3:1]
        uint32_t stop : 1;      // [4], write clear
        uint32_t __7_5 : 3;     // [7:5]
        uint32_t wdenabled : 1; // [8], read only
        uint32_t __15_9 : 7;    // [15:9]
        uint32_t reload : 1;    // [16]
        uint32_t __31_17 : 15;  // [31:17]
    } b;
} REG_TIMER_WDTIMER_CTRL_T;

// wdtimer_loadval
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdloadval : 24; // [23:0]
        uint32_t __31_24 : 8;    // [31:24]
    } b;
} REG_TIMER_WDTIMER_LOADVAL_T;

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
} REG_TIMER_HWTIMER_CTRL_T;

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
} REG_TIMER_TIMER_IRQ_MASK_SET_T;

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
} REG_TIMER_TIMER_IRQ_MASK_CLR_T;

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
} REG_TIMER_TIMER_IRQ_CLR_T;

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
} REG_TIMER_TIMER_IRQ_CAUSE_T;

// ostimer_ctrl
#define TIMER_LOADVAL(n) (((n)&0xffffff) << 0)
#define TIMER_ENABLE (1 << 24)
#define TIMER_ENABLED (1 << 25)
#define TIMER_CLEARED (1 << 26)
#define TIMER_REPEAT (1 << 28)
#define TIMER_WRAP (1 << 29)
#define TIMER_LOAD (1 << 30)

// ostimer_curval
#define TIMER_CURVAL(n) (((n)&0xffffffff) << 0)

// wdtimer_ctrl
#define TIMER_START (1 << 0)
#define TIMER_STOP (1 << 4)
#define TIMER_WDENABLED (1 << 8)
#define TIMER_RELOAD (1 << 16)

// wdtimer_loadval
#define TIMER_WDLOADVAL(n) (((n)&0xffffff) << 0)

// hwtimer_ctrl
#define TIMER_INTERVAL(n) (((n)&0x3) << 0)
#define TIMER_INTERVAL_EN (1 << 8)

// hwtimer_curval
#define TIMER_CURVAL(n) (((n)&0xffffffff) << 0)

// timer_irq_mask_set
#define TIMER_OSTIMER_MASK (1 << 0)
#define TIMER_HWTIMER_WRAP_MASK (1 << 1)
#define TIMER_HWTIMER_ITV_MASK (1 << 2)

// timer_irq_mask_clr
#define TIMER_OSTIMER_MASK (1 << 0)
#define TIMER_HWTIMER_WRAP_MASK (1 << 1)
#define TIMER_HWTIMER_ITV_MASK (1 << 2)

// timer_irq_clr
#define TIMER_OSTIMER_CLR (1 << 0)
#define TIMER_HWTIMER_WRAP_CLR (1 << 1)
#define TIMER_HWTIMER_ITV_CLR (1 << 2)

// timer_irq_cause
#define TIMER_OTHER_TIMS_IRQ(n) (((n)&0x3) << 1)
#define TIMER_OSTIMER_CAUSE (1 << 0)
#define TIMER_HWTIMER_WRAP_CAUSE (1 << 1)
#define TIMER_HWTIMER_ITV_CAUSE (1 << 2)
#define TIMER_OSTIMER_STATUS (1 << 16)
#define TIMER_HWTIMER_WRAP_STATUS (1 << 17)
#define TIMER_HWTIMER_ITV_STATUS (1 << 18)

#endif // _TIMER_H_
