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

#include "osi_vsmap.h"
#include "osi_api.h"
#include "osi_log.h"
#include <string.h>
#include <stdlib.h>

int osiUintIdCompare(const void *key, const void *p)
{
    uint32_t ik = *(const uint32_t *)key;
    uint32_t ip = *(const uint32_t *)p;
    return (ik > ip) ? 1 : (ik < ip) ? -1 : 0;
}

int osiUint16IdCompare(const void *key, const void *p)
{
    uint16_t ik = *(const uint16_t *)key;
    uint16_t ip = *(const uint16_t *)p;
    return (ik > ip) ? 1 : (ik < ip) ? -1 : 0;
}

bool osiArrayIsSorted(const void *base, size_t nmemb, size_t size,
                      int (*compar)(const void *, const void *))
{
    if (base == NULL || compar == NULL)
        return false;

    const char *curr = (const char *)base;
    for (unsigned n = 1; n < nmemb; n++)
    {
        const char *prev = curr;
        curr += size;
        if (compar(curr, prev) <= 0)
        {
            OSI_LOGE(0, "array 0x%x value %d not sorted", base, n);
            return false;
        }
    }
    return true;
}

bool osiVsmapIsSorted(const osiValueStrMap_t *vs, size_t count)
{
    bool ok = true;
    for (int n = 1; n < count; n++)
    {
        if (vs[n].value <= vs[n - 1].value)
        {
            OSI_LOGXE(OSI_LOGPAR_ISIS, 0, "value not sorted: %d/%s %d/%s",
                      vs[n - 1].value, vs[n - 1].str,
                      vs[n].value, vs[n].str);
            ok = false;
        }
    }
    return ok;
}

const char *osiVsmapBsearch(uint32_t value, const osiValueStrMap_t *vs, size_t count, const char *defval)
{
    osiValueStrMap_t *p = bsearch(&value, vs, count, sizeof(vs[0]), osiUintIdCompare);
    return (p == NULL) ? defval : p->str;
}

bool osiVsmapIsSortedEx(const osiValueStrMap_t *vs, size_t count, size_t size)
{
    bool ok = true;
    const osiValueStrMap_t *pre = vs;
    for (int n = 1; n < count; n++)
    {
        const osiValueStrMap_t *cur = (const osiValueStrMap_t *)((char *)pre + size);
        if (cur->value <= pre->value)
        {
            OSI_LOGXE(OSI_LOGPAR_ISIS, 0, "value not sorted: %d/%s %d/%s",
                      pre->value, pre->str, cur->value, cur->str);
            ok = false;
        }
        pre = cur;
    }
    return ok;
}

const char *osiVsmapBsearchEx(uint32_t value, const osiValueStrMap_t *vs, size_t count, size_t size, const char *defval)
{
    osiValueStrMap_t *p = bsearch(&value, vs, count, size, osiUintIdCompare);
    return (p == NULL) ? defval : p->str;
}

const osiValueStrMap_t *osiVsmapFindByIStr(const osiValueStrMap_t *vsmap, const char *str)
{
    for (;;)
    {
        const osiValueStrMap_t *vs = vsmap++;
        if (vs->str == NULL)
            return NULL;
        if (strcasecmp(vs->str, str) == 0)
            return vs;
    }
    return NULL; // never reach
}

const osiValueStrMap_t *osiVsmapFindByStr(const osiValueStrMap_t *vsmap, const char *str)
{
    for (;;)
    {
        const osiValueStrMap_t *vs = vsmap++;
        if (vs->str == NULL)
            return NULL;
        if (strcmp(vs->str, str) == 0)
            return vs;
    }
    return NULL; // never reach
}

const osiValueStrMap_t *osiVsmapFindByVal(const osiValueStrMap_t *vsmap, uint32_t value)
{
    for (;;)
    {
        const osiValueStrMap_t *vs = vsmap++;
        if (vs->str == NULL)
            return NULL;
        if (vs->value == value)
            return vs;
    }
    return NULL; // never reach
}

const char *osiVsmapFindStr(const osiValueStrMap_t *vsmap, uint32_t value, const char *defval)
{
    const osiValueStrMap_t *vs = osiVsmapFindByVal(vsmap, value);
    return (vs == NULL) ? defval : vs->str;
}

uint32_t osiVsmalFindVal(const osiValueStrMap_t *vsmap, const char *str, uint32_t defval)
{
    const osiValueStrMap_t *vs = osiVsmapFindByStr(vsmap, str);
    return (vs == NULL) ? defval : vs->value;
}

uint32_t osiVsmalFindIVal(const osiValueStrMap_t *vsmap, const char *str, uint32_t defval)
{
    const osiValueStrMap_t *vs = osiVsmapFindByIStr(vsmap, str);
    return (vs == NULL) ? defval : vs->value;
}

bool osiIsUintInList(uint32_t value, const uint32_t *varlist, unsigned count)
{
    for (unsigned n = 0; n < count; n++)
    {
        if (value == *varlist++)
            return true;
    }
    return false;
}

bool osiIsUintInRange(uint32_t value, uint32_t minval, uint32_t maxval)
{
    return (value >= minval && value <= maxval);
}

bool osiIsUintInRanges(uint32_t value, const osiUintRange_t *ranges, unsigned count)
{
    for (unsigned n = 0; n < count; n++)
    {
        if (value >= ranges[n].minval && value <= ranges[n].maxval)
            return true;
    }
    return false;
}

bool osiIsIntInList(int value, const int *varlist, unsigned count)
{
    for (unsigned n = 0; n < count; n++)
    {
        if (value == *varlist++)
            return true;
    }
    return false;
}

bool osiIsIntInRange(int value, int minval, int maxval)
{
    return (value >= minval && value <= maxval);
}

bool osiIsIntInRanges(int value, const osiIntRange_t *ranges, unsigned count)
{
    for (unsigned n = 0; n < count; n++)
    {
        if (value >= ranges[n].minval && value <= ranges[n].maxval)
            return true;
    }
    return false;
}

bool osiIsUint64InList(uint64_t value, const uint64_t *varlist, unsigned count)
{
    for (unsigned n = 0; n < count; n++)
    {
        if (value == *varlist++)
            return true;
    }
    return false;
}

bool osiIsUint64InRange(uint64_t value, uint64_t minval, uint64_t maxval)
{
    return (value >= minval && value <= maxval);
}

bool osiIsUint64InRanges(uint64_t value, const osiUint64Range_t *ranges, unsigned count)
{
    for (unsigned n = 0; n < count; n++)
    {
        if (value >= ranges[n].minval && value <= ranges[n].maxval)
            return true;
    }
    return false;
}

bool osiIsInt64InList(int64_t value, const int64_t *varlist, unsigned count)
{
    for (unsigned n = 0; n < count; n++)
    {
        if (value == *varlist++)
            return true;
    }
    return false;
}

bool osiIsInt64InRange(int64_t value, int64_t minval, int64_t maxval)
{
    return (value >= minval && value <= maxval);
}

bool osiIsInt64InRanges(int64_t value, const osiInt64Range_t *ranges, unsigned count)
{
    for (unsigned n = 0; n < count; n++)
    {
        if (value >= ranges[n].minval && value <= ranges[n].maxval)
            return true;
    }
    return false;
}
