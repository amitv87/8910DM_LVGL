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

#ifndef _DRV_LCD_PANEL_H_
#define _DRV_LCD_PANEL_H_

#include "drv_lcd_v2.h"
#include "hwregs.h"

OSI_EXTERN_C_BEGIN

typedef enum
{
    DRV_LCD_SPI_4WIRE,
    DRV_LCD_SPI_3WIRE,
    DRV_LCD_SPI_4WIRE_START_BYTE,
    DRV_LCD_SPI_3WIRE_2LANE,
} drvLcdSpiLineMode_t;

typedef enum
{
    DRV_LCD_OUT_FMT_8BIT_RGB332 = 0,
    DRV_LCD_OUT_FMT_8BIT_RGB444 = 1,
    DRV_LCD_OUT_FMT_8BIT_RGB565 = 2,
    DRV_LCD_OUT_FMT_16BIT_RGB332 = 4,
    DRV_LCD_OUT_FMT_16BIT_RGB444 = 5,
    DRV_LCD_OUT_FMT_16BIT_RGB565 = 6,
} drvLcdOutputFormat_t;

/**
 * \brief LCD panel operations
 *
 * All functions can't be NULL. User won't check NULL pointer before calling
 * panel operations. Also, caller will take care thread safe.
 */
typedef struct
{
    /**
     * \brief probe the panel
     * \param d         LCD instance
     * \return
     *      - true if the panel matches the driver
     *      - false if not match
     */
    bool (*probe)(drvLcd_t *d);
    /**
     * \brief initialization
     * \param d         LCD instance
     */
    void (*init)(drvLcd_t *d);
    /**
     * \brief prepare data transfer, set direction and ROI
     * \param d         LCD instance
     * \param dir       direction
     * \param roi       region of interest
     */
    void (*blit_prepare)(drvLcd_t *d, drvLcdDirection_t dir, const drvLcdArea_t *roi);
} drvLcdPanelOps_t;

bool drvLcdDummyProbe(drvLcd_t *d);
void drvLcdDummyInit(drvLcd_t *d);
void drvLcdDummyBlitPrepare(drvLcd_t *d, drvLcdDirection_t dir, const drvLcdArea_t *roi);

/**
 * \brief panel configurations
 */
typedef struct
{
    /** panel operation */
    drvLcdPanelOps_t ops;
    /** name string */
    const char *name;
    /** device ID */
    unsigned dev_id;
    /** reset time in us */
    unsigned reset_us;
    /** delay time after reset in us */
    unsigned init_delay_us;
    /** width in normal direction */
    uint16_t width;
    /** height in normal direction */
    uint16_t height;
    /** output format */
    drvLcdOutputFormat_t out_fmt;
    /** panel initial direction */
    drvLcdDirection_t dir;
    /** SPI line mode */
    drvLcdSpiLineMode_t line_mode;
    /** fmark enabled */
    bool fmark_enabled;
    /** fmark delay */
    unsigned fmark_delay;
    /** SPI frequency */
    unsigned freq;
    /** frmae period in us */
    unsigned frame_us;
} drvLcdPanelDesc_t;

/**
 * \brief set panel description to LCD instance
 *
 * \param d         LCD instance
 * \param desc      panel description
 */
void drvLcdSetDesc(drvLcd_t *d, const drvLcdPanelDesc_t *desc);

/**
 * \brief get panel description from LCD instance
 *
 * \param d         LCD instance
 * \return
 *      - panel description
 */
const drvLcdPanelDesc_t *drvLcdGetDesc(drvLcd_t *d);

/**
 * \brief write command to panel
 *
 * This should be called only in panel driver, caller will ensure it is
 * called inside lock.
 *
 * \param d         LCD instance
 * \param cmd       command
 */
void drvLcdWriteCmd(drvLcd_t *d, uint16_t cmd);

/**
 * \brief write data to panel
 *
 * This should be called only in panel driver, caller will ensure it is
 * called inside lock.
 *
 * \param d         LCD instance
 * \param data      data
 */
void drvLcdWriteData(drvLcd_t *d, uint16_t data);

/**
 * \brief read data from panel by command
 */
void drvLcdReadData(drvLcd_t *d, uint16_t cmd, uint8_t *data, unsigned len);

OSI_EXTERN_C_END
#endif
