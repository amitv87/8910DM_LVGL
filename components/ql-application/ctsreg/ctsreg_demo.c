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
#include <stdint.h>
#include <time.h>

#include "ql_api_osi.h"
#include "ql_api_nw.h"

#include "ql_log.h"
#include "ql_api_datacall.h"
#include "ql_ctsreg.h"

#define QL_CTSREG_LOG_LEVEL	                QL_LOG_LEVEL_INFO
#define QL_CTSREG_LOG(msg, ...)			    QL_LOG(QL_CTSREG_LOG_LEVEL, "ql_ctsreg_DEMO", msg, ##__VA_ARGS__)
#define QL_CTSREG_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_ctsreg_DEMO", msg, ##__VA_ARGS__)

static ql_task_t ctsreg_task = NULL;

static ql_ctsreg_hndl  ctsreg_cli = 0;
static ql_sem_t  ctsreg_semp;

static ql_ctsreg_param_t  reg_param = {
	{"EC200U\0"},
	{"EC200UR01A01M2G\0"},
	{"89860318145512192783\0"},
	{"861687000001091\0"},
	{"460110934876073\0"},
	{"DD8A30B\0"},
	{"2020 11 20 14:15:15\0"}
};


static ql_ctsreg_option_t reg_option = {
	1, 0, "zzhc.vnet.cn",9999, "\0", 1, 3, 60, 1
};

static void ctsreg_state_cb_fcn(ql_ctsreg_hndl  cli_hndl, int state_code, int finished, void *arg)
{
	QL_CTSREG_LOG("ctsreg state: %d, finished:%d", state_code, finished);
	if(finished){
		ql_rtos_semaphore_release(ctsreg_semp);
		ctsreg_cli = 0;
	}
}

static void ctsreg_app_thread(void * arg)
{
	int ret = 0;
	int i = 0, run_num = 1;
	int profile_idx = 1;
    ql_data_call_info_s info;
	char ip4_addr_str[16] = {0};
	uint8_t sim_id = 0;
	

	ql_rtos_task_sleep_s(10);
	QL_CTSREG_LOG("==========lbs demo start ==========");
	QL_CTSREG_LOG("wait for network register done");

	ql_rtos_semaphore_create(&ctsreg_semp, 0);
	while((ret = ql_network_register_wait(sim_id, 120)) != 0 && i < 10){
    	i++;
		ql_rtos_task_sleep_s(1);
	}
	if(ret == 0){
		i = 0;
		QL_CTSREG_LOG("====network registered!!!!====");
	}else{
		QL_CTSREG_LOG("====network register failure!!!!!====");
		goto exit;
	}

	ql_set_data_call_asyn_mode(sim_id, profile_idx, 0);

	QL_CTSREG_LOG("===start data call====");
	ret=ql_start_data_call(sim_id, profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0); 
	QL_CTSREG_LOG("===data call result:%d", ret);
	if(ret != 0){
		QL_CTSREG_LOG("====data call failure!!!!=====");
	}
	memset(&info, 0x00, sizeof(ql_data_call_info_s));
	
	ret = ql_get_data_call_info(sim_id, profile_idx, &info);
	if(ret != 0){
		QL_CTSREG_LOG("ql_get_data_call_info ret: %d", ret);
		ql_stop_data_call(sim_id, profile_idx);
		goto exit;
	}
    QL_CTSREG_LOG("info->profile_idx: %d", info.profile_idx);
	QL_CTSREG_LOG("info->ip_version: %d", info.ip_version);
            
	QL_CTSREG_LOG("info->v4.state: %d", info.v4.state); 
	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	QL_CTSREG_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_CTSREG_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_CTSREG_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
	
	while(run_num <= 200){
		int error_num = 0;
		
		QL_CTSREG_LOG("==============ctsreg_test[%d]================\n",run_num);

		reg_option.contextid = profile_idx;
		
		ctsreg_cli = ql_ctsreg_perform(&reg_option, &reg_param, ctsreg_state_cb_fcn, NULL, &error_num);
		
		if(ctsreg_cli != 0){
			ql_rtos_semaphore_wait(ctsreg_semp, QL_WAIT_FOREVER);
		}else{
			QL_CTSREG_LOG("ctsreg failed");
		}
		QL_CTSREG_LOG("==============ctsreg_test_end[%d]================\n",run_num);
		run_num++;
		//ql_rtos_task_sleep_s(1);
	}
exit:
   ql_rtos_semaphore_delete(ctsreg_semp);
   ql_rtos_task_delete(ctsreg_task);	
  
   return;	
}


int ql_ctsreg_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	
    err = ql_rtos_task_create(&ctsreg_task, 5*1024, APP_PRIORITY_ABOVE_NORMAL, "ctsreg_app", ctsreg_app_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
    {
		QL_CTSREG_LOG("ctsreg_app init failed");
	}

	return err;
}

