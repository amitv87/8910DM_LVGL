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

#ifndef _WCN_SYS_CTRL_H_
#define _WCN_SYS_CTRL_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_WCN_SYS_CTRL_BASE (0x15000000)

typedef volatile struct
{
    uint32_t soft_reset;        // 0x00000000
    uint32_t clk_cfg;           // 0x00000004
    uint32_t clk_cfg1;          // 0x00000008
    uint32_t clk_208m_cfg;      // 0x0000000c
    uint32_t sys_conn;          // 0x00000010
    uint32_t riscv_boot_addr;   // 0x00000014
    uint32_t rf_cfg;            // 0x00000018
    uint32_t lvds_cfg;          // 0x0000001c
    uint32_t debug_config;      // 0x00000020
    uint32_t wakeup_ctrl_0;     // 0x00000024
    uint32_t wakeup_ctrl_1;     // 0x00000028
    uint32_t wakeup_status;     // 0x0000002c
    uint32_t extmem_offset;     // 0x00000030
    uint32_t wcn2sys_offset;    // 0x00000034
    uint32_t audio_out_cfg;     // 0x00000038
    uint32_t pulp_tag_mem_cfg;  // 0x0000003c
    uint32_t pulp_data_mem_cfg; // 0x00000040
    uint32_t wcn_rom_mem_cfg;   // 0x00000044
    uint32_t wcn_dram_mem_cfg;  // 0x00000048
    uint32_t wcn_bram_mem_cfg;  // 0x0000004c
    uint32_t bt_link_mem_cfg;   // 0x00000050
    uint32_t bt_modem_mem_cfg;  // 0x00000054
    uint32_t wlan_mem_cfg;      // 0x00000058
    uint32_t plc_mem_cfg;       // 0x0000005c
    uint32_t hclk_freq;         // 0x00000060
    uint32_t branch_addr;       // 0x00000064
    uint32_t revision_id;       // 0x00000068
    uint32_t __108[1];          // 0x0000006c
    uint32_t simu_rsvd;         // 0x00000070
} HWP_WCN_SYS_CTRL_T;

#define hwp_wcnSysCtrl ((HWP_WCN_SYS_CTRL_T *)REG_ACCESS_ADDRESS(REG_WCN_SYS_CTRL_BASE))

// soft_reset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_apb_presetn : 1;       // [0]
        uint32_t reg_mem_hresetn : 1;       // [1]
        uint32_t reg_pulp_mcu_hresetn : 1;  // [2]
        uint32_t reg_jtag_hresetn : 1;      // [3]
        uint32_t reg_sys_ifc_hresetn : 1;   // [4]
        uint32_t reg_aud_ifc_hresetn : 1;   // [5]
        uint32_t reg_bt_dbm_hresetn : 1;    // [6]
        uint32_t reg_bt_master_rstb : 1;    // [7]
        uint32_t reg_wdt_clk_rstb : 1;      // [8]
        uint32_t reg_uart_clk_rstb : 1;     // [9]
        uint32_t reg_low_power_rstb : 1;    // [10]
        uint32_t reg_bt_hresetn : 1;        // [11]
        uint32_t reg_wakeup_ctrl_rstb : 1;  // [12]
        uint32_t reg_wdt_rst_sys_en : 1;    // [13]
        uint32_t reg_pulp_dbg_rstb : 1;     // [14]
        uint32_t reg_dbg_hst_uart_rstb : 1; // [15]
        uint32_t __30_16 : 15;              // [30:16]
        uint32_t reg_sys_resetb : 1;        // [31]
    } b;
} REG_WCN_SYS_CTRL_SOFT_RESET_T;

// clk_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_mem_auto_ckg : 1;              // [0]
        uint32_t reg_apb_reg_clk_force_on : 1;      // [1]
        uint32_t reg_pulp_mcu_hclk_en : 1;          // [2]
        uint32_t reg_wdt_clk_en : 1;                // [3]
        uint32_t reg_uart_sys_clk_force_on : 1;     // [4]
        uint32_t reg_uart_clk_force_on : 1;         // [5]
        uint32_t reg_bt_dbm_hclk_en : 1;            // [6]
        uint32_t reg_bt_hclk_en : 1;                // [7]
        uint32_t reg_mem_hclk_en : 1;               // [8]
        uint32_t reg_aud_ifc_ch2_hclk_force_on : 1; // [9]
        uint32_t reg_aud_ifc_hclk_force_on : 1;     // [10]
        uint32_t reg_sys_ifc_ch_hclk_force_on : 2;  // [12:11]
        uint32_t reg_sys_ifc_hclk_force_on : 1;     // [13]
        uint32_t reg_jtag_hclk_en : 1;              // [14]
        uint32_t reg_low_power_clk_en : 1;          // [15]
        uint32_t reg_master_clk_en : 1;             // [16]
        uint32_t reg_bt_master_clk_ld : 1;          // [17], write set
        uint32_t reg_bt_master_clk_denom : 4;       // [21:18]
        uint32_t bt_clksel : 6;                     // [27:22]
        uint32_t reg_wakeup_ctrl_clk_en : 1;        // [28]
        uint32_t reg_bus_clk_sel : 3;               // [31:29]
    } b;
} REG_WCN_SYS_CTRL_CLK_CFG_T;

// clk_cfg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_uart_clk_denom : 13;             // [12:0]
        uint32_t reg_uart_clk_div_en : 1;             // [13]
        uint32_t reg_uart_clk_div_ld_cfg : 1;         // [14], write set
        uint32_t dbg_hst_uart_clk_en_force_on : 1;    // [15]
        uint32_t dbg_uart_sys_clk_en_force_on : 1;    // [16]
        uint32_t dbg_hst_sys_clk_en_force_on : 1;     // [17]
        uint32_t sysifc_dbg_hclk_clk_en_force_on : 1; // [18]
        uint32_t reg_bt_clk_en : 1;                   // [19]
        uint32_t reg_bt_master_clk_sel : 2;           // [21:20]
        uint32_t reg_bt_13m_clk_en : 1;               // [22]
        uint32_t __31_23 : 9;                         // [31:23]
    } b;
} REG_WCN_SYS_CTRL_CLK_CFG1_T;

// clk_208m_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_clk_208m_denom : 4;          // [3:0]
        uint32_t reg_clk_208m_num : 4;            // [7:4]
        uint32_t reg_clk_208m_div_en : 1;         // [8]
        uint32_t reg_clk_208m_div_ld_cfg : 1;     // [9], write set
        uint32_t reg_dbg_hst_div_en : 1;          // [10]
        uint32_t reg_dbg_hst_uart_clk_denom : 10; // [20:11]
        uint32_t reg_dbg_hst_uart_clk_num : 10;   // [30:21]
        uint32_t reg_dbg_hst_uart_clk_ld_cfg : 1; // [31], write set
    } b;
} REG_WCN_SYS_CTRL_CLK_208M_CFG_T;

// sys_conn
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt2sys_soft_int : 1;        // [0]
        uint32_t bt2sys_wakeup : 1;          // [1]
        uint32_t reg_wdt_gen_irq2sys_en : 1; // [2]
        uint32_t sys2bt_irq : 1;             // [3], read only
        uint32_t __31_4 : 28;                // [31:4]
    } b;
} REG_WCN_SYS_CTRL_SYS_CONN_T;

// rf_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_reg_src_sel : 1; // [0]
        uint32_t __31_1 : 31;        // [31:1]
    } b;
} REG_WCN_SYS_CTRL_RF_CFG_T;

// lvds_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_lvds_adc : 1;     // [0]
        uint32_t reg_lvds_dac : 1;     // [1]
        uint32_t reg_lvds_out_sel : 1; // [2]
        uint32_t reg_lvds_rx_mode : 2; // [4:3]
        uint32_t sdm_clk_sel : 1;      // [5]
        uint32_t sdm_clk_div_sel : 1;  // [6]
        uint32_t __31_7 : 25;          // [31:7]
    } b;
} REG_WCN_SYS_CTRL_LVDS_CFG_T;

// debug_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_dbg_out_sel : 4;               // [3:0]
        uint32_t reg_dbg_out_en : 1;                // [4]
        uint32_t reg_dbg_out_nibble_swap : 1;       // [5]
        uint32_t reg_dbg_out_byte_swap : 1;         // [6]
        uint32_t reg_dbg_out_nibble_shift : 1;      // [7]
        uint32_t reg_dbg_out_nibble_shift_mode : 1; // [8]
        uint32_t __21_9 : 13;                       // [21:9]
        uint32_t reg_dbg_clk_en : 1;                // [22]
        uint32_t reg_dbg_clk_sel : 4;               // [26:23]
        uint32_t reg_dbg_trig_en : 1;               // [27]
        uint32_t reg_dbg_trig_sel : 4;              // [31:28]
    } b;
} REG_WCN_SYS_CTRL_DEBUG_CONFIG_T;

// wakeup_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt2host_wakeup_period : 10;         // [9:0]
        uint32_t bt2host_wakeup_mode : 1;            // [10]
        uint32_t host2bt_hci_activity_wakeup_en : 1; // [11]
        uint32_t host2bt_hci_break_wakeup_en : 1;    // [12]
        uint32_t host2bt_ext_wakeup_en : 1;          // [13]
        uint32_t __31_14 : 18;                       // [31:14]
    } b;
} REG_WCN_SYS_CTRL_WAKEUP_CTRL_0_T;

// wakeup_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt2host_wakeup_trig : 1; // [0], write set
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_WCN_SYS_CTRL_WAKEUP_CTRL_1_T;

// wakeup_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt2host_wakeup_status : 1; // [0], read only
        uint32_t __31_1 : 31;               // [31:1]
    } b;
} REG_WCN_SYS_CTRL_WAKEUP_STATUS_T;

// audio_out_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_i2s_sel : 1; // [0]
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_WCN_SYS_CTRL_AUDIO_OUT_CFG_T;

// pulp_tag_mem_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tag_mem_cfg_shrink : 12; // [11:0]
        uint32_t __31_12 : 20;            // [31:12]
    } b;
} REG_WCN_SYS_CTRL_PULP_TAG_MEM_CFG_T;

// pulp_data_mem_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data_mem_cfg_shrink : 12; // [11:0]
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_WCN_SYS_CTRL_PULP_DATA_MEM_CFG_T;

// wcn_rom_mem_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rom_mem_cfg_shrink : 11; // [10:0]
        uint32_t __31_11 : 21;            // [31:11]
    } b;
} REG_WCN_SYS_CTRL_WCN_ROM_MEM_CFG_T;

// wcn_dram_mem_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dram_mem_cfg_shrink : 12; // [11:0]
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_WCN_SYS_CTRL_WCN_DRAM_MEM_CFG_T;

// wcn_bram_mem_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bram_mem_cfg_shrink : 12; // [11:0]
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_WCN_SYS_CTRL_WCN_BRAM_MEM_CFG_T;

// bt_link_mem_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_link_mem_cfg_shrink : 12; // [11:0]
        uint32_t __31_12 : 20;                // [31:12]
    } b;
} REG_WCN_SYS_CTRL_BT_LINK_MEM_CFG_T;

// bt_modem_mem_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_modem_mem_cfg_shrink : 12; // [11:0]
        uint32_t __31_12 : 20;                 // [31:12]
    } b;
} REG_WCN_SYS_CTRL_BT_MODEM_MEM_CFG_T;

// wlan_mem_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wlan_mem_cfg_shrink : 17; // [16:0]
        uint32_t __31_17 : 15;             // [31:17]
    } b;
} REG_WCN_SYS_CTRL_WLAN_MEM_CFG_T;

// plc_mem_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t plc_mem_cfg_shrink : 12; // [11:0]
        uint32_t __31_12 : 20;            // [31:12]
    } b;
} REG_WCN_SYS_CTRL_PLC_MEM_CFG_T;

// hclk_freq
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wcn_hclk_freq : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_WCN_SYS_CTRL_HCLK_FREQ_T;

// revision_id
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wcn_id : 8;  // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_WCN_SYS_CTRL_REVISION_ID_T;

// simu_rsvd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t simu_rsvd_0 : 8; // [7:0]
        uint32_t simu_rsvd_1 : 8; // [15:8]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_WCN_SYS_CTRL_SIMU_RSVD_T;

// soft_reset
#define WCN_SYS_CTRL_REG_APB_PRESETN (1 << 0)
#define WCN_SYS_CTRL_REG_MEM_HRESETN (1 << 1)
#define WCN_SYS_CTRL_REG_PULP_MCU_HRESETN (1 << 2)
#define WCN_SYS_CTRL_REG_JTAG_HRESETN (1 << 3)
#define WCN_SYS_CTRL_REG_SYS_IFC_HRESETN (1 << 4)
#define WCN_SYS_CTRL_REG_AUD_IFC_HRESETN (1 << 5)
#define WCN_SYS_CTRL_REG_BT_DBM_HRESETN (1 << 6)
#define WCN_SYS_CTRL_REG_BT_MASTER_RSTB (1 << 7)
#define WCN_SYS_CTRL_REG_WDT_CLK_RSTB (1 << 8)
#define WCN_SYS_CTRL_REG_UART_CLK_RSTB (1 << 9)
#define WCN_SYS_CTRL_REG_LOW_POWER_RSTB (1 << 10)
#define WCN_SYS_CTRL_REG_BT_HRESETN (1 << 11)
#define WCN_SYS_CTRL_REG_WAKEUP_CTRL_RSTB (1 << 12)
#define WCN_SYS_CTRL_REG_WDT_RST_SYS_EN (1 << 13)
#define WCN_SYS_CTRL_REG_PULP_DBG_RSTB (1 << 14)
#define WCN_SYS_CTRL_REG_DBG_HST_UART_RSTB (1 << 15)
#define WCN_SYS_CTRL_REG_SYS_RESETB (1 << 31)

// clk_cfg
#define WCN_SYS_CTRL_REG_MEM_AUTO_CKG (1 << 0)
#define WCN_SYS_CTRL_REG_APB_REG_CLK_FORCE_ON (1 << 1)
#define WCN_SYS_CTRL_REG_PULP_MCU_HCLK_EN (1 << 2)
#define WCN_SYS_CTRL_REG_WDT_CLK_EN (1 << 3)
#define WCN_SYS_CTRL_REG_UART_SYS_CLK_FORCE_ON (1 << 4)
#define WCN_SYS_CTRL_REG_UART_CLK_FORCE_ON (1 << 5)
#define WCN_SYS_CTRL_REG_BT_DBM_HCLK_EN (1 << 6)
#define WCN_SYS_CTRL_REG_BT_HCLK_EN (1 << 7)
#define WCN_SYS_CTRL_REG_MEM_HCLK_EN (1 << 8)
#define WCN_SYS_CTRL_REG_AUD_IFC_CH2_HCLK_FORCE_ON (1 << 9)
#define WCN_SYS_CTRL_REG_AUD_IFC_HCLK_FORCE_ON (1 << 10)
#define WCN_SYS_CTRL_REG_SYS_IFC_CH_HCLK_FORCE_ON(n) (((n)&0x3) << 11)
#define WCN_SYS_CTRL_REG_SYS_IFC_HCLK_FORCE_ON (1 << 13)
#define WCN_SYS_CTRL_REG_JTAG_HCLK_EN (1 << 14)
#define WCN_SYS_CTRL_REG_LOW_POWER_CLK_EN (1 << 15)
#define WCN_SYS_CTRL_REG_MASTER_CLK_EN (1 << 16)
#define WCN_SYS_CTRL_REG_BT_MASTER_CLK_LD (1 << 17)
#define WCN_SYS_CTRL_REG_BT_MASTER_CLK_DENOM(n) (((n)&0xf) << 18)
#define WCN_SYS_CTRL_BT_CLKSEL(n) (((n)&0x3f) << 22)
#define WCN_SYS_CTRL_REG_WAKEUP_CTRL_CLK_EN (1 << 28)
#define WCN_SYS_CTRL_REG_BUS_CLK_SEL(n) (((n)&0x7) << 29)

// clk_cfg1
#define WCN_SYS_CTRL_REG_UART_CLK_DENOM(n) (((n)&0x1fff) << 0)
#define WCN_SYS_CTRL_REG_UART_CLK_DIV_EN (1 << 13)
#define WCN_SYS_CTRL_REG_UART_CLK_DIV_LD_CFG (1 << 14)
#define WCN_SYS_CTRL_DBG_HST_UART_CLK_EN_FORCE_ON (1 << 15)
#define WCN_SYS_CTRL_DBG_UART_SYS_CLK_EN_FORCE_ON (1 << 16)
#define WCN_SYS_CTRL_DBG_HST_SYS_CLK_EN_FORCE_ON (1 << 17)
#define WCN_SYS_CTRL_SYSIFC_DBG_HCLK_CLK_EN_FORCE_ON (1 << 18)
#define WCN_SYS_CTRL_REG_BT_CLK_EN (1 << 19)
#define WCN_SYS_CTRL_REG_BT_MASTER_CLK_SEL(n) (((n)&0x3) << 20)
#define WCN_SYS_CTRL_REG_BT_13M_CLK_EN (1 << 22)

// clk_208m_cfg
#define WCN_SYS_CTRL_REG_CLK_208M_DENOM(n) (((n)&0xf) << 0)
#define WCN_SYS_CTRL_REG_CLK_208M_NUM(n) (((n)&0xf) << 4)
#define WCN_SYS_CTRL_REG_CLK_208M_DIV_EN (1 << 8)
#define WCN_SYS_CTRL_REG_CLK_208M_DIV_LD_CFG (1 << 9)
#define WCN_SYS_CTRL_REG_DBG_HST_DIV_EN (1 << 10)
#define WCN_SYS_CTRL_REG_DBG_HST_UART_CLK_DENOM(n) (((n)&0x3ff) << 11)
#define WCN_SYS_CTRL_REG_DBG_HST_UART_CLK_NUM(n) (((n)&0x3ff) << 21)
#define WCN_SYS_CTRL_REG_DBG_HST_UART_CLK_LD_CFG (1 << 31)

// sys_conn
#define WCN_SYS_CTRL_BT2SYS_SOFT_INT (1 << 0)
#define WCN_SYS_CTRL_BT2SYS_WAKEUP (1 << 1)
#define WCN_SYS_CTRL_REG_WDT_GEN_IRQ2SYS_EN (1 << 2)
#define WCN_SYS_CTRL_SYS2BT_IRQ (1 << 3)

// riscv_boot_addr
#define WCN_SYS_CTRL_PULP_MCU_BOOT_ADDR(n) (((n)&0xffffffff) << 0)

// rf_cfg
#define WCN_SYS_CTRL_RF_REG_SRC_SEL (1 << 0)

// lvds_cfg
#define WCN_SYS_CTRL_REG_LVDS_ADC (1 << 0)
#define WCN_SYS_CTRL_REG_LVDS_DAC (1 << 1)
#define WCN_SYS_CTRL_REG_LVDS_OUT_SEL (1 << 2)
#define WCN_SYS_CTRL_REG_LVDS_RX_MODE(n) (((n)&0x3) << 3)
#define WCN_SYS_CTRL_SDM_CLK_SEL (1 << 5)
#define WCN_SYS_CTRL_SDM_CLK_DIV_SEL (1 << 6)

// debug_config
#define WCN_SYS_CTRL_REG_DBG_OUT_SEL(n) (((n)&0xf) << 0)
#define WCN_SYS_CTRL_REG_DBG_OUT_EN (1 << 4)
#define WCN_SYS_CTRL_REG_DBG_OUT_NIBBLE_SWAP (1 << 5)
#define WCN_SYS_CTRL_REG_DBG_OUT_BYTE_SWAP (1 << 6)
#define WCN_SYS_CTRL_REG_DBG_OUT_NIBBLE_SHIFT (1 << 7)
#define WCN_SYS_CTRL_REG_DBG_OUT_NIBBLE_SHIFT_MODE (1 << 8)
#define WCN_SYS_CTRL_REG_DBG_CLK_EN (1 << 22)
#define WCN_SYS_CTRL_REG_DBG_CLK_SEL(n) (((n)&0xf) << 23)
#define WCN_SYS_CTRL_REG_DBG_TRIG_EN (1 << 27)
#define WCN_SYS_CTRL_REG_DBG_TRIG_SEL(n) (((n)&0xf) << 28)

// wakeup_ctrl_0
#define WCN_SYS_CTRL_BT2HOST_WAKEUP_PERIOD(n) (((n)&0x3ff) << 0)
#define WCN_SYS_CTRL_BT2HOST_WAKEUP_MODE (1 << 10)
#define WCN_SYS_CTRL_HOST2BT_HCI_ACTIVITY_WAKEUP_EN (1 << 11)
#define WCN_SYS_CTRL_HOST2BT_HCI_BREAK_WAKEUP_EN (1 << 12)
#define WCN_SYS_CTRL_HOST2BT_EXT_WAKEUP_EN (1 << 13)

// wakeup_ctrl_1
#define WCN_SYS_CTRL_BT2HOST_WAKEUP_TRIG (1 << 0)

// wakeup_status
#define WCN_SYS_CTRL_BT2HOST_WAKEUP_STATUS (1 << 0)

// extmem_offset
#define WCN_SYS_CTRL_REG_EXTMEM_OFFSET_ADDR(n) (((n)&0xffffffff) << 0)

// wcn2sys_offset
#define WCN_SYS_CTRL_REG_WCN2SYS_OFFSET_ADDR(n) (((n)&0xffffffff) << 0)

// audio_out_cfg
#define WCN_SYS_CTRL_REG_I2S_SEL (1 << 0)

// pulp_tag_mem_cfg
#define WCN_SYS_CTRL_TAG_MEM_CFG_SHRINK(n) (((n)&0xfff) << 0)

// pulp_data_mem_cfg
#define WCN_SYS_CTRL_DATA_MEM_CFG_SHRINK(n) (((n)&0xfff) << 0)

// wcn_rom_mem_cfg
#define WCN_SYS_CTRL_ROM_MEM_CFG_SHRINK(n) (((n)&0x7ff) << 0)

// wcn_dram_mem_cfg
#define WCN_SYS_CTRL_DRAM_MEM_CFG_SHRINK(n) (((n)&0xfff) << 0)

// wcn_bram_mem_cfg
#define WCN_SYS_CTRL_BRAM_MEM_CFG_SHRINK(n) (((n)&0xfff) << 0)

// bt_link_mem_cfg
#define WCN_SYS_CTRL_BT_LINK_MEM_CFG_SHRINK(n) (((n)&0xfff) << 0)

// bt_modem_mem_cfg
#define WCN_SYS_CTRL_BT_MODEM_MEM_CFG_SHRINK(n) (((n)&0xfff) << 0)

// wlan_mem_cfg
#define WCN_SYS_CTRL_WLAN_MEM_CFG_SHRINK(n) (((n)&0x1ffff) << 0)

// plc_mem_cfg
#define WCN_SYS_CTRL_PLC_MEM_CFG_SHRINK(n) (((n)&0xfff) << 0)

// hclk_freq
#define WCN_SYS_CTRL_WCN_HCLK_FREQ(n) (((n)&0xff) << 0)

// branch_addr
#define WCN_SYS_CTRL_RISC_BRANCH_ADDR(n) (((n)&0xffffffff) << 0)

// revision_id
#define WCN_SYS_CTRL_WCN_ID(n) (((n)&0xff) << 0)

// simu_rsvd
#define WCN_SYS_CTRL_SIMU_RSVD_0(n) (((n)&0xff) << 0)
#define WCN_SYS_CTRL_SIMU_RSVD_1(n) (((n)&0xff) << 8)

#endif // _WCN_SYS_CTRL_H_
