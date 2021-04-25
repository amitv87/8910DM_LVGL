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

#ifndef _KEYPAD_H_
#define _KEYPAD_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!


#define LOW_KEY_NB (32)
#define HIGH_KEY_NB (32)

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_KEYPAD_BASE (0x05106000)
#else
#define REG_KEYPAD_BASE (0x50106000)
#endif

typedef volatile struct
{
    uint32_t kp_data_l;    // 0x00000000
    uint32_t kp_data_h;    // 0x00000004
    uint32_t kp_status;    // 0x00000008
    uint32_t kp_ctrl;      // 0x0000000c
    uint32_t kp_irq_mask;  // 0x00000010
    uint32_t kp_irq_cause; // 0x00000014
    uint32_t kp_irq_clr;   // 0x00000018
} HWP_KEYPAD_T;

#define hwp_keypad ((HWP_KEYPAD_T *)REG_ACCESS_ADDRESS(REG_KEYPAD_BASE))

// kp_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t keyin_status : 8; // [7:0], read only
        uint32_t __30_8 : 23;      // [30:8]
        uint32_t kp_on : 1;        // [31], read only
    } b;
} REG_KEYPAD_KP_STATUS_T;

// kp_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t kp_en : 1;       // [0]
        uint32_t __1_1 : 1;       // [1]
        uint32_t kp_dbn_time : 8; // [9:2]
        uint32_t kp_itv_time : 6; // [15:10]
        uint32_t kp_in_mask : 8;  // [23:16]
        uint32_t kp_out_mask : 8; // [31:24]
    } b;
} REG_KEYPAD_KP_CTRL_T;

// kp_irq_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t kp_evt0_irq_mask : 1; // [0]
        uint32_t kp_evt1_irq_mask : 1; // [1]
        uint32_t kp_itv_irq_mask : 1;  // [2]
        uint32_t __31_3 : 29;          // [31:3]
    } b;
} REG_KEYPAD_KP_IRQ_MASK_T;

// kp_irq_cause
typedef union {
    uint32_t v;
    struct
    {
        uint32_t kp_evt0_irq_cause : 1;  // [0], read only
        uint32_t kp_evt1_irq_cause : 1;  // [1], read only
        uint32_t kp_itv_irq_cause : 1;   // [2], read only
        uint32_t __15_3 : 13;            // [15:3]
        uint32_t kp_evt0_irq_status : 1; // [16], read only
        uint32_t kp_evt1_irq_status : 1; // [17], read only
        uint32_t kp_itv_irq_status : 1;  // [18], read only
        uint32_t __31_19 : 13;           // [31:19]
    } b;
} REG_KEYPAD_KP_IRQ_CAUSE_T;

// kp_irq_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t kp_irq_clr : 1; // [0], write clear
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_KEYPAD_KP_IRQ_CLR_T;

// kp_data_l
#define KEYPAD_KP_DATA_L(n) (((n)&0xffffffff) << 0)

// kp_data_h
#define KEYPAD_KP_DATA_H(n) (((n)&0xffffffff) << 0)

// kp_status
#define KEYPAD_KEYIN_STATUS(n) (((n)&0xff) << 0)
#define KEYPAD_KP_ON (1 << 31)

// kp_ctrl
#define KEYPAD_KP_EN (1 << 0)
#define KEYPAD_KP_DBN_TIME(n) (((n)&0xff) << 2)
#define KEYPAD_KP_ITV_TIME(n) (((n)&0x3f) << 10)
#define KEYPAD_KP_IN_MASK(n) (((n)&0xff) << 16)
#define KEYPAD_KP_OUT_MASK(n) (((n)&0xff) << 24)

// kp_irq_mask
#define KEYPAD_KP_EVT0_IRQ_MASK (1 << 0)
#define KEYPAD_KP_EVT1_IRQ_MASK (1 << 1)
#define KEYPAD_KP_ITV_IRQ_MASK (1 << 2)

// kp_irq_cause
#define KEYPAD_KP_EVT0_IRQ_CAUSE (1 << 0)
#define KEYPAD_KP_EVT1_IRQ_CAUSE (1 << 1)
#define KEYPAD_KP_ITV_IRQ_CAUSE (1 << 2)
#define KEYPAD_KP_EVT0_IRQ_STATUS (1 << 16)
#define KEYPAD_KP_EVT1_IRQ_STATUS (1 << 17)
#define KEYPAD_KP_ITV_IRQ_STATUS (1 << 18)

// kp_irq_clr
#define KEYPAD_KP_IRQ_CLR (1 << 0)

#endif // _KEYPAD_H_
