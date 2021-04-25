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

#ifndef _ARM_UART_H_
#define _ARM_UART_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_UART2_BASE (0x08817000)
#define REG_UART3_BASE (0x08818000)
#if defined(REG_ADDRESS_FOR_GGE)
#define REG_UART1_BASE (0x05098000)
#else
#define REG_UART1_BASE (0x50098000)
#endif

typedef volatile struct
{
    uint32_t uart_tx;          // 0x00000000
    uint32_t uart_rx;          // 0x00000004
    uint32_t uart_baud;        // 0x00000008
    uint32_t uart_conf;        // 0x0000000c
    uint32_t uart_rxtrig;      // 0x00000010
    uint32_t uart_txtrig;      // 0x00000014
    uint32_t uart_delay;       // 0x00000018
    uint32_t uart_status;      // 0x0000001c
    uint32_t uart_rxfifo_stat; // 0x00000020
    uint32_t uart_txfifo_stat; // 0x00000024
    uint32_t uart_rxfifo_hdlc; // 0x00000028
    uint32_t uart_at_status;   // 0x0000002c
    uint32_t uart_swfc_cc;     // 0x00000030
} HWP_ARM_UART_T;

#define hwp_uart2 ((HWP_ARM_UART_T *)REG_ACCESS_ADDRESS(REG_UART2_BASE))
#define hwp_uart3 ((HWP_ARM_UART_T *)REG_ACCESS_ADDRESS(REG_UART3_BASE))
#define hwp_uart1 ((HWP_ARM_UART_T *)REG_ACCESS_ADDRESS(REG_UART1_BASE))

// uart_tx
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_data : 8; // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_ARM_UART_UART_TX_T;

// uart_rx
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_data : 8; // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_ARM_UART_UART_RX_T;

// uart_baud
typedef union {
    uint32_t v;
    struct
    {
        uint32_t baud_div : 16;  // [15:0]
        uint32_t baud_const : 4; // [19:16]
        uint32_t __31_20 : 12;   // [31:20]
    } b;
} REG_ARM_UART_UART_BAUD_T;

// uart_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t check : 1;           // [0]
        uint32_t parity : 1;          // [1]
        uint32_t stop_bit : 1;        // [2]
        uint32_t st_check : 1;        // [3]
        uint32_t rx_ie : 1;           // [4]
        uint32_t tx_ie : 1;           // [5]
        uint32_t tout_ie : 1;         // [6]
        uint32_t hwfc : 1;            // [7]
        uint32_t rx_trig_hwfc : 1;    // [8]
        uint32_t tout_hwfc : 1;       // [9]
        uint32_t hdlc : 1;            // [10]
        uint32_t frm_stp : 1;         // [11]
        uint32_t trail : 1;           // [12]
        uint32_t txrst : 1;           // [13]
        uint32_t rxrst : 1;           // [14]
        uint32_t at_parity_en : 1;    // [15]
        uint32_t at_parity_sel : 2;   // [17:16]
        uint32_t __18_18 : 1;         // [18]
        uint32_t at_verify_2byte : 1; // [19]
        uint32_t at_div_mode : 1;     // [20]
        uint32_t at_enable : 1;       // [21]
        uint32_t swfc_xoff_ie : 1;    // [22]
        uint32_t swfc_xon_ie : 1;     // [23]
        uint32_t swfc_en : 1;         // [24]
        uint32_t at_lock_ie : 1;      // [25]
        uint32_t st_parity : 1;       // [26]
        uint32_t __31_27 : 5;         // [31:27]
    } b;
} REG_ARM_UART_UART_CONF_T;

// uart_rxtrig
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_trig : 8; // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_ARM_UART_UART_RXTRIG_T;

// uart_txtrig
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_trig : 8; // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_ARM_UART_UART_TXTRIG_T;

// uart_delay
typedef union {
    uint32_t v;
    struct
    {
        uint32_t toutcnt : 8;      // [7:0]
        uint32_t two_tx_delay : 4; // [11:8]
        uint32_t __31_12 : 20;     // [31:12]
    } b;
} REG_ARM_UART_UART_DELAY_T;

// uart_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_int : 1;      // [0], write clear
        uint32_t rx_int : 1;      // [1], write clear
        uint32_t timeout_int : 1; // [2], write clear
        uint32_t frame_int : 1;   // [3], write clear
        uint32_t p_error : 1;     // [4], write clear
        uint32_t st_error : 1;    // [5], write clear
        uint32_t cts : 1;         // [6], write clear
        uint32_t rts : 1;         // [7], write clear
        uint32_t swfc_status : 1; // [8], read only
        uint32_t xoff_status : 1; // [9], write clear
        uint32_t xon_status : 1;  // [10], write clear
        uint32_t __31_11 : 21;    // [31:11]
    } b;
} REG_ARM_UART_UART_STATUS_T;

// uart_rxfifo_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_fifo_cnt : 8; // [7:0], read only
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_ARM_UART_UART_RXFIFO_STAT_T;

// uart_txfifo_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_fifo_cnt : 8; // [7:0], read only
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_ARM_UART_UART_TXFIFO_STAT_T;

// uart_rxfifo_hdlc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hdlc_rxfifo_cnt : 8; // [7:0], read only
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_ARM_UART_UART_RXFIFO_HDLC_T;

// uart_at_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t at_baud_div : 16;         // [15:0], read only
        uint32_t character_miscompare : 1; // [16], read only
        uint32_t auto_baud_locked : 1;     // [17], read only
        uint32_t character_status : 1;     // [18], read only
        uint32_t __31_19 : 13;             // [31:19]
    } b;
} REG_ARM_UART_UART_AT_STATUS_T;

// uart_swfc_cc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xoff_cc : 8;  // [7:0]
        uint32_t xon_cc : 8;   // [15:8]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_UART_UART_SWFC_CC_T;

// uart_tx
#define ARM_UART_TX_DATA(n) (((n)&0xff) << 0)

// uart_rx
#define ARM_UART_RX_DATA(n) (((n)&0xff) << 0)

// uart_baud
#define ARM_UART_BAUD_DIV(n) (((n)&0xffff) << 0)
#define ARM_UART_BAUD_CONST(n) (((n)&0xf) << 16)

// uart_conf
#define ARM_UART_CHECK (1 << 0)
#define ARM_UART_PARITY (1 << 1)
#define ARM_UART_STOP_BIT (1 << 2)
#define ARM_UART_ST_CHECK (1 << 3)
#define ARM_UART_RX_IE (1 << 4)
#define ARM_UART_TX_IE (1 << 5)
#define ARM_UART_TOUT_IE (1 << 6)
#define ARM_UART_HWFC (1 << 7)
#define ARM_UART_RX_TRIG_HWFC (1 << 8)
#define ARM_UART_TOUT_HWFC (1 << 9)
#define ARM_UART_HDLC (1 << 10)
#define ARM_UART_FRM_STP (1 << 11)
#define ARM_UART_TRAIL (1 << 12)
#define ARM_UART_TXRST (1 << 13)
#define ARM_UART_RXRST (1 << 14)
#define ARM_UART_AT_PARITY_EN (1 << 15)
#define ARM_UART_AT_PARITY_SEL(n) (((n)&0x3) << 16)
#define ARM_UART_AT_VERIFY_2BYTE (1 << 19)
#define ARM_UART_AT_DIV_MODE (1 << 20)
#define ARM_UART_AT_ENABLE (1 << 21)
#define ARM_UART_SWFC_XOFF_IE (1 << 22)
#define ARM_UART_SWFC_XON_IE (1 << 23)
#define ARM_UART_SWFC_EN (1 << 24)
#define ARM_UART_AT_LOCK_IE (1 << 25)
#define ARM_UART_ST_PARITY (1 << 26)

// uart_rxtrig
#define ARM_UART_RX_TRIG(n) (((n)&0xff) << 0)

// uart_txtrig
#define ARM_UART_TX_TRIG(n) (((n)&0xff) << 0)

// uart_delay
#define ARM_UART_TOUTCNT(n) (((n)&0xff) << 0)
#define ARM_UART_TWO_TX_DELAY(n) (((n)&0xf) << 8)

// uart_status
#define ARM_UART_TX_INT (1 << 0)
#define ARM_UART_RX_INT (1 << 1)
#define ARM_UART_TIMEOUT_INT (1 << 2)
#define ARM_UART_FRAME_INT (1 << 3)
#define ARM_UART_P_ERROR (1 << 4)
#define ARM_UART_ST_ERROR (1 << 5)
#define ARM_UART_CTS (1 << 6)
#define ARM_UART_RTS (1 << 7)
#define ARM_UART_SWFC_STATUS (1 << 8)
#define ARM_UART_XOFF_STATUS (1 << 9)
#define ARM_UART_XON_STATUS (1 << 10)

// uart_rxfifo_stat
#define ARM_UART_RX_FIFO_CNT(n) (((n)&0xff) << 0)

// uart_txfifo_stat
#define ARM_UART_TX_FIFO_CNT(n) (((n)&0xff) << 0)

// uart_rxfifo_hdlc
#define ARM_UART_HDLC_RXFIFO_CNT(n) (((n)&0xff) << 0)

// uart_at_status
#define ARM_UART_AT_BAUD_DIV(n) (((n)&0xffff) << 0)
#define ARM_UART_CHARACTER_MISCOMPARE (1 << 16)
#define ARM_UART_AUTO_BAUD_LOCKED (1 << 17)
#define ARM_UART_CHARACTER_STATUS (1 << 18)

// uart_swfc_cc
#define ARM_UART_XOFF_CC(n) (((n)&0xff) << 0)
#define ARM_UART_XON_CC(n) (((n)&0xff) << 8)

#endif // _ARM_UART_H_
