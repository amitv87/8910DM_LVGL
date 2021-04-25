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
#include "ql_fs.h"
#include "ql_log.h"
#include "ql_api_datacall.h"
#include "ql_mms_client.h"

#define QL_MMS_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_MMS_LOG(msg, ...)			QL_LOG(QL_MMS_LOG_LEVEL, "ql_MMS_DEMO", msg, ##__VA_ARGS__)
#define QL_MMS_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_MMS_DEMO", msg, ##__VA_ARGS__)

static ql_task_t mms_task = NULL;

static ql_mms_client  mms_cli_hndl = 0;
static ql_sem_t  mms_semp;

static void mms_result_cb(ql_mms_client        client_hndl, int mms_result, int http_code, char *mms_err_msg, void *arg)
{
	if(mms_cli_hndl != client_hndl)
		return;
	
	QL_MMS_LOG("mms send result: %d,http_code:%d", mms_result, http_code);
	
	ql_rtos_semaphore_release(mms_semp);
}

static void mms_app_thread(void * arg)
{
	int ret = 0;
	int i = 0, run_num = 1;
	int profile_idx = 2;
    ql_data_call_info_s info;
	char ip4_addr_str[16] = {0};
	uint8_t nSim = 0;
	QFILE fd;

	ql_rtos_task_sleep_s(20);
	QL_MMS_LOG("==========mms demo start ==========");
	QL_MMS_LOG("wait for network register done");

	ql_rtos_semaphore_create(&mms_semp, 0);
	while((ret = ql_network_register_wait(nSim, 120)) != 0 && i < 10){
    	i++;
		ql_rtos_task_sleep_s(1);
	}
	if(ret == 0){
		i = 0;
		QL_MMS_LOG("====network registered!!!!====");
	}else{
		QL_MMS_LOG("====network register failure!!!!!====");
		goto exit;
	}

	ql_set_data_call_asyn_mode(nSim, profile_idx, 0);

	QL_MMS_LOG("===start data call====");
	ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "ctwap", NULL, NULL, 0); 
	QL_MMS_LOG("===data call result:%d", ret);
	
	if(ret != 0){
		QL_MMS_LOG("====data call failure!!!!=====");
	}
	memset(&info, 0x00, sizeof(ql_data_call_info_s));
	
	ret = ql_get_data_call_info(nSim, profile_idx, &info);
	if(ret != 0){
		QL_MMS_LOG("ql_get_data_call_info ret: %d", ret);
		ql_stop_data_call(nSim, profile_idx);
		goto exit;
	}
	
    QL_MMS_LOG("info->profile_idx: %d", info.profile_idx);
	QL_MMS_LOG("info->ip_version: %d", info.ip_version);
            
	QL_MMS_LOG("info->v4.state: %d", info.v4.state); 
	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	QL_MMS_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_MMS_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_MMS_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);

	fd = ql_fopen("UFS:test.txt","w+");
	ql_fwrite("123456789",9,1,fd);
	ql_fclose(fd);
	ret = ql_mms_client_new(&mms_cli_hndl, mms_result_cb, NULL);
	QL_MMS_LOG("creat mms client ret:%d",ret);

	if(ret != QL_MMS_OK){
		goto exit;
	}
	while(run_num <= 20){
		struct mms_option_t  send_option;
		struct mms_info_t    msg_info;
		struct mms_subject_t mms_subject;
		struct mms_addr_t    to_addr[2];
		struct mms_addr_t    cc_addr[2];
		struct mms_addr_t    bcc_addr[2];
		struct mms_attachment_t attachment;
		
		QL_MMS_LOG("==============mms_test[%d]================\n",run_num);

		memset(&send_option, 0x00, sizeof(struct mms_option_t));
		memset(&msg_info, 0x00, sizeof(struct mms_info_t));
		memset(&mms_subject, 0x00, sizeof(struct mms_subject_t));
		memset(&to_addr[0], 0x00, sizeof(struct mms_addr_t)*2);
		memset(&cc_addr[0], 0x00, sizeof(struct mms_addr_t)*2);
		memset(&bcc_addr[0], 0x00, sizeof(struct mms_addr_t)*2);
		memset(&attachment, 0x00, sizeof(struct mms_attachment_t));
		
		send_option.charset = MMS_CHARSET_ASCII_VALUE;
		send_option.wait_timeout = 60;
		send_option.context_id = profile_idx;
		send_option.sim_id = 0;
		send_option.mmsc_url = "http://mmsc.vnet.mobi";
		send_option.proxy_addr = "10.0.0.200";
		send_option.proxy_port = 80;
		send_option.send_param.class = MMS_PARAM_CLASS_DEFAULT;
		send_option.send_param.validity_period = MMS_PARAM_VALID_DEFAULT;
		send_option.send_param.priority = MMS_PARAM_PRIORITY_HIGHEST;
		send_option.send_param.delivery_report = 0;
		send_option.send_param.read_report = 0;
		send_option.send_param.visible = MMS_PARAM_SENDERADDR_DEFAULT;
		send_option.supportfield = 0;

		mms_subject.subject = "test mms";
		mms_subject.length = strlen("test mms");
		msg_info.subject = &mms_subject;

		to_addr[0].address = "18225658934";
		to_addr[0].addr_len = strlen("18225658934");
		to_addr[0].next =NULL;
		msg_info.to_addr = &(to_addr[0]);

		attachment.file_path = "UFS:test.txt";
		attachment.next = NULL;
		msg_info.attachments = &attachment;

		if(ql_mms_client_send_msg(&mms_cli_hndl, &send_option, &msg_info) == QL_MMS_OK){
			QL_MMS_LOG("===wait mms send complete=====");
			ql_rtos_semaphore_wait(mms_semp, QL_WAIT_FOREVER);
		}
	
		
		QL_MMS_LOG("==============mms_test_end[%d]================\n",run_num);
		run_num++;
		ql_rtos_task_sleep_s(1);
	}
	ql_mms_client_release(&mms_cli_hndl);
exit:
   ql_stop_data_call(nSim, profile_idx);
   ql_rtos_semaphore_delete(mms_semp);
   ql_rtos_task_delete(mms_task);	

   return;	
}


int ql_mms_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	
    err = ql_rtos_task_create(&mms_task, 16*1024, 23, "mms_app", mms_app_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
    {
		QL_MMS_LOG("mms_app init failed");
	}

	return err;
}

