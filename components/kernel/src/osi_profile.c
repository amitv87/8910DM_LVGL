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

#include "osi_api.h"
#include "osi_profile.h"
#include "osi_internal.h"
#include "osi_chip.h"
#include "hwregs.h"
#include <assert.h>

static_assert(OSI_IS_ALIGNED(CONFIG_KERNEL_PROFILE_BUF_SIZE, 4),
              "CONFIG_KERNEL_PROFILE_BUF_SIZE should be 4 aligned");

typedef struct
{
    uint32_t mode;           // profile mode
    uint32_t *start_address; // profile buffer start address
    uint16_t pos;            // profile next write position, in word
    uint16_t size;           // profile buffer size, in word
    uint32_t freq;           // profile tick frequency
} osiProfileContext_t;

#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
static uint32_t gProfileBuf[CONFIG_KERNEL_PROFILE_BUF_SIZE / 4];
static osiProfileContext_t gProfileCtx;
#endif

/**
 * Tick value for profile
 */
static inline uint32_t prvProfileTick(void)
{
#if defined(CONFIG_SOC_8910) && defined(CONFIG_KERNEL_PROFILE_CLOCK32K)
    return hwp_idle->idl_32k_ref;
#else
    return HAL_TIMER_CURVAL_LO + (uint32_t)gOsiUpTimeOffset;
#endif
}

static inline uint32_t prvProfileTickFreq(void)
{
#if defined(CONFIG_SOC_8910) && defined(CONFIG_KERNEL_PROFILE_CLOCK32K)
    return 32768;
#else
    return CONFIG_KERNEL_HWTICK_FREQ;
#endif
}

void osiProfileInit(void)
{
#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
    osiProfileContext_t *p = &gProfileCtx;
    p->mode = OSI_PROFILE_MODE_NORMAL;
    p->start_address = gProfileBuf;
    p->pos = 0;
    p->size = CONFIG_KERNEL_PROFILE_BUF_SIZE / 4;
    p->freq = prvProfileTickFreq();
#endif
}

void osiProfileSetMode(osiProfileMode_t mode)
{
#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
    unsigned critical = osiEnterCritical();
    osiProfileContext_t *p = &gProfileCtx;
    if (p->mode != mode)
    {
        p->mode = mode;
        p->pos = 0;
    }
    osiExitCritical(critical);
#endif
}

void osiProfileCode(unsigned code)
{
#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
    osiProfileContext_t *p = &gProfileCtx;
    if (p->start_address == 0)
        return;

    unsigned critical = osiEnterCritical();

    if (p->mode == OSI_PROFILE_MODE_STOP_ON_FULL && p->pos + 1 >= p->size)
    {
        osiExitCritical(critical);
        return;
    }

    static uint32_t prev_tick = 0;
    uint32_t tick = prvProfileTick();
    uint32_t pcode = (tick << 16) | (code & 0xffff);
    uint32_t tick_delta_hi = (tick - prev_tick) >> 16;

    prev_tick = tick;
    if (tick_delta_hi > 1)
    {
        p->start_address[p->pos++] = (tick_delta_hi << 16) | 0;
        if (p->pos >= p->size)
            p->pos = 0;
    }

    p->start_address[p->pos++] = pcode;
    if (p->pos >= p->size)
        p->pos = 0;

    osiExitCritical(critical);
#endif
}

void osiProfileEnter(unsigned code)
{
#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
    osiProfileCode(code & ~PROFCODE_EXIT_FLAG);
#endif
}

void osiProfileExit(unsigned code)
{
#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
    osiProfileCode(code | PROFCODE_EXIT_FLAG);
#endif
}

void osiProfileThreadEnter(unsigned id)
{
#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
    unsigned code = id + PROFCODE_THREAD_START;
    if (code > PROFCODE_THREAD_END)
        code = PROFCODE_THREAD_END;
    osiProfileCode(code);
#endif
}

void osiProfileThreadExit(unsigned id)
{
    // coolprofile doen't need the event of thread exit
    // If thread exit event is inserted, coolprofile will show double
    // events at thread switch.
}

void osiProfileIrqEnter(unsigned id)
{
#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
    unsigned code = id + PROFCODE_IRQ_START;
    if (code > PROFCODE_IRQ_END)
        code = PROFCODE_IRQ_END;
    osiProfileCode(code);
#endif
}

void osiProfileIrqExit(unsigned id)
{
#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
    unsigned code = id + PROFCODE_IRQ_START;
    if (code > PROFCODE_IRQ_END)
        code = PROFCODE_IRQ_END;
    osiProfileCode(code | PROFCODE_EXIT_FLAG);
#endif
}

osiBuffer_t osiProfileLinearBuf(uint32_t pos)
{
    osiBuffer_t buf = {.ptr = 0, .size = 0};
#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
    osiProfileContext_t *p = &gProfileCtx;
    unsigned critical = osiEnterCritical();
    if (p->start_address != NULL && p->size > 0)
    {
        pos /= 4;
        pos = (pos % p->size);
        buf.ptr = (uintptr_t)(p->start_address + pos);
        buf.size = 4 * ((pos == p->pos) ? 0 : (pos < p->pos) ? p->pos - pos : p->size - pos);
    }
    osiExitCritical(critical);
#endif
    return buf;
}

unsigned osiProfileGetLastest(uint32_t *mem, unsigned size)
{
#if (CONFIG_KERNEL_PROFILE_BUF_SIZE > 0)
    if (mem == NULL || size == 0)
        return 0;

    osiProfileContext_t *p = &gProfileCtx;
    unsigned critical = osiEnterCritical();
    if (p->start_address != NULL && p->size > 0)
    {
        mem += (size / 4 - 1);
        int pos = p->pos;
        for (unsigned n = 0; n < size / 4; n++)
        {
            pos -= 1;
            if (pos < 0)
                pos = p->size - 1;
            *mem-- = p->start_address[pos];
        }
    }
    osiExitCritical(critical);
    return size;
#else
    return 0;
#endif
}
