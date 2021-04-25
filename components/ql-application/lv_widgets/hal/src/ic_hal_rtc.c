/// @file ic_hal.c
/// @Synopsis:  hal layer adaptor for mt2523
/// @author Wang Hua (whfyzg@gmail.com))
/// @version V1.0
/// @date 2020-11-14

#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "ic_hal_rtc.h"
#include "ql_api_rtc.h"
/*******************************************************
 *
 * rtc function layer
 ******************************************************/
bool ic_hal_rtc_init(void)
{
   
   
   

    return true;
}

bool ic_hal_rtc_deinit(void)
{
    return true;
}


bool ic_hal_rtc_get_time(ic_hal_rtc_t *rtc)
{
   ql_rtc_time_t tm;
   //get current time
   ql_rtc_get_time(&tm);   
   ql_rtc_print_time(tm);

    rtc->year   =   tm.tm_year;
    rtc->mon    =   tm.tm_mon;
    rtc->day    =   tm.tm_mday;
    rtc->hour   =   tm.tm_hour;
    rtc->min    =   tm.tm_min;
    rtc->sec    =   tm.tm_sec;
    rtc->week   =   tm.tm_wday;


    return true;
}

bool ic_hal_rtc_set_time(ic_hal_rtc_t *rtc)
{
    
  ql_rtc_time_t tm;
  //get current time
  ql_rtc_get_time(&tm);   
  ql_rtc_print_time(tm);
  
  //2020-12-22 16:50:30 [WED]
  tm.tm_year = rtc->year;
  tm.tm_mon  = rtc->mon;
  tm.tm_mday = rtc->day;
  tm.tm_wday = rtc->week;
  tm.tm_hour = rtc->hour;
  tm.tm_min  = rtc->min;
  tm.tm_sec  = rtc->sec;

  ql_rtc_print_time(tm);
  //set time
  ql_rtc_set_time(&tm);

    return true;
}

bool ic_hal_rtc_set_time_notification(ic_hal_time_notification_t period, 
        ic_hal_time_notification_cb_t time_cb, void *user_data)
{
    return true;
}

bool ic_hal_rtc_get_alarm_time(ic_hal_rtc_t *rtc)
{
    return true;
}

bool ic_hal_rtc_set_alarm_time(ic_hal_rtc_t *rtc)
{
    return true;
}

bool ic_hal_rtc_set_alarm_cb(ic_hal_time_notification_cb_t callback, void *user_data) {
    return true;
}

bool ic_hal_rtc_enable_alarm(void)
{
    return true;
}

bool ic_hal_rtc_disable_alarm(void)
{
    return true;
}
