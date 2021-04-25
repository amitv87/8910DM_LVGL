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

    *val = *pin;
    return 1;

not_enough:
    *val = 0;
    return size;
}

static int _writeChar(void *out, unsigned size, unsigned val)
{
    uint8_t *pout = (uint8_t *)out;
    if (val < 0x100)
    {
        if (pout != NULL && size >= 1)
            *pout++ = val;
        return 1;
    }
    return -1;
}

static mlCharset_t iso8859_1Chset = {
    .chset = ML_ISO8859_1,
    .min_bytes = 1,
    .max_bytes = 1,
    .invalid_fill_char = '?',
    .read_char = _readChar,
    .write_char = _writeChar,
};

void mlAddIso8859_1(void)
{
    mlAddCharset(&iso8859_1Chset);
}
