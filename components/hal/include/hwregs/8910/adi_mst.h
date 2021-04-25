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

#ifndef _ADI_MST_H_
#define _ADI_MST_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_ADI_MST_BASE (0x05300000)
#else
#define REG_ADI_MST_BASE (0x50300000)
#endif

typedef volatile struct
{
    uint32_t adi_version;    // 0x00000000
    uint32_t adi_ctrl;       // 0x00000004
    uint32_t adi_pril;       // 0x00000008
    uint32_t __12[1];        // 0x0000000c
    uint32_t adi_int_en;     // 0x00000010
    uint32_t adi_int_raw;    // 0x00000014
    uint32_t adi_int_status; // 0x00000018
    uint32_t adi_int_clear;  // 0x0000001c
    uint32_t adi_cfg0;       // 0x00000020
    uint32_t adi_cfg1;       // 0x00000024
    uint32_t arm_rd_cmd;     // 0x00000028
    uint32_t arm_rd_data;    // 0x0000002c
    uint32_t arm_cmd_status; // 0x00000030
    uint32_t adi_chanel_en;  // 0x00000034
    uint32_t adi_cmd_wr;     // 0x00000038
    uint32_t adi_dat_wr;     // 0x0000003c
    uint32_t event0_waddr;   // 0x00000040
    uint32_t event1_waddr;   // 0x00000044
    uint32_t event2_waddr;   // 0x00000048
    uint32_t event3_waddr;   // 0x0000004c
    uint32_t event0_wdata;   // 0x00000050
    uint32_t event1_wdata;   // 0x00000054
    uint32_t event2_wdata;   // 0x00000058
    uint32_t event3_wdata;   // 0x0000005c
} HWP_ADI_MST_T;

#define hwp_adiMst ((HWP_ADI_MST_T *)REG_ACCESS_ADDRESS(REG_ADI_MST_BASE))

// adi_version
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adi_version_low : 4;   // [3:0]
        uint32_t adi_version_high : 12; // [15:4], read only
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_ADI_MST_ADI_VERSION_T;

// adi_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t addr_byte_sel : 2; // [1:0]
        uint32_t wr_bit_flag : 1;   // [2]
        uint32_t addr_bits_sel : 2; // [4:3]
        uint32_t wr_cmd_en : 1;     // [5]
        uint32_t __31_6 : 26;       // [31:6]
    } b;
} REG_ADI_MST_ADI_CTRL_T;

// adi_pril
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chnl0_pri : 3;  // [2:0]
        uint32_t chnl1_pri : 3;  // [5:3]
        uint32_t event0_pri : 3; // [8:6]
        uint32_t event1_pri : 3; // [11:9]
        uint32_t event2_pri : 3; // [14:12]
        uint32_t event3_pri : 3; // [17:15]
        uint32_t __31_18 : 14;   // [31:18]
    } b;
} REG_ADI_MST_ADI_PRIL_T;

// adi_int_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wfifo_en : 1;             // [0]
        uint32_t __2_1 : 2;                // [2:1]
        uint32_t fifo_overflow_int_en : 1; // [3]
        uint32_t __31_4 : 28;              // [31:4]
    } b;
} REG_ADI_MST_ADI_INT_EN_T;

// adi_int_raw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __2_0 : 3;             // [2:0]
        uint32_t fifo_overflow_raw : 1; // [3], read only
        uint32_t __31_4 : 28;           // [31:4]
    } b;
} REG_ADI_MST_ADI_INT_RAW_T;

// adi_int_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __2_0 : 3;                // [2:0]
        uint32_t fifo_overflow_status : 1; // [3], read only
        uint32_t __31_4 : 28;              // [31:4]
    } b;
} REG_ADI_MST_ADI_INT_STATUS_T;

// adi_int_clear
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __2_0 : 3;               // [2:0]
        uint32_t fifo_overflow_clear : 1; // [3]
        uint32_t __31_4 : 28;             // [31:4]
    } b;
} REG_ADI_MST_ADI_INT_CLEAR_T;

// adi_cfg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_gssi_frame_len : 6;    // [5:0]
        uint32_t rf_gssi_cmd_len : 5;      // [10:6]
        uint32_t rf_gssi_data_len : 5;     // [15:11]
        uint32_t rf_gssi_wr_pos : 5;       // [20:16]
        uint32_t rf_gssi_wr_pol : 1;       // [21]
        uint32_t rf_gssi_sync_sel : 1;     // [22]
        uint32_t rf_gssi_sync_mode : 1;    // [23]
        uint32_t rf_gssi_sync : 1;         // [24]
        uint32_t rf_gssi_sck_rev : 1;      // [25]
        uint32_t rf_gssi_oe_cfg : 1;       // [26]
        uint32_t rf_gssi_ie_cfg : 1;       // [27]
        uint32_t rf_gssi_dummy_clk_en : 1; // [28]
        uint32_t rf_gssi_fast_mode : 1;    // [29]
        uint32_t rf_gssi_sck_all_on : 1;   // [30]
        uint32_t rf_gssi_wr_disable : 1;   // [31]
    } b;
} REG_ADI_MST_ADI_CFG0_T;

// adi_cfg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rf_gssi_ng_tx : 1;         // [0]
        uint32_t rf_gssi_ng_rx : 1;         // [1]
        uint32_t rf_gssi_clk_div : 8;       // [9:2]
        uint32_t rf_gssi_sync_head_len : 3; // [12:10]
        uint32_t rf_gssi_sync_end_len : 3;  // [15:13]
        uint32_t rf_gssi_dummy_len : 4;     // [19:16]
        uint32_t rf_gssi_sample_delay : 1;  // [20]
        uint32_t rf_gssi_scc_len : 3;       // [23:21]
        uint32_t rf_gssi_wbp_len : 3;       // [26:24]
        uint32_t __27_27 : 1;               // [27]
        uint32_t rf_gssi_rbp_len : 3;       // [30:28]
        uint32_t rf_gssi_strtbit_mode : 1;  // [31]
    } b;
} REG_ADI_MST_ADI_CFG1_T;

// arm_rd_cmd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t arm_rd_cmd : 17; // [16:0]
        uint32_t __31_17 : 15;    // [31:17]
    } b;
} REG_ADI_MST_ARM_RD_CMD_T;

// arm_rd_data
typedef union {
    uint32_t v;
    struct
    {
        uint32_t arm_rd_cmd : 16;     // [15:0], read only
        uint32_t arm_rd_addr : 15;    // [30:16], read only
        uint32_t arm_rd_cmd_busy : 1; // [31], read only
    } b;
} REG_ADI_MST_ARM_RD_DATA_T;

// arm_cmd_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t arm_wr_status : 1;         // [0], read only
        uint32_t arm_rd_status : 1;         // [1], read only
        uint32_t __3_2 : 2;                 // [3:2]
        uint32_t adi_busy : 1;              // [4], read only
        uint32_t __7_5 : 3;                 // [7:5]
        uint32_t wfifo_full : 1;            // [8], read only
        uint32_t wfifo_empty : 1;           // [9], read only
        uint32_t __11_10 : 2;               // [11:10]
        uint32_t wfifo_fill_data_level : 3; // [14:12], read only
        uint32_t __15_15 : 1;               // [15]
        uint32_t adi_fsm_status : 4;        // [19:16], read only
        uint32_t event0_wr_status : 1;      // [20], read only
        uint32_t event1_wr_status : 1;      // [21], read only
        uint32_t event2_wr_status : 1;      // [22], read only
        uint32_t event3_wr_status : 1;      // [23], read only
        uint32_t __31_24 : 8;               // [31:24]
    } b;
} REG_ADI_MST_ARM_CMD_STATUS_T;

// adi_chanel_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t event0_trigger_negedge_en : 1; // [0]
        uint32_t event0_trigger_posedge_en : 1; // [1]
        uint32_t event1_trigger_negedge_en : 1; // [2]
        uint32_t event1_trigger_posedge_en : 1; // [3]
        uint32_t event2_trigger_negedge_en : 1; // [4]
        uint32_t event2_trigger_posedge_en : 1; // [5]
        uint32_t event3_trigger_negedge_en : 1; // [6]
        uint32_t event3_trigger_posedge_en : 1; // [7]
        uint32_t __31_8 : 24;                   // [31:8]
    } b;
} REG_ADI_MST_ADI_CHANEL_EN_T;

// adi_cmd_wr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adi_cmd_wr : 17; // [16:0]
        uint32_t __31_17 : 15;    // [31:17]
    } b;
} REG_ADI_MST_ADI_CMD_WR_T;

// adi_dat_wr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adi_dat_wr : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_ADI_MST_ADI_DAT_WR_T;

// event0_waddr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t event0_waddr : 17; // [16:0]
        uint32_t __31_17 : 15;      // [31:17]
    } b;
} REG_ADI_MST_EVENT0_WADDR_T;

// event1_waddr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t event1_waddr : 17; // [16:0]
        uint32_t __31_17 : 15;      // [31:17]
    } b;
} REG_ADI_MST_EVENT1_WADDR_T;

// event2_waddr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t event2_waddr : 17; // [16:0]
        uint32_t __31_17 : 15;      // [31:17]
    } b;
} REG_ADI_MST_EVENT2_WADDR_T;

// event3_waddr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t event3_waddr : 17; // [16:0]
        uint32_t __31_17 : 15;      // [31:17]
    } b;
} REG_ADI_MST_EVENT3_WADDR_T;

// event0_wdata
typedef union {
    uint32_t v;
    struct
    {
        uint32_t event0_neg_wdata : 16; // [15:0]
        uint32_t event0_pos_wdata : 16; // [31:16]
    } b;
} REG_ADI_MST_EVENT0_WDATA_T;

// event1_wdata
typedef union {
    uint32_t v;
    struct
    {
        uint32_t event1_neg_wdata : 16; // [15:0]
        uint32_t event1_pos_wdata : 16; // [31:16]
    } b;
} REG_ADI_MST_EVENT1_WDATA_T;

// event2_wdata
typedef union {
    uint32_t v;
    struct
    {
        uint32_t event2_neg_wdata : 16; // [15:0]
        uint32_t event2_pos_wdata : 16; // [31:16]
    } b;
} REG_ADI_MST_EVENT2_WDATA_T;

// event3_wdata
typedef union {
    uint32_t v;
    struct
    {
        uint32_t event3_neg_wdata : 16; // [15:0]
        uint32_t event3_pos_wdata : 16; // [31:16]
    } b;
} REG_ADI_MST_EVENT3_WDATA_T;

// adi_version
#define ADI_MST_ADI_VERSION_LOW(n) (((n)&0xf) << 0)
#define ADI_MST_ADI_VERSION_HIGH(n) (((n)&0xfff) << 4)

// adi_ctrl
#define ADI_MST_ADDR_BYTE_SEL(n) (((n)&0x3) << 0)
#define ADI_MST_WR_BIT_FLAG (1 << 2)
#define ADI_MST_ADDR_BITS_SEL(n) (((n)&0x3) << 3)
#define ADI_MST_WR_CMD_EN (1 << 5)

// adi_pril
#define ADI_MST_CHNL0_PRI(n) (((n)&0x7) << 0)
#define ADI_MST_CHNL1_PRI(n) (((n)&0x7) << 3)
#define ADI_MST_EVENT0_PRI(n) (((n)&0x7) << 6)
#define ADI_MST_EVENT1_PRI(n) (((n)&0x7) << 9)
#define ADI_MST_EVENT2_PRI(n) (((n)&0x7) << 12)
#define ADI_MST_EVENT3_PRI(n) (((n)&0x7) << 15)

// adi_int_en
#define ADI_MST_WFIFO_EN (1 << 0)
#define ADI_MST_FIFO_OVERFLOW_INT_EN (1 << 3)

// adi_int_raw
#define ADI_MST_FIFO_OVERFLOW_RAW (1 << 3)

// adi_int_status
#define ADI_MST_FIFO_OVERFLOW_STATUS (1 << 3)

// adi_int_clear
#define ADI_MST_FIFO_OVERFLOW_CLEAR (1 << 3)

// adi_cfg0
#define ADI_MST_RF_GSSI_FRAME_LEN(n) (((n)&0x3f) << 0)
#define ADI_MST_RF_GSSI_CMD_LEN(n) (((n)&0x1f) << 6)
#define ADI_MST_RF_GSSI_DATA_LEN(n) (((n)&0x1f) << 11)
#define ADI_MST_RF_GSSI_WR_POS(n) (((n)&0x1f) << 16)
#define ADI_MST_RF_GSSI_WR_POL (1 << 21)
#define ADI_MST_RF_GSSI_SYNC_SEL (1 << 22)
#define ADI_MST_RF_GSSI_SYNC_MODE (1 << 23)
#define ADI_MST_RF_GSSI_SYNC (1 << 24)
#define ADI_MST_RF_GSSI_SCK_REV (1 << 25)
#define ADI_MST_RF_GSSI_OE_CFG (1 << 26)
#define ADI_MST_RF_GSSI_IE_CFG (1 << 27)
#define ADI_MST_RF_GSSI_DUMMY_CLK_EN (1 << 28)
#define ADI_MST_RF_GSSI_FAST_MODE (1 << 29)
#define ADI_MST_RF_GSSI_SCK_ALL_ON (1 << 30)
#define ADI_MST_RF_GSSI_WR_DISABLE (1 << 31)

// adi_cfg1
#define ADI_MST_RF_GSSI_NG_TX (1 << 0)
#define ADI_MST_RF_GSSI_NG_RX (1 << 1)
#define ADI_MST_RF_GSSI_CLK_DIV(n) (((n)&0xff) << 2)
#define ADI_MST_RF_GSSI_SYNC_HEAD_LEN(n) (((n)&0x7) << 10)
#define ADI_MST_RF_GSSI_SYNC_END_LEN(n) (((n)&0x7) << 13)
#define ADI_MST_RF_GSSI_DUMMY_LEN(n) (((n)&0xf) << 16)
#define ADI_MST_RF_GSSI_SAMPLE_DELAY (1 << 20)
#define ADI_MST_RF_GSSI_SCC_LEN(n) (((n)&0x7) << 21)
#define ADI_MST_RF_GSSI_WBP_LEN(n) (((n)&0x7) << 24)
#define ADI_MST_RF_GSSI_RBP_LEN(n) (((n)&0x7) << 28)
#define ADI_MST_RF_GSSI_STRTBIT_MODE (1 << 31)

// arm_rd_cmd
#define ADI_MST_ARM_RD_CMD_ARM_RD_CMD(n) (((n)&0x1ffff) << 0)

// arm_rd_data
#define ADI_MST_ARM_RD_DATA_ARM_RD_CMD(n) (((n)&0xffff) << 0)
#define ADI_MST_ARM_RD_ADDR(n) (((n)&0x7fff) << 16)
#define ADI_MST_ARM_RD_CMD_BUSY (1 << 31)

// arm_cmd_status
#define ADI_MST_ARM_WR_STATUS (1 << 0)
#define ADI_MST_ARM_RD_STATUS (1 << 1)
#define ADI_MST_ADI_BUSY (1 << 4)
#define ADI_MST_WFIFO_FULL (1 << 8)
#define ADI_MST_WFIFO_EMPTY (1 << 9)
#define ADI_MST_WFIFO_FILL_DATA_LEVEL(n) (((n)&0x7) << 12)
#define ADI_MST_ADI_FSM_STATUS(n) (((n)&0xf) << 16)
#define ADI_MST_EVENT0_WR_STATUS (1 << 20)
#define ADI_MST_EVENT1_WR_STATUS (1 << 21)
#define ADI_MST_EVENT2_WR_STATUS (1 << 22)
#define ADI_MST_EVENT3_WR_STATUS (1 << 23)

// adi_chanel_en
#define ADI_MST_EVENT0_TRIGGER_NEGEDGE_EN (1 << 0)
#define ADI_MST_EVENT0_TRIGGER_POSEDGE_EN (1 << 1)
#define ADI_MST_EVENT1_TRIGGER_NEGEDGE_EN (1 << 2)
#define ADI_MST_EVENT1_TRIGGER_POSEDGE_EN (1 << 3)
#define ADI_MST_EVENT2_TRIGGER_NEGEDGE_EN (1 << 4)
#define ADI_MST_EVENT2_TRIGGER_POSEDGE_EN (1 << 5)
#define ADI_MST_EVENT3_TRIGGER_NEGEDGE_EN (1 << 6)
#define ADI_MST_EVENT3_TRIGGER_POSEDGE_EN (1 << 7)

// adi_cmd_wr
#define ADI_MST_ADI_CMD_WR(n) (((n)&0x1ffff) << 0)

// adi_dat_wr
#define ADI_MST_ADI_DAT_WR(n) (((n)&0xffff) << 0)

// event0_waddr
#define ADI_MST_EVENT0_WADDR(n) (((n)&0x1ffff) << 0)

// event1_waddr
#define ADI_MST_EVENT1_WADDR(n) (((n)&0x1ffff) << 0)

// event2_waddr
#define ADI_MST_EVENT2_WADDR(n) (((n)&0x1ffff) << 0)

// event3_waddr
#define ADI_MST_EVENT3_WADDR(n) (((n)&0x1ffff) << 0)

// event0_wdata
#define ADI_MST_EVENT0_NEG_WDATA(n) (((n)&0xffff) << 0)
#define ADI_MST_EVENT0_POS_WDATA(n) (((n)&0xffff) << 16)

// event1_wdata
#define ADI_MST_EVENT1_NEG_WDATA(n) (((n)&0xffff) << 0)
#define ADI_MST_EVENT1_POS_WDATA(n) (((n)&0xffff) << 16)

// event2_wdata
#define ADI_MST_EVENT2_NEG_WDATA(n) (((n)&0xffff) << 0)
#define ADI_MST_EVENT2_POS_WDATA(n) (((n)&0xffff) << 16)

// event3_wdata
#define ADI_MST_EVENT3_NEG_WDATA(n) (((n)&0xffff) << 0)
#define ADI_MST_EVENT3_POS_WDATA(n) (((n)&0xffff) << 16)

#endif // _ADI_MST_H_
