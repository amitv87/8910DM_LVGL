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

#ifndef _RDA2720M_ADC_H_
#define _RDA2720M_ADC_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_ADC_BASE (0x50308400)

typedef volatile struct
{
    uint32_t adc_version;                  // 0x00000000
    uint32_t adc_cfg_ctrl;                 // 0x00000004
    uint32_t adc_sw_ch_cfg;                // 0x00000008
    uint32_t adc_fast_hw_ch0_cfg;          // 0x0000000c
    uint32_t adc_fast_hw_ch1_cfg;          // 0x00000010
    uint32_t adc_fast_hw_ch2_cfg;          // 0x00000014
    uint32_t adc_fast_hw_ch3_cfg;          // 0x00000018
    uint32_t adc_fast_hw_ch4_cfg;          // 0x0000001c
    uint32_t adc_fast_hw_ch5_cfg;          // 0x00000020
    uint32_t adc_fast_hw_ch6_cfg;          // 0x00000024
    uint32_t adc_fast_hw_ch7_cfg;          // 0x00000028
    uint32_t adc_slow_hw_ch0_cfg;          // 0x0000002c
    uint32_t adc_slow_hw_ch1_cfg;          // 0x00000030
    uint32_t adc_slow_hw_ch2_cfg;          // 0x00000034
    uint32_t adc_slow_hw_ch3_cfg;          // 0x00000038
    uint32_t adc_slow_hw_ch4_cfg;          // 0x0000003c
    uint32_t adc_slow_hw_ch5_cfg;          // 0x00000040
    uint32_t adc_slow_hw_ch6_cfg;          // 0x00000044
    uint32_t adc_slow_hw_ch7_cfg;          // 0x00000048
    uint32_t adc_hw_ch_delay;              // 0x0000004c
    uint32_t adc_dat;                      // 0x00000050
    uint32_t adc_cfg_int_en;               // 0x00000054
    uint32_t adc_cfg_int_clr;              // 0x00000058
    uint32_t adc_cfg_int_sts;              // 0x0000005c
    uint32_t adc_cfg_int_raw;              // 0x00000060
    uint32_t adc_debug;                    // 0x00000064
    uint32_t adc_fast_hw_timer_en;         // 0x00000068
    uint32_t adc_fast_hw_timer_div;        // 0x0000006c
    uint32_t adc_fast_hw_ch0_timer_thresh; // 0x00000070
    uint32_t adc_fast_hw_ch1_timer_thresh; // 0x00000074
    uint32_t adc_fast_hw_ch2_timer_thresh; // 0x00000078
    uint32_t adc_fast_hw_ch3_timer_thresh; // 0x0000007c
    uint32_t adc_fast_hw_ch4_timer_thresh; // 0x00000080
    uint32_t adc_fast_hw_ch5_timer_thresh; // 0x00000084
    uint32_t adc_fast_hw_ch6_timer_thresh; // 0x00000088
    uint32_t adc_fast_hw_ch7_timer_thresh; // 0x0000008c
    uint32_t adc_fast_hw_ch0_dat;          // 0x00000090
    uint32_t adc_fast_hw_ch1_dat;          // 0x00000094
    uint32_t adc_fast_hw_ch2_dat;          // 0x00000098
    uint32_t adc_fast_hw_ch3_dat;          // 0x0000009c
    uint32_t adc_fast_hw_ch4_dat;          // 0x000000a0
    uint32_t adc_fast_hw_ch5_dat;          // 0x000000a4
    uint32_t adc_fast_hw_ch6_dat;          // 0x000000a8
    uint32_t adc_fast_hw_ch7_dat;          // 0x000000ac
    uint32_t auxad_ctl0;                   // 0x000000b0
    uint32_t auxad_ctl1;                   // 0x000000b4
} HWP_RDA2720M_ADC_T;

#define hwp_rda2720mAdc ((HWP_RDA2720M_ADC_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_ADC_BASE))

// adc_version
typedef union {
    uint32_t v;
    struct
    {
        uint32_t auxadc_version : 16; // [15:0], read only
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_VERSION_T;

// adc_cfg_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_en : 1;            // [0]
        uint32_t sw_ch_run : 1;         // [1]
        uint32_t adc_12b : 1;           // [2]
        uint32_t __3_3 : 1;             // [3]
        uint32_t sw_ch_run_num : 4;     // [7:4]
        uint32_t rg_auxad_average : 3;  // [10:8]
        uint32_t __11_11 : 1;           // [11]
        uint32_t adc_offset_cal_en : 1; // [12]
        uint32_t __31_13 : 19;          // [31:13]
    } b;
} REG_RDA2720M_ADC_ADC_CFG_CTRL_T;

// adc_sw_ch_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_cs : 5;    // [4:0]
        uint32_t __5_5 : 1;     // [5]
        uint32_t adc_slow : 1;  // [6]
        uint32_t __8_7 : 2;     // [8:7]
        uint32_t adc_scale : 2; // [10:9]
        uint32_t __31_11 : 21;  // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_SW_CH_CFG_T;

// adc_fast_hw_ch0_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frq_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t frq_slow : 1;     // [6]
        uint32_t frq_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t frq_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH0_CFG_T;

// adc_fast_hw_ch1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frq_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t frq_slow : 1;     // [6]
        uint32_t frq_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t frq_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH1_CFG_T;

// adc_fast_hw_ch2_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frq_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t frq_slow : 1;     // [6]
        uint32_t frq_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t frq_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH2_CFG_T;

// adc_fast_hw_ch3_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frq_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t frq_slow : 1;     // [6]
        uint32_t frq_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t frq_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH3_CFG_T;

// adc_fast_hw_ch4_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frq_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t frq_slow : 1;     // [6]
        uint32_t frq_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t frq_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH4_CFG_T;

// adc_fast_hw_ch5_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frq_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t frq_slow : 1;     // [6]
        uint32_t frq_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t frq_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH5_CFG_T;

// adc_fast_hw_ch6_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frq_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t frq_slow : 1;     // [6]
        uint32_t frq_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t frq_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH6_CFG_T;

// adc_fast_hw_ch7_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frq_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t frq_slow : 1;     // [6]
        uint32_t frq_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t frq_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH7_CFG_T;

// adc_slow_hw_ch0_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t req_slow : 1;     // [6]
        uint32_t req_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t req_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_SLOW_HW_CH0_CFG_T;

// adc_slow_hw_ch1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t req_slow : 1;     // [6]
        uint32_t req_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t req_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_SLOW_HW_CH1_CFG_T;

// adc_slow_hw_ch2_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t req_slow : 1;     // [6]
        uint32_t req_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t req_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_SLOW_HW_CH2_CFG_T;

// adc_slow_hw_ch3_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t req_slow : 1;     // [6]
        uint32_t req_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t req_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_SLOW_HW_CH3_CFG_T;

// adc_slow_hw_ch4_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t req_slow : 1;     // [6]
        uint32_t req_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t req_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_SLOW_HW_CH4_CFG_T;

// adc_slow_hw_ch5_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t req_slow : 1;     // [6]
        uint32_t req_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t req_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_SLOW_HW_CH5_CFG_T;

// adc_slow_hw_ch6_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t req_slow : 1;     // [6]
        uint32_t req_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t req_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_SLOW_HW_CH6_CFG_T;

// adc_slow_hw_ch7_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_cs : 5;       // [4:0]
        uint32_t __5_5 : 1;        // [5]
        uint32_t req_slow : 1;     // [6]
        uint32_t req_delay_en : 1; // [7]
        uint32_t __8_8 : 1;        // [8]
        uint32_t req_scale : 2;    // [10:9]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_RDA2720M_ADC_ADC_SLOW_HW_CH7_CFG_T;

// adc_hw_ch_delay
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hw_ch_delay : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_RDA2720M_ADC_ADC_HW_CH_DELAY_T;

// adc_dat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_dat : 12; // [11:0], read only
        uint32_t __31_12 : 20; // [31:12]
    } b;
} REG_RDA2720M_ADC_ADC_DAT_T;

// adc_cfg_int_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_irq_en : 1; // [0]
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_RDA2720M_ADC_ADC_CFG_INT_EN_T;

// adc_cfg_int_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_irq_clr : 1; // [0]
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_RDA2720M_ADC_ADC_CFG_INT_CLR_T;

// adc_cfg_int_sts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_irq_sts : 1; // [0], read only
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_RDA2720M_ADC_ADC_CFG_INT_STS_T;

// adc_cfg_int_raw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_irq_raw : 1; // [0], read only
        uint32_t __31_1 : 31;     // [31:1]
    } b;
} REG_RDA2720M_ADC_ADC_CFG_INT_RAW_T;

// adc_debug
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_dbg_cnt : 8;   // [7:0], read only
        uint32_t adc_dbg_state : 3; // [10:8], read only
        uint32_t adc_dbg_ch : 5;    // [15:11], read only
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_DEBUG_T;

// adc_fast_hw_timer_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch0_timer_en : 1; // [0]
        uint32_t adc_fast_hw_ch1_timer_en : 1; // [1]
        uint32_t adc_fast_hw_ch2_timer_en : 1; // [2]
        uint32_t adc_fast_hw_ch3_timer_en : 1; // [3]
        uint32_t adc_fast_hw_ch4_timer_en : 1; // [4]
        uint32_t adc_fast_hw_ch5_timer_en : 1; // [5]
        uint32_t adc_fast_hw_ch6_timer_en : 1; // [6]
        uint32_t adc_fast_hw_ch7_timer_en : 1; // [7]
        uint32_t __31_8 : 24;                  // [31:8]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_TIMER_EN_T;

// adc_fast_hw_timer_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_timer_div : 16; // [15:0]
        uint32_t __31_16 : 16;               // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_TIMER_DIV_T;

// adc_fast_hw_ch0_timer_thresh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch0_timer_thresh : 16; // [15:0]
        uint32_t __31_16 : 16;                      // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH0_TIMER_THRESH_T;

// adc_fast_hw_ch1_timer_thresh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch1_timer_thresh : 16; // [15:0]
        uint32_t __31_16 : 16;                      // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH1_TIMER_THRESH_T;

// adc_fast_hw_ch2_timer_thresh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch2_timer_thresh : 16; // [15:0]
        uint32_t __31_16 : 16;                      // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH2_TIMER_THRESH_T;

// adc_fast_hw_ch3_timer_thresh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch3_timer_thresh : 16; // [15:0]
        uint32_t __31_16 : 16;                      // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH3_TIMER_THRESH_T;

// adc_fast_hw_ch4_timer_thresh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch4_timer_thresh : 16; // [15:0]
        uint32_t __31_16 : 16;                      // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH4_TIMER_THRESH_T;

// adc_fast_hw_ch5_timer_thresh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch5_timer_thresh : 16; // [15:0]
        uint32_t __31_16 : 16;                      // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH5_TIMER_THRESH_T;

// adc_fast_hw_ch6_timer_thresh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch6_timer_thresh : 16; // [15:0]
        uint32_t __31_16 : 16;                      // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH6_TIMER_THRESH_T;

// adc_fast_hw_ch7_timer_thresh
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch7_timer_thresh : 16; // [15:0]
        uint32_t __31_16 : 16;                      // [31:16]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH7_TIMER_THRESH_T;

// adc_fast_hw_ch0_dat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch0_dat : 12; // [11:0], read only
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH0_DAT_T;

// adc_fast_hw_ch1_dat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch1_dat : 12; // [11:0], read only
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH1_DAT_T;

// adc_fast_hw_ch2_dat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch2_dat : 12; // [11:0], read only
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH2_DAT_T;

// adc_fast_hw_ch3_dat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch3_dat : 12; // [11:0], read only
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH3_DAT_T;

// adc_fast_hw_ch4_dat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch4_dat : 12; // [11:0], read only
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH4_DAT_T;

// adc_fast_hw_ch5_dat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch5_dat : 12; // [11:0], read only
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH5_DAT_T;

// adc_fast_hw_ch6_dat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch6_dat : 12; // [11:0], read only
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH6_DAT_T;

// adc_fast_hw_ch7_dat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_fast_hw_ch7_dat : 12; // [11:0], read only
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_RDA2720M_ADC_ADC_FAST_HW_CH7_DAT_T;

// auxad_ctl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_auxad_currentsen_en : 1; // [0]
        uint32_t __3_1 : 3;                  // [3:1]
        uint32_t rg_auxad_thm_cal : 1;       // [4]
        uint32_t rg_auxad_ref_sel : 1;       // [5]
        uint32_t __31_6 : 26;                // [31:6]
    } b;
} REG_RDA2720M_ADC_AUXAD_CTL0_T;

// auxad_ctl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_adc_fast_hw_ch0_dvld : 1; // [0], read only
        uint32_t rg_adc_fast_hw_ch1_dvld : 1; // [1], read only
        uint32_t rg_adc_fast_hw_ch2_dvld : 1; // [2], read only
        uint32_t rg_adc_fast_hw_ch3_dvld : 1; // [3], read only
        uint32_t rg_adc_fast_hw_ch4_dvld : 1; // [4], read only
        uint32_t rg_adc_fast_hw_ch5_dvld : 1; // [5], read only
        uint32_t rg_adc_fast_hw_ch6_dvld : 1; // [6], read only
        uint32_t rg_adc_fast_hw_ch7_dvld : 1; // [7], read only
        uint32_t __31_8 : 24;                 // [31:8]
    } b;
} REG_RDA2720M_ADC_AUXAD_CTL1_T;

// adc_version
#define RDA2720M_ADC_AUXADC_VERSION(n) (((n)&0xffff) << 0)

// adc_cfg_ctrl
#define RDA2720M_ADC_ADC_EN (1 << 0)
#define RDA2720M_ADC_SW_CH_RUN (1 << 1)
#define RDA2720M_ADC_ADC_12B (1 << 2)
#define RDA2720M_ADC_SW_CH_RUN_NUM(n) (((n)&0xf) << 4)
#define RDA2720M_ADC_RG_AUXAD_AVERAGE(n) (((n)&0x7) << 8)
#define RDA2720M_ADC_ADC_OFFSET_CAL_EN (1 << 12)

// adc_sw_ch_cfg
#define RDA2720M_ADC_ADC_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_ADC_SLOW (1 << 6)
#define RDA2720M_ADC_ADC_SCALE(n) (((n)&0x3) << 9)

// adc_fast_hw_ch0_cfg
#define RDA2720M_ADC_FRQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_FRQ_SLOW (1 << 6)
#define RDA2720M_ADC_FRQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_FRQ_SCALE(n) (((n)&0x3) << 9)

// adc_fast_hw_ch1_cfg
#define RDA2720M_ADC_FRQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_FRQ_SLOW (1 << 6)
#define RDA2720M_ADC_FRQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_FRQ_SCALE(n) (((n)&0x3) << 9)

// adc_fast_hw_ch2_cfg
#define RDA2720M_ADC_FRQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_FRQ_SLOW (1 << 6)
#define RDA2720M_ADC_FRQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_FRQ_SCALE(n) (((n)&0x3) << 9)

// adc_fast_hw_ch3_cfg
#define RDA2720M_ADC_FRQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_FRQ_SLOW (1 << 6)
#define RDA2720M_ADC_FRQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_FRQ_SCALE(n) (((n)&0x3) << 9)

// adc_fast_hw_ch4_cfg
#define RDA2720M_ADC_FRQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_FRQ_SLOW (1 << 6)
#define RDA2720M_ADC_FRQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_FRQ_SCALE(n) (((n)&0x3) << 9)

// adc_fast_hw_ch5_cfg
#define RDA2720M_ADC_FRQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_FRQ_SLOW (1 << 6)
#define RDA2720M_ADC_FRQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_FRQ_SCALE(n) (((n)&0x3) << 9)

// adc_fast_hw_ch6_cfg
#define RDA2720M_ADC_FRQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_FRQ_SLOW (1 << 6)
#define RDA2720M_ADC_FRQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_FRQ_SCALE(n) (((n)&0x3) << 9)

// adc_fast_hw_ch7_cfg
#define RDA2720M_ADC_FRQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_FRQ_SLOW (1 << 6)
#define RDA2720M_ADC_FRQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_FRQ_SCALE(n) (((n)&0x3) << 9)

// adc_slow_hw_ch0_cfg
#define RDA2720M_ADC_REQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_REQ_SLOW (1 << 6)
#define RDA2720M_ADC_REQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_REQ_SCALE(n) (((n)&0x3) << 9)

// adc_slow_hw_ch1_cfg
#define RDA2720M_ADC_REQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_REQ_SLOW (1 << 6)
#define RDA2720M_ADC_REQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_REQ_SCALE(n) (((n)&0x3) << 9)

// adc_slow_hw_ch2_cfg
#define RDA2720M_ADC_REQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_REQ_SLOW (1 << 6)
#define RDA2720M_ADC_REQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_REQ_SCALE(n) (((n)&0x3) << 9)

// adc_slow_hw_ch3_cfg
#define RDA2720M_ADC_REQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_REQ_SLOW (1 << 6)
#define RDA2720M_ADC_REQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_REQ_SCALE(n) (((n)&0x3) << 9)

// adc_slow_hw_ch4_cfg
#define RDA2720M_ADC_REQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_REQ_SLOW (1 << 6)
#define RDA2720M_ADC_REQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_REQ_SCALE(n) (((n)&0x3) << 9)

// adc_slow_hw_ch5_cfg
#define RDA2720M_ADC_REQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_REQ_SLOW (1 << 6)
#define RDA2720M_ADC_REQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_REQ_SCALE(n) (((n)&0x3) << 9)

// adc_slow_hw_ch6_cfg
#define RDA2720M_ADC_REQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_REQ_SLOW (1 << 6)
#define RDA2720M_ADC_REQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_REQ_SCALE(n) (((n)&0x3) << 9)

// adc_slow_hw_ch7_cfg
#define RDA2720M_ADC_REQ_CS(n) (((n)&0x1f) << 0)
#define RDA2720M_ADC_REQ_SLOW (1 << 6)
#define RDA2720M_ADC_REQ_DELAY_EN (1 << 7)
#define RDA2720M_ADC_REQ_SCALE(n) (((n)&0x3) << 9)

// adc_hw_ch_delay
#define RDA2720M_ADC_HW_CH_DELAY(n) (((n)&0xff) << 0)

// adc_dat
#define RDA2720M_ADC_ADC_DAT(n) (((n)&0xfff) << 0)

// adc_cfg_int_en
#define RDA2720M_ADC_ADC_IRQ_EN (1 << 0)

// adc_cfg_int_clr
#define RDA2720M_ADC_ADC_IRQ_CLR (1 << 0)

// adc_cfg_int_sts
#define RDA2720M_ADC_ADC_IRQ_STS (1 << 0)

// adc_cfg_int_raw
#define RDA2720M_ADC_ADC_IRQ_RAW (1 << 0)

// adc_debug
#define RDA2720M_ADC_ADC_DBG_CNT(n) (((n)&0xff) << 0)
#define RDA2720M_ADC_ADC_DBG_STATE(n) (((n)&0x7) << 8)
#define RDA2720M_ADC_ADC_DBG_CH(n) (((n)&0x1f) << 11)

// adc_fast_hw_timer_en
#define RDA2720M_ADC_ADC_FAST_HW_CH0_TIMER_EN (1 << 0)
#define RDA2720M_ADC_ADC_FAST_HW_CH1_TIMER_EN (1 << 1)
#define RDA2720M_ADC_ADC_FAST_HW_CH2_TIMER_EN (1 << 2)
#define RDA2720M_ADC_ADC_FAST_HW_CH3_TIMER_EN (1 << 3)
#define RDA2720M_ADC_ADC_FAST_HW_CH4_TIMER_EN (1 << 4)
#define RDA2720M_ADC_ADC_FAST_HW_CH5_TIMER_EN (1 << 5)
#define RDA2720M_ADC_ADC_FAST_HW_CH6_TIMER_EN (1 << 6)
#define RDA2720M_ADC_ADC_FAST_HW_CH7_TIMER_EN (1 << 7)

// adc_fast_hw_timer_div
#define RDA2720M_ADC_ADC_FAST_HW_TIMER_DIV(n) (((n)&0xffff) << 0)

// adc_fast_hw_ch0_timer_thresh
#define RDA2720M_ADC_ADC_FAST_HW_CH0_TIMER_THRESH(n) (((n)&0xffff) << 0)

// adc_fast_hw_ch1_timer_thresh
#define RDA2720M_ADC_ADC_FAST_HW_CH1_TIMER_THRESH(n) (((n)&0xffff) << 0)

// adc_fast_hw_ch2_timer_thresh
#define RDA2720M_ADC_ADC_FAST_HW_CH2_TIMER_THRESH(n) (((n)&0xffff) << 0)

// adc_fast_hw_ch3_timer_thresh
#define RDA2720M_ADC_ADC_FAST_HW_CH3_TIMER_THRESH(n) (((n)&0xffff) << 0)

// adc_fast_hw_ch4_timer_thresh
#define RDA2720M_ADC_ADC_FAST_HW_CH4_TIMER_THRESH(n) (((n)&0xffff) << 0)

// adc_fast_hw_ch5_timer_thresh
#define RDA2720M_ADC_ADC_FAST_HW_CH5_TIMER_THRESH(n) (((n)&0xffff) << 0)

// adc_fast_hw_ch6_timer_thresh
#define RDA2720M_ADC_ADC_FAST_HW_CH6_TIMER_THRESH(n) (((n)&0xffff) << 0)

// adc_fast_hw_ch7_timer_thresh
#define RDA2720M_ADC_ADC_FAST_HW_CH7_TIMER_THRESH(n) (((n)&0xffff) << 0)

// adc_fast_hw_ch0_dat
#define RDA2720M_ADC_ADC_FAST_HW_CH0_DAT(n) (((n)&0xfff) << 0)

// adc_fast_hw_ch1_dat
#define RDA2720M_ADC_ADC_FAST_HW_CH1_DAT(n) (((n)&0xfff) << 0)

// adc_fast_hw_ch2_dat
#define RDA2720M_ADC_ADC_FAST_HW_CH2_DAT(n) (((n)&0xfff) << 0)

// adc_fast_hw_ch3_dat
#define RDA2720M_ADC_ADC_FAST_HW_CH3_DAT(n) (((n)&0xfff) << 0)

// adc_fast_hw_ch4_dat
#define RDA2720M_ADC_ADC_FAST_HW_CH4_DAT(n) (((n)&0xfff) << 0)

// adc_fast_hw_ch5_dat
#define RDA2720M_ADC_ADC_FAST_HW_CH5_DAT(n) (((n)&0xfff) << 0)

// adc_fast_hw_ch6_dat
#define RDA2720M_ADC_ADC_FAST_HW_CH6_DAT(n) (((n)&0xfff) << 0)

// adc_fast_hw_ch7_dat
#define RDA2720M_ADC_ADC_FAST_HW_CH7_DAT(n) (((n)&0xfff) << 0)

// auxad_ctl0
#define RDA2720M_ADC_RG_AUXAD_CURRENTSEN_EN (1 << 0)
#define RDA2720M_ADC_RG_AUXAD_THM_CAL (1 << 4)
#define RDA2720M_ADC_RG_AUXAD_REF_SEL (1 << 5)

// auxad_ctl1
#define RDA2720M_ADC_RG_ADC_FAST_HW_CH0_DVLD (1 << 0)
#define RDA2720M_ADC_RG_ADC_FAST_HW_CH1_DVLD (1 << 1)
#define RDA2720M_ADC_RG_ADC_FAST_HW_CH2_DVLD (1 << 2)
#define RDA2720M_ADC_RG_ADC_FAST_HW_CH3_DVLD (1 << 3)
#define RDA2720M_ADC_RG_ADC_FAST_HW_CH4_DVLD (1 << 4)
#define RDA2720M_ADC_RG_ADC_FAST_HW_CH5_DVLD (1 << 5)
#define RDA2720M_ADC_RG_ADC_FAST_HW_CH6_DVLD (1 << 6)
#define RDA2720M_ADC_RG_ADC_FAST_HW_CH7_DVLD (1 << 7)

#endif // _RDA2720M_ADC_H_
