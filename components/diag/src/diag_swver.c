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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "diag_config.h"
#include "diag.h"
#include "osi_api.h"
#include "osi_log.h"
#include "drv_uart.h"
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#include "cmddef.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define _MOCOR_SW_VERSION_ "MOCOR_16A_UIS8915_M2M"
#define _MOCOR_SW_PROJECT_ "UIS8915A"
#define _MOCOR_PROJECT_VERSION_ "UIS8915A.BASE"
#define _MOCOR_BASE_VERSION_ "BASE_SVN"
#define _MOCOR_HW_VERSION_ "UIS8915"
#define PROJECT_BUILD_TIME "11-11-2018 11.11.11"

static bool _swver(const diagMsgHead_t *cmd, void *ctx)
{
    char *version = malloc(512);
    char *ver = version;

#ifdef _VERSION_RELEASE_
    ver += sprintf(ver, "\nPlatform Version: %s_Release", _MOCOR_SW_VERSION_);
#else
    ver += sprintf(ver, "\nPlatform Version: %s_Debug\n%s", _MOCOR_SW_VERSION_, _MOCOR_SW_PROJECT_);
#endif
    ver += sprintf(ver, "\nProject Version:   %s", _MOCOR_PROJECT_VERSION_);
    ver += sprintf(ver, "\nBASE  Version:     %s", _MOCOR_BASE_VERSION_);
    ver += sprintf(ver, "\nHW Version:        %s", _MOCOR_HW_VERSION_);
    ver += sprintf(ver, "\n%s", PROJECT_BUILD_TIME);

    size_t ver_str_len = strlen(version);
    diagOutputPacket2(cmd, version, ver_str_len);
    free(version);
    return true;
}

void diagSwverInit(void)
{
    diagRegisterCmdHandle(DIAG_SWVER_F, _swver, NULL);
}
