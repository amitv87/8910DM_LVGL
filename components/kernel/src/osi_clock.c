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

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "osi_clock.h"
#include "osi_api.h"
#include "osi_log.h"
#include "osi_generic_list.h"
#include "osi_byte_buf.h"
#include "hal_chip.h"
#include <string.h>
#include <stdlib.h>
#include <sys/queue.h>

#ifdef CONFIG_SOC_8955
#define EXTRAM_CONSTRAIN_SUPPORTED
#else
#undef EXTRAM_CONSTRAIN_SUPPORTED
#endif

#ifdef CONFIG_SOC_8910
#undef DYNAMIC_SYS_CLK_SUPPORTED
#else
#define DYNAMIC_SYS_CLK_SUPPORTED
#endif

typedef union {
    uint32_t v;
    struct
    {
        bool min_freq : 1;      // minimal freq constrain
        bool hard_min_freq : 1; // minimal freq constrain is hard
        bool extram : 1;        // external RAM access constrain
    } b;
} osiclkConsFlag_t;

typedef SLIST_ENTRY(osiSysClkCbReg) osiSysClkCbRegIter_t;
typedef SLIST_HEAD(osiSysClkCbRegHead, osiSysClkCbReg) osiSysClkCbRegHead_t;
typedef struct osiSysClkCbReg
{
    uint32_t tag;
    osiSysClkChangeCallback_t cb;
    osiSysClkCbRegIter_t iter;
} osiSysClkCbReg_t;

typedef TAILQ_ENTRY(osiClkConsReg) osiClkConsRegIter_t;
typedef TAILQ_HEAD(osiClkConsRegHead, osiClkConsReg) osiClkConsRegHead_t;
typedef struct osiClkConsReg
{
    uint32_t tag;
    osiclkConsFlag_t flag;
    uint32_t freq;
    osiClkConsRegIter_t iter;
} osiClkConsReg_t;

typedef struct
{
    bool started;                    // clock management core started
    uint16_t hard_freq_count;        // hard "minimal freq constrain" count
    uint16_t extram_count;           // external RAM access constrain count
    osiSysClkCbRegHead_t cbreg_list; // callback list
    osiClkConsRegHead_t ccreg_list;  // constrain list
} osiClockContext_t;

static osiClockContext_t gOsiClockCtx;

void osiClockManInit(void)
{
    osiClockContext_t *d = &gOsiClockCtx;
    d->started = false;
    d->hard_freq_count = 0;
    d->extram_count = 0;
    SLIST_INIT(&d->cbreg_list);
    TAILQ_INIT(&d->ccreg_list);
}

void osiClockManStart(void)
{
    // TODO: gOsiClockCtx.started = true;
    osiReapplySysClk();
}

static void _cbregCheckInsert(osiSysClkCbRegHead_t *head, osiSysClkCbReg_t *item)
{
    osiSysClkCbReg_t *var;
    SLIST_FOREACH(var, head, iter)
    {
        if (var == item)
            return;
    }
    SLIST_INSERT_HEAD(head, item, iter);
}

static void _cbregCheckRemove(osiSysClkCbRegHead_t *head, osiSysClkCbReg_t *item)
{
    osiSysClkCbReg_t *var = SLIST_FIRST(head);
    if (var == item)
    {
        SLIST_REMOVE_HEAD(head, iter);
        return;
    }

    osiSysClkCbReg_t *varn;
    SLIST_FOREACH_FROM_SAFE(var, head, iter, varn)
    {
        if (varn == item)
        {
            SLIST_REMOVE_AFTER(var, iter);
            return;
        }
    }
}

void osiRegisterSysClkChangeCallback(osiSysClkCallbackRegistry_t *pr, osiSysClkChangeCallback_t cb)
{
    if (pr == NULL)
        return;

    if (cb == NULL)
    {
        osiUnregisterSysClkChangeCallback(pr);
        return;
    }

    osiClockContext_t *d = &gOsiClockCtx;
    osiSysClkCbReg_t *r = (osiSysClkCbReg_t *)pr;
    uint32_t critical = osiEnterCritical();
    r->cb = cb;
    _cbregCheckInsert(&d->cbreg_list, r);
    osiExitCritical(critical);
}

void osiUnregisterSysClkChangeCallback(osiSysClkCallbackRegistry_t *pr)
{
    osiClockContext_t *d = &gOsiClockCtx;
    osiSysClkCbReg_t *r = (osiSysClkCbReg_t *)pr;
    uint32_t critical = osiEnterCritical();
    r->cb = NULL;
    _cbregCheckRemove(&d->cbreg_list, r);
    osiExitCritical(critical);
}

void osiInvokeSysClkChangeCallbacks(uint32_t freq)
{
    osiClockContext_t *d = &gOsiClockCtx;
    osiSysClkCbReg_t *var;
    SLIST_FOREACH(var, &d->cbreg_list, iter)
    {
        var->cb(freq);
    }
}

static inline uint32_t _getMaxRequestFreq(void)
{
    osiClockContext_t *d = &gOsiClockCtx;
    osiClkConsReg_t *var = TAILQ_FIRST(&d->ccreg_list);
    return (var == NULL) ? 0 : var->freq;
}

static void _changeSysClk(uint32_t freq)
{
    osiClockContext_t *d = &gOsiClockCtx;
    if (d->started)
        halChangeSysClk(freq, d->extram_count != 0);
}

static void _addFreqOrderCons(osiClkConsReg_t *r)
{
    osiClockContext_t *d = &gOsiClockCtx;

    if (TAILQ_EMPTY(&d->ccreg_list))
    {
        TAILQ_INSERT_HEAD(&d->ccreg_list, r, iter);
        return;
    }

    osiClkConsReg_t *var = TAILQ_LAST(&d->ccreg_list, osiClkConsRegHead);
    if (r->freq <= var->freq)
    {
        TAILQ_INSERT_TAIL(&d->ccreg_list, r, iter);
        return;
    }

    TAILQ_FOREACH(var, &d->ccreg_list, iter)
    {
        if (r->freq >= var->freq)
        {
            TAILQ_INSERT_BEFORE(var, r, iter);
            break;
        }
    }
}

void osiClockConstrainInit(osiClockConstrainRegistry_t *pr, uint32_t tag)
{
    osiClkConsReg_t *r = (osiClkConsReg_t *)pr;
    r->tag = tag;
    r->flag.v = 0;
}

void osiRequestSysClk(osiClockConstrainRegistry_t *pr, uint32_t freq)
{
    osiClkConsReg_t *r = (osiClkConsReg_t *)pr;
    osiClockContext_t *d = &gOsiClockCtx;
    uint32_t critical = osiEnterCritical();

    uint32_t old_max_freq = _getMaxRequestFreq();
    OSI_LOGD(0, "CLOCK req %4c freq/%d flags/0x%x curr/%d",
             r->tag, freq, r->flag.v, old_max_freq);

    // if it is not "hard-min-freq", increase count
    if (!(r->flag.b.min_freq && r->flag.b.hard_min_freq))
        d->hard_freq_count++;

    // if already active constrain, remove it and will be added later
    if (r->flag.v != 0)
        TAILQ_REMOVE(&d->ccreg_list, r, iter);

    r->flag.b.min_freq = true;
    r->flag.b.hard_min_freq = true;
    r->freq = freq;

    // add the constrain, ordered by freq
    _addFreqOrderCons(r);

    uint32_t new_max_freq = _getMaxRequestFreq();
    if (new_max_freq != old_max_freq)
        _changeSysClk(new_max_freq);

    osiExitCritical(critical);
}

void osiRequestSysClkActive(osiClockConstrainRegistry_t *r)
{
    osiRequestSysClk(r, HAL_FREQ_26M);
}

void osiReleaseClk(osiClockConstrainRegistry_t *pr)
{
    osiClkConsReg_t *r = (osiClkConsReg_t *)pr;
    osiClockContext_t *d = &gOsiClockCtx;
    uint32_t critical = osiEnterCritical();

    OSI_LOGD(0, "CLOCK rel %4c flags/0x%x", r->tag, r->flag.v);

    // only handle if "min-freq" constrain is already set
    if (r->flag.b.min_freq)
    {
        uint32_t old_max_freq = _getMaxRequestFreq();

        // if it is "hard-min-freq", decrease the count
        if (r->flag.b.hard_min_freq)
            d->hard_freq_count--;

        r->flag.b.min_freq = false;
        r->flag.b.hard_min_freq = false;
        r->freq = 0;

        TAILQ_REMOVE(&d->ccreg_list, r, iter);

        // if there are other constrains, add to end
        if (r->flag.v != 0)
            TAILQ_INSERT_TAIL(&d->ccreg_list, r, iter);

        uint32_t new_max_freq = _getMaxRequestFreq();
        if (new_max_freq != old_max_freq)
            _changeSysClk(new_max_freq);
    }

    osiExitCritical(critical);
}

void osiRequestPerfClk(osiClockConstrainRegistry_t *pr, uint32_t freq)
{
    osiClkConsReg_t *r = (osiClkConsReg_t *)pr;
    osiClockContext_t *d = &gOsiClockCtx;
    uint32_t critical = osiEnterCritical();

    uint32_t old_max_freq = _getMaxRequestFreq();

    // if it is "hard-min-freq", decrease count
    if (r->flag.b.min_freq && r->flag.b.hard_min_freq)
        d->hard_freq_count--;

    // if already active constrain, remove it and will be added later
    if (r->flag.v != 0)
        TAILQ_REMOVE(&d->ccreg_list, r, iter);

    r->flag.b.min_freq = true;
    r->flag.b.hard_min_freq = false;
    r->freq = freq;

    // add the constrain, ordered by freq
    _addFreqOrderCons(r);

    uint32_t new_max_freq = _getMaxRequestFreq();
    if (new_max_freq != old_max_freq)
        _changeSysClk(new_max_freq);

    osiExitCritical(critical);
}

bool osiIsSlowSysClkAllowed(void)
{
    osiClockContext_t *d = &gOsiClockCtx;
    uint32_t critical = osiEnterCritical();
    bool allowed = true;
    osiClkConsReg_t *var = NULL;
    TAILQ_FOREACH(var, &d->ccreg_list, iter)
    {
        if (var->flag.b.hard_min_freq)
        {
            allowed = false;
            break;
        }
    }
    osiExitCritical(critical);
    return allowed;
}

void osiRequestExtRamAccess(osiClockConstrainRegistry_t *pr)
{
#ifdef EXTRAM_CONSTRAIN_SUPPORTED
    osiClkConsReg_t *r = (osiClkConsReg_t *)pr;
    osiClockContext_t *d = &gOsiClockCtx;
    uint32_t critical = osiEnterCritical();

    // only handle if "extram" is not set
    if (!r->flag.b.extram)
    {
        if (r->flag.v == 0)
        {
            // no other constrains, add to end
            r->flag.b.extram = true;
            r->freq = 0;
            TAILQ_INSERT_TAIL(&d->ccreg_list, r, iter);
        }
        else
        {
            // with other constrains, change flag in place
            r->flag.b.extram = true;
        }

        d->extram_count++;
        if (d->extram_count == 1)
            _changeSysClk(_getMaxRequestFreq());
    }

    osiExitCritical(critical);
#endif
}

void osiReleaseExtRamAccess(osiClockConstrainRegistry_t *pr)
{
#ifdef EXTRAM_CONSTRAIN_SUPPORTED
    osiClkConsReg_t *r = (osiClkConsReg_t *)pr;
    osiClockContext_t *d = &gOsiClockCtx;
    uint32_t critical = osiEnterCritical();

    // only handle if "extram" is not set
    if (r->flag.b.extram)
    {
        r->flag.b.extram = false;

        // if no other constrains, remove it from list
        if (r->flag.v == 0)
            TAILQ_REMOVE(&d->ccreg_list, r, iter);

        d->extram_count--;
        if (d->extram_count == 0)
            _changeSysClk(_getMaxRequestFreq());
    }

    osiExitCritical(critical);
#endif
}

void osiReleaseAllConstrain(osiClockConstrainRegistry_t *pr)
{
    osiClkConsReg_t *r = (osiClkConsReg_t *)pr;
    osiClockContext_t *d = &gOsiClockCtx;
    uint32_t critical = osiEnterCritical();

    if (r->flag.v != 0)
    {
        bool reconf_needed = false;

        uint32_t old_max_freq = _getMaxRequestFreq();
        if (r->flag.b.min_freq && r->flag.b.hard_min_freq)
            d->hard_freq_count--;

        if (r->flag.b.extram)
        {
            d->extram_count--;
            if (d->extram_count == 0)
                reconf_needed = true;
        }

        r->flag.v = 0;
        r->freq = 0;

        TAILQ_REMOVE(&d->ccreg_list, r, iter);

        uint32_t new_max_freq = _getMaxRequestFreq();
        if (new_max_freq != old_max_freq)
            reconf_needed = true;

        if (reconf_needed)
            _changeSysClk(new_max_freq);
    }

    osiExitCritical(critical);
}

void osiReapplySysClk(void)
{
    _changeSysClk(_getMaxRequestFreq());
}

int osiClockConstrainDump(void *mem, unsigned size)
{
    uint32_t critical = osiEnterCritical();
    osiClockContext_t *d = &gOsiClockCtx;

    osiClkConsReg_t *p;
    int count = 0;
    TAILQ_FOREACH(p, &d->ccreg_list, iter)
    {
        count++;
    }

    int total = 2 + count * 12;
    if (mem == NULL)
    {
        osiExitCritical(critical);
        return total;
    }
    if (total > size)
    {
        osiExitCritical(critical);
        return -1;
    }

    uint8_t *pmem = mem;
    OSI_STRM_WLE16(pmem, count);
    TAILQ_FOREACH(p, &d->ccreg_list, iter)
    {
        OSI_STRM_WLE32(pmem, p->tag);
        OSI_STRM_WLE32(pmem, p->flag.v);
        OSI_STRM_WLE32(pmem, p->freq);
    }

    osiExitCritical(critical);
    return total;
}
