/******************************************************************************
* @Time    :   2021/4/13 11:50:21
* @FileName:   ic_mainmenu_phonebook.c
* @Author  :   xiongweiming
* @Software:   VSCode
* @Descripton: obj move animation implementation on LVGL  
******************************************************************************/


/******************************************************************************
 * include  head file
 * 
 *
 ******************************************************************************/
#include "ic_mainmenu_phonebook.h"

/******************************************************************************
 * local variable
 * 
 *
 ******************************************************************************/
static lv_obj_t* select1 = NULL;
static lv_obj_t* select2 = NULL;

/******************************************************************************
 * external function
 * 
 *
 ******************************************************************************/
void ic_entry_pb_list_screen(void);

/******************************************************************************
 * Local function
 * 
 *
 ******************************************************************************/

/******************************************************************************
 *  Function    -  pb_screen_event_cb
 * 
 *  Purpose     -  通讯录的回调函数
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-21,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void pb_screen_event_cb(lv_obj_t* obj, lv_event_t event)
{
    static lv_point_t down_pos;
    static lv_point_t last_pos;
    lv_point_t pos;
    static lv_point_t offset = {0};
	
    //LOGI("infocontroller_screen_event_cb,obj:%x, event:%d\n", obj, event);

    switch (event) 
	{
	    case LV_EVENT_PRESSED:
	        lv_indev_get_point(lv_indev_get_act(), &down_pos);
	        LOGI("pressed(%d, %d)\n", down_pos.x, down_pos.y);
	        offset.x = 0;
	        offset.y = 0;
	        break;
			
	    case LV_EVENT_PRESSING: 
	        LOGI("pressing\n");
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
			lv_indev_get_point(lv_indev_get_act(), &pos);
	        LOGI("release(%d, %d)\n", pos.x, pos.y);

	        offset.x = pos.x - down_pos.x;
	        offset.y = pos.y - down_pos.y;

			if(abs(offset.x) > abs(offset.y) && offset.x > 20)//右移
			{
				//最近通话记录
				ic_recent_callog_create();
			}
			else if(abs(offset.x) > abs(offset.y) && offset.x < -20)//左移
			{
				//未接电话
			}
			else if(abs(offset.y) > abs(offset.x) && offset.y > 20)//上移
			{
				//退出
				//ic_screen_goback();
			}
			else if(obj == select1)
			{
				ic_entry_dial_screen();
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

static void pb_list_init(void *arg)
{
	ic_screen_t* screen = (ic_screen_t*)arg;
	lv_obj_t* pb_list = lv_list_create(NULL, NULL);
	static lv_style_t pb_list_bg;//must static type
	lv_style_init(&pb_list_bg);
	screen->data = pb_list;

	//添加窗口的风格
	lv_obj_set_size(pb_list,LV_HOR_RES,LV_VER_RES);
	lv_style_set_radius(&pb_list_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&pb_list_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&pb_list_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&pb_list_bg, LV_STATE_DEFAULT, 0);
	lv_list_set_scrollbar_mode(pb_list, LV_SCROLLBAR_MODE_OFF);//不显示滚动条
	lv_obj_add_style(pb_list, LV_OBJ_PART_MAIN, &pb_list_bg);

	//按键
	static lv_style_t pb_list_style;
	lv_style_init(&pb_list_style);
	lv_style_set_radius(&pb_list_style, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&pb_list_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_color(&pb_list_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_dir(&pb_list_style, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&pb_list_style, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_text_color(&pb_list_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_border_width(&pb_list_style, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&pb_list_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);//添加边界框


	//按下的状态
	lv_style_set_bg_opa(&pb_list_style, LV_STATE_PRESSED, LV_OPA_COVER);	//设置背景透明度
	lv_style_set_bg_color(&pb_list_style, LV_STATE_PRESSED, LV_COLOR_BLACK);	//设置背景颜色
	lv_style_set_bg_grad_color(&pb_list_style, LV_STATE_PRESSED, LV_COLOR_BLACK);	//设置背景下半部分颜色
	lv_style_set_bg_grad_dir(&pb_list_style, LV_STATE_PRESSED, LV_GRAD_DIR_VER);	//设置下半部分方向

	//按键下按选择框的颜色
	lv_style_set_outline_color(&pb_list_style, LV_STATE_FOCUSED, LV_COLOR_BLACK);

	//添加列表成员
	lv_obj_t* mother_btn = lv_list_add_btn(pb_list, iclv_get_image_by_id(IMG_RECORD_MOTHER_ID), "mother");
	lv_obj_add_style(mother_btn, LV_BTN_PART_MAIN, &pb_list_style);
	lv_obj_set_event_cb(mother_btn, pb_screen_event_cb);

	lv_obj_t* father_btn = lv_list_add_btn(pb_list, iclv_get_image_by_id(IMG_RECORD_FATHER_ID), "father");
	lv_obj_add_style(father_btn, LV_BTN_PART_MAIN, &pb_list_style);
	lv_obj_set_event_cb(father_btn, pb_screen_event_cb);
	
	lv_obj_t* teacher_btn = lv_list_add_btn(pb_list, iclv_get_image_by_id(IMG_RECORD_TEACHER_ID), "teacher");
	lv_obj_add_style(teacher_btn, LV_BTN_PART_MAIN, &pb_list_style);
	lv_obj_set_event_cb(teacher_btn, pb_screen_event_cb);
	
	lv_obj_t* grandmother_btn = lv_list_add_btn(pb_list, iclv_get_image_by_id(IMG_RECORD_GRANDMOTHER_ID), "grandmother");
	lv_obj_add_style(grandmother_btn, LV_BTN_PART_MAIN, &pb_list_style);
	lv_obj_set_event_cb(grandmother_btn, pb_screen_event_cb);

	lv_obj_t* grandfather_btn = lv_list_add_btn(pb_list, iclv_get_image_by_id(IMG_RECORD_GRANDFATHER_ID), "grandfather");
	lv_obj_add_style(grandfather_btn, LV_BTN_PART_MAIN, &pb_list_style);
	lv_obj_set_event_cb(grandfather_btn, pb_screen_event_cb);
	
}

static void pb_list_deinit(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    lv_obj_del((lv_obj_t*)screen->data);

}

static void pb_list_exit(void* arg)
{
    LOGI("pb list screen exit");
}

static void pb_list_entry(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;
    lv_scr_load((lv_obj_t*)screen->data);
}

/******************************************************************************
 *  Function    -  ic_entry_pb_list_screen
 * 
 *  Purpose     -  通讯录界面入口
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-21,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_entry_pb_list_screen(void)
{
	screen_callback_t cb;

    cb.init = pb_list_init;
    cb.deinit = pb_list_deinit;
    cb.entry = pb_list_entry;
    cb.exit = pb_list_exit;


	if(!ic_screen_create(SCREEN_PB_LIST, &cb)) 
	{
		LOGI("create pb list screen fail");
		return;
	}

    if(!ic_screen_entry(SCREEN_PB_LIST)) 
	{
        LOGI("enter pb list screen fail");
		return;
    }
}

/******************************************************************************
 *  Function    -  phonebook_or_dial_event_cb
 * 
 *  Purpose     -  选择通讯录或是拨号界面的回调
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-21,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void phonebook_or_dial_event_cb(lv_obj_t* obj, lv_event_t event)
{
    static lv_point_t down_pos;
    static lv_point_t last_pos;
    lv_point_t pos;
    static lv_point_t offset = {0};

    //LOGI("mainmenu_screen_event_cb,obj:%x, event:%d\n", obj, event);

    switch (event) 
	{
	    case LV_EVENT_PRESSED:
	        lv_indev_get_point(lv_indev_get_act(), &down_pos);
	        LOGI("pressed(%d, %d)\n", down_pos.x, down_pos.y);
	        offset.x = 0;
	        offset.y = 0;
            break;
			
	    case LV_EVENT_PRESSING:
	        LOGI("pressing\n");
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
	        lv_indev_get_point(lv_indev_get_act(), &pos);
	        LOGI("release(%d, %d)\n", pos.x, pos.y);

	        offset.x = pos.x - down_pos.x;
	        offset.y = pos.y - down_pos.y;

			if(abs(offset.x) > abs(offset.y) && offset.x > 20)
			{
				ic_screen_goback();
			}
			else if(obj == select1)
			{
				ic_entry_dial_screen();
			}
			else if(obj == select2)
			{
				ic_entry_pb_list_screen();
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

/******************************************************************************
 *  Function    -  phonebook_or_dial_init
 * 
 *  Purpose     -  选择进入电话簿或是拨号
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-19,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void phonebook_or_dial_init(void *arg)
{
	ic_screen_t* screen = (ic_screen_t*)arg;
	lv_obj_t* phonebook_or_dial = lv_obj_create(NULL, NULL);
	static lv_style_t phonebook_or_dial_bg;//must static type
	lv_style_init(&phonebook_or_dial_bg);
	screen->data = phonebook_or_dial;

	//添加窗口的风格
	lv_style_set_radius(&phonebook_or_dial_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&phonebook_or_dial_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&phonebook_or_dial_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&phonebook_or_dial_bg, LV_STATE_DEFAULT, 0);
	lv_obj_set_size(phonebook_or_dial,LV_HOR_RES,LV_VER_RES);
	lv_obj_add_style(phonebook_or_dial, LV_OBJ_PART_MAIN, &phonebook_or_dial_bg);

	select1 = lv_img_create(phonebook_or_dial, NULL);
	lv_obj_t* dial = lv_img_create(phonebook_or_dial, NULL);
	lv_obj_t *label1 = lv_label_create(select1, NULL);
	
	select2 = lv_img_create(phonebook_or_dial, NULL);
	lv_obj_t* phonebook = lv_img_create(phonebook_or_dial, NULL);
	lv_obj_t *label2 = lv_label_create(phonebook_or_dial, NULL);
	
	//拨号
	lv_obj_set_pos(select1, 10,45);
	lv_img_set_src(select1, iclv_get_image_by_id(IMG_PHONEBOOK_SELECT1_ID));
	lv_obj_set_pos(dial, 74,64);
	lv_img_set_src(dial, iclv_get_image_by_id(IMG_PHONEBOOK_DIAL_ID));
	lv_label_set_text(label1, "btn1");
	lv_obj_align(label1, dial, LV_ALIGN_IN_LEFT_MID, 30, 0);/* 设置位置 */
	lv_obj_set_click(select1, true);

	//电话簿
	lv_obj_set_pos(select2, 10,135);
	lv_img_set_src(select2, iclv_get_image_by_id(IMG_PHONEBOOK_SELECT2_ID));
	lv_obj_set_pos(phonebook, 74,154);
	lv_img_set_src(phonebook, iclv_get_image_by_id(IMG_PHONEBOOK_PB_ID));
	lv_label_set_text(label2, "btn2");
	lv_obj_align(label2, phonebook, LV_ALIGN_IN_LEFT_MID, 30, 0);/* 设置位置 */
	lv_obj_set_click(select2, true);

	lv_obj_set_event_cb(select1, phonebook_or_dial_event_cb);
	lv_obj_set_event_cb(select2, phonebook_or_dial_event_cb);
	lv_obj_set_event_cb(phonebook_or_dial,phonebook_or_dial_event_cb);
}

static void phonebook_or_dial_deinit(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    lv_obj_del((lv_obj_t*)screen->data);

}

static void phonebook_or_dial_exit(void* arg)
{
    LOGI("phonebook or dial screen exit");
}

static void phonebook_or_dial_entry(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;
    lv_scr_load((lv_obj_t*)screen->data);
}

void ic_select_phonebook_or_dial_create(void)
{
	screen_callback_t cb;

    cb.init = phonebook_or_dial_init;
    cb.deinit = phonebook_or_dial_deinit;
    cb.entry = phonebook_or_dial_entry;
    cb.exit = phonebook_or_dial_exit;


	if(!ic_screen_create(SCREEN_SELECT_PHOBEBOOK_OR_DIAL, &cb)) 
	{
		LOGI("create calllog information screen fail");
		return;
	}

    if(!ic_screen_entry(SCREEN_SELECT_PHOBEBOOK_OR_DIAL)) 
	{
        LOGI("enter calllog information screen fail");
		return;
    }
}

