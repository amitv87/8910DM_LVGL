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

#include "osi_line_cache.h"
#include "osi_api.h"
#include <stdlib.h>
#include <string.h>

struct osiLineCache
{
    char *data;
    unsigned size;
    unsigned wpos;
    osiLineCacheOutput_t sender;
    void *owner;
    osiMutex_t *lock;
};

osiLineCache_t *osiLineCacheCreate(osiLineCacheOutput_t sender, unsigned size)
{
    if (sender == NULL || size == 0)
        return NULL;

    osiLineCache_t *d = calloc(1, sizeof(osiLineCache_t) + size);
    if (d == NULL)
        return NULL;

    d->data = (char *)d + sizeof(osiLineCache_t);
    d->size = size;
    d->wpos = 0;
    d->sender = sender;
    d->owner = NULL;
    d->lock = osiMutexCreate();
    return d;
}

void osiLineCacheDelete(osiLineCache_t *d)
{
    if (d == NULL)
        return;

    osiMutexDelete(d->lock);
    free(d);
}

static void prvSend(osiLineCache_t *d)
{
    if (d->wpos > 0)
    {
        d->sender(d->owner, d->data, d->wpos);
        d->wpos = 0;
    }
}

void osiLineCacheWrite(osiLineCache_t *d, void *owner, const void *data, unsigned size)
{
    if (data == NULL || size == 0)
        return;

    osiMutexLock(d->lock);

    if (owner != d->owner)
        prvSend(d);

    d->owner = owner;
    while (size > 0)
    {
        unsigned bsize = OSI_MIN(unsigned, size, d->size - d->wpos);
        memcpy(d->data + d->wpos, data, bsize);
        data = (const char *)data + bsize;
        size -= bsize;
        d->wpos += bsize;

        if (d->wpos >= d->size)
            prvSend(d);
    }

    osiMutexUnlock(d->lock);
}

void osiLineCacheFlush(osiLineCache_t *d)
{
    osiMutexLock(d->lock);
    prvSend(d);
    osiMutexUnlock(d->lock);
}
