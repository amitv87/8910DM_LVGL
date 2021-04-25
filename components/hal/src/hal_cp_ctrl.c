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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('C', 'P', 'L', 'D')

#include "osi_api.h"
#include "osi_log.h"
#include "hal_cp_ctrl.h"
#include "hal_shmem_region.h"
#include "hal_chip.h"
#include "hwregs.h"
#include "hal_lzma.h"
#include "vfs.h"
#include "calclib/unlzma.h"
#include "calclib/crc32.h"
#include "hal_iomux.h"
#include "nvm.h"
#include "drv_md_ipc.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

// #define DEBUG_NOT_START_CP

#define SUPPORT_SOFTLZMA
#undef SUPPORT_HARDLZMA2
#define SUPPORT_HARDLZMA3

#define MAX_FILENAME_LEN (128)
#define MAX_FILE_SIZE (128 * 1024)
#define MAX_BLOCK_SIZE (128 * 1024)

#define CP_JUMP_MAGIC_REG (hwp_idle->idle_res2)
#define CP_JUMP_ADDRESS_REG (hwp_idle->idle_res3)
#define CP_PSM_WAKE_TIME_REG (hwp_sysCtrl->cfg_reserve11) // in 10ms
#define CP_BOOT_MODE_REG (hwp_mailbox->sysmail30)

#define CP_WAKEUP_JUMP_MAGIC 0xC8DACAFE
#define CP_NORMAL_BOOT_MODE (0)
#define CP_CALIBRATION_BOOT_MODE (2)
#define CP_BBAT_BOOT_MODE (3)
#define CP_PSM_WAKE_BOOT_MODE (4)
#define CP_CALIBRATION_POST_MODE (5)
#define MODEMNV_DIR CONFIG_FS_MODEM_MOUNT_POINT "/nvm"
#define DELTANV_FMAME MODEMNV_DIR "/ims_delta_nv.bin"

#ifdef SUPPORT_SOFTLZMA
static bool _loadRegionLzma(const char *name, void *output)
{
    long out_size;
    if (lzmaDecompressFileName(name, output, &out_size) < 0)
        return false;

    uint32_t crc = crc32Calc(output, out_size);
    OSI_LOGI(0, "lzma out_size/%d address/%p crc/0x%08x",
             out_size, output, crc);

    osiDCacheClean(output, out_size);
    return true;
}
#endif

#ifdef SUPPORT_HARDLZMA2
static bool _loadRegionLzma2(const char *name, void *output)
{
    return false; // TODO
}
#endif

#ifdef SUPPORT_HARDLZMA3
static bool _loadRegionLzma3(const char *name, void *output)
{
    void *file_buf = malloc(MAX_FILE_SIZE);
    if (file_buf == NULL)
        return false;

    void *dest_buf = memalign(MAX_BLOCK_SIZE, 32);
    if (dest_buf == NULL)
    {
        free(file_buf);
        return false;
    }

    char *ps = (char *)output;
    unsigned out_size = 0;
    char part_name[MAX_FILENAME_LEN];
    for (int n = 0;; n++)
    {
        sprintf(part_name, "%s.%03d", name, n);
        int fd = vfs_open(part_name, O_RDONLY);
        if (fd < 0) // it is not error if file not exist
            break;

        int fsize = vfs_read(fd, file_buf, MAX_FILE_SIZE);
        OSI_LOGXD(OSI_LOGPAR_SSI, 0, "cp: %s %s %d", name, part_name, fsize);
        vfs_close(fd);

        if (fsize < 0)
            osiPanic();

        int dsize = halLzmaDecompressFile(file_buf, dest_buf);
        if (dsize < 0)
            osiPanic();

        memcpy(ps, dest_buf, dsize);
        ps += dsize;
        out_size += dsize;
    }

    free(file_buf);
    free(dest_buf);

    uint32_t crc = crc32Calc(output, out_size);
    OSI_LOGI(0, "lzma3 out_size/%d address/%p crc/0x%08x",
             out_size, output, crc);
    return true;
}
#endif

static bool _loadRegionPlain(const char *name, void *output)
{
    ssize_t size = vfs_file_size(name);
    if (size > 0)
    {
        OSI_LOGI(0, "load out_size/%d address/%p", size, output);
        vfs_file_read(name, output, size);
        return true;
    }
    return false;
}

static bool _loadRegion(const char *name, void *output, uint32_t flags)
{
#ifdef SUPPORT_SOFTLZMA
    if (flags & HAL_SHARE_MEM_FLAG_LZMA)
        return _loadRegionLzma(name, output);
#endif
#ifdef SUPPORT_HARDLZMA2
    if (flags & HAL_SHARE_MEM_FLAG_LZMA2)
        return _loadRegionLzma2(name, output);
#endif
#ifdef SUPPORT_HARDLZMA3
    if (flags & HAL_SHARE_MEM_FLAG_LZMA3)
        return _loadRegionLzma3(name, output);
#endif
    return _loadRegionPlain(name, output);
}

#ifdef CONFIG_SUPPORT_IMS_DELTA_NV
static int readImsNvBuf(uint16_t nvid, void *buf)
{
    int nvsize = nvmReadItem(nvid, NULL, 0);
    if (nvsize <= 0)
        return nvsize;
    return nvmReadItem(nvid, buf, nvsize);
}

static void prvLoadImsDeltaNv(void)
{

    int len = 0, size = 0;

    uint8_t *ims_delta_nv_addr = ipc_get_deltanv_addr();
    imsNVInfo_t *ims_nv_addr_info = (imsNVInfo_t *)ipc_get_ims_nv_addr();

    len = readImsNvBuf(NVID_IMS_CSM_NV, ims_delta_nv_addr);
    if (len > 0)
    {
        ims_nv_addr_info->ims_csm_addr = (uint32_t)ims_delta_nv_addr;
        ims_nv_addr_info->ims_csm_len = len;
        ims_delta_nv_addr += len;
    }
    len = readImsNvBuf(NVID_IMS_SAPP_NV, ims_delta_nv_addr);
    if (len > 0)
    {
        ims_nv_addr_info->ims_sapp_addr = (uint32_t)ims_delta_nv_addr;
        ims_nv_addr_info->ims_sapp_len = len;
        ims_delta_nv_addr += len;
    }
    len = readImsNvBuf(NVID_IMS_ISIM_NV, ims_delta_nv_addr);
    if (len > 0)
    {
        ims_nv_addr_info->ims_isim_addr = (uint32_t)ims_delta_nv_addr;
        ims_nv_addr_info->ims_isim_len = len;
        ims_delta_nv_addr += len;
    }
    len = readImsNvBuf(NVID_SIM_DELTA_NV, ims_delta_nv_addr);
    if (len > 0)
    {
        ims_nv_addr_info->ims_sim1_delta_addr = (uint32_t)ims_delta_nv_addr;
        ims_nv_addr_info->ims_sim1_delta_len = len;
        ims_delta_nv_addr += len;
    }
    len = readImsNvBuf(NVID_SIM_DELTA_CARD2_NV, ims_delta_nv_addr);
    if (len > 0)
    {
        ims_nv_addr_info->ims_sim2_delta_addr = (uint32_t)ims_delta_nv_addr;
        ims_nv_addr_info->ims_sim2_delta_len = len;
        ims_delta_nv_addr += len;
    }
    len = readImsNvBuf(NVID_PLMN_DELTA_NV, ims_delta_nv_addr);
    if (len > 0)
    {
        ims_nv_addr_info->ims_plmn1_delta_addr = (uint32_t)ims_delta_nv_addr;
        ims_nv_addr_info->ims_plmn1_delta_len = len;
        ims_delta_nv_addr += len;
    }
    len = readImsNvBuf(NVID_PLMN_DELTA_CARD2_NV, ims_delta_nv_addr);
    if (len > 0)
    {
        ims_nv_addr_info->ims_plmn2_delta_addr = (uint32_t)ims_delta_nv_addr;
        ims_nv_addr_info->ims_plmn2_delta_len = len;
        ims_delta_nv_addr += len;
    }

    // read delta nv in /modem/nvm
    size = vfs_sfile_size(DELTANV_FMAME);
    if (size > 0)
    {
        vfs_sfile_read(DELTANV_FMAME, ims_delta_nv_addr, size);
        ipc_set_deltanv_bin((uint32_t)ims_delta_nv_addr, size);
    }
}
#endif
bool halCpLoad(void)
{
    OSI_LOGI(0, "load CP");

    const halShmemRegion_t *regions = NULL;
    size_t count = halShmemGetAllRegions(&regions);
    if (regions == NULL || count == 0)
        return false;

    uint32_t cp_address = 0;
    for (size_t n = 0; n < count; n++)
    {
        const halShmemRegion_t *r = &regions[n];
        if ((r->flags & HAL_SHARE_MEM_FLAG_CPNEED) == 0)
            continue;

        OSI_LOGXD(OSI_LOGPAR_SI, 0, "cpneed: %s / %d", r->name, r->size);
        int nvid = nvmGetIdFromFileName(r->name);
        if (nvid >= 0)
        {
            memset((void *)r->address, 0, r->size);
            int readnv = nvmReadItem(nvid, (void *)r->address, r->size);

            OSI_LOGD(0, "read to sharemem: %x / %d/%d", nvid, r->size, readnv);
        }
        else
        {
            char file_name[HAL_SHARE_MEM_NAME_LEN + 16];
            sprintf(file_name, CONFIG_FS_MODEM_MOUNT_POINT "/%s", r->name);

            if (!_loadRegion(file_name, (void *)r->address, r->flags))
            {
                OSI_LOGI(0, "clear out_size/%d address/%p", r->size, (void *)r->address);
                memset((void *)r->address, 0, r->size);
                continue;
            }
        }

        if (strcmp(r->name, MEM_CP_BIN_NAME) == 0)
            cp_address = r->address;
    }

#ifdef CONFIG_SUPPORT_IMS_DELTA_NV
    OSI_LOGI(0, "load ims delta nv");
    prvLoadImsDeltaNv();
#endif

#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPIO
#ifdef CONFIG_GPIO_USED_FOR_VBAT_RF_SWITCH
    halPmuSwitchPower(HAL_POWER_VBAT_RF, true, false);
#endif
#endif

    halShmemUpdateRamAccess();

    osiBootMode_t boot_mode = osiGetBootMode();
    CP_PSM_WAKE_TIME_REG = 0;
    if (boot_mode == OSI_BOOTMODE_CALIB)
    {
        CP_BOOT_MODE_REG = CP_CALIBRATION_BOOT_MODE;
    }
    else if (boot_mode == OSI_BOOTMODE_CALIB_POST)
    {
        CP_BOOT_MODE_REG = CP_CALIBRATION_POST_MODE;
    }
    else if (boot_mode == OSI_BOOTMODE_BBAT)
    {
        CP_BOOT_MODE_REG = CP_BBAT_BOOT_MODE;
    }
    else if (boot_mode == OSI_BOOTMODE_PSM_RESTORE)
    {
        uint32_t psm_elapsed = osiGetPsmElapsedTime() / 10;
        CP_BOOT_MODE_REG = CP_PSM_WAKE_BOOT_MODE;
        CP_PSM_WAKE_TIME_REG = psm_elapsed;
        OSI_LOGI(0, "cp psm restore boot elapsed/%d", psm_elapsed);
    }
    else
    {
        CP_BOOT_MODE_REG = 0;
    }

    if (cp_address != 0)
    {
#ifndef DEBUG_NOT_START_CP
        CP_JUMP_ADDRESS_REG = cp_address;
        CP_JUMP_MAGIC_REG = CP_WAKEUP_JUMP_MAGIC;

        REG_SYS_CTRL_CPCPU_RST_CLR_T cpcpu_rst_clr = {};
        cpcpu_rst_clr.b.clr_cpcpu_rst_id_sys = 1;
        cpcpu_rst_clr.b.clr_cpcpu_rst_id_core = 1;
        cpcpu_rst_clr.b.clr_cpcpu_rst_id_dbg = 1;
        cpcpu_rst_clr.b.clr_3 = 1;
        hwp_sysCtrl->cpcpu_rst_clr = cpcpu_rst_clr.v;
#endif
    }

    OSI_LOGI(0, "load CP done");
    return true;
}
