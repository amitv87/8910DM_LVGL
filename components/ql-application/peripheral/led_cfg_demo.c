/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_gpio.h"

#include "led_cfg_demo.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_LEDCFGDEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_LEDCFGDEMO_LOG(msg, ...)         QL_LOG(QL_LEDCFGDEMO_LOG_LEVEL, "ql_LEDCFGDEMO", msg, ##__VA_ARGS__)
#define QL_LEDCFGDEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("ql_LEDCFGDEMO", msg, ##__VA_ARGS__)

/*===========================================================================
 * Variate
 ===========================================================================*/
ql_task_t ledcfg_task = NULL;

/*===========================================================================
 * Functions
 ===========================================================================*/
static void _ledcfg_demo_cb(unsigned int ind_type, uint32_t net_status, uint32_t net_type)
{
    if( ind_type == QUEC_LEDCFG_EVENT_IND )
    {
        ql_event_t event;
        event.id     = ind_type;
        event.param1 = net_status;
        event.param2 = net_type;
        ql_rtos_event_send(ledcfg_task, &event);
        QL_LEDCFGDEMO_LOG("led config net_status is %d, net_type is %d", event.param1, event.param2);
    }
    else
    {
        QL_LEDCFGDEMO_LOG("led config ind error!");
    }
}

static void ql_ledcfg_demo_thread(void *param)
{
    QL_LEDCFGDEMO_LOG("led config demo thread enter, param 0x%x", param);

    /*** LPG open ***/
    ql_pwm_lpg_open();
    QL_LEDCFGDEMO_LOG("led config start");

    ql_pin_set_func(126, 3);    //Pin126 set zsp_uart_cts
    ql_pin_set_func(6, 4);      //Pin6 set gpio_22
    ql_gpio_init(GPIO_22, GPIO_OUTPUT, PULL_NONE, LVL_LOW);

    ql_event_t event;
    ql_ledcfg_callback_register(_ledcfg_demo_cb);

    while(1)
    {
#if 0
/***** demo1 *****/
        ql_event_wait(&event, 1);

        /*** LPG enable ***/
        /* slow twinkle */
        ql_pwm_lpg_enable(LPG_PER_2000MS, LPG_ONTIME_200MS);
        QL_LEDCFGDEMO_LOG("led config slow twinkle");
        ql_rtos_task_sleep_s(20);

        /* fast twinkle */
        ql_pwm_lpg_enable(LPG_PER_500MS, LPG_ONTIME_234MS);
        QL_LEDCFGDEMO_LOG("led config fast twinkle");
        ql_rtos_task_sleep_s(20);

        /* speed twinkle */
        ql_pwm_lpg_enable(LPG_PER_125MS, LPG_ONTIME_62MS);
        QL_LEDCFGDEMO_LOG("led config speed twinkle");
        ql_rtos_task_sleep_s(20);

        /*** LPG disable ***/
        ql_pwm_lpg_disable();
        QL_LEDCFGDEMO_LOG("led config disable");

        /* keep on */
        ql_pwm_lpg_enable(LPG_PER_125MS, LPG_ONTIME_200MS);
        QL_LEDCFGDEMO_LOG("led config keep on");
        ql_rtos_task_sleep_s(20);

        /* keep off */
        ql_pwm_lpg_enable(LPG_PER_2000MS, LPG_ONTIME_UNDEFINE);
        QL_LEDCFGDEMO_LOG("led config keep off");
        ql_rtos_task_sleep_s(20);

        /*** LPG close ***/
        ql_pwm_lpg_close();
        QL_LEDCFGDEMO_LOG("led config stop");
/***** demo1 *****/
#endif

        if( ql_event_try_wait(&event) != 0 )
        {
            continue;
        }

        if( event.id == QUEC_LEDCFG_EVENT_IND )
        {
            switch( event.param1 )
            {
                case IND_SIM_NOT_INSERT:
                case IND_SIM_INITIALIZING:
                case IND_SIM_PIN_LOCK:
                case IND_SIM_CPIN_READY:
                case IND_SEARCHING:
                case IND_CAMP_ON_CELL:
                case IND_REGISTERING:
                    QL_LEDCFGDEMO_LOG("led config slow twinkle [%d]", event.param1);
                    ql_pwm_lpg_enable(LED_TWINKLE_SLOW_PERIOD, LED_TWINKLE_SLOW_ONTIME);
                    break;
            
                case IND_STANDBY:
                case IND_PDP_ACTIVING:
                case IND_PDP_ACTIVED:
                case IND_SOCKET_SET_UP:
                case IND_PDP_DEACTIVING:
                    QL_LEDCFGDEMO_LOG("led config fast twinkle [%d]", event.param1);
                    ql_pwm_lpg_enable(LED_TWINKLE_FAST_PERIOD, LED_TWINKLE_FAST_ONTIME);
                    break;
            
                case IND_DATA_TRANSMIT:
                    QL_LEDCFGDEMO_LOG("led config speed twinkle [%d]", event.param1);
                    ql_pwm_lpg_enable(LED_TWINKLE_SPEED_PERIOD, LED_TWINKLE_SPEED_ONTIME);
                    break;
            
                case IND_DIALING:
                    QL_LEDCFGDEMO_LOG("led config keep on [%d]", event.param1);
                    ql_pwm_lpg_enable(LED_TWINKLE_ON_PERIOD, LED_TWINKLE_ON_ONTIME);
                    break;
            
                default:
                    QL_LEDCFGDEMO_LOG("led config error!");
                    break;
            }
            
            if( event.param2 == IND_NET_TYPE_4G )
            {
                QL_LEDCFGDEMO_LOG("led config network is 4G");
                ql_gpio_set_level(GPIO_22, LVL_HIGH);
            }
            else
            {
                QL_LEDCFGDEMO_LOG("led config network is not 4G");
                ql_gpio_set_level(GPIO_22, LVL_LOW);
            }
        }
    }

    ql_rtos_task_delete(NULL);
}

void ql_ledcfg_app_init(void)
{
    QlOSStatus err = QL_OSI_SUCCESS;

    err = ql_rtos_task_create(&ledcfg_task, 1024, APP_PRIORITY_NORMAL, "ql_ledcfgdemo", ql_ledcfg_demo_thread, NULL, 1);
    if( err != QL_OSI_SUCCESS )
    {
        QL_LEDCFGDEMO_LOG("led config demo task created failed");
    }
}


