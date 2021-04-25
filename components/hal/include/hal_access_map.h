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

#ifndef _HAL_ACCESS_MAP_H_
#define _HAL_ACCESS_MAP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    HAL_MAP_ID_00,
    HAL_MAP_ID_PAL,
    HAL_MAP_ID_SPAL,
    HAL_MAP_ID_SPP,
    HAL_MAP_ID_SPC,
    HAL_MAP_ID_05,
    HAL_MAP_ID_06,
    HAL_MAP_ID_07,
    HAL_MAP_ID_MEMD,
    HAL_MAP_ID_09,
    HAL_MAP_ID_STACK,
    HAL_MAP_ID_0B,
    HAL_MAP_ID_0C,
    HAL_MAP_ID_0D,
    HAL_MAP_ID_0E,
    HAL_MAP_ID_0F,
    HAL_MAP_ID_10,
    HAL_MAP_ID_11,
    HAL_MAP_ID_CALIB,
    HAL_MAP_ID_13,
    HAL_MAP_ID_FLASH_PROG,
    HAL_MAP_ID_15,
    HAL_MAP_ID_16,
    HAL_MAP_ID_17,
    HAL_MAP_ID_18,
    HAL_MAP_ID_19,
    HAL_MAP_ID_NB_CALIB,
    HAL_MAP_ID_COUNT
} halMapId_t;

typedef struct
{
    uint32_t revision;
    uint32_t number;
    uint32_t date;
    char *string;
} halMapVersion_t;

void halMapEngineInit(void);
void halMapEngineRegister(halMapId_t id, const halMapVersion_t *version, const void *access);

#ifdef __cplusplus
}
#endif
#endif
