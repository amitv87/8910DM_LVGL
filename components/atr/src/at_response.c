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
#include "osi_vsmap.h"
#include "at_response.h"
#include "at_engine_imp.h"
#include "at_command.h"
#include "at_response.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "quec_common.h"
#include "quec_urc.h"
#include "quec_atcmd_general.h"
#ifdef CONFIG_QUEC_PROJECT_FEATURE
#define quec_atresp_log(msg, ...)  custom_log("ATRESP", msg, ##__VA_ARGS__)
#endif

static const osiValueStrMap_t gAtResultText[] = {
    {CMD_RC_OK, "OK"},
    {CMD_RC_CONNECT, "CONNECT"},
    {CMD_RC_RING, "RING"},
    {CMD_RC_NOCARRIER, "NO CARRIER"},
    {CMD_RC_ERROR, "ERROR"},
    {CMD_RC_NOTSUPPORT, "NOT SUPPORT"},
    {CMD_RC_NODIALTONE, "NO DIALTONE"},
    {CMD_RC_BUSY, "BUSY"},
    {CMD_RC_NOANSWER, "NO ANSWER"},
    {CMD_RC_INVCMDLINE, "INVALID COMMAND LINE"},
    {CMD_RC_CR, ""},
    {CMD_RC_SIMDROP, "SIM not inserted"},
};

// The CME error information.
static const osiValueStrMap_t gAtCmeText[] = {
    //
    // General errors
    //
    {ERR_AT_CME_PHONE_FAILURE, "phone failure"},
    {ERR_AT_CME_NO_CONNECT_PHONE, "no connection to phone"},
    {ERR_AT_CME_PHONE_ADAPTER_LINK_RESERVED, "phone-adapter link reserved"},
    {ERR_AT_CME_OPERATION_NOT_ALLOWED, "Operation not allowed"},
    {ERR_AT_CME_OPERATION_NOT_SUPPORTED, "Operation not supported"},
    {ERR_AT_CME_PHSIM_PIN_REQUIRED, "PH-SIM PIN required"},
    {ERR_AT_CME_PHFSIM_PIN_REQUIRED, "PH-FSIM PIN required"},
    {ERR_AT_CME_PHFSIM_PUK_REQUIRED, "PH-FSIM PUK required"},
    {ERR_AT_CME_SIM_NOT_INSERTED, "SIM not inserted"},
    {ERR_AT_CME_SIM_PIN_REQUIRED, "SIM PIN required"},
    {ERR_AT_CME_SIM_PUK_REQUIRED, "SIM PUK required"},
    {ERR_AT_CME_SIM_FAILURE, "SIM failure"},
    {ERR_AT_CME_SIM_BUSY, "SIM busy"},
    {ERR_AT_CME_SIM_WRONG, "SIM wrong"},
    {ERR_AT_CME_INCORRECT_PASSWORD, "Incorrect password"},
    {ERR_AT_CME_SIM_PIN2_REQUIRED, "SIM PIN2 required"},
    {ERR_AT_CME_SIM_PUK2_REQUIRED, "SIM PUK2 required"},
    {ERR_AT_CME_MEMORY_FULL, "Memory full"},
    {ERR_AT_CME_INVALID_INDEX, "invalid index"},
    {ERR_AT_CME_NOT_FOUND, "not found"},
    {ERR_AT_CME_MEMORY_FAILURE, "Memory failure"},
    {ERR_AT_CME_TEXT_LONG, "text string too long"},
    {ERR_AT_CME_INVALID_CHAR_INTEXT, "invalid characters in text string"},
    {ERR_AT_CME_DAIL_STR_LONG, "dial string too long"},
    {ERR_AT_CME_INVALID_CHAR_INDIAL, "invalid characters in dial string"},
    {ERR_AT_CME_NO_NET_SERVICE, "no network service"},
    {ERR_AT_CME_NETWORK_TIMOUT, "Network timeout"},
    {ERR_AT_CME_NOT_ALLOW_EMERGENCY, "Network not allowed emergency calls only"},
    {ERR_AT_CME_NET_PER_PIN_REQUIRED, "Network personalization PIN required"},
    {ERR_AT_CME_NET_PER_PUK_REQUIRED, "Network personalization PUK required"},
    {ERR_AT_CME_NET_SUB_PER_PIN_REQ, "Network subset personalization PIN required"},
    {ERR_AT_CME_NET_SUB_PER_PUK_REQ, "Network subset personalization PUK required"},
    {ERR_AT_CME_SERVICE_PROV_PER_PIN_REQ, "service provider personalization PIN required"},
    {ERR_AT_CME_SERVICE_PROV_PER_PUK_REQ, "service provider personalization PUK required"},
    {ERR_AT_CME_CORPORATE_PER_PIN_REQ, "Corporate personalization PIN required"},
    {ERR_AT_CME_CORPORATE_PER_PUK_REQ, "Corporate personalization PUK required"},
    {ERR_AT_CME_PHSIM_PBK_REQUIRED, "PH-SIM PUK required (PH-SIM PUK may also be referred to as Master Phone Code. For further details"},

    {ERR_AT_CME_EXE_NOT_SURPORT, "The excute command not support"},
    {ERR_AT_CME_EXE_FAIL, "Excute command failure"},
    {ERR_AT_CME_NO_MEMORY, "no memory"},
    {ERR_AT_CME_OPTION_NOT_SURPORT, "The command not support,check your input,pls"},
    {ERR_AT_CME_PARAM_INVALID, "parameters are invalid"},
    {ERR_AT_CME_EXT_REG_NOT_EXIT, "REG not exit in flash"},
    {ERR_AT_CME_EXT_SMS_NOT_EXIT, "SMS not eixt in flash"},
    {ERR_AT_CME_EXT_PBK_NOT_EXIT, "Phone book not eixt in flash"},
    {ERR_AT_CME_EXT_FFS_NOT_EXIT, "file system  not eixt in flash"},
    {ERR_AT_CME_INVALID_COMMAND_LINE, "invalid command line"},
    {ERR_AT_CME_FILE_NOT_EXIST, "file not exist"},
    {ERR_AT_CME_FILE_TOO_LARGE, "file too large"},

    {ERR_AT_CME_INVALID_DATE_OR_TIME, "invalid date or time"},
    {ERR_AT_CME_DIR_CREATE_FAIL, "directory create fail"},
    {ERR_AT_CME_DIR_NOT_EXIST, "directory not exist"},

    {ERR_AT_CME_GPRS_ILLEGAL_MS_3, "Illegal MS"},
    {ERR_AT_CME_GPRS_ILLEGAL_MS_6, "Illegal ME"},
    {ERR_AT_CME_GPRS_SVR_NOT_ALLOWED, "GPRS services not allowed"},
    {ERR_AT_CME_GPRS_PLMN_NOT_ALLOWED, "PLMN not allowed"},
    {ERR_AT_CME_GPRS_LOCATION_AREA_NOT_ALLOWED, "Location area not allowed"},
    {ERR_AT_CME_GPRS_ROAMING_NOT_ALLOWED, "Roaming not allowed in this location area"},
    {ERR_AT_CME_GPRS_OPTION_NOT_SUPPORTED, "Service option not supported"},
    {ERR_AT_CME_GPRS_OPTION_NOT_SUBSCRIBED, "Requested service option not subscribed"},
    {ERR_AT_CME_GPRS_OPTION_TEMP_ORDER_OUT, "Service option temporarily out of order"},
    {ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, "Unspecified GPRS error"},
    {ERR_AT_CME_GPRS_PDP_AUTHENTICATION_FAILURE, "PDP authentication failure"},
    {ERR_AT_CME_GPRS_INVALID_MOBILE_CLASS, "Invalid mobile class"},
    {ERR_AT_CME_GPRS_UNSUPPORTED_QCI_VALUE, "Unsupproted QCI value"},

    {ERR_AT_CME_SIM_VERIFY_FAIL, "SIM card verify failure"},
    {ERR_AT_CME_SIM_UNBLOCK_FAIL, "Unblock SIM card fail"},
    {ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, "Condition not fullfilled"},
    {ERR_AT_CME_SIM_UNBLOCK_FAIL_NO_LEFT, "Unblock SIM card failed, no rest time"},
    {ERR_AT_CME_SIM_VERIFY_FAIL_NO_LEFT, "Verify SIM card failed, no rest time"},
    {ERR_AT_CME_SIM_INVALID_PARAMETER, "Input parameter is invalid"},
    {ERR_AT_CME_SIM_UNKNOW_COMMAND, "file is inconsistent with the command"},
    {ERR_AT_CME_SIM_WRONG_CLASS, "wrong instruction class given in the command"},
    {ERR_AT_CME_SIM_TECHNICAL_PROBLEM, "SIM returned technical problem"},
    {ERR_AT_CME_SIM_CHV_NEED_UNBLOCK, "CHV need unblock"},
    {ERR_AT_CME_SIM_NOEF_SELECTED, "NO SIM EF selected"},
    {ERR_AT_CME_SIM_FILE_UNMATCH_COMMAND, "SIM file unmatch command"},
    {ERR_AT_CME_SIM_CONTRADICTION_CHV, "contradiction CHV"},
    {ERR_AT_CME_SIM_CONTRADICTION_INVALIDATION, "contradiction invalidation"},
    {ERR_AT_CME_SIM_MAXVALUE_REACHED, "SIM MAX value reached"},
    {ERR_AT_CME_SIM_PATTERN_NOT_FOUND, "SIM returned pattern not found"},
    {ERR_AT_CME_SIM_FILEID_NOT_FOUND, "SIM file ID not found"},
    {ERR_AT_CME_SIM_STK_BUSY, "STK busy"},
    {ERR_AT_CME_SIM_UNKNOW, "SIM returned UNKNOWN"},
    {ERR_AT_CME_SIM_PROFILE_ERROR, "SIM profile ERR"},
    {ERR_AT_CME_HTTP_NETIF_NULL, "netif is error"},
    {ERR_AT_CME_HTTP_ACTION_NEED, "HTTPACTION is needed first"},
    {ERR_AT_CME_HTTP_INIT_NEED, "HTTPINIT is needed first"},
    {ERR_AT_CME_HTTP_PARA_CID_INVALID, "HTTPPARA CID invalid"},
    {ERR_AT_CME_SNTP_SYNCING, "SNTP is syncing time, wait it done and try it later"},
    {ERR_AT_CME_NETWORK_REFUSED, "username or password is invalid"},
    {ERR_AT_CME_PDP_DEACTIVED, "pdp have not actived"},
    {ERR_AT_CME_PDP_ACTIVED, "pdp is actived"},
    {ERR_AT_CME_TCPIP_PEER_REFUSED, "peer refused"},
    {ERR_AT_CME_TCPIP_CONNECT_TIMEOUT, "tcp connection timeout"}, //IP OR PORT IS NOT CORRECT
    {ERR_AT_CME_TCPIP_ALREADY_CONNECTED, "socket is connected"},
    {ERR_AT_CME_TCPIP_NOT_CONNECTED, "socket is not connected"},
    {ERR_AT_CME_BUFFER_FULL, "buffer is full"},
    {ERR_AT_CME_SEND_TIMEOUT, "send data timeout"},
    {ERR_AT_CME_DNS_FAILED, "dns failed"},
    {ERR_AT_CME_DNS_TIMEOUT, "dns timeout"},
    {ERR_AT_CME_DNS_UNKNOW_ERROR, "dns unknown error"},
    {ERR_AT_CME_PARAM_ERROR, "param error"},
    {ERR_AT_CME_UNKNOWN_ERROR, "unknown error"},
};

// The CMS error information.
static const osiValueStrMap_t gAtCmsText[] = {
    {ERR_AT_CMS_UNASSIGNED_NUM, "Unassigned (unallocated) number"},
    {ERR_AT_CMS_OPER_DETERM_BARR, "Operator determined barring"},
    {ERR_AT_CMS_CALL_BARRED, "Call barred"},
    {ERR_AT_CMS_SM_TRANS_REJE, "Short message transfer rejected"},
    {ERR_AT_CMS_DEST_OOS, "Destination out of service"},
    {ERR_AT_CMS_UNINDENT_SUB, "Unidentified subscriber"},
    {ERR_AT_CMS_FACILIT_REJE, "Facility rejected"},
    {ERR_AT_CMS_UNKONWN_SUB, "Unknown subscriber"},
    {ERR_AT_CMS_NW_OOO, "Network out of order"},
    {ERR_AT_CMS_TMEP_FAIL, "Temporary failure"},
    {ERR_AT_CMS_CONGESTION, "Congestion"},
    {ERR_AT_CMS_RES_UNAVAILABLE, "Resources unavailable, unspecified"},
    {ERR_AT_CMS_REQ_FAC_NOT_SUB, "Requested facility not subscribed"},
    {ERR_AT_CMS_RFQ_FAC_NOT_IMP, "Requested facility not implemented"},
    {ERR_AT_CMS_INVALID_SM_TRV, "Invalid short message transfer reference value"},
    {ERR_AT_CMS_INVALID_MSG, "Invalid message, unspecified"},
    {ERR_AT_CMS_INVALID_MAND_INFO, "Invalid mandatory information"},
    {ERR_AT_CMS_MSG_TYPE_ERROR, "Message type non-existent or not implemented"},
    {ERR_AT_CMS_MSG_NOT_COMP, "Message not compatible with short message protocol state"},
    {ERR_AT_CMS_INFO_ELEMENT_ERROR, "Information element non-existent or not implemented"},
    {ERR_AT_CMS_PROT_ERROR, " Protocol error, unspecified"},
    {ERR_AT_CMS_IW_UNSPEC, "Interworking, unspecified"},
    {ERR_AT_CMS_TEL_IW_NOT_SUPP, "Telematic interworking not supported"},
    {ERR_AT_CMS_SMS_TYPE0_NOT_SUPP, "Short message Type 0 not supported"},
    {ERR_AT_CMS_CANNOT_REP_SMS, "Cannot replace short message"},
    {ERR_AT_CMS_UNSPEC_TP_ERROR, "Unspecified TP-PID error"},
    {ERR_AT_CMS_DCS_NOT_SUPP, "Data coding scheme (alphabet) not supported"},
    {ERR_AT_CMS_MSG_CLASS_NOT_SUPP, "Message class not supported"},
    {ERR_AT_CMS_UNSPEC_TD_ERROR, "Unspecified TP-DCS error"},
    {ERR_AT_CMS_CMD_CANNOT_ACT, "Command cannot be actioned "},
    {ERR_AT_CMS_CMD_UNSUPP, "Command unsupported"},
    {ERR_AT_CMS_UNSPEC_TC_ERROR, "Unspecified TP-Command error"},
    {ERR_AT_CMS_TPDU_NOT_SUPP, "TPDU not supported"},
    {ERR_AT_CMS_SC_BUSY, "SC busy"},
    {ERR_AT_CMS_NO_SC_SUB, "No SC subscription"},
    {ERR_AT_CMS_SC_SYS_FAIL, "SC system failure"},
    {ERR_AT_CMS_INVALID_SME_ADDR, "Invalid SME address"},
    {ERR_AT_CMS_DEST_SME_BARR, "Destination SME barred"},
    {ERR_AT_CMS_SM_RD_SM, "SM Rejected-Duplicate SM"},
    {ERR_AT_CMS_TP_VPF_NOT_SUPP, "TP-VPF not supported"},
    {ERR_AT_CMS_TP_VP_NOT_SUPP, "TP-VP not supported"},
    {ERR_AT_CMS_D0_SIM_SMS_STO_FULL, "D0 SIM SMS storage full"},
    {ERR_AT_CMS_NO_SMS_STO_IN_SIM, "No SMS storage capability in SIM"},
    {ERR_AT_CMS_ERR_IN_MS, "Error in MS"},
    {ERR_AT_CMS_MEM_CAP_EXCCEEDED, "Memory Capacity Exceeded"},
    {ERR_AT_CMS_SIM_APP_TK_BUSY, "SIM Application Toolkit Busy"},
    {ERR_AT_CMS_SIM_DATA_DL_ERROR, "SIM data download error"},
    {ERR_AT_CMS_UNSPEC_ERRO_CAUSE, "Unspecified error cause"},

    {ERR_AT_CMS_ME_FAIL, "ME failure"},
    {ERR_AT_CMS_SMS_SERVIEC_RESERVED, "SMS service of ME reserved"},
    {ERR_AT_CMS_OPER_NOT_ALLOWED, "Operation not allowed"},
    {ERR_AT_CMS_OPER_NOT_SUPP, "Operation not supported"},
    {ERR_AT_CMS_INVALID_PDU_PARAM, "Invalid PDU mode parameter"},
    {ERR_AT_CMS_INVALID_TXT_PARAM, "Invalid text mode parameter"},
    {ERR_AT_CMS_SIM_NOT_INSERT, "SIM not inserted"},
    {ERR_AT_CMS_SIM_PIN_REQUIRED, "SIM PIN required"},
    {ERR_AT_CMS_PH_SIM_PIN_REQUIRED, "PH-SIM PIN required"},
    {ERR_AT_CMS_SIM_FAIL, "SIM failure"},
    {ERR_AT_CMS_SIM_BUSY, "SIM busy"},
    {ERR_AT_CMS_SIM_WRONG, "SIM wrong"},
    {ERR_AT_CMS_SIM_PUK_REQUIRED, "SIM PUK required"},
    {ERR_AT_CMS_SIM_PIN2_REQUIRED, "SIM PIN2 required"},
    {ERR_AT_CMS_SIM_PUK2_REQUIRED, "SIM PUK2 required"},
    {ERR_AT_CMS_MEM_FAIL, "Memory failure"},
    {ERR_AT_CMS_INVALID_MEM_INDEX, "Invalid memory index"},
    {ERR_AT_CMS_MEM_FULL, "Memory full"},
    {ERR_AT_CMS_SCA_ADDR_UNKNOWN, "SMSC address unknown"},
    {ERR_AT_CMS_NO_NW_SERVICE, "no network service"},
    {ERR_AT_CMS_NW_TIMEOUT, "Network timeout"},
    {ERR_AT_CMS_NO_CNMA_ACK_EXPECTED, "NO +CNMA ACK EXPECTED"},
    {ERR_AT_CMS_UNKNOWN_ERROR, "Unknown error"},

    {ERR_AT_CMS_USER_ABORT, "User abort"},
    {ERR_AT_CMS_UNABLE_TO_STORE, "unable to store"},
    {ERR_AT_CMS_INVALID_STATUS, "invalid status"},
    {ERR_AT_CMS_INVALID_ADDR_CHAR, "invalid character in address string"},
    {ERR_AT_CMS_INVALID_LEN, "invalid length"},
    {ERR_AT_CMS_INVALID_PDU_CHAR, "invalid character in pdu"},
    {ERR_AT_CMS_INVALID_PARA, "invalid parameter"},
    {ERR_AT_CMS_INVALID_LEN_OR_CHAR, "invalid length or character"},
    {ERR_AT_CMS_INVALID_TXT_CHAR, "invalid character in text"},
    {ERR_AT_CMS_TIMER_EXPIRED, "timer expired"},
    {ERR_AT_CMS_SMS_SEND_FAIL, "send sms error"},
};

static const char *_getCodeText(int code, const osiValueStrMap_t *info, unsigned count)
{
    const char *p = osiVsmapBsearch(code, info, count, NULL);
    if (p != NULL)
        return p;

    static char text[16];
    sprintf(text, "%d", code);
    return text;
}
static bool existSameSimIDChannel(uint8_t sim)
{
    osiSlistHead_t *list = atDispatchGetList();
    osiSlistItem_t *item;
    SLIST_FOREACH(item, list, iter)
    {
        atDispatch_t *dispatch = (atDispatch_t *)item;
        if (dispatch != NULL && atDispatchIsCmdMode(dispatch) && atDispatchGetDeviceReadyStatus(dispatch))
        {
            atCmdEngine_t *cmd_engine = atDispatchGetCmdEngine(dispatch);
            if (sim == atCmdGetSim(cmd_engine))
                return true;
        }
    }
    return false;
}
static const char *_resultCodeText(int code)
{
    return _getCodeText(code, gAtResultText, OSI_ARRAY_SIZE(gAtResultText));
}

static const char *_cmeCodeText(int code)
{
    return _getCodeText(code, gAtCmeText, OSI_ARRAY_SIZE(gAtCmeText));
}

static const char *_cmsCodeText(int code)
{
    return _getCodeText(code, gAtCmsText, OSI_ARRAY_SIZE(gAtCmsText));
}

void atCodeTextCheck(void)
{
    osiVsmapIsSorted(gAtResultText, OSI_ARRAY_SIZE(gAtResultText));
    osiVsmapIsSorted(gAtCmeText, OSI_ARRAY_SIZE(gAtCmeText));
    osiVsmapIsSorted(gAtCmsText, OSI_ARRAY_SIZE(gAtCmsText));
}

// =============================================================================
// atCmdRespInfoNText/atCmdRespInfoText
// =============================================================================
void atCmdRespInfoNText(atCmdEngine_t *engine, const char *text, size_t length)
{
    if (text != NULL)
        OSI_LOGXI(OSI_LOGPAR_IIS, 0x1000006c, "AT CMD%d info text len=%d: %s",
                  atCmdChannelIndex(engine), length, text);
    else
        OSI_LOGI(0x10005280, "AT CMD%d info text len=%d: (null)",
                 atCmdChannelIndex(engine), length);

    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
    uint8_t crlf[2] = {chsetting->s3, chsetting->s4};
    uint8_t atv = chsetting->atv;

    if (atCmdIsFirstInfoText(engine) && atv == 1)
        atCmdWrite(engine, crlf, 2);
    atCmdSetFirstInfoText(engine, false);
    if ((length > 0) && (text != NULL))
        atCmdWrite(engine, text, length);
    atCmdWrite(engine, crlf, 2);
    atCmdWriteFlush(engine);
}

void atCmdRespInfoText(atCmdEngine_t *engine, const char *text)
{
    atCmdRespInfoNText(engine, text, strlen(text));
}

void atCmdRespInfoNTextBegin(atCmdEngine_t *engine, const char *text, size_t length)
{
    if (text == NULL)
        OSI_LOGXI(OSI_LOGPAR_IIS, 0x10005281, "AT CMD%d info text begin len=%d: %s",
                  atCmdChannelIndex(engine), length, text);
    else
        OSI_LOGI(0x10005282, "AT CMD%d info text begin len=%d: (null)",
                 atCmdChannelIndex(engine), length);

    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
    uint8_t crlf[2] = {chsetting->s3, chsetting->s4};
    uint8_t atv = chsetting->atv;

    if (atCmdIsFirstInfoText(engine) && atv == 1)
        atCmdWrite(engine, crlf, 2);
    atCmdSetFirstInfoText(engine, false);
    if ((length > 0) && (text != NULL))
        atCmdWrite(engine, text, length);
}

void atCmdRespInfoTextBegin(atCmdEngine_t *engine, const char *text)
{
    atCmdRespInfoNTextBegin(engine, text, strlen(text));
}

void atCmdRespInfoNTextEnd(atCmdEngine_t *engine, const char *text, size_t length)
{
    if (text == NULL)
        OSI_LOGXI(OSI_LOGPAR_IIS, 0x10005283, "AT CMD%d info text end len=%d: %s",
                  atCmdChannelIndex(engine), length, text);
    else
        OSI_LOGI(0x10005284, "AT CMD%d info text end len=%d: (null)",
                 atCmdChannelIndex(engine), length);

    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
    uint8_t crlf[2] = {chsetting->s3, chsetting->s4};
    if ((length > 0) && (text != NULL))
        atCmdWrite(engine, text, length);
    atCmdWrite(engine, crlf, 2);
    atCmdWriteFlush(engine);
}

void atCmdRespInfoTextEnd(atCmdEngine_t *engine, const char *text)
{
    atCmdRespInfoNTextEnd(engine, text, strlen(text));
}

// =============================================================================
// atCmdRespOutputResultCode
// =============================================================================
static void atCmdRespOutputResultCode(atCmdEngine_t *engine, int code)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
    uint8_t atq = chsetting->atq;
    if (atq != 0)
        return;

    uint8_t crlf[2] = {chsetting->s3, chsetting->s4};
    uint8_t atv = chsetting->atv;
    if (atv == 0)
    {
        char resp[32];
        int len = sprintf(resp, "%u", code);
        atCmdWrite(engine, resp, len);
        atCmdWrite(engine, crlf, 1);
    }
    else
    {
        atCmdWrite(engine, crlf, 2);
        const char *s = _resultCodeText(code);
        atCmdWrite(engine, s, strlen(s));
        atCmdWrite(engine, crlf, 2);
    }
    atCmdWriteFlush(engine);
	
#ifdef CONFIG_QUEC_PROJECT_FEATURE
	quec_urc_flush();
#endif

}

// =============================================================================
// atCmdRespOK
// =============================================================================
void atCmdRespOK(atCmdEngine_t *engine)
{
    OSI_LOGI(0x1000006d, "AT CMD%d OK", atCmdChannelIndex(engine));

    atCmdFinalHandle(engine, true);

    if (atCmdListIsEmpty(engine))
        atCmdRespOutputResultCode(engine, CMD_RC_OK);

    atCmdCommandFinished(engine);
}

// =============================================================================
// atCmdRespOKText
// =============================================================================
void atCmdRespOKText(atCmdEngine_t *engine, const char *text)
{
    OSI_LOGXI(OSI_LOGPAR_IS, 0x1000006e, "AT CMD%d OK: %s",
              atCmdChannelIndex(engine), text);

    atCmdFinalHandle(engine, true);

    // output it even it is not the last one, and not affected by ATV, ATQ
    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
    uint8_t crlf[2] = {chsetting->s3, chsetting->s4};
    atCmdWrite(engine, crlf, 2);
    atCmdWrite(engine, text, strlen(text));
    atCmdWrite(engine, crlf, 2);
    atCmdWriteFlush(engine);

    atCmdCommandFinished(engine);

#ifdef CONFIG_QUEC_PROJECT_FEATURE
		quec_urc_flush();
#endif


}

// =============================================================================
// atCmdRespError
// =============================================================================
void atCmdRespError(atCmdEngine_t *engine)
{
    OSI_LOGI(0x1000006f, "AT CMD%d ERROR", atCmdChannelIndex(engine));

    atCmdRespErrorCode(engine, CMD_RC_ERROR);
}

// =============================================================================
// atCmdRespErrorText
// =============================================================================
void atCmdRespErrorText(atCmdEngine_t *engine, const char *text)
{
    OSI_LOGXI(OSI_LOGPAR_IS, 0x10000070, "AT CMD%d ERROR: %s",
              atCmdChannelIndex(engine), text);

    atCmdFinalHandle(engine, false);

    // not affected by ATV, ATQ
    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
    uint8_t crlf[2] = {chsetting->s3, chsetting->s4};
    atCmdWrite(engine, crlf, 2);
    atCmdWrite(engine, text, strlen(text));
    atCmdWrite(engine, crlf, 2);
    atCmdWriteFlush(engine);

    atCmdCommandFinished(engine);
	
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		quec_urc_flush();
#endif

}

// =============================================================================
// atCmdRespErrorCode
// =============================================================================
void atCmdRespErrorCode(atCmdEngine_t *engine, int code)
{
    OSI_LOGI(0x10000071, "AT CMD%d ERROR code=%d",
             atCmdChannelIndex(engine), code);

    atCmdFinalHandle(engine, false);
    atCmdRespOutputResultCode(engine, code);

    atCmdCommandFinished(engine);
}

// =============================================================================
// atCmdRespIntermCode
// =============================================================================
void atCmdRespIntermCode(atCmdEngine_t *engine, int code)
{
    OSI_LOGI(0x10000072, "AT CMD%d interm code=%d",
             atCmdChannelIndex(engine), code);

    atCmdRespOutputResultCode(engine, code);
}

// =============================================================================
// atCmdRespFinish
// =============================================================================
void atCmdRespFinish(atCmdEngine_t *engine)
{
    OSI_LOGI(0x10005285, "AT CMD%d finish", atCmdChannelIndex(engine));

    atCmdFinalHandle(engine, true);
    atCmdCommandFinished(engine);
}

// =============================================================================
// atCmdRespUrcCode
// =============================================================================
void atCmdRespUrcCode(atCmdEngine_t *engine, int code)
{
#ifdef CONFIG_QUEC_PROJECT_FEATURE

#else
    OSI_LOGI(0x10000073, "AT CMD%d urc code=%d",
             atCmdChannelIndex(engine), code);

    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
    uint8_t atq = chsetting->atq;
    if (atq != 0)
        return;

    // TODO: CMER is not considered
    uint8_t crlf[2] = {chsetting->s3, chsetting->s4};
    uint8_t atv = chsetting->atv;

    atCmdUrcStart(engine);

    if (atv == 0)
    {
        char resp[32];
        int len = sprintf(resp, "%u", code);
        atCmdUrcWrite(engine, resp, len);
        atCmdUrcWrite(engine, crlf, 1);
    }
    else
    {
        atCmdUrcWrite(engine, crlf, 2);
        const char *s = _resultCodeText(code);
        atCmdUrcWrite(engine, s, strlen(s));
        atCmdUrcWrite(engine, crlf, 2);
    }

    atCmdUrcStop(engine);
#endif
}

// =============================================================================
// atCmdRespUrcNText/atCmdRespUrcText
// =============================================================================
void atCmdRespUrcNText(atCmdEngine_t *engine, const char *text, size_t length)
{
#ifdef CONFIG_QUEC_PROJECT_FEATURE
	
#else

    if (!atCmdEngineIsValid(engine))
        return;

    if (text != NULL)
    {
        OSI_LOGXI(OSI_LOGPAR_IIS, 0x10000074, "AT CMD%d urc len=%d: %s",
                  atCmdChannelIndex(engine), length, text);
    }
    else
    {
        OSI_LOGI(0x10005286, "AT CMD%d urc len=%d: (null)",
                 atCmdChannelIndex(engine), length);
    }

    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
    uint8_t atq = chsetting->atq;
    if (atq != 0)
        return;

    // TODO: CMER is not considered
    uint8_t crlf[2] = {chsetting->s3, chsetting->s4};

#if defined(AT_CMUX_SUPPORT) && defined(AT_URC_2_ALL_MUX_CHANNEL)
    atCmdEngine_t *engines[MAX_DLC_NUM] = {NULL};
    unsigned count = at_DispatchGetAllCmdEngine(engines, MAX_DLC_NUM);

    for (unsigned i = 0; i < count; i++)
    {
        if (engines[i])
        {
            atCmdUrcStart(engines[i]);

            atCmdUrcWrite(engines[i], crlf, 2);
            if ((length > 0) && (text != NULL))
                atCmdUrcWrite(engines[i], text, length);
            atCmdUrcWrite(engines[i], crlf, 2);

            atCmdUrcStop(engines[i]);
        }
    }
#else
    atCmdUrcStart(engine);

    atCmdUrcWrite(engine, crlf, 2);
    if ((length > 0) && (text != NULL))
        atCmdUrcWrite(engine, text, length);
    atCmdUrcWrite(engine, crlf, 2);

    atCmdUrcStop(engine);
#endif

#endif
}

void atCmdRespUrcText(atCmdEngine_t *engine, const char *text)
{
    atCmdRespUrcNText(engine, text, strlen(text));
}

void atCmdRespDefUrcCode(int code)
{
    osiSlistHead_t *list = atDispatchGetList();
    osiSlistItem_t *item;
    SLIST_FOREACH(item, list, iter)
    {
        atDispatch_t *dispatch = (atDispatch_t *)item;
        if (dispatch != NULL && atDispatchIsCmdMode(dispatch))
            atCmdRespUrcCode(atDispatchGetCmdEngine(dispatch), code);
    }
}

void atCmdRespDefUrcNText(const char *text, size_t length)
{
    osiSlistHead_t *list = atDispatchGetList();
    osiSlistItem_t *item;
    SLIST_FOREACH(item, list, iter)
    {
        atDispatch_t *dispatch = (atDispatch_t *)item;
        if (dispatch != NULL && atDispatchIsCmdMode(dispatch))
            atCmdRespUrcNText(atDispatchGetCmdEngine(dispatch), text, length);
    }
}

void atCmdRespDefUrcText(const char *text)
{
    atCmdRespDefUrcNText(text, strlen(text));
}

void atCmdRespSimUrcCode(uint8_t sim, int code)
{
#ifdef CONFIG_QUEC_PROJECT_FEATURE
	char *s = (char *)_resultCodeText(code);
	quec_urc_send(NULL, s, strlen(s), URC_OUT_METHOD_UNICAST, RI_CATEG_RING, 0);
#else
    osiSlistHead_t *list = atDispatchGetList();
    osiSlistItem_t *item;
    SLIST_FOREACH(item, list, iter)
    {
        atDispatch_t *dispatch = (atDispatch_t *)item;
        if (dispatch != NULL && atDispatchIsCmdMode(dispatch))
            atCmdRespUrcCode(atDispatchGetCmdEngine(dispatch), code);
    }
#endif
}

void atCmdRespSimUrcNText(uint8_t sim, const char *text, size_t length)
{
    bool bCheckSim = existSameSimIDChannel(sim);

    osiSlistHead_t *list = atDispatchGetList();
    osiSlistItem_t *item;
    SLIST_FOREACH(item, list, iter)
    {
        atDispatch_t *dispatch = (atDispatch_t *)item;
        if (dispatch != NULL && atDispatchIsCmdMode(dispatch) && atDispatchGetDeviceReadyStatus(dispatch))
        {
            if (bCheckSim && (sim != atCmdGetSim(atDispatchGetCmdEngine(dispatch))))
                continue;
            atCmdRespUrcNText(atDispatchGetCmdEngine(dispatch), text, length);
        }
    }
}

void atCmdRespSimUrcText(uint8_t sim, const char *text)
{
    atCmdRespSimUrcNText(sim, text, strlen(text));
}

// =============================================================================
// atCmdRespCmeError
// =============================================================================
void atCmdRespCmeError(atCmdEngine_t *engine, int code)
{
    OSI_LOGI(0x10000075, "AT CMD%d CME error code=%d", atCmdChannelIndex(engine), code);

    atCmdFinalHandle(engine, false);

    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
    uint8_t atq = chsetting->atq;
    if (atq == 0)
    {
        uint8_t cmee = chsetting->cmee;
        if (cmee == 0)
        {
            atCmdRespOutputResultCode(engine, CMD_RC_ERROR);
        }
        else
        {
            atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
            uint8_t crlf[2] = {chsetting->s3, chsetting->s4};
            atCmdWrite(engine, crlf, 2);
            atCmdWrite(engine, "+CME ERROR: ", strlen("+CME ERROR: "));
            if (cmee == 1)
            {
                char text[16];
                int len = sprintf(text, "%d", code);
                atCmdWrite(engine, text, len);
            }
            else
            {
                const char *text = _cmeCodeText(code);
                atCmdWrite(engine, text, strlen(text));
            }
            atCmdWrite(engine, crlf, 2);
        }
        atCmdWriteFlush(engine);
    }

    atCmdCommandFinished(engine);
}

// =============================================================================
// atCmdRespCmsError
// =============================================================================
void atCmdRespCmsError(atCmdEngine_t *engine, int code)
{
    OSI_LOGI(0x10000076, "AT CMD%d CMS error code=%d", atCmdChannelIndex(engine), code);

    atCmdFinalHandle(engine, false);

    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
    uint8_t atq = chsetting->atq;
    if (atq == 0)
    {
        uint8_t cmee = chsetting->cmee;
        if (cmee == 0)
        {
            atCmdRespOutputResultCode(engine, CMD_RC_ERROR);
        }
        else
        {
            // not affected by ATV
            atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
            uint8_t crlf[2] = {chsetting->s3, chsetting->s4};
            atCmdWrite(engine, crlf, 2);
            atCmdWrite(engine, "+CMS ERROR: ", strlen("+CMS ERROR: "));
            if (cmee == 1)
            {
                char text[16];
                int len = sprintf(text, "%d", code);
                atCmdWrite(engine, text, len);
            }
            else
            {
                const char *text = _cmsCodeText(code);
                atCmdWrite(engine, text, strlen(text));
            }
            atCmdWrite(engine, crlf, 2);
        }
        atCmdWriteFlush(engine);
    }

    atCmdCommandFinished(engine);
}

// =============================================================================
// atCmdRespIntermText
// =============================================================================
void atCmdRespIntermText(atCmdEngine_t *engine, const char *text)
{
    OSI_LOGXI(OSI_LOGPAR_IS, 0x10000077, "AT CMD%d interm: %s",
              atCmdChannelIndex(engine), text);
    atCmdWrite(engine, text, strlen(text));
    atCmdWriteFlush(engine);
}

// =============================================================================
// atCmdRespOutputText
// =============================================================================
void atCmdRespOutputText(atCmdEngine_t *engine, const char *text)
{
    OSI_LOGXI(OSI_LOGPAR_IS, 0x1000007a, "AT CMD%d output: %s", atCmdChannelIndex(engine), text);
    atCmdWrite(engine, text, strlen(text));
    atCmdWriteFlush(engine);
}

// =============================================================================
// atCmdRespOutputNText
// =============================================================================
void atCmdRespOutputNText(atCmdEngine_t *engine, const char *text, size_t length)
{
    if (text == NULL || length == 0)
        return;

    OSI_LOGI(0x1000007b, "AT CMD%d output length %d", atCmdChannelIndex(engine), length);
    atCmdWrite(engine, text, length);
    atCmdWriteFlush(engine);
}

// =============================================================================
// atCmdRespOutputPrompt
// =============================================================================
void atCmdRespOutputPrompt(atCmdEngine_t *engine)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
#ifdef __QUEC_OEM_VER_LD__
    uint8_t rsp[5] = {chsetting->s3, chsetting->s4, '>', chsetting->s3, chsetting->s4};
    atCmdWrite(engine, rsp, 5);
#else
    uint8_t rsp[4] = {chsetting->s3, chsetting->s4, '>', ' '};
    atCmdWrite(engine, rsp, 4);
#endif
    atCmdWriteFlush(engine);
}

// =============================================================================
// atCmdRingInd
// =============================================================================
void atCmdRingInd(uint8_t sim)
{
#ifdef CONFIG_QUEC_PROJECT_FEATURE
	char cring[] = "+CRING: VOICE";
#else
    static const char cring[] = "+CRING: <voice>";
#endif
    bool bCheckSim = existSameSimIDChannel(sim);

    osiSlistHead_t *list = atDispatchGetList();
    osiSlistItem_t *item;
    SLIST_FOREACH(item, list, iter)
    {
        atDispatch_t *dispatch = (atDispatch_t *)item;
        if (dispatch != NULL && atDispatchIsCmdMode(dispatch) && atDispatchGetDeviceReadyStatus(dispatch))
        {
            atCmdEngine_t *cmd_engine = atDispatchGetCmdEngine(dispatch);
            if (bCheckSim && (sim != atCmdGetSim(cmd_engine)))
                continue;

            atChannelSetting_t *chsetting = atCmdChannelSetting(cmd_engine);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
			if (chsetting->crc)
			{
				quec_urc_send(NULL, cring, strlen(cring), URC_OUT_METHOD_UNICAST, RI_CATEG_RING, 0);
			}
			else if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_RING_IND))
			{
				char *s = (char *)_resultCodeText(CMD_RC_RING);
				quec_urc_send(NULL, s, strlen(s), URC_OUT_METHOD_UNICAST, RI_CATEG_RING, 0);
			}
			return;
#else
            if (chsetting->crc)
                atCmdRespUrcText(cmd_engine, cring);
            else
                atCmdRespUrcCode(cmd_engine, CMD_RC_RING);
#endif
        }
    }

    // TODO: other V.24 signals
}

const char *_getCCresultCodeText(int code)
{
    return _getCodeText(code, gAtResultText, OSI_ARRAY_SIZE(gAtResultText));
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE
void quec_atCmdResp(atCmdEngine_t *engine, _AtciResultCode resultCode, uint report_code)
{
	switch (resultCode)
	{
		case ATCI_RESULT_CODE_CME_ERROR:
		{
			atCmdRespCmeError(engine, report_code);
			break;
		}
		
		case ATCI_RESULT_CODE_CMS_ERROR:
		{
			atCmdRespCmsError(engine, report_code);
			break;
		}
		case ATCI_RESULT_CODE_OK:
		{
			atCmdRespOK(engine);
			break;
		}
		case ATCI_RESULT_CODE_ERROR:
		default:
		{
			atCmdRespError(engine);
			break;
		}
	}
	quec_urc_flush();
}

//length: the min is 2
//padding   :   if need add \r\n at the begin and/or end of the string, the begin padding is effected by atv
//		0: no add, 1: auto detect and add, 2: at the begin and end, 3: at the begin, 4: at the end
//		and 1 is default.
int quec_atCmdRespInfoNText(atCmdEngine_t *engine, const char *text, size_t length, unsigned char padding)
{
	char *buffer;
    atChannelSetting_t *chsetting = atCmdChannelSetting(engine);	 
	uint8_t crlf[3] = {chsetting->s3, chsetting->s4, 0};
    uint8_t atv = chsetting->atv;
	
	if (text != NULL)
	{
        quec_atresp_log("AT CMD%d info text len=%d: %s", atCmdChannelIndex(engine), length, text);
	}
    else
    {
        quec_atresp_log("AT CMD%d info text len=%d: (null)", atCmdChannelIndex(engine), length);
    }

    if(length < 2)
    {
        return -1;
    }
	
	buffer = (char *)malloc(length+8);
	if (buffer == NULL)
	{
		return -1;
	}

    
	memset(buffer, 0, length+8);

	switch (padding)
	{
		case 2:
			if (atCmdIsFirstInfoText(engine) && atv == 1)
			{
				strncat(buffer, (const char *)crlf, 2);
			}
		case 4:
			strncat(buffer, text, length);
			strncat(buffer, (const char *)crlf, 2);
			break;
		case 3:
			if (atCmdIsFirstInfoText(engine) && atv == 1)
			{
				strncat(buffer, (const char *)crlf, 2);
			}
		case 0:
			strncat(buffer, text, length);
			break;
		case 1:
		default:
			if (atCmdIsFirstInfoText(engine) && atv == 1)
			{
				if ((text[0]!=crlf[0]) || (text[1]!=crlf[1]))
				{
					strncat(buffer, (const char *)crlf, 2);
				}
			}
			strncat(buffer, text, length);
			if ((text[length-2]!=crlf[0]) || (text[length-1]!=crlf[1]))
			{
				strncat(buffer, (const char *)crlf, 2);
			}
			break;
	}
	
	length = strlen(buffer);
    atCmdSetFirstInfoText(engine, false);
    if ((length > 0) && (text != NULL))
    {
        atCmdWrite(engine, buffer, length);
    }
	free(buffer);
	
    atCmdWriteFlush(engine);
	return 0;
}

/**********************************************************************************************************************
    resultCode:   ATCI_RESULT_CODE_OK  ATCI_RESULT_CODE_ERROR  ATCI_RESULT_CODE_CME_ERROR  ATCI_RESULT_CODE_CMS_ERROR
    report_code:  CMD_RC_OK,  CMD_RC_ERROR, CMD_RC_CONNECT, CMD_RC_BUSY ......
	rsp_buffer:   the buffer to be send out , if no buffer to send , input NULL
	padding   :   if need add \r\n at the begin and/or end of the string, the begin padding is effected by atv
		0: no add, 1: auto detect and add, 2: at the begin and end, 3: at the begin, 4: at the end
		and 1 is default
***********************************************************************************************************************/
void quec_atResp(atCmdEngine_t *engine, _AtciResultCode resultCode, uint report_code, char *rsp_buffer , unsigned char padding)
{
	if (ATCI_RESULT_CODE_OK == resultCode)
	{
		if(rsp_buffer != NULL)
		{
			quec_atCmdRespInfoNText(engine, rsp_buffer, strlen(rsp_buffer),padding);       
		}
	}
	
	quec_atCmdResp(engine, resultCode, report_code);
}

#endif

