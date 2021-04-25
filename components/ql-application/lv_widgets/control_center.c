/** 
* @Time    :   2020/12/24 11:17:13
* @FileName:   control_center.c
* @Author  :   Wang Hua(whfyzg@gmail.com)
* @Software:   VSCode
* @Descripton:  quick device control screen 
*/
#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "ic_widgets_inc.h"

typedef struct {
    lv_obj_t* controller;
}control_center_context_t;

static control_center_context_t g_controller_ctx;

static void control_screen_event_cb(lv_obj_t* obj, lv_event_t event)
{
    static lv_point_t down_pos;
    static lv_point_t last_pos;
    lv_point_t pos;
    static uint8_t x_flag = 0, y_flag = 0;
    static uint32_t down_tick = 0;
    static int8_t direct = -1;
    static int8_t direct_x = -1, direct_y = -1;
    static lv_point_t offset;
    static ic_obj_message_t message;
    message.sender = g_controller_ctx.controller;

    //LOGI("infocontroller_screen_event_cb,obj:%x, event:%d\n", obj, event);

    switch (event) {
    case LV_EVENT_PRESSED:
        lv_indev_get_point(lv_indev_get_act(), &down_pos);
        LOGI("pressed(%d, %d)\n", down_pos.x, down_pos.y);
        last_pos = down_pos;
        direct = -1;
        down_tick = lv_tick_get();
        direct_x = -1;
        direct_y = -1;
        offset.x = 0;
        offset.y = 0;


        break;
    case LV_EVENT_PRESSING: {
        lv_indev_get_point(lv_indev_get_act(), &pos);
        LOGI("pressing(%d, %d)\n", pos.x, pos.y);

        int16_t offset_x = pos.x - last_pos.x;
        int16_t offset_y = pos.y - last_pos.y;

        if (offset_x > 0) direct_x = 0;
        if (offset_x < 0) direct_x = 1;

        if (offset_y > 0) direct_y = 0;
        if (offset_y < 0) direct_y = 1;

        offset.x += offset_x;
        offset.y += offset_y;

        if (direct == -1) {
            if (abs(offset_x) > abs(offset_y) && abs(offset_x) > 5) {
                direct = 1;
            }
            else if (abs(offset_y) > abs(offset_x)) {
                if (offset_y > 5) {
                    direct = 2;
                }
                if (offset_y < -5) {
                    direct = 3;
                }
            }
        }

        last_pos = pos;

        if (direct == 3) {
            if (offset.y > 0) offset.y = 0;

            message.msg = IC_OBJ_MSG_GRP_MOVE;
            message.data = (void*)&offset;

            lv_event_send(g_controller_ctx.controller->parent, LV_EVENT_VALUE_CHANGED, &message);
        }

//        if (direct == 3) {
//            if (offset.y < 0)
//                lv_obj_set_pos(g_controller_ctx.controller, 0, 0 + offset.y);
//            else
//                lv_obj_set_pos(g_controller_ctx.controller, 0, 0);
//        }

        break;


    case LV_EVENT_SHORT_CLICKED:
        LOGI("short click\n");
        break;

    case LV_EVENT_CLICKED:
        LOGI("clicked\n");
        break;

    case LV_EVENT_LONG_PRESSED:
        LOGI("LONG pressed\n");
        break;

    case LV_EVENT_LONG_PRESSED_REPEAT:
        LOGI("long pressed repeat\n");
        break;

    case LV_EVENT_PRESS_LOST:
        LOGI("press lost\n");
    case LV_EVENT_RELEASED: {
        lv_indev_get_point(lv_indev_get_act(), &pos);
        LOGI("release(%d, %d)\n", pos.x, pos.y);

        int16_t offset_x = pos.x - last_pos.x;
        int16_t offset_y = pos.y - last_pos.y;

        offset.x += offset_x;
        offset.y += offset_y;

        if (offset_x > 0) direct_x = 0;
        if (offset_x < 0) direct_x = 1;

        if (offset_y > 0) direct_y = 0;
        if (offset_y < 0) direct_y = 1;


        if (direct == 3) {
            message.data = (void*)&offset;
            if ((lv_tick_get() - down_tick < MAINSCREEN_PULL_FAST_TIME)) {
                message.msg = IC_OBJ_MSG_GRP_ANIM_FAST;
            }
            else {
                message.msg = IC_OBJ_MSG_GRP_ANIM_SLOW;
            }
            lv_event_send(g_controller_ctx.controller->parent, LV_EVENT_VALUE_CHANGED, &message);
        }
    }
                          break;

    case LV_EVENT_DRAG_BEGIN:
        LOGI("drage begin\n");
        break;

    case LV_EVENT_DRAG_END:
        LOGI("drag end\n");
        break;
    case LV_EVENT_DRAG_THROW_BEGIN:
        LOGI("drag throw begin\n");
        break;
    case LV_EVENT_GESTURE:
        LOGI("gesture\n");
        break;
    case LV_EVENT_KEY:
        LOGI("KEY\n");
        break;
    case LV_EVENT_FOCUSED:
        LOGI("FOCUSED\n");
        break;
    case LV_EVENT_DEFOCUSED:
        LOGI("DEFOCUSED\n");
        break;
    case LV_EVENT_LEAVE:
        LOGI("leave\n");
        break;
    case LV_EVENT_VALUE_CHANGED:
        LOGI("value changed\n");
        break;
    case LV_EVENT_INSERT:
        LOGI("insert\n");
        break;
    case LV_EVENT_REFRESH:
        LOGI("refresh\n");
        break;
    case LV_EVENT_APPLY:
        LOGI("apply\n");
        break;
    case LV_EVENT_CANCEL:
        LOGI("cancel\n");
        break;
    case LV_EVENT_DELETE:
        LOGI("delete\n");
        break;
    }
    }
}

/*****************************************************************************************************
*
* create, drawand update control center
*
****************************************************************************************************/
lv_obj_t* ic_control_center_create(lv_obj_t* parent, lv_style_t* style)
{
    if (!parent || !style) return NULL;

    lv_obj_t* controller = lv_page_create(parent, NULL);
    lv_obj_add_style(controller, LV_OBJ_PART_MAIN, style);

 

    lv_obj_set_size(controller, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_obj_set_pos(controller, 0, -LV_VER_RES_MAX);

    /**
     Draw infocontroller center like this
    */
    //date area
    lv_obj_t* date = lv_label_create(controller, NULL);

    static lv_style_t date_style;
    lv_style_init(&date_style);
    lv_style_set_text_color(&date_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&date_style, LV_STATE_DEFAULT, &lv_font_montserrat_30);
    lv_style_set_bg_color(&date_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_width(&date_style, LV_STATE_DEFAULT, 0);

    lv_label_set_align(date, LV_LABEL_ALIGN_CENTER);
    lv_obj_add_style(date, LV_OBJ_PART_MAIN, &date_style);
    lv_obj_align(date, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    lv_label_set_text(date, _("12/17"));

    //week area
    lv_obj_t* week = lv_label_create(controller, NULL);

    static lv_style_t week_style;
    lv_style_init(&week_style);
    lv_style_set_text_color(&week_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&week_style, LV_STATE_DEFAULT, &lv_font_montserrat_14);
    lv_style_set_bg_color(&week_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_width(&week_style, LV_STATE_DEFAULT, 0);

    lv_label_set_align(week, LV_LABEL_ALIGN_CENTER);
    lv_obj_add_style(week, LV_OBJ_PART_MAIN, &week_style);
    lv_obj_align(week, NULL, LV_ALIGN_IN_TOP_MID, 0, 52);

    lv_label_set_text(week, _("Tue"));
    
    //infos


    lv_obj_set_event_cb(controller, control_screen_event_cb);
    lv_obj_set_event_cb(date, control_screen_event_cb);
    lv_obj_set_event_cb(week, control_screen_event_cb);

    g_controller_ctx.controller = controller;

    return controller;
}

void ic_control_center_delete(lv_obj_t* obj)
{
}

void ic_control_center_update(lv_obj_t* obj)
{
}
