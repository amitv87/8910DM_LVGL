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

#ifndef _DRV_LCD_DESC_H_
#define _DRV_LCD_DESC_H_

#include "drv_config.h"
#include "drv_lcd_v2.h"
#include "drv_lcd_panel.h"

extern const drvLcdPanelDesc_t gLcdGc9305Desc;
extern const drvLcdPanelDesc_t gLcdSt7735sDesc;
extern const drvLcdPanelDesc_t gLcdSt7789Desc;
#define CONFIG_SUPPORT_LCD_ST7789V

static const drvLcdPanelDesc_t *gLcdDescs[] = {
#ifdef CONFIG_SUPPORT_LCD_GC9305
    &gLcdGc9305Desc,
#endif 
#ifdef CONFIG_SUPPORT_LCD_ST7789V
    &gLcdSt7789Desc,
#endif
};

#endif
