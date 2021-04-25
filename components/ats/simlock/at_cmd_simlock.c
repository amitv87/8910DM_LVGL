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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('S', 'I', 'M', 'L')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "atr_config.h"
#include "cfw_config.h"
#include "cfw_chset.h"
#include "osi_log.h"
#include "at_cfg.h"
#include "at_cfw.h"
#include "cfw.h"
#include "cfw_errorcode.h"
#include "ml.h"
#include "mal_api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <inttypes.h>
#include "srv_simlock.h"

#define S_ATC_DISCARD 2
#define S_ATC_SUCCESS 1
#define S_ATC_FAIL 0
#define ATC_STATUS uint32_t

typedef struct
{
    uint32_t pos;
    uint32_t size;
    char *mem;
} simlockAsyncCtx_t;
/*****************************************************************************/
//  Description : This function handle the AT^SPHUK  command
//  Global resource dependence : none
//  Author:
//  Note:
/*****************************************************************************/

void atCmdHandleSimlockSPHUK(atCommand_t *cmd)
{
    ATC_STATUS status = S_ATC_SUCCESS;
    bool paramok = true;
    const char *data = NULL;
    switch (cmd->type)
    {
    case AT_CMD_SET:
        data = atParamStr(cmd->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        status = srvSimlockSetHuk(data, strlen(data));
        if (status == S_ATC_SUCCESS)
            atCmdRespOK(cmd->engine);
        else
            atCmdRespCmeError(cmd->engine, ERR_AT_UNKNOWN);
        break;
    case AT_CMD_READ:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    case AT_CMD_TEST:
        atCmdRespInfoText(cmd->engine, "^SPHUK:HUK\r\n HUK is 8 digits");
        atCmdRespOK(cmd->engine);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    }
}

/*****************************************************************************/
//  Description : This function handle the AT^SPFACAUTHPUBKEY  command
//  Global resource dependence : none
//  Author:
//  Note:
/*****************************************************************************/
void atCmdHandleSimlockSPFACAUTHPUBKEY(atCommand_t *cmd)
{

    bool paramok = true;
    ATC_STATUS status = S_ATC_SUCCESS;
    char rsp[32];
    const char *pubkey;
    const char *sign;

    switch (cmd->type)
    {
    case AT_CMD_SET:
        pubkey = atParamStr(cmd->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        sign = atParamStr(cmd->params[1], &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        status = srvSimlockFacauthPubkey(pubkey, strlen(pubkey), sign, strlen(sign));
        sprintf(rsp, "xor_SPFACAUTHPUBKEY=%lu", status);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
        break;
    case AT_CMD_READ:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    case AT_CMD_TEST:
        atCmdRespInfoText(cmd->engine, "^SPFACAUTHPUBKEY:public key, sign\r\n");
        atCmdRespOK(cmd->engine);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    }
}

/*****************************************************************************/
//  Description : This function handle the AT^SPIDENTIFYSTART  command
//  Global resource dependence : none
//  Author:
//  Note:
/*****************************************************************************/
void atCmdHandleSimlockSPIDENTIFYSTART(atCommand_t *cmd)
{

    ATC_STATUS status = S_ATC_SUCCESS;
    char data[SL_RSA_N_LEN + 1] = {0}; //must initialize to  "0"
    uint32_t len = sizeof(data);
    char rsp[32 + SL_RSA_N_LEN + 1];

    switch (cmd->type)
    {
    case AT_CMD_EXE:
        status = srvSimlockAuthStart(data, &len);
        if (status == S_ATC_SUCCESS)
        {
            sprintf(rsp, "%s=%s", "xor_SPIDENTIFYSTART", data);
            atCmdRespInfoText(cmd->engine, rsp);
            atCmdRespOK(cmd->engine);
        }
        else
            atCmdRespCmeError(cmd->engine, ERR_AT_UNKNOWN);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    }
}

/*****************************************************************************/
//  Description : This function handle the AT^SPIDENTIFYEND  command
//  Global resource dependence : none
//  Author:
//  Note:
/*****************************************************************************/
void atCmdHandleSimlockSPIDENTIFYEND(atCommand_t *cmd)
{

    ATC_STATUS status = S_ATC_SUCCESS;
    bool paramok = true;
    char rsp[32];
    const char *data;

    switch (cmd->type)
    {
    case AT_CMD_SET:
        data = atParamStr(cmd->params[0], &paramok);
        if (paramok != true)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        status = srvSimlockAuthEnd(data, strlen(data));
        sprintf(rsp, "%s=%lu", "xor_SPIDENTIFYEND", status);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
        break;
    case AT_CMD_READ:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    case AT_CMD_TEST:
        atCmdRespInfoText(cmd->engine, "^SPIDENTIFYEND:data\r\n");
        atCmdRespOK(cmd->engine);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    }
}

/*****************************************************************************/
//  Description : This function handle the AT^SPDATAENCRYPT  command
//  Global resource dependence : none
//  Author:
//  Note:
/*****************************************************************************/
void atCmdHandleSimlockSPDATAENCRYPT(atCommand_t *cmd)
{
    ATC_STATUS status = S_ATC_SUCCESS;
    char *encrypt_key = NULL;
    bool paramok = true;

#define PAD_LEN 31
    switch (cmd->type)
    {
    case AT_CMD_SET:
        encrypt_key = malloc(2 * sizeof(simlock_encrypt_keys_t) + 1);
        if (encrypt_key == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        memset(encrypt_key, 0, 2 * sizeof(simlock_encrypt_keys_t) + 1);

        char *rsp = malloc(2 * sizeof(simlock_encrypt_keys_t) + 1 + PAD_LEN);
        if (rsp == NULL)
        {
            free(encrypt_key);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        }
        memset(rsp, 0, 2 * sizeof(simlock_encrypt_keys_t) + PAD_LEN);

        const char *data = atParamStr(cmd->params[0], &paramok);
        if (!paramok)
        {
            free(encrypt_key);
            free(rsp);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        status = srvSimlockEncryptKeys(data, strlen(data), (char *)encrypt_key, 2 * sizeof(simlock_encrypt_keys_t) + 1);
        if (status == S_ATC_SUCCESS)
        {
            sprintf(rsp, "%s=%s", "xor_SPDATAENCRYPT", encrypt_key);
            atCmdRespInfoText(cmd->engine, rsp);
            atCmdRespOK(cmd->engine);
        }
        else
        {
            atCmdRespCmeError(cmd->engine, ERR_AT_UNKNOWN);
        }

        if (encrypt_key)
        {
            free(encrypt_key);
            free(rsp);
        }
        break;
    case AT_CMD_READ:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    case AT_CMD_TEST:
        atCmdRespInfoText(cmd->engine, "^SPDATAENCRYPT:data \r\n");
        atCmdRespOK(cmd->engine);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    }
}

/*****************************************************************************/
//  Description : This function handle the AT^SPSIMLOCKDATAWRITE  PDU process func
//  Global resource dependence : none
//  Author:
//  Note:
/*****************************************************************************/
static int prvProcessSimlockData(void *param, const void *data, size_t size)
{
    uint32_t status;
    atCommand_t *cmd = (atCommand_t *)param;
    simlockAsyncCtx_t *async = (simlockAsyncCtx_t *)cmd->async_ctx;

    OSI_LOGI(0, "SIMLOCK: async->size =%d async->pos=%d size=%d",
             async->size, async->pos, size);

    size_t len = async->size - async->pos;
    if (len > size)
        len = size;
    memcpy(async->mem + async->pos, data, len);
    async->pos += len;

    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    if (chsetting->ate)
        atCmdWrite(cmd->engine, data, len);

    if (async->pos >= async->size)
    {
        status = srvSimlockDataProcess((char *)(async->mem), async->size);
        if (status == S_ATC_SUCCESS)
            atCmdRespOK(cmd->engine);
        else
            atCmdRespCmeError(cmd->engine, ERR_AT_UNKNOWN);
    }
    return len;
}

void atCmdHandleSimlockSimlockData(atCommand_t *cmd)
{
    size_t length;
    bool paramok = true;
    simlockAsyncCtx_t *async = NULL;

    switch (cmd->type)
    {
    case AT_CMD_SET:
        length = atParamUintInRange(cmd->params[0], 0, 5120, &paramok);
        async = (simlockAsyncCtx_t *)malloc(sizeof(*async) + length);
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        async->size = length;
        async->pos = 0;
        async->mem = (char *)async + sizeof(*async);

        if (!paramok)
        {
            OSI_LOGI(0, "SIMLOCK: atCmdHandleSimlockSimlockData 0");
        }
        atCmdRespOutputPrompt(cmd->engine);
        atCmdSetBypassMode(cmd->engine, prvProcessSimlockData, cmd);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    }
}

/*****************************************************************************/
//  Description : This function handle the AT^SPSIMLOCKIMEI  command
//  Global resource dependence : none
//  Author:
//  Note:
/*****************************************************************************/
void atCmdHandleSimlockSPSIMLOCKIMEI(atCommand_t *cmd)
{
    atCmdRespOK(cmd->engine);
}

uint32_t CFW_GetSimLockStatusFromAP(CFW_SRV_SIMLOCKDATA_T *data)
{
    srvSimlockData_t *pSimLockData = (srvSimlockData_t *)malloc(sizeof(srvSimlockData_t));
    if (pSimLockData == NULL)
        return 1;

    memset(pSimLockData, 0x00, sizeof(srvSimlockData_t));
    srvSimlockGetLocksData(pSimLockData);

    data->SIM_lock_status = pSimLockData->SIM_lock_status;
    data->max_num_trials = pSimLockData->max_num_trials;

    data->network_locks.numLocks = pSimLockData->network_locks.numLocks;
    data->network_locks.reserved = pSimLockData->network_locks.reserved;
    memcpy(data->network_locks.locks, pSimLockData->network_locks.locks, CFW_MAX_PERSONALISATIONS * sizeof(CFW_SIM_LOCK_PLMN_T));

    data->network_subset_locks.numLocks = pSimLockData->network_subset_locks.numLocks;
    data->network_subset_locks.reserved = pSimLockData->network_subset_locks.reserved;
    memcpy(data->network_subset_locks.locks, pSimLockData->network_subset_locks.locks, CFW_MAX_PERSONALISATIONS * sizeof(CFW_NETWORK_SUBSET_LOCK_DATA_T));

    data->SP_locks.numLocks = pSimLockData->SP_locks.numLocks;
    data->SP_locks.reserved = pSimLockData->SP_locks.reserved;
    memcpy(data->SP_locks.locks, pSimLockData->SP_locks.locks, CFW_MAX_PERSONALISATIONS * sizeof(CFW_SP_LOCK_DATA_T));

    data->corporate_locks.numLocks = pSimLockData->corporate_locks.numLocks;
    data->corporate_locks.reserved = pSimLockData->corporate_locks.reserved;
    memcpy(data->corporate_locks.locks, pSimLockData->corporate_locks.locks, CFW_MAX_PERSONALISATIONS * sizeof(CORPORATE_LOCK_DATA_T));

    data->user_locks.numLocks = pSimLockData->user_locks.numLocks;
    memcpy(&data->user_locks.locks, &pSimLockData->user_locks.locks, sizeof(USER_LOCK_DATA_T) * 3);

    free(pSimLockData);
    return 0;
}
