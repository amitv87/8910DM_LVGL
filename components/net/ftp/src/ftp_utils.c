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

#include "ftp_config.h"

#ifdef CONFIG_FTP_MODULE_SUPPORT

#include <stdio.h>
#include <stdarg.h>

#include "osi_api.h"
#include "osi_log.h"
#include "ftp_utils.h"

static const char *const ftp_log_names[] = {
    "CON",
    "LIB",
    "A T"};

void FTPLOGI(ftp_log_type_t t, const char *format, ...)
{
    char msg[512] = {0};
    uint32_t msglen = 0;
    msglen = snprintf(msg, sizeof(msg) - 1, "FTP^[%s][task:%p]: ", ftp_log_names[t], osiThreadCurrent());

    va_list args;
    va_start(args, format);
    vsnprintf(msg + msglen, sizeof(msg) - msglen - 1, format, args);
    va_end(args);

    OSI_LOGI(0, msg);
}
#endif
