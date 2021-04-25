/**  
  @file
  ql_api_nw.h

  @brief
  This file provides the definitions for nw, and declares the 
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

#ifndef QL_API_NW_H
#define QL_API_NW_H

#include <stdint.h>
#include <stdbool.h>
#include "ql_api_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*========================================================================
 *  Marco Definition
 *========================================================================*/
#define QL_NW_MCC_MAX_LEN 3                 //maxinum length of MCC
#define QL_NW_MNC_MAX_LEN 3                 //maxinum length of MNC
#define QL_NW_LONG_OPER_MAX_LEN 32          //maxinum length of long operator name
#define QL_NW_SHORT_OPER_MAX_LEN 32         //maxinum length of short operator name

#define QL_NW_CELL_MAX_NUM 7                //maxinum number of cell infomation (include serving cell and neighbour cell)

/*========================================================================
 *  Enumeration Definition
 *========================================================================*/

/*network mode type*/
typedef enum
{
    QL_NW_MODE_AUTO = 0,		            //  AUTO mode and LTE preferred
    QL_NW_MODE_GSM,    			            //  GSM only mode
    QL_NW_MODE_LTE,				            //  LTE only mode
}ql_nw_mode_type_e;

/*network access technology type, only support 0 and 7*/
typedef enum 
{
    QL_NW_ACCESS_TECH_GSM                 = 0,
    QL_NW_ACCESS_TECH_GSM_COMPACT         = 1,
    QL_NW_ACCESS_TECH_UTRAN               = 2,
    QL_NW_ACCESS_TECH_GSM_wEGPRS          = 3,
    QL_NW_ACCESS_TECH_UTRAN_wHSDPA        = 4,
    QL_NW_ACCESS_TECH_UTRAN_wHSUPA        = 5,
    QL_NW_ACCESS_TECH_UTRAN_wHSDPA_HSUPA  = 6,
    QL_NW_ACCESS_TECH_E_UTRAN             = 7,
}ql_nw_act_type_e;

/*network register status, equal to creg*/
typedef enum
{
    QL_NW_REG_STATE_NOT_REGISTERED=0,             //not registered, MT is not currently searching an operator to register to
    QL_NW_REG_STATE_HOME_NETWORK=1,               //registered, home network
    QL_NW_REG_STATE_TRYING_ATTACH_OR_SEARCHING=2, //not registered, but MT is currently trying to attach or searching an operator to register to
    QL_NW_REG_STATE_DENIED=3,                     //registration denied
    QL_NW_REG_STATE_UNKNOWN=4,                    //unknown
    QL_NW_REG_STATE_ROAMING=5,                    //registered, roaming
}ql_nw_reg_state_e;

typedef enum
{
	QL_NW_SUCCESS                   = 0,
    QL_NW_EXECUTE_ERR               = 1 | (QL_COMPONENT_NETWORK << 16),
    QL_NW_MEM_ADDR_NULL_ERR,         
    QL_NW_INVALID_PARAM_ERR,
    QL_NW_CFW_CFUN_GET_ERR,
    QL_NW_CFUN_DISABLE_ERR          = 5 | (QL_COMPONENT_NETWORK << 16),
    QL_NW_CFW_NW_STATUS_GET_ERR,
    QL_NW_NOT_SEARCHING_ERR,
    QL_NW_NOT_REGISTERED_ERR,
    QL_NW_CFW_GPRS_STATUS_GET_ERR,
    QL_GPRS_NOT_SEARCHING_ERR     = 10 | (QL_COMPONENT_NETWORK << 16),
    QL_GPRS_NOT_REGISTERED_ERR,
    QL_NW_CFW_NW_QUAL_GET_ERR,
    QL_NW_CFW_OPER_ID_GET_ERR,
    QL_NW_CFW_OPER_NAME_GET_ERR,
    QL_NW_CFW_OPER_SET_ERR        = 15 | (QL_COMPONENT_NETWORK << 16),
    QL_NW_SIM_ERR,
    QL_NW_NO_MEM_ERR,
    QL_NW_SEMAPHORE_CREATE_ERR,
    QL_NW_SEMAPHORE_TIMEOUT_ERR,
    QL_NW_NITZ_NOT_UPDATE_ERR     = 20 | (QL_COMPONENT_NETWORK << 16),
    QL_NW_CFW_EMOD_START_ERR,
}ql_nw_errcode_e;

/*========================================================================
*  Type Definition
*========================================================================*/

/*network operator information*/
typedef struct
{
    char long_oper_name[QL_NW_LONG_OPER_MAX_LEN+1];
    char short_oper_name[QL_NW_SHORT_OPER_MAX_LEN+1];
    char mcc[QL_NW_MCC_MAX_LEN+1];
    char mnc[QL_NW_MNC_MAX_LEN+1];
}ql_nw_operator_info_s;

typedef struct
{
	ql_nw_reg_state_e   state;              //network register state
	int                 lac;                //location area code
	int                 cid;                //cell ID
	ql_nw_act_type_e    act;                //access technology
}ql_nw_common_reg_status_info_s;

typedef struct
{
	ql_nw_common_reg_status_info_s   voice_reg;         //voice register info
	ql_nw_common_reg_status_info_s   data_reg;          //data register info
}ql_nw_reg_status_info_s;

typedef struct
{
    int flag;                               //Cell type, 0:serving, 1:neighbor
    int cid;                                //Cell ID, (0 indicates that the cellid is not received)
    int mcc;                                
    int mnc;                                
    int lac;                                //Location area code
    int arfcn;                              //Absolute RF channel number
    char bsic;                              //Base station identity code. (0 indicates information is not present)
	int rssi;		                        //Receive signal strength, Value range: rxlev-111 for dbm format
	char mnc_len;
	char RX_dBm;                            //Received power
}ql_nw_gsm_cell_info_s;

typedef struct
{
    int flag;                               //Cell type, 0:serving, 1:neighbor
    int cid;                                //Cell ID, (0 indicates that the cellid is not received)
    int mcc;                                
    int mnc;                                
    int tac;                                //Tracing area code
    int pci;                                //Physical cell ID
    int earfcn;                             //E-UTRA absolute radio frequency channel number of the cell. RANGE: 0 TO 65535
	int rssi;		                        //Receive signal strength, Value range: rsrp-140 for dbm format
	char mnc_len;
	char RX_dBm;                            //Received power
}ql_nw_lte_cell_info_s;

typedef struct 
{
    int                     gsm_info_valid;                     //0: GSM cell information is invalid   1: GSM cell information is valid
    int                     gsm_info_num;                       //GSM cell number
    ql_nw_gsm_cell_info_s   gsm_info[QL_NW_CELL_MAX_NUM];       //GSM cell information (Serving and neighbor)
    int                     lte_info_valid;                     //0: LTE cell information is invalid   1: LTE cell information is valid
    int                     lte_info_num;                       //LTE cell number
    ql_nw_lte_cell_info_s   lte_info[QL_NW_CELL_MAX_NUM];       //LTE cell information (Serving and neighbor)
}ql_nw_cell_info_s;

typedef struct 
{
    unsigned char     nw_selection_mode;                        //0 auto select operator；1 manual select operator
    char              mcc[QL_NW_MCC_MAX_LEN+1];                 //String format
    char              mnc[QL_NW_MNC_MAX_LEN+1];                 //String format    eg:China Mobile -----> mcc="460"   mnc="00"
    ql_nw_act_type_e  act;                                      //access technology
}ql_nw_seclection_info_s;

typedef struct
{
    int rssi;                                   // received signal strength level (return 99 indicates that not known or not detectable)
    int bitErrorRate;                           // channel bit error rate (return 99 indicates that not known or not detectable)
    int rsrq;                                   // reference signal received quality (return 255 indicates that not known or not detectable)
    int rsrp;				                    // reference signal received power (return 255 indicates that not known or not detectable) 
}ql_nw_signal_strength_info_s;

typedef struct 
{
  char      nitz_time[32];                      //string format: YY/MM/DD HH:MM:SS '+/-'TZ daylight,   20/09/25 07:40:18 +32 00.
  long      abs_time;                           //Numeric format of NITZ time    0 is unavailable
}ql_nw_nitz_time_info_s;


/*========================================================================
 *	function Definition
 *========================================================================*/

typedef void (*ql_nw_callback)(uint8_t nSim, unsigned int ind_type, void *ctx);

/*****************************************************************
* Function: ql_nw_get_csq
*
* Description: 获取csq信号强度
* 
* Parameters:
*	nSim        [in]    SIM卡索引，取值：0-1.
* 	csq	  		[out] 	返回csq信号强度信息，范围（0-31）,返回 99 为无效值。
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_get_csq(uint8_t nSim, unsigned char *csq);

/*****************************************************************
* Function: ql_nw_set_mode
*
* Description: 配置网络制式
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1.
* 	nw_mode	  		[in] 	需要设置的网络制式，见ql_nw_mode_type_e
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_set_mode(uint8_t nSim, ql_nw_mode_type_e nw_mode);

/*****************************************************************
* Function: ql_nw_get_mode
*
* Description: 获取当前设置的网络制式
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1.
* 	nw_mode	  		[out] 	返回设置的网络制式
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_get_mode(uint8_t nSim, ql_nw_mode_type_e *nw_mode);

/*****************************************************************
* Function: ql_nw_get_operator_name
*
* Description: 获取当前注网的运营商信息，需要等待注网成功后才可获取到
* 
* Parameters:
*	nSim        		[in]    SIM卡索引，取值：0-1
* 	oper_info	  		[out] 	获取运营商信息
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_get_operator_name(uint8_t nSim, ql_nw_operator_info_s *oper_info);

/*****************************************************************
* Function: ql_nw_get_reg_status
*
* Description: 获取当前网络注册信息。
* 
* Parameters:
*	nSim        		[in]    SIM卡索引，取值：0-1
* 	reg_info	  		[out] 	获取当前网络注册信息
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_get_reg_status(uint8_t nSim, ql_nw_reg_status_info_s *reg_info);

/*****************************************************************
* Function: ql_nw_set_selection
*
* Description: 设置运营商的选择，同步API,最长等待120秒，取决于网络
* 
* Parameters:
*	nSim        		[in]    SIM卡索引，取值：0-1
* 	select_info	  		[in] 	选择的运营商信息
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_set_selection(uint8_t nSim, ql_nw_seclection_info_s *select_info);

/*****************************************************************
* Function: ql_nw_set_selection
*
* Description: 获取选择的运营商信息，需要等待注网成功
* 
* Parameters:
*	nSim        		[in]    SIM卡索引，取值：0-1
* 	select_info	  		[out] 	获取选择的运营商信息
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_get_selection(uint8_t nSim, ql_nw_seclection_info_s *select_info);

/*****************************************************************
* Function: ql_nw_get_signal_strength
*
* Description: 获取详细信号强度信息
* 
* Parameters:
*	nSim        	[in]    SIM卡索引，取值：0-1
* 	pt_info	  		[out] 	获取的详细信号强度信息
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_get_signal_strength(uint8_t nSim, ql_nw_signal_strength_info_s *pt_info);

/*****************************************************************
* Function: ql_nw_get_nitz_time_info
*
* Description: 获取当前基站时间，此时间只在注网成功的那一刻更新，需要当地网络支持
* 
* Parameters:
* 	nitz_info	  		[out] 	返回 modem 从基站获取的时间信息
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_get_nitz_time_info(ql_nw_nitz_time_info_s *nitz_info);

/*****************************************************************
* Function: ql_nw_get_cell_info
*
* Description: 获取当前服务及邻近小区信息。同步API,最长等待20秒
* 
* Parameters:
*	nSim        		[in]    SIM卡索引，取值：0-1
* 	cell_info	  		[out] 	返回当前服务及邻近小区信息。
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_get_cell_info(uint8_t nSim, ql_nw_cell_info_s *cell_info);

/*****************************************************************
* Function: ql_nw_register_cb
*
* Description: 注册事件回调函数
* 
* Parameters:
* 	nw_cb	  		[in] 	事件回调函数。
*
* Return:
* 	0			成功。
*	other 	    错误码。
*
*****************************************************************/
ql_nw_errcode_e ql_nw_register_cb(ql_nw_callback nw_cb);


#ifdef __cplusplus
}/*"C" */
#endif

#endif   /*QL_API_NW_H*/


