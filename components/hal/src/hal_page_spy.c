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

#include "hal_page_spy.h"
#include "hwregs.h"
#include "osi_api.h"

int halPageSpyProtect(const void *start, size_t size, uint32_t mode)
{
    if (mode == 0 || size == 0)
        return -1;

    uint32_t critical = osiEnterCritical();

    uint32_t enable = hwp_pageSpy->enable;
    for (int n = 0; n < PAGE_SPY_NB_PAGE; n++)
    {
        if (enable & (1 << n))
            continue;

#ifdef CONFIG_SOC_8909
        hwp_pageSpy->page[n].start = OSI_KSEG01_PHY_ADDR(start);
        hwp_pageSpy->page[n].end = OSI_KSEG01_PHY_ADDR(start) + size;

        if (mode & HAL_PAGESPY_DETECT_READ)
            hwp_pageSpy->detect_read_reg |= (1 << n);
        else
            hwp_pageSpy->detect_read_reg &= ~(1 << n);

        if (mode & HAL_PAGESPY_DETECT_WRITE)
            hwp_pageSpy->detect_write_reg |= (1 << n);
        else
            hwp_pageSpy->detect_write_reg &= ~(1 << n);
#endif

#ifdef CONFIG_SOC_8955
        REG_PAGE_SPY_START_T start_reg = {
            .b.start_address = OSI_KSEG01_PHY_ADDR(start),
            .b.detect_read = (mode & HAL_PAGESPY_DETECT_READ) ? 1 : 0,
            .b.detect_write = (mode & HAL_PAGESPY_DETECT_WRITE) ? 1 : 0,
        };
        hwp_pageSpy->page[n].start = start_reg.v;
        hwp_pageSpy->page[n].end = OSI_KSEG01_PHY_ADDR(start) + size;
#endif

        hwp_pageSpy->enable = (1 << n); // write set
        osiExitCritical(critical);
        return n;
    }

    osiExitCritical(critical);
    return -1;
}

bool halPageSpyUnprotect(int id)
{
    if ((unsigned)id >= PAGE_SPY_NB_PAGE)
        return false;

    hwp_pageSpy->disable = (1 << id); // write clear
    return true;
}
