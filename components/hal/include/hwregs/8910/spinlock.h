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

#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_SPINLOCK_BASE (0x0510d000)
#else
#define REG_SPINLOCK_BASE (0x5010d000)
#endif

typedef volatile struct
{
    uint32_t chk_id;      // 0x00000000
    uint32_t recctrl;     // 0x00000004
    uint32_t ttlsts;      // 0x00000008
    uint32_t swflag0;     // 0x0000000c
    uint32_t swflag1;     // 0x00000010
    uint32_t swflag2;     // 0x00000014
    uint32_t swflag3;     // 0x00000018
    uint32_t __28[25];    // 0x0000001c
    uint32_t mstid0;      // 0x00000080
    uint32_t mstid1;      // 0x00000084
    uint32_t mstid2;      // 0x00000088
    uint32_t mstid3;      // 0x0000008c
    uint32_t mstid4;      // 0x00000090
    uint32_t mstid5;      // 0x00000094
    uint32_t mstid6;      // 0x00000098
    uint32_t mstid7;      // 0x0000009c
    uint32_t mstid8;      // 0x000000a0
    uint32_t mstid9;      // 0x000000a4
    uint32_t mstid10;     // 0x000000a8
    uint32_t mstid11;     // 0x000000ac
    uint32_t mstid12;     // 0x000000b0
    uint32_t mstid13;     // 0x000000b4
    uint32_t mstid14;     // 0x000000b8
    uint32_t mstid15;     // 0x000000bc
    uint32_t mstid16;     // 0x000000c0
    uint32_t mstid17;     // 0x000000c4
    uint32_t mstid18;     // 0x000000c8
    uint32_t mstid19;     // 0x000000cc
    uint32_t mstid20;     // 0x000000d0
    uint32_t mstid21;     // 0x000000d4
    uint32_t mstid22;     // 0x000000d8
    uint32_t mstid23;     // 0x000000dc
    uint32_t mstid24;     // 0x000000e0
    uint32_t mstid25;     // 0x000000e4
    uint32_t mstid26;     // 0x000000e8
    uint32_t mstid27;     // 0x000000ec
    uint32_t mstid28;     // 0x000000f0
    uint32_t mstid29;     // 0x000000f4
    uint32_t mstid30;     // 0x000000f8
    uint32_t mstid31;     // 0x000000fc
    uint32_t __256[448];  // 0x00000100
    uint32_t locksts0;    // 0x00000800
    uint32_t locksts1;    // 0x00000804
    uint32_t locksts2;    // 0x00000808
    uint32_t locksts3;    // 0x0000080c
    uint32_t locksts4;    // 0x00000810
    uint32_t locksts5;    // 0x00000814
    uint32_t locksts6;    // 0x00000818
    uint32_t locksts7;    // 0x0000081c
    uint32_t locksts8;    // 0x00000820
    uint32_t locksts9;    // 0x00000824
    uint32_t locksts10;   // 0x00000828
    uint32_t locksts11;   // 0x0000082c
    uint32_t locksts12;   // 0x00000830
    uint32_t locksts13;   // 0x00000834
    uint32_t locksts14;   // 0x00000838
    uint32_t locksts15;   // 0x0000083c
    uint32_t locksts16;   // 0x00000840
    uint32_t locksts17;   // 0x00000844
    uint32_t locksts18;   // 0x00000848
    uint32_t locksts19;   // 0x0000084c
    uint32_t locksts20;   // 0x00000850
    uint32_t locksts21;   // 0x00000854
    uint32_t locksts22;   // 0x00000858
    uint32_t locksts23;   // 0x0000085c
    uint32_t locksts24;   // 0x00000860
    uint32_t locksts25;   // 0x00000864
    uint32_t locksts26;   // 0x00000868
    uint32_t locksts27;   // 0x0000086c
    uint32_t locksts28;   // 0x00000870
    uint32_t locksts29;   // 0x00000874
    uint32_t locksts30;   // 0x00000878
    uint32_t locksts31;   // 0x0000087c
    uint32_t __2176[479]; // 0x00000880
    uint32_t verid;       // 0x00000ffc
} HWP_SPINLOCK_T;

#define hwp_spinlock ((HWP_SPINLOCK_T *)REG_ACCESS_ADDRESS(REG_SPINLOCK_BASE))

// recctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rec_ctrl : 1; // [0]
        uint32_t __31_1 : 31;  // [31:1]
    } b;
} REG_SPINLOCK_RECCTRL_T;

// locksts0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts0_locksts0 : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS0_T;

// locksts1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts1_locksts1 : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS1_T;

// locksts2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts2_locksts2 : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS2_T;

// locksts3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts3_locksts3 : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS3_T;

// locksts4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts4_locksts4 : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS4_T;

// locksts5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts5_locksts5 : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS5_T;

// locksts6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts6_locksts6 : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS6_T;

// locksts7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts7_locksts7 : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS7_T;

// locksts8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts8_locksts8 : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS8_T;

// locksts9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts9_locksts9 : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS9_T;

// locksts10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts10_locksts10 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS10_T;

// locksts11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts11_locksts11 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS11_T;

// locksts12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts12_locksts12 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS12_T;

// locksts13
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts13_locksts13 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS13_T;

// locksts14
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts14_locksts14 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS14_T;

// locksts15
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts15_locksts15 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS15_T;

// locksts16
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts16_locksts16 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS16_T;

// locksts17
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts17_locksts17 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS17_T;

// locksts18
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts18_locksts18 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS18_T;

// locksts19
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts19_locksts19 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS19_T;

// locksts20
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts20_locksts20 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS20_T;

// locksts21
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts21_locksts21 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS21_T;

// locksts22
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts22_locksts22 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS22_T;

// locksts23
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts23_locksts23 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS23_T;

// locksts24
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts24_locksts24 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS24_T;

// locksts25
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts25_locksts25 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS25_T;

// locksts26
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts26_locksts26 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS26_T;

// locksts27
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts27_locksts27 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS27_T;

// locksts28
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts28_locksts28 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS28_T;

// locksts29
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts29_locksts29 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS29_T;

// locksts30
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts30_locksts30 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS30_T;

// locksts31
typedef union {
    uint32_t v;
    struct
    {
        uint32_t locksts31_locksts31 : 1; // [0], read only
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_SPINLOCK_LOCKSTS31_T;

// chk_id
#define SPINLOCK_CHK_ID_CHK_ID(n) (((n)&0xffffffff) << 0)

// recctrl
#define SPINLOCK_REC_CTRL (1 << 0)

// ttlsts
#define SPINLOCK_LOCK(n) (((n)&0xffffffff) << 0)

// swflag0
#define SPINLOCK_SWFLAG0_SWFLAG0(n) (((n)&0xffffffff) << 0)

// swflag1
#define SPINLOCK_SWFLAG1_SWFLAG1(n) (((n)&0xffffffff) << 0)

// swflag2
#define SPINLOCK_SWFLAG2_SWFLAG2(n) (((n)&0xffffffff) << 0)

// swflag3
#define SPINLOCK_SWFLAG3_SWFLAG3(n) (((n)&0xffffffff) << 0)

// mstid0
#define SPINLOCK_MSTID0_MSTID0(n) (((n)&0xffffffff) << 0)

// mstid1
#define SPINLOCK_MSTID1_MSTID1(n) (((n)&0xffffffff) << 0)

// mstid2
#define SPINLOCK_MSTID2_MSTID2(n) (((n)&0xffffffff) << 0)

// mstid3
#define SPINLOCK_MSTID3_MSTID3(n) (((n)&0xffffffff) << 0)

// mstid4
#define SPINLOCK_MSTID4_MSTID4(n) (((n)&0xffffffff) << 0)

// mstid5
#define SPINLOCK_MSTID5_MSTID5(n) (((n)&0xffffffff) << 0)

// mstid6
#define SPINLOCK_MSTID6_MSTID6(n) (((n)&0xffffffff) << 0)

// mstid7
#define SPINLOCK_MSTID7_MSTID7(n) (((n)&0xffffffff) << 0)

// mstid8
#define SPINLOCK_MSTID8_MSTID8(n) (((n)&0xffffffff) << 0)

// mstid9
#define SPINLOCK_MSTID9_MSTID9(n) (((n)&0xffffffff) << 0)

// mstid10
#define SPINLOCK_MSTID10_MSTID10(n) (((n)&0xffffffff) << 0)

// mstid11
#define SPINLOCK_MSTID11_MSTID11(n) (((n)&0xffffffff) << 0)

// mstid12
#define SPINLOCK_MSTID12_MSTID12(n) (((n)&0xffffffff) << 0)

// mstid13
#define SPINLOCK_MSTID13_MSTID13(n) (((n)&0xffffffff) << 0)

// mstid14
#define SPINLOCK_MSTID14_MSTID14(n) (((n)&0xffffffff) << 0)

// mstid15
#define SPINLOCK_MSTID15_MSTID15(n) (((n)&0xffffffff) << 0)

// mstid16
#define SPINLOCK_MSTID16_MSTID16(n) (((n)&0xffffffff) << 0)

// mstid17
#define SPINLOCK_MSTID17_MSTID17(n) (((n)&0xffffffff) << 0)

// mstid18
#define SPINLOCK_MSTID18_MSTID18(n) (((n)&0xffffffff) << 0)

// mstid19
#define SPINLOCK_MSTID19_MSTID19(n) (((n)&0xffffffff) << 0)

// mstid20
#define SPINLOCK_MSTID20_MSTID20(n) (((n)&0xffffffff) << 0)

// mstid21
#define SPINLOCK_MSTID21_MSTID21(n) (((n)&0xffffffff) << 0)

// mstid22
#define SPINLOCK_MSTID22_MSTID22(n) (((n)&0xffffffff) << 0)

// mstid23
#define SPINLOCK_MSTID23_MSTID23(n) (((n)&0xffffffff) << 0)

// mstid24
#define SPINLOCK_MSTID24_MSTID24(n) (((n)&0xffffffff) << 0)

// mstid25
#define SPINLOCK_MSTID25_MSTID25(n) (((n)&0xffffffff) << 0)

// mstid26
#define SPINLOCK_MSTID26_MSTID26(n) (((n)&0xffffffff) << 0)

// mstid27
#define SPINLOCK_MSTID27_MSTID27(n) (((n)&0xffffffff) << 0)

// mstid28
#define SPINLOCK_MSTID28_MSTID28(n) (((n)&0xffffffff) << 0)

// mstid29
#define SPINLOCK_MSTID29_MSTID29(n) (((n)&0xffffffff) << 0)

// mstid30
#define SPINLOCK_MSTID30_MSTID30(n) (((n)&0xffffffff) << 0)

// mstid31
#define SPINLOCK_MSTID31_MSTID31(n) (((n)&0xffffffff) << 0)

// locksts0
#define SPINLOCK_LOCKSTS0_LOCKSTS0 (1 << 0)

// locksts1
#define SPINLOCK_LOCKSTS1_LOCKSTS1 (1 << 0)

// locksts2
#define SPINLOCK_LOCKSTS2_LOCKSTS2 (1 << 0)

// locksts3
#define SPINLOCK_LOCKSTS3_LOCKSTS3 (1 << 0)

// locksts4
#define SPINLOCK_LOCKSTS4_LOCKSTS4 (1 << 0)

// locksts5
#define SPINLOCK_LOCKSTS5_LOCKSTS5 (1 << 0)

// locksts6
#define SPINLOCK_LOCKSTS6_LOCKSTS6 (1 << 0)

// locksts7
#define SPINLOCK_LOCKSTS7_LOCKSTS7 (1 << 0)

// locksts8
#define SPINLOCK_LOCKSTS8_LOCKSTS8 (1 << 0)

// locksts9
#define SPINLOCK_LOCKSTS9_LOCKSTS9 (1 << 0)

// locksts10
#define SPINLOCK_LOCKSTS10_LOCKSTS10 (1 << 0)

// locksts11
#define SPINLOCK_LOCKSTS11_LOCKSTS11 (1 << 0)

// locksts12
#define SPINLOCK_LOCKSTS12_LOCKSTS12 (1 << 0)

// locksts13
#define SPINLOCK_LOCKSTS13_LOCKSTS13 (1 << 0)

// locksts14
#define SPINLOCK_LOCKSTS14_LOCKSTS14 (1 << 0)

// locksts15
#define SPINLOCK_LOCKSTS15_LOCKSTS15 (1 << 0)

// locksts16
#define SPINLOCK_LOCKSTS16_LOCKSTS16 (1 << 0)

// locksts17
#define SPINLOCK_LOCKSTS17_LOCKSTS17 (1 << 0)

// locksts18
#define SPINLOCK_LOCKSTS18_LOCKSTS18 (1 << 0)

// locksts19
#define SPINLOCK_LOCKSTS19_LOCKSTS19 (1 << 0)

// locksts20
#define SPINLOCK_LOCKSTS20_LOCKSTS20 (1 << 0)

// locksts21
#define SPINLOCK_LOCKSTS21_LOCKSTS21 (1 << 0)

// locksts22
#define SPINLOCK_LOCKSTS22_LOCKSTS22 (1 << 0)

// locksts23
#define SPINLOCK_LOCKSTS23_LOCKSTS23 (1 << 0)

// locksts24
#define SPINLOCK_LOCKSTS24_LOCKSTS24 (1 << 0)

// locksts25
#define SPINLOCK_LOCKSTS25_LOCKSTS25 (1 << 0)

// locksts26
#define SPINLOCK_LOCKSTS26_LOCKSTS26 (1 << 0)

// locksts27
#define SPINLOCK_LOCKSTS27_LOCKSTS27 (1 << 0)

// locksts28
#define SPINLOCK_LOCKSTS28_LOCKSTS28 (1 << 0)

// locksts29
#define SPINLOCK_LOCKSTS29_LOCKSTS29 (1 << 0)

// locksts30
#define SPINLOCK_LOCKSTS30_LOCKSTS30 (1 << 0)

// locksts31
#define SPINLOCK_LOCKSTS31_LOCKSTS31 (1 << 0)

// verid
#define SPINLOCK_VERID_VERID(n) (((n)&0xffffffff) << 0)

#endif // _SPINLOCK_H_
