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

#ifndef _BOOT_SIMAGE_H_
#define _BOOT_SIMAGE_H_

#include "calclib/simage_types.h"

/**
 * \brief get simage security version
 *
 * \param header    pointer of simage header
 * \return          security version of the simage
 */
uint32_t bootSimageSecurityVersion(const simageHeader_t *header);

#endif