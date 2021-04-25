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

#ifndef _DMC400_H_
#define _DMC400_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_DMC_CTRL_BASE (0x08803000)

typedef volatile struct
{
    uint32_t memc_status;             // 0x00000000
    uint32_t memc_config;             // 0x00000004
    uint32_t memc_cmd;                // 0x00000008
    uint32_t __12[1];                 // 0x0000000c
    uint32_t address_control;         // 0x00000010
    uint32_t decode_control;          // 0x00000014
    uint32_t format_control;          // 0x00000018
    uint32_t __28[1];                 // 0x0000001c
    uint32_t low_power_control;       // 0x00000020
    uint32_t __36[3];                 // 0x00000024
    uint32_t turnaround_priority;     // 0x00000030
    uint32_t hit_priority;            // 0x00000034
    uint32_t qos0_control;            // 0x00000038
    uint32_t qos1_control;            // 0x0000003c
    uint32_t qos2_control;            // 0x00000040
    uint32_t qos3_control;            // 0x00000044
    uint32_t qos4_control;            // 0x00000048
    uint32_t qos5_control;            // 0x0000004c
    uint32_t qos6_control;            // 0x00000050
    uint32_t qos7_control;            // 0x00000054
    uint32_t qos8_control;            // 0x00000058
    uint32_t qos9_control;            // 0x0000005c
    uint32_t qos10_control;           // 0x00000060
    uint32_t qos11_control;           // 0x00000064
    uint32_t qos12_control;           // 0x00000068
    uint32_t __108[1];                // 0x0000006c
    uint32_t qos13_control;           // 0x00000070
    uint32_t qos14_control;           // 0x00000074
    uint32_t qos15_control;           // 0x00000078
    uint32_t timeout_control;         // 0x0000007c
    uint32_t queue_control;           // 0x00000080
    uint32_t __132[1];                // 0x00000084
    uint32_t write_priority_control;  // 0x00000088
    uint32_t write_priority_control2; // 0x0000008c
    uint32_t read_priority_control;   // 0x00000090
    uint32_t read_priority_control2;  // 0x00000094
    uint32_t access_address_match;    // 0x00000098
    uint32_t __156[1];                // 0x0000009c
    uint32_t access_address_mask;     // 0x000000a0
    uint32_t __164[23];               // 0x000000a4
    uint32_t channel_status;          // 0x00000100
    uint32_t __260[1];                // 0x00000104
    uint32_t direct_cmd;              // 0x00000108
    uint32_t __268[1];                // 0x0000010c
    uint32_t mr_data;                 // 0x00000110
    uint32_t __276[3];                // 0x00000114
    uint32_t refresh_control;         // 0x00000120
    uint32_t __292[55];               // 0x00000124
    uint32_t t_refi;                  // 0x00000200
    uint32_t t_rfc;                   // 0x00000204
    uint32_t t_mrr;                   // 0x00000208
    uint32_t t_mrw;                   // 0x0000020c
    uint32_t __528[2];                // 0x00000210
    uint32_t t_rcd;                   // 0x00000218
    uint32_t t_ras;                   // 0x0000021c
    uint32_t t_rp;                    // 0x00000220
    uint32_t t_rpall;                 // 0x00000224
    uint32_t t_rrd;                   // 0x00000228
    uint32_t t_faw;                   // 0x0000022c
    uint32_t read_latency;            // 0x00000230
    uint32_t t_rtr;                   // 0x00000234
    uint32_t t_rtw;                   // 0x00000238
    uint32_t t_rtp;                   // 0x0000023c
    uint32_t write_latency;           // 0x00000240
    uint32_t t_wr;                    // 0x00000244
    uint32_t t_wtr;                   // 0x00000248
    uint32_t t_wtw;                   // 0x0000024c
    uint32_t t_eckd;                  // 0x00000250
    uint32_t t_xckd;                  // 0x00000254
    uint32_t t_ep;                    // 0x00000258
    uint32_t t_xp;                    // 0x0000025c
    uint32_t t_esr;                   // 0x00000260
    uint32_t t_xsr;                   // 0x00000264
    uint32_t t_srckd;                 // 0x00000268
    uint32_t t_cksrd;                 // 0x0000026c
    uint32_t __624[36];               // 0x00000270
    uint32_t t_rddata_en;             // 0x00000300
    uint32_t t_phywrlat;              // 0x00000304
    uint32_t rdlvl_control;           // 0x00000308
    uint32_t rdlvl_mrs;               // 0x0000030c
    uint32_t rdlvl_direct;            // 0x00000310
    uint32_t __788[1];                // 0x00000314
    uint32_t t_rdlvl_en;              // 0x00000318
    uint32_t t_rdlvl_rr;              // 0x0000031c
    uint32_t __800[2];                // 0x00000320
    uint32_t wrlvl_control;           // 0x00000328
    uint32_t wrlvl_mrs;               // 0x0000032c
    uint32_t wrlvl_direct;            // 0x00000330
    uint32_t __820[1];                // 0x00000334
    uint32_t t_wrlvl_en;              // 0x00000338
    uint32_t t_wrlvl_ww;              // 0x0000033c
    uint32_t __832[2];                // 0x00000340
    uint32_t phy_power_control;       // 0x00000348
    uint32_t __844[1];                // 0x0000034c
    uint32_t phy_update_control;      // 0x00000350
    uint32_t __852[43];               // 0x00000354
    uint32_t user_status;             // 0x00000400
    uint32_t user_config0;            // 0x00000404
    uint32_t user_config1;            // 0x00000408
    uint32_t __1036[637];             // 0x0000040c
    uint32_t integ_cfg;               // 0x00000e00
    uint32_t __3588[1];               // 0x00000e04
    uint32_t integ_outputs;           // 0x00000e08
    uint32_t __3596[117];             // 0x00000e0c
    uint32_t periph_id_0;             // 0x00000fe0
    uint32_t periph_id_1;             // 0x00000fe4
    uint32_t periph_id_2;             // 0x00000fe8
    uint32_t periph_id_3;             // 0x00000fec
    uint32_t component_id_0;          // 0x00000ff0
    uint32_t component_id_1;          // 0x00000ff4
    uint32_t component_id_2;          // 0x00000ff8
    uint32_t component_id_3;          // 0x00000ffc
} HWP_DMC400_T;

#define hwp_dmcCtrl ((HWP_DMC400_T *)REG_ACCESS_ADDRESS(REG_DMC_CTRL_BASE))

// memc_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t memc_status : 2; // [1:0], read only
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_DMC400_MEMC_STATUS_T;

// memc_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t system_interfaces_cfg : 2;   // [1:0], read only
        uint32_t __3_2 : 2;                   // [3:2]
        uint32_t memory_interfaces_cfg : 2;   // [5:4], read only
        uint32_t __7_6 : 2;                   // [7:6]
        uint32_t memory_data_width_cfg : 2;   // [9:8], read only
        uint32_t __11_10 : 2;                 // [11:10]
        uint32_t memory_chip_selects_cfg : 2; // [13:12], read only
        uint32_t __15_14 : 2;                 // [15:14]
        uint32_t read_queue_depth_cfg : 3;    // [18:16], read only
        uint32_t __19_19 : 1;                 // [19]
        uint32_t write_queue_depth_cfg : 3;   // [22:20], read only
        uint32_t __23_23 : 1;                 // [23]
        uint32_t max_burst_length_cfg : 2;    // [25:24], read only
        uint32_t __27_26 : 2;                 // [27:26]
        uint32_t memory_ecc_cfg : 1;          // [28], read only
        uint32_t __31_29 : 3;                 // [31:29]
    } b;
} REG_DMC400_MEMC_CONFIG_T;

// memc_cmd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t memc_cmd : 3; // [2:0]
        uint32_t __31_3 : 29;  // [31:3]
    } b;
} REG_DMC400_MEMC_CMD_T;

// address_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t column_bits : 4;  // [3:0]
        uint32_t __7_4 : 4;        // [7:4]
        uint32_t row_bits : 4;     // [11:8]
        uint32_t __15_12 : 4;      // [15:12]
        uint32_t bank_bits : 4;    // [19:16]
        uint32_t __23_20 : 4;      // [23:20]
        uint32_t chip_bits : 2;    // [25:24]
        uint32_t __27_26 : 2;      // [27:26]
        uint32_t channel_bits : 2; // [29:28]
        uint32_t __31_30 : 2;      // [31:30]
    } b;
} REG_DMC400_ADDRESS_CONTROL_T;

// decode_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t addr_decode : 2;  // [1:0]
        uint32_t __3_2 : 2;        // [3:2]
        uint32_t strip_decode : 4; // [7:4]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_DMC400_DECODE_CONTROL_T;

// format_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mem_width : 2;      // [1:0]
        uint32_t __7_2 : 6;          // [7:2]
        uint32_t mem_burst : 2;      // [9:8]
        uint32_t __23_10 : 14;       // [23:10]
        uint32_t acc_granu : 2;      // [25:24]
        uint32_t __27_26 : 2;        // [27:26]
        uint32_t align_boundary : 2; // [29:28]
        uint32_t __31_30 : 2;        // [31:30]
    } b;
} REG_DMC400_FORMAT_CONTROL_T;

// low_power_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t stop_mem_clock_idle : 1; // [0]
        uint32_t stop_mem_clock_sref : 1; // [1]
        uint32_t auto_power_down : 1;     // [2]
        uint32_t auto_self_refresh : 1;   // [3]
        uint32_t asr_period : 4;          // [7:4]
        uint32_t __31_8 : 24;             // [31:8]
    } b;
} REG_DMC400_LOW_POWER_CONTROL_T;

// turnaround_priority
typedef union {
    uint32_t v;
    struct
    {
        uint32_t turnaround_priority : 4; // [3:0]
        uint32_t turnaround_limit : 4;    // [7:4]
        uint32_t __31_8 : 24;             // [31:8]
    } b;
} REG_DMC400_TURNAROUND_PRIORITY_T;

// hit_priority
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hit_priority : 4; // [3:0]
        uint32_t hit_limit : 4;    // [7:4]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_DMC400_HIT_PRIORITY_T;

// qos0_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos0_priority : 4; // [3:0]
        uint32_t __7_4 : 4;         // [7:4]
        uint32_t qos0_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_DMC400_QOS0_CONTROL_T;

// qos1_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos1_priority : 4; // [3:0]
        uint32_t __7_4 : 4;         // [7:4]
        uint32_t qos1_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_DMC400_QOS1_CONTROL_T;

// qos2_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos2_priority : 4; // [3:0]
        uint32_t __7_4 : 4;         // [7:4]
        uint32_t qos2_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_DMC400_QOS2_CONTROL_T;

// qos3_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos3_priority : 4; // [3:0]
        uint32_t __7_4 : 4;         // [7:4]
        uint32_t qos3_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_DMC400_QOS3_CONTROL_T;

// qos4_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos4_priority : 4; // [3:0]
        uint32_t __7_4 : 4;         // [7:4]
        uint32_t qos4_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_DMC400_QOS4_CONTROL_T;

// qos5_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos5_priority : 4; // [3:0]
        uint32_t __7_4 : 4;         // [7:4]
        uint32_t qos5_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_DMC400_QOS5_CONTROL_T;

// qos6_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos6_priority : 4; // [3:0]
        uint32_t __7_4 : 4;         // [7:4]
        uint32_t qos6_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_DMC400_QOS6_CONTROL_T;

// qos7_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos7_priority : 4; // [3:0]
        uint32_t __7_4 : 4;         // [7:4]
        uint32_t qos7_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_DMC400_QOS7_CONTROL_T;

// qos8_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos8_priority : 4; // [3:0]
        uint32_t __7_4 : 4;         // [7:4]
        uint32_t qos8_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_DMC400_QOS8_CONTROL_T;

// qos9_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos9_priority : 4; // [3:0]
        uint32_t __7_4 : 4;         // [7:4]
        uint32_t qos9_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_DMC400_QOS9_CONTROL_T;

// qos10_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos10_priority : 4; // [3:0]
        uint32_t __7_4 : 4;          // [7:4]
        uint32_t qos10_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;       // [31:12]
    } b;
} REG_DMC400_QOS10_CONTROL_T;

// qos11_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos11_priority : 4; // [3:0]
        uint32_t __7_4 : 4;          // [7:4]
        uint32_t qos11_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;       // [31:12]
    } b;
} REG_DMC400_QOS11_CONTROL_T;

// qos12_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos12_priority : 4; // [3:0]
        uint32_t __7_4 : 4;          // [7:4]
        uint32_t qos12_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;       // [31:12]
    } b;
} REG_DMC400_QOS12_CONTROL_T;

// qos13_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos13_priority : 4; // [3:0]
        uint32_t __7_4 : 4;          // [7:4]
        uint32_t qos13_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;       // [31:12]
    } b;
} REG_DMC400_QOS13_CONTROL_T;

// qos14_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos14_priority : 4; // [3:0]
        uint32_t __7_4 : 4;          // [7:4]
        uint32_t qos14_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;       // [31:12]
    } b;
} REG_DMC400_QOS14_CONTROL_T;

// qos15_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t qos15_priority : 4; // [3:0]
        uint32_t __7_4 : 4;          // [7:4]
        uint32_t qos15_timeout : 4;  // [11:8]
        uint32_t __31_12 : 20;       // [31:12]
    } b;
} REG_DMC400_QOS15_CONTROL_T;

// timeout_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t timeout_prescalar : 2; // [1:0]
        uint32_t __31_2 : 30;           // [31:2]
    } b;
} REG_DMC400_TIMEOUT_CONTROL_T;

// queue_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t s0_reserve : 4; // [3:0]
        uint32_t __31_4 : 28;    // [31:4]
    } b;
} REG_DMC400_QUEUE_CONTROL_T;

// write_priority_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t write_threshold_en : 1;          // [0]
        uint32_t __3_1 : 3;                       // [3:1]
        uint32_t write_fill_priority_1_16ths : 4; // [7:4]
        uint32_t write_fill_priority_2_16ths : 4; // [11:8]
        uint32_t write_fill_priority_3_16ths : 4; // [15:12]
        uint32_t write_fill_priority_4_16ths : 4; // [19:16]
        uint32_t write_fill_priority_5_16ths : 4; // [23:20]
        uint32_t write_fill_priority_6_16ths : 4; // [27:24]
        uint32_t write_fill_priority_7_16ths : 4; // [31:28]
    } b;
} REG_DMC400_WRITE_PRIORITY_CONTROL_T;

// write_priority_control2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t write_fill_priority_8_16ths : 4;  // [3:0]
        uint32_t write_fill_priority_9_16ths : 4;  // [7:4]
        uint32_t write_fill_priority_10_16ths : 4; // [11:8]
        uint32_t write_fill_priority_11_16ths : 4; // [15:12]
        uint32_t write_fill_priority_12_16ths : 4; // [19:16]
        uint32_t write_fill_priority_13_16ths : 4; // [23:20]
        uint32_t write_fill_priority_14_16ths : 4; // [27:24]
        uint32_t write_fill_priority_15_16ths : 4; // [31:28]
    } b;
} REG_DMC400_WRITE_PRIORITY_CONTROL2_T;

// read_priority_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t read_escalation : 1;              // [0]
        uint32_t read_in_burst_prioritisation : 1; // [1]
        uint32_t __3_2 : 2;                        // [3:2]
        uint32_t read_fill_priority_1_16ths : 4;   // [7:4]
        uint32_t read_fill_priority_2_16ths : 4;   // [11:8]
        uint32_t read_fill_priority_3_16ths : 4;   // [15:12]
        uint32_t read_fill_priority_4_16ths : 4;   // [19:16]
        uint32_t read_fill_priority_5_16ths : 4;   // [23:20]
        uint32_t read_fill_priority_6_16ths : 4;   // [27:24]
        uint32_t read_fill_priority_7_16ths : 4;   // [31:28]
    } b;
} REG_DMC400_READ_PRIORITY_CONTROL_T;

// read_priority_control2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t read_fill_priority_8_16ths : 4;  // [3:0]
        uint32_t read_fill_priority_9_16ths : 4;  // [7:4]
        uint32_t read_fill_priority_10_16ths : 4; // [11:8]
        uint32_t read_fill_priority_11_16ths : 4; // [15:12]
        uint32_t read_fill_priority_12_16ths : 4; // [19:16]
        uint32_t read_fill_priority_13_16ths : 4; // [23:20]
        uint32_t read_fill_priority_14_16ths : 4; // [27:24]
        uint32_t read_fill_priority_15_16ths : 4; // [31:28]
    } b;
} REG_DMC400_READ_PRIORITY_CONTROL2_T;

// access_address_match
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __11_0 : 12;               // [11:0]
        uint32_t access_address_match : 20; // [31:12]
    } b;
} REG_DMC400_ACCESS_ADDRESS_MATCH_T;

// access_address_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __11_0 : 12;              // [11:0]
        uint32_t access_address_mask : 20; // [31:12]
    } b;
} REG_DMC400_ACCESS_ADDRESS_MASK_T;

// channel_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t m0_state : 4; // [3:0], read only
        uint32_t m1_state : 4; // [7:4], read only
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_DMC400_CHANNEL_STATUS_T;

// direct_cmd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t direct_addr : 16; // [15:0]
        uint32_t direct_ba : 3;    // [18:16]
        uint32_t __19_19 : 1;      // [19]
        uint32_t chip_addr : 1;    // [20]
        uint32_t __23_21 : 3;      // [23:21]
        uint32_t channel_addr : 1; // [24]
        uint32_t __27_25 : 3;      // [27:25]
        uint32_t direct_cmd : 4;   // [31:28]
    } b;
} REG_DMC400_DIRECT_CMD_T;

// mr_data
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mr_data : 8; // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_DMC400_MR_DATA_T;

// refresh_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t per_bank_refresh : 1; // [0]
        uint32_t __31_1 : 31;          // [31:1]
    } b;
} REG_DMC400_REFRESH_CONTROL_T;

// t_refi
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_refi : 11;  // [10:0]
        uint32_t __31_11 : 21; // [31:11]
    } b;
} REG_DMC400_T_REFI_T;

// t_rfc
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rfc : 9;   // [8:0]
        uint32_t __15_9 : 7;  // [15:9]
        uint32_t t_rfcab : 9; // [24:16]
        uint32_t __31_25 : 7; // [31:25]
    } b;
} REG_DMC400_T_RFC_T;

// t_mrr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_mrr : 3;   // [2:0]
        uint32_t __31_3 : 29; // [31:3]
    } b;
} REG_DMC400_T_MRR_T;

// t_mrw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_mrw : 7;   // [6:0]
        uint32_t __31_7 : 25; // [31:7]
    } b;
} REG_DMC400_T_MRW_T;

// t_rcd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rcd : 4;   // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_DMC400_T_RCD_T;

// t_ras
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_ras : 6;   // [5:0]
        uint32_t __31_6 : 26; // [31:6]
    } b;
} REG_DMC400_T_RAS_T;

// t_rp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rp : 5;    // [4:0]
        uint32_t __31_5 : 27; // [31:5]
    } b;
} REG_DMC400_T_RP_T;

// t_rpall
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rpall : 5; // [4:0]
        uint32_t __31_5 : 27; // [31:5]
    } b;
} REG_DMC400_T_RPALL_T;

// t_rrd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rrd : 4;   // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_DMC400_T_RRD_T;

// t_faw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_faw : 6;   // [5:0]
        uint32_t __31_6 : 26; // [31:6]
    } b;
} REG_DMC400_T_FAW_T;

// read_latency
typedef union {
    uint32_t v;
    struct
    {
        uint32_t read_latency : 4; // [3:0]
        uint32_t __31_4 : 28;      // [31:4]
    } b;
} REG_DMC400_READ_LATENCY_T;

// t_rtr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rtr : 4;   // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_DMC400_T_RTR_T;

// t_rtw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rtw : 5;   // [4:0]
        uint32_t __31_5 : 27; // [31:5]
    } b;
} REG_DMC400_T_RTW_T;

// t_rtp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rtp : 4;   // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_DMC400_T_RTP_T;

// write_latency
typedef union {
    uint32_t v;
    struct
    {
        uint32_t write_latency : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_DMC400_WRITE_LATENCY_T;

// t_wr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_wr : 5;    // [4:0]
        uint32_t __31_5 : 27; // [31:5]
    } b;
} REG_DMC400_T_WR_T;

// t_wtr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_wtr : 5;    // [4:0]
        uint32_t __15_5 : 11;  // [15:5]
        uint32_t t_wtr_cs : 5; // [20:16]
        uint32_t __31_21 : 11; // [31:21]
    } b;
} REG_DMC400_T_WTR_T;

// t_wtw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __15_0 : 16;  // [15:0]
        uint32_t t_wtw : 6;    // [21:16]
        uint32_t __31_22 : 10; // [31:22]
    } b;
} REG_DMC400_T_WTW_T;

// t_eckd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_eckd : 4;  // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_DMC400_T_ECKD_T;

// t_xckd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_xckd : 4;  // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_DMC400_T_XCKD_T;

// t_ep
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_ep : 4;    // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_DMC400_T_EP_T;

// t_xp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_xp : 5;     // [4:0]
        uint32_t __15_5 : 11;  // [15:5]
        uint32_t t_xpdll : 5;  // [20:16]
        uint32_t __31_21 : 11; // [31:21]
    } b;
} REG_DMC400_T_XP_T;

// t_esr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_esr : 9;   // [8:0]
        uint32_t __31_9 : 23; // [31:9]
    } b;
} REG_DMC400_T_ESR_T;

// t_xsr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_xsr : 10;    // [9:0]
        uint32_t __15_10 : 6;   // [15:10]
        uint32_t t_xsrdll : 10; // [25:16]
        uint32_t __31_26 : 6;   // [31:26]
    } b;
} REG_DMC400_T_XSR_T;

// t_srckd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_srckd : 4; // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_DMC400_T_SRCKD_T;

// t_cksrd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_cksrd : 4; // [3:0]
        uint32_t __31_4 : 28; // [31:4]
    } b;
} REG_DMC400_T_CKSRD_T;

// t_rddata_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rddata_en : 4; // [3:0]
        uint32_t __31_4 : 28;     // [31:4]
    } b;
} REG_DMC400_T_RDDATA_EN_T;

// t_phywrlat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_phywrlat : 4;  // [3:0]
        uint32_t __7_4 : 4;       // [7:4]
        uint32_t t_phywrdata : 1; // [8]
        uint32_t __31_9 : 23;     // [31:9]
    } b;
} REG_DMC400_T_PHYWRLAT_T;

// rdlvl_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rdlvl_mode : 2;    // [1:0]
        uint32_t __3_2 : 2;         // [3:2]
        uint32_t rdlvl_setup : 1;   // [4]
        uint32_t __7_5 : 3;         // [7:5]
        uint32_t rdlvl_cmd : 1;     // [8]
        uint32_t __11_9 : 3;        // [11:9]
        uint32_t rdlvl_refresh : 1; // [12]
        uint32_t __15_13 : 3;       // [15:13]
        uint32_t rdlvl_reg_sel : 1; // [16]
        uint32_t __31_17 : 15;      // [31:17]
    } b;
} REG_DMC400_RDLVL_CONTROL_T;

// rdlvl_mrs
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rdlvl_mrs : 3; // [2:0]
        uint32_t __31_3 : 29;   // [31:3]
    } b;
} REG_DMC400_RDLVL_MRS_T;

// rdlvl_direct
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rdlvl_req : 2;          // [1:0]
        uint32_t __23_2 : 22;            // [23:2]
        uint32_t rdlvl_chip_addr : 1;    // [24]
        uint32_t __27_25 : 3;            // [27:25]
        uint32_t rdlvl_channel_addr : 1; // [28]
        uint32_t __31_29 : 3;            // [31:29]
    } b;
} REG_DMC400_RDLVL_DIRECT_T;

// t_rdlvl_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rdlvl_en : 6; // [5:0]
        uint32_t __31_6 : 26;    // [31:6]
    } b;
} REG_DMC400_T_RDLVL_EN_T;

// t_rdlvl_rr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_rdlvl_rr : 6; // [5:0]
        uint32_t __31_6 : 26;    // [31:6]
    } b;
} REG_DMC400_T_RDLVL_RR_T;

// wrlvl_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wrlvl_mode : 2;    // [1:0]
        uint32_t __11_2 : 10;       // [11:2]
        uint32_t wrlvl_refresh : 1; // [12]
        uint32_t __31_13 : 19;      // [31:13]
    } b;
} REG_DMC400_WRLVL_CONTROL_T;

// wrlvl_mrs
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wrlvl_mrs : 13; // [12:0]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_DMC400_WRLVL_MRS_T;

// wrlvl_direct
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wrlvl_req : 1;          // [0]
        uint32_t __23_1 : 23;            // [23:1]
        uint32_t wrlvl_chip_addr : 1;    // [24]
        uint32_t __27_25 : 3;            // [27:25]
        uint32_t wrlvl_channel_addr : 1; // [28]
        uint32_t __31_29 : 3;            // [31:29]
    } b;
} REG_DMC400_WRLVL_DIRECT_T;

// t_wrlvl_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_wrlvl_en : 6; // [5:0]
        uint32_t __31_6 : 26;    // [31:6]
    } b;
} REG_DMC400_T_WRLVL_EN_T;

// t_wrlvl_ww
typedef union {
    uint32_t v;
    struct
    {
        uint32_t t_wrlvl_ww : 6; // [5:0]
        uint32_t __31_6 : 26;    // [31:6]
    } b;
} REG_DMC400_T_WRLVL_WW_T;

// phy_power_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lp_wr_en : 1;       // [0]
        uint32_t lp_rd_en : 1;       // [1]
        uint32_t lp_idle_en : 1;     // [2]
        uint32_t lp_pd_en : 1;       // [3]
        uint32_t lp_sref_en : 1;     // [4]
        uint32_t lp_dpd_en : 1;      // [5]
        uint32_t __7_6 : 2;          // [7:6]
        uint32_t lp_wakeup_wr : 4;   // [11:8]
        uint32_t lp_wakeup_rd : 4;   // [15:12]
        uint32_t lp_wakeup_idle : 4; // [19:16]
        uint32_t lp_wakeup_pd : 4;   // [23:20]
        uint32_t lp_wakeup_sref : 4; // [27:24]
        uint32_t lp_wakeup_dpd : 4;  // [31:28]
    } b;
} REG_DMC400_PHY_POWER_CONTROL_T;

// phy_update_control
typedef union {
    uint32_t v;
    struct
    {
        uint32_t phyupd_type_00 : 2; // [1:0]
        uint32_t phyupd_type_01 : 2; // [3:2]
        uint32_t phyupd_type_10 : 2; // [5:4]
        uint32_t phyupd_type_11 : 2; // [7:6]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_DMC400_PHY_UPDATE_CONTROL_T;

// user_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t user_status : 8; // [7:0], read only
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_DMC400_USER_STATUS_T;

// user_config0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t user_config0 : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_DMC400_USER_CONFIG0_T;

// user_config1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t user_config1 : 8; // [7:0]
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_DMC400_USER_CONFIG1_T;

// integ_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t integ_test_en : 1; // [0]
        uint32_t __31_1 : 31;       // [31:1]
    } b;
} REG_DMC400_INTEG_CFG_T;

// integ_outputs
typedef union {
    uint32_t v;
    struct
    {
        uint32_t combined_integ : 1;     // [0]
        uint32_t ecc_sec_integ : 1;      // [1]
        uint32_t ecc_ded_integ : 1;      // [2]
        uint32_t ecc_overflow_integ : 1; // [3]
        uint32_t __31_4 : 28;            // [31:4]
    } b;
} REG_DMC400_INTEG_OUTPUTS_T;

// periph_id_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t part_0 : 8;  // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_DMC400_PERIPH_ID_0_T;

// periph_id_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t part_1 : 4;  // [3:0], read only
        uint32_t des_0 : 4;   // [7:4], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_DMC400_PERIPH_ID_1_T;

// periph_id_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t des_1 : 3;    // [2:0], read only
        uint32_t jedec : 1;    // [3], read only
        uint32_t revision : 4; // [7:4], read only
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_DMC400_PERIPH_ID_2_T;

// periph_id_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cmod : 8;    // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_DMC400_PERIPH_ID_3_T;

// component_id_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prmbl_0 : 8; // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_DMC400_COMPONENT_ID_0_T;

// component_id_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prmbl_1 : 4; // [3:0], read only
        uint32_t pclass : 4;  // [7:4], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_DMC400_COMPONENT_ID_1_T;

// component_id_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prmbl_2 : 8; // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_DMC400_COMPONENT_ID_2_T;

// component_id_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prmbl_3 : 8; // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_DMC400_COMPONENT_ID_3_T;

// memc_status
#define DMC400_MEMC_STATUS(n) (((n)&0x3) << 0)
#define DMC400_MEMC_STATUS_CONFIG (0 << 0)
#define DMC400_MEMC_STATUS_LOW_POWER (1 << 0)
#define DMC400_MEMC_STATUS_PAUSED (2 << 0)
#define DMC400_MEMC_STATUS_READY (3 << 0)

#define DMC400_MEMC_STATUS_V_CONFIG (0)
#define DMC400_MEMC_STATUS_V_LOW_POWER (1)
#define DMC400_MEMC_STATUS_V_PAUSED (2)
#define DMC400_MEMC_STATUS_V_READY (3)

// memc_config
#define DMC400_SYSTEM_INTERFACES_CFG(n) (((n)&0x3) << 0)
#define DMC400_SYSTEM_INTERFACES_CFG_1_SYSTEM_INTERFACE (0 << 0)
#define DMC400_SYSTEM_INTERFACES_CFG_2_SYSTEM_INTERFACE (1 << 0)
#define DMC400_SYSTEM_INTERFACES_CFG_4_SYSTEM_INTERFACE (3 << 0)
#define DMC400_MEMORY_INTERFACES_CFG(n) (((n)&0x3) << 4)
#define DMC400_MEMORY_INTERFACES_CFG_1_MEMORY_INTERFACE (0 << 4)
#define DMC400_MEMORY_INTERFACES_CFG_2_MEMORY_INTERFACE (1 << 4)
#define DMC400_MEMORY_DATA_WIDTH_CFG(n) (((n)&0x3) << 8)
#define DMC400_MEMORY_DATA_WIDTH_CFG_32BIT_PHY_IF (1 << 8)
#define DMC400_MEMORY_DATA_WIDTH_CFG_64BIT_PHY_IF (2 << 8)
#define DMC400_MEMORY_DATA_WIDTH_CFG_128BIT_PHY_IF (3 << 8)
#define DMC400_MEMORY_CHIP_SELECTS_CFG(n) (((n)&0x3) << 12)
#define DMC400_MEMORY_CHIP_SELECTS_CFG_1_CHIP_SEL (0 << 12)
#define DMC400_MEMORY_CHIP_SELECTS_CFG_2_CHIP_SEL (1 << 12)
#define DMC400_READ_QUEUE_DEPTH_CFG(n) (((n)&0x7) << 16)
#define DMC400_READ_QUEUE_DEPTH_CFG_16_ENTRY (0 << 16)
#define DMC400_READ_QUEUE_DEPTH_CFG_32_ENTRY (1 << 16)
#define DMC400_READ_QUEUE_DEPTH_CFG_64_ENTRY (3 << 16)
#define DMC400_READ_QUEUE_DEPTH_CFG_128_ENTRY (7 << 16)
#define DMC400_WRITE_QUEUE_DEPTH_CFG(n) (((n)&0x7) << 20)
#define DMC400_WRITE_QUEUE_DEPTH_CFG_16_ENTRY (0 << 20)
#define DMC400_WRITE_QUEUE_DEPTH_CFG_32_ENTRY (1 << 20)
#define DMC400_WRITE_QUEUE_DEPTH_CFG_64_ENTRY (3 << 20)
#define DMC400_WRITE_QUEUE_DEPTH_CFG_128_ENTRY (7 << 20)
#define DMC400_MAX_BURST_LENGTH_CFG(n) (((n)&0x3) << 24)
#define DMC400_MAX_BURST_LENGTH_CFG_2_DMC_CYCLE (1 << 24)
#define DMC400_MAX_BURST_LENGTH_CFG_4_DMC_CYCLE (2 << 24)
#define DMC400_MAX_BURST_LENGTH_CFG_8_DMC_CYCLE (3 << 24)
#define DMC400_MEMORY_ECC_CFG_FALSE (0 << 28)
#define DMC400_MEMORY_ECC_CFG_TRUE (1 << 28)

#define DMC400_SYSTEM_INTERFACES_CFG_V_1_SYSTEM_INTERFACE (0)
#define DMC400_SYSTEM_INTERFACES_CFG_V_2_SYSTEM_INTERFACE (1)
#define DMC400_SYSTEM_INTERFACES_CFG_V_4_SYSTEM_INTERFACE (3)
#define DMC400_MEMORY_INTERFACES_CFG_V_1_MEMORY_INTERFACE (0)
#define DMC400_MEMORY_INTERFACES_CFG_V_2_MEMORY_INTERFACE (1)
#define DMC400_MEMORY_DATA_WIDTH_CFG_V_32BIT_PHY_IF (1)
#define DMC400_MEMORY_DATA_WIDTH_CFG_V_64BIT_PHY_IF (2)
#define DMC400_MEMORY_DATA_WIDTH_CFG_V_128BIT_PHY_IF (3)
#define DMC400_MEMORY_CHIP_SELECTS_CFG_V_1_CHIP_SEL (0)
#define DMC400_MEMORY_CHIP_SELECTS_CFG_V_2_CHIP_SEL (1)
#define DMC400_READ_QUEUE_DEPTH_CFG_V_16_ENTRY (0)
#define DMC400_READ_QUEUE_DEPTH_CFG_V_32_ENTRY (1)
#define DMC400_READ_QUEUE_DEPTH_CFG_V_64_ENTRY (3)
#define DMC400_READ_QUEUE_DEPTH_CFG_V_128_ENTRY (7)
#define DMC400_WRITE_QUEUE_DEPTH_CFG_V_16_ENTRY (0)
#define DMC400_WRITE_QUEUE_DEPTH_CFG_V_32_ENTRY (1)
#define DMC400_WRITE_QUEUE_DEPTH_CFG_V_64_ENTRY (3)
#define DMC400_WRITE_QUEUE_DEPTH_CFG_V_128_ENTRY (7)
#define DMC400_MAX_BURST_LENGTH_CFG_V_2_DMC_CYCLE (1)
#define DMC400_MAX_BURST_LENGTH_CFG_V_4_DMC_CYCLE (2)
#define DMC400_MAX_BURST_LENGTH_CFG_V_8_DMC_CYCLE (3)
#define DMC400_MEMORY_ECC_CFG_V_FALSE (0)
#define DMC400_MEMORY_ECC_CFG_V_TRUE (1)

// memc_cmd
#define DMC400_MEMC_CMD(n) (((n)&0x7) << 0)
#define DMC400_MEMC_CMD_CONFIG (0 << 0)
#define DMC400_MEMC_CMD_SLEEP (1 << 0)
#define DMC400_MEMC_CMD_PAUSE (2 << 0)
#define DMC400_MEMC_CMD_GO (3 << 0)
#define DMC400_MEMC_CMD_INVALIDATE (4 << 0)

#define DMC400_MEMC_CMD_V_CONFIG (0)
#define DMC400_MEMC_CMD_V_SLEEP (1)
#define DMC400_MEMC_CMD_V_PAUSE (2)
#define DMC400_MEMC_CMD_V_GO (3)
#define DMC400_MEMC_CMD_V_INVALIDATE (4)

// address_control
#define DMC400_COLUMN_BITS(n) (((n)&0xf) << 0)
#define DMC400_COLUMN_BITS_8_COL_BITS (0 << 0)
#define DMC400_COLUMN_BITS_9_COL_BITS (1 << 0)
#define DMC400_COLUMN_BITS_10_COL_BITS (2 << 0)
#define DMC400_COLUMN_BITS_11_COL_BITS (3 << 0)
#define DMC400_COLUMN_BITS_12_COL_BITS (4 << 0)
#define DMC400_ROW_BITS(n) (((n)&0xf) << 8)
#define DMC400_ROW_BITS_13_ROW_BITS (2 << 8)
#define DMC400_ROW_BITS_14_ROW_BITS (3 << 8)
#define DMC400_ROW_BITS_15_ROW_BITS (4 << 8)
#define DMC400_ROW_BITS_16_ROW_BITS (5 << 8)
#define DMC400_BANK_BITS(n) (((n)&0xf) << 16)
#define DMC400_BANK_BITS_2_BANK_BITS_4BK (2 << 16)
#define DMC400_BANK_BITS_3_BANK_BITS_8BK (3 << 16)
#define DMC400_CHIP_BITS(n) (((n)&0x3) << 24)
#define DMC400_CHIP_BITS_0_CHIP_BITS_1CS (0 << 24)
#define DMC400_CHIP_BITS_1_CHIP_BITS_2CS (1 << 24)
#define DMC400_CHANNEL_BITS(n) (((n)&0x3) << 28)
#define DMC400_CHANNEL_BITS_0_CHANNEL_BITS_1MEMIF (0 << 28)
#define DMC400_CHANNEL_BITS_1_CHANNEL_BITS_2MEMIF (1 << 28)

#define DMC400_COLUMN_BITS_V_8_COL_BITS (0)
#define DMC400_COLUMN_BITS_V_9_COL_BITS (1)
#define DMC400_COLUMN_BITS_V_10_COL_BITS (2)
#define DMC400_COLUMN_BITS_V_11_COL_BITS (3)
#define DMC400_COLUMN_BITS_V_12_COL_BITS (4)
#define DMC400_ROW_BITS_V_13_ROW_BITS (2)
#define DMC400_ROW_BITS_V_14_ROW_BITS (3)
#define DMC400_ROW_BITS_V_15_ROW_BITS (4)
#define DMC400_ROW_BITS_V_16_ROW_BITS (5)
#define DMC400_BANK_BITS_V_2_BANK_BITS_4BK (2)
#define DMC400_BANK_BITS_V_3_BANK_BITS_8BK (3)
#define DMC400_CHIP_BITS_V_0_CHIP_BITS_1CS (0)
#define DMC400_CHIP_BITS_V_1_CHIP_BITS_2CS (1)
#define DMC400_CHANNEL_BITS_V_0_CHANNEL_BITS_1MEMIF (0)
#define DMC400_CHANNEL_BITS_V_1_CHANNEL_BITS_2MEMIF (1)

// decode_control
#define DMC400_ADDR_DECODE(n) (((n)&0x3) << 0)
#define DMC400_ADDR_DECODE_CHANNEL_CHIP_ROW_BANK_COL (0 << 0)
#define DMC400_ADDR_DECODE_ROW_CHANNEL_CHIP_BANK_COL (1 << 0)
#define DMC400_ADDR_DECODE_CHIP_BANK_ROW_CHANNEL_COL (2 << 0)
#define DMC400_ADDR_DECODE_ROW_CHIP_BANK_CHANNEL_COL (3 << 0)
#define DMC400_STRIP_DECODE(n) (((n)&0xf) << 4)
#define DMC400_STRIP_DECODE_PAGE_ADDR_13_12 (0 << 4)
#define DMC400_STRIP_DECODE_PAGE_ADDR_12_11 (1 << 4)
#define DMC400_STRIP_DECODE_PAGE_ADDR_11_10 (2 << 4)
#define DMC400_STRIP_DECODE_PAGE_ADDR_10_9 (3 << 4)
#define DMC400_STRIP_DECODE_PAGE_ADDR_9_8 (4 << 4)
#define DMC400_STRIP_DECODE_PAGE_ADDR_8_7 (5 << 4)
#define DMC400_STRIP_DECODE_PAGE_ADDR_7_6 (6 << 4)
#define DMC400_STRIP_DECODE_PAGE_ADDR_6_5 (7 << 4)

#define DMC400_ADDR_DECODE_V_CHANNEL_CHIP_ROW_BANK_COL (0)
#define DMC400_ADDR_DECODE_V_ROW_CHANNEL_CHIP_BANK_COL (1)
#define DMC400_ADDR_DECODE_V_CHIP_BANK_ROW_CHANNEL_COL (2)
#define DMC400_ADDR_DECODE_V_ROW_CHIP_BANK_CHANNEL_COL (3)
#define DMC400_STRIP_DECODE_V_PAGE_ADDR_13_12 (0)
#define DMC400_STRIP_DECODE_V_PAGE_ADDR_12_11 (1)
#define DMC400_STRIP_DECODE_V_PAGE_ADDR_11_10 (2)
#define DMC400_STRIP_DECODE_V_PAGE_ADDR_10_9 (3)
#define DMC400_STRIP_DECODE_V_PAGE_ADDR_9_8 (4)
#define DMC400_STRIP_DECODE_V_PAGE_ADDR_8_7 (5)
#define DMC400_STRIP_DECODE_V_PAGE_ADDR_7_6 (6)
#define DMC400_STRIP_DECODE_V_PAGE_ADDR_6_5 (7)

// format_control
#define DMC400_MEM_WIDTH(n) (((n)&0x3) << 0)
#define DMC400_MEM_WIDTH_PHY_WIDTH_32_X16_DDR (1 << 0)
#define DMC400_MEM_WIDTH_PHY_WIDTH_64_X32_DDR (2 << 0)
#define DMC400_MEM_WIDTH_PHY_WIDTH_128_X64_DDR (3 << 0)
#define DMC400_MEM_BURST(n) (((n)&0x3) << 8)
#define DMC400_MEM_BURST_MEM_BURST_2_DDR_BL4 (1 << 8)
#define DMC400_MEM_BURST_MEM_BURST_4_DDR_BL8 (2 << 8)
#define DMC400_MEM_BURST_MEM_BURST_8_DDR_BL16 (3 << 8)
#define DMC400_ACC_GRANU(n) (((n)&0x3) << 24)
#define DMC400_ACC_GRANU_ACC_GRANU_1_DDR_2N (0 << 24)
#define DMC400_ACC_GRANU_ACC_GRANU_2_DDR_4N (1 << 24)
#define DMC400_ACC_GRANU_ACC_GRANU_4_DDR_8N (2 << 24)
#define DMC400_ACC_GRANU_ACC_GRANU_8_DDR_16N (3 << 24)
#define DMC400_ALIGN_BOUNDARY(n) (((n)&0x3) << 28)
#define DMC400_ALIGN_BOUNDARY_ALIGN_BOUNDARY_1_COL_1BIT (0 << 28)
#define DMC400_ALIGN_BOUNDARY_ALIGN_BOUNDARY_2_COL_2BIT (1 << 28)
#define DMC400_ALIGN_BOUNDARY_ALIGN_BOUNDARY_4_COL_3BIT (2 << 28)
#define DMC400_ALIGN_BOUNDARY_ALIGN_BOUNDARY_8_COL_4BIT (3 << 28)

#define DMC400_MEM_WIDTH_V_PHY_WIDTH_32_X16_DDR (1)
#define DMC400_MEM_WIDTH_V_PHY_WIDTH_64_X32_DDR (2)
#define DMC400_MEM_WIDTH_V_PHY_WIDTH_128_X64_DDR (3)
#define DMC400_MEM_BURST_V_MEM_BURST_2_DDR_BL4 (1)
#define DMC400_MEM_BURST_V_MEM_BURST_4_DDR_BL8 (2)
#define DMC400_MEM_BURST_V_MEM_BURST_8_DDR_BL16 (3)
#define DMC400_ACC_GRANU_V_ACC_GRANU_1_DDR_2N (0)
#define DMC400_ACC_GRANU_V_ACC_GRANU_2_DDR_4N (1)
#define DMC400_ACC_GRANU_V_ACC_GRANU_4_DDR_8N (2)
#define DMC400_ACC_GRANU_V_ACC_GRANU_8_DDR_16N (3)
#define DMC400_ALIGN_BOUNDARY_V_ALIGN_BOUNDARY_1_COL_1BIT (0)
#define DMC400_ALIGN_BOUNDARY_V_ALIGN_BOUNDARY_2_COL_2BIT (1)
#define DMC400_ALIGN_BOUNDARY_V_ALIGN_BOUNDARY_4_COL_3BIT (2)
#define DMC400_ALIGN_BOUNDARY_V_ALIGN_BOUNDARY_8_COL_4BIT (3)

// low_power_control
#define DMC400_STOP_MEM_CLOCK_IDLE_DISABLE (0 << 0)
#define DMC400_STOP_MEM_CLOCK_IDLE_ENABLE (1 << 0)
#define DMC400_STOP_MEM_CLOCK_SREF_DISABLE (0 << 1)
#define DMC400_STOP_MEM_CLOCK_SREF_ENABLE (1 << 1)
#define DMC400_AUTO_POWER_DOWN_DISABLE (0 << 2)
#define DMC400_AUTO_POWER_DOWN_ENABLE (1 << 2)
#define DMC400_AUTO_SELF_REFRESH_DISABLE (0 << 3)
#define DMC400_AUTO_SELF_REFRESH_ENABLE (1 << 3)
#define DMC400_ASR_PERIOD(n) (((n)&0xf) << 4)

#define DMC400_STOP_MEM_CLOCK_IDLE_V_DISABLE (0)
#define DMC400_STOP_MEM_CLOCK_IDLE_V_ENABLE (1)
#define DMC400_STOP_MEM_CLOCK_SREF_V_DISABLE (0)
#define DMC400_STOP_MEM_CLOCK_SREF_V_ENABLE (1)
#define DMC400_AUTO_POWER_DOWN_V_DISABLE (0)
#define DMC400_AUTO_POWER_DOWN_V_ENABLE (1)
#define DMC400_AUTO_SELF_REFRESH_V_DISABLE (0)
#define DMC400_AUTO_SELF_REFRESH_V_ENABLE (1)

// turnaround_priority
#define DMC400_TURNAROUND_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_TURNAROUND_LIMIT(n) (((n)&0xf) << 4)

// hit_priority
#define DMC400_HIT_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_HIT_LIMIT(n) (((n)&0xf) << 4)

// qos0_control
#define DMC400_QOS0_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS0_TIMEOUT(n) (((n)&0xf) << 8)

// qos1_control
#define DMC400_QOS1_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS1_TIMEOUT(n) (((n)&0xf) << 8)

// qos2_control
#define DMC400_QOS2_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS2_TIMEOUT(n) (((n)&0xf) << 8)

// qos3_control
#define DMC400_QOS3_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS3_TIMEOUT(n) (((n)&0xf) << 8)

// qos4_control
#define DMC400_QOS4_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS4_TIMEOUT(n) (((n)&0xf) << 8)

// qos5_control
#define DMC400_QOS5_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS5_TIMEOUT(n) (((n)&0xf) << 8)

// qos6_control
#define DMC400_QOS6_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS6_TIMEOUT(n) (((n)&0xf) << 8)

// qos7_control
#define DMC400_QOS7_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS7_TIMEOUT(n) (((n)&0xf) << 8)

// qos8_control
#define DMC400_QOS8_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS8_TIMEOUT(n) (((n)&0xf) << 8)

// qos9_control
#define DMC400_QOS9_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS9_TIMEOUT(n) (((n)&0xf) << 8)

// qos10_control
#define DMC400_QOS10_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS10_TIMEOUT(n) (((n)&0xf) << 8)

// qos11_control
#define DMC400_QOS11_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS11_TIMEOUT(n) (((n)&0xf) << 8)

// qos12_control
#define DMC400_QOS12_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS12_TIMEOUT(n) (((n)&0xf) << 8)

// qos13_control
#define DMC400_QOS13_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS13_TIMEOUT(n) (((n)&0xf) << 8)

// qos14_control
#define DMC400_QOS14_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS14_TIMEOUT(n) (((n)&0xf) << 8)

// qos15_control
#define DMC400_QOS15_PRIORITY(n) (((n)&0xf) << 0)
#define DMC400_QOS15_TIMEOUT(n) (((n)&0xf) << 8)

// timeout_control
#define DMC400_TIMEOUT_PRESCALAR(n) (((n)&0x3) << 0)
#define DMC400_TIMEOUT_PRESCALAR_8_CLK (0 << 0)
#define DMC400_TIMEOUT_PRESCALAR_16_CLK (1 << 0)
#define DMC400_TIMEOUT_PRESCALAR_32_CLK (2 << 0)
#define DMC400_TIMEOUT_PRESCALAR_64_CLK (3 << 0)

#define DMC400_TIMEOUT_PRESCALAR_V_8_CLK (0)
#define DMC400_TIMEOUT_PRESCALAR_V_16_CLK (1)
#define DMC400_TIMEOUT_PRESCALAR_V_32_CLK (2)
#define DMC400_TIMEOUT_PRESCALAR_V_64_CLK (3)

// queue_control
#define DMC400_S0_RESERVE(n) (((n)&0xf) << 0)

// write_priority_control
#define DMC400_WRITE_THRESHOLD_EN_DISABLE (0 << 0)
#define DMC400_WRITE_THRESHOLD_EN_ENABLE (1 << 0)
#define DMC400_WRITE_FILL_PRIORITY_1_16THS(n) (((n)&0xf) << 4)
#define DMC400_WRITE_FILL_PRIORITY_2_16THS(n) (((n)&0xf) << 8)
#define DMC400_WRITE_FILL_PRIORITY_3_16THS(n) (((n)&0xf) << 12)
#define DMC400_WRITE_FILL_PRIORITY_4_16THS(n) (((n)&0xf) << 16)
#define DMC400_WRITE_FILL_PRIORITY_5_16THS(n) (((n)&0xf) << 20)
#define DMC400_WRITE_FILL_PRIORITY_6_16THS(n) (((n)&0xf) << 24)
#define DMC400_WRITE_FILL_PRIORITY_7_16THS(n) (((n)&0xf) << 28)

#define DMC400_WRITE_THRESHOLD_EN_V_DISABLE (0)
#define DMC400_WRITE_THRESHOLD_EN_V_ENABLE (1)

// write_priority_control2
#define DMC400_WRITE_FILL_PRIORITY_8_16THS(n) (((n)&0xf) << 0)
#define DMC400_WRITE_FILL_PRIORITY_9_16THS(n) (((n)&0xf) << 4)
#define DMC400_WRITE_FILL_PRIORITY_10_16THS(n) (((n)&0xf) << 8)
#define DMC400_WRITE_FILL_PRIORITY_11_16THS(n) (((n)&0xf) << 12)
#define DMC400_WRITE_FILL_PRIORITY_12_16THS(n) (((n)&0xf) << 16)
#define DMC400_WRITE_FILL_PRIORITY_13_16THS(n) (((n)&0xf) << 20)
#define DMC400_WRITE_FILL_PRIORITY_14_16THS(n) (((n)&0xf) << 24)
#define DMC400_WRITE_FILL_PRIORITY_15_16THS(n) (((n)&0xf) << 28)

// read_priority_control
#define DMC400_READ_ESCALATION_DISABLE (0 << 0)
#define DMC400_READ_ESCALATION_ENABLE (1 << 0)
#define DMC400_READ_IN_BURST_PRIORITISATION_DISABLE (0 << 1)
#define DMC400_READ_IN_BURST_PRIORITISATION_ENABLE (1 << 1)
#define DMC400_READ_FILL_PRIORITY_1_16THS(n) (((n)&0xf) << 4)
#define DMC400_READ_FILL_PRIORITY_2_16THS(n) (((n)&0xf) << 8)
#define DMC400_READ_FILL_PRIORITY_3_16THS(n) (((n)&0xf) << 12)
#define DMC400_READ_FILL_PRIORITY_4_16THS(n) (((n)&0xf) << 16)
#define DMC400_READ_FILL_PRIORITY_5_16THS(n) (((n)&0xf) << 20)
#define DMC400_READ_FILL_PRIORITY_6_16THS(n) (((n)&0xf) << 24)
#define DMC400_READ_FILL_PRIORITY_7_16THS(n) (((n)&0xf) << 28)

#define DMC400_READ_ESCALATION_V_DISABLE (0)
#define DMC400_READ_ESCALATION_V_ENABLE (1)
#define DMC400_READ_IN_BURST_PRIORITISATION_V_DISABLE (0)
#define DMC400_READ_IN_BURST_PRIORITISATION_V_ENABLE (1)

// read_priority_control2
#define DMC400_READ_FILL_PRIORITY_8_16THS(n) (((n)&0xf) << 0)
#define DMC400_READ_FILL_PRIORITY_9_16THS(n) (((n)&0xf) << 4)
#define DMC400_READ_FILL_PRIORITY_10_16THS(n) (((n)&0xf) << 8)
#define DMC400_READ_FILL_PRIORITY_11_16THS(n) (((n)&0xf) << 12)
#define DMC400_READ_FILL_PRIORITY_12_16THS(n) (((n)&0xf) << 16)
#define DMC400_READ_FILL_PRIORITY_13_16THS(n) (((n)&0xf) << 20)
#define DMC400_READ_FILL_PRIORITY_14_16THS(n) (((n)&0xf) << 24)
#define DMC400_READ_FILL_PRIORITY_15_16THS(n) (((n)&0xf) << 28)

// access_address_match
#define DMC400_ACCESS_ADDRESS_MATCH(n) (((n)&0xfffff) << 12)

// access_address_mask
#define DMC400_ACCESS_ADDRESS_MASK(n) (((n)&0xfffff) << 12)

// channel_status
#define DMC400_M0_STATE(n) (((n)&0xf) << 0)
#define DMC400_M0_STATE_STANDBY (0 << 0)
#define DMC400_M0_STATE_DPD (1 << 0)
#define DMC400_M0_STATE_IDLE (2 << 0)
#define DMC400_M0_STATE_SELF_REFRESH (3 << 0)
#define DMC400_M0_STATE_READING (4 << 0)
#define DMC400_M0_STATE_POWER_DOWN (5 << 0)
#define DMC400_M0_STATE_WRITING (6 << 0)
#define DMC400_M1_STATE(n) (((n)&0xf) << 4)
#define DMC400_M1_STATE_STANDBY (0 << 4)
#define DMC400_M1_STATE_DPD (1 << 4)
#define DMC400_M1_STATE_IDLE (2 << 4)
#define DMC400_M1_STATE_SELF_REFRESH (3 << 4)
#define DMC400_M1_STATE_READING (4 << 4)
#define DMC400_M1_STATE_POWER_DOWN (5 << 4)
#define DMC400_M1_STATE_WRITING (6 << 4)

#define DMC400_M0_STATE_V_STANDBY (0)
#define DMC400_M0_STATE_V_DPD (1)
#define DMC400_M0_STATE_V_IDLE (2)
#define DMC400_M0_STATE_V_SELF_REFRESH (3)
#define DMC400_M0_STATE_V_READING (4)
#define DMC400_M0_STATE_V_POWER_DOWN (5)
#define DMC400_M0_STATE_V_WRITING (6)
#define DMC400_M1_STATE_V_STANDBY (0)
#define DMC400_M1_STATE_V_DPD (1)
#define DMC400_M1_STATE_V_IDLE (2)
#define DMC400_M1_STATE_V_SELF_REFRESH (3)
#define DMC400_M1_STATE_V_READING (4)
#define DMC400_M1_STATE_V_POWER_DOWN (5)
#define DMC400_M1_STATE_V_WRITING (6)

// direct_cmd
#define DMC400_DIRECT_ADDR(n) (((n)&0xffff) << 0)
#define DMC400_DIRECT_BA(n) (((n)&0x7) << 16)
#define DMC400_CHIP_ADDR_CHIP_0 (0 << 20)
#define DMC400_CHIP_ADDR_CHIP_1 (1 << 20)
#define DMC400_CHANNEL_ADDR_CHANNEL_0 (0 << 24)
#define DMC400_CHANNEL_ADDR_CHANNEL_1 (1 << 24)
#define DMC400_DIRECT_CMD(n) (((n)&0xf) << 28)
#define DMC400_DIRECT_CMD_NOP (0 << 28)
#define DMC400_DIRECT_CMD_MRS (1 << 28)
#define DMC400_DIRECT_CMD_PRECHARGEALL (2 << 28)
#define DMC400_DIRECT_CMD_AUTOREFRESH (3 << 28)
#define DMC400_DIRECT_CMD_SELFREFRESH_ENTRY (4 << 28)
#define DMC400_DIRECT_CMD_ZQC (5 << 28)
#define DMC400_DIRECT_CMD_MRR (6 << 28)
#define DMC400_DIRECT_CMD_POWERDOWN_ENTRY (7 << 28)
#define DMC400_DIRECT_CMD_DEEP_POWERDOWN_ENTRY (8 << 28)

#define DMC400_CHIP_ADDR_V_CHIP_0 (0)
#define DMC400_CHIP_ADDR_V_CHIP_1 (1)
#define DMC400_CHANNEL_ADDR_V_CHANNEL_0 (0)
#define DMC400_CHANNEL_ADDR_V_CHANNEL_1 (1)
#define DMC400_DIRECT_CMD_V_NOP (0)
#define DMC400_DIRECT_CMD_V_MRS (1)
#define DMC400_DIRECT_CMD_V_PRECHARGEALL (2)
#define DMC400_DIRECT_CMD_V_AUTOREFRESH (3)
#define DMC400_DIRECT_CMD_V_SELFREFRESH_ENTRY (4)
#define DMC400_DIRECT_CMD_V_ZQC (5)
#define DMC400_DIRECT_CMD_V_MRR (6)
#define DMC400_DIRECT_CMD_V_POWERDOWN_ENTRY (7)
#define DMC400_DIRECT_CMD_V_DEEP_POWERDOWN_ENTRY (8)

// mr_data
#define DMC400_MR_DATA(n) (((n)&0xff) << 0)

// refresh_control
#define DMC400_PER_BANK_REFRESH_ALL_BANK_AUTOREFRESH (0 << 0)
#define DMC400_PER_BANK_REFRESH_PRE_BANK_AUTOREFRESH (1 << 0)

#define DMC400_PER_BANK_REFRESH_V_ALL_BANK_AUTOREFRESH (0)
#define DMC400_PER_BANK_REFRESH_V_PRE_BANK_AUTOREFRESH (1)

// t_refi
#define DMC400_T_REFI(n) (((n)&0x7ff) << 0)

// t_rfc
#define DMC400_T_RFC(n) (((n)&0x1ff) << 0)
#define DMC400_T_RFCAB(n) (((n)&0x1ff) << 16)

// t_mrr
#define DMC400_T_MRR(n) (((n)&0x7) << 0)

// t_mrw
#define DMC400_T_MRW(n) (((n)&0x7f) << 0)

// t_rcd
#define DMC400_T_RCD(n) (((n)&0xf) << 0)

// t_ras
#define DMC400_T_RAS(n) (((n)&0x3f) << 0)

// t_rp
#define DMC400_T_RP(n) (((n)&0x1f) << 0)

// t_rpall
#define DMC400_T_RPALL(n) (((n)&0x1f) << 0)

// t_rrd
#define DMC400_T_RRD(n) (((n)&0xf) << 0)

// t_faw
#define DMC400_T_FAW(n) (((n)&0x3f) << 0)

// read_latency
#define DMC400_READ_LATENCY(n) (((n)&0xf) << 0)

// t_rtr
#define DMC400_T_RTR(n) (((n)&0xf) << 0)

// t_rtw
#define DMC400_T_RTW(n) (((n)&0x1f) << 0)

// t_rtp
#define DMC400_T_RTP(n) (((n)&0xf) << 0)

// write_latency
#define DMC400_WRITE_LATENCY(n) (((n)&0xf) << 0)

// t_wr
#define DMC400_T_WR(n) (((n)&0x1f) << 0)

// t_wtr
#define DMC400_T_WTR(n) (((n)&0x1f) << 0)
#define DMC400_T_WTR_CS(n) (((n)&0x1f) << 16)

// t_wtw
#define DMC400_T_WTW(n) (((n)&0x3f) << 16)

// t_eckd
#define DMC400_T_ECKD(n) (((n)&0xf) << 0)

// t_xckd
#define DMC400_T_XCKD(n) (((n)&0xf) << 0)

// t_ep
#define DMC400_T_EP(n) (((n)&0xf) << 0)

// t_xp
#define DMC400_T_XP(n) (((n)&0x1f) << 0)
#define DMC400_T_XPDLL(n) (((n)&0x1f) << 16)

// t_esr
#define DMC400_T_ESR(n) (((n)&0x1ff) << 0)

// t_xsr
#define DMC400_T_XSR(n) (((n)&0x3ff) << 0)
#define DMC400_T_XSRDLL(n) (((n)&0x3ff) << 16)

// t_srckd
#define DMC400_T_SRCKD(n) (((n)&0xf) << 0)

// t_cksrd
#define DMC400_T_CKSRD(n) (((n)&0xf) << 0)

// t_rddata_en
#define DMC400_T_RDDATA_EN(n) (((n)&0xf) << 0)

// t_phywrlat
#define DMC400_T_PHYWRLAT(n) (((n)&0xf) << 0)
#define DMC400_T_PHYWRDATA (1 << 8)

// rdlvl_control
#define DMC400_RDLVL_MODE(n) (((n)&0x3) << 0)
#define DMC400_RDLVL_MODE_NO_TRAINING (0 << 0)
#define DMC400_RDLVL_MODE_PHY_INDEPENDENT_MODE (1 << 0)
#define DMC400_RDLVL_MODE_PHY_EVALUATION_MODE (2 << 0)
#define DMC400_RDLVL_SETUP_MRS_PRIOR_TRAIN (0 << 4)
#define DMC400_RDLVL_SETUP_NOP_PRIOR_TRAIN (1 << 4)
#define DMC400_RDLVL_CMD_READ_FOR_TRAIN (0 << 8)
#define DMC400_RDLVL_CMD_MRR_FOR_TRAIN (1 << 8)
#define DMC400_RDLVL_REFRESH_PRECHARGEALL_PRIOR_TRAIN (0 << 12)
#define DMC400_RDLVL_REFRESH_PRECHARGEALL_AUTOREFRESH_PRIOR_TRAIN (1 << 12)
#define DMC400_RDLVL_REG_SEL_MR32_FOR_TRAIN (0 << 16)
#define DMC400_RDLVL_REG_SEL_MR40_FOR_TRAIN (1 << 16)

#define DMC400_RDLVL_MODE_V_NO_TRAINING (0)
#define DMC400_RDLVL_MODE_V_PHY_INDEPENDENT_MODE (1)
#define DMC400_RDLVL_MODE_V_PHY_EVALUATION_MODE (2)
#define DMC400_RDLVL_SETUP_V_MRS_PRIOR_TRAIN (0)
#define DMC400_RDLVL_SETUP_V_NOP_PRIOR_TRAIN (1)
#define DMC400_RDLVL_CMD_V_READ_FOR_TRAIN (0)
#define DMC400_RDLVL_CMD_V_MRR_FOR_TRAIN (1)
#define DMC400_RDLVL_REFRESH_V_PRECHARGEALL_PRIOR_TRAIN (0)
#define DMC400_RDLVL_REFRESH_V_PRECHARGEALL_AUTOREFRESH_PRIOR_TRAIN (1)
#define DMC400_RDLVL_REG_SEL_V_MR32_FOR_TRAIN (0)
#define DMC400_RDLVL_REG_SEL_V_MR40_FOR_TRAIN (1)

// rdlvl_mrs
#define DMC400_RDLVL_MRS(n) (((n)&0x7) << 0)

// rdlvl_direct
#define DMC400_RDLVL_REQ(n) (((n)&0x3) << 0)
#define DMC400_RDLVL_REQ_READ_EYE_TRAIN (1 << 0)
#define DMC400_RDLVL_REQ_READ_GATE_TRAIN (2 << 0)
#define DMC400_RDLVL_CHIP_ADDR_CHIP_0 (0 << 24)
#define DMC400_RDLVL_CHIP_ADDR_CHIP_1 (1 << 24)
#define DMC400_RDLVL_CHANNEL_ADDR_CHANNEL_0 (0 << 28)
#define DMC400_RDLVL_CHANNEL_ADDR_CHANNEL_1 (1 << 28)

#define DMC400_RDLVL_REQ_V_READ_EYE_TRAIN (1)
#define DMC400_RDLVL_REQ_V_READ_GATE_TRAIN (2)
#define DMC400_RDLVL_CHIP_ADDR_V_CHIP_0 (0)
#define DMC400_RDLVL_CHIP_ADDR_V_CHIP_1 (1)
#define DMC400_RDLVL_CHANNEL_ADDR_V_CHANNEL_0 (0)
#define DMC400_RDLVL_CHANNEL_ADDR_V_CHANNEL_1 (1)

// t_rdlvl_en
#define DMC400_T_RDLVL_EN(n) (((n)&0x3f) << 0)

// t_rdlvl_rr
#define DMC400_T_RDLVL_RR(n) (((n)&0x3f) << 0)

// wrlvl_control
#define DMC400_WRLVL_MODE(n) (((n)&0x3) << 0)
#define DMC400_WRLVL_MODE_NO_TRAINING (0 << 0)
#define DMC400_WRLVL_MODE_PHY_INDEPENDENT_MODE (1 << 0)
#define DMC400_WRLVL_MODE_PHY_EVALUATION_MODE (2 << 0)
#define DMC400_WRLVL_REFRESH_PRECHARGEALL_PRIOR_TRAIN (0 << 12)
#define DMC400_WRLVL_REFRESH_PRECHARGEALL_AUTOREFRESH_PRIOR_TRAIN (1 << 12)

#define DMC400_WRLVL_MODE_V_NO_TRAINING (0)
#define DMC400_WRLVL_MODE_V_PHY_INDEPENDENT_MODE (1)
#define DMC400_WRLVL_MODE_V_PHY_EVALUATION_MODE (2)
#define DMC400_WRLVL_REFRESH_V_PRECHARGEALL_PRIOR_TRAIN (0)
#define DMC400_WRLVL_REFRESH_V_PRECHARGEALL_AUTOREFRESH_PRIOR_TRAIN (1)

// wrlvl_mrs
#define DMC400_WRLVL_MRS(n) (((n)&0x1fff) << 0)

// wrlvl_direct
#define DMC400_WRLVL_REQ (1 << 0)
#define DMC400_WRLVL_CHIP_ADDR_CHIP_0 (0 << 24)
#define DMC400_WRLVL_CHIP_ADDR_CHIP_1 (1 << 24)
#define DMC400_WRLVL_CHANNEL_ADDR_CHANNEL_0 (0 << 28)
#define DMC400_WRLVL_CHANNEL_ADDR_CHANNEL_1 (1 << 28)

#define DMC400_WRLVL_CHIP_ADDR_V_CHIP_0 (0)
#define DMC400_WRLVL_CHIP_ADDR_V_CHIP_1 (1)
#define DMC400_WRLVL_CHANNEL_ADDR_V_CHANNEL_0 (0)
#define DMC400_WRLVL_CHANNEL_ADDR_V_CHANNEL_1 (1)

// t_wrlvl_en
#define DMC400_T_WRLVL_EN(n) (((n)&0x3f) << 0)

// t_wrlvl_ww
#define DMC400_T_WRLVL_WW(n) (((n)&0x3f) << 0)

// phy_power_control
#define DMC400_LP_WR_EN_DISABLE (0 << 0)
#define DMC400_LP_WR_EN_ENABLE (1 << 0)
#define DMC400_LP_RD_EN_DISABLE (0 << 1)
#define DMC400_LP_RD_EN_ENABLE (1 << 1)
#define DMC400_LP_IDLE_EN_DISABLE (0 << 2)
#define DMC400_LP_IDLE_EN_ENABLE (1 << 2)
#define DMC400_LP_PD_EN_DISABLE (0 << 3)
#define DMC400_LP_PD_EN_ENABLE (1 << 3)
#define DMC400_LP_SREF_EN_DISABLE (0 << 4)
#define DMC400_LP_SREF_EN_ENABLE (1 << 4)
#define DMC400_LP_DPD_EN_DISABLE (0 << 5)
#define DMC400_LP_DPD_EN_ENABLE (1 << 5)
#define DMC400_LP_WAKEUP_WR(n) (((n)&0xf) << 8)
#define DMC400_LP_WAKEUP_RD(n) (((n)&0xf) << 12)
#define DMC400_LP_WAKEUP_IDLE(n) (((n)&0xf) << 16)
#define DMC400_LP_WAKEUP_PD(n) (((n)&0xf) << 20)
#define DMC400_LP_WAKEUP_SREF(n) (((n)&0xf) << 24)
#define DMC400_LP_WAKEUP_DPD(n) (((n)&0xf) << 28)

#define DMC400_LP_WR_EN_V_DISABLE (0)
#define DMC400_LP_WR_EN_V_ENABLE (1)
#define DMC400_LP_RD_EN_V_DISABLE (0)
#define DMC400_LP_RD_EN_V_ENABLE (1)
#define DMC400_LP_IDLE_EN_V_DISABLE (0)
#define DMC400_LP_IDLE_EN_V_ENABLE (1)
#define DMC400_LP_PD_EN_V_DISABLE (0)
#define DMC400_LP_PD_EN_V_ENABLE (1)
#define DMC400_LP_SREF_EN_V_DISABLE (0)
#define DMC400_LP_SREF_EN_V_ENABLE (1)
#define DMC400_LP_DPD_EN_V_DISABLE (0)
#define DMC400_LP_DPD_EN_V_ENABLE (1)

// phy_update_control
#define DMC400_PHYUPD_TYPE_00(n) (((n)&0x3) << 0)
#define DMC400_PHYUPD_TYPE_00_SREF (0 << 0)
#define DMC400_PHYUPD_TYPE_00_STALL (1 << 0)
#define DMC400_PHYUPD_TYPE_00_REFNSTALL (2 << 0)
#define DMC400_PHYUPD_TYPE_00_DEFER (3 << 0)
#define DMC400_PHYUPD_TYPE_01(n) (((n)&0x3) << 2)
#define DMC400_PHYUPD_TYPE_01_SREF (0 << 2)
#define DMC400_PHYUPD_TYPE_01_STALL (1 << 2)
#define DMC400_PHYUPD_TYPE_01_REFNSTALL (2 << 2)
#define DMC400_PHYUPD_TYPE_01_DEFER (3 << 2)
#define DMC400_PHYUPD_TYPE_10(n) (((n)&0x3) << 4)
#define DMC400_PHYUPD_TYPE_10_SREF (0 << 4)
#define DMC400_PHYUPD_TYPE_10_STALL (1 << 4)
#define DMC400_PHYUPD_TYPE_10_REFNSTALL (2 << 4)
#define DMC400_PHYUPD_TYPE_10_DEFER (3 << 4)
#define DMC400_PHYUPD_TYPE_11(n) (((n)&0x3) << 6)
#define DMC400_PHYUPD_TYPE_11_SREF (0 << 6)
#define DMC400_PHYUPD_TYPE_11_STALL (1 << 6)
#define DMC400_PHYUPD_TYPE_11_REFNSTALL (2 << 6)
#define DMC400_PHYUPD_TYPE_11_DEFER (3 << 6)

#define DMC400_PHYUPD_TYPE_00_V_SREF (0)
#define DMC400_PHYUPD_TYPE_00_V_STALL (1)
#define DMC400_PHYUPD_TYPE_00_V_REFNSTALL (2)
#define DMC400_PHYUPD_TYPE_00_V_DEFER (3)
#define DMC400_PHYUPD_TYPE_01_V_SREF (0)
#define DMC400_PHYUPD_TYPE_01_V_STALL (1)
#define DMC400_PHYUPD_TYPE_01_V_REFNSTALL (2)
#define DMC400_PHYUPD_TYPE_01_V_DEFER (3)
#define DMC400_PHYUPD_TYPE_10_V_SREF (0)
#define DMC400_PHYUPD_TYPE_10_V_STALL (1)
#define DMC400_PHYUPD_TYPE_10_V_REFNSTALL (2)
#define DMC400_PHYUPD_TYPE_10_V_DEFER (3)
#define DMC400_PHYUPD_TYPE_11_V_SREF (0)
#define DMC400_PHYUPD_TYPE_11_V_STALL (1)
#define DMC400_PHYUPD_TYPE_11_V_REFNSTALL (2)
#define DMC400_PHYUPD_TYPE_11_V_DEFER (3)

// user_status
#define DMC400_USER_STATUS(n) (((n)&0xff) << 0)

// user_config0
#define DMC400_USER_CONFIG0(n) (((n)&0xff) << 0)

// user_config1
#define DMC400_USER_CONFIG1(n) (((n)&0xff) << 0)

// integ_cfg
#define DMC400_INTEG_TEST_EN (1 << 0)

// integ_outputs
#define DMC400_COMBINED_INTEG (1 << 0)
#define DMC400_ECC_SEC_INTEG (1 << 1)
#define DMC400_ECC_DED_INTEG (1 << 2)
#define DMC400_ECC_OVERFLOW_INTEG (1 << 3)

// periph_id_0
#define DMC400_PART_0(n) (((n)&0xff) << 0)

// periph_id_1
#define DMC400_PART_1(n) (((n)&0xf) << 0)
#define DMC400_DES_0(n) (((n)&0xf) << 4)

// periph_id_2
#define DMC400_DES_1(n) (((n)&0x7) << 0)
#define DMC400_JEDEC (1 << 3)
#define DMC400_REVISION(n) (((n)&0xf) << 4)

// periph_id_3
#define DMC400_CMOD(n) (((n)&0xff) << 0)

// component_id_0
#define DMC400_PRMBL_0(n) (((n)&0xff) << 0)

// component_id_1
#define DMC400_PRMBL_1(n) (((n)&0xf) << 0)
#define DMC400_PCLASS(n) (((n)&0xf) << 4)

// component_id_2
#define DMC400_PRMBL_2(n) (((n)&0xff) << 0)

// component_id_3
#define DMC400_PRMBL_3(n) (((n)&0xff) << 0)

#endif // _DMC400_H_
