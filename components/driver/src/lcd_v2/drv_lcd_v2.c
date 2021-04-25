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

#include "drv_lcd_v2.h"
#include "drv_lcd_desc.h"
#include "drv_lcd_panel.h"
#include "drv_names.h"
#include "hal_chip.h"
#include "osi_api.h"
#include "osi_api_inside.h"
#include "osi_log.h"
#include <string.h>

#undef WAIT_GOUDA_BY_TIMER

#define LCD_BACK_LIGHT_POWER (HAL_POWER_BACK_LIGHT)
#define LCD_CS (GOUDA_DESTINATION_V_LCD_CS_0)
#define LCD_CS0_POLARITY (0)
#define LCD_CS1_POLARITY (0)
#define LCD_LOW_FREQ (800000)

#define LCD_STATE_CLOSED (0)
#define LCD_STATE_PROBE_FAILD (1)
#define LCD_STATE_OPENED (2)
#define LCD_STATE_SLEEP (3)

#define GOUDA_STATE_UNINIT (0)
#define GOUDA_STATE_OPEN (1)
#define GOUDA_STATE_CLOSE (2)

struct drvLcd
{
    uint8_t state;                           // state
    drvLcdDirection_t app_dir;               // application direction
    const drvLcdPanelDesc_t *desc;           // panel description
    REG_GOUDA_GD_LCD_CTRL_T lcd_ctrl;        // GOUDA lcd_ctrl
    REG_GOUDA_GD_SPILCD_CONFIG_T spi_config; // GOUDA spi_config
    unsigned mem_address;                    // GOUDA mem_address
};

typedef struct
{
    uint8_t state;            // state
    osiSemaphore_t *sema;     // semaphore for finish
    osiMutex_t *lock;         // API lock
    osiPmSource_t *pm_source; // PM source
    osiClockConstrainRegistry_t clk_constrain;
} drvLcdGoudaContext_t;

static drvLcd_t gLcd1;
static drvLcdGoudaContext_t gGoudaCtx;

static void prvSetPowerEnable(bool enabled)
{
    halPmuSwitchPower(HAL_POWER_LCD, enabled, false);
    if (enabled)
        osiDelayUS(200);
}

static void prvWaitGouda(void)
{
#ifdef WAIT_GOUDA_BY_TIMER
    while ((hwp_gouda->gd_status & (/*GOUDA_IA_BUSY | */ GOUDA_LCD_BUSY)) != 0)
        osiThreadSleepUS(1000);
#else
    uint32_t critical = osiEnterCritical();
    for (;;)
    {
        if ((hwp_gouda->gd_status & (GOUDA_IA_BUSY | GOUDA_LCD_BUSY)) == 0)
            break;

        osiSemaphoreAcquire(gGoudaCtx.sema);
    }
    osiExitCritical(critical);
#endif
}

static void prvGoudaISR(void *param)
{
    hwp_gouda->gd_eof_irq = GOUDA_EOF_CAUSE; // clear interrupt
    (void)hwp_gouda->gd_eof_irq;

    // interrupt is diabled by default
    hwp_gouda->gd_eof_irq_mask = 0;

    osiSemaphoreRelease(gGoudaCtx.sema);
    osiReleaseClk(&gGoudaCtx.clk_constrain);
}

static void prvGoudaInit(void)
{
    // disable auto gating
    REG_SYS_CTRL_CLK_SYS_AHB_MODE_T clk_sys_ahb_mode = {hwp_sysCtrl->clk_sys_ahb_mode};
    clk_sys_ahb_mode.b.mode_sys_ahb_clk_id_gouda = 1; // manual
    hwp_sysCtrl->clk_sys_ahb_mode = clk_sys_ahb_mode.v;

    REG_SYS_CTRL_CLK_AP_APB_MODE_T clk_ap_apb_mode = {hwp_sysCtrl->clk_ap_apb_mode};
    clk_ap_apb_mode.b.mode_ap_apb_clk_id_gouda = 1; // manual
    hwp_sysCtrl->clk_ap_apb_mode = clk_ap_apb_mode.v;

    osiIrqSetPriority(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GOUDA), SYS_IRQ_PRIO_GOUDA);
    osiIrqSetHandler(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GOUDA), prvGoudaISR, NULL);
    osiIrqEnable(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GOUDA));

    gGoudaCtx.state = GOUDA_STATE_OPEN;
    osiPmWakeLock(gGoudaCtx.pm_source);
}

static void prvGoudaDeinit(void)
{
    // enable auto gating
    REG_SYS_CTRL_CLK_SYS_AHB_MODE_T clk_sys_ahb_mode = {hwp_sysCtrl->clk_sys_ahb_mode};
    clk_sys_ahb_mode.b.mode_sys_ahb_clk_id_gouda = 0; // auto
    hwp_sysCtrl->clk_sys_ahb_mode = clk_sys_ahb_mode.v;

    REG_SYS_CTRL_CLK_AP_APB_MODE_T clk_ap_apb_mode = {hwp_sysCtrl->clk_ap_apb_mode};
    clk_ap_apb_mode.b.mode_ap_apb_clk_id_gouda = 0; // auto
    hwp_sysCtrl->clk_ap_apb_mode = clk_ap_apb_mode.v;

    hwp_gouda->gd_spilcd_config = 0;
    hwp_gouda->gd_lcd_ctrl = 0;

    osiIrqDisable(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GOUDA));
    gGoudaCtx.state = GOUDA_STATE_CLOSE;
    osiPmWakeUnlock(gGoudaCtx.pm_source);
}

static void prvPanelConfig(drvLcd_t *d)
{
    d->lcd_ctrl.b.lcd_resetb = 0;
    hwp_gouda->gd_lcd_ctrl = d->lcd_ctrl.v;

    osiThreadSleepUS(d->desc->reset_us);

    d->lcd_ctrl.b.lcd_resetb = 1;
    hwp_gouda->gd_lcd_ctrl = d->lcd_ctrl.v;

    hwp_gouda->gd_spilcd_config = d->spi_config.v;
    hwp_gouda->gd_lcd_mem_address = d->mem_address;

    hwp_gouda->tecon = 0;
    if (d->desc->fmark_enabled)
    {
        hwp_gouda->tecon = GOUDA_TE_EN | GOUDA_TE_EDGE_SEL;
        hwp_gouda->tecon2 = GOUDA_TE_COUNT1(d->desc->fmark_delay);
    }

    osiThreadSleepUS(d->desc->init_delay_us);
}

void drvLcdInitV2(void)
{
    if (gGoudaCtx.state != GOUDA_STATE_UNINIT)
        return;

    gGoudaCtx.state = GOUDA_STATE_CLOSE;
    gGoudaCtx.sema = osiSemaphoreCreate(1, 1);
    gGoudaCtx.lock = osiMutexCreate();
    gGoudaCtx.pm_source = osiPmSourceCreate(DRV_NAME_LCD1, NULL, NULL);
    gGoudaCtx.clk_constrain.tag = DRV_NAME_GOUDA;
    gLcd1.state = LCD_STATE_CLOSED;
}

drvLcd_t *drvLcdGetByname(unsigned name)
{
    if (name == DRV_NAME_LCD1)
        return &gLcd1;

    return NULL;
}

bool drvLcdGetPanelInfo(drvLcd_t *d, drvLcdPanelInfo_t *info)
{
    if (d == NULL || (d->state != LCD_STATE_OPENED && d->state != LCD_STATE_SLEEP))
        return false;

    unsigned spi_freq = CONFIG_DEFAULT_SYSAHB_FREQ / d->spi_config.b.spi_clk_divider;
    unsigned spi_bps = (d->desc->line_mode == DRV_LCD_SPI_3WIRE_2LANE) ? (spi_freq * 2) : spi_freq;
    info->name = d->desc->name;
    info->dev_id = d->desc->dev_id;
    info->width = d->desc->width;
    info->height = d->desc->height;
    info->intf = DRV_LCD_INTF_SPI;
    info->fmark_enabled = d->desc->fmark_enabled;
    info->speed = spi_bps / sizeof(uint16_t);
    info->frame_us = d->desc->frame_us;
    if (drvLcdDirSwapped(drvLcdDirCombine(d->app_dir, d->desc->dir)))
        OSI_SWAP(uint16_t, info->width, info->height);

    return true;
}

void drvLcdSetDesc(drvLcd_t *d, const drvLcdPanelDesc_t *desc)
{
    d->desc = desc;

    d->lcd_ctrl.v = 0;
    d->lcd_ctrl.b.destination = LCD_CS;
    d->lcd_ctrl.b.output_format = desc->out_fmt;
    d->lcd_ctrl.b.high_byte = false;
    d->lcd_ctrl.b.cs0_polarity = LCD_CS0_POLARITY;
    d->lcd_ctrl.b.cs1_polarity = LCD_CS1_POLARITY;
    d->lcd_ctrl.b.rs_polarity = 0;
    d->lcd_ctrl.b.wr_polarity = 0;
    d->lcd_ctrl.b.rd_polarity = 0;
    d->lcd_ctrl.b.lcd_resetb = 0; // release reset

    d->spi_config.v = 0;
    d->spi_config.b.spi_lcd_select = 1;
    d->spi_config.b.spi_clk_divider = OSI_DIV_ROUND_UP(CONFIG_DEFAULT_SYSAHB_FREQ, desc->freq);
    d->spi_config.b.spi_dummy_cycle = 0;
    d->spi_config.b.spi_line = desc->line_mode;
    d->spi_config.b.spi_rx_byte = 0;
    d->spi_config.b.spi_rw = 0;

    prvPanelConfig(d);
}

bool drvLcdOpenV2(drvLcd_t *d)
{
    if (d == NULL)
        return false;

    osiMutexLock(gGoudaCtx.lock);

    if (d->state == LCD_STATE_OPENED)
        goto pass_unlock;

    if (d->state == LCD_STATE_PROBE_FAILD)
        goto fail_unlock;

    prvGoudaInit();
    prvSetPowerEnable(true);

    d->state = LCD_STATE_PROBE_FAILD;
    for (unsigned n = 0; n < OSI_ARRAY_SIZE(gLcdDescs); n++)
    {
        drvLcdSetDesc(d, gLcdDescs[n]);
        if (d->desc->ops.probe(d))
        {
            d->desc->ops.init(d);
            d->state = LCD_STATE_OPENED;
            break;
        }
    }

    if (d->state != LCD_STATE_OPENED)
    {
        prvGoudaDeinit();
        prvSetPowerEnable(false);
        goto fail_unlock;
    }

pass_unlock:
    OSI_LOGI(0, "lcd open");
    osiMutexUnlock(gGoudaCtx.lock);
    return true;

fail_unlock:
    OSI_LOGI(0, "lcd open failed");
    osiMutexUnlock(gGoudaCtx.lock);
    return false;
}

void drvLcdCloseV2(drvLcd_t *d)
{
    if (d == NULL)
        return;

    osiMutexLock(gGoudaCtx.lock);
    OSI_LOGI(0, "lcd close");

    if (d->state == LCD_STATE_OPENED || d->state == LCD_STATE_SLEEP)
    {
        prvGoudaDeinit();
        prvSetPowerEnable(false);
        memset(d, 0, sizeof(drvLcd_t));
    }

    osiMutexUnlock(gGoudaCtx.lock);
}

bool drvLcdSleep(drvLcd_t *d)
{
    if (d == NULL)
        return false;

    osiMutexLock(gGoudaCtx.lock);
    OSI_LOGI(0, "lcd sleep");

    if (d->state != LCD_STATE_OPENED)
        goto fail_unlock;

    prvWaitGouda();
    prvGoudaDeinit();
    prvSetPowerEnable(false);
    d->state = LCD_STATE_SLEEP;

    osiMutexUnlock(gGoudaCtx.lock);
    return true;

fail_unlock:
    osiMutexUnlock(gGoudaCtx.lock);
    return false;
}

bool drvLcdWakeup(drvLcd_t *d)
{
    if (d == NULL)
        return false;

    osiMutexLock(gGoudaCtx.lock);
    OSI_LOGI(0, "lcd wakeup");

    if (d->state != LCD_STATE_SLEEP)
        goto fail_unlock;

    prvSetPowerEnable(true);
    prvGoudaInit();
    prvPanelConfig(d);
    d->desc->ops.init(d);
    d->state = LCD_STATE_OPENED;

    osiMutexUnlock(gGoudaCtx.lock);
    return true;

fail_unlock:
    osiMutexUnlock(gGoudaCtx.lock);
    return false;
}

void drvLcdSetDirection(drvLcd_t *d, drvLcdDirection_t dir)
{
    if (d == NULL)
        return;

    d->app_dir = dir;
}

void drvLcdSetBackLightEnable(drvLcd_t *d, bool enabled)
{
    halPmuSwitchPower(LCD_BACK_LIGHT_POWER, enabled, false);
}

void drvLcdWaitTransferDone(void)
{
    prvWaitGouda();
}

static int prvToVlInputFmt(uint8_t in_fmt)
{
    if (in_fmt == DRV_LCD_IN_FMT_RGB565)
        return 0;
    if (in_fmt == DRV_LCD_IN_FMT_UYVY)
        return 1;
    if (in_fmt == DRV_LCD_IN_FMT_YUYV)
        return 2;
    if (in_fmt == DRV_LCD_IN_FMT_IYUV)
        return 3;
    return -1;
}

static int prvToFrameInputFmt(uint8_t in_fmt)
{
    if (in_fmt == DRV_LCD_IN_FMT_RGB565)
        return 0;
    if (in_fmt == DRV_LCD_IN_FMT_ARGB8888)
        return 1;
    return -1;
}

static bool prvSetVideoLayer(const drvLcdVideoLayer_t *vl)
{
    if (vl == NULL || !vl->enabled)
    {
        hwp_gouda->gd_vl_input_fmt = 0;
        return true;
    }

    if (drvLcdAreaIsNul(&vl->out))
        return false;
    if (vl->buf_width == 0 || vl->buf_height == 0)
        return false;
    if (vl->stride < vl->buf_width)
        return false;

    int in_fmt = prvToVlInputFmt(vl->in_fmt);
    if (in_fmt < 0)
        return false;

    unsigned bpp = (vl->in_fmt == DRV_LCD_IN_FMT_IYUV) ? 1 : 2; // byte per pixel
    unsigned y_bl_offset = (vl->buf_height - 1) * vl->stride * bpp;
    unsigned uv_bl_offset = (vl->buf_height / 2 - 1) * vl->stride / 2;
    unsigned bwidth = vl->rotation ? vl->buf_height : vl->buf_width;
    unsigned bheight = vl->rotation ? vl->buf_width : vl->buf_height;
    unsigned xpitch = (bwidth << 8) / vl->out.w;
    unsigned ypitch = (bheight << 8) / vl->out.h;
    unsigned by = (uint32_t)vl->buf + (vl->rotation ? y_bl_offset : 0);
    unsigned bu = (vl->buf_u == NULL) ? 0 : (uint32_t)vl->buf_u + (vl->rotation ? uv_bl_offset : 0);
    unsigned bv = (vl->buf_v == NULL) ? 0 : (uint32_t)vl->buf_v + (vl->rotation ? uv_bl_offset : 0);

    if (vl->in_fmt == DRV_LCD_IN_FMT_IYUV)
    {
        if (!OSI_IS_ALIGNED(vl->buf, 16) || !OSI_IS_ALIGNED(vl->buf_u, 16) ||
            !OSI_IS_ALIGNED(vl->buf_v, 16) || !OSI_IS_ALIGNED(vl->stride, 16))
            return false;
    }
    else if (vl->in_fmt == DRV_LCD_IN_FMT_UYVY || vl->in_fmt == DRV_LCD_IN_FMT_YUYV)
    {
        if (!OSI_IS_ALIGNED(vl->buf, 4))
            return false;
    }
    else if (vl->in_fmt == DRV_LCD_IN_FMT_RGB565)
    {
        if (!OSI_IS_ALIGNED(vl->buf, 2))
            return false;
    }

    if (xpitch > (0x100 * 4) || xpitch < (0x100 / 4))
        return false;
    if (ypitch > (0x100 * 4) || ypitch < (0x100 / 4))
        return false;

    hwp_gouda->gd_vl_y_src = by;
    hwp_gouda->gd_vl_u_src = bu;
    hwp_gouda->gd_vl_v_src = bv;
    hwp_gouda->gd_vl_extents =
        GOUDA_MAX_COL(bwidth - 1) |
        GOUDA_MAX_LINE(bheight - 1);
    hwp_gouda->gd_vl_resc_ratio =
        GOUDA_XPITCH(xpitch) |
        GOUDA_YPITCH(ypitch) |
        GOUDA_YPITCH_SCALE_ENABLE;
    hwp_gouda->gd_vl_tl_ppos =
        GOUDA_X0(vl->out.x) |
        GOUDA_Y0(vl->out.y);
    hwp_gouda->gd_vl_br_ppos =
        GOUDA_X1(drvLcdAreaEndX(&vl->out)) |
        GOUDA_Y1(drvLcdAreaEndY(&vl->out));
    hwp_gouda->gd_vl_blend_opt =
        GOUDA_DEPTH(vl->depth) |
        GOUDA_ROTATION(vl->rotation) |
        GOUDA_ALPHA(vl->alpha) |
        GOUDA_CHROMA_KEY_COLOR(vl->key_color) |
        (vl->key_en ? GOUDA_CHROMA_KEY_ENABLE : 0) |
        GOUDA_CHROMA_KEY_MASK(vl->key_mask);
    hwp_gouda->gd_vl_input_fmt =
        GOUDA_FORMAT(in_fmt) |
        GOUDA_STRIDE(vl->stride * bpp) |
        GOUDA_ACTIVE;
    return true;
}

static bool prvSetOverlay(unsigned n, const drvLcdOverlay_t *ovl)
{
    if (ovl == NULL || !ovl->enabled)
    {
        hwp_gouda->overlay_layer[n].gd_ol_input_fmt = 0;
        return true;
    }

    if (drvLcdAreaIsNul(&ovl->out))
        return false;
    if (!OSI_IS_ALIGNED(ovl->buf, 2))
        return false;

    int in_fmt = prvToFrameInputFmt(ovl->in_fmt);
    if (in_fmt < 0)
        return false;

    hwp_gouda->overlay_layer[n].gd_ol_rgb_src = (uint32_t)ovl->buf;
    hwp_gouda->overlay_layer[n].gd_ol_tl_ppos =
        GOUDA_X0(ovl->out.x) |
        GOUDA_Y0(ovl->out.y);
    hwp_gouda->overlay_layer[n].gd_ol_br_ppos =
        GOUDA_X1(drvLcdAreaEndX(&ovl->out)) |
        GOUDA_Y1(drvLcdAreaEndY(&ovl->out));
    hwp_gouda->overlay_layer[n].gd_ol_blend_opt =
        GOUDA_ALPHA(ovl->alpha) |
        GOUDA_CHROMA_KEY_COLOR(ovl->key_color) |
        (ovl->key_en ? GOUDA_CHROMA_KEY_ENABLE : 0) |
        GOUDA_CHROMA_KEY_MASK(ovl->key_mask);
    hwp_gouda->overlay_layer[n].gd_ol_input_fmt =
        GOUDA_FORMAT(in_fmt) |
        GOUDA_STRIDE(ovl->stride) |
        GOUDA_PREFETCH |
        GOUDA_ACTIVE;
    return true;
}

bool drvLcdFlush(drvLcd_t *d, const drvLcdLayers_t *cfg, bool sync)
{
    if (d == NULL || cfg == NULL)
        return false;

    osiMutexLock(gGoudaCtx.lock);

    unsigned tick1 = osiUpHWTick32();

    if (d->state != LCD_STATE_OPENED)
        goto fail_unlock;

    if (drvLcdAreaIsNul(&cfg->layer_roi))
        goto fail_unlock;
    if (!drvLcdAreaShapeEqual(&cfg->layer_roi, &cfg->screen_roi))
        goto fail_unlock;

    prvWaitGouda();
    unsigned tick2 = osiUpHWTick32() - tick1;

    if (!prvSetVideoLayer(cfg->vl))
        goto fail_unlock;

    for (unsigned n = 0; n < DRV_LCD_OVERLAY_COUNT; n++)
    {
        if (!prvSetOverlay(n, cfg->ovl[n]))
            goto fail_unlock;
    }

    hwp_gouda->gd_lcd_ctrl = d->lcd_ctrl.v;
    hwp_gouda->gd_lcd_mem_address = d->mem_address;
    hwp_gouda->gd_spilcd_config = d->spi_config.v;
    hwp_gouda->gd_roi_tl_ppos =
        GOUDA_X0(cfg->layer_roi.x) |
        GOUDA_Y0(cfg->layer_roi.y);
    hwp_gouda->gd_roi_br_ppos =
        GOUDA_X0(drvLcdAreaEndX(&cfg->layer_roi)) |
        GOUDA_Y0(drvLcdAreaEndY(&cfg->layer_roi));
    hwp_gouda->gd_roi_bg_color = cfg->bg_color;

    drvLcdDirection_t dir = drvLcdDirCombine(d->app_dir, d->desc->dir);
    d->desc->ops.blit_prepare(d, dir, &cfg->screen_roi);

    osiDCacheCleanAll(); // it is simpler than clean needed
    hwp_gouda->gd_eof_irq_mask = GOUDA_EOF_MASK;
    hwp_gouda->gd_command = GOUDA_START;
    osiRequestSysClkActive(&gGoudaCtx.clk_constrain);

    unsigned tick3 = osiUpHWTick32() - tick1;
    if (sync)
        prvWaitGouda();

    unsigned tick4 = osiUpHWTick32() - tick1;
    OSI_LOGD(0, "lcd flush ticks: %d %d %d", tick2, tick3, tick4);
    osiMutexUnlock(gGoudaCtx.lock);
    return true;

fail_unlock:
    osiMutexUnlock(gGoudaCtx.lock);
    return false;
}

bool drvLcdFill(drvLcd_t *d, uint16_t color, const drvLcdArea_t *screen_roi, bool sync)
{
    if (d == NULL || d->desc == NULL)
        return false;

    drvLcdArea_t roi;
    if (screen_roi != NULL)
    {
        roi = *screen_roi;
    }
    else
    {
        roi.x = 0;
        roi.y = 0;
        roi.w = d->desc->width;
        roi.h = d->desc->height;
        if (drvLcdDirSwapped(drvLcdDirCombine(d->app_dir, d->desc->dir)))
            OSI_SWAP(uint16_t, roi.w, roi.h);
    }

    drvLcdLayers_t layers = {
        .bg_color = color,
        .layer_roi = roi,
        .screen_roi = roi,
    };
    // dcache clean inside is not needed
    return drvLcdFlush(d, &layers, sync);
}

bool drvLcdBlend(const drvLcdLayers_t *cfg, void *buf, unsigned stride)
{
    if (cfg == NULL || buf == NULL)
        return false;

    osiMutexLock(gGoudaCtx.lock);

    bool close_gouda_needed = false;
    if (stride < cfg->screen_roi.w)
        goto fail_unlock;

    if (drvLcdAreaIsNul(&cfg->layer_roi))
        goto fail_unlock;
    if (!drvLcdAreaShapeEqual(&cfg->layer_roi, &cfg->screen_roi))
        goto fail_unlock;

    if (gGoudaCtx.state == GOUDA_STATE_CLOSE)
    {
        close_gouda_needed = true;
        prvGoudaInit();
    }

    prvWaitGouda();
    if (!prvSetVideoLayer(cfg->vl))
        goto fail_unlock;

    for (unsigned n = 0; n < DRV_LCD_OVERLAY_COUNT; n++)
    {
        if (!prvSetOverlay(n, cfg->ovl[n]))
            goto fail_unlock;
    }

    hwp_gouda->gd_roi_tl_ppos =
        GOUDA_X0(cfg->layer_roi.x) |
        GOUDA_Y0(cfg->layer_roi.y);
    hwp_gouda->gd_roi_br_ppos =
        GOUDA_X0(drvLcdAreaEndX(&cfg->layer_roi)) |
        GOUDA_Y0(drvLcdAreaEndY(&cfg->layer_roi));
    hwp_gouda->gd_roi_bg_color = cfg->bg_color;

    REG_GOUDA_GD_LCD_CTRL_T lcd_ctrl = {
        .b.destination = GOUDA_DESTINATION_V_MEMORY_RAM,
        .b.output_format = GOUDA_OUTPUT_FORMAT_V_16_BIT_RGB565,
    };

    hwp_gouda->gd_lcd_mem_address = (uint32_t)buf + (cfg->screen_roi.x + cfg->screen_roi.y * stride) * sizeof(uint16_t);
    hwp_gouda->gd_lcd_stride_offset = (stride - cfg->screen_roi.w) * sizeof(uint16_t);
    hwp_gouda->gd_lcd_ctrl = lcd_ctrl.v;

    unsigned outline_start = (uint32_t)buf + cfg->screen_roi.y * stride * sizeof(uint16_t);
    unsigned outline_size = cfg->screen_roi.h * stride * sizeof(uint16_t);

    osiDCacheCleanAll(); // it is simpler than clean needed
    osiDCacheInvalidate((void *)outline_start, outline_size);

    hwp_gouda->gd_eof_irq_mask = GOUDA_EOF_MASK;
    hwp_gouda->gd_command = GOUDA_START;
    osiRequestSysClkActive(&gGoudaCtx.clk_constrain);

    prvWaitGouda();
    if (close_gouda_needed)
        prvGoudaDeinit();

    osiMutexUnlock(gGoudaCtx.lock);
    return true;

fail_unlock:
    if (close_gouda_needed)
        prvGoudaDeinit();

    osiMutexUnlock(gGoudaCtx.lock);
    return false;
}

const drvLcdPanelDesc_t *drvLcdGetDesc(drvLcd_t *d)
{
    return d->desc;
}

void drvLcdWriteCmd(drvLcd_t *d, uint16_t cmd)
{
    OSI_LOOP_WAIT((hwp_gouda->gd_status & GOUDA_LCD_BUSY) == 0);
    hwp_gouda->gd_lcd_ctrl = d->lcd_ctrl.v;
    hwp_gouda->gd_lcd_mem_address = d->mem_address;
    hwp_gouda->gd_lcd_single_access = GOUDA_START_WRITE | GOUDA_LCD_DATA(cmd);
    OSI_LOOP_WAIT((hwp_gouda->gd_status & GOUDA_LCD_BUSY) == 0);
}

void drvLcdWriteData(drvLcd_t *d, uint16_t data)
{
    OSI_LOOP_WAIT((hwp_gouda->gd_status & GOUDA_LCD_BUSY) == 0);
    hwp_gouda->gd_lcd_ctrl = d->lcd_ctrl.v;
    hwp_gouda->gd_lcd_mem_address = d->mem_address;
    hwp_gouda->gd_lcd_single_access = GOUDA_START_WRITE | GOUDA_TYPE | GOUDA_LCD_DATA(data);
    OSI_LOOP_WAIT((hwp_gouda->gd_status & GOUDA_LCD_BUSY) == 0);
}

void drvLcdReadData(drvLcd_t *d, uint16_t cmd, uint8_t *data, unsigned len)
{
    bool spilcd = (d->spi_config.v & GOUDA_SPI_LCD_SELECT);
    if (spilcd && (len > GD_MAX_SLCD_READ_LEN))
        osiPanic();

    OSI_LOOP_WAIT((hwp_gouda->gd_status & GOUDA_LCD_BUSY) == 0);
    hwp_gouda->gd_lcd_ctrl = d->lcd_ctrl.v;
    hwp_gouda->gd_lcd_mem_address = d->mem_address;
    if (spilcd)
    {
        REG_GOUDA_GD_SPILCD_CONFIG_T spi_config = {d->spi_config.v};
        spi_config.b.spi_rx_byte = len;
        spi_config.b.spi_rw = GOUDA_SPI_RW_V_READ;
        spi_config.b.spi_dummy_cycle = 1;
        spi_config.b.spi_clk_divider = CONFIG_DEFAULT_SYSAHB_FREQ / LCD_LOW_FREQ;
        if (spi_config.b.spi_line == GOUDA_SPI_LINE_V_3_TWO_LANE)
            spi_config.b.spi_line = GOUDA_SPI_LINE_V_3;
        hwp_gouda->gd_spilcd_config = spi_config.v;
    }

    hwp_gouda->gd_lcd_single_access = GOUDA_START_WRITE | GOUDA_LCD_DATA(cmd);
    OSI_LOOP_WAIT((hwp_gouda->gd_status & GOUDA_LCD_BUSY) == 0);

    if (spilcd)
    {
        unsigned rd = hwp_gouda->gd_spilcd_rd;
        for (unsigned n = 0; n < len; n++)
            data[n] = (rd >> (8 * n)) & 0xff;

        hwp_gouda->gd_spilcd_config = d->spi_config.v;
    }
    else
    {
        for (unsigned n = 0; n < len; n += 2)
        {
            hwp_gouda->gd_lcd_single_access = GOUDA_START_READ | GOUDA_TYPE;
            OSI_LOOP_WAIT((hwp_gouda->gd_status & GOUDA_LCD_BUSY) == 0);

            unsigned rd = hwp_gouda->gd_lcd_single_access;
            data[n] = rd & 0xff;
            data[n + 1] = (rd >> 8) & 0xff;
        }
    }
}

bool drvLcdDummyProbe(drvLcd_t *d) { return false; }
void drvLcdDummyInit(drvLcd_t *d) {}
void drvLcdDummyBlitPrepare(drvLcd_t *d, drvLcdDirection_t dir, const drvLcdArea_t *roi) {}
