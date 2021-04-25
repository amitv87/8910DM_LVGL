/**  
  @file
  ql_boot_lcd.c

  @brief
  quectel boot lcd interface.

*/
/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------
08/12/2020        Neo         Init version
=================================================================*/


/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "drv_lcd.h"
#include "hal_gouda.h"
#include "hal_iomux.h"
#include "hal_chip.h"
#include "quec_proj_config.h"
#include "hwregs.h"
#include "hal_adi_bus.h"

#include "ql_boot_lcd.h"
#include "osi_log.h"
#include "osi_api.h"


/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define LCD_RED     0xf800
#define LCD_GREEN   0x07e0
#define LCD_BLUE    0x001f
        
#define LCD_WHITE   0xffff
#define LCD_BLACK   0x0000
        
#define LCD_YELLOW  0xffe0
#define LCD_PURPLE  0xf81f

#define QL_BOOT_LCDC_LAYER_NUM 4
#define QL_BOOT_BLOCK_SUM 4


typedef enum
{
    QL_BOOT_LCD_BRUSH_MODE_SYNC = 0,
    QL_BOOT_LCD_BRUSH_MODE_ASYNC
} QL_BOOT_LCD_BRUSH_MODE_E;

typedef struct
{
    QL_BOOT_LCD_BRUSH_MODE_E brush_mode; // 1 if brush lcd with synchronistic mean, or else 0
    uint32_t sleep_state;
    lcdSpec_t *lcd_spec;
} QL_BOOT_LCD_CONFIG_T;

/*========================================================================
 *  Global Variable
 *========================================================================*/
uint16_t ql_boot_image1[30*50] = {};
uint16_t ql_boot_image2[100*100] = {};
uint16_t *ql_boot_lcd_buffer = NULL;

lcdSpec_t *ql_boot_lcd_info;
QL_BOOT_LCD_CONFIG_T ql_boot_lcd_cfg;
    
static HAL_GOUDA_LCD_CONFIG_T ql_boot_gd_lcd_config =
    {

        .b.cs = HAL_GOUDA_LCD_CS_0,
        .b.outputFormat = HAL_GOUDA_LCD_OUTPUT_FORMAT_16_bit_RGB565,
        .b.cs0Polarity = false,
        .b.cs1Polarity = false,
        .b.resetb = true,
        .b.rsPolarity = false,
        .b.wrPolarity = false,
        .b.rdPolarity = false,
        .b.highByte = false,
};


/*===========================================================================
 * Functions declaration
 *===========================================================================*/
extern lcdSpec_t *drvLcdGetSpecInfo(void);
extern bool ql_boot_lcd_param_put(lcdSpec_t *boot_lcd_spec_info, void* boot_lcd_cfg);
extern uint16_t ql_boot_lcd_block_info_enable(int block_index);
extern void ql_boot_gouda_init(HAL_GOUDA_LCD_CONFIG_T cfg, uint32_t clk);

extern void drvLcdDisableAllBlock(void);
extern void drvLcdSetBlockBuffer(uint32_t blk_num, uint32_t *buf_ptr);
extern bool drvLcdConfigBlock(uint32_t blk_num, lcdBlockCfg_t config);
extern void drvLcdEnableBlock(uint32_t blk_num);
extern void drvLcdInvalidateRect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
extern void drvDisableBlock(uint32_t blk_num);
extern void drvLcdConfigGoudaLayers();


/*===========================================================================
 * Functions Definition
 *===========================================================================*/
static void ql_boot_lcd_handler(void)
{
    int i;

    halGoudaVidLayerClose();

    for (i = 1; i < QL_BOOT_LCDC_LAYER_NUM; i++)
    {
        if (ql_boot_lcd_block_info_enable(i))
        {
            halGoudaOvlLayerClose(i - 1);
            drvDisableBlock(i);
        }
    }

    drvLcdSetImageLayerSize(0, 0, 0);
}
 
static void ql_boot_lcd_gouda_update(const HAL_GOUDA_WINDOW_T *pWin, bool bSync)
{
    HAL_GOUDA_LCD_CMD_T cmd[13];

    if (!((pWin->tlPX < pWin->brPX) && (pWin->tlPY < pWin->brPY)))
    {
        ql_boot_lcd_handler();
        return;
    }
    if (bSync)
    {
        while (0 != halGoudaBlitRoi((HAL_GOUDA_WINDOW_T *)pWin, 0, cmd, NULL))
            ;
        while (halGoudaIsActive())
            ;
        ql_boot_lcd_handler();
    }
    else
    {
        while (0 != halGoudaBlitRoi((HAL_GOUDA_WINDOW_T *)pWin, 0, cmd, ql_boot_lcd_handler))
            ;
    }
}

static void ql_boot_lcd_invalidate_rect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
    HAL_GOUDA_WINDOW_T winRect;
    winRect.tlPX = left;
    winRect.tlPY = top;
    winRect.brPX = right;
    winRect.brPY = bottom;

    if (ql_boot_lcd_info == NULL)
    {
        OSI_LOGI(0, "boot lcd: lcd hardware has error!");
        return;
    }
    drvLcdConfigGoudaLayers();
    
    ql_boot_lcd_gouda_update(&winRect, true);
    
    OSI_LOGI(0, "boot lcd: InvalidateRect [OUT]");
}


static bool ql_boot_lcd_block_transfer(const lcdFrameBuffer_t *dataBufferWin, const lcdDisplay_t *lcdRec)
{
    lcdBlockCfg_t cfg;
    uint16_t left = 0; //left of the rectangle
    uint16_t top = 0;  //top of the rectangle
    uint16_t right;    //right of the rectangle
    uint16_t bottom;   //bottom of the rectangle.
    uint16_t layer = 0;
    if (ql_boot_lcd_info == NULL)
    {
        OSI_LOGI(0, "boot lcd: lcd hardware has error!");
        return false;
    }

    if (dataBufferWin->colorFormat != LCD_RESOLUTION_RGB565)
    {
        layer = 0;
        drvLcdSetImageLayerSize(dataBufferWin->width, dataBufferWin->height, dataBufferWin->widthOriginal);
    }
    else
    {
        layer = 3;
    }

    cfg.colorkey = 0;
    cfg.resolution = dataBufferWin->colorFormat;
    cfg.type = 1;
    cfg.start_x = dataBufferWin->region_x;
    cfg.start_y = dataBufferWin->region_y;
    cfg.end_x = cfg.start_x + dataBufferWin->width - 1;
    cfg.end_y = cfg.start_y + dataBufferWin->height - 1;
    cfg.colorkey_en = dataBufferWin->keyMaskEnable;
    cfg.colorkey = dataBufferWin->maskColor;
    cfg.alpha = 0xff;

    cfg.rotation = dataBufferWin->rotation;
    OSI_LOGI(0, "boot lcd: drvLcdBlockTransfer startx %d", cfg.start_x);

    if (false == drvLcdConfigBlock(layer, cfg))
    {
        return false;
    }
    
    drvLcdSetBlockBuffer(layer, (void *)dataBufferWin->buffer);
    drvLcdEnableBlock(layer);

    left = lcdRec->x;
    top = lcdRec->y;
    right = left + lcdRec->width - 1;
    bottom = top + lcdRec->height - 1;

    if (false == drvLcdSetDisplayWindow(left, top, right, bottom))
    {
        return false;
    }

    ql_boot_lcd_invalidate_rect(0, 0, lcdRec->width - 1, lcdRec->height - 1);

    return true;
}

static bool ql_boot_pmu_switch_power(uint32_t id, bool enabled, bool lp_enabled)
{
    REG_RDA2720M_GLOBAL_LDO_LCD_REG0_T ldo_lcd_reg0;
    REG_RDA2720M_GLOBAL_KPLED_CTRL0_T kpled_ctrl0;
    REG_RDA2720M_GLOBAL_SLP_LDO_PD_CTRL0_T slp_ldo_pd_ctrl0;
    REG_RDA2720M_BLTC_BLTC_CTRL_T bltc_ctrl;
    REG_RDA2720M_BLTC_BLTC_PD_CTRL_T bltc_pd_ctrl;
    REG_RDA2720M_GLOBAL_MODULE_EN0_T module_en0;
    REG_RDA2720M_BLTC_RG_RGB_V0_T rg_rgb_v0;
    REG_RDA2720M_BLTC_RG_RGB_V1_T rg_rgb_v1;
    REG_RDA2720M_BLTC_RG_RGB_V2_T rg_rgb_v2;
    REG_RDA2720M_BLTC_RG_RGB_V3_T rg_rgb_v3;

    switch (id)
    {
    case HAL_POWER_VDD28:
        // shouldn't power down VDD28, in POWER_PD_SW
        REG_ADI_CHANGE1(hwp_rda2720mGlobal->slp_ldo_pd_ctrl0, slp_ldo_pd_ctrl0,
                        slp_ldovdd28_pd_en, lp_enabled ? 0 : 1);
        break;

    case HAL_POWER_LCD:
        REG_ADI_CHANGE1(hwp_rda2720mGlobal->ldo_lcd_reg0, ldo_lcd_reg0,
                        ldo_lcd_pd, enabled ? 0 : 1);
        REG_ADI_CHANGE1(hwp_rda2720mGlobal->slp_ldo_pd_ctrl0, slp_ldo_pd_ctrl0,
                        slp_ldolcd_pd_en, lp_enabled ? 0 : 1);
        break;

    case HAL_POWER_KEYLED:
        REG_ADI_CHANGE2(hwp_rda2720mBltc->bltc_ctrl, bltc_ctrl,
                        b_sel, 1, b_sw, 1);
        REG_ADI_CHANGE1(hwp_rda2720mGlobal->kpled_ctrl0, kpled_ctrl0,
                        kpled_v, 0xb);

        REG_ADI_CHANGE1(hwp_rda2720mGlobal->kpled_ctrl0, kpled_ctrl0,
                        slp_ldokpled_pd_en, lp_enabled ? 0 : 1);

        REG_RDA2720M_GLOBAL_KPLED_CTRL1_T kpled_ctrl1 = {
            .b.ldo_kpled_pd = 1,
            .b.ldo_kpled_reftrim = 16,
        };
        halAdiBusWrite(&hwp_rda2720mGlobal->kpled_ctrl1, kpled_ctrl1.v);

        REG_ADI_CHANGE1(hwp_rda2720mGlobal->kpled_ctrl1, kpled_ctrl1,
                        ldo_kpled_pd, enabled ? 0 : 1);
        break;

    case HAL_POWER_BACK_LIGHT:
        REG_ADI_CHANGE1(hwp_rda2720mGlobal->module_en0, module_en0, bltc_en, 1);

        REG_ADI_CHANGE4(hwp_rda2720mBltc->bltc_ctrl, bltc_ctrl,
                        g_sel, enabled ? 1 : 0, g_sw, enabled ? 1 : 0,
                        r_sel, enabled ? 1 : 0, r_sw, enabled ? 1 : 0);

        REG_ADI_CHANGE4(hwp_rda2720mBltc->bltc_ctrl, bltc_ctrl,
                        wled_sel, enabled ? 1 : 0, wled_sw, enabled ? 1 : 0,
                        b_sel, enabled ? 1 : 0, b_sw, enabled ? 1 : 0);

        REG_ADI_CHANGE2(hwp_rda2720mBltc->bltc_pd_ctrl, bltc_pd_ctrl,
                        hw_pd, 0,
                        sw_pd, enabled ? 0 : 1);
        REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v0, rg_rgb_v0, rg_rgb_v0, 0x20);
        REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v1, rg_rgb_v1, rg_rgb_v1, 0x20);
        REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v2, rg_rgb_v2, rg_rgb_v2, 0x20);
        REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v3, rg_rgb_v3, rg_rgb_v3, 0x20);
        REG_ADI_CHANGE1(hwp_rda2720mGlobal->module_en0, module_en0, bltc_en, enabled ? 1 : 0);
        break;

    default:
        // ignore silently
        break;
    }

    return true;
}

static bool ql_boot_drv_lcd_init(void)
{
    uint32_t clk;
    
    ql_boot_lcd_info = drvLcdGetSpecInfo();
    if (NULL != ql_boot_lcd_info)
    {
        OSI_ASSERT(NULL != ql_boot_lcd_info->operation, "boot lcd init null"); /*assert verified*/
    }
    else
    {
        return false;
    }
    OSI_LOGI(0, "boot lcd:  get  width %d  height %d", ql_boot_lcd_info->width, ql_boot_lcd_info->height);


    ql_boot_lcd_cfg.brush_mode = QL_BOOT_LCD_BRUSH_MODE_SYNC;
    ql_boot_lcd_cfg.lcd_spec = ql_boot_lcd_info;
    ql_boot_lcd_cfg.sleep_state = false;

    if (ql_boot_lcd_info->freq == 0)
    {
        clk = 20000000;
    }
    clk = ql_boot_lcd_info->freq;
    
    ql_boot_gouda_init(ql_boot_gd_lcd_config, clk);

    drvLcdDisableAllBlock();

    if (NULL != ql_boot_lcd_info->operation)
    {
        ql_boot_lcd_info->operation->init();
    }
    
    ql_boot_lcd_param_put(ql_boot_lcd_info, (void*)&ql_boot_lcd_cfg);
    
    
    OSI_LOGI(0, "boot lcd:  drvLcdInit end");
    return true;
}

ql_boot_errcode_lcd ql_boot_lcd_clear_screen(uint16_t color)
{
    uint16_t height, width;

    for( height = 0; height < ql_boot_lcd_info->height; height++ )
    {
        for( width = 0; width < ql_boot_lcd_info->width; width++ )
        {
            *(ql_boot_lcd_buffer + height * ql_boot_lcd_info->width + width) = color;
        }
    }

    lcdFrameBuffer_t fb = {};
    fb.buffer        = ql_boot_lcd_buffer;
    fb.colorFormat   = LCD_RESOLUTION_RGB565;
    fb.height        = ql_boot_lcd_info->height;
    fb.width         = ql_boot_lcd_info->width;
    fb.widthOriginal = ql_boot_lcd_info->width;

    lcdDisplay_t rec = {};
    rec.width  = ql_boot_lcd_info->width;
    rec.height = ql_boot_lcd_info->height;

    if( false == ql_boot_lcd_block_transfer(&fb, &rec) )
    {
        OSI_LOGI(0, "boot lcd transfer failed!");
        return QL_BOOT_LCD_BLOCK_SET_ERR;
    }

    return QL_BOOT_LCD_SUCCESS;
}

ql_boot_errcode_lcd ql_boot_lcd_write(uint16_t *buffer, uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y)
{
    if( (start_x > ql_boot_lcd_info->width)
     || (end_x > ql_boot_lcd_info->width)
     || (start_y > ql_boot_lcd_info->height)
     || (end_y > ql_boot_lcd_info->height) )
    {
        return QL_BOOT_LCD_INVALID_PARAM_ERR;
    }

    uint16_t height, width;

    for( height = start_y; height < (end_y + 1); height++ )
    {
        for( width = start_x; width < (end_x + 1); width++ )
        {
            *(ql_boot_lcd_buffer + height * ql_boot_lcd_info->width + width) = *buffer;
            buffer++;
        }
    }

    lcdFrameBuffer_t fb = {};
    fb.buffer        = ql_boot_lcd_buffer + start_y * ql_boot_lcd_info->width + start_x;
    fb.colorFormat   = LCD_RESOLUTION_RGB565;
    fb.height        = ql_boot_lcd_info->height;
    fb.width         = ql_boot_lcd_info->width;
    fb.widthOriginal = ql_boot_lcd_info->width;

    lcdDisplay_t rec = {};
    rec.x      = start_x;
    rec.y      = start_y;
    rec.width  = end_x - start_x + 1;
    rec.height = end_y - start_y + 1;

    if( false == ql_boot_lcd_block_transfer(&fb, &rec) )
    {
        OSI_LOGI(0, "boot lcd transfer failed!");
        return QL_BOOT_LCD_BLOCK_SET_ERR;
    }

    return QL_BOOT_LCD_SUCCESS;
}

ql_boot_errcode_lcd ql_boot_lcd_display_on(void)
{
    if( NULL == ql_boot_lcd_info->operation )
    {
        OSI_LOGI(0, "boot lcd operation error!");
        return QL_BOOT_LCD_OPERATION_ERR;
    }
    ql_boot_lcd_info->operation->enterSleep(false);

    return QL_BOOT_LCD_SUCCESS;
}

ql_boot_errcode_lcd ql_boot_lcd_display_off(void)
{
    if( NULL == ql_boot_lcd_info->operation )
    {
        OSI_LOGI(0, "boot lcd operation error!");
        return QL_BOOT_LCD_OPERATION_ERR;
    }
    ql_boot_lcd_info->operation->enterSleep(true);

    return QL_BOOT_LCD_SUCCESS;
}

ql_boot_errcode_lcd ql_boot_lcd_display_close(void)
{
    if( NULL == ql_boot_lcd_info->operation )
    {
        OSI_LOGI(0, "boot lcd operation error!");
        return QL_BOOT_LCD_OPERATION_ERR;
    }
    
    ql_boot_lcd_info->operation->close();
    halGoudaClose();
    ql_boot_pmu_switch_power(HAL_POWER_LCD, false, false);
    //free(ql_boot_lcd_info);
    free(ql_boot_lcd_buffer);
    OSI_LOGI(0, "boot lcd close ok");
    
    return QL_BOOT_LCD_SUCCESS;
}


ql_boot_errcode_lcd ql_boot_spi_lcd_write_cmd(uint8_t cmd)
{
    if( cmd > 0xff )
    {
        return QL_BOOT_LCD_INVALID_PARAM_ERR;
    }

    while (halGoudaWriteCmd(cmd) != HAL_ERR_NO)
    {/* wait resource is not busy */
        ;
    }

    OSI_LOGI(0, "boot lcd cmd: 0x%x", cmd);
    return QL_BOOT_LCD_SUCCESS;
}

ql_boot_errcode_lcd ql_boot_spi_lcd_write_data(uint8_t data)
{
    if( data > 0xff )
    {
        return QL_BOOT_LCD_INVALID_PARAM_ERR;
    }

    //wait resource is not busy
    while (halGoudaWriteData(data) != HAL_ERR_NO)
    {
        ;
    }

    OSI_LOGI(0, "boot lcd data: 0x%x", data);
    return QL_BOOT_LCD_SUCCESS;
}

ql_boot_errcode_lcd ql_boot_lcd_set_brightness(uint32_t level)
{
    if( level > 5 )
    {
        return QL_BOOT_LCD_INVALID_PARAM_ERR;
    }

    if( level == 0 )
    {
        ql_boot_pmu_switch_power(HAL_POWER_BACK_LIGHT, false, false);
    }
    else
    {
        //bootPmuSwitchPower(HAL_POWER_BACK_LIGHT, true, false);

        uint32_t light_level = 0;

        uint32_t rg_rgb_v0_current_level = 0;
        uint32_t rg_rgb_v1_current_level = 0;
        uint32_t rg_rgb_v2_current_level = 0;
        uint32_t rg_rgb_v3_current_level = 0;

        REG_RDA2720M_BLTC_RG_RGB_V0_T rg_rgb_v0;
        REG_RDA2720M_BLTC_RG_RGB_V1_T rg_rgb_v1;
        REG_RDA2720M_BLTC_RG_RGB_V2_T rg_rgb_v2;
        REG_RDA2720M_BLTC_RG_RGB_V3_T rg_rgb_v3;

        switch (level)
        {
            case 1:
                light_level = 0;   //000000  1.68mA
                break;

            case 2:
                light_level = 7;   //000111  7.56mA
                break;

            case 3:
                light_level = 15;  //001111  14.28mA
                break;

            case 4:
                light_level = 31;  //011111  27.72mA
                break;

            case 5:
                light_level = 63;  //111111  54.6mA
                break;

            default:
                break;
        }

        rg_rgb_v0_current_level = light_level;
        rg_rgb_v1_current_level = light_level;
        rg_rgb_v2_current_level = light_level;
        rg_rgb_v3_current_level = light_level;

        REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v0, rg_rgb_v0, rg_rgb_v0, rg_rgb_v0_current_level);
        REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v1, rg_rgb_v1, rg_rgb_v1, rg_rgb_v1_current_level);
        REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v2, rg_rgb_v2, rg_rgb_v2, rg_rgb_v2_current_level);
        REG_ADI_CHANGE1(hwp_rda2720mBltc->rg_rgb_v3, rg_rgb_v3, rg_rgb_v3, rg_rgb_v3_current_level);
    }

    return QL_BOOT_LCD_SUCCESS;
}

ql_boot_errcode_lcd ql_boot_lcd_init(void)
{
    ql_boot_errcode_lcd err;
    
    ql_boot_pmu_switch_power(HAL_POWER_LCD, true, true);
    ql_boot_pmu_switch_power(HAL_POWER_VDD28, true, true);
    ql_boot_pmu_switch_power(HAL_POWER_KEYLED, true, true);

    if(ql_boot_drv_lcd_init() == false )
    {
        OSI_LOGI(0, "boot lcd init failed!");
        ql_boot_pmu_switch_power(HAL_POWER_LCD, false, false);
        return QL_BOOT_LCD_INIT_ERR;
    }
    
    OSI_LOGI(0, "boot lcd %u/%u", ql_boot_lcd_info->width, ql_boot_lcd_info->height);

    ql_boot_lcd_buffer = (uint16_t *)malloc(ql_boot_lcd_info->width * ql_boot_lcd_info->height * sizeof(uint16_t));
    if( NULL == ql_boot_lcd_buffer )
    {
        free(ql_boot_lcd_buffer);
        OSI_LOGI(0, "boot lcd buffer malloc failed!");
        ql_boot_pmu_switch_power(HAL_POWER_LCD, false, false);
        return QL_BOOT_LCD_BUFFER_CREATE_ERR;
    }

    err = ql_boot_lcd_clear_screen(0xffff);
    if( err == QL_BOOT_LCD_BLOCK_SET_ERR )
    {
        free(ql_boot_lcd_buffer);
        ql_boot_pmu_switch_power(HAL_POWER_LCD, false, false);
        return QL_BOOT_LCD_BLOCK_SET_ERR;
    }

    ql_boot_pmu_switch_power(HAL_POWER_BACK_LIGHT, true, true);
    
    return QL_BOOT_LCD_SUCCESS;

}


void ql_boot_lcd_display_demo()
{
    //image init
    uint16_t count = 0;
    uint16_t image1_len = sizeof(ql_boot_image1)/sizeof(ql_boot_image1[0]);
    uint16_t image2_len = sizeof(ql_boot_image2)/sizeof(ql_boot_image2[0]);

    for( count = 0; count < image1_len; count++ )
    {
        ql_boot_image1[count] = LCD_YELLOW;
    }
    for( count = 0; count < image2_len; count++ )
    {
        ql_boot_image2[count] = LCD_PURPLE;
    }
    
    //lcd init
    if( ql_boot_lcd_init() != QL_BOOT_LCD_SUCCESS )
    {
        OSI_LOGE(0,"boot lcd init failed");
        return;
    }

    ql_boot_lcd_display_on();
    OSI_LOGE(0,"boot lcd display on");
    osiDelayUS(1000 * 1000);
    
    ql_boot_lcd_write(ql_boot_image1, 50, 50, 50+30-1, 50+50-1);
    osiDelayUS(1000 * 2000);

    ql_boot_lcd_clear_screen(LCD_RED);
    OSI_LOGE(0,"boot lcd red on");
    osiDelayUS(1000 * 500);

    ql_boot_lcd_write(ql_boot_image2, 50, 100, 50+100-1, 100+100-1);
    osiDelayUS(1000 * 2000);

    ql_boot_lcd_clear_screen(LCD_GREEN);
    OSI_LOGE(0,"boot lcd green on");
    osiDelayUS(1000 * 500);

    //rotation 90 display
    ql_boot_spi_lcd_write_cmd(0x36);
    ql_boot_spi_lcd_write_data(0x60);
    ql_boot_lcd_write(ql_boot_image1, 50, 50, 50+30-1, 50+50-1);
    osiDelayUS(1000 * 500);
    
    // normal display
    ql_boot_spi_lcd_write_cmd(0x36);
    ql_boot_spi_lcd_write_data(0x00);
    ql_boot_lcd_clear_screen(LCD_BLUE);
    OSI_LOGE(0,"boot lcd blue on");
    osiDelayUS(1000 * 500);

    ql_boot_lcd_set_brightness(0);
    OSI_LOGE(0,"boot lcd backlight 0");
    
    ql_boot_lcd_display_close();
    OSI_LOGE(0,"boot lcd display close");

    return;
}


