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

#ifndef _AES_H_
#define _AES_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define CIOS_RAM_SIZE (1024)

#define REG_AES_BASE (0x09140000)

typedef volatile struct
{
    uint32_t dma_ctrl;            // 0x00000000
    uint32_t dma_src;             // 0x00000004
    uint32_t dma_dst;             // 0x00000008
    uint32_t dma_len;             // 0x0000000c
    uint32_t crc_gen;             // 0x00000010
    uint32_t dma_func_ctrl;       // 0x00000014
    uint32_t aes_key0;            // 0x00000018
    uint32_t aes_key1;            // 0x0000001c
    uint32_t aes_key2;            // 0x00000020
    uint32_t aes_key3;            // 0x00000024
    uint32_t aes_iv0;             // 0x00000028
    uint32_t aes_iv1;             // 0x0000002c
    uint32_t aes_iv2;             // 0x00000030
    uint32_t aes_iv3;             // 0x00000034
    uint32_t aes_mode;            // 0x00000038
    uint32_t cios_ctrl;           // 0x0000003c
    uint32_t cios_reg0;           // 0x00000040
    uint32_t crc_init_val;        // 0x00000044
    uint32_t crc_out_xorval;      // 0x00000048
    uint32_t crc_out_val;         // 0x0000004c
    uint32_t crc_size_val;        // 0x00000050
    uint32_t ififo_thr;           // 0x00000054
    uint32_t ofifo_thr;           // 0x00000058
    uint32_t __92[9];             // 0x0000005c
    uint32_t dma_int_out;         // 0x00000080
    uint32_t dma_int_mask;        // 0x00000084
    uint32_t __136[30];           // 0x00000088
    uint32_t trng_ctrl;           // 0x00000100
    uint32_t prng_ctrl;           // 0x00000104
    uint32_t prng_seed;           // 0x00000108
    uint32_t prng_timer_init;     // 0x0000010c
    uint32_t prng_timer;          // 0x00000110
    uint32_t trng_data0;          // 0x00000114
    uint32_t trng_data0_mask;     // 0x00000118
    uint32_t trng_data1;          // 0x0000011c
    uint32_t trng_data1_mask;     // 0x00000120
    uint32_t prng_data;           // 0x00000124
    uint32_t trng_hc;             // 0x00000128
    uint32_t __300[181];          // 0x0000012c
    uint32_t sha_command;         // 0x00000400
    uint32_t sha_message_addr;    // 0x00000404
    uint32_t sha_message_len;     // 0x00000408
    uint32_t sha_rd_counter;      // 0x0000040c
    uint32_t sha_wr_counter;      // 0x00000410
    uint32_t sha_umac_state_c;    // 0x00000414
    uint32_t sha_state_c;         // 0x00000418
    uint32_t __1052[6];           // 0x0000041c
    uint32_t sha_int_clr;         // 0x00000434
    uint32_t sha_int_en;          // 0x00000438
    uint32_t sha_int_status;      // 0x0000043c
    uint32_t sha_restart;         // 0x00000440
    uint32_t __1092[47];          // 0x00000444
    uint32_t hash_value_0;        // 0x00000500
    uint32_t hash_value_1;        // 0x00000504
    uint32_t hash_value_2;        // 0x00000508
    uint32_t hash_value_3;        // 0x0000050c
    uint32_t hash_value_4;        // 0x00000510
    uint32_t hash_value_5;        // 0x00000514
    uint32_t hash_value_6;        // 0x00000518
    uint32_t hash_value_7;        // 0x0000051c
    uint32_t __1312[8];           // 0x00000520
    uint32_t sha_byte_count;      // 0x00000540
    uint32_t __1348[175];         // 0x00000544
    uint8_t cios_local_ram[1024]; // 0x00000800
} HWP_AES_T;

#define hwp_aes ((HWP_AES_T *)REG_ACCESS_ADDRESS(REG_AES_BASE))

// dma_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fix_src : 1;     // [0]
        uint32_t fix_dst : 1;     // [1]
        uint32_t hsizem : 3;      // [4:2]
        uint32_t cycle2_3 : 1;    // [5]
        uint32_t __27_6 : 22;     // [27:6]
        uint32_t crc_ce_ctrl : 3; // [30:28]
        uint32_t __31_31 : 1;     // [31]
    } b;
} REG_AES_DMA_CTRL_T;

// dma_len
typedef union {
    uint32_t v;
    struct
    {
        uint32_t length : 18;  // [17:0]
        uint32_t __31_18 : 14; // [31:18]
    } b;
} REG_AES_DMA_LEN_T;

// dma_func_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t crc_gen_len : 2;     // [1:0]
        uint32_t __15_2 : 14;         // [15:2]
        uint32_t crc_ctrl : 4;        // [19:16]
        uint32_t __23_20 : 4;         // [23:20]
        uint32_t aes_eng_cgen : 1;    // [24]
        uint32_t aes_keygen_cgen : 1; // [25]
        uint32_t __27_26 : 2;         // [27:26]
        uint32_t crc_cgen : 1;        // [28]
        uint32_t __29_29 : 1;         // [29]
        uint32_t trng_cgen : 1;       // [30]
        uint32_t __31_31 : 1;         // [31]
    } b;
} REG_AES_DMA_FUNC_CTRL_T;

// aes_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode : 1;           // [0]
        uint32_t key_start : 1;      // [1]
        uint32_t key_start_mode : 1; // [2]
        uint32_t __31_3 : 29;        // [31:3]
    } b;
} REG_AES_AES_MODE_T;

// cios_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cios_ram_en : 1;        // [0]
        uint32_t cios_mode : 3;          // [3:1]
        uint32_t cios_force_ram : 1;     // [4]
        uint32_t cios_clk_en : 1;        // [5]
        uint32_t cios_start_mode : 1;    // [6]
        uint32_t cios_start_compute : 1; // [7]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_AES_CIOS_CTRL_T;

// crc_size_val
typedef union {
    uint32_t v;
    struct
    {
        uint32_t crc_size : 3; // [2:0]
        uint32_t __31_3 : 29;  // [31:3]
    } b;
} REG_AES_CRC_SIZE_VAL_T;

// ififo_thr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ififo_rd_low_thr : 7;   // [6:0]
        uint32_t __7_7 : 1;              // [7]
        uint32_t ififo_rd_hight_thr : 7; // [14:8]
        uint32_t __31_15 : 17;           // [31:15]
    } b;
} REG_AES_IFIFO_THR_T;

// ofifo_thr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ofifo_wr_low_thr : 7;   // [6:0]
        uint32_t __7_7 : 1;              // [7]
        uint32_t ofifo_wr_hight_thr : 7; // [14:8]
        uint32_t __31_15 : 17;           // [31:15]
    } b;
} REG_AES_OFIFO_THR_T;

// dma_int_out
typedef union {
    uint32_t v;
    struct
    {
        uint32_t int_out : 6; // [5:0], read only
        uint32_t __31_6 : 26; // [31:6]
    } b;
} REG_AES_DMA_INT_OUT_T;

// dma_int_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t int_mask : 6; // [5:0]
        uint32_t __31_6 : 26;  // [31:6]
    } b;
} REG_AES_DMA_INT_MASK_T;

// trng_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t trng_ctrl : 7; // [6:0]
        uint32_t __15_7 : 9;    // [15:7]
        uint32_t trng_mask : 8; // [23:16]
        uint32_t __31_24 : 8;   // [31:24]
    } b;
} REG_AES_TRNG_CTRL_T;

// prng_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prng_ctrl : 4; // [3:0]
        uint32_t __31_4 : 28;   // [31:4]
    } b;
} REG_AES_PRNG_CTRL_T;

// prng_timer_init
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prng_clk_sel : 1; // [0]
        uint32_t prng_init : 31;   // [31:1]
    } b;
} REG_AES_PRNG_TIMER_INIT_T;

// trng_hc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t trng_c_value : 16; // [15:0], read only
        uint32_t trng_h_value : 16; // [31:16], read only
    } b;
} REG_AES_TRNG_HC_T;

// sha_command
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_start : 1;             // [0]
        uint32_t sha_complete : 1;          // [1], read only
        uint32_t sha_message_end : 1;       // [2]
        uint32_t sha_message_start : 1;     // [3]
        uint32_t sha_algorithm : 4;         // [7:4]
        uint32_t sha_int_on_completion : 1; // [8]
        uint32_t sha_msg_endian_mode : 1;   // [9]
        uint32_t sha_hash_endian_mode : 1;  // [10]
        uint32_t sha_rev_16_11 : 6;         // [16:11], read only
        uint32_t sha_illegal_len : 1;       // [17], read only
        uint32_t sha_rev_31_18 : 14;        // [31:18], read only
    } b;
} REG_AES_SHA_COMMAND_T;

// sha_message_len
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_message_len : 19; // [18:0]
        uint32_t __31_19 : 13;         // [31:19]
    } b;
} REG_AES_SHA_MESSAGE_LEN_T;

// sha_rd_counter
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_rd_counter : 16; // [15:0], read only
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_AES_SHA_RD_COUNTER_T;

// sha_wr_counter
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_wr_counter : 16; // [15:0], read only
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_AES_SHA_WR_COUNTER_T;

// sha_umac_state_c
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_umac_state_c : 2; // [1:0], read only
        uint32_t __31_2 : 30;          // [31:2]
    } b;
} REG_AES_SHA_UMAC_STATE_C_T;

// sha_state_c
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_state_c : 3; // [2:0], read only
        uint32_t __31_3 : 29;     // [31:3]
    } b;
} REG_AES_SHA_STATE_C_T;

// sha_int_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_int_clr : 1; // [0]
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_AES_SHA_INT_CLR_T;

// sha_int_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_int_en : 1; // [0]
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_AES_SHA_INT_EN_T;

// sha_int_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_int_status : 1; // [0], read only
        uint32_t __31_1 : 31;        // [31:1]
    } b;
} REG_AES_SHA_INT_STATUS_T;

// sha_restart
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_restart : 1; // [0]
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_AES_SHA_RESTART_T;

// sha_byte_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sha_byte_count : 29; // [28:0]
        uint32_t __31_29 : 3;         // [31:29]
    } b;
} REG_AES_SHA_BYTE_COUNT_T;

// dma_ctrl
#define AES_FIX_SRC (1 << 0)
#define AES_FIX_DST (1 << 1)
#define AES_HSIZEM(n) (((n)&0x7) << 2)
#define AES_CYCLE2_3 (1 << 5)
#define AES_CRC_CE_CTRL(n) (((n)&0x7) << 28)

// dma_src
#define AES_SRC_ADDR(n) (((n)&0xffffffff) << 0)

// dma_dst
#define AES_DST_ADDR(n) (((n)&0xffffffff) << 0)

// dma_len
#define AES_LENGTH(n) (((n)&0x3ffff) << 0)

// crc_gen
#define AES_CRC_POLY(n) (((n)&0xffffffff) << 0)

// dma_func_ctrl
#define AES_CRC_GEN_LEN(n) (((n)&0x3) << 0)
#define AES_CRC_CTRL(n) (((n)&0xf) << 16)
#define AES_AES_ENG_CGEN (1 << 24)
#define AES_AES_KEYGEN_CGEN (1 << 25)
#define AES_CRC_CGEN (1 << 28)
#define AES_TRNG_CGEN (1 << 30)

// aes_key0
#define AES_KEY0(n) (((n)&0xffffffff) << 0)

// aes_key1
#define AES_KEY1(n) (((n)&0xffffffff) << 0)

// aes_key2
#define AES_KEY2(n) (((n)&0xffffffff) << 0)

// aes_key3
#define AES_KEY3(n) (((n)&0xffffffff) << 0)

// aes_iv0
#define AES_IV0(n) (((n)&0xffffffff) << 0)

// aes_iv1
#define AES_IV1(n) (((n)&0xffffffff) << 0)

// aes_iv2
#define AES_IV2(n) (((n)&0xffffffff) << 0)

// aes_iv3
#define AES_IV3(n) (((n)&0xffffffff) << 0)

// aes_mode
#define AES_MODE (1 << 0)
#define AES_KEY_START (1 << 1)
#define AES_KEY_START_MODE (1 << 2)

// cios_ctrl
#define AES_CIOS_RAM_EN (1 << 0)
#define AES_CIOS_MODE(n) (((n)&0x7) << 1)
#define AES_CIOS_FORCE_RAM (1 << 4)
#define AES_CIOS_CLK_EN (1 << 5)
#define AES_CIOS_START_MODE (1 << 6)
#define AES_CIOS_START_COMPUTE (1 << 7)

// cios_reg0
#define AES_CIOS_REG0(n) (((n)&0xffffffff) << 0)

// crc_init_val
#define AES_CRC_INIT(n) (((n)&0xffffffff) << 0)

// crc_out_xorval
#define AES_CRC_XORVAL(n) (((n)&0xffffffff) << 0)

// crc_out_val
#define AES_CRC_OUT(n) (((n)&0xffffffff) << 0)

// crc_size_val
#define AES_CRC_SIZE(n) (((n)&0x7) << 0)

// ififo_thr
#define AES_IFIFO_RD_LOW_THR(n) (((n)&0x7f) << 0)
#define AES_IFIFO_RD_HIGHT_THR(n) (((n)&0x7f) << 8)

// ofifo_thr
#define AES_OFIFO_WR_LOW_THR(n) (((n)&0x7f) << 0)
#define AES_OFIFO_WR_HIGHT_THR(n) (((n)&0x7f) << 8)

// dma_int_out
#define AES_INT_OUT(n) (((n)&0x3f) << 0)

// dma_int_mask
#define AES_INT_MASK(n) (((n)&0x3f) << 0)

// trng_ctrl
#define AES_TRNG_CTRL(n) (((n)&0x7f) << 0)
#define AES_TRNG_MASK(n) (((n)&0xff) << 16)

// prng_ctrl
#define AES_PRNG_CTRL(n) (((n)&0xf) << 0)

// prng_seed
#define AES_PRNG_SEED(n) (((n)&0xffffffff) << 0)

// prng_timer_init
#define AES_PRNG_CLK_SEL (1 << 0)
#define AES_PRNG_INIT(n) (((n)&0x7fffffff) << 1)

// prng_timer
#define AES_PRNG_TIMER(n) (((n)&0xffffffff) << 0)

// trng_data0
#define AES_TRNG_DATA0(n) (((n)&0xffffffff) << 0)

// trng_data0_mask
#define AES_TRNG_DATA0_MASK(n) (((n)&0xffffffff) << 0)

// trng_data1
#define AES_TRNG_DATA1(n) (((n)&0xffffffff) << 0)

// trng_data1_mask
#define AES_TRNG_DATA1_MASK(n) (((n)&0xffffffff) << 0)

// prng_data
#define AES_PRNG_DATA(n) (((n)&0xffffffff) << 0)

// trng_hc
#define AES_TRNG_C_VALUE(n) (((n)&0xffff) << 0)
#define AES_TRNG_H_VALUE(n) (((n)&0xffff) << 16)

// sha_command
#define AES_SHA_START (1 << 0)
#define AES_SHA_COMPLETE (1 << 1)
#define AES_SHA_MESSAGE_END (1 << 2)
#define AES_SHA_MESSAGE_START (1 << 3)
#define AES_SHA_ALGORITHM(n) (((n)&0xf) << 4)
#define AES_SHA_INT_ON_COMPLETION (1 << 8)
#define AES_SHA_MSG_ENDIAN_MODE (1 << 9)
#define AES_SHA_HASH_ENDIAN_MODE (1 << 10)
#define AES_SHA_REV_16_11(n) (((n)&0x3f) << 11)
#define AES_SHA_ILLEGAL_LEN (1 << 17)
#define AES_SHA_REV_31_18(n) (((n)&0x3fff) << 18)

// sha_message_addr
#define AES_SHA_MESSAGE_ADDR(n) (((n)&0xffffffff) << 0)

// sha_message_len
#define AES_SHA_MESSAGE_LEN(n) (((n)&0x7ffff) << 0)

// sha_rd_counter
#define AES_SHA_RD_COUNTER(n) (((n)&0xffff) << 0)

// sha_wr_counter
#define AES_SHA_WR_COUNTER(n) (((n)&0xffff) << 0)

// sha_umac_state_c
#define AES_SHA_UMAC_STATE_C(n) (((n)&0x3) << 0)

// sha_state_c
#define AES_SHA_STATE_C(n) (((n)&0x7) << 0)

// sha_int_clr
#define AES_SHA_INT_CLR (1 << 0)

// sha_int_en
#define AES_SHA_INT_EN (1 << 0)

// sha_int_status
#define AES_SHA_INT_STATUS (1 << 0)

// sha_restart
#define AES_SHA_RESTART (1 << 0)

// hash_value_0
#define AES_HASH_VALUE_0(n) (((n)&0xffffffff) << 0)

// hash_value_1
#define AES_HASH_VALUE_1(n) (((n)&0xffffffff) << 0)

// hash_value_2
#define AES_HASH_VALUE_2(n) (((n)&0xffffffff) << 0)

// hash_value_3
#define AES_HASH_VALUE_3(n) (((n)&0xffffffff) << 0)

// hash_value_4
#define AES_HASH_VALUE_4(n) (((n)&0xffffffff) << 0)

// hash_value_5
#define AES_HASH_VALUE_5(n) (((n)&0xffffffff) << 0)

// hash_value_6
#define AES_HASH_VALUE_6(n) (((n)&0xffffffff) << 0)

// hash_value_7
#define AES_HASH_VALUE_7(n) (((n)&0xffffffff) << 0)

// sha_byte_count
#define AES_SHA_BYTE_COUNT(n) (((n)&0x1fffffff) << 0)

#endif // _AES_H_
