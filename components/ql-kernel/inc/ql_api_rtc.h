/**  @file
  ql_api_rtc.h

  @brief
  This file is used to define rtc interface for different Quectel Project.

*/

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
16/12/2020        Neo         Init version
=================================================================*/


#ifndef _QL_API_RTC_H_
#define _QL_API_RTC_H_

#include "ql_api_common.h"

#ifdef __cplusplus
	 extern "C" {
#endif


/*===========================================================================
 * Macro Definition
 *===========================================================================*/
#define QL_RTC_ERRCODE_BASE (QL_COMPONENT_BSP_RTC<<16)


/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
/**
 * rtc errcode
 */ 
typedef enum
{
    QL_RTC_SUCCESS = QL_SUCCESS,

    QL_RTC_INVALID_PARAM_ERR                 = 1|QL_RTC_ERRCODE_BASE,
    QL_RTC_SET_TIME_ERROR,
    QL_RTC_SET_CB_ERR,
    QL_RTC_ENABLE_ALARM_ERR,
    QL_RTC_REMOVE_ALARM_ERR
}ql_errcode_rtc_e;


/*========================================================================
 *  Type Definition
 *========================================================================*/
//time[2000-01-01 00:00:00-----2100-01-01 00:00:00]
typedef struct ql_rtc_time_struct {
	int tm_sec;	 // seconds [0,59]
	int tm_min;	 // minutes [0,59]
	int tm_hour; // hour [0,23]
	int tm_mday; // day of month [1,31]
	int tm_mon;	 // month of year [1,12]
	int tm_year; // year [2000-2100] 
	int tm_wday; // wday [0-6],sunday = 0,this value has no effect when setting the time
}ql_rtc_time_t;


/*===========================================================================
 * Functions declaration
 ===========================================================================*/
/*****************************************************************
* Description: RTC 回调函数，用于接收alarm通知
* 
* Parameters:
*     无  
*
*****************************************************************/
typedef void (*ql_rtc_cb)(void);

/*****************************************************************
* Function: ql_rtc_set_time
*
* Description: 设置rtc时间
* 
* Parameters:
*   tm	        [in] 	时间结构体 
*
* Return:
*   0           设置时间成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_rtc_e ql_rtc_set_time(ql_rtc_time_t *tm);

/*****************************************************************
* Function: ql_rtc_get_time
*
* Description: 获取rtc时间
* 
* Parameters:
*   tm	        [out] 	时间结构体 
*
* Return:
*   0           获取时间成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_rtc_e ql_rtc_get_time(ql_rtc_time_t *tm);

/*****************************************************************
* Function: ql_rtc_print_time
*
* Description: 打印rtc时间
* 
* Parameters:
*   tm	        [in] 	时间结构体 
*
* Return:
*   0           打印时间成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_rtc_e ql_rtc_print_time(ql_rtc_time_t tm);

/*****************************************************************
* Function: ql_rtc_set_alarm
*
* Description: 设置rtc alarm时间
* 
* Parameters:
*   tm	        [in] 	时间结构体 
*
* Return:
*   0           设置alarm时间成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_rtc_e ql_rtc_set_alarm(ql_rtc_time_t *tm);

/*****************************************************************
* Function: ql_rtc_get_alarm
*
* Description: 获取rtc alarm时间
* 
* Parameters:
*   tm	        [out] 	时间结构体 
*
* Return:
*   0           获取alarm时间成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_rtc_e ql_rtc_get_alarm(ql_rtc_time_t *tm);

/*****************************************************************
* Function: ql_rtc_enable_alarm
*
* Description: 打开和关闭rtc alarm
* 
* Parameters:
*   on_off	    [in]    开关, 取值1和0, 1表示打开, 0表示关闭 
*
* Return:
*   0           设置alarm时间成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_rtc_e ql_rtc_enable_alarm(unsigned char on_off);

/*****************************************************************
* Function: ql_rtc_register_cb
*
* Description: 注册rtc alarm 回调函数
* 
* Parameters:
*   cb	        [in] 	alarm回调函数 
*
* Return:
*   0           注册成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_rtc_e ql_rtc_register_cb(ql_rtc_cb cb);




#ifdef __cplusplus
	 } /*"C" */
#endif
 
#endif /* _QL_API_RTC_H_ */


