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

#ifndef _CP_PWRCTRL_H_
#define _CP_PWRCTRL_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_PWRCTRL_BASE (0x050a0000)
#else
#define REG_PWRCTRL_BASE (0x500a0000)
#endif

typedef volatile struct
{
    uint32_t pwr_hwen;         // 0x00000000
    uint32_t ap_pwr_ctrl;      // 0x00000004
    uint32_t zsp_pwr_ctrl;     // 0x00000008
    uint32_t lte_pwr_ctrl;     // 0x0000000c
    uint32_t gge_pwr_ctrl;     // 0x00000010
    uint32_t rf_pwr_ctrl;      // 0x00000014
    uint32_t btfm_pwr_ctrl;    // 0x00000018
    uint32_t aon_lp_pwr_ctrl;  // 0x0000001c
    uint32_t ap_pwr_stat;      // 0x00000020
    uint32_t zsp_pwr_stat;     // 0x00000024
    uint32_t lte_pwr_stat;     // 0x00000028
    uint32_t gge_pwr_stat;     // 0x0000002c
    uint32_t rf_pwr_stat;      // 0x00000030
    uint32_t btfm_pwr_stat;    // 0x00000034
    uint32_t aon_lp_pwr_stat;  // 0x00000038
    uint32_t state_delay;      // 0x0000003c
    uint32_t prepgc_delay;     // 0x00000040
    uint32_t allpgc_delay;     // 0x00000044
    uint32_t ddr_hold_ctrl;    // 0x00000048
    uint32_t zsp_pd_poll;      // 0x0000004c
    uint32_t zsp_pd_poll_set;  // 0x00000050
    uint32_t zsp_pd_poll_clr;  // 0x00000054
    uint32_t __88[14];         // 0x00000058
    uint32_t wcn_lps;          // 0x00000090
    uint32_t arm_slp_req_sw;   // 0x00000094
    uint32_t arm_slp_ack;      // 0x00000098
    uint32_t arm_slp_req_hwen; // 0x0000009c
    uint32_t zsp_slp_req_sw;   // 0x000000a0
    uint32_t zsp_slp_ack;      // 0x000000a4
    uint32_t zsp_slp_req_hwen; // 0x000000a8
    uint32_t ddr_slp_req_sw;   // 0x000000ac
    uint32_t ddr_slp_ack;      // 0x000000b0
    uint32_t ddr_slp_req_hwen; // 0x000000b4
    uint32_t timeout_flag;     // 0x000000b8
    uint32_t power_state;      // 0x000000bc
    uint32_t pwrctrl_mode;     // 0x000000c0
    uint32_t pwrctrl_sw;       // 0x000000c4
    uint32_t pwrctrl_sw_set;   // 0x000000c8
    uint32_t pwrctrl_sw_clr;   // 0x000000cc
    uint32_t pwrctrl_sw1;      // 0x000000d0
    uint32_t pwrctrl_sw1_set;  // 0x000000d4
    uint32_t pwrctrl_sw1_clr;  // 0x000000d8
} HWP_CP_PWRCTRL_T;

#define hwp_pwrctrl ((HWP_CP_PWRCTRL_T *)REG_ACCESS_ADDRESS(REG_PWRCTRL_BASE))

// pwr_hwen
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_pwr_en : 1;     // [0]
        uint32_t zsp_pon_en : 1;    // [1]
        uint32_t lte_pon_en : 1;    // [2]
        uint32_t gge_pon_en : 1;    // [3]
        uint32_t rf_pon_en : 1;     // [4]
        uint32_t btfm_pon_en : 1;   // [5]
        uint32_t aon_lp_pon_en : 1; // [6]
        uint32_t __31_7 : 25;       // [31:7]
    } b;
} REG_CP_PWRCTRL_PWR_HWEN_T;

// ap_pwr_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_poff : 1; // [0]
        uint32_t ap_pon : 1;  // [1]
        uint32_t __31_2 : 30; // [31:2]
    } b;
} REG_CP_PWRCTRL_AP_PWR_CTRL_T;

// zsp_pwr_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsp_poff : 1; // [0]
        uint32_t zsp_pon : 1;  // [1]
        uint32_t __31_2 : 30;  // [31:2]
    } b;
} REG_CP_PWRCTRL_ZSP_PWR_CTRL_T;

// lte_pwr_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lte_poff : 1; // [0]
        uint32_t lte_pon : 1;  // [1]
        uint32_t __31_2 : 30;  // [31:2]
    } b;
} REG_CP_PWRCTRL_LTE_PWR_CTRL_T;

// gge_pwr_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gge_poff : 1; // [0]
        uint32_t gge_pon : 1;  // [1]
        uint32_t __31_2 : 30;  // [31:2]
    } b;
} REG_CP_PWRCTRL_GGE_PWR_CTRL_T;

// rf_pwr_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_poff : 1; // [0]
        uint32_t rf_pon : 1;  // [1]
        uint32_t __31_2 : 30; // [31:2]
    } b;
} REG_CP_PWRCTRL_RF_PWR_CTRL_T;

// btfm_pwr_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t btfm_poff : 1; // [0]
        uint32_t btfm_pon : 1;  // [1]
        uint32_t __31_2 : 30;   // [31:2]
    } b;
} REG_CP_PWRCTRL_BTFM_PWR_CTRL_T;

// aon_lp_pwr_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aon_lp_poff : 1; // [0]
        uint32_t aon_lp_pon : 1;  // [1]
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_CP_PWRCTRL_AON_LP_PWR_CTRL_T;

// ap_pwr_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_pstat : 1;  // [0], read only
        uint32_t ap_stable : 1; // [1], read only
        uint32_t __31_2 : 30;   // [31:2]
    } b;
} REG_CP_PWRCTRL_AP_PWR_STAT_T;

// zsp_pwr_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsp_pstat : 1;  // [0], read only
        uint32_t zsp_stable : 1; // [1], read only
        uint32_t __31_2 : 30;    // [31:2]
    } b;
} REG_CP_PWRCTRL_ZSP_PWR_STAT_T;

// lte_pwr_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lte_pstat : 1;  // [0], read only
        uint32_t lte_stable : 1; // [1], read only
        uint32_t __31_2 : 30;    // [31:2]
    } b;
} REG_CP_PWRCTRL_LTE_PWR_STAT_T;

// gge_pwr_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gge_pstat : 1;  // [0], read only
        uint32_t gge_stable : 1; // [1], read only
        uint32_t __31_2 : 30;    // [31:2]
    } b;
} REG_CP_PWRCTRL_GGE_PWR_STAT_T;

// rf_pwr_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_pstat : 1;  // [0], read only
        uint32_t rf_stable : 1; // [1], read only
        uint32_t __31_2 : 30;   // [31:2]
    } b;
} REG_CP_PWRCTRL_RF_PWR_STAT_T;

// btfm_pwr_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t btfm_pstat : 1;  // [0], read only
        uint32_t btfm_stable : 1; // [1], read only
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_CP_PWRCTRL_BTFM_PWR_STAT_T;

// aon_lp_pwr_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aon_lp_pstat : 1;  // [0], read only
        uint32_t aon_lp_stable : 1; // [1], read only
        uint32_t __31_2 : 30;       // [31:2]
    } b;
} REG_CP_PWRCTRL_AON_LP_PWR_STAT_T;

// state_delay
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay : 8;   // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_CP_PWRCTRL_STATE_DELAY_T;

// prepgc_delay
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay : 16;   // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_CP_PWRCTRL_PREPGC_DELAY_T;

// allpgc_delay
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay : 16;   // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_CP_PWRCTRL_ALLPGC_DELAY_T;

// ddr_hold_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hold_ctrl : 1; // [0]
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CP_PWRCTRL_DDR_HOLD_CTRL_T;

// zsp_pd_poll
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_poll : 1;  // [0]
        uint32_t cp_poll : 1;  // [1]
        uint32_t zsp_poll : 1; // [2]
        uint32_t __31_3 : 29;  // [31:3]
    } b;
} REG_CP_PWRCTRL_ZSP_PD_POLL_T;

// zsp_pd_poll_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsppollset : 3; // [2:0], write set
        uint32_t __31_3 : 29;    // [31:3]
    } b;
} REG_CP_PWRCTRL_ZSP_PD_POLL_SET_T;

// zsp_pd_poll_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zsppollclr : 3; // [2:0], write clear
        uint32_t __31_3 : 29;    // [31:3]
    } b;
} REG_CP_PWRCTRL_ZSP_PD_POLL_CLR_T;

// wcn_lps
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sys2wcn_wakeup : 1; // [0]
        uint32_t wcn2sys_wakeup : 1; // [1], read only
        uint32_t wcn2sys_osc_en : 1; // [2], read only
        uint32_t wcn2sys_sleep : 1;  // [3], read only
        uint32_t __31_4 : 28;        // [31:4]
    } b;
} REG_CP_PWRCTRL_WCN_LPS_T;

// arm_slp_req_sw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t armslpreq : 1; // [0]
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CP_PWRCTRL_ARM_SLP_REQ_SW_T;

// arm_slp_ack
typedef union {
    uint32_t v;
    struct
    {
        uint32_t armslpack : 1; // [0], read only
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CP_PWRCTRL_ARM_SLP_ACK_T;

// arm_slp_req_hwen
typedef union {
    uint32_t v;
    struct
    {
        uint32_t armslpreq_hwen : 1; // [0]
        uint32_t __31_1 : 31;        // [31:1]
    } b;
} REG_CP_PWRCTRL_ARM_SLP_REQ_HWEN_T;

// zsp_slp_req_sw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zspslpreq : 1; // [0]
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CP_PWRCTRL_ZSP_SLP_REQ_SW_T;

// zsp_slp_ack
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zspslpack : 1; // [0], read only
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CP_PWRCTRL_ZSP_SLP_ACK_T;

// zsp_slp_req_hwen
typedef union {
    uint32_t v;
    struct
    {
        uint32_t zspslpreq_hwen : 1; // [0]
        uint32_t __31_1 : 31;        // [31:1]
    } b;
} REG_CP_PWRCTRL_ZSP_SLP_REQ_HWEN_T;

// ddr_slp_req_sw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddrslpreq : 1; // [0]
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CP_PWRCTRL_DDR_SLP_REQ_SW_T;

// ddr_slp_ack
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddrslpack : 1; // [0], read only
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CP_PWRCTRL_DDR_SLP_ACK_T;

// ddr_slp_req_hwen
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ddrslpreq_hwen : 1; // [0]
        uint32_t __31_1 : 31;        // [31:1]
    } b;
} REG_CP_PWRCTRL_DDR_SLP_REQ_HWEN_T;

// timeout_flag
typedef union {
    uint32_t v;
    struct
    {
        uint32_t armbusslptimeout : 1; // [0]
        uint32_t zspbusslptimeout : 1; // [1]
        uint32_t ddrslptimeout : 1;    // [2]
        uint32_t __31_3 : 29;          // [31:3]
    } b;
} REG_CP_PWRCTRL_TIMEOUT_FLAG_T;

// power_state
typedef union {
    uint32_t v;
    struct
    {
        uint32_t armpowerstate : 4;   // [3:0], read only
        uint32_t zsppowerstate : 4;   // [7:4], read only
        uint32_t ltepowerstate : 4;   // [11:8], read only
        uint32_t ggepowerstate : 4;   // [15:12], read only
        uint32_t rfpowerstate : 4;    // [19:16], read only
        uint32_t btfmpowerstate : 4;  // [23:20], read only
        uint32_t aonlppowerstate : 4; // [27:24], read only
        uint32_t corepowerstate : 4;  // [31:28], read only
    } b;
} REG_CP_PWRCTRL_POWER_STATE_T;

// pwrctrl_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pwr_mode : 1; // [0]
        uint32_t __31_1 : 31;  // [31:1]
    } b;
} REG_CP_PWRCTRL_PWRCTRL_MODE_T;

// pwrctrl_sw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_clk_ctrl : 1;       // [0]
        uint32_t ap_rst_ctrl : 1;       // [1]
        uint32_t ap_hold : 1;           // [2]
        uint32_t ap_pwr_ctrl_pre : 1;   // [3]
        uint32_t ap_pwr_ctrl : 1;       // [4]
        uint32_t zsp_clk_ctrl : 1;      // [5]
        uint32_t zsp_rst_ctrl : 1;      // [6]
        uint32_t zsp_hold : 1;          // [7]
        uint32_t zsp_pwr_ctrl_pre : 1;  // [8]
        uint32_t zsp_pwr_ctrl : 1;      // [9]
        uint32_t lte_clk_ctrl : 1;      // [10]
        uint32_t lte_rst_ctrl : 1;      // [11]
        uint32_t lte_hold : 1;          // [12]
        uint32_t lte_pwr_ctrl_pre : 1;  // [13]
        uint32_t lte_pwr_ctrl : 1;      // [14]
        uint32_t gge_clk_ctrl : 1;      // [15]
        uint32_t gge_rst_ctrl : 1;      // [16]
        uint32_t gge_hold : 1;          // [17]
        uint32_t gge_pwr_ctrl_pre : 1;  // [18]
        uint32_t gge_pwr_ctrl : 1;      // [19]
        uint32_t rf_clk_ctrl : 1;       // [20]
        uint32_t rf_rst_ctrl : 1;       // [21]
        uint32_t rf_hold : 1;           // [22]
        uint32_t rf_pwr_ctrl_pre : 1;   // [23]
        uint32_t rf_pwr_ctrl : 1;       // [24]
        uint32_t btfm_clk_ctrl : 1;     // [25]
        uint32_t btfm_rst_ctrl : 1;     // [26]
        uint32_t btfm_hold : 1;         // [27]
        uint32_t btfm_pwr_ctrl_pre : 1; // [28]
        uint32_t btfm_pwr_ctrl : 1;     // [29]
        uint32_t __31_30 : 2;           // [31:30]
    } b;
} REG_CP_PWRCTRL_PWRCTRL_SW_T;

// pwrctrl_sw_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set : 30;    // [29:0], write set
        uint32_t __31_30 : 2; // [31:30]
    } b;
} REG_CP_PWRCTRL_PWRCTRL_SW_SET_T;

// pwrctrl_sw_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clear : 30;  // [29:0], write clear
        uint32_t __31_30 : 2; // [31:30]
    } b;
} REG_CP_PWRCTRL_PWRCTRL_SW_CLR_T;

// pwrctrl_sw1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aon_lp_clk_ctrl : 1;     // [0]
        uint32_t aon_lp_rst_ctrl : 1;     // [1]
        uint32_t aon_lp_hold : 1;         // [2]
        uint32_t aon_lp_pwr_ctrl_pre : 1; // [3]
        uint32_t aon_lp_pwr_ctrl : 1;     // [4]
        uint32_t __31_5 : 27;             // [31:5]
    } b;
} REG_CP_PWRCTRL_PWRCTRL_SW1_T;

// pwrctrl_sw1_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set : 5;     // [4:0], write set
        uint32_t __31_5 : 27; // [31:5]
    } b;
} REG_CP_PWRCTRL_PWRCTRL_SW1_SET_T;

// pwrctrl_sw1_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clear : 5;   // [4:0], write clear
        uint32_t __31_5 : 27; // [31:5]
    } b;
} REG_CP_PWRCTRL_PWRCTRL_SW1_CLR_T;

// pwr_hwen
#define CP_PWRCTRL_AP_PWR_EN (1 << 0)
#define CP_PWRCTRL_ZSP_PON_EN (1 << 1)
#define CP_PWRCTRL_LTE_PON_EN (1 << 2)
#define CP_PWRCTRL_GGE_PON_EN (1 << 3)
#define CP_PWRCTRL_RF_PON_EN (1 << 4)
#define CP_PWRCTRL_BTFM_PON_EN (1 << 5)
#define CP_PWRCTRL_AON_LP_PON_EN (1 << 6)

// ap_pwr_ctrl
#define CP_PWRCTRL_AP_POFF (1 << 0)
#define CP_PWRCTRL_AP_PON (1 << 1)

// zsp_pwr_ctrl
#define CP_PWRCTRL_ZSP_POFF (1 << 0)
#define CP_PWRCTRL_ZSP_PON (1 << 1)

// lte_pwr_ctrl
#define CP_PWRCTRL_LTE_POFF (1 << 0)
#define CP_PWRCTRL_LTE_PON (1 << 1)

// gge_pwr_ctrl
#define CP_PWRCTRL_GGE_POFF (1 << 0)
#define CP_PWRCTRL_GGE_PON (1 << 1)

// rf_pwr_ctrl
#define CP_PWRCTRL_RF_POFF (1 << 0)
#define CP_PWRCTRL_RF_PON (1 << 1)

// btfm_pwr_ctrl
#define CP_PWRCTRL_BTFM_POFF (1 << 0)
#define CP_PWRCTRL_BTFM_PON (1 << 1)

// aon_lp_pwr_ctrl
#define CP_PWRCTRL_AON_LP_POFF (1 << 0)
#define CP_PWRCTRL_AON_LP_PON (1 << 1)

// ap_pwr_stat
#define CP_PWRCTRL_AP_PSTAT (1 << 0)
#define CP_PWRCTRL_AP_STABLE (1 << 1)

// zsp_pwr_stat
#define CP_PWRCTRL_ZSP_PSTAT (1 << 0)
#define CP_PWRCTRL_ZSP_STABLE (1 << 1)

// lte_pwr_stat
#define CP_PWRCTRL_LTE_PSTAT (1 << 0)
#define CP_PWRCTRL_LTE_STABLE (1 << 1)

// gge_pwr_stat
#define CP_PWRCTRL_GGE_PSTAT (1 << 0)
#define CP_PWRCTRL_GGE_STABLE (1 << 1)

// rf_pwr_stat
#define CP_PWRCTRL_RF_PSTAT (1 << 0)
#define CP_PWRCTRL_RF_STABLE (1 << 1)

// btfm_pwr_stat
#define CP_PWRCTRL_BTFM_PSTAT (1 << 0)
#define CP_PWRCTRL_BTFM_STABLE (1 << 1)

// aon_lp_pwr_stat
#define CP_PWRCTRL_AON_LP_PSTAT (1 << 0)
#define CP_PWRCTRL_AON_LP_STABLE (1 << 1)

// state_delay
#define CP_PWRCTRL_STATE_DELAY_DELAY(n) (((n)&0xff) << 0)

// prepgc_delay
#define CP_PWRCTRL_PREPGC_DELAY_DELAY(n) (((n)&0xffff) << 0)

// allpgc_delay
#define CP_PWRCTRL_ALLPGC_DELAY_DELAY(n) (((n)&0xffff) << 0)

// ddr_hold_ctrl
#define CP_PWRCTRL_HOLD_CTRL (1 << 0)

// zsp_pd_poll
#define CP_PWRCTRL_AP_POLL (1 << 0)
#define CP_PWRCTRL_CP_POLL (1 << 1)
#define CP_PWRCTRL_ZSP_POLL (1 << 2)

// zsp_pd_poll_set
#define CP_PWRCTRL_ZSPPOLLSET(n) (((n)&0x7) << 0)

// zsp_pd_poll_clr
#define CP_PWRCTRL_ZSPPOLLCLR(n) (((n)&0x7) << 0)

// wcn_lps
#define CP_PWRCTRL_SYS2WCN_WAKEUP (1 << 0)
#define CP_PWRCTRL_WCN2SYS_WAKEUP (1 << 1)
#define CP_PWRCTRL_WCN2SYS_OSC_EN (1 << 2)
#define CP_PWRCTRL_WCN2SYS_SLEEP (1 << 3)

// arm_slp_req_sw
#define CP_PWRCTRL_ARMSLPREQ (1 << 0)

// arm_slp_ack
#define CP_PWRCTRL_ARMSLPACK (1 << 0)

// arm_slp_req_hwen
#define CP_PWRCTRL_ARMSLPREQ_HWEN (1 << 0)

// zsp_slp_req_sw
#define CP_PWRCTRL_ZSPSLPREQ (1 << 0)

// zsp_slp_ack
#define CP_PWRCTRL_ZSPSLPACK (1 << 0)

// zsp_slp_req_hwen
#define CP_PWRCTRL_ZSPSLPREQ_HWEN (1 << 0)

// ddr_slp_req_sw
#define CP_PWRCTRL_DDRSLPREQ (1 << 0)

// ddr_slp_ack
#define CP_PWRCTRL_DDRSLPACK (1 << 0)

// ddr_slp_req_hwen
#define CP_PWRCTRL_DDRSLPREQ_HWEN (1 << 0)

// timeout_flag
#define CP_PWRCTRL_ARMBUSSLPTIMEOUT (1 << 0)
#define CP_PWRCTRL_ZSPBUSSLPTIMEOUT (1 << 1)
#define CP_PWRCTRL_DDRSLPTIMEOUT (1 << 2)

// power_state
#define CP_PWRCTRL_ARMPOWERSTATE(n) (((n)&0xf) << 0)
#define CP_PWRCTRL_ZSPPOWERSTATE(n) (((n)&0xf) << 4)
#define CP_PWRCTRL_LTEPOWERSTATE(n) (((n)&0xf) << 8)
#define CP_PWRCTRL_GGEPOWERSTATE(n) (((n)&0xf) << 12)
#define CP_PWRCTRL_RFPOWERSTATE(n) (((n)&0xf) << 16)
#define CP_PWRCTRL_BTFMPOWERSTATE(n) (((n)&0xf) << 20)
#define CP_PWRCTRL_AONLPPOWERSTATE(n) (((n)&0xf) << 24)
#define CP_PWRCTRL_COREPOWERSTATE(n) (((n)&0xf) << 28)

// pwrctrl_mode
#define CP_PWRCTRL_PWR_MODE (1 << 0)

// pwrctrl_sw
#define CP_PWRCTRL_AP_CLK_CTRL (1 << 0)
#define CP_PWRCTRL_AP_RST_CTRL (1 << 1)
#define CP_PWRCTRL_AP_HOLD (1 << 2)
#define CP_PWRCTRL_AP_PWR_CTRL_PRE (1 << 3)
#define CP_PWRCTRL_AP_PWR_CTRL (1 << 4)
#define CP_PWRCTRL_ZSP_CLK_CTRL (1 << 5)
#define CP_PWRCTRL_ZSP_RST_CTRL (1 << 6)
#define CP_PWRCTRL_ZSP_HOLD (1 << 7)
#define CP_PWRCTRL_ZSP_PWR_CTRL_PRE (1 << 8)
#define CP_PWRCTRL_ZSP_PWR_CTRL (1 << 9)
#define CP_PWRCTRL_LTE_CLK_CTRL (1 << 10)
#define CP_PWRCTRL_LTE_RST_CTRL (1 << 11)
#define CP_PWRCTRL_LTE_HOLD (1 << 12)
#define CP_PWRCTRL_LTE_PWR_CTRL_PRE (1 << 13)
#define CP_PWRCTRL_LTE_PWR_CTRL (1 << 14)
#define CP_PWRCTRL_GGE_CLK_CTRL (1 << 15)
#define CP_PWRCTRL_GGE_RST_CTRL (1 << 16)
#define CP_PWRCTRL_GGE_HOLD (1 << 17)
#define CP_PWRCTRL_GGE_PWR_CTRL_PRE (1 << 18)
#define CP_PWRCTRL_GGE_PWR_CTRL (1 << 19)
#define CP_PWRCTRL_RF_CLK_CTRL (1 << 20)
#define CP_PWRCTRL_RF_RST_CTRL (1 << 21)
#define CP_PWRCTRL_RF_HOLD (1 << 22)
#define CP_PWRCTRL_RF_PWR_CTRL_PRE (1 << 23)
#define CP_PWRCTRL_RF_PWR_CTRL (1 << 24)
#define CP_PWRCTRL_BTFM_CLK_CTRL (1 << 25)
#define CP_PWRCTRL_BTFM_RST_CTRL (1 << 26)
#define CP_PWRCTRL_BTFM_HOLD (1 << 27)
#define CP_PWRCTRL_BTFM_PWR_CTRL_PRE (1 << 28)
#define CP_PWRCTRL_BTFM_PWR_CTRL (1 << 29)

// pwrctrl_sw_set
#define CP_PWRCTRL_PWRCTRL_SW_SET_SET(n) (((n)&0x3fffffff) << 0)

// pwrctrl_sw_clr
#define CP_PWRCTRL_PWRCTRL_SW_CLR_CLEAR(n) (((n)&0x3fffffff) << 0)

// pwrctrl_sw1
#define CP_PWRCTRL_AON_LP_CLK_CTRL (1 << 0)
#define CP_PWRCTRL_AON_LP_RST_CTRL (1 << 1)
#define CP_PWRCTRL_AON_LP_HOLD (1 << 2)
#define CP_PWRCTRL_AON_LP_PWR_CTRL_PRE (1 << 3)
#define CP_PWRCTRL_AON_LP_PWR_CTRL (1 << 4)

// pwrctrl_sw1_set
#define CP_PWRCTRL_PWRCTRL_SW1_SET_SET(n) (((n)&0x1f) << 0)

// pwrctrl_sw1_clr
#define CP_PWRCTRL_PWRCTRL_SW1_CLR_CLEAR(n) (((n)&0x1f) << 0)

#endif // _CP_PWRCTRL_H_
