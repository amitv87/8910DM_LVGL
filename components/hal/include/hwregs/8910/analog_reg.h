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

#ifndef _ANALOG_REG_H_
#define _ANALOG_REG_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_ANALOG_REG_BASE (0x05109000)
#else
#define REG_ANALOG_REG_BASE (0x50109000)
#endif

typedef volatile struct
{
    uint32_t apll_reg0;                // 0x00000000
    uint32_t apll_reg1;                // 0x00000004
    uint32_t apll_reg2;                // 0x00000008
    uint32_t sdm_apll_reg0;            // 0x0000000c
    uint32_t sdm_apll_reg1;            // 0x00000010
    uint32_t sdm_apll_reg2;            // 0x00000014
    uint32_t sdm_apll_reg3;            // 0x00000018
    uint32_t sdm_apll_reg4;            // 0x0000001c
    uint32_t __32[8];                  // 0x00000020
    uint32_t mempll_reg0;              // 0x00000040
    uint32_t mempll_reg1;              // 0x00000044
    uint32_t mempll_reg2;              // 0x00000048
    uint32_t sdm_mempll_reg0;          // 0x0000004c
    uint32_t sdm_mempll_reg1;          // 0x00000050
    uint32_t sdm_mempll_reg2;          // 0x00000054
    uint32_t sdm_mempll_reg3;          // 0x00000058
    uint32_t sdm_mempll_reg4;          // 0x0000005c
    uint32_t __96[8];                  // 0x00000060
    uint32_t usb_reg0;                 // 0x00000080
    uint32_t usb_reg1;                 // 0x00000084
    uint32_t usb_reg2;                 // 0x00000088
    uint32_t usb_reg3;                 // 0x0000008c
    uint32_t usb_reg4;                 // 0x00000090
    uint32_t usb_pll1;                 // 0x00000094
    uint32_t usb_pll2;                 // 0x00000098
    uint32_t usb_suspend;              // 0x0000009c
    uint32_t usb11_reg;                // 0x000000a0
    uint32_t usb_mon;                  // 0x000000a4
    uint32_t usb_reserved;             // 0x000000a8
    uint32_t sdm_usbpll_reg0;          // 0x000000ac
    uint32_t sdm_usbpll_reg1;          // 0x000000b0
    uint32_t sdm_usbpll_reg2;          // 0x000000b4
    uint32_t sdm_usbpll_reg3;          // 0x000000b8
    uint32_t sdm_usbpll_reg4;          // 0x000000bc
    uint32_t usb_digphy_ana1;          // 0x000000c0
    uint32_t usb_digphy_ana2;          // 0x000000c4
    uint32_t usb_digphy_ana3;          // 0x000000c8
    uint32_t usb_digphy_ana4;          // 0x000000cc
    uint32_t usb11_digphy_ana_reg;     // 0x000000d0
    uint32_t __212[11];                // 0x000000d4
    uint32_t ddr_pad_cfg;              // 0x00000100
    uint32_t psram_pad_cfg;            // 0x00000104
    uint32_t __264[14];                // 0x00000108
    uint32_t sdm_dsipll_reg0;          // 0x00000140
    uint32_t sdm_dsipll_reg1;          // 0x00000144
    uint32_t sdm_dsipll_reg2;          // 0x00000148
    uint32_t sdm_dsipll_reg3;          // 0x0000014c
    uint32_t sdm_dsipll_reg4;          // 0x00000150
    uint32_t mipi_lvds_phy_reg;        // 0x00000154
    uint32_t __344[10];                // 0x00000158
    uint32_t pad_ctrl_resv;            // 0x00000180
    uint32_t pad_resetb_cfg;           // 0x00000184
    uint32_t pad_adi_cfg;              // 0x00000188
    uint32_t pad_ap_jtag_cfg;          // 0x0000018c
    uint32_t pad_spi_camera_cfg;       // 0x00000190
    uint32_t pad_camera_cfg;           // 0x00000194
    uint32_t pad_debug_host_cfg;       // 0x00000198
    uint32_t pad_gpio_cfg1;            // 0x0000019c
    uint32_t pad_gpio_cfg2;            // 0x000001a0
    uint32_t pad_gpo_cfg;              // 0x000001a4
    uint32_t pad_i2c_cfg;              // 0x000001a8
    uint32_t pad_aud_ad_cfg;           // 0x000001ac
    uint32_t pad_keyin_cfg;            // 0x000001b0
    uint32_t pad_keyout_cfg;           // 0x000001b4
    uint32_t pad_lcd_cfg;              // 0x000001b8
    uint32_t pad_gpio13_cfg;           // 0x000001bc
    uint32_t pad_rfdig_gpio_cfg1;      // 0x000001c0
    uint32_t pad_rfdig_gpio_cfg2;      // 0x000001c4
    uint32_t pad_sdmmc1_others_cfg;    // 0x000001c8
    uint32_t pad_sdmmc1_data_cfg;      // 0x000001cc
    uint32_t pad_sim_1_cfg;            // 0x000001d0
    uint32_t pad_sim_2_cfg;            // 0x000001d4
    uint32_t pad_spi_flash_cfg1;       // 0x000001d8
    uint32_t pad_spi_flash_cfg2;       // 0x000001dc
    uint32_t pad_spi_lcd_cfg1;         // 0x000001e0
    uint32_t pad_spi_lcd_cfg2;         // 0x000001e4
    uint32_t pad_tst_cfg;              // 0x000001e8
    uint32_t pad_uart_1_cfg;           // 0x000001ec
    uint32_t pad_uart_2_cfg;           // 0x000001f0
    uint32_t pad_secure_boot_mode_cfg; // 0x000001f4
    uint32_t __504[2];                 // 0x000001f8
    uint32_t resv0;                    // 0x00000200
    uint32_t resv1;                    // 0x00000204
    uint32_t resv2;                    // 0x00000208
    uint32_t resv3;                    // 0x0000020c
    uint32_t resv4;                    // 0x00000210
    uint32_t resv5;                    // 0x00000214
    uint32_t resv6;                    // 0x00000218
    uint32_t resv7;                    // 0x0000021c
    uint32_t resv8;                    // 0x00000220
    uint32_t resv9;                    // 0x00000224
    uint32_t resv10;                   // 0x00000228
    uint32_t resv11;                   // 0x0000022c
    uint32_t resv12;                   // 0x00000230
    uint32_t resv13;                   // 0x00000234
    uint32_t resv14;                   // 0x00000238
    uint32_t resv15;                   // 0x0000023c
} HWP_ANALOG_REG_T;

#define hwp_analogReg ((HWP_ANALOG_REG_T *)REG_ACCESS_ADDRESS(REG_ANALOG_REG_BASE))

// apll_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t apll_clk_ap_en : 1;  // [0]
        uint32_t apll_lp_mode_en : 1; // [1]
        uint32_t apll_lock : 1;       // [2], read only
        uint32_t __31_3 : 29;         // [31:3]
    } b;
} REG_ANALOG_REG_APLL_REG0_T;

// apll_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t apll_refmulti2_en : 1; // [0]
        uint32_t apll_pcon_mode : 1;    // [1]
        uint32_t apll_cpr2_ibit : 3;    // [4:2]
        uint32_t apll_cpc2_ibit : 3;    // [7:5]
        uint32_t apll_cpbias_ibit : 3;  // [10:8]
        uint32_t apll_reg_res_bit : 2;  // [12:11]
        uint32_t apll_vreg_bit : 4;     // [16:13]
        uint32_t apll_pfd_dly_num : 3;  // [19:17]
        uint32_t __31_20 : 12;          // [31:20]
    } b;
} REG_ANALOG_REG_APLL_REG1_T;

// apll_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t apll_vco_low_test : 1;    // [0]
        uint32_t apll_vco_high_test : 1;   // [1]
        uint32_t apll_sdm_clk_test_en : 1; // [2]
        uint32_t apll_test_en : 1;         // [3]
        uint32_t __31_4 : 28;              // [31:4]
    } b;
} REG_ANALOG_REG_APLL_REG2_T;

// sdm_apll_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_apll_int_dec_sel : 3;     // [2:0]
        uint32_t sdm_apll_pu_pll_reg : 1;      // [3]
        uint32_t sdm_apll_pu_pll_dr : 1;       // [4]
        uint32_t sdm_apll_sdm_clk_sel_nor : 1; // [5]
        uint32_t sdm_apll_sdm_clk_sel_rst : 1; // [6]
        uint32_t sdm_apll_pu : 1;              // [7]
        uint32_t sdm_apll_fbc_inv : 1;         // [8]
        uint32_t __31_9 : 23;                  // [31:9]
    } b;
} REG_ANALOG_REG_SDM_APLL_REG0_T;

// sdm_apll_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_apll_lock_counter_sel : 2;      // [1:0]
        uint32_t sdm_apll_clkout_en_counter_sel : 2; // [3:2]
        uint32_t sdm_apll_clk_gen_en_reg : 1;        // [4]
        uint32_t sdm_apll_sdmclk_sel_time_sel : 2;   // [6:5]
        uint32_t sdm_apll_sdm_reset_time_sel : 2;    // [8:7]
        uint32_t sdm_apll_sdm_resetn_reg : 1;        // [9]
        uint32_t sdm_apll_sdm_resetn_dr : 1;         // [10]
        uint32_t sdm_apll_dither_bypass : 1;         // [11]
        uint32_t __31_12 : 20;                       // [31:12]
    } b;
} REG_ANALOG_REG_SDM_APLL_REG2_T;

// sdm_apll_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_apll_clk_ready : 1;   // [0], read only
        uint32_t sdm_apll_lock_steady : 1; // [1], read only
        uint32_t sdm_apll_sdm_resetn : 1;  // [2], read only
        uint32_t __31_3 : 29;              // [31:3]
    } b;
} REG_ANALOG_REG_SDM_APLL_REG3_T;

// sdm_apll_reg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_apll_ss_devi_step : 16; // [15:0]
        uint32_t sdm_apll_ss_devi : 12;      // [27:16]
        uint32_t sdm_apll_ss_en : 1;         // [28]
        uint32_t sdm_apll_sdm_en : 1;        // [29]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_ANALOG_REG_SDM_APLL_REG4_T;

// mempll_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mempll_clk_mem_en : 1; // [0]
        uint32_t mempll_lp_mode_en : 1; // [1]
        uint32_t mempll_lock : 1;       // [2], read only
        uint32_t __31_3 : 29;           // [31:3]
    } b;
} REG_ANALOG_REG_MEMPLL_REG0_T;

// mempll_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mempll_refmulti2_en : 1; // [0]
        uint32_t mempll_pcon_mode : 1;    // [1]
        uint32_t mempll_cpr2_ibit : 3;    // [4:2]
        uint32_t mempll_cpc2_ibit : 3;    // [7:5]
        uint32_t mempll_cpbias_ibit : 3;  // [10:8]
        uint32_t mempll_reg_res_bit : 2;  // [12:11]
        uint32_t mempll_vreg_bit : 4;     // [16:13]
        uint32_t mempll_pfd_dly_num : 3;  // [19:17]
        uint32_t __31_20 : 12;            // [31:20]
    } b;
} REG_ANALOG_REG_MEMPLL_REG1_T;

// mempll_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mempll_vco_low_test : 1;    // [0]
        uint32_t mempll_vco_high_test : 1;   // [1]
        uint32_t mempll_sdm_clk_test_en : 1; // [2]
        uint32_t mempll_test_en : 1;         // [3]
        uint32_t __31_4 : 28;                // [31:4]
    } b;
} REG_ANALOG_REG_MEMPLL_REG2_T;

// sdm_mempll_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_mempll_int_dec_sel : 3;     // [2:0]
        uint32_t sdm_mempll_pu_pll_reg : 1;      // [3]
        uint32_t sdm_mempll_pu_pll_dr : 1;       // [4]
        uint32_t sdm_mempll_sdm_clk_sel_nor : 1; // [5]
        uint32_t sdm_mempll_sdm_clk_sel_rst : 1; // [6]
        uint32_t sdm_mempll_pu : 1;              // [7]
        uint32_t sdm_mempll_fbc_inv : 1;         // [8]
        uint32_t __31_9 : 23;                    // [31:9]
    } b;
} REG_ANALOG_REG_SDM_MEMPLL_REG0_T;

// sdm_mempll_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_mempll_lock_counter_sel : 2;      // [1:0]
        uint32_t sdm_mempll_clkout_en_counter_sel : 2; // [3:2]
        uint32_t sdm_mempll_clk_gen_en_reg : 1;        // [4]
        uint32_t sdm_mempll_sdmclk_sel_time_sel : 2;   // [6:5]
        uint32_t sdm_mempll_sdm_reset_time_sel : 2;    // [8:7]
        uint32_t sdm_mempll_sdm_resetn_reg : 1;        // [9]
        uint32_t sdm_mempll_sdm_resetn_dr : 1;         // [10]
        uint32_t sdm_mempll_dither_bypass : 1;         // [11]
        uint32_t __31_12 : 20;                         // [31:12]
    } b;
} REG_ANALOG_REG_SDM_MEMPLL_REG2_T;

// sdm_mempll_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_mempll_clk_ready : 1;   // [0], read only
        uint32_t sdm_mempll_lock_steady : 1; // [1], read only
        uint32_t sdm_mempll_sdm_resetn : 1;  // [2], read only
        uint32_t __31_3 : 29;                // [31:3]
    } b;
} REG_ANALOG_REG_SDM_MEMPLL_REG3_T;

// sdm_mempll_reg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_mempll_ss_devi_step : 16; // [15:0]
        uint32_t sdm_mempll_ss_devi : 12;      // [27:16]
        uint32_t sdm_mempll_ss_en : 1;         // [28]
        uint32_t sdm_mempll_sdm_en : 1;        // [29]
        uint32_t __31_30 : 2;                  // [31:30]
    } b;
} REG_ANALOG_REG_SDM_MEMPLL_REG4_T;

// usb_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_cdr_gain : 4;         // [3:0]
        uint32_t usb_phase_sel : 2;        // [5:4]
        uint32_t usb_updn_mode : 1;        // [6]
        uint32_t usb_rst_intp_enable : 1;  // [7]
        uint32_t usb_cdr_clk_edge_bit : 1; // [8]
        uint32_t usb_tx_rstn : 1;          // [9]
        uint32_t __31_10 : 22;             // [31:10]
    } b;
} REG_ANALOG_REG_USB_REG0_T;

// usb_reg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_hs_lvlout_bit : 4; // [3:0]
        uint32_t usb_v125m_sel_bit : 3; // [6:4]
        uint32_t usb_v575m_sel_bit : 3; // [9:7]
        uint32_t usb_vref_vbit : 3;     // [12:10]
        uint32_t usb_vref_ibit : 2;     // [14:13]
        uint32_t __31_15 : 17;          // [31:15]
    } b;
} REG_ANALOG_REG_USB_REG1_T;

// usb_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_pu_lptx : 1;           // [0]
        uint32_t usb_pu_hstx : 1;           // [1]
        uint32_t usb_pu_hsrx : 1;           // [2]
        uint32_t usb_pu_iref : 1;           // [3]
        uint32_t usb_pu_1v2 : 1;            // [4]
        uint32_t usb_pu_1v8 : 1;            // [5]
        uint32_t usb_pu_pkd : 1;            // [6]
        uint32_t usb_pwr_on : 1;            // [7]
        uint32_t usb_pu_usb_dev : 1;        // [8]
        uint32_t usb_vbusvld_bit : 1;       // [9]
        uint32_t usb_pu_otg : 1;            // [10]
        uint32_t usb_vbg_sel : 1;           // [11]
        uint32_t usb_squelch_mode_sel : 1;  // [12]
        uint32_t usb_discnnxt_mode_sel : 1; // [13]
        uint32_t usb_squelch_aux : 1;       // [14]
        uint32_t __31_15 : 17;              // [31:15]
    } b;
} REG_ANALOG_REG_USB_REG2_T;

// usb_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_pattern : 10;   // [9:0]
        uint32_t usb_en_pattern : 1; // [10]
        uint32_t usb_loopback : 1;   // [11]
        uint32_t usb_det_en : 1;     // [12]
        uint32_t __31_13 : 19;       // [31:13]
    } b;
} REG_ANALOG_REG_USB_REG3_T;

// usb_reg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_lptx_drv_sel : 8; // [7:0]
        uint32_t usb_res_term_bit : 2; // [9:8]
        uint32_t usb_io33_enable : 1;  // [10]
        uint32_t __31_11 : 21;         // [31:11]
    } b;
} REG_ANALOG_REG_USB_REG4_T;

// usb_pll1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_pll_cpr2_ibit : 3;   // [2:0]
        uint32_t usb_pll_cpc2_ibit : 3;   // [5:3]
        uint32_t usb_pll_cpbias_ibit : 3; // [8:6]
        uint32_t usb_reg_res_bit : 2;     // [10:9]
        uint32_t usb_pll_vreg_bit : 4;    // [14:11]
        uint32_t __31_15 : 17;            // [31:15]
    } b;
} REG_ANALOG_REG_USB_PLL1_T;

// usb_pll2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_pll_clk_960m_en : 1; // [0]
        uint32_t usb_vco_low_test : 1;    // [1]
        uint32_t usb_vco_high_test : 1;   // [2]
        uint32_t usb_sdm_clk_test_en : 1; // [3]
        uint32_t usb_pll_test_en : 1;     // [4]
        uint32_t usb_refmulti2_en : 1;    // [5]
        uint32_t usb_pcon_mode : 1;       // [6]
        uint32_t __31_7 : 25;             // [31:7]
    } b;
} REG_ANALOG_REG_USB_PLL2_T;

// usb_suspend
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_pu_hstx_suspend : 1;   // [0]
        uint32_t usb_pu_hsrx_suspend : 1;   // [1]
        uint32_t usb_pu_iref_suspend : 1;   // [2]
        uint32_t usb_pu_pkd_suspend : 1;    // [3]
        uint32_t usb_pu_otg_suspend : 1;    // [4]
        uint32_t usb_pu_suspend_bypass : 1; // [5]
        uint32_t __31_6 : 26;               // [31:6]
    } b;
} REG_ANALOG_REG_USB_SUSPEND_T;

// usb11_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb11_lptx_drvsel : 8;    // [7:0]
        uint32_t usb11_io_bias_en : 1;     // [8]
        uint32_t usb11_pu_usb : 1;         // [9]
        uint32_t usb11_usb_ctrl : 3;       // [12:10]
        uint32_t usb11_usb_standby : 1;    // [13]
        uint32_t usb11_suspend_bypass : 1; // [14]
        uint32_t __31_15 : 17;             // [31:15]
    } b;
} REG_ANALOG_REG_USB11_REG_T;

// usb_mon
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_pll_lock : 1; // [0], read only
        uint32_t usb_lock : 1;     // [1], read only
        uint32_t usb_correct : 1;  // [2], read only
        uint32_t usb_dm_chr : 1;   // [3], read only
        uint32_t usb_dp_chr : 1;   // [4], read only
        uint32_t __31_5 : 27;      // [31:5]
    } b;
} REG_ANALOG_REG_USB_MON_T;

// usb_reserved
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_reg_resv : 16; // [15:0]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_ANALOG_REG_USB_RESERVED_T;

// sdm_usbpll_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_usbpll_int_dec_sel : 3;     // [2:0]
        uint32_t sdm_usbpll_pu_pll_reg : 1;      // [3]
        uint32_t sdm_usbpll_pu_pll_dr : 1;       // [4]
        uint32_t sdm_usbpll_sdm_clk_sel_nor : 1; // [5]
        uint32_t sdm_usbpll_sdm_clk_sel_rst : 1; // [6]
        uint32_t sdm_usbpll_pu : 1;              // [7]
        uint32_t sdm_usbpll_fbc_inv : 1;         // [8]
        uint32_t __31_9 : 23;                    // [31:9]
    } b;
} REG_ANALOG_REG_SDM_USBPLL_REG0_T;

// sdm_usbpll_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_usbpll_lock_counter_sel : 2;      // [1:0]
        uint32_t sdm_usbpll_clkout_en_counter_sel : 2; // [3:2]
        uint32_t sdm_usbpll_clk_gen_en_reg : 1;        // [4]
        uint32_t sdm_usbpll_sdmclk_sel_time_sel : 2;   // [6:5]
        uint32_t sdm_usbpll_sdm_reset_time_sel : 2;    // [8:7]
        uint32_t sdm_usbpll_sdm_resetn_reg : 1;        // [9]
        uint32_t sdm_usbpll_sdm_resetn_dr : 1;         // [10]
        uint32_t sdm_usbpll_dither_bypass : 1;         // [11]
        uint32_t __31_12 : 20;                         // [31:12]
    } b;
} REG_ANALOG_REG_SDM_USBPLL_REG2_T;

// sdm_usbpll_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_usbpll_clk_ready : 1;   // [0], read only
        uint32_t sdm_usbpll_lock_steady : 1; // [1], read only
        uint32_t sdm_usbpll_sdm_resetn : 1;  // [2], read only
        uint32_t __31_3 : 29;                // [31:3]
    } b;
} REG_ANALOG_REG_SDM_USBPLL_REG3_T;

// sdm_usbpll_reg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_usbpll_ss_devi_step : 16; // [15:0]
        uint32_t sdm_usbpll_ss_devi : 12;      // [27:16]
        uint32_t sdm_usbpll_ss_en : 1;         // [28]
        uint32_t sdm_usbpll_sdm_en : 1;        // [29]
        uint32_t __31_30 : 2;                  // [31:30]
    } b;
} REG_ANALOG_REG_SDM_USBPLL_REG4_T;

// ddr_pad_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddr_latch : 1; // [0]
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_ANALOG_REG_DDR_PAD_CFG_T;

// psram_pad_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psram_latch : 1; // [0]
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_ANALOG_REG_PSRAM_PAD_CFG_T;

// sdm_dsipll_reg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_dsipll_int_dec_sel : 3;     // [2:0]
        uint32_t sdm_dsipll_pu_pll_reg : 1;      // [3]
        uint32_t sdm_dsipll_pu_pll_dr : 1;       // [4]
        uint32_t sdm_dsipll_sdm_clk_sel_nor : 1; // [5]
        uint32_t sdm_dsipll_sdm_clk_sel_rst : 1; // [6]
        uint32_t sdm_dsipll_pu : 1;              // [7]
        uint32_t sdm_dsipll_fbc_inv : 1;         // [8]
        uint32_t __31_9 : 23;                    // [31:9]
    } b;
} REG_ANALOG_REG_SDM_DSIPLL_REG0_T;

// sdm_dsipll_reg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_dsipll_lock_counter_sel : 2;      // [1:0]
        uint32_t sdm_dsipll_clkout_en_counter_sel : 2; // [3:2]
        uint32_t sdm_dsipll_clk_gen_en_reg : 1;        // [4]
        uint32_t sdm_dsipll_sdmclk_sel_time_sel : 2;   // [6:5]
        uint32_t sdm_dsipll_sdm_reset_time_sel : 2;    // [8:7]
        uint32_t sdm_dsipll_sdm_resetn_reg : 1;        // [9]
        uint32_t sdm_dsipll_sdm_resetn_dr : 1;         // [10]
        uint32_t sdm_dsipll_dither_bypass : 1;         // [11]
        uint32_t __31_12 : 20;                         // [31:12]
    } b;
} REG_ANALOG_REG_SDM_DSIPLL_REG2_T;

// sdm_dsipll_reg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_dsipll_clk_ready : 1;   // [0], read only
        uint32_t sdm_dsipll_lock_steady : 1; // [1], read only
        uint32_t sdm_dsipll_sdm_resetn : 1;  // [2], read only
        uint32_t __31_3 : 29;                // [31:3]
    } b;
} REG_ANALOG_REG_SDM_DSIPLL_REG3_T;

// sdm_dsipll_reg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sdm_dsipll_ss_devi_step : 16; // [15:0]
        uint32_t sdm_dsipll_ss_devi : 12;      // [27:16]
        uint32_t sdm_dsipll_ss_en : 1;         // [28]
        uint32_t sdm_dsipll_sdm_en : 1;        // [29]
        uint32_t __31_30 : 2;                  // [31:30]
    } b;
} REG_ANALOG_REG_SDM_DSIPLL_REG4_T;

// mipi_lvds_phy_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t csi_lvds_mode : 1;           // [0]
        uint32_t lvds_rx_terminal_enable : 1; // [1]
        uint32_t lvdspll_sdm_sel : 1;         // [2]
        uint32_t lvdspll_refmulti2_en : 1;    // [3]
        uint32_t __31_4 : 28;                 // [31:4]
    } b;
} REG_ANALOG_REG_MIPI_LVDS_PHY_REG_T;

// pad_resetb_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_resetb_wpus : 1; // [0]
        uint32_t pad_resetb_se : 1;   // [1]
        uint32_t pad_resetb_ie : 1;   // [2]
        uint32_t pad_resetb_drv : 2;  // [4:3]
        uint32_t __31_5 : 27;         // [31:5]
    } b;
} REG_ANALOG_REG_PAD_RESETB_CFG_T;

// pad_adi_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_adi_sync_wpus : 1; // [0]
        uint32_t pad_adi_sync_se : 1;   // [1]
        uint32_t pad_adi_sync_ie : 1;   // [2]
        uint32_t pad_adi_sync_drv : 2;  // [4:3]
        uint32_t pad_adi_sda_wpus : 1;  // [5]
        uint32_t pad_adi_sda_se : 1;    // [6]
        uint32_t pad_adi_sda_ie : 1;    // [7]
        uint32_t pad_adi_sda_drv : 2;   // [9:8]
        uint32_t pad_adi_scl_wpus : 1;  // [10]
        uint32_t pad_adi_scl_se : 1;    // [11]
        uint32_t pad_adi_scl_ie : 1;    // [12]
        uint32_t pad_adi_scl_drv : 2;   // [14:13]
        uint32_t __31_15 : 17;          // [31:15]
    } b;
} REG_ANALOG_REG_PAD_ADI_CFG_T;

// pad_ap_jtag_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_ap_jtag_trst_wpus : 1; // [0]
        uint32_t pad_ap_jtag_trst_se : 1;   // [1]
        uint32_t pad_ap_jtag_trst_ie : 1;   // [2]
        uint32_t pad_ap_jtag_trst_drv : 2;  // [4:3]
        uint32_t pad_ap_jtag_tms_wpus : 1;  // [5]
        uint32_t pad_ap_jtag_tms_se : 1;    // [6]
        uint32_t pad_ap_jtag_tms_ie : 1;    // [7]
        uint32_t pad_ap_jtag_tms_drv : 2;   // [9:8]
        uint32_t pad_ap_jtag_tdo_wpus : 1;  // [10]
        uint32_t pad_ap_jtag_tdo_se : 1;    // [11]
        uint32_t pad_ap_jtag_tdo_ie : 1;    // [12]
        uint32_t pad_ap_jtag_tdo_drv : 2;   // [14:13]
        uint32_t pad_ap_jtag_tdi_wpus : 1;  // [15]
        uint32_t pad_ap_jtag_tdi_se : 1;    // [16]
        uint32_t pad_ap_jtag_tdi_ie : 1;    // [17]
        uint32_t pad_ap_jtag_tdi_drv : 2;   // [19:18]
        uint32_t pad_ap_jtag_tck_wpus : 1;  // [20]
        uint32_t pad_ap_jtag_tck_se : 1;    // [21]
        uint32_t pad_ap_jtag_tck_ie : 1;    // [22]
        uint32_t pad_ap_jtag_tck_drv : 2;   // [24:23]
        uint32_t __31_25 : 7;               // [31:25]
    } b;
} REG_ANALOG_REG_PAD_AP_JTAG_CFG_T;

// pad_spi_camera_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_spi_camera_si_1_wpus : 1; // [0]
        uint32_t pad_spi_camera_si_1_se : 1;   // [1]
        uint32_t pad_spi_camera_si_1_ie : 1;   // [2]
        uint32_t pad_spi_camera_si_1_drv : 2;  // [4:3]
        uint32_t pad_spi_camera_si_0_wpus : 1; // [5]
        uint32_t pad_spi_camera_si_0_se : 1;   // [6]
        uint32_t pad_spi_camera_si_0_ie : 1;   // [7]
        uint32_t pad_spi_camera_si_0_drv : 2;  // [9:8]
        uint32_t pad_spi_camera_sck_wpus : 1;  // [10]
        uint32_t pad_spi_camera_sck_se : 1;    // [11]
        uint32_t pad_spi_camera_sck_ie : 1;    // [12]
        uint32_t pad_spi_camera_sck_drv : 2;   // [14:13]
        uint32_t __31_15 : 17;                 // [31:15]
    } b;
} REG_ANALOG_REG_PAD_SPI_CAMERA_CFG_T;

// pad_camera_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_camera_rst_l_wpus : 1;   // [0]
        uint32_t pad_camera_rst_l_se : 1;     // [1]
        uint32_t pad_camera_rst_l_ie : 1;     // [2]
        uint32_t pad_camera_rst_l_drv : 2;    // [4:3]
        uint32_t pad_camera_ref_clk_wpus : 1; // [5]
        uint32_t pad_camera_ref_clk_se : 1;   // [6]
        uint32_t pad_camera_ref_clk_ie : 1;   // [7]
        uint32_t pad_camera_ref_clk_drv : 2;  // [9:8]
        uint32_t pad_camera_pwdn_wpus : 1;    // [10]
        uint32_t pad_camera_pwdn_se : 1;      // [11]
        uint32_t pad_camera_pwdn_ie : 1;      // [12]
        uint32_t pad_camera_pwdn_drv : 2;     // [14:13]
        uint32_t pad_camera_wpdi : 1;         // [15]
        uint32_t __31_16 : 16;                // [31:16]
    } b;
} REG_ANALOG_REG_PAD_CAMERA_CFG_T;

// pad_debug_host_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_debug_host_tx_wpus : 1;  // [0]
        uint32_t pad_debug_host_tx_se : 1;    // [1]
        uint32_t pad_debug_host_tx_ie : 1;    // [2]
        uint32_t pad_debug_host_tx_drv : 2;   // [4:3]
        uint32_t pad_debug_host_rx_wpus : 1;  // [5]
        uint32_t pad_debug_host_rx_se : 1;    // [6]
        uint32_t pad_debug_host_rx_ie : 1;    // [7]
        uint32_t pad_debug_host_rx_drv : 2;   // [9:8]
        uint32_t pad_debug_host_clk_wpus : 1; // [10]
        uint32_t pad_debug_host_clk_se : 1;   // [11]
        uint32_t pad_debug_host_clk_ie : 1;   // [12]
        uint32_t pad_debug_host_clk_drv : 2;  // [14:13]
        uint32_t __31_15 : 17;                // [31:15]
    } b;
} REG_ANALOG_REG_PAD_DEBUG_HOST_CFG_T;

// pad_gpio_cfg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_3_wpus : 1; // [0]
        uint32_t pad_gpio_3_se : 1;   // [1]
        uint32_t pad_gpio_3_ie : 1;   // [2]
        uint32_t pad_gpio_3_drv : 2;  // [4:3]
        uint32_t pad_gpio_2_wpus : 1; // [5]
        uint32_t pad_gpio_2_se : 1;   // [6]
        uint32_t pad_gpio_2_ie : 1;   // [7]
        uint32_t pad_gpio_2_drv : 2;  // [9:8]
        uint32_t pad_gpio_1_wpus : 1; // [10]
        uint32_t pad_gpio_1_se : 1;   // [11]
        uint32_t pad_gpio_1_ie : 1;   // [12]
        uint32_t pad_gpio_1_drv : 2;  // [14:13]
        uint32_t pad_gpio_0_wpus : 1; // [15]
        uint32_t pad_gpio_0_se : 1;   // [16]
        uint32_t pad_gpio_0_ie : 1;   // [17]
        uint32_t pad_gpio_0_drv : 2;  // [19:18]
        uint32_t __31_20 : 12;        // [31:20]
    } b;
} REG_ANALOG_REG_PAD_GPIO_CFG1_T;

// pad_gpio_cfg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_7_wpus : 1; // [0]
        uint32_t pad_gpio_7_se : 1;   // [1]
        uint32_t pad_gpio_7_ie : 1;   // [2]
        uint32_t pad_gpio_7_drv : 2;  // [4:3]
        uint32_t pad_gpio_5_wpus : 1; // [5]
        uint32_t pad_gpio_5_se : 1;   // [6]
        uint32_t pad_gpio_5_ie : 1;   // [7]
        uint32_t pad_gpio_5_drv : 2;  // [9:8]
        uint32_t pad_gpio_4_wpus : 1; // [10]
        uint32_t pad_gpio_4_se : 1;   // [11]
        uint32_t pad_gpio_4_ie : 1;   // [12]
        uint32_t pad_gpio_4_drv : 2;  // [14:13]
        uint32_t __31_15 : 17;        // [31:15]
    } b;
} REG_ANALOG_REG_PAD_GPIO_CFG2_T;

// pad_gpo_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_12_wpus : 1; // [0]
        uint32_t pad_gpio_12_se : 1;   // [1]
        uint32_t pad_gpio_12_ie : 1;   // [2]
        uint32_t pad_gpio_12_drv : 2;  // [4:3]
        uint32_t pad_gpio_11_wpus : 1; // [5]
        uint32_t pad_gpio_11_se : 1;   // [6]
        uint32_t pad_gpio_11_ie : 1;   // [7]
        uint32_t pad_gpio_11_drv : 2;  // [9:8]
        uint32_t pad_gpio_10_wpus : 1; // [10]
        uint32_t pad_gpio_10_se : 1;   // [11]
        uint32_t pad_gpio_10_ie : 1;   // [12]
        uint32_t pad_gpio_10_drv : 2;  // [14:13]
        uint32_t pad_gpio_9_wpus : 1;  // [15]
        uint32_t pad_gpio_9_se : 1;    // [16]
        uint32_t pad_gpio_9_ie : 1;    // [17]
        uint32_t pad_gpio_9_drv : 2;   // [19:18]
        uint32_t pad_gpio_8_wpus : 1;  // [20]
        uint32_t pad_gpio_8_se : 1;    // [21]
        uint32_t pad_gpio_8_ie : 1;    // [22]
        uint32_t pad_gpio_8_drv : 2;   // [24:23]
        uint32_t __31_25 : 7;          // [31:25]
    } b;
} REG_ANALOG_REG_PAD_GPO_CFG_T;

// pad_i2c_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_15_wpus : 1;    // [0]
        uint32_t pad_gpio_15_se : 1;      // [1]
        uint32_t pad_gpio_15_ie : 1;      // [2]
        uint32_t pad_gpio_15_drv : 2;     // [4:3]
        uint32_t pad_gpio_14_wpus : 1;    // [5]
        uint32_t pad_gpio_14_se : 1;      // [6]
        uint32_t pad_gpio_14_ie : 1;      // [7]
        uint32_t pad_gpio_14_drv : 2;     // [9:8]
        uint32_t pad_i2c_m1_sda_wpus : 1; // [10]
        uint32_t pad_i2c_m1_sda_se : 1;   // [11]
        uint32_t pad_i2c_m1_sda_ie : 1;   // [12]
        uint32_t pad_i2c_m1_sda_drv : 2;  // [14:13]
        uint32_t pad_i2c_m1_scl_wpus : 1; // [15]
        uint32_t pad_i2c_m1_scl_se : 1;   // [16]
        uint32_t pad_i2c_m1_scl_ie : 1;   // [17]
        uint32_t pad_i2c_m1_scl_drv : 2;  // [19:18]
        uint32_t __31_20 : 12;            // [31:20]
    } b;
} REG_ANALOG_REG_PAD_I2C_CFG_T;

// pad_aud_ad_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_aud_sclk_wpus : 1;    // [0]
        uint32_t pad_aud_sclk_se : 1;      // [1]
        uint32_t pad_aud_sclk_ie : 1;      // [2]
        uint32_t pad_aud_sclk_drv : 2;     // [4:3]
        uint32_t pad_aud_da_sync_wpus : 1; // [5]
        uint32_t pad_aud_da_sync_se : 1;   // [6]
        uint32_t pad_aud_da_sync_ie : 1;   // [7]
        uint32_t pad_aud_da_sync_drv : 2;  // [9:8]
        uint32_t pad_aud_da_d1_wpus : 1;   // [10]
        uint32_t pad_aud_da_d1_se : 1;     // [11]
        uint32_t pad_aud_da_d1_ie : 1;     // [12]
        uint32_t pad_aud_da_d1_drv : 2;    // [14:13]
        uint32_t pad_aud_da_d0_wpus : 1;   // [15]
        uint32_t pad_aud_da_d0_se : 1;     // [16]
        uint32_t pad_aud_da_d0_ie : 1;     // [17]
        uint32_t pad_aud_da_d0_drv : 2;    // [19:18]
        uint32_t pad_aud_ad_sync_wpus : 1; // [20]
        uint32_t pad_aud_ad_sync_se : 1;   // [21]
        uint32_t pad_aud_ad_sync_ie : 1;   // [22]
        uint32_t pad_aud_ad_sync_drv : 2;  // [24:23]
        uint32_t pad_aud_ad_d0_wpus : 1;   // [25]
        uint32_t pad_aud_ad_d0_se : 1;     // [26]
        uint32_t pad_aud_ad_d0_ie : 1;     // [27]
        uint32_t pad_aud_ad_d0_drv : 2;    // [29:28]
        uint32_t __31_30 : 2;              // [31:30]
    } b;
} REG_ANALOG_REG_PAD_AUD_AD_CFG_T;

// pad_keyin_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_keyin_5_wpus : 1; // [0]
        uint32_t pad_keyin_5_se : 1;   // [1]
        uint32_t pad_keyin_5_ie : 1;   // [2]
        uint32_t pad_keyin_5_drv : 2;  // [4:3]
        uint32_t pad_keyin_4_wpus : 1; // [5]
        uint32_t pad_keyin_4_se : 1;   // [6]
        uint32_t pad_keyin_4_ie : 1;   // [7]
        uint32_t pad_keyin_4_drv : 2;  // [9:8]
        uint32_t pad_keyin_3_wpus : 1; // [10]
        uint32_t pad_keyin_3_se : 1;   // [11]
        uint32_t pad_keyin_3_ie : 1;   // [12]
        uint32_t pad_keyin_3_drv : 2;  // [14:13]
        uint32_t pad_keyin_2_wpus : 1; // [15]
        uint32_t pad_keyin_2_se : 1;   // [16]
        uint32_t pad_keyin_2_ie : 1;   // [17]
        uint32_t pad_keyin_2_drv : 2;  // [19:18]
        uint32_t pad_keyin_1_wpus : 1; // [20]
        uint32_t pad_keyin_1_se : 1;   // [21]
        uint32_t pad_keyin_1_ie : 1;   // [22]
        uint32_t pad_keyin_1_drv : 2;  // [24:23]
        uint32_t pad_keyin_0_wpus : 1; // [25]
        uint32_t pad_keyin_0_se : 1;   // [26]
        uint32_t pad_keyin_0_ie : 1;   // [27]
        uint32_t pad_keyin_0_drv : 2;  // [29:28]
        uint32_t pad_key_wpdi : 1;     // [30]
        uint32_t __31_31 : 1;          // [31]
    } b;
} REG_ANALOG_REG_PAD_KEYIN_CFG_T;

// pad_keyout_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_keyout_5_wpus : 1; // [0]
        uint32_t pad_keyout_5_se : 1;   // [1]
        uint32_t pad_keyout_5_ie : 1;   // [2]
        uint32_t pad_keyout_5_drv : 2;  // [4:3]
        uint32_t pad_keyout_4_wpus : 1; // [5]
        uint32_t pad_keyout_4_se : 1;   // [6]
        uint32_t pad_keyout_4_ie : 1;   // [7]
        uint32_t pad_keyout_4_drv : 2;  // [9:8]
        uint32_t pad_keyout_3_wpus : 1; // [10]
        uint32_t pad_keyout_3_se : 1;   // [11]
        uint32_t pad_keyout_3_ie : 1;   // [12]
        uint32_t pad_keyout_3_drv : 2;  // [14:13]
        uint32_t pad_keyout_2_wpus : 1; // [15]
        uint32_t pad_keyout_2_se : 1;   // [16]
        uint32_t pad_keyout_2_ie : 1;   // [17]
        uint32_t pad_keyout_2_drv : 2;  // [19:18]
        uint32_t pad_keyout_1_wpus : 1; // [20]
        uint32_t pad_keyout_1_se : 1;   // [21]
        uint32_t pad_keyout_1_ie : 1;   // [22]
        uint32_t pad_keyout_1_drv : 2;  // [24:23]
        uint32_t pad_keyout_0_wpus : 1; // [25]
        uint32_t pad_keyout_0_se : 1;   // [26]
        uint32_t pad_keyout_0_ie : 1;   // [27]
        uint32_t pad_keyout_0_drv : 2;  // [29:28]
        uint32_t __31_30 : 2;           // [31:30]
    } b;
} REG_ANALOG_REG_PAD_KEYOUT_CFG_T;

// pad_lcd_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_lcd_rstb_spu : 1;  // [0]
        uint32_t pad_lcd_rstb_se : 1;   // [1]
        uint32_t pad_lcd_rstb_ie : 1;   // [2]
        uint32_t pad_lcd_rstb_drv : 4;  // [6:3]
        uint32_t pad_lcd_fmark_spu : 1; // [7]
        uint32_t pad_lcd_fmark_se : 1;  // [8]
        uint32_t pad_lcd_fmark_ie : 1;  // [9]
        uint32_t pad_lcd_fmark_drv : 4; // [13:10]
        uint32_t pad_lcd_ms : 1;        // [14]
        uint32_t pad_lcd_msen : 1;      // [15]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_ANALOG_REG_PAD_LCD_CFG_T;

// pad_gpio13_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_13_wpus : 1; // [0]
        uint32_t pad_gpio_13_se : 1;   // [1]
        uint32_t pad_gpio_13_ie : 1;   // [2]
        uint32_t pad_gpio_13_drv : 2;  // [4:3]
        uint32_t pad_misc_r_wpdi : 1;  // [5]
        uint32_t pad_misc_l_wpdi : 1;  // [6]
        uint32_t pad_mipidsi_wpdi : 1; // [7]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_ANALOG_REG_PAD_GPIO13_CFG_T;

// pad_rfdig_gpio_cfg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_rfdig_gpio_4_wpus : 1; // [0]
        uint32_t pad_rfdig_gpio_4_se : 1;   // [1]
        uint32_t pad_rfdig_gpio_4_ie : 1;   // [2]
        uint32_t pad_rfdig_gpio_4_drv : 2;  // [4:3]
        uint32_t pad_rfdig_gpio_3_wpus : 1; // [5]
        uint32_t pad_rfdig_gpio_3_se : 1;   // [6]
        uint32_t pad_rfdig_gpio_3_ie : 1;   // [7]
        uint32_t pad_rfdig_gpio_3_drv : 2;  // [9:8]
        uint32_t pad_rfdig_gpio_2_wpus : 1; // [10]
        uint32_t pad_rfdig_gpio_2_se : 1;   // [11]
        uint32_t pad_rfdig_gpio_2_ie : 1;   // [12]
        uint32_t pad_rfdig_gpio_2_drv : 2;  // [14:13]
        uint32_t pad_rfdig_gpio_1_wpus : 1; // [15]
        uint32_t pad_rfdig_gpio_1_se : 1;   // [16]
        uint32_t pad_rfdig_gpio_1_ie : 1;   // [17]
        uint32_t pad_rfdig_gpio_1_drv : 2;  // [19:18]
        uint32_t pad_rfdig_gpio_0_wpus : 1; // [20]
        uint32_t pad_rfdig_gpio_0_se : 1;   // [21]
        uint32_t pad_rfdig_gpio_0_ie : 1;   // [22]
        uint32_t pad_rfdig_gpio_0_drv : 2;  // [24:23]
        uint32_t pad_rf_wpdi : 1;           // [25]
        uint32_t __31_26 : 6;               // [31:26]
    } b;
} REG_ANALOG_REG_PAD_RFDIG_GPIO_CFG1_T;

// pad_rfdig_gpio_cfg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_rfdig_gpio_7_wpus : 1; // [0]
        uint32_t pad_rfdig_gpio_7_se : 1;   // [1]
        uint32_t pad_rfdig_gpio_7_ie : 1;   // [2]
        uint32_t pad_rfdig_gpio_7_drv : 2;  // [4:3]
        uint32_t pad_rfdig_gpio_6_wpus : 1; // [5]
        uint32_t pad_rfdig_gpio_6_se : 1;   // [6]
        uint32_t pad_rfdig_gpio_6_ie : 1;   // [7]
        uint32_t pad_rfdig_gpio_6_drv : 2;  // [9:8]
        uint32_t pad_rfdig_gpio_5_wpus : 1; // [10]
        uint32_t pad_rfdig_gpio_5_se : 1;   // [11]
        uint32_t pad_rfdig_gpio_5_ie : 1;   // [12]
        uint32_t pad_rfdig_gpio_5_drv : 2;  // [14:13]
        uint32_t __31_15 : 17;              // [31:15]
    } b;
} REG_ANALOG_REG_PAD_RFDIG_GPIO_CFG2_T;

// pad_sdmmc1_others_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_sdmmc1_cmd_spu : 1; // [0]
        uint32_t pad_sdmmc1_cmd_se : 1;  // [1]
        uint32_t pad_sdmmc1_cmd_ie : 1;  // [2]
        uint32_t pad_sdmmc1_cmd_drv : 4; // [6:3]
        uint32_t pad_sdmmc1_clk_spu : 1; // [7]
        uint32_t pad_sdmmc1_clk_se : 1;  // [8]
        uint32_t pad_sdmmc1_clk_ie : 1;  // [9]
        uint32_t pad_sdmmc1_clk_drv : 4; // [13:10]
        uint32_t pad_sdmmc1_msen : 1;    // [14]
        uint32_t pad_sdmmc1_ms : 1;      // [15]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_ANALOG_REG_PAD_SDMMC1_OTHERS_CFG_T;

// pad_sdmmc1_data_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_sdmmc1_data_3_spu : 1; // [0]
        uint32_t pad_sdmmc1_data_3_se : 1;  // [1]
        uint32_t pad_sdmmc1_data_3_ie : 1;  // [2]
        uint32_t pad_sdmmc1_data_3_drv : 4; // [6:3]
        uint32_t pad_sdmmc1_data_2_spu : 1; // [7]
        uint32_t pad_sdmmc1_data_2_se : 1;  // [8]
        uint32_t pad_sdmmc1_data_2_ie : 1;  // [9]
        uint32_t pad_sdmmc1_data_2_drv : 4; // [13:10]
        uint32_t pad_sdmmc1_data_1_spu : 1; // [14]
        uint32_t pad_sdmmc1_data_1_se : 1;  // [15]
        uint32_t pad_sdmmc1_data_1_ie : 1;  // [16]
        uint32_t pad_sdmmc1_data_1_drv : 4; // [20:17]
        uint32_t pad_sdmmc1_data_0_spu : 1; // [21]
        uint32_t pad_sdmmc1_data_0_se : 1;  // [22]
        uint32_t pad_sdmmc1_data_0_ie : 1;  // [23]
        uint32_t pad_sdmmc1_data_0_drv : 4; // [27:24]
        uint32_t __31_28 : 4;               // [31:28]
    } b;
} REG_ANALOG_REG_PAD_SDMMC1_DATA_CFG_T;

// pad_sim_1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_sim_1_rst_spu : 1; // [0]
        uint32_t pad_sim_1_rst_se : 1;  // [1]
        uint32_t pad_sim_1_rst_ie : 1;  // [2]
        uint32_t pad_sim_1_rst_drv : 4; // [6:3]
        uint32_t pad_sim_1_dio_spu : 1; // [7]
        uint32_t pad_sim_1_dio_se : 1;  // [8]
        uint32_t pad_sim_1_dio_ie : 1;  // [9]
        uint32_t pad_sim_1_dio_drv : 4; // [13:10]
        uint32_t pad_sim_1_clk_spu : 1; // [14]
        uint32_t pad_sim_1_clk_se : 1;  // [15]
        uint32_t pad_sim_1_clk_ie : 1;  // [16]
        uint32_t pad_sim_1_clk_drv : 4; // [20:17]
        uint32_t pad_sim_1_msen : 1;    // [21]
        uint32_t pad_sim_1_ms : 1;      // [22]
        uint32_t __31_23 : 9;           // [31:23]
    } b;
} REG_ANALOG_REG_PAD_SIM_1_CFG_T;

// pad_sim_2_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_sim_2_rst_spu : 1; // [0]
        uint32_t pad_sim_2_rst_se : 1;  // [1]
        uint32_t pad_sim_2_rst_ie : 1;  // [2]
        uint32_t pad_sim_2_rst_drv : 4; // [6:3]
        uint32_t pad_sim_2_dio_spu : 1; // [7]
        uint32_t pad_sim_2_dio_se : 1;  // [8]
        uint32_t pad_sim_2_dio_ie : 1;  // [9]
        uint32_t pad_sim_2_dio_drv : 4; // [13:10]
        uint32_t pad_sim_2_clk_spu : 1; // [14]
        uint32_t pad_sim_2_clk_se : 1;  // [15]
        uint32_t pad_sim_2_clk_ie : 1;  // [16]
        uint32_t pad_sim_2_clk_drv : 4; // [20:17]
        uint32_t pad_sim_2_msen : 1;    // [21]
        uint32_t pad_sim_2_ms : 1;      // [22]
        uint32_t __31_23 : 9;           // [31:23]
    } b;
} REG_ANALOG_REG_PAD_SIM_2_CFG_T;

// pad_spi_flash_cfg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_spi_flash_cs_spu : 1;    // [0]
        uint32_t pad_spi_flash_cs_se : 1;     // [1]
        uint32_t pad_spi_flash_cs_ie : 1;     // [2]
        uint32_t pad_spi_flash_cs_drv : 4;    // [6:3]
        uint32_t pad_spi_flash_clk_spu : 1;   // [7]
        uint32_t pad_spi_flash_clk_se : 1;    // [8]
        uint32_t pad_spi_flash_clk_ie : 1;    // [9]
        uint32_t pad_spi_flash_clk_drv : 4;   // [13:10]
        uint32_t pad_spi_flash_sel_spu : 1;   // [14]
        uint32_t pad_spi_flash_sel_se : 1;    // [15]
        uint32_t pad_spi_flash_sel_ie : 1;    // [16]
        uint32_t pad_spi_flash_sel_drv : 4;   // [20:17]
        uint32_t pad_nand_flash_sel_wpus : 1; // [21]
        uint32_t pad_nand_flash_sel_se : 1;   // [22]
        uint32_t pad_nand_flash_sel_ie : 1;   // [23]
        uint32_t pad_nand_flash_sel_drv : 2;  // [25:24]
        uint32_t __31_26 : 6;                 // [31:26]
    } b;
} REG_ANALOG_REG_PAD_SPI_FLASH_CFG1_T;

// pad_spi_flash_cfg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_spi_flash_sio_3_spu : 1; // [0]
        uint32_t pad_spi_flash_sio_3_se : 1;  // [1]
        uint32_t pad_spi_flash_sio_3_ie : 1;  // [2]
        uint32_t pad_spi_flash_sio_3_drv : 4; // [6:3]
        uint32_t pad_spi_flash_sio_2_spu : 1; // [7]
        uint32_t pad_spi_flash_sio_2_se : 1;  // [8]
        uint32_t pad_spi_flash_sio_2_ie : 1;  // [9]
        uint32_t pad_spi_flash_sio_2_drv : 4; // [13:10]
        uint32_t pad_spi_flash_sio_1_spu : 1; // [14]
        uint32_t pad_spi_flash_sio_1_se : 1;  // [15]
        uint32_t pad_spi_flash_sio_1_ie : 1;  // [16]
        uint32_t pad_spi_flash_sio_1_drv : 4; // [20:17]
        uint32_t pad_spi_flash_sio_0_spu : 1; // [21]
        uint32_t pad_spi_flash_sio_0_se : 1;  // [22]
        uint32_t pad_spi_flash_sio_0_ie : 1;  // [23]
        uint32_t pad_spi_flash_sio_0_drv : 4; // [27:24]
        uint32_t pad_spi_flash_msen : 1;      // [28]
        uint32_t pad_spi_flash_ms : 1;        // [29]
        uint32_t __31_30 : 2;                 // [31:30]
    } b;
} REG_ANALOG_REG_PAD_SPI_FLASH_CFG2_T;

// pad_spi_lcd_cfg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_spi_lcd_cs_spu : 1;  // [0]
        uint32_t pad_spi_lcd_cs_se : 1;   // [1]
        uint32_t pad_spi_lcd_cs_ie : 1;   // [2]
        uint32_t pad_spi_lcd_cs_drv : 4;  // [6:3]
        uint32_t pad_spi_lcd_clk_spu : 1; // [7]
        uint32_t pad_spi_lcd_clk_se : 1;  // [8]
        uint32_t pad_spi_lcd_clk_ie : 1;  // [9]
        uint32_t pad_spi_lcd_clk_drv : 4; // [13:10]
        uint32_t __31_14 : 18;            // [31:14]
    } b;
} REG_ANALOG_REG_PAD_SPI_LCD_CFG1_T;

// pad_spi_lcd_cfg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_spi_lcd_sio_spu : 1;    // [0]
        uint32_t pad_spi_lcd_sio_se : 1;     // [1]
        uint32_t pad_spi_lcd_sio_ie : 1;     // [2]
        uint32_t pad_spi_lcd_sio_drv : 4;    // [6:3]
        uint32_t pad_spi_lcd_select_spu : 1; // [7]
        uint32_t pad_spi_lcd_select_se : 1;  // [8]
        uint32_t pad_spi_lcd_select_ie : 1;  // [9]
        uint32_t pad_spi_lcd_select_drv : 4; // [13:10]
        uint32_t pad_spi_lcd_sdc_spu : 1;    // [14]
        uint32_t pad_spi_lcd_sdc_se : 1;     // [15]
        uint32_t pad_spi_lcd_sdc_ie : 1;     // [16]
        uint32_t pad_spi_lcd_sdc_drv : 4;    // [20:17]
        uint32_t __31_21 : 11;               // [31:21]
    } b;
} REG_ANALOG_REG_PAD_SPI_LCD_CFG2_T;

// pad_tst_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_tst_h_wpus : 1; // [0]
        uint32_t pad_tst_h_se : 1;   // [1]
        uint32_t pad_tst_h_ie : 1;   // [2]
        uint32_t pad_tst_h_drv : 2;  // [4:3]
        uint32_t __31_5 : 27;        // [31:5]
    } b;
} REG_ANALOG_REG_PAD_TST_CFG_T;

// pad_uart_1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_19_wpus : 1; // [0]
        uint32_t pad_gpio_19_se : 1;   // [1]
        uint32_t pad_gpio_19_ie : 1;   // [2]
        uint32_t pad_gpio_19_drv : 2;  // [4:3]
        uint32_t pad_gpio_18_wpus : 1; // [5]
        uint32_t pad_gpio_18_se : 1;   // [6]
        uint32_t pad_gpio_18_ie : 1;   // [7]
        uint32_t pad_gpio_18_drv : 2;  // [9:8]
        uint32_t __31_10 : 22;         // [31:10]
    } b;
} REG_ANALOG_REG_PAD_UART_1_CFG_T;

// pad_uart_2_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_21_wpus : 1; // [0]
        uint32_t pad_gpio_21_se : 1;   // [1]
        uint32_t pad_gpio_21_ie : 1;   // [2]
        uint32_t pad_gpio_21_drv : 2;  // [4:3]
        uint32_t pad_gpio_20_wpus : 1; // [5]
        uint32_t pad_gpio_20_se : 1;   // [6]
        uint32_t pad_gpio_20_ie : 1;   // [7]
        uint32_t pad_gpio_20_drv : 2;  // [9:8]
        uint32_t pad_gpio_23_wpus : 1; // [10]
        uint32_t pad_gpio_23_se : 1;   // [11]
        uint32_t pad_gpio_23_ie : 1;   // [12]
        uint32_t pad_gpio_23_drv : 2;  // [14:13]
        uint32_t pad_gpio_22_wpus : 1; // [15]
        uint32_t pad_gpio_22_se : 1;   // [16]
        uint32_t pad_gpio_22_ie : 1;   // [17]
        uint32_t pad_gpio_22_drv : 2;  // [19:18]
        uint32_t __31_20 : 12;         // [31:20]
    } b;
} REG_ANALOG_REG_PAD_UART_2_CFG_T;

// pad_secure_boot_mode_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_secure_boot_mode_wpus : 1; // [0]
        uint32_t pad_secure_boot_mode_se : 1;   // [1]
        uint32_t pad_secure_boot_mode_ie : 1;   // [2]
        uint32_t pad_secure_boot_mode_drv : 2;  // [4:3]
        uint32_t __31_5 : 27;                   // [31:5]
    } b;
} REG_ANALOG_REG_PAD_SECURE_BOOT_MODE_CFG_T;

// apll_reg0
#define ANALOG_REG_APLL_CLK_AP_EN (1 << 0)
#define ANALOG_REG_APLL_LP_MODE_EN (1 << 1)
#define ANALOG_REG_APLL_LOCK (1 << 2)

// apll_reg1
#define ANALOG_REG_APLL_REFMULTI2_EN (1 << 0)
#define ANALOG_REG_APLL_PCON_MODE (1 << 1)
#define ANALOG_REG_APLL_CPR2_IBIT(n) (((n)&0x7) << 2)
#define ANALOG_REG_APLL_CPC2_IBIT(n) (((n)&0x7) << 5)
#define ANALOG_REG_APLL_CPBIAS_IBIT(n) (((n)&0x7) << 8)
#define ANALOG_REG_APLL_REG_RES_BIT(n) (((n)&0x3) << 11)
#define ANALOG_REG_APLL_VREG_BIT(n) (((n)&0xf) << 13)
#define ANALOG_REG_APLL_PFD_DLY_NUM(n) (((n)&0x7) << 17)

// apll_reg2
#define ANALOG_REG_APLL_VCO_LOW_TEST (1 << 0)
#define ANALOG_REG_APLL_VCO_HIGH_TEST (1 << 1)
#define ANALOG_REG_APLL_SDM_CLK_TEST_EN (1 << 2)
#define ANALOG_REG_APLL_TEST_EN (1 << 3)

// sdm_apll_reg0
#define ANALOG_REG_SDM_APLL_INT_DEC_SEL(n) (((n)&0x7) << 0)
#define ANALOG_REG_SDM_APLL_PU_PLL_REG (1 << 3)
#define ANALOG_REG_SDM_APLL_PU_PLL_DR (1 << 4)
#define ANALOG_REG_SDM_APLL_SDM_CLK_SEL_NOR (1 << 5)
#define ANALOG_REG_SDM_APLL_SDM_CLK_SEL_RST (1 << 6)
#define ANALOG_REG_SDM_APLL_PU (1 << 7)
#define ANALOG_REG_SDM_APLL_FBC_INV (1 << 8)

// sdm_apll_reg1
#define ANALOG_REG_SDM_APLL_SDM_FREQ(n) (((n)&0xffffffff) << 0)

// sdm_apll_reg2
#define ANALOG_REG_SDM_APLL_LOCK_COUNTER_SEL(n) (((n)&0x3) << 0)
#define ANALOG_REG_SDM_APLL_CLKOUT_EN_COUNTER_SEL(n) (((n)&0x3) << 2)
#define ANALOG_REG_SDM_APLL_CLK_GEN_EN_REG (1 << 4)
#define ANALOG_REG_SDM_APLL_SDMCLK_SEL_TIME_SEL(n) (((n)&0x3) << 5)
#define ANALOG_REG_SDM_APLL_SDM_RESET_TIME_SEL(n) (((n)&0x3) << 7)
#define ANALOG_REG_SDM_APLL_SDM_RESETN_REG (1 << 9)
#define ANALOG_REG_SDM_APLL_SDM_RESETN_DR (1 << 10)
#define ANALOG_REG_SDM_APLL_DITHER_BYPASS (1 << 11)

// sdm_apll_reg3
#define ANALOG_REG_SDM_APLL_CLK_READY (1 << 0)
#define ANALOG_REG_SDM_APLL_LOCK_STEADY (1 << 1)
#define ANALOG_REG_SDM_APLL_SDM_RESETN (1 << 2)

// sdm_apll_reg4
#define ANALOG_REG_SDM_APLL_SS_DEVI_STEP(n) (((n)&0xffff) << 0)
#define ANALOG_REG_SDM_APLL_SS_DEVI(n) (((n)&0xfff) << 16)
#define ANALOG_REG_SDM_APLL_SS_EN (1 << 28)
#define ANALOG_REG_SDM_APLL_SDM_EN (1 << 29)

// mempll_reg0
#define ANALOG_REG_MEMPLL_CLK_MEM_EN (1 << 0)
#define ANALOG_REG_MEMPLL_LP_MODE_EN (1 << 1)
#define ANALOG_REG_MEMPLL_LOCK (1 << 2)

// mempll_reg1
#define ANALOG_REG_MEMPLL_REFMULTI2_EN (1 << 0)
#define ANALOG_REG_MEMPLL_PCON_MODE (1 << 1)
#define ANALOG_REG_MEMPLL_CPR2_IBIT(n) (((n)&0x7) << 2)
#define ANALOG_REG_MEMPLL_CPC2_IBIT(n) (((n)&0x7) << 5)
#define ANALOG_REG_MEMPLL_CPBIAS_IBIT(n) (((n)&0x7) << 8)
#define ANALOG_REG_MEMPLL_REG_RES_BIT(n) (((n)&0x3) << 11)
#define ANALOG_REG_MEMPLL_VREG_BIT(n) (((n)&0xf) << 13)
#define ANALOG_REG_MEMPLL_PFD_DLY_NUM(n) (((n)&0x7) << 17)

// mempll_reg2
#define ANALOG_REG_MEMPLL_VCO_LOW_TEST (1 << 0)
#define ANALOG_REG_MEMPLL_VCO_HIGH_TEST (1 << 1)
#define ANALOG_REG_MEMPLL_SDM_CLK_TEST_EN (1 << 2)
#define ANALOG_REG_MEMPLL_TEST_EN (1 << 3)

// sdm_mempll_reg0
#define ANALOG_REG_SDM_MEMPLL_INT_DEC_SEL(n) (((n)&0x7) << 0)
#define ANALOG_REG_SDM_MEMPLL_PU_PLL_REG (1 << 3)
#define ANALOG_REG_SDM_MEMPLL_PU_PLL_DR (1 << 4)
#define ANALOG_REG_SDM_MEMPLL_SDM_CLK_SEL_NOR (1 << 5)
#define ANALOG_REG_SDM_MEMPLL_SDM_CLK_SEL_RST (1 << 6)
#define ANALOG_REG_SDM_MEMPLL_PU (1 << 7)
#define ANALOG_REG_SDM_MEMPLL_FBC_INV (1 << 8)

// sdm_mempll_reg1
#define ANALOG_REG_SDM_MEMPLL_SDM_FREQ(n) (((n)&0xffffffff) << 0)

// sdm_mempll_reg2
#define ANALOG_REG_SDM_MEMPLL_LOCK_COUNTER_SEL(n) (((n)&0x3) << 0)
#define ANALOG_REG_SDM_MEMPLL_CLKOUT_EN_COUNTER_SEL(n) (((n)&0x3) << 2)
#define ANALOG_REG_SDM_MEMPLL_CLK_GEN_EN_REG (1 << 4)
#define ANALOG_REG_SDM_MEMPLL_SDMCLK_SEL_TIME_SEL(n) (((n)&0x3) << 5)
#define ANALOG_REG_SDM_MEMPLL_SDM_RESET_TIME_SEL(n) (((n)&0x3) << 7)
#define ANALOG_REG_SDM_MEMPLL_SDM_RESETN_REG (1 << 9)
#define ANALOG_REG_SDM_MEMPLL_SDM_RESETN_DR (1 << 10)
#define ANALOG_REG_SDM_MEMPLL_DITHER_BYPASS (1 << 11)

// sdm_mempll_reg3
#define ANALOG_REG_SDM_MEMPLL_CLK_READY (1 << 0)
#define ANALOG_REG_SDM_MEMPLL_LOCK_STEADY (1 << 1)
#define ANALOG_REG_SDM_MEMPLL_SDM_RESETN (1 << 2)

// sdm_mempll_reg4
#define ANALOG_REG_SDM_MEMPLL_SS_DEVI_STEP(n) (((n)&0xffff) << 0)
#define ANALOG_REG_SDM_MEMPLL_SS_DEVI(n) (((n)&0xfff) << 16)
#define ANALOG_REG_SDM_MEMPLL_SS_EN (1 << 28)
#define ANALOG_REG_SDM_MEMPLL_SDM_EN (1 << 29)

// usb_reg0
#define ANALOG_REG_USB_CDR_GAIN(n) (((n)&0xf) << 0)
#define ANALOG_REG_USB_PHASE_SEL(n) (((n)&0x3) << 4)
#define ANALOG_REG_USB_UPDN_MODE (1 << 6)
#define ANALOG_REG_USB_RST_INTP_ENABLE (1 << 7)
#define ANALOG_REG_USB_CDR_CLK_EDGE_BIT (1 << 8)
#define ANALOG_REG_USB_TX_RSTN (1 << 9)

// usb_reg1
#define ANALOG_REG_USB_HS_LVLOUT_BIT(n) (((n)&0xf) << 0)
#define ANALOG_REG_USB_V125M_SEL_BIT(n) (((n)&0x7) << 4)
#define ANALOG_REG_USB_V575M_SEL_BIT(n) (((n)&0x7) << 7)
#define ANALOG_REG_USB_VREF_VBIT(n) (((n)&0x7) << 10)
#define ANALOG_REG_USB_VREF_IBIT(n) (((n)&0x3) << 13)

// usb_reg2
#define ANALOG_REG_USB_PU_LPTX (1 << 0)
#define ANALOG_REG_USB_PU_HSTX (1 << 1)
#define ANALOG_REG_USB_PU_HSRX (1 << 2)
#define ANALOG_REG_USB_PU_IREF (1 << 3)
#define ANALOG_REG_USB_PU_1V2 (1 << 4)
#define ANALOG_REG_USB_PU_1V8 (1 << 5)
#define ANALOG_REG_USB_PU_PKD (1 << 6)
#define ANALOG_REG_USB_PWR_ON (1 << 7)
#define ANALOG_REG_USB_PU_USB_DEV (1 << 8)
#define ANALOG_REG_USB_VBUSVLD_BIT (1 << 9)
#define ANALOG_REG_USB_PU_OTG (1 << 10)
#define ANALOG_REG_USB_VBG_SEL (1 << 11)
#define ANALOG_REG_USB_SQUELCH_MODE_SEL (1 << 12)
#define ANALOG_REG_USB_DISCNNXT_MODE_SEL (1 << 13)
#define ANALOG_REG_USB_SQUELCH_AUX (1 << 14)

// usb_reg3
#define ANALOG_REG_USB_PATTERN(n) (((n)&0x3ff) << 0)
#define ANALOG_REG_USB_EN_PATTERN (1 << 10)
#define ANALOG_REG_USB_LOOPBACK (1 << 11)
#define ANALOG_REG_USB_DET_EN (1 << 12)

// usb_reg4
#define ANALOG_REG_USB_LPTX_DRV_SEL(n) (((n)&0xff) << 0)
#define ANALOG_REG_USB_RES_TERM_BIT(n) (((n)&0x3) << 8)
#define ANALOG_REG_USB_IO33_ENABLE (1 << 10)

// usb_pll1
#define ANALOG_REG_USB_PLL_CPR2_IBIT(n) (((n)&0x7) << 0)
#define ANALOG_REG_USB_PLL_CPC2_IBIT(n) (((n)&0x7) << 3)
#define ANALOG_REG_USB_PLL_CPBIAS_IBIT(n) (((n)&0x7) << 6)
#define ANALOG_REG_USB_REG_RES_BIT(n) (((n)&0x3) << 9)
#define ANALOG_REG_USB_PLL_VREG_BIT(n) (((n)&0xf) << 11)

// usb_pll2
#define ANALOG_REG_USB_PLL_CLK_960M_EN (1 << 0)
#define ANALOG_REG_USB_VCO_LOW_TEST (1 << 1)
#define ANALOG_REG_USB_VCO_HIGH_TEST (1 << 2)
#define ANALOG_REG_USB_SDM_CLK_TEST_EN (1 << 3)
#define ANALOG_REG_USB_PLL_TEST_EN (1 << 4)
#define ANALOG_REG_USB_REFMULTI2_EN (1 << 5)
#define ANALOG_REG_USB_PCON_MODE (1 << 6)

// usb_suspend
#define ANALOG_REG_USB_PU_HSTX_SUSPEND (1 << 0)
#define ANALOG_REG_USB_PU_HSRX_SUSPEND (1 << 1)
#define ANALOG_REG_USB_PU_IREF_SUSPEND (1 << 2)
#define ANALOG_REG_USB_PU_PKD_SUSPEND (1 << 3)
#define ANALOG_REG_USB_PU_OTG_SUSPEND (1 << 4)
#define ANALOG_REG_USB_PU_SUSPEND_BYPASS (1 << 5)

// usb11_reg
#define ANALOG_REG_USB11_LPTX_DRVSEL(n) (((n)&0xff) << 0)
#define ANALOG_REG_USB11_IO_BIAS_EN (1 << 8)
#define ANALOG_REG_USB11_PU_USB (1 << 9)
#define ANALOG_REG_USB11_USB_CTRL(n) (((n)&0x7) << 10)
#define ANALOG_REG_USB11_USB_STANDBY (1 << 13)
#define ANALOG_REG_USB11_SUSPEND_BYPASS (1 << 14)

// usb_mon
#define ANALOG_REG_USB_PLL_LOCK (1 << 0)
#define ANALOG_REG_USB_LOCK (1 << 1)
#define ANALOG_REG_USB_CORRECT (1 << 2)
#define ANALOG_REG_USB_DM_CHR (1 << 3)
#define ANALOG_REG_USB_DP_CHR (1 << 4)

// usb_reserved
#define ANALOG_REG_USB_REG_RESV(n) (((n)&0xffff) << 0)

// sdm_usbpll_reg0
#define ANALOG_REG_SDM_USBPLL_INT_DEC_SEL(n) (((n)&0x7) << 0)
#define ANALOG_REG_SDM_USBPLL_PU_PLL_REG (1 << 3)
#define ANALOG_REG_SDM_USBPLL_PU_PLL_DR (1 << 4)
#define ANALOG_REG_SDM_USBPLL_SDM_CLK_SEL_NOR (1 << 5)
#define ANALOG_REG_SDM_USBPLL_SDM_CLK_SEL_RST (1 << 6)
#define ANALOG_REG_SDM_USBPLL_PU (1 << 7)
#define ANALOG_REG_SDM_USBPLL_FBC_INV (1 << 8)

// sdm_usbpll_reg1
#define ANALOG_REG_SDM_USBPLL_SDM_FREQ(n) (((n)&0xffffffff) << 0)

// sdm_usbpll_reg2
#define ANALOG_REG_SDM_USBPLL_LOCK_COUNTER_SEL(n) (((n)&0x3) << 0)
#define ANALOG_REG_SDM_USBPLL_CLKOUT_EN_COUNTER_SEL(n) (((n)&0x3) << 2)
#define ANALOG_REG_SDM_USBPLL_CLK_GEN_EN_REG (1 << 4)
#define ANALOG_REG_SDM_USBPLL_SDMCLK_SEL_TIME_SEL(n) (((n)&0x3) << 5)
#define ANALOG_REG_SDM_USBPLL_SDM_RESET_TIME_SEL(n) (((n)&0x3) << 7)
#define ANALOG_REG_SDM_USBPLL_SDM_RESETN_REG (1 << 9)
#define ANALOG_REG_SDM_USBPLL_SDM_RESETN_DR (1 << 10)
#define ANALOG_REG_SDM_USBPLL_DITHER_BYPASS (1 << 11)

// sdm_usbpll_reg3
#define ANALOG_REG_SDM_USBPLL_CLK_READY (1 << 0)
#define ANALOG_REG_SDM_USBPLL_LOCK_STEADY (1 << 1)
#define ANALOG_REG_SDM_USBPLL_SDM_RESETN (1 << 2)

// sdm_usbpll_reg4
#define ANALOG_REG_SDM_USBPLL_SS_DEVI_STEP(n) (((n)&0xffff) << 0)
#define ANALOG_REG_SDM_USBPLL_SS_DEVI(n) (((n)&0xfff) << 16)
#define ANALOG_REG_SDM_USBPLL_SS_EN (1 << 28)
#define ANALOG_REG_SDM_USBPLL_SDM_EN (1 << 29)

// usb_digphy_ana1
#define ANALOG_REG_USB_DIGPHY_ANA_1(n) (((n)&0xffffffff) << 0)

// usb_digphy_ana2
#define ANALOG_REG_USB_DIGPHY_ANA_2(n) (((n)&0xffffffff) << 0)

// usb_digphy_ana3
#define ANALOG_REG_USB_DIGPHY_ANA_3(n) (((n)&0xffffffff) << 0)

// usb_digphy_ana4
#define ANALOG_REG_USB_DIGPHY_ANA_4(n) (((n)&0xffffffff) << 0)

// usb11_digphy_ana_reg
#define ANALOG_REG_USB11_DIGPHY_ANA(n) (((n)&0xffffffff) << 0)

// ddr_pad_cfg
#define ANALOG_REG_DDR_LATCH (1 << 0)

// psram_pad_cfg
#define ANALOG_REG_PSRAM_LATCH (1 << 0)

// sdm_dsipll_reg0
#define ANALOG_REG_SDM_DSIPLL_INT_DEC_SEL(n) (((n)&0x7) << 0)
#define ANALOG_REG_SDM_DSIPLL_PU_PLL_REG (1 << 3)
#define ANALOG_REG_SDM_DSIPLL_PU_PLL_DR (1 << 4)
#define ANALOG_REG_SDM_DSIPLL_SDM_CLK_SEL_NOR (1 << 5)
#define ANALOG_REG_SDM_DSIPLL_SDM_CLK_SEL_RST (1 << 6)
#define ANALOG_REG_SDM_DSIPLL_PU (1 << 7)
#define ANALOG_REG_SDM_DSIPLL_FBC_INV (1 << 8)

// sdm_dsipll_reg1
#define ANALOG_REG_SDM_DSIPLL_SDM_FREQ(n) (((n)&0xffffffff) << 0)

// sdm_dsipll_reg2
#define ANALOG_REG_SDM_DSIPLL_LOCK_COUNTER_SEL(n) (((n)&0x3) << 0)
#define ANALOG_REG_SDM_DSIPLL_CLKOUT_EN_COUNTER_SEL(n) (((n)&0x3) << 2)
#define ANALOG_REG_SDM_DSIPLL_CLK_GEN_EN_REG (1 << 4)
#define ANALOG_REG_SDM_DSIPLL_SDMCLK_SEL_TIME_SEL(n) (((n)&0x3) << 5)
#define ANALOG_REG_SDM_DSIPLL_SDM_RESET_TIME_SEL(n) (((n)&0x3) << 7)
#define ANALOG_REG_SDM_DSIPLL_SDM_RESETN_REG (1 << 9)
#define ANALOG_REG_SDM_DSIPLL_SDM_RESETN_DR (1 << 10)
#define ANALOG_REG_SDM_DSIPLL_DITHER_BYPASS (1 << 11)

// sdm_dsipll_reg3
#define ANALOG_REG_SDM_DSIPLL_CLK_READY (1 << 0)
#define ANALOG_REG_SDM_DSIPLL_LOCK_STEADY (1 << 1)
#define ANALOG_REG_SDM_DSIPLL_SDM_RESETN (1 << 2)

// sdm_dsipll_reg4
#define ANALOG_REG_SDM_DSIPLL_SS_DEVI_STEP(n) (((n)&0xffff) << 0)
#define ANALOG_REG_SDM_DSIPLL_SS_DEVI(n) (((n)&0xfff) << 16)
#define ANALOG_REG_SDM_DSIPLL_SS_EN (1 << 28)
#define ANALOG_REG_SDM_DSIPLL_SDM_EN (1 << 29)

// mipi_lvds_phy_reg
#define ANALOG_REG_CSI_LVDS_MODE (1 << 0)
#define ANALOG_REG_LVDS_RX_TERMINAL_ENABLE (1 << 1)
#define ANALOG_REG_LVDSPLL_SDM_SEL (1 << 2)
#define ANALOG_REG_LVDSPLL_REFMULTI2_EN (1 << 3)

// pad_ctrl_resv
#define ANALOG_REG_REG_PAD_CTRL_RESV(n) (((n)&0xffffffff) << 0)

// pad_resetb_cfg
#define ANALOG_REG_PAD_RESETB_WPUS (1 << 0)
#define ANALOG_REG_PAD_RESETB_SE (1 << 1)
#define ANALOG_REG_PAD_RESETB_IE (1 << 2)
#define ANALOG_REG_PAD_RESETB_DRV(n) (((n)&0x3) << 3)

// pad_adi_cfg
#define ANALOG_REG_PAD_ADI_SYNC_WPUS (1 << 0)
#define ANALOG_REG_PAD_ADI_SYNC_SE (1 << 1)
#define ANALOG_REG_PAD_ADI_SYNC_IE (1 << 2)
#define ANALOG_REG_PAD_ADI_SYNC_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_ADI_SDA_WPUS (1 << 5)
#define ANALOG_REG_PAD_ADI_SDA_SE (1 << 6)
#define ANALOG_REG_PAD_ADI_SDA_IE (1 << 7)
#define ANALOG_REG_PAD_ADI_SDA_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_ADI_SCL_WPUS (1 << 10)
#define ANALOG_REG_PAD_ADI_SCL_SE (1 << 11)
#define ANALOG_REG_PAD_ADI_SCL_IE (1 << 12)
#define ANALOG_REG_PAD_ADI_SCL_DRV(n) (((n)&0x3) << 13)

// pad_ap_jtag_cfg
#define ANALOG_REG_PAD_AP_JTAG_TRST_WPUS (1 << 0)
#define ANALOG_REG_PAD_AP_JTAG_TRST_SE (1 << 1)
#define ANALOG_REG_PAD_AP_JTAG_TRST_IE (1 << 2)
#define ANALOG_REG_PAD_AP_JTAG_TRST_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_AP_JTAG_TMS_WPUS (1 << 5)
#define ANALOG_REG_PAD_AP_JTAG_TMS_SE (1 << 6)
#define ANALOG_REG_PAD_AP_JTAG_TMS_IE (1 << 7)
#define ANALOG_REG_PAD_AP_JTAG_TMS_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_AP_JTAG_TDO_WPUS (1 << 10)
#define ANALOG_REG_PAD_AP_JTAG_TDO_SE (1 << 11)
#define ANALOG_REG_PAD_AP_JTAG_TDO_IE (1 << 12)
#define ANALOG_REG_PAD_AP_JTAG_TDO_DRV(n) (((n)&0x3) << 13)
#define ANALOG_REG_PAD_AP_JTAG_TDI_WPUS (1 << 15)
#define ANALOG_REG_PAD_AP_JTAG_TDI_SE (1 << 16)
#define ANALOG_REG_PAD_AP_JTAG_TDI_IE (1 << 17)
#define ANALOG_REG_PAD_AP_JTAG_TDI_DRV(n) (((n)&0x3) << 18)
#define ANALOG_REG_PAD_AP_JTAG_TCK_WPUS (1 << 20)
#define ANALOG_REG_PAD_AP_JTAG_TCK_SE (1 << 21)
#define ANALOG_REG_PAD_AP_JTAG_TCK_IE (1 << 22)
#define ANALOG_REG_PAD_AP_JTAG_TCK_DRV(n) (((n)&0x3) << 23)

// pad_spi_camera_cfg
#define ANALOG_REG_PAD_SPI_CAMERA_SI_1_WPUS (1 << 0)
#define ANALOG_REG_PAD_SPI_CAMERA_SI_1_SE (1 << 1)
#define ANALOG_REG_PAD_SPI_CAMERA_SI_1_IE (1 << 2)
#define ANALOG_REG_PAD_SPI_CAMERA_SI_1_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_SPI_CAMERA_SI_0_WPUS (1 << 5)
#define ANALOG_REG_PAD_SPI_CAMERA_SI_0_SE (1 << 6)
#define ANALOG_REG_PAD_SPI_CAMERA_SI_0_IE (1 << 7)
#define ANALOG_REG_PAD_SPI_CAMERA_SI_0_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_SPI_CAMERA_SCK_WPUS (1 << 10)
#define ANALOG_REG_PAD_SPI_CAMERA_SCK_SE (1 << 11)
#define ANALOG_REG_PAD_SPI_CAMERA_SCK_IE (1 << 12)
#define ANALOG_REG_PAD_SPI_CAMERA_SCK_DRV(n) (((n)&0x3) << 13)

// pad_camera_cfg
#define ANALOG_REG_PAD_CAMERA_RST_L_WPUS (1 << 0)
#define ANALOG_REG_PAD_CAMERA_RST_L_SE (1 << 1)
#define ANALOG_REG_PAD_CAMERA_RST_L_IE (1 << 2)
#define ANALOG_REG_PAD_CAMERA_RST_L_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_CAMERA_REF_CLK_WPUS (1 << 5)
#define ANALOG_REG_PAD_CAMERA_REF_CLK_SE (1 << 6)
#define ANALOG_REG_PAD_CAMERA_REF_CLK_IE (1 << 7)
#define ANALOG_REG_PAD_CAMERA_REF_CLK_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_CAMERA_PWDN_WPUS (1 << 10)
#define ANALOG_REG_PAD_CAMERA_PWDN_SE (1 << 11)
#define ANALOG_REG_PAD_CAMERA_PWDN_IE (1 << 12)
#define ANALOG_REG_PAD_CAMERA_PWDN_DRV(n) (((n)&0x3) << 13)
#define ANALOG_REG_PAD_CAMERA_WPDI (1 << 15)

// pad_debug_host_cfg
#define ANALOG_REG_PAD_DEBUG_HOST_TX_WPUS (1 << 0)
#define ANALOG_REG_PAD_DEBUG_HOST_TX_SE (1 << 1)
#define ANALOG_REG_PAD_DEBUG_HOST_TX_IE (1 << 2)
#define ANALOG_REG_PAD_DEBUG_HOST_TX_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_DEBUG_HOST_RX_WPUS (1 << 5)
#define ANALOG_REG_PAD_DEBUG_HOST_RX_SE (1 << 6)
#define ANALOG_REG_PAD_DEBUG_HOST_RX_IE (1 << 7)
#define ANALOG_REG_PAD_DEBUG_HOST_RX_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_DEBUG_HOST_CLK_WPUS (1 << 10)
#define ANALOG_REG_PAD_DEBUG_HOST_CLK_SE (1 << 11)
#define ANALOG_REG_PAD_DEBUG_HOST_CLK_IE (1 << 12)
#define ANALOG_REG_PAD_DEBUG_HOST_CLK_DRV(n) (((n)&0x3) << 13)

// pad_gpio_cfg1
#define ANALOG_REG_PAD_GPIO_3_WPUS (1 << 0)
#define ANALOG_REG_PAD_GPIO_3_SE (1 << 1)
#define ANALOG_REG_PAD_GPIO_3_IE (1 << 2)
#define ANALOG_REG_PAD_GPIO_3_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_GPIO_2_WPUS (1 << 5)
#define ANALOG_REG_PAD_GPIO_2_SE (1 << 6)
#define ANALOG_REG_PAD_GPIO_2_IE (1 << 7)
#define ANALOG_REG_PAD_GPIO_2_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_GPIO_1_WPUS (1 << 10)
#define ANALOG_REG_PAD_GPIO_1_SE (1 << 11)
#define ANALOG_REG_PAD_GPIO_1_IE (1 << 12)
#define ANALOG_REG_PAD_GPIO_1_DRV(n) (((n)&0x3) << 13)
#define ANALOG_REG_PAD_GPIO_0_WPUS (1 << 15)
#define ANALOG_REG_PAD_GPIO_0_SE (1 << 16)
#define ANALOG_REG_PAD_GPIO_0_IE (1 << 17)
#define ANALOG_REG_PAD_GPIO_0_DRV(n) (((n)&0x3) << 18)

// pad_gpio_cfg2
#define ANALOG_REG_PAD_GPIO_7_WPUS (1 << 0)
#define ANALOG_REG_PAD_GPIO_7_SE (1 << 1)
#define ANALOG_REG_PAD_GPIO_7_IE (1 << 2)
#define ANALOG_REG_PAD_GPIO_7_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_GPIO_5_WPUS (1 << 5)
#define ANALOG_REG_PAD_GPIO_5_SE (1 << 6)
#define ANALOG_REG_PAD_GPIO_5_IE (1 << 7)
#define ANALOG_REG_PAD_GPIO_5_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_GPIO_4_WPUS (1 << 10)
#define ANALOG_REG_PAD_GPIO_4_SE (1 << 11)
#define ANALOG_REG_PAD_GPIO_4_IE (1 << 12)
#define ANALOG_REG_PAD_GPIO_4_DRV(n) (((n)&0x3) << 13)

// pad_gpo_cfg
#define ANALOG_REG_PAD_GPIO_12_WPUS (1 << 0)
#define ANALOG_REG_PAD_GPIO_12_SE (1 << 1)
#define ANALOG_REG_PAD_GPIO_12_IE (1 << 2)
#define ANALOG_REG_PAD_GPIO_12_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_GPIO_11_WPUS (1 << 5)
#define ANALOG_REG_PAD_GPIO_11_SE (1 << 6)
#define ANALOG_REG_PAD_GPIO_11_IE (1 << 7)
#define ANALOG_REG_PAD_GPIO_11_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_GPIO_10_WPUS (1 << 10)
#define ANALOG_REG_PAD_GPIO_10_SE (1 << 11)
#define ANALOG_REG_PAD_GPIO_10_IE (1 << 12)
#define ANALOG_REG_PAD_GPIO_10_DRV(n) (((n)&0x3) << 13)
#define ANALOG_REG_PAD_GPIO_9_WPUS (1 << 15)
#define ANALOG_REG_PAD_GPIO_9_SE (1 << 16)
#define ANALOG_REG_PAD_GPIO_9_IE (1 << 17)
#define ANALOG_REG_PAD_GPIO_9_DRV(n) (((n)&0x3) << 18)
#define ANALOG_REG_PAD_GPIO_8_WPUS (1 << 20)
#define ANALOG_REG_PAD_GPIO_8_SE (1 << 21)
#define ANALOG_REG_PAD_GPIO_8_IE (1 << 22)
#define ANALOG_REG_PAD_GPIO_8_DRV(n) (((n)&0x3) << 23)

// pad_i2c_cfg
#define ANALOG_REG_PAD_GPIO_15_WPUS (1 << 0)
#define ANALOG_REG_PAD_GPIO_15_SE (1 << 1)
#define ANALOG_REG_PAD_GPIO_15_IE (1 << 2)
#define ANALOG_REG_PAD_GPIO_15_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_GPIO_14_WPUS (1 << 5)
#define ANALOG_REG_PAD_GPIO_14_SE (1 << 6)
#define ANALOG_REG_PAD_GPIO_14_IE (1 << 7)
#define ANALOG_REG_PAD_GPIO_14_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_I2C_M1_SDA_WPUS (1 << 10)
#define ANALOG_REG_PAD_I2C_M1_SDA_SE (1 << 11)
#define ANALOG_REG_PAD_I2C_M1_SDA_IE (1 << 12)
#define ANALOG_REG_PAD_I2C_M1_SDA_DRV(n) (((n)&0x3) << 13)
#define ANALOG_REG_PAD_I2C_M1_SCL_WPUS (1 << 15)
#define ANALOG_REG_PAD_I2C_M1_SCL_SE (1 << 16)
#define ANALOG_REG_PAD_I2C_M1_SCL_IE (1 << 17)
#define ANALOG_REG_PAD_I2C_M1_SCL_DRV(n) (((n)&0x3) << 18)

// pad_aud_ad_cfg
#define ANALOG_REG_PAD_AUD_SCLK_WPUS (1 << 0)
#define ANALOG_REG_PAD_AUD_SCLK_SE (1 << 1)
#define ANALOG_REG_PAD_AUD_SCLK_IE (1 << 2)
#define ANALOG_REG_PAD_AUD_SCLK_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_AUD_DA_SYNC_WPUS (1 << 5)
#define ANALOG_REG_PAD_AUD_DA_SYNC_SE (1 << 6)
#define ANALOG_REG_PAD_AUD_DA_SYNC_IE (1 << 7)
#define ANALOG_REG_PAD_AUD_DA_SYNC_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_AUD_DA_D1_WPUS (1 << 10)
#define ANALOG_REG_PAD_AUD_DA_D1_SE (1 << 11)
#define ANALOG_REG_PAD_AUD_DA_D1_IE (1 << 12)
#define ANALOG_REG_PAD_AUD_DA_D1_DRV(n) (((n)&0x3) << 13)
#define ANALOG_REG_PAD_AUD_DA_D0_WPUS (1 << 15)
#define ANALOG_REG_PAD_AUD_DA_D0_SE (1 << 16)
#define ANALOG_REG_PAD_AUD_DA_D0_IE (1 << 17)
#define ANALOG_REG_PAD_AUD_DA_D0_DRV(n) (((n)&0x3) << 18)
#define ANALOG_REG_PAD_AUD_AD_SYNC_WPUS (1 << 20)
#define ANALOG_REG_PAD_AUD_AD_SYNC_SE (1 << 21)
#define ANALOG_REG_PAD_AUD_AD_SYNC_IE (1 << 22)
#define ANALOG_REG_PAD_AUD_AD_SYNC_DRV(n) (((n)&0x3) << 23)
#define ANALOG_REG_PAD_AUD_AD_D0_WPUS (1 << 25)
#define ANALOG_REG_PAD_AUD_AD_D0_SE (1 << 26)
#define ANALOG_REG_PAD_AUD_AD_D0_IE (1 << 27)
#define ANALOG_REG_PAD_AUD_AD_D0_DRV(n) (((n)&0x3) << 28)

// pad_keyin_cfg
#define ANALOG_REG_PAD_KEYIN_5_WPUS (1 << 0)
#define ANALOG_REG_PAD_KEYIN_5_SE (1 << 1)
#define ANALOG_REG_PAD_KEYIN_5_IE (1 << 2)
#define ANALOG_REG_PAD_KEYIN_5_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_KEYIN_4_WPUS (1 << 5)
#define ANALOG_REG_PAD_KEYIN_4_SE (1 << 6)
#define ANALOG_REG_PAD_KEYIN_4_IE (1 << 7)
#define ANALOG_REG_PAD_KEYIN_4_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_KEYIN_3_WPUS (1 << 10)
#define ANALOG_REG_PAD_KEYIN_3_SE (1 << 11)
#define ANALOG_REG_PAD_KEYIN_3_IE (1 << 12)
#define ANALOG_REG_PAD_KEYIN_3_DRV(n) (((n)&0x3) << 13)
#define ANALOG_REG_PAD_KEYIN_2_WPUS (1 << 15)
#define ANALOG_REG_PAD_KEYIN_2_SE (1 << 16)
#define ANALOG_REG_PAD_KEYIN_2_IE (1 << 17)
#define ANALOG_REG_PAD_KEYIN_2_DRV(n) (((n)&0x3) << 18)
#define ANALOG_REG_PAD_KEYIN_1_WPUS (1 << 20)
#define ANALOG_REG_PAD_KEYIN_1_SE (1 << 21)
#define ANALOG_REG_PAD_KEYIN_1_IE (1 << 22)
#define ANALOG_REG_PAD_KEYIN_1_DRV(n) (((n)&0x3) << 23)
#define ANALOG_REG_PAD_KEYIN_0_WPUS (1 << 25)
#define ANALOG_REG_PAD_KEYIN_0_SE (1 << 26)
#define ANALOG_REG_PAD_KEYIN_0_IE (1 << 27)
#define ANALOG_REG_PAD_KEYIN_0_DRV(n) (((n)&0x3) << 28)
#define ANALOG_REG_PAD_KEY_WPDI (1 << 30)

// pad_keyout_cfg
#define ANALOG_REG_PAD_KEYOUT_5_WPUS (1 << 0)
#define ANALOG_REG_PAD_KEYOUT_5_SE (1 << 1)
#define ANALOG_REG_PAD_KEYOUT_5_IE (1 << 2)
#define ANALOG_REG_PAD_KEYOUT_5_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_KEYOUT_4_WPUS (1 << 5)
#define ANALOG_REG_PAD_KEYOUT_4_SE (1 << 6)
#define ANALOG_REG_PAD_KEYOUT_4_IE (1 << 7)
#define ANALOG_REG_PAD_KEYOUT_4_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_KEYOUT_3_WPUS (1 << 10)
#define ANALOG_REG_PAD_KEYOUT_3_SE (1 << 11)
#define ANALOG_REG_PAD_KEYOUT_3_IE (1 << 12)
#define ANALOG_REG_PAD_KEYOUT_3_DRV(n) (((n)&0x3) << 13)
#define ANALOG_REG_PAD_KEYOUT_2_WPUS (1 << 15)
#define ANALOG_REG_PAD_KEYOUT_2_SE (1 << 16)
#define ANALOG_REG_PAD_KEYOUT_2_IE (1 << 17)
#define ANALOG_REG_PAD_KEYOUT_2_DRV(n) (((n)&0x3) << 18)
#define ANALOG_REG_PAD_KEYOUT_1_WPUS (1 << 20)
#define ANALOG_REG_PAD_KEYOUT_1_SE (1 << 21)
#define ANALOG_REG_PAD_KEYOUT_1_IE (1 << 22)
#define ANALOG_REG_PAD_KEYOUT_1_DRV(n) (((n)&0x3) << 23)
#define ANALOG_REG_PAD_KEYOUT_0_WPUS (1 << 25)
#define ANALOG_REG_PAD_KEYOUT_0_SE (1 << 26)
#define ANALOG_REG_PAD_KEYOUT_0_IE (1 << 27)
#define ANALOG_REG_PAD_KEYOUT_0_DRV(n) (((n)&0x3) << 28)

// pad_lcd_cfg
#define ANALOG_REG_PAD_LCD_RSTB_SPU (1 << 0)
#define ANALOG_REG_PAD_LCD_RSTB_SE (1 << 1)
#define ANALOG_REG_PAD_LCD_RSTB_IE (1 << 2)
#define ANALOG_REG_PAD_LCD_RSTB_DRV(n) (((n)&0xf) << 3)
#define ANALOG_REG_PAD_LCD_FMARK_SPU (1 << 7)
#define ANALOG_REG_PAD_LCD_FMARK_SE (1 << 8)
#define ANALOG_REG_PAD_LCD_FMARK_IE (1 << 9)
#define ANALOG_REG_PAD_LCD_FMARK_DRV(n) (((n)&0xf) << 10)
#define ANALOG_REG_PAD_LCD_MS (1 << 14)
#define ANALOG_REG_PAD_LCD_MSEN (1 << 15)

// pad_gpio13_cfg
#define ANALOG_REG_PAD_GPIO_13_WPUS (1 << 0)
#define ANALOG_REG_PAD_GPIO_13_SE (1 << 1)
#define ANALOG_REG_PAD_GPIO_13_IE (1 << 2)
#define ANALOG_REG_PAD_GPIO_13_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_MISC_R_WPDI (1 << 5)
#define ANALOG_REG_PAD_MISC_L_WPDI (1 << 6)
#define ANALOG_REG_PAD_MIPIDSI_WPDI (1 << 7)

// pad_rfdig_gpio_cfg1
#define ANALOG_REG_PAD_RFDIG_GPIO_4_WPUS (1 << 0)
#define ANALOG_REG_PAD_RFDIG_GPIO_4_SE (1 << 1)
#define ANALOG_REG_PAD_RFDIG_GPIO_4_IE (1 << 2)
#define ANALOG_REG_PAD_RFDIG_GPIO_4_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_RFDIG_GPIO_3_WPUS (1 << 5)
#define ANALOG_REG_PAD_RFDIG_GPIO_3_SE (1 << 6)
#define ANALOG_REG_PAD_RFDIG_GPIO_3_IE (1 << 7)
#define ANALOG_REG_PAD_RFDIG_GPIO_3_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_RFDIG_GPIO_2_WPUS (1 << 10)
#define ANALOG_REG_PAD_RFDIG_GPIO_2_SE (1 << 11)
#define ANALOG_REG_PAD_RFDIG_GPIO_2_IE (1 << 12)
#define ANALOG_REG_PAD_RFDIG_GPIO_2_DRV(n) (((n)&0x3) << 13)
#define ANALOG_REG_PAD_RFDIG_GPIO_1_WPUS (1 << 15)
#define ANALOG_REG_PAD_RFDIG_GPIO_1_SE (1 << 16)
#define ANALOG_REG_PAD_RFDIG_GPIO_1_IE (1 << 17)
#define ANALOG_REG_PAD_RFDIG_GPIO_1_DRV(n) (((n)&0x3) << 18)
#define ANALOG_REG_PAD_RFDIG_GPIO_0_WPUS (1 << 20)
#define ANALOG_REG_PAD_RFDIG_GPIO_0_SE (1 << 21)
#define ANALOG_REG_PAD_RFDIG_GPIO_0_IE (1 << 22)
#define ANALOG_REG_PAD_RFDIG_GPIO_0_DRV(n) (((n)&0x3) << 23)
#define ANALOG_REG_PAD_RF_WPDI (1 << 25)

// pad_rfdig_gpio_cfg2
#define ANALOG_REG_PAD_RFDIG_GPIO_7_WPUS (1 << 0)
#define ANALOG_REG_PAD_RFDIG_GPIO_7_SE (1 << 1)
#define ANALOG_REG_PAD_RFDIG_GPIO_7_IE (1 << 2)
#define ANALOG_REG_PAD_RFDIG_GPIO_7_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_RFDIG_GPIO_6_WPUS (1 << 5)
#define ANALOG_REG_PAD_RFDIG_GPIO_6_SE (1 << 6)
#define ANALOG_REG_PAD_RFDIG_GPIO_6_IE (1 << 7)
#define ANALOG_REG_PAD_RFDIG_GPIO_6_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_RFDIG_GPIO_5_WPUS (1 << 10)
#define ANALOG_REG_PAD_RFDIG_GPIO_5_SE (1 << 11)
#define ANALOG_REG_PAD_RFDIG_GPIO_5_IE (1 << 12)
#define ANALOG_REG_PAD_RFDIG_GPIO_5_DRV(n) (((n)&0x3) << 13)

// pad_sdmmc1_others_cfg
#define ANALOG_REG_PAD_SDMMC1_CMD_SPU (1 << 0)
#define ANALOG_REG_PAD_SDMMC1_CMD_SE (1 << 1)
#define ANALOG_REG_PAD_SDMMC1_CMD_IE (1 << 2)
#define ANALOG_REG_PAD_SDMMC1_CMD_DRV(n) (((n)&0xf) << 3)
#define ANALOG_REG_PAD_SDMMC1_CLK_SPU (1 << 7)
#define ANALOG_REG_PAD_SDMMC1_CLK_SE (1 << 8)
#define ANALOG_REG_PAD_SDMMC1_CLK_IE (1 << 9)
#define ANALOG_REG_PAD_SDMMC1_CLK_DRV(n) (((n)&0xf) << 10)
#define ANALOG_REG_PAD_SDMMC1_MSEN (1 << 14)
#define ANALOG_REG_PAD_SDMMC1_MS (1 << 15)

// pad_sdmmc1_data_cfg
#define ANALOG_REG_PAD_SDMMC1_DATA_3_SPU (1 << 0)
#define ANALOG_REG_PAD_SDMMC1_DATA_3_SE (1 << 1)
#define ANALOG_REG_PAD_SDMMC1_DATA_3_IE (1 << 2)
#define ANALOG_REG_PAD_SDMMC1_DATA_3_DRV(n) (((n)&0xf) << 3)
#define ANALOG_REG_PAD_SDMMC1_DATA_2_SPU (1 << 7)
#define ANALOG_REG_PAD_SDMMC1_DATA_2_SE (1 << 8)
#define ANALOG_REG_PAD_SDMMC1_DATA_2_IE (1 << 9)
#define ANALOG_REG_PAD_SDMMC1_DATA_2_DRV(n) (((n)&0xf) << 10)
#define ANALOG_REG_PAD_SDMMC1_DATA_1_SPU (1 << 14)
#define ANALOG_REG_PAD_SDMMC1_DATA_1_SE (1 << 15)
#define ANALOG_REG_PAD_SDMMC1_DATA_1_IE (1 << 16)
#define ANALOG_REG_PAD_SDMMC1_DATA_1_DRV(n) (((n)&0xf) << 17)
#define ANALOG_REG_PAD_SDMMC1_DATA_0_SPU (1 << 21)
#define ANALOG_REG_PAD_SDMMC1_DATA_0_SE (1 << 22)
#define ANALOG_REG_PAD_SDMMC1_DATA_0_IE (1 << 23)
#define ANALOG_REG_PAD_SDMMC1_DATA_0_DRV(n) (((n)&0xf) << 24)

// pad_sim_1_cfg
#define ANALOG_REG_PAD_SIM_1_RST_SPU (1 << 0)
#define ANALOG_REG_PAD_SIM_1_RST_SE (1 << 1)
#define ANALOG_REG_PAD_SIM_1_RST_IE (1 << 2)
#define ANALOG_REG_PAD_SIM_1_RST_DRV(n) (((n)&0xf) << 3)
#define ANALOG_REG_PAD_SIM_1_DIO_SPU (1 << 7)
#define ANALOG_REG_PAD_SIM_1_DIO_SE (1 << 8)
#define ANALOG_REG_PAD_SIM_1_DIO_IE (1 << 9)
#define ANALOG_REG_PAD_SIM_1_DIO_DRV(n) (((n)&0xf) << 10)
#define ANALOG_REG_PAD_SIM_1_CLK_SPU (1 << 14)
#define ANALOG_REG_PAD_SIM_1_CLK_SE (1 << 15)
#define ANALOG_REG_PAD_SIM_1_CLK_IE (1 << 16)
#define ANALOG_REG_PAD_SIM_1_CLK_DRV(n) (((n)&0xf) << 17)
#define ANALOG_REG_PAD_SIM_1_MSEN (1 << 21)
#define ANALOG_REG_PAD_SIM_1_MS (1 << 22)

// pad_sim_2_cfg
#define ANALOG_REG_PAD_SIM_2_RST_SPU (1 << 0)
#define ANALOG_REG_PAD_SIM_2_RST_SE (1 << 1)
#define ANALOG_REG_PAD_SIM_2_RST_IE (1 << 2)
#define ANALOG_REG_PAD_SIM_2_RST_DRV(n) (((n)&0xf) << 3)
#define ANALOG_REG_PAD_SIM_2_DIO_SPU (1 << 7)
#define ANALOG_REG_PAD_SIM_2_DIO_SE (1 << 8)
#define ANALOG_REG_PAD_SIM_2_DIO_IE (1 << 9)
#define ANALOG_REG_PAD_SIM_2_DIO_DRV(n) (((n)&0xf) << 10)
#define ANALOG_REG_PAD_SIM_2_CLK_SPU (1 << 14)
#define ANALOG_REG_PAD_SIM_2_CLK_SE (1 << 15)
#define ANALOG_REG_PAD_SIM_2_CLK_IE (1 << 16)
#define ANALOG_REG_PAD_SIM_2_CLK_DRV(n) (((n)&0xf) << 17)
#define ANALOG_REG_PAD_SIM_2_MSEN (1 << 21)
#define ANALOG_REG_PAD_SIM_2_MS (1 << 22)

// pad_spi_flash_cfg1
#define ANALOG_REG_PAD_SPI_FLASH_CS_SPU (1 << 0)
#define ANALOG_REG_PAD_SPI_FLASH_CS_SE (1 << 1)
#define ANALOG_REG_PAD_SPI_FLASH_CS_IE (1 << 2)
#define ANALOG_REG_PAD_SPI_FLASH_CS_DRV(n) (((n)&0xf) << 3)
#define ANALOG_REG_PAD_SPI_FLASH_CLK_SPU (1 << 7)
#define ANALOG_REG_PAD_SPI_FLASH_CLK_SE (1 << 8)
#define ANALOG_REG_PAD_SPI_FLASH_CLK_IE (1 << 9)
#define ANALOG_REG_PAD_SPI_FLASH_CLK_DRV(n) (((n)&0xf) << 10)
#define ANALOG_REG_PAD_SPI_FLASH_SEL_SPU (1 << 14)
#define ANALOG_REG_PAD_SPI_FLASH_SEL_SE (1 << 15)
#define ANALOG_REG_PAD_SPI_FLASH_SEL_IE (1 << 16)
#define ANALOG_REG_PAD_SPI_FLASH_SEL_DRV(n) (((n)&0xf) << 17)
#define ANALOG_REG_PAD_NAND_FLASH_SEL_WPUS (1 << 21)
#define ANALOG_REG_PAD_NAND_FLASH_SEL_SE (1 << 22)
#define ANALOG_REG_PAD_NAND_FLASH_SEL_IE (1 << 23)
#define ANALOG_REG_PAD_NAND_FLASH_SEL_DRV(n) (((n)&0x3) << 24)

// pad_spi_flash_cfg2
#define ANALOG_REG_PAD_SPI_FLASH_SIO_3_SPU (1 << 0)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_3_SE (1 << 1)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_3_IE (1 << 2)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_3_DRV(n) (((n)&0xf) << 3)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_2_SPU (1 << 7)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_2_SE (1 << 8)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_2_IE (1 << 9)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_2_DRV(n) (((n)&0xf) << 10)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_1_SPU (1 << 14)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_1_SE (1 << 15)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_1_IE (1 << 16)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_1_DRV(n) (((n)&0xf) << 17)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_0_SPU (1 << 21)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_0_SE (1 << 22)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_0_IE (1 << 23)
#define ANALOG_REG_PAD_SPI_FLASH_SIO_0_DRV(n) (((n)&0xf) << 24)
#define ANALOG_REG_PAD_SPI_FLASH_MSEN (1 << 28)
#define ANALOG_REG_PAD_SPI_FLASH_MS (1 << 29)

// pad_spi_lcd_cfg1
#define ANALOG_REG_PAD_SPI_LCD_CS_SPU (1 << 0)
#define ANALOG_REG_PAD_SPI_LCD_CS_SE (1 << 1)
#define ANALOG_REG_PAD_SPI_LCD_CS_IE (1 << 2)
#define ANALOG_REG_PAD_SPI_LCD_CS_DRV(n) (((n)&0xf) << 3)
#define ANALOG_REG_PAD_SPI_LCD_CLK_SPU (1 << 7)
#define ANALOG_REG_PAD_SPI_LCD_CLK_SE (1 << 8)
#define ANALOG_REG_PAD_SPI_LCD_CLK_IE (1 << 9)
#define ANALOG_REG_PAD_SPI_LCD_CLK_DRV(n) (((n)&0xf) << 10)

// pad_spi_lcd_cfg2
#define ANALOG_REG_PAD_SPI_LCD_SIO_SPU (1 << 0)
#define ANALOG_REG_PAD_SPI_LCD_SIO_SE (1 << 1)
#define ANALOG_REG_PAD_SPI_LCD_SIO_IE (1 << 2)
#define ANALOG_REG_PAD_SPI_LCD_SIO_DRV(n) (((n)&0xf) << 3)
#define ANALOG_REG_PAD_SPI_LCD_SELECT_SPU (1 << 7)
#define ANALOG_REG_PAD_SPI_LCD_SELECT_SE (1 << 8)
#define ANALOG_REG_PAD_SPI_LCD_SELECT_IE (1 << 9)
#define ANALOG_REG_PAD_SPI_LCD_SELECT_DRV(n) (((n)&0xf) << 10)
#define ANALOG_REG_PAD_SPI_LCD_SDC_SPU (1 << 14)
#define ANALOG_REG_PAD_SPI_LCD_SDC_SE (1 << 15)
#define ANALOG_REG_PAD_SPI_LCD_SDC_IE (1 << 16)
#define ANALOG_REG_PAD_SPI_LCD_SDC_DRV(n) (((n)&0xf) << 17)

// pad_tst_cfg
#define ANALOG_REG_PAD_TST_H_WPUS (1 << 0)
#define ANALOG_REG_PAD_TST_H_SE (1 << 1)
#define ANALOG_REG_PAD_TST_H_IE (1 << 2)
#define ANALOG_REG_PAD_TST_H_DRV(n) (((n)&0x3) << 3)

// pad_uart_1_cfg
#define ANALOG_REG_PAD_GPIO_19_WPUS (1 << 0)
#define ANALOG_REG_PAD_GPIO_19_SE (1 << 1)
#define ANALOG_REG_PAD_GPIO_19_IE (1 << 2)
#define ANALOG_REG_PAD_GPIO_19_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_GPIO_18_WPUS (1 << 5)
#define ANALOG_REG_PAD_GPIO_18_SE (1 << 6)
#define ANALOG_REG_PAD_GPIO_18_IE (1 << 7)
#define ANALOG_REG_PAD_GPIO_18_DRV(n) (((n)&0x3) << 8)

// pad_uart_2_cfg
#define ANALOG_REG_PAD_GPIO_21_WPUS (1 << 0)
#define ANALOG_REG_PAD_GPIO_21_SE (1 << 1)
#define ANALOG_REG_PAD_GPIO_21_IE (1 << 2)
#define ANALOG_REG_PAD_GPIO_21_DRV(n) (((n)&0x3) << 3)
#define ANALOG_REG_PAD_GPIO_20_WPUS (1 << 5)
#define ANALOG_REG_PAD_GPIO_20_SE (1 << 6)
#define ANALOG_REG_PAD_GPIO_20_IE (1 << 7)
#define ANALOG_REG_PAD_GPIO_20_DRV(n) (((n)&0x3) << 8)
#define ANALOG_REG_PAD_GPIO_23_WPUS (1 << 10)
#define ANALOG_REG_PAD_GPIO_23_SE (1 << 11)
#define ANALOG_REG_PAD_GPIO_23_IE (1 << 12)
#define ANALOG_REG_PAD_GPIO_23_DRV(n) (((n)&0x3) << 13)
#define ANALOG_REG_PAD_GPIO_22_WPUS (1 << 15)
#define ANALOG_REG_PAD_GPIO_22_SE (1 << 16)
#define ANALOG_REG_PAD_GPIO_22_IE (1 << 17)
#define ANALOG_REG_PAD_GPIO_22_DRV(n) (((n)&0x3) << 18)

// pad_secure_boot_mode_cfg
#define ANALOG_REG_PAD_SECURE_BOOT_MODE_WPUS (1 << 0)
#define ANALOG_REG_PAD_SECURE_BOOT_MODE_SE (1 << 1)
#define ANALOG_REG_PAD_SECURE_BOOT_MODE_IE (1 << 2)
#define ANALOG_REG_PAD_SECURE_BOOT_MODE_DRV(n) (((n)&0x3) << 3)

// resv0
#define ANALOG_REG_REG_RESV0(n) (((n)&0xffffffff) << 0)

// resv1
#define ANALOG_REG_REG_RESV1(n) (((n)&0xffffffff) << 0)

// resv2
#define ANALOG_REG_REG_RESV2(n) (((n)&0xffffffff) << 0)

// resv3
#define ANALOG_REG_REG_RESV3(n) (((n)&0xffffffff) << 0)

// resv4
#define ANALOG_REG_REG_RESV4(n) (((n)&0xffffffff) << 0)

// resv5
#define ANALOG_REG_REG_RESV5(n) (((n)&0xffffffff) << 0)

// resv6
#define ANALOG_REG_REG_RESV6(n) (((n)&0xffffffff) << 0)

// resv7
#define ANALOG_REG_REG_RESV7(n) (((n)&0xffffffff) << 0)

// resv8
#define ANALOG_REG_REG_RESV8(n) (((n)&0xffffffff) << 0)

// resv9
#define ANALOG_REG_REG_RESV9(n) (((n)&0xffffffff) << 0)

// resv10
#define ANALOG_REG_REG_RESV10(n) (((n)&0xffffffff) << 0)

// resv11
#define ANALOG_REG_REG_RESV11(n) (((n)&0xffffffff) << 0)

// resv12
#define ANALOG_REG_REG_RESV12(n) (((n)&0xffffffff) << 0)

// resv13
#define ANALOG_REG_REG_RESV13(n) (((n)&0xffffffff) << 0)

// resv14
#define ANALOG_REG_REG_RESV14(n) (((n)&0xffffffff) << 0)

// resv15
#define ANALOG_REG_REG_RESV15(n) (((n)&0xffffffff) << 0)

#endif // _ANALOG_REG_H_
