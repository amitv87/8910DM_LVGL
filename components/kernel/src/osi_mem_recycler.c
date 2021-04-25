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

#include "osi_mem_recycler.h"
#include "osi_api.h"
#include "osi_log.h"
#include <string.h>
#include <stdlib.h>

struct osiMemRecycler
{
    size_t size;
    size_t count;
    size_t max_count;
    void *ptrs[1];
};

osiMemRecycler_t *osiMemRecyclerCreate(size_t depth)
{
    size_t mem_size = sizeof(osiMemRecycler_t) + (depth - 1) * sizeof(void *);
    osiMemRecycler_t *d = calloc(1, mem_size);
    if (d == NULL)
        return NULL;

    d->size = depth;
    d->count = 0;
    d->max_count = 0;
    return d;
}

void osiMemRecyclerDelete(osiMemRecycler_t *d)
{
    osiMemRecyclerEmpty(d);
    free(d);
}

void osiMemRecyclerEmpty(osiMemRecycler_t *d)
{
    if (d == NULL)
        return;

    for (int n = 0; n < d->count; n++)
        free(d->ptrs[n]);
    d->count = 0;
}

static inline int _index(osiMemRecycler_t *d, const void *p)
{
    for (int n = 0; n < d->count; n++)
    {
        if (d->ptrs[n] == p)
            return n;
    }
    return -1;
}

bool osiMemRecyclerPut(osiMemRecycler_t *d, const void *p)
{
    if (p == NULL)
        return true;

    if (d == NULL)
        return false;

    if (d->count >= d->size)
        return false;

    int idx = _index(d, p);
    if (idx >= 0) // already exists
        return false;

    d->ptrs[d->count] = (void *)p;
    d->count++;
    if (d->count > d->max_count)
        d->max_count = d->count;
    return true;
}

bool osiMemRecyclerUndoPut(osiMemRecycler_t *d, const void *p)
{
    if (d == NULL || p == NULL)
        return false;

    int idx = _index(d, p);
    if (idx < 0) // not exists
        return false;

    d->ptrs[idx] = d->ptrs[d->count - 1];
    d->count--;
    return true;
}
