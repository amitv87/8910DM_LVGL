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

#ifndef QL_API_SIM_H
#define QL_API_SIM_H


#include "ql_api_common.h"


#ifdef __cplusplus
extern "C" {
#endif

/*========================================================================
 *  Marco Definition
 *========================================================================*/
#define QL_SIM_ERRCODE_BASE (QL_COMPONENT_SIM<<16)

#define QL_SIM_PIN_LEN_MAX  8          // Maximum length of PIN data. 
#define QL_SIM_NUM_MAX  	2



/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
//SIM componant detail errcode, 4 bytes
typedef enum
{
    QL_SIM_SUCCESS                          = 0,
    QL_SIM_EXECUTE_ERR                      = 1|QL_SIM_ERRCODE_BASE,
    QL_SIM_MEM_ADDR_NULL_ERR,
    QL_SIM_INVALID_PARAM_ERR,
    QL_SIM_NO_MEMORY_ERR,
    QL_SIM_SEMAPHORE_CREATE_ERR             = 5|QL_SIM_ERRCODE_BASE,
    QL_SIM_SEMAPHORE_TIMEOUT_ERR,
    QL_SIM_NOT_INSERTED_ERR,
    QL_SIM_CFW_IMSI_GET_REQUEST_ERR,
    QL_SIM_CFW_IMSI_GET_RSP_ERR,
    QL_SIM_CFW_ICCID_GET_REQUEST_ERR        = 10|QL_SIM_ERRCODE_BASE,
    QL_SIM_CFW_PHONE_NUM_GET_REQUEST_ERR,
    QL_SIM_CFW_PHONE_NUM_GET_RSP_NULL_ERR,
    QL_SIM_CFW_STATUS_GET_REQUEST_ERR,
    QL_SIM_CFW_PIN_ENABLE_REQUEST_ERR,
    QL_SIM_CFW_PIN_DISABLE_REQUEST_ERR      = 15|QL_SIM_ERRCODE_BASE,
    QL_SIM_CFW_PIN_VERIFY_REQUEST_ERR,
    QL_SIM_CFW_PIN_CHANGE_REQUEST_ERR,
    QL_SIM_CFW_PIN_UNBLOCK_REQUEST_ERR,
} ql_sim_errcode_e;


typedef enum 
{
    QL_SIM_STATUS_READY                 = 0,
    QL_SIM_STATUS_PIN1_READY,
    QL_SIM_STATUS_SIMPIN,
    QL_SIM_STATUS_SIMPUK,
    QL_SIM_STATUS_PHONE_TO_SIMPIN,
    QL_SIM_STATUS_PHONE_TO_FIRST_SIMPIN = 5,
    QL_SIM_STATUS_PHONE_TO_FIRST_SIMPUK,
    QL_SIM_STATUS_SIMPIN2,
    QL_SIM_STATUS_SIMPUK2,
    QL_SIM_STATUS_NETWORKPIN,
    QL_SIM_STATUS_NETWORKPUK            = 10,
    QL_SIM_STATUS_NETWORK_SUBSETPIN,
    QL_SIM_STATUS_NETWORK_SUBSETPUK,
    QL_SIM_STATUS_PROVIDERPIN,
    QL_SIM_STATUS_PROVIDERPUK,
    QL_SIM_STATUS_CORPORATEPIN          = 15,
    QL_SIM_STATUS_CORPORATEPUK,
    QL_SIM_STATUS_NOSIM,
    QL_SIM_STATUS_PIN1BLOCK,
    QL_SIM_STATUS_PIN2BLOCK,
    QL_SIM_STATUS_PIN1_DISABLE          = 20,
    QL_SIM_STATUS_SIM_PRESENT,
    QL_SIM_STATUS_UNKNOW,
}ql_sim_status_e;

/*========================================================================
*  Type Definition
*========================================================================*/

typedef struct 
{
    uint8_t                     pin_value[QL_SIM_PIN_LEN_MAX+1];      /*  Value of the PIN */
}ql_sim_verify_pin_info_s;  

typedef struct 
{
    uint8_t                     old_pin_value[QL_SIM_PIN_LEN_MAX+1];  /**< Value of the old PIN as a sequence of ASCII characters. */
    uint8_t                     new_pin_value[QL_SIM_PIN_LEN_MAX+1];  /**< Value of the new PIN as a sequence of ASCII characters. */
}ql_sim_change_pin_info_s; 

typedef struct 
{
    uint8_t                     puk_value[QL_SIM_PIN_LEN_MAX+1];      /**< Value of the PUK as a sequence of ASCII characters. */
    uint8_t                     new_pin_value[QL_SIM_PIN_LEN_MAX+1];  /**< Value of the new PIN as a sequence of ASCII characters. */
}ql_sim_unblock_pin_info_s;  

/*========================================================================
 *	function Definition
 *========================================================================*/
 
/*****************************************************************
* Function: ql_sim_get_imsi
*
* Description:
* 	获取 SIM 卡的 IMSI。
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1
* 	imsi	  		[out] 		SIM 卡的 IMSI，字符串。  
* 	imsi_len	  	[in] 		imsi 数组的长度。
*
* Return:
* 	QL_SIM_SUCCESS			成功。
*	other           		错误码。
*
*****************************************************************/
ql_sim_errcode_e ql_sim_get_imsi(uint8_t nSim, char *imsi, size_t imsi_len);

/*****************************************************************
* Function: ql_sim_get_iccid
*
* Description:
* 	获取 SIM 卡的 ICCID。
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1
* 	iccid	  		[out] 		SIM 卡的 ICCID，字符串。 
* 	iccid_len	  	[in] 		iccid 数组的长度。  
*
* Return:
* 	QL_SIM_SUCCESS			成功。
*	other           		错误码。
*
*****************************************************************/
ql_sim_errcode_e ql_sim_get_iccid(uint8_t nSim, char *iccid, size_t iccid_len);

/*****************************************************************
* Function: ql_sim_get_phonenumber
*
* Description:
* 	获取 SIM 卡本机号码。 
* 
* Parameters:
*	nSim        		[in]    SIM卡索引，取值：0-1
* 	phonenumber	  		[out] 	SIM 卡本机号码，字符串。  
* 	phonenumber_len	  	[in] 	phonenumber 数组的长度。  
*
* Return:
* 	QL_SIM_SUCCESS			成功。
*	other           		错误码。
*
*****************************************************************/
ql_sim_errcode_e ql_sim_get_phonenumber(uint8_t nSim, char *phonenumber,size_t phonenumber_len);

/*****************************************************************
* Function: ql_sim_get_card_status
*
* Description:
* 	获取 SIM 卡状态信息。
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1
* 	card_status	  		[out] 	返回 SIM 卡类型信息，见 ql_sim_status_e 
*
* Return:
* 	QL_SIM_SUCCESS				成功。
*	other           		    错误码。
*
*****************************************************************/
ql_sim_errcode_e ql_sim_get_card_status(uint8_t nSim, ql_sim_status_e *card_status);

/*****************************************************************
* Function: ql_sim_enable_pin
*
* Description:
* 	启用 SIM 卡 PIN 码验证，重启生效。
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1
* 	pt_info	  		[in] 	输入 PIN 码，见 ql_sim_verify_pin_info_s
*
* Return:
* 	QL_SIM_SUCCESS				成功。
*	other           		    错误码。
*
*****************************************************************/
ql_sim_errcode_e ql_sim_enable_pin(uint8_t nSim, ql_sim_verify_pin_info_s *pt_info);

/*****************************************************************
* Function: ql_sim_disable_pin
*
* Description:
* 	关闭 SIM 卡 PIN 码验证。
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1
* 	pt_info	  		[in] 	输入 PIN 码，见 ql_sim_verify_pin_info_s
*
* Return:
* 	QL_SIM_SUCCESS				成功。
*	other           		    错误码。
*
*****************************************************************/
ql_sim_errcode_e ql_sim_disable_pin(uint8_t nSim, ql_sim_verify_pin_info_s *pt_info);

/*****************************************************************
* Function: ql_sim_verify_pin
*
* Description:
* 	当 SIM 状态为请求 PIN/PIN2，输入 PIN/PIN2 码，进行验证。
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1
* 	pt_info	  		[in] 	输入 PIN 码，见 ql_sim_verify_pin_info_s
*
* Return:
* 	QL_SIM_SUCCESS				成功。
*	other           		    错误码。
*
*****************************************************************/
ql_sim_errcode_e ql_sim_verify_pin(uint8_t nSim, ql_sim_verify_pin_info_s *pt_info);

/*****************************************************************
* Function: ql_sim_change_pin
*
* Description:
* 	启用 SIM 卡 PIN 码验证后，更改 SIM 卡的 PIN 码。
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1
* 	pt_info	  		[in] 	输入旧的 PIN 和新的 PIN，见 ql_sim_change_pin_info_s
*
* Return:
* 	QL_SIM_SUCCESS				成功。
*	other           		    错误码。
*
*****************************************************************/
ql_sim_errcode_e ql_sim_change_pin(uint8_t nSim, ql_sim_change_pin_info_s *pt_info);

/*****************************************************************
* Function: ql_sim_change_pin
*
* Description:
* 	当多次错误输入 PIN/PIN2 码后，SIM 卡状态为请求 PUK/PUK2 时，输入 PUK/PUK2 码和新的PIN/PIN2 码进行解锁,
*   解PUK锁成功后，SIM卡会自动启用PIN 码验证，用户需再次调用ql_sim_disable_pin()关闭 SIM 卡 PIN 码验证。
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1
* 	pt_info	  		[in] 	输入 PUK/PUK2 码和新的 PIN/PIN2 码，见 ql_sim_unblock_pin_info_s
*
* Return:
* 	QL_SIM_SUCCESS				成功。
*	other           		    错误码。
*
*****************************************************************/
ql_sim_errcode_e ql_sim_unblock_pin(uint8_t nSim, ql_sim_unblock_pin_info_s *pt_info);

/*****************************************************************
* Function: ql_sim_set_operate_id
*
* Description:
* 	设置需要操作的SIM卡索引号,默认值为0。
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1
*
* Return:
* 	QL_SIM_SUCCESS				成功。
*	other           		    错误码。
*
*****************************************************************/
ql_sim_errcode_e ql_sim_set_operate_id(uint8_t nSim);

/*****************************************************************
* Function: ql_sim_get_operate_id
*
* Description:
* 	获取当前操作的SIM卡索引号。
* 
* Parameters:
*	NULL
*
* Return:
* 	SIM ID。
*
*****************************************************************/
uint8_t ql_sim_get_operate_id(void);


#ifdef __cplusplus
}/*"C" */
#endif

#endif   /*QL_API_SIM_H*/

