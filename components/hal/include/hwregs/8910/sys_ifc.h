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

#ifndef _SYS_IFC_H_
#define _SYS_IFC_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define SYS_IFC_ADDR_ALIGN (0)
#define SYS_IFC_TC_LEN (23)
#define SYS_IFC_STD_CHAN_NB (2)
#define SYS_IFC_RFSPI_CHAN (2)

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_SYS_IFC_BASE (0x00026000)
#else
#define REG_SYS_IFC_BASE (0x40026000)
#endif

typedef volatile struct
{
    uint32_t get_ch;                  // 0x00000000
    uint32_t dma_status;              // 0x00000004
    uint32_t debug_status;            // 0x00000008
    uint32_t __12[1];                 // 0x0000000c
    struct                            // 0x00000010
    {                                 //
        uint32_t control;             // 0x00000000
        uint32_t status;              // 0x00000004
        uint32_t start_addr;          // 0x00000008
        uint32_t tc;                  // 0x0000000c
    } std_ch[2];                      //
    uint32_t __48[20];                // 0x00000030
    struct                            // 0x00000080
    {                                 //
        uint32_t ch_rfspi_control;    // 0x00000000
        uint32_t ch_rfspi_status;     // 0x00000004
        uint32_t ch_rfspi_start_addr; // 0x00000008
        uint32_t ch_rfspi_end_addr;   // 0x0000000c
        uint32_t ch_rfspi_tc;         // 0x00000010
    } rfspi_ch[2];                    //
} HWP_SYS_IFC_T;

#define hwp_sysIfc ((HWP_SYS_IFC_T *)REG_ACCESS_ADDRESS(REG_SYS_IFC_BASE))

// get_ch
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ch_to_use : 4; // [3:0], read only
        uint32_t __31_4 : 28;   // [31:4]
    } b;
} REG_SYS_IFC_GET_CH_T;

// dma_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ch_enable : 4; // [3:0], read only
        uint32_t __15_4 : 12;   // [15:4]
        uint32_t ch_busy : 2;   // [17:16], read only
        uint32_t __31_18 : 14;  // [31:18]
    } b;
} REG_SYS_IFC_DMA_STATUS_T;

// debug_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbg_status : 1; // [0], read only
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_SYS_IFC_DEBUG_STATUS_T;

// control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;           // [0]
        uint32_t disable : 1;          // [1]
        uint32_t ch_rd_hw_exch : 1;    // [2]
        uint32_t ch_wr_hw_exch : 1;    // [3]
        uint32_t autodisable : 1;      // [4]
        uint32_t size : 1;             // [5]
        uint32_t __7_6 : 2;            // [7:6]
        uint32_t req_src : 5;          // [12:8]
        uint32_t __15_13 : 3;          // [15:13]
        uint32_t flush : 1;            // [16]
        uint32_t max_burst_length : 2; // [18:17]
        uint32_t __31_19 : 13;         // [31:19]
    } b;
} REG_SYS_IFC_CONTROL_T;

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
} REG_SYS_IFC_STATUS_T;

// tc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tc : 23;     // [22:0]
        uint32_t __31_23 : 9; // [31:23]
    } b;
} REG_SYS_IFC_TC_T;

// ch_rfspi_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;  // [0], write set
        uint32_t disable : 1; // [1], write clear
        uint32_t __31_2 : 30; // [31:2]
    } b;
} REG_SYS_IFC_CH_RFSPI_CONTROL_T;

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
} REG_SYS_IFC_CH_RFSPI_STATUS_T;

// ch_rfspi_tc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tc : 14;      // [13:0]
        uint32_t __31_14 : 18; // [31:14]
    } b;
} REG_SYS_IFC_CH_RFSPI_TC_T;

// get_ch
#define SYS_IFC_CH_TO_USE(n) (((n)&0xf) << 0)

// dma_status
#define SYS_IFC_CH_ENABLE(n) (((n)&0xf) << 0)
#define SYS_IFC_CH_BUSY(n) (((n)&0x3) << 16)

// debug_status
#define SYS_IFC_DBG_STATUS (1 << 0)

// control
#define SYS_IFC_ENABLE (1 << 0)
#define SYS_IFC_DISABLE (1 << 1)
#define SYS_IFC_CH_RD_HW_EXCH (1 << 2)
#define SYS_IFC_CH_WR_HW_EXCH (1 << 3)
#define SYS_IFC_AUTODISABLE (1 << 4)
#define SYS_IFC_SIZE (1 << 5)
#define SYS_IFC_REQ_SRC(n) (((n)&0x1f) << 8)
#define SYS_IFC_FLUSH (1 << 16)
#define SYS_IFC_MAX_BURST_LENGTH(n) (((n)&0x3) << 17)

// status
#define SYS_IFC_ENABLE (1 << 0)
#define SYS_IFC_FIFO_EMPTY (1 << 4)

// start_addr
#define SYS_IFC_START_ADDR(n) (((n)&0xffffffff) << 0)

// tc
#define SYS_IFC_TC_TC(n) (((n)&0x7fffff) << 0)

// ch_rfspi_control
#define SYS_IFC_ENABLE (1 << 0)
#define SYS_IFC_DISABLE (1 << 1)

// ch_rfspi_status
#define SYS_IFC_ENABLE (1 << 0)
#define SYS_IFC_FIFO_EMPTY (1 << 4)
#define SYS_IFC_FIFO_LEVEL(n) (((n)&0x1f) << 8)

// ch_rfspi_start_addr
#define SYS_IFC_START_AHB_ADDR(n) (((n)&0xffffffff) << 0)

// ch_rfspi_end_addr
#define SYS_IFC_END_AHB_ADDR(n) (((n)&0xffffffff) << 0)

// ch_rfspi_tc
#define SYS_IFC_CH_RFSPI_TC_TC(n) (((n)&0x3fff) << 0)

#endif // _SYS_IFC_H_
