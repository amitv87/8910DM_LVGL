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

#ifndef _SPI_H_
#define _SPI_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define SPI_TX_FIFO_SIZE (16)
#define SPI_RX_FIFO_SIZE (16)

#define REG_SPI1_BASE (0x08812000)
#define REG_SPI2_BASE (0x08813000)

typedef volatile struct
{
    uint32_t ctrl;        // 0x00000000
    uint32_t status;      // 0x00000004
    uint32_t rxtx_buffer; // 0x00000008
    uint32_t cfg;         // 0x0000000c
    uint32_t pattern;     // 0x00000010
    uint32_t stream;      // 0x00000014
    uint32_t pin_control; // 0x00000018
    uint32_t irq;         // 0x0000001c
} HWP_SPI_T;

#define hwp_spi1 ((HWP_SPI_T *)REG_ACCESS_ADDRESS(REG_SPI1_BASE))
#define hwp_spi2 ((HWP_SPI_T *)REG_ACCESS_ADDRESS(REG_SPI2_BASE))

// ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;            // [0]
        uint32_t cs_sel : 2;            // [2:1]
        uint32_t __3_3 : 1;             // [3]
        uint32_t input_mode : 1;        // [4]
        uint32_t clock_polarity : 1;    // [5]
        uint32_t clock_delay : 2;       // [7:6]
        uint32_t do_delay : 2;          // [9:8]
        uint32_t di_delay : 2;          // [11:10]
        uint32_t cs_delay : 2;          // [13:12]
        uint32_t cs_pulse : 2;          // [15:14]
        uint32_t frame_size : 5;        // [20:16]
        uint32_t __23_21 : 3;           // [23:21]
        uint32_t oe_delay : 5;          // [28:24]
        uint32_t ctrl_data_mux_sel : 1; // [29]
        uint32_t input_sel : 2;         // [31:30]
    } b;
} REG_SPI_CTRL_T;

// status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t active_status : 1;    // [0], read only
        uint32_t __2_1 : 2;            // [2:1]
        uint32_t cause_rx_ovf_irq : 1; // [3]
        uint32_t cause_tx_th_irq : 1;  // [4], read only
        uint32_t cause_tx_dma_irq : 1; // [5]
        uint32_t cause_rx_th_irq : 1;  // [6], read only
        uint32_t cause_rx_dma_irq : 1; // [7]
        uint32_t __8_8 : 1;            // [8]
        uint32_t tx_ovf : 1;           // [9]
        uint32_t rx_udf : 1;           // [10]
        uint32_t rx_ovf : 1;           // [11]
        uint32_t tx_th : 1;            // [12], read only
        uint32_t tx_dma_done : 1;      // [13]
        uint32_t rx_th : 1;            // [14], read only
        uint32_t rx_dma_done : 1;      // [15]
        uint32_t tx_space : 5;         // [20:16], read only
        uint32_t __23_21 : 3;          // [23:21]
        uint32_t rx_level : 5;         // [28:24], read only
        uint32_t __29_29 : 1;          // [29]
        uint32_t fifo_flush : 1;       // [30]
        uint32_t __31_31 : 1;          // [31]
    } b;
} REG_SPI_STATUS_T;

// rxtx_buffer
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data_in_out : 29; // [28:0]
        uint32_t cs : 2;           // [30:29]
        uint32_t read_ena : 1;     // [31]
    } b;
} REG_SPI_RXTX_BUFFER_T;

// cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cs_polarity_0 : 1;  // [0]
        uint32_t cs_polarity_1 : 1;  // [1]
        uint32_t cs_polarity_2 : 1;  // [2]
        uint32_t __15_3 : 13;        // [15:3]
        uint32_t clock_divider : 10; // [25:16]
        uint32_t __27_26 : 2;        // [27:26]
        uint32_t clock_limiter : 1;  // [28]
        uint32_t __31_29 : 3;        // [31:29]
    } b;
} REG_SPI_CFG_T;

// pattern
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pattern : 8;          // [7:0]
        uint32_t pattern_mode : 1;     // [8]
        uint32_t pattern_selector : 1; // [9]
        uint32_t __31_10 : 22;         // [31:10]
    } b;
} REG_SPI_PATTERN_T;

// stream
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_stream_bit : 1;                   // [0]
        uint32_t __7_1 : 7;                           // [7:1]
        uint32_t tx_stream_mode : 1;                  // [8]
        uint32_t __15_9 : 7;                          // [15:9]
        uint32_t tx_stream_stop_with_rx_dma_done : 1; // [16]
        uint32_t __31_17 : 15;                        // [31:17]
    } b;
} REG_SPI_STREAM_T;

// pin_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_ctrl : 2; // [1:0]
        uint32_t do_ctrl : 2;  // [3:2]
        uint32_t cs0_ctrl : 2; // [5:4]
        uint32_t cs1_ctrl : 2; // [7:6]
        uint32_t cs2_ctrl : 2; // [9:8]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_SPI_PIN_CONTROL_T;

// irq
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mask_rx_ovf_irq : 1; // [0]
        uint32_t mask_tx_th_irq : 1;  // [1]
        uint32_t mask_tx_dma_irq : 1; // [2]
        uint32_t mask_rx_th_irq : 1;  // [3]
        uint32_t mask_rx_dma_irq : 1; // [4]
        uint32_t tx_threshold : 2;    // [6:5]
        uint32_t rx_threshold : 2;    // [8:7]
        uint32_t __31_9 : 23;         // [31:9]
    } b;
} REG_SPI_IRQ_T;

// ctrl
#define SPI_ENABLE (1 << 0)
#define SPI_CS_SEL(n) (((n)&0x3) << 1)
#define SPI_CS_SEL_CS0 (0 << 1)
#define SPI_CS_SEL_CS1 (1 << 1)
#define SPI_CS_SEL_CS2 (2 << 1)
#define SPI_CS_SEL_CS3 (3 << 1)
#define SPI_INPUT_MODE (1 << 4)
#define SPI_CLOCK_POLARITY (1 << 5)
#define SPI_CLOCK_DELAY(n) (((n)&0x3) << 6)
#define SPI_DO_DELAY(n) (((n)&0x3) << 8)
#define SPI_DI_DELAY(n) (((n)&0x3) << 10)
#define SPI_CS_DELAY(n) (((n)&0x3) << 12)
#define SPI_CS_PULSE(n) (((n)&0x3) << 14)
#define SPI_FRAME_SIZE(n) (((n)&0x1f) << 16)
#define SPI_OE_DELAY(n) (((n)&0x1f) << 24)
#define SPI_CTRL_DATA_MUX_SEL_CTRL_REG_SEL (0 << 29)
#define SPI_CTRL_DATA_MUX_SEL_DATA_REG_SEL (1 << 29)
#define SPI_INPUT_SEL(n) (((n)&0x3) << 30)

#define SPI_CS_SEL_V_CS0 (0)
#define SPI_CS_SEL_V_CS1 (1)
#define SPI_CS_SEL_V_CS2 (2)
#define SPI_CS_SEL_V_CS3 (3)
#define SPI_CTRL_DATA_MUX_SEL_V_CTRL_REG_SEL (0)
#define SPI_CTRL_DATA_MUX_SEL_V_DATA_REG_SEL (1)

// status
#define SPI_IRQ_CAUSE(n) (((n)&0x1f) << 3)
#define SPI_ACTIVE_STATUS (1 << 0)
#define SPI_CAUSE_RX_OVF_IRQ (1 << 3)
#define SPI_CAUSE_TX_TH_IRQ (1 << 4)
#define SPI_CAUSE_TX_DMA_IRQ (1 << 5)
#define SPI_CAUSE_RX_TH_IRQ (1 << 6)
#define SPI_CAUSE_RX_DMA_IRQ (1 << 7)
#define SPI_TX_OVF (1 << 9)
#define SPI_RX_UDF (1 << 10)
#define SPI_RX_OVF (1 << 11)
#define SPI_TX_TH (1 << 12)
#define SPI_TX_DMA_DONE (1 << 13)
#define SPI_RX_TH (1 << 14)
#define SPI_RX_DMA_DONE (1 << 15)
#define SPI_TX_SPACE(n) (((n)&0x1f) << 16)
#define SPI_RX_LEVEL(n) (((n)&0x1f) << 24)
#define SPI_FIFO_FLUSH (1 << 30)

// rxtx_buffer
#define SPI_DATA_IN_OUT(n) (((n)&0x1fffffff) << 0)
#define SPI_CS(n) (((n)&0x3) << 29)
#define SPI_READ_ENA (1 << 31)

// cfg
#define SPI_CS_POLARITY(n) (((n)&0x7) << 0)
#define SPI_CS_POLARITY_0_ACTIVE_HIGH (0 << 0)
#define SPI_CS_POLARITY_0_ACTIVE_LOW (1 << 0)
#define SPI_CS_POLARITY_1_ACTIVE_HIGH (0 << 1)
#define SPI_CS_POLARITY_1_ACTIVE_LOW (1 << 1)
#define SPI_CS_POLARITY_2_ACTIVE_HIGH (0 << 2)
#define SPI_CS_POLARITY_2_ACTIVE_LOW (1 << 2)
#define SPI_CLOCK_DIVIDER(n) (((n)&0x3ff) << 16)
#define SPI_CLOCK_LIMITER (1 << 28)

#define SPI_CS_POLARITY_0_V_ACTIVE_HIGH (0)
#define SPI_CS_POLARITY_0_V_ACTIVE_LOW (1)
#define SPI_CS_POLARITY_1_V_ACTIVE_HIGH (0)
#define SPI_CS_POLARITY_1_V_ACTIVE_LOW (1)
#define SPI_CS_POLARITY_2_V_ACTIVE_HIGH (0)
#define SPI_CS_POLARITY_2_V_ACTIVE_LOW (1)

// pattern
#define SPI_PATTERN(n) (((n)&0xff) << 0)
#define SPI_PATTERN_MODE_DISABLED (0 << 8)
#define SPI_PATTERN_MODE_ENABLED (1 << 8)
#define SPI_PATTERN_SELECTOR_UNTIL (0 << 9)
#define SPI_PATTERN_SELECTOR_WHILE (1 << 9)

#define SPI_PATTERN_MODE_V_DISABLED (0)
#define SPI_PATTERN_MODE_V_ENABLED (1)
#define SPI_PATTERN_SELECTOR_V_UNTIL (0)
#define SPI_PATTERN_SELECTOR_V_WHILE (1)

// stream
#define SPI_TX_STREAM_BIT_ZERO (0 << 0)
#define SPI_TX_STREAM_BIT_ONE (1 << 0)
#define SPI_TX_STREAM_MODE_DISABLED (0 << 8)
#define SPI_TX_STREAM_MODE_ENABLED (1 << 8)
#define SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE_DISABLED (0 << 16)
#define SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE_ENABLED (1 << 16)

#define SPI_TX_STREAM_BIT_V_ZERO (0)
#define SPI_TX_STREAM_BIT_V_ONE (1)
#define SPI_TX_STREAM_MODE_V_DISABLED (0)
#define SPI_TX_STREAM_MODE_V_ENABLED (1)
#define SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE_V_DISABLED (0)
#define SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE_V_ENABLED (1)

// pin_control
#define SPI_CLK_CTRL(n) (((n)&0x3) << 0)
#define SPI_CLK_CTRL_SPI_CTRL (0 << 0)
#define SPI_CLK_CTRL_INPUT_CTRL (1 << 0)
#define SPI_CLK_CTRL_FORCE_0_CTRL (2 << 0)
#define SPI_CLK_CTRL_FORCE_1_CTRL (3 << 0)
#define SPI_DO_CTRL(n) (((n)&0x3) << 2)
#define SPI_DO_CTRL_SPI_CTRL (0 << 2)
#define SPI_DO_CTRL_INPUT_CTRL (1 << 2)
#define SPI_DO_CTRL_FORCE_0_CTRL (2 << 2)
#define SPI_DO_CTRL_FORCE_1_CTRL (3 << 2)
#define SPI_CS0_CTRL(n) (((n)&0x3) << 4)
#define SPI_CS0_CTRL_SPI_CTRL (0 << 4)
#define SPI_CS0_CTRL_INPUT_CTRL (1 << 4)
#define SPI_CS0_CTRL_FORCE_0_CTRL (2 << 4)
#define SPI_CS0_CTRL_FORCE_1_CTRL (3 << 4)
#define SPI_CS1_CTRL(n) (((n)&0x3) << 6)
#define SPI_CS1_CTRL_SPI_CTRL (0 << 6)
#define SPI_CS1_CTRL_INPUT_CTRL (1 << 6)
#define SPI_CS1_CTRL_FORCE_0_CTRL (2 << 6)
#define SPI_CS1_CTRL_FORCE_1_CTRL (3 << 6)
#define SPI_CS2_CTRL(n) (((n)&0x3) << 8)
#define SPI_CS2_CTRL_SPI_CTRL (0 << 8)
#define SPI_CS2_CTRL_INPUT_CTRL (1 << 8)
#define SPI_CS2_CTRL_FORCE_0_CTRL (2 << 8)
#define SPI_CS2_CTRL_FORCE_1_CTRL (3 << 8)

#define SPI_CLK_CTRL_V_SPI_CTRL (0)
#define SPI_CLK_CTRL_V_INPUT_CTRL (1)
#define SPI_CLK_CTRL_V_FORCE_0_CTRL (2)
#define SPI_CLK_CTRL_V_FORCE_1_CTRL (3)
#define SPI_DO_CTRL_V_SPI_CTRL (0)
#define SPI_DO_CTRL_V_INPUT_CTRL (1)
#define SPI_DO_CTRL_V_FORCE_0_CTRL (2)
#define SPI_DO_CTRL_V_FORCE_1_CTRL (3)
#define SPI_CS0_CTRL_V_SPI_CTRL (0)
#define SPI_CS0_CTRL_V_INPUT_CTRL (1)
#define SPI_CS0_CTRL_V_FORCE_0_CTRL (2)
#define SPI_CS0_CTRL_V_FORCE_1_CTRL (3)
#define SPI_CS1_CTRL_V_SPI_CTRL (0)
#define SPI_CS1_CTRL_V_INPUT_CTRL (1)
#define SPI_CS1_CTRL_V_FORCE_0_CTRL (2)
#define SPI_CS1_CTRL_V_FORCE_1_CTRL (3)
#define SPI_CS2_CTRL_V_SPI_CTRL (0)
#define SPI_CS2_CTRL_V_INPUT_CTRL (1)
#define SPI_CS2_CTRL_V_FORCE_0_CTRL (2)
#define SPI_CS2_CTRL_V_FORCE_1_CTRL (3)

// irq
#define SPI_IRQ_MASK(n) (((n)&0x1f) << 0)
#define SPI_MASK_RX_OVF_IRQ (1 << 0)
#define SPI_MASK_TX_TH_IRQ (1 << 1)
#define SPI_MASK_TX_DMA_IRQ (1 << 2)
#define SPI_MASK_RX_TH_IRQ (1 << 3)
#define SPI_MASK_RX_DMA_IRQ (1 << 4)
#define SPI_TX_THRESHOLD(n) (((n)&0x3) << 5)
#define SPI_TX_THRESHOLD_1_EMPTY_SLOT (0 << 5)
#define SPI_TX_THRESHOLD_4_EMPTY_SLOTS (1 << 5)
#define SPI_TX_THRESHOLD_8_EMPTY_SLOTS (2 << 5)
#define SPI_TX_THRESHOLD_12_EMPTY_SLOTS (3 << 5)
#define SPI_RX_THRESHOLD(n) (((n)&0x3) << 7)
#define SPI_RX_THRESHOLD_1_VALID_DATA (0 << 7)
#define SPI_RX_THRESHOLD_4_VALID_DATA (1 << 7)
#define SPI_RX_THRESHOLD_8_VALID_DATA (2 << 7)
#define SPI_RX_THRESHOLD_12_VALID_DATA (3 << 7)

#define SPI_TX_THRESHOLD_V_1_EMPTY_SLOT (0)
#define SPI_TX_THRESHOLD_V_4_EMPTY_SLOTS (1)
#define SPI_TX_THRESHOLD_V_8_EMPTY_SLOTS (2)
#define SPI_TX_THRESHOLD_V_12_EMPTY_SLOTS (3)
#define SPI_RX_THRESHOLD_V_1_VALID_DATA (0)
#define SPI_RX_THRESHOLD_V_4_VALID_DATA (1)
#define SPI_RX_THRESHOLD_V_8_VALID_DATA (2)
#define SPI_RX_THRESHOLD_V_12_VALID_DATA (3)

#endif // _SPI_H_
