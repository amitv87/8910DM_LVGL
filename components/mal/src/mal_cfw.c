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

#include "mal_api.h"
#include "mal_internal.h"
#include "at_response.h"
#include "cfw.h"
#include "cfw_event.h"
#include "cfw_errorcode.h"
#include "osi_log.h"
#include <string.h>
#include <stdlib.h>

extern bool gSimDropInd[CONFIG_NUMBER_OF_SIM];

typedef struct
{
    unsigned errcode;
    unsigned state;
    unsigned remainRetries;
} ateContextReadCPIN_t;

static void prvCpinReadRspCB(malTransaction_t *trans, const osiEvent_t *event)
{
    // EV_CFW_SIM_GET_AUTH_STATUS_RSP
    ateContextReadCPIN_t *ctx = (ateContextReadCPIN_t *)malTransContext(trans);
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    int auth = -1;
    if (cfw_event->nType == 0)
        auth = cfw_event->nParam1;
    if ((cfw_event->nType == 0xf0) /*&& (gAtCfwCtx.init_status == AT_MODULE_INIT_NO_SIM)*/)
        auth = CFW_STY_AUTH_NOSIM;
    if (auth == CFW_STY_AUTH_PIN1_DISABLE || auth == CFW_STY_AUTH_PIN1_READY)
        auth = CFW_STY_AUTH_READY;

    if (auth >= 0)
    {
        ctx->errcode = 0;
        ctx->state = auth;
        ctx->remainRetries = cfw_event->nParam2;
    }
    else
    {
        ctx->errcode = atCfwToCmeError(cfw_event->nParam1);
    }

    malTransFinished(trans);
}

unsigned malSimGetState(int sim, malSimState_t *state, unsigned *remaintries)
{
    if (state == NULL)
        return ERR_AT_CME_PARAM_INVALID;

    if (remaintries == NULL)
        return ERR_AT_CME_PARAM_INVALID;

    if (gSimDropInd[sim])
        return ERR_AT_CME_SIM_NOT_INSERTED;

    ateContextReadCPIN_t ctx = {};
    malTransaction_t *trans = malCreateTrans();
    if (trans == NULL)
        return ERR_AT_CME_NO_MEMORY;

    malSetTransContext(trans, &ctx, NULL);
    malStartUtiTrans(trans, prvCpinReadRspCB);

    unsigned result = CFW_SimGetAuthenticationStatus(malTransUti(trans), sim);
    if (result != 0)
    {
        malAbortTrans(trans);

        if (ERR_CFW_INVALID_PARAMETER == result)
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_PARAM_INVALID);
        else if (ERR_CME_OPERATION_NOT_ALLOWED == result)
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        else if (ERR_NO_MORE_MEMORY == result)
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_NO_MEMORY);
        else if ((ERR_CME_SIM_NOT_INSERTED == result) ||
                 (ERR_CFW_SIM_NOT_INITIATE == result))
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_SIM_NOT_INSERTED);
        else
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_EXE_NOT_SURPORT);
    }

    if (!malTransWait(trans, CONFIG_MAL_GET_SIM_AUTH_TIMEOUT))
    {
        malAbortTrans(trans);
        MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_SEND_TIMEOUT);
    }

    if (ctx.errcode == 0)
    {
        *state = ctx.state;
        *remaintries = ctx.remainRetries;
    }
    MAL_TRANS_RETURN_ERR(trans, ctx.errcode);
}

#define SIM_CHANNEL_MAX_NUM 7
#define CFW_SIM_NUMBER CONFIG_NUMBER_OF_SIM

#define SIM_CHANNEL_ACCESS_DEFAULT_TIMEOUT 60
#define SIM_CHANNEL_ACCESS_TIMEOUT_ERROR (-1)
uint16_t ascii2hex(const char *pInput, uint16_t length, uint8_t *pOutput);

typedef struct
{
    uint8_t df[20];
    uint16_t length;
    unsigned errcode;
    uint8_t channel_id;
} openChannel_t;
static void openChannelRspCB(malTransaction_t *trans, const osiEvent_t *event)
{
    openChannel_t *ctx = (openChannel_t *)malTransContext(trans);
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t sim = cfw_event->nFlag;
    if (cfw_event->nEventId == EV_CFW_SIM_MANAGE_CHANNEL_RSP)
    {
        if ((cfw_event->nType == 0) && (cfw_event->nParam2 == 1))
        {
            if (ctx->channel_id == 0) //open channel
            {
                ctx->channel_id = cfw_event->nParam1;
                OSI_LOGI(0, "ctx->length = %d, ctx->channel_id = %d", ctx->length, ctx->channel_id);
                OSI_LOGXI(OSI_LOGPAR_S, 0, "dfname = %s", ctx->df);
                for (uint8_t i = 0; i < ctx->length; i++)
                    OSI_LOGI(0, "%X", ctx->df[i]);
                malStartUtiTrans(trans, openChannelRspCB);
                uint32_t result = CFW_SimSelectApplication(ctx->df, ctx->length, ctx->channel_id, malTransUti(trans), sim);
                if (result != 0)
                {
                    malStartUtiTrans(trans, openChannelRspCB);
                    uint32_t result = CFW_SimManageChannel(CLOSE_CHANNEL_CMD, ctx->channel_id, malTransUti(trans), sim);
                    if (result != 0)
                    {
                        ctx->errcode = -1;
                        malAbortTrans(trans);
                    }
                }
            }
            else //close channel
            {
                ctx->errcode = -1;
                malAbortTrans(trans);
            }
        }
        else
        {
            ctx->errcode = -1;
            malAbortTrans(trans);
        }
        return;
    }
    else if (cfw_event->nEventId == EV_CFW_SIM_SELECT_APPLICATION_RSP)
    {
        OSI_LOGI(0, "cfw_event->nType = %d, cfw_event->nParam2 = %d", cfw_event->nType, cfw_event->nParam2);
        if ((cfw_event->nType == 0) && (cfw_event->nParam2 == 1))
        {
            ctx->errcode = 0;
            malTransFinished(trans);
        }
        else
        {
            malStartUtiTrans(trans, openChannelRspCB);
            uint32_t result = CFW_SimManageChannel(CLOSE_CHANNEL_CMD, ctx->channel_id, malTransUti(trans), sim);
            if (result != 0)
            {
                malAbortTrans(trans);
                return;
            }
        }
        return;
    }
    return;
}

int sim_channel_open(uint8_t *dfname, uint8_t *channel_id, uint16_t timeout, uint8_t sim_id)
{
    if (dfname == NULL)
        return -1;
    uint8_t length = strlen((const char *)dfname);
    if ((length < 10) || (length > 32))
        return -1;

    if (channel_id == NULL)
        return -1;

    if (sim_id > CFW_SIM_NUMBER - 1)
        return -1;

    openChannel_t ctx = {};
    malTransaction_t *trans = malCreateTrans();
    if (trans == NULL)
    {
        OSI_LOGI(0, "malCreateTrans failed!");
        return -1;
    }
    ctx.length = ascii2hex((const char *)dfname, strlen((const char *)dfname), ctx.df);
    if (ctx.length == 0)
    {
        free(trans);
        OSI_LOGI(0, "dfname is illegal!");
        return -1;
    }

    malSetTransContext(trans, &ctx, NULL);
    malStartUtiTrans(trans, openChannelRspCB);

    uint32_t result = CFW_SimManageChannel(OPEN_CHANNEL_CMD, 0, malTransUti(trans), sim_id);
    if (result != 0)
    {
        malAbortTrans(trans);
        OSI_LOGI(0, "malCreateTrans failed!");
        return -1;
    }

    if (!malTransWait(trans, timeout))
    {
        malAbortTrans(trans);
        MAL_TRANS_RETURN_ERR(trans, SIM_CHANNEL_ACCESS_TIMEOUT_ERROR);
    }
    OSI_LOGI(0, "ctx.errcode == %d, ctx.channel_id = %d", ctx.errcode, ctx.channel_id);

    if (ctx.errcode == 0)
    {
        *channel_id = ctx.channel_id;
    }
    MAL_TRANS_RETURN_ERR(trans, ctx.errcode);
}

typedef struct
{
    unsigned errcode;
    unsigned channel_id;
} clolseChannel_t;
static void closeChannelRspCB(malTransaction_t *trans, const osiEvent_t *event)
{
    //EV_CFW_SIM_MANAGE_CHANNEL_RSP
    clolseChannel_t *ctx = (clolseChannel_t *)malTransContext(trans);
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if ((cfw_event->nType == 0) && (cfw_event->nParam2 == 1))
    {
        ctx->channel_id = cfw_event->nParam1;
        ctx->errcode = 0;
    }
    else
        ctx->errcode = -1;
    malTransFinished(trans);
}

int sim_channel_close(uint8_t channel_id, uint16_t timeout, uint8_t sim_id)
{
    if (channel_id > SIM_CHANNEL_MAX_NUM)
        return -1;

    if (sim_id > CFW_SIM_NUMBER - 1)
        return -1;

    clolseChannel_t ctx = {};
    malTransaction_t *trans = malCreateTrans();
    if (trans == NULL)
        return -1;
    ctx.channel_id = channel_id;
    ctx.errcode = 0;

    malSetTransContext(trans, &ctx, NULL);
    malStartUtiTrans(trans, closeChannelRspCB);

    uint32_t result = CFW_SimManageChannel(CLOSE_CHANNEL_CMD, channel_id, malTransUti(trans), sim_id);
    if (result != 0)
    {
        malAbortTrans(trans);
        OSI_LOGI(0, "CFW_SimManageChannel failed!");
        return -1;
    }

    if (!malTransWait(trans, timeout))
    {
        malAbortTrans(trans);
        MAL_TRANS_RETURN_ERR(trans, SIM_CHANNEL_ACCESS_TIMEOUT_ERROR);
    }

    MAL_TRANS_RETURN_ERR(trans, ctx.errcode);
}

typedef struct
{
    unsigned errcode;
    uint8_t *resp;
    uint16_t respLen;
} transmitChannel_t;
static void transmitChannelRspCB(malTransaction_t *trans, const osiEvent_t *event)
{
    //EV_CFW_SIM_TPDU_COMMAND_RSP
    transmitChannel_t *ctx = (transmitChannel_t *)malTransContext(trans);
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    OSI_LOGI(0, "cfw_event->nEventId = %d", cfw_event->nEventId);
    OSI_LOGI(0, "cfw_event->nParam1 = %X", cfw_event->nParam1);
    OSI_LOGI(0, "cfw_event->nParam2 = %x", cfw_event->nParam2);
    OSI_LOGI(0, "cfw_event->nType = %d", cfw_event->nType);

    if (cfw_event->nType == 0)
    {
        uint16_t sw = 0;
        uint16_t length = cfw_event->nParam2 & 0xFFFF;
        OSI_LOGI(0, "length = %d, ctx->respLen = %d", length, ctx->respLen);
        if (ctx->respLen < length + 2) //plus two bytes sw
        {
            OSI_LOGI(0, "ctx->respLen = %d(%d)", ctx->respLen, length + 2);
            ctx->errcode = -1;
            malAbortTrans(trans);
            return;
        }
        if (cfw_event->nParam2 == 0)
        {
            sw = cfw_event->nParam1;
            OSI_LOGI(0, "sw = %X", sw);
        }
        else
        {
            sw = cfw_event->nParam2 >> 16;
            OSI_LOGI(0, "sw = %X(%d)", sw, length);
            OSI_LOGI(0, "ctx->resp = %X(%X)", ctx->resp, (uint8_t *)cfw_event->nParam1);
            memcpy(ctx->resp, (uint8_t *)cfw_event->nParam1, length);
            free((uint8_t *)cfw_event->nParam1);
        }

        ctx->respLen = length + 2;
        ctx->resp[length] = sw >> 8;
        ctx->resp[length + 1] = sw & 0xFF;
        OSI_LOGI(0, "ctx->respLen = %d(sw = 0x%X,%X)", ctx->respLen, ctx->resp[length], ctx->resp[length + 1]);
        ctx->errcode = 0;
        malTransFinished(trans);
        return;
    }
    else
    {
        ctx->errcode = -1;
        malAbortTrans(trans);
        return;
    }
}

int sim_channel_transmit(uint8_t channel_id, uint8_t *apdu, uint16_t apduLen,
                         uint8_t *resp, uint16_t *respLen, uint16_t timeout, uint8_t sim_id)
{
    if (channel_id > SIM_CHANNEL_MAX_NUM)
        return -1;

    if (sim_id > CFW_SIM_NUMBER - 1)
        return -1;
#if 0
    uint16_t length = (apduLen + 1) >> 1;
    OSI_LOGI(0, "apdu length = %d(%d)", length, apduLen);
    
    uint8_t* cmd = malloc(length);
    if(cmd == NULL)
        return -1;
    length = ascii2hex(apdu, apduLen, cmd);
    if(length == 0)
        return -1;
    OSI_LOGI(0, "apdu hex length = %d", length);
#else
    uint8_t *cmd = apdu;
    uint16_t length = apduLen;
#endif
    transmitChannel_t ctx = {};
    ctx.resp = resp;
    ctx.respLen = *respLen;

    malTransaction_t *trans = malCreateTrans();
    if (trans == NULL)
        return -1;

    malSetTransContext(trans, &ctx, NULL);
    malStartUtiTrans(trans, transmitChannelRspCB);

    uint32_t result = CFW_SimTPDUCommand(cmd, length, channel_id, malTransUti(trans), sim_id);
    if (result != 0)
    {
        malAbortTrans(trans);
        OSI_LOGI(0, "CFW_SimManageChannel failed!");
        return -1;
    }

    if (!malTransWait(trans, timeout))
    {
        malAbortTrans(trans);
        MAL_TRANS_RETURN_ERR(trans, SIM_CHANNEL_ACCESS_TIMEOUT_ERROR);
    }
    *respLen = ctx.respLen;
    MAL_TRANS_RETURN_ERR(trans, ctx.errcode);
}

typedef struct
{
    uint16_t operatorCount;
    unsigned errcode;
    CFW_NW_OPERATOR_INFO_V2 *operatorInfo_v2;
} malNwVsimEOCtx_t;

static void prvSimVsimGetEORspCB(malTransaction_t *trans, const osiEvent_t *event)
{
    malNwVsimEOCtx_t *ctx = (malNwVsimEOCtx_t *)malTransContext(trans);
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nEventId == EV_CFW_NW_ABORT_LIST_OPERATORS_RSP)
        ctx->errcode = atCfwToCmeError(ERR_CME_NETWORK_TIMEOUT);

    if (cfw_event->nType == 0)
    {
        ctx->operatorCount = (uint16_t)cfw_event->nParam2;
        size_t nOperatorInfoSize_V2 = sizeof(CFW_NW_OPERATOR_INFO_V2) * ctx->operatorCount;
        ctx->operatorInfo_v2 = (CFW_NW_OPERATOR_INFO_V2 *)malloc(nOperatorInfoSize_V2);
        memcpy(ctx->operatorInfo_v2, (CFW_NW_OPERATOR_INFO_V2 *)cfw_event->nParam1, nOperatorInfoSize_V2);
        ctx->errcode = 0;
    }
    else
    {
        if (cfw_event->nParam1 == ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED)
            ctx->errcode = atCfwToCmeError(ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED);
        ctx->errcode = atCfwToCmeError(ERR_CME_OPERATION_NOT_ALLOWED);
    }
    atMemFreeLater((CFW_NW_OPERATOR_INFO_V2 *)cfw_event->nParam1);
    malTransFinished(trans);
}

unsigned simVsimGetEO(CFW_NW_OPERATOR_INFO_V2 **operatorInfo_v2, uint16_t *operatorCount, uint8_t nSim)
{
    malNwVsimEOCtx_t ctx = {};
    malTransaction_t *trans = malCreateTrans();
    if (trans == NULL)
        return ERR_AT_CME_NO_MEMORY;

    malSetTransContext(trans, &ctx, NULL);
    malStartUtiTrans(trans, prvSimVsimGetEORspCB);
    unsigned result = CFW_NwGetAvailableOperators_V2(malTransUti(trans), nSim);
    if (result != 0)
    {
        malAbortTrans(trans);

        if (ERR_CFW_INVALID_PARAMETER == result)
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_PARAM_INVALID);
        else if (ERR_CME_OPERATION_NOT_ALLOWED == result)
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        else if (ERR_NO_MORE_MEMORY == result)
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_NO_MEMORY);
        else if ((ERR_CME_SIM_NOT_INSERTED == result) ||
                 (ERR_CFW_SIM_NOT_INITIATE == result))
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_SIM_NOT_INSERTED);
        else
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_EXE_NOT_SURPORT);
    }

    if (!malTransWait(trans, CONFIG_MAL_GET_NW_OPERATOR_TIMEOUT))
        MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_SEND_TIMEOUT);
    if (ctx.errcode == 0)
    {
        size_t nOperatorInfoSize_V2 = sizeof(CFW_NW_OPERATOR_INFO_V2) * ctx.operatorCount;
        *operatorInfo_v2 = (CFW_NW_OPERATOR_INFO_V2 *)malloc(nOperatorInfoSize_V2);
        memcpy(*operatorInfo_v2, ctx.operatorInfo_v2, nOperatorInfoSize_V2);
        *operatorCount = ctx.operatorCount;
        free(ctx.operatorInfo_v2);
    }
    MAL_TRANS_RETURN_ERR(trans, ctx.errcode);
}

typedef struct
{
    unsigned errcode;
} malVsimResetCtx_t;

static void prvSimVsimResetRspCB(malTransaction_t *trans, const osiEvent_t *event)
{
    malVsimResetCtx_t *ctx = (malVsimResetCtx_t *)malTransContext(trans);
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType == 0)
        ctx->errcode = 0;
    else
        ctx->errcode = atCfwToCmeError(ERR_CME_OPERATION_NOT_ALLOWED);
    malTransFinished(trans);
}

unsigned simVsimReset(uint8_t nSim)
{
    malVsimResetCtx_t ctx = {};
    malTransaction_t *trans = malCreateTrans();
    if (trans == NULL)
        return ERR_AT_CME_NO_MEMORY;

    malSetTransContext(trans, &ctx, NULL);
    malStartUtiTrans(trans, prvSimVsimResetRspCB);
    unsigned result = CFW_SimReset(malTransUti(trans), nSim);
    if (result != 0)
    {
        malAbortTrans(trans);

        if (ERR_CFW_INVALID_PARAMETER == result)
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_PARAM_INVALID);
        else if (ERR_CME_OPERATION_NOT_ALLOWED == result)
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        else if (ERR_NO_MORE_MEMORY == result)
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_NO_MEMORY);
        else if ((ERR_CME_SIM_NOT_INSERTED == result) ||
                 (ERR_CFW_SIM_NOT_INITIATE == result))
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_SIM_NOT_INSERTED);
        else
            MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_EXE_NOT_SURPORT);
    }

    if (!malTransWait(trans, CONFIG_MAL_GET_SIM_AUTH_TIMEOUT))
    {
        malAbortTrans(trans);
        MAL_TRANS_RETURN_ERR(trans, ERR_AT_CME_SEND_TIMEOUT);
    }

    MAL_TRANS_RETURN_ERR(trans, ctx.errcode);
}