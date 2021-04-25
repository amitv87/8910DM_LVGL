/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "boot_platform.h"
#include "hwregs.h"
#include "osi_compiler.h"
#include "osi_api.h"
#include "hal_chip.h"
#include "osi_tick_unit.h"

#define HWTICK_FROM(start) ((unsigned)(HAL_TIMER_CURVAL_LO - (start)))

OSI_NO_RETURN void bootPanic(void)
{
    __builtin_trap();
}

void bootElapsedTimerStart(osiElapsedTimer_t *timer)
{
    if (timer != NULL)
        *timer = HAL_TIMER_CURVAL_LO;
}

uint32_t bootElapsedTime(osiElapsedTimer_t *timer)
{
    if (timer == NULL)
        return 0;

    return OSI_HWTICK_TO_MS_U32(HWTICK_FROM(*timer));
}

uint32_t bootElapsedTimeUS(osiElapsedTimer_t *timer)
{
    if (timer == NULL)
        return 0;

    return OSI_HWTICK_TO_US_U32(HWTICK_FROM(*timer));
}

OSI_DECL_STRONG_ALIAS(bootPanic, OSI_NO_RETURN void osiPanic(void));
OSI_DECL_STRONG_ALIAS(bootElapsedTimerStart, void osiElapsedTimerStart(osiElapsedTimer_t *timer));
OSI_DECL_STRONG_ALIAS(bootElapsedTime, uint32_t osiElapsedTime(osiElapsedTimer_t *timer));
OSI_DECL_STRONG_ALIAS(bootElapsedTimeUS, uint32_t osiElapsedTimeUS(osiElapsedTimer_t *timer));
