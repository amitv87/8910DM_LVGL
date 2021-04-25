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

#include "boot_pdl.h"
#include "boot_uimage.h"
#include "boot_platform.h"
#include "boot_secure.h"
#include "osi_api.h"
#include "osi_log.h"
#include <string.h>

#define PDL_MAX_DATA_SIZE (2 * 1024)
#define RESP_RETURN(pdl, resp) OSI_DO_WHILE0(pdlEngineSendResp(pdl, resp); return;)

typedef struct
{
    bool connected;
    osiElapsedTimer_t connect_timer;
    unsigned connect_timeout;
    unsigned call_param;
    unsigned region_start;
    unsigned region_size;
    unsigned file_start;
    unsigned file_size;
    unsigned recv_size;
} pdlUimageContext_t;

static bool prvPdlPolling(pdlEngine_t *pdl, void *param)
{
    pdlUimageContext_t *d = (pdlUimageContext_t *)param;
    if (!d->connected && d->connect_timeout != 0 &&
        osiElapsedTime(&d->connect_timer) > d->connect_timeout)
        return false;

    return true;
}

static void prvPdlProcess(pdlEngine_t *pdl, void *param, pdlCommand_t *cmd, unsigned size)
{
    pdlUimageContext_t *d = (pdlUimageContext_t *)param;

    OSI_LOGD(0, "PDL command %d size/%d", cmd->cmd_type, size);

    if (cmd->cmd_type == PDL_CMD_CONNECT)
    {
        d->connected = true;
        RESP_RETURN(pdl, PDL_RESP_ACK);
    }
    else if (cmd->cmd_type == PDL_CMD_START_DATA)
    {
        pdlDataCommand_t *dcmd = (pdlDataCommand_t *)cmd;
        if (!(dcmd->address >= d->region_start &&
              dcmd->address + dcmd->size <= d->region_start + d->region_size))
            RESP_RETURN(pdl, PDL_RESP_INVALID_ADDR);

        d->file_start = dcmd->address;
        d->file_size = dcmd->size;
        d->recv_size = 0;
        RESP_RETURN(pdl, PDL_RESP_ACK);
    }
    else if (cmd->cmd_type == PDL_CMD_MID_DATA)
    {
        pdlDataCommand_t *dcmd = (pdlDataCommand_t *)cmd;
        if (dcmd->size + sizeof(pdlDataCommand_t) > size)
            RESP_RETURN(pdl, PDL_RESP_INVALID_SIZE);

        if (d->recv_size + dcmd->size > d->file_size)
            RESP_RETURN(pdl, PDL_RESP_INVALID_SIZE);

        memcpy((void *)d->region_start + d->recv_size, dcmd->data, dcmd->size);
        d->recv_size += dcmd->size;
        RESP_RETURN(pdl, PDL_RESP_ACK);
    }
    else if (cmd->cmd_type == PDL_CMD_END_DATA)
    {
        // check data in EXEC
        RESP_RETURN(pdl, PDL_RESP_ACK);
    }
    else if (cmd->cmd_type == PDL_CMD_EXEC_DATA)
    {
        if (!bootUimageCheck((void *)d->file_start, d->file_size))
            RESP_RETURN(pdl, PDL_RESP_VERIFY_ERROR);

        if (!bootSecureUimageSigCheck((void *)d->file_start))
            RESP_RETURN(pdl, PDL_RESP_VERIFY_ERROR);

        // ROM doesn't send resp. Though it is bad design, we have to follow.
        bootJumpFunc_t entry = (bootJumpFunc_t)bootUimageEntry((void *)d->file_start);
        osiDCacheCleanInvalidateAll();
        bootJumpImageEntry(d->call_param, entry);
    }
    else
    {
        pdlEngineSendResp(pdl, PDL_RESP_UNKNOWN_CMD);
    }
}

bool pdlDnldUimage(fdlChannel_t *ch, unsigned callparam, unsigned start, unsigned size, unsigned timeout)
{
    pdlUimageContext_t ctx = {
        .connected = false,
        .connect_timeout = timeout,
        .call_param = callparam,
        .region_start = start,
        .region_size = size,
    };

    osiElapsedTimerStart(&ctx.connect_timer);
    pdlEngine_t *pdl = pdlEngineCreate(ch, PDL_MAX_DATA_SIZE + sizeof(pdlDataCommand_t));
    if (pdl == NULL)
        return false;

    if (!pdlEngineLoop(pdl, prvPdlProcess, prvPdlPolling, &ctx))
        osiPanic();

    return true;
}
