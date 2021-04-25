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

#include "osi_order_list.h"
#include "osi_vsmap.h"
#include <stdlib.h>
#include <string.h>

#define FIRST (d->records)
#define LAST (d->records + (TOTAL_SIZE - d->record_size))
#define TOTAL_SIZE (d->count * d->record_size)
#define NEXT(p) (p + d->record_size)
#define PREV(p) (p - d->record_size)

struct osiOrderList
{
    int (*compar)(const void *, const void *);
    bool replace;
    unsigned record_size;
    unsigned count;
    unsigned max_count;
    char records[0]; // 4 bytes aligned
};

/**
 * Create the order list
 */
osiOrderList_t *osiOrderListCreate(unsigned record_size, int (*compar)(const void *, const void *),
                                   unsigned max_count, bool replace)
{
    if (max_count < 4 || compar == NULL || record_size == 0)
        return NULL;

    osiOrderList_t *d = (osiOrderList_t *)calloc(1, sizeof(osiOrderList_t) + record_size * max_count);
    if (d == NULL)
        return NULL;

    d->record_size = record_size;
    d->compar = compar;
    d->count = 0;
    d->max_count = max_count;
    d->replace = replace;
    return d;
}

/**
 * Delete the order list
 */
void osiOrderListDelete(osiOrderList_t *d)
{
    if (d == NULL)
        return;
    free(d);
}

/**
 * Clear the order list, by just set count to 0.
 */
void osiOrderListClear(osiOrderList_t *d)
{
    d->count = 0;
}

/**
 * The first element of order list
 */
const void *osiOrderListFirst(osiOrderList_t *d)
{
    return (d->count == 0) ? NULL : FIRST;
}

/**
 * The last element of order list
 */
const void *osiOrderListLast(osiOrderList_t *d)
{
    return (d->count == 0) ? NULL : LAST;
}

/**
 * Element count of order list
 */
unsigned osiOrderListCount(osiOrderList_t *d)
{
    return d->count;
}

/**
 * Insert element into order list
 */
const void *osiOrderListInsert(osiOrderList_t *d, const void *val)
{
    if (d->count == 0)
    {
        memcpy(FIRST, val, d->record_size);
        d->count = 1;
        return FIRST;
    }

    bool full = (d->count >= d->max_count);
    if (full && !d->replace)
        return NULL;

    if (full)
    {
        if (d->compar(NEXT(FIRST), val) > 0)
        {
            memcpy(FIRST, val, d->record_size);
            return FIRST;
        }

        if (d->compar(val, LAST) > 0)
        {
            memmove(FIRST, NEXT(FIRST), (d->count - 1) * d->record_size);
            memcpy(LAST, val, d->record_size);
            return LAST;
        }
    }
    else
    {
        if (d->compar(val, LAST) > 0)
        {
            char *insert = NEXT(LAST);
            memcpy(insert, val, d->record_size);
            d->count++;
            return insert;
        }
    }

    char *r = (char *)osiOrderListLowerBound(d, val); // can't be NULL
    int rcomp = d->compar(r, val);
    if (rcomp == 0)
    {
        memcpy(r, val, d->record_size); // replace, and count not changed
        return r;
    }

    if (full)
    {
        // Example: [3] > val ==> move count 2, use [2]
        memmove(FIRST, NEXT(FIRST), (r - FIRST) - d->record_size);
        memcpy(PREV(r), val, d->record_size);
        return PREV(r);
    }

    // Example: [3] > val, total 7 ==> move count 4, use [3]
    memmove(NEXT(r), r, TOTAL_SIZE - (r - FIRST));
    memcpy(r, val, d->record_size);
    d->count++;
    return r;
}

/**
 * Find (r >= val), or the last one
 */
const void *osiOrderListLowerBound(osiOrderList_t *d, const void *val)
{
    if (d->count == 0)
        return NULL;

    unsigned count = d->count;
    const char *first = FIRST;
    while (count > 0)
    {
        const char *it = first;
        unsigned step = count / 2;
        it += (step * d->record_size);
        if (d->compar(it, val) < 0)
        {
            first = (it + d->record_size);
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }
    return first;
}

/**
 * Find (r > val), or the last one
 */
const void *osiOrderListUpperBound(osiOrderList_t *d, const void *val)
{
    if (d->count == 0)
        return NULL;

    unsigned count = d->count;
    const char *first = FIRST;
    while (count > 0)
    {
        const char *it = first;
        unsigned step = count / 2;
        it += (step * d->record_size);
        if (!(d->compar(val, it) < 0))
        {
            first = (it + d->record_size);
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }
    return first;
}

/**
 * Find (r == val)
 */
const void *osiOrderListFind(osiOrderList_t *d, const void *val)
{
    if (d->count == 0)
        return NULL;

    const void *r = osiOrderListLowerBound(d, val);
    return (d->compar(r, val) == 0) ? r : NULL;
}

/**
 * Find (r < val)
 */
const void *osiOrderListFindLT(osiOrderList_t *d, const void *val)
{
    if (d->count == 0)
        return NULL;

    const char *r = (const char *)osiOrderListLowerBound(d, val);
    if (d->compar(r, val) < 0)
        return r;
    return (r == FIRST) ? NULL : PREV(r);
}

/**
 * Find (r > val)
 */
const void *osiOrderListFindGT(osiOrderList_t *d, const void *val)
{
    if (d->count == 0)
        return NULL;

    const char *r = (const char *)osiOrderListLowerBound(d, val);
    return (d->compar(r, val) > 0) ? r : NULL;
}

/**
 * Find (r <= val)
 */
const void *osiOrderListFindLE(osiOrderList_t *d, const void *val)
{
    if (d->count == 0)
        return NULL;

    const char *r = (const char *)osiOrderListUpperBound(d, val);
    if (d->compar(r, val) <= 0)
        return r;
    return (r == FIRST) ? NULL : PREV(r);
}

/**
 * Find (r >= val)
 */
const void *osiOrderListFindGE(osiOrderList_t *d, const void *val)
{
    if (d->count == 0)
        return NULL;

    const char *r = (const char *)osiOrderListLowerBound(d, val);
    return (d->compar(r, val) >= 0) ? r : NULL;
}

/**
 * Help for unsigned integer compare
 */
int osiUintCompare(const void *a, const void *b)
{
    uint32_t ia = *(const uint32_t *)a;
    uint32_t ib = *(const uint32_t *)b;
    return (ia > ib) ? 1 : (ia < ib) ? -1 : 0;
}

/**
 * Help for signed integer compare
 */
int osiIntCompare(const void *a, const void *b)
{
    int32_t ia = *(const int32_t *)a;
    int32_t ib = *(const int32_t *)b;
    return (ia > ib) ? 1 : (ia < ib) ? -1 : 0;
}
