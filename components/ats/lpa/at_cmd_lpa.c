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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('L', 'P', 'A', 'D')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "atr_config.h"
#include "cfw_config.h"
#include "cfw_chset.h"
#include "osi_log.h"
#include "osi_sysnv.h"
#include "at_cfg.h"
#include "at_cfw.h"
#include "cfw.h"
#include "cfw_errorcode.h"
#include "ml.h"
#include "mal_api.h"
#include "srv_sim_detect.h"
#include "drv_md_ipc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <inttypes.h>

#include "lpa_config.h"
#include "lpa.h"

#ifdef CONFIG_LPA_SUPPORT
typedef enum
{
    LPA_FUNC_DOWLOAD,
    LPA_FUNC_GET_EUICC_CONFIGURED_ADDRESS,
    LPA_FUNC_SET_DEFAULT_DP_ADDRESS,
    LPA_FUNC_GET_PROFILES_INFO,
    LPA_FUNC_ENABLE_PROFILE,
    LPA_FUNC_DISABLE_PROFILE,
    LPA_FUNC_DELETE_PROFILE,
    LPA_FUNC_EUICC_MEMORY_RESET,
    LPA_FUNC_GET_EID,
    LPA_FUNC_SET_NICKNAME,
    LPA_FUNC_START_GUARDIAN,
    LPA_FUNC_APP_ID,
    LPA_FUNC_APP_SECRET,
    LPA_FUNC_APP_URL,
} LPA_FUNC_E;

static uint8_t _GetFuncIndex(const char *pFuncName)
{
    char upperName[32] = {
        0,
    };
    int nLen = strlen(pFuncName);
    for (int i = 0; i < nLen; i++)
    {
        if (pFuncName[i] >= 'a' && pFuncName[i] <= 'z')
            upperName[i] = 'A' + (pFuncName[i] - 'a');
        else
            upperName[i] = pFuncName[i];
    }

    OSI_LOGXI(OSI_LOGPAR_S, 0, "upper name: %s", upperName);

    if (strcmp(upperName, "DOWNLOAD") == 0)
    {
        return LPA_FUNC_DOWLOAD;
    }

    if (strcmp(upperName, "GETEUICCCONFIGUREDADDRESS") == 0)
    {
        return LPA_FUNC_GET_EUICC_CONFIGURED_ADDRESS;
    }

    if (strcmp(upperName, "SETDEFAULTDPADDRESS") == 0)
    {
        return LPA_FUNC_SET_DEFAULT_DP_ADDRESS;
    }

    if (strcmp(upperName, "GETPROFILESINFO") == 0)
    {
        return LPA_FUNC_GET_PROFILES_INFO;
    }

    if (strcmp(upperName, "ENABLEPROFILE") == 0)
    {
        return LPA_FUNC_ENABLE_PROFILE;
    }

    if (strcmp(upperName, "DISABLEPROFILE") == 0)
    {
        return LPA_FUNC_DISABLE_PROFILE;
    }

    if (strcmp(upperName, "DELETEPROFILE") == 0)
    {
        return LPA_FUNC_DELETE_PROFILE;
    }

    if (strcmp(upperName, "EUICCMEMORYRESET") == 0)
    {
        return LPA_FUNC_EUICC_MEMORY_RESET;
    }

    if (strcmp(upperName, "GETEID") == 0)
    {
        return LPA_FUNC_GET_EID;
    }

    if (strcmp(upperName, "SETNICKNAME") == 0)
    {
        return LPA_FUNC_SET_NICKNAME;
    }

    if (strcmp(upperName, "STARTGUARDIAN") == 0)
    {
        return LPA_FUNC_START_GUARDIAN;
    }

    if (strcmp(upperName, "APPID") == 0)
    {
        return LPA_FUNC_APP_ID;
    }

    if (strcmp(upperName, "APPSECRET") == 0)
    {
        return LPA_FUNC_APP_SECRET;
    }

    if (strcmp(upperName, "APPURL") == 0)
    {
        return LPA_FUNC_APP_URL;
    }
    return 0xFF;
}

static void _handleLpaAppDownload(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
    OSI_LOGI(0, "_handleLpaAppDownload, nStatus:%d", nStatus);
    atCommand_t *cmd = (atCommand_t *)ctx;
    LPA_DOWNLOADD_RESULT_T *pResult = (LPA_DOWNLOADD_RESULT_T *)pData;
    OSI_LOGI(0, "pResult->nStatus:%d", pResult->nStatus);
    OSI_LOGXI(OSI_LOGPAR_S, 0, "desc:%s", pResult->pDes);
    atCmdRespInfoText(cmd->engine, (char *)pResult->pDes);
    if (nStatus)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {

        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}

static void _handleLdsdGetEUICCConfiguredAddress(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
    OSI_LOGI(0, "_handleLdsdGetEUICCConfiguredAddress, nStatus:%d", nStatus);
    atCommand_t *cmd = (atCommand_t *)ctx;
    if (nStatus)
    {
        char *pResultStr = malloc(256);
        memset(pResultStr, 0, 256);
        strcpy(pResultStr, "Address:\"");
        EUICC_CONFIGURED_ADDRESS_T *pRsp = (EUICC_CONFIGURED_ADDRESS_T *)pData;
        if (pRsp->pDefaultDpAddress != NULL)
        {
            int nLen = 0;
            char *pTemp = mlConvertStr((const void *)pRsp->pDefaultDpAddress, strlen(pRsp->pDefaultDpAddress), ML_UTF8, ML_CP936, &nLen);
            strcat(pResultStr, pTemp);
            free(pTemp);
        }
        strcat(pResultStr, "\",\"");
        if (pRsp->pRootDsAddress != NULL)
        {
            int nLen = 0;
            char *pTemp = mlConvertStr((const void *)pRsp->pRootDsAddress, strlen(pRsp->pRootDsAddress), ML_UTF8, ML_CP936, &nLen);
            strcat(pResultStr, pTemp);
            free(pTemp);
        }
        strcat(pResultStr, "\"");
        atCmdRespInfoText(cmd->engine, pResultStr);
        RETURN_OK(cmd->engine);
        free(pResultStr);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}

static void _handleLdsdSetDefaultDpAddress(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
    OSI_LOGI(0, "_handleLdsdSetDefaultDpAddress, nStatus:%d", nStatus);
    atCommand_t *cmd = (atCommand_t *)ctx;
    if (nStatus)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}

static void _handleGetProfilesInfo(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
#define PROFILE_INFO "PROFILE INFO:"
    atCommand_t *cmd = (atCommand_t *)ctx;
    OSI_LOGI(0, "_handleGetProfilesInfo, nStatus: %d", nStatus);
    char *pRspStr = NULL;
    if (nStatus)
    {
        char tempStr[LPA_AID_LENGTH * 2 + 1] = {
            0,
        };

        LPA_PROFILE_INFO_LIST_T *pRsp = (LPA_PROFILE_INFO_LIST_T *)pData;
        uint16_t nLen = ((LPA_ICCID_LENGTH + LPA_AID_LENGTH) * 2 + 8) * (pRsp->nProfofileInfo) + strlen(PROFILE_INFO) + LPA_PROFILE_NICK_NAME_LENGTH + 10;
        OSI_LOGI(0, "%d Profile present", pRsp->nProfofileInfo);
        pRspStr = malloc(nLen);
        memset(pRspStr, 0, nLen);
        strcpy(pRspStr, PROFILE_INFO);
        for (uint16_t i = 0; i < pRsp->nProfofileInfo; i++)
        {
            OSI_LOGI(0, "Profile: %d", i);
            strcat(pRspStr, "\n\"");
            if (pRsp->pProfileInfo[i]->bIccid)
            {
                OSI_LOGXI(OSI_LOGPAR_M, 0, "iccid:%*s", LPA_ICCID_LENGTH, pRsp->pProfileInfo[i]->iccid);
                cfwBytesToHexStr((const void *)pRsp->pProfileInfo[i]->iccid, LPA_ICCID_LENGTH, tempStr);
                strcat(pRspStr, tempStr);
            }
            strcat(pRspStr, "\",\"");
            if (pRsp->pProfileInfo[i]->bIsDpAid)
            {
                OSI_LOGXI(OSI_LOGPAR_M, 0, "isdpAid:%*s", LPA_AID_LENGTH, pRsp->pProfileInfo[i]->isdpAid);
                cfwBytesToHexStr((const void *)pRsp->pProfileInfo[i]->isdpAid, LPA_AID_LENGTH, tempStr);
                strcat(pRspStr, tempStr);
            }
            strcat(pRspStr, "\",");

            if (pRsp->pProfileInfo[i]->bProfileState)
            {
                sprintf(tempStr, "%d", pRsp->pProfileInfo[i]->profileState);
                strcat(pRspStr, tempStr);
            }
            strcat(pRspStr, ",\"");
            if (pRsp->pProfileInfo[i]->bProfileNickName)
            {
                int nStrlen = 0;
                char *pNickname = mlConvertStr((const void *)pRsp->pProfileInfo[i]->profileNickname, strlen((const char *)pRsp->pProfileInfo[i]->profileNickname), ML_UTF8, ML_CP936, &nStrlen);
                strcat(pRspStr, pNickname);
            }
            strcat(pRspStr, "\"");
        }
        atCmdRespInfoText(cmd->engine, pRspStr);
        free(pRspStr);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "None Profile present");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}

static void _handleEnableProfile(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
    atCommand_t *cmd = (atCommand_t *)ctx;
    OSI_LOGI(0, "_handleEnableProfile, nStatus: %d", nStatus);
    if (nStatus)
    {
        OSI_LOGI(0, "execute success");
        uint8_t *pEnableResult = (uint8_t *)pData;
        if (*pEnableResult == 0)
        {
            OSI_LOGI(0, "Enable success");
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "Enable fail for result: %d", *pEnableResult);
        }
    }
    else
    {
        OSI_LOGI(0, "execute fail");
    }

    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

static void _handleDisableProfile(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
    atCommand_t *cmd = (atCommand_t *)ctx;
    OSI_LOGI(0, "_handleDisableProfile, nStatus: %d", nStatus);
    if (nStatus)
    {
        OSI_LOGI(0, "execute success");
        uint8_t *pDisableResult = (uint8_t *)pData;
        if (*pDisableResult == 0)
        {
            OSI_LOGI(0, "Disable success");
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "Disable fail for result: %d", *pDisableResult);
        }
    }
    else
    {
        OSI_LOGI(0, "execute fail");
    }

    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

static void _handleDeleteProfile(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
    atCommand_t *cmd = (atCommand_t *)ctx;
    OSI_LOGI(0, "lpa_DeleteProfile_Callback, nStatus: %d", nStatus);
    if (nStatus)
    {
        OSI_LOGI(0, "execute success");
        uint8_t *pDeleteResult = (uint8_t *)pData;
        if (*pDeleteResult == 0)
        {
            OSI_LOGI(0, "Delete success");
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "Delete fail for result: %d", *pDeleteResult);
        }
    }
    else
    {
        OSI_LOGI(0, "execute fail");
    }

    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

void _handleEUICCMemoryReset(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
    atCommand_t *cmd = (atCommand_t *)ctx;
    OSI_LOGI(0, "_handleEUICCMemoryReset, nStatus: %d", nStatus);
    if (nStatus)
    {
        OSI_LOGI(0, "execute success");
        uint8_t *pResetResult = (uint8_t *)pData;
        if (*pResetResult == 0)
        {
            OSI_LOGI(0, "Reset success");
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "Reset fail for result: %d", *pResetResult);
        }
    }
    else
    {
        OSI_LOGI(0, "execute fail");
    }

    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

static void _handleGetEID(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
    OSI_LOGI(0, "_handleGetEID,nStatus:%d", nStatus);
    atCommand_t *cmd = (atCommand_t *)ctx;
    if (nStatus)
    {
#define LPA_EID "EID:"
        uint8_t nLen = strlen(LPA_EID) + LPA_EID_LENGTH * 2 + 2;
        char *pRspStr = malloc(nLen);
        memset(pRspStr, 0, nLen);
        strcpy(pRspStr, LPA_EID);
        char tempStr[LPA_EID_LENGTH * 2 + 1] = {0};
        cfwBytesToHexStr((const void *)pData, LPA_EID_LENGTH, tempStr);
        strcat(pRspStr, tempStr);
        atCmdRespInfoText(cmd->engine, pRspStr);
        free(pRspStr);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}

void _handleSetNickname(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
    atCommand_t *cmd = (atCommand_t *)ctx;
    OSI_LOGI(0, "_handleSetNickname, nStatus: %d", nStatus);
    if (nStatus)
    {
        OSI_LOGI(0, "execute success");
        char *pResult = pData;
        if (*pResult == 0)
        {
            OSI_LOGI(0, "success");
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "fail for %d", *pResult);
        }
    }
    else
    {
        OSI_LOGI(0, "execute fail");
    }

    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

void _handleGuardian(void *ctx, uint8_t nStatus, void *pData, CFW_SIM_ID nSimId)
{
    atCommand_t *cmd = (atCommand_t *)ctx;
    OSI_LOGI(0, "_handleGuardian, nStatus: %d", nStatus);
    if (nStatus)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        atCmdRespInfoText(cmd->engine, (const char *)pData);
    }

    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

void atCmdHandleLPA(atCommand_t *cmd)
{
    OSI_LOGI(0, "atCmdHandleLPA");
    if (cmd->type == AT_CMD_SET)
    {
        uint8_t nSim = atCmdGetSim(cmd->engine);
        bool paramok = true;
        const char *pFuncName = atParamStr(cmd->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        OSI_LOGXI(OSI_LOGPAR_S, 0, "pFuncName: %s", pFuncName);
        uint8_t nFunc = _GetFuncIndex(pFuncName);
        switch (nFunc)
        {
        case LPA_FUNC_DOWLOAD:
        {
            if (lpa_app_download((void *)cmd, "esim.wo.cn", _handleLpaAppDownload, nSim) == LPA_SUCCESS)
                RETURN_FOR_ASYNC();
            else
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        case LPA_FUNC_GET_EUICC_CONFIGURED_ADDRESS:
        {
            if (ldsd_GetEUICCConfiguredAddress((void *)cmd, _handleLdsdGetEUICCConfiguredAddress, nSim) == LPA_SUCCESS)
                RETURN_FOR_ASYNC();
            else
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        case LPA_FUNC_SET_DEFAULT_DP_ADDRESS:
        {
            const char *pAddress = atParamStr(cmd->params[1], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            int nLen = 0;
            uint8_t *pUtf8Address = mlConvertStr((const void *)pAddress, strlen(pAddress), ML_CP936, ML_UTF8, &nLen);
            if (ldsd_SetDefaultDpAddress((void *)cmd, pUtf8Address, nLen, _handleLdsdSetDefaultDpAddress, nSim) == LPA_SUCCESS)
            {
                free(pUtf8Address);
                RETURN_FOR_ASYNC();
            }
            else
            {
                free(pUtf8Address);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        case LPA_FUNC_GET_PROFILES_INFO:
        {
            if (luid_GetProfilesInfo((void *)cmd, _handleGetProfilesInfo, nSim) == LPA_SUCCESS)
            {
                RETURN_FOR_ASYNC();
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        case LPA_FUNC_ENABLE_PROFILE:
        {
            const char *pIccid = atParamStr(cmd->params[1], &paramok);
            if (!paramok || (strlen(pIccid) != 0 && strlen(pIccid) != LPA_ICCID_LENGTH * 2))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pIccid:%s", pIccid);

            const char *pAid = atParamStr(cmd->params[2], &paramok);
            if (!paramok || (strlen(pAid) != 0 && strlen(pAid) != LPA_AID_LENGTH * 2))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pAid:%s", pAid);

            uint8_t pData[LPA_AID_LENGTH] = {
                0,
            };
            uint8_t bIccid = 0;
            uint8_t refreshFlag = 1;
            if (strlen(pIccid) != 0)
            {
                cfwHexStrToBytes((const void *)pIccid, LPA_ICCID_LENGTH * 2, pData);
                bIccid = 1;
            }
            else
            {
                cfwHexStrToBytes((const void *)pAid, LPA_AID_LENGTH * 2, pData);
            }

            if (luid_EnableProfile((void *)cmd, bIccid, (uint8_t *)pData, refreshFlag, _handleEnableProfile, nSim) == LPA_SUCCESS)
            {
                RETURN_FOR_ASYNC();
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        case LPA_FUNC_DISABLE_PROFILE:
        {
            const char *pIccid = atParamStr(cmd->params[1], &paramok);
            if (!paramok || (strlen(pIccid) != 0 && strlen(pIccid) != LPA_ICCID_LENGTH * 2))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pIccid:%s", pIccid);

            const char *pAid = atParamStr(cmd->params[2], &paramok);
            if (!paramok || (strlen(pAid) != 0 && strlen(pAid) != LPA_AID_LENGTH * 2))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pAid:%s", pAid);

            uint8_t pData[LPA_AID_LENGTH] = {
                0,
            };
            uint8_t bIccid = 0;
            uint8_t refreshFlag = 1;
            if (strlen(pIccid) != 0)
            {
                cfwHexStrToBytes((const void *)pIccid, LPA_ICCID_LENGTH * 2, pData);
                bIccid = 1;
            }
            else
            {
                cfwHexStrToBytes((const void *)pAid, LPA_AID_LENGTH * 2, pData);
            }

            if (luid_DisableProfile((void *)cmd, bIccid, (uint8_t *)pData, refreshFlag, _handleDisableProfile, nSim) == LPA_SUCCESS)
            {
                RETURN_FOR_ASYNC();
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        case LPA_FUNC_DELETE_PROFILE:
        {
            const char *pIccid = atParamStr(cmd->params[1], &paramok);
            if (!paramok || (strlen(pIccid) != 0 && strlen(pIccid) != LPA_ICCID_LENGTH * 2))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pIccid:%s", pIccid);

            const char *pAid = atParamStr(cmd->params[2], &paramok);
            if (!paramok || (strlen(pAid) != 0 && strlen(pAid) != LPA_AID_LENGTH * 2))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pAid:%s", pAid);

            char pData[LPA_AID_LENGTH] = {
                0,
            };
            uint8_t bIccid = 0;
            if (strlen(pIccid) != 0)
            {
                cfwHexStrToBytes((const void *)pIccid, LPA_ICCID_LENGTH * 2, pData);
                bIccid = 1;
            }
            else
            {
                cfwHexStrToBytes((const void *)pAid, LPA_AID_LENGTH * 2, pData);
            }
            luid_DeleteProfile((void *)cmd, bIccid, (uint8_t *)pData, _handleDeleteProfile, nSim);

            RETURN_FOR_ASYNC();
        }
        case LPA_FUNC_EUICC_MEMORY_RESET:
        {
            luid_eUICCMemoryReset((void *)cmd, 1, 1, 1, _handleEUICCMemoryReset, nSim);
            RETURN_FOR_ASYNC();
        }
        case LPA_FUNC_GET_EID:
        {
            luid_GetEID((void *)cmd, _handleGetEID, nSim);
            RETURN_FOR_ASYNC();
        }
        case LPA_FUNC_SET_NICKNAME:
        {
            const char *pIccid = atParamStr(cmd->params[1], &paramok);
            if (!paramok || (strlen(pIccid) != 0 && strlen(pIccid) != LPA_ICCID_LENGTH * 2))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pIccid:%s", pIccid);
            char iccid[LPA_ICCID_LENGTH] = {
                0,
            };
            cfwHexStrToBytes((const void *)pIccid, LPA_ICCID_LENGTH * 2, iccid);

            const char *pNickName = atParamStr(cmd->params[2], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            int nStrlen = 0;
            char *pProfileNickname = (char *)mlConvertStr((const void *)pNickName, strlen(pNickName), ML_CP936, ML_UTF8, &nStrlen);

            luid_SetNickname((void *)cmd, (void *)iccid, pProfileNickname, _handleSetNickname, nSim);
            free(pProfileNickname);
            RETURN_FOR_ASYNC();
        }
        case LPA_FUNC_START_GUARDIAN:
        {
            if (0 == lpa_guardian_start((void *)cmd, _handleGuardian, nSim))
                RETURN_FOR_ASYNC();
            else
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        case LPA_FUNC_APP_ID:
        {
            const char *pAppId = atParamStr(cmd->params[1], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pAppId:%s", pAppId);
            lpa_guardian_set_app_id(pAppId);
            RETURN_OK(cmd->engine);
        }
        case LPA_FUNC_APP_SECRET:
        {
            const char *pAppSecret = atParamStr(cmd->params[1], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pAppSecret:%s", pAppSecret);
            lpa_guardian_set_app_secret(pAppSecret);
            RETURN_OK(cmd->engine);
        }
        case LPA_FUNC_APP_URL:
        {
            const char *pAppUrl = atParamStr(cmd->params[1], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pAppUrl:%s", pAppUrl);
            lpa_guardian_set_app_url(pAppUrl);
            RETURN_OK(cmd->engine);
        }
        default:
            OSI_LOGI(0, "Not Handle!!");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        }
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
#endif
