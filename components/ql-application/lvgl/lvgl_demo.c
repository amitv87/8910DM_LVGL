/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------
2020/12/14      lambert.zhao    Init version
=================================================================*/

/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_log.h"
#include "ql_gpio.h"
#include "ql_lcd.h"

#include "lvgl_demo.h"
#include "lvgl.h"
#include "ql_keypad.h"
#include "ql_app_feature_config.h"
#include "ql_api_voice_call.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_LVGLDEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_LVGLDEMO_LOG(msg, ...)         QL_LOG(QL_LVGLDEMO_LOG_LEVEL, "ql_LVGLDEMO", msg, ##__VA_ARGS__)
#define QL_LVGLDEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("ql_LVGLDEMO", msg, ##__VA_ARGS__)
#define QL_Lvgl_TASK_STACK_SIZE  1024*4
#define QL_Lvgl_TASK_PRIO        APP_PRIORITY_NORMAL
#define QL_LVGL_TASK_EVENT_CNT   5
#define FALSE    0
#define TRUE     1
#if !defined(require_action)
	#define require_action(x, action, str)																		\
             do                                                                                                  \
             {                                                                                                   \
                 if(x != 0)                                                                                      \
                 {                                                                                               \
                     QL_LVGLDEMO_LOG(str);                                                                      \
                     {action;}                                                                                   \
                 }                                                                                               \
             } while( 1==0 )
#endif

/*===========================================================================
* struct
===========================================================================*/
typedef struct
{
    bool screen_on;                 // state of screen on
    bool keypad_pending;            // keypad pending, set in ISR, clear in thread
    bool anim_inactive;             // property of whether animation is regarded as inactive
    ql_task_t thread;               // gui thread
    ql_timer_t task_timer;          // timer to trigger task handler
    lv_disp_buf_t disp_buf;         // display buffer
    lv_disp_t *disp;                // display device    
    lv_indev_t *keypad;             // keypad device
    ql_keymap_e last_key;           // last key from ISR
    ql_keystate_e last_key_state;   // last key state from ISR    
    uint32_t screen_on_users;       // screen on user bitmap
    uint32_t inactive_timeout;      // property of inactive timeout
} lvglContext_t;

typedef struct
{
 uint8_t keypad;
 uint8_t lv_key;
} lvGuiKeypadMap_t;


uint32_t g_lcd_width = CONFIG_LV_GUI_HOR_RES;
uint32_t g_lcd_height = CONFIG_LV_GUI_VER_RES;

static lv_group_t *lv_group;
static lvglContext_t gLvCtx;
extern void main_screen(void);

ql_task_t lvgl_gui_task=NULL;

static const lvGuiKeypadMap_t gLvKeyMap[] = {
 {QL_KEY_MAP_1, '0'},
 {QL_KEY_MAP_2, '1'},
 {QL_KEY_MAP_3, '2'},
 {QL_KEY_MAP_4, '3'},
 {QL_KEY_MAP_5, '4'},
 {QL_KEY_MAP_6, '5'},
 {QL_KEY_MAP_7, '6'},
 {QL_KEY_MAP_8, '7'},
 {QL_KEY_MAP_9, '8'},
 {QL_KEY_MAP_10, '9'},
 {QL_KEY_MAP_11, '*'},
 {QL_KEY_MAP_12, '#'},
 {QL_KEY_MAP_13, LV_KEY_ENTER},
 {QL_KEY_MAP_14, LV_KEY_LEFT},
 {QL_KEY_MAP_15, LV_KEY_RIGHT},
 {QL_KEY_MAP_16, LV_KEY_UP},
 {QL_KEY_MAP_17, LV_KEY_DOWN},
 {QL_KEY_MAP_18, LV_KEY_PREV},
 {QL_KEY_MAP_19, LV_KEY_NEXT},
};

/*===========================================================================
 * Functions
 ===========================================================================*/
/**
* display device flush_cb
*/
static void prvDispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    QL_LVGLDEMO_LOG("disp flush %d/%d/%d/%d", area->x1, area->y1, area->x2, area->y2);

    ql_lcd_write((uint16_t*)color_p, area->x1 , area->y1, area->x2 , area->y2);

    lv_disp_flush_ready(disp);
}

/**
* initialize LCD display device
*/
static bool prvLvInitLcd(void)
{
    lvglContext_t *d = &gLvCtx;

    unsigned pixel_cnt = g_lcd_width * g_lcd_height;
    lv_color_t *buf = (lv_color_t *)malloc(pixel_cnt * sizeof(lv_color_t));
    if (buf == NULL)
        return false;

    lv_disp_buf_init(&(d->disp_buf), buf, buf, pixel_cnt);
    
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = prvDispFlush;
    disp_drv.buffer = &(d->disp_buf);

    lv_disp_drv_register(&disp_drv); // pointer copy; 
    
    return true;
}

/**
* callback of keypad driver, called in ISR
*/
static void prvKeypadCallback(ql_keymatrix_t keymatrix)
{
    lvglContext_t *d = &gLvCtx;

    d->last_key = keymatrix.keymap;
    d->last_key_state = keymatrix.keystate;
    d->keypad_pending = true;
    QL_LVGLDEMO_LOG("keypad last key:%d , state:%d", keymatrix.keymap, keymatrix.keystate);
}

/**
* keypad device read_cb
*/
static bool prvLvKeypadRead(lv_indev_drv_t *kp, lv_indev_data_t *data)
{
    lvglContext_t *d = &gLvCtx;

    uint32_t critical = ql_rtos_enter_critical();
    ql_keymap_e last_key = d->last_key;
    ql_keystate_e last_key_state = d->last_key_state;
    bool keypad_pending = d->keypad_pending;
    d->keypad_pending = false;
    ql_rtos_exit_critical(critical);

    if (keypad_pending)
    {
        data->state = (last_key_state & QL_KEY_STATE_RELEASE) ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR;
        data->key = 0xff;
        for (unsigned n = 0; n < OSI_ARRAY_SIZE(gLvKeyMap); n++)
        {
            if (gLvKeyMap[n].keypad == last_key)
            {
                data->key = gLvKeyMap[n].lv_key;
                break;
            }
        }
        QL_LVGLDEMO_LOG("lvgl keymap:%d   key:%d   state:%d", last_key, data->key, data->state);            
        //lvGuiScreenOn();
    }

    // no more to be read
    return false;
}

/**
* get dev
*/
lv_indev_t* Get_indev(void)
{
    return gLvCtx.keypad;
}

/**
* get group
*/
lv_group_t* Get_group(void)
{
    return lv_group;
}

/**
* initialize keypad device
*/
static bool prvLvInitKeypad(void)
{    
    // configure keypad matrix here
    // example:configure out port:keyout0/keyout1/keyout2/keyout3/keyout4,in port:keyin1/keyin2/keyin3
    ql_keypad_out_e row[QL_KEYPAD_ROW_LENGTH] = {QL_KP_OUT0, QL_KP_OUT1, QL_KP_OUT2, QL_KP_OUT3, QL_KP_OUT4, QL_KP_OUT_NO_VALID};
	ql_keypad_in_e  col[QL_KEYPAD_COL_LENGTH] = {QL_KP_IN1,QL_KP_IN2, QL_KP_IN3, QL_KP_IN4, QL_KP_IN5};
    
    lvglContext_t *d = &gLvCtx;

    lv_indev_drv_t kp_drv;
    lv_indev_drv_init(&kp_drv);
    kp_drv.type = LV_INDEV_TYPE_KEYPAD;
    kp_drv.read_cb = prvLvKeypadRead;
    d->keypad = lv_indev_drv_register(&kp_drv);     
    
    lv_group = lv_group_create();	                // create group
    lv_indev_set_group(d->keypad, lv_group);	    // connect group to dev

    ql_keypad_init(prvKeypadCallback, row, col);    // keypad init

    return true;
}

/**
* run littlevgl task handler
*/
static void prvLvTaskHandler(void)
{
    lvglContext_t *d = &gLvCtx;

    lv_task_handler();
    
#ifdef QL_APP_FEATURE_LVGL_V7
    ql_rtos_timer_start(d->task_timer, 10, TRUE);
 #else
    unsigned next_run = lv_task_get_tick_next_run();
    
    ql_rtos_timer_start_relaxed(d->task_timer, next_run, TRUE, QL_WAIT_FOREVER);
    
    lv_refr_now(d->disp);
#endif

}

/**
* whether inactive timeout
*/
static bool prvIsInactiveTimeout(void)
{
    lvglContext_t *d = &gLvCtx;

    if (d->screen_on_users != 0)
        return false;
    if (d->inactive_timeout == 0)
        return false;
    if (!d->anim_inactive && lv_anim_count_running())
        return false;
    return lv_disp_get_inactive_time(d->disp) > d->inactive_timeout;
}

/**
* get keypad input device
*/
lv_indev_t *lvGuiGetKeyPad(void)
{
    lvglContext_t *d = &gLvCtx;
    return d->keypad;
}

/**
* send event to gui thread
*/
void lvGuiSendEvent(ql_event_t *evt)
{
    lvglContext_t *d = &gLvCtx;
    ql_rtos_event_send(d->thread, evt);
}

/**
* request screen on
*/
bool lvGuiRequestSceenOn(uint8_t id)
{
    lvglContext_t *d = &gLvCtx;

    if (id > 31)
        return false;

    unsigned mask = (1 << id);
    d->screen_on_users |= mask;
    return true;
}

/**
* release screen on request
*/
bool lvGuiReleaseScreenOn(uint8_t id)
{
    lvglContext_t *d = &gLvCtx;

    if (id > 31)
        return false;

    unsigned mask = (1 << id);
    d->screen_on_users &= ~mask;
    return true;
}

/**
* turn off screen
*/
void lvGuiScreenOff(void)
{
    QL_LVGLDEMO_LOG("screen off");

    lvglContext_t *d = &gLvCtx;
    
    if (!d->screen_on)
        return;

    ql_lcd_display_off();
    d->screen_on = false;
}

/**
* turn on screen
*/
void lvGuiScreenOn(void)
{
    lvglContext_t *d = &gLvCtx;

    if (d->screen_on)
        return;

    QL_LVGLDEMO_LOG("screen on");
    ql_lcd_display_on();
    d->screen_on = true;
}

/**
* set screen off timeout at inactive
*/
void lvGuiSetInactiveTimeout(unsigned timeout)
{
    lvglContext_t *d = &gLvCtx;
    d->inactive_timeout = timeout;
}

/**
* set whether animation is regarded as inactive
*/
void lvGuiSetAnimationInactive(bool inactive)
{
    lvglContext_t *d = &gLvCtx;
    d->anim_inactive = inactive;
}


/**
* button event cb entry
*/
static void lvglBtnEventCb(lv_obj_t * obj, lv_event_t event)
{
    #if 0
    lv_obj_t* lv_label = lv_list_get_btn_label(obj);    
    char* str1 = lv_label_get_text(lv_label);           
    char* str2 = "ON";
    if(event == LV_EVENT_CLICKED)               //click event
    {        
        printf("Clicked\n");

        if(strcmp(str1,str2))
        {            
			lv_label_set_text(lv_label, "ON");
		}
        else
        {
			lv_label_set_text(lv_label, "OFF");
		}
    }
    else if(event == LV_EVENT_VALUE_CHANGED)    //switch change event
    {
        printf("Toggled\n");
		if(lv_btn_get_state(obj) == LV_BTN_STATE_REL)
        {
			lv_label_set_text(lv_label, "OFF");
		}
        else
        {
			lv_label_set_text(lv_label, "ON");
		}
    }
    #endif
}



static void ql_lvgl_demo_thread(void *param)
{
    lvglContext_t *d = &gLvCtx;
    QlOSStatus err = 0;
    ql_event_t event;

    QL_LVGLDEMO_LOG("lvgl demo thread enter, param 0x%x", param);

    d->screen_on = true;
    d->keypad_pending = false;
    d->anim_inactive = false;

    d->screen_on_users = 0;
    d->inactive_timeout = CONFIG_LV_GUI_SCREEN_OFF_TIMEOUT;
    
    
    err=ql_rtos_task_get_current_ref(&d->thread);
    require_action(err, goto exit, "lvgl_demo_taskref get failed");
    err=ql_rtos_timer_create(&d->task_timer, d->thread, (lvgl_Callback_t)prvLvTaskHandler, NULL);
	require_action(err, goto exit, "lvgl_demo_timer created failed");
    
    if(ql_lcd_init() != QL_LCD_SUCCESS )
    {
        QL_LVGLDEMO_LOG("LCD init failed");
        goto exit;
    }

    lv_init();
    prvLvInitLcd();
    prvLvInitKeypad();

    ctp_init();

    lvGuiCreate_t create = (lvGuiCreate_t)param;
    if (create != NULL)
    {
        create();
    }

	err = ql_rtos_timer_start(d->task_timer, 10, 1);
	require_action(err, goto exit, "lvgl_demo_timer start failed");
   
    lv_disp_trig_activity(d->disp);
    
    while(1)
    {
        ql_event_try_wait(&event);
        if (event.id == QUEC_LVGL_QUIT_IND)
            break;

		//语音相关的事件	
		if(QL_VC_INIT_OK_IND <= event.id && event.id <= QL_VC_CCWA_IND)
		{
			ic_vc_call_event(event);
			break;
		}
	
        if (d->screen_on && prvIsInactiveTimeout())
        {
            QL_LVGLDEMO_LOG("inactive timeout, screen off");
            lvGuiScreenOff();
        }
    }
    
exit:
    QL_LVGLDEMO_LOG("lvgl demo thread exit, param 0x%x", param);
    err = ql_rtos_task_delete(NULL);
    if(err != QL_OSI_SUCCESS)
    {
        QL_LVGLDEMO_LOG("lvgl task deleted failed");
    }
}

void ql_lvgl_app_init(void)
{
    QlOSStatus err = 0;
    #ifdef QL_APP_FEATURE_LVGL_V7
    err = ql_rtos_task_create(&lvgl_gui_task, QL_Lvgl_TASK_STACK_SIZE, QL_Lvgl_TASK_PRIO, "QLVGLDEMO", ql_lvgl_demo_thread, main_screen, QL_LVGL_TASK_EVENT_CNT);   
    #else
    err = ql_rtos_task_create(&lvgl_gui_task, QL_Lvgl_TASK_STACK_SIZE, QL_Lvgl_TASK_PRIO, "QLVGLDEMO", ql_lvgl_demo_thread, lvglAnimCreate, QL_LVGL_TASK_EVENT_CNT);   
    #endif 
    if (err != QL_OSI_SUCCESS)
    {
        QL_LVGLDEMO_LOG("lvgl demo task created failed");
    }
}



