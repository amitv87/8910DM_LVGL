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

#ifndef _RDA2720M_PSM_H_
#define _RDA2720M_PSM_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_PSM_BASE (0x50308900)

typedef volatile struct
{
    uint32_t psm_reg_wr_protect;     // 0x00000000
    uint32_t psm_32k_cal_th;         // 0x00000004
    uint32_t psm_26m_cal_dn_th;      // 0x00000008
    uint32_t psm_26m_cal_up_th;      // 0x0000000c
    uint32_t psm_ctrl;               // 0x00000010
    uint32_t rtc_pwr_off_th1;        // 0x00000014
    uint32_t rtc_pwr_off_th2;        // 0x00000018
    uint32_t rtc_pwr_off_th3;        // 0x0000001c
    uint32_t rtc_pwr_on_th1;         // 0x00000020
    uint32_t rtc_pwr_on_th2;         // 0x00000024
    uint32_t rtc_pwr_on_th3;         // 0x00000028
    uint32_t psm_cnt_l_th;           // 0x0000002c
    uint32_t psm_cnt_h_th;           // 0x00000030
    uint32_t psm_alarm_cnt_l_th;     // 0x00000034
    uint32_t psm_alarm_cnt_h_th;     // 0x00000038
    uint32_t psm_cnt_interval_th;    // 0x0000003c
    uint32_t psm_cnt_interval_phase; // 0x00000040
    uint32_t dcxo;                   // 0x00000044
    uint32_t psm_rc_clk_div;         // 0x00000048
    uint32_t reserved_2;             // 0x0000004c
    uint32_t reserved_3;             // 0x00000050
    uint32_t reserved_4;             // 0x00000054
    uint32_t reserved_5;             // 0x00000058
    uint32_t reserved_6;             // 0x0000005c
    uint32_t psm_cnt_update_l_value; // 0x00000060
    uint32_t psm_cnt_update_h_value; // 0x00000064
    uint32_t psm_status;             // 0x00000068
    uint32_t psm_fsm_status;         // 0x0000006c
    uint32_t psm_cal_cnt;            // 0x00000070
} HWP_RDA2720M_PSM_T;

#define hwp_rda2720mPsm ((HWP_RDA2720M_PSM_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_PSM_BASE))

// psm_reg_wr_protect
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_reg_wr : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_REG_WR_PROTECT_T;

// psm_32k_cal_th
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rc_32k_cal_cnt_n : 4;  // [3:0]
        uint32_t __7_4 : 4;             // [7:4]
        uint32_t rc_32k_cal_pre_th : 4; // [11:8]
        uint32_t __31_12 : 20;          // [31:12]
    } b;
} REG_RDA2720M_PSM_PSM_32K_CAL_TH_T;

// psm_26m_cal_dn_th
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rc_26m_cal_cnt_dn_th : 16; // [15:0]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_26M_CAL_DN_TH_T;

// psm_26m_cal_up_th
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rc_26m_cal_cnt_up_th : 16; // [15:0]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_26M_CAL_UP_TH_T;

// psm_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_en : 1;                // [0]
        uint32_t rtc_pwr_on_timeout_en : 1; // [1]
        uint32_t ext_int_pwr_en : 1;        // [2]
        uint32_t pbint1_pwr_en : 1;         // [3]
        uint32_t pbint2_pwr_en : 1;         // [4]
        uint32_t charger_pwr_en : 1;        // [5]
        uint32_t psm_cnt_alarm_en : 1;      // [6]
        uint32_t psm_cnt_alm_en : 1;        // [7]
        uint32_t psm_software_reset : 1;    // [8]
        uint32_t psm_cnt_update : 1;        // [9]
        uint32_t psm_cnt_en : 1;            // [10]
        uint32_t psm_status_clr : 1;        // [11]
        uint32_t psm_cal_en : 1;            // [12]
        uint32_t __14_13 : 2;               // [14:13]
        uint32_t rtc_32k_clk_sel : 1;       // [15]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_CTRL_T;

// rtc_pwr_off_th1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_pwr_off_clk_en_th : 8; // [7:0]
        uint32_t rtc_pwr_off_hold_th : 8;   // [15:8]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_PSM_RTC_PWR_OFF_TH1_T;

// rtc_pwr_off_th2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_pwr_off_rstn_th : 8; // [7:0]
        uint32_t rtc_pwr_off_pd_th : 8;   // [15:8]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_RDA2720M_PSM_RTC_PWR_OFF_TH2_T;

// rtc_pwr_off_th3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_pwr_off_done_th : 8; // [7:0]
        uint32_t __31_8 : 24;             // [31:8]
    } b;
} REG_RDA2720M_PSM_RTC_PWR_OFF_TH3_T;

// rtc_pwr_on_th1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_pwr_on_pd_th : 8;   // [7:0]
        uint32_t rtc_pwr_on_rstn_th : 8; // [15:8]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_PSM_RTC_PWR_ON_TH1_T;

// rtc_pwr_on_th2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_pwr_on_hold_th : 8;   // [7:0]
        uint32_t rtc_pwr_on_clk_en_th : 8; // [15:8]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_RDA2720M_PSM_RTC_PWR_ON_TH2_T;

// rtc_pwr_on_th3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_pwr_on_done_th : 8;    // [7:0]
        uint32_t rtc_pwr_on_timeout_th : 8; // [15:8]
        uint32_t __31_16 : 16;              // [31:16]
    } b;
} REG_RDA2720M_PSM_RTC_PWR_ON_TH3_T;

// psm_cnt_l_th
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_cnt_th_15_0_ : 16; // [15:0]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_CNT_L_TH_T;

// psm_cnt_h_th
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_cnt_th_31_16_ : 16; // [15:0]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_CNT_H_TH_T;

// psm_alarm_cnt_l_th
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_alarm_cnt_th_15_0_ : 16; // [15:0]
        uint32_t __31_16 : 16;                // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_ALARM_CNT_L_TH_T;

// psm_alarm_cnt_h_th
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_alarm_cnt_th_31_16_ : 16; // [15:0]
        uint32_t __31_16 : 16;                 // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_ALARM_CNT_H_TH_T;

// psm_cnt_interval_th
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_cnt_interval_th_15_0_ : 16; // [15:0]
        uint32_t __31_16 : 16;                   // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_CNT_INTERVAL_TH_T;

// psm_cnt_interval_phase
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_cnt_interval_phase_15_0_ : 16; // [15:0]
        uint32_t __31_16 : 16;                      // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_CNT_INTERVAL_PHASE_T;

// dcxo
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_dcxo_v : 7;        // [6:0]
        uint32_t __7_7 : 1;             // [7]
        uint32_t ldo_dcxo_cl_adj : 1;   // [8]
        uint32_t ldo_dcxo_shpt_adj : 1; // [9]
        uint32_t ldo_dcxo_stb : 2;      // [11:10]
        uint32_t ldo_dcxo_cap_sel : 1;  // [12]
        uint32_t ldo_dcxo_shpt_pd : 1;  // [13]
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_RDA2720M_PSM_DCXO_T;

// psm_rc_clk_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdg_rst_clk_sel_en : 1; // [0]
        uint32_t __3_1 : 3;              // [3:1]
        uint32_t clk_cal_64k_div_th : 3; // [6:4]
        uint32_t __7_7 : 1;              // [7]
        uint32_t rc_32k_cal_cnt_p : 4;   // [11:8]
        uint32_t __31_12 : 20;           // [31:12]
    } b;
} REG_RDA2720M_PSM_PSM_RC_CLK_DIV_T;

// psm_cnt_update_l_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_cnt_update_value_15_0_ : 16; // [15:0], read only
        uint32_t __31_16 : 16;                    // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_CNT_UPDATE_L_VALUE_T;

// psm_cnt_update_h_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_cnt_update_value_31_16_ : 16; // [15:0], read only
        uint32_t __31_16 : 16;                     // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_CNT_UPDATE_H_VALUE_T;

// psm_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ext_int : 1;            // [0], read only
        uint32_t pbint1_int : 1;         // [1], read only
        uint32_t pbint2_int : 1;         // [2], read only
        uint32_t charger_int : 1;        // [3], read only
        uint32_t psm_req_int : 1;        // [4], read only
        uint32_t alarm_req_int : 1;      // [5], read only
        uint32_t psm_cnt_update_vld : 1; // [6], read only
        uint32_t __7_7 : 1;              // [7]
        uint32_t ext_int_mask : 1;       // [8]
        uint32_t pbint1_int_mask : 1;    // [9]
        uint32_t pbint2_int_mask : 1;    // [10]
        uint32_t charger_int_mask : 1;   // [11]
        uint32_t psm_req_int_mask : 1;   // [12]
        uint32_t alarm_req_int_mask : 1; // [13]
        uint32_t __31_14 : 18;           // [31:14]
    } b;
} REG_RDA2720M_PSM_PSM_STATUS_T;

// psm_fsm_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_fsm : 15; // [14:0], read only
        uint32_t __31_15 : 17; // [31:15]
    } b;
} REG_RDA2720M_PSM_PSM_FSM_STATUS_T;

// psm_cal_cnt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psm_cal_cnt : 16; // [15:0], read only
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_RDA2720M_PSM_PSM_CAL_CNT_T;

// psm_reg_wr_protect
#define RDA2720M_PSM_PSM_REG_WR(n) (((n)&0xffff) << 0)

// psm_32k_cal_th
#define RDA2720M_PSM_RC_32K_CAL_CNT_N(n) (((n)&0xf) << 0)
#define RDA2720M_PSM_RC_32K_CAL_PRE_TH(n) (((n)&0xf) << 8)

// psm_26m_cal_dn_th
#define RDA2720M_PSM_RC_26M_CAL_CNT_DN_TH(n) (((n)&0xffff) << 0)

// psm_26m_cal_up_th
#define RDA2720M_PSM_RC_26M_CAL_CNT_UP_TH(n) (((n)&0xffff) << 0)

// psm_ctrl
#define RDA2720M_PSM_PSM_EN (1 << 0)
#define RDA2720M_PSM_RTC_PWR_ON_TIMEOUT_EN (1 << 1)
#define RDA2720M_PSM_EXT_INT_PWR_EN (1 << 2)
#define RDA2720M_PSM_PBINT1_PWR_EN (1 << 3)
#define RDA2720M_PSM_PBINT2_PWR_EN (1 << 4)
#define RDA2720M_PSM_CHARGER_PWR_EN (1 << 5)
#define RDA2720M_PSM_PSM_CNT_ALARM_EN (1 << 6)
#define RDA2720M_PSM_PSM_CNT_ALM_EN (1 << 7)
#define RDA2720M_PSM_PSM_SOFTWARE_RESET (1 << 8)
#define RDA2720M_PSM_PSM_CNT_UPDATE (1 << 9)
#define RDA2720M_PSM_PSM_CNT_EN (1 << 10)
#define RDA2720M_PSM_PSM_STATUS_CLR (1 << 11)
#define RDA2720M_PSM_PSM_CAL_EN (1 << 12)
#define RDA2720M_PSM_RTC_32K_CLK_SEL (1 << 15)

// rtc_pwr_off_th1
#define RDA2720M_PSM_RTC_PWR_OFF_CLK_EN_TH(n) (((n)&0xff) << 0)
#define RDA2720M_PSM_RTC_PWR_OFF_HOLD_TH(n) (((n)&0xff) << 8)

// rtc_pwr_off_th2
#define RDA2720M_PSM_RTC_PWR_OFF_RSTN_TH(n) (((n)&0xff) << 0)
#define RDA2720M_PSM_RTC_PWR_OFF_PD_TH(n) (((n)&0xff) << 8)

// rtc_pwr_off_th3
#define RDA2720M_PSM_RTC_PWR_OFF_DONE_TH(n) (((n)&0xff) << 0)

// rtc_pwr_on_th1
#define RDA2720M_PSM_RTC_PWR_ON_PD_TH(n) (((n)&0xff) << 0)
#define RDA2720M_PSM_RTC_PWR_ON_RSTN_TH(n) (((n)&0xff) << 8)

// rtc_pwr_on_th2
#define RDA2720M_PSM_RTC_PWR_ON_HOLD_TH(n) (((n)&0xff) << 0)
#define RDA2720M_PSM_RTC_PWR_ON_CLK_EN_TH(n) (((n)&0xff) << 8)

// rtc_pwr_on_th3
#define RDA2720M_PSM_RTC_PWR_ON_DONE_TH(n) (((n)&0xff) << 0)
#define RDA2720M_PSM_RTC_PWR_ON_TIMEOUT_TH(n) (((n)&0xff) << 8)

// psm_cnt_l_th
#define RDA2720M_PSM_PSM_CNT_TH_15_0_(n) (((n)&0xffff) << 0)

// psm_cnt_h_th
#define RDA2720M_PSM_PSM_CNT_TH_31_16_(n) (((n)&0xffff) << 0)

// psm_alarm_cnt_l_th
#define RDA2720M_PSM_PSM_ALARM_CNT_TH_15_0_(n) (((n)&0xffff) << 0)

// psm_alarm_cnt_h_th
#define RDA2720M_PSM_PSM_ALARM_CNT_TH_31_16_(n) (((n)&0xffff) << 0)

// psm_cnt_interval_th
#define RDA2720M_PSM_PSM_CNT_INTERVAL_TH_15_0_(n) (((n)&0xffff) << 0)

// psm_cnt_interval_phase
#define RDA2720M_PSM_PSM_CNT_INTERVAL_PHASE_15_0_(n) (((n)&0xffff) << 0)

// dcxo
#define RDA2720M_PSM_LDO_DCXO_V(n) (((n)&0x7f) << 0)
#define RDA2720M_PSM_LDO_DCXO_CL_ADJ (1 << 8)
#define RDA2720M_PSM_LDO_DCXO_SHPT_ADJ (1 << 9)
#define RDA2720M_PSM_LDO_DCXO_STB(n) (((n)&0x3) << 10)
#define RDA2720M_PSM_LDO_DCXO_CAP_SEL (1 << 12)
#define RDA2720M_PSM_LDO_DCXO_SHPT_PD (1 << 13)

// psm_rc_clk_div
#define RDA2720M_PSM_WDG_RST_CLK_SEL_EN (1 << 0)
#define RDA2720M_PSM_CLK_CAL_64K_DIV_TH(n) (((n)&0x7) << 4)
#define RDA2720M_PSM_RC_32K_CAL_CNT_P(n) (((n)&0xf) << 8)

// psm_cnt_update_l_value
#define RDA2720M_PSM_PSM_CNT_UPDATE_VALUE_15_0_(n) (((n)&0xffff) << 0)

// psm_cnt_update_h_value
#define RDA2720M_PSM_PSM_CNT_UPDATE_VALUE_31_16_(n) (((n)&0xffff) << 0)

// psm_status
#define RDA2720M_PSM_EXT_INT (1 << 0)
#define RDA2720M_PSM_PBINT1_INT (1 << 1)
#define RDA2720M_PSM_PBINT2_INT (1 << 2)
#define RDA2720M_PSM_CHARGER_INT (1 << 3)
#define RDA2720M_PSM_PSM_REQ_INT (1 << 4)
#define RDA2720M_PSM_ALARM_REQ_INT (1 << 5)
#define RDA2720M_PSM_PSM_CNT_UPDATE_VLD (1 << 6)
#define RDA2720M_PSM_EXT_INT_MASK (1 << 8)
#define RDA2720M_PSM_PBINT1_INT_MASK (1 << 9)
#define RDA2720M_PSM_PBINT2_INT_MASK (1 << 10)
#define RDA2720M_PSM_CHARGER_INT_MASK (1 << 11)
#define RDA2720M_PSM_PSM_REQ_INT_MASK (1 << 12)
#define RDA2720M_PSM_ALARM_REQ_INT_MASK (1 << 13)

// psm_fsm_status
#define RDA2720M_PSM_PSM_FSM(n) (((n)&0x7fff) << 0)

// psm_cal_cnt
#define RDA2720M_PSM_PSM_CAL_CNT(n) (((n)&0xffff) << 0)

#endif // _RDA2720M_PSM_H_
