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

#include "osi_log.h"
#include "ats_config.h"
#include "at_cfw.h"
#include "at_command.h"
#include "at_response.h"
#include "cfw_chset.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern void hal_WriteLteCalib(void);
extern void hal_WriteGsmCalib(void);

void atCmdHandleNVPC(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t dataType = atParamUint(cmd->params[0], &paramok);
        uint16_t operType = atParamUint(cmd->params[1], &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (operType == 0)
        {
            uint16_t offset = atParamUint(cmd->params[2], &paramok);
            uint16_t dataLength = atParamUint(cmd->params[3], &paramok);
            const char *value = atParamDefStr(cmd->params[4], "", &paramok);
            (void)value; // not used

            if (!paramok || cmd->param_count != 4)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            uint8_t *p_nv = CFW_EmodGetNvData(dataType, nSim);
            if (p_nv == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            // get nv param from psram variables
            char *rsp = (char *)malloc(dataLength * 2 + 16);
            char *prsp = rsp;

            prsp += sprintf(prsp, "%s: \"", cmd->desc->name);
            for (unsigned n = 0; n < dataLength; n++)
                prsp += sprintf(prsp, "%02X", *(p_nv + offset + n));
            prsp += sprintf(prsp, "\"");

            atCmdRespInfoText(cmd->engine, rsp);
            atCmdRespOK(cmd->engine);
            free(rsp);
        }
        else if (operType == 1)
        {
            uint16_t offset = atParamUint(cmd->params[2], &paramok);
            uint16_t dataLength = atParamUint(cmd->params[3], &paramok);
            const char *value = atParamStr(cmd->params[4], &paramok);
            if (!paramok || cmd->param_count != 5 || strlen(value) != dataLength * 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            uint8_t *data = (uint8_t *)malloc(dataLength);
            if (data == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            atMemFreeLater(data);

            if (cfwHexStrToBytes(value, dataLength * 2, data) < 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            // set nv data to ddr
            if (CFW_EmodSetNvData(dataType, offset, dataLength, data, nSim) != 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            atCmdRespOK(cmd->engine);
        }
        else if (operType == 2)
        {
            if (cmd->param_count != 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            // save nv data to flash
            if (CFW_EmodSaveNvData(dataType, nSim) != 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            atCmdRespOK(cmd->engine);
        }
        else
        {
            uint16_t offset = atParamUint(cmd->params[2], &paramok);
            uint16_t dataLength = atParamUint(cmd->params[3], &paramok);
            const char *value = atParamStr(cmd->params[4], &paramok);
            if (!paramok || cmd->param_count != 5 || strlen(value) != dataLength * 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            uint8_t *data = (uint8_t *)malloc(dataLength);
            if (data == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            atMemFreeLater(data);

            if (cfwHexStrToBytes(value, dataLength * 2, data) < 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            if (CFW_EmodSpecialNvProcess(operType, dataType, offset, dataLength, data, nSim) != 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            atCmdRespOK(cmd->engine);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char rsp[64];
        sprintf(rsp, "%s: (0-7),(0-2),(0-65535),(0-65535),", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

void atCmdHandleNVGV(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        uint32_t version = CFW_EmodGetNvVersion();

        char rsp[32];
        char verstr[9];
        cfwBytesToHexStr(&version, 4, verstr);
        sprintf(rsp, "%s: \"%s\"", cmd->desc->name, verstr);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

void atCmdHandleRFCALIB(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;

        uint16_t operType = atParamUint(cmd->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (operType == 0)
        {
            //case 0: read data from ddr
            const char *address_str = atParamStr(cmd->params[1], &paramok);
            uint16_t dataLength = atParamUint(cmd->params[2], &paramok);
            if (!paramok || cmd->param_count > 3)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            unsigned address;
            if (sscanf(address_str, "%08x", &address) != 1)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            char *rsp = (char *)malloc(dataLength * 2 + 32);
            char *prsp = rsp;
            prsp += sprintf(prsp, "+RFCALIB: %d,\"", dataLength);
            prsp += cfwBytesToHexStr((void *)address, dataLength, prsp);
            prsp += sprintf(prsp, "\"");

            atCmdRespInfoText(cmd->engine, rsp);
            atCmdRespOK(cmd->engine);
        }
        else if (operType == 1)
        {
            //case 1: write data to ddr
            const char *address_str = atParamStr(cmd->params[1], &paramok);
            uint16_t dataLength = atParamUint(cmd->params[2], &paramok);
            const char *value = atParamStr(cmd->params[3], &paramok);
            if (!paramok || cmd->param_count > 4)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            unsigned address;
            if (sscanf(address_str, "%08x", &address) != 1)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            uint8_t *data = (uint8_t *)malloc(dataLength);
            if (data == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            atMemFreeLater(data);

            if (cfwHexStrToBytes(value, dataLength * 2, data) < 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            memcpy((void *)(address), data, dataLength);

            atCmdRespOK(cmd->engine);
        }
        else if (operType == 2)
        {
            //case 2: write calib data to flash
            hal_WriteLteCalib();
            hal_WriteGsmCalib();

            atCmdRespOK(cmd->engine);
        }
        else
        {
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

bool _valueInRange(uint32_t value, uint32_t min, uint32_t max)
{
    return value >= min && value <= max;
}

void atCmdHandleSSIT(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint32_t module = atParamUint(cmd->params[0], &paramok);
        uint32_t testcmd = atParamDefUint(cmd->params[1], 0, &paramok);
        uint32_t param1 = atParamDefUint(cmd->params[2], 0, &paramok);
        uint32_t param2 = atParamDefUint(cmd->params[3], 0, &paramok);
        uint32_t param3 = atParamDefUint(cmd->params[4], 0, &paramok);
        if (!paramok || cmd->param_count > 5)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint32_t result = CFW_EmodEngineerCmd(module, testcmd, param1, param2, param3);

        char rsp[32];
        sprintf(rsp, "^SSIT: %d", (unsigned)result);

        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

void atCmdHandleSETDTPORT(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // AT+SETDTPORT=<flag>
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool paramok = true;
        uint8_t flag = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint32_t result = CFW_SwitchPort(flag);
        if (result != 0)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            atCmdRespOK(cmd->engine);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char rsp[32] = {0};
        sprintf(rsp, "+SETDTPORT: (0-1)");

        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

void atCmdHandleCFGNVUPFLAG(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // AT+CFGNVUPFLAG=<flag>
        bool paramok = true;
        uint8_t nSim = atCmdGetSim(cmd->engine);
        uint8_t position = atParamUintInRange(cmd->params[0], 0, 31, &paramok);

        if (((cmd->param_count != 1) && (cmd->param_count != 2)) || !paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (cmd->param_count == 1)
        {
            char rsp[32] = {0};
            sprintf(rsp, "%s: %ld", cmd->desc->name, (gAtSetting.sim[nSim].cfgNvFlag >> position) & 0x01);

            atCmdRespInfoText(cmd->engine, rsp);
        }
        else
        {
            uint8_t flag = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            uint32_t originalValue = (gAtSetting.sim[nSim].cfgNvFlag >> position) & 0x01;
            if (originalValue == flag)
                RETURN_OK(cmd->engine);

            originalValue = gAtSetting.sim[nSim].cfgNvFlag;
            originalValue &= ~(1 << position);
            originalValue |= flag << position;
            gAtSetting.sim[nSim].cfgNvFlag = originalValue;
            if (CFW_SetNvUpdateFlag(gAtSetting.sim[nSim].cfgNvFlag, nSim) != 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char rsp[32] = {0};
        sprintf(rsp, "%s: (0-1)", cmd->desc->name);

        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID);
}