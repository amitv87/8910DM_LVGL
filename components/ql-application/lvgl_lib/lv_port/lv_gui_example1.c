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

#include "lv_gui_main.h"
#include "drv_lcd_v2.h"
#include "drv_names.h"
#include "lvgl.h"
#include "osi_api.h"
#include "osi_log.h"
#include <string.h>
#include <stdlib.h>

// This example from https://github.com/littlevgl/lv_examples

static lv_style_t btn3_style;

static void lvglAnimCreate(void)
{
    lv_theme_t *th = lv_theme_material_init(210, NULL);
    lv_theme_set_current(th);

    lv_obj_t *scr = lv_disp_get_scr_act(NULL); /*Get the current screen*/

    lv_obj_t *label;

    /*Create a button the demonstrate built-in animations*/
    lv_obj_t *btn1;
    btn1 = lv_btn_create(scr, NULL);
    lv_obj_set_pos(btn1, 10, 10); /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn1, 80, 50);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Float");

    /* Float in the button using a built-in function
     * Delay the animation with 2000 ms and float in 300 ms. NULL means no end callback*/
    lv_anim_t a;
    a.var = btn1;
    a.start = -lv_obj_get_height(btn1);
    a.end = lv_obj_get_y(btn1);
    a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
    a.path_cb = lv_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = -2000; /*Delay the animation*/
    a.time = 300;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;
#if LV_USE_USER_DATA
    a.user_data = NULL;
#endif
    lv_anim_create(&a);

    /*Create a button to demonstrate user defined animations*/
    lv_obj_t *btn2;
    btn2 = lv_btn_create(scr, NULL);
    lv_obj_set_pos(btn2, 10, 80); /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn2, 80, 50);

    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Move");

    /*Create an animation to move the button continuously left to right*/
    a.var = btn2;
    a.start = lv_obj_get_x(btn2);
    a.end = a.start + (100);
    a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
    a.path_cb = lv_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = -1000;   /*Negative number to set a delay*/
    a.time = 400;         /*Animate in 400 ms*/
    a.playback = 1;       /*Make the animation backward too when it's ready*/
    a.playback_pause = 0; /*Wait before playback*/
    a.repeat = 1;         /*Repeat the animation*/
    a.repeat_pause = 500; /*Wait before repeat*/
    lv_anim_create(&a);

    /*Create a button to demonstrate the style animations*/
    lv_obj_t *btn3;
    btn3 = lv_btn_create(scr, NULL);
    lv_obj_set_pos(btn3, 10, 150); /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn3, 80, 50);

    label = lv_label_create(btn3, NULL);
    lv_label_set_text(label, "Style");

    /*Create a unique style for the button*/
    lv_style_copy(&btn3_style, lv_btn_get_style(btn3, LV_BTN_STYLE_REL));
    lv_btn_set_style(btn3, LV_BTN_STATE_REL, &btn3_style);

    /*Animate the new style*/
    lv_anim_t sa;
    lv_style_anim_init(&sa);
    lv_style_anim_set_styles(&sa, &btn3_style, &lv_style_btn_rel, &lv_style_pretty);
    lv_style_anim_set_time(&sa, 500, 500);
    lv_style_anim_set_playback(&sa, 500);
    lv_style_anim_set_repeat(&sa, 500);
    lv_style_anim_create(&sa);
}

void lvGuiStartExample1(void)
{
    OSI_LOGI(0, "lvgl example1 start");

    drvLcdInitV2();

    drvLcd_t *lcd = drvLcdGetByname(DRV_NAME_LCD1);
    if(drvLcdOpenV2(lcd))
    {
       drvLcdFill(lcd, 0, NULL, true);
       drvLcdSetBackLightEnable(lcd, true);
       lvGuiInit(lvglAnimCreate);
    }
}
