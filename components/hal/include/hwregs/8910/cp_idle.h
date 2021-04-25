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

#ifndef _CP_IDLE_H_
#define _CP_IDLE_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_IDLE_BASE (0x05090000)
#else
#define REG_IDLE_BASE (0x50090000)
#endif

typedef volatile struct
{
    uint32_t idl_ctrl_sys1;         // 0x00000000
    uint32_t idl_ctrl_sys2;         // 0x00000004
    uint32_t idl_en;                // 0x00000008
    uint32_t idl_m_timer;           // 0x0000000c
    uint32_t idl_wcn_en;            // 0x00000010
    uint32_t idl_ctrl_timer;        // 0x00000014
    uint32_t idl_m2_sys;            // 0x00000018
    uint32_t idl_tc_start;          // 0x0000001c
    uint32_t idl_tc_end;            // 0x00000020
    uint32_t idl_awk_timer;         // 0x00000024
    uint32_t gsm_lp_pu_done;        // 0x00000028
    uint32_t gsm_frame_inten;       // 0x0000002c
    uint32_t gsm_frame_int_sta;     // 0x00000030
    uint32_t ltem1_frame_inten;     // 0x00000034
    uint32_t ltem1_frame_int_sta;   // 0x00000038
    uint32_t ltem2_frame_inten;     // 0x0000003c
    uint32_t ltem2_frame_int_sta;   // 0x00000040
    uint32_t idl_sta;               // 0x00000044
    uint32_t idl_h_ctrl;            // 0x00000048
    uint32_t idl_h_val;             // 0x0000004c
    uint32_t idl_h_gsm;             // 0x00000050
    uint32_t idl_h_ltem;            // 0x00000054
    uint32_t idl_awk_en;            // 0x00000058
    uint32_t idl_awk_st;            // 0x0000005c
    uint32_t idl_awk_self;          // 0x00000060
    uint32_t idl_osw1_en;           // 0x00000064
    uint32_t idl_osw1_cont;         // 0x00000068
    uint32_t idl_fn_gsm;            // 0x0000006c
    uint32_t idl_fn_ltem1;          // 0x00000070
    uint32_t idl_fn_ltem2;          // 0x00000074
    uint32_t idl_ltem_rfl;          // 0x00000078
    uint32_t idl_ltem_sfl;          // 0x0000007c
    uint32_t idl_sig_en;            // 0x00000080
    uint32_t idl_sig_timer;         // 0x00000084
    uint32_t idl_32k_ref;           // 0x00000088
    uint32_t cp_inten;              // 0x0000008c
    uint32_t cp_inten_set;          // 0x00000090
    uint32_t cp_inten_clr;          // 0x00000094
    uint32_t cp_int_sta;            // 0x00000098
    uint32_t ap_inten;              // 0x0000009c
    uint32_t ap_inten_set;          // 0x000000a0
    uint32_t ap_inten_clr;          // 0x000000a4
    uint32_t ap_int_sta;            // 0x000000a8
    uint32_t ltem1_cfsr_hfn;        // 0x000000ac
    uint32_t ltem1_cfsr_fn;         // 0x000000b0
    uint32_t ltem1_cfsrs;           // 0x000000b4
    uint32_t ltem1_cfsr_rdh;        // 0x000000b8
    uint32_t ltem1_cfsr_rdl;        // 0x000000bc
    uint32_t ltem1_framc;           // 0x000000c0
    uint32_t ltem1_framl;           // 0x000000c4
    uint32_t ltem1_framls;          // 0x000000c8
    uint32_t ltem1_cfsr_tph;        // 0x000000cc
    uint32_t ltem1_cfsr_tpl;        // 0x000000d0
    uint32_t ltem1_framc_tp;        // 0x000000d4
    uint32_t ltem2_cfsr_hfn;        // 0x000000d8
    uint32_t ltem2_cfsr_fn;         // 0x000000dc
    uint32_t ltem2_cfsrs;           // 0x000000e0
    uint32_t ltem2_cfsr_rdh;        // 0x000000e4
    uint32_t ltem2_cfsr_rdl;        // 0x000000e8
    uint32_t ltem2_framc;           // 0x000000ec
    uint32_t ltem2_framl;           // 0x000000f0
    uint32_t ltem2_framls;          // 0x000000f4
    uint32_t ltem2_cfsr_tph;        // 0x000000f8
    uint32_t ltem2_cfsr_tpl;        // 0x000000fc
    uint32_t ltem2_framc_tp;        // 0x00000100
    uint32_t gsm_cfsr;              // 0x00000104
    uint32_t gsm_cfsrs;             // 0x00000108
    uint32_t gsm_cfsro;             // 0x0000010c
    uint32_t ltem1_fhl;             // 0x00000110
    uint32_t ltem1_fll;             // 0x00000114
    uint32_t ltem1_fcl;             // 0x00000118
    uint32_t ltem2_fhl;             // 0x0000011c
    uint32_t ltem2_fll;             // 0x00000120
    uint32_t ltem2_fcl;             // 0x00000124
    uint32_t gsm_fl;                // 0x00000128
    uint32_t gsm_fcl;               // 0x0000012c
    uint32_t tpctrl;                // 0x00000130
    uint32_t layoutt;               // 0x00000134
    uint32_t layoutctrl;            // 0x00000138
    uint32_t ltem1_fint_dly1;       // 0x0000013c
    uint32_t ltem1_fint_dly2;       // 0x00000140
    uint32_t ltem2_fint_dly1;       // 0x00000144
    uint32_t ltem2_fint_dly2;       // 0x00000148
    uint32_t fint_en;               // 0x0000014c
    uint32_t timer_en;              // 0x00000150
    uint32_t idle_frame_sta;        // 0x00000154
    uint32_t idle_frame_ltem1;      // 0x00000158
    uint32_t idle_frame_ltem2;      // 0x0000015c
    uint32_t idle_frame_gsm;        // 0x00000160
    uint32_t idle_frame_lte;        // 0x00000164
    uint32_t idle_frame_lte_conf;   // 0x00000168
    uint32_t ltem_ref_fn;           // 0x0000016c
    uint32_t ltem_ref_fnl;          // 0x00000170
    uint32_t ltem_ref_fcl;          // 0x00000174
    uint32_t ref_32k_fnl;           // 0x00000178
    uint32_t ltem_ref_fc;           // 0x0000017c
    uint32_t gsm_framl;             // 0x00000180
    uint32_t idl_osw2_en;           // 0x00000184
    uint32_t idl_osw2_cont;         // 0x00000188
    uint32_t idle_framc_gsm;        // 0x0000018c
    uint32_t ltem1_fint_dly3;       // 0x00000190
    uint32_t ltem2_fint_dly3;       // 0x00000194
    uint32_t idle_time_sel;         // 0x00000198
    uint32_t idle_time;             // 0x0000019c
    uint32_t idl_h_gsm_lp;          // 0x000001a0
    uint32_t idl_h_ltem_lp;         // 0x000001a4
    uint32_t idl_tc_start_nb;       // 0x000001a8
    uint32_t idl_tc_end_nb;         // 0x000001ac
    uint32_t nb_lp_pu_done;         // 0x000001b0
    uint32_t idl_h_nb;              // 0x000001b4
    uint32_t idl_h_nb_lp;           // 0x000001b8
    uint32_t idl_fn_nb;             // 0x000001bc
    uint32_t nb_frame_inten;        // 0x000001c0
    uint32_t nb_frame_int_sta;      // 0x000001c4
    uint32_t nb_cfsr;               // 0x000001c8
    uint32_t nb_framl;              // 0x000001cc
    uint32_t nb_cfsrs;              // 0x000001d0
    uint32_t nb_cfsro;              // 0x000001d4
    uint32_t nb_fl;                 // 0x000001d8
    uint32_t nb_fcl;                // 0x000001dc
    uint32_t idle_frame_nb;         // 0x000001e0
    uint32_t idle_framc_nb;         // 0x000001e4
    uint32_t idl_awk_en_set;        // 0x000001e8
    uint32_t idl_awk_en_clr;        // 0x000001ec
    uint32_t gsm_framc;             // 0x000001f0
    uint32_t nb_framc;              // 0x000001f4
    uint32_t eliminat_jitter;       // 0x000001f8
    uint32_t gsm_en_sel;            // 0x000001fc
    uint32_t nb_en_sel;             // 0x00000200
    uint32_t pd_pll_sw;             // 0x00000204
    uint32_t pd_pll_sw_set;         // 0x00000208
    uint32_t pd_pll_sw_clr;         // 0x0000020c
    uint32_t pd_pll_sel;            // 0x00000210
    uint32_t pd_pll_sel_set;        // 0x00000214
    uint32_t pd_pll_sel_clr;        // 0x00000218
    uint32_t idle_cg_sw;            // 0x0000021c
    uint32_t idle_cg_sw_set;        // 0x00000220
    uint32_t idle_cg_sw_clr;        // 0x00000224
    uint32_t idle_cg_sel;           // 0x00000228
    uint32_t idle_cg_sel_set;       // 0x0000022c
    uint32_t idle_cg_sel_clr;       // 0x00000230
    uint32_t rf_idle_enable_sw;     // 0x00000234
    uint32_t rf_idle_enable_sel;    // 0x00000238
    uint32_t __572[8];              // 0x0000023c
    uint32_t mem_ema_cfg;           // 0x0000025c
    uint32_t uart_ctrl;             // 0x00000260
    uint32_t ddr_latch;             // 0x00000264
    uint32_t pad_ctrl;              // 0x00000268
    uint32_t __620[4];              // 0x0000026c
    uint32_t pad_ctrl_uart_txd;     // 0x0000027c
    uint32_t mon_sel;               // 0x00000280
    uint32_t mon_sel_set;           // 0x00000284
    uint32_t mon_sel_clr;           // 0x00000288
    uint32_t target_timer;          // 0x0000028c
    uint32_t target_timer_en;       // 0x00000290
    uint32_t target_value_lock;     // 0x00000294
    uint32_t target_timer_stat;     // 0x00000298
    uint32_t slow_sys_clk_sel_hwen; // 0x0000029c
    uint32_t slow_clk_sel_hwen;     // 0x000002a0
    uint32_t sleep_prot_time;       // 0x000002a4
    uint32_t __680[3926];           // 0x000002a8
    uint32_t idle_res0;             // 0x00004000
    uint32_t idle_res1;             // 0x00004004
    uint32_t idle_res2;             // 0x00004008
    uint32_t idle_res3;             // 0x0000400c
    uint32_t idle_res4;             // 0x00004010
    uint32_t idle_res5;             // 0x00004014
    uint32_t idle_res6;             // 0x00004018
    uint32_t idle_res7;             // 0x0000401c
    uint32_t idle_res8;             // 0x00004020
    uint32_t idle_res9;             // 0x00004024
    uint32_t idle_res10;            // 0x00004028
    uint32_t idle_res11;            // 0x0000402c
} HWP_CP_IDLE_T;

#define hwp_idle ((HWP_CP_IDLE_T *)REG_ACCESS_ADDRESS(REG_IDLE_BASE))

// idl_ctrl_sys1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idct_ctrl_sys1 : 1; // [0]
        uint32_t __31_1 : 31;        // [31:1]
    } b;
} REG_CP_IDLE_IDL_CTRL_SYS1_T;

// idl_ctrl_sys2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idct_ctrl_sys2 : 1; // [0]
        uint32_t __31_1 : 31;        // [31:1]
    } b;
} REG_CP_IDLE_IDL_CTRL_SYS2_T;

// idl_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idl_cp_en : 1; // [0]
        uint32_t idl_ap_en : 1; // [1]
        uint32_t __31_2 : 30;   // [31:2]
    } b;
} REG_CP_IDLE_IDL_EN_T;

// idl_wcn_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wcn_chip_pd : 1; // [0]
        uint32_t wcn_pd_xtal : 1; // [1]
        uint32_t wcn_pd_pll : 1;  // [2]
        uint32_t wcn_idle_cg : 1; // [3]
        uint32_t wcn_res_val : 1; // [4]
        uint32_t __31_5 : 27;     // [31:5]
    } b;
} REG_CP_IDLE_IDL_WCN_EN_T;

// idl_ctrl_timer
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idct_ctrl_timer : 1; // [0]
        uint32_t __31_1 : 31;         // [31:1]
    } b;
} REG_CP_IDLE_IDL_CTRL_TIMER_T;

// idl_m2_sys
typedef union {
    uint32_t v;
    struct
    {
        uint32_t m2_sys : 16; // [15:0]
        uint32_t m1_sys : 16; // [31:16]
    } b;
} REG_CP_IDLE_IDL_M2_SYS_T;

// idl_tc_start
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tc_start_mod : 2; // [1:0]
        uint32_t __31_2 : 30;      // [31:2]
    } b;
} REG_CP_IDLE_IDL_TC_START_T;

// idl_tc_end
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tc_end_mod : 2;    // [1:0]
        uint32_t __3_2 : 2;         // [3:2]
        uint32_t tc_end_framc : 17; // [20:4]
        uint32_t __31_21 : 11;      // [31:21]
    } b;
} REG_CP_IDLE_IDL_TC_END_T;

// idl_awk_timer
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wake_timer : 1; // [0]
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_CP_IDLE_IDL_AWK_TIMER_T;

// gsm_lp_pu_done
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lp_pu_done : 1; // [0]
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_CP_IDLE_GSM_LP_PU_DONE_T;

// gsm_frame_inten
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gsm_frame_irq_en : 1; // [0]
        uint32_t __31_1 : 31;          // [31:1]
    } b;
} REG_CP_IDLE_GSM_FRAME_INTEN_T;

// gsm_frame_int_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gsm_frame_int_sta : 1; // [0]
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_CP_IDLE_GSM_FRAME_INT_STA_T;

// ltem1_frame_inten
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem1_frame1_irq_en : 1; // [0]
        uint32_t ltem1_frame2_irq_en : 1; // [1]
        uint32_t ltem1_frame3_irq_en : 1; // [2]
        uint32_t __31_3 : 29;             // [31:3]
    } b;
} REG_CP_IDLE_LTEM1_FRAME_INTEN_T;

// ltem1_frame_int_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem1_frame_int_sta : 3; // [2:0], write clear
        uint32_t __31_3 : 29;             // [31:3]
    } b;
} REG_CP_IDLE_LTEM1_FRAME_INT_STA_T;

// ltem2_frame_inten
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem2_frame1_irq_en : 1; // [0]
        uint32_t ltem2_frame2_irq_en : 1; // [1]
        uint32_t ltem2_frame3_irq_en : 1; // [2]
        uint32_t __31_3 : 29;             // [31:3]
    } b;
} REG_CP_IDLE_LTEM2_FRAME_INTEN_T;

// ltem2_frame_int_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem2_frame_int_sta : 3; // [2:0], write clear
        uint32_t __31_3 : 29;             // [31:3]
    } b;
} REG_CP_IDLE_LTEM2_FRAME_INT_STA_T;

// idl_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idle_sys_stat : 1;         // [0], read only
        uint32_t idle_gsm_timer_stat : 1;   // [1], read only
        uint32_t idle_ltem1_timer_stat : 1; // [2], read only
        uint32_t idle_ltem2_timer_stat : 1; // [3], read only
        uint32_t h_stat : 1;                // [4], read only
        uint32_t idle_nb_timer_stat : 1;    // [5], read only
        uint32_t __31_6 : 26;               // [31:6]
    } b;
} REG_CP_IDLE_IDL_STA_T;

// idl_h_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;      // [0]
        uint32_t h_ctrl_en : 1;  // [1]
        uint32_t h_auto_en : 1;  // [2]
        uint32_t h_run_time : 4; // [6:3]
        uint32_t __31_7 : 25;    // [31:7]
    } b;
} REG_CP_IDLE_IDL_H_CTRL_T;

// idl_h_val
typedef union {
    uint32_t v;
    struct
    {
        uint32_t h_value : 27; // [26:0]
        uint32_t __31_27 : 5;  // [31:27]
    } b;
} REG_CP_IDLE_IDL_H_VAL_T;

// idl_h_gsm
typedef union {
    uint32_t v;
    struct
    {
        uint32_t h_value : 27; // [26:0]
        uint32_t __31_27 : 5;  // [31:27]
    } b;
} REG_CP_IDLE_IDL_H_GSM_T;

// idl_h_ltem
typedef union {
    uint32_t v;
    struct
    {
        uint32_t h_value : 27; // [26:0]
        uint32_t __31_27 : 5;  // [31:27]
    } b;
} REG_CP_IDLE_IDL_H_LTEM_T;

// idl_awk_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awk0_en : 1;            // [0]
        uint32_t awk1_en : 1;            // [1]
        uint32_t awk2_en : 1;            // [2]
        uint32_t awk3_en : 1;            // [3]
        uint32_t awk4_en : 1;            // [4]
        uint32_t awk5_en : 1;            // [5]
        uint32_t awk6_en : 1;            // [6]
        uint32_t awk7_en : 1;            // [7]
        uint32_t awk_osw1_en : 1;        // [8]
        uint32_t awk_osw2_en : 1;        // [9]
        uint32_t awk_self_en : 1;        // [10]
        uint32_t gsm_lp_pu_reach_en : 1; // [11]
        uint32_t nb_lp_pu_reach_en : 1;  // [12]
        uint32_t __31_13 : 19;           // [31:13]
    } b;
} REG_CP_IDLE_IDL_AWK_EN_T;

// idl_awk_st
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awk0_awk_stat : 1;        // [0], write clear
        uint32_t awk1_awk_stat : 1;        // [1], write clear
        uint32_t awk2_awk_stat : 1;        // [2], write clear
        uint32_t awk3_awk_stat : 1;        // [3], write clear
        uint32_t awk4_awk_stat : 1;        // [4], write clear
        uint32_t awk5_awk_stat : 1;        // [5], write clear
        uint32_t awk6_awk_stat : 1;        // [6], write clear
        uint32_t awk7_awk_stat : 1;        // [7], write clear
        uint32_t awk_osw1_stat : 1;        // [8], write clear
        uint32_t awk_osw2_stat : 1;        // [9], write clear
        uint32_t awk_self_stat : 1;        // [10], write clear
        uint32_t gsm_lp_pu_reach_stat : 1; // [11], write clear
        uint32_t nb_lp_pu_reach_stat : 1;  // [12], write clear
        uint32_t __15_13 : 3;              // [15:13]
        uint32_t awk_up_stat : 1;          // [16], write clear
        uint32_t __19_17 : 3;              // [19:17]
        uint32_t idle_stat : 1;            // [20], write clear
        uint32_t __23_21 : 3;              // [23:21]
        uint32_t pow_sta : 1;              // [24], write clear
        uint32_t thr_sta : 1;              // [25], write clear
        uint32_t pow_dfe_sta : 1;          // [26], read only
        uint32_t __31_27 : 5;              // [31:27]
    } b;
} REG_CP_IDLE_IDL_AWK_ST_T;

// idl_awk_self
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wake_self : 1; // [0]
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CP_IDLE_IDL_AWK_SELF_T;

// idl_osw1_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t osw1_time : 31; // [30:0]
        uint32_t osw1_en : 1;    // [31]
    } b;
} REG_CP_IDLE_IDL_OSW1_EN_T;

// idl_fn_ltem1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idfn_sub_ltem : 4;  // [3:0], read only
        uint32_t idfn_rad_ltem : 28; // [31:4], read only
    } b;
} REG_CP_IDLE_IDL_FN_LTEM1_T;

// idl_fn_ltem2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idfn_sub_ltem : 4;  // [3:0], read only
        uint32_t idfn_rad_ltem : 28; // [31:4], read only
    } b;
} REG_CP_IDLE_IDL_FN_LTEM2_T;

// idl_ltem_rfl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_idle_radioframe_parameter : 21; // [20:0]
        uint32_t __31_21 : 11;                        // [31:21]
    } b;
} REG_CP_IDLE_IDL_LTEM_RFL_T;

// idl_ltem_sfl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_idle_frame_parameter : 17; // [16:0]
        uint32_t __31_17 : 15;                   // [31:17]
    } b;
} REG_CP_IDLE_IDL_LTEM_SFL_T;

// idl_sig_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chip_pd_en : 1; // [0]
        uint32_t pd_xtal_en : 1; // [1]
        uint32_t pd_pll_en : 1;  // [2]
        uint32_t idle_cg_en : 1; // [3]
        uint32_t __31_4 : 28;    // [31:4]
    } b;
} REG_CP_IDLE_IDL_SIG_EN_T;

// idl_sig_timer
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t1 : 8; // [7:0]
        uint32_t t2 : 8; // [15:8]
        uint32_t t3 : 8; // [23:16]
        uint32_t t4 : 8; // [31:24]
    } b;
} REG_CP_IDLE_IDL_SIG_TIMER_T;

// cp_inten
typedef union {
    uint32_t v;
    struct
    {
        uint32_t layout_irq : 1;       // [0]
        uint32_t idle_h_irq : 1;       // [1]
        uint32_t idle_frame_irq : 1;   // [2]
        uint32_t tstamp_irq : 1;       // [3]
        uint32_t osw1_irq : 1;         // [4]
        uint32_t gsm_tc_start_irq : 1; // [5]
        uint32_t gsm_tc_end_irq : 1;   // [6]
        uint32_t gsm_pu_reach_irq : 1; // [7]
        uint32_t timer_awk_irq : 1;    // [8]
        uint32_t sys_wak_irq : 1;      // [9]
        uint32_t nb_tc_start_irq : 1;  // [10]
        uint32_t nb_tc_end_irq : 1;    // [11]
        uint32_t nb_pu_reach_irq : 1;  // [12]
        uint32_t target_irq : 1;       // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_CP_IDLE_CP_INTEN_T;

// cp_inten_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t int_en_set : 14; // [13:0], write set
        uint32_t __31_14 : 18;    // [31:14]
    } b;
} REG_CP_IDLE_CP_INTEN_SET_T;

// cp_inten_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t int_en_clr : 14; // [13:0], write clear
        uint32_t __31_14 : 18;    // [31:14]
    } b;
} REG_CP_IDLE_CP_INTEN_CLR_T;

// cp_int_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cp_int_sta : 14; // [13:0], write clear
        uint32_t __31_14 : 18;    // [31:14]
    } b;
} REG_CP_IDLE_CP_INT_STA_T;

// ap_inten
typedef union {
    uint32_t v;
    struct
    {
        uint32_t osw2_irq : 1;         // [0]
        uint32_t gsm_pu_reach_irq : 1; // [1]
        uint32_t timer_awk_irq : 1;    // [2]
        uint32_t sys_wak_irq : 1;      // [3]
        uint32_t nb_pu_reach_irq : 1;  // [4]
        uint32_t target_irq : 1;       // [5]
        uint32_t __31_6 : 26;          // [31:6]
    } b;
} REG_CP_IDLE_AP_INTEN_T;

// ap_inten_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t int_en_set : 6; // [5:0], write set
        uint32_t __31_6 : 26;    // [31:6]
    } b;
} REG_CP_IDLE_AP_INTEN_SET_T;

// ap_inten_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t int_en_clr : 6; // [5:0], write clear
        uint32_t __31_6 : 26;    // [31:6]
    } b;
} REG_CP_IDLE_AP_INTEN_CLR_T;

// ap_int_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_int_sta : 6; // [5:0], write clear
        uint32_t __31_6 : 26;    // [31:6]
    } b;
} REG_CP_IDLE_AP_INT_STA_T;

// ltem1_cfsr_hfn
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_hfn : 22; // [21:0]
        uint32_t __31_22 : 10;       // [31:22]
    } b;
} REG_CP_IDLE_LTEM1_CFSR_HFN_T;

// ltem1_cfsr_fn
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_sub : 4;  // [3:0]
        uint32_t ltem_cfsr_rad : 10; // [13:4]
        uint32_t __31_14 : 18;       // [31:14]
    } b;
} REG_CP_IDLE_LTEM1_CFSR_FN_T;

// ltem1_cfsrs
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsrs : 24;   // [23:0]
        uint32_t adjust_direct : 1; // [24]
        uint32_t active_time : 1;   // [25]
        uint32_t __31_26 : 6;       // [31:26]
    } b;
} REG_CP_IDLE_LTEM1_CFSRS_T;

// ltem1_cfsr_rdh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_rdh : 22; // [21:0], read only
        uint32_t __31_22 : 10;       // [31:22]
    } b;
} REG_CP_IDLE_LTEM1_CFSR_RDH_T;

// ltem1_cfsr_rdl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_rdl_sub : 4;  // [3:0], read only
        uint32_t ltem_cfsr_rdl_rad : 10; // [13:4], read only
        uint32_t __31_14 : 18;           // [31:14]
    } b;
} REG_CP_IDLE_LTEM1_CFSR_RDL_T;

// ltem1_framc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lframc : 16;  // [15:0], read only
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_CP_IDLE_LTEM1_FRAMC_T;

// ltem1_framl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lframl : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_CP_IDLE_LTEM1_FRAML_T;

// ltem1_framls
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lframls : 16;    // [15:0]
        uint32_t active_time : 1; // [16]
        uint32_t __31_17 : 15;    // [31:17]
    } b;
} REG_CP_IDLE_LTEM1_FRAMLS_T;

// ltem1_cfsr_tph
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_tph : 22; // [21:0]
        uint32_t __31_22 : 10;       // [31:22]
    } b;
} REG_CP_IDLE_LTEM1_CFSR_TPH_T;

// ltem1_cfsr_tpl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_tpl : 14; // [13:0]
        uint32_t __31_14 : 18;       // [31:14]
    } b;
} REG_CP_IDLE_LTEM1_CFSR_TPL_T;

// ltem1_framc_tp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_framc_tp : 16; // [15:0]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_CP_IDLE_LTEM1_FRAMC_TP_T;

// ltem2_cfsr_hfn
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_hfn : 22; // [21:0]
        uint32_t __31_22 : 10;       // [31:22]
    } b;
} REG_CP_IDLE_LTEM2_CFSR_HFN_T;

// ltem2_cfsr_fn
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_sub : 4;  // [3:0]
        uint32_t ltem_cfsr_rad : 10; // [13:4]
        uint32_t __31_14 : 18;       // [31:14]
    } b;
} REG_CP_IDLE_LTEM2_CFSR_FN_T;

// ltem2_cfsrs
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsrs : 24;   // [23:0]
        uint32_t adjust_direct : 1; // [24]
        uint32_t active_time : 1;   // [25]
        uint32_t __31_26 : 6;       // [31:26]
    } b;
} REG_CP_IDLE_LTEM2_CFSRS_T;

// ltem2_cfsr_rdh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_rdh : 22; // [21:0], read only
        uint32_t __31_22 : 10;       // [31:22]
    } b;
} REG_CP_IDLE_LTEM2_CFSR_RDH_T;

// ltem2_cfsr_rdl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_rdl_sub : 4;  // [3:0], read only
        uint32_t ltem_cfsr_rdl_rad : 10; // [13:4], read only
        uint32_t __31_14 : 18;           // [31:14]
    } b;
} REG_CP_IDLE_LTEM2_CFSR_RDL_T;

// ltem2_framc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lframc : 16;  // [15:0], read only
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_CP_IDLE_LTEM2_FRAMC_T;

// ltem2_framl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lframl : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_CP_IDLE_LTEM2_FRAML_T;

// ltem2_framls
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lframls : 16;    // [15:0]
        uint32_t active_time : 1; // [16]
        uint32_t __31_17 : 15;    // [31:17]
    } b;
} REG_CP_IDLE_LTEM2_FRAMLS_T;

// ltem2_cfsr_tph
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_tph : 22; // [21:0]
        uint32_t __31_22 : 10;       // [31:22]
    } b;
} REG_CP_IDLE_LTEM2_CFSR_TPH_T;

// ltem2_cfsr_tpl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_cfsr_tpl : 14; // [13:0]
        uint32_t __31_14 : 18;       // [31:14]
    } b;
} REG_CP_IDLE_LTEM2_CFSR_TPL_T;

// ltem2_framc_tp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_framc_tp : 16; // [15:0]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_CP_IDLE_LTEM2_FRAMC_TP_T;

// gsm_cfsr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gsm_cfsr_gsm_cfsr : 24; // [23:0]
        uint32_t __31_24 : 8;            // [31:24]
    } b;
} REG_CP_IDLE_GSM_CFSR_T;

// gsm_cfsrs
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gsm_cfsrs_gsm_cfsrs : 24; // [23:0]
        uint32_t adjust_direct : 1;        // [24]
        uint32_t __31_25 : 7;              // [31:25]
    } b;
} REG_CP_IDLE_GSM_CFSRS_T;

// gsm_cfsro
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gsm_cfsr_overflow : 24; // [23:0]
        uint32_t __31_24 : 8;            // [31:24]
    } b;
} REG_CP_IDLE_GSM_CFSRO_T;

// ltem1_fhl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_fhl : 22; // [21:0], read only
        uint32_t __31_22 : 10;  // [31:22]
    } b;
} REG_CP_IDLE_LTEM1_FHL_T;

// ltem1_fll
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_fll : 14; // [13:0], read only
        uint32_t __31_14 : 18;  // [31:14]
    } b;
} REG_CP_IDLE_LTEM1_FLL_T;

// ltem1_fcl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_fcl : 16; // [15:0], read only
        uint32_t __31_16 : 16;  // [31:16]
    } b;
} REG_CP_IDLE_LTEM1_FCL_T;

// ltem2_fhl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_fhl : 22; // [21:0], read only
        uint32_t __31_22 : 10;  // [31:22]
    } b;
} REG_CP_IDLE_LTEM2_FHL_T;

// ltem2_fll
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_fll : 14; // [13:0], read only
        uint32_t __31_14 : 18;  // [31:14]
    } b;
} REG_CP_IDLE_LTEM2_FLL_T;

// ltem2_fcl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_fcl : 16; // [15:0], read only
        uint32_t __31_16 : 16;  // [31:16]
    } b;
} REG_CP_IDLE_LTEM2_FCL_T;

// gsm_fl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fl : 24;     // [23:0], read only
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_CP_IDLE_GSM_FL_T;

// gsm_fcl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gsm_fcl_gsm_fcl : 17; // [16:0], read only
        uint32_t __31_17 : 15;         // [31:17]
    } b;
} REG_CP_IDLE_GSM_FCL_T;

// tpctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inner_ctrl : 1;  // [0]
        uint32_t __3_1 : 3;       // [3:1]
        uint32_t inner_confg : 2; // [5:4]
        uint32_t __7_6 : 2;       // [7:6]
        uint32_t mod_sel : 3;     // [10:8]
        uint32_t __31_11 : 21;    // [31:11]
    } b;
} REG_CP_IDLE_TPCTRL_T;

// layoutctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;       // [0]
        uint32_t timer_select : 1; // [1]
        uint32_t __7_2 : 6;        // [7:2]
        uint32_t chip_count : 15;  // [22:8]
        uint32_t __31_23 : 9;      // [31:23]
    } b;
} REG_CP_IDLE_LAYOUTCTRL_T;

// ltem1_fint_dly1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay_time : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_CP_IDLE_LTEM1_FINT_DLY1_T;

// ltem1_fint_dly2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay_time : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_CP_IDLE_LTEM1_FINT_DLY2_T;

// ltem2_fint_dly1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay_time : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_CP_IDLE_LTEM2_FINT_DLY1_T;

// ltem2_fint_dly2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay_time : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_CP_IDLE_LTEM2_FINT_DLY2_T;

// fint_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lte_m1_fint_enable : 10; // [9:0]
        uint32_t __11_10 : 2;             // [11:10]
        uint32_t lte_m2_fint_enable : 10; // [21:12]
        uint32_t __31_22 : 10;            // [31:22]
    } b;
} REG_CP_IDLE_FINT_EN_T;

// timer_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lte_m1_timer_enable : 1; // [0]
        uint32_t lte_m2_timer_enable : 1; // [1]
        uint32_t gsm_timer_enable : 1;    // [2]
        uint32_t ltem_timer_enable : 1;   // [3]
        uint32_t nb_timer_enable : 1;     // [4]
        uint32_t __31_5 : 27;             // [31:5]
    } b;
} REG_CP_IDLE_TIMER_EN_T;

// idle_frame_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lte_m1_frame_state : 1; // [0], write clear
        uint32_t lte_m2_frame_state : 1; // [1], write clear
        uint32_t gsm_frame_state : 1;    // [2], write clear
        uint32_t lte_m_frame_state : 1;  // [3], write clear
        uint32_t nb_frame_state : 1;     // [4], write clear
        uint32_t __31_5 : 27;            // [31:5]
    } b;
} REG_CP_IDLE_IDLE_FRAME_STA_T;

// idle_frame_ltem1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frame_cfsr : 22; // [21:0]
        uint32_t __23_22 : 2;     // [23:22]
        uint32_t frame_conf : 1;  // [24]
        uint32_t __31_25 : 7;     // [31:25]
    } b;
} REG_CP_IDLE_IDLE_FRAME_LTEM1_T;

// idle_frame_ltem2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frame_cfsr : 22; // [21:0]
        uint32_t __23_22 : 2;     // [23:22]
        uint32_t frame_conf : 1;  // [24]
        uint32_t __31_25 : 7;     // [31:25]
    } b;
} REG_CP_IDLE_IDLE_FRAME_LTEM2_T;

// idle_frame_gsm
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frame_cfsr : 24; // [23:0]
        uint32_t frame_conf : 1;  // [24]
        uint32_t __31_25 : 7;     // [31:25]
    } b;
} REG_CP_IDLE_IDLE_FRAME_GSM_T;

// idle_frame_lte_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frame_ref_lte_conf : 1; // [0]
        uint32_t resrved : 31;           // [31:1], read only
    } b;
} REG_CP_IDLE_IDLE_FRAME_LTE_CONF_T;

// ltem_ref_fcl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ref_ltem_fcl : 15; // [14:0], read only
        uint32_t __31_15 : 17;      // [31:15]
    } b;
} REG_CP_IDLE_LTEM_REF_FCL_T;

// ltem_ref_fc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ltem_ref_fc_ltem_ref_fc : 15; // [14:0], read only
        uint32_t __31_15 : 17;                 // [31:15]
    } b;
} REG_CP_IDLE_LTEM_REF_FC_T;

// gsm_framl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gsm_framl : 17; // [16:0]
        uint32_t __31_17 : 15;   // [31:17]
    } b;
} REG_CP_IDLE_GSM_FRAML_T;

// idl_osw2_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t osw2_time : 31; // [30:0]
        uint32_t osw2_en : 1;    // [31]
    } b;
} REG_CP_IDLE_IDL_OSW2_EN_T;

// idle_framc_gsm
typedef union {
    uint32_t v;
    struct
    {
        uint32_t framc_cfsr : 17; // [16:0]
        uint32_t __31_17 : 15;    // [31:17]
    } b;
} REG_CP_IDLE_IDLE_FRAMC_GSM_T;

// ltem1_fint_dly3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay_time : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_CP_IDLE_LTEM1_FINT_DLY3_T;

// ltem2_fint_dly3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay_time : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_CP_IDLE_LTEM2_FINT_DLY3_T;

// idle_time_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t time_sel : 1; // [0]
        uint32_t __31_1 : 31;  // [31:1]
    } b;
} REG_CP_IDLE_IDLE_TIME_SEL_T;

// idl_h_gsm_lp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t h_value : 27; // [26:0]
        uint32_t __31_27 : 5;  // [31:27]
    } b;
} REG_CP_IDLE_IDL_H_GSM_LP_T;

// idl_h_ltem_lp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t h_value : 27; // [26:0]
        uint32_t __31_27 : 5;  // [31:27]
    } b;
} REG_CP_IDLE_IDL_H_LTEM_LP_T;

// idl_tc_start_nb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tc_start_mod : 2; // [1:0]
        uint32_t __31_2 : 30;      // [31:2]
    } b;
} REG_CP_IDLE_IDL_TC_START_NB_T;

// idl_tc_end_nb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tc_end_mod : 2;    // [1:0]
        uint32_t __3_2 : 2;         // [3:2]
        uint32_t tc_end_framc : 17; // [20:4]
        uint32_t __31_21 : 11;      // [31:21]
    } b;
} REG_CP_IDLE_IDL_TC_END_NB_T;

// nb_lp_pu_done
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lp_pu_done : 1; // [0]
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_CP_IDLE_NB_LP_PU_DONE_T;

// idl_h_nb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t h_value : 27; // [26:0]
        uint32_t __31_27 : 5;  // [31:27]
    } b;
} REG_CP_IDLE_IDL_H_NB_T;

// idl_h_nb_lp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t h_value : 27; // [26:0]
        uint32_t __31_27 : 5;  // [31:27]
    } b;
} REG_CP_IDLE_IDL_H_NB_LP_T;

// nb_frame_inten
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nb_frame_irq_en : 1; // [0]
        uint32_t __31_1 : 31;         // [31:1]
    } b;
} REG_CP_IDLE_NB_FRAME_INTEN_T;

// nb_frame_int_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gsm_frame_int_sta : 1; // [0]
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_CP_IDLE_NB_FRAME_INT_STA_T;

// nb_cfsr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gsm_cfsr : 24; // [23:0]
        uint32_t __31_24 : 8;   // [31:24]
    } b;
} REG_CP_IDLE_NB_CFSR_T;

// nb_framl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nb_framl : 17; // [16:0]
        uint32_t __31_17 : 15;  // [31:17]
    } b;
} REG_CP_IDLE_NB_FRAML_T;

// nb_cfsrs
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nb_cfsrs_nb_cfsrs : 24; // [23:0]
        uint32_t adjust_direct : 1;      // [24]
        uint32_t __31_25 : 7;            // [31:25]
    } b;
} REG_CP_IDLE_NB_CFSRS_T;

// nb_cfsro
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nb_cfsr_overflow : 24; // [23:0]
        uint32_t __31_24 : 8;           // [31:24]
    } b;
} REG_CP_IDLE_NB_CFSRO_T;

// nb_fl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fl : 24;     // [23:0], read only
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_CP_IDLE_NB_FL_T;

// nb_fcl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nb_fcl_nb_fcl : 17; // [16:0], read only
        uint32_t __31_17 : 15;       // [31:17]
    } b;
} REG_CP_IDLE_NB_FCL_T;

// idle_frame_nb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frame_cfsr : 24; // [23:0]
        uint32_t frame_conf : 1;  // [24]
        uint32_t __31_25 : 7;     // [31:25]
    } b;
} REG_CP_IDLE_IDLE_FRAME_NB_T;

// idle_framc_nb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t framc_cfsr : 17; // [16:0]
        uint32_t __31_17 : 15;    // [31:17]
    } b;
} REG_CP_IDLE_IDLE_FRAMC_NB_T;

// idl_awk_en_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awk_en_set : 13; // [12:0], write set
        uint32_t __31_13 : 19;    // [31:13]
    } b;
} REG_CP_IDLE_IDL_AWK_EN_SET_T;

// idl_awk_en_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t awk_en_clear : 13; // [12:0], write clear
        uint32_t __31_13 : 19;      // [31:13]
    } b;
} REG_CP_IDLE_IDL_AWK_EN_CLR_T;

// gsm_framc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t framc : 17;    // [16:0], read only
        uint32_t __19_17 : 3;   // [19:17]
        uint32_t rd_enable : 1; // [20]
        uint32_t __31_21 : 11;  // [31:21]
    } b;
} REG_CP_IDLE_GSM_FRAMC_T;

// nb_framc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t framc : 17;    // [16:0], read only
        uint32_t __19_17 : 3;   // [19:17]
        uint32_t rd_enable : 1; // [20]
        uint32_t __31_21 : 11;  // [31:21]
    } b;
} REG_CP_IDLE_NB_FRAMC_T;

// eliminat_jitter
typedef union {
    uint32_t v;
    struct
    {
        uint32_t elimiate_en : 8;   // [7:0]
        uint32_t eliminat_time : 8; // [15:8]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_CP_IDLE_ELIMINAT_JITTER_T;

// gsm_en_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t select : 1;  // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_IDLE_GSM_EN_SEL_T;

// nb_en_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t select : 1;  // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_IDLE_NB_EN_SEL_T;

// pd_pll_sw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bbpll1 : 1;   // [0]
        uint32_t bbpll2 : 1;   // [1]
        uint32_t apll : 1;     // [2]
        uint32_t audiopll : 1; // [3]
        uint32_t usbpll : 1;   // [4]
        uint32_t mempll : 1;   // [5]
        uint32_t dsipll : 1;   // [6]
        uint32_t __31_7 : 25;  // [31:7]
    } b;
} REG_CP_IDLE_PD_PLL_SW_T;

// pd_pll_sw_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pdpllswset : 7; // [6:0], write set
        uint32_t __31_7 : 25;    // [31:7]
    } b;
} REG_CP_IDLE_PD_PLL_SW_SET_T;

// pd_pll_sw_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pdpllswclr : 7; // [6:0], write clear
        uint32_t __31_7 : 25;    // [31:7]
    } b;
} REG_CP_IDLE_PD_PLL_SW_CLR_T;

// pd_pll_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bbpll1 : 1;   // [0]
        uint32_t bbpll2 : 1;   // [1]
        uint32_t apll : 1;     // [2]
        uint32_t audiopll : 1; // [3]
        uint32_t usbpll : 1;   // [4]
        uint32_t mempll : 1;   // [5]
        uint32_t dsipll : 1;   // [6]
        uint32_t __31_7 : 25;  // [31:7]
    } b;
} REG_CP_IDLE_PD_PLL_SEL_T;

// pd_pll_sel_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pdpllselset : 7; // [6:0], write set
        uint32_t __31_7 : 25;     // [31:7]
    } b;
} REG_CP_IDLE_PD_PLL_SEL_SET_T;

// pd_pll_sel_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pdpllselclr : 7; // [6:0], write clear
        uint32_t __31_7 : 25;     // [31:7]
    } b;
} REG_CP_IDLE_PD_PLL_SEL_CLR_T;

// idle_cg_sw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bbpll1 : 1;   // [0]
        uint32_t bbpll2 : 1;   // [1]
        uint32_t apll : 1;     // [2]
        uint32_t audiopll : 1; // [3]
        uint32_t usbpll : 1;   // [4]
        uint32_t mempll : 1;   // [5]
        uint32_t dsipll : 1;   // [6]
        uint32_t __31_7 : 25;  // [31:7]
    } b;
} REG_CP_IDLE_IDLE_CG_SW_T;

// idle_cg_sw_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idlecgswset : 7; // [6:0], write set
        uint32_t __31_7 : 25;     // [31:7]
    } b;
} REG_CP_IDLE_IDLE_CG_SW_SET_T;

// idle_cg_sw_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idlecgswclr : 7; // [6:0], write clear
        uint32_t __31_7 : 25;     // [31:7]
    } b;
} REG_CP_IDLE_IDLE_CG_SW_CLR_T;

// idle_cg_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bbpll1 : 1;   // [0]
        uint32_t bbpll2 : 1;   // [1]
        uint32_t apll : 1;     // [2]
        uint32_t audiopll : 1; // [3]
        uint32_t usbpll : 1;   // [4]
        uint32_t mempll : 1;   // [5]
        uint32_t dsipll : 1;   // [6]
        uint32_t __31_7 : 25;  // [31:7]
    } b;
} REG_CP_IDLE_IDLE_CG_SEL_T;

// idle_cg_sel_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idlecgselset : 7; // [6:0], write set
        uint32_t __31_7 : 25;      // [31:7]
    } b;
} REG_CP_IDLE_IDLE_CG_SEL_SET_T;

// idle_cg_sel_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t idlecgselclr : 7; // [6:0], write clear
        uint32_t __31_7 : 25;      // [31:7]
    } b;
} REG_CP_IDLE_IDLE_CG_SEL_CLR_T;

// rf_idle_enable_sw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfidleenablesw : 1; // [0]
        uint32_t __31_1 : 31;        // [31:1]
    } b;
} REG_CP_IDLE_RF_IDLE_ENABLE_SW_T;

// rf_idle_enable_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfidleenablesel : 1; // [0]
        uint32_t __31_1 : 31;         // [31:1]
    } b;
} REG_CP_IDLE_RF_IDLE_ENABLE_SEL_T;

// mem_ema_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rftpd_rmea : 1; // [0]
        uint32_t rftpd_rma : 4;  // [4:1]
        uint32_t rftpd_rmeb : 1; // [5]
        uint32_t rftpd_rmb : 4;  // [9:6]
        uint32_t __31_10 : 22;   // [31:10]
    } b;
} REG_CP_IDLE_MEM_EMA_CFG_T;

// uart_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_clk_uart : 1; // [0]
        uint32_t rst_ctrl_uart : 1;   // [1]
        uint32_t __31_2 : 30;         // [31:2]
    } b;
} REG_CP_IDLE_UART_CTRL_T;

// ddr_latch
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lpddr_latch : 1; // [0]
        uint32_t psram_latch : 1; // [1]
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_CP_IDLE_DDR_LATCH_T;

// pad_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_uart_1_rxd_wpus : 1;      // [0]
        uint32_t pad_uart_1_rxd_se : 1;        // [1]
        uint32_t pad_uart_1_rxd_pull_up : 1;   // [2]
        uint32_t pad_uart_1_rxd_pull_dowe : 1; // [3]
        uint32_t pad_uart_1_rxd_drv : 2;       // [5:4]
        uint32_t pad_uart_1_rxd_pull_frc : 1;  // [6]
        uint32_t pad_chip_pd_wpus : 1;         // [7]
        uint32_t pad_chip_pd_se : 1;           // [8]
        uint32_t pad_chip_pd_pull_up : 1;      // [9]
        uint32_t pad_chip_pd_pull_dowe : 1;    // [10]
        uint32_t pad_chip_pd_drv : 2;          // [12:11]
        uint32_t pad_chip_pd_pull_frc : 1;     // [13]
        uint32_t pad_chip_pd_out_frc : 1;      // [14]
        uint32_t pad_chip_pd_out : 1;          // [15]
        uint32_t pad_gpio_6_wpus : 1;          // [16]
        uint32_t pad_gpio_6_se : 1;            // [17]
        uint32_t pad_gpio_6_pull_up : 1;       // [18]
        uint32_t pad_gpio_6_pull_dowe : 1;     // [19]
        uint32_t pad_gpio_6_drv : 2;           // [21:20]
        uint32_t pad_gpio_6_pull_frc : 1;      // [22]
        uint32_t pad_osc_32k_wpus : 1;         // [23]
        uint32_t pad_osc_32k_se : 1;           // [24]
        uint32_t pad_osc_32k_drv : 2;          // [26:25]
        uint32_t pad_misc_idle_wpdi : 1;       // [27]
        uint32_t __31_28 : 4;                  // [31:28]
    } b;
} REG_CP_IDLE_PAD_CTRL_T;

// pad_ctrl_uart_txd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_uart_1_txd_wpus : 1;      // [0]
        uint32_t pad_uart_1_txd_se : 1;        // [1]
        uint32_t pad_uart_1_txd_pull_up : 1;   // [2]
        uint32_t pad_uart_1_txd_pull_dowe : 1; // [3]
        uint32_t pad_uart_1_txd_drv : 2;       // [5:4]
        uint32_t pad_uart_1_txd_pull_frc : 1;  // [6]
        uint32_t pad_uart_1_txd_out_frc : 1;   // [7]
        uint32_t pad_uart_1_txd_out : 1;       // [8]
        uint32_t __31_9 : 23;                  // [31:9]
    } b;
} REG_CP_IDLE_PAD_CTRL_UART_TXD_T;

// mon_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mon0_sel : 2;  // [1:0]
        uint32_t mon1_sel : 2;  // [3:2]
        uint32_t mon2_sel : 2;  // [5:4]
        uint32_t mon3_sel : 2;  // [7:6]
        uint32_t mon4_sel : 2;  // [9:8]
        uint32_t mon5_sel : 2;  // [11:10]
        uint32_t mon6_sel : 2;  // [13:12]
        uint32_t mon7_sel : 2;  // [15:14]
        uint32_t mon8_sel : 2;  // [17:16]
        uint32_t mon9_sel : 2;  // [19:18]
        uint32_t mon10_sel : 2; // [21:20]
        uint32_t mon11_sel : 2; // [23:22]
        uint32_t mon12_sel : 2; // [25:24]
        uint32_t mon13_sel : 2; // [27:26]
        uint32_t mon14_sel : 2; // [29:28]
        uint32_t mon15_sel : 2; // [31:30]
    } b;
} REG_CP_IDLE_MON_SEL_T;

// target_timer_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_target_timer : 1; // [0]
        uint32_t __31_1 : 31;              // [31:1]
    } b;
} REG_CP_IDLE_TARGET_TIMER_EN_T;

// target_timer_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t timer_stat_122m : 1; // [0], read only
        uint32_t timer_stat_32k : 1;  // [1], read only
        uint32_t __31_2 : 30;         // [31:2]
    } b;
} REG_CP_IDLE_TARGET_TIMER_STAT_T;

// slow_sys_clk_sel_hwen
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hwen : 1;    // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_IDLE_SLOW_SYS_CLK_SEL_HWEN_T;

// slow_clk_sel_hwen
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hwen : 1;    // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_IDLE_SLOW_CLK_SEL_HWEN_T;

// sleep_prot_time
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prot_time : 8; // [7:0]
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_CP_IDLE_SLEEP_PROT_TIME_T;

// idl_ctrl_sys1
#define CP_IDLE_IDCT_CTRL_SYS1 (1 << 0)

// idl_ctrl_sys2
#define CP_IDLE_IDCT_CTRL_SYS2 (1 << 0)

// idl_en
#define CP_IDLE_IDL_CP_EN (1 << 0)
#define CP_IDLE_IDL_AP_EN (1 << 1)

// idl_m_timer
#define CP_IDLE_IDCT_M_SYS(n) (((n)&0xffffffff) << 0)

// idl_wcn_en
#define CP_IDLE_WCN_CHIP_PD (1 << 0)
#define CP_IDLE_WCN_PD_XTAL (1 << 1)
#define CP_IDLE_WCN_PD_PLL (1 << 2)
#define CP_IDLE_WCN_IDLE_CG (1 << 3)
#define CP_IDLE_WCN_RES_VAL (1 << 4)

// idl_ctrl_timer
#define CP_IDLE_IDCT_CTRL_TIMER (1 << 0)

// idl_m2_sys
#define CP_IDLE_M2_SYS(n) (((n)&0xffff) << 0)
#define CP_IDLE_M1_SYS(n) (((n)&0xffff) << 16)

// idl_tc_start
#define CP_IDLE_TC_START_MOD(n) (((n)&0x3) << 0)

// idl_tc_end
#define CP_IDLE_TC_END_MOD(n) (((n)&0x3) << 0)
#define CP_IDLE_TC_END_FRAMC(n) (((n)&0x1ffff) << 4)

// idl_awk_timer
#define CP_IDLE_WAKE_TIMER (1 << 0)

// gsm_lp_pu_done
#define CP_IDLE_LP_PU_DONE (1 << 0)

// gsm_frame_inten
#define CP_IDLE_GSM_FRAME_IRQ_EN (1 << 0)

// gsm_frame_int_sta
#define CP_IDLE_GSM_FRAME_INT_STA (1 << 0)

// ltem1_frame_inten
#define CP_IDLE_LTEM1_FRAME1_IRQ_EN (1 << 0)
#define CP_IDLE_LTEM1_FRAME2_IRQ_EN (1 << 1)
#define CP_IDLE_LTEM1_FRAME3_IRQ_EN (1 << 2)

// ltem1_frame_int_sta
#define CP_IDLE_LTEM1_FRAME_INT_STA(n) (((n)&0x7) << 0)

// ltem2_frame_inten
#define CP_IDLE_LTEM2_FRAME1_IRQ_EN (1 << 0)
#define CP_IDLE_LTEM2_FRAME2_IRQ_EN (1 << 1)
#define CP_IDLE_LTEM2_FRAME3_IRQ_EN (1 << 2)

// ltem2_frame_int_sta
#define CP_IDLE_LTEM2_FRAME_INT_STA(n) (((n)&0x7) << 0)

// idl_sta
#define CP_IDLE_IDLE_SYS_STAT (1 << 0)
#define CP_IDLE_IDLE_GSM_TIMER_STAT (1 << 1)
#define CP_IDLE_IDLE_LTEM1_TIMER_STAT (1 << 2)
#define CP_IDLE_IDLE_LTEM2_TIMER_STAT (1 << 3)
#define CP_IDLE_H_STAT (1 << 4)
#define CP_IDLE_IDLE_NB_TIMER_STAT (1 << 5)

// idl_h_ctrl
#define CP_IDLE_H_CTRL_EN (1 << 1)
#define CP_IDLE_H_AUTO_EN (1 << 2)
#define CP_IDLE_H_RUN_TIME(n) (((n)&0xf) << 3)

// idl_h_val
#define CP_IDLE_H_VALUE(n) (((n)&0x7ffffff) << 0)

// idl_h_gsm
#define CP_IDLE_H_VALUE(n) (((n)&0x7ffffff) << 0)

// idl_h_ltem
#define CP_IDLE_H_VALUE(n) (((n)&0x7ffffff) << 0)

// idl_awk_en
#define CP_IDLE_AWK0_EN (1 << 0)
#define CP_IDLE_AWK1_EN (1 << 1)
#define CP_IDLE_AWK2_EN (1 << 2)
#define CP_IDLE_AWK3_EN (1 << 3)
#define CP_IDLE_AWK4_EN (1 << 4)
#define CP_IDLE_AWK5_EN (1 << 5)
#define CP_IDLE_AWK6_EN (1 << 6)
#define CP_IDLE_AWK7_EN (1 << 7)
#define CP_IDLE_AWK_OSW1_EN (1 << 8)
#define CP_IDLE_AWK_OSW2_EN (1 << 9)
#define CP_IDLE_AWK_SELF_EN (1 << 10)
#define CP_IDLE_GSM_LP_PU_REACH_EN (1 << 11)
#define CP_IDLE_NB_LP_PU_REACH_EN (1 << 12)

// idl_awk_st
#define CP_IDLE_AWK0_AWK_STAT (1 << 0)
#define CP_IDLE_AWK1_AWK_STAT (1 << 1)
#define CP_IDLE_AWK2_AWK_STAT (1 << 2)
#define CP_IDLE_AWK3_AWK_STAT (1 << 3)
#define CP_IDLE_AWK4_AWK_STAT (1 << 4)
#define CP_IDLE_AWK5_AWK_STAT (1 << 5)
#define CP_IDLE_AWK6_AWK_STAT (1 << 6)
#define CP_IDLE_AWK7_AWK_STAT (1 << 7)
#define CP_IDLE_AWK_OSW1_STAT (1 << 8)
#define CP_IDLE_AWK_OSW2_STAT (1 << 9)
#define CP_IDLE_AWK_SELF_STAT (1 << 10)
#define CP_IDLE_GSM_LP_PU_REACH_STAT (1 << 11)
#define CP_IDLE_NB_LP_PU_REACH_STAT (1 << 12)
#define CP_IDLE_AWK_UP_STAT (1 << 16)
#define CP_IDLE_IDLE_STAT (1 << 20)
#define CP_IDLE_POW_STA (1 << 24)
#define CP_IDLE_THR_STA (1 << 25)
#define CP_IDLE_POW_DFE_STA (1 << 26)

// idl_awk_self
#define CP_IDLE_WAKE_SELF (1 << 0)

// idl_osw1_en
#define CP_IDLE_OSW1_TIME(n) (((n)&0x7fffffff) << 0)
#define CP_IDLE_OSW1_EN (1 << 31)

// idl_osw1_cont
#define CP_IDLE_OSW1_COUNT(n) (((n)&0xffffffff) << 0)

// idl_fn_gsm
#define CP_IDLE_IDFN_GSM(n) (((n)&0xffffffff) << 0)

// idl_fn_ltem1
#define CP_IDLE_IDFN_SUB_LTEM(n) (((n)&0xf) << 0)
#define CP_IDLE_IDFN_RAD_LTEM(n) (((n)&0xfffffff) << 4)

// idl_fn_ltem2
#define CP_IDLE_IDFN_SUB_LTEM(n) (((n)&0xf) << 0)
#define CP_IDLE_IDFN_RAD_LTEM(n) (((n)&0xfffffff) << 4)

// idl_ltem_rfl
#define CP_IDLE_LTEM_IDLE_RADIOFRAME_PARAMETER(n) (((n)&0x1fffff) << 0)

// idl_ltem_sfl
#define CP_IDLE_LTEM_IDLE_FRAME_PARAMETER(n) (((n)&0x1ffff) << 0)

// idl_sig_en
#define CP_IDLE_CHIP_PD_EN (1 << 0)
#define CP_IDLE_PD_XTAL_EN (1 << 1)
#define CP_IDLE_PD_PLL_EN (1 << 2)
#define CP_IDLE_IDLE_CG_EN (1 << 3)

// idl_sig_timer
#define CP_IDLE_T1(n) (((n)&0xff) << 0)
#define CP_IDLE_T2(n) (((n)&0xff) << 8)
#define CP_IDLE_T3(n) (((n)&0xff) << 16)
#define CP_IDLE_T4(n) (((n)&0xff) << 24)

// idl_32k_ref
#define CP_IDLE_REK_REF(n) (((n)&0xffffffff) << 0)

// cp_inten
#define CP_IDLE_LAYOUT_IRQ (1 << 0)
#define CP_IDLE_IDLE_H_IRQ (1 << 1)
#define CP_IDLE_IDLE_FRAME_IRQ (1 << 2)
#define CP_IDLE_TSTAMP_IRQ (1 << 3)
#define CP_IDLE_OSW1_IRQ (1 << 4)
#define CP_IDLE_GSM_TC_START_IRQ (1 << 5)
#define CP_IDLE_GSM_TC_END_IRQ (1 << 6)
#define CP_IDLE_CP_INTEN_GSM_PU_REACH_IRQ (1 << 7)
#define CP_IDLE_CP_INTEN_TIMER_AWK_IRQ (1 << 8)
#define CP_IDLE_CP_INTEN_SYS_WAK_IRQ (1 << 9)
#define CP_IDLE_NB_TC_START_IRQ (1 << 10)
#define CP_IDLE_NB_TC_END_IRQ (1 << 11)
#define CP_IDLE_CP_INTEN_NB_PU_REACH_IRQ (1 << 12)
#define CP_IDLE_CP_INTEN_TARGET_IRQ (1 << 13)

// cp_inten_set
#define CP_IDLE_CP_INTEN_SET_INT_EN_SET(n) (((n)&0x3fff) << 0)

// cp_inten_clr
#define CP_IDLE_CP_INTEN_CLR_INT_EN_CLR(n) (((n)&0x3fff) << 0)

// cp_int_sta
#define CP_IDLE_CP_INT_STA(n) (((n)&0x3fff) << 0)

// ap_inten
#define CP_IDLE_OSW2_IRQ (1 << 0)
#define CP_IDLE_AP_INTEN_GSM_PU_REACH_IRQ (1 << 1)
#define CP_IDLE_AP_INTEN_TIMER_AWK_IRQ (1 << 2)
#define CP_IDLE_AP_INTEN_SYS_WAK_IRQ (1 << 3)
#define CP_IDLE_AP_INTEN_NB_PU_REACH_IRQ (1 << 4)
#define CP_IDLE_AP_INTEN_TARGET_IRQ (1 << 5)

// ap_inten_set
#define CP_IDLE_AP_INTEN_SET_INT_EN_SET(n) (((n)&0x3f) << 0)

// ap_inten_clr
#define CP_IDLE_AP_INTEN_CLR_INT_EN_CLR(n) (((n)&0x3f) << 0)

// ap_int_sta
#define CP_IDLE_AP_INT_STA(n) (((n)&0x3f) << 0)

// ltem1_cfsr_hfn
#define CP_IDLE_LTEM_CFSR_HFN(n) (((n)&0x3fffff) << 0)

// ltem1_cfsr_fn
#define CP_IDLE_LTEM_CFSR_SUB(n) (((n)&0xf) << 0)
#define CP_IDLE_LTEM_CFSR_RAD(n) (((n)&0x3ff) << 4)

// ltem1_cfsrs
#define CP_IDLE_LTEM_CFSRS(n) (((n)&0xffffff) << 0)
#define CP_IDLE_ADJUST_DIRECT (1 << 24)
#define CP_IDLE_LTEM1_CFSRS_ACTIVE_TIME (1 << 25)

// ltem1_cfsr_rdh
#define CP_IDLE_LTEM_CFSR_RDH(n) (((n)&0x3fffff) << 0)

// ltem1_cfsr_rdl
#define CP_IDLE_LTEM_CFSR_RDL_SUB(n) (((n)&0xf) << 0)
#define CP_IDLE_LTEM_CFSR_RDL_RAD(n) (((n)&0x3ff) << 4)

// ltem1_framc
#define CP_IDLE_LFRAMC(n) (((n)&0xffff) << 0)

// ltem1_framl
#define CP_IDLE_LFRAML(n) (((n)&0xffff) << 0)

// ltem1_framls
#define CP_IDLE_LFRAMLS(n) (((n)&0xffff) << 0)
#define CP_IDLE_LTEM1_FRAMLS_ACTIVE_TIME (1 << 16)

// ltem1_cfsr_tph
#define CP_IDLE_LTEM_CFSR_TPH(n) (((n)&0x3fffff) << 0)

// ltem1_cfsr_tpl
#define CP_IDLE_LTEM_CFSR_TPL(n) (((n)&0x3fff) << 0)

// ltem1_framc_tp
#define CP_IDLE_LTEM_FRAMC_TP(n) (((n)&0xffff) << 0)

// ltem2_cfsr_hfn
#define CP_IDLE_LTEM_CFSR_HFN(n) (((n)&0x3fffff) << 0)

// ltem2_cfsr_fn
#define CP_IDLE_LTEM_CFSR_SUB(n) (((n)&0xf) << 0)
#define CP_IDLE_LTEM_CFSR_RAD(n) (((n)&0x3ff) << 4)

// ltem2_cfsrs
#define CP_IDLE_LTEM_CFSRS(n) (((n)&0xffffff) << 0)
#define CP_IDLE_ADJUST_DIRECT (1 << 24)
#define CP_IDLE_LTEM2_CFSRS_ACTIVE_TIME (1 << 25)

// ltem2_cfsr_rdh
#define CP_IDLE_LTEM_CFSR_RDH(n) (((n)&0x3fffff) << 0)

// ltem2_cfsr_rdl
#define CP_IDLE_LTEM_CFSR_RDL_SUB(n) (((n)&0xf) << 0)
#define CP_IDLE_LTEM_CFSR_RDL_RAD(n) (((n)&0x3ff) << 4)

// ltem2_framc
#define CP_IDLE_LFRAMC(n) (((n)&0xffff) << 0)

// ltem2_framl
#define CP_IDLE_LFRAML(n) (((n)&0xffff) << 0)

// ltem2_framls
#define CP_IDLE_LFRAMLS(n) (((n)&0xffff) << 0)
#define CP_IDLE_LTEM2_FRAMLS_ACTIVE_TIME (1 << 16)

// ltem2_cfsr_tph
#define CP_IDLE_LTEM_CFSR_TPH(n) (((n)&0x3fffff) << 0)

// ltem2_cfsr_tpl
#define CP_IDLE_LTEM_CFSR_TPL(n) (((n)&0x3fff) << 0)

// ltem2_framc_tp
#define CP_IDLE_LTEM_FRAMC_TP(n) (((n)&0xffff) << 0)

// gsm_cfsr
#define CP_IDLE_GSM_CFSR_GSM_CFSR(n) (((n)&0xffffff) << 0)

// gsm_cfsrs
#define CP_IDLE_GSM_CFSRS_GSM_CFSRS(n) (((n)&0xffffff) << 0)
#define CP_IDLE_ADJUST_DIRECT (1 << 24)

// gsm_cfsro
#define CP_IDLE_GSM_CFSR_OVERFLOW(n) (((n)&0xffffff) << 0)

// ltem1_fhl
#define CP_IDLE_LTEM_FHL(n) (((n)&0x3fffff) << 0)

// ltem1_fll
#define CP_IDLE_LTEM_FLL(n) (((n)&0x3fff) << 0)

// ltem1_fcl
#define CP_IDLE_LTEM_FCL(n) (((n)&0xffff) << 0)

// ltem2_fhl
#define CP_IDLE_LTEM_FHL(n) (((n)&0x3fffff) << 0)

// ltem2_fll
#define CP_IDLE_LTEM_FLL(n) (((n)&0x3fff) << 0)

// ltem2_fcl
#define CP_IDLE_LTEM_FCL(n) (((n)&0xffff) << 0)

// gsm_fl
#define CP_IDLE_FL(n) (((n)&0xffffff) << 0)

// gsm_fcl
#define CP_IDLE_GSM_FCL_GSM_FCL(n) (((n)&0x1ffff) << 0)

// tpctrl
#define CP_IDLE_INNER_CTRL (1 << 0)
#define CP_IDLE_INNER_CONFG(n) (((n)&0x3) << 4)
#define CP_IDLE_MOD_SEL(n) (((n)&0x7) << 8)

// layoutt
#define CP_IDLE_LAYOUTT(n) (((n)&0xffffffff) << 0)

// layoutctrl
#define CP_IDLE_ENABLE (1 << 0)
#define CP_IDLE_TIMER_SELECT (1 << 1)
#define CP_IDLE_CHIP_COUNT(n) (((n)&0x7fff) << 8)

// ltem1_fint_dly1
#define CP_IDLE_DELAY_TIME(n) (((n)&0xffff) << 0)

// ltem1_fint_dly2
#define CP_IDLE_DELAY_TIME(n) (((n)&0xffff) << 0)

// ltem2_fint_dly1
#define CP_IDLE_DELAY_TIME(n) (((n)&0xffff) << 0)

// ltem2_fint_dly2
#define CP_IDLE_DELAY_TIME(n) (((n)&0xffff) << 0)

// fint_en
#define CP_IDLE_LTE_M1_FINT_ENABLE(n) (((n)&0x3ff) << 0)
#define CP_IDLE_LTE_M2_FINT_ENABLE(n) (((n)&0x3ff) << 12)

// timer_en
#define CP_IDLE_LTE_M1_TIMER_ENABLE (1 << 0)
#define CP_IDLE_LTE_M2_TIMER_ENABLE (1 << 1)
#define CP_IDLE_GSM_TIMER_ENABLE (1 << 2)
#define CP_IDLE_LTEM_TIMER_ENABLE (1 << 3)
#define CP_IDLE_NB_TIMER_ENABLE (1 << 4)

// idle_frame_sta
#define CP_IDLE_LTE_M1_FRAME_STATE (1 << 0)
#define CP_IDLE_LTE_M2_FRAME_STATE (1 << 1)
#define CP_IDLE_GSM_FRAME_STATE (1 << 2)
#define CP_IDLE_LTE_M_FRAME_STATE (1 << 3)
#define CP_IDLE_NB_FRAME_STATE (1 << 4)

// idle_frame_ltem1
#define CP_IDLE_IDLE_FRAME_LTEM1_FRAME_CFSR(n) (((n)&0x3fffff) << 0)
#define CP_IDLE_FRAME_CONF (1 << 24)

// idle_frame_ltem2
#define CP_IDLE_IDLE_FRAME_LTEM2_FRAME_CFSR(n) (((n)&0x3fffff) << 0)
#define CP_IDLE_FRAME_CONF (1 << 24)

// idle_frame_gsm
#define CP_IDLE_IDLE_FRAME_GSM_FRAME_CFSR(n) (((n)&0xffffff) << 0)
#define CP_IDLE_FRAME_CONF (1 << 24)

// idle_frame_lte
#define CP_IDLE_FRAME_REF_CFSR(n) (((n)&0xffffffff) << 0)

// idle_frame_lte_conf
#define CP_IDLE_FRAME_REF_LTE_CONF (1 << 0)
#define CP_IDLE_RESRVED(n) (((n)&0x7fffffff) << 1)

// ltem_ref_fn
#define CP_IDLE_LTEM_REF_FN_LTEM_REF_FN(n) (((n)&0xffffffff) << 0)

// ltem_ref_fnl
#define CP_IDLE_REF_LTEM_FNL(n) (((n)&0xffffffff) << 0)

// ltem_ref_fcl
#define CP_IDLE_REF_LTEM_FCL(n) (((n)&0x7fff) << 0)

// ref_32k_fnl
#define CP_IDLE_REF_32K_FNL_REF_32K_FNL(n) (((n)&0xffffffff) << 0)

// ltem_ref_fc
#define CP_IDLE_LTEM_REF_FC_LTEM_REF_FC(n) (((n)&0x7fff) << 0)

// gsm_framl
#define CP_IDLE_GSM_FRAML(n) (((n)&0x1ffff) << 0)

// idl_osw2_en
#define CP_IDLE_OSW2_TIME(n) (((n)&0x7fffffff) << 0)
#define CP_IDLE_OSW2_EN (1 << 31)

// idl_osw2_cont
#define CP_IDLE_OSW2_COUNT(n) (((n)&0xffffffff) << 0)

// idle_framc_gsm
#define CP_IDLE_FRAMC_CFSR(n) (((n)&0x1ffff) << 0)

// ltem1_fint_dly3
#define CP_IDLE_DELAY_TIME(n) (((n)&0xffff) << 0)

// ltem2_fint_dly3
#define CP_IDLE_DELAY_TIME(n) (((n)&0xffff) << 0)

// idle_time_sel
#define CP_IDLE_TIME_SEL (1 << 0)

// idle_time
#define CP_IDLE_IDL_TIME(n) (((n)&0xffffffff) << 0)

// idl_h_gsm_lp
#define CP_IDLE_H_VALUE(n) (((n)&0x7ffffff) << 0)

// idl_h_ltem_lp
#define CP_IDLE_H_VALUE(n) (((n)&0x7ffffff) << 0)

// idl_tc_start_nb
#define CP_IDLE_TC_START_MOD(n) (((n)&0x3) << 0)

// idl_tc_end_nb
#define CP_IDLE_TC_END_MOD(n) (((n)&0x3) << 0)
#define CP_IDLE_TC_END_FRAMC(n) (((n)&0x1ffff) << 4)

// nb_lp_pu_done
#define CP_IDLE_LP_PU_DONE (1 << 0)

// idl_h_nb
#define CP_IDLE_H_VALUE(n) (((n)&0x7ffffff) << 0)

// idl_h_nb_lp
#define CP_IDLE_H_VALUE(n) (((n)&0x7ffffff) << 0)

// idl_fn_nb
#define CP_IDLE_IDFN_NB(n) (((n)&0xffffffff) << 0)

// nb_frame_inten
#define CP_IDLE_NB_FRAME_IRQ_EN (1 << 0)

// nb_frame_int_sta
#define CP_IDLE_GSM_FRAME_INT_STA (1 << 0)

// nb_cfsr
#define CP_IDLE_GSM_CFSR(n) (((n)&0xffffff) << 0)

// nb_framl
#define CP_IDLE_NB_FRAML(n) (((n)&0x1ffff) << 0)

// nb_cfsrs
#define CP_IDLE_NB_CFSRS_NB_CFSRS(n) (((n)&0xffffff) << 0)
#define CP_IDLE_ADJUST_DIRECT (1 << 24)

// nb_cfsro
#define CP_IDLE_NB_CFSR_OVERFLOW(n) (((n)&0xffffff) << 0)

// nb_fl
#define CP_IDLE_FL(n) (((n)&0xffffff) << 0)

// nb_fcl
#define CP_IDLE_NB_FCL_NB_FCL(n) (((n)&0x1ffff) << 0)

// idle_frame_nb
#define CP_IDLE_IDLE_FRAME_NB_FRAME_CFSR(n) (((n)&0xffffff) << 0)
#define CP_IDLE_FRAME_CONF (1 << 24)

// idle_framc_nb
#define CP_IDLE_FRAMC_CFSR(n) (((n)&0x1ffff) << 0)

// idl_awk_en_set
#define CP_IDLE_AWK_EN_SET(n) (((n)&0x1fff) << 0)

// idl_awk_en_clr
#define CP_IDLE_AWK_EN_CLEAR(n) (((n)&0x1fff) << 0)

// gsm_framc
#define CP_IDLE_FRAMC(n) (((n)&0x1ffff) << 0)
#define CP_IDLE_RD_ENABLE (1 << 20)

// nb_framc
#define CP_IDLE_FRAMC(n) (((n)&0x1ffff) << 0)
#define CP_IDLE_RD_ENABLE (1 << 20)

// eliminat_jitter
#define CP_IDLE_ELIMIATE_EN(n) (((n)&0xff) << 0)
#define CP_IDLE_ELIMINAT_TIME(n) (((n)&0xff) << 8)

// gsm_en_sel
#define CP_IDLE_SELECT (1 << 0)

// nb_en_sel
#define CP_IDLE_SELECT (1 << 0)

// pd_pll_sw
#define CP_IDLE_BBPLL1 (1 << 0)
#define CP_IDLE_BBPLL2 (1 << 1)
#define CP_IDLE_APLL (1 << 2)
#define CP_IDLE_AUDIOPLL (1 << 3)
#define CP_IDLE_USBPLL (1 << 4)
#define CP_IDLE_MEMPLL (1 << 5)
#define CP_IDLE_DSIPLL (1 << 6)

// pd_pll_sw_set
#define CP_IDLE_PDPLLSWSET(n) (((n)&0x7f) << 0)

// pd_pll_sw_clr
#define CP_IDLE_PDPLLSWCLR(n) (((n)&0x7f) << 0)

// pd_pll_sel
#define CP_IDLE_BBPLL1 (1 << 0)
#define CP_IDLE_BBPLL2 (1 << 1)
#define CP_IDLE_APLL (1 << 2)
#define CP_IDLE_AUDIOPLL (1 << 3)
#define CP_IDLE_USBPLL (1 << 4)
#define CP_IDLE_MEMPLL (1 << 5)
#define CP_IDLE_DSIPLL (1 << 6)

// pd_pll_sel_set
#define CP_IDLE_PDPLLSELSET(n) (((n)&0x7f) << 0)

// pd_pll_sel_clr
#define CP_IDLE_PDPLLSELCLR(n) (((n)&0x7f) << 0)

// idle_cg_sw
#define CP_IDLE_BBPLL1 (1 << 0)
#define CP_IDLE_BBPLL2 (1 << 1)
#define CP_IDLE_APLL (1 << 2)
#define CP_IDLE_AUDIOPLL (1 << 3)
#define CP_IDLE_USBPLL (1 << 4)
#define CP_IDLE_MEMPLL (1 << 5)
#define CP_IDLE_DSIPLL (1 << 6)

// idle_cg_sw_set
#define CP_IDLE_IDLECGSWSET(n) (((n)&0x7f) << 0)

// idle_cg_sw_clr
#define CP_IDLE_IDLECGSWCLR(n) (((n)&0x7f) << 0)

// idle_cg_sel
#define CP_IDLE_BBPLL1 (1 << 0)
#define CP_IDLE_BBPLL2 (1 << 1)
#define CP_IDLE_APLL (1 << 2)
#define CP_IDLE_AUDIOPLL (1 << 3)
#define CP_IDLE_USBPLL (1 << 4)
#define CP_IDLE_MEMPLL (1 << 5)
#define CP_IDLE_DSIPLL (1 << 6)

// idle_cg_sel_set
#define CP_IDLE_IDLECGSELSET(n) (((n)&0x7f) << 0)

// idle_cg_sel_clr
#define CP_IDLE_IDLECGSELCLR(n) (((n)&0x7f) << 0)

// rf_idle_enable_sw
#define CP_IDLE_RFIDLEENABLESW (1 << 0)

// rf_idle_enable_sel
#define CP_IDLE_RFIDLEENABLESEL (1 << 0)

// mem_ema_cfg
#define CP_IDLE_RFTPD_RMEA (1 << 0)
#define CP_IDLE_RFTPD_RMA(n) (((n)&0xf) << 1)
#define CP_IDLE_RFTPD_RMEB (1 << 5)
#define CP_IDLE_RFTPD_RMB(n) (((n)&0xf) << 6)

// uart_ctrl
#define CP_IDLE_ENABLE_CLK_UART (1 << 0)
#define CP_IDLE_RST_CTRL_UART (1 << 1)

// ddr_latch
#define CP_IDLE_LPDDR_LATCH (1 << 0)
#define CP_IDLE_PSRAM_LATCH (1 << 1)

// pad_ctrl
#define CP_IDLE_PAD_UART_1_RXD_WPUS (1 << 0)
#define CP_IDLE_PAD_UART_1_RXD_SE (1 << 1)
#define CP_IDLE_PAD_UART_1_RXD_PULL_UP (1 << 2)
#define CP_IDLE_PAD_UART_1_RXD_PULL_DOWE (1 << 3)
#define CP_IDLE_PAD_UART_1_RXD_DRV(n) (((n)&0x3) << 4)
#define CP_IDLE_PAD_UART_1_RXD_PULL_FRC (1 << 6)
#define CP_IDLE_PAD_CHIP_PD_WPUS (1 << 7)
#define CP_IDLE_PAD_CHIP_PD_SE (1 << 8)
#define CP_IDLE_PAD_CHIP_PD_PULL_UP (1 << 9)
#define CP_IDLE_PAD_CHIP_PD_PULL_DOWE (1 << 10)
#define CP_IDLE_PAD_CHIP_PD_DRV(n) (((n)&0x3) << 11)
#define CP_IDLE_PAD_CHIP_PD_PULL_FRC (1 << 13)
#define CP_IDLE_PAD_CHIP_PD_OUT_FRC (1 << 14)
#define CP_IDLE_PAD_CHIP_PD_OUT (1 << 15)
#define CP_IDLE_PAD_GPIO_6_WPUS (1 << 16)
#define CP_IDLE_PAD_GPIO_6_SE (1 << 17)
#define CP_IDLE_PAD_GPIO_6_PULL_UP (1 << 18)
#define CP_IDLE_PAD_GPIO_6_PULL_DOWE (1 << 19)
#define CP_IDLE_PAD_GPIO_6_DRV(n) (((n)&0x3) << 20)
#define CP_IDLE_PAD_GPIO_6_PULL_FRC (1 << 22)
#define CP_IDLE_PAD_OSC_32K_WPUS (1 << 23)
#define CP_IDLE_PAD_OSC_32K_SE (1 << 24)
#define CP_IDLE_PAD_OSC_32K_DRV(n) (((n)&0x3) << 25)
#define CP_IDLE_PAD_MISC_IDLE_WPDI (1 << 27)

// pad_ctrl_uart_txd
#define CP_IDLE_PAD_UART_1_TXD_WPUS (1 << 0)
#define CP_IDLE_PAD_UART_1_TXD_SE (1 << 1)
#define CP_IDLE_PAD_UART_1_TXD_PULL_UP (1 << 2)
#define CP_IDLE_PAD_UART_1_TXD_PULL_DOWE (1 << 3)
#define CP_IDLE_PAD_UART_1_TXD_DRV(n) (((n)&0x3) << 4)
#define CP_IDLE_PAD_UART_1_TXD_PULL_FRC (1 << 6)
#define CP_IDLE_PAD_UART_1_TXD_OUT_FRC (1 << 7)
#define CP_IDLE_PAD_UART_1_TXD_OUT (1 << 8)

// mon_sel
#define CP_IDLE_MON0_SEL(n) (((n)&0x3) << 0)
#define CP_IDLE_MON1_SEL(n) (((n)&0x3) << 2)
#define CP_IDLE_MON2_SEL(n) (((n)&0x3) << 4)
#define CP_IDLE_MON3_SEL(n) (((n)&0x3) << 6)
#define CP_IDLE_MON4_SEL(n) (((n)&0x3) << 8)
#define CP_IDLE_MON5_SEL(n) (((n)&0x3) << 10)
#define CP_IDLE_MON6_SEL(n) (((n)&0x3) << 12)
#define CP_IDLE_MON7_SEL(n) (((n)&0x3) << 14)
#define CP_IDLE_MON8_SEL(n) (((n)&0x3) << 16)
#define CP_IDLE_MON9_SEL(n) (((n)&0x3) << 18)
#define CP_IDLE_MON10_SEL(n) (((n)&0x3) << 20)
#define CP_IDLE_MON11_SEL(n) (((n)&0x3) << 22)
#define CP_IDLE_MON12_SEL(n) (((n)&0x3) << 24)
#define CP_IDLE_MON13_SEL(n) (((n)&0x3) << 26)
#define CP_IDLE_MON14_SEL(n) (((n)&0x3) << 28)
#define CP_IDLE_MON15_SEL(n) (((n)&0x3) << 30)

// mon_sel_set
#define CP_IDLE_MON_SEL_SET(n) (((n)&0xffffffff) << 0)

// mon_sel_clr
#define CP_IDLE_MON_SEL_CLR(n) (((n)&0xffffffff) << 0)

// target_timer
#define CP_IDLE_TARGET_TIME(n) (((n)&0xffffffff) << 0)

// target_timer_en
#define CP_IDLE_DISABLE_TARGET_TIMER (1 << 0)

// target_value_lock
#define CP_IDLE_LOCK_VALUE(n) (((n)&0xffffffff) << 0)

// target_timer_stat
#define CP_IDLE_TIMER_STAT_122M (1 << 0)
#define CP_IDLE_TIMER_STAT_32K (1 << 1)

// slow_sys_clk_sel_hwen
#define CP_IDLE_HWEN (1 << 0)

// slow_clk_sel_hwen
#define CP_IDLE_HWEN (1 << 0)

// sleep_prot_time
#define CP_IDLE_PROT_TIME(n) (((n)&0xff) << 0)

#endif // _CP_IDLE_H_
