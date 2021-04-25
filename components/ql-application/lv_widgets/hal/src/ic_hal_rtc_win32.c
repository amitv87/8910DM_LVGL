/// @file ic_hal.c
/// @Synopsis:  hal layer adaptor for win32
/// @author Wang Hua (whfyzg@gmail.com))
/// @version V1.0
/// @date 2020-11-14

#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "time.h"

#include "ic_hal_rtc.h"

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
    time_t my_time;
    struct tm * timeinfo; 
    time (&my_time);
    timeinfo = localtime (&my_time);

    rtc->year   =   timeinfo->tm_year+1900; 
    rtc->mon    =   timeinfo->tm_mon+1;
    rtc->day    =   timeinfo->tm_mday;
    rtc->hour   =   timeinfo->tm_hour;
    rtc->min    =   timeinfo->tm_min;
    rtc->sec    =   timeinfo->tm_sec;

    //printf("year->%d\n", timeinfo->tm_year + 1900);
    //printf("month->%d\n", timeinfo->tm_mon + 1);
    //printf("date->%d\n", timeinfo->tm_mday);
    //printf("hour->%d\n", timeinfo->tm_hour);
    //printf("minutes->%d\n", timeinfo->tm_min);
    //printf("seconds->%d\n", timeinfo->tm_sec);

    //not support ms

    return true;
}

bool ic_hal_rtc_set_time(ic_hal_rtc_t *rtc)
{
    return true;
}

bool ic_hal_rtc_set_time_notification(ic_hal_time_notification_t period, 
        ic_hal_time_notification_cb_t time_cb, void *user_data)
{
    return false;
}

bool ic_hal_rtc_get_alarm_time(ic_hal_rtc_t *rtc)
{
    return false;
}

bool ic_hal_rtc_set_alarm_time(ic_hal_rtc_t *rtc)
{
    return false;
}

bool ic_hal_rtc_set_alarm_cb(ic_hal_time_notification_cb_t callback, void *user_data) {
    return false;
}

bool ic_hal_rtc_enable_alarm(void)
{
    return false;
}

bool ic_hal_rtc_disable_alarm(void)
{
    return false;
}
