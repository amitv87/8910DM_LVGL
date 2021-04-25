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

#ifndef _WCN_WLAN_H_
#define _WCN_WLAN_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_WCN_WLAN_BASE (0x15007000)

typedef volatile struct
{
    uint32_t __0[1];              // 0x00000000
    uint32_t phy_reg_bank_addr;   // 0x00000004
    uint32_t phy_reg_offset_addr; // 0x00000008
    uint32_t phy_reg_write_data;  // 0x0000000c
    uint32_t phy_reg_read_data;   // 0x00000010
    uint32_t protocol_version;    // 0x00000014
    uint32_t type;                // 0x00000018
    uint32_t sub_type;            // 0x0000001c
    uint32_t desaddr_l;           // 0x00000020
    uint32_t desaddr_h;           // 0x00000024
    uint32_t srcaddr_l;           // 0x00000028
    uint32_t srcaddr_h;           // 0x0000002c
    uint32_t bssidaddr_l;         // 0x00000030
    uint32_t bssidaddr_h;         // 0x00000034
    uint32_t seqcontrol;          // 0x00000038
    uint32_t rssival;             // 0x0000003c
    uint32_t __64[16];            // 0x00000040
    uint32_t config_reg;          // 0x00000080
    uint32_t datardyint;          // 0x00000084
    uint32_t phy_sel_set0;        // 0x00000088
    uint32_t phy_sel_set2;        // 0x0000008c
    uint32_t phy_sel_set3;        // 0x00000090
    uint32_t phy_sel_set4;        // 0x00000094
    uint32_t phy_sel_set5;        // 0x00000098
    uint32_t phy_sel_set6;        // 0x0000009c
    uint32_t phy_sel_set7;        // 0x000000a0
    uint32_t phy_sel_set8;        // 0x000000a4
} HWP_WCN_WLAN_T;

#define hwp_wcnWlan ((HWP_WCN_WLAN_T *)REG_ACCESS_ADDRESS(REG_WCN_WLAN_BASE))

// protocol_version
typedef union {
    uint32_t v;
    struct
    {
        uint32_t protocol_version : 2; // [1:0], read only
        uint32_t __31_2 : 30;          // [31:2]
    } b;
} REG_WCN_WLAN_PROTOCOL_VERSION_T;

// type
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tp : 2;      // [1:0], read only
        uint32_t __31_2 : 30; // [31:2]
    } b;
} REG_WCN_WLAN_TYPE_T;

// sub_type
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sub_type : 4; // [3:0], read only
        uint32_t __31_4 : 28;  // [31:4]
    } b;
} REG_WCN_WLAN_SUB_TYPE_T;

// desaddr_h
typedef union {
    uint32_t v;
    struct
    {
        uint32_t desaddr_h : 16; // [15:0], read only
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_WCN_WLAN_DESADDR_H_T;

// srcaddr_h
typedef union {
    uint32_t v;
    struct
    {
        uint32_t srcaddr_h : 16; // [15:0], read only
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_WCN_WLAN_SRCADDR_H_T;

// bssidaddr_h
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bssidaddr_h : 16; // [15:0], read only
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_WCN_WLAN_BSSIDADDR_H_T;

// seqcontrol
typedef union {
    uint32_t v;
    struct
    {
        uint32_t seqcontrol : 16; // [15:0], read only
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_WCN_WLAN_SEQCONTROL_T;

// rssival
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rssival : 8; // [7:0], read only
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_WCN_WLAN_RSSIVAL_T;

// config_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t apb_clear : 1;          // [0]
        uint32_t apb_hold : 1;           // [1]
        uint32_t __7_2 : 6;              // [7:2]
        uint32_t subtype_config_reg : 4; // [11:8]
        uint32_t type_config_reg : 2;    // [13:12]
        uint32_t __15_14 : 2;            // [15:14]
        uint32_t phyrx_en_b : 1;         // [16]
        uint32_t __31_17 : 15;           // [31:17]
    } b;
} REG_WCN_WLAN_CONFIG_REG_T;

// datardyint
typedef union {
    uint32_t v;
    struct
    {
        uint32_t datardyint : 1; // [0], read only
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_WCN_WLAN_DATARDYINT_T;

// phy_sel_set0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t phy_sel_0_reg : 6; // [5:0]
        uint32_t __7_6 : 2;         // [7:6]
        uint32_t phy_sel_1_reg : 6; // [13:8]
        uint32_t __15_14 : 2;       // [15:14]
        uint32_t phy_sel_2_reg : 6; // [21:16]
        uint32_t __23_22 : 2;       // [23:22]
        uint32_t phy_sel_3_reg : 6; // [29:24]
        uint32_t __31_30 : 2;       // [31:30]
    } b;
} REG_WCN_WLAN_PHY_SEL_SET0_T;

// phy_sel_set2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t phy_sel_4_reg : 6; // [5:0]
        uint32_t __7_6 : 2;         // [7:6]
        uint32_t phy_sel_5_reg : 6; // [13:8]
        uint32_t __15_14 : 2;       // [15:14]
        uint32_t phy_sel_6_reg : 6; // [21:16]
        uint32_t __23_22 : 2;       // [23:22]
        uint32_t phy_sel_7_reg : 6; // [29:24]
        uint32_t __31_30 : 2;       // [31:30]
    } b;
} REG_WCN_WLAN_PHY_SEL_SET2_T;

// phy_sel_set3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t phy_sel_8_reg : 6;  // [5:0]
        uint32_t __7_6 : 2;          // [7:6]
        uint32_t phy_sel_9_reg : 6;  // [13:8]
        uint32_t __15_14 : 2;        // [15:14]
        uint32_t phy_sel_10_reg : 6; // [21:16]
        uint32_t __23_22 : 2;        // [23:22]
        uint32_t phy_sel_11_reg : 6; // [29:24]
        uint32_t __31_30 : 2;        // [31:30]
    } b;
} REG_WCN_WLAN_PHY_SEL_SET3_T;

// phy_sel_set4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t phy_sel_12_reg : 6; // [5:0]
        uint32_t __7_6 : 2;          // [7:6]
        uint32_t phy_sel_13_reg : 6; // [13:8]
        uint32_t __15_14 : 2;        // [15:14]
        uint32_t phy_sel_14_reg : 6; // [21:16]
        uint32_t __23_22 : 2;        // [23:22]
        uint32_t phy_sel_15_reg : 6; // [29:24]
        uint32_t __31_30 : 2;        // [31:30]
    } b;
} REG_WCN_WLAN_PHY_SEL_SET4_T;

// phy_sel_set5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t phy_sel_16_reg : 6; // [5:0]
        uint32_t __7_6 : 2;          // [7:6]
        uint32_t phy_sel_17_reg : 6; // [13:8]
        uint32_t __15_14 : 2;        // [15:14]
        uint32_t phy_sel_18_reg : 6; // [21:16]
        uint32_t __23_22 : 2;        // [23:22]
        uint32_t phy_sel_19_reg : 6; // [29:24]
        uint32_t __31_30 : 2;        // [31:30]
    } b;
} REG_WCN_WLAN_PHY_SEL_SET5_T;

// phy_sel_set6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t phy_sel_20_reg : 6; // [5:0]
        uint32_t __7_6 : 2;          // [7:6]
        uint32_t phy_sel_21_reg : 6; // [13:8]
        uint32_t __15_14 : 2;        // [15:14]
        uint32_t phy_sel_22_reg : 6; // [21:16]
        uint32_t __23_22 : 2;        // [23:22]
        uint32_t phy_sel_23_reg : 6; // [29:24]
        uint32_t __31_30 : 2;        // [31:30]
    } b;
} REG_WCN_WLAN_PHY_SEL_SET6_T;

// phy_sel_set7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t phy_sel_24_reg : 6; // [5:0]
        uint32_t __7_6 : 2;          // [7:6]
        uint32_t phy_sel_25_reg : 6; // [13:8]
        uint32_t __15_14 : 2;        // [15:14]
        uint32_t phy_sel_26_reg : 6; // [21:16]
        uint32_t __23_22 : 2;        // [23:22]
        uint32_t phy_sel_27_reg : 6; // [29:24]
        uint32_t __31_30 : 2;        // [31:30]
    } b;
} REG_WCN_WLAN_PHY_SEL_SET7_T;

// phy_sel_set8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t phy_sel_28_reg : 6; // [5:0]
        uint32_t __7_6 : 2;          // [7:6]
        uint32_t phy_sel_29_reg : 6; // [13:8]
        uint32_t __15_14 : 2;        // [15:14]
        uint32_t phy_sel_30_reg : 6; // [21:16]
        uint32_t __23_22 : 2;        // [23:22]
        uint32_t phy_sel_31_reg : 6; // [29:24]
        uint32_t __31_30 : 2;        // [31:30]
    } b;
} REG_WCN_WLAN_PHY_SEL_SET8_T;

// phy_reg_bank_addr
#define WCN_WLAN_PHY_REG_BANK_ADDR(n) (((n)&0xffffffff) << 0)

// phy_reg_offset_addr
#define WCN_WLAN_PHY_REG_OFFSET_ADDR(n) (((n)&0xffffffff) << 0)

// phy_reg_write_data
#define WCN_WLAN_PHY_REG_WRITE_DATA(n) (((n)&0xffffffff) << 0)

// phy_reg_read_data
#define WCN_WLAN_PHY_REG_READ_DATA(n) (((n)&0xffffffff) << 0)

// protocol_version
#define WCN_WLAN_PROTOCOL_VERSION(n) (((n)&0x3) << 0)

// type
#define WCN_WLAN_TP(n) (((n)&0x3) << 0)

// sub_type
#define WCN_WLAN_SUB_TYPE(n) (((n)&0xf) << 0)

// desaddr_l
#define WCN_WLAN_DESADDR_L(n) (((n)&0xffffffff) << 0)

// desaddr_h
#define WCN_WLAN_DESADDR_H(n) (((n)&0xffff) << 0)

// srcaddr_l
#define WCN_WLAN_SRCADDR_L(n) (((n)&0xffffffff) << 0)

// srcaddr_h
#define WCN_WLAN_SRCADDR_H(n) (((n)&0xffff) << 0)

// bssidaddr_l
#define WCN_WLAN_BSSIDADDR_L(n) (((n)&0xffffffff) << 0)

// bssidaddr_h
#define WCN_WLAN_BSSIDADDR_H(n) (((n)&0xffff) << 0)

// seqcontrol
#define WCN_WLAN_SEQCONTROL(n) (((n)&0xffff) << 0)

// rssival
#define WCN_WLAN_RSSIVAL(n) (((n)&0xff) << 0)

// config_reg
#define WCN_WLAN_APB_CLEAR (1 << 0)
#define WCN_WLAN_APB_HOLD (1 << 1)
#define WCN_WLAN_SUBTYPE_CONFIG_REG(n) (((n)&0xf) << 8)
#define WCN_WLAN_TYPE_CONFIG_REG(n) (((n)&0x3) << 12)
#define WCN_WLAN_PHYRX_EN_B (1 << 16)

// datardyint
#define WCN_WLAN_DATARDYINT (1 << 0)

// phy_sel_set0
#define WCN_WLAN_PHY_SEL_0_REG(n) (((n)&0x3f) << 0)
#define WCN_WLAN_PHY_SEL_1_REG(n) (((n)&0x3f) << 8)
#define WCN_WLAN_PHY_SEL_2_REG(n) (((n)&0x3f) << 16)
#define WCN_WLAN_PHY_SEL_3_REG(n) (((n)&0x3f) << 24)

// phy_sel_set2
#define WCN_WLAN_PHY_SEL_4_REG(n) (((n)&0x3f) << 0)
#define WCN_WLAN_PHY_SEL_5_REG(n) (((n)&0x3f) << 8)
#define WCN_WLAN_PHY_SEL_6_REG(n) (((n)&0x3f) << 16)
#define WCN_WLAN_PHY_SEL_7_REG(n) (((n)&0x3f) << 24)

// phy_sel_set3
#define WCN_WLAN_PHY_SEL_8_REG(n) (((n)&0x3f) << 0)
#define WCN_WLAN_PHY_SEL_9_REG(n) (((n)&0x3f) << 8)
#define WCN_WLAN_PHY_SEL_10_REG(n) (((n)&0x3f) << 16)
#define WCN_WLAN_PHY_SEL_11_REG(n) (((n)&0x3f) << 24)

// phy_sel_set4
#define WCN_WLAN_PHY_SEL_12_REG(n) (((n)&0x3f) << 0)
#define WCN_WLAN_PHY_SEL_13_REG(n) (((n)&0x3f) << 8)
#define WCN_WLAN_PHY_SEL_14_REG(n) (((n)&0x3f) << 16)
#define WCN_WLAN_PHY_SEL_15_REG(n) (((n)&0x3f) << 24)

// phy_sel_set5
#define WCN_WLAN_PHY_SEL_16_REG(n) (((n)&0x3f) << 0)
#define WCN_WLAN_PHY_SEL_17_REG(n) (((n)&0x3f) << 8)
#define WCN_WLAN_PHY_SEL_18_REG(n) (((n)&0x3f) << 16)
#define WCN_WLAN_PHY_SEL_19_REG(n) (((n)&0x3f) << 24)

// phy_sel_set6
#define WCN_WLAN_PHY_SEL_20_REG(n) (((n)&0x3f) << 0)
#define WCN_WLAN_PHY_SEL_21_REG(n) (((n)&0x3f) << 8)
#define WCN_WLAN_PHY_SEL_22_REG(n) (((n)&0x3f) << 16)
#define WCN_WLAN_PHY_SEL_23_REG(n) (((n)&0x3f) << 24)

// phy_sel_set7
#define WCN_WLAN_PHY_SEL_24_REG(n) (((n)&0x3f) << 0)
#define WCN_WLAN_PHY_SEL_25_REG(n) (((n)&0x3f) << 8)
#define WCN_WLAN_PHY_SEL_26_REG(n) (((n)&0x3f) << 16)
#define WCN_WLAN_PHY_SEL_27_REG(n) (((n)&0x3f) << 24)

// phy_sel_set8
#define WCN_WLAN_PHY_SEL_28_REG(n) (((n)&0x3f) << 0)
#define WCN_WLAN_PHY_SEL_29_REG(n) (((n)&0x3f) << 8)
#define WCN_WLAN_PHY_SEL_30_REG(n) (((n)&0x3f) << 16)
#define WCN_WLAN_PHY_SEL_31_REG(n) (((n)&0x3f) << 24)

#endif // _WCN_WLAN_H_
