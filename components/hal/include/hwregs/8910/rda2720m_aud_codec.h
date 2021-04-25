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

#ifndef _RDA2720M_AUD_CODEC_H_
#define _RDA2720M_AUD_CODEC_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_AUD_CODEC_BASE (0x50308700)

typedef volatile struct
{
    uint32_t ana_pmu0; // 0x00000000
    uint32_t ana_pmu1; // 0x00000004
    uint32_t ana_pmu2; // 0x00000008
    uint32_t ana_pmu3; // 0x0000000c
    uint32_t ana_pmu4; // 0x00000010
    uint32_t ana_pmu5; // 0x00000014
    uint32_t ana_clk0; // 0x00000018
    uint32_t ana_cdc0; // 0x0000001c
    uint32_t ana_cdc1; // 0x00000020
    uint32_t ana_cdc2; // 0x00000024
    uint32_t ana_cdc3; // 0x00000028
    uint32_t ana_cdc4; // 0x0000002c
    uint32_t ana_hdt0; // 0x00000030
    uint32_t ana_hdt1; // 0x00000034
    uint32_t ana_hdt2; // 0x00000038
    uint32_t ana_dcl0; // 0x0000003c
    uint32_t ana_dcl1; // 0x00000040
    uint32_t ana_dcl2; // 0x00000044
    uint32_t __72[1];  // 0x00000048
    uint32_t ana_dcl4; // 0x0000004c
    uint32_t ana_dcl5; // 0x00000050
    uint32_t ana_dcl6; // 0x00000054
    uint32_t ana_dcl7; // 0x00000058
    uint32_t ana_sts0; // 0x0000005c
    uint32_t __96[1];  // 0x00000060
    uint32_t ana_sts2; // 0x00000064
    uint32_t ana_sts3; // 0x00000068
    uint32_t ana_sts4; // 0x0000006c
    uint32_t ana_sts5; // 0x00000070
    uint32_t ana_sts6; // 0x00000074
    uint32_t ana_sts7; // 0x00000078
    uint32_t ana_clk1; // 0x0000007c
} HWP_RDA2720M_AUD_CODEC_T;

#define hwp_rda2720mAudCodec ((HWP_RDA2720M_AUD_CODEC_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_AUD_CODEC_BASE))

// ana_pmu0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_hmic_comp_mode1_en : 1;    // [0]
        uint32_t rg_aud_hmicbias_vref_sel : 1; // [1]
        uint32_t rg_aud_micbias_plgb : 1;      // [2]
        uint32_t rg_aud_vbg_temp_tune : 2;     // [4:3]
        uint32_t rg_aud_vbg_temp_biastune : 1; // [5]
        uint32_t rg_aud_vbg_sel : 1;           // [6]
        uint32_t rg_aud_mic_sleep_en : 1;      // [7]
        uint32_t rg_aud_hmic_sleep_en : 1;     // [8]
        uint32_t rg_aud_hmic_bias_en : 1;      // [9]
        uint32_t rg_aud_micbias_en : 1;        // [10]
        uint32_t rg_aud_bias_en : 1;           // [11]
        uint32_t rg_aud_bg_en : 1;             // [12]
        uint32_t rg_aud_vb_hdmc_sp_pd : 1;     // [13]
        uint32_t rg_aud_vb_nleak_pd : 1;       // [14]
        uint32_t rg_aud_vb_en : 1;             // [15]
        uint32_t __31_16 : 16;                 // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_PMU0_T;

// ana_pmu1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_micbias_v : 3;   // [2:0]
        uint32_t rg_aud_hmic_bias_v : 3; // [5:3]
        uint32_t rg_aud_vb_v : 5;        // [10:6]
        uint32_t rg_aud_vb_cal : 5;      // [15:11]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_PMU1_T;

// ana_pmu2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;                  // [0]
        uint32_t rg_aud_drv_pm_sel : 2;      // [2:1]
        uint32_t rg_aud_da_ig : 2;           // [4:3]
        uint32_t rg_aud_adpga_ibias_sel : 4; // [8:5]
        uint32_t rg_aud_pa_ab_i : 2;         // [10:9]
        uint32_t rg_aud_hp_ibcur3 : 3;       // [13:11]
        uint32_t rg_aud_hp_ib : 2;           // [15:14]
        uint32_t __31_16 : 16;               // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_PMU2_T;

// ana_pmu3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_pa_vcm_v : 2;     // [1:0]
        uint32_t rg_aud_drv_ocp_mode : 2; // [3:2]
        uint32_t rg_aud_drv_ocp_pd : 1;   // [4]
        uint32_t rg_aud_pa_ocp_s : 1;     // [5]
        uint32_t rg_aud_pa_ocp_pd : 1;    // [6]
        uint32_t rg_aud_pa_ovp_v : 3;     // [9:7]
        uint32_t rg_aud_pa_ovp_thd : 1;   // [10]
        uint32_t rg_aud_pa_ovp_pd : 1;    // [11]
        uint32_t rg_aud_pa_otp_t : 3;     // [14:12]
        uint32_t rg_aud_pa_otp_pd : 1;    // [15]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_PMU3_T;

// ana_pmu4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_pa_ss_t : 3;      // [2:0]
        uint32_t rg_aud_pa_ss_32k_en : 1; // [3]
        uint32_t rg_aud_pa_ss_f : 2;      // [5:4]
        uint32_t rg_aud_pa_ss_rst : 1;    // [6]
        uint32_t rg_aud_pa_ss_en : 1;     // [7]
        uint32_t rg_aud_pa_emi_l : 3;     // [10:8]
        uint32_t rg_aud_pa_degsel : 2;    // [12:11]
        uint32_t rg_aud_pa_ksel : 2;      // [14:13]
        uint32_t __31_15 : 17;            // [31:15]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_PMU4_T;

// ana_pmu5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_pa_sl_det_en : 1; // [0]
        uint32_t rg_aud_pa_sh_det_en : 1; // [1]
        uint32_t rg_aud_pa_stop_en : 1;   // [2]
        uint32_t rg_aud_pa_dtri_ff : 6;   // [8:3]
        uint32_t rg_aud_pa_dtri_fc : 3;   // [11:9]
        uint32_t rg_aud_pa_dflck_rsl : 1; // [12]
        uint32_t rg_aud_pa_dflck_en : 1;  // [13]
        uint32_t rg_aud_pa_d_en : 1;      // [14]
        uint32_t __31_15 : 17;            // [31:15]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_PMU5_T;

// ana_clk0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_da_clk_f : 2;        // [1:0]
        uint32_t rg_aud_ad_clk_f : 2;        // [3:2]
        uint32_t rg_aud_vad_en : 1;          // [4]
        uint32_t rg_aud_dcdccore_clk_en : 1; // [5]
        uint32_t rg_aud_dcdcmem_clk_en : 1;  // [6]
        uint32_t rg_aud_dcdcgen_clk_en : 1;  // [7]
        uint32_t rg_drv_clk_en : 1;          // [8]
        uint32_t rg_aud_da_clk_en : 1;       // [9]
        uint32_t rg_aud_ad_clk_rst : 1;      // [10]
        uint32_t rg_aud_ad_clk_en : 1;       // [11]
        uint32_t rg_aud_ana_clk_en : 1;      // [12]
        uint32_t rg_aud_dig_clk_loop_en : 1; // [13]
        uint32_t rg_aud_dig_clk_6p5m_en : 1; // [14]
        uint32_t __31_15 : 17;               // [31:15]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_CLK0_T;

// ana_cdc0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_adc_bulksw : 1;       // [0]
        uint32_t rg_aud_shmic_dpopvcm_en : 1; // [1]
        uint32_t rg_aud_shmic_dpop : 1;       // [2]
        uint32_t rg_aud_vref_sfcur : 1;       // [3]
        uint32_t rg_aud_adr_rst : 1;          // [4]
        uint32_t rg_aud_adr_en : 1;           // [5]
        uint32_t rg_aud_adl_rst : 1;          // [6]
        uint32_t rg_aud_adl_en : 1;           // [7]
        uint32_t rg_aud_adpgar_byp : 2;       // [9:8]
        uint32_t rg_aud_adpgal_byp : 2;       // [11:10]
        uint32_t rg_aud_adpgar_en : 1;        // [12]
        uint32_t rg_aud_adpgal_en : 1;        // [13]
        uint32_t rg_aud_adpga_ibuf_en : 1;    // [14]
        uint32_t rg_aud_adpga_ibias_en : 1;   // [15]
        uint32_t __31_16 : 16;                // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_CDC0_T;

// ana_cdc1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_das_os_d : 3;       // [2:0]
        uint32_t rg_aud_dalr_os_d : 3;      // [5:3]
        uint32_t rg_aud_adpgar_g : 3;       // [8:6]
        uint32_t rg_aud_adpgal_g : 3;       // [11:9]
        uint32_t rg_aud_advcmi_int_sel : 2; // [13:12]
        uint32_t __31_14 : 18;              // [31:14]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_CDC1_T;

// ana_cdc2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_pa_en : 1;    // [0]
        uint32_t rg_aud_rcv_en : 1;   // [1]
        uint32_t rg_aud_hpbuf_en : 1; // [2]
        uint32_t rg_aud_hpr_en : 1;   // [3]
        uint32_t rg_aud_hpl_en : 1;   // [4]
        uint32_t rcv_floop_end : 1;   // [5]
        uint32_t rcv_floopen : 1;     // [6]
        uint32_t hpr_floop_end : 1;   // [7]
        uint32_t hpr_floopen : 1;     // [8]
        uint32_t hpl_floop_end : 1;   // [9]
        uint32_t hpl_floopen : 1;     // [10]
        uint32_t rg_aud_dar_en : 1;   // [11]
        uint32_t rg_aud_dal_en : 1;   // [12]
        uint32_t rg_aud_das_en : 1;   // [13]
        uint32_t __31_14 : 18;        // [31:14]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_CDC2_T;

// ana_cdc3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_shmicpgar : 1;     // [0]
        uint32_t rg_aud_shmicpgal : 1;     // [1]
        uint32_t rg_aud_smic2pgar : 1;     // [2]
        uint32_t rg_aud_smic1pgal : 1;     // [3]
        uint32_t rg_aud_smicdrv_debug : 1; // [4]
        uint32_t rg_aud_shmicpa_debug : 1; // [5]
        uint32_t rg_aud_sdapa : 1;         // [6]
        uint32_t rg_aud_sdalrcv : 1;       // [7]
        uint32_t rg_aud_sdarhpr : 1;       // [8]
        uint32_t rg_aud_sdalhpl : 1;       // [9]
        uint32_t rg_aud_pa_ng_en : 1;      // [10]
        uint32_t rg_aud_das_os_en : 1;     // [11]
        uint32_t rg_aud_dalr_os_en : 1;    // [12]
        uint32_t __31_13 : 19;             // [31:13]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_CDC3_T;

// ana_cdc4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_hpr_g : 4; // [3:0]
        uint32_t rg_aud_hpl_g : 4; // [7:4]
        uint32_t rg_aud_rcv_g : 4; // [11:8]
        uint32_t rg_aud_pa_g : 4;  // [15:12]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_CDC4_T;

// ana_hdt0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_hedet_v2i_sel : 4;        // [3:0]
        uint32_t rg_aud_hedet_mux2adc_sel : 3;    // [6:4]
        uint32_t rg_aud_hedet_buf_chop : 1;       // [7]
        uint32_t rg_aud_hedet_ldet_l_filter : 1;  // [8]
        uint32_t rg_aud_hedet_v2ad_scale : 1;     // [9]
        uint32_t rg_aud_hedet_micdet_en : 1;      // [10]
        uint32_t rg_aud_hedet_vref_en : 1;        // [11]
        uint32_t rg_aud_hedet_v2i_en : 1;         // [12]
        uint32_t rg_aud_hedet_bdet_en : 1;        // [13]
        uint32_t rg_aud_hedet_buf_en : 1;         // [14]
        uint32_t rg_aud_hedet_mux2adc_sel_pd : 1; // [15]
        uint32_t __31_16 : 16;                    // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_HDT0_T;

// ana_hdt1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_hedet_ldet_h_hys_sel : 2; // [1:0]
        uint32_t rg_aud_hedet_ldet_l_hys_sel : 2; // [3:2]
        uint32_t rg_aud_hedet_ldet_pu_pd : 2;     // [5:4]
        uint32_t rg_aud_hedet_ldet_refh_sel : 2;  // [7:6]
        uint32_t rg_aud_hedet_ldet_refl_sel : 3;  // [10:8]
        uint32_t rg_aud_hedet_micdet_hys_sel : 2; // [12:11]
        uint32_t rg_aud_hedet_micdet_ref_sel : 3; // [15:13]
        uint32_t __31_16 : 16;                    // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_HDT1_T;

// ana_hdt2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_hpl_en_d2hdt_t : 2;     // [1:0]
        uint32_t rg_aud_hpl_en_d2hdt_en : 1;    // [2]
        uint32_t rg_hp_driver_en : 1;           // [3]
        uint32_t rg_aud_plgpd_en : 1;           // [4]
        uint32_t rg_aud_hedet_bdet_hys_sel : 2; // [6:5]
        uint32_t rg_aud_hedet_bdet_ref_sel : 4; // [10:7]
        uint32_t rg_aud_hedet_jack_type : 2;    // [12:11]
        uint32_t rg_chg_proc_sts_bypass : 1;    // [13]
        uint32_t __31_14 : 18;                  // [31:14]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_HDT2_T;

// ana_dcl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_dpop_auto_rst : 1; // [0]
        uint32_t rg_aud_drv_soft_en : 1;   // [1]
        uint32_t rg_aud_drv_soft_t : 3;    // [4:2]
        uint32_t rg_aud_dcl_rst : 1;       // [5]
        uint32_t rg_aud_dcl_en : 1;        // [6]
        uint32_t __31_7 : 25;              // [31:7]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_DCL0_T;

// ana_dcl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_pa_ovp_deg_t : 3;    // [2:0]
        uint32_t rg_aud_pa_ovp_deg_en : 1;   // [3]
        uint32_t rg_aud_pa_ovp_abmod_t : 3;  // [6:4]
        uint32_t rg_aud_pa_ovp_abmod_pd : 1; // [7]
        uint32_t rg_aud_pacal_div : 2;       // [9:8]
        uint32_t rg_aud_pacal_en : 1;        // [10]
        uint32_t __31_11 : 21;               // [31:11]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_DCL1_T;

// ana_dcl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_pa_ocp_mute_t : 3;  // [2:0]
        uint32_t rg_aud_pa_ocp_mute_en : 1; // [3]
        uint32_t rg_aud_pa_ocp_deg_t : 3;   // [6:4]
        uint32_t rg_aud_pa_ocp_deg_en : 1;  // [7]
        uint32_t rg_aud_pa_otp_mute_en : 1; // [8]
        uint32_t rg_aud_pa_otp_deg_t : 3;   // [11:9]
        uint32_t rg_aud_pa_otp_deg_en : 1;  // [12]
        uint32_t __31_13 : 19;              // [31:13]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_DCL2_T;

// ana_dcl4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_hpr_depop_chg_cursel : 8; // [7:0]
        uint32_t rg_hpl_depop_chg_cursel : 8; // [15:8]
        uint32_t __31_16 : 16;                // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_DCL4_T;

// ana_dcl5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;                   // [0]
        uint32_t rg_aud_hpr_rdac_sts : 1;     // [1], read only
        uint32_t rg_aud_hpl_rdac_sts : 1;     // [2], read only
        uint32_t rg_aud_hp_dpop_gain_t : 3;   // [5:3]
        uint32_t rg_aud_hp_dpop_gain_n2 : 3;  // [8:6]
        uint32_t rg_aud_hp_dpop_gain_n1 : 3;  // [11:9]
        uint32_t rg_aud_hp_dpop_fdout_en : 1; // [12]
        uint32_t rg_aud_hp_dpop_fdin_en : 1;  // [13]
        uint32_t rg_aud_hpr_rdac_start : 1;   // [14]
        uint32_t rg_aud_hpl_rdac_start : 1;   // [15]
        uint32_t __31_16 : 16;                // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_DCL5_T;

// ana_dcl6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;                 // [0]
        uint32_t rg_aud_hpl_dpop_n2 : 2;    // [2:1]
        uint32_t rg_aud_hpl_dpop_clkn2 : 2; // [4:3]
        uint32_t rg_aud_hpl_dpop_val1 : 3;  // [7:5]
        uint32_t rg_aud_hpl_dpop_n1 : 2;    // [9:8]
        uint32_t rg_aud_hpl_dpop_clkn1 : 2; // [11:10]
        uint32_t rg_caldc_wait_t : 3;       // [14:12]
        uint32_t __31_15 : 17;              // [31:15]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_DCL6_T;

// ana_dcl7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_hpr_dpop_n2 : 2;    // [1:0]
        uint32_t rg_aud_hpr_dpop_clkn2 : 2; // [3:2]
        uint32_t rg_aud_hpr_dpop_val1 : 3;  // [6:4]
        uint32_t rg_aud_hpr_dpop_n1 : 2;    // [8:7]
        uint32_t rg_aud_hpr_dpop_clkn1 : 2; // [10:9]
        uint32_t rg_depopr_pcur_opt : 2;    // [12:11]
        uint32_t rg_depopl_pcur_opt : 2;    // [14:13]
        uint32_t __31_15 : 17;              // [31:15]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_DCL7_T;

// ana_sts0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_dccali_rdaci_adj : 1; // [0]
        uint32_t dc_cali_idac_cursel : 2; // [2:1]
        uint32_t rg_dc_cali_idacval : 3;  // [5:3]
        uint32_t rg_hp_depop_wait_t4 : 2; // [7:6]
        uint32_t rg_hp_depop_wait_t3 : 2; // [9:8]
        uint32_t rg_hp_depop_wait_t2 : 3; // [12:10]
        uint32_t rg_hp_depop_wait_t1 : 3; // [15:13]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_STS0_T;

// ana_sts2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;                // [0]
        uint32_t rg_insbuf_en : 1;         // [1]
        uint32_t rg_hpr_pu_enb : 1;        // [2]
        uint32_t rg_hpl_pu_enb : 1;        // [3]
        uint32_t rg_aud_rcv_dpop_dvld : 1; // [4], read only
        uint32_t rg_depop_chg_sts : 1;     // [5], read only
        uint32_t rg_depop_en : 1;          // [6]
        uint32_t rg_aud_plugin : 1;        // [7]
        uint32_t rg_depop_chg_en : 1;      // [8]
        uint32_t rg_depop_chg_start : 1;   // [9]
        uint32_t rg_aud_hp_dpop_dvld : 1;  // [10], read only
        uint32_t rg_dccali_sts_bypass : 1; // [11]
        uint32_t rg_aud_dccal_sts : 1;     // [12], read only
        uint32_t rg_caldc_eno : 1;         // [13]
        uint32_t rg_caldc_en : 1;          // [14]
        uint32_t rg_caldc_start : 1;       // [15]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_STS2_T;

// ana_sts3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_hwsw_sel : 6;     // [5:0]
        uint32_t rg_depop_opa_sel : 2;    // [7:6]
        uint32_t rg_depop_bias_sel : 2;   // [9:8]
        uint32_t rg_aud_clk3_reserve : 2; // [11:10]
        uint32_t __31_12 : 20;            // [31:12]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_STS3_T;

// ana_sts4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hpr_dccal_idacl_sel : 2; // [1:0], read only
        uint32_t hpl_dccal_idacl_sel : 2; // [3:2], read only
        uint32_t hpr_dccal_rdacl : 4;     // [7:4], read only
        uint32_t hpl_dccal_rdacl : 8;     // [15:8], read only
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_STS4_T;

// ana_sts5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;           // [1:0]
        uint32_t rg_hpr_comp_ins : 2; // [3:2], read only
        uint32_t rg_hpl_comp_ins : 2; // [5:4], read only
        uint32_t hpr_dccal_idacl : 5; // [10:6], read only
        uint32_t hpl_dccal_idacl : 5; // [15:11], read only
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_STS5_T;

// ana_sts6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;             // [1:0]
        uint32_t rg_aud_pacal_dvld : 1; // [2], read only
        uint32_t rg_aud_pacal_do : 13;  // [15:3], read only
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_STS6_T;

// ana_sts7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_drv_ocp_flag : 4;    // [3:0], read only
        uint32_t rg_aud_pa_otp_flag : 1;     // [4], read only
        uint32_t rg_aud_pa_ovp_flag : 1;     // [5], read only
        uint32_t rg_aud_pa_sl_flag : 1;      // [6], read only
        uint32_t rg_aud_pa_sh_flag : 1;      // [7], read only
        uint32_t rg_aud_head_button : 1;     // [8], read only
        uint32_t rg_aud_head_insert : 1;     // [9], read only
        uint32_t rg_aud_head_insert2 : 1;    // [10], read only
        uint32_t rg_aud_head_insert3 : 1;    // [11], read only
        uint32_t rg_aud_head_insert_all : 1; // [12], read only
        uint32_t __31_13 : 19;               // [31:13]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_STS7_T;

// ana_clk1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_aud_clk_pn_sel : 8;     // [7:0]
        uint32_t rg_aud_pa_clk_f : 2;       // [9:8]
        uint32_t rg_aud_dcdcchg_clk_f : 2;  // [11:10]
        uint32_t rg_aud_dcdccore_clk_f : 2; // [13:12]
        uint32_t rg_aud_dcdcgen_clk_f : 2;  // [15:14]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_AUD_CODEC_ANA_CLK1_T;

// ana_pmu0
#define RDA2720M_AUD_CODEC_RG_HMIC_COMP_MODE1_EN (1 << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_HMICBIAS_VREF_SEL (1 << 1)
#define RDA2720M_AUD_CODEC_RG_AUD_MICBIAS_PLGB (1 << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_VBG_TEMP_TUNE(n) (((n)&0x3) << 3)
#define VBG_TEMP_TUNE_NORMAL         (0x0)
#define VBG_TEMP_TUNE_TC_REDUCE      (0x1)
#define VBG_TEMP_TUNE_TC_REDUCE_MORE (0x2)
#define VBG_TEMP_TUNE_TC_ENHANCE     (0x3)
#define RDA2720M_AUD_CODEC_RG_AUD_VBG_TEMP_BIASTUNE (1 << 5)
#define RDA2720M_AUD_CODEC_RG_AUD_VBG_SEL (1 << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_MIC_SLEEP_EN (1 << 7)
#define RDA2720M_AUD_CODEC_RG_AUD_HMIC_SLEEP_EN (1 << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_HMIC_BIAS_EN (1 << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_MICBIAS_EN (1 << 10)
#define RDA2720M_AUD_CODEC_RG_AUD_BIAS_EN (1 << 11)
#define RDA2720M_AUD_CODEC_RG_AUD_BG_EN (1 << 12)
#define RDA2720M_AUD_CODEC_RG_AUD_VB_HDMC_SP_PD (1 << 13)
#define RDA2720M_AUD_CODEC_RG_AUD_VB_NLEAK_PD (1 << 14)
#define RDA2720M_AUD_CODEC_RG_AUD_VB_EN (1 << 15)

// ana_pmu1
#define RDA2720M_AUD_CODEC_RG_AUD_MICBIAS_V(n) (((n)&0x7) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_HMIC_BIAS_V(n) (((n)&0x7) << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_VB_V(n) (((n)&0x1f) << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_VB_CAL(n) (((n)&0x1f) << 11)

// ana_pmu2
#define RDA2720M_AUD_CODEC_RG_AUD_DRV_PM_SEL(n) (((n)&0x3) << 1)
#define RDA2720M_AUD_CODEC_RG_AUD_DA_IG(n) (((n)&0x3) << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_ADPGA_IBIAS_SEL(n) (((n)&0xf) << 5)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_AB_I(n) (((n)&0x3) << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_HP_IBCUR3(n) (((n)&0x7) << 11)
#define RDA2720M_AUD_CODEC_RG_AUD_HP_IB(n) (((n)&0x3) << 14)

// ana_pmu3
#define RDA2720M_AUD_CODEC_RG_AUD_PA_VCM_V(n) (((n)&0x3) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_DRV_OCP_MODE(n) (((n)&0x3) << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_DRV_OCP_PD (1 << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OCP_S (1 << 5)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OCP_PD (1 << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OVP_V(n) (((n)&0x7) << 7)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OVP_THD (1 << 10)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OVP_PD (1 << 11)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OTP_T(n) (((n)&0x7) << 12)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OTP_PD (1 << 15)

// ana_pmu4
#define RDA2720M_AUD_CODEC_RG_AUD_PA_SS_T(n) (((n)&0x7) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_SS_32K_EN (1 << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_SS_F(n) (((n)&0x3) << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_SS_RST (1 << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_SS_EN (1 << 7)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_EMI_L(n) (((n)&0x7) << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_DEGSEL(n) (((n)&0x3) << 11)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_KSEL(n) (((n)&0x3) << 13)

// ana_pmu5
#define RDA2720M_AUD_CODEC_RG_AUD_PA_SL_DET_EN (1 << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_SH_DET_EN (1 << 1)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_STOP_EN (1 << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_DTRI_FF(n) (((n)&0x3f) << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_DTRI_FC(n) (((n)&0x7) << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_DFLCK_RSL (1 << 12)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_DFLCK_EN (1 << 13)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_D_EN (1 << 14)

// ana_clk0
#define RDA2720M_AUD_CODEC_RG_AUD_DA_CLK_F(n) (((n)&0x3) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_AD_CLK_F(n) (((n)&0x3) << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_VAD_EN (1 << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_DCDCCORE_CLK_EN (1 << 5)
#define RDA2720M_AUD_CODEC_RG_AUD_DCDCMEM_CLK_EN (1 << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_DCDCGEN_CLK_EN (1 << 7)
#define RDA2720M_AUD_CODEC_RG_DRV_CLK_EN (1 << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_DA_CLK_EN (1 << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_AD_CLK_RST (1 << 10)
#define RDA2720M_AUD_CODEC_RG_AUD_AD_CLK_EN (1 << 11)
#define RDA2720M_AUD_CODEC_RG_AUD_ANA_CLK_EN (1 << 12)
#define RDA2720M_AUD_CODEC_RG_AUD_DIG_CLK_LOOP_EN (1 << 13)
#define RDA2720M_AUD_CODEC_RG_AUD_DIG_CLK_6P5M_EN (1 << 14)

// ana_cdc0
#define RDA2720M_AUD_CODEC_RG_AUD_ADC_BULKSW (1 << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_SHMIC_DPOPVCM_EN (1 << 1)
#define RDA2720M_AUD_CODEC_RG_AUD_SHMIC_DPOP (1 << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_VREF_SFCUR (1 << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_ADR_RST (1 << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_ADR_EN (1 << 5)
#define RDA2720M_AUD_CODEC_RG_AUD_ADL_RST (1 << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_ADL_EN (1 << 7)
#define RDA2720M_AUD_CODEC_RG_AUD_ADPGAR_BYP(n) (((n)&0x3) << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_ADPGAL_BYP(n) (((n)&0x3) << 10)
#define RDA2720M_AUD_CODEC_RG_AUD_ADPGAR_EN (1 << 12)
#define RDA2720M_AUD_CODEC_RG_AUD_ADPGAL_EN (1 << 13)
#define RDA2720M_AUD_CODEC_RG_AUD_ADPGA_IBUF_EN (1 << 14)
#define RDA2720M_AUD_CODEC_RG_AUD_ADPGA_IBIAS_EN (1 << 15)

// ana_cdc1
#define RDA2720M_AUD_CODEC_RG_AUD_DAS_OS_D(n) (((n)&0x7) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_DALR_OS_D(n) (((n)&0x7) << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_ADPGAR_G(n) (((n)&0x7) << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_ADPGAL_G(n) (((n)&0x7) << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_ADVCMI_INT_SEL(n) (((n)&0x3) << 12)

// ana_cdc2
#define RDA2720M_AUD_CODEC_RG_AUD_PA_EN (1 << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_RCV_EN (1 << 1)
#define RDA2720M_AUD_CODEC_RG_AUD_HPBUF_EN (1 << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_HPR_EN (1 << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_EN (1 << 4)
#define RDA2720M_AUD_CODEC_RCV_FLOOP_END (1 << 5)
#define RDA2720M_AUD_CODEC_RCV_FLOOPEN (1 << 6)
#define RDA2720M_AUD_CODEC_HPR_FLOOP_END (1 << 7)
#define RDA2720M_AUD_CODEC_HPR_FLOOPEN (1 << 8)
#define RDA2720M_AUD_CODEC_HPL_FLOOP_END (1 << 9)
#define RDA2720M_AUD_CODEC_HPL_FLOOPEN (1 << 10)
#define RDA2720M_AUD_CODEC_RG_AUD_DAR_EN (1 << 11)
#define RDA2720M_AUD_CODEC_RG_AUD_DAL_EN (1 << 12)
#define RDA2720M_AUD_CODEC_RG_AUD_DAS_EN (1 << 13)

// ana_cdc3
#define RDA2720M_AUD_CODEC_RG_AUD_SHMICPGAR (1 << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_SHMICPGAL (1 << 1)
#define RDA2720M_AUD_CODEC_RG_AUD_SMIC2PGAR (1 << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_SMIC1PGAL (1 << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_SMICDRV_DEBUG (1 << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_SHMICPA_DEBUG (1 << 5)
#define RDA2720M_AUD_CODEC_RG_AUD_SDAPA (1 << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_SDALRCV (1 << 7)
#define RDA2720M_AUD_CODEC_RG_AUD_SDARHPR (1 << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_SDALHPL (1 << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_NG_EN (1 << 10)
#define RDA2720M_AUD_CODEC_RG_AUD_DAS_OS_EN (1 << 11)
#define RDA2720M_AUD_CODEC_RG_AUD_DALR_OS_EN (1 << 12)

// ana_cdc4
#define RDA2720M_AUD_CODEC_RG_AUD_HPR_G(n) (((n)&0xf) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_G(n) (((n)&0xf) << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_RCV_G(n) (((n)&0xf) << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_G(n) (((n)&0xf) << 12)

// ana_hdt0
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_V2I_SEL(n) (((n)&0xf) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_MUX2ADC_SEL(n) (((n)&0x7) << 4)
#define HEDET_MUX2ADC_SEL_HEADMIC_IN_DETECT (0)
#define HEDET_MUX2ADC_SEL_HEADSET_L_INT     (1)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_BUF_CHOP (1 << 7)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_LDET_L_FILTER (1 << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_V2AD_SCALE (1 << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_MICDET_EN (1 << 10)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_VREF_EN (1 << 11)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_V2I_EN (1 << 12)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_BDET_EN (1 << 13)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_BUF_EN (1 << 14)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_MUX2ADC_SEL_PD (1 << 15)

// ana_hdt1
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_LDET_H_HYS_SEL(n) (((n)&0x3) << 0)
#define HEDET_LDET_H_HYS_SEL_20MV   (0x1)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_LDET_L_HYS_SEL(n) (((n)&0x3) << 2)
#define HEDET_LDET_L_HYS_SEL_20MV   (0x1)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_LDET_PU_PD(n) (((n)&0x3) << 4)
#define HEDET_LDET_PU_PD_PU         (0)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_LDET_REFH_SEL(n) (((n)&0x3) << 6)
#define HEDET_LDET_REFH_SEL_1P7     (0)
#define HEDET_LDET_REFH_SEL_1P9     (2)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_LDET_REFL_SEL(n) (((n)&0x7) << 8)
#define HEDET_LDET_REFL_SEL_300MV   (0x6)
#define HEDET_LDET_REFL_SEL_50MV    (0x1)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_MICDET_HYS_SEL(n) (((n)&0x3) << 11)
#define HEDET_MICDET_HYS_SEL_20MV   (0x1)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_MICDET_REF_SEL(n) (((n)&0x7) << 13)
#define HEDET_MICDET_REF_SEL_2P6    (0x6)

// ana_hdt2
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_EN_D2HDT_T(n) (((n)&0x3) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_EN_D2HDT_EN (1 << 2)
#define RDA2720M_AUD_CODEC_RG_HP_DRIVER_EN (1 << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_PLGPD_EN (1 << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_BDET_HYS_SEL(n) (((n)&0x3) << 5)
#define HEDET_BDET_HYS_SEL_20MV     (0x1)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_BDET_REF_SEL(n) (((n)&0xf) << 7)
#define HEDET_BDET_REF_SEL_850MV    (0xA)
#define RDA2720M_AUD_CODEC_RG_AUD_HEDET_JACK_TYPE(n) (((n)&0x3) << 11)
#define RDA2720M_AUD_CODEC_RG_CHG_PROC_STS_BYPASS (1 << 13)

// ana_dcl0
#define RDA2720M_AUD_CODEC_RG_AUD_DPOP_AUTO_RST (1 << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_DRV_SOFT_EN (1 << 1)
#define RDA2720M_AUD_CODEC_RG_AUD_DRV_SOFT_T(n) (((n)&0x7) << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_DCL_RST (1 << 5)
#define RDA2720M_AUD_CODEC_RG_AUD_DCL_EN (1 << 6)

// ana_dcl1
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OVP_DEG_T(n) (((n)&0x7) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OVP_DEG_EN (1 << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OVP_ABMOD_T(n) (((n)&0x7) << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OVP_ABMOD_PD (1 << 7)
#define RDA2720M_AUD_CODEC_RG_AUD_PACAL_DIV(n) (((n)&0x3) << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_PACAL_EN (1 << 10)

// ana_dcl2
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OCP_MUTE_T(n) (((n)&0x7) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OCP_MUTE_EN (1 << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OCP_DEG_T(n) (((n)&0x7) << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OCP_DEG_EN (1 << 7)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OTP_MUTE_EN (1 << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OTP_DEG_T(n) (((n)&0x7) << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OTP_DEG_EN (1 << 12)

// ana_dcl4
#define RDA2720M_AUD_CODEC_RG_HPR_DEPOP_CHG_CURSEL(n) (((n)&0xff) << 0)
#define RDA2720M_AUD_CODEC_RG_HPL_DEPOP_CHG_CURSEL(n) (((n)&0xff) << 8)

// ana_dcl5
#define RDA2720M_AUD_CODEC_RG_AUD_HPR_RDAC_STS (1 << 1)
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_RDAC_STS (1 << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_HP_DPOP_GAIN_T(n) (((n)&0x7) << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_HP_DPOP_GAIN_N2(n) (((n)&0x7) << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_HP_DPOP_GAIN_N1(n) (((n)&0x7) << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_HP_DPOP_FDOUT_EN (1 << 12)
#define RDA2720M_AUD_CODEC_RG_AUD_HP_DPOP_FDIN_EN (1 << 13)
#define RDA2720M_AUD_CODEC_RG_AUD_HPR_RDAC_START (1 << 14)
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_RDAC_START (1 << 15)

// ana_dcl6
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_DPOP_N2(n) (((n)&0x3) << 1)
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_DPOP_CLKN2(n) (((n)&0x3) << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_DPOP_VAL1(n) (((n)&0x7) << 5)
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_DPOP_N1(n) (((n)&0x3) << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_HPL_DPOP_CLKN1(n) (((n)&0x3) << 10)
#define RDA2720M_AUD_CODEC_RG_CALDC_WAIT_T(n) (((n)&0x7) << 12)

// ana_dcl7
#define RDA2720M_AUD_CODEC_RG_AUD_HPR_DPOP_N2(n) (((n)&0x3) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_HPR_DPOP_CLKN2(n) (((n)&0x3) << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_HPR_DPOP_VAL1(n) (((n)&0x7) << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_HPR_DPOP_N1(n) (((n)&0x3) << 7)
#define RDA2720M_AUD_CODEC_RG_AUD_HPR_DPOP_CLKN1(n) (((n)&0x3) << 9)
#define RDA2720M_AUD_CODEC_RG_DEPOPR_PCUR_OPT(n) (((n)&0x3) << 11)
#define RDA2720M_AUD_CODEC_RG_DEPOPL_PCUR_OPT(n) (((n)&0x3) << 13)

// ana_sts0
#define RDA2720M_AUD_CODEC_RG_DCCALI_RDACI_ADJ (1 << 0)
#define RDA2720M_AUD_CODEC_DC_CALI_IDAC_CURSEL(n) (((n)&0x3) << 1)
#define RDA2720M_AUD_CODEC_RG_DC_CALI_IDACVAL(n) (((n)&0x7) << 3)
#define RDA2720M_AUD_CODEC_RG_HP_DEPOP_WAIT_T4(n) (((n)&0x3) << 6)
#define RDA2720M_AUD_CODEC_RG_HP_DEPOP_WAIT_T3(n) (((n)&0x3) << 8)
#define RDA2720M_AUD_CODEC_RG_HP_DEPOP_WAIT_T2(n) (((n)&0x7) << 10)
#define RDA2720M_AUD_CODEC_RG_HP_DEPOP_WAIT_T1(n) (((n)&0x7) << 13)

// ana_sts2
#define RDA2720M_AUD_CODEC_RG_INSBUF_EN (1 << 1)
#define RDA2720M_AUD_CODEC_RG_HPR_PU_ENB (1 << 2)
#define RDA2720M_AUD_CODEC_RG_HPL_PU_ENB (1 << 3)
#define RDA2720M_AUD_CODEC_RG_AUD_RCV_DPOP_DVLD (1 << 4)
#define RDA2720M_AUD_CODEC_RG_DEPOP_CHG_STS (1 << 5)
#define RDA2720M_AUD_CODEC_RG_DEPOP_EN (1 << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_PLUGIN (1 << 7)
#define RDA2720M_AUD_CODEC_RG_DEPOP_CHG_EN (1 << 8)
#define RDA2720M_AUD_CODEC_RG_DEPOP_CHG_START (1 << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_HP_DPOP_DVLD (1 << 10)
#define RDA2720M_AUD_CODEC_RG_DCCALI_STS_BYPASS (1 << 11)
#define RDA2720M_AUD_CODEC_RG_AUD_DCCAL_STS (1 << 12)
#define RDA2720M_AUD_CODEC_RG_CALDC_ENO (1 << 13)
#define RDA2720M_AUD_CODEC_RG_CALDC_EN (1 << 14)
#define RDA2720M_AUD_CODEC_RG_CALDC_START (1 << 15)

// ana_sts3
#define RDA2720M_AUD_CODEC_RG_AUD_HWSW_SEL(n) (((n)&0x3f) << 0)
#define RDA2720M_AUD_CODEC_RG_DEPOP_OPA_SEL(n) (((n)&0x3) << 6)
#define RDA2720M_AUD_CODEC_RG_DEPOP_BIAS_SEL(n) (((n)&0x3) << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_CLK3_RESERVE(n) (((n)&0x3) << 10)

// ana_sts4
#define RDA2720M_AUD_CODEC_HPR_DCCAL_IDACL_SEL(n) (((n)&0x3) << 0)
#define RDA2720M_AUD_CODEC_HPL_DCCAL_IDACL_SEL(n) (((n)&0x3) << 2)
#define RDA2720M_AUD_CODEC_HPR_DCCAL_RDACL(n) (((n)&0xf) << 4)
#define RDA2720M_AUD_CODEC_HPL_DCCAL_RDACL(n) (((n)&0xff) << 8)

// ana_sts5
#define RDA2720M_AUD_CODEC_RG_HPR_COMP_INS(n) (((n)&0x3) << 2)
#define RDA2720M_AUD_CODEC_RG_HPL_COMP_INS(n) (((n)&0x3) << 4)
#define RDA2720M_AUD_CODEC_HPR_DCCAL_IDACL(n) (((n)&0x1f) << 6)
#define RDA2720M_AUD_CODEC_HPL_DCCAL_IDACL(n) (((n)&0x1f) << 11)

// ana_sts6
#define RDA2720M_AUD_CODEC_RG_AUD_PACAL_DVLD (1 << 2)
#define RDA2720M_AUD_CODEC_RG_AUD_PACAL_DO(n) (((n)&0x1fff) << 3)

// ana_sts7
#define RDA2720M_AUD_CODEC_RG_AUD_DRV_OCP_FLAG(n) (((n)&0xf) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OTP_FLAG (1 << 4)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_OVP_FLAG (1 << 5)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_SL_FLAG (1 << 6)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_SH_FLAG (1 << 7)
#define RDA2720M_AUD_CODEC_RG_AUD_HEAD_BUTTON (1 << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_HEAD_INSERT (1 << 9)
#define RDA2720M_AUD_CODEC_RG_AUD_HEAD_INSERT2 (1 << 10)
#define RDA2720M_AUD_CODEC_RG_AUD_HEAD_INSERT3 (1 << 11)
#define RDA2720M_AUD_CODEC_RG_AUD_HEAD_INSERT_ALL (1 << 12)

// ana_clk1
#define RDA2720M_AUD_CODEC_RG_AUD_CLK_PN_SEL(n) (((n)&0xff) << 0)
#define RDA2720M_AUD_CODEC_RG_AUD_PA_CLK_F(n) (((n)&0x3) << 8)
#define RDA2720M_AUD_CODEC_RG_AUD_DCDCCHG_CLK_F(n) (((n)&0x3) << 10)
#define RDA2720M_AUD_CODEC_RG_AUD_DCDCCORE_CLK_F(n) (((n)&0x3) << 12)
#define RDA2720M_AUD_CODEC_RG_AUD_DCDCGEN_CLK_F(n) (((n)&0x3) << 14)

#endif // _RDA2720M_AUD_CODEC_H_
