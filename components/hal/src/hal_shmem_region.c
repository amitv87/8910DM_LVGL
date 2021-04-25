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

#include "osi_log.h"
#include "hal_shmem_region.h"
#include "hal_config.h"
#include "hal_mmu.h"
#include "drv_md_ipc.h"
#include "osi_api.h"
#include "vfs.h"
#include "cmsis_core.h"
#include <string.h>
#include <stdlib.h>

#define MEM_MEM_INDEX_LIST_FILE_NAME CONFIG_FS_MODEM_MOUNT_POINT "/" MEM_MEM_INDEX_LIST_NAME
#define MEM_LIST_SIZE_MAX (0x1000)

#define SRAM_START (CONFIG_SRAM_PHY_ADDRESS)
#define SRAM_END (CONFIG_SRAM_PHY_ADDRESS + 0x80000)
#define RAM_START (CONFIG_RAM_PHY_ADDRESS)
#define RAM_END (CONFIG_RAM_PHY_ADDRESS + CONFIG_RAM_SIZE)

#define APP_SRAM_SHMEM_END (CONFIG_SRAM_PHY_ADDRESS + CONFIG_APP_SRAM_SHMEM_OFFSET + CONFIG_APP_SRAM_SHMEM_SIZE)
#define APP_RAM_START (CONFIG_RAM_PHY_ADDRESS + CONFIG_APP_RAM_OFFSET)
#define APP_RAM_SIZE (CONFIG_APP_TOTAL_RAM_SIZE)
#define APP_RAM_END (CONFIG_RAM_PHY_ADDRESS + CONFIG_APP_RAM_OFFSET + APP_RAM_SIZE)

static uint32_t gMemIndexListAddress = 0;
static size_t gMemIndexListCount = 0;

bool halShareMemRegionLoad(void)
{
    if (gMemIndexListAddress != 0 && gMemIndexListCount != 0)
        return false;

    struct stat st;
    if (vfs_stat(MEM_MEM_INDEX_LIST_FILE_NAME, &st) < 0)
    {
        OSI_LOGXE(OSI_LOGPAR_S, 0, "failed to stat: %s", MEM_MEM_INDEX_LIST_FILE_NAME);
        return false;
    }
    if (st.st_size > MEM_LIST_SIZE_MAX ||
        (st.st_size % sizeof(halShmemRegion_t)) != 0)
    {
        OSI_LOGE(0, "mem index list size too large %d", st.st_size);
        return false;
    }

    halShmemRegion_t *regions = (halShmemRegion_t *)malloc(st.st_size);
    if (regions == NULL)
        return false;

    if (vfs_file_read(MEM_MEM_INDEX_LIST_FILE_NAME, regions, st.st_size) != st.st_size)
    {
        OSI_LOGE(0, "failed to read mem index list file");
        free(regions);
        return false;
    }

    size_t count = st.st_size / sizeof(halShmemRegion_t);
    for (size_t n = 0; n < count; n++)
    {
        if (regions[n].name[HAL_SHARE_MEM_NAME_LEN - 1] != '\0')
        {
            OSI_LOGE(0, "invalid mem index list name");
            free(regions);
            return false;
        }
    }

    halShmemRegion_t *mem_list_region = NULL;
    for (size_t n = 0; n < count; n++)
    {
        if (strcmp(regions[n].name, MEM_MEM_INDEX_LIST_NAME) == 0)
        {
            mem_list_region = &regions[n];
            break;
        }
    }

    if (mem_list_region == NULL)
    {
        OSI_LOGE(0, "mem index list not exist in file");
        free(regions);
        return false;
    }

    memset((void *)mem_list_region->address, 0, mem_list_region->size);
    memcpy((void *)mem_list_region->address, regions, st.st_size);
    osiDCacheClean((void *)mem_list_region->address, mem_list_region->size);

    gMemIndexListAddress = mem_list_region->address;
    gMemIndexListCount = count;
    free(regions);

    const halShmemRegion_t *apregion = halShmemGetRegion(MEM_AP_BIN_NAME);
    if (apregion == NULL)
    {
        OSI_LOGE(0, "failed to get ap.bin from mem index list");
    }
    else if (apregion->address != APP_RAM_START ||
             apregion->size != APP_RAM_SIZE)
    {
        OSI_LOGE(0, "ap.bin mismatch address/0x%08x/0x%08x size/0x%08x/0x%08x",
                 apregion->address, APP_RAM_START,
                 apregion->size, APP_RAM_SIZE);
    }

    return true;
}

const halShmemRegion_t *halShmemGetRegionByAddr(uint32_t address)
{
    if (gMemIndexListAddress == 0 || gMemIndexListCount == 0)
        return NULL;

    const halShmemRegion_t *regions = (halShmemRegion_t *)gMemIndexListAddress;
    for (size_t n = 0; n < gMemIndexListCount; n++)
    {
        const halShmemRegion_t *r = &regions[n];
        if (r->address == address)
            return r;
    }

    return NULL;
}

const halShmemRegion_t *halShmemGetRegion(const char *name)
{
    if (gMemIndexListAddress == 0 || gMemIndexListCount == 0)
        return NULL;

    const halShmemRegion_t *regions = (halShmemRegion_t *)gMemIndexListAddress;
    for (size_t n = 0; n < gMemIndexListCount; n++)
    {
        const halShmemRegion_t *r = &regions[n];
        if (strncmp(r->name, name, HAL_SHARE_MEM_NAME_LEN) == 0)
            return r;
    }

    return NULL;
}

size_t halShmemGetAllRegions(const halShmemRegion_t **regions)
{
    if (regions != NULL)
        *regions = (const halShmemRegion_t *)gMemIndexListAddress;
    return gMemIndexListCount;
}

void halShmemUpdateRamAccess(void)
{
    L1C_CleanDCacheAll();

    // SRAM used by CP
    halMmuSetAccess(APP_SRAM_SHMEM_END, SRAM_END - APP_SRAM_SHMEM_END, HAL_MMU_ACCESS_UNCACHE_R);

    // RAM shared by AP/CP.
    halMmuSetAccess(RAM_START, APP_RAM_START - RAM_START, HAL_MMU_ACCESS_UNCACHE_RWX);

    // RAM used by CP
    halMmuSetAccess(APP_RAM_END, RAM_END - APP_RAM_END, HAL_MMU_ACCESS_UNCACHE_R);

    // CP address
    const halShmemRegion_t *cp = halShmemGetRegion(MEM_CP_BIN_NAME);
    halMmuSetAccess(cp->address, cp->size, HAL_MMU_ACCESS_UNCACHE_R);

    const halShmemRegion_t *regions = (halShmemRegion_t *)gMemIndexListAddress;
    for (size_t n = 0; n < gMemIndexListCount; n++)
    {
        const halShmemRegion_t *r = &regions[n];
        if ((r->flags & HAL_SHARE_MEM_FLAG_APRO) &&
            r->address >= RAM_START &&
            r->address + r->size <= RAM_END)
        {
            OSI_LOGI(0, "APRO: %p %d", r->address, r->size);
            halMmuSetAccess(r->address, r->size, HAL_MMU_ACCESS_UNCACHE_R);
        }
    }

    // The first page is memlist. It won't be changed anymore.
    // Also, it can detect unexpected write.
    halMmuSetAccess(RAM_START, 4096, HAL_MMU_ACCESS_CACHE_R);

    ipc_update_buf_access();

    L1C_CleanDCacheAll();
    MMU_InvalidateTLB();
}
