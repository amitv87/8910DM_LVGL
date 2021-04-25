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

#ifndef _RDA2720M_RTC_H_
#define _RDA2720M_RTC_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_RTC_BASE (0x50308200)

typedef volatile struct
{
    uint32_t rtc_sec_cnt_value;  // 0x00000000
    uint32_t rtc_min_cnt_value;  // 0x00000004
    uint32_t rtc_hrs_cnt_value;  // 0x00000008
    uint32_t rtc_day_cnt_value;  // 0x0000000c
    uint32_t rtc_sec_cnt_upd;    // 0x00000010
    uint32_t rtc_min_cnt_upd;    // 0x00000014
    uint32_t rtc_hrs_cnt_upd;    // 0x00000018
    uint32_t rtc_day_cnt_upd;    // 0x0000001c
    uint32_t rtc_sec_alm_upd;    // 0x00000020
    uint32_t rtc_min_alm_upd;    // 0x00000024
    uint32_t rtc_hrs_alm_upd;    // 0x00000028
    uint32_t rtc_day_alm_upd;    // 0x0000002c
    uint32_t rtc_int_en;         // 0x00000030
    uint32_t rtc_int_raw_sts;    // 0x00000034
    uint32_t rtc_int_clr;        // 0x00000038
    uint32_t rtc_int_mask_sts;   // 0x0000003c
    uint32_t rtc_sec_alm_value;  // 0x00000040
    uint32_t rtc_min_alm_value;  // 0x00000044
    uint32_t rtc_hrs_alm_value;  // 0x00000048
    uint32_t rtc_day_alm_value;  // 0x0000004c
    uint32_t rtc_spg_value;      // 0x00000050
    uint32_t rtc_spg_upd;        // 0x00000054
    uint32_t rtc_pwr_flag_ctrl;  // 0x00000058
    uint32_t rtc_pwr_flag_sts;   // 0x0000005c
    uint32_t rtc_sec_auxalm_upd; // 0x00000060
    uint32_t rtc_min_auxalm_upd; // 0x00000064
    uint32_t rtc_hrs_auxalm_upd; // 0x00000068
    uint32_t rtc_day_auxalm_upd; // 0x0000006c
    uint32_t rtc_sec_cnt_raw;    // 0x00000070
    uint32_t rtc_min_cnt_raw;    // 0x00000074
    uint32_t rtc_hrs_cnt_raw;    // 0x00000078
    uint32_t rtc_day_cnt_raw;    // 0x0000007c
} HWP_RDA2720M_RTC_T;

#define hwp_rda2720mRtc ((HWP_RDA2720M_RTC_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_RTC_BASE))

// rtc_sec_cnt_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_sec_cnt_value : 6; // [5:0], read only
        uint32_t __31_6 : 26;           // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_SEC_CNT_VALUE_T;

// rtc_min_cnt_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_min_cnt_value : 6; // [5:0], read only
        uint32_t __31_6 : 26;           // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_MIN_CNT_VALUE_T;

// rtc_hrs_cnt_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_hrs_cnt_value : 5; // [4:0], read only
        uint32_t __31_5 : 27;           // [31:5]
    } b;
} REG_RDA2720M_RTC_RTC_HRS_CNT_VALUE_T;

// rtc_day_cnt_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_day_cnt_value : 16; // [15:0], read only
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_DAY_CNT_VALUE_T;

// rtc_sec_cnt_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_sec_cnt_upd : 6; // [5:0]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_SEC_CNT_UPD_T;

// rtc_min_cnt_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_min_cnt_upd : 6; // [5:0]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_MIN_CNT_UPD_T;

// rtc_hrs_cnt_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_hrs_cnt_upd : 5; // [4:0]
        uint32_t __31_5 : 27;         // [31:5]
    } b;
} REG_RDA2720M_RTC_RTC_HRS_CNT_UPD_T;

// rtc_day_cnt_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_day_cnt_upd : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_DAY_CNT_UPD_T;

// rtc_sec_alm_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_sec_alm_upd : 6; // [5:0]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_SEC_ALM_UPD_T;

// rtc_min_alm_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_min_alm_upd : 6; // [5:0]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_MIN_ALM_UPD_T;

// rtc_hrs_alm_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_hrs_alm_upd : 5; // [4:0]
        uint32_t __31_5 : 27;         // [31:5]
    } b;
} REG_RDA2720M_RTC_RTC_HRS_ALM_UPD_T;

// rtc_day_alm_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_day_alm_upd : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_DAY_ALM_UPD_T;

// rtc_int_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_sec_int_en : 1;         // [0]
        uint32_t rtc_min_int_en : 1;         // [1]
        uint32_t rtc_hrs_int_en : 1;         // [2]
        uint32_t rtc_day_int_en : 1;         // [3]
        uint32_t rtc_alm_int_en : 1;         // [4]
        uint32_t rtc_hrs_format_sel : 1;     // [5]
        uint32_t rtc_auxalm_int_en : 1;      // [6]
        uint32_t rtc_spg_upd_int_en : 1;     // [7]
        uint32_t rtc_sec_cnt_upd_int_en : 1; // [8]
        uint32_t rtc_min_cnt_upd_int_en : 1; // [9]
        uint32_t rtc_hrs_cnt_upd_int_en : 1; // [10]
        uint32_t rtc_day_cnt_upd_int_en : 1; // [11]
        uint32_t rtc_sec_alm_upd_int_en : 1; // [12]
        uint32_t rtc_min_alm_upd_int_en : 1; // [13]
        uint32_t rtc_hrs_alm_upd_int_en : 1; // [14]
        uint32_t rtc_day_alm_upd_int_en : 1; // [15]
        uint32_t __31_16 : 16;               // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_INT_EN_T;

// rtc_int_raw_sts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_sec_int_raw_sts : 1;         // [0], read only
        uint32_t rtc_min_int_raw_sts : 1;         // [1], read only
        uint32_t rtc_hrs_int_raw_sts : 1;         // [2], read only
        uint32_t rtc_day_int_raw_sts : 1;         // [3], read only
        uint32_t rtc_alm_int_raw_sts : 1;         // [4], read only
        uint32_t rtc_alm_int0_raw_sts : 1;        // [5], read only
        uint32_t rtc_auxalm_int_raw_sts : 1;      // [6], read only
        uint32_t rtc_spg_upd_int_raw_sts : 1;     // [7], read only
        uint32_t rtc_sec_cnt_upd_int_raw_sts : 1; // [8], read only
        uint32_t rtc_min_cnt_upd_int_raw_sts : 1; // [9], read only
        uint32_t rtc_hrs_cnt_upd_int_raw_sts : 1; // [10], read only
        uint32_t rtc_day_cnt_upd_int_raw_sts : 1; // [11], read only
        uint32_t rtc_sec_alm_upd_int_raw_sts : 1; // [12], read only
        uint32_t rtc_min_alm_upd_int_raw_sts : 1; // [13], read only
        uint32_t rtc_hrs_alm_upd_int_raw_sts : 1; // [14], read only
        uint32_t rtc_day_alm_upd_int_raw_sts : 1; // [15], read only
        uint32_t __31_16 : 16;                    // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_INT_RAW_STS_T;

// rtc_int_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_sec_int_clr : 1;          // [0]
        uint32_t rtc_min_int_clr : 1;          // [1]
        uint32_t rtc_hrs_int_clr : 1;          // [2]
        uint32_t rtc_day_int_clr : 1;          // [3]
        uint32_t rtc_alm_int_clr : 1;          // [4]
        uint32_t __5_5 : 1;                    // [5]
        uint32_t rtc_auxalm_int_clr : 1;       // [6]
        uint32_t rtc_spg_upd_int_clr : 1;      // [7]
        uint32_t rtc_sec_cnt_upd_int_clr : 1;  // [8]
        uint32_t rtc_min_cnt_upd_int_clr : 1;  // [9]
        uint32_t rtc_hour_cnt_upd_int_clr : 1; // [10]
        uint32_t rtc_day_cnt_upd_int_clr : 1;  // [11]
        uint32_t rtc_sec_alm_upd_int_clr : 1;  // [12]
        uint32_t rtc_min_alm_upd_int_clr : 1;  // [13]
        uint32_t rtc_hour_alm_upd_int_clr : 1; // [14]
        uint32_t rtc_day_alm_upd_int_clr : 1;  // [15]
        uint32_t __31_16 : 16;                 // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_INT_CLR_T;

// rtc_int_mask_sts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_sec_int_mask_sts : 1;         // [0], read only
        uint32_t rtc_min_int_mask_sts : 1;         // [1], read only
        uint32_t rtc_hrs_int_mask_sts : 1;         // [2], read only
        uint32_t rtc_day_int_mask_sts : 1;         // [3], read only
        uint32_t rtc_alm_int_mask_sts : 1;         // [4], read only
        uint32_t __5_5 : 1;                        // [5]
        uint32_t rtc_auxalm_int_mask_sts : 1;      // [6], read only
        uint32_t rtc_spg_upd_int_mask_sts : 1;     // [7], read only
        uint32_t rtc_sec_cnt_upd_int_mask_sts : 1; // [8], read only
        uint32_t rtc_min_cnt_upd_int_mask_sts : 1; // [9], read only
        uint32_t rtc_hrs_cnt_upd_int_mask_sts : 1; // [10], read only
        uint32_t rtc_day_cnt_upd_int_mask_sts : 1; // [11], read only
        uint32_t rtc_sec_alm_upd_int_mask_sts : 1; // [12], read only
        uint32_t rtc_min_alm_upd_int_mask_sts : 1; // [13], read only
        uint32_t rtc_hrs_alm_upd_int_mask_sts : 1; // [14], read only
        uint32_t rtc_day_alm_upd_int_mask_sts : 1; // [15], read only
        uint32_t __31_16 : 16;                     // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_INT_MASK_STS_T;

// rtc_sec_alm_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_sec_alm_value : 6; // [5:0], read only
        uint32_t __31_6 : 26;           // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_SEC_ALM_VALUE_T;

// rtc_min_alm_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_min_alm_value : 6; // [5:0], read only
        uint32_t __31_6 : 26;           // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_MIN_ALM_VALUE_T;

// rtc_hrs_alm_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_hrs_alm_value : 5; // [4:0], read only
        uint32_t __31_5 : 27;           // [31:5]
    } b;
} REG_RDA2720M_RTC_RTC_HRS_ALM_VALUE_T;

// rtc_day_alm_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_day_alm_value : 16; // [15:0], read only
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_DAY_ALM_VALUE_T;

// rtc_spg_value
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_almlock_value : 8; // [7:0], read only
        uint32_t rtc_spg_value : 8;     // [15:8], read only
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_SPG_VALUE_T;

// rtc_spg_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_almlock_upd : 8; // [7:0]
        uint32_t rtc_spg_upd : 8;     // [15:8]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_SPG_UPD_T;

// rtc_pwr_flag_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_pwr_flag_clr : 8; // [7:0]
        uint32_t rtc_pwr_flag_set : 8; // [15:8]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_PWR_FLAG_CTRL_T;

// rtc_pwr_flag_sts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_pwr_flag_sts : 8; // [7:0], read only
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_RDA2720M_RTC_RTC_PWR_FLAG_STS_T;

// rtc_sec_auxalm_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_sec_auxalm_upd : 6; // [5:0]
        uint32_t __31_6 : 26;            // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_SEC_AUXALM_UPD_T;

// rtc_min_auxalm_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_min_auxalm_upd : 6; // [5:0]
        uint32_t __31_6 : 26;            // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_MIN_AUXALM_UPD_T;

// rtc_hrs_auxalm_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_hrs_auxalm_upd : 5; // [4:0]
        uint32_t __31_5 : 27;            // [31:5]
    } b;
} REG_RDA2720M_RTC_RTC_HRS_AUXALM_UPD_T;

// rtc_day_auxalm_upd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_day_auxalm_upd : 16; // [15:0]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_DAY_AUXALM_UPD_T;

// rtc_sec_cnt_raw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_sec_cnt_raw : 6; // [5:0], read only
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_SEC_CNT_RAW_T;

// rtc_min_cnt_raw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_min_cnt_raw : 6; // [5:0], read only
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_RDA2720M_RTC_RTC_MIN_CNT_RAW_T;

// rtc_hrs_cnt_raw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_hrs_cnt_raw : 5; // [4:0], read only
        uint32_t __31_5 : 27;         // [31:5]
    } b;
} REG_RDA2720M_RTC_RTC_HRS_CNT_RAW_T;

// rtc_day_cnt_raw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_day_cnt_raw : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_RDA2720M_RTC_RTC_DAY_CNT_RAW_T;

// rtc_sec_cnt_value
#define RDA2720M_RTC_RTC_SEC_CNT_VALUE(n) (((n)&0x3f) << 0)

// rtc_min_cnt_value
#define RDA2720M_RTC_RTC_MIN_CNT_VALUE(n) (((n)&0x3f) << 0)

// rtc_hrs_cnt_value
#define RDA2720M_RTC_RTC_HRS_CNT_VALUE(n) (((n)&0x1f) << 0)

// rtc_day_cnt_value
#define RDA2720M_RTC_RTC_DAY_CNT_VALUE(n) (((n)&0xffff) << 0)

// rtc_sec_cnt_upd
#define RDA2720M_RTC_RTC_SEC_CNT_UPD(n) (((n)&0x3f) << 0)

// rtc_min_cnt_upd
#define RDA2720M_RTC_RTC_MIN_CNT_UPD(n) (((n)&0x3f) << 0)

// rtc_hrs_cnt_upd
#define RDA2720M_RTC_RTC_HRS_CNT_UPD(n) (((n)&0x1f) << 0)

// rtc_day_cnt_upd
#define RDA2720M_RTC_RTC_DAY_CNT_UPD(n) (((n)&0xffff) << 0)

// rtc_sec_alm_upd
#define RDA2720M_RTC_RTC_SEC_ALM_UPD(n) (((n)&0x3f) << 0)

// rtc_min_alm_upd
#define RDA2720M_RTC_RTC_MIN_ALM_UPD(n) (((n)&0x3f) << 0)

// rtc_hrs_alm_upd
#define RDA2720M_RTC_RTC_HRS_ALM_UPD(n) (((n)&0x1f) << 0)

// rtc_day_alm_upd
#define RDA2720M_RTC_RTC_DAY_ALM_UPD(n) (((n)&0xffff) << 0)

// rtc_int_en
#define RDA2720M_RTC_RTC_SEC_INT_EN (1 << 0)
#define RDA2720M_RTC_RTC_MIN_INT_EN (1 << 1)
#define RDA2720M_RTC_RTC_HRS_INT_EN (1 << 2)
#define RDA2720M_RTC_RTC_DAY_INT_EN (1 << 3)
#define RDA2720M_RTC_RTC_ALM_INT_EN (1 << 4)
#define RDA2720M_RTC_RTC_HRS_FORMAT_SEL (1 << 5)
#define RDA2720M_RTC_RTC_AUXALM_INT_EN (1 << 6)
#define RDA2720M_RTC_RTC_SPG_UPD_INT_EN (1 << 7)
#define RDA2720M_RTC_RTC_SEC_CNT_UPD_INT_EN (1 << 8)
#define RDA2720M_RTC_RTC_MIN_CNT_UPD_INT_EN (1 << 9)
#define RDA2720M_RTC_RTC_HRS_CNT_UPD_INT_EN (1 << 10)
#define RDA2720M_RTC_RTC_DAY_CNT_UPD_INT_EN (1 << 11)
#define RDA2720M_RTC_RTC_SEC_ALM_UPD_INT_EN (1 << 12)
#define RDA2720M_RTC_RTC_MIN_ALM_UPD_INT_EN (1 << 13)
#define RDA2720M_RTC_RTC_HRS_ALM_UPD_INT_EN (1 << 14)
#define RDA2720M_RTC_RTC_DAY_ALM_UPD_INT_EN (1 << 15)

// rtc_int_raw_sts
#define RDA2720M_RTC_RTC_SEC_INT_RAW_STS (1 << 0)
#define RDA2720M_RTC_RTC_MIN_INT_RAW_STS (1 << 1)
#define RDA2720M_RTC_RTC_HRS_INT_RAW_STS (1 << 2)
#define RDA2720M_RTC_RTC_DAY_INT_RAW_STS (1 << 3)
#define RDA2720M_RTC_RTC_ALM_INT_RAW_STS (1 << 4)
#define RDA2720M_RTC_RTC_ALM_INT0_RAW_STS (1 << 5)
#define RDA2720M_RTC_RTC_AUXALM_INT_RAW_STS (1 << 6)
#define RDA2720M_RTC_RTC_SPG_UPD_INT_RAW_STS (1 << 7)
#define RDA2720M_RTC_RTC_SEC_CNT_UPD_INT_RAW_STS (1 << 8)
#define RDA2720M_RTC_RTC_MIN_CNT_UPD_INT_RAW_STS (1 << 9)
#define RDA2720M_RTC_RTC_HRS_CNT_UPD_INT_RAW_STS (1 << 10)
#define RDA2720M_RTC_RTC_DAY_CNT_UPD_INT_RAW_STS (1 << 11)
#define RDA2720M_RTC_RTC_SEC_ALM_UPD_INT_RAW_STS (1 << 12)
#define RDA2720M_RTC_RTC_MIN_ALM_UPD_INT_RAW_STS (1 << 13)
#define RDA2720M_RTC_RTC_HRS_ALM_UPD_INT_RAW_STS (1 << 14)
#define RDA2720M_RTC_RTC_DAY_ALM_UPD_INT_RAW_STS (1 << 15)

// rtc_int_clr
#define RDA2720M_RTC_RTC_SEC_INT_CLR (1 << 0)
#define RDA2720M_RTC_RTC_MIN_INT_CLR (1 << 1)
#define RDA2720M_RTC_RTC_HRS_INT_CLR (1 << 2)
#define RDA2720M_RTC_RTC_DAY_INT_CLR (1 << 3)
#define RDA2720M_RTC_RTC_ALM_INT_CLR (1 << 4)
#define RDA2720M_RTC_RTC_AUXALM_INT_CLR (1 << 6)
#define RDA2720M_RTC_RTC_SPG_UPD_INT_CLR (1 << 7)
#define RDA2720M_RTC_RTC_SEC_CNT_UPD_INT_CLR (1 << 8)
#define RDA2720M_RTC_RTC_MIN_CNT_UPD_INT_CLR (1 << 9)
#define RDA2720M_RTC_RTC_HOUR_CNT_UPD_INT_CLR (1 << 10)
#define RDA2720M_RTC_RTC_DAY_CNT_UPD_INT_CLR (1 << 11)
#define RDA2720M_RTC_RTC_SEC_ALM_UPD_INT_CLR (1 << 12)
#define RDA2720M_RTC_RTC_MIN_ALM_UPD_INT_CLR (1 << 13)
#define RDA2720M_RTC_RTC_HOUR_ALM_UPD_INT_CLR (1 << 14)
#define RDA2720M_RTC_RTC_DAY_ALM_UPD_INT_CLR (1 << 15)

// rtc_int_mask_sts
#define RDA2720M_RTC_RTC_SEC_INT_MASK_STS (1 << 0)
#define RDA2720M_RTC_RTC_MIN_INT_MASK_STS (1 << 1)
#define RDA2720M_RTC_RTC_HRS_INT_MASK_STS (1 << 2)
#define RDA2720M_RTC_RTC_DAY_INT_MASK_STS (1 << 3)
#define RDA2720M_RTC_RTC_ALM_INT_MASK_STS (1 << 4)
#define RDA2720M_RTC_RTC_AUXALM_INT_MASK_STS (1 << 6)
#define RDA2720M_RTC_RTC_SPG_UPD_INT_MASK_STS (1 << 7)
#define RDA2720M_RTC_RTC_SEC_CNT_UPD_INT_MASK_STS (1 << 8)
#define RDA2720M_RTC_RTC_MIN_CNT_UPD_INT_MASK_STS (1 << 9)
#define RDA2720M_RTC_RTC_HRS_CNT_UPD_INT_MASK_STS (1 << 10)
#define RDA2720M_RTC_RTC_DAY_CNT_UPD_INT_MASK_STS (1 << 11)
#define RDA2720M_RTC_RTC_SEC_ALM_UPD_INT_MASK_STS (1 << 12)
#define RDA2720M_RTC_RTC_MIN_ALM_UPD_INT_MASK_STS (1 << 13)
#define RDA2720M_RTC_RTC_HRS_ALM_UPD_INT_MASK_STS (1 << 14)
#define RDA2720M_RTC_RTC_DAY_ALM_UPD_INT_MASK_STS (1 << 15)

// rtc_sec_alm_value
#define RDA2720M_RTC_RTC_SEC_ALM_VALUE(n) (((n)&0x3f) << 0)

// rtc_min_alm_value
#define RDA2720M_RTC_RTC_MIN_ALM_VALUE(n) (((n)&0x3f) << 0)

// rtc_hrs_alm_value
#define RDA2720M_RTC_RTC_HRS_ALM_VALUE(n) (((n)&0x1f) << 0)

// rtc_day_alm_value
#define RDA2720M_RTC_RTC_DAY_ALM_VALUE(n) (((n)&0xffff) << 0)

// rtc_spg_value
#define RDA2720M_RTC_RTC_ALMLOCK_VALUE(n) (((n)&0xff) << 0)
#define RDA2720M_RTC_RTC_SPG_VALUE(n) (((n)&0xff) << 8)

// rtc_spg_upd
#define RDA2720M_RTC_RTC_ALMLOCK_UPD(n) (((n)&0xff) << 0)
#define RDA2720M_RTC_RTC_SPG_UPD(n) (((n)&0xff) << 8)

// rtc_pwr_flag_ctrl
#define RDA2720M_RTC_RTC_PWR_FLAG_CLR(n) (((n)&0xff) << 0)
#define RDA2720M_RTC_RTC_PWR_FLAG_SET(n) (((n)&0xff) << 8)

// rtc_pwr_flag_sts
#define RDA2720M_RTC_RTC_PWR_FLAG_STS(n) (((n)&0xff) << 0)

// rtc_sec_auxalm_upd
#define RDA2720M_RTC_RTC_SEC_AUXALM_UPD(n) (((n)&0x3f) << 0)

// rtc_min_auxalm_upd
#define RDA2720M_RTC_RTC_MIN_AUXALM_UPD(n) (((n)&0x3f) << 0)

// rtc_hrs_auxalm_upd
#define RDA2720M_RTC_RTC_HRS_AUXALM_UPD(n) (((n)&0x1f) << 0)

// rtc_day_auxalm_upd
#define RDA2720M_RTC_RTC_DAY_AUXALM_UPD(n) (((n)&0xffff) << 0)

// rtc_sec_cnt_raw
#define RDA2720M_RTC_RTC_SEC_CNT_RAW(n) (((n)&0x3f) << 0)

// rtc_min_cnt_raw
#define RDA2720M_RTC_RTC_MIN_CNT_RAW(n) (((n)&0x3f) << 0)

// rtc_hrs_cnt_raw
#define RDA2720M_RTC_RTC_HRS_CNT_RAW(n) (((n)&0x1f) << 0)

// rtc_day_cnt_raw
#define RDA2720M_RTC_RTC_DAY_CNT_RAW(n) (((n)&0xffff) << 0)

#endif // _RDA2720M_RTC_H_
