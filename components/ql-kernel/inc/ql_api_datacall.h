/**  
  @file
  ql_api_datacall.h

  @brief
  This file provides the definitions for datacall, and declares the 
  API functions.

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

=============================================================================*/

#ifndef QL_API_DATACALL_H
#define QL_API_DATACALL_H

#include <stdint.h>
#include <stdbool.h>
#include "sockets.h"
#include "ql_api_common.h"

#ifdef __cplusplus
extern "C" {
#endif


/*========================================================================
 *  Marco Definition
 *========================================================================*/
#define QL_PDP_TYPE_IP                  1           
#define QL_PDP_TYPE_IPV6                2           
#define QL_PDP_TYPE_IPV4V6              3           

#define PROFILE_IDX_MIN                 1
#define PROFILE_IDX_VOLTE_MAX           5
#define PROFILE_IDX_MAX                 7

#define APN_LEN_MAX                     64
#define USERNAME_LEN_MAX                64
#define PASSWORD_LEN_MAX                64

#define PROFILE_IDX_NUM                 (PROFILE_IDX_MAX - PROFILE_IDX_MIN + 1 )

#define QL_PDP_DEACTIVED                0 
#define QL_PDP_ACTIVED                  1  

#define QL_DATACALL_ERRCODE_BASE        (QL_COMPONENT_NETWORK_MANAGE<<16)
/*========================================================================
 *  Enumeration Definition
 *========================================================================*/

typedef enum
{
    QL_PDP_AUTH_TYPE_NONE = 0,		            //no authentication protocol is used for this PDP context. Username and password are removed if previously specified
    QL_PDP_AUTH_TYPE_PAP,    			        
    QL_PDP_AUTH_TYPE_CHAP,				            
}ql_pdp_auth_type_e;

typedef enum
{
	QL_DATACALL_SUCCESS                     = 0,
    QL_DATACALL_EXECUTE_ERR                 = 1 | QL_DATACALL_ERRCODE_BASE,  
    QL_DATACALL_MEM_ADDR_NULL_ERR,         
    QL_DATACALL_INVALID_PARAM_ERR,
    QL_DATACALL_NW_REGISTER_TIMEOUT_ERR,
    QL_DATACALL_CFW_ACT_STATE_GET_ERR       = 5 | QL_DATACALL_ERRCODE_BASE,
    QL_DATACALL_REPEAT_ACTIVE_ERR,
    QL_DATACALL_REPEAT_DEACTIVE_ERR,
    QL_DATACALL_CFW_PDP_CTX_SET_ERR,
    QL_DATACALL_CFW_PDP_CTX_GET_ERR,
    QL_DATACALL_CS_CALL_ERR                 = 10 | QL_DATACALL_ERRCODE_BASE,
    QL_DATACALL_CFW_CFUN_GET_ERR,
    QL_DATACALL_CFUN_DISABLE_ERR,
    QL_DATACALL_NW_STATUS_GET_ERR,
    QL_DATACALL_NOT_REGISTERED_ERR,
    QL_DATACALL_NO_MEM_ERR                  = 15 | QL_DATACALL_ERRCODE_BASE,
    QL_DATACALL_CFW_ATTACH_STATUS_GET_ERR,
    QL_DATACALL_SEMAPHORE_CREATE_ERR,
    QL_DATACALL_SEMAPHORE_TIMEOUT_ERR,
    QL_DATACALL_CFW_ATTACH_REQUEST_ERR,
    QL_DATACALL_CFW_ACTIVE_REQUEST_ERR      = 20 | QL_DATACALL_ERRCODE_BASE,
    QL_DATACALL_ACTIVE_FAIL_ERR,
    QL_DATACALL_CFW_DEACTIVE_REQUEST_ERR,
}ql_datacall_errcode_e;

/*========================================================================
*  Type Definition
*========================================================================*/

struct v4_address_status
{
	ip4_addr_t ip;
	ip4_addr_t pri_dns;
	ip4_addr_t sec_dns;
};

struct v4_info
{
	int state;                          //dial status
	struct v4_address_status addr;      //IPv4 address information
};

struct v6_address_status
{
	ip6_addr_t  ip;
	ip6_addr_t  pri_dns;
	ip6_addr_t  sec_dns;
};

struct v6_info
{
	int state;                          //dial status
	struct v6_address_status addr;      //IPv6 address information
};

typedef struct
{
	int profile_idx;
	int ip_version;
	struct v4_info v4;
	struct v6_info v6;
}ql_data_call_info_s;

/*========================================================================
 *	function Definition
 *========================================================================*/

/*****************************************************************
* Description: datacall callback
* 
* Parameters:
*   ind_type        [in]    事件类型
* 	profile_idx	    [in] 	PDP索引 
* 	result	  		[in] 	执行结果
*
*****************************************************************/
typedef void (*ql_datacall_callback)(uint8_t nSim, unsigned int ind_type, int profile_idx, bool result, void *ctx);

/*****************************************************************
* Function: ql_network_register_wait
*
* Description:
* 	等待网络注册 
* 
* Parameters:
*   nSim					[in] 	SIM卡索引，取值0-1。
* 	timeout_s	  			[in] 	超时时间，单位秒。 
*
* Return:
* 	0			注册成功。
*	other 	    错误码。
*
*****************************************************************/
ql_datacall_errcode_e ql_network_register_wait(uint8_t nSim, unsigned int timeout_s);


/*****************************************************************
* Function: ql_set_data_call_asyn_mode
*
* Description:
* 	设置ql_start_data_call和ql_stop_data_call接口的执行模式，并设置异步模式的回调函数， 
* 
* Parameters:
*   nSim				[in] 	SIM卡索引，取值0-1。
*   deactive_cb         [in]    PDP索引，取值1-7。 
* 	enable	  			[in] 	1：使能异步模式；0：不使能异步模式 
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_datacall_errcode_e ql_set_data_call_asyn_mode(uint8_t nSim, int profile_idx, bool enable);


/*****************************************************************
* Function: ql_start_data_call
*
* Description:
*	启动拨号。默认为同步模式，可由ql_set_data_call_asyn_mode设置为异步模式
* 
* Parameters:
*   nSim				[in] 	SIM卡索引，取值0-1。
* 	profile_idx	  		[in] 	PDP索引，取值1-7。 
* 	ip_version	  		[in] 	IP类型：1 为IPv4，2 为IPv6，3 为IPv4v6。
* 	apn_name	  		[in] 	apn名称。
* 	username	  		[in] 	用户名称。
* 	password	  		[in] 	用户密码。
* 	auth_type	  		[in] 	认证类型: 0 NONE
*										  1 PAP
*										  2 CHAP
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_datacall_errcode_e ql_start_data_call(uint8_t nSim, int profile_idx, int ip_version, char *apn_name, char *username, char *password, int auth_type);

/*****************************************************************
* Function: ql_get_data_call_info
*
* Description:
* 	获取拨号信息。 
* 
* Parameters:
*   nSim				[in] 	SIM卡索引，取值0-1。
* 	profile_idx	  		[in] 	PDP索引，取值1-7。 
*	info				[out] 	获取拨号信息。 
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_datacall_errcode_e ql_get_data_call_info(uint8_t nSim, int profile_idx, ql_data_call_info_s *info);

/*****************************************************************
* Function: ql_stop_data_call
*
* Description:
* 	关闭拨号。 默认为同步模式，可由ql_set_data_call_asyn_mode设置为异步模式
* 
* Parameters:
*   nSim				[in] 	SIM卡索引，取值0-1。
* 	profile_idx	  		[in] 	PDP索引，取值1-7。 
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_datacall_errcode_e ql_stop_data_call(uint8_t nSim, int profile_idx);

/*****************************************************************
* Function: ql_datacall_register_cb
*
* Description: 注册数据拨号的回调函数
* 
* Parameters:
*   nSim			[in] 	SIM卡索引，取值0-1。
*   profile_idx     [in]    PDP索引，取值1-7。 
*   datacall_cb     [in]    需要注册的回调函数
*   ctx             [in]    回调函数的参数
*
* Return:
* 	0			注册成功。
*	other 	    错误码。
*
*****************************************************************/
ql_datacall_errcode_e ql_datacall_register_cb(uint8_t nSim, int profile_idx, ql_datacall_callback datacall_cb, void *ctx);

/*****************************************************************
* Function: ql_datacall_unregister_cb
*
* Description: 取消已注册的数据拨号回调函数
* 
* Parameters:
*   nSim			[in] 	SIM卡索引，取值0-1。
*   profile_idx     [in]    PDP索引，取值1-7。 
*   datacall_cb     [in]    需要取消注册的回调函数
*   ctx             [in]    回调函数的参数
*
* Return:
* 	0			取消注册成功。
*	other 	    错误码。
*
*****************************************************************/
ql_datacall_errcode_e ql_datacall_unregister_cb(uint8_t nSim, int profile_idx, ql_datacall_callback datacall_cb, void *ctx);

/*****************************************************************
* Function: ql_datacall_get_sim_profile_is_active
*
* Description: 获取当前PDP是否激活
* 
* Parameters:
*   nSim			[in] 	SIM卡索引，取值0-1。
*   profile_idx     [in]    PDP索引，取值1-7。 
*
* Return:
* 	0			未激活
*	1 	        已激活
*
*****************************************************************/
bool ql_datacall_get_sim_profile_is_active(uint8_t nSim, int profile_idx);

#ifdef __cplusplus
}/*"C" */
#endif

#endif   /*QL_API_DATACALL_H*/

