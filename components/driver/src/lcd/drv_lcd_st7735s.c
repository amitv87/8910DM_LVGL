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

#include "osi_api.h"
#include "osi_log.h"
#include "hwregs.h"
#include "drv_config.h"
#include "hal_gouda.h"
#include "hal_chip.h"
#include "drv_lcd.h"
#include "quec_proj_config.h"

#define LCD_DRV_ID_ST7735 0x7c89f0
#define LCD_WIDTH 128
#define LCD_HEIGHT 128

static uint16_t st7735_line_mode;
static bool st7735_farmk;

#define COL_OFFSET (3)
#define ROW_OFFSET (2)


#define LCM_WR_REG(Addr, Data)                             \
    {                                                      \
        while (halGoudaWriteReg(Addr, Data) != HAL_ERR_NO) \
            ;                                              \
    }
#define LCD_DataWrite_ST7735(Data)                    \
    {                                                 \
        while (halGoudaWriteData(Data) != HAL_ERR_NO) \
            ;                                         \
    }
#define LCD_CtrlWrite_ST7735(Cmd)                   \
    {                                               \
        while (halGoudaWriteCmd(Cmd) != HAL_ERR_NO) \
            ;                                       \
    }

static void _st7735Close(void);
static uint32_t _st7735ReadId(void);
static void _st7735SleepIn(bool is_sleep);
static void _st7735Init(void);
static void _st7735Invalidate(void);
static void _st7735InvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
);

static void _st7735SetDisplayWindow(
    uint16_t left,  // start Horizon address
    uint16_t right, // end Horizon address
    uint16_t top,   // start Vertical address
    uint16_t bottom // end Vertical address
);
static void _lcdDelayMs(int ms_delay)
{
    osiDelayUS(ms_delay * 1000);
}

OSI_UNUSED static void _st7735SetDirection(lcdDirect_t direct_type)
{

    OSI_LOGI(0, "lcd:    _st7735SetDirection = %d", direct_type);

    switch (direct_type)
    {
    case LCD_DIRECT_NORMAL:
        LCD_CtrlWrite_ST7735(0x36);
        LCD_DataWrite_ST7735(0x00);
        break;
    case LCD_DIRECT_ROT_90:
        LCD_CtrlWrite_ST7735(0x36);
        LCD_DataWrite_ST7735(0x60);
        break;
    }

    LCD_CtrlWrite_ST7735(0x2c);
}

/******************************************************************************/
//  Description:   Set the windows address to display, in this windows
//                 color is  refreshed.
/******************************************************************************/
static void _st7735SetDisplayWindow(
    uint16_t left,  // start Horizon address
    uint16_t top,   // start Vertical address
    uint16_t right, // end Horizon address
    uint16_t bottom // end Vertical address
)
{
    uint16_t newleft = left + ROW_OFFSET;
    uint16_t newtop = top + COL_OFFSET;
    uint16_t newright = right + ROW_OFFSET;
    uint16_t newbottom = bottom + COL_OFFSET;
    OSI_LOGI(0, "lcd:st7735SetDisplayWindow L = %d, top = %d, R = %d, bot = %d", left, top, right, bottom);

    LCD_CtrlWrite_ST7735(0x2a);                   // set hori start , end (left, right)
    LCD_DataWrite_ST7735((newleft >> 8) & 0xff);  // left high 8 b
    LCD_DataWrite_ST7735(newleft & 0xff);         // left low 8 b
    LCD_DataWrite_ST7735((newright >> 8) & 0xff); // right high 8 b
    LCD_DataWrite_ST7735(newright & 0xff);        // right low 8 b

    LCD_CtrlWrite_ST7735(0x2b);                    // set vert start , end (top, bot)
    LCD_DataWrite_ST7735((newtop >> 8) & 0xff);    // top high 8 b
    LCD_DataWrite_ST7735(newtop & 0xff);           // top low 8 b
    LCD_DataWrite_ST7735((newbottom >> 8) & 0xff); // bot high 8 b
    LCD_DataWrite_ST7735(newbottom & 0xff);        // bot low 8 b
    LCD_CtrlWrite_ST7735(0x2c);                    // recover memory write mode
}

/**************************************************************************************/
// Description: initialize all LCD with LCDC MCU MODE and LCDC mcu mode
/**************************************************************************************/
static void _st7735Init(void)
{
	OSI_LOGI(0, "lcd:    _st7735Init ");
	//HW_Reset();                

	_lcdDelayMs(120);                //ms

	LCD_CtrlWrite_ST7735(0x11);     

	_lcdDelayMs(120);                //ms

	LCD_CtrlWrite_ST7735(0xB1);     
	LCD_DataWrite_ST7735(0x05);   
	LCD_DataWrite_ST7735(0x3C);   
	LCD_DataWrite_ST7735(0x3C);   

	LCD_CtrlWrite_ST7735(0xB2);     
	LCD_DataWrite_ST7735(0x05);   
	LCD_DataWrite_ST7735(0x3C);   
	LCD_DataWrite_ST7735(0x3C);   

	LCD_CtrlWrite_ST7735(0xB3);     
	LCD_DataWrite_ST7735(0x05);   
	LCD_DataWrite_ST7735(0x3C);   
	LCD_DataWrite_ST7735(0x3C);   
	LCD_DataWrite_ST7735(0x05);   
	LCD_DataWrite_ST7735(0x3C);   
	LCD_DataWrite_ST7735(0x3C);   

	LCD_CtrlWrite_ST7735(0xB4);     
	LCD_DataWrite_ST7735(0x03);   

	LCD_CtrlWrite_ST7735(0xC0);     
	LCD_DataWrite_ST7735(0x28);   
	LCD_DataWrite_ST7735(0x08);   
	LCD_DataWrite_ST7735(0x84);   

	LCD_CtrlWrite_ST7735(0xC1);     
	LCD_DataWrite_ST7735(0x05);   

	LCD_CtrlWrite_ST7735(0xC2);     
	LCD_DataWrite_ST7735(0x0D);   
	LCD_DataWrite_ST7735(0x00);   

	LCD_CtrlWrite_ST7735(0xC3);     
	LCD_DataWrite_ST7735(0x8D);   
	LCD_DataWrite_ST7735(0xEA);   

	LCD_CtrlWrite_ST7735(0xC4);     
	LCD_DataWrite_ST7735(0x8D);   
	LCD_DataWrite_ST7735(0xEE);   

	LCD_CtrlWrite_ST7735(0xC5);     
	LCD_DataWrite_ST7735(0x00);   

	LCD_CtrlWrite_ST7735(0x36);     
	LCD_DataWrite_ST7735(0xC8);   

	LCD_CtrlWrite_ST7735(0x3A);     
	LCD_DataWrite_ST7735(0x05); 

	LCD_CtrlWrite_ST7735(0xE0);     
	LCD_DataWrite_ST7735(0x1B);   
	LCD_DataWrite_ST7735(0x1A);   
	LCD_DataWrite_ST7735(0x10);   
	LCD_DataWrite_ST7735(0x15);   
	LCD_DataWrite_ST7735(0x34);   
	LCD_DataWrite_ST7735(0x2E);   
	LCD_DataWrite_ST7735(0x2A);   
	LCD_DataWrite_ST7735(0x2A);   
	LCD_DataWrite_ST7735(0x2C);   
	LCD_DataWrite_ST7735(0x27);   
	LCD_DataWrite_ST7735(0x2E);   
	LCD_DataWrite_ST7735(0x39);   
	LCD_DataWrite_ST7735(0x00);   
	LCD_DataWrite_ST7735(0x15);   
	LCD_DataWrite_ST7735(0x02);   
	LCD_DataWrite_ST7735(0x3F);   

	LCD_CtrlWrite_ST7735(0xE1);     
	LCD_DataWrite_ST7735(0x1A);   
	LCD_DataWrite_ST7735(0x1A);   
	LCD_DataWrite_ST7735(0x0F);   
	LCD_DataWrite_ST7735(0x15);   
	LCD_DataWrite_ST7735(0x34);   
	LCD_DataWrite_ST7735(0x2E);   
	LCD_DataWrite_ST7735(0x25);   
	LCD_DataWrite_ST7735(0x2B);   
	LCD_DataWrite_ST7735(0x26);   
	LCD_DataWrite_ST7735(0x26);   
	LCD_DataWrite_ST7735(0x2F);   
	LCD_DataWrite_ST7735(0x3A);   
	LCD_DataWrite_ST7735(0x00);   
	LCD_DataWrite_ST7735(0x15);   
	LCD_DataWrite_ST7735(0x02);   
	LCD_DataWrite_ST7735(0x3F);   

	LCD_CtrlWrite_ST7735(0x29);     

 
}

static void _st7735SleepIn(bool is_sleep)
{
    OSI_LOGI(0, "lcd:   _st7735SleepIn, is_sleep = %d", is_sleep);

    if (is_sleep)
    {
        LCD_CtrlWrite_ST7735(0x28); //display off
        _lcdDelayMs(120);
        LCD_CtrlWrite_ST7735(0x10); // enter sleep mode
    }
    else
    {

#if 0
        LCD_CtrlWrite_ST7735(0x11); // sleep out mode
        _lcdDelayMs(120);
        LCD_CtrlWrite_ST7735(0x29); // display on
#endif
        _st7735Init();
    }
}

static void _st7735Close(void)
{
    OSI_LOGI(0, "lcd:     in GC9304_Close");

    _st7735SleepIn(true);
}
static void _st7735Invalidate(void)
{
    OSI_LOGI(0, "lcd:     in _st7735Invalidate");

    _st7735SetDisplayWindow(0x0, 0x0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
}

void _st7735InvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
)
{

    OSI_LOGI(0, "lcd:  _st7735InvalidateRect");
    left = (left >= LCD_WIDTH) ? LCD_WIDTH - 1 : left;
    right = (right >= LCD_WIDTH) ? LCD_WIDTH - 1 : right;
    top = (top >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : top;
    bottom = (bottom >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : bottom;

    if ((right < left) || (bottom < top))
    {

        OSI_ASSERT(false, "lcd:  _st7735InvalidateRect error"); /*assert verified*/
    }

    _st7735SetDisplayWindow(left, top, right, bottom);
}

static void _st7735RotationInvalidateRect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, lcdAngle_t angle)
{

    OSI_LOGI(0, "lcd: gc9305 gc9305RotationInvalidateRect");
    switch (angle)
    {
    case LCD_ANGLE_0:
        _st7735SetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_90:
        _st7735SetDisplayWindow(left, top, bottom, right);
        break;
    case LCD_ANGLE_180:
        _st7735SetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_270:
        _st7735SetDisplayWindow(left, top, bottom, right);
        break;
    default:
        _st7735SetDisplayWindow(left, top, right, bottom);
        break;
    }
}

static void _st7735EsdCheck(void)
{

    OSI_LOGI(0, "lcd:  st7735sEsdCheck");
}

static const lcdOperations_t st7735sOperations =
    {
        _st7735Init,
        _st7735SleepIn,
        _st7735EsdCheck,
        _st7735SetDisplayWindow,
        _st7735InvalidateRect,
        _st7735Invalidate,
        _st7735Close,
        _st7735RotationInvalidateRect,
        NULL,
        _st7735ReadId,
};

const lcdSpec_t g_lcd_st7735s =
    {
        LCD_DRV_ID_ST7735,
        LCD_WIDTH,
        LCD_HEIGHT,
        HAL_GOUDA_SPI_LINE_4,
        LCD_CTRL_SPI,
        (lcdOperations_t *)&st7735sOperations,
        false,
        0x2a000,
       10000000,// 500000, //10000000, 10M
};

static uint32_t _st7735ReadId(void)
{
    uint32_t ret_id = 0;
    HAL_ERR_T r_err;
    uint8_t id[4] = {0};
    OSI_LOGI(0, "lcd:st7735ReadId \n");
#ifndef CONFIG_QUEC_PROJECT_FEATURE_LCD
    halPmuSwitchPower(HAL_POWER_LCD, true, true);
#endif
    _lcdDelayMs(10);
    st7735_farmk = g_lcd_st7735s.is_use_fmark;
    st7735_line_mode = g_lcd_st7735s.bus_mode;

    halGoudaReadConfig(st7735_line_mode, g_lcd_st7735s.is_use_fmark, g_lcd_st7735s.fmark_delay);
    _lcdDelayMs(140);
    //hwp_analogReg->pad_spi_lcd_cfg2 |= ANALOG_REG_PAD_SPI_LCD_SIO_SPU;
    _lcdDelayMs(10);
    r_err = halGoudaReadData(0x04, id, 4);
    //hwp_analogReg->pad_spi_lcd_cfg2 =  hwp_analogReg->pad_spi_lcd_cfg2&(~ANALOG_REG_PAD_SPI_LCD_SIO_SPU);
    OSI_LOGI(0, "lcd:st7735ReadId LCM: 0x%0x, 0x%x, 0x%0x (read return : %d)\n", id[2], id[1], id[3], r_err);
    ret_id = ((id[3] << 16) | (id[2] << 8) | id[1]);

    if (LCD_DRV_ID_ST7735 == ret_id)
    {
        OSI_LOGI(0, "lcd:   is using !");
    }
    halGoudaClose();
#ifndef CONFIG_QUEC_PROJECT_FEATURE_LCD
    halPmuSwitchPower(HAL_POWER_LCD, false, false);
#endif
    osiDebugEvent(0x666661d);osiDebugEvent(ret_id);
    return ret_id;
}


