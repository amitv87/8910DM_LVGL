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

#ifndef _GPIO_H_
#define _GPIO_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define IDX_GPIO_DCON (0)
#define IDX_GPO_CHG (0)

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_GPIO1_BASE (0x05107000)
#else
#define REG_GPIO1_BASE (0x50107000)
#endif

typedef volatile struct
{
    uint32_t gpio_oen_val;         // 0x00000000
    uint32_t gpio_oen_set_out;     // 0x00000004
    uint32_t gpio_oen_set_in;      // 0x00000008
    uint32_t gpio_val_reg;         // 0x0000000c
    uint32_t gpio_set_reg;         // 0x00000010
    uint32_t gpio_clr_reg;         // 0x00000014
    uint32_t gpint_ctrl_r_set_reg; // 0x00000018
    uint32_t gpint_ctrl_r_clr_reg; // 0x0000001c
    uint32_t int_clr;              // 0x00000020
    uint32_t int_status;           // 0x00000024
    uint32_t chg_ctrl;             // 0x00000028
    uint32_t chg_cmd;              // 0x0000002c
    uint32_t gpo_set_reg;          // 0x00000030
    uint32_t gpo_clr_reg;          // 0x00000034
    uint32_t gpint_ctrl_f_set_reg; // 0x00000038
    uint32_t gpint_ctrl_f_clr_reg; // 0x0000003c
    uint32_t dbn_en_set_reg;       // 0x00000040
    uint32_t dbn_en_clr_reg;       // 0x00000044
    uint32_t gpint_mode_set_reg;   // 0x00000048
    uint32_t gpint_mode_clr_reg;   // 0x0000004c
} HWP_GPIO_T;

#define hwp_gpio1 ((HWP_GPIO_T *)REG_ACCESS_ADDRESS(REG_GPIO1_BASE))

// chg_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t out_time : 4;  // [3:0]
        uint32_t wait_time : 6; // [9:4]
        uint32_t __15_10 : 6;   // [15:10]
        uint32_t int_mode : 2;  // [17:16]
        uint32_t __31_18 : 14;  // [31:18]
    } b;
} REG_GPIO_CHG_CTRL_T;

// chg_cmd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dcon_mode_set : 1; // [0], write set
        uint32_t __3_1 : 3;         // [3:1]
        uint32_t chg_mode_set : 1;  // [4], write set
        uint32_t __7_5 : 3;         // [7:5]
        uint32_t dcon_mode_clr : 1; // [8], write clear
        uint32_t __11_9 : 3;        // [11:9]
        uint32_t chg_mode_clr : 1;  // [12], write clear
        uint32_t __23_13 : 11;      // [23:13]
        uint32_t chg_down : 1;      // [24], write set
        uint32_t __31_25 : 7;       // [31:25]
    } b;
} REG_GPIO_CHG_CMD_T;

// gpo_set_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpo_set : 8; // [7:0], write set
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_GPIO_GPO_SET_REG_T;

// gpo_clr_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpo_clr : 8; // [7:0], write clear
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_GPIO_GPO_CLR_REG_T;

// gpio_oen_val
#define GPIO_OEN_VAL(n) (((n)&0xffffffff) << 0)
#define GPIO_OEN_VAL_INPUT (1 << 0)
#define GPIO_OEN_VAL_OUTPUT (0 << 0)

#define GPIO_OEN_VAL_V_INPUT (1)
#define GPIO_OEN_VAL_V_OUTPUT (0)

// gpio_oen_set_out
#define GPIO_OEN_SET_OUT(n) (((n)&0xffffffff) << 0)

// gpio_oen_set_in
#define GPIO_OEN_SET_IN(n) (((n)&0xffffffff) << 0)

// gpio_val_reg
#define GPIO_GPIO_VAL(n) (((n)&0xffffffff) << 0)

// gpio_set_reg
#define GPIO_GPIO_SET(n) (((n)&0xffffffff) << 0)

// gpio_clr_reg
#define GPIO_GPIO_CLR(n) (((n)&0xffffffff) << 0)

// gpint_ctrl_r_set_reg
#define GPIO_GPINT_CTRL_R_SET(n) (((n)&0xffffffff) << 0)

// gpint_ctrl_r_clr_reg
#define GPIO_GPINT_CTRL_R_CLR(n) (((n)&0xffffffff) << 0)

// int_clr
#define GPIO_GPINT_CLR(n) (((n)&0xffffffff) << 0)

// int_status
#define GPIO_GPINT_STATUS(n) (((n)&0xffffffff) << 0)

// chg_ctrl
#define GPIO_OUT_TIME(n) (((n)&0xf) << 0)
#define GPIO_WAIT_TIME(n) (((n)&0x3f) << 4)
#define GPIO_INT_MODE(n) (((n)&0x3) << 16)
#define GPIO_INT_MODE_L2H (0 << 16)
#define GPIO_INT_MODE_H2L (1 << 16)
#define GPIO_INT_MODE_RR (3 << 16)

#define GPIO_INT_MODE_V_L2H (0)
#define GPIO_INT_MODE_V_H2L (1)
#define GPIO_INT_MODE_V_RR (3)

// chg_cmd
#define GPIO_DCON_MODE_SET (1 << 0)
#define GPIO_CHG_MODE_SET (1 << 4)
#define GPIO_DCON_MODE_CLR (1 << 8)
#define GPIO_CHG_MODE_CLR (1 << 12)
#define GPIO_CHG_DOWN (1 << 24)

// gpo_set_reg
#define GPIO_GPO_SET(n) (((n)&0xff) << 0)

// gpo_clr_reg
#define GPIO_GPO_CLR(n) (((n)&0xff) << 0)

// gpint_ctrl_f_set_reg
#define GPIO_GPINT_CTRL_F_SET(n) (((n)&0xffffffff) << 0)

// gpint_ctrl_f_clr_reg
#define GPIO_GPINT_CTRL_F_CLR(n) (((n)&0xffffffff) << 0)

// dbn_en_set_reg
#define GPIO_DBN_EN_SET(n) (((n)&0xffffffff) << 0)

// dbn_en_clr_reg
#define GPIO_DBN_EN_CLR(n) (((n)&0xffffffff) << 0)

// gpint_mode_set_reg
#define GPIO_GPINT_MODE_SET(n) (((n)&0xffffffff) << 0)

// gpint_mode_clr_reg
#define GPIO_GPINT_MODE_CLR(n) (((n)&0xffffffff) << 0)

#endif // _GPIO_H_
