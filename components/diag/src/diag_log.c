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

static bool _handleLog(const diagMsgHead_t *cmd, void *ctx)
{
    switch (cmd->subtype)
    {
    case LOG_GET_FILTER:
        // no log filter
        diagOutputPacket2(cmd, NULL, 0);
        break;

    case LOG_SET_FILTER:
        diagOutputPacket2(cmd, NULL, 0);
        break;

    case ARM_LOG_ENABLE:
    case ARM_LOG_DISABLE:
    case DSP_LOG_ENABLE:
    case DSP_LOG_DISABLE:
        diagOutputPacket2(cmd, NULL, 0);
        break;

    case LOG_SET_FLOW_LOG:
        diagOutputPacket2(cmd, NULL, 0);
        break;

    default:
        diagBadCommand(cmd);
        break;
    }
    return true;
}

void diagLogInit(void)
{
    diagRegisterCmdHandle(DIAG_LOG_F, _handleLog, NULL);
}
