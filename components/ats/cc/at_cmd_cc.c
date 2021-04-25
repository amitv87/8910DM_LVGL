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

#include "osi_log.h"
#include "osi_api.h"
#include "at_command.h"
#include "at_response.h"
#include "at_cfw.h"
#include "at_cfg.h"
#include "cfw_event.h"
#include "cfw_chset.h"
#include "at_cmd_ss.h"
#include "cfw_errorcode.h"
#include "audio_device.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "net_config.h"
#include "ppp_interface.h"

#include "quec_modem_interface.h"
#include "quec_urc.h"
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
#include "ql_api_voice_call.h"
#include "quec_atcmd_call.h"
#endif


#define AT_CMD_D_ParaMaxLen 41
#define AT_CMD_D_OPTYPE_ERR -1
#define AT_CMD_D_OPTYPE_VOICECALL 1
#define AT_CMD_D_OPTYPE_GPRSDIAL 2
#define AT_CMD_D_OPTYPE_NOTSUPPORT 100
#define AT_CMD_CDU_ParaMinLen 4

#define AT_GPRS_DIALSTR1 "T*98"
#define AT_GPRS_DIALSTR2 "T*99"
#define AT_GPRS_DIALSTR3 "t*98"
#define AT_GPRS_DIALSTR4 "t*99"
#define AT_GPRS_DIALSTR5 "*98"
#define AT_GPRS_DIALSTR6 "*99"

#define AT_RING_ELAPSE_MS 6000
#define ATD_EXTENSION_FIRST_DELAY_MS 2000
#define ATD_EXTENSION_INTERVAL_MS 100

extern uint8_t g_Pin1PukStauts[];
uint8_t g_cc_CHLD = 0; // this is used to record whether the csw sent one or two rsp to AT.
uint8_t gAudioOnFlag = 0;
uint8_t gCcCallHoldEmcCallOn = 0;

extern uint32_t CFW_CcReleaseCallX(uint8_t nIndex, CFW_SIM_ID nSimId);
extern uint32_t CFW_CcReleaseCallX_V2(uint16_t nUTI, uint8_t nIndex, uint8_t cause, CFW_SIM_ID nSimId);

extern void qtonedet_reset(void);

extern uint8_t AT_GprsGetActivePdpCount(uint8_t nSim);

static int generateCeerRCCode(uint32_t cause);

static void prvReportSounder(CFW_SIM_ID sim, uint8_t type)
{
    char rsp[32] = {0};
    sprintf(rsp, "+CIEV: \"SOUNDER\",%d", type);
    atCmdRespSimUrcText(sim, rsp);
}

static void prvAlertStop(void)
{
    gAtCfwCtx.cc.alert_flag = false;
    osiTimerStop(gAtCfwCtx.cc.alert_timer);
}

#ifdef CFW_VOLTE_SUPPORT
CFW_NW_EMC_NUM_LIST_INFO gAtNwEmcNumList;

void atPlmnArray2Api(uint8_t *nOperatorId, uint8_t *pLAI)
{
    pLAI[0] = (nOperatorId[0] & 0x0F) + ((nOperatorId[1] << 4) & 0xF0);
    pLAI[1] = (nOperatorId[2] & 0x0F) + ((nOperatorId[5] << 4) & 0xF0);
    pLAI[2] = (nOperatorId[3] & 0x0F) + ((nOperatorId[4] << 4) & 0xF0);
}

static bool AT_CC_IsNwEmcNum(uint8_t *pBcd, uint8_t nBcdLen, CFW_SIM_ID sim)
{
    uint8_t OperID[6] = {0};
    uint8_t nMode = 0;
    uint32_t ret = ERR_SUCCESS;
    uint8_t Plmn[3] = {0};
    uint8_t i = 0;
    uint8_t nLen = 0;

    if ((ret = CFW_NwGetCurrentOperator(OperID, &nMode, sim)) != 0)
        return false;

    atPlmnArray2Api(OperID, Plmn);

    if (memcmp(Plmn, gAtNwEmcNumList.plmn, 3) != 0)
        return false;

    for (i = 0; i < gAtNwEmcNumList.lenOfNumList;)
    {
        nLen = gAtNwEmcNumList.listContent[i];
        if (nLen != nBcdLen + 1)
        {
            i += (nLen + 1);
            continue;
        }

        if (memcmp(&(gAtNwEmcNumList.listContent[i + 2]), pBcd, nBcdLen) == 0)
            return true;

        i += (nLen + 1);
    }

    return false;
}
#endif

//
// 该函数会去掉最后的F，同时将A转化为�?’将B转化为�?’将C转化为‘p�?
// --> 13811189836
//
uint16_t CSW_ATCcGsmBcdToAscii(uint8_t *pBcd,                    // input
                               uint8_t nBcdLen, uint8_t *pNumber // output
)
{
    uint8_t i;
    uint8_t *pTmp = pNumber;
    uint16_t nSize = 0; // nBcdLen*2;

    OSI_LOGI(0x10004490, "AT+CSW_ATCcGsmBcdToAscii:   nBcdLen = 0x%x, \n", nBcdLen);

    for (i = 0; i < nBcdLen; i++)
    {
        OSI_LOGI(0x10004491, "AT+CSW_ATCcGsmBcdToAscii:   pBcd[%d] = 0x%x, \n", i, pBcd[i]);

        if (pBcd[i] == 0xff)
            break;

        if ((pBcd[i] & 0x0F) == 0x0A)
        {
            *pTmp = '*';
        }
        else if ((pBcd[i] & 0x0F) == 0x0B)
        {
            *pTmp = '#';
        }
        else if ((pBcd[i] & 0x0F) == 0x0C)
        {
            *pTmp = 'p';
        }
        else if ((pBcd[i] & 0x0f) <= 9)
        {
            *pTmp = (pBcd[i] & 0x0F) + 0x30;
        }
        else
        {
            nSize = 1;
            *pTmp = 0;
            break;
        }

        pTmp++;

        if ((pBcd[i]) >> 4 == 0x0A)
        {
            *pTmp = '*';
        }
        else if ((pBcd[i]) >> 4 == 0x0B)
        {
            *pTmp = '#';
        }
        else if ((pBcd[i]) >> 4 == 0x0C)
        {
            *pTmp = 'p';
        }
        else if ((pBcd[i] >> 4) <= 9)
        {
            *pTmp = (pBcd[i] >> 4) + 0x30;
        }
        else
        {
            nSize = 1;
            *pTmp = 0;
            break;
        }

        pTmp++;
    }

    OSI_LOGI(0x10004492, "AT+CSW_ATCcGsmBcdToAscii: i=%d  nSize = 0x%x, \n", i, nSize);

    // [hameina[mod]2007.12.6 for bug 7166
    // nSize=(1+i)*2 -nSize;
    nSize = i * 2 + nSize;

    // ]]hameina[mod]2007.12.6 for bug 7166
    OSI_LOGI(0x10004492, "AT+CSW_ATCcGsmBcdToAscii: i=%d  nSize = 0x%x, \n", i, nSize);
    return nSize;
}

//
// GSMBCD�?如果转化奇数个数字的话，将会在对应高位补F
// 支持'*''#''p'
// 13811189836 --> 0x31 0x18 0x11 0x98 0x38 0xF6
//
uint16_t CSW_ATCcAsciiToGsmBcd(int8_t *pNumber,                  // input
                               uint8_t nNumberLen, uint8_t *pBCD // output should >= nNumberLen/2+1
)
{
    uint8_t Tmp;
    uint32_t i;
    uint32_t nBcdSize = 0;
    uint8_t *pTmp = pBCD;

    if (pNumber == (const int8_t *)NULL || pBCD == (uint8_t *)NULL)
        return false;

    memset(pBCD, 0, nNumberLen >> 1);

    for (i = 0; i < nNumberLen; i++)
    {
        switch (*pNumber)
        {

        case '*':
            Tmp = (int8_t)0x0A;
            break;

        case '#':
            Tmp = (int8_t)0x0B;
            break;

        case 'p':
            Tmp = (int8_t)0x0C;
            break;

        default:
            Tmp = (int8_t)(*pNumber - '0');
            break;
        }

        if (i % 2)
        {
            *pTmp++ |= (Tmp << 4) & 0xF0;
        }
        else
        {
            *pTmp = (int8_t)(Tmp & 0x0F);
        }

        pNumber++;
    }

    nBcdSize = nNumberLen >> 1;

    if (i % 2)
    {
        *pTmp |= 0xF0;
        nBcdSize += 1;
    }

    return nBcdSize;
}

bool CSW_ATCcIsDTMF(int8_t iTone)
{
    if (((iTone >= '0') && (iTone <= '9')) ||
        ((iTone >= 'A') && (iTone <= 'D')) ||
        (iTone == '#') || (iTone == '*'))
        return true;

    return false;
}

uint8_t g_cc_ciev = 0; // bit0: sounder, bit1:; call in progress
bool AT_CIND_GetCC(bool *pCall, bool *pSounder)
{
    *pSounder = g_cc_ciev & 1;
    *pCall = (g_cc_ciev >> 1) & 1;
    return 1;
}

uint8_t g_RingTimes = 0;
extern bool gATSATSetupCallProcessingFlag[CONFIG_NUMBER_OF_SIM];

uint8_t AT_CC_GetCCCount(uint8_t sim)
{
    uint8_t cnt = 0;
    CFW_CC_CURRENT_CALL_INFO call_info[AT_CC_MAX_NUM];

    CFW_CcGetCurrentCall(call_info, &cnt, sim);
    OSI_LOGI(0, "SETVOLTE AT_CC_GetCCCount cnt = %d", cnt);

    return cnt;
}

/*void AT_CC_AsyncEventProcess(const osiEvent_t *pEvent)
{
#ifdef PORTING_ON_GOING
    CFW_EVENT CfwEv;
    CFW_EVENT *pEv = &CfwEv;
    uint8_t nClip = 0, nCssi = 0, nCssu = 0, nCcwa = 0;
    // uint16_t nMer = 0;
    uint8_t ind = 0;
    // uint32_t nRet = 0;
    uint8_t sim = 0;
    AT_CosEvent2CfwEvent(pEvent, pEv);

    sim = pEv->nFlag;

    // [[hameina[mod] 2008-04-14 :CSSI, CSSU的设置不起作�?
    nClip = gATCurrentucClip;
    nCssi = gATCurrentucCSSI;
    nCssu = gATCurrentucCSSU;
    nCcwa = gATCurrentucSsCallWaitingPresentResultCode;
    ind = gATCurrentuCmer_ind[sim];

    // ]]hameina[mod] 2008-04-14 :CSSI, CSSU的设置不起作�?
    OSI_LOGI(0x10004493, "AT_CC  nEventId = 0x%x, nType = 0x%x, nParam1 = 0x%x, nParam2 = 0x%x\n\r", pEvent->nEventId,
             cfw_event->nType, cfw_event->nParam1, cfw_event->nParam2);

    switch (pEvent->nEventId)
    {
    case EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP:
    {
        // ---------------------------------------------------------
        // CHLD Event
        // ---------------------------------------------------------
        if (g_cc_CHLD)
            g_cc_CHLD--;
        pEvent->nParam1 = 0;

        OSI_LOGI(0x100052bf, "EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, g_cc_CHLD =%d, cfw_event->nType =0x%x\n", g_cc_CHLD, cfw_event->nType);

        if (AT_isWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, sim, pEv->uti))
        {
            pEv->uti = AT_GetDLCIFromWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, sim);
        }

        if ((0x00 == cfw_event->nType) || (0x01 == cfw_event->nType) || (0x0F == cfw_event->nType))
        {
            if (!g_cc_CHLD)
            {
                AT_DelWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, sim, pEv->uti);
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pEv->uti, sim);
            }
            return;
        }
        else if (0x10 == cfw_event->nType)
        {
            AT_DelWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, sim, pEv->uti);
            AT_CC_Result_Err(CMD_FUNC_SUCC, CMD_RC_ERROR, pEv->uti, sim);
            return;
        }
        else
        {
            //AT_CC_Result_Err(cfw_event->nParam1, CMD_ERROR_CODE_TYPE_CME, pEv->uti, sim);
            AT_DelWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, sim, pEv->uti);
            AT_CC_Result_Err(0, CMD_ERROR_CODE_TYPE_CME, pEv->uti, sim);
            return;
        }

        break;
    }

    default:
        //AT_CC_Result_Err(cfw_event->nParam1, CMD_ERROR_CODE_TYPE_CME, pEv->uti, sim);
        AT_CC_Result_Err(0, CMD_ERROR_CODE_TYPE_CME, pEv->uti, sim);
        break;
    }

    return;
#endif
}*/

static bool _checkAutoAnswer(uint8_t sim)
{
    if (gAtSetting.sim[sim].s0 != 0 &&
        gAtCfwCtx.sim[sim].ring_count >= gAtSetting.sim[sim].s0)
    {
#ifdef CONFIG_SOC_8910
        audevStopTone();
        if (!audevStartVoice())
            return false;
#endif
        CFW_CcAcceptSpeechCall(sim);
        return true;
    }
    return false;
}

static void _ringTimeout(void *param)
{
    uint8_t sim = (uint32_t)param;
    uint32_t cc_status = CFW_CcGetCallStatus(sim);

    if (cc_status == CC_STATE_INCOMING || cc_status == CC_STATE_WAITING)
    {
        gAtCfwCtx.sim[sim].ring_count++;
        osiTimerStop(gAtCfwCtx.sim[sim].ring_timer);
        if (!_checkAutoAnswer(sim))
        {
            osiTimerStart(gAtCfwCtx.sim[sim].ring_timer, AT_RING_ELAPSE_MS);
            atCmdRingInd(sim);
#ifdef CONFIG_SOC_8910
            if (gAtSetting.callmode == 0)
            {
                audevPlayTone(AUDEV_TONE_DIAL, 500);
            }
#endif
        }
    }
}

typedef uint32_t (*PFN_AT_CC_CB)(const osiEvent_t *event);

PFN_AT_CC_CB pAT_CC_SPEECH_CALL_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_RELEASE_CALL_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_PROGRESS_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_ERROR_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_CALL_INFO_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_CRSSINFO_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_AUDIOON_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_ALERT_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_ACCEPT_SPEECH_CALL_RSP_CB = NULL;
PFN_AT_CC_CB pAT_CC_CALL_PATH_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_CALL_TI_ASSIGNED_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_AUDIO_RESTART_IND_CB = NULL;
PFN_AT_CC_CB pAT_CC_INITIATE_SPEECH_CALL_RSP_CB = NULL;
PFN_AT_CC_CB pAT_CC_RELEASE_CALL_RSP_CB = NULL;
PFN_AT_CC_CB pAT_CC_CALL_HOLD_MULTIPARTY_RSP_CB = NULL;

#ifndef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
#define URI_MAX_NUMBER 21
#endif

#define SIP_STR "sip:"
#define min(a, b) ((a) < (b) ? (a) : (b))

bool atGetUriCallNumber(
    const char *uri_ptr,            //[IN]
    char uriNum[URI_MAX_NUMBER + 1] //[OUT]
)
{
    const char *pURINumStart = NULL;
    const char *pURINumEnd = NULL;
    const char *pURIHostStart = NULL;
    const char *pURINumSemic = NULL; //semicolon

    if (uri_ptr == NULL || uriNum == NULL)
    {
        return false;
    }

    pURINumStart = strstr(uri_ptr, SIP_STR);
    if (pURINumStart != NULL)
    {
        pURINumStart += strlen(SIP_STR);
    }
    else
    {
        return false;
    }

    //get host string start pointer
    pURIHostStart = strstr(uri_ptr, "@");
    pURINumSemic = strstr(uri_ptr, ";");
    if (NULL != pURINumSemic)
    {
        if (NULL == pURIHostStart)
        {
            pURINumEnd = pURINumSemic;
        }
        else if (0 > (pURINumSemic - pURIHostStart))
        {
            pURINumEnd = pURINumSemic;
        }
        else
        {
            pURINumEnd = pURIHostStart;
        }
    }
    else if (NULL != pURIHostStart)
    {
        pURINumEnd = pURIHostStart;
    }
    else
    {
        return false;
    }

    if (pURINumEnd != pURINumStart)
    {
        strncpy(
            uriNum,
            pURINumStart,
            min(URI_MAX_NUMBER, pURINumEnd - pURINumStart));
    }

    return false;
}

#if defined(CONFIG_QL_OPEN_EXPORT_PKG) && defined(CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL)
uint32_t* _rcCodeConvert2Ql(uint32_t rc_code)
{
	static uint32_t ql_code = 0;
    switch (rc_code)
    {
    case CFW_CC_CAUSE_NOUSED_CAUSE:
    case CFW_CC_CAUSE_USER_ALERTING_NO_ANSWER:
    case CFW_CC_CAUSE_RECOVERY_ON_TIMER_EXPIRY:
    case CFW_CC_CAUSE_DESTINATION_OUT_OF_ORDER:
	case CFW_CC_CAUSE_NO_USER_RESPONDING:
        ql_code = QL_VC_REASON_NOANSWER;
        break;
    case CFW_CC_CAUSE_NORMAL_CALL_CLEARING:
        ql_code = QL_VC_REASON_NOCARRIER;
        break;
	case CFW_CC_CAUSE_CHANNEL_UNAVAILABLE:
    case CFW_CC_CAUSE_USER_BUSY:
        ql_code = QL_VC_REASON_BUSY;
        break;
    case CFW_CC_CAUSE_NORMAL_UNSPECIFIED:
		ql_code = QL_VC_REASON_PWROFF;
		break;
    default:
        ql_code = QL_VC_REASON_ERROR;
        break;
    }

	return &ql_code;
}
#endif

static void _onEV_CFW_CC_SPEECH_CALL_IND(const osiEvent_t *event)
{
    if (pAT_CC_SPEECH_CALL_IND_CB)
        (*pAT_CC_SPEECH_CALL_IND_CB)(event);

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType != 0)
        return; // shouldn't exist

    audevPlayTone(AUDEV_TONE_DIAL, 500);

    uint8_t sim = cfw_event->nFlag;
    CFW_SPEECH_CALL_IND *call_info = (CFW_SPEECH_CALL_IND *)cfw_event->nParam1;
    atMemFreeLater(call_info);
	
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
	if(q_atcmd_par.ats7 != 0)
	{
		if(CFW_CcGetCallStatus(sim) == CC_STATE_INCOMING)
		{
			if( Qats7Timer == NULL)
			{
				ql_rtos_timer_create(&Qats7Timer, (ql_task_t)atEngineGetThreadId(), QATS7_TimmerHandler, NULL);
				if(Qats7Timer != NULL)
				{
					ql_rtos_timer_start(Qats7Timer, q_atcmd_par.ats7*1000, 0);
				}
			}
			else
			{
				if(osiTimerIsRunning(Qats7Timer) == true)
				{
					ql_rtos_timer_stop(Qats7Timer);
					ql_rtos_timer_delete(Qats7Timer);
					Qats7Timer = NULL;
					ql_rtos_timer_create(&Qats7Timer, atEngineGetThreadId(), QATS7_TimmerHandler, NULL);
					if(Qats7Timer != NULL)
					{
						ql_rtos_timer_start(Qats7Timer, q_atcmd_par.ats7*1000, 0);
					}
				}
				else
				{
					
					ql_rtos_timer_start(Qats7Timer, q_atcmd_par.ats7*1000, 0);
				}
			}
		}
	}
	
#endif
	
    gAtCfwCtx.sim[sim].ring_count = 0;

#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
	if(!(gAtSetting.sim[sim].ccwa && (call_info->nDisplayType == 4 ||call_info->nDisplayType == 5 || call_info->nDisplayType == 6)))
#endif
	{
		atCmdRingInd(sim);
	}

    char num_str[TEL_NUMBER_MAX_LEN + 2] = {
        0x00,
    };

    if (call_info->TelNumber.nSize)
    {
        cfwBcdToDialString(call_info->TelNumber.nTelNumber, call_info->TelNumber.nSize, (char *)num_str);
    }
    else
    {
        OSI_LOGXI(OSI_LOGPAR_SI, 0, "_onEV_CFW_CC_SPEECH_CALL_IND call_info->calling_uri = %s", call_info->calling_uri);
        atGetUriCallNumber((const char *)call_info->calling_uri, num_str);
        if ((num_str[0] == '+') ||
            ((num_str[0] == '0') && (num_str[1] == '0')))
        {
            call_info->TelNumber.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
        }
        else
        {
            call_info->TelNumber.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
        }
    }
    OSI_LOGXI(OSI_LOGPAR_SI, 0, "_onEV_CFW_CC_SPEECH_CALL_IND num_str = %s", num_str);

    // char *num_str = alloca(call_info->TelNumber.nSize * 2 + 1);
    // cfwBcdToDialString(call_info->TelNumber.nTelNumber, call_info->TelNumber.nSize, num_str);

    // 1: CLIP
    // 2: CSSI
    // 3: CSSU
    // 4: CCWA
    // 5: CSSI+CCWA
    // 6: CSSU+CCWA
    // 7: CSSI+CLIP
    // 8: CSSU+CLIP

    char rsp[128];
    bool show_clip = gAtSetting.sim[sim].clip &&
                     (call_info->nDisplayType == 1 ||
                      call_info->nDisplayType == 7 ||
                      call_info->nDisplayType == 8);
    bool show_cssi = gAtSetting.sim[sim].cssi &&
                     (call_info->nCode != 0xff) &&
                     (call_info->nDisplayType == 2 ||
                      call_info->nDisplayType == 5 ||
                      call_info->nDisplayType == 7);
    bool show_cssu = gAtSetting.sim[sim].cssu &&
                     (call_info->nCode != 0xff) &&
                     (call_info->nDisplayType == 3 ||
                      call_info->nDisplayType == 6 ||
                      call_info->nDisplayType == 8);
    bool show_ccwa = gAtSetting.sim[sim].ccwa &&
                     (call_info->nDisplayType == 4 ||
                      call_info->nDisplayType == 5 ||
                      call_info->nDisplayType == 6);

    if (show_cssi)
    {
        sprintf(rsp, "+CSSI: %d", call_info->nCode);
        atCmdRespSimUrcText(sim, rsp);
    }
    if (show_cssu)
    {
        sprintf(rsp, "+CSSU: %d", call_info->nCode);
        atCmdRespSimUrcText(sim, rsp);
    }
    if (show_ccwa)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
		sprintf(rsp, "+CCWA: \"%s\",%d,1", num_str, call_info->TelNumber.nType);
        quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, 0);
#else
		sprintf(rsp, "+CCWA: \"%s\",%d,1,,%d", num_str, call_info->TelNumber.nType,call_info->nCLIValidity);
		atCmdRespSimUrcText(sim, rsp);
#endif
#if defined(CONFIG_QL_OPEN_EXPORT_PKG) && defined(CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL)
		if(ql_voice_call_callback)
			ql_voice_call_callback(QL_VC_CCWA_IND, rsp);
#endif
    }
    if (show_clip)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
		sprintf(rsp, "+CLIP: \"%s\",%d,"",0,"",%d", num_str, call_info->TelNumber.nType,call_info->nCLIValidity);
		quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, 0);
#else
		sprintf(rsp, "+CLIP: \"%s\",%d,,,,%d", num_str, call_info->TelNumber.nType,call_info->nCLIValidity);
        atCmdRespSimUrcText(sim, rsp);
#endif
    }
	
#if defined(CONFIG_QL_OPEN_EXPORT_PKG) && defined(CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL)
	if(ql_voice_call_callback)
		ql_voice_call_callback(QL_VC_RING_IND, num_str);
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		uint8_t status = QUEC_CALL_INCOMING;
		if(call_info->nDisplayType == 4 || call_info->nDisplayType == 5 || call_info->nDisplayType == 6){
			status = QUEC_CALL_WAITING;
		}
		uint8_t index = (call_info->nIndex - 1 > 0) ? (call_info->nIndex - 1 >= 0) : 0;
		quec_dsci_urc(sim, index, status);
#endif
    osiTimerStop(gAtCfwCtx.sim[sim].ring_timer);
    if (!_checkAutoAnswer(sim))
        osiTimerStart(gAtCfwCtx.sim[sim].ring_timer, AT_RING_ELAPSE_MS);
}

static void _onEV_CFW_CC_RELEASE_CALL_IND(const osiEvent_t *event)
{
    if (pAT_CC_RELEASE_CALL_IND_CB)
        (*pAT_CC_RELEASE_CALL_IND_CB)(event);
	
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType != 0)
        return; // shouldn't exist

    // TODO: audio and RI
    uint8_t sim = cfw_event->nFlag;
    uint8_t cnt = AT_CC_GetCCCount(sim);
    OSI_LOGI(0, "release call rsp, count/%d", cnt);
    if (cnt == 0)
    {
        prvReportSounder(sim, 0);
        audevStopVoice();
    }

    prvAlertStop();
    OSI_LOGI(0, "release call rsp, cfw_event->nParam2: %d", (uint32_t)cfw_event->nParam2);
    OSI_LOGI(0, "release call rsp, cfw_event->nParam1: %d", (uint32_t)cfw_event->nParam1);
    int rc_code = generateCeerRCCode(cfw_event->nParam2);
    
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		int8_t index = (cfw_event->nParam1 - 1 >= 0) ? (cfw_event->nParam1 - 1) : 0;
		quec_dsci_urc(sim, index, QUEC_CALL_END);
#endif
#if defined(CONFIG_QL_OPEN_EXPORT_PKG) && defined(CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL)
	if(ql_voice_call_callback)
		ql_voice_call_callback(QL_VC_NOCARRIER_IND, _rcCodeConvert2Ql(cfw_event->nParam2));
#endif
    atCfwCcSetOffline();
    atCmdRespSimUrcCode(sim, rc_code);
}

static void _onEV_CFW_CC_PROGRESS_IND(const osiEvent_t *event)
{
    if (pAT_CC_PROGRESS_IND_CB)
        (*pAT_CC_PROGRESS_IND_CB)(event);

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t sim = cfw_event->nFlag;

    char rsp[32];
    if (1 == cfw_event->nType) // call in progress
    {
        sprintf(rsp, "+CIEV: \"CALL\",%d", (int)cfw_event->nParam1);
        atCmdRespSimUrcText(sim, rsp);
    }

#ifdef PORTING_ON_GOING
    extern bool gATSATSetupCallFlag[];
    // [[hameina[+]2007-10-30 for CIND
    if (0 == cfw_event->nType) // set sounder
    {
        g_cc_ciev = (g_cc_ciev & 0xfe) | cfw_event->nParam1;
        if (gATSATSetupCallFlag[pEv->nFlag])
        {
            if (false == gATSATSetupCallProcessingFlag[pEv->nFlag])
            {
                gATSATSetupCallProcessingFlag[pEv->nFlag] = true;
                CFW_SatResponse(0x10, 0x00, 0x00, NULL, 0x00, 12, pEv->nFlag);
            }
            //gATSATSetupCallFlag[pEv->nFlag] = false;
        }
    }

    else if (1 == cfw_event->nType) // call in progress
    {
        g_cc_ciev = (g_cc_ciev & 0xfd) | (cfw_event->nParam1 << 1);
        if ((gATSATSetupCallFlag[pEv->nFlag]) && (cfw_event->nParam1 == 0))
        {
            if (false == gATSATSetupCallProcessingFlag[pEv->nFlag])
            {
                gATSATSetupCallProcessingFlag[pEv->nFlag] = true;
                CFW_SatResponse(0x10, 0x35, 0x00, NULL, 0x00, 12, pEv->nFlag);
            }

            // gATSATSetupCallFlag[pEv->nFlag] = false;
        }
    }
    else
    {
        if (gATSATSetupCallFlag[pEv->nFlag])
        {
            CFW_SatResponse(0x10, 0x35, 0x00, NULL, 0x00, 12, pEv->nFlag);
            // gATSATSetupCallFlag[pEv->nFlag] = false;
        }
    }

    pEvent->nParam1 = 0;
    // ]]hameina[+]2007-10-30 for CIND
    return;
#endif
}

static void _onEV_CFW_CC_ERROR_IND(const osiEvent_t *event)
{
    if (pAT_CC_ERROR_IND_CB)
        (*pAT_CC_ERROR_IND_CB)(event);

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType != 0xf0 && cfw_event->nType != 0x10)
        return; // shouldn't exist

    prvAlertStop();
    if (cfw_event->nType == 0xf0)
    {

        uint8_t sim = cfw_event->nFlag;
        uint8_t cnt = AT_CC_GetCCCount(sim);
        OSI_LOGI(0, "_onEV_CFW_CC_ERROR_IND release call rsp, count/%d", cnt);
        if (cnt == 0)
        {
            audevStopVoice();
            prvReportSounder(sim, 0);
        }
        int rc_code;
        OSI_LOGI(0, "_onEV_CFW_CC_ERROR_IND cause value = %d\n", cfw_event->nParam1);
        if (CFW_CC_CAUSE_USER_BUSY == cfw_event->nParam1)
            rc_code = CMD_RC_BUSY;
        else if (CFW_CC_CAUSE_NO_USER_RESPONDING == cfw_event->nParam1)
            rc_code = CMD_RC_NODIALTONE;
        else if (CFW_CC_CAUSE_USER_ALERTING_NO_ANSWER == cfw_event->nParam1)
            rc_code = CMD_RC_NOANSWER;
        else
            rc_code = CMD_RC_NOCARRIER;

        atCfwCcSetOffline();
        atCmdRespSimUrcCode(sim, rc_code);
    }
    else
        OSI_LOGI(0, "_onEV_CFW_CC_ERROR_IND CC local error %d", cfw_event->nParam2);
}

static void _onEV_CFW_CC_CALL_INFO_IND(const osiEvent_t *event)
{
    if (pAT_CC_CALL_INFO_IND_CB)
        (*pAT_CC_CALL_INFO_IND_CB)(event);

    _onEV_CFW_CC_PROGRESS_IND(event);
}

static void _onEV_CFW_CC_CRSSINFO_IND(const osiEvent_t *event)
{
    if (pAT_CC_CRSSINFO_IND_CB)
        (*pAT_CC_CRSSINFO_IND_CB)(event);

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t sim = cfw_event->nFlag;

    if (0x11 == cfw_event->nType)
    {
        char rsp[32];

        if ((1 == gAtSetting.sim[sim].cssi) && (cfw_event->nParam1 != 0xff))
        {
            sprintf(rsp, "+CSSI: %d", (int)cfw_event->nParam1);
            atCmdRespSimUrcText(sim, rsp);
        }

        if ((1 == gAtSetting.sim[sim].cssu) && (0xff != cfw_event->nParam2))
        {
            sprintf(rsp, "+CSSU: %d", (int)cfw_event->nParam2);
            atCmdRespSimUrcText(sim, rsp);
        }
    }
}

static void _onEV_CFW_CC_AUDIOON_IND(const osiEvent_t *event)
{
    if (pAT_CC_AUDIOON_IND_CB)
        (*pAT_CC_AUDIOON_IND_CB)(event);

    OSI_LOGI(0, "_onEV_CFW_CC_AUDIOON_IND event");
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t sim = cfw_event->nFlag;
    uint8_t index = cfw_event->nParam1;
    uint8_t cnt = 0;
    CFW_CC_CURRENT_CALL_INFO call_info[AT_CC_MAX_NUM];

    gAudioOnFlag = 1;
    CFW_CcGetCurrentCall(call_info, &cnt, sim);
    OSI_LOGI(0, "EV_CFW_CC_AUDIOON_IND:gAtCfwCtx.cc.alert_flag=%d", gAtCfwCtx.cc.alert_flag);
    OSI_LOGI(0, "EV_CFW_CC_AUDIOON_IND:status=%d", call_info[index].status);
    if ((!gAtCfwCtx.cc.alert_flag) &&
        (CFW_CM_STATUS_INCOMING == call_info[index].status || CFW_CM_STATUS_WAITING == call_info[index].status))
    {
        audevPlayTone(AUDEV_TONE_DIAL, 1000);
    }
    if (!gAtCfwCtx.cc.alert_flag)
        prvReportSounder(sim, 1);

    prvAlertStop();
}

static void prvAlertTimerCB(void *param)
{
    if (gAtCfwCtx.cc.alert_flag)
    {
        audevPlayTone(AUDEV_TONE_DIAL, 1000);
        osiTimerStart(gAtCfwCtx.cc.alert_timer, 5000);
    }
}

static void _onEV_CFW_CC_ALERT_IND(const osiEvent_t *event)
{
    if (pAT_CC_ALERT_IND_CB)
        (*pAT_CC_ALERT_IND_CB)(event);

    OSI_LOGI(0, "_onEV_CFW_CC_ALERT_IND event");
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t sim = cfw_event->nFlag;
    uint8_t cnt = AT_CC_GetCCCount(sim);
    OSI_LOGI(0, "EV_CFW_CC_ALERT_IND gAudioOnFlag=%d", gAudioOnFlag);
    if ((gAudioOnFlag == 0) && !(cnt > 1 && !gAtCfwCtx.cc.ims_call))
    {
        prvReportSounder(sim, 1);
        gAtCfwCtx.cc.alert_flag = true;

        audevPlayTone(AUDEV_TONE_DIAL, 1000);
        osiTimerStart(gAtCfwCtx.cc.alert_timer, 5000);
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		quec_dsci_urc(sim, 0, QUEC_CALL_ALERTING);
#endif
}

static void _onEV_CFW_CC_ACCEPT_SPEECH_CALL_RSP(const osiEvent_t *event)
{
    if (pAT_CC_ACCEPT_SPEECH_CALL_RSP_CB)
        (*pAT_CC_ACCEPT_SPEECH_CALL_RSP_CB)(event);

    char rsp[32];
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t sim = cfw_event->nFlag;

    osiTimerStop(gAtCfwCtx.sim[sim].ring_timer);
    if (0x0f == cfw_event->nType) // success, and the call was connected
    {
        sprintf(rsp, "CONNECT");
#if defined(CONFIG_QL_OPEN_EXPORT_PKG) && defined(CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL)
	if(ql_voice_call_callback)
		ql_voice_call_callback(QL_VC_CONNECT_IND, NULL);
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		quec_dsci_urc(sim, 0, QUEC_CALL_CONNECT);
#endif
    }
    else
    {
        sprintf(rsp, "ERROR");

        if (0 == AT_CC_GetCCCount(sim))
        {
            audevStopVoice();
        }
        atCfwCcSetOffline();
#if defined(CONFIG_QL_OPEN_EXPORT_PKG) && defined(CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL)
		if(ql_voice_call_callback)
			ql_voice_call_callback(QL_VC_ERROR_IND, NULL);
#endif

    }
    atCmdRespSimUrcText(sim, rsp);
}

static void _onEV_CFW_CC_CALL_PATH_IND(const osiEvent_t *event)
{
    if (pAT_CC_CALL_PATH_IND_CB)
        (*pAT_CC_CALL_PATH_IND_CB)(event);

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    gAtCfwCtx.cc.ims_call = (cfw_event->nParam1 != 0);
    OSI_LOGI(0, "EV_CFW_CC_CALL_PATH_IND:gAtCfwCtx.cc.ims_call=%d", gAtCfwCtx.cc.ims_call);
}

static void _onEV_CFW_CC_CALL_TI_ASSIGNED_IND(const osiEvent_t *event)
{
    if (pAT_CC_CALL_TI_ASSIGNED_IND_CB)
        (*pAT_CC_CALL_TI_ASSIGNED_IND_CB)(event);

    gAudioOnFlag = 0;
    gAtCfwCtx.cc.alert_flag = false;
    audevStartVoice();
}

static void _onEV_CFW_CC_AUDIO_RESTART_IND(const osiEvent_t *event)
{
    audevRestartVoice();
    if (pAT_CC_AUDIO_RESTART_IND_CB)
        (*pAT_CC_AUDIO_RESTART_IND_CB)(event);
}

static void _onEV_DM_SPEECH_IND(const osiEvent_t *event)
{
}

static void _onEV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP(const osiEvent_t *event)
{
    if (gCcCallHoldEmcCallOn == 1)
    {
        audevStopVoice();
        gCcCallHoldEmcCallOn = 0;
    }
}

static void _voiceCallStateUpdate(uint8_t sim)
{
    uint8_t cnt = AT_CC_GetCCCount(sim);
    if (cnt == 0)
    {
        audevStopVoice();
        atCfwCcSetOffline();
    }
}

static void _voiceDialFinished(uint8_t sim, bool success)
{
    if (success)
    {
        if (gAtCfwCtx.cc.extension_len == 0)
            atCfwCcDailFinished();
        else
            osiTimerStart(gAtCfwCtx.cc.extension_timer, ATD_EXTENSION_FIRST_DELAY_MS);
    }
    else
    {
        gAtCfwCtx.cc.extension_len = 0;

        uint8_t cnt = AT_CC_GetCCCount(sim);
        if (cnt == 0)
            atCfwCcSetOffline();
        else
            atCfwCcDailFinished();
    }
}

static void _sendExtenstion(void *param)
{
    atCmdEngine_t *engine = atCfwDialingEngine();
    if (engine == NULL)
        return;

    if (gAtCfwCtx.cc.extension_len == 0)
        return;

    uint8_t sim = atCmdGetSim(engine);

    OSI_LOGI(0, "ATD extension timeout len/%d pos/%d",
             gAtCfwCtx.cc.extension_len, gAtCfwCtx.cc.extension_pos);

    if (gAtCfwCtx.cc.extension_pos >= gAtCfwCtx.cc.extension_len * 2)
    {
        gAtCfwCtx.cc.extension_len = 0;
        gAtCfwCtx.cc.extension_pos = 0;
        atCfwCcDailFinished();
    }
    else if ((gAtCfwCtx.cc.extension_pos % 2) == 0)
    {
        CFW_CcPlayTone(gAtCfwCtx.cc.extension[gAtCfwCtx.cc.extension_pos / 2], sim);
        osiTimerStart(gAtCfwCtx.cc.extension_timer, ATD_EXTENSION_INTERVAL_MS);
        gAtCfwCtx.cc.extension_pos++;
    }
    else
    {
        CFW_CcStopTone(sim);
        osiTimerStart(gAtCfwCtx.cc.extension_timer, ATD_EXTENSION_INTERVAL_MS);
        gAtCfwCtx.cc.extension_pos++;
    }
}

static void _onEV_CFW_CC_INITIATE_SPEECH_CALL_RSP(const osiEvent_t *event)
{
    if (pAT_CC_INITIATE_SPEECH_CALL_RSP_CB)
        (*pAT_CC_INITIATE_SPEECH_CALL_RSP_CB)(event);

    atCmdEngine_t *engine = atCfwDialingEngine();
    
#if defined(CONFIG_QL_OPEN_EXPORT_PKG) && defined(CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL)
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t sim = 0;
	prvAlertStop();
	if(engine)
		sim = atCmdGetSim(engine);
	
    OSI_LOGI(0, "_onEV_CFW_CC_INITIATE_SPEECH_CALL_RSP cfw_event->nType: %d", cfw_event->nType);
    OSI_LOGI(0, "_onEV_CFW_CC_INITIATE_SPEECH_CALL_RSP cfw_event->nParam1: %x", (uint32_t)cfw_event->nParam1);
    OSI_LOGI(0, "_onEV_CFW_CC_INITIATE_SPEECH_CALL_RSP cfw_event->nParam2: %x", (uint32_t)cfw_event->nParam2);
	if (0 == cfw_event->nType)
    {
        _voiceDialFinished(sim, true);
		
		if(ql_voice_call_callback)
			ql_voice_call_callback(QL_VC_CONNECT_IND,NULL);
    }
    else if (1 == cfw_event->nType)
    {
        CFW_TELNUMBER *telnum = (CFW_TELNUMBER *)cfw_event->nParam1;
        atMemFreeLater(telnum);
		
        char num_str[TEL_NUMBER_MAX_LEN * 2 + 1];
        cfwBcdToDialString(telnum->nTelNumber, telnum->nSize, num_str);
        _voiceDialFinished(sim, true);
		
		if(ql_voice_call_callback)
			ql_voice_call_callback(QL_VC_CONNECT_IND, num_str);
    }
    else if (0x10 == cfw_event->nType) //  time up
    {
        generateCeerRCCode(cfw_event->nParam1);
		if(engine)
		{
        	atCmdRespIntermCode(engine, CMD_RC_NOCARRIER); // ERR_AT_CME_NETWORK_TIMOUT
		}
        _voiceDialFinished(sim, false);

		if(ql_voice_call_callback)
			ql_voice_call_callback(QL_VC_NOCARRIER_IND, _rcCodeConvert2Ql(cfw_event->nParam1));
    }
    else if (0xF0 == cfw_event->nType)
    {
        int code = 0;
        audevStopVoice();
        code = generateCeerRCCode(cfw_event->nParam1);
		if(engine)
		{
        	atCmdRespIntermCode(engine, code);
		}
        _voiceDialFinished(sim, false);
		
		if(ql_voice_call_callback)
			ql_voice_call_callback(QL_VC_NOCARRIER_IND, _rcCodeConvert2Ql(cfw_event->nParam1));
    }
    else
    {
        audevStopVoice();
		if(engine)
		{
        	atCmdRespIntermCode(engine, CMD_RC_ERROR);
		}
        _voiceDialFinished(sim, false);
		
		if(ql_voice_call_callback)
			ql_voice_call_callback(QL_VC_NOCARRIER_IND, _rcCodeConvert2Ql(cfw_event->nParam1));
    }
	
#else 
	if (engine == NULL)
    {
        OSI_LOGI(0, "_onEV_CFW_CC_INITIATE_SPEECH_CALL_RSP engine is NULL !!!");
        return; // unexpected event
    }
    prvAlertStop();
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t sim = atCmdGetSim(engine);
    char rsp[64];

    OSI_LOGI(0, "_onEV_CFW_CC_INITIATE_SPEECH_CALL_RSP cfw_event->nType: %d", cfw_event->nType);
    OSI_LOGI(0, "_onEV_CFW_CC_INITIATE_SPEECH_CALL_RSP cfw_event->nParam1: %x", (uint32_t)cfw_event->nParam1);
    OSI_LOGI(0, "_onEV_CFW_CC_INITIATE_SPEECH_CALL_RSP cfw_event->nParam2: %x", (uint32_t)cfw_event->nParam2);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
	char num_str[TEL_NUMBER_MAX_LEN * 2 + 1] = {0};
	cfwBcdToDialString(g_callinfo[0].dialNumber.nTelNumber, g_callinfo[0].dialNumber.nSize, num_str);
	if(0 == cfw_event->nType || 1 == cfw_event->nType)
	{
		quec_dsci_urc(sim,0,QUEC_CALL_CONNECT);
	}
	else
	{
		quec_dsci_urc(sim, 0, QUEC_CALL_END);
	}
#endif
	
    if (0 == cfw_event->nType)
    {
#ifndef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
        atCmdRespIntermCode(engine, CMD_RC_CONNECT);
#endif
        _voiceDialFinished(sim, true);
    }
    else if (1 == cfw_event->nType)
    {
        CFW_TELNUMBER *telnum = (CFW_TELNUMBER *)cfw_event->nParam1;
        atMemFreeLater(telnum);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
		sprintf(rsp, "+COLP: \"%s\",%d", telnum->nTelNumber, telnum->nType);
		atCmdRespInfoText(engine, rsp);
#else
        char num_str[TEL_NUMBER_MAX_LEN * 2 + 1];
        cfwBcdToDialString(telnum->nTelNumber, telnum->nSize, num_str);
        sprintf(rsp, "+COLP: \"%s\",%d", num_str, telnum->nType);
        atCmdRespInfoText(engine, rsp);
        atCmdRespIntermCode(engine, CMD_RC_CONNECT);
#endif        
        _voiceDialFinished(sim, true);
    }
    else if (0x10 == cfw_event->nType) //  time up
    {
        generateCeerRCCode(cfw_event->nParam1);

        atCmdRespIntermCode(engine, CMD_RC_NOCARRIER); // ERR_AT_CME_NETWORK_TIMOUT
        _voiceDialFinished(sim, false);
    }
    else if (0xF0 == cfw_event->nType)
    {
        int code = 0;

        audevStopVoice();
        code = generateCeerRCCode(cfw_event->nParam1);
        atCmdRespIntermCode(engine, code);
        _voiceDialFinished(sim, false);
    }
    else
    {
        audevStopVoice();
        atCmdRespIntermCode(engine, CMD_RC_ERROR);
        _voiceDialFinished(sim, false);
    }
#endif
}

void atCfwCcInit(void)
{
    for (int n = 0; n < CONFIG_NUMBER_OF_SIM; n++)
    {
        gAtCfwCtx.sim[n].ring_count = 0;
        gAtCfwCtx.sim[n].ring_timer = osiTimerCreate(atEngineGetThreadId(), _ringTimeout, (void *)n);

        CFW_CfgSetSSN(gAtSetting.sim[n].cssi, gAtSetting.sim[n].cssu, n);
    }

    gAtCfwCtx.cc.extension_timer = osiTimerCreate(atEngineGetThreadId(), _sendExtenstion, NULL);
    gAtCfwCtx.cc.alert_timer = osiTimerCreate(atEngineGetThreadId(), prvAlertTimerCB, NULL);

    atEventsRegister(
        CFW_REGISTER_EVENT(EV_CFW_CC_SPEECH_CALL_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_RELEASE_CALL_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_PROGRESS_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_ERROR_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_CALL_INFO_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_CRSSINFO_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_AUDIOON_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_ALERT_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_CALL_PATH_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_CALL_TI_ASSIGNED_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_AUDIO_RESTART_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_INITIATE_SPEECH_CALL_RSP),
        CFW_REGISTER_EVENT(EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP),
        CFW_REGISTER_EVENT(EV_DM_SPEECH_IND),
        CFW_REGISTER_EVENT(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP),
        0);
}

void atCmdHandleA(atCommand_t *cmd)
{
    // CC can only run in one channel at any time
    if (!atCfwCcIsPermitted(cmd->engine))
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

    uint8_t sim = atCmdGetSim(cmd->engine);

    atCmdClearRemains(cmd->engine);

    // TODO: audio
    if (AT_CMD_EXE == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
	if(q_atcmd_par.ats7 != 0)
	{
		if(Qats7Timer != NULL)
		{
			if(osiTimerIsRunning(Qats7Timer) == true)
			{
				ql_rtos_timer_stop(Qats7Timer);
			}
		}
	}
#endif	
        if (cmd->param_count > 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint32_t cc_status = CFW_CcGetCallStatus(sim);
        if (cc_status != CC_STATE_INCOMING && cc_status != CC_STATE_WAITING)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

#ifdef CONFIG_SOC_8910
        audevStopTone();
        if (!audevStartVoice())
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        uint32_t result = CFW_CcAcceptSpeechCall(sim);
        if (result != 0)
        {
#ifdef CONFIG_SOC_8910
            audevStopVoice();
#endif
            RETURN_CME_CFW_ERR(cmd->engine, result);
        }
        atCfwCcSetOnline(cmd->engine);
        //atSetPendingIdCmd(cmd, EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP, _ataRspCB);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
		osiThreadSleep(5);
        RETURN_OK(cmd->engine);
#else
        atCmdRespFinish(cmd->engine);
#endif
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

//
//  SIP URI has a similar form to an email address. It contains typically a username and a hostname,
//  for example test@realtimecommunication.info, where realtimecommunication.info is the domain of a SIP service provider.
//  A SIP or SIPS URI identifies a communications resource. Its general form, in the case of a SIP URI, is:
//  sip:user:password@host:port;uri-parameters?headers
//  password: we don't use passwords in VoLTE networks in SIP Protocol
//
//  RFC3261
//

// 7.18 List current calls
void atCmdHandleCLCC(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_EXE)
    {
        CFW_CC_CURRENT_CALL_INFO call_info[AT_CC_MAX_NUM] = {};
        uint8_t cnt = 0;

        uint32_t nRet = CFW_CcGetCurrentCall(call_info, &cnt, sim);
        OSI_LOGI(0, "atCmdHandleCLCC cnt = %d", cnt);

        if (nRet == ERR_CFW_NO_CALL_INPROGRESS || cnt == 0)
            RETURN_OK(cmd->engine);
        if (nRet != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

        char rsp[128];
        char dialnum[TEL_NUMBER_MAX_LEN + 2];
        for (int i = 0; i < cnt; i++)
        {
            // [+CLCC: <ccid1>,<dir>,<stat>,<mode>,<mpty>[,<number>,<type>
            // [,<alpha>[,<priority>[,<CLI validity>]]]]
            if (call_info[i].dialNumber.nSize)
            {
                cfwBcdToDialString(call_info[i].dialNumber.nTelNumber,
                                   call_info[i].dialNumber.nSize, dialnum);
            }
            else
            {
                char uriNum[URI_MAX_NUMBER + 1] = {
                    0x00,
                };
                atGetUriCallNumber((const char *)call_info[i].calling_uri, uriNum);
                strcpy(dialnum, uriNum);
            }
            sprintf(rsp, "+CLCC: %d,%d,%d,%d,%d,\"%s\",%d",
                    call_info[i].idx, call_info[i].direction,
                    call_info[i].status, call_info[i].mode,
                    call_info[i].multiparty, dialnum,
                    call_info[i].dialNumber.nType);

            atCmdRespInfoText(cmd->engine, rsp);
        }
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleO(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        bool paramok = true;
        unsigned cause = atParamDefUintInRange(cmd->params[0], 0, 0, 0, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        (void)cause; // not used
        atDispatch_t *dispatch = atCmdGetDispatch(cmd->engine);
        if (!atDispatchInDataEscape(dispatch))
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP
		{
			atCmdFinalHandle(cmd->engine, true);
			atCmdRespIntermCode(cmd->engine, CMD_RC_NOCARRIER);
			atCmdCommandFinished(cmd->engine);
			return;
		}
#else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        atEngineModeSwitch(AT_MODE_SWITCH_DATA_RESUME, dispatch);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

static bool _isGprsDial(const char *dial)
{
    if (memcmp(dial, AT_GPRS_DIALSTR1, strlen(AT_GPRS_DIALSTR1)) == 0 ||
        memcmp(dial, AT_GPRS_DIALSTR2, strlen(AT_GPRS_DIALSTR2)) == 0 ||
        memcmp(dial, AT_GPRS_DIALSTR3, strlen(AT_GPRS_DIALSTR3)) == 0 ||
        memcmp(dial, AT_GPRS_DIALSTR4, strlen(AT_GPRS_DIALSTR4)) == 0 ||
        memcmp(dial, AT_GPRS_DIALSTR5, strlen(AT_GPRS_DIALSTR5)) == 0 ||
        memcmp(dial, AT_GPRS_DIALSTR6, strlen(AT_GPRS_DIALSTR6)) == 0)
        return true;
    return false;
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
extern quec_at_ecc_s quec_at_ecc_current;
bool _isSimEmcNum(uint8_t *bcd, uint8_t len)
{
    if ((len != 2) && (!(len == 1 && (bcd[0] == 0x80||bcd[0] == 0x00))))
        return false;
	
    for (size_t n = 0; n < 20; n++)
    {
        if (memcmp(bcd, quec_at_ecc_current.emcNum_sim[n], len) == 0)
            return true;
    }
    return false;
}

bool _isNoSimEmcNum(uint8_t *bcd, uint8_t len)
{
    if ((len != 2) && (!(len == 1 && (bcd[0] == 0x80||bcd[0] == 0x00))))
        return false;

    for (size_t n = 0; n < 20; n++)
    {
        if (memcmp(bcd, quec_at_ecc_current.emcNum_nosim[n], len) == 0)
            return true;
    }
    return false;
}

#else

static bool _isSimEmcNum(uint8_t *bcd, uint8_t len)
{
    static uint8_t emcNum[][2] = {
        {0x11, 0xF2},
        {0x19, 0xF1},
    };

    if (len != 2)
        return false;

    for (size_t n = 0; n < OSI_ARRAY_SIZE(emcNum); n++)
    {
        if (memcmp(bcd, emcNum[n], len) == 0)
            return true;
    }
    return false;
}

static bool _isNoSimEmcNum(uint8_t *bcd, uint8_t len)
{
    static uint8_t emcNum[][2] = {
        {0x11, 0xF2},
        {0x19, 0xF1},
        {0x11, 0xF0},
        {0x11, 0xF9},
        {0x99, 0xF9},
        {0x11, 0xF8},
        {0x00, 0xF0},
        {0x21, 0xF0},
        {0x21, 0xF2},
        {0x80},
    };
    if ((len != 2) && (!((len == 1) && (bcd[0] == 0x80))))
        return false;

    for (size_t n = 0; n < OSI_ARRAY_SIZE(emcNum); n++)
    {
        if (memcmp(bcd, emcNum[n], len) == 0)
            return true;
    }
    return false;
}
#endif


static void _voiceDail(atCommand_t *cmd, CFW_DIALNUMBER_V2 *dnum, bool redial)
{
    uint8_t sim = atCmdGetSim(cmd->engine);
    uint32_t res;

    // not permit if in dialing
    if (!atCfwCcIsDialPermitted(cmd->engine))
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

#ifdef CFW_VOLTE_SUPPORT
    if (AT_CC_IsNwEmcNum(dnum->pDialNumber, dnum->nDialNumberSize, sim))
    {
        OSI_LOGI(0, "_voiceDail: EmcNum is from NW");

        if ((res = CFW_CcEmcDial(dnum->pDialNumber, dnum->nDialNumberSize, sim)) != 0)
            RETURN_CME_CFW_ERR(cmd->engine, res);

        atCfwCcSetDialing(cmd->engine, true);
        RETURN_OK(cmd->engine);
    }
#endif

    CFW_SIM_STATUS sim_status = CFW_GetSimStatus(sim);
    OSI_LOGI(0, "_voiceDail sim_status = 0x%x", sim_status);
    bool is_emc = ((sim_status == CFW_SIM_NORMAL || sim_status == CFW_SIM_TEST) &&
                   _isSimEmcNum(dnum->pDialNumber, dnum->nDialNumberSize)) ||
                  ((sim_status != CFW_SIM_NORMAL && sim_status != CFW_SIM_TEST) &&
                   _isNoSimEmcNum(dnum->pDialNumber, dnum->nDialNumberSize));
    OSI_LOGI(0, "_voiceDail: is_emc=%d", is_emc);

    if (is_emc)
    {
        CFW_CC_CURRENT_CALL_INFO call_info[AT_CC_MAX_NUM];
        memset(&call_info, 0, sizeof(call_info));
        uint8_t cnt = 0;

        res = CFW_CcGetCurrentCall(call_info, &cnt, sim);
        if (res == ERR_SUCCESS)
        {
            while (cnt--)
            {
                if (call_info[cnt].status != 0) // status==0,active
                {
                    if ((res = CFW_CcCallHoldMultiparty(1, call_info[cnt].idx, sim)) != 0)
                        RETURN_CME_CFW_ERR(cmd->engine, res);
                    else
                        gCcCallHoldEmcCallOn = 1;
                }
            }
        }
        osiThreadSleep(1000);

        if ((res = CFW_CcEmcDial(dnum->pDialNumber, dnum->nDialNumberSize, sim)) != 0)
            RETURN_CME_CFW_ERR(cmd->engine, res);

        atCfwCcSetDialing(cmd->engine, true);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		g_callinfo[0].dialNumber.nType = dnum->nType;
		g_callinfo[0].dialNumber.nSize = dnum->nDialNumberSize;
		memcpy(g_callinfo[0].dialNumber.nTelNumber, dnum->pDialNumber, dnum->nDialNumberSize);
		atCmdRespOK(cmd->engine);
		quec_dsci_urc(sim, 0, QUEC_CALL_ORIGINAL);
		return;
#else
        RETURN_OK(cmd->engine);
#endif
    }

    // TODO: SS
    if (atFDNProhibit(dnum->pDialNumber, dnum->nDialNumberSize, dnum->nType, sim))
    {
        sprintf(gAtCfwCtx.g_pCeer, "+CEER: Condition not fullfilled");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
    }

    // NOTE: this is special case, the command will finished here, however
    // there are UTI events to be handled.
    OSI_LOGI(0, "ss:dnum.Clir=%d nType=%d", dnum->nClir, dnum->nType);
    OSI_LOGXI(OSI_LOGPAR_SI, 0, "ss dnum->pDialNumber=%s size=%d", dnum->pDialNumber, dnum->nDialNumberSize);
    cmd->uti = cfwRequestNoWaitUTI();
    if ((res = CFW_CcInitiateSpeechCall_V2(dnum, cmd->uti, sim)) != 0)
    {
        if (ERR_CFW_INVALID_PARAMETER == res)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_CHAR_INDIAL);

        if (ERR_CFW_NOT_EXIST_FREE_UTI == res)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
    {
        char rsp[5] = {
            0x00,
        };
        sprintf(rsp, "OK");
        atCmdRespInfoText(cmd->engine, rsp);
    }
	OSI_LOGI(0,"cc!!!!%d",CFW_CcGetCallStatus(sim));
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
	if(q_atcmd_par.ats7 != 0)
	{
		if( Qats7Timer == NULL)
		{
			ql_rtos_timer_create(&Qats7Timer, (ql_task_t)atEngineGetThreadId(), QATS7_TimmerHandler, NULL);
			if(Qats7Timer != NULL)
			{
				ql_rtos_timer_start(Qats7Timer, q_atcmd_par.ats7*1000, 0);
			}
		}
		else
		{
			if(osiTimerIsRunning(Qats7Timer) == true)
			{
				ql_rtos_timer_stop(Qats7Timer);
				ql_rtos_timer_delete(Qats7Timer);
				Qats7Timer = NULL;
				ql_rtos_timer_create(&Qats7Timer, atEngineGetThreadId(), QATS7_TimmerHandler, NULL);
				if(Qats7Timer != NULL)
				{
					ql_rtos_timer_start(Qats7Timer, q_atcmd_par.ats7*1000, 0);
				}
			}
			else
			{
				ql_rtos_timer_start(Qats7Timer, q_atcmd_par.ats7*1000, 0);
			}
		}
	}
	g_callinfo[0].dialNumber.nType = dnum->nType;
	g_callinfo[0].dialNumber.nSize = dnum->nDialNumberSize;
	memcpy(g_callinfo[0].dialNumber.nTelNumber, dnum->pDialNumber, dnum->nDialNumberSize);
	quec_dsci_urc(sim, 0, QUEC_CALL_ORIGINAL);
#endif
    atCfwCcSetDialing(cmd->engine, false);
    atCmdRespFinish(cmd->engine);
}

// V.250 dial command D
void atCmdHandleD(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);
    CFW_COMM_MODE nFM = CFW_DISABLE_COMM;
    //flight mode,can't dail
    if (0 != CFW_GetComm(&nFM, sim))
    {
        OSI_LOGI(0, "atd CFW_GetComm fail");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    if (nFM == CFW_DISABLE_COMM)
    {
        OSI_LOGI(0, "atd in flight mode,not allowed");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
    CFW_DIALNUMBER_V2 dnum;

    memset(&dnum, 0x00, sizeof(CFW_DIALNUMBER_V2));
    if ((cmd->type != AT_CMD_EXE) && (cmd->type != AT_CMD_SET))
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);

    bool paramok = true;
    const char *dial_num = atParamRawText(cmd->params[0], &paramok);
    if (!paramok || cmd->param_count > 1)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

    size_t dial_num_len = strlen(dial_num);
    if (dial_num_len > ATD_PARA_MAX_LEN)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

    if (_isGprsDial(dial_num))
    {
#ifndef CONFIG_QUEC_PROJECT_FEATURE_PPP		
        CFW_NW_STATUS_INFO nStatusInfo;
#endif
        //One at angine support one ppp session
        atDispatch_t *dispatch = atCmdGetDispatch(cmd->engine);
        if (dispatch != NULL)
        {
            atDataEngine_t *dengine = atDispatchGetDataEngine(dispatch);
            if (dengine != NULL)
            {
                if (atDataIsPPPMode(dengine))
                {
                    OSI_LOGI(0, "at engine ppp is opened");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
        }
#ifndef CONFIG_QUEC_PROJECT_FEATURE_PPP
        if (CFW_GprsGetstatus(&nStatusInfo, sim) != ERR_SUCCESS)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NOT_ALLOW_EMERGENCY);
        if ((nStatusInfo.nStatus != CFW_NW_STATUS_REGISTERED_HOME) && (nStatusInfo.nStatus != CFW_NW_STATUS_REGISTERED_ROAMING))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NOT_ALLOW_EMERGENCY);
        OSI_LOGI(0, "ATD, GPRS dial");
#endif
        // not permit if the CC channel is dialing or online
        if (atCfwCcIsChannelActive(cmd->engine))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

        uint8_t cid = 1; // default value
        // when end with #[1-7]*, the number is cid
        if (dial_num_len > 3 &&
            dial_num[dial_num_len - 1] == '#' &&
            dial_num[dial_num_len - 3] == '*')
            cid = dial_num[dial_num_len - 2] - '0';
#ifndef CONFIG_QUEC_PROJECT_FEATURE_PPP			
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
#endif
        // at invalid, change to default cid silently
        if (cid < AT_PDPCID_MIN || cid > AT_PDPCID_MAX)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP  
        uint32_t ret = quec_check_gprs_pdp_state(sim, cid);
        if (CMD_RC_OK != ret)
        {
            RETURN_CME_ERR(cmd->engine, ret);
        }

        quec_netif *wan_if = NULL;
        quec_netif *ppp_netif = NULL;
        uint8_t todoAction = 0xFF;
#if IP_NAT
		if (get_nat_enabled(sim, cid))
		{
			todoAction = quec_check_netif_exist_or_available(sim, cid, &wan_if, &ppp_netif, NETIF_LINK_MODE_NAT_PPP_LAN);
		}
		else
#endif
		{
			todoAction = quec_check_netif_exist_or_available(sim, cid, &wan_if, &ppp_netif, NETIF_LINK_MODE_PPP);
		}
		OSI_LOGI(0, "QUEC_PPP ATD %d %d todo: %d %p %p", sim, cid, todoAction, ppp_netif, wan_if);
		if ((QUEC_NETIF_ACTION_BUSY == todoAction) || (QUEC_NETIF_ACTION_NONE == todoAction) || (QUEC_NETIF_ACTION_MATCH_MODE == todoAction))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        }
#endif
        atEnterPPPState(cmd, cid);
        return;
    }

    // voice dial format
    // * end with 'I', clir=1;
    // * end with 'i', clir=2;
    // * start with '+' or '00', international number
    // * extension is separated by p/P
    // * ATD_PARA_MAX_LEN (41)

    uint8_t clir = 0;
    if (dial_num[dial_num_len - 1] == 'I')
    {
        dial_num_len--;
        clir = 1;
    }
    if (dial_num[dial_num_len - 1] == 'i')
    {
        dial_num_len--;
        clir = 2;
    }

    char *pp = strchr(dial_num, 'p');
    if (pp == NULL)
        pp = strchr(dial_num, 'P');

    gAtCfwCtx.cc.extension_len = 0;
    gAtCfwCtx.cc.extension_pos = 0;
    if (pp != NULL)
    {
        int p_pos = pp - dial_num;
        memcpy(gAtCfwCtx.cc.extension, &dial_num[p_pos + 1], dial_num_len - p_pos - 1);
        gAtCfwCtx.cc.extension_len = dial_num_len - p_pos - 1;
        dial_num_len = p_pos;
    }

    int bcd_len;
    if ('+' == dial_num[0])
        bcd_len = cfwDialStringToBcd(dial_num + 1, dial_num_len - 1, dnum.pDialNumber);
    else if ((('0' == dial_num[0]) && ('0' == dial_num[1]) && (!('0' == dial_num[2] && dial_num_len == 3))))
        bcd_len = cfwDialStringToBcd(dial_num + 2, dial_num_len - 2, dnum.pDialNumber);
    else
        bcd_len = cfwDialStringToBcd(dial_num, dial_num_len, dnum.pDialNumber);

    if (bcd_len < 0)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_CHAR_INDIAL);
    if (bcd_len > TEL_NUMBER_MAX_LEN)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_DAIL_STR_LONG);

    dnum.nDialNumberSize = bcd_len;
    dnum.nClir = (clir > 0) ? clir : gAtSetting.sim[sim].clir;
    if (('+' == dial_num[0]) || (('0' == dial_num[0]) && ('0' == dial_num[1])))
        dnum.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
    else
        dnum.nType = gAtSetting.csta;

    OSI_LOGXI(OSI_LOGPAR_MSII, 0, "ATD num/%*s ext/%s/%d type/%d",
              bcd_len, dnum.pDialNumber, gAtCfwCtx.cc.extension,
              gAtCfwCtx.cc.extension_len, dnum.nType);

    // TODO: add dial number to phone book
    // TODO: check pin_puk state
#ifdef CONFIG_SOC_8910
    OSI_LOGI(0, "ATD, if smsmode[%d] is 0 while playtone stoptone", gAtSetting.smsmode);
    if (gAtSetting.smsmode == 0)
    {
        audevStopTone();
    }
#endif
    gAtCfwCtx.cc.last_dail_present = true;
    gAtCfwCtx.cc.last_dial = dnum;

    //for ss test
    OSI_LOGXI(OSI_LOGPAR_S, 0, "ss:ATD ss phone number=%s", dial_num);
    if (AT_IsSSString(dial_num, sim))
    {
        at_ss_parsing_string_req_struct sStringSTR;
        OSI_LOGI(0x100044b3, "ss:CmdFunc_D This is ss operation!!\n\r");
        memset(&sStringSTR, 0, sizeof(at_ss_parsing_string_req_struct));
        memcpy(sStringSTR.input, dial_num, dial_num_len);
        sStringSTR.length = dial_num_len;
        //#ifndef  AT_WITHOUT_SS
        OSI_LOGXI(OSI_LOGPAR_S, 0, "ss:CmdFunc_D sStringSTR.input=%s", sStringSTR.input);
        cmd->uti = cfwRequestNoWaitUTI();
        if (AT_SS_Adaption_ParseStringREQ(&sStringSTR, cmd->uti, sim))
        {
            OSI_LOGI(0x100044b4, "ss:CmdFunc_D The operation is OK!!\n\r");
            RETURN_OK(cmd->engine);
        }
        else
        //#endif
        {
            OSI_LOGI(0x100044b5, "ss:CmdFunc_D The operation is fail!!\n\r");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_UNKNOWN_ERROR);
        }
        return;
    }
    OSI_LOGI(0, "ss:dnum.nClir = %d", dnum.nClir);
    _voiceDail(cmd, &dnum, false);
}

// [+]for AT^DLST redial last call 2007.11.09
void atCmdHandleDLST(atCommand_t *cmd)
{
    CFW_DIALNUMBER_V2 dlstNum;
    if (AT_CMD_EXE == cmd->type)
    {
        OSI_LOGXI(OSI_LOGPAR_I, 0x00000000, "gAtCfwCtx.last_dail_present: %d",
                  gAtCfwCtx.cc.last_dail_present);

        if (!gAtCfwCtx.cc.last_dail_present)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        dlstNum = gAtCfwCtx.cc.last_dial;
        _voiceDail(cmd, &dlstNum, false);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
typedef struct
{
    uint8_t g_cc_h_cnt; // this is used to record whether the csw sent one or two rsp to AT.
} hAsyncCtx_t;

static void _hAsyncContextDelete(atCommand_t *cmd)
#else
void _hAsyncContextDelete(atCommand_t *cmd)
#endif
{
    if (cmd->async_ctx != NULL)
    {
        hAsyncCtx_t *async = (hAsyncCtx_t *)cmd->async_ctx;
        free(async);
        cmd->async_ctx = NULL;
    }
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
static void _athRspCB(atCommand_t *cmd, const osiEvent_t *event)
#else
void _athRspCB(atCommand_t *cmd, const osiEvent_t *event)
#endif
{
    if (!atCheckCfwEvent(event, EV_CFW_CC_RELEASE_CALL_RSP))
    {
        OSI_LOGI(0, "_athRspCB event error\n");
        cfwReleaseUTI(cmd->uti);
        if (cmd->engine)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PHONE_FAILURE);
        return;
    }

    if (pAT_CC_RELEASE_CALL_RSP_CB)
        (*pAT_CC_RELEASE_CALL_RSP_CB)(event);

    prvAlertStop();

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t sim = cfw_event->nFlag;
    hAsyncCtx_t *ctx = (hAsyncCtx_t *)cmd->async_ctx;
    OSI_LOGI(0, "_athRspCB, ctx->g_cc_h_cnt =%d, cfw_event->nType =0x%x, cfw_event->nParam1: %d, cfw_event->nParam2: %d\n",
             ctx->g_cc_h_cnt, cfw_event->nType, cfw_event->nParam1, cfw_event->nParam2);

    if (ctx->g_cc_h_cnt)
        ctx->g_cc_h_cnt--;
    if ((CC_STATE_DIALING == cfw_event->nType) || (CC_STATE_ALERTLING == cfw_event->nType))
    {
        // TODO: "+CIEV: \"SOUNDER\",0"
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		uint8_t index = (cfw_event->nParam1 - 1 >= 0) ? (cfw_event->nParam1 - 1) : 0;
		quec_dsci_urc(sim, index, QUEC_CALL_END);
#else
        sprintf(gAtCfwCtx.g_pCeer, "+CEER: %s", "NO CARRIER");
        atCmdRespSimUrcCode(sim, CMD_RC_NOCARRIER);
#endif
        if (ctx->g_cc_h_cnt == 0)
        {
            audevStopVoice();
            prvReportSounder(sim, 0);

            atCfwCcSetOffline();
            cfwReleaseUTI(cmd->uti);
			if (cmd->engine)
				RETURN_OK(cmd->engine);
        }
    }
    else if (0xF0 == cfw_event->nType)
    {
        generateCeerRCCode(cfw_event->nParam1);

        if (cfw_event->nParam2 == 0x10008) // 建立链路过程中，想挂断电话会返回err
        {
            OSI_LOGI(0, "_athRspCB failure in link establishment, err code  0x10008");
            audevStopVoice();

            atCfwCcSetOffline();
            cfwReleaseUTI(cmd->uti);
            if (cmd->engine)
                RETURN_OK(cmd->engine);
        }

        if (ctx->g_cc_h_cnt == 0)
        {
            audevStopVoice();

            atCfwCcSetOffline();
            cfwReleaseUTI(cmd->uti);
            if (cmd->engine)
                RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
        }
    }
    else
    {
        cfwReleaseUTI(cmd->uti);
        if (cmd->engine)
            RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
    }
}

void atCmdHandleH(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);
    uint32_t result;

    if (AT_CMD_EXE == cmd->type)
    {
        if (cmd->param_count > 0)
        {
            bool paramok = true;
            uint8_t value = atParamUint(cmd->params[0], &paramok);
            if (!paramok || value != 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		if(q_atcmd_par.ats7 != 0)
		{
			if(Qats7Timer != NULL)
			{
				if(osiTimerIsRunning(Qats7Timer) == true)
				{
					ql_rtos_timer_stop(Qats7Timer);
				}
			}
		}
		
		if(q_atcmd_par.cvhu == true)
		{
			RETURN_OK(cmd->engine);
		}
#endif
        atDispatch_t *dispatch = atCmdGetDispatch(cmd->engine);
        if (dispatch != NULL)
        {
            atDataEngine_t *dengine = atDispatchGetDataEngine(dispatch);
            if (dengine != NULL)
            {
                if (atDataIsPPPMode(dengine))
                {
                    pppSession_t *pppSession = atDataEngineGetPppSession(dengine);
                    if (pppSession == NULL)
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                    pppSessionClose(pppSession, 0);
                    pppSession = NULL;
                    RETURN_OK(cmd->engine);
                }
            }
        }

        // when CC active, can only operate on CC channel
        if (!atCfwCcIsPermitted(cmd->engine))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

        uint32_t cc_state = CFW_CcGetCallStatus(sim);
        if (cc_state == CC_STATE_NULL)
            RETURN_OK(cmd->engine);

        if (cc_state == CC_STATE_RELEASE)
        {
            atCfwCcSetOffline();
            RETURN_OK(cmd->engine);
        }

        hAsyncCtx_t *ctx = (hAsyncCtx_t *)malloc(sizeof(*ctx));
        if (ctx == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = ctx;
        cmd->async_ctx_destroy = _hAsyncContextDelete;
        ctx->g_cc_h_cnt = AT_CC_GetCCCount(sim);
        OSI_LOGI(0, "ath callp, count/%d", ctx->g_cc_h_cnt);

        //use UTI modify by ying.li1 for 8910V1.2

        // cmd->uti = cfwRequestUTI((osiEventCallback_t)_athRspCB, cmd);
        cmd->uti = cfwRequestUTIEx((osiEventCallback_t)_athRspCB, cmd, NULL, false);
        result = CFW_CcReleaseCall_V2(cmd->uti, sim);
        if (ERR_SUCCESS != result)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_CFW_ERR(cmd->engine, result);
        }
        RETURN_FOR_ASYNC();
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// Tone duration +VTD
void atCmdHandleVTD(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +VTD=<n>
        bool paramok = true;
        uint8_t vtd = atParamUintInRange(cmd->params[0], 1, 10, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.vtd = vtd;
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+VTD: (1-10)");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
        sprintf(rsp, "+VTD: %d", gAtSetting.vtd);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

typedef struct
{
    uint8_t *dtmf;
    uint8_t duration;
    size_t count;
    size_t pos;
    osiTimer_t *timer;
} vtsAsyncCtx_t;

static void vtsAsyncCtxDestroy(atCommand_t *cmd)
{
    vtsAsyncCtx_t *ctx = (vtsAsyncCtx_t *)cmd->async_ctx;
    if (ctx == NULL)
        return;

    osiTimerDelete(ctx->timer);
    free(ctx);
    cmd->async_ctx = NULL;
}

static void _vtsPlay(void *param)
{
    atCommand_t *cmd = (atCommand_t *)param;
    vtsAsyncCtx_t *ctx = (vtsAsyncCtx_t *)cmd->async_ctx;
    uint8_t sim = atCmdGetSim(cmd->engine);

    if (ctx->pos >= ctx->count)
    {
        CFW_CcStopTone(sim);
        RETURN_OK(cmd->engine);
    }

    if (ctx->pos > 0)
        CFW_CcStopTone(sim);

    uint8_t tone = ctx->dtmf[ctx->pos++];
    uint32_t ret = CFW_CcPlayTone(cfwToneToDtmfChar(tone), sim);
    if (ret != 0)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

    osiTimerStart(ctx->timer, ctx->duration * 100);
}

// DTMF and tone generation +VTS
void atCmdHandleVTS(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count != 1 && cmd->param_count != 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_CHAR_INTEXT);

        bool paramok = true;
        const char *dtmf = atParamDtmf(cmd->params[0], &paramok);
        uint16_t duration = atParamDefUintInRange(cmd->params[1], gAtSetting.vtd, 1, 10, &paramok);
        if (!paramok || dtmf[0] == 0 || cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_CHAR_INTEXT);

        size_t len = strlen(dtmf);
        vtsAsyncCtx_t *ctx = (vtsAsyncCtx_t *)malloc(sizeof(*ctx) + len);
        if (ctx == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = ctx;
        cmd->async_ctx_destroy = vtsAsyncCtxDestroy;
        ctx->dtmf = (uint8_t *)ctx + sizeof(*ctx);
        ctx->duration = duration;
        ctx->count = strlen(dtmf);
        ctx->pos = 0;
        ctx->timer = osiTimerCreate(osiThreadCurrent(), _vtsPlay, cmd);

        for (size_t n = 0; n < len; n++)
        {
            int tone = cfwDtmfCharToTone(dtmf[n]);
            if (tone < 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_CHAR_INTEXT);
            ctx->dtmf[n] = tone;
        }

        osiThreadCallback(atEngineGetThreadId(), _vtsPlay, cmd);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+VTS: (0-9,*,#,A,B,C,D),(1-10)");
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

typedef struct
{
    uint8_t g_cc_chld; // this is used to record whether the csw sent one or two rsp to AT.
} chldAsyncCtx_t;

static void _chldAsyncContextDelete(atCommand_t *cmd)
{
    if (cmd->async_ctx != NULL)
    {
        chldAsyncCtx_t *async = (chldAsyncCtx_t *)cmd->async_ctx;
        free(async);
        cmd->async_ctx = NULL;
    }
}

static void _chldRspCB(atCommand_t *cmd, const osiEvent_t *event)
{

    if (pAT_CC_CALL_HOLD_MULTIPARTY_RSP_CB)
        (*pAT_CC_CALL_HOLD_MULTIPARTY_RSP_CB)(event);

    if (!atCheckCfwEvent(event, EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP))
    {
        OSI_LOGI(0, "_chldRspCB event error\n");
        cfwReleaseUTI(cmd->uti);
        if (cmd->engine)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PHONE_FAILURE);
        return;
    }
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    chldAsyncCtx_t *ctx = (chldAsyncCtx_t *)cmd->async_ctx;

    if (ctx->g_cc_chld)
        ctx->g_cc_chld--;
	
    OSI_LOGI(0, "EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, ctx->g_cc_chld =%d, cfw_event->nType =0x%x\n",
             ctx->g_cc_chld, cfw_event->nType);
	
    prvAlertStop();
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
	int8_t index = (cfw_event->nParam1 - 1 >= 0) ? (cfw_event->nParam1 - 1) : 0;
	if(cfw_event->nType){
		quec_dsci_urc(cfw_event->nFlag, index, QUEC_CALL_CONNECT);
	}else{
		quec_dsci_urc(cfw_event->nFlag, index, QUEC_CALL_HOLD);
	}
#endif
    if ((0x00 == cfw_event->nType) ||
        (0x01 == cfw_event->nType) ||
        (0x0F == cfw_event->nType))
    {
        uint8_t cnt = AT_CC_GetCCCount(cfw_event->nFlag);
        if (cnt == 0 || ctx->g_cc_chld == 0)
        {
            _voiceCallStateUpdate(cfw_event->nFlag);
            cfwReleaseUTI(cmd->uti);
            if (cmd->engine)
                RETURN_OK(cmd->engine);
        }
    }
    else
    {
        _voiceCallStateUpdate(cfw_event->nFlag);
        cfwReleaseUTI(cmd->uti);
        if (cmd->engine)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PHONE_FAILURE);
    }
}

#ifdef CFW_VOLTE_SUPPORT
static void _chldvolteRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    if (!atCheckCfwEvent(event, EV_CFW_CC_CALL_SWAP_RSP))
    {
        OSI_LOGI(0, "_chldvolteRspCB event error\n");
        cfwReleaseUTI(cmd->uti);
        if (cmd->engine)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PHONE_FAILURE);
        return;
    }

    OSI_LOGI(0, "_chldvolteRspCB_EV_CFW_CC_CALL_SWAP_RSP \n");
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    chldAsyncCtx_t *ctx = (chldAsyncCtx_t *)cmd->async_ctx;

    if (ctx->g_cc_chld)
        ctx->g_cc_chld--;

    OSI_LOGI(0, "EV_CFW_CC_CALL_SWAP_RSP, ctx->g_cc_chld =%d, cfw_event->nType =0x%x\n",
             ctx->g_cc_chld, cfw_event->nType);
    
    if (0x00 == cfw_event->nType)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		int8_t index = (cfw_event->nParam1 - 1 >= 0) ? (cfw_event->nParam1 - 1) : 0;
		quec_dsci_urc(cfw_event->nFlag, index, QUEC_CALL_HOLD);
#endif
		if (ctx->g_cc_chld == 0)
        {
            _voiceCallStateUpdate(cfw_event->nFlag);
            cfwReleaseUTI(cmd->uti);
            if (cmd->engine)
                RETURN_OK(cmd->engine);
        }
		
        //   atSetPendingIdCmd(cmd, EV_CFW_CC_CALL_SWAP_RSP, _chldvolteRspCB);
    }
    else
    {
        _voiceCallStateUpdate(cfw_event->nFlag);
        cfwReleaseUTI(cmd->uti);
        if (cmd->engine)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PHONE_FAILURE);
    }
}
#endif

void atCmdHandleCHLD(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (AT_CMD_SET == cmd->type)
    {
        // if CC active, can only on CC channel
        if (!atCfwCcIsPermitted(cmd->engine))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

        // +CHLD=<n>
        CFW_CC_CURRENT_CALL_INFO call_info[AT_CC_MAX_NUM];
        bool paramok = true;
        uint8_t nCmd;
        uint8_t nIndex;
        uint8_t nCnt = 0;
        uint8_t wait = 0, active = 0, hold = 0;
        uint8_t sign = 0;

        uint32_t uData = atParamUint(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if ((0 != uData) &&
            (1 != uData) &&
            (2 != uData) &&
            (3 != uData) &&
            ((uData < 11) || (uData > 17)) &&
            ((uData < 21) || (uData > 27)))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (uData > 10)
        {
            nCmd = uData / 10;
            nIndex = uData % 10;
        }
        else
        {
            nCmd = uData;
            nIndex = 0xFF;
        }

        ret = CFW_CcGetCurrentCall(call_info, &nCnt, sim);
        if ((!nCnt) && (ret != ERR_CFW_NO_CALL_INPROGRESS))
            RETURN_OK(cmd->engine);

        for (int i = 0; i < nCnt; i++)
        {
            if (call_info[i].status == 0) // active
                active++;
            else if (call_info[i].status == 5) // waiting,incoming call  hung up by +CHLD=0
                wait++;
            else if (call_info[i].status == 1) // hold
                hold++;
            if (nIndex == call_info[i].idx)
                sign = 1;
        }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
		if(nCmd == 2 && nIndex != 0xFF && nCnt>0)//2X
		{
			if(call_info[nIndex-1].status == 5){//waiting (MT call)
				nIndex = 0xFF;
			}
		}
#endif
        OSI_LOGI(0, "AT+CHLD:   active = %d, wait=%d, hold=%d, ims_call=%d",
                 active, wait, hold, gAtCfwCtx.cc.ims_call);

        chldAsyncCtx_t *ctx = (chldAsyncCtx_t *)malloc(sizeof(*ctx));
        if (ctx == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = ctx;
        cmd->async_ctx_destroy = _chldAsyncContextDelete;
        ctx->g_cc_chld = 0;

        if (0xff == nIndex)
        {
            if (0 == nCmd)
                ctx->g_cc_chld = wait ? 1 : 0 + ((!wait) ? hold : 0);
            else if (1 == nCmd)
                ctx->g_cc_chld = active + ((hold + wait) ? 1 : 0);
            else if (2 == nCmd)
                ctx->g_cc_chld = (active ? 1 : 0) + ((hold + wait) ? 1 : 0);
            else if (3 == nCmd)
                ctx->g_cc_chld = (active && hold) ? 1 : 0;
        }
        else
        {
            if (1 == nCmd)
            {
                ctx->g_cc_chld = sign ? 1 : 0;
            }
            else if (2 == nCmd)
            {
                OSI_LOGI(0, "AT+CHLD:   call_info[0].multiparty = %d", call_info[0].multiparty);
                active = (active && call_info[0].multiparty) ? 1 : active;
                hold = (hold && call_info[0].multiparty) ? 1 : hold;
                ctx->g_cc_chld = active + hold;
                ctx->g_cc_chld = (ctx->g_cc_chld > 2) ? 1 : ctx->g_cc_chld;
                // changyg[+]2008.6.20 due to network reason, we limit the split operation. reference bug8594
                if ((1 == nCnt) || (!sign) || gAtCfwCtx.cc.ims_call)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
        }

        OSI_LOGI(0, "AT+CHLD:   nCmd = %d, nIndex=%x\n", nCmd, nIndex);
        OSI_LOGI(0, "AT+CHLD:   g_cc_CHLD=%d\n", ctx->g_cc_chld);
        if (0 == ctx->g_cc_chld)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

#ifdef CFW_VOLTE_SUPPORT
        //Active + Hold in VOLTE
        uint8_t nCurrentRat = CFW_NWGetStackRat(sim);
        OSI_LOGI(0, "CHLD: CFW_NWGetStackRat nCurrentRat=%d", nCurrentRat);
        if (CFW_ImsIsSet(sim) && 2 == nCmd && 0xff == nIndex && (active > 0 && hold > 0 && wait == 0) && nCurrentRat != CFW_RAT_GSM_ONLY)
        {
            OSI_LOGI(0, "CHLD: volte chld CFW_ImsIsSet(sim) != 0");
            ctx->g_cc_chld = 1;
            cmd->uti = cfwRequestUTIEx((osiEventCallback_t)_chldvolteRspCB, cmd, NULL, false);
            ret = CFW_CcCallHoldMultiparty_V2(cmd->uti, nCmd, nIndex, sim);
            if (ERR_SUCCESS != ret)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
        }
        else
#endif
        {
            //   atSetPendingIdCmd(cmd, EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, _chldRspCB);
            OSI_LOGI(0, "CHLD: chld wait EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP event");
            cmd->uti = cfwRequestUTIEx((osiEventCallback_t)_chldRspCB, cmd, NULL, false);
            ret = CFW_CcCallHoldMultiparty_V2(cmd->uti, nCmd, nIndex, sim);
            if (ERR_SUCCESS != ret)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
        }
        RETURN_FOR_ASYNC();
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CHLD: (0,1,1X,2,2X,3)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// 6.4 Call mode
void atCmdHandleCMOD(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        // +CMOD=[<mode>]
        bool paramok = true;
        uint8_t mode = atParamDefUintInRange(cmd->params[0], 0, 0, 0, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        (void)mode;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        // only "single mode" supported
        atCmdRespInfoText(cmd->engine, "+CMOD: 0");
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CMOD: (0)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

static void _chupRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    if (!atCheckCfwEvent(event, EV_CFW_CC_RELEASE_CALL_RSP))
    {
        OSI_LOGI(0, "_chupRspCB event error\n");
        cfwReleaseUTI(cmd->uti);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PHONE_FAILURE);
        return;
    }

    _athRspCB(cmd, event);
}

// 6.5 Hangup call
void atCmdHandleCHUP(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (AT_CMD_EXE == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		if(q_atcmd_par.ats7 != 0)
		{
			if(Qats7Timer != NULL)
			{
				if(osiTimerIsRunning(Qats7Timer) == true)
				{
					ql_rtos_timer_stop(Qats7Timer);
				}
			}
		}
#endif	
        // when CC active, can only operate on CC channel
        if (!atCfwCcIsPermitted(cmd->engine))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

        uint32_t cc_state = CFW_CcGetCallStatus(sim);
        if (cc_state == CC_STATE_NULL)
            RETURN_OK(cmd->engine);

        if (cc_state == CC_STATE_RELEASE)
        {
            atCfwCcSetOffline();
            RETURN_OK(cmd->engine);
        }

        hAsyncCtx_t *ctx = (hAsyncCtx_t *)malloc(sizeof(*ctx));
        if (ctx == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = ctx;
        cmd->async_ctx_destroy = _hAsyncContextDelete;
        ctx->g_cc_h_cnt = AT_CC_GetCCCount(sim);
        OSI_LOGI(0, "chup call count/%d", ctx->g_cc_h_cnt);

        cmd->uti = cfwRequestUTIEx((osiEventCallback_t)_chupRspCB, cmd, NULL, false);
        ret = CFW_CcReleaseCall_V2(cmd->uti, sim);
        if (ERR_SUCCESS != ret)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_CFW_ERR(cmd->engine, ret);
        }
        RETURN_FOR_ASYNC();
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// 6.9 Service reporting control
void atCmdHandleCR(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);

    if (AT_CMD_SET == cmd->type)
    {
        // +CR=[<mode>]
        bool paramok = true;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
		uint8_t mode = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);
#else
		uint8_t mode = atParamDefUintInRange(cmd->params[0], 0, 0, 0, &paramok);
#endif
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.sim[sim].cr = mode;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char rsp[32];
        sprintf(rsp, "+CR: %d", gAtSetting.sim[sim].cr);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CR: (0,1)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// 6.11 Cellular result codes
void atCmdHandleCRC(atCommand_t *cmd)
{
    char rsp[32];
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    if (cmd->type == AT_CMD_SET)
    {
        // +CRC=[<mode>]
        bool paramok = true;
        unsigned mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
		
#if QUEC_AT_CONFIG_ALL_CHANNEL
		osiSlistHead_t *list = atDispatchGetList();
		osiSlistItem_t *item;
		SLIST_FOREACH(item, list, iter)
		{
			atDispatch_t *dispatch = (atDispatch_t *)item;
			chsetting = atCmdChannelSetting(atDispatchGetCmdEngine(dispatch));
			chsetting->crc = mode;
		}
#else
        chsetting->crc = mode;
#endif
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CRC: (0,1)");
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        sprintf(rsp, "+CRC: %d", chsetting->crc);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
static void _chccsRspCB(atCommand_t *cmd, const osiEvent_t *event)
#else
void _chccsRspCB(atCommand_t *cmd, const osiEvent_t *event)
#endif
{
    if (!atCheckCfwEvent(event, EV_CFW_CC_RELEASE_CALL_RSP))
    {
        OSI_LOGI(0, "_chccsRspCB event error\n");
        cfwReleaseUTI(cmd->uti);
        if (cmd->engine)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PHONE_FAILURE);
        return;
    }
    _athRspCB(cmd, event);
}

void atCmdHandleCHCCS(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);
    uint32_t nRet = 0;
    uint8_t index = 0;
    uint8_t cause = 0;

    if (cmd->type == AT_CMD_SET)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
		if(q_atcmd_par.ats7 != 0)
		{
			if(Qats7Timer != NULL)
			{
				if(osiTimerIsRunning(Qats7Timer) == true)
				{
					ql_rtos_timer_stop(Qats7Timer);
				}
			}
		}
#endif	
        bool paramok = true;
        CFW_CC_CURRENT_CALL_INFO call_info[7];
        uint8_t nCnt = 0;
        if ((cmd->param_count < 1) || (cmd->param_count > 2))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (cmd->param_count > 0)
        {
            index = atParamUintInRange(cmd->params[0], 0, 7, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (cmd->param_count > 1)
        {
            cause = atParamUintInRange(cmd->params[1], 1, 99, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (index == 0)
            RETURN_OK(cmd->engine);

        nRet = CFW_CcGetCurrentCall(call_info, &nCnt, sim);
        if (nRet == ERR_CFW_NO_CALL_INPROGRESS || nCnt == 0)
            RETURN_OK(cmd->engine);

        hAsyncCtx_t *ctx = (hAsyncCtx_t *)malloc(sizeof(*ctx));
        if (ctx == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = ctx;
        cmd->async_ctx_destroy = _hAsyncContextDelete;
        ctx->g_cc_h_cnt = 1;
        OSI_LOGI(0, "CHCCS call count/%d", ctx->g_cc_h_cnt);

        cmd->uti = cfwRequestUTIEx((osiEventCallback_t)_chccsRspCB, cmd, NULL, false);
        nRet = CFW_CcReleaseCallX_V2(cmd->uti, index, cause, sim);
        if (ERR_SUCCESS != nRet)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_CFW_ERR(cmd->engine, nRet);
        }
        RETURN_FOR_ASYNC();

        // atSetPendingIdCmd(cmd, EV_CFW_CC_RELEASE_CALL_RSP, _chccsRspCB);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CHCCS: (0-n)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

uint8_t UriParaParse(const char *pPara, char *pDialNum)
{
    if (strlen(pPara) > AT_CMD_D_ParaMaxLen || strlen(pPara) < AT_CMD_CDU_ParaMinLen)
        return AT_CMD_D_OPTYPE_NOTSUPPORT;

    memset(pDialNum, 0, ATD_PARA_MAX_LEN);
    memcpy(pDialNum, &pPara[4], strlen(pPara) - 4);

    if (!memcmp(pPara, "sip:", strlen("sip:")))
        return AT_CMD_D_OPTYPE_GPRSDIAL;
    else if (!memcmp(pPara, "tel:", strlen("tel:")))
        return AT_CMD_D_OPTYPE_VOICECALL;
    else if (!memcmp(pPara, "url:", strlen("tel:")))
        return AT_CMD_D_OPTYPE_NOTSUPPORT;
    else
        return AT_CMD_D_OPTYPE_ERR;
}

//Dial URI +CDU
void atCmdHandleCDU(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);
    CFW_DIALNUMBER_V2 dnum = {};
    if (AT_CMD_SET == cmd->type)
    {
        char dial_num[ATD_PARA_MAX_LEN] = {0};
        bool paramok = true;
        uint8_t action = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        const char *uri = atParamOptStr(cmd->params[1], &paramok);
        uint8_t client = atParamDefUint(cmd->params[2], 1, &paramok);
        uint8_t mpidx = atParamDefUintInRange(cmd->params[3], 1, 1, 2, &paramok);
        uint8_t CLIR_OIR = atParamDefUintInRange(cmd->params[4], 0, 0, 2, &paramok);
        signed CUG_pointer = atParamDefUint(cmd->params[5], 0, &paramok);
        uint8_t type_of_call = atParamDefUintInRange(cmd->params[6], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 7 || mpidx < 1 || CUG_pointer < 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if ((client != 1) && (client < 128 || client > 255))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        (void)client;
        (void)mpidx;
        (void)CLIR_OIR;
        (void)CUG_pointer;
        (void)type_of_call;
        if (cmd->param_count == 1)
        {
            if (action == 0)
            {
                atCmdRespInfoText(cmd->engine, "+CDUT: \"sip\",1");
                atCmdRespInfoText(cmd->engine, "+CDUT: \"tel\",1");
                atCmdRespInfoText(cmd->engine, "+CDUT: \"urn\",1");
            }
            RETURN_OK(cmd->engine);
        }
        else if (action == 0 && cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t uOpType = UriParaParse(uri, dial_num);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "atCmdHandleCDU dial_num =%s", dial_num);
        size_t dial_num_len = strlen(dial_num);
        if (dial_num_len > ATD_PARA_MAX_LEN)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (AT_CMD_D_OPTYPE_VOICECALL == uOpType)
        {
            uint8_t clir = 0;
            if (dial_num[dial_num_len - 1] == 'I')
            {
                dial_num_len--;
                clir = 1;
            }
            if (dial_num[dial_num_len - 1] == 'i')
            {
                dial_num_len--;
                clir = 2;
            }
            char *pp = strchr(dial_num, 'p');
            if (pp == NULL)
                pp = strchr(dial_num, 'P');

            gAtCfwCtx.cc.extension_len = 0;
            gAtCfwCtx.cc.extension_pos = 0;
            if (pp != NULL)
            {
                int p_pos = pp - dial_num;
                memcpy(gAtCfwCtx.cc.extension, &dial_num[p_pos + 1], dial_num_len - p_pos - 1);
                gAtCfwCtx.cc.extension_len = dial_num_len - p_pos - 1;
                dial_num_len = p_pos;
            }
            int bcd_len;
            if ('+' == dial_num[0])
                bcd_len = cfwDialStringToBcd(dial_num + 1, dial_num_len - 1, dnum.pDialNumber);
            else if (('0' == dial_num[0]) && ('0' == dial_num[1]))
                bcd_len = cfwDialStringToBcd(dial_num + 2, dial_num_len - 2, dnum.pDialNumber);
            else
                bcd_len = cfwDialStringToBcd(dial_num, dial_num_len, dnum.pDialNumber);
            if (bcd_len < 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_CHAR_INDIAL);
            if (bcd_len > SIM_PBK_NUMBER_SIZE)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_DAIL_STR_LONG);

            dnum.nDialNumberSize = bcd_len;
            dnum.nClir = (clir > 0) ? clir : gAtSetting.sim[sim].clir;
            if (('+' == dial_num[0]) || (('0' == dial_num[0]) && ('0' == dial_num[1])))
                dnum.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
            else
                dnum.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
            OSI_LOGXI(OSI_LOGPAR_MSII, 0, "CDU num/%*s ext/%s/%d type/%d",
                      bcd_len, dnum.pDialNumber, gAtCfwCtx.cc.extension,
                      gAtCfwCtx.cc.extension_len, dnum.nType);

            gAtCfwCtx.cc.last_dail_present = true;
            gAtCfwCtx.cc.last_dial = dnum;
            _voiceDail(cmd, &dnum, false);
        }
        else if (AT_CMD_D_OPTYPE_GPRSDIAL == uOpType)
        {
            if (_isGprsDial(dial_num))
            {
                CFW_NW_STATUS_INFO nStatusInfo;

                //One at angine support one ppp session
                atDispatch_t *dispatch = atCmdGetDispatch(cmd->engine);
                if (dispatch != NULL)
                {
                    atDataEngine_t *dengine = atDispatchGetDataEngine(dispatch);
                    if (dengine != NULL)
                    {
                        if (atDataIsPPPMode(dengine))
                        {
                            OSI_LOGI(0, "at engine ppp is opened");
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                        }
                    }
                }

                if (CFW_GprsGetstatus(&nStatusInfo, sim) != ERR_SUCCESS)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NOT_ALLOW_EMERGENCY);
                if ((nStatusInfo.nStatus != CFW_NW_STATUS_REGISTERED_HOME) && (nStatusInfo.nStatus != CFW_NW_STATUS_REGISTERED_ROAMING))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NOT_ALLOW_EMERGENCY);

                OSI_LOGI(0, "CDU, GPRS dial");

                // not permit if the CC channel is dialing or online
                if (atCfwCcIsChannelActive(cmd->engine))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

                uint8_t cid = 1; // default value
                // when end with #[1-7]*, the number is cid
                if (dial_num_len > 3 &&
                    dial_num[dial_num_len - 1] == '#' &&
                    dial_num[dial_num_len - 3] == '*')
                    cid = dial_num[dial_num_len - 2] - '0';

                // at invalid, change to default cid silently
                if (cid < AT_PDPCID_MIN || cid > AT_PDPCID_MAX)
                    cid = 1;

                atEnterPPPState(cmd, cid);
                return;
            }
            else
            {
                OSI_LOGI(0, "CDU, GPRS dialnum invalid");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CDU: (\"sip\",\"tel\")");
        atCmdRespOK(cmd->engine);
    }
    else
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
}

// IMS single radio voice call continuity
void atCmdHandleCISRVCC(atCommand_t *cmd)
{

    uint8_t sim = atCmdGetSim(cmd->engine);
    if (AT_CMD_SET == cmd->type)
    {
        // AT+CISRVCC=<uesrvcc>
        bool paramok = true;
        uint8_t ueSrvcc = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint16_t uti = cfwRequestNoWaitUTI();
        if (CFW_ImsSetSrvccMode(ueSrvcc, uti, sim) != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char response[32] = {0};
        uint8_t ueSrvcc = CFW_ImsGetSrvccMode(sim);
        sprintf(response, "+CISRVCC: %d", ueSrvcc);
        atCmdRespInfoText(cmd->engine, response);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CISRVCC: (0-1)");
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// UE's usage setting for EPS
void atCmdHandleCEUS(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t setting = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint16_t uti = cfwRequestNoWaitUTI();
        if (CFW_ImsSetTerminalMode(setting, uti, sim) != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char response[32] = {0};
        uint8_t setting = CFW_ImsGetTerminalMode(sim);
        sprintf(response, "+CEUS: %d", setting);
        atCmdRespInfoText(cmd->engine, response);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CEUS:(0,1)");
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// UE's Voice Domain Preference E-UTRAN
void atCmdHandleCEVDP(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t setting = atParamUintInRange(cmd->params[0], 1, 4, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint16_t uti = cfwRequestNoWaitUTI();
        if (CFW_ImsSetCevdp(setting, uti, sim) != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char response[32] = {0};
        uint8_t getsetting = CFW_ImsGetCevdp(sim);
        sprintf(response, "+CEVDP:%d", getsetting);
        atCmdRespInfoText(cmd->engine, response);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CEVDP:1..4");
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void atCmdHandleSETVOLTE(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);

    if (CFW_CfgGetPocEnable())
    {
        OSI_LOGI(0, "not poc project");
        AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t setvolte = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        uint8_t count = AT_GprsGetActivePdpCount(sim);
        uint8_t cccnt = AT_CC_GetCCCount(sim);

        OSI_LOGI(0, "SETVOLTE atCmdHandleSETVOLTE count = %d, cccnt = %d", count, cccnt);

        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if ((0 != cccnt) || count > 5)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

        uint16_t uti = cfwRequestNoWaitUTI();
        if (CFW_ImsSetVolte(setvolte, uti, sim) != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        gAtSetting.sim[sim].volte = setvolte;
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+SETVOLTE: 0,1");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char rsp[20] = {0};
        sprintf(rsp, "+SETVOLTE: %d", CFW_ImsIsSet(sim));
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

bool atFDNProhibit(const uint8_t *bcd, uint8_t bcd_len, uint8_t nTpye, uint8_t sim)
{
    AT_FDN_PBK_LIST *fdn_list = gAtCfwCtx.sim[sim].fdn_list;

    // not prohibited when no FDN list
    if (fdn_list == NULL || fdn_list->nFDNSatus != 1)
        return false;

    if (NULL == bcd || 0 == bcd_len || bcd_len > SIM_PBK_NUMBER_SIZE)
        return true;

    // fill 0xff for easier compare
    uint8_t fill_bcd[AT_FDN_NUMBER_SIZE];
    memset(fill_bcd, 0xff, AT_FDN_NUMBER_SIZE);
    memcpy(fill_bcd, bcd, bcd_len);

    for (uint8_t n = 0; n < fdn_list->nRealRecordNum; n++)
    {
        if (memcmp(fill_bcd, fdn_list->sRecord[n].nNumber,
                   AT_FDN_NUMBER_SIZE) == 0)
            return false;
    }
    return true;
}

static int generateCeerRCCode(uint32_t cause)
{
    OSI_LOGI(0, "generateCeerRCCode cause: %d", cause);

    int code = 0;
    switch (cause)
    {
    case CFW_CC_CAUSE_USER_ALERTING_NO_ANSWER:
    case CFW_CC_CAUSE_RECOVERY_ON_TIMER_EXPIRY:
        code = CMD_RC_NOANSWER;
        break;
    case CFW_CC_CAUSE_NOUSED_CAUSE:
    case CFW_CC_CAUSE_NORMAL_CALL_CLEARING:
    case CFW_CC_CAUSE_CHANNEL_UNAVAILABLE:
    case CFW_CC_CAUSE_NORMAL_UNSPECIFIED:
        code = CMD_RC_NOCARRIER;
        break;
    case CFW_CC_CAUSE_USER_BUSY:
        code = CMD_RC_BUSY;
        break;
    case CFW_CC_CAUSE_NO_USER_RESPONDING:
        code = CMD_RC_NODIALTONE;
        break;
    default:
		code = CMD_RC_NOCARRIER;
        break;
    }

    sprintf(gAtCfwCtx.g_pCeer, "+CEER: %s", _getCCresultCodeText(code));
    
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
	return CMD_RC_NOCARRIER;
#else
    return code;
#endif
}

void atCmdHandleCSTA(atCommand_t *cmd)
{
    char rsp[64];

    if (cmd->type == AT_CMD_SET)
    {
        // AT+CSTA=<type>
        bool paramok = true;
        const uint32_t list[] = {129, 145, 161};
        uint8_t type = atParamUintInList(cmd->params[0], list, 3, &paramok);
        if (!paramok || cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.csta = type;
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        sprintf(rsp, "%s: %d", cmd->desc->name, gAtSetting.csta);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        sprintf(rsp, "%s: (129,145,161)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}
