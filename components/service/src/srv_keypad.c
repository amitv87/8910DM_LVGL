/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('S', 'K', 'P', 'D')

#include "srv_keypad.h"
#include "osi_api.h"
#include "osi_log.h"
#include <stdlib.h>
#include <sys/queue.h>
#include <assert.h>
#include "quec_proj_config.h"

#ifndef CONFIG_QUEC_PROJECT_FEATURE_KEYPAD
#define CONFIG_SRV_KEYPAD_ENABLE
#endif

typedef LIST_ENTRY(srv_kpd_listener) srvKpdLsnerIter_t;
typedef LIST_HEAD(srv_kpd_head, srv_kpd_listener) srvKpdLsnerHead_t;
struct srv_kpd_listener
{
    srvKpdLsnerIter_t iter;
    uint32_t id;
    kpdHook_t func;
    void *ctx;
};

typedef struct
{
    uint32_t pending;
    srvKpdLsnerHead_t lsners;
} srvKpd_t;

typedef struct
{
    osiWorkQueue_t *wq;
    osiWork_t *work;
    osiMutex_t *lock;
    srvKpd_t *kpds[KEY_MAP_MAX_COUNT];
    uint32_t unprocessed;
} srvKpdMgr_t;

static srvKpdMgr_t *gKpdMgr = NULL;

static_assert(KEY_MAP_MAX_COUNT <= 32, "Too many keys");

static void prvKpdProcessWork(void *p)
{
    srvKpdMgr_t *km = (srvKpdMgr_t *)p;
    OSI_LOGD(0, "srvkpd work %x", km->unprocessed);
    while (km->unprocessed)
    {
        uint32_t critical = osiEnterCritical();
        uint32_t id = 31 - __builtin_clz(km->unprocessed);
        km->unprocessed &= ~(1 << id);
        srvKpd_t *k = km->kpds[id];
        if (k == NULL || k->pending == 0)
            continue;

        uint32_t pending = k->pending;
        k->pending = 0;
        osiExitCritical(critical);

        while (pending && !LIST_EMPTY(&k->lsners))
        {
            critical = osiEnterCritical();
            uint32_t index = 31 - __builtin_clz(pending);
            pending &= ~(1 << index);
            if ((index % 2) == 0)
            {
                osiExitCritical(critical);
                OSI_LOGW(0, "invalid pending event %u/%x", id, pending);
                continue;
            }
            keyState_t v = (pending & (1 << (index - 1))) ? KEY_STATE_PRESS : KEY_STATE_RELEASE;
            pending &= ~(1 << (index - 1));
            osiExitCritical(critical);

            osiMutexLock(km->lock);
            srvKpdLsn_t *lsn;
            LIST_FOREACH(lsn, &k->lsners, iter)
            {
                if (lsn->func)
                    lsn->func(id, v, lsn->ctx);
            }
            osiMutexUnlock(km->lock);
        }
    }
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_KEYPAD
static void prvKpdIsrCB(keyMap_t id, keyState_t evt, void *p)
{
    if (id >= KEY_MAP_MAX_COUNT)
        return;

    srvKpdMgr_t *km = (srvKpdMgr_t *)p;
    srvKpd_t *k = km->kpds[id];
    OSI_LOGD(0, "srvkpd isr %d/%x", id, evt);
    if (k == NULL)
        return;

    if (k->pending & (1 << 31))
    {
        OSI_LOGW(0, "too many unprocessed key event %u/%x/%x", id, k->pending, evt);
    }

    km->unprocessed |= (1 << id);
    k->pending = (k->pending << 2) | (0b10 | (evt == KEY_STATE_PRESS ? 1 : 0));
    osiWorkEnqueue(km->work, km->wq);
}
#endif

static void prvPwrKeyIsrCB(pwrState_t evt, void *p)
{

    uint8_t pwrId = POWERKEY_KEYCODE;
    srvKpdMgr_t *km = (srvKpdMgr_t *)p;
    srvKpd_t *k = km->kpds[pwrId];
    OSI_LOGD(0, "srvkpd isr %d/%x", pwrId, evt);
    if (k == NULL)
        return;

    km->unprocessed |= (1 << pwrId);
    k->pending = (k->pending << 2) | (0b10 | (evt == PWR_STATE_PRESS ? 1 : 0));
    osiWorkEnqueue(km->work, km->wq);
}

srvKpdLsn_t *srvKpdListen(keyMap_t id, kpdHook_t func, void *ctx)
{
    srvKpdMgr_t *km = gKpdMgr;
    if (id >= KEY_MAP_MAX_COUNT || km == NULL)
        return NULL;

    osiMutexLock(km->lock);
    srvKpdLsn_t *lsn = NULL;
    srvKpd_t *k = km->kpds[id];
    if (k == NULL)
    {
        k = (srvKpd_t *)calloc(1, sizeof(*k));
        km->kpds[id] = k;
    }

    if (k)
    {
        lsn = calloc(1, sizeof(*lsn));
        if (lsn == NULL)
        {
            if (LIST_EMPTY(&k->lsners))
            {
                km->kpds[id] = NULL;
                free(k);
            }
        }
        else
        {
            lsn->func = func;
            lsn->ctx = ctx;
            lsn->id = id;
            LIST_INSERT_HEAD(&k->lsners, lsn, iter);
        }
    }

    osiMutexUnlock(km->lock);
    return lsn;
}

bool srvKpdListenBreak(srvKpdLsn_t *lsn)
{
    srvKpdMgr_t *km = gKpdMgr;
    if (km == NULL || lsn == NULL || lsn->id >= KEY_MAP_MAX_COUNT)
        return false;

    osiMutexLock(km->lock);
    bool result = false;
    uint32_t id = lsn->id;
    srvKpd_t *k = km->kpds[id];
    if (k)
    {
        bool des = false;
        uint32_t critical = osiEnterCritical();
        LIST_REMOVE(lsn, iter);
        if (LIST_EMPTY(&k->lsners))
        {
            km->kpds[id] = NULL;
            des = true;
        }
        osiExitCritical(critical);

        result = true;
        free(lsn);
        if (des)
            free(k);
    }
    osiMutexUnlock(km->lock);
    return result;
}

void srvKpdInit()
{
    if (gKpdMgr != NULL)
        return;

    srvKpdMgr_t *km = calloc(1, sizeof(*km));
    bool result = false;
    if (km)
    {
        do
        {
            km->lock = osiMutexCreate();
            if (!km->lock)
                break;

            km->work = osiWorkCreate(prvKpdProcessWork, NULL, km);
            if (!km->work)
                break;

#ifdef CONFIG_SRV_KEYPAD_ENABLE

            drvKeypadInit();
            uint32_t mask = KEY_STATE_PRESS | KEY_STATE_RELEASE;
            drvKeypadSetCB(prvKpdIsrCB, mask, km);
#endif

            drvPowerkeyInit();
            uint32_t pmask = PWR_STATE_PRESS | PWR_STATE_RELEASE;
            drvPwrKeySetCB(prvPwrKeyIsrCB, pmask, km);

            km->wq = osiSysWorkQueueHighPriority();

            gKpdMgr = km;
            result = true;
        } while (0);

        if (!result)
        {
            osiMutexDelete(km->lock);
            osiWorkDelete(km->work);
            free(km);
        }
    }

    if (!result)
    {
        OSI_LOGE(0, "fail init serial kpd");
        osiPanic();
    }
}
