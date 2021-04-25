/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "ats_config.h"
#include "at_cfw.h"
#include "at_command.h"
#include "at_response.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nvm.h"
#include "osi_log.h"

#ifndef CONFIG_QUEC_PROJECT_FEATURE

OSI_WEAK const char *AT_GMI_ID = GMI_ID;
OSI_WEAK const char *AT_GMM_ID = GMM_ID;
OSI_WEAK const char *AT_GMR_ID = GMR_ID;

void atCmdHandleCGMM(atCommand_t *cmd)
{
    switch (cmd->type)
    {
    case AT_CMD_EXE:
        atCmdRespInfoText(cmd->engine, AT_GMM_ID);
        atCmdRespOK(cmd->engine);
        break;

    case AT_CMD_TEST:
        atCmdRespOK(cmd->engine);
        break;

    default:
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}

void atCmdHandleGMM(atCommand_t *cmd)
{
    atCmdHandleCGMM(cmd);
}

void atCmdHandleCGMR(atCommand_t *cmd)
{
    switch (cmd->type)
    {
    case AT_CMD_EXE:
        atCmdRespInfoText(cmd->engine, AT_GMR_ID);
        atCmdRespOK(cmd->engine);
        break;

    case AT_CMD_TEST:
        atCmdRespOK(cmd->engine);
        break;

    default:
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}

void atCmdHandleGMR(atCommand_t *cmd)
{
    switch (cmd->type)
    {
    case AT_CMD_EXE:
        atCmdRespInfoText(cmd->engine, AT_GMR_ID);
        atCmdRespOK(cmd->engine);
        break;

    case AT_CMD_TEST:
        atCmdRespOK(cmd->engine);
        break;

    default:
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}

void atCmdHandleCGMI(atCommand_t *cmd)
{
    switch (cmd->type)
    {
    case AT_CMD_EXE:
        atCmdRespInfoText(cmd->engine, AT_GMI_ID);
        atCmdRespOK(cmd->engine);
        break;

    case AT_CMD_TEST:
        atCmdRespOK(cmd->engine);
        break;

    default:
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}

void atCmdHandleGMI(atCommand_t *cmd)
{
    atCmdHandleCGMI(cmd);
}

void atCmdHandleCGSN(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (mode == 0)
        {
            uint8_t sn[25] = {
                0,
            };
            int len = nvmReadSN(0, &sn, 24);
            if (len == -1)
                AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL));
            atCmdRespInfoNText(cmd->engine, (char *)sn, len);
        }
        else // 1
        {
            uint8_t nImei[16] = {
                0,
            };
            int nImeiLen = nvmReadImei(nSim, (nvmImei_t *)&nImei[0]);
            if (nImeiLen == -1)
                AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL));

            atCmdRespInfoNText(cmd->engine, (char *)nImei, nImeiLen);
        }
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char outStr[50] = {0};
        sprintf(outStr, "%s: (0,1)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, outStr);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_EXE)
    {
        uint8_t nImei[16] = {0};
        int nImeiLen = nvmReadImei(nSim, (nvmImei_t *)&nImei[0]);
        if (nImeiLen == -1)
            AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL));
        char rsp[256] = {0};
        sprintf(rsp, "%s:\"%s\"", cmd->desc->name, (char *)nImei);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void atCmdHandleGSN(atCommand_t *cmd)
{
    atCmdHandleCGSN(cmd);
}

void atCmdHandleCGBV(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        if (atParamIsEmpty(cmd->params[0]))
        {
            uint8_t *basebandVersion = CFW_EmodGetBaseBandVersionV1();
            if (basebandVersion == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            char rsp[64];
            sprintf(rsp, "%s: %s", cmd->desc->name, basebandVersion);
            atCmdRespInfoText(cmd->engine, rsp);
            RETURN_OK(cmd->engine);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
        RETURN_OK(cmd->engine);
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}

void atCmdHandleI(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        if (atParamIsEmpty(cmd->params[0]))
        {
            atCmdRespInfoText(cmd->engine, AT_GMI_ID);
            atCmdRespInfoText(cmd->engine, AT_GMM_ID);
            atCmdRespInfoText(cmd->engine, AT_GMR_ID);
            RETURN_OK(cmd->engine);
        }
    }
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}
#endif
static void _cimiRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    // EV_CFW_SIM_GET_PROVIDER_ID_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nType == 0)
    {
        atCmdRespInfoNText(cmd->engine, (char *)cfw_event->nParam1, cfw_event->nParam2);
        atCmdRespOK(cmd->engine);
        free((void *)cfw_event->nParam1);
    }
    else if (cfw_event->nType == 0xF0)
    {
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}

void atCmdHandleCIMI(atCommand_t *cmd)
{
    uint32_t res;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_EXE)
    {
        CFW_SIM_STATUS status = CFW_GetSimStatus(nSim);
        OSI_LOGI(0, "CIMI get sim status: %d", status);
        if (status == CFW_SIM_ABSENT)
        {
            RETURN_CME_CFW_ERR(cmd->engine, ERR_AT_CME_SIM_NOT_INSERTED);
        }
        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cimiRsp, cmd);
        if ((res = CFW_SimGetProviderId(cmd->uti, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_CFW_ERR(cmd->engine, res);
        }
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

extern uint16_t hex2ascii(uint8_t *pInput, uint16_t nInputLen, char *pOutput);
static void _eidRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    OSI_LOGI(0, "receive EV_CFW_SIM_GET_EID_RSP , cfw_event->nType = %d", cfw_event->nType);
    if (cfw_event->nType == 0)
    {
        char eid[21] = {0};
        uint16_t length = hex2ascii((uint8_t *)cfw_event->nParam1, cfw_event->nParam2, eid);
        if (length == 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        atCmdRespInfoNText(cmd->engine, eid, length);
        atCmdRespOK(cmd->engine);
        free((void *)cfw_event->nParam1);
    }
    else if (cfw_event->nType == 0xF0)
    {
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}
//CFW_SimGetEID
void atCmdHandleEID(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t res;

    if (cmd->type == AT_CMD_EXE)
    {
        uint16_t uti = cfwRequestUTI((osiEventCallback_t)_eidRsp, cmd);
        if ((res = CFW_SimGetEID(uti, nSim)) != 0)
        {
            cfwReleaseUTI(uti);
            RETURN_CME_CFW_ERR(cmd->engine, res);
        }
        OSI_LOGI(0, "call CFW_SimGetEID OK");
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
