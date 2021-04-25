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
* 	发送文本格式的短消息
* 
* Parameters:
* 	phone_num	  [in] 	目的手机号码。 
*	data		  [in] 	短消息数据。 
*   code          [in]  数据编码格式,可取值GSM或者UCS2
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_send_msg(char *phone_num, char *data, ql_sms_code_e code);

/*****************************************************************
* Function: ql_sms_send_pdu
*
* Description:
* 	发送PDU格式的短消息
* 
* Parameters:
*	pdu			 [in] 	短消息数据。
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_send_pdu(char *pdu);

/*****************************************************************
* Function: ql_sms_get_msg_list
*
* Description:
* 	获取短消息列表
* 
* Parameters:
*	status		[in] 	短消息数据。
*   format      [in]    PDU或者TEXT格式
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_read_msg_list(ql_sms_format_e format);

/*****************************************************************
* Function: ql_sms_read_msg
*
* Description:
* 	文本方式读取单条短消息
* 
* Parameters:
*	index		[in] 	短消息序列号。
*   msg         [out]   包含消息状态,消息时间,消息发送方手机号码,消息内容
*   format      [format]使用TEXT或者PDU格式读取消息
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_read_msg(uint8_t index, char *buf, uint16_t buf_len, ql_sms_format_e format);

/*****************************************************************
* Function: ql_sms_delete_msg
*
* Description:
* 	删除单条消息
* 
* Parameters:
*	index		[in] 	短消息序列号。
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_delete_msg(uint8_t index);

/*****************************************************************
* Function: ql_sms_get_center_address
*
* Description:
* 	获取短消息中心号码
* 
* Parameters:
*	address		[out] 	短消息中心号码。
*   len         [in]    接收短消息中心号码的数组长度.
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_get_center_address(char* address, uint8_t len);

/*****************************************************************
* Function: ql_sms_set_center_address
*
* Description:
* 	设置短消息中心号码
* 
* Parameters:
*	address		[in] 	短消息中心号码。
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_set_center_address(char* address);

/*****************************************************************
* Function: ql_sms_get_storage_info
*
* Description:
* 	获取存储信息
* 
* Parameters:
*	stor_info		[out] 	包含SIM卡与设备中保存的短信存储状态,
                            具体见ql_sms_stor_info_s结构体。
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_get_storage_info(ql_sms_stor_info_s *stor_info);

/*****************************************************************
* Function: ql_sms_set_storage
*
* Description:
* 	获取短信存储状态
* 
* Parameters:
*	mem1		[in] 	读取和删除消息所在内存空间。
*	mem2		[in] 	写入和发送消息的内存空间。
*	mem3		[in] 	接收消息的存储位置。
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_set_storage(ql_sms_stor_e mem1,ql_sms_stor_e mem2, ql_sms_stor_e mem3);

/*****************************************************************
* Function: ql_sms_get_storage_info
*
* Description:
* 	获取短信模块初始化状态
* 
* Parameters:
*	status		[out] 	短信初始化状态
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
ql_sms_errcode_e ql_sms_get_init_status(uint8_t *status);

/*****************************************************************
* Function: ql_sms_get_storage_info
*
* Description:
* 	注册短信回调函数
* 
* Parameters:
*	status		[out] 	短信初始化状态
*
* Return:ql_sms_errcode_e
*
*****************************************************************/
void ql_sms_callback_register(ql_sms_event_handler_t cb);




#ifdef __cplusplus
 }/*"C" */
#endif

#endif /*QL_API_SMS_H*/



