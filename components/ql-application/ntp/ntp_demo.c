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
#include "ql_ntp_client.h"

#define QL_NTP_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_NTP_LOG(msg, ...)			QL_LOG(QL_NTP_LOG_LEVEL, "ql_NTP_DEMO", msg, ##__VA_ARGS__)
#define QL_NTP_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_NTP_DEMO", msg, ##__VA_ARGS__)

static ql_task_t ntp_task = NULL;

static ntp_client_id  ntp_cli_id = 0;
static ql_sem_t  ntp_semp;

static void ntp_result_cb(ntp_client_id     cli_id, int result, struct tm *sync_time, void *arg)
{
	if(ntp_cli_id != cli_id)
		return;
	
	if(result == QL_NTP_SUCCESS){
		char time_str[256] = {0};

		snprintf(time_str, 256, "%04d/%02d/%02d,%02d:%02d:%02d",sync_time->tm_year + 1900, sync_time->tm_mon + 1, sync_time->tm_mday,
                       sync_time->tm_hour, sync_time->tm_min, sync_time->tm_sec);
		QL_NTP_LOG("ntp sync time:%s", time_str);
	}else{
		QL_NTP_LOG("ntp sync failed :%d", result);
	}
	ql_rtos_semaphore_release(ntp_semp);
}

static void ntp_app_thread(void * arg)
{
	int ret = 0;
	int i = 0, run_num = 1;
	int profile_idx = 1;
    ql_data_call_info_s info;
	char ip4_addr_str[16] = {0};
	uint8_t nSim = 0;

	ql_rtos_task_sleep_s(10);
	QL_NTP_LOG("==========ntp demo start ==========");
	QL_NTP_LOG("wait for network register done");

	ql_rtos_semaphore_create(&ntp_semp, 0);
	while((ret = ql_network_register_wait(nSim, 120)) != 0 && i < 10){
    	i++;
		ql_rtos_task_sleep_s(1);
	}
	if(ret == 0){
		i = 0;
		QL_NTP_LOG("====network registered!!!!====");
	}else{
		QL_NTP_LOG("====network register failure!!!!!====");
		goto exit;
	}

	ql_set_data_call_asyn_mode(nSim, profile_idx, 0);

	QL_NTP_LOG("===start data call====");
	ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0); 
	QL_NTP_LOG("===data call result:%d", ret);
	if(ret != 0){
		QL_NTP_LOG("====data call failure!!!!=====");
	}
	memset(&info, 0x00, sizeof(ql_data_call_info_s));
	
	ret = ql_get_data_call_info(nSim, profile_idx, &info);
	if(ret != 0){
		QL_NTP_LOG("ql_get_data_call_info ret: %d", ret);
		ql_stop_data_call(nSim, profile_idx);
		goto exit;
	}
    QL_NTP_LOG("info->profile_idx: %d", info.profile_idx);
	QL_NTP_LOG("info->ip_version: %d", info.ip_version);
            
	QL_NTP_LOG("info->v4.state: %d", info.v4.state); 
	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	QL_NTP_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_NTP_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_NTP_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
	
	while(run_num <= 100){
		ql_ntp_sync_option  sync_option;
		int error_num = 0;
		QL_NTP_LOG("==============ntp_test[%d]================\n",run_num);

		memset(&sync_option, 0x00, sizeof(ql_ntp_sync_option));
		sync_option.pdp_cid = profile_idx;
		sync_option.sim_id = 0;
		sync_option.retry_cnt = 3;
		sync_option.retry_interval_tm = 60;

		ntp_cli_id = ql_ntp_sync("ntp.aliyun.com", &sync_option, ntp_result_cb, NULL, &error_num);
		
		if(ntp_cli_id != 0){
			ql_rtos_semaphore_wait(ntp_semp, QL_WAIT_FOREVER);
		}else{
			QL_NTP_LOG("ntp failed");
		}
		QL_NTP_LOG("==============ntp_test_end[%d]================\n",run_num);
		run_num++;
		ql_rtos_task_sleep_s(1);
	}
	
exit:
   ql_rtos_semaphore_delete(ntp_semp);
   ql_rtos_task_delete(ntp_task);	

   return;	
}


int ql_ntp_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	
    err = ql_rtos_task_create(&ntp_task, 16*1024, 23, "ntp_app", ntp_app_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
    {
		QL_NTP_LOG("ntp_app init failed");
	}

	return err;
}

