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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('A', 'S', 'M', 'S')
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
#include "at_cmd_ss.h"
#include "quec_atcmd_sim.h"
#include "quec_urc.h"
#include "ql_api_sim.h"

#define CRSM_READ_BINARY 176
#define CRSM_READ_RECORD 178
#define CRSM_GET_RESPONSE 192
#define CRSM_UPDATE_BINARY 214
#define CRSM_UPDATE_RECORD 220
#define CRSM_STATUS 242

static const osiValueStrMap_t gCpinStr[] = {
    {CFW_STY_AUTH_READY, "READY"},
    {CFW_STY_AUTH_PIN1_READY, "PIN1 READY"},
    {CFW_STY_AUTH_SIMPIN, "SIM PIN"},
    {CFW_STY_AUTH_SIMPUK, "SIM PUK"},
    {CFW_STY_AUTH_PHONE_TO_SIMPIN, "PH-SIM PIN"},
    {CFW_STY_AUTH_PHONE_TO_FIRST_SIMPIN, "PH-FSIM PIN"},
    {CFW_STY_AUTH_PHONE_TO_FIRST_SIMPUK, "PH-FSIM PUK"},
    {CFW_STY_AUTH_SIMPIN2, "SIM PIN2"},
    {CFW_STY_AUTH_SIMPUK2, "SIM PUK2"},
    {CFW_STY_AUTH_NETWORKPIN, "PH-NET PIN"},
    {CFW_STY_AUTH_NETWORKPUK, "PH-NET PUK"},
    {CFW_STY_AUTH_NETWORK_SUBSETPIN, "PH-NETSUB PIN"},
    {CFW_STY_AUTHNETWORK_SUBSETPUK, "PH-NETSUB PUK"},
    {CFW_STY_AUTH_PROVIDERPIN, "PH-SP PIN"},
    {CFW_STY_AUTH_PROVIDERPUK, "PH-SP PUK"},
    {CFW_STY_AUTH_CORPORATEPIN, "PH-CORP PIN"},
    {CFW_STY_AUTH_CORPORATEPUK, "PH-CORP PUK"},
    {CFW_STY_AUTH_NOSIM, "NO SIM"},
    {CFW_STY_AUTH_PIN1BLOCK, "PIN1 BLOCK"},
    {CFW_STY_AUTH_PIN2BLOCK, "PIN2 BLOCK"},
    {CFW_STY_AUTH_PIN1_DISABLE, "PIN1 DISABLE"},
    {CFW_STY_AUTH_SIM_PRESENT, "SIM PRESENT"},
    {0, NULL},
};

static uint8_t gLstStatusRec; // 1--->pin1 or puk1,2--->pin2 or puk2

// for get auth  response ,different cmd call wait same event
static uint8_t AT_security_currentCMD[CFW_SIM_COUNT] = {
    0x00,
};
/*
********************************************************************************
* because CPIN CPIN2,CPIN2 will call some CFW interface,and wait same event    *
* so following definition is to seperate it                                    *
* ERR_CME_SIM_UNKNOW                                                           *
********************************************************************************
*/
#define CPIN 1
#define CPIN2 2
#define CPINCEXE 3
#define CPINCREAD 4
#define CLCK 5
#define CPWD 6

#ifdef PORTING_ON_GOING
#include "at.h"
#include "at_cmd_sim.h"
#include "csw.h"

uint16_t ascii2hex(const char *pInput, uint16_t length, uint8_t *pOutput);
uint8_t SUL_hex2ascii(uint8_t *pInput, uint16_t nInputLen, uint8_t *pOutput);
// define trigger debug inform output by trace:   1 ---> debug;   0 ---> colse debug  //add by wxd
#define SIM_DEBUG 0

// CPIN related definition.actually this  is enough for all the code
#define PIN1CODEMAX 12
#define PINNEEDPUK 3

// status of PUK1
uint8_t g_Pin1PukStauts[CFW_SIM_COUNT] = {
    0,
};
static uint8_t g_Pin1ErrorTimes[CFW_SIM_COUNT] = {
    0,
};
extern uint32_t CFW_GetStackSimFileID(uint16_t n3GppFileID, uint16_t EFPath, CFW_SIM_ID nSimID);

// #define PUK1CODEMAX 12
#define SIM_UTI 6

// CLCK related
#define U16_SWAP(x) ((((x)&0xFF) << 8) | (((x) >> 8) & 0xFF))
#define FAC_MAX_NUM 6
#define CLCK_PWD_MAX 12
// static uint8_t clckTestSTR[] = "+CLCK: (\"PS\",\"PF\",\"SC\",\"FD\",\"PN\",\"PU\",\"PP\",\"PC\")";

// CPWD related
/*
******************************************************************************
*   #define CFW_STY_FAC_TYPE_SC  0x5343
*   #define CFW_STY_FAC_TYPE_PS  0x5053
*   #define CFW_STY_FAC_TYPE_P2  0x5032
*   #define CFW_STY_FAC_TYPE_PF  0x5046
*   #define CFW_STY_FAC_TYPE_PN  0x504E
*   #define CFW_STY_FAC_TYPE_PU  0x5055
*   #define CFW_STY_FAC_TYPE_PP  0x5050
*   #define CFW_STY_FAC_TYPE_PC  0x5043
*   #define CFW_STY_FAC_TYPE_FD  0x4644
*
*   +CPWD: ("PS",4),("PF",8),("SC",8),("AO",4),("OI",4),("OX",4),("AI",4),("IR",4),(
*   "AB",4),("AG",4),("AC",4),("PN",8),("PU",8),("PP",8),("PC",8),("P2",8)
*****************************************************************************
*/

// static uint8_t cpwdTestSTR[] = "+CPWD: (\"PS\",4),(\"PF\",8),(\"SC\",8),(\"PN\",8),(\"PU\",8),(\"P2\",8),(\"PC\",8),(\"FD\",8)";
// *************time stamp for asyn event timeout in ATM*************//
static uint32_t u_gCurrentCmdStamp[CFW_SIM_COUNT] = {
    0x00,
};
/*
******************************************************************************************************
#define CFW_STY_AUTH_READY         0  //Phone is not waiting for any password.
#define CFW_STY_AUTH_PIN1_READY        1  //Phone is not waiting for PIN1 password.
#define CFW_STY_AUTH_SIMPIN        2  //Phone is waiting for the SIM Personal Identification Number (PIN)
#define CFW_STY_AUTH_SIMPUK        3  //Phone is waiting for the SIM Personal Unlocking Key (PUK).
#define CFW_STY_AUTH_PHONE_TO_SIMPIN       4  //Phone is waiting for the phone-to-SIM card password.
#define CFW_STY_AUTH_PHONE_TO_FIRST_SIMPIN 5  //Phone is waiting for the phone-to-first-SIM card PIN.
#define CFW_STY_AUTH_PHONE_TO_FIRST_SIMPUK 6  //Phone is waiting for the phone-to-first-SIM card PUK.
#define CFW_STY_AUTH_SIMPIN2           7  //Phone is waiting for the SIM PIN2.
#define CFW_STY_AUTH_SIMPUK2  ----NO         8  //Phone is waiting for the SIM PUK2.
#define CFW_STY_AUTH_NETWORKPIN        9  //Phone is waiting for the network personalization PIN.
#define CFW_STY_AUTH_NETWORKPUK        10 //Phone is waiting for the network personalization PUK.
#define CFW_STY_AUTH_NETWORK_SUBSETPIN     11 //Phone is waiting for the network subset personalization PIN.
#define CFW_STY_AUTHNETWORK_SUBSETPUK      12 //Phone is waiting for the network subset personalization PUK.
#define CFW_STY_AUTH_PROVIDERPIN       13 //Phone is waiting for the service provider personalization PIN.
#define CFW_STY_AUTH_PROVIDERPUK       14 //Phone is waiting for the service provider personalization PUK.
#define CFW_STY_AUTH_CORPORATEPIN      15 //Phone is waiting for the corporate personalization PIN.
#define CFW_STY_AUTH_CORPORATEPUK      16 //Phone is waiting for the corporate personalization PUK.
#define CFW_STY_AUTH_NOSIM         17 //No SIM inserted.
#define CFW_STY_AUTH_PIN1BLOCK         18
#define CFW_STY_AUTH_PIN2BLOCK         19
#define CFW_STY_AUTH_PIN1_DISABLE      20
#define CFW_STY_AUTH_SIM_PRESENT       21
#define CFW_STY_AUTH_SIM_END           22
********************************************************************************************************
*/
/************CFW_SimGetAuthenticationStatus string********************/
static uint8_t cpinasynSTR[23][20] =
    {
        "+CPIN:READY",
        "+CPIN:PIN1 READY",
        "+CPIN:SIM PIN",
        "+CPIN:SIM PUK",
        "+CPIN:PH-SIM PIN", // no  PUK supported!!
        "+CPIN:PH-FSIM PIN",
        "+CPIN:PH-FSIM PUK",
        "+CPIN:SIM PIN2",
        "+CPIN:SIM PUK2",
        "+CPIN:PH-NET PIN",
        "+CPIN:PH-NET PUK",
        "+CPIN:PH-NETSUB PIN",
        "+CPIN:PH-NETSUB PUK",
        "+CPIN:PH-SP PIN",
        "+CPIN:PH-SP PUK",
        "+CPIN:PH-CORP PIN",
        "+CPIN:PH-CORP PUK",
        "+CPIN:NO SIM",
        "+CPIN:PIN1 BLOCK",
        "+CPIN:PIN2 BLOCK",
        "+CPIN:PIN1 DISABLE",
        "+CPIN:SIM PRESENT",
};

// for get auth  response ,different cmd call wait same event
static uint8_t AT_security_currentCMD[CFW_SIM_COUNT] = {
    0x00,
};

uint8_t gAtSimID = 0;

#ifdef AT_SUPPORT_BT
uint8_t gAtBTFlag = 0;
#endif

/************************cmd realization***************************************/
uint32_t AT_CAMM_STRtoUINT32(uint8_t *pData)
{
    uint32_t iLen = 0;
    uint32_t res = 0;
    uint8_t *data = pData;
    uint8_t iCnt = 0;
    uint8_t temp;
    uint8_t t;

    iLen = AT_StrLen(pData);
    if (iLen != 6)
        return ERR_AT_CME_PARAM_INVALID;

    for (iCnt = 0; iCnt < iLen; iCnt++)
    {
        temp = *(data + iCnt);

        if (temp <= 57 && temp >= 48) // '0'-'9'
        {
            t = temp - 48;
            res |= (((uint32_t)t) << ((5 - iCnt) * 4));
        }
        else if ((temp <= 102 && temp >= 97) || (temp <= 70 && temp >= 65)) // 'a'-'f','A'-'F'
        {
            if (temp <= 102 && temp >= 97)
                temp -= 32;

            t = 10 + temp - 65;

            res |= (((uint32_t)t) << ((5 - iCnt) * 4));
        }
        else
        {
            return ERR_AT_CME_PARAM_INVALID;
        }
    }
    OSI_LOGI(0x1000469a, "res = %lu", res);
    return res;
}

// add for crsm begin

void AT_SIM_Result_OK(uint32_t uReturnValue,
                      uint32_t uResultCode, uint8_t *pBuffer, uint8_t nDLCI)
{
    PAT_CMD_RESULT pResult = AT_CreateRC(uReturnValue, uResultCode,
                                         CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME,
                                         0, pBuffer, AT_StrLen(pBuffer), nDLCI);
    if (pResult != NULL)
    {
        AT_Notify2ATM(pResult, nDLCI);
        AT_FREE(pResult);
    }
}

void AT_SIM_Result_Err(uint32_t uErrorCode, uint8_t nDLCI)
{
    PAT_CMD_RESULT pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                                         uErrorCode, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0, nDLCI);
    if (pResult != NULL)
    {
        AT_Notify2ATM(pResult, nDLCI);
        AT_FREE(pResult);
    }
}

uint32_t gCrsmCommandDataLen[CFW_SIM_COUNT] = {
    0x00,
};
uint32_t ATGetRealFileID(uint16_t nFileID, uint16_t nCurrentIndex, CFW_SIM_ID nSimID)
{
    if ((API_USIM_EF_GB_ADN == nFileID) || (API_USIM_EF_ADN == nFileID))
    {
        if (nCurrentIndex > CFW_GetPBKRecordNum(nSimID))
            return nFileID + 1;
        else
            return nFileID;
    }
    else
    {
        return nFileID;
    }
}

uint32_t ATGetRealCurrIndex(uint16_t nFileID, uint16_t nCurrentIndex, CFW_SIM_ID nSimID)
{
    if ((API_USIM_EF_GB_ADN == nFileID) || (API_USIM_EF_ADN == nFileID))
    {
        if (nCurrentIndex > CFW_GetPBKRecordNum(nSimID))
            return nCurrentIndex - CFW_GetPBKRecordNum(nSimID);
        else
            return nCurrentIndex;
    }
    else
    {
        return nCurrentIndex;
    }
}
#endif

static uint32_t _parseSw1Sw2ErrorCode(uint32_t errorCode)
{
    OSI_LOGI(0, "_parseSw1Sw2ErrorCode errorCode: %x", errorCode);
    switch (errorCode)
    {
    case ERR_CME_SIM_VERIFY_FAIL:
        return ERR_AT_CME_SIM_VERIFY_FAIL;
    case ERR_CME_SIM_UNBLOCK_FAIL:
        return ERR_AT_CME_SIM_UNBLOCK_FAIL;
    case ERR_CME_SIM_CONDITION_NO_FULLFILLED:
        return ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED;
    case ERR_CME_SIM_UNBLOCK_FAIL_NO_LEFT:
        return ERR_AT_CME_SIM_UNBLOCK_FAIL_NO_LEFT;
    case ERR_CME_SIM_VERIFY_FAIL_NO_LEFT:
        return ERR_AT_CME_SIM_VERIFY_FAIL_NO_LEFT;
    case ERR_CME_SIM_INVALID_PARAMETER:
        return ERR_AT_CME_SIM_INVALID_PARAMETER;
    case ERR_CME_SIM_UNKNOW_COMMAND:
        return ERR_AT_CME_SIM_UNKNOW_COMMAND;
    case ERR_CME_SIM_WRONG_CLASS:
        return ERR_AT_CME_SIM_WRONG_CLASS;
    case ERR_CME_SIM_TECHNICAL_PROBLEM:
        return ERR_AT_CME_SIM_TECHNICAL_PROBLEM;
    case ERR_CME_SIM_CHV_NEED_UNBLOCK:
        return ERR_AT_CME_SIM_CHV_NEED_UNBLOCK;
    case ERR_CME_SIM_NOEF_SELECTED:
        return ERR_AT_CME_SIM_NOEF_SELECTED;
    case ERR_CME_SIM_FILE_UNMATCH_COMMAND:
        return ERR_AT_CME_SIM_FILE_UNMATCH_COMMAND;
    case ERR_CME_SIM_CONTRADICTION_CHV:
        return ERR_AT_CME_SIM_CONTRADICTION_CHV;
    case ERR_CME_SIM_CONTRADICTION_INVALIDATION:
        return ERR_AT_CME_SIM_CONTRADICTION_INVALIDATION;
    case ERR_CME_SIM_MAXVALUE_REACHED:
        return ERR_AT_CME_SIM_MAXVALUE_REACHED;
    case ERR_CME_SIM_PATTERN_NOT_FOUND:
        return ERR_AT_CME_SIM_PATTERN_NOT_FOUND;
    case ERR_CME_SIM_FILEID_NOT_FOUND:
        return ERR_AT_CME_SIM_FILEID_NOT_FOUND;
    case ERR_CME_SIM_STK_BUSY:
        return ERR_AT_CME_SIM_STK_BUSY;
    case ERR_CME_SIM_UNKNOW:
        return ERR_AT_CME_SIM_UNKNOW;
    case ERR_CME_SIM_PROFILE_ERROR:
        return ERR_AT_CME_SIM_PROFILE_ERROR;
    default:
        return ERR_AT_CME_SIM_UNKNOW;
    }
}

void atCfwSimHotPlugProc(osiEvent_t *event)
{
    int num;
    bool connect;
    char rsp[15] = {0};
    num = event->param1;
    connect = event->param2;
    OSI_LOGI(0, "num = %d, connect = %d ", num, connect);
    
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    if (QUEC_ENABLE == quec_at_current_setting.sim_stat_urc)
    {
        char q_simstate_urc_buf[64] = {0};
        sprintf(q_simstate_urc_buf, "+QSIMSTAT: %d,%d", quec_at_current_setting.sim_stat_urc, connect);
        quec_urc_send(NULL, q_simstate_urc_buf, strlen(q_simstate_urc_buf), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, false);
    }
    if (false == connect)
    {
        quec_cfw_init_done = 0;
        
        CFW_EVENT no_sim_event = {0};
        no_sim_event.nEventId = EV_CFW_INIT_IND;
        no_sim_event.nType = CFW_INIT_STATUS_NO_SIM;
        quec_event_send(atEngineGetThreadId(), (quec_event_t *)&no_sim_event);
    }
#endif
    sprintf(rsp, "+SIMSTAT: %d,%d", num, connect);
    if (num == 1)
    {
#ifdef CONFIG_BOARD_SUPPORT_SIM1_DETECT
        atCmdRespSimUrcText(0, rsp);
#endif
    }
    else
    {
#ifdef CONFIG_BOARD_SUPPORT_SIM2_DETECT
        atCmdRespSimUrcText(1, rsp);
#endif
    }
    osiBootMode_t boot_mode = osiGetBootMode();
    if (boot_mode != OSI_BOOTMODE_CALIB)
        ipc_notify_sim_detect(num, connect);
    free(event);
}

void atCfwSimHotPlugCB(int num, bool connect)
{
    osiEvent_t *ev = malloc(sizeof(osiEvent_t));
    ev->param1 = num;
    ev->param2 = connect;

    osiThreadCallback(atEngineGetThreadId(), (osiCallback_t)atCfwSimHotPlugProc, ev);
}

typedef struct
{
    uint32_t iCommand;
    uint32_t iP1;
    uint32_t iP2;
    uint32_t iP3;
    uint16_t nFileIDs;
} CRSMAsyncCtx_t;
#define SIM_FILE_STRUCTURE_BINARY 0
#define SIM_FILE_STRUCTURE_LINEAR 1
#define SIM_FILE_STRUCTURE_CYCLIC 3
#define SIM_FILE_TYPE_EF 4
#define USIM_FILE_STRUCTURE_BINARY 1
#define USIM_FILE_STRUCTURE_LINEAR 2
#define USIM_FILE_STRUCTURE_CYCLIC 6

static void CRSM_rspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nType != 0)
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);

    if (cfw_event->nEventId == EV_CFW_SIM_GET_FILE_STATUS_RSP)
    {
        uint32_t nRet = 0;
        uint8_t pOutput[256] = {0};
        const char *sData = NULL;
        bool paramok = true;

        uint8_t nSim = atCmdGetSim(cmd->engine);
        CRSMAsyncCtx_t *async = (CRSMAsyncCtx_t *)cmd->async_ctx;

        if ((async->iCommand & 0xFE) != CRSM_GET_RESPONSE)
        {
            CFW_UsimEfStatus gUsim = {0};
            uint8_t iFileStructure = 0xFF;
            uint16_t iFileLength = 0, iFileOffset = 0;
            uint8_t iFileRecordLength = 0;

            atMemFreeLater((void *)cfw_event->nParam1);

            if (CFW_GetSimType(nSim) == false) //SIM Card
            {
                if (((uint8_t *)cfw_event->nParam1)[6] != SIM_FILE_TYPE_EF)
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
                }

                iFileStructure = ((uint8_t *)cfw_event->nParam1)[13];
                if ((iFileStructure != SIM_FILE_STRUCTURE_BINARY) && (iFileStructure != SIM_FILE_STRUCTURE_LINEAR) && (iFileStructure != SIM_FILE_STRUCTURE_CYCLIC))
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
                }

                if (iFileStructure == SIM_FILE_STRUCTURE_BINARY)
                {
                    iFileLength = ((((uint16_t)((uint8_t *)cfw_event->nParam1)[2]) << 8) | (((uint8_t *)cfw_event->nParam1)[3]));
                }
                else
                {
                    iFileRecordLength = ((uint8_t *)cfw_event->nParam1)[14]; //mandatory
                }
            }
            else //USIM Card
            {
                uint16_t nDataLength = (uint16_t)(cfw_event->nParam2 >> 16);

                uint32_t iRet = CFW_UsimDecodeEFFcpV2((uint8_t *)cfw_event->nParam1, nDataLength, &gUsim);
                if (ERR_SUCCESS != iRet)
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
                }

                iFileStructure = gUsim.efStructure;
                if ((iFileStructure != USIM_FILE_STRUCTURE_BINARY) && (iFileStructure != USIM_FILE_STRUCTURE_LINEAR) && (iFileStructure != USIM_FILE_STRUCTURE_CYCLIC))
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
                }

                if (iFileStructure == USIM_FILE_STRUCTURE_BINARY)
                {
                    iFileLength = gUsim.fileSize;
                }
                else
                {
                    iFileRecordLength = gUsim.recordLength; //mandatory
                }
                (iFileStructure == USIM_FILE_STRUCTURE_BINARY) ? (iFileStructure = SIM_FILE_STRUCTURE_BINARY) : (iFileStructure = iFileStructure);
            }

            cmd->uti = cfwRequestUTI((osiEventCallback_t)CRSM_rspCB, cmd);
            switch ((async->iCommand) & 0xFE)
            {
            case CRSM_READ_BINARY:
                if (iFileStructure != SIM_FILE_STRUCTURE_BINARY)
                {
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if ((CFW_GetSimType(nSim) == true) && (((async->iP1) >> 7) == 1)) //USIM Card && SFI
                {
                    iFileOffset = async->iP2;
                }
                else
                {
                    iFileOffset = (((async->iP1) << 8)) + async->iP2;
                }

                if ((async->iP3 > iFileLength) || ((iFileOffset + async->iP3) > iFileLength))
                {
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                nRet = CFW_SimReadBinary(async->nFileIDs, iFileOffset, async->iP3, cmd->uti, nSim);
                break;
            case CRSM_UPDATE_BINARY:
                if (iFileStructure != SIM_FILE_STRUCTURE_BINARY)
                {
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if ((CFW_GetSimType(nSim) == true) && (((async->iP1) >> 7) == 1)) //USIM Card && SFI
                {
                    iFileOffset = async->iP2;
                }
                else
                {
                    iFileOffset = (((async->iP1) << 8)) + async->iP2;
                }
                if ((async->iP3 > iFileLength) || ((iFileOffset + async->iP3) > iFileLength))
                {
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                sData = atParamDefStr(cmd->params[5], "", &paramok);
                cfwHexStrToBytes(sData, 2 * (async->iP3), pOutput);
                nRet = CFW_SimUpdateBinary(async->nFileIDs, iFileOffset, pOutput, async->iP3, cmd->uti, nSim);
                break;
            case CRSM_READ_RECORD:
                if (iFileStructure == SIM_FILE_STRUCTURE_BINARY)
                {
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (async->iP3 > iFileRecordLength)
                {
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                nRet = CFW_SimReadRecordAllParam(async->nFileIDs, async->iP1, async->iP2, async->iP3, cmd->uti, nSim);
                break;
            case CRSM_UPDATE_RECORD:
                if (iFileStructure == SIM_FILE_STRUCTURE_BINARY)
                {
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (async->iP3 > iFileRecordLength)
                {
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                sData = atParamDefStr(cmd->params[5], "", &paramok);
                cfwHexStrToBytes(sData, 2 * (async->iP3), pOutput);
                nRet = CFW_SimUpdateRecordAllParam(async->nFileIDs, async->iP1, async->iP3, async->iP2, pOutput, cmd->uti, nSim);
                break;
            }
            if (0 != nRet)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            RETURN_FOR_ASYNC();
        }
    }
    if (cfw_event->nEventId == EV_CFW_SIM_GET_DF_STATUS_RSP)
    {
        uint8_t *pData = (uint8_t *)cfw_event->nParam1;
        uint8_t length = (uint8_t)cfw_event->nParam2;

        if (length != 0)
        {
            atMemFreeLater((void *)cfw_event->nParam1);
        }

        if (cmd->type == 0)
        {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
			const char *prefix = "+CRSM: 144,0,\"";
			uint16_t size = (length << 1) + strlen(prefix) + 2;
#else
            const char *prefix = "+CRSM: 144,0,";
            uint16_t size = (length << 1) + strlen(prefix) + 1;
#endif
            OSI_LOGI(0, "length = %d, length << 1 = %d, size = %d", length, length << 1, size);

            char *pResponse = malloc(size);
            if (pResponse == 0)
            {
                RETURN_CME_CFW_ERR(cmd->engine, ERR_AT_CME_SIM_FAILURE);
            }
#ifdef CONFIG_QUEC_PROJECT_FEATURE
			memset(pResponse, 0, size);
#endif
            uint16_t i = sprintf(pResponse, prefix);

            size = cfwBytesToHexStr(pData, length, pResponse + i);
            if (size == 0)
            {
                free(pResponse);
                RETURN_CME_CFW_ERR(cmd->engine, ERR_AT_CME_SIM_FAILURE);
            }
#ifdef CONFIG_QUEC_PROJECT_FEATURE
			sprintf(pResponse + i + size, "\"");
#else
            pResponse[i + size] = 0x00;
#endif
            atCmdRespInfoText(cmd->engine, pResponse);
            atCmdRespOK(cmd->engine);
            free(pResponse);
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, cfw_event->nParam1);
        }
        return;
    }
    // +CRSM: <sw1>,<sw2>[,<response>]
    uint16_t bytes = ((cfw_event->nEventId == EV_CFW_SIM_READ_BINARY_RSP) || (cfw_event->nEventId == EV_CFW_SIM_READ_RECORD_RSP) || (cfw_event->nEventId == EV_CFW_SIM_GET_FILE_STATUS_RSP))
                         ? ((cfw_event->nParam2 >> 16) & 0xffff)
                         : cfw_event->nParam2;
    if ((cfw_event->nEventId == EV_CFW_SIM_UPDATE_RECORD_RSP) ||
        (cfw_event->nEventId == EV_CFW_SIM_UPDATE_BINARY_RSP) ||
        cfw_event->nParam1 == 0 || bytes == 0)
    {
        atCmdRespInfoText(cmd->engine, "+CRSM: 144,0");
        RETURN_OK(cmd->engine);
    }

    atMemFreeLater((void *)cfw_event->nParam1);
    char *rsp = malloc(bytes * 2 + 32);
    if (rsp == NULL)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

    char *prsp = rsp;
    prsp += sprintf(prsp, "+CRSM: 144,0,\"");
    prsp += cfwBytesToHexStr((void *)cfw_event->nParam1, bytes, prsp);
    prsp += sprintf(prsp, "\"");
    atCmdRespInfoText(cmd->engine, rsp);
    atCmdRespOK(cmd->engine);
    free(rsp);
}

// 8.18 Restricted SIM access
extern bool CFW_GetSimType(CFW_SIM_ID nSimID);
void atCmdHandleCRSM(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        // +CRSM=<command>[,<fileid>[,<P1>,<P2>,<P3>[,<data>[,<pathid>]]]]
        uint32_t uiP1 = 0;
        uint32_t uiP2 = 0;
        uint32_t uiP3 = 0;
        uint32_t nEFID = 0;
        uint8_t nEFPath[5] = {0};
        const char *sData = NULL;
        bool paramok = true;
        char *nFullPathStr = NULL;
        uint8_t uStrLen = 0;
        uint16_t nFileID = 0;

        if ((cmd->param_count > 7) || (cmd->param_count < 1) || (cmd->param_count == 3 || cmd->param_count == 4)) // if param count equal to 3 or 4,then the fifth param must be filled
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        uint32_t uCommand = atParamUint(cmd->params[0], &paramok);

        if (cmd->param_count > 1)
        {
            if ((uCommand != CRSM_GET_RESPONSE) && (cmd->param_count == 2)) // except STATUS and GET RESPONSE, <P1>,<P2>,<P3> is needed.
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            nEFID = atParamUint(cmd->params[1], &paramok);
            if (paramok == false)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            nEFID = nEFID & 0xFFFF;

            if (cmd->param_count > 2)
            {
                uiP1 = atParamUint(cmd->params[2], &paramok);
                uiP2 = atParamUint(cmd->params[3], &paramok);
                uiP3 = atParamUint(cmd->params[4], &paramok);
                if (!paramok)
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }

                if (cmd->param_count > 5)
                {
                    sData = atParamDefStr(cmd->params[5], "", &paramok);
                    if (!paramok) /* get uIndex failed */
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    if (((uCommand & 0xFE) == CRSM_UPDATE_BINARY) || ((uCommand & 0xFE) == CRSM_UPDATE_RECORD))
                    {
                        if (strlen(sData) != 2 * uiP3) //2*hex = string
                        {
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                        }
                    }
                    //cfwHexStrToBytes(sData, 2 * uiP3, pOutput);
                }
                if (cmd->param_count > 6)
                {
                    nFullPathStr = (char *)atParamDefStr(cmd->params[6], "", &paramok);
                    uStrLen = (uint8_t)strlen(nFullPathStr);
                    if ((!paramok) || ((uStrLen != 0) && (uStrLen != 4) && (uStrLen != 8)))
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }

                    //get uiPath
                    cfwHexStrToBytes((uint8_t *)nFullPathStr, uStrLen, nEFPath);
                }
            }

            if (cmd->param_count > 6)
            {
                nFileID = CFW_GetStackSimFileIDByPath_V2(nEFID, nEFPath, uStrLen / 2, nSim);
                OSI_LOGI(0, "uCommand = %d, nEFPath = %s, nEFID = %x, nFileID = %x\n", uCommand, nFullPathStr, nEFID, nFileID);
            }
            else
            {
                nFileID = CFW_GetStackSimFileID(nEFID, 0, nSim);
                OSI_LOGI(0, "uCommand = %d, nEFID = %x, nFileID = %x\n", uCommand, nEFID, nFileID);
            }

            if (nFileID == 0xFFFF)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        else
        {
            if (uCommand == CRSM_STATUS)
            {
                if (CFW_GetSimType(nSim) == true)
                {
                    nFileID = 2;
                }
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        OSI_LOGI(0, "CRSM path/%04x id/%04x file/%04x", nEFPath, nEFID, nFileID);

        if (((uCommand & 0xFE) == CRSM_UPDATE_BINARY) || ((uCommand & 0xFE) == CRSM_UPDATE_RECORD))
        {
            if (cmd->param_count < 6)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        CRSMAsyncCtx_t *async = (CRSMAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;

        async->iCommand = uCommand;
        async->iP1 = uiP1;
        async->iP2 = uiP2;
        async->iP3 = uiP3;
        async->nFileIDs = nFileID;

        uint32_t nRet;
        cmd->uti = cfwRequestUTI((osiEventCallback_t)CRSM_rspCB, cmd);
        switch (uCommand & 0xFE)
        {
        case CRSM_READ_BINARY:
        case CRSM_UPDATE_BINARY:
        case CRSM_READ_RECORD:
        case CRSM_UPDATE_RECORD:
        case CRSM_GET_RESPONSE:
            nRet = CFW_SimGetFileStatus(nFileID, cmd->uti, nSim);
            break;

        case CRSM_STATUS:
            nRet = CFW_SimGetDFStatus((uint8_t)nFileID, cmd->uti, nSim);
            break;

        default:
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (0 != nRet)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        RETURN_FOR_ASYNC();
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
typedef struct
{
    uint32_t nStartIndex;
    uint32_t nCount;
    uint32_t nCurrentIndex;
    uint32_t nLen;
    uint32_t nFileID;
} CRSMLAsyncCtx_t;

static void CRSML_rspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    CRSMLAsyncCtx_t *async = (CRSMLAsyncCtx_t *)cmd->async_ctx;
    uint8_t nSim = cfw_event->nFlag;

    if (EV_CFW_SIM_GET_FILE_STATUS_RSP == cfw_event->nEventId)
    {
        if (cfw_event->nType == 0)
        {
            CFW_UsimEfStatus gUsim = {0};
            atMemFreeLater((void *)cfw_event->nParam1);

            if (CFW_GetSimType(nSim) == false)
            {
                async->nLen = ((uint8_t *)cfw_event->nParam1)[14];
            }
            else
            {
                uint16_t nDataLength = (uint16_t)(cfw_event->nParam2 >> 16);

                uint32_t iRet = CFW_UsimDecodeEFFcpV2((uint8_t *)cfw_event->nParam1, nDataLength, &gUsim);
                if (ERR_SUCCESS != iRet)
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                async->nLen = gUsim.recordLength;
            }

            OSI_LOGI(0, "async->nLen = %d, nDataLength = %d", async->nLen, async->nLen);

            uint16_t nFileID = CFW_GetStackSimFileID(async->nFileID & 0XFFFF, async->nFileID >> 16, nSim);
            if (nFileID == 0xFFFF)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            cmd->uti = cfwRequestUTI((osiEventCallback_t)CRSML_rspCB, cmd);
            uint32_t nRet = CFW_SimReadRecordWithLen(nFileID, async->nCurrentIndex, async->nLen, cmd->uti, nSim);
            if (ERR_SUCCESS != nRet)
            {
                OSI_LOGI(0, "nRet = 0x%x", nRet);
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_FAILURE);
        }
    }
    else if (EV_CFW_SIM_READ_RECORD_RSP == cfw_event->nEventId)
    {
        if (cfw_event->nType == 0)
        {
            char *pRetString = NULL;
            char *pAsciiRetString = NULL;
            uint16_t nAsciiLen = 0;
            uint16_t nRetStrLen = 0;

            nAsciiLen = ((uint16_t)((cfw_event->nParam2) >> 16) * 2);
            nRetStrLen = nAsciiLen + 16;
            pRetString = (char *)malloc(nRetStrLen);
            if (NULL == pRetString)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            }

            pAsciiRetString = (char *)malloc(nAsciiLen + 2);
            if (NULL == pAsciiRetString)
            {
                free(pRetString);
                pRetString = NULL;
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            }

            memset(pRetString, 0, nRetStrLen);
            memset(pAsciiRetString, 0, nAsciiLen + 2);
            cfwBytesToHexStr((uint8_t *)(cfw_event->nParam1), cfw_event->nParam2 >> 16, pAsciiRetString);

            sprintf(pRetString, "+CRSML:144,%d,%s", (uint8_t)async->nCurrentIndex, pAsciiRetString);
            //strcat(pRetString, pAsciiRetString);

            atMemFreeLater((void *)cfw_event->nParam1);

            if (async->nCurrentIndex < (async->nStartIndex + async->nCount - 1))
            {
                atCmdRespInfoText(cmd->engine, pRetString);

                async->nCurrentIndex++;
                uint16_t nFileID = CFW_GetStackSimFileID(async->nFileID & 0XFFFF, async->nFileID >> 16, nSim);
                if (nFileID == 0xFFFF)
                {
                    free(pRetString);
                    free(pAsciiRetString);
                    pAsciiRetString = NULL;
                    pRetString = NULL;
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }

                cmd->uti = cfwRequestUTI((osiEventCallback_t)CRSML_rspCB, cmd);
                uint32_t nRet = CFW_SimReadRecordWithLen(nFileID, async->nCurrentIndex, async->nLen, cmd->uti, nSim);
                if (ERR_SUCCESS != nRet)
                {
                    OSI_LOGI(0, "nRet = 0x%x", nRet);
                    free(pRetString);
                    free(pAsciiRetString);
                    pAsciiRetString = NULL;
                    pRetString = NULL;
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }
            else
            {
                atCmdRespInfoText(cmd->engine, pRetString);
                atCmdRespOK(cmd->engine);
            }
            free(pRetString);
            free(pAsciiRetString);
            pAsciiRetString = NULL;
            pRetString = NULL;
        }
        else if (cfw_event->nType == 0xf0)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_FAILURE);
        }
    }
    return;
}

void atCmdHandleCRSML(atCommand_t *cmd)
{
    uint32_t nEFFileID = 0;
    uint32_t nStartIndex = 0x00;
    uint32_t nCount = 0x00;
    bool paramok = true;

    uint8_t nSim = atCmdGetSim(cmd->engine);
    if (cmd->params == NULL)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (cmd->type == AT_CMD_TEST) //AT_CMD_READ
    {
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        if (3 != cmd->param_count)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        /* get all param */

        nEFFileID = atParamUint(cmd->params[0], &paramok);
        nStartIndex = atParamUint(cmd->params[1], &paramok);
        nCount = atParamUint(cmd->params[2], &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        OSI_LOGI(0, "nFileID = %d,nStartIndex = %d,nCount = %d", nEFFileID, nStartIndex, nCount);
        uint32_t nRet = 0x00;
        CRSMLAsyncCtx_t *async = (CRSMLAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;

        async->nCount = nCount;
        async->nStartIndex = nStartIndex;
        async->nCurrentIndex = nStartIndex;
        async->nFileID = nEFFileID;

        uint16_t nFileID = CFW_GetStackSimFileID(async->nFileID & 0XFFFF, async->nFileID >> 16, nSim);
        if (nFileID == 0xFFFF)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CRSML_rspCB, cmd);
        nRet = CFW_SimGetFileStatus(nFileID, cmd->uti, nSim);
        if (ERR_SUCCESS != nRet)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        RETURN_FOR_ASYNC();
    }
    else //if (pParam->iType == AT_CMD_TEST)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
//convert ucs2 starting with 0x81 or 0x82 to 0x80
bool converUCS2toStandard(uint8_t *dest, int16_t *length, uint8_t *src, int16_t size)
{
    OSI_LOGI(0, "src[0] = %d, src[1] = %d, src[2] = %d", src[0], src[1], src[2]);
    uint16_t d = 0;
    if (src[0] == 0x80)
    {
        if ((size - 1) > *length)
            return false;
        memcpy(dest, src + 1, size - 1);
        *length = size - 1;
        return true;
    }
    else if (src[0] == 0x81)
    {
        int16_t data_len = src[1];
        if ((data_len << 1) > *length)
        {
            OSI_LOGI(0, "data_len = %d, *length = %d", data_len << 1, *length);
            return false;
        }
        *length = data_len << 1;
        uint16_t base = src[2] << 7;
        uint16_t ucs2;
        for (uint8_t i = 0; i < data_len; i++)
        {
            if ((src[3 + i] & 0x80) == 0)
            {
                dest[d++] = 0x0;
                dest[d++] = src[3 + i];
            }
            else
            {
                ucs2 = base + (src[3 + i] & 0x7F);
                dest[d++] = ucs2 >> 8;
                dest[d++] = ucs2 & 0xFF;
            }

            OSI_LOGI(0, "dest = %X, %X", dest[d - 2], dest[d - 1]);
        }
        return true;
    }
    else if (src[0] == 0x82)
    {
        uint16_t data_len = src[1];
        if ((data_len << 1) > *length)
        {
            OSI_LOGI(0, "data_len = %d, *length = %d", data_len << 1, *length);
            return false;
        }
        *length = data_len << 1;
        uint16_t base = (src[2] << 8) | src[3];
        uint16_t ucs2;
        for (uint8_t i = 0; i < data_len; i++)
        {
            if ((src[4 + i] & 0x80) == 0)
            {
                dest[d++] = 0x0;
                dest[d++] = src[4 + i];
            }
            else
            {
                ucs2 = base + (src[4 + i] & 0x7F);
                dest[d++] = ucs2 >> 8;
                dest[d++] = ucs2 & 0xFF;
            }
            OSI_LOGI(0, "dest = %X, %X", dest[d - 2], dest[d - 1]);
        }
        return true;
    }
    else
        return false;
}
static void _qspnRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nType == 0)
    {
        uint8_t *nData = (uint8_t *)cfw_event->nParam1;
        int i = 0;
        for (i = 1; i < 17; i++)
        {
            if (nData[i] == 0xFF)
            {
                nData[i] = 0;
                break;
            }
        }
        i--;
        uint8_t nSPN[18] = {0};
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        char nResponse[100] = {0};
        char *prsp = nResponse;
        uint8_t nSim = atCmdGetSim(cmd->engine);
        uint8_t oper_id[6] = {0};
        uint8_t mode = 0;
        uint8_t alphabet = 0;
        CFW_NW_STATUS_INFO nw_info;
        OSI_LOGI(0, "nData[0] = 0x%02x, nData[1] = 0x%02x, i = %d", nData[0], nData[1], i);

        if (CFW_NwGetStatus(&nw_info, nSim) != 0)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        }

        if (nw_info.nStatus == CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING ||
            nw_info.nStatus == CFW_NW_STATUS_NOTREGISTERED_SEARCHING ||
            nw_info.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED || nw_info.nStatus == CFW_NW_STATUS_UNKNOW)
        {
            if (ERR_SUCCESS != CFW_GprsGetstatus(&nw_info, nSim))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            
            if (nw_info.nStatus == CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING ||
                nw_info.nStatus == CFW_NW_STATUS_NOTREGISTERED_SEARCHING ||
                nw_info.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED || nw_info.nStatus == CFW_NW_STATUS_UNKNOW)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
        }
        
        if (CFW_NwGetCurrentOperator(oper_id, &mode, nSim) != 0)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        uint8_t *oper_name = NULL;
        uint8_t *oper_short_name = NULL;

        if (CFW_CfgNwGetOperatorName(oper_id, &oper_name, &oper_short_name) != 0)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (i > 0)
        {
            if (nData[1] < 0x80)
            {
                alphabet = 0;
                memcpy(nSPN, nData + 1, i);
            }
            else
            {
                int16_t size = 18;

                alphabet = 1;
                if (converUCS2toStandard(nSPN, &size, nData + 1, i) == false)
                {
                    OSI_LOGI(0, "convert FALSE!");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
                int length;
                uint8_t *ucs2 = mlConvertStr(nSPN, size, ML_UTF16BE, ML_CP936, &length);
                OSI_LOGI(0, "ucs2 = %X, size = %d", ucs2, length);
                memcpy(nSPN, ucs2, length);
            }
        }
        prsp += sprintf(prsp, "+QSPN: \"%s\",\"%s\",\"%s\",%d,\"", oper_name, oper_short_name, nSPN, alphabet);
        for (int n = 0; n < 6; n++)
        {
            if (oper_id[n] > 9)
                break;
            *prsp++ = '0' + oper_id[n];
        }
        *prsp++ = '\"';
#else
        char nResponse[30] = {0};
        //according to 31.102(4.2.12),if byte[0]B2==0 display SPN,B2==1 dont display
        uint8_t nShow = nData[0] & 0x02;
        OSI_LOGI(0, "nData[0] = 0x%02x, nData[1] = 0x%02x, i = %d", nData[0], nData[1], i);
        if ((nShow == 0) && (nData[1] != 0))
        {
            nShow = 0x01;
        }
        else if ((nShow == 1) && (nData[1] == 0))
        {
            nShow = 0;
        }
        else
        {
            nShow = nData[0] & 0x02;
        }
        uint8_t offset = sprintf(nResponse, "+QSPN: %d", nShow);
        if (i > 0)
        {
            if (nData[1] < 0x80)
                memcpy(nSPN, nData + 1, i);
            else
            {
                int16_t size = 18;
                if (converUCS2toStandard(nSPN, &size, nData + 1, i) == false)
                {
                    OSI_LOGI(0, "convert FALSE!");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
                int length;
                uint8_t *ucs2 = mlConvertStr(nSPN, size, ML_UTF16BE, ML_CP936, &length);
                OSI_LOGI(0, "ucs2 = %X, size = %d", ucs2, length);
                memcpy(nSPN, ucs2, length);
            }
            sprintf(nResponse + offset, ", %s", nSPN);
        }
#endif        
        atCmdRespInfoText(cmd->engine, nResponse);
        atCmdRespOK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}
void atCmdHandleQSPN(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
    if (cmd->type == AT_CMD_TEST)
#else
    if (cmd->type == AT_CMD_TEST || cmd->type == AT_CMD_READ)
#endif        
    {
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_EXE)
    {
        uint8_t nEF = 0;
        if (CFW_GetSimType(nSim))
            nEF = 77;
        else
            nEF = 27;
        cmd->uti = cfwRequestUTI((osiEventCallback_t)_qspnRspCB, cmd);
        uint32_t nRet = CFW_SimReadBinary(nEF, 0, 0x11, cmd->uti, nSim);
        if (0 != nRet)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        RETURN_FOR_ASYNC();
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}
typedef struct
{
    uint16_t qgid_file_size;
    uint8_t qgid_file_id[2];
    uint8_t *qgid_gid[2];
} qgidAsyncCtx_t;

static void _qgidReadBinaryRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    qgidAsyncCtx_t *async = (qgidAsyncCtx_t *)cmd->async_ctx;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    if (cfw_event->nType == 0)
    {
        if (async->qgid_gid[0] == 0)
        {
            OSI_LOGI(0, "GID1");
            async->qgid_gid[0] = (uint8_t *)cfw_event->nParam1;
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_qgidReadBinaryRspCB, cmd);
            uint32_t nResult = CFW_SimReadBinary(async->qgid_file_id[1], 0, async->qgid_file_size, cmd->uti, nSim);
            if (0 != nResult)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            RETURN_FOR_ASYNC();
        }
        else
        {
            OSI_LOGI(0, "GID2");
            async->qgid_gid[1] = (uint8_t *)cfw_event->nParam1;
            char rsp[255] = {0};
            char qgid_gid0[125] = {0};
            char qgid_gid1[125] = {0};
            cfwBytesToHexStr(async->qgid_gid[0], async->qgid_file_size, qgid_gid0);
            cfwBytesToHexStr(async->qgid_gid[1], async->qgid_file_size, qgid_gid1);
            sprintf(rsp, "+QGID: %s, %s", qgid_gid0, qgid_gid1);
            atCmdRespInfoText(cmd->engine, rsp);
            RETURN_OK(cmd->engine);
        }
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

static void _qgidReadElementaryRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    qgidAsyncCtx_t *async = (qgidAsyncCtx_t *)cmd->async_ctx;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    if (cfw_event->nType == 0)
    {
        async->qgid_file_size = cfw_event->nParam1;
        OSI_LOGI(0, "GID: qgid_file_size = %d", async->qgid_file_size);
        cmd->uti = cfwRequestUTI((osiEventCallback_t)_qgidReadBinaryRspCB, cmd);
        uint32_t nResult = CFW_SimReadBinary(async->qgid_file_id[0], 0, async->qgid_file_size, cmd->uti, nSim);
        if (0 != nResult)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        RETURN_FOR_ASYNC();
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

void atCmdHandleQGID(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_TEST || cmd->type == AT_CMD_READ)
    {
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_EXE)
    {
        qgidAsyncCtx_t *async = (qgidAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        if (CFW_GetSimType(nSim))
        {
            async->qgid_file_id[0] = 70;
            async->qgid_file_id[1] = 71;
        }
        else
        {
            async->qgid_file_id[0] = 23;
            async->qgid_file_id[1] = 24;
        }
        async->qgid_gid[0] = 0;
        async->qgid_gid[1] = 0;
        async->qgid_file_size = 0;
        OSI_LOGI(0, "qgid_file_id[0] = %x", async->qgid_file_id[0]);
        cmd->uti = cfwRequestUTI((osiEventCallback_t)_qgidReadElementaryRspCB, cmd);
        uint32_t nResult = CFW_SimReadElementary(async->qgid_file_id[0], cmd->uti, nSim);
        if (0 != nResult)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        RETURN_FOR_ASYNC();
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}

#ifdef PORTING_ON_GOING
void AT_SIM_CmdFunc_CAMM(atCommand_t *cmd)
{
    uint8_t cmd->param_count = 0;
    uint32_t eParamOk = 0;
    uint32_t iACMMax = 0;
    uint8_t usACMMax[CAMMLENGTH] = {0}; // /????????data闀垮害搴旇鏄灏??
    uint8_t usPassword[PIN2CODELENGTH] = {0};
    uint16_t uStrLen = 0;
    uint32_t nResult = 0;
    //  uint8_t nUTI                       = pParam->nDLCI;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }

    uint8_t nSim = atCmdGetSim(cmd->engine);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    if (cmd->type == AT_CMD_TEST)
    {
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", pParam->nDLCI);
        return;
    }
    else if (cmd->type == AT_CMD_READ)
    {
        nResult = CFW_SimGetACMMax(pParam->nDLCI, nSim);
        if (ERR_SUCCESS == nResult)
        {
            AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, "", pParam->nDLCI);
            return;
        }
        else
        {
            AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, pParam->nDLCI);
            return;
        }
    }
    else if (cmd->type == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &cmd->param_count))) /* GET param count */
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (cmd->param_count != 2)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        uStrLen = CAMMLENGTH;
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, usACMMax, &uStrLen);
        if (eParamOk != ERR_SUCCESS) /* get uCallType failed */
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (cmd->param_count > 1)
        {

            uStrLen = PIN2CODELENGTH;
            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, &usPassword, &uStrLen);
            if (eParamOk != ERR_SUCCESS) /* get uVolume failed */
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }
        }
        OSI_LOGXI(OSI_LOGPAR_ISSI, 0x100052c3, "cmd->param_count = %d,usACMMax = %s,usPassword = %s,uStrLen = %d", cmd->param_count,
                  usACMMax, usPassword, uStrLen);

        iACMMax = AT_CAMM_STRtoUINT32(usACMMax);
        if (iACMMax == ERR_AT_CME_PARAM_INVALID)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        //format of iACMMax is 0x00FFFFFF
        OSI_LOGXI(OSI_LOGPAR(I, S, S, I, I), 0x100052c4, "cmd->param_count = %d,usACMMax = %s,usPassword = %s,uStrLen = %d,iACMMax = %lu", cmd->param_count, usACMMax,
                  usPassword, uStrLen, iACMMax);
        nResult = CFW_SimSetACMMax(iACMMax, usPassword, uStrLen, pParam->nDLCI, nSim);
        // execute result return
        if (ERR_SUCCESS == nResult)
        {
            AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, "", pParam->nDLCI);
            return;
        }
        else
        {
            AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, pParam->nDLCI);
            return;
        }
    }
    else // changyg[+] 2008-6-12 default wrong process for bug 8758
    {
        AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
        return;
    }
}

uint16_t qgid_file_size;
uint8_t qgid_file_id[2];
uint8_t *qgid_gid[2] = {0, 0};
void AT_SIM_CmdFunc_QGID(atCommand_t *cmd)
{
    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    OSI_LOGI(0x100052c5, "GID: cmd->type = %d", cmd->type);

    uint8_t nSim = atCmdGetSim(cmd->engine);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    switch (cmd->type)
    {
    case AT_CMD_TEST:
    case AT_CMD_READ:
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", pParam->nDLCI);
        return;
    case AT_CMD_EXE:
    {
        if (CFW_GetSimType(nSim) == TRUE)
        {
            qgid_file_id[0] = API_USIM_EF_GID1;
            qgid_file_id[1] = API_USIM_EF_GID2;
        }
        else
        {
            qgid_file_id[0] = API_SIM_EF_GID1;
            qgid_file_id[1] = API_SIM_EF_GID2;
        }
        qgid_gid[0] = 0;
        qgid_gid[1] = 0;
        qgid_file_size = 0;
        OSI_LOGI(0x100046a5, "qgid_file_id[0] = %x", qgid_file_id[0]);
        uint32_t nResult = CFW_SimReadElementary(qgid_file_id[0], pParam->nDLCI, nSim);
        if (nResult == ERR_SUCCESS)
            AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, "", pParam->nDLCI);
        else
            AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, pParam->nDLCI);
        return;
    }
    default:
    {
        AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, pParam->nDLCI);
        return;
    }
    }
}

void AT_SIM_CmdFunc_QSPN(atCommand_t *cmd)
{
    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }

    uint8_t nSim = atCmdGetSim(cmd->engine);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;
    OSI_LOGI(0x100052c6, "SPN: cmd->type = %d", cmd->type);

    switch (cmd->type)
    {
    case AT_CMD_TEST:
    case AT_CMD_READ:
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", pParam->nDLCI);
        return;
    case AT_CMD_EXE:
    {
        uint8_t nEF = 0;
        if (CFW_GetSimType(nSim))
            nEF = API_USIM_EF_SPN;
        else
            nEF = API_SIM_EF_SPN;

        uint32_t nResult = CFW_SimReadBinary(nEF, 0, 0x11, pParam->nDLCI, nSim);
        if (nResult == ERR_SUCCESS)
            AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, "", pParam->nDLCI);
        else
            AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, pParam->nDLCI);

        return;
    }
    default:
    {
        AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, pParam->nDLCI);
        return;
    }
    }
}

//AT+SIMIF=<type>,<mode>
//===============================
//|                 |   0     |    1      |
//|  mode      | value  | text    |
//-------------------------------------
//|       |        |   0     |   SIM   |
//|       | 1     ------------------------------
//|       |        |   1     |  UICC  |
//|type| -------------------------------
//|       |todo...
//|===============================
uint8_t g_simif_type = 1;
uint8_t g_simif_mode = 0;
void AT_SIM_CmdFunc_SIMIF(atCommand_t *cmd)
{
    uint8_t Count = 0;
    uint32_t uErrCode = ERR_AT_CME_PARAM_INVALID;
    uint8_t nResp[50] = {0};

    if ((pParam == NULL) || (pParam->pPara == NULL))
    {
        OSI_LOGI(0x100046a7, "SIMIF: (pParam == NULL) ||(pParam->pPara == NULL)");
        goto FAILED_EXIT;
    }

    uint8_t nSim = atCmdGetSim(cmd->engine);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    OSI_LOGI(0x100052c7, "cmd->type = %d", cmd->type);
    if (cmd->type == AT_CMD_SET)
    {
        if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &Count))
        {
            OSI_LOGI(0x100046a8, "SIMIF: AT_Util_GetParaCount return Error!");
            goto FAILED_EXIT;
        }
        if ((Count != 1) && (Count != 2))
        {
            OSI_LOGI(0x100046a9, "SIMIF: AT_Util_GetParaCount = %d", Count);
            goto FAILED_EXIT;
        }

        bool simType = CFW_GetSimType(nSim); //0: sim; 1:Usim
        uint8_t type = 0xFF;
        uint16_t length = 1;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &type, &length))
        {
            OSI_LOGI(0x100046aa, "SIMIF: AT_Util_GetParaWithRule get first parameter return Error ");
            goto FAILED_EXIT;
        }
        if (type != 1) //the first parameter just support 1 in this version, other for the furture expansion.
            goto FAILED_EXIT;
        g_simif_type = type;
        OSI_LOGI(0x100046ab, "SIMIF: type = %d", type);

        uint8_t mode = 0xFF;
        length = 1;
        if (Count == 2)
        {
            if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &mode, &length))
            {
                OSI_LOGI(0x100046ac, "SIMIF: AT_Util_GetParaWithRule get second parameter return Error ");
                goto FAILED_EXIT;
            }
            if (mode > 1)
                goto FAILED_EXIT;
            g_simif_mode = mode;
            OSI_LOGI(0x100046ad, "SIMIF: mode = %d", mode);
        }

        if ((mode == 0) || (mode == 0xFF))
        {
            sprintf(nResp, "^SIMIF: %d", simType);
        }
        else //mode == 1
        {
            uint8_t *pType;
            if (simType == 0)
                pType = "SIM";
            else
                pType = "UICC";

            sprintf(nResp, "^SIMIF: %s", pType);
        }
        goto SUCC_EXIT;
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        sprintf(nResp, "^SIMIF: (1), (0,1)");
        goto SUCC_EXIT;
    }
    else if (cmd->type == AT_CMD_READ)
    {
        sprintf(nResp, "^SIMIF: %d,%d", g_simif_type, g_simif_mode);
        goto SUCC_EXIT;
    }
    else
    {
        uErrCode = ERR_AT_CME_EXE_NOT_SURPORT;
    }

FAILED_EXIT:
    AT_SIM_Result_Err(uErrCode, pParam->nDLCI);
    return;

SUCC_EXIT:
    AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nResp, pParam->nDLCI);
    return;
}
#endif
/*
 *******************************************************************************
 *  possible input from ATM:                                                   *
 *  AT+cpin=? -->test, return OK;                                              *
 *  AT+cpin? --> read,wait asyncrounous event,response e.g.:                   *
 *          ---> "+CPIN: SIM PIN","+CPIN: READY"                               *
 *  AT+cpin="1234"(4<pinSize<8) -->reponse:OK,ERROR                            *
 *  AT+cpin="12345678","1234"(pukSize=8),(4<pinSize<8) -->reponse:OK,ERROR     *
 *******************************************************************************
*/
/*extern*/ bool gSimDropInd[CONFIG_NUMBER_OF_SIM];
static void _cpinEnterRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (0 == cfw_event->nType && 0 == cfw_event->nParam1)
        RETURN_OK(cmd->engine);

#ifdef PORTING_ON_GOING
    // add by wangxd for bug 8046
    if (g_Pin1PukStauts[nSim])
        g_Pin1ErrorTimes[nSim] = 0;
    else
    {
        if ((cfw_event->nParam1 != ERR_CFW_QUEUE_FULL) ||
            (cfw_event->nParam1 != ERR_CMS_MEMORY_FAILURE))
            g_Pin1ErrorTimes[nSim]++;
        if (g_Pin1ErrorTimes[nSim] >= 3)
        {
            g_Pin1PukStauts[nSim] = 1;
            g_Pin1ErrorTimes[nSim] = 0;
        }
    }
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE
	atCmdRespCmeError(cmd->engine, atCfwToCmeError(cfw_event->nParam1));
	if(cfw_event->nParam1 == ERR_CME_SIM_COMMAND_SUCCESS_VERIFICATION_FAIL)
	{
		ql_sim_status_e card_status = 0;
		uint32_t ret = ql_sim_get_card_status(cfw_event->nFlag, &card_status);
		if(ret == QL_SIM_SUCCESS)
		{
			char rsp[20] = {0};
			if(QL_SIM_STATUS_SIMPUK == card_status){
				snprintf(rsp,sizeof(rsp),"+CPIN: SIM PUK");
				quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, 0);
		    }else if(QL_SIM_STATUS_SIMPUK2 == card_status){
				snprintf(rsp,sizeof(rsp),"+CPIN: SIM PUK2");
				quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, 0);
			}
	    }
    }
#else
    RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
#endif
}

typedef struct
{
    unsigned errcode;
    malSimState_t state;
    unsigned remainRetries;
} atCpinReadContext_t;

#ifdef CONFIG_QUEC_PROJECT_FEATURE
void quec_simstate_resp(atCommand_t *cmd, malSimState_t state)
{
    char rsp[64] = {0};
    uint32_t sim_err_code = ERR_AT_CME_SIM_FAILURE;
    
    switch (state)
    {
        case CFW_STY_AUTH_READY:
            sprintf(rsp, "%s: READY", cmd->desc->name);
            quec_atResp(cmd->engine, ATCI_RESULT_CODE_OK, CMD_RC_OK, rsp, 1);
            return;
        case CFW_STY_AUTH_PIN1_READY: //"PIN1 READY"
            sim_err_code = ERR_AT_CME_SIM_BUSY;
            break;
        case CFW_STY_AUTH_SIMPIN:
        case CFW_STY_AUTH_PIN1BLOCK: //"PIN1 BLOCK"
            sprintf(rsp, "%s: SIM PIN", cmd->desc->name);
            quec_atResp(cmd->engine, ATCI_RESULT_CODE_OK, CMD_RC_OK, rsp, 1);
            return;
            //sim_err_code = ERR_AT_CME_SIM_PIN_REQUIRED;
        case CFW_STY_AUTH_PIN1_DISABLE: //"PIN1 DISABLE"
            sim_err_code = ERR_AT_CME_SIM_BUSY;
            break;
        case CFW_STY_AUTH_SIMPUK:
        	sprintf(rsp, "%s: SIM PUK", cmd->desc->name);
            quec_atResp(cmd->engine, ATCI_RESULT_CODE_OK, CMD_RC_OK, rsp, 1);
            return;
            //sim_err_code = ERR_AT_CME_SIM_PUK_REQUIRED;
            break;
        case CFW_STY_AUTH_PHONE_TO_SIMPIN:
            sim_err_code = ERR_AT_CME_PHSIM_PIN_REQUIRED;
            break;
        case CFW_STY_AUTH_PHONE_TO_FIRST_SIMPIN:
            sim_err_code = ERR_AT_CME_PHFSIM_PIN_REQUIRED;
            break;
        case CFW_STY_AUTH_PHONE_TO_FIRST_SIMPUK:
            sim_err_code = ERR_AT_CME_PHFSIM_PUK_REQUIRED;
            break;
        case CFW_STY_AUTH_SIMPIN2:
        case CFW_STY_AUTH_PIN2BLOCK: //"PIN2 BLOCK"
            sprintf(rsp, "%s: SIM PIN2", cmd->desc->name);
            quec_atResp(cmd->engine, ATCI_RESULT_CODE_OK, CMD_RC_OK, rsp, 1);
            return;
            //sim_err_code = ERR_AT_CME_SIM_PIN2_REQUIRED;
            break;
        case CFW_STY_AUTH_SIMPUK2:
        	sprintf(rsp, "%s: SIM PUK2", cmd->desc->name);
            quec_atResp(cmd->engine, ATCI_RESULT_CODE_OK, CMD_RC_OK, rsp, 1);
            return;
            //sim_err_code = ERR_AT_CME_SIM_PUK2_REQUIRED;
            break;
        case CFW_STY_AUTH_NETWORKPIN:
            sim_err_code = ERR_AT_CME_NET_PER_PIN_REQUIRED;
            break;
        case CFW_STY_AUTH_NETWORKPUK:
            sim_err_code = ERR_AT_CME_NET_PER_PUK_REQUIRED;
            break;
        case CFW_STY_AUTH_NETWORK_SUBSETPIN:
            sim_err_code = ERR_AT_CME_NET_SUB_PER_PIN_REQ;
            break;
        case CFW_STY_AUTHNETWORK_SUBSETPUK:
            sim_err_code = ERR_AT_CME_NET_SUB_PER_PUK_REQ;
            break;
        case CFW_STY_AUTH_PROVIDERPIN:
            sim_err_code = ERR_AT_CME_SERVICE_PROV_PER_PIN_REQ;
            break;
        case CFW_STY_AUTH_PROVIDERPUK:
            sim_err_code = ERR_AT_CME_SERVICE_PROV_PER_PUK_REQ;
            break;
        case CFW_STY_AUTH_CORPORATEPIN:
            sim_err_code = ERR_AT_CME_CORPORATE_PER_PIN_REQ;
            break;
        case CFW_STY_AUTH_CORPORATEPUK:
            sim_err_code = ERR_AT_CME_CORPORATE_PER_PUK_REQ;
            break;
        case CFW_STY_AUTH_NOSIM:
            sim_err_code = ERR_AT_CME_SIM_NOT_INSERTED;
            break;
        case CFW_STY_AUTH_SIM_PRESENT: //"SIM PRESENT"
            sim_err_code = ERR_AT_CME_SIM_BUSY;
            break;
        default:
            sim_err_code = ERR_AT_CME_SIM_FAILURE;
            break;
    }

    quec_atCmdResp(cmd->engine, ATCI_RESULT_CODE_CME_ERROR, sim_err_code);
}
#endif

static void prvCpinReadFinish(void *param)
{
    atCommand_t *cmd = (atCommand_t *)param;
    atCpinReadContext_t *ctx = (atCpinReadContext_t *)cmd->async_ctx;

    if (ctx->errcode != 0)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

    const char *str = osiVsmapFindStr(gCpinStr, ctx->state, NULL);
    if (str == NULL)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
	quec_simstate_resp(cmd, ctx->state);
#else
    char rsp[64];
    sprintf(rsp, "%s: %s", cmd->desc->name, osiVsmapFindStr(gCpinStr, ctx->state, ""));
    atCmdRespInfoText(cmd->engine, rsp);
    RETURN_OK(cmd->engine);
#endif	
}

static void prvCpinReadCall(void *param)
{
    atCommand_t *cmd = (atCommand_t *)param;
    atCpinReadContext_t *ctx = (atCpinReadContext_t *)cmd->async_ctx;
    ctx->errcode = malSimGetState(atCmdGetSim(cmd->engine), &ctx->state, &ctx->remainRetries);
    atEngineCallback(prvCpinReadFinish, cmd);
}

void atCmdHandleCPIN(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t result;

    if (cmd->type == AT_CMD_SET)
    {
        // AT_CMD_SET-------------------->
        /********************************************************
             *  uint32_t CFW_SimEnterAuthentication(      *
             *                uint8_t* pPin,      *
             *                uint8_t  nPinSize,  *
             *                uint8_t* pNewPin,   *
             *                uint8_t  nNewPinSize,   *
             *                uint8_t  nOption,   *
             *                uint16_t nUTI);     *
             ********************************************************/
        bool paramok = true;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		if (cmd->param_count > 2){
			RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
		}
		char pin[16] = {0};
		char newpin[16] = {0};
		//按照字符串格式读取pin
		const char *pin_str = atParamStr(cmd->params[0], &paramok);
		if (!paramok){
			paramok = true;
			//按照uint格式读取pin
			uint32_t pin_uint = atParamUint(cmd->params[0], &paramok);
			if(!paramok){
				RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
			}
			itoa(pin_uint, pin, 10);//按10进制方式将uint变量转换为字符串型
		}
		else
		{
			strncpy(pin, pin_str, sizeof(pin));
		}
		if(cmd->param_count == 2)
		{
			//按照字符串格式读取pin
			const char *newpin_str = atParamStr(cmd->params[1], &paramok);
			if (!paramok){
				paramok = true;
				//按照uint格式读取pin
				uint32_t newpin_uint = atParamUint(cmd->params[1], &paramok);
				if(!paramok){
					RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
				}
				itoa(newpin_uint, newpin, 10);//按10进制方式将uint变量转换为字符串型
			}
			else
			{
				strncpy(newpin, newpin_str, sizeof(newpin));
			}
		}
#else
        const char *pin = atParamStr(cmd->params[0], &paramok);
        const char *newpin = atParamDefStr(cmd->params[1], NULL, &paramok);
        if (!paramok || cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#endif
        int pin_size = strlen(pin);
        int newpin_size = (newpin == NULL) ? 0 : strlen(newpin);

#ifdef PORTING_ON_GOING
        if (newpin == NULL && g_Pin1PukStauts[nSim])
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#endif

        uint8_t nOption = 1; // what is it?
        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpinEnterRspCB, cmd);
        result = CFW_SimEnterAuthentication((uint8_t *)pin, pin_size,
                                            (uint8_t *)newpin, newpin_size,
                                            nOption, cmd->uti, nSim);
        if (result != ERR_SUCCESS)
        {
            cfwReleaseUTI(cmd->uti);
            if (ERR_CFW_INVALID_PARAMETER == result)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (ERR_NO_MORE_MEMORY == result)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        }

        gLstStatusRec = 1;
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        atCpinReadContext_t *ctx = (atCpinReadContext_t *)calloc(1, sizeof(*ctx));
        if (ctx == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = ctx;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;

        atCmdWorkerCall(cmd->engine, prvCpinReadCall, cmd);
        RETURN_FOR_ASYNC();
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void sulgsmbcd2asciiEx(uint8_t *pBcd, uint16_t nBcdLen, uint8_t *pNumber)
{
    uint16_t i;
    uint8_t *pTmp = pNumber;
    for (i = 0; i < nBcdLen; i++)
    {
        uint16_t high4bits = pBcd[i] >> 4;
        uint16_t low4bits = pBcd[i] & 0x0F;

        if (low4bits < 0x0A)
            *pTmp++ = low4bits + '0'; // 0 - 0x09
        else if (low4bits < 0x0F)     // 0x0A - 0x0F
            *pTmp++ = low4bits - 0x0A + 'A';

        if (high4bits < 0x0A)
            *pTmp++ = high4bits + '0'; // 0 - 9
        else if (high4bits < 0x0F)     // 0x0A - 0x0F
            *pTmp++ = high4bits - 0x0A + 'A';
    }
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SIM_AT
uint16_t atConvertICCID(uint8_t *pInput, uint16_t nInputLen, uint8_t *pOutput)
{
    uint16_t i;
    uint8_t *pBuffer = pOutput;
    for (i = 0; i < nInputLen; i++)
    {
        uint8_t high4bits = pInput[i] >> 4;
        uint8_t low4bits = pInput[i] & 0x0F;

        if (low4bits < 0x0A)
            *pOutput++ = low4bits + '0'; // 0 - 0x09
        else                             // 0x0A - 0x0F
            *pOutput++ = low4bits - 0x0A + 'A';

        if (high4bits < 0x0A)
            *pOutput++ = high4bits + '0'; // 0 - 9
        else                              // 0x0A - 0x0F
            *pOutput++ = high4bits - 0x0A + 'A';
    }
    OSI_LOGI(0, "shane: return size = %d", pOutput - pBuffer);
    return pOutput - pBuffer;
}

static void _ccidRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    //EV_CFW_SIM_GET_ICCID_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType == 0)
    {
        uint8_t ICCID[21] = {0};
        atConvertICCID((uint8_t *)(cfw_event->nParam1), 10, ICCID);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "pICCID2 =%s\n", ICCID);

        atMemFreeLater((void *)cfw_event->nParam1);

        char response[30] = {0};
        if (strcmp("$MYCCID", cmd->desc->name) == 0)
            sprintf(response, "$MYCCID: \"%s\"", ICCID);
        else if (strcmp("+CCID", cmd->desc->name) == 0)
            sprintf(response, "+CCID: %s", ICCID);
        else
            RETURN_CME_CFW_ERR(cmd->engine, CMD_RC_NOTSUPPORT);

        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
    }
}

void atCmdHandleCCID(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    if (cmd->type == AT_CMD_EXE)
    {
        CFW_SIM_STATUS status = CFW_GetSimStatus(nSim);
        OSI_LOGI(0, "CCID get sim status: %d", status);
        if (status == CFW_SIM_ABSENT)
        {
            RETURN_CME_CFW_ERR(cmd->engine, ERR_AT_CME_SIM_NOT_INSERTED);
        }

        uint8_t *pICCID = CFW_GetICCID(nSim);
        if (pICCID != NULL)
        {
            uint8_t ICCID[21] = {0};
            OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", 10, pICCID);
            OSI_LOGI(0, "shane: at_hex2ascii");
            atConvertICCID(pICCID, 10, ICCID);
            OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", 20, ICCID);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "pICCID1 =%s\n", ICCID);
            char response[30] = {0};

            if (strcmp("$MYCCID", cmd->desc->name) == 0)
                sprintf(response, "$MYCCID: \"%s\"", ICCID);
            else if (strcmp("+CCID", cmd->desc->name) == 0)
                sprintf(response, "+CCID: %s", ICCID);
            else
                RETURN_CME_CFW_ERR(cmd->engine, CMD_RC_NOTSUPPORT);
            atCmdRespInfoText(cmd->engine, response);
            RETURN_OK(cmd->engine);
        }
        else
        {
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_ccidRspCB, cmd);
            uint32_t result = CFW_SimGetICCID(cmd->uti, nSim);
            if (result != ERR_SUCCESS)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, result);
            }
            RETURN_FOR_ASYNC();
        }
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}
#endif
void atCmdHandleSIM(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    if (cmd->type == AT_CMD_TEST)
    {
        char response[30] = {0};
        sprintf(response, "+SIM: (0 - %d)", CFW_SIM_COUNT - 1);
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }

    else if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t nSimID = atParamUintInRange(cmd->params[0], 0, CFW_SIM_COUNT, &paramok);
        if ((!paramok) || (cmd->param_count != 1))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        bool simType = CFW_GetSimType(nSim);
        char *pType;
        if (simType == 1)
            pType = "USIM";
        else
            pType = "SIM";
        if (nSimID < CFW_SIM_COUNT)
        {
            CFW_SIM_STATUS status = CFW_GetSimStatus(nSimID);
            OSI_LOGXI(OSI_LOGPAR_SI, 0, "%s status :%d", pType, status);
            char response[30] = {0};
            if (status != 0x04)
            {
                if (status == CFW_SIM_ABSENT)
                    sprintf(response, "+%s: ABSENT", pType);
                else if (status == CFW_SIM_NORMAL)
                    sprintf(response, "+%s: NORMAL", pType);
                else if (status == CFW_SIM_TEST)
                    sprintf(response, "+%s: TEST", pType);
                else if (status == CFW_SIM_ABNORMAL)
                    sprintf(response, "+%s: ABNORMAL", pType);
            }
            else
                sprintf(response, "+SIM: ABSENT");
            atCmdRespInfoText(cmd->engine, response);
            RETURN_OK(cmd->engine);
        }
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}

// ============================================================================
// AT^SIMOperTimes
/// Get the total number of SIM card operations.
/// This function is added to check the total number of SIM card operations
/// after power on in order to prevent some software from operating the SIM
/// card abnormally,which causes the SIM card file to be operated repeatedly
/// for longer than its service life.
// ============================================================================

void atCmdHandleSIMOperationTimes(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        char response[40] = {0};
        sprintf(response, "Total operation times %lu", CFW_SimGetOperationTimes());
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}

//AT+SIMIF=<type>,<mode>
//===============================
//|                 |   0     |    1      |
//|  mode      | value  | text    |
//-------------------------------------
//|       |        |   0     |   SIM   |
//|       | 1     ------------------------------
//|       |        |   1     |  UICC  |
//|type| -------------------------------
//|       |todo...
//|===============================
uint8_t g_simif_type = 1;
uint8_t g_simif_mode = 0;
void atCmdHandleSIMIF(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    char rsp[50] = {
        0,
    };
    bool simType = CFW_GetSimType(nSim);

    if (cmd->type == AT_CMD_SET)
    {
        CFW_SIM_STATUS status = CFW_GetSimStatus(nSim);
        OSI_LOGXI(OSI_LOGPAR_I, 0, "status: %d", status);
        if (status != CFW_SIM_ABNORMAL && status != CFW_SIM_TEST && status != CFW_SIM_NORMAL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_NOT_INSERTED);

        bool paramok = true;
        if ((cmd->param_count != 1) && (cmd->param_count != 2))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint8_t type = atParamUintInRange(cmd->params[0], 1, 1, &paramok);
        if (!paramok || (cmd->params[0]->length != 1))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        g_simif_type = type;

        uint8_t mode = 0xFF;
        if (cmd->param_count == 2)
        {
            mode = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
            if (!paramok || (cmd->params[1]->length != 1))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            g_simif_mode = mode;
        }
        if (mode == 0)
            sprintf(rsp, "^SIMIF: %d", simType);
        else
        {
            char *pType;
            if (simType == 0)
                pType = "SIM";
            else
                pType = "UICC";
            sprintf(rsp, "^SIMIF: %s", pType);
        }
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        sprintf(rsp, "^SIMIF: (1), (0,1)");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        if (nSim < CFW_SIM_COUNT)
        {
            CFW_SIM_STATUS status = CFW_GetSimStatus(nSim);
            OSI_LOGXI(OSI_LOGPAR_I, 0, "status: %d", status);
            if (status != CFW_SIM_STATUS_END)
            {
                if (status == CFW_SIM_ABSENT)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_NOT_INSERTED);
                else if (status == CFW_SIM_ABNORMAL || status == CFW_SIM_TEST || status == CFW_SIM_NORMAL)
                {
                    sprintf(rsp, "^SIMIF: %d,%d", g_simif_type, simType);
                    atCmdRespInfoText(cmd->engine, rsp);
                    RETURN_OK(cmd->engine);
                }
                else
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_FILEID_NOT_FOUND);
            }
            else
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_FILEID_NOT_FOUND);
        }
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}

static const osiValueStrMap_t gClckTypeVSMap[] = {
    {CFW_STY_FAC_TYPE_AO, "AO"},
    {CFW_STY_FAC_TYPE_OI, "OI"},
    {CFW_STY_FAC_TYPE_OX, "OX"},
    {CFW_STY_FAC_TYPE_AI, "AI"},
    {CFW_STY_FAC_TYPE_IR, "IR"},
    {CFW_STY_FAC_TYPE_AB, "AB"},
    {CFW_STY_FAC_TYPE_AG, "AG"},
    {CFW_STY_FAC_TYPE_AC, "AC"},
    {CFW_STY_FAC_TYPE_SC, "SC"},
    {CFW_STY_FAC_TYPE_PS, "PS"},
    {CFW_STY_FAC_TYPE_P2, "P2"},
    {CFW_STY_FAC_TYPE_PF, "PF"},
    {CFW_STY_FAC_TYPE_PN, "PN"},
    {CFW_STY_FAC_TYPE_PU, "PU"},
    {CFW_STY_FAC_TYPE_PP, "PP"},
    {CFW_STY_FAC_TYPE_PC, "PC"},
    {CFW_STY_FAC_TYPE_FD, "FD"},
    {0, NULL},
};

static void _clckSimSetRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (0 == cfw_event->nType)
    {
#ifdef PORTING_ON_GOING
        AT_FDN_SetStatus(nSim);
#endif
        RETURN_OK(cmd->engine);
    }
    else if (0xF0 == cfw_event->nType)
    {
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
}

static void _clckSsSetRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    OSI_LOGI(0, "_clckSsSetRspCB");

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    OSI_LOGI(0, "_clckSsSetRspCB cfw_event->nType: %x, cfw_event->nParam1: %x, cfw_event->nParam2: %x", cfw_event->nType, cfw_event->nParam1, cfw_event->nParam2);
    if (0 == cfw_event->nType)
    {
        RETURN_OK(cmd->engine);
    }
    else if (0xF0 == cfw_event->nType)
    {
        uint32_t uiErrCode = 0;
        uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
        if (0 != uiErrCode)
        {
            RETURN_CME_ERR(cmd->engine, uiErrCode);
        }
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

static void _clckSimGetRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (0 == cfw_event->nType)
    {
        char rsp[10] = {
            0,
        };
        sprintf(rsp, "+CLCK:%d", (uint8_t)cfw_event->nParam1);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (0xF0 == cfw_event->nType)
        RETURN_CME_ERR(cmd->engine, atCfwToCmeError(cfw_event->nParam1));
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
}

static void _clckSsQueryRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

/*
*******************************************************************************
*  Write: AT+CLCK=<fac>,<mode>[,<passwd>[,<class>]]
*  Test:  AT+CLCK=?
*  possible input from ATM:
*  AT+CLCK=?            -->res:+CLCK: ("PS","PF","SC","AO","OI","OX","AI",
*                "IR","AB","AG","AC","FD","PN","PU","PP","PC")
*                OK
*  AT+CLCK="SC",1,"1234"---->reponse:OK,ERROR
*  AT+CLCK="SC",2       ---->read,res:1.ERROR(when need to enter pin),2.+clck:1(No need to enter pin)
*  AT+CLCK="SC",0,"1234"---->reponse:OK,ERROR
*******************************************************************************
*/
void atCmdHandleCLCK(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t return_val = 0;
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t nClassx = 0;
        uint8_t uBufPwd[12] = {0};
        uint8_t nPwdSize = 0;
        if ((cmd->param_count != 2) && (cmd->param_count != 3) && (cmd->param_count != 4))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint16_t nFac = atParamUintByStrMap(cmd->params[0], gClckTypeVSMap, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint8_t nMode = atParamUint(cmd->params[1], &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (!((((0 == nMode) || (1 == nMode)) && ((3 == cmd->param_count) || (4 == cmd->param_count))) || ((2 == nMode) && ((2 == cmd->param_count) || (3 == cmd->param_count)))))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (cmd->param_count >= 3)
        {
            if (2 == nMode)
            {
                nClassx = atParamUint(cmd->params[2], &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            else
            {
                const char *uPwd = atParamStr(cmd->params[2], &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                nPwdSize = strlen(uPwd);
                if ((nPwdSize > 8) || (nPwdSize < 1))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                for (int i = 0; i < nPwdSize; i++)
                {
                    uBufPwd[i] = *uPwd;
                    uPwd++;
                    if ((uBufPwd[i] < '0') || (uBufPwd[i] > '9'))
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }
        }

        if (cmd->param_count == 4)
        {
            nClassx = atParamUint(cmd->params[3], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if ((0 == nMode) || (1 == nMode))
        {
            if ((nFac == CFW_STY_FAC_TYPE_SC) || (nFac == CFW_STY_FAC_TYPE_FD))
            {
                OSI_LOGXI(OSI_LOGPAR_ISII, 0, "Set Facility Lock nFac:0x%x,uBufPwd:%s,nPwdSize:0x%x,nMode:0x%x\n", nFac, uBufPwd, nPwdSize, nMode);
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_clckSimSetRspCB, cmd);
                return_val = CFW_SimSetFacilityLock(nFac, uBufPwd, nPwdSize, nMode, cmd->uti, nSim);
            }
            else
            {
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_clckSsSetRspCB, cmd);
                return_val = CFW_SsSetFacilityLock(nFac, uBufPwd, nPwdSize, nClassx, nMode, cmd->uti, nSim);
            }
        }
        else
        {
            if ((nFac == CFW_STY_FAC_TYPE_SC) || (nFac == CFW_STY_FAC_TYPE_FD))
            {
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_clckSimGetRspCB, cmd);
                return_val = CFW_SimGetFacilityLock(nFac, cmd->uti, nSim);
            }
            else
            {
                switch (nClassx)
                {
                //Specify the sum of class to represent the class information.
                //Reference to csw development document
                case 1:
                    nClassx = 11;
                    break;
                case 2:
                    nClassx = 16;
                    break;
                case 4:
                    nClassx = 13;
                    break;
                case 7:
                    nClassx = 20;
                    break;
                default:
                    break;
                }
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_clckSsQueryRspCB, cmd);
                return_val = CFW_SsQueryFacilityLock(nFac, nClassx, cmd->uti, nSim);
            }
        }
        if (return_val != ERR_SUCCESS)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, atCfwToCmeError(return_val));
        }
        if (nFac == CFW_STY_FAC_TYPE_SC)
            gLstStatusRec = 1;
        else if (nFac == CFW_STY_FAC_TYPE_FD)
            gLstStatusRec = 2;
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        const char *pResp = "+CLCK: (\"SC\",\"FD\",\"AO\",\"OX\",\"OI\")";
        atCmdRespInfoText(cmd->engine, pResp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

/*
 *******************************************************************************
 *  possible input from ATM:
 *  AT+cpin2=? -->test, return OK;
 *  AT+cpin2? --> read,wait asyncrounous event,response e.g.:
 *            --> "+CPIN: SIM PIN","+CPIN: READY"
 *  AT+cpin2="1234"(4<pinSize<8)    -->reponse:OK,ERROR
 *  AT+cpin2="12345678","1234"(pukSize=8),(4<pinSize<8) -->reponse:OK,ERROR
 *  remain problem:
 *  1.if data from UART is larger than actual buffer
 *******************************************************************************
*/
#define CFW_STY_AUTH_INITVALUE 100
#define CFW_STY_AUTH_INVALID 120
static void _cpin2ReadRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    char pResp[48] = {0};
    uint8_t iCpin2Flag = CFW_STY_AUTH_INITVALUE;

    if (0 == cfw_event->nType)
    {
        if (((uint16_t)(cfw_event->nParam2) & 0xF000) >> 12 == 0)
        {
            iCpin2Flag = CFW_STY_AUTH_PIN2BLOCK;
        }
        else if (((uint16_t)(cfw_event->nParam2) & 0xF00) >> 8 == 0)
        {
            iCpin2Flag = CFW_STY_AUTH_SIMPUK2;
        }
        else if (((uint16_t)(cfw_event->nParam2) & 0xF00) >> 8 == 3)
        {
            iCpin2Flag = CFW_STY_AUTH_READY;
        }
        else if ((((uint16_t)(cfw_event->nParam2) & 0xF00) >> 8 == 1) || (((uint16_t)(cfw_event->nParam2) & 0xF00) >> 8 == 2))
        {
            iCpin2Flag = CFW_STY_AUTH_SIMPIN2;
        }
        else
        {
            iCpin2Flag = CFW_STY_AUTH_INVALID;
        }
    }

    if ((cfw_event->nType == 0xf0) && (gAtCfwCtx.init_status == AT_MODULE_INIT_NO_SIM))
    {
        iCpin2Flag = CFW_STY_AUTH_NOSIM;
    }

    if (iCpin2Flag < CFW_STY_AUTH_INITVALUE)
    {
        sprintf(pResp, "%s: %s", cmd->desc->name, osiVsmapFindStr(gCpinStr, iCpin2Flag, ""));
        atCmdRespInfoText(cmd->engine, pResp);
        RETURN_OK(cmd->engine);
    }
    else if (iCpin2Flag > CFW_STY_AUTH_INITVALUE)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, cfw_event->nParam1);
    }
}
void atCmdHandleCPIN2(atCommand_t *cmd)
{
    uint32_t return_val;
    uint16_t i = 20;
    const char *uPin = NULL;
    uint8_t nPinSize = 0;
    const char *uNewPin = NULL;
    uint8_t nNewPinSize = 0;
    uint8_t nOption = 2;
    bool paramok = true;

    if (cmd->params == NULL)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    uint8_t nSim = atCmdGetSim(cmd->engine);
    if (gSimDropInd[nSim])
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_NOT_INSERTED);
    }

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if ((cmd->param_count != 1) && (cmd->param_count != 2))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        uPin = atParamDefStr(cmd->params[0], "", &paramok);
        nPinSize = (uint8_t)strlen(uPin);
        if (!paramok || nPinSize > 8 || nPinSize < 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        for (i = 0; i < nPinSize; i++)
        {
            if ((*(uPin + i) < '0') || (*(uPin + i) > '9'))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
        }
        if (cmd->param_count == 2)
        {
            // 4.3 update nNewPinSize for call CFW interface
            uNewPin = atParamDefStr(cmd->params[1], "", &paramok);
            nNewPinSize = (uint8_t)strlen(uNewPin);
            if (!paramok || nNewPinSize > 8 || nNewPinSize < 1)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            for (i = 0; i < nNewPinSize; i++)
            {
                if ((*(uNewPin + i) < '0') || (*(uNewPin + i) > '9'))
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }
        }

        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpinEnterRspCB, cmd);
        return_val = CFW_SimEnterAuthentication((uint8_t *)uPin, nPinSize, (uint8_t *)uNewPin,
                                                nNewPinSize, nOption, cmd->uti, nSim);
        if (ERR_SUCCESS != return_val)
        {
            if (ERR_CFW_INVALID_PARAMETER == return_val)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            else if (ERR_CME_OPERATION_NOT_ALLOWED == return_val)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else if (ERR_NO_MORE_MEMORY == return_val)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            }
        }
        gLstStatusRec = 2;
        AT_security_currentCMD[nSim] = CPIN2;
        RETURN_FOR_ASYNC();
    }
    break;

    case AT_CMD_READ:
    {
        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpin2ReadRspCB, cmd);
        return_val = CFW_SimGetAuthenticationStatus(cmd->uti, nSim);
        if (ERR_SUCCESS != return_val)
        {
            if (ERR_CFW_INVALID_PARAMETER == return_val)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            else if (ERR_CME_OPERATION_NOT_ALLOWED == return_val)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else if (ERR_NO_MORE_MEMORY == return_val)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            }
        }
        AT_security_currentCMD[nSim] = CPIN2;
        RETURN_FOR_ASYNC();
    }
    break;
    case AT_CMD_TEST:
    {
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
/*
*******************************************************************************
*  Function:  Set SIM hot-plug enable/disable
*  AT+SIMHOTSWAP=XX  ---->set,   0--disable  1--enable
*  AT+SIMHOTSWAP?    ---->read,  return whether sim hot-plug is enable or not
*******************************************************************************
*/
void atCmdHandleSIMHOTSWAP(atCommand_t *cmd)
{
    bool paramok = true;
    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        uint8_t nSlotStatus = atParamUint(cmd->params[0], &paramok);
        if (!paramok || ((nSlotStatus != 0) && (nSlotStatus != 1)))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (gSysnvSimHotPlug != (bool)nSlotStatus)
        {
#ifdef CONFIG_BOARD_SUPPORT_SIM1_DETECT
            paramok = srvSimDetectSwitch(1, (bool)nSlotStatus);
#endif
#ifdef CONFIG_BOARD_SUPPORT_SIM2_DETECT
            paramok = srvSimDetectSwitch(2, (bool)nSlotStatus);
#endif
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                gSysnvSimHotPlug = (bool)nSlotStatus;
                osiSysnvSave();
                RETURN_OK(cmd->engine);
            }
        }
        else
        {
            RETURN_OK(cmd->engine);
        }
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[20] = {
            0,
        };
        sprintf(rsp, "+SIMHOTSWAP:%d", gSysnvSimHotPlug);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
/*
*******************************************************************************
*  Function:  Set SIM hot-plug voltage trigger mode
*  AT+SIMPHASE=XX  ---->set,   0--low level(insert card) 1--high level(insert card)
*  AT+SIMPHASE?    ---->read,  return current voltage level mode
*******************************************************************************
*/
void atCmdHandleGTSET(atCommand_t *cmd)
{
    bool paramok = true;

    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        uint8_t nVoltage = atParamUint(cmd->params[0], &paramok);
        if (!paramok || ((nVoltage != 0) && (nVoltage != 1)))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (gSysnvSimVoltTrigMode != (bool)nVoltage)
        {
            gSysnvSimVoltTrigMode = (bool)nVoltage;
            osiSysnvSave();
            RETURN_OK(cmd->engine);
        }
        else
        {
            RETURN_OK(cmd->engine);
        }
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[20] = {
            0,
        };
        sprintf(rsp, "+GTSET:%d", gSysnvSimVoltTrigMode);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
#ifdef PORTING_ON_GOING
/*
*******************************************************************************
*  Write: AT+CLCK=<fac>,<mode>[,<passwd>[,<class>]]
*  Test:  AT+CLCK=?
*  possible input from ATM:
*  AT+CLCK=?            -->res:+CLCK: ("PS","PF","SC","AO","OI","OX","AI",
*                "IR","AB","AG","AC","FD","PN","PU","PP","PC")
*                OK
*  AT+CLCK="SC",1,"1234"---->reponse:OK,ERROR
*  AT+CLCK="SC",2       ---->read,res:1.ERROR(when need to enter pin),2.+clck:1(No need to enter pin)
*  AT+CLCK="SC",0,"1234"---->reponse:OK,ERROR
*******************************************************************************
*/
void AT_SIM_CmdFunc_CLCK(atCommand_t *cmd)
{
    uint32_t return_val = 0;
    uint16_t i = 20;
    uint8_t NumOfParam = 0;
    uint8_t uFAC[FAC_MAX_NUM] = {0};
    uint16_t nFac;
    uint8_t uBufPwd[CLCK_PWD_MAX] = {0};
    uint8_t nPwdSize = 0;
    uint8_t nClassx = 0;
    uint8_t nMode;

    OSI_LOGI(0x100046be, "CLCK: start\n");
    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t nUTI = pParam->nDLCI;
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (NULL == pParam->pPara)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &NumOfParam))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if ((NumOfParam != 2) && (NumOfParam != 3) && (NumOfParam != 4))
        {
            OSI_LOGI(0x100046bf, "CLCK: error num range!\n");
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        i = FAC_MAX_NUM;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, uFAC, &i))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        uint8_t FACsize = (uint8_t)AT_StrLen(uFAC);
        if (2 != FACsize)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        /***************************************************************************
            *   #define CFW_STY_FAC_TYPE_SC  0x5343 --support
            *   #define CFW_STY_FAC_TYPE_PS  0x5053
            *   #define CFW_STY_FAC_TYPE_P2  0x5032
            *   #define CFW_STY_FAC_TYPE_PF  0x5046
            *   #define CFW_STY_FAC_TYPE_PN  0x504E
            *   #define CFW_STY_FAC_TYPE_PU  0x5055
            *   #define CFW_STY_FAC_TYPE_PP  0x5050
            *   #define CFW_STY_FAC_TYPE_PC  0x5043
            *   #define CFW_STY_FAC_TYPE_FD  0x4644 --support
            ****************************************************************************/
        nFac = *(uint16_t *)uFAC;
        nFac = U16_SWAP(nFac);
        switch (nFac)
        {
        case CFW_STY_FAC_TYPE_AO:
        case CFW_STY_FAC_TYPE_OI:
        case CFW_STY_FAC_TYPE_OX:
        case CFW_STY_FAC_TYPE_AI:
        case CFW_STY_FAC_TYPE_IR:
        case CFW_STY_FAC_TYPE_AB:
        case CFW_STY_FAC_TYPE_AG:
        case CFW_STY_FAC_TYPE_AC:
        case CFW_STY_FAC_TYPE_SC:
        case CFW_STY_FAC_TYPE_PS:
        case CFW_STY_FAC_TYPE_P2:
        case CFW_STY_FAC_TYPE_PF:
        case CFW_STY_FAC_TYPE_PN:
        case CFW_STY_FAC_TYPE_PU:
        case CFW_STY_FAC_TYPE_PP:
        case CFW_STY_FAC_TYPE_PC:
        case CFW_STY_FAC_TYPE_FD:
            break;
        default:
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        }

        i = 1;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nMode, &i))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (!((((0 == nMode) || (1 == nMode)) && ((3 == NumOfParam) || (4 == NumOfParam))) || ((2 == nMode) && ((2 == NumOfParam) || (3 == NumOfParam)))))
        {
            OSI_LOGI(0x100046c0, "CLCK: error 2 para range!\n");
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (NumOfParam >= 3)
        {
            if (2 == nMode)
            {
                i = 1;
                if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, &nClassx, &i))
                {
                    OSI_LOGI(0x100046c1, "CLCK: error 3 para mode ==2!\n");
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
            }
            else
            {
                i = CLCK_PWD_MAX;
                if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, uBufPwd, &i))
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }

                nPwdSize = (uint8_t)AT_StrLen(uBufPwd);
                if ((nPwdSize > 8) || (nPwdSize < 1))
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }

                for (i = 0; i < nPwdSize; i++)
                {
                    if ((*(uBufPwd + i) < '0') || (*(uBufPwd + i) > '9'))
                    {
                        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                        return;
                    }
                }
            }
        }
        if (NumOfParam == 4)
        {
            i = 1;
            if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nClassx, &i))
            {
                OSI_LOGI(0x100046c2, "CLCK: error 4 para!\n");
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }
        }
#if SIM_DEBUG
        if (ERR_SUCCESS != AT_GetFreeUTI(CFW_SIM_SRV_ID, &nUTI))
        {
            OSI_LOGI(0x100046c3, "CLCK:NO UTI! error=0x%x", return_val);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
#else
        nUTI = SIM_UTI;
#endif
        // 6. if mode == 0 or mode == 1  call CFW interface SetFacilityLock
        if ((0 == nMode) || (1 == nMode))
        {
            if ((nFac == CFW_STY_FAC_TYPE_SC) || (nFac == CFW_STY_FAC_TYPE_FD))
            {
                /* CFW_SimSetFacilityLock(uint16_t  nFac, uint8_t * pBufPwd, uint8_t nPwdSize, uint8_t nMode, uint16_t nUTI) */
                OSI_LOGXI(OSI_LOGPAR_ISII, 0x100046c4, "Set Facility Lock nFac:0x%x,uBufPwd:%s,nPwdSize:0x%x,nMode:0x%x\n", nFac, uBufPwd, nPwdSize,
                          nMode);
                return_val = CFW_SimSetFacilityLock(nFac, uBufPwd, nPwdSize, nMode, pParam->nDLCI, nSim);
            }
            else
            {
                return_val = CFW_SsSetFacilityLock(nFac, uBufPwd, nPwdSize, nClassx, nMode, pParam->nDLCI, nSim);
            }
        }
        else
        {
            if ((nFac == CFW_STY_FAC_TYPE_SC) || (nFac == CFW_STY_FAC_TYPE_FD))
            {
                return_val = CFW_SimGetFacilityLock(nFac, pParam->nDLCI, nSim);
            }
            else
            {
                switch (nClassx)
                {
                //Specify the sum of class to represent the class information.
                //Reference to csw development document
                case 1:
                    nClassx = 11;
                    break;
                case 2:
                    nClassx = 16;
                    break;
                case 4:
                    nClassx = 13;
                    break;
                case 7:
                    nClassx = 20;
                    break;
                default:
                    break;
                }
                return_val = CFW_SsQueryFacilityLock(nFac, nClassx, pParam->nDLCI, nSim);
            }
        }
        if (ERR_SUCCESS != return_val)
        {
            if (ERR_CFW_INVALID_PARAMETER == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            else if (ERR_CME_OPERATION_NOT_ALLOWED == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, pParam->nDLCI);
            else if (ERR_NO_MORE_MEMORY == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pParam->nDLCI);
            else
                AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
            return;
        }
        if (nFac == CFW_STY_FAC_TYPE_SC)
            gLstStatusRec = 1;
        else if (nFac == CFW_STY_FAC_TYPE_FD)
            gLstStatusRec = 2;
        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, "", pParam->nDLCI);
        return;
    }
    break;

    case AT_CMD_TEST:
    {
        uint8_t *pResp = "+CLCK: (\"SC\",\"FD\",\"AO\",\"OX\",\"OI\")";
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pResp, pParam->nDLCI);
    }
    break;

    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
    } // <----------end of switch cmd  type-------
}
#endif
/*
 *******************************************************************************
 *  AT+CPWD=<fac>,<oldpwd>,<newpwd>; -->OK,ERROR
 *  AT+CPWD=<fac>,<newpwd>;
 *  AT+CPWD=?   -->+CPWD: list of supported (<fac>,<pwdlength>)s
 *  possible input from ATM:
 *  AT+CPWD=?  -->test, return OK;
 *******************************************************************************
*/
static void _cpwdRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    // EV_CFW_SIM_CHANGE_PWD_RSP
    if (cfw_event->nType == 0x0)
    {
        RETURN_OK(cmd->engine);
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

void atCmdHandleCPWD(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        uint32_t return_val;
        uint16_t i = 0; // a temp
        const char *uBufOldPwd;
        uint8_t uOldPwd[12] = {0};
        uint8_t nOldPwdSize = 0;
        const char *uBufNewPwd;
        uint8_t uNewPwd[12] = {0};
        uint8_t nNewPwdSize = 0;
        bool paramok = true;
        if ((cmd->param_count != 2) && (cmd->param_count != 3))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint16_t nFac = atParamUintByStrMap(cmd->params[0], gClckTypeVSMap, &paramok);
        if (!paramok || ((nFac != CFW_STY_FAC_TYPE_SC) && (nFac != CFW_STY_FAC_TYPE_P2)))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (cmd->param_count == 2)
        {
            uBufNewPwd = atParamOptStr(cmd->params[1], &paramok);
            nNewPwdSize = strlen(uBufNewPwd);
            if (!paramok || nNewPwdSize > 8 || nNewPwdSize < 1)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            for (i = 0; i < nNewPwdSize; i++)
            {
                if ((uBufNewPwd[i] < '0') || (uBufNewPwd[i] > '9'))
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                uNewPwd[i] = uBufNewPwd[i];
            }
        }
        else
        {
            uBufOldPwd = atParamOptStr(cmd->params[1], &paramok);
            nOldPwdSize = strlen(uBufOldPwd);
            if (!paramok || nOldPwdSize > 8 || nOldPwdSize < 1)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            for (i = 0; i < nOldPwdSize; i++)
            {
                if ((uBufOldPwd[i] < '0') || (uBufOldPwd[i] > '9'))
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                uOldPwd[i] = uBufOldPwd[i];
            }

            uBufNewPwd = atParamOptStr(cmd->params[2], &paramok);
            nNewPwdSize = strlen(uBufNewPwd);
            if (!paramok || nNewPwdSize > 8 || nNewPwdSize < 1)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            for (i = 0; i < nNewPwdSize; i++)
            {
                if ((uBufNewPwd[i] < '0') || (uBufNewPwd[i] > '9'))
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                uNewPwd[i] = uBufNewPwd[i];
            }
        }

        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpwdRspCB, cmd);
        return_val = CFW_SimChangePassword(nFac, uOldPwd, nOldPwdSize, uNewPwd, nNewPwdSize, cmd->uti, nSim);
        if (ERR_SUCCESS != return_val)
        {
            OSI_LOGI(0, "nResult = %x", return_val);
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_CFW_ERR(cmd->engine, return_val);
        }
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CPWD: (\"SC\",8),(\"P2\",8)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
} // <----------end of switch cmd  type-------

static void _cpincRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType == 0)
    {
        char nResp[30] = {0};
        sprintf(nResp, "^CPINC:%d,%d,%d,%d", ((uint16_t)cfw_event->nParam2) & 0x0F, (((uint16_t)cfw_event->nParam2) & 0xF0) >> 4,
                (((uint16_t)cfw_event->nParam2) & 0xF00) >> 8, (((uint16_t)cfw_event->nParam2) & 0xF000) >> 12);
        atCmdRespInfoText(cmd->engine, nResp);
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
}

/*
 *******************************************************************************
 *  possible input from ATM:
 *  AT^cpinc=? -->test, return OK;
 *  AT^cpinc?  --> read,wait asyncrounous event,response e.g.:
 *             --> "^SPIC: SIM PIN2"
 *  AT^cpinc   --> exe  -->reponse:  "^SPIC: 3"
 *  AT^cpinc="SC" ---we didn't support this
 *******************************************************************************
*/
void atCmdHandleCPINC(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t return_val = 0;
    if (cmd->type == AT_CMD_EXE)
    {
        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpincRspCB, cmd);
        return_val = CFW_SimGetAuthenticationStatus(cmd->uti, nSim);
        if (return_val != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_CFW_ERR(cmd->engine, return_val);
        }
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "^CPINC: PIN1&PIN2: (1-3), PUK1&PUK2: (1-10)");
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}
/*
 **************************************************************************************
 *  possible input from ATM:
 *  AT+cpinr=? -->test, return OK;
 *  AT+cpinr="sel_code" -->set, we support those sel_codes as map gCpinrStr discribled.
 *******************************************************************************
*/
typedef struct
{
    unsigned errcode;
    malSimState_t state;
    unsigned remainRetries;
    uint8_t codeType;
} atCpinrContext_t;

static const osiValueStrMap_t gCpinrStr[] = {
    {CFW_STY_AUTH_SIMPIN, "SIM PIN"},
    {CFW_STY_AUTH_SIMPUK, "SIM PUK"},
    {CFW_STY_AUTH_PHONE_TO_SIMPIN, "PH-SIM PIN"},
    {CFW_STY_AUTH_PHONE_TO_FIRST_SIMPIN, "PH-FSIM PIN"},
    {CFW_STY_AUTH_PHONE_TO_FIRST_SIMPUK, "PH-FSIM PUK"},
    {CFW_STY_AUTH_SIMPIN2, "SIM PIN2"},
    {CFW_STY_AUTH_SIMPUK2, "SIM PUK2"},
    {CFW_STY_AUTH_GENERALPIN, "SIM*"},
    {CFW_STY_AUTH_TOTALPIN, "*SIM*"},
    {0, NULL},
};

static void prvCpinrFinish(void *param)
{
    atCommand_t *cmd = (atCommand_t *)param;
    atCpinrContext_t *ctx = (atCpinrContext_t *)cmd->async_ctx;

    if (ctx->errcode != 0)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    if (ctx->state == CFW_STY_AUTH_NOSIM)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_NOT_INSERTED);
    }
    const char *str = osiVsmapFindStr(gCpinrStr, ctx->codeType, NULL);
    if (str == NULL)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    char rsp[50] = {0};
    OSI_LOGI(0, "remainRetries = %x", ctx->remainRetries);
    switch (ctx->codeType)
    {
    case CFW_STY_AUTH_SIMPIN:
        sprintf(rsp, "%s: %s,%d,3", cmd->desc->name, str, ctx->remainRetries & 0x000F); //low 4 bits of low bytes
        break;
    case CFW_STY_AUTH_SIMPUK:
        sprintf(rsp, "%s: %s,%d,10", cmd->desc->name, str, ((ctx->remainRetries & 0x00F0) >> 4)); //low 4 bits of low bytes
        break;
    case CFW_STY_AUTH_SIMPIN2:
        sprintf(rsp, "%s: %s,%d,3", cmd->desc->name, str, ((ctx->remainRetries & 0x0F00) >> 8)); //low 4 bits of low bytes
        break;
    case CFW_STY_AUTH_SIMPUK2:
        sprintf(rsp, "%s: %s,%d,10", cmd->desc->name, str, ((ctx->remainRetries & 0xF000) >> 12)); //low 4 bits of low bytes
        break;
    case CFW_STY_AUTH_GENERALPIN:
    case CFW_STY_AUTH_TOTALPIN:
        sprintf(rsp, "%s: %s,%d,3\n", cmd->desc->name, "SIM PIN", ctx->remainRetries & 0x000F);
        atCmdRespInfoText(cmd->engine, rsp);
        memset(rsp, 0, sizeof(rsp));
        sprintf(rsp, "%s: %s,%d,10\n", cmd->desc->name, "SIM PUK", ((ctx->remainRetries & 0x00F0) >> 4));
        atCmdRespInfoText(cmd->engine, rsp);
        memset(rsp, 0, sizeof(rsp));
        sprintf(rsp, "%s: %s,%d,3\n", cmd->desc->name, "SIM PIN2", ((ctx->remainRetries & 0x0F00) >> 8));
        atCmdRespInfoText(cmd->engine, rsp);
        memset(rsp, 0, sizeof(rsp));
        sprintf(rsp, "%s: %s,%d,10", cmd->desc->name, "SIM PUK2", ((ctx->remainRetries & 0xF000) >> 12));
        break;
    }
    atCmdRespInfoText(cmd->engine, rsp);
    RETURN_OK(cmd->engine);
}
static void prvCpinrCall(void *param)
{
    atCommand_t *cmd = (atCommand_t *)param;
    atCpinrContext_t *ctx = (atCpinrContext_t *)cmd->async_ctx;
    ctx->errcode = malSimGetState(atCmdGetSim(cmd->engine), &ctx->state, &ctx->remainRetries);
    atEngineCallback(prvCpinrFinish, cmd);
}
void atCmdHandleCPINR(atCommand_t *cmd)
{
    bool paramok = true;
    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        uint32_t nCodeType = atParamUintByStrMap(cmd->params[0], gCpinrStr, &paramok);
        if ((!paramok) || (nCodeType == 0))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if ((nCodeType == CFW_STY_AUTH_PHONE_TO_SIMPIN) || (nCodeType == CFW_STY_AUTH_PHONE_TO_FIRST_SIMPIN) || (nCodeType == CFW_STY_AUTH_PHONE_TO_FIRST_SIMPUK))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        }

        atCpinrContext_t *ctx = (atCpinrContext_t *)calloc(1, sizeof(*ctx));
        if (ctx == NULL)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        }
        ctx->codeType = nCodeType;
        cmd->async_ctx = ctx;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;

        atCmdWorkerCall(cmd->engine, prvCpinrCall, cmd);
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}
void atCmdHandleSIMCROSS(atCommand_t *cmd)
{
    uint8_t nSwitchSimID = 0;
    OSI_LOGI(0, "SIMCROSS, start!\n");

    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t nSwitchSimID = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        CFW_CfgSetSimSwitch(nSwitchSimID);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char refstr[20] = {0};
        CFW_CfgGetSimSwitch(&nSwitchSimID);
        sprintf(refstr, "+SIMCROSS:%d", nSwitchSimID);
        atCmdRespInfoText(cmd->engine, refstr);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char refstr[20] = {0};
        sprintf(refstr, "+SIMCROSS:(0,1)");
        atCmdRespInfoText(cmd->engine, refstr);
        atCmdRespOK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

static void _csimRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    OSI_LOGI(0, "_csimRspCB: nType: %X", cfw_event->nType);

    if (cfw_event->nType == 0)
    {
        uint16_t length = cfw_event->nParam2;
        uint8_t *TPDU = (uint8_t *)cfw_event->nParam1;

        if (length > 0)
            atMemFreeLater((void *)cfw_event->nParam1);
        if (length > 260)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        else if (length == 0)
        {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
			char response[18] = {0};
			sprintf(response, "%s: 4,\"%lX\"", cmd->desc->name, cfw_event->nParam1);
#else
            char response[16] = {0};
            sprintf(response, "%s: 4,%lX", cmd->desc->name, cfw_event->nParam1);
#endif
            atCmdRespInfoText(cmd->engine, response);
            atCmdRespOK(cmd->engine);
        }
        else
        {
            uint16_t sw = cfw_event->nParam2 >> 16;
            uint16_t len = (cfw_event->nParam2 & 0xFFFF) << 1;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
			len += 12 + 4 + 1 + 2;
#else
            len += 12 + 4 + 1; // add prefix and length, sw1 and sw2, terminal zero
#endif
            char *response = malloc(len);
            if (response == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

            memset(response, 0, len);
            atMemFreeLater(response);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
			uint16_t i = sprintf(response, "%s: %d,\"", cmd->desc->name, (length + 2) << 1);
			length = cfwBytesToHexStr(TPDU, length, response + i);
			sprintf(response + i + length, "\"");
#else
            uint16_t i = sprintf(response, "%s: %d,", cmd->desc->name, (length + 2) << 1);
            length = cfwBytesToHexStr(TPDU, length, response + i);
#endif
            if (length == 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            i += sprintf(response + i + length, "%X", sw);

            atCmdRespInfoText(cmd->engine, response);
            atCmdRespOK(cmd->engine);
        }
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

void atCmdHandleCSIM(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        bool paramok = true;
        uint16_t length = atParamUint(cmd->params[0], &paramok);
        const char *command = atParamStr(cmd->params[1], &paramok);
        if ((cmd->param_count != 2) || !paramok || (strlen(command) != length))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if ((length > 520) || (length < 10) || (length % 2))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t tpdu[265] = {0};
        int32_t retval = cfwHexStrToBytes(command, length, tpdu);
        if (retval == 0 || retval == -1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        OSI_LOGXI(OSI_LOGPAR_M, 0, "AT CSIM send %*s", retval, tpdu);

        cmd->uti = cfwRequestUTI((osiEventCallback_t)_csimRspCB, cmd);
        uint8_t channel = tpdu[0] & 0x0F;
        retval = CFW_SimTPDUCommand(tpdu, retval, channel, cmd->uti, nSim);
        if (ERR_SUCCESS != retval)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
            RETURN_FOR_ASYNC();
    }
    case AT_CMD_TEST:
        RETURN_OK(cmd->engine);
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
/*
 *******************************************************************************
 *  #define EV_CFW_SIM_CHANGE_PWD_RSP        (EV_CFW_SIM_RSP_BASE+8)
 *  #define EV_CFW_SIM_GET_AUTH_STATUS_RSP       (EV_CFW_SIM_RSP_BASE+9)
 *  #define EV_CFW_SIM_ENTER_AUTH_RSP        (EV_CFW_SIM_RSP_BASE+11)
 *  #define EV_CFW_SIM_SET_FACILITY_LOCK_RSP     (EV_CFW_SIM_RSP_BASE+12)
 *  #define EV_CFW_SIM_GET_FACILITY_LOCK_RSP     (EV_CFW_SIM_RSP_BASE+13)
 *******************************************************************************
*/
#ifdef PORTING_ON_GOING
void AT_SIM_AsyncEventProcess(COS_EVENT *pEvent)
{
    CFW_EVENT CFWev;
    CFW_EVENT *pcfwEv = &CFWev;
    AT_CosEvent2CfwEvent(pEvent, pcfwEv);
    PAT_CMD_RESULT pResult = NULL;
    uint8_t sStr[10] = {0};
    uint8_t nSim = cfw_event->nFlag;

    OSI_LOGI(0x100046cb, "---- event id = %x,param1 = %x,param3 = %x,nSim = %d", pEvent->nEventId, pEvent->nParam1, pEvent->nParam3, nSim);
    if (cfw_event->nParam1 == PINNEEDPUK)
    {
        g_Pin1PukStauts[nSim] = 1;
    }
    else
    {
        g_Pin1PukStauts[nSim] = 0;
    }

    switch (cfw_event->nEventId)
    {
    case EV_CFW_SIM_SET_ACMMAX_RSP:
        if (AT_IsAsynCmdAvailable("+CAMM", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            if (cfw_event->nType == 0)
            {
                if (cfw_event->nParam1 == ERR_SUCCESS)
                    AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", cfw_event->nUTI);
                else
                    AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, cfw_event->nUTI);
            }
            else
            {
                AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, cfw_event->nUTI);
            }
        }

        AT_ZERO_PARAM1(pEvent);
        break;

    case EV_CFW_SIM_GET_ACMMAX_RSP:
        if (AT_IsAsynCmdAvailable("+CAMM", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            if (cfw_event->nType == 0)
            {
                uint8_t *pData = (uint8_t *)&cfw_event->nParam1;
                sprintf(sStr, "+CAMM:\"%02X%02X%02X\"", pData[0], pData[1], pData[2]);
                AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, sStr, cfw_event->nUTI);
            }
            else
            {
                AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, cfw_event->nUTI);
            }
        }
        break;

    case EV_CFW_SIM_ENTER_AUTH_RSP:
    {
        if (CPIN == AT_security_currentCMD[nSim])
        {
            AT_security_currentCMD[nSim] = 0;
        }
        else if (CPIN2 == AT_security_currentCMD[nSim])
        {
            AT_security_currentCMD[nSim] = 0;

            if (AT_IsAsynCmdAvailable("+CPIN2", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
            {
                AT_SIM_CmdFunc_CPIN2_Set_AsyncEventProcess(pcfwEv, pResult);
            }
        }

        if (CFWev.nType != 0)
            AT_ZERO_PARAM1(pEvent);
        break;
    }
    /**********************************************
     * AT+cpin? -->:+CPIN: READY,+CPIN: SIM PIN  *
     * AT+cpin2?-->:+CPIN2: SIM PUK2                 *
     * AT^cpinc -->:^SPIC: 2                         *
     * AT^cpinc?-->:^cpinc:SIM PUK(not supported) *
     **********************************************/
    case EV_CFW_SIM_GET_AUTH_STATUS_RSP:
    {
        switch (AT_security_currentCMD[nSim])
        {
        case CPIN:
        {
            AT_security_currentCMD[nSim] = 0;
        }

        break;

        case CPIN2:
        {
            AT_security_currentCMD[nSim] = 0;
            if (AT_IsAsynCmdAvailable("+CPIN2", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
            {
                AT_SIM_CmdFunc_CPIN2_Read_AsyncEventProcess(pcfwEv, pResult);
            }
        }
        break;

        case CPINCEXE:
        {
            AT_security_currentCMD[nSim] = 0;
            if (AT_IsAsynCmdAvailable("^CPINC", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
            {
                AT_SIM_CmdFunc_CPINC_Exe_AsyncEventProcess(pcfwEv, pResult);
            }
        }
        break;

        default:
        {
            OSI_LOGI(0x100046cc, "no wait RSP ignore EV_CFW_SIM_GET_AUTH_STATUS_RSP");
            break;
        }
        }

        AT_ZERO_PARAM1(pEvent);
        break;
    }
    case EV_CFW_SIM_CHANGE_PWD_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CPWD", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            AT_SIM_CmdFunc_CPWD_Set_AsyncEventProcess(pcfwEv, pResult);
        }
    }
    break;

    case EV_CFW_SIM_SET_FACILITY_LOCK_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CLCK", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            AT_SIM_CmdFunc_CLCK_Set_AsyncEventProcess(pcfwEv, pResult);
        }
    }
    break;

    case EV_CFW_SIM_GET_FACILITY_LOCK_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CLCK", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            AT_SIM_CmdFunc_CLCK_Read_AsyncEventProcess(pcfwEv, pResult);
        }
    }
    break;

    case EV_CFW_SIM_READ_ELEMENTARY_RSP:
    {
        if (CFWev.nType == 0)
        {
            qgid_file_size = CFWev.nParam1;

            OSI_LOGI(0x100046cd, "GID: qgid_file_size = %d", qgid_file_size);
            uint32_t nResult = CFW_SimReadBinary(qgid_file_id[0], 0, qgid_file_size, CFWev.nUTI, nSim);
            if (nResult != ERR_SUCCESS)
            {
                OSI_LOGI(0x100046ce, "GID1: nResult = %x", nResult);
                AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, CFWev.nUTI);
            }
        }
        else
            AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, CFWev.nUTI);
        return;
    }
    case EV_CFW_SIM_READ_BINARY_RSP:
    case EV_CFW_SIM_UPDATE_BINARY_RSP:
    case EV_CFW_SIM_READ_RECORD_RSP:
    case EV_CFW_SIM_UPDATE_RECORD_RSP:
    case EV_CFW_SIM_GET_FILE_STATUS_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CRSM", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            AT_SIM_CmdFunc_CRSM_AsyncEventProcess(pcfwEv, pResult);
        }
        else if (AT_IsAsynCmdAvailable("+CRSML", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            AT_SIM_CmdFunc_CRSML_AsyncEventProcess(pcfwEv, pResult);
        }
        else if (cfw_event->nEventId == EV_CFW_SIM_READ_BINARY_RSP)
        {
            OSI_LOGI(0x100046cf, "=====EV_CFW_SIM_READ_BINARY_RSP=====");
            if (AT_IsAsynCmdAvailable("+QSPN", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
            {
                AT_SIM_CmdFunc_QSPN_AsyncEventProcess(pcfwEv, pResult);
            }
            else if (AT_IsAsynCmdAvailable("+QGID", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
            {
                AT_SIM_CmdFunc_QGID_AsyncEventProcess(pcfwEv, pResult);
            }
        }

        if (CFWev.nType != 0)
            AT_ZERO_PARAM1(pEvent);
    }
    break;

    case EV_CFW_SIM_RESET_RSP:
    {
        OSI_LOGI(0x100046d0, "EV_CFW_SIM_RESET_RSP asyn event nType=%d,nParam1=%d", cfw_event->nType, cfw_event->nParam1);
        if (cfw_event->nType == 0)
            AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", cfw_event->nUTI);
        else
            AT_SIM_Result_Err(CMD_RC_SIMDROP, cfw_event->nUTI);
    }
    break;
    default:
    {
        break;
    }
    }
}

void AT_SIM_CmdFunc_CPIN2_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    uint32_t retErrCode = 50;
#if SIM_DEBUG
    OSI_LOGI(0x100046d1, "CPIN asyn event nType=%d,nParam1=%d", cfw_event->nType, cfw_event->nParam1);
#endif

    if (0 == cfw_event->nType && 0 == cfw_event->nParam1)
    {
        // end ok yaoal changed it //&& 0 == cfw_event->nParam1//
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", cfw_event->nUTI);
    }
    else
    {
        // end error
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(cfw_event->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, cfw_event->nUTI);
    }
}

/*************************************************************
* in 27007,this command is not included
* so i do the same as at55 does
* if nType == 0, i will return OK,no matter nParam1'value is
*************************************************************/
void AT_SIM_CmdFunc_CPIN2_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    uint32_t retErrCode = 50;

    if (0 == cfw_event->nType)
    {
        uint8_t *pResp = NULL;
        if (CFW_STY_AUTH_NOSIM == cfw_event->nParam1)
            pResp = "+CPIN2:NO SIM";
        else if ((LOUINT16(cfw_event->nParam2) & 0xF000) >> 12 == 0)
            pResp = "+CPIN2:PIN2 BLOCK";
        else if ((LOUINT16(cfw_event->nParam2) & 0xF00) >> 8 == 0)
            pResp = "+CPIN2:SIM PUK2";
        else if ((LOUINT16(cfw_event->nParam2) & 0xF00) >> 8 == 3)
            pResp = "+CPIN2: READY";
        else if ((LOUINT16(cfw_event->nParam2) & 0xF00) >> 8 != 0)
            pResp = "+CPIN2:SIM PIN2";
        else
        {
            AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, cfw_event->nUTI);
            return;
        }
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pResp, cfw_event->nUTI);
    }
    else
    {
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(cfw_event->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, cfw_event->nUTI);
    }
}

/*************************************************************
*   OK or FAIL
*************************************************************/
void AT_SIM_CmdFunc_CPWD_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    uint32_t retErrCode = 50;

    if (0 == cfw_event->nType)
    {
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", cfw_event->nUTI);
    }
    else if (0xF0 == cfw_event->nType)
    {
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(cfw_event->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, cfw_event->nUTI);
    }
    else
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, cfw_event->nUTI);
    }
}

/*************************************************************
*   OK or FAIL
*************************************************************/
void AT_SIM_CmdFunc_CLCK_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    uint32_t retErrCode = 50;
    uint8_t nSim = cfw_event->nFlag;
    if (0 == cfw_event->nType)
    {
#ifndef AT_WITHOUT_PBK
        AT_FDN_SetStatus(nSim);
#endif
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", cfw_event->nUTI);
    }
    else if (0xF0 == cfw_event->nType)
    {
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(cfw_event->nParam1, 1);
        if (ERR_AT_CME_SIM_FAILURE == retErrCode)
        {
            if (CFW_STY_FAC_TYPE_FD == cfw_event->nParam2)
            {
                AT_SIM_Result_Err(ERR_AT_CME_SIM_PUK2_REQUIRED, cfw_event->nUTI);
            }
        }
        else
            AT_SIM_Result_Err(retErrCode, cfw_event->nUTI);
    }
    else
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, cfw_event->nUTI);
    }
}

void AT_SIM_CmdFunc_CLCK_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    uint32_t retErrCode = 50;
    if (0 == cfw_event->nType)
    {
        uint8_t nResp[10] = {0};
        sprintf(nResp, "+CLCK:%d", LOUINT8(cfw_event->nParam1));
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nResp, cfw_event->nUTI);
    }
    else if (0xF0 == cfw_event->nType)
    {
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(cfw_event->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, cfw_event->nUTI);
    }
    else
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, cfw_event->nUTI);
    }
}

/*************************************************************
* in 27007,this command is not included
* so i do the same as at55 does
* if nType == 0, i will return OK,no matter nParam1'value is
*************************************************************/
void AT_SIM_CmdFunc_CPINC_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    uint32_t retErrCode = 50;
    if (0 == cfw_event->nType)
    {
        // end ok
        // check value if in a valued range.
        switch (cfw_event->nParam1)
        {
        case CFW_STY_AUTH_READY:
        case CFW_STY_AUTH_PIN1_READY:
        case CFW_STY_AUTH_SIMPIN:
        case CFW_STY_AUTH_SIMPUK:
        case CFW_STY_AUTH_SIMPIN2:
        case CFW_STY_AUTH_SIMPUK2:
        case CFW_STY_AUTH_PHONE_TO_SIMPIN:
        case CFW_STY_AUTH_NETWORKPUK:
        case CFW_STY_AUTH_PIN1_DISABLE:
            break;

        default:
        {
            AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", cfw_event->nUTI);
            return;
        }
        }
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, cpinasynSTR[cfw_event->nParam1], cfw_event->nUTI);
    }
    else
    {
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(cfw_event->nParam1, 1);

        AT_SIM_Result_Err(retErrCode, cfw_event->nUTI);
    }
}

extern CFW_SIMSTATUS g_cfw_sim_status[];
typedef struct _CFW_UsimEfStatus
{
    uint8_t efStructure; // 1:Transparent ; 2: Linear Fixed ; 6: Cyclic
    INT16 fileId;
    INT16 fileSize;
    uint8_t recordLength;
    uint8_t numberOfRecords;
} CFW_UsimEfStatus;

void AT_SIM_CmdFunc_QGID_AsyncEventProcess(CFW_EVENT *CFWev, PAT_CMD_RESULT pResult)
{
    uint8_t nSim = CFWev->nFlag;
    if (CFWev->nType == 0)
    {
        if (qgid_gid[0] == 0)
        {
            qgid_gid[0] = AT_MALLOC(qgid_file_size << 1);
            if (qgid_gid[0] == NULL)
            {
                AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, CFWev->nUTI);
                return;
            }
            qgid_gid[1] = qgid_gid[0] + qgid_file_size;

            OSI_LOGI(0x100046d6, "=========GID1=========");
            memcpy(qgid_gid[0], (uint8_t *)CFWev->nParam1, qgid_file_size);
            CSW_TC_MEMBLOCK(g_sw_AT_SIM, qgid_gid[0], 10, 16);
            uint32_t nResult = CFW_SimReadBinary(qgid_file_id[1], 0, qgid_file_size, CFWev->nUTI, nSim);
            if (nResult != ERR_SUCCESS)
            {
                OSI_LOGI(0x100046d7, "GID2: nResult = %x", nResult);
                AT_FREE(qgid_gid[0]);
                AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, CFWev->nUTI);
            }
        }
        else
        {
            OSI_LOGI(0x100046d8, "=========GID2=========");
            memcpy(qgid_gid[1], (uint8_t *)CFWev->nParam1, qgid_file_size);
            CSW_TC_MEMBLOCK(g_sw_AT_SIM, qgid_gid[1], 10, 16);
            uint8_t nResponse[255] = {0};
            uint8_t *prefix = "+QGID: ";
            uint8_t i = strlen(prefix);
            memcpy(nResponse, prefix, i);
            OSI_LOGI(0x100046d9, "=========1========i=%d, %d", i, qgid_file_size);
            CSW_TC_MEMBLOCK(g_sw_AT_SIM, nResponse, 30, 16);

            i += SUL_hex2ascii(qgid_gid[0], qgid_file_size, nResponse + i);
            //memcpy(nResponse + i, qgid_gid[0], qgid_file_size);

            CSW_TC_MEMBLOCK(g_sw_AT_SIM, nResponse, 50, 16);
            OSI_LOGI(0x100046da, "=========2========i=%d, %d", i, qgid_file_size);
            //i += qgid_file_size;

            OSI_LOGI(0x100046db, "=========x======i=%d, %d", i, qgid_file_size);
            nResponse[i] = ',';
            i++;
            i += SUL_hex2ascii(qgid_gid[1], qgid_file_size, nResponse + i);
            //memcpy(nResponse + i, qgid_gid[1], qgid_file_size);
            OSI_LOGI(0x100046dc, "=========3========i=%d,%d", i, qgid_file_size);
            CSW_TC_MEMBLOCK(g_sw_AT_SIM, nResponse, 50, 16);
            AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nResponse, CFWev->nUTI);
            AT_FREE(qgid_gid[0]);
        }
    }
    else
    {
        if (qgid_gid[0] != 0)
            AT_FREE(qgid_gid[0]);
        AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, CFWev->nUTI);
    }
    return;
}

void AT_SIM_CmdFunc_QSPN_AsyncEventProcess(CFW_EVENT *CFWev, PAT_CMD_RESULT pResult)
{
    OSI_LOGI(0x100046dd, "=====QSPN===CFWev.nType=%d=", CFWev->nType);
    uint8_t nSim = CFWev->nFlag;
    if (CFWev->nType == 0)
    {
        uint8_t *nData = (uint8_t *)CFWev->nParam1;
        CSW_TC_MEMBLOCK(g_sw_AT_SIM, nData, 17, 16);
        uint8_t i = 0;
        for (i = 0; i < 17; i++)
        {
            if (nData[i] == 0xFF)
            {
                nData[i] = 0;
                break;
            }
        }
        i--;
        //i hold the length of SPN
        uint8_t nSPN[17] = {0};
        uint8_t nShow = nData[0] & 0x01;
        uint8_t num = 0;
        if (nData[1] == 0x80)
        {
            OSI_LOGI(0x100046de, "Chinese charactor, i = %d", i);
            num = unicode2gbk(nSPN, nData + 2, i - 1); //skip 0x80
            if (num == 0)
                OSI_LOGI(0x100046df, "Convert failed!");
            i = num;
        }
        else if (nData[1] > 0x80) //(nData[1] == 0x81) && (nData[1] == 0x82))
        {
            if (i - 1 > 8)
                i = 8;
            else
                i--;
            SUL_hex2ascii(nData + 2, i, nSPN);
        }
        else
            memcpy(nSPN, nData + 1, i);
        CSW_TC_MEMBLOCK(g_sw_AT_SIM, nSPN, i, 16);
        uint8_t nResponse[30] = {0};
        sprintf(nResponse, "+QSPN: %d, %s", nShow, nSPN);
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nResponse, CFWev->nUTI);
    }
    else
        AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, CFWev->nUTI);
    return;
}

extern void CFW_UsimDecodeEFFcp(uint8_t *RespData, CFW_UsimEfStatus *pSimEfStatus);

/*************************************************************************************
This function is used to tackle bug8124 and bug8310
When receive async event about CPINC, SIM event process modle will call this function
This function is added by wangxd
*************************************************************************************/
void AT_SIM_CmdFunc_CPINC_Exe_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    uint32_t retErrCode = 50;
#if SIM_DEBUG
    OSI_LOGI(0x100052c8, "cfw_event->nType == %d, cfw_event->nParam1 == 0x%x", cfw_event->nType, cfw_event->nParam1);
    OSI_LOGI(0x100046e2, "gLstStatusRec == %d", gLstStatusRec);
    OSI_LOGI(0x100052c9, "CPINC_Event cfw_event->nParam1 = %d", cfw_event->nParam1);
#endif

    if (cfw_event->nType == 0)
    {
        uint8_t nResp[20] = {0};
        sprintf(nResp, "^CPINC:%d,%d,%d,%d", LOUINT16(cfw_event->nParam2) & 0x0F, (LOUINT16(cfw_event->nParam2) & 0xF0) >> 4,
                (LOUINT16(cfw_event->nParam2) & 0xF00) >> 8, (LOUINT16(cfw_event->nParam2) & 0xF000) >> 12);

        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nResp, cfw_event->nUTI);
    }
    else
    {
        retErrCode = AT_SetCmeErrorCode(cfw_event->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, cfw_event->nUTI);
    }
}

void AT_SIM_CmdFunc_SETSIM(atCommand_t *cmd)
{
    uint8_t paraCount = 0;
    INT32 iRet = 0;
    uint16_t nTypSize = 0;
    uint8_t nIndex = 0;
    uint8_t nSim = 0;

    OSI_LOGI(0x100046e4, "AT+SIM:CmdFunc_SETSIM ");
    if (pParam == NULL)
    {
        OSI_LOGI(0x100046e5, "AT+SIM:pParam == NULL !");
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    switch (cmd->type)
    {
    case AT_CMD_SET:
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 1))
        {
            OSI_LOGI(0x100046e6, "AT+SIM:paraCount: %d", paraCount);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        nTypSize = sizeof(nSim);
        iRet = AT_Util_GetParaWithRule(pParam->pPara, nIndex, AT_UTIL_PARA_TYPE_UINT8, &nSim, &nTypSize);
        if (iRet != ERR_SUCCESS)
        {
            OSI_LOGI(0x100046e7, "AT+SIM:iRet : %d !", iRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        if (nSim < 5)
        {
            gAtSimID = nSim;
            OSI_LOGI(0x100046e8, "From SIM:%d", nSim);
        }
        else
        {
            OSI_LOGI(0x100046e7, "AT+SIM:iRet : %d !", iRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", pParam->nDLCI);
        break;

    case AT_CMD_READ:
        break;

    case AT_CMD_TEST:
        break;

    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, pParam->nDLCI);
        break;
    }
    return;
}

/*
 *******************************************************************************
 *  possible input from ATM:
 *  AT^creset=? -->test,    return OK;
 *  AT^creset   --> exe -->reponse:  "OK" && "+CME ERROR:11"
 *******************************************************************************
*/
void AT_SIM_CmdFunc_CRESET(atCommand_t *cmd)
{
    uint32_t return_val;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    uint8_t nSim = atCmdGetSim(cmd->engine);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    switch (cmd->type)
    {
    case AT_CMD_EXE:
        return_val = CFW_SimReset(pParam->nDLCI, nSim);
        if (ERR_SUCCESS != return_val)
        {
            if (ERR_NO_MORE_MEMORY == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pParam->nDLCI);
            else
                AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
            return;
        }

        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, "", pParam->nDLCI);
        break;

    case AT_CMD_TEST:
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "^CRESET: ", pParam->nDLCI);
    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
    }
}

/*
 ========================================================================================================
 *  possible input from ATM:
 *  AT+CSST=? -->test,  return OK;
 *  AT+CSST=n   --> set -->reponse:  "service xxx allocated ( not allocated ) and activated(not activated)"
 ========================================================================================================
*/
const uint8_t gSSTServicesName[][50] =
    {
        "CHV1disablefunction",
        "AbbreviatedDiallingNumbers(ADN)",
        "FixedDiallingNumbers(FDN)",
        "ShortMessageStorage(SMS)",
        "AdviceofCharge(AoC)",
        "CapabilityConfigurationParameters(CCP)",
        "PLMNselector",
        "RFU",
        "MSISDN",
        "Extension1",
        "Extension2",
        "SMSParameters",
        "LastNumberDialled(LND)",
        "CellBroadcastMessageIdentifier",
        "GroupIdentifierLevel1",
        "GroupIdentifierLevel2",
        "ServiceProviderName",
        "ServiceDiallingNumbers(SDN)",
        "Extension3",
        "RFU",
        "VGCSGroupIdentifierList(EFVGCSandEFVGCSS)",
        "VBSGroupIdentifierList(EFVBSandEFVBSS)",
        "enhancedMultiLevelPrecedenceandPreemptionService",
        "AutomaticAnswerforeMLPP",
        "DatadownloadviaSMSCB",
        "DatadownloadviaSMSPP",
        "Menuselection",
        "Callcontrol",
        "ProactiveSIM",
        "CellBroadcastMessageIdentifierRanges",
        "BarredDiallingNumbers(BDN)",
        "Extension4",
        "DepersonalizationControlKeys",
        "CooperativeNetworkList",
        "ShortMessageStatusReports",
        "Network'sindicationofalertingintheMS",
        "MobileOriginatedShortMessagecontrolbySIM",
        "GPRS",
        "Image(IMG)",
        "SoLSA(SupportofLocalServiceArea)",
        "USSDstringdataobjectsupportedinCallControl",
        "RUNATCOMMANDcommand",
        "UsercontrolledPLMNSelectorwithAccessTechnology",
        "OperatorcontrolledPLMNSelectorwithAccessTechnology",
        "HPLMNSelectorwithAccessTechnology",
        "CPBCCHInformation",
        "InvestigationScan",
        "ExtendedCapabilityConfigurationParameters",
        "MExE",
        "RPLMNlastusedAccessTechnology",
};

void AT_SIM_CmdFunc_CSST(atCommand_t *cmd)
{
    uint32_t return_val;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    uint8_t nSim = atCmdGetSim(cmd->engine);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        uint8_t paraCount = 0;
        uint8_t nService = 0;
        uint16_t nSize = 0x01;

        INT32 iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 1))
        {
            OSI_LOGI(0x100046e9, "AT+CSST:paraCount: %d", paraCount);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nService, &nSize);
        if (iRet != ERR_SUCCESS)
        {
            OSI_LOGI(0x100046ea, "AT+CSST:iRet : %d !", iRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        return_val = CFW_GetSSTStatus(nService, nSim);
        uint8_t nOutString[300] = {
            0x00,
        };
        sprintf(nOutString, "Services %s ", gSSTServicesName[nService - 1]);

        if (return_val & 0x01)
            SUL_StrCat(nOutString, " < allocated > ");
        else
            SUL_StrCat(nOutString, " <  not allocated > ");

        if (return_val & 0x02)
            SUL_StrCat(nOutString, " <  activated > ");
        else
            SUL_StrCat(nOutString, " <  not activated > ");
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nOutString, pParam->nDLCI);
    }
    break;
    case AT_CMD_TEST:
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "+CSST: ", pParam->nDLCI);
    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
    }
}

void AT_SIM_CmdFunc_SIM(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    AT_ASSERT(NULL != pParam);
    OSI_LOGI(0x100052ca, "AT_SIM_CmdFunc_SIM : cmd->type = %d", cmd->type);

    if (cmd->type == AT_CMD_TEST)
    {
        uint8_t *pRsp = "+SIM: (NORMAL,ABSENT,ABNORMAL)";
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pRsp, pParam->nDLCI);
        return;
    }

    else if (cmd->type == AT_CMD_EXE)
    {

        if (CFW_GetSimStatus(nSim) == 0x01)
        {
            uint8_t *pRsp = AT_MALLOC(20);

            memset(pRsp, 0x00, 20);

            sprintf(pRsp, "+SIM");
            AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pRsp, pParam->nDLCI);

            AT_FREE(pRsp);
        }
        else
        {
            AT_SIM_Result_Err(ERR_AT_CME_SIM_FAILURE, pParam->nDLCI);
        }
        return;
    }
    else if (cmd->type == AT_CMD_SET)
    {
        uint8_t paraCount = 0;
        uint8_t nIndex = 0;
        uint8_t nSimID = 0;
        INT32 iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 1))
        {
            OSI_LOGI(0x100046e6, "AT+SIM:paraCount: %d", paraCount);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        uint16_t nTypSize = 1;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, nIndex, AT_UTIL_PARA_TYPE_UINT8, &nSimID, &nTypSize);
        if (iRet != ERR_SUCCESS)
        {
            OSI_LOGI(0x100046e7, "AT+SIM:iRet : %d !", iRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        if (nSimID < CFW_SIM_COUNT)
        {
            OSI_LOGI(0x100046e8, "From SIM:%d", nSim);
            uint8_t nStatus = CFW_GetSimStatus(nSimID);
            uint8_t *pRsp = NULL;
            if (0x01 == nStatus)
                pRsp = "+SIM: NORMAL";
            else if (0x00 == nStatus)
                pRsp = "+SIM: ABSENT";
            else
                pRsp = "+SIM: ABNORMAL";
            AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pRsp, pParam->nDLCI);
        }
        else
        {
            OSI_LOGI(0x100046e7, "AT+SIM:iRet : %d !", iRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        return;
    }
    else
    {
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
        return;
    }
}

#if defined(SIMCARD_HOT_DETECT) && (SIMCARD_HOT_DETECT > NUMBER_OF_SIM || SIMCARD_HOT_DETECT < 0)
#error "SIMCARD_HOT_DETECT MUST be [1..NUMBER_OF_SIM]"
#endif
#ifdef SIMCARD_HOT_DETECT
static uint8_t gbATSimDetectInit = FALSE;

void AT_SimDetectStartSim()
{
    if (pmd_GetSimDetectStatus())
    {
        hal_HstSendEvent(0x6007101d);
        gbATSimDetectInit = TRUE;
        CFW_SimInit(1, SIMCARD_HOT_DETECT - 1);
    }
    else
    {
        gSimDropInd[SIMCARD_HOT_DETECT - 1] = TRUE;
        CFW_Exit(2);
    }
}
void AT_SimCardDetectGpioInit(void)
{
    pmd_SetSimDetectHandler(AT_SimDetectStartSim);
}
uint8_t AT_SimCardDetectIsInit(void)
{
    return gbATSimDetectInit;
}

#endif
#endif

struct
{
    uint8_t aid[20];
    uint8_t aid_len;
    uint8_t sim_channel;
    uint8_t channel_status;
} at_sim_session[8] = {
    {{0}, 0, 0, 0},
};
static uint8_t at_session = 0;

uint8_t getSession(uint8_t sim_channel)
{
    if (sim_channel > 8)
        return 0xFF;
    else
    {
        uint8_t i = 0;
        for (i = 0; i < 8; i++)
        {
            if (at_sim_session[i].sim_channel == sim_channel)
                return i;
        }
        return 0xFF;
    }
}
uint8_t getFreeSession()
{
    uint8_t i = 0;
    for (i = 0; i < 8; i++)
    {
        if (at_sim_session[i].aid_len == 0)
            return i;
    }
    return 0xFF;
}

uint16_t ascii2hex(const char *pInput, uint16_t length, uint8_t *pOutput)
{
    uint16_t j = length;

    uint8_t nData;
    uint8_t *pBuffer = pOutput;
    const char *pIn = pInput;

    if ((length & 0x01) == 0x01)
    {
        nData = pIn[0];
        if ((nData >= '0') && (nData <= '9'))
            *pBuffer = nData - '0';
        else if ((nData >= 'a') && (nData <= 'f'))
            *pBuffer = nData - 'a' + 10;
        else if ((nData >= 'A') && (nData <= 'F'))
            *pBuffer = nData - 'A' + 10;
        else
            return 0;
        length--;
        pBuffer++;
        pIn++;
    }

    uint16_t i = 0;
    for (i = 0; i < length; i++)
    {
        if ((i & 0x01) == 0x01) // even is high 4 bits, it should be shift 4 bits for left.
            *pBuffer = (*pBuffer) << 4;
        else
            *pBuffer = 0;

        nData = pIn[i];
        if ((nData >= '0') && (nData <= '9'))
            *pBuffer |= nData - '0';
        else if ((nData >= 'a') && (nData <= 'f'))
            *pBuffer |= nData - 'a' + 10;
        else if ((nData >= 'A') && (nData <= 'F'))
            *pBuffer |= nData - 'A' + 10;
        else
            return 0;
        if ((i & 0x01) == 0x01)
            pBuffer++;
    }
    if ((j & 0x01) == 0x01)
        length++;
    return (length + 1) >> 1;
}

uint16_t hex2ascii(uint8_t *pInput, uint16_t nInputLen, char *pOutput)
{
    uint16_t i;
    char *pBuffer = pOutput;
    for (i = 0; i < nInputLen; i++)
    {
        uint8_t high4bits = pInput[i] >> 4;
        uint8_t low4bits = pInput[i] & 0x0F;

        if (high4bits < 0x0A)
            *pOutput++ = high4bits + '0'; // 0 - 9
        else                              // 0x0A - 0x0F
            *pOutput++ = high4bits - 0x0A + 'A';

        if (low4bits < 0x0A)
            *pOutput++ = low4bits + '0'; // 0 - 0x09
        else                             // 0x0A - 0x0F
            *pOutput++ = low4bits - 0x0A + 'A';
    }
    return pOutput - pBuffer;
}

static void callback_ccho(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    OSI_LOGI(0, "callback_ccho: nEventId: %X, nType: %X", cfw_event->nEventId, cfw_event->nType);
    switch (cfw_event->nEventId)
    {
    case EV_CFW_SIM_MANAGE_CHANNEL_RSP:
    {
        if (cfw_event->nType == 0)
        {
            if (at_sim_session[at_session].channel_status == 0)
            {
                at_sim_session[at_session].sim_channel = cfw_event->nParam1;

                OSI_LOGI(0, "callback_ccho at_sim_session[at_session].sim_channel = %d", at_sim_session[at_session].sim_channel);
                uint16_t uti = cfwRequestUTI((osiEventCallback_t)callback_ccho, cmd);
                uint32_t ret = CFW_SimSelectApplication(at_sim_session[at_session].aid, at_sim_session[at_session].aid_len,
                                                        at_sim_session[at_session].sim_channel, uti, nSim);
                if (ret != ERR_SUCCESS)
                {
                    cfwReleaseUTI(cmd->uti);
                    memset(at_sim_session[at_session].aid, 0, 20);
                    at_sim_session[at_session].aid_len = 0;
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
                else
                    at_sim_session[at_session].channel_status = 1;
                RETURN_FOR_ASYNC();
            }
            else //close channel response
            {
                OSI_LOGI(0, "callback_ccho Open channel failed, close channel!");
                memset(at_sim_session[at_session].aid, 0, 20);
                at_sim_session[at_session].aid_len = 0;
                at_sim_session[at_session].channel_status = 0;
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else
        {
            memset(at_sim_session[at_session].aid, 0, 20);
            at_sim_session[at_session].aid_len = 0;
            at_sim_session[at_session].channel_status = 0;
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    case EV_CFW_SIM_SELECT_APPLICATION_RSP:
    {
        char response[11] = {0};
        if (cfw_event->nType == 0)
        {
            sprintf(response, "%d", at_session + 1);
            atCmdRespInfoText(cmd->engine, response);
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "callback_ccho Close channel, channel id = %d", at_sim_session[at_session].sim_channel);
            cmd->uti = cfwRequestUTI((osiEventCallback_t)callback_ccho, cmd);
            uint32_t ret = CFW_SimManageChannel(CLOSE_CHANNEL_CMD, at_sim_session[at_session].sim_channel, cmd->uti, nSim);
            if (ret != ERR_SUCCESS)
            {
                cfwReleaseUTI(cmd->uti);
                memset(at_sim_session[at_session].aid, 0, 20);
                at_sim_session[at_session].aid_len = 0;
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            RETURN_FOR_ASYNC();
        }
    }
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}

static void callback_cchc(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType == 0)
    {
        memset(at_sim_session[at_session].aid, 0, 20);
        at_sim_session[at_session].aid_len = 0;
        at_sim_session[at_session].channel_status = 0;
        at_session = 0xFF;
        atCmdRespOK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

static void callback_cgla(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType == 0)
    {
        uint16_t length = cfw_event->nParam2;
        uint8_t *TPDU = (uint8_t *)cfw_event->nParam1;
        if (length > 260)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            return;
        }

        if (length == 0)
        {
            uint16_t sw = cfw_event->nParam1;
            char response[16] = {0};
            uint8_t i = sprintf(response, "+CGLA: 4, %X", sw);
            OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP: return data length = %d", strlen(response), i);
            atCmdRespInfoText(cmd->engine, response);
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP: length = %d!", length);
            uint16_t sw = cfw_event->nParam2 >> 16;
            uint16_t len = (cfw_event->nParam2 & 0xFFFF) << 1;
            OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP: updated len = %d!", len);
            len += 12 + 4 + 1; //add prefix and length, sw1 and sw2, terminal zero
            char *response = malloc(len);
            if (response == NULL)
            {
                OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP Malloc failed!");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
                return;
            }
            memset((void *)response, 0, len);
            uint8_t i = sprintf(response, "+CGLA: %d, ", (length + 2) << 1);
            length = hex2ascii(TPDU, length, (response + i));
            if (length == 0)
            {
                OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP hex2ascii failed!");
                free(response);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            }
            i += sprintf(response + i + length, "%X", sw);
            atCmdRespInfoText(cmd->engine, response);
            atCmdRespOK(cmd->engine);
            OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP: return data length = %X, len = %d", response, len);
            OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP: return data length = %d, i = %d", length, i);
            atMemFreeLater((void *)cfw_event->nParam1);
            free(response);
        }
    }
    else
    {
        OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP exec failed!");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}

void atCmdHandleCCHO(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (CFW_GetSimType(nSim) == 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);

        bool paramok = true;
        const char *dfname = atParamStr(cmd->params[0], &paramok);
        if ((cmd->param_count != 1) || !paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        at_session = getFreeSession();
        if (at_session == 0xFF)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        memset(at_sim_session[at_session].aid, 0, 20);
        at_sim_session[at_session].channel_status = 0;
        uint16_t length = ascii2hex(dfname, strlen(dfname), at_sim_session[at_session].aid);
        if ((length == 0) || (length > 32))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        cmd->uti = cfwRequestUTI((osiEventCallback_t)callback_ccho, cmd);
        uint8_t ret = CFW_SimManageChannel(OPEN_CHANNEL_CMD, 0, cmd->uti, nSim);
        if (ret != ERR_SUCCESS)
        {
            cfwReleaseUTI(cmd->uti);
            at_sim_session[at_session].aid_len = 0;
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            at_sim_session[at_session].aid_len = length;
            RETURN_FOR_ASYNC();
        }
    }
    case AT_CMD_TEST:
        RETURN_OK(cmd->engine);
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    } // <----------end of switch cmd  type-------
}

void atCmdHandleCCHC(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1)
        {
            OSI_LOGI(0, "CCHC:ERROR! more than one parameter");
        CCHC_ERROR:
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool status = true;
        uint8_t cmd_session = atParamUint(cmd->params[0], &status);
        if (status != true || cmd_session <= 0)
        {
            OSI_LOGI(0, "CCHC:ERROR! The first parameter is error.");
            goto CCHC_ERROR;
        }
        cmd_session--;
        if (at_sim_session[cmd_session].channel_status == 0)
        {
            OSI_LOGI(0, "CCHC:ERROR! The session id is not activated(session id = %d)", cmd_session + 1);
            goto CCHC_ERROR;
        }
        at_session = cmd_session;
        cmd->uti = cfwRequestUTI((osiEventCallback_t)callback_cchc, cmd);
        uint32_t ret = CFW_SimManageChannel(CLOSE_CHANNEL_CMD, at_sim_session[at_session].sim_channel, cmd->uti, nSim);
        if (ret != ERR_SUCCESS)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
            RETURN_FOR_ASYNC();
        break;
    }
    case AT_CMD_TEST:
    {
        OSI_LOGI(0, "======================= CCHC =======================");
        uint8_t i = 0;
        for (i = 0; i < 8; i++)
        {
            OSI_LOGI(0, "------------ %d ---------------", i);
            //CSW_TC_MEMBLOCK(4, at_sim_session[i].aid, at_sim_session[i].aid_len, 16);
            OSI_LOGI(0, "aid_len = %d", at_sim_session[i].aid_len);
            OSI_LOGI(0, "channel_status = %d", at_sim_session[i].channel_status);
            OSI_LOGI(0, "sim_channel = %d", at_sim_session[i].sim_channel);
        }
        OSI_LOGI(0, "====================================================");
        RETURN_OK(cmd->engine);
        break;
    }
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    } // <----------end of switch cmd  type-------
    return;
}

void atCmdHandleCGLA(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        bool paramok = true;
        uint8_t cmd_session = atParamUintInRange(cmd->params[0], 1, 8, &paramok);
        uint16_t length = atParamUint(cmd->params[1], &paramok);
        const char *pCommand = atParamStr(cmd->params[2], &paramok);
        if ((cmd->param_count != 3) || !paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (at_sim_session[--cmd_session].aid_len == 0)
        {
            OSI_LOGI(0, "CGLA:ERROR! The session id is not activated(session id = %d)", cmd_session + 1);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if ((strlen(pCommand) != length) || (length > 520))
        {
            OSI_LOGI(0, "CGLA:ERROR! length error");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        int16_t tpdu_len = (length + 1) >> 1;
        OSI_LOGI(0, "CGLA:length[%d], tpdu_len[%d]", length, tpdu_len);
        uint8_t *pTPDU = malloc(tpdu_len);
        if (pTPDU == NULL)
        {
            OSI_LOGI(0, "CGLA: no more memory");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        }
        memset(pTPDU, 0x00, tpdu_len);

        if (ascii2hex(pCommand, strlen(pCommand), pTPDU) == 0)
        {
            OSI_LOGI(0, "CALA,ascii2hex ERROR!");
            free(pTPDU);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        OSI_LOGI(0, "Original channel value of TPDU[%d]", pTPDU[0]);
        pTPDU[0] = (pTPDU[0] & 0xF0) | at_sim_session[cmd_session].sim_channel;
        OSI_LOGI(0, "Used channel value of TPDU[%d]", pTPDU[0]);

        OSI_LOGI(0, "at_sim_session[at_session].sim_channel[%d]", at_sim_session[cmd_session].sim_channel);
        cmd->uti = cfwRequestUTI((osiEventCallback_t)callback_cgla, cmd);
        uint32_t ret = CFW_SimTPDUCommand(pTPDU, length >> 1, at_sim_session[cmd_session].sim_channel, cmd->uti, nSim);
        memset(pTPDU, 0x00, tpdu_len);
        free(pTPDU);

        if (ERR_SUCCESS != ret)
        {
            OSI_LOGI(0, "AT+CGLA: ERR_AT_CME_EXE_FAIL = %d !", ret);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        OSI_LOGI(0, "AT+CGLA: OK");
        RETURN_FOR_ASYNC();
    }
    case AT_CMD_TEST:
        RETURN_OK(cmd->engine);
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    } // <----------end of switch cmd  type-------
}
#if 0 //test for mal interface

void atCmdHandleMCCHO(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (CFW_GetSimType(nSim) == 0)
        {
            OSI_LOGI(0, "CCHO:ERROR! This is a SIM card. only USIM support this command!");
        CCHO_ERROR:
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (cmd->param_count > 1)
        {
            OSI_LOGI(0, "CCHO:ERROR!more than one parameter");
            goto CCHO_ERROR;
        }

        bool status = true;
        const char *dfname = atParamStr(cmd->params[0], &status);
        if (status != true)
        {
            OSI_LOGI(0, "CCHO:ERROR! the dfname parameter is error.");
            goto CCHO_ERROR;
        }
        at_session = getFreeSession();
        if (at_session == 0xFF)
        {
            OSI_LOGI(0, "CCHO:ERROR! There is no free sesstion.");
            goto CCHO_ERROR;
        }
        OSI_LOGI(0, "CCHO:  at_session=%d", at_session);
        memset(at_sim_session[at_session].aid, 0, 20);
        at_sim_session[at_session].channel_status = 0;
        uint8_t channel_id;
        if (sim_channel_open(dfname, &channel_id, 60, nSim) == -1)
        {
            at_sim_session[at_session].aid_len = 0;
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        at_sim_session[at_session].sim_channel = channel_id;

        char response[11] = {0};
        at_sim_session[at_session].aid_len = strlen(dfname) >> 2;
        at_sim_session[at_session].channel_status = 1;
        sprintf(response, "+CCHO: %d", at_session + 1);
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    case AT_CMD_TEST:
    {
        OSI_LOGI(0, "======================= CCHO =======================");
        uint8_t i = 0;
        for (i = 0; i < 8; i++)
        {
            OSI_LOGI(0, "------------ %d ---------------", i);
            //CSW_TC_MEMBLOCK(4, at_sim_session[i].aid, at_sim_session[i].aid_len, 16);
            OSI_LOGI(0, "aid_len = %d", at_sim_session[i].aid_len);
            OSI_LOGI(0, "channel_status = %d", at_sim_session[i].channel_status);
            OSI_LOGI(0, "sim_channel = %d", at_sim_session[i].sim_channel);
        }
        OSI_LOGI(0, "====================================================");

        RETURN_OK(cmd->engine);
        break;
    }
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    } // <----------end of switch cmd  type-------
    return;
}

void atCmdHandleMCCHC(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1)
        {
            OSI_LOGI(0, "CCHC:ERROR! more than one parameter");
        CCHC_ERROR:
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool status = true;
        uint8_t cmd_session = atParamUint(cmd->params[0], &status);
        if (status != true || cmd_session <= 0)
        {
            OSI_LOGI(0, "CCHC:ERROR! The first parameter is error.");
            goto CCHC_ERROR;
        }
        cmd_session--;
        if (at_sim_session[cmd_session].channel_status == 0)
        {
            OSI_LOGI(0, "CCHC:ERROR! The session id is not activated(session id = %d)", cmd_session + 1);
            goto CCHC_ERROR;
        }
        at_session = cmd_session;
        if (sim_channel_close(at_sim_session[at_session].sim_channel, 60, nSim) == -1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            char response[11] = {0};
            at_sim_session[at_session].channel_status = 0;
            at_sim_session[at_session].aid_len = 0;
            sprintf(response, "+CCHC: %d", at_session + 1);
            atCmdRespInfoText(cmd->engine, response);
            RETURN_OK(cmd->engine);
        }
        break;
    }
    case AT_CMD_TEST:
    {
        OSI_LOGI(0, "======================= CCHC =======================");
        uint8_t i = 0;
        for (i = 0; i < 8; i++)
        {
            OSI_LOGI(0, "------------ %d ---------------", i);
            //CSW_TC_MEMBLOCK(4, at_sim_session[i].aid, at_sim_session[i].aid_len, 16);
            OSI_LOGI(0, "aid_len = %d", at_sim_session[i].aid_len);
            OSI_LOGI(0, "channel_status = %d", at_sim_session[i].channel_status);
            OSI_LOGI(0, "sim_channel = %d", at_sim_session[i].sim_channel);
        }
        OSI_LOGI(0, "====================================================");
        RETURN_OK(cmd->engine);
        break;
    }
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    } // <----------end of switch cmd  type-------
    return;
}

void atCmdHandleMCGLA(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 3)
        {
            OSI_LOGI(0, "CGLA:ERROR!need three parameters!");
        CGLA_ERROR:
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool status = true;
        uint8_t cmd_session = atParamUint(cmd->params[0], &status);
        if (status != true || cmd_session <= 0)
        {
            OSI_LOGI(0, "CGLA:ERROR! Get parameters error.");
            goto CGLA_ERROR;
        }

        cmd_session--;
        if (at_sim_session[cmd_session].aid_len == 0)
        {
            OSI_LOGI(0, "CGLA:ERROR! The session id is not activated(session id = %d)", cmd_session + 1);
            goto CGLA_ERROR;
        }

        uint16_t length = atParamUint(cmd->params[1], &status);
        if (status != true)
        {
            OSI_LOGI(0, "CGLA:ERROR! Get parameters two error.");
            goto CGLA_ERROR;
        }

        const char *pCommand = atParamStr(cmd->params[2], &status);
        if (status != true)
        {
            OSI_LOGI(0, "CGLA:ERROR! The third parameter three error.");
            goto CGLA_ERROR;
        }
        if (strlen(pCommand) != length)
        {
            OSI_LOGI(0, "CGLA:ERROR! length of third parameter isn't equal to input.");
            goto CGLA_ERROR;
        }

        int16_t tpdu_len = (length + 1) >> 1;
        OSI_LOGI(0, "CGLA:length = %d, tpdu_len = %d", length, tpdu_len);
        uint8_t *pTPDU = malloc(tpdu_len);
        if (pTPDU == NULL)
        {
            OSI_LOGI(0, "AT+CGLA: no more memory");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        }
        length = ascii2hex(pCommand, strlen(pCommand), pTPDU);
        if (length == 0)
        {
            OSI_LOGI(0, "ascii2hex ERROR!");
            free(pTPDU);
            goto CGLA_ERROR;
        }
        OSI_LOGI(0, "tpdu_len = %d锛宭ength = %d", tpdu_len, length);
        OSI_LOGI(0, "at_sim_session[at_session].sim_channel = %d", at_sim_session[cmd_session].sim_channel);

        uint8_t resp[600] = {0};
        uint16_t respLen = 600;
        uint8_t ret = sim_channel_transmit(at_sim_session[cmd_session].sim_channel, pTPDU, length,
                                           resp, &respLen, 60, nSim);
        free(pTPDU);
        if (0 != ret)
        {
            OSI_LOGI(0, "AT+CGLA: ERR_AT_CME_EXE_FAIL = %d !", ret);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP: respLen = %d!", respLen);
        char response[1024] = {0};
        memset((void *)response, 0, 1024);

        uint8_t i = sprintf(response, "+CGLA: %d, ", respLen << 1);
        OSI_LOGI(0, "i = %d!", i);
        length = hex2ascii(resp, respLen, response + i);
        if (length == 0)
        {
            OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP hex2ascii failed!");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        }

        OSI_LOGI(0, "EV_CFW_SIM_TPDU_COMMAND_RSP: return data length = %d, i = %d", length, i);
        atCmdRespInfoText(cmd->engine, response);
        atCmdRespOK(cmd->engine);
        OSI_LOGI(0, "AT+CGLA: OK");
        break;
    }
    case AT_CMD_TEST:
    {
        OSI_LOGI(0, "======================= CGLA =======================");
        uint8_t i = 0;
        for (i = 0; i < 8; i++)
        {
            OSI_LOGI(0, "------------ %d ---------------", i);
            //CSW_TC_MEMBLOCK(4, at_sim_session[i].aid, at_sim_session[i].aid_len, 16);
            OSI_LOGI(0, "aid_len = %d", at_sim_session[i].aid_len);
            OSI_LOGI(0, "channel_status = %d", at_sim_session[i].channel_status);
            OSI_LOGI(0, "sim_channel = %d", at_sim_session[i].sim_channel);
        }
        OSI_LOGI(0, "====================================================");
        RETURN_OK(cmd->engine);
    }
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    } // <----------end of switch cmd  type-------
    return;
}
#endif
static uint8_t g_simCon = 0;
void atCmdHandleSIMCON(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1)
        {
            OSI_LOGI(0, "SIMCON:ERROR!param count error!");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        bool paramok = true;
        uint32_t op_mode = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
        if (!paramok)
        {
            OSI_LOGI(0, "SIMCON:ERROR!param error!");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        //reset command should not affect current state
        if (op_mode == 1)
            g_simCon = 1;
        else if (op_mode == 0)
            g_simCon = 0;
        CFW_SetSimFileUpdateCountMode((uint8_t)op_mode, nSim);
        RETURN_OK(cmd->engine);
    }
    break;
    case AT_CMD_READ:
    {
        char response[32] = {0};
        sprintf(response, "+SIMCON:%d", g_simCon);
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    break;
    case AT_CMD_TEST:
    {
        char response[32] = {0};
        sprintf(response, "+SIMCON:(0-2)");
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
    break;
    }
}
void atCmdHandleSIMCNT(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        uint32_t nEFID = 0;

        if (cmd->param_count != 1)
        {
            OSI_LOGI(0, "param_count is: %d", cmd->param_count);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool paramok = true;
        const char *pEFID = atParamHexRawText(cmd->params[0], 2, &paramok);
        if (paramok == false)
        {
            OSI_LOGI(0, "param error");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        nEFID = ((*pEFID) << 8) + *(pEFID + 1);
        nEFID = nEFID & 0xFFFF;
        uint8_t nFileID = 0;
        uint16_t nRetryCount = 0;
        uint16_t nRealCount = 0;
        nFileID = (uint8_t)CFW_GetStackSimFileID(nEFID, 0, nSim);
        OSI_LOGI(0, "nEFID = %x, nFileID = %x\n", nEFID, nFileID);
        nRetryCount = CFW_GetSimFileUpdateCount(nFileID, 0, nSim);
        nRealCount = CFW_GetSimFileUpdateCount(nFileID, 1, nSim);
        char response[32] = {0};
        uint16_t out_nEFID = (uint16_t)nEFID;
        if (nRetryCount != 0xFFFF && nRealCount != 0xFFFF)
        {
            sprintf(response, "+SIMCNT:%X,%d,%d", out_nEFID, nRetryCount, nRealCount);
            atCmdRespInfoText(cmd->engine, response);
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "not support file");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    break;
    case AT_CMD_TEST:
    {
        char response[32] = {0};
        sprintf(response, "+SIMCNT=<EF ID>");
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
    break;
    }
}
static void CSVM_SetRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType == 0xF0)
    {
        atCmdRespCmeError(cmd->engine, _parseSw1Sw2ErrorCode(cfw_event->nParam1));
        return;
    }
    RETURN_OK(cmd->engine);
}

static void CSVM_GetRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType == 0xF0)
    {
        atCmdRespCmeError(cmd->engine, _parseSw1Sw2ErrorCode(cfw_event->nParam1));
        return;
    }

    CFW_SIM_INFO_VOICEMAIL *pVoicemail = (CFW_SIM_INFO_VOICEMAIL *)cfw_event->nParam1;
    if (pVoicemail != NULL)
    {
        char rsp[64];
        char number[32];
        cfwBcdToDialString(pVoicemail->mailbox_number, pVoicemail->mailbox_number_len, number);
        sprintf(rsp, "%s: %d,\"%s\"", cmd->desc->name, gAtSetting.csvm, number);
        atCmdRespInfoText(cmd->engine, rsp);
    }

    RETURN_OK(cmd->engine);
}

void atCmdHandleCSVM(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // AT+CSVM=<mode>[,<number>]
        bool paramok = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || (cmd->param_count != 1 && cmd->param_count != 2))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.csvm = mode;
        if (cmd->param_count == 1)
            RETURN_OK(cmd->engine);

        const char *number = atParamDefStr(cmd->params[1], "", &paramok);
        if (!paramok || strlen(number) > 20)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        CFW_SIM_INFO_VOICEMAIL updateVoiceMailInfo = {0};
        memset(&updateVoiceMailInfo, 0, sizeof(CFW_SIM_INFO_VOICEMAIL));

        int length = cfwDialStringToBcd(number, strlen(number), updateVoiceMailInfo.mailbox_number);
        if (length < 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        updateVoiceMailInfo.mailbox_number_len = length;

        updateVoiceMailInfo.ccp_id = 0;
        updateVoiceMailInfo.is_ext_exist = false;
        updateVoiceMailInfo.ext_id = 0;
        updateVoiceMailInfo.is_ccp_exist = false;
        updateVoiceMailInfo.is_ton_npi_exist = false;

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CSVM_SetRspCB, cmd);
        uint32_t ret = CFW_SimUpdateVoiceMailInfo(1, 0, &updateVoiceMailInfo, cmd->uti, atCmdGetSim(cmd->engine));
        if (ret != ERR_SUCCESS)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_READ)
    {
        cmd->uti = cfwRequestUTI((osiEventCallback_t)CSVM_GetRspCB, cmd);
        CFW_SimReadVoiceMailInfo(1, 0, cmd->uti, atCmdGetSim(cmd->engine));
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char rsp[64];
        sprintf(rsp, "%s: (0-1),<number>", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}

static void _cocsimCBRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    CFW_SIM_ID nSim = (CFW_SIM_ID)cfw_event->nFlag;
    if (cfw_event->nType == 0xF0)
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
        return;
    }
    if (cfw_event->nEventId == EV_CFW_SIM_CLOSE_RSP)
    {
        CFW_SetSimStatus(nSim, CFW_SIM_ABSENT);
    }
    else if (cfw_event->nEventId == EV_CFW_SIM_RESET_RSP)
    {
        CFW_SetSimStatus(nSim, CFW_SIM_NORMAL);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_INVALID_DATE_OR_TIME);
        return;
    }
    RETURN_OK(cmd->engine);
}
void atCmdHandleCOCSIM(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t nSim = atCmdGetSim(cmd->engine);
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || (cmd->param_count != 1))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        CFW_SIM_STATUS status = CFW_GetSimStatus(nSim);
        OSI_LOGI(0, "COCSIM get sim status: %d", status);

        if (mode == 0)
        {
            if (status == CFW_SIM_NORMAL)
            {
                RETURN_OK(cmd->engine);
            }
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cocsimCBRsp, cmd);
            uint32_t ret = CFW_SimWakeup(cmd->uti, nSim, 0);
            if (ret != ERR_SUCCESS)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else
        {
            if (status == CFW_SIM_ABSENT)
            {
                RETURN_CME_CFW_ERR(cmd->engine, ERR_AT_CME_SIM_NOT_INSERTED);
            }
            if (status != CFW_SIM_NORMAL)
            {
                RETURN_CME_CFW_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cocsimCBRsp, cmd);
            uint32_t ret = CFW_SimCloseEx(cmd->uti, nSim, 1);
            if (ret != ERR_SUCCESS)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        RETURN_FOR_ASYNC();
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}
