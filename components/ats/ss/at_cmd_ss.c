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

#include "atr_config.h"
#include "cfw_config.h"
#include "cfw_chset.h"
#include "osi_log.h"
#include "at_cfg.h"
#include "at_cfw.h"
#include "at_cmd_ss.h"
#include "cfw_event.h"
#include "cfw_errorcode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "quec_proj_config_at.h"
#include "quec_cust_patch.h"
uint8_t g_ss_ussdVer = 2;
//#define AT_MAX_DIGITS_USSD 183
#define MAX_LENGTH_STRING_USSD 160
#define MAX_LENGTH_BUFF 256
#define DEFAULT_ALPHABET 0x0f
bool gATSATSendUSSDFlag[CONFIG_NUMBER_OF_SIM]; //extern bool gATSATSendUSSDFlag[CONFIG_NUMBER_OF_SIM]; <-at_cmd_sat.c

extern uint8_t UriParaParse(const char *pPara, char *pDialNum);
static bool AT_SS_IsValidPhoneNumber(const char *s, uint8_t size, bool *bIsInternational);
typedef struct
{
    uint8_t ussd_input[AT_MAX_DIGITS_USSD];
    bool b_used;
} AT_USSD_STRING;
AT_USSD_STRING at_ussdstring[NUMBER_OF_SIM];

struct ussd_record_state
{
    uint8_t ussd_valid;
    uint8_t ussd_num;
};
static struct ussd_record_state ussd_state = {0, 0};

typedef enum _AT_MSG_ALPHABET
{
    AT_MSG_GSM_7 = 0,
    AT_MSG_8_BIT_DATA,
    AT_MSG_UCS2,
    AT_MSG_RESERVED,
} AT_MSG_ALPHABET;

uint8_t AT_SS_GetUSSDState()
{
    OSI_LOGXI(OSI_LOGPAR_SI, 0x10004509, "%s state=%d", __FUNCTION__, ussd_state.ussd_valid);
    return ussd_state.ussd_valid;
}

uint8_t AT_SS_GetUSSDNum()
{
    OSI_LOGXI(OSI_LOGPAR_SI, 0x1000450a, "%s num =%d", __FUNCTION__, ussd_state.ussd_num);
    return ussd_state.ussd_num;
}

void AT_SS_SetUSSDNum(uint8_t num)
{
    OSI_LOGXI(OSI_LOGPAR_SI, 0x10004508, "%s num=%d", __FUNCTION__, num);
    ussd_state.ussd_valid = 1;
    ussd_state.ussd_num = num;
}

void AT_SS_ResetUSSDState()
{
    OSI_LOGXI(OSI_LOGPAR_S, 0x080000a1, "%s ", __FUNCTION__);
    ussd_state.ussd_valid = 0;
    ussd_state.ussd_num = 0;
}

//Parse TP-DCS , more in "3G TS 23.038 V3.3.0"
void AT_USSDParseDCS(uint8_t nInputData, uint8_t *pAlphabet)
{
    //Parse TP-DCS , more in "3G TS 23.038 V3.3.0"
    *pAlphabet = AT_MSG_RESERVED;

    if ((nInputData & 0xf0) == 0x00) // bit(7-0): 0000 xxxx
    {
        *pAlphabet = AT_MSG_GSM_7;
    }
    else if ((nInputData & 0xf0) == 0x10) // bit(7-0): 0001 xxxx
    {
        if ((nInputData & 0x0f) == 0x00) // bit(7-0): 0001 0000
            *pAlphabet = AT_MSG_GSM_7;
        else if ((nInputData & 0x0f) == 0x01) // bit(7-0): 0001 0001
            *pAlphabet = AT_MSG_UCS2;
        else
            *pAlphabet = AT_MSG_RESERVED;
    }
    else if ((nInputData & 0xc0) == 0x40) //bit(7-0): 01xx xxxx
    {
        //Set pAlphabet
        if ((nInputData & 0x0c) == 0x00) // bit(7-0): 01xx 00xx
        {
            *pAlphabet = AT_MSG_GSM_7;
        }
        else if ((nInputData & 0x0c) == 0x04) // bit(7-0): 01xx 01xx
        {
            *pAlphabet = AT_MSG_8_BIT_DATA;
        }
        else if ((nInputData & 0x0c) == 0x08) // bit(7-0): 01xx 10xx
        {
            *pAlphabet = AT_MSG_UCS2;
        }
        else if ((nInputData & 0x0c) == 0x0c) // bit(7-0): 01xx 11xx
        {
            *pAlphabet = AT_MSG_RESERVED;
        }
    }
    else if ((nInputData & 0xf0) == 0x90) //bit(7-0): 1001 xxxx
    {
        //Set pAlphabet
        if ((nInputData & 0x0c) == 0x00) // bit(7-0): 1001 00xx
        {
            *pAlphabet = AT_MSG_GSM_7;
        }
        else if ((nInputData & 0x0c) == 0x04) // bit(7-0): 1001 01xx
        {
            *pAlphabet = AT_MSG_8_BIT_DATA;
        }
        else if ((nInputData & 0x0c) == 0x08) // bit(7-0): 1001 10xx
        {
            *pAlphabet = AT_MSG_UCS2;
        }
        else if ((nInputData & 0x0c) == 0x0c) // bit(7-0): 1001 11xx
        {
            *pAlphabet = AT_MSG_RESERVED;
        }
    }
    else if ((nInputData & 0xf0) == 0xf0) //bit(7-0): 1111 0xxx
    {
        if ((nInputData & 0x04) == 0x04) // bit(7-0): 1111 01xx
            *pAlphabet = AT_MSG_8_BIT_DATA;
        else // bit(7-0): 1111 00xx
            *pAlphabet = AT_MSG_GSM_7;
    }

    if (*pAlphabet == AT_MSG_RESERVED)
    {
        *pAlphabet = AT_MSG_GSM_7;
    }
}

extern at_chset_t cfg_GetTeChset(void);

void _GenerateCUSData(uint8_t *pUSSDString, uint8_t nUSSDLen, uint8_t *pDestUSSDString, uint8_t nUSSDDCS)
{
    OSI_LOGI(0x1000450b, "_GenerateCUSData nUSSDDCS %d, nUSSDLen: %d\n", nUSSDDCS, nUSSDLen);

    if (nUSSDDCS == AT_MSG_UCS2)
    {
        uint8_t *in = NULL;
        uint8_t nSwitchData = 0x00;
        uint8_t i = 0x00;

        in = pUSSDString;

        for (i = 0x00; i < nUSSDLen; i++)
        {
            nSwitchData = (in[i] >> 4) + (in[i] << 4);
            in[i] = nSwitchData;
        }
    }
    else if (nUSSDDCS == AT_MSG_GSM_7)
    {
        OSI_LOGI(0, "_GenerateCUSData AT_MSG_GSM_7\n");
        cfwDecode7Bit(pDestUSSDString, pUSSDString, nUSSDLen);
    }
    else
    {
        OSI_LOGI(0, "_GenerateCUSData AT_MSG_8_BIT_DATA\n");
        memcpy(pDestUSSDString, pUSSDString, nUSSDLen);
    }
#if 0    
    if (cs_hex == gAtSetting.cscs)
    {
        OSI_LOGI(0, "_GenerateCUSData cs_hex == cfg_GetTeChset()\n");

        if (nUSSDDCS == AT_MSG_GSM_7 || nUSSDDCS == AT_MSG_8_BIT_DATA)
        {
            uint8_t *in = NULL;
            uint8_t nSwitchData = 0x00;
            uint8_t i = 0x00;

            in = pUSSDString;

            for (i = 0x00; i < nUSSDLen; i++)
            {
                nSwitchData = (in[i] >> 4) + (in[i] << 4);
                in[i] = nSwitchData;
            }
        }
        cfwBcdToDialString(pUSSDString, nUSSDLen, pDestUSSDString);
    }
#endif
}

/*==========================================================================
 * This function is used to process SS PROC_USS REQ to SSCode related REQ.
 *=========================================================================*/

uint32_t SS_AT_ReSendProcUssV1Req(uint8_t nUTI, CFW_SIM_ID nSIMID)
{
    uint32_t ret;
    uint8_t srcLen;

    at_ussdstring[nSIMID].b_used = true;

    OSI_LOGXI(OSI_LOGPAR_SS, 0x09000509, "USSD Func: %s ussd_input:%s", __FUNCTION__, at_ussdstring[nSIMID].ussd_input);
    srcLen = strlen((const char *)at_ussdstring[nSIMID].ussd_input);

    ret = CFW_SsSendUSSD(at_ussdstring[nSIMID].ussd_input, srcLen, 131, 0x0F, nUTI, nSIMID);
    if (ret != ERR_SUCCESS)
    {
        OSI_LOGI(0x0900050a, "SS_ReSendProcUssV1Req()   CFW_CmSendUSSD  ret = %d", ret);
        return ERR_AT_CME_PHONE_FAILURE;
    }
    /*Must add code here*/
    return ERR_AT_CME_PHONE_FAILURE;
}

#ifdef PORTING_ON_GOING
#define AT_SS_DEBUG
uint8_t g_SsCaocStatus = 0;
uint32_t g_uiCurrentCmdStampSs = 0;
uint8_t g_ucAtSsModeCSVMOrCCFCRead = 0;

const uint8_t *strTCCWA = "+CCWA:(0,1)";         // "+CCWA:(0-1),(0-2),(1,2,4)";
const uint8_t *strTCCFC = "+CCFC:(0,1,2,3,4,5)"; // "+CCFC:(0-5),(0-4),(NUMBER),(128,129,145,161,208),(5-30)";
const uint8_t *strTCLIP = "+CLIP:(0,1)";
const uint8_t *strTCLIR = "+CLIR:(0,1,2)";
const uint8_t *strTCOLP = "+COLP:(0,1)";
const uint8_t *strTCOLR = "+COLR:(0,2)";
const uint8_t *strTCUSD = "+CUSD:(0,1,2)";
const uint8_t *strTCSSN = "+CSSN:(0,1),(0,1)";
const uint8_t *strTCSVM = "+CSVM:(0,1),(129,145)";

const uint8_t *strFCCWA = ""; // "+CCWA=[<n>[,<mode>[,<class>]]]\r\n";

const uint8_t *strRCSSN = "+CSSN:%d,%d"; // hameina2007.12.07 [mod] :delete the last sp.
const uint8_t *strRCAMM = "+CAMM:%s ";
const uint8_t *strRCAOC = "+CAOC:%d";
const uint8_t *strRCPUC = "+CPUC:%s,%s";
const uint8_t *strRCUSD = "+CUSD:%d";
const uint8_t *strRCOLP = "+COLP:%d";

extern void AT_Result_OK(uint32_t uReturnValue, uint32_t uResultCode, uint8_t nDelayTime, uint8_t *pBuffer, uint16_t nDataSize,
                         uint8_t nDLCI, uint8_t nSim);
extern void AT_Result_Err(uint32_t uErrorCode, uint8_t nErrorType, uint8_t nDLCI, uint8_t nSim);
extern void AT_SMS_Result_OK(uint32_t uReturnValue, uint32_t uResultCode, uint8_t nDelayTime, uint8_t *pBuffer,
                             uint16_t nDataSize, uint8_t nDLCI, uint8_t nSim);
extern BOOL SMS_PDUBCDToASCII(uint8_t *pBCD, uint16_t *pLen, uint8_t *pStr);
extern uint32_t CFW_SimSetPUCT(CFW_SIM_PUCT_INFO *pPUCT, uint8_t *pPin2, uint8_t nPinSize, uint16_t nUTI, CFW_SIM_ID nSimID);
extern uint32_t CFW_SimGetPUCT(uint16_t nUTI, CFW_SIM_ID nSimID);
extern uint32_t CFW_SimSetACM(uint32_t iCurValue, uint8_t *pPIN2, uint8_t nPinSize, uint16_t nUTI, CFW_SIM_ID nSimID);
extern uint32_t CFW_SimGetACM(uint16_t nUTI, CFW_SIM_ID nSimID);

uint32_t SS_ReadAocFromSim(uint8_t ucFileType, uint32_t *uiRetAoc)
{
    return 0;
}

// *************************************
// discription: this function check the DCS type, 7bit, 8 bit or UCS2
// *****************************************************
// return value:  1:    7bit
// 2:    8 bit
// 4:    UCS2
// 0xff:   err
// ************************************************
uint8_t AT_CBS_CheckDCS(uint8_t uDCS)
{
    uint8_t ret = 0xff;
    uint8_t uHigh = 0;
    uint8_t uLow = 0;

    uHigh = uDCS >> 4;
    uLow = uDCS & 0x0f;

    if (!uHigh) // 0
    {
        ret = 1;
    }
    else if (1 == uHigh)
    {
        if (uLow == 0)
            ret = 1;
        else if (uLow == 1)
            ret = 4;
    }
    else if (2 == uHigh || 3 == uHigh)
    {
        ret = 1;
    }
    else if ((uHigh >> 2) == 01) // 01xx
    {
        if ((uLow >> 2) == 0)
            ret = 1;
        else if ((uLow >> 2) == 1)
            ret = 2;
        else if ((uLow >> 2) == 2)
            ret = 4;
    }
    else if (uHigh == 15)
    {
        if (((uLow >> 2) & 1) == 0)
            ret = 1;
        else if (((uLow >> 2) & 1) == 1)
            ret = 2;
    }

    return (ret);
}
#endif

uint32_t AT_SS_QUERYERR(uint8_t nType, uint16_t nErrCode)
{
    uint32_t uiErrCode = 0;

    if (0xfa == nType || (0xfc == nType)) // Local Error /net work reject
    {
        switch (nErrCode)
        {
        case 10: // CFW_Error_SS_Service_Not_Implemented
        case 11: // CFW_Error_SS_Invalid_Operation
        case 12: // CFW_Error_SS_Invalid_Service_Code
            uiErrCode = ERR_AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case 14: // CFW_Error_SS_Ctxt_Allocation
            uiErrCode = ERR_AT_CME_MEMORY_FAILURE;
            break;

        case 20: // CFW_Error_SS_MM_Establishment
        case 21: // CFW_Error_SS_TransactionTimeout
        case 22: // CFW_Error_SS_MMConnectionReleased
        case 23: // CFW_Error_SS_UnknownComponentReceived
            uiErrCode = ERR_AT_CME_PHONE_FAILURE;
            break;

        default:
            uiErrCode = ERR_AT_UNKNOWN;
            break;
        }
    }
    else if (0xfb == nType || 0xfe == nType) // Nwk returned a return error
    {
        uiErrCode = ERR_AT_CME_OPERATION_NOT_SUPPORTED;
    }
    else if (0xfd == nType) // Nwk returned a Cause IE
    {
        uiErrCode = ERR_AT_CME_OPERATION_NOT_SUPPORTED;
    }
    else // return event error
    {
        uiErrCode = ERR_AT_UNKNOWN;
    }

    return uiErrCode;
}

/******************************************************************************************
Function            :   AT_SMS_IsValidPhoneNumber
Description     :       check number copy from pbk module is valid or not
Called By           :   by SMS module
Data Accessed       :
Data Updated        :
Input           :       uint8_t * arrNumber, uint8_t nNumberSize, bool * bIsInternational
Output          :
Return          :   bool
Others          :       //[+]2007.11.29 for check number copy from pbk module
                    //arrNumber[in]; pNumberSize[in]; bIsInternational[out]
                    //legal phone number char: 0-9,*,#,+ (+ can only be the first position)
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
static bool AT_SS_IsValidPhoneNumber(const char *s, uint8_t size, bool *bIsInternational)
{
    if (s == NULL || size == 0)
        return false;

    *bIsInternational = false;
    const char *end = s + size;
    if (*s == '+')
    {
        *bIsInternational = true;
        s++;
    }

    while (s < end)
    {
        char c = *s++;
        if (!(c == '#' || c == '*' || (c >= '0' && c <= '9')))
            return false;
    }
    return true;
}

static void _onEV_CFW_SS_USSD_IND(const osiEvent_t *event)
{
    OSI_LOGI(0, "CUSD: In _onEV_CFW_SS_USSD_IND event");
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    char *aucBuffer = (char *)malloc(2 * MAX_LENGTH_STRING_USSD + 30);
    memset(aucBuffer, 0x00, 2 * MAX_LENGTH_STRING_USSD + 30);

    CFW_SS_USSD_IND_INFO_V2 *pUSSDInd = NULL;
    uint8_t nUSSDDCS = 0x00;
    uint8_t aucDestUsd[2 * MAX_LENGTH_STRING_USSD + 2] = {0};

    pUSSDInd = (CFW_SS_USSD_IND_INFO_V2 *)cfw_event->nParam1;
    atMemFreeLater(pUSSDInd);
    AT_USSDParseDCS(pUSSDInd->nDcs, &nUSSDDCS);
    OSI_LOGI(0, "CUSD:pUSSDInd->nDcs=%d, nUSSDDCS: %d\n", pUSSDInd->nDcs, nUSSDDCS);
    OSI_LOGXI(OSI_LOGPAR_SI, 0, "CUSD:pUsdString=%s nStingSize=%d", pUSSDInd->pUsdString, pUSSDInd->nStingSize);
    _GenerateCUSData(pUSSDInd->pUsdString, pUSSDInd->nStingSize, aucDestUsd, nUSSDDCS);
    OSI_LOGI(0, "CUSD:   nUTI = %d\n", cfw_event->nUTI);
#if QUEC_PATCH_CUSD
	//AT_SS_SetUSSDNum(cfw_event->nUTI);
#else
	AT_SS_SetUSSDNum(cfw_event->nUTI);
#endif
    OSI_LOGXI(OSI_LOGPAR_S, 0, "CUSD:EV_CFW_SS_USSD_IND aucDestUsd: %s\n", aucDestUsd);
    sprintf(aucBuffer, "+CUSD: %u, \"%s\" ,%u\r\n", pUSSDInd->nOption, aucDestUsd, pUSSDInd->nDcs);

    atCmdRespSimUrcText(cfw_event->nFlag, aucBuffer);
    free(aucBuffer);
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: called by ATM,supplementary service initial
//
// Parameter:
// Caller:
// Called:
// Return:
// Remark:
//
// ////////////////////////////////////////////////////////////////////////////

void AT_SS_Init(void)
{

    // global variable initial
    OSI_LOGI(0, "ss: in AT_SS_INIT nUTI=%d");
    atEventsRegister(EV_CFW_SS_USSD_IND, _onEV_CFW_SS_USSD_IND,
                     0);
}
#ifdef PORTING_ON_GOING
// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: called by ATM,supplementary service asynchronous event process
//
// Parameter:   pstOsEvent,os event structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      same respond for different function,from flag to process
//
// ////////////////////////////////////////////////////////////////////////////

void AT_SS_AsyncEventProcess(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event
        uint8_t aucBuffer[256] = {0}; // 256
    uint8_t aucBufferTemp[256] = {0};
    //EV_CFW_SS_QUERY_CALL_FORWARDING_RSP
    uint8_t nSim = cfw_event->nFlag;

    if (NULL == cfw_event)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_MEMORY_FAILURE);

#ifdef AT_SS_DEBUG
    OSI_LOGI(0x1000450e, "Event structure:EventId: %d Para1: %d Para2: %d Type: %d Flag: %d UTI: %d\n",
             cfw_event->nEventId, cfw_event->nParam1, cfw_event->nParam2, cfw_event->nType, cfw_event->nFlag,
             cfw_event->nUTI);
#endif
    // receive event from CSW and process

    switch (cfw_event->nEventId)
    {
    case EV_CFW_SS_QUERY_FACILITY_LOCK_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CLCK", g_uiCurrentCmdStampSs, cfw_event->nUTI))
        {
            if (0 == cfw_event->nType)
            {
                //when <mode>=2 and command successful:
                //+CLCK: <status>[,<class1>[<CR><LF>+CLCK: <status>,<class2>

                sprintf(aucBufferTemp, "+CLCK:%d,%d", LOUINT8(cfw_event->nParam2), HIUINT8(cfw_event->nParam2));
                AT_StrCat(aucBuffer, aucBufferTemp);
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), cfw_event->nUTI, nSim);
                pstOsEvent->nParam1 = 0;

                return;
            }
            else if (0xF0 == cfw_event->nType)
            {
                uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);

                if (ERR_AT_OK != uiErrCode)
                {
                    AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
                    pstOsEvent->nParam1 = 0;
                    return;
                }
            }
            else
            {
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
                pstOsEvent->nParam1 = 0;
                return;
            }
        }
    }
    break;
    case EV_CFW_SS_SET_FACILITY_LOCK_RSP:
    {
        pstOsEvent->nParam1 = 0;
        if (AT_IsAsynCmdAvailable("+CLCK", g_uiCurrentCmdStampSs, cfw_event->nUTI))
        {

            if (0 == cfw_event->nType)
            {
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, cfw_event->nUTI, nSim);
            }
            else if (0xF0 == cfw_event->nType)
            {
                uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
                if (ERR_AT_OK != uiErrCode)
                {
                    AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
                    return;
                }
            }
            else
            {
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
                return;
            }
        }
    }
    break;
    case EV_CFW_SS_SET_CALL_WAITING_RSP: // EV_CSW_SS_SET_CW_RSP:
    {
        pstOsEvent->nParam1 = 0;
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+CCWA", g_uiCurrentCmdStampSs, cfw_event->nUTI))
        {
            return;
        }

        if (0 == cfw_event->nType) // success
        {
            // OK return
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, cfw_event->nUTI, nSim);
        }
        else // if (0xfa == cfw_event->nType || (0xfc == cfw_event->nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
            }
        }
    }

    break;

        // CCWA read response event +CCWA: <n>

    case EV_CFW_SS_QUERY_CALL_WAITING_RSP: // EV_CSW_SS_QUERY_CW_RSP:
        break;

        // CCFC,set response event

    case EV_CFW_SS_SET_CALL_FORWARDING_RSP: // EV_CSW_SS_SET_CF_RSP:
    {
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+CCFC", g_uiCurrentCmdStampSs, cfw_event->nUTI))
        {
            return;
        }

        if (0 == cfw_event->nType) // success
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, cfw_event->nUTI, nSim);
            pstOsEvent->nParam1 = 0;
        }
        else // if (0xfa == cfw_event->nType || (0xfc == cfw_event->nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
            }
        }
    }

    break;

        // CCFC, read response event

    case EV_CFW_SS_QUERY_CALL_FORWARDING_RSP: // EV_CSW_SS_QUERY_CF_RSP:
    {
        // check command life cycle
        //if (!AT_IsAsynCmdAvailable("+CCFC", g_uiCurrentCmdStampSs, cfw_event->nUTI))
        //    return;

        if (0 == cfw_event->nType) // success
        {
            CFW_SS_QUERY_CALLFORWARDING_RSP *pstSsQueryCallForwardingRsp = NULL;
            uint8_t aucAsciiNumber[43] = {0}; // 43
            uint8_t ucDestClass = 0;
            uint8_t aucStatus = 0;
            uint8_t aucType = 0;

            pstSsQueryCallForwardingRsp = (CFW_SS_QUERY_CALLFORWARDING_RSP *)(cfw_event->nParam1);

            // convert class software and 3gpp
            AT_SsClassConvert2Ccfc(pstSsQueryCallForwardingRsp->nClass, &ucDestClass);
            aucType = pstSsQueryCallForwardingRsp->nNumber.nType;
            aucStatus = pstSsQueryCallForwardingRsp->nStatus;

            // paucAsciiNumber[ucDestClass] = aucAsciiNumber;
            // set status of current class is 1
            // [[Changyg[mod] 2008-04-22 :add "+" when acuType is 145

            if (145 == aucType)
            {
                aucAsciiNumber[0] = 0x2b;
                SUL_GsmBcdToAscii(pstSsQueryCallForwardingRsp->nNumber.nTelNumber, pstSsQueryCallForwardingRsp->nNumber.nSize,
                                  &aucAsciiNumber[1]);
            }
            else
            {
                SUL_GsmBcdToAscii(pstSsQueryCallForwardingRsp->nNumber.nTelNumber, pstSsQueryCallForwardingRsp->nNumber.nSize,
                                  aucAsciiNumber);
            }

            // ]]Changyg[mod] 2008-04-22 :end

            // CCFC respond
            // parameter2 is size of structure, not The counter of structure array
            if (pstSsQueryCallForwardingRsp->nStatus)
            {
                if (!pstSsQueryCallForwardingRsp->nTime)
                {
                    // number exist,display all information
                    sprintf(aucBufferTemp, "+CCFC:%u,%u,\"%s\",%u", aucStatus, ucDestClass, aucAsciiNumber, aucType);
                }
                else
                {
                    sprintf(aucBufferTemp, "+CCFC:%u,%u,\"%s\",%u,,,%u", aucStatus, ucDestClass, aucAsciiNumber, aucType,
                            pstSsQueryCallForwardingRsp->nTime);
                }
            }
            else
                sprintf(aucBufferTemp, "+CCFC:%u,%u", aucStatus, ucDestClass);
            // OK retrun
            atCmdRespOK(cmd->engine);
        }
        else
        {
            uint32_t uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
            // ERROR return
            if (ERR_SUCCESS != uiErrCode)
                RETURN_CME_ERR(cmd->engine, uiErrCode);
        }
    }

    break;

        // CLIP read response event

        // CLIR read response event

    case EV_CFW_SS_QUERY_CLIR_RSP: // EV_CSW_SS_QUERY_CLIR_RSP:
    {
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+CLIR", g_uiCurrentCmdStampSs, cfw_event->nUTI))
        {
            return;
        }

        if (0 == cfw_event->nType) // success
        {
            // parameter1 parsing
            // 0 Presentation indicator is used according to the subscription of the CLIR service
            // 1 CLIR invocation. Activate CLIR. Disable presentation of own phone number to called party
            // 2 CLIR suppression. Suppress CLIR. Enable presentation of own phone number to called party.

            // parameter2 parsing
            // 0 CLIR not provisioned
            // 1 CLIR provisioned in permanent mode
            // 2 unknown (e.g. no network, etc.)
            // 3 temporary mode presentation restricted
            // 4 CLIR temporary mode presentation allowed
            sprintf(aucBuffer, "+CLIR:%u,%u", gATCurrentucClir, (uint8_t)cfw_event->nParam2);
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), cfw_event->nUTI, nSim);
        }
        else // if (0xfa == cfw_event->nType || (0xfc == cfw_event->nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
            }
        }
        pstOsEvent->nParam1 = 0;
    }

    break;

        // CLOR read response event

    case EV_CFW_SS_QUERY_COLR_RSP: // EV_CSW_SS_QUERY_COLR_RSP:
    {
        // check command life cycle
        OSI_LOGI(0x1000450f, "EV_CFW_SS_QUERY_COLR_RSP\n");
        if (!AT_IsAsynCmdAvailable("+COLR", g_uiCurrentCmdStampSs, cfw_event->nUTI))
        {
            return;
        }

        if (0 == cfw_event->nType) // success
        {
            // parameter1 parsing
            // 0 means don't presentation of the COLR.
            // 1 indicate presentation of the COLR

            // parameter2 parsing
            // 0 COLR not active
            // 1 COLR active
            // 2 unknown (e.g. no network, etc.)

            sprintf(aucBuffer, "+COLR:%u,%u", (uint8_t)cfw_event->nParam1, (uint8_t)cfw_event->nParam2);
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), cfw_event->nUTI, nSim);
        }
        else // if (0xfa == cfw_event->nType || (0xfc == cfw_event->nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
            }
        }
        pstOsEvent->nParam1 = 0;
    }

    break;

        // COLP read response event

    case EV_CFW_SS_QUERY_COLP_RSP: // EV_CSW_SS_QUERY_COLP_RSP:
    {
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+COLP", g_uiCurrentCmdStampSs, cfw_event->nUTI))
        {
            return;
        }

        if (0 == cfw_event->nType) // success
        {
            // parameter1 parsing
            // 0 Disable  to show the COLP
            // 1 Enable  to show the COLP

            // parameter2 parsing
            // 0 COLP not provisioned
            // 1 COLP provisioned
            // 2 unknown (e.g. no network, etc.)
            sprintf(aucBuffer, "+COLP:%u,%u", gATCurrentucColp, (uint8_t)cfw_event->nParam2);
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), cfw_event->nUTI, nSim);
        }
        else // if (0xfa == cfw_event->nType || (0xfc == cfw_event->nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
            }
        }
        pstOsEvent->nParam1 = 0;
    }

    break;

    // CUSD set response event
    case EV_CFW_SS_USSD_IND: // EV_CFW_SS_USSD_IND:
    {
        CFW_SS_USSD_IND_INFO *pUSSDInd = NULL;
        uint8_t nUSSDDCS = 0x00;
        uint8_t aucDestUsd[2 * MAX_LENGTH_STRING_USSD + 2] = {0};

        pUSSDInd = (CFW_SS_USSD_IND_INFO *)cfw_event->nParam1;

        AT_USSDParseDCS(pUSSDInd->nDcs, &nUSSDDCS);
        OSI_LOGI(0x10004510, "pUSSDInd->nDcs=%d, nUSSDDCS: %d\n", pUSSDInd->nDcs, nUSSDDCS);
        _GenerateCUSData(pUSSDInd->pUsdString, pUSSDInd->nStingSize, aucDestUsd, nUSSDDCS);

        AT_SS_SetUSSDNum(HIUINT16(pstOsEvent->nParam3));
        OSI_LOGXI(OSI_LOGPAR_IS, 0x10004511, "EV_CFW_SS_USSD_IND strlen(aucDestUsd)=%d, aucDestUsd: %s\n", strlen(aucDestUsd), aucDestUsd);
        sprintf(aucBuffer, "+CUSD: %u, \"%s\" ,%u", pUSSDInd->nOption, aucDestUsd, pUSSDInd->nDcs);
        AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, aucBuffer, AT_StrLen(aucBuffer), cfw_event->nUTI, nSim);
        break;
    }

    case EV_CFW_SS_SEND_USSD_RSP:
    {
        uint8_t nUSSDstr[256] = {
            0x00,
        };
        uint8_t nUSSDlen = 0x00;
        if (0x00 == cfw_event->nType) // success
        {
            CFW_SS_USSD_IND_INFO *pstSsUssdIndInfo = NULL;
            uint8_t aucDestUsd[2 * MAX_LENGTH_STRING_USSD + 2] = {0}; // 140
            uint8_t nUSSDDCS = 0xff;

            if (AT_isWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, cfw_event->nUTI))
            {
                AT_DelWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, cfw_event->nUTI);
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, cfw_event->nUTI, nSim);
            }
            if (0 == cfw_event->nParam1)
            {
                AT_SS_ResetUSSDState();
                if (0x00 != gATSATSendUSSDFlag[nSim])
                {
                    OSI_LOGI(0x10004512, "ATSendUSSD:gATSATSendUSSDFlag[nSim]%d\n", gATSATSendUSSDFlag[nSim]);
                    CFW_SatResponse(0x12, 0x00, 0x00, 0x00, 0x00, cfw_event->nUTI, nSim);
                    gATSATSendUSSDFlag[nSim] = FALSE;
                }
                return;
            }

            pstSsUssdIndInfo = (CFW_SS_USSD_IND_INFO *)cfw_event->nParam1;
            if (pstSsUssdIndInfo->nOption != 0 && pstSsUssdIndInfo->nOption != 1)
            {
                AT_SS_SetUSSDNum(HIUINT16(pstOsEvent->nParam3));
            }
            if (gATCurrentss_ussd && cfw_event->nParam1)
            {
                if (pstSsUssdIndInfo->nStingSize)
                {
                    AT_MemZero(nUSSDstr, 256);
                    if (pstSsUssdIndInfo != NULL && pstSsUssdIndInfo->pUsdString != NULL && pstSsUssdIndInfo->nStingSize != 0)
                    {
                        nUSSDstr[0] = pstSsUssdIndInfo->nDcs;
                        AT_MemCpy(&nUSSDstr[1], pstSsUssdIndInfo->pUsdString, pstSsUssdIndInfo->nStingSize);
                        nUSSDlen = 1 + pstSsUssdIndInfo->nStingSize;
                    }
                    //nUSSDDCS = AT_CBS_CheckDCS(pstSsUssdIndInfo->nDcs);
                    AT_USSDParseDCS(pstSsUssdIndInfo->nDcs, &nUSSDDCS);
                    nUSSDstr[0] = nUSSDDCS;
                    OSI_LOGI(0x10004513, "pstSsUssdIndInfo->nDcs=%d, nUSSDDCS: %d\n", pstSsUssdIndInfo->nDcs, nUSSDDCS);

                    _GenerateCUSData(pstSsUssdIndInfo->pUsdString, pstSsUssdIndInfo->nStingSize, aucDestUsd, nUSSDDCS);
                    OSI_LOGXI(OSI_LOGPAR_SI, 0x10004514, "EV_CFW_SS_SEND_USSD_RSP aucDestUsd: %s, strlen(aucDestUsd)=%d\n", aucDestUsd, strlen(aucDestUsd));
                    sprintf(aucBuffer, "+CUSD: %u, \"%s\" ,%u", pstSsUssdIndInfo->nOption, aucDestUsd,
                            pstSsUssdIndInfo->nDcs);
                }
                else
                {
                    sprintf(aucBuffer, "+CUSD: %u", pstSsUssdIndInfo->nOption);
                }

                AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, aucBuffer, AT_StrLen(aucBuffer), cfw_event->nUTI, nSim);
            }
        }
        else // if (0xfa == cfw_event->nType || (0xfc == cfw_event->nType)) //  Local Error /net work reject
        {
            OSI_LOGXI(OSI_LOGPAR_SI, 0x090004df, "SS func: %s nType = %x", __FUNCTION__, cfw_event->nType);
            if (AT_isWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, cfw_event->nUTI))
            {
                if ((cfw_event->nParam2 == 0x24) || (cfw_event->nParam2 == 0x22))
                {
                    AT_DelWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, cfw_event->nUTI);
                    AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, cfw_event->nUTI, nSim);
                    return;
                }
                AT_DelWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, cfw_event->nUTI);
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
            }

            if (0xFC == cfw_event->nType)
            {
                SS_AT_ReSendProcUssV1Req(cfw_event->nUTI, nSim);
                pstOsEvent->nParam1 = 0;
                return;
            }
            else
            {
                AT_SS_ResetUSSDState();
                AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
            }
        }

        if (0x00 != gATSATSendUSSDFlag[nSim])
        {
            if (0 == cfw_event->nType)
            {
                CFW_SS_USSD_IND_INFO *pstSsUssdIndInfo = NULL;
                pstSsUssdIndInfo = (CFW_SS_USSD_IND_INFO *)cfw_event->nParam1;

                if ((NULL != pstSsUssdIndInfo) && (NULL != pstSsUssdIndInfo->pUsdString))
                {
                    AT_TC_MEMBLOCK(g_sw_AT_SS, nUSSDstr, nUSSDlen, 16);
                }

                OSI_LOGI(0x10004515, "ATSendUSSD:nDcs=%d, nOption: %d, nStingSize:%d, nParam2:%d\n", pstSsUssdIndInfo->nDcs, pstSsUssdIndInfo->nOption, pstSsUssdIndInfo->nStingSize, cfw_event->nParam2);

                if (0x02 == pstSsUssdIndInfo->nOption)
                {
                    uint32_t nRet = CFW_SatResponse(0x12, 0x00, 0x00, nUSSDstr, nUSSDlen, cfw_event->nUTI, nSim);
                    OSI_LOGI(0x10004516, "CFW_SatResponse:CFW_SatResponse nOption =2 release complete nRet:%x\n", nRet);
                }
            }
            else
            {
            }

            gATSATSendUSSDFlag[nSim] = FALSE;
        }
    }
    break;

        // get PUC response

    case EV_CFW_SIM_GET_PUCT_RSP:
    {
        OSI_LOGI(0x10004517, "EV_CFW_SIM_GET_PUCT_RSP-COME\n");

        // check command life cycle

        if (!AT_IsAsynCmdAvailable("+CPUC", g_uiCurrentCmdStampSs, cfw_event->nUTI))
        {
            OSI_LOGI(0x10004518, "EV_CFW_SIM_GET_PUCT_RSP -----error---AT_IsAsynCmdAvailable");
            return;
        }

        OSI_LOGI(0x10004519, "EV_CFW_SIM_GET_PUCT_RSP-AT_IsAsynCmdAvailable SUCCESS\n");

        if (0 == cfw_event->nType) // success
        {
            CFW_SIM_PUCT_INFO *pstPucInfo = (CFW_SIM_PUCT_INFO *)cfw_event->nParam1;

            OSI_LOGI(0x1000451a, "EV_CFW_SIM_GET_PUCT_RSP-Currency=%c%c%c\n",
                     pstPucInfo->iCurrency[0], pstPucInfo->iCurrency[1], pstPucInfo->iCurrency[2]);
            OSI_LOGI(0x1000451b, "EV_CFW_SIM_GET_PUCT_RSP-iEPPU=%x,EX=%x,sign = %x\n", pstPucInfo->iEPPU, pstPucInfo->iEX, pstPucInfo->iSign);

            uint8_t currency[4];
            AT_MemZero(currency, 4);
            AT_MemCpy(currency, pstPucInfo->iCurrency, sizeof(pstPucInfo->iCurrency));

            // get ppu
            uint8_t aucPPU[MAX_LENGTH_PPU_CPUC];

            AT_MemZero(aucPPU, sizeof(aucPPU));
#if 0
            if (!SS_PUCT2String(aucPPU, pstPucInfo))
            {
                OSI_LOGI(0x1000451c, "SS_PPUIntStructureToFloatStr error-\n\r");
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
                return;
            }
#else
            //pstPucInfo->iEPPU = 1288;
            //pstPucInfo->iEX = 2;
            //pstPucInfo->iSign= 1;
            puct2string(pstPucInfo, aucPPU);
#endif
            OSI_LOGXI(OSI_LOGPAR_S, 0x1000451d, "==== aucPPU = %s ====", aucPPU);

            sprintf(aucBuffer, "+CPUC:\"%s\",\"%s\"", currency, aucPPU);

            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), cfw_event->nUTI, nSim);
            return;
        }
        else if (0xf0 == cfw_event->nType) // Local Error
        {
            OSI_LOGI(0x1000451e, "EV_CFW_SIM_GET_PUCT_RSP error-\n\r");
            uint32_t nErrorCode = AT_SetCmeErrorCode(cfw_event->nParam1, FALSE);

            AT_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
            pstOsEvent->nParam1 = 0;
            return;
        }
        else // return event error
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
            return;
        }
    }

    break;

        // set PUC response

    case EV_CFW_SIM_SET_PUCT_RSP:
    {
        // check command life cycle
        OSI_LOGI(0x1000451f, "EV_CFW_SIM_SET_PUCT_RSP-COME\n");

        if (!AT_IsAsynCmdAvailable("+CPUC", g_uiCurrentCmdStampSs, cfw_event->nUTI))
        {
            OSI_LOGI(0x10004520, "EV_CFW_SIM_SET_PUCT_RSP -----error---AT_IsAsynCmdAvailable");
            return;
        }

        OSI_LOGI(0x10004521, "EV_CFW_SIM_SET_PUCT_RSP-COME --AT_IsAsynCmdAvailable  SUCCESS\n");

        if (0 == cfw_event->nType) // success
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, cfw_event->nUTI, nSim);
            return;
        }
        else if (0xf0 == cfw_event->nType) // Local Error
        {
            OSI_LOGI(0x10004522, "EV_CFW_SIM_SET_PUCT_RSP error-\n\r");
            uint32_t nErrorCode = AT_SetCmeErrorCode(cfw_event->nParam1, FALSE);

            AT_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
            pstOsEvent->nParam1 = 0;
            return;
        }
        else // return event error
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfw_event->nUTI, nSim);
            return;
        }
    }

    break;

    default:
        uiErrCode = ERR_AT_CME_UNKNOWN_ERROR;

        AT_SS_EVENT_FAIL_RETURN(ppstResult, uiErrCode, NULL, 0, cfw_event->nUTI);

        break;
    }

    return;
}
#endif

// ATss class -> CFW class
uint8_t SS_ClassToCFWClass(uint8_t nclass)
{
    OSI_LOGI(0, "SS_ClassToCFWClass nclass=%d", nclass);
    switch (nclass)
    {
    case 1: // vioce
        nclass = 11;
        break;
    case 2: // data
        nclass = 20;
        break;
    case 4: // fax
        nclass = 13;
        break;
    case 8: // sms
        nclass = 16;
        break;
    default:
        nclass = 11;
        break;
    }
    return nclass;
    OSI_LOGI(0, "SS_ClassToCFWClass End nclass=%d", nclass);
}

static void CCWA_QueryRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    OSI_LOGI(0, "CCWA_QueryRspCB:nType=%d nParam2=%d", cfw_event->nType, cfw_event->nParam2);
    if (0 == cfw_event->nType) // success  VOICE DATA FAX SMS ...
    {
        char rsp[32];
        for (int n = 0; n < 3; n++)
        {
            int status = (cfw_event->nParam1 & (1 << n)) ? 1 : 0;
            int nclass = (1 << n);

            sprintf(rsp, "+CCWA: %d,%d", status, nclass);
            atCmdRespInfoText(cmd->engine, rsp);
        }
        atCmdRespOK(cmd->engine);
    }
    else //type:0xfa:LocalError 0xfb:NwkReturnError 0xfc:Nwk Rej
    {
        unsigned err_code = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
        RETURN_CME_ERR(cmd->engine, err_code);
    }
}

static void CCWA_SetRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (0 == cfw_event->nType) // success  VOICE DATA FAX SMS ...
    {
        if (cfw_event->nParam2 == 0x05)
            OSI_LOGI(0, "there is an active 4g call,the 4g ccwa is set success,2g ccwa set faild");
        atCmdRespOK(cmd->engine);
    }
    else
    {
        unsigned err_code = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
        OSI_LOGI(0, "CCWA:  err_code=%d", err_code);
        RETURN_CME_ERR(cmd->engine, err_code);
    }
}

// // 7.12 Call waiting
void atCmdHandleCCWA(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        // +CCWA=[<n>[,<mode>[,<class>]]]
        bool paramok = true;
        uint8_t n = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 3)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        // set URC control value
        gAtSetting.sim[nSim].ccwa = n;
        if (cmd->param_count <= 1)
        {
            RETURN_OK(cmd->engine);
        }

        uint8_t mode = atParamUintInRange(cmd->params[1], 0, 2, &paramok);
        uint8_t nclass = atParamDefUintInRange(cmd->params[2], 1, 1, 1, &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        nclass = SS_ClassToCFWClass(nclass);
        if (mode == 2)
        {
            cmd->uti = cfwRequestUTI((osiEventCallback_t)CCWA_QueryRspCB, cmd);
            if (CFW_SsQueryCallWaiting(nclass, cmd->uti, nSim) != 0)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            RETURN_FOR_ASYNC();
        }
        else
        {
            cmd->uti = cfwRequestUTI((osiEventCallback_t)CCWA_SetRspCB, cmd);
            if (CFW_SsSetCallWaiting(mode, nclass, cmd->uti, nSim) != 0)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            RETURN_FOR_ASYNC();
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CCWA: (0,1)");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
        sprintf(rsp, "+CCWA: %u", gAtSetting.sim[nSim].ccwa);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: change class to display
//
//
// Parameter:
// Caller:
// Called:
// Return:
// Remark:
//
// ////////////////////////////////////////////////////////////////////////////
void AT_SsClassConvert2Ccfc(uint8_t ucSrcClass, uint8_t *pucDestClass)
{
    OSI_LOGI(0x10004506, "<><><><><><>:%d\n", ucSrcClass);

    switch (ucSrcClass)
    {

    case 11:               // voice
        *pucDestClass = 1; // real value 2^0
        break;

    case 13:               // fax
        *pucDestClass = 4; // 2^1
        break;

    case 16: // sms
        *pucDestClass = 8;
        break;

    case 20: // data
        *pucDestClass = 2;
        break;

    default:
        *pucDestClass = 1;
        break;
    }
}

static void _CallForwardRspTimeOutCB(atCommand_t *cmd)
{
    OSI_LOGI(0, "_CallForwardRspTimeOutCB: uti=%d", cmd->uti);
    if (AT_CFW_UTI_INVALID != cmd->uti)
        cfwReleaseUTI(cmd->uti);
    AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_SEND_TIMEOUT));
}

static void QueryCallForwardCallBack(atCommand_t *cmd, const osiEvent_t *event)
{
    //EV_CFW_SS_QUERY_CALL_FORWARDING_RSP  // EV_CSW_SS_QUERY_CF_RSP
    char aucBufferTemp[256] = {0};
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    OSI_LOGI(0, "QueryCallForwardCallBack:event_type=%d", cfw_event->nType);
    if (0 == cfw_event->nType) // success
    {
        CFW_SS_QUERY_CALLFORWARDING_RSP *pstSsQueryCallForwardingRsp = NULL;
        uint8_t aucAsciiNumber[43] = {0}; // 43
        uint8_t ucDestClass = 0;
        uint8_t aucStatus = 0;
        uint8_t aucType = 0;

        pstSsQueryCallForwardingRsp = (CFW_SS_QUERY_CALLFORWARDING_RSP *)(cfw_event->nParam1);

        // convert class software and 3gpp
        AT_SsClassConvert2Ccfc(pstSsQueryCallForwardingRsp->nClass, &ucDestClass);
        OSI_LOGI(0, "QueryCallForwardCallBack:nClass=%d ucDestClass=%d", pstSsQueryCallForwardingRsp->nClass, ucDestClass);

        aucType = pstSsQueryCallForwardingRsp->nNumber.nType;
        aucStatus = pstSsQueryCallForwardingRsp->nStatus;
        OSI_LOGI(0, "QueryCallForwardCallBack:aucType=%d aucStatus=%d", aucType, aucStatus);
        // paucAsciiNumber[ucDestClass] = aucAsciiNumber;
        // set status of current class is 1
        // [[Changyg[mod] 2008-04-22 :add "+" when acuType is 145

        if (145 == aucType)
        {
            aucAsciiNumber[0] = 0x2b;
            SUL_GsmBcdToAscii(pstSsQueryCallForwardingRsp->nNumber.nTelNumber, pstSsQueryCallForwardingRsp->nNumber.nSize,
                              &aucAsciiNumber[1]);
        }
        else
        {
            SUL_GsmBcdToAscii(pstSsQueryCallForwardingRsp->nNumber.nTelNumber, pstSsQueryCallForwardingRsp->nNumber.nSize,
                              aucAsciiNumber);
        }
        OSI_LOGXI(OSI_LOGPAR_S, 0, "QueryCallForwardCallBack:AsciiNumber=%s", aucAsciiNumber);
        // ]]Changyg[mod] 2008-04-22 :end

        // CCFC respond
        // parameter2 is size of structure, not The counter of structure array
        char *aucBufferTemp2 = aucBufferTemp;
        if (strcmp("+CCFC", cmd->desc->name) == 0)
            aucBufferTemp2 += sprintf(aucBufferTemp2, "+CCFC:");
        else
            aucBufferTemp2 += sprintf(aucBufferTemp2, "+CCFCU:");
        if (pstSsQueryCallForwardingRsp->nStatus)
        {
            OSI_LOGI(0, "QueryCallForwardCallBack:pstSsQueryCallForwardingRsp->nTime=%d", pstSsQueryCallForwardingRsp->nTime);
            if (!pstSsQueryCallForwardingRsp->nTime)
            {
                // number exist,display all information
                sprintf(aucBufferTemp2, "%u,%u,\"%s\",%u", aucStatus, ucDestClass, aucAsciiNumber, aucType);
            }
            else
            {
                sprintf(aucBufferTemp2, "%u,%u,\"%s\",%u,,,%u", aucStatus, ucDestClass, aucAsciiNumber, aucType,
                        pstSsQueryCallForwardingRsp->nTime);
            }
        }
        else
            sprintf(aucBufferTemp2, "%u,%u", aucStatus, ucDestClass);
        // OK retrun
        atCmdRespInfoText(cmd->engine, aucBufferTemp);
        atMemFreeLater((void *)cfw_event->nParam1);
        RETURN_OK(cmd->engine);
    }
    else
    {
        uint32_t uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
        // ERROR return
        if (ERR_SUCCESS != uiErrCode)
            RETURN_CME_ERR(cmd->engine, uiErrCode);
    }
}

static void SetCallForwardCallBack(atCommand_t *cmd, const osiEvent_t *event)
{
    //EV_CFW_SS_SET_CALL_FORWARDING_RSP   //EV_CSW_SS_SET_CF_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    OSI_LOGI(0, "SetCallForwardCallBack: eventID:%d", cfw_event->nEventId);

    if (0 == cfw_event->nType) // success
    {
        atMemFreeLater((void *)cfw_event->nParam1);
        RETURN_OK(cmd->engine);
    }
    else // if (0xfa == stCfwEvent.nType || (0xfc == stCfwEvent.nType)) //  Local Error /net work reject
    {
        uint32_t uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
        OSI_LOGI(0, "SetCallForwardCallBack: uiErrCode=%d", uiErrCode);
        RETURN_CME_ERR(cmd->engine, uiErrCode);
    }
}

// Communication forwarding number and conditions with URI support
// Param ton:129, 145, 161
// Param number_type: 0, 1, 2
void atCmdHandleCCFCU(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        static const uint32_t ton_list[] = {CFW_TELNUMBER_TYPE_UNKNOWN, CFW_TELNUMBER_TYPE_INTERNATIONAL, CFW_TELNUMBER_TYPE_NATIONAL};
        static const uint32_t class_list[] = {1, 2, 4, 8};
        uint32_t result = 0;
        uint8_t nSim = atCmdGetSim(cmd->engine);
        bool paramok = true;
        uint8_t reason = atParamDefUintInRange(cmd->params[0], 0, 0, 7, &paramok);
        uint8_t mode = atParamDefUintInRange(cmd->params[1], 0, 0, 4, &paramok);
        uint8_t number_type = atParamDefUintInRange(cmd->params[2], 0, 0, 2, &paramok);
        uint8_t ton = atParamDefUintInList(cmd->params[3], 129, ton_list, 3, &paramok);
        const char *number = atParamOptStr(cmd->params[4], &paramok);
        uint8_t nClass = atParamDefUintInList(cmd->params[5], 1, class_list, 4, &paramok);
        uint8_t nTime = atParamDefUintInRange(cmd->params[6], 20, 1, 30, &paramok);
        size_t number_len = strlen(number);
        if (cmd->param_count > 7 || !paramok)
        {
            OSI_LOGI(0, "CCFCU error: param_count=%d paramok=%d", cmd->param_count, paramok);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        nClass = SS_ClassToCFWClass(nClass);
        OSI_LOGI(0, "CCFCU: mode=%d nClass=%d", mode, nClass);
        if (mode == 2) // query status
        {
            //EV_CFW_SS_QUERY_CALL_FORWARDING_RSP
            cmd->uti = cfwRequestUTI((osiEventCallback_t)QueryCallForwardCallBack, cmd);
            atCmdSetTimeoutHandler(cmd->engine, 1000 * 90, _CallForwardRspTimeOutCB);
            result = CFW_SsQueryCallForwarding(reason, nClass, cmd->uti, nSim);
            if (result != ERR_SUCCESS)
            {
                cfwReleaseUTI(cmd->uti);
                OSI_LOGI(0, "CCFCU error:CFW_SsQueryCallForwarding result=%d", result);
                RETURN_CME_ERR(cmd->engine, atCfwToCmeError(result));
            }
            RETURN_FOR_ASYNC();
        }
        if (number_type == CFW_TELNUMBER_TYPE_URI)
            ton = 0;
        OSI_LOGI(0, "CCFCU: ton=%d number_len=%d number_type=%d", ton, number_len, number_type);
        // handle phone number
        uint8_t bcd_num[MAX_LENGTH_NUMBER_CCFCU] = {0};
        char dial_num[MAX_LENGTH_NUMBER_CCFCU] = {0};
        uint16_t bcd_len = 0;
        uint8_t dial_num_len = 0;
        if (number_type == CFW_TELNUMBER_TYPE_INVALID)
        {
            number = "";
            number_len = 0;
            OSI_LOGXI(OSI_LOGPAR_SI, 0, "CCFCU: number=%s number_len=%d", number, number_len);
        }
        else
        {
            uint8_t optype = 0;
            if (number_type == CFW_TELNUMBER_TYPE_URI)
            {
                optype = UriParaParse(number, dial_num);
                dial_num_len = strlen(dial_num);
                OSI_LOGXI(OSI_LOGPAR_S, 0, "CCFCU: number=%s", number);
                if (optype == AT_CMD_CCFCU_OPTYPE_VOICECALL)
                {
                    if (dial_num_len > MAX_LENGTH_NUMBER_CCFCU)
                        dial_num_len = MAX_LENGTH_NUMBER_CCFCU;
                    if (dial_num[0] == '+')
                    {
                        dial_num_len--;
                        ton = CFW_TELNUMBER_TYPE_INTERNATIONAL;
                        bcd_len = AT_SsAsciiToGsmBcd((dial_num + 1), dial_num_len, bcd_num);
                    }
                    else if (dial_num[0] == '0' && dial_num[1] == '0')
                    {
                        dial_num_len -= 2;
                        bcd_len = AT_SsAsciiToGsmBcd((dial_num + 2), dial_num_len, bcd_num);
                    }
                    else
                        bcd_len = AT_SsAsciiToGsmBcd(dial_num, dial_num_len, bcd_num);
                    dial_num_len = bcd_len;
                }
                else
                {
                    OSI_LOGI(0, "CCFCU:optype=%d is not uri");
                    number_len = 0;
                }
                OSI_LOGI(0, "CCFCU:01--dial_num_len=%d", dial_num_len);
            }
            else
            {
                for (uint8_t i = 1; i < number_len; i++)
                {
                    if ((number[i] < 0x30) || (number[i] > 0x39))
                    {
                        OSI_LOGD(0, "CCFCU: number[%d]=%c", i, number[i]);
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_CHAR_INDIAL);
                    }
                }
                if (number[0] == '+')
                {
                    ton = CFW_TELNUMBER_TYPE_INTERNATIONAL;
                    bcd_len = AT_SsAsciiToGsmBcd((number + 1), number_len - 1, bcd_num);
                }
                else if ((number[0] == '0') && (number[1] == '0'))
                {
                    number_len -= 2;
                    bcd_len = AT_SsAsciiToGsmBcd(number + 2, number_len, bcd_num);
                }
                else
                    bcd_len = AT_SsAsciiToGsmBcd(number, number_len, bcd_num);
                dial_num_len = bcd_len;
                OSI_LOGI(0, "CCFCU:02--dial_num_len=%d", dial_num_len);
            }
        }
        CFW_SS_SET_CALLFORWARDING_INFO ss_setccfcu;
        memset(&ss_setccfcu, 0, sizeof(CFW_SS_SET_CALLFORWARDING_INFO));
        ss_setccfcu.nReason = reason;
        ss_setccfcu.nMode = mode;
        ss_setccfcu.nClass = nClass;
        ss_setccfcu.nTime = nTime;
        OSI_LOGI(0, "CCFCU:03--number_len=%d", number_len);
        if (number_len)
        {
            ss_setccfcu.nNumber.nDialNumberSize = bcd_len;
            ss_setccfcu.nNumber.nType = ton;
            memcpy(ss_setccfcu.nNumber.pDialNumber, bcd_num, TEL_NUMBER_MAX_LEN);
            ss_setccfcu.nNumber.nClir = 0;

            ss_setccfcu.nNumber.nNumType = number_type;
            ss_setccfcu.nNumber.nExtend = 1;
        }
        OSI_LOGXI(OSI_LOGPAR_S, 0, "CCFCU:bcd_num=%s", bcd_num);
        //EV_CFW_SS_SET_CALL_FORWARDING_RSP
        cmd->uti = cfwRequestUTI((osiEventCallback_t)SetCallForwardCallBack, cmd);
        atCmdSetTimeoutHandler(cmd->engine, 1000 * 90, _CallForwardRspTimeOutCB);
        result = CFW_SsSetCallForwarding(&ss_setccfcu, cmd->uti, nSim);
        if (result != ERR_SUCCESS)
        {
            cfwReleaseUTI(cmd->uti);
            OSI_LOGI(0, "CCFCU error: CFW_SsSetCallForwarding result=%d", result);
            RETURN_CME_ERR(cmd->engine, atCfwToCmeError(result));
        }
        RETURN_FOR_ASYNC();
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        OSI_LOGI(0, "CCFCU format:reason,mode,numbertype,ton,number,class,time");
        atCmdRespInfoText(cmd->engine, "+CCFCU:(0-7)");
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: AT+CCFC = <reason>,<mode!=2>[,<number>[,<type>[,<class>[,<subaddr>[,<satype>[,<time>]]]]]]
// Call forwarding number and conditions
// AT+CCFC=0,3,"02164708411",129
// Parameter:   pParam, parsed command structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      query mode,only for single class,not for all class
//
// ////////////////////////////////////////////////////////////////////////////

void atCmdHandleCCFC(atCommand_t *cmd)
{
    osiThreadSleep(50);
    uint32_t uiRetVal;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    // check event respond exclusive
    //g_uiCurrentCmdStampSs = pParam->uCmdStamp;
    OSI_LOGI(0, "CCFC: Come in atCmdHandleCCFC nSim=%d", nSim);
    if (AT_CMD_SET == cmd->type)
    {
        uint8_t ucNum[TEL_NUMBER_MAX_LEN] = {
            0,
        };

        OSI_LOGI(0, "CCFC:param_count=%d", cmd->param_count);
        if ((8 < cmd->param_count) || cmd->param_count < 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        // first and second parameter
        static const uint32_t NumTypeList[] = {CFW_TELNUMBER_TYPE_UNKNOWN, CFW_TELNUMBER_TYPE_INTERNATIONAL, CFW_TELNUMBER_TYPE_NATIONAL};
        bool paramok = true;
        uint8_t ucReason = atParamDefUintInRange(cmd->params[0], 0, 0, 5, &paramok);
        uint8_t ucMode = atParamDefUintInRange(cmd->params[1], 0, 0, 4, &paramok);
        const char *tempNum = atParamOptStr(cmd->params[2], &paramok);
        uint8_t ucNumType = atParamDefUintInList(cmd->params[3], CFW_TELNUMBER_TYPE_UNKNOWN, NumTypeList, 3, &paramok);
        static const uint32_t class_list[] = {1, 2, 4, 8};
        uint8_t ucClass = atParamDefUintInList(cmd->params[4], 1, class_list, 4, &paramok);
        uint8_t ucTime = atParamDefUintInRange(cmd->params[7], 20, 5, 30, &paramok);
        size_t ucNumLen = strlen(tempNum);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (ucNumLen > 20)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        ucClass = SS_ClassToCFWClass(ucClass);
        // output debug information
        // eighth parameter: time
        // excuete condition
        if (ucMode == 2) // query status
        {
            OSI_LOGI(0, "CCFC: event EV_CFW_SS_QUERY_CALL_FORWARDING_RSP ucMode=%d", ucMode);
            //EV_CFW_SS_QUERY_CALL_FORWARDING_RSP
            cmd->uti = cfwRequestUTI((osiEventCallback_t)QueryCallForwardCallBack, cmd);
            atCmdSetTimeoutHandler(cmd->engine, 1000 * 90, _CallForwardRspTimeOutCB);
            uiRetVal = CFW_SsQueryCallForwarding(ucReason, ucClass, cmd->uti, nSim);
            OSI_LOGI(0x10004533, "CCFC:uiRetVal:0x%x reason=%d,   class=%d\n", uiRetVal, ucReason, ucClass);
            if (uiRetVal != ERR_SUCCESS)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_ERR(cmd->engine, atCfwToCmsError(uiRetVal));
            }
            RETURN_FOR_ASYNC();
        }
        else // set call forwarding
        {
            OSI_LOGI(0, "CCFC:SetCallForward ucReason:%d ucMode:%d ucClass:%d ucTime:%d", ucReason, ucMode, ucClass, ucTime);
            CFW_SS_SET_CALLFORWARDING_INFO sSetCCFC;

            memset(&sSetCCFC, 0, sizeof(CFW_SS_SET_CALLFORWARDING_INFO));
            sSetCCFC.nReason = ucReason;
            sSetCCFC.nMode = ucMode;
            sSetCCFC.nClass = ucClass;
            sSetCCFC.nTime = ucTime;
            if (ucNumLen)
            {
                bool bIsInternational = false;
                if (!AT_SS_IsValidPhoneNumber(tempNum, ucNumLen, &bIsInternational) && (ucMode != 2)) //check phonenumber
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                if (bIsInternational)
                {
                    tempNum++;
                    ucNumLen--;
                    if (ucNumType != CFW_TELNUMBER_TYPE_INTERNATIONAL)
                    {
                        OSI_LOGI(0, "CCFC: International,phonenumber and Numtype Mismatch");
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                }
                uint8_t ucNumSize = SUL_AsciiToGsmBcd(tempNum, ucNumLen, ucNum);
                OSI_LOGXI(OSI_LOGPAR_SII, 0, "CCFC:ucNum:%s ucNumType:%d ucNumSize:%d", ucNum, ucNumType, ucNumSize);
                sSetCCFC.nNumber.nDialNumberSize = ucNumSize;
                sSetCCFC.nNumber.nType = ucNumType;
                memcpy(sSetCCFC.nNumber.pDialNumber, ucNum, TEL_NUMBER_MAX_LEN);
                sSetCCFC.nNumber.nClir = 0;
            }
            //EV_CFW_SS_SET_CALL_FORWARDING_RSP
            OSI_LOGI(0, "CCFC:event set EV_CFW_SS_SET_CALL_FORWARDING_RSP ucMode:%d\n", ucMode);
            cmd->uti = cfwRequestUTI((osiEventCallback_t)SetCallForwardCallBack, cmd);
            atCmdSetTimeoutHandler(cmd->engine, 1000 * 90, _CallForwardRspTimeOutCB);
            uiRetVal = CFW_SsSetCallForwarding(&sSetCCFC, cmd->uti, nSim);
            if (uiRetVal != ERR_SUCCESS)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_ERR(cmd->engine, atCfwToCmsError(uiRetVal));
            }
            RETURN_FOR_ASYNC();
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        // return command format "+CCFC:<0-5>
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		atCmdRespInfoText(cmd->engine, "+CCFC: (0-5)");
#else
		atCmdRespInfoText(cmd->engine, "+CCFC:(0-5)");
#endif

        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

static void CLIP_queryRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    // EV_CFW_SS_QUERY_CLIP_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cfw_event->nType != 0)
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);

    // +CLIP: <n>,<m>
    char rsp[32];
    sprintf(rsp, "+CLIP: %d,%d", gAtSetting.sim[nSim].clip, (int)cfw_event->nParam2);
    atCmdRespInfoText(cmd->engine, rsp);
    RETURN_OK(cmd->engine);
}

// 7.6 Calling line identification presentation
void atCmdHandleCLIP(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        // +CLIP=[<n>]
        bool paramok = true;
        uint8_t n = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            // call CSW interface function
#ifdef CFW_VOLTE_SUPPORT
        cmd->uti = cfwRequestNoWaitUTI();
        CFW_SsSetClip(n, cmd->uti, nSim);
#endif
        gAtSetting.sim[nSim].clip = n;
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CLIP: (0,1)");
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        cmd->uti = cfwRequestUTI((osiEventCallback_t)CLIP_queryRspCB, cmd);
        if (CFW_SsQueryClip(cmd->uti, nSim) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        RETURN_FOR_ASYNC();
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}
#ifndef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
static void CLIR_queryRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    // EV_CFW_SS_QUERY_CLIR_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cfw_event->nType != 0)
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);

    // +CLIR: <n>,<m>
    char rsp[32];
    sprintf(rsp, "+CLIR: %d,%d", gAtSetting.sim[nSim].clir, (int)cfw_event->nParam2);
    atCmdRespInfoText(cmd->engine, rsp);
    RETURN_OK(cmd->engine);
}
#endif
// 7.7 Calling line identification restriction
void atCmdHandleCLIR(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        // +CLIR=[<n>]
        bool paramok = true;
        uint8_t n = atParamDefUintInRange(cmd->params[0], 0, 0, 2, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#ifdef CFW_VOLTE_SUPPORT
        cmd->uti = cfwRequestNoWaitUTI();
        CFW_SsSetClir(n, cmd->uti, nSim);
#endif
        gAtSetting.sim[nSim].clir = n;
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CLIR: (0-2)");
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		char rsp[16] = {0};
		snprintf(rsp,sizeof(rsp),"+CLIR: %d,2", gAtSetting.sim[nSim].clir);
		atCmdRespInfoText(cmd->engine, rsp);
		RETURN_OK(cmd->engine);
#else

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CLIR_queryRspCB, cmd);
        if (CFW_SsQueryClir(cmd->uti, nSim) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        RETURN_FOR_ASYNC();
#endif
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: +CSSN=[<n>[,<m>]]
// Supplementary service notifications,+CSSI messages,+CSSU messages
// AT+CSSN=1,0
// Parameter:   pParam, parsed command structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      have on/off function to CC module
//
// ////////////////////////////////////////////////////////////////////////////

void atCmdHandleCSSN(atCommand_t *cmd)
{
    uint8_t ucCSSI = 0;
    uint8_t ucCSSU = 0;
    char aucBuffer[20] = {0};
    uint32_t uiRetVal = ERR_SUCCESS;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    OSI_LOGI(0, "CSSN: Come In atCmdHandleCSSN nSim=%d", nSim);
    // call ATM function
    if (AT_CMD_SET == cmd->type)
    {
        OSI_LOGI(0, "CSSN: param_count=%d", cmd->param_count);
        if (cmd->param_count < 1 || cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        bool paramok = true;
        ucCSSI = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        ucCSSU = atParamDefUintInRange(cmd->params[1], 0, 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        OSI_LOGI(0, "CSSN:Parameter ucCSSI: %d  Parameter ucCSSU: %d\n", ucCSSI, ucCSSU);
        gAtSetting.sim[nSim].cssi = ucCSSI;
        gAtSetting.sim[nSim].cssu = ucCSSU;
        // call CSW interface function
        uiRetVal = CFW_CfgSetSSN(ucCSSI, ucCSSU, nSim);
        OSI_LOGI(0x10004545, "CSSN set ret: %x\n", uiRetVal);
        if (ERR_SUCCESS != uiRetVal)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PHONE_FAILURE);
        RETURN_OK(cmd->engine);
    }
	
    else if (AT_CMD_TEST == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		atCmdRespInfoText(cmd->engine, "+CSSN: (0,1),(0,1)");
#else
        atCmdRespInfoText(cmd->engine, "+CSSN:(0-1),(0-1)");
#endif
        RETURN_OK(cmd->engine);
    }

    else if (AT_CMD_READ == cmd->type)
    {
        // call CSW interface function
        uiRetVal = CFW_CfgGetSSN(&ucCSSI, &ucCSSU, nSim);
        OSI_LOGI(0x10004546, "CSSN ret: %x ucCSSI=%d ucCSSU=%d\n", uiRetVal, ucCSSI, ucCSSU);
        if (ERR_SUCCESS != uiRetVal)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PHONE_FAILURE);
        // execute result return
        //sprintf(aucBuffer, strRCSSN, gATCurrentucCSSI, gATCurrentucCSSU);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		sprintf(aucBuffer, "+CSSN: %d,%d", ucCSSI, ucCSSU);
#else
        sprintf(aucBuffer, "+CSSN:%d,%d", ucCSSI, ucCSSU);
#endif
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }

    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
}
#ifndef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
static void COLP_RspCallBack(atCommand_t *cmd, const osiEvent_t *event)
{
    //case EV_CFW_SS_QUERY_COLP_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    char aucBuffer[20] = {0};
    uint32_t uiErrCode = 0;
    OSI_LOGI(0, "COLP:Come in COLP_RspCallBack event ID=%d", cfw_event->nEventId);

    if (0 == cfw_event->nType) // success
    {
        // parameter1 parsing
        // 0 Disable  to show the COLP
        // 1 Enable  to show the COLP

        // parameter2 parsing
        // 0 COLP not provisioned
        // 1 COLP provisioned
        // 2 unknown (e.g. no network, etc.)
        uint8_t GetColp;
        uint32_t ColpErrCode = CFW_CfgGetColp(&GetColp, nSim);
        if (ColpErrCode != 0)
            RETURN_CME_ERR(cmd->engine, atCfwToCmeError(ColpErrCode));
        sprintf(aucBuffer, "%s: %u,%u", cmd->desc->name, GetColp, (uint8_t)cfw_event->nParam2);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else // if (0xfa == cfw_event->nType || (0xfc == cfw_event->nType)) //  Local Error /net work reject
    {
        uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
        RETURN_CME_ERR(cmd->engine, uiErrCode);
    }
}
#endif
void atCmdHandleCOLP(atCommand_t *cmd)
{
    uint32_t uiRetVal = ERR_SUCCESS;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    OSI_LOGI(0, "COLP:Come In atCmdHandleCOLP nSim=%d", nSim);

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint8_t n = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            // call CSW interface function
#ifdef CFW_VOLTE_SUPPORT
        cmd->uti = cfwRequestNoWaitUTI();
        CFW_SsSetColp(n, cmd->uti, nSim);
        OSI_LOGI(0, "COLP: n====%d", n);
#endif
        uiRetVal = CFW_CfgSetColp(n, nSim);
        if (ERR_SUCCESS == uiRetVal)
            RETURN_OK(cmd->engine);
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[20] = {0};
        sprintf(aucBuffer, "%s: (0,1)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		char rsp[16] = {0};
		uint8_t n;
		CFW_CfgGetColp(&n, nSim);
		snprintf(rsp,sizeof(rsp),"+COLP: %d,2",n);
		atCmdRespInfoText(cmd->engine, rsp);
		RETURN_OK(cmd->engine);
#else
        // call CSW interface function
        cmd->uti = cfwRequestUTI((osiEventCallback_t)COLP_RspCallBack, cmd);
        uiRetVal = CFW_SsQueryColp(cmd->uti, nSim); // network don't support
        // output debug information
        OSI_LOGI(0x1000453d, "COLP read ret: %x\n", uiRetVal);
        if (ERR_SUCCESS != uiRetVal)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, atCfwToCmeError(uiRetVal));
        }
        RETURN_FOR_ASYNC();
#endif
    }

    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
}

#ifdef PORTING_ON_GOING
// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description:
// Connected Line Identification Restriction
// Parameter:  pParam, parsed command structure
// Caller:
// Called:     ATM
// Return:     fail or succeed
// Remark:     set only on/off function
//
// ////////////////////////////////////////////////////////////////////////////

void atCmdHandleCOLR(atCommand_t *cmd)
{
    uint32_t uiServiceId = CFW_SS_SRV_ID;
    uint32_t uiRetVal = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    PAT_CMD_RESULT pstResult;
    PAT_CMD_RESULT *ppstResult = &pstResult;
#ifdef AT_SS_DEBUG
    OSI_LOGXI(OSI_LOGPAR_SI, 0x10004525, "Parameter string: %s  Parameter type: %d\n", pParam->pPara, cmd->type);
#endif
    if (NULL == pParam)
    {
        AT_SS_FAIL_RETURN(ppstResult, CMD_FUNC_FAIL, (uint8_t *)strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);
        return;
    }
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;
    switch (cmd->type)
    {
    case AT_CMD_SET:
        AT_SS_FAIL_RETURN(ppstResult, ERR_AT_CME_OPERATION_NOT_SUPPORTED, (uint8_t *)strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);
        break;

    case AT_CMD_TEST:
        AT_SS_SYN_RETURN(ppstResult, (uint8_t *)strTCOLR, AT_StrLen(strTCOLR), pParam->nDLCI);
        break;

    case AT_CMD_READ:
    case AT_CMD_EXE:
        uiRetVal = AT_GetFreeUTI(uiServiceId, &pParam->nDLCI);
        if (ERR_SUCCESS != uiRetVal)
        {
            AT_SS_FAIL_RETURN(ppstResult, CMD_FUNC_FAIL, (uint8_t *)strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);
        }
        uiRetVal = CFW_SsQueryColr(pParam->nDLCI, nSim);
#ifdef AT_SS_DEBUG
        OSI_LOGI(0x1000453e, "COLR read ret: %x\n", uiRetVal);
#endif
        if (ERR_SUCCESS == uiRetVal)
        {
            AT_SS_TRUE_RETURN(ppstResult, CMD_FUNC_SUCC_ASYN, ASYN_DELAY_COLR, NULL, 0, pParam->nDLCI);
        }
        else
        {
            AT_SS_FAIL_RETURN(ppstResult, CMD_FUNC_FAIL, (uint8_t *)strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);
        }

        break;
    default:
        AT_SS_FAIL_RETURN(ppstResult, CMD_FUNC_FAIL, (uint8_t *)strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);
        break;
    }
    if (ppstResult != NULL)
    {
        free(ppstResult);
        ppstResult = NULL;
    }
    return;
}
#endif

#if QUEC_PATCH_CUSD
typedef struct
{
    uint8_t nDcs;
    uint8_t *ussdStr;
} cusdContext_t;

static void CUSD_AsyncCtxDestroy(atCommand_t *cmd)
{
    cusdContext_t *ctx = (cusdContext_t *)cmd->async_ctx;
    if (ctx == NULL)
        return;

    free(ctx->ussdStr);
    free(ctx);
    cmd->async_ctx = NULL;
}

static void CUSD_InputUssdStrCB(void *param, atCmdPromptEndMode_t end_mode, size_t ussdStrSize)
{
    atCommand_t *cmd = (atCommand_t *)param;
    cusdContext_t *ctx = (cusdContext_t *)cmd->async_ctx;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t result = 0;
    uint8_t u7bitStr[MAX_LENGTH_STRING_USSD] = {0};

    if (end_mode == AT_PROMPT_END_ESC)
        RETURN_OK(cmd->engine);
    if (end_mode == AT_PROMPT_END_OVERFLOW)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
    // trim last CTRL_Z
    ussdStrSize--;

    uint8_t uti_v2 = AT_SS_GetUSSDNum();
    OSI_LOGI(0, "CUSD_InputUssdStrCB: asyncUti=%d  cmdUti=%d, ucDcs=%d", uti_v2, cmd->uti, ctx->nDcs);
    if (ctx->nDcs == 15)
    {
        uint8_t u8bitStr[MAX_LENGTH_STRING_USSD] = {0};
        memcpy(u8bitStr, ctx->ussdStr, ussdStrSize);
        result = CFW_SsSendUSSD(u8bitStr, ussdStrSize, 131, ctx->nDcs, uti_v2, nSim);
    }
    else
    {
        g_ss_ussdVer = 2;
        result = SUL_Encode7Bit(ctx->ussdStr, u7bitStr, ussdStrSize);
        uint8_t u7bitLen = (ussdStrSize % 8) ? (ussdStrSize * 7 / 8 + 1) : (ussdStrSize * 7 / 8);
        u7bitStr[u7bitLen] = '\0';
        OSI_LOGI(0, "CUSD_InputUssdStrCB: SUL_Encode7Bit result=%d\n", result);
        result = CFW_SsSendUSSD(u7bitStr, u7bitLen, 3, ctx->nDcs, uti_v2, nSim);
    }
    if (result != 0)
    {
        OSI_LOGI(0, "CUSD_InputUssdStrCB: CFW_SsSendUSSD: result=0x%x\n", result);
        cfwReleaseUTI(cmd->uti);
        AT_SS_ResetUSSDState();
        RETURN_CME_CFW_ERR(cmd->engine, result);
    }
    RETURN_FOR_ASYNC();
}
#endif

static void CUSD_RspCallBack(atCommand_t *cmd, const osiEvent_t *event)
{
    //EV_CFW_SS_SEND_USSD_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t totalLen;
    char *aucBuffer = NULL;
    uint32_t uiErrCode = 0;
    OSI_LOGI(0, "CUSD_RspCallBack:event ID=%d type=%d", cfw_event->nEventId, cfw_event->nType);
    uint8_t nUSSDstr[256] = {
        0x00,
    };
#if QUEC_PATCH_CUSD
	cusdContext_t *ctx = (cusdContext_t *)cmd->async_ctx;
#endif
    if (0 == cfw_event->nType) // success
    {
        CFW_SS_USSD_IND_INFO_V2 *pstSsUssdIndInfo = NULL;
        uint8_t aucDestUsd[2 * MAX_LENGTH_STRING_USSD + 2] = {0}; // 140
        uint8_t nUSSDDCS = 0xff;
        OSI_LOGI(0, "CUSD: CUSD_RspCallBack cfw_event->nParam1=%d", cfw_event->nParam1);
        if (cfw_event->nParam1 == 0)
        {
            AT_SS_ResetUSSDState();
            if (0 != gATSATSendUSSDFlag[nSim])
            {
                OSI_LOGI(0, "CUSD:ATSendUSSD:gATSATSendUSSDFlag[nSim]%d\n", gATSATSendUSSDFlag[nSim]);
                CFW_SatResponse(0x12, 0x00, 0x00, 0x00, 0x00, cfw_event->nUTI, nSim);
                gATSATSendUSSDFlag[nSim] = false;
            }
            RETURN_OK(cmd->engine);
        }

        pstSsUssdIndInfo = (CFW_SS_USSD_IND_INFO_V2 *)cfw_event->nParam1;
        atMemFreeLater(pstSsUssdIndInfo);
        if (pstSsUssdIndInfo == NULL)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        OSI_LOGI(0, "CUSD: CUSD_RspCallBack nOption=%d nStingSize=%d", pstSsUssdIndInfo->nOption, pstSsUssdIndInfo->nStingSize);
#if !QUEC_PATCH_CUSD
        if (pstSsUssdIndInfo->nOption != 0 && pstSsUssdIndInfo->nOption != 1)
            AT_SS_SetUSSDNum(cfw_event->nUTI);
#endif
        if (gAtCfwCtx.sim[nSim].ussd && cfw_event->nParam1)
        {
            bool isStingSize = false;
            if (pstSsUssdIndInfo->nStingSize)
            {
                isStingSize = true;
                memset(nUSSDstr, 0, 256);
                if (pstSsUssdIndInfo->nStingSize != 0)
                {
                    nUSSDstr[0] = pstSsUssdIndInfo->nDcs;
                    memcpy(&nUSSDstr[1], pstSsUssdIndInfo->pUsdString, pstSsUssdIndInfo->nStingSize);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }

                //nUSSDDCS = AT_CBS_CheckDCS(pstSsUssdIndInfo->nDcs);
                AT_USSDParseDCS(pstSsUssdIndInfo->nDcs, &nUSSDDCS);
                nUSSDstr[0] = nUSSDDCS;
                OSI_LOGI(0, "CUSD:pstSsUssdIndInfo->nDcs=%d, nUSSDDCS: %d\n", pstSsUssdIndInfo->nDcs, nUSSDDCS);
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "CUSD:pUsdString=%s nStingSize=%d", pstSsUssdIndInfo->pUsdString, pstSsUssdIndInfo->nStingSize);
                _GenerateCUSData(pstSsUssdIndInfo->pUsdString, pstSsUssdIndInfo->nStingSize, aucDestUsd, nUSSDDCS);
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "CUSD:EV_CFW_SS_SEND_USSD_RSP aucDestUsd: %s, strlen(aucDestUsd)\n", aucDestUsd);

                totalLen = strlen("+CUSD: %u, \"%s\" ,%u") + 2 * MAX_LENGTH_STRING_USSD + 2;
            }
            else
            {
                totalLen = strlen("+CUSD: %u") + 1;
            }

            aucBuffer = (char *)malloc(totalLen);
            if (aucBuffer == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            memset(aucBuffer, 0x00, totalLen);
            if (isStingSize)
                sprintf(aucBuffer, "+CUSD: %u, \"%s\" ,%u", pstSsUssdIndInfo->nOption, aucDestUsd,
                        pstSsUssdIndInfo->nDcs);
            else
                sprintf(aucBuffer, "+CUSD: %u", pstSsUssdIndInfo->nOption);

            atCmdRespInfoText(cmd->engine, aucBuffer);
#if QUEC_PATCH_CUSD
			if (aucBuffer != NULL)
                free(aucBuffer);

            //the session need continue, so save the uti
            if (pstSsUssdIndInfo->nOption == 1)
                AT_SS_SetUSSDNum(cfw_event->nUTI);
            OSI_LOGI(0, "CUSD: AT_SS_GetUSSDState=%d", AT_SS_GetUSSDState());
            if (AT_SS_GetUSSDState() == 1)
            {
                ctx->ussdStr = (uint8_t *)malloc(MAX_LENGTH_STRING_USSD);
                if (ctx->ussdStr == NULL)
                {
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
                }
                atCmdRespOutputPrompt(cmd->engine);
                atCmdSetPromptMode(cmd->engine, CUSD_InputUssdStrCB, cmd, ctx->ussdStr, MAX_LENGTH_STRING_USSD);
            }
            else
                RETURN_OK(cmd->engine);
#else
            //memset(aucBuffer, 0x00, totalLen);
            free(aucBuffer);
            aucBuffer = NULL;
            RETURN_OK(cmd->engine);
#endif
        }
        else
        {
            OSI_LOGI(0, "CUSD:cfw_event->nParam1=%d ussd_param1=%d", cfw_event->nParam1, gAtCfwCtx.sim[nSim].ussd);
            RETURN_CME_CFW_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    else if (0xfa == cfw_event->nType || (0xfc == cfw_event->nType)) //  Local Error /net work reject
    {
        OSI_LOGXI(OSI_LOGPAR_SI, 0, "SS func: %s nType = %x", __FUNCTION__, cfw_event->nType);
        OSI_LOGI(0, "CUSD:CUSD_RspCallBack cfw_event->nParam2=%d", cfw_event->nParam2);
        if (cfw_event->nEventId == EV_CFW_SS_SEND_USSD_RSP)
        {
#if 0
            if ((cfw_event->nParam2 == 0x24) || (cfw_event->nParam2 == 0x22))
            {
                RETURN_OK(cmd->engine);
            }
#endif
            uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
            RETURN_CME_CFW_ERR(cmd->engine, uiErrCode);
        }

        if (0xFC == cfw_event->nType)
        {
            uiErrCode = SS_AT_ReSendProcUssV1Req(cfw_event->nUTI, nSim);
            RETURN_CME_CFW_ERR(cmd->engine, uiErrCode);
        }
        else
        {
            AT_SS_ResetUSSDState();
            uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
            RETURN_CME_CFW_ERR(cmd->engine, uiErrCode);
        }
    }
    else if (0xfb == cfw_event->nType || (0xfe == cfw_event->nType))
    {
        OSI_LOGI(0, "CUSD_RspCallBack: NetWork return error");
        OSI_LOGI(0, "CUSD:CUSD_RspCallBack cfw_event->nParam2=%d", cfw_event->nParam2);
        uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
        RETURN_CME_CFW_ERR(cmd->engine, uiErrCode);
    }
    else
    {
        uiErrCode = AT_SS_QUERYERR(cfw_event->nType, cfw_event->nParam2 & 0xffff);
        RETURN_CME_CFW_ERR(cmd->engine, uiErrCode);
    }
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: +CUSD=[<n>[,<str>[,<dcs>]]]
// Unstructured supplementary service data
// AT+CUSD=1,"*#100#",0x0f
// Parameter:   pParam, parsed command structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      during same session,must have same UTI
// only judge session start/end from respond event
// ////////////////////////////////////////////////////////////////////////////
void atCmdHandleCUSD(atCommand_t *cmd)
{
    uint32_t uiRetVal = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    OSI_LOGI(0, "CUSD: Come In atCmdHandleCUSD nSim[%d]", nSim);
    if (AT_CMD_SET == cmd->type)
    {
        uint8_t ucOption = 3; // MS originate USSD service
        uint8_t u7bitStr[MAX_LENGTH_STRING_USSD] = {
            0,
        },
                u7bitLen = 0;
        OSI_LOGI(0, "CUSD: param_count[%d]", cmd->param_count);

        bool paramok = true;
        uint8_t ucPara1 = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
        const char *aucPara2 = atParamOptStr(cmd->params[1], &paramok);
        uint8_t ucDcs = atParamDefUintInRange(cmd->params[2], 0, 0, 128, &paramok);
        size_t aucPara2len = strlen(aucPara2);
        if (!paramok || aucPara2len > MAX_LENGTH_STRING_USSD)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        OSI_LOGXI(OSI_LOGPAR_ISI, 0, "CUSD:Parameter1: %d  string: %s ucDcs: %d\n", ucPara1, aucPara2, ucDcs);
        //
        // [[hameina[mod] 2008-4-14 for bug 8005: if send USSD string failed, the first param can also be changed.
        if (cmd->param_count == 1 && ucPara1 != 2) // hameina[+]07.10.16
        {
            gAtCfwCtx.sim[nSim].ussd = ucPara1;
            RETURN_OK(cmd->engine);
        }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		if (cmd->param_count == 1 && ucPara1 == 2 && AT_SS_GetUSSDNum() == 0)
		{
			RETURN_OK(cmd->engine);
		}
#endif
        /*
           option Value Description
           0 MS 返回 Error(nUsdStringSize标识 Errsource , pUsdString[0]标识具体的 ErrCode );USSD version 2 operation.
           1 MS返回拒绝 (nUsdStringSize标识 ProblemCodeTag ,pUsdString[0]标识具体的 ProblemCode );USSD version 2 operation.
           2 terminate the session（release complete）; USSD version 2 operation.
           3 MS发起 ussd 或返回网络发起的 ussd 操作;USSD version 2 operation.
           128 MS 返回 Error(nUsdStringSize标识 Errsource, pUsdString[0]标识具体的 ErrCode ); USSD version 1 operation.（The string must be encoded as an IA5 string, nDcs is ignored）
           129 MS返回拒绝(nUsdStringSize标识 ProblemCodeTag, pUsdString[0]标识具体的 ProblemCode); USSD version 1 operation.（The string must be encoded as an IA5 string, nDcs is ignored）
           130 terminate the session（release complete）; USSD version 1 operation.（The string must be encoded as an IA5 string, nDcs is ignored）
           131 MS发起ussd或返回网络发起的ussd操作; USSD version 1 operation.（The string must be encoded as an IA5 string, nDcs is ignored）
           */
#if QUEC_PATCH_CUSD
		cusdContext_t *ctx = (cusdContext_t *)malloc(sizeof(*ctx));
        if (ctx == NULL)
        {
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
        }
        memset(ctx, 0, sizeof(*ctx));
        cmd->async_ctx = ctx;
        cmd->async_ctx_destroy = CUSD_AsyncCtxDestroy;
        ctx->nDcs = ucDcs;
#endif
        if (ucPara1 == 2)
        {
            if (g_ss_ussdVer == 2)
                ucOption = 2;
            else
                ucOption = 130;
        }
        else
        {
            if (ucDcs != 0) // has dcs 7bit dcs 0x10
            {
                int32_t rst;
                g_ss_ussdVer = 2;
                ucOption = 3;
                rst = SUL_Encode7Bit(aucPara2, u7bitStr, aucPara2len);
                OSI_LOGI(0, "CUSD:rst= %u \n", rst);
                u7bitLen = (aucPara2len % 8) ? (aucPara2len * 7 / 8 + 1) : (aucPara2len * 7 / 8);
                OSI_LOGI(0, "CUSD:u7bitLen = %u \n", u7bitLen);
                u7bitStr[u7bitLen] = '\0';
            }
            else
            {
                g_ss_ussdVer = 1;
                ucOption = 131;
            }
        }
#if QUEC_PATCH_CUSD
		OSI_LOGXI(OSI_LOGPAR_SI, 0, "CUSD:u7bitStr=%s u7bitLen=%d", u7bitStr, u7bitLen);
#else
        OSI_LOGXI(OSI_LOGPAR_S, 0, "CUSD:u7bitStr=%s", u7bitStr);
        OSI_LOGXI(OSI_LOGPAR_SI, 0, "CUSD:aucPara2=%s aucPara2len=%d", aucPara2, aucPara2len);

        OSI_LOGI(0, "CUSD: u7bitLen=%d ucDcs= %d, u7bitStr[0]=0x%d, u7bitStr[1]=0x%d\n",
                 u7bitLen, ucDcs, u7bitStr[0], u7bitStr[1]);
        OSI_LOGI(0, "CUSD2: u7bitLen=%d ucDcs= %d, u7bitStr[0]=0%d, u7bitStr[1]=0%d\n",
                 u7bitLen, ucDcs, u7bitStr[0], u7bitStr[1]);
#endif
        memcpy(at_ussdstring[nSim].ussd_input, u7bitStr, MAX_LENGTH_STRING_USSD);
        at_ussdstring[nSim].b_used = false;
#if QUEC_PATCH_CUSD
        cmd->uti = cfwRequestUTIEx((osiEventCallback_t)CUSD_RspCallBack, cmd, NULL, false);
        OSI_LOGI(0, "CUSD: uti=%d  ucDcs=%d", cmd->uti, ucDcs);
        if (ucDcs == 15)
        {
			uint8_t u8bitStr[MAX_LENGTH_STRING_USSD] = {0};
			memcpy(u8bitStr, aucPara2, aucPara2len);
			uiRetVal = CFW_SsSendUSSD(u8bitStr, aucPara2len, ucOption, ucDcs, cmd->uti, nSim);
        }
        else
        {
			uiRetVal = CFW_SsSendUSSD(u7bitStr, u7bitLen, ucOption, ucDcs, cmd->uti, nSim);
        }
        if (uiRetVal != 0)
        {
            OSI_LOGI(0, "CUSD2: CFW_SsSendUSSD: uiRetVal=0x%x\n", uiRetVal);
            cfwReleaseUTI(cmd->uti);
            AT_SS_ResetUSSDState();
            RETURN_CME_CFW_ERR(cmd->engine, uiRetVal);
        }
        gAtCfwCtx.sim[nSim].ussd = ucPara1;
        RETURN_FOR_ASYNC();
#else
        if (AT_SS_GetUSSDState())
        {
            OSI_LOGI(0, "CUSD: ucOption=%d, uiRetVal=0x%x\n", ucOption, uiRetVal);
            cmd->uti = cfwRequestUTI((osiEventCallback_t)CUSD_RspCallBack, cmd);
            OSI_LOGI(0, "CUSD: cmd->uti=%d AT_SS_GetUSSDNum=%d", cmd->uti, AT_SS_GetUSSDNum());
            if ((uiRetVal = CFW_SsSendUSSD_V2(u7bitStr, u7bitLen, ucOption, ucDcs, cmd->uti, AT_SS_GetUSSDNum(), nSim)) != 0)
            {
                OSI_LOGI(0, "CUSD: CFW_SsSendUSSD: uiRetVal=0x%x\n", uiRetVal);
                cfwReleaseUTI(cmd->uti);
                AT_SS_ResetUSSDState();
                RETURN_CME_CFW_ERR(cmd->engine, uiRetVal);
            }
            gAtCfwCtx.sim[nSim].ussd = ucPara1;
            RETURN_FOR_ASYNC();
        }
        else
        {
            OSI_LOGXI(OSI_LOGPAR_S, 0, "CUSD: req uti u7bitStr=%s", u7bitStr);
            OSI_LOGI(0, "CUSD 2: ucOption=%d, ucDcs=%d\n", ucOption, ucDcs);

            cmd->uti = cfwRequestUTI((osiEventCallback_t)CUSD_RspCallBack, cmd);
            if (ucDcs == 0)
            {
                ucDcs = 15;
                uint8_t u8bitStr[MAX_LENGTH_STRING_USSD] = {0};
                memcpy(u8bitStr, aucPara2, aucPara2len);
                uiRetVal = CFW_SsSendUSSD(u8bitStr, aucPara2len, ucOption, ucDcs, cmd->uti, nSim);
            }
            else
                uiRetVal = CFW_SsSendUSSD(u7bitStr, u7bitLen, ucOption, ucDcs, cmd->uti, nSim);
            if (uiRetVal != 0)
            {
                OSI_LOGI(0, "CUSD2: CFW_SsSendUSSD: uiRetVal=0x%x\n", uiRetVal);
                cfwReleaseUTI(cmd->uti);
                AT_SS_ResetUSSDState();
                RETURN_CME_CFW_ERR(cmd->engine, uiRetVal);
            }
            gAtCfwCtx.sim[nSim].ussd = ucPara1;
            RETURN_FOR_ASYNC();
        }
#endif
    }
    else if (AT_CMD_TEST == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		atCmdRespInfoText(cmd->engine, "+CUSD: (0-2)");
#else
        atCmdRespInfoText(cmd->engine, "+CUSD:(0,1,2)");
#endif
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char uOutString[20];

        memset(uOutString, 0, 20);
        sprintf(uOutString, "+CUSD:%d", gAtCfwCtx.sim[nSim].ussd);
        atCmdRespInfoText(cmd->engine, uOutString);
        RETURN_OK(cmd->engine);
    }

    else
        RETURN_CME_CFW_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
}

#ifdef PORTING_ON_GOING
// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description:+CAOC[=<mode>]
// Advice of Charge
// AT+CAOC=2
// Parameter:  pParam, parsed command structure
// Caller:
// Called:     ATM
// Return:     fail or succeed
// Remark:     no CSW interface
// not support the AT command
// ////////////////////////////////////////////////////////////////////////////

void atCmdHandleCAOC(atCommand_t *cmd)
{
    uint16_t ucLen;
    uint8_t ucParaCount = 0;
    uint8_t aucBuffer[MAX_LENGTH_BUFF] = {0};
    uint8_t *pucParam = (uint8_t *)(pParam->pPara);
    int32_t iRetVal = 0;
    uint32_t uiRetVal;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (NULL == pParam)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;
    iRetVal = AT_Util_GetParaCount(pucParam, &ucParaCount);
    if (ERR_SUCCESS != iRetVal)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        uint8_t ucFileType = AT_SIM_FILE_CAOC;
        uint32_t uiRetAoc;
        uint8_t ucPara1 = 0;
        uint8_t ucPara1Flg = EXIST;
        if (PARA_CNT_CAOC != ucParaCount)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        ucLen = sizeof(uint8_t);
        iRetVal = AT_Util_GetParaWithRule(pucParam, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucPara1, &ucLen);
        if (ERR_SUCCESS != iRetVal && ERR_AT_UTIL_CMD_PARA_NULL != iRetVal)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (0 == ucLen)
        {
            ucPara1Flg = NOT_EXIST;
        }
        if (NOT_EXIST != ucPara1Flg)
        {
            if (0 == ucPara1)
            {
                g_SsCaocStatus = ucPara1;
                uiRetVal = SS_ReadAocFromSim(ucFileType, &uiRetAoc);
                if (ERR_SUCCESS == uiRetVal)
                {
                    AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), pParam->nDLCI, nSim);
                }
                else
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }
            else
            {
                g_SsCaocStatus = ucPara1;
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }
    break;

    case AT_CMD_READ:
        sprintf(aucBuffer, strRCAOC, g_SsCaocStatus);
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), pParam->nDLCI, nSim);
        return;
    default:
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description:+CPUC=<currency>,<ppu>[,<passwd>]
// Price per unit and currency table
// AT+CPUC="GBP","0.125","2222"
// Parameter:  pParam, parsed command structure
// Caller:
// Called:     ATM
// Return:     fail or succeed
// Remark:     PPU must be change into 3gpp 11.890 format
//
// ////////////////////////////////////////////////////////////////////////////

void atCmdHandleCPUC(atCommand_t *cmd)
{
    uint16_t ucLen = 0;
    uint8_t ucParaCount = 0;
    uint32_t uiRetVal = 0;
    uint8_t arrCurrency[MAX_LENGTH_CURRENCY_CPUC];
    uint8_t arrPricePerUnit[MAX_LENGTH_PPU_CPUC];
    uint8_t arrPassword[MAX_LENGTH_PIN2];
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (NULL == pParam)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    // check event respond exclusive
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;

    switch (cmd->type)
    {

    case AT_CMD_SET:
    {
        int32_t iRetVal = 0;

        OSI_LOGI(0x10004547, "cpuc set ---in\n");
        iRetVal = AT_Util_GetParaCount(pParam->pPara, &ucParaCount);

        if ((ERR_SUCCESS != iRetVal) || (ucParaCount < 2) || (ucParaCount > 3))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // parameter initial
        CFW_SIM_PUCT_INFO stPUCT;
        AT_MemSet(&stPUCT, 0, sizeof(CFW_SIM_PUCT_INFO));
        //      SIM_PUCT_INFO stPUCTTmp;
        // include string must be initial
        //      AT_MemSet(&stPUCTTmp, 0, sizeof(SIM_PUCT_INFO));
        // get parameter: <currency>
        AT_MemZero(arrCurrency, sizeof(arrCurrency));

        ucLen = MAX_LENGTH_CURRENCY_CPUC;

        iRetVal = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, arrCurrency, &ucLen);

        if ((ERR_SUCCESS != iRetVal) || (ucLen > 3))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // AT_StrUpr(arrCurrency);
        // if arrCurrency size <3, default char is filled with ' '.
        AT_MemSet(stPUCT.iCurrency, ' ', 3);
        AT_MemCpy(stPUCT.iCurrency, arrCurrency, AT_StrLen(arrCurrency));
        // get parameter: <ppu>
        AT_MemZero(arrPricePerUnit, sizeof(arrPricePerUnit));

        ucLen = MAX_LENGTH_PPU_CPUC;

        iRetVal = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, arrPricePerUnit, &ucLen);

        if (ERR_SUCCESS != iRetVal || (!ucLen)) // can't input a string which len=0
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // get parameter: <password>
        AT_MemZero(arrPassword, sizeof(arrPassword));

        if (ucParaCount > 2)
        {
            ucLen = MAX_LENGTH_PIN2;
            iRetVal = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, arrPassword, &ucLen);

            if (ERR_SUCCESS != iRetVal)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        // decimal be changed into integer format to store
        //if (!SS_PPUFloatStrToIntStructure(&stPUCTTmp, arrPricePerUnit, AT_StrLen(arrPricePerUnit)))
        if (!SS_String2PUCT(&stPUCT, arrPricePerUnit))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        // get changed parameter
        //stPUCT.iEPPU = stPUCTTmp.iEPPU;
        //      stPUCT.iEX = stPUCTTmp.iEX;
        //  stPUCT.iSign = stPUCTTmp.iSign;

        OSI_LOGI(0x10004548, "CFW_SimSetPUCT,iEPPU --%d, iEX--%d, iSign--%d", stPUCT.iEPPU, stPUCT.iEX, stPUCT.iSign);
        uiRetVal = CFW_SimSetPUCT(&stPUCT, arrPassword, AT_StrLen(arrPassword), pParam->nDLCI, nSim);

        if (ERR_SUCCESS == uiRetVal)
        {
            OSI_LOGI(0x10004549, "CFW_SimSetPUCT SUCCESS");
            AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            OSI_LOGI(0x1000454a, "CFW_SimSetPUCT error");
            uint32_t nErrorCode = AT_SetCmeErrorCode(uiRetVal, TRUE);
            AT_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }

    break;

    case AT_CMD_TEST:
        OSI_LOGI(0x1000454b, "cpuc test ---in\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
        break;

    case AT_CMD_READ:
    {
        OSI_LOGI(0x1000454c, "cpuc read-in\n");

        uiRetVal = CFW_SimGetPUCT(pParam->nDLCI, nSim);
        if (ERR_SUCCESS == uiRetVal)
        {
            OSI_LOGI(0x1000454d, "CFW_SimGetPUCT SUCCESS");
            AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            OSI_LOGI(0x1000454e, "CFW_SimGetPUCT error");
            uint32_t nErrorCode = AT_SetCmeErrorCode(uiRetVal, FALSE);

            AT_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        break;
    }

    default:
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
        break;
    }
}
#endif

uint8_t ATssPswdServiceCode[] = {'0', '3', 0};
uint8_t ATssClipCode[] = {'3', '0', 0};
uint8_t ATssClirCode[] = {'3', '1', 0};
uint8_t ATssColpCode[] = {'7', '6', 0};
uint8_t ATssColrCode[] = {'7', '7', 0};
uint8_t ATssCnapCode[] = {'3', '0', '0', 0};
uint8_t ATssCfuCode[] = {'2', '1', 0};
uint8_t ATssCfbCode[] = {'6', '7', 0};
uint8_t ATssCfnryCode[] = {'6', '1', 0};
uint8_t ATssCfnrcCode[] = {'6', '2', 0};
uint8_t ATssAllForwardingCode[] = {'0', '0', '2', 0};
uint8_t ATssAllCondForwardingCode[] = {'0', '0', '4', 0};
uint8_t ATssCwCode[] = {'4', '3', 0};
uint8_t ATssBaocCode[] = {'3', '3', 0};
uint8_t ATssBoicCode[] = {'3', '3', '1', 0};
uint8_t ATssBoicExhcCode[] = {'3', '3', '2', 0};
uint8_t ATssBaicCode[] = {'3', '5', 0};
uint8_t ATssBicroamCode[] = {'3', '5', '1', 0};
uint8_t ATssAllCallRestrictionCode[] = {'3', '3', '0', 0};
uint8_t ATssBarringOfOutgoingCode[] = {'3', '3', '3', 0};
uint8_t ATssBarringOfIncomingCode[] = {'3', '5', '3', 0};

const ATCodeToSsCode ssATCodeToSsCode[] =
    {
        {ATssClipCode, SS_CLIP},
        {ATssClirCode, SS_CLIR},
        {ATssColpCode, SS_COLP},
        {ATssColrCode, SS_COLR},
        {ATssCnapCode, SS_CNAP},
        {ATssCfuCode, SS_CFU},
        {ATssCfbCode, SS_CFB},
        {ATssCfnryCode, SS_CFNRY},
        {ATssCfnrcCode, SS_CFNRC},
        {ATssAllForwardingCode, SS_ALLFORWARDINGSS},
        {ATssAllCondForwardingCode, SS_ALLCONDFORWARDINGSS},
        {ATssCwCode, SS_CW},
        {ATssBaocCode, SS_BAOC},
        {ATssBoicCode, SS_BOIC},
        {ATssBoicExhcCode, SS_BOICEXHC},
        {ATssBaicCode, SS_BAIC},
        {ATssBicroamCode, SS_BICROAM},
        {ATssAllCallRestrictionCode, SS_ALLBARRINGSS},
        //{ATssBarringOfOutgoingCode, SS_BARRINGOFOUTGOINGCALLS},
        {ATssBarringOfIncomingCode, SS_BARRINGOFINCOMINGCALLS},
        {(uint8_t *)NULL, (ss_code_enum)0}};

static bool CallForwardingFlag = false;
bool AT_IsDigit(uint8_t CharValue)
{
    if ((CharValue >= '0') && (CharValue <= '9'))
        return true;
    else
        return false;
}
//uint8_t at_branchbuf[80] ;//man
static bool AT_IsCCString(const char *s)
{
    uint8_t len;
    uint16_t i;

    if (NULL == s)
    {
        OSI_LOGI(0x10004557, "AT_IsCCString:ERROR!string is NULL");
        return false;
        ;
    }

    len = strlen(s);
    OSI_LOGXI(OSI_LOGPAR_S, 0x10004558, "AT_IsCCString:s string is %s", s);

    if (0 == len)
        return false;
    len = strlen(s);

    // special case for #+number, it is a number, we just dial
    OSI_LOGI(0, "ss:AT_IsCCString s[0]=%c s[len - 1]=%c", s[0], s[len - 1]);
    if ((s[0] == '#') && (s[len - 1] != '#'))
        return true;

    if ((s[0] == '+') || AT_IsDigit(s[0]) || ((s[0] == '*') && (s[len - 1] != '#')))
    {
        for (i = 1; i < len - 1; i++)
        {
            OSI_LOGI(0, "ss:AT_IsCCString: s[%d]=%c", s[i]);
            if (!(AT_IsDigit(s[i]) || s[i] == 'P'                                              //CHAR_DTMF ||   /* allow DTMF separator */
                  || s[i] == 'p' || s[i] == '*' || s[i] == '#' || s[i] == 'w' || s[i] == 'W')) //man

                return false;
        }

        return true;
    }

    return false;
}
bool AT_GetCallInfo(CFW_CC_CURRENT_CALL_INFO *pCallInfo, uint8_t CallStatus, CFW_SIM_ID nSimID)
{
    CFW_CC_CURRENT_CALL_INFO CallInfo[AT_MAX_CC_NUM];
    uint8_t Count = 0;
    bool RtnValue = false;
    uint8_t i;

    //assert(pCallInfo != NULL);
    memset(CallInfo, 0, AT_MAX_CC_NUM * sizeof(CFW_CC_CURRENT_CALL_INFO));
    RtnValue = CFW_CcGetCurrentCall(CallInfo, &Count, nSimID);
    OSI_LOGI(0x10004559, "ss:AT_GetCallInfo:Count:%d,rtnValue:%d", Count, RtnValue);
    if (ERR_SUCCESS == RtnValue)
    {

        if ((Count > 0) && (Count <= AT_MAX_CC_NUM))
        {
            for (i = 0; i < Count; i++)
            {
                if (CallStatus == CallInfo[i].status)
                {
                    *pCallInfo = CallInfo[i];
                    return true;
                }
            }
        }
    }

    return false;
}

bool AT_IsSSString(const char *s, CFW_SIM_ID nSimID)
{

    uint8_t len;

    CFW_CC_CURRENT_CALL_INFO CallInfo;
    bool CallStarting;

    if (NULL == s)
    {
        OSI_LOGI(0x1000455a, "ss:AT_IsSSString:ERROR!string is NULL");
        return false;
        ;
    }
    if (AT_IsCCString(s))
    {
        OSI_LOGI(0, "ss:AT_IsSSString: AT_IsCCString ruturn true=%d", AT_IsCCString(s));
        return false;
    }
    CallStarting = AT_GetCallInfo(&CallInfo, CFW_CM_STATUS_ACTIVE, nSimID);
    len = strlen(s);

    if (0 == len)
        return false;

    if ((len > 4) && (s[len - 1] == '#') && (s[len - 2] != '#') && (s[len - 2] != '*'))
        return true;

    if (((s[0] == '*') || (s[0] == '#')) && (s[len - 1] == '#') && (s[len - 2] != '#') && (s[len - 2] != '*'))
        return true;

    if ((len < 3) && (len > 0))
    {
        /* There is at least one call active */
        if (!CallStarting && s[0] == '1')
            return false;

        return true;
    }

    return false;
}

/*==========================================================================
 * This function scans the user input string from the position specified
 * and parses upto four * deleimited parameters.
 *
 * On entry it is assumed that the last character is a '#'
 *=========================================================================*/
static void BreaktItUp(uint8_t *pStr, uint8_t len, uint8_t pos, SSParsedParameters *pParsed)
{
    uint8_t parmNum = 0;

    //assert(pStr != NULL);
    //assert(pParsed != NULL);
    //assert(pStr[len - 1] == CHR_HASH);

    /* Pos points to the first parameter */
    pParsed->parmStart[parmNum] = pos;

    /* scan string until end of string or upto MAX_PARMS parameter parsed */
    /* or upto the first # */
    while ((parmNum < MAX_PARMS) && (pos <= len))
    {
        if (pStr[pos] == CHR_HASH)
        {
            /* abort scanning after updating the length */
            pParsed->parmLen[parmNum] = pos - pParsed->parmStart[parmNum];
            break;
        }
        else
        {
            /* Not terminator so check if delimitor */
            if (pStr[pos] == CHR_STAR)
            {
                /* It is a delimiter so update the length of the current parm*/
                pParsed->parmLen[parmNum] = pos - pParsed->parmStart[parmNum];
                /* advance pointer and increment the parm number*/
                parmNum++;
                pos++;
                /* and update start position of this parameter */
                if (parmNum < MAX_PARMS)
                {
                    pParsed->parmStart[parmNum] = pos;
                }
            }
            else
            {
                /* not a delimitor so increment pos */
                pos++;
            }
        }
    }
}

/*==========================================================================
 * Given a length terminated string, this routine parses it so that upto 4
 * parameters seperated by delimitors is extracted. The start of the string
 * can be one of the following:-
 *
 *                *                            SSMMI_STAR
 *                #                            SSMMI_HASH
 *                *#                           SSMMI_STARHASH
 *                **                           SSMMI_STARSTAR
 *                ##                           SSMMI_HASHHASH
 *                none of the above.           SSMMI_USS
 *
 * the rest of the string will consist of parameters delimited by a '*' and
 * will be terminated by a '#'.
 *
 * Assumptions on entry:-
 *
 *  1) The input string is not empty
 *  2) The last chr is # if len > 2
 *
 *=========================================================================*/
static uint8_t ParseUserInput(uint8_t *pStr, uint8_t len, SSParsedParameters *pParsed)
{
    uint8_t i, pos = 0;
    uint8_t resultFlag = 1;
    OSI_LOGXI(OSI_LOGPAR_S, 0, "ss:-->Comer in ParseUserInput pStr=%s", pStr);
    /* Check that the last chr is # if the length is > 2 */
    /*Correct assert, allow a user_len <=2*/
    //assert((len <= 2) || ((len > 2) && (pStr[len - 1] == CHR_HASH)));

    /* make all parms empty */
    for (i = 0; i < MAX_PARMS; i++)
    {
        pParsed->parmStart[i] = 0;
        pParsed->parmLen[i] = 0;
    }
    OSI_LOGI(0, "ss:-->Comer in ParseUserInput->1 len=%d", len);
    /* Not empty so look at the beginning of the string .. treat special if len <=2 */
    if (len <= 2)
    {
        pParsed->type = SSMMI_USS;
        pParsed->parmStart[0] = pos;
        pParsed->parmLen[0] = len;
    }
    else
    {
        /* the string length is 3 or more */
        OSI_LOGI(0, "ss:ParseUserInput * or #-->pStr[pos]=%d", pStr[pos]);
        switch (pStr[pos])
        {
        case CHR_STAR:
            /* it could be * or *# or ** */
            pos++;
            if ((pStr[pos + 1] >= '0' && pStr[pos + 1] <= '9') || (pStr[pos] >= '0' && pStr[pos] <= '9'))
            {
                OSI_LOGI(0, "ss:ParseUserInput *-->pStr[pos]=%d", pStr[pos]);
                switch (pStr[pos])
                {
                case CHR_HASH:
                    pParsed->type = SSMMI_STARHASH;
                    pos++;
                    break;

                case CHR_STAR:
                    pParsed->type = SSMMI_STARSTAR;
                    pos++;
                    break;

                default:
                    pParsed->type = SSMMI_STAR;
                    break;
                }
                BreaktItUp(pStr, len, pos, pParsed);
            }
            else
            {
                OSI_LOGI(0, "ss: ParseUserInput * resultFlag=0");
                resultFlag = 0;
            }
            break;

        case CHR_HASH:
            /* it could be # or ## */
            pos++;
            if ((pStr[pos + 1] >= '0' && pStr[pos + 1] <= '9') || (pStr[pos] >= '0' && pStr[pos] <= '9'))
            {
                OSI_LOGI(0, "ss:ParseUserInput #-->pStr[pos]=%d", pStr[pos]);
                if (pStr[pos] == CHR_HASH)
                {
                    pos++;
                    pParsed->type = SSMMI_HASHHASH;
                }
                else
                {
                    pParsed->type = SSMMI_HASH;
                }
                BreaktItUp(pStr, len, pos, pParsed);
            }
            else
            {
                OSI_LOGI(0, "ss: ParseUserInput # resultFlag=0");
                resultFlag = 0;
            }
            break;

        default:
            /* It does not begin with a * or # so treat it as a uss message */
            pParsed->type = SSMMI_USS;
            pParsed->parmStart[0] = pos;
            pParsed->parmLen[0] = len;
            OSI_LOGI(0, "ss: ParseUserInput default!!");
            break;
        }
    }
    OSI_LOGI(0, "ss: ParseUserInput end resultFlag=%d", resultFlag);
    return resultFlag;
}

/*==========================================================================
 * This routine is used to check if the length terminated string specified
 * matches the null terminated string
 *=========================================================================*/
static bool CmpzStrToLStr(const uint8_t *pZStr, const uint8_t *pLStr, uint8_t strLength)
{
    bool retValue = true; /* Assume they match */

    //assert(pZStr != NULL);
    //assert(pLStr != NULL);
    OSI_LOGXI(OSI_LOGPAR_SI, 0, "ss:CmpzStrToLStr pZStr=%s strLength=%d", pZStr, strLength);
    /* Check if both of same length */
    if (strlen((const char *)pZStr) == strLength)
    {
        /* the lengths are the same so they could be the same */
        while (strLength)
        {
            OSI_LOGI(0, "ss:CmpzStrToLStr pZStr=%d pLStr=%d", *pZStr, *pLStr);
            if (*pZStr != *pLStr)
            {
                /* Not same so the strings don't match */
                retValue = false;
                break;
            }
            pZStr++;
            pLStr++;
            strLength--;
        }
    }
    else
    {
        /* Not of same length so set result to false */
        retValue = false;
    }
    OSI_LOGI(0, "ss:CmpzStrToLStr retValue=%d", retValue);
    /* return the result */
    return retValue;
}

/*==========================================================================
 * This routine converts a text string assumed to be a number into
 * a binary enum value. If non digit characters are encountered then
 * a false is returned otherwise a true.
 *=========================================================================*/
static bool ValidMmiBsCode(uint8_t *pBsCodeStr, uint8_t bsCodeLen, Adp_bs_code_enum *pMmiBsCode)
{
    uint32_t code = 0;
    bool retValue = true; /* assume it is ok */

    //assert(pBsCodeStr != NULL);
    //assert(pMmiBsCode != NULL);
    //assert(bsCodeLen != 0);

    while (bsCodeLen != 0)
    {
        /* check if valid digit */
        if (!isdigit(*pBsCodeStr))
        {
            /* A non digit has been encountered */
            retValue = false;
            break;
        }

        /* This is a digit */
        code = (code * 10) + (*pBsCodeStr - '0');

        if (code > 0xFFFF)
        {
            /* the bs code value is too big so flag it as illegal */
            retValue = false;
            break;
        }

        /* advance pointers and decrement count */
        pBsCodeStr++;
        bsCodeLen--;
    }
    OSI_LOGI(0, "ValidMmiBsCode *pBsCodeStr:%d, (Adp_bs_code_enum)code:%d code:%d", *pBsCodeStr, (Adp_bs_code_enum)code, code);
    /* copy the decoded code into the output variable */
    *pMmiBsCode = (Adp_bs_code_enum)code;

    if (!((*pMmiBsCode >= 10 && *pMmiBsCode <= 25 && *pMmiBsCode != 14 && *pMmiBsCode != 15) || (*pMmiBsCode == 29) || (*pMmiBsCode == 0)))
        retValue = false;

    /* return the conversion status */
    return retValue;
}

/*==========================================================================
 * This routine copies the "uint8_t" substring starting at
 * 'pSrc' and 'subLen' characters upto 'maxlen' long to the "uint8_t" string
 * 'pDst[0]'. It also copies the value of 'subLen' into
 * the destination string length variable provided.
 *=========================================================================*/
static void CopySubString(uint8_t *pSrc, uint8_t subLen, uint8_t *pDst, uint8_t *dstLen, uint8_t maxLen)
{
    //assert(pSrc != NULL);
    //assert(pDst != NULL);
    //assert(dstLen != NULL);

    /* update the destiantion string length */
    *dstLen = subLen = (subLen > maxLen) ? maxLen : subLen;

    /* copy the substring */
    while (subLen != 0)
    {
        *pDst = *pSrc;
        pDst++;
        pSrc++;
        subLen--;
    }
}

/*==========================================================================
 * This routine is used to determine which of the operations listed below
 * the user mmi string should result in:
 *                     REGISTER,          pSSCode supplied
 *                     ERASE,             pSSCode supplied
 *                     ACTIVATE,          pSSCode supplied
 *                     DEACTIVATE,        pSSCode supplied
 *                     INTERROGATE,       pSSCode supplied
 *                     REG_PASSWORD,      pSSCode supplied
 *                     USSD
 *
 * If it is of the first 5 types then the sscode is also updated
 *=========================================================================*/
static void GetTypeOfOperation(uint8_t *pStr, SSParsedParameters *pParsedParameters, l4_op_code_enum *pOpCode, ss_code_enum *pSSCode)
{
    const ATCodeToSsCode *pTable = &ssATCodeToSsCode[0];
    bool found;
    uint8_t *servCode_p = NULL;
    uint8_t servCodeLen;

    //assert(pStr != NULL);
    //assert(pParsedParameters != NULL);
    //assert(pOpCode != NULL);
    //assert(pSSCode != NULL);
    OSI_LOGI(0, "ss:GetTypeOfOperation: pParsedParameters->type=%d", pParsedParameters->type);
    /* check if the user string prefix is ussd type */
    if (pParsedParameters->type == SSMMI_USS)
    {
        /* It is */
        *pOpCode = SS_OP_PROCESSUNSTRUCTUREDSS_DATA;
    }
    else
    {
        /* It is NOT but it could be if the service code is meaningless */
        /* so first look through the recognised service code table */
        found = false;
        servCode_p = &pStr[pParsedParameters->parmStart[0]];
        servCodeLen = pParsedParameters->parmLen[0];
        OSI_LOGXI(OSI_LOGPAR_S, 0, "ss:GetTypeOfOperation servCode_p=%s", servCode_p);
        /* search through table */
        while (pTable->pzMmiStr != NULL)
        {
            if (CmpzStrToLStr(pTable->pzMmiStr, servCode_p, servCodeLen) == true)
            {
                /* A match has been found */
                found = true;
                break;
            }
            /* increment the table pointer */
            pTable++;
        }
        OSI_LOGI(0, "ss:GetTypeOfOperation: 1--found=%d", found);
        if (found == true)
        {
            /* A match found in the table so it is a recognised type */
            *pSSCode = pTable->SSCode;
            OSI_LOGI(0, "ss: GetTypeOfOperation pParsedParameters->type=%d", pParsedParameters->type);
            switch (pParsedParameters->type)
            {
            case SSMMI_STAR: /* activate or register */
                /* assume that it is activation */
                *pOpCode = SS_OP_ACTIVATESS;
                break;

            case SSMMI_HASH: /* deactivate */
                *pOpCode = SS_OP_DEACTIVATESS;
                break;

            case SSMMI_STARHASH: /* interrogate */
                *pOpCode = SS_OP_INTERROGATESS;
                break;

            case SSMMI_STARSTAR: /* register */
                *pOpCode = SS_OP_REGISTERSS;
                break;

            case SSMMI_HASHHASH: /* erase */
                *pOpCode = SS_OP_ERASESS;
                break;

            default:
                //The program can not go here.
                OSI_LOGI(0x090004e2, "ss: MMI Parsing should never reach here");
                //assert(0);
                break;
            }
        }
        else
        {
            OSI_LOGI(0, "ss:GetTypeOfOperation: 2 found =%d", found);
        }
    }
}

/*Invoke CSW function to set or guery call waiting.*/
static void ADP_SS_SetCallWaiting(l4_op_code_enum OpCode, bool BsCodePresent, Adp_bs_code_enum MmiBsCode, uint8_t nDLCI, CFW_SIM_ID nSimID)
{
    uint32_t ret;

    if (BsCodePresent == false)
    {
        BsCodePresent = true;
        MmiBsCode = Adp_TELEPHONY;
    }
    OSI_LOGI(0, "ss:ADP_SS_SetCallWaiting MmiBsCode BsCodePresent=%d MmiBsCode=%d", BsCodePresent, MmiBsCode);
    switch (OpCode)
    {
    case SS_OP_REGISTERSS:
    case SS_OP_ACTIVATESS:

        ret = CFW_SsSetCallWaiting(CW_ENABLE, MmiBsCode, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004e5, "ADP_SS_SetCallWaiting()   CFW_SsSetCallWaiting  ret = %d", ret);
        }
        break;

    case SS_OP_ERASESS:
    case SS_OP_DEACTIVATESS:
        ret = CFW_SsSetCallWaiting(CW_DISABLE, MmiBsCode, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004e5, "ADP_SS_SetCallWaiting()   CFW_SsSetCallWaiting  ret = %d", ret);
        }
        break;

    case SS_OP_INTERROGATESS:
        ret = CFW_SsQueryCallWaiting(MmiBsCode, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004e7, "ADP_SS_SetCallWaiting()   CFW_SsQueryCallWaiting  ret = %d", ret);
        }
        break;

    default:
        //The program can not go here.
        OSI_LOGI(0x090004e8, "ADP_SS_SetCallWaiting Error OpCode,%d", OpCode);
        break;
    }
}

/*Invoke CSW function to set or guery call forwarding.*/
static void ADP_SS_SetCallForward(l4_op_code_enum OpCode, ss_code_enum SSCode, bool BsCodePresent, Adp_bs_code_enum MmiBsCode, bool TimePresent, uint8_t Time, uint8_t numberBCDBytesLen, uint8_t *pNumberBCD, uint8_t nDLCI, CFW_SIM_ID nSimID)
{
    CFW_SS_SET_CALLFORWARDING_INFO CallForward;
    uint32_t ret;
    memset(&CallForward, 0, sizeof(CFW_SS_SET_CALLFORWARDING_INFO));

    if (BsCodePresent == false)
    {
        BsCodePresent = true;
        MmiBsCode = Adp_TELEPHONY;
    }

    CallForward.nClass = MmiBsCode;

    OSI_LOGI(0x090004e9, "ADP_SS_SetCallForward MmiBsCode = %d", MmiBsCode);
    OSI_LOGI(0x090004ea, "ADP_SS_SetCallForward SSCode = 0x%x", SSCode);
    OSI_LOGI(0x090004eb, "ADP_SS_SetCallForward OpCode = %d", OpCode);
    switch (SSCode)
    {
    case SS_CFU:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_UNCONDITIONAL;
        if (OpCode != SS_OP_REGISTERSS && OpCode != SS_OP_ERASESS)
            CallForward.nClass = 0;
        break;

    case SS_CFB:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_MOBILEBUSY;
        if (OpCode != SS_OP_INTERROGATESS)
            CallForward.nClass = 0;
        break;

    case SS_CFNRY:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_NOREPLY;
        break;

    case SS_CFNRC:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_NOTREACHABLE;
        if (OpCode != SS_OP_DEACTIVATESS)
            CallForward.nClass = 0; //31.2.1.2.1
        break;

    case SS_ALLFORWARDINGSS:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_ALL_CALLFORWARDING;
        break;

    case SS_ALLCONDFORWARDINGSS:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_ALL_CONDCALLFORWARDING;
        break;

    default:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_UNCONDITIONAL;
        break;
    }

    switch (OpCode)
    {
    case SS_OP_REGISTERSS:
    case SS_OP_ACTIVATESS:
        if (pNumberBCD != NULL)
        {
            //CallForward.nNumber.pDialNumber = (uint8_t *)malloc(numberBCDBytesLen);
            //assert(CallForward.nNumber.pDialNumber != NULL);
            memcpy(CallForward.nNumber.pDialNumber, pNumberBCD, numberBCDBytesLen);
        }
        CallForward.nNumber.nDialNumberSize = numberBCDBytesLen;
        CallForward.nNumber.nClir = 0;
        if (CallForwardingFlag == false)
        {
            CallForward.nNumber.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
        }
        else
        {
            CallForward.nNumber.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
            CallForwardingFlag = false;
        }

        if (TimePresent == true)
        {
            CallForward.nTime = Time;
        }
        CallForward.nMode = CFW_SS_FORWARDING_MODE_REGISTRATION;
        ret = CFW_SsSetCallForwarding(&CallForward, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004ec, "ADP_SS_SetCallForward()   CFW_SsSetCallForwarding  ret = %d", ret);
        }
#if 0        
        if (CallForward.nNumber.pDialNumber != NULL)
        {
            free(CallForward.nNumber.pDialNumber);
            CallForward.nNumber.pDialNumber = NULL;
        }
#endif
        break;

    case SS_OP_ERASESS:
        CallForward.nMode = CFW_SS_FORWARDING_MODE_ERASURE;
        ret = CFW_SsSetCallForwarding(&CallForward, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004ec, "ADP_SS_SetCallForward()   CFW_SsSetCallForwarding  ret = %d", ret);
        }
        break;

    case SS_OP_DEACTIVATESS:
        CallForward.nMode = CFW_SS_FORWARDING_MODE_DISABLE;
        ret = CFW_SsSetCallForwarding(&CallForward, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004ec, "ADP_SS_SetCallForward()   CFW_SsSetCallForwarding  ret = %d", ret);
        }
        break;

    case SS_OP_INTERROGATESS:
        OSI_LOGI(0x090004ef, "CFW_SsQueryCallForwarding reason = %d", CallForward.nReason);
        OSI_LOGI(0x090004f0, "CFW_SsQueryCallForwarding mode = %d", CallForward.nMode);
        OSI_LOGI(0x090004f1, "CFW_SsQueryCallForwarding class = %d", CallForward.nClass);
        OSI_LOGI(0x090004f2, "CFW_SsQueryCallForwarding time= %d", CallForward.nTime);

        ret = CFW_SsQueryCallForwarding(CallForward.nReason, CallForward.nClass, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004f3, "ADP_SS_SetCallForward()   CFW_SsQueryCallForwarding  ret = %d", ret);
        }
        break;

    default:
        //The program can not go here.
        OSI_LOGI(0x090004f4, "ADP_SS_SetCallForward Error OpCode,%d", OpCode);
        break;
    }
}

/*Invoke CSW function to set or guery call barring.*/
static void ADP_SS_SetCallBarring(l4_op_code_enum OpCode, ss_code_enum SSCode, bool BsCodePresent, Adp_bs_code_enum MmiBsCode, uint8_t PasswordLength, uint8_t *pPassword, uint8_t nDLCI, CFW_SIM_ID nSimID)
{
    uint16_t nFac = 0;
    uint8_t nClass = 0;
    uint32_t ret = 0;
    //assert(pPassword != NULL);

    OSI_LOGI(0x090004f5, "ADP_SS_SetCallBarring SSCode = %d", SSCode);
    switch (SSCode)
    {
    case SS_BAOC:
        nFac = CFW_STY_FAC_TYPE_AO;
        break;

    case SS_BOIC:
        nFac = CFW_STY_FAC_TYPE_OI;
        break;

    case SS_BOICEXHC:
        nFac = CFW_STY_FAC_TYPE_OX;
        break;

    case SS_BAIC:
        nFac = CFW_STY_FAC_TYPE_AI;
        break;

    case SS_BICROAM:
        nFac = CFW_STY_FAC_TYPE_IR;
        break;

    case SS_ALLBARRINGSS:
        nFac = CFW_STY_FAC_TYPE_AB;
        break;

    case SS_BARRINGOFINCOMINGCALLS:
        nFac = CFW_STY_FAC_TYPE_AC;
        break;

    default:
        break;
    }

    if (BsCodePresent == false)
    {
        BsCodePresent = true;
        MmiBsCode = Adp_TELEPHONY;
    }
    nClass = MmiBsCode;

    switch (OpCode)
    {
    case SS_OP_REGISTERSS:
    case SS_OP_ACTIVATESS:
        ret = CFW_SsSetFacilityLock(nFac, pPassword, PasswordLength, nClass, CFW_CM_MODE_LOCK, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004f6, "ADP_SS_SetCallBarring()   CFW_SsSetFacilityLock  ret = %d", ret);
        }
        break;

    case SS_OP_ERASESS:
    case SS_OP_DEACTIVATESS:
        ret = CFW_SsSetFacilityLock(nFac, pPassword, PasswordLength, nClass, CFW_CM_MODE_UNLOCK, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004f6, "ADP_SS_SetCallBarring()   CFW_SsSetFacilityLock  ret = %d", ret);
        }
        break;

    case SS_OP_INTERROGATESS:
        ret = CFW_SsQueryFacilityLock(nFac, nClass, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004f8, "ADP_SS_SetCallBarring()   CFW_SsQueryFacilityLock  ret = %d", ret);
        }
        break;
    default:
        //The program can not go here.
        OSI_LOGI(0x090004f9, "ADP_SS_SetCallBarring Error OpCode,%d", OpCode);
        break;
    }
}

/*==========================================================================
 * This function is used to process SS REGISTER REQ to SSCode related REQ.
 *=========================================================================*/
static void SS_RegisterREQ(uint8_t *pStr, SSParsedParameters *pParsed, ss_code_enum SSCode, uint8_t nDLCI, CFW_SIM_ID nSimID)
{
    bool MmiBsCodePresent = false;
    Adp_bs_code_enum MmiBsCode = Adp_ALL_BS_AND_TS;
    uint8_t PasswordLength = 0;
    uint8_t pwd[MAXPASSWORDLEN] = {0};
    bool TimePresent = 0;
    uint8_t Time = 0;
    uint8_t NumberBCD[AT_MAX_CC_ADDR_LEN] = {0};
    uint8_t BytesLenBCD = 0;

    //assert(pStr != NULL);
    //assert(pParsed != NULL);
    OSI_LOGI(0, "ss:SS_RegisterREQ SSCode=%d", SSCode);
    /*. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
    /* Copy the optional mmi basic service code */
    switch (SSCode)
    {
    case SS_CW:
        /* For call waiitng the BS is in second parameter */
        if (pParsed->parmLen[1] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
        }

        /* Change to CSW request */
        ADP_SS_SetCallWaiting(SS_OP_REGISTERSS, MmiBsCodePresent, MmiBsCode, nDLCI, nSimID);
        break;

    case SS_BAOC:
    case SS_BOIC:
    case SS_BOICEXHC:
    case SS_BAIC:
    case SS_BICROAM:
    case SS_ALLBARRINGSS:
    case SS_BARRINGOFOUTGOINGCALLS:
    case SS_BARRINGOFINCOMINGCALLS:
        /* The password is required for the call restriction services only */
        /* and will always be in the second paremeter */
        CopySubString(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &pwd[0], &PasswordLength, MAXPASSWORDLEN);

        /* For these BS is in the third parameter */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallBarring(SS_OP_REGISTERSS, SSCode, MmiBsCodePresent, MmiBsCode, PasswordLength, pwd, nDLCI, nSimID);
        break;

    case SS_CFNRY:
    case SS_ALLFORWARDINGSS:
    case SS_ALLCONDFORWARDINGSS:
        /* The reply timeout is always in the fourth parameter */
        if (pParsed->parmLen[3] != 0)
        {
            /* A timer value has been supplied so try and decode it */
            TimePresent = true; //ValidReplyTime(&pStr[pParsed->parmStart[3]], pParsed->parmLen[3], &Time);
        }
        else
        {
            TimePresent = false;
            Time = 0;
        }
        if (SS_CFNRY == SSCode && Time == 0)
        {
            TimePresent = true;
            Time = 5;
        }
    /*Don't break here, WARNING, WARNING, WARNING, WARNING*/
    /*Don't break here, WARNING, WARNING, WARNING, WARNING*/
    case SS_CFU:
    case SS_CFB:
    case SS_CFNRC:
        /* For these BS is in the third parameter */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        memset(NumberBCD, 0, AT_MAX_CC_ADDR_LEN);
        /* The fwd to number is in the second parameter  */
        if (pParsed->parmLen[1] != 0)
        {
            /* A forwarding number has been supplied so try and bcd encode it */
            if (strncmp((const char *)&pStr[pParsed->parmStart[1]], "+", 1) == 0)
            {
                SUL_AsciiToGsmBcd(&pStr[pParsed->parmStart[1] + 1], pParsed->parmLen[1] - 1, &NumberBCD[0]);
                CallForwardingFlag = true;
                BytesLenBCD = (pParsed->parmLen[1] - 1) / 2;
                BytesLenBCD += (pParsed->parmLen[1] - 1) % 2;
            }
            else
            {
                SUL_AsciiToGsmBcd(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &NumberBCD[0]);
                BytesLenBCD = pParsed->parmLen[1] / 2;
                BytesLenBCD += pParsed->parmLen[1] % 2;
            }
        }

        ADP_SS_SetCallForward(SS_OP_REGISTERSS, SSCode, MmiBsCodePresent, MmiBsCode, TimePresent, Time, BytesLenBCD, NumberBCD, nDLCI, nSimID);
        break;

    default:
        OSI_LOGI(0x090004fa, "SS_RegisterREQ have any undeveloped SSCode %d", SSCode);
        break;
    }
}

/*==========================================================================
 * This function is used to process SS ERASE REQ to SSCode related REQ.
 *=========================================================================*/
static void SS_EraseREQ(uint8_t *pStr, SSParsedParameters *pParsed, ss_code_enum SSCode, uint8_t nDLCI, CFW_SIM_ID nSimID)
{
    bool MmiBsCodePresent = false;
    Adp_bs_code_enum MmiBsCode = Adp_ALL_BS_AND_TS;
    uint8_t PasswordLength = 0;
    uint8_t pwd[MAXPASSWORDLEN] = {0};

    //assert(pStr != NULL);
    //assert(pParsed != NULL);
    OSI_LOGI(0, "ss:SS_EraseREQ SSCode=%d", SSCode);
    switch (SSCode)
    {
    case SS_CW:
        /* For call waiitng the BS is in parm 1 */
        if (pParsed->parmLen[1] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
        }

        /* Change to CSW request */
        ADP_SS_SetCallWaiting(SS_OP_ERASESS, MmiBsCodePresent, MmiBsCode, nDLCI, nSimID);
        break;

    case SS_BAOC:
    case SS_BOIC:
    case SS_BOICEXHC:
    case SS_BAIC:
    case SS_BICROAM:
    case SS_ALLBARRINGSS:
    case SS_BARRINGOFOUTGOINGCALLS:
    case SS_BARRINGOFINCOMINGCALLS:
        /* The password is required for the call restriction services only */
        CopySubString(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &pwd[0], &PasswordLength, MAXPASSWORDLEN);

        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallBarring(SS_OP_ERASESS, SSCode, MmiBsCodePresent, MmiBsCode, PasswordLength, pwd, nDLCI, nSimID);
        break;

    case SS_CFU:
    case SS_CFB:
    case SS_CFNRY:
    case SS_CFNRC:
    case SS_ALLFORWARDINGSS:
    case SS_ALLCONDFORWARDINGSS:
        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }
        else
        {
            /* Basic Service code can be included in parm1 in CF erase */
            if (pParsed->parmLen[1] != 0)
            {
                /* A basic service code has been supplied so try and decode it */
                MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
            }
        }
        ADP_SS_SetCallForward(SS_OP_ERASESS, SSCode, MmiBsCodePresent, MmiBsCode, false, 0, 0, NULL, nDLCI, nSimID);
        break;

    default:
        OSI_LOGI(0x090004fb, "SS_EraseREQ have any undeveloped SSCode %d", SSCode);
        break;
    }
}

/*==========================================================================
 * This function is used to process SS ACTIVATE REQ to SSCode related REQ.
 *=========================================================================*/
static void SS_ActivateREQ(uint8_t *pStr, SSParsedParameters *pParsed, ss_code_enum SSCode, uint8_t nDLCI, CFW_SIM_ID nSimID)
{
    bool MmiBsCodePresent = false;
    Adp_bs_code_enum MmiBsCode = Adp_ALL_BS_AND_TS;
    uint8_t PasswordLength = 0;
    uint8_t pwd[MAXPASSWORDLEN] = {0};
    bool TimePresent = 0;
    uint8_t Time = 0;
    uint8_t NumberBCD[AT_MAX_CC_ADDR_LEN] = {0};
    uint8_t BytesLenBCD = 0;

    //assert(pStr != NULL);
    //assert(pParsed != NULL);
    OSI_LOGI(0, "ss:SS_ActivateREQ SSCode=%d", SSCode);
    switch (SSCode)
    {
    case SS_CW:
        /* For call waiitng the BS is in parm 1 */
        if (pParsed->parmLen[1] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
        }

        /* Change to CSW request */
        ADP_SS_SetCallWaiting(SS_OP_ACTIVATESS, MmiBsCodePresent, MmiBsCode, nDLCI, nSimID);
        break;

    case SS_BAOC:
    case SS_BOIC:
    case SS_BOICEXHC:
    case SS_BAIC:
    case SS_BICROAM:
    case SS_ALLBARRINGSS:
    case SS_BARRINGOFOUTGOINGCALLS:
    case SS_BARRINGOFINCOMINGCALLS:
        /* The password is required for the call restriction services only */
        CopySubString(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &pwd[0], &PasswordLength, MAXPASSWORDLEN);

        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallBarring(SS_OP_ACTIVATESS, SSCode, MmiBsCodePresent, MmiBsCode, PasswordLength, pwd, nDLCI, nSimID);
        break;

    case SS_CFNRY:
    case SS_ALLFORWARDINGSS:
    case SS_ALLCONDFORWARDINGSS:
        /* The reply timeout is always in the fourth parameter */
        if (pParsed->parmLen[3] != 0)
        {
            /* A timer value has been supplied so try and decode it */
            TimePresent = true; //ValidReplyTime(&pStr[pParsed->parmStart[3]], pParsed->parmLen[3], &Time);
        }
        else
        {
            TimePresent = false;
            Time = 0;
        }
    /*Don't break here, WARNING, WARNING, WARNING, WARNING*/
    /*Don't break here, WARNING, WARNING, WARNING, WARNING*/
    case SS_CFU:
    case SS_CFB:
    case SS_CFNRC:
        /* For these BS is in the third parameter */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        memset(NumberBCD, 0, AT_MAX_CC_ADDR_LEN);
        /* The fwd to number is in the second parameter  */
        if (pParsed->parmLen[1] != 0)
        {
            /* A forwarding number has been supplied so try and bcd encode it */
            if (strncmp((const char *)&pStr[pParsed->parmStart[1]], "+", 1) == 0)
            {
                SUL_AsciiToGsmBcd(&pStr[pParsed->parmStart[1] + 1], pParsed->parmLen[1] - 1, &NumberBCD[0]);
                CallForwardingFlag = true;
            }
            else
            {
                SUL_AsciiToGsmBcd(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &NumberBCD[0]);
            }
            BytesLenBCD = pParsed->parmLen[1] / 2;
            BytesLenBCD += pParsed->parmLen[1] % 2;
        }
        ADP_SS_SetCallForward(SS_OP_ACTIVATESS, SSCode, MmiBsCodePresent, MmiBsCode, TimePresent, Time, BytesLenBCD, NumberBCD, nDLCI, nSimID);
        break;

    default:
        OSI_LOGI(0x090004fc, "SS_ActivateREQ have any undeveloped SSCode %d", SSCode);
        break;
    }
}

/*==========================================================================
 * This function is used to process SS DEACTIVATE REQ to SSCode related REQ.
 *=========================================================================*/
static void SS_DeactivateREQ(uint8_t *pStr, SSParsedParameters *pParsed, ss_code_enum SSCode, uint8_t nDLCI, CFW_SIM_ID nSimID)
{
    bool MmiBsCodePresent = true;
    Adp_bs_code_enum MmiBsCode = Adp_ALL_BS_AND_TS;
    uint8_t PasswordLength = 0;
    uint8_t pwd[MAXPASSWORDLEN] = {0};

    //assert(pStr != NULL);
    //assert(pParsed != NULL);
    OSI_LOGI(0, "ss:SS_DeactivateREQ SSCode=%d", SSCode);
    switch (SSCode)
    {
    case SS_CW:
        /* For call waiitng the BS is in parm 1 */
        if (pParsed->parmLen[1] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
        }
        OSI_LOGI(0, "ss:SS_DeactivateREQ MmiBsCode=%d MmiBsCodePresent=%d", MmiBsCode, MmiBsCodePresent);
        /* Change to CSW request */
        ADP_SS_SetCallWaiting(SS_OP_DEACTIVATESS, MmiBsCodePresent, MmiBsCode, nDLCI, nSimID);
        break;

    case SS_BAOC:
    case SS_BOIC:
    case SS_BOICEXHC:
    case SS_BAIC:
    case SS_BICROAM:
    case SS_ALLBARRINGSS:
    case SS_BARRINGOFOUTGOINGCALLS:
    case SS_BARRINGOFINCOMINGCALLS:
        /* The password is required for the call restriction services only */
        CopySubString(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &pwd[0], &PasswordLength, MAXPASSWORDLEN);

        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallBarring(SS_OP_DEACTIVATESS, SSCode, MmiBsCodePresent, MmiBsCode, PasswordLength, pwd, nDLCI, nSimID);
        break;

    case SS_CFU:
    case SS_CFB:
    case SS_CFNRY:
    case SS_CFNRC:
    case SS_ALLFORWARDINGSS:
    case SS_ALLCONDFORWARDINGSS:
        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallForward(SS_OP_DEACTIVATESS, SSCode, MmiBsCodePresent, MmiBsCode, false, 0, 0, NULL, nDLCI, nSimID);
        break;

    default:
        OSI_LOGI(0x090004fd, "SS_DeactivateREQ have any undeveloped SSCode %d", SSCode);
        break;
    }
}

/*==========================================================================
 * This function is used to process SS INTERROGATE REQ to SSCode related REQ.
 *=========================================================================*/
static void SS_InterrogateREQ(uint8_t *pStr, SSParsedParameters *pParsed, ss_code_enum SSCode, uint8_t nDLCI, CFW_SIM_ID nSimID)
{
    uint32_t ret;
    bool MmiBsCodePresent = false;
    Adp_bs_code_enum MmiBsCode = Adp_ALL_BS_AND_TS;
    uint8_t PasswordLength = 0;
    uint8_t pwd[MAXPASSWORDLEN] = {0};

    //assert(pStr != NULL);
    //assert(pParsed != NULL);
    OSI_LOGI(0, "ss:SS_InterrogateREQ SSCode=%d", SSCode);
    switch (SSCode)
    {
    case SS_CW:
        /* For call waiitng the BS is in parm 1 */
        if (pParsed->parmLen[1] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
        }

        /* Change to CSW request */
        ADP_SS_SetCallWaiting(SS_OP_INTERROGATESS, MmiBsCodePresent, MmiBsCode, nDLCI, nSimID);
        break;

    case SS_BAOC:
    case SS_BOIC:
    case SS_BOICEXHC:
    case SS_BAIC:
    case SS_BICROAM:
    case SS_ALLBARRINGSS:
    case SS_BARRINGOFOUTGOINGCALLS:
    case SS_BARRINGOFINCOMINGCALLS:
        /* The password is required for the call restriction services only */
        CopySubString(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &pwd[0], &PasswordLength, MAXPASSWORDLEN);

        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallBarring(SS_OP_INTERROGATESS, SSCode, MmiBsCodePresent, MmiBsCode, PasswordLength, pwd, nDLCI, nSimID);
        break;

    case SS_CFU:
    case SS_CFB:
    case SS_CFNRY:
    case SS_CFNRC:
    case SS_ALLFORWARDINGSS:
    case SS_ALLCONDFORWARDINGSS:
        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }
        ADP_SS_SetCallForward(SS_OP_INTERROGATESS, SSCode, MmiBsCodePresent, MmiBsCode, false, 0, 0, NULL, nDLCI, nSimID);
        break;

    case SS_CLIP:

        ret = CFW_SsQueryClip(nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004fe, "SS_InterrogateREQ()   CFW_SsQueryClip  ret = %d", ret);
        }
        break;

    case SS_CLIR:
        ret = CFW_SsQueryClir(nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x090004ff, "SS_InterrogateREQ()   CFW_SsQueryClir  ret = %d", ret);
        }
        break;

    case SS_COLP:
        ret = CFW_SsQueryColp(nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x09000500, "SS_InterrogateREQ()   CFW_SsQueryColp  ret = %d", ret);
        }
        break;

    case SS_COLR:
        ret = CFW_SsQueryColr(nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x09000501, "SS_InterrogateREQ()   CFW_SsQueryColr  ret = %d", ret);
        }
        break;

    default:
        OSI_LOGI(0x09000502, "SS_InterrogateREQ have any undeveloped SSCode %d", SSCode);
        break;
    }
}

/**************************************************************
  FUNCTION NAME     : SS_Adaption_ParseStringREQ
PURPOSE             : Parse SS string.
INPUT PARAMETERS    : mmi_ss_parsing_string_req_struct *pReq
 **************************************************************/
bool AT_SS_Adaption_ParseStringREQ(at_ss_parsing_string_req_struct *pReq, uint8_t nDLCI, CFW_SIM_ID nSimID)
{
    uint8_t resultFlag = 1;
    SSParsedParameters Parsed;
    l4_op_code_enum OpCode = 0;
    ss_code_enum SSCode = 0;
    /* parse the mmi string */
    OSI_LOGXI(OSI_LOGPAR_SI, 0, "ss:AT_SS_Adaption_ParseStringREQ pReq->input=%s length=%d", pReq->input, pReq->length);
    resultFlag = ParseUserInput(pReq->input, pReq->length, &Parsed);
    OSI_LOGXI(OSI_LOGPAR_SI, 0, "ss:AT_SS_Adaption_ParseStringREQ: pReq=%s resultFlag=%d", pReq->input, resultFlag);
    if (resultFlag == 1)
    {
        /* Determine the type of operation and process accordingly */
        GetTypeOfOperation(pReq->input, &Parsed, &OpCode, &SSCode);
        OSI_LOGI(0, "ss:AT_SS_Adaption_ParseStringREQ: OpCode=%d SSCode=%d", OpCode, SSCode);
        OSI_LOGI(0, "ss:Parsed->length[1]=%d Parsed->length[2]=%d", Parsed.parmLen[1], Parsed.parmLen[2]);
        OSI_LOGI(0, "ss:Parsed->parmStart[1]=%d Parsed->parmStart[2]=%d", Parsed.parmStart[1], Parsed.parmStart[2]);
        /* process accordingly */
        switch (OpCode)
        {
        case SS_OP_REGISTERSS:
            /*SS Register REQ, invoke CSW function.*/
            SS_RegisterREQ(pReq->input, &Parsed, SSCode, nDLCI, nSimID);
            break;

        case SS_OP_ERASESS:
            /*SS Erase REQ, invoke CSW function.*/
            SS_EraseREQ(pReq->input, &Parsed, SSCode, nDLCI, nSimID);
            break;

        case SS_OP_ACTIVATESS:
            /*SS Activate REQ, invoke CSW function.*/
            SS_ActivateREQ(pReq->input, &Parsed, SSCode, nDLCI, nSimID);
            break;

        case SS_OP_DEACTIVATESS:
            /*SS Deactivate REQ, invoke CSW function.*/
            SS_DeactivateREQ(pReq->input, &Parsed, SSCode, nDLCI, nSimID);
            break;

        case SS_OP_INTERROGATESS:
            /*SS Interrogate REQ, invoke CSW function.*/
            SS_InterrogateREQ(pReq->input, &Parsed, SSCode, nDLCI, nSimID);
            break;

        case SS_OP_REGISTERPASSWORD:
            resultFlag = 0;
            OSI_LOGI(0x1000455b, "ss:ParseStringREQ ERROR! don't support password!");
            /*GSM2.30 V4.11.0 page 15 states that only registartion shall be allowed*/
            /*this means that process only if * or ** prefix in string           */
            if ((Parsed.type == SSMMI_STAR) || (Parsed.type == SSMMI_STARSTAR))
            {
                /*SS RegPassword REQ, invoke CSW function.*/
                //SS_RegPasswordREQ(pReq->input, &Parsed, SSCode);
            }
            else
            {
                /*Indicate Parse string failed.*/
                //SS_Adaption_SendParseStringRSP(PARSE_STRING_FAILED);
            }
            break;

        case SS_OP_PROCESSUNSTRUCTUREDSS_DATA:
            resultFlag = 0;
            OSI_LOGI(0x1000455c, "ss:ParseStringREQ ERROR! don't support USSD!");
            /*SS ProcUss REQ, invoke CSW function.*/
            //resultFlag = SS_ProcUssReq(pReq->input, pReq->length);
            //SS_Adaption_SendParseStringRSP(resultFlag);
            break;

        default:
            /* this should never happen */
            resultFlag = 0;
            OSI_LOGI(0x1000455d, "ss:AT_SS_Adaption_ParseStringREQ default resultFlag = %d", resultFlag);
            //assert(0);
            break;
        }
    }
    else
    {
        resultFlag = 0;
        //SS_Adaption_SendParseStringRSP(resultFlag);
    }

    return (bool)resultFlag;
}

static void CACM_SetRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nType != 0)
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);

    RETURN_OK(cmd->engine);
}

static void CACM_GetRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nType != 0)
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);

    char rsp[64];
    uint8_t *pData = (uint8_t *)&cfw_event->nParam1;
    sprintf(rsp, "+CACM: \"%02X%02X%02X\"", pData[0], pData[1], pData[2]);
    atCmdRespInfoText(cmd->engine, rsp);
    RETURN_OK(cmd->engine);
}

// 8.25 Accumulated call meter
void atCmdHandleCACM(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (cmd->type == AT_CMD_SET)
    {
        // +CACM=[<passwd>]
        bool paramok = true;
        const char *passwd = atParamDefStr(cmd->params[0], "", &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CACM_SetRspCB, cmd);
        if ((ret = CFW_SimSetACM(0, (uint8_t *)passwd, strlen(passwd), cmd->uti, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_CFW_ERR(cmd->engine, ret);
        }
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        cmd->uti = cfwRequestUTI((osiEventCallback_t)CACM_GetRspCB, cmd);
        if ((ret = CFW_SimGetACM(cmd->uti, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_CFW_ERR(cmd->engine, ret);
        }
        RETURN_FOR_ASYNC();
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
