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

#ifndef _ARM_AXIDMA_H_
#define _ARM_AXIDMA_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_DMA_BASE (0x090c0000)

typedef volatile struct
{
    uint32_t axidma_conf;          // 0x00000000
    uint32_t axidma_delay;         // 0x00000004
    uint32_t axidma_status;        // 0x00000008
    uint32_t axidma_irq_stat;      // 0x0000000c
    uint32_t axidma_arm_req_stat;  // 0x00000010
    uint32_t axidma_arm_ack_stat;  // 0x00000014
    uint32_t axidma_zsp_req_stat0; // 0x00000018
    uint32_t axidma_zsp_req_stat1; // 0x0000001c
    uint32_t axidma_ch_irq_distr;  // 0x00000020
    uint32_t __36[7];              // 0x00000024
    uint32_t axidma_c0_conf;       // 0x00000040
    uint32_t axidma_c0_map;        // 0x00000044
    uint32_t axidma_c0_saddr;      // 0x00000048
    uint32_t axidma_c0_daddr;      // 0x0000004c
    uint32_t axidma_c0_count;      // 0x00000050
    uint32_t axidma_c0_countp;     // 0x00000054
    uint32_t axidma_c0_status;     // 0x00000058
    uint32_t axidma_c0_sgaddr;     // 0x0000005c
    uint32_t axidma_c0_sgconf;     // 0x00000060
    uint32_t axidma_c0_set;        // 0x00000064
    uint32_t axidma_c0_clr;        // 0x00000068
    uint32_t __108[5];             // 0x0000006c
    uint32_t axidma_c1_conf;       // 0x00000080
    uint32_t axidma_c1_map;        // 0x00000084
    uint32_t axidma_c1_saddr;      // 0x00000088
    uint32_t axidma_c1_daddr;      // 0x0000008c
    uint32_t axidma_c1_count;      // 0x00000090
    uint32_t axidma_c1_countp;     // 0x00000094
    uint32_t axidma_c1_status;     // 0x00000098
    uint32_t axidma_c1_sgaddr;     // 0x0000009c
    uint32_t axidma_c1_sgconf;     // 0x000000a0
    uint32_t axidma_c1_set;        // 0x000000a4
    uint32_t axidma_c1_clr;        // 0x000000a8
    uint32_t __172[5];             // 0x000000ac
    uint32_t axidma_c2_conf;       // 0x000000c0
    uint32_t axidma_c2_map;        // 0x000000c4
    uint32_t axidma_c2_saddr;      // 0x000000c8
    uint32_t axidma_c2_daddr;      // 0x000000cc
    uint32_t axidma_c2_count;      // 0x000000d0
    uint32_t axidma_c2_countp;     // 0x000000d4
    uint32_t axidma_c2_status;     // 0x000000d8
    uint32_t axidma_c2_sgaddr;     // 0x000000dc
    uint32_t axidma_c2_sgconf;     // 0x000000e0
    uint32_t axidma_c2_set;        // 0x000000e4
    uint32_t axidma_c2_clr;        // 0x000000e8
    uint32_t __236[5];             // 0x000000ec
    uint32_t axidma_c3_conf;       // 0x00000100
    uint32_t axidma_c3_map;        // 0x00000104
    uint32_t axidma_c3_saddr;      // 0x00000108
    uint32_t axidma_c3_daddr;      // 0x0000010c
    uint32_t axidma_c3_count;      // 0x00000110
    uint32_t axidma_c3_countp;     // 0x00000114
    uint32_t axidma_c3_status;     // 0x00000118
    uint32_t axidma_c3_sgaddr;     // 0x0000011c
    uint32_t axidma_c3_sgconf;     // 0x00000120
    uint32_t axidma_c3_set;        // 0x00000124
    uint32_t axidma_c3_clr;        // 0x00000128
    uint32_t __300[5];             // 0x0000012c
    uint32_t axidma_c4_conf;       // 0x00000140
    uint32_t axidma_c4_map;        // 0x00000144
    uint32_t axidma_c4_saddr;      // 0x00000148
    uint32_t axidma_c4_daddr;      // 0x0000014c
    uint32_t axidma_c4_count;      // 0x00000150
    uint32_t axidma_c4_countp;     // 0x00000154
    uint32_t axidma_c4_status;     // 0x00000158
    uint32_t axidma_c4_sgaddr;     // 0x0000015c
    uint32_t axidma_c4_sgconf;     // 0x00000160
    uint32_t axidma_c4_set;        // 0x00000164
    uint32_t axidma_c4_clr;        // 0x00000168
    uint32_t __364[5];             // 0x0000016c
    uint32_t axidma_c5_conf;       // 0x00000180
    uint32_t axidma_c5_map;        // 0x00000184
    uint32_t axidma_c5_saddr;      // 0x00000188
    uint32_t axidma_c5_daddr;      // 0x0000018c
    uint32_t axidma_c5_count;      // 0x00000190
    uint32_t axidma_c5_countp;     // 0x00000194
    uint32_t axidma_c5_status;     // 0x00000198
    uint32_t axidma_c5_sgaddr;     // 0x0000019c
    uint32_t axidma_c5_sgconf;     // 0x000001a0
    uint32_t axidma_c5_set;        // 0x000001a4
    uint32_t axidma_c5_clr;        // 0x000001a8
    uint32_t __428[5];             // 0x000001ac
    uint32_t axidma_c6_conf;       // 0x000001c0
    uint32_t axidma_c6_map;        // 0x000001c4
    uint32_t axidma_c6_saddr;      // 0x000001c8
    uint32_t axidma_c6_daddr;      // 0x000001cc
    uint32_t axidma_c6_count;      // 0x000001d0
    uint32_t axidma_c6_countp;     // 0x000001d4
    uint32_t axidma_c6_status;     // 0x000001d8
    uint32_t axidma_c6_sgaddr;     // 0x000001dc
    uint32_t axidma_c6_sgconf;     // 0x000001e0
    uint32_t axidma_c6_set;        // 0x000001e4
    uint32_t axidma_c6_clr;        // 0x000001e8
    uint32_t __492[5];             // 0x000001ec
    uint32_t axidma_c7_conf;       // 0x00000200
    uint32_t axidma_c7_map;        // 0x00000204
    uint32_t axidma_c7_saddr;      // 0x00000208
    uint32_t axidma_c7_daddr;      // 0x0000020c
    uint32_t axidma_c7_count;      // 0x00000210
    uint32_t axidma_c7_countp;     // 0x00000214
    uint32_t axidma_c7_status;     // 0x00000218
    uint32_t axidma_c7_sgaddr;     // 0x0000021c
    uint32_t axidma_c7_sgconf;     // 0x00000220
    uint32_t axidma_c7_set;        // 0x00000224
    uint32_t axidma_c7_clr;        // 0x00000228
    uint32_t __556[5];             // 0x0000022c
    uint32_t axidma_c8_conf;       // 0x00000240
    uint32_t axidma_c8_map;        // 0x00000244
    uint32_t axidma_c8_saddr;      // 0x00000248
    uint32_t axidma_c8_daddr;      // 0x0000024c
    uint32_t axidma_c8_count;      // 0x00000250
    uint32_t axidma_c8_countp;     // 0x00000254
    uint32_t axidma_c8_status;     // 0x00000258
    uint32_t axidma_c8_sgaddr;     // 0x0000025c
    uint32_t axidma_c8_sgconf;     // 0x00000260
    uint32_t axidma_c8_set;        // 0x00000264
    uint32_t axidma_c8_clr;        // 0x00000268
    uint32_t __620[5];             // 0x0000026c
    uint32_t axidma_c9_conf;       // 0x00000280
    uint32_t axidma_c9_map;        // 0x00000284
    uint32_t axidma_c9_saddr;      // 0x00000288
    uint32_t axidma_c9_daddr;      // 0x0000028c
    uint32_t axidma_c9_count;      // 0x00000290
    uint32_t axidma_c9_countp;     // 0x00000294
    uint32_t axidma_c9_status;     // 0x00000298
    uint32_t axidma_c9_sgaddr;     // 0x0000029c
    uint32_t axidma_c9_sgconf;     // 0x000002a0
    uint32_t axidma_c9_set;        // 0x000002a4
    uint32_t axidma_c9_clr;        // 0x000002a8
    uint32_t __684[5];             // 0x000002ac
    uint32_t axidma_c10_conf;      // 0x000002c0
    uint32_t axidma_c10_map;       // 0x000002c4
    uint32_t axidma_c10_saddr;     // 0x000002c8
    uint32_t axidma_c10_daddr;     // 0x000002cc
    uint32_t axidma_c10_count;     // 0x000002d0
    uint32_t axidma_c10_countp;    // 0x000002d4
    uint32_t axidma_c10_status;    // 0x000002d8
    uint32_t axidma_c10_sgaddr;    // 0x000002dc
    uint32_t axidma_c10_sgconf;    // 0x000002e0
    uint32_t axidma_c10_set;       // 0x000002e4
    uint32_t axidma_c10_clr;       // 0x000002e8
    uint32_t __748[5];             // 0x000002ec
    uint32_t axidma_c11_conf;      // 0x00000300
    uint32_t axidma_c11_map;       // 0x00000304
    uint32_t axidma_c11_saddr;     // 0x00000308
    uint32_t axidma_c11_daddr;     // 0x0000030c
    uint32_t axidma_c11_count;     // 0x00000310
    uint32_t axidma_c11_countp;    // 0x00000314
    uint32_t axidma_c11_status;    // 0x00000318
    uint32_t axidma_c11_sgaddr;    // 0x0000031c
    uint32_t axidma_c11_sgconf;    // 0x00000320
    uint32_t axidma_c11_set;       // 0x00000324
    uint32_t axidma_c11_clr;       // 0x00000328
} HWP_ARM_AXIDMA_T;

#define hwp_dma ((HWP_ARM_AXIDMA_T *)REG_ACCESS_ADDRESS(REG_DMA_BASE))

// axidma_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t stop : 1;     // [0]
        uint32_t stop_ie : 1;  // [1]
        uint32_t priority : 1; // [2]
        uint32_t outstand : 2; // [4:3]
        uint32_t mode_sel : 1; // [5]
        uint32_t __31_6 : 26;  // [31:6]
    } b;
} REG_ARM_AXIDMA_AXIDMA_CONF_T;

// axidma_delay
typedef union {
    uint32_t v;
    struct
    {
        uint32_t delay : 16;   // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_DELAY_T;

// axidma_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ch_num : 4;      // [3:0], read only
        uint32_t stop_status : 1; // [4], read only
        uint32_t __31_5 : 27;     // [31:5]
    } b;
} REG_ARM_AXIDMA_AXIDMA_STATUS_T;

// axidma_irq_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ch0_irq : 1;  // [0], read only
        uint32_t ch1_irq : 1;  // [1], read only
        uint32_t ch2_irq : 1;  // [2], read only
        uint32_t ch3_irq : 1;  // [3], read only
        uint32_t ch4_irq : 1;  // [4], read only
        uint32_t ch5_irq : 1;  // [5], read only
        uint32_t ch6_irq : 1;  // [6], read only
        uint32_t ch7_irq : 1;  // [7], read only
        uint32_t ch8_irq : 1;  // [8], read only
        uint32_t ch9_irq : 1;  // [9], read only
        uint32_t ch10_irq : 1; // [10], read only
        uint32_t ch11_irq : 1; // [11], read only
        uint32_t __31_12 : 20; // [31:12]
    } b;
} REG_ARM_AXIDMA_AXIDMA_IRQ_STAT_T;

// axidma_arm_req_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t irq0 : 1;    // [0], read only
        uint32_t irq1 : 1;    // [1], read only
        uint32_t irq2 : 1;    // [2], read only
        uint32_t irq3 : 1;    // [3], read only
        uint32_t irq4 : 1;    // [4], read only
        uint32_t irq5 : 1;    // [5], read only
        uint32_t irq6 : 1;    // [6], read only
        uint32_t irq7 : 1;    // [7], read only
        uint32_t irq8 : 1;    // [8], read only
        uint32_t irq9 : 1;    // [9], read only
        uint32_t irq10 : 1;   // [10], read only
        uint32_t irq11 : 1;   // [11], read only
        uint32_t irq12 : 1;   // [12], read only
        uint32_t irq13 : 1;   // [13], read only
        uint32_t irq14 : 1;   // [14], read only
        uint32_t irq15 : 1;   // [15], read only
        uint32_t irq16 : 1;   // [16], read only
        uint32_t irq17 : 1;   // [17], read only
        uint32_t irq18 : 1;   // [18], read only
        uint32_t irq19 : 1;   // [19], read only
        uint32_t irq20 : 1;   // [20], read only
        uint32_t irq21 : 1;   // [21], read only
        uint32_t irq22 : 1;   // [22], read only
        uint32_t irq23 : 1;   // [23], read only
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_ARM_REQ_STAT_T;

// axidma_arm_ack_stat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ack0 : 1;    // [0], read only
        uint32_t ack1 : 1;    // [1], read only
        uint32_t ack2 : 1;    // [2], read only
        uint32_t ack3 : 1;    // [3], read only
        uint32_t ack4 : 1;    // [4], read only
        uint32_t ack5 : 1;    // [5], read only
        uint32_t ack6 : 1;    // [6], read only
        uint32_t ack7 : 1;    // [7], read only
        uint32_t ack8 : 1;    // [8], read only
        uint32_t ack9 : 1;    // [9], read only
        uint32_t ack10 : 1;   // [10], read only
        uint32_t ack11 : 1;   // [11], read only
        uint32_t ack12 : 1;   // [12], read only
        uint32_t ack13 : 1;   // [13], read only
        uint32_t ack14 : 1;   // [14], read only
        uint32_t ack15 : 1;   // [15], read only
        uint32_t ack16 : 1;   // [16], read only
        uint32_t ack17 : 1;   // [17], read only
        uint32_t ack18 : 1;   // [18], read only
        uint32_t ack19 : 1;   // [19], read only
        uint32_t ack20 : 1;   // [20], read only
        uint32_t ack21 : 1;   // [21], read only
        uint32_t ack22 : 1;   // [22], read only
        uint32_t ack23 : 1;   // [23], read only
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_ARM_ACK_STAT_T;

// axidma_zsp_req_stat0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req0 : 3;    // [2:0]
        uint32_t __3_3 : 1;   // [3]
        uint32_t req1 : 3;    // [6:4]
        uint32_t __7_7 : 1;   // [7]
        uint32_t req2 : 3;    // [10:8]
        uint32_t __11_11 : 1; // [11]
        uint32_t req3 : 3;    // [14:12]
        uint32_t __15_15 : 1; // [15]
        uint32_t req4 : 3;    // [18:16]
        uint32_t __19_19 : 1; // [19]
        uint32_t req5 : 3;    // [22:20]
        uint32_t __23_23 : 1; // [23]
        uint32_t req6 : 3;    // [26:24]
        uint32_t __27_27 : 1; // [27]
        uint32_t req7 : 3;    // [30:28]
        uint32_t __31_31 : 1; // [31]
    } b;
} REG_ARM_AXIDMA_AXIDMA_ZSP_REQ_STAT0_T;

// axidma_zsp_req_stat1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req8 : 3;     // [2:0]
        uint32_t __3_3 : 1;    // [3]
        uint32_t req9 : 3;     // [6:4]
        uint32_t __7_7 : 1;    // [7]
        uint32_t req10 : 3;    // [10:8]
        uint32_t __11_11 : 1;  // [11]
        uint32_t req11 : 3;    // [14:12]
        uint32_t __31_15 : 17; // [31:15]
    } b;
} REG_ARM_AXIDMA_AXIDMA_ZSP_REQ_STAT1_T;

// axidma_ch_irq_distr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ch0_irq_en0 : 1;  // [0]
        uint32_t ch1_irq_en0 : 1;  // [1]
        uint32_t ch2_irq_en0 : 1;  // [2]
        uint32_t ch3_irq_en0 : 1;  // [3]
        uint32_t ch4_irq_en0 : 1;  // [4]
        uint32_t ch5_irq_en0 : 1;  // [5]
        uint32_t ch6_irq_en0 : 1;  // [6]
        uint32_t ch7_irq_en0 : 1;  // [7]
        uint32_t ch8_irq_en0 : 1;  // [8]
        uint32_t ch9_irq_en0 : 1;  // [9]
        uint32_t ch10_irq_en0 : 1; // [10]
        uint32_t ch11_irq_en0 : 1; // [11]
        uint32_t __31_12 : 20;     // [31:12]
    } b;
} REG_ARM_AXIDMA_AXIDMA_CH_IRQ_DISTR_T;

// axidma_c0_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C0_CONF_T;

// axidma_c0_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C0_MAP_T;

// axidma_c0_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C0_COUNT_T;

// axidma_c0_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C0_COUNTP_T;

// axidma_c0_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C0_STATUS_T;

// axidma_c0_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C0_SGCONF_T;

// axidma_c0_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C0_SET_T;

// axidma_c0_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C0_CLR_T;

// axidma_c1_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C1_CONF_T;

// axidma_c1_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C1_MAP_T;

// axidma_c1_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C1_COUNT_T;

// axidma_c1_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C1_COUNTP_T;

// axidma_c1_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C1_STATUS_T;

// axidma_c1_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C1_SGCONF_T;

// axidma_c1_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C1_SET_T;

// axidma_c1_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C1_CLR_T;

// axidma_c2_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C2_CONF_T;

// axidma_c2_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C2_MAP_T;

// axidma_c2_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C2_COUNT_T;

// axidma_c2_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C2_COUNTP_T;

// axidma_c2_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C2_STATUS_T;

// axidma_c2_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C2_SGCONF_T;

// axidma_c2_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C2_SET_T;

// axidma_c2_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C2_CLR_T;

// axidma_c3_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C3_CONF_T;

// axidma_c3_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C3_MAP_T;

// axidma_c3_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C3_COUNT_T;

// axidma_c3_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C3_COUNTP_T;

// axidma_c3_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C3_STATUS_T;

// axidma_c3_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C3_SGCONF_T;

// axidma_c3_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C3_SET_T;

// axidma_c3_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C3_CLR_T;

// axidma_c4_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C4_CONF_T;

// axidma_c4_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C4_MAP_T;

// axidma_c4_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C4_COUNT_T;

// axidma_c4_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C4_COUNTP_T;

// axidma_c4_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C4_STATUS_T;

// axidma_c4_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C4_SGCONF_T;

// axidma_c4_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C4_SET_T;

// axidma_c4_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C4_CLR_T;

// axidma_c5_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C5_CONF_T;

// axidma_c5_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C5_MAP_T;

// axidma_c5_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C5_COUNT_T;

// axidma_c5_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C5_COUNTP_T;

// axidma_c5_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C5_STATUS_T;

// axidma_c5_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C5_SGCONF_T;

// axidma_c5_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C5_SET_T;

// axidma_c5_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C5_CLR_T;

// axidma_c6_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C6_CONF_T;

// axidma_c6_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C6_MAP_T;

// axidma_c6_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C6_COUNT_T;

// axidma_c6_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C6_COUNTP_T;

// axidma_c6_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C6_STATUS_T;

// axidma_c6_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C6_SGCONF_T;

// axidma_c6_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C6_SET_T;

// axidma_c6_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C6_CLR_T;

// axidma_c7_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C7_CONF_T;

// axidma_c7_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C7_MAP_T;

// axidma_c7_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C7_COUNT_T;

// axidma_c7_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C7_COUNTP_T;

// axidma_c7_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C7_STATUS_T;

// axidma_c7_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C7_SGCONF_T;

// axidma_c7_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C7_SET_T;

// axidma_c7_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C7_CLR_T;

// axidma_c8_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C8_CONF_T;

// axidma_c8_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C8_MAP_T;

// axidma_c8_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C8_COUNT_T;

// axidma_c8_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C8_COUNTP_T;

// axidma_c8_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C8_STATUS_T;

// axidma_c8_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C8_SGCONF_T;

// axidma_c8_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C8_SET_T;

// axidma_c8_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C8_CLR_T;

// axidma_c9_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C9_CONF_T;

// axidma_c9_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C9_MAP_T;

// axidma_c9_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C9_COUNT_T;

// axidma_c9_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C9_COUNTP_T;

// axidma_c9_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C9_STATUS_T;

// axidma_c9_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C9_SGCONF_T;

// axidma_c9_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C9_SET_T;

// axidma_c9_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C9_CLR_T;

// axidma_c10_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C10_CONF_T;

// axidma_c10_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C10_MAP_T;

// axidma_c10_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C10_COUNT_T;

// axidma_c10_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C10_COUNTP_T;

// axidma_c10_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C10_STATUS_T;

// axidma_c10_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C10_SGCONF_T;

// axidma_c10_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C10_SET_T;

// axidma_c10_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C10_CLR_T;

// axidma_c11_conf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t start : 1;            // [0]
        uint32_t data_type : 2;        // [2:1]
        uint32_t syn_irq : 1;          // [3]
        uint32_t irq_f : 1;            // [4]
        uint32_t irq_t : 1;            // [5]
        uint32_t saddr_fix : 1;        // [6]
        uint32_t daddr_fix : 1;        // [7]
        uint32_t force_trans : 1;      // [8]
        uint32_t req_sel : 1;          // [9]
        uint32_t count_sel : 1;        // [10]
        uint32_t zsp_dma_ack_en : 1;   // [11]
        uint32_t saddr_turnaround : 1; // [12]
        uint32_t daddr_turnaround : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C11_CONF_T;

// axidma_c11_map
typedef union {
    uint32_t v;
    struct
    {
        uint32_t req_source : 5; // [4:0]
        uint32_t __7_5 : 3;      // [7:5]
        uint32_t ack_map : 5;    // [12:8]
        uint32_t __31_13 : 19;   // [31:13]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C11_MAP_T;

// axidma_c11_count
typedef union {
    uint32_t v;
    struct
    {
        uint32_t count : 24;  // [23:0]
        uint32_t __31_24 : 8; // [31:24]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C11_COUNT_T;

// axidma_c11_countp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t countp : 16;  // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C11_COUNTP_T;

// axidma_c11_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run : 1;               // [0], write clear
        uint32_t count_finish : 1;      // [1], write clear
        uint32_t countp_finish : 1;     // [2], write clear
        uint32_t sg_finish : 1;         // [3], write clear
        uint32_t sg_count : 16;         // [19:4], write clear
        uint32_t sg_suspend : 1;        // [20], write clear
        uint32_t count_finish_sta : 1;  // [21], write clear
        uint32_t countp_finish_sta : 1; // [22], write clear
        uint32_t sg_finish_sta : 1;     // [23], write clear
        uint32_t sg_suspend_sta : 1;    // [24], write clear
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C11_STATUS_T;

// axidma_c11_sgconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sg_en : 1;         // [0], write clear
        uint32_t sg_finish_ie : 1;  // [1]
        uint32_t sg_suspend_ie : 1; // [2]
        uint32_t desc_rd_ctrl : 1;  // [3]
        uint32_t sg_num : 16;       // [19:4]
        uint32_t __31_20 : 12;      // [31:20]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C11_SGCONF_T;

// axidma_c11_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_set : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C11_SET_T;

// axidma_c11_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t run_clr : 1; // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_ARM_AXIDMA_AXIDMA_C11_CLR_T;

// axidma_conf
#define ARM_AXIDMA_STOP (1 << 0)
#define ARM_AXIDMA_STOP_IE (1 << 1)
#define ARM_AXIDMA_PRIORITY (1 << 2)
#define ARM_AXIDMA_OUTSTAND(n) (((n)&0x3) << 3)
#define ARM_AXIDMA_MODE_SEL (1 << 5)

// axidma_delay
#define ARM_AXIDMA_DELAY(n) (((n)&0xffff) << 0)

// axidma_status
#define ARM_AXIDMA_CH_NUM(n) (((n)&0xf) << 0)
#define ARM_AXIDMA_STOP_STATUS (1 << 4)

// axidma_irq_stat
#define ARM_AXIDMA_CH0_IRQ (1 << 0)
#define ARM_AXIDMA_CH1_IRQ (1 << 1)
#define ARM_AXIDMA_CH2_IRQ (1 << 2)
#define ARM_AXIDMA_CH3_IRQ (1 << 3)
#define ARM_AXIDMA_CH4_IRQ (1 << 4)
#define ARM_AXIDMA_CH5_IRQ (1 << 5)
#define ARM_AXIDMA_CH6_IRQ (1 << 6)
#define ARM_AXIDMA_CH7_IRQ (1 << 7)
#define ARM_AXIDMA_CH8_IRQ (1 << 8)
#define ARM_AXIDMA_CH9_IRQ (1 << 9)
#define ARM_AXIDMA_CH10_IRQ (1 << 10)
#define ARM_AXIDMA_CH11_IRQ (1 << 11)

// axidma_arm_req_stat
#define ARM_AXIDMA_IRQ0 (1 << 0)
#define ARM_AXIDMA_IRQ1 (1 << 1)
#define ARM_AXIDMA_IRQ2 (1 << 2)
#define ARM_AXIDMA_IRQ3 (1 << 3)
#define ARM_AXIDMA_IRQ4 (1 << 4)
#define ARM_AXIDMA_IRQ5 (1 << 5)
#define ARM_AXIDMA_IRQ6 (1 << 6)
#define ARM_AXIDMA_IRQ7 (1 << 7)
#define ARM_AXIDMA_IRQ8 (1 << 8)
#define ARM_AXIDMA_IRQ9 (1 << 9)
#define ARM_AXIDMA_IRQ10 (1 << 10)
#define ARM_AXIDMA_IRQ11 (1 << 11)
#define ARM_AXIDMA_IRQ12 (1 << 12)
#define ARM_AXIDMA_IRQ13 (1 << 13)
#define ARM_AXIDMA_IRQ14 (1 << 14)
#define ARM_AXIDMA_IRQ15 (1 << 15)
#define ARM_AXIDMA_IRQ16 (1 << 16)
#define ARM_AXIDMA_IRQ17 (1 << 17)
#define ARM_AXIDMA_IRQ18 (1 << 18)
#define ARM_AXIDMA_IRQ19 (1 << 19)
#define ARM_AXIDMA_IRQ20 (1 << 20)
#define ARM_AXIDMA_IRQ21 (1 << 21)
#define ARM_AXIDMA_IRQ22 (1 << 22)
#define ARM_AXIDMA_IRQ23 (1 << 23)

// axidma_arm_ack_stat
#define ARM_AXIDMA_ACK0 (1 << 0)
#define ARM_AXIDMA_ACK1 (1 << 1)
#define ARM_AXIDMA_ACK2 (1 << 2)
#define ARM_AXIDMA_ACK3 (1 << 3)
#define ARM_AXIDMA_ACK4 (1 << 4)
#define ARM_AXIDMA_ACK5 (1 << 5)
#define ARM_AXIDMA_ACK6 (1 << 6)
#define ARM_AXIDMA_ACK7 (1 << 7)
#define ARM_AXIDMA_ACK8 (1 << 8)
#define ARM_AXIDMA_ACK9 (1 << 9)
#define ARM_AXIDMA_ACK10 (1 << 10)
#define ARM_AXIDMA_ACK11 (1 << 11)
#define ARM_AXIDMA_ACK12 (1 << 12)
#define ARM_AXIDMA_ACK13 (1 << 13)
#define ARM_AXIDMA_ACK14 (1 << 14)
#define ARM_AXIDMA_ACK15 (1 << 15)
#define ARM_AXIDMA_ACK16 (1 << 16)
#define ARM_AXIDMA_ACK17 (1 << 17)
#define ARM_AXIDMA_ACK18 (1 << 18)
#define ARM_AXIDMA_ACK19 (1 << 19)
#define ARM_AXIDMA_ACK20 (1 << 20)
#define ARM_AXIDMA_ACK21 (1 << 21)
#define ARM_AXIDMA_ACK22 (1 << 22)
#define ARM_AXIDMA_ACK23 (1 << 23)

// axidma_zsp_req_stat0
#define ARM_AXIDMA_REQ0(n) (((n)&0x7) << 0)
#define ARM_AXIDMA_REQ1(n) (((n)&0x7) << 4)
#define ARM_AXIDMA_REQ2(n) (((n)&0x7) << 8)
#define ARM_AXIDMA_REQ3(n) (((n)&0x7) << 12)
#define ARM_AXIDMA_REQ4(n) (((n)&0x7) << 16)
#define ARM_AXIDMA_REQ5(n) (((n)&0x7) << 20)
#define ARM_AXIDMA_REQ6(n) (((n)&0x7) << 24)
#define ARM_AXIDMA_REQ7(n) (((n)&0x7) << 28)

// axidma_zsp_req_stat1
#define ARM_AXIDMA_REQ8(n) (((n)&0x7) << 0)
#define ARM_AXIDMA_REQ9(n) (((n)&0x7) << 4)
#define ARM_AXIDMA_REQ10(n) (((n)&0x7) << 8)
#define ARM_AXIDMA_REQ11(n) (((n)&0x7) << 12)

// axidma_ch_irq_distr
#define ARM_AXIDMA_CH0_IRQ_EN0 (1 << 0)
#define ARM_AXIDMA_CH1_IRQ_EN0 (1 << 1)
#define ARM_AXIDMA_CH2_IRQ_EN0 (1 << 2)
#define ARM_AXIDMA_CH3_IRQ_EN0 (1 << 3)
#define ARM_AXIDMA_CH4_IRQ_EN0 (1 << 4)
#define ARM_AXIDMA_CH5_IRQ_EN0 (1 << 5)
#define ARM_AXIDMA_CH6_IRQ_EN0 (1 << 6)
#define ARM_AXIDMA_CH7_IRQ_EN0 (1 << 7)
#define ARM_AXIDMA_CH8_IRQ_EN0 (1 << 8)
#define ARM_AXIDMA_CH9_IRQ_EN0 (1 << 9)
#define ARM_AXIDMA_CH10_IRQ_EN0 (1 << 10)
#define ARM_AXIDMA_CH11_IRQ_EN0 (1 << 11)

// axidma_c0_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c0_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c0_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c0_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c0_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c0_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c0_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c0_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c0_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c0_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c0_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c1_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c1_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c1_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c1_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c1_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c1_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c1_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c1_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c1_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c1_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c1_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c2_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c2_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c2_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c2_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c2_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c2_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c2_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c2_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c2_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c2_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c2_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c3_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c3_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c3_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c3_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c3_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c3_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c3_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c3_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c3_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c3_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c3_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c4_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c4_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c4_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c4_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c4_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c4_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c4_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c4_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c4_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c4_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c4_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c5_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c5_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c5_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c5_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c5_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c5_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c5_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c5_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c5_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c5_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c5_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c6_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c6_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c6_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c6_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c6_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c6_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c6_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c6_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c6_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c6_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c6_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c7_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c7_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c7_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c7_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c7_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c7_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c7_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c7_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c7_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c7_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c7_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c8_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c8_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c8_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c8_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c8_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c8_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c8_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c8_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c8_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c8_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c8_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c9_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c9_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c9_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c9_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c9_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c9_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c9_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c9_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c9_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c9_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c9_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c10_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c10_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c10_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c10_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c10_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c10_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c10_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c10_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c10_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c10_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c10_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

// axidma_c11_conf
#define ARM_AXIDMA_START (1 << 0)
#define ARM_AXIDMA_DATA_TYPE(n) (((n)&0x3) << 1)
#define ARM_AXIDMA_SYN_IRQ (1 << 3)
#define ARM_AXIDMA_IRQ_F (1 << 4)
#define ARM_AXIDMA_IRQ_T (1 << 5)
#define ARM_AXIDMA_SADDR_FIX (1 << 6)
#define ARM_AXIDMA_DADDR_FIX (1 << 7)
#define ARM_AXIDMA_FORCE_TRANS (1 << 8)
#define ARM_AXIDMA_REQ_SEL (1 << 9)
#define ARM_AXIDMA_COUNT_SEL (1 << 10)
#define ARM_AXIDMA_ZSP_DMA_ACK_EN (1 << 11)
#define ARM_AXIDMA_SADDR_TURNAROUND (1 << 12)
#define ARM_AXIDMA_DADDR_TURNAROUND (1 << 13)

// axidma_c11_map
#define ARM_AXIDMA_REQ_SOURCE(n) (((n)&0x1f) << 0)
#define ARM_AXIDMA_ACK_MAP(n) (((n)&0x1f) << 8)

// axidma_c11_saddr
#define ARM_AXIDMA_S_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c11_daddr
#define ARM_AXIDMA_D_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c11_count
#define ARM_AXIDMA_COUNT(n) (((n)&0xffffff) << 0)

// axidma_c11_countp
#define ARM_AXIDMA_COUNTP(n) (((n)&0xffff) << 0)

// axidma_c11_status
#define ARM_AXIDMA_RUN (1 << 0)
#define ARM_AXIDMA_COUNT_FINISH (1 << 1)
#define ARM_AXIDMA_COUNTP_FINISH (1 << 2)
#define ARM_AXIDMA_SG_FINISH (1 << 3)
#define ARM_AXIDMA_SG_COUNT(n) (((n)&0xffff) << 4)
#define ARM_AXIDMA_SG_SUSPEND (1 << 20)
#define ARM_AXIDMA_COUNT_FINISH_STA (1 << 21)
#define ARM_AXIDMA_COUNTP_FINISH_STA (1 << 22)
#define ARM_AXIDMA_SG_FINISH_STA (1 << 23)
#define ARM_AXIDMA_SG_SUSPEND_STA (1 << 24)

// axidma_c11_sgaddr
#define ARM_AXIDMA_SG_ADDR(n) (((n)&0xffffffff) << 0)

// axidma_c11_sgconf
#define ARM_AXIDMA_SG_EN (1 << 0)
#define ARM_AXIDMA_SG_FINISH_IE (1 << 1)
#define ARM_AXIDMA_SG_SUSPEND_IE (1 << 2)
#define ARM_AXIDMA_DESC_RD_CTRL (1 << 3)
#define ARM_AXIDMA_SG_NUM(n) (((n)&0xffff) << 4)

// axidma_c11_set
#define ARM_AXIDMA_RUN_SET (1 << 0)

// axidma_c11_clr
#define ARM_AXIDMA_RUN_CLR (1 << 0)

#endif // _ARM_AXIDMA_H_
