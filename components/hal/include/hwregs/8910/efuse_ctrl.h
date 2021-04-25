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

#ifndef _EFUSE_CTRL_H_
#define _EFUSE_CTRL_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_EFUSE_CTRL_BASE (0x0510e000)
#else
#define REG_EFUSE_CTRL_BASE (0x5010e000)
#endif

typedef volatile struct
{
    uint32_t __0[2];                    // 0x00000000
    uint32_t efuse_all0_index;          // 0x00000008
    uint32_t efuse_mode_ctrl;           // 0x0000000c
    uint32_t __16[1];                   // 0x00000010
    uint32_t efuse_ip_ver_reg;          // 0x00000014
    uint32_t efuse_cfg0;                // 0x00000018
    uint32_t __28[9];                   // 0x0000001c
    uint32_t efuse_sec_en;              // 0x00000040
    uint32_t efuse_sec_err_flag;        // 0x00000044
    uint32_t efuse_sec_flag_clr;        // 0x00000048
    uint32_t efuse_sec_magic_number;    // 0x0000004c
    uint32_t efuse_fw_cfg;              // 0x00000050
    uint32_t efuse_pw_swt;              // 0x00000054
    uint32_t ap_ca5_dbgen_reg;          // 0x00000058
    uint32_t ap_ca5_niden_reg;          // 0x0000005c
    uint32_t ap_ca5_spien_reg;          // 0x00000060
    uint32_t ap_ca5_spnien_reg;         // 0x00000064
    uint32_t ap_ca5_dap_deviceen_reg;   // 0x00000068
    uint32_t riscv_jtag_disable_reg;    // 0x0000006c
    uint32_t zsp_jtag_disable_reg;      // 0x00000070
    uint32_t debug_host_rx_disable_reg; // 0x00000074
    uint32_t uart_1_rx_disable_reg;     // 0x00000078
    uint32_t uart_2_rx_disable_reg;     // 0x0000007c
    uint32_t uart_3_rx_disable_reg;     // 0x00000080
    uint32_t uart_cp_rx_disable_reg;    // 0x00000084
    uint32_t mbist_disable_reg;         // 0x00000088
    uint32_t scan_disable_reg;          // 0x0000008c
    uint32_t efuse_bist_en_reg;         // 0x00000090
    uint32_t cp_ca5_dbgen_reg;          // 0x00000094
    uint32_t cp_ca5_niden_reg;          // 0x00000098
    uint32_t cp_ca5_spien_reg;          // 0x0000009c
    uint32_t cp_ca5_spnien_reg;         // 0x000000a0
    uint32_t cp_ca5_dcp_deviceen_reg;   // 0x000000a4
    uint32_t efuse_block0_rw_ctrl_reg;  // 0x000000a8
    uint32_t efuse_block1_rw_ctrl_reg;  // 0x000000ac
    uint32_t efuse_block2_rw_ctrl_reg;  // 0x000000b0
    uint32_t efuse_block3_rw_ctrl_reg;  // 0x000000b4
    uint32_t efuse_block4_rw_ctrl_reg;  // 0x000000b8
    uint32_t efuse_block5_rw_ctrl_reg;  // 0x000000bc
    uint32_t efuse_block6_rw_ctrl_reg;  // 0x000000c0
    uint32_t efuse_block7_rw_ctrl_reg;  // 0x000000c4
    uint32_t efuse_block8_rw_ctrl_reg;  // 0x000000c8
    uint32_t efuse_block9_rw_ctrl_reg;  // 0x000000cc
    uint32_t efuse_block10_rw_ctrl_reg; // 0x000000d0
    uint32_t efuse_block11_rw_ctrl_reg; // 0x000000d4
    uint32_t efuse_block12_rw_ctrl_reg; // 0x000000d8
    uint32_t efuse_block13_rw_ctrl_reg; // 0x000000dc
    uint32_t efuse_block14_rw_ctrl_reg; // 0x000000e0
    uint32_t efuse_block15_rw_ctrl_reg; // 0x000000e4
    uint32_t efuse_block16_rw_ctrl_reg; // 0x000000e8
    uint32_t efuse_block17_rw_ctrl_reg; // 0x000000ec
    uint32_t efuse_block18_rw_ctrl_reg; // 0x000000f0
    uint32_t efuse_block19_rw_ctrl_reg; // 0x000000f4
    uint32_t efuse_block20_rw_ctrl_reg; // 0x000000f8
    uint32_t efuse_block21_rw_ctrl_reg; // 0x000000fc
    uint32_t efuse_block22_rw_ctrl_reg; // 0x00000100
    uint32_t efuse_block23_rw_ctrl_reg; // 0x00000104
    uint32_t efuse_block24_rw_ctrl_reg; // 0x00000108
    uint32_t efuse_block25_rw_ctrl_reg; // 0x0000010c
    uint32_t efuse_block26_rw_ctrl_reg; // 0x00000110
    uint32_t efuse_block27_rw_ctrl_reg; // 0x00000114
    uint32_t efuse_block28_rw_ctrl_reg; // 0x00000118
    uint32_t efuse_block29_rw_ctrl_reg; // 0x0000011c
    uint32_t efuse_block30_rw_ctrl_reg; // 0x00000120
    uint32_t efuse_block31_rw_ctrl_reg; // 0x00000124
    uint32_t por_read_done_reg;         // 0x00000128
    uint32_t efuse_cfg_reg;             // 0x0000012c
    uint32_t efuse_block_en_reg;        // 0x00000130
    uint32_t por_read_data0_reg;        // 0x00000134
    uint32_t __312[2];                  // 0x00000138
    uint32_t wcn_jtag_disable_reg;      // 0x00000140
    uint32_t wcn_uart_disable_reg;      // 0x00000144
    uint32_t rf_uart_disable_reg;       // 0x00000148
    uint32_t __332[429];                // 0x0000014c
    uint32_t efuse_mem;                 // 0x00000800
} HWP_EFUSE_CTRL_T;

#define hwp_efuseCtrl ((HWP_EFUSE_CTRL_T *)REG_ACCESS_ADDRESS(REG_EFUSE_CTRL_BASE))

// efuse_all0_index
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_all0_end_index : 16;   // [15:0]
        uint32_t efuse_all0_start_index : 16; // [31:16]
    } b;
} REG_EFUSE_CTRL_EFUSE_ALL0_INDEX_T;

// efuse_mode_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_all0_check_start : 1; // [0]
        uint32_t __31_1 : 31;                // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_MODE_CTRL_T;

// efuse_ip_ver_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_ip_ver : 16; // [15:0], read only
        uint32_t efuse_type : 2;    // [17:16], read only
        uint32_t __31_18 : 14;      // [31:18]
    } b;
} REG_EFUSE_CTRL_EFUSE_IP_VER_REG_T;

// efuse_cfg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tpgm_time_cnt : 9;          // [8:0]
        uint32_t __15_9 : 7;                 // [15:9]
        uint32_t efuse_strobe_low_width : 8; // [23:16]
        uint32_t clk_efs_div : 8;            // [31:24]
    } b;
} REG_EFUSE_CTRL_EFUSE_CFG0_T;

// efuse_sec_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sec_vdd_en : 1;            // [0]
        uint32_t sec_auto_check_enable : 1; // [1]
        uint32_t double_bit_en_sec : 1;     // [2]
        uint32_t sec_margin_rd_enable : 1;  // [3]
        uint32_t sec_lock_bit_wr_en : 1;    // [4]
        uint32_t __31_5 : 27;               // [31:5]
    } b;
} REG_EFUSE_CTRL_EFUSE_SEC_EN_T;

// efuse_sec_err_flag
typedef union {
    uint32_t v;
    struct
    {
        uint32_t block_auto_check_failed : 1;        // [0], read only
        uint32_t shadow_block_auto_check_failed : 1; // [1], read only
        uint32_t __3_2 : 2;                          // [3:2]
        uint32_t sec_word0_prot_flag : 1;            // [4], read only
        uint32_t sec_word1_prot_flag : 1;            // [5], read only
        uint32_t __7_6 : 2;                          // [7:6]
        uint32_t sec_pg_en_wr_flag : 1;              // [8], read only
        uint32_t sec_vdd_on_rd_flag : 1;             // [9], read only
        uint32_t sec_block0_rd_flag : 1;             // [10], read only
        uint32_t sec_magnum_wr_flag : 1;             // [11], read only
        uint32_t sec_enk_err_flag : 1;               // [12], read only
        uint32_t sec_all0_check_flag : 1;            // [13], read only
        uint32_t __31_14 : 18;                       // [31:14]
    } b;
} REG_EFUSE_CTRL_EFUSE_SEC_ERR_FLAG_T;

// efuse_sec_flag_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sec_word0_err_clr : 1;  // [0]
        uint32_t sec_word1_err_clr : 1;  // [1]
        uint32_t __3_2 : 2;              // [3:2]
        uint32_t sec_word0_prot_clr : 1; // [4]
        uint32_t sec_word1_prot_clr : 1; // [5]
        uint32_t __7_6 : 2;              // [7:6]
        uint32_t sec_pg_en_wr_clr : 1;   // [8]
        uint32_t sec_vdd_on_rd_clr : 1;  // [9]
        uint32_t sec_block0_rd_clr : 1;  // [10]
        uint32_t sec_magnum_wr_clr : 1;  // [11]
        uint32_t sec_enk_err_clr : 1;    // [12]
        uint32_t sec_all0_check_clr : 1; // [13]
        uint32_t __31_14 : 18;           // [31:14]
    } b;
} REG_EFUSE_CTRL_EFUSE_SEC_FLAG_CLR_T;

// efuse_sec_magic_number
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sec_efuse_magic_number : 16; // [15:0]
        uint32_t __31_16 : 16;                // [31:16]
    } b;
} REG_EFUSE_CTRL_EFUSE_SEC_MAGIC_NUMBER_T;

// efuse_fw_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t conf_prot : 1;   // [0]
        uint32_t access_prot : 1; // [1]
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_EFUSE_CTRL_EFUSE_FW_CFG_T;

// efuse_pw_swt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efs_enk1_on : 1; // [0]
        uint32_t efs_enk2_on : 1; // [1]
        uint32_t ns_s_pg_en : 1;  // [2]
        uint32_t __31_3 : 29;     // [31:3]
    } b;
} REG_EFUSE_CTRL_EFUSE_PW_SWT_T;

// ap_ca5_dbgen_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_ca5_dbgen : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_EFUSE_CTRL_AP_CA5_DBGEN_REG_T;

// ap_ca5_niden_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_ca5_niden : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_EFUSE_CTRL_AP_CA5_NIDEN_REG_T;

// ap_ca5_spien_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_ca5_spien : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_EFUSE_CTRL_AP_CA5_SPIEN_REG_T;

// ap_ca5_spnien_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_ca5_spnien : 1; // [0]
        uint32_t __31_1 : 31;       // [31:1]
    } b;
} REG_EFUSE_CTRL_AP_CA5_SPNIEN_REG_T;

// ap_ca5_dap_deviceen_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_ca5_dap_deviceen : 1; // [0]
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_EFUSE_CTRL_AP_CA5_DAP_DEVICEEN_REG_T;

// riscv_jtag_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t riscv_jtag_disable : 1; // [0]
        uint32_t __31_1 : 31;            // [31:1]
    } b;
} REG_EFUSE_CTRL_RISCV_JTAG_DISABLE_REG_T;

// zsp_jtag_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsp_jtag_disable : 1; // [0]
        uint32_t __31_1 : 31;          // [31:1]
    } b;
} REG_EFUSE_CTRL_ZSP_JTAG_DISABLE_REG_T;

// debug_host_rx_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t debug_host_rx_disable : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_DEBUG_HOST_RX_DISABLE_REG_T;

// uart_1_rx_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart_1_rx_disable : 1; // [0]
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_EFUSE_CTRL_UART_1_RX_DISABLE_REG_T;

// uart_2_rx_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart_2_rx_disable : 1; // [0]
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_EFUSE_CTRL_UART_2_RX_DISABLE_REG_T;

// uart_3_rx_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart_3_rx_disable : 1; // [0]
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_EFUSE_CTRL_UART_3_RX_DISABLE_REG_T;

// uart_cp_rx_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart_cp_rx_disable : 1; // [0]
        uint32_t __31_1 : 31;            // [31:1]
    } b;
} REG_EFUSE_CTRL_UART_CP_RX_DISABLE_REG_T;

// mbist_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mbist_disable : 1; // [0]
        uint32_t __31_1 : 31;       // [31:1]
    } b;
} REG_EFUSE_CTRL_MBIST_DISABLE_REG_T;

// scan_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t scan_disable : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_EFUSE_CTRL_SCAN_DISABLE_REG_T;

// efuse_bist_en_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_bist_en : 1; // [0]
        uint32_t __31_1 : 31;       // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BIST_EN_REG_T;

// cp_ca5_dbgen_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cp_ca5_dbgen : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_EFUSE_CTRL_CP_CA5_DBGEN_REG_T;

// cp_ca5_niden_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cp_ca5_niden : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_EFUSE_CTRL_CP_CA5_NIDEN_REG_T;

// cp_ca5_spien_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cp_ca5_spien : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_EFUSE_CTRL_CP_CA5_SPIEN_REG_T;

// cp_ca5_spnien_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cp_ca5_spnien : 1; // [0]
        uint32_t __31_1 : 31;       // [31:1]
    } b;
} REG_EFUSE_CTRL_CP_CA5_SPNIEN_REG_T;

// cp_ca5_dcp_deviceen_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cp_ca5_dcp_deviceen : 1; // [0]
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_EFUSE_CTRL_CP_CA5_DCP_DEVICEEN_REG_T;

// efuse_block0_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block0_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK0_RW_CTRL_REG_T;

// efuse_block1_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block1_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK1_RW_CTRL_REG_T;

// efuse_block2_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block2_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK2_RW_CTRL_REG_T;

// efuse_block3_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block3_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK3_RW_CTRL_REG_T;

// efuse_block4_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block4_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK4_RW_CTRL_REG_T;

// efuse_block5_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block5_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK5_RW_CTRL_REG_T;

// efuse_block6_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block6_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK6_RW_CTRL_REG_T;

// efuse_block7_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block7_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK7_RW_CTRL_REG_T;

// efuse_block8_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block8_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK8_RW_CTRL_REG_T;

// efuse_block9_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block9_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK9_RW_CTRL_REG_T;

// efuse_block10_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block10_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK10_RW_CTRL_REG_T;

// efuse_block11_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block11_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK11_RW_CTRL_REG_T;

// efuse_block12_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block12_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK12_RW_CTRL_REG_T;

// efuse_block13_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block13_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK13_RW_CTRL_REG_T;

// efuse_block14_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block14_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK14_RW_CTRL_REG_T;

// efuse_block15_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block15_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK15_RW_CTRL_REG_T;

// efuse_block16_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block16_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK16_RW_CTRL_REG_T;

// efuse_block17_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block17_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK17_RW_CTRL_REG_T;

// efuse_block18_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block18_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK18_RW_CTRL_REG_T;

// efuse_block19_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block19_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK19_RW_CTRL_REG_T;

// efuse_block20_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block20_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK20_RW_CTRL_REG_T;

// efuse_block21_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block21_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK21_RW_CTRL_REG_T;

// efuse_block22_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block22_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK22_RW_CTRL_REG_T;

// efuse_block23_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block23_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK23_RW_CTRL_REG_T;

// efuse_block24_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block24_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK24_RW_CTRL_REG_T;

// efuse_block25_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block25_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK25_RW_CTRL_REG_T;

// efuse_block26_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block26_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK26_RW_CTRL_REG_T;

// efuse_block27_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block27_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK27_RW_CTRL_REG_T;

// efuse_block28_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block28_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK28_RW_CTRL_REG_T;

// efuse_block29_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block29_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK29_RW_CTRL_REG_T;

// efuse_block30_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block30_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK30_RW_CTRL_REG_T;

// efuse_block31_rw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_block31_rw_ctrl : 1; // [0]
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_EFUSE_CTRL_EFUSE_BLOCK31_RW_CTRL_REG_T;

// por_read_done_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t por_read_done : 1; // [0], read only
        uint32_t __31_1 : 31;       // [31:1]
    } b;
} REG_EFUSE_CTRL_POR_READ_DONE_REG_T;

// efuse_cfg_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cp_ca5_dap_deviceen_status : 1;   // [0], read only
        uint32_t cp_ca5_spniden_status : 1;        // [1], read only
        uint32_t cp_ca5_spiden_status : 1;         // [2], read only
        uint32_t cp_ca5_niden_status : 1;          // [3], read only
        uint32_t cp_ca5_dbgen_status : 1;          // [4], read only
        uint32_t efuse_bist_en_status : 1;         // [5], read only
        uint32_t scan_disable_status : 1;          // [6], read only
        uint32_t mbist_disable_status : 1;         // [7], read only
        uint32_t uart_cp_rx_disable_status : 1;    // [8], read only
        uint32_t uart_3_rx_disable_status : 1;     // [9], read only
        uint32_t uart_2_rx_disable_status : 1;     // [10], read only
        uint32_t uart_1_rx_disable_status : 1;     // [11], read only
        uint32_t debug_host_rx_disable_status : 1; // [12], read only
        uint32_t zsp_jtag_disable_status : 1;      // [13], read only
        uint32_t riscv_jtag_disable_status : 1;    // [14], read only
        uint32_t ap_ca5_dap_deviceen_status : 1;   // [15], read only
        uint32_t ap_ca5_spniden_status : 1;        // [16], read only
        uint32_t ap_ca5_spiden_status : 1;         // [17], read only
        uint32_t ap_ca5_niden_status : 1;          // [18], read only
        uint32_t ap_ca5_dbgen_status : 1;          // [19], read only
        uint32_t __31_20 : 12;                     // [31:20]
    } b;
} REG_EFUSE_CTRL_EFUSE_CFG_REG_T;

// wcn_jtag_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wcn_jtag_disable : 1; // [0]
        uint32_t __31_1 : 31;          // [31:1]
    } b;
} REG_EFUSE_CTRL_WCN_JTAG_DISABLE_REG_T;

// wcn_uart_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wcn_uart_disable : 1; // [0]
        uint32_t __31_1 : 31;          // [31:1]
    } b;
} REG_EFUSE_CTRL_WCN_UART_DISABLE_REG_T;

// rf_uart_disable_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_uart_disable : 1; // [0]
        uint32_t __31_1 : 31;         // [31:1]
    } b;
} REG_EFUSE_CTRL_RF_UART_DISABLE_REG_T;

// efuse_all0_index
#define EFUSE_CTRL_EFUSE_ALL0_END_INDEX(n) (((n)&0xffff) << 0)
#define EFUSE_CTRL_EFUSE_ALL0_START_INDEX(n) (((n)&0xffff) << 16)

// efuse_mode_ctrl
#define EFUSE_CTRL_EFUSE_ALL0_CHECK_START (1 << 0)

// efuse_ip_ver_reg
#define EFUSE_CTRL_EFUSE_IP_VER(n) (((n)&0xffff) << 0)
#define EFUSE_CTRL_EFUSE_TYPE(n) (((n)&0x3) << 16)

// efuse_cfg0
#define EFUSE_CTRL_TPGM_TIME_CNT(n) (((n)&0x1ff) << 0)
#define EFUSE_CTRL_EFUSE_STROBE_LOW_WIDTH(n) (((n)&0xff) << 16)
#define EFUSE_CTRL_CLK_EFS_DIV(n) (((n)&0xff) << 24)

// efuse_sec_en
#define EFUSE_CTRL_SEC_VDD_EN (1 << 0)
#define EFUSE_CTRL_SEC_AUTO_CHECK_ENABLE (1 << 1)
#define EFUSE_CTRL_DOUBLE_BIT_EN_SEC (1 << 2)
#define EFUSE_CTRL_SEC_MARGIN_RD_ENABLE (1 << 3)
#define EFUSE_CTRL_SEC_LOCK_BIT_WR_EN (1 << 4)

// efuse_sec_err_flag
#define EFUSE_CTRL_BLOCK_AUTO_CHECK_FAILED (1 << 0)
#define EFUSE_CTRL_SHADOW_BLOCK_AUTO_CHECK_FAILED (1 << 1)
#define EFUSE_CTRL_SEC_WORD0_PROT_FLAG (1 << 4)
#define EFUSE_CTRL_SEC_WORD1_PROT_FLAG (1 << 5)
#define EFUSE_CTRL_SEC_PG_EN_WR_FLAG (1 << 8)
#define EFUSE_CTRL_SEC_VDD_ON_RD_FLAG (1 << 9)
#define EFUSE_CTRL_SEC_BLOCK0_RD_FLAG (1 << 10)
#define EFUSE_CTRL_SEC_MAGNUM_WR_FLAG (1 << 11)
#define EFUSE_CTRL_SEC_ENK_ERR_FLAG (1 << 12)
#define EFUSE_CTRL_SEC_ALL0_CHECK_FLAG (1 << 13)

// efuse_sec_flag_clr
#define EFUSE_CTRL_SEC_WORD0_ERR_CLR (1 << 0)
#define EFUSE_CTRL_SEC_WORD1_ERR_CLR (1 << 1)
#define EFUSE_CTRL_SEC_WORD0_PROT_CLR (1 << 4)
#define EFUSE_CTRL_SEC_WORD1_PROT_CLR (1 << 5)
#define EFUSE_CTRL_SEC_PG_EN_WR_CLR (1 << 8)
#define EFUSE_CTRL_SEC_VDD_ON_RD_CLR (1 << 9)
#define EFUSE_CTRL_SEC_BLOCK0_RD_CLR (1 << 10)
#define EFUSE_CTRL_SEC_MAGNUM_WR_CLR (1 << 11)
#define EFUSE_CTRL_SEC_ENK_ERR_CLR (1 << 12)
#define EFUSE_CTRL_SEC_ALL0_CHECK_CLR (1 << 13)

// efuse_sec_magic_number
#define EFUSE_CTRL_SEC_EFUSE_MAGIC_NUMBER(n) (((n)&0xffff) << 0)

// efuse_fw_cfg
#define EFUSE_CTRL_CONF_PROT (1 << 0)
#define EFUSE_CTRL_ACCESS_PROT (1 << 1)

// efuse_pw_swt
#define EFUSE_CTRL_EFS_ENK1_ON (1 << 0)
#define EFUSE_CTRL_EFS_ENK2_ON (1 << 1)
#define EFUSE_CTRL_NS_S_PG_EN (1 << 2)

// ap_ca5_dbgen_reg
#define EFUSE_CTRL_AP_CA5_DBGEN (1 << 0)

// ap_ca5_niden_reg
#define EFUSE_CTRL_AP_CA5_NIDEN (1 << 0)

// ap_ca5_spien_reg
#define EFUSE_CTRL_AP_CA5_SPIEN (1 << 0)

// ap_ca5_spnien_reg
#define EFUSE_CTRL_AP_CA5_SPNIEN (1 << 0)

// ap_ca5_dap_deviceen_reg
#define EFUSE_CTRL_AP_CA5_DAP_DEVICEEN (1 << 0)

// riscv_jtag_disable_reg
#define EFUSE_CTRL_RISCV_JTAG_DISABLE (1 << 0)

// zsp_jtag_disable_reg
#define EFUSE_CTRL_ZSP_JTAG_DISABLE (1 << 0)

// debug_host_rx_disable_reg
#define EFUSE_CTRL_DEBUG_HOST_RX_DISABLE (1 << 0)

// uart_1_rx_disable_reg
#define EFUSE_CTRL_UART_1_RX_DISABLE (1 << 0)

// uart_2_rx_disable_reg
#define EFUSE_CTRL_UART_2_RX_DISABLE (1 << 0)

// uart_3_rx_disable_reg
#define EFUSE_CTRL_UART_3_RX_DISABLE (1 << 0)

// uart_cp_rx_disable_reg
#define EFUSE_CTRL_UART_CP_RX_DISABLE (1 << 0)

// mbist_disable_reg
#define EFUSE_CTRL_MBIST_DISABLE (1 << 0)

// scan_disable_reg
#define EFUSE_CTRL_SCAN_DISABLE (1 << 0)

// efuse_bist_en_reg
#define EFUSE_CTRL_EFUSE_BIST_EN (1 << 0)

// cp_ca5_dbgen_reg
#define EFUSE_CTRL_CP_CA5_DBGEN (1 << 0)

// cp_ca5_niden_reg
#define EFUSE_CTRL_CP_CA5_NIDEN (1 << 0)

// cp_ca5_spien_reg
#define EFUSE_CTRL_CP_CA5_SPIEN (1 << 0)

// cp_ca5_spnien_reg
#define EFUSE_CTRL_CP_CA5_SPNIEN (1 << 0)

// cp_ca5_dcp_deviceen_reg
#define EFUSE_CTRL_CP_CA5_DCP_DEVICEEN (1 << 0)

// efuse_block0_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK0_RW_CTRL (1 << 0)

// efuse_block1_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK1_RW_CTRL (1 << 0)

// efuse_block2_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK2_RW_CTRL (1 << 0)

// efuse_block3_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK3_RW_CTRL (1 << 0)

// efuse_block4_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK4_RW_CTRL (1 << 0)

// efuse_block5_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK5_RW_CTRL (1 << 0)

// efuse_block6_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK6_RW_CTRL (1 << 0)

// efuse_block7_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK7_RW_CTRL (1 << 0)

// efuse_block8_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK8_RW_CTRL (1 << 0)

// efuse_block9_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK9_RW_CTRL (1 << 0)

// efuse_block10_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK10_RW_CTRL (1 << 0)

// efuse_block11_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK11_RW_CTRL (1 << 0)

// efuse_block12_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK12_RW_CTRL (1 << 0)

// efuse_block13_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK13_RW_CTRL (1 << 0)

// efuse_block14_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK14_RW_CTRL (1 << 0)

// efuse_block15_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK15_RW_CTRL (1 << 0)

// efuse_block16_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK16_RW_CTRL (1 << 0)

// efuse_block17_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK17_RW_CTRL (1 << 0)

// efuse_block18_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK18_RW_CTRL (1 << 0)

// efuse_block19_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK19_RW_CTRL (1 << 0)

// efuse_block20_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK20_RW_CTRL (1 << 0)

// efuse_block21_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK21_RW_CTRL (1 << 0)

// efuse_block22_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK22_RW_CTRL (1 << 0)

// efuse_block23_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK23_RW_CTRL (1 << 0)

// efuse_block24_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK24_RW_CTRL (1 << 0)

// efuse_block25_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK25_RW_CTRL (1 << 0)

// efuse_block26_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK26_RW_CTRL (1 << 0)

// efuse_block27_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK27_RW_CTRL (1 << 0)

// efuse_block28_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK28_RW_CTRL (1 << 0)

// efuse_block29_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK29_RW_CTRL (1 << 0)

// efuse_block30_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK30_RW_CTRL (1 << 0)

// efuse_block31_rw_ctrl_reg
#define EFUSE_CTRL_EFUSE_BLOCK31_RW_CTRL (1 << 0)

// por_read_done_reg
#define EFUSE_CTRL_POR_READ_DONE (1 << 0)

// efuse_cfg_reg
#define EFUSE_CTRL_CP_CA5_DAP_DEVICEEN_STATUS (1 << 0)
#define EFUSE_CTRL_CP_CA5_SPNIDEN_STATUS (1 << 1)
#define EFUSE_CTRL_CP_CA5_SPIDEN_STATUS (1 << 2)
#define EFUSE_CTRL_CP_CA5_NIDEN_STATUS (1 << 3)
#define EFUSE_CTRL_CP_CA5_DBGEN_STATUS (1 << 4)
#define EFUSE_CTRL_EFUSE_BIST_EN_STATUS (1 << 5)
#define EFUSE_CTRL_SCAN_DISABLE_STATUS (1 << 6)
#define EFUSE_CTRL_MBIST_DISABLE_STATUS (1 << 7)
#define EFUSE_CTRL_UART_CP_RX_DISABLE_STATUS (1 << 8)
#define EFUSE_CTRL_UART_3_RX_DISABLE_STATUS (1 << 9)
#define EFUSE_CTRL_UART_2_RX_DISABLE_STATUS (1 << 10)
#define EFUSE_CTRL_UART_1_RX_DISABLE_STATUS (1 << 11)
#define EFUSE_CTRL_DEBUG_HOST_RX_DISABLE_STATUS (1 << 12)
#define EFUSE_CTRL_ZSP_JTAG_DISABLE_STATUS (1 << 13)
#define EFUSE_CTRL_RISCV_JTAG_DISABLE_STATUS (1 << 14)
#define EFUSE_CTRL_AP_CA5_DAP_DEVICEEN_STATUS (1 << 15)
#define EFUSE_CTRL_AP_CA5_SPNIDEN_STATUS (1 << 16)
#define EFUSE_CTRL_AP_CA5_SPIDEN_STATUS (1 << 17)
#define EFUSE_CTRL_AP_CA5_NIDEN_STATUS (1 << 18)
#define EFUSE_CTRL_AP_CA5_DBGEN_STATUS (1 << 19)

// efuse_block_en_reg
#define EFUSE_CTRL_EFUSE_BLOCK_EN(n) (((n)&0xffffffff) << 0)

// por_read_data0_reg
#define EFUSE_CTRL_POR_READ_DATA0(n) (((n)&0xffffffff) << 0)

// wcn_jtag_disable_reg
#define EFUSE_CTRL_WCN_JTAG_DISABLE (1 << 0)

// wcn_uart_disable_reg
#define EFUSE_CTRL_WCN_UART_DISABLE (1 << 0)

// rf_uart_disable_reg
#define EFUSE_CTRL_RF_UART_DISABLE (1 << 0)

// efuse_mem
#define EFUSE_CTRL_EFUSE_DATA(n) (((n)&0xffffffff) << 0)

#endif // _EFUSE_CTRL_H_
