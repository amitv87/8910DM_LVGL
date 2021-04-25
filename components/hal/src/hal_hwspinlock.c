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

#include "hal_hwspinlock.h"
#include "osi_api.h"
#include "hwregs.h"

#define LOCK_TIMEOUT (100 * 1000) // 100ms
#define HWSPINLOCK_RELEASE (0x55aa10c5)
#define LOCKSTS(n) ((&(hwp_spinlock->locksts0))[n])

uint32_t halHwspinlockAcquire(uint32_t id)
{
    osiElapsedTimer_t elapsed;
    osiElapsedTimerStart(&elapsed);

    for (;;)
    {
        uint32_t critical = osiEnterCritical();
        uint32_t locksts = LOCKSTS(id);
        if (locksts == 0)
            return critical;
        osiExitCritical(critical);

        if (osiElapsedTimeUS(&elapsed) > LOCK_TIMEOUT)
            osiPanic();
    }
    return 0; // never reach
}

void halHwspinlockRelease(uint32_t critical, uint32_t id)
{
    LOCKSTS(id) = HWSPINLOCK_RELEASE;
    osiExitCritical(critical);
}

bool halHwspinlockTryAcquireInCritical(uint32_t id)
{
    uint32_t locksts = LOCKSTS(id);
    return (locksts == 0);
}

void halHwspinlockAcquireInCritical(uint32_t id)
{
    OSI_LOOP_WAIT(LOCKSTS(id) == 0);
}

void halHwspinlockReleaseInCritical(uint32_t id)
{
    LOCKSTS(id) = HWSPINLOCK_RELEASE;
}
