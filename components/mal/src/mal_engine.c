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

#include "mal_api.h"
#include "mal_internal.h"
#include "cfw_dispatch.h"
#include "cfw.h"
#include "osi_log.h"
#include <stdlib.h>

#define MAL_THREAD_STACK_SIZE (2048)
#define MAL_THREAD_EVENT_COUNT (32)

typedef enum
{
    MAL_TRANS_UNKNOWN = 0,
    MAL_TRANS_UTI,
} malTransType_t;

struct malTransaction
{
    malTransType_t type;
    uint16_t uti;
    void *ctx;
    malContextDelete_t ctx_delete;
    osiSemaphore_t *wait_sema;
};

typedef struct
{
    osiThread_t *cb_thread;
} malContext_t;

static malContext_t gMalCtx;

static void prvMalThreadEntry(void *param)
{
    gMalCtx.cb_thread = osiThreadCurrent();
    for (;;)
    {
        osiEvent_t event = {};
        osiEventWait(gMalCtx.cb_thread, &event);

        if (event.id == 0)
            continue;

        OSI_LOGXI(OSI_LOGPAR(I, S, I, I, I), 0, "MAL get event: %d(%s)/0x%08x/0x%08x/0x%08x",
                  event.id, CFW_EventName(event.id),
                  event.param1, event.param2, event.param3);

        if ((!cfwIsCfwIndicate(event.id)) && (cfwInvokeUtiCallback(&event)))
            continue;
    }
    // never exit
}

void malInit(void)
{
    gMalCtx.cb_thread = osiThreadCreate(
        "mal", prvMalThreadEntry, NULL, OSI_PRIORITY_NORMAL,
        MAL_THREAD_STACK_SIZE, MAL_THREAD_EVENT_COUNT);
}

void malSetTransContext(malTransaction_t *trans, void *ctx, malContextDelete_t destroy)
{
    if (trans == NULL)
        return;

    trans->ctx = ctx;
    trans->ctx_delete = destroy;
}

malTransaction_t *malCreateTrans(void)
{
    malTransaction_t *trans = (malTransaction_t *)calloc(1, sizeof(*trans));
    if (trans == NULL)
        return NULL;

    trans->wait_sema = osiSemaphoreCreate(1, 0);
    if (trans->wait_sema == NULL)
    {
        free(trans);
        return NULL;
    }

    return trans;
}

void malStartUtiTrans(malTransaction_t *trans, malTransCallback_t cb)
{
    malContext_t *d = &gMalCtx;
    trans->type = MAL_TRANS_UTI;
    trans->uti = cfwRequestUTIEx((osiEventCallback_t)cb, trans, d->cb_thread, true);
}

void malAbortTrans(malTransaction_t *trans)
{
    if (trans == NULL)
        return;

    if (trans->type == MAL_TRANS_UTI)
        cfwReleaseUTI(trans->uti);
}

void malTerminateTrans(malTransaction_t *trans)
{
    if (trans == NULL)
        return;

    if (trans->ctx_delete != NULL)
        trans->ctx_delete(trans->ctx);
    osiSemaphoreDelete(trans->wait_sema);
    free(trans);
}

uint16_t malTransUti(malTransaction_t *trans)
{
    return trans->uti;
}

void malTransFinished(malTransaction_t *trans)
{
    if (trans == NULL)
        return;

    osiSemaphoreRelease(trans->wait_sema);
}

bool malTransWait(malTransaction_t *trans, unsigned timeout)
{
    if (trans == NULL)
        return false;

    return osiSemaphoreTryAcquire(trans->wait_sema, timeout);
}

void malContextFree(void *ctx)
{
    free(ctx);
}

void *malTransContext(malTransaction_t *trans)
{
    return trans->ctx;
}
