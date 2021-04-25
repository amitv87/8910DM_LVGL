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

#include "hal_access_map.h"
#include "osi_compiler.h"

typedef struct halMapModule
{
    const halMapVersion_t *version;
    const void *access;
} halMapModule_t;

static OSI_SECTION_SRAM_UC_BSS halMapModule_t gMapModules[HAL_MAP_ID_COUNT];

void halMapEngineInit(void)
{
    extern uint32_t __fixptr;
    *(uint32_t *)OSI_KSEG1(&__fixptr) = (uint32_t)gMapModules;
}

void halMapEngineRegister(halMapId_t id, const halMapVersion_t *version, const void *access)
{
    if (id >= HAL_MAP_ID_COUNT)
        return;

    gMapModules[id].version = version;
    gMapModules[id].access = access;
}
