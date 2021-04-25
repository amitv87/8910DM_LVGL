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

#ifndef _HAL_PAGE_SPY_H_
#define _HAL_PAGE_SPY_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_config.h"
#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#define HAL_PAGESPY_DETECT_READ (1 << 0)
#define HAL_PAGESPY_DETECT_WRITE (1 << 1)

/**
 * set page spy protect
 *
 * \p mode can be combination of \p HAL_PAGESPY_DETECT_READ and
 * \p HAL_PAGESPY_DETECT_WRITE.
 *
 * \param start     start address
 * \param size      size for protection
 * \param mode      detect mode
 * \return
 *      - page spy ID
 *      - -1 if all page spy resources are used
 */
int halPageSpyProtect(const void *start, size_t size, uint32_t mode);

/**
 * unset page spy protect
 *
 * \param id        page spy ID, should be returned by \p halPageSpyProtect
 * \return
 *      - true on success
 *      - false on invalid parameter
 */
bool halPageSpyUnprotect(int id);

OSI_EXTERN_C_END
#endif
