/// @file ic_hal.h
/// @Synopsis: generic hal layer header 
/// @author Wang Hua (whfyzg@gmail.com))
/// @version V1.0
/// @date 2020-11-14

#ifndef __IC_HAL_RTC_H__
#define  __IC_HAL_RTC_H__

typedef struct 
{
   uint8_t  year;     //year-2000 
   uint8_t  mon;      //1 -- 12
   uint8_t  day;      //1--31
   
   uint8_t  hour;
   uint8_t  min;
   uint8_t  sec;
   uint16_t msec;

   uint8_t week;
}ic_hal_rtc_t;

typedef enum
{
    TIEM_NORIFY_EVERY_SECOND,
    TIEM_NORIFY_EVERY_MINUTE,
    TIEM_NORIFY_EVERY_HOUR,
    TIEM_NORIFY_EVERY_DAY,
    TIEM_NORIFY_EVERY_MONTH,
    TIEM_NORIFY_EVERY_YEAR,
    TIEM_NORIFY_EVERY_SECOND_1_2,
    TIEM_NORIFY_EVERY_SECOND_1_4,
    TIEM_NORIFY_EVERY_SECOND_1_8
}ic_hal_time_notification_t;

typedef void(* 	ic_hal_time_notification_cb_t) (void *user_data);

extern bool ic_hal_rtc_init(void);
extern bool ic_hal_rtc_deinit(void);

//get current time
extern bool ic_hal_rtc_get_time(ic_hal_rtc_t *rtc);
extern bool ic_hal_rtc_set_time(ic_hal_rtc_t *rtc);

extern bool ic_hal_rtc_set_time_notification(ic_hal_time_notification_t period, 
        ic_hal_time_notification_cb_t time_cb, void *user_data);

extern bool ic_hal_rtc_get_alarm_time(ic_hal_rtc_t *rtc);
extern bool ic_hal_rtc_set_alarm_time(ic_hal_rtc_t *rtc);
extern bool ic_hal_rtc_set_alarm_cb(ic_hal_time_notification_cb_t callback, void *user_data);
extern bool ic_hal_rtc_enable_alarm(void);
extern bool ic_hal_rtc_disable_alarm(void);

//backlight adjust
#endif
