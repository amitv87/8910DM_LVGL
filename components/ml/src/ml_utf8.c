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

#include "ml_internal.h"

static int _readChar(const void *in, unsigned size, unsigned *val)
{
    const uint8_t *pin = (const uint8_t *)in;
    if (size < 1)
        goto not_enough;

    uint32_t ch1 = *pin++;
    if (ch1 <= 0x80)
    {
        *val = ch1;
        return 1;
    }

    if ((ch1 & 0xe0) == 0xc0)
    {
        if (size < 2)
            goto not_enough;

        uint32_t ch2 = *pin++;
        if ((ch2 & 0xc0) != 0x80)
            goto invalid;

        *val = ((ch1 & 0x1f) << 6) | (ch2 & 0x3f);
        return 2;
    }
    else if ((ch1 & 0xf0) == 0xe0)
    {
        if (size < 3)
            goto not_enough;

        uint32_t ch2 = *pin++;
        if ((ch2 & 0xc0) != 0x80)
            goto invalid;
        uint32_t ch3 = *pin++;
        if ((ch3 & 0xc0) != 0x80)
            goto invalid;

        *val = ((ch1 & 0xf) << 12) | ((ch2 & 0x3f) << 6) | (ch3 & 0x3f);
        return 3;
    }
    else if ((ch1 & 0xf8) == 0xf0)
    {
        if (size < 3)
            goto not_enough;

        uint32_t ch2 = *pin++;
        if ((ch2 & 0xc0) != 0x80)
            goto invalid;
        uint32_t ch3 = *pin++;
        if ((ch3 & 0xc0) != 0x80)
            goto invalid;
        uint32_t ch4 = *pin++;
        if ((ch4 & 0xc0) != 0x80)
            goto invalid;

        *val = ((ch1 & 0x7) << 18) | ((ch2 & 0x3f) << 12) | ((ch3 & 0x3f) << 6) | (ch4 & 0x3f);
        return 4;
    }

invalid:
    *val = INVALID_CODE_POINT;
    return 1;

not_enough:
    *val = 0;
    return size;
}

static int _writeChar(void *out, unsigned size, unsigned val)
{
    uint8_t *pout = (uint8_t *)out;
    if (val <= 0x7f)
    {
        if (pout != NULL && size >= 1)
            *pout++ = val;
        return 1;
    }
    else if (val <= 0x7ff)
    {
        if (pout != NULL && size >= 2)
        {
            *pout++ = (val >> 6) | 0xc0;
            *pout++ = (val & 0x3f) | 0x80;
        }
        return 2;
    }
    else if (val <= 0xffff)
    {
        if (pout != NULL && size >= 3)
        {
            *pout++ = (val >> 12) | 0xe0;
            *pout++ = ((val >> 6) & 0x3f) | 0x80;
            *pout++ = (val & 0x3f) | 0x80;
        }
        return 3;
    }
    else if (val <= 0x10ffff)
    {
        if (pout != NULL && size >= 4)
        {
            *pout++ = (val >> 18) | 0xe0;
            *pout++ = ((val >> 12) & 0x3f) | 0x80;
            *pout++ = ((val >> 6) & 0x3f) | 0x80;
            *pout++ = (val & 0x3f) | 0x80;
        }
        return 4;
    }
    else
    {
        return -1;
    }
}

static mlCharset_t utf8Chset = {
    .chset = ML_UTF8,
    .min_bytes = 1,
    .max_bytes = 4,
    .invalid_fill_char = '?',
    .read_char = _readChar,
    .write_char = _writeChar,
};

void mlAddUtf8(void)
{
    mlAddCharset(&utf8Chset);
}
