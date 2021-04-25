#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "osi_demo.h"

#define QL_OSI_DEMO_LOG_LEVEL		QL_LOG_LEVEL_INFO
#define QL_OSI_DEMO_LOG(msg, ...)	QL_LOG(QL_OSI_DEMO_LOG_LEVEL, "ql_osi_demo", msg, ##__VA_ARGS__)
#define FEED_DOG_MAX_MISS_CNT   5

ql_task_t demo_task = NULL;
ql_timer_t demo_timer = NULL;

void demo_task_callback(void *ctx)
{
	ql_event_t test_event = {0};
	 
	while(1)
	{
		if(ql_event_try_wait(&test_event) != 0)
		{
			continue;
		}
		
		if(test_event.id == QUEC_KERNEL_FEED_DOG)
		{
			QL_OSI_DEMO_LOG("demo task receive feed dog event");

			if(ql_rtos_feed_dog() != QL_OSI_SUCCESS)
			{
				QL_OSI_DEMO_LOG("feed dog failed");
			}
		}
	}
}

void feed_dog_callback(uint32 id_type, void *ctx)
{	
	ql_event_t event;

	if(id_type == QUEC_KERNEL_FEED_DOG)
	{
		QL_OSI_DEMO_LOG("feed dog callback run");
		
		event.id = QUEC_KERNEL_FEED_DOG;
		if(ql_rtos_event_send(demo_task, &event) != QL_OSI_SUCCESS)
		{
			QL_OSI_DEMO_LOG("send feed_dog event to demo task failed");
		}	
		else
		{
			QL_OSI_DEMO_LOG("send feed dog event to demo task ok");
		}
	}
}

void timer_callback(void *ctx)
{
	static int cnt = 0;

	QL_OSI_DEMO_LOG("timer run %d times", ++cnt);

	return;
}

QlOSStatus ql_osi_demo_init(void)
{	
	QlOSStatus err = QL_OSI_SUCCESS;
	
	err = ql_rtos_task_create(&demo_task, DEMO_TASK_STACK_SIZE, DEMO_TASK_PRIO, "DEMO_TASK", demo_task_callback, NULL, DEMO_TASK_EVENT_CNT);
	if(err != QL_OSI_SUCCESS)
	{
		QL_OSI_DEMO_LOG("demo_task created failed");
		goto exit;
	}

	err = ql_rtos_timer_create(&demo_timer, demo_task, timer_callback, NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_OSI_DEMO_LOG("demo_timer created failed");
		goto exit;
	}

	err = ql_rtos_timer_start(demo_timer, 1000, 0);
	if(err != QL_OSI_SUCCESS)
	{
		QL_OSI_DEMO_LOG("demo_timer start failed");
		goto exit;
	}	

	err = ql_rtos_swdog_register((ql_swdog_callback)feed_dog_callback, demo_task);  
	if(err != QL_OSI_SUCCESS)
	{
		QL_OSI_DEMO_LOG("demo_task register sw dog failed");  
		goto exit;
	}	

	err = ql_rtos_sw_dog_enable(5000, 3);  //配置软件看门狗每 5 秒调用一次 feed_dog_callback ，如果连续 3 次调用且没有进行喂狗操作，则判定线程已陷入死循环，触发重启
	if(err != QL_OSI_SUCCESS)
	{
		QL_OSI_DEMO_LOG("sw dog enable failed");  
		goto exit;
	}
	
	return QL_OSI_SUCCESS;

exit:
	if(demo_task != NULL)   
	{
		ql_rtos_task_delete(demo_task);
		demo_task = NULL;
	}
	if(demo_timer != NULL) 
	{
		ql_rtos_timer_delete(demo_timer);
		demo_timer = NULL;
	}
	return err;
}


