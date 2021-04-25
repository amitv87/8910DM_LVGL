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

#ifndef _WCN_RF_IF_H_
#define _WCN_RF_IF_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_WCN_RF_IF_BASE (0x15005000)

typedef volatile struct
{
    uint32_t revision;              // 0x00000000
    uint32_t sys_control;           // 0x00000004
    uint32_t bt_control;            // 0x00000008
    uint32_t wf_control;            // 0x0000000c
    uint32_t fm_control;            // 0x00000010
    uint32_t inter_freq_setting;    // 0x00000014
    uint32_t bandgap_setting;       // 0x00000018
    uint32_t lna_rmx_setting;       // 0x0000001c
    uint32_t pga_setting0;          // 0x00000020
    uint32_t pga_setting1;          // 0x00000024
    uint32_t rxflt_setting;         // 0x00000028
    uint32_t adc_setting0;          // 0x0000002c
    uint32_t adc_setting1;          // 0x00000030
    uint32_t bt_dac_setting;        // 0x00000034
    uint32_t bt_txrf_setting;       // 0x00000038
    uint32_t fm_dsp_setting;        // 0x0000003c
    uint32_t vco_setting;           // 0x00000040
    uint32_t pll_setting0;          // 0x00000044
    uint32_t pll_setting1;          // 0x00000048
    uint32_t pll_setting2;          // 0x0000004c
    uint32_t pll_status;            // 0x00000050
    uint32_t adpll_setting0;        // 0x00000054
    uint32_t adpll_setting1;        // 0x00000058
    uint32_t adpll_status;          // 0x0000005c
    uint32_t mdll_setting;          // 0x00000060
    uint32_t pll_sdm_setting0;      // 0x00000064
    uint32_t pll_sdm_setting1;      // 0x00000068
    uint32_t adpll_sdm_setting0;    // 0x0000006c
    uint32_t adpll_sdm_setting1;    // 0x00000070
    uint32_t adpll_sdm_setting2;    // 0x00000074
    uint32_t rxflt_cal_setting0;    // 0x00000078
    uint32_t rxflt_cal_setting1;    // 0x0000007c
    uint32_t rxflt_cal_setting2;    // 0x00000080
    uint32_t rxflt_cal_setting3;    // 0x00000084
    uint32_t vco_pkd_cal_setting;   // 0x00000088
    uint32_t pll_cal_setting0;      // 0x0000008c
    uint32_t pll_cal_setting1;      // 0x00000090
    uint32_t adc_cal_setting;       // 0x00000094
    uint32_t cal_dr_setting;        // 0x00000098
    uint32_t cal_status;            // 0x0000009c
    uint32_t cal_results0;          // 0x000000a0
    uint32_t cal_results1;          // 0x000000a4
    uint32_t power_dr;              // 0x000000a8
    uint32_t power_reg;             // 0x000000ac
    uint32_t power_status;          // 0x000000b0
    uint32_t bt_gain_table_0;       // 0x000000b4
    uint32_t bt_gain_table_1;       // 0x000000b8
    uint32_t bt_gain_table_2;       // 0x000000bc
    uint32_t bt_gain_table_3;       // 0x000000c0
    uint32_t bt_gain_table_4;       // 0x000000c4
    uint32_t bt_gain_table_5;       // 0x000000c8
    uint32_t bt_gain_table_6;       // 0x000000cc
    uint32_t bt_gain_table_7;       // 0x000000d0
    uint32_t bt_gain_table_8;       // 0x000000d4
    uint32_t bt_gain_table_9;       // 0x000000d8
    uint32_t bt_gain_table_a;       // 0x000000dc
    uint32_t bt_gain_table_b;       // 0x000000e0
    uint32_t bt_gain_table_c;       // 0x000000e4
    uint32_t bt_gain_table_d;       // 0x000000e8
    uint32_t bt_gain_table_e;       // 0x000000ec
    uint32_t bt_gain_table_f;       // 0x000000f0
    uint32_t wf_gain_table_0;       // 0x000000f4
    uint32_t wf_gain_table_1;       // 0x000000f8
    uint32_t wf_gain_table_2;       // 0x000000fc
    uint32_t wf_gain_table_3;       // 0x00000100
    uint32_t wf_gain_table_4;       // 0x00000104
    uint32_t wf_gain_table_5;       // 0x00000108
    uint32_t wf_gain_table_6;       // 0x0000010c
    uint32_t wf_gain_table_7;       // 0x00000110
    uint32_t wf_gain_table_8;       // 0x00000114
    uint32_t wf_gain_table_9;       // 0x00000118
    uint32_t wf_gain_table_a;       // 0x0000011c
    uint32_t wf_gain_table_b;       // 0x00000120
    uint32_t wf_gain_table_c;       // 0x00000124
    uint32_t wf_gain_table_d;       // 0x00000128
    uint32_t wf_gain_table_e;       // 0x0000012c
    uint32_t wf_gain_table_f;       // 0x00000130
    uint32_t bt_tx_gain_table_0;    // 0x00000134
    uint32_t bt_tx_gain_table_1;    // 0x00000138
    uint32_t bt_tx_gain_table_2;    // 0x0000013c
    uint32_t bt_tx_gain_table_3;    // 0x00000140
    uint32_t fm_gain_table_0;       // 0x00000144
    uint32_t fm_gain_table_1;       // 0x00000148
    uint32_t fm_gain_table_2;       // 0x0000014c
    uint32_t fm_gain_table_3;       // 0x00000150
    uint32_t fm_gain_table_4;       // 0x00000154
    uint32_t fm_gain_table_5;       // 0x00000158
    uint32_t fm_gain_table_6;       // 0x0000015c
    uint32_t fm_gain_table_7;       // 0x00000160
    uint32_t direct_gain_setting;   // 0x00000164
    uint32_t bt_rxflt_cal_bit_reg0; // 0x00000168
    uint32_t bt_rxflt_cal_bit_reg1; // 0x0000016c
    uint32_t bt_rxflt_cal_bit_reg2; // 0x00000170
    uint32_t bt_rxflt_cal_bit_reg3; // 0x00000174
    uint32_t bt_rxflt_cal_bit_reg4; // 0x00000178
    uint32_t bt_rxflt_cal_bit_reg5; // 0x0000017c
    uint32_t bt_rxflt_cal_bit_reg6; // 0x00000180
    uint32_t bt_rxflt_cal_bit_reg7; // 0x00000184
    uint32_t bt_rxflt_cal_bit_reg8; // 0x00000188
    uint32_t bt_rxflt_cal_bit_reg9; // 0x0000018c
    uint32_t bt_rxflt_cal_bit_rega; // 0x00000190
    uint32_t bt_rxflt_cal_bit_regb; // 0x00000194
    uint32_t wf_rxflt_cal_bit_reg0; // 0x00000198
    uint32_t wf_rxflt_cal_bit_reg1; // 0x0000019c
    uint32_t wf_rxflt_cal_bit_reg2; // 0x000001a0
    uint32_t wf_rxflt_cal_bit_reg3; // 0x000001a4
    uint32_t wf_rxflt_cal_bit_reg4; // 0x000001a8
    uint32_t wf_rxflt_cal_bit_reg5; // 0x000001ac
    uint32_t wf_rxflt_cal_bit_reg6; // 0x000001b0
    uint32_t wf_rxflt_cal_bit_reg7; // 0x000001b4
    uint32_t wf_rxflt_cal_bit_reg8; // 0x000001b8
    uint32_t wf_rxflt_cal_bit_reg9; // 0x000001bc
    uint32_t wf_rxflt_cal_bit_rega; // 0x000001c0
    uint32_t wf_rxflt_cal_bit_regb; // 0x000001c4
    uint32_t fm_rxflt_cal_bit_reg0; // 0x000001c8
    uint32_t fm_rxflt_cal_bit_reg1; // 0x000001cc
    uint32_t fm_rxflt_cal_bit_reg2; // 0x000001d0
    uint32_t fm_rxflt_cal_bit_reg3; // 0x000001d4
    uint32_t fm_rxflt_cal_bit_reg4; // 0x000001d8
    uint32_t fm_rxflt_cal_bit_reg5; // 0x000001dc
    uint32_t fm_rxflt_cal_bit_reg6; // 0x000001e0
    uint32_t fm_rxflt_cal_bit_reg7; // 0x000001e4
    uint32_t test_buf;              // 0x000001e8
    uint32_t ana_reserved;          // 0x000001ec
    uint32_t dig_reserved;          // 0x000001f0
    uint32_t new_add;               // 0x000001f4
} HWP_WCN_RF_IF_T;

#define hwp_wcnRfIf ((HWP_WCN_RF_IF_T *)REG_ACCESS_ADDRESS(REG_WCN_RF_IF_BASE))

// revision
typedef union {
    uint32_t v;
    struct
    {
        uint32_t revid : 4;   // [3:0], read only
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_WCN_RF_IF_REVISION_T;

// sys_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t soft_resetn : 1;            // [0]
        uint32_t chip_self_cal_enable : 1;   // [1]
        uint32_t rf_mode : 2;                // [3:2]
        uint32_t clk_digital_enable_reg : 1; // [4]
        uint32_t bt_tune_diff_en : 1;        // [5]
        uint32_t bt_hopping_en : 1;          // [6]
        uint32_t fm_adc_clk_mode : 1;        // [7]
        uint32_t bt_ch_ctrl_src_sel : 1;     // [8]
        uint32_t bt_tx_type : 1;             // [9]
        uint32_t __31_10 : 22;               // [31:10]
    } b;
} REG_WCN_RF_IF_SYS_CONTROL_T;

// bt_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_arfcn : 7;   // [6:0]
        uint32_t bt_ch_type : 1; // [7]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_WCN_RF_IF_BT_CONTROL_T;

// wf_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_tune : 1;         // [0]
        uint32_t wf_chn : 4;          // [4:1]
        uint32_t wf_freq_mode : 1;    // [5]
        uint32_t wf_freq_direct : 17; // [22:6]
        uint32_t __31_23 : 9;         // [31:23]
    } b;
} REG_WCN_RF_IF_WF_CONTROL_T;

// fm_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_tune : 1;         // [0]
        uint32_t fm_chan_reg : 11;    // [11:1]
        uint32_t fm_freq_mode : 1;    // [12]
        uint32_t fm_band_sel : 2;     // [14:13]
        uint32_t __15_15 : 1;         // [15]
        uint32_t fm_freq_direct : 16; // [31:16]
    } b;
} REG_WCN_RF_IF_FM_CONTROL_T;

// inter_freq_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_if : 11;    // [10:0]
        uint32_t bt_imgrej : 1; // [11]
        uint32_t bt_zif : 1;    // [12]
        uint32_t __15_13 : 3;   // [15:13]
        uint32_t fm_if : 11;    // [26:16]
        uint32_t fm_imgrej : 1; // [27]
        uint32_t __31_28 : 4;   // [31:28]
    } b;
} REG_WCN_RF_IF_INTER_FREQ_SETTING_T;

// bandgap_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sel_bg : 1;  // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_WCN_RF_IF_BANDGAP_SETTING_T;

// lna_rmx_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rmx_rshort_en : 1;      // [0]
        uint32_t fm_rmx_reg_bit : 3;        // [3:1]
        uint32_t fm_rmx_lobias_bit : 2;     // [5:4]
        uint32_t fm_rmx_harm_rej_en : 1;    // [6]
        uint32_t fm_lna_port_sel : 2;       // [8:7]
        uint32_t __11_9 : 3;                // [11:9]
        uint32_t fm_lna_reg_ictrl : 1;      // [12]
        uint32_t bt_rmx_lo_vcom_bit : 2;    // [14:13]
        uint32_t bt_tia_rin_bit : 2;        // [16:15]
        uint32_t wf_tia_rin_bit : 2;        // [18:17]
        uint32_t bt_rmx_disable : 1;        // [19]
        uint32_t bt_tia_bypass : 1;         // [20]
        uint32_t bt_lna_vcas_bit : 2;       // [22:21]
        uint32_t bt_lna_en : 1;             // [23]
        uint32_t bt_lna_input_short_tx : 1; // [24]
        uint32_t bt_lna_input_short_rx : 1; // [25]
        uint32_t __31_26 : 6;               // [31:26]
    } b;
} REG_WCN_RF_IF_LNA_RMX_SETTING_T;

// pga_setting0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pga_rs_bit : 5;     // [4:0]
        uint32_t pga_iq_sw : 1;      // [5]
        uint32_t pga_cf_bit : 5;     // [10:6]
        uint32_t pga_bw_tun_bit : 3; // [13:11]
        uint32_t pga_blk_mode : 1;   // [14]
        uint32_t pga_fm_en_reg : 1;  // [15]
        uint32_t pga_fm_en_dr : 1;   // [16]
        uint32_t __31_17 : 15;       // [31:17]
    } b;
} REG_WCN_RF_IF_PGA_SETTING0_T;

// pga_setting1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_pga_if_mode : 2;    // [1:0]
        uint32_t wf_pga_if_mode : 2;    // [3:2]
        uint32_t bt_pga_if_mode : 2;    // [5:4]
        uint32_t fm_pga_ibit : 2;       // [7:6]
        uint32_t wf_pga_ibit : 2;       // [9:8]
        uint32_t bt_pga_ibit : 2;       // [11:10]
        uint32_t fm_pga_bw_mode : 2;    // [13:12]
        uint32_t wf_pga_bw_mode : 2;    // [15:14]
        uint32_t bt_pga_bw_mode_2m : 2; // [17:16]
        uint32_t bt_pga_bw_mode_1m : 2; // [19:18]
        uint32_t fm_pga_gain_bit : 3;   // [22:20]
        uint32_t __31_23 : 9;           // [31:23]
    } b;
} REG_WCN_RF_IF_PGA_SETTING1_T;

// rxflt_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_wifi_hp : 1;     // [0]
        uint32_t wf_rxflt_wifi_hp : 1;     // [1]
        uint32_t bt_rxflt_wifi_hp : 1;     // [2]
        uint32_t fm_rxflt_if_mode : 2;     // [4:3]
        uint32_t wf_rxflt_if_mode : 2;     // [6:5]
        uint32_t bt_rxflt_if_mode : 2;     // [8:7]
        uint32_t fm_rxflt_ibit : 2;        // [10:9]
        uint32_t wf_rxflt_ibit : 2;        // [12:11]
        uint32_t bt_rxflt_ibit : 2;        // [14:13]
        uint32_t fm_rxflt_mode_sel : 2;    // [16:15]
        uint32_t wf_rxflt_mode_sel : 2;    // [18:17]
        uint32_t bt_rxflt_mode_sel_2m : 2; // [20:19]
        uint32_t bt_rxflt_mode_sel_1m : 2; // [22:21]
        uint32_t rxflt_cap_bit : 4;        // [26:23]
        uint32_t rxflt_iqswap : 1;         // [27]
        uint32_t rxflt_aux_en : 1;         // [28]
        uint32_t rxflt_rstn_reg : 1;       // [29]
        uint32_t rxflt_rstn_dr : 1;        // [30]
        uint32_t __31_31 : 1;              // [31]
    } b;
} REG_WCN_RF_IF_RXFLT_SETTING_T;

// adc_setting0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_clk_sel_reg : 3; // [2:0]
        uint32_t adc_clk_sel_dr : 1;  // [3]
        uint32_t adc_clk_div2 : 1;    // [4]
        uint32_t adc_clk_edge : 1;    // [5]
        uint32_t adc_vcm_sel : 2;     // [7:6]
        uint32_t adc_dly_in_ctrl : 1; // [8]
        uint32_t adc_delay_bit : 4;   // [12:9]
        uint32_t __31_13 : 19;        // [31:13]
    } b;
} REG_WCN_RF_IF_ADC_SETTING0_T;

// adc_setting1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_adc_reg_ibit : 3;    // [2:0]
        uint32_t wf_adc_reg_ibit : 3;    // [5:3]
        uint32_t bt_adc_reg_ibit_2m : 3; // [8:6]
        uint32_t bt_adc_reg_ibit_1m : 3; // [11:9]
        uint32_t fm_adc_ref_ibit : 3;    // [14:12]
        uint32_t wf_adc_ref_ibit : 3;    // [17:15]
        uint32_t bt_adc_ref_ibit_2m : 3; // [20:18]
        uint32_t bt_adc_ref_ibit_1m : 3; // [23:21]
        uint32_t __31_24 : 8;            // [31:24]
    } b;
} REG_WCN_RF_IF_ADC_SETTING1_T;

// bt_dac_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_dac_range_bit : 2;        // [1:0]
        uint32_t bt_dac_mux_en : 1;           // [2]
        uint32_t bt_dac_lpwr_mode : 1;        // [3]
        uint32_t bt_dac_iout_enable : 1;      // [4]
        uint32_t bt_dac_core_bit : 3;         // [7:5]
        uint32_t bt_dac_cm_bit : 2;           // [9:8]
        uint32_t bt_dac_auxout_en : 3;        // [12:10]
        uint32_t bt_dac_vtr_sel : 1;          // [13]
        uint32_t bt_dac_vlow_ctrl_bit : 3;    // [16:14]
        uint32_t bt_dac_clk_edge : 1;         // [17]
        uint32_t bt_dac_rstn_reg : 1;         // [18]
        uint32_t bt_dac_rstn_dr : 1;          // [19]
        uint32_t bt_dac_low_mag : 1;          // [20]
        uint32_t bt_dac_filter_ibias_bit : 2; // [22:21]
        uint32_t bt_dac_bw_tune_bit_2m : 4;   // [26:23]
        uint32_t bt_dac_bw_tune_bit_1m : 4;   // [30:27]
        uint32_t bt_tmx_cal_clk_edge : 1;     // [31]
    } b;
} REG_WCN_RF_IF_BT_DAC_SETTING_T;

// bt_txrf_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txrf_captune_bit_tx_hi : 4; // [3:0]
        uint32_t txrf_captune_bit_tx_md : 4; // [7:4]
        uint32_t txrf_captune_bit_tx_lo : 4; // [11:8]
        uint32_t txrf_captune_bit_rx_hi : 4; // [15:12]
        uint32_t txrf_captune_bit_rx_md : 4; // [19:16]
        uint32_t txrf_captune_bit_rx_lo : 4; // [23:20]
        uint32_t bt_delay_padrv : 8;         // [31:24]
    } b;
} REG_WCN_RF_IF_BT_TXRF_SETTING_T;

// fm_dsp_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_dsp_resetn_reg : 1;   // [0]
        uint32_t fm_dsp_resetn_dr : 1;    // [1]
        uint32_t fm_dsp_resetn_delay : 2; // [3:2]
        uint32_t __31_4 : 28;             // [31:4]
    } b;
} REG_WCN_RF_IF_FM_DSP_SETTING_T;

// vco_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_vco_vmode : 1;      // [0]
        uint32_t wf_vco_vmode : 1;      // [1]
        uint32_t bt_vco_vmode : 1;      // [2]
        uint32_t fm_vco_imode : 1;      // [3]
        uint32_t wf_vco_imode : 1;      // [4]
        uint32_t bt_vco_imode : 1;      // [5]
        uint32_t vco_vmode_bit : 3;     // [8:6]
        uint32_t vco_var_vcom : 3;      // [11:9]
        uint32_t vco_var_short : 1;     // [12]
        uint32_t vco_var_reverse : 1;   // [13]
        uint32_t vco_var_bit : 3;       // [16:14]
        uint32_t vco_buf_bias_bit : 2;  // [18:17]
        uint32_t vco_div2_bias_bit : 3; // [21:19]
        uint32_t vco_buf_ibit : 4;      // [25:22]
        uint32_t vco_regcap_selh : 1;   // [26]
        uint32_t __31_27 : 5;           // [31:27]
    } b;
} REG_WCN_RF_IF_VCO_SETTING_T;

// pll_setting0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pll_div_fm_adc_clk_reg : 5; // [4:0]
        uint32_t pll_div_fm_adc_clk_dr : 1;  // [5]
        uint32_t pll_div_fm_lo_clk_reg : 4;  // [9:6]
        uint32_t pll_div_fm_lo_clk_dr : 1;   // [10]
        uint32_t pll_bypass_notch : 1;       // [11]
        uint32_t pll_xfer_aux_en : 1;        // [12]
        uint32_t pll_sinc_mode : 3;          // [15:13]
        uint32_t pll_fbc_sel : 3;            // [18:16]
        uint32_t pll_open_en : 1;            // [19]
        uint32_t pll_reg_presc_rc : 2;       // [21:20]
        uint32_t pll_reg_presc : 4;          // [25:22]
        uint32_t pll_reg_peri_bit : 4;       // [29:26]
        uint32_t pll_test_en : 1;            // [30]
        uint32_t __31_31 : 1;                // [31]
    } b;
} REG_WCN_RF_IF_PLL_SETTING0_T;

// pll_setting1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pll_refmulti2_en_tx : 1; // [0]
        uint32_t pll_refmulti2_en_rx : 1; // [1]
        uint32_t pll_r_bit_tx : 2;        // [3:2]
        uint32_t pll_r_bit_rx : 2;        // [5:4]
        uint32_t pll_lpf_gain_tx : 4;     // [9:6]
        uint32_t pll_lpf_gain_rx : 4;     // [13:10]
        uint32_t pll_gain_bit_tx : 4;     // [17:14]
        uint32_t pll_gain_bit_rx : 4;     // [21:18]
        uint32_t pll_cp_r_bit_tx : 4;     // [25:22]
        uint32_t pll_cp_r_bit_rx : 4;     // [29:26]
        uint32_t pll_lowpwr_mode_tx : 1;  // [30]
        uint32_t pll_lowpwr_mode_rx : 1;  // [31]
    } b;
} REG_WCN_RF_IF_PLL_SETTING1_T;

// pll_setting2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mdll_div_tx : 4;            // [3:0]
        uint32_t mdll_div_rx : 4;            // [7:4]
        uint32_t pll_phase_ctrl_dly_tx : 2;  // [9:8]
        uint32_t pll_phase_ctrl_dly_rx : 2;  // [11:10]
        uint32_t pll_pfd_res_bit_tx : 6;     // [17:12]
        uint32_t pll_pfd_res_bit_rx : 6;     // [23:18]
        uint32_t pll_ref_mode_tx : 1;        // [24]
        uint32_t pll_ref_mode_rx : 1;        // [25]
        uint32_t pll_mdll_refclk_en_reg : 1; // [26]
        uint32_t pll_mdll_refclk_en_dr : 1;  // [27]
        uint32_t pll_clk_dig52m_en_reg : 1;  // [28]
        uint32_t pll_clk_dig52m_en_dr : 1;   // [29]
        uint32_t pll_bt_adc_clk_en_reg : 1;  // [30]
        uint32_t pll_bt_adc_clk_en_dr : 1;   // [31]
    } b;
} REG_WCN_RF_IF_PLL_SETTING2_T;

// pll_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pll_lock_det : 1;                  // [0], read only
        uint32_t pll_lock_flag : 1;                 // [1], read only
        uint32_t pll_lock_det_timer_delay_sel : 2;  // [3:2]
        uint32_t pll_lock_flag_timer_delay_sel : 2; // [5:4]
        uint32_t __31_6 : 26;                       // [31:6]
    } b;
} REG_WCN_RF_IF_PLL_STATUS_T;

// adpll_setting0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adpll_sdm_clk_test_en : 1;         // [0]
        uint32_t adpll_vco_low_test : 1;            // [1]
        uint32_t adpll_vco_high_test : 1;           // [2]
        uint32_t adpll_refmulti2_en : 1;            // [3]
        uint32_t adpll_pcon_mode : 1;               // [4]
        uint32_t adpll_vreg_bit : 4;                // [8:5]
        uint32_t adpll_test_en : 1;                 // [9]
        uint32_t adpll_res_bit : 2;                 // [11:10]
        uint32_t adpll_reg_res_bit : 2;             // [13:12]
        uint32_t adpll_cp_ibit : 3;                 // [16:14]
        uint32_t adpll_rstn_reg : 1;                // [17]
        uint32_t adpll_rstn_dr : 1;                 // [18]
        uint32_t adpll_clk_en_timer_delay : 1;      // [19]
        uint32_t adpll_rstn_timer_delay : 7;        // [26:20]
        uint32_t adpll_sdm_clk_sel_timer_delay : 3; // [29:27]
        uint32_t __31_30 : 2;                       // [31:30]
    } b;
} REG_WCN_RF_IF_ADPLL_SETTING0_T;

// adpll_setting1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adpll_clk2wf_dig_sel_reg : 1;   // [0]
        uint32_t adpll_clk2wf_dig_sel_dr : 1;    // [1]
        uint32_t adpll_clk2wf_dig_en_reg : 1;    // [2]
        uint32_t adpll_clk2wf_dig_en_dr : 1;     // [3]
        uint32_t adpll_clk2fmwf_adc_sel_reg : 1; // [4]
        uint32_t adpll_clk2fmwf_adc_sel_dr : 1;  // [5]
        uint32_t adpll_clk2fmwf_adc_en_reg : 1;  // [6]
        uint32_t adpll_clk2fmwf_adc_en_dr : 1;   // [7]
        uint32_t adpll_clk2bt_adc_sel_reg : 1;   // [8]
        uint32_t adpll_clk2bt_adc_sel_dr : 1;    // [9]
        uint32_t adpll_clk2bt_adc_en_reg : 1;    // [10]
        uint32_t adpll_clk2bt_adc_en_dr : 1;     // [11]
        uint32_t adpll_clk2bt_dig_sel : 1;       // [12]
        uint32_t adpll_clk2bt_dig_en : 1;        // [13]
        uint32_t __31_14 : 18;                   // [31:14]
    } b;
} REG_WCN_RF_IF_ADPLL_SETTING1_T;

// adpll_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adpll_lock_det : 1;                  // [0], read only
        uint32_t adpll_lock_flag : 1;                 // [1], read only
        uint32_t adpll_lock_det_timer_delay_sel : 2;  // [3:2]
        uint32_t adpll_lock_flag_timer_delay_sel : 2; // [5:4]
        uint32_t __31_6 : 26;                         // [31:6]
    } b;
} REG_WCN_RF_IF_ADPLL_STATUS_T;

// mdll_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mdll_div_bit : 5;       // [4:0]
        uint32_t mdll_band_sel : 1;      // [5]
        uint32_t mdll_band_bit : 3;      // [8:6]
        uint32_t mdll_cp_ibit : 3;       // [11:9]
        uint32_t mdll_startup_bit : 3;   // [14:12]
        uint32_t disable_refclk_pll : 1; // [15]
        uint32_t mdll_rstn_reg : 1;      // [16]
        uint32_t mdll_rstn_dr : 1;       // [17]
        uint32_t __31_18 : 14;           // [31:18]
    } b;
} REG_WCN_RF_IF_MDLL_SETTING_T;

// pll_sdm_setting0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pll_sdm_delay_sel : 4;         // [3:0]
        uint32_t pll_sdm_dither_bypass : 1;     // [4]
        uint32_t pll_sdm_int_dec_sel : 2;       // [6:5]
        uint32_t pll_sdm_clk_edge : 1;          // [7]
        uint32_t pll_sdm_resetn_reg : 1;        // [8]
        uint32_t pll_sdm_resetn_dr : 1;         // [9]
        uint32_t reset_pll_sdm_timer_delay : 4; // [13:10]
        uint32_t pll_sdm_clk_sel_1 : 1;         // [14]
        uint32_t pll_sdm_clk_sel_0 : 1;         // [15]
        uint32_t pll_div_dr : 1;                // [16]
        uint32_t pll_freq_former_bypass : 1;    // [17]
        uint32_t pll_freq_former_shift_ct : 3;  // [20:18]
        uint32_t __31_21 : 11;                  // [31:21]
    } b;
} REG_WCN_RF_IF_PLL_SDM_SETTING0_T;

// pll_sdm_setting1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pll_div_reg : 31; // [30:0]
        uint32_t __31_31 : 1;      // [31]
    } b;
} REG_WCN_RF_IF_PLL_SDM_SETTING1_T;

// adpll_sdm_setting0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adpll_sdm_dither_bypass : 1;     // [0]
        uint32_t adpll_sdm_int_dec_sel : 3;       // [3:1]
        uint32_t adpll_sdm_clk_fbc_inv : 1;       // [4]
        uint32_t adpll_sdm_resetn_reg : 1;        // [5]
        uint32_t adpll_sdm_resetn_dr : 1;         // [6]
        uint32_t reset_adpll_sdm_timer_delay : 4; // [10:7]
        uint32_t adpll_sdm_clk_sel_1 : 1;         // [11]
        uint32_t adpll_sdm_clk_sel_0 : 1;         // [12]
        uint32_t adpll_sdm_freq_dr : 1;           // [13]
        uint32_t __31_14 : 18;                    // [31:14]
    } b;
} REG_WCN_RF_IF_ADPLL_SDM_SETTING0_T;

// adpll_sdm_setting2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adpll_sdm_ss_devi_step : 16; // [15:0]
        uint32_t adpll_sdm_ss_devi : 12;      // [27:16]
        uint32_t adpll_sdm_ss_en : 1;         // [28]
        uint32_t __31_29 : 3;                 // [31:29]
    } b;
} REG_WCN_RF_IF_ADPLL_SDM_SETTING2_T;

// rxflt_cal_setting0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rxflt_cal_init_delay : 4;    // [3:0]
        uint32_t rxflt_cal_cnt : 4;           // [7:4]
        uint32_t rxflt_cal_iqswap : 1;        // [8]
        uint32_t rxflt_cal_polarity : 1;      // [9]
        uint32_t rxflt_cal_mode_sel : 2;      // [11:10]
        uint32_t rxflt_cal_clk_edge_sel : 1;  // [12]
        uint32_t rxflt_cal_clk_edge : 1;      // [13]
        uint32_t rxflt_cal_cycles : 2;        // [15:14]
        uint32_t wf_rxflt_cal_loop_coef : 15; // [30:16]
        uint32_t __31_31 : 1;                 // [31]
    } b;
} REG_WCN_RF_IF_RXFLT_CAL_SETTING0_T;

// rxflt_cal_setting1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rxflt_cal_q_bit_reg : 8;         // [7:0]
        uint32_t rxflt_cal_i_bit_reg : 8;         // [15:8]
        uint32_t wf_rxflt_cal_loop_adc_rng : 10;  // [25:16]
        uint32_t wf_rxflt_cal_loop_pol : 1;       // [26]
        uint32_t wf_rxflt_cal_loop_en : 1;        // [27]
        uint32_t fm_rxflt_cal_resolv_enhance : 1; // [28]
        uint32_t wf_rxflt_cal_resolv_enhance : 1; // [29]
        uint32_t bt_rxflt_cal_resolv_enhance : 1; // [30]
        uint32_t rxflt_cal_bit_dr : 1;            // [31]
    } b;
} REG_WCN_RF_IF_RXFLT_CAL_SETTING1_T;

// rxflt_cal_setting2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_chn_md : 7;                  // [6:0]
        uint32_t bt_rxflt_cal_inter_en : 1;      // [7]
        uint32_t wf_rxflt_cal_inter_coef_hi : 8; // [15:8]
        uint32_t wf_rxflt_cal_inter_coef_lo : 8; // [23:16]
        uint32_t wf_chn_md : 4;                  // [27:24]
        uint32_t wf_rxflt_cal_inter_en : 1;      // [28]
        uint32_t rxflt_cal_range_bit : 2;        // [30:29]
        uint32_t __31_31 : 1;                    // [31]
    } b;
} REG_WCN_RF_IF_RXFLT_CAL_SETTING2_T;

// rxflt_cal_setting3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_inter_coef_hi : 15; // [14:0]
        uint32_t __15_15 : 1;                     // [15]
        uint32_t bt_rxflt_cal_inter_coef_lo : 15; // [30:16]
        uint32_t __31_31 : 1;                     // [31]
    } b;
} REG_WCN_RF_IF_RXFLT_CAL_SETTING3_T;

// vco_pkd_cal_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vco_pkd_cal_polarity : 1;   // [0]
        uint32_t vco_pkd_cal_init_delay : 4; // [4:1]
        uint32_t vco_pkd_ref_bit : 3;        // [7:5]
        uint32_t vco_vbit_reg : 4;           // [11:8]
        uint32_t vco_vbit_dr : 1;            // [12]
        uint32_t vco_ibit_reg : 4;           // [16:13]
        uint32_t vco_ibit_dr : 1;            // [17]
        uint32_t __31_18 : 14;               // [31:18]
    } b;
} REG_WCN_RF_IF_VCO_PKD_CAL_SETTING_T;

// pll_cal_setting0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pll_init_delay : 3;        // [2:0]
        uint32_t pll_cal_cnt_sel : 3;       // [5:3]
        uint32_t pll_cal_opt : 1;           // [6]
        uint32_t pll_vco_bit_hold_time : 3; // [9:7]
        uint32_t pll_cal_freq_dr : 1;       // [10]
        uint32_t vco_band_reg : 10;         // [20:11]
        uint32_t vco_band_dr : 1;           // [21]
        uint32_t pll_cal_bit : 2;           // [23:22]
        uint32_t pll_cal_clk_sel : 1;       // [24]
        uint32_t __31_25 : 7;               // [31:25]
    } b;
} REG_WCN_RF_IF_PLL_CAL_SETTING0_T;

// pll_cal_setting1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pll_cal_freq_reg : 16; // [15:0]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_WCN_RF_IF_PLL_CAL_SETTING1_T;

// adc_cal_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_vreg_vbit_reg : 3;      // [2:0]
        uint32_t adc_vreg_vbit_dr : 1;       // [3]
        uint32_t adc_reg_cal_polarity : 1;   // [4]
        uint32_t adc_reg_cal_init_delay : 4; // [8:5]
        uint32_t adc_vref_vbit_reg : 3;      // [11:9]
        uint32_t adc_vref_vbit_dr : 1;       // [12]
        uint32_t adc_ref_cal_polarity : 1;   // [13]
        uint32_t adc_ref_cal_init_delay : 4; // [17:14]
        uint32_t adc_cal_rstn_reg : 1;       // [18]
        uint32_t adc_cal_rstn_dr : 1;        // [19]
        uint32_t __31_20 : 12;               // [31:20]
    } b;
} REG_WCN_RF_IF_ADC_CAL_SETTING_T;

// cal_dr_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pll_cal_en_reg : 1;     // [0]
        uint32_t pll_cal_en_dr : 1;      // [1]
        uint32_t rxflt_cal_en_reg : 1;   // [2]
        uint32_t rxflt_cal_en_dr : 1;    // [3]
        uint32_t adc_reg_cal_en_reg : 1; // [4]
        uint32_t adc_reg_cal_en_dr : 1;  // [5]
        uint32_t adc_ref_cal_en_reg : 1; // [6]
        uint32_t adc_ref_cal_en_dr : 1;  // [7]
        uint32_t vco_pkd_cal_en_reg : 1; // [8]
        uint32_t vco_pkd_cal_en_dr : 1;  // [9]
        uint32_t __31_10 : 22;           // [31:10]
    } b;
} REG_WCN_RF_IF_CAL_DR_SETTING_T;

// cal_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_self_cal_ready : 1; // [0], read only
        uint32_t wf_self_cal_ready : 1; // [1], read only
        uint32_t fm_self_cal_ready : 1; // [2], read only
        uint32_t pll_cal_ready : 1;     // [3], read only
        uint32_t rxflt_cal_ready : 1;   // [4], read only
        uint32_t adc_reg_cal_ready : 1; // [5], read only
        uint32_t adc_ref_cal_ready : 1; // [6], read only
        uint32_t vco_pkd_cal_ready : 1; // [7], read only
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_WCN_RF_IF_CAL_STATUS_T;

// cal_results0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rxflt_cal_q_bit : 8; // [7:0], read only
        uint32_t rxflt_cal_i_bit : 8; // [15:8], read only
        uint32_t adc_vreg_vbit : 3;   // [18:16], read only
        uint32_t adc_vref_vbit : 3;   // [21:19], read only
        uint32_t vco_vbit : 4;        // [25:22], read only
        uint32_t vco_ibit : 4;        // [29:26], read only
        uint32_t __31_30 : 2;         // [31:30]
    } b;
} REG_WCN_RF_IF_CAL_RESULTS0_T;

// cal_results1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vco_band : 10;       // [9:0], read only
        uint32_t rxflt_cal_out_q : 1; // [10], read only
        uint32_t rxflt_cal_out_i : 1; // [11], read only
        uint32_t adc_cal_ref_out : 1; // [12], read only
        uint32_t adc_cal_reg_out : 1; // [13], read only
        uint32_t vco_pkd_cal_out : 1; // [14], read only
        uint32_t __31_15 : 17;        // [31:15]
    } b;
} REG_WCN_RF_IF_CAL_RESULTS1_T;

// power_dr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pu_mdll_dr : 1;           // [0]
        uint32_t pu_adpll_dr : 1;          // [1]
        uint32_t pu_pll_fm_adc_clk_dr : 1; // [2]
        uint32_t pu_pll_fm_lo_clk_dr : 1;  // [3]
        uint32_t pu_pll_presc_dr : 1;      // [4]
        uint32_t pu_pll_peri_dr : 1;       // [5]
        uint32_t pu_vco_pkd_dr : 1;        // [6]
        uint32_t pu_vco_rxlo_dr : 1;       // [7]
        uint32_t pu_vco_txlo_dr : 1;       // [8]
        uint32_t pu_vco_dr : 1;            // [9]
        uint32_t pu_bt_txrf_dr : 1;        // [10]
        uint32_t pu_bt_tmx_dr : 1;         // [11]
        uint32_t pu_bt_padrv_dr : 1;       // [12]
        uint32_t pu_bt_dac_dr : 1;         // [13]
        uint32_t pu_adc_dr : 1;            // [14]
        uint32_t pu_rxflt_dr : 1;          // [15]
        uint32_t pu_pga_dr : 1;            // [16]
        uint32_t pu_bt_tia_dr : 1;         // [17]
        uint32_t pu_bt_lna_dr : 1;         // [18]
        uint32_t pu_fm_rmx_dr : 1;         // [19]
        uint32_t pu_fm_lna_dr : 1;         // [20]
        uint32_t pu_bg_dr : 1;             // [21]
        uint32_t __31_22 : 10;             // [31:22]
    } b;
} REG_WCN_RF_IF_POWER_DR_T;

// power_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pu_mdll_reg : 1;           // [0]
        uint32_t pu_adpll_reg : 1;          // [1]
        uint32_t pu_pll_fm_adc_clk_reg : 1; // [2]
        uint32_t pu_pll_fm_lo_clk_reg : 1;  // [3]
        uint32_t pu_pll_presc_reg : 1;      // [4]
        uint32_t pu_pll_peri_reg : 1;       // [5]
        uint32_t pu_vco_pkd_reg : 1;        // [6]
        uint32_t pu_vco_rxlo_reg : 1;       // [7]
        uint32_t pu_vco_txlo_reg : 1;       // [8]
        uint32_t pu_vco_reg : 1;            // [9]
        uint32_t pu_bt_txrf_reg : 1;        // [10]
        uint32_t pu_bt_tmx_reg : 1;         // [11]
        uint32_t pu_bt_padrv_reg : 1;       // [12]
        uint32_t pu_bt_dac_reg : 1;         // [13]
        uint32_t pu_adc_reg : 1;            // [14]
        uint32_t pu_rxflt_reg : 1;          // [15]
        uint32_t pu_pga_reg : 1;            // [16]
        uint32_t pu_bt_tia_reg : 1;         // [17]
        uint32_t pu_bt_lna_reg : 1;         // [18]
        uint32_t pu_fm_rmx_reg : 1;         // [19]
        uint32_t pu_fm_lna_reg : 1;         // [20]
        uint32_t pu_bg_reg : 1;             // [21]
        uint32_t __31_22 : 10;              // [31:22]
    } b;
} REG_WCN_RF_IF_POWER_REG_T;

// power_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pu_mdll : 1;           // [0], read only
        uint32_t pu_adpll : 1;          // [1], read only
        uint32_t pu_pll_fm_adc_clk : 1; // [2], read only
        uint32_t pu_pll_fm_lo_clk : 1;  // [3], read only
        uint32_t pu_pll_presc : 1;      // [4], read only
        uint32_t pu_pll_peri : 1;       // [5], read only
        uint32_t pu_vco_pkd : 1;        // [6], read only
        uint32_t pu_vco_rxlo : 1;       // [7], read only
        uint32_t pu_vco_txlo : 1;       // [8], read only
        uint32_t pu_vco : 1;            // [9], read only
        uint32_t pu_bt_txrf : 1;        // [10], read only
        uint32_t pu_bt_tmx : 1;         // [11], read only
        uint32_t pu_bt_padrv : 1;       // [12], read only
        uint32_t pu_bt_dac : 1;         // [13], read only
        uint32_t pu_adc : 1;            // [14], read only
        uint32_t pu_rxflt : 1;          // [15], read only
        uint32_t pu_pga : 1;            // [16], read only
        uint32_t pu_bt_tia : 1;         // [17], read only
        uint32_t pu_bt_lna : 1;         // [18], read only
        uint32_t pu_fm_rmx : 1;         // [19], read only
        uint32_t pu_fm_lna : 1;         // [20], read only
        uint32_t pu_bg : 1;             // [21], read only
        uint32_t __31_22 : 10;          // [31:22]
    } b;
} REG_WCN_RF_IF_POWER_STATUS_T;

// bt_gain_table_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_0 : 4; // [3:0]
        uint32_t bt_pga_gain_bit_0 : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_0 : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_0 : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_0 : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_0 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_0_T;

// bt_gain_table_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_1 : 4; // [3:0]
        uint32_t bt_pga_gain_bit_1 : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_1 : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_1 : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_1 : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_1 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_1_T;

// bt_gain_table_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_2 : 4; // [3:0]
        uint32_t bt_pga_gain_bit_2 : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_2 : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_2 : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_2 : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_2 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_2_T;

// bt_gain_table_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_3 : 4; // [3:0]
        uint32_t bt_pga_gain_bit_3 : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_3 : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_3 : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_3 : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_3 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_3_T;

// bt_gain_table_4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_4 : 4; // [3:0]
        uint32_t bt_pga_gain_bit_4 : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_4 : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_4 : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_4 : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_4 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_4_T;

// bt_gain_table_5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_5 : 4; // [3:0]
        uint32_t bt_pga_gain_bit_5 : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_5 : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_5 : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_5 : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_5 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_5_T;

// bt_gain_table_6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_6 : 4; // [3:0]
        uint32_t bt_pga_gain_bit_6 : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_6 : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_6 : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_6 : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_6 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_6_T;

// bt_gain_table_7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_7 : 4; // [3:0]
        uint32_t bt_pga_gain_bit_7 : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_7 : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_7 : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_7 : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_7 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_7_T;

// bt_gain_table_8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_8 : 4; // [3:0]
        uint32_t bt_pga_gain_bit_8 : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_8 : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_8 : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_8 : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_8 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_8_T;

// bt_gain_table_9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_9 : 4; // [3:0]
        uint32_t bt_pga_gain_bit_9 : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_9 : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_9 : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_9 : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_9 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_9_T;

// bt_gain_table_a
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_a : 4; // [3:0]
        uint32_t bt_pga_gain_bit_a : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_a : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_a : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_a : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_a : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_A_T;

// bt_gain_table_b
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_b : 4; // [3:0]
        uint32_t bt_pga_gain_bit_b : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_b : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_b : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_b : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_b : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_B_T;

// bt_gain_table_c
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_c : 4; // [3:0]
        uint32_t bt_pga_gain_bit_c : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_c : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_c : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_c : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_c : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_C_T;

// bt_gain_table_d
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_d : 4; // [3:0]
        uint32_t bt_pga_gain_bit_d : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_d : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_d : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_d : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_d : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_D_T;

// bt_gain_table_e
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_e : 4; // [3:0]
        uint32_t bt_pga_gain_bit_e : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_e : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_e : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_e : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_e : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_E_T;

// bt_gain_table_f
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_bt_gain_bit_f : 4; // [3:0]
        uint32_t bt_pga_gain_bit_f : 3;      // [6:4]
        uint32_t bt_lna_resf_bit_f : 4;      // [10:7]
        uint32_t bt_lna_gain3_bit_f : 2;     // [12:11]
        uint32_t bt_lna_gain2_bit_f : 2;     // [14:13]
        uint32_t bt_lna_gain1_bit_f : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_BT_GAIN_TABLE_F_T;

// wf_gain_table_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_0 : 4; // [3:0]
        uint32_t wf_pga_gain_bit_0 : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_0 : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_0 : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_0 : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_0 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_0_T;

// wf_gain_table_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_1 : 4; // [3:0]
        uint32_t wf_pga_gain_bit_1 : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_1 : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_1 : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_1 : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_1 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_1_T;

// wf_gain_table_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_2 : 4; // [3:0]
        uint32_t wf_pga_gain_bit_2 : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_2 : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_2 : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_2 : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_2 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_2_T;

// wf_gain_table_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_3 : 4; // [3:0]
        uint32_t wf_pga_gain_bit_3 : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_3 : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_3 : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_3 : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_3 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_3_T;

// wf_gain_table_4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_4 : 4; // [3:0]
        uint32_t wf_pga_gain_bit_4 : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_4 : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_4 : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_4 : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_4 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_4_T;

// wf_gain_table_5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_5 : 4; // [3:0]
        uint32_t wf_pga_gain_bit_5 : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_5 : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_5 : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_5 : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_5 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_5_T;

// wf_gain_table_6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_6 : 4; // [3:0]
        uint32_t wf_pga_gain_bit_6 : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_6 : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_6 : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_6 : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_6 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_6_T;

// wf_gain_table_7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_7 : 4; // [3:0]
        uint32_t wf_pga_gain_bit_7 : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_7 : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_7 : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_7 : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_7 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_7_T;

// wf_gain_table_8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_8 : 4; // [3:0]
        uint32_t wf_pga_gain_bit_8 : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_8 : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_8 : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_8 : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_8 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_8_T;

// wf_gain_table_9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_9 : 4; // [3:0]
        uint32_t wf_pga_gain_bit_9 : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_9 : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_9 : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_9 : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_9 : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_9_T;

// wf_gain_table_a
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_a : 4; // [3:0]
        uint32_t wf_pga_gain_bit_a : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_a : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_a : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_a : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_a : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_A_T;

// wf_gain_table_b
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_b : 4; // [3:0]
        uint32_t wf_pga_gain_bit_b : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_b : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_b : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_b : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_b : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_B_T;

// wf_gain_table_c
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_c : 4; // [3:0]
        uint32_t wf_pga_gain_bit_c : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_c : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_c : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_c : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_c : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_C_T;

// wf_gain_table_d
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_d : 4; // [3:0]
        uint32_t wf_pga_gain_bit_d : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_d : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_d : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_d : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_d : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_D_T;

// wf_gain_table_e
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_e : 4; // [3:0]
        uint32_t wf_pga_gain_bit_e : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_e : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_e : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_e : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_e : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_E_T;

// wf_gain_table_f
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_bt_gain_bit_f : 4; // [3:0]
        uint32_t wf_pga_gain_bit_f : 3;      // [6:4]
        uint32_t wf_lna_resf_bit_f : 4;      // [10:7]
        uint32_t wf_lna_gain3_bit_f : 2;     // [12:11]
        uint32_t wf_lna_gain2_bit_f : 2;     // [14:13]
        uint32_t wf_lna_gain1_bit_f : 2;     // [16:15]
        uint32_t __31_17 : 15;               // [31:17]
    } b;
} REG_WCN_RF_IF_WF_GAIN_TABLE_F_T;

// bt_tx_gain_table_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txrf_pad_cas_vbit_1 : 2;  // [1:0]
        uint32_t txrf_pad_aux_vbit_1 : 2;  // [3:2]
        uint32_t txrf_pad_bias_ibit_1 : 2; // [5:4]
        uint32_t txrf_att_en_1 : 2;        // [7:6]
        uint32_t txrf_gain_bit_1 : 4;      // [11:8]
        uint32_t txrf_pad_mode_1 : 1;      // [12]
        uint32_t __15_13 : 3;              // [15:13]
        uint32_t txrf_pad_cas_vbit_0 : 2;  // [17:16]
        uint32_t txrf_pad_aux_vbit_0 : 2;  // [19:18]
        uint32_t txrf_pad_bias_ibit_0 : 2; // [21:20]
        uint32_t txrf_att_en_0 : 2;        // [23:22]
        uint32_t txrf_gain_bit_0 : 4;      // [27:24]
        uint32_t txrf_pad_mode_0 : 1;      // [28]
        uint32_t __31_29 : 3;              // [31:29]
    } b;
} REG_WCN_RF_IF_BT_TX_GAIN_TABLE_0_T;

// bt_tx_gain_table_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txrf_pad_cas_vbit_3 : 2;  // [1:0]
        uint32_t txrf_pad_aux_vbit_3 : 2;  // [3:2]
        uint32_t txrf_pad_bias_ibit_3 : 2; // [5:4]
        uint32_t txrf_att_en_3 : 2;        // [7:6]
        uint32_t txrf_gain_bit_3 : 4;      // [11:8]
        uint32_t txrf_pad_mode_3 : 1;      // [12]
        uint32_t __15_13 : 3;              // [15:13]
        uint32_t txrf_pad_cas_vbit_2 : 2;  // [17:16]
        uint32_t txrf_pad_aux_vbit_2 : 2;  // [19:18]
        uint32_t txrf_pad_bias_ibit_2 : 2; // [21:20]
        uint32_t txrf_att_en_2 : 2;        // [23:22]
        uint32_t txrf_gain_bit_2 : 4;      // [27:24]
        uint32_t txrf_pad_mode_2 : 1;      // [28]
        uint32_t __31_29 : 3;              // [31:29]
    } b;
} REG_WCN_RF_IF_BT_TX_GAIN_TABLE_1_T;

// bt_tx_gain_table_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txrf_pad_cas_vbit_5 : 2;  // [1:0]
        uint32_t txrf_pad_aux_vbit_5 : 2;  // [3:2]
        uint32_t txrf_pad_bias_ibit_5 : 2; // [5:4]
        uint32_t txrf_att_en_5 : 2;        // [7:6]
        uint32_t txrf_gain_bit_5 : 4;      // [11:8]
        uint32_t txrf_pad_mode_5 : 1;      // [12]
        uint32_t __15_13 : 3;              // [15:13]
        uint32_t txrf_pad_cas_vbit_4 : 2;  // [17:16]
        uint32_t txrf_pad_aux_vbit_4 : 2;  // [19:18]
        uint32_t txrf_pad_bias_ibit_4 : 2; // [21:20]
        uint32_t txrf_att_en_4 : 2;        // [23:22]
        uint32_t txrf_gain_bit_4 : 4;      // [27:24]
        uint32_t txrf_pad_mode_4 : 1;      // [28]
        uint32_t __31_29 : 3;              // [31:29]
    } b;
} REG_WCN_RF_IF_BT_TX_GAIN_TABLE_2_T;

// bt_tx_gain_table_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txrf_pad_cas_vbit_7 : 2;  // [1:0]
        uint32_t txrf_pad_aux_vbit_7 : 2;  // [3:2]
        uint32_t txrf_pad_bias_ibit_7 : 2; // [5:4]
        uint32_t txrf_att_en_7 : 2;        // [7:6]
        uint32_t txrf_gain_bit_7 : 4;      // [11:8]
        uint32_t txrf_pad_mode_7 : 1;      // [12]
        uint32_t __15_13 : 3;              // [15:13]
        uint32_t txrf_pad_cas_vbit_6 : 2;  // [17:16]
        uint32_t txrf_pad_aux_vbit_6 : 2;  // [19:18]
        uint32_t txrf_pad_bias_ibit_6 : 2; // [21:20]
        uint32_t txrf_att_en_6 : 2;        // [23:22]
        uint32_t txrf_gain_bit_6 : 4;      // [27:24]
        uint32_t txrf_pad_mode_6 : 1;      // [28]
        uint32_t __31_29 : 3;              // [31:29]
    } b;
} REG_WCN_RF_IF_BT_TX_GAIN_TABLE_3_T;

// fm_gain_table_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_bt_gain_bit_1 : 4; // [3:0]
        uint32_t fm_rmx_gain_bit_1 : 3;      // [6:4]
        uint32_t fm_lna_rload_bit_1 : 2;     // [8:7]
        uint32_t fm_lna_gain_bit_1 : 2;      // [10:9]
        uint32_t fm_lna_reg_ibit_1 : 3;      // [13:11]
        uint32_t __15_14 : 2;                // [15:14]
        uint32_t fm_rxflt_bt_gain_bit_0 : 4; // [19:16]
        uint32_t fm_rmx_gain_bit_0 : 3;      // [22:20]
        uint32_t fm_lna_rload_bit_0 : 2;     // [24:23]
        uint32_t fm_lna_gain_bit_0 : 2;      // [26:25]
        uint32_t fm_lna_reg_ibit_0 : 3;      // [29:27]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_WCN_RF_IF_FM_GAIN_TABLE_0_T;

// fm_gain_table_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_bt_gain_bit_3 : 4; // [3:0]
        uint32_t fm_rmx_gain_bit_3 : 3;      // [6:4]
        uint32_t fm_lna_rload_bit_3 : 2;     // [8:7]
        uint32_t fm_lna_gain_bit_3 : 2;      // [10:9]
        uint32_t fm_lna_reg_ibit_3 : 3;      // [13:11]
        uint32_t __15_14 : 2;                // [15:14]
        uint32_t fm_rxflt_bt_gain_bit_2 : 4; // [19:16]
        uint32_t fm_rmx_gain_bit_2 : 3;      // [22:20]
        uint32_t fm_lna_rload_bit_2 : 2;     // [24:23]
        uint32_t fm_lna_gain_bit_2 : 2;      // [26:25]
        uint32_t fm_lna_reg_ibit_2 : 3;      // [29:27]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_WCN_RF_IF_FM_GAIN_TABLE_1_T;

// fm_gain_table_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_bt_gain_bit_5 : 4; // [3:0]
        uint32_t fm_rmx_gain_bit_5 : 3;      // [6:4]
        uint32_t fm_lna_rload_bit_5 : 2;     // [8:7]
        uint32_t fm_lna_gain_bit_5 : 2;      // [10:9]
        uint32_t fm_lna_reg_ibit_5 : 3;      // [13:11]
        uint32_t __15_14 : 2;                // [15:14]
        uint32_t fm_rxflt_bt_gain_bit_4 : 4; // [19:16]
        uint32_t fm_rmx_gain_bit_4 : 3;      // [22:20]
        uint32_t fm_lna_rload_bit_4 : 2;     // [24:23]
        uint32_t fm_lna_gain_bit_4 : 2;      // [26:25]
        uint32_t fm_lna_reg_ibit_4 : 3;      // [29:27]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_WCN_RF_IF_FM_GAIN_TABLE_2_T;

// fm_gain_table_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_bt_gain_bit_7 : 4; // [3:0]
        uint32_t fm_rmx_gain_bit_7 : 3;      // [6:4]
        uint32_t fm_lna_rload_bit_7 : 2;     // [8:7]
        uint32_t fm_lna_gain_bit_7 : 2;      // [10:9]
        uint32_t fm_lna_reg_ibit_7 : 3;      // [13:11]
        uint32_t __15_14 : 2;                // [15:14]
        uint32_t fm_rxflt_bt_gain_bit_6 : 4; // [19:16]
        uint32_t fm_rmx_gain_bit_6 : 3;      // [22:20]
        uint32_t fm_lna_rload_bit_6 : 2;     // [24:23]
        uint32_t fm_lna_gain_bit_6 : 2;      // [26:25]
        uint32_t fm_lna_reg_ibit_6 : 3;      // [29:27]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_WCN_RF_IF_FM_GAIN_TABLE_3_T;

// fm_gain_table_4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_bt_gain_bit_9 : 4; // [3:0]
        uint32_t fm_rmx_gain_bit_9 : 3;      // [6:4]
        uint32_t fm_lna_rload_bit_9 : 2;     // [8:7]
        uint32_t fm_lna_gain_bit_9 : 2;      // [10:9]
        uint32_t fm_lna_reg_ibit_9 : 3;      // [13:11]
        uint32_t __15_14 : 2;                // [15:14]
        uint32_t fm_rxflt_bt_gain_bit_8 : 4; // [19:16]
        uint32_t fm_rmx_gain_bit_8 : 3;      // [22:20]
        uint32_t fm_lna_rload_bit_8 : 2;     // [24:23]
        uint32_t fm_lna_gain_bit_8 : 2;      // [26:25]
        uint32_t fm_lna_reg_ibit_8 : 3;      // [29:27]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_WCN_RF_IF_FM_GAIN_TABLE_4_T;

// fm_gain_table_5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_bt_gain_bit_b : 4; // [3:0]
        uint32_t fm_rmx_gain_bit_b : 3;      // [6:4]
        uint32_t fm_lna_rload_bit_b : 2;     // [8:7]
        uint32_t fm_lna_gain_bit_b : 2;      // [10:9]
        uint32_t fm_lna_reg_ibit_b : 3;      // [13:11]
        uint32_t __15_14 : 2;                // [15:14]
        uint32_t fm_rxflt_bt_gain_bit_a : 4; // [19:16]
        uint32_t fm_rmx_gain_bit_a : 3;      // [22:20]
        uint32_t fm_lna_rload_bit_a : 2;     // [24:23]
        uint32_t fm_lna_gain_bit_a : 2;      // [26:25]
        uint32_t fm_lna_reg_ibit_a : 3;      // [29:27]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_WCN_RF_IF_FM_GAIN_TABLE_5_T;

// fm_gain_table_6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_bt_gain_bit_d : 4; // [3:0]
        uint32_t fm_rmx_gain_bit_d : 3;      // [6:4]
        uint32_t fm_lna_rload_bit_d : 2;     // [8:7]
        uint32_t fm_lna_gain_bit_d : 2;      // [10:9]
        uint32_t fm_lna_reg_ibit_d : 3;      // [13:11]
        uint32_t __15_14 : 2;                // [15:14]
        uint32_t fm_rxflt_bt_gain_bit_c : 4; // [19:16]
        uint32_t fm_rmx_gain_bit_c : 3;      // [22:20]
        uint32_t fm_lna_rload_bit_c : 2;     // [24:23]
        uint32_t fm_lna_gain_bit_c : 2;      // [26:25]
        uint32_t fm_lna_reg_ibit_c : 3;      // [29:27]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_WCN_RF_IF_FM_GAIN_TABLE_6_T;

// fm_gain_table_7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_bt_gain_bit_f : 4; // [3:0]
        uint32_t fm_rmx_gain_bit_f : 3;      // [6:4]
        uint32_t fm_lna_rload_bit_f : 2;     // [8:7]
        uint32_t fm_lna_gain_bit_f : 2;      // [10:9]
        uint32_t fm_lna_reg_ibit_f : 3;      // [13:11]
        uint32_t __15_14 : 2;                // [15:14]
        uint32_t fm_rxflt_bt_gain_bit_e : 4; // [19:16]
        uint32_t fm_rmx_gain_bit_e : 3;      // [22:20]
        uint32_t fm_lna_rload_bit_e : 2;     // [24:23]
        uint32_t fm_lna_gain_bit_e : 2;      // [26:25]
        uint32_t fm_lna_reg_ibit_e : 3;      // [29:27]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_WCN_RF_IF_FM_GAIN_TABLE_7_T;

// direct_gain_setting
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_agc_gain_reg : 4; // [3:0]
        uint32_t fm_agc_gain_dr : 1;  // [4]
        uint32_t __7_5 : 3;           // [7:5]
        uint32_t wf_agc_gain_reg : 4; // [11:8]
        uint32_t wf_agc_gain_dr : 1;  // [12]
        uint32_t __15_13 : 3;         // [15:13]
        uint32_t bt_agc_gain_reg : 4; // [19:16]
        uint32_t bt_agc_gain_dr : 1;  // [20]
        uint32_t __31_21 : 11;        // [31:21]
    } b;
} REG_WCN_RF_IF_DIRECT_GAIN_SETTING_T;

// bt_rxflt_cal_bit_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_1 : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_1 : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_0 : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_0 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REG0_T;

// bt_rxflt_cal_bit_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_3 : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_3 : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_2 : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_2 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REG1_T;

// bt_rxflt_cal_bit_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_5 : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_5 : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_4 : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_4 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REG2_T;

// bt_rxflt_cal_bit_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_7 : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_7 : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_6 : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_6 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REG3_T;

// bt_rxflt_cal_bit_reg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_9 : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_9 : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_8 : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_8 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REG4_T;

// bt_rxflt_cal_bit_reg5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_b : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_b : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_a : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_a : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REG5_T;

// bt_rxflt_cal_bit_reg6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_d : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_d : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_c : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_c : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REG6_T;

// bt_rxflt_cal_bit_reg7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_f : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_f : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_e : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_e : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REG7_T;

// bt_rxflt_cal_bit_reg8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_d_lo : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_d_lo : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_c_lo : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_c_lo : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REG8_T;

// bt_rxflt_cal_bit_reg9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_f_lo : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_f_lo : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_e_lo : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_e_lo : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REG9_T;

// bt_rxflt_cal_bit_rega
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_d_hi : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_d_hi : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_c_hi : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_c_hi : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REGA_T;

// bt_rxflt_cal_bit_regb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bt_rxflt_cal_q_bit_f_hi : 8; // [7:0]
        uint32_t bt_rxflt_cal_i_bit_f_hi : 8; // [15:8]
        uint32_t bt_rxflt_cal_q_bit_e_hi : 8; // [23:16]
        uint32_t bt_rxflt_cal_i_bit_e_hi : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_BT_RXFLT_CAL_BIT_REGB_T;

// wf_rxflt_cal_bit_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_1 : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_1 : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_0 : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_0 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REG0_T;

// wf_rxflt_cal_bit_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_3 : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_3 : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_2 : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_2 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REG1_T;

// wf_rxflt_cal_bit_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_5 : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_5 : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_4 : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_4 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REG2_T;

// wf_rxflt_cal_bit_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_7 : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_7 : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_6 : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_6 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REG3_T;

// wf_rxflt_cal_bit_reg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_9 : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_9 : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_8 : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_8 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REG4_T;

// wf_rxflt_cal_bit_reg5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_b : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_b : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_a : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_a : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REG5_T;

// wf_rxflt_cal_bit_reg6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_d : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_d : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_c : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_c : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REG6_T;

// wf_rxflt_cal_bit_reg7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_f : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_f : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_e : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_e : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REG7_T;

// wf_rxflt_cal_bit_reg8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_d_lo : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_d_lo : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_c_lo : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_c_lo : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REG8_T;

// wf_rxflt_cal_bit_reg9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_f_lo : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_f_lo : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_e_lo : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_e_lo : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REG9_T;

// wf_rxflt_cal_bit_rega
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_d_hi : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_d_hi : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_c_hi : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_c_hi : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REGA_T;

// wf_rxflt_cal_bit_regb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wf_rxflt_cal_q_bit_f_hi : 8; // [7:0]
        uint32_t wf_rxflt_cal_i_bit_f_hi : 8; // [15:8]
        uint32_t wf_rxflt_cal_q_bit_e_hi : 8; // [23:16]
        uint32_t wf_rxflt_cal_i_bit_e_hi : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_WF_RXFLT_CAL_BIT_REGB_T;

// fm_rxflt_cal_bit_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_cal_q_bit_1 : 8; // [7:0]
        uint32_t fm_rxflt_cal_i_bit_1 : 8; // [15:8]
        uint32_t fm_rxflt_cal_q_bit_0 : 8; // [23:16]
        uint32_t fm_rxflt_cal_i_bit_0 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_FM_RXFLT_CAL_BIT_REG0_T;

// fm_rxflt_cal_bit_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_cal_q_bit_3 : 8; // [7:0]
        uint32_t fm_rxflt_cal_i_bit_3 : 8; // [15:8]
        uint32_t fm_rxflt_cal_q_bit_2 : 8; // [23:16]
        uint32_t fm_rxflt_cal_i_bit_2 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_FM_RXFLT_CAL_BIT_REG1_T;

// fm_rxflt_cal_bit_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_cal_q_bit_5 : 8; // [7:0]
        uint32_t fm_rxflt_cal_i_bit_5 : 8; // [15:8]
        uint32_t fm_rxflt_cal_q_bit_4 : 8; // [23:16]
        uint32_t fm_rxflt_cal_i_bit_4 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_FM_RXFLT_CAL_BIT_REG2_T;

// fm_rxflt_cal_bit_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_cal_q_bit_7 : 8; // [7:0]
        uint32_t fm_rxflt_cal_i_bit_7 : 8; // [15:8]
        uint32_t fm_rxflt_cal_q_bit_6 : 8; // [23:16]
        uint32_t fm_rxflt_cal_i_bit_6 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_FM_RXFLT_CAL_BIT_REG3_T;

// fm_rxflt_cal_bit_reg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_cal_q_bit_9 : 8; // [7:0]
        uint32_t fm_rxflt_cal_i_bit_9 : 8; // [15:8]
        uint32_t fm_rxflt_cal_q_bit_8 : 8; // [23:16]
        uint32_t fm_rxflt_cal_i_bit_8 : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_FM_RXFLT_CAL_BIT_REG4_T;

// fm_rxflt_cal_bit_reg5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_cal_q_bit_b : 8; // [7:0]
        uint32_t fm_rxflt_cal_i_bit_b : 8; // [15:8]
        uint32_t fm_rxflt_cal_q_bit_a : 8; // [23:16]
        uint32_t fm_rxflt_cal_i_bit_a : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_FM_RXFLT_CAL_BIT_REG5_T;

// fm_rxflt_cal_bit_reg6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_cal_q_bit_d : 8; // [7:0]
        uint32_t fm_rxflt_cal_i_bit_d : 8; // [15:8]
        uint32_t fm_rxflt_cal_q_bit_c : 8; // [23:16]
        uint32_t fm_rxflt_cal_i_bit_c : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_FM_RXFLT_CAL_BIT_REG6_T;

// fm_rxflt_cal_bit_reg7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fm_rxflt_cal_q_bit_f : 8; // [7:0]
        uint32_t fm_rxflt_cal_i_bit_f : 8; // [15:8]
        uint32_t fm_rxflt_cal_q_bit_e : 8; // [23:16]
        uint32_t fm_rxflt_cal_i_bit_e : 8; // [31:24]
    } b;
} REG_WCN_RF_IF_FM_RXFLT_CAL_BIT_REG7_T;

// test_buf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pll_test_out_en : 1; // [0]
        uint32_t dac_out_en : 1;      // [1]
        uint32_t tx_if_en : 1;        // [2]
        uint32_t refclk_lvds_en : 1;  // [3]
        uint32_t __31_4 : 28;         // [31:4]
    } b;
} REG_WCN_RF_IF_TEST_BUF_T;

// ana_reserved
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bw_rsvd_in : 16;   // [15:0]
        uint32_t adpll_rsvd_in : 8; // [23:16]
        uint32_t bw_rsvd_out : 8;   // [31:24], read only
    } b;
} REG_WCN_RF_IF_ANA_RESERVED_T;

// dig_reserved
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dig_rsvd : 16; // [15:0]
        uint32_t __31_16 : 16;  // [31:16]
    } b;
} REG_WCN_RF_IF_DIG_RESERVED_T;

// new_add
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txrf_capbank_en_reg : 1; // [0]
        uint32_t txrf_capbank_en_dr : 1;  // [1]
        uint32_t pll_reg_digi_bit : 4;    // [5:2]
        uint32_t bt_dac_lp_mode2 : 2;     // [7:6]
        uint32_t __31_8 : 24;             // [31:8]
    } b;
} REG_WCN_RF_IF_NEW_ADD_T;

// revision
#define WCN_RF_IF_REVID(n) (((n)&0xf) << 0)

// sys_control
#define WCN_RF_IF_SOFT_RESETN (1 << 0)
#define WCN_RF_IF_CHIP_SELF_CAL_ENABLE (1 << 1)
#define WCN_RF_IF_RF_MODE(n) (((n)&0x3) << 2)
#define WCN_RF_IF_CLK_DIGITAL_ENABLE_REG (1 << 4)
#define WCN_RF_IF_BT_TUNE_DIFF_EN (1 << 5)
#define WCN_RF_IF_BT_HOPPING_EN (1 << 6)
#define WCN_RF_IF_FM_ADC_CLK_MODE (1 << 7)
#define WCN_RF_IF_BT_CH_CTRL_SRC_SEL (1 << 8)
#define WCN_RF_IF_BT_TX_TYPE (1 << 9)

// bt_control
#define WCN_RF_IF_BT_ARFCN(n) (((n)&0x7f) << 0)
#define WCN_RF_IF_BT_CH_TYPE (1 << 7)

// wf_control
#define WCN_RF_IF_WF_TUNE (1 << 0)
#define WCN_RF_IF_WF_CHN(n) (((n)&0xf) << 1)
#define WCN_RF_IF_WF_FREQ_MODE (1 << 5)
#define WCN_RF_IF_WF_FREQ_DIRECT(n) (((n)&0x1ffff) << 6)

// fm_control
#define WCN_RF_IF_FM_TUNE (1 << 0)
#define WCN_RF_IF_FM_CHAN_REG(n) (((n)&0x7ff) << 1)
#define WCN_RF_IF_FM_FREQ_MODE (1 << 12)
#define WCN_RF_IF_FM_BAND_SEL(n) (((n)&0x3) << 13)
#define WCN_RF_IF_FM_FREQ_DIRECT(n) (((n)&0xffff) << 16)

// inter_freq_setting
#define WCN_RF_IF_BT_IF(n) (((n)&0x7ff) << 0)
#define WCN_RF_IF_BT_IMGREJ (1 << 11)
#define WCN_RF_IF_BT_ZIF (1 << 12)
#define WCN_RF_IF_FM_IF(n) (((n)&0x7ff) << 16)
#define WCN_RF_IF_FM_IMGREJ (1 << 27)

// bandgap_setting
#define WCN_RF_IF_SEL_BG (1 << 0)

// lna_rmx_setting
#define WCN_RF_IF_FM_RMX_RSHORT_EN (1 << 0)
#define WCN_RF_IF_FM_RMX_REG_BIT(n) (((n)&0x7) << 1)
#define WCN_RF_IF_FM_RMX_LOBIAS_BIT(n) (((n)&0x3) << 4)
#define WCN_RF_IF_FM_RMX_HARM_REJ_EN (1 << 6)
#define WCN_RF_IF_FM_LNA_PORT_SEL(n) (((n)&0x3) << 7)
#define WCN_RF_IF_FM_LNA_REG_ICTRL (1 << 12)
#define WCN_RF_IF_BT_RMX_LO_VCOM_BIT(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_TIA_RIN_BIT(n) (((n)&0x3) << 15)
#define WCN_RF_IF_WF_TIA_RIN_BIT(n) (((n)&0x3) << 17)
#define WCN_RF_IF_BT_RMX_DISABLE (1 << 19)
#define WCN_RF_IF_BT_TIA_BYPASS (1 << 20)
#define WCN_RF_IF_BT_LNA_VCAS_BIT(n) (((n)&0x3) << 21)
#define WCN_RF_IF_BT_LNA_EN (1 << 23)
#define WCN_RF_IF_BT_LNA_INPUT_SHORT_TX (1 << 24)
#define WCN_RF_IF_BT_LNA_INPUT_SHORT_RX (1 << 25)

// pga_setting0
#define WCN_RF_IF_PGA_RS_BIT(n) (((n)&0x1f) << 0)
#define WCN_RF_IF_PGA_IQ_SW (1 << 5)
#define WCN_RF_IF_PGA_CF_BIT(n) (((n)&0x1f) << 6)
#define WCN_RF_IF_PGA_BW_TUN_BIT(n) (((n)&0x7) << 11)
#define WCN_RF_IF_PGA_BLK_MODE (1 << 14)
#define WCN_RF_IF_PGA_FM_EN_REG (1 << 15)
#define WCN_RF_IF_PGA_FM_EN_DR (1 << 16)

// pga_setting1
#define WCN_RF_IF_FM_PGA_IF_MODE(n) (((n)&0x3) << 0)
#define WCN_RF_IF_WF_PGA_IF_MODE(n) (((n)&0x3) << 2)
#define WCN_RF_IF_BT_PGA_IF_MODE(n) (((n)&0x3) << 4)
#define WCN_RF_IF_FM_PGA_IBIT(n) (((n)&0x3) << 6)
#define WCN_RF_IF_WF_PGA_IBIT(n) (((n)&0x3) << 8)
#define WCN_RF_IF_BT_PGA_IBIT(n) (((n)&0x3) << 10)
#define WCN_RF_IF_FM_PGA_BW_MODE(n) (((n)&0x3) << 12)
#define WCN_RF_IF_WF_PGA_BW_MODE(n) (((n)&0x3) << 14)
#define WCN_RF_IF_BT_PGA_BW_MODE_2M(n) (((n)&0x3) << 16)
#define WCN_RF_IF_BT_PGA_BW_MODE_1M(n) (((n)&0x3) << 18)
#define WCN_RF_IF_FM_PGA_GAIN_BIT(n) (((n)&0x7) << 20)

// rxflt_setting
#define WCN_RF_IF_FM_RXFLT_WIFI_HP (1 << 0)
#define WCN_RF_IF_WF_RXFLT_WIFI_HP (1 << 1)
#define WCN_RF_IF_BT_RXFLT_WIFI_HP (1 << 2)
#define WCN_RF_IF_FM_RXFLT_IF_MODE(n) (((n)&0x3) << 3)
#define WCN_RF_IF_WF_RXFLT_IF_MODE(n) (((n)&0x3) << 5)
#define WCN_RF_IF_BT_RXFLT_IF_MODE(n) (((n)&0x3) << 7)
#define WCN_RF_IF_FM_RXFLT_IBIT(n) (((n)&0x3) << 9)
#define WCN_RF_IF_WF_RXFLT_IBIT(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_RXFLT_IBIT(n) (((n)&0x3) << 13)
#define WCN_RF_IF_FM_RXFLT_MODE_SEL(n) (((n)&0x3) << 15)
#define WCN_RF_IF_WF_RXFLT_MODE_SEL(n) (((n)&0x3) << 17)
#define WCN_RF_IF_BT_RXFLT_MODE_SEL_2M(n) (((n)&0x3) << 19)
#define WCN_RF_IF_BT_RXFLT_MODE_SEL_1M(n) (((n)&0x3) << 21)
#define WCN_RF_IF_RXFLT_CAP_BIT(n) (((n)&0xf) << 23)
#define WCN_RF_IF_RXFLT_IQSWAP (1 << 27)
#define WCN_RF_IF_RXFLT_AUX_EN (1 << 28)
#define WCN_RF_IF_RXFLT_RSTN_REG (1 << 29)
#define WCN_RF_IF_RXFLT_RSTN_DR (1 << 30)

// adc_setting0
#define WCN_RF_IF_ADC_CLK_SEL_REG(n) (((n)&0x7) << 0)
#define WCN_RF_IF_ADC_CLK_SEL_DR (1 << 3)
#define WCN_RF_IF_ADC_CLK_DIV2 (1 << 4)
#define WCN_RF_IF_ADC_CLK_EDGE (1 << 5)
#define WCN_RF_IF_ADC_VCM_SEL(n) (((n)&0x3) << 6)
#define WCN_RF_IF_ADC_DLY_IN_CTRL (1 << 8)
#define WCN_RF_IF_ADC_DELAY_BIT(n) (((n)&0xf) << 9)

// adc_setting1
#define WCN_RF_IF_FM_ADC_REG_IBIT(n) (((n)&0x7) << 0)
#define WCN_RF_IF_WF_ADC_REG_IBIT(n) (((n)&0x7) << 3)
#define WCN_RF_IF_BT_ADC_REG_IBIT_2M(n) (((n)&0x7) << 6)
#define WCN_RF_IF_BT_ADC_REG_IBIT_1M(n) (((n)&0x7) << 9)
#define WCN_RF_IF_FM_ADC_REF_IBIT(n) (((n)&0x7) << 12)
#define WCN_RF_IF_WF_ADC_REF_IBIT(n) (((n)&0x7) << 15)
#define WCN_RF_IF_BT_ADC_REF_IBIT_2M(n) (((n)&0x7) << 18)
#define WCN_RF_IF_BT_ADC_REF_IBIT_1M(n) (((n)&0x7) << 21)

// bt_dac_setting
#define WCN_RF_IF_BT_DAC_RANGE_BIT(n) (((n)&0x3) << 0)
#define WCN_RF_IF_BT_DAC_MUX_EN (1 << 2)
#define WCN_RF_IF_BT_DAC_LPWR_MODE (1 << 3)
#define WCN_RF_IF_BT_DAC_IOUT_ENABLE (1 << 4)
#define WCN_RF_IF_BT_DAC_CORE_BIT(n) (((n)&0x7) << 5)
#define WCN_RF_IF_BT_DAC_CM_BIT(n) (((n)&0x3) << 8)
#define WCN_RF_IF_BT_DAC_AUXOUT_EN(n) (((n)&0x7) << 10)
#define WCN_RF_IF_BT_DAC_VTR_SEL (1 << 13)
#define WCN_RF_IF_BT_DAC_VLOW_CTRL_BIT(n) (((n)&0x7) << 14)
#define WCN_RF_IF_BT_DAC_CLK_EDGE (1 << 17)
#define WCN_RF_IF_BT_DAC_RSTN_REG (1 << 18)
#define WCN_RF_IF_BT_DAC_RSTN_DR (1 << 19)
#define WCN_RF_IF_BT_DAC_LOW_MAG (1 << 20)
#define WCN_RF_IF_BT_DAC_FILTER_IBIAS_BIT(n) (((n)&0x3) << 21)
#define WCN_RF_IF_BT_DAC_BW_TUNE_BIT_2M(n) (((n)&0xf) << 23)
#define WCN_RF_IF_BT_DAC_BW_TUNE_BIT_1M(n) (((n)&0xf) << 27)
#define WCN_RF_IF_BT_TMX_CAL_CLK_EDGE (1 << 31)

// bt_txrf_setting
#define WCN_RF_IF_TXRF_CAPTUNE_BIT_TX_HI(n) (((n)&0xf) << 0)
#define WCN_RF_IF_TXRF_CAPTUNE_BIT_TX_MD(n) (((n)&0xf) << 4)
#define WCN_RF_IF_TXRF_CAPTUNE_BIT_TX_LO(n) (((n)&0xf) << 8)
#define WCN_RF_IF_TXRF_CAPTUNE_BIT_RX_HI(n) (((n)&0xf) << 12)
#define WCN_RF_IF_TXRF_CAPTUNE_BIT_RX_MD(n) (((n)&0xf) << 16)
#define WCN_RF_IF_TXRF_CAPTUNE_BIT_RX_LO(n) (((n)&0xf) << 20)
#define WCN_RF_IF_BT_DELAY_PADRV(n) (((n)&0xff) << 24)

// fm_dsp_setting
#define WCN_RF_IF_FM_DSP_RESETN_REG (1 << 0)
#define WCN_RF_IF_FM_DSP_RESETN_DR (1 << 1)
#define WCN_RF_IF_FM_DSP_RESETN_DELAY(n) (((n)&0x3) << 2)

// vco_setting
#define WCN_RF_IF_FM_VCO_VMODE (1 << 0)
#define WCN_RF_IF_WF_VCO_VMODE (1 << 1)
#define WCN_RF_IF_BT_VCO_VMODE (1 << 2)
#define WCN_RF_IF_FM_VCO_IMODE (1 << 3)
#define WCN_RF_IF_WF_VCO_IMODE (1 << 4)
#define WCN_RF_IF_BT_VCO_IMODE (1 << 5)
#define WCN_RF_IF_VCO_VMODE_BIT(n) (((n)&0x7) << 6)
#define WCN_RF_IF_VCO_VAR_VCOM(n) (((n)&0x7) << 9)
#define WCN_RF_IF_VCO_VAR_SHORT (1 << 12)
#define WCN_RF_IF_VCO_VAR_REVERSE (1 << 13)
#define WCN_RF_IF_VCO_VAR_BIT(n) (((n)&0x7) << 14)
#define WCN_RF_IF_VCO_BUF_BIAS_BIT(n) (((n)&0x3) << 17)
#define WCN_RF_IF_VCO_DIV2_BIAS_BIT(n) (((n)&0x7) << 19)
#define WCN_RF_IF_VCO_BUF_IBIT(n) (((n)&0xf) << 22)
#define WCN_RF_IF_VCO_REGCAP_SELH (1 << 26)

// pll_setting0
#define WCN_RF_IF_PLL_DIV_FM_ADC_CLK_REG(n) (((n)&0x1f) << 0)
#define WCN_RF_IF_PLL_DIV_FM_ADC_CLK_DR (1 << 5)
#define WCN_RF_IF_PLL_DIV_FM_LO_CLK_REG(n) (((n)&0xf) << 6)
#define WCN_RF_IF_PLL_DIV_FM_LO_CLK_DR (1 << 10)
#define WCN_RF_IF_PLL_BYPASS_NOTCH (1 << 11)
#define WCN_RF_IF_PLL_XFER_AUX_EN (1 << 12)
#define WCN_RF_IF_PLL_SINC_MODE(n) (((n)&0x7) << 13)
#define WCN_RF_IF_PLL_FBC_SEL(n) (((n)&0x7) << 16)
#define WCN_RF_IF_PLL_OPEN_EN (1 << 19)
#define WCN_RF_IF_PLL_REG_PRESC_RC(n) (((n)&0x3) << 20)
#define WCN_RF_IF_PLL_REG_PRESC(n) (((n)&0xf) << 22)
#define WCN_RF_IF_PLL_REG_PERI_BIT(n) (((n)&0xf) << 26)
#define WCN_RF_IF_PLL_TEST_EN (1 << 30)

// pll_setting1
#define WCN_RF_IF_PLL_REFMULTI2_EN_TX (1 << 0)
#define WCN_RF_IF_PLL_REFMULTI2_EN_RX (1 << 1)
#define WCN_RF_IF_PLL_R_BIT_TX(n) (((n)&0x3) << 2)
#define WCN_RF_IF_PLL_R_BIT_RX(n) (((n)&0x3) << 4)
#define WCN_RF_IF_PLL_LPF_GAIN_TX(n) (((n)&0xf) << 6)
#define WCN_RF_IF_PLL_LPF_GAIN_RX(n) (((n)&0xf) << 10)
#define WCN_RF_IF_PLL_GAIN_BIT_TX(n) (((n)&0xf) << 14)
#define WCN_RF_IF_PLL_GAIN_BIT_RX(n) (((n)&0xf) << 18)
#define WCN_RF_IF_PLL_CP_R_BIT_TX(n) (((n)&0xf) << 22)
#define WCN_RF_IF_PLL_CP_R_BIT_RX(n) (((n)&0xf) << 26)
#define WCN_RF_IF_PLL_LOWPWR_MODE_TX (1 << 30)
#define WCN_RF_IF_PLL_LOWPWR_MODE_RX (1 << 31)

// pll_setting2
#define WCN_RF_IF_MDLL_DIV_TX(n) (((n)&0xf) << 0)
#define WCN_RF_IF_MDLL_DIV_RX(n) (((n)&0xf) << 4)
#define WCN_RF_IF_PLL_PHASE_CTRL_DLY_TX(n) (((n)&0x3) << 8)
#define WCN_RF_IF_PLL_PHASE_CTRL_DLY_RX(n) (((n)&0x3) << 10)
#define WCN_RF_IF_PLL_PFD_RES_BIT_TX(n) (((n)&0x3f) << 12)
#define WCN_RF_IF_PLL_PFD_RES_BIT_RX(n) (((n)&0x3f) << 18)
#define WCN_RF_IF_PLL_REF_MODE_TX (1 << 24)
#define WCN_RF_IF_PLL_REF_MODE_RX (1 << 25)
#define WCN_RF_IF_PLL_MDLL_REFCLK_EN_REG (1 << 26)
#define WCN_RF_IF_PLL_MDLL_REFCLK_EN_DR (1 << 27)
#define WCN_RF_IF_PLL_CLK_DIG52M_EN_REG (1 << 28)
#define WCN_RF_IF_PLL_CLK_DIG52M_EN_DR (1 << 29)
#define WCN_RF_IF_PLL_BT_ADC_CLK_EN_REG (1 << 30)
#define WCN_RF_IF_PLL_BT_ADC_CLK_EN_DR (1 << 31)

// pll_status
#define WCN_RF_IF_PLL_LOCK_DET (1 << 0)
#define WCN_RF_IF_PLL_LOCK_FLAG (1 << 1)
#define WCN_RF_IF_PLL_LOCK_DET_TIMER_DELAY_SEL(n) (((n)&0x3) << 2)
#define WCN_RF_IF_PLL_LOCK_FLAG_TIMER_DELAY_SEL(n) (((n)&0x3) << 4)

// adpll_setting0
#define WCN_RF_IF_ADPLL_SDM_CLK_TEST_EN (1 << 0)
#define WCN_RF_IF_ADPLL_VCO_LOW_TEST (1 << 1)
#define WCN_RF_IF_ADPLL_VCO_HIGH_TEST (1 << 2)
#define WCN_RF_IF_ADPLL_REFMULTI2_EN (1 << 3)
#define WCN_RF_IF_ADPLL_PCON_MODE (1 << 4)
#define WCN_RF_IF_ADPLL_VREG_BIT(n) (((n)&0xf) << 5)
#define WCN_RF_IF_ADPLL_TEST_EN (1 << 9)
#define WCN_RF_IF_ADPLL_RES_BIT(n) (((n)&0x3) << 10)
#define WCN_RF_IF_ADPLL_REG_RES_BIT(n) (((n)&0x3) << 12)
#define WCN_RF_IF_ADPLL_CP_IBIT(n) (((n)&0x7) << 14)
#define WCN_RF_IF_ADPLL_RSTN_REG (1 << 17)
#define WCN_RF_IF_ADPLL_RSTN_DR (1 << 18)
#define WCN_RF_IF_ADPLL_CLK_EN_TIMER_DELAY (1 << 19)
#define WCN_RF_IF_ADPLL_RSTN_TIMER_DELAY(n) (((n)&0x7f) << 20)
#define WCN_RF_IF_ADPLL_SDM_CLK_SEL_TIMER_DELAY(n) (((n)&0x7) << 27)

// adpll_setting1
#define WCN_RF_IF_ADPLL_CLK2WF_DIG_SEL_REG (1 << 0)
#define WCN_RF_IF_ADPLL_CLK2WF_DIG_SEL_DR (1 << 1)
#define WCN_RF_IF_ADPLL_CLK2WF_DIG_EN_REG (1 << 2)
#define WCN_RF_IF_ADPLL_CLK2WF_DIG_EN_DR (1 << 3)
#define WCN_RF_IF_ADPLL_CLK2FMWF_ADC_SEL_REG (1 << 4)
#define WCN_RF_IF_ADPLL_CLK2FMWF_ADC_SEL_DR (1 << 5)
#define WCN_RF_IF_ADPLL_CLK2FMWF_ADC_EN_REG (1 << 6)
#define WCN_RF_IF_ADPLL_CLK2FMWF_ADC_EN_DR (1 << 7)
#define WCN_RF_IF_ADPLL_CLK2BT_ADC_SEL_REG (1 << 8)
#define WCN_RF_IF_ADPLL_CLK2BT_ADC_SEL_DR (1 << 9)
#define WCN_RF_IF_ADPLL_CLK2BT_ADC_EN_REG (1 << 10)
#define WCN_RF_IF_ADPLL_CLK2BT_ADC_EN_DR (1 << 11)
#define WCN_RF_IF_ADPLL_CLK2BT_DIG_SEL (1 << 12)
#define WCN_RF_IF_ADPLL_CLK2BT_DIG_EN (1 << 13)

// adpll_status
#define WCN_RF_IF_ADPLL_LOCK_DET (1 << 0)
#define WCN_RF_IF_ADPLL_LOCK_FLAG (1 << 1)
#define WCN_RF_IF_ADPLL_LOCK_DET_TIMER_DELAY_SEL(n) (((n)&0x3) << 2)
#define WCN_RF_IF_ADPLL_LOCK_FLAG_TIMER_DELAY_SEL(n) (((n)&0x3) << 4)

// mdll_setting
#define WCN_RF_IF_MDLL_DIV_BIT(n) (((n)&0x1f) << 0)
#define WCN_RF_IF_MDLL_BAND_SEL (1 << 5)
#define WCN_RF_IF_MDLL_BAND_BIT(n) (((n)&0x7) << 6)
#define WCN_RF_IF_MDLL_CP_IBIT(n) (((n)&0x7) << 9)
#define WCN_RF_IF_MDLL_STARTUP_BIT(n) (((n)&0x7) << 12)
#define WCN_RF_IF_DISABLE_REFCLK_PLL (1 << 15)
#define WCN_RF_IF_MDLL_RSTN_REG (1 << 16)
#define WCN_RF_IF_MDLL_RSTN_DR (1 << 17)

// pll_sdm_setting0
#define WCN_RF_IF_PLL_SDM_DELAY_SEL(n) (((n)&0xf) << 0)
#define WCN_RF_IF_PLL_SDM_DITHER_BYPASS (1 << 4)
#define WCN_RF_IF_PLL_SDM_INT_DEC_SEL(n) (((n)&0x3) << 5)
#define WCN_RF_IF_PLL_SDM_CLK_EDGE (1 << 7)
#define WCN_RF_IF_PLL_SDM_RESETN_REG (1 << 8)
#define WCN_RF_IF_PLL_SDM_RESETN_DR (1 << 9)
#define WCN_RF_IF_RESET_PLL_SDM_TIMER_DELAY(n) (((n)&0xf) << 10)
#define WCN_RF_IF_PLL_SDM_CLK_SEL_1 (1 << 14)
#define WCN_RF_IF_PLL_SDM_CLK_SEL_0 (1 << 15)
#define WCN_RF_IF_PLL_DIV_DR (1 << 16)
#define WCN_RF_IF_PLL_FREQ_FORMER_BYPASS (1 << 17)
#define WCN_RF_IF_PLL_FREQ_FORMER_SHIFT_CT(n) (((n)&0x7) << 18)

// pll_sdm_setting1
#define WCN_RF_IF_PLL_DIV_REG(n) (((n)&0x7fffffff) << 0)

// adpll_sdm_setting0
#define WCN_RF_IF_ADPLL_SDM_DITHER_BYPASS (1 << 0)
#define WCN_RF_IF_ADPLL_SDM_INT_DEC_SEL(n) (((n)&0x7) << 1)
#define WCN_RF_IF_ADPLL_SDM_CLK_FBC_INV (1 << 4)
#define WCN_RF_IF_ADPLL_SDM_RESETN_REG (1 << 5)
#define WCN_RF_IF_ADPLL_SDM_RESETN_DR (1 << 6)
#define WCN_RF_IF_RESET_ADPLL_SDM_TIMER_DELAY(n) (((n)&0xf) << 7)
#define WCN_RF_IF_ADPLL_SDM_CLK_SEL_1 (1 << 11)
#define WCN_RF_IF_ADPLL_SDM_CLK_SEL_0 (1 << 12)
#define WCN_RF_IF_ADPLL_SDM_FREQ_DR (1 << 13)

// adpll_sdm_setting1
#define WCN_RF_IF_ADPLL_SDM_FREQ_REG(n) (((n)&0xffffffff) << 0)

// adpll_sdm_setting2
#define WCN_RF_IF_ADPLL_SDM_SS_DEVI_STEP(n) (((n)&0xffff) << 0)
#define WCN_RF_IF_ADPLL_SDM_SS_DEVI(n) (((n)&0xfff) << 16)
#define WCN_RF_IF_ADPLL_SDM_SS_EN (1 << 28)

// rxflt_cal_setting0
#define WCN_RF_IF_RXFLT_CAL_INIT_DELAY(n) (((n)&0xf) << 0)
#define WCN_RF_IF_RXFLT_CAL_CNT(n) (((n)&0xf) << 4)
#define WCN_RF_IF_RXFLT_CAL_IQSWAP (1 << 8)
#define WCN_RF_IF_RXFLT_CAL_POLARITY (1 << 9)
#define WCN_RF_IF_RXFLT_CAL_MODE_SEL(n) (((n)&0x3) << 10)
#define WCN_RF_IF_RXFLT_CAL_CLK_EDGE_SEL (1 << 12)
#define WCN_RF_IF_RXFLT_CAL_CLK_EDGE (1 << 13)
#define WCN_RF_IF_RXFLT_CAL_CYCLES(n) (((n)&0x3) << 14)
#define WCN_RF_IF_WF_RXFLT_CAL_LOOP_COEF(n) (((n)&0x7fff) << 16)

// rxflt_cal_setting1
#define WCN_RF_IF_RXFLT_CAL_Q_BIT_REG(n) (((n)&0xff) << 0)
#define WCN_RF_IF_RXFLT_CAL_I_BIT_REG(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_LOOP_ADC_RNG(n) (((n)&0x3ff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_LOOP_POL (1 << 26)
#define WCN_RF_IF_WF_RXFLT_CAL_LOOP_EN (1 << 27)
#define WCN_RF_IF_FM_RXFLT_CAL_RESOLV_ENHANCE (1 << 28)
#define WCN_RF_IF_WF_RXFLT_CAL_RESOLV_ENHANCE (1 << 29)
#define WCN_RF_IF_BT_RXFLT_CAL_RESOLV_ENHANCE (1 << 30)
#define WCN_RF_IF_RXFLT_CAL_BIT_DR (1 << 31)

// rxflt_cal_setting2
#define WCN_RF_IF_BT_CHN_MD(n) (((n)&0x7f) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_INTER_EN (1 << 7)
#define WCN_RF_IF_WF_RXFLT_CAL_INTER_COEF_HI(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_INTER_COEF_LO(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_CHN_MD(n) (((n)&0xf) << 24)
#define WCN_RF_IF_WF_RXFLT_CAL_INTER_EN (1 << 28)
#define WCN_RF_IF_RXFLT_CAL_RANGE_BIT(n) (((n)&0x3) << 29)

// rxflt_cal_setting3
#define WCN_RF_IF_BT_RXFLT_CAL_INTER_COEF_HI(n) (((n)&0x7fff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_INTER_COEF_LO(n) (((n)&0x7fff) << 16)

// vco_pkd_cal_setting
#define WCN_RF_IF_VCO_PKD_CAL_POLARITY (1 << 0)
#define WCN_RF_IF_VCO_PKD_CAL_INIT_DELAY(n) (((n)&0xf) << 1)
#define WCN_RF_IF_VCO_PKD_REF_BIT(n) (((n)&0x7) << 5)
#define WCN_RF_IF_VCO_VBIT_REG(n) (((n)&0xf) << 8)
#define WCN_RF_IF_VCO_VBIT_DR (1 << 12)
#define WCN_RF_IF_VCO_IBIT_REG(n) (((n)&0xf) << 13)
#define WCN_RF_IF_VCO_IBIT_DR (1 << 17)

// pll_cal_setting0
#define WCN_RF_IF_PLL_INIT_DELAY(n) (((n)&0x7) << 0)
#define WCN_RF_IF_PLL_CAL_CNT_SEL(n) (((n)&0x7) << 3)
#define WCN_RF_IF_PLL_CAL_OPT (1 << 6)
#define WCN_RF_IF_PLL_VCO_BIT_HOLD_TIME(n) (((n)&0x7) << 7)
#define WCN_RF_IF_PLL_CAL_FREQ_DR (1 << 10)
#define WCN_RF_IF_VCO_BAND_REG(n) (((n)&0x3ff) << 11)
#define WCN_RF_IF_VCO_BAND_DR (1 << 21)
#define WCN_RF_IF_PLL_CAL_BIT(n) (((n)&0x3) << 22)
#define WCN_RF_IF_PLL_CAL_CLK_SEL (1 << 24)

// pll_cal_setting1
#define WCN_RF_IF_PLL_CAL_FREQ_REG(n) (((n)&0xffff) << 0)

// adc_cal_setting
#define WCN_RF_IF_ADC_VREG_VBIT_REG(n) (((n)&0x7) << 0)
#define WCN_RF_IF_ADC_VREG_VBIT_DR (1 << 3)
#define WCN_RF_IF_ADC_REG_CAL_POLARITY (1 << 4)
#define WCN_RF_IF_ADC_REG_CAL_INIT_DELAY(n) (((n)&0xf) << 5)
#define WCN_RF_IF_ADC_VREF_VBIT_REG(n) (((n)&0x7) << 9)
#define WCN_RF_IF_ADC_VREF_VBIT_DR (1 << 12)
#define WCN_RF_IF_ADC_REF_CAL_POLARITY (1 << 13)
#define WCN_RF_IF_ADC_REF_CAL_INIT_DELAY(n) (((n)&0xf) << 14)
#define WCN_RF_IF_ADC_CAL_RSTN_REG (1 << 18)
#define WCN_RF_IF_ADC_CAL_RSTN_DR (1 << 19)

// cal_dr_setting
#define WCN_RF_IF_PLL_CAL_EN_REG (1 << 0)
#define WCN_RF_IF_PLL_CAL_EN_DR (1 << 1)
#define WCN_RF_IF_RXFLT_CAL_EN_REG (1 << 2)
#define WCN_RF_IF_RXFLT_CAL_EN_DR (1 << 3)
#define WCN_RF_IF_ADC_REG_CAL_EN_REG (1 << 4)
#define WCN_RF_IF_ADC_REG_CAL_EN_DR (1 << 5)
#define WCN_RF_IF_ADC_REF_CAL_EN_REG (1 << 6)
#define WCN_RF_IF_ADC_REF_CAL_EN_DR (1 << 7)
#define WCN_RF_IF_VCO_PKD_CAL_EN_REG (1 << 8)
#define WCN_RF_IF_VCO_PKD_CAL_EN_DR (1 << 9)

// cal_status
#define WCN_RF_IF_BT_SELF_CAL_READY (1 << 0)
#define WCN_RF_IF_WF_SELF_CAL_READY (1 << 1)
#define WCN_RF_IF_FM_SELF_CAL_READY (1 << 2)
#define WCN_RF_IF_PLL_CAL_READY (1 << 3)
#define WCN_RF_IF_RXFLT_CAL_READY (1 << 4)
#define WCN_RF_IF_ADC_REG_CAL_READY (1 << 5)
#define WCN_RF_IF_ADC_REF_CAL_READY (1 << 6)
#define WCN_RF_IF_VCO_PKD_CAL_READY (1 << 7)

// cal_results0
#define WCN_RF_IF_RXFLT_CAL_Q_BIT(n) (((n)&0xff) << 0)
#define WCN_RF_IF_RXFLT_CAL_I_BIT(n) (((n)&0xff) << 8)
#define WCN_RF_IF_ADC_VREG_VBIT(n) (((n)&0x7) << 16)
#define WCN_RF_IF_ADC_VREF_VBIT(n) (((n)&0x7) << 19)
#define WCN_RF_IF_VCO_VBIT(n) (((n)&0xf) << 22)
#define WCN_RF_IF_VCO_IBIT(n) (((n)&0xf) << 26)

// cal_results1
#define WCN_RF_IF_VCO_BAND(n) (((n)&0x3ff) << 0)
#define WCN_RF_IF_RXFLT_CAL_OUT_Q (1 << 10)
#define WCN_RF_IF_RXFLT_CAL_OUT_I (1 << 11)
#define WCN_RF_IF_ADC_CAL_REF_OUT (1 << 12)
#define WCN_RF_IF_ADC_CAL_REG_OUT (1 << 13)
#define WCN_RF_IF_VCO_PKD_CAL_OUT (1 << 14)

// power_dr
#define WCN_RF_IF_PU_MDLL_DR (1 << 0)
#define WCN_RF_IF_PU_ADPLL_DR (1 << 1)
#define WCN_RF_IF_PU_PLL_FM_ADC_CLK_DR (1 << 2)
#define WCN_RF_IF_PU_PLL_FM_LO_CLK_DR (1 << 3)
#define WCN_RF_IF_PU_PLL_PRESC_DR (1 << 4)
#define WCN_RF_IF_PU_PLL_PERI_DR (1 << 5)
#define WCN_RF_IF_PU_VCO_PKD_DR (1 << 6)
#define WCN_RF_IF_PU_VCO_RXLO_DR (1 << 7)
#define WCN_RF_IF_PU_VCO_TXLO_DR (1 << 8)
#define WCN_RF_IF_PU_VCO_DR (1 << 9)
#define WCN_RF_IF_PU_BT_TXRF_DR (1 << 10)
#define WCN_RF_IF_PU_BT_TMX_DR (1 << 11)
#define WCN_RF_IF_PU_BT_PADRV_DR (1 << 12)
#define WCN_RF_IF_PU_BT_DAC_DR (1 << 13)
#define WCN_RF_IF_PU_ADC_DR (1 << 14)
#define WCN_RF_IF_PU_RXFLT_DR (1 << 15)
#define WCN_RF_IF_PU_PGA_DR (1 << 16)
#define WCN_RF_IF_PU_BT_TIA_DR (1 << 17)
#define WCN_RF_IF_PU_BT_LNA_DR (1 << 18)
#define WCN_RF_IF_PU_FM_RMX_DR (1 << 19)
#define WCN_RF_IF_PU_FM_LNA_DR (1 << 20)
#define WCN_RF_IF_PU_BG_DR (1 << 21)

// power_reg
#define WCN_RF_IF_PU_MDLL_REG (1 << 0)
#define WCN_RF_IF_PU_ADPLL_REG (1 << 1)
#define WCN_RF_IF_PU_PLL_FM_ADC_CLK_REG (1 << 2)
#define WCN_RF_IF_PU_PLL_FM_LO_CLK_REG (1 << 3)
#define WCN_RF_IF_PU_PLL_PRESC_REG (1 << 4)
#define WCN_RF_IF_PU_PLL_PERI_REG (1 << 5)
#define WCN_RF_IF_PU_VCO_PKD_REG (1 << 6)
#define WCN_RF_IF_PU_VCO_RXLO_REG (1 << 7)
#define WCN_RF_IF_PU_VCO_TXLO_REG (1 << 8)
#define WCN_RF_IF_PU_VCO_REG (1 << 9)
#define WCN_RF_IF_PU_BT_TXRF_REG (1 << 10)
#define WCN_RF_IF_PU_BT_TMX_REG (1 << 11)
#define WCN_RF_IF_PU_BT_PADRV_REG (1 << 12)
#define WCN_RF_IF_PU_BT_DAC_REG (1 << 13)
#define WCN_RF_IF_PU_ADC_REG (1 << 14)
#define WCN_RF_IF_PU_RXFLT_REG (1 << 15)
#define WCN_RF_IF_PU_PGA_REG (1 << 16)
#define WCN_RF_IF_PU_BT_TIA_REG (1 << 17)
#define WCN_RF_IF_PU_BT_LNA_REG (1 << 18)
#define WCN_RF_IF_PU_FM_RMX_REG (1 << 19)
#define WCN_RF_IF_PU_FM_LNA_REG (1 << 20)
#define WCN_RF_IF_PU_BG_REG (1 << 21)

// power_status
#define WCN_RF_IF_PU_MDLL (1 << 0)
#define WCN_RF_IF_PU_ADPLL (1 << 1)
#define WCN_RF_IF_PU_PLL_FM_ADC_CLK (1 << 2)
#define WCN_RF_IF_PU_PLL_FM_LO_CLK (1 << 3)
#define WCN_RF_IF_PU_PLL_PRESC (1 << 4)
#define WCN_RF_IF_PU_PLL_PERI (1 << 5)
#define WCN_RF_IF_PU_VCO_PKD (1 << 6)
#define WCN_RF_IF_PU_VCO_RXLO (1 << 7)
#define WCN_RF_IF_PU_VCO_TXLO (1 << 8)
#define WCN_RF_IF_PU_VCO (1 << 9)
#define WCN_RF_IF_PU_BT_TXRF (1 << 10)
#define WCN_RF_IF_PU_BT_TMX (1 << 11)
#define WCN_RF_IF_PU_BT_PADRV (1 << 12)
#define WCN_RF_IF_PU_BT_DAC (1 << 13)
#define WCN_RF_IF_PU_ADC (1 << 14)
#define WCN_RF_IF_PU_RXFLT (1 << 15)
#define WCN_RF_IF_PU_PGA (1 << 16)
#define WCN_RF_IF_PU_BT_TIA (1 << 17)
#define WCN_RF_IF_PU_BT_LNA (1 << 18)
#define WCN_RF_IF_PU_FM_RMX (1 << 19)
#define WCN_RF_IF_PU_FM_LNA (1 << 20)
#define WCN_RF_IF_PU_BG (1 << 21)

// bt_gain_table_0
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_0(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_0(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_0(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_0(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_0(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_0(n) (((n)&0x3) << 15)

// bt_gain_table_1
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_1(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_1(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_1(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_1(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_1(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_1(n) (((n)&0x3) << 15)

// bt_gain_table_2
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_2(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_2(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_2(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_2(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_2(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_2(n) (((n)&0x3) << 15)

// bt_gain_table_3
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_3(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_3(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_3(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_3(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_3(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_3(n) (((n)&0x3) << 15)

// bt_gain_table_4
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_4(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_4(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_4(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_4(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_4(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_4(n) (((n)&0x3) << 15)

// bt_gain_table_5
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_5(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_5(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_5(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_5(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_5(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_5(n) (((n)&0x3) << 15)

// bt_gain_table_6
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_6(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_6(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_6(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_6(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_6(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_6(n) (((n)&0x3) << 15)

// bt_gain_table_7
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_7(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_7(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_7(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_7(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_7(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_7(n) (((n)&0x3) << 15)

// bt_gain_table_8
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_8(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_8(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_8(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_8(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_8(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_8(n) (((n)&0x3) << 15)

// bt_gain_table_9
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_9(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_9(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_9(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_9(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_9(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_9(n) (((n)&0x3) << 15)

// bt_gain_table_a
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_A(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_A(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_A(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_A(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_A(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_A(n) (((n)&0x3) << 15)

// bt_gain_table_b
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_B(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_B(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_B(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_B(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_B(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_B(n) (((n)&0x3) << 15)

// bt_gain_table_c
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_C(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_C(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_C(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_C(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_C(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_C(n) (((n)&0x3) << 15)

// bt_gain_table_d
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_D(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_D(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_D(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_D(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_D(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_D(n) (((n)&0x3) << 15)

// bt_gain_table_e
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_E(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_E(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_E(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_E(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_E(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_E(n) (((n)&0x3) << 15)

// bt_gain_table_f
#define WCN_RF_IF_BT_RXFLT_BT_GAIN_BIT_F(n) (((n)&0xf) << 0)
#define WCN_RF_IF_BT_PGA_GAIN_BIT_F(n) (((n)&0x7) << 4)
#define WCN_RF_IF_BT_LNA_RESF_BIT_F(n) (((n)&0xf) << 7)
#define WCN_RF_IF_BT_LNA_GAIN3_BIT_F(n) (((n)&0x3) << 11)
#define WCN_RF_IF_BT_LNA_GAIN2_BIT_F(n) (((n)&0x3) << 13)
#define WCN_RF_IF_BT_LNA_GAIN1_BIT_F(n) (((n)&0x3) << 15)

// wf_gain_table_0
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_0(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_0(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_0(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_0(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_0(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_0(n) (((n)&0x3) << 15)

// wf_gain_table_1
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_1(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_1(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_1(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_1(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_1(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_1(n) (((n)&0x3) << 15)

// wf_gain_table_2
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_2(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_2(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_2(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_2(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_2(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_2(n) (((n)&0x3) << 15)

// wf_gain_table_3
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_3(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_3(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_3(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_3(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_3(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_3(n) (((n)&0x3) << 15)

// wf_gain_table_4
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_4(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_4(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_4(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_4(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_4(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_4(n) (((n)&0x3) << 15)

// wf_gain_table_5
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_5(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_5(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_5(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_5(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_5(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_5(n) (((n)&0x3) << 15)

// wf_gain_table_6
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_6(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_6(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_6(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_6(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_6(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_6(n) (((n)&0x3) << 15)

// wf_gain_table_7
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_7(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_7(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_7(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_7(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_7(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_7(n) (((n)&0x3) << 15)

// wf_gain_table_8
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_8(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_8(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_8(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_8(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_8(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_8(n) (((n)&0x3) << 15)

// wf_gain_table_9
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_9(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_9(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_9(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_9(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_9(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_9(n) (((n)&0x3) << 15)

// wf_gain_table_a
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_A(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_A(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_A(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_A(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_A(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_A(n) (((n)&0x3) << 15)

// wf_gain_table_b
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_B(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_B(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_B(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_B(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_B(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_B(n) (((n)&0x3) << 15)

// wf_gain_table_c
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_C(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_C(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_C(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_C(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_C(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_C(n) (((n)&0x3) << 15)

// wf_gain_table_d
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_D(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_D(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_D(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_D(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_D(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_D(n) (((n)&0x3) << 15)

// wf_gain_table_e
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_E(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_E(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_E(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_E(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_E(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_E(n) (((n)&0x3) << 15)

// wf_gain_table_f
#define WCN_RF_IF_WF_RXFLT_BT_GAIN_BIT_F(n) (((n)&0xf) << 0)
#define WCN_RF_IF_WF_PGA_GAIN_BIT_F(n) (((n)&0x7) << 4)
#define WCN_RF_IF_WF_LNA_RESF_BIT_F(n) (((n)&0xf) << 7)
#define WCN_RF_IF_WF_LNA_GAIN3_BIT_F(n) (((n)&0x3) << 11)
#define WCN_RF_IF_WF_LNA_GAIN2_BIT_F(n) (((n)&0x3) << 13)
#define WCN_RF_IF_WF_LNA_GAIN1_BIT_F(n) (((n)&0x3) << 15)

// bt_tx_gain_table_0
#define WCN_RF_IF_TXRF_PAD_CAS_VBIT_1(n) (((n)&0x3) << 0)
#define WCN_RF_IF_TXRF_PAD_AUX_VBIT_1(n) (((n)&0x3) << 2)
#define WCN_RF_IF_TXRF_PAD_BIAS_IBIT_1(n) (((n)&0x3) << 4)
#define WCN_RF_IF_TXRF_ATT_EN_1(n) (((n)&0x3) << 6)
#define WCN_RF_IF_TXRF_GAIN_BIT_1(n) (((n)&0xf) << 8)
#define WCN_RF_IF_TXRF_PAD_MODE_1 (1 << 12)
#define WCN_RF_IF_TXRF_PAD_CAS_VBIT_0(n) (((n)&0x3) << 16)
#define WCN_RF_IF_TXRF_PAD_AUX_VBIT_0(n) (((n)&0x3) << 18)
#define WCN_RF_IF_TXRF_PAD_BIAS_IBIT_0(n) (((n)&0x3) << 20)
#define WCN_RF_IF_TXRF_ATT_EN_0(n) (((n)&0x3) << 22)
#define WCN_RF_IF_TXRF_GAIN_BIT_0(n) (((n)&0xf) << 24)
#define WCN_RF_IF_TXRF_PAD_MODE_0 (1 << 28)

// bt_tx_gain_table_1
#define WCN_RF_IF_TXRF_PAD_CAS_VBIT_3(n) (((n)&0x3) << 0)
#define WCN_RF_IF_TXRF_PAD_AUX_VBIT_3(n) (((n)&0x3) << 2)
#define WCN_RF_IF_TXRF_PAD_BIAS_IBIT_3(n) (((n)&0x3) << 4)
#define WCN_RF_IF_TXRF_ATT_EN_3(n) (((n)&0x3) << 6)
#define WCN_RF_IF_TXRF_GAIN_BIT_3(n) (((n)&0xf) << 8)
#define WCN_RF_IF_TXRF_PAD_MODE_3 (1 << 12)
#define WCN_RF_IF_TXRF_PAD_CAS_VBIT_2(n) (((n)&0x3) << 16)
#define WCN_RF_IF_TXRF_PAD_AUX_VBIT_2(n) (((n)&0x3) << 18)
#define WCN_RF_IF_TXRF_PAD_BIAS_IBIT_2(n) (((n)&0x3) << 20)
#define WCN_RF_IF_TXRF_ATT_EN_2(n) (((n)&0x3) << 22)
#define WCN_RF_IF_TXRF_GAIN_BIT_2(n) (((n)&0xf) << 24)
#define WCN_RF_IF_TXRF_PAD_MODE_2 (1 << 28)

// bt_tx_gain_table_2
#define WCN_RF_IF_TXRF_PAD_CAS_VBIT_5(n) (((n)&0x3) << 0)
#define WCN_RF_IF_TXRF_PAD_AUX_VBIT_5(n) (((n)&0x3) << 2)
#define WCN_RF_IF_TXRF_PAD_BIAS_IBIT_5(n) (((n)&0x3) << 4)
#define WCN_RF_IF_TXRF_ATT_EN_5(n) (((n)&0x3) << 6)
#define WCN_RF_IF_TXRF_GAIN_BIT_5(n) (((n)&0xf) << 8)
#define WCN_RF_IF_TXRF_PAD_MODE_5 (1 << 12)
#define WCN_RF_IF_TXRF_PAD_CAS_VBIT_4(n) (((n)&0x3) << 16)
#define WCN_RF_IF_TXRF_PAD_AUX_VBIT_4(n) (((n)&0x3) << 18)
#define WCN_RF_IF_TXRF_PAD_BIAS_IBIT_4(n) (((n)&0x3) << 20)
#define WCN_RF_IF_TXRF_ATT_EN_4(n) (((n)&0x3) << 22)
#define WCN_RF_IF_TXRF_GAIN_BIT_4(n) (((n)&0xf) << 24)
#define WCN_RF_IF_TXRF_PAD_MODE_4 (1 << 28)

// bt_tx_gain_table_3
#define WCN_RF_IF_TXRF_PAD_CAS_VBIT_7(n) (((n)&0x3) << 0)
#define WCN_RF_IF_TXRF_PAD_AUX_VBIT_7(n) (((n)&0x3) << 2)
#define WCN_RF_IF_TXRF_PAD_BIAS_IBIT_7(n) (((n)&0x3) << 4)
#define WCN_RF_IF_TXRF_ATT_EN_7(n) (((n)&0x3) << 6)
#define WCN_RF_IF_TXRF_GAIN_BIT_7(n) (((n)&0xf) << 8)
#define WCN_RF_IF_TXRF_PAD_MODE_7 (1 << 12)
#define WCN_RF_IF_TXRF_PAD_CAS_VBIT_6(n) (((n)&0x3) << 16)
#define WCN_RF_IF_TXRF_PAD_AUX_VBIT_6(n) (((n)&0x3) << 18)
#define WCN_RF_IF_TXRF_PAD_BIAS_IBIT_6(n) (((n)&0x3) << 20)
#define WCN_RF_IF_TXRF_ATT_EN_6(n) (((n)&0x3) << 22)
#define WCN_RF_IF_TXRF_GAIN_BIT_6(n) (((n)&0xf) << 24)
#define WCN_RF_IF_TXRF_PAD_MODE_6 (1 << 28)

// fm_gain_table_0
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_1(n) (((n)&0xf) << 0)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_1(n) (((n)&0x7) << 4)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_1(n) (((n)&0x3) << 7)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_1(n) (((n)&0x3) << 9)
#define WCN_RF_IF_FM_LNA_REG_IBIT_1(n) (((n)&0x7) << 11)
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_0(n) (((n)&0xf) << 16)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_0(n) (((n)&0x7) << 20)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_0(n) (((n)&0x3) << 23)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_0(n) (((n)&0x3) << 25)
#define WCN_RF_IF_FM_LNA_REG_IBIT_0(n) (((n)&0x7) << 27)

// fm_gain_table_1
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_3(n) (((n)&0xf) << 0)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_3(n) (((n)&0x7) << 4)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_3(n) (((n)&0x3) << 7)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_3(n) (((n)&0x3) << 9)
#define WCN_RF_IF_FM_LNA_REG_IBIT_3(n) (((n)&0x7) << 11)
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_2(n) (((n)&0xf) << 16)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_2(n) (((n)&0x7) << 20)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_2(n) (((n)&0x3) << 23)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_2(n) (((n)&0x3) << 25)
#define WCN_RF_IF_FM_LNA_REG_IBIT_2(n) (((n)&0x7) << 27)

// fm_gain_table_2
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_5(n) (((n)&0xf) << 0)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_5(n) (((n)&0x7) << 4)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_5(n) (((n)&0x3) << 7)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_5(n) (((n)&0x3) << 9)
#define WCN_RF_IF_FM_LNA_REG_IBIT_5(n) (((n)&0x7) << 11)
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_4(n) (((n)&0xf) << 16)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_4(n) (((n)&0x7) << 20)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_4(n) (((n)&0x3) << 23)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_4(n) (((n)&0x3) << 25)
#define WCN_RF_IF_FM_LNA_REG_IBIT_4(n) (((n)&0x7) << 27)

// fm_gain_table_3
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_7(n) (((n)&0xf) << 0)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_7(n) (((n)&0x7) << 4)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_7(n) (((n)&0x3) << 7)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_7(n) (((n)&0x3) << 9)
#define WCN_RF_IF_FM_LNA_REG_IBIT_7(n) (((n)&0x7) << 11)
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_6(n) (((n)&0xf) << 16)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_6(n) (((n)&0x7) << 20)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_6(n) (((n)&0x3) << 23)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_6(n) (((n)&0x3) << 25)
#define WCN_RF_IF_FM_LNA_REG_IBIT_6(n) (((n)&0x7) << 27)

// fm_gain_table_4
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_9(n) (((n)&0xf) << 0)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_9(n) (((n)&0x7) << 4)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_9(n) (((n)&0x3) << 7)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_9(n) (((n)&0x3) << 9)
#define WCN_RF_IF_FM_LNA_REG_IBIT_9(n) (((n)&0x7) << 11)
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_8(n) (((n)&0xf) << 16)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_8(n) (((n)&0x7) << 20)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_8(n) (((n)&0x3) << 23)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_8(n) (((n)&0x3) << 25)
#define WCN_RF_IF_FM_LNA_REG_IBIT_8(n) (((n)&0x7) << 27)

// fm_gain_table_5
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_B(n) (((n)&0xf) << 0)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_B(n) (((n)&0x7) << 4)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_B(n) (((n)&0x3) << 7)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_B(n) (((n)&0x3) << 9)
#define WCN_RF_IF_FM_LNA_REG_IBIT_B(n) (((n)&0x7) << 11)
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_A(n) (((n)&0xf) << 16)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_A(n) (((n)&0x7) << 20)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_A(n) (((n)&0x3) << 23)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_A(n) (((n)&0x3) << 25)
#define WCN_RF_IF_FM_LNA_REG_IBIT_A(n) (((n)&0x7) << 27)

// fm_gain_table_6
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_D(n) (((n)&0xf) << 0)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_D(n) (((n)&0x7) << 4)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_D(n) (((n)&0x3) << 7)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_D(n) (((n)&0x3) << 9)
#define WCN_RF_IF_FM_LNA_REG_IBIT_D(n) (((n)&0x7) << 11)
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_C(n) (((n)&0xf) << 16)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_C(n) (((n)&0x7) << 20)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_C(n) (((n)&0x3) << 23)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_C(n) (((n)&0x3) << 25)
#define WCN_RF_IF_FM_LNA_REG_IBIT_C(n) (((n)&0x7) << 27)

// fm_gain_table_7
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_F(n) (((n)&0xf) << 0)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_F(n) (((n)&0x7) << 4)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_F(n) (((n)&0x3) << 7)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_F(n) (((n)&0x3) << 9)
#define WCN_RF_IF_FM_LNA_REG_IBIT_F(n) (((n)&0x7) << 11)
#define WCN_RF_IF_FM_RXFLT_BT_GAIN_BIT_E(n) (((n)&0xf) << 16)
#define WCN_RF_IF_FM_RMX_GAIN_BIT_E(n) (((n)&0x7) << 20)
#define WCN_RF_IF_FM_LNA_RLOAD_BIT_E(n) (((n)&0x3) << 23)
#define WCN_RF_IF_FM_LNA_GAIN_BIT_E(n) (((n)&0x3) << 25)
#define WCN_RF_IF_FM_LNA_REG_IBIT_E(n) (((n)&0x7) << 27)

// direct_gain_setting
#define WCN_RF_IF_FM_AGC_GAIN_REG(n) (((n)&0xf) << 0)
#define WCN_RF_IF_FM_AGC_GAIN_DR (1 << 4)
#define WCN_RF_IF_WF_AGC_GAIN_REG(n) (((n)&0xf) << 8)
#define WCN_RF_IF_WF_AGC_GAIN_DR (1 << 12)
#define WCN_RF_IF_BT_AGC_GAIN_REG(n) (((n)&0xf) << 16)
#define WCN_RF_IF_BT_AGC_GAIN_DR (1 << 20)

// bt_rxflt_cal_bit_reg0
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_1(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_1(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_0(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_0(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_reg1
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_3(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_3(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_2(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_2(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_reg2
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_5(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_5(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_4(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_4(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_reg3
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_7(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_7(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_6(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_6(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_reg4
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_9(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_9(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_8(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_8(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_reg5
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_B(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_B(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_A(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_A(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_reg6
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_D(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_D(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_C(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_C(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_reg7
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_F(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_F(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_E(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_E(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_reg8
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_D_LO(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_D_LO(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_C_LO(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_C_LO(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_reg9
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_F_LO(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_F_LO(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_E_LO(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_E_LO(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_rega
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_D_HI(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_D_HI(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_C_HI(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_C_HI(n) (((n)&0xff) << 24)

// bt_rxflt_cal_bit_regb
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_F_HI(n) (((n)&0xff) << 0)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_F_HI(n) (((n)&0xff) << 8)
#define WCN_RF_IF_BT_RXFLT_CAL_Q_BIT_E_HI(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BT_RXFLT_CAL_I_BIT_E_HI(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_reg0
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_1(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_1(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_0(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_0(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_reg1
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_3(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_3(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_2(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_2(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_reg2
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_5(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_5(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_4(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_4(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_reg3
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_7(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_7(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_6(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_6(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_reg4
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_9(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_9(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_8(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_8(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_reg5
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_B(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_B(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_A(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_A(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_reg6
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_D(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_D(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_C(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_C(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_reg7
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_F(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_F(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_E(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_E(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_reg8
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_D_LO(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_D_LO(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_C_LO(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_C_LO(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_reg9
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_F_LO(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_F_LO(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_E_LO(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_E_LO(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_rega
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_D_HI(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_D_HI(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_C_HI(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_C_HI(n) (((n)&0xff) << 24)

// wf_rxflt_cal_bit_regb
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_F_HI(n) (((n)&0xff) << 0)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_F_HI(n) (((n)&0xff) << 8)
#define WCN_RF_IF_WF_RXFLT_CAL_Q_BIT_E_HI(n) (((n)&0xff) << 16)
#define WCN_RF_IF_WF_RXFLT_CAL_I_BIT_E_HI(n) (((n)&0xff) << 24)

// fm_rxflt_cal_bit_reg0
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_1(n) (((n)&0xff) << 0)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_1(n) (((n)&0xff) << 8)
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_0(n) (((n)&0xff) << 16)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_0(n) (((n)&0xff) << 24)

// fm_rxflt_cal_bit_reg1
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_3(n) (((n)&0xff) << 0)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_3(n) (((n)&0xff) << 8)
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_2(n) (((n)&0xff) << 16)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_2(n) (((n)&0xff) << 24)

// fm_rxflt_cal_bit_reg2
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_5(n) (((n)&0xff) << 0)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_5(n) (((n)&0xff) << 8)
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_4(n) (((n)&0xff) << 16)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_4(n) (((n)&0xff) << 24)

// fm_rxflt_cal_bit_reg3
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_7(n) (((n)&0xff) << 0)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_7(n) (((n)&0xff) << 8)
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_6(n) (((n)&0xff) << 16)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_6(n) (((n)&0xff) << 24)

// fm_rxflt_cal_bit_reg4
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_9(n) (((n)&0xff) << 0)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_9(n) (((n)&0xff) << 8)
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_8(n) (((n)&0xff) << 16)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_8(n) (((n)&0xff) << 24)

// fm_rxflt_cal_bit_reg5
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_B(n) (((n)&0xff) << 0)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_B(n) (((n)&0xff) << 8)
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_A(n) (((n)&0xff) << 16)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_A(n) (((n)&0xff) << 24)

// fm_rxflt_cal_bit_reg6
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_D(n) (((n)&0xff) << 0)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_D(n) (((n)&0xff) << 8)
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_C(n) (((n)&0xff) << 16)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_C(n) (((n)&0xff) << 24)

// fm_rxflt_cal_bit_reg7
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_F(n) (((n)&0xff) << 0)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_F(n) (((n)&0xff) << 8)
#define WCN_RF_IF_FM_RXFLT_CAL_Q_BIT_E(n) (((n)&0xff) << 16)
#define WCN_RF_IF_FM_RXFLT_CAL_I_BIT_E(n) (((n)&0xff) << 24)

// test_buf
#define WCN_RF_IF_PLL_TEST_OUT_EN (1 << 0)
#define WCN_RF_IF_DAC_OUT_EN (1 << 1)
#define WCN_RF_IF_TX_IF_EN (1 << 2)
#define WCN_RF_IF_REFCLK_LVDS_EN (1 << 3)

// ana_reserved
#define WCN_RF_IF_BW_RSVD_IN(n) (((n)&0xffff) << 0)
#define WCN_RF_IF_ADPLL_RSVD_IN(n) (((n)&0xff) << 16)
#define WCN_RF_IF_BW_RSVD_OUT(n) (((n)&0xff) << 24)

// dig_reserved
#define WCN_RF_IF_DIG_RSVD(n) (((n)&0xffff) << 0)

// new_add
#define WCN_RF_IF_TXRF_CAPBANK_EN_REG (1 << 0)
#define WCN_RF_IF_TXRF_CAPBANK_EN_DR (1 << 1)
#define WCN_RF_IF_PLL_REG_DIGI_BIT(n) (((n)&0xf) << 2)
#define WCN_RF_IF_BT_DAC_LP_MODE2(n) (((n)&0x3) << 6)

#endif // _WCN_RF_IF_H_
