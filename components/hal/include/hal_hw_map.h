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

#ifndef _HAL_HW_MAP_H_
#define _HAL_HW_MAP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief properties to be set for blue screen dump regions
 */
typedef enum
{
    HAL_BS_REGION_IGNORE = (1 << 0), ///< the region should be ignored
    HAL_BS_REGION_FLASH = (1 << 1),  ///< the region is flash
    HAL_BS_REGION_ZSP = (1 << 2),    ///< the region is in zsp power domain
    HAL_BS_REGION_GGE = (1 << 3),    ///< the region is in gge power domain
    HAL_BS_REGION_LTE = (1 << 4),    ///< the region is in zsp power domain
    HAL_BS_REGION_WCN = (1 << 5),    ///< the region is in wcn power domain
    HAL_BS_REGION_RFDIG = (1 << 6),  ///< the region is in rfdig power domain
    HAL_BS_REGION_END = (1 << 31),   ///< the end of regions
} halBsDumpRegionFlags_t;

/**
 * \brief address range in [start, end]
 *
 * The end address is inclusive. For example, a range from 0x100000, with
 * size of 0x100000 should be expressed as:
 *
 * \code{.cpp}
 * halAddressRange_t range = {
 *     .start = 0x100000,
 *     .end = 0x1fffff,
 * };
 * \endcode
 */
typedef struct
{
    uint32_t start; ///< start address, inclusive
    uint32_t end;   ///< end address, inclusive
} halAddressRange_t;

/**
 * \brief blue screen dump region
 *
 * One or more property bits can be set for each region. At blue screen dump,
 * region can be ignored. For example, if the region is in power off power
 * domain.
 */
typedef struct
{
    uint32_t address;  ///< start address, inclusive
    uint32_t size;     ///< size
    uint32_t property; ///< properties
} halBsDumpRegion_t;

/**
 * \brief whether the specified address are accessible
 *
 * Typically, this will only be called during process debug commands from
 * PC. And it will avoid to access non-accessible addresses.
 *
 * \param address start address
 * \param size range size
 * \return
 *      - true if the whole region is accessible
 *      - false if some of the region is non-accessible.
 */
bool halHwMapValid(uint32_t address, size_t size);

/**
 * \brief get regions for blue screen dump
 *
 * The last region will be marked as \p HAL_BS_REGION_END.
 *
 * \return blue screen dump regions
 */
const halBsDumpRegion_t *halHwBsDumpRegions(void);

#ifdef __cplusplus
}
#endif
#endif
