/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _DRV_BACKLIGHT_
#define _DRV_BACKLIGHT_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include <stdint.h>
#include <stdbool.h>

/**
 * \brief open backlight
 *
 * \param light_level from 0 to 255
 * \return
 *      - true on succeed else fail
 */
bool drvBackLightOpen(uint32_t light_level);

/**
 * \brief close backlight
 *
 * \return
 *      - true on succeed else fail
 */
bool drvBackLightClose();

OSI_EXTERN_C_END

#endif //_DRV_BACKLIGHT_
