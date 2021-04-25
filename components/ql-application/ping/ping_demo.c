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

#include "ql_api_osi.h"
#include "ql_api_nw.h"

#include "ql_log.h"
#include "ql_api_datacall.h"
#include "ql_ping_app.h"

#define QL_PING_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_PING_LOG(msg, ...)			QL_LOG(QL_PING_LOG_LEVEL, "ql_PING_DEMO", msg, ##__VA_ARGS__)
#define QL_PING_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_PING_DEMO", msg, ##__VA_ARGS__)

static ql_task_t ping_task = NULL;

static ping_session_id  ping_sess_id = 0;
static ql_sem_t  ping_semp;

static void ping_event_cb(ping_session_id session_id, uint16_t event_id, int evt_code, void *evt_param)
{
	if(session_id != ping_sess_id)
		return;
	
	switch(event_id){
	case QL_PING_STATS:{
			if(evt_code == QL_PING_OK){
				ql_ping_stats_type *stats  = (ql_ping_stats_type *)evt_param;
				QL_PING_LOG("from \"%s\" 's reply: bystes=%d, rtt=%d, ttl=%d", stats->resolved_ip_addr, stats->ping_size, stats->ping_rtt, stats->ping_ttl);
			}
		}
		break;
	case QL_PING_SUMMARY:{
			if(evt_code == QL_PING_OK){
				ql_ping_summary_type *summary = (ql_ping_summary_type *)evt_param;
				QL_PING_LOG("statistics info: send=%dpackets, recv=%dpackets, lost=%dpackets, max_rtt=%d,min_rtt=%d,avg_rtt=%d",
				            summary->num_pkts_sent, summary->num_pkts_recvd, summary->num_pkts_lost,
				            summary->max_rtt, summary->min_rtt,summary->avg_rtt);
			}else{
				QL_PING_LOG("ping timeout!!!!!");
			}
			ping_sess_id = 0;
			ql_rtos_semaphore_release(ping_semp);
		}
		break;
	default:
		break;	
	}
}

static void ping_app_thread(void * arg)
{
	int ret = 0;
	int i = 0, run_num = 1;
	int profile_idx = 1;
    ql_data_call_info_s info;
	char ip4_addr_str[16] = {0};
	uint8_t nSim = 0;

	ql_rtos_task_sleep_s(10);
	QL_PING_LOG("==========ping demo start ==========");
	QL_PING_LOG("wait for network register done");

	ql_rtos_semaphore_create(&ping_semp, 0);
	while((ret = ql_network_register_wait(nSim, 120)) != 0 && i < 10){
    	i++;
		ql_rtos_task_sleep_s(1);
	}
	if(ret == 0){
		i = 0;
		QL_PING_LOG("====network registered!!!!====");
	}else{
		QL_PING_LOG("====network register failure!!!!!====");
		goto exit;
	}

	ql_set_data_call_asyn_mode(nSim, profile_idx, 0);

	QL_PING_LOG("===start data call====");
	ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0); 
	QL_PING_LOG("===data call result:%d", ret);
	if(ret != 0){
		QL_PING_LOG("====data call failure!!!!=====");
	}
	memset(&info, 0x00, sizeof(ql_data_call_info_s));
	
	ret = ql_get_data_call_info(nSim, profile_idx, &info);
	if(ret != 0){
		QL_PING_LOG("ql_get_data_call_info ret: %d", ret);
		ql_stop_data_call(nSim, profile_idx);
		goto exit;
	}
    QL_PING_LOG("info->profile_idx: %d", info.profile_idx);
	QL_PING_LOG("info->ip_version: %d", info.ip_version);
            
	QL_PING_LOG("info->v4.state: %d", info.v4.state); 
	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	QL_PING_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_PING_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_PING_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
	
	while(run_num < 100){
		int case_id = run_num%5;
		ql_ping_config_type  ping_option;

		memset(&ping_option, 0x00, sizeof(ql_ping_config_type));
		
		QL_PING_LOG("==============ping_test[%d]================\n",run_num+1);
		
		if(case_id == 1){
			ping_option.num_data_bytes = 56;
			ping_option.num_pings = 10;
		    ping_option.ping_response_time_out = 10;
			ping_option.ttl = 255;

			ping_sess_id = ql_ping_start(profile_idx, 0, "www.baidu.com", &ping_option, ping_event_cb);
			
		}else if(case_id == 2){
			ping_option.num_data_bytes = 48;
			ping_option.num_pings = 15;
		    ping_option.ping_response_time_out = 10;
			ping_option.ttl = 255;

			ping_sess_id = ql_ping_start(profile_idx, 0, "www.163.com", &ping_option, ping_event_cb);
					
		}else if(case_id == 3){
			ping_option.num_data_bytes = 56;
			ping_option.num_pings = 4;
		    ping_option.ping_response_time_out = 10;
			ping_option.ttl = 255;

			ping_sess_id = ql_ping_start(profile_idx, 0, "www.hao123.com", &ping_option, ping_event_cb);
		}else if(case_id == 4){
			ping_option.num_data_bytes = 56;
			ping_option.num_pings = 8;
		    ping_option.ping_response_time_out = 10;
			ping_option.ttl = 255;

			ping_sess_id = ql_ping_start(profile_idx, 0, "www.sina.com.cn", &ping_option, ping_event_cb);
		}else{
			ping_option.num_data_bytes = 48;
			ping_option.num_pings = 12;
		    ping_option.ping_response_time_out = 10;
			ping_option.ttl = 255;

			ping_sess_id = ql_ping_start(profile_idx, 0, "www.jianshu.com", &ping_option, ping_event_cb);
		}
		if(ping_sess_id != 0){
			ql_rtos_semaphore_wait(ping_semp, QL_WAIT_FOREVER);
		}else{
			QL_PING_LOG("ping failed");
		}
		QL_PING_LOG("==============ping_test_end[%d]================\n",run_num+1);
		run_num++;
		ql_rtos_task_sleep_s(1);
	}
	
exit:
   ql_rtos_semaphore_delete(ping_semp);
   ql_rtos_task_delete(ping_task);	

   return;	
}


int ql_ping_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	
    err = ql_rtos_task_create(&ping_task, 16*1024, 23, "ping_app", ping_app_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
    {
		QL_PING_LOG("ping_app init failed");
	}

	return err;
}

