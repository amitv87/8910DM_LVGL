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

#ifndef _DEBUG_HOST_H_
#define _DEBUG_HOST_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_DEBUG_HOST_BASE (0x0517f000)
#else
#define REG_DEBUG_HOST_BASE (0x5017f000)
#endif

typedef volatile struct
{
    uint32_t cmd;            // 0x00000000
    uint32_t data_reg;       // 0x00000004
    uint32_t event;          // 0x00000008
    uint32_t mode;           // 0x0000000c
    uint32_t h2p_status_reg; // 0x00000010
    uint32_t p2h_status_reg; // 0x00000014
    uint32_t irq;            // 0x00000018
} HWP_DEBUG_HOST_T;

#define hwp_debugHost ((HWP_DEBUG_HOST_T *)REG_ACCESS_ADDRESS(REG_DEBUG_HOST_BASE))

// cmd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t addr : 29;   // [28:0], read only
        uint32_t size : 2;    // [30:29], read only
        uint32_t write_h : 1; // [31], read only
    } b;
} REG_DEBUG_HOST_CMD_T;

// event
typedef union {
    uint32_t v;
    struct
    {
        uint32_t event0_sema : 1; // [0]
        uint32_t event31_1 : 31;  // [31:1]
    } b;
} REG_DEBUG_HOST_EVENT_T;

// mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t force_on : 1;    // [0]
        uint32_t clk_host_on : 1; // [1], read only
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_DEBUG_HOST_MODE_T;

// h2p_status_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t h2p_status : 8;     // [7:0], read only
        uint32_t __15_8 : 8;         // [15:8]
        uint32_t h2p_status_rst : 1; // [16]
        uint32_t __31_17 : 15;       // [31:17]
    } b;
} REG_DEBUG_HOST_H2P_STATUS_REG_T;

// p2h_status_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2h_status : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_DEBUG_HOST_P2H_STATUS_REG_T;

// irq
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xcpu_irq : 1; // [0], read only
        uint32_t bcpu_irq : 1; // [1], read only
        uint32_t __31_2 : 30;  // [31:2]
    } b;
} REG_DEBUG_HOST_IRQ_T;

// cmd
#define DEBUG_HOST_ADDR(n) (((n)&0x1fffffff) << 0)
#define DEBUG_HOST_SIZE(n) (((n)&0x3) << 29)
#define DEBUG_HOST_WRITE_H (1 << 31)

// data_reg
#define DEBUG_HOST_DATA(n) (((n)&0xffffffff) << 0)

// event
#define DEBUG_HOST_EVENT0_SEMA (1 << 0)
#define DEBUG_HOST_EVENT31_1(n) (((n)&0x7fffffff) << 1)

// mode
#define DEBUG_HOST_FORCE_ON (1 << 0)
#define DEBUG_HOST_CLK_HOST_ON (1 << 1)

// h2p_status_reg
#define DEBUG_HOST_H2P_STATUS(n) (((n)&0xff) << 0)
#define DEBUG_HOST_H2P_STATUS_RST (1 << 16)

// p2h_status_reg
#define DEBUG_HOST_P2H_STATUS(n) (((n)&0xff) << 0)

// irq
#define DEBUG_HOST_XCPU_IRQ (1 << 0)
#define DEBUG_HOST_BCPU_IRQ (1 << 1)

#endif // _DEBUG_HOST_H_
