/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "boot_mmu.h"
#include "boot_platform.h"
#include "cmsis_core.h"
#include "hal_chip.h"
#include "osi_api.h"

void bootMmuEnable(uint32_t *l1, uint32_t *l2)
{
    if (!OSI_IS_ALIGNED((unsigned)l1, 16 * 1024))
        osiPanic();

    __set_TLBIALL(0);
    __set_BPIALL(0);
    __DSB();
    __ISB();

    __set_ICIALLU(0);
    __DSB();
    __ISB();

    L1C_InvalidateDCacheAll();
    L1C_InvalidateICacheAll();

    MMU_TTSection(l1, 0, 4096, 0);
    MMU_TTSection(l1, 0x00000000, 0x8, HAL_DESCL1_SECTION_NORMAL_RX);
    MMU_TTSection(l1, 0x00900000, 0x600 - 0x9, HAL_DESCL1_SECTION_DEVICE_RW);
    MMU_TTSection(l1, 0x60000000, 0x200, HAL_DESCL1_SECTION_NORMAL_RX);
    MMU_TTSection(l1, 0x80000000, 0x200, HAL_DESCL1_SECTION_NORMAL_RWX);

    MMU_TTPage64k(l1, 0x00800000, 0x80000 >> 16, HAL_DESCL1_SECTION_PAGE64K, l2, HAL_DESCL2_PAGE64K_NORMAL_RWX);
    MMU_TTPage64k(l1, 0x00880000, 0x80000 >> 16, HAL_DESCL1_SECTION_PAGE64K, l2, HAL_DESCL2_PAGE64K_DEVICE_RW);

    __set_TTBR0(((uint32_t)l1) | 9);
    __ISB();

    __set_DACR(1);
    __ISB();

    MMU_Enable();

    L1C_EnableCaches();
    L1C_EnableBTAC();
}

void bootMmuDisable(void)
{
    L1C_CleanInvalidateDCacheAll();
    __DSB();
    __ISB();

    L1C_DisableCaches();
    MMU_Disable();
}
