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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('R', 'T', 'C', 'A')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_rtc_hal.h"
#include "hal_config.h"
#include "hal_chip.h"
#include "hal_adi_bus.h"
#include "drv_config.h"
#include "drv_pmic_intr.h"
#include "kernel_config.h"
#include "hwregs.h"
#include "osi_log.h"
#include <assert.h>

typedef struct
{
    uint16_t day;
    uint8_t min;
    uint8_t hour;
    uint8_t sec;
} rtcTime_t;

static rtcTime_t prvSecToRtcTime(int64_t sec)
{
    rtcTime_t t;
    unsigned sec_in_day = (unsigned)sec % DAY_SECONDS;
    t.day = (unsigned)sec / DAY_SECONDS;
    t.hour = sec_in_day / HOUR_SECONDS;
    t.min = (sec_in_day % HOUR_SECONDS) / MIN_SECONDS;
    t.sec = sec_in_day % MIN_SECONDS;
    return t;
}

int64_t drvRtcHalReadSecond(drvRtcHalContext_t *d)
{
    OSI_LOGI(0, "RTC readsecond:%u  UTC:%u",halPmuRtcReadSecond(),CONFIG_KERNEL_MIN_UTC_SECOND);
    return halPmuRtcReadSecond() + CONFIG_KERNEL_MIN_UTC_SECOND;
}

void drvRtcHalWriteSecond(drvRtcHalContext_t *d, int64_t sec_utc)
{
    if (sec_utc < CONFIG_KERNEL_MIN_UTC_SECOND)
        osiPanic();

    int64_t sec = sec_utc - CONFIG_KERNEL_MIN_UTC_SECOND;
    rtcTime_t t = prvSecToRtcTime(sec);
    OSI_LOGI(0, "RTC write second %u/%u:%u:%u", t.day, t.hour, t.min, t.sec);
    osiSemaphoreAcquire(d->sem_rtc_upd);

    REG_RDA2720M_RTC_RTC_INT_EN_T rtc_int_en;
    halAdiBusBatchChange(
        &hwp_rda2720mRtc->rtc_day_cnt_upd, HAL_ADI_BUS_OVERWITE(t.day),
        &hwp_rda2720mRtc->rtc_hrs_cnt_upd, HAL_ADI_BUS_OVERWITE(t.hour),
        &hwp_rda2720mRtc->rtc_min_cnt_upd, HAL_ADI_BUS_OVERWITE(t.min),
        &hwp_rda2720mRtc->rtc_sec_cnt_upd, HAL_ADI_BUS_OVERWITE(t.sec),
        &hwp_rda2720mRtc->rtc_int_en, REG_FIELD_MASKVAL1(rtc_int_en, rtc_sec_cnt_upd_int_en, 1),
        HAL_ADI_BUS_CHANGE_END);
}

void drvRtcHalUnsetAlarm(drvRtcHalContext_t *d)
{
    OSI_LOGI(0, "RTC unset alarm");

    REG_RDA2720M_RTC_RTC_INT_EN_T rtc_int_en;
    halAdiBusChange(&hwp_rda2720mRtc->rtc_int_en, REG_FIELD_MASKVAL1(rtc_int_en, rtc_alm_int_en, 0));
}

void drvRtcHalSetAlarm(drvRtcHalContext_t *d, int64_t sec_utc)
{
    if (sec_utc < CONFIG_KERNEL_MIN_UTC_SECOND)
        osiPanic();

    int64_t sec = sec_utc - CONFIG_KERNEL_MIN_UTC_SECOND;
    REG_RDA2720M_RTC_RTC_INT_EN_T rtc_int_en;
    rtcTime_t t = prvSecToRtcTime(sec);

    OSI_LOGI(0, "RTC set alarm day/%d hour/%d min/%d sec/%d", t.day, t.hour, t.min, t.sec);

    //osiSemaphoreAcquire(d->sem_alarm_upd);
    halAdiBusBatchChange(
        &hwp_rda2720mRtc->rtc_int_en, REG_FIELD_MASKVAL1(rtc_int_en, rtc_sec_alm_upd_int_en, 1),
        &hwp_rda2720mRtc->rtc_day_alm_upd, HAL_ADI_BUS_OVERWITE(t.day),
        &hwp_rda2720mRtc->rtc_hrs_alm_upd, HAL_ADI_BUS_OVERWITE(t.hour),
        &hwp_rda2720mRtc->rtc_min_alm_upd, HAL_ADI_BUS_OVERWITE(t.min),
        &hwp_rda2720mRtc->rtc_sec_alm_upd, HAL_ADI_BUS_OVERWITE(t.sec),
        &hwp_rda2720mRtc->rtc_int_en, REG_FIELD_MASKVAL1(rtc_int_en, rtc_alm_int_en, 1),
        HAL_ADI_BUS_CHANGE_END);
}

static void prvRtcIntrCB(void *ctx)
{
    drvRtcHalContext_t *d = (drvRtcHalContext_t *)ctx;
    REG_RDA2720M_RTC_RTC_INT_MASK_STS_T status;
    status.v = halAdiBusRead(&hwp_rda2720mRtc->rtc_int_mask_sts);

    // clear all interrupts
    halAdiBusWrite(&hwp_rda2720mRtc->rtc_int_clr, status.v);

    OSI_LOGD(0, "RTC intr 0x%08x", status.v);

    REG_RDA2720M_RTC_RTC_INT_EN_T int_disable = {};

    if (status.b.rtc_spg_upd_int_mask_sts)
    {
        osiSemaphoreRelease(d->sem_spg_upd);
        int_disable.b.rtc_spg_upd_int_en = 1;
    }

    if (status.b.rtc_sec_cnt_upd_int_mask_sts)
    {
        osiSemaphoreRelease(d->sem_rtc_upd);
        int_disable.b.rtc_sec_cnt_upd_int_en = 1;
    }

    if (status.b.rtc_sec_alm_upd_int_mask_sts)
    {
        osiSemaphoreRelease(d->sem_alarm_upd);
        int_disable.b.rtc_sec_alm_upd_int_en = 1;
    }

    if (status.b.rtc_alm_int_mask_sts)
    {
        int_disable.b.rtc_alm_int_en = 1;
    }

    if (int_disable.v != 0)
        halAdiBusChange(&hwp_rda2720mRtc->rtc_int_en, int_disable.v, 0);
}

void drvRtcHalInit(drvRtcHalContext_t *d)
{
    d->sem_rtc_upd = osiSemaphoreCreate(1, 1);
    d->sem_alarm_upd = osiSemaphoreCreate(1, 1);
    d->sem_spg_upd = osiSemaphoreCreate(1, 1);

    osiSemaphoreAcquire(d->sem_spg_upd);
    drvPmicIntrEnable(DRV_PMIC_INTR_RTC, prvRtcIntrCB, d);
}
