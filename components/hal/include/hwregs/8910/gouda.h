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

#ifndef _GOUDA_H_
#define _GOUDA_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define GD_MAX_OUT_WIDTH (640)
#define GD_NB_BITS_LCDPOS (11)
#define GD_FP_FRAC_SIZE (8)
#define GD_FIXEDPOINT_SIZE (11)
#define GD_NB_BITS_STRIDE (13)
#define GD_MAX_SLCD_READ_LEN (4)
#define GD_MAX_SLCD_CLK_DIVIDER (255)
#define GD_NB_WORKBUF_WORDS (0x16e0)
#define GD_NB_LCD_CMD_WORDS (64)
#define GD_SRAM_SIZE (0x2e40)
#define GD_SRAM_ADDR_WIDTH (13)

#define REG_GOUDA_BASE (0x08806000)

typedef volatile struct
{
    uint32_t gd_command;           // 0x00000000
    uint32_t gd_status;            // 0x00000004
    uint32_t gd_eof_irq;           // 0x00000008
    uint32_t gd_eof_irq_mask;      // 0x0000000c
    uint32_t gd_roi_tl_ppos;       // 0x00000010
    uint32_t gd_roi_br_ppos;       // 0x00000014
    uint32_t gd_roi_bg_color;      // 0x00000018
    uint32_t gd_vl_input_fmt;      // 0x0000001c
    uint32_t gd_vl_tl_ppos;        // 0x00000020
    uint32_t gd_vl_br_ppos;        // 0x00000024
    uint32_t gd_vl_extents;        // 0x00000028
    uint32_t gd_vl_blend_opt;      // 0x0000002c
    uint32_t gd_vl_y_src;          // 0x00000030
    uint32_t gd_vl_u_src;          // 0x00000034
    uint32_t gd_vl_v_src;          // 0x00000038
    uint32_t gd_vl_resc_ratio;     // 0x0000003c
    struct                         // 0x00000040
    {                              //
        uint32_t gd_ol_input_fmt;  // 0x00000000
        uint32_t gd_ol_tl_ppos;    // 0x00000004
        uint32_t gd_ol_br_ppos;    // 0x00000008
        uint32_t gd_ol_blend_opt;  // 0x0000000c
        uint32_t gd_ol_rgb_src;    // 0x00000010
    } overlay_layer[3];            //
    uint32_t gd_lcd_ctrl;          // 0x0000007c
    uint32_t gd_lcd_timing;        // 0x00000080
    uint32_t gd_lcd_mem_address;   // 0x00000084
    uint32_t gd_lcd_stride_offset; // 0x00000088
    uint32_t gd_lcd_single_access; // 0x0000008c
    uint32_t gd_spilcd_config;     // 0x00000090
    uint32_t gd_spilcd_rd;         // 0x00000094
    uint32_t gd_vl_fix_ratio;      // 0x00000098
    uint32_t __156[41];            // 0x0000009c
    uint32_t tecon;                // 0x00000140
    uint32_t tecon2;               // 0x00000144
    uint32_t __328[174];           // 0x00000148
} HWP_GOUDA_T;

#define hwp_gouda ((HWP_GOUDA_T *)REG_ACCESS_ADDRESS(REG_GOUDA_BASE))

#define REG_GOUDA_SRAM_BASE (0x09080000)

typedef volatile struct
{
    uint8_t sram_array[11840]; // 0x00000000
} HWP_GOUDA_SRAM_T;

#define hwp_goudaSram ((HWP_GOUDA_SRAM_T *)REG_ACCESS_ADDRESS(REG_GOUDA_SRAM_BASE))

// gd_command
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;   // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_GOUDA_GD_COMMAND_T;

// gd_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ia_busy : 1;  // [0], read only
        uint32_t __3_1 : 3;    // [3:1]
        uint32_t lcd_busy : 1; // [4], read only
        uint32_t __31_5 : 27;  // [31:5]
    } b;
} REG_GOUDA_GD_STATUS_T;

// gd_eof_irq
typedef union {
    uint32_t v;
    struct
    {
        uint32_t eof_cause : 1;  // [0], write clear
        uint32_t __15_1 : 15;    // [15:1]
        uint32_t eof_status : 1; // [16], write clear
        uint32_t __31_17 : 15;   // [31:17]
    } b;
} REG_GOUDA_GD_EOF_IRQ_T;

// gd_eof_irq_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t eof_mask : 1; // [0]
        uint32_t __31_1 : 31;  // [31:1]
    } b;
} REG_GOUDA_GD_EOF_IRQ_MASK_T;

// gd_roi_tl_ppos
typedef union {
    uint32_t v;
    struct
    {
        uint32_t x0 : 11;     // [10:0]
        uint32_t __15_11 : 5; // [15:11]
        uint32_t y0 : 11;     // [26:16]
        uint32_t __31_27 : 5; // [31:27]
    } b;
} REG_GOUDA_GD_ROI_TL_PPOS_T;

// gd_roi_br_ppos
typedef union {
    uint32_t v;
    struct
    {
        uint32_t x1 : 11;     // [10:0]
        uint32_t __15_11 : 5; // [15:11]
        uint32_t y1 : 11;     // [26:16]
        uint32_t __31_27 : 5; // [31:27]
    } b;
} REG_GOUDA_GD_ROI_BR_PPOS_T;

// gd_roi_bg_color
typedef union {
    uint32_t v;
    struct
    {
        uint32_t b : 5;        // [4:0]
        uint32_t g : 6;        // [10:5]
        uint32_t r : 5;        // [15:11]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_GOUDA_GD_ROI_BG_COLOR_T;

// gd_vl_input_fmt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t format : 2;   // [1:0]
        uint32_t stride : 13;  // [14:2]
        uint32_t __30_15 : 16; // [30:15]
        uint32_t active : 1;   // [31]
    } b;
} REG_GOUDA_GD_VL_INPUT_FMT_T;

// gd_vl_tl_ppos
typedef union {
    uint32_t v;
    struct
    {
        uint32_t x0 : 11;     // [10:0]
        uint32_t __15_11 : 5; // [15:11]
        uint32_t y0 : 11;     // [26:16]
        uint32_t __31_27 : 5; // [31:27]
    } b;
} REG_GOUDA_GD_VL_TL_PPOS_T;

// gd_vl_br_ppos
typedef union {
    uint32_t v;
    struct
    {
        uint32_t x1 : 11;     // [10:0]
        uint32_t __15_11 : 5; // [15:11]
        uint32_t y1 : 11;     // [26:16]
        uint32_t __31_27 : 5; // [31:27]
    } b;
} REG_GOUDA_GD_VL_BR_PPOS_T;

// gd_vl_extents
typedef union {
    uint32_t v;
    struct
    {
        uint32_t max_line : 11; // [10:0]
        uint32_t __15_11 : 5;   // [15:11]
        uint32_t max_col : 11;  // [26:16]
        uint32_t __31_27 : 5;   // [31:27]
    } b;
} REG_GOUDA_GD_VL_EXTENTS_T;

// gd_vl_blend_opt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chroma_key_b : 5;      // [4:0]
        uint32_t chroma_key_g : 6;      // [10:5]
        uint32_t chroma_key_r : 5;      // [15:11]
        uint32_t chroma_key_enable : 1; // [16]
        uint32_t chroma_key_mask : 3;   // [19:17]
        uint32_t alpha : 8;             // [27:20]
        uint32_t rotation : 2;          // [29:28]
        uint32_t depth : 2;             // [31:30]
    } b;
} REG_GOUDA_GD_VL_BLEND_OPT_T;

// gd_vl_y_src
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2; // [1:0]
        uint32_t addr : 30; // [31:2]
    } b;
} REG_GOUDA_GD_VL_Y_SRC_T;

// gd_vl_u_src
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2; // [1:0]
        uint32_t addr : 30; // [31:2]
    } b;
} REG_GOUDA_GD_VL_U_SRC_T;

// gd_vl_v_src
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2; // [1:0]
        uint32_t addr : 30; // [31:2]
    } b;
} REG_GOUDA_GD_VL_V_SRC_T;

// gd_vl_resc_ratio
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xpitch : 11;             // [10:0]
        uint32_t __15_11 : 5;             // [15:11]
        uint32_t ypitch : 11;             // [26:16]
        uint32_t __28_27 : 2;             // [28:27]
        uint32_t pre_fetch_en : 1;        // [29]
        uint32_t iy_dctenable : 1;        // [30]
        uint32_t ypitch_scale_enable : 1; // [31]
    } b;
} REG_GOUDA_GD_VL_RESC_RATIO_T;

// gd_ol_input_fmt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t format : 2;   // [1:0]
        uint32_t stride : 13;  // [14:2]
        uint32_t __17_15 : 3;  // [17:15]
        uint32_t prefetch : 1; // [18]
        uint32_t __30_19 : 12; // [30:19]
        uint32_t active : 1;   // [31]
    } b;
} REG_GOUDA_GD_OL_INPUT_FMT_T;

// gd_ol_tl_ppos
typedef union {
    uint32_t v;
    struct
    {
        uint32_t x0 : 11;     // [10:0]
        uint32_t __15_11 : 5; // [15:11]
        uint32_t y0 : 11;     // [26:16]
        uint32_t __31_27 : 5; // [31:27]
    } b;
} REG_GOUDA_GD_OL_TL_PPOS_T;

// gd_ol_br_ppos
typedef union {
    uint32_t v;
    struct
    {
        uint32_t x1 : 11;     // [10:0]
        uint32_t __15_11 : 5; // [15:11]
        uint32_t y1 : 11;     // [26:16]
        uint32_t __31_27 : 5; // [31:27]
    } b;
} REG_GOUDA_GD_OL_BR_PPOS_T;

// gd_ol_blend_opt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chroma_key_b : 5;      // [4:0]
        uint32_t chroma_key_g : 6;      // [10:5]
        uint32_t chroma_key_r : 5;      // [15:11]
        uint32_t chroma_key_enable : 1; // [16]
        uint32_t chroma_key_mask : 3;   // [19:17]
        uint32_t alpha : 8;             // [27:20]
        uint32_t __31_28 : 4;           // [31:28]
    } b;
} REG_GOUDA_GD_OL_BLEND_OPT_T;

// gd_ol_rgb_src
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2; // [1:0]
        uint32_t addr : 30; // [31:2]
    } b;
} REG_GOUDA_GD_OL_RGB_SRC_T;

// gd_lcd_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t destination : 2;   // [1:0]
        uint32_t __3_2 : 2;         // [3:2]
        uint32_t output_format : 3; // [6:4]
        uint32_t high_byte : 1;     // [7]
        uint32_t cs0_polarity : 1;  // [8]
        uint32_t cs1_polarity : 1;  // [9]
        uint32_t rs_polarity : 1;   // [10]
        uint32_t wr_polarity : 1;   // [11]
        uint32_t rd_polarity : 1;   // [12]
        uint32_t __15_13 : 3;       // [15:13]
        uint32_t nb_command : 6;    // [21:16]
        uint32_t __23_22 : 2;       // [23:22]
        uint32_t start_command : 1; // [24]
        uint32_t lcd_resetb : 1;    // [25]
        uint32_t __31_26 : 6;       // [31:26]
    } b;
} REG_GOUDA_GD_LCD_CTRL_T;

// gd_lcd_timing
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tas : 3;      // [2:0]
        uint32_t __3_3 : 1;    // [3]
        uint32_t tah : 3;      // [6:4]
        uint32_t __7_7 : 1;    // [7]
        uint32_t pwl : 6;      // [13:8]
        uint32_t __15_14 : 2;  // [15:14]
        uint32_t pwh : 6;      // [21:16]
        uint32_t __31_22 : 10; // [31:22]
    } b;
} REG_GOUDA_GD_LCD_TIMING_T;

// gd_lcd_mem_address
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;     // [1:0]
        uint32_t addr_dst : 30; // [31:2]
    } b;
} REG_GOUDA_GD_LCD_MEM_ADDRESS_T;

// gd_lcd_stride_offset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t stride_offset : 10; // [9:0]
        uint32_t __31_10 : 22;       // [31:10]
    } b;
} REG_GOUDA_GD_LCD_STRIDE_OFFSET_T;

// gd_lcd_single_access
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lcd_data : 16;   // [15:0]
        uint32_t type : 1;        // [16]
        uint32_t start_write : 1; // [17]
        uint32_t start_read : 1;  // [18]
        uint32_t __31_19 : 13;    // [31:19]
    } b;
} REG_GOUDA_GD_LCD_SINGLE_ACCESS_T;

// gd_spilcd_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spi_lcd_select : 1;  // [0]
        uint32_t spi_device_id : 6;   // [6:1]
        uint32_t spi_clk_divider : 8; // [14:7]
        uint32_t spi_dummy_cycle : 3; // [17:15]
        uint32_t spi_line : 2;        // [19:18]
        uint32_t spi_rx_byte : 3;     // [22:20]
        uint32_t spi_rw : 1;          // [23]
        uint32_t __31_24 : 8;         // [31:24]
    } b;
} REG_GOUDA_GD_SPILCD_CONFIG_T;

// gd_vl_fix_ratio
typedef union {
    uint32_t v;
    struct
    {
        uint32_t l_xratio : 8;        // [7:0]
        uint32_t l_yratio : 8;        // [15:8]
        uint32_t l_xfixen : 1;        // [16]
        uint32_t l_yfixen : 1;        // [17]
        uint32_t mirror : 1;          // [18]
        uint32_t reg_vl_only_sel : 1; // [19]
        uint32_t __31_20 : 12;        // [31:20]
    } b;
} REG_GOUDA_GD_VL_FIX_RATIO_T;

// tecon
typedef union {
    uint32_t v;
    struct
    {
        uint32_t te_en : 1;       // [0]
        uint32_t te_edge_sel : 1; // [1]
        uint32_t te_mode : 1;     // [2]
        uint32_t __15_3 : 13;     // [15:3]
        uint32_t te_count2 : 12;  // [27:16]
        uint32_t __31_28 : 4;     // [31:28]
    } b;
} REG_GOUDA_TECON_T;

// tecon2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t te_count1 : 29; // [28:0]
        uint32_t __31_29 : 3;    // [31:29]
    } b;
} REG_GOUDA_TECON2_T;

// gd_command
#define GOUDA_START (1 << 0)

// gd_status
#define GOUDA_IA_BUSY (1 << 0)
#define GOUDA_LCD_BUSY (1 << 4)

// gd_eof_irq
#define GOUDA_EOF_CAUSE (1 << 0)
#define GOUDA_EOF_STATUS (1 << 16)

// gd_eof_irq_mask
#define GOUDA_EOF_MASK (1 << 0)

// gd_roi_tl_ppos
#define GOUDA_X0(n) (((n)&0x7ff) << 0)
#define GOUDA_Y0(n) (((n)&0x7ff) << 16)

// gd_roi_br_ppos
#define GOUDA_X1(n) (((n)&0x7ff) << 0)
#define GOUDA_Y1(n) (((n)&0x7ff) << 16)

// gd_roi_bg_color
#define GOUDA_B(n) (((n)&0x1f) << 0)
#define GOUDA_G(n) (((n)&0x3f) << 5)
#define GOUDA_R(n) (((n)&0x1f) << 11)

// gd_vl_input_fmt
#define GOUDA_FORMAT(n) (((n)&0x3) << 0)
#define GOUDA_STRIDE(n) (((n)&0x1fff) << 2)
#define GOUDA_ACTIVE (1 << 31)

// gd_vl_tl_ppos
#define GOUDA_X0(n) (((n)&0x7ff) << 0)
#define GOUDA_Y0(n) (((n)&0x7ff) << 16)

// gd_vl_br_ppos
#define GOUDA_X1(n) (((n)&0x7ff) << 0)
#define GOUDA_Y1(n) (((n)&0x7ff) << 16)

// gd_vl_extents
#define GOUDA_MAX_LINE(n) (((n)&0x7ff) << 0)
#define GOUDA_MAX_COL(n) (((n)&0x7ff) << 16)

// gd_vl_blend_opt
#define GOUDA_CHROMA_KEY_COLOR(n) (((n)&0xffff) << 0)
#define GOUDA_CHROMA_KEY_B(n) (((n)&0x1f) << 0)
#define GOUDA_CHROMA_KEY_G(n) (((n)&0x3f) << 5)
#define GOUDA_CHROMA_KEY_R(n) (((n)&0x1f) << 11)
#define GOUDA_CHROMA_KEY_ENABLE (1 << 16)
#define GOUDA_CHROMA_KEY_MASK(n) (((n)&0x7) << 17)
#define GOUDA_ALPHA(n) (((n)&0xff) << 20)
#define GOUDA_ROTATION(n) (((n)&0x3) << 28)
#define GOUDA_DEPTH(n) (((n)&0x3) << 30)

// gd_vl_y_src
#define GOUDA_ADDR(n) (((n)&0x3fffffff) << 2)

// gd_vl_u_src
#define GOUDA_ADDR(n) (((n)&0x3fffffff) << 2)

// gd_vl_v_src
#define GOUDA_ADDR(n) (((n)&0x3fffffff) << 2)

// gd_vl_resc_ratio
#define GOUDA_XPITCH(n) (((n)&0x7ff) << 0)
#define GOUDA_YPITCH(n) (((n)&0x7ff) << 16)
#define GOUDA_PRE_FETCH_EN (1 << 29)
#define GOUDA_IY_DCTENABLE (1 << 30)
#define GOUDA_YPITCH_SCALE_ENABLE (1 << 31)

// gd_ol_input_fmt
#define GOUDA_FORMAT(n) (((n)&0x3) << 0)
#define GOUDA_STRIDE(n) (((n)&0x1fff) << 2)
#define GOUDA_PREFETCH (1 << 18)
#define GOUDA_ACTIVE (1 << 31)

// gd_ol_tl_ppos
#define GOUDA_X0(n) (((n)&0x7ff) << 0)
#define GOUDA_Y0(n) (((n)&0x7ff) << 16)

// gd_ol_br_ppos
#define GOUDA_X1(n) (((n)&0x7ff) << 0)
#define GOUDA_Y1(n) (((n)&0x7ff) << 16)

// gd_ol_blend_opt
#define GOUDA_CHROMA_KEY_COLOR(n) (((n)&0xffff) << 0)
#define GOUDA_CHROMA_KEY_B(n) (((n)&0x1f) << 0)
#define GOUDA_CHROMA_KEY_G(n) (((n)&0x3f) << 5)
#define GOUDA_CHROMA_KEY_R(n) (((n)&0x1f) << 11)
#define GOUDA_CHROMA_KEY_ENABLE (1 << 16)
#define GOUDA_CHROMA_KEY_MASK(n) (((n)&0x7) << 17)
#define GOUDA_ALPHA(n) (((n)&0xff) << 20)

// gd_ol_rgb_src
#define GOUDA_ADDR(n) (((n)&0x3fffffff) << 2)

// gd_lcd_ctrl
#define GOUDA_DESTINATION(n) (((n)&0x3) << 0)
#define GOUDA_DESTINATION_LCD_CS_0 (0 << 0)
#define GOUDA_DESTINATION_LCD_CS_1 (1 << 0)
#define GOUDA_DESTINATION_MEMORY_LCD_TYPE (2 << 0)
#define GOUDA_DESTINATION_MEMORY_RAM (3 << 0)
#define GOUDA_OUTPUT_FORMAT(n) (((n)&0x7) << 4)
#define GOUDA_OUTPUT_FORMAT_8_BIT_RGB332 (0 << 4)
#define GOUDA_OUTPUT_FORMAT_8_BIT_RGB444 (1 << 4)
#define GOUDA_OUTPUT_FORMAT_8_BIT_RGB565 (2 << 4)
#define GOUDA_OUTPUT_FORMAT_16_BIT_RGB332 (4 << 4)
#define GOUDA_OUTPUT_FORMAT_16_BIT_RGB444 (5 << 4)
#define GOUDA_OUTPUT_FORMAT_16_BIT_RGB565 (6 << 4)
#define GOUDA_HIGH_BYTE (1 << 7)
#define GOUDA_CS0_POLARITY (1 << 8)
#define GOUDA_CS1_POLARITY (1 << 9)
#define GOUDA_RS_POLARITY (1 << 10)
#define GOUDA_WR_POLARITY (1 << 11)
#define GOUDA_RD_POLARITY (1 << 12)
#define GOUDA_NB_COMMAND(n) (((n)&0x3f) << 16)
#define GOUDA_START_COMMAND (1 << 24)
#define GOUDA_LCD_RESETB (1 << 25)

#define GOUDA_DESTINATION_V_LCD_CS_0 (0)
#define GOUDA_DESTINATION_V_LCD_CS_1 (1)
#define GOUDA_DESTINATION_V_MEMORY_LCD_TYPE (2)
#define GOUDA_DESTINATION_V_MEMORY_RAM (3)
#define GOUDA_OUTPUT_FORMAT_V_8_BIT_RGB332 (0)
#define GOUDA_OUTPUT_FORMAT_V_8_BIT_RGB444 (1)
#define GOUDA_OUTPUT_FORMAT_V_8_BIT_RGB565 (2)
#define GOUDA_OUTPUT_FORMAT_V_16_BIT_RGB332 (4)
#define GOUDA_OUTPUT_FORMAT_V_16_BIT_RGB444 (5)
#define GOUDA_OUTPUT_FORMAT_V_16_BIT_RGB565 (6)

// gd_lcd_timing
#define GOUDA_TAS(n) (((n)&0x7) << 0)
#define GOUDA_TAH(n) (((n)&0x7) << 4)
#define GOUDA_PWL(n) (((n)&0x3f) << 8)
#define GOUDA_PWH(n) (((n)&0x3f) << 16)

// gd_lcd_mem_address
#define GOUDA_ADDR_DST(n) (((n)&0x3fffffff) << 2)

// gd_lcd_stride_offset
#define GOUDA_STRIDE_OFFSET(n) (((n)&0x3ff) << 0)

// gd_lcd_single_access
#define GOUDA_LCD_DATA(n) (((n)&0xffff) << 0)
#define GOUDA_TYPE (1 << 16)
#define GOUDA_START_WRITE (1 << 17)
#define GOUDA_START_READ (1 << 18)

// gd_spilcd_config
#define GOUDA_SPI_LCD_SELECT (1 << 0)
#define GOUDA_SPI_DEVICE_ID(n) (((n)&0x3f) << 1)
#define GOUDA_SPI_CLK_DIVIDER(n) (((n)&0xff) << 7)
#define GOUDA_SPI_DUMMY_CYCLE(n) (((n)&0x7) << 15)
#define GOUDA_SPI_LINE(n) (((n)&0x3) << 18)
#define GOUDA_SPI_LINE_4 (0 << 18)
#define GOUDA_SPI_LINE_3 (1 << 18)
#define GOUDA_SPI_LINE_4_START_BYTE (2 << 18)
#define GOUDA_SPI_LINE_3_TWO_LANE (3 << 18)
#define GOUDA_SPI_RX_BYTE(n) (((n)&0x7) << 20)
#define GOUDA_SPI_RW_WRITE (0 << 23)
#define GOUDA_SPI_RW_READ (1 << 23)

#define GOUDA_SPI_LINE_V_4 (0)
#define GOUDA_SPI_LINE_V_3 (1)
#define GOUDA_SPI_LINE_V_4_START_BYTE (2)
#define GOUDA_SPI_LINE_V_3_TWO_LANE (3)
#define GOUDA_SPI_RW_V_WRITE (0)
#define GOUDA_SPI_RW_V_READ (1)

// gd_spilcd_rd
#define GOUDA_SPI_LCD_RD(n) (((n)&0xffffffff) << 0)

// gd_vl_fix_ratio
#define GOUDA_L_XRATIO(n) (((n)&0xff) << 0)
#define GOUDA_L_YRATIO(n) (((n)&0xff) << 8)
#define GOUDA_L_XFIXEN (1 << 16)
#define GOUDA_L_YFIXEN (1 << 17)
#define GOUDA_MIRROR (1 << 18)
#define GOUDA_REG_VL_ONLY_SEL (1 << 19)

// tecon
#define GOUDA_TE_EN (1 << 0)
#define GOUDA_TE_EDGE_SEL (1 << 1)
#define GOUDA_TE_MODE (1 << 2)
#define GOUDA_TE_COUNT2(n) (((n)&0xfff) << 16)

// tecon2
#define GOUDA_TE_COUNT1(n) (((n)&0x1fffffff) << 0)

#endif // _GOUDA_H_
