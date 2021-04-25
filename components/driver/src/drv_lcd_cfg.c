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

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#include "hal_chip.h"
#include "hwregs.h"
#include "osi_api.h"
#include "osi_log.h"
#include "osi_profile.h"
#include "osi_byte_buf.h"
#include "drv_lcd.h"
#include "quec_proj_config.h"

extern const lcdSpec_t g_lcd_gc9305;
extern const lcdSpec_t g_lcd_st7735s;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LCD
extern const lcdSpec_t g_lcd_st7789v;
#endif
extern const lcdSpec_t g_lcd_gc9307;

static uint8_t g_lcd_index = 0xff;

static lcdSpec_t *lcd_cfg_tab[] =
    {
        (lcdSpec_t *)&g_lcd_gc9305,
        (lcdSpec_t *)&g_lcd_st7735s,
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LCD
        (lcdSpec_t *)&g_lcd_st7789v,
#endif
        (lcdSpec_t *)&g_lcd_gc9307,
};

static uint32_t _drvLcdGetCfgCount(void)
{
    uint32_t count;
    count = sizeof(lcd_cfg_tab) / sizeof(lcd_cfg_tab[0]);
    OSI_LOGI(0, "lcd:  _drvLcdGetCfgCount count=%d \n", count);

    return count;
}

/*****************************************************************************/
//  Description:    This function is used to get LCD spec information
/*****************************************************************************/
lcdSpec_t *drvLcdGetSpecInfo(void)
{
    uint32_t count = 0;
    uint32_t i = 0;
    uint32_t lcm_dev_id = 0;
    OSI_LOGI(0, "lcd:  drvLcdGetSpecInfo \n");
    count = _drvLcdGetCfgCount();
    if ((count != 0) && (g_lcd_index == 0xff))
    {
        for (i = 0; i < count; i++)
        {
            if (lcd_cfg_tab[i]->operation->readId)
            {
                lcm_dev_id = lcd_cfg_tab[i]->operation->readId();
                OSI_LOGI(0, "lcd:  drvLcdGetSpecInfo id=0x%x \n", lcm_dev_id);
            }
            if (lcm_dev_id == lcd_cfg_tab[i]->dev_id)
            {
                g_lcd_index = i;
                OSI_LOGI(0, "lcd:  drvLcdGetSpecInfo g_lcd_index =0x%x \n", g_lcd_index);
                return lcd_cfg_tab[g_lcd_index];
            }
        }
    }
    else if (g_lcd_index != 0xff)
    {
        return lcd_cfg_tab[g_lcd_index];
    }
    OSI_LOGI(0, "lcd:  drvLcdGetSpecInfo null ! \n");
    return NULL;
}

#ifdef __cplusplus
}

#endif // End of lcm_cfg_info.c
