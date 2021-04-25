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
10/10/2020  marvin          create

=================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_common.h"
#include "ql_api_osi.h"
#include "ql_api_sms.h"
#include "ql_log.h"

ql_task_t sms_task = NULL;
ql_sem_t  sms_init_sem = NULL;
ql_sem_t  sms_list_sem = NULL;

void user_sms_event_callback(int event_id, void *ctx)
{
	switch(event_id)
	{
		case QL_SMS_INIT_OK_IND:
			QL_SMS_LOG("QL_SMS_INIT_OK_IND");
			ql_rtos_semaphore_release(sms_init_sem);
			break;
		case QL_SMS_NEW_MSG_IND:
			QL_SMS_LOG("QL_SMS_NEW_MSG_IND, index = %d", *(uint16_t *)ctx);
			break;
		case QL_SMS_LIST_IND:
		{
			ql_sms_msg_s *msg = (ql_sms_msg_s *)ctx;
			QL_SMS_LOG("index=%d, msg = %s",msg->index, msg->buf);
			break;
		}
		case QL_SMS_LIST_END_IND:
			QL_SMS_LOG("QL_SMS_LIST_END_IND");
			ql_rtos_semaphore_release(sms_list_sem);
			break;
		default :
			break;
	}
}


void sms_demo_task(void * param)
{
	char addr[20] = {0};
	QL_SMS_LOG("enter");
	ql_sms_callback_register(user_sms_event_callback);
	
	//wait sms ok
	if(ql_rtos_semaphore_wait(sms_init_sem, QL_WAIT_FOREVER)){
		QL_SMS_LOG("Waiting for SMS init timeout");
	}
	
	if(QL_SMS_SUCCESS == ql_sms_get_center_address(addr, sizeof(addr))){
		QL_SMS_LOG("ql_sms_get_center_address OK, addr=%s",addr);
	}else{
		QL_SMS_LOG("ql_sms_get_center_address FAIL");
	}
	
	//Send English text message
	#if 0
	if(QL_SMS_SUCCESS == ql_sms_send_msg("+8618924012349","Hello,marvin", GSM)){
		QL_SMS_LOG("send sms OK");
	}else{
		QL_SMS_LOG("send sms FAIL");
	}
	//Send messages in Chinese and English. (Need use UTF8 encoding to open sms_demo.c for chinese.)
	if(QL_SMS_SUCCESS == ql_sms_send_msg("+8618924012349","hello,你好", UCS2)){
		QL_SMS_LOG("send pdu sms OK");
	}else{
		QL_SMS_LOG("send pdu sms FAIL");
	}
	#endif
	
	//Get how many SMS messages can be stored in the SIM card in total and how much storage is used
	#if 1
	ql_sms_stor_info_s stor_info;
	if(QL_SMS_SUCCESS == ql_sms_get_storage_info(&stor_info)){
		QL_SMS_LOG("ql_sms_get_storage_info OK");
		QL_SMS_LOG("SM used=%u,SM total=%u,SM unread=%u,ME used=%u,ME total=%u,ME unread=%u, newSmsStorId=%u",
			stor_info.usedSlotSM,stor_info.totalSlotSM,stor_info.unReadRecordsSM,
			stor_info.usedSlotME,stor_info.totalSlotME,stor_info.unReadRecordsME,
			stor_info.newSmsStorId);
	}else{
		QL_SMS_LOG("ql_sms_get_storage_info FAIL");
	}
	#endif
	
	
	//Read one messages in SIM
	#if 1
	uint16_t msg_len = 512;
	char *msg = malloc(msg_len);
	if(msg == NULL){
		QL_SMS_LOG("malloc ql_sms_msg_s fail");
		goto exit;
	}
	memset(msg ,0 ,msg_len);
	//The first parameter specifies that SMS messages are read from SM
	ql_sms_set_storage(SM,SM,SM);
	
	//Read SMS messages as text
	if(QL_SMS_SUCCESS == ql_sms_read_msg(1, msg, msg_len, TEXT)){
		QL_SMS_LOG("read msg OK, msg=%s", msg);
	}else{
		QL_SMS_LOG("read sms FAIL");
	}
	//Read SMS messages as pdu
	memset(msg ,0 ,msg_len);
	if(QL_SMS_SUCCESS == ql_sms_read_msg(1, msg, msg_len, PDU)){
		QL_SMS_LOG("read msg OK, msg=%s", msg);
	}else{
		QL_SMS_LOG("read sms FAIL");
	}
	if(msg)free(msg);
	#endif


	//Read all message in SIM
	#if 1
	ql_sms_set_storage(SM, SM, SM);//set sms storage as SIM.
	if(QL_SMS_SUCCESS == ql_sms_read_msg_list(TEXT)){
		if(ql_rtos_semaphore_wait(sms_list_sem, QL_WAIT_FOREVER)){
			QL_SMS_LOG("sms_list_sem time out");
		}
	}else{
		QL_SMS_LOG("get msg list FAIL");
	}
	#endif

	
	//Delete message.
	#if 0
	if(QL_SMS_SUCCESS == ql_sms_delete_msg(14)){
		QL_SMS_LOG("delete msg OK");
	}else{
		QL_SMS_LOG("delete sms FAIL");
	}
	#endif
	
exit:

	ql_rtos_task_delete(NULL);
}


QlOSStatus ql_sms_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;

	err = ql_rtos_task_create(&sms_task, 4096, APP_PRIORITY_NORMAL, "SMS_TASK", sms_demo_task, NULL, 2);
	if(err != QL_OSI_SUCCESS)
	{
		QL_SMS_LOG("sms_task created failed, ret = 0x%x", err);
	}
	
	err = ql_rtos_semaphore_create(&sms_init_sem, 0);
	if(err != QL_OSI_SUCCESS)
	{
		QL_SMS_LOG("sms_init_sem created failed, ret = 0x%x", err);
	}

	err = ql_rtos_semaphore_create(&sms_list_sem, 0);
	if(err != QL_OSI_SUCCESS)
	{
		QL_SMS_LOG("sms_init_sem created failed, ret = 0x%x", err);
	}

	return err;
}

