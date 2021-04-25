/******************************************************************************
* @Time    :   2021/4/10 11:16:21
* @FileName:   ic_alarm_clock.c
* @Author  :   xiongweiming
* @Software:   VSCode
* @Descripton: obj move animation implementation on LVGL  
******************************************************************************/


/******************************************************************************
 * include  head file
 * 
 *
 ******************************************************************************/
#include "ic_alarm_clock.h"

static lv_obj_t * roller_hour;
static lv_obj_t * roller_min;
static ic_hal_rtc_t alarm_clock[] = {0};

/******************************************************************************
 * Local function
 * 
 *
 ******************************************************************************/
static void alarm_clock_fre_deinit(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    lv_obj_del((lv_obj_t*)screen->data);

}

static void alarm_clock_fre_exit(void* arg)
{
    LOGI("alarm clock screen exit");
}

static void alarm_clock_fre_entry(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;
    lv_scr_load((lv_obj_t*)screen->data);
}

static void cb_event_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) 
	{
        printf("State: %s\n", lv_checkbox_is_checked(obj) ? "Checked" : "Unchecked");
    }
}

/******************************************************************************
 *  Function    -  alarm_clock_init
 * 
 *  Purpose     -  通话记录二级菜单显示
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void alarm_clock_fre_init(void *arg)
{
	ic_screen_t* screen = (ic_screen_t*)arg;

	//添加page
	lv_obj_t * page1 = lv_page_create(NULL, NULL); /* 创建page控件 */
	screen->data = page1;

#if 0	
	static lv_style_t alarm_page_style;
	lv_style_init(&alarm_page_style);
	lv_obj_set_size(page1, LV_HOR_RES, LV_VER_RES);			 /* 设置大小 */
	lv_style_set_radius(&alarm_page_style, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&alarm_page_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&alarm_page_style, LV_STATE_DEFAULT, LV_COLOR_BLACK); //添加边界框
	lv_style_set_border_width(&alarm_page_style, LV_STATE_DEFAULT, 0);
  	lv_obj_add_style(page1, LV_OBJ_PART_MAIN, &alarm_page_style);

	lv_obj_t *cb0 = lv_checkbox_create(page1,NULL);

	lv_obj_set_style_local_value_align(page1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_OUT_LEFT_MID);
	lv_obj_set_style_local_value_ofs_x(page1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, -5);

#else	

	static lv_style_t alarm_page_style;
	lv_style_init(&alarm_page_style);	

	lv_style_set_radius(&alarm_page_style, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&alarm_page_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&alarm_page_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&alarm_page_style, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(page1, LV_OBJ_PART_MAIN, &alarm_page_style);


	lv_obj_set_size(page1, LV_HOR_RES, LV_VER_RES);			  /* 设置大小 */	
	lv_page_set_scrl_layout(page1, LV_LAYOUT_COLUMN_LEFT);
	lv_page_set_scrollable_fit(page1, LV_FIT_TIGHT);
	lv_page_set_scrlbar_mode(page1, LV_SCROLLBAR_MODE_OFF);

	//复选框
	lv_obj_t *cb0 = lv_checkbox_create(page1,NULL);
	lv_obj_t *cb1 = lv_checkbox_create(page1,NULL);
	lv_obj_t *cb2 = lv_checkbox_create(page1,NULL);
	lv_obj_t *cb3 = lv_checkbox_create(page1,NULL);
	lv_obj_t *cb4 = lv_checkbox_create(page1,NULL);
	lv_obj_t *cb5 = lv_checkbox_create(page1,NULL);
	lv_obj_t *cb6 = lv_checkbox_create(page1,NULL);



	static lv_style_t alarm_checkbox_style;
	lv_style_init(&alarm_checkbox_style);

	lv_style_set_radius(&alarm_checkbox_style, LV_STATE_DEFAULT, 30);
	lv_style_set_bg_color(&alarm_checkbox_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&alarm_checkbox_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&alarm_checkbox_style, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(cb0, LV_OBJ_PART_MAIN, &alarm_checkbox_style);
	lv_obj_add_style(cb1, LV_OBJ_PART_MAIN, &alarm_checkbox_style);
	lv_obj_add_style(cb2, LV_OBJ_PART_MAIN, &alarm_checkbox_style);
	lv_obj_add_style(cb3, LV_OBJ_PART_MAIN, &alarm_checkbox_style);
	lv_obj_add_style(cb4, LV_OBJ_PART_MAIN, &alarm_checkbox_style);
	lv_obj_add_style(cb5, LV_OBJ_PART_MAIN, &alarm_checkbox_style);
	lv_obj_add_style(cb6, LV_OBJ_PART_MAIN, &alarm_checkbox_style);
	lv_style_set_text_color(&alarm_checkbox_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);

	//按下的状态
	lv_style_set_bg_opa(&alarm_checkbox_style, LV_STATE_PRESSED, LV_OPA_COVER);	//设置背景透明度
	lv_style_set_bg_color(&alarm_checkbox_style, LV_STATE_CHECKED, LV_COLOR_BLACK);	//设置背景颜色
	lv_style_set_bg_grad_color(&alarm_checkbox_style, LV_STATE_PRESSED, LV_COLOR_BLACK);	//设置背景下半部分颜色
	lv_style_set_bg_grad_dir(&alarm_checkbox_style, LV_STATE_PRESSED, LV_GRAD_DIR_VER);	//设置下半部分方向
	lv_style_set_border_color(&alarm_checkbox_style, LV_STATE_FOCUSED, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&alarm_checkbox_style, LV_STATE_FOCUSED, 0);
	//按键下按选择框的颜色
	lv_style_set_outline_color(&alarm_checkbox_style, LV_STATE_FOCUSED, LV_COLOR_BLACK);

	lv_checkbox_set_text(cb0, "00000");
	lv_page_glue_obj(cb0,true);
	lv_obj_set_width(cb0, lv_page_get_width_fit(page1));//不能左右移动
	lv_obj_set_event_cb(cb0, cb_event_cb); /* 设置对象事件回调函数 */

	
	lv_checkbox_set_text(cb1, "11111");
	lv_page_glue_obj(cb1,true);
	lv_obj_set_width(cb1, lv_page_get_width_fit(page1));//不能左右移动

		
	lv_checkbox_set_text(cb2, "22222");
	lv_page_glue_obj(cb2,true);
	lv_obj_set_width(cb2, lv_page_get_width_fit(page1));//不能左右移动

	lv_checkbox_set_text(cb3, "33333");
	lv_page_glue_obj(cb3,true);
	lv_obj_set_width(cb3, lv_page_get_width_fit(page1));//不能左右移动

	lv_checkbox_set_text(cb4, "4444");
	lv_page_glue_obj(cb4,true);
	lv_obj_set_width(cb4, lv_page_get_width_fit(page1));//不能左右移动

	lv_checkbox_set_text(cb5, "5555");
	lv_page_glue_obj(cb5,true);
	lv_obj_set_width(cb5, lv_page_get_width_fit(page1));//不能左右移动

	lv_checkbox_set_text(cb6, "6666");
	lv_page_glue_obj(cb6,true);
	lv_obj_set_width(cb6, lv_page_get_width_fit(page1));//不能左右移动
#endif

}


static void alarm_clock_fre_create(void)
{
	screen_callback_t cb;

    cb.init = alarm_clock_fre_init;
    cb.deinit = alarm_clock_fre_deinit;
    cb.entry = alarm_clock_fre_entry;
    cb.exit = alarm_clock_fre_exit;


	if(!ic_screen_create(SCREEN_ALARM_CLOCK_FRE, &cb)) 
	{
		LOGI("create alarm clock screen fail");
		return;
	}

    if(!ic_screen_entry(SCREEN_ALARM_CLOCK_FRE)) 
	{
        LOGI("enter alarm clock screen fail");
		return;
    }
}


static void alarm_clock_add_exit(void* arg)
{
    LOGI("alarm clock screen exit");
}

static void alarm_clock_add_entry(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;
    lv_scr_load((lv_obj_t*)screen->data);
}


static void alarm_clock_add_deinit(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    lv_obj_del((lv_obj_t*)screen->data);

}

/******************************************************************************
 *  Function    -  roller_event_cb
 * 
 *  Purpose     -  点击确定的回调
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void roller_event_cb(lv_obj_t * obj, lv_event_t event)
{
	if(event == LV_EVENT_CLICKED) 
	{
		ic_hal_rtc_t rtc;
		ic_hal_rtc_get_time(&rtc); 
	
		rtc.hour = (uint8_t)lv_roller_get_selected(roller_hour);
		rtc.min = (uint8_t)lv_roller_get_selected(roller_min);
		
#if defined(PLATFORM_EC600)
	ql_rtc_print_time(rtc);
	//ql_rtc_set_alarm(&rtc);
#else
	LOGI("time \n");
#endif	
	//alarm_clock_fre_create();
	}
} 

/******************************************************************************
 *  Function    -  alarm_clock_add_init
 * 
 *  Purpose     -  添加闹钟界面
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void alarm_clock_add_init(void *arg)
{
	ic_screen_t* screen = (ic_screen_t*)arg;
	lv_obj_t* alarm_clock_add = lv_obj_create(NULL, NULL);
	static lv_style_t alarm_clock_add_bg;//must static type
	lv_style_init(&alarm_clock_add_bg);
	screen->data = alarm_clock_add;
	uint8_t i = 0;
	uint8_t buf[100] = {0};
	uint8_t min[200] = {0};
	uint8_t hour[100] = {0};



	//添加窗口的风格
	lv_style_set_radius(&alarm_clock_add_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&alarm_clock_add_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&alarm_clock_add_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&alarm_clock_add_bg, LV_STATE_DEFAULT, 0);
	
	lv_obj_set_size(alarm_clock_add,LV_HOR_RES,LV_VER_RES);
	lv_obj_add_style(alarm_clock_add, LV_OBJ_PART_MAIN, &alarm_clock_add_bg);

	//滚筒的风格
	static lv_style_t alarm_clock_roller_style;
	lv_style_init(&alarm_clock_roller_style);
	roller_hour = lv_roller_create(alarm_clock_add, NULL);
	roller_min = lv_roller_create(alarm_clock_add, NULL);
	lv_style_set_bg_color(&alarm_clock_roller_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&alarm_clock_roller_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&alarm_clock_roller_style, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(roller_hour, LV_OBJ_PART_MAIN, &alarm_clock_roller_style);
	lv_obj_add_style(roller_min, LV_OBJ_PART_MAIN, &alarm_clock_roller_style);
	
	for (i = 0; i <= 23; i++) 
	{
		if (i < 23)
			sprintf(buf, "%d\n", i);
		else
			sprintf(buf, "%d", i);
		strcat(hour, buf);
	}

	for (i = 0; i <= 59; i++) 
	{
		if (i < 59)
			sprintf(buf, "%d\n", i);
		else
			sprintf(buf, "%d", i);
		strcat(min, buf);
	}

	//小时的滚筒
	lv_obj_set_pos(roller_hour, 13, 5);
	lv_roller_set_options(roller_hour, hour, LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_fix_width(roller_hour, 40);
	lv_roller_set_visible_row_count(roller_hour, 2);//滚动显示行数
	//lv_obj_set_event_cb(roller_hour, roller_event_cb);
	lv_roller_set_selected(roller_hour, 20, true);//设置选择的选项

	//分钟的滚筒
	lv_obj_set_pos(roller_min, 70, 5);
	lv_roller_set_options(roller_min, min, LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_fix_width(roller_min, 40);
	lv_roller_set_visible_row_count(roller_min, 2);
	//lv_obj_set_event_cb(roller_min, roller_event_cb);
	lv_roller_set_selected(roller_min, 4, true);

	//冒号的显示
	lv_obj_t *label1 = lv_label_create(alarm_clock_add, NULL);
	lv_obj_set_pos(label1,59,34);
	lv_label_set_recolor(label1, true);
	lv_label_set_text(label1, "#FFFFFF : #");


	//按键的显示
	lv_obj_t *ok_btn = lv_btn_create(alarm_clock_add, NULL);
	lv_obj_t *label_ok_btn = lv_label_create(ok_btn, NULL);
	static lv_style_t alarm_clock_btn_style;
	lv_style_init(&alarm_clock_btn_style);

	lv_style_set_bg_color(&alarm_clock_btn_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&alarm_clock_btn_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&alarm_clock_btn_style, LV_STATE_DEFAULT, 0);
	lv_obj_set_size(ok_btn,80,30);
	lv_obj_set_pos(ok_btn,22,90);
	lv_label_set_text(label_ok_btn, "box");
	lv_obj_set_event_cb(ok_btn,roller_event_cb);
} 


/******************************************************************************
 *  Function    -  alarm_clock_add_screen
 * 
 *  Purpose     -  添加闹钟入口
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void alarm_clock_add_screen(void)
{
	screen_callback_t cb;
    cb.init = alarm_clock_add_init;
    cb.deinit = alarm_clock_add_deinit;
    cb.entry = alarm_clock_add_entry;
    cb.exit = alarm_clock_add_exit;


	if(!ic_screen_create(SCREEN_ALARM_CLOCK_ADD, &cb)) 
	{
		LOGI("create alarm clock screen fail");
		return;
	}

    if(!ic_screen_entry(SCREEN_ALARM_CLOCK_ADD)) 
	{
        LOGI("enter alarm clock screen fail");
		return;
    }
}

/******************************************************************************
 *  Function    -  alarm_clock_screen_event_cb
 * 
 *  Purpose     -  添加闹钟的回调函数
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void alarm_clock_screen_event_cb(lv_obj_t* obj, lv_event_t event)
{
    static lv_point_t down_pos;
    static lv_point_t last_pos;
    lv_point_t pos;
    static uint8_t x_flag = 0, y_flag = 0;
    static uint32_t down_tick = 0;
    static int8_t direct = -1;
    static int8_t direct_x = -1, direct_y = -1;
    static lv_point_t offset;


    //LOGI("infocontroller_screen_event_cb,obj:%x, event:%d\n", obj, event);

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
	        offset.y = 0;

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
					direct = 1;
				}
				else if (abs(offset_y) > abs(offset_x)) 
				{
					if (offset_y > 5) 
					{
						direct = 2;
					}
					
					if (offset_y < -5) 
					{
						direct = 3;
					}
				}
			}

	        last_pos = pos;

	        break;


	    case LV_EVENT_SHORT_CLICKED:
	        LOGI("short click\n");
	        break;

	    case LV_EVENT_CLICKED:
	        LOGI("clicked\n");
			alarm_clock_add_screen();
	        break;

	    case LV_EVENT_LONG_PRESSED:
	        LOGI("LONG pressed\n");
	        break;

	    case LV_EVENT_LONG_PRESSED_REPEAT:
	        LOGI("long pressed repeat\n");
	        break;

	    case LV_EVENT_PRESS_LOST:
	        LOGI("press lost\n");
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

#if 0
	        if (direct == 3) 
			{
	            message.data = (void*)&offset;
	            if ((lv_tick_get() - down_tick < MAINSCREEN_PULL_FAST_TIME)) {
	                message.msg = IC_OBJ_MSG_GRP_ANIM_FAST;
	            }
	            else {
	                message.msg = IC_OBJ_MSG_GRP_ANIM_SLOW;
	            }
	            lv_event_send(g_controller_ctx.controller->parent, LV_EVENT_VALUE_CHANGED, &message);
				
	        }
#endif			
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


/******************************************************************************
 *  Function    -  alarm_clock_init
 * 
 *  Purpose     -  通话记录二级菜单显示
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void alarm_clock_init(void *arg)
{
	ic_screen_t* screen = (ic_screen_t*)arg;
	lv_obj_t* alarm_clock = lv_obj_create(NULL, NULL);
	static lv_style_t alarm_clock_bg;//must static type
	lv_style_init(&alarm_clock_bg);
	screen->data = alarm_clock;

	//添加窗口的风格
	lv_style_set_radius(&alarm_clock_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&alarm_clock_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&alarm_clock_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&alarm_clock_bg, LV_STATE_DEFAULT, 0);
	lv_obj_set_size(alarm_clock,LV_HOR_RES,LV_VER_RES);
	lv_obj_add_style(alarm_clock, LV_OBJ_PART_MAIN, &alarm_clock_bg);

	//显示字符串
	static lv_style_t alarm_clock_style;
	lv_style_init(&alarm_clock_style);
	lv_obj_t *add_alarm_num = lv_textarea_create(alarm_clock, NULL);
	
	lv_obj_set_size(add_alarm_num,LV_HOR_RES,LV_VER_RES);
	lv_obj_align(add_alarm_num, NULL, LV_ALIGN_CENTER, 0, 40);
	lv_style_set_bg_color(&alarm_clock_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&alarm_clock_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&alarm_clock_style, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&alarm_clock_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_text_font(&alarm_clock_style, LV_STATE_DEFAULT, &lv_font_montserrat_14);
	lv_obj_add_style(add_alarm_num, LV_OBJ_PART_MAIN, &alarm_clock_style);
	lv_obj_set_pos(add_alarm_num,8,40);

	lv_textarea_set_text(add_alarm_num, "ADD ALMARM             +");
	

	lv_obj_set_event_cb(add_alarm_num, alarm_clock_screen_event_cb);

}

/******************************************************************************
 *  Function    -  alarm_clock_deinit
 * 
 *  Purpose     -  删除通话记录菜单
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void alarm_clock_deinit(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    lv_obj_del((lv_obj_t*)screen->data);

}

/******************************************************************************
 *  Function    -  alarm_clock_exit
 * 
 *  Purpose     -  退出通话记录菜单
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void alarm_clock_exit(void* arg)
{
    LOGI("alarm clock screen exit");
}

/******************************************************************************
 *  Function    -  alarm_clock_entry
 * 
 *  Purpose     -  进入通话记录菜单 
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void alarm_clock_entry(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;
    lv_scr_load((lv_obj_t*)screen->data);
}

/******************************************************************************
 *  Function    -  ic_alarm_clock_create
 * 
 *  Purpose     -  创建闹钟
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_alarm_clock_create(void)
{
	screen_callback_t cb;

    cb.init = alarm_clock_init;
    cb.deinit = alarm_clock_deinit;
    cb.entry = alarm_clock_entry;
    cb.exit = alarm_clock_exit;


	if(!ic_screen_create(SCREEN_ALARM_CLOCK, &cb)) 
	{
		LOGI("create alarm clock screen fail");
		return;
	}

    if(!ic_screen_entry(SCREEN_ALARM_CLOCK)) 
	{
        LOGI("enter alarm clock screen fail");
		return;
    }
}
