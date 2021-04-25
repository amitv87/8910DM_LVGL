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

#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_SPI_FLASH_BASE (0x00880000)
#define REG_SPI_FLASH1_BASE (0x00881000)

typedef volatile struct
{
    uint32_t spi_cmd_addr;             // 0x00000000
    uint32_t spi_block_size;           // 0x00000004
    uint32_t spi_data_fifo;            // 0x00000008
    uint32_t spi_status;               // 0x0000000c
    uint32_t rx_status;                // 0x00000010
    uint32_t spi_config;               // 0x00000014
    uint32_t spi_fifo_control;         // 0x00000018
    uint32_t spi_cs_size;              // 0x0000001c
    uint32_t spi_read_cmd;             // 0x00000020
    uint32_t spi_nand_config;          // 0x00000024
    uint32_t spi_nand_config2;         // 0x00000028
    uint32_t spi_256_512_flash_config; // 0x0000002c
    uint32_t spi_128_flash_config;     // 0x00000030
    uint32_t spi_cs4_sel;              // 0x00000034
    uint32_t page0_addr;               // 0x00000038
    uint32_t page1_addr;               // 0x0000003c
    uint32_t page2_addr;               // 0x00000040
    uint32_t page3_addr;               // 0x00000044
    uint32_t page4_addr;               // 0x00000048
    uint32_t page5_addr;               // 0x0000004c
    uint32_t page6_addr;               // 0x00000050
    uint32_t page7_addr;               // 0x00000054
    uint32_t page8_addr;               // 0x00000058
    uint32_t page9_addr;               // 0x0000005c
    uint32_t page10_addr;              // 0x00000060
    uint32_t page11_addr;              // 0x00000064
    uint32_t page12_addr;              // 0x00000068
    uint32_t page13_addr;              // 0x0000006c
    uint32_t page14_addr;              // 0x00000070
    uint32_t page15_addr;              // 0x00000074
    uint32_t spi_page_config;          // 0x00000078
    uint32_t spi_cmd_reconfig;         // 0x0000007c
    uint32_t page0_col_addr;           // 0x00000080
    uint32_t page1_col_addr;           // 0x00000084
    uint32_t page2_col_addr;           // 0x00000088
    uint32_t page3_col_addr;           // 0x0000008c
    uint32_t page4_col_addr;           // 0x00000090
    uint32_t page5_col_addr;           // 0x00000094
    uint32_t page6_col_addr;           // 0x00000098
    uint32_t page7_col_addr;           // 0x0000009c
    uint32_t page8_col_addr;           // 0x000000a0
    uint32_t page9_col_addr;           // 0x000000a4
    uint32_t page10_col_addr;          // 0x000000a8
    uint32_t page11_col_addr;          // 0x000000ac
    uint32_t page12_col_addr;          // 0x000000b0
    uint32_t page13_col_addr;          // 0x000000b4
    uint32_t page14_col_addr;          // 0x000000b8
    uint32_t page15_col_addr;          // 0x000000bc
    uint32_t nand_int_mask;            // 0x000000c0
} HWP_SPI_FLASH_T;

#define hwp_spiFlash ((HWP_SPI_FLASH_T *)REG_ACCESS_ADDRESS(REG_SPI_FLASH_BASE))
#define hwp_spiFlash1 ((HWP_SPI_FLASH_T *)REG_ACCESS_ADDRESS(REG_SPI_FLASH1_BASE))

// spi_cmd_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spi_tx_cmd : 8;   // [7:0]
        uint32_t spi_address : 24; // [31:8]
    } b;
} REG_SPI_FLASH_SPI_CMD_ADDR_T;

// spi_block_size
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spi_modebit : 8;       // [7:0]
        uint32_t spi_rw_blk_size : 14;  // [21:8]
        uint32_t __23_22 : 2;           // [23:22]
        uint32_t continuous_enable : 1; // [24]
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_SPI_FLASH_SPI_BLOCK_SIZE_T;

// spi_data_fifo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spi_tx_data : 8;        // [7:0]
        uint32_t spi_read_back_reg : 32; // [31:0], read only
        uint32_t spi_send_type : 1;      // [8]
        uint32_t __31_9 : 23;            // [31:9]
    } b;
} REG_SPI_FLASH_SPI_DATA_FIFO_T;

// spi_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spi_flash_busy : 1; // [0], read only
        uint32_t tx_fifo_empty : 1;  // [1], read only
        uint32_t tx_fifo_full : 1;   // [2], read only
        uint32_t rx_fifo_empty : 1;  // [3], read only
        uint32_t rx_fifo_count : 5;  // [8:4], read only
        uint32_t read_stat_busy : 1; // [9], read only
        uint32_t nand_int : 1;       // [10], read only
        uint32_t spiflash_int : 1;   // [11], read only
        uint32_t __31_12 : 20;       // [31:12]
    } b;
} REG_SPI_FLASH_SPI_STATUS_T;

// spi_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t quad_mode : 1;        // [0]
        uint32_t spi_wprotect_pin : 1; // [1]
        uint32_t spi_hold_pin : 1;     // [2]
        uint32_t __3_3 : 1;            // [3]
        uint32_t sample_delay : 3;     // [6:4]
        uint32_t __7_7 : 1;            // [7]
        uint32_t clk_divider : 8;      // [15:8]
        uint32_t cmd_quad : 1;         // [16]
        uint32_t tx_rx_size : 2;       // [18:17]
        uint32_t __31_19 : 13;         // [31:19]
    } b;
} REG_SPI_FLASH_SPI_CONFIG_T;

// spi_fifo_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_fifo_clr : 1; // [0]
        uint32_t tx_fifo_clr : 1; // [1]
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_SPI_FLASH_SPI_FIFO_CONTROL_T;

// spi_cs_size
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spi_cs_num : 1;            // [0]
        uint32_t spi_size : 2;              // [2:1]
        uint32_t spi_128m : 1;              // [3]
        uint32_t ahb_read_disable : 1;      // [4]
        uint32_t sel_flash_1 : 1;           // [5]
        uint32_t sel1_flash_1 : 1;          // [6]
        uint32_t diff_128m_diff_cmd_en : 1; // [7]
        uint32_t spi_256m : 1;              // [8]
        uint32_t spi_512m : 1;              // [9]
        uint32_t spi_cs1_sel2 : 1;          // [10]
        uint32_t spi_1g : 1;                // [11]
        uint32_t spi_2g : 1;                // [12]
        uint32_t spi_4g : 1;                // [13]
        uint32_t spi_cs1_sel3 : 1;          // [14]
        uint32_t spi_cs1_sel4 : 1;          // [15]
        uint32_t spi_cs1_sel5 : 1;          // [16]
        uint32_t __31_17 : 15;              // [31:17]
    } b;
} REG_SPI_FLASH_SPI_CS_SIZE_T;

// spi_read_cmd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qread_cmd : 8;    // [7:0]
        uint32_t fread_cmd : 8;    // [15:8]
        uint32_t read_cmd : 8;     // [23:16]
        uint32_t protect_byte : 8; // [31:24]
    } b;
} REG_SPI_FLASH_SPI_READ_CMD_T;

// spi_nand_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nand_sel : 1;        // [0]
        uint32_t nand_addr : 2;       // [2:1]
        uint32_t reuse_nand_ram : 1;  // [3]
        uint32_t reuse_read : 1;      // [4]
        uint32_t write_page_hit : 1;  // [5]
        uint32_t nand_data_trans : 1; // [6]
        uint32_t page_size_sel : 1;   // [7]
        uint32_t page_read_cmd : 8;   // [15:8]
        uint32_t get_sts_cmd : 8;     // [23:16]
        uint32_t ram_read_cmd : 8;    // [31:24]
    } b;
} REG_SPI_FLASH_SPI_NAND_CONFIG_T;

// spi_nand_config2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t get_sts_addr : 8; // [7:0]
        uint32_t __15_8 : 8;       // [15:8]
        uint32_t sts_qip : 8;      // [23:16]
        uint32_t __31_24 : 8;      // [31:24]
    } b;
} REG_SPI_FLASH_SPI_NAND_CONFIG2_T;

// spi_256_512_flash_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t four_byte_addr : 1; // [0]
        uint32_t dummy_cycle_en : 1; // [1]
        uint32_t __7_2 : 6;          // [7:2]
        uint32_t dummy_cycle : 4;    // [11:8]
        uint32_t wrap_en : 1;        // [12]
        uint32_t __15_13 : 3;        // [15:13]
        uint32_t wrap_code : 4;      // [19:16]
        uint32_t __31_20 : 12;       // [31:20]
    } b;
} REG_SPI_FLASH_SPI_256_512_FLASH_CONFIG_T;

// spi_128_flash_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t first_128m_cmd : 8;  // [7:0]
        uint32_t second_128m_cmd : 8; // [15:8]
        uint32_t third_128m_cmd : 8;  // [23:16]
        uint32_t fourth_128m_cmd : 8; // [31:24]
    } b;
} REG_SPI_FLASH_SPI_128_FLASH_CONFIG_T;

// spi_cs4_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spi_cs4_sel : 3; // [2:0]
        uint32_t __31_3 : 29;     // [31:3]
    } b;
} REG_SPI_FLASH_SPI_CS4_SEL_T;

// page0_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page0_addr : 24; // [23:0]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t page0_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE0_ADDR_T;

// page1_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page1_addr : 24; // [23:0]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t page1_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE1_ADDR_T;

// page2_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page2_addr : 24; // [23:0]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t page2_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE2_ADDR_T;

// page3_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page3_addr : 24; // [23:0]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t page3_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE3_ADDR_T;

// page4_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page4_addr : 24; // [23:0]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t page4_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE4_ADDR_T;

// page5_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page5_addr : 24; // [23:0]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t page5_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE5_ADDR_T;

// page6_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page6_addr : 24; // [23:0]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t page6_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE6_ADDR_T;

// page7_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page7_addr : 24; // [23:0]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t page7_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE7_ADDR_T;

// page8_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page8_addr : 24; // [23:0]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t page8_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE8_ADDR_T;

// page9_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page9_addr : 24; // [23:0]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t page9_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE9_ADDR_T;

// page10_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page10_addr : 24; // [23:0]
        uint32_t __30_24 : 7;      // [30:24]
        uint32_t page10_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE10_ADDR_T;

// page11_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page11_addr : 24; // [23:0]
        uint32_t __30_24 : 7;      // [30:24]
        uint32_t page11_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE11_ADDR_T;

// page12_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page12_addr : 24; // [23:0]
        uint32_t __30_24 : 7;      // [30:24]
        uint32_t page12_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE12_ADDR_T;

// page13_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page13_addr : 24; // [23:0]
        uint32_t __30_24 : 7;      // [30:24]
        uint32_t page13_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE13_ADDR_T;

// page14_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page14_addr : 24; // [23:0]
        uint32_t __30_24 : 7;      // [30:24]
        uint32_t page14_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE14_ADDR_T;

// page15_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page15_addr : 24; // [23:0]
        uint32_t __30_24 : 7;      // [30:24]
        uint32_t page15_valid : 1; // [31]
    } b;
} REG_SPI_FLASH_PAGE15_ADDR_T;

// spi_page_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t multi_page_enable_multi_page_start : 1; // [0]
        uint32_t __7_1 : 7;                              // [7:1]
        uint32_t page_num : 5;                           // [12:8]
        uint32_t __31_13 : 19;                           // [31:13]
    } b;
} REG_SPI_FLASH_SPI_PAGE_CONFIG_T;

// spi_cmd_reconfig
typedef union {
    uint32_t v;
    struct
    {
        uint32_t program_exe_cmd : 8;  // [7:0]
        uint32_t program_load_cmd : 8; // [15:8]
        uint32_t write_enable_cmd : 8; // [23:16]
        uint32_t __31_24 : 8;          // [31:24]
    } b;
} REG_SPI_FLASH_SPI_CMD_RECONFIG_T;

// page0_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page0_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SPI_FLASH_PAGE0_COL_ADDR_T;

// page1_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page1_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SPI_FLASH_PAGE1_COL_ADDR_T;

// page2_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page2_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SPI_FLASH_PAGE2_COL_ADDR_T;

// page3_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page3_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SPI_FLASH_PAGE3_COL_ADDR_T;

// page4_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page4_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SPI_FLASH_PAGE4_COL_ADDR_T;

// page5_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page5_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SPI_FLASH_PAGE5_COL_ADDR_T;

// page6_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page6_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SPI_FLASH_PAGE6_COL_ADDR_T;

// page7_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page7_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SPI_FLASH_PAGE7_COL_ADDR_T;

// page8_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page8_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SPI_FLASH_PAGE8_COL_ADDR_T;

// page9_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page9_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SPI_FLASH_PAGE9_COL_ADDR_T;

// page10_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page10_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_SPI_FLASH_PAGE10_COL_ADDR_T;

// page11_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page11_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_SPI_FLASH_PAGE11_COL_ADDR_T;

// page12_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page12_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_SPI_FLASH_PAGE12_COL_ADDR_T;

// page13_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page13_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_SPI_FLASH_PAGE13_COL_ADDR_T;

// page14_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page14_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_SPI_FLASH_PAGE14_COL_ADDR_T;

// page15_col_addr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t page15_col_addr : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_SPI_FLASH_PAGE15_COL_ADDR_T;

// nand_int_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nand_int_mask : 1; // [0]
        uint32_t __31_1 : 31;       // [31:1]
    } b;
} REG_SPI_FLASH_NAND_INT_MASK_T;

// spi_cmd_addr
#define SPI_FLASH_SPI_TX_CMD(n) (((n)&0xff) << 0)
#define SPI_FLASH_SPI_ADDRESS(n) (((n)&0xffffff) << 8)

// spi_block_size
#define SPI_FLASH_SPI_MODEBIT(n) (((n)&0xff) << 0)
#define SPI_FLASH_SPI_RW_BLK_SIZE(n) (((n)&0x3fff) << 8)
#define SPI_FLASH_CONTINUOUS_ENABLE (1 << 24)

// spi_data_fifo
#define SPI_FLASH_SPI_TX_DATA(n) (((n)&0xff) << 0)
#define SPI_FLASH_SPI_READ_BACK_REG(n) (((n)&0xffffffff) << 0)
#define SPI_FLASH_SPI_SEND_TYPE (1 << 8)

// spi_status
#define SPI_FLASH_SPI_FLASH_BUSY (1 << 0)
#define SPI_FLASH_TX_FIFO_EMPTY (1 << 1)
#define SPI_FLASH_TX_FIFO_FULL (1 << 2)
#define SPI_FLASH_RX_FIFO_EMPTY (1 << 3)
#define SPI_FLASH_RX_FIFO_COUNT(n) (((n)&0x1f) << 4)
#define SPI_FLASH_READ_STAT_BUSY (1 << 9)
#define SPI_FLASH_NAND_INT (1 << 10)
#define SPI_FLASH_SPIFLASH_INT (1 << 11)

// rx_status
#define SPI_FLASH_RX_STATUS(n) (((n)&0xffffffff) << 0)

// spi_config
#define SPI_FLASH_QUAD_MODE_SPI_READ (0 << 0)
#define SPI_FLASH_QUAD_MODE_QUAD_READ (1 << 0)
#define SPI_FLASH_SPI_WPROTECT_PIN (1 << 1)
#define SPI_FLASH_SPI_HOLD_PIN (1 << 2)
#define SPI_FLASH_SAMPLE_DELAY(n) (((n)&0x7) << 4)
#define SPI_FLASH_CLK_DIVIDER(n) (((n)&0xff) << 8)
#define SPI_FLASH_CMD_QUAD (1 << 16)
#define SPI_FLASH_TX_RX_SIZE(n) (((n)&0x3) << 17)

#define SPI_FLASH_QUAD_MODE_V_SPI_READ (0)
#define SPI_FLASH_QUAD_MODE_V_QUAD_READ (1)

// spi_fifo_control
#define SPI_FLASH_RX_FIFO_CLR (1 << 0)
#define SPI_FLASH_TX_FIFO_CLR (1 << 1)

// spi_cs_size
#define SPI_FLASH_SPI_CS_NUM_1_SPIFLASH (0 << 0)
#define SPI_FLASH_SPI_CS_NUM_2_SPIFLASH (1 << 0)
#define SPI_FLASH_SPI_SIZE(n) (((n)&0x3) << 1)
#define SPI_FLASH_SPI_SIZE_32M (0 << 1)
#define SPI_FLASH_SPI_SIZE_64M (1 << 1)
#define SPI_FLASH_SPI_SIZE_16M (2 << 1)
#define SPI_FLASH_SPI_SIZE_8M (3 << 1)
#define SPI_FLASH_SPI_128M_OTHER_SPIFLASH (0 << 3)
#define SPI_FLASH_SPI_128M_128M_SPIFLASH (1 << 3)
#define SPI_FLASH_AHB_READ_DISABLE_ENABLE_AHB_READ (0 << 4)
#define SPI_FLASH_AHB_READ_DISABLE_DISABLE_AHB_READ (1 << 4)
#define SPI_FLASH_SEL_FLASH_1_SEL_FLASH_0 (0 << 5)
#define SPI_FLASH_SEL_FLASH_1_SEL_FLASH_1 (1 << 5)
#define SPI_FLASH_SEL1_FLASH_1 (1 << 6)
#define SPI_FLASH_DIFF_128M_DIFF_CMD_EN (1 << 7)
#define SPI_FLASH_SPI_256M (1 << 8)
#define SPI_FLASH_SPI_512M (1 << 9)
#define SPI_FLASH_SPI_CS1_SEL2 (1 << 10)
#define SPI_FLASH_SPI_1G (1 << 11)
#define SPI_FLASH_SPI_2G (1 << 12)
#define SPI_FLASH_SPI_4G (1 << 13)
#define SPI_FLASH_SPI_CS1_SEL3 (1 << 14)
#define SPI_FLASH_SPI_CS1_SEL4 (1 << 15)
#define SPI_FLASH_SPI_CS1_SEL5 (1 << 16)

#define SPI_FLASH_SPI_CS_NUM_V_1_SPIFLASH (0)
#define SPI_FLASH_SPI_CS_NUM_V_2_SPIFLASH (1)
#define SPI_FLASH_SPI_SIZE_V_32M (0)
#define SPI_FLASH_SPI_SIZE_V_64M (1)
#define SPI_FLASH_SPI_SIZE_V_16M (2)
#define SPI_FLASH_SPI_SIZE_V_8M (3)
#define SPI_FLASH_SPI_128M_V_OTHER_SPIFLASH (0)
#define SPI_FLASH_SPI_128M_V_128M_SPIFLASH (1)
#define SPI_FLASH_AHB_READ_DISABLE_V_ENABLE_AHB_READ (0)
#define SPI_FLASH_AHB_READ_DISABLE_V_DISABLE_AHB_READ (1)
#define SPI_FLASH_SEL_FLASH_1_V_SEL_FLASH_0 (0)
#define SPI_FLASH_SEL_FLASH_1_V_SEL_FLASH_1 (1)

// spi_read_cmd
#define SPI_FLASH_QREAD_CMD(n) (((n)&0xff) << 0)
#define SPI_FLASH_FREAD_CMD(n) (((n)&0xff) << 8)
#define SPI_FLASH_READ_CMD(n) (((n)&0xff) << 16)
#define SPI_FLASH_PROTECT_BYTE(n) (((n)&0xff) << 24)

// spi_nand_config
#define SPI_FLASH_NAND_SEL (1 << 0)
#define SPI_FLASH_NAND_ADDR(n) (((n)&0x3) << 1)
#define SPI_FLASH_REUSE_NAND_RAM (1 << 3)
#define SPI_FLASH_REUSE_READ (1 << 4)
#define SPI_FLASH_WRITE_PAGE_HIT (1 << 5)
#define SPI_FLASH_NAND_DATA_TRANS (1 << 6)
#define SPI_FLASH_PAGE_SIZE_SEL (1 << 7)
#define SPI_FLASH_PAGE_READ_CMD(n) (((n)&0xff) << 8)
#define SPI_FLASH_GET_STS_CMD(n) (((n)&0xff) << 16)
#define SPI_FLASH_RAM_READ_CMD(n) (((n)&0xff) << 24)

// spi_nand_config2
#define SPI_FLASH_GET_STS_ADDR(n) (((n)&0xff) << 0)
#define SPI_FLASH_STS_QIP(n) (((n)&0xff) << 16)

// spi_256_512_flash_config
#define SPI_FLASH_FOUR_BYTE_ADDR (1 << 0)
#define SPI_FLASH_DUMMY_CYCLE_EN (1 << 1)
#define SPI_FLASH_DUMMY_CYCLE(n) (((n)&0xf) << 8)
#define SPI_FLASH_WRAP_EN (1 << 12)
#define SPI_FLASH_WRAP_CODE(n) (((n)&0xf) << 16)

// spi_128_flash_config
#define SPI_FLASH_FIRST_128M_CMD(n) (((n)&0xff) << 0)
#define SPI_FLASH_SECOND_128M_CMD(n) (((n)&0xff) << 8)
#define SPI_FLASH_THIRD_128M_CMD(n) (((n)&0xff) << 16)
#define SPI_FLASH_FOURTH_128M_CMD(n) (((n)&0xff) << 24)

// spi_cs4_sel
#define SPI_FLASH_SPI_CS4_SEL(n) (((n)&0x7) << 0)

// page0_addr
#define SPI_FLASH_PAGE0_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE0_VALID (1 << 31)

// page1_addr
#define SPI_FLASH_PAGE1_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE1_VALID (1 << 31)

// page2_addr
#define SPI_FLASH_PAGE2_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE2_VALID (1 << 31)

// page3_addr
#define SPI_FLASH_PAGE3_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE3_VALID (1 << 31)

// page4_addr
#define SPI_FLASH_PAGE4_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE4_VALID (1 << 31)

// page5_addr
#define SPI_FLASH_PAGE5_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE5_VALID (1 << 31)

// page6_addr
#define SPI_FLASH_PAGE6_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE6_VALID (1 << 31)

// page7_addr
#define SPI_FLASH_PAGE7_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE7_VALID (1 << 31)

// page8_addr
#define SPI_FLASH_PAGE8_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE8_VALID (1 << 31)

// page9_addr
#define SPI_FLASH_PAGE9_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE9_VALID (1 << 31)

// page10_addr
#define SPI_FLASH_PAGE10_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE10_VALID (1 << 31)

// page11_addr
#define SPI_FLASH_PAGE11_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE11_VALID (1 << 31)

// page12_addr
#define SPI_FLASH_PAGE12_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE12_VALID (1 << 31)

// page13_addr
#define SPI_FLASH_PAGE13_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE13_VALID (1 << 31)

// page14_addr
#define SPI_FLASH_PAGE14_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE14_VALID (1 << 31)

// page15_addr
#define SPI_FLASH_PAGE15_ADDR(n) (((n)&0xffffff) << 0)
#define SPI_FLASH_PAGE15_VALID (1 << 31)

// spi_page_config
#define SPI_FLASH_MULTI_PAGE_ENABLE_MULTI_PAGE_START (1 << 0)
#define SPI_FLASH_PAGE_NUM(n) (((n)&0x1f) << 8)

// spi_cmd_reconfig
#define SPI_FLASH_PROGRAM_EXE_CMD(n) (((n)&0xff) << 0)
#define SPI_FLASH_PROGRAM_LOAD_CMD(n) (((n)&0xff) << 8)
#define SPI_FLASH_WRITE_ENABLE_CMD(n) (((n)&0xff) << 16)

// page0_col_addr
#define SPI_FLASH_PAGE0_COL_ADDR(n) (((n)&0xffff) << 0)

// page1_col_addr
#define SPI_FLASH_PAGE1_COL_ADDR(n) (((n)&0xffff) << 0)

// page2_col_addr
#define SPI_FLASH_PAGE2_COL_ADDR(n) (((n)&0xffff) << 0)

// page3_col_addr
#define SPI_FLASH_PAGE3_COL_ADDR(n) (((n)&0xffff) << 0)

// page4_col_addr
#define SPI_FLASH_PAGE4_COL_ADDR(n) (((n)&0xffff) << 0)

// page5_col_addr
#define SPI_FLASH_PAGE5_COL_ADDR(n) (((n)&0xffff) << 0)

// page6_col_addr
#define SPI_FLASH_PAGE6_COL_ADDR(n) (((n)&0xffff) << 0)

// page7_col_addr
#define SPI_FLASH_PAGE7_COL_ADDR(n) (((n)&0xffff) << 0)

// page8_col_addr
#define SPI_FLASH_PAGE8_COL_ADDR(n) (((n)&0xffff) << 0)

// page9_col_addr
#define SPI_FLASH_PAGE9_COL_ADDR(n) (((n)&0xffff) << 0)

// page10_col_addr
#define SPI_FLASH_PAGE10_COL_ADDR(n) (((n)&0xffff) << 0)

// page11_col_addr
#define SPI_FLASH_PAGE11_COL_ADDR(n) (((n)&0xffff) << 0)

// page12_col_addr
#define SPI_FLASH_PAGE12_COL_ADDR(n) (((n)&0xffff) << 0)

// page13_col_addr
#define SPI_FLASH_PAGE13_COL_ADDR(n) (((n)&0xffff) << 0)

// page14_col_addr
#define SPI_FLASH_PAGE14_COL_ADDR(n) (((n)&0xffff) << 0)

// page15_col_addr
#define SPI_FLASH_PAGE15_COL_ADDR(n) (((n)&0xffff) << 0)

// nand_int_mask
#define SPI_FLASH_NAND_INT_MASK (1 << 0)

#endif // _SPI_FLASH_H_
