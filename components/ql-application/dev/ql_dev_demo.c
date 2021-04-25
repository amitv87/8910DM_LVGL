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

=================================================================*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_api_dev.h"
#include "ql_dev_demo.h"

#define QL_DEV_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_DEV_LOG(msg, ...)			QL_LOG(QL_DEV_LOG_LEVEL, "ql_DEV", msg, ##__VA_ARGS__)
#define QL_DEV_LOG_PUSH(msg, ...)		QL_LOG_PUSH("ql_DEV", msg, ##__VA_ARGS__)

#define QL_DEV_TASK_STACK_SIZE      2048
#define QL_DEV_TASK_PRIO            APP_PRIORITY_NORMAL
#define QL_DEV_TASK_EVENT_CNT       5

static void ql_dev_demo_thread(void *param)
{
	QlOSStatus err = QL_OSI_SUCCESS;
    QL_DEV_LOG("dev demo thread enter, param 0x%x", param);
	
	for (int n = 0; n < 10; n++)
    {
        QL_DEV_LOG("hello dev demo %d", n);
        ql_rtos_task_sleep_ms(500);
    }
	
	char devinfo[64] = {0};
	ql_dev_get_imei(devinfo, 64, 0);
	QL_DEV_LOG("IMEI: %s", devinfo);
    
	ql_dev_get_sn(devinfo, 64, 0);
	QL_DEV_LOG("SN: %s", devinfo);
    
	ql_dev_get_product_id(devinfo, 64);
	QL_DEV_LOG("Product: %s", devinfo);
    
	ql_dev_get_model(devinfo, 64);
	QL_DEV_LOG("Model: %s", devinfo);
    
	ql_dev_get_firmware_version(devinfo, 64);
	QL_DEV_LOG("FW: %s", devinfo);

	ql_dev_get_firmware_subversion(devinfo, 64);
	QL_DEV_LOG("subVer: %s", devinfo);
    
	uint8_t cfun = 0;
	ql_dev_get_modem_fun(&cfun, 0);
	QL_DEV_LOG("cfun: %d", cfun);
	ql_dev_set_modem_fun(1, 0, 0);

    //ql_log_set_port(QL_LOG_PORT_USB);//set the log port as USB AP port
    
	int32_t ql_temp = 0;
	ql_dev_get_temp_value(&ql_temp);
	QL_DEV_LOG("TEMP: %d", ql_temp);
	
	err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_DEV_LOG("task deleted failed");
	}
}

void ql_dev_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	ql_task_t dev_task = NULL;
	
	err = ql_rtos_task_create(&dev_task, QL_DEV_TASK_STACK_SIZE, QL_DEV_TASK_PRIO, "QDEVDEMO", ql_dev_demo_thread, NULL, QL_DEV_TASK_EVENT_CNT);
	if (err != QL_OSI_SUCCESS)
	{
		QL_DEV_LOG("dev demo task created failed");
	}
}

