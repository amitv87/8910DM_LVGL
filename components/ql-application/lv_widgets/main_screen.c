/** 
* @Time    :   2020/12/24 11:15:38
* @FileName:   main_screen.c
* @Author  :   Wang Hua(whfyzg@gmail.com)
* @Software:   VSCode
* @Descripton: watch clockface enterys
*/
#include "clockface/clockface.h"

#include "main_screen.h"
#include "ic_widgets_inc.h"



#ifdef __cplusplus
extern "C" {
#endif

//lv_color_t  current_screen_map[IC_CANVAS_WIDTH * IC_CANVAS_HEIGHT];


static mainscreen_obj_t mainscreen_obj;

#define MAINSCREEN_ANIM_INTERVAL   (15)    //ms

#define MAINSCREEN_ANIM_HOR_STEP   (LV_HOR_RES_MAX/5)
#define MAINSCREEN_ANIM_VER_STEP   (LV_VER_RES_MAX/10)
extern const clockface_t clock_table[];

static void clock_update_task(lv_task_t * task)
{
  /*Use the user_data*/
  clockface_obj_t * clock = (clockface_obj_t *)task->user_data;

  if (mainscreen_obj.focused_obj != mainscreen_obj.clock.bg) return;

  LOGI("clock_update_task\n");
  ic_clockface_update(clock);
}

static void pause_clock_task(void)
{
    lv_task_set_cb(mainscreen_obj.clockupdate_task, NULL);
}

static void resume_clock_task(void)
{
    lv_task_set_cb(mainscreen_obj.clockupdate_task, clock_update_task);
}

static void touch_animation_finish_cb(void) {
    mainscreen_obj.anim_para.is_moving = 0;
    resume_clock_task();
}
    
static void main_screen_event_cb(lv_obj_t* obj, lv_event_t event)
{
    static lv_point_t down_pos;
    static lv_point_t last_pos;
    lv_point_t pos;
    static uint32_t down_tick;
    static int8_t direct;
    static int8_t direct_x, direct_y;

    LOGI("main_screen_event_cb,obj:%x, event:%d\n",obj, event);

    switch (event){
    case LV_EVENT_PRESSED:
        lv_indev_get_point(lv_indev_get_act(), &down_pos);
        LOGI("pressed(%d, %d)\n", down_pos.x, down_pos.y);
        last_pos = down_pos;
        direct = -1;
        down_tick = lv_tick_get();
        direct_x = -1;
        direct_y = -1;

        mainscreen_obj.anim_para.offset.x = 0;
        mainscreen_obj.anim_para.offset.y = 0;

        break;
    case LV_EVENT_PRESSING: {
        lv_indev_get_point(lv_indev_get_act(), &pos);
        LOGI("pressing(%d, %d)\n", pos.x, pos.y);

        int16_t offset_x = pos.x - last_pos.x;
        int16_t offset_y = pos.y - last_pos.y;

        mainscreen_obj.anim_para.offset.x += offset_x;
        mainscreen_obj.anim_para.offset.y += offset_y;

        if (offset_x > 0) direct_x = 0;
        if (offset_x < 0) direct_x = 1;

        if (offset_y > 0) direct_y = 0;
        if (offset_y < 0) direct_y = 1;

        if (direct == -1) {
            if (abs(offset_x) > abs(offset_y) && abs(offset_x) > 5) {
                direct = 1;
                
                pause_clock_task();
            }
            else if (abs(offset_y) > abs(offset_x)){
                if (offset_y > 5) {
                    direct = 2;

                }
                if (offset_y < -5) {               
                    direct = 3;
                }
            }
        }

        last_pos = pos;   
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

        mainscreen_obj.anim_para.offset.x += offset_x;
        mainscreen_obj.anim_para.offset.y += offset_y;

        if (offset_x > 0) direct_x = 0;
        if (offset_x < 0) direct_x = 1;

        if (offset_y > 0) direct_y = 0;
        if (offset_y < 0) direct_y = 1;

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
    case LV_EVENT_VALUE_CHANGED: {
        ic_obj_message_t* message = (ic_obj_message_t *)lv_event_get_data();
        
        if (!message) break;

        lv_point_t* offset = (lv_point_t*)message->data;
        if (!offset) break;

        if (mainscreen_obj.anim_para.is_moving) break;

        pause_clock_task();

        if (message->sender == mainscreen_obj.clock.bg) 
		{
            obj_move_desc_t* desc = (obj_move_desc_t*)message->data;
            mainscreen_obj.anim_para.offset.y = desc->offset.y;
            mainscreen_obj.anim_para.offset.x = desc->offset.x;

            if (message->msg == IC_OBJ_MSG_GRP_MOVE) 
			{
                if (desc->direct == TOUCH_MOVE_DIRECT_LEFT || desc->direct == TOUCH_MOVE_DIRECT_RIGHT)
				{
                    mainscreen_obj.anim_para.offset.y = 0;
                    lv_obj_set_pos(mainscreen_obj.infocard, -LV_HOR_RES_MAX + desc->offset.x, 0);
                    lv_obj_set_pos(mainscreen_obj.clock.bg, desc->offset.x, 0);
                    lv_obj_set_pos(mainscreen_obj.mainmenu, LV_HOR_RES_MAX + desc->offset.x, 0);
                }
                else 
				{
                    mainscreen_obj.anim_para.offset.x = 0;
                    if (desc->direct == TOUCH_MOVE_DIRECT_DOWN) 
					{
                            if (mainscreen_obj.anim_para.offset.y > 0)
                                lv_obj_set_pos(mainscreen_obj.control_center, 0, -LV_VER_RES_MAX + mainscreen_obj.anim_para.offset.y);
                            else
                                lv_obj_set_pos(mainscreen_obj.control_center, 0, -LV_VER_RES_MAX);
                    }
                    else 
					{
                        if (mainscreen_obj.anim_para.offset.y < 0)
                            lv_obj_set_pos(mainscreen_obj.message_center, 0, LV_VER_RES_MAX + mainscreen_obj.anim_para.offset.y);
                        else
                            lv_obj_set_pos(mainscreen_obj.message_center, 0, LV_VER_RES_MAX);

                    }
                }
            }
            else 
			{
                if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST || message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) 
				{
                    if (desc->direct == TOUCH_MOVE_DIRECT_LEFT || desc->direct == TOUCH_MOVE_DIRECT_RIGHT) 
					{
                        mainscreen_obj.anim_para.objgrp.num = 3;
                        mainscreen_obj.anim_para.objgrp.objs[0].obj = mainscreen_obj.clock.bg;
                        mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.x = 0;
                        mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.y = 0;
                        mainscreen_obj.anim_para.objgrp.objs[1].obj = mainscreen_obj.infocard;
                        mainscreen_obj.anim_para.objgrp.objs[1].origin_pos.x = -LV_HOR_RES_MAX;
                        mainscreen_obj.anim_para.objgrp.objs[1].origin_pos.y = 0;
                        mainscreen_obj.anim_para.objgrp.objs[2].obj = mainscreen_obj.mainmenu;
                        mainscreen_obj.anim_para.objgrp.objs[2].origin_pos.x = LV_HOR_RES_MAX;
                        mainscreen_obj.anim_para.objgrp.objs[2].origin_pos.y = 0;

                        mainscreen_obj.anim_para.distance = LV_HOR_RES_MAX;

                        mainscreen_obj.anim_para.direct = desc->direct;

                        if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST) 
						{
                            mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                            mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;
                            mainscreen_obj.anim_para.offset.y = 0;
                            ic_start_move_objs(&mainscreen_obj.anim_para);
                        }
                        else if (message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) 
						{
                            if (abs(offset->x) > (LV_HOR_RES_MAX / 3)) 
							{
                                mainscreen_obj.anim_para.offset.y = 0;
                                mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                                mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;
                            }
                            else 
							{
                                mainscreen_obj.anim_para.offset.y = 0;
                                mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_BACKWARD;
                            }

                            ic_start_move_objs(&mainscreen_obj.anim_para);

                        }
                    }
                    else if (desc->direct == TOUCH_MOVE_DIRECT_UP || desc->direct == TOUCH_MOVE_DIRECT_DOWN) 
					{
                        mainscreen_obj.anim_para.direct = desc->direct;
                        mainscreen_obj.anim_para.objgrp.num = 1;
                        mainscreen_obj.anim_para.offset.x = 0;
                        mainscreen_obj.anim_para.offset.y = desc->offset.y;
                        if (desc->direct == TOUCH_MOVE_DIRECT_UP) 
						{
                            mainscreen_obj.anim_para.objgrp.objs[0].obj = mainscreen_obj.message_center;
                            mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.y = LV_VER_RES_MAX;
                        }
                        else 
						{
                            mainscreen_obj.anim_para.objgrp.objs[0].obj = mainscreen_obj.control_center;
                            mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.y = -LV_VER_RES_MAX;
                        }

                        mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.x = 0;

                        mainscreen_obj.anim_para.distance = LV_VER_RES_MAX;

                        //LOGI("direct:%d, offset(%d, %d)\n", desc->direct, mainscreen_obj.anim_para.offset.x, mainscreen_obj.anim_para.offset.y);

                        if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST) 
						{
                            mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                            mainscreen_obj.focused_obj = desc->direct == TOUCH_MOVE_DIRECT_UP ? mainscreen_obj.message_center : mainscreen_obj.control_center;
                            ic_start_move_objs(&mainscreen_obj.anim_para);
                        }
                        else if (message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) 
						{
                                if (abs(mainscreen_obj.anim_para.offset.y) > LV_VER_RES_MAX / 3) 
								{
                                    mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                                    mainscreen_obj.focused_obj = desc->direct == TOUCH_MOVE_DIRECT_UP? mainscreen_obj.message_center:mainscreen_obj.control_center;
                                }
                                else
								{
                                    mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_BACKWARD;
                                }

                                ic_start_move_objs(&mainscreen_obj.anim_para);
                        }

                    }
                }
            }
        }

        if (message->sender == mainscreen_obj.infocard) {

            if (message->msg == IC_OBJ_MSG_GRP_MOVE) {

                lv_obj_set_pos(mainscreen_obj.infocard, offset->x, 0);
                lv_obj_set_pos(mainscreen_obj.clock.bg, LV_HOR_RES_MAX + offset->x, 0);
                lv_obj_set_pos(mainscreen_obj.mainmenu, 2 * LV_HOR_RES_MAX + offset->x, 0);
            }
            else {
                if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST || message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) {
                    mainscreen_obj.anim_para.objgrp.num = 3;
                    mainscreen_obj.anim_para.objgrp.objs[0].obj = mainscreen_obj.clock.bg;
                    mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.x = LV_HOR_RES_MAX;
                    mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.y = 0;
                    mainscreen_obj.anim_para.objgrp.objs[1].obj = mainscreen_obj.infocard;
                    mainscreen_obj.anim_para.objgrp.objs[1].origin_pos.x = 0;
                    mainscreen_obj.anim_para.objgrp.objs[1].origin_pos.y = 0;
                    mainscreen_obj.anim_para.objgrp.objs[2].obj = mainscreen_obj.mainmenu;
                    mainscreen_obj.anim_para.objgrp.objs[2].origin_pos.x = 2 * LV_HOR_RES_MAX;
                    mainscreen_obj.anim_para.objgrp.objs[2].origin_pos.y = 0;

                    mainscreen_obj.anim_para.distance = LV_HOR_RES_MAX;

                    mainscreen_obj.anim_para.direct = TOUCH_MOVE_DIRECT_LEFT;

                    if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST) {
                        mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                        mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;
                        if (offset->x < 0) {
                            mainscreen_obj.anim_para.offset.x = offset->x;
                            mainscreen_obj.anim_para.offset.y = 0;
                            ic_start_move_objs(&mainscreen_obj.anim_para);
                        }
                    }
                    else if (message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) {
                        if (offset->x < -(LV_HOR_RES_MAX / 3)) {
                            mainscreen_obj.anim_para.offset.x = offset->x;
                            mainscreen_obj.anim_para.offset.y = 0;
                            mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                            mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;
                        }
                        else {
                            mainscreen_obj.anim_para.offset.x = offset->x;
                            mainscreen_obj.anim_para.offset.y = 0;
                            mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_BACKWARD;
                        }

                        ic_start_move_objs(&mainscreen_obj.anim_para);

                    }
                }
            }
        }

        if (message->sender == mainscreen_obj.mainmenu) {

            if (message->msg == IC_OBJ_MSG_GRP_MOVE) {

                lv_obj_set_pos(mainscreen_obj.infocard, - 2 * LV_HOR_RES_MAX + offset->x, 0);
                lv_obj_set_pos(mainscreen_obj.clock.bg, -LV_HOR_RES_MAX + offset->x, 0);
                lv_obj_set_pos(mainscreen_obj.mainmenu, offset->x, 0);
            }
            else {
                if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST || message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) {
                    mainscreen_obj.anim_para.objgrp.num = 3;
                    mainscreen_obj.anim_para.objgrp.objs[0].obj = mainscreen_obj.clock.bg;
                    mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.x = -LV_HOR_RES_MAX;
                    mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.y = 0;
                    mainscreen_obj.anim_para.objgrp.objs[1].obj = mainscreen_obj.infocard;
                    mainscreen_obj.anim_para.objgrp.objs[1].origin_pos.x = 22 * LV_HOR_RES_MAX;
                    mainscreen_obj.anim_para.objgrp.objs[1].origin_pos.y = 0;
                    mainscreen_obj.anim_para.objgrp.objs[2].obj = mainscreen_obj.mainmenu;
                    mainscreen_obj.anim_para.objgrp.objs[2].origin_pos.x = 0;
                    mainscreen_obj.anim_para.objgrp.objs[2].origin_pos.y = 0;

                    mainscreen_obj.anim_para.distance = LV_HOR_RES_MAX;

                    mainscreen_obj.anim_para.direct = TOUCH_MOVE_DIRECT_RIGHT;

                    if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST) {
                        mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                        mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;
                        if (offset->x > 0) {
                            mainscreen_obj.anim_para.offset.x = offset->x;
                            mainscreen_obj.anim_para.offset.y = 0;
                            ic_start_move_objs(&mainscreen_obj.anim_para);
                        }
                    }
                    else if (message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) {
                        if (offset->x > (LV_HOR_RES_MAX / 3)) {
                            mainscreen_obj.anim_para.offset.x = offset->x;
                            mainscreen_obj.anim_para.offset.y = 0;
                            mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                            mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;
                        }
                        else {
                            mainscreen_obj.anim_para.offset.x = offset->x;
                            mainscreen_obj.anim_para.offset.y = 0;
                            mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_BACKWARD;
                        }

                        ic_start_move_objs(&mainscreen_obj.anim_para);
                    }
                }
            }
        }

        if (message->sender == mainscreen_obj.control_center) {            
            if (message->msg == IC_OBJ_MSG_GRP_MOVE) {          
                lv_obj_set_pos(mainscreen_obj.control_center, 0, offset->y);
            }
            else if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST || message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) {

                mainscreen_obj.anim_para.objgrp.num = 1;
                mainscreen_obj.anim_para.direct = TOUCH_MOVE_DIRECT_UP;
                mainscreen_obj.anim_para.objgrp.objs[0].obj = mainscreen_obj.control_center;

                mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.x = 0;
                mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.y = 0;

                mainscreen_obj.anim_para.offset.y = offset->y;
                mainscreen_obj.anim_para.offset.x = 0;

                mainscreen_obj.anim_para.distance = LV_VER_RES_MAX;

                if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST) {
                    mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                    mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;
                    ic_start_move_objs(&mainscreen_obj.anim_para);
                }
                else if (message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) {
                    if (mainscreen_obj.anim_para.offset.y < -(LV_VER_RES_MAX / 3)) {
                        mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                        mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;
                    }
                    else {
                        mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_BACKWARD;

                    }
                    ic_start_move_objs(&mainscreen_obj.anim_para);
                }
            }
        }

        if (message->sender == mainscreen_obj.message_center) {
            if (message->msg == IC_OBJ_MSG_GRP_MOVE) {
                lv_obj_set_pos(mainscreen_obj.message_center, 0, offset->y);
            }
            else if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST || message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) {

                mainscreen_obj.anim_para.objgrp.num = 1;
                mainscreen_obj.anim_para.direct = TOUCH_MOVE_DIRECT_DOWN;
                mainscreen_obj.anim_para.objgrp.objs[0].obj = mainscreen_obj.message_center;

                mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.x = 0;
                mainscreen_obj.anim_para.objgrp.objs[0].origin_pos.y = 0;

                mainscreen_obj.anim_para.offset.y = offset->y;
                mainscreen_obj.anim_para.offset.x = 0;

                mainscreen_obj.anim_para.distance = LV_VER_RES_MAX;

                if (message->msg == IC_OBJ_MSG_GRP_ANIM_FAST) {
                    mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                    mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;
                    ic_start_move_objs(&mainscreen_obj.anim_para);
                }
                else if (message->msg == IC_OBJ_MSG_GRP_ANIM_SLOW) {
                    if (mainscreen_obj.anim_para.offset.y > (LV_VER_RES_MAX / 3)) {
                        mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_FORWARD;
                        mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;
                    }
                    else {
                        mainscreen_obj.anim_para.action = TOUCH_ANIM_ACTION_BACKWARD;

                    }
                    ic_start_move_objs(&mainscreen_obj.anim_para);
                }
            }
        }
           
    }
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



static void main_screen_init(void *arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    //style
    lv_style_init(&mainscreen_obj.style);
    lv_style_set_bg_color(&mainscreen_obj.style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_width(&mainscreen_obj.style, LV_STATE_DEFAULT, 0);

    mainscreen_obj.root = lv_obj_create(NULL, NULL);
    screen->data = mainscreen_obj.root;
  

    lv_obj_set_size(mainscreen_obj.root, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_obj_set_pos(mainscreen_obj.root, 0,0);
    //lv_obj_set_drag(main_screen_obj, true);
    lv_obj_add_style(mainscreen_obj.root, LV_OBJ_PART_MAIN, &mainscreen_obj.style);
    

    /**
    * clockface initialization
    * TODO: use a global clockface array to mantain all clockface
    */
	if (0)
	{
		mainscreen_obj.clock.desc = &clock_table[0];
	}
	else if(1)
	{
		mainscreen_obj.clock.desc = &clock_table[1];
	}
	else
	{
		mainscreen_obj.clock.desc = &clock_table[2];
	}
	
 
    ic_clockface_create(&mainscreen_obj.clock, mainscreen_obj.root);

    mainscreen_obj.clockupdate_task = lv_task_create(clock_update_task, CLOCK_UPDATE_INTERVAL, LV_TASK_PRIO_HIGHEST, &mainscreen_obj.clock);

    /**
      Create the quick control center, it`s a drawer style screen, show by pull down at clockface screen
    */
    mainscreen_obj.control_center = ic_control_center_create(mainscreen_obj.root, &mainscreen_obj.style);
    if (!mainscreen_obj.control_center) {
        LOGE("create control center fail\n");
    }

    /**
      Create the quick infomation card center, show by pull right
    */    
		mainscreen_obj.infocard = ic_infocard_create(mainscreen_obj.root, &mainscreen_obj.style);
		if (!mainscreen_obj.infocard) 
		{
			LOGE("create infocard center fail\n");
		}

    /**
      Create the message center, show by pull up
    */
    mainscreen_obj.message_center = ic_message_center_create(mainscreen_obj.root, &mainscreen_obj.style);
    if (!mainscreen_obj.message_center) {
        LOGE("create message center fail\n");
    }

    /**
     Create Mainmenu
     */
    mainscreen_obj.mainmenu = ic_mainmenu_create(mainscreen_obj.root, &mainscreen_obj.style);

    /**
      Event callback
    */
    lv_obj_set_event_cb(mainscreen_obj.root, main_screen_event_cb);


    mainscreen_obj.anim_para.h_step = MAINSCREEN_ANIM_HOR_STEP;
    mainscreen_obj.anim_para.v_step = MAINSCREEN_ANIM_VER_STEP;
    mainscreen_obj.anim_para.interval = MAINSCREEN_ANIM_INTERVAL;
    mainscreen_obj.anim_para.finish_cb = touch_animation_finish_cb;

    mainscreen_obj.focused_obj = mainscreen_obj.clock.bg;

}


static void main_screen_deinit(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    lv_obj_del((lv_obj_t*)screen->data);

    lv_task_del(mainscreen_obj.clockupdate_task);
}

static void main_screen_exit(void* arg)
{
    lv_task_set_cb(mainscreen_obj.clockupdate_task, NULL);
}

static void main_screen_entry(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;
    lv_scr_load((lv_obj_t*)screen->data);
    lv_task_set_cb(mainscreen_obj.clockupdate_task, clock_update_task);
}

void main_screen_1(void)
{
    screen_callback_t cb;

    cb.init = main_screen_init;
    cb.deinit = main_screen_deinit;
    cb.entry = main_screen_entry;
    cb.exit = main_screen_exit;


    if (!ic_screen_create(SCREEN_INDEX_IDLE, &cb)) {
        LOGI("create idle screen fail");
        return;
   }

    if (!ic_screen_entry(SCREEN_INDEX_IDLE)) {
        LOGI("enter idle screen fail");
    }
}

void main_screen(void) {
   	//nvram_main_init();

    //init png decode 
    //lv_png_init();

    //i18n init
    lv_i18n_init(lv_i18n_language_pack);

    //default langauge
    lv_i18n_set_locale("zh-CN");

    main_screen_1();
}
#ifdef __cplusplus
}
#endif
