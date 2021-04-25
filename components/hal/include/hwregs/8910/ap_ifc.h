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

#ifndef _AP_IFC_H_
#define _AP_IFC_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define AP_IFC_ADDR_ALIGN (0)
#define AP_IFC_TC_LEN (23)
#define AP_IFC_STD_CHAN_NB (7)
#define AP_IFC_RFSPI_CHAN (0)
#define AP_APB1_IFC_AHB_MAXSPACE (20)
#define AP_APB1_IFC_ADDR_ALIGN (2)

#define REG_AP_IFC_BASE (0x0880f000)

typedef volatile struct
{
    uint32_t get_ch;              // 0x00000000
    uint32_t dma_status;          // 0x00000004
    uint32_t debug_status;        // 0x00000008
    uint32_t __12[1];             // 0x0000000c
    struct                        // 0x00000010
    {                             //
        uint32_t control;         // 0x00000000
        uint32_t status;          // 0x00000004
        uint32_t start_addr;      // 0x00000008
        uint32_t tc;              // 0x0000000c
    } std_ch[7];                  //
    uint32_t ch_rfspi_control;    // 0x00000080
    uint32_t ch_rfspi_status;     // 0x00000084
    uint32_t ch_rfspi_start_addr; // 0x00000088
    uint32_t ch_rfspi_end_addr;   // 0x0000008c
    uint32_t ch_rfspi_tc;         // 0x00000090
    uint32_t __148[27];           // 0x00000094
} HWP_AP_IFC_T;

#define hwp_apIfc ((HWP_AP_IFC_T *)REG_ACCESS_ADDRESS(REG_AP_IFC_BASE))

#define REG_AUDIO_IFC_BASE (0x0880f100)

typedef volatile struct
{
    struct                     // 0x00000000
    {                          //
        uint32_t control;      // 0x00000000
        uint32_t status;       // 0x00000004
        uint32_t start_addr;   // 0x00000008
        uint32_t fifo_size;    // 0x0000000c
        uint32_t __16[1];      // 0x00000010
        uint32_t int_mask;     // 0x00000014
        uint32_t int_clear;    // 0x00000018
        uint32_t cur_ahb_addr; // 0x0000001c
    } ch[4];                   //
} HWP_AUDIO_IFC_T;

#define hwp_audioIfc ((HWP_AUDIO_IFC_T *)REG_ACCESS_ADDRESS(REG_AUDIO_IFC_BASE))

// get_ch
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ch_to_use : 4; // [3:0], read only
        uint32_t __31_4 : 28;   // [31:4]
    } b;
} REG_AP_IFC_GET_CH_T;

// dma_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ch_enable : 7; // [6:0], read only
        uint32_t __15_7 : 9;    // [15:7]
        uint32_t ch_busy : 7;   // [22:16], read only
        uint32_t __31_23 : 9;   // [31:23]
    } b;
} REG_AP_IFC_DMA_STATUS_T;

// debug_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbg_status : 1; // [0], read only
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_AP_IFC_DEBUG_STATUS_T;

// control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;      // [0]
        uint32_t disable : 1;     // [1]
        uint32_t rd_hw_exch : 1;  // [2]
        uint32_t wr_hw_exch : 1;  // [3]
        uint32_t autodisable : 1; // [4]
        uint32_t size : 1;        // [5]
        uint32_t __7_6 : 2;       // [7:6]
        uint32_t req_src : 5;     // [12:8]
        uint32_t __15_13 : 3;     // [15:13]
        uint32_t flush : 1;       // [16]
        uint32_t __31_17 : 15;    // [31:17]
    } b;
} REG_AP_IFC_CONTROL_T;

// status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;     // [0], read only
        uint32_t __3_1 : 3;      // [3:1]
        uint32_t fifo_empty : 1; // [4], read only
        uint32_t __31_5 : 27;    // [31:5]
    } b;
} REG_AP_IFC_STATUS_T;

// tc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tc : 23;     // [22:0]
        uint32_t __31_23 : 9; // [31:23]
    } b;
} REG_AP_IFC_TC_T;

// ch_rfspi_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;  // [0], write set
        uint32_t disable : 1; // [1], write clear
        uint32_t __31_2 : 30; // [31:2]
    } b;
} REG_AP_IFC_CH_RFSPI_CONTROL_T;

// ch_rfspi_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;     // [0], read only
        uint32_t __3_1 : 3;      // [3:1]
        uint32_t fifo_empty : 1; // [4], read only
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t fifo_level : 5; // [12:8], read only
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_AP_IFC_CH_RFSPI_STATUS_T;

// ch_rfspi_tc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tc : 14;      // [13:0]
        uint32_t __31_14 : 18; // [31:14]
    } b;
} REG_AP_IFC_CH_RFSPI_TC_T;

// control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;       // [0]
        uint32_t disable : 1;      // [1]
        uint32_t __3_2 : 2;        // [3:2]
        uint32_t auto_disable : 1; // [4]
        uint32_t __31_5 : 27;      // [31:5]
    } b;
} REG_AUDIO_IFC_CONTROL_T;

// status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;          // [0], read only
        uint32_t __3_1 : 3;           // [3:1]
        uint32_t fifo_empty : 1;      // [4], read only
        uint32_t __7_5 : 3;           // [7:5]
        uint32_t cause_ief : 1;       // [8], read only
        uint32_t cause_ihf : 1;       // [9], read only
        uint32_t cause_i4f : 1;       // [10], read only
        uint32_t cause_i3_4f : 1;     // [11], read only
        uint32_t cause_ahb_error : 1; // [12], read only
        uint32_t __15_13 : 3;         // [15:13]
        uint32_t ief : 1;             // [16], read only
        uint32_t ihf : 1;             // [17], read only
        uint32_t i4f : 1;             // [18], read only
        uint32_t i3_4f : 1;           // [19], read only
        uint32_t ahb_error : 1;       // [20], read only
        uint32_t ch_idle : 1;         // [21], read only
        uint32_t __31_22 : 10;        // [31:22]
    } b;
} REG_AUDIO_IFC_STATUS_T;

// start_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;       // [1:0]
        uint32_t start_addr : 30; // [31:2]
    } b;
} REG_AUDIO_IFC_START_ADDR_T;

// fifo_size
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __3_0 : 4;      // [3:0]
        uint32_t fifo_size : 16; // [19:4]
        uint32_t __31_20 : 12;   // [31:20]
    } b;
} REG_AUDIO_IFC_FIFO_SIZE_T;

// int_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __7_0 : 8;              // [7:0]
        uint32_t end_fifo : 1;           // [8]
        uint32_t half_fifo : 1;          // [9]
        uint32_t quarter_fifo : 1;       // [10]
        uint32_t three_quarter_fifo : 1; // [11]
        uint32_t ahb_error : 1;          // [12]
        uint32_t __31_13 : 19;           // [31:13]
    } b;
} REG_AUDIO_IFC_INT_MASK_T;

// int_clear
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __7_0 : 8;              // [7:0]
        uint32_t end_fifo : 1;           // [8], write clear
        uint32_t half_fifo : 1;          // [9], write clear
        uint32_t quarter_fifo : 1;       // [10], write clear
        uint32_t three_quarter_fifo : 1; // [11], write clear
        uint32_t ahb_error : 1;          // [12], write clear
        uint32_t __31_13 : 19;           // [31:13]
    } b;
} REG_AUDIO_IFC_INT_CLEAR_T;

// get_ch
#define AP_IFC_CH_TO_USE(n) (((n)&0xf) << 0)

// dma_status
#define AP_IFC_CH_ENABLE(n) (((n)&0x7f) << 0)
#define AP_IFC_CH_BUSY(n) (((n)&0x7f) << 16)

// debug_status
#define AP_IFC_DBG_STATUS (1 << 0)

// control
#define AP_IFC_ENABLE (1 << 0)
#define AP_IFC_DISABLE (1 << 1)
#define AP_IFC_RD_HW_EXCH (1 << 2)
#define AP_IFC_WR_HW_EXCH (1 << 3)
#define AP_IFC_AUTODISABLE (1 << 4)
#define AP_IFC_SIZE (1 << 5)
#define AP_IFC_REQ_SRC(n) (((n)&0x1f) << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SDMMC1_TX (0 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SDMMC1_RX (1 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SDMMC2_TX (2 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SDMMC2_RX (3 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SPI_1_TX (4 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SPI_1_RX (5 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SPI_2_TX (6 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SPI_2_RX (7 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SCI_1_TX (8 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SCI_1_RX (9 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SCI_2_TX (10 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_SCI_2_RX (11 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_FREE_TX (18 << 8)
#define AP_IFC_REQ_SRC_AP_APB_DMA_ID_CAMERA_RX (19 << 8)
#define AP_IFC_FLUSH (1 << 16)

#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SDMMC1_TX (0)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SDMMC1_RX (1)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SDMMC2_TX (2)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SDMMC2_RX (3)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SPI_1_TX (4)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SPI_1_RX (5)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SPI_2_TX (6)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SPI_2_RX (7)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SCI_1_TX (8)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SCI_1_RX (9)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SCI_2_TX (10)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_SCI_2_RX (11)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_FREE_TX (18)
#define AP_IFC_REQ_SRC_V_AP_APB_DMA_ID_CAMERA_RX (19)

// status
#define AP_IFC_ENABLE (1 << 0)
#define AP_IFC_FIFO_EMPTY (1 << 4)

// start_addr
#define AP_IFC_START_ADDR(n) (((n)&0xffffffff) << 0)

// tc
#define AP_IFC_TC_TC(n) (((n)&0x7fffff) << 0)

// ch_rfspi_control
#define AP_IFC_ENABLE (1 << 0)
#define AP_IFC_DISABLE (1 << 1)

// ch_rfspi_status
#define AP_IFC_ENABLE (1 << 0)
#define AP_IFC_FIFO_EMPTY (1 << 4)
#define AP_IFC_FIFO_LEVEL(n) (((n)&0x1f) << 8)

// ch_rfspi_start_addr
#define AP_IFC_START_AHB_ADDR(n) (((n)&0xffffffff) << 0)

// ch_rfspi_end_addr
#define AP_IFC_END_AHB_ADDR(n) (((n)&0xffffffff) << 0)

// ch_rfspi_tc
#define AP_IFC_CH_RFSPI_TC_TC(n) (((n)&0x3fff) << 0)

// control
#define AUDIO_IFC_ENABLE (1 << 0)
#define AUDIO_IFC_DISABLE (1 << 1)
#define AUDIO_IFC_AUTO_DISABLE (1 << 4)

// status
#define AUDIO_IFC_ENABLE (1 << 0)
#define AUDIO_IFC_FIFO_EMPTY (1 << 4)
#define AUDIO_IFC_CAUSE_IEF (1 << 8)
#define AUDIO_IFC_CAUSE_IHF (1 << 9)
#define AUDIO_IFC_CAUSE_I4F (1 << 10)
#define AUDIO_IFC_CAUSE_I3_4F (1 << 11)
#define AUDIO_IFC_CAUSE_AHB_ERROR (1 << 12)
#define AUDIO_IFC_IEF (1 << 16)
#define AUDIO_IFC_IHF (1 << 17)
#define AUDIO_IFC_I4F (1 << 18)
#define AUDIO_IFC_I3_4F (1 << 19)
#define AUDIO_IFC_STATUS_AHB_ERROR (1 << 20)
#define AUDIO_IFC_CH_IDLE (1 << 21)

// start_addr
#define AUDIO_IFC_START_ADDR(n) (((n)&0x3fffffff) << 2)

// fifo_size
#define AUDIO_IFC_FIFO_SIZE(n) (((n)&0xffff) << 4)

// int_mask
#define AUDIO_IFC_END_FIFO (1 << 8)
#define AUDIO_IFC_HALF_FIFO (1 << 9)
#define AUDIO_IFC_QUARTER_FIFO (1 << 10)
#define AUDIO_IFC_THREE_QUARTER_FIFO (1 << 11)
#define AUDIO_IFC_INT_MASK_AHB_ERROR (1 << 12)

// int_clear
#define AUDIO_IFC_END_FIFO (1 << 8)
#define AUDIO_IFC_HALF_FIFO (1 << 9)
#define AUDIO_IFC_QUARTER_FIFO (1 << 10)
#define AUDIO_IFC_THREE_QUARTER_FIFO (1 << 11)
#define AUDIO_IFC_INT_CLEAR_AHB_ERROR (1 << 12)

// cur_ahb_addr
#define AUDIO_IFC_CUR_AHB_ADDR(n) (((n)&0xffffffff) << 0)

#endif // _AP_IFC_H_
