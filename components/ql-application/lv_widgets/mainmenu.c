/** 
* @Time    :   2020/12/24 11:19:25
* @FileName:   mainmenu.c
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
    lv_obj_t* mainmenu;

}mainmenu_context_t;

static mainmenu_context_t g_card_center_ctx;


static void mainmenu_screen_event_cb(lv_obj_t* obj, lv_event_t event)
{
    static lv_point_t down_pos;
    static lv_point_t last_pos;
    lv_point_t pos;
    static uint8_t x_flag = 0, y_flag = 0;
    static uint32_t down_tick = 0;
    static int8_t direct = -1;
    static int8_t direct_x = -1, direct_y = -1;
    static lv_point_t offset = {0};
    static ic_obj_message_t message;
    message.sender = g_card_center_ctx.mainmenu;

    LOGI("mainmenu_screen_event_cb,obj:%x, event:%d\n", obj, event);

    switch (event) 
	{
	    case LV_EVENT_PRESSED:
	        lv_indev_get_point(lv_indev_get_act(), &down_pos);
	        LOGI("pressed(%d, %d)\n", down_pos.x, down_pos.y);
	        last_pos = down_pos;
	        direct = -1;
	        down_tick = lv_tick_get();
	        direct_x = -1;
	        direct_y = -1;
	        offset.x = 0;
	        offset.y = lv_obj_get_y(g_card_center_ctx.mainmenu);
            break;
			
	    case LV_EVENT_PRESSING: 
		{
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

	        if (direct == -1) 
			{
	            if (abs(offset_x) > abs(offset_y) && abs(offset_x) > 5) 
				{
	                direct = 1;//右移
	            }
	            else if (abs(offset_y) > abs(offset_x)) 
				{
	                if (offset_y > 5)
					{
	                    direct = 2; //上移
	                }
					
	                if (offset_y < -5) 
					{
	                    direct = 3;//下移
	                }
	            }
	        }

	        last_pos = pos;

	        if (direct == 1) 
			{
	            if (offset.x < 0) offset.x = 0;

	            message.msg = IC_OBJ_MSG_GRP_MOVE;
	            message.data = (void*)&offset;
	            lv_event_send(g_card_center_ctx.mainmenu->parent, LV_EVENT_VALUE_CHANGED, &message);
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
			break;
			
	    case LV_EVENT_RELEASED: 
		{
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


	        if (direct == 1) 
			{
	            message.data = (void*)&offset;
	            if ((lv_tick_get() - down_tick < MAINSCREEN_PULL_FAST_TIME)) 
				{
	                message.msg = IC_OBJ_MSG_GRP_ANIM_FAST;
	            }
	            else
				{
	                message.msg = IC_OBJ_MSG_GRP_ANIM_SLOW;
	            }
	            lv_event_send(g_card_center_ctx.mainmenu->parent, LV_EVENT_VALUE_CHANGED, &message);
	        }
			else if(direct != 2 && direct != 3)//单点进入二级菜单
			{
				ic_select_phonebook_or_dial_create();
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
 *  create, draw and update mainmenu
 *
 ****************************************************************************************************/
/******************************************************************************
 *  Function    -  ic_mainmenu_create
 * 
 *  Purpose     -  进入电话簿
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-19,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
lv_obj_t* ic_mainmenu_create(lv_obj_t* parent, lv_style_t* style)
{
    if (!parent || !style) return NULL;

    lv_obj_t* mainmenu = lv_obj_create(parent, NULL);
	lv_obj_t* mainmenu_temper = NULL;//测量温度菜单
    lv_obj_add_style(mainmenu, LV_OBJ_PART_MAIN, style);

    lv_obj_set_size(mainmenu, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_obj_set_pos(mainmenu, -LV_HOR_RES_MAX, 0);

	mainmenu_temper = lv_img_create(mainmenu, NULL);
	lv_obj_set_click(mainmenu_temper, true);
	
	lv_obj_set_pos(mainmenu_temper,23,23);
	lv_img_set_src(mainmenu_temper, iclv_get_image_by_id(IMG_MAINMENU_TEMPERATURE_ID));



    /**
     Draw infocard senter
    */

#if 0
        /*Create a "8 bit alpha" canvas and clear it*/
        lv_obj_t* canvas = lv_canvas_create(lv_scr_act(), NULL);
        lv_canvas_set_buffer(canvas, current_screen_map, 240, 240, LV_IMG_CF_ALPHA_8BIT);
        lv_canvas_fill_bg(canvas, LV_COLOR_BLACK, LV_OPA_TRANSP);

        /*Draw an arc to the canvas. The result "image" will be used as mask*/
        lv_draw_line_dsc_t arc_dsc;
        lv_draw_line_dsc_init(&arc_dsc);
        arc_dsc.color = LV_COLOR_WHITE;
        arc_dsc.width = 10;
        lv_canvas_draw_arc(canvas, 240 / 2, 240 / 2, 240 / 2, 120, 240, &arc_dsc);
        lv_canvas_draw_arc(canvas, 250 / 2, 250 / 2, 240 / 2, 180, 360, &arc_dsc);

        //lv_draw_label_dsc_t lab_dsc;
        //lv_draw_label_init(&lab_dsc);
        //lab_dsc.color = LV_COLOR_WHITE;

        //lv_canvas_draw_text(canvas, 50, 140, 220, &lab_dsc, "This is a gradient Txts SHOW!!!!", LV_LABEL_ALIGN_CENTER);

        /*The mask is reads the canvas is not required anymore*/
        lv_obj_del(canvas);

        /*Create an object mask which will use the created mask*/
        lv_obj_t* om = lv_objmask_create(mainmenu, NULL);
        lv_obj_set_size(om, 240, 240);
        lv_obj_align(om, NULL, LV_ALIGN_CENTER, 0, 0);

        /*Add the created mask map to the object mask*/
        lv_draw_mask_map_param_t m;
        lv_area_t a;
        a.x1 = 0;
        a.y1 = 0;
        a.x2 = 240 - 1;
        a.y2 = 240 - 1;
        lv_draw_mask_map_init(&m, &a, current_screen_map);
        lv_objmask_add_mask(om, &m);

        /*Create a style with gradient*/
        static lv_style_t style_bg;
        lv_style_init(&style_bg);
        lv_style_set_bg_opa(&style_bg, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_bg_color(&style_bg, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00,0x66,0xed));
        lv_style_set_bg_grad_color(&style_bg, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xb5, 0x0e));
        lv_style_set_bg_grad_dir(&style_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_HOR);

        /* Create and object with the gradient style on the object mask.
         * The arc will be masked from the gradient*/
        lv_obj_t* bg = lv_obj_create(om, NULL);
        lv_obj_reset_style_list(bg, LV_OBJ_PART_MAIN);
        lv_obj_add_style(bg, LV_OBJ_PART_MAIN, &style_bg);
        lv_obj_set_size(bg, 240, 240);
#endif
  
	lv_obj_set_event_cb(mainmenu_temper, mainmenu_screen_event_cb);
    lv_obj_set_event_cb(mainmenu, mainmenu_screen_event_cb);

    g_card_center_ctx.mainmenu = mainmenu;
    return mainmenu;
}

void ic_mainmenu_delete(lv_obj_t *obj)
{
}

void ic_mainmenu_update(lv_obj_t* obj)
{
}
