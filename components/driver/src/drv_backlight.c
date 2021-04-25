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
#include "osi_api.h"
#include "drv_wdt.h"
#include "hal_adi_bus.h"
#include "hwregs.h"
#include "drv_backlight.h"
#include "hal_chip.h"

#define BACK_LIGHT_LEVEL_MAX (255)

static void prvSetBackLightLevel(uint32_t light_level)
{
    uint32_t rg_rgb_v0_current_level;
    uint32_t rg_rgb_v1_current_level;
    uint32_t rg_rgb_v2_current_level;
    uint32_t rg_rgb_v3_current_level;

    REG_RDA2720M_BLTC_RG_RGB_V0_T rg_rgb_v0;
    REG_RDA2720M_BLTC_RG_RGB_V1_T rg_rgb_v1;
    REG_RDA2720M_BLTC_RG_RGB_V2_T rg_rgb_v2;
    REG_RDA2720M_BLTC_RG_RGB_V3_T rg_rgb_v3;

    if (light_level > BACK_LIGHT_LEVEL_MAX)
        light_level = BACK_LIGHT_LEVEL_MAX;

    uint32_t level = light_level / 4;
    uint32_t level_mod = light_level % 4;

    rg_rgb_v0_current_level = level;
    rg_rgb_v1_current_level = level;
    rg_rgb_v2_current_level = level;
    rg_rgb_v3_current_level = level;

    switch (level_mod)
    {
    case 1:
        rg_rgb_v0_current_level += 1;
        break;

    case 2:
        rg_rgb_v0_current_level += 1;
        rg_rgb_v1_current_level += 1;
        break;

    case 3:
        rg_rgb_v0_current_level += 1;
        rg_rgb_v1_current_level += 1;
        rg_rgb_v2_current_level += 1;
        break;

    default:
        break;
    }

    REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v0, rg_rgb_v0, rg_rgb_v0, rg_rgb_v0_current_level);
    REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v1, rg_rgb_v1, rg_rgb_v1, rg_rgb_v1_current_level);
    REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v2, rg_rgb_v2, rg_rgb_v2, rg_rgb_v2_current_level);
    REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v3, rg_rgb_v3, rg_rgb_v3, rg_rgb_v3_current_level);
    return;
}

bool drvBackLightOpen(uint32_t light_level)
{
    halPmuSwitchPower(HAL_POWER_BACK_LIGHT, true, false);
    prvSetBackLightLevel(light_level);
    return true;
}

bool drvBackLightClose()
{
    halPmuSwitchPower(HAL_POWER_BACK_LIGHT, false, false);

    return true;
}
