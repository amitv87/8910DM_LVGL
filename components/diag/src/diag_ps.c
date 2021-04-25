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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "diag_config.h"
#include "diag.h"
#include "osi_log.h"
#include "osi_api.h"
#include "cmddef.h"
#include "drv_secure.h"
#include <stdlib.h>
#include <string.h>
#include "srv_simlock.h"

static void _handleGetSimlockDataHash(const diagMsgHead_t *cmd)
{
    uint32_t length;
    diagMsgHead_t head = *cmd;
    uint8_t hash[32] = {0};

    bool status = srvSimlockDdataSha(hash);
    if (status)
    {
        length = sizeof(hash);
        head.len = length + sizeof(diagMsgHead_t);
        diagOutputPacket2(&head, hash, length);
    }
    else
    {
        length = 1;
        head.len = length + sizeof(diagMsgHead_t);
        diagOutputPacket2(&head, 0x00, length);
        OSI_LOGD(0, "SIMLOCK: Read data hash fail...");
    }
}
static void _handleWriteSimlockDataSign(const diagMsgHead_t *cmd)
{
    uint8_t ret;
    diagMsgHead_t head = *cmd;
    const void *data = diagCmdData(cmd);
    unsigned size = diagCmdDataSize(cmd);
    bool status = srvSimlockSignData(data, size);

    if (status)
        ret = 1;
    else
        ret = 0;

    head.len = 1 + sizeof(diagMsgHead_t);
    diagOutputPacket2(&head, &ret, 1);
}
static bool _handlePs(const diagMsgHead_t *cmd, void *ctx)
{
    switch (cmd->subtype)
    {
    case GET_SIMLOCK_DATA_HASH_F:
        _handleGetSimlockDataHash(cmd);
        break;

    case WRITE_SIMLOCK_SIGN_F:
        _handleWriteSimlockDataSign(cmd);
        break;

    default:
        diagOutputPacket2(cmd, NULL, 0);
        break;
    }
    return true;
}

void diagPsInit(void)
{
    diagRegisterCmdHandle(DIAG_PS_F, _handlePs, NULL);
}
