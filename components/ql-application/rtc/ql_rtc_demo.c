/**  
  @file
  ql_rtc_demo.c

  @brief
  quectel rtc demo.

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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_api_rtc.h"
#include "ql_rtc_demo.h"


/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_RTCDEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_RTCDEMO_LOG(msg, ...)         QL_LOG(QL_RTCDEMO_LOG_LEVEL, "ql_RTCDEMO", msg, ##__VA_ARGS__)
#define QL_RTCDEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("ql_RTCDEMO", msg, ##__VA_ARGS__)


/*========================================================================
 *  function Definition
 *========================================================================*/
void ql_rtc_test_callback(void)
{
    QL_RTCDEMO_LOG("ql rtc test callback come in!");
    
    ql_rtc_time_t test_tm = {0}; 
    
    //disable RTC alarm
    //ql_rtc_enable_alarm(0);
    
    //get alarm time
    QL_RTCDEMO_LOG("=========callback print alarm time===========\r\n");
    ql_rtc_get_alarm(&test_tm);
    ql_rtc_print_time(test_tm);
}

static void ql_rtc_demo_thread()
{
    QL_RTCDEMO_LOG("ql rtc demo enter! \n");

    ql_rtc_time_t tm;

    //get current time
    ql_rtc_get_time(&tm);   
    ql_rtc_print_time(tm);

    //2020-12-22 16:50:30 [WED]
    tm.tm_year = 2020;
    tm.tm_mon  = 12;
    tm.tm_mday = 22;
    tm.tm_wday = 2;
    tm.tm_hour = 16;
    tm.tm_min  = 50;
    tm.tm_sec  = 30;
    ql_rtc_print_time(tm);

    //set time
    ql_rtc_set_time(&tm);

    ql_rtc_get_time(&tm);
    ql_rtc_print_time(tm);

    //set & enable alarm
 	tm.tm_sec += 20;
	ql_rtc_set_alarm(&tm);
    ql_rtc_register_cb(ql_rtc_test_callback);
	ql_rtc_enable_alarm(1); 
    
	while (1) 
    { 
		ql_rtc_get_time(&tm);
		QL_RTCDEMO_LOG("=========print current time===========\r\n");
		ql_rtc_print_time(tm);
		ql_rtos_task_sleep_s(5);
	}
}

void ql_rtc_app_init()
{
    QlOSStatus err = QL_OSI_SUCCESS;
    ql_task_t rtc_task = NULL;

    err = ql_rtos_task_create(&rtc_task, 1024, APP_PRIORITY_NORMAL, "ql_rtcdemo", ql_rtc_demo_thread, NULL, 1);
    if( err != QL_OSI_SUCCESS )
    {
        QL_RTCDEMO_LOG("rtc demo task created failed");
    }
}



