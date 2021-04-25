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
#include "ql_api_sim.h"
#include "ql_sim_demo.h"

#define QL_SIM_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_SIM_LOG(msg, ...)			QL_LOG(QL_SIM_LOG_LEVEL, "ql_sim_demo", msg, ##__VA_ARGS__)

#define QL_SIM_TASK_STACK_SIZE      2048
#define QL_SIM_TASK_PRIO            APP_PRIORITY_NORMAL
#define QL_SIM_TASK_EVENT_CNT       5

static void ql_sim_demo_thread(void *param)
{
	QlOSStatus err = QL_OSI_SUCCESS;
    ql_sim_errcode_e ret = QL_SIM_SUCCESS;
    char siminfo[64] = {0};
	uint8_t sim_id = ql_sim_get_operate_id();
    ql_sim_status_e card_status = 0;
    ql_sim_verify_pin_info_s   pin = {0};
    ql_sim_unblock_pin_info_s  pin_un = {0};
    ql_sim_change_pin_info_s   pin_change = {0};
    	
    for (int n = 0; n < 5; n++)
    {
        QL_SIM_LOG("hello sim demo %d", n);
        ql_rtos_task_sleep_ms(2000);
    }

	ret = ql_sim_get_imsi(sim_id, siminfo, 64);
	QL_SIM_LOG("ret:0x%x, IMSI: %s", ret, siminfo);

    ret = ql_sim_get_iccid(sim_id, siminfo, 64);
    QL_SIM_LOG("ret:0x%x, ICCID: %s", ret, siminfo);

    ret = ql_sim_get_phonenumber(sim_id, siminfo, 64);
    QL_SIM_LOG("ret:0x%x, phonenumber: %s", ret, siminfo);

    //enable pin verify and effective after reboot
    memset(&pin, 0, sizeof(ql_sim_verify_pin_info_s));	
	memcpy(pin.pin_value,"1234",5);
	ret = ql_sim_enable_pin(sim_id, &pin);
	QL_SIM_LOG("ql_sim_enable_pin ret:0x%x", ret);

    ret = ql_sim_get_card_status(sim_id, &card_status);
    QL_SIM_LOG("ret:0x%x, card_status: %d", ret, card_status);

    if(QL_SIM_STATUS_SIMPIN == card_status)
    {
        memset(&pin, 0, sizeof(ql_sim_verify_pin_info_s));
        memcpy(pin.pin_value, "1234", 5);
        ret = ql_sim_verify_pin(sim_id, &pin);
        QL_SIM_LOG("ql_sim_verify_pin ret:0x%x", ret);

        ret = ql_sim_get_card_status(sim_id, &card_status);
        QL_SIM_LOG("ret:0x%x, card_status: %d", ret, card_status);
    }
    else if(QL_SIM_STATUS_SIMPUK == card_status)
    {
        memset(&pin_un, 0, sizeof(ql_sim_verify_pin_info_s));
        memcpy(pin_un.puk_value, "26601934", 9);
        memcpy(pin_un.new_pin_value, "1234", 5);
        ret = ql_sim_unblock_pin(sim_id, &pin_un);
        QL_SIM_LOG("ql_sim_unblock_pin ret:0x%x", ret);

        ret = ql_sim_get_card_status(sim_id, &card_status);
        QL_SIM_LOG("ret:0x%x, card_status: %d", ret, card_status);
    }

    memset(&pin_change, 0, sizeof(ql_sim_change_pin_info_s));
    memcpy(pin_change.old_pin_value, "1234", 5);
    memcpy(pin_change.new_pin_value, "4321", 5);
    ret = ql_sim_change_pin(sim_id, &pin_change);
    QL_SIM_LOG("ql_sim_change_pin ret:0x%x", ret);

    memset(&pin_change, 0, sizeof(ql_sim_change_pin_info_s));
    memcpy(pin_change.old_pin_value, "4321", 5);
    memcpy(pin_change.new_pin_value, "1234", 5);
    ret = ql_sim_change_pin(sim_id, &pin_change);
    QL_SIM_LOG("ql_sim_change_pin ret:0x%x", ret);

    memset(&pin,0,sizeof(ql_sim_verify_pin_info_s));	
	memcpy(pin.pin_value,"1234",5);
	ret = ql_sim_disable_pin(sim_id, &pin);
	QL_SIM_LOG("ql_sim_disable_pin ret:0x%x", ret);
    	
	err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_SIM_LOG("task deleted failed");
	}
}

void ql_sim_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	ql_task_t sim_task = NULL;
	
	err = ql_rtos_task_create(&sim_task, QL_SIM_TASK_STACK_SIZE, QL_SIM_TASK_PRIO, "QSIMDEMO", ql_sim_demo_thread, NULL, QL_SIM_TASK_EVENT_CNT);
	if (err != QL_OSI_SUCCESS)
	{
		QL_SIM_LOG("task created failed");
	}
}

