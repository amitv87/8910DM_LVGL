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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "osi_event_hub.h"
#include "osi_vsmap.h"
#include "osi_api.h"
#include "osi_log.h"
#include <string.h>
#include <stdlib.h>

typedef struct
{
    uint32_t id;
    osiEventCallback_t cb;
    void *cb_ctx;
} osiEventCallbackReg_t;

typedef struct
{
    uint32_t id;
    osiEventHandler_t handler;
} osiEventHandlerReg_t;

struct osiEventHub
{
    size_t size;
    size_t count;
    size_t max_count;
    osiEventHandlerReg_t *regs;
};

struct osiEventDispatch
{
    size_t size;
    size_t count;
    size_t max_count;
    osiEventCallbackReg_t *regs;
};

osiEventHub_t *osiEventHubCreate(size_t depth)
{
    size_t mem_size = sizeof(osiEventHub_t) + depth * sizeof(osiEventHandlerReg_t);
    uintptr_t mem = (uintptr_t)calloc(1, mem_size);
    if ((void *)mem == NULL)
        return NULL;

    osiEventHub_t *p = (osiEventHub_t *)OSI_PTR_INCR_POST(mem, sizeof(osiEventHub_t));
    p->regs = (osiEventHandlerReg_t *)OSI_PTR_INCR_POST(mem, depth * sizeof(osiEventHandlerReg_t));
    p->count = 0;
    p->max_count = 0;
    p->size = depth;
    return p;
}

void osiEventHubDelete(osiEventHub_t *p)
{
    free(p);
}

static osiEventHandlerReg_t *_findHandler(osiEventHub_t *p, uint32_t id)
{
    if (p->count == 0)
        return NULL;
    return (osiEventHandlerReg_t *)bsearch(&id, p->regs, p->count, sizeof(p->regs[0]), osiUintIdCompare);
}

bool osiEventHubRegister(osiEventHub_t *p, uint32_t id, osiEventHandler_t handler)
{
    if (p->count >= p->size || id == 0xffffffff)
        return false;

    osiEventHandlerReg_t *reg = _findHandler(p, id);
    if (reg != NULL)
    {
        reg->handler = handler;
        return true;
    }

    p->regs[p->count].id = id;
    p->regs[p->count].handler = handler;
    p->count++;
    if (p->count > p->max_count)
        p->max_count = p->count;

    qsort(p->regs, p->count, sizeof(p->regs[0]), osiUintIdCompare);
    OSI_LOGD(0, "ID HUB REG: %d count/%d", id, p->count);
    return true;
}

bool osiEventHubBatchRegister(osiEventHub_t *p, uint32_t id, ...)
{
    va_list ap;
    va_start(ap, id);
    bool ok = osiEventHubVBatchRegister(p, id, ap);
    va_end(ap);
    return ok;
}

bool osiEventHubVBatchRegister(osiEventHub_t *p, uint32_t id, va_list ap)
{
    bool ok = true;
    while (id != 0)
    {
        if (p->count >= p->size || id == 0xffffffff)
        {
            ok = false;
            break;
        }

        osiEventHandler_t handler = va_arg(ap, osiEventHandler_t);
        osiEventHandlerReg_t *reg = _findHandler(p, id);
        if (reg != NULL)
        {
            reg->handler = handler;
        }
        else
        {
            p->regs[p->count].id = id;
            p->regs[p->count].handler = handler;
            p->count++;
        }

        id = va_arg(ap, uint32_t);
    }

    if (p->count > p->max_count)
        p->max_count = p->count;

    qsort(p->regs, p->count, sizeof(p->regs[0]), osiUintIdCompare);
    OSI_LOGD(0, "ID HUB BATCH REG: count/%d", p->count);
    return ok;
}

bool osiEventHubRun(osiEventHub_t *p, const osiEvent_t *event)
{
    if (p->count == 0)
        return false;

    osiEventHandlerReg_t *reg = _findHandler(p, event->id);
    if (reg == NULL)
        return false;

    if (reg->handler != NULL)
        reg->handler(event);
    return true;
}

osiEventDispatch_t *osiEventDispatchCreate(size_t depth)
{
    size_t mem_size = sizeof(osiEventDispatch_t) + depth * sizeof(osiEventCallbackReg_t);
    uintptr_t mem = (uintptr_t)calloc(1, mem_size);
    if ((void *)mem == NULL)
        return NULL;

    osiEventDispatch_t *p = (osiEventDispatch_t *)OSI_PTR_INCR_POST(mem, sizeof(osiEventDispatch_t));
    p->regs = (osiEventCallbackReg_t *)OSI_PTR_INCR_POST(mem, depth * sizeof(osiEventCallbackReg_t));
    p->count = 0;
    p->max_count = 0;
    p->size = depth;
    return p;
}

void osiEventDispatchDelete(osiEventDispatch_t *p)
{
    free(p);
}

static osiEventCallbackReg_t *_findCallback(osiEventDispatch_t *p, uint32_t id)
{
    if (p->count == 0)
        return NULL;
    return (osiEventCallbackReg_t *)bsearch(&id, p->regs, p->count, sizeof(p->regs[0]), osiUintIdCompare);
}

bool osiEventDispatchRegister(osiEventDispatch_t *p, uint32_t id, osiEventCallback_t cb, void *cb_ctx)
{
    if (p->count >= p->size || cb == NULL)
        return false;

    osiEventCallbackReg_t *reg = _findCallback(p, id);
    if (reg != NULL)
        return false;

    p->regs[p->count].id = id;
    p->regs[p->count].cb = cb;
    p->regs[p->count].cb_ctx = cb_ctx;
    p->count++;
    if (p->count > p->max_count)
        p->max_count = p->count;

    qsort(p->regs, p->count, sizeof(p->regs[0]), osiUintIdCompare);
    OSI_LOGD(0, "ID REG: %d count/%d", id, p->count);
    return true;
}

bool osiEventDispatchRun(osiEventDispatch_t *p, const osiEvent_t *event)
{
    if (p->count == 0)
        return false;

    osiEventCallbackReg_t *reg = _findCallback(p, event->id);
    if (reg == NULL)
        return false;

    osiEventCallbackReg_t found = *reg;
    reg->id = 0xffffffff; // set to maximum value to sort it to the last one
    qsort(p->regs, p->count, sizeof(p->regs[0]), osiUintIdCompare);
    p->count--;

    if (found.cb != NULL)
        found.cb(found.cb_ctx, event);
    return true;
}
