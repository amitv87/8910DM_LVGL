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
#include "ql_lbs_client.h"

#define QL_LBS_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_LBS_LOG(msg, ...)			QL_LOG(QL_LBS_LOG_LEVEL, "ql_LBS_DEMO", msg, ##__VA_ARGS__)
#define QL_LBS_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_LBS_DEMO", msg, ##__VA_ARGS__)

static ql_task_t lbs_task = NULL;

static lbs_client_hndl  lbs_cli = 0;
static ql_sem_t  lbs_semp;


static lbs_basic_info_t  basic_info = {
     .type = 1,
     .encrypt = 1,
     .key_index = 1,
     .pos_format = 1,
     .loc_method = 4
};

static lbs_auth_info_t  auth_info = {
     .user_name = "quectel",
     .user_pwd = "123456",
     .token = "1111111122222222",//"A693EDC90C42E624",
     .imei = "861687000001091",
     .rand = 2346
};

static lbs_cell_info_t lbs_cell_info[] = {
    {
      .radio = 3,
      .mcc = 460,
      .mnc = 0,
      .lac_id = 0x550B,
      .cell_id = 0xF2D4A48,
      .signal = 0,
      .tac = 3,
      .bcch = 0,
      .bsic = 0,
      .uarfcndl = 0,
      .psc = 0,
      .rsrq = 0,
      .pci = 0,
      .earfcn = 0
    }
};

static void lbs_result_cb(lbs_client_hndl hndl, int result, int pos_num, lbs_postion_info_t  *pos_info, char *date)
{
	int i = 0;
	if(lbs_cli != hndl)
		return;

	QL_LBS_LOG("lbs result: %d", result);
	if(result == LBS_RES_OK){
		for(i = 0; i < pos_num; i++){
			QL_LBS_LOG("Location[%d]: %f, %f, %d\n", i, pos_info[i].longitude, pos_info[i].latitude, pos_info[i].accuracy);
		}
	}
	ql_rtos_semaphore_release(lbs_semp);
}

static void lbs_app_thread(void * arg)
{
	int ret = 0;
	int i = 0, run_num = 1;
	int profile_idx = 1;
    ql_data_call_info_s info;
	char ip4_addr_str[16] = {0};
	uint8_t nSim = 0;
    ql_nw_cell_info_s cell_info;
    ql_nw_seclection_info_s select_info;

	ql_rtos_task_sleep_s(10);
	QL_LBS_LOG("==========lbs demo start ==========");
	QL_LBS_LOG("wait for network register done");

	ql_rtos_semaphore_create(&lbs_semp, 0);
	while((ret = ql_network_register_wait(nSim, 120)) != 0 && i < 10){
    	i++;
		ql_rtos_task_sleep_s(1);
	}
	if(ret == 0){
		i = 0;
		QL_LBS_LOG("====network registered!!!!====");
	}else{
		QL_LBS_LOG("====network register failure!!!!!====");
		goto exit;
	}

	ql_set_data_call_asyn_mode(nSim, profile_idx, 0);

	QL_LBS_LOG("===start data call====");
	ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0); 
	QL_LBS_LOG("===data call result:%d", ret);
	if(ret != 0){
		QL_LBS_LOG("====data call failure!!!!=====");
	}
	memset(&info, 0x00, sizeof(ql_data_call_info_s));
	
	ret = ql_get_data_call_info(nSim, profile_idx, &info);
	if(ret != 0){
		QL_LBS_LOG("ql_get_data_call_info ret: %d", ret);
		ql_stop_data_call(nSim, profile_idx);
		goto exit;
	}
    QL_LBS_LOG("info->profile_idx: %d", info.profile_idx);
	QL_LBS_LOG("info->ip_version: %d", info.ip_version);
            
	QL_LBS_LOG("info->v4.state: %d", info.v4.state); 
	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	QL_LBS_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_LBS_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_LBS_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
	
	while(run_num <= 100){
		lbs_option_t  user_option;
		int error_num = 0;
		QL_LBS_LOG("==============lbs_test[%d]================\n",run_num);

        if(ql_nw_get_cell_info(nSim, &cell_info)!=QL_NW_SUCCESS)
        {
            QL_LBS_LOG("===============lbs get cell info fail===============\n");
            continue;
        }
        ret = ql_nw_get_selection(nSim, &select_info);	
        if(ret != 0){
    		QL_LBS_LOG("ql_nw_get_selection ret: %d", ret);
    		goto exit;
	    }
        QL_LBS_LOG("nw_act_type=%d",select_info.act);
        if(select_info.act==QL_NW_ACCESS_TECH_GSM)
        {
            lbs_cell_info[0].radio=1;
            lbs_cell_info[0].mcc = cell_info.gsm_info[0].mcc;
            lbs_cell_info[0].mnc = cell_info.gsm_info[0].mnc;
            lbs_cell_info[0].cell_id = cell_info.gsm_info[0].cid;
            lbs_cell_info[0].lac_id = cell_info.gsm_info[0].lac;
            lbs_cell_info[0].bsic = cell_info.gsm_info[0].bsic;
            lbs_cell_info[0].uarfcndl = cell_info.gsm_info[0].arfcn;
            lbs_cell_info[0].bcch = cell_info.gsm_info[0].arfcn;
            lbs_cell_info[0].signal=cell_info.gsm_info[0].rssi;
        }
        else if(select_info.act==QL_NW_ACCESS_TECH_E_UTRAN)
        {
            lbs_cell_info[0].radio=3;
            lbs_cell_info[0].mcc = cell_info.lte_info[0].mcc;
            lbs_cell_info[0].mnc = cell_info.lte_info[0].mnc;
            lbs_cell_info[0].cell_id = cell_info.lte_info[0].cid;
            lbs_cell_info[0].lac_id = cell_info.lte_info[0].tac;
            lbs_cell_info[0].tac = cell_info.lte_info[0].tac;
            lbs_cell_info[0].pci = cell_info.lte_info[0].pci;
            lbs_cell_info[0].earfcn = cell_info.lte_info[0].earfcn;
            lbs_cell_info[0].bcch = cell_info.lte_info[0].earfcn;
            lbs_cell_info[0].signal=cell_info.lte_info[0].rssi;
        }
        else
        {
            goto exit;
        }
        QL_LBS_LOG("cell infoinfo: radio=%d,mcc=%d,mnc=%d,lac_id=%x,cell_id=%x,signal=%d,tac =%x,bcch=%d,bsic=%d,uarfcndl=%d,psc=%d,rsrq=%d,pci=%d,earfcn=%d",\
        lbs_cell_info[0].radio,lbs_cell_info[0].mcc,lbs_cell_info[0].mnc,lbs_cell_info[0].lac_id,lbs_cell_info[0].cell_id,lbs_cell_info[0].signal,lbs_cell_info[0].tac,\
        lbs_cell_info[0].bcch,lbs_cell_info[0].bsic,lbs_cell_info[0].uarfcndl,lbs_cell_info[0].psc,lbs_cell_info[0].rsrq,lbs_cell_info[0].pci,lbs_cell_info[0].earfcn);

		memset(&user_option, 0x00, sizeof(lbs_option_t));
		user_option.pdp_cid = profile_idx;
		user_option.sim_id = 0;
		user_option.req_timeout = 60;
		user_option.basic_info = &basic_info;
		user_option.auth_info = &auth_info;
		user_option.cell_num = 1;
		user_option.cell_info = &lbs_cell_info[0];

		lbs_cli = ql_lbs_get_position("www.queclocator.com", &user_option, lbs_result_cb, NULL, &error_num);
		
		if(lbs_cli != 0){
			ql_rtos_semaphore_wait(lbs_semp, QL_WAIT_FOREVER);
		}else{
			QL_LBS_LOG("lbs failed");
		}
		QL_LBS_LOG("==============lbs_test_end[%d]================\n",run_num);
		run_num++;
		ql_rtos_task_sleep_s(1);
	}
	
exit:
   ql_rtos_semaphore_delete(lbs_semp);
   ql_rtos_task_delete(lbs_task);	

   return;	
}


int ql_lbs_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	
    err = ql_rtos_task_create(&lbs_task, 16*1024, 23, "lbs_app", lbs_app_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
    {
		QL_LBS_LOG("lbs_app init failed");
	}

	return err;
}

