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

// 3GPP 23.038

#define ESCAPE_CHAR (0x1b)

typedef struct
{
    uint16_t from;
    uint16_t to;
} u16map_t;

static const uint16_t gsmBasic[128] = {
    0x40, 0xa3, 0x24, 0xa5, 0xe8, 0xe9, 0xf9, 0xec,
    0xf2, 0xc7, 0x0a, 0xd8, 0xf8, 0x0d, 0xc5, 0xe5,
    0x394, 0x5f, 0x3a6, 0x393, 0x39b, 0x3a9, 0x3a0, 0x3a8,
    0x3a3, 0x398, 0x39e, 0x1b, 0xc6, 0xe6, 0xdf, 0xc9,
    0x20, 0x21, 0x22, 0x23, 0xa4, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0xa1, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5a, 0xc4, 0xd6, 0xd1, 0xdc, 0xa7,
    0xbf, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0xe4, 0xf6, 0xf1, 0xfc, 0xe0};

static const u16map_t gsmExt[] = {
    {0x0a, 0x0a},
    {0x14, 0x5e},
    {0x1b, 0x20},
    {0x28, 0x7b},
    {0x29, 0x7d},
    {0x2f, 0x5c},
    {0x3c, 0x5b},
    {0x3d, 0x7e},
    {0x3e, 0x5d},
    {0x40, 0x7c},
    {0x65, 0x20ac}};

static int _readChar(const void *in, unsigned size, unsigned *val)
{
    const uint8_t *pin = (const uint8_t *)in;
    if (size < 1)
        goto not_enough;

    uint32_t ch1 = *pin++;
    if (ch1 >= 0x80)
        goto invalid;

    if (ch1 != ESCAPE_CHAR)
    {
        // *val = ch1;
        *val = gsmBasic[ch1];
        return 1;
    }

    if (size < 2)
        goto not_enough;

    uint32_t ch2 = *pin++;
    for (int n = 0; n < OSI_ARRAY_SIZE(gsmExt); n++)
    {
        if (ch2 == gsmExt[n].from)
        {
            *val = gsmExt[n].to;
            return 2;
        }
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
    if (val == ESCAPE_CHAR)
        return -1;

    for (int n = 0; n < OSI_ARRAY_SIZE(gsmBasic); n++)
    {
        if (val == gsmBasic[n])
        {
            if (pout != NULL && size >= 1)
                // *pout++ = val;
                *pout++ = n;
            return 1;
        }
    }

    for (int n = 0; n < OSI_ARRAY_SIZE(gsmExt); n++)
    {
        if (val == gsmExt[n].to)
        {
            if (pout != NULL && size >= 2)
            {
                *pout++ = ESCAPE_CHAR;
                *pout++ = gsmExt[n].from;
            }
            return 2;
        }
    }

#if 0
    if (val < 0x80)
    {
        *pout++ = val;
        return 1;
    }
#endif

    return -1;
}

static mlCharset_t gsmChset = {
    .chset = ML_GSM,
    .min_bytes = 1,
    .max_bytes = 2,
    .invalid_fill_char = '?',
    .read_char = _readChar,
    .write_char = _writeChar,
};

void mlAddGsm(void)
{
    mlAddCharset(&gsmChset);
}
