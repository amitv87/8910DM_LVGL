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

#ifndef _RDA2720M_EFS_H_
#define _RDA2720M_EFS_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_EFS_BASE (0x50308300)

typedef volatile struct
{
    uint32_t efuse_glb_ctrl;           // 0x00000000
    uint32_t efuse_data_rd;            // 0x00000004
    uint32_t efuse_data_wr;            // 0x00000008
    uint32_t efuse_addr_index;         // 0x0000000c
    uint32_t efuse_mode_ctrl;          // 0x00000010
    uint32_t efuse_status;             // 0x00000014
    uint32_t efuse_magic_number;       // 0x00000018
    uint32_t efuse_magin_magic_number; // 0x0000001c
    uint32_t efuse_wr_timing_ctrl;     // 0x00000020
    uint32_t efuse_rd_timing_ctrl;     // 0x00000024
    uint32_t efuse_version;            // 0x00000028
} HWP_RDA2720M_EFS_T;

#define hwp_rda2720mEfs ((HWP_RDA2720M_EFS_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_EFS_BASE))

// efuse_glb_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_pgm_enable : 1; // [0]
        uint32_t efuse_type : 2;       // [2:1]
        uint32_t margin_mode : 1;      // [3]
        uint32_t __31_4 : 28;          // [31:4]
    } b;
} REG_RDA2720M_EFS_EFUSE_GLB_CTRL_T;

// efuse_data_rd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_data_rd : 16; // [15:0], read only
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EFS_EFUSE_DATA_RD_T;

// efuse_data_wr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_data_wr : 16; // [15:0]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EFS_EFUSE_DATA_WR_T;

// efuse_addr_index
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_addr_index : 5; // [4:0]
        uint32_t __31_5 : 27;          // [31:5]
    } b;
} REG_RDA2720M_EFS_EFUSE_ADDR_INDEX_T;

// efuse_mode_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_pg_start : 1;           // [0]
        uint32_t efuse_rd_start : 1;           // [1]
        uint32_t efuse_normal_rd_flag_clr : 1; // [2]
        uint32_t __31_3 : 29;                  // [31:3]
    } b;
} REG_RDA2720M_EFS_EFUSE_MODE_CTRL_T;

// efuse_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pgm_busy : 1;             // [0], read only
        uint32_t read_busy : 1;            // [1], read only
        uint32_t standby_busy : 1;         // [2], read only
        uint32_t efuse_global_prot : 1;    // [3], read only
        uint32_t efuse_normal_rd_done : 1; // [4], read only
        uint32_t __31_5 : 27;              // [31:5]
    } b;
} REG_RDA2720M_EFS_EFUSE_STATUS_T;

// efuse_magic_number
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_magin_magic_number : 16; // [15:0]
        uint32_t __31_16 : 16;                  // [31:16]
    } b;
} REG_RDA2720M_EFS_EFUSE_MAGIC_NUMBER_T;

// efuse_magin_magic_number
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_magin_magic_number : 16; // [15:0]
        uint32_t __31_16 : 16;                  // [31:16]
    } b;
} REG_RDA2720M_EFS_EFUSE_MAGIN_MAGIC_NUMBER_T;

// efuse_wr_timing_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_wr_timing_ctrl : 14; // [13:0], read only
        uint32_t __31_14 : 18;              // [31:14]
    } b;
} REG_RDA2720M_EFS_EFUSE_WR_TIMING_CTRL_T;

// efuse_rd_timing_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_rd_timing_ctrl : 10; // [9:0]
        uint32_t __31_10 : 22;              // [31:10]
    } b;
} REG_RDA2720M_EFS_EFUSE_RD_TIMING_CTRL_T;

// efuse_version
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_version : 16; // [15:0]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_RDA2720M_EFS_EFUSE_VERSION_T;

// efuse_glb_ctrl
#define RDA2720M_EFS_EFUSE_PGM_ENABLE (1 << 0)
#define RDA2720M_EFS_EFUSE_TYPE(n) (((n)&0x3) << 1)
#define RDA2720M_EFS_MARGIN_MODE (1 << 3)

// efuse_data_rd
#define RDA2720M_EFS_EFUSE_DATA_RD(n) (((n)&0xffff) << 0)

// efuse_data_wr
#define RDA2720M_EFS_EFUSE_DATA_WR(n) (((n)&0xffff) << 0)

// efuse_addr_index
#define RDA2720M_EFS_EFUSE_ADDR_INDEX(n) (((n)&0x1f) << 0)

// efuse_mode_ctrl
#define RDA2720M_EFS_EFUSE_PG_START (1 << 0)
#define RDA2720M_EFS_EFUSE_RD_START (1 << 1)
#define RDA2720M_EFS_EFUSE_NORMAL_RD_FLAG_CLR (1 << 2)

// efuse_status
#define RDA2720M_EFS_PGM_BUSY (1 << 0)
#define RDA2720M_EFS_READ_BUSY (1 << 1)
#define RDA2720M_EFS_STANDBY_BUSY (1 << 2)
#define RDA2720M_EFS_EFUSE_GLOBAL_PROT (1 << 3)
#define RDA2720M_EFS_EFUSE_NORMAL_RD_DONE (1 << 4)

// efuse_magic_number
#define RDA2720M_EFS_EFUSE_MAGIN_MAGIC_NUMBER(n) (((n)&0xffff) << 0)

// efuse_magin_magic_number
#define RDA2720M_EFS_EFUSE_MAGIN_MAGIC_NUMBER(n) (((n)&0xffff) << 0)

// efuse_wr_timing_ctrl
#define RDA2720M_EFS_EFUSE_WR_TIMING_CTRL(n) (((n)&0x3fff) << 0)

// efuse_rd_timing_ctrl
#define RDA2720M_EFS_EFUSE_RD_TIMING_CTRL(n) (((n)&0x3ff) << 0)

// efuse_version
#define RDA2720M_EFS_EFUSE_VERSION(n) (((n)&0xffff) << 0)

#endif // _RDA2720M_EFS_H_
