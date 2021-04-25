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

#include "lv_gui_main.h"
#include "drv_lcd_v2.h"
#include "drv_names.h"
#include "drv_keypad.h"
#include "lvgl.h"
#include "osi_api.h"
#include "osi_log.h"
#include <string.h>
#include <stdlib.h>

typedef struct
{
    bool screen_on;            // state of screen on
    bool keypad_pending;       // keypad pending, set in ISR, clear in thread
    bool anim_inactive;        // property of whether animation is regarded as inactive
    drvLcd_t *lcd;             // LCD instance
    osiThread_t *thread;       // gui thread
    osiTimer_t *task_timer;    // timer to trigger task handler
    drvLcdVideoLayer_t vl;     // extern video layer
    lv_disp_buf_t disp_buf;    // display buffer
    lv_disp_t *disp;           // display device
    lv_indev_t *keypad;        // keypad device
    keyMap_t last_key;         // last key from ISR
    keyState_t last_key_state; // last key state from ISR
    uint32_t screen_on_users;  // screen on user bitmap
    uint32_t inactive_timeout; // property of inactive timeout
} lvGuiContext_t;

typedef struct
{
    uint8_t keypad;
    uint8_t lv_key;
} lvGuiKeypadMap_t;

static const lvGuiKeypadMap_t gLvKeyMap[] = {
    {KEY_MAP_POWER, 0xf0},
    {KEY_MAP_SIM1, 0xf1},
    {KEY_MAP_SIM2, 0xf2},
    {KEY_MAP_0, '0'},
    {KEY_MAP_1, '1'},
    {KEY_MAP_2, '2'},
    {KEY_MAP_3, '3'},
    {KEY_MAP_4, '4'},
    {KEY_MAP_5, '5'},
    {KEY_MAP_6, '6'},
    {KEY_MAP_7, '7'},
    {KEY_MAP_8, '8'},
    {KEY_MAP_9, '9'},
    {KEY_MAP_STAR, '*'},
    {KEY_MAP_SHARP, '#'},
    {KEY_MAP_OK, LV_KEY_ENTER},
    {KEY_MAP_LEFT, LV_KEY_LEFT},
    {KEY_MAP_RIGHT, LV_KEY_RIGHT},
    {KEY_MAP_UP, LV_KEY_UP},
    {KEY_MAP_DOWN, LV_KEY_DOWN},
    {KEY_MAP_SOFT_L, LV_KEY_PREV},
    {KEY_MAP_SOFT_R, LV_KEY_NEXT},
};

static lvGuiContext_t gLvGuiCtx;

/**
 * flush display forcedly
 */
static void prvDispForceFlush(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    drvLcdArea_t roi = {
        .x = 0,
        .y = 0,
        .w = d->disp->driver.hor_res,
        .h = d->disp->driver.ver_res,
    };

    drvLcdOverlay_t ovl = {
        .buf = d->disp_buf.buf1,
        .enabled = true,
        .in_fmt = DRV_LCD_IN_FMT_RGB565,
        .alpha = 255,
        .key_en = LV_INDEXED_CHROMA,
        .key_color = lv_color_to16(LV_COLOR_TRANSP),
        .stride = roi.w,
        .out = roi,
    };
    drvLcdLayers_t layers = {
        .vl = &d->vl,
        .ovl = {NULL, NULL, &ovl},
        .layer_roi = roi,
        .screen_roi = roi,
    };

    drvLcdFlush(d->lcd, &layers, true);
}


/**
 * initialize LCD display device
 */
static bool prvLvInitLcd(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    d->lcd = drvLcdGetByname(DRV_NAME_LCD1);
    if (d->lcd == NULL)
        return false;

    if (!drvLcdOpenV2(d->lcd))
        return false;

    drvLcdSetDirection(d->lcd, DRV_LCD_DIR_NORMAL);

    drvLcdPanelInfo_t panel_info;
    if (!drvLcdGetPanelInfo(d->lcd, &panel_info))
        return false;

    unsigned pixel_cnt = panel_info.width * panel_info.height;
    lv_color_t *buf = (lv_color_t *)malloc(pixel_cnt * sizeof(lv_color_t));
    if (buf == NULL)
        return false;

    lv_disp_buf_init(&d->disp_buf, buf, buf, pixel_cnt);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = prvDispFlush;
    disp_drv.buffer = &d->disp_buf;
    d->disp = lv_disp_drv_register(&disp_drv); // pointer copy
    return true;
}

/**
 * callback of keypad driver, called in ISR
 */
static void prvKeypadCallback(keyMap_t key, keyState_t evt, void *p)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    d->last_key = key;
    d->last_key_state = evt;
    d->keypad_pending = true;
}

/**
 * keypad device read_cb
 */
static bool prvLvKeypadRead(lv_indev_drv_t *kp, lv_indev_data_t *data)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    uint32_t critical = osiEnterCritical();
    keyMap_t last_key = d->last_key;
    keyState_t last_key_state = d->last_key_state;
    bool keypad_pending = d->keypad_pending;
    d->keypad_pending = false;
    osiExitCritical(critical);

    if (keypad_pending)
    {
        data->state = (last_key_state & KEY_STATE_RELEASE) ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR;
        data->key = 0xff;
        for (unsigned n = 0; n < OSI_ARRAY_SIZE(gLvKeyMap); n++)
        {
            if (gLvKeyMap[n].keypad == last_key)
            {
                data->key = gLvKeyMap[n].lv_key;
                break;
            }
        }

        lvGuiScreenOn();
    }

    // no more to be read
    return false;
}

/**
 * initialize keypad device
 */
static bool prvLvInitKeypad(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    lv_indev_drv_t kp_drv;
    lv_indev_drv_init(&kp_drv);
    kp_drv.type = LV_INDEV_TYPE_KEYPAD;
    kp_drv.read_cb = prvLvKeypadRead;
    d->keypad = lv_indev_drv_register(&kp_drv); // pointer copy

    drvKeypadInit(); // permit multiple calls
    drvKeypadSetCB(prvKeypadCallback, KEY_STATE_PRESS | KEY_STATE_RELEASE, NULL);
    return true;
}

/**
 * run littlevgl task handler
 */
static void prvLvTaskHandler(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    lv_task_handler();

    unsigned next_run = lv_task_get_tick_next_run();
    osiTimerStartRelaxed(d->task_timer, next_run, OSI_WAIT_FOREVER);

    lv_refr_now(d->disp);
}

/**
 * whether inactive timeout
 */
static bool prvIsInactiveTimeout(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    if (d->screen_on_users != 0)
        return false;
    if (d->inactive_timeout == 0)
        return false;
    if (!d->anim_inactive && lv_anim_count_running())
        return false;
    return lv_disp_get_inactive_time(d->disp) > d->inactive_timeout;
}

/**
 * gui thread entry
 */
static void prvLvThreadEntry(void *param)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    d->thread = osiThreadCurrent();
    d->task_timer = osiTimerCreate(d->thread, (osiCallback_t)prvLvTaskHandler, NULL);

    lv_init();
    prvLvInitLcd();
    prvLvInitKeypad();

    lvGuiCreate_t create = (lvGuiCreate_t)param;
    if (create != NULL)
        create();

    osiTimerStart(d->task_timer, 0);
    lv_disp_trig_activity(d->disp);
    for (;;)
    {
        osiEvent_t event;
        event.id = OSI_EVENT_ID_NONE;

        osiEventWait(d->thread, &event);
        if (event.id == OSI_EVENT_ID_QUIT)
            break;

        if (d->screen_on && prvIsInactiveTimeout())
        {
            OSI_LOGI(0, "inactive timeout, screen off");
            lvGuiScreenOff();
        }
    }

    osiThreadExit();
}

/**
 * start gui based on littlevgl
 */
void lvGuiInit(lvGuiCreate_t create)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    d->screen_on = true;
    d->keypad_pending = false;
    d->anim_inactive = false;
    d->last_key = 0xff;
    d->last_key_state = KEY_STATE_RELEASE;
    d->screen_on_users = 0;
    d->inactive_timeout = CONFIG_LV_GUI_SCREEN_OFF_TIMEOUT;

    d->thread = osiThreadCreate("lvgl", prvLvThreadEntry, create,
                                OSI_PRIORITY_NORMAL,
                                CONFIG_LV_GUI_THREAD_STACK_SIZE,
                                CONFIG_LV_GUI_THREAD_EVENT_COUNT);
}

/**
 * get littlevgl gui thread
 */
osiThread_t *lvGuiGetThread(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    return d->thread;
}

/**
 * get keypad input device
 */
lv_indev_t *lvGuiGetKeyPad(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    return d->keypad;
}

/**
 * schedule a callback to be executed in gui thread
 */
void lvGuiThreadCallback(osiCallback_t cb, void *param)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    osiThreadCallback(d->thread, cb, param);
}

/**
 * send event to gui thread
 */
void lvGuiSendEvent(const osiEvent_t *evt)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    osiEventSend(d->thread, evt);
}

/**
 * request screen on
 */
bool lvGuiRequestSceenOn(uint8_t id)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    if (id > 31)
        return false;

    unsigned mask = (1 << id);
    d->screen_on_users |= mask;
    return true;
}

/**
 * release screen on request
 */
bool lvGuiReleaseScreenOn(uint8_t id)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    if (id > 31)
        return false;

    unsigned mask = (1 << id);
    d->screen_on_users &= ~mask;
    return true;
}

/**
 * turn off screen
 */
void lvGuiScreenOff(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    if (!d->screen_on)
        return;

    OSI_LOGI(0, "screen off");
    drvLcdSetBackLightEnable(d->lcd, false);
    drvLcdSleep(d->lcd);
    d->screen_on = false;
}

/**
 * turn on screen
 */
void lvGuiScreenOn(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    if (d->screen_on)
        return;

    OSI_LOGI(0, "screen on");
    drvLcdWakeup(d->lcd);
    prvDispForceFlush();
    drvLcdSetBackLightEnable(d->lcd, true);
    d->screen_on = true;
}

/**
 * set screen off timeout at inactive
 */
void lvGuiSetInactiveTimeout(unsigned timeout)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    d->inactive_timeout = timeout;
}

/**
 * set whether animation is regarded as inactive
 */
void lvGuiSetAnimationInactive(bool inactive)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    d->anim_inactive = inactive;
}
