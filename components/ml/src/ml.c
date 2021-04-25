/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "ml_internal.h"
#include "osi_log.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>

typedef struct
{
    mlCharsetHead_t chsets;
} mlContext_t;

static mlContext_t gMlCtx;

static mlCharset_t *_findCharset(unsigned chset)
{
    mlContext_t *p = &gMlCtx;
    mlCharset_t *ch;
    SLIST_FOREACH(ch, &p->chsets, iter)
    {
        if (ch->chset == chset)
            return ch;
    }
    return NULL;
}

void mlInit(void)
{
    mlContext_t *p = &gMlCtx;
    SLIST_INIT(&p->chsets);

    mlAddIso8859_1();
    mlAddGsm();
    mlAddUtf8();
    mlAddUtf16be();
    mlAddUtf16le();
    mlAddCP936();
}

void mlAddCharset(mlCharset_t *conv)
{
    mlContext_t *p = &gMlCtx;
    SLIST_INSERT_HEAD(&p->chsets, conv, iter);
}

static bool _charByteCount(const void *s, int size, unsigned chset, int *char_count, int *byte_count)
{
    mlCharset_t *ch = _findCharset(chset);
    if (ch == NULL || s == NULL)
        return false;

    if (size < 0)
        size = INT_MAX;

    unsigned code_point;
    int chars = 0;
    int bytes = 0;
    while (size > 0)
    {
        int rsize = ch->read_char(s, size, &code_point);
        if (code_point == 0)
            break;
        size -= rsize;
        bytes += rsize;
        chars++;
    }

    *char_count = chars;
    *byte_count = bytes;
    return true;
}

int mlCharCount(const void *s, unsigned chset)
{
    int chars = -1;
    int bytes = -1;
    _charByteCount(s, -1, chset, &chars, &bytes);
    return chars;
}

int mlByteCount(const void *s, unsigned chset)
{
    int chars = -1;
    int bytes = -1;
    _charByteCount(s, -1, chset, &chars, &bytes);
    return bytes;
}

void *mlConvertStr(const void *from, int from_size, unsigned from_chset, unsigned to_chset, int *pto_size)
{
    if (from == NULL)
        return NULL;

    mlCharset_t *fch = _findCharset(from_chset);
    mlCharset_t *tch = _findCharset(to_chset);
    if (fch == NULL || tch == NULL)
        return NULL;

    int char_count, byte_count;
    if (!_charByteCount(from, from_size, from_chset, &char_count, &byte_count))
        return NULL;

    if (from_size < byte_count)
        from_size = byte_count;

    int to_mem_size = (char_count + 1) * tch->max_bytes;
    void *to = calloc(1, to_mem_size);
    uint8_t *pt = (uint8_t *)to;
    const uint8_t *pf = (const uint8_t *)from;
    int to_size = 0;

    unsigned codepoint;
    while (from_size > 0)
    {
        int rsize = fch->read_char(pf, from_size, &codepoint);
        pf += rsize;
        from_size -= rsize;
        if (codepoint == 0)
            break;

        int wsize = tch->write_char(pt, INT_MAX, codepoint);
        if (wsize < 0)
            wsize = tch->write_char(pt, INT_MAX, tch->invalid_fill_char);
        to_size += wsize;
        pt += wsize;
    }

    tch->write_char(pt, INT_MAX, 0);
    if (pto_size != NULL)
        *pto_size = to_size;
    return to;
}

uint16_t mlGetUnicode(uint16_t from, uint32_t from_chset)
{
    mlCharset_t *fch = _findCharset(from_chset);
    if (fch == NULL)
        return 0;

    unsigned codepoint;

    int rsize = fch->read_char(&from, sizeof(uint16_t), &codepoint);

    if (rsize > 0)
        return codepoint;

    return 0;
}

uint16_t mlGetOEM(uint32_t unicd, uint32_t to_chset)
{
    unsigned ret;

    mlCharset_t *tch = _findCharset(to_chset);
    if (tch == NULL)
        return 0;

    int wsize = tch->write_char((char *)&ret, 2, unicd);

    if (wsize > 0)
        return ret;

    return 0;
}
