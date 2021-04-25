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

#include "hal_chip.h"
#include "hwregs.h"
#include "osi_api.h"
#include "osi_profile.h"
#include "osi_byte_buf.h"
#include "drv_lcd.h"
#include "hal_gouda.h"
#include "osi_log.h"
#include "osi_mem.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "quec_proj_config.h"

#define LCDD_SPI_FREQ (50000000)
#define LCD_LOCK(lock) osiSemaphoreAcquire(lock)
#define LCD_UNLOCK(lock) osiSemaphoreRelease(lock)
#define BLOCK_SUM 4
static osiSemaphore_t *lcdLock = NULL;

static void _lcdDelayMs(int ms_delay)
{
    osiDelayUS(ms_delay * 1000);
}

typedef struct block_tag
{
    uint16_t is_enable; //1: enable this block;0: disable this block
    uint32_t *mem_ptr;
    uint32_t *alpha_addr;
    uint32_t *uv_addr;
    uint32_t *v_addr;
    lcdBlockCfg_t cfg;
} BLOCK_T;

typedef struct _lcd_rect_t_tag
{
    uint16_t left;   //LCD display window left position
    uint16_t top;    //LCD display window top position
    uint16_t right;  //LCD display window right position
    uint16_t bottom; //LCD display window bottom position
} LCD_RECT_T;

//Brush LCD mode
typedef enum
{
    LCD_BRUSH_MODE_SYNC = 0,
    LCD_BRUSH_MODE_ASYNC
} LCD_BRUSH_MODE_E;

static struct LAYER_SIZE
{
    uint32_t width;
    uint32_t height;
    uint32_t imageBufWidth; //the origin image buffer width
} g_vid_size = {0};

typedef struct
{
    LCD_BRUSH_MODE_E brush_mode; // 1 if brush lcd with synchronistic mean, or else 0
    uint32_t sleep_state;
    lcdSpec_t *lcd_spec;
} LCD_CONFIG_T;

typedef enum _lcd_layer_id_tag
{
    LCD_LAYER_IMAGE,
    LCD_LAYER_OSD1,
    LCD_LAYER_OSD2,
    LCD_LAYER_OSD3,
    LCD_LAYER_ID_MAX
} LCD_LAYER_ID_E;

static uint32_t s_lcd_used_num = 0;
static BLOCK_T g_block_info[BLOCK_SUM];
static lcdSpec_t *s_lcd_spec_info_ptr;

static LCD_CONFIG_T g_lcd_cfg;

#define LCDC_LAYER_NUM 4

#define IS_VIDEO_LAYER(iBlock) \
    (iBlock == LCD_LAYER_IMAGE && (g_block_info[iBlock].cfg.resolution == LCD_RESOLUTION_YUV420 || g_block_info[iBlock].cfg.resolution == LCD_RESOLUTION_YUV422_YUYV || g_block_info[iBlock].cfg.resolution == LCD_RESOLUTION_YUV420_3PLANE || g_block_info[iBlock].cfg.resolution == LCD_RESOLUTION_YUV422_UYVY))

static HAL_GOUDA_LCD_CONFIG_T gd_lcd_config =
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

void drvLcdDisableAllBlock(void);
void drvLcdEnableBlock(uint32_t blk_num);
bool drvLcdConfigBlock(uint32_t blk_num, lcdBlockCfg_t config);
void drvLcdSetBlockBuffer(uint32_t blk_num, uint32_t *buf_ptr);
void drvLcdInvalidataRectToRam(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint32_t *buffer);
void drvLcdInvalidateRect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
void drvDisableBlock(uint32_t blk_num);

extern lcdSpec_t *drvLcdGetSpecInfo(void);


#ifdef CONFIG_QUEC_PROJECT_FEATURE_BOOT_LCD
bool ql_boot_lcd_param_put(lcdSpec_t *boot_lcd_spec_info, void* boot_lcd_cfg)
{
    LCD_CONFIG_T *lcd_cfg;

    s_lcd_spec_info_ptr = boot_lcd_spec_info;
    lcd_cfg = (LCD_CONFIG_T*) boot_lcd_cfg;
    
    g_lcd_cfg.brush_mode  = lcd_cfg->brush_mode;
    g_lcd_cfg.lcd_spec    = lcd_cfg->lcd_spec;
    g_lcd_cfg.sleep_state = lcd_cfg->sleep_state;
    
    return true;
}

uint16_t ql_boot_lcd_block_info_enable(int block_index)
{  
    return g_block_info[block_index].is_enable;
}

#endif

static void _drvGetSemaphore(void)
{
    LCD_LOCK(lcdLock);
}

static void _lcdPutSemaphore(void)
{
    LCD_UNLOCK(lcdLock);
}

/*****************************************************************************/
//Description:Set video layer parameter
//
/*****************************************************************************/

bool drvLcdSetImageLayerSize(uint32_t width, uint32_t height, uint32_t imageBufWidth)
{
    g_vid_size.width = width;
    g_vid_size.height = height;
    g_vid_size.imageBufWidth = imageBufWidth;
    return true;
}

/*****************************************************************************/
//Description: Close the lcd.
//Global resource dependence:
/*****************************************************************************/
void drvLcdClose(void)
{
    uint16_t lcd_id;
    OSI_LOGI(0, "lcd:  :: drvLcdClose [IN]");
    if (s_lcd_spec_info_ptr == NULL)
    {
        OSI_LOGI(0, "lcd: lcd hardware has error!");
        return;
    }
    //close all lcd support
    for (lcd_id = 0; lcd_id < s_lcd_used_num; lcd_id++)
    {
        if (NULL != s_lcd_spec_info_ptr->operation)
        {
            OSI_LOGI(0, "lcd:  :: drvLcdClose [operation close]");

            s_lcd_spec_info_ptr->operation->close();
        }
    }
    halGoudaClose();
    halPmuSwitchPower(HAL_POWER_LCD, false, false);
    OSI_LOGI(0, "lcd:    drvLcdClose [OUT]");
}
/*****************************************************************************/
//Description:   get the lcd information.
//Global resource dependence:
/*****************************************************************************/
void drvLcdGetLcdInfo(lcdSpec_t *lcd_info_ptr)
{
    if (s_lcd_spec_info_ptr == NULL)
    {
        OSI_LOGI(0, "lcd: lcd hardware has error!");
        return;
    }
    lcd_info_ptr->dev_id = s_lcd_spec_info_ptr->dev_id;
    lcd_info_ptr->width = s_lcd_spec_info_ptr->width;
    lcd_info_ptr->height = s_lcd_spec_info_ptr->height;
    lcd_info_ptr->bus_mode = s_lcd_spec_info_ptr->bus_mode;
    lcd_info_ptr->controller = s_lcd_spec_info_ptr->controller;
    lcd_info_ptr->operation = s_lcd_spec_info_ptr->operation;
    lcd_info_ptr->is_use_fmark = s_lcd_spec_info_ptr->is_use_fmark;
    lcd_info_ptr->fmark_delay = s_lcd_spec_info_ptr->fmark_delay;
}

static void _drvLcdHandler(void)
{
    int i;

    halGoudaVidLayerClose();

    for (i = 1; i < LCDC_LAYER_NUM; i++)
    {
        if (g_block_info[i].is_enable)
        {
            halGoudaOvlLayerClose(i - 1);
            drvDisableBlock(i);
        }
    }

    drvLcdSetImageLayerSize(0, 0, 0);
    _lcdPutSemaphore();
}

static void _drvLcdGoudaUpdate(const HAL_GOUDA_WINDOW_T *pWin, bool bSync)
{
    HAL_GOUDA_LCD_CMD_T cmd[13];

    if (!((pWin->tlPX < pWin->brPX) && (pWin->tlPY < pWin->brPY)))
    {
        _drvLcdHandler();
        return;
    }
    if (bSync)
    {
        while (0 != halGoudaBlitRoi((HAL_GOUDA_WINDOW_T *)pWin, 0, cmd, NULL))
            ;
        while (halGoudaIsActive())
            ;
        _drvLcdHandler();
    }
    else
    {
        while (0 != halGoudaBlitRoi((HAL_GOUDA_WINDOW_T *)pWin, 0, cmd, _drvLcdHandler))
            ;
    }
}

static void _drvLcdGoudaUpdataRam(const HAL_GOUDA_WINDOW_T *pWin, const uint32_t *distBuffer, uint16_t width)
{
    if (!((pWin->tlPX < pWin->brPX) && (pWin->tlPY < pWin->brPY)))
    {
        _drvLcdHandler();
        return;
    }
    while (0 != halGoudaBlitRoi2Ram((uint32_t *)distBuffer, width, pWin, NULL))
        ;

    while (halGoudaIsActive())
        ;
    _drvLcdHandler();
}

#define LCD_RESOLUTION_RGB888 0
#define LCD_RESOLUTION_RGB666 1
#define LCD_RESOLUTION_RGB565 2
#define LCD_RESOLUTION_RGB555 3
#define LCD_RESOLUTION_GREY 4
#define LCD_RESOLUTION_YUV422_UYVY 5
#define LCD_RESOLUTION_YUV420 6
#define LCD_RESOLUTION_YUV400 7
#define LCD_RESOLUTION_YUV420_3PLANE 8
#define LCD_RESOLUTION_YUV422_YUYV 9

static HAL_GOUDA_IMG_FORMAT_T _drvLcdLayerFormatConverter(uint8_t _resolution)
{
    switch (_resolution)
    {
    case LCD_RESOLUTION_RGB888:
        return HAL_GOUDA_IMG_FORMAT_RGBA;
    case LCD_RESOLUTION_YUV420:
        return HAL_GOUDA_IMG_FORMAT_IYUV;
    case LCD_RESOLUTION_YUV420_3PLANE:
        return HAL_GOUDA_IMG_FORMAT_IYUV;
    case LCD_RESOLUTION_YUV422_UYVY:
        return HAL_GOUDA_IMG_FORMAT_UYVY;
    case LCD_RESOLUTION_YUV422_YUYV:
        return HAL_GOUDA_IMG_FORMAT_YUYV;
    case LCD_RESOLUTION_RGB565:
    default:
        return HAL_GOUDA_IMG_FORMAT_RGB565;
    }
}


#ifdef CONFIG_QUEC_PROJECT_FEATURE_BOOT_LCD
void drvLcdConfigGoudaLayers()
#else
static void drvLcdConfigGoudaLayers()
#endif
{
    int32_t i = 0;
    uint8_t bpp = 1;
    HAL_GOUDA_OVL_LAYER_ID_T over_layer_use_id = HAL_GOUDA_OVL_LAYER_ID0;

    for (i = 0; i < LCDC_LAYER_NUM; i++)
    {
        if (g_block_info[i].is_enable)
        {
            //    OSI_LOGI(0,"lcd:  :: configure gouda Layer [%d]   = (enable)",  i);

            if (i == 0)
            {
                HAL_GOUDA_VID_LAYER_DEF_T video_layer_cfg;

                video_layer_cfg.fmt = _drvLcdLayerFormatConverter(g_block_info[i].cfg.resolution);
                video_layer_cfg.addrY = (uint32_t *)g_block_info[i].mem_ptr;
                video_layer_cfg.addrU = (uint32_t *)g_block_info[i].uv_addr;

                if (g_block_info[i].cfg.resolution == LCD_RESOLUTION_YUV420_3PLANE)
                    video_layer_cfg.addrV = (uint32_t *)g_block_info[i].v_addr;
                else if ((g_block_info[i].cfg.resolution == LCD_RESOLUTION_YUV422_UYVY) || (g_block_info[i].cfg.resolution == LCD_RESOLUTION_YUV422_YUYV))
                    video_layer_cfg.addrV = video_layer_cfg.addrU + (g_vid_size.width * g_vid_size.height) / 8;
                else
                    video_layer_cfg.addrV = video_layer_cfg.addrU + (g_vid_size.width * g_vid_size.height) / 16;

                video_layer_cfg.alpha = 0xFF;
                video_layer_cfg.cKeyColor = 0;
                video_layer_cfg.cKeyEn = false;
                video_layer_cfg.cKeyMask = HAL_GOUDA_CKEY_MASK_OFF;
                video_layer_cfg.depth = HAL_GOUDA_VID_LAYER_BEHIND_ALL;
                //video_layer_cfg.depth = HAL_GOUDA_VID_LAYER_OVER_ALL;
                //video_layer_cfg.stride = g_block_info[i].cfg.width;

                bpp = (video_layer_cfg.fmt == HAL_GOUDA_IMG_FORMAT_IYUV) ? 1 : 2;
                // Note that stride could be different in case of padding
                video_layer_cfg.stride = g_vid_size.imageBufWidth * bpp;
                video_layer_cfg.pos.tlPX = g_block_info[i].cfg.start_x;
                video_layer_cfg.pos.tlPY = g_block_info[i].cfg.start_y;
                video_layer_cfg.pos.brPX = g_block_info[i].cfg.end_x;
                video_layer_cfg.pos.brPY = g_block_info[i].cfg.end_y;

                video_layer_cfg.width = g_vid_size.width;
                video_layer_cfg.height = g_vid_size.height;
                video_layer_cfg.rotation = g_block_info[i].cfg.rotation; //0: 0; 1: 90; clockwise

                halGoudaVidLayerClose();
                halGoudaVidLayerOpen(&video_layer_cfg);
                //  OSI_LOGI(0,"lcd:  :: Open Video Layer [0] addr = (0x%x) -- config layer = %d",  video_layer_cfg.addrY, i);
            }
            else
            {
                HAL_GOUDA_OVL_LAYER_DEF_T over_layer_cfg;

                over_layer_cfg.addr = (uint32_t *)g_block_info[i].mem_ptr;
                over_layer_cfg.fmt = _drvLcdLayerFormatConverter(g_block_info[i].cfg.resolution);
                over_layer_cfg.stride = 0;
                over_layer_cfg.pos.tlPX = g_block_info[i].cfg.start_x;
                over_layer_cfg.pos.tlPY = g_block_info[i].cfg.start_y;
                over_layer_cfg.pos.brPX = g_block_info[i].cfg.end_x;
                over_layer_cfg.pos.brPY = g_block_info[i].cfg.end_y;
                over_layer_cfg.alpha = g_block_info[i].cfg.alpha;
                over_layer_cfg.cKeyColor = g_block_info[i].cfg.colorkey;
                over_layer_cfg.cKeyEn = g_block_info[i].cfg.colorkey_en;

                // open the layer
                halGoudaOvlLayerClose(i - 1);

                halGoudaOvlLayerOpen(i - 1, &over_layer_cfg);
                OSI_LOGI(0, "lcd:Over Layer [%d] sx=%d", i - 1, over_layer_cfg.pos.tlPX);
                over_layer_use_id++;
            }
        }
        else
        {
            //   OSI_LOGI(0,"lcd:  :: configure gouda Layer [%d]   = (disable)",  i);
        }
    }
}

/*****************************************************************************/
//Description:   entern/exit sleep mode
//Global resource dependence:
/*****************************************************************************/
void drvLcdEnterSleep(bool is_sleep)
{
    if (NULL != s_lcd_spec_info_ptr->operation)
    {
        if (g_lcd_cfg.sleep_state != (uint32_t)is_sleep)
        {

            g_lcd_cfg.sleep_state = (uint32_t)is_sleep;
            if (is_sleep == false)
            {
                osiThreadSleep(150);
                halPmuSwitchPower(HAL_POWER_LCD, true, true);
                halGoudaClkEnable();
                halGoudaInit(gd_lcd_config, LCDD_SPI_FREQ);

                if (NULL != s_lcd_spec_info_ptr->operation)
                {
                    s_lcd_spec_info_ptr->operation->init();
                }

                OSI_LOGI(0, "lcd gouda init before turn on backlight");
            }
            s_lcd_spec_info_ptr->operation->enterSleep(is_sleep);
            if (is_sleep == true)
            {
                halGoudaClose();
                osiThreadSleep(150);
                halGoudaClkDisable();
                halPmuSwitchPower(HAL_POWER_LCD, false, false);
                OSI_LOGI(0, "lcd gouda init before turn off backlight");
            }
        }
    }
}

bool drvLcdSetDisplayWindow(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
    if (NULL != s_lcd_spec_info_ptr->operation)
    {

        s_lcd_spec_info_ptr->operation->setDisplayWindow(left, top, right, bottom);
    }
    else
    {
        return false;
    }
    return true;
}

void drvLcdCheckESD(void)
{
    if (NULL != g_lcd_cfg.lcd_spec->operation->checkEsd)
    {

        g_lcd_cfg.lcd_spec->operation->checkEsd();
    }
}
/*****************************************************************************/
//Description: set display memory
//Global resource dependence:
/*****************************************************************************/
void drvLcdSetBlockBuffer(uint32_t blk_num, uint32_t *buf_ptr)
{
    OSI_ASSERT(blk_num < BLOCK_SUM, "drvLcdSetBlockBuffer number error"); /*assert verified*/
    OSI_ASSERT(buf_ptr != NULL, "drvLcdSetBlockBuffer null");             /*assert verified*/

    g_block_info[blk_num].mem_ptr = buf_ptr;
}

bool drvSetBrushMode(LCD_BRUSH_MODE_E mode)
{

    if (mode == g_lcd_cfg.brush_mode)
    {

        return true;
    }
    g_lcd_cfg.brush_mode = mode;

    return true;
}

bool drvSetUVBufferAddress(
    uint32_t blk_num,
    uint32_t *buf_ptr)
{
    OSI_ASSERT(blk_num == 0, "lcd:  drvSetUVBufferAddress blk");
    g_block_info[blk_num].uv_addr = buf_ptr;
    return true;
}

bool drvSetVBufferAddress(
    uint32_t blk_num,
    uint32_t *buf_ptr)
{
    OSI_ASSERT(blk_num == 0, "lcd:  drvSetVBufferAddress");
    g_block_info[blk_num].v_addr = buf_ptr;
    return true;
}

/*
src fmt :: 
    #define  LCD_RESOLUTION_RGB888          0
    #define  LCD_RESOLUTION_RGB666          1
    #define  LCD_RESOLUTION_RGB565          2
    #define  LCD_RESOLUTION_RGB555          3
    #define  LCD_RESOLUTION_GREY            4
    #define  LCD_RESOLUTION_YUV422          5
    #define  LCD_RESOLUTION_YUV420          6
    #define  LCD_RESOLUTION_YUV400          7
    #define  LCD_RESOLUTION_YUV420_3PLANE          8
dst format :
    must be LCD_RESOLUTION_RGB565

rotation :
    //0: 0; 1: 90; 2: 180; 3: 270; 
note: mocor player_display_v1.c
*/
OSI_UNUSED void drvLcdPictrueConverter(uint8_t src_fmt, uint32_t *src_addr, uint32_t src_w, uint32_t src_h, uint32_t stride,
                                       uint32_t *dst_addr, uint32_t dst_w, uint32_t dst_h, uint8_t rotation)
{
    uint32_t *p_y = NULL, *p_uv = NULL;
    lcdBlockCfg_t vid_cfg = {0};

    OSI_LOGI(0, "lcd:  david record drvLcdPictrueConverter fmt:%d, srcaddr:0x%x, w/h(%d/%d), dstaddr:0x%x, dstw/h(%d/%d), r=%d",
             src_fmt, src_addr, src_w, src_h, dst_addr, dst_w, dst_h, rotation);

    p_y = (uint32_t *)src_addr;

    if (rotation == 1) // 90
        p_y += (src_h - 1) * (src_w * 2) / 4;
    else if (rotation == 2) // 180
        p_y += (src_w * 2 - 16) / 4;
    else if (rotation == 3) // 270
        p_y += (src_h - 1) * (src_w * 2) / 4;
    else if (rotation != 0)
        return; // error

    if (src_fmt == LCD_RESOLUTION_YUV420)
        p_uv = (uint32_t *)(p_y + (src_w * src_h) / 4);

    halGoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
    halGoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID1);
    halGoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID2);

    drvLcdSetBlockBuffer(0, p_y);
    drvSetUVBufferAddress(0, p_uv);

    vid_cfg.start_x = 0;
    vid_cfg.start_y = 0;
    vid_cfg.end_x = dst_w - 1;
    vid_cfg.end_y = dst_h - 1;
    vid_cfg.width = stride;
    vid_cfg.colorkey_en = 0;
    vid_cfg.alpha = 0xFF;
    vid_cfg.type = 1;
    vid_cfg.resolution = src_fmt;
    vid_cfg.rotation = rotation;

    drvLcdSetImageLayerSize(src_w, src_h, src_w);
    drvLcdConfigBlock(0, vid_cfg);

#if 0 // output to screen
    drvLcdGetLcdInfo(&lcd_info);
    drvLcdInvalidateRect(0, 0, lcd_info.lcd_width - 1, lcd_info.lcd_height - 1);
#else
    drvLcdInvalidataRectToRam(vid_cfg.start_x, vid_cfg.start_y, vid_cfg.end_x, vid_cfg.end_y, dst_addr);

#endif
}

void drvDisableBlock(uint32_t blk_num)
{
    OSI_ASSERT(blk_num < BLOCK_SUM, "drvLcdDisableBlock"); /*assert verified*/
    OSI_LOGI(0, "[lcd:   EnableBlock  : %d)", blk_num);

    g_block_info[blk_num].is_enable = false;
}

void drvLcdEnableBlock(uint32_t blk_num)
{
    OSI_ASSERT(blk_num < BLOCK_SUM, "drvLcdEnableBlock"); /*assert verified*/
    OSI_LOGI(0, "lcd:  EnableBlock  %d", blk_num);

    g_block_info[blk_num].is_enable = true;
}

uint32_t drvLcdGetFreeBlock(void)
{
    uint8_t i;

    for (i = 1; i < BLOCK_SUM; i++)
    {
        if (g_block_info[i].is_enable == false)
        {

            return i;
        }
    }

    OSI_ASSERT((i < BLOCK_SUM), ("drvLcdGetFreeBlock fail")); /*assert verified*/
    return 0;
}

void drvLcdDisableAllBlock(void)
{
    uint32_t i = 0;
    OSI_LOGI(0, "lcd:   DisableAllBlock ");

    for (i = 0; i < LCDC_LAYER_NUM; i++)
    {
        g_block_info[i].is_enable = false;
    }
}

/*****************************************************************************/
//Description: configure block parameters
//Global resource dependence:

/*****************************************************************************/
bool drvLcdConfigBlock(uint32_t blk_num, lcdBlockCfg_t config)
{
    OSI_ASSERT((blk_num < BLOCK_SUM), ("blk_num=%d", blk_num)); /*assert verified*/
    if ((config.start_x >= config.end_x) || (config.start_y >= config.end_y))
    {

        OSI_LOGI(0, "lcd: drvLcdConfigBlock error!!!");
        return false;
    }
    if (s_lcd_spec_info_ptr == NULL)
    {
        OSI_LOGI(0, "lcd: lcd hardware has error!");
        return false;
    }

    g_block_info[blk_num].cfg.alpha = config.alpha;
    g_block_info[blk_num].cfg.width = config.width;
    g_block_info[blk_num].cfg.type = config.type;
    g_block_info[blk_num].cfg.start_x = config.start_x;
    g_block_info[blk_num].cfg.start_y = config.start_y;
    g_block_info[blk_num].cfg.end_x = config.end_x;
    g_block_info[blk_num].cfg.end_y = config.end_y;
    g_block_info[blk_num].cfg.colorkey = config.colorkey;
    g_block_info[blk_num].cfg.resolution = config.resolution;
    g_block_info[blk_num].cfg.colorkey_en = config.colorkey_en;
    g_block_info[blk_num].cfg.rotation = config.rotation;

    OSI_LOGI(0, "lcd:  :: config block (%d) -- [%d, %d, %d, %d] -%d ", blk_num,
             config.start_x, config.start_y, config.end_x, config.end_y, config.resolution);

    return true;
}

/*****************************************************************************/
//Description:	 invalidate a rectang with the internal lcd data buffer
//Global resource dependence:
/*****************************************************************************/
void drvLcdInvalidateRect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
    HAL_GOUDA_WINDOW_T winRect;
    winRect.tlPX = left;
    winRect.tlPY = top;
    winRect.brPX = right;
    winRect.brPY = bottom;

    if (s_lcd_spec_info_ptr == NULL)
    {
        OSI_LOGI(0, "lcd: lcd hardware has error!");
        return;
    }
    drvLcdConfigGoudaLayers();
    _drvLcdGoudaUpdate(&winRect, true);
    OSI_LOGI(0, "lcd:  :: InvalidateRect [OUT]");
}

/*****************************************************************************/
//Description:	 converter lcd data to buffer
//Global resource dependence:
/*****************************************************************************/

void drvLcdInvalidataRectToRam(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint32_t *buffer)
{
    HAL_GOUDA_WINDOW_T winRect;

    if (s_lcd_spec_info_ptr == NULL)
    {
        OSI_LOGI(0, "lcd: lcd hardware has error!");
        return;
    }
    winRect.tlPX = left;
    winRect.tlPY = top;
    winRect.brPX = right;
    winRect.brPY = bottom;
    drvLcdConfigGoudaLayers();
    _drvLcdGoudaUpdataRam(&winRect, buffer, 0);

    OSI_LOGI(0, "lcd:  :: InvalidateRect [OUT]");
}

bool drvLcdBlockTransfer(const lcdFrameBuffer_t *dataBufferWin, const lcdDisplay_t *lcdRec)
{
    lcdBlockCfg_t cfg;
    uint16_t left = 0; //left of the rectangle
    uint16_t top = 0;  //top of the rectangle
    uint16_t right;    //right of the rectangle
    uint16_t bottom;   //bottom of the rectangle.
    uint16_t layer = 0;
    if (s_lcd_spec_info_ptr == NULL)
    {
        OSI_LOGI(0, "lcd: lcd hardware has error!");
        return false;
    }
    _drvGetSemaphore();
    if (dataBufferWin->colorFormat != LCD_RESOLUTION_RGB565)
    {
        layer = 0;

        if ((g_vid_size.width == 0) || (g_vid_size.height == 0))
        {
            drvLcdSetImageLayerSize(dataBufferWin->width, dataBufferWin->height, dataBufferWin->widthOriginal);
        }
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
    //cfg.width = s_lcd_spec_info_ptr->width;
    cfg.alpha = 0xff;

    cfg.rotation = dataBufferWin->rotation;
    OSI_LOGI(0, "lcd: drvLcdBlockTransfer startx %d", cfg.start_x);

    if (false == drvLcdConfigBlock(layer, cfg))
    {
        _lcdPutSemaphore();
        return false;
    }
    drvLcdSetBlockBuffer(layer, (void *)dataBufferWin->buffer);

    //gouda_event(0x9000c1e5);
    drvLcdEnableBlock(layer);

    left = lcdRec->x;
    top = lcdRec->y;
    right = left + lcdRec->width - 1;
    bottom = top + lcdRec->height - 1;

    if (false == drvLcdSetDisplayWindow(left, top, right, bottom))
    {
        _lcdPutSemaphore();
        return false;
    }

    drvLcdInvalidateRect(0, 0, lcdRec->width - 1, lcdRec->height - 1);

    //right = left + dataBufferWin->width - 1;
    //bottom = top + dataBufferWin->height - 1;

    return true;
}

bool drvLcdBlockSetOverLay(const lcdFrameBuffer_t *dataBufferWin)
{
    lcdBlockCfg_t cfg;
    uint16_t layer = 0;

    if (s_lcd_spec_info_ptr == NULL)
    {
        OSI_LOGI(0, "lcd: lcd hardware has error!");
        return false;
    }
    _drvGetSemaphore();
    if (dataBufferWin->colorFormat != LCD_RESOLUTION_RGB565)
    {
        layer = 0;
        if ((g_vid_size.width == 0) || (g_vid_size.height == 0))
        {
            drvLcdSetImageLayerSize(dataBufferWin->width, dataBufferWin->height, dataBufferWin->widthOriginal);
        }
    }
    else
    {
        layer = drvLcdGetFreeBlock();
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

    //cfg.width = s_lcd_spec_info_ptr->width;
    cfg.alpha = 0xff;

    cfg.rotation = dataBufferWin->rotation;

    if (false == drvLcdConfigBlock(layer, cfg))
    {
        _lcdPutSemaphore();
        return false;
    }

    drvLcdSetBlockBuffer(layer, (void *)dataBufferWin->buffer);

    //gouda_event(0x9000c1e5);
    drvLcdEnableBlock(layer);
    _lcdPutSemaphore();

    return true;
}

bool drvLcdInit(void)
{
    OSI_LOGI(0, "lcd:  drvLcdInit");
    uint32_t clk;
    if (lcdLock == NULL)
    {
        lcdLock = osiSemaphoreCreate(1, 1);
    }
    else
    {
        OSI_LOGI(0, "lcd:  drvLcd already Init");
        return true;
    }
    s_lcd_spec_info_ptr = drvLcdGetSpecInfo();
    if (NULL != s_lcd_spec_info_ptr)
    {
        OSI_ASSERT(NULL != s_lcd_spec_info_ptr->operation, "lcd init null"); /*assert verified*/
    }
    else
    {
        return false;
    }
    OSI_LOGI(0, "lcd:  get  w %d  h %d", s_lcd_spec_info_ptr->width, s_lcd_spec_info_ptr->height);
#ifndef CONFIG_QUEC_PROJECT_FEATURE_LCD
    halPmuSwitchPower(HAL_POWER_LCD, true, true);
#endif
    _lcdDelayMs(10);

    g_lcd_cfg.brush_mode = LCD_BRUSH_MODE_SYNC;

    g_lcd_cfg.lcd_spec = s_lcd_spec_info_ptr;

    g_lcd_cfg.sleep_state = false;

    if (s_lcd_spec_info_ptr->freq == 0)
    {
        clk = 20000000;
    }
    clk = s_lcd_spec_info_ptr->freq;
    halGoudaInit(gd_lcd_config, clk);

    drvLcdDisableAllBlock();

    if (NULL != s_lcd_spec_info_ptr->operation)
    {
        s_lcd_spec_info_ptr->operation->init();
    }
    OSI_LOGI(0, "lcd:  drvLcdInit end");
    return true;
}

/* end of lcd.c*/
