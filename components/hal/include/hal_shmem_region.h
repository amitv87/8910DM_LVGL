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

#ifndef _HAL_SHMEM_REGION_H_
#define _HAL_SHMEM_REGION_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEM_MEM_INDEX_LIST_NAME "mem_index_list"
#define MEM_CP_BCPU_SM_NAME "cp_bcpu_sm"
#define MEM_CP_ASSERT_NAME "cp_assert_reg"
#define MEM_ZSP_ASSERT_NAME "zsp_assert_reg"
#define MEM_BCPU_ASSERT_NAME "bcpu_assert_reg"
#define MEM_DYNAMIC_NV_NAME "dynamic_nv.bin"
#define MEM_STATIC_NV_NAME "static_nv.bin"
#define MEM_PHY_NV_NAME "phy_nv.bin"
#define MEM_DYNAMIC_NV_2_NAME "dynamic_nv_2.bin"
#define MEM_STATIC_NV_2_NAME "static_nv_2.bin"
#define MEM_PHY_NV_2_NAME "phy_nv_2.bin"
#define MEM_PSM_NV_NAME "psm_nv.bin"
#define MEM_CFW_NV_NAME "cfw_nv.bin"
#define MEM_AT_NV_NAME "at_nv.bin"
#define MEM_SMS_NV_NAME "sms_nv.bin"
#define MEM_RF_NV_NAME "rf_nv.bin"
#define MEM_LTE_RF_CALIB_NAME "lte_rf_calib.bin"
#define MEM_GSM_RF_CALIB_NAME "gsm_rf_calib.bin"
#define MEM_GOLDEN_BOARD_NAME "golden_board.bin"
#define MEM_AUDIO_CALIB_NAME "audio_calib.bin"
#define MEM_RISCV_BIN_NAME "riscV.bin"
#define MEM_ZSP_BIN_NAME "zsp.bin"
#define MEM_BCPU_NBIOT_BIN_NAME "bcpu_nbiot.bin"
#define MEM_BCPU_GSM_BIN_NAME "bcpu_gsm.bin"
#define MEM_AUDIO_SM_NAME "audio_sm"
#define MEM_CP_ZSP_SM_NAME "cp_zsp_sm"
#define MEM_AP_CP_SM_NAME "ap_cp_sm"
#define MEM_CP_BIN_NAME "cp.bin"
#define MEM_AP_BIN_NAME "ap.bin"
#define MEM_CP_DEBUG_NAME "cp_debug"
#define MEM_MOS_DEBUG_NAME "dt_data"
#define MEM_CP_SECOND_DEBUG_NAME "cp_second_debug"
#define MEM_ZSP_DEBUG_NAME "zsp_debug"
#define MEM_RF_DEBUG_NAME "rf_debug"
#define MEM_AUDIO_DEBUG_NAME "audio_debug"
#define MEM_BCPU_ROM_NAME "bcpu_rom.bin"

#define HAL_SHARE_MEM_NAME_LEN (20)
#define HAL_SHARE_MEM_FLAG_CPNEED (1)
#define HAL_SHARE_MEM_FLAG_APRO (2)
#define HAL_SHARE_MEM_FLAG_LZMA (0x100)  // software LZMA
#define HAL_SHARE_MEM_FLAG_LZMA2 (0x200) // hardware LZMA, one file
#define HAL_SHARE_MEM_FLAG_LZMA3 (0x400) // hardware LZMA, multiple files

typedef struct
{
    char name[HAL_SHARE_MEM_NAME_LEN];
    uint32_t address;
    uint32_t size;
    uint32_t flags;
} halShmemRegion_t;

bool halShareMemRegionLoad(void);
const halShmemRegion_t *halShmemGetRegion(const char *name);
const halShmemRegion_t *halShmemGetRegionByAddr(uint32_t address);
size_t halShmemGetAllRegions(const halShmemRegion_t **regions);
void halShmemUpdateRamAccess(void);

#ifdef __cplusplus
}
#endif
#endif
