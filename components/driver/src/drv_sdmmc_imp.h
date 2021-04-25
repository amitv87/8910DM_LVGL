/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _DRV_SDMMC_IMP_H_
#define _DRV_SDMMC_IMP_H_

#include "osi_compiler.h"
#include "drv_names.h"
#include "drv_config.h"
#include "osi_api.h"
#include "osi_clock.h"
#include "hwregs.h"
#include "drv_ifc.h"
#include <assert.h>

#include "quec_proj_config.h"

OSI_EXTERN_C_BEGIN

#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
#define SDMMC_CLOCK_SOURCE CONFIG_DEFAULT_SYSAXI_FREQ
#endif

#define SDMMC_CLK_FREQ_IDENTIFY (400000)
#define SDMMC_CLK_FREQ_SD CONFIG_SDMMC_CLK_FREQ_SD
#define SDMMC_CLK_FREQ_SDHC CONFIG_SDMMC_CLK_FREQ_SDHC

#define SDMMC_CMD8_CHECK_PATTERN 0xaa
#define SDMMC_CMD8_VOLTAGE_SEL (0x1 << 8)

#define SDMMC_READ_TIMEOUT_MS (5000)
#define SDMMC_WRITE_TIMEOUT_MS (5000)
#define SDMMC_CMD_TIMEOUT_US (10000)
#define SDMMC_RESP_TIMEOUT_US (10000)
#define SDMMC_TRAN_TIMEOUT_US (2000000)
#define SDMMC_SDMMC_OCR_TIMEOUT_US (1000000) // the card is supposed to answer within 1s

#define SDMMC_CSD_VERSION_1 0
#define SDMMC_CSD_VERSION_2 1
#define SDMMC_CSD_VERSION_3 2

typedef struct
{
    uint32_t v[4];
} drvSdmmcResp_t;

/**
 * sdmmcCardState_t
 *
 * The state of the card when receiving the command. If the command execution
 * causes a state change, it will be visible to the host in the response to
 * the next command. The four bits are interpreted as a binary coded number
 * between 0 and 15.
 */
typedef enum
{
    SDMMC_CARD_STATE_IDLE = 0,
    SDMMC_CARD_STATE_READY = 1,
    SDMMC_CARD_STATE_IDENT = 2,
    SDMMC_CARD_STATE_STBY = 3,
    SDMMC_CARD_STATE_TRAN = 4,
    SDMMC_CARD_STATE_DATA = 5,
    SDMMC_CARD_STATE_RCV = 6,
    SDMMC_CARD_STATE_PRG = 7,
    SDMMC_CARD_STATE_DIS = 8
} sdmmcCardState_t;

typedef union {
    uint32_t v;
    struct
    {
        uint32_t vdd_win : 24;        // [23:0]
        uint32_t s18a : 1;            // [24]
        uint32_t __16_15 : 2;         // [26:25]
        uint32_t co2t : 1;            // [27]
        uint32_t __28_28 : 1;         // [28]
        uint32_t uhs2 : 1;            // [29]
        uint32_t ccs : 1;             // [30]
        uint32_t power_on_finish : 1; // [31]
    } b;
} sdmmcOcr_t;

/**
 * sdmmcCardStatus_t
 *
 * Card status as returned by R1 reponses (spec V2 pdf p.)
 */
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __2_0 : 3;                 // [0:2]
        uint32_t ake_seq_error : 1;         // [3]
        uint32_t __4_4 : 1;                 // [4]
        uint32_t app_cmd : 1;               // [5]
        uint32_t fx_event : 1;              // [6]
        uint32_t __7_7 : 1;                 // [7]
        uint32_t ready_for_data : 1;        // [8]
        sdmmcCardState_t current_state : 4; // [9:12]
        uint32_t erase_reset : 1;           // [13]
        uint32_t card_ecc_disabled : 1;     // [14]
        uint32_t wp_erase_skip : 1;         // [15]
        uint32_t csd_overwrite : 1;         // [16]
        uint32_t __18_17 : 2;               // [17:18]
        uint32_t error : 1;                 // [19]
        uint32_t cc_error : 1;              // [20]
        uint32_t card_ecc_failed : 1;       // [21]
        uint32_t illegal_command : 1;       // [22]
        uint32_t com_crc_error : 1;         // [23]
        uint32_t lock_unlock_fail : 1;      // [24]
        uint32_t card_is_locked : 1;        // [25]
        uint32_t wp_violation : 1;          // [26]
        uint32_t erase_param : 1;           // [27]
        uint32_t erase_seq_error : 1;       // [28]
        uint32_t block_len_error : 1;       // [29]
        uint32_t address_error : 1;         // [30]
        uint32_t out_of_range : 1;          // [31]
    } b;
} sdmmcCardStatus_SD_t;


//重新定义一个符合EMMC 5.1协议的sdmmcCardStatus_t

typedef union {
    uint32_t v;
    struct
    {
        uint32_t __4_0 : 5;                 // [0:4]
        uint32_t app_cmd : 1;               // [5]
        uint32_t exception_event : 1;       // [6]
        uint32_t switch_error : 1;          // [7]
        uint32_t ready_for_data : 1;        // [8]
        sdmmcCardState_t current_state : 4; // [9:12]
        uint32_t erase_reset : 1;           // [13]
        uint32_t __14_14 : 1;               // [14]
        uint32_t wp_erase_skip : 1;         // [15]
        uint32_t csd_overwrite : 1;         // [16]
        uint32_t obsolete : 2;              // [17:18]
        uint32_t error : 1;                 // [19]
        uint32_t cc_error : 1;              // [20]
        uint32_t device_ecc_failed : 1;     // [21]
        uint32_t illegal_command : 1;       // [22]
        uint32_t com_crc_error : 1;         // [23]
        uint32_t lock_unlock_failed : 1;    // [24]
        uint32_t device_is_locked : 1;      // [25]
        uint32_t wp_violation : 1;          // [26]
        uint32_t erase_param : 1;           // [27]
        uint32_t erase_seq_error : 1;       // [28]
        uint32_t block_len_error : 1;       // [29]
        uint32_t address_misalign : 1;      // [30]
        uint32_t address_out_of_range : 1;  // [31]
    } b;
} sdmmcCardStatus_t;

#else
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __2_0 : 3;                 // [0:2]
        uint32_t ake_seq_error : 1;         // [3]
        uint32_t __4_4 : 1;                 // [4]
        uint32_t app_cmd : 1;               // [5]
        uint32_t fx_event : 1;              // [6]
        uint32_t __7_7 : 1;                 // [7]
        uint32_t ready_for_data : 1;        // [8]
        sdmmcCardState_t current_state : 4; // [9:12]
        uint32_t erase_reset : 1;           // [13]
        uint32_t card_ecc_disabled : 1;     // [14]
        uint32_t wp_erase_skip : 1;         // [15]
        uint32_t csd_overwrite : 1;         // [16]
        uint32_t __18_17 : 2;               // [17:18]
        uint32_t error : 1;                 // [19]
        uint32_t cc_error : 1;              // [20]
        uint32_t card_ecc_failed : 1;       // [21]
        uint32_t illegal_command : 1;       // [22]
        uint32_t com_crc_error : 1;         // [23]
        uint32_t lock_unlock_fail : 1;      // [24]
        uint32_t card_is_locked : 1;        // [25]
        uint32_t wp_violation : 1;          // [26]
        uint32_t erase_param : 1;           // [27]
        uint32_t erase_seq_error : 1;       // [28]
        uint32_t block_len_error : 1;       // [29]
        uint32_t address_error : 1;         // [30]
        uint32_t out_of_range : 1;          // [31]
    } b;
} sdmmcCardStatus_t;
#endif

typedef struct __attribute__((packed, aligned(1)))
{
    unsigned __0__0 : 1;  // [0:0]
    unsigned crc : 7;     // [7:1]
    unsigned mdt : 12;    // [19:8]
    unsigned __23_20 : 4; // [23:20]
    unsigned psn : 32;    // [55:24]
    unsigned prv : 8;     // [63:56]
    uint64_t pnm : 40;    // [103:64]
    unsigned oid : 16;    // [119:104]
    unsigned mid : 8;     // [127:120]
} sdmmcCid_t;

typedef struct __attribute__((packed, aligned(1)))
{
    unsigned __0_0 : 1;              // [0:0]
    unsigned crc : 7;                // [7:1]
    unsigned __9_8 : 2;              // [9:8]
    unsigned file_format : 2;        // [11:10]
    unsigned tmp_write_protect : 1;  // [12:12]
    unsigned perm_write_protect : 1; // [13:13]
    unsigned copy : 1;               // [14:14]
    unsigned file_format_grp : 1;    // [15:15]
    unsigned __20_16 : 5;            // [20:16]
    unsigned write_bl_partial : 1;   // [21:21]
    unsigned write_bl_len : 4;       // [25:22]
    unsigned r2w_factor : 3;         // [28:26]
    unsigned __30_29 : 2;            // [30:29]
    unsigned wr_grp_enable : 1;      // [31:31]
    unsigned wr_grp_size : 7;        // [38:32]
    unsigned sector_size : 7;        // [45:39]
    unsigned erase_blk_en : 1;       // [46:46]
    unsigned c_size_mult : 3;        // [49:47]
    unsigned vdd_w_curr_max : 3;     // [52:50]
    unsigned vdd_w_curr_min : 3;     // [55:53]
    unsigned vdd_r_curr_max : 3;     // [58:56]
    unsigned vdd_r_curr_min : 3;     // [61:59]
    unsigned c_size : 12;            // [73:62]
    unsigned __75_74 : 2;            // [75:74]
    unsigned dsr_imp : 1;            // [76:76]
    unsigned read_blk_misalign : 1;  // [77:77]
    unsigned write_blk_misalign : 1; // [78:78]
    unsigned read_bl_partial : 1;    // [79:79]
    unsigned read_bl_len : 4;        // [83:80]
    unsigned ccc : 12;               // [95:84]
    unsigned trans_speed : 8;        // [103:96]
    unsigned nsac : 8;               // [111:104]
    unsigned taac : 8;               // [119:112]
    unsigned __125_120 : 6;          // [125:120]
    unsigned csd_struct : 2;         // [127:126]
} sdmmcCsdV1_t;

typedef struct __attribute__((packed, aligned(1)))
{
    unsigned __0_0 : 1;              // [0:0]
    unsigned crc : 7;                // [7:1]
    unsigned __9_8 : 2;              // [9:8]
    unsigned file_format : 2;        // [11:10]
    unsigned tmp_write_protect : 1;  // [12:12]
    unsigned perm_write_protect : 1; // [13:13]
    unsigned copy : 1;               // [14:14]
    unsigned file_format_grp : 1;    // [15:15]
    unsigned __20_16 : 5;            // [20:16]
    unsigned write_bl_partial : 1;   // [21:21]
    unsigned write_bl_len : 4;       // [25:22]
    unsigned r2w_factor : 3;         // [28:26]
    unsigned __30_29 : 2;            // [30:29]
    unsigned wr_grp_enable : 1;      // [31:31]
    unsigned wr_grp_size : 7;        // [38:32]
    unsigned sector_size : 7;        // [45:39]
    unsigned erase_blk_en : 1;       // [46:46]
    unsigned __47_47 : 1;            // [47:47]
    unsigned c_size : 22;            // [69:48]
    unsigned __75_70 : 6;            // [75:70]
    unsigned dsr_imp : 1;            // [76:76]
    unsigned read_blk_misalign : 1;  // [77:77]
    unsigned write_blk_misalign : 1; // [78:78]
    unsigned read_bl_partial : 1;    // [79:79]
    unsigned read_bl_len : 4;        // [83:80]
    unsigned ccc : 12;               // [95:84]
    unsigned trans_speed : 8;        // [103:96]
    unsigned nsac : 8;               // [111:104]
    unsigned taac : 8;               // [119:112]
    unsigned __125_120 : 6;          // [125:120]
    unsigned csd_struct : 2;         // [127:126]
} sdmmcCsdV2_t;

typedef struct __attribute__((packed, aligned(1)))
{
    unsigned __0_0 : 1;              // [0:0]
    unsigned crc : 7;                // [7:1]
    unsigned __9_8 : 2;              // [9:8]
    unsigned file_format : 2;        // [11:10]
    unsigned tmp_write_protect : 1;  // [12:12]
    unsigned perm_write_protect : 1; // [13:13]
    unsigned copy : 1;               // [14:14]
    unsigned file_format_grp : 1;    // [15:15]
    unsigned __20_16 : 5;            // [20:16]
    unsigned write_bl_partial : 1;   // [21:21]
    unsigned write_bl_len : 4;       // [25:22]
    unsigned r2w_factor : 3;         // [28:26]
    unsigned __30_29 : 2;            // [30:29]
    unsigned wr_grp_enable : 1;      // [31:31]
    unsigned wr_grp_size : 7;        // [38:32]
    unsigned sector_size : 7;        // [45:39]
    unsigned erase_blk_en : 1;       // [46:46]
    unsigned __47_47 : 1;            // [47:47]
    unsigned c_size : 28;            // [75:48]
    unsigned dsr_imp : 1;            // [76:76]
    unsigned read_blk_misalign : 1;  // [77:77]
    unsigned write_blk_misalign : 1; // [78:78]
    unsigned read_bl_partial : 1;    // [79:79]
    unsigned read_bl_len : 4;        // [83:80]
    unsigned ccc : 12;               // [95:84]
    unsigned trans_speed : 8;        // [103:96]
    unsigned nsac : 8;               // [111:104]
    unsigned taac : 8;               // [119:112]
    unsigned __125_120 : 6;          // [125:120]
    unsigned csd_struct : 2;         // [127:126]
} sdmmcCsdV3_t;

#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
//符合EMMC 4.4和EMMC 5.1规范的CSD寄存器定义
typedef struct __attribute__((packed, aligned(1)))
{
    unsigned __0_0 : 1;              // [0:0]
    unsigned crc : 7;                // [7:1]
    unsigned ecc : 2;                // [9:8]
    unsigned file_format : 2;        // [11:10]
    unsigned tmp_write_protect : 1;  // [12:12]
    unsigned perm_write_protect : 1; // [13:13]
    unsigned copy : 1;               // [14:14]
    unsigned file_format_grp : 1;    // [15:15]
    unsigned content_prot_app : 1;   // [16:16]
    unsigned __20_17 : 4;            // [20:17]
    unsigned write_bl_partial : 1;   // [21:21]
    unsigned write_bl_len : 4;       // [25:22]
    unsigned r2w_factor : 3;         // [28:26]
    unsigned default_ecc : 2;        // [30:29]
    unsigned wp_grp_enable : 1;      // [31:31]
    unsigned wp_grp_size : 5;        // [36:32]
    unsigned erase_grp_mult : 5;     // [41:37]
    unsigned erase_grp_size : 5;     // [46:42]
    unsigned c_size_mult : 3;        // [49:47]
    unsigned vdd_w_curr_max : 3;     // [52:50]
    unsigned vdd_w_curr_min : 3;     // [55:53]
    unsigned vdd_r_curr_max : 3;     // [58:56]
    unsigned vdd_r_curr_min : 3;     // [61:59]
    unsigned c_size : 12;            // [73:62]
    unsigned __75_74 : 2;            // [75:74]
    unsigned dsr_imp : 1;            // [76:76]
    unsigned read_blk_misalign : 1;  // [77:77]
    unsigned write_blk_misalign : 1; // [78:78]
    unsigned read_bl_partial : 1;    // [79:79]
    unsigned read_bl_len : 4;        // [83:80]
    unsigned ccc : 12;               // [95:84]
    unsigned trans_speed : 8;        // [103:96]
    unsigned nsac : 8;               // [111:104]
    unsigned taac : 8;               // [119:112]
    unsigned __121_120 : 2;          // [121:120]
    unsigned spec_vers : 4;          // [125:122]
    unsigned csd_struct : 2;         // [127:126]
} sdmmcCsdV4_t;
#endif

typedef union {
    sdmmcCsdV1_t v1;
    sdmmcCsdV2_t v2;
    sdmmcCsdV3_t v3;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
    sdmmcCsdV4_t v4;
#endif

} sdmmcCsd_t;

#define CSD_FIELD(csd, f) (csd->v1.csd_struct == 0 ? csd->v1.f : csd->v1.csd_struct == 1 ? csd->v2.f : csd->v3.f)

static_assert(sizeof(sdmmcCsdV1_t) == 16, "sdmmcCsdV1_t is wrong");
static_assert(sizeof(sdmmcCsdV2_t) == 16, "sdmmcCsdV2_t is wrong");
static_assert(sizeof(sdmmcCsdV3_t) == 16, "sdmmcCsdV3_t is wrong");
static_assert(sizeof(sdmmcCsd_t) == 16, "sdmmcCsd_t is wrong");
static_assert(sizeof(sdmmcCid_t) == 16, "sdmmcCid_t is wrong");
static inline unsigned prvCsdStruct(sdmmcCsd_t *csd) { return csd->v1.csd_struct; }
static inline unsigned prvCsdTaac(sdmmcCsd_t *csd) { return CSD_FIELD(csd, taac); }
static inline unsigned prvCsdNsac(sdmmcCsd_t *csd) { return CSD_FIELD(csd, nsac); }
static inline unsigned prvCsdTransSpeed(sdmmcCsd_t *csd) { return CSD_FIELD(csd, trans_speed); }
static inline unsigned prvCsdCcc(sdmmcCsd_t *csd) { return CSD_FIELD(csd, ccc); }
static inline unsigned prvCsdReadBlLen(sdmmcCsd_t *csd) { return CSD_FIELD(csd, read_bl_len); }
static inline unsigned prvCsdReadBlPartial(sdmmcCsd_t *csd) { return CSD_FIELD(csd, read_bl_partial); }
static inline unsigned prvCsdWriteBlkMisalign(sdmmcCsd_t *csd) { return CSD_FIELD(csd, write_blk_misalign); }
static inline unsigned prvCsdReadBlkMisalign(sdmmcCsd_t *csd) { return CSD_FIELD(csd, read_blk_misalign); }
static inline unsigned prvCsdDsrImp(sdmmcCsd_t *csd) { return CSD_FIELD(csd, dsr_imp); }
static inline unsigned prvCsdCSize(sdmmcCsd_t *csd) { return CSD_FIELD(csd, c_size); }
static inline unsigned prvCsdVddRCurrMin(sdmmcCsd_t *csd) { return csd->v1.vdd_r_curr_min; }
static inline unsigned prvCsdVddRCurrMax(sdmmcCsd_t *csd) { return csd->v1.vdd_r_curr_max; }
static inline unsigned prvCsdVddWCurrMin(sdmmcCsd_t *csd) { return csd->v1.vdd_w_curr_min; }
static inline unsigned prvCsdVddWCurrMax(sdmmcCsd_t *csd) { return csd->v1.vdd_w_curr_max; }
static inline unsigned prvCsdCSizeMult(sdmmcCsd_t *csd) { return csd->v1.c_size_mult; }
static inline unsigned prvCsdEraseBlkEn(sdmmcCsd_t *csd) { return CSD_FIELD(csd, erase_blk_en); }
static inline unsigned prvCsdSectorSize(sdmmcCsd_t *csd) { return CSD_FIELD(csd, sector_size); }
static inline unsigned prvCsdWrGrpSize(sdmmcCsd_t *csd) { return CSD_FIELD(csd, wr_grp_size); }
static inline unsigned prvCsdWrGrpEnable(sdmmcCsd_t *csd) { return CSD_FIELD(csd, wr_grp_enable); }
static inline unsigned prvCsdR2wFactor(sdmmcCsd_t *csd) { return CSD_FIELD(csd, r2w_factor); }
static inline unsigned prvCsdWriteBlLen(sdmmcCsd_t *csd) { return CSD_FIELD(csd, write_bl_len); }
static inline unsigned prvCsdWriteBlPartial(sdmmcCsd_t *csd) { return CSD_FIELD(csd, write_bl_partial); }
static inline unsigned prvCsdFileFormatGrp(sdmmcCsd_t *csd) { return CSD_FIELD(csd, file_format_grp); }
static inline unsigned prvCsdCopy(sdmmcCsd_t *csd) { return CSD_FIELD(csd, copy); }
static inline unsigned prvCsdPermWriteProtect(sdmmcCsd_t *csd) { return CSD_FIELD(csd, perm_write_protect); }
static inline unsigned prvCsdTmpWriteProtect(sdmmcCsd_t *csd) { return CSD_FIELD(csd, tmp_write_protect); }
static inline unsigned prvCsdFileFormat(sdmmcCsd_t *csd) { return CSD_FIELD(csd, file_format_grp); }
static inline unsigned prvCsdCrc(sdmmcCsd_t *csd) { return CSD_FIELD(csd, crc); }

/**
 * SDMMC_ACMD_SEL
 * mark a command as application specific
 */
#define SDMMC_ACMD_SEL 0x80000000

/**
 * SDMMC_CMD_MASK
 *
 * Mask to get from a sdmmcCmd_t value the corresponding
 * command index
 */
#define SDMMC_CMD_MASK 0x3F

/**
 * sdmmcCmd_t
 * SD commands index
 */
 #ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
 typedef enum
 {
     SDMMC_CMD_GO_IDLE_STATE = 0,
     SDMMC_CMD_MMC_SEND_OP_COND = 1,
     SDMMC_CMD_ALL_SEND_CID = 2,
     SDMMC_CMD_SEND_RELATIVE_ADDR = 3,
     SDMMC_CMD_SET_DSR = 4,
     SDMMC_CMD_SWITCH = 6,
     SDMMC_CMD_SELECT_CARD = 7,
     // SDMMC_CMD_SEND_IF_COND = 8,
     SEND_EXT_CSD = 8,
     SDMMC_CMD_SEND_CSD = 9,
     SDMMC_CMD_STOP_TRANSMISSION = 12,
     SDMMC_CMD_SEND_STATUS = 13,
     SDMMC_CMD_SET_BLOCKLEN = 16,
     SDMMC_CMD_READ_SINGLE_BLOCK = 17,
     SDMMC_CMD_READ_MULT_BLOCK = 18,
     SDMMC_CMD_WRITE_SINGLE_BLOCK = 24,
     SDMMC_CMD_WRITE_MULT_BLOCK = 25,
     SDMMC_CMD_APP_CMD = 55,
     SDMMC_CMD_SET_BUS_WIDTH = (6 | SDMMC_ACMD_SEL),
     SDMMC_CMD_SEND_NUM_WR_BLOCKS = (22 | SDMMC_ACMD_SEL),
     SDMMC_CMD_SET_WR_BLK_COUNT = 23, //(23 | SDMMC_ACMD_SEL),
     SDMMC_CMD_SET_WR_BLK_COUNT_SD = (23 | SDMMC_ACMD_SEL),    
     SDMMC_CMD_SEND_OP_COND = (41 | SDMMC_ACMD_SEL)
 } sdmmcCmd_t;
#else
typedef enum
{
    SDMMC_CMD_GO_IDLE_STATE = 0,
    SDMMC_CMD_MMC_SEND_OP_COND = 1,
    SDMMC_CMD_ALL_SEND_CID = 2,
    SDMMC_CMD_SEND_RELATIVE_ADDR = 3,
    SDMMC_CMD_SET_DSR = 4,
    SDMMC_CMD_SWITCH = 6,
    SDMMC_CMD_SELECT_CARD = 7,
    SDMMC_CMD_SEND_IF_COND = 8,
    SDMMC_CMD_SEND_CSD = 9,
    SDMMC_CMD_STOP_TRANSMISSION = 12,
    SDMMC_CMD_SEND_STATUS = 13,
    SDMMC_CMD_SET_BLOCKLEN = 16,
    SDMMC_CMD_READ_SINGLE_BLOCK = 17,
    SDMMC_CMD_READ_MULT_BLOCK = 18,
    SDMMC_CMD_WRITE_SINGLE_BLOCK = 24,
    SDMMC_CMD_WRITE_MULT_BLOCK = 25,
    SDMMC_CMD_APP_CMD = 55,
    SDMMC_CMD_SET_BUS_WIDTH = (6 | SDMMC_ACMD_SEL),
    SDMMC_CMD_SEND_NUM_WR_BLOCKS = (22 | SDMMC_ACMD_SEL),
    SDMMC_CMD_SET_WR_BLK_COUNT = (23 | SDMMC_ACMD_SEL),
    SDMMC_CMD_SEND_OP_COND = (41 | SDMMC_ACMD_SEL)
} sdmmcCmd_t;
#endif
/**
 * sdmmcCheckStatus_t
 *
 * check register hwp->sdmmc_status different bit
 */
typedef enum
{
    SDMMC_CHECK_TRANSFER_DONE,
    SDMMC_CHECK_RESPONSE_OK,
    SDMMC_CHECK_CRC_OK,
    SDMMC_CHECK_DATA_OK,
} sdmmcCheckStatus_t;

/**
 * sdmmcDataBusWidth_t
 *
 * Cf spec v2 pdf p. 76 for ACMD6 argument
 * That type is used to describe how many data lines are used to transfer data
 * to and from the SD card.
 */
typedef enum
{
    SDMMC_DATA_BUS_WIDTH_1 = 0x0,
    SDMMC_DATA_BUS_WIDTH_4 = 0x2,
    SDMMC_DATA_BUS_WIDTH_8 = 0x4
} sdmmcDataBusWidth_t;

typedef enum
{
    SDMMC_RESP_NONE = 0,
    SDMMC_RESP_FETCH = 1,
    SDMMC_RESP_CHECK = 2,
} sdmmcRespOption_t;

/**
 * Maximum block count per transfter are affected by 2 registers:
 * 1. sdmmc_block_cnt_reg: 16bits
 * 2. ifc.tc: 23bits, 14bits when block size is 512
 */
#define SDMMC_MAX_BLOCK_PER_XFER (0x3fff)

struct drvSdmmc
{
    uint32_t name;
    HWP_SDMMC_T *hwp;
    drvIfcChannel_t rx_ifc;
    drvIfcChannel_t tx_ifc;
    uint32_t irqn;

    drvSdmmcResp_t resp;
    sdmmcCsd_t csd;
    sdmmcCid_t cid;
    sdmmcOcr_t ocr;
    uint32_t rca;
    uint32_t dsr;
    uint8_t clk_adj;
    uint8_t clk_inv;
    bool card_is_sdhc;
    sdmmcDataBusWidth_t bus_width;
    uint32_t max_clk;
    uint32_t master_clk;

    bool opened;
    bool rx_enabled;
    bool tx_enabled;

    uint32_t block_count;
    uint32_t block_size;
    osiClockConstrainRegistry_t clk_constrain;
    osiPmSource_t *pm_source;
    osiSemaphore_t *tx_done_sema;
    osiSemaphore_t *rx_done_sema;
    osiMutex_t *lock;
    struct
    {
        unsigned sdmmc_trans_speed_reg;
    } pm_ctx;
};

OSI_EXTERN_C_END
#endif
