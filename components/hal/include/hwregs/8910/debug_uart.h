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

#ifndef _DEBUG_UART_H_
#define _DEBUG_UART_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define DEBUG_UART_RX_FIFO_SIZE (16)
#define DEBUG_UART_TX_FIFO_SIZE (16)
#define DEBUG_UART_NB_RX_FIFO_BITS (4)
#define DEBUG_UART_NB_TX_FIFO_BITS (4)
#define ESC_DAT (92)

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_DEBUG_UART_BASE (0x05110000)
#else
#define REG_DEBUG_UART_BASE (0x50110000)
#endif

typedef volatile struct
{
    uint32_t ctrl;        // 0x00000000
    uint32_t status;      // 0x00000004
    uint32_t rxtx_buffer; // 0x00000008
    uint32_t irq_mask;    // 0x0000000c
    uint32_t irq_cause;   // 0x00000010
    uint32_t triggers;    // 0x00000014
    uint32_t xchar;       // 0x00000018
} HWP_DEBUG_UART_T;

#define hwp_debugUart ((HWP_DEBUG_UART_T *)REG_ACCESS_ADDRESS(REG_DEBUG_UART_BASE))

// ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;            // [0]
        uint32_t data_bits : 1;         // [1]
        uint32_t tx_stop_bits : 1;      // [2]
        uint32_t parity_enable : 1;     // [3]
        uint32_t parity_select : 2;     // [5:4]
        uint32_t tx_break_control : 1;  // [6]
        uint32_t rx_fifo_reset : 1;     // [7]
        uint32_t tx_fifo_reset : 1;     // [8]
        uint32_t dma_mode : 1;          // [9]
        uint32_t __11_10 : 2;           // [11:10]
        uint32_t swrx_flow_ctrl : 2;    // [13:12]
        uint32_t swtx_flow_ctrl : 2;    // [15:14]
        uint32_t backslash_en : 1;      // [16]
        uint32_t send_xon : 1;          // [17]
        uint32_t __18_18 : 1;           // [18]
        uint32_t tx_finish_n_wait : 1;  // [19]
        uint32_t divisor_mode : 1;      // [20]
        uint32_t irda_enable : 1;       // [21]
        uint32_t rx_rts : 1;            // [22]
        uint32_t auto_flow_control : 1; // [23]
        uint32_t loop_back_mode : 1;    // [24]
        uint32_t rx_lock_err : 1;       // [25]
        uint32_t hst_txd_oen : 1;       // [26]
        uint32_t __27_27 : 1;           // [27]
        uint32_t rx_break_length : 4;   // [31:28]
    } b;
} REG_DEBUG_UART_CTRL_T;

// status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_fifo_level : 5;     // [4:0], read only
        uint32_t __7_5 : 3;             // [7:5]
        uint32_t tx_fifo_level : 5;     // [12:8], read only
        uint32_t tx_active : 1;         // [13], read only
        uint32_t rx_active : 1;         // [14], read only
        uint32_t __15_15 : 1;           // [15]
        uint32_t rx_overflow_err : 1;   // [16], read only
        uint32_t tx_overflow_err : 1;   // [17], read only
        uint32_t rx_parity_err : 1;     // [18], read only
        uint32_t rx_framing_err : 1;    // [19], read only
        uint32_t rx_break_int : 1;      // [20], read only
        uint32_t __23_21 : 3;           // [23:21]
        uint32_t tx_dcts : 1;           // [24], read only
        uint32_t tx_cts : 1;            // [25], read only
        uint32_t __27_26 : 2;           // [27:26]
        uint32_t tx_fifo_rsted_l : 1;   // [28], read only
        uint32_t rx_fifo_rsted_l : 1;   // [29], read only
        uint32_t enable_n_finished : 1; // [30], read only
        uint32_t clk_enabled : 1;       // [31], read only
    } b;
} REG_DEBUG_UART_STATUS_T;

// rxtx_buffer
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_data : 8; // [7:0], read only
        uint32_t tx_data : 8; // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_DEBUG_UART_RXTX_BUFFER_T;

// irq_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_modem_status : 1;   // [0]
        uint32_t rx_data_available : 1; // [1]
        uint32_t tx_data_needed : 1;    // [2]
        uint32_t rx_timeout : 1;        // [3]
        uint32_t rx_line_err : 1;       // [4]
        uint32_t tx_dma_done : 1;       // [5]
        uint32_t rx_dma_done : 1;       // [6]
        uint32_t rx_dma_timeout : 1;    // [7]
        uint32_t xoff_detected : 1;     // [8]
        uint32_t __31_9 : 23;           // [31:9]
    } b;
} REG_DEBUG_UART_IRQ_MASK_T;

// irq_cause
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_modem_status : 1;     // [0], read only
        uint32_t rx_data_available : 1;   // [1], read only
        uint32_t tx_data_needed : 1;      // [2], read only
        uint32_t rx_timeout : 1;          // [3], read only
        uint32_t rx_line_err : 1;         // [4], read only
        uint32_t tx_dma_done : 1;         // [5]
        uint32_t rx_dma_done : 1;         // [6]
        uint32_t rx_dma_timeout : 1;      // [7]
        uint32_t xoff_detected : 1;       // [8], read only
        uint32_t __15_9 : 7;              // [15:9]
        uint32_t tx_modem_status_u : 1;   // [16], read only
        uint32_t rx_data_available_u : 1; // [17], read only
        uint32_t tx_data_needed_u : 1;    // [18], read only
        uint32_t rx_timeout_u : 1;        // [19], read only
        uint32_t rx_line_err_u : 1;       // [20], read only
        uint32_t tx_dma_done_u : 1;       // [21], read only
        uint32_t rx_dma_done_u : 1;       // [22], read only
        uint32_t rx_dma_timeout_u : 1;    // [23], read only
        uint32_t xoff_detected_u : 1;     // [24], read only
        uint32_t __31_25 : 7;             // [31:25]
    } b;
} REG_DEBUG_UART_IRQ_CAUSE_T;

// triggers
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_trigger : 4; // [3:0]
        uint32_t tx_trigger : 4; // [7:4]
        uint32_t afc_level : 4;  // [11:8]
        uint32_t __31_12 : 20;   // [31:12]
    } b;
} REG_DEBUG_UART_TRIGGERS_T;

// xchar
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xon1 : 8;  // [7:0]
        uint32_t xoff1 : 8; // [15:8]
        uint32_t xon2 : 8;  // [23:16]
        uint32_t xoff2 : 8; // [31:24]
    } b;
} REG_DEBUG_UART_XCHAR_T;

// ctrl
#define DEBUG_UART_ENABLE_DISABLE (0 << 0)
#define DEBUG_UART_ENABLE_ENABLE (1 << 0)
#define DEBUG_UART_DATA_BITS_7_BITS (0 << 1)
#define DEBUG_UART_DATA_BITS_8_BITS (1 << 1)
#define DEBUG_UART_TX_STOP_BITS_1_BIT (0 << 2)
#define DEBUG_UART_TX_STOP_BITS_2_BITS (1 << 2)
#define DEBUG_UART_PARITY_ENABLE_NO (0 << 3)
#define DEBUG_UART_PARITY_ENABLE_YES (1 << 3)
#define DEBUG_UART_PARITY_SELECT(n) (((n)&0x3) << 4)
#define DEBUG_UART_PARITY_SELECT_ODD (0 << 4)
#define DEBUG_UART_PARITY_SELECT_EVEN (1 << 4)
#define DEBUG_UART_PARITY_SELECT_SPACE (2 << 4)
#define DEBUG_UART_PARITY_SELECT_MARK (3 << 4)
#define DEBUG_UART_TX_BREAK_CONTROL_OFF (0 << 6)
#define DEBUG_UART_TX_BREAK_CONTROL_ON (1 << 6)
#define DEBUG_UART_RX_FIFO_RESET (1 << 7)
#define DEBUG_UART_TX_FIFO_RESET (1 << 8)
#define DEBUG_UART_DMA_MODE_DISABLE (0 << 9)
#define DEBUG_UART_DMA_MODE_ENABLE (1 << 9)
#define DEBUG_UART_SWRX_FLOW_CTRL(n) (((n)&0x3) << 12)
#define DEBUG_UART_SWTX_FLOW_CTRL(n) (((n)&0x3) << 14)
#define DEBUG_UART_BACKSLASH_EN (1 << 16)
#define DEBUG_UART_SEND_XON (1 << 17)
#define DEBUG_UART_TX_FINISH_N_WAIT (1 << 19)
#define DEBUG_UART_DIVISOR_MODE (1 << 20)
#define DEBUG_UART_IRDA_ENABLE (1 << 21)
#define DEBUG_UART_RX_RTS_INACTIVE (0 << 22)
#define DEBUG_UART_RX_RTS_ACTIVE (1 << 22)
#define DEBUG_UART_AUTO_FLOW_CONTROL_ENABLE (1 << 23)
#define DEBUG_UART_AUTO_FLOW_CONTROL_DISABLE (0 << 23)
#define DEBUG_UART_LOOP_BACK_MODE (1 << 24)
#define DEBUG_UART_RX_LOCK_ERR_DISABLE (0 << 25)
#define DEBUG_UART_RX_LOCK_ERR_ENABLE (1 << 25)
#define DEBUG_UART_HST_TXD_OEN_DISABLE (1 << 26)
#define DEBUG_UART_HST_TXD_OEN_ENABLE (0 << 26)
#define DEBUG_UART_RX_BREAK_LENGTH(n) (((n)&0xf) << 28)

#define DEBUG_UART_ENABLE_V_DISABLE (0)
#define DEBUG_UART_ENABLE_V_ENABLE (1)
#define DEBUG_UART_DATA_BITS_V_7_BITS (0)
#define DEBUG_UART_DATA_BITS_V_8_BITS (1)
#define DEBUG_UART_TX_STOP_BITS_V_1_BIT (0)
#define DEBUG_UART_TX_STOP_BITS_V_2_BITS (1)
#define DEBUG_UART_PARITY_ENABLE_V_NO (0)
#define DEBUG_UART_PARITY_ENABLE_V_YES (1)
#define DEBUG_UART_PARITY_SELECT_V_ODD (0)
#define DEBUG_UART_PARITY_SELECT_V_EVEN (1)
#define DEBUG_UART_PARITY_SELECT_V_SPACE (2)
#define DEBUG_UART_PARITY_SELECT_V_MARK (3)
#define DEBUG_UART_TX_BREAK_CONTROL_V_OFF (0)
#define DEBUG_UART_TX_BREAK_CONTROL_V_ON (1)
#define DEBUG_UART_DMA_MODE_V_DISABLE (0)
#define DEBUG_UART_DMA_MODE_V_ENABLE (1)
#define DEBUG_UART_RX_RTS_V_INACTIVE (0)
#define DEBUG_UART_RX_RTS_V_ACTIVE (1)
#define DEBUG_UART_AUTO_FLOW_CONTROL_V_ENABLE (1)
#define DEBUG_UART_AUTO_FLOW_CONTROL_V_DISABLE (0)
#define DEBUG_UART_RX_LOCK_ERR_V_DISABLE (0)
#define DEBUG_UART_RX_LOCK_ERR_V_ENABLE (1)
#define DEBUG_UART_HST_TXD_OEN_V_DISABLE (1)
#define DEBUG_UART_HST_TXD_OEN_V_ENABLE (0)

// status
#define DEBUG_UART_RX_FIFO_LEVEL(n) (((n)&0x1f) << 0)
#define DEBUG_UART_TX_FIFO_LEVEL(n) (((n)&0x1f) << 8)
#define DEBUG_UART_TX_ACTIVE (1 << 13)
#define DEBUG_UART_RX_ACTIVE (1 << 14)
#define DEBUG_UART_RX_OVERFLOW_ERR (1 << 16)
#define DEBUG_UART_TX_OVERFLOW_ERR (1 << 17)
#define DEBUG_UART_RX_PARITY_ERR (1 << 18)
#define DEBUG_UART_RX_FRAMING_ERR (1 << 19)
#define DEBUG_UART_RX_BREAK_INT (1 << 20)
#define DEBUG_UART_TX_DCTS (1 << 24)
#define DEBUG_UART_TX_CTS (1 << 25)
#define DEBUG_UART_TX_FIFO_RSTED_L (1 << 28)
#define DEBUG_UART_RX_FIFO_RSTED_L (1 << 29)
#define DEBUG_UART_ENABLE_N_FINISHED (1 << 30)
#define DEBUG_UART_CLK_ENABLED (1 << 31)

// rxtx_buffer
#define DEBUG_UART_RX_DATA(n) (((n)&0xff) << 0)
#define DEBUG_UART_TX_DATA(n) (((n)&0xff) << 0)

// irq_mask
#define DEBUG_UART_TX_MODEM_STATUS (1 << 0)
#define DEBUG_UART_RX_DATA_AVAILABLE (1 << 1)
#define DEBUG_UART_TX_DATA_NEEDED (1 << 2)
#define DEBUG_UART_RX_TIMEOUT (1 << 3)
#define DEBUG_UART_RX_LINE_ERR (1 << 4)
#define DEBUG_UART_TX_DMA_DONE (1 << 5)
#define DEBUG_UART_RX_DMA_DONE (1 << 6)
#define DEBUG_UART_RX_DMA_TIMEOUT (1 << 7)
#define DEBUG_UART_XOFF_DETECTED (1 << 8)

// irq_cause
#define DEBUG_UART_TX_MODEM_STATUS (1 << 0)
#define DEBUG_UART_RX_DATA_AVAILABLE (1 << 1)
#define DEBUG_UART_TX_DATA_NEEDED (1 << 2)
#define DEBUG_UART_RX_TIMEOUT (1 << 3)
#define DEBUG_UART_RX_LINE_ERR (1 << 4)
#define DEBUG_UART_TX_DMA_DONE (1 << 5)
#define DEBUG_UART_RX_DMA_DONE (1 << 6)
#define DEBUG_UART_RX_DMA_TIMEOUT (1 << 7)
#define DEBUG_UART_XOFF_DETECTED (1 << 8)
#define DEBUG_UART_TX_MODEM_STATUS_U (1 << 16)
#define DEBUG_UART_RX_DATA_AVAILABLE_U (1 << 17)
#define DEBUG_UART_TX_DATA_NEEDED_U (1 << 18)
#define DEBUG_UART_RX_TIMEOUT_U (1 << 19)
#define DEBUG_UART_RX_LINE_ERR_U (1 << 20)
#define DEBUG_UART_TX_DMA_DONE_U (1 << 21)
#define DEBUG_UART_RX_DMA_DONE_U (1 << 22)
#define DEBUG_UART_RX_DMA_TIMEOUT_U (1 << 23)
#define DEBUG_UART_XOFF_DETECTED_U (1 << 24)

// triggers
#define DEBUG_UART_RX_TRIGGER(n) (((n)&0xf) << 0)
#define DEBUG_UART_TX_TRIGGER(n) (((n)&0xf) << 4)
#define DEBUG_UART_AFC_LEVEL(n) (((n)&0xf) << 8)

// xchar
#define DEBUG_UART_XON1(n) (((n)&0xff) << 0)
#define DEBUG_UART_XOFF1(n) (((n)&0xff) << 8)
#define DEBUG_UART_XON2(n) (((n)&0xff) << 16)
#define DEBUG_UART_XOFF2(n) (((n)&0xff) << 24)

#endif // _DEBUG_UART_H_
