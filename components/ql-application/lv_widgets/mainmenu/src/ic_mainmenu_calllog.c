/******************************************************************************
* @Time    :   2021/4/10 11:16:21
* @FileName:   ic_mainmenu_calllog.c
* @Author  :   xiongweiming
* @Software:   VSCode
* @Descripton: obj move animation implementation on LVGL  
******************************************************************************/


/******************************************************************************
 * include  head file
 * 
 *
 ******************************************************************************/
#include "ic_mainmenu_calllog.h"
#include "ic_call_phone.h"

/******************************************************************************
 * Local variable
 * 
 *
 ******************************************************************************/
static ic_call_phone_str call_information[200] = {0};//通话信息



/******************************************************************************
 * Local function
 * 
 *
 ******************************************************************************/
/******************************************************************************
 *  Function    -  missed_calllog_entry
 * 
 *  Purpose     -  未接电话界面进入
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void missed_calllog_entry(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;
    lv_scr_load((lv_obj_t*)screen->data);
}

/******************************************************************************
 *  Function    -  missed_calllog_exit
 * 
 *  Purpose     -  未接电话界面退出
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void missed_calllog_exit(void* arg)
{
    LOGI("missed calllog screen exit");
}

/******************************************************************************
 *  Function    -  missed_calllog_deinit
 * 
 *  Purpose     -  未接电话界面删除
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void missed_calllog_deinit(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    lv_obj_del((lv_obj_t*)screen->data);

}

/******************************************************************************
 *  Function    -  missed_calllog_init
 * 
 *  Purpose     -  未接电话界面创建
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void missed_calllog_init(void *arg)
{
	ic_screen_t* screen = (ic_screen_t*)arg;
	lv_obj_t* missed_calllog = lv_obj_create(NULL, NULL);
	static lv_style_t missed_calllog_bg;//must static type

	lv_style_init(&missed_calllog_bg);
	screen->data = missed_calllog;

	//添加窗口的风格
	lv_style_set_radius(&missed_calllog_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&missed_calllog_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&missed_calllog_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&missed_calllog_bg, LV_STATE_DEFAULT, 0);
	lv_obj_set_size(missed_calllog,LV_HOR_RES,LV_VER_RES);
	lv_obj_add_style(missed_calllog, LV_OBJ_PART_MAIN, &missed_calllog_bg);

	//添加列表
	lv_obj_t* missed_calllog_list = lv_list_create(missed_calllog, NULL);

	//添加列表的风格
	static lv_style_t calllog_information_type;
	lv_obj_set_size(missed_calllog_list, LV_HOR_RES, LV_VER_RES);
	lv_style_init(&calllog_information_type);
	lv_style_set_radius(&calllog_information_type, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&calllog_information_type, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_color(&calllog_information_type, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_dir(&calllog_information_type, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&calllog_information_type, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_border_width(&calllog_information_type, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&calllog_information_type, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_obj_add_style(missed_calllog_list, LV_OBJ_PART_MAIN, &calllog_information_type);
    lv_list_set_scrollbar_mode(missed_calllog_list, LV_SCROLLBAR_MODE_OFF);//不显示滚动条

	//添加按键
	lv_obj_t *screen_list0_btn;
	screen_list0_btn = lv_list_add_btn(missed_calllog_list, NULL, "ces");
	lv_obj_add_style(screen_list0_btn, LV_BTN_PART_MAIN, &calllog_information_type);

}


/******************************************************************************
 *  Function    -  calllog_missed_call_screen_create
 * 
 *  Purpose     -  未接电话窗口创建
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void calllog_missed_call_screen_create(void)
{
	screen_callback_t cb;

    cb.init = missed_calllog_init;
    cb.deinit = missed_calllog_deinit;
    cb.entry = missed_calllog_entry;
    cb.exit = missed_calllog_exit;


	if(!ic_screen_create(SCREEN_MISSED_CALLLOG, &cb)) 
	{
		LOGI("missed calllog screen fail");
		return;
	}

    if(!ic_screen_entry(SCREEN_MISSED_CALLLOG)) 
	{
        LOGI("enter missed calllog screen fail");
		return;
    }
}

/******************************************************************************
 *  Function    -  calllog_information_screen_create
 * 
 *  Purpose     -  进入未接电话或是已接电话等其它界面选择
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void calllog_information_screen_create(const char * menu)
{
	char *a = "2222";
	char *b = "3333";
	char *c = "4444";

	if(strcmp(a,menu) == 0)
	{
		calllog_missed_call_screen_create();
	}
		
}

/******************************************************************************
 *  Function    -  calllog_screen_event_cb
 * 
 *  Purpose     -  通话记录二级菜单触摸回调函数
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-13,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void calllog_screen_event_cb(lv_obj_t* obj, lv_event_t event)
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
			calllog_information_screen_create(lv_list_get_btn_text(obj));
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

static void ic_update_number_information()
{
	uint8_t call_total = 0;

	ic_get_number_information(call_information);
	call_total = ic_get_callog_total();
	
	for(int i = 0;i < call_total;i++)
	{
		LOGI("recent call[%d]: %d-%d-%d-%d-%d-%d,name = %s, number = %s,call_type = %d,call_type =%d,flag = %d\n",i,call_information[i].start.tm_year,
		call_information[i].start.tm_mon,call_information[i].start.tm_mday,call_information[i].start.tm_hour,call_information[i].start.tm_min,call_information[i].start.tm_sec,
		call_information[i].name,call_information[i].number,call_information[i].call_type,call_information[i].call_type2,call_information[i].flag);
	}
}

/******************************************************************************
 *  Function    -  recent_callog_init
 * 
 *  Purpose     -  最近通话界面
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-21,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void recent_callog_init(void *arg)
{
	ic_screen_t* screen = (ic_screen_t*)arg;

	//添加列表
	lv_obj_t* calllog_list = lv_list_create(NULL, NULL);
	screen->data = calllog_list;

	//更新最新的通话记录
	ic_update_number_information();
	
	//添加列表的风格
	static lv_style_t calllog_information_list;
	lv_obj_set_size(calllog_list, LV_HOR_RES, LV_VER_RES);
	lv_style_init(&calllog_information_list);
	lv_style_set_radius(&calllog_information_list, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&calllog_information_list, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_color(&calllog_information_list, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_dir(&calllog_information_list, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&calllog_information_list, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_border_width(&calllog_information_list, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&calllog_information_list, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_obj_add_style(calllog_list, LV_OBJ_PART_MAIN, &calllog_information_list);
    lv_list_set_scrollbar_mode(calllog_list, LV_SCROLLBAR_MODE_OFF);//不显示滚动条
	
	//添加按键的风格	
	static lv_style_t calllog_information_list_child;
	lv_style_init(&calllog_information_list_child);
	lv_style_set_radius(&calllog_information_list_child, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&calllog_information_list_child, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_color(&calllog_information_list_child, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_dir(&calllog_information_list_child, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&calllog_information_list_child, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_text_color(&calllog_information_list_child, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_border_width(&calllog_information_list_child, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&calllog_information_list_child, LV_STATE_DEFAULT, LV_COLOR_BLACK);//添加边界框
	
	//按下的状态
	lv_style_set_bg_opa(&calllog_information_list_child, LV_STATE_PRESSED, LV_OPA_COVER);	//设置背景透明度
	lv_style_set_bg_color(&calllog_information_list_child, LV_STATE_PRESSED, LV_COLOR_BLACK);	//设置背景颜色
	lv_style_set_bg_grad_color(&calllog_information_list_child, LV_STATE_PRESSED, LV_COLOR_BLACK);	//设置背景下半部分颜色
	lv_style_set_bg_grad_dir(&calllog_information_list_child, LV_STATE_PRESSED, LV_GRAD_DIR_VER);	//设置下半部分方向

	//按键下按选择框的颜色
	lv_style_set_outline_color(&calllog_information_list_child, LV_STATE_FOCUSED, LV_COLOR_BLACK);


	uint8_t count = ic_get_callog_total();
	
	for(uint8_t i = 0;i < count; i++)
	{
		//添加按键
		lv_obj_t * call_total = lv_list_add_btn(calllog_list, iclv_get_image_by_id(IMG_RECORD_MOTHER_ID), call_information[i].number);
		lv_obj_add_style(call_total, LV_BTN_PART_MAIN, &calllog_information_list_child);
		lv_obj_set_event_cb(call_total, calllog_screen_event_cb);

		//创建在按键上，然后静置布局，不然移不动
		lv_obj_t * out_or_in = lv_img_create(call_total,NULL);
		lv_btn_set_layout(call_total, LV_LAYOUT_OFF);
		lv_obj_align(out_or_in, NULL, LV_ALIGN_IN_TOP_RIGHT, 40, 30);


		if(call_information[i].call_type2 == 1)//拨出
		{
			lv_img_set_src(out_or_in, iclv_get_image_by_id(IMG_CALL_OUT_ID));
		}
		else if(call_information[i].call_type == 1)//未接
		{
			lv_img_set_src(out_or_in, iclv_get_image_by_id(IMG_CALL_MISS_ID));
		}
		else //打进已接
		{
			lv_img_set_src(out_or_in, iclv_get_image_by_id(IMG_CALL_IN_ID));
		}
	}

}

static void recent_callog_deinit(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    lv_obj_del((lv_obj_t*)screen->data);

}

static void recent_callog_exit(void* arg)
{
    LOGI("recent callog screen exit");
}

static void recent_callog_entry(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;
    lv_scr_load((lv_obj_t*)screen->data);
}

/******************************************************************************
 *  Function    -  ic_recent_callog_create
 * 
 *  Purpose     -  最近通话记录
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-21,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_recent_callog_create(void)
{
	screen_callback_t cb;

    cb.init = recent_callog_init;
    cb.deinit = recent_callog_deinit;
    cb.entry = recent_callog_entry;
    cb.exit = recent_callog_exit;


	if(!ic_screen_create(SCREEN_RECENT_CALLOG, &cb)) 
	{
		LOGI("create recent callog screen fail");
		return;
	}

    if(!ic_screen_entry(SCREEN_RECENT_CALLOG)) 
	{
        LOGI("enter recent callog screen fail");
		return;
    }
}
