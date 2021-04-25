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

#ifndef _DRV_RTC_HAL_H_
#define _DRV_RTC_HAL_H_

#include "hal_config.h"
#include "osi_api.h"
#include "quec_proj_config.h"

#define DAY_SECONDS (24 * 60 * 60)
#define HOUR_SECONDS (60 * 60)
#define MIN_SECONDS (60)
#define WEEK_DAYS (7)

#if defined(CONFIG_SOC_8910)
typedef struct
{
    osiSemaphore_t *sem_rtc_upd;
    osiSemaphore_t *sem_alarm_upd;
    osiSemaphore_t *sem_spg_upd;
} drvRtcHalContext_t;
#elif defined(CONFIG_SOC_8909) || defined(CONFIG_SOC_8955)
typedef struct
{
} drvRtcHalContext_t;
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_RTC
typedef void (*ql_rtc_hal_cb)(void);
#endif

void drvRtcHalInit(drvRtcHalContext_t *d);
int64_t drvRtcHalReadSecond(drvRtcHalContext_t *d);
void drvRtcHalWriteSecond(drvRtcHalContext_t *d, int64_t sec);
void drvRtcHalUnsetAlarm(drvRtcHalContext_t *d);
void drvRtcHalSetAlarm(drvRtcHalContext_t *d, int64_t sec);

void drvRtcHandleAlarm(void);

#endif
