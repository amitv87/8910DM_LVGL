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

#include "drv_rtc_hal.h"
#include "hal_config.h"
#include "hwregs.h"
#include "drv_config.h"
#include <time.h>

typedef struct
{
    uint8_t year; // [0,127] Year-2000, tm_year: Year-1900
    uint8_t mon;  // [1,12], tm_mon: (0-11)
    uint8_t day;  // [1,31], tm_mday: (1-31)
    uint8_t hour; // [0,23], tm_hour: (0-23)
    uint8_t min;  // [0,59], tm_min: (0-59)
    uint8_t sec;  // [0,59], tm_sec: (0-60)
    uint8_t wday; // [1,7] 1 for Monday, tm_wday: (0-6) 0 for Sunday
} rtcTime_t;

static int64_t _rtcTimeToSec(rtcTime_t t)
{
    struct tm tm = {
        .tm_sec = t.sec,
        .tm_min = t.min,
        .tm_hour = t.hour,
        .tm_mday = t.day,
        .tm_mon = (t.mon > 0) ? t.mon - 1 : 0,
        .tm_year = t.year + 100,
    };
    return mktime(&tm);
}

static rtcTime_t _secToRtcTime(int64_t sec)
{
    time_t ts = sec;
    struct tm tm;
    gmtime_r(&ts, &tm);

    rtcTime_t t = {
        .sec = tm.tm_sec,
        .min = tm.tm_min,
        .hour = tm.tm_hour,
        .day = tm.tm_mday,
        .mon = tm.tm_mon + 1,
        .year = (tm.tm_year > 100) ? tm.tm_year - 100 : 0,
        .wday = (tm.tm_wday == 0) ? 7 : tm.tm_mday,
    };
    return t;
}

int64_t drvRtcHalReadSecond(drvRtcHalContext_t *d)
{
    REG_CALENDAR_CMD_T cmd;
    REG_WAIT_FIELD_EQZ(cmd, hwp_calendar->cmd, calendar_not_valid);

    REG_CALENDAR_CALENDAR_CURVAL_L_T curval_l = {hwp_calendar->calendar_curval_l};
    REG_CALENDAR_CALENDAR_CURVAL_H_T curval_h = {hwp_calendar->calendar_curval_h};

    rtcTime_t t = {
        .sec = curval_l.b.sec,
        .min = curval_l.b.min,
        .hour = curval_l.b.hour,
        .day = curval_h.b.day,
        .mon = curval_h.b.mon,
        .year = curval_h.b.year,
    };
    return _rtcTimeToSec(t);
}

void drvRtcHalWriteSecond(drvRtcHalContext_t *d, int64_t sec)
{
    rtcTime_t t = _secToRtcTime(sec);

    REG_CALENDAR_CALENDAR_LOADVAL_L_T loadval_l = {
        .b = {
            .sec = t.sec,
            .min = t.min,
            .hour = t.hour,
        }};
    REG_CALENDAR_CALENDAR_LOADVAL_H_T loadval_h = {
        .b = {
            .day = t.day,
            .mon = t.mon,
            .year = t.year,
            .weekday = t.wday,
        }};

    hwp_calendar->calendar_loadval_l = loadval_l.v;
    hwp_calendar->calendar_loadval_h = loadval_h.v;

    REG_CALENDAR_CMD_T cmd;
    REG_FIELD_WRITE1(hwp_calendar->cmd, cmd, calendar_load, 1);
    REG_WAIT_FIELD_EQZ(cmd, hwp_calendar->cmd, calendar_load);
}

void drvRtcHalUnsetAlarm(drvRtcHalContext_t *d)
{
    REG_CALENDAR_CMD_T cmd;
    REG_FIELD_WRITE1(hwp_calendar->cmd, cmd, alarm_enable_clr, 1);
    REG_WAIT_FIELD_EQZ(cmd, hwp_calendar->cmd, alarm_enable_clr);
}

void drvRtcHalSetAlarm(drvRtcHalContext_t *d, int64_t sec)
{
    rtcTime_t t = _secToRtcTime(sec);

    REG_CALENDAR_ALARMVAL_L_T alarmval_l = {
        .b = {
            .sec = t.sec,
            .min = t.min,
            .hour = t.hour,
        }};
    REG_CALENDAR_ALARMVAL_H_T alarmval_h = {
        .b = {
            .day = t.day,
            .mon = t.mon,
            .year = t.year,
        }};

    hwp_calendar->alarmval_l = alarmval_l.v;
    hwp_calendar->alarmval_h = alarmval_h.v;

    REG_CALENDAR_CMD_T cmd;
    REG_FIELD_WRITE2(hwp_calendar->cmd, cmd, alarm_load, 1, alarm_enable_set, 1);
    REG_WAIT_FIELD_EQZ(cmd, hwp_calendar->cmd, alarm_load);
}

void drvRtcHalInit(drvRtcHalContext_t *d) {}
