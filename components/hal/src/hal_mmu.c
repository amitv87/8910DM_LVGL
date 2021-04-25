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

#include "hal_mmu.h"
#include "hal_config.h"
#include "hal_chip.h"
#include "cmsis_core.h"
#include "osi_api.h"
#include "osi_log.h"

#define SIZE_1M (1 << 20)
#define SIZE_4K (4 * 1024)
#define SIZE_64K (64 * 1024)

// Simple TTB is used: SRAM and RAM using L2, and others using L1
#define SRAM_SECTION_START (CONFIG_SRAM_PHY_ADDRESS)
#define SRAM_SECTION_END (CONFIG_SRAM_PHY_ADDRESS + 0x100000)
#define RAM_SECTION_START (CONFIG_RAM_PHY_ADDRESS)
#define RAM_SECTION_END (CONFIG_RAM_PHY_ADDRESS + CONFIG_RAM_SIZE)

static uint32_t gHalMmuTableL1[4096] OSI_SECTION(.TTBL1) OSI_ALIGNED(16384);
static uint32_t gHalMmuTableL2Rom[256] OSI_SECTION(.TTBL2) OSI_ALIGNED(1024);
static uint32_t gHalMmuTableL2Sram[256] OSI_SECTION(.TTBL2) OSI_ALIGNED(1024);
static uint32_t gHalMmuTableL2ExtRam[256 * (CONFIG_RAM_SIZE >> 20)] OSI_SECTION(.TTBL2) OSI_ALIGNED(1024);

void halMmuCreateTable(void)
{
    uint32_t *l1 = gHalMmuTableL1;
    uint32_t *l2rom = gHalMmuTableL2Rom;
    uint32_t *l2sram = gHalMmuTableL2Sram;
    uint32_t *l2extram = gHalMmuTableL2ExtRam;

    // Create 4GB of faulting entries
    MMU_TTSection(l1, 0, 4096, 0);

    MMU_TTSection(l1, 0x00000000, 0x8, HAL_DESCL1_SECTION_NORMAL_RX);
    MMU_TTSection(l1, 0x00900000, 0x600 - 0x9, HAL_DESCL1_SECTION_DEVICE_RW);
    MMU_TTSection(l1, 0x60000000, 0x200, HAL_DESCL1_SECTION_NORMAL_RX);

    // Set the first page of ROM as non-executable, to detect zero function
    MMU_TTPage4k(l1, 0x00000000, SIZE_1M / SIZE_4K, HAL_DESCL1_SECTION_PAGE4K, l2rom, HAL_DESCL2_PAGE4K_NORMAL_RX);
    MMU_TTPage4k(l1, 0x00000000, 1, HAL_DESCL1_SECTION_PAGE4K, l2rom, HAL_DESCL2_PAGE4K_NO_ACCESS);

    MMU_TTPage4k(l1, 0x00800000, 0x80000 / SIZE_4K, HAL_DESCL1_SECTION_PAGE4K, l2sram, HAL_DESCL2_PAGE4K_NORMAL_RWX);
    MMU_TTPage4k(l1, 0x00880000, 0x80000 / SIZE_4K, HAL_DESCL1_SECTION_PAGE4K, l2sram, HAL_DESCL2_PAGE4K_DEVICE_RW);

    // SRAM shared by AP/CP, should be set as uncachable
    MMU_TTPage4k(l1, CONFIG_SRAM_PHY_ADDRESS + CONFIG_APP_SRAM_SHMEM_OFFSET, CONFIG_APP_SRAM_SHMEM_SIZE / SIZE_4K,
                 HAL_DESCL1_SECTION_PAGE4K, l2sram, HAL_DESCL2_PAGE4K_NC_RW);

    // PSRAM/DDR shall always use 4K page, rather than 1M section.
    // It is needed to set as 4K page here.
    for (unsigned i = 0; i < (CONFIG_RAM_SIZE / SIZE_1M); ++i)
    {
        MMU_TTPage4k(l1, 0x80000000 + SIZE_1M * i, SIZE_1M / SIZE_4K, HAL_DESCL1_SECTION_PAGE4K,
                     &l2extram[256 * i], HAL_DESCL2_PAGE4K_NORMAL_RWX);
    }

    /* Set location of level 1 page table
    ; 31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
    ; 13:7  - 0x0
    ; 6     - IRGN[0] 0x0 (Inner WB WA)
    ; 5     - NOS     0x0 (Non-shared)
    ; 4:3   - RGN     0x1 (Outer WB WA)
    ; 2     - IMP     0x0 (Implementation Defined)
    ; 1     - S       0x0 (Non-shared)
    ; 0     - IRGN[1] 0x1 (Inner WB WA) */
    __set_TTBR0(((uint32_t)l1) | 9);
    __ISB();

    /* Set up domain access control register
    ; We set domain 0 to Client and all other domains to No Access.
    ; All translation table entries specify domain 0 */
    __set_DACR(1);
    __ISB();
}

static uint32_t _sectionDesc(uint32_t access)
{
    switch (access)
    {
    case HAL_MMU_ACCESS_CACHE_RWX:
        return HAL_DESCL1_SECTION_NORMAL_RWX;
    case HAL_MMU_ACCESS_CACHE_RX:
        return HAL_DESCL1_SECTION_NORMAL_RX;
    case HAL_MMU_ACCESS_CACHE_R:
        return HAL_DESCL1_SECTION_NORMAL_R;

    case HAL_MMU_ACCESS_UNCACHE_RWX:
        return HAL_DESCL1_SECTION_NC_RWX;
    case HAL_MMU_ACCESS_UNCACHE_RX:
        return HAL_DESCL1_SECTION_NC_RX;
    case HAL_MMU_ACCESS_UNCACHE_R:
        return HAL_DESCL1_SECTION_NC_R;

    case HAL_MMU_ACCESS_DEVICE_RW:
        return HAL_DESCL1_SECTION_DEVICE_RW;
    case HAL_MMU_ACCESS_DEVICE_R:
        return HAL_DESCL1_SECTION_DEVICE_RW;
    default:
        return 0;
    }
}

static uint32_t _page4kDesc(uint32_t access)
{
    switch (access)
    {
    case HAL_MMU_ACCESS_CACHE_RWX:
        return HAL_DESCL2_PAGE4K_NORMAL_RWX;
    case HAL_MMU_ACCESS_CACHE_RX:
        return HAL_DESCL2_PAGE4K_NORMAL_RX;
    case HAL_MMU_ACCESS_CACHE_R:
        return HAL_DESCL2_PAGE4K_NORMAL_R;

    case HAL_MMU_ACCESS_UNCACHE_RWX:
        return HAL_DESCL2_PAGE4K_NC_RWX;
    case HAL_MMU_ACCESS_UNCACHE_RX:
        return HAL_DESCL2_PAGE4K_NC_RX;
    case HAL_MMU_ACCESS_UNCACHE_R:
        return HAL_DESCL2_PAGE4K_NC_R;

    case HAL_MMU_ACCESS_DEVICE_RW:
        return HAL_DESCL2_PAGE4K_DEVICE_RW;
    case HAL_MMU_ACCESS_DEVICE_R:
        return HAL_DESCL2_PAGE4K_DEVICE_R;
    default:
        return 0;
    }
}

static void _setPageAccess(uintptr_t start, size_t size, uint32_t access)
{
    uintptr_t end = OSI_ALIGN_UP(start + size, SIZE_4K);
    start = OSI_ALIGN_DOWN(start, SIZE_4K);

    uint32_t *l1 = gHalMmuTableL1;
    uint32_t *l2 = NULL;
    while (start < end)
    {
        uintptr_t set_end = OSI_ALIGN_DOWN(start + SIZE_1M, SIZE_1M);
        if (end < set_end)
            set_end = end;

        if (start >= SRAM_SECTION_START && start < SRAM_SECTION_END)
            l2 = gHalMmuTableL2Sram;
        else
            l2 = &gHalMmuTableL2ExtRam[256 * ((start - RAM_SECTION_START) >> 20)];

        MMU_TTPage4k(l1, start, (set_end - start) / SIZE_4K, HAL_DESCL1_SECTION_PAGE4K,
                     l2, _page4kDesc(access));
        start = set_end;
    }
}

static void _setSectionAccess(uintptr_t start, size_t size, uint32_t access)
{
    if (!OSI_IS_ALIGNED(start, SIZE_1M) || !OSI_IS_ALIGNED(size, SIZE_1M))
        osiPanic();

    uint32_t *l1 = gHalMmuTableL1;
    MMU_TTSection(l1, start, size / SIZE_1M, _sectionDesc(access));
}

void halMmuSetAccess(uintptr_t start, size_t size, uint32_t access)
{
    if ((start >= SRAM_SECTION_START && start < SRAM_SECTION_END) ||
        (start >= RAM_SECTION_START && start < RAM_SECTION_END))
        _setPageAccess(start, size, access);
    else
        _setSectionAccess(start, size, access);
}

void halMmuSetNullProtection(bool enable)
{
    uint32_t *l1 = gHalMmuTableL1;
    uint32_t *l2rom = gHalMmuTableL2Rom;

    if (enable)
    {
        MMU_TTPage4k(l1, 0x00000000, 1, HAL_DESCL1_SECTION_PAGE4K,
                     l2rom, HAL_DESCL2_PAGE4K_NO_ACCESS);
    }
    else
    {
        MMU_TTPage4k(l1, 0x00000000, 1, HAL_DESCL1_SECTION_PAGE4K,
                     l2rom, HAL_DESCL2_PAGE4K_NORMAL_RX);
    }

    L1C_CleanDCacheMVA(&l2rom[0]);
    MMU_InvalidateTLB();
}

void halMmuShowDescriptors(void)
{
    uint32_t descl1;
    uint32_t descl2;
    mmu_region_attributes_Type region;

    region_normal(region, SECTION);
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_NORMAL_RWX 0x%08x", descl1);

    region_normal(region, SECTION);
    region.xn_t = NON_EXECUTE;
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_NORMAL_RW 0x%08x", descl1);

    region_normal(region, SECTION);
    region.priv_t = region.user_t = READ;
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_NORMAL_RX 0x%08x", descl1);

    region_normal(region, SECTION);
    region.priv_t = region.user_t = READ;
    region.xn_t = NON_EXECUTE;
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_NORMAL_R 0x%08x", descl1);

    region_normal_nc(region, SECTION);
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_NC_RWX 0x%08x", descl1);

    region_normal_nc(region, SECTION);
    region.xn_t = NON_EXECUTE;
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_NC_RW 0x%08x", descl1);

    region_normal_nc(region, SECTION);
    region.priv_t = region.user_t = READ;
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_NC_RX 0x%08x", descl1);

    region_normal_nc(region, SECTION);
    region.priv_t = region.user_t = READ;
    region.xn_t = NON_EXECUTE;
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_NC_R 0x%08x", descl1);

    region_device_rw(region, SECTION);
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_DEVICE_RW 0x%08x", descl1);

    region_device_ro(region, SECTION);
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_DEVICE_R 0x%08x", descl1);

    region_no_access(region, SECTION);
    MMU_GetSectionDescriptor(&descl1, region);
    OSI_LOGI(0, "DESCL1_SECTION_NO_ACCESS 0x%08x", descl1);

    region_normal(region, SECTION);
    region.rg_t = PAGE_64k;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL1_SECTION_PAGE64K 0x%08x", descl1);
    OSI_LOGI(0, "DESCL2_PAGE64K_NORMAL_RWX 0x%08x", descl2);

    region_normal(region, SECTION);
    region.rg_t = PAGE_64k;
    region.xn_t = NON_EXECUTE;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE64K_NORMAL_RW 0x%08x", descl2);

    region_normal(region, SECTION);
    region.rg_t = PAGE_64k;
    region.priv_t = region.user_t = READ;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE64K_NORMAL_RX 0x%08x", descl2);

    region_normal(region, SECTION);
    region.rg_t = PAGE_64k;
    region.priv_t = region.user_t = READ;
    region.xn_t = NON_EXECUTE;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE64K_NORMAL_R 0x%08x", descl2);

    region_normal_nc(region, SECTION);
    region.rg_t = PAGE_64k;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE64K_NC_RWX 0x%08x", descl2);

    region_normal_nc(region, SECTION);
    region.rg_t = PAGE_64k;
    region.xn_t = NON_EXECUTE;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE64K_NC_RW 0x%08x", descl2);

    region_normal_nc(region, SECTION);
    region.rg_t = PAGE_64k;
    region.priv_t = region.user_t = READ;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE64K_NC_RX 0x%08x", descl2);

    region_normal_nc(region, SECTION);
    region.rg_t = PAGE_64k;
    region.priv_t = region.user_t = READ;
    region.xn_t = NON_EXECUTE;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE64K_NC_R 0x%08x", descl2);

    region_device_rw(region, SECTION);
    region.rg_t = PAGE_64k;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE64K_DEVICE_RW 0x%08x", descl2);

    region_device_ro(region, SECTION);
    region.rg_t = PAGE_64k;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE64K_DEVICE_R 0x%08x", descl2);

    region_no_access(region, SECTION);
    region.rg_t = PAGE_64k;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE64K_NO_ACCESS 0x%08x", descl2);

    region_normal(region, SECTION);
    region.rg_t = PAGE_4k;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL1_SECTION_PAGE4K 0x%08x", descl1);
    OSI_LOGI(0, "DESCL2_PAGE4K_NORMAL_RWX 0x%08x", descl2);

    region_normal(region, SECTION);
    region.rg_t = PAGE_4k;
    region.xn_t = NON_EXECUTE;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE4K_NORMAL_RW 0x%08x", descl2);

    region_normal(region, SECTION);
    region.rg_t = PAGE_4k;
    region.priv_t = region.user_t = READ;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE4K_NORMAL_RX 0x%08x", descl2);

    region_normal(region, SECTION);
    region.rg_t = PAGE_4k;
    region.priv_t = region.user_t = READ;
    region.xn_t = NON_EXECUTE;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE4K_NORMAL_R 0x%08x", descl2);

    region_normal_nc(region, SECTION);
    region.rg_t = PAGE_4k;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE4K_NC_RWX 0x%08x", descl2);

    region_normal_nc(region, SECTION);
    region.rg_t = PAGE_4k;
    region.xn_t = NON_EXECUTE;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE4K_NC_RW 0x%08x", descl2);

    region_normal_nc(region, SECTION);
    region.rg_t = PAGE_4k;
    region.priv_t = region.user_t = READ;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE4K_NC_RX 0x%08x", descl2);

    region_normal_nc(region, SECTION);
    region.rg_t = PAGE_4k;
    region.priv_t = region.user_t = READ;
    region.xn_t = NON_EXECUTE;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE4K_NC_R 0x%08x", descl2);

    region_device_rw(region, SECTION);
    region.rg_t = PAGE_4k;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE4K_DEVICE_RW 0x%08x", descl2);

    region_device_ro(region, SECTION);
    region.rg_t = PAGE_4k;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE4K_DEVICE_R 0x%08x", descl2);

    region_no_access(region, SECTION);
    region.rg_t = PAGE_4k;
    MMU_GetPageDescriptor(&descl1, &descl2, region);
    OSI_LOGI(0, "DESCL2_PAGE4K_NO_ACCESS 0x%08x", descl2);
}
