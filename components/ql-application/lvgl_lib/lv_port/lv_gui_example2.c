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

static lv_group_t *g;
static lv_obj_t *btn_enable;
static lv_style_t style_mbox_bg;
static void mbox_event_cb(lv_obj_t *mbox, lv_event_t event)
{
    if (event != LV_EVENT_CLICKED)
        return;

    const char *btn_txt = lv_mbox_get_active_btn_text(mbox);
    if (btn_txt)
    {
        lv_group_focus_freeze(g, false); /*Release the freeze*/

        /*Revert the top layer to not block*/
        lv_obj_set_style(lv_disp_get_layer_top(NULL), &lv_style_transp);
        lv_obj_set_click(lv_disp_get_layer_top(NULL), false);

        /*Mark the enabled state by toggling the button*/
        if (strcmp(btn_txt, "No") == 0)
            lv_btn_set_state(btn_enable, LV_BTN_STATE_REL);
        else if (strcmp(btn_txt, "Yes") == 0)
            lv_btn_set_state(btn_enable, LV_BTN_STATE_TGL_REL);

        lv_obj_del(mbox);
    }
}

static void message_btn_event_cb(lv_obj_t *btn, lv_event_t event)
{
    if (event != LV_EVENT_RELEASED)
        return; /*We only care only with the release event*/

    /*If the butto nsi released the show message box to be sure about the Enable*/
    if (lv_btn_get_state(btn) == LV_BTN_STATE_REL)
    {
        /* Create a dark screen sized bg. with opacity to show
         * the other objects are not available now*/
        lv_obj_set_style(lv_disp_get_layer_top(NULL), &style_mbox_bg);
        lv_obj_set_click(lv_disp_get_layer_top(NULL), false); /*It should be `true` but it'd block the emulated keyboard too*/

        /*Create a message box*/
        lv_obj_t *mbox = lv_mbox_create(lv_disp_get_layer_top(NULL), NULL);
        lv_mbox_set_text(mbox, "Turn on something?");
        lv_obj_set_event_cb(mbox, mbox_event_cb);
        lv_group_add_obj(g, mbox); /*Add to he group*/

        /*Add two buttons*/
        static const char *btns[] = {"Yes", "No", ""};
        lv_mbox_add_btns(mbox, btns);

        lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, -LV_DPI / 2);

        /*Focus on the new message box, can freeze focus on it*/
        lv_group_focus_obj(mbox);
        lv_group_focus_freeze(g, true);
    }
    /*Just disable without message*/
    else
    {
        lv_btn_set_state(btn_enable, LV_BTN_STATE_REL);
    }
}

void lv_tutorial_keyboard(lv_indev_t *kp_indev)
{
    /*Create an object group*/
    g = lv_group_create();

    /*Assig the input device(s) to the created group*/
    lv_indev_set_group(kp_indev, g);

    /*Create a demo GUI*/
    lv_obj_t *scr = lv_disp_get_scr_act(NULL); /*Get the current screen*/

    /*Create a drop down list*/
    lv_obj_t *ddlist = lv_ddlist_create(scr, NULL);
    lv_ddlist_set_options(ddlist, "Low\nMedium\nHigh");
    lv_obj_set_pos(ddlist, LV_DPI / 4, LV_DPI / 4);
    lv_group_add_obj(g, ddlist); /*Add the object to the group*/

    /*Create a holder and check boxes on it*/
    lv_obj_t *holder = lv_cont_create(scr, NULL); /*Create a transparent holder*/
    lv_cont_set_fit(holder, LV_FIT_TIGHT);

    lv_cont_set_layout(holder, LV_LAYOUT_COL_L);
    lv_obj_set_style(holder, &lv_style_transp);
    lv_obj_align(holder, ddlist, LV_ALIGN_OUT_RIGHT_TOP, LV_DPI / 4, 0);

    lv_obj_t *cb = lv_cb_create(holder, NULL); /*First check box*/
    lv_cb_set_text(cb, "Red");
    lv_group_add_obj(g, cb); /*Add to the group*/

    cb = lv_cb_create(holder, cb); /*Copy the first check box. Automatically added to the same group*/
    lv_cb_set_text(cb, "Green");

    cb = lv_cb_create(holder, cb); /*Copy the second check box. Automatically added to the same group*/
    lv_cb_set_text(cb, "Blue");

    /*Create a sliders*/
    lv_obj_t *slider = lv_slider_create(scr, NULL);
    lv_obj_set_size(slider, LV_DPI, LV_DPI / 3);
    lv_obj_align(slider, ddlist, LV_ALIGN_OUT_BOTTOM_MID, 0, LV_DPI * 3 / 4);
    lv_bar_set_range(slider, 0, 20);
    lv_group_add_obj(g, slider); /*Add to the group*/

    /*Create a button*/
    btn_enable = lv_btn_create(scr, NULL);
    lv_obj_set_event_cb(btn_enable, message_btn_event_cb);
    lv_btn_set_fit(btn_enable, LV_FIT_TIGHT);
    lv_group_add_obj(g, btn_enable); /*Add to the group*/
    lv_obj_t *l = lv_label_create(btn_enable, NULL);
    lv_label_set_text(l, "Message");
    lv_obj_align(btn_enable, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, LV_DPI / 4);

    /* Create a dark plain style for a message box's background (modal)*/
    lv_style_copy(&style_mbox_bg, &lv_style_plain);
    style_mbox_bg.body.main_color = LV_COLOR_BLACK;
    style_mbox_bg.body.grad_color = LV_COLOR_BLACK;
    style_mbox_bg.body.opa = LV_OPA_50;
}

static void lvglKeyboardCreate(void)
{
    lv_tutorial_keyboard(lvGuiGetKeyPad());
}

void lvGuiStartExample2(void)
{
    OSI_LOGI(0, "lvgl example2 start");

    drvLcdInitV2();

    drvLcd_t *lcd = drvLcdGetByname(DRV_NAME_LCD1);
    drvLcdOpenV2(lcd);
    drvLcdFill(lcd, 0, NULL, true);
    drvLcdSetBackLightEnable(lcd, true);

    lvGuiInit(lvglKeyboardCreate);
}
