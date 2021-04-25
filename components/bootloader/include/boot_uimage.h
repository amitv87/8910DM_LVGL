/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _BOOT_UIMAGE_H_
#define _BOOT_UIMAGE_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief get uimage size
 *
 * \p buf should be large enough for uimage header. The return size
 * is the total size including uimage header.
 *
 * \param buf       buffer for uimage, 4 bytes aligned
 * \return  uimage size
 */
unsigned bootUimageSize(const void *buf);

/**
 * \brief check whether uimage is valid
 *
 * \p len is the buffer size. It shoun't be less than the uimage size
 * including uimage header.
 *
 * \param buf       buffer for uimage, 4 bytes aligned
 * \param len       buffer size
 * \return
 *      - true if the uimage is valid
 *      - false if the uimage is invalid
 */
bool bootUimageCheck(const void *buf, unsigned len);

/**
 * \brief quick check whether uimage is valid
 *
 * It is similar to \p bootUimageCheck, and the data CRC isn't checked.
 * It is provided in case that a quick and not-so-reliable check is
 * needed.
 *
 * \param buf       buffer for uimage, 4 bytes aligned
 * \param len       buffer size
 * \return
 *      - true if the uimage is valid
 *      - false if the uimage is invalid
 */
bool bootUimageQuickCheck(const void *buf, unsigned len);

/**
 * \brief get uimage entry address
 *
 * \p buf should be checked by \p bootUimageCheck before.
 *
 * \param buf       buffer for uimage, must be checked
 * \return  uimage entry point
 */
uint32_t bootUimageEntry(const void *buf);

OSI_EXTERN_C_END
#endif
