/**  
  @file
  ql_sms.h

  @brief
  This file provides the definitions for sms API functions.

*/
/*============================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------
            marvin          create

=============================================================================*/

#ifndef QL_API_SMS_H
#define QL_API_SMS_H


#include <stdint.h>
#include <stdbool.h>
#include "sockets.h"
#include "ql_api_common.h"
#include "ql_api_osi.h"


#ifdef __cplusplus
 extern "C" {
#endif



 /*========================================================================
  *  Marco Definition
  *========================================================================*/

#define QL_SMS_LOG_LEVEL		QL_LOG_LEVEL_INFO
#define QL_SMS_LOG(msg, ...)	QL_LOG(QL_SMS_LOG_LEVEL, "ql_sms", msg, ##__VA_ARGS__)


#define map_size(map) (sizeof(map) / sizeof(struct map_node))

#define QL_SMS_MIN_LEN                1       
#define MAX_LONGSMS_SEGMENT           8
#define MAX_SMS_LENGTH                (160*4)
#define SMS_BUF_LEN                   (MAX_SMS_LENGTH + 32)
#define MAX_LONGSMS_LENGTH            (160*4*MAX_LONGSMS_SEGMENT)


 /*========================================================================
  *  Enumeration Definition
  *========================================================================*/

 typedef enum
 {
	 QL_SMS_SUCCESS					 = 0,
	 QL_SMS_ERROR		             = 1 | (QL_COMPONENT_SMS << 16),
	 QL_SMS_NOT_INIT_ERR,
	 QL_SMS_PARA_ERR,
	 QL_SMS_NO_MEMORY_ERR,
	 QL_SMS_SEM_CREATE_ERR,
	 QL_SMS_SEM_TIMEOUT_ERR,    
	 QL_SMS_NO_MSG_ERR,
 }ql_sms_errcode_e;


typedef enum
{
	QL_SMS_INIT_OK_IND = 1| (QL_COMPONENT_SMS << 16),
	QL_SMS_NEW_MSG_IND,
	QL_SMS_LIST_IND,
	QL_SMS_LIST_END_IND,
}ql_sms_event_id_e;


typedef enum
{
	ME = 1, //Mobile Equipment message storage
	SM = 2,//SIM message storage
}ql_sms_stor_e;

typedef enum
{
	GSM = 0,
	UCS2 = 1,
}ql_sms_code_e;

typedef enum
{
	PDU = 0,
	TEXT = 1,
}ql_sms_format_e;

typedef enum
{
	QL_SMS_UNREAD = 0,
	QL_SMS_READ = 1,
	QL_SMS_UNSENT = 2,
	QL_SMS_SENT = 3,
	QL_SMS_ALL,
}ql_sms_status_e;

 /*========================================================================
 *	Struct Definition
 *========================================================================*/
  typedef struct{
     uint8_t status;
	 uint8_t index;
	 char* buf;
	 uint16_t buf_len;
 }ql_sms_msg_s;

 
 typedef struct
 {
	 ql_sms_errcode_e err;
	 uint16_t uti;
	 ql_sms_msg_s *msg;
	 ql_sem_t wait_sema;
 }ql_sms_ctx_s;

 typedef struct
 {
	 uint16_t usedSlotSM;
	 uint16_t totalSlotSM;
	 uint16_t unReadRecordsSM;
	 uint16_t usedSlotME;
	 uint16_t totalSlotME;
	 uint16_t unReadRecordsME;
	 ql_sms_stor_e  newSmsStorId;
 }ql_sms_stor_info_s;



 /*========================================================================
  *  Callback Definition
  *========================================================================*/

 typedef void (*ql_sms_event_handler_t)(int event_id, void *ctx);
 extern ql_sms_event_handler_t ql_sms_callback;


 /*========================================================================
  *  function Definition
  *========================================================================*/


/*****************************************************************
* Function: ql_sms_send_msg
*
* Description:
* 	??????????????????????????????
* 
* Parameters:
* 	phone_num	  [in] 	????????????????????? 
*	data		  [in] 	?????????????????? 
*   code          [in]  ??????????????????,?????????GSM??????UCS2
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_send_msg(char *phone_num, char *data, ql_sms_code_e code);

/*****************************************************************
* Function: ql_sms_send_pdu
*
* Description:
* 	??????PDU??????????????????
* 
* Parameters:
*	pdu			 [in] 	??????????????????
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_send_pdu(char *pdu);

/*****************************************************************
* Function: ql_sms_get_msg_list
*
* Description:
* 	?????????????????????
* 
* Parameters:
*	status		[in] 	??????????????????
*   format      [in]    PDU??????TEXT??????
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_read_msg_list(ql_sms_format_e format);

/*****************************************************************
* Function: ql_sms_read_msg
*
* Description:
* 	?????????????????????????????????
* 
* Parameters:
*	index		[in] 	?????????????????????
*   msg         [out]   ??????????????????,????????????,???????????????????????????,????????????
*   format      [format]??????TEXT??????PDU??????????????????
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_read_msg(uint8_t index, char *buf, uint16_t buf_len, ql_sms_format_e format);

/*****************************************************************
* Function: ql_sms_delete_msg
*
* Description:
* 	??????????????????
* 
* Parameters:
*	index		[in] 	?????????????????????
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_delete_msg(uint8_t index);

/*****************************************************************
* Function: ql_sms_get_center_address
*
* Description:
* 	???????????????????????????
* 
* Parameters:
*	address		[out] 	????????????????????????
*   len         [in]    ??????????????????????????????????????????.
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_get_center_address(char* address, uint8_t len);

/*****************************************************************
* Function: ql_sms_set_center_address
*
* Description:
* 	???????????????????????????
* 
* Parameters:
*	address		[in] 	????????????????????????
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_set_center_address(char* address);

/*****************************************************************
* Function: ql_sms_get_storage_info
*
* Description:
* 	??????????????????
* 
* Parameters:
*	stor_info		[out] 	??????SIM??????????????????????????????????????????,
                            ?????????ql_sms_stor_info_s????????????
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_get_storage_info(ql_sms_stor_info_s *stor_info);

/*****************************************************************
* Function: ql_sms_set_storage
*
* Description:
* 	????????????????????????
* 
* Parameters:
*	mem1		[in] 	??????????????????????????????????????????
*	mem2		[in] 	???????????????????????????????????????
*	mem3		[in] 	??????????????????????????????
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_set_storage(ql_sms_stor_e mem1,ql_sms_stor_e mem2, ql_sms_stor_e mem3);

/*****************************************************************
* Function: ql_sms_get_storage_info
*
* Description:
* 	?????????????????????????????????
* 
* Parameters:
*	status		[out] 	?????????????????????
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_get_init_status(uint8_t *status);

/*****************************************************************
* Function: ql_sms_get_storage_info
*
* Description:
* 	????????????????????????
* 
* Parameters:
*	status		[out] 	?????????????????????
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
void ql_sms_callback_register(ql_sms_event_handler_t cb);




#ifdef __cplusplus
 }/*"C" */
#endif

#endif /*QL_API_SMS_H*/



