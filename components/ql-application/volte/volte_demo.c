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
#include "ql_api_volte.h"
#include "ql_log.h"

ql_task_t volte_task = NULL;


void volte_demo_task(void * param)
{
	ql_volte_errcode_e err = QL_VOLTE_SUCCESS;
	
	//China Telecom 4G/5G SIM card must enable VoLTE before making a call 
	err = ql_volte_set(1);
	if(err != QL_VOLTE_SUCCESS){
		QL_VOLTE_LOG("ql_voice_call_volte_set FAIL");
	}else {
		QL_VOLTE_LOG("ql_voice_call_volte_set OK");
	}
	
	//data centric
	err = ql_volte_usage_set(1);
	if(err != QL_VOLTE_SUCCESS){
		QL_VOLTE_LOG("data centric set FAIL,err = %d",err);
	}else{
		QL_VOLTE_LOG("data centric set OK");
	}
	
	//set IMS PS Voice preferred, CS Voice as secondary
	err = ql_volte_vdp_set(3);
	if(err != QL_VOLTE_SUCCESS){
		QL_VOLTE_LOG("set IMS PS Voice preferred FAIL,err=%d",err);
	}else{
		QL_VOLTE_LOG("set IMS PS Voice preferred OK");
	}
	
	ql_rtos_task_delete(NULL);
}


QlOSStatus ql_volte_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	
	err = ql_rtos_task_create(&volte_task, 2048, APP_PRIORITY_NORMAL, "volte_task", volte_demo_task, NULL, 2);
	if(err != QL_OSI_SUCCESS)
	{
		QL_VOLTE_LOG("volte_demo_task created failed, ret = 0x%x", err);
	}
	
	return err;
}

