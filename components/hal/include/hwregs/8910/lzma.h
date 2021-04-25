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

#ifndef _LZMA_H_
#define _LZMA_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_LZMA_BASE (0x08805000)

typedef volatile struct
{
    uint32_t lzma_cmd_reg;            // 0x00000000
    uint32_t lzma_status_reg;         // 0x00000004
    uint32_t lzma_irq_mask;           // 0x00000008
    uint32_t reserve0;                // 0x0000000c
    uint32_t lzma_config_reg1;        // 0x00000010
    uint32_t lzma_config_reg2;        // 0x00000014
    uint32_t lzma_config_reg3;        // 0x00000018
    uint32_t lzma_status_reg2;        // 0x0000001c
    uint32_t lzma_status_reg3;        // 0x00000020
    uint32_t lzma_error_type;         // 0x00000024
    uint32_t reserve1;                // 0x00000028
    uint32_t reserve2;                // 0x0000002c
    uint32_t lzma_input_crc;          // 0x00000030
    uint32_t lzma_output_crc;         // 0x00000034
    uint32_t reserve3;                // 0x00000038
    uint32_t reserve4;                // 0x0000003c
    uint32_t lzma_dma_raddr_reg;      // 0x00000040
    uint32_t lzma_dma_waddr_reg;      // 0x00000044
    uint32_t lzma_inbuf_rwmargin_reg; // 0x00000048
} HWP_LZMA_T;

#define hwp_lzma ((HWP_LZMA_T *)REG_ACCESS_ADDRESS(REG_LZMA_BASE))

// lzma_cmd_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;   // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_LZMA_LZMA_CMD_REG_T;

// lzma_status_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dec_done : 1; // [0]
        uint32_t dec_err : 1;  // [1]
        uint32_t axi_err : 1;  // [2]
        uint32_t __31_3 : 29;  // [31:3]
    } b;
} REG_LZMA_LZMA_STATUS_REG_T;

// lzma_irq_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dec_doneirqmask : 1; // [0]
        uint32_t dec_errirqmask : 1;  // [1]
        uint32_t axi_errirqmask : 1;  // [2]
        uint32_t __31_3 : 29;         // [31:3]
    } b;
} REG_LZMA_LZMA_IRQ_MASK_T;

// lzma_config_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_block_size : 17; // [16:0]
        uint32_t reg_dict_size : 13;  // [29:17]
        uint32_t __31_30 : 2;         // [31:30]
    } b;
} REG_LZMA_LZMA_CONFIG_REG1_T;

// lzma_config_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_stream_len : 17; // [16:0]
        uint32_t __31_17 : 15;        // [31:17]
    } b;
} REG_LZMA_LZMA_CONFIG_REG2_T;

// lzma_config_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_cabac_totalbits : 1; // [0]
        uint32_t reg_cabac_movebits : 1;  // [1]
        uint32_t reg_refbyte_en : 1;      // [2]
        uint32_t __31_3 : 29;             // [31:3]
    } b;
} REG_LZMA_LZMA_CONFIG_REG3_T;

// lzma_status_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t stream_byte_pos : 17; // [16:0], read only
        uint32_t __31_17 : 15;         // [31:17]
    } b;
} REG_LZMA_LZMA_STATUS_REG2_T;

// lzma_status_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dict_byte_pos : 17; // [16:0], read only
        uint32_t __31_17 : 15;       // [31:17]
    } b;
} REG_LZMA_LZMA_STATUS_REG3_T;

// lzma_error_type
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inbuf_overflow : 1;   // [0], read only
        uint32_t symbol_type_err : 1;  // [1], read only
        uint32_t symbol_reps_err1 : 1; // [2], read only
        uint32_t symbol_reps_err0 : 1; // [3], read only
        uint32_t symbol_len_err : 1;   // [4], read only
        uint32_t outbuf_overflow : 1;  // [5], read only
        uint32_t inbuf_underflow : 1;  // [6], read only
        uint32_t __31_7 : 25;          // [31:7]
    } b;
} REG_LZMA_LZMA_ERROR_TYPE_T;

// lzma_inbuf_rwmargin_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inbuf_rwmargin_reg : 6; // [5:0]
        uint32_t __31_6 : 26;            // [31:6]
    } b;
} REG_LZMA_LZMA_INBUF_RWMARGIN_REG_T;

// lzma_cmd_reg
#define LZMA_START (1 << 0)

// lzma_status_reg
#define LZMA_DEC_DONE (1 << 0)
#define LZMA_DEC_ERR (1 << 1)
#define LZMA_AXI_ERR (1 << 2)

// lzma_irq_mask
#define LZMA_DEC_DONEIRQMASK (1 << 0)
#define LZMA_DEC_ERRIRQMASK (1 << 1)
#define LZMA_AXI_ERRIRQMASK (1 << 2)

// lzma_config_reg1
#define LZMA_REG_BLOCK_SIZE(n) (((n)&0x1ffff) << 0)
#define LZMA_REG_DICT_SIZE(n) (((n)&0x1fff) << 17)

// lzma_config_reg2
#define LZMA_REG_STREAM_LEN(n) (((n)&0x1ffff) << 0)

// lzma_config_reg3
#define LZMA_REG_CABAC_TOTALBITS (1 << 0)
#define LZMA_REG_CABAC_MOVEBITS (1 << 1)
#define LZMA_REG_REFBYTE_EN (1 << 2)

// lzma_status_reg2
#define LZMA_STREAM_BYTE_POS(n) (((n)&0x1ffff) << 0)

// lzma_status_reg3
#define LZMA_DICT_BYTE_POS(n) (((n)&0x1ffff) << 0)

// lzma_error_type
#define LZMA_INBUF_OVERFLOW (1 << 0)
#define LZMA_SYMBOL_TYPE_ERR (1 << 1)
#define LZMA_SYMBOL_REPS_ERR1 (1 << 2)
#define LZMA_SYMBOL_REPS_ERR0 (1 << 3)
#define LZMA_SYMBOL_LEN_ERR (1 << 4)
#define LZMA_OUTBUF_OVERFLOW (1 << 5)
#define LZMA_INBUF_UNDERFLOW (1 << 6)

// lzma_input_crc
#define LZMA_INPUT_CRC(n) (((n)&0xffffffff) << 0)

// lzma_output_crc
#define LZMA_OUTPUT_CRC(n) (((n)&0xffffffff) << 0)

// lzma_dma_raddr_reg
#define LZMA_DMA_RADDR_REG(n) (((n)&0xffffffff) << 0)

// lzma_dma_waddr_reg
#define LZMA_DMA_WADDR_REG(n) (((n)&0xffffffff) << 0)

// lzma_inbuf_rwmargin_reg
#define LZMA_INBUF_RWMARGIN_REG(n) (((n)&0x3f) << 0)

#endif // _LZMA_H_
