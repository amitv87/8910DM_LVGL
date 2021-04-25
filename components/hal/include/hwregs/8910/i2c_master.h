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

#ifndef _I2C_MASTER_H_
#define _I2C_MASTER_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_I2C_MASTER1_BASE (0x0880a000)
#define REG_I2C_MASTER3_BASE (0x0880b000)
#if defined(REG_ADDRESS_FOR_GGE)
#define REG_I2C_MASTER2_BASE (0x05103000)
#else
#define REG_I2C_MASTER2_BASE (0x50103000)
#endif

typedef volatile struct
{
    uint32_t ctrl;        // 0x00000000
    uint32_t status;      // 0x00000004
    uint32_t txrx_buffer; // 0x00000008
    uint32_t cmd;         // 0x0000000c
    uint32_t irq_clr;     // 0x00000010
} HWP_I2C_MASTER_T;

#define hwp_i2cMaster1 ((HWP_I2C_MASTER_T *)REG_ACCESS_ADDRESS(REG_I2C_MASTER1_BASE))
#define hwp_i2cMaster3 ((HWP_I2C_MASTER_T *)REG_ACCESS_ADDRESS(REG_I2C_MASTER3_BASE))
#define hwp_i2cMaster2 ((HWP_I2C_MASTER_T *)REG_ACCESS_ADDRESS(REG_I2C_MASTER2_BASE))

// ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t en : 1;              // [0]
        uint32_t __7_1 : 7;           // [7:1]
        uint32_t irq_mask : 1;        // [8]
        uint32_t __15_9 : 7;          // [15:9]
        uint32_t clock_prescale : 16; // [31:16]
    } b;
} REG_I2C_MASTER_CTRL_T;

// status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t irq_cause : 1;  // [0], read only
        uint32_t __3_1 : 3;      // [3:1]
        uint32_t irq_status : 1; // [4], read only
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t tip : 1;        // [8], read only
        uint32_t __11_9 : 3;     // [11:9]
        uint32_t al : 1;         // [12], read only
        uint32_t __15_13 : 3;    // [15:13]
        uint32_t busy : 1;       // [16], read only
        uint32_t __19_17 : 3;    // [19:17]
        uint32_t rxack : 1;      // [20], read only
        uint32_t __31_21 : 11;   // [31:21]
    } b;
} REG_I2C_MASTER_STATUS_T;

// txrx_buffer
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_data : 8; // [7:0]
        uint32_t rx_data : 8; // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_I2C_MASTER_TXRX_BUFFER_T;

// cmd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ack : 1;      // [0]
        uint32_t __3_1 : 3;    // [3:1]
        uint32_t rd : 1;       // [4]
        uint32_t __7_5 : 3;    // [7:5]
        uint32_t sto : 1;      // [8]
        uint32_t __11_9 : 3;   // [11:9]
        uint32_t rw : 1;       // [12]
        uint32_t __15_13 : 3;  // [15:13]
        uint32_t sta : 1;      // [16]
        uint32_t __31_17 : 15; // [31:17]
    } b;
} REG_I2C_MASTER_CMD_T;

// irq_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t irq_clr : 1; // [0], write clear
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_I2C_MASTER_IRQ_CLR_T;

// ctrl
#define I2C_MASTER_EN (1 << 0)
#define I2C_MASTER_IRQ_MASK (1 << 8)
#define I2C_MASTER_CLOCK_PRESCALE(n) (((n)&0xffff) << 16)

// status
#define I2C_MASTER_IRQ_CAUSE (1 << 0)
#define I2C_MASTER_IRQ_STATUS (1 << 4)
#define I2C_MASTER_TIP (1 << 8)
#define I2C_MASTER_AL (1 << 12)
#define I2C_MASTER_BUSY (1 << 16)
#define I2C_MASTER_RXACK (1 << 20)

// txrx_buffer
#define I2C_MASTER_TX_DATA(n) (((n)&0xff) << 0)
#define I2C_MASTER_RX_DATA(n) (((n)&0xff) << 0)

// cmd
#define I2C_MASTER_ACK (1 << 0)
#define I2C_MASTER_RD (1 << 4)
#define I2C_MASTER_STO (1 << 8)
#define I2C_MASTER_RW (1 << 12)
#define I2C_MASTER_STA (1 << 16)

// irq_clr
#define I2C_MASTER_IRQ_CLR (1 << 0)

#endif // _I2C_MASTER_H_
