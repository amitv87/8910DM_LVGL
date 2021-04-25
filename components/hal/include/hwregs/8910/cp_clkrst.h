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

#ifndef _CP_CLKRST_H_
#define _CP_CLKRST_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_CLKRST_BASE (0x05081000)
#else
#define REG_CLKRST_BASE (0x50081000)
#endif

typedef volatile struct
{
    uint32_t clksel;                 // 0x00000000
    uint32_t clksel_set;             // 0x00000004
    uint32_t clksel_clr;             // 0x00000008
    uint32_t clkdiv_zsp;             // 0x0000000c
    uint32_t clkdiv_lte;             // 0x00000010
    uint32_t clken_bb_sysctrl;       // 0x00000014
    uint32_t clken_bb_sysctrl_set;   // 0x00000018
    uint32_t clken_bb_sysctrl_clr;   // 0x0000001c
    uint32_t clken_zsp;              // 0x00000020
    uint32_t clken_zsp_set;          // 0x00000024
    uint32_t clken_zsp_clr;          // 0x00000028
    uint32_t clken_lte;              // 0x0000002c
    uint32_t clken_lte_set;          // 0x00000030
    uint32_t clken_lte_clr;          // 0x00000034
    uint32_t clken_zspcore_mode;     // 0x00000038
    uint32_t clken_zsp_axidma_mode;  // 0x0000003c
    uint32_t rstctrl_bb_sysctrl;     // 0x00000040
    uint32_t rstctrl_bb_sysctrl_set; // 0x00000044
    uint32_t rstctrl_bb_sysctrl_clr; // 0x00000048
    uint32_t rstctrl_zsp;            // 0x0000004c
    uint32_t rstctrl_zsp_set;        // 0x00000050
    uint32_t rstctrl_zsp_clr;        // 0x00000054
    uint32_t rstctrl_lte;            // 0x00000058
    uint32_t rstctrl_lte_set;        // 0x0000005c
    uint32_t rstctrl_lte_clr;        // 0x00000060
    uint32_t zsp_soft_rst;           // 0x00000064
    uint32_t lte_soft_rst;           // 0x00000068
    uint32_t zsp_axilpcnt;           // 0x0000006c
    uint32_t zsp_buslpmc;            // 0x00000070
    uint32_t zsp_buslpmc_set;        // 0x00000074
    uint32_t zsp_buslpmc_clr;        // 0x00000078
    uint32_t zsp_busforcelpmc;       // 0x0000007c
    uint32_t zsp_busforcelpmc_set;   // 0x00000080
    uint32_t zsp_busforcelpmc_clr;   // 0x00000084
    uint32_t clken_mailbox_mode;     // 0x00000088
    uint32_t clken_lte_intf;         // 0x0000008c
    uint32_t clken_lte_intf_set;     // 0x00000090
    uint32_t clken_lte_intf_clr;     // 0x00000094
    uint32_t lte_autogate_mode;      // 0x00000098
    uint32_t lte_autogate_en;        // 0x0000009c
    uint32_t lte_autogate_en_set;    // 0x000000a0
    uint32_t lte_autogate_en_clr;    // 0x000000a4
    uint32_t lte_autogate_delay_num; // 0x000000a8
} HWP_CP_CLKRST_T;

#define hwp_clkrst ((HWP_CP_CLKRST_T *)REG_ACCESS_ADDRESS(REG_CLKRST_BASE))

// clksel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t slow_sel_122m : 1;     // [0]
        uint32_t slow_sel_480m : 1;     // [1]
        uint32_t bb_sysctrl_wd_sel : 1; // [2]
        uint32_t zsp_wd_sel : 1;        // [3]
        uint32_t idle_h_sel : 2;        // [5:4]
        uint32_t __31_6 : 26;           // [31:6]
    } b;
} REG_CP_CLKRST_CLKSEL_T;

// clksel_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clkselset : 5; // [4:0], write set
        uint32_t __31_5 : 27;   // [31:5]
    } b;
} REG_CP_CLKRST_CLKSEL_SET_T;

// clksel_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clkselclear : 5; // [4:0], write clear
        uint32_t __31_5 : 27;     // [31:5]
    } b;
} REG_CP_CLKRST_CLKSEL_CLR_T;

// clkdiv_zsp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsp_cd : 4;  // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_CP_CLKRST_CLKDIV_ZSP_T;

// clkdiv_lte
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lte_cd : 4;  // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_CP_CLKRST_CLKDIV_LTE_T;

// clken_bb_sysctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_sysreg : 1;  // [0]
        uint32_t clken_pwrctrl : 1; // [1]
        uint32_t clken_mailbox : 1; // [2]
        uint32_t clken_idle : 1;    // [3]
        uint32_t clken_cp_wd : 1;   // [4]
        uint32_t __31_5 : 27;       // [31:5]
    } b;
} REG_CP_CLKRST_CLKEN_BB_SYSCTRL_T;

// clken_bb_sysctrl_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_bb_sysctrlset : 5; // [4:0], write set
        uint32_t __31_5 : 27;             // [31:5]
    } b;
} REG_CP_CLKRST_CLKEN_BB_SYSCTRL_SET_T;

// clken_bb_sysctrl_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_bb_sysctrlclear : 5; // [4:0], write clear
        uint32_t __31_5 : 27;               // [31:5]
    } b;
} REG_CP_CLKRST_CLKEN_BB_SYSCTRL_CLR_T;

// clken_zsp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_zspcore : 1; // [0]
        uint32_t clken_axidma : 1;  // [1]
        uint32_t clken_zirq : 1;    // [2]
        uint32_t clken_busmon : 1;  // [3]
        uint32_t clken_wd : 1;      // [4]
        uint32_t clken_aud_dft : 1; // [5]
        uint32_t __31_6 : 26;       // [31:6]
    } b;
} REG_CP_CLKRST_CLKEN_ZSP_T;

// clken_zsp_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_zspset : 6; // [5:0], write set
        uint32_t __31_6 : 26;      // [31:6]
    } b;
} REG_CP_CLKRST_CLKEN_ZSP_SET_T;

// clken_zsp_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_zspclear : 6; // [5:0], write clear
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CP_CLKRST_CLKEN_ZSP_CLR_T;

// clken_lte
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_txrx : 1;    // [0]
        uint32_t clken_coeff : 1;   // [1]
        uint32_t clken_ldtc : 1;    // [2]
        uint32_t clken_ldtc1 : 1;   // [3]
        uint32_t clken_measpwr : 1; // [4]
        uint32_t clken_iddet : 1;   // [5]
        uint32_t clken_otdoa : 1;   // [6]
        uint32_t clken_uldft : 1;   // [7]
        uint32_t clken_pusch : 1;   // [8]
        uint32_t clken_csirs : 1;   // [9]
        uint32_t clken_dlfft : 1;   // [10]
        uint32_t clken_rfad : 1;    // [11]
        uint32_t clken_rxcapt : 1;  // [12]
        uint32_t __31_13 : 19;      // [31:13]
    } b;
} REG_CP_CLKRST_CLKEN_LTE_T;

// clken_lte_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_lteset : 13; // [12:0], write set
        uint32_t __31_13 : 19;      // [31:13]
    } b;
} REG_CP_CLKRST_CLKEN_LTE_SET_T;

// clken_lte_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_lteclear : 13; // [12:0], write clear
        uint32_t __31_13 : 19;        // [31:13]
    } b;
} REG_CP_CLKRST_CLKEN_LTE_CLR_T;

// clken_zspcore_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode : 1;    // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_CLKRST_CLKEN_ZSPCORE_MODE_T;

// clken_zsp_axidma_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode : 1;    // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_CLKRST_CLKEN_ZSP_AXIDMA_MODE_T;

// rstctrl_bb_sysctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rstctrl_sysreg : 1;  // [0]
        uint32_t rstctrl_pwrctrl : 1; // [1]
        uint32_t rstctrl_mailbox : 1; // [2]
        uint32_t rstctrl_idle : 1;    // [3]
        uint32_t rstctrl_cp_wd : 1;   // [4]
        uint32_t rstctrl_monitor : 1; // [5]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_CP_CLKRST_RSTCTRL_BB_SYSCTRL_T;

// rstctrl_bb_sysctrl_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rstctrl_bb_sysctrlset : 6; // [5:0], write set
        uint32_t __31_6 : 26;               // [31:6]
    } b;
} REG_CP_CLKRST_RSTCTRL_BB_SYSCTRL_SET_T;

// rstctrl_bb_sysctrl_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rstctrl_bb_sysctrlclear : 6; // [5:0], write clear
        uint32_t __31_6 : 26;                 // [31:6]
    } b;
} REG_CP_CLKRST_RSTCTRL_BB_SYSCTRL_CLR_T;

// rstctrl_zsp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rstctrl_zspcore : 1; // [0]
        uint32_t rstctrl_axidma : 1;  // [1]
        uint32_t rstctrl_zirq : 1;    // [2]
        uint32_t rstctrl_busmon : 1;  // [3]
        uint32_t rstctrl_wd : 1;      // [4]
        uint32_t rstctrl_aud_dft : 1; // [5]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_CP_CLKRST_RSTCTRL_ZSP_T;

// rstctrl_zsp_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rstctrl_zspset : 6; // [5:0], write set
        uint32_t __31_6 : 26;        // [31:6]
    } b;
} REG_CP_CLKRST_RSTCTRL_ZSP_SET_T;

// rstctrl_zsp_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rstctrl_zspclear : 6; // [5:0], write clear
        uint32_t __31_6 : 26;          // [31:6]
    } b;
} REG_CP_CLKRST_RSTCTRL_ZSP_CLR_T;

// rstctrl_lte
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rstctrl_txrx_tx : 1; // [0]
        uint32_t rstctrl_txrx_rx : 1; // [1]
        uint32_t rstctrl_coeff : 1;   // [2]
        uint32_t rstctrl_ldtc : 1;    // [3]
        uint32_t rstctrl_ldtc1 : 1;   // [4]
        uint32_t rstctrl_measpwr : 1; // [5]
        uint32_t rstctrl_iddet : 1;   // [6]
        uint32_t rstctrl_otdoa : 1;   // [7]
        uint32_t rstctrl_uldft : 1;   // [8]
        uint32_t rstctrl_pusch : 1;   // [9]
        uint32_t rstctrl_csirs : 1;   // [10]
        uint32_t rstctrl_dlfft : 1;   // [11]
        uint32_t rstctrl_rfad : 1;    // [12]
        uint32_t rstctrl_rxcapt : 1;  // [13]
        uint32_t __31_14 : 18;        // [31:14]
    } b;
} REG_CP_CLKRST_RSTCTRL_LTE_T;

// rstctrl_lte_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rstctrl_lteset : 14; // [13:0], write set
        uint32_t __31_14 : 18;        // [31:14]
    } b;
} REG_CP_CLKRST_RSTCTRL_LTE_SET_T;

// rstctrl_lte_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rstctrl_lteclear : 14; // [13:0], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_CP_CLKRST_RSTCTRL_LTE_CLR_T;

// zsp_soft_rst
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsp_soft_rst_ctrl : 1; // [0]
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_CP_CLKRST_ZSP_SOFT_RST_T;

// lte_soft_rst
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lte_soft_rst_ctrl : 1; // [0]
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_CP_CLKRST_LTE_SOFT_RST_T;

// zsp_axilpcnt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cnt : 16;     // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_CP_CLKRST_ZSP_AXILPCNT_T;

// zsp_buslpmc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t buslpmc_sw1 : 1;     // [0]
        uint32_t buslpmc_sw2 : 1;     // [1]
        uint32_t buslpmc_phy : 1;     // [2]
        uint32_t buslpmc_zspcore : 1; // [3]
        uint32_t __31_4 : 28;         // [31:4]
    } b;
} REG_CP_CLKRST_ZSP_BUSLPMC_T;

// zsp_buslpmc_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsp_buslpmcset : 4; // [3:0], write set
        uint32_t __31_4 : 28;        // [31:4]
    } b;
} REG_CP_CLKRST_ZSP_BUSLPMC_SET_T;

// zsp_buslpmc_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsp_buslpmcclear : 4; // [3:0], write clear
        uint32_t __31_4 : 28;          // [31:4]
    } b;
} REG_CP_CLKRST_ZSP_BUSLPMC_CLR_T;

// zsp_busforcelpmc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t busforcelpmc_sw1 : 1;     // [0]
        uint32_t busforcelpmc_sw2 : 1;     // [1]
        uint32_t busforcelpmc_phy : 1;     // [2]
        uint32_t busforcelpmc_zspcore : 1; // [3]
        uint32_t __31_4 : 28;              // [31:4]
    } b;
} REG_CP_CLKRST_ZSP_BUSFORCELPMC_T;

// zsp_busforcelpmc_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsp_busforcelpmcset : 4; // [3:0], write set
        uint32_t __31_4 : 28;             // [31:4]
    } b;
} REG_CP_CLKRST_ZSP_BUSFORCELPMC_SET_T;

// zsp_busforcelpmc_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsp_busforcelpmcclear : 4; // [3:0], write clear
        uint32_t __31_4 : 28;               // [31:4]
    } b;
} REG_CP_CLKRST_ZSP_BUSFORCELPMC_CLR_T;

// clken_mailbox_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode : 1;    // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_CLKRST_CLKEN_MAILBOX_MODE_T;

// clken_lte_intf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_txrx_intf : 1;    // [0]
        uint32_t clken_coeff_intf : 1;   // [1]
        uint32_t clken_ldtc_intf : 1;    // [2]
        uint32_t clken_ldtc1_intf : 1;   // [3]
        uint32_t clken_measpwr_intf : 1; // [4]
        uint32_t clken_iddet_intf : 1;   // [5]
        uint32_t clken_otdoa_intf : 1;   // [6]
        uint32_t clken_uldft_intf : 1;   // [7]
        uint32_t clken_pusch_intf : 1;   // [8]
        uint32_t clken_csirs_intf : 1;   // [9]
        uint32_t clken_dlfft_intf : 1;   // [10]
        uint32_t clken_rfad_intf : 1;    // [11]
        uint32_t clken_rxcapt_intf : 1;  // [12]
        uint32_t __31_13 : 19;           // [31:13]
    } b;
} REG_CP_CLKRST_CLKEN_LTE_INTF_T;

// clken_lte_intf_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_lte_intfset : 13; // [12:0], write set
        uint32_t __31_13 : 19;           // [31:13]
    } b;
} REG_CP_CLKRST_CLKEN_LTE_INTF_SET_T;

// clken_lte_intf_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_lte_intfclear : 13; // [12:0], write clear
        uint32_t __31_13 : 19;             // [31:13]
    } b;
} REG_CP_CLKRST_CLKEN_LTE_INTF_CLR_T;

// lte_autogate_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode : 1;    // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_CLKRST_LTE_AUTOGATE_MODE_T;

// lte_autogate_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txrx_func_autogate_en : 1;     // [0]
        uint32_t coeff_func_autogate_en : 1;    // [1]
        uint32_t ldtc_func_autogate_en : 1;     // [2]
        uint32_t ldtc1_func_autogate_en : 1;    // [3]
        uint32_t measpwr_func_autogate_en : 1;  // [4]
        uint32_t iddet_func_autogate_en : 1;    // [5]
        uint32_t otdoa_func_autogate_en : 1;    // [6]
        uint32_t uldft_func_autogate_en : 1;    // [7]
        uint32_t pusch_func_autogate_en : 1;    // [8]
        uint32_t csirs_func_autogate_en : 1;    // [9]
        uint32_t dlfft_func_autogate_en : 1;    // [10]
        uint32_t txrx_intf_autogate_en : 1;     // [11]
        uint32_t coeff_intf_autogate_en : 1;    // [12]
        uint32_t ldtc_intf_autogate_en : 1;     // [13]
        uint32_t ldtc1_intf_autogate_en : 1;    // [14]
        uint32_t measpwr_intf_autogate_en : 1;  // [15]
        uint32_t iddet_intf_autogate_en : 1;    // [16]
        uint32_t otdoa_intf_autogate_en : 1;    // [17]
        uint32_t uldft_intf_autogate_en : 1;    // [18]
        uint32_t pusch_intf_autogate_en : 1;    // [19]
        uint32_t csirs_intf_autogate_en : 1;    // [20]
        uint32_t dlfft_intf_autogate_en : 1;    // [21]
        uint32_t __23_22 : 2;                   // [23:22]
        uint32_t downlink_func_autogate_en : 1; // [24]
        uint32_t uplink_func_autogate_en : 1;   // [25]
        uint32_t downlink_intf_autogate_en : 1; // [26]
        uint32_t uplink_intf_autogate_en : 1;   // [27]
        uint32_t __31_28 : 4;                   // [31:28]
    } b;
} REG_CP_CLKRST_LTE_AUTOGATE_EN_T;

// lte_autogate_en_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_lte_intfset : 28; // [27:0], write set
        uint32_t __31_28 : 4;            // [31:28]
    } b;
} REG_CP_CLKRST_LTE_AUTOGATE_EN_SET_T;

// lte_autogate_en_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clken_lte_intfclear : 28; // [27:0], write clear
        uint32_t __31_28 : 4;              // [31:28]
    } b;
} REG_CP_CLKRST_LTE_AUTOGATE_EN_CLR_T;

// lte_autogate_delay_num
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delaycounternumber : 8; // [7:0]
        uint32_t __31_8 : 24;            // [31:8]
    } b;
} REG_CP_CLKRST_LTE_AUTOGATE_DELAY_NUM_T;

// clksel
#define CP_CLKRST_SLOW_SEL_122M (1 << 0)
#define CP_CLKRST_SLOW_SEL_480M (1 << 1)
#define CP_CLKRST_BB_SYSCTRL_WD_SEL (1 << 2)
#define CP_CLKRST_ZSP_WD_SEL (1 << 3)
#define CP_CLKRST_IDLE_H_SEL(n) (((n)&0x3) << 4)

// clksel_set
#define CP_CLKRST_CLKSELSET(n) (((n)&0x1f) << 0)

// clksel_clr
#define CP_CLKRST_CLKSELCLEAR(n) (((n)&0x1f) << 0)

// clkdiv_zsp
#define CP_CLKRST_ZSP_CD(n) (((n)&0xf) << 0)

// clkdiv_lte
#define CP_CLKRST_LTE_CD(n) (((n)&0xf) << 0)

// clken_bb_sysctrl
#define CP_CLKRST_CLKEN_SYSREG (1 << 0)
#define CP_CLKRST_CLKEN_PWRCTRL (1 << 1)
#define CP_CLKRST_CLKEN_MAILBOX (1 << 2)
#define CP_CLKRST_CLKEN_IDLE (1 << 3)
#define CP_CLKRST_CLKEN_CP_WD (1 << 4)

// clken_bb_sysctrl_set
#define CP_CLKRST_CLKEN_BB_SYSCTRLSET(n) (((n)&0x1f) << 0)

// clken_bb_sysctrl_clr
#define CP_CLKRST_CLKEN_BB_SYSCTRLCLEAR(n) (((n)&0x1f) << 0)

// clken_zsp
#define CP_CLKRST_CLKEN_ZSPCORE (1 << 0)
#define CP_CLKRST_CLKEN_AXIDMA (1 << 1)
#define CP_CLKRST_CLKEN_ZIRQ (1 << 2)
#define CP_CLKRST_CLKEN_BUSMON (1 << 3)
#define CP_CLKRST_CLKEN_WD (1 << 4)
#define CP_CLKRST_CLKEN_AUD_DFT (1 << 5)

// clken_zsp_set
#define CP_CLKRST_CLKEN_ZSPSET(n) (((n)&0x3f) << 0)

// clken_zsp_clr
#define CP_CLKRST_CLKEN_ZSPCLEAR(n) (((n)&0x3f) << 0)

// clken_lte
#define CP_CLKRST_CLKEN_TXRX (1 << 0)
#define CP_CLKRST_CLKEN_COEFF (1 << 1)
#define CP_CLKRST_CLKEN_LDTC (1 << 2)
#define CP_CLKRST_CLKEN_LDTC1 (1 << 3)
#define CP_CLKRST_CLKEN_MEASPWR (1 << 4)
#define CP_CLKRST_CLKEN_IDDET (1 << 5)
#define CP_CLKRST_CLKEN_OTDOA (1 << 6)
#define CP_CLKRST_CLKEN_ULDFT (1 << 7)
#define CP_CLKRST_CLKEN_PUSCH (1 << 8)
#define CP_CLKRST_CLKEN_CSIRS (1 << 9)
#define CP_CLKRST_CLKEN_DLFFT (1 << 10)
#define CP_CLKRST_CLKEN_RFAD (1 << 11)
#define CP_CLKRST_CLKEN_RXCAPT (1 << 12)

// clken_lte_set
#define CP_CLKRST_CLKEN_LTESET(n) (((n)&0x1fff) << 0)

// clken_lte_clr
#define CP_CLKRST_CLKEN_LTECLEAR(n) (((n)&0x1fff) << 0)

// clken_zspcore_mode
#define CP_CLKRST_MODE (1 << 0)

// clken_zsp_axidma_mode
#define CP_CLKRST_MODE (1 << 0)

// rstctrl_bb_sysctrl
#define CP_CLKRST_RSTCTRL_SYSREG (1 << 0)
#define CP_CLKRST_RSTCTRL_PWRCTRL (1 << 1)
#define CP_CLKRST_RSTCTRL_MAILBOX (1 << 2)
#define CP_CLKRST_RSTCTRL_IDLE (1 << 3)
#define CP_CLKRST_RSTCTRL_CP_WD (1 << 4)
#define CP_CLKRST_RSTCTRL_MONITOR (1 << 5)

// rstctrl_bb_sysctrl_set
#define CP_CLKRST_RSTCTRL_BB_SYSCTRLSET(n) (((n)&0x3f) << 0)

// rstctrl_bb_sysctrl_clr
#define CP_CLKRST_RSTCTRL_BB_SYSCTRLCLEAR(n) (((n)&0x3f) << 0)

// rstctrl_zsp
#define CP_CLKRST_RSTCTRL_ZSPCORE (1 << 0)
#define CP_CLKRST_RSTCTRL_AXIDMA (1 << 1)
#define CP_CLKRST_RSTCTRL_ZIRQ (1 << 2)
#define CP_CLKRST_RSTCTRL_BUSMON (1 << 3)
#define CP_CLKRST_RSTCTRL_WD (1 << 4)
#define CP_CLKRST_RSTCTRL_AUD_DFT (1 << 5)

// rstctrl_zsp_set
#define CP_CLKRST_RSTCTRL_ZSPSET(n) (((n)&0x3f) << 0)

// rstctrl_zsp_clr
#define CP_CLKRST_RSTCTRL_ZSPCLEAR(n) (((n)&0x3f) << 0)

// rstctrl_lte
#define CP_CLKRST_RSTCTRL_TXRX_TX (1 << 0)
#define CP_CLKRST_RSTCTRL_TXRX_RX (1 << 1)
#define CP_CLKRST_RSTCTRL_COEFF (1 << 2)
#define CP_CLKRST_RSTCTRL_LDTC (1 << 3)
#define CP_CLKRST_RSTCTRL_LDTC1 (1 << 4)
#define CP_CLKRST_RSTCTRL_MEASPWR (1 << 5)
#define CP_CLKRST_RSTCTRL_IDDET (1 << 6)
#define CP_CLKRST_RSTCTRL_OTDOA (1 << 7)
#define CP_CLKRST_RSTCTRL_ULDFT (1 << 8)
#define CP_CLKRST_RSTCTRL_PUSCH (1 << 9)
#define CP_CLKRST_RSTCTRL_CSIRS (1 << 10)
#define CP_CLKRST_RSTCTRL_DLFFT (1 << 11)
#define CP_CLKRST_RSTCTRL_RFAD (1 << 12)
#define CP_CLKRST_RSTCTRL_RXCAPT (1 << 13)

// rstctrl_lte_set
#define CP_CLKRST_RSTCTRL_LTESET(n) (((n)&0x3fff) << 0)

// rstctrl_lte_clr
#define CP_CLKRST_RSTCTRL_LTECLEAR(n) (((n)&0x3fff) << 0)

// zsp_soft_rst
#define CP_CLKRST_ZSP_SOFT_RST_CTRL (1 << 0)

// lte_soft_rst
#define CP_CLKRST_LTE_SOFT_RST_CTRL (1 << 0)

// zsp_axilpcnt
#define CP_CLKRST_CNT(n) (((n)&0xffff) << 0)

// zsp_buslpmc
#define CP_CLKRST_BUSLPMC_SW1 (1 << 0)
#define CP_CLKRST_BUSLPMC_SW2 (1 << 1)
#define CP_CLKRST_BUSLPMC_PHY (1 << 2)
#define CP_CLKRST_BUSLPMC_ZSPCORE (1 << 3)

// zsp_buslpmc_set
#define CP_CLKRST_ZSP_BUSLPMCSET(n) (((n)&0xf) << 0)

// zsp_buslpmc_clr
#define CP_CLKRST_ZSP_BUSLPMCCLEAR(n) (((n)&0xf) << 0)

// zsp_busforcelpmc
#define CP_CLKRST_BUSFORCELPMC_SW1 (1 << 0)
#define CP_CLKRST_BUSFORCELPMC_SW2 (1 << 1)
#define CP_CLKRST_BUSFORCELPMC_PHY (1 << 2)
#define CP_CLKRST_BUSFORCELPMC_ZSPCORE (1 << 3)

// zsp_busforcelpmc_set
#define CP_CLKRST_ZSP_BUSFORCELPMCSET(n) (((n)&0xf) << 0)

// zsp_busforcelpmc_clr
#define CP_CLKRST_ZSP_BUSFORCELPMCCLEAR(n) (((n)&0xf) << 0)

// clken_mailbox_mode
#define CP_CLKRST_MODE (1 << 0)

// clken_lte_intf
#define CP_CLKRST_CLKEN_TXRX_INTF (1 << 0)
#define CP_CLKRST_CLKEN_COEFF_INTF (1 << 1)
#define CP_CLKRST_CLKEN_LDTC_INTF (1 << 2)
#define CP_CLKRST_CLKEN_LDTC1_INTF (1 << 3)
#define CP_CLKRST_CLKEN_MEASPWR_INTF (1 << 4)
#define CP_CLKRST_CLKEN_IDDET_INTF (1 << 5)
#define CP_CLKRST_CLKEN_OTDOA_INTF (1 << 6)
#define CP_CLKRST_CLKEN_ULDFT_INTF (1 << 7)
#define CP_CLKRST_CLKEN_PUSCH_INTF (1 << 8)
#define CP_CLKRST_CLKEN_CSIRS_INTF (1 << 9)
#define CP_CLKRST_CLKEN_DLFFT_INTF (1 << 10)
#define CP_CLKRST_CLKEN_RFAD_INTF (1 << 11)
#define CP_CLKRST_CLKEN_RXCAPT_INTF (1 << 12)

// clken_lte_intf_set
#define CP_CLKRST_CLKEN_LTE_INTF_SET_CLKEN_LTE_INTFSET(n) (((n)&0x1fff) << 0)

// clken_lte_intf_clr
#define CP_CLKRST_CLKEN_LTE_INTF_CLR_CLKEN_LTE_INTFCLEAR(n) (((n)&0x1fff) << 0)

// lte_autogate_mode
#define CP_CLKRST_MODE (1 << 0)

// lte_autogate_en
#define CP_CLKRST_TXRX_FUNC_AUTOGATE_EN (1 << 0)
#define CP_CLKRST_COEFF_FUNC_AUTOGATE_EN (1 << 1)
#define CP_CLKRST_LDTC_FUNC_AUTOGATE_EN (1 << 2)
#define CP_CLKRST_LDTC1_FUNC_AUTOGATE_EN (1 << 3)
#define CP_CLKRST_MEASPWR_FUNC_AUTOGATE_EN (1 << 4)
#define CP_CLKRST_IDDET_FUNC_AUTOGATE_EN (1 << 5)
#define CP_CLKRST_OTDOA_FUNC_AUTOGATE_EN (1 << 6)
#define CP_CLKRST_ULDFT_FUNC_AUTOGATE_EN (1 << 7)
#define CP_CLKRST_PUSCH_FUNC_AUTOGATE_EN (1 << 8)
#define CP_CLKRST_CSIRS_FUNC_AUTOGATE_EN (1 << 9)
#define CP_CLKRST_DLFFT_FUNC_AUTOGATE_EN (1 << 10)
#define CP_CLKRST_TXRX_INTF_AUTOGATE_EN (1 << 11)
#define CP_CLKRST_COEFF_INTF_AUTOGATE_EN (1 << 12)
#define CP_CLKRST_LDTC_INTF_AUTOGATE_EN (1 << 13)
#define CP_CLKRST_LDTC1_INTF_AUTOGATE_EN (1 << 14)
#define CP_CLKRST_MEASPWR_INTF_AUTOGATE_EN (1 << 15)
#define CP_CLKRST_IDDET_INTF_AUTOGATE_EN (1 << 16)
#define CP_CLKRST_OTDOA_INTF_AUTOGATE_EN (1 << 17)
#define CP_CLKRST_ULDFT_INTF_AUTOGATE_EN (1 << 18)
#define CP_CLKRST_PUSCH_INTF_AUTOGATE_EN (1 << 19)
#define CP_CLKRST_CSIRS_INTF_AUTOGATE_EN (1 << 20)
#define CP_CLKRST_DLFFT_INTF_AUTOGATE_EN (1 << 21)
#define CP_CLKRST_DOWNLINK_FUNC_AUTOGATE_EN (1 << 24)
#define CP_CLKRST_UPLINK_FUNC_AUTOGATE_EN (1 << 25)
#define CP_CLKRST_DOWNLINK_INTF_AUTOGATE_EN (1 << 26)
#define CP_CLKRST_UPLINK_INTF_AUTOGATE_EN (1 << 27)

// lte_autogate_en_set
#define CP_CLKRST_LTE_AUTOGATE_EN_SET_CLKEN_LTE_INTFSET(n) (((n)&0xfffffff) << 0)

// lte_autogate_en_clr
#define CP_CLKRST_LTE_AUTOGATE_EN_CLR_CLKEN_LTE_INTFCLEAR(n) (((n)&0xfffffff) << 0)

// lte_autogate_delay_num
#define CP_CLKRST_DELAYCOUNTERNUMBER(n) (((n)&0xff) << 0)

#endif // _CP_CLKRST_H_
