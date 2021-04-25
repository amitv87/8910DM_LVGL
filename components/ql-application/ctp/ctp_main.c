#include "ctp_interface.h"
#include "stdint.h"
#include "stdlib.h"
#include <stdio.h>
#include <string.h>


#include "ql_gpio.h"
#include "ql_api_osi.h"
#include "lvgl.h"

#define TOUCH_QUEUE_SIZE      (20)
ql_queue_t touch_data_queue = NULL;


ctp_funcs_t *ctp_func;



static void ic_lvgl_queue_touch_event(lv_indev_data_t *event)
{
    if (touch_data_queue != NULL && event) {
        if(ql_rtos_queue_release(touch_data_queue, sizeof(lv_indev_data_t), (uint8_t *)event, 0) != QL_OSI_SUCCESS){
                CTP_LOG("queue touch event fail"); 
        }
    }
}

static bool ic_lvgl_pop_touch_event(lv_indev_data_t *event)
{
    if (touch_data_queue != NULL && event) {
        if(ql_rtos_queue_wait(touch_data_queue, (uint8_t *)event, sizeof(lv_indev_data_t), 0) == QL_OSI_SUCCESS){
            return true;
        }else{
            //CTP_LOG("touch queue empty");
            return false; 
        }
    }
}


static void ctp_interrupt_callback(void *ctx)
{
    multi_touch_event_t tp_event = {0};
    lv_indev_data_t lv_tp_event;
    static ctp_pen_state_enum_t pre_state = PEN_UP;
    ctp_pen_state_enum_t current_state;

    CTP_LOG("ctp_interrupt_callback");
    if(ctp_func){
        ctp_func->read(&tp_event);
        CTP_LOG("tp_event, timestamp:%d, event:%s, x:%d, y:%d", 
                tp_event.time_stamp, tp_event.model?"DOWN":"UP", tp_event.points[0].x, tp_event.points[0].y);
        if(tp_event.model){
            current_state = PEN_DOWN;
        }else{
            current_state = PEN_UP;
        }

        //if(current_state == pre_state) return;

        pre_state = current_state;

        //send touch event to touch queue
        lv_tp_event.point.x = tp_event.points[0].x;
        lv_tp_event.point.y = tp_event.points[0].y;
        lv_tp_event.state = current_state==PEN_DOWN?LV_INDEV_STATE_PR:LV_INDEV_STATE_REL;

        ic_lvgl_queue_touch_event(&lv_tp_event);

    }
}

static bool ic_lv_touch_screen_read_cb(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    static lv_indev_drv_t last_data = {0};

    memcpy(data, &last_data, sizeof(lv_indev_data_t));
    if(ic_lvgl_pop_touch_event(&last_data)){
        CTP_LOG("%d, %d, state:%d", data->point.x, data->point.y, data->state);
        return true;
    }else{
        //memset(&last_data, 0, sizeof(lv_indev_data_t));
        return false;
    }

    return false;
}

bool ctp_init(void)
{
    CTP_LOG("ctp_init");

    //create a queue for touch data
    if(ql_rtos_queue_create(&touch_data_queue, sizeof(lv_indev_data_t), TOUCH_QUEUE_SIZE) != QL_OSI_SUCCESS){
        CTP_LOG("create touch queue fail");
        return false;
    }

    //start detect touch ic
    ctp_func = ctp_GetFunc();
    if(!ctp_func){
        CTP_LOG("get ctp funcs fail");
        return false;
    }

    //register ctp eint
    ql_gpio_deinit(CTP_EINT_PIN);

    ql_int_register(CTP_EINT_PIN, EDGE_TRIGGER, DEBOUNCE_EN, EDGE_FALLING, PULL_UP, ctp_interrupt_callback, NULL);
    ql_int_enable(CTP_EINT_PIN);


    //input devices
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);      /*Basic initialization*/

    //touch
    indev_drv.type = LV_INDEV_TYPE_POINTER;    
    indev_drv.read_cb = ic_lv_touch_screen_read_cb;
    lv_indev_t * touch_indev = lv_indev_drv_register(&indev_drv);

    return true;
}
