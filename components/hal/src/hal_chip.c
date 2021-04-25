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

#include "hal_chip.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hwregs.h"
#include <stdarg.h>

unsigned halCalcDivider24(unsigned input, unsigned output)
{
    if (input == 0 || output == 0 || output > input)
        return 0;

    unsigned delta = -1U;
    unsigned rnum = 0, rdenom = 0;
    for (unsigned num = 1; num < 0x400; num++)
    {
        uint64_t up = (uint64_t)input * num;
        unsigned denom = (up + output / 2) / output;
        if (denom == 0 || denom >= 0x4000)
            continue;

        unsigned out = up / denom;
        unsigned diff = (out > output) ? out - output : output - out;
        if (diff < delta)
        {
            delta = diff;
            rnum = num;
            rdenom = denom;
        }
        if (diff == 0)
            break;
    }

    if (delta == -1U)
        return 0;
    return (rdenom << 10) | rnum;
}

unsigned halDivider24OutFreq(unsigned input, unsigned divider)
{
    unsigned rdenom = (divider >> 10) & 0x3fff;
    unsigned rnum = (divider >> 0) & 0x3ff;
    return ((uint64_t)input * rnum) / rdenom;
}

unsigned halCalcDivider20(unsigned input, unsigned output)
{
    if (input == 0 || output == 0 || output > (input / 6))
        return 0;

    unsigned delta = -1U;
    unsigned rset = 1;
    unsigned rdiv = 1;
    for (unsigned nset = 16; nset >= 6; nset--)
    {
        unsigned ndiv = (input + (nset * output / 2)) / (nset * output);
        if (ndiv <= 1 || ndiv >= (1 << 16))
            continue;

        unsigned out = input / (nset * ndiv);
        unsigned diff = (out > output) ? out - output : output - out;
        if (diff < delta)
        {
            delta = diff;
            rset = nset;
            rdiv = ndiv;
        }
    }

    if (delta == -1U)
        return 0;
    return ((rset - 1) << 16) | (rdiv - 1);
}

unsigned halDivider20OutFreq(unsigned input, unsigned divider)
{
    unsigned rset = ((divider >> 16) & 0xf) + 1;
    unsigned rdiv = ((divider >> 0) & 0xffff) + 1;
    return input / (rset * rdiv);
}

unsigned halCalHalfDivider(unsigned input, unsigned output)
{
#define HALF_DIVIDER_CHECK(half_div, n, v) OSI_DO_WHILE0(if ((half_div) <= (n)) return (v);)
    unsigned half_div = (input * 2 + output / 2) / output;
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV1P0, 15);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV1P5, 14);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV2P0, 13);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV2P5, 12);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV3P0, 11);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV3P5, 10);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV4P0, 9);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV4P5, 8);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV5P0, 7);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV5P5, 6);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV6P0, 5);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV7P0, 4);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV8P0, 3);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV12P0, 2);
    HALF_DIVIDER_CHECK(half_div, HAL_CLK_DIV16P0, 1);
#undef HALF_DIVIDER_CHECK
    return 0; // HAL_CLK_DIV24P0
}

static const uint8_t gHalfDividerDiv[16] = {
    HAL_CLK_DIV24P0, // 0
    HAL_CLK_DIV16P0, // 1
    HAL_CLK_DIV12P0, // 2
    HAL_CLK_DIV8P0,  // 3
    HAL_CLK_DIV7P0,  // 4
    HAL_CLK_DIV6P0,  // 5
    HAL_CLK_DIV5P5,  // 6
    HAL_CLK_DIV5P0,  // 7
    HAL_CLK_DIV4P5,  // 8
    HAL_CLK_DIV4P0,  // 9
    HAL_CLK_DIV3P5,  // 10
    HAL_CLK_DIV3P0,  // 11
    HAL_CLK_DIV2P5,  // 12
    HAL_CLK_DIV2P0,  // 13
    HAL_CLK_DIV1P5,  // 14
    HAL_CLK_DIV1P0,  // 15
};

unsigned halHalfDividerOutFreq(unsigned input, unsigned divider)
{
    unsigned half_div = gHalfDividerDiv[divider & 0xf]; // [3:0]
    return (input * 2) / half_div;
}
