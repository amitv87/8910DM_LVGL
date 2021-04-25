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

#ifndef _DRV_RNG_H_
#define _DRV_RNG_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include <stdbool.h>

/**
 * \brief generate rng data
 * \param buf   buffer to store random data
 * \param len   buffer length
 * \return
 *      - true on success else false
 */
bool drvRngGenerate(void *buf, unsigned len);

OSI_EXTERN_C_END

#endif