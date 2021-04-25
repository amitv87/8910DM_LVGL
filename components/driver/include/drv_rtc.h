/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _DRV_RTC_H_
#define _DRV_RTC_H_

#include <stdint.h>
#include <stdbool.h>
#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * alarm opaque information size
 */
#define DRV_RTC_ALARM_INFO_SIZE (40)

/**
 * forward decalre of alarm data structure
 */
struct drvRtcAlarm;

/**
 * alarm type
 */
typedef enum
{
    DRV_RTC_ALARM_ONE_TIME,     ///< one time alarm
    DRV_RTC_ALARM_WDAY_REPEATED ///< repeated alarm in selected day of the week.
} drvRtcAlarmType_t;

/**
 * Callback function type to be called on alarm expiration.
 *
 * In callback, it is prohibited to call alarm related APIs.
 * Ecah owner should set there own callback first.
 *
 * The callback will be executed in RTC thread.
 */
typedef void (*drvRtcAlarmCB_t)(struct drvRtcAlarm *alarm, void *ctx);

/**
 * Parameters for day of the week repeated alarm
 */
typedef struct
{
    uint8_t wday_mask;   ///< selected day of the week
    uint32_t sec_in_day; ///< time in day, in seconds
    int32_t timezone;    ///< timezone of the alarm
} drvRtcAlarmWdayRepeated_t;

/**
 * alarm information
 */
typedef struct drvRtcAlarm
{
    uint32_t owner;                          ///< alarm owner
    uint32_t name;                           ///< name of the alarm
    drvRtcAlarmType_t type;                  ///< type of the alarm
    drvRtcAlarmWdayRepeated_t wday_repeated; ///< day of the week repeated parameters
    int64_t expire_sec;                      ///< next expiration time
    uint8_t info[DRV_RTC_ALARM_INFO_SIZE];   ///< opaque information
} drvRtcAlarm_t;

/**
 * @brief initialize RTC driver
 *
 * At initialization, NVM file may be read. And then it MUST be called
 * after file system is initialized. Also, it will use ADI bus, and then
 * it should be called after ADI bus is initialized.
 */
void drvRtcInit(void);

/**
 * @brief set one time alarm
 *
 * Set a one time alarm with specified time. The time is offset in
 * second from 1970-01-01 UTC.
 *
 * When the alarm already exists, it will be replaced with the new
 * parameters.
 *
 * @param owner     alarm owner
 * @param name      alarm name
 * @param info      alarm opaque information
 * @param info_size alarm opaque information size
 * @param sec       time second from 1970-01-01 UTC
 * @param replace   true: replace existed alarm with the same owner and name
 * @return
 *      - true on success
 *      - false on failure
 *          - invalid parameter
 *          - alarm exists, and replace is false
 *          - out of memory
 */
bool drvRtcSetAlarm(uint32_t owner, uint32_t name, const void *info,
                    uint32_t info_size, int64_t sec, bool replace);

/**
 * @brief set repeated alarm
 *
 * Set a repeated alarm with specified time.
 *
 * @param owner         alarm owner
 * @param name          alarm name
 * @param info          alarm opaque information
 * @param info_size     alarm opaque information size
 * @param wday_mask     day of the week for the alarm. bit 0 for Sunday, bit 6 for Saturday.
 * @param sec_in_day    alarm time in the day
 * @param timezone      timezone (second offset from UTC) of the alarm
 * @param replace       true: replace existed alarm with the same owner and name
 * @return
 *      - true on success
 *      - false on failure
 *          - invalid parameter
 *          - alarm exists, and replace is false
 *          - out of memory
 */
bool drvRtcSetRepeatAlarm(uint32_t owner, uint32_t name, const void *info,
                          uint32_t info_size, uint8_t wday_mask,
                          uint32_t sec_in_day, int timezone, bool replace);

/**
 * @brief remove an alarm
 *
 * @param owner         alarm owner
 * @param name          alarm name
 * @return
 *      - true on success
 *      - false on failure
 */
bool drvRtcRemoveAlarm(uint32_t owner, uint32_t name);

/**
 * @brief get alarm count for specific owner
 *
 * @param owner     alarm owner
 * @return          alarm count
 */
int drvRtcGetAlarmCount(uint32_t owner);

/**
 * @brief get alarm information for specific owner
 *
 * The returned alarms will be ordered by expiration time.
 *
 * @param owner     alarm owner
 * @param alarms    pointer to alarm array for output (caller allocated)
 * @param count     max count of alarms
 * @return          count of alarm
 */
int drvRtcGetAlarms(uint32_t owner, drvRtcAlarm_t *alarms, uint32_t count);

/**
 * @brief remove all alarms
 *
 * Maybe the only purpose is "factory reset".
 */
void drvRtcRemoveAllAlarms(void);

/**
 * @brief set alarm process callback for specific owner
 *
 * Caller must set a alarm owner callback before set an alarm, or the
 * alarm won't response.
 *
 * @param owner     alarm owner
 * @param context   caller context
 * @param cb        callback (NULL is allowed)
 * @return
 *      - true on success
 *      - false on fail
 */
bool drvRtcAlarmOwnerSetCB(uint32_t owner, void *context, drvRtcAlarmCB_t cb);

/**
 * @brief update RTC time from system epoch time
 */
void drvRtcUpdateTime(void);

/**
 * @brief get wakeup alarm in epoch time
 *
 * It may be called in sleep procedure, so thread synchronization (such as
 * smaphore and mutex) shouldn't be called.
 *
 * @return
 *      - wakeup alarm in epoch time in millisecond
 *      - INT64_MAX if there are no alarms
 */
int64_t drvRtcGetWakeupTime(void);

OSI_EXTERN_C_END
#endif
