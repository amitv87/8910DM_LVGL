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
#include "lwip/apps/sntp.h"
#include "at_cmd_sntp.h"
#include "at_response.h"
#include "osi_log.h"
#include "sockets.h"

#define AT_SNTP_SERVER_MAX_LEN 128

extern struct netif *netif_default;

CFW_SNTP_CONFIG *sntpClient = NULL;
char cNtpServer[AT_SNTP_SERVER_MAX_LEN + 1];

/*****************************************************************************
* Name:        sntpSyncDoneCB
* Description: sntp callback 
* Parameter:  void *param
* Return:       viod
* Remark:      n/a
* Author:       Alfred
* Data:          2019-05-23
******************************************************************************/
void sntpSyncDoneCB(void *param)
{
    cbResult *result = (cbResult *)param;
    char cString[20] = {0};
    if (result->iRspCode == 0)
    {
        sprintf(cString, "+SNTP: FAILED\r\n");
    }
    else
    {
        sprintf(cString, "+SNTP: OK\r\n");
    }
    atCmdRespDefUrcText(cString);

    CFW_SntpStop(sntpClient);
    sntpClient = NULL;
}

/*****************************************************************************
* Name:        sntpSyncDone
* Description:  finish sntp sync and callback 
* Parameter:  void *param
* Return:       viod
* Remark:      n/a
* Author:       Alfred
* Data:          2019-05-23
******************************************************************************/
void sntpSyncDone(void *param)
{
    osiThreadCallback(atEngineGetThreadId(), sntpSyncDoneCB, param);
}

/*****************************************************************************
* Name:        sntpConfigInit
* Description: sntp client config init 
* Parameter:  void
* Return:       void
* Remark:      n/a
* Author:       Alfred
* Data:          2019-05-23
******************************************************************************/
void sntpConfigInit()
{
    sntpClient = CFW_SntpInit();
    if (sntpClient != NULL)
    {
        sntpClient->cServer = cNtpServer;
        memset(sntpClient->cServer, 0, AT_SNTP_SERVER_MAX_LEN + 1);
        sntpClient->fCallback = sntpSyncDone;
    }
}

/*****************************************************************************
* Name:        sntpUpdateStart
* Description:  start sntp sync operation 
* Parameter:  void
* Return:       sntp_status_t
* Remark:      n/a
* Author:       Alfred
* Data:          2019-05-23
******************************************************************************/
sntp_status_t sntpUpdateStart()
{
    return CFW_SntpStart(sntpClient);
}

/*****************************************************************************
* Name:        AT_CmdFunc_SNTP
* Description:  Sync system time with the NTP server 
* Parameter:  atCommand_t *cmd
* Return:       void
* Remark:      n/a
* Author:       Alfred
* Data:          2019-05-23
******************************************************************************/
void AT_CmdFunc_SNTP(atCommand_t *cmd)
{
    atCmdEngine_t *engine = cmd->engine;
    bool paramok = true;
    sntp_status_t result;
    ip_addr_t addr;

    OSI_LOGI(0, "AT+SNTP ... ...");

    if (AT_CMD_SET == cmd->type)
    {
        if (netif_default == NULL)
        {
            OSI_LOGI(0, "AT_CmdFunc_SNTP: network is not ready\n");
            RETURN_CME_ERR(engine, ERR_AT_CME_HTTP_NETIF_NULL);
        }

        if (cmd->param_count > 1)
        {
            OSI_LOGI(0, "AT_CmdFunc_SNTP: param number error\n");
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        }
        const char *tmpStr = atParamStr(cmd->params[0], &paramok);
        if (!paramok)
        {
            OSI_LOGI(0, "AT_CmdFunc_SNTP: param 0 error\n");
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (strlen(tmpStr) > AT_SNTP_SERVER_MAX_LEN || strlen(tmpStr) == 0)
        {
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (ipaddr_aton(tmpStr, &addr) == 0)
        {
            if (strstr(tmpStr, ".") == NULL || strlen(tmpStr) < 4)
            {
                RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        if (sntpClient == NULL)
        {
            sntpConfigInit();
            if (sntpClient == NULL)
            {
                OSI_LOGI(0, "AT_CmdFunc_SNTP:  malloc failed\n");
                RETURN_CME_ERR(engine, ERR_AT_CME_NO_MEMORY);
            }
        }
        else
        {
            OSI_LOGI(0, "AT_CmdFunc_SNTP:  sync time processing\n");
            RETURN_CME_ERR(engine, ERR_AT_CME_SNTP_SYNCING);
        }

        strncpy(sntpClient->cServer, tmpStr, AT_SNTP_SERVER_MAX_LEN);
        sntpClient->cServer[AT_SNTP_SERVER_MAX_LEN] = '\0';

        OSI_LOGXI(OSI_LOGPAR_S, 0, "AT_CmdFunc_SNTP: ntpServer = %s", sntpClient->cServer);

        result = sntpUpdateStart(sntpClient);

        if (result == CFW_SNTP_READY)
        {
            RETURN_OK(engine);
        }
        else if (result == CFW_SNTP_PARAM_INVALID)
        {
            free(sntpClient);
            sntpClient = NULL;
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        }
        else
        {
            OSI_LOGI(0, "AT_CmdFunc_SNTP:  sync time is processing\n");
            RETURN_CME_ERR(engine, ERR_AT_CME_SNTP_SYNCING);
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[20] = {0};

        sprintf(aucBuffer, "+SNTP:ntpserver");
        atCmdRespInfoText(engine, aucBuffer);
        RETURN_OK(engine);
    }
    else
    {
        OSI_LOGI(0, "AT_CmdFunc_SNTP, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
