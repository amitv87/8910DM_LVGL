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

static uint16_t _readBE(const uint8_t *pin)
{
    return (pin[0] << 8) | pin[1];
}
static uint16_t _readLE(const uint8_t *pin)
{
    return (pin[1] << 8) | pin[0];
}
static void _writeBE(uint8_t *pout, uint32_t val)
{
    *pout++ = (val >> 8) & 0xff;
    *pout = val & 0xff;
}
static void _writeLE(uint8_t *pout, uint32_t val)
{
    *pout++ = val & 0xff;
    *pout = (val >> 8) & 0xff;
}

static int _readChar(const void *in, unsigned size, unsigned *val, uint16_t (*readc)(const uint8_t *pin))
{
    const uint8_t *pin = (const uint8_t *)in;
    if (size < 2)
        goto not_enough;

    uint32_t ch1 = readc(pin);
    if ((ch1 >= 0 && ch1 < 0xd800) || (ch1 >= 0xe000 && ch1 < 0x10000))
    {
        *val = ch1;
        return 2;
    }

    if (!(ch1 >= 0xd800 && ch1 < 0xdc00))
        goto invalid;
    if (size < 4) // not enough for valid character
        goto not_enough;

    uint32_t ch2 = readc(pin + 2);
    if (!(ch2 >= 0xdc00 && ch2 < 0xe000))
        goto invalid;

    *val = ((ch1 - 0xd800) << 10) | (ch2 - 0xdc00);
    return 4;

invalid:
    *val = INVALID_CODE_POINT;
    return 2;

not_enough:
    *val = 0;
    return size;
}

static int _readCharBE(const void *in, unsigned size, unsigned *val)
{
    return _readChar(in, size, val, _readBE);
}

static int _readCharLE(const void *in, unsigned size, unsigned *val)
{
    return _readChar(in, size, val, _readLE);
}

static int _writeChar(void *out, unsigned size, unsigned val, void (*writec)(uint8_t *pout, uint32_t val))
{
    uint8_t *pout = (uint8_t *)out;
    if ((val >= 0xd800 && val < 0xe000) || (val > 0x100000))
        return -1;

    if ((val >= 0 && val < 0xd800) || (val >= 0xe000 && val < 0x10000))
    {
        if (pout != NULL && size >= 2)
            writec(pout, val);
        return 2;
    }

    uint16_t ch1 = 0xd800 + ((val >> 10) & 0x3ff);
    uint16_t ch2 = 0xdc00 + (val & 0x3ff);
    if (pout != NULL && size >= 4)
    {
        writec(pout, ch1);
        writec(pout + 2, ch2);
    }
    return 4;
}

static int _writeCharBE(void *out, unsigned size, unsigned val)
{
    return _writeChar(out, size, val, _writeBE);
}

static int _writeCharLE(void *out, unsigned size, unsigned val)
{
    return _writeChar(out, size, val, _writeLE);
}

static mlCharset_t utf16beChset = {
    .chset = ML_UTF16BE,
    .min_bytes = 2,
    .max_bytes = 4,
    .invalid_fill_char = '?',
    .read_char = _readCharBE,
    .write_char = _writeCharBE,
};

static mlCharset_t utf16leChset = {
    .chset = ML_UTF16LE,
    .min_bytes = 2,
    .max_bytes = 4,
    .invalid_fill_char = '?',
    .read_char = _readCharLE,
    .write_char = _writeCharLE,
};

void mlAddUtf16be(void)
{
    mlAddCharset(&utf16beChset);
}

void mlAddUtf16le(void)
{
    mlAddCharset(&utf16leChset);
}
