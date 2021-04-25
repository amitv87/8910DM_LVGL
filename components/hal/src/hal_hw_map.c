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

#include "hal_hw_map.h"
#include "osi_compiler.h"

#ifdef CONFIG_SOC_8910
#include "8910/hal_address_map.h"
#elif defined(CONFIG_SOC_8909)
#include "8909/hal_address_map.h"
#else
static const halAddressRange_t gHwMap[] = {{0, 0xffffffff}};
static const halBsDumpRegion_t gBsDumpRegions[] = {{0, 0, HAL_BS_REGION_END}};
#endif

bool halHwMapValid(uint32_t address, size_t size)
{
#ifdef CONFIG_CPU_MIPS
    address = OSI_KSEG01_PHY_ADDR(address);
#endif

    for (int n = 0; n < OSI_ARRAY_SIZE(gHwMap); n++)
    {
        const halAddressRange_t *r = &gHwMap[n];
        if (address >= r->start && address + size <= r->end)
            return true;
    }
    return false;
}

const halBsDumpRegion_t *halHwBsDumpRegions(void)
{
    return gBsDumpRegions;
}
