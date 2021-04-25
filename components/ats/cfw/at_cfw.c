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

#include "at_cfw.h"
#include "at_response.h"
#include "atr_config.h"
#include "osi_api.h"
#include "osi_log.h"
#include "cfw_event.h"
#include "cfw_errorcode.h"
#include "cfw_error_id.h"
#include <string.h>
#include <stdio.h>
#include "ats_config.h"

#include "quec_urc.h"
#include "quec_atcmd_general.h"
#include "quec_atcmd_sim.h"
#include "quec_led_task.h"
#include "quec_led_cfg_tbl.h"
#ifdef CONFIG_QUEC_PROJECT_FEATURE
#define quec_at_cfw_log(msg, ...)  custom_log("QATCFW", msg, ##__VA_ARGS__)
#endif

atCfwCtx_t gAtCfwCtx;

#define AT_PM3_FLAG 0 // TODO:
//When there is no card in the current sim slot,go to RESET another sim card.
//#define AT_PROIORITY_RESET_SIM
extern uint8_t atGetFlightModeFlag(CFW_SIM_ID nSimID);
#ifdef CONFIG_SOC_8910
void atSetFMProcess(void);
#endif

CFW_SIM_ID gPocMasterCard = 0xFF;
void atSetPocMasterCard(CFW_SIM_ID nSimID)
{
    OSI_LOGI(0, "atSetPocMasterCard, nSimID: 0x%x", nSimID);
    gPocMasterCard = nSimID;
    CFW_NwSetPocSimIndex(nSimID);

    char rsp[50] = {
        0,
    };

    sprintf(rsp, "+POC SIM: %d", nSimID);
    atCmdRespSimUrcText(nSimID, rsp);
}

CFW_SIM_ID atGetPocMasterCard(void)
{
    OSI_LOGI(0, "atGetPocMasterCard, gPocMasterCard: 0x%x", gPocMasterCard);
    return gPocMasterCard;
}

static void _onEV_CFW_INIT_IND(const osiEvent_t *event)
{
    CFW_EVENT cfw_event = *(const CFW_EVENT *)event;
    uint8_t nSim = cfw_event.nFlag;
    gAtCfwCtx.g_gc_pwroff = 0;

#ifdef CONFIG_ATR_CINIT_URC_ENABLED
#ifdef LTE_NBIOT_SUPPORT
    if (AT_PM3_FLAG == 0)
#endif
    {
        char rsp[32];
        sprintf(rsp, "^CINIT: %u, %lu, %lu", cfw_event.nType,
                cfw_event.nParam1, cfw_event.nParam2);
        atCmdRespSimUrcText(nSim, rsp);
    }
#endif

#ifdef PORTING_ON_GOING
    if (gAtCfwCtx.g_gc_pwroff && gAtCfwCtx.pwroff_engine != NULL)
    {
        gAtCfwCtx.g_gc_pwroff = 0; // hameina[+]2007.11.15 support CFUN
        atCmdRespOK(gAtCfwCtx.pwroff_engine);
        return;
    }
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    char quec_init_urc[64] = {0};
    quec_at_cfw_log("event:%d, %d, init=%d", cfw_event.nType, cfw_event.nParam1, quec_cfw_init_done);
    if (-1 == quec_cfw_init_done)
    {
        quec_cfw_init_done = 0;
        CFW_COMM_MODE nFM = CFW_DISABLE_COMM;
        uint32_t ret = CFW_GetComm(&nFM, nSim);
        quec_at_cfw_log("nFM:%d %d %d %d", nFM, nSim, ret, gAtSetting.sim[nSim].flight_mode);
        uint8_t cfun_mode = 0;
        if (0 == ret)
        {
            if (CFW_ENABLE_COMM == nFM)
            {
                cfun_mode = 1;
            }
        }
        sprintf(quec_init_urc, "+CFUN: %d", cfun_mode);
        quec_urc_send(NULL, quec_init_urc, strlen(quec_init_urc), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, 0);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG
        if(CFW_INIT_STATUS_NO_SIM != cfw_event.nType)
        {
            quec_netstatuslight_switch(SIM_INITIALIZING);
        }
#endif
    }
#endif

    OSI_LOGI(0, "_onEV_CFW_INIT_IND, nType: 0x%x, nSim: %d", cfw_event.nType, nSim);

    switch (cfw_event.nType)
    {
    case CFW_INIT_STATUS_NO_SIM: //
        gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_NO_SIM;
        memcpy(&(gAtCfwCtx.init_info.noSim[nSim]), &cfw_event, sizeof(cfw_event));

        if (false == gAtCfwCtx.setComFlag[nSim])
        {
            uint8_t nOtherSimID = (CFW_SIM_0 == nSim) ? CFW_SIM_1 : CFW_SIM_0;
            uint8_t nSimStatus = CFW_GetSimStatus(nOtherSimID);

            if (CFW_SIM_ABSENT == nSimStatus)
            {
                // When both SIM are absent, set SIM1 to fligt mode can save power.
                gAtCfwCtx.setComFlag[nSim] = true;
                CFW_SetComm(CFW_DISABLE_COMM, 1, cfw_event.nUTI, CFW_SIM_1);
                gAtCfwCtx.powerCommFlag[CFW_SIM_1] = true;
                gAtCfwCtx.gbPowerCommMsgFlag[CFW_SIM_1] = true;
            }
            else if (CFW_SIM_STATUS_END != nSimStatus)
            {
                gAtCfwCtx.setComFlag[nSim] = true;
                CFW_SetComm(CFW_DISABLE_COMM, 1, cfw_event.nUTI, nSim);
                gAtCfwCtx.powerCommFlag[nSim] = true;
                gAtCfwCtx.gbPowerCommMsgFlag[nSim] = true;
            }
        }
#ifdef CONFIG_DUAL_SIM_SUPPORT
        if (CFW_CfgGetPocEnable())
        {
            if (gAtSetting.poc_user_sim == 0x0 || gAtSetting.poc_user_sim == 0x01)
            {
                if (nSim == gAtSetting.poc_user_sim)
                {
                    OSI_LOGI(0, "POC, no sim for user select");
                    gPocMasterCard = nSim == 0 ? 1 : 0;
                }
            }
        }
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        strcpy(quec_init_urc, "+CPIN: NOT READY");
        quec_urc_send(NULL, quec_init_urc, strlen(quec_init_urc), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, 0);
        quec_cfw_init_done = 0;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG         
        quec_netstatuslight_switch(SIM_NOT_INSERT);
#endif
#endif
        break;

    case CFW_INIT_STATUS_SIM_DROP:
        OSI_LOGI(0x10003ee0, "Received CFW_INIT_STATUS_SIM_DROP in AT(nSim = %d)", nSim);
        break;

    case CFW_INIT_STATUS_SIM:
        gAtCfwCtx.setComFlag[nSim] = false;
        gAtCfwCtx.powerCommFlag[nSim] = false;
        if (cfw_event.nParam1 & CFW_INIT_SIM_PIN1_READY)
        {
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_SIM_AUTH_READY;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
            strcpy(quec_init_urc, "+CPIN: READY");
            quec_urc_send(NULL, quec_init_urc, strlen(quec_init_urc), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, 0);
            bool simType = CFW_GetSimType(nSim);
            if (simType == 1)
            {
                strcpy(quec_init_urc, "+QUSIM: 1");
            }
            else
            {
                strcpy(quec_init_urc, "+QUSIM: 0");
            }
            quec_urc_send(NULL, quec_init_urc, strlen(quec_init_urc), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS , 0);
            quec_cfw_init_done = 1;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG             
            //8910 not have network searching indication and we assume it start searching after SIM initialized successfully
            quec_netstatuslight_switch(SEARCHING);
#endif
#endif
        }
        else if (cfw_event.nParam1 & CFW_INIT_SIM_WAIT_PIN1)
        {
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_SIM_AUTH_WAIT_PIN1;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
            strcpy(quec_init_urc, "+CPIN: SIM PIN");
            quec_urc_send(NULL, quec_init_urc, strlen(quec_init_urc), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, 0);
            quec_cfw_init_done = 0;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG             
            quec_netstatuslight_switch(SIM_PIN_LOCK);
#endif
#endif
        }
        else if (cfw_event.nParam1 & CFW_INIT_SIM_WAIT_PUK1)
        {
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_SIM_AUTH_WAIT_PUK1;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
            strcpy(quec_init_urc, "+CPIN: SIM PUK");
            quec_urc_send(NULL, quec_init_urc, strlen(quec_init_urc), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, 0);
            quec_cfw_init_done = 0;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG             
            quec_netstatuslight_switch(SIM_PIN_LOCK);
#endif
#endif
        }

        memcpy(&(gAtCfwCtx.init_info.sim), &cfw_event, sizeof(cfw_event));
        break;

#ifndef CONFIG_AT_WITHOUT_SAT
    case CFW_INIT_STATUS_SAT:
        if (!(cfw_event.nParam1 & CFW_INIT_SIM_SAT))
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_SAT_OK;
        else //  if (cfw_event.nParam1 & CFW_INIT_SIM_SAT)
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_SAT_ERROR;

        memcpy(&(gAtCfwCtx.init_info.sat), &cfw_event, sizeof(cfw_event));
        break;
#endif

    case CFW_INIT_STATUS_SIMCARD:
        if ((cfw_event.nParam1 & CFW_INIT_SIM_CARD_CHANGED) ||
            (cfw_event.nParam1 & CFW_INIT_SIM_NORMAL_CARD))
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_NORMAL_CHANGED_SIM_CARD;
        else if (cfw_event.nParam1 & CFW_INIT_SIM_NORMAL_CARD)
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_NORMAL_SIM_CARD;
        else if (cfw_event.nParam1 & CFW_INIT_SIM_CARD_CHANGED)
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_CHANGED_SIM_CARD;
        else if (cfw_event.nParam1 & CFW_INIT_SIM_TEST_CARD)
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_TEST_SIM_CARD;

        memcpy(&(gAtCfwCtx.init_info.simCard), &cfw_event, sizeof(cfw_event));
#ifdef CONFIG_DUAL_SIM_SUPPORT
        if (CFW_CfgGetPocEnable())
        {
            if (gAtSetting.poc_user_sim == 0x0 || gAtSetting.poc_user_sim == 0x01)
            {
                if (nSim == gAtSetting.poc_user_sim)
                {
                    OSI_LOGI(0, "POC, find user select");
                    atSetPocMasterCard(nSim);
                    CFW_COMM_MODE nCom = CFW_DISABLE_COMM;
                    CFW_GetComm(&nCom, nSim);
                    if (nCom == CFW_ENABLE_COMM)
                    {
                        uint16_t uti = cfwRequestNoWaitUTI();
                        if (0 != CFW_GprsAtt(CFW_GPRS_ATTACHED, uti, nSim))
                        {
                            OSI_LOGI(0, "POC call gprs attach failed");
                        }
                    }
                }
                else if (atGetPocMasterCard() == 0xFF)
                {
                    OSI_LOGI(0, "POC, wait user select status");
                }
                else if (nSim == atGetPocMasterCard())
                {
                    OSI_LOGI(0, "POC, user select no sim, use sim %d", nSim);
                    atSetPocMasterCard(nSim);
                    CFW_COMM_MODE nCom = CFW_DISABLE_COMM;
                    CFW_GetComm(&nCom, nSim);
                    if (nCom == CFW_ENABLE_COMM)
                    {
                        uint16_t uti = cfwRequestNoWaitUTI();
                        if (0 != CFW_GprsAtt(CFW_GPRS_ATTACHED, uti, nSim))
                        {
                            OSI_LOGI(0, "POC call gprs attach failed");
                        }
                    }
                }
            }
            else if (atGetPocMasterCard() == 0xFF)
            {
                OSI_LOGI(0, "POC: select card %d as master card", nSim);
                atSetPocMasterCard(nSim);
                CFW_COMM_MODE nCom = CFW_DISABLE_COMM;
                CFW_GetComm(&nCom, nSim);
                if (nCom == CFW_ENABLE_COMM)
                {
                    uint16_t uti = cfwRequestNoWaitUTI();
                    if (0 != CFW_GprsAtt(CFW_GPRS_ATTACHED, uti, nSim))
                    {
                        OSI_LOGI(0, "POC call gprs attach failed");
                    }
                }
            }
        }
#endif
        break;

#ifndef CONFIG_AT_WITHOUT_SMS
    case CFW_INIT_STATUS_SMS: // SMS初始化完毕
    {
        CFW_SMS_STORAGE_INFO nStorageInfo;

        memset(&nStorageInfo, 0, sizeof(CFW_SMS_STORAGE_INFO));
        nStorageInfo.totalSlot = cfw_event.nParam2 & 0xFF;

        CFW_CfgSetSmsStorageInfo(&nStorageInfo, CFW_SMS_STORAGE_SM, nSim);
        OSI_LOGI(0x10003ee1, "Sim SMS GetTotalNum Success Total sms %d\n",
                 nStorageInfo.totalSlot);

        nStorageInfo.totalSlot = PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM;
        OSI_LOGI(0x10003ee2, "ME SMS GetTotalNum Success Total sms %d\n",
                 nStorageInfo.totalSlot);
        CFW_CfgSetSmsStorageInfo(&nStorageInfo, CFW_SMS_STORAGE_ME, nSim);

        CFW_SmsMoInit(cfw_event.nUTI, nSim);

        if (false == gAtCfwCtx.setComFlag[nSim])
        {
            uint8_t nOtherSimID = (CFW_SIM_0 == nSim) ? CFW_SIM_1 : CFW_SIM_0;
            uint8_t nSimStatus = CFW_GetSimStatus(nOtherSimID);

            if ((0 == nSimStatus)) //0 means CFW_SIM_ABSENT
            {
                gAtCfwCtx.powerCommFlag[nOtherSimID] = true;
                gAtCfwCtx.gbPowerCommMsgFlag[nOtherSimID] = true;
                gAtCfwCtx.setComFlag[nSim] = true;

                CFW_SetComm(CFW_DISABLE_COMM, 1, cfw_event.nUTI, nOtherSimID);
            }
        }

        gAtCfwCtx.sim[nSim].sms_init_done = true;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		//将保存的cssi,cssu同步到CP侧
		CFW_CfgSetSSN(gAtSetting.sim[nSim].cssi, gAtSetting.sim[nSim].cssu, nSim);

        if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND))
        {
            strcpy(quec_init_urc, "+QIND: SMS DONE");
            quec_urc_send(NULL, quec_init_urc, strlen(quec_init_urc), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
        }
        if (quec_cfw_init_done > 0)
        {
            quec_cfw_init_done += 2;
        }
#endif
        break;
    }
#endif

#ifndef CONFIG_AT_WITHOUT_PBK
    case CFW_INIT_STATUS_PBK:
        if (0 == (cfw_event.nParam1))
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_SIM_PBK_OK;
        else if (1 == (cfw_event.nParam1))
            gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_SIM_PBK_ERROR;

        AT_PBK_Init(nSim);
        memcpy(&(gAtCfwCtx.init_info.pbk), &cfw_event, sizeof(cfw_event));
        gAtCfwCtx.pbkReady = true;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND))
        {
            sprintf(quec_init_urc, "%s", "+QIND: PB DONE");
            quec_urc_send(NULL, quec_init_urc, strlen(quec_init_urc), URC_OUT_METHOD_UNICAST, RI_CATEG_OTHERS, 0);
        }
        if (quec_cfw_init_done > 0)
        {
            quec_cfw_init_done += 4;
        }
#endif
        break;
#endif

    case CFW_INIT_STATUS_AUTODAIL:
    {
        uint32_t ret = 0x00;
        uint8_t DestNumber[2] = {0x11, 0xF2};

        OSI_LOGI(0x10003ee3, "CFW_INIT_STATUS_AUTODAIL\n");

        while ((ret = CFW_CcEmcDial(DestNumber, 2, nSim)) != 0)
        {
            OSI_LOGI(0x08100a18, "Auto dial 112 failed,The errorcode is 0x%x!!!\n", ret);
            osiThreadSleep(500);
        }
        OSI_LOGI(0x08100a17, "Auto dial 112 Successl!!!\n");
        break;
    }

    case CFW_INIT_STATUS_NET:
        OSI_LOGI(0x10003ee4, "CFW_INIT_STATUS_NET\n");
#if defined(CFW_GPRS_SUPPORT) && !defined(CONFIG_AT_WITHOUT_GPRS)
        CFW_GprsAtt(CFW_GPRS_ATTACHED, 2, nSim); // TODO: fixed UTI??
#endif
        break;

    default:
        break;
    }
}

static void _onEV_CFW_SRV_STATUS_IND(const osiEvent_t *event)
{
    CFW_EVENT cfw_event = *(const CFW_EVENT *)event;
    uint8_t nSim = cfw_event.nFlag;

    if (cfw_event.nType == 0x10) // TODO: no enum??
        gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_SIM_SMS_OK;
    else if (cfw_event.nType == 0x11) // TODO; no enum??
        gAtCfwCtx.init_status[nSim] = AT_MODULE_INIT_SIM_SMS_ERROR;

#ifndef CONFIG_AT_WITHOUT_SMS
    CFW_SmsInitComplete(cfw_event.nUTI, nSim);

    atCfwSmsInit(cfw_event.nUTI, nSim);
    memcpy(&(gAtCfwCtx.init_info.sms[nSim]), &cfw_event, sizeof(cfw_event));

    gAtCfwCtx.smsReady = true;
    // TODO: (what is it?) gATCurrentAtSMSInfo[nSim].sCnmi.nMt = 2;
#endif
}

static void _onEV_CFW_SIM_GET_SMS_TOTAL_NUM_RSP(const osiEvent_t *event)
{
    CFW_EVENT cfw_event = *(const CFW_EVENT *)event;
    uint8_t nSim = cfw_event.nFlag;

    if (false == gAtCfwCtx.setComFlag[nSim])
    {
        uint8_t nOtherSimID = (CFW_SIM_0 == nSim) ? CFW_SIM_1 : CFW_SIM_0;
        uint8_t nSimStatus = CFW_GetSimStatus(nOtherSimID);

        gAtCfwCtx.setComFlag[nSim] = true;
        if (0 == nSimStatus || 4 == nSimStatus)
            CFW_SetComm(CFW_DISABLE_COMM, 1, cfw_event.nUTI, nOtherSimID);
    }
}

void atCfwInit(void)
{
    atCfwCtx_t *p = &gAtCfwCtx;

    memset(p, 0, sizeof(*p));

    Cfw_SetTimezone(gAtSetting.timezone);

    atCfwGcInit();
    atCfwNwInit();
    atCfwCcInit();
    atCfwGprsInit();
    AT_SS_Init();
}

#ifdef CONFIG_SOC_8910
void atSetFMProcess(void)
{
    uint16_t uti = cfwRequestNoWaitUTI();
    if (AT_ENABLE_COMM == atGetFlightModeFlag((CFW_SIM_ID)0))
    {
        CFW_SetComm(CFW_ENABLE_COMM, 0, uti, (CFW_SIM_ID)0);
    }
#ifdef CONFIG_DUAL_SIM_SUPPORT
    if (AT_ENABLE_COMM == atGetFlightModeFlag((CFW_SIM_ID)1))
    {
        CFW_SetComm(CFW_ENABLE_COMM, 0, uti, (CFW_SIM_ID)1);
    }
#endif
}
#endif
void atCfwPowerOn(const osiEvent_t *event)
{
    atEventsRegister(
        EV_CFW_INIT_IND, _onEV_CFW_INIT_IND,
        EV_CFW_SRV_STATUS_IND, _onEV_CFW_SRV_STATUS_IND,
        EV_CFW_SIM_GET_SMS_TOTAL_NUM_RSP, _onEV_CFW_SIM_GET_SMS_TOTAL_NUM_RSP,
        0);

    OSI_LOGI(0, "atEventRegister: EV_CFW_SAT_CMDTYPE_IND");
    atEventRegister(EV_CFW_SAT_CMDTYPE_IND, _onEV_CFW_SAT_CMDTYPE_IND);
    //atEventRegister(EV_CFW_SAT_RESPONSE_RSP, _onEV_CFW_SAT_RESPONSE_RSP);

#ifdef CONFIG_AT_VSIM_SUPPORT
    atVsimInit();
#endif

    CFW_CfgSetNwStatus(1, (CFW_SIM_ID)1);
    CFW_CfgSetNwStatus(1, (CFW_SIM_ID)0);
    CFW_CfgSetCharacterSet(CFW_CHSET_GSM);
    CFW_ShellControl(CFW_CONTROL_CMD_POWER_ON);
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
    atEngineSetDeviceAutoSleep(gAtSetting.csclk == 2 ? true : false);
#endif
#ifdef AT_PROIORITY_RESET_SIM
    CFW_SimSetProiorityResetFlag(1);
#endif

#ifdef CONFIG_SOC_8910
    if (!CFW_CfgGetPocEnable())
    {
        OSI_LOGI(0, "normal set comm");

        uint16_t uti = cfwRequestNoWaitUTI();
        CFW_ImsSetVolte(gAtSetting.sim[0].volte, uti, 0);

#ifdef CONFIG_DUAL_SIM_SUPPORT
        uti = cfwRequestNoWaitUTI();
        CFW_ImsSetVolte(gAtSetting.sim[1].volte, uti, 1);
#endif

        CFW_CfgNwSetCgclassType(gAtSetting.CgClassType[0], 0);
        CFW_SetNvUpdateFlag(gAtSetting.sim[0].cfgNvFlag, 0);
#ifdef CONFIG_DUAL_SIM_SUPPORT
        CFW_CfgNwSetCgclassType(gAtSetting.CgClassType[1], 1);
        CFW_SetNvUpdateFlag(gAtSetting.sim[1].cfgNvFlag, 1);
#endif

        atSetFMProcess();
    }
    else
    {
        OSI_LOGI(0, "Poc Enable");
        // TODO:g_uClassType should double
        gAtCfwCtx.g_uClassType = CLASS_TYPE_CG;
        CFW_CfgNwSetCgclassType(CLASS_TYPE_CG, CFW_SIM_0);
        CFW_CfgNwSetCgclassType(CLASS_TYPE_CG, CFW_SIM_1);
        atSetFMProcess();
    }
#endif
}

uint32_t atCfwToCmeError(uint32_t nCfwErrorCode)
{
    OSI_LOGI(0, "Error Code = %d", nCfwErrorCode);
    switch (nCfwErrorCode)
    {
    case ERR_CFW_INVALID_PARAMETER:
        return ERR_AT_CME_EXE_FAIL;

    case ERR_CME_SIM_NOT_INSERTED:
        return ERR_AT_CME_SIM_NOT_INSERTED;

    case ERR_CME_SIM_PIN_REQUIRED:
        return ERR_AT_CME_SIM_PIN_REQUIRED;

    case ERR_CME_SIM_PUK_REQUIRED:
        return ERR_AT_CME_SIM_PUK_REQUIRED;

    case ERR_CME_SIM_PIN2_REQUIRED:
        return ERR_AT_CME_SIM_PIN2_REQUIRED;

    case ERR_CME_SIM_PUK2_REQUIRED:
    case ERR_CME_SIM_CHV_NEED_UNBLOCK:
        return ERR_AT_CME_SIM_PUK2_REQUIRED;

    case ERR_NO_MORE_MEMORY:
        return ERR_AT_CME_MEMORY_FULL;

    case ERR_CME_MEMORY_FULL:
        return ERR_AT_CME_MEMORY_FULL;

    case ERR_CME_INCORRECT_PASSWORD:
    case ERR_CME_SIM_COMMAND_SUCCESS_VERIFICATION_FAIL:
        return ERR_AT_CME_INCORRECT_PASSWORD;

    case ERR_CFW_NOT_SUPPORT:
        return ERR_AT_CME_OPERATION_NOT_SUPPORTED;

    case ERR_CME_INVALID_INDEX:
        return ERR_AT_CME_INVALID_INDEX;

    case ERR_CME_MEMORY_FAILURE:
        return ERR_AT_CME_MEMORY_FAILURE;

    case ERR_CME_SIM_CHV_UNINIT:
    case ERR_CME_SIM_UNBLOCK_FAIL:
    case ERR_CME_SIM_UNBLOCK_FAIL_NO_LEFT:
    case ERR_CME_SIM_VERIFY_FAIL_NO_LEFT:
        return ERR_AT_CME_SIM_FAILURE;

    case ERR_CME_SIM_VERIFY_FAIL:
        return ERR_AT_CME_INCORRECT_PASSWORD;

    case ERR_CME_SIM_BLOCKED:
    case ERR_CME_SIM_PROFILE_ERROR:
    case ERR_CME_SIM_FILEID_NOT_FOUND:
    case ERR_CME_SIM_PATTERN_NOT_FOUND:
    case ERR_CME_SIM_MAXVALUE_REACHED:
    case ERR_CME_SIM_CONTRADICTION_INVALIDATION:
    case ERR_CME_SIM_CONTRADICTION_CHV:
    case ERR_CME_SIM_FILE_UNMATCH_COMMAND:
    case ERR_CME_SIM_NOEF_SELECTED:
    case ERR_CME_SIM_WRONG_CLASS:
    case ERR_CME_SIM_UNKNOW_COMMAND:
    case ERR_CME_SIM_CONDITION_NO_FULLFILLED:
        return ERR_AT_CME_SIM_WRONG;

    case ERR_CME_SIM_UNKNOW:
        return ERR_AT_CME_SIM_FAILURE;

    case ERR_CME_SIM_STK_BUSY:
        return ERR_AT_CME_SIM_BUSY;

    case ERR_CME_PHONE_FAILURE:
        return ERR_AT_CME_PHONE_FAILURE;

    case ERR_CME_NO_CONNECTION_TO_PHONE:
        return ERR_AT_CME_NO_CONNECT_PHONE;

    case ERR_CME_PHONE_ADAPTER_LINK_RESERVED:
        return ERR_AT_CME_PHONE_ADAPTER_LINK_RESERVED;

    case ERR_CME_OPERATION_NOT_ALLOWED:
        return ERR_AT_CME_OPERATION_NOT_ALLOWED;

    case ERR_CME_OPERATION_NOT_SUPPORTED:
        return ERR_AT_CME_OPERATION_NOT_SUPPORTED;

    case ERR_CME_PH_SIM_PIN_REQUIRED:
        return ERR_AT_CME_PHSIM_PIN_REQUIRED;

    case ERR_CME_PH_FSIM_PIN_REQUIRED:
        return ERR_AT_CME_PHFSIM_PIN_REQUIRED;

    case ERR_CME_PH_FSIM_PUK_REQUIRED:
        return ERR_AT_CME_PHFSIM_PUK_REQUIRED;

    case ERR_CME_SIM_FAILURE:
        return ERR_AT_CME_SIM_FAILURE;

    case ERR_CME_SIM_BUSY:
        return ERR_AT_CME_SIM_BUSY;

    case ERR_CME_NOT_FOUND:
        return ERR_AT_CME_NOT_FOUND;

    case ERR_CME_TEXT_STRING_TOO_LONG:
        return ERR_AT_CME_TEXT_LONG;

    case ERR_CME_INVALID_CHARACTERS_IN_TEXT_STRING:
        return ERR_AT_CME_INVALID_CHAR_INTEXT;

    case ERR_CME_DIAL_STRING_TOO_LONG:
        return ERR_AT_CME_DAIL_STR_LONG;

    case ERR_CME_INVALID_CHARACTERS_IN_DIAL_STRING:
        return ERR_AT_CME_INVALID_CHAR_INDIAL;

    case ERR_CME_NO_NETWORK_SERVICE:
        return ERR_AT_CME_NO_NET_SERVICE;

    case ERR_CME_NETWORK_TIMEOUT:
        return ERR_AT_CME_NETWORK_TIMOUT;

    case ERR_CME_NETWORK_NOT_ALLOWED_EMERGENCY_CALLS_ONLY:
        return ERR_AT_CME_NOT_ALLOW_EMERGENCY;

    case ERR_CME_NETWORK_PERSONALIZATION_PIN_REQUIRED:
        return ERR_AT_CME_NET_PER_PIN_REQUIRED;

    case ERR_CME_NETWORK_PERSONALIZATION_PUK_REQUIRED:
        return ERR_AT_CME_NET_PER_PUK_REQUIRED;

    case ERR_CME_NETWORK_SUBSET_PS_PIN_REQUIRED:
        return ERR_AT_CME_NET_SUB_PER_PIN_REQ;

    case ERR_CME_NETWORK_SUBSET_PS_PUK_REQUIRED:
        return ERR_AT_CME_NET_SUB_PER_PUK_REQ;

    case ERR_CME_SERVICE_PROVIDER_PS_PIN_REQUIRED:
        return ERR_AT_CME_SERVICE_PROV_PER_PIN_REQ;

    case ERR_CME_SERVICE_PROVIDER_PS_PUK_REQUIRED:
        return ERR_AT_CME_SERVICE_PROV_PER_PUK_REQ;

    case ERR_CME_CORPORATE_PERSONALIZATION_PIN_REQUIRED:
        return ERR_AT_CME_CORPORATE_PER_PIN_REQ;

    case ERR_CME_CORPORATE_PERSONALIZATION_PUK_REQUIRED:
        return ERR_AT_CME_CORPORATE_PER_PUK_REQ;

    case ERR_CME_SIM_TECHNICAL_PROBLEM:
    case ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED:
        return ERR_AT_CME_OPERATION_NOT_ALLOWED;

    case ERR_CFW_UTI_IS_BUSY:
    default:
        return ERR_AT_CME_EXE_FAIL;
    }
}

uint32_t atCfwToCmsError(uint32_t nInErrcode)
{
    switch (nInErrcode)
    {
    case ERR_CMS_INVALID_MEMORY_INDEX:
        return ERR_AT_CMS_INVALID_MEM_INDEX;
    case ERR_CMS_MEMORY_FULL:
        return ERR_AT_CMS_MEM_FULL;
    case ERR_CMS_TEMPORARY_FAILURE:
        return ERR_AT_CMS_TMEP_FAIL;
    case ERR_CMS_OPERATION_NOT_ALLOWED:
        return ERR_AT_CMS_OPER_NOT_ALLOWED;
    case ERR_CMS_MEMORY_CAPACITY_EXCEEDED:
        return ERR_AT_CMS_MEM_CAP_EXCCEEDED;
    case ERR_CMS_SMSC_ADDRESS_UNKNOWN:
        return ERR_AT_CMS_SCA_ADDR_UNKNOWN;
    case ERR_CMS_INVALID_CHARACTER_IN_PDU:
        return ERR_AT_CMS_INVALID_PDU_CHAR;
    case ERR_CMS_INVALID_CHARACTER_IN_ADDRESS_STRING:
        return ERR_AT_CMS_INVALID_ADDR_CHAR;
    case ERR_CMS_INVALID_PDU_MODE_PARAMETER:
        return ERR_AT_CMS_INVALID_PDU_PARAM;
    case ERR_CMS_INVALID_TEXT_MODE_PARAMETER:
        return ERR_AT_CMS_INVALID_TXT_PARAM;
    case ERR_CFW_INVALID_PARAMETER:
        return ERR_AT_CMS_INVALID_PARA;
    case ERR_CMS_TP_VPF_NOT_SUPPORTED:
        return ERR_AT_CMS_TP_VPF_NOT_SUPP;
    case ERR_CMS_INVALID_STATUS:
        return ERR_AT_CMS_INVALID_STATUS;
    case ERR_CMS_D0_SIM_SMS_STORAGE_FULL:
        return ERR_AT_CMS_D0_SIM_SMS_STO_FULL;
    case ERR_SMS_DM_SPACE_FULL:
        return ERR_AT_CMS_MEM_FULL;
    case ERR_CMS_PROTOCOLERROR:
        return ERR_AT_CMS_SMS_SEND_FAIL;
    case ERR_CMS_UNSPECIFIED_TP_PID_ERROR:
    case ERR_CMS_UNSPECIFIED_TP_DCS_ERROR:
    case ERR_CMS_UNKNOWN_ERROR:
    case ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED:
    case ERR_CFW_NOT_EXIST_FREE_UTI:
    case ERR_CFW_UTI_IS_BUSY:
    default:
        return ERR_AT_CMS_UNKNOWN_ERROR;
    }
}
/*
Because some events's parameter 1 is not an error code,function atCfwToCmeError is used instead.
uint32_t atCfwEventToCmeError(const CFW_EVENT *cfw_event)
{
    return (cfw_event->nType == 0xF0)
               ? atCfwToCmeError(cfw_event->nParam1)
               : ERR_AT_CME_EXE_FAIL;
}
*/
bool atCfwEventIdMatched(uint32_t id, uint32_t expected)
{
    if (id != expected)
    {
        OSI_LOGXE(OSI_LOGPAR_ISIS, 0, "unexpceted event, expect %d(%s), get %d(%s)",
                  expected, CFW_EventName(expected), id, CFW_EventName(id));
        return false;
    }
    return true;
}

atCindValue_t atCfwGetIndicator(uint8_t nSim)
{
    atCindValue_t cind;
    cind.battchg = 5;
    cind.call = gAtCfwCtx.sim[nSim].cc_call;
    cind.sounder = gAtCfwCtx.sim[nSim].cc_sounder;

    CFW_NW_STATUS_INFO nw_info;
    CFW_NwGetStatus(&nw_info, nSim);

    cind.service = 0;
    cind.roam = 0;
    if (CFW_NwGetStatus(&nw_info, nSim) == 0)
    {
        if (nw_info.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)

        {
            cind.service = 1;
            cind.roam = 1;
        }
        else if (nw_info.nStatus == CFW_NW_STATUS_REGISTERED_HOME)
        {
            cind.roam = 0;
            cind.service = 1;
        }
    }

    cind.signal = 0;
    uint8_t level, ber;
    if (CFW_NwGetSignalQuality(&level, &ber, nSim) == 0)
    {
        if (level >= 1 && level < 7)
            cind.signal = 1;
        else if (level >= 7 && level < 13)
            cind.signal = 2;
        else if (level >= 13 && level < 19)
            cind.signal = 3;
        else if (level >= 19 && level < 25)
            cind.signal = 4;
        else if (level >= 25 && level < 32)
            cind.signal = 5;
        else
            cind.signal = 0;
    }

    cind.message = gAtCfwCtx.sim[nSim].sms_newsms = gAtSetting.sim[nSim].cind_message;
    uint8_t full_mask = (CFW_SMS_STORAGE_ME | CFW_SMS_STORAGE_SM);
    cind.smsfull = ((gAtCfwCtx.sim[nSim].sms_memfull & full_mask) == full_mask) ? 1 : 0;
    return cind;
}

uint32_t atCfwPhoneActiveStatus(uint8_t nSim)
{
    uint32_t ret = AT_ACTIVE_STATUS_READY;
    uint8_t status = CFW_CcGetCallStatus(nSim);

    switch (status)
    {
    case CC_STATE_ACTIVE:
    case CC_STATE_HOLD:
    case CC_STATE_RELEASE:
        ret = AT_ACTIVE_STATUS_CALL_IN_PROGRESS;
        break;

    case CC_STATE_WAITING:
    case CC_STATE_INCOMING:
    case CC_STATE_DIALING:
    case CC_STATE_ALERTLING:
        ret = AT_ACTIVE_STATUS_RINGING;
        break;

    default:
        break;
    }

    if ((status == CC_STATE_ALERTLING) && gAtCfwCtx.cc.emc_dailing)
        ret = AT_ACTIVE_STATUS_CALL_IN_PROGRESS;
    return ret;
}

void atCfwGprsSetPCid(uint8_t cid, uint8_t pcid, uint8_t nsim)
{
    AT_Gprs_CidInfo *info = gAtCfwCtx.sim[nsim].cid_info;
    memcpy(&info[cid], &info[pcid], sizeof(AT_Gprs_CidInfo));
    info[cid].uCid = cid;
#ifdef LTE_NBIOT_SUPPORT
    info[cid].uPCid = pcid;
#endif
}

bool atCheckCfwEvent(const osiEvent_t *event, uint32_t expected)
{
    if (event->id != expected)
    {
        OSI_LOGE(0, "Unexpected event/%d, expected/%d", event->id, expected);
        return false;
    }
    return true;
}

bool atCfwCcIsActive(void)
{
    return gAtCfwCtx.cc.state != AT_CHANNEL_CC_NONE &&
           gAtCfwCtx.cc.engine != NULL;
}

bool atCfwCcIsChannelActive(atCmdEngine_t *engine)
{
    return gAtCfwCtx.cc.state != AT_CHANNEL_CC_NONE &&
           gAtCfwCtx.cc.engine != NULL &&
           engine == gAtCfwCtx.cc.engine;
}

//Modify the cc engine process mechanism.
//When the call originating channel exists, operations are only allowed on that channel.
//If the channel for initiating the call has been released.
//the current operating channel will be used as the default channel,
//and future call-related operations can only be performed on this channel.
//This can pervent multiple channels cc operations at the same time,causing stuck problems.
//
bool atCfwCcIsPermitted(atCmdEngine_t *engine)
{
    if (gAtCfwCtx.cc.state == AT_CHANNEL_CC_NONE ||
        engine == gAtCfwCtx.cc.engine)
        return true;

    osiSlistHead_t *list = atDispatchGetList();
    osiSlistItem_t *item;
    SLIST_FOREACH(item, list, iter)
    {
        atDispatch_t *dispatch = (atDispatch_t *)item;
        if (dispatch != NULL && atDispatchIsCmdMode(dispatch) && (atDispatchGetDeviceReadyStatus(dispatch)))
            if (gAtCfwCtx.cc.engine == atDispatchGetCmdEngine(dispatch))
            {
                return false;
            }
    }

    if (gAtCfwCtx.cc.engine != NULL)
        atCmdDeceaseRef(gAtCfwCtx.cc.engine);
    if (engine != NULL)
        atCmdIncreaseRef(engine);

    gAtCfwCtx.cc.engine = engine;

    return true;
}

bool atCfwCcIsDialPermitted(atCmdEngine_t *engine)
{
    // not permit if in dialing
    if (gAtCfwCtx.cc.state == AT_CHANNEL_CC_DIALING)
        return false;

    return gAtCfwCtx.cc.state == AT_CHANNEL_CC_NONE ||
           engine == gAtCfwCtx.cc.engine;
}

atCmdEngine_t *atCfwDialingEngine(void)
{
    OSI_LOGI(0, "atCfwDialingEngine CC state %d", gAtCfwCtx.cc.state);

    if (gAtCfwCtx.cc.state != AT_CHANNEL_CC_DIALING)
        return NULL;

    return gAtCfwCtx.cc.engine;
}

static void prvCcSetMode(atChannelCCState_t state, atCmdEngine_t *engine)
{
    OSI_LOGI(0, "CC state %d/%p -> %d/%p", gAtCfwCtx.cc.state,
             gAtCfwCtx.cc.engine, state, engine);

    gAtCfwCtx.cc.state = state;
    if (gAtCfwCtx.cc.engine != engine)
    {
        if (gAtCfwCtx.cc.engine != NULL)
            atCmdDeceaseRef(gAtCfwCtx.cc.engine);
        if (engine != NULL)
            atCmdIncreaseRef(engine);

        gAtCfwCtx.cc.engine = engine;
    }
}

void atCfwCcSetDialing(atCmdEngine_t *engine, bool emc)
{
    gAtCfwCtx.cc.emc_dailing = emc;
    prvCcSetMode(AT_CHANNEL_CC_DIALING, engine);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG 
    quec_netlight_deal_call_change(1);
#endif
}

void atCfwCcDailFinished(void)
{
    gAtCfwCtx.cc.emc_dailing = false;
    prvCcSetMode(AT_CHANNEL_CC_ONLINE, gAtCfwCtx.cc.engine);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG   
    quec_netlight_deal_call_change(1);
#endif
}

void atCfwCcSetOffline(void)
{
    gAtCfwCtx.cc.emc_dailing = false;
    prvCcSetMode(AT_CHANNEL_CC_NONE, NULL);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG  
    quec_netlight_deal_call_change(0);
#endif
}

void atCfwCcSetOnline(atCmdEngine_t *engine)
{
    gAtCfwCtx.cc.emc_dailing = false;
    prvCcSetMode(AT_CHANNEL_CC_ONLINE, engine);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG   
    quec_netlight_deal_call_change(1);
#endif
}
