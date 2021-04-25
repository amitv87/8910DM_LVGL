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

#include "osi_log.h"
#include "osi_api.h"
#include "osi_chip.h"
#include "osi_internal.h"
#include "kernel_config.h"
#include "hwregs.h"
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

static_assert(CONFIG_KERNEL_PERIODIC_TIMER_MIN_PERIOD * CONFIG_KERNEL_TICK_HZ <= 1000,
              "CONFIG_KERNEL_PERIODIC_TIMER_MIN_PERIOD is too large");

const unsigned gOstickRelaxedTick = (CONFIG_KERNEL_OSTICK_RELAXED_TIME < 0)
                                        ? OSI_WAIT_FOREVER
                                        : OSI_MS_TO_HWTICK(CONFIG_KERNEL_OSTICK_RELAXED_TIME);
const unsigned gTimerWqStackSize = CONFIG_KERNEL_TIMER_WQ_STACKSIZE;
const unsigned gPeriodicTimerMinPeriod = OSI_MS_TO_HWTICK(CONFIG_KERNEL_PERIODIC_TIMER_MIN_PERIOD);
int64_t gOsiEpochOffset = 0;  // hwtick + offset => epoch in tick
int64_t gOsiUpTimeOffset = 0; // hwtick + offset => up time in tick
int64_t gOsiLocalOffset = 0;  // hwtick + offset => local time (epoch + time zone) in tick

/**
 * \brief read 64bits hardware tick
 * \return 64bits hardware tick
 */
OSI_FORCE_INLINE static uint64_t prvChipHwTick(void)
{
#ifdef HAL_TIMER_CURVAL_HI
    for (;;)
    {
        uint32_t hi = HAL_TIMER_CURVAL_HI;
        uint32_t lo = HAL_TIMER_CURVAL_LO;
        if (HAL_TIMER_CURVAL_HI == hi)
            return ((uint64_t)hi << 32) | lo;
    }
#else
    return HAL_TIMER_CURVAL_LO;
#endif
}

void osiSetUpHWTick(int64_t tick)
{
    uint32_t critical = osiEnterCritical();
    int64_t curr_tick = prvChipHwTick();
    int64_t delta_tick = tick - (curr_tick + gOsiUpTimeOffset);
    if (delta_tick > 0) // it is unreasonable to change uptime back
    {
        gOsiEpochOffset += delta_tick;
        gOsiUpTimeOffset += delta_tick;
        gOsiLocalOffset += delta_tick;
    }
    osiExitCritical(critical);
}

void osiSetUpTime(int64_t ms)
{
    uint32_t critical = osiEnterCritical();
    int64_t curr_tick = prvChipHwTick();
    int64_t delta_tick = OSI_MS_TO_HWTICK(ms) - (curr_tick + gOsiUpTimeOffset);
    if (delta_tick > 0) // it is unreasonable to change uptime back
    {
        gOsiEpochOffset += delta_tick;
        gOsiUpTimeOffset += delta_tick;
        gOsiLocalOffset += delta_tick;
    }
    osiExitCritical(critical);
}

int64_t osiUpHWTick(void)
{
    uint32_t critical = osiEnterCritical();
    int64_t tick = prvChipHwTick() + gOsiUpTimeOffset;
    osiExitCritical(critical);
    return tick;
}

uint32_t osiUpHWTick32(void)
{
    return HAL_TIMER_CURVAL_LO + (uint32_t)gOsiUpTimeOffset;
}

int64_t osiEpochTick(void)
{
    uint32_t critical = osiEnterCritical();
    int64_t tick = prvChipHwTick() + gOsiEpochOffset;
    osiExitCritical(critical);
    return tick;
}

int64_t osiLocalTick(void)
{
    uint32_t critical = osiEnterCritical();
    int64_t tick = prvChipHwTick() + gOsiLocalOffset;
    osiExitCritical(critical);
    return tick;
}

int gettimeofday(struct timeval *tv, void *tz)
{
    uint64_t ticks = osiEpochTick();
    uint64_t us = OSI_HWTICK_TO_US(ticks);
    tv->tv_usec = us % 1000000;
    tv->tv_sec = us / 1000000;
    // tz->tz_minuteswest = 0;
    // tz->tz_dsttime = 0;
    return 0;
}

int64_t osiUpTime(void)
{
    return OSI_HWTICK_TO_MS(osiUpHWTick());
}

int64_t osiUpTimeUS(void)
{
    return OSI_HWTICK_TO_US(osiUpHWTick());
}

int64_t osiEpochTime(void)
{
    return OSI_HWTICK_TO_MS(osiEpochTick());
}

int64_t osiEpochSecond(void)
{
    return OSI_HWTICK_TO_SECOND(osiEpochTick());
}

int64_t osiLocalTime(void)
{
    return OSI_HWTICK_TO_MS(osiLocalTick());
}

int64_t osiLocalSecond(void)
{
    return OSI_HWTICK_TO_SECOND(osiLocalTick());
}

bool osiSetEpochTime(int64_t ms)
{
    if (ms < (int64_t)CONFIG_KERNEL_MIN_UTC_SECOND * 1000 ||
        ms > (int64_t)CONFIG_KERNEL_MAX_UTC_SECOND * 1000)
        return false;

    uint32_t cs = osiEnterCritical();
    int64_t ticks = OSI_MS_TO_HWTICK(ms);
    int64_t curr = prvChipHwTick();
    int64_t delta = ticks - (curr + gOsiEpochOffset);
    gOsiLocalOffset += delta;
    gOsiEpochOffset += delta;
    osiExitCritical(cs);
    return true;
}

int osiTimeZoneOffset(void)
{
    uint32_t cs = osiEnterCritical();
    int64_t time_zone = gOsiLocalOffset - gOsiEpochOffset;
    osiExitCritical(cs);
    return OSI_HWTICK_TO_SECOND(time_zone);
}

void osiSetTimeZoneOffset(int offset)
{
    uint32_t cs = osiEnterCritical();
    gOsiLocalOffset = gOsiEpochOffset + OSI_SECOND_TO_HWTICK(offset);
    osiExitCritical(cs);
}

int64_t osiEpochToUpTime(int64_t epoch)
{
    return epoch + OSI_HWTICK_TO_MS(gOsiUpTimeOffset - gOsiEpochOffset);
}

int64_t osiUpTimeToEpoch(int64_t uptime)
{
    return uptime + OSI_HWTICK_TO_MS(gOsiEpochOffset - gOsiUpTimeOffset);
}

void osiElapsedTimerStart(osiElapsedTimer_t *timer)
{
    if (timer != NULL)
        *timer = HAL_TIMER_CURVAL_LO + (uint32_t)gOsiUpTimeOffset;
}

uint32_t osiElapsedTime(osiElapsedTimer_t *timer)
{
    if (timer == NULL)
        return 0;

    uint32_t curr = HAL_TIMER_CURVAL_LO + (uint32_t)gOsiUpTimeOffset;
    uint32_t delta = curr - (*timer);
    return OSI_HWTICK_TO_MS_U32(delta);
}

uint32_t osiElapsedTimeUS(osiElapsedTimer_t *timer)
{
    if (timer == NULL)
        return 0;

    uint32_t curr = HAL_TIMER_CURVAL_LO + (uint32_t)gOsiUpTimeOffset;
    uint32_t delta = curr - (*timer);
    return OSI_HWTICK_TO_US_U32(delta);
}
