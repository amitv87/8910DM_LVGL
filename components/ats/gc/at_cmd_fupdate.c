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

#define OSI_LOG_TAG LOG_TAG_FUPDATE
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "atr_config.h"
#include "ats_config.h"
#include "at_command.h"
#include "at_engine.h"
#include "at_response.h"
#include "osi_log.h"
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

#ifdef CONFIG_ATS_UPDATE_SUPPORT
#include "fupdate.h"
#include "vfs.h"

typedef struct updateAsyncContext
{
    unsigned size;
    unsigned pos;
    char data[0];
} updateAsyncContext_t;

static void _update(atCommand_t *cmd, const void *data, unsigned size)
{
    vfs_mkdir(CONFIG_FS_FOTA_DATA_DIR, 0);
    int wsize = vfs_file_write(FUPDATE_PACK_FILE_NAME, data, size);
    if (wsize != size)
    {
        OSI_LOGE(0, "Fota Error : write file fail, errno %d", errno);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }

    // To check current version, pass the current version string
    // as parameter.
    if (!fupdateSetReady(NULL))
    {
        OSI_LOGE(0, "Fota Error: not ready");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }

    atCmdRespOK(cmd->engine);
    osiShutdown(OSI_SHUTDOWN_RESET);
}

static int _updateCB(void *ctx, const void *data, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)ctx;
    updateAsyncContext_t *async = (updateAsyncContext_t *)cmd->async_ctx;

    int needed = OSI_MIN(int, size, async->size - async->pos);
    if (needed <= 0)
    {
        OSI_LOGE(0, "Fota Error : needed %d", needed);
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
        return 0;
    }

    OSI_LOGD(0, "update size/%d pos/%d size/%d needed/%d",
             async->size, async->pos, size, needed);

    memcpy(async->data + async->pos, data, needed);
    async->pos += needed;
    if (async->pos >= async->size)
        _update(cmd, async->data, async->size);

    return needed;
}

void atCmdHandleUPDATE(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +UPDATE=<n>
        bool paramok = true;
        unsigned size = atParamUint(cmd->params[0], &paramok);
        if (!paramok || size == 0 || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        updateAsyncContext_t *async = malloc(sizeof(updateAsyncContext_t) + size);
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        async->size = size;
        async->pos = 0;
        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;

        fupdateInvalidate(true);
        atCmdRespOutputPrompt(cmd->engine);
        atCmdSetBypassMode(cmd->engine, _updateCB, cmd);
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_EXE)
    {
        // To check current version, pass the current version string
        // as parameter.
        if (!fupdateSetReady(NULL))
        {
            OSI_LOGE(0, "Fota Error: not ready");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        atCmdRespOK(cmd->engine);
        osiShutdown(OSI_SHUTDOWN_RESET);
    }
    else
    {
        //+UPDATE,+UPDATE?,UPDATE=? is not support
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
#endif
