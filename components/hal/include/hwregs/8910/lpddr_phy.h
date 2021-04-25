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

#ifndef _LPDDR_PHY_H_
#define _LPDDR_PHY_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_LPDDR_PHY_BASE (0x08801000)

typedef volatile struct
{
    uint32_t lpddr_rf_cfg_phy;                  // 0x00000000
    uint32_t lpddr_rf_cfg_clock_gate;           // 0x00000004
    uint32_t lpddr_rf_cfg_sample_resync;        // 0x00000008
    uint32_t lpddr_rf_cfg_lpi;                  // 0x0000000c
    uint32_t __16[60];                          // 0x00000010
    uint32_t lpddr_rf_cfg_ext;                  // 0x00000100
    uint32_t __260[63];                         // 0x00000104
    uint32_t lpddr_rfdll_cfg_dll;               // 0x00000200
    uint32_t lpddr_rfdll_status_cpst_idle;      // 0x00000204
    uint32_t lpddr_rf_cfg_dll_ac;               // 0x00000208
    uint32_t lpddr_rfdll_status_dll_ac;         // 0x0000020c
    uint32_t lpddr_rf_cfg_dll_dl_0_wr_ac;       // 0x00000210
    uint32_t lpddr_rf_cfg_dll_dl_1_wr_ac;       // 0x00000214
    uint32_t lpddr_rf_cfg_dll_dl_2_wr_ac;       // 0x00000218
    uint32_t lpddr_rf_cfg_dll_dl_3_wr_ac;       // 0x0000021c
    uint32_t lpddr_rf_cfg_dll_dl_4_wr_ac;       // 0x00000220
    uint32_t lpddr_rfdll_status_max_cnt_ac;     // 0x00000224
    uint32_t lpddr_rfdll_status_min_cnt_ac;     // 0x00000228
    uint32_t lpddr_rf_cfg_phy_iomux_sel_wr_ac;  // 0x0000022c
    uint32_t lpddr_rf_cfg_phy_iomux_ie_wr_ac;   // 0x00000230
    uint32_t lpddr_rf_cfg_phy_iomux_oe_wr_ac;   // 0x00000234
    uint32_t lpddr_rf_cfg_phy_iomux_out_wr_ac;  // 0x00000238
    uint32_t lpddr_rf_cfg_dll_ds0;              // 0x0000023c
    uint32_t lpddr_rfdll_status_dll_ds0;        // 0x00000240
    uint32_t lpddr_rf_cfg_dll_dl_0_wr_ds0;      // 0x00000244
    uint32_t lpddr_rf_cfg_dll_dl_1_wr_ds0;      // 0x00000248
    uint32_t lpddr_rf_cfg_dll_dl_2_wr_ds0;      // 0x0000024c
    uint32_t lpddr_rf_cfg_dll_dl_3_wr_ds0;      // 0x00000250
    uint32_t lpddr_rf_cfg_dll_dl_4_wr_ds0;      // 0x00000254
    uint32_t lpddr_rf_cfg_dll_dl_5_wr_ds0;      // 0x00000258
    uint32_t lpddr_rf_cfg_dll_dl_6_wr_ds0;      // 0x0000025c
    uint32_t lpddr_rf_cfg_dll_dl_7_wr_ds0;      // 0x00000260
    uint32_t lpddr_rf_cfg_dll_dl_8_wr_ds0;      // 0x00000264
    uint32_t lpddr_rfdll_status_max_cnt_ds0;    // 0x00000268
    uint32_t lpddr_rfdll_status_min_cnt_ds0;    // 0x0000026c
    uint32_t lpddr_rf_cfg_phy_iomux_sel_wr_ds0; // 0x00000270
    uint32_t lpddr_rf_cfg_phy_iomux_ie_wr_ds0;  // 0x00000274
    uint32_t lpddr_rf_cfg_phy_iomux_oe_wr_ds0;  // 0x00000278
    uint32_t lpddr_rf_cfg_phy_iomux_out_wr_ds0; // 0x0000027c
    uint32_t lpddr_rf_cfg_dll_ds1;              // 0x00000280
    uint32_t lpddr_rfdll_status_dll_ds1;        // 0x00000284
    uint32_t lpddr_rf_cfg_dll_dl_0_wr_ds1;      // 0x00000288
    uint32_t lpddr_rf_cfg_dll_dl_1_wr_ds1;      // 0x0000028c
    uint32_t lpddr_rf_cfg_dll_dl_2_wr_ds1;      // 0x00000290
    uint32_t lpddr_rf_cfg_dll_dl_3_wr_ds1;      // 0x00000294
    uint32_t lpddr_rf_cfg_dll_dl_4_wr_ds1;      // 0x00000298
    uint32_t lpddr_rf_cfg_dll_dl_5_wr_ds1;      // 0x0000029c
    uint32_t lpddr_rf_cfg_dll_dl_6_wr_ds1;      // 0x000002a0
    uint32_t lpddr_rf_cfg_dll_dl_7_wr_ds1;      // 0x000002a4
    uint32_t lpddr_rf_cfg_dll_dl_8_wr_ds1;      // 0x000002a8
    uint32_t lpddr_rfdll_status_max_cnt_ds1;    // 0x000002ac
    uint32_t lpddr_rfdll_status_min_cnt_ds1;    // 0x000002b0
    uint32_t lpddr_rf_cfg_phy_iomux_sel_wr_ds1; // 0x000002b4
    uint32_t lpddr_rf_cfg_phy_iomux_ie_wr_ds1;  // 0x000002b8
    uint32_t lpddr_rf_cfg_phy_iomux_oe_wr_ds1;  // 0x000002bc
    uint32_t lpddr_rf_cfg_phy_iomux_out_wr_ds1; // 0x000002c0
    uint32_t lpddr_rf_status_phy_data_in;       // 0x000002c4
    uint32_t __712[78];                         // 0x000002c8
    uint32_t lpddr_drf_cfg;                     // 0x00000400
    uint32_t lpddr_drf_cfg_reg_sel;             // 0x00000404
    uint32_t lpddr_drf_cfg_dqs_ie_sel_f0;       // 0x00000408
    uint32_t lpddr_drf_cfg_dqs_oe_sel_f0;       // 0x0000040c
    uint32_t lpddr_drf_cfg_dqs_out_sel_f0;      // 0x00000410
    uint32_t lpddr_drf_cfg_dqs_gate_sel_f0;     // 0x00000414
    uint32_t lpddr_drf_cfg_data_ie_sel_f0;      // 0x00000418
    uint32_t lpddr_drf_cfg_data_oe_sel_f0;      // 0x0000041c
    uint32_t lpddr_drf_cfg_read_en_sel_f0;      // 0x00000420
    uint32_t lpddr_drf_cfg_dqs_ie_sel_f1;       // 0x00000424
    uint32_t lpddr_drf_cfg_dqs_oe_sel_f1;       // 0x00000428
    uint32_t lpddr_drf_cfg_dqs_out_sel_f1;      // 0x0000042c
    uint32_t lpddr_drf_cfg_dqs_gate_sel_f1;     // 0x00000430
    uint32_t lpddr_drf_cfg_data_ie_sel_f1;      // 0x00000434
    uint32_t lpddr_drf_cfg_data_oe_sel_f1;      // 0x00000438
    uint32_t lpddr_drf_cfg_read_en_sel_f1;      // 0x0000043c
    uint32_t lpddr_drf_cfg_dqs_ie_sel_f2;       // 0x00000440
    uint32_t lpddr_drf_cfg_dqs_oe_sel_f2;       // 0x00000444
    uint32_t lpddr_drf_cfg_dqs_out_sel_f2;      // 0x00000448
    uint32_t lpddr_drf_cfg_dqs_gate_sel_f2;     // 0x0000044c
    uint32_t lpddr_drf_cfg_data_ie_sel_f2;      // 0x00000450
    uint32_t lpddr_drf_cfg_data_oe_sel_f2;      // 0x00000454
    uint32_t lpddr_drf_cfg_read_en_sel_f2;      // 0x00000458
    uint32_t lpddr_drf_cfg_dqs_ie_sel_f3;       // 0x0000045c
    uint32_t lpddr_drf_cfg_dqs_oe_sel_f3;       // 0x00000460
    uint32_t lpddr_drf_cfg_dqs_out_sel_f3;      // 0x00000464
    uint32_t lpddr_drf_cfg_dqs_gate_sel_f3;     // 0x00000468
    uint32_t lpddr_drf_cfg_data_ie_sel_f3;      // 0x0000046c
    uint32_t lpddr_drf_cfg_data_oe_sel_f3;      // 0x00000470
    uint32_t lpddr_drf_cfg_read_en_sel_f3;      // 0x00000474
    uint32_t lpddr_drf_cfg_dll_mode_f0;         // 0x00000478
    uint32_t lpddr_drf_cfg_dll_cnt_f0;          // 0x0000047c
    uint32_t lpddr_drf_cfg_dll_mode_f1;         // 0x00000480
    uint32_t lpddr_drf_cfg_dll_cnt_f1;          // 0x00000484
    uint32_t lpddr_drf_cfg_dll_mode_f2;         // 0x00000488
    uint32_t lpddr_drf_cfg_dll_cnt_f2;          // 0x0000048c
    uint32_t lpddr_drf_cfg_dll_mode_f3;         // 0x00000490
    uint32_t lpddr_drf_cfg_dll_cnt_f3;          // 0x00000494
    uint32_t __1176[26];                        // 0x00000498
    uint32_t lpddr_drf_train_cfg;               // 0x00000500
    uint32_t lpddr_drf_mr_data_0;               // 0x00000504
    uint32_t lpddr_drf_mr_data_1;               // 0x00000508
    uint32_t __1292[61];                        // 0x0000050c
    uint32_t lpddr_rf_irq_ctrl;                 // 0x00000600
    uint32_t lpddr_rf_irq_status_clr;           // 0x00000604
    uint32_t lpddr_rf_irq_status;               // 0x00000608
    uint32_t lpddr_rf_irq_cnt_clr;              // 0x0000060c
    uint32_t lpddr_rf_irq_cnt_dll_unlock_ac;    // 0x00000610
    uint32_t lpddr_rf_irq_cnt_dll_unlock_ds0;   // 0x00000614
    uint32_t lpddr_rf_irq_cnt_dll_unlock_ds1;   // 0x00000618
    uint32_t __1564[57];                        // 0x0000061c
    uint32_t io_rf_ddr_rpull_cfg;               // 0x00000700
    uint32_t io_rf_ddr_drv_cfg;                 // 0x00000704
    uint32_t io_rf_ddr_pad_cfg;                 // 0x00000708
    uint32_t io_rf_ddr_vref_cfg;                // 0x0000070c
    uint32_t io_rf_ddr_zq_cfg;                  // 0x00000710
    uint32_t io_rf_ddr_zq_mon;                  // 0x00000714
    uint32_t io_rf_ddr_odt_cfg;                 // 0x00000718
    uint32_t io_rf_ddr_reserved;                // 0x0000071c
    uint32_t io_rf_psram_drv_cfg;               // 0x00000720
    uint32_t io_rf_psram_pad_en_cfg;            // 0x00000724
    uint32_t io_rf_psram_pull_cfg;              // 0x00000728
    uint32_t io_rf_psram_reserved;              // 0x0000072c
} HWP_LPDDR_PHY_T;

#define hwp_lpddrPhy ((HWP_LPDDR_PHY_T *)REG_ACCESS_ADDRESS(REG_LPDDR_PHY_BASE))

// lpddr_rf_cfg_phy
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_en : 1;            // [0]
        uint32_t rf_phy_init_complete : 1; // [1]
        uint32_t __31_2 : 30;              // [31:2]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_T;

// lpddr_rf_cfg_clock_gate
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_clk_gate_en : 1;       // [0]
        uint32_t rf_clk_gate_fg_en : 1;    // [1]
        uint32_t rf_clk_gate_ag_en : 1;    // [2]
        uint32_t rf_clk_gate_ag_wr_en : 1; // [3]
        uint32_t rf_clk_gate_ag_rd_en : 1; // [4]
        uint32_t __31_5 : 27;              // [31:5]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_CLOCK_GATE_T;

// lpddr_rf_cfg_sample_resync
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_sample_resync : 1;         // [0]
        uint32_t rf_sample_resync_auto_en : 1; // [1]
        uint32_t __15_2 : 14;                  // [15:2]
        uint32_t rf_data_resync_sel : 16;      // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_SAMPLE_RESYNC_T;

// lpddr_rf_cfg_lpi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_lpi_sel_m0 : 1; // [0]
        uint32_t rf_cwakeup_m0 : 1; // [1]
        uint32_t rf_cwakeup_s0 : 1; // [2]
        uint32_t __31_3 : 29;       // [31:3]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_LPI_T;

// lpddr_rf_cfg_ext
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_data_oe_ext : 4;     // [3:0]
        uint32_t rf_data_ie_ext : 4;     // [7:4]
        uint32_t rf_dqs_oe_ext : 4;      // [11:8]
        uint32_t rf_dqs_ie_ext : 4;      // [15:12]
        uint32_t rf_dqs_gate_ext : 4;    // [19:16]
        uint32_t rf_data_resync_ext : 4; // [23:20]
        uint32_t __31_24 : 8;            // [31:24]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_EXT_T;

// lpddr_rfdll_cfg_dll
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdll_reset : 1; // [0]
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_CFG_DLL_T;

// lpddr_rfdll_status_cpst_idle
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdl_cpst_st_idle_ac : 1;  // [0], read only
        uint32_t rfdl_cpst_st_idle_ds0 : 1; // [1], read only
        uint32_t rfdl_cpst_st_idle_ds1 : 1; // [2], read only
        uint32_t __31_3 : 29;               // [31:3]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_CPST_IDLE_T;

// lpddr_rf_cfg_dll_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __7_0 : 8;                     // [7:0]
        uint32_t rf_dll_clr_ac : 1;             // [8]
        uint32_t rf_dll_auto_clr_en_ac : 1;     // [9]
        uint32_t rf_dl_cpst_en_ac : 1;          // [10]
        uint32_t rf_dl_cpst_start_ac : 1;       // [11]
        uint32_t rf_dl_cpst_auto_ref_en_ac : 1; // [12]
        uint32_t rf_dll_err_clr_ac : 1;         // [13]
        uint32_t rf_dll_clk_sel_ac : 1;         // [14]
        uint32_t rf_dll_en_ac : 1;              // [15]
        uint32_t rf_dl_cpst_thr_ac : 8;         // [23:16]
        uint32_t rf_dll_pd_cnt_ac : 3;          // [26:24]
        uint32_t rf_dll_auto_err_clr_en_ac : 1; // [27]
        uint32_t rf_dll_lock_wait_ac : 4;       // [31:28]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_AC_T;

// lpddr_rfdll_status_dll_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdll_cnt_ac : 7;    // [6:0], read only
        uint32_t __23_7 : 17;         // [23:7]
        uint32_t rfdl_cpst_st_ac : 1; // [24], read only
        uint32_t rfdll_st_ac : 3;     // [27:25], read only
        uint32_t rfdll_locked_ac : 1; // [28], read only
        uint32_t rfdll_error_ac : 1;  // [29], read only
        uint32_t __31_30 : 2;         // [31:30]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_DLL_AC_T;

// lpddr_rf_cfg_dll_dl_0_wr_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_clkwr_raw_dl_sel_ac : 7;         // [6:0]
        uint32_t __7_7 : 1;                          // [7]
        uint32_t rfdl_clkwr_raw_cnt_ac : 7;          // [14:8], read only
        uint32_t __15_15 : 1;                        // [15]
        uint32_t rf_clkwr_raw_dl_cpst_offset_ac : 7; // [22:16]
        uint32_t __23_23 : 1;                        // [23]
        uint32_t rf_clkwr_qtr_dl_sel_ac : 2;         // [25:24]
        uint32_t rfdl_clkwr_qtr_cnt_ac : 2;          // [27:26], read only
        uint32_t rf_clkwr_qtr_dl_cpst_offset_ac : 2; // [29:28]
        uint32_t rf_clkwr_dl_cpst_minus_ac : 1;      // [30]
        uint32_t rf_clkwr_dl_cpst_en_ac : 1;         // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_0_WR_AC_T;

// lpddr_rf_cfg_dll_dl_1_wr_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_out_a0_dl_sel : 5; // [4:0]
        uint32_t __7_5 : 3;                // [7:5]
        uint32_t rf_dly_out_a1_dl_sel : 5; // [12:8]
        uint32_t __15_13 : 3;              // [15:13]
        uint32_t rf_dly_out_a2_dl_sel : 5; // [20:16]
        uint32_t __23_21 : 3;              // [23:21]
        uint32_t rf_dly_out_a3_dl_sel : 5; // [28:24]
        uint32_t __31_29 : 3;              // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_1_WR_AC_T;

// lpddr_rf_cfg_dll_dl_2_wr_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_out_a4_dl_sel : 5; // [4:0]
        uint32_t __7_5 : 3;                // [7:5]
        uint32_t rf_dly_out_a5_dl_sel : 5; // [12:8]
        uint32_t __15_13 : 3;              // [15:13]
        uint32_t rf_dly_out_a6_dl_sel : 5; // [20:16]
        uint32_t __23_21 : 3;              // [23:21]
        uint32_t rf_dly_out_a7_dl_sel : 5; // [28:24]
        uint32_t __31_29 : 3;              // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_2_WR_AC_T;

// lpddr_rf_cfg_dll_dl_3_wr_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_out_a8_dl_sel : 5; // [4:0]
        uint32_t __7_5 : 3;                // [7:5]
        uint32_t rf_dly_out_a9_dl_sel : 5; // [12:8]
        uint32_t __31_13 : 19;             // [31:13]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_3_WR_AC_T;

// lpddr_rf_cfg_dll_dl_4_wr_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_out_csn_dl_sel : 5; // [4:0]
        uint32_t __15_5 : 11;               // [15:5]
        uint32_t rf_dly_out_cke_dl_sel : 5; // [20:16]
        uint32_t __23_21 : 3;               // [23:21]
        uint32_t rf_dly_out_clk_dl_sel : 5; // [28:24]
        uint32_t __31_29 : 3;               // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_4_WR_AC_T;

// lpddr_rfdll_status_max_cnt_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdll_max_cnt_f0_ac : 7; // [6:0], read only
        uint32_t __7_7 : 1;               // [7]
        uint32_t rfdll_max_cnt_f1_ac : 7; // [14:8], read only
        uint32_t __15_15 : 1;             // [15]
        uint32_t rfdll_max_cnt_f2_ac : 7; // [22:16], read only
        uint32_t __23_23 : 1;             // [23]
        uint32_t rfdll_max_cnt_f3_ac : 7; // [30:24], read only
        uint32_t __31_31 : 1;             // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_MAX_CNT_AC_T;

// lpddr_rfdll_status_min_cnt_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdll_min_cnt_f0_ac : 7; // [6:0], read only
        uint32_t __7_7 : 1;               // [7]
        uint32_t rfdll_min_cnt_f1_ac : 7; // [14:8], read only
        uint32_t __15_15 : 1;             // [15]
        uint32_t rfdll_min_cnt_f2_ac : 7; // [22:16], read only
        uint32_t __23_23 : 1;             // [23]
        uint32_t rfdll_min_cnt_f3_ac : 7; // [30:24], read only
        uint32_t __31_31 : 1;             // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_MIN_CNT_AC_T;

// lpddr_rf_cfg_phy_iomux_sel_wr_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_a0_sel_ac : 1;   // [0]
        uint32_t rf_phy_io_a1_sel_ac : 1;   // [1]
        uint32_t rf_phy_io_a2_sel_ac : 1;   // [2]
        uint32_t rf_phy_io_a3_sel_ac : 1;   // [3]
        uint32_t rf_phy_io_a4_sel_ac : 1;   // [4]
        uint32_t rf_phy_io_a5_sel_ac : 1;   // [5]
        uint32_t rf_phy_io_a6_sel_ac : 1;   // [6]
        uint32_t rf_phy_io_a7_sel_ac : 1;   // [7]
        uint32_t rf_phy_io_a8_sel_ac : 1;   // [8]
        uint32_t rf_phy_io_a9_sel_ac : 1;   // [9]
        uint32_t __15_10 : 6;               // [15:10]
        uint32_t rf_phy_io_csn_sel_ac : 1;  // [16]
        uint32_t __19_17 : 3;               // [19:17]
        uint32_t rf_phy_io_cke0_sel_ac : 1; // [20]
        uint32_t rf_phy_io_cke1_sel_ac : 1; // [21]
        uint32_t __31_22 : 10;              // [31:22]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_SEL_WR_AC_T;

// lpddr_rf_cfg_phy_iomux_ie_wr_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_a0_ie_ac : 1;   // [0]
        uint32_t rf_phy_io_a1_ie_ac : 1;   // [1]
        uint32_t rf_phy_io_a2_ie_ac : 1;   // [2]
        uint32_t rf_phy_io_a3_ie_ac : 1;   // [3]
        uint32_t rf_phy_io_a4_ie_ac : 1;   // [4]
        uint32_t rf_phy_io_a5_ie_ac : 1;   // [5]
        uint32_t rf_phy_io_a6_ie_ac : 1;   // [6]
        uint32_t rf_phy_io_a7_ie_ac : 1;   // [7]
        uint32_t rf_phy_io_a8_ie_ac : 1;   // [8]
        uint32_t rf_phy_io_a9_ie_ac : 1;   // [9]
        uint32_t __15_10 : 6;              // [15:10]
        uint32_t rf_phy_io_csn_ie_ac : 1;  // [16]
        uint32_t __19_17 : 3;              // [19:17]
        uint32_t rf_phy_io_cke0_ie_ac : 1; // [20]
        uint32_t rf_phy_io_cke1_ie_ac : 1; // [21]
        uint32_t __31_22 : 10;             // [31:22]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_IE_WR_AC_T;

// lpddr_rf_cfg_phy_iomux_oe_wr_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_a0_oe_ac : 1;   // [0]
        uint32_t rf_phy_io_a1_oe_ac : 1;   // [1]
        uint32_t rf_phy_io_a2_oe_ac : 1;   // [2]
        uint32_t rf_phy_io_a3_oe_ac : 1;   // [3]
        uint32_t rf_phy_io_a4_oe_ac : 1;   // [4]
        uint32_t rf_phy_io_a5_oe_ac : 1;   // [5]
        uint32_t rf_phy_io_a6_oe_ac : 1;   // [6]
        uint32_t rf_phy_io_a7_oe_ac : 1;   // [7]
        uint32_t rf_phy_io_a8_oe_ac : 1;   // [8]
        uint32_t rf_phy_io_a9_oe_ac : 1;   // [9]
        uint32_t __15_10 : 6;              // [15:10]
        uint32_t rf_phy_io_csn_oe_ac : 1;  // [16]
        uint32_t __19_17 : 3;              // [19:17]
        uint32_t rf_phy_io_cke0_oe_ac : 1; // [20]
        uint32_t rf_phy_io_cke1_oe_ac : 1; // [21]
        uint32_t __31_22 : 10;             // [31:22]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_OE_WR_AC_T;

// lpddr_rf_cfg_phy_iomux_out_wr_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_a0_out_ac : 1;   // [0]
        uint32_t rf_phy_io_a1_out_ac : 1;   // [1]
        uint32_t rf_phy_io_a2_out_ac : 1;   // [2]
        uint32_t rf_phy_io_a3_out_ac : 1;   // [3]
        uint32_t rf_phy_io_a4_out_ac : 1;   // [4]
        uint32_t rf_phy_io_a5_out_ac : 1;   // [5]
        uint32_t rf_phy_io_a6_out_ac : 1;   // [6]
        uint32_t rf_phy_io_a7_out_ac : 1;   // [7]
        uint32_t rf_phy_io_a8_out_ac : 1;   // [8]
        uint32_t rf_phy_io_a9_out_ac : 1;   // [9]
        uint32_t __15_10 : 6;               // [15:10]
        uint32_t rf_phy_io_csn_out_ac : 1;  // [16]
        uint32_t __19_17 : 3;               // [19:17]
        uint32_t rf_phy_io_cke0_out_ac : 1; // [20]
        uint32_t rf_phy_io_cke1_out_ac : 1; // [21]
        uint32_t __31_22 : 10;              // [31:22]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_OUT_WR_AC_T;

// lpddr_rf_cfg_dll_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __7_0 : 8;                      // [7:0]
        uint32_t rf_dll_clr_ds0 : 1;             // [8]
        uint32_t rf_dll_auto_clr_en_ds0 : 1;     // [9]
        uint32_t rf_dl_cpst_en_ds0 : 1;          // [10]
        uint32_t rf_dl_cpst_start_ds0 : 1;       // [11]
        uint32_t rf_dl_cpst_auto_ref_en_ds0 : 1; // [12]
        uint32_t rf_dll_err_clr_ds0 : 1;         // [13]
        uint32_t rf_dll_clk_sel_ds0 : 1;         // [14]
        uint32_t rf_dll_en_ds0 : 1;              // [15]
        uint32_t rf_dl_cpst_thr_ds0 : 8;         // [23:16]
        uint32_t rf_dll_pd_cnt_ds0 : 3;          // [26:24]
        uint32_t rf_dll_auto_err_clr_en_ds0 : 1; // [27]
        uint32_t rf_dll_lock_wait_ds0 : 4;       // [31:28]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DS0_T;

// lpddr_rfdll_status_dll_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdll_cnt_ds0 : 7;    // [6:0], read only
        uint32_t __23_7 : 17;          // [23:7]
        uint32_t rfdl_cpst_st_ds0 : 1; // [24], read only
        uint32_t rfdll_st_ds0 : 3;     // [27:25], read only
        uint32_t rfdll_locked_ds0 : 1; // [28], read only
        uint32_t rfdll_error_ds0 : 1;  // [29], read only
        uint32_t __31_30 : 2;          // [31:30]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_DLL_DS0_T;

// lpddr_rf_cfg_dll_dl_0_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_clkwr_raw_dl_sel_ds0 : 7;         // [6:0]
        uint32_t __7_7 : 1;                           // [7]
        uint32_t rfdl_clkwr_raw_cnt_ds0 : 7;          // [14:8], read only
        uint32_t __15_15 : 1;                         // [15]
        uint32_t rf_clkwr_raw_dl_cpst_offset_ds0 : 7; // [22:16]
        uint32_t __23_23 : 1;                         // [23]
        uint32_t rf_clkwr_qtr_dl_sel_ds0 : 2;         // [25:24]
        uint32_t rfdl_clkwr_qtr_cnt_ds0 : 2;          // [27:26], read only
        uint32_t rf_clkwr_qtr_dl_cpst_offset_ds0 : 2; // [29:28]
        uint32_t rf_clkwr_dl_cpst_minus_ds0 : 1;      // [30]
        uint32_t rf_clkwr_dl_cpst_en_ds0 : 1;         // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_0_WR_DS0_T;

// lpddr_rf_cfg_dll_dl_1_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dqs_in_pos_raw_dl_sel_ds0 : 7;         // [6:0]
        uint32_t __7_7 : 1;                                // [7]
        uint32_t rfdl_dqs_in_pos_raw_cnt_ds0 : 7;          // [14:8], read only
        uint32_t __15_15 : 1;                              // [15]
        uint32_t rf_dqs_in_pos_raw_dl_cpst_offset_ds0 : 7; // [22:16]
        uint32_t __23_23 : 1;                              // [23]
        uint32_t rf_dqs_in_pos_qtr_dl_sel_ds0 : 2;         // [25:24]
        uint32_t rfdl_dqs_in_pos_qtr_cnt_ds0 : 2;          // [27:26], read only
        uint32_t rf_dqs_in_pos_qtr_dl_cpst_offset_ds0 : 2; // [29:28]
        uint32_t rf_dqs_in_pos_dl_cpst_minus_ds0 : 1;      // [30]
        uint32_t rf_dqs_in_pos_dl_cpst_en_ds0 : 1;         // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_1_WR_DS0_T;

// lpddr_rf_cfg_dll_dl_2_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dqs_in_neg_raw_dl_sel_ds0 : 7;         // [6:0]
        uint32_t __7_7 : 1;                                // [7]
        uint32_t rfdl_dqs_in_neg_raw_cnt_ds0 : 7;          // [14:8], read only
        uint32_t __15_15 : 1;                              // [15]
        uint32_t rf_dqs_in_neg_raw_dl_cpst_offset_ds0 : 7; // [22:16]
        uint32_t __23_23 : 1;                              // [23]
        uint32_t rf_dqs_in_neg_qtr_dl_sel_ds0 : 2;         // [25:24]
        uint32_t rfdl_dqs_in_neg_qtr_cnt_ds0 : 2;          // [27:26], read only
        uint32_t rf_dqs_in_neg_qtr_dl_cpst_offset_ds0 : 2; // [29:28]
        uint32_t rf_dqs_in_neg_dl_cpst_minus_ds0 : 1;      // [30]
        uint32_t rf_dqs_in_neg_dl_cpst_en_ds0 : 1;         // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_2_WR_DS0_T;

// lpddr_rf_cfg_dll_dl_3_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dqs_gate_raw_dl_sel_ds0 : 7;         // [6:0]
        uint32_t __7_7 : 1;                              // [7]
        uint32_t rfdl_dqs_gate_raw_cnt_ds0 : 7;          // [14:8], read only
        uint32_t __15_15 : 1;                            // [15]
        uint32_t rf_dqs_gate_raw_dl_cpst_offset_ds0 : 7; // [22:16]
        uint32_t __23_23 : 1;                            // [23]
        uint32_t rf_dqs_gate_qtr_dl_sel_ds0 : 2;         // [25:24]
        uint32_t rfdl_dqs_gate_qtr_cnt_ds0 : 2;          // [27:26], read only
        uint32_t rf_dqs_gate_qtr_dl_cpst_offset_ds0 : 2; // [29:28]
        uint32_t rf_dqs_gate_dl_cpst_minus_ds0 : 1;      // [30]
        uint32_t rf_dqs_gate_dl_cpst_en_ds0 : 1;         // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_3_WR_DS0_T;

// lpddr_rf_cfg_dll_dl_4_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_out_d0_dl_sel_ds0 : 5; // [4:0]
        uint32_t __7_5 : 3;                    // [7:5]
        uint32_t rf_dly_out_d1_dl_sel_ds0 : 5; // [12:8]
        uint32_t __15_13 : 3;                  // [15:13]
        uint32_t rf_dly_out_d2_dl_sel_ds0 : 5; // [20:16]
        uint32_t __23_21 : 3;                  // [23:21]
        uint32_t rf_dly_out_d3_dl_sel_ds0 : 5; // [28:24]
        uint32_t __31_29 : 3;                  // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_4_WR_DS0_T;

// lpddr_rf_cfg_dll_dl_5_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_out_d4_dl_sel_ds0 : 5; // [4:0]
        uint32_t __7_5 : 3;                    // [7:5]
        uint32_t rf_dly_out_d5_dl_sel_ds0 : 5; // [12:8]
        uint32_t __15_13 : 3;                  // [15:13]
        uint32_t rf_dly_out_d6_dl_sel_ds0 : 5; // [20:16]
        uint32_t __23_21 : 3;                  // [23:21]
        uint32_t rf_dly_out_d7_dl_sel_ds0 : 5; // [28:24]
        uint32_t __31_29 : 3;                  // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_5_WR_DS0_T;

// lpddr_rf_cfg_dll_dl_6_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_in_d0_dl_sel_ds0 : 5; // [4:0]
        uint32_t __7_5 : 3;                   // [7:5]
        uint32_t rf_dly_in_d1_dl_sel_ds0 : 5; // [12:8]
        uint32_t __15_13 : 3;                 // [15:13]
        uint32_t rf_dly_in_d2_dl_sel_ds0 : 5; // [20:16]
        uint32_t __23_21 : 3;                 // [23:21]
        uint32_t rf_dly_in_d3_dl_sel_ds0 : 5; // [28:24]
        uint32_t __31_29 : 3;                 // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_6_WR_DS0_T;

// lpddr_rf_cfg_dll_dl_7_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_in_d4_dl_sel_ds0 : 5; // [4:0]
        uint32_t __7_5 : 3;                   // [7:5]
        uint32_t rf_dly_in_d5_dl_sel_ds0 : 5; // [12:8]
        uint32_t __15_13 : 3;                 // [15:13]
        uint32_t rf_dly_in_d6_dl_sel_ds0 : 5; // [20:16]
        uint32_t __23_21 : 3;                 // [23:21]
        uint32_t rf_dly_in_d7_dl_sel_ds0 : 5; // [28:24]
        uint32_t __31_29 : 3;                 // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_7_WR_DS0_T;

// lpddr_rf_cfg_dll_dl_8_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_out_dqs_dl_sel_ds0 : 5; // [4:0]
        uint32_t __7_5 : 3;                     // [7:5]
        uint32_t rf_dly_out_dqm_dl_sel_ds0 : 5; // [12:8]
        uint32_t __15_13 : 3;                   // [15:13]
        uint32_t rf_dly_in_dqs_dl_sel_ds0 : 5;  // [20:16]
        uint32_t __31_21 : 11;                  // [31:21]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_8_WR_DS0_T;

// lpddr_rfdll_status_max_cnt_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdll_max_cnt_f0_ds0 : 7; // [6:0], read only
        uint32_t __7_7 : 1;                // [7]
        uint32_t rfdll_max_cnt_f1_ds0 : 7; // [14:8], read only
        uint32_t __15_15 : 1;              // [15]
        uint32_t rfdll_max_cnt_f2_ds0 : 7; // [22:16], read only
        uint32_t __23_23 : 1;              // [23]
        uint32_t rfdll_max_cnt_f3_ds0 : 7; // [30:24], read only
        uint32_t __31_31 : 1;              // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_MAX_CNT_DS0_T;

// lpddr_rfdll_status_min_cnt_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdll_min_cnt_f0_ds0 : 7; // [6:0], read only
        uint32_t __7_7 : 1;                // [7]
        uint32_t rfdll_min_cnt_f1_ds0 : 7; // [14:8], read only
        uint32_t __15_15 : 1;              // [15]
        uint32_t rfdll_min_cnt_f2_ds0 : 7; // [22:16], read only
        uint32_t __23_23 : 1;              // [23]
        uint32_t rfdll_min_cnt_f3_ds0 : 7; // [30:24], read only
        uint32_t __31_31 : 1;              // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_MIN_CNT_DS0_T;

// lpddr_rf_cfg_phy_iomux_sel_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_d0_sel_ds0 : 1;  // [0]
        uint32_t rf_phy_io_d1_sel_ds0 : 1;  // [1]
        uint32_t rf_phy_io_d2_sel_ds0 : 1;  // [2]
        uint32_t rf_phy_io_d3_sel_ds0 : 1;  // [3]
        uint32_t rf_phy_io_d4_sel_ds0 : 1;  // [4]
        uint32_t rf_phy_io_d5_sel_ds0 : 1;  // [5]
        uint32_t rf_phy_io_d6_sel_ds0 : 1;  // [6]
        uint32_t rf_phy_io_d7_sel_ds0 : 1;  // [7]
        uint32_t rf_phy_io_dqm_sel_ds0 : 1; // [8]
        uint32_t rf_phy_io_dqs_sel_ds0 : 1; // [9]
        uint32_t __31_10 : 22;              // [31:10]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_SEL_WR_DS0_T;

// lpddr_rf_cfg_phy_iomux_ie_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_d0_ie_ds0 : 1;  // [0]
        uint32_t rf_phy_io_d1_ie_ds0 : 1;  // [1]
        uint32_t rf_phy_io_d2_ie_ds0 : 1;  // [2]
        uint32_t rf_phy_io_d3_ie_ds0 : 1;  // [3]
        uint32_t rf_phy_io_d4_ie_ds0 : 1;  // [4]
        uint32_t rf_phy_io_d5_ie_ds0 : 1;  // [5]
        uint32_t rf_phy_io_d6_ie_ds0 : 1;  // [6]
        uint32_t rf_phy_io_d7_ie_ds0 : 1;  // [7]
        uint32_t rf_phy_io_dqm_ie_ds0 : 1; // [8]
        uint32_t rf_phy_io_dqs_ie_ds0 : 1; // [9]
        uint32_t __31_10 : 22;             // [31:10]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_IE_WR_DS0_T;

// lpddr_rf_cfg_phy_iomux_oe_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_d0_oe_ds0 : 1;  // [0]
        uint32_t rf_phy_io_d1_oe_ds0 : 1;  // [1]
        uint32_t rf_phy_io_d2_oe_ds0 : 1;  // [2]
        uint32_t rf_phy_io_d3_oe_ds0 : 1;  // [3]
        uint32_t rf_phy_io_d4_oe_ds0 : 1;  // [4]
        uint32_t rf_phy_io_d5_oe_ds0 : 1;  // [5]
        uint32_t rf_phy_io_d6_oe_ds0 : 1;  // [6]
        uint32_t rf_phy_io_d7_oe_ds0 : 1;  // [7]
        uint32_t rf_phy_io_dqm_oe_ds0 : 1; // [8]
        uint32_t rf_phy_io_dqs_oe_ds0 : 1; // [9]
        uint32_t __31_10 : 22;             // [31:10]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_OE_WR_DS0_T;

// lpddr_rf_cfg_phy_iomux_out_wr_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_d0_out_ds0 : 1;  // [0]
        uint32_t rf_phy_io_d1_out_ds0 : 1;  // [1]
        uint32_t rf_phy_io_d2_out_ds0 : 1;  // [2]
        uint32_t rf_phy_io_d3_out_ds0 : 1;  // [3]
        uint32_t rf_phy_io_d4_out_ds0 : 1;  // [4]
        uint32_t rf_phy_io_d5_out_ds0 : 1;  // [5]
        uint32_t rf_phy_io_d6_out_ds0 : 1;  // [6]
        uint32_t rf_phy_io_d7_out_ds0 : 1;  // [7]
        uint32_t rf_phy_io_dqm_out_ds0 : 1; // [8]
        uint32_t rf_phy_io_dqs_out_ds0 : 1; // [9]
        uint32_t __31_10 : 22;              // [31:10]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_OUT_WR_DS0_T;

// lpddr_rf_cfg_dll_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __7_0 : 8;                      // [7:0]
        uint32_t rf_dll_clr_ds1 : 1;             // [8]
        uint32_t rf_dll_auto_clr_en_ds1 : 1;     // [9]
        uint32_t rf_dl_cpst_en_ds1 : 1;          // [10]
        uint32_t rf_dl_cpst_start_ds1 : 1;       // [11]
        uint32_t rf_dl_cpst_auto_ref_en_ds1 : 1; // [12]
        uint32_t rf_dll_err_clr_ds1 : 1;         // [13]
        uint32_t rf_dll_clk_sel_ds1 : 1;         // [14]
        uint32_t rf_dll_en_ds1 : 1;              // [15]
        uint32_t rf_dl_cpst_thr_ds1 : 8;         // [23:16]
        uint32_t rf_dll_pd_cnt_ds1 : 3;          // [26:24]
        uint32_t rf_dll_auto_err_clr_en_ds1 : 1; // [27]
        uint32_t rf_dll_lock_wait_ds1 : 4;       // [31:28]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DS1_T;

// lpddr_rfdll_status_dll_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdll_cnt_ds1 : 7;    // [6:0], read only
        uint32_t __23_7 : 17;          // [23:7]
        uint32_t rfdl_cpst_st_ds1 : 1; // [24], read only
        uint32_t rfdll_st_ds1 : 3;     // [27:25], read only
        uint32_t rfdll_locked_ds1 : 1; // [28], read only
        uint32_t rfdll_error_ds1 : 1;  // [29], read only
        uint32_t __31_30 : 2;          // [31:30]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_DLL_DS1_T;

// lpddr_rf_cfg_dll_dl_0_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_clkwr_raw_dl_sel_ds1 : 7;         // [6:0]
        uint32_t __7_7 : 1;                           // [7]
        uint32_t rfdl_clkwr_raw_cnt_ds1 : 7;          // [14:8], read only
        uint32_t __15_15 : 1;                         // [15]
        uint32_t rf_clkwr_raw_dl_cpst_offset_ds1 : 7; // [22:16]
        uint32_t __23_23 : 1;                         // [23]
        uint32_t rf_clkwr_qtr_dl_sel_ds1 : 2;         // [25:24]
        uint32_t rfdl_clkwr_qtr_cnt_ds1 : 2;          // [27:26], read only
        uint32_t rf_clkwr_qtr_dl_cpst_offset_ds1 : 2; // [29:28]
        uint32_t rf_clkwr_dl_cpst_minus_ds1 : 1;      // [30]
        uint32_t rf_clkwr_dl_cpst_en_ds1 : 1;         // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_0_WR_DS1_T;

// lpddr_rf_cfg_dll_dl_1_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dqs_in_pos_raw_dl_sel_ds1 : 7;         // [6:0]
        uint32_t __7_7 : 1;                                // [7]
        uint32_t rfdl_dqs_in_pos_raw_cnt_ds1 : 7;          // [14:8], read only
        uint32_t __15_15 : 1;                              // [15]
        uint32_t rf_dqs_in_pos_raw_dl_cpst_offset_ds1 : 7; // [22:16]
        uint32_t __23_23 : 1;                              // [23]
        uint32_t rf_dqs_in_pos_qtr_dl_sel_ds1 : 2;         // [25:24]
        uint32_t rfdl_dqs_in_pos_qtr_cnt_ds1 : 2;          // [27:26], read only
        uint32_t rf_dqs_in_pos_qtr_dl_cpst_offset_ds1 : 2; // [29:28]
        uint32_t rf_dqs_in_pos_dl_cpst_minus_ds1 : 1;      // [30]
        uint32_t rf_dqs_in_pos_dl_cpst_en_ds1 : 1;         // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_1_WR_DS1_T;

// lpddr_rf_cfg_dll_dl_2_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dqs_in_neg_raw_dl_sel_ds1 : 7;         // [6:0]
        uint32_t __7_7 : 1;                                // [7]
        uint32_t rfdl_dqs_in_neg_raw_cnt_ds1 : 7;          // [14:8], read only
        uint32_t __15_15 : 1;                              // [15]
        uint32_t rf_dqs_in_neg_raw_dl_cpst_offset_ds1 : 7; // [22:16]
        uint32_t __23_23 : 1;                              // [23]
        uint32_t rf_dqs_in_neg_qtr_dl_sel_ds1 : 2;         // [25:24]
        uint32_t rfdl_dqs_in_neg_qtr_cnt_ds1 : 2;          // [27:26], read only
        uint32_t rf_dqs_in_neg_qtr_dl_cpst_offset_ds1 : 2; // [29:28]
        uint32_t rf_dqs_in_neg_dl_cpst_minus_ds1 : 1;      // [30]
        uint32_t rf_dqs_in_neg_dl_cpst_en_ds1 : 1;         // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_2_WR_DS1_T;

// lpddr_rf_cfg_dll_dl_3_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dqs_gate_raw_dl_sel_ds1 : 7;         // [6:0]
        uint32_t __7_7 : 1;                              // [7]
        uint32_t rfdl_dqs_gate_raw_cnt_ds1 : 7;          // [14:8], read only
        uint32_t __15_15 : 1;                            // [15]
        uint32_t rf_dqs_gate_raw_dl_cpst_offset_ds1 : 7; // [22:16]
        uint32_t __23_23 : 1;                            // [23]
        uint32_t rf_dqs_gate_qtr_dl_sel_ds1 : 2;         // [25:24]
        uint32_t rfdl_dqs_gate_qtr_cnt_ds1 : 2;          // [27:26], read only
        uint32_t rf_dqs_gate_qtr_dl_cpst_offset_ds1 : 2; // [29:28]
        uint32_t rf_dqs_gate_dl_cpst_minus_ds1 : 1;      // [30]
        uint32_t rf_dqs_gate_dl_cpst_en_ds1 : 1;         // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_3_WR_DS1_T;

// lpddr_rf_cfg_dll_dl_4_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_out_d0_dl_sel_ds1 : 5; // [4:0]
        uint32_t __7_5 : 3;                    // [7:5]
        uint32_t rf_dly_out_d1_dl_sel_ds1 : 5; // [12:8]
        uint32_t __15_13 : 3;                  // [15:13]
        uint32_t rf_dly_out_d2_dl_sel_ds1 : 5; // [20:16]
        uint32_t __23_21 : 3;                  // [23:21]
        uint32_t rf_dly_out_d3_dl_sel_ds1 : 5; // [28:24]
        uint32_t __31_29 : 3;                  // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_4_WR_DS1_T;

// lpddr_rf_cfg_dll_dl_5_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_out_d4_dl_sel_ds1 : 5; // [4:0]
        uint32_t __7_5 : 3;                    // [7:5]
        uint32_t rf_dly_out_d5_dl_sel_ds1 : 5; // [12:8]
        uint32_t __15_13 : 3;                  // [15:13]
        uint32_t rf_dly_out_d6_dl_sel_ds1 : 5; // [20:16]
        uint32_t __23_21 : 3;                  // [23:21]
        uint32_t rf_dly_out_d7_dl_sel_ds1 : 5; // [28:24]
        uint32_t __31_29 : 3;                  // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_5_WR_DS1_T;

// lpddr_rf_cfg_dll_dl_6_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_in_d0_dl_sel_ds1 : 5; // [4:0]
        uint32_t __7_5 : 3;                   // [7:5]
        uint32_t rf_dly_in_d1_dl_sel_ds1 : 5; // [12:8]
        uint32_t __15_13 : 3;                 // [15:13]
        uint32_t rf_dly_in_d2_dl_sel_ds1 : 5; // [20:16]
        uint32_t __23_21 : 3;                 // [23:21]
        uint32_t rf_dly_in_d3_dl_sel_ds1 : 5; // [28:24]
        uint32_t __31_29 : 3;                 // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_6_WR_DS1_T;

// lpddr_rf_cfg_dll_dl_7_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_in_d4_dl_sel_ds1 : 5; // [4:0]
        uint32_t __7_5 : 3;                   // [7:5]
        uint32_t rf_dly_in_d5_dl_sel_ds1 : 5; // [12:8]
        uint32_t __15_13 : 3;                 // [15:13]
        uint32_t rf_dly_in_d6_dl_sel_ds1 : 5; // [20:16]
        uint32_t __23_21 : 3;                 // [23:21]
        uint32_t rf_dly_in_d7_dl_sel_ds1 : 5; // [28:24]
        uint32_t __31_29 : 3;                 // [31:29]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_7_WR_DS1_T;

// lpddr_rf_cfg_dll_dl_8_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_dly_out_dqs_dl_sel_ds1 : 5; // [4:0]
        uint32_t __7_5 : 3;                     // [7:5]
        uint32_t rf_dly_out_dqm_dl_sel_ds1 : 5; // [12:8]
        uint32_t __15_13 : 3;                   // [15:13]
        uint32_t rf_dly_in_dqs_dl_sel_ds1 : 5;  // [20:16]
        uint32_t __31_21 : 11;                  // [31:21]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_DLL_DL_8_WR_DS1_T;

// lpddr_rfdll_status_max_cnt_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdll_max_cnt_f0_ds1 : 7; // [6:0], read only
        uint32_t __7_7 : 1;                // [7]
        uint32_t rfdll_max_cnt_f1_ds1 : 7; // [14:8], read only
        uint32_t __15_15 : 1;              // [15]
        uint32_t rfdll_max_cnt_f2_ds1 : 7; // [22:16], read only
        uint32_t __23_23 : 1;              // [23]
        uint32_t rfdll_max_cnt_f3_ds1 : 7; // [30:24], read only
        uint32_t __31_31 : 1;              // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_MAX_CNT_DS1_T;

// lpddr_rfdll_status_min_cnt_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfdll_min_cnt_f0_ds1 : 7; // [6:0], read only
        uint32_t __7_7 : 1;                // [7]
        uint32_t rfdll_min_cnt_f1_ds1 : 7; // [14:8], read only
        uint32_t __15_15 : 1;              // [15]
        uint32_t rfdll_min_cnt_f2_ds1 : 7; // [22:16], read only
        uint32_t __23_23 : 1;              // [23]
        uint32_t rfdll_min_cnt_f3_ds1 : 7; // [30:24], read only
        uint32_t __31_31 : 1;              // [31]
    } b;
} REG_LPDDR_PHY_LPDDR_RFDLL_STATUS_MIN_CNT_DS1_T;

// lpddr_rf_cfg_phy_iomux_sel_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_d0_sel_ds1 : 1;  // [0]
        uint32_t rf_phy_io_d1_sel_ds1 : 1;  // [1]
        uint32_t rf_phy_io_d2_sel_ds1 : 1;  // [2]
        uint32_t rf_phy_io_d3_sel_ds1 : 1;  // [3]
        uint32_t rf_phy_io_d4_sel_ds1 : 1;  // [4]
        uint32_t rf_phy_io_d5_sel_ds1 : 1;  // [5]
        uint32_t rf_phy_io_d6_sel_ds1 : 1;  // [6]
        uint32_t rf_phy_io_d7_sel_ds1 : 1;  // [7]
        uint32_t rf_phy_io_dqm_sel_ds1 : 1; // [8]
        uint32_t rf_phy_io_dqs_sel_ds1 : 1; // [9]
        uint32_t __31_10 : 22;              // [31:10]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_SEL_WR_DS1_T;

// lpddr_rf_cfg_phy_iomux_ie_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_d0_ie_ds1 : 1;  // [0]
        uint32_t rf_phy_io_d1_ie_ds1 : 1;  // [1]
        uint32_t rf_phy_io_d2_ie_ds1 : 1;  // [2]
        uint32_t rf_phy_io_d3_ie_ds1 : 1;  // [3]
        uint32_t rf_phy_io_d4_ie_ds1 : 1;  // [4]
        uint32_t rf_phy_io_d5_ie_ds1 : 1;  // [5]
        uint32_t rf_phy_io_d6_ie_ds1 : 1;  // [6]
        uint32_t rf_phy_io_d7_ie_ds1 : 1;  // [7]
        uint32_t rf_phy_io_dqm_ie_ds1 : 1; // [8]
        uint32_t rf_phy_io_dqs_ie_ds1 : 1; // [9]
        uint32_t __31_10 : 22;             // [31:10]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_IE_WR_DS1_T;

// lpddr_rf_cfg_phy_iomux_oe_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_d0_oe_ds1 : 1;  // [0]
        uint32_t rf_phy_io_d1_oe_ds1 : 1;  // [1]
        uint32_t rf_phy_io_d2_oe_ds1 : 1;  // [2]
        uint32_t rf_phy_io_d3_oe_ds1 : 1;  // [3]
        uint32_t rf_phy_io_d4_oe_ds1 : 1;  // [4]
        uint32_t rf_phy_io_d5_oe_ds1 : 1;  // [5]
        uint32_t rf_phy_io_d6_oe_ds1 : 1;  // [6]
        uint32_t rf_phy_io_d7_oe_ds1 : 1;  // [7]
        uint32_t rf_phy_io_dqm_oe_ds1 : 1; // [8]
        uint32_t rf_phy_io_dqs_oe_ds1 : 1; // [9]
        uint32_t __31_10 : 22;             // [31:10]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_OE_WR_DS1_T;

// lpddr_rf_cfg_phy_iomux_out_wr_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_io_d0_out_ds1 : 1;  // [0]
        uint32_t rf_phy_io_d1_out_ds1 : 1;  // [1]
        uint32_t rf_phy_io_d2_out_ds1 : 1;  // [2]
        uint32_t rf_phy_io_d3_out_ds1 : 1;  // [3]
        uint32_t rf_phy_io_d4_out_ds1 : 1;  // [4]
        uint32_t rf_phy_io_d5_out_ds1 : 1;  // [5]
        uint32_t rf_phy_io_d6_out_ds1 : 1;  // [6]
        uint32_t rf_phy_io_d7_out_ds1 : 1;  // [7]
        uint32_t rf_phy_io_dqm_out_ds1 : 1; // [8]
        uint32_t rf_phy_io_dqs_out_ds1 : 1; // [9]
        uint32_t __31_10 : 22;              // [31:10]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_CFG_PHY_IOMUX_OUT_WR_DS1_T;

// lpddr_rf_status_phy_data_in
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_phy_data_in : 16; // [15:0], read only
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_STATUS_PHY_DATA_IN_T;

// lpddr_drf_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_clkdmem_out_sel : 1; // [0]
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_T;

// lpddr_drf_cfg_reg_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_reg_sel : 2; // [1:0]
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_REG_SEL_T;

// lpddr_drf_cfg_dqs_ie_sel_f0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_ie_sel_f0 : 16; // [15:0]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_IE_SEL_F0_T;

// lpddr_drf_cfg_dqs_oe_sel_f0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_oe_sel_f0 : 16; // [15:0]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_OE_SEL_F0_T;

// lpddr_drf_cfg_dqs_out_sel_f0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_out_sel_f0 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_OUT_SEL_F0_T;

// lpddr_drf_cfg_dqs_gate_sel_f0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_gate_sel_f0 : 16; // [15:0]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_GATE_SEL_F0_T;

// lpddr_drf_cfg_data_ie_sel_f0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_data_ie_sel_f0 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DATA_IE_SEL_F0_T;

// lpddr_drf_cfg_data_oe_sel_f0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_data_oe_sel_f0 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DATA_OE_SEL_F0_T;

// lpddr_drf_cfg_read_en_sel_f0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_read_en_sel_f0 : 4; // [3:0]
        uint32_t __31_4 : 28;            // [31:4]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_READ_EN_SEL_F0_T;

// lpddr_drf_cfg_dqs_ie_sel_f1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_ie_sel_f1 : 16; // [15:0]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_IE_SEL_F1_T;

// lpddr_drf_cfg_dqs_oe_sel_f1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_oe_sel_f1 : 16; // [15:0]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_OE_SEL_F1_T;

// lpddr_drf_cfg_dqs_out_sel_f1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_out_sel_f1 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_OUT_SEL_F1_T;

// lpddr_drf_cfg_dqs_gate_sel_f1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_gate_sel_f1 : 16; // [15:0]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_GATE_SEL_F1_T;

// lpddr_drf_cfg_data_ie_sel_f1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_data_ie_sel_f1 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DATA_IE_SEL_F1_T;

// lpddr_drf_cfg_data_oe_sel_f1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_data_oe_sel_f1 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DATA_OE_SEL_F1_T;

// lpddr_drf_cfg_read_en_sel_f1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_read_en_sel_f1 : 4; // [3:0]
        uint32_t __31_4 : 28;            // [31:4]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_READ_EN_SEL_F1_T;

// lpddr_drf_cfg_dqs_ie_sel_f2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_ie_sel_f2 : 16; // [15:0]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_IE_SEL_F2_T;

// lpddr_drf_cfg_dqs_oe_sel_f2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_oe_sel_f2 : 16; // [15:0]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_OE_SEL_F2_T;

// lpddr_drf_cfg_dqs_out_sel_f2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_out_sel_f2 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_OUT_SEL_F2_T;

// lpddr_drf_cfg_dqs_gate_sel_f2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_gate_sel_f2 : 16; // [15:0]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_GATE_SEL_F2_T;

// lpddr_drf_cfg_data_ie_sel_f2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_data_ie_sel_f2 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DATA_IE_SEL_F2_T;

// lpddr_drf_cfg_data_oe_sel_f2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_data_oe_sel_f2 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DATA_OE_SEL_F2_T;

// lpddr_drf_cfg_read_en_sel_f2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_read_en_sel_f2 : 4; // [3:0]
        uint32_t __31_4 : 28;            // [31:4]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_READ_EN_SEL_F2_T;

// lpddr_drf_cfg_dqs_ie_sel_f3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_ie_sel_f3 : 16; // [15:0]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_IE_SEL_F3_T;

// lpddr_drf_cfg_dqs_oe_sel_f3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_oe_sel_f3 : 16; // [15:0]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_OE_SEL_F3_T;

// lpddr_drf_cfg_dqs_out_sel_f3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_out_sel_f3 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_OUT_SEL_F3_T;

// lpddr_drf_cfg_dqs_gate_sel_f3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dqs_gate_sel_f3 : 16; // [15:0]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DQS_GATE_SEL_F3_T;

// lpddr_drf_cfg_data_ie_sel_f3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_data_ie_sel_f3 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DATA_IE_SEL_F3_T;

// lpddr_drf_cfg_data_oe_sel_f3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_data_oe_sel_f3 : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DATA_OE_SEL_F3_T;

// lpddr_drf_cfg_read_en_sel_f3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_read_en_sel_f3 : 4; // [3:0]
        uint32_t __31_4 : 28;            // [31:4]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_READ_EN_SEL_F3_T;

// lpddr_drf_cfg_dll_mode_f0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dll_clk_mode_f0 : 1;  // [0]
        uint32_t drf_dll_half_mode_f0 : 1; // [1]
        uint32_t drf_dll_satu_mode_f0 : 1; // [2]
        uint32_t __31_3 : 29;              // [31:3]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DLL_MODE_F0_T;

// lpddr_drf_cfg_dll_cnt_f0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dll_init_cnt_f0 : 10; // [9:0]
        uint32_t drf_dll_satu_cnt_f0 : 10; // [19:10]
        uint32_t drf_dll_auto_cnt_f0 : 10; // [29:20]
        uint32_t __31_30 : 2;              // [31:30]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DLL_CNT_F0_T;

// lpddr_drf_cfg_dll_mode_f1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dll_clk_mode_f1 : 1;  // [0]
        uint32_t drf_dll_half_mode_f1 : 1; // [1]
        uint32_t drf_dll_satu_mode_f1 : 1; // [2]
        uint32_t __31_3 : 29;              // [31:3]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DLL_MODE_F1_T;

// lpddr_drf_cfg_dll_cnt_f1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dll_init_cnt_f1 : 10; // [9:0]
        uint32_t drf_dll_satu_cnt_f1 : 10; // [19:10]
        uint32_t drf_dll_auto_cnt_f1 : 10; // [29:20]
        uint32_t __31_30 : 2;              // [31:30]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DLL_CNT_F1_T;

// lpddr_drf_cfg_dll_mode_f2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dll_clk_mode_f2 : 1;  // [0]
        uint32_t drf_dll_half_mode_f2 : 1; // [1]
        uint32_t drf_dll_satu_mode_f2 : 1; // [2]
        uint32_t __31_3 : 29;              // [31:3]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DLL_MODE_F2_T;

// lpddr_drf_cfg_dll_cnt_f2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dll_init_cnt_f2 : 10; // [9:0]
        uint32_t drf_dll_satu_cnt_f2 : 10; // [19:10]
        uint32_t drf_dll_auto_cnt_f2 : 10; // [29:20]
        uint32_t __31_30 : 2;              // [31:30]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DLL_CNT_F2_T;

// lpddr_drf_cfg_dll_mode_f3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dll_clk_mode_f3 : 1;  // [0]
        uint32_t drf_dll_half_mode_f3 : 1; // [1]
        uint32_t drf_dll_satu_mode_f3 : 1; // [2]
        uint32_t __31_3 : 29;              // [31:3]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DLL_MODE_F3_T;

// lpddr_drf_cfg_dll_cnt_f3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_dll_init_cnt_f3 : 10; // [9:0]
        uint32_t drf_dll_satu_cnt_f3 : 10; // [19:10]
        uint32_t drf_dll_auto_cnt_f3 : 10; // [29:20]
        uint32_t __31_30 : 2;              // [31:30]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_CFG_DLL_CNT_F3_T;

// lpddr_drf_train_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t drf_phyupd_en : 1;         // [0]
        uint32_t drf_phyupd_type_sel : 2;   // [2:1]
        uint32_t __3_3 : 1;                 // [3]
        uint32_t drf_phyupd_type_0 : 2;     // [5:4]
        uint32_t drf_phyupd_type_1 : 2;     // [7:6]
        uint32_t drf_phyupd_type_2 : 2;     // [9:8]
        uint32_t drf_phyupd_type_3 : 2;     // [11:10]
        uint32_t drf_phy_wrlvl_en : 1;      // [12]
        uint32_t drf_dmc_wrlvl_en : 1;      // [13]
        uint32_t __15_14 : 2;               // [15:14]
        uint32_t drf_phy_rdlvl_en : 1;      // [16]
        uint32_t drf_dmc_rdlvl_en : 1;      // [17]
        uint32_t __19_18 : 2;               // [19:18]
        uint32_t drf_phy_rdlvl_gate_en : 1; // [20]
        uint32_t drf_dmc_rdlvl_gate_en : 1; // [21]
        uint32_t __31_22 : 10;              // [31:22]
    } b;
} REG_LPDDR_PHY_LPDDR_DRF_TRAIN_CFG_T;

// lpddr_rf_irq_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_irq_en_dll_unlock_ac : 1;  // [0]
        uint32_t rf_irq_en_dll_unlock_ds0 : 1; // [1]
        uint32_t rf_irq_en_dll_unlock_ds1 : 1; // [2]
        uint32_t __31_3 : 29;                  // [31:3]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_IRQ_CTRL_T;

// lpddr_rf_irq_status_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_irq_st_clr_dll_unlock_ac : 1;  // [0]
        uint32_t rf_irq_st_clr_dll_unlock_ds0 : 1; // [1]
        uint32_t rf_irq_st_clr_dll_unlock_ds1 : 1; // [2]
        uint32_t __31_3 : 29;                      // [31:3]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_IRQ_STATUS_CLR_T;

// lpddr_rf_irq_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_irq_st_dll_unlock_ac : 1;  // [0], read only
        uint32_t rf_irq_st_dll_unlock_ds0 : 1; // [1], read only
        uint32_t rf_irq_st_dll_unlock_ds1 : 1; // [2], read only
        uint32_t __31_3 : 29;                  // [31:3]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_IRQ_STATUS_T;

// lpddr_rf_irq_cnt_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_irq_cnt_clr_dll_unlock_ac : 1;  // [0]
        uint32_t rf_irq_cnt_clr_dll_unlock_ds0 : 1; // [1]
        uint32_t rf_irq_cnt_clr_dll_unlock_ds1 : 1; // [2]
        uint32_t __31_3 : 29;                       // [31:3]
    } b;
} REG_LPDDR_PHY_LPDDR_RF_IRQ_CNT_CLR_T;

// lpddr_rf_irq_cnt_dll_unlock_ac
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_irq_cnt_dll_unlock_ac : 31;         // [30:0], read only
        uint32_t rf_irq_cnt_overflow_dll_unlock_ac : 1; // [31], read only
    } b;
} REG_LPDDR_PHY_LPDDR_RF_IRQ_CNT_DLL_UNLOCK_AC_T;

// lpddr_rf_irq_cnt_dll_unlock_ds0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_irq_cnt_dll_unlock_ds0 : 31;         // [30:0], read only
        uint32_t rf_irq_cnt_overflow_dll_unlock_ds0 : 1; // [31], read only
    } b;
} REG_LPDDR_PHY_LPDDR_RF_IRQ_CNT_DLL_UNLOCK_DS0_T;

// lpddr_rf_irq_cnt_dll_unlock_ds1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_irq_cnt_dll_unlock_ds1 : 31;         // [30:0], read only
        uint32_t rf_irq_cnt_overflow_dll_unlock_ds1 : 1; // [31], read only
    } b;
} REG_LPDDR_PHY_LPDDR_RF_IRQ_CNT_DLL_UNLOCK_DS1_T;

// io_rf_ddr_rpull_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddr_dqs_rpull : 2; // [1:0]
        uint32_t ddr_ck_rpull : 2;  // [3:2]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_LPDDR_PHY_IO_RF_DDR_RPULL_CFG_T;

// io_rf_ddr_drv_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddr_drvpd : 5; // [4:0]
        uint32_t ddr_drvpc : 5; // [9:5]
        uint32_t ddr_drvnd : 5; // [14:10]
        uint32_t ddr_drvnc : 5; // [19:15]
        uint32_t __31_20 : 12;  // [31:20]
    } b;
} REG_LPDDR_PHY_IO_RF_DDR_DRV_CFG_T;

// io_rf_ddr_pad_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddr_lp2d : 1; // [0]
        uint32_t ddr_lp2c : 1; // [1]
        uint32_t __31_2 : 30;  // [31:2]
    } b;
} REG_LPDDR_PHY_IO_RF_DDR_PAD_CFG_T;

// io_rf_ddr_vref_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddr_vref_en_pad : 1; // [0]
        uint32_t ddr_vref_en_int : 1; // [1]
        uint32_t ddr_sel_pad : 6;     // [7:2]
        uint32_t ddr_sel_int : 6;     // [13:8]
        uint32_t __31_14 : 18;        // [31:14]
    } b;
} REG_LPDDR_PHY_IO_RF_DDR_VREF_CFG_T;

// io_rf_ddr_zq_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddr_zprog : 5;      // [4:0]
        uint32_t ddr_cal_zq_pd : 1;  // [5]
        uint32_t ddr_cal_zq_cal : 1; // [6]
        uint32_t __31_7 : 25;        // [31:7]
    } b;
} REG_LPDDR_PHY_IO_RF_DDR_ZQ_CFG_T;

// io_rf_ddr_zq_mon
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddr_calover : 1; // [0], read only
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_LPDDR_PHY_IO_RF_DDR_ZQ_MON_T;

// io_rf_ddr_odt_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddr_odt_to_pad : 1;       // [0]
        uint32_t ddr_odt_write_enable : 1; // [1]
        uint32_t ddr_odt_read_enable : 1;  // [2]
        uint32_t __31_3 : 29;              // [31:3]
    } b;
} REG_LPDDR_PHY_IO_RF_DDR_ODT_CFG_T;

// io_rf_psram_drv_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psram_fix_read0 : 1; // [0]
        uint32_t psram_slewrate : 2;  // [2:1]
        uint32_t psram_drvp : 5;      // [7:3]
        uint32_t psram_drvn : 5;      // [12:8]
        uint32_t __31_13 : 19;        // [31:13]
    } b;
} REG_LPDDR_PHY_IO_RF_PSRAM_DRV_CFG_T;

// io_rf_psram_pad_en_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psram_pad_clkn_en : 1; // [0]
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_LPDDR_PHY_IO_RF_PSRAM_PAD_EN_CFG_T;

// io_rf_psram_pull_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psram_dqs_pull0_bit : 2;  // [1:0]
        uint32_t psram_dq_pull0_bit : 2;   // [3:2]
        uint32_t psram_dm_pull1_bit : 2;   // [5:4]
        uint32_t psram_clkn_pull1_bit : 2; // [7:6]
        uint32_t psram_clk_pull0_bit : 2;  // [9:8]
        uint32_t psram_cen_pull1_bit : 2;  // [11:10]
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_LPDDR_PHY_IO_RF_PSRAM_PULL_CFG_T;

// lpddr_rf_cfg_phy
#define LPDDR_PHY_RF_PHY_EN (1 << 0)
#define LPDDR_PHY_RF_PHY_INIT_COMPLETE (1 << 1)

// lpddr_rf_cfg_clock_gate
#define LPDDR_PHY_RF_CLK_GATE_EN (1 << 0)
#define LPDDR_PHY_RF_CLK_GATE_FG_EN (1 << 1)
#define LPDDR_PHY_RF_CLK_GATE_AG_EN (1 << 2)
#define LPDDR_PHY_RF_CLK_GATE_AG_WR_EN (1 << 3)
#define LPDDR_PHY_RF_CLK_GATE_AG_RD_EN (1 << 4)

// lpddr_rf_cfg_sample_resync
#define LPDDR_PHY_RF_SAMPLE_RESYNC (1 << 0)
#define LPDDR_PHY_RF_SAMPLE_RESYNC_AUTO_EN (1 << 1)
#define LPDDR_PHY_RF_DATA_RESYNC_SEL(n) (((n)&0xffff) << 16)

// lpddr_rf_cfg_lpi
#define LPDDR_PHY_RF_LPI_SEL_M0 (1 << 0)
#define LPDDR_PHY_RF_CWAKEUP_M0 (1 << 1)
#define LPDDR_PHY_RF_CWAKEUP_S0 (1 << 2)

// lpddr_rf_cfg_ext
#define LPDDR_PHY_RF_DATA_OE_EXT(n) (((n)&0xf) << 0)
#define LPDDR_PHY_RF_DATA_IE_EXT(n) (((n)&0xf) << 4)
#define LPDDR_PHY_RF_DQS_OE_EXT(n) (((n)&0xf) << 8)
#define LPDDR_PHY_RF_DQS_IE_EXT(n) (((n)&0xf) << 12)
#define LPDDR_PHY_RF_DQS_GATE_EXT(n) (((n)&0xf) << 16)
#define LPDDR_PHY_RF_DATA_RESYNC_EXT(n) (((n)&0xf) << 20)

// lpddr_rfdll_cfg_dll
#define LPDDR_PHY_RFDLL_RESET (1 << 0)

// lpddr_rfdll_status_cpst_idle
#define LPDDR_PHY_RFDL_CPST_ST_IDLE_AC (1 << 0)
#define LPDDR_PHY_RFDL_CPST_ST_IDLE_DS0 (1 << 1)
#define LPDDR_PHY_RFDL_CPST_ST_IDLE_DS1 (1 << 2)

// lpddr_rf_cfg_dll_ac
#define LPDDR_PHY_RF_DLL_CLR_AC (1 << 8)
#define LPDDR_PHY_RF_DLL_AUTO_CLR_EN_AC (1 << 9)
#define LPDDR_PHY_RF_DL_CPST_EN_AC (1 << 10)
#define LPDDR_PHY_RF_DL_CPST_START_AC (1 << 11)
#define LPDDR_PHY_RF_DL_CPST_AUTO_REF_EN_AC (1 << 12)
#define LPDDR_PHY_RF_DLL_ERR_CLR_AC (1 << 13)
#define LPDDR_PHY_RF_DLL_CLK_SEL_AC (1 << 14)
#define LPDDR_PHY_RF_DLL_EN_AC (1 << 15)
#define LPDDR_PHY_RF_DL_CPST_THR_AC(n) (((n)&0xff) << 16)
#define LPDDR_PHY_RF_DLL_PD_CNT_AC(n) (((n)&0x7) << 24)
#define LPDDR_PHY_RF_DLL_AUTO_ERR_CLR_EN_AC (1 << 27)
#define LPDDR_PHY_RF_DLL_LOCK_WAIT_AC(n) (((n)&0xf) << 28)

// lpddr_rfdll_status_dll_ac
#define LPDDR_PHY_RFDLL_CNT_AC(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_CPST_ST_AC (1 << 24)
#define LPDDR_PHY_RFDLL_ST_AC(n) (((n)&0x7) << 25)
#define LPDDR_PHY_RFDLL_LOCKED_AC (1 << 28)
#define LPDDR_PHY_RFDLL_ERROR_AC (1 << 29)

// lpddr_rf_cfg_dll_dl_0_wr_ac
#define LPDDR_PHY_RF_CLKWR_RAW_DL_SEL_AC(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_CLKWR_RAW_CNT_AC(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RF_CLKWR_RAW_DL_CPST_OFFSET_AC(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RF_CLKWR_QTR_DL_SEL_AC(n) (((n)&0x3) << 24)
#define LPDDR_PHY_RFDL_CLKWR_QTR_CNT_AC(n) (((n)&0x3) << 26)
#define LPDDR_PHY_RF_CLKWR_QTR_DL_CPST_OFFSET_AC(n) (((n)&0x3) << 28)
#define LPDDR_PHY_RF_CLKWR_DL_CPST_MINUS_AC (1 << 30)
#define LPDDR_PHY_RF_CLKWR_DL_CPST_EN_AC (1 << 31)

// lpddr_rf_cfg_dll_dl_1_wr_ac
#define LPDDR_PHY_RF_DLY_OUT_A0_DL_SEL(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_OUT_A1_DL_SEL(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_OUT_A2_DL_SEL(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_OUT_A3_DL_SEL(n) (((n)&0x1f) << 24)

// lpddr_rf_cfg_dll_dl_2_wr_ac
#define LPDDR_PHY_RF_DLY_OUT_A4_DL_SEL(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_OUT_A5_DL_SEL(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_OUT_A6_DL_SEL(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_OUT_A7_DL_SEL(n) (((n)&0x1f) << 24)

// lpddr_rf_cfg_dll_dl_3_wr_ac
#define LPDDR_PHY_RF_DLY_OUT_A8_DL_SEL(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_OUT_A9_DL_SEL(n) (((n)&0x1f) << 8)

// lpddr_rf_cfg_dll_dl_4_wr_ac
#define LPDDR_PHY_RF_DLY_OUT_CSN_DL_SEL(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_OUT_CKE_DL_SEL(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_OUT_CLK_DL_SEL(n) (((n)&0x1f) << 24)

// lpddr_rfdll_status_max_cnt_ac
#define LPDDR_PHY_RFDLL_MAX_CNT_F0_AC(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDLL_MAX_CNT_F1_AC(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RFDLL_MAX_CNT_F2_AC(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RFDLL_MAX_CNT_F3_AC(n) (((n)&0x7f) << 24)

// lpddr_rfdll_status_min_cnt_ac
#define LPDDR_PHY_RFDLL_MIN_CNT_F0_AC(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDLL_MIN_CNT_F1_AC(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RFDLL_MIN_CNT_F2_AC(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RFDLL_MIN_CNT_F3_AC(n) (((n)&0x7f) << 24)

// lpddr_rf_cfg_phy_iomux_sel_wr_ac
#define LPDDR_PHY_RF_PHY_IO_A0_SEL_AC (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_A1_SEL_AC (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_A2_SEL_AC (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_A3_SEL_AC (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_A4_SEL_AC (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_A5_SEL_AC (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_A6_SEL_AC (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_A7_SEL_AC (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_A8_SEL_AC (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_A9_SEL_AC (1 << 9)
#define LPDDR_PHY_RF_PHY_IO_CSN_SEL_AC (1 << 16)
#define LPDDR_PHY_RF_PHY_IO_CKE0_SEL_AC (1 << 20)
#define LPDDR_PHY_RF_PHY_IO_CKE1_SEL_AC (1 << 21)

// lpddr_rf_cfg_phy_iomux_ie_wr_ac
#define LPDDR_PHY_RF_PHY_IO_A0_IE_AC (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_A1_IE_AC (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_A2_IE_AC (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_A3_IE_AC (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_A4_IE_AC (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_A5_IE_AC (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_A6_IE_AC (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_A7_IE_AC (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_A8_IE_AC (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_A9_IE_AC (1 << 9)
#define LPDDR_PHY_RF_PHY_IO_CSN_IE_AC (1 << 16)
#define LPDDR_PHY_RF_PHY_IO_CKE0_IE_AC (1 << 20)
#define LPDDR_PHY_RF_PHY_IO_CKE1_IE_AC (1 << 21)

// lpddr_rf_cfg_phy_iomux_oe_wr_ac
#define LPDDR_PHY_RF_PHY_IO_A0_OE_AC (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_A1_OE_AC (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_A2_OE_AC (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_A3_OE_AC (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_A4_OE_AC (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_A5_OE_AC (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_A6_OE_AC (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_A7_OE_AC (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_A8_OE_AC (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_A9_OE_AC (1 << 9)
#define LPDDR_PHY_RF_PHY_IO_CSN_OE_AC (1 << 16)
#define LPDDR_PHY_RF_PHY_IO_CKE0_OE_AC (1 << 20)
#define LPDDR_PHY_RF_PHY_IO_CKE1_OE_AC (1 << 21)

// lpddr_rf_cfg_phy_iomux_out_wr_ac
#define LPDDR_PHY_RF_PHY_IO_A0_OUT_AC (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_A1_OUT_AC (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_A2_OUT_AC (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_A3_OUT_AC (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_A4_OUT_AC (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_A5_OUT_AC (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_A6_OUT_AC (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_A7_OUT_AC (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_A8_OUT_AC (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_A9_OUT_AC (1 << 9)
#define LPDDR_PHY_RF_PHY_IO_CSN_OUT_AC (1 << 16)
#define LPDDR_PHY_RF_PHY_IO_CKE0_OUT_AC (1 << 20)
#define LPDDR_PHY_RF_PHY_IO_CKE1_OUT_AC (1 << 21)

// lpddr_rf_cfg_dll_ds0
#define LPDDR_PHY_RF_DLL_CLR_DS0 (1 << 8)
#define LPDDR_PHY_RF_DLL_AUTO_CLR_EN_DS0 (1 << 9)
#define LPDDR_PHY_RF_DL_CPST_EN_DS0 (1 << 10)
#define LPDDR_PHY_RF_DL_CPST_START_DS0 (1 << 11)
#define LPDDR_PHY_RF_DL_CPST_AUTO_REF_EN_DS0 (1 << 12)
#define LPDDR_PHY_RF_DLL_ERR_CLR_DS0 (1 << 13)
#define LPDDR_PHY_RF_DLL_CLK_SEL_DS0 (1 << 14)
#define LPDDR_PHY_RF_DLL_EN_DS0 (1 << 15)
#define LPDDR_PHY_RF_DL_CPST_THR_DS0(n) (((n)&0xff) << 16)
#define LPDDR_PHY_RF_DLL_PD_CNT_DS0(n) (((n)&0x7) << 24)
#define LPDDR_PHY_RF_DLL_AUTO_ERR_CLR_EN_DS0 (1 << 27)
#define LPDDR_PHY_RF_DLL_LOCK_WAIT_DS0(n) (((n)&0xf) << 28)

// lpddr_rfdll_status_dll_ds0
#define LPDDR_PHY_RFDLL_CNT_DS0(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_CPST_ST_DS0 (1 << 24)
#define LPDDR_PHY_RFDLL_ST_DS0(n) (((n)&0x7) << 25)
#define LPDDR_PHY_RFDLL_LOCKED_DS0 (1 << 28)
#define LPDDR_PHY_RFDLL_ERROR_DS0 (1 << 29)

// lpddr_rf_cfg_dll_dl_0_wr_ds0
#define LPDDR_PHY_RF_CLKWR_RAW_DL_SEL_DS0(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_CLKWR_RAW_CNT_DS0(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RF_CLKWR_RAW_DL_CPST_OFFSET_DS0(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RF_CLKWR_QTR_DL_SEL_DS0(n) (((n)&0x3) << 24)
#define LPDDR_PHY_RFDL_CLKWR_QTR_CNT_DS0(n) (((n)&0x3) << 26)
#define LPDDR_PHY_RF_CLKWR_QTR_DL_CPST_OFFSET_DS0(n) (((n)&0x3) << 28)
#define LPDDR_PHY_RF_CLKWR_DL_CPST_MINUS_DS0 (1 << 30)
#define LPDDR_PHY_RF_CLKWR_DL_CPST_EN_DS0 (1 << 31)

// lpddr_rf_cfg_dll_dl_1_wr_ds0
#define LPDDR_PHY_RF_DQS_IN_POS_RAW_DL_SEL_DS0(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_DQS_IN_POS_RAW_CNT_DS0(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RF_DQS_IN_POS_RAW_DL_CPST_OFFSET_DS0(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RF_DQS_IN_POS_QTR_DL_SEL_DS0(n) (((n)&0x3) << 24)
#define LPDDR_PHY_RFDL_DQS_IN_POS_QTR_CNT_DS0(n) (((n)&0x3) << 26)
#define LPDDR_PHY_RF_DQS_IN_POS_QTR_DL_CPST_OFFSET_DS0(n) (((n)&0x3) << 28)
#define LPDDR_PHY_RF_DQS_IN_POS_DL_CPST_MINUS_DS0 (1 << 30)
#define LPDDR_PHY_RF_DQS_IN_POS_DL_CPST_EN_DS0 (1 << 31)

// lpddr_rf_cfg_dll_dl_2_wr_ds0
#define LPDDR_PHY_RF_DQS_IN_NEG_RAW_DL_SEL_DS0(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_DQS_IN_NEG_RAW_CNT_DS0(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RF_DQS_IN_NEG_RAW_DL_CPST_OFFSET_DS0(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RF_DQS_IN_NEG_QTR_DL_SEL_DS0(n) (((n)&0x3) << 24)
#define LPDDR_PHY_RFDL_DQS_IN_NEG_QTR_CNT_DS0(n) (((n)&0x3) << 26)
#define LPDDR_PHY_RF_DQS_IN_NEG_QTR_DL_CPST_OFFSET_DS0(n) (((n)&0x3) << 28)
#define LPDDR_PHY_RF_DQS_IN_NEG_DL_CPST_MINUS_DS0 (1 << 30)
#define LPDDR_PHY_RF_DQS_IN_NEG_DL_CPST_EN_DS0 (1 << 31)

// lpddr_rf_cfg_dll_dl_3_wr_ds0
#define LPDDR_PHY_RF_DQS_GATE_RAW_DL_SEL_DS0(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_DQS_GATE_RAW_CNT_DS0(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RF_DQS_GATE_RAW_DL_CPST_OFFSET_DS0(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RF_DQS_GATE_QTR_DL_SEL_DS0(n) (((n)&0x3) << 24)
#define LPDDR_PHY_RFDL_DQS_GATE_QTR_CNT_DS0(n) (((n)&0x3) << 26)
#define LPDDR_PHY_RF_DQS_GATE_QTR_DL_CPST_OFFSET_DS0(n) (((n)&0x3) << 28)
#define LPDDR_PHY_RF_DQS_GATE_DL_CPST_MINUS_DS0 (1 << 30)
#define LPDDR_PHY_RF_DQS_GATE_DL_CPST_EN_DS0 (1 << 31)

// lpddr_rf_cfg_dll_dl_4_wr_ds0
#define LPDDR_PHY_RF_DLY_OUT_D0_DL_SEL_DS0(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_OUT_D1_DL_SEL_DS0(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_OUT_D2_DL_SEL_DS0(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_OUT_D3_DL_SEL_DS0(n) (((n)&0x1f) << 24)

// lpddr_rf_cfg_dll_dl_5_wr_ds0
#define LPDDR_PHY_RF_DLY_OUT_D4_DL_SEL_DS0(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_OUT_D5_DL_SEL_DS0(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_OUT_D6_DL_SEL_DS0(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_OUT_D7_DL_SEL_DS0(n) (((n)&0x1f) << 24)

// lpddr_rf_cfg_dll_dl_6_wr_ds0
#define LPDDR_PHY_RF_DLY_IN_D0_DL_SEL_DS0(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_IN_D1_DL_SEL_DS0(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_IN_D2_DL_SEL_DS0(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_IN_D3_DL_SEL_DS0(n) (((n)&0x1f) << 24)

// lpddr_rf_cfg_dll_dl_7_wr_ds0
#define LPDDR_PHY_RF_DLY_IN_D4_DL_SEL_DS0(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_IN_D5_DL_SEL_DS0(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_IN_D6_DL_SEL_DS0(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_IN_D7_DL_SEL_DS0(n) (((n)&0x1f) << 24)

// lpddr_rf_cfg_dll_dl_8_wr_ds0
#define LPDDR_PHY_RF_DLY_OUT_DQS_DL_SEL_DS0(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_OUT_DQM_DL_SEL_DS0(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_IN_DQS_DL_SEL_DS0(n) (((n)&0x1f) << 16)

// lpddr_rfdll_status_max_cnt_ds0
#define LPDDR_PHY_RFDLL_MAX_CNT_F0_DS0(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDLL_MAX_CNT_F1_DS0(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RFDLL_MAX_CNT_F2_DS0(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RFDLL_MAX_CNT_F3_DS0(n) (((n)&0x7f) << 24)

// lpddr_rfdll_status_min_cnt_ds0
#define LPDDR_PHY_RFDLL_MIN_CNT_F0_DS0(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDLL_MIN_CNT_F1_DS0(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RFDLL_MIN_CNT_F2_DS0(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RFDLL_MIN_CNT_F3_DS0(n) (((n)&0x7f) << 24)

// lpddr_rf_cfg_phy_iomux_sel_wr_ds0
#define LPDDR_PHY_RF_PHY_IO_D0_SEL_DS0 (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_D1_SEL_DS0 (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_D2_SEL_DS0 (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_D3_SEL_DS0 (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_D4_SEL_DS0 (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_D5_SEL_DS0 (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_D6_SEL_DS0 (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_D7_SEL_DS0 (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_DQM_SEL_DS0 (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_DQS_SEL_DS0 (1 << 9)

// lpddr_rf_cfg_phy_iomux_ie_wr_ds0
#define LPDDR_PHY_RF_PHY_IO_D0_IE_DS0 (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_D1_IE_DS0 (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_D2_IE_DS0 (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_D3_IE_DS0 (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_D4_IE_DS0 (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_D5_IE_DS0 (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_D6_IE_DS0 (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_D7_IE_DS0 (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_DQM_IE_DS0 (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_DQS_IE_DS0 (1 << 9)

// lpddr_rf_cfg_phy_iomux_oe_wr_ds0
#define LPDDR_PHY_RF_PHY_IO_D0_OE_DS0 (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_D1_OE_DS0 (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_D2_OE_DS0 (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_D3_OE_DS0 (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_D4_OE_DS0 (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_D5_OE_DS0 (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_D6_OE_DS0 (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_D7_OE_DS0 (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_DQM_OE_DS0 (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_DQS_OE_DS0 (1 << 9)

// lpddr_rf_cfg_phy_iomux_out_wr_ds0
#define LPDDR_PHY_RF_PHY_IO_D0_OUT_DS0 (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_D1_OUT_DS0 (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_D2_OUT_DS0 (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_D3_OUT_DS0 (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_D4_OUT_DS0 (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_D5_OUT_DS0 (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_D6_OUT_DS0 (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_D7_OUT_DS0 (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_DQM_OUT_DS0 (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_DQS_OUT_DS0 (1 << 9)

// lpddr_rf_cfg_dll_ds1
#define LPDDR_PHY_RF_DLL_CLR_DS1 (1 << 8)
#define LPDDR_PHY_RF_DLL_AUTO_CLR_EN_DS1 (1 << 9)
#define LPDDR_PHY_RF_DL_CPST_EN_DS1 (1 << 10)
#define LPDDR_PHY_RF_DL_CPST_START_DS1 (1 << 11)
#define LPDDR_PHY_RF_DL_CPST_AUTO_REF_EN_DS1 (1 << 12)
#define LPDDR_PHY_RF_DLL_ERR_CLR_DS1 (1 << 13)
#define LPDDR_PHY_RF_DLL_CLK_SEL_DS1 (1 << 14)
#define LPDDR_PHY_RF_DLL_EN_DS1 (1 << 15)
#define LPDDR_PHY_RF_DL_CPST_THR_DS1(n) (((n)&0xff) << 16)
#define LPDDR_PHY_RF_DLL_PD_CNT_DS1(n) (((n)&0x7) << 24)
#define LPDDR_PHY_RF_DLL_AUTO_ERR_CLR_EN_DS1 (1 << 27)
#define LPDDR_PHY_RF_DLL_LOCK_WAIT_DS1(n) (((n)&0xf) << 28)

// lpddr_rfdll_status_dll_ds1
#define LPDDR_PHY_RFDLL_CNT_DS1(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_CPST_ST_DS1 (1 << 24)
#define LPDDR_PHY_RFDLL_ST_DS1(n) (((n)&0x7) << 25)
#define LPDDR_PHY_RFDLL_LOCKED_DS1 (1 << 28)
#define LPDDR_PHY_RFDLL_ERROR_DS1 (1 << 29)

// lpddr_rf_cfg_dll_dl_0_wr_ds1
#define LPDDR_PHY_RF_CLKWR_RAW_DL_SEL_DS1(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_CLKWR_RAW_CNT_DS1(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RF_CLKWR_RAW_DL_CPST_OFFSET_DS1(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RF_CLKWR_QTR_DL_SEL_DS1(n) (((n)&0x3) << 24)
#define LPDDR_PHY_RFDL_CLKWR_QTR_CNT_DS1(n) (((n)&0x3) << 26)
#define LPDDR_PHY_RF_CLKWR_QTR_DL_CPST_OFFSET_DS1(n) (((n)&0x3) << 28)
#define LPDDR_PHY_RF_CLKWR_DL_CPST_MINUS_DS1 (1 << 30)
#define LPDDR_PHY_RF_CLKWR_DL_CPST_EN_DS1 (1 << 31)

// lpddr_rf_cfg_dll_dl_1_wr_ds1
#define LPDDR_PHY_RF_DQS_IN_POS_RAW_DL_SEL_DS1(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_DQS_IN_POS_RAW_CNT_DS1(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RF_DQS_IN_POS_RAW_DL_CPST_OFFSET_DS1(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RF_DQS_IN_POS_QTR_DL_SEL_DS1(n) (((n)&0x3) << 24)
#define LPDDR_PHY_RFDL_DQS_IN_POS_QTR_CNT_DS1(n) (((n)&0x3) << 26)
#define LPDDR_PHY_RF_DQS_IN_POS_QTR_DL_CPST_OFFSET_DS1(n) (((n)&0x3) << 28)
#define LPDDR_PHY_RF_DQS_IN_POS_DL_CPST_MINUS_DS1 (1 << 30)
#define LPDDR_PHY_RF_DQS_IN_POS_DL_CPST_EN_DS1 (1 << 31)

// lpddr_rf_cfg_dll_dl_2_wr_ds1
#define LPDDR_PHY_RF_DQS_IN_NEG_RAW_DL_SEL_DS1(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_DQS_IN_NEG_RAW_CNT_DS1(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RF_DQS_IN_NEG_RAW_DL_CPST_OFFSET_DS1(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RF_DQS_IN_NEG_QTR_DL_SEL_DS1(n) (((n)&0x3) << 24)
#define LPDDR_PHY_RFDL_DQS_IN_NEG_QTR_CNT_DS1(n) (((n)&0x3) << 26)
#define LPDDR_PHY_RF_DQS_IN_NEG_QTR_DL_CPST_OFFSET_DS1(n) (((n)&0x3) << 28)
#define LPDDR_PHY_RF_DQS_IN_NEG_DL_CPST_MINUS_DS1 (1 << 30)
#define LPDDR_PHY_RF_DQS_IN_NEG_DL_CPST_EN_DS1 (1 << 31)

// lpddr_rf_cfg_dll_dl_3_wr_ds1
#define LPDDR_PHY_RF_DQS_GATE_RAW_DL_SEL_DS1(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDL_DQS_GATE_RAW_CNT_DS1(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RF_DQS_GATE_RAW_DL_CPST_OFFSET_DS1(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RF_DQS_GATE_QTR_DL_SEL_DS1(n) (((n)&0x3) << 24)
#define LPDDR_PHY_RFDL_DQS_GATE_QTR_CNT_DS1(n) (((n)&0x3) << 26)
#define LPDDR_PHY_RF_DQS_GATE_QTR_DL_CPST_OFFSET_DS1(n) (((n)&0x3) << 28)
#define LPDDR_PHY_RF_DQS_GATE_DL_CPST_MINUS_DS1 (1 << 30)
#define LPDDR_PHY_RF_DQS_GATE_DL_CPST_EN_DS1 (1 << 31)

// lpddr_rf_cfg_dll_dl_4_wr_ds1
#define LPDDR_PHY_RF_DLY_OUT_D0_DL_SEL_DS1(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_OUT_D1_DL_SEL_DS1(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_OUT_D2_DL_SEL_DS1(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_OUT_D3_DL_SEL_DS1(n) (((n)&0x1f) << 24)

// lpddr_rf_cfg_dll_dl_5_wr_ds1
#define LPDDR_PHY_RF_DLY_OUT_D4_DL_SEL_DS1(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_OUT_D5_DL_SEL_DS1(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_OUT_D6_DL_SEL_DS1(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_OUT_D7_DL_SEL_DS1(n) (((n)&0x1f) << 24)

// lpddr_rf_cfg_dll_dl_6_wr_ds1
#define LPDDR_PHY_RF_DLY_IN_D0_DL_SEL_DS1(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_IN_D1_DL_SEL_DS1(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_IN_D2_DL_SEL_DS1(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_IN_D3_DL_SEL_DS1(n) (((n)&0x1f) << 24)

// lpddr_rf_cfg_dll_dl_7_wr_ds1
#define LPDDR_PHY_RF_DLY_IN_D4_DL_SEL_DS1(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_IN_D5_DL_SEL_DS1(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_IN_D6_DL_SEL_DS1(n) (((n)&0x1f) << 16)
#define LPDDR_PHY_RF_DLY_IN_D7_DL_SEL_DS1(n) (((n)&0x1f) << 24)

// lpddr_rf_cfg_dll_dl_8_wr_ds1
#define LPDDR_PHY_RF_DLY_OUT_DQS_DL_SEL_DS1(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_RF_DLY_OUT_DQM_DL_SEL_DS1(n) (((n)&0x1f) << 8)
#define LPDDR_PHY_RF_DLY_IN_DQS_DL_SEL_DS1(n) (((n)&0x1f) << 16)

// lpddr_rfdll_status_max_cnt_ds1
#define LPDDR_PHY_RFDLL_MAX_CNT_F0_DS1(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDLL_MAX_CNT_F1_DS1(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RFDLL_MAX_CNT_F2_DS1(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RFDLL_MAX_CNT_F3_DS1(n) (((n)&0x7f) << 24)

// lpddr_rfdll_status_min_cnt_ds1
#define LPDDR_PHY_RFDLL_MIN_CNT_F0_DS1(n) (((n)&0x7f) << 0)
#define LPDDR_PHY_RFDLL_MIN_CNT_F1_DS1(n) (((n)&0x7f) << 8)
#define LPDDR_PHY_RFDLL_MIN_CNT_F2_DS1(n) (((n)&0x7f) << 16)
#define LPDDR_PHY_RFDLL_MIN_CNT_F3_DS1(n) (((n)&0x7f) << 24)

// lpddr_rf_cfg_phy_iomux_sel_wr_ds1
#define LPDDR_PHY_RF_PHY_IO_D0_SEL_DS1 (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_D1_SEL_DS1 (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_D2_SEL_DS1 (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_D3_SEL_DS1 (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_D4_SEL_DS1 (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_D5_SEL_DS1 (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_D6_SEL_DS1 (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_D7_SEL_DS1 (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_DQM_SEL_DS1 (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_DQS_SEL_DS1 (1 << 9)

// lpddr_rf_cfg_phy_iomux_ie_wr_ds1
#define LPDDR_PHY_RF_PHY_IO_D0_IE_DS1 (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_D1_IE_DS1 (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_D2_IE_DS1 (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_D3_IE_DS1 (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_D4_IE_DS1 (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_D5_IE_DS1 (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_D6_IE_DS1 (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_D7_IE_DS1 (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_DQM_IE_DS1 (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_DQS_IE_DS1 (1 << 9)

// lpddr_rf_cfg_phy_iomux_oe_wr_ds1
#define LPDDR_PHY_RF_PHY_IO_D0_OE_DS1 (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_D1_OE_DS1 (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_D2_OE_DS1 (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_D3_OE_DS1 (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_D4_OE_DS1 (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_D5_OE_DS1 (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_D6_OE_DS1 (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_D7_OE_DS1 (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_DQM_OE_DS1 (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_DQS_OE_DS1 (1 << 9)

// lpddr_rf_cfg_phy_iomux_out_wr_ds1
#define LPDDR_PHY_RF_PHY_IO_D0_OUT_DS1 (1 << 0)
#define LPDDR_PHY_RF_PHY_IO_D1_OUT_DS1 (1 << 1)
#define LPDDR_PHY_RF_PHY_IO_D2_OUT_DS1 (1 << 2)
#define LPDDR_PHY_RF_PHY_IO_D3_OUT_DS1 (1 << 3)
#define LPDDR_PHY_RF_PHY_IO_D4_OUT_DS1 (1 << 4)
#define LPDDR_PHY_RF_PHY_IO_D5_OUT_DS1 (1 << 5)
#define LPDDR_PHY_RF_PHY_IO_D6_OUT_DS1 (1 << 6)
#define LPDDR_PHY_RF_PHY_IO_D7_OUT_DS1 (1 << 7)
#define LPDDR_PHY_RF_PHY_IO_DQM_OUT_DS1 (1 << 8)
#define LPDDR_PHY_RF_PHY_IO_DQS_OUT_DS1 (1 << 9)

// lpddr_rf_status_phy_data_in
#define LPDDR_PHY_RF_PHY_DATA_IN(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg
#define LPDDR_PHY_DRF_CLKDMEM_OUT_SEL (1 << 0)

// lpddr_drf_cfg_reg_sel
#define LPDDR_PHY_DRF_REG_SEL(n) (((n)&0x3) << 0)

// lpddr_drf_cfg_dqs_ie_sel_f0
#define LPDDR_PHY_DRF_DQS_IE_SEL_F0(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_oe_sel_f0
#define LPDDR_PHY_DRF_DQS_OE_SEL_F0(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_out_sel_f0
#define LPDDR_PHY_DRF_DQS_OUT_SEL_F0(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_gate_sel_f0
#define LPDDR_PHY_DRF_DQS_GATE_SEL_F0(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_data_ie_sel_f0
#define LPDDR_PHY_DRF_DATA_IE_SEL_F0(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_data_oe_sel_f0
#define LPDDR_PHY_DRF_DATA_OE_SEL_F0(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_read_en_sel_f0
#define LPDDR_PHY_DRF_READ_EN_SEL_F0(n) (((n)&0xf) << 0)

// lpddr_drf_cfg_dqs_ie_sel_f1
#define LPDDR_PHY_DRF_DQS_IE_SEL_F1(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_oe_sel_f1
#define LPDDR_PHY_DRF_DQS_OE_SEL_F1(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_out_sel_f1
#define LPDDR_PHY_DRF_DQS_OUT_SEL_F1(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_gate_sel_f1
#define LPDDR_PHY_DRF_DQS_GATE_SEL_F1(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_data_ie_sel_f1
#define LPDDR_PHY_DRF_DATA_IE_SEL_F1(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_data_oe_sel_f1
#define LPDDR_PHY_DRF_DATA_OE_SEL_F1(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_read_en_sel_f1
#define LPDDR_PHY_DRF_READ_EN_SEL_F1(n) (((n)&0xf) << 0)

// lpddr_drf_cfg_dqs_ie_sel_f2
#define LPDDR_PHY_DRF_DQS_IE_SEL_F2(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_oe_sel_f2
#define LPDDR_PHY_DRF_DQS_OE_SEL_F2(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_out_sel_f2
#define LPDDR_PHY_DRF_DQS_OUT_SEL_F2(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_gate_sel_f2
#define LPDDR_PHY_DRF_DQS_GATE_SEL_F2(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_data_ie_sel_f2
#define LPDDR_PHY_DRF_DATA_IE_SEL_F2(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_data_oe_sel_f2
#define LPDDR_PHY_DRF_DATA_OE_SEL_F2(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_read_en_sel_f2
#define LPDDR_PHY_DRF_READ_EN_SEL_F2(n) (((n)&0xf) << 0)

// lpddr_drf_cfg_dqs_ie_sel_f3
#define LPDDR_PHY_DRF_DQS_IE_SEL_F3(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_oe_sel_f3
#define LPDDR_PHY_DRF_DQS_OE_SEL_F3(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_out_sel_f3
#define LPDDR_PHY_DRF_DQS_OUT_SEL_F3(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_dqs_gate_sel_f3
#define LPDDR_PHY_DRF_DQS_GATE_SEL_F3(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_data_ie_sel_f3
#define LPDDR_PHY_DRF_DATA_IE_SEL_F3(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_data_oe_sel_f3
#define LPDDR_PHY_DRF_DATA_OE_SEL_F3(n) (((n)&0xffff) << 0)

// lpddr_drf_cfg_read_en_sel_f3
#define LPDDR_PHY_DRF_READ_EN_SEL_F3(n) (((n)&0xf) << 0)

// lpddr_drf_cfg_dll_mode_f0
#define LPDDR_PHY_DRF_DLL_CLK_MODE_F0 (1 << 0)
#define LPDDR_PHY_DRF_DLL_HALF_MODE_F0 (1 << 1)
#define LPDDR_PHY_DRF_DLL_SATU_MODE_F0 (1 << 2)

// lpddr_drf_cfg_dll_cnt_f0
#define LPDDR_PHY_DRF_DLL_INIT_CNT_F0(n) (((n)&0x3ff) << 0)
#define LPDDR_PHY_DRF_DLL_SATU_CNT_F0(n) (((n)&0x3ff) << 10)
#define LPDDR_PHY_DRF_DLL_AUTO_CNT_F0(n) (((n)&0x3ff) << 20)

// lpddr_drf_cfg_dll_mode_f1
#define LPDDR_PHY_DRF_DLL_CLK_MODE_F1 (1 << 0)
#define LPDDR_PHY_DRF_DLL_HALF_MODE_F1 (1 << 1)
#define LPDDR_PHY_DRF_DLL_SATU_MODE_F1 (1 << 2)

// lpddr_drf_cfg_dll_cnt_f1
#define LPDDR_PHY_DRF_DLL_INIT_CNT_F1(n) (((n)&0x3ff) << 0)
#define LPDDR_PHY_DRF_DLL_SATU_CNT_F1(n) (((n)&0x3ff) << 10)
#define LPDDR_PHY_DRF_DLL_AUTO_CNT_F1(n) (((n)&0x3ff) << 20)

// lpddr_drf_cfg_dll_mode_f2
#define LPDDR_PHY_DRF_DLL_CLK_MODE_F2 (1 << 0)
#define LPDDR_PHY_DRF_DLL_HALF_MODE_F2 (1 << 1)
#define LPDDR_PHY_DRF_DLL_SATU_MODE_F2 (1 << 2)

// lpddr_drf_cfg_dll_cnt_f2
#define LPDDR_PHY_DRF_DLL_INIT_CNT_F2(n) (((n)&0x3ff) << 0)
#define LPDDR_PHY_DRF_DLL_SATU_CNT_F2(n) (((n)&0x3ff) << 10)
#define LPDDR_PHY_DRF_DLL_AUTO_CNT_F2(n) (((n)&0x3ff) << 20)

// lpddr_drf_cfg_dll_mode_f3
#define LPDDR_PHY_DRF_DLL_CLK_MODE_F3 (1 << 0)
#define LPDDR_PHY_DRF_DLL_HALF_MODE_F3 (1 << 1)
#define LPDDR_PHY_DRF_DLL_SATU_MODE_F3 (1 << 2)

// lpddr_drf_cfg_dll_cnt_f3
#define LPDDR_PHY_DRF_DLL_INIT_CNT_F3(n) (((n)&0x3ff) << 0)
#define LPDDR_PHY_DRF_DLL_SATU_CNT_F3(n) (((n)&0x3ff) << 10)
#define LPDDR_PHY_DRF_DLL_AUTO_CNT_F3(n) (((n)&0x3ff) << 20)

// lpddr_drf_train_cfg
#define LPDDR_PHY_DRF_PHYUPD_EN (1 << 0)
#define LPDDR_PHY_DRF_PHYUPD_TYPE_SEL(n) (((n)&0x3) << 1)
#define LPDDR_PHY_DRF_PHYUPD_TYPE_0(n) (((n)&0x3) << 4)
#define LPDDR_PHY_DRF_PHYUPD_TYPE_1(n) (((n)&0x3) << 6)
#define LPDDR_PHY_DRF_PHYUPD_TYPE_2(n) (((n)&0x3) << 8)
#define LPDDR_PHY_DRF_PHYUPD_TYPE_3(n) (((n)&0x3) << 10)
#define LPDDR_PHY_DRF_PHY_WRLVL_EN (1 << 12)
#define LPDDR_PHY_DRF_DMC_WRLVL_EN (1 << 13)
#define LPDDR_PHY_DRF_PHY_RDLVL_EN (1 << 16)
#define LPDDR_PHY_DRF_DMC_RDLVL_EN (1 << 17)
#define LPDDR_PHY_DRF_PHY_RDLVL_GATE_EN (1 << 20)
#define LPDDR_PHY_DRF_DMC_RDLVL_GATE_EN (1 << 21)

// lpddr_drf_mr_data_0
#define LPDDR_PHY_DRF_MR_DATA_0(n) (((n)&0xffffffff) << 0)

// lpddr_drf_mr_data_1
#define LPDDR_PHY_DRF_MR_DATA_1(n) (((n)&0xffffffff) << 0)

// lpddr_rf_irq_ctrl
#define LPDDR_PHY_RF_IRQ_EN_DLL_UNLOCK_AC (1 << 0)
#define LPDDR_PHY_RF_IRQ_EN_DLL_UNLOCK_DS0 (1 << 1)
#define LPDDR_PHY_RF_IRQ_EN_DLL_UNLOCK_DS1 (1 << 2)

// lpddr_rf_irq_status_clr
#define LPDDR_PHY_RF_IRQ_ST_CLR_DLL_UNLOCK_AC (1 << 0)
#define LPDDR_PHY_RF_IRQ_ST_CLR_DLL_UNLOCK_DS0 (1 << 1)
#define LPDDR_PHY_RF_IRQ_ST_CLR_DLL_UNLOCK_DS1 (1 << 2)

// lpddr_rf_irq_status
#define LPDDR_PHY_RF_IRQ_ST_DLL_UNLOCK_AC (1 << 0)
#define LPDDR_PHY_RF_IRQ_ST_DLL_UNLOCK_DS0 (1 << 1)
#define LPDDR_PHY_RF_IRQ_ST_DLL_UNLOCK_DS1 (1 << 2)

// lpddr_rf_irq_cnt_clr
#define LPDDR_PHY_RF_IRQ_CNT_CLR_DLL_UNLOCK_AC (1 << 0)
#define LPDDR_PHY_RF_IRQ_CNT_CLR_DLL_UNLOCK_DS0 (1 << 1)
#define LPDDR_PHY_RF_IRQ_CNT_CLR_DLL_UNLOCK_DS1 (1 << 2)

// lpddr_rf_irq_cnt_dll_unlock_ac
#define LPDDR_PHY_RF_IRQ_CNT_DLL_UNLOCK_AC(n) (((n)&0x7fffffff) << 0)
#define LPDDR_PHY_RF_IRQ_CNT_OVERFLOW_DLL_UNLOCK_AC (1 << 31)

// lpddr_rf_irq_cnt_dll_unlock_ds0
#define LPDDR_PHY_RF_IRQ_CNT_DLL_UNLOCK_DS0(n) (((n)&0x7fffffff) << 0)
#define LPDDR_PHY_RF_IRQ_CNT_OVERFLOW_DLL_UNLOCK_DS0 (1 << 31)

// lpddr_rf_irq_cnt_dll_unlock_ds1
#define LPDDR_PHY_RF_IRQ_CNT_DLL_UNLOCK_DS1(n) (((n)&0x7fffffff) << 0)
#define LPDDR_PHY_RF_IRQ_CNT_OVERFLOW_DLL_UNLOCK_DS1 (1 << 31)

// io_rf_ddr_rpull_cfg
#define LPDDR_PHY_DDR_DQS_RPULL(n) (((n)&0x3) << 0)
#define LPDDR_PHY_DDR_CK_RPULL(n) (((n)&0x3) << 2)

// io_rf_ddr_drv_cfg
#define LPDDR_PHY_DDR_DRVPD(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_DDR_DRVPC(n) (((n)&0x1f) << 5)
#define LPDDR_PHY_DDR_DRVND(n) (((n)&0x1f) << 10)
#define LPDDR_PHY_DDR_DRVNC(n) (((n)&0x1f) << 15)

// io_rf_ddr_pad_cfg
#define LPDDR_PHY_DDR_LP2D (1 << 0)
#define LPDDR_PHY_DDR_LP2C (1 << 1)

// io_rf_ddr_vref_cfg
#define LPDDR_PHY_DDR_VREF_EN_PAD (1 << 0)
#define LPDDR_PHY_DDR_VREF_EN_INT (1 << 1)
#define LPDDR_PHY_DDR_SEL_PAD(n) (((n)&0x3f) << 2)
#define LPDDR_PHY_DDR_SEL_INT(n) (((n)&0x3f) << 8)

// io_rf_ddr_zq_cfg
#define LPDDR_PHY_DDR_ZPROG(n) (((n)&0x1f) << 0)
#define LPDDR_PHY_DDR_CAL_ZQ_PD (1 << 5)
#define LPDDR_PHY_DDR_CAL_ZQ_CAL (1 << 6)

// io_rf_ddr_zq_mon
#define LPDDR_PHY_DDR_CALOVER (1 << 0)

// io_rf_ddr_odt_cfg
#define LPDDR_PHY_DDR_ODT_TO_PAD (1 << 0)
#define LPDDR_PHY_DDR_ODT_WRITE_ENABLE (1 << 1)
#define LPDDR_PHY_DDR_ODT_READ_ENABLE (1 << 2)

// io_rf_ddr_reserved
#define LPDDR_PHY_DDR_REG_RESV(n) (((n)&0xffffffff) << 0)

// io_rf_psram_drv_cfg
#define LPDDR_PHY_PSRAM_FIX_READ0 (1 << 0)
#define LPDDR_PHY_PSRAM_SLEWRATE(n) (((n)&0x3) << 1)
#define LPDDR_PHY_PSRAM_DRVP(n) (((n)&0x1f) << 3)
#define LPDDR_PHY_PSRAM_DRVN(n) (((n)&0x1f) << 8)

// io_rf_psram_pad_en_cfg
#define LPDDR_PHY_PSRAM_PAD_CLKN_EN (1 << 0)

// io_rf_psram_pull_cfg
#define LPDDR_PHY_PSRAM_DQS_PULL0_BIT(n) (((n)&0x3) << 0)
#define LPDDR_PHY_PSRAM_DQ_PULL0_BIT(n) (((n)&0x3) << 2)
#define LPDDR_PHY_PSRAM_DM_PULL1_BIT(n) (((n)&0x3) << 4)
#define LPDDR_PHY_PSRAM_CLKN_PULL1_BIT(n) (((n)&0x3) << 6)
#define LPDDR_PHY_PSRAM_CLK_PULL0_BIT(n) (((n)&0x3) << 8)
#define LPDDR_PHY_PSRAM_CEN_PULL1_BIT(n) (((n)&0x3) << 10)

// io_rf_psram_reserved
#define LPDDR_PHY_PSRAM_REG_RESV(n) (((n)&0xffffffff) << 0)

#endif // _LPDDR_PHY_H_
