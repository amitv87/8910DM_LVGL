/** 
* @Time    :   2020/12/24 11:18:16
* @FileName:   infocard_center.c
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
    lv_obj_t *card;
    lv_obj_t* card_grp;
    uint16_t grp_height;
    int16_t  grp_start_y;
}card_center_context_t;

static card_center_context_t g_card_center_ctx;

/**
   Card

   ---------------------------------
   |title                      icon|
   |-------------------------------|
   |                               |
   |       content                 |
   |                               |
   --------------------------------|
 */
typedef struct {
    lv_style_t* style;
    uint8_t* title;
    void* icon;
}card_desc_t;

lv_obj_t* ic_create_card(lv_obj_t* parent, card_desc_t* desc)
{
    lv_obj_t* card = lv_obj_create(parent, NULL);

    lv_obj_t* title = lv_label_create(card, NULL);

    lv_obj_add_style(card, LV_OBJ_PART_MAIN, desc->style);

    lv_obj_set_pos(card, 0, 0);
    lv_obj_set_size(card, LV_HOR_RES_MAX, TITLE_BAR_HEIGHT);

    lv_label_set_text(title, desc->title);
    lv_label_set_align(title, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_pos(title, TITLE_BAR_BORDER_WIDTH, 0);

    //icon

    return card;
}

static void infocard_screen_event_cb(lv_obj_t* obj, lv_event_t event)
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
    message.sender = g_card_center_ctx.card;

    //LOGI("infocard_screen_event_cb,obj:%x, event:%d\n", obj, event);

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
        offset.y = lv_obj_get_y(g_card_center_ctx.card_grp);


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

        if (direct == 1) {            
            if (offset.x > 0) offset.x = 0;

                message.msg = IC_OBJ_MSG_GRP_MOVE;
                message.data = (void *)&offset;
                lv_event_send(g_card_center_ctx.card->parent, LV_EVENT_VALUE_CHANGED, &message);          
        }

        if (direct > 1 && g_card_center_ctx.grp_height > LV_VER_RES_MAX) {
            int16_t grp_pos_y = lv_obj_get_y(g_card_center_ctx.card_grp) + offset_y;

            if (grp_pos_y > g_card_center_ctx.grp_start_y) grp_pos_y = g_card_center_ctx.grp_start_y;

            if (grp_pos_y < (g_card_center_ctx.grp_start_y - (g_card_center_ctx.grp_height - LV_VER_RES_MAX)))
                grp_pos_y = g_card_center_ctx.grp_start_y - (g_card_center_ctx.grp_height - LV_VER_RES_MAX);

            if (grp_pos_y > g_card_center_ctx.grp_start_y) grp_pos_y = g_card_center_ctx.grp_start_y;

            LOGI("gourp h: %d, start_y:%d, pos:%d\n", g_card_center_ctx.grp_height, g_card_center_ctx.grp_start_y, grp_pos_y);

            lv_obj_set_pos(g_card_center_ctx.card_grp, 0, grp_pos_y);
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


        if (direct == 1) {
            message.data = (void*)&offset;
            if ((lv_tick_get() - down_tick < MAINSCREEN_PULL_FAST_TIME)) {
                message.msg = IC_OBJ_MSG_GRP_ANIM_FAST;     
            }
            else {
                message.msg = IC_OBJ_MSG_GRP_ANIM_SLOW;
            }
            lv_event_send(g_card_center_ctx.card->parent, LV_EVENT_VALUE_CHANGED, &message);
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
 *  create, draw and update inofcard center
 *
 ****************************************************************************************************/
lv_obj_t* ic_infocard_create(lv_obj_t* parent, lv_style_t* style)
{
    if (!parent || !style) return NULL;

    lv_obj_t* card = lv_obj_create(parent, NULL);
    lv_obj_add_style(card, LV_OBJ_PART_MAIN, style);

    lv_obj_set_size(card, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_obj_set_pos(card, -LV_HOR_RES_MAX, 0);

    /**
     Draw infocard center
    */
    //title bar
    static lv_style_t title_style;
    lv_style_init(&title_style);
    lv_style_set_text_color(&title_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&title_style, LV_STATE_DEFAULT, ic_font_get(IC_FONT_SMALL));
    lv_style_set_bg_color(&title_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_width(&title_style, LV_STATE_DEFAULT, 0);

    screen_title_bar_desc_t title_desc;
    title_desc.title = _("Info Cards");
    title_desc.title_style = &title_style;
    ic_watch_create_title_bar(card, &title_desc);

    //card list
    lv_obj_t* container = lv_obj_create(card, NULL);
    lv_obj_set_pos(container, 0, TITLE_BAR_HEIGHT + 5);
    lv_obj_add_style(container, LV_OBJ_PART_MAIN, &title_style);

    lv_obj_t* card_grp = lv_obj_create(container, NULL);
    lv_obj_set_pos(card_grp, 0, 0);
    g_card_center_ctx.grp_start_y = 0;
    lv_obj_add_style(card_grp, LV_OBJ_PART_MAIN, &title_style);

    static lv_style_t card_style;
    lv_style_init(&card_style);
    lv_style_set_text_color(&card_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&card_style, LV_STATE_DEFAULT, ic_font_get(IC_FONT_SMALL));
    lv_style_set_bg_color(&card_style, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x20, 0x20, 0x20));
    lv_style_set_border_width(&card_style, LV_STATE_DEFAULT, 0);
    lv_style_set_radius(&card_style, LV_STATE_DEFAULT, 10);

    card_desc_t card_desc;
    card_desc.style = &card_style;
    card_desc.title = _("Alarm");
    
    lv_obj_t* alarm = ic_create_card(card_grp, &card_desc);
    
    lv_obj_set_pos(alarm, 0, 0);
    lv_obj_set_size(alarm, LV_HOR_RES_MAX, 100);
    g_card_center_ctx.grp_height += 105;

    card_desc_t sp_desc;
    sp_desc.style = &card_style;
    sp_desc.title = _("Sport");

    lv_obj_t* sport = ic_create_card(card_grp, &sp_desc);

    lv_obj_set_pos(sport, 0, g_card_center_ctx.grp_height);
    lv_obj_set_size(sport, LV_HOR_RES_MAX, 60);

    g_card_center_ctx.grp_height += 65;

    card_desc_t ex_desc1;
    ex_desc1.style = &card_style;
    ex_desc1.title = _("example1");

    lv_obj_t* example1 = ic_create_card(card_grp, &ex_desc1);

    lv_obj_set_pos(example1, 0, g_card_center_ctx.grp_height);
    lv_obj_set_size(example1, LV_HOR_RES_MAX, 200);

    g_card_center_ctx.grp_height += 205;

    card_desc_t ex_desc2;
    ex_desc2.style = &card_style;
    ex_desc2.title = _("example2");

    lv_obj_t* example2 = ic_create_card(card_grp, &ex_desc2);

    lv_obj_set_pos(example2, 0, g_card_center_ctx.grp_height);
    lv_obj_set_size(example2, LV_HOR_RES_MAX, 130);

    g_card_center_ctx.grp_height += 160;

    lv_obj_set_size(card_grp, LV_HOR_RES_MAX, g_card_center_ctx.grp_height);
    lv_obj_set_size(container, LV_HOR_RES_MAX, g_card_center_ctx.grp_height);

    lv_obj_set_event_cb(card, infocard_screen_event_cb);
    lv_obj_set_event_cb(card_grp, infocard_screen_event_cb);
    lv_obj_set_event_cb(example2, infocard_screen_event_cb);
    lv_obj_set_event_cb(example1, infocard_screen_event_cb);
    lv_obj_set_event_cb(sport, infocard_screen_event_cb);
    lv_obj_set_event_cb(alarm, infocard_screen_event_cb);

    g_card_center_ctx.card = card;
    g_card_center_ctx.card_grp = card_grp;   
    return card;
}

void ic_infocard_delete(lv_obj_t *obj)
{
}

void ic_infocard_update(lv_obj_t* obj)
{
}
