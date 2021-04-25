/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('T', 'R', 'N', 'G')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "hwregs.h"
#include <stdint.h>
#include <stdbool.h>
#include "osi_api.h"
#include "osi_log.h"

void halAesTrngInit()
{
    REG_AES_DMA_FUNC_CTRL_T dma_func_ctrl = {.b.trng_cgen = 1};
    hwp_aes->dma_func_ctrl = dma_func_ctrl.v;
}

void halAesTrngReset()
{
    REG_AES_TRNG_CTRL_T trng_ctrl = {hwp_aes->trng_ctrl};
    trng_ctrl.b.trng_ctrl = 0;
    hwp_aes->dma_int_out = 0xff;
    hwp_aes->trng_ctrl = trng_ctrl.v;
}

void halAesTrngEnable()
{
    REG_AES_TRNG_CTRL_T trng_ctrl = {
        .b.trng_mask = 0xff,
        .b.trng_ctrl = 0x1f,
    };
    hwp_aes->dma_int_out = 0xff;
    hwp_aes->trng_ctrl = trng_ctrl.v;
}

void halAesTrngGetValue(uint32_t *v0, uint32_t *v1)
{
    if (v0)
        *v0 = hwp_aes->trng_data0;
    if (v1)
        *v1 = hwp_aes->trng_data1;
}

bool halAesCheckTrngComplete()
{
    return (hwp_aes->dma_int_out & 0x10);
}
