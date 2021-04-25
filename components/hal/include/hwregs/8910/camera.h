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

#ifndef _CAMERA_H_
#define _CAMERA_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define FIFORAM_SIZE (80)

#define REG_CAMERA_BASE (0x08819000)

typedef volatile struct
{
    uint32_t ctrl;                  // 0x00000000
    uint32_t status;                // 0x00000004
    uint32_t data;                  // 0x00000008
    uint32_t irq_mask;              // 0x0000000c
    uint32_t irq_clear;             // 0x00000010
    uint32_t irq_cause;             // 0x00000014
    uint32_t cmd_set;               // 0x00000018
    uint32_t cmd_clr;               // 0x0000001c
    uint32_t dstwincol;             // 0x00000020
    uint32_t dstwinrow;             // 0x00000024
    uint32_t scl_config;            // 0x00000028
    uint32_t spi_camera_reg0;       // 0x0000002c
    uint32_t spi_camera_reg1;       // 0x00000030
    uint32_t spi_camera_reg2;       // 0x00000034
    uint32_t spi_camera_reg3;       // 0x00000038
    uint32_t spi_camera_reg4;       // 0x0000003c
    uint32_t spi_camera_reg5;       // 0x00000040
    uint32_t spi_camera_reg6;       // 0x00000044
    uint32_t spi_camera_obs0;       // 0x00000048
    uint32_t spi_camera_obs1;       // 0x0000004c
    uint32_t csi_config_reg0;       // 0x00000050
    uint32_t csi_config_reg1;       // 0x00000054
    uint32_t csi_config_reg2;       // 0x00000058
    uint32_t csi_config_reg3;       // 0x0000005c
    uint32_t csi_config_reg4;       // 0x00000060
    uint32_t csi_config_reg5;       // 0x00000064
    uint32_t csi_config_reg6;       // 0x00000068
    uint32_t csi_config_reg7;       // 0x0000006c
    uint32_t csi_obs4;              // 0x00000070
    uint32_t csi_obs5;              // 0x00000074
    uint32_t csi_obs6;              // 0x00000078
    uint32_t csi_obs7;              // 0x0000007c
    uint32_t csi_enable;            // 0x00000080
    uint32_t csi_config_reg8;       // 0x00000084
    uint32_t __136[222];            // 0x00000088
    struct                          // 0x00000400
    {                               //
        uint32_t ramdata;           // 0x00000000
    } fiforam[80];                  //
    uint32_t __1344[176];           // 0x00000540
    uint32_t soft_reset;            // 0x00000800
    uint32_t __2052[17];            // 0x00000804
    uint32_t awb_x1_min;            // 0x00000848
    uint32_t awb_x1_max;            // 0x0000084c
    uint32_t awb_y1_min;            // 0x00000850
    uint32_t awb_y1_max;            // 0x00000854
    uint32_t awb_x2_min;            // 0x00000858
    uint32_t awb_x2_max;            // 0x0000085c
    uint32_t awb_y2_min;            // 0x00000860
    uint32_t awb_y2_max;            // 0x00000864
    uint32_t awb_x3_min;            // 0x00000868
    uint32_t awb_x3_max;            // 0x0000086c
    uint32_t awb_y3_min;            // 0x00000870
    uint32_t awb_y3_max;            // 0x00000874
    uint32_t awb_x4_min;            // 0x00000878
    uint32_t awb_x4_max;            // 0x0000087c
    uint32_t awb_y4_min;            // 0x00000880
    uint32_t awb_y4_max;            // 0x00000884
    uint32_t awb_x5_min;            // 0x00000888
    uint32_t awb_x5_max;            // 0x0000088c
    uint32_t awb_y5_min;            // 0x00000890
    uint32_t awb_y5_max;            // 0x00000894
    uint32_t awb_skin_x1_min;       // 0x00000898
    uint32_t awb_skin_x1_max;       // 0x0000089c
    uint32_t awb_skin_y1_min;       // 0x000008a0
    uint32_t awb_skin_y1_max;       // 0x000008a4
    uint32_t awb_skin_x2_min;       // 0x000008a8
    uint32_t awb_skin_x2_max;       // 0x000008ac
    uint32_t awb_skin_y2_min;       // 0x000008b0
    uint32_t awb_skin_y2_max;       // 0x000008b4
    uint32_t awb_ctd_msb;           // 0x000008b8
    uint32_t int_dif_thr_mid;       // 0x000008bc
    uint32_t lb_soft_rstn;          // 0x000008c0
    uint32_t vsync_end_high;        // 0x000008c4
    uint32_t vsync_end_low;         // 0x000008c8
    uint32_t line_numl;             // 0x000008cc
    uint32_t pix_numl;              // 0x000008d0
    uint32_t pix_line_numh;         // 0x000008d4
    uint32_t lb_ctrl;               // 0x000008d8
    uint32_t data_format;           // 0x000008dc
    uint32_t lb_enable;             // 0x000008e0
    uint32_t vh_inv;                // 0x000008e4
    uint32_t line_cnt_l;            // 0x000008e8
    uint32_t line_cnt_h;            // 0x000008ec
    uint32_t num_check;             // 0x000008f0
    uint32_t dci_ctrl_reg;          // 0x000008f4
    uint32_t dci_ofst_reg;          // 0x000008f8
    uint32_t dci_hist_reg;          // 0x000008fc
    uint32_t ae_sw_ctrl_reg;        // 0x00000900
    uint32_t ae_thr_reg;            // 0x00000904
    uint32_t ae_misc_ctrl_reg;      // 0x00000908
    uint32_t csup_xx_reg;           // 0x0000090c
    uint32_t contr_ythr_reg;        // 0x00000910
    uint32_t contr_yave_offset_reg; // 0x00000914
    uint32_t contr_ku_lo_reg;       // 0x00000918
    uint32_t contr_kl_lo_reg;       // 0x0000091c
    uint32_t contr_ku_mid_reg;      // 0x00000920
    uint32_t contr_kl_mid_reg;      // 0x00000924
    uint32_t contr_ku_hi_reg;       // 0x00000928
    uint32_t contr_kl_hi_reg;       // 0x0000092c
    uint32_t luma_offset_lo_reg;    // 0x00000930
    uint32_t luma_offset_mid_reg;   // 0x00000934
    uint32_t luma_offset_hi_reg;    // 0x00000938
    uint32_t u_gain_lo_reg;         // 0x0000093c
    uint32_t v_gain_lo_reg;         // 0x00000940
    uint32_t u_gain_mid_reg;        // 0x00000944
    uint32_t v_gain_mid_reg;        // 0x00000948
    uint32_t u_gain_hi_reg;         // 0x0000094c
    uint32_t v_gain_hi_reg;         // 0x00000950
    uint32_t again_sel_th0_reg;     // 0x00000954
    uint32_t awb_cc_type_ctrl_reg;  // 0x00000958
    uint32_t awb_cc_type_th_reg;    // 0x0000095c
    uint32_t isp_wrapper_ctrl_1;    // 0x00000960
    uint32_t top_dummy;             // 0x00000964
    uint32_t left_dummy;            // 0x00000968
    uint32_t isp_wrapper_ctrl_2;    // 0x0000096c
    uint32_t line_num_l_reg;        // 0x00000970
    uint32_t pix_num_l_reg;         // 0x00000974
    uint32_t v_dummy;               // 0x00000978
    uint32_t scg;                   // 0x0000097c
    uint32_t y_gamma_b0;            // 0x00000980
    uint32_t y_gamma_b1;            // 0x00000984
    uint32_t y_gamma_b2;            // 0x00000988
    uint32_t y_gamma_b4;            // 0x0000098c
    uint32_t y_gamma_b6;            // 0x00000990
    uint32_t y_gamma_b8;            // 0x00000994
    uint32_t y_gamma_b10;           // 0x00000998
    uint32_t y_gamma_b12;           // 0x0000099c
    uint32_t y_gamma_b16;           // 0x000009a0
    uint32_t y_gamma_b20;           // 0x000009a4
    uint32_t y_gamma_b24;           // 0x000009a8
    uint32_t y_gamma_b28;           // 0x000009ac
    uint32_t y_gamma_b32;           // 0x000009b0
    uint32_t r_awb_gain_in;         // 0x000009b4
    uint32_t g_awb_gain_in;         // 0x000009b8
    uint32_t b_awb_gain_in;         // 0x000009bc
    uint32_t r_drc_gain_in;         // 0x000009c0
    uint32_t gr_drc_gain_in;        // 0x000009c4
    uint32_t gb_drc_gain_in;        // 0x000009c8
    uint32_t b_drc_gain_in;         // 0x000009cc
    uint32_t ae_ctrl;               // 0x000009d0
    uint32_t ae_ctrl2;              // 0x000009d4
    uint32_t ae_ctrl3;              // 0x000009d8
    uint32_t ae_ctrl4;              // 0x000009dc
    uint32_t ae_win_start;          // 0x000009e0
    uint32_t ae_win_width;          // 0x000009e4
    uint32_t ae_win_height;         // 0x000009e8
    uint32_t exp_init;              // 0x000009ec
    uint32_t exp_ceil_init;         // 0x000009f0
    uint32_t ae_exp_1e;             // 0x000009f4
    uint32_t ae_diff_thr;           // 0x000009f8
    uint32_t ae_bh_sel;             // 0x000009fc
    uint32_t awb_ctrl;              // 0x00000a00
    uint32_t awb_ctrl2;             // 0x00000a04
    uint32_t awb_y_max;             // 0x00000a08
    uint32_t awb_stop;              // 0x00000a0c
    uint32_t awb_algo;              // 0x00000a10
    uint32_t awb_ctrl3;             // 0x00000a14
    uint32_t awb_ctrl4;             // 0x00000a18
    uint32_t dig_gain_in;           // 0x00000a1c
    uint32_t y_init_thr;            // 0x00000a20
    uint32_t y_ave_target;          // 0x00000a24
    uint32_t y_lmt_offset;          // 0x00000a28
    uint32_t again_sel_th2;         // 0x00000a2c
    uint32_t yave_target_chg1;      // 0x00000a30
    uint32_t image_eff_reg;         // 0x00000a34
    uint32_t ywave_out;             // 0x00000a38
    uint32_t ae_bright_hist;        // 0x00000a3c
    uint32_t yave_out;              // 0x00000a40
    uint32_t exp_out;               // 0x00000a44
    uint32_t misc_out;              // 0x00000a48
    uint32_t awb_debug_out;         // 0x00000a4c
    uint32_t mono_color;            // 0x00000a50
    uint32_t r_awb_gain;            // 0x00000a54
    uint32_t b_awb_gain;            // 0x00000a58
    uint32_t misc_status;           // 0x00000a5c
    uint32_t yave_contr;            // 0x00000a60
    uint32_t gamma_type;            // 0x00000a64
    uint32_t blc_line;              // 0x00000a68
    uint32_t lsc_xx;                // 0x00000a6c
    uint32_t lsc_blc_gain_th;       // 0x00000a70
    uint32_t blc_ctrl;              // 0x00000a74
    uint32_t blc_init;              // 0x00000a78
    uint32_t blc_offset;            // 0x00000a7c
    uint32_t blc_thr;               // 0x00000a80
    uint32_t lsc_xy_cent;           // 0x00000a84
    uint32_t cnr_dif_thr;           // 0x00000a88
    uint32_t cnr_thr;               // 0x00000a8c
    uint32_t gamma_ctrl;            // 0x00000a90
    uint32_t bayer_gamma_b0;        // 0x00000a94
    uint32_t bayer_gamma_b1;        // 0x00000a98
    uint32_t bayer_gamma_b2;        // 0x00000a9c
    uint32_t bayer_gamma_b3;        // 0x00000aa0
    uint32_t bayer_gamma_b4;        // 0x00000aa4
    uint32_t bayer_gamma_b6;        // 0x00000aa8
    uint32_t bayer_gamma_b8;        // 0x00000aac
    uint32_t bayer_gamma_b10;       // 0x00000ab0
    uint32_t bayer_gamma_b12;       // 0x00000ab4
    uint32_t bayer_gamma_b16;       // 0x00000ab8
    uint32_t bayer_gamma_b20;       // 0x00000abc
    uint32_t bayer_gamma_b24;       // 0x00000ac0
    uint32_t bayer_gamma_b28;       // 0x00000ac4
    uint32_t bayer_gamma_b32;       // 0x00000ac8
    uint32_t bayer_gamma_b36;       // 0x00000acc
    uint32_t bayer_gamma_b40;       // 0x00000ad0
    uint32_t bayer_gamma_b48;       // 0x00000ad4
    uint32_t bayer_gamma_b56;       // 0x00000ad8
    uint32_t bayer_gamma_b64;       // 0x00000adc
    uint32_t blc_out0;              // 0x00000ae0
    uint32_t blc_out1;              // 0x00000ae4
    uint32_t dpc_ctrl_0;            // 0x00000ae8
    uint32_t dpc_ctrl_1;            // 0x00000aec
    uint32_t y_thr_lo;              // 0x00000af0
    uint32_t y_thr_mid;             // 0x00000af4
    uint32_t y_thr_hi;              // 0x00000af8
    uint32_t intp_cfa_hv;           // 0x00000afc
    uint32_t manual_adj;            // 0x00000b00
    uint32_t dpc_int_thr_lo;        // 0x00000b04
    uint32_t dpc_int_thr_hi;        // 0x00000b08
    uint32_t again_sel_th1;         // 0x00000b0c
    uint32_t dpc_nr_lf_str_lo;      // 0x00000b10
    uint32_t dpc_nr_hf_str_lo;      // 0x00000b14
    uint32_t dpc_nr_area_thr_lo;    // 0x00000b18
    uint32_t dpc_nr_lf_str_mid;     // 0x00000b1c
    uint32_t dpc_nr_hf_str_mid;     // 0x00000b20
    uint32_t dpc_nr_area_thr_mid;   // 0x00000b24
    uint32_t dpc_nr_lf_str_hi;      // 0x00000b28
    uint32_t dpc_nr_hf_str_hi;      // 0x00000b2c
    uint32_t dpc_nr_area_thr_hi;    // 0x00000b30
    uint32_t intp_ctrl;             // 0x00000b34
    uint32_t intp_cfa_h_thr;        // 0x00000b38
    uint32_t intp_cfa_v_thr;        // 0x00000b3c
    uint32_t intp_grgb_sel_lmt;     // 0x00000b40
    uint32_t intp_gf_lmt_thr;       // 0x00000b44
    uint32_t cc_r_offset;           // 0x00000b48
    uint32_t cc_g_offset;           // 0x00000b4c
    uint32_t cc_b_offset;           // 0x00000b50
    uint32_t cc_00;                 // 0x00000b54
    uint32_t cc_01;                 // 0x00000b58
    uint32_t cc_10;                 // 0x00000b5c
    uint32_t cc_11;                 // 0x00000b60
    uint32_t cc_20;                 // 0x00000b64
    uint32_t cc_21;                 // 0x00000b68
    uint32_t cc_r_offset_post;      // 0x00000b6c
    uint32_t cc_g_offset_post;      // 0x00000b70
    uint32_t cc_b_offset_post;      // 0x00000b74
    uint32_t cc2_r_offset;          // 0x00000b78
    uint32_t cc2_g_offset;          // 0x00000b7c
    uint32_t cc2_b_offset;          // 0x00000b80
    uint32_t cc2_00;                // 0x00000b84
    uint32_t cc2_01;                // 0x00000b88
    uint32_t cc2_10;                // 0x00000b8c
    uint32_t cc2_11;                // 0x00000b90
    uint32_t cc2_20;                // 0x00000b94
    uint32_t cc2_21;                // 0x00000b98
    uint32_t sharp_lmt;             // 0x00000b9c
    uint32_t sharp_mode;            // 0x00000ba0
    uint32_t sharp_gain_str_lo;     // 0x00000ba4
    uint32_t sharp_nr_area_thr_lo;  // 0x00000ba8
    uint32_t sharp_gain_str_mid;    // 0x00000bac
    uint32_t sharp_nr_area_thr_mid; // 0x00000bb0
    uint32_t sharp_gain_str_hi;     // 0x00000bb4
    uint32_t sharp_nr_area_thr_hi;  // 0x00000bb8
    uint32_t ynr_ctrl_reg;          // 0x00000bbc
    uint32_t ynr_lf_method_str;     // 0x00000bc0
    uint32_t ynr_lf_str_lo;         // 0x00000bc4
    uint32_t ynr_hf_str_lo;         // 0x00000bc8
    uint32_t ynr_area_thr_lo;       // 0x00000bcc
    uint32_t ynr_lf_str_mid;        // 0x00000bd0
    uint32_t ynr_hf_str_mid;        // 0x00000bd4
    uint32_t ynr_area_thr_mid;      // 0x00000bd8
    uint32_t ynr_lf_str_hi;         // 0x00000bdc
    uint32_t ynr_hf_str_hi;         // 0x00000be0
    uint32_t ynr_area_thr_hi;       // 0x00000be4
    uint32_t hue_sin_reg;           // 0x00000be8
    uint32_t hue_cos_reg;           // 0x00000bec
    uint32_t cnr_1d_ctrl_reg;       // 0x00000bf0
    uint32_t cnr_xx_reg;            // 0x00000bf4
    uint32_t in5_low_th_reg;        // 0x00000bf8
    uint32_t in5_high_th_reg;       // 0x00000bfc
    uint32_t __3072[72];            // 0x00000c00
    uint32_t p2_up_r_reg;           // 0x00000d20
    uint32_t p2_up_g_reg;           // 0x00000d24
    uint32_t p2_up_b_reg;           // 0x00000d28
    uint32_t p2_down_r_reg;         // 0x00000d2c
    uint32_t p2_down_g_reg;         // 0x00000d30
    uint32_t p2_down_b_reg;         // 0x00000d34
    uint32_t p2_left_r_reg;         // 0x00000d38
    uint32_t p2_left_g_reg;         // 0x00000d3c
    uint32_t p2_left_b_reg;         // 0x00000d40
    uint32_t p2_right_r_reg;        // 0x00000d44
    uint32_t p2_right_g_reg;        // 0x00000d48
    uint32_t p2_right_b_reg;        // 0x00000d4c
    uint32_t p4_q1_r_reg;           // 0x00000d50
    uint32_t p4_q1_g_reg;           // 0x00000d54
    uint32_t p4_q1_b_reg;           // 0x00000d58
    uint32_t p4_q2_r_reg;           // 0x00000d5c
    uint32_t p4_q2_g_reg;           // 0x00000d60
    uint32_t p4_q2_b_reg;           // 0x00000d64
    uint32_t p4_q3_r_reg;           // 0x00000d68
    uint32_t p4_q3_g_reg;           // 0x00000d6c
    uint32_t p4_q3_b_reg;           // 0x00000d70
    uint32_t p4_q4_r_reg;           // 0x00000d74
    uint32_t p4_q4_g_reg;           // 0x00000d78
    uint32_t p4_q4_b_reg;           // 0x00000d7c
    uint32_t ae_e00_sta_reg;        // 0x00000d80
    uint32_t ae_e00_num_reg;        // 0x00000d84
    uint32_t ae_e01_sta_reg;        // 0x00000d88
    uint32_t ae_e01_num_reg;        // 0x00000d8c
    uint32_t ae_e02_sta_reg;        // 0x00000d90
    uint32_t ae_e02_num_reg;        // 0x00000d94
    uint32_t ae_e1_sta_reg;         // 0x00000d98
    uint32_t ae_e1_num_reg;         // 0x00000d9c
    uint32_t ae_e2_sta_reg;         // 0x00000da0
    uint32_t ae_e2_num_reg;         // 0x00000da4
    uint32_t ae_e3_sta_reg;         // 0x00000da8
    uint32_t ae_e3_num_reg;         // 0x00000dac
    uint32_t ae_e4_sta_reg;         // 0x00000db0
    uint32_t ae_e4_num_reg;         // 0x00000db4
    uint32_t ae_e5_sta_reg;         // 0x00000db8
    uint32_t ae_e5_num_reg;         // 0x00000dbc
    uint32_t ae_e6_sta_reg;         // 0x00000dc0
    uint32_t ae_e6_num_reg;         // 0x00000dc4
    uint32_t ae_e7_sta_reg;         // 0x00000dc8
    uint32_t ae_e7_num_reg;         // 0x00000dcc
    uint32_t ae_e8_sta_reg;         // 0x00000dd0
    uint32_t ae_e8_num_reg;         // 0x00000dd4
    uint32_t ae_e9_sta_reg;         // 0x00000dd8
    uint32_t ae_e9_num_reg;         // 0x00000ddc
    uint32_t ae_ea_sta_reg;         // 0x00000de0
    uint32_t ae_ea_num_reg;         // 0x00000de4
    uint32_t ae_eb_sta_reg;         // 0x00000de8
    uint32_t ae_eb_num_reg;         // 0x00000dec
    uint32_t ae_ec_sta_reg;         // 0x00000df0
    uint32_t ae_ec_num_reg;         // 0x00000df4
    uint32_t ae_ed_sta_reg;         // 0x00000df8
    uint32_t ae_ed_num_reg;         // 0x00000dfc
    uint32_t bayer_gamma2_b0;       // 0x00000e00
    uint32_t bayer_gamma2_b1;       // 0x00000e04
    uint32_t bayer_gamma2_b2;       // 0x00000e08
    uint32_t bayer_gamma2_b3;       // 0x00000e0c
    uint32_t bayer_gamma2_b4;       // 0x00000e10
    uint32_t bayer_gamma2_b6;       // 0x00000e14
    uint32_t bayer_gamma2_b8;       // 0x00000e18
    uint32_t bayer_gamma2_b10;      // 0x00000e1c
    uint32_t bayer_gamma2_b12;      // 0x00000e20
    uint32_t bayer_gamma2_b16;      // 0x00000e24
    uint32_t bayer_gamma2_b20;      // 0x00000e28
    uint32_t bayer_gamma2_b24;      // 0x00000e2c
    uint32_t bayer_gamma2_b28;      // 0x00000e30
    uint32_t bayer_gamma2_b32;      // 0x00000e34
    uint32_t bayer_gamma2_b36;      // 0x00000e38
    uint32_t bayer_gamma2_b40;      // 0x00000e3c
    uint32_t bayer_gamma2_b48;      // 0x00000e40
    uint32_t bayer_gamma2_b56;      // 0x00000e44
    uint32_t bayer_gamma2_b64;      // 0x00000e48
    uint32_t y_thr7_lo_reg;         // 0x00000e4c
    uint32_t y_thr7_mid_reg;        // 0x00000e50
    uint32_t y_thr7_hi_reg;         // 0x00000e54
    uint32_t dpa_new_ctrl_reg;      // 0x00000e58
    uint32_t dpa_new_ctrl_hi_reg;   // 0x00000e5c
    uint32_t ae_index_gap;          // 0x00000e60
    uint32_t awb_calc_height_reg;   // 0x00000e64
    uint32_t drc_r_clp_value_reg;   // 0x00000e68
    uint32_t drc_gr_clp_value_reg;  // 0x00000e6c
    uint32_t drc_gb_clp_value_reg;  // 0x00000e70
    uint32_t drc_b_clp_value_reg;   // 0x00000e74
    uint32_t sepia_cr_reg;          // 0x00000e78
    uint32_t sepia_cb_reg;          // 0x00000e7c
    uint32_t csup_y_min_hi_reg;     // 0x00000e80
    uint32_t csup_gain_hi_reg;      // 0x00000e84
    uint32_t csup_y_max_low_reg;    // 0x00000e88
    uint32_t csup_gain_low_reg;     // 0x00000e8c
    uint32_t ae_dk_hist_thr_reg;    // 0x00000e90
    uint32_t ae_br_hist_thr_reg;    // 0x00000e94
    uint32_t hist_bp_level_reg;     // 0x00000e98
    uint32_t outdoor_th_reg;        // 0x00000e9c
    uint32_t awb_rgain_low_reg;     // 0x00000ea0
    uint32_t awb_rgain_high_reg;    // 0x00000ea4
    uint32_t awb_bgain_low_reg;     // 0x00000ea8
    uint32_t awb_bgain_high_reg;    // 0x00000eac
    uint32_t awb_calc_start_reg;    // 0x00000eb0
    uint32_t awb_calc_width_reg;    // 0x00000eb4
    uint32_t hist_dp_level_reg;     // 0x00000eb8
    uint32_t awb_y_fmin;            // 0x00000ebc
    uint32_t awb_y_fmax;            // 0x00000ec0
    uint32_t awb_cb_fmin;           // 0x00000ec4
    uint32_t awb_cb_fmax;           // 0x00000ec8
    uint32_t awb_cr_fmin;           // 0x00000ecc
    uint32_t awb_cr_fmax;           // 0x00000ed0
    uint32_t awb_y_fmin2;           // 0x00000ed4
    uint32_t awb_y_fmax2;           // 0x00000ed8
    uint32_t awb_cb_fmin2;          // 0x00000edc
    uint32_t awb_cb_fmax2;          // 0x00000ee0
    uint32_t awb_cr_fmin2;          // 0x00000ee4
    uint32_t awb_cr_fmax2;          // 0x00000ee8
    uint32_t ae_use_mean;           // 0x00000eec
    uint32_t ae_weight_sta;         // 0x00000ef0
    uint32_t ae_qwidth;             // 0x00000ef4
    uint32_t ae_qheight;            // 0x00000ef8
    uint32_t ae_win_sta;            // 0x00000efc
    uint32_t ae_width;              // 0x00000f00
    uint32_t ae_height;             // 0x00000f04
    uint32_t sw_update;             // 0x00000f08
    uint32_t awb_ctrl5;             // 0x00000f0c
    uint32_t awb_ctrl6;             // 0x00000f10
    uint32_t sca_reg;               // 0x00000f14
    uint32_t ae_ee_sta_reg;         // 0x00000f18
    uint32_t ae_ee_num_reg;         // 0x00000f1c
    uint32_t ae_ef_sta_reg;         // 0x00000f20
    uint32_t ae_ef_num_reg;         // 0x00000f24
    uint32_t ae_thr_big_reg;        // 0x00000f28
    uint32_t sharp_gain_minus_low;  // 0x00000f2c
    uint32_t sharp_gain_minus_mid;  // 0x00000f30
    uint32_t sharp_gain_minus_hi;   // 0x00000f34
    uint32_t sharp_mode_mid_hi;     // 0x00000f38
    uint32_t fw_version_reg;        // 0x00000f3c
    uint32_t awb_y_min_reg;         // 0x00000f40
    uint32_t y_red_coef_reg;        // 0x00000f44
    uint32_t y_blue_coef_reg;       // 0x00000f48
    uint32_t cb_red_coef_reg;       // 0x00000f4c
    uint32_t cr_blue_coef_reg;      // 0x00000f50
    uint32_t hist_vbp_level_reg;    // 0x00000f54
    uint32_t hist_vdp_level_reg;    // 0x00000f58
    uint32_t __3932[40];            // 0x00000f5c
} HWP_CAMERA_T;

#define hwp_camera ((HWP_CAMERA_T *)REG_ACCESS_ADDRESS(REG_CAMERA_BASE))

// ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;        // [0]
        uint32_t dctenable : 1;     // [1]
        uint32_t buf_enable : 1;    // [2]
        uint32_t rgb_rfirst : 1;    // [3]
        uint32_t dataformat : 2;    // [5:4]
        uint32_t cfg_cam_c2cse : 2; // [7:6]
        uint32_t reset_pol : 1;     // [8]
        uint32_t pwdn_pol : 1;      // [9]
        uint32_t vsync_pol : 1;     // [10]
        uint32_t href_pol : 1;      // [11]
        uint32_t pixclk_pol : 1;    // [12]
        uint32_t __13_13 : 1;       // [13]
        uint32_t vsync_drop : 1;    // [14]
        uint32_t __15_15 : 1;       // [15]
        uint32_t decimfrm : 2;      // [17:16]
        uint32_t decimcol : 2;      // [19:18]
        uint32_t decimrow : 2;      // [21:20]
        uint32_t __23_22 : 2;       // [23:22]
        uint32_t reorder : 3;       // [26:24]
        uint32_t __27_27 : 1;       // [27]
        uint32_t cropen : 1;        // [28]
        uint32_t __29_29 : 1;       // [29]
        uint32_t bist_mode : 1;     // [30]
        uint32_t test : 1;          // [31]
    } b;
} REG_CAMERA_CTRL_T;

// status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ovfl : 1;       // [0], read only
        uint32_t vsync_r : 1;    // [1], read only
        uint32_t vsync_f : 1;    // [2], read only
        uint32_t dma_done : 1;   // [3], read only
        uint32_t fifo_empty : 1; // [4], read only
        uint32_t spi_ovfl : 1;   // [5], read only
        uint32_t __31_6 : 26;    // [31:6]
    } b;
} REG_CAMERA_STATUS_T;

// irq_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ovfl : 1;     // [0]
        uint32_t vsync_r : 1;  // [1]
        uint32_t vsync_f : 1;  // [2]
        uint32_t dma_done : 1; // [3]
        uint32_t __31_4 : 28;  // [31:4]
    } b;
} REG_CAMERA_IRQ_MASK_T;

// irq_clear
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ovfl : 1;     // [0]
        uint32_t vsync_r : 1;  // [1]
        uint32_t vsync_f : 1;  // [2]
        uint32_t dma_done : 1; // [3]
        uint32_t __31_4 : 28;  // [31:4]
    } b;
} REG_CAMERA_IRQ_CLEAR_T;

// irq_cause
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ovfl : 1;     // [0], read only
        uint32_t vsync_r : 1;  // [1], read only
        uint32_t vsync_f : 1;  // [2], read only
        uint32_t dma_done : 1; // [3], read only
        uint32_t __31_4 : 28;  // [31:4]
    } b;
} REG_CAMERA_IRQ_CAUSE_T;

// cmd_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pwdn : 1;       // [0], write set
        uint32_t __3_1 : 3;      // [3:1]
        uint32_t reset : 1;      // [4], write set
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t fifo_reset : 1; // [8], write set
        uint32_t __31_9 : 23;    // [31:9]
    } b;
} REG_CAMERA_CMD_SET_T;

// cmd_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pwdn : 1;    // [0], write clear
        uint32_t __3_1 : 3;   // [3:1]
        uint32_t reset : 1;   // [4], write clear
        uint32_t __31_5 : 27; // [31:5]
    } b;
} REG_CAMERA_CMD_CLR_T;

// dstwincol
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dstwincolstart : 12; // [11:0]
        uint32_t __15_12 : 4;         // [15:12]
        uint32_t dstwincolend : 12;   // [27:16]
        uint32_t __31_28 : 4;         // [31:28]
    } b;
} REG_CAMERA_DSTWINCOL_T;

// dstwinrow
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dstwinrowstart : 12; // [11:0]
        uint32_t __15_12 : 4;         // [15:12]
        uint32_t dstwinrowend : 12;   // [27:16]
        uint32_t __31_28 : 4;         // [31:28]
    } b;
} REG_CAMERA_DSTWINROW_T;

// scl_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t scale_en : 1;      // [0]
        uint32_t __3_1 : 3;         // [3:1]
        uint32_t data_out_swap : 1; // [4]
        uint32_t __7_5 : 3;         // [7:5]
        uint32_t scale_col : 2;     // [9:8]
        uint32_t __15_10 : 6;       // [15:10]
        uint32_t scale_row : 2;     // [17:16]
        uint32_t __31_18 : 14;      // [31:18]
    } b;
} REG_CAMERA_SCL_CONFIG_T;

// spi_camera_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t camera_spi_slave_en : 1;             // [0]
        uint32_t camera_spi_master_en : 1;            // [1]
        uint32_t yuv_out_format : 3;                  // [4:2]
        uint32_t overflow_rstn_only_vsync_low : 1;    // [5]
        uint32_t overflow_observe_only_vsync_low : 1; // [6]
        uint32_t overflow_rstn_en : 1;                // [7]
        uint32_t big_end_dis : 1;                     // [8]
        uint32_t overflow_inv : 1;                    // [9]
        uint32_t href_inv : 1;                        // [10]
        uint32_t vsync_inv : 1;                       // [11]
        uint32_t block_num_per_line : 10;             // [21:12]
        uint32_t line_num_per_frame : 10;             // [31:22]
    } b;
} REG_CAMERA_SPI_CAMERA_REG0_T;

// spi_camera_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t camera_clk_div_num : 16; // [15:0]
        uint32_t cts_spi_master_reg : 1;  // [16]
        uint32_t ssn_cm_inv : 1;          // [17]
        uint32_t sck_cm_inv : 1;          // [18]
        uint32_t ssn_spi_oenb_dr : 1;     // [19]
        uint32_t ssn_spi_oenb_reg : 1;    // [20]
        uint32_t sck_spi_oenb_dr : 1;     // [21]
        uint32_t sck_spi_oenb_reg : 1;    // [22]
        uint32_t __28_23 : 6;             // [28:23]
        uint32_t sdo_spi_swap : 1;        // [29]
        uint32_t clk_inv : 1;             // [30]
        uint32_t sck_ddr_en : 1;          // [31]
    } b;
} REG_CAMERA_SPI_CAMERA_REG1_T;

// spi_camera_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ssn_wait_length : 8;         // [7:0]
        uint32_t init_wait_length : 8;        // [15:8]
        uint32_t word_num_per_block : 8;      // [23:16]
        uint32_t ssn_cs_delay : 2;            // [25:24]
        uint32_t data_receive_choose_bit : 2; // [27:26]
        uint32_t ready_cs_inv : 1;            // [28]
        uint32_t ssn_cs_inv : 1;              // [29]
        uint32_t __30_30 : 1;                 // [30]
        uint32_t eco_bypass_isp : 1;          // [31]
    } b;
} REG_CAMERA_SPI_CAMERA_REG2_T;

// spi_camera_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t line_wait_length : 16; // [15:0]
        uint32_t block_wait_length : 8; // [23:16]
        uint32_t ssn_high_length : 8;   // [31:24]
    } b;
} REG_CAMERA_SPI_CAMERA_REG3_T;

// spi_camera_reg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t camera_spi_master_en_2 : 1;  // [0]
        uint32_t sdo_line_choose_bit : 2;     // [2:1]
        uint32_t data_size_choose_bit : 1;    // [3]
        uint32_t image_height_choose_bit : 1; // [4]
        uint32_t image_width_choose_bit : 1;  // [5]
        uint32_t block_num_per_packet : 10;   // [15:6]
        uint32_t spi_data0_phase_sel : 2;     // [17:16]
        uint32_t spi_data1_phase_sel : 2;     // [19:18]
        uint32_t __31_20 : 12;                // [31:20]
    } b;
} REG_CAMERA_SPI_CAMERA_REG4_T;

// spi_camera_reg5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sync_code : 24; // [23:0]
        uint32_t __31_24 : 8;    // [31:24]
    } b;
} REG_CAMERA_SPI_CAMERA_REG5_T;

// spi_camera_reg6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t packet_id_data_start : 8;  // [7:0]
        uint32_t packet_id_line_start : 8;  // [15:8]
        uint32_t packet_id_frame_end : 8;   // [23:16]
        uint32_t packet_id_frame_start : 8; // [31:24]
    } b;
} REG_CAMERA_SPI_CAMERA_REG6_T;

// spi_camera_obs0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t line_id_15_0_ : 16;             // [15:0], read only
        uint32_t data_id_7_0_ : 8;               // [23:16], read only
        uint32_t observe_data_size_wrong : 1;    // [24], read only
        uint32_t observe_image_height_wrong : 1; // [25], read only
        uint32_t observe_image_width_wrong : 1;  // [26], read only
        uint32_t observe_line_num_wrong : 1;     // [27], read only
        uint32_t observe_data_id_wrong : 1;      // [28], read only
        uint32_t __31_29 : 3;                    // [31:29]
    } b;
} REG_CAMERA_SPI_CAMERA_OBS0_T;

// spi_camera_obs1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t image_height : 16; // [15:0], read only
        uint32_t image_width : 16;  // [31:16], read only
    } b;
} REG_CAMERA_SPI_CAMERA_OBS1_T;

// csi_config_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t num_d_term_en : 8;               // [7:0]
        uint32_t cur_frame_line_num : 13;         // [20:8]
        uint32_t data_lp_in_choose_bit : 2;       // [22:21]
        uint32_t clk_lp_inv : 1;                  // [23]
        uint32_t trail_data_wrong_choose_bit : 1; // [24]
        uint32_t sync_bypass : 1;                 // [25]
        uint32_t rdata_bit_inv_en : 1;            // [26]
        uint32_t hs_sync_find_en : 1;             // [27]
        uint32_t line_packet_enable : 1;          // [28]
        uint32_t ecc_bypass : 1;                  // [29]
        uint32_t data_lane_choose_bit : 1;        // [30]
        uint32_t csi_module_enable : 1;           // [31]
    } b;
} REG_CAMERA_CSI_CONFIG_REG0_T;

// csi_config_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t num_hs_settle : 8;              // [7:0]
        uint32_t lp_data_length_choose_bit : 3;  // [10:8]
        uint32_t data_clk_lp_posedge_choose : 3; // [13:11]
        uint32_t clk_lp_ck_inv : 1;              // [14]
        uint32_t rclr_mask_en : 1;               // [15]
        uint32_t rinc_mask_en : 1;               // [16]
        uint32_t hs_enable_mask_en : 1;          // [17]
        uint32_t den_csi_inv_bit : 1;            // [18]
        uint32_t hsync_csi_inv_bit : 1;          // [19]
        uint32_t vsync_csi_inv_bit : 1;          // [20]
        uint32_t hs_data2_enable_reg : 1;        // [21]
        uint32_t hs_data1_enable_reg : 1;        // [22]
        uint32_t hs_data1_enable_choose_bit : 1; // [23]
        uint32_t hs_data1_enable_dr : 1;         // [24]
        uint32_t data2_terminal_enable_reg : 1;  // [25]
        uint32_t data1_terminal_enable_reg : 1;  // [26]
        uint32_t data1_terminal_enable_dr : 1;   // [27]
        uint32_t lp_data_interrupt_clr : 1;      // [28]
        uint32_t lp_cmd_interrupt_clr : 1;       // [29]
        uint32_t lp_data_clr : 1;                // [30]
        uint32_t lp_cmd_clr : 1;                 // [31]
    } b;
} REG_CAMERA_CSI_CONFIG_REG1_T;

// csi_config_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t num_hs_settle_clk : 16; // [15:0]
        uint32_t num_c_term_en : 16;     // [31:16]
    } b;
} REG_CAMERA_CSI_CONFIG_REG2_T;

// csi_config_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __5_0 : 6;                     // [5:0]
        uint32_t clk_lp_in_choose_bit : 2;      // [7:6]
        uint32_t pu_lprx_reg : 1;               // [8]
        uint32_t pu_hsrx_reg : 1;               // [9]
        uint32_t pu_dr : 1;                     // [10]
        uint32_t data_pnsw_reg : 1;             // [11]
        uint32_t hs_clk_enable_reg : 1;         // [12]
        uint32_t hs_clk_enable_choose_bit : 1;  // [13]
        uint32_t hs_clk_enable_dr : 1;          // [14]
        uint32_t clk_terminal_enable_reg : 1;   // [15]
        uint32_t clk_terminal_enable_dr : 1;    // [16]
        uint32_t observe_reg_5_low8_choose : 1; // [17]
        uint32_t ecc_error_flag_reg : 1;        // [18]
        uint32_t ecc_error_dr : 1;              // [19]
        uint32_t csi_channel_sel : 1;           // [20]
        uint32_t two_lane_bit_reverse : 1;      // [21]
        uint32_t data2_lane_bit_reverse : 1;    // [22]
        uint32_t data1_lane_bit_reverse : 1;    // [23]
        uint32_t data2_hs_no_mask : 1;          // [24]
        uint32_t data1_hs_no_mask : 1;          // [25]
        uint32_t pu_lprx_d2_reg : 1;            // [26]
        uint32_t pu_lprx_d1_reg : 1;            // [27]
        uint32_t __28_28 : 1;                   // [28]
        uint32_t clk_edge_sel : 1;              // [29]
        uint32_t clk_x2_sel : 1;                // [30]
        uint32_t single_data_lane_en : 1;       // [31]
    } b;
} REG_CAMERA_CSI_CONFIG_REG3_T;

// csi_config_reg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t num_hs_clk_useful : 31;   // [30:0]
        uint32_t num_hs_clk_useful_en : 1; // [31]
    } b;
} REG_CAMERA_CSI_CONFIG_REG4_T;

// csi_config_reg5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vc_id_set : 2;                 // [1:0]
        uint32_t data_lp_inv : 1;               // [2]
        uint32_t fifo_rclr_8809p_reg : 1;       // [3]
        uint32_t fifo_wclr_8809p_reg : 1;       // [4]
        uint32_t hs_sync_16bit_8809p_mode : 1;  // [5]
        uint32_t d_term_small_8809p_en : 1;     // [6]
        uint32_t data_line_inv_8809p_en : 1;    // [7]
        uint32_t hs_enable_8809p_mode : 1;      // [8]
        uint32_t sp_to_trail_8809p_en : 1;      // [9]
        uint32_t trail_wrong_8809p_bypass : 1;  // [10]
        uint32_t rinc_trail_8809p_bypass : 1;   // [11]
        uint32_t hs_data_enable_8809p_mode : 1; // [12]
        uint32_t hs_clk_enable_8809p_mode : 1;  // [13]
        uint32_t data_type_re_check_en : 1;     // [14]
        uint32_t sync_id_reg : 8;               // [22:15]
        uint32_t sync_id_dr : 1;                // [23]
        uint32_t csi_observe_choose_bit : 5;    // [28:24]
        uint32_t crc_error_flag_reg : 1;        // [29]
        uint32_t crc_error_flag_dr : 1;         // [30]
        uint32_t csi_rinc_new_mode_dis : 1;     // [31]
    } b;
} REG_CAMERA_CSI_CONFIG_REG5_T;

// csi_config_reg6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data_type_dp_reg : 6; // [5:0]
        uint32_t data_type_le_reg : 6; // [11:6]
        uint32_t data_type_ls_reg : 6; // [17:12]
        uint32_t data_type_fe_reg : 6; // [23:18]
        uint32_t data_type_fs_reg : 6; // [29:24]
        uint32_t data_type_dp_dr : 1;  // [30]
        uint32_t data_type_dr : 1;     // [31]
    } b;
} REG_CAMERA_CSI_CONFIG_REG6_T;

// csi_config_reg7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;                          // [1:0]
        uint32_t data_lane_16bits_mode : 1;          // [2]
        uint32_t terminal_2_hs_exchage_8809p : 1;    // [3]
        uint32_t terminal_1_hs_exchage_8809p : 1;    // [4]
        uint32_t data2_terminal_enable_8809p_dr : 1; // [5]
        uint32_t hs_data2_enable_8809p_dr : 1;       // [6]
        uint32_t csi_dout_test_8809p_en : 1;         // [7]
        uint32_t csi_dout_test_8809p : 8;            // [15:8]
        uint32_t num_d_term_en : 8;                  // [23:16]
        uint32_t num_hs_settle : 8;                  // [31:24]
    } b;
} REG_CAMERA_CSI_CONFIG_REG7_T;

// csi_obs4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hs_data_state : 14;    // [13:0]
        uint32_t phy_data_state : 15;   // [28:14]
        uint32_t fifo_wfull_almost : 1; // [29]
        uint32_t fifo_wfull : 1;        // [30]
        uint32_t fifo_wempty : 1;       // [31]
    } b;
} REG_CAMERA_CSI_OBS4_T;

// csi_obs5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t csi_observe_reg_5_low : 8;   // [7:0], read only
        uint32_t lp_data_interrupt_flag : 1;  // [8], read only
        uint32_t lp_cmd_interrupt_flag : 1;   // [9], read only
        uint32_t phy_clk_state : 9;           // [18:10], read only
        uint32_t fifo_rcount : 9;             // [27:19], read only
        uint32_t crc_error : 1;               // [28], read only
        uint32_t err_ecc_corrected_flag : 1;  // [29], read only
        uint32_t err_data_corrected_flag : 1; // [30], read only
        uint32_t err_data_zero_flag : 1;      // [31], read only
    } b;
} REG_CAMERA_CSI_OBS5_T;

// csi_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t csi_enable : 1; // [0]
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_CAMERA_CSI_ENABLE_T;

// csi_config_reg8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dly_sel_clkn_reg : 4;  // [3:0]
        uint32_t dly_sel_clkp_reg : 4;  // [7:4]
        uint32_t dly_sel_data2_reg : 4; // [11:8]
        uint32_t dly_sel_data1_reg : 4; // [15:12]
        uint32_t vth_sel : 1;           // [16]
        uint32_t __31_17 : 15;          // [31:17]
    } b;
} REG_CAMERA_CSI_CONFIG_REG8_T;

// soft_reset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dsp_reset : 1; // [0]
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CAMERA_SOFT_RESET_T;

// awb_x1_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x1_min : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_X1_MIN_T;

// awb_x1_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x1_max : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_X1_MAX_T;

// awb_y1_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y1_min : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y1_MIN_T;

// awb_y1_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y1_max : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y1_MAX_T;

// awb_x2_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x2_min : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_X2_MIN_T;

// awb_x2_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x2_max : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_X2_MAX_T;

// awb_y2_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y2_min : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y2_MIN_T;

// awb_y2_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y2_max : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y2_MAX_T;

// awb_x3_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x3_min : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_X3_MIN_T;

// awb_x3_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x3_max : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_X3_MAX_T;

// awb_y3_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y3_min : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y3_MIN_T;

// awb_y3_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y3_max : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y3_MAX_T;

// awb_x4_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x4_min : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_X4_MIN_T;

// awb_x4_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x4_max : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_X4_MAX_T;

// awb_y4_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y4_min : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y4_MIN_T;

// awb_y4_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y4_max : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y4_MAX_T;

// awb_x5_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x5_min : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_X5_MIN_T;

// awb_x5_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x5_max : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_X5_MAX_T;

// awb_y5_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y5_min : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y5_MIN_T;

// awb_y5_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y5_max : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y5_MAX_T;

// awb_skin_x1_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_skin_x1_min : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AWB_SKIN_X1_MIN_T;

// awb_skin_x1_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_skin_x1_max : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AWB_SKIN_X1_MAX_T;

// awb_skin_y1_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_skin_y1_min : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AWB_SKIN_Y1_MIN_T;

// awb_skin_y1_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_skin_y1_max : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AWB_SKIN_Y1_MAX_T;

// awb_skin_x2_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_skin_x2_min : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AWB_SKIN_X2_MIN_T;

// awb_skin_x2_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_skin_x2_max : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AWB_SKIN_X2_MAX_T;

// awb_skin_y2_min
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_skin_y2_min : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AWB_SKIN_Y2_MIN_T;

// awb_skin_y2_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_skin_y2_max : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AWB_SKIN_Y2_MAX_T;

// awb_ctd_msb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_x1_min_msb : 1; // [0]
        uint32_t awb_x1_max_msb : 1; // [1]
        uint32_t awb_y5_min_msb : 1; // [2]
        uint32_t awb_y5_max_msb : 1; // [3]
        uint32_t awb_adj_mode : 2;   // [5:4]
        uint32_t awb_ratio_mode : 2; // [7:6]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_AWB_CTD_MSB_T;

// int_dif_thr_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t int_dif_thr_mid : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_INT_DIF_THR_MID_T;

// lb_soft_rstn
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lb_soft_rstn : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_CAMERA_LB_SOFT_RSTN_T;

// vsync_end_high
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vsync_end_high : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_VSYNC_END_HIGH_T;

// vsync_end_low
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vsync_end_low : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_VSYNC_END_LOW_T;

// line_numl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t line_numl : 8; // [7:0]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_LINE_NUML_T;

// pix_numl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pix_numl : 8; // [7:0]
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_CAMERA_PIX_NUML_T;

// pix_line_numh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t line_numh : 1;      // [0]
        uint32_t pix_numh_rsvd : 3;  // [3:1]
        uint32_t pix_numh : 2;       // [5:4]
        uint32_t line_numh_rsvd : 2; // [7:6]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_PIX_LINE_NUMH_T;

// lb_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t low_order : 1;      // [0]
        uint32_t use_fb_reg : 1;     // [1]
        uint32_t not_cvp_reg : 1;    // [2]
        uint32_t first_byte_reg : 3; // [5:3]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_LB_CTRL_T;

// data_format
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data_format : 2; // [1:0]
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_CAMERA_DATA_FORMAT_T;

// lb_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lb_enable : 1; // [0]
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CAMERA_LB_ENABLE_T;

// vh_inv
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hsync_inv : 1; // [0]
        uint32_t vsync_inv : 1; // [1]
        uint32_t __31_2 : 30;   // [31:2]
    } b;
} REG_CAMERA_VH_INV_T;

// line_cnt_l
typedef union {
    uint32_t v;
    struct
    {
        uint32_t line_cnt_l : 8; // [7:0], read only
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_LINE_CNT_L_T;

// line_cnt_h
typedef union {
    uint32_t v;
    struct
    {
        uint32_t line_cnt_h : 2; // [1:0], read only
        uint32_t __31_2 : 30;    // [31:2]
    } b;
} REG_CAMERA_LINE_CNT_H_T;

// num_check
typedef union {
    uint32_t v;
    struct
    {
        uint32_t line_num_check : 1; // [0], read only
        uint32_t byte_num_check : 1; // [1], read only
        uint32_t __3_2 : 2;          // [3:2]
        uint32_t line_num_clear : 1; // [4]
        uint32_t byte_num_clear : 1; // [5]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_NUM_CHECK_T;

// dci_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t kl_low_light_fix : 1; // [0]
        uint32_t kl_reg_fix : 1;       // [1]
        uint32_t ku_low_light_fix : 1; // [2]
        uint32_t ku_reg_fix : 1;       // [3]
        uint32_t hofst : 2;            // [5:4]
        uint32_t vbh_sel : 2;          // [7:6]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_DCI_CTRL_REG_T;

// dci_ofst_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t kl_ofstx1 : 4; // [3:0]
        uint32_t ku_ofstx1 : 4; // [7:4]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_DCI_OFST_REG_T;

// dci_hist_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dk_histx1 : 4; // [3:0]
        uint32_t br_histx1 : 4; // [7:4]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_DCI_HIST_REG_T;

// ae_sw_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nexp_sw_in : 4;       // [3:0]
        uint32_t __6_4 : 3;            // [6:4]
        uint32_t ae_ext_adj_start : 1; // [7]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_AE_SW_CTRL_REG_T;

// ae_thr_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t thr_dark : 4;   // [3:0]
        uint32_t thr_bright : 4; // [7:4]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AE_THR_REG_T;

// ae_misc_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ofst_dec_low_sel : 2;   // [1:0]
        uint32_t ofst_dec_high_sel : 2;  // [3:2]
        uint32_t force_adj1 : 1;         // [4]
        uint32_t force_adj2 : 1;         // [5]
        uint32_t force_adj3 : 1;         // [6]
        uint32_t index_ofst_no_step : 1; // [7]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_AE_MISC_CTRL_REG_T;

// csup_xx_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t x_low : 4;   // [3:0]
        uint32_t x_high : 4;  // [7:4]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CSUP_XX_REG_T;

// contr_ythr_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t csup_gain_low_th_h : 1; // [0]
        uint32_t csup_gain_high_th : 3;  // [3:1]
        uint32_t fixed_contr_ythr : 4;   // [7:4]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_CONTR_YTHR_REG_T;

// contr_yave_offset_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t yave_offset_reg : 6;  // [5:0]
        uint32_t ythr_sel : 1;         // [6]
        uint32_t yave_offset_sign : 1; // [7]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_CONTR_YAVE_OFFSET_REG_T;

// contr_ku_lo_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ku : 7;      // [6:0]
        uint32_t ku_sign : 1; // [7]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CONTR_KU_LO_REG_T;

// contr_kl_lo_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t kl : 7;      // [6:0]
        uint32_t kl_sign : 1; // [7]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CONTR_KL_LO_REG_T;

// contr_ku_mid_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ku : 7;      // [6:0]
        uint32_t ku_sign : 1; // [7]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CONTR_KU_MID_REG_T;

// contr_kl_mid_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t kl : 7;      // [6:0]
        uint32_t kl_sign : 1; // [7]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CONTR_KL_MID_REG_T;

// contr_ku_hi_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ku : 7;      // [6:0]
        uint32_t ku_sign : 1; // [7]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CONTR_KU_HI_REG_T;

// contr_kl_hi_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t kl : 7;      // [6:0]
        uint32_t kl_sign : 1; // [7]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CONTR_KL_HI_REG_T;

// luma_offset_lo_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t offset : 6;      // [5:0]
        uint32_t algo_sel : 1;    // [6]
        uint32_t offset_sign : 1; // [7]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_LUMA_OFFSET_LO_REG_T;

// luma_offset_mid_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t offset : 6;      // [5:0]
        uint32_t algo_sel : 1;    // [6]
        uint32_t offset_sign : 1; // [7]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_LUMA_OFFSET_MID_REG_T;

// luma_offset_hi_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t offset : 6;      // [5:0]
        uint32_t algo_sel : 1;    // [6]
        uint32_t offset_sign : 1; // [7]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_LUMA_OFFSET_HI_REG_T;

// u_gain_lo_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t u_gain_lo_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_U_GAIN_LO_REG_T;

// v_gain_lo_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t v_gain_lo_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_V_GAIN_LO_REG_T;

// u_gain_mid_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t u_gain_mid_reg : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_U_GAIN_MID_REG_T;

// v_gain_mid_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t v_gain_mid_reg : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_V_GAIN_MID_REG_T;

// u_gain_hi_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t u_gain_hi_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_U_GAIN_HI_REG_T;

// v_gain_hi_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t v_gain_hi_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_V_GAIN_HI_REG_T;

// again_sel_th0_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t contr_gain_low_th : 3;  // [2:0]
        uint32_t again_sel_th0_rsvd : 1; // [3]
        uint32_t contr_gain_hi_th : 3;   // [6:4]
        uint32_t __31_7 : 25;            // [31:7]
    } b;
} REG_CAMERA_AGAIN_SEL_TH0_REG_T;

// awb_cc_type_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_type_mode : 4;  // [3:0]
        uint32_t cc_gain_hi_th : 3; // [6:4]
        uint32_t luma_first : 1;    // [7]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_AWB_CC_TYPE_CTRL_REG_T;

// awb_cc_type_th_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t r_big_th : 4; // [3:0]
        uint32_t b_big_th : 4; // [7:4]
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_CAMERA_AWB_CC_TYPE_TH_REG_T;

// isp_wrapper_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pout_mode : 2;    // [1:0]
        uint32_t yuv_mode : 2;     // [3:2]
        uint32_t vsync_toggle : 1; // [4]
        uint32_t mipi_rstn : 1;    // [5]
        uint32_t hsync_fix : 1;    // [6]
        uint32_t __31_7 : 25;      // [31:7]
    } b;
} REG_CAMERA_ISP_WRAPPER_CTRL_1_T;

// top_dummy
typedef union {
    uint32_t v;
    struct
    {
        uint32_t top_dummy : 7; // [6:0]
        uint32_t __31_7 : 25;   // [31:7]
    } b;
} REG_CAMERA_TOP_DUMMY_T;

// left_dummy
typedef union {
    uint32_t v;
    struct
    {
        uint32_t left_dummy : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_LEFT_DUMMY_T;

// isp_wrapper_ctrl_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rgb_mode_reg : 3; // [2:0]
        uint32_t sub_mode : 1;     // [3]
        uint32_t mon_mode_reg : 1; // [4]
        uint32_t oclk_inv_reg : 1; // [5]
        uint32_t isp_out_en : 1;   // [6]
        uint32_t __31_7 : 25;      // [31:7]
    } b;
} REG_CAMERA_ISP_WRAPPER_CTRL_2_T;

// line_num_l_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t line_num_l_reg : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_LINE_NUM_L_REG_T;

// pix_num_l_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pix_num_l_reg : 7; // [6:0]
        uint32_t csi_mon_reg : 1;   // [7]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_PIX_NUM_L_REG_T;

// v_dummy
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vbot_dummy_reg : 4; // [3:0]
        uint32_t vtop_dummy_reg : 4; // [7:4]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_V_DUMMY_T;

// scg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t kukl_sel : 1;      // [0]
        uint32_t reg94_rd_sel : 1;  // [1]
        uint32_t bayer_out_sel : 1; // [2]
        uint32_t csup_en : 1;       // [3]
        uint32_t y_gamma_en : 2;    // [5:4]
        uint32_t yuv_sdi_en : 1;    // [6]
        uint32_t reg92_rd_sel : 1;  // [7]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_SCG_T;

// y_gamma_b0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b0 : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B0_T;

// y_gamma_b1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b1 : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B1_T;

// y_gamma_b2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b2 : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B2_T;

// y_gamma_b4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b4 : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B4_T;

// y_gamma_b6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b6 : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B6_T;

// y_gamma_b8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b8 : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B8_T;

// y_gamma_b10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b10 : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B10_T;

// y_gamma_b12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b12 : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B12_T;

// y_gamma_b16
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b16 : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B16_T;

// y_gamma_b20
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b20 : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B20_T;

// y_gamma_b24
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b24 : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B24_T;

// y_gamma_b28
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b28 : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B28_T;

// y_gamma_b32
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_gamma_b32 : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_Y_GAMMA_B32_T;

// r_awb_gain_in
typedef union {
    uint32_t v;
    struct
    {
        uint32_t r_awb_gain_in : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_R_AWB_GAIN_IN_T;

// g_awb_gain_in
typedef union {
    uint32_t v;
    struct
    {
        uint32_t g_awb_gain_in : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_G_AWB_GAIN_IN_T;

// b_awb_gain_in
typedef union {
    uint32_t v;
    struct
    {
        uint32_t b_awb_gain_in : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_B_AWB_GAIN_IN_T;

// r_drc_gain_in
typedef union {
    uint32_t v;
    struct
    {
        uint32_t r_drc_gain_in : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_R_DRC_GAIN_IN_T;

// gr_drc_gain_in
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gr_drc_gain_in : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_GR_DRC_GAIN_IN_T;

// gb_drc_gain_in
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gb_drc_gain_in : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_GB_DRC_GAIN_IN_T;

// b_drc_gain_in
typedef union {
    uint32_t v;
    struct
    {
        uint32_t b_drc_gain_in : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_B_DRC_GAIN_IN_T;

// ae_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ana_gain_in : 6;  // [5:0]
        uint32_t ae_update_en : 1; // [6]
        uint32_t ae_en : 1;        // [7]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_AE_CTRL_T;

// ae_ctrl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_adj_sel : 2;      // [1:0]
        uint32_t gap_ae : 1;           // [2]
        uint32_t gap_be : 1;           // [3]
        uint32_t ae_action_period : 3; // [6:4]
        uint32_t yave_mon_sel : 1;     // [7]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_AE_CTRL2_T;

// ae_ctrl3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t yave_use_mean : 2;      // [1:0]
        uint32_t yave_diff_thr_reg : 2;  // [3:2]
        uint32_t yave_sel : 2;           // [5:4]
        uint32_t yave_plus_bh_mode : 1;  // [6]
        uint32_t ywave_plus_bh_mode : 1; // [7]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_AE_CTRL3_T;

// ae_ctrl4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_hist_big_en : 1;     // [0]
        uint32_t ae_hist_too_big_en : 1; // [1]
        uint32_t hist_ofst0 : 2;         // [3:2]
        uint32_t index_ofst0 : 2;        // [5:4]
        uint32_t index_ofst1 : 2;        // [7:6]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_AE_CTRL4_T;

// ae_win_start
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pcnt_left : 4; // [3:0]
        uint32_t lcnt_top : 4;  // [7:4]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_AE_WIN_START_T;

// ae_win_width
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_win_width : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_AE_WIN_WIDTH_T;

// ae_win_height
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_win_height : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_AE_WIN_HEIGHT_T;

// exp_init
typedef union {
    uint32_t v;
    struct
    {
        uint32_t exp_init : 8; // [7:0]
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_CAMERA_EXP_INIT_T;

// exp_ceil_init
typedef union {
    uint32_t v;
    struct
    {
        uint32_t exp_ceil_init : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_EXP_CEIL_INIT_T;

// ae_exp_1e
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_exp_1e : 8; // [7:0]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_AE_EXP_1E_T;

// ae_diff_thr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t thr2_dark : 4;   // [3:0]
        uint32_t thr2_bright : 4; // [7:4]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_AE_DIFF_THR_T;

// ae_bh_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bh_factor_indoor : 3;  // [2:0]
        uint32_t bh_factor_outdoor : 3; // [5:3]
        uint32_t bh_mean_sel : 2;       // [7:6]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_AE_BH_SEL_T;

// awb_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_sw_mon_en : 1;     // [0]
        uint32_t fast_2x : 1;           // [1]
        uint32_t fast_4x : 1;           // [2]
        uint32_t awb_action_period : 3; // [5:3]
        uint32_t awb_update_en : 1;     // [6]
        uint32_t awb_en : 1;            // [7]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_AWB_CTRL_T;

// awb_ctrl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_mon_sel : 3;  // [2:0]
        uint32_t awb_vld_sel : 1;  // [3]
        uint32_t awb_vld_mode : 3; // [6:4]
        uint32_t awb_adj : 1;      // [7], read only
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_AWB_CTRL2_T;

// awb_y_max
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y_max : 8; // [7:0]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_AWB_Y_MAX_T;

// awb_stop
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_stop_cb_neg_level : 2; // [1:0]
        uint32_t awb_stop_cb_pos_level : 2; // [3:2]
        uint32_t awb_stop_cr_neg_level : 2; // [5:4]
        uint32_t awb_stop_cr_pos_level : 2; // [7:6]
        uint32_t __31_8 : 24;               // [31:8]
    } b;
} REG_CAMERA_AWB_STOP_T;

// awb_algo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_algo : 8; // [7:0]
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_CAMERA_AWB_ALGO_T;

// awb_ctrl3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cr_ofst_lt1x : 1;     // [0]
        uint32_t cr_ofst_gt1x : 1;     // [1]
        uint32_t cb_ofst_lt1x : 1;     // [2]
        uint32_t cb_ofst_gt1x : 1;     // [3]
        uint32_t awb_sum_vld_sel : 1;  // [4]
        uint32_t awb_stop_sel_reg : 1; // [5]
        uint32_t awb_skin_sel : 1;     // [6]
        uint32_t awb_algo_mode : 1;    // [7]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_AWB_CTRL3_T;

// awb_ctrl4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_ctrl4 : 8; // [7:0]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_AWB_CTRL4_T;

// dig_gain_in
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dig_gain_in : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_DIG_GAIN_IN_T;

// y_init_thr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_init_mode : 1; // [0]
        uint32_t y_low_en : 1;    // [1]
        uint32_t y_high_en : 1;   // [2]
        uint32_t y_low_thr : 5;   // [7:3]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_Y_INIT_THR_T;

// y_ave_target
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_ave_target : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_Y_AVE_TARGET_T;

// y_lmt_offset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_low_limit : 3;  // [2:0]
        uint32_t y_lmt_ofst : 1;   // [3]
        uint32_t y_high_limit : 3; // [6:4]
        uint32_t __31_7 : 25;      // [31:7]
    } b;
} REG_CAMERA_Y_LMT_OFFSET_T;

// again_sel_th2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_gain_low_th : 3; // [2:0]
        uint32_t again_sel_th2 : 1;   // [3]
        uint32_t ynr_gain_hi_th : 3;  // [6:4]
        uint32_t __31_7 : 25;         // [31:7]
    } b;
} REG_CAMERA_AGAIN_SEL_TH2_T;

// yave_target_chg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t yave_target_ofst_l : 4; // [3:0]
        uint32_t yave_target_ofst_h : 4; // [7:4]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_YAVE_TARGET_CHG1_T;

// image_eff_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t grey_en : 1;        // [0]
        uint32_t sepia_en : 1;       // [1]
        uint32_t negative_en : 1;    // [2]
        uint32_t color_bar_en : 1;   // [3]
        uint32_t image_eff_rsvd : 1; // [4]
        uint32_t reg93_sel : 1;      // [5]
        uint32_t reg94_sel : 1;      // [6]
        uint32_t sharp_mon : 1;      // [7]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_IMAGE_EFF_REG_T;

// ywave_out
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ywave_out : 8; // [7:0], read only
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_YWAVE_OUT_T;

// ae_bright_hist
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_bright_hist : 8; // [7:0], read only
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_AE_BRIGHT_HIST_T;

// yave_out
typedef union {
    uint32_t v;
    struct
    {
        uint32_t yave_out : 8; // [7:0], read only
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_CAMERA_YAVE_OUT_T;

// exp_out
typedef union {
    uint32_t v;
    struct
    {
        uint32_t exp_out : 8; // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_EXP_OUT_T;

// misc_out
typedef union {
    uint32_t v;
    struct
    {
        uint32_t exp_out_h : 3; // [2:0], read only
        uint32_t awb_ok : 1;    // [3], read only
        uint32_t nexp_sel : 2;  // [5:4], read only
        uint32_t fixed_0 : 1;   // [6], read only
        uint32_t ae_ok : 1;     // [7], read only
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_MISC_OUT_T;

// awb_debug_out
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_crgt : 2;       // [1:0], read only
        uint32_t awb_cbgt : 2;       // [3:2], read only
        uint32_t awb_crsum_sign : 1; // [4], read only
        uint32_t awb_cbsum_sign : 1; // [5], read only
        uint32_t awb_cbcr : 1;       // [6], read only
        uint32_t awb_sum_vld : 1;    // [7], read only
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_AWB_DEBUG_OUT_T;

// mono_color
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mono_color : 8; // [7:0], read only
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_MONO_COLOR_T;

// r_awb_gain
typedef union {
    uint32_t v;
    struct
    {
        uint32_t r_awb_gain : 8; // [7:0], read only
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_R_AWB_GAIN_T;

// b_awb_gain
typedef union {
    uint32_t v;
    struct
    {
        uint32_t b_awb_gain : 8; // [7:0], read only
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_B_AWB_GAIN_T;

// misc_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ana_gain_out : 6; // [5:0], read only
        uint32_t cc_type : 1;      // [6], read only
        uint32_t is_outdoor : 1;   // [7], read only
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_MISC_STATUS_T;

// yave_contr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t yave_contr : 8; // [7:0], read only
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_YAVE_CONTR_T;

// gamma_type
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gamma_type_mode : 3;  // [2:0]
        uint32_t gamma_gain_hi_th : 3; // [5:3]
        uint32_t vgas : 2;             // [7:6]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_GAMMA_TYPE_T;

// blc_line
typedef union {
    uint32_t v;
    struct
    {
        uint32_t blc_line : 8; // [7:0]
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_CAMERA_BLC_LINE_T;

// lsc_xx
typedef union {
    uint32_t v;
    struct
    {
        uint32_t x_low : 4;   // [3:0]
        uint32_t x_high : 4;  // [7:4]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_LSC_XX_T;

// lsc_blc_gain_th
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lsc_gain_low_th : 3;  // [2:0]
        uint32_t lsc_gain_hi_th : 3;   // [5:3]
        uint32_t csup_gain_low_th : 2; // [7:6]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_LSC_BLC_GAIN_TH_T;

// blc_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t blc_out_mode : 2;  // [1:0]
        uint32_t line_init_h : 1;   // [2]
        uint32_t blc_ofst_sign : 1; // [3]
        uint32_t blc_mode : 2;      // [5:4]
        uint32_t blc_sel : 1;       // [6]
        uint32_t blc_en : 1;        // [7]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_BLC_CTRL_T;

// blc_init
typedef union {
    uint32_t v;
    struct
    {
        uint32_t blc00_ofst : 4; // [3:0]
        uint32_t blc01_ofst : 4; // [7:4]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_BLC_INIT_T;

// blc_offset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t blc10_ofst : 4; // [3:0]
        uint32_t blc11_ofst : 4; // [7:4]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_BLC_OFFSET_T;

// blc_thr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t blc_thr : 6; // [5:0]
        uint32_t __31_6 : 26; // [31:6]
    } b;
} REG_CAMERA_BLC_THR_T;

// lsc_xy_cent
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_cent : 4;  // [3:0]
        uint32_t x_cent : 4;  // [7:4]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_LSC_XY_CENT_T;

// cnr_dif_thr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cnr_v_en : 1;      // [0]
        uint32_t cnr_h_en : 1;      // [1]
        uint32_t vcnr_sel : 1;      // [2]
        uint32_t edge_mon : 1;      // [3]
        uint32_t awb_skin_mode : 3; // [6:4]
        uint32_t gamma_type : 1;    // [7], read only
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_CNR_DIF_THR_T;

// cnr_thr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cnr_thr_v : 3; // [2:0]
        uint32_t edge_en_v : 1; // [3]
        uint32_t cnr_thr_h : 3; // [6:4]
        uint32_t edge_en_h : 1; // [7]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_CNR_THR_T;

// gamma_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gamma_p_id : 1;           // [0]
        uint32_t gamma_l_id : 1;           // [1]
        uint32_t gamma_en_non_outdoor : 1; // [2]
        uint32_t gamma_en_outdoor : 1;     // [3]
        uint32_t lsc_p_id : 1;             // [4]
        uint32_t lsc_l_id : 1;             // [5]
        uint32_t lsc_en_non_outdoor : 1;   // [6]
        uint32_t lsc_en_outdoor : 1;       // [7]
        uint32_t __31_8 : 24;              // [31:8]
    } b;
} REG_CAMERA_GAMMA_CTRL_T;

// bayer_gamma_b0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b0 : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B0_T;

// bayer_gamma_b1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b1 : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B1_T;

// bayer_gamma_b2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b2 : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B2_T;

// bayer_gamma_b3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b3 : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B3_T;

// bayer_gamma_b4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b4 : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B4_T;

// bayer_gamma_b6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b6 : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B6_T;

// bayer_gamma_b8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b8 : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B8_T;

// bayer_gamma_b10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b10 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B10_T;

// bayer_gamma_b12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b12 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B12_T;

// bayer_gamma_b16
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b16 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B16_T;

// bayer_gamma_b20
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b20 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B20_T;

// bayer_gamma_b24
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b24 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B24_T;

// bayer_gamma_b28
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b28 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B28_T;

// bayer_gamma_b32
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b32 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B32_T;

// bayer_gamma_b36
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b36 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B36_T;

// bayer_gamma_b40
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b40 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B40_T;

// bayer_gamma_b48
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b48 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B48_T;

// bayer_gamma_b56
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b56 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B56_T;

// bayer_gamma_b64
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma_b64 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA_B64_T;

// blc_out0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t blc_out0 : 8; // [7:0], read only
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_CAMERA_BLC_OUT0_T;

// blc_out1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t blc_out1 : 8; // [7:0], read only
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_CAMERA_BLC_OUT1_T;

// dpc_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_on : 1;       // [0]
        uint32_t adp_med_sel : 1;  // [1]
        uint32_t ana_gain_cmp : 2; // [3:2]
        uint32_t rsvd : 1;         // [4]
        uint32_t nrf_gaus_sel : 1; // [5]
        uint32_t bayer_nr_on : 1;  // [6]
        uint32_t cc_on : 1;        // [7]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_DPC_CTRL_0_T;

// dpc_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t int_flg_cmp : 2;      // [1:0]
        uint32_t abs_sign_all_cmp : 2; // [3:2]
        uint32_t int_dif_sel : 1;      // [4]
        uint32_t __31_5 : 27;          // [31:5]
    } b;
} REG_CAMERA_DPC_CTRL_1_T;

// y_thr_lo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_thr_lo : 8; // [7:0]
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_CAMERA_Y_THR_LO_T;

// y_thr_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_thr_mid : 8; // [7:0]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_Y_THR_MID_T;

// y_thr_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_thr_hi : 8; // [7:0]
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_CAMERA_Y_THR_HI_T;

// intp_cfa_hv
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfa_v_thr_l : 3; // [2:0]
        uint32_t rsvd1 : 1;       // [3]
        uint32_t cfa_h_thr_l : 3; // [6:4]
        uint32_t rsvd2 : 1;       // [7]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_INTP_CFA_HV_T;

// manual_adj
typedef union {
    uint32_t v;
    struct
    {
        uint32_t b_gain_adj : 1;       // [0]
        uint32_t g_gain_adj : 1;       // [1]
        uint32_t r_gain_adj : 1;       // [2]
        uint32_t ana_gain_adj : 1;     // [3]
        uint32_t adj_direction : 1;    // [4]
        uint32_t index_manual_adj : 1; // [5]
        uint32_t in_capture_awb : 1;   // [6]
        uint32_t in_capture_ae : 1;    // [7]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_MANUAL_ADJ_T;

// dpc_int_thr_lo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_int_thr_lo : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_DPC_INT_THR_LO_T;

// dpc_int_thr_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_int_thr_hi : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_DPC_INT_THR_HI_T;

// again_sel_th1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bnr_gain_low_th : 3;    // [2:0]
        uint32_t again_sel_th1_rsvd : 1; // [3]
        uint32_t bnr_gain_hi_th : 3;     // [6:4]
        uint32_t __31_7 : 25;            // [31:7]
    } b;
} REG_CAMERA_AGAIN_SEL_TH1_T;

// dpc_nr_lf_str_lo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_nr_lf_str_lo : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_DPC_NR_LF_STR_LO_T;

// dpc_nr_hf_str_lo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_nr_hf_str_lo : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_DPC_NR_HF_STR_LO_T;

// dpc_nr_area_thr_lo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_nr_area_thr_lo : 8; // [7:0]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_DPC_NR_AREA_THR_LO_T;

// dpc_nr_lf_str_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_nr_lf_str_mid : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_DPC_NR_LF_STR_MID_T;

// dpc_nr_hf_str_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_nr_hf_str_mid : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_DPC_NR_HF_STR_MID_T;

// dpc_nr_area_thr_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_nr_area_thr_mid : 8; // [7:0]
        uint32_t __31_8 : 24;             // [31:8]
    } b;
} REG_CAMERA_DPC_NR_AREA_THR_MID_T;

// dpc_nr_lf_str_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_nr_lf_str_hi : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_DPC_NR_LF_STR_HI_T;

// dpc_nr_hf_str_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_nr_hf_str_hi : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_DPC_NR_HF_STR_HI_T;

// dpc_nr_area_thr_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dpc_nr_area_thr_hi : 8; // [7:0]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_DPC_NR_AREA_THR_HI_T;

// intp_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pid_inv_en : 1;  // [0]
        uint32_t lid_inv_en : 1;  // [1]
        uint32_t gfilter_en : 1;  // [2]
        uint32_t gfilter3_en : 1; // [3]
        uint32_t gfliter5_en : 1; // [4]
        uint32_t sort_sel : 3;    // [7:5]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_INTP_CTRL_T;

// intp_cfa_h_thr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t intp_cfa_h_thr : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_INTP_CFA_H_THR_T;

// intp_cfa_v_thr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t intp_cfa_v_thr : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_INTP_CFA_V_THR_T;

// intp_grgb_sel_lmt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t intp_grgb_sel_lmt : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_INTP_GRGB_SEL_LMT_T;

// intp_gf_lmt_thr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t intp_gf_lmt_thr : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_INTP_GF_LMT_THR_T;

// cc_r_offset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_r_offset : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_CC_R_OFFSET_T;

// cc_g_offset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_g_offset : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_CC_G_OFFSET_T;

// cc_b_offset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_b_offset : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_CC_B_OFFSET_T;

// cc_00
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_00 : 8;   // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC_00_T;

// cc_01
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_01 : 8;   // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC_01_T;

// cc_10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_10 : 8;   // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC_10_T;

// cc_11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_11 : 8;   // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC_11_T;

// cc_20
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_20 : 8;   // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC_20_T;

// cc_21
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_21 : 8;   // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC_21_T;

// cc_r_offset_post
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_r_offset_post : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_CC_R_OFFSET_POST_T;

// cc_g_offset_post
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_g_offset_post : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_CC_G_OFFSET_POST_T;

// cc_b_offset_post
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_b_offset_post : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_CC_B_OFFSET_POST_T;

// cc2_r_offset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc2_r_offset : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_CC2_R_OFFSET_T;

// cc2_g_offset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc2_g_offset : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_CC2_G_OFFSET_T;

// cc2_b_offset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc2_b_offset : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_CC2_B_OFFSET_T;

// cc2_00
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc2_00 : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC2_00_T;

// cc2_01
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc2_01 : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC2_01_T;

// cc2_10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc2_10 : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC2_10_T;

// cc2_11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc2_11 : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC2_11_T;

// cc2_20
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc2_20 : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC2_20_T;

// cc2_21
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc2_21 : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_CC2_21_T;

// sharp_lmt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_lmt : 7;     // [6:0]
        uint32_t sharp_final_h : 1; // [7]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_SHARP_LMT_T;

// sharp_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_cmp_gap_lo : 4; // [3:0]
        uint32_t sharp_final : 2;      // [5:4]
        uint32_t sharp_sel : 1;        // [6]
        uint32_t rgb_test_pattern : 1; // [7]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_SHARP_MODE_T;

// sharp_gain_str_lo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_gain_str_lo : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_SHARP_GAIN_STR_LO_T;

// sharp_nr_area_thr_lo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_nr_area_thr_lo : 7; // [6:0]
        uint32_t __31_7 : 25;              // [31:7]
    } b;
} REG_CAMERA_SHARP_NR_AREA_THR_LO_T;

// sharp_gain_str_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_gain_str_mid : 8; // [7:0]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_SHARP_GAIN_STR_MID_T;

// sharp_nr_area_thr_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_nr_area_thr_mid : 7; // [6:0]
        uint32_t __31_7 : 25;               // [31:7]
    } b;
} REG_CAMERA_SHARP_NR_AREA_THR_MID_T;

// sharp_gain_str_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_gain_str_hi : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_SHARP_GAIN_STR_HI_T;

// sharp_nr_area_thr_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_nr_area_thr_hi : 7; // [6:0]
        uint32_t __31_7 : 25;              // [31:7]
    } b;
} REG_CAMERA_SHARP_NR_AREA_THR_HI_T;

// ynr_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_on : 1;           // [0]
        uint32_t ynr_edge_methode : 2; // [2:1]
        uint32_t sharp_on : 1;         // [3]
        uint32_t sharp_plus_mode : 2;  // [5:4]
        uint32_t y_ae_sel : 2;         // [7:6]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_YNR_CTRL_REG_T;

// ynr_lf_method_str
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_lf_method_str : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_YNR_LF_METHOD_STR_T;

// ynr_lf_str_lo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_lf_str_lo : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_YNR_LF_STR_LO_T;

// ynr_hf_str_lo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_hf_str_lo : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_YNR_HF_STR_LO_T;

// ynr_area_thr_lo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_area_thr_lo : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_YNR_AREA_THR_LO_T;

// ynr_lf_str_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_lf_str_mid : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_YNR_LF_STR_MID_T;

// ynr_hf_str_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_hf_str_mid : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_YNR_HF_STR_MID_T;

// ynr_area_thr_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_area_thr_mid : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_YNR_AREA_THR_MID_T;

// ynr_lf_str_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_lf_str_hi : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_YNR_LF_STR_HI_T;

// ynr_hf_str_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_hf_str_hi : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_YNR_HF_STR_HI_T;

// ynr_area_thr_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ynr_area_thr_hi : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_YNR_AREA_THR_HI_T;

// hue_sin_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hue_sin_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_HUE_SIN_REG_T;

// hue_cos_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hue_cosx_reg : 7; // [6:0]
        uint32_t sin_sign_reg : 1; // [7]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_HUE_COS_REG_T;

// cnr_1d_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cnr_dif_thr_mid : 4; // [3:0]
        uint32_t cnr_1d_on : 1;       // [4]
        uint32_t satur_on : 1;        // [5]
        uint32_t hue_on : 1;          // [6]
        uint32_t __31_7 : 25;         // [31:7]
    } b;
} REG_CAMERA_CNR_1D_CTRL_REG_T;

// cnr_xx_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cnr_dif_thr_low : 4;  // [3:0]
        uint32_t cnr_dif_thr_high : 4; // [7:4]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_CNR_XX_REG_T;

// in5_low_th_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t in5_low_th_reg : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_IN5_LOW_TH_REG_T;

// in5_high_th_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t in5_high_th_reg : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_IN5_HIGH_TH_REG_T;

// p2_up_r_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_up_r_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P2_UP_R_REG_T;

// p2_up_g_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_up_g_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P2_UP_G_REG_T;

// p2_up_b_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_up_b_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P2_UP_B_REG_T;

// p2_down_r_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_down_r_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_P2_DOWN_R_REG_T;

// p2_down_g_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_down_g_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_P2_DOWN_G_REG_T;

// p2_down_b_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_down_b_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_P2_DOWN_B_REG_T;

// p2_left_r_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_left_r_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_P2_LEFT_R_REG_T;

// p2_left_g_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_left_g_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_P2_LEFT_G_REG_T;

// p2_left_b_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_left_b_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_P2_LEFT_B_REG_T;

// p2_right_r_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_right_r_reg : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_P2_RIGHT_R_REG_T;

// p2_right_g_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_right_g_reg : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_P2_RIGHT_G_REG_T;

// p2_right_b_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p2_right_b_reg : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_P2_RIGHT_B_REG_T;

// p4_q1_r_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q1_r_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q1_R_REG_T;

// p4_q1_g_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q1_g_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q1_G_REG_T;

// p4_q1_b_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q1_b_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q1_B_REG_T;

// p4_q2_r_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q2_r_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q2_R_REG_T;

// p4_q2_g_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q2_g_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q2_G_REG_T;

// p4_q2_b_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q2_b_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q2_B_REG_T;

// p4_q3_r_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q3_r_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q3_R_REG_T;

// p4_q3_g_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q3_g_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q3_G_REG_T;

// p4_q3_b_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q3_b_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q3_B_REG_T;

// p4_q4_r_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q4_r_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q4_R_REG_T;

// p4_q4_g_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q4_g_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q4_G_REG_T;

// p4_q4_b_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t p4_q4_b_reg : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_P4_Q4_B_REG_T;

// ae_e00_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e00_sta_line : 6; // [5:0]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_CAMERA_AE_E00_STA_REG_T;

// ae_e00_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e00_num : 4;      // [3:0]
        uint32_t ae_e00_interval : 2; // [5:4]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_CAMERA_AE_E00_NUM_REG_T;

// ae_e01_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e01_sta_line : 6; // [5:0]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_CAMERA_AE_E01_STA_REG_T;

// ae_e01_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e01_num : 4;      // [3:0]
        uint32_t ae_e01_interval : 3; // [6:4]
        uint32_t __31_7 : 25;         // [31:7]
    } b;
} REG_CAMERA_AE_E01_NUM_REG_T;

// ae_e02_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e02_sta_line : 7; // [6:0]
        uint32_t __31_7 : 25;         // [31:7]
    } b;
} REG_CAMERA_AE_E02_STA_REG_T;

// ae_e02_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e02_num : 4;      // [3:0]
        uint32_t ae_e02_interval : 4; // [7:4]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AE_E02_NUM_REG_T;

// ae_e1_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e1_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_E1_STA_REG_T;

// ae_e1_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e1_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_E1_NUM_REG_T;

// ae_e2_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e2_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_E2_STA_REG_T;

// ae_e2_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e2_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_E2_NUM_REG_T;

// ae_e3_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e3_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_E3_STA_REG_T;

// ae_e3_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e3_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_E3_NUM_REG_T;

// ae_e4_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e4_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_E4_STA_REG_T;

// ae_e4_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e4_num_reg : 5; // [4:0]
        uint32_t __31_5 : 27;       // [31:5]
    } b;
} REG_CAMERA_AE_E4_NUM_REG_T;

// ae_e5_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e5_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_E5_STA_REG_T;

// ae_e5_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e5_num_reg : 5; // [4:0]
        uint32_t __31_5 : 27;       // [31:5]
    } b;
} REG_CAMERA_AE_E5_NUM_REG_T;

// ae_e6_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e6_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_E6_STA_REG_T;

// ae_e6_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e6_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_E6_NUM_REG_T;

// ae_e7_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e7_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_E7_STA_REG_T;

// ae_e7_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e7_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_E7_NUM_REG_T;

// ae_e8_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e8_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_E8_STA_REG_T;

// ae_e8_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e8_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_E8_NUM_REG_T;

// ae_e9_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e9_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_E9_STA_REG_T;

// ae_e9_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_e9_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_E9_NUM_REG_T;

// ae_ea_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_ea_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_EA_STA_REG_T;

// ae_ea_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_ea_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_EA_NUM_REG_T;

// ae_eb_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_eb_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_EB_STA_REG_T;

// ae_eb_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_eb_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_EB_NUM_REG_T;

// ae_ec_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_ec_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_EC_STA_REG_T;

// ae_ec_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_ec_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_EC_NUM_REG_T;

// ae_ed_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_ed_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_ED_STA_REG_T;

// ae_ed_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_ed_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_ED_NUM_REG_T;

// bayer_gamma2_b0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b0 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B0_T;

// bayer_gamma2_b1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b1 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B1_T;

// bayer_gamma2_b2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b2 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B2_T;

// bayer_gamma2_b3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b3 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B3_T;

// bayer_gamma2_b4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b4 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B4_T;

// bayer_gamma2_b6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b6 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B6_T;

// bayer_gamma2_b8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b8 : 8; // [7:0]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B8_T;

// bayer_gamma2_b10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b10 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B10_T;

// bayer_gamma2_b12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b12 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B12_T;

// bayer_gamma2_b16
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b16 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B16_T;

// bayer_gamma2_b20
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b20 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B20_T;

// bayer_gamma2_b24
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b24 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B24_T;

// bayer_gamma2_b28
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b28 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B28_T;

// bayer_gamma2_b32
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b32 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B32_T;

// bayer_gamma2_b36
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b36 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B36_T;

// bayer_gamma2_b40
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b40 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B40_T;

// bayer_gamma2_b48
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b48 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B48_T;

// bayer_gamma2_b56
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b56 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B56_T;

// bayer_gamma2_b64
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bayer_gamma2_b64 : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_BAYER_GAMMA2_B64_T;

// y_thr7_lo_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_thr7_lo_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_Y_THR7_LO_REG_T;

// y_thr7_mid_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_thr7_mid_reg : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_Y_THR7_MID_REG_T;

// y_thr7_hi_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_thr7_hi_reg : 8; // [7:0]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_Y_THR7_HI_REG_T;

// dpa_new_ctrl_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inflg_ctrl_reg_0 : 1; // [0]
        uint32_t inflg_ctrl_reg_1 : 1; // [1]
        uint32_t inflg_ctrl_reg_2 : 1; // [2]
        uint32_t __31_3 : 29;          // [31:3]
    } b;
} REG_CAMERA_DPA_NEW_CTRL_REG_T;

// dpa_new_ctrl_hi_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inflg_ctrl_reg0_h : 1; // [0]
        uint32_t inflg_ctrl_reg1_h : 1; // [1]
        uint32_t inflg_ctrl_reg2_h : 1; // [2]
        uint32_t threshold_rsvd : 2;    // [4:3]
        uint32_t __31_5 : 27;           // [31:5]
    } b;
} REG_CAMERA_DPA_NEW_CTRL_HI_REG_T;

// ae_index_gap
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gap_2e : 1;  // [0]
        uint32_t gap_3e : 1;  // [1]
        uint32_t gap_4e : 1;  // [2]
        uint32_t gap_5e : 1;  // [3]
        uint32_t gap_6e : 1;  // [4]
        uint32_t gap_7e : 1;  // [5]
        uint32_t gap_8e : 1;  // [6]
        uint32_t gap_9e : 1;  // [7]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_AE_INDEX_GAP_T;

// awb_calc_height_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_calc_height_reg : 8; // [7:0]
        uint32_t __31_8 : 24;             // [31:8]
    } b;
} REG_CAMERA_AWB_CALC_HEIGHT_REG_T;

// drc_r_clp_value_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drc_r_clp_value_reg : 6; // [5:0]
        uint32_t __31_6 : 26;             // [31:6]
    } b;
} REG_CAMERA_DRC_R_CLP_VALUE_REG_T;

// drc_gr_clp_value_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drc_gr_clp_value_reg : 6; // [5:0]
        uint32_t __31_6 : 26;              // [31:6]
    } b;
} REG_CAMERA_DRC_GR_CLP_VALUE_REG_T;

// drc_gb_clp_value_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drc_gb_clp_value_reg : 6; // [5:0]
        uint32_t __31_6 : 26;              // [31:6]
    } b;
} REG_CAMERA_DRC_GB_CLP_VALUE_REG_T;

// drc_b_clp_value_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drc_b_clp_value_reg : 6; // [5:0]
        uint32_t __31_6 : 26;             // [31:6]
    } b;
} REG_CAMERA_DRC_B_CLP_VALUE_REG_T;

// sepia_cr_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sepia_cr_reg : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_SEPIA_CR_REG_T;

// sepia_cb_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sepia_cb_reg : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_SEPIA_CB_REG_T;

// csup_y_min_hi_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t csup_y_min_hi_reg : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_CSUP_Y_MIN_HI_REG_T;

// csup_gain_hi_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t csup_gain_hi_reg : 8; // [7:0]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_CAMERA_CSUP_GAIN_HI_REG_T;

// csup_y_max_low_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t csup_y_max_low_reg : 8; // [7:0]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_CSUP_Y_MAX_LOW_REG_T;

// csup_gain_low_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t csup_gain_low_reg : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_CSUP_GAIN_LOW_REG_T;

// ae_dk_hist_thr_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_dk_hist_thr_reg : 8; // [7:0]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_AE_DK_HIST_THR_REG_T;

// ae_br_hist_thr_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_br_hist_thr_reg : 8; // [7:0]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_AE_BR_HIST_THR_REG_T;

// hist_bp_level_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hist_bp_level_reg : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_HIST_BP_LEVEL_REG_T;

// outdoor_th_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t outdoor_th : 4;     // [3:0]
        uint32_t non_outdoor_th : 4; // [7:4]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_OUTDOOR_TH_REG_T;

// awb_rgain_low_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;             // [1:0]
        uint32_t awb_rgain_low_reg : 6; // [7:2]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_AWB_RGAIN_LOW_REG_T;

// awb_rgain_high_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;              // [1:0]
        uint32_t awb_rgain_high_reg : 6; // [7:2]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_AWB_RGAIN_HIGH_REG_T;

// awb_bgain_low_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;             // [1:0]
        uint32_t awb_bgain_low_reg : 6; // [7:2]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_AWB_BGAIN_LOW_REG_T;

// awb_bgain_high_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;              // [1:0]
        uint32_t awb_bgain_high_reg : 6; // [7:2]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_AWB_BGAIN_HIGH_REG_T;

// awb_calc_start_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_win_y_start : 4; // [3:0]
        uint32_t awb_win_x_start : 4; // [7:4]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AWB_CALC_START_REG_T;

// awb_calc_width_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_calc_width_reg : 8; // [7:0]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_AWB_CALC_WIDTH_REG_T;

// hist_dp_level_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hist_dp_level_reg : 8; // [7:0]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_HIST_DP_LEVEL_REG_T;

// awb_y_fmin
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y_fmin : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y_FMIN_T;

// awb_y_fmax
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y_fmax : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_AWB_Y_FMAX_T;

// awb_cb_fmin
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_cb_fmin : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_AWB_CB_FMIN_T;

// awb_cb_fmax
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_cb_fmax : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_AWB_CB_FMAX_T;

// awb_cr_fmin
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_cr_fmin : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_AWB_CR_FMIN_T;

// awb_cr_fmax
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_cr_fmax : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_AWB_CR_FMAX_T;

// awb_y_fmin2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y_fmin2 : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_AWB_Y_FMIN2_T;

// awb_y_fmax2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y_fmax2 : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_AWB_Y_FMAX2_T;

// awb_cb_fmin2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_cb_fmin2 : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_AWB_CB_FMIN2_T;

// awb_cb_fmax2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_cb_fmax2 : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_AWB_CB_FMAX2_T;

// awb_cr_fmin2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_cr_fmin2 : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_AWB_CR_FMIN2_T;

// awb_cr_fmax2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_cr_fmax2 : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_AWB_CR_FMAX2_T;

// ae_use_mean
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ycave_use_mean : 2;     // [1:0]
        uint32_t ywave_use_mean : 2;     // [3:2]
        uint32_t yave_weight_mode : 1;   // [4]
        uint32_t nexp_out_sel_reg : 1;   // [5]
        uint32_t ae_ext_adj_val_reg : 1; // [6]
        uint32_t ae_ext_adj_on_reg : 1;  // [7]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CAMERA_AE_USE_MEAN_T;

// ae_weight_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ywave_pcnt_left : 4; // [3:0]
        uint32_t ywave_lcnt_top : 4;  // [7:4]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_AE_WEIGHT_STA_T;

// ae_qwidth
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qwidth : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_AE_QWIDTH_T;

// ae_qheight
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qheight : 7;   // [6:0]
        uint32_t ywave_sel : 1; // [7]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_AE_QHEIGHT_T;

// ae_win_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t yave_pcnt_sta : 4; // [3:0]
        uint32_t yave_lcnt_sta : 4; // [7:4]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_AE_WIN_STA_T;

// ae_width
typedef union {
    uint32_t v;
    struct
    {
        uint32_t width : 8;   // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_AE_WIDTH_T;

// ae_height
typedef union {
    uint32_t v;
    struct
    {
        uint32_t height : 8;  // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CAMERA_AE_HEIGHT_T;

// sw_update
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cc_type_sw : 1;      // [0]
        uint32_t is_outdoor_sw : 1;   // [1]
        uint32_t gamma_type_sw : 1;   // [2]
        uint32_t sw_update_rsvd : 1;  // [3]
        uint32_t is_outdoor_mode : 3; // [6:4]
        uint32_t awb_outdoor_en : 1;  // [7]
        uint32_t __31_8 : 24;         // [31:8]
    } b;
} REG_CAMERA_SW_UPDATE_T;

// awb_ctrl5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t r_low_non_a : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_AWB_CTRL5_T;

// awb_ctrl6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_stop_h : 4;    // [3:0]
        uint32_t awb_adj_again : 2; // [5:4]
        uint32_t awb_algo_en : 1;   // [6]
        uint32_t check_r_low : 1;   // [7]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_CAMERA_AWB_CTRL6_T;

// sca_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sca_mode : 3; // [2:0]
        uint32_t __31_3 : 29;  // [31:3]
    } b;
} REG_CAMERA_SCA_REG_T;

// ae_ee_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_ee_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_EE_STA_REG_T;

// ae_ee_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_ee_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_EE_NUM_REG_T;

// ae_ef_sta_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_ef_sta_gain : 6; // [5:0]
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CAMERA_AE_EF_STA_REG_T;

// ae_ef_num_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_ef_num_reg : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_CAMERA_AE_EF_NUM_REG_T;

// ae_thr_big_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ae_thr_big_dark : 4;   // [3:0]
        uint32_t ae_thr_big_bright : 4; // [7:4]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_AE_THR_BIG_REG_T;

// sharp_gain_minus_low
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_gain_minus_low : 8; // [7:0]
        uint32_t __31_8 : 24;              // [31:8]
    } b;
} REG_CAMERA_SHARP_GAIN_MINUS_LOW_T;

// sharp_gain_minus_mid
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_gain_minus_mid : 8; // [7:0]
        uint32_t __31_8 : 24;              // [31:8]
    } b;
} REG_CAMERA_SHARP_GAIN_MINUS_MID_T;

// sharp_gain_minus_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_gain_minus_hi : 8; // [7:0]
        uint32_t __31_8 : 24;             // [31:8]
    } b;
} REG_CAMERA_SHARP_GAIN_MINUS_HI_T;

// sharp_mode_mid_hi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sharp_cmp_gap_mid : 4; // [3:0]
        uint32_t sharp_cmp_gap_hi : 4;  // [7:4]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_CAMERA_SHARP_MODE_MID_HI_T;

// fw_version_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fw_version : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_FW_VERSION_REG_T;

// awb_y_min_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awb_y_min : 8; // [7:0]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CAMERA_AWB_Y_MIN_REG_T;

// y_red_coef_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_red_coef : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_CAMERA_Y_RED_COEF_REG_T;

// y_blue_coef_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t y_blue_coef : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_Y_BLUE_COEF_REG_T;

// cb_red_coef_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cb_red_coef : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_CAMERA_CB_RED_COEF_REG_T;

// cr_blue_coef_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cr_blue_coef : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_CAMERA_CR_BLUE_COEF_REG_T;

// hist_vbp_level_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hist_vbp_level : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_HIST_VBP_LEVEL_REG_T;

// hist_vdp_level_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hist_vdp_level : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_CAMERA_HIST_VDP_LEVEL_REG_T;

// ctrl
#define CAMERA_ENABLE_ENABLE (1 << 0)
#define CAMERA_ENABLE_DISABLE (0 << 0)
#define CAMERA_DCTENABLE (1 << 1)
#define CAMERA_BUF_ENABLE (1 << 2)
#define CAMERA_RGB_RFIRST (1 << 3)
#define CAMERA_DATAFORMAT(n) (((n)&0x3) << 4)
#define CAMERA_DATAFORMAT_RGB565 (0 << 4)
#define CAMERA_DATAFORMAT_YUV422 (1 << 4)
#define CAMERA_DATAFORMAT_JPEG (2 << 4)
#define CAMERA_DATAFORMAT_RESERVE (3 << 4)
#define CAMERA_CFG_CAM_C2CSE(n) (((n)&0x3) << 6)
#define CAMERA_RESET_POL_INVERT (1 << 8)
#define CAMERA_RESET_POL_NORMAL (0 << 8)
#define CAMERA_PWDN_POL_INVERT (1 << 9)
#define CAMERA_PWDN_POL_NORMAL (0 << 9)
#define CAMERA_VSYNC_POL_INVERT (1 << 10)
#define CAMERA_VSYNC_POL_NORMAL (0 << 10)
#define CAMERA_HREF_POL_INVERT (1 << 11)
#define CAMERA_HREF_POL_NORMAL (0 << 11)
#define CAMERA_PIXCLK_POL_INVERT (1 << 12)
#define CAMERA_PIXCLK_POL_NORMAL (0 << 12)
#define CAMERA_VSYNC_DROP_DROP (1 << 14)
#define CAMERA_VSYNC_DROP_NORMAL (0 << 14)
#define CAMERA_DECIMFRM(n) (((n)&0x3) << 16)
#define CAMERA_DECIMFRM_ORIGINAL (0 << 16)
#define CAMERA_DECIMFRM_DIV_2 (1 << 16)
#define CAMERA_DECIMFRM_DIV_3 (2 << 16)
#define CAMERA_DECIMFRM_DIV_4 (3 << 16)
#define CAMERA_DECIMCOL(n) (((n)&0x3) << 18)
#define CAMERA_DECIMCOL_ORIGINAL (0 << 18)
#define CAMERA_DECIMCOL_DIV_2 (1 << 18)
#define CAMERA_DECIMCOL_DIV_3 (2 << 18)
#define CAMERA_DECIMCOL_DIV_4 (3 << 18)
#define CAMERA_DECIMROW(n) (((n)&0x3) << 20)
#define CAMERA_DECIMROW_ORIGINAL (0 << 20)
#define CAMERA_DECIMROW_DIV_2 (1 << 20)
#define CAMERA_DECIMROW_DIV_3 (2 << 20)
#define CAMERA_DECIMROW_DIV_4 (3 << 20)
#define CAMERA_REORDER(n) (((n)&0x7) << 24)
#define CAMERA_CROPEN_ENABLE (1 << 28)
#define CAMERA_CROPEN_DISABLE (0 << 28)
#define CAMERA_BIST_MODE_BIST (1 << 30)
#define CAMERA_BIST_MODE_NORMAL (0 << 30)
#define CAMERA_TEST_TEST (1 << 31)
#define CAMERA_TEST_NORMAL (0 << 31)

#define CAMERA_ENABLE_V_ENABLE (1)
#define CAMERA_ENABLE_V_DISABLE (0)
#define CAMERA_DATAFORMAT_V_RGB565 (0)
#define CAMERA_DATAFORMAT_V_YUV422 (1)
#define CAMERA_DATAFORMAT_V_JPEG (2)
#define CAMERA_DATAFORMAT_V_RESERVE (3)
#define CAMERA_RESET_POL_V_INVERT (1)
#define CAMERA_RESET_POL_V_NORMAL (0)
#define CAMERA_PWDN_POL_V_INVERT (1)
#define CAMERA_PWDN_POL_V_NORMAL (0)
#define CAMERA_VSYNC_POL_V_INVERT (1)
#define CAMERA_VSYNC_POL_V_NORMAL (0)
#define CAMERA_HREF_POL_V_INVERT (1)
#define CAMERA_HREF_POL_V_NORMAL (0)
#define CAMERA_PIXCLK_POL_V_INVERT (1)
#define CAMERA_PIXCLK_POL_V_NORMAL (0)
#define CAMERA_VSYNC_DROP_V_DROP (1)
#define CAMERA_VSYNC_DROP_V_NORMAL (0)
#define CAMERA_DECIMFRM_V_ORIGINAL (0)
#define CAMERA_DECIMFRM_V_DIV_2 (1)
#define CAMERA_DECIMFRM_V_DIV_3 (2)
#define CAMERA_DECIMFRM_V_DIV_4 (3)
#define CAMERA_DECIMCOL_V_ORIGINAL (0)
#define CAMERA_DECIMCOL_V_DIV_2 (1)
#define CAMERA_DECIMCOL_V_DIV_3 (2)
#define CAMERA_DECIMCOL_V_DIV_4 (3)
#define CAMERA_DECIMROW_V_ORIGINAL (0)
#define CAMERA_DECIMROW_V_DIV_2 (1)
#define CAMERA_DECIMROW_V_DIV_3 (2)
#define CAMERA_DECIMROW_V_DIV_4 (3)
#define CAMERA_CROPEN_V_ENABLE (1)
#define CAMERA_CROPEN_V_DISABLE (0)
#define CAMERA_BIST_MODE_V_BIST (1)
#define CAMERA_BIST_MODE_V_NORMAL (0)
#define CAMERA_TEST_V_TEST (1)
#define CAMERA_TEST_V_NORMAL (0)

// status
#define CAMERA_OVFL (1 << 0)
#define CAMERA_VSYNC_R (1 << 1)
#define CAMERA_VSYNC_F (1 << 2)
#define CAMERA_DMA_DONE (1 << 3)
#define CAMERA_FIFO_EMPTY (1 << 4)
#define CAMERA_SPI_OVFL (1 << 5)

// data
#define CAMERA_RX_DATA(n) (((n)&0xffffffff) << 0)

// irq_mask
#define CAMERA_OVFL (1 << 0)
#define CAMERA_VSYNC_R (1 << 1)
#define CAMERA_VSYNC_F (1 << 2)
#define CAMERA_DMA_DONE (1 << 3)

// irq_clear
#define CAMERA_OVFL (1 << 0)
#define CAMERA_VSYNC_R (1 << 1)
#define CAMERA_VSYNC_F (1 << 2)
#define CAMERA_DMA_DONE (1 << 3)

// irq_cause
#define CAMERA_OVFL (1 << 0)
#define CAMERA_VSYNC_R (1 << 1)
#define CAMERA_VSYNC_F (1 << 2)
#define CAMERA_DMA_DONE (1 << 3)

// cmd_set
#define CAMERA_PWDN (1 << 0)
#define CAMERA_RESET (1 << 4)
#define CAMERA_FIFO_RESET (1 << 8)

// cmd_clr
#define CAMERA_PWDN (1 << 0)
#define CAMERA_RESET (1 << 4)

// dstwincol
#define CAMERA_DSTWINCOLSTART(n) (((n)&0xfff) << 0)
#define CAMERA_DSTWINCOLEND(n) (((n)&0xfff) << 16)

// dstwinrow
#define CAMERA_DSTWINROWSTART(n) (((n)&0xfff) << 0)
#define CAMERA_DSTWINROWEND(n) (((n)&0xfff) << 16)

// scl_config
#define CAMERA_SCALE_EN (1 << 0)
#define CAMERA_DATA_OUT_SWAP (1 << 4)
#define CAMERA_SCALE_COL(n) (((n)&0x3) << 8)
#define CAMERA_SCALE_ROW(n) (((n)&0x3) << 16)

// spi_camera_reg0
#define CAMERA_CAMERA_SPI_SLAVE_EN (1 << 0)
#define CAMERA_CAMERA_SPI_MASTER_EN (1 << 1)
#define CAMERA_YUV_OUT_FORMAT(n) (((n)&0x7) << 2)
#define CAMERA_OVERFLOW_RSTN_ONLY_VSYNC_LOW (1 << 5)
#define CAMERA_OVERFLOW_OBSERVE_ONLY_VSYNC_LOW (1 << 6)
#define CAMERA_OVERFLOW_RSTN_EN (1 << 7)
#define CAMERA_BIG_END_DIS (1 << 8)
#define CAMERA_OVERFLOW_INV (1 << 9)
#define CAMERA_HREF_INV (1 << 10)
#define CAMERA_SPI_CAMERA_REG0_VSYNC_INV (1 << 11)
#define CAMERA_BLOCK_NUM_PER_LINE(n) (((n)&0x3ff) << 12)
#define CAMERA_LINE_NUM_PER_FRAME(n) (((n)&0x3ff) << 22)

// spi_camera_reg1
#define CAMERA_CAMERA_CLK_DIV_NUM(n) (((n)&0xffff) << 0)
#define CAMERA_CTS_SPI_MASTER_REG (1 << 16)
#define CAMERA_SSN_CM_INV (1 << 17)
#define CAMERA_SCK_CM_INV (1 << 18)
#define CAMERA_SSN_SPI_OENB_DR (1 << 19)
#define CAMERA_SSN_SPI_OENB_REG (1 << 20)
#define CAMERA_SCK_SPI_OENB_DR (1 << 21)
#define CAMERA_SCK_SPI_OENB_REG (1 << 22)
#define CAMERA_SDO_SPI_SWAP (1 << 29)
#define CAMERA_CLK_INV (1 << 30)
#define CAMERA_SCK_DDR_EN (1 << 31)

// spi_camera_reg2
#define CAMERA_SSN_WAIT_LENGTH(n) (((n)&0xff) << 0)
#define CAMERA_INIT_WAIT_LENGTH(n) (((n)&0xff) << 8)
#define CAMERA_WORD_NUM_PER_BLOCK(n) (((n)&0xff) << 16)
#define CAMERA_SSN_CS_DELAY(n) (((n)&0x3) << 24)
#define CAMERA_DATA_RECEIVE_CHOOSE_BIT(n) (((n)&0x3) << 26)
#define CAMERA_READY_CS_INV (1 << 28)
#define CAMERA_SSN_CS_INV (1 << 29)
#define CAMERA_ECO_BYPASS_ISP (1 << 31)

// spi_camera_reg3
#define CAMERA_LINE_WAIT_LENGTH(n) (((n)&0xffff) << 0)
#define CAMERA_BLOCK_WAIT_LENGTH(n) (((n)&0xff) << 16)
#define CAMERA_SSN_HIGH_LENGTH(n) (((n)&0xff) << 24)

// spi_camera_reg4
#define CAMERA_CAMERA_SPI_MASTER_EN_2 (1 << 0)
#define CAMERA_SDO_LINE_CHOOSE_BIT(n) (((n)&0x3) << 1)
#define CAMERA_DATA_SIZE_CHOOSE_BIT (1 << 3)
#define CAMERA_IMAGE_HEIGHT_CHOOSE_BIT (1 << 4)
#define CAMERA_IMAGE_WIDTH_CHOOSE_BIT (1 << 5)
#define CAMERA_BLOCK_NUM_PER_PACKET(n) (((n)&0x3ff) << 6)
#define CAMERA_SPI_DATA0_PHASE_SEL(n) (((n)&0x3) << 16)
#define CAMERA_SPI_DATA1_PHASE_SEL(n) (((n)&0x3) << 18)

// spi_camera_reg5
#define CAMERA_SYNC_CODE(n) (((n)&0xffffff) << 0)

// spi_camera_reg6
#define CAMERA_PACKET_ID_DATA_START(n) (((n)&0xff) << 0)
#define CAMERA_PACKET_ID_LINE_START(n) (((n)&0xff) << 8)
#define CAMERA_PACKET_ID_FRAME_END(n) (((n)&0xff) << 16)
#define CAMERA_PACKET_ID_FRAME_START(n) (((n)&0xff) << 24)

// spi_camera_obs0
#define CAMERA_LINE_ID_15_0_(n) (((n)&0xffff) << 0)
#define CAMERA_DATA_ID_7_0_(n) (((n)&0xff) << 16)
#define CAMERA_OBSERVE_DATA_SIZE_WRONG (1 << 24)
#define CAMERA_OBSERVE_IMAGE_HEIGHT_WRONG (1 << 25)
#define CAMERA_OBSERVE_IMAGE_WIDTH_WRONG (1 << 26)
#define CAMERA_OBSERVE_LINE_NUM_WRONG (1 << 27)
#define CAMERA_OBSERVE_DATA_ID_WRONG (1 << 28)

// spi_camera_obs1
#define CAMERA_IMAGE_HEIGHT(n) (((n)&0xffff) << 0)
#define CAMERA_IMAGE_WIDTH(n) (((n)&0xffff) << 16)

// csi_config_reg0
#define CAMERA_CSI_CONFIG_REG0_NUM_D_TERM_EN(n) (((n)&0xff) << 0)
#define CAMERA_CUR_FRAME_LINE_NUM(n) (((n)&0x1fff) << 8)
#define CAMERA_DATA_LP_IN_CHOOSE_BIT(n) (((n)&0x3) << 21)
#define CAMERA_CLK_LP_INV (1 << 23)
#define CAMERA_TRAIL_DATA_WRONG_CHOOSE_BIT (1 << 24)
#define CAMERA_SYNC_BYPASS (1 << 25)
#define CAMERA_RDATA_BIT_INV_EN (1 << 26)
#define CAMERA_HS_SYNC_FIND_EN (1 << 27)
#define CAMERA_LINE_PACKET_ENABLE (1 << 28)
#define CAMERA_ECC_BYPASS (1 << 29)
#define CAMERA_DATA_LANE_CHOOSE_BIT (1 << 30)
#define CAMERA_CSI_MODULE_ENABLE (1 << 31)

// csi_config_reg1
#define CAMERA_CSI_CONFIG_REG1_NUM_HS_SETTLE(n) (((n)&0xff) << 0)
#define CAMERA_LP_DATA_LENGTH_CHOOSE_BIT(n) (((n)&0x7) << 8)
#define CAMERA_DATA_CLK_LP_POSEDGE_CHOOSE(n) (((n)&0x7) << 11)
#define CAMERA_CLK_LP_CK_INV (1 << 14)
#define CAMERA_RCLR_MASK_EN (1 << 15)
#define CAMERA_RINC_MASK_EN (1 << 16)
#define CAMERA_HS_ENABLE_MASK_EN (1 << 17)
#define CAMERA_DEN_CSI_INV_BIT (1 << 18)
#define CAMERA_HSYNC_CSI_INV_BIT (1 << 19)
#define CAMERA_VSYNC_CSI_INV_BIT (1 << 20)
#define CAMERA_HS_DATA2_ENABLE_REG (1 << 21)
#define CAMERA_HS_DATA1_ENABLE_REG (1 << 22)
#define CAMERA_HS_DATA1_ENABLE_CHOOSE_BIT (1 << 23)
#define CAMERA_HS_DATA1_ENABLE_DR (1 << 24)
#define CAMERA_DATA2_TERMINAL_ENABLE_REG (1 << 25)
#define CAMERA_DATA1_TERMINAL_ENABLE_REG (1 << 26)
#define CAMERA_DATA1_TERMINAL_ENABLE_DR (1 << 27)
#define CAMERA_LP_DATA_INTERRUPT_CLR (1 << 28)
#define CAMERA_LP_CMD_INTERRUPT_CLR (1 << 29)
#define CAMERA_LP_DATA_CLR (1 << 30)
#define CAMERA_LP_CMD_CLR (1 << 31)

// csi_config_reg2
#define CAMERA_NUM_HS_SETTLE_CLK(n) (((n)&0xffff) << 0)
#define CAMERA_NUM_C_TERM_EN(n) (((n)&0xffff) << 16)

// csi_config_reg3
#define CAMERA_CLK_LP_IN_CHOOSE_BIT(n) (((n)&0x3) << 6)
#define CAMERA_PU_LPRX_REG (1 << 8)
#define CAMERA_PU_HSRX_REG (1 << 9)
#define CAMERA_PU_DR (1 << 10)
#define CAMERA_DATA_PNSW_REG (1 << 11)
#define CAMERA_HS_CLK_ENABLE_REG (1 << 12)
#define CAMERA_HS_CLK_ENABLE_CHOOSE_BIT (1 << 13)
#define CAMERA_HS_CLK_ENABLE_DR (1 << 14)
#define CAMERA_CLK_TERMINAL_ENABLE_REG (1 << 15)
#define CAMERA_CLK_TERMINAL_ENABLE_DR (1 << 16)
#define CAMERA_OBSERVE_REG_5_LOW8_CHOOSE (1 << 17)
#define CAMERA_ECC_ERROR_FLAG_REG (1 << 18)
#define CAMERA_ECC_ERROR_DR (1 << 19)
#define CAMERA_CSI_CHANNEL_SEL (1 << 20)
#define CAMERA_TWO_LANE_BIT_REVERSE (1 << 21)
#define CAMERA_DATA2_LANE_BIT_REVERSE (1 << 22)
#define CAMERA_DATA1_LANE_BIT_REVERSE (1 << 23)
#define CAMERA_DATA2_HS_NO_MASK (1 << 24)
#define CAMERA_DATA1_HS_NO_MASK (1 << 25)
#define CAMERA_PU_LPRX_D2_REG (1 << 26)
#define CAMERA_PU_LPRX_D1_REG (1 << 27)
#define CAMERA_CLK_EDGE_SEL (1 << 29)
#define CAMERA_CLK_X2_SEL (1 << 30)
#define CAMERA_SINGLE_DATA_LANE_EN (1 << 31)

// csi_config_reg4
#define CAMERA_NUM_HS_CLK_USEFUL(n) (((n)&0x7fffffff) << 0)
#define CAMERA_NUM_HS_CLK_USEFUL_EN (1 << 31)

// csi_config_reg5
#define CAMERA_VC_ID_SET(n) (((n)&0x3) << 0)
#define CAMERA_DATA_LP_INV (1 << 2)
#define CAMERA_FIFO_RCLR_8809P_REG (1 << 3)
#define CAMERA_FIFO_WCLR_8809P_REG (1 << 4)
#define CAMERA_HS_SYNC_16BIT_8809P_MODE (1 << 5)
#define CAMERA_D_TERM_SMALL_8809P_EN (1 << 6)
#define CAMERA_DATA_LINE_INV_8809P_EN (1 << 7)
#define CAMERA_HS_ENABLE_8809P_MODE (1 << 8)
#define CAMERA_SP_TO_TRAIL_8809P_EN (1 << 9)
#define CAMERA_TRAIL_WRONG_8809P_BYPASS (1 << 10)
#define CAMERA_RINC_TRAIL_8809P_BYPASS (1 << 11)
#define CAMERA_HS_DATA_ENABLE_8809P_MODE (1 << 12)
#define CAMERA_HS_CLK_ENABLE_8809P_MODE (1 << 13)
#define CAMERA_DATA_TYPE_RE_CHECK_EN (1 << 14)
#define CAMERA_SYNC_ID_REG(n) (((n)&0xff) << 15)
#define CAMERA_SYNC_ID_DR (1 << 23)
#define CAMERA_CSI_OBSERVE_CHOOSE_BIT(n) (((n)&0x1f) << 24)
#define CAMERA_CRC_ERROR_FLAG_REG (1 << 29)
#define CAMERA_CRC_ERROR_FLAG_DR (1 << 30)
#define CAMERA_CSI_RINC_NEW_MODE_DIS (1 << 31)

// csi_config_reg6
#define CAMERA_DATA_TYPE_DP_REG(n) (((n)&0x3f) << 0)
#define CAMERA_DATA_TYPE_LE_REG(n) (((n)&0x3f) << 6)
#define CAMERA_DATA_TYPE_LS_REG(n) (((n)&0x3f) << 12)
#define CAMERA_DATA_TYPE_FE_REG(n) (((n)&0x3f) << 18)
#define CAMERA_DATA_TYPE_FS_REG(n) (((n)&0x3f) << 24)
#define CAMERA_DATA_TYPE_DP_DR (1 << 30)
#define CAMERA_DATA_TYPE_DR (1 << 31)

// csi_config_reg7
#define CAMERA_DATA_LANE_16BITS_MODE (1 << 2)
#define CAMERA_TERMINAL_2_HS_EXCHAGE_8809P (1 << 3)
#define CAMERA_TERMINAL_1_HS_EXCHAGE_8809P (1 << 4)
#define CAMERA_DATA2_TERMINAL_ENABLE_8809P_DR (1 << 5)
#define CAMERA_HS_DATA2_ENABLE_8809P_DR (1 << 6)
#define CAMERA_CSI_DOUT_TEST_8809P_EN (1 << 7)
#define CAMERA_CSI_DOUT_TEST_8809P(n) (((n)&0xff) << 8)
#define CAMERA_CSI_CONFIG_REG7_NUM_D_TERM_EN(n) (((n)&0xff) << 16)
#define CAMERA_CSI_CONFIG_REG7_NUM_HS_SETTLE(n) (((n)&0xff) << 24)

// csi_obs4
#define CAMERA_HS_DATA_STATE(n) (((n)&0x3fff) << 0)
#define CAMERA_PHY_DATA_STATE(n) (((n)&0x7fff) << 14)
#define CAMERA_FIFO_WFULL_ALMOST (1 << 29)
#define CAMERA_FIFO_WFULL (1 << 30)
#define CAMERA_FIFO_WEMPTY (1 << 31)

// csi_obs5
#define CAMERA_CSI_OBSERVE_REG_5_LOW(n) (((n)&0xff) << 0)
#define CAMERA_LP_DATA_INTERRUPT_FLAG (1 << 8)
#define CAMERA_LP_CMD_INTERRUPT_FLAG (1 << 9)
#define CAMERA_PHY_CLK_STATE(n) (((n)&0x1ff) << 10)
#define CAMERA_FIFO_RCOUNT(n) (((n)&0x1ff) << 19)
#define CAMERA_CRC_ERROR (1 << 28)
#define CAMERA_ERR_ECC_CORRECTED_FLAG (1 << 29)
#define CAMERA_ERR_DATA_CORRECTED_FLAG (1 << 30)
#define CAMERA_ERR_DATA_ZERO_FLAG (1 << 31)

// csi_obs6
#define CAMERA_CSI_OBSERVE_REG_6(n) (((n)&0xffffffff) << 0)

// csi_obs7
#define CAMERA_CSI_OBSERVE_REG_7(n) (((n)&0xffffffff) << 0)

// csi_enable
#define CAMERA_CSI_ENABLE (1 << 0)

// csi_config_reg8
#define CAMERA_DLY_SEL_CLKN_REG(n) (((n)&0xf) << 0)
#define CAMERA_DLY_SEL_CLKP_REG(n) (((n)&0xf) << 4)
#define CAMERA_DLY_SEL_DATA2_REG(n) (((n)&0xf) << 8)
#define CAMERA_DLY_SEL_DATA1_REG(n) (((n)&0xf) << 12)
#define CAMERA_VTH_SEL (1 << 16)

// ramdata
#define CAMERA_DATA(n) (((n)&0xffffffff) << 0)

// soft_reset
#define CAMERA_DSP_RESET (1 << 0)

// awb_x1_min
#define CAMERA_AWB_X1_MIN(n) (((n)&0xff) << 0)

// awb_x1_max
#define CAMERA_AWB_X1_MAX(n) (((n)&0xff) << 0)

// awb_y1_min
#define CAMERA_AWB_Y1_MIN(n) (((n)&0xff) << 0)

// awb_y1_max
#define CAMERA_AWB_Y1_MAX(n) (((n)&0xff) << 0)

// awb_x2_min
#define CAMERA_AWB_X2_MIN(n) (((n)&0xff) << 0)

// awb_x2_max
#define CAMERA_AWB_X2_MAX(n) (((n)&0xff) << 0)

// awb_y2_min
#define CAMERA_AWB_Y2_MIN(n) (((n)&0xff) << 0)

// awb_y2_max
#define CAMERA_AWB_Y2_MAX(n) (((n)&0xff) << 0)

// awb_x3_min
#define CAMERA_AWB_X3_MIN(n) (((n)&0xff) << 0)

// awb_x3_max
#define CAMERA_AWB_X3_MAX(n) (((n)&0xff) << 0)

// awb_y3_min
#define CAMERA_AWB_Y3_MIN(n) (((n)&0xff) << 0)

// awb_y3_max
#define CAMERA_AWB_Y3_MAX(n) (((n)&0xff) << 0)

// awb_x4_min
#define CAMERA_AWB_X4_MIN(n) (((n)&0xff) << 0)

// awb_x4_max
#define CAMERA_AWB_X4_MAX(n) (((n)&0xff) << 0)

// awb_y4_min
#define CAMERA_AWB_Y4_MIN(n) (((n)&0xff) << 0)

// awb_y4_max
#define CAMERA_AWB_Y4_MAX(n) (((n)&0xff) << 0)

// awb_x5_min
#define CAMERA_AWB_X5_MIN(n) (((n)&0xff) << 0)

// awb_x5_max
#define CAMERA_AWB_X5_MAX(n) (((n)&0xff) << 0)

// awb_y5_min
#define CAMERA_AWB_Y5_MIN(n) (((n)&0xff) << 0)

// awb_y5_max
#define CAMERA_AWB_Y5_MAX(n) (((n)&0xff) << 0)

// awb_skin_x1_min
#define CAMERA_AWB_SKIN_X1_MIN(n) (((n)&0xff) << 0)

// awb_skin_x1_max
#define CAMERA_AWB_SKIN_X1_MAX(n) (((n)&0xff) << 0)

// awb_skin_y1_min
#define CAMERA_AWB_SKIN_Y1_MIN(n) (((n)&0xff) << 0)

// awb_skin_y1_max
#define CAMERA_AWB_SKIN_Y1_MAX(n) (((n)&0xff) << 0)

// awb_skin_x2_min
#define CAMERA_AWB_SKIN_X2_MIN(n) (((n)&0xff) << 0)

// awb_skin_x2_max
#define CAMERA_AWB_SKIN_X2_MAX(n) (((n)&0xff) << 0)

// awb_skin_y2_min
#define CAMERA_AWB_SKIN_Y2_MIN(n) (((n)&0xff) << 0)

// awb_skin_y2_max
#define CAMERA_AWB_SKIN_Y2_MAX(n) (((n)&0xff) << 0)

// awb_ctd_msb
#define CAMERA_AWB_X1_MIN_MSB (1 << 0)
#define CAMERA_AWB_X1_MAX_MSB (1 << 1)
#define CAMERA_AWB_Y5_MIN_MSB (1 << 2)
#define CAMERA_AWB_Y5_MAX_MSB (1 << 3)
#define CAMERA_AWB_ADJ_MODE(n) (((n)&0x3) << 4)
#define CAMERA_AWB_RATIO_MODE(n) (((n)&0x3) << 6)

// int_dif_thr_mid
#define CAMERA_INT_DIF_THR_MID(n) (((n)&0xff) << 0)

// lb_soft_rstn
#define CAMERA_LB_SOFT_RSTN (1 << 0)

// vsync_end_high
#define CAMERA_VSYNC_END_HIGH(n) (((n)&0xff) << 0)

// vsync_end_low
#define CAMERA_VSYNC_END_LOW(n) (((n)&0xff) << 0)

// line_numl
#define CAMERA_LINE_NUML(n) (((n)&0xff) << 0)

// pix_numl
#define CAMERA_PIX_NUML(n) (((n)&0xff) << 0)

// pix_line_numh
#define CAMERA_LINE_NUMH (1 << 0)
#define CAMERA_PIX_NUMH_RSVD(n) (((n)&0x7) << 1)
#define CAMERA_PIX_NUMH(n) (((n)&0x3) << 4)
#define CAMERA_LINE_NUMH_RSVD(n) (((n)&0x3) << 6)

// lb_ctrl
#define CAMERA_LOW_ORDER (1 << 0)
#define CAMERA_USE_FB_REG (1 << 1)
#define CAMERA_NOT_CVP_REG (1 << 2)
#define CAMERA_FIRST_BYTE_REG(n) (((n)&0x7) << 3)

// data_format
#define CAMERA_DATA_FORMAT(n) (((n)&0x3) << 0)

// lb_enable
#define CAMERA_LB_ENABLE (1 << 0)

// vh_inv
#define CAMERA_HSYNC_INV (1 << 0)
#define CAMERA_VH_INV_VSYNC_INV (1 << 1)

// line_cnt_l
#define CAMERA_LINE_CNT_L(n) (((n)&0xff) << 0)

// line_cnt_h
#define CAMERA_LINE_CNT_H(n) (((n)&0x3) << 0)

// num_check
#define CAMERA_LINE_NUM_CHECK (1 << 0)
#define CAMERA_BYTE_NUM_CHECK (1 << 1)
#define CAMERA_LINE_NUM_CLEAR (1 << 4)
#define CAMERA_BYTE_NUM_CLEAR (1 << 5)

// dci_ctrl_reg
#define CAMERA_KL_LOW_LIGHT_FIX (1 << 0)
#define CAMERA_KL_REG_FIX (1 << 1)
#define CAMERA_KU_LOW_LIGHT_FIX (1 << 2)
#define CAMERA_KU_REG_FIX (1 << 3)
#define CAMERA_HOFST(n) (((n)&0x3) << 4)
#define CAMERA_VBH_SEL(n) (((n)&0x3) << 6)

// dci_ofst_reg
#define CAMERA_KL_OFSTX1(n) (((n)&0xf) << 0)
#define CAMERA_KU_OFSTX1(n) (((n)&0xf) << 4)

// dci_hist_reg
#define CAMERA_DK_HISTX1(n) (((n)&0xf) << 0)
#define CAMERA_BR_HISTX1(n) (((n)&0xf) << 4)

// ae_sw_ctrl_reg
#define CAMERA_NEXP_SW_IN(n) (((n)&0xf) << 0)
#define CAMERA_AE_EXT_ADJ_START (1 << 7)

// ae_thr_reg
#define CAMERA_THR_DARK(n) (((n)&0xf) << 0)
#define CAMERA_THR_BRIGHT(n) (((n)&0xf) << 4)

// ae_misc_ctrl_reg
#define CAMERA_OFST_DEC_LOW_SEL(n) (((n)&0x3) << 0)
#define CAMERA_OFST_DEC_HIGH_SEL(n) (((n)&0x3) << 2)
#define CAMERA_FORCE_ADJ1 (1 << 4)
#define CAMERA_FORCE_ADJ2 (1 << 5)
#define CAMERA_FORCE_ADJ3 (1 << 6)
#define CAMERA_INDEX_OFST_NO_STEP (1 << 7)

// csup_xx_reg
#define CAMERA_X_LOW(n) (((n)&0xf) << 0)
#define CAMERA_X_HIGH(n) (((n)&0xf) << 4)

// contr_ythr_reg
#define CAMERA_CSUP_GAIN_LOW_TH_H (1 << 0)
#define CAMERA_CSUP_GAIN_HIGH_TH(n) (((n)&0x7) << 1)
#define CAMERA_FIXED_CONTR_YTHR(n) (((n)&0xf) << 4)

// contr_yave_offset_reg
#define CAMERA_YAVE_OFFSET_REG(n) (((n)&0x3f) << 0)
#define CAMERA_YTHR_SEL (1 << 6)
#define CAMERA_YAVE_OFFSET_SIGN (1 << 7)

// contr_ku_lo_reg
#define CAMERA_KU(n) (((n)&0x7f) << 0)
#define CAMERA_KU_SIGN (1 << 7)

// contr_kl_lo_reg
#define CAMERA_KL(n) (((n)&0x7f) << 0)
#define CAMERA_KL_SIGN (1 << 7)

// contr_ku_mid_reg
#define CAMERA_KU(n) (((n)&0x7f) << 0)
#define CAMERA_KU_SIGN (1 << 7)

// contr_kl_mid_reg
#define CAMERA_KL(n) (((n)&0x7f) << 0)
#define CAMERA_KL_SIGN (1 << 7)

// contr_ku_hi_reg
#define CAMERA_KU(n) (((n)&0x7f) << 0)
#define CAMERA_KU_SIGN (1 << 7)

// contr_kl_hi_reg
#define CAMERA_KL(n) (((n)&0x7f) << 0)
#define CAMERA_KL_SIGN (1 << 7)

// luma_offset_lo_reg
#define CAMERA_OFFSET(n) (((n)&0x3f) << 0)
#define CAMERA_ALGO_SEL (1 << 6)
#define CAMERA_OFFSET_SIGN (1 << 7)

// luma_offset_mid_reg
#define CAMERA_OFFSET(n) (((n)&0x3f) << 0)
#define CAMERA_ALGO_SEL (1 << 6)
#define CAMERA_OFFSET_SIGN (1 << 7)

// luma_offset_hi_reg
#define CAMERA_OFFSET(n) (((n)&0x3f) << 0)
#define CAMERA_ALGO_SEL (1 << 6)
#define CAMERA_OFFSET_SIGN (1 << 7)

// u_gain_lo_reg
#define CAMERA_U_GAIN_LO_REG(n) (((n)&0xff) << 0)

// v_gain_lo_reg
#define CAMERA_V_GAIN_LO_REG(n) (((n)&0xff) << 0)

// u_gain_mid_reg
#define CAMERA_U_GAIN_MID_REG(n) (((n)&0xff) << 0)

// v_gain_mid_reg
#define CAMERA_V_GAIN_MID_REG(n) (((n)&0xff) << 0)

// u_gain_hi_reg
#define CAMERA_U_GAIN_HI_REG(n) (((n)&0xff) << 0)

// v_gain_hi_reg
#define CAMERA_V_GAIN_HI_REG(n) (((n)&0xff) << 0)

// again_sel_th0_reg
#define CAMERA_CONTR_GAIN_LOW_TH(n) (((n)&0x7) << 0)
#define CAMERA_AGAIN_SEL_TH0_RSVD (1 << 3)
#define CAMERA_CONTR_GAIN_HI_TH(n) (((n)&0x7) << 4)

// awb_cc_type_ctrl_reg
#define CAMERA_CC_TYPE_MODE(n) (((n)&0xf) << 0)
#define CAMERA_CC_GAIN_HI_TH(n) (((n)&0x7) << 4)
#define CAMERA_LUMA_FIRST (1 << 7)

// awb_cc_type_th_reg
#define CAMERA_R_BIG_TH(n) (((n)&0xf) << 0)
#define CAMERA_B_BIG_TH(n) (((n)&0xf) << 4)

// isp_wrapper_ctrl_1
#define CAMERA_POUT_MODE(n) (((n)&0x3) << 0)
#define CAMERA_YUV_MODE(n) (((n)&0x3) << 2)
#define CAMERA_VSYNC_TOGGLE (1 << 4)
#define CAMERA_MIPI_RSTN (1 << 5)
#define CAMERA_HSYNC_FIX (1 << 6)

// top_dummy
#define CAMERA_TOP_DUMMY(n) (((n)&0x7f) << 0)

// left_dummy
#define CAMERA_LEFT_DUMMY(n) (((n)&0xff) << 0)

// isp_wrapper_ctrl_2
#define CAMERA_RGB_MODE_REG(n) (((n)&0x7) << 0)
#define CAMERA_SUB_MODE (1 << 3)
#define CAMERA_MON_MODE_REG (1 << 4)
#define CAMERA_OCLK_INV_REG (1 << 5)
#define CAMERA_ISP_OUT_EN (1 << 6)

// line_num_l_reg
#define CAMERA_LINE_NUM_L_REG(n) (((n)&0x3f) << 0)

// pix_num_l_reg
#define CAMERA_PIX_NUM_L_REG(n) (((n)&0x7f) << 0)
#define CAMERA_CSI_MON_REG (1 << 7)

// v_dummy
#define CAMERA_VBOT_DUMMY_REG(n) (((n)&0xf) << 0)
#define CAMERA_VTOP_DUMMY_REG(n) (((n)&0xf) << 4)

// scg
#define CAMERA_KUKL_SEL (1 << 0)
#define CAMERA_REG94_RD_SEL (1 << 1)
#define CAMERA_BAYER_OUT_SEL (1 << 2)
#define CAMERA_CSUP_EN (1 << 3)
#define CAMERA_Y_GAMMA_EN(n) (((n)&0x3) << 4)
#define CAMERA_YUV_SDI_EN (1 << 6)
#define CAMERA_REG92_RD_SEL (1 << 7)

// y_gamma_b0
#define CAMERA_Y_GAMMA_B0(n) (((n)&0xff) << 0)

// y_gamma_b1
#define CAMERA_Y_GAMMA_B1(n) (((n)&0xff) << 0)

// y_gamma_b2
#define CAMERA_Y_GAMMA_B2(n) (((n)&0xff) << 0)

// y_gamma_b4
#define CAMERA_Y_GAMMA_B4(n) (((n)&0xff) << 0)

// y_gamma_b6
#define CAMERA_Y_GAMMA_B6(n) (((n)&0xff) << 0)

// y_gamma_b8
#define CAMERA_Y_GAMMA_B8(n) (((n)&0xff) << 0)

// y_gamma_b10
#define CAMERA_Y_GAMMA_B10(n) (((n)&0xff) << 0)

// y_gamma_b12
#define CAMERA_Y_GAMMA_B12(n) (((n)&0xff) << 0)

// y_gamma_b16
#define CAMERA_Y_GAMMA_B16(n) (((n)&0xff) << 0)

// y_gamma_b20
#define CAMERA_Y_GAMMA_B20(n) (((n)&0xff) << 0)

// y_gamma_b24
#define CAMERA_Y_GAMMA_B24(n) (((n)&0xff) << 0)

// y_gamma_b28
#define CAMERA_Y_GAMMA_B28(n) (((n)&0xff) << 0)

// y_gamma_b32
#define CAMERA_Y_GAMMA_B32(n) (((n)&0xff) << 0)

// r_awb_gain_in
#define CAMERA_R_AWB_GAIN_IN(n) (((n)&0xff) << 0)

// g_awb_gain_in
#define CAMERA_G_AWB_GAIN_IN(n) (((n)&0xff) << 0)

// b_awb_gain_in
#define CAMERA_B_AWB_GAIN_IN(n) (((n)&0xff) << 0)

// r_drc_gain_in
#define CAMERA_R_DRC_GAIN_IN(n) (((n)&0xff) << 0)

// gr_drc_gain_in
#define CAMERA_GR_DRC_GAIN_IN(n) (((n)&0xff) << 0)

// gb_drc_gain_in
#define CAMERA_GB_DRC_GAIN_IN(n) (((n)&0xff) << 0)

// b_drc_gain_in
#define CAMERA_B_DRC_GAIN_IN(n) (((n)&0xff) << 0)

// ae_ctrl
#define CAMERA_ANA_GAIN_IN(n) (((n)&0x3f) << 0)
#define CAMERA_AE_UPDATE_EN (1 << 6)
#define CAMERA_AE_EN (1 << 7)

// ae_ctrl2
#define CAMERA_AWB_ADJ_SEL(n) (((n)&0x3) << 0)
#define CAMERA_GAP_AE (1 << 2)
#define CAMERA_GAP_BE (1 << 3)
#define CAMERA_AE_ACTION_PERIOD(n) (((n)&0x7) << 4)
#define CAMERA_YAVE_MON_SEL (1 << 7)

// ae_ctrl3
#define CAMERA_YAVE_USE_MEAN(n) (((n)&0x3) << 0)
#define CAMERA_YAVE_DIFF_THR_REG(n) (((n)&0x3) << 2)
#define CAMERA_YAVE_SEL(n) (((n)&0x3) << 4)
#define CAMERA_YAVE_PLUS_BH_MODE (1 << 6)
#define CAMERA_YWAVE_PLUS_BH_MODE (1 << 7)

// ae_ctrl4
#define CAMERA_AE_HIST_BIG_EN (1 << 0)
#define CAMERA_AE_HIST_TOO_BIG_EN (1 << 1)
#define CAMERA_HIST_OFST0(n) (((n)&0x3) << 2)
#define CAMERA_INDEX_OFST0(n) (((n)&0x3) << 4)
#define CAMERA_INDEX_OFST1(n) (((n)&0x3) << 6)

// ae_win_start
#define CAMERA_PCNT_LEFT(n) (((n)&0xf) << 0)
#define CAMERA_LCNT_TOP(n) (((n)&0xf) << 4)

// ae_win_width
#define CAMERA_AE_WIN_WIDTH(n) (((n)&0xff) << 0)

// ae_win_height
#define CAMERA_AE_WIN_HEIGHT(n) (((n)&0xff) << 0)

// exp_init
#define CAMERA_EXP_INIT(n) (((n)&0xff) << 0)

// exp_ceil_init
#define CAMERA_EXP_CEIL_INIT(n) (((n)&0xf) << 0)

// ae_exp_1e
#define CAMERA_AE_EXP_1E(n) (((n)&0xff) << 0)

// ae_diff_thr
#define CAMERA_THR2_DARK(n) (((n)&0xf) << 0)
#define CAMERA_THR2_BRIGHT(n) (((n)&0xf) << 4)

// ae_bh_sel
#define CAMERA_BH_FACTOR_INDOOR(n) (((n)&0x7) << 0)
#define CAMERA_BH_FACTOR_OUTDOOR(n) (((n)&0x7) << 3)
#define CAMERA_BH_MEAN_SEL(n) (((n)&0x3) << 6)

// awb_ctrl
#define CAMERA_AWB_SW_MON_EN (1 << 0)
#define CAMERA_FAST_2X (1 << 1)
#define CAMERA_FAST_4X (1 << 2)
#define CAMERA_AWB_ACTION_PERIOD(n) (((n)&0x7) << 3)
#define CAMERA_AWB_UPDATE_EN (1 << 6)
#define CAMERA_AWB_EN (1 << 7)

// awb_ctrl2
#define CAMERA_AWB_MON_SEL(n) (((n)&0x7) << 0)
#define CAMERA_AWB_VLD_SEL (1 << 3)
#define CAMERA_AWB_VLD_MODE(n) (((n)&0x7) << 4)
#define CAMERA_AWB_ADJ (1 << 7)

// awb_y_max
#define CAMERA_AWB_Y_MAX(n) (((n)&0xff) << 0)

// awb_stop
#define CAMERA_AWB_STOP_CB_NEG_LEVEL(n) (((n)&0x3) << 0)
#define CAMERA_AWB_STOP_CB_POS_LEVEL(n) (((n)&0x3) << 2)
#define CAMERA_AWB_STOP_CR_NEG_LEVEL(n) (((n)&0x3) << 4)
#define CAMERA_AWB_STOP_CR_POS_LEVEL(n) (((n)&0x3) << 6)

// awb_algo
#define CAMERA_AWB_ALGO(n) (((n)&0xff) << 0)

// awb_ctrl3
#define CAMERA_CR_OFST_LT1X (1 << 0)
#define CAMERA_CR_OFST_GT1X (1 << 1)
#define CAMERA_CB_OFST_LT1X (1 << 2)
#define CAMERA_CB_OFST_GT1X (1 << 3)
#define CAMERA_AWB_SUM_VLD_SEL (1 << 4)
#define CAMERA_AWB_STOP_SEL_REG (1 << 5)
#define CAMERA_AWB_SKIN_SEL (1 << 6)
#define CAMERA_AWB_ALGO_MODE (1 << 7)

// awb_ctrl4
#define CAMERA_AWB_CTRL4(n) (((n)&0xff) << 0)

// dig_gain_in
#define CAMERA_DIG_GAIN_IN(n) (((n)&0xff) << 0)

// y_init_thr
#define CAMERA_Y_INIT_MODE (1 << 0)
#define CAMERA_Y_LOW_EN (1 << 1)
#define CAMERA_Y_HIGH_EN (1 << 2)
#define CAMERA_Y_LOW_THR(n) (((n)&0x1f) << 3)

// y_ave_target
#define CAMERA_Y_AVE_TARGET(n) (((n)&0xff) << 0)

// y_lmt_offset
#define CAMERA_Y_LOW_LIMIT(n) (((n)&0x7) << 0)
#define CAMERA_Y_LMT_OFST (1 << 3)
#define CAMERA_Y_HIGH_LIMIT(n) (((n)&0x7) << 4)

// again_sel_th2
#define CAMERA_YNR_GAIN_LOW_TH(n) (((n)&0x7) << 0)
#define CAMERA_AGAIN_SEL_TH2 (1 << 3)
#define CAMERA_YNR_GAIN_HI_TH(n) (((n)&0x7) << 4)

// yave_target_chg1
#define CAMERA_YAVE_TARGET_OFST_L(n) (((n)&0xf) << 0)
#define CAMERA_YAVE_TARGET_OFST_H(n) (((n)&0xf) << 4)

// image_eff_reg
#define CAMERA_GREY_EN (1 << 0)
#define CAMERA_SEPIA_EN (1 << 1)
#define CAMERA_NEGATIVE_EN (1 << 2)
#define CAMERA_COLOR_BAR_EN (1 << 3)
#define CAMERA_IMAGE_EFF_RSVD (1 << 4)
#define CAMERA_REG93_SEL (1 << 5)
#define CAMERA_REG94_SEL (1 << 6)
#define CAMERA_SHARP_MON (1 << 7)

// ywave_out
#define CAMERA_YWAVE_OUT(n) (((n)&0xff) << 0)

// ae_bright_hist
#define CAMERA_AE_BRIGHT_HIST(n) (((n)&0xff) << 0)

// yave_out
#define CAMERA_YAVE_OUT(n) (((n)&0xff) << 0)

// exp_out
#define CAMERA_EXP_OUT(n) (((n)&0xff) << 0)

// misc_out
#define CAMERA_EXP_OUT_H(n) (((n)&0x7) << 0)
#define CAMERA_AWB_OK (1 << 3)
#define CAMERA_NEXP_SEL(n) (((n)&0x3) << 4)
#define CAMERA_FIXED_0 (1 << 6)
#define CAMERA_AE_OK (1 << 7)

// awb_debug_out
#define CAMERA_AWB_CRGT(n) (((n)&0x3) << 0)
#define CAMERA_AWB_CBGT(n) (((n)&0x3) << 2)
#define CAMERA_AWB_CRSUM_SIGN (1 << 4)
#define CAMERA_AWB_CBSUM_SIGN (1 << 5)
#define CAMERA_AWB_CBCR (1 << 6)
#define CAMERA_AWB_SUM_VLD (1 << 7)

// mono_color
#define CAMERA_MONO_COLOR(n) (((n)&0xff) << 0)

// r_awb_gain
#define CAMERA_R_AWB_GAIN(n) (((n)&0xff) << 0)

// b_awb_gain
#define CAMERA_B_AWB_GAIN(n) (((n)&0xff) << 0)

// misc_status
#define CAMERA_ANA_GAIN_OUT(n) (((n)&0x3f) << 0)
#define CAMERA_CC_TYPE (1 << 6)
#define CAMERA_IS_OUTDOOR (1 << 7)

// yave_contr
#define CAMERA_YAVE_CONTR(n) (((n)&0xff) << 0)

// gamma_type
#define CAMERA_GAMMA_TYPE_MODE(n) (((n)&0x7) << 0)
#define CAMERA_GAMMA_GAIN_HI_TH(n) (((n)&0x7) << 3)
#define CAMERA_VGAS(n) (((n)&0x3) << 6)

// blc_line
#define CAMERA_BLC_LINE(n) (((n)&0xff) << 0)

// lsc_xx
#define CAMERA_X_LOW(n) (((n)&0xf) << 0)
#define CAMERA_X_HIGH(n) (((n)&0xf) << 4)

// lsc_blc_gain_th
#define CAMERA_LSC_GAIN_LOW_TH(n) (((n)&0x7) << 0)
#define CAMERA_LSC_GAIN_HI_TH(n) (((n)&0x7) << 3)
#define CAMERA_CSUP_GAIN_LOW_TH(n) (((n)&0x3) << 6)

// blc_ctrl
#define CAMERA_BLC_OUT_MODE(n) (((n)&0x3) << 0)
#define CAMERA_LINE_INIT_H (1 << 2)
#define CAMERA_BLC_OFST_SIGN (1 << 3)
#define CAMERA_BLC_MODE(n) (((n)&0x3) << 4)
#define CAMERA_BLC_SEL (1 << 6)
#define CAMERA_BLC_EN (1 << 7)

// blc_init
#define CAMERA_BLC00_OFST(n) (((n)&0xf) << 0)
#define CAMERA_BLC01_OFST(n) (((n)&0xf) << 4)

// blc_offset
#define CAMERA_BLC10_OFST(n) (((n)&0xf) << 0)
#define CAMERA_BLC11_OFST(n) (((n)&0xf) << 4)

// blc_thr
#define CAMERA_BLC_THR(n) (((n)&0x3f) << 0)

// lsc_xy_cent
#define CAMERA_Y_CENT(n) (((n)&0xf) << 0)
#define CAMERA_X_CENT(n) (((n)&0xf) << 4)

// cnr_dif_thr
#define CAMERA_CNR_V_EN (1 << 0)
#define CAMERA_CNR_H_EN (1 << 1)
#define CAMERA_VCNR_SEL (1 << 2)
#define CAMERA_EDGE_MON (1 << 3)
#define CAMERA_AWB_SKIN_MODE(n) (((n)&0x7) << 4)
#define CAMERA_GAMMA_TYPE (1 << 7)

// cnr_thr
#define CAMERA_CNR_THR_V(n) (((n)&0x7) << 0)
#define CAMERA_EDGE_EN_V (1 << 3)
#define CAMERA_CNR_THR_H(n) (((n)&0x7) << 4)
#define CAMERA_EDGE_EN_H (1 << 7)

// gamma_ctrl
#define CAMERA_GAMMA_P_ID (1 << 0)
#define CAMERA_GAMMA_L_ID (1 << 1)
#define CAMERA_GAMMA_EN_NON_OUTDOOR (1 << 2)
#define CAMERA_GAMMA_EN_OUTDOOR (1 << 3)
#define CAMERA_LSC_P_ID (1 << 4)
#define CAMERA_LSC_L_ID (1 << 5)
#define CAMERA_LSC_EN_NON_OUTDOOR (1 << 6)
#define CAMERA_LSC_EN_OUTDOOR (1 << 7)

// bayer_gamma_b0
#define CAMERA_BAYER_GAMMA_B0(n) (((n)&0xff) << 0)

// bayer_gamma_b1
#define CAMERA_BAYER_GAMMA_B1(n) (((n)&0xff) << 0)

// bayer_gamma_b2
#define CAMERA_BAYER_GAMMA_B2(n) (((n)&0xff) << 0)

// bayer_gamma_b3
#define CAMERA_BAYER_GAMMA_B3(n) (((n)&0xff) << 0)

// bayer_gamma_b4
#define CAMERA_BAYER_GAMMA_B4(n) (((n)&0xff) << 0)

// bayer_gamma_b6
#define CAMERA_BAYER_GAMMA_B6(n) (((n)&0xff) << 0)

// bayer_gamma_b8
#define CAMERA_BAYER_GAMMA_B8(n) (((n)&0xff) << 0)

// bayer_gamma_b10
#define CAMERA_BAYER_GAMMA_B10(n) (((n)&0xff) << 0)

// bayer_gamma_b12
#define CAMERA_BAYER_GAMMA_B12(n) (((n)&0xff) << 0)

// bayer_gamma_b16
#define CAMERA_BAYER_GAMMA_B16(n) (((n)&0xff) << 0)

// bayer_gamma_b20
#define CAMERA_BAYER_GAMMA_B20(n) (((n)&0xff) << 0)

// bayer_gamma_b24
#define CAMERA_BAYER_GAMMA_B24(n) (((n)&0xff) << 0)

// bayer_gamma_b28
#define CAMERA_BAYER_GAMMA_B28(n) (((n)&0xff) << 0)

// bayer_gamma_b32
#define CAMERA_BAYER_GAMMA_B32(n) (((n)&0xff) << 0)

// bayer_gamma_b36
#define CAMERA_BAYER_GAMMA_B36(n) (((n)&0xff) << 0)

// bayer_gamma_b40
#define CAMERA_BAYER_GAMMA_B40(n) (((n)&0xff) << 0)

// bayer_gamma_b48
#define CAMERA_BAYER_GAMMA_B48(n) (((n)&0xff) << 0)

// bayer_gamma_b56
#define CAMERA_BAYER_GAMMA_B56(n) (((n)&0xff) << 0)

// bayer_gamma_b64
#define CAMERA_BAYER_GAMMA_B64(n) (((n)&0xff) << 0)

// blc_out0
#define CAMERA_BLC_OUT0(n) (((n)&0xff) << 0)

// blc_out1
#define CAMERA_BLC_OUT1(n) (((n)&0xff) << 0)

// dpc_ctrl_0
#define CAMERA_DPC_ON (1 << 0)
#define CAMERA_ADP_MED_SEL (1 << 1)
#define CAMERA_ANA_GAIN_CMP(n) (((n)&0x3) << 2)
#define CAMERA_RSVD (1 << 4)
#define CAMERA_NRF_GAUS_SEL (1 << 5)
#define CAMERA_BAYER_NR_ON (1 << 6)
#define CAMERA_CC_ON (1 << 7)

// dpc_ctrl_1
#define CAMERA_INT_FLG_CMP(n) (((n)&0x3) << 0)
#define CAMERA_ABS_SIGN_ALL_CMP(n) (((n)&0x3) << 2)
#define CAMERA_INT_DIF_SEL (1 << 4)

// y_thr_lo
#define CAMERA_Y_THR_LO(n) (((n)&0xff) << 0)

// y_thr_mid
#define CAMERA_Y_THR_MID(n) (((n)&0xff) << 0)

// y_thr_hi
#define CAMERA_Y_THR_HI(n) (((n)&0xff) << 0)

// intp_cfa_hv
#define CAMERA_CFA_V_THR_L(n) (((n)&0x7) << 0)
#define CAMERA_RSVD1 (1 << 3)
#define CAMERA_CFA_H_THR_L(n) (((n)&0x7) << 4)
#define CAMERA_RSVD2 (1 << 7)

// manual_adj
#define CAMERA_B_GAIN_ADJ (1 << 0)
#define CAMERA_G_GAIN_ADJ (1 << 1)
#define CAMERA_R_GAIN_ADJ (1 << 2)
#define CAMERA_ANA_GAIN_ADJ (1 << 3)
#define CAMERA_ADJ_DIRECTION (1 << 4)
#define CAMERA_INDEX_MANUAL_ADJ (1 << 5)
#define CAMERA_IN_CAPTURE_AWB (1 << 6)
#define CAMERA_IN_CAPTURE_AE (1 << 7)

// dpc_int_thr_lo
#define CAMERA_DPC_INT_THR_LO(n) (((n)&0xff) << 0)

// dpc_int_thr_hi
#define CAMERA_DPC_INT_THR_HI(n) (((n)&0xff) << 0)

// again_sel_th1
#define CAMERA_BNR_GAIN_LOW_TH(n) (((n)&0x7) << 0)
#define CAMERA_AGAIN_SEL_TH1_RSVD (1 << 3)
#define CAMERA_BNR_GAIN_HI_TH(n) (((n)&0x7) << 4)

// dpc_nr_lf_str_lo
#define CAMERA_DPC_NR_LF_STR_LO(n) (((n)&0xff) << 0)

// dpc_nr_hf_str_lo
#define CAMERA_DPC_NR_HF_STR_LO(n) (((n)&0xff) << 0)

// dpc_nr_area_thr_lo
#define CAMERA_DPC_NR_AREA_THR_LO(n) (((n)&0xff) << 0)

// dpc_nr_lf_str_mid
#define CAMERA_DPC_NR_LF_STR_MID(n) (((n)&0xff) << 0)

// dpc_nr_hf_str_mid
#define CAMERA_DPC_NR_HF_STR_MID(n) (((n)&0xff) << 0)

// dpc_nr_area_thr_mid
#define CAMERA_DPC_NR_AREA_THR_MID(n) (((n)&0xff) << 0)

// dpc_nr_lf_str_hi
#define CAMERA_DPC_NR_LF_STR_HI(n) (((n)&0xff) << 0)

// dpc_nr_hf_str_hi
#define CAMERA_DPC_NR_HF_STR_HI(n) (((n)&0xff) << 0)

// dpc_nr_area_thr_hi
#define CAMERA_DPC_NR_AREA_THR_HI(n) (((n)&0xff) << 0)

// intp_ctrl
#define CAMERA_PID_INV_EN (1 << 0)
#define CAMERA_LID_INV_EN (1 << 1)
#define CAMERA_GFILTER_EN (1 << 2)
#define CAMERA_GFILTER3_EN (1 << 3)
#define CAMERA_GFLITER5_EN (1 << 4)
#define CAMERA_SORT_SEL(n) (((n)&0x7) << 5)

// intp_cfa_h_thr
#define CAMERA_INTP_CFA_H_THR(n) (((n)&0xff) << 0)

// intp_cfa_v_thr
#define CAMERA_INTP_CFA_V_THR(n) (((n)&0xff) << 0)

// intp_grgb_sel_lmt
#define CAMERA_INTP_GRGB_SEL_LMT(n) (((n)&0xff) << 0)

// intp_gf_lmt_thr
#define CAMERA_INTP_GF_LMT_THR(n) (((n)&0xff) << 0)

// cc_r_offset
#define CAMERA_CC_R_OFFSET(n) (((n)&0xff) << 0)

// cc_g_offset
#define CAMERA_CC_G_OFFSET(n) (((n)&0xff) << 0)

// cc_b_offset
#define CAMERA_CC_B_OFFSET(n) (((n)&0xff) << 0)

// cc_00
#define CAMERA_CC_00(n) (((n)&0xff) << 0)

// cc_01
#define CAMERA_CC_01(n) (((n)&0xff) << 0)

// cc_10
#define CAMERA_CC_10(n) (((n)&0xff) << 0)

// cc_11
#define CAMERA_CC_11(n) (((n)&0xff) << 0)

// cc_20
#define CAMERA_CC_20(n) (((n)&0xff) << 0)

// cc_21
#define CAMERA_CC_21(n) (((n)&0xff) << 0)

// cc_r_offset_post
#define CAMERA_CC_R_OFFSET_POST(n) (((n)&0xff) << 0)

// cc_g_offset_post
#define CAMERA_CC_G_OFFSET_POST(n) (((n)&0xff) << 0)

// cc_b_offset_post
#define CAMERA_CC_B_OFFSET_POST(n) (((n)&0xff) << 0)

// cc2_r_offset
#define CAMERA_CC2_R_OFFSET(n) (((n)&0xff) << 0)

// cc2_g_offset
#define CAMERA_CC2_G_OFFSET(n) (((n)&0xff) << 0)

// cc2_b_offset
#define CAMERA_CC2_B_OFFSET(n) (((n)&0xff) << 0)

// cc2_00
#define CAMERA_CC2_00(n) (((n)&0xff) << 0)

// cc2_01
#define CAMERA_CC2_01(n) (((n)&0xff) << 0)

// cc2_10
#define CAMERA_CC2_10(n) (((n)&0xff) << 0)

// cc2_11
#define CAMERA_CC2_11(n) (((n)&0xff) << 0)

// cc2_20
#define CAMERA_CC2_20(n) (((n)&0xff) << 0)

// cc2_21
#define CAMERA_CC2_21(n) (((n)&0xff) << 0)

// sharp_lmt
#define CAMERA_SHARP_LMT(n) (((n)&0x7f) << 0)
#define CAMERA_SHARP_FINAL_H (1 << 7)

// sharp_mode
#define CAMERA_SHARP_CMP_GAP_LO(n) (((n)&0xf) << 0)
#define CAMERA_SHARP_FINAL(n) (((n)&0x3) << 4)
#define CAMERA_SHARP_SEL (1 << 6)
#define CAMERA_RGB_TEST_PATTERN (1 << 7)

// sharp_gain_str_lo
#define CAMERA_SHARP_GAIN_STR_LO(n) (((n)&0xff) << 0)

// sharp_nr_area_thr_lo
#define CAMERA_SHARP_NR_AREA_THR_LO(n) (((n)&0x7f) << 0)

// sharp_gain_str_mid
#define CAMERA_SHARP_GAIN_STR_MID(n) (((n)&0xff) << 0)

// sharp_nr_area_thr_mid
#define CAMERA_SHARP_NR_AREA_THR_MID(n) (((n)&0x7f) << 0)

// sharp_gain_str_hi
#define CAMERA_SHARP_GAIN_STR_HI(n) (((n)&0xff) << 0)

// sharp_nr_area_thr_hi
#define CAMERA_SHARP_NR_AREA_THR_HI(n) (((n)&0x7f) << 0)

// ynr_ctrl_reg
#define CAMERA_YNR_ON (1 << 0)
#define CAMERA_YNR_EDGE_METHODE(n) (((n)&0x3) << 1)
#define CAMERA_SHARP_ON (1 << 3)
#define CAMERA_SHARP_PLUS_MODE(n) (((n)&0x3) << 4)
#define CAMERA_Y_AE_SEL(n) (((n)&0x3) << 6)

// ynr_lf_method_str
#define CAMERA_YNR_LF_METHOD_STR(n) (((n)&0xff) << 0)

// ynr_lf_str_lo
#define CAMERA_YNR_LF_STR_LO(n) (((n)&0xff) << 0)

// ynr_hf_str_lo
#define CAMERA_YNR_HF_STR_LO(n) (((n)&0xff) << 0)

// ynr_area_thr_lo
#define CAMERA_YNR_AREA_THR_LO(n) (((n)&0xff) << 0)

// ynr_lf_str_mid
#define CAMERA_YNR_LF_STR_MID(n) (((n)&0xff) << 0)

// ynr_hf_str_mid
#define CAMERA_YNR_HF_STR_MID(n) (((n)&0xff) << 0)

// ynr_area_thr_mid
#define CAMERA_YNR_AREA_THR_MID(n) (((n)&0xff) << 0)

// ynr_lf_str_hi
#define CAMERA_YNR_LF_STR_HI(n) (((n)&0xff) << 0)

// ynr_hf_str_hi
#define CAMERA_YNR_HF_STR_HI(n) (((n)&0xff) << 0)

// ynr_area_thr_hi
#define CAMERA_YNR_AREA_THR_HI(n) (((n)&0xff) << 0)

// hue_sin_reg
#define CAMERA_HUE_SIN_REG(n) (((n)&0xff) << 0)

// hue_cos_reg
#define CAMERA_HUE_COSX_REG(n) (((n)&0x7f) << 0)
#define CAMERA_SIN_SIGN_REG (1 << 7)

// cnr_1d_ctrl_reg
#define CAMERA_CNR_DIF_THR_MID(n) (((n)&0xf) << 0)
#define CAMERA_CNR_1D_ON (1 << 4)
#define CAMERA_SATUR_ON (1 << 5)
#define CAMERA_HUE_ON (1 << 6)

// cnr_xx_reg
#define CAMERA_CNR_DIF_THR_LOW(n) (((n)&0xf) << 0)
#define CAMERA_CNR_DIF_THR_HIGH(n) (((n)&0xf) << 4)

// in5_low_th_reg
#define CAMERA_IN5_LOW_TH_REG(n) (((n)&0xff) << 0)

// in5_high_th_reg
#define CAMERA_IN5_HIGH_TH_REG(n) (((n)&0xff) << 0)

// p2_up_r_reg
#define CAMERA_P2_UP_R_REG(n) (((n)&0xff) << 0)

// p2_up_g_reg
#define CAMERA_P2_UP_G_REG(n) (((n)&0xff) << 0)

// p2_up_b_reg
#define CAMERA_P2_UP_B_REG(n) (((n)&0xff) << 0)

// p2_down_r_reg
#define CAMERA_P2_DOWN_R_REG(n) (((n)&0xff) << 0)

// p2_down_g_reg
#define CAMERA_P2_DOWN_G_REG(n) (((n)&0xff) << 0)

// p2_down_b_reg
#define CAMERA_P2_DOWN_B_REG(n) (((n)&0xff) << 0)

// p2_left_r_reg
#define CAMERA_P2_LEFT_R_REG(n) (((n)&0xff) << 0)

// p2_left_g_reg
#define CAMERA_P2_LEFT_G_REG(n) (((n)&0xff) << 0)

// p2_left_b_reg
#define CAMERA_P2_LEFT_B_REG(n) (((n)&0xff) << 0)

// p2_right_r_reg
#define CAMERA_P2_RIGHT_R_REG(n) (((n)&0xff) << 0)

// p2_right_g_reg
#define CAMERA_P2_RIGHT_G_REG(n) (((n)&0xff) << 0)

// p2_right_b_reg
#define CAMERA_P2_RIGHT_B_REG(n) (((n)&0xff) << 0)

// p4_q1_r_reg
#define CAMERA_P4_Q1_R_REG(n) (((n)&0xff) << 0)

// p4_q1_g_reg
#define CAMERA_P4_Q1_G_REG(n) (((n)&0xff) << 0)

// p4_q1_b_reg
#define CAMERA_P4_Q1_B_REG(n) (((n)&0xff) << 0)

// p4_q2_r_reg
#define CAMERA_P4_Q2_R_REG(n) (((n)&0xff) << 0)

// p4_q2_g_reg
#define CAMERA_P4_Q2_G_REG(n) (((n)&0xff) << 0)

// p4_q2_b_reg
#define CAMERA_P4_Q2_B_REG(n) (((n)&0xff) << 0)

// p4_q3_r_reg
#define CAMERA_P4_Q3_R_REG(n) (((n)&0xff) << 0)

// p4_q3_g_reg
#define CAMERA_P4_Q3_G_REG(n) (((n)&0xff) << 0)

// p4_q3_b_reg
#define CAMERA_P4_Q3_B_REG(n) (((n)&0xff) << 0)

// p4_q4_r_reg
#define CAMERA_P4_Q4_R_REG(n) (((n)&0xff) << 0)

// p4_q4_g_reg
#define CAMERA_P4_Q4_G_REG(n) (((n)&0xff) << 0)

// p4_q4_b_reg
#define CAMERA_P4_Q4_B_REG(n) (((n)&0xff) << 0)

// ae_e00_sta_reg
#define CAMERA_AE_E00_STA_LINE(n) (((n)&0x3f) << 0)

// ae_e00_num_reg
#define CAMERA_AE_E00_NUM(n) (((n)&0xf) << 0)
#define CAMERA_AE_E00_INTERVAL(n) (((n)&0x3) << 4)

// ae_e01_sta_reg
#define CAMERA_AE_E01_STA_LINE(n) (((n)&0x3f) << 0)

// ae_e01_num_reg
#define CAMERA_AE_E01_NUM(n) (((n)&0xf) << 0)
#define CAMERA_AE_E01_INTERVAL(n) (((n)&0x7) << 4)

// ae_e02_sta_reg
#define CAMERA_AE_E02_STA_LINE(n) (((n)&0x7f) << 0)

// ae_e02_num_reg
#define CAMERA_AE_E02_NUM(n) (((n)&0xf) << 0)
#define CAMERA_AE_E02_INTERVAL(n) (((n)&0xf) << 4)

// ae_e1_sta_reg
#define CAMERA_AE_E1_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_e1_num_reg
#define CAMERA_AE_E1_NUM_REG(n) (((n)&0xf) << 0)

// ae_e2_sta_reg
#define CAMERA_AE_E2_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_e2_num_reg
#define CAMERA_AE_E2_NUM_REG(n) (((n)&0xf) << 0)

// ae_e3_sta_reg
#define CAMERA_AE_E3_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_e3_num_reg
#define CAMERA_AE_E3_NUM_REG(n) (((n)&0xf) << 0)

// ae_e4_sta_reg
#define CAMERA_AE_E4_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_e4_num_reg
#define CAMERA_AE_E4_NUM_REG(n) (((n)&0x1f) << 0)

// ae_e5_sta_reg
#define CAMERA_AE_E5_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_e5_num_reg
#define CAMERA_AE_E5_NUM_REG(n) (((n)&0x1f) << 0)

// ae_e6_sta_reg
#define CAMERA_AE_E6_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_e6_num_reg
#define CAMERA_AE_E6_NUM_REG(n) (((n)&0xf) << 0)

// ae_e7_sta_reg
#define CAMERA_AE_E7_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_e7_num_reg
#define CAMERA_AE_E7_NUM_REG(n) (((n)&0xf) << 0)

// ae_e8_sta_reg
#define CAMERA_AE_E8_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_e8_num_reg
#define CAMERA_AE_E8_NUM_REG(n) (((n)&0xf) << 0)

// ae_e9_sta_reg
#define CAMERA_AE_E9_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_e9_num_reg
#define CAMERA_AE_E9_NUM_REG(n) (((n)&0xf) << 0)

// ae_ea_sta_reg
#define CAMERA_AE_EA_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_ea_num_reg
#define CAMERA_AE_EA_NUM_REG(n) (((n)&0xf) << 0)

// ae_eb_sta_reg
#define CAMERA_AE_EB_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_eb_num_reg
#define CAMERA_AE_EB_NUM_REG(n) (((n)&0xf) << 0)

// ae_ec_sta_reg
#define CAMERA_AE_EC_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_ec_num_reg
#define CAMERA_AE_EC_NUM_REG(n) (((n)&0xf) << 0)

// ae_ed_sta_reg
#define CAMERA_AE_ED_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_ed_num_reg
#define CAMERA_AE_ED_NUM_REG(n) (((n)&0xf) << 0)

// bayer_gamma2_b0
#define CAMERA_BAYER_GAMMA2_B0(n) (((n)&0xff) << 0)

// bayer_gamma2_b1
#define CAMERA_BAYER_GAMMA2_B1(n) (((n)&0xff) << 0)

// bayer_gamma2_b2
#define CAMERA_BAYER_GAMMA2_B2(n) (((n)&0xff) << 0)

// bayer_gamma2_b3
#define CAMERA_BAYER_GAMMA2_B3(n) (((n)&0xff) << 0)

// bayer_gamma2_b4
#define CAMERA_BAYER_GAMMA2_B4(n) (((n)&0xff) << 0)

// bayer_gamma2_b6
#define CAMERA_BAYER_GAMMA2_B6(n) (((n)&0xff) << 0)

// bayer_gamma2_b8
#define CAMERA_BAYER_GAMMA2_B8(n) (((n)&0xff) << 0)

// bayer_gamma2_b10
#define CAMERA_BAYER_GAMMA2_B10(n) (((n)&0xff) << 0)

// bayer_gamma2_b12
#define CAMERA_BAYER_GAMMA2_B12(n) (((n)&0xff) << 0)

// bayer_gamma2_b16
#define CAMERA_BAYER_GAMMA2_B16(n) (((n)&0xff) << 0)

// bayer_gamma2_b20
#define CAMERA_BAYER_GAMMA2_B20(n) (((n)&0xff) << 0)

// bayer_gamma2_b24
#define CAMERA_BAYER_GAMMA2_B24(n) (((n)&0xff) << 0)

// bayer_gamma2_b28
#define CAMERA_BAYER_GAMMA2_B28(n) (((n)&0xff) << 0)

// bayer_gamma2_b32
#define CAMERA_BAYER_GAMMA2_B32(n) (((n)&0xff) << 0)

// bayer_gamma2_b36
#define CAMERA_BAYER_GAMMA2_B36(n) (((n)&0xff) << 0)

// bayer_gamma2_b40
#define CAMERA_BAYER_GAMMA2_B40(n) (((n)&0xff) << 0)

// bayer_gamma2_b48
#define CAMERA_BAYER_GAMMA2_B48(n) (((n)&0xff) << 0)

// bayer_gamma2_b56
#define CAMERA_BAYER_GAMMA2_B56(n) (((n)&0xff) << 0)

// bayer_gamma2_b64
#define CAMERA_BAYER_GAMMA2_B64(n) (((n)&0xff) << 0)

// y_thr7_lo_reg
#define CAMERA_Y_THR7_LO_REG(n) (((n)&0xff) << 0)

// y_thr7_mid_reg
#define CAMERA_Y_THR7_MID_REG(n) (((n)&0xff) << 0)

// y_thr7_hi_reg
#define CAMERA_Y_THR7_HI_REG(n) (((n)&0xff) << 0)

// dpa_new_ctrl_reg
#define CAMERA_INFLG_CTRL_REG_0 (1 << 0)
#define CAMERA_INFLG_CTRL_REG_1 (1 << 1)
#define CAMERA_INFLG_CTRL_REG_2 (1 << 2)

// dpa_new_ctrl_hi_reg
#define CAMERA_INFLG_CTRL_REG0_H (1 << 0)
#define CAMERA_INFLG_CTRL_REG1_H (1 << 1)
#define CAMERA_INFLG_CTRL_REG2_H (1 << 2)
#define CAMERA_THRESHOLD_RSVD(n) (((n)&0x3) << 3)

// ae_index_gap
#define CAMERA_GAP_2E (1 << 0)
#define CAMERA_GAP_3E (1 << 1)
#define CAMERA_GAP_4E (1 << 2)
#define CAMERA_GAP_5E (1 << 3)
#define CAMERA_GAP_6E (1 << 4)
#define CAMERA_GAP_7E (1 << 5)
#define CAMERA_GAP_8E (1 << 6)
#define CAMERA_GAP_9E (1 << 7)

// awb_calc_height_reg
#define CAMERA_AWB_CALC_HEIGHT_REG(n) (((n)&0xff) << 0)

// drc_r_clp_value_reg
#define CAMERA_DRC_R_CLP_VALUE_REG(n) (((n)&0x3f) << 0)

// drc_gr_clp_value_reg
#define CAMERA_DRC_GR_CLP_VALUE_REG(n) (((n)&0x3f) << 0)

// drc_gb_clp_value_reg
#define CAMERA_DRC_GB_CLP_VALUE_REG(n) (((n)&0x3f) << 0)

// drc_b_clp_value_reg
#define CAMERA_DRC_B_CLP_VALUE_REG(n) (((n)&0x3f) << 0)

// sepia_cr_reg
#define CAMERA_SEPIA_CR_REG(n) (((n)&0xff) << 0)

// sepia_cb_reg
#define CAMERA_SEPIA_CB_REG(n) (((n)&0xff) << 0)

// csup_y_min_hi_reg
#define CAMERA_CSUP_Y_MIN_HI_REG(n) (((n)&0xff) << 0)

// csup_gain_hi_reg
#define CAMERA_CSUP_GAIN_HI_REG(n) (((n)&0xff) << 0)

// csup_y_max_low_reg
#define CAMERA_CSUP_Y_MAX_LOW_REG(n) (((n)&0xff) << 0)

// csup_gain_low_reg
#define CAMERA_CSUP_GAIN_LOW_REG(n) (((n)&0xff) << 0)

// ae_dk_hist_thr_reg
#define CAMERA_AE_DK_HIST_THR_REG(n) (((n)&0xff) << 0)

// ae_br_hist_thr_reg
#define CAMERA_AE_BR_HIST_THR_REG(n) (((n)&0xff) << 0)

// hist_bp_level_reg
#define CAMERA_HIST_BP_LEVEL_REG(n) (((n)&0xff) << 0)

// outdoor_th_reg
#define CAMERA_OUTDOOR_TH(n) (((n)&0xf) << 0)
#define CAMERA_NON_OUTDOOR_TH(n) (((n)&0xf) << 4)

// awb_rgain_low_reg
#define CAMERA_AWB_RGAIN_LOW_REG(n) (((n)&0x3f) << 2)

// awb_rgain_high_reg
#define CAMERA_AWB_RGAIN_HIGH_REG(n) (((n)&0x3f) << 2)

// awb_bgain_low_reg
#define CAMERA_AWB_BGAIN_LOW_REG(n) (((n)&0x3f) << 2)

// awb_bgain_high_reg
#define CAMERA_AWB_BGAIN_HIGH_REG(n) (((n)&0x3f) << 2)

// awb_calc_start_reg
#define CAMERA_AWB_WIN_Y_START(n) (((n)&0xf) << 0)
#define CAMERA_AWB_WIN_X_START(n) (((n)&0xf) << 4)

// awb_calc_width_reg
#define CAMERA_AWB_CALC_WIDTH_REG(n) (((n)&0xff) << 0)

// hist_dp_level_reg
#define CAMERA_HIST_DP_LEVEL_REG(n) (((n)&0xff) << 0)

// awb_y_fmin
#define CAMERA_AWB_Y_FMIN(n) (((n)&0xff) << 0)

// awb_y_fmax
#define CAMERA_AWB_Y_FMAX(n) (((n)&0xff) << 0)

// awb_cb_fmin
#define CAMERA_AWB_CB_FMIN(n) (((n)&0xff) << 0)

// awb_cb_fmax
#define CAMERA_AWB_CB_FMAX(n) (((n)&0xff) << 0)

// awb_cr_fmin
#define CAMERA_AWB_CR_FMIN(n) (((n)&0xff) << 0)

// awb_cr_fmax
#define CAMERA_AWB_CR_FMAX(n) (((n)&0xff) << 0)

// awb_y_fmin2
#define CAMERA_AWB_Y_FMIN2(n) (((n)&0xff) << 0)

// awb_y_fmax2
#define CAMERA_AWB_Y_FMAX2(n) (((n)&0xff) << 0)

// awb_cb_fmin2
#define CAMERA_AWB_CB_FMIN2(n) (((n)&0xff) << 0)

// awb_cb_fmax2
#define CAMERA_AWB_CB_FMAX2(n) (((n)&0xff) << 0)

// awb_cr_fmin2
#define CAMERA_AWB_CR_FMIN2(n) (((n)&0xff) << 0)

// awb_cr_fmax2
#define CAMERA_AWB_CR_FMAX2(n) (((n)&0xff) << 0)

// ae_use_mean
#define CAMERA_YCAVE_USE_MEAN(n) (((n)&0x3) << 0)
#define CAMERA_YWAVE_USE_MEAN(n) (((n)&0x3) << 2)
#define CAMERA_YAVE_WEIGHT_MODE (1 << 4)
#define CAMERA_NEXP_OUT_SEL_REG (1 << 5)
#define CAMERA_AE_EXT_ADJ_VAL_REG (1 << 6)
#define CAMERA_AE_EXT_ADJ_ON_REG (1 << 7)

// ae_weight_sta
#define CAMERA_YWAVE_PCNT_LEFT(n) (((n)&0xf) << 0)
#define CAMERA_YWAVE_LCNT_TOP(n) (((n)&0xf) << 4)

// ae_qwidth
#define CAMERA_QWIDTH(n) (((n)&0xff) << 0)

// ae_qheight
#define CAMERA_QHEIGHT(n) (((n)&0x7f) << 0)
#define CAMERA_YWAVE_SEL (1 << 7)

// ae_win_sta
#define CAMERA_YAVE_PCNT_STA(n) (((n)&0xf) << 0)
#define CAMERA_YAVE_LCNT_STA(n) (((n)&0xf) << 4)

// ae_width
#define CAMERA_WIDTH(n) (((n)&0xff) << 0)

// ae_height
#define CAMERA_HEIGHT(n) (((n)&0xff) << 0)

// sw_update
#define CAMERA_CC_TYPE_SW (1 << 0)
#define CAMERA_IS_OUTDOOR_SW (1 << 1)
#define CAMERA_GAMMA_TYPE_SW (1 << 2)
#define CAMERA_SW_UPDATE_RSVD (1 << 3)
#define CAMERA_IS_OUTDOOR_MODE(n) (((n)&0x7) << 4)
#define CAMERA_AWB_OUTDOOR_EN (1 << 7)

// awb_ctrl5
#define CAMERA_R_LOW_NON_A(n) (((n)&0xff) << 0)

// awb_ctrl6
#define CAMERA_AWB_STOP_H(n) (((n)&0xf) << 0)
#define CAMERA_AWB_ADJ_AGAIN(n) (((n)&0x3) << 4)
#define CAMERA_AWB_ALGO_EN (1 << 6)
#define CAMERA_CHECK_R_LOW (1 << 7)

// sca_reg
#define CAMERA_SCA_MODE(n) (((n)&0x7) << 0)

// ae_ee_sta_reg
#define CAMERA_AE_EE_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_ee_num_reg
#define CAMERA_AE_EE_NUM_REG(n) (((n)&0xf) << 0)

// ae_ef_sta_reg
#define CAMERA_AE_EF_STA_GAIN(n) (((n)&0x3f) << 0)

// ae_ef_num_reg
#define CAMERA_AE_EF_NUM_REG(n) (((n)&0xf) << 0)

// ae_thr_big_reg
#define CAMERA_AE_THR_BIG_DARK(n) (((n)&0xf) << 0)
#define CAMERA_AE_THR_BIG_BRIGHT(n) (((n)&0xf) << 4)

// sharp_gain_minus_low
#define CAMERA_SHARP_GAIN_MINUS_LOW(n) (((n)&0xff) << 0)

// sharp_gain_minus_mid
#define CAMERA_SHARP_GAIN_MINUS_MID(n) (((n)&0xff) << 0)

// sharp_gain_minus_hi
#define CAMERA_SHARP_GAIN_MINUS_HI(n) (((n)&0xff) << 0)

// sharp_mode_mid_hi
#define CAMERA_SHARP_CMP_GAP_MID(n) (((n)&0xf) << 0)
#define CAMERA_SHARP_CMP_GAP_HI(n) (((n)&0xf) << 4)

// fw_version_reg
#define CAMERA_FW_VERSION(n) (((n)&0xff) << 0)

// awb_y_min_reg
#define CAMERA_AWB_Y_MIN(n) (((n)&0xff) << 0)

// y_red_coef_reg
#define CAMERA_Y_RED_COEF(n) (((n)&0xff) << 0)

// y_blue_coef_reg
#define CAMERA_Y_BLUE_COEF(n) (((n)&0xff) << 0)

// cb_red_coef_reg
#define CAMERA_CB_RED_COEF(n) (((n)&0xff) << 0)

// cr_blue_coef_reg
#define CAMERA_CR_BLUE_COEF(n) (((n)&0xff) << 0)

// hist_vbp_level_reg
#define CAMERA_HIST_VBP_LEVEL(n) (((n)&0xff) << 0)

// hist_vdp_level_reg
#define CAMERA_HIST_VDP_LEVEL(n) (((n)&0xff) << 0)

#endif // _CAMERA_H_
