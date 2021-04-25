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

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_lcd_panel.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hwregs.h"

#define LCD_CMD_READ_ID (0x04)

static void prvSt7789SetDir(drvLcd_t *d, drvLcdDirection_t dir)
{
    // MY MX MV ML BGR MH x x
    uint8_t ctrl = 0x8; // default value except MY/MX/MV
    if (dir & 1)
        ctrl |= 0x80;
    if (dir & 2)
        ctrl |= 0x40;
    if (dir & 4)
        ctrl |= 0x20;

    drvLcdWriteCmd(d, 0x36); // memory access control
    drvLcdWriteData(d, ctrl);
}

static void prvSt7789BlitPrepare(drvLcd_t *d, drvLcdDirection_t dir, const drvLcdArea_t *roi)
{
    OSI_LOGD(0, "St7789 dir/%d roi/%d/%d/%d/%d", dir, roi->x, roi->y, roi->w, roi->h);

    prvSt7789SetDir(d, dir);

    uint16_t left = roi->x;
    uint16_t right = drvLcdAreaEndX(roi);
    uint16_t top = roi->y;
    uint16_t bot = drvLcdAreaEndY(roi);

    drvLcdWriteCmd(d, 0x2a);                 // set hori start , end (left, right)
    drvLcdWriteData(d, (left >> 8) & 0xff);  // left high 8 b
    drvLcdWriteData(d, left & 0xff);         // left low 8 b
    drvLcdWriteData(d, (right >> 8) & 0xff); // right high 8 b
    drvLcdWriteData(d, right & 0xff);        // right low 8 b

    drvLcdWriteCmd(d, 0x2b);               // set vert start , end (top, bot)
    drvLcdWriteData(d, (top >> 8) & 0xff); // top high 8 b
    drvLcdWriteData(d, top & 0xff);        // top low 8 b
    drvLcdWriteData(d, (bot >> 8) & 0xff); // bot high 8 b
    drvLcdWriteData(d, bot & 0xff);        // bot low 8 b

    drvLcdWriteCmd(d, 0x2c); // recover memory write mode
}

static uint32_t prvSt7789ReadId(drvLcd_t *d)
{
    uint8_t id[4];
    drvLcdReadData(d, LCD_CMD_READ_ID, id, 4);

    uint32_t dev_id = (id[3] << 16) | (id[2] << 8) | id[1];
    OSI_LOGI(0, "St7789 read id: 0x%08x", dev_id);
    return dev_id;
}

static void prvSt7789Init(drvLcd_t *d)
{
        OSI_LOGI(0, "lcd:    _st7789vInit ");
        const drvLcdPanelDesc_t *desc = drvLcdGetDesc(d);
    
        drvLcdWriteCmd(d,0x11);    //Sleep out
        osiThreadSleep(120);               //Delay 120ms
    //------------------------------------Display and color format setting-----------------------------------------//
        prvSt7789SetDir(d, desc->dir);
      
        drvLcdWriteCmd(d,0x3A);    //Interface pixel format
        drvLcdWriteData(d,0x05);    //Format is RGB565
    
        //------------------------------------ST7789V Frame Rate-----------------------------------------//
        drvLcdWriteCmd(d,0xB2);    //Porch control
        drvLcdWriteData(d,0x0C);
        drvLcdWriteData(d,0x0C);
        drvLcdWriteData(d,0x00);
        drvLcdWriteData(d,0x33);
        drvLcdWriteData(d,0x33);
    
        drvLcdWriteCmd(d,0xB7);
        drvLcdWriteData(d,0x75);
        //------------------------------------ST7789V Power Sequence-------------------------------------//
        drvLcdWriteCmd(d,0xBB);    //VCOMS Setting
        drvLcdWriteData(d,0x1C);
    
        drvLcdWriteCmd(d,0xC0);    //LCM Control
        drvLcdWriteData(d,0x2C);
    
        drvLcdWriteCmd(d,0xC2);    //VDV and VRH Command Enable
        drvLcdWriteData(d,0x01);
    
        drvLcdWriteCmd(d,0xC3);    //VRH Set
        drvLcdWriteData(d,0x13);
    
        drvLcdWriteCmd(d,0xC4);    //VDV Setting
        drvLcdWriteData(d,0x20);
    
        drvLcdWriteCmd(d,0xC6);    //FR Control 2
        drvLcdWriteData(d,0x0F);    //60Hz
    
        drvLcdWriteCmd(d,0xD0);    //Power Control 1
        drvLcdWriteData(d,0xA4);
        drvLcdWriteData(d,0xA1);
        //------------------------------------ST7789V Gamma Sequence-----------------------------------------//
        drvLcdWriteCmd(d,0xE0);
        drvLcdWriteData(d,0xD0);
        drvLcdWriteData(d,0x0C);
        drvLcdWriteData(d,0x12);
        drvLcdWriteData(d,0x0E);
        drvLcdWriteData(d,0x0D);
        drvLcdWriteData(d,0x08);
        drvLcdWriteData(d,0x36);
        drvLcdWriteData(d,0x44);
        drvLcdWriteData(d,0x4B);
        drvLcdWriteData(d,0x39);
        drvLcdWriteData(d,0x15);
        drvLcdWriteData(d,0x15);
        drvLcdWriteData(d,0x2C);
        drvLcdWriteData(d,0x2F);
    
        drvLcdWriteCmd(d,0xE1);
        drvLcdWriteData(d,0xD0);
        drvLcdWriteData(d,0x09);
        drvLcdWriteData(d,0x0E);
        drvLcdWriteData(d,0x08);
        drvLcdWriteData(d,0x07);
        drvLcdWriteData(d,0x14);
        drvLcdWriteData(d,0x35);
        drvLcdWriteData(d,0x43);
        drvLcdWriteData(d,0x4B);
        drvLcdWriteData(d,0x36);
        drvLcdWriteData(d,0x12);
        drvLcdWriteData(d,0x13);
        drvLcdWriteData(d,0x29);
        drvLcdWriteData(d,0x2D);
    
        drvLcdWriteCmd(d,0x21);
    
        drvLcdWriteCmd(d,0x29);    //Display on
}

static bool prvSt7789Probe(drvLcd_t *d)
{
    const drvLcdPanelDesc_t *desc = drvLcdGetDesc(d);

    OSI_LOGI(0, "St7789 probe");
    return prvSt7789ReadId(d) == desc->dev_id;
}
const drvLcdPanelDesc_t gLcdSt7789Desc = {
    .ops = {
        .probe = prvSt7789Probe,
        .init = prvSt7789Init,
        .blit_prepare = prvSt7789BlitPrepare,
    },
    .name = "ST7789v",
    .dev_id = 0x858552,
    .reset_us = 20 * 1000,
    .init_delay_us = 100 * 1000,
    .width = 240,
    .height = 320,
    .out_fmt = DRV_LCD_OUT_FMT_16BIT_RGB565,
    .dir = DRV_LCD_DIR_NORMAL,
    .line_mode = DRV_LCD_SPI_4WIRE,
    .fmark_enabled = false,
    .fmark_delay = 0x2a000,
    .freq = 50 * 1000000,
    .frame_us = (unsigned)(1000000 / 28.0),
};

