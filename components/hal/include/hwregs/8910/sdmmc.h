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

#ifndef _SDMMC_H_
#define _SDMMC_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_SDMMC_BASE (0x08810000)
#define REG_SDMMC2_BASE (0x08811000)

typedef volatile struct
{
    uint32_t apbi_ctrl_sdmmc;       // 0x00000000
    uint32_t __4[1];                // 0x00000004
    uint32_t apbi_fifo_txrx;        // 0x00000008
    uint32_t __12[509];             // 0x0000000c
    uint32_t sdmmc_config;          // 0x00000800
    uint32_t sdmmc_status;          // 0x00000804
    uint32_t sdmmc_cmd_index;       // 0x00000808
    uint32_t sdmmc_cmd_arg;         // 0x0000080c
    uint32_t sdmmc_resp_index;      // 0x00000810
    uint32_t sdmmc_resp_arg3;       // 0x00000814
    uint32_t sdmmc_resp_arg2;       // 0x00000818
    uint32_t sdmmc_resp_arg1;       // 0x0000081c
    uint32_t sdmmc_resp_arg0;       // 0x00000820
    uint32_t sdmmc_data_width_reg;  // 0x00000824
    uint32_t sdmmc_block_size_reg;  // 0x00000828
    uint32_t sdmmc_block_cnt_reg;   // 0x0000082c
    uint32_t sdmmc_int_status;      // 0x00000830
    uint32_t sdmmc_int_mask;        // 0x00000834
    uint32_t sdmmc_int_clear;       // 0x00000838
    uint32_t sdmmc_trans_speed_reg; // 0x0000083c
    uint32_t sdmmc_mclk_adjust_reg; // 0x00000840
} HWP_SDMMC_T;

#define hwp_sdmmc ((HWP_SDMMC_T *)REG_ACCESS_ADDRESS(REG_SDMMC_BASE))
#define hwp_sdmmc2 ((HWP_SDMMC_T *)REG_ACCESS_ADDRESS(REG_SDMMC2_BASE))

// apbi_ctrl_sdmmc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t l_endian : 3;   // [2:0]
        uint32_t soft_rst_l : 1; // [3]
        uint32_t __31_4 : 28;    // [31:4]
    } b;
} REG_SDMMC_APBI_CTRL_SDMMC_T;

// apbi_fifo_txrx
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data_in : 32;  // [31:0]
        uint32_t data_out : 32; // [31:0], read only
    } b;
} REG_SDMMC_APBI_FIFO_TXRX_T;

// sdmmc_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdmmc_sendcmd : 1; // [0]
        uint32_t sdmmc_suspend : 1; // [1]
        uint32_t __3_2 : 2;         // [3:2]
        uint32_t rsp_en : 1;        // [4]
        uint32_t rsp_sel : 2;       // [6:5]
        uint32_t __7_7 : 1;         // [7]
        uint32_t rd_wt_en : 1;      // [8]
        uint32_t rd_wt_sel : 1;     // [9]
        uint32_t s_m_sel : 1;       // [10]
        uint32_t __15_11 : 5;       // [15:11]
        uint32_t bit_16 : 1;        // [16]
        uint32_t __31_17 : 15;      // [31:17]
    } b;
} REG_SDMMC_SDMMC_CONFIG_T;

// sdmmc_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t not_sdmmc_over : 1; // [0], read only
        uint32_t busy : 1;           // [1], read only
        uint32_t dl_busy : 1;        // [2], read only
        uint32_t suspend : 1;        // [3], read only
        uint32_t __7_4 : 4;          // [7:4]
        uint32_t rsp_error : 1;      // [8], read only
        uint32_t no_rsp_error : 1;   // [9], read only
        uint32_t __11_10 : 2;        // [11:10]
        uint32_t crc_status : 3;     // [14:12], read only
        uint32_t __15_15 : 1;        // [15]
        uint32_t data_error : 8;     // [23:16], read only
        uint32_t dat3_val : 1;       // [24], read only
        uint32_t __31_25 : 7;        // [31:25]
    } b;
} REG_SDMMC_SDMMC_STATUS_T;

// sdmmc_cmd_index
typedef union {
    uint32_t v;
    struct
    {
        uint32_t command : 6; // [5:0]
        uint32_t __31_6 : 26; // [31:6]
    } b;
} REG_SDMMC_SDMMC_CMD_INDEX_T;

// sdmmc_resp_index
typedef union {
    uint32_t v;
    struct
    {
        uint32_t response : 6; // [5:0], read only
        uint32_t __31_6 : 26;  // [31:6]
    } b;
} REG_SDMMC_SDMMC_RESP_INDEX_T;

// sdmmc_data_width_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdmmc_data_width : 4; // [3:0]
        uint32_t __31_4 : 28;          // [31:4]
    } b;
} REG_SDMMC_SDMMC_DATA_WIDTH_REG_T;

// sdmmc_block_size_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdmmc_block_size : 4; // [3:0]
        uint32_t __31_4 : 28;          // [31:4]
    } b;
} REG_SDMMC_SDMMC_BLOCK_SIZE_REG_T;

// sdmmc_block_cnt_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdmmc_block_cnt : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_SDMMC_SDMMC_BLOCK_CNT_REG_T;

// sdmmc_int_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t no_rsp_int : 1;     // [0], read only
        uint32_t rsp_err_int : 1;    // [1], read only
        uint32_t rd_err_int : 1;     // [2], read only
        uint32_t wr_err_int : 1;     // [3], read only
        uint32_t dat_over_int : 1;   // [4], read only
        uint32_t txdma_done_int : 1; // [5], read only
        uint32_t rxdma_done_int : 1; // [6], read only
        uint32_t __7_7 : 1;          // [7]
        uint32_t no_rsp_sc : 1;      // [8], read only
        uint32_t rsp_err_sc : 1;     // [9], read only
        uint32_t rd_err_sc : 1;      // [10], read only
        uint32_t wr_err_sc : 1;      // [11], read only
        uint32_t dat_over_sc : 1;    // [12], read only
        uint32_t txdma_done_sc : 1;  // [13], read only
        uint32_t rxdma_done_sc : 1;  // [14], read only
        uint32_t __31_15 : 17;       // [31:15]
    } b;
} REG_SDMMC_SDMMC_INT_STATUS_T;

// sdmmc_int_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t no_rsp_mk : 1;     // [0]
        uint32_t rsp_err_mk : 1;    // [1]
        uint32_t rd_err_mk : 1;     // [2]
        uint32_t wr_err_mk : 1;     // [3]
        uint32_t dat_over_mk : 1;   // [4]
        uint32_t txdma_done_mk : 1; // [5]
        uint32_t rxdma_done_mk : 1; // [6]
        uint32_t __31_7 : 25;       // [31:7]
    } b;
} REG_SDMMC_SDMMC_INT_MASK_T;

// sdmmc_int_clear
typedef union {
    uint32_t v;
    struct
    {
        uint32_t no_rsp_cl : 1;     // [0]
        uint32_t rsp_err_cl : 1;    // [1]
        uint32_t rd_err_cl : 1;     // [2]
        uint32_t wr_err_cl : 1;     // [3]
        uint32_t dat_over_cl : 1;   // [4]
        uint32_t txdma_done_cl : 1; // [5]
        uint32_t rxdma_done_cl : 1; // [6]
        uint32_t __31_7 : 25;       // [31:7]
    } b;
} REG_SDMMC_SDMMC_INT_CLEAR_T;

// sdmmc_trans_speed_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdmmc_trans_speed : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_SDMMC_SDMMC_TRANS_SPEED_REG_T;

// sdmmc_mclk_adjust_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdmmc_mclk_adjust : 4; // [3:0]
        uint32_t clk_inv : 1;           // [4]
        uint32_t __31_5 : 27;           // [31:5]
    } b;
} REG_SDMMC_SDMMC_MCLK_ADJUST_REG_T;

// apbi_ctrl_sdmmc
#define SDMMC_L_ENDIAN(n) (((n)&0x7) << 0)
#define SDMMC_SOFT_RST_L (1 << 3)

// apbi_fifo_txrx
#define SDMMC_DATA_IN(n) (((n)&0xffffffff) << 0)
#define SDMMC_DATA_OUT(n) (((n)&0xffffffff) << 0)

// sdmmc_config
#define SDMMC_SDMMC_SENDCMD (1 << 0)
#define SDMMC_SDMMC_SUSPEND (1 << 1)
#define SDMMC_RSP_EN (1 << 4)
#define SDMMC_RSP_SEL(n) (((n)&0x3) << 5)
#define SDMMC_RSP_SEL_R2 (2 << 5)
#define SDMMC_RSP_SEL_R3 (1 << 5)
#define SDMMC_RSP_SEL_OTHER (0 << 5)
#define SDMMC_RD_WT_EN (1 << 8)
#define SDMMC_RD_WT_SEL_READ (0 << 9)
#define SDMMC_RD_WT_SEL_WRITE (1 << 9)
#define SDMMC_S_M_SEL_SIMPLE (0 << 10)
#define SDMMC_S_M_SEL_MULTIPLE (1 << 10)
#define SDMMC_BIT_16 (1 << 16)

#define SDMMC_RSP_SEL_V_R2 (2)
#define SDMMC_RSP_SEL_V_R3 (1)
#define SDMMC_RSP_SEL_V_OTHER (0)
#define SDMMC_RD_WT_SEL_V_READ (0)
#define SDMMC_RD_WT_SEL_V_WRITE (1)
#define SDMMC_S_M_SEL_V_SIMPLE (0)
#define SDMMC_S_M_SEL_V_MULTIPLE (1)

// sdmmc_status
#define SDMMC_NOT_SDMMC_OVER (1 << 0)
#define SDMMC_BUSY (1 << 1)
#define SDMMC_DL_BUSY (1 << 2)
#define SDMMC_SUSPEND (1 << 3)
#define SDMMC_RSP_ERROR (1 << 8)
#define SDMMC_NO_RSP_ERROR (1 << 9)
#define SDMMC_CRC_STATUS(n) (((n)&0x7) << 12)
#define SDMMC_DATA_ERROR(n) (((n)&0xff) << 16)
#define SDMMC_DAT3_VAL (1 << 24)

// sdmmc_cmd_index
#define SDMMC_COMMAND(n) (((n)&0x3f) << 0)

// sdmmc_cmd_arg
#define SDMMC_ARGUMENT(n) (((n)&0xffffffff) << 0)

// sdmmc_resp_index
#define SDMMC_RESPONSE(n) (((n)&0x3f) << 0)

// sdmmc_resp_arg3
#define SDMMC_ARGUMENT3(n) (((n)&0xffffffff) << 0)

// sdmmc_resp_arg2
#define SDMMC_ARGUMENT2(n) (((n)&0xffffffff) << 0)

// sdmmc_resp_arg1
#define SDMMC_ARGUMENT1(n) (((n)&0xffffffff) << 0)

// sdmmc_resp_arg0
#define SDMMC_ARGUMENT0(n) (((n)&0xffffffff) << 0)

// sdmmc_data_width_reg
#define SDMMC_SDMMC_DATA_WIDTH(n) (((n)&0xf) << 0)

// sdmmc_block_size_reg
#define SDMMC_SDMMC_BLOCK_SIZE(n) (((n)&0xf) << 0)

// sdmmc_block_cnt_reg
#define SDMMC_SDMMC_BLOCK_CNT(n) (((n)&0xffff) << 0)

// sdmmc_int_status
#define SDMMC_NO_RSP_INT (1 << 0)
#define SDMMC_RSP_ERR_INT (1 << 1)
#define SDMMC_RD_ERR_INT (1 << 2)
#define SDMMC_WR_ERR_INT (1 << 3)
#define SDMMC_DAT_OVER_INT (1 << 4)
#define SDMMC_TXDMA_DONE_INT (1 << 5)
#define SDMMC_RXDMA_DONE_INT (1 << 6)
#define SDMMC_NO_RSP_SC (1 << 8)
#define SDMMC_RSP_ERR_SC (1 << 9)
#define SDMMC_RD_ERR_SC (1 << 10)
#define SDMMC_WR_ERR_SC (1 << 11)
#define SDMMC_DAT_OVER_SC (1 << 12)
#define SDMMC_TXDMA_DONE_SC (1 << 13)
#define SDMMC_RXDMA_DONE_SC (1 << 14)

// sdmmc_int_mask
#define SDMMC_NO_RSP_MK (1 << 0)
#define SDMMC_RSP_ERR_MK (1 << 1)
#define SDMMC_RD_ERR_MK (1 << 2)
#define SDMMC_WR_ERR_MK (1 << 3)
#define SDMMC_DAT_OVER_MK (1 << 4)
#define SDMMC_TXDMA_DONE_MK (1 << 5)
#define SDMMC_RXDMA_DONE_MK (1 << 6)

// sdmmc_int_clear
#define SDMMC_NO_RSP_CL (1 << 0)
#define SDMMC_RSP_ERR_CL (1 << 1)
#define SDMMC_RD_ERR_CL (1 << 2)
#define SDMMC_WR_ERR_CL (1 << 3)
#define SDMMC_DAT_OVER_CL (1 << 4)
#define SDMMC_TXDMA_DONE_CL (1 << 5)
#define SDMMC_RXDMA_DONE_CL (1 << 6)

// sdmmc_trans_speed_reg
#define SDMMC_SDMMC_TRANS_SPEED(n) (((n)&0xff) << 0)

// sdmmc_mclk_adjust_reg
#define SDMMC_SDMMC_MCLK_ADJUST(n) (((n)&0xf) << 0)
#define SDMMC_CLK_INV (1 << 4)

#endif // _SDMMC_H_
