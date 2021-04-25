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

=============================================================================*/

#ifndef QL_HTTPLCLIENT_H
#define QL_HTTPLCLIENT_H

typedef int http_client_t;

typedef void(*http_client_event_cb_t)(http_client_t *client, int evt, int evt_code, void *arg);

typedef int(*http_client_write_data_cb_t)(http_client_t *client, void *arg, char *data, int size, unsigned char end);

typedef int(*http_client_read_data_cb_t)(http_client_t *client, void *arg, char *data, int size);


typedef enum{
	HTTPS_VERIFY_NONE          = 0,
	HTTPS_VERIFY_SERVER        = 1,
	HTTPS_VERIFY_SERVER_CLIENT = 2,			
}https_verify_level_e;
typedef enum{
	HTTP_EVENT_SESSION_ESTABLISH           = 0,
	HTTP_EVENT_RESPONE_STATE_LINE          = 1,
	HTTP_EVENT_SESSION_DISCONNECT          = 2,
}http_event_id_e;

typedef enum{
	HTTP_SUCCESS                  = 0,
	HTTP_ERR_UNKOWN               = -1,
	HTTP_ERR_WOUNDBLOCK           = -2,
	HTTP_ERR_INVALID_PARAM        = -3,
	HTTP_ERR_OUT_OF_MEM           = -4,
	HTTP_ERR_BUSY            	  = -5,
	HTTP_ERR_BAD_REQUEST   		  = -6,
	HTTP_ERR_SOCKE_CONNECT_FAIL   = -7,
	HTTP_ERR_DNS_FAIL             = -8,
	HTTP_ERR_SOCKET_EXCEPTION     = -9,
	HTTP_ERR_TIMEOUT              = -10,
}http_error_code_e;

typedef enum {
  HTTP_METHOD_NONE, 
  HTTP_METHOD_GET,
  HTTP_METHOD_POST,
  HTTP_METHOD_PUT,
  HTTP_METHOD_HEAD,
  HTTP_METHOD_LAST
} http_method_e;
  

typedef enum{
	HTTP_CLIENT_OPT_PDPCID = 1,
	HTTP_CLIENT_OPT_SSLCTXID,
	HTTP_CLIENT_OPT_BASIC_AUTH,
	HTTP_CLIENT_OPT_REQUEST_HEADER,
	HTTP_CLIENT_OPT_WRITE_HEADER,
	HTTP_CLIENT_OPT_INTERVAL_TIME,
	HTTP_CLIENT_OPT_METHOD,
	HTTP_CLIENT_OPT_WRITE_FUNC,
	HTTP_CLIENT_OPT_WRITE_DATA,
	HTTP_CLIENT_OPT_READ_FUNC,
	HTTP_CLIENT_OPT_READ_DATA,
	HTTP_CLIENT_OPT_UPLOAD_LEN,
	HTTP_CLIENT_OPT_URL,
	HTTP_CLIENT_OPT_SSL_VERIFY_LEVEL,
	HTTP_CLIENT_OPT_SSL_CACERT_PATH,
	HTTP_CLIENT_OPT_SSL_OWNCERT_PATH,	
	HTTP_CLIENT_OPT_URI,
}http_option_e;


typedef enum{
	HTTP_INFO_RESPONSE_CODE = 0,
	HTTP_INFO_LOCATION,
	HTTP_INFO_DATE,
	HTTP_INFO_CONTENT_LEN,
	HTTP_INFO_CHUNK_ENCODE,
}http_info_e;


typedef enum{
	HTTP_FORM_NAME = 1,
	HTTP_FORM_FILENAME = 2,
	HTTP_FORM_CONTENT_TYPE = 3,
}http_formopt_e;

/*****************************************************************
* Function: ql_httpc_new
*
* Description: 创建并初始化http_client上下文结构
* 
* Parameters:
*	client        [out]    用于回传http client上下文指针.
*   cb            [in]     该http client上下文事件回调处理函数.
*   arg           [in]     回调处理函数的自变量参数.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_httpc_new(http_client_t *client, http_client_event_cb_t cb, void *arg);

/*****************************************************************
* Function: ql_httpc_setopt
*
* Description: 通过opt_tag类型信息,配置http client上下文
* 
* Parameters:
*	client        [in]    http client上下文指针.
*   opt_tag       [in]    http client上下文中配置的参数处理类型
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_httpc_setopt(http_client_t *client, int opt_tag, ...);

/*****************************************************************
* Function: ql_httpc_formadd
*
* Description: 添加http的头部字段
* 
* Parameters:
*	client        [in]    http client上下文指针.
*   opt_tag       [in]    http client上下文中配置添加http头部字段类型参数的处理.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_httpc_formadd(http_client_t *client, int opt_tag, ...);

/*****************************************************************
* Function: ql_httpc_perform
*
* Description: 关联http client此上下文和http的读写回调函数
* 
* Parameters:
*	client        [in]    http client上下文指针.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_httpc_perform(http_client_t *client);

/*****************************************************************
* Function: ql_httpc_getinfo
*
* Description: 获取已配置的http client上下文的配置信息
* 
* Parameters:
*	client        [in]    http client上下文指针.
*   info          [in]    http client上下文中配置的参数类型.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_httpc_getinfo(http_client_t  *client, int info,...);

/*****************************************************************
* Function: ql_httpc_release
*
* Description: 释放http client上下文,以及http client上下文所占用内存块
* 
* Parameters:
*	client        [in]    http client上下文指针.
*
* Return:
* 	0           成功
*	other 	    错误码
*
*****************************************************************/
int  ql_httpc_release(http_client_t *client);

#endif

