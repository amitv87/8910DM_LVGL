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

#include "hal_chip.h"
#include "osi_api.h"
#include "hwregs.h"
#include <stdarg.h>

void OSI_SECTION_SRAM_BOOT_TEXT halApplyRegisters(uint32_t address, ...)
{
    va_list ap;
    va_start(ap, address);
    while (address != REG_APPLY_TYPE_END)
    {
        if (address == REG_APPLY_TYPE_UDELAY)
        {
            uint32_t us = va_arg(ap, uint32_t);
            osiDelayUS(us);
        }
        else
        {
            uint32_t value = va_arg(ap, uint32_t);
            *(volatile uint32_t *)address = value;
        }

        address = va_arg(ap, uint32_t);
    }
    va_end(ap);
}

void OSI_SECTION_SRAM_BOOT_TEXT halApplyRegisterList(const uint32_t *data)
{
    if (data == NULL)
        return;

    uint32_t address;
    while ((address = *data++) != REG_APPLY_TYPE_END)
    {
        if (address == REG_APPLY_TYPE_UDELAY)
        {
            uint32_t us = *data++;
            osiDelayUS(us);
        }
        else
        {
            uint32_t value = *data++;
            *(volatile uint32_t *)address = value;
        }
    }
}
