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

static void prvGc9305SetDir(drvLcd_t *d, drvLcdDirection_t dir)
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

static void prvGc9305Init(drvLcd_t *d)
{
    const drvLcdPanelDesc_t *desc = drvLcdGetDesc(d);

    drvLcdWriteCmd(d, 0xfe); // inter register enable1
    drvLcdWriteCmd(d, 0xef); // inter register enable2

    prvGc9305SetDir(d, desc->dir);

    drvLcdWriteCmd(d, 0x3a); // pixel format set
    drvLcdWriteData(d, 0x05);

    drvLcdWriteCmd(d, 0xa4); // power control 7
    drvLcdWriteData(d, 0x44);
    drvLcdWriteData(d, 0x44);

    drvLcdWriteCmd(d, 0xa5);
    drvLcdWriteData(d, 0x42);
    drvLcdWriteData(d, 0x42);

    drvLcdWriteCmd(d, 0xaa);
    drvLcdWriteData(d, 0x88);
    drvLcdWriteData(d, 0x88);

    if (desc->fmark_enabled)
    {
        drvLcdWriteCmd(d, 0x44); // set tear scanline
        drvLcdWriteData(d, 0x0);
        drvLcdWriteData(d, 0x1);

        drvLcdWriteCmd(d, 0x35); // tearing effect line on
        drvLcdWriteData(d, 0x0);

        drvLcdWriteCmd(d, 0xe8);  // frame rate
        drvLcdWriteData(d, 0x08); // 4=41hz 7=30hz 8=28hz
        drvLcdWriteData(d, 0x40);
    }
    else
    {
        drvLcdWriteCmd(d, 0xe8); // frame rate
        drvLcdWriteData(d, 0x11);
        drvLcdWriteData(d, 0x77);
    }

    drvLcdWriteCmd(d, 0xe3);
    drvLcdWriteData(d, 0x01);
    drvLcdWriteData(d, 0x10);

    drvLcdWriteCmd(d, 0xff);
    drvLcdWriteData(d, 0x61);

    drvLcdWriteCmd(d, 0xac);
    drvLcdWriteData(d, 0x00);

    drvLcdWriteCmd(d, 0xaf);
    drvLcdWriteData(d, 0x67);

    drvLcdWriteCmd(d, 0xa6);
    drvLcdWriteData(d, 0x2a);
    drvLcdWriteData(d, 0x2a);

    drvLcdWriteCmd(d, 0xa7);
    drvLcdWriteData(d, 0x2b);
    drvLcdWriteData(d, 0x2b);

    drvLcdWriteCmd(d, 0xa8);
    drvLcdWriteData(d, 0x18);
    drvLcdWriteData(d, 0x18);

    drvLcdWriteCmd(d, 0xa9);
    drvLcdWriteData(d, 0x2a);
    drvLcdWriteData(d, 0x2a);

    drvLcdWriteCmd(d, 0x2a);
    drvLcdWriteData(d, 0x00);
    drvLcdWriteData(d, 0x00);
    drvLcdWriteData(d, 0x00);
    drvLcdWriteData(d, 0xef);

    drvLcdWriteCmd(d, 0x2b);
    drvLcdWriteData(d, 0x00);
    drvLcdWriteData(d, 0x00);
    drvLcdWriteData(d, 0x01);
    drvLcdWriteData(d, 0x3f);
    drvLcdWriteCmd(d, 0x2c);

    drvLcdWriteCmd(d, 0xf0);
    drvLcdWriteData(d, 0x02);
    drvLcdWriteData(d, 0x00);
    drvLcdWriteData(d, 0x00);
    drvLcdWriteData(d, 0x1b);
    drvLcdWriteData(d, 0x1f);
    drvLcdWriteData(d, 0xb);

    drvLcdWriteCmd(d, 0xf1);
    drvLcdWriteData(d, 0x01);
    drvLcdWriteData(d, 0x3);
    drvLcdWriteData(d, 0x00);
    drvLcdWriteData(d, 0x28);
    drvLcdWriteData(d, 0x2b);
    drvLcdWriteData(d, 0xe);

    drvLcdWriteCmd(d, 0xf2);
    drvLcdWriteData(d, 0xb);
    drvLcdWriteData(d, 0x08);
    drvLcdWriteData(d, 0x3b);
    drvLcdWriteData(d, 0x04);
    drvLcdWriteData(d, 0x03);
    drvLcdWriteData(d, 0x4c);

    drvLcdWriteCmd(d, 0xf3);
    drvLcdWriteData(d, 0xe);
    drvLcdWriteData(d, 0x7);
    drvLcdWriteData(d, 0x46);
    drvLcdWriteData(d, 0x4);
    drvLcdWriteData(d, 0x5);
    drvLcdWriteData(d, 0x51);

    drvLcdWriteCmd(d, 0xf4);
    drvLcdWriteData(d, 0x08);
    drvLcdWriteData(d, 0x15);
    drvLcdWriteData(d, 0x15);
    drvLcdWriteData(d, 0x1f);
    drvLcdWriteData(d, 0x22);
    drvLcdWriteData(d, 0x0F);

    drvLcdWriteCmd(d, 0xf5);
    drvLcdWriteData(d, 0xb);
    drvLcdWriteData(d, 0x13);
    drvLcdWriteData(d, 0x11);
    drvLcdWriteData(d, 0x1f);
    drvLcdWriteData(d, 0x21);
    drvLcdWriteData(d, 0x0F);

    drvLcdWriteCmd(d, 0x11); // sleep out mode
    osiThreadSleep(120);

    drvLcdWriteCmd(d, 0x29); // display on
    osiThreadSleep(20);

    drvLcdWriteCmd(d, 0x2c); // memory write

    if (desc->line_mode == DRV_LCD_SPI_3WIRE_2LANE)
    {
        drvLcdWriteCmd(d, 0xe9); // spi 2 data control
        drvLcdWriteData(d, 0x08);
    }
}

static void prvGc9305BlitPrepare(drvLcd_t *d, drvLcdDirection_t dir, const drvLcdArea_t *roi)
{
    OSI_LOGD(0, "GC9305 dir/%d roi/%d/%d/%d/%d", dir, roi->x, roi->y, roi->w, roi->h);

    prvGc9305SetDir(d, dir);

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

static uint32_t prvGc9305ReadId(drvLcd_t *d)
{
    uint8_t id[4];
    drvLcdReadData(d, LCD_CMD_READ_ID, id, 4);

    uint32_t dev_id = (id[3] << 16) | (id[2] << 8) | id[1];
    OSI_LOGI(0, "GC9305 read id: 0x%08x", dev_id);
    return dev_id;
}

static bool prvGc9305Probe(drvLcd_t *d)
{
    const drvLcdPanelDesc_t *desc = drvLcdGetDesc(d);

    OSI_LOGI(0, "GC9305 probe");
    return prvGc9305ReadId(d) == desc->dev_id;
}

const drvLcdPanelDesc_t gLcdGc9305Desc = {
    .ops = {
        .probe = prvGc9305Probe,
        .init = prvGc9305Init,
        .blit_prepare = prvGc9305BlitPrepare,
    },
    .name = "GC9305",
    .dev_id = 0x009305,
    .reset_us = 20 * 1000,
    .init_delay_us = 100 * 1000,
    .width = 240,
    .height = 320,
    .out_fmt = DRV_LCD_OUT_FMT_16BIT_RGB565,
    .dir = DRV_LCD_DIR_XINV,
    .line_mode = DRV_LCD_SPI_4WIRE,
    .fmark_enabled = false,
    .fmark_delay = 0x2a000,
    .freq = 50 * 1000000,
    .frame_us = (unsigned)(1000000 / 28.0),
};
