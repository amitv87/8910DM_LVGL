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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('C', 'F', 'W', 'P')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#include "quec_proj_config.h"
#include "osi_log.h"
#include "osi_vsmap.h"
#include "cfw.h"
#include "cfw_dispatch.h"
#include "cfw.h"
#include "cfw_event.h"
#include <string.h>
#include <stdlib.h>

typedef struct
{
    uint16_t uti;
    bool auto_remove;
    osiThread_t *thread;
    osiEventCallback_t cb;
    void *cb_ctx;
} cfwUtiRegistry_t;

typedef struct
{
    osiMutex_t *lock;
    osiThread_t *main_thread;
    cfwUtiRegistry_t pending_uti_regs[CONFIG_CFW_PENDING_UTI_COUNT];
    uint16_t pending_uti_count;
} cfwContext_t;

static cfwContext_t *gCfwCtx = NULL;

bool cfwIsCfwEvent(uint32_t id)
{
	 return ((id >= EV_CFW_START_ && id < EV_CFW_END_)
#ifdef CONFIG_QUEC_PROJECT_FEATURE_TCPIP
	    ||id == QL_TCPIP_EVENT
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_HTTP
	    ||id == QL_HTTP_EVENT
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_WIFISCAN
		||id == QL_WIFISCAN_SYNC_RSP
#endif
	    );
}

bool cfwIsCfwIndicate(uint32_t id)
{
    return ((id >= EV_CFW_IND_BASE && id < EV_CFW_IND_END_) || (id >= EV_WIFI_IND_BASE && id < EV_WIFI_IND_END_) ||
            (id == EV_BT_TETHER_DATA_IND) || (id == EV_CFW_CC_INITIATE_SPEECH_CALL_RSP) ||
            (id == EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP) || (id == EV_CFW_SAT_RESPONSE_RSP) ||
            (id == EV_INTER_APS_TCPIP_REQ) || (id >= EV_TCPIP_CFW_GPRS_ACT && id <= EV_TCPIP_ETHCARD_DISCONNECT));
}

void cfwDispatchInit(void)
{
    if (gCfwCtx != NULL)
        return;

    cfwContext_t *p = (cfwContext_t *)calloc(1, sizeof(cfwContext_t));
    p->lock = osiMutexCreate();
    gCfwCtx = p;
}

void cfwDispatchSetMainThread(osiThread_t *main_thread)
{
    gCfwCtx->main_thread = main_thread;
}

static int prvFindRegistryByUti(cfwContext_t *p, uint16_t uti)
{
    for (uint16_t n = 0; n < p->pending_uti_count; n++)
    {
        if (p->pending_uti_regs[n].uti == uti)
            return n;
    }

    return -1;
}

static void prvRemoveRegistry(cfwContext_t *p, uint16_t n)
{
    uint16_t last = p->pending_uti_count - 1;
    if (n < last)
        p->pending_uti_regs[n] = p->pending_uti_regs[last];
    p->pending_uti_count--;
}

uint16_t cfwRequestUTIEx(osiEventCallback_t cb, void *cb_ctx, osiThread_t *thread, bool auto_remove)
{
    cfwContext_t *p = gCfwCtx;

    uint8_t uti = 0;
    CFW_GetFreeUTI(0 /* not used */, &uti);

    osiMutexLock(p->lock);

    if (p->pending_uti_count >= CONFIG_CFW_PENDING_UTI_COUNT)
    {
        OSI_LOGE(0x100056e8, "Too many pending UTI");
        osiPanic();
    }

    if (prvFindRegistryByUti(p, uti) >= 0)
        OSI_LOGE(0x100056e9, "Request UTI %d already registed", uti);
    else
        OSI_LOGI(0x100056ea, "Request UTI %d", uti);

    if (thread == NULL)
        thread = osiThreadCurrent();

    p->pending_uti_regs[p->pending_uti_count].uti = uti;
    p->pending_uti_regs[p->pending_uti_count].thread = thread;
    p->pending_uti_regs[p->pending_uti_count].cb = cb;
    p->pending_uti_regs[p->pending_uti_count].cb_ctx = cb_ctx;
    p->pending_uti_regs[p->pending_uti_count].auto_remove = auto_remove;
    p->pending_uti_count++;

    osiMutexUnlock(p->lock);
    return uti;
}

uint16_t cfwRequestUTI(osiEventCallback_t cb, void *cb_ctx)
{
    return cfwRequestUTIEx(cb, cb_ctx, NULL, true);
}

uint16_t cfwRequestNoWaitUTI(void)
{
    uint8_t uti = 0;
    CFW_GetFreeUTI(0 /* not used */, &uti);
    OSI_LOGI(0x100056eb, "Request no wait UTI %d", uti);
    return uti;
}

void cfwReleaseUTI(uint16_t uti)
{
    cfwContext_t *p = gCfwCtx;

    osiMutexLock(p->lock);

    int n = prvFindRegistryByUti(p, uti);
    if (n < 0)
    {
        OSI_LOGE(0x100056ec, "UTI %d is not registered", uti);
    }
    else
    {
        prvRemoveRegistry(p, n);
        OSI_LOGE(0x100056ed, "Release UTI %d", uti);
    }

    osiMutexUnlock(p->lock);
}

void cfwDispatchSendEvent(const osiEvent_t *event)
{
    cfwContext_t *p = gCfwCtx;

    if (cfwIsCfwEvent(event->id))
    {
        const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

        uint16_t uti = cfw_event->nUTI;
        osiMutexLock(p->lock);

        int n = prvFindRegistryByUti(p, uti);
        if (n >= 0)
        {
            osiThread_t *thread = p->pending_uti_regs[n].thread;
            osiMutexUnlock(p->lock);

            osiEventSend(thread, event);
            return;
        }

        osiMutexUnlock(p->lock);
    }

    // When the UTI is not registered, send the event to main thread
    if (p->main_thread != NULL)
        osiEventSend(p->main_thread, event);
    return;
}

bool cfwInvokeUtiCallback(const osiEvent_t *event)
{
    cfwContext_t *p = gCfwCtx;

    if (!cfwIsCfwEvent(event->id))
        return false;

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    uint16_t uti = cfw_event->nUTI;
    osiMutexLock(p->lock);

    int n = prvFindRegistryByUti(p, uti);
    if (n >= 0)
    {
        osiEventCallback_t cb = p->pending_uti_regs[n].cb;
        void *cb_ctx = p->pending_uti_regs[n].cb_ctx;
        if (p->pending_uti_regs[n].auto_remove)
            prvRemoveRegistry(p, n);

        osiMutexUnlock(p->lock);

        if (cb != NULL)
            cb(cb_ctx, event);
        return true;
    }

    osiMutexUnlock(p->lock);
    return false;
}
