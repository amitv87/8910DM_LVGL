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

#ifdef CONFIG_AT_MYINFO_SUPPORT

#include "cfw.h"
#include "stdlib.h"

#include "osi_api.h"
#include "osi_log.h"
#include "stdio.h"
#include "string.h"
#include "at_response.h"
#include "at_cfw.h"
#include "at_engine.h"
#include "at_command.h"
#include "cfw.h"
#include "at_cmd_myinfo.h"
#include "at_cfg.h"
#include "at_cfg.h"

#define MAX_KEY_LEN 16

#define SUPPORT_TRANS 1
#define SUPPORT_NORM 1

#define SUPPORT_GPRS2G 1
#define SUPPORT_EMTC 0
#define SUPPORT_NBIOT 0

#define SUPPORT_GPS 0
#define SUPPORT_BDS 0

#define EXTEND_BYTE 0

unsigned char randkey[MAX_KEY_LEN];

static int myrand(unsigned char *output, size_t len)
{
    if (output == NULL)
        return -1;
    unsigned char *dest = output;
    int rnd;
    int i;
    srand(osiUpTimeUS());
    char keyboundary[63] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (i = 0; i < len; i++)
    {
        rnd = rand() % 62;
        *dest = keyboundary[rnd];
        dest++;
    }
    return (0);
}

void AT_TCPIP_CmdFunc_MYGETKEY(atCommand_t *cmd)
{
    char AtRet[128] = {0};

    if (cmd->type == AT_CMD_EXE)
    {
        int ret = -1;
        ret = myrand(randkey, MAX_KEY_LEN);
        if (ret < 0)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        atCmdRespInfoText(cmd->engine, (const char *)randkey);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "AT+MYGETKEY: command not supported");
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYSETINFO(atCommand_t *cmd)
{
    bool paramok = true;
    int i, j;
    char AtRet[128] = {0};
    if (cmd->type == AT_CMD_SET)
    {
        const char *key = atParamStr(cmd->params[0], &paramok);
        const char *type = atParamStr(cmd->params[1], &paramok);
        //const char* type_name = atParamStr(cmd->params[2], &paramok);
        if (!paramok)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        char skey[MAX_KEY_LEN] = {0};
        memcpy(skey, randkey, MAX_KEY_LEN);
        for (i = 0; i < MAX_KEY_LEN - 1; i++)
        {
            for (j = 0; j < MAX_KEY_LEN - 1 - i; j++)
            {
                if (skey[j] > skey[j + 1])
                {
                    char tmp_key = skey[j + 1];
                    skey[j + 1] = skey[j];
                    skey[j] = tmp_key;
                }
            }
        }
        if (strlen(key) != MAX_KEY_LEN || strncmp((const char *)skey, key, MAX_KEY_LEN) != 0)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        if (strcmp(type, "module_info") == 0)
        {
        }
        else if (strcmp(type, "revision") == 0)
        {
        }
        else if (strcmp(type, "aid") == 0)
        {
        }
        else if (strcmp(type, "sq_send_interval") == 0)
        {
        }
        else if (strcmp(type, "tar") == 0)
        {
        }
        else if (strcmp(type, "mygmr") == 0)
        {
        }
        else
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "AT+MYGETKEY: command not supported");
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYTYPE(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_READ)
    {
        uint8_t mode = SUPPORT_NORM << 1 | SUPPORT_TRANS;
        uint8_t nwType = SUPPORT_NBIOT << 2 | SUPPORT_EMTC << 1 | SUPPORT_GPRS2G;
        uint8_t extendFeature = EXTEND_BYTE << 7 | SUPPORT_BDS << 1 | SUPPORT_GPS;

        char rspStr[128] = {0};
        sprintf(rspStr, "$MYTYPE: %02X, %02X, %02X", mode, nwType, extendFeature);
        atCmdRespInfoText(cmd->engine, rspStr);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        char AtRet[128] = {0};
        OSI_LOGI(0, "AT+MYTYPE: command not supported");
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYPOWEROFF(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        char rspStr[20] = {0};
        sprintf(rspStr, "OK");
        //need api for reboot
        atCmdRespOKText(cmd->engine, rspStr);
        osiShutdown(OSI_SHUTDOWN_POWER_OFF);
    }
    else
    {
        OSI_LOGI(0, "AT$MYPOWEROFF: command not supported");
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

#ifdef CONFIG_SOC_6760
extern uint16_t RF_Report_Calibrate_flag(void);
#else
OSI_WEAK uint16_t RF_Report_Calibrate_flag(void)
{
    return 0;
}
#endif

void AT_TCPIP_CmdFunc_MYCALIB(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_READ)
    {
        char rspStr[20] = {0};
        uint16_t flag = 0;
        flag = RF_Report_Calibrate_flag();
        sprintf(rspStr, "$MYCALIB: %d", flag);
        atCmdRespOKText(cmd->engine, rspStr);
    }
    else if (cmd->type == AT_CMD_EXE)
    {
        char rspStr[20] = {0};
        sprintf(rspStr, "$MYCALIB");
        //need api for set calib
        //RF_Calibrate();
        atCmdRespOKText(cmd->engine, rspStr);
    }
    else
    {
        OSI_LOGI(0, "AT$MYPOWEROFF: command not supported");
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

#endif
