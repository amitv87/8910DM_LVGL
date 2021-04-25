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

#ifndef _BOOT_VFS_H_
#define _BOOT_VFS_H_

#include "osi_compiler.h"
#include "sffs.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

OSI_EXTERN_C_BEGIN

// Bootloader will supported a simplified version of VFS.
//
// The reason not to reuse appication vfs is bootloader is more sensitive
// to code size.
//
// * Only SFFS is supported
// * Not all vfs APIs are implemented
// * Restrictions for path:
//   * Must started with '/'
//   * No "." and ".."
//   * "//" is not supported

bool bootVfsMount(const char *path, sffsFs_t *fs);

OSI_EXTERN_C_END
#endif
