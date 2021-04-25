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
20/10/2020  marvin          create

=================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_common.h"
#include "ql_api_osi.h"
#include "ql_api_voice_call.h"
#include "ql_log.h"

ql_task_t vc_task = NULL;

void user_voice_call_event_callback(int event_id, void *ctx)
{
	ql_event_t vc_event = {0};
	switch(event_id)
	{
		case QL_VC_RING_IND:
			//ctx will be destroyed when exit.
			QL_VC_LOG("RING TEL = %s",(char *)ctx);
			break;
		case QL_VC_CCWA_IND:
			//ctx will be destroyed when exit.
			QL_VC_LOG("QL_VC_CCWA_IND : %s",(char *)ctx);
			break;
		default:
			break;
	}
	vc_event.id = event_id;
	if(NULL != ctx)
	{
		vc_event.param1 = *(uint32 *)ctx;
	}
	ql_rtos_event_send(vc_task, &vc_event);
}


void voice_call_demo_task(void * param)
{
	QL_VC_LOG("enter");
	ql_vc_errcode_e err = QL_VC_SUCCESS;
	ql_event_t vc_event = {0};
	
	ql_voice_call_callback_register(user_voice_call_event_callback);
	
	//wait init ok;
	ql_rtos_task_sleep_s(10);
	
#if 0
	//0 : Disable automatic answer
	//x : x>0, answer automatically after x rings
	err = ql_voice_auto_answer(0);
	if(err != QL_VC_SUCCESS){
		QL_VC_LOG("ql_voice_auto_answer FAIL");
	}else {
		QL_VC_LOG("ql_voice_auto_answer OK");
	}
#endif

#if 0
	//Enable call waiting
	err = ql_voice_call_wait_set(0);
	if(err != QL_VC_SUCCESS){
		QL_VC_LOG("ql_voice_call_wait_set FAIL");
	}else {
		QL_VC_LOG("ql_voice_call_wait_set OK");
	}
	
	//Get call waiting status
	uint8_t result = 0;
	err = ql_voice_call_wait_get(&result);
	if(err != QL_VC_SUCCESS){
		QL_VC_LOG("ql_voice_call_wait_get FAIL,result=%d",result);
	}else {
		QL_VC_LOG("ql_voice_call_wait_get OK,result=%d",result);
	}
#endif

#if 0
	//Set call forwarding
	ql_rtos_task_sleep_s(3);
	err = ql_voice_call_fw(0,3,"18924012349");
	if(err != QL_VC_SUCCESS){
		QL_VC_LOG("ql_voice_call_fw register FAIL");
	}else{
		QL_VC_LOG("ql_voice_call_fw register OK");
	}
	
	//Get call forwarding
	ql_rtos_task_sleep_s(3);
	char telNum[32] = {0};
	err = ql_voice_call_fw(0,2, telNum);
	if(err != QL_VC_SUCCESS){
		QL_VC_LOG("ql_voice_call_fw get FAIL");
	}else{
		QL_VC_LOG("ql_voice_call_fw get OK, telNum = %s",telNum);
	}

	//Cancel call forwarding
	ql_rtos_task_sleep_s(5);
	err = ql_voice_call_fw(0,4,"18924012349");
	if(err != QL_VC_SUCCESS){
		QL_VC_LOG("ql_voice_call_fw erasure FAIL");
	}else{
		QL_VC_LOG("ql_voice_call_fw erasure OK");
	}

	//Get call forwarding
	ql_rtos_task_sleep_s(3);
	memset(telNum, 0, sizeof(telNum));
	err = ql_voice_call_fw(0,2, telNum);
	if(err != QL_VC_SUCCESS){
		QL_VC_LOG("ql_voice_call_fw get FAIL");
	}else{
		QL_VC_LOG("ql_voice_call_fw get OK, telNum = %s",telNum);
	}
#endif

	
#if 0
	QL_VC_LOG("start call");
	err = ql_voice_call_start("18924012349");
	if(err != QL_VC_SUCCESS){
		QL_VC_LOG("ql_voice_call_start FAIL");
	}else{
		QL_VC_LOG("ql_voice_call_start OK");
	}
	
	ql_event_wait(&vc_event, QL_WAIT_FOREVER);
	if(vc_event.id == QL_VC_NOCARRIER_IND)
	{
		QL_VC_LOG("NOCARRIER, reson = %d", vc_event.param1);
	}
	else if(vc_event.id == QL_VC_CONNECT_IND)
	{
		QL_VC_LOG("CONNECT");
		ql_rtos_task_sleep_s(2);
		err = ql_voice_call_start_dtmf("123456789*#ABCD", 0);
		if(err != QL_VC_SUCCESS){
			QL_VC_LOG("ql_voice_call_start_dtmf FAIL");
		}else{
			QL_VC_LOG("ql_voice_call_start_dtmf OK");
		}
		ql_rtos_task_sleep_s(10);
		err = ql_voice_call_end();
		if(err != QL_VC_SUCCESS){
			QL_VC_LOG("ql_voice_call_end FAIL");
		}else{
			QL_VC_LOG("ql_voice_call_end OK");
		}
	}
#endif

#if 0
	ql_voice_call_start("10086");
	ql_rtos_task_sleep_s(15);
	uint8_t total;
	ql_vc_info_s vc_info[QL_VC_MAX_NUM];
	err = ql_voice_call_clcc(&total, vc_info);
	if(err != QL_VC_SUCCESS){
		QL_VC_LOG("ql_voice_call_clcc FAIL");
	}else {
		QL_VC_LOG("total number = %d",total);
		for(uint8_t i = 0; i<total; i++){
			QL_VC_LOG("idx:%d idr:%d status:%d mpty:%d number:%s",
					vc_info[i].idx,vc_info[i].direction,vc_info[i].status,vc_info[i].multiparty,vc_info[i].number);
		}
	}
#endif

	QL_VC_LOG("wait call");
	while(1){
		ql_event_wait(&vc_event, QL_WAIT_FOREVER);
		switch(vc_event.id)
		{
			case QL_VC_RING_IND:
				ql_rtos_task_sleep_s(10);
				err = ql_voice_call_answer();
				if(err != QL_VC_SUCCESS){
					QL_VC_LOG("ql_voice_call_answer FAIL");
				}else{
					QL_VC_LOG("ql_voice_call_answer OK");
				}
				break;
			case QL_VC_CONNECT_IND:
				QL_VC_LOG("CONNECT");
				ql_rtos_task_sleep_s(10);
				err = ql_voice_call_end();
				if(err != QL_VC_SUCCESS){
					QL_VC_LOG("ql_voice_call_end FAIL");
				}else{
					QL_VC_LOG("ql_voice_call_end OK");
				}
				break;
			case QL_VC_NOCARRIER_IND:
				QL_VC_LOG("NOCARRIER");
				break;
			default:
				QL_VC_LOG("event id = 0x%x",vc_event.id);
				break;
		}
	}
	ql_rtos_task_delete(NULL);
}


QlOSStatus ql_voice_call_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	
	err = ql_rtos_task_create(&vc_task, 4096, APP_PRIORITY_NORMAL, "vc_task", voice_call_demo_task, NULL, 10);
	if(err != QL_OSI_SUCCESS)
	{
		QL_VC_LOG("voice_call_demo_task created failed, ret = 0x%x", err);
	}
	
	return err;
}

