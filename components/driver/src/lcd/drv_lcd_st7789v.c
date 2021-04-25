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

#define LCD_DRV_ID_ST7789V 0x858552
#define LCD_WIDTH  240
#define LCD_HEIGHT 320

static uint16_t st7789v_line_mode;
static bool st7789v_farmk;



#define LCM_WR_REG(Addr, Data)                             \
    {                                                      \
        while (halGoudaWriteReg(Addr, Data) != HAL_ERR_NO) \
            ;                                              \
    }
#define LCD_DataWrite_ST7789V(Data)                    \
    {                                                 \
        while (halGoudaWriteData(Data) != HAL_ERR_NO) \
            ;                                         \
    }
#define LCD_CtrlWrite_ST7789V(Cmd)                   \
    {                                               \
        while (halGoudaWriteCmd(Cmd) != HAL_ERR_NO) \
            ;                                       \
    }

static void _st7789vClose(void);
static uint32_t _st7789vReadId(void);
static void _st7789vSleepIn(bool is_sleep);
static void _st7789vInit(void);
static void _st7789vInvalidate(void);
static void _st7789vInvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
);

static void _st7789vSetDisplayWindow(
    uint16_t left,  // start Horizon address
    uint16_t right, // end Horizon address
    uint16_t top,   // start Vertical address
    uint16_t bottom // end Vertical address
);
static void _lcdDelayMs(int ms_delay)
{
    osiDelayUS(ms_delay * 1000);
}

OSI_UNUSED static void _st7789vSetDirection(lcdDirect_t direct_type)
{

    OSI_LOGI(0, "lcd:    _st7789vSetDirection = %d", direct_type);

    switch (direct_type)
    {
    case LCD_DIRECT_NORMAL:
        LCD_CtrlWrite_ST7789V(0x36);
        LCD_DataWrite_ST7789V(0x00);
        break;
    case LCD_DIRECT_ROT_90:
        LCD_CtrlWrite_ST7789V(0x36);
        LCD_DataWrite_ST7789V(0x60);
        break;
    }

    LCD_CtrlWrite_ST7789V(0x2c);
}

/******************************************************************************/
//  Description:   Set the windows address to display, in this windows
//                 color is  refreshed.
/******************************************************************************/
static void _st7789vSetDisplayWindow(
    uint16_t left,  // start Horizon address
    uint16_t top,   // start Vertical address
    uint16_t right, // end Horizon address
    uint16_t bottom // end Vertical address
)
{
    uint16_t newleft = left;
    uint16_t newtop = top;
    uint16_t newright = right;
    uint16_t newbottom = bottom;
    OSI_LOGI(0, "lcd:st7735SetDisplayWindow L = %d, top = %d, R = %d, bot = %d", left, top, right, bottom);

    LCD_CtrlWrite_ST7789V(0x2a);                   // set hori start , end (left, right)
    LCD_DataWrite_ST7789V((newleft >> 8) & 0xff);  // left high 8 b
    LCD_DataWrite_ST7789V(newleft & 0xff);         // left low 8 b
    LCD_DataWrite_ST7789V((newright >> 8) & 0xff); // right high 8 b
    LCD_DataWrite_ST7789V(newright & 0xff);        // right low 8 b

    LCD_CtrlWrite_ST7789V(0x2b);                    // set vert start , end (top, bot)
    LCD_DataWrite_ST7789V((newtop >> 8) & 0xff);    // top high 8 b
    LCD_DataWrite_ST7789V(newtop & 0xff);           // top low 8 b
    LCD_DataWrite_ST7789V((newbottom >> 8) & 0xff); // bot high 8 b
    LCD_DataWrite_ST7789V(newbottom & 0xff);        // bot low 8 b

    LCD_CtrlWrite_ST7789V(0x2c);                    // recover memory write mode
}

/**************************************************************************************/
// Description: initialize all LCD with LCDC MCU MODE and LCDC mcu mode
/**************************************************************************************/
static void _st7789vInit(void)
{
    OSI_LOGI(0, "lcd:    _st7789vInit ");

    LCD_CtrlWrite_ST7789V(0x11);    //Sleep out
    _lcdDelayMs(120);               //Delay 120ms

    //------------------------------------Display and color format setting-----------------------------------------//
    LCD_CtrlWrite_ST7789V(0x36);    //Memory data access control
    LCD_DataWrite_ST7789V(0x00);    //Normal display

    LCD_CtrlWrite_ST7789V(0x3A);    //Interface pixel format
    LCD_DataWrite_ST7789V(0x05);    //Format is RGB565

    LCD_CtrlWrite_ST7789V(0x35);
    LCD_DataWrite_ST7789V(0x00);

    LCD_CtrlWrite_ST7789V(0xC7);
    LCD_DataWrite_ST7789V(0x00);

    LCD_CtrlWrite_ST7789V(0xCC);
    LCD_DataWrite_ST7789V(0x09);
    //------------------------------------ST7789V Frame Rate-----------------------------------------//
    LCD_CtrlWrite_ST7789V(0xB2);    //Porch control
    LCD_DataWrite_ST7789V(0x0C);
    LCD_DataWrite_ST7789V(0x0C);
    LCD_DataWrite_ST7789V(0x00);
    LCD_DataWrite_ST7789V(0x33);
    LCD_DataWrite_ST7789V(0x33);

    LCD_CtrlWrite_ST7789V(0xB7);
    LCD_DataWrite_ST7789V(0x35);    //vgh=13.26 vgl=-12.5    //vgl=-10.43 ???
    //------------------------------------ST7789V Power Sequence-------------------------------------//
    LCD_CtrlWrite_ST7789V(0xBB);    //VCOMS Setting
    LCD_DataWrite_ST7789V(0x36);

    LCD_CtrlWrite_ST7789V(0xC0);    //LCM Control
    LCD_DataWrite_ST7789V(0x2C);

    LCD_CtrlWrite_ST7789V(0xC2);    //VDV and VRH Command Enable
    LCD_DataWrite_ST7789V(0x01);

    LCD_CtrlWrite_ST7789V(0xC3);    //VRH Set
    LCD_DataWrite_ST7789V(0x0D);

    LCD_CtrlWrite_ST7789V(0xC4);    //VDV Setting
    LCD_DataWrite_ST7789V(0x20);

    LCD_CtrlWrite_ST7789V(0xC6);    //FR Control 2
    LCD_DataWrite_ST7789V(0x0F);    //60Hz

    LCD_CtrlWrite_ST7789V(0xD0);    //Power Control 1
    LCD_DataWrite_ST7789V(0xA4);
    LCD_DataWrite_ST7789V(0xA1);
    //------------------------------------ST7789V Gamma Sequence-----------------------------------------//
    LCD_CtrlWrite_ST7789V(0xE0);
    LCD_DataWrite_ST7789V(0xD0);
    LCD_DataWrite_ST7789V(0x17);
    LCD_DataWrite_ST7789V(0x19);
    LCD_DataWrite_ST7789V(0x04);
    LCD_DataWrite_ST7789V(0x03);
    LCD_DataWrite_ST7789V(0x04);
    LCD_DataWrite_ST7789V(0x32);
    LCD_DataWrite_ST7789V(0x41);
    LCD_DataWrite_ST7789V(0x43);
    LCD_DataWrite_ST7789V(0x09);
    LCD_DataWrite_ST7789V(0x14);
    LCD_DataWrite_ST7789V(0x12);
    LCD_DataWrite_ST7789V(0x33);
    LCD_DataWrite_ST7789V(0x2C);

    LCD_CtrlWrite_ST7789V(0xE1);
    LCD_DataWrite_ST7789V(0xD0);
    LCD_DataWrite_ST7789V(0x18);
    LCD_DataWrite_ST7789V(0x17);
    LCD_DataWrite_ST7789V(0x04);
    LCD_DataWrite_ST7789V(0x03);
    LCD_DataWrite_ST7789V(0x04);
    LCD_DataWrite_ST7789V(0x31);
    LCD_DataWrite_ST7789V(0x46);
    LCD_DataWrite_ST7789V(0x43);
    LCD_DataWrite_ST7789V(0x09);
    LCD_DataWrite_ST7789V(0x14);
    LCD_DataWrite_ST7789V(0x13);
    LCD_DataWrite_ST7789V(0x31);
    LCD_DataWrite_ST7789V(0x2D);

    LCD_CtrlWrite_ST7789V(0x29);    //Display on

    _lcdDelayMs(20);
    LCD_CtrlWrite_ST7789V(0x2c);
}

static void _st7789vSleepIn(bool is_sleep)
{
    OSI_LOGI(0, "lcd:   _st7789vSleepIn, is_sleep = %d", is_sleep);

    if (is_sleep)
    {
        LCD_CtrlWrite_ST7789V(0x28); // display off
        _lcdDelayMs(120);
        LCD_CtrlWrite_ST7789V(0x10); // enter sleep mode
    }
    else
    {

#if 0
        LCD_CtrlWrite_ST7789V(0x11); // sleep out mode
        _lcdDelayMs(120);
        LCD_CtrlWrite_ST7789V(0x29); // display on
#endif
        _st7789vInit();
    }
}

static void _st7789vClose(void)
{
    OSI_LOGI(0, "lcd:     in st7789v_Close");

    _st7789vSleepIn(true);
}
static void _st7789vInvalidate(void)
{
    OSI_LOGI(0, "lcd:     in _st7789vInvalidate");

    _st7789vSetDisplayWindow(0x0, 0x0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
}

void _st7789vInvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
)
{

    OSI_LOGI(0, "lcd:  _st7789vInvalidateRect");
    left = (left >= LCD_WIDTH) ? LCD_WIDTH - 1 : left;
    right = (right >= LCD_WIDTH) ? LCD_WIDTH - 1 : right;
    top = (top >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : top;
    bottom = (bottom >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : bottom;

    if ((right < left) || (bottom < top))
    {

        OSI_ASSERT(false, "lcd:  _st7789vInvalidateRect error"); /*assert verified*/
    }

    _st7789vSetDisplayWindow(left, top, right, bottom);
}

static void _st7789vRotationInvalidateRect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, lcdAngle_t angle)
{

    OSI_LOGI(0, "lcd: st7789v st7789vRotationInvalidateRect");
    switch (angle)
    {
    case LCD_ANGLE_0:
        _st7789vSetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_90:
        _st7789vSetDisplayWindow(left, top, bottom, right);
        break;
    case LCD_ANGLE_180:
        _st7789vSetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_270:
        _st7789vSetDisplayWindow(left, top, bottom, right);
        break;
    default:
        _st7789vSetDisplayWindow(left, top, right, bottom);
        break;
    }
}

static void _st7789vEsdCheck(void)
{

    OSI_LOGI(0, "lcd:  st7789vEsdCheck");
}

static const lcdOperations_t st7789vOperations =
    {
        _st7789vInit,
        _st7789vSleepIn,
        _st7789vEsdCheck,
        _st7789vSetDisplayWindow,
        _st7789vInvalidateRect,
        _st7789vInvalidate,
        _st7789vClose,
        _st7789vRotationInvalidateRect,
        NULL,
        _st7789vReadId,
};

const lcdSpec_t g_lcd_st7789v =
    {
        LCD_DRV_ID_ST7789V,
        LCD_WIDTH,
        LCD_HEIGHT,
        HAL_GOUDA_SPI_LINE_4,
        LCD_CTRL_SPI,
        (lcdOperations_t *)&st7789vOperations,
        false,
        0x2a000,
        50000000,
};

static uint32_t _st7789vReadId(void)
{
    uint32_t ret_id = 0;
    HAL_ERR_T r_err;
    uint8_t id[4] = {0};
    OSI_LOGI(0, "lcd:st7789vReadId \n");
    //halPmuSwitchPower(HAL_POWER_LCD, true, true);
    _lcdDelayMs(10);
    st7789v_farmk = g_lcd_st7789v.is_use_fmark;
    st7789v_line_mode = g_lcd_st7789v.bus_mode;

    halGoudaReadConfig(st7789v_line_mode, g_lcd_st7789v.is_use_fmark, g_lcd_st7789v.fmark_delay);
    _lcdDelayMs(140);
    //hwp_analogReg->pad_spi_lcd_cfg2 |= ANALOG_REG_PAD_SPI_LCD_SIO_SPU;
    _lcdDelayMs(10);
    r_err = halGoudaReadData(0x04, id, 4);
    //hwp_analogReg->pad_spi_lcd_cfg2 =  hwp_analogReg->pad_spi_lcd_cfg2&(~ANALOG_REG_PAD_SPI_LCD_SIO_SPU);
    OSI_LOGI(0, "lcd:st7789vReadId LCM: 0x%0x, 0x%x, 0x%0x (read return : %d)\n", id[2], id[1], id[3], r_err);
    ret_id = ((id[3] << 16) | (id[2] << 8) | id[1]);

    if (LCD_DRV_ID_ST7789V == ret_id)
    {
        OSI_LOGI(0, "lcd:   is using !");
    }
    halGoudaClose();
    //halPmuSwitchPower(HAL_POWER_LCD, false, false);
    osiDebugEvent(0x666661d);osiDebugEvent(ret_id);
    return ret_id;
}


