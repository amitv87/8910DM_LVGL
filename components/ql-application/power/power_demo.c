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

#include "power_demo.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_POWERDEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_POWERDEMO_LOG(msg, ...)         QL_LOG(QL_POWERDEMO_LOG_LEVEL, "ql_POWER", msg, ##__VA_ARGS__)
#define QL_POWERDEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("ql_POWER", msg, ##__VA_ARGS__)

#if !defined(require_action)
	#define require_action(x, action, str)																		\
			do                                                                                                  \
			{                                                                                                   \
				if(x != 0)                                                                        				\
				{                                                                                               \
					QL_POWERDEMO_LOG(str);																		\
					{action;}																					\
				}                                                                                               \
			} while( 1==0 )
#endif

/*===========================================================================
 * Variate
 ===========================================================================*/
ql_task_t power_task = NULL;
ql_timer_t power_timer = NULL;
int wake_lock_1, wake_lock_2;

/*===========================================================================
 * Functions
 ===========================================================================*/
static void ql_power_demo_thread(void *param)
{
    //QL_POWERDEMO_LOG("power demo thread enter, param 0x%x", param);

	ql_event_t event;
	int err;

	while(1)
	{
		if(ql_event_try_wait(&event) != 0)
		{
			continue;
		}	
		QL_POWERDEMO_LOG("receive event, id is %d", event.id);
		
		switch(event.id)
		{
			case QUEC_SLEEP_ENETR_AUTO_SLEPP:
				
				err = ql_autosleep_enable(QL_ALLOW_SLEEP);
				require_action(err, continue, "failed to set auto sleep");

				err = ql_lpm_wakelock_unlock(wake_lock_1);
				require_action(err, continue, "lock1 unlocked failed");

				err = ql_lpm_wakelock_unlock(wake_lock_2);
				require_action(err, continue, "lock2 unlocked failed");		
				
				QL_POWERDEMO_LOG("set auto sleep mode ok");
				
			break;

			case QUEC_SLEEP_EXIT_AUTO_SLEPP:
				err = ql_autosleep_enable(QL_NOT_ALLOW_SLEEP);
				require_action(err, continue, "failed to set auto sleep");
			break;

			case QUEC_SLEEP_QUICK_POWER_DOWM:
				ql_power_down(POWD_IMMDLY);
			break;

			case QUEC_SLEEP_NORMAL_POWER_DOWM:
				ql_power_down(POWD_NORMAL);
			break;

			case QUEC_SLEEP_QUICK_RESET:
				ql_power_reset(RESET_QUICK);
			break;

			case QUEC_SLEEP_NORMAL_RESET:
				ql_power_reset(RESET_NORMAL);
			break;

			default:
			break;
		}
	}

    ql_rtos_task_delete(NULL);
}

void power_timer_callback(void *ctx)
{
	ql_event_t event = {0};

	event.id = QUEC_SLEEP_ENETR_AUTO_SLEPP;
	ql_rtos_event_send(power_task, &event);
}

void ql_power_app_init(void)
{
    QlOSStatus err = QL_OSI_SUCCESS;

    err = ql_rtos_task_create(&power_task, 1024, APP_PRIORITY_NORMAL, "ql_powerdemo", ql_power_demo_thread, NULL, 3);
	require_action(err, return, "power demo task created failed");

	err = ql_rtos_timer_create(&power_timer, power_task, power_timer_callback, NULL);
	require_action(err, return, "demo_timer created failed");

	err = ql_rtos_timer_start(power_timer, 1000, 0);   // 1秒后开启自动休眠
	require_action(err, return, "demo_timer start failed");

	wake_lock_1 = ql_lpm_wakelock_create("my_lock_1", 10);
	require_action((wake_lock_1 <= 0), return, "lock1 created failed");
	
	wake_lock_2 = ql_lpm_wakelock_create("my_lock_2", 10);
	require_action((wake_lock_2 <= 0), return, "lock2 created failed");

	err = ql_lpm_wakelock_lock(wake_lock_1);
	require_action(err, return, "lock1 locked failed");

	err = ql_lpm_wakelock_lock(wake_lock_2);
	require_action(err, return, "lock2 locked failed");	
}


