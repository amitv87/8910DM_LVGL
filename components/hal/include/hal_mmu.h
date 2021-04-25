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

#ifndef _HAL_MMU_H_
#define _HAL_MMU_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

// MMU table uses a simple model
// * Use L1 if possible
// * L2 is used for SRAM, pages used by CP can be set to RO
// * L2 page is used for ONE L1 entry. Shared memory pages can be set to RW
//      and others can be set to RO
// * AP memory should be started at L1 (1MB) boundary.

enum
{
    HAL_MMU_ACCESS_CACHE_RWX,
    HAL_MMU_ACCESS_CACHE_RX,
    HAL_MMU_ACCESS_CACHE_R,
    HAL_MMU_ACCESS_UNCACHE_RWX,
    HAL_MMU_ACCESS_UNCACHE_RX,
    HAL_MMU_ACCESS_UNCACHE_R,

    HAL_MMU_ACCESS_DEVICE_RW,
    HAL_MMU_ACCESS_DEVICE_R
};

/**
 * \brief create initial MMU table
 *
 * For 8910, the initial MMU table is:
 * - ROM (1M) uses 4K page
 * - SRAM (1M) uses 4K page
 * - RAM (CONFIG_RAM_SIZE) uses 4K page
 * - Others uses 1M section
 *
 * Section or page properties:
 * - ROM: The first page is NO_ACCESS to detect NULL access, other pages
 *          are NORMAL_RX.
 * - SRAM: (CONFIG_APP_SRAM_SHMEM_OFFSET, CONFIG_APP_SRAM_SHMEM_SIZE) is NC_RW,
 *          other pages are NORMAL_RWX. After memory list is loaded, it will
 *          be changed later.
 * - RAM: NORMAL_RWX
 * - FLASH: (0x60000000, 0x20000000) is NORMAL_RX
 * - (0x880000, 0x80000): DEVICE_RW
 * - (0x900000, ): DEVICE_RW
 */
void halMmuCreateTable(void);

/**
 * \brief set range MMU properties
 *
 * For 8910, the implicit rules for section and pages will be applied.
 *
 * When the region uses 4K page, \p start will be *aligned down*,
 * \p (start + size) will be *aligned up*. That is, when \p start and
 * \p size are not 4K aligned, the actual changed range will be no less
 * than the specified range. However, it is recommended to set \p start
 * and \p size as 4K aligned.
 *
 * When the region uses 1M section, and \p start or \p size is not 1MB
 * aligned, system will panic.
 *
 * Note: this is designed for batch set. So, cache coherence isn't called
 * inside. Caller should take care of cache coherence. An example:
 *
 * \code{.cpp}
 * L1C_CleanDCacheAll();
 * halMmuSetAccess(...);
 * halMmuSetAccess(...);
 * ......
 * L1C_CleanDCacheAll();
 * MMU_InvalidateTLB();
 * \endcode
 *
 * \param start         range start
 * \param size          range size
 * \param access        range MMU property
 */
void halMmuSetAccess(uintptr_t start, size_t size, uint32_t access);

/**
 * \brief enable or disable NULL pointer protection
 *
 * When enabled, the first page at address 0 is set as inaccessible. It can
 * detect NULL pointer access (usually illegal) earlier.
 *
 * However, ROM is located at address 0. To enable ROM access, this
 * protection should be disabled.
 *
 * Unlike \p halMmuSetAccess, cache and tlb coherence will be handled inside.
 *
 * \param enable        true for enable address 0 protection, inaccessible
 */
void halMmuSetNullProtection(bool enable);

#ifdef __cplusplus
}
#endif
#endif
