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

#ifndef _WCN_BT_LINK_H_
#define _WCN_BT_LINK_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_WCN_BT_REG_BASE (0x14100000)

typedef volatile struct
{
    uint32_t rwbtcnt;             // 0x00000000
    uint32_t version;             // 0x00000004
    uint32_t rwbtconf;            // 0x00000008
    uint32_t intcntl;             // 0x0000000c
    uint32_t intstat;             // 0x00000010
    uint32_t intrawstat;          // 0x00000014
    uint32_t intack;              // 0x00000018
    uint32_t slotclk;             // 0x0000001c
    uint32_t finetimecnt;         // 0x00000020
    uint32_t abtraincnt;          // 0x00000024
    uint32_t edrcntl_nwinsize;    // 0x00000028
    uint32_t et_currentrxdescptr; // 0x0000002c
    uint32_t deepslcntl;          // 0x00000030
    uint32_t deepslwkup;          // 0x00000034
    uint32_t deepslstat;          // 0x00000038
    uint32_t enbpreset;           // 0x0000003c
    uint32_t finecntcorr;         // 0x00000040
    uint32_t clkncntcorr;         // 0x00000044
    uint32_t tokencntl;           // 0x00000048
    uint32_t __76[1];             // 0x0000004c
    uint32_t diagcntl;            // 0x00000050
    uint32_t diagstat;            // 0x00000054
    uint32_t debugaddmax;         // 0x00000058
    uint32_t debugaddmin;         // 0x0000005c
    uint32_t errortypestat;       // 0x00000060
    uint32_t swprofiling;         // 0x00000064
    uint32_t __104[2];            // 0x00000068
    uint32_t radiocntl0;          // 0x00000070
    uint32_t radiocntl1;          // 0x00000074
    uint32_t radiocntl2;          // 0x00000078
    uint32_t radiocntl3;          // 0x0000007c
    uint32_t radiopwrupdn;        // 0x00000080
    uint32_t __132[3];            // 0x00000084
    uint32_t txmicval;            // 0x00000090
    uint32_t rxmicval;            // 0x00000094
    uint32_t e0ptr;               // 0x00000098
    uint32_t __156[1];            // 0x0000009c
    uint32_t rftestcntl;          // 0x000000a0
    uint32_t rftestfreq;          // 0x000000a4
    uint32_t __168[2];            // 0x000000a8
    uint32_t timgencntl;          // 0x000000b0
    uint32_t grosstimtgt;         // 0x000000b4
    uint32_t finetimtgt;          // 0x000000b8
    uint32_t sketclkn;            // 0x000000bc
    uint32_t pcacntl0;            // 0x000000c0
    uint32_t pcacntl1;            // 0x000000c4
    uint32_t pcastat;             // 0x000000c8
    uint32_t __204[29];           // 0x000000cc
    uint32_t escochancntl0;       // 0x00000140
    uint32_t escomutecntl0;       // 0x00000144
    uint32_t escocurrenttxptr0;   // 0x00000148
    uint32_t escocurrentrxptr0;   // 0x0000014c
    uint32_t escoltcntl0;         // 0x00000150
    uint32_t escotrcntl0;         // 0x00000154
    uint32_t escodaycnt0;         // 0x00000158
    uint32_t __348[1];            // 0x0000015c
    uint32_t escochancntl1;       // 0x00000160
    uint32_t escomutecntl1;       // 0x00000164
    uint32_t escocurrenttxptr1;   // 0x00000168
    uint32_t escocurrentrxptr1;   // 0x0000016c
    uint32_t escoltcntl1;         // 0x00000170
    uint32_t escotrcntl1;         // 0x00000174
    uint32_t escodaycnt1;         // 0x00000178
    uint32_t __380[1];            // 0x0000017c
    uint32_t escochancntl2;       // 0x00000180
    uint32_t escomutecntl2;       // 0x00000184
    uint32_t escocurrenttxptr2;   // 0x00000188
    uint32_t escocurrentrxptr2;   // 0x0000018c
    uint32_t escoltcntl2;         // 0x00000190
    uint32_t escotrcntl2;         // 0x00000194
    uint32_t escodaycnt2;         // 0x00000198
    uint32_t __412[1];            // 0x0000019c
    uint32_t audiocntl0;          // 0x000001a0
    uint32_t audiocntl1;          // 0x000001a4
    uint32_t audiocntl2;          // 0x000001a8
    uint32_t __428[1];            // 0x000001ac
    uint32_t pcmgencntl;          // 0x000001b0
    uint32_t pcmphyscntl0;        // 0x000001b4
    uint32_t pcmphyscntl1;        // 0x000001b8
    uint32_t pcmpadding;          // 0x000001bc
    uint32_t pcmpllcntl0;         // 0x000001c0
    uint32_t pcmpllcntl1;         // 0x000001c4
    uint32_t pcmpllcntl2;         // 0x000001c8
    uint32_t pcmsourceptr;        // 0x000001cc
    uint32_t pcmsinkptr;          // 0x000001d0
    uint32_t __468[3];            // 0x000001d4
    uint32_t bredrprioscharb;     // 0x000001e0
    uint32_t plcaddr01;           // 0x000001e4
    uint32_t plcaddr23;           // 0x000001e8
    uint32_t plcaddr45;           // 0x000001ec
    uint32_t plcaddr67;           // 0x000001f0
    uint32_t plcaddr89;           // 0x000001f4
    uint32_t plcaddrb;            // 0x000001f8
    uint32_t plcconf;             // 0x000001fc
} HWP_WCN_BT_LINK_T;

#define hwp_wcnBtReg ((HWP_WCN_BT_LINK_T *)REG_ACCESS_ADDRESS(REG_WCN_BT_REG_BASE))

// rwbtcnt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t syncerr : 4;           // [3:0]
        uint32_t __7_4 : 4;             // [7:4]
        uint32_t rwbten : 1;            // [8]
        uint32_t __11_9 : 3;            // [11:9]
        uint32_t cx_dnabort : 1;        // [12]
        uint32_t cx_rxbsyena : 1;       // [13]
        uint32_t cx_txbsyena : 1;       // [14]
        uint32_t seqndsb : 1;           // [15]
        uint32_t hopdsb : 1;            // [16]
        uint32_t crcdsb : 1;            // [17]
        uint32_t arqndsb : 1;           // [18]
        uint32_t whitdsb : 1;           // [19]
        uint32_t cryptdsb : 1;          // [20]
        uint32_t lmpflowdsb : 1;        // [21]
        uint32_t __22_22 : 1;           // [22]
        uint32_t flowdsb : 1;           // [23]
        uint32_t sniff_abort : 1;       // [24]
        uint32_t pageinq_abort : 1;     // [25]
        uint32_t rftest_abort : 1;      // [26]
        uint32_t scan_abort : 1;        // [27]
        uint32_t swint_req : 1;         // [28]
        uint32_t reg_soft_rst : 1;      // [29]
        uint32_t master_tgsoft_rst : 1; // [30]
        uint32_t master_soft_rst : 1;   // [31]
    } b;
} REG_WCN_BT_LINK_RWBTCNT_T;

// version
typedef union {
    uint32_t v;
    struct
    {
        uint32_t build_num : 8; // [7:0], read only
        uint32_t upg : 8;       // [15:8], read only
        uint32_t rel : 8;       // [23:16], read only
        uint32_t typ : 8;       // [31:24], read only
    } b;
} REG_WCN_BT_LINK_VERSION_T;

// rwbtconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t addr_width : 5; // [4:0], read only
        uint32_t data_width : 1; // [5], read only
        uint32_t bus_type : 1;   // [6], read only
        uint32_t intmode : 1;    // [7], read only
        uint32_t clk_sel : 6;    // [13:8], read only
        uint32_t usecrypt : 1;   // [14], read only
        uint32_t usedbg : 1;     // [15], read only
        uint32_t rfif : 5;       // [20:16], read only
        uint32_t wlancoex : 1;   // [21], read only
        uint32_t mwscoex : 1;    // [22], read only
        uint32_t pcm : 1;        // [23], read only
        uint32_t vxportnb : 2;   // [25:24], read only
        uint32_t mwswci1 : 1;    // [26], read only
        uint32_t mwswci2 : 1;    // [27], read only
        uint32_t __30_28 : 3;    // [30:28]
        uint32_t dmmode : 1;     // [31], read only
    } b;
} REG_WCN_BT_LINK_RWBTCONF_T;

// intcntl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clknintmsk : 1;      // [0]
        uint32_t rxintmsk : 1;        // [1]
        uint32_t slpintmsk : 1;       // [2]
        uint32_t audioint0msk : 1;    // [3]
        uint32_t audioint1msk : 1;    // [4]
        uint32_t audioint2msk : 1;    // [5]
        uint32_t frsyncintmsk : 1;    // [6]
        uint32_t mtoffint0msk : 1;    // [7]
        uint32_t mtoffint1msk : 1;    // [8]
        uint32_t finetgtintmsk : 1;   // [9]
        uint32_t grosstgtintmsk : 1;  // [10]
        uint32_t errorintmsk : 1;     // [11]
        uint32_t mwswcitxintmsk : 1;  // [12]
        uint32_t mwswcirxintmsk : 1;  // [13]
        uint32_t frameintmsk : 1;     // [14]
        uint32_t frameapfaintmsk : 1; // [15]
        uint32_t swintmsk : 1;        // [16]
        uint32_t sketintmsk : 1;      // [17]
        uint32_t __31_18 : 14;        // [31:18]
    } b;
} REG_WCN_BT_LINK_INTCNTL_T;

// intstat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clknintstat : 1;      // [0], read only
        uint32_t rxintstat : 1;        // [1], read only
        uint32_t slpintstat : 1;       // [2], read only
        uint32_t audioint0stat : 1;    // [3], read only
        uint32_t audioint1stat : 1;    // [4], read only
        uint32_t audioint2stat : 1;    // [5], read only
        uint32_t frsyncintstat : 1;    // [6], read only
        uint32_t mtoffint0stat : 1;    // [7], read only
        uint32_t mtoffint1stat : 1;    // [8], read only
        uint32_t finetgtintstat : 1;   // [9], read only
        uint32_t grosstgtintstat : 1;  // [10], read only
        uint32_t errorintstat : 1;     // [11], read only
        uint32_t mwswcitxintstat : 1;  // [12], read only
        uint32_t mwswcirxintstat : 1;  // [13], read only
        uint32_t frameintstat : 1;     // [14], read only
        uint32_t frameapfaintstat : 1; // [15], read only
        uint32_t swintstat : 1;        // [16], read only
        uint32_t sketintstat : 1;      // [17], read only
        uint32_t __31_18 : 14;         // [31:18]
    } b;
} REG_WCN_BT_LINK_INTSTAT_T;

// intrawstat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clknintrawstat : 1;      // [0], read only
        uint32_t rxintrawstat : 1;        // [1], read only
        uint32_t slpintrawstat : 1;       // [2], read only
        uint32_t audioint0rawstat : 1;    // [3], read only
        uint32_t audioint1rawstat : 1;    // [4], read only
        uint32_t audioint2rawstat : 1;    // [5], read only
        uint32_t frsyncintrawstat : 1;    // [6], read only
        uint32_t mtoffint0rawstat : 1;    // [7], read only
        uint32_t mtoffint1rawstat : 1;    // [8], read only
        uint32_t finetgtintrawstat : 1;   // [9], read only
        uint32_t grosstgtintrawstat : 1;  // [10], read only
        uint32_t errorintrawstat : 1;     // [11], read only
        uint32_t mwswcitxintrawstat : 1;  // [12], read only
        uint32_t mwswcirxintrawstat : 1;  // [13], read only
        uint32_t frameintrawstat : 1;     // [14], read only
        uint32_t frameapfaintrawstat : 1; // [15], read only
        uint32_t swintrawstat : 1;        // [16], read only
        uint32_t sketintrawstat : 1;      // [17], read only
        uint32_t __31_18 : 14;            // [31:18]
    } b;
} REG_WCN_BT_LINK_INTRAWSTAT_T;

// intack
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clknintack : 1;      // [0], write set
        uint32_t rxintack : 1;        // [1], write set
        uint32_t slpintack : 1;       // [2], write set
        uint32_t audioint0ack : 1;    // [3], write set
        uint32_t audioint1ack : 1;    // [4], write set
        uint32_t audioint2ack : 1;    // [5], write set
        uint32_t frsyncintack : 1;    // [6], write set
        uint32_t mtoffint0ack : 1;    // [7], write set
        uint32_t mtoffint1ack : 1;    // [8], write set
        uint32_t finetgtintack : 1;   // [9], write set
        uint32_t grosstgtintack : 1;  // [10], write set
        uint32_t errorintack : 1;     // [11], write set
        uint32_t mwswcitxintack : 1;  // [12], write set
        uint32_t mwswcirxintack : 1;  // [13], write set
        uint32_t frameintack : 1;     // [14], write set
        uint32_t frameapfaintack : 1; // [15], write set
        uint32_t swintack : 1;        // [16], write set
        uint32_t sketintack : 1;      // [17], write set
        uint32_t __31_18 : 14;        // [31:18]
    } b;
} REG_WCN_BT_LINK_INTACK_T;

// slotclk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sclk : 28;    // [27:0]
        uint32_t __29_28 : 2;  // [29:28]
        uint32_t clkn_upd : 1; // [30]
        uint32_t samp : 1;     // [31]
    } b;
} REG_WCN_BT_LINK_SLOTCLK_T;

// finetimecnt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t finecnt : 10; // [9:0], read only
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_WCN_BT_LINK_FINETIMECNT_T;

// abtraincnt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t abtinqtime : 11;       // [10:0]
        uint32_t __11_11 : 1;           // [11]
        uint32_t abtinqload : 1;        // [12]
        uint32_t __13_13 : 1;           // [13]
        uint32_t abtinqstartvalue : 1;  // [14]
        uint32_t abtinqen : 1;          // [15]
        uint32_t abtpagetime : 11;      // [26:16]
        uint32_t __27_27 : 1;           // [27]
        uint32_t abtpageload : 1;       // [28]
        uint32_t __29_29 : 1;           // [29]
        uint32_t abtpagestartvalue : 1; // [30]
        uint32_t abtpageen : 1;         // [31]
    } b;
} REG_WCN_BT_LINK_ABTRAINCNT_T;

// edrcntl_nwinsize
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rxgrd_timeout : 10; // [9:0]
        uint32_t __12_10 : 3;        // [12:10]
        uint32_t rx_swap : 1;        // [13]
        uint32_t tx_swap : 1;        // [14]
        uint32_t rxguarddsb : 1;     // [15]
        uint32_t edrbcast : 1;       // [16]
        uint32_t __23_17 : 7;        // [23:17]
        uint32_t nwinsize : 8;       // [31:24]
    } b;
} REG_WCN_BT_LINK_EDRCNTL_NWINSIZE_T;

// et_currentrxdescptr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t currentrxdescptr : 15; // [14:0]
        uint32_t __15_15 : 1;           // [15]
        uint32_t etptr : 16;            // [31:16]
    } b;
} REG_WCN_BT_LINK_ET_CURRENTRXDESCPTR_T;

// deepslcntl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t osc_sleep_en : 1;           // [0]
        uint32_t radio_sleep_en : 1;         // [1]
        uint32_t deep_sleep_on : 1;          // [2]
        uint32_t deep_sleep_corr_en : 1;     // [3]
        uint32_t soft_wakeup_req : 1;        // [4]
        uint32_t __14_5 : 10;                // [14:5]
        uint32_t deep_sleep_stat : 1;        // [15], read only
        uint32_t __27_16 : 12;               // [27:16]
        uint32_t ext_activity_wakeup_en : 1; // [28]
        uint32_t ext_low_wakeup_en : 1;      // [29]
        uint32_t ext_high_wakeup_en : 1;     // [30]
        uint32_t extwkupdsb : 1;             // [31]
    } b;
} REG_WCN_BT_LINK_DEEPSLCNTL_T;

// enbpreset
typedef union {
    uint32_t v;
    struct
    {
        uint32_t twrm : 10;  // [9:0]
        uint32_t twosc : 11; // [20:10]
        uint32_t twext : 11; // [31:21]
    } b;
} REG_WCN_BT_LINK_ENBPRESET_T;

// finecntcorr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t finecntcorr : 10; // [9:0]
        uint32_t __31_10 : 22;     // [31:10]
    } b;
} REG_WCN_BT_LINK_FINECNTCORR_T;

// clkncntcorr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clkncntcorr : 28; // [27:0]
        uint32_t __30_28 : 3;      // [30:28]
        uint32_t abs_delta : 1;    // [31]
    } b;
} REG_WCN_BT_LINK_CLKNCNTCORR_T;

// tokencntl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t observer_en : 1;      // [0]
        uint32_t observer_active : 1;  // [1]
        uint32_t arbiter_en : 1;       // [2]
        uint32_t arbiter_bypass : 1;   // [3]
        uint32_t __7_4 : 4;            // [7:4]
        uint32_t token_rxwin : 7;      // [14:8]
        uint32_t token_quick_sync : 1; // [15]
        uint32_t token_rx_delay : 6;   // [21:16]
        uint32_t __23_22 : 2;          // [23:22]
        uint32_t token_tx_delay : 6;   // [29:24]
        uint32_t __31_30 : 2;          // [31:30]
    } b;
} REG_WCN_BT_LINK_TOKENCNTL_T;

// diagcntl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t diag0 : 6;    // [5:0]
        uint32_t __6_6 : 1;    // [6]
        uint32_t diag0_en : 1; // [7]
        uint32_t diag1 : 6;    // [13:8]
        uint32_t __14_14 : 1;  // [14]
        uint32_t diag1_en : 1; // [15]
        uint32_t diag2 : 6;    // [21:16]
        uint32_t __22_22 : 1;  // [22]
        uint32_t diag2_en : 1; // [23]
        uint32_t diag3 : 6;    // [29:24]
        uint32_t __30_30 : 1;  // [30]
        uint32_t diag3_en : 1; // [31]
    } b;
} REG_WCN_BT_LINK_DIAGCNTL_T;

// diagstat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t diag0stat : 8; // [7:0], read only
        uint32_t diag1stat : 8; // [15:8], read only
        uint32_t diag2stat : 8; // [23:16], read only
        uint32_t diag3stat : 8; // [31:24], read only
    } b;
} REG_WCN_BT_LINK_DIAGSTAT_T;

// debugaddmax
typedef union {
    uint32_t v;
    struct
    {
        uint32_t em_addmax : 16;  // [15:0]
        uint32_t reg_addmax : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_DEBUGADDMAX_T;

// debugaddmin
typedef union {
    uint32_t v;
    struct
    {
        uint32_t em_addmin : 16;  // [15:0]
        uint32_t reg_addmin : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_DEBUGADDMIN_T;

// errortypestat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txcrypt_error : 1;         // [0], read only
        uint32_t rxcrypt_error : 1;         // [1], read only
        uint32_t cryptmode_error : 1;       // [2], read only
        uint32_t pktcntl_emacc_error : 1;   // [3], read only
        uint32_t radiocntl_emacc_error : 1; // [4], read only
        uint32_t audio_emacc_error : 1;     // [5], read only
        uint32_t pcm_emacc_error : 1;       // [6], read only
        uint32_t frm_schdl_emacc_error : 1; // [7], read only
        uint32_t mwswci_emacc_error : 1;    // [8], read only
        uint32_t frm_schdl_entry_error : 1; // [9], read only
        uint32_t frm_schdl_apfm_error : 1;  // [10], read only
        uint32_t frm_cntl_apfm_error : 1;   // [11], read only
        uint32_t frm_cntl_emacc_error : 1;  // [12], read only
        uint32_t frm_cntl_timer_error : 1;  // [13], read only
        uint32_t hopunderrun_error : 1;     // [14], read only
        uint32_t chmap_error : 1;           // [15], read only
        uint32_t csformat_error : 1;        // [16], read only
        uint32_t csattnb_error : 1;         // [17], read only
        uint32_t txdesc_empty_error : 1;    // [18], read only
        uint32_t rxdesc_empty_error : 1;    // [19], read only
        uint32_t txbuf_ptr_error : 1;       // [20], read only
        uint32_t rxbuf_ptr_error : 1;       // [21], read only
        uint32_t __31_22 : 10;              // [31:22]
    } b;
} REG_WCN_BT_LINK_ERRORTYPESTAT_T;

// radiocntl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spigo : 1;           // [0]
        uint32_t spicomp : 1;         // [1], read only
        uint32_t __4_2 : 3;           // [4:2]
        uint32_t spifreq : 2;         // [6:5]
        uint32_t __20_7 : 14;         // [20:7]
        uint32_t forceagc_en : 1;     // [21]
        uint32_t dpcorr_en : 1;       // [22]
        uint32_t sync_pulse_mode : 1; // [23]
        uint32_t __31_24 : 8;         // [31:24]
    } b;
} REG_WCN_BT_LINK_RADIOCNTL0_T;

// radiocntl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spiptr : 16;  // [15:0]
        uint32_t xrfsel : 5;   // [20:16]
        uint32_t __31_21 : 11; // [31:21]
    } b;
} REG_WCN_BT_LINK_RADIOCNTL1_T;

// radiocntl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t forceagc_length : 12; // [11:0]
        uint32_t __15_12 : 4;          // [15:12]
        uint32_t sync_position : 7;    // [22:16]
        uint32_t __23_23 : 1;          // [23]
        uint32_t tx_delay : 8;         // [31:24]
    } b;
} REG_WCN_BT_LINK_RADIOCNTL2_T;

// radiocntl3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t freqtable_ptr : 16; // [15:0]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_WCN_BT_LINK_RADIOCNTL3_T;

// radiopwrupdn
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txpwrupct : 8;   // [7:0]
        uint32_t txpwrdnct : 5;   // [12:8]
        uint32_t __15_13 : 3;     // [15:13]
        uint32_t rxpwrupct : 8;   // [23:16]
        uint32_t rtrip_delay : 7; // [30:24]
        uint32_t __31_31 : 1;     // [31]
    } b;
} REG_WCN_BT_LINK_RADIOPWRUPDN_T;

// e0ptr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t e0ptr : 16;   // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_E0PTR_T;

// rftestcntl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txpldsrc : 1;       // [0]
        uint32_t prbstype : 1;       // [1]
        uint32_t infinitetx : 1;     // [2]
        uint32_t infinite_entry : 1; // [3]
        uint32_t __6_4 : 3;          // [6:4]
        uint32_t infiniterx : 1;     // [7]
        uint32_t __15_8 : 8;         // [15:8]
        uint32_t herrren : 1;        // [16]
        uint32_t sserrren : 1;       // [17]
        uint32_t __31_18 : 14;       // [31:18]
    } b;
} REG_WCN_BT_LINK_RFTESTCNTL_T;

// rftestfreq
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txfreq : 7;          // [6:0]
        uint32_t __7_7 : 1;           // [7]
        uint32_t rxfreq : 7;          // [14:8]
        uint32_t __15_15 : 1;         // [15]
        uint32_t testmodeen : 1;      // [16]
        uint32_t directlopbacken : 1; // [17]
        uint32_t __31_18 : 14;        // [31:18]
    } b;
} REG_WCN_BT_LINK_RFTESTFREQ_T;

// timgencntl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prefetch_time : 9;       // [8:0]
        uint32_t __15_9 : 7;              // [15:9]
        uint32_t prefetchabort_time : 10; // [25:16]
        uint32_t __30_26 : 5;             // [30:26]
        uint32_t apfm_en : 1;             // [31]
    } b;
} REG_WCN_BT_LINK_TIMGENCNTL_T;

// grosstimtgt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t grosstarget : 23; // [22:0]
        uint32_t __31_23 : 9;      // [31:23]
    } b;
} REG_WCN_BT_LINK_GROSSTIMTGT_T;

// finetimtgt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t finetarget : 27; // [26:0]
        uint32_t __31_27 : 5;     // [31:27]
    } b;
} REG_WCN_BT_LINK_FINETIMTGT_T;

// sketclkn
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sketclkn : 28; // [27:0], read only
        uint32_t __31_28 : 4;   // [31:28]
    } b;
} REG_WCN_BT_LINK_SKETCLKN_T;

// pcacntl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t phase_shift_en : 1; // [0]
        uint32_t sync_source : 1;    // [1]
        uint32_t frsync_pol : 1;     // [2]
        uint32_t blindcorr_en : 1;   // [3]
        uint32_t corr_step : 4;      // [7:4]
        uint32_t slvlbl : 5;         // [12:8]
        uint32_t __15_13 : 3;        // [15:13]
        uint32_t target_offset : 11; // [26:16]
        uint32_t __31_27 : 5;        // [31:27]
    } b;
} REG_WCN_BT_LINK_PCACNTL0_T;

// pcacntl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clock_shift : 11;   // [10:0]
        uint32_t __11_11 : 1;        // [11]
        uint32_t clock_shift_en : 1; // [12]
        uint32_t __15_13 : 3;        // [15:13]
        uint32_t corr_interval : 8;  // [23:16]
        uint32_t __31_24 : 8;        // [31:24]
    } b;
} REG_WCN_BT_LINK_PCACNTL1_T;

// pcastat
typedef union {
    uint32_t v;
    struct
    {
        uint32_t moment_offset : 11; // [10:0], read only
        uint32_t __15_11 : 5;        // [15:11]
        uint32_t shift_phase : 11;   // [26:16], read only
        uint32_t __31_27 : 5;        // [31:27]
    } b;
} REG_WCN_BT_LINK_PCASTAT_T;

// escochancntl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tesco0 : 8;        // [7:0]
        uint32_t intdelay0 : 5;     // [12:8]
        uint32_t itmode0 : 1;       // [13]
        uint32_t escochanen0 : 1;   // [14]
        uint32_t escochanswen0 : 1; // [15]
        uint32_t __30_16 : 15;      // [30:16]
        uint32_t tog0 : 1;          // [31], read only
    } b;
} REG_WCN_BT_LINK_ESCOCHANCNTL0_T;

// escomutecntl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mutepatt0 : 16;   // [15:0]
        uint32_t invl0_0 : 2;      // [17:16]
        uint32_t invl0_1 : 2;      // [19:18]
        uint32_t __21_20 : 2;      // [21:20]
        uint32_t mute_source0 : 1; // [22]
        uint32_t mute_sink0 : 1;   // [23]
        uint32_t __31_24 : 8;      // [31:24]
    } b;
} REG_WCN_BT_LINK_ESCOMUTECNTL0_T;

// escocurrenttxptr0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t esco0ptrtx0 : 16; // [15:0]
        uint32_t esco0ptrtx1 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_ESCOCURRENTTXPTR0_T;

// escocurrentrxptr0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t esco0ptrrx0 : 16; // [15:0]
        uint32_t esco0ptrrx1 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_ESCOCURRENTRXPTR0_T;

// escoltcntl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t synltaddr0 : 3; // [2:0]
        uint32_t syntype0 : 1;   // [3]
        uint32_t escoedrtx0 : 1; // [4]
        uint32_t escoedrrx0 : 1; // [5]
        uint32_t __15_6 : 10;    // [15:6]
        uint32_t retxnb0 : 8;    // [23:16]
        uint32_t __31_24 : 8;    // [31:24]
    } b;
} REG_WCN_BT_LINK_ESCOLTCNTL0_T;

// escotrcntl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rxtype0 : 4; // [3:0]
        uint32_t rxlen0 : 10; // [13:4]
        uint32_t __15_14 : 2; // [15:14]
        uint32_t txtype0 : 4; // [19:16]
        uint32_t txlen0 : 10; // [29:20]
        uint32_t __30_30 : 1; // [30]
        uint32_t txseqn0 : 1; // [31]
    } b;
} REG_WCN_BT_LINK_ESCOTRCNTL0_T;

// escodaycnt0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t daycounter0 : 11; // [10:0]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_WCN_BT_LINK_ESCODAYCNT0_T;

// escochancntl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tesco1 : 8;        // [7:0]
        uint32_t intdelay1 : 5;     // [12:8]
        uint32_t itmode1 : 1;       // [13]
        uint32_t escochanen1 : 1;   // [14]
        uint32_t escochanswen1 : 1; // [15]
        uint32_t __30_16 : 15;      // [30:16]
        uint32_t tog1 : 1;          // [31], read only
    } b;
} REG_WCN_BT_LINK_ESCOCHANCNTL1_T;

// escomutecntl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mutepatt1 : 16;   // [15:0]
        uint32_t invl1_0 : 2;      // [17:16]
        uint32_t invl1_1 : 2;      // [19:18]
        uint32_t __21_20 : 2;      // [21:20]
        uint32_t mute_source1 : 1; // [22]
        uint32_t mute_sink1 : 1;   // [23]
        uint32_t __31_24 : 8;      // [31:24]
    } b;
} REG_WCN_BT_LINK_ESCOMUTECNTL1_T;

// escocurrenttxptr1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t esco1ptrtx0 : 16; // [15:0]
        uint32_t esco1ptrtx1 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_ESCOCURRENTTXPTR1_T;

// escocurrentrxptr1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t esco1ptrrx0 : 16; // [15:0]
        uint32_t esco1ptrrx1 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_ESCOCURRENTRXPTR1_T;

// escoltcntl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t syntaddr1 : 3;  // [2:0]
        uint32_t syntype1 : 1;   // [3]
        uint32_t escoedrtx1 : 1; // [4]
        uint32_t escoedrrx1 : 1; // [5]
        uint32_t __15_6 : 10;    // [15:6]
        uint32_t retxnb1 : 8;    // [23:16]
        uint32_t __31_24 : 8;    // [31:24]
    } b;
} REG_WCN_BT_LINK_ESCOLTCNTL1_T;

// escotrcntl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rxtype1 : 4; // [3:0]
        uint32_t rxlen1 : 10; // [13:4]
        uint32_t __15_14 : 2; // [15:14]
        uint32_t txtype1 : 4; // [19:16]
        uint32_t txlen1 : 10; // [29:20]
        uint32_t __30_30 : 1; // [30]
        uint32_t txseqn1 : 1; // [31]
    } b;
} REG_WCN_BT_LINK_ESCOTRCNTL1_T;

// escodaycnt1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t daycounter1 : 11; // [10:0]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_WCN_BT_LINK_ESCODAYCNT1_T;

// escochancntl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tesco2 : 8;        // [7:0]
        uint32_t intdelay2 : 5;     // [12:8]
        uint32_t itmode2 : 1;       // [13]
        uint32_t escochanen2 : 1;   // [14]
        uint32_t escochanswen2 : 1; // [15]
        uint32_t __30_16 : 15;      // [30:16]
        uint32_t tog2 : 1;          // [31], read only
    } b;
} REG_WCN_BT_LINK_ESCOCHANCNTL2_T;

// escomutecntl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mutepatt2 : 16;   // [15:0]
        uint32_t invl2_0 : 2;      // [17:16]
        uint32_t invl2_1 : 2;      // [19:18]
        uint32_t __21_20 : 2;      // [21:20]
        uint32_t mute_source2 : 1; // [22]
        uint32_t mute_sink2 : 1;   // [23]
        uint32_t __31_24 : 8;      // [31:24]
    } b;
} REG_WCN_BT_LINK_ESCOMUTECNTL2_T;

// escocurrenttxptr2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t esco2ptrtx0 : 16; // [15:0]
        uint32_t esco2ptrtx1 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_ESCOCURRENTTXPTR2_T;

// escocurrentrxptr2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t esco2ptrrx0 : 16; // [15:0]
        uint32_t esco2ptrrx1 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_ESCOCURRENTRXPTR2_T;

// escoltcntl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t syntaddr2 : 3;  // [2:0]
        uint32_t syntype2 : 1;   // [3]
        uint32_t escoedrtx2 : 1; // [4]
        uint32_t escoedrrx2 : 1; // [5]
        uint32_t __15_6 : 10;    // [15:6]
        uint32_t retxnb2 : 8;    // [23:16]
        uint32_t __31_24 : 8;    // [31:24]
    } b;
} REG_WCN_BT_LINK_ESCOLTCNTL2_T;

// escotrcntl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rxtype2 : 4; // [3:0]
        uint32_t rxlen2 : 10; // [13:4]
        uint32_t __15_14 : 2; // [15:14]
        uint32_t txtype2 : 4; // [19:16]
        uint32_t txlen2 : 10; // [29:20]
        uint32_t __30_30 : 1; // [30]
        uint32_t txseqn2 : 1; // [31]
    } b;
} REG_WCN_BT_LINK_ESCOTRCNTL2_T;

// escodaycnt2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t daycounter2 : 11; // [10:0]
        uint32_t __31_11 : 21;     // [31:11]
    } b;
} REG_WCN_BT_LINK_ESCODAYCNT2_T;

// audiocntl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cvsd_bitorder0 : 1; // [0]
        uint32_t __6_1 : 6;          // [6:1]
        uint32_t cvsden0 : 1;        // [7]
        uint32_t aulaw_code0 : 4;    // [11:8]
        uint32_t __14_12 : 3;        // [14:12]
        uint32_t aulawen0 : 1;       // [15]
        uint32_t sample_type0 : 2;   // [17:16]
        uint32_t __19_18 : 2;        // [19:18]
        uint32_t linear_format0 : 2; // [21:20]
        uint32_t __31_22 : 10;       // [31:22]
    } b;
} REG_WCN_BT_LINK_AUDIOCNTL0_T;

// audiocntl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cvsd_bitorder1 : 1; // [0]
        uint32_t __6_1 : 6;          // [6:1]
        uint32_t cvsden1 : 1;        // [7]
        uint32_t aulaw_code1 : 4;    // [11:8]
        uint32_t __14_12 : 3;        // [14:12]
        uint32_t aulawen1 : 1;       // [15]
        uint32_t sample_type1 : 2;   // [17:16]
        uint32_t __19_18 : 2;        // [19:18]
        uint32_t linear_format1 : 2; // [21:20]
        uint32_t __31_22 : 10;       // [31:22]
    } b;
} REG_WCN_BT_LINK_AUDIOCNTL1_T;

// audiocntl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cvsd_bitorder2 : 1; // [0]
        uint32_t __6_1 : 6;          // [6:1]
        uint32_t cvsden2 : 1;        // [7]
        uint32_t aulaw_code2 : 4;    // [11:8]
        uint32_t __14_12 : 3;        // [14:12]
        uint32_t aulawen2 : 1;       // [15]
        uint32_t sample_type2 : 2;   // [17:16]
        uint32_t __19_18 : 2;        // [19:18]
        uint32_t linear_format2 : 2; // [21:20]
        uint32_t __31_22 : 10;       // [31:22]
    } b;
} REG_WCN_BT_LINK_AUDIOCNTL2_T;

// pcmgencntl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pcmen : 1;       // [0]
        uint32_t lrswap : 1;      // [1]
        uint32_t byteswap : 1;    // [2]
        uint32_t mstslv : 1;      // [3]
        uint32_t mono_stereo : 1; // [4]
        uint32_t mono_lr_sel : 1; // [5]
        uint32_t loopback : 1;    // [6]
        uint32_t __7_7 : 1;       // [7]
        uint32_t vxchsel : 2;     // [9:8]
        uint32_t __31_10 : 22;    // [31:10]
    } b;
} REG_WCN_BT_LINK_PCMGENCNTL_T;

// pcmphyscntl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fsyncshp : 3;     // [2:0]
        uint32_t __3_3 : 1;        // [3]
        uint32_t doutcfg : 2;      // [5:4]
        uint32_t __7_6 : 2;        // [7:6]
        uint32_t lrchpol : 1;      // [8]
        uint32_t pcm_iom : 1;      // [9]
        uint32_t lsb1st : 1;       // [10]
        uint32_t __11_11 : 1;      // [11]
        uint32_t sampsz : 1;       // [12]
        uint32_t samptype : 1;     // [13]
        uint32_t __15_14 : 2;      // [15:14]
        uint32_t slotnb : 3;       // [18:16]
        uint32_t __19_19 : 1;      // [19]
        uint32_t firstactslot : 2; // [21:20]
        uint32_t __31_22 : 10;     // [31:22]
    } b;
} REG_WCN_BT_LINK_PCMPHYSCNTL0_T;

// pcmphyscntl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pcmclkval : 9;   // [8:0]
        uint32_t __15_9 : 7;      // [15:9]
        uint32_t pcmclklimit : 8; // [23:16]
        uint32_t __30_24 : 7;     // [30:24]
        uint32_t clkinv : 1;      // [31]
    } b;
} REG_WCN_BT_LINK_PCMPHYSCNTL1_T;

// pcmpadding
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lsamppad : 16; // [15:0]
        uint32_t rsamppad : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_PCMPADDING_T;

// pcmpllcntl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rl : 20;      // [19:0]
        uint32_t __31_20 : 12; // [31:20]
    } b;
} REG_WCN_BT_LINK_PCMPLLCNTL0_T;

// pcmpllcntl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t a : 19;      // [18:0]
        uint32_t __19_19 : 1; // [19]
        uint32_t olc : 11;    // [30:20]
        uint32_t __31_31 : 1; // [31]
    } b;
} REG_WCN_BT_LINK_PCMPLLCNTL1_T;

// pcmpllcntl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t w : 19;       // [18:0]
        uint32_t __31_19 : 13; // [31:19]
    } b;
} REG_WCN_BT_LINK_PCMPLLCNTL2_T;

// pcmsourceptr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pcmsourceptr0 : 16; // [15:0]
        uint32_t pcmsourceptr1 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_PCMSOURCEPTR_T;

// pcmsinkptr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pcmsinkptr0 : 16; // [15:0]
        uint32_t pcmsinkptr1 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_PCMSINKPTR_T;

// bredrprioscharb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bredrmargin : 8;   // [7:0]
        uint32_t __14_8 : 7;        // [14:8]
        uint32_t bredrpriomode : 1; // [15]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_WCN_BT_LINK_BREDRPRIOSCHARB_T;

// plcaddr01
typedef union {
    uint32_t v;
    struct
    {
        uint32_t plcbaseaddr0 : 16; // [15:0]
        uint32_t plcbaseaddr1 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_PLCADDR01_T;

// plcaddr23
typedef union {
    uint32_t v;
    struct
    {
        uint32_t plcbaseaddr2 : 16; // [15:0]
        uint32_t plcbaseaddr3 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_PLCADDR23_T;

// plcaddr45
typedef union {
    uint32_t v;
    struct
    {
        uint32_t plcbaseaddr4 : 16; // [15:0]
        uint32_t plcbaseaddr5 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_PLCADDR45_T;

// plcaddr67
typedef union {
    uint32_t v;
    struct
    {
        uint32_t plcbaseaddr6 : 16; // [15:0]
        uint32_t plcbaseaddr7 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_PLCADDR67_T;

// plcaddr89
typedef union {
    uint32_t v;
    struct
    {
        uint32_t plcbaseaddr8 : 16; // [15:0]
        uint32_t plcbaseaddr9 : 16; // [31:16]
    } b;
} REG_WCN_BT_LINK_PLCADDR89_T;

// plcaddrb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t plcbufaddr : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_WCN_BT_LINK_PLCADDRB_T;

// plcconf
typedef union {
    uint32_t v;
    struct
    {
        uint32_t conf_no_pitch_find_ref_threshold_dist : 14; // [13:0]
        uint32_t conf_pool_mem_word_swap : 1;                // [14]
        uint32_t conf_plc_except : 1;                        // [15]
        uint32_t conf_plc_en : 1;                            // [16]
        uint32_t conf_plc_type : 1;                          // [17]
        uint32_t conf_frame_mode : 2;                        // [19:18]
        uint32_t conf_scaling_mode : 1;                      // [20]
        uint32_t conf_diag0_sel : 2;                         // [22:21]
        uint32_t conf_plc_int_mask : 1;                      // [23]
        uint32_t conf_plc_ctrl_sw : 1;                       // [24]
        uint32_t conf_plc_start : 1;                         // [25], write set
        uint32_t plc_fsm_state : 4;                          // [29:26], read only
        uint32_t plc_int : 1;                                // [30], read only
        uint32_t plc_int_clr : 1;                            // [31], write set
    } b;
} REG_WCN_BT_LINK_PLCCONF_T;

// rwbtcnt
#define WCN_BT_LINK_SYNCERR(n) (((n)&0xf) << 0)
#define WCN_BT_LINK_RWBTEN (1 << 8)
#define WCN_BT_LINK_CX_DNABORT (1 << 12)
#define WCN_BT_LINK_CX_RXBSYENA (1 << 13)
#define WCN_BT_LINK_CX_TXBSYENA (1 << 14)
#define WCN_BT_LINK_SEQNDSB (1 << 15)
#define WCN_BT_LINK_HOPDSB (1 << 16)
#define WCN_BT_LINK_CRCDSB (1 << 17)
#define WCN_BT_LINK_ARQNDSB (1 << 18)
#define WCN_BT_LINK_WHITDSB (1 << 19)
#define WCN_BT_LINK_CRYPTDSB (1 << 20)
#define WCN_BT_LINK_LMPFLOWDSB (1 << 21)
#define WCN_BT_LINK_FLOWDSB (1 << 23)
#define WCN_BT_LINK_SNIFF_ABORT (1 << 24)
#define WCN_BT_LINK_PAGEINQ_ABORT (1 << 25)
#define WCN_BT_LINK_RFTEST_ABORT (1 << 26)
#define WCN_BT_LINK_SCAN_ABORT (1 << 27)
#define WCN_BT_LINK_SWINT_REQ (1 << 28)
#define WCN_BT_LINK_REG_SOFT_RST (1 << 29)
#define WCN_BT_LINK_MASTER_TGSOFT_RST (1 << 30)
#define WCN_BT_LINK_MASTER_SOFT_RST (1 << 31)

// version
#define WCN_BT_LINK_BUILD_NUM(n) (((n)&0xff) << 0)
#define WCN_BT_LINK_UPG(n) (((n)&0xff) << 8)
#define WCN_BT_LINK_REL(n) (((n)&0xff) << 16)
#define WCN_BT_LINK_TYP(n) (((n)&0xff) << 24)

// rwbtconf
#define WCN_BT_LINK_ADDR_WIDTH(n) (((n)&0x1f) << 0)
#define WCN_BT_LINK_DATA_WIDTH (1 << 5)
#define WCN_BT_LINK_BUS_TYPE (1 << 6)
#define WCN_BT_LINK_INTMODE (1 << 7)
#define WCN_BT_LINK_CLK_SEL(n) (((n)&0x3f) << 8)
#define WCN_BT_LINK_USECRYPT (1 << 14)
#define WCN_BT_LINK_USEDBG (1 << 15)
#define WCN_BT_LINK_RFIF(n) (((n)&0x1f) << 16)
#define WCN_BT_LINK_WLANCOEX (1 << 21)
#define WCN_BT_LINK_MWSCOEX (1 << 22)
#define WCN_BT_LINK_PCM (1 << 23)
#define WCN_BT_LINK_VXPORTNB(n) (((n)&0x3) << 24)
#define WCN_BT_LINK_MWSWCI1 (1 << 26)
#define WCN_BT_LINK_MWSWCI2 (1 << 27)
#define WCN_BT_LINK_DMMODE (1 << 31)

// intcntl
#define WCN_BT_LINK_CLKNINTMSK (1 << 0)
#define WCN_BT_LINK_RXINTMSK (1 << 1)
#define WCN_BT_LINK_SLPINTMSK (1 << 2)
#define WCN_BT_LINK_AUDIOINT0MSK (1 << 3)
#define WCN_BT_LINK_AUDIOINT1MSK (1 << 4)
#define WCN_BT_LINK_AUDIOINT2MSK (1 << 5)
#define WCN_BT_LINK_FRSYNCINTMSK (1 << 6)
#define WCN_BT_LINK_MTOFFINT0MSK (1 << 7)
#define WCN_BT_LINK_MTOFFINT1MSK (1 << 8)
#define WCN_BT_LINK_FINETGTINTMSK (1 << 9)
#define WCN_BT_LINK_GROSSTGTINTMSK (1 << 10)
#define WCN_BT_LINK_ERRORINTMSK (1 << 11)
#define WCN_BT_LINK_MWSWCITXINTMSK (1 << 12)
#define WCN_BT_LINK_MWSWCIRXINTMSK (1 << 13)
#define WCN_BT_LINK_FRAMEINTMSK (1 << 14)
#define WCN_BT_LINK_FRAMEAPFAINTMSK (1 << 15)
#define WCN_BT_LINK_SWINTMSK (1 << 16)
#define WCN_BT_LINK_SKETINTMSK (1 << 17)

// intstat
#define WCN_BT_LINK_CLKNINTSTAT (1 << 0)
#define WCN_BT_LINK_RXINTSTAT (1 << 1)
#define WCN_BT_LINK_SLPINTSTAT (1 << 2)
#define WCN_BT_LINK_AUDIOINT0STAT (1 << 3)
#define WCN_BT_LINK_AUDIOINT1STAT (1 << 4)
#define WCN_BT_LINK_AUDIOINT2STAT (1 << 5)
#define WCN_BT_LINK_FRSYNCINTSTAT (1 << 6)
#define WCN_BT_LINK_MTOFFINT0STAT (1 << 7)
#define WCN_BT_LINK_MTOFFINT1STAT (1 << 8)
#define WCN_BT_LINK_FINETGTINTSTAT (1 << 9)
#define WCN_BT_LINK_GROSSTGTINTSTAT (1 << 10)
#define WCN_BT_LINK_ERRORINTSTAT (1 << 11)
#define WCN_BT_LINK_MWSWCITXINTSTAT (1 << 12)
#define WCN_BT_LINK_MWSWCIRXINTSTAT (1 << 13)
#define WCN_BT_LINK_FRAMEINTSTAT (1 << 14)
#define WCN_BT_LINK_FRAMEAPFAINTSTAT (1 << 15)
#define WCN_BT_LINK_SWINTSTAT (1 << 16)
#define WCN_BT_LINK_SKETINTSTAT (1 << 17)

// intrawstat
#define WCN_BT_LINK_CLKNINTRAWSTAT (1 << 0)
#define WCN_BT_LINK_RXINTRAWSTAT (1 << 1)
#define WCN_BT_LINK_SLPINTRAWSTAT (1 << 2)
#define WCN_BT_LINK_AUDIOINT0RAWSTAT (1 << 3)
#define WCN_BT_LINK_AUDIOINT1RAWSTAT (1 << 4)
#define WCN_BT_LINK_AUDIOINT2RAWSTAT (1 << 5)
#define WCN_BT_LINK_FRSYNCINTRAWSTAT (1 << 6)
#define WCN_BT_LINK_MTOFFINT0RAWSTAT (1 << 7)
#define WCN_BT_LINK_MTOFFINT1RAWSTAT (1 << 8)
#define WCN_BT_LINK_FINETGTINTRAWSTAT (1 << 9)
#define WCN_BT_LINK_GROSSTGTINTRAWSTAT (1 << 10)
#define WCN_BT_LINK_ERRORINTRAWSTAT (1 << 11)
#define WCN_BT_LINK_MWSWCITXINTRAWSTAT (1 << 12)
#define WCN_BT_LINK_MWSWCIRXINTRAWSTAT (1 << 13)
#define WCN_BT_LINK_FRAMEINTRAWSTAT (1 << 14)
#define WCN_BT_LINK_FRAMEAPFAINTRAWSTAT (1 << 15)
#define WCN_BT_LINK_SWINTRAWSTAT (1 << 16)
#define WCN_BT_LINK_SKETINTRAWSTAT (1 << 17)

// intack
#define WCN_BT_LINK_CLKNINTACK (1 << 0)
#define WCN_BT_LINK_RXINTACK (1 << 1)
#define WCN_BT_LINK_SLPINTACK (1 << 2)
#define WCN_BT_LINK_AUDIOINT0ACK (1 << 3)
#define WCN_BT_LINK_AUDIOINT1ACK (1 << 4)
#define WCN_BT_LINK_AUDIOINT2ACK (1 << 5)
#define WCN_BT_LINK_FRSYNCINTACK (1 << 6)
#define WCN_BT_LINK_MTOFFINT0ACK (1 << 7)
#define WCN_BT_LINK_MTOFFINT1ACK (1 << 8)
#define WCN_BT_LINK_FINETGTINTACK (1 << 9)
#define WCN_BT_LINK_GROSSTGTINTACK (1 << 10)
#define WCN_BT_LINK_ERRORINTACK (1 << 11)
#define WCN_BT_LINK_MWSWCITXINTACK (1 << 12)
#define WCN_BT_LINK_MWSWCIRXINTACK (1 << 13)
#define WCN_BT_LINK_FRAMEINTACK (1 << 14)
#define WCN_BT_LINK_FRAMEAPFAINTACK (1 << 15)
#define WCN_BT_LINK_SWINTACK (1 << 16)
#define WCN_BT_LINK_SKETINTACK (1 << 17)

// slotclk
#define WCN_BT_LINK_SCLK(n) (((n)&0xfffffff) << 0)
#define WCN_BT_LINK_CLKN_UPD (1 << 30)
#define WCN_BT_LINK_SAMP (1 << 31)

// finetimecnt
#define WCN_BT_LINK_FINECNT(n) (((n)&0x3ff) << 0)

// abtraincnt
#define WCN_BT_LINK_ABTINQTIME(n) (((n)&0x7ff) << 0)
#define WCN_BT_LINK_ABTINQLOAD (1 << 12)
#define WCN_BT_LINK_ABTINQSTARTVALUE (1 << 14)
#define WCN_BT_LINK_ABTINQEN (1 << 15)
#define WCN_BT_LINK_ABTPAGETIME(n) (((n)&0x7ff) << 16)
#define WCN_BT_LINK_ABTPAGELOAD (1 << 28)
#define WCN_BT_LINK_ABTPAGESTARTVALUE (1 << 30)
#define WCN_BT_LINK_ABTPAGEEN (1 << 31)

// edrcntl_nwinsize
#define WCN_BT_LINK_RXGRD_TIMEOUT(n) (((n)&0x3ff) << 0)
#define WCN_BT_LINK_RX_SWAP (1 << 13)
#define WCN_BT_LINK_TX_SWAP (1 << 14)
#define WCN_BT_LINK_RXGUARDDSB (1 << 15)
#define WCN_BT_LINK_EDRBCAST (1 << 16)
#define WCN_BT_LINK_NWINSIZE(n) (((n)&0xff) << 24)

// et_currentrxdescptr
#define WCN_BT_LINK_CURRENTRXDESCPTR(n) (((n)&0x7fff) << 0)
#define WCN_BT_LINK_ETPTR(n) (((n)&0xffff) << 16)

// deepslcntl
#define WCN_BT_LINK_OSC_SLEEP_EN (1 << 0)
#define WCN_BT_LINK_RADIO_SLEEP_EN (1 << 1)
#define WCN_BT_LINK_DEEP_SLEEP_ON (1 << 2)
#define WCN_BT_LINK_DEEP_SLEEP_CORR_EN (1 << 3)
#define WCN_BT_LINK_SOFT_WAKEUP_REQ (1 << 4)
#define WCN_BT_LINK_DEEP_SLEEP_STAT (1 << 15)
#define WCN_BT_LINK_EXT_ACTIVITY_WAKEUP_EN (1 << 28)
#define WCN_BT_LINK_EXT_LOW_WAKEUP_EN (1 << 29)
#define WCN_BT_LINK_EXT_HIGH_WAKEUP_EN (1 << 30)
#define WCN_BT_LINK_EXTWKUPDSB (1 << 31)

// deepslwkup
#define WCN_BT_LINK_DEEPSLTIME(n) (((n)&0xffffffff) << 0)

// deepslstat
#define WCN_BT_LINK_DEEPSLDUR(n) (((n)&0xffffffff) << 0)

// enbpreset
#define WCN_BT_LINK_TWRM(n) (((n)&0x3ff) << 0)
#define WCN_BT_LINK_TWOSC(n) (((n)&0x7ff) << 10)
#define WCN_BT_LINK_TWEXT(n) (((n)&0x7ff) << 21)

// finecntcorr
#define WCN_BT_LINK_FINECNTCORR(n) (((n)&0x3ff) << 0)

// clkncntcorr
#define WCN_BT_LINK_CLKNCNTCORR(n) (((n)&0xfffffff) << 0)
#define WCN_BT_LINK_ABS_DELTA (1 << 31)

// tokencntl
#define WCN_BT_LINK_OBSERVER_EN (1 << 0)
#define WCN_BT_LINK_OBSERVER_ACTIVE (1 << 1)
#define WCN_BT_LINK_ARBITER_EN (1 << 2)
#define WCN_BT_LINK_ARBITER_BYPASS (1 << 3)
#define WCN_BT_LINK_TOKEN_RXWIN(n) (((n)&0x7f) << 8)
#define WCN_BT_LINK_TOKEN_QUICK_SYNC (1 << 15)
#define WCN_BT_LINK_TOKEN_RX_DELAY(n) (((n)&0x3f) << 16)
#define WCN_BT_LINK_TOKEN_TX_DELAY(n) (((n)&0x3f) << 24)

// diagcntl
#define WCN_BT_LINK_DIAG0(n) (((n)&0x3f) << 0)
#define WCN_BT_LINK_DIAG0_EN (1 << 7)
#define WCN_BT_LINK_DIAG1(n) (((n)&0x3f) << 8)
#define WCN_BT_LINK_DIAG1_EN (1 << 15)
#define WCN_BT_LINK_DIAG2(n) (((n)&0x3f) << 16)
#define WCN_BT_LINK_DIAG2_EN (1 << 23)
#define WCN_BT_LINK_DIAG3(n) (((n)&0x3f) << 24)
#define WCN_BT_LINK_DIAG3_EN (1 << 31)

// diagstat
#define WCN_BT_LINK_DIAG0STAT(n) (((n)&0xff) << 0)
#define WCN_BT_LINK_DIAG1STAT(n) (((n)&0xff) << 8)
#define WCN_BT_LINK_DIAG2STAT(n) (((n)&0xff) << 16)
#define WCN_BT_LINK_DIAG3STAT(n) (((n)&0xff) << 24)

// debugaddmax
#define WCN_BT_LINK_EM_ADDMAX(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_REG_ADDMAX(n) (((n)&0xffff) << 16)

// debugaddmin
#define WCN_BT_LINK_EM_ADDMIN(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_REG_ADDMIN(n) (((n)&0xffff) << 16)

// errortypestat
#define WCN_BT_LINK_TXCRYPT_ERROR (1 << 0)
#define WCN_BT_LINK_RXCRYPT_ERROR (1 << 1)
#define WCN_BT_LINK_CRYPTMODE_ERROR (1 << 2)
#define WCN_BT_LINK_PKTCNTL_EMACC_ERROR (1 << 3)
#define WCN_BT_LINK_RADIOCNTL_EMACC_ERROR (1 << 4)
#define WCN_BT_LINK_AUDIO_EMACC_ERROR (1 << 5)
#define WCN_BT_LINK_PCM_EMACC_ERROR (1 << 6)
#define WCN_BT_LINK_FRM_SCHDL_EMACC_ERROR (1 << 7)
#define WCN_BT_LINK_MWSWCI_EMACC_ERROR (1 << 8)
#define WCN_BT_LINK_FRM_SCHDL_ENTRY_ERROR (1 << 9)
#define WCN_BT_LINK_FRM_SCHDL_APFM_ERROR (1 << 10)
#define WCN_BT_LINK_FRM_CNTL_APFM_ERROR (1 << 11)
#define WCN_BT_LINK_FRM_CNTL_EMACC_ERROR (1 << 12)
#define WCN_BT_LINK_FRM_CNTL_TIMER_ERROR (1 << 13)
#define WCN_BT_LINK_HOPUNDERRUN_ERROR (1 << 14)
#define WCN_BT_LINK_CHMAP_ERROR (1 << 15)
#define WCN_BT_LINK_CSFORMAT_ERROR (1 << 16)
#define WCN_BT_LINK_CSATTNB_ERROR (1 << 17)
#define WCN_BT_LINK_TXDESC_EMPTY_ERROR (1 << 18)
#define WCN_BT_LINK_RXDESC_EMPTY_ERROR (1 << 19)
#define WCN_BT_LINK_TXBUF_PTR_ERROR (1 << 20)
#define WCN_BT_LINK_RXBUF_PTR_ERROR (1 << 21)

// swprofiling
#define WCN_BT_LINK_SWPROF(n) (((n)&0xffffffff) << 0)

// radiocntl0
#define WCN_BT_LINK_SPIGO (1 << 0)
#define WCN_BT_LINK_SPICOMP (1 << 1)
#define WCN_BT_LINK_SPIFREQ(n) (((n)&0x3) << 5)
#define WCN_BT_LINK_FORCEAGC_EN (1 << 21)
#define WCN_BT_LINK_DPCORR_EN (1 << 22)
#define WCN_BT_LINK_SYNC_PULSE_MODE (1 << 23)

// radiocntl1
#define WCN_BT_LINK_SPIPTR(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_XRFSEL(n) (((n)&0x1f) << 16)

// radiocntl2
#define WCN_BT_LINK_FORCEAGC_LENGTH(n) (((n)&0xfff) << 0)
#define WCN_BT_LINK_SYNC_POSITION(n) (((n)&0x7f) << 16)
#define WCN_BT_LINK_TX_DELAY(n) (((n)&0xff) << 24)

// radiocntl3
#define WCN_BT_LINK_FREQTABLE_PTR(n) (((n)&0xffff) << 0)

// radiopwrupdn
#define WCN_BT_LINK_TXPWRUPCT(n) (((n)&0xff) << 0)
#define WCN_BT_LINK_TXPWRDNCT(n) (((n)&0x1f) << 8)
#define WCN_BT_LINK_RXPWRUPCT(n) (((n)&0xff) << 16)
#define WCN_BT_LINK_RTRIP_DELAY(n) (((n)&0x7f) << 24)

// txmicval
#define WCN_BT_LINK_TXMICVAL(n) (((n)&0xffffffff) << 0)

// rxmicval
#define WCN_BT_LINK_RXMICVAL(n) (((n)&0xffffffff) << 0)

// e0ptr
#define WCN_BT_LINK_E0PTR(n) (((n)&0xffff) << 0)

// rftestcntl
#define WCN_BT_LINK_TXPLDSRC (1 << 0)
#define WCN_BT_LINK_PRBSTYPE (1 << 1)
#define WCN_BT_LINK_INFINITETX (1 << 2)
#define WCN_BT_LINK_INFINITE_ENTRY (1 << 3)
#define WCN_BT_LINK_INFINITERX (1 << 7)
#define WCN_BT_LINK_HERRREN (1 << 16)
#define WCN_BT_LINK_SSERRREN (1 << 17)

// rftestfreq
#define WCN_BT_LINK_TXFREQ(n) (((n)&0x7f) << 0)
#define WCN_BT_LINK_RXFREQ(n) (((n)&0x7f) << 8)
#define WCN_BT_LINK_TESTMODEEN (1 << 16)
#define WCN_BT_LINK_DIRECTLOPBACKEN (1 << 17)

// timgencntl
#define WCN_BT_LINK_PREFETCH_TIME(n) (((n)&0x1ff) << 0)
#define WCN_BT_LINK_PREFETCHABORT_TIME(n) (((n)&0x3ff) << 16)
#define WCN_BT_LINK_APFM_EN (1 << 31)

// grosstimtgt
#define WCN_BT_LINK_GROSSTARGET(n) (((n)&0x7fffff) << 0)

// finetimtgt
#define WCN_BT_LINK_FINETARGET(n) (((n)&0x7ffffff) << 0)

// sketclkn
#define WCN_BT_LINK_SKETCLKN(n) (((n)&0xfffffff) << 0)

// pcacntl0
#define WCN_BT_LINK_PHASE_SHIFT_EN (1 << 0)
#define WCN_BT_LINK_SYNC_SOURCE (1 << 1)
#define WCN_BT_LINK_FRSYNC_POL (1 << 2)
#define WCN_BT_LINK_BLINDCORR_EN (1 << 3)
#define WCN_BT_LINK_CORR_STEP(n) (((n)&0xf) << 4)
#define WCN_BT_LINK_SLVLBL(n) (((n)&0x1f) << 8)
#define WCN_BT_LINK_TARGET_OFFSET(n) (((n)&0x7ff) << 16)

// pcacntl1
#define WCN_BT_LINK_CLOCK_SHIFT(n) (((n)&0x7ff) << 0)
#define WCN_BT_LINK_CLOCK_SHIFT_EN (1 << 12)
#define WCN_BT_LINK_CORR_INTERVAL(n) (((n)&0xff) << 16)

// pcastat
#define WCN_BT_LINK_MOMENT_OFFSET(n) (((n)&0x7ff) << 0)
#define WCN_BT_LINK_SHIFT_PHASE(n) (((n)&0x7ff) << 16)

// escochancntl0
#define WCN_BT_LINK_TESCO0(n) (((n)&0xff) << 0)
#define WCN_BT_LINK_INTDELAY0(n) (((n)&0x1f) << 8)
#define WCN_BT_LINK_ITMODE0 (1 << 13)
#define WCN_BT_LINK_ESCOCHANEN0 (1 << 14)
#define WCN_BT_LINK_ESCOCHANSWEN0 (1 << 15)
#define WCN_BT_LINK_TOG0 (1 << 31)

// escomutecntl0
#define WCN_BT_LINK_MUTEPATT0(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_INVL0_0(n) (((n)&0x3) << 16)
#define WCN_BT_LINK_INVL0_1(n) (((n)&0x3) << 18)
#define WCN_BT_LINK_MUTE_SOURCE0 (1 << 22)
#define WCN_BT_LINK_MUTE_SINK0 (1 << 23)

// escocurrenttxptr0
#define WCN_BT_LINK_ESCO0PTRTX0(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_ESCO0PTRTX1(n) (((n)&0xffff) << 16)

// escocurrentrxptr0
#define WCN_BT_LINK_ESCO0PTRRX0(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_ESCO0PTRRX1(n) (((n)&0xffff) << 16)

// escoltcntl0
#define WCN_BT_LINK_SYNLTADDR0(n) (((n)&0x7) << 0)
#define WCN_BT_LINK_SYNTYPE0 (1 << 3)
#define WCN_BT_LINK_ESCOEDRTX0 (1 << 4)
#define WCN_BT_LINK_ESCOEDRRX0 (1 << 5)
#define WCN_BT_LINK_RETXNB0(n) (((n)&0xff) << 16)

// escotrcntl0
#define WCN_BT_LINK_RXTYPE0(n) (((n)&0xf) << 0)
#define WCN_BT_LINK_RXLEN0(n) (((n)&0x3ff) << 4)
#define WCN_BT_LINK_TXTYPE0(n) (((n)&0xf) << 16)
#define WCN_BT_LINK_TXLEN0(n) (((n)&0x3ff) << 20)
#define WCN_BT_LINK_TXSEQN0 (1 << 31)

// escodaycnt0
#define WCN_BT_LINK_DAYCOUNTER0(n) (((n)&0x7ff) << 0)

// escochancntl1
#define WCN_BT_LINK_TESCO1(n) (((n)&0xff) << 0)
#define WCN_BT_LINK_INTDELAY1(n) (((n)&0x1f) << 8)
#define WCN_BT_LINK_ITMODE1 (1 << 13)
#define WCN_BT_LINK_ESCOCHANEN1 (1 << 14)
#define WCN_BT_LINK_ESCOCHANSWEN1 (1 << 15)
#define WCN_BT_LINK_TOG1 (1 << 31)

// escomutecntl1
#define WCN_BT_LINK_MUTEPATT1(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_INVL1_0(n) (((n)&0x3) << 16)
#define WCN_BT_LINK_INVL1_1(n) (((n)&0x3) << 18)
#define WCN_BT_LINK_MUTE_SOURCE1 (1 << 22)
#define WCN_BT_LINK_MUTE_SINK1 (1 << 23)

// escocurrenttxptr1
#define WCN_BT_LINK_ESCO1PTRTX0(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_ESCO1PTRTX1(n) (((n)&0xffff) << 16)

// escocurrentrxptr1
#define WCN_BT_LINK_ESCO1PTRRX0(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_ESCO1PTRRX1(n) (((n)&0xffff) << 16)

// escoltcntl1
#define WCN_BT_LINK_SYNTADDR1(n) (((n)&0x7) << 0)
#define WCN_BT_LINK_SYNTYPE1 (1 << 3)
#define WCN_BT_LINK_ESCOEDRTX1 (1 << 4)
#define WCN_BT_LINK_ESCOEDRRX1 (1 << 5)
#define WCN_BT_LINK_RETXNB1(n) (((n)&0xff) << 16)

// escotrcntl1
#define WCN_BT_LINK_RXTYPE1(n) (((n)&0xf) << 0)
#define WCN_BT_LINK_RXLEN1(n) (((n)&0x3ff) << 4)
#define WCN_BT_LINK_TXTYPE1(n) (((n)&0xf) << 16)
#define WCN_BT_LINK_TXLEN1(n) (((n)&0x3ff) << 20)
#define WCN_BT_LINK_TXSEQN1 (1 << 31)

// escodaycnt1
#define WCN_BT_LINK_DAYCOUNTER1(n) (((n)&0x7ff) << 0)

// escochancntl2
#define WCN_BT_LINK_TESCO2(n) (((n)&0xff) << 0)
#define WCN_BT_LINK_INTDELAY2(n) (((n)&0x1f) << 8)
#define WCN_BT_LINK_ITMODE2 (1 << 13)
#define WCN_BT_LINK_ESCOCHANEN2 (1 << 14)
#define WCN_BT_LINK_ESCOCHANSWEN2 (1 << 15)
#define WCN_BT_LINK_TOG2 (1 << 31)

// escomutecntl2
#define WCN_BT_LINK_MUTEPATT2(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_INVL2_0(n) (((n)&0x3) << 16)
#define WCN_BT_LINK_INVL2_1(n) (((n)&0x3) << 18)
#define WCN_BT_LINK_MUTE_SOURCE2 (1 << 22)
#define WCN_BT_LINK_MUTE_SINK2 (1 << 23)

// escocurrenttxptr2
#define WCN_BT_LINK_ESCO2PTRTX0(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_ESCO2PTRTX1(n) (((n)&0xffff) << 16)

// escocurrentrxptr2
#define WCN_BT_LINK_ESCO2PTRRX0(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_ESCO2PTRRX1(n) (((n)&0xffff) << 16)

// escoltcntl2
#define WCN_BT_LINK_SYNTADDR2(n) (((n)&0x7) << 0)
#define WCN_BT_LINK_SYNTYPE2 (1 << 3)
#define WCN_BT_LINK_ESCOEDRTX2 (1 << 4)
#define WCN_BT_LINK_ESCOEDRRX2 (1 << 5)
#define WCN_BT_LINK_RETXNB2(n) (((n)&0xff) << 16)

// escotrcntl2
#define WCN_BT_LINK_RXTYPE2(n) (((n)&0xf) << 0)
#define WCN_BT_LINK_RXLEN2(n) (((n)&0x3ff) << 4)
#define WCN_BT_LINK_TXTYPE2(n) (((n)&0xf) << 16)
#define WCN_BT_LINK_TXLEN2(n) (((n)&0x3ff) << 20)
#define WCN_BT_LINK_TXSEQN2 (1 << 31)

// escodaycnt2
#define WCN_BT_LINK_DAYCOUNTER2(n) (((n)&0x7ff) << 0)

// audiocntl0
#define WCN_BT_LINK_CVSD_BITORDER0 (1 << 0)
#define WCN_BT_LINK_CVSDEN0 (1 << 7)
#define WCN_BT_LINK_AULAW_CODE0(n) (((n)&0xf) << 8)
#define WCN_BT_LINK_AULAWEN0 (1 << 15)
#define WCN_BT_LINK_SAMPLE_TYPE0(n) (((n)&0x3) << 16)
#define WCN_BT_LINK_LINEAR_FORMAT0(n) (((n)&0x3) << 20)

// audiocntl1
#define WCN_BT_LINK_CVSD_BITORDER1 (1 << 0)
#define WCN_BT_LINK_CVSDEN1 (1 << 7)
#define WCN_BT_LINK_AULAW_CODE1(n) (((n)&0xf) << 8)
#define WCN_BT_LINK_AULAWEN1 (1 << 15)
#define WCN_BT_LINK_SAMPLE_TYPE1(n) (((n)&0x3) << 16)
#define WCN_BT_LINK_LINEAR_FORMAT1(n) (((n)&0x3) << 20)

// audiocntl2
#define WCN_BT_LINK_CVSD_BITORDER2 (1 << 0)
#define WCN_BT_LINK_CVSDEN2 (1 << 7)
#define WCN_BT_LINK_AULAW_CODE2(n) (((n)&0xf) << 8)
#define WCN_BT_LINK_AULAWEN2 (1 << 15)
#define WCN_BT_LINK_SAMPLE_TYPE2(n) (((n)&0x3) << 16)
#define WCN_BT_LINK_LINEAR_FORMAT2(n) (((n)&0x3) << 20)

// pcmgencntl
#define WCN_BT_LINK_PCMEN (1 << 0)
#define WCN_BT_LINK_LRSWAP (1 << 1)
#define WCN_BT_LINK_BYTESWAP (1 << 2)
#define WCN_BT_LINK_MSTSLV (1 << 3)
#define WCN_BT_LINK_MONO_STEREO (1 << 4)
#define WCN_BT_LINK_MONO_LR_SEL (1 << 5)
#define WCN_BT_LINK_LOOPBACK (1 << 6)
#define WCN_BT_LINK_VXCHSEL(n) (((n)&0x3) << 8)

// pcmphyscntl0
#define WCN_BT_LINK_FSYNCSHP(n) (((n)&0x7) << 0)
#define WCN_BT_LINK_DOUTCFG(n) (((n)&0x3) << 4)
#define WCN_BT_LINK_LRCHPOL (1 << 8)
#define WCN_BT_LINK_PCM_IOM (1 << 9)
#define WCN_BT_LINK_LSB1ST (1 << 10)
#define WCN_BT_LINK_SAMPSZ (1 << 12)
#define WCN_BT_LINK_SAMPTYPE (1 << 13)
#define WCN_BT_LINK_SLOTNB(n) (((n)&0x7) << 16)
#define WCN_BT_LINK_FIRSTACTSLOT(n) (((n)&0x3) << 20)

// pcmphyscntl1
#define WCN_BT_LINK_PCMCLKVAL(n) (((n)&0x1ff) << 0)
#define WCN_BT_LINK_PCMCLKLIMIT(n) (((n)&0xff) << 16)
#define WCN_BT_LINK_CLKINV (1 << 31)

// pcmpadding
#define WCN_BT_LINK_LSAMPPAD(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_RSAMPPAD(n) (((n)&0xffff) << 16)

// pcmpllcntl0
#define WCN_BT_LINK_RL(n) (((n)&0xfffff) << 0)

// pcmpllcntl1
#define WCN_BT_LINK_A(n) (((n)&0x7ffff) << 0)
#define WCN_BT_LINK_OLC(n) (((n)&0x7ff) << 20)

// pcmpllcntl2
#define WCN_BT_LINK_W(n) (((n)&0x7ffff) << 0)

// pcmsourceptr
#define WCN_BT_LINK_PCMSOURCEPTR0(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_PCMSOURCEPTR1(n) (((n)&0xffff) << 16)

// pcmsinkptr
#define WCN_BT_LINK_PCMSINKPTR0(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_PCMSINKPTR1(n) (((n)&0xffff) << 16)

// bredrprioscharb
#define WCN_BT_LINK_BREDRMARGIN(n) (((n)&0xff) << 0)
#define WCN_BT_LINK_BREDRPRIOMODE (1 << 15)

// plcaddr01
#define WCN_BT_LINK_PLCBASEADDR0(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_PLCBASEADDR1(n) (((n)&0xffff) << 16)

// plcaddr23
#define WCN_BT_LINK_PLCBASEADDR2(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_PLCBASEADDR3(n) (((n)&0xffff) << 16)

// plcaddr45
#define WCN_BT_LINK_PLCBASEADDR4(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_PLCBASEADDR5(n) (((n)&0xffff) << 16)

// plcaddr67
#define WCN_BT_LINK_PLCBASEADDR6(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_PLCBASEADDR7(n) (((n)&0xffff) << 16)

// plcaddr89
#define WCN_BT_LINK_PLCBASEADDR8(n) (((n)&0xffff) << 0)
#define WCN_BT_LINK_PLCBASEADDR9(n) (((n)&0xffff) << 16)

// plcaddrb
#define WCN_BT_LINK_PLCBUFADDR(n) (((n)&0xffff) << 0)

// plcconf
#define WCN_BT_LINK_CONF_NO_PITCH_FIND_REF_THRESHOLD_DIST(n) (((n)&0x3fff) << 0)
#define WCN_BT_LINK_CONF_POOL_MEM_WORD_SWAP (1 << 14)
#define WCN_BT_LINK_CONF_PLC_EXCEPT (1 << 15)
#define WCN_BT_LINK_CONF_PLC_EN (1 << 16)
#define WCN_BT_LINK_CONF_PLC_TYPE (1 << 17)
#define WCN_BT_LINK_CONF_FRAME_MODE(n) (((n)&0x3) << 18)
#define WCN_BT_LINK_CONF_SCALING_MODE (1 << 20)
#define WCN_BT_LINK_CONF_DIAG0_SEL(n) (((n)&0x3) << 21)
#define WCN_BT_LINK_CONF_PLC_INT_MASK (1 << 23)
#define WCN_BT_LINK_CONF_PLC_CTRL_SW (1 << 24)
#define WCN_BT_LINK_CONF_PLC_START (1 << 25)
#define WCN_BT_LINK_PLC_FSM_STATE(n) (((n)&0xf) << 26)
#define WCN_BT_LINK_PLC_INT (1 << 30)
#define WCN_BT_LINK_PLC_INT_CLR (1 << 31)

#endif // _WCN_BT_LINK_H_
