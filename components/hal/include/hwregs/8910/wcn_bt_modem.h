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

#ifndef _WCN_BT_MODEM_H_
#define _WCN_BT_MODEM_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_WCN_MODEM_BASE (0x15006000)

typedef volatile struct
{
    uint32_t bypass_ctrl;            // 0x00000000
    uint32_t sw_swap_dccal;          // 0x00000004
    uint32_t dem750_afc_freq;        // 0x00000008
    uint32_t dpsk_gfsk_tx_ctrl;      // 0x0000000c
    uint32_t tx_gain_ctrl;           // 0x00000010
    uint32_t rssi_out1;              // 0x00000014
    uint32_t sw_ctrl;                // 0x00000018
    uint32_t adcclk_sw_ctrl;         // 0x0000001c
    uint32_t dpsk_demod_sw;          // 0x00000020
    uint32_t min_phase_err;          // 0x00000024
    uint32_t afc_ctrl;               // 0x00000028
    uint32_t dpsk_gfsk_smp_th;       // 0x0000002c
    uint32_t dpsk_gfsk_smp_th_1;     // 0x00000030
    uint32_t gfsk_smp_ref_ctrl;      // 0x00000034
    uint32_t gfsk_smp_ref_ctrl_1;    // 0x00000038
    uint32_t rateconv_ctrl1;         // 0x0000003c
    uint32_t rateconv_ctrl2;         // 0x00000040
    uint32_t demod_smp_ctrl;         // 0x00000044
    uint32_t agc_ctrl;               // 0x00000048
    uint32_t agc_th_ctrl1;           // 0x0000004c
    uint32_t agc_th_ctrl2;           // 0x00000050
    uint32_t agc_ctrl1;              // 0x00000054
    uint32_t agc_ctrl2;              // 0x00000058
    uint32_t agc_dgc_ctrl;           // 0x0000005c
    uint32_t agc_dccal_ctrl;         // 0x00000060
    uint32_t rx_dccal_ctrl;          // 0x00000064
    uint32_t rx_dcofst_inuse;        // 0x00000068
    uint32_t tx_dc_calib;            // 0x0000006c
    uint32_t tx_fix_i_dcofst;        // 0x00000070
    uint32_t tx_fix_q_dcofst;        // 0x00000074
    uint32_t tx_i_dcofst_inuse;      // 0x00000078
    uint32_t tx_q_dcofst_inuse;      // 0x0000007c
    uint32_t rssi_gain_ctrl1;        // 0x00000080
    uint32_t rssi_gain_ctrl2;        // 0x00000084
    uint32_t rssi_gain_ctrl3;        // 0x00000088
    uint32_t rssi_gain_ctrl4;        // 0x0000008c
    uint32_t rssi_gain_ctrl5;        // 0x00000090
    uint32_t rssi_gain_ctrl6;        // 0x00000094
    uint32_t rssi_gain_ctrl7;        // 0x00000098
    uint32_t rssi_gain_ctrl8;        // 0x0000009c
    uint32_t modem_tpd_sel;          // 0x000000a0
    uint32_t demod_smp_th_ctrl;      // 0x000000a4
    uint32_t newpacket_byte4;        // 0x000000a8
    uint32_t newpacket_byte3;        // 0x000000ac
    uint32_t newpacket_byte2;        // 0x000000b0
    uint32_t newpacket_byte1;        // 0x000000b4
    uint32_t gfsk_mod_ctrl;          // 0x000000b8
    uint32_t gfsk_mod_ref_ctrl;      // 0x000000bc
    uint32_t sym_dly_ctrl;           // 0x000000c0
    uint32_t rssi_out2;              // 0x000000c4
    uint32_t trx_clk_ctrl;           // 0x000000c8
    uint32_t iq_swap_ctrl;           // 0x000000cc
    uint32_t gfsk_sync_ctrl;         // 0x000000d0
    uint32_t gfsk_demod_ctrl;        // 0x000000d4
    uint32_t gfsk_mod_idx;           // 0x000000d8
    uint32_t dpsk_gfsk_misc_ctrl;    // 0x000000dc
    uint32_t modem_dbm_sel;          // 0x000000e0
    uint32_t gfsk_mod_idx_le;        // 0x000000e4
    uint32_t newpacket_byte4_inuse;  // 0x000000e8
    uint32_t newpacket_byte3_inuse;  // 0x000000ec
    uint32_t newpacket_byte2_inuse;  // 0x000000f0
    uint32_t newpacket_byte1_inuse;  // 0x000000f4
    uint32_t le_mode_ctrl1;          // 0x000000f8
    uint32_t le_mode_ctrl2;          // 0x000000fc
    uint32_t le_mode_ctrl3;          // 0x00000100
    uint32_t le_mode_ctrl4;          // 0x00000104
    uint32_t le_mode_ctrl5;          // 0x00000108
    uint32_t rf_ctrl;                // 0x0000010c
    uint32_t tx_q_im;                // 0x00000110
    uint32_t tx_i_im;                // 0x00000114
    uint32_t tx_am_p0;               // 0x00000118
    uint32_t tx_am_p1;               // 0x0000011c
    uint32_t tx_am_p2;               // 0x00000120
    uint32_t tx_am_p3;               // 0x00000124
    uint32_t tx_am_p4;               // 0x00000128
    uint32_t tx_am_p5;               // 0x0000012c
    uint32_t tx_am_p6;               // 0x00000130
    uint32_t tx_am_p7;               // 0x00000134
    uint32_t tx_am_p8;               // 0x00000138
    uint32_t tx_am_p9;               // 0x0000013c
    uint32_t tx_am_p10;              // 0x00000140
    uint32_t tx_am_p11;              // 0x00000144
    uint32_t tx_am_p12;              // 0x00000148
    uint32_t tx_am_p13;              // 0x0000014c
    uint32_t tx_am_p14;              // 0x00000150
    uint32_t tx_am_p15;              // 0x00000154
    uint32_t tx_am_p16;              // 0x00000158
    uint32_t tx_pm_p0;               // 0x0000015c
    uint32_t tx_pm_p1;               // 0x00000160
    uint32_t tx_pm_p2;               // 0x00000164
    uint32_t tx_pm_p3;               // 0x00000168
    uint32_t tx_pm_p4;               // 0x0000016c
    uint32_t tx_pm_p5;               // 0x00000170
    uint32_t tx_pm_p6;               // 0x00000174
    uint32_t tx_pm_p7;               // 0x00000178
    uint32_t tx_pm_p8;               // 0x0000017c
    uint32_t tx_pm_p9;               // 0x00000180
    uint32_t tx_pm_p10;              // 0x00000184
    uint32_t tx_pm_p11;              // 0x00000188
    uint32_t tx_pm_p12;              // 0x0000018c
    uint32_t tx_pm_p13;              // 0x00000190
    uint32_t tx_pm_p14;              // 0x00000194
    uint32_t tx_pm_p15;              // 0x00000198
    uint32_t tx_pm_p16;              // 0x0000019c
    uint32_t notch_coef;             // 0x000001a0
    uint32_t adapt_edr3_demod;       // 0x000001a4
    uint32_t adapt_edr3_thresh;      // 0x000001a8
    uint32_t tx_auto_gain1_gfsk;     // 0x000001ac
    uint32_t tx_auto_gain0_gfsk;     // 0x000001b0
    uint32_t tx_auto_gain1_gfsk_edr; // 0x000001b4
    uint32_t tx_auto_gain0_gfsk_edr; // 0x000001b8
    uint32_t tx_auto_gain1_dpsk;     // 0x000001bc
    uint32_t tx_auto_gain0_dpsk;     // 0x000001c0
    uint32_t gfsk_mod_eql_gain;      // 0x000001c4
    uint32_t tx_lfp_delay_ctrl;      // 0x000001c8
    uint32_t tx_hfp_delay;           // 0x000001cc
    uint32_t tx_polar_mode_ctl;      // 0x000001d0
    uint32_t tx_apf_num_b1;          // 0x000001d4
    uint32_t tx_apf_num_b2;          // 0x000001d8
    uint32_t tx_apf_num_b3;          // 0x000001dc
    uint32_t tx_apf_num_b4;          // 0x000001e0
    uint32_t tx_apf_den_a2;          // 0x000001e4
    uint32_t tx_apf_den_a3;          // 0x000001e8
    uint32_t tx_apf_den_a4;          // 0x000001ec
    uint32_t adapt_edr3_cci;         // 0x000001f0
} HWP_WCN_BT_MODEM_T;

#define hwp_wcnModem ((HWP_WCN_BT_MODEM_T *)REG_ACCESS_ADDRESS(REG_WCN_MODEM_BASE))

// bypass_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;             // [0]
        uint32_t bypass_smppt_dpsk : 1; // [1]
        uint32_t bypass_patch_dpsk : 1; // [2]
        uint32_t bypass_derr2_dpsk : 1; // [3]
        uint32_t bypass_derr1_dpsk : 1; // [4]
        uint32_t bypass_smppt_gfsk : 1; // [5]
        uint32_t bypass_patch_gfsk : 1; // [6]
        uint32_t bypass_derr2_gfsk : 1; // [7]
        uint32_t bypass_derr1_gfsk : 1; // [8]
        uint32_t __11_9 : 3;            // [11:9]
        uint32_t bypass_srrc : 1;       // [12]
        uint32_t bypass_mixer : 1;      // [13]
        uint32_t bypass_dccancel2 : 1;  // [14]
        uint32_t bypass_dccancel : 1;   // [15]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_WCN_BT_MODEM_BYPASS_CTRL_T;

// sw_swap_dccal
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sel_sumerr_range : 3; // [2:0]
        uint32_t __4_3 : 2;            // [4:3]
        uint32_t iq_sel_pol : 1;       // [5]
        uint32_t swch_sign_rx : 1;     // [6]
        uint32_t swch_sign_q_tx : 1;   // [7]
        uint32_t swch_sign_i_tx : 1;   // [8]
        uint32_t lpf_dwidth_sel : 2;   // [10:9]
        uint32_t rsvd0 : 1;            // [11]
        uint32_t swch_clk_dac : 1;     // [12]
        uint32_t sel_sync : 1;         // [13]
        uint32_t swch_clk_adc : 1;     // [14]
        uint32_t __31_15 : 17;         // [31:15]
    } b;
} REG_WCN_BT_MODEM_SW_SWAP_DCCAL_T;

// dem750_afc_freq
typedef union {
    uint32_t v;
    struct
    {
        uint32_t afc_smtif : 16; // [15:0]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_WCN_BT_MODEM_DEM750_AFC_FREQ_T;

// dpsk_gfsk_tx_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay_dpsk : 4;   // [3:0]
        uint32_t delay_gfsk : 4;   // [7:4]
        uint32_t tx_gain_dpsk : 8; // [15:8]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_WCN_BT_MODEM_DPSK_GFSK_TX_CTRL_T;

// tx_gain_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_gain_gfsk : 8;        // [7:0]
        uint32_t tx_auto_gain_bypass : 1; // [8]
        uint32_t tx_power_gain_sel : 1;   // [9]
        uint32_t cnt_guard_ini : 6;       // [15:10]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_WCN_BT_MODEM_TX_GAIN_CTRL_T;

// rssi_out1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_out_pre : 10;  // [9:0], read only
        uint32_t __15_10 : 6;        // [15:10]
        uint32_t rssi_out_post : 10; // [25:16], read only
        uint32_t __31_26 : 6;        // [31:26]
    } b;
} REG_WCN_BT_MODEM_RSSI_OUT1_T;

// sw_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_gain_gfsk_edr : 8;      // [7:0]
        uint32_t notch_disb : 1;            // [8]
        uint32_t swap_pn_q_dccl_0 : 1;      // [9]
        uint32_t swap_pn_i_dccl_0 : 1;      // [10]
        uint32_t swap_iq_mixer_0 : 1;       // [11]
        uint32_t swap_iq_smtif_0 : 1;       // [12]
        uint32_t swap_iq_dccl_0 : 1;        // [13]
        uint32_t swap_iq : 1;               // [14]
        uint32_t rssi_lock_by_agc_pre : 1;  // [15]
        uint32_t rssi_lock_by_agc_post : 1; // [16]
        uint32_t __31_17 : 15;              // [31:17]
    } b;
} REG_WCN_BT_MODEM_SW_CTRL_T;

// adcclk_sw_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ct_u_sp_gfsk : 4; // [3:0]
        uint32_t ct_u_1_gfsk : 5;  // [8:4]
        uint32_t ct_u_gfsk : 4;    // [12:9]
        uint32_t pckt_sel : 1;     // [13]
        uint32_t en_bbpkg_flg : 1; // [14]
        uint32_t __31_15 : 17;     // [31:15]
    } b;
} REG_WCN_BT_MODEM_ADCCLK_SW_CTRL_T;

// dpsk_demod_sw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ct_u_sp_dpsk : 4; // [3:0]
        uint32_t ct_u_1_dpsk : 5;  // [8:4]
        uint32_t ct_u_dpsk : 4;    // [12:9]
        uint32_t ct_u_patch : 3;   // [15:13]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_WCN_BT_MODEM_DPSK_DEMOD_SW_T;

// min_phase_err
typedef union {
    uint32_t v;
    struct
    {
        uint32_t min_error_th : 16; // [15:0]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_WCN_BT_MODEM_MIN_PHASE_ERR_T;

// afc_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t th_gfsk_dem : 10;     // [9:0]
        uint32_t rsvd1 : 2;            // [11:10]
        uint32_t sel_afc_dpskseek : 2; // [13:12]
        uint32_t sel_afc_gfskdem : 2;  // [15:14]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_WCN_BT_MODEM_AFC_CTRL_T;

// dpsk_gfsk_smp_th
typedef union {
    uint32_t v;
    struct
    {
        uint32_t th_gfsk_dem2 : 10;    // [9:0]
        uint32_t sp_th_sel_gfsk : 2;   // [11:10]
        uint32_t sp_th_sel_dpsk : 2;   // [13:12]
        uint32_t diff_enable_gfsk : 1; // [14]
        uint32_t diff_enable_dpsk : 1; // [15]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_WCN_BT_MODEM_DPSK_GFSK_SMP_TH_T;

// dpsk_gfsk_smp_th_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ref_a2_sek : 10; // [9:0]
        uint32_t __31_10 : 22;    // [31:10]
    } b;
} REG_WCN_BT_MODEM_DPSK_GFSK_SMP_TH_1_T;

// gfsk_smp_ref_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ref_a3_sek : 8; // [7:0]
        uint32_t ref_a1_sek : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_WCN_BT_MODEM_GFSK_SMP_REF_CTRL_T;

// gfsk_smp_ref_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dc_ct2 : 9;          // [8:0]
        uint32_t guard_time_conf : 5; // [13:9]
        uint32_t rsvd2 : 2;           // [15:14]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_WCN_BT_MODEM_GFSK_SMP_REF_CTRL_1_T;

// rateconv_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dac_data_sel : 6; // [5:0]
        uint32_t dac_test_en : 1;  // [6]
        uint32_t dc_ct : 9;        // [15:7]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_WCN_BT_MODEM_RATECONV_CTRL1_T;

// rateconv_ctrl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_tm_th_pre : 6;    // [5:0]
        uint32_t dc_hold_edr_en : 1;    // [6]
        uint32_t rssi_tm_th_post : 6;   // [12:7]
        uint32_t rssi_sel : 2;          // [14:13]
        uint32_t sel_rssi_src_pre : 1;  // [15]
        uint32_t sel_rssi_src_post : 1; // [16]
        uint32_t __31_17 : 15;          // [31:17]
    } b;
} REG_WCN_BT_MODEM_RATECONV_CTRL2_T;

// demod_smp_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cnt_sample_ini : 4; // [3:0]
        uint32_t ref_ready_th : 6;   // [9:4]
        uint32_t ini_rst_th : 6;     // [15:10]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_WCN_BT_MODEM_DEMOD_SMP_CTRL_T;

// agc_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t agc_sinc_over_th : 2; // [1:0]
        uint32_t agc_sinc_over_en : 1; // [2]
        uint32_t agc_mod_fix7 : 1;     // [3]
        uint32_t agc_en_fix7 : 1;      // [4]
        uint32_t dpsk_seek_st_cnt : 5; // [9:5]
        uint32_t mix_guard_th : 6;     // [15:10]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_WCN_BT_MODEM_AGC_CTRL_T;

// agc_th_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t agc_th_min : 8; // [7:0]
        uint32_t agc_th_max : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_WCN_BT_MODEM_AGC_TH_CTRL1_T;

// agc_th_ctrl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t agc_th_min_lg : 8; // [7:0]
        uint32_t agc_th_max_lg : 8; // [15:8]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_WCN_BT_MODEM_AGC_TH_CTRL2_T;

// agc_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t agc_mod_dem : 2;       // [1:0]
        uint32_t agc_index_ini_dsp : 4; // [5:2]
        uint32_t agc_en_dly : 3;        // [8:6]
        uint32_t agc_step_over : 2;     // [10:9]
        uint32_t agc_step_mode : 2;     // [12:11]
        uint32_t agc_en_lg_stp : 1;     // [13]
        uint32_t agc_ct_th_min : 1;     // [14]
        uint32_t __31_15 : 17;          // [31:15]
    } b;
} REG_WCN_BT_MODEM_AGC_CTRL1_T;

// agc_ctrl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t agc_tm_intv_lg : 7; // [6:0]
        uint32_t agc_tm_intv : 7;    // [13:7]
        uint32_t agc_th_min_gn : 2;  // [15:14]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_WCN_BT_MODEM_AGC_CTRL2_T;

// agc_dgc_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t agc_mode_dsp : 3;          // [2:0]
        uint32_t __5_3 : 3;                 // [5:3]
        uint32_t newpacket_zero_wd_cnt : 2; // [7:6]
        uint32_t pass_newpacket_sel : 1;    // [8]
        uint32_t dgc_index_max : 3;         // [11:9]
        uint32_t dgc_index_dsp : 3;         // [14:12]
        uint32_t sel_reg_agc : 1;           // [15]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_WCN_BT_MODEM_AGC_DGC_CTRL_T;

// agc_dccal_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_fix_dcofst : 1;  // [0]
        uint32_t dc_cal_rx_dly : 2;  // [2:1]
        uint32_t rx_dc_cal_done : 1; // [3], read only
        uint32_t __5_4 : 2;          // [5:4]
        uint32_t agc_tm_hold : 5;    // [10:6]
        uint32_t agc_tm_wait : 5;    // [15:11]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_WCN_BT_MODEM_AGC_DCCAL_CTRL_T;

// rx_dccal_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_dc_cali_q_fix : 10; // [9:0]
        uint32_t __15_10 : 6;           // [15:10]
        uint32_t rx_dc_cali_i_fix : 10; // [25:16]
        uint32_t __31_26 : 6;           // [31:26]
    } b;
} REG_WCN_BT_MODEM_RX_DCCAL_CTRL_T;

// rx_dcofst_inuse
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_dcoffset_q : 10; // [9:0], read only
        uint32_t __15_10 : 6;        // [15:10]
        uint32_t rx_dcoffset_i : 10; // [25:16], read only
        uint32_t __31_26 : 6;        // [31:26]
    } b;
} REG_WCN_BT_MODEM_RX_DCOFST_INUSE_T;

// tx_dc_calib
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bypass_tx_cal : 1;  // [0]
        uint32_t tx_apc : 3;         // [3:1]
        uint32_t tx_cal_shift : 2;   // [5:4]
        uint32_t tx_cal_sel : 2;     // [7:6]
        uint32_t tx_cal_pol : 1;     // [8]
        uint32_t tx_cal_cmp_pol : 1; // [9]
        uint32_t tx_cal_cnt : 2;     // [11:10]
        uint32_t tx_fix_dcofst : 1;  // [12]
        uint32_t tx_cal_out_q : 1;   // [13], read only
        uint32_t tx_cal_out_i : 1;   // [14], read only
        uint32_t tx_dc_cal_done : 1; // [15], read only
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_WCN_BT_MODEM_TX_DC_CALIB_T;

// tx_fix_i_dcofst
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_dc_idata_fix : 12; // [11:0]
        uint32_t __31_12 : 20;         // [31:12]
    } b;
} REG_WCN_BT_MODEM_TX_FIX_I_DCOFST_T;

// tx_fix_q_dcofst
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_dc_qdata_fix : 12; // [11:0]
        uint32_t __31_12 : 20;         // [31:12]
    } b;
} REG_WCN_BT_MODEM_TX_FIX_Q_DCOFST_T;

// tx_i_dcofst_inuse
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dc_cal_tx_idata : 12; // [11:0], read only
        uint32_t __31_12 : 20;         // [31:12]
    } b;
} REG_WCN_BT_MODEM_TX_I_DCOFST_INUSE_T;

// tx_q_dcofst_inuse
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dc_cal_tx_qdata : 12; // [11:0], read only
        uint32_t __31_12 : 20;         // [31:12]
    } b;
} REG_WCN_BT_MODEM_TX_Q_DCOFST_INUSE_T;

// rssi_gain_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_ana_gain0001 : 8; // [7:0]
        uint32_t rssi_ana_gain0000 : 8; // [15:8]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_WCN_BT_MODEM_RSSI_GAIN_CTRL1_T;

// rssi_gain_ctrl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_ana_gain0011 : 8; // [7:0]
        uint32_t rssi_ana_gain0010 : 8; // [15:8]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_WCN_BT_MODEM_RSSI_GAIN_CTRL2_T;

// rssi_gain_ctrl3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_ana_gain0101 : 8; // [7:0]
        uint32_t rssi_ana_gain0100 : 8; // [15:8]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_WCN_BT_MODEM_RSSI_GAIN_CTRL3_T;

// rssi_gain_ctrl4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_ana_gain0111 : 8; // [7:0]
        uint32_t rssi_ana_gain0110 : 8; // [15:8]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_WCN_BT_MODEM_RSSI_GAIN_CTRL4_T;

// rssi_gain_ctrl5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_ana_gain1001 : 8; // [7:0]
        uint32_t rssi_ana_gain1000 : 8; // [15:8]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_WCN_BT_MODEM_RSSI_GAIN_CTRL5_T;

// rssi_gain_ctrl6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_ana_gain1011 : 8; // [7:0]
        uint32_t rssi_ana_gain1010 : 8; // [15:8]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_WCN_BT_MODEM_RSSI_GAIN_CTRL6_T;

// rssi_gain_ctrl7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_ana_gain1101 : 8; // [7:0]
        uint32_t rssi_ana_gain1100 : 8; // [15:8]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_WCN_BT_MODEM_RSSI_GAIN_CTRL7_T;

// rssi_gain_ctrl8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_ana_gain1111 : 8; // [7:0]
        uint32_t rssi_ana_gain1110 : 8; // [15:8]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_WCN_BT_MODEM_RSSI_GAIN_CTRL8_T;

// modem_tpd_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tpd_trig_sel : 4;     // [3:0]
        uint32_t tpd_data_sel : 4;     // [7:4]
        uint32_t tpd_clk_sel : 1;      // [8]
        uint32_t dac_clk_force_en : 1; // [9]
        uint32_t __31_10 : 22;         // [31:10]
    } b;
} REG_WCN_BT_MODEM_MODEM_TPD_SEL_T;

// demod_smp_th_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dem_sp_th1 : 8; // [7:0]
        uint32_t dem_sp_th2 : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_WCN_BT_MODEM_DEMOD_SMP_TH_CTRL_T;

// newpacket_byte4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t newpacket_4 : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_WCN_BT_MODEM_NEWPACKET_BYTE4_T;

// newpacket_byte3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t newpacket_3 : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_WCN_BT_MODEM_NEWPACKET_BYTE3_T;

// newpacket_byte2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t newpacket_2 : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_WCN_BT_MODEM_NEWPACKET_BYTE2_T;

// newpacket_byte1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t newpacket_1 : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_WCN_BT_MODEM_NEWPACKET_BYTE1_T;

// gfsk_mod_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ref_a2_dem : 10;    // [9:0]
        uint32_t __11_10 : 2;        // [11:10]
        uint32_t rssi_ct_u_pre : 3;  // [14:12]
        uint32_t bt_mod : 1;         // [15]
        uint32_t rssi_ct_u_post : 3; // [18:16]
        uint32_t __31_19 : 13;       // [31:19]
    } b;
} REG_WCN_BT_MODEM_GFSK_MOD_CTRL_T;

// gfsk_mod_ref_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ref_a3_dem : 8; // [7:0]
        uint32_t ref_a1_dem : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_WCN_BT_MODEM_GFSK_MOD_REF_CTRL_T;

// sym_dly_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay_gfsk_1m : 3;     // [2:0]
        uint32_t delay_dpsk_1m : 3;     // [5:3]
        uint32_t delay_gfsk_off_1m : 3; // [8:6]
        uint32_t delay_gfsk_off : 4;    // [12:9]
        uint32_t ramp_speed_gfsk : 1;   // [13]
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_WCN_BT_MODEM_SYM_DLY_CTRL_T;

// rssi_out2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssi_noise_out_pre : 10;  // [9:0], read only
        uint32_t __15_10 : 6;              // [15:10]
        uint32_t rssi_noise_out_post : 10; // [25:16], read only
        uint32_t __31_26 : 6;              // [31:26]
    } b;
} REG_WCN_BT_MODEM_RSSI_OUT2_T;

// trx_clk_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __5_0 : 6;           // [5:0]
        uint32_t apc_switch_mode : 4; // [9:6]
        uint32_t __11_10 : 2;         // [11:10]
        uint32_t tx_rx_reg : 1;       // [12]
        uint32_t tx_rx_dir : 1;       // [13]
        uint32_t swch_clk_52m_rx : 1; // [14]
        uint32_t __31_15 : 17;        // [31:15]
    } b;
} REG_WCN_BT_MODEM_TRX_CLK_CTRL_T;

// iq_swap_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t notch_en_0 : 1;      // [0]
        uint32_t rate_conv_en_0 : 1;  // [1]
        uint32_t lpf_en_0 : 1;        // [2]
        uint32_t __3_3 : 1;           // [3]
        uint32_t notch_en_1 : 1;      // [4]
        uint32_t rate_conv_en_1 : 1;  // [5]
        uint32_t lpf_en_1 : 1;        // [6]
        uint32_t __11_7 : 5;          // [11:7]
        uint32_t iq_swap_srrc : 1;    // [12]
        uint32_t iq_swap_lpf : 1;     // [13]
        uint32_t iq_swap_gain2 : 1;   // [14]
        uint32_t swch_clk_52m_tx : 1; // [15]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_WCN_BT_MODEM_IQ_SWAP_CTRL_T;

// gfsk_sync_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dynamic_sync_th : 10; // [9:0]
        uint32_t dynamic_sync_en : 1;  // [10]
        uint32_t __31_11 : 21;         // [31:11]
    } b;
} REG_WCN_BT_MODEM_GFSK_SYNC_CTRL_T;

// gfsk_demod_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t min_error_th2 : 16; // [15:0]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_WCN_BT_MODEM_GFSK_DEMOD_CTRL_T;

// gfsk_mod_idx
typedef union {
    uint32_t v;
    struct
    {
        uint32_t h_gain : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_WCN_BT_MODEM_GFSK_MOD_IDX_T;

// dpsk_gfsk_misc_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __2_0 : 3;       // [2:0]
        uint32_t dly_ct_amp : 3;  // [5:3]
        uint32_t dly_ct_dpsk : 3; // [8:6]
        uint32_t dly_ct_gfsk : 3; // [11:9]
        uint32_t __14_12 : 3;     // [14:12]
        uint32_t iq_swap_tx : 1;  // [15]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_WCN_BT_MODEM_DPSK_GFSK_MISC_CTRL_T;

// modem_dbm_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbm_data_sel : 5; // [4:0]
        uint32_t __31_5 : 27;      // [31:5]
    } b;
} REG_WCN_BT_MODEM_MODEM_DBM_SEL_T;

// gfsk_mod_idx_le
typedef union {
    uint32_t v;
    struct
    {
        uint32_t h_gain_le : 16; // [15:0]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_WCN_BT_MODEM_GFSK_MOD_IDX_LE_T;

// newpacket_byte4_inuse
typedef union {
    uint32_t v;
    struct
    {
        uint32_t newpacket_dsp4 : 16; // [15:0], read only
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_WCN_BT_MODEM_NEWPACKET_BYTE4_INUSE_T;

// newpacket_byte3_inuse
typedef union {
    uint32_t v;
    struct
    {
        uint32_t newpacket_dsp3 : 16; // [15:0], read only
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_WCN_BT_MODEM_NEWPACKET_BYTE3_INUSE_T;

// newpacket_byte2_inuse
typedef union {
    uint32_t v;
    struct
    {
        uint32_t newpacket_dsp2 : 16; // [15:0], read only
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_WCN_BT_MODEM_NEWPACKET_BYTE2_INUSE_T;

// newpacket_byte1_inuse
typedef union {
    uint32_t v;
    struct
    {
        uint32_t newpacket_dsp1 : 16; // [15:0], read only
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_WCN_BT_MODEM_NEWPACKET_BYTE1_INUSE_T;

// le_mode_ctrl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ref_a1_dem_le : 8; // [7:0]
        uint32_t ref_a1_sek_le : 8; // [15:8]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_WCN_BT_MODEM_LE_MODE_CTRL1_T;

// le_mode_ctrl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ref_a2_dem_le : 8; // [7:0]
        uint32_t ref_a2_sek_le : 8; // [15:8]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_WCN_BT_MODEM_LE_MODE_CTRL2_T;

// le_mode_ctrl3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ref_a3_dem_le : 8; // [7:0]
        uint32_t ref_a3_sek_le : 8; // [15:8]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_WCN_BT_MODEM_LE_MODE_CTRL3_T;

// le_mode_ctrl4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t min_error_th_le : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_WCN_BT_MODEM_LE_MODE_CTRL4_T;

// le_mode_ctrl5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ref_ready_th_le : 6;  // [5:0]
        uint32_t mix_guard_th_le : 6;  // [11:6]
        uint32_t ref_point_sel_le : 2; // [13:12]
        uint32_t rsvd5 : 2;            // [15:14]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_WCN_BT_MODEM_LE_MODE_CTRL5_T;

// rf_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sync_shift_mode : 1; // [0]
        uint32_t __31_1 : 31;         // [31:1]
    } b;
} REG_WCN_BT_MODEM_RF_CTRL_T;

// tx_q_im
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_iqim_phase_err : 15; // [14:0]
        uint32_t __31_15 : 17;           // [31:15]
    } b;
} REG_WCN_BT_MODEM_TX_Q_IM_T;

// tx_i_im
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_iqim_amp_err : 15; // [14:0]
        uint32_t __31_15 : 17;         // [31:15]
    } b;
} REG_WCN_BT_MODEM_TX_I_IM_T;

// tx_am_p0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p0 : 10;         // [9:0]
        uint32_t am_comp_bypass : 1; // [10]
        uint32_t pm_comp_bypass : 1; // [11]
        uint32_t pm_shift : 3;       // [14:12]
        uint32_t __31_15 : 17;       // [31:15]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P0_T;

// tx_am_p1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p1 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P1_T;

// tx_am_p2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p2 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P2_T;

// tx_am_p3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p3 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P3_T;

// tx_am_p4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p4 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P4_T;

// tx_am_p5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p5 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P5_T;

// tx_am_p6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p6 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P6_T;

// tx_am_p7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p7 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P7_T;

// tx_am_p8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p8 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P8_T;

// tx_am_p9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p9 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P9_T;

// tx_am_p10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p10 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P10_T;

// tx_am_p11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p11 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P11_T;

// tx_am_p12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p12 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P12_T;

// tx_am_p13
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p13 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P13_T;

// tx_am_p14
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p14 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P14_T;

// tx_am_p15
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p15 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P15_T;

// tx_am_p16
typedef union {
    uint32_t v;
    struct
    {
        uint32_t am_p16 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_AM_P16_T;

// tx_pm_p0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p0 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P0_T;

// tx_pm_p1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p1 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P1_T;

// tx_pm_p2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p2 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P2_T;

// tx_pm_p3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p3 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P3_T;

// tx_pm_p4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p4 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P4_T;

// tx_pm_p5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p5 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P5_T;

// tx_pm_p6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p6 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P6_T;

// tx_pm_p7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p7 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P7_T;

// tx_pm_p8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p8 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P8_T;

// tx_pm_p9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p9 : 10;   // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P9_T;

// tx_pm_p10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p10 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P10_T;

// tx_pm_p11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p11 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P11_T;

// tx_pm_p12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p12 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P12_T;

// tx_pm_p13
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p13 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P13_T;

// tx_pm_p14
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p14 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P14_T;

// tx_pm_p15
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p15 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P15_T;

// tx_pm_p16
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_p16 : 10;  // [9:0]
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_MODEM_TX_PM_P16_T;

// notch_coef
typedef union {
    uint32_t v;
    struct
    {
        uint32_t notch_b : 14; // [13:0]
        uint32_t __31_14 : 18; // [31:14]
    } b;
} REG_WCN_BT_MODEM_NOTCH_COEF_T;

// adapt_edr3_demod
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ct_u_1_dpsk1 : 5;  // [4:0]
        uint32_t ct_u_dpsk1 : 4;    // [8:5]
        uint32_t edr3_adapt_en : 1; // [9]
        uint32_t __11_10 : 2;       // [11:10]
        uint32_t notch_a : 4;       // [15:12]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_WCN_BT_MODEM_ADAPT_EDR3_DEMOD_T;

// adapt_edr3_thresh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t edr3_adapt_th : 12; // [11:0]
        uint32_t __31_12 : 20;       // [31:12]
    } b;
} REG_WCN_BT_MODEM_ADAPT_EDR3_THRESH_T;

// tx_auto_gain1_gfsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_auto_gain_gfsk4 : 4; // [3:0]
        uint32_t tx_auto_gain_gfsk5 : 4; // [7:4]
        uint32_t tx_auto_gain_gfsk6 : 4; // [11:8]
        uint32_t tx_auto_gain_gfsk7 : 4; // [15:12]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_WCN_BT_MODEM_TX_AUTO_GAIN1_GFSK_T;

// tx_auto_gain0_gfsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_auto_gain_gfsk0 : 4; // [3:0]
        uint32_t tx_auto_gain_gfsk1 : 4; // [7:4]
        uint32_t tx_auto_gain_gfsk2 : 4; // [11:8]
        uint32_t tx_auto_gain_gfsk3 : 4; // [15:12]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_WCN_BT_MODEM_TX_AUTO_GAIN0_GFSK_T;

// tx_auto_gain1_gfsk_edr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_auto_gain_gfsk_edr4 : 4; // [3:0]
        uint32_t tx_auto_gain_gfsk_edr5 : 4; // [7:4]
        uint32_t tx_auto_gain_gfsk_edr6 : 4; // [11:8]
        uint32_t tx_auto_gain_gfsk_edr7 : 4; // [15:12]
        uint32_t __31_16 : 16;               // [31:16]
    } b;
} REG_WCN_BT_MODEM_TX_AUTO_GAIN1_GFSK_EDR_T;

// tx_auto_gain0_gfsk_edr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_auto_gain_gfsk_edr0 : 4; // [3:0]
        uint32_t tx_auto_gain_gfsk_edr1 : 4; // [7:4]
        uint32_t tx_auto_gain_gfsk_edr2 : 4; // [11:8]
        uint32_t tx_auto_gain_gfsk_edr3 : 4; // [15:12]
        uint32_t __31_16 : 16;               // [31:16]
    } b;
} REG_WCN_BT_MODEM_TX_AUTO_GAIN0_GFSK_EDR_T;

// tx_auto_gain1_dpsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_auto_gain_dpsk4 : 4; // [3:0]
        uint32_t tx_auto_gain_dpsk5 : 4; // [7:4]
        uint32_t tx_auto_gain_dpsk6 : 4; // [11:8]
        uint32_t tx_auto_gain_dpsk7 : 4; // [15:12]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_WCN_BT_MODEM_TX_AUTO_GAIN1_DPSK_T;

// tx_auto_gain0_dpsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_auto_gain_dpsk0 : 4; // [3:0]
        uint32_t tx_auto_gain_dpsk1 : 4; // [7:4]
        uint32_t tx_auto_gain_dpsk2 : 4; // [11:8]
        uint32_t tx_auto_gain_dpsk3 : 4; // [15:12]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_WCN_BT_MODEM_TX_AUTO_GAIN0_DPSK_T;

// gfsk_mod_eql_gain
typedef union {
    uint32_t v;
    struct
    {
        uint32_t equ_g_gfsk : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_WCN_BT_MODEM_GFSK_MOD_EQL_GAIN_T;

// tx_lfp_delay_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lpfil_freq_tx_bw_ct : 5;  // [4:0]
        uint32_t lpfil_freq_tx_enable : 1; // [5]
        uint32_t lpfil_freq_tx_bypass : 1; // [6]
        uint32_t __7_7 : 1;                // [7]
        uint32_t dly_ct_freq1 : 3;         // [10:8]
        uint32_t __11_11 : 1;              // [11]
        uint32_t dly_ct_freq2 : 3;         // [14:12]
        uint32_t __31_15 : 17;             // [31:15]
    } b;
} REG_WCN_BT_MODEM_TX_LFP_DELAY_CTRL_T;

// tx_hfp_delay
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dly_ct_freq_high : 3; // [2:0]
        uint32_t __3_3 : 1;            // [3]
        uint32_t dly_ct_iq2 : 3;       // [6:4]
        uint32_t __7_7 : 1;            // [7]
        uint32_t dly_ct_iq1 : 3;       // [10:8]
        uint32_t __11_11 : 1;          // [11]
        uint32_t dly_sel_freq : 3;     // [14:12]
        uint32_t __31_15 : 17;         // [31:15]
    } b;
} REG_WCN_BT_MODEM_TX_HFP_DELAY_T;

// tx_polar_mode_ctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_apf_bypass : 1;     // [0]
        uint32_t tx_polar_mode_sel : 1; // [1]
        uint32_t tx_tmp_dly : 3;        // [4:2]
        uint32_t __31_5 : 27;           // [31:5]
    } b;
} REG_WCN_BT_MODEM_TX_POLAR_MODE_CTL_T;

// tx_apf_num_b1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t num_coe_b1 : 14; // [13:0]
        uint32_t __31_14 : 18;    // [31:14]
    } b;
} REG_WCN_BT_MODEM_TX_APF_NUM_B1_T;

// tx_apf_num_b2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t num_coe_b2 : 14; // [13:0]
        uint32_t __31_14 : 18;    // [31:14]
    } b;
} REG_WCN_BT_MODEM_TX_APF_NUM_B2_T;

// tx_apf_num_b3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t num_coe_b3 : 14; // [13:0]
        uint32_t __31_14 : 18;    // [31:14]
    } b;
} REG_WCN_BT_MODEM_TX_APF_NUM_B3_T;

// tx_apf_num_b4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t num_coe_b4 : 14; // [13:0]
        uint32_t __31_14 : 18;    // [31:14]
    } b;
} REG_WCN_BT_MODEM_TX_APF_NUM_B4_T;

// tx_apf_den_a2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t den_coe_a2 : 14; // [13:0]
        uint32_t __31_14 : 18;    // [31:14]
    } b;
} REG_WCN_BT_MODEM_TX_APF_DEN_A2_T;

// tx_apf_den_a3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t den_coe_a3 : 14; // [13:0]
        uint32_t __31_14 : 18;    // [31:14]
    } b;
} REG_WCN_BT_MODEM_TX_APF_DEN_A3_T;

// tx_apf_den_a4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t den_coe_a4 : 14; // [13:0]
        uint32_t __31_14 : 18;    // [31:14]
    } b;
} REG_WCN_BT_MODEM_TX_APF_DEN_A4_T;

// adapt_edr3_cci
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ct_u_1_dpsk2 : 5; // [4:0]
        uint32_t ct_u_dpsk2 : 4;   // [8:5]
        uint32_t __12_9 : 4;       // [12:9]
        uint32_t ct_u_errsum : 3;  // [15:13]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_WCN_BT_MODEM_ADAPT_EDR3_CCI_T;

// bypass_ctrl
#define WCN_BT_MODEM_BYPASS_SMPPT_DPSK (1 << 1)
#define WCN_BT_MODEM_BYPASS_PATCH_DPSK (1 << 2)
#define WCN_BT_MODEM_BYPASS_DERR2_DPSK (1 << 3)
#define WCN_BT_MODEM_BYPASS_DERR1_DPSK (1 << 4)
#define WCN_BT_MODEM_BYPASS_SMPPT_GFSK (1 << 5)
#define WCN_BT_MODEM_BYPASS_PATCH_GFSK (1 << 6)
#define WCN_BT_MODEM_BYPASS_DERR2_GFSK (1 << 7)
#define WCN_BT_MODEM_BYPASS_DERR1_GFSK (1 << 8)
#define WCN_BT_MODEM_BYPASS_SRRC (1 << 12)
#define WCN_BT_MODEM_BYPASS_MIXER (1 << 13)
#define WCN_BT_MODEM_BYPASS_DCCANCEL2 (1 << 14)
#define WCN_BT_MODEM_BYPASS_DCCANCEL (1 << 15)

// sw_swap_dccal
#define WCN_BT_MODEM_SEL_SUMERR_RANGE(n) (((n)&0x7) << 0)
#define WCN_BT_MODEM_IQ_SEL_POL (1 << 5)
#define WCN_BT_MODEM_SWCH_SIGN_RX (1 << 6)
#define WCN_BT_MODEM_SWCH_SIGN_Q_TX (1 << 7)
#define WCN_BT_MODEM_SWCH_SIGN_I_TX (1 << 8)
#define WCN_BT_MODEM_LPF_DWIDTH_SEL(n) (((n)&0x3) << 9)
#define WCN_BT_MODEM_RSVD0 (1 << 11)
#define WCN_BT_MODEM_SWCH_CLK_DAC (1 << 12)
#define WCN_BT_MODEM_SEL_SYNC (1 << 13)
#define WCN_BT_MODEM_SWCH_CLK_ADC (1 << 14)

// dem750_afc_freq
#define WCN_BT_MODEM_AFC_SMTIF(n) (((n)&0xffff) << 0)

// dpsk_gfsk_tx_ctrl
#define WCN_BT_MODEM_DELAY_DPSK(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_DELAY_GFSK(n) (((n)&0xf) << 4)
#define WCN_BT_MODEM_TX_GAIN_DPSK(n) (((n)&0xff) << 8)

// tx_gain_ctrl
#define WCN_BT_MODEM_TX_GAIN_GFSK(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_TX_AUTO_GAIN_BYPASS (1 << 8)
#define WCN_BT_MODEM_TX_POWER_GAIN_SEL (1 << 9)
#define WCN_BT_MODEM_CNT_GUARD_INI(n) (((n)&0x3f) << 10)

// rssi_out1
#define WCN_BT_MODEM_RSSI_OUT_PRE(n) (((n)&0x3ff) << 0)
#define WCN_BT_MODEM_RSSI_OUT_POST(n) (((n)&0x3ff) << 16)

// sw_ctrl
#define WCN_BT_MODEM_TX_GAIN_GFSK_EDR(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_NOTCH_DISB (1 << 8)
#define WCN_BT_MODEM_SWAP_PN_Q_DCCL_0 (1 << 9)
#define WCN_BT_MODEM_SWAP_PN_I_DCCL_0 (1 << 10)
#define WCN_BT_MODEM_SWAP_IQ_MIXER_0 (1 << 11)
#define WCN_BT_MODEM_SWAP_IQ_SMTIF_0 (1 << 12)
#define WCN_BT_MODEM_SWAP_IQ_DCCL_0 (1 << 13)
#define WCN_BT_MODEM_SWAP_IQ (1 << 14)
#define WCN_BT_MODEM_RSSI_LOCK_BY_AGC_PRE (1 << 15)
#define WCN_BT_MODEM_RSSI_LOCK_BY_AGC_POST (1 << 16)

// adcclk_sw_ctrl
#define WCN_BT_MODEM_CT_U_SP_GFSK(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_CT_U_1_GFSK(n) (((n)&0x1f) << 4)
#define WCN_BT_MODEM_CT_U_GFSK(n) (((n)&0xf) << 9)
#define WCN_BT_MODEM_PCKT_SEL (1 << 13)
#define WCN_BT_MODEM_EN_BBPKG_FLG (1 << 14)

// dpsk_demod_sw
#define WCN_BT_MODEM_CT_U_SP_DPSK(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_CT_U_1_DPSK(n) (((n)&0x1f) << 4)
#define WCN_BT_MODEM_CT_U_DPSK(n) (((n)&0xf) << 9)
#define WCN_BT_MODEM_CT_U_PATCH(n) (((n)&0x7) << 13)

// min_phase_err
#define WCN_BT_MODEM_MIN_ERROR_TH(n) (((n)&0xffff) << 0)

// afc_ctrl
#define WCN_BT_MODEM_TH_GFSK_DEM(n) (((n)&0x3ff) << 0)
#define WCN_BT_MODEM_RSVD1(n) (((n)&0x3) << 10)
#define WCN_BT_MODEM_SEL_AFC_DPSKSEEK(n) (((n)&0x3) << 12)
#define WCN_BT_MODEM_SEL_AFC_GFSKDEM(n) (((n)&0x3) << 14)

// dpsk_gfsk_smp_th
#define WCN_BT_MODEM_TH_GFSK_DEM2(n) (((n)&0x3ff) << 0)
#define WCN_BT_MODEM_SP_TH_SEL_GFSK(n) (((n)&0x3) << 10)
#define WCN_BT_MODEM_SP_TH_SEL_DPSK(n) (((n)&0x3) << 12)
#define WCN_BT_MODEM_DIFF_ENABLE_GFSK (1 << 14)
#define WCN_BT_MODEM_DIFF_ENABLE_DPSK (1 << 15)

// dpsk_gfsk_smp_th_1
#define WCN_BT_MODEM_REF_A2_SEK(n) (((n)&0x3ff) << 0)

// gfsk_smp_ref_ctrl
#define WCN_BT_MODEM_REF_A3_SEK(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_REF_A1_SEK(n) (((n)&0xff) << 8)

// gfsk_smp_ref_ctrl_1
#define WCN_BT_MODEM_DC_CT2(n) (((n)&0x1ff) << 0)
#define WCN_BT_MODEM_GUARD_TIME_CONF(n) (((n)&0x1f) << 9)
#define WCN_BT_MODEM_RSVD2(n) (((n)&0x3) << 14)

// rateconv_ctrl1
#define WCN_BT_MODEM_DAC_DATA_SEL(n) (((n)&0x3f) << 0)
#define WCN_BT_MODEM_DAC_TEST_EN (1 << 6)
#define WCN_BT_MODEM_DC_CT(n) (((n)&0x1ff) << 7)

// rateconv_ctrl2
#define WCN_BT_MODEM_RSSI_TM_TH_PRE(n) (((n)&0x3f) << 0)
#define WCN_BT_MODEM_DC_HOLD_EDR_EN (1 << 6)
#define WCN_BT_MODEM_RSSI_TM_TH_POST(n) (((n)&0x3f) << 7)
#define WCN_BT_MODEM_RSSI_SEL(n) (((n)&0x3) << 13)
#define WCN_BT_MODEM_SEL_RSSI_SRC_PRE (1 << 15)
#define WCN_BT_MODEM_SEL_RSSI_SRC_POST (1 << 16)

// demod_smp_ctrl
#define WCN_BT_MODEM_CNT_SAMPLE_INI(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_REF_READY_TH(n) (((n)&0x3f) << 4)
#define WCN_BT_MODEM_INI_RST_TH(n) (((n)&0x3f) << 10)

// agc_ctrl
#define WCN_BT_MODEM_AGC_SINC_OVER_TH(n) (((n)&0x3) << 0)
#define WCN_BT_MODEM_AGC_SINC_OVER_EN (1 << 2)
#define WCN_BT_MODEM_AGC_MOD_FIX7 (1 << 3)
#define WCN_BT_MODEM_AGC_EN_FIX7 (1 << 4)
#define WCN_BT_MODEM_DPSK_SEEK_ST_CNT(n) (((n)&0x1f) << 5)
#define WCN_BT_MODEM_MIX_GUARD_TH(n) (((n)&0x3f) << 10)

// agc_th_ctrl1
#define WCN_BT_MODEM_AGC_TH_MIN(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_AGC_TH_MAX(n) (((n)&0xff) << 8)

// agc_th_ctrl2
#define WCN_BT_MODEM_AGC_TH_MIN_LG(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_AGC_TH_MAX_LG(n) (((n)&0xff) << 8)

// agc_ctrl1
#define WCN_BT_MODEM_AGC_MOD_DEM(n) (((n)&0x3) << 0)
#define WCN_BT_MODEM_AGC_INDEX_INI_DSP(n) (((n)&0xf) << 2)
#define WCN_BT_MODEM_AGC_EN_DLY(n) (((n)&0x7) << 6)
#define WCN_BT_MODEM_AGC_STEP_OVER(n) (((n)&0x3) << 9)
#define WCN_BT_MODEM_AGC_STEP_MODE(n) (((n)&0x3) << 11)
#define WCN_BT_MODEM_AGC_EN_LG_STP (1 << 13)
#define WCN_BT_MODEM_AGC_CT_TH_MIN (1 << 14)

// agc_ctrl2
#define WCN_BT_MODEM_AGC_TM_INTV_LG(n) (((n)&0x7f) << 0)
#define WCN_BT_MODEM_AGC_TM_INTV(n) (((n)&0x7f) << 7)
#define WCN_BT_MODEM_AGC_TH_MIN_GN(n) (((n)&0x3) << 14)

// agc_dgc_ctrl
#define WCN_BT_MODEM_AGC_MODE_DSP(n) (((n)&0x7) << 0)
#define WCN_BT_MODEM_NEWPACKET_ZERO_WD_CNT(n) (((n)&0x3) << 6)
#define WCN_BT_MODEM_PASS_NEWPACKET_SEL (1 << 8)
#define WCN_BT_MODEM_DGC_INDEX_MAX(n) (((n)&0x7) << 9)
#define WCN_BT_MODEM_DGC_INDEX_DSP(n) (((n)&0x7) << 12)
#define WCN_BT_MODEM_SEL_REG_AGC (1 << 15)

// agc_dccal_ctrl
#define WCN_BT_MODEM_RX_FIX_DCOFST (1 << 0)
#define WCN_BT_MODEM_DC_CAL_RX_DLY(n) (((n)&0x3) << 1)
#define WCN_BT_MODEM_RX_DC_CAL_DONE (1 << 3)
#define WCN_BT_MODEM_AGC_TM_HOLD(n) (((n)&0x1f) << 6)
#define WCN_BT_MODEM_AGC_TM_WAIT(n) (((n)&0x1f) << 11)

// rx_dccal_ctrl
#define WCN_BT_MODEM_RX_DC_CALI_Q_FIX(n) (((n)&0x3ff) << 0)
#define WCN_BT_MODEM_RX_DC_CALI_I_FIX(n) (((n)&0x3ff) << 16)

// rx_dcofst_inuse
#define WCN_BT_MODEM_RX_DCOFFSET_Q(n) (((n)&0x3ff) << 0)
#define WCN_BT_MODEM_RX_DCOFFSET_I(n) (((n)&0x3ff) << 16)

// tx_dc_calib
#define WCN_BT_MODEM_BYPASS_TX_CAL (1 << 0)
#define WCN_BT_MODEM_TX_APC(n) (((n)&0x7) << 1)
#define WCN_BT_MODEM_TX_CAL_SHIFT(n) (((n)&0x3) << 4)
#define WCN_BT_MODEM_TX_CAL_SEL(n) (((n)&0x3) << 6)
#define WCN_BT_MODEM_TX_CAL_POL (1 << 8)
#define WCN_BT_MODEM_TX_CAL_CMP_POL (1 << 9)
#define WCN_BT_MODEM_TX_CAL_CNT(n) (((n)&0x3) << 10)
#define WCN_BT_MODEM_TX_FIX_DCOFST (1 << 12)
#define WCN_BT_MODEM_TX_CAL_OUT_Q (1 << 13)
#define WCN_BT_MODEM_TX_CAL_OUT_I (1 << 14)
#define WCN_BT_MODEM_TX_DC_CAL_DONE (1 << 15)

// tx_fix_i_dcofst
#define WCN_BT_MODEM_TX_DC_IDATA_FIX(n) (((n)&0xfff) << 0)

// tx_fix_q_dcofst
#define WCN_BT_MODEM_TX_DC_QDATA_FIX(n) (((n)&0xfff) << 0)

// tx_i_dcofst_inuse
#define WCN_BT_MODEM_DC_CAL_TX_IDATA(n) (((n)&0xfff) << 0)

// tx_q_dcofst_inuse
#define WCN_BT_MODEM_DC_CAL_TX_QDATA(n) (((n)&0xfff) << 0)

// rssi_gain_ctrl1
#define WCN_BT_MODEM_RSSI_ANA_GAIN0001(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_RSSI_ANA_GAIN0000(n) (((n)&0xff) << 8)

// rssi_gain_ctrl2
#define WCN_BT_MODEM_RSSI_ANA_GAIN0011(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_RSSI_ANA_GAIN0010(n) (((n)&0xff) << 8)

// rssi_gain_ctrl3
#define WCN_BT_MODEM_RSSI_ANA_GAIN0101(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_RSSI_ANA_GAIN0100(n) (((n)&0xff) << 8)

// rssi_gain_ctrl4
#define WCN_BT_MODEM_RSSI_ANA_GAIN0111(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_RSSI_ANA_GAIN0110(n) (((n)&0xff) << 8)

// rssi_gain_ctrl5
#define WCN_BT_MODEM_RSSI_ANA_GAIN1001(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_RSSI_ANA_GAIN1000(n) (((n)&0xff) << 8)

// rssi_gain_ctrl6
#define WCN_BT_MODEM_RSSI_ANA_GAIN1011(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_RSSI_ANA_GAIN1010(n) (((n)&0xff) << 8)

// rssi_gain_ctrl7
#define WCN_BT_MODEM_RSSI_ANA_GAIN1101(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_RSSI_ANA_GAIN1100(n) (((n)&0xff) << 8)

// rssi_gain_ctrl8
#define WCN_BT_MODEM_RSSI_ANA_GAIN1111(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_RSSI_ANA_GAIN1110(n) (((n)&0xff) << 8)

// modem_tpd_sel
#define WCN_BT_MODEM_TPD_TRIG_SEL(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_TPD_DATA_SEL(n) (((n)&0xf) << 4)
#define WCN_BT_MODEM_TPD_CLK_SEL (1 << 8)
#define WCN_BT_MODEM_DAC_CLK_FORCE_EN (1 << 9)

// demod_smp_th_ctrl
#define WCN_BT_MODEM_DEM_SP_TH1(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_DEM_SP_TH2(n) (((n)&0xff) << 8)

// newpacket_byte4
#define WCN_BT_MODEM_NEWPACKET_4(n) (((n)&0xffff) << 0)

// newpacket_byte3
#define WCN_BT_MODEM_NEWPACKET_3(n) (((n)&0xffff) << 0)

// newpacket_byte2
#define WCN_BT_MODEM_NEWPACKET_2(n) (((n)&0xffff) << 0)

// newpacket_byte1
#define WCN_BT_MODEM_NEWPACKET_1(n) (((n)&0xffff) << 0)

// gfsk_mod_ctrl
#define WCN_BT_MODEM_REF_A2_DEM(n) (((n)&0x3ff) << 0)
#define WCN_BT_MODEM_RSSI_CT_U_PRE(n) (((n)&0x7) << 12)
#define WCN_BT_MODEM_BT_MOD (1 << 15)
#define WCN_BT_MODEM_RSSI_CT_U_POST(n) (((n)&0x7) << 16)

// gfsk_mod_ref_ctrl
#define WCN_BT_MODEM_REF_A3_DEM(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_REF_A1_DEM(n) (((n)&0xff) << 8)

// sym_dly_ctrl
#define WCN_BT_MODEM_DELAY_GFSK_1M(n) (((n)&0x7) << 0)
#define WCN_BT_MODEM_DELAY_DPSK_1M(n) (((n)&0x7) << 3)
#define WCN_BT_MODEM_DELAY_GFSK_OFF_1M(n) (((n)&0x7) << 6)
#define WCN_BT_MODEM_DELAY_GFSK_OFF(n) (((n)&0xf) << 9)
#define WCN_BT_MODEM_RAMP_SPEED_GFSK (1 << 13)

// rssi_out2
#define WCN_BT_MODEM_RSSI_NOISE_OUT_PRE(n) (((n)&0x3ff) << 0)
#define WCN_BT_MODEM_RSSI_NOISE_OUT_POST(n) (((n)&0x3ff) << 16)

// trx_clk_ctrl
#define WCN_BT_MODEM_APC_SWITCH_MODE(n) (((n)&0xf) << 6)
#define WCN_BT_MODEM_TX_RX_REG (1 << 12)
#define WCN_BT_MODEM_TX_RX_DIR (1 << 13)
#define WCN_BT_MODEM_SWCH_CLK_52M_RX (1 << 14)

// iq_swap_ctrl
#define WCN_BT_MODEM_NOTCH_EN_0 (1 << 0)
#define WCN_BT_MODEM_RATE_CONV_EN_0 (1 << 1)
#define WCN_BT_MODEM_LPF_EN_0 (1 << 2)
#define WCN_BT_MODEM_NOTCH_EN_1 (1 << 4)
#define WCN_BT_MODEM_RATE_CONV_EN_1 (1 << 5)
#define WCN_BT_MODEM_LPF_EN_1 (1 << 6)
#define WCN_BT_MODEM_IQ_SWAP_SRRC (1 << 12)
#define WCN_BT_MODEM_IQ_SWAP_LPF (1 << 13)
#define WCN_BT_MODEM_IQ_SWAP_GAIN2 (1 << 14)
#define WCN_BT_MODEM_SWCH_CLK_52M_TX (1 << 15)

// gfsk_sync_ctrl
#define WCN_BT_MODEM_DYNAMIC_SYNC_TH(n) (((n)&0x3ff) << 0)
#define WCN_BT_MODEM_DYNAMIC_SYNC_EN (1 << 10)

// gfsk_demod_ctrl
#define WCN_BT_MODEM_MIN_ERROR_TH2(n) (((n)&0xffff) << 0)

// gfsk_mod_idx
#define WCN_BT_MODEM_H_GAIN(n) (((n)&0xffff) << 0)

// dpsk_gfsk_misc_ctrl
#define WCN_BT_MODEM_DLY_CT_AMP(n) (((n)&0x7) << 3)
#define WCN_BT_MODEM_DLY_CT_DPSK(n) (((n)&0x7) << 6)
#define WCN_BT_MODEM_DLY_CT_GFSK(n) (((n)&0x7) << 9)
#define WCN_BT_MODEM_IQ_SWAP_TX (1 << 15)

// modem_dbm_sel
#define WCN_BT_MODEM_DBM_DATA_SEL(n) (((n)&0x1f) << 0)

// gfsk_mod_idx_le
#define WCN_BT_MODEM_H_GAIN_LE(n) (((n)&0xffff) << 0)

// newpacket_byte4_inuse
#define WCN_BT_MODEM_NEWPACKET_DSP4(n) (((n)&0xffff) << 0)

// newpacket_byte3_inuse
#define WCN_BT_MODEM_NEWPACKET_DSP3(n) (((n)&0xffff) << 0)

// newpacket_byte2_inuse
#define WCN_BT_MODEM_NEWPACKET_DSP2(n) (((n)&0xffff) << 0)

// newpacket_byte1_inuse
#define WCN_BT_MODEM_NEWPACKET_DSP1(n) (((n)&0xffff) << 0)

// le_mode_ctrl1
#define WCN_BT_MODEM_REF_A1_DEM_LE(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_REF_A1_SEK_LE(n) (((n)&0xff) << 8)

// le_mode_ctrl2
#define WCN_BT_MODEM_REF_A2_DEM_LE(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_REF_A2_SEK_LE(n) (((n)&0xff) << 8)

// le_mode_ctrl3
#define WCN_BT_MODEM_REF_A3_DEM_LE(n) (((n)&0xff) << 0)
#define WCN_BT_MODEM_REF_A3_SEK_LE(n) (((n)&0xff) << 8)

// le_mode_ctrl4
#define WCN_BT_MODEM_MIN_ERROR_TH_LE(n) (((n)&0xffff) << 0)

// le_mode_ctrl5
#define WCN_BT_MODEM_REF_READY_TH_LE(n) (((n)&0x3f) << 0)
#define WCN_BT_MODEM_MIX_GUARD_TH_LE(n) (((n)&0x3f) << 6)
#define WCN_BT_MODEM_REF_POINT_SEL_LE(n) (((n)&0x3) << 12)
#define WCN_BT_MODEM_RSVD5(n) (((n)&0x3) << 14)

// rf_ctrl
#define WCN_BT_MODEM_SYNC_SHIFT_MODE (1 << 0)

// tx_q_im
#define WCN_BT_MODEM_TX_IQIM_PHASE_ERR(n) (((n)&0x7fff) << 0)

// tx_i_im
#define WCN_BT_MODEM_TX_IQIM_AMP_ERR(n) (((n)&0x7fff) << 0)

// tx_am_p0
#define WCN_BT_MODEM_AM_P0(n) (((n)&0x3ff) << 0)
#define WCN_BT_MODEM_AM_COMP_BYPASS (1 << 10)
#define WCN_BT_MODEM_PM_COMP_BYPASS (1 << 11)
#define WCN_BT_MODEM_PM_SHIFT(n) (((n)&0x7) << 12)

// tx_am_p1
#define WCN_BT_MODEM_AM_P1(n) (((n)&0x3ff) << 0)

// tx_am_p2
#define WCN_BT_MODEM_AM_P2(n) (((n)&0x3ff) << 0)

// tx_am_p3
#define WCN_BT_MODEM_AM_P3(n) (((n)&0x3ff) << 0)

// tx_am_p4
#define WCN_BT_MODEM_AM_P4(n) (((n)&0x3ff) << 0)

// tx_am_p5
#define WCN_BT_MODEM_AM_P5(n) (((n)&0x3ff) << 0)

// tx_am_p6
#define WCN_BT_MODEM_AM_P6(n) (((n)&0x3ff) << 0)

// tx_am_p7
#define WCN_BT_MODEM_AM_P7(n) (((n)&0x3ff) << 0)

// tx_am_p8
#define WCN_BT_MODEM_AM_P8(n) (((n)&0x3ff) << 0)

// tx_am_p9
#define WCN_BT_MODEM_AM_P9(n) (((n)&0x3ff) << 0)

// tx_am_p10
#define WCN_BT_MODEM_AM_P10(n) (((n)&0x3ff) << 0)

// tx_am_p11
#define WCN_BT_MODEM_AM_P11(n) (((n)&0x3ff) << 0)

// tx_am_p12
#define WCN_BT_MODEM_AM_P12(n) (((n)&0x3ff) << 0)

// tx_am_p13
#define WCN_BT_MODEM_AM_P13(n) (((n)&0x3ff) << 0)

// tx_am_p14
#define WCN_BT_MODEM_AM_P14(n) (((n)&0x3ff) << 0)

// tx_am_p15
#define WCN_BT_MODEM_AM_P15(n) (((n)&0x3ff) << 0)

// tx_am_p16
#define WCN_BT_MODEM_AM_P16(n) (((n)&0x3ff) << 0)

// tx_pm_p0
#define WCN_BT_MODEM_PM_P0(n) (((n)&0x3ff) << 0)

// tx_pm_p1
#define WCN_BT_MODEM_PM_P1(n) (((n)&0x3ff) << 0)

// tx_pm_p2
#define WCN_BT_MODEM_PM_P2(n) (((n)&0x3ff) << 0)

// tx_pm_p3
#define WCN_BT_MODEM_PM_P3(n) (((n)&0x3ff) << 0)

// tx_pm_p4
#define WCN_BT_MODEM_PM_P4(n) (((n)&0x3ff) << 0)

// tx_pm_p5
#define WCN_BT_MODEM_PM_P5(n) (((n)&0x3ff) << 0)

// tx_pm_p6
#define WCN_BT_MODEM_PM_P6(n) (((n)&0x3ff) << 0)

// tx_pm_p7
#define WCN_BT_MODEM_PM_P7(n) (((n)&0x3ff) << 0)

// tx_pm_p8
#define WCN_BT_MODEM_PM_P8(n) (((n)&0x3ff) << 0)

// tx_pm_p9
#define WCN_BT_MODEM_PM_P9(n) (((n)&0x3ff) << 0)

// tx_pm_p10
#define WCN_BT_MODEM_PM_P10(n) (((n)&0x3ff) << 0)

// tx_pm_p11
#define WCN_BT_MODEM_PM_P11(n) (((n)&0x3ff) << 0)

// tx_pm_p12
#define WCN_BT_MODEM_PM_P12(n) (((n)&0x3ff) << 0)

// tx_pm_p13
#define WCN_BT_MODEM_PM_P13(n) (((n)&0x3ff) << 0)

// tx_pm_p14
#define WCN_BT_MODEM_PM_P14(n) (((n)&0x3ff) << 0)

// tx_pm_p15
#define WCN_BT_MODEM_PM_P15(n) (((n)&0x3ff) << 0)

// tx_pm_p16
#define WCN_BT_MODEM_PM_P16(n) (((n)&0x3ff) << 0)

// notch_coef
#define WCN_BT_MODEM_NOTCH_B(n) (((n)&0x3fff) << 0)

// adapt_edr3_demod
#define WCN_BT_MODEM_CT_U_1_DPSK1(n) (((n)&0x1f) << 0)
#define WCN_BT_MODEM_CT_U_DPSK1(n) (((n)&0xf) << 5)
#define WCN_BT_MODEM_EDR3_ADAPT_EN (1 << 9)
#define WCN_BT_MODEM_NOTCH_A(n) (((n)&0xf) << 12)

// adapt_edr3_thresh
#define WCN_BT_MODEM_EDR3_ADAPT_TH(n) (((n)&0xfff) << 0)

// tx_auto_gain1_gfsk
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK4(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK5(n) (((n)&0xf) << 4)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK6(n) (((n)&0xf) << 8)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK7(n) (((n)&0xf) << 12)

// tx_auto_gain0_gfsk
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK0(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK1(n) (((n)&0xf) << 4)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK2(n) (((n)&0xf) << 8)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK3(n) (((n)&0xf) << 12)

// tx_auto_gain1_gfsk_edr
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK_EDR4(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK_EDR5(n) (((n)&0xf) << 4)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK_EDR6(n) (((n)&0xf) << 8)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK_EDR7(n) (((n)&0xf) << 12)

// tx_auto_gain0_gfsk_edr
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK_EDR0(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK_EDR1(n) (((n)&0xf) << 4)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK_EDR2(n) (((n)&0xf) << 8)
#define WCN_BT_MODEM_TX_AUTO_GAIN_GFSK_EDR3(n) (((n)&0xf) << 12)

// tx_auto_gain1_dpsk
#define WCN_BT_MODEM_TX_AUTO_GAIN_DPSK4(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_TX_AUTO_GAIN_DPSK5(n) (((n)&0xf) << 4)
#define WCN_BT_MODEM_TX_AUTO_GAIN_DPSK6(n) (((n)&0xf) << 8)
#define WCN_BT_MODEM_TX_AUTO_GAIN_DPSK7(n) (((n)&0xf) << 12)

// tx_auto_gain0_dpsk
#define WCN_BT_MODEM_TX_AUTO_GAIN_DPSK0(n) (((n)&0xf) << 0)
#define WCN_BT_MODEM_TX_AUTO_GAIN_DPSK1(n) (((n)&0xf) << 4)
#define WCN_BT_MODEM_TX_AUTO_GAIN_DPSK2(n) (((n)&0xf) << 8)
#define WCN_BT_MODEM_TX_AUTO_GAIN_DPSK3(n) (((n)&0xf) << 12)

// gfsk_mod_eql_gain
#define WCN_BT_MODEM_EQU_G_GFSK(n) (((n)&0xffff) << 0)

// tx_lfp_delay_ctrl
#define WCN_BT_MODEM_LPFIL_FREQ_TX_BW_CT(n) (((n)&0x1f) << 0)
#define WCN_BT_MODEM_LPFIL_FREQ_TX_ENABLE (1 << 5)
#define WCN_BT_MODEM_LPFIL_FREQ_TX_BYPASS (1 << 6)
#define WCN_BT_MODEM_DLY_CT_FREQ1(n) (((n)&0x7) << 8)
#define WCN_BT_MODEM_DLY_CT_FREQ2(n) (((n)&0x7) << 12)

// tx_hfp_delay
#define WCN_BT_MODEM_DLY_CT_FREQ_HIGH(n) (((n)&0x7) << 0)
#define WCN_BT_MODEM_DLY_CT_IQ2(n) (((n)&0x7) << 4)
#define WCN_BT_MODEM_DLY_CT_IQ1(n) (((n)&0x7) << 8)
#define WCN_BT_MODEM_DLY_SEL_FREQ(n) (((n)&0x7) << 12)

// tx_polar_mode_ctl
#define WCN_BT_MODEM_TX_APF_BYPASS (1 << 0)
#define WCN_BT_MODEM_TX_POLAR_MODE_SEL (1 << 1)
#define WCN_BT_MODEM_TX_TMP_DLY(n) (((n)&0x7) << 2)

// tx_apf_num_b1
#define WCN_BT_MODEM_NUM_COE_B1(n) (((n)&0x3fff) << 0)

// tx_apf_num_b2
#define WCN_BT_MODEM_NUM_COE_B2(n) (((n)&0x3fff) << 0)

// tx_apf_num_b3
#define WCN_BT_MODEM_NUM_COE_B3(n) (((n)&0x3fff) << 0)

// tx_apf_num_b4
#define WCN_BT_MODEM_NUM_COE_B4(n) (((n)&0x3fff) << 0)

// tx_apf_den_a2
#define WCN_BT_MODEM_DEN_COE_A2(n) (((n)&0x3fff) << 0)

// tx_apf_den_a3
#define WCN_BT_MODEM_DEN_COE_A3(n) (((n)&0x3fff) << 0)

// tx_apf_den_a4
#define WCN_BT_MODEM_DEN_COE_A4(n) (((n)&0x3fff) << 0)

// adapt_edr3_cci
#define WCN_BT_MODEM_CT_U_1_DPSK2(n) (((n)&0x1f) << 0)
#define WCN_BT_MODEM_CT_U_DPSK2(n) (((n)&0xf) << 5)
#define WCN_BT_MODEM_CT_U_ERRSUM(n) (((n)&0x7) << 13)

#endif // _WCN_BT_MODEM_H_
