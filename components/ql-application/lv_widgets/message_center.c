/** 
* @Time    :   2020/12/24 11:19:41
* @FileName:   message_center.c
* @Author  :   Wang Hua(whfyzg@gmail.com)
* @Software:   VSCode
* @Descripton:  
*/
#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "ic_widgets_inc.h"

typedef struct {
    lv_obj_t* message_obj;
}message_center_context_t;

static message_center_context_t g_message_center_ctx;

static void message_center_screen_event_cb(lv_obj_t* obj, lv_event_t event)
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
    message.sender = g_message_center_ctx.message_obj;

    LOGI("infocontroller_screen_event_cb,obj:%x, event:%d\n", obj, event);

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

        if (direct == 2) {
            if (offset.y < 0) offset.y = 0;

            message.msg = IC_OBJ_MSG_GRP_MOVE;
            message.data = (void*)&offset;

            lv_event_send(g_message_center_ctx.message_obj->parent, LV_EVENT_VALUE_CHANGED, &message);
        }
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


        if (direct == 2) {
            message.data = (void*)&offset;
            if ((lv_tick_get() - down_tick < MAINSCREEN_PULL_FAST_TIME)) {
                message.msg = IC_OBJ_MSG_GRP_ANIM_FAST;
            }
            else {
                message.msg = IC_OBJ_MSG_GRP_ANIM_SLOW;
            }
            lv_event_send(g_message_center_ctx.message_obj->parent, LV_EVENT_VALUE_CHANGED, &message);
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
* create, drawand update inofcard center
*
****************************************************************************************************/
lv_obj_t * ic_message_center_create(lv_obj_t * parent, lv_style_t * style)
{
    if (!parent || !style) return NULL;

    lv_obj_t* obj = lv_obj_create(parent, NULL);
    lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);

    lv_obj_set_size(obj, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_obj_set_pos(obj, 0, LV_VER_RES_MAX);

    /**
     Draw infocard senter
    */
    //title bar
    static lv_style_t title_style;
    lv_style_init(&title_style);
    lv_style_set_text_color(&title_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&title_style, LV_STATE_DEFAULT, ic_font_get(IC_FONT_SMALL));
    lv_style_set_bg_color(&title_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_width(&title_style, LV_STATE_DEFAULT, 0);

    screen_title_bar_desc_t title_desc;
    title_desc.title = _("Message");
    title_desc.title_style = &title_style;

    ic_watch_create_title_bar(obj, &title_desc);

    lv_obj_set_event_cb(obj, message_center_screen_event_cb);
 

    g_message_center_ctx.message_obj = obj;

    return obj;
}

void ic_message_center_delete(lv_obj_t* obj)
{
}

void ic_message_center_update(lv_obj_t* obj)
{
}
