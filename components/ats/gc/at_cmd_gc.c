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

#include "ats_config.h"
#include "osi_log.h"
#include "at_cfw.h"
#include "at_cfg.h"
#include "at_engine.h"
#include "at_command.h"
#include "at_response.h"
#include "cfw_event.h"
#include "drv_rtc.h"
#include "nvm.h"
#include "vfs.h"
#include "fs_mount.h"
#include "cfw.h"
#ifndef CONFIG_SOC_6760
#include "srv_dtr.h"
#endif
#include "audio_device.h"
#include "ppp_interface.h"
#include "kernel_config.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#include "quec_common.h"
#include "quec_atcmd_sim.h"
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP
#include "quec_ppp.h"
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
#include "quec_rtos.h"
#include "ql_uart.h"
#include "ql_uart_internal.h"
extern quec_dev_e  quec_get_dev_by_dispatch(atDispatch_t *dispatch);
extern quec_uart_setting_s quec_uart_current_setting[QL_UART_MAX_NUM]; 
extern uint32_t quec_main_uart_cbaud;
#endif

extern void atSetFlightModeFlag(uint8_t flag, CFW_SIM_ID nSimID);

#ifdef CONFIG_SOC_8910
extern uint32_t CFW_GprsSetstatus(CFW_NW_STATUS_INFO *pStatus, CFW_SIM_ID nSimID);
#endif
static const osiValueStrMap_t gCharSetVSMap[] = {
    {cs_gsm, "GSM"},
    {cs_hex, "HEX"},
    {cs_gbk, "PCCP936"},
    {cs_ucs2, "UCS2"},
    {cs_ira, "IRA"},
    {0, NULL},
};

extern void AT_Audio_Init(void);

static void _onEV_TIM_ALRAM_IND(const osiEvent_t *event)
{
    // int index = HIINT16(pEvent->nParam3);
    // if (isAlarmForUnicomSelfRegister(index))
    //     unicom_TimerRegProcess(0);
#ifdef LTE_NBIOT_SUPPORT
    // else if (isAlarmForPsmData(index))
    // {
    //     void AT_PsmData_TimerExpiry();
    //     AT_PsmData_TimerExpiry();
    // }
    // else
#endif
    {
        char rsp[20];
        sprintf(rsp, "+CALV: %lu", event->param3 >> 16);
        atCmdRespDefUrcText(rsp);
    }
}

static void _onEV_CFW_EXIT_IND(const osiEvent_t *event)
{
    drvRtcUpdateTime();
    atCfgAutoSave();
}

#ifdef CONFIG_SRV_DTR_ENABLE
#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPIO
static
#endif
void prvDtrRising(void *param)
{
    atDispatch_t *channel = atFindDataChannel();
    if (channel != NULL)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP        
        quec_ppp_dtr_rising_handler(channel);
#endif
        ; // AT&D only handles falling edge
    }
#if defined (CONFIG_QUEC_PROJECT_FEATURE_SLEEP) && !defined (CONFIG_QL_OPEN_EXPORT_PKG)
    else if (gAtSetting.csclk == 1)
    {
        atEngineSetDeviceAutoSleep(true);
    }
#endif
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPIO
static
#endif
void prvDtrFalling(void *param)
{
    atDispatch_t *channel = atFindDataChannel();

    if (channel != NULL)
    {
#ifndef CONFIG_QUEC_PROJECT_FEATURE_PPP        
        // Though it looks weird, AT&D is per channel setting.
        atCmdEngine_t *engine = atDispatchGetCmdEngine(channel);
        atChannelSetting_t *chsetting = atCmdChannelSetting(engine);
        if (chsetting->andd == 1)
        {
            atEngineModeSwitch(AT_MODE_SWITCH_DATA_ESCAPE, channel);
        }
        else if (chsetting->andd == 2)
        {
            atDataEngine_t *data_engine = atDispatchGetDataEngine(channel);
            if (data_engine == NULL)
            {
                OSI_LOGE(0, "Data engine is NULL in data channel");
                return;
            }

            pppSession_t *ppp = atDataEngineGetPppSession(data_engine);
            if (ppp == NULL)
            {
                OSI_LOGE(0, "PPP session is NULL in data engine");
                return;
            }

            pppSessionClose(ppp, 0);
        }
#endif        
    }
	
#if defined (CONFIG_QUEC_PROJECT_FEATURE_SLEEP) && !defined (CONFIG_QL_OPEN_EXPORT_PKG)
    else if (gAtSetting.csclk == 1)
    {
        atEngineSetDeviceAutoSleep(false);
    }
#endif

}
#endif

void atCfwGcInit(void)
{
// DM may be busy, and AT_Audio_Init will delay stack init.
// Using event, most likely EV_DM_POWER_ON_IND will be handle before AT_Audio_Init.
#ifdef AT_SOUND_RECORDER_SUPPORT
    //COS_TaskCallback(g_hAtTask, (COS_CALLBACK_FUNC_T)AT_Audio_Init, 0);
#endif

#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPIO
#ifdef CONFIG_SRV_DTR_ENABLE
    srvDtrStart(osiThreadCurrent(), prvDtrRising, prvDtrFalling, NULL);
#endif
#endif

    atEventsRegister(
        EV_TIM_ALRAM_IND, _onEV_TIM_ALRAM_IND,
        EV_CFW_EXIT_IND, _onEV_CFW_EXIT_IND,
        0);

#ifdef CONFIG_SOC_8910
    CFW_EnableLpsLock(gAtSetting.csclk == 0, 0);
#endif
}

// 9.1 Report mobile termination error
void atCmdHandleCMEE(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t cmee = atParamDefUintInRange(cmd->params[0], 0, 0, 2, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#if QUEC_AT_CONFIG_ALL_CHANNEL
		osiSlistHead_t *list = atDispatchGetList();
		osiSlistItem_t *item;
		SLIST_FOREACH(item, list, iter)
		{
			atDispatch_t *dispatch = (atDispatch_t *)item;
			chsetting = atCmdChannelSetting(atDispatchGetCmdEngine(dispatch));
			chsetting->cmee = cmee;
		}
#else
        chsetting->cmee = cmee;
#endif
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CMEE: (0-2)");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
        sprintf(rsp, "+CMEE: %u", chsetting->cmee);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleCEER(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        if (strlen(gAtCfwCtx.g_pCeer) != 0)
        {
            atCmdRespInfoText(cmd->engine, gAtCfwCtx.g_pCeer);
            atCmdRespOK(cmd->engine);
        }
        else
        {
            atCmdRespInfoText(cmd->engine, "+CEER:No Error Call");
            atCmdRespOK(cmd->engine);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// 8.10 Mobile termination event reporting
void atCmdHandleCMER(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        if (cmd->param_count == 0) // no parameter, just return ok
            RETURN_OK(cmd->engine);

        // +CMER=[<mode>[,<keyp>[,<disp>[,<ind>[,<bfr>[,<tscrn>][,<orientation>]]]]]]
        uint8_t mode = atParamDefUintInRange(cmd->params[0], 3, 3, 3, &paramok);
        uint8_t key = atParamDefUintInRange(cmd->params[1], 0, 0, 0, &paramok);
        uint8_t disp = atParamDefUintInRange(cmd->params[2], 0, 0, 0, &paramok);
        uint8_t ind = atParamDefUintInRange(cmd->params[3], 0, 0, 2, &paramok);

        (void)mode; // not used
        (void)key;  // not used
        (void)disp; // not used
        if ((!paramok) || (1 == ind))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.cmer_ind = ind;
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CMER: (3),(0),(0),(0,2)");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[24];
        // +CMER: <mode>,<keyp>,<disp>,<ind>,<bfr>
        sprintf(rsp, "+CMER: 3,0,0,%u", gAtSetting.cmer_ind);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

extern uint8_t atGetFlightModeFlag(CFW_SIM_ID nSimID);
#ifdef CONFIG_DUAL_SIM_SUPPORT
static void _cfunAttachRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    //CFW_EVENT cfwEvent = *(const CFW_EVENT *)event;
    //if (cfwEvent.nType != 0 && cfwEvent.nType != 1)
    //    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

    atCmdRespOK(cmd->engine);
}
#endif

#ifndef CONFIG_QUEC_PROJECT_FEATURE
static void _cfunSetCommRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    CFW_EVENT cfwEvent = *(const CFW_EVENT *)event;
    if (cfwEvent.nType != 0)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

#ifdef CONFIG_DUAL_SIM_SUPPORT
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (atGetFlightModeFlag(nSim) == AT_ENABLE_COMM && CFW_CfgGetPocEnable() && atGetPocMasterCard() == nSim)
    {
        uint16_t uti = cfwRequestUTI((osiEventCallback_t)_cfunAttachRsp, cmd);
        if (0 != CFW_GprsAtt(CFW_GPRS_ATTACHED, uti, nSim))
        {
            cfwReleaseUTI(uti);
            OSI_LOGI(0, "POC call gprs attach failed");
        }
        else
        {
            return;
        }
    }
#endif

    atCmdRespOK(cmd->engine);
}

static void _cfunSetCommRspTimeOutCB(atCommand_t *cmd)
{
    cfwReleaseUTI(cmd->uti);
    AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_SEND_TIMEOUT));
}

static void _cfunSimCloseRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    atCmdRespOK(cmd->engine);
}
#endif

static uint8_t GetBitCalibInfo(uint32_t param32, int nBit)
{
    if (nBit < 0 || nBit > 31)
        return -1;
    else
    {
        if (param32 & (1 << nBit))
            return 1;
        else
            return 0;
    }
}

void atCmdHandleCALIBINFO(atCommand_t *cmd)
{
    uint32_t gsmCal, lteCal;
    char RspGsm[200] = {0}, RspLte[200] = {0};
    uint8_t gsmBit[16], lteBit[16];
    if (cmd->type == AT_CMD_READ)
    {
        gsmCal = CFW_EmodGetGsmCalibInfo();
        lteCal = CFW_EmodGetLteCalibInfo();
        for (int nBit = 0; nBit < 16; nBit++)
        {
            gsmBit[nBit] = GetBitCalibInfo(gsmCal, nBit);
            lteBit[nBit] = GetBitCalibInfo(lteCal, nBit);
        }
        sprintf(RspGsm, "GSM_CALIBINFO: GSM_afc:%d, GSM850_agc:%d, GSM850_apc:%d, GSM900_agc:%d, GSM900_apc:%d, DCS1800_agc:%d,DCS1800_apc:%d, PCS1900_agc:%d, PCS1900_apc:%d, GSM_FT:%d, GSM_ANT:%d ",
                gsmBit[0], gsmBit[1], gsmBit[2], gsmBit[3], gsmBit[4], gsmBit[5], gsmBit[6], gsmBit[7], gsmBit[8], gsmBit[9], gsmBit[10]);
        sprintf(RspLte, "LTE_CALIBINFO: LTE_afc:%d, LTE_TDD_agc:%d, LTE_TDD_apc:%d, LTE_FDD_agc:%d, LTE_FDD_apc:%d, FINAL_LTE:%d, ANT_LTE:%d",
                lteBit[0], lteBit[1], lteBit[2], lteBit[4], lteBit[5], lteBit[8], lteBit[9]);
        atCmdRespInfoText(cmd->engine, RspGsm);
        atCmdRespInfoText(cmd->engine, RspLte);
        atCmdRespOK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE
void atCmdHandleCFUN(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    CFW_COMM_MODE nFM = CFW_DISABLE_COMM;

    if (cmd->type == AT_CMD_SET)
    {
        if (gAtCfwCtx.powerCommFlag[nSim])
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        static const uint32_t valid_funs[] = {0, 1, 4, 8};
        bool paramok = true;
        uint8_t fun = atParamUintInList(cmd->params[0], valid_funs, 3, &paramok);
        uint8_t OSI_UNUSED rst = atParamDefUintInRange(cmd->params[1], 0, 0, 1, &paramok);

        if (!paramok || cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (fun == 1)
        {
            if (0 != CFW_GetComm(&nFM, nSim))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            gAtSetting.sim[nSim].flight_mode = fun;
            OSI_LOGI(0, "atCmdHandleCFUN nFM = %d", nFM);
            if (CFW_ENABLE_COMM == nFM)
            {
                uint8_t nDetach = 0xFF;
                CFW_NwGetAllDetach(&nDetach, nSim);
                OSI_LOGI(0, "atCmdHandleCFUN nDetach = %d", nDetach);
                if (nDetach != 1)
                {
#ifdef CONFIG_DUAL_SIM_SUPPORT
                    if (CFW_CfgGetPocEnable())
                    {
                        if (atGetPocMasterCard() != nSim)
                        {
                            OSI_LOGI(0, "should not attach standby sim");
                            RETURN_OK(cmd->engine);
                        }
                    }
#endif
                    cmd->uti = cfwRequestUTI((osiEventCallback_t)_cfunSetCommRsp, cmd);
                    if (0 != CFW_AttachCSAndPS(cmd->uti, nSim))
                    {
                        cfwReleaseUTI(cmd->uti);
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    RETURN_FOR_ASYNC();
                }
                else
                    atCmdRespOK(cmd->engine);
            }
            else
            {
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_cfunSetCommRsp, cmd);
                if (0 != CFW_SetComm(CFW_ENABLE_COMM, 0, cmd->uti, nSim))
                {
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                atSetFlightModeFlag(AT_ENABLE_COMM, nSim);
                RETURN_FOR_ASYNC();
            }
        }
        else if ((fun == 0) || (fun == 4))
        {
            if (0 != CFW_GetComm(&nFM, nSim))
            {
                atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }

            gAtSetting.sim[nSim].flight_mode = ((fun == 0) ? 0 : 4);
            if (CFW_DISABLE_COMM == nFM)
            {
                atCmdRespOK(cmd->engine);
            }
            else
            {
                CFW_NW_STATUS_INFO sStatus;
                memset(&sStatus, 0, sizeof(CFW_NW_STATUS_INFO));
#ifdef CONFIG_SOC_8910
                CFW_GprsSetstatus(&sStatus, nSim);
#endif
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_cfunSetCommRsp, cmd);
                atCmdSetTimeoutHandler(cmd->engine, 1000 * 60 * 2, _cfunSetCommRspTimeOutCB);
                CFW_SetComm(CFW_DISABLE_COMM, 0, cmd->uti, nSim);
                atSetFlightModeFlag(AT_DISABLE_COMM, nSim);
                RETURN_FOR_ASYNC();
            }
        }
        else if (fun == 8)
        {
            for (int n = AT_PDPCID_MIN; n <= AT_PDPCID_MAX; n++)
            {
                uint8_t nGprsStatus = 0;
                if (0 != CFW_GetGprsActState(n, &nGprsStatus, nSim))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

                if (nGprsStatus == CFW_GPRS_ACTIVED)
                    CFW_ReleaseCID(n, nSim);
            }
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cfunSimCloseRsp, cmd);
            CFW_SimClose(cmd->uti, nSim);
            RETURN_FOR_ASYNC();
        }
        else // impossible
        {
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CFUN: (0,1,4),(0,1)");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        if (0 != CFW_GetComm(&nFM, nSim))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        char rsp[40];
        if (CFW_ENABLE_COMM == nFM)
            gAtSetting.sim[nSim].flight_mode = CFW_ENABLE_COMM; //modify for bug1187584
        else if ((CFW_DISABLE_COMM == nFM) && (1 == gAtSetting.sim[nSim].flight_mode))
            gAtSetting.sim[nSim].flight_mode = CFW_DISABLE_COMM;
        sprintf(rsp, "+CFUN: %u", gAtSetting.sim[nSim].flight_mode);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

static void _cpofRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    // EV_CFW_EXIT_IND
    if (cfw_event->nType == 0)
    {
        if (strcmp("$MYPOWEROFF", cmd->desc->name) != 0)
            atCmdRespInfoText(cmd->engine, "+CPOF: MS OFF");
        atCmdRespOK(cmd->engine);

        // Ideally, we should wait AT device output finished
        osiThreadSleep(50);
        osiShutdown(OSI_SHUTDOWN_POWER_OFF);
    }
    else if (cfw_event->nType == 0xf0)
    {
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam2);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}

#ifndef CONFIG_SOC_6760
static void _trbRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    // EV_CFW_EXIT_IND
    if (cfw_event->nType == 0)
    {
        atCmdRespInfoText(cmd->engine, "REBOOTING");
        drvRtcUpdateTime();
        atCfgAutoSave();
        atCmdRespOK(cmd->engine);

        // Ideally, we should wait AT device output finished
        osiThreadSleep(50);
        osiShutdown(OSI_SHUTDOWN_RESET);
    }
    else if (cfw_event->nType == 0xf0)
    {
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam2);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}
#endif

void atCmdHandleCPOF(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        gAtCfwCtx.g_gc_pwroff = 1;
        CFW_UpdateNV(true);
        if (0 != CFW_ShellControl(CFW_CONTROL_CMD_POWER_OFF))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        atSetPendingIdCmd(cmd, EV_CFW_EXIT_IND, _cpofRspCB);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

static void _trbRspTimeOutCB(atCommand_t *cmd)
{
    atCmdRespCmeError(cmd->engine, ERR_AT_CME_SEND_TIMEOUT);
    osiThreadSleep(50);
    osiShutdown(OSI_SHUTDOWN_RESET);
}

void atCmdHandleTRB(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        if (gAtCfwCtx.g_gc_pwroff != 0)
        {
            atCmdRespDefUrcText("Pleas wait REBOOT complete");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        CFW_UpdateNV(true);

        gAtCfwCtx.g_gc_pwroff = 2;
        atCmdSetTimeoutHandler(cmd->engine, 1000 * 60, _trbRspTimeOutCB);
        if (0 != CFW_ShellControl(CFW_CONTROL_CMD_POWER_OFF))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

#ifndef CONFIG_SOC_6760
        atSetPendingIdCmd(cmd, EV_CFW_EXIT_IND, _trbRspCB);
#else
        atCmdRespInfoText(cmd->engine, "REBOOTING");
        atCmdRespOK(cmd->engine);

        // Ideally, we should wait AT device output finished
        osiThreadSleep(50);
        osiShutdown(OSI_SHUTDOWN_RESET);
#endif
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
#endif

// 5.5 Select TE character set
void atCmdHandleCSCS(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CSCS: (\"GSM\",\"HEX\",\"PCCP936\",\"UCS2\",\"IRA\")");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        const char *chset_str = atParamStr(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        const osiValueStrMap_t *chset_vs = osiVsmapFindByIStr(gCharSetVSMap, chset_str);
        if (chset_vs == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.cscs = chset_vs->value;
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        const osiValueStrMap_t *chset_vs = osiVsmapFindByVal(gCharSetVSMap, gAtSetting.cscs);
        if (chset_vs == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        char rsp[32];
        sprintf(rsp, "+CSCS: \"%s\"", chset_vs->str);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// 6.22 Settings date format +CSDF
void atCmdHandleCSDF(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +CSDF=[[<mode>][,<auxmode>]]
        bool paramok = true;
        uint8_t mode = atParamDefUintInRange(cmd->params[0], 1, 1, 7, &paramok);
        uint8_t auxmode = atParamDefUintInRange(cmd->params[1], 1, 1, 2, &paramok);
        if (!paramok || cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.csdf_mode = mode;
        gAtSetting.csdf_auxmode = auxmode;
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CSDF: (1-7),(1,2)");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[64];
        sprintf(rsp, "+CSDF: %d,%d", gAtSetting.csdf_mode, gAtSetting.csdf_auxmode);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

OSI_UNUSED static int _outputTimeWithZone(char *rsp, struct tm *tm, int timezone)
{
    static const char *fmt1 = "%02d/%02d/%02d,%02d:%02d:%02d%+03d";
    static const char *fmt2 = "%04d/%02d/%02d,%02d:%02d:%02d%+03d";
    if (gAtSetting.csdf_auxmode == 1)
    {
        return sprintf(rsp, fmt1, (tm->tm_year + 1900) % 100, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec, timezone);
    }
    else
    {
        return sprintf(rsp, fmt2, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec, timezone);
    }
}

OSI_UNUSED static int _outputTime(char *rsp, struct tm *tm)
{
    static const char *fmt1 = "%02d/%02d/%02d,%02d:%02d:%02d";
    static const char *fmt2 = "%04d/%02d/%02d,%02d:%02d:%02d";
    if (gAtSetting.csdf_auxmode == 1)
    {
        return sprintf(rsp, fmt1, (tm->tm_year + 1900) % 100, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
    else
    {
        return sprintf(rsp, fmt2, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
}

OSI_UNUSED static bool _inputTime(const char *s, struct tm *tm, int *timezone, bool *has_zz)
{
    static const char *fmt1 = "%02d/%02d/%02d,%02d:%02d:%02d";
    static const char *fmt2 = "%04d/%02d/%02d,%02d:%02d:%02d";
    static const char *fmtz1 = "%02d/%02d/%02d,%02d:%02d:%02d%03d";
    static const char *fmtz2 = "%04d/%02d/%02d,%02d:%02d:%02d%03d";
    const char *fmt;
    size_t slen = strlen(s);

    *timezone = gAtSetting.timezone;
    if (slen == 17 || slen == 19)
    {
        *has_zz = false;
        fmt = (slen == 17) ? fmt1 : fmt2;
        int n = sscanf(s, fmt, &tm->tm_year, &tm->tm_mon, &tm->tm_mday,
                       &tm->tm_hour, &tm->tm_min, &tm->tm_sec);
        if (n != 6)
            return false;
    }
    else if (slen == 20 || slen == 22)
    {
        *has_zz = true;
        fmt = (slen == 20) ? fmtz1 : fmtz2;
        int n = sscanf(s, fmt, &tm->tm_year, &tm->tm_mon, &tm->tm_mday,
                       &tm->tm_hour, &tm->tm_min, &tm->tm_sec, timezone);
        if (n != 7)
            return false;

        if (*timezone < -96 || *timezone > 96)
            return false;
    }
    else
    {
        return false;
    }

    if (slen == 19 || slen == 22)
    {
        if (tm->tm_year < 2000 || tm->tm_year >= 2100)
            return false;
        tm->tm_year -= 1900;
    }
    else
    {
        // 3GPP doesn't define how to interprete 2 digitals of year
        // So, [0-70) -> [2000-2070), [70-99] -> [1970-1999]
        if (tm->tm_year < 100)
            tm->tm_year += (2000 - 1900);
    }

    tm->tm_mon -= 1; // (1-12) -> (0-11)

    // Use normalize feature of library function to validate
    tm->tm_isdst = 0;
    struct tm ntm = *tm;
    time_t t = mktime(&ntm);
    if (t == -1)
        return false;

    if (tm->tm_year != ntm.tm_year ||
        tm->tm_mon != ntm.tm_mon ||
        tm->tm_mday != ntm.tm_mday ||
        tm->tm_hour != ntm.tm_hour ||
        tm->tm_min != ntm.tm_min ||
        tm->tm_sec != ntm.tm_sec)
        return false;

    return true;
}

// 8.15 Clock +CCLK
void atCmdHandleCCLK(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +CCLK=<time>
        bool paramok = true;
        const char *ts = atParamStr(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        struct tm tm;
        memset(&tm, 0x00, sizeof(tm));
        int timezone = 0;
        bool has_zz = false, set_etime = false;
        if (!_inputTime(ts, &tm, &timezone, &has_zz))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
		if((timezone < -48) || (timezone > 56))
		{
			RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
		}
#endif
        if (has_zz)
        {
            gAtSetting.timezone = timezone;
            Cfw_SetTimezone(timezone);
        }

        set_etime = osiSetEpochTime((mktime(&tm) - timezone * CFW_TIME_ZONE_SECOND) * 1000LL);
        if (!set_etime)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        drvRtcUpdateTime();
        atCfgAutoSave();
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
		int timeoffset = 0;
		if(3 == gAtSetting.ctzu)
		{
			timeoffset = gAtSetting.timezone * CFW_TIME_ZONE_SECOND;
		}
#else		
        int timeoffset = gAtSetting.timezone * CFW_TIME_ZONE_SECOND;
#endif
        time_t lt = osiEpochSecond() + timeoffset;
        struct tm ltm;
        gmtime_r(&lt, &ltm);

        char rsp[66];
        char ts[32];
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
		int8_t timezone = (0 == gAtSetting.ctzu)? 0 : gAtSetting.timezone;
		_outputTimeWithZone(ts, &ltm, timezone);
#else
        _outputTimeWithZone(ts, &ltm, gAtSetting.timezone);
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		sprintf(rsp, "+CCLK: \"%s\"", ts);
#else
		sprintf(rsp, "+CCLK: \"20%s\"", ts);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#ifdef CONFIG_ATS_ALARM_SUPPORT
typedef struct
{
    char text[CONFIG_ATS_ALARM_TEXT_LEN + 1];
    uint8_t has_timezone;
    int8_t timezone;
    uint8_t silent;
} atAlarmInfo_t;

static_assert(sizeof(atAlarmInfo_t) <= DRV_RTC_ALARM_INFO_SIZE, "atAlarmInfo_t is too large");

static void _calaAlarmCB(drvRtcAlarm_t *alarm, void *unused)
{
    atAlarmInfo_t *info = (atAlarmInfo_t *)alarm->info;
    char rsp[80];

    if (info->silent)
        sprintf(rsp, "+CALV: %ld", alarm->name);
    else
        sprintf(rsp, "+CALV: %ld, \"%s\"", alarm->name, info->text);
    atCmdRespDefUrcText(rsp);
}

static int _currentWday()
{
    int timeoffset = gAtSetting.timezone * CFW_TIME_ZONE_SECOND;
    time_t lt = osiEpochSecond() + timeoffset;
    struct tm ltm;
    gmtime_r(&lt, &ltm);
    return ltm.tm_wday;
}

// 8.16 Alarm +CALA
void atCmdHandleCALA(atCommand_t *cmd)
{
    bool paramok = true;
    if (cmd->type == AT_CMD_SET)
    {
        // +CALA=<time>[,<n>[,<type>[,<text>[,<recurr>[,<silent>]]]]]
        if (cmd->param_count < 1 || cmd->param_count > 6)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        const char *alarmTime = atParamStr(cmd->params[0], &paramok);
        uint8_t index = atParamDefUintInRange(cmd->params[1], 1, 1, CONFIG_ATS_ALARM_COUNT, &paramok);
        atParamDefUintInRange(cmd->params[2], 0, 0, 0, &paramok);
        const char *alarmText = atParamDefStr(cmd->params[3], "", &paramok);
        const char *alarmRecurr = atParamOptStr(cmd->params[4], &paramok);
        uint8_t silent = atParamDefUintInRange(cmd->params[5], 0, 0, 1, &paramok);
        if ((!paramok) || (strlen(alarmText) > CONFIG_ATS_ALARM_TEXT_LEN))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        bool recurr_present = !atParamIsEmpty(cmd->params[4]);
        size_t alarmTimeLen = strlen(alarmTime);
        if (recurr_present && alarmTimeLen >= 17)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint32_t secInDay = 0, wdaymask = 0;
        int64_t setAlarmSec = 0;
        int setalarmtimezone = 0;
        bool has_utc = false;
        //handle the time --->hh:mm:ss
        if (alarmTimeLen < 17)
        {
            int uRecurr_tmp = 0;
            if ((!paramok) || (strlen(alarmRecurr) > 13))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->param_count < 5)
            {
                uRecurr_tmp = _currentWday(); // What day is it today
                wdaymask |= 1 << uRecurr_tmp;
            }
            else
            {
                if ((alarmRecurr[0] == '0') && (strlen(alarmRecurr) == 1))
                {
                    wdaymask = (uint8_t)0x7f;
                }
                else
                {
                    int k = 0;
                    char *recurr_arr[8];
                    recurr_arr[0] = strtok((char *)alarmRecurr, ",");
                    while (recurr_arr[k] != NULL)
                    {
                        k++;
                        recurr_arr[k] = strtok(NULL, ",");
                    }
                    recurr_arr[k] = "\0";

                    for (int i = 0; i < k; i++)
                    {
                        if (strlen(recurr_arr[i]) != 1)
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                        uRecurr_tmp = atoi(recurr_arr[i]);
                        if ((uRecurr_tmp > 7) || (uRecurr_tmp <= 0))
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                        else if (uRecurr_tmp == 7)
                            uRecurr_tmp = 0;
                        wdaymask |= 1 << uRecurr_tmp;
                    }
                }
            }

            if ((strlen(alarmTime) != 8) && (strlen(alarmTime) != 11))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_DATE_OR_TIME);
            static const char *fmttime1 = "%02d:%02d:%02d";
            static const char *fmttimez1 = "%02d:%02d:%02d%03d";
            int n = 0;
            uint32_t alarm_hour = 0, alarm_minute = 0, alarm_second = 0;
            if ((alarmTime[2] != ':') && (alarmTime[5] != ':'))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_DATE_OR_TIME);

            if (strlen(alarmTime) == 8)
            {
                n = sscanf(alarmTime, fmttime1, &alarm_hour, &alarm_minute, &alarm_second);
                if (n != 3)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_DATE_OR_TIME);
                has_utc = false;
                setalarmtimezone = gAtSetting.timezone;
            }
            else
            {
                n = sscanf(alarmTime, fmttimez1, &alarm_hour, &alarm_minute, &alarm_second, &setalarmtimezone);
                if (n != 4)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_DATE_OR_TIME);
                has_utc = true;
            }
            secInDay = alarm_hour * 60 * 60 + alarm_minute * 60 + alarm_second;
        }
        //handle the time --->yy/mm/dd,hh:mm:ss
        else
        {
            struct tm tm;
            memset(&tm, 0x00, sizeof(tm));
            if (!_inputTime(alarmTime, &tm, &setalarmtimezone, &has_utc))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_DATE_OR_TIME);

            setAlarmSec = mktime(&tm) - setalarmtimezone * CFW_TIME_ZONE_SECOND;
            int64_t curr = osiEpochSecond();
            if (setAlarmSec < curr)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_INVALID_DATE_OR_TIME);
        }

        atAlarmInfo_t info = {};
        strcpy(info.text, alarmText);
        info.has_timezone = has_utc;
        info.timezone = setalarmtimezone;
        info.silent = silent;

        if (alarmTimeLen < 17)
        {
            if (!drvRtcSetRepeatAlarm(AT_ALARM, index, &info, sizeof(info), wdaymask,
                                      secInDay, setalarmtimezone * CFW_TIME_ZONE_SECOND, true))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            if (!drvRtcSetAlarm(AT_ALARM, index, &info, sizeof(info), setAlarmSec, true))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char rsp[80];
        sprintf(rsp, "+CALA: (1-%d),(0),(%d),(15),(0,1)",
                CONFIG_ATS_ALARM_COUNT, CONFIG_ATS_ALARM_TEXT_LEN);

        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        int count = drvRtcGetAlarmCount(AT_ALARM);
        if (count == 0)
            RETURN_OK(cmd->engine);

        char rsp[256];
        drvRtcAlarm_t *alarms = malloc(count * sizeof(drvRtcAlarm_t));
        if (alarms == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        // [+CALA: <time>,<n1>,<type>,[<text>],[<recurr>],<silent>]
        drvRtcGetAlarms(AT_ALARM, alarms, count);
        for (int i = 0; i < count; i++)
        {
            atAlarmInfo_t *info = (atAlarmInfo_t *)alarms[i].info;
            char *prsp = rsp;

            prsp += sprintf(prsp, "+CALA: ");
            if (alarms[i].type == DRV_RTC_ALARM_WDAY_REPEATED)
            {
                unsigned sec_in_day = alarms[i].wday_repeated.sec_in_day;
                unsigned hour = sec_in_day / 3600;
                unsigned min = (sec_in_day % 3600) / 60;
                unsigned sec = sec_in_day % 60;
                if (info->has_timezone)
                    prsp += sprintf(prsp, "\"%02d:%02d:%02d%+03d\"", hour, min, sec, info->timezone);
                else
                    prsp += sprintf(prsp, "\"%02d:%02d:%02d\"", hour, min, sec);
            }
            else
            {
                prsp += sprintf(prsp, "\"");
                struct tm tm;
                if (info->has_timezone)
                {
                    time_t t = alarms[i].expire_sec + info->timezone * CFW_TIME_ZONE_SECOND;
                    gmtime_r(&t, &tm);
                    prsp += _outputTimeWithZone(prsp, &tm, info->timezone);
                }
                else
                {
                    time_t t = alarms[i].expire_sec + gAtSetting.timezone * CFW_TIME_ZONE_SECOND;
                    gmtime_r(&t, &tm);
                    prsp += _outputTime(prsp, &tm);
                }
                prsp += sprintf(prsp, "\"");
            }

            prsp += sprintf(prsp, ",%ld,0,\"%s\"", alarms[i].name, info->text);

            if (alarms[i].type == DRV_RTC_ALARM_WDAY_REPEATED &&
                alarms[i].wday_repeated.wday_mask != 0)
            {
                prsp += sprintf(prsp, ",\"");
                uint8_t wday_mask = alarms[i].wday_repeated.wday_mask;

                // It is tricky for the loop. Mask (0~6) is Sunday, Monday, ..
                // and what we need is (1~7) Monday, ...
                for (int n = 1; n < 8; n++)
                {
                    uint8_t mask = (1 << (n % 7));
                    if ((wday_mask & mask) == 0)
                        continue;

                    wday_mask &= ~mask;
                    if (wday_mask == 0) // last one
                        prsp += sprintf(prsp, "%d\"", n);
                    else
                        prsp += sprintf(prsp, "%d,", n);
                }
            }
            prsp += sprintf(prsp, ",%d", info->silent);
            atCmdRespInfoText(cmd->engine, rsp);
        }

        free(alarms);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}

void atCmdHandleCALD(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +CALD=<n>
        bool paramok = true;
        uint8_t alarm_index = atParamUintInRange(cmd->params[0], 1, CONFIG_ATS_ALARM_COUNT, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (!drvRtcRemoveAlarm(AT_ALARM, alarm_index))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        // +CALD: (list of supported <n>s)
        int count = drvRtcGetAlarmCount(AT_ALARM);
        if (count == 0)
            RETURN_OK(cmd->engine);

        drvRtcAlarm_t *alarms = malloc(count * sizeof(drvRtcAlarm_t));
        if (alarms == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        drvRtcGetAlarms(AT_ALARM, alarms, count);
        char rsp[50];
        char *prsp = rsp;
        prsp += sprintf(prsp, "+CALD: ");
        for (int n = 0; n < count; n++)
        {
            if (n == count - 1)
                prsp += sprintf(prsp, "%ld", alarms[n].name);
            else
                prsp += sprintf(prsp, "%ld,", alarms[n].name);
        }

        atCmdRespInfoText(cmd->engine, rsp);

        free(alarms);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
}
#endif

void atAlarmInit()
{
#ifdef CONFIG_ATS_ALARM_SUPPORT
    drvRtcAlarmOwnerSetCB(AT_ALARM, NULL, _calaAlarmCB);
#endif
}

// 8.41 Time zone reporting +CTZR
void atCmdHandleCTZR(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +CTZR=[<reporting>]
        bool paramok = true;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        uint8_t mode = atParamDefUintInRange(cmd->params[0], 0, 0, 2, &paramok);
#else
        uint8_t mode = atParamDefUintInRange(cmd->params[0], 0, 0, 3, &paramok);
#endif
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        uint8_t nSim = atCmdGetSim(cmd->engine);
        atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
        if(gAtSetting.ctzr != mode)
        {
            gAtSetting.ctzr = mode;
            gAtSettingSave.ctzr = mode;
            if (atCfgSave(0, chsetting))
            {
            	atCfgAutoSave();
            	CFW_nvmWriteStatic(nSim);
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
#else
        gAtSetting.ctzr = mode;
#endif
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        atCmdRespInfoText(cmd->engine, "+CTZR: (0-2)");
#else
        atCmdRespInfoText(cmd->engine, "+CTZR: (0,1,2,3)");
#endif
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        sprintf(rsp, "+CTZR: %d", gAtSetting.ctzr);
#else
        sprintf(rsp, "+CTZR:%d", gAtSetting.ctzr);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

// E: Command echo
void atCmdHandleE(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    if (cmd->type == AT_CMD_EXE)
    {
        bool paramok = true;
        uint8_t ate = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
		
#if QUEC_AT_CONFIG_ALL_CHANNEL
		osiSlistHead_t *list = atDispatchGetList();
		osiSlistItem_t *item;
		SLIST_FOREACH(item, list, iter)
		{
			atDispatch_t *dispatch = (atDispatch_t *)item;
			chsetting = atCmdChannelSetting(atDispatchGetCmdEngine(dispatch));
			chsetting->ate = ate;
		}
#else
        chsetting->ate = ate;
#endif
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

// V: DCE response format
void atCmdHandleV(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    if (cmd->type == AT_CMD_EXE)
    {
        bool paramok = true;
        uint8_t atv = atParamDefUintInRange(cmd->params[0], 1, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
		
#if QUEC_AT_CONFIG_ALL_CHANNEL
		osiSlistHead_t *list = atDispatchGetList();
		osiSlistItem_t *item;
		SLIST_FOREACH(item, list, iter)
		{
			atDispatch_t *dispatch = (atDispatch_t *)item;
			chsetting = atCmdChannelSetting(atDispatchGetCmdEngine(dispatch));
			chsetting->atv = atv;
		}
#else
        chsetting->atv = atv;
#endif
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

// Q: Result code suppression
void atCmdHandleQ(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    if (cmd->type == AT_CMD_EXE)
    {
        bool paramok = true;
        uint8_t atq = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#if QUEC_AT_CONFIG_ALL_CHANNEL
		osiSlistHead_t *list = atDispatchGetList();
		osiSlistItem_t *item;
		SLIST_FOREACH(item, list, iter)
		{
			atDispatch_t *dispatch = (atDispatch_t *)item;
			chsetting = atCmdChannelSetting(atDispatchGetCmdEngine(dispatch));
			chsetting->atq = atq;
		}
#else
		chsetting->atq = atq;
#endif
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

// X: Result code selection and call progress monitoring control
void atCmdHandleX(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    if (cmd->type == AT_CMD_EXE)
    {
        if (cmd->param_count == 0)
            RETURN_OK(cmd->engine);

        bool paramok = true;
        uint8_t atx = atParamUintInRange(cmd->params[0], 0, 4, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        CFW_CfgSetConnectResultFormatCode(atx); // Really needed??

#if QUEC_AT_CONFIG_ALL_CHANNEL
		osiSlistHead_t *list = atDispatchGetList();
		osiSlistItem_t *item;
		SLIST_FOREACH(item, list, iter)
		{
			atDispatch_t *dispatch = (atDispatch_t *)item;
			chsetting = atCmdChannelSetting(atDispatchGetCmdEngine(dispatch));
			chsetting->atx = atx;
		}
#else		
        chsetting->atx = atx;
#endif
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// Z: Reset to default configuration
void atCmdHandleZ(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    if (cmd->type == AT_CMD_EXE)
    {
        bool paramok = true;
        uint8_t profile = atParamDefUintInRange(cmd->params[0], 0, 0, 0, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        quec_at_setting_read();
#endif
        if (!atCfgResetProfile(profile, chsetting))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// 6.3.8 Automatic answer
void atCmdHandleS0(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		sprintf(rsp, "%.3u", gAtSetting.sim[nSim].s0);
#else
        sprintf(rsp, "%u", gAtSetting.sim[nSim].s0);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t s0 = atParamDefUintInRange(cmd->params[0], 0, 0, 255, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.sim[nSim].s0 = s0;
        RETURN_OK(cmd->engine);
    }
    if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "0-255");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// S3: Command line termination character
void atCmdHandleS3(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);

    if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		sprintf(rsp, "%.3u", chsetting->s3);
#else
        sprintf(rsp, "%u", chsetting->s3);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t s3 = atParamDefUintInRange(cmd->params[0], 0x0d, 0, 31, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#if QUEC_AT_CONFIG_ALL_CHANNEL
		osiSlistHead_t *list = atDispatchGetList();
		osiSlistItem_t *item;
		SLIST_FOREACH(item, list, iter)
		{
			atDispatch_t *dispatch = (atDispatch_t *)item;
			chsetting = atCmdChannelSetting(atDispatchGetCmdEngine(dispatch));
			chsetting->s3 = s3;
		}
#else
        chsetting->s3 = s3;
#endif
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// S4: Response formatting character
void atCmdHandleS4(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);

    if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		sprintf(rsp, "%.3u", chsetting->s4);
#else
        sprintf(rsp, "%u", chsetting->s4);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t s4 = atParamDefUintInRange(cmd->params[0], 0x0a, 0, 31, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#if QUEC_AT_CONFIG_ALL_CHANNEL
		osiSlistHead_t *list = atDispatchGetList();
		osiSlistItem_t *item;
		SLIST_FOREACH(item, list, iter)
		{
			atDispatch_t *dispatch = (atDispatch_t *)item;
			chsetting = atCmdChannelSetting(atDispatchGetCmdEngine(dispatch));
			chsetting->s4 = s4;
		}
#else
        chsetting->s4 = s4;
#endif
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// S5: Command line editing character
void atCmdHandleS5(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);

    if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		sprintf(rsp, "%.3u", chsetting->s5);
#else
        sprintf(rsp, "%u", chsetting->s5);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t s5 = atParamDefUintInRange(cmd->params[0], 0x08, 0, 31, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#if QUEC_AT_CONFIG_ALL_CHANNEL
		osiSlistHead_t *list = atDispatchGetList();
		osiSlistItem_t *item;
		SLIST_FOREACH(item, list, iter)
		{
			atDispatch_t *dispatch = (atDispatch_t *)item;
			chsetting = atCmdChannelSetting(atDispatchGetCmdEngine(dispatch));
			chsetting->s5 = s5;
		}
#else
        chsetting->s5 = s5;
#endif
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void atCmdHandleICF(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
        atDispatch_t* dispatch;
        uint32_t uart_dev;

        dispatch = atCmdGetDispatch(cmd->engine);
        uart_dev = quec_get_dev_by_dispatch(dispatch);
        if(uart_dev == QL_MIAN_UART_PORT)
        {
#endif
            bool paramok = true;
#ifndef CONFIG_SOC_8910
            uint8_t format = atParamDefUintInRange(cmd->params[0], AT_DEVICE_FORMAT_8N1, 1, 6, &paramok);
#else
            //8910 ARM UART hardware not support 7bit Data,so format parm just support 8n2 1,811 2,8n1 3
            uint8_t format = atParamDefUintInRange(cmd->params[0], AT_DEVICE_FORMAT_8N1, 1, 3, &paramok);
#endif
#ifndef CONFIG_SOC_8910
            uint8_t parity = atParamDefUintInRange(cmd->params[1], AT_DEVICE_PARITY_SPACE, 0, 3, &paramok);
#else
            //8910 ARM UART hardware not support Mark Space,so parity parm just support odd 0 even 1
            uint8_t parity = atParamDefUintInRange(cmd->params[1], AT_DEVICE_PARITY_ODD, 0, 1, &paramok);
#endif
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            gAtSetting.icf_format = format;
            gAtSetting.icf_parity = parity;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART            
            quec_uart_current_setting[QL_MIAN_UART_PORT].icf_format = format;
            quec_uart_current_setting[QL_MIAN_UART_PORT].icf_parity = parity;
#endif 
            atCmdDeviceSetFormatNeeded(cmd->engine);
            atCmdRespOK(cmd->engine);
        #ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
        }
        else 
        {
            atCmdRespOK(cmd->engine);
        }
#endif    
    }
    else if (cmd->type == AT_CMD_TEST)
    {
#ifndef CONFIG_SOC_8910
        atCmdRespInfoText(cmd->engine, "+ICF:(1-6),(0-3)");
#else
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		atCmdRespInfoText(cmd->engine, "+ICF: (1-3),(0,1)");
#else
        atCmdRespInfoText(cmd->engine, "+ICF:(1-3),(0-1)");
#endif
#endif
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		sprintf(rsp, "+ICF: %d,%d", gAtSetting.icf_format, gAtSetting.icf_parity);
#else
        sprintf(rsp, "+ICF: %d, %d", gAtSetting.icf_format, gAtSetting.icf_parity);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleIPR(atCommand_t *cmd)
{
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    static const uint32_t list[] = {0, 2400, 4800, 9600, 14400, 19200,
                                    28800, 33600, 38400, 57600, 115200,
                                    230400, 250000, 460800, 500000, 921600, 1000000};
#else
    static const uint32_t list[] = {0, 1200, 2400, 4800, 9600, 14400, 19200,
                                    28800, 33600, 38400, 57600, 115200,
                                    230400, 460800, 921600};
#endif
    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
        atDispatch_t* dispatch;
        uint32_t uart_dev;

        dispatch = atCmdGetDispatch(cmd->engine);
        uart_dev = quec_get_dev_by_dispatch(dispatch);
        if(uart_dev == QL_MIAN_UART_PORT)
        {
#endif
#ifdef CONFIG_AT_IPR_SUPPORT_ANDW
            bool paramok = true;
            const char *pars = atParamRawText(cmd->params[0], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            int pars_len = strlen(pars);
            bool save_needed = false;
            if (pars_len >= 2 && pars[pars_len - 2] == '&' && OSI_CHAR_CASE_EQU(pars[pars_len - 1], 'W'))
            {
                save_needed = true;
                atParamTrimTail(cmd->params[0], 2);
            }

            uint32_t baud = atParamUintInList(cmd->params[0], list, sizeof(list) / sizeof(list[0]), &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            gAtSetting.ipr = baud;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART            
            quec_uart_current_setting[QL_MIAN_UART_PORT].baudrate = baud;
            quec_main_uart_cbaud = baud;
#endif            
            if (save_needed)
            {
                atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
#ifdef CONFIG_QUEC_PROJECT_FEATURE                
                gAtSettingSave.ipr = baud;
#endif
                atCfgSave(gAtSetting.profile, chsetting);
            }

            atCmdDeviceSetBaudNeeded(cmd->engine);
            atCmdRespOK(cmd->engine);
#else
            bool paramok = true;
            uint32_t baud = atParamUintInList(cmd->params[0], list, sizeof(list), &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            gAtSetting.ipr = baud;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART            
            quec_uart_current_setting[QL_MIAN_UART_PORT].baudrate = baud;
            quec_main_uart_cbaud = baud;
#endif
            atCmdDeviceSetBaudNeeded(cmd->engine);
            atCmdRespOK(cmd->engine);
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
        }
        else 
        {
            atCmdRespOK(cmd->engine);
        }
#endif
    }
    else if (cmd->type == AT_CMD_TEST)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        atCmdRespInfoText(cmd->engine, "+IPR: (2400,4800,9600,14400,19200,28800,33600,38400,57600,115200,230400,460800),(2400,4800,9600,14400,19200,28800,33600,38400,57600,115200,230400,250000,460800,500000,921600,1000000)");
#else
        atCmdRespInfoText(cmd->engine, "+IPR: (0,1200,2400,4800,9600,14400,19200,28800,33600,38400,57600,115200,230400,460800,921600)");
#endif
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
        sprintf(rsp, "+IPR: %lu", gAtSetting.ipr);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleIFC(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
        atDispatch_t* dispatch;
        uint32_t uart_dev;

        dispatch = atCmdGetDispatch(cmd->engine);
        uart_dev = quec_get_dev_by_dispatch(dispatch);
        if(uart_dev == QL_MIAN_UART_PORT)
        {
#endif
            static const uint32_t list[] = {0, 2};
            bool paramok = true;
            uint8_t rxfc = atParamDefUintInList(cmd->params[0], 0, list, 2, &paramok);
            uint8_t txfc = atParamDefUintInList(cmd->params[1], 0, list, 2, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            gAtSetting.ifc_rx = rxfc;
            gAtSetting.ifc_tx = txfc;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART            
            quec_uart_current_setting[QL_MIAN_UART_PORT].ifc_rx = rxfc;
            quec_uart_current_setting[QL_MIAN_UART_PORT].ifc_tx = txfc;
#endif    
            atCmdDeviceSetIfcNeeded(cmd->engine);
            atCmdRespOK(cmd->engine);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
        }
        else 
        {
            atCmdRespOK(cmd->engine);
        }
#endif
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
        sprintf(rsp, "+IFC: %d,%d", gAtSetting.ifc_rx, gAtSetting.ifc_tx);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+IFC: (0,2),(0,2)");
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// &D: Circuit 108/2 (DTR) behavior
void atCmdHandleAndD(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);

    if (cmd->type == AT_CMD_EXE)
    {
        bool paramok = true;
        uint8_t value = atParamDefUintInRange(cmd->params[0], 1, 0, 2, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
		
#if QUEC_AT_CONFIG_ALL_CHANNEL
		osiSlistHead_t *list = atDispatchGetList();
	    osiSlistItem_t *item;
	    SLIST_FOREACH(item, list, iter)
	    {
	        atDispatch_t *dispatch = (atDispatch_t *)item;
			chsetting = atCmdChannelSetting(atDispatchGetCmdEngine(dispatch));
			chsetting->andd = value;
	    }
#else
        chsetting->andd = value;
#endif
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// &W: Store current profile
void atCmdHandleAndW(atCommand_t *cmd)
{
    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_EXE)
    {
        bool paramok = true;
        uint8_t profile = atParamDefUintInRange(cmd->params[0], 0, 0,
                                                0, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#ifdef CONFIG_QUEC_PROJECT_FEATURE        
        memcpy(&gAtSettingSave, &gAtSetting, sizeof(atSetting_t));
#endif
        if (atCfgSave(profile, chsetting))
        {
            drvRtcUpdateTime();
            atCfgAutoSave();
#ifdef CONFIG_QUEC_PROJECT_FEATURE
            quec_at_setting_save();              
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
            quec_uart_setting_save();
#endif
            CFW_nvmWriteStatic(nSim);
            atCmdRespOK(cmd->engine);
        }
        else
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// &Y: Set default reset profile
void atCmdHandleAndY(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        bool paramok = true;
        uint8_t profile = atParamDefUintInRange(cmd->params[0], 0, 0,
                                                CONFIG_ATR_PROFILE_COUNT - 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (gAtSetting.profile != profile)
        {
            gAtSetting.profile = profile;
            if (!atCfgGlobalSave())
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// &F: Set to factory defined configuration
void atCmdHandleAndF(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        bool paramok = true;
        uint8_t value = atParamDefUintInRange(cmd->params[0], 0, 0, 0, &paramok);

        (void)value;
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#ifdef CONFIG_AT_AUDIO_SUPPORT
        audevResetSettings();
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        quec_at_setting_reset_default();
#endif
        if (!atCfgResetFactory(atCmdChannelSetting(cmd->engine)))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// 8.1 Phone activity status
void atCmdHandleCPAS(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    char rsp[32];

    if (cmd->type == AT_CMD_EXE)
    {
        uint32_t status = atCfwPhoneActiveStatus(nSim);
        if (status == AT_ACTIVE_STATUS_READY ||
            status == AT_ACTIVE_STATUS_UNAVAILABLE ||
            status == AT_ACTIVE_STATUS_UNKNOWN ||
            status == AT_ACTIVE_STATUS_RINGING ||
            status == AT_ACTIVE_STATUS_CALL_IN_PROGRESS)
        {
            sprintf(rsp, "+CPAS: %u", (int)status);
            atCmdRespInfoText(cmd->engine, rsp);
            atCmdRespOK(cmd->engine);
        }
        else
        {
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CPAS: (0,3,4)");
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void atCmdHandleCIND(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    char rsp[64];

    if (cmd->type == AT_CMD_SET) // +CIND=[<ind>[,<ind>[,...]]]
    {
        bool paramok = true;
        uint8_t battchg = atParamDefUintInRange(cmd->params[0], gAtSetting.cind_battchg,
                                                0, 5, &paramok);
        uint8_t signal = atParamDefUintInRange(cmd->params[1], gAtSetting.sim[nSim].cind_signal,
                                               0, 5, &paramok);
        uint8_t service = atParamDefUintInRange(cmd->params[2], gAtSetting.sim[nSim].cind_service,
                                                0, 1, &paramok);
        uint8_t sounder = atParamDefUintInRange(cmd->params[3], gAtSetting.sim[nSim].cind_sounder,
                                                0, 1, &paramok);
        uint8_t message = atParamDefUintInRange(cmd->params[4], gAtSetting.sim[nSim].cind_message,
                                                0, 1, &paramok);
        uint8_t call = atParamDefUintInRange(cmd->params[5], gAtSetting.sim[nSim].cind_call,
                                             0, 1, &paramok);
        uint8_t roam = atParamDefUintInRange(cmd->params[6], gAtSetting.sim[nSim].cind_roam,
                                             0, 1, &paramok);
        uint8_t smsfull = atParamDefUintInRange(cmd->params[7], gAtSetting.sim[nSim].cind_smsfull,
                                                0, 1, &paramok);
        if (!paramok || cmd->param_count > 8)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.cind_battchg = battchg;
        gAtSetting.sim[nSim].cind_signal = signal;
        gAtSetting.sim[nSim].cind_service = service;
        gAtSetting.sim[nSim].cind_sounder = sounder;
        gAtSetting.sim[nSim].cind_message = message;
        gAtSetting.sim[nSim].cind_call = call;
        gAtSetting.sim[nSim].cind_roam = roam;
        gAtSetting.sim[nSim].cind_smsfull = smsfull;
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
#if 0
        atCindValue_t cind = atCfwGetIndicator(nSim);
        sprintf(rsp, "+CIND: %u,%u,%u,%d,%u,%d,%u,%u",
                cind.battchg, cind.signal, cind.service,
                cind.sounder, cind.message, cind.call,
                cind.roam, cind.smsfull);
#else
        sprintf(rsp, "+CIND: %u,%u,%u,%d,%u,%d,%u,%u",
                gAtSetting.cind_battchg, gAtSetting.sim[nSim].cind_signal, gAtSetting.sim[nSim].cind_service,
                gAtSetting.sim[nSim].cind_sounder, gAtSetting.sim[nSim].cind_message, gAtSetting.sim[nSim].cind_call,
                gAtSetting.sim[nSim].cind_roam, gAtSetting.sim[nSim].cind_smsfull);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		atCmdRespInfoText(cmd->engine, "+CIND: "
                                       "(\"battchg\",(0-5)),"
                                       "(\"signal\",(0-5)),"
                                       "(\"service\",(0,1)),"
                                       "(\"sounder\",(0,1)),"
                                       "(\"message\",(0,1)),"
                                       "(\"call\",(0,1)),"
                                       "(\"roam\",(0,1)),"
                                       "(\"smsfull\",(0,1))");
#else
        atCmdRespInfoText(cmd->engine, "+CIND: "
                                       "(\"battchg\",(0-5)),"
                                       "(\"signal\",(0-5)),"
                                       "(\"service\",(0-1)),"
                                       "(\"sounder\",(0-1)),"
                                       "(\"message\",(0-1)),"
                                       "(\"call\",(0-1)),"
                                       "(\"roam\",(0-1)),"
                                       "(\"smsfull\",(0-1))");
#endif
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}
#ifndef CONFIG_QUEC_PROJECT_FEATURE
bool utiImeiIsValidate(const char *Number, uint8_t Len)
{
    uint8_t i, iLength;

    iLength = strlen(Number);
    if (Len != iLength)
    {
        return false;
    }
    for (i = 0; i < iLength; i++)
    {
        if (!(Number[i] >= '0' && Number[i] <= '9'))
        {
            return false;
        }
    }

    if (iLength == 14)
    {
        return true;
    }
    if (iLength == 15) //luhn algorithm
    {
        uint32_t sum = 0, v = 0;

        for (i = 0; i < iLength; i++)
        {
            if ((i & 1) == 0)
            {
                sum += Number[i] - '0';
            }
            else
            {
                v = 2 * (Number[i] - '0');
                sum += (v > 9) ? (v - 9) : v;
            }
        }
        return ((sum % 10) == 0);
    }
    return false;
}
char utiAddImeiCheckSum(const char *Number, uint8_t Len)
{
    uint8_t i, iLength;

    iLength = strlen(Number);
    if (Len != iLength || iLength != 14)
    {
        return 0;
    }
    for (i = 0; i < iLength; i++)
    {
        if (!(Number[i] >= '0' && Number[i] <= '9'))
        {
            return 0;
        }
    }

    uint32_t sum = 0, v = 0;
    for (i = 0; i < iLength; i++)
    {
        if ((i & 1) == 0)
        {
            sum += Number[i] - '0';
        }
        else
        {
            v = 2 * (Number[i] - '0');
            sum += (v > 9) ? (v - 9) : v;
        }
    }
    sum = (sum * 9) % 10;
    return (char)(sum + '0');
}
void atCmdHandleEGMR(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+EGMR: (1,2),(7)");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t command = atParamUintInRange(cmd->params[0], 1, 2, &paramok);
        uint8_t recvtype = atParamUintInRange(cmd->params[1], 7, 7, &paramok);

        (void)recvtype;
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (command == 2)
        {
            if (cmd->param_count > 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            uint8_t nImei[16] = {};
            int nImeiLen = nvmReadImei(nSim, (nvmImei_t *)nImei);
            if (nImeiLen == -1)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            atCmdRespInfoNText(cmd->engine, (char *)nImei, nImeiLen);
            RETURN_OK(cmd->engine);
        }
        else // command == 1
        {
            const char *imei = atParamStr(cmd->params[2], &paramok);
            uint8_t iLength = strlen(imei);

            if (!paramok || cmd->param_count > 3 || ((iLength != 14) && (iLength != 15)))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            if (!utiImeiIsValidate(imei, iLength))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
#ifndef CONFIG_SOC_6760
            fsRemountFactory(0);
#endif
            int nImeiLen = 0;
            if (iLength == 14)
            {
                char iCheckSum[15] = {0};
                memcpy(iCheckSum, imei, 14);
                iCheckSum[14] = utiAddImeiCheckSum(imei, 14);
                nImeiLen = nvmWriteImei(nSim, (nvmImei_t *)iCheckSum);
            }
            else
            {
                nImeiLen = nvmWriteImei(nSim, (const nvmImei_t *)imei);
            }
#ifndef CONFIG_SOC_6760
            fsRemountFactory(MS_RDONLY);
#endif
            if (nImeiLen == -1)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            RETURN_OK(cmd->engine);
        }
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}
#endif
void atCmdHandleCSCLK(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t csclk = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.csclk = csclk;
        bool auto_sleep = (csclk == 2);
#ifdef CONFIG_SRV_DTR_ENABLE
        if (csclk == 1 && srvDtrIsHigh())
            auto_sleep = true;
#endif

#ifdef CONFIG_SOC_8910
        CFW_EnableLpsLock(csclk == 0, atCmdGetSim(cmd->engine));
#endif

        atEngineSetDeviceAutoSleep(auto_sleep);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CSCLK: (0,1,2)");
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
        sprintf(rsp, "%s: %u", cmd->desc->name, gAtSetting.csclk);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// 8.8 Display control
void atCmdHandleCDIS(atCommand_t *cmd)
{
    static char fake_text[80 + 1];

    if (cmd->type == AT_CMD_SET)
    {
        // +CDIS=[<text>[,<text>[,...]]]
        bool paramok = true;
        const char *text = atParamDefStr(cmd->params[0], "", &paramok);
        if (!paramok || cmd->param_count > 1 || strlen(text) > 80)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        strcpy(fake_text, text);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        // +CDIS: <length>[,<length>[,...]]
        atCmdRespInfoText(cmd->engine, "+CDIS: 80");
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        // +CDIS: <text>[,<text>[,...]]
        char rsp[128];
        sprintf(rsp, "+CDIS: \"%s\"", fake_text);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

// 8.40 Automatic time zone update +CTZU
void atCmdHandleCTZU(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +CTZU=[<onoff>]
        bool paramok = true;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        uint8_t mode = atParamDefIntInRange(cmd->params[0], 0, 0, 3, &paramok);
        if (!paramok || (cmd->param_count > 1) || (mode == 2))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t nSim = atCmdGetSim(cmd->engine);
        atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
        if(gAtSetting.ctzu != mode)
        {
            gAtSetting.ctzu = mode;
            gAtSettingSave.ctzu = mode;
            if (atCfgSave(0, chsetting))
            {
            	atCfgAutoSave();
            	CFW_nvmWriteStatic(nSim);
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
#else
        uint8_t mode = atParamDefIntInRange(cmd->params[0], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.ctzu = mode;
#endif        
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        atCmdRespInfoText(cmd->engine, "+CTZU: (0,1,3)");
#else
        atCmdRespInfoText(cmd->engine, "+CTZU:(0, 1)");
#endif
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        sprintf(rsp, "+CTZU: %d", gAtSetting.ctzu);
#else
        sprintf(rsp, "+CTZU:%d", gAtSetting.ctzu);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#ifdef CONFIG_AT_AUDIO_SUPPORT
void atCmdHandleCMUT(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +CMUT=<mute>
        bool paramok = true;
        uint8_t mute = atParamIntInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (!audevSetVoiceUplinkMute(mute))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char rsp[48];
        sprintf(rsp, "%s: (0,1)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        bool mute = audevIsVoiceUplinkMute();
        char rsp[48];
        sprintf(rsp, "%s: %d", cmd->desc->name, mute);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

// 8.23 Loudspeaker volume level
void atCmdHandleCLVL(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +CLVL=<level>
        bool paramok = true;
        uint8_t level = atParamIntInRange(cmd->params[0], 0, 100, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        audevSetVoiceVolume(level);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CLVL: (0-100)");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        unsigned level = audevGetVoiceVolume();

        char rsp[32];
        sprintf(rsp, "+CLVL: %d", level);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

// Set receiver, headset and loudspeaker channel
void atCmdHandleAUDCH(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +AUDCH=<ch>
#ifdef CONFIG_SOC_8910
        if (cmd->param_count != 1 && cmd->param_count != 2)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
#else
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
#endif

        bool paramok = true;
        uint8_t audch = atParamIntInRange(cmd->params[0], 0, 2, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#ifdef CONFIG_SOC_8910
        if (cmd->param_count == 2)
        {
            //Dual Mic is not support on 8910
            static const uint32_t list[] = {0, 1, 3, 4};
            uint8_t inputType = atParamUintInList(cmd->params[1], list, 4, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            audevSetOutput((audevOutput_t)audch);
            audevSetInput((audevInput_t)inputType);
            atCmdRespOK(cmd->engine);
        }
        else
#endif
        {
            audevSetOutput((audevOutput_t)audch);
            atCmdRespOK(cmd->engine);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
#ifdef CONFIG_SOC_8910
        atCmdRespInfoText(cmd->engine, "+AUDCH: (0-2),(0,1,3,4)");
#else
        atCmdRespInfoText(cmd->engine, "+AUDCH: (0-2)");
#endif
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
#ifdef CONFIG_SOC_8910
        audevOutput_t outdev = audevGetOutput();
        audevInput_t indev = audevGetInput();
        sprintf(rsp, "+AUDCH: %d,%d", outdev, indev);
#else
        audevOutput_t outdev = audevGetOutput();
        sprintf(rsp, "+AUDCH: %d", outdev);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
#endif

void atCmdHandleSPENGMD(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        atParamDefUintInRange(cmd->params[0], 1, 1, 1, &paramok); //uint8_t oper
        atParamDefUintInRange(cmd->params[1], 8, 8, 8, &paramok); //uint8_t maintype --> PS_MEM_DEINIT_ASSERT
        atParamDefUintInRange(cmd->params[2], 1, 1, 1, &paramok); //uint8_t subtype

        if (!paramok || cmd->param_count > 3)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        atCmdRespOK(cmd->engine);
        osiPanic();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+SPENGMD: 1,8,1");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32] = {
            0x00,
        };
        strcpy(rsp, "+SPENGMD: 1,8,1");
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleUpTime(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        int64_t ms = osiUpTime();
        char resp[50];
        sprintf(resp, "^UPTIME: %lld", ms);
        atCmdRespInfoText(cmd->engine, resp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void atCmdHandleUpTimePlus(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        int64_t ms = osiUpTime();
        char resp[50];
        sprintf(resp, "+UPTIME: %lld", ms);
        atCmdRespInfoText(cmd->engine, resp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#ifdef CONFIG_SOC_6760
extern void nbiot_nvSetNoLock();
extern bool nbiot_nvIsInputNumber(const char input[]);
extern int nvmWriteStatic(void);
extern void nbiot_nvSetLockCell(uint16_t pci, uint32_t earfcn, int8_t offset);
extern void nbiot_nvSetLockFreqList(uint8_t freq_num, uint32_t a_earfcn[], int8_t a_offset[]);
extern void nbiot_nvGetLockFreqList(uint8_t *pfreq_num, uint32_t a_earfcn[], int8_t a_offset[], uint16_t *ppci);
extern void nbiot_nvSetFreqList(uint8_t freq_num, uint32_t a_earfcn[], int8_t a_offset[]);
extern void nbiot_nvGetFreqList(uint8_t *p_freq_num, uint32_t a_earfcn[], int8_t a_offset[]);
void atCmdHandleLockFreq(atCommand_t *cmd)
{
    uint8_t freq_num = 0;
    uint32_t a_earfcn[10];
    int8_t a_offset[10];
    uint8_t uIndex = 0;
    uint8_t uNvIndex = 0;
    uint32_t arfcn = 0;
    uint8_t arfcnCount = 0;
    uint8_t mode = 0;
    int8_t offset = 0;
    const uint16_t invalidU16 = 0xffff;
    uint16_t cellId = invalidU16;
    char str[256];
    uint8_t uPrintLen = 0;

    OSI_LOGI(0, "atCmdHandleLockFreq!!!\n");

    if (cmd == NULL)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_MEMORY_FAILURE);
    }
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        /* check para count */
        if (1 > cmd->param_count)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        uIndex = 0;
        mode = atParamUintInRange(cmd->params[uIndex], 0, 2, &paramok);
        if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value) || (2 < mode))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        /*Don't lock freq*/
        if (0 == mode)
        {
            OSI_LOGI(0, "atCmdHandleLockFreq: %d close.\n", mode);
            nbiot_nvSetNoLock();
            nvmWriteStatic();
            AT_CMD_RETURN(atCmdRespOK(cmd->engine));
        }
        /*set locked cell*/
        if (1 == mode)
        {
            if (4 != cmd->param_count)
            {
                OSI_LOGI(0, "atCmdHandleLockFreq: lock cell param error,uParaCount = %d, mode = %d.\n", cmd->param_count, mode);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            /*get locked arfcn*/
            uIndex++;
            arfcn = atParamUint(cmd->params[uIndex], &paramok);
            if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            /*NB-IoT channel number offset(-20,-18,-16,-14,-12,-10,-8,-6,-4,-2,-1,0,2,4,6,8,10,12,14,16,18),  offset value is 20*/ /*get offset*/
            uIndex++;
            offset = atParamUintInRange(cmd->params[uIndex], 0, 38, &paramok);
            if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            /*get locked pci*/
            uIndex++;
            cellId = atParamUintInRange(cmd->params[uIndex], 0, 503, &paramok);
            if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            OSI_LOGI(0, "atCmdHandleLockFreq:freq %d pci %d \n", arfcn, cellId);
            nbiot_nvSetLockCell(cellId, arfcn, offset);
            nvmWriteStatic();
            AT_CMD_RETURN(atCmdRespOK(cmd->engine));
        }
        /*set locked arfcn*/
        if (2 == mode)
        {
            if (0 != (cmd->param_count % 2))
            {
                OSI_LOGI(0, "atCmdHandleLockFreq: lock freq param error,uParaCount = %d, mode = %d.\n", cmd->param_count, mode);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            /*get arfcnCount*/
            uIndex++;
            arfcnCount = atParamUint(cmd->params[uIndex], &paramok);
            if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            if ((arfcnCount * 2 + 2) != cmd->param_count)
            {
                OSI_LOGI(0, "atCmdHandleLockFreq: lock freq param error,uParaCount = %d, arfcnCount = %d.\n", cmd->param_count, arfcnCount);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            OSI_LOGI(0, "atCmdHandleLockFreq, lock freq uParaCount = %d, arfcnCount = %d.\n", cmd->param_count, arfcnCount);
            for (uNvIndex = 0; uNvIndex < arfcnCount; uNvIndex++)
            {
                /*get arfcn*/
                uIndex++;
                arfcn = atParamUint(cmd->params[uIndex], &paramok);
                if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value))
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                /*get offset*/
                uIndex++;
                offset = atParamUintInRange(cmd->params[uIndex], 0, 38, &paramok);
                if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value))
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                /*NB-IoT channel number offset(-20,-18,-16,-14,-12,-10,-8,-6,-4,-2,-1,0,2,4,6,8,10,12,14,16,18),  offset value is 20*/
                OSI_LOGI(0, "atCmdHandleLockFreq, lock freq uParaCount = %d, arfcn = %d, offset = %d.\n", cmd->param_count, arfcn, offset);
                a_earfcn[uNvIndex] = arfcn;
                a_offset[uNvIndex] = offset - 20;
            }
            if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            nbiot_nvSetLockFreqList(arfcnCount, a_earfcn, a_offset);
            nvmWriteStatic();
            AT_CMD_RETURN(atCmdRespOK(cmd->engine));
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        OSI_LOGI(0, "atCmdHandleLockFreq, type = AT_CMD_TEST.\n");
        memset(str, 0, sizeof(str));
        sprintf(str, "+NVSETLOCKFREQ: \n 0: close LOCKFREQ \n 1:Lock cell(1,cell_fcn,offset,cell_pci) \n 2:Lock fcn(2,fcn_Num(1-9),fcn(fcn,offset),..)");
        atCmdRespInfoText(cmd->engine, str);
        AT_CMD_RETURN(atCmdRespOK(cmd->engine));
    }
    else if (AT_CMD_READ == cmd->type)
    {
        // output debug information
        OSI_LOGI(0, "atCmdHandleLockFreq read.\n");
        /*offset is useless at LTE_230 project,set default*/
        memset(a_offset, 0, 10);
        nbiot_nvGetLockFreqList(&freq_num, a_earfcn, a_offset, &cellId);
        if (0 == freq_num)
        {
            memset(str, 0, sizeof(str));
            strcpy(str, "LOCKFREQ FUNCTION CLOSED!");
            atCmdRespInfoText(cmd->engine, str);
            AT_CMD_RETURN(atCmdRespOK(cmd->engine));
        }
        if (invalidU16 != cellId)
        {
            memset(str, 0, sizeof(str));
            uPrintLen = sprintf(str, "lock cell: freq(%lu %d), pci(%u)",
                                a_earfcn[0],
                                a_offset[0],
                                cellId);
            atCmdRespInfoText(cmd->engine, str);
            AT_CMD_RETURN(atCmdRespOK(cmd->engine));
        }
        memset(str, 0, sizeof(str));
        uPrintLen = sprintf(str, "lock freqNum %d in total:", freq_num);
        for (uNvIndex = 0; uNvIndex < freq_num; uNvIndex++)
        {
            uPrintLen += sprintf(str + uPrintLen, "(%lu %d) ",
                                 a_earfcn[uNvIndex],
                                 a_offset[uNvIndex]);
        }
        atCmdRespInfoText(cmd->engine, str);
        AT_CMD_RETURN(atCmdRespOK(cmd->engine));
    }
    else
    {
        OSI_LOGI(0, "atCmdHandleLockFreq, error.\n");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void atCmdHandleCfgArfcn(atCommand_t *cmd)
{

    uint8_t freq_num = 0;
    uint32_t a_earfcn[10];
    int8_t a_offset[10];
    uint8_t uIndex = 0;
    uint8_t uNvIndex = 0;
    uint32_t arfcn = 0;
    uint8_t arfcnCount = 0;
    int8_t offset = 0;
    char str[256];
    uint8_t uPrintLen = 0;

    OSI_LOGI(0, "AT_GC_CmdFunc_NVCFGARFCN!!!\n");

    /*offset is useless at LTE_230 project,set default*/
    memset(a_offset, 0, 10);

    if (cmd == NULL)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_MEMORY_FAILURE);
    }
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        /* check para count */
        if (1 > cmd->param_count)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        uIndex = 0;
        arfcnCount = atParamUint(cmd->params[uIndex], &paramok);
        if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if ((6 < arfcnCount) || ((arfcnCount * 2 + 1) != cmd->param_count))
        {
            OSI_LOGI(0, "AT_GC_CmdFunc_NVCFGARFCN: param error,uParaCount = %d, arfcnCount = %d.\n", cmd->param_count, arfcnCount);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        OSI_LOGI(0, "AT_GC_CmdFunc_NVCFGARFCN, uParaCount = %d, arfcnCount = %d.\n", cmd->param_count, arfcnCount);

        for (uNvIndex = 0; uNvIndex < arfcnCount; uNvIndex++)
        {
            uIndex++;
            arfcn = atParamUint(cmd->params[uIndex], &paramok);
            if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

#if 0
            /*bigger than band66, error*/
            if (67335 < arfcn)
            {
                OSI_LOGI(0, "AT_GC_CmdFunc_CNVCFGARFCN: param error, %dst arfcn(%d) error.\n", uNvIndex+1, arfcn);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
#endif
            uIndex++;
            offset = atParamUintInRange(cmd->params[uIndex], 0, 38, &paramok);
            if (!paramok || !nbiot_nvIsInputNumber((const char *)cmd->params[uIndex]->value))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            /*NB-IoT channel number offset(-20,-18,-16,-14,-12,-10,-8,-6,-4,-2,-1,0,2,4,6,8,10,12,14,16,18),  offset value is 20*/
            OSI_LOGI(0, "AT_GC_CmdFunc_NVCFGARFCN, uParaCount = %d, arfcn = %d, offset = %d.\n",
                     cmd->param_count, arfcn, offset);

            a_earfcn[uNvIndex] = arfcn;
            a_offset[uNvIndex] = offset - 20;
        }

        freq_num = arfcnCount;
        nbiot_nvSetFreqList(freq_num, a_earfcn, a_offset);
        nvmWriteStatic();
        OSI_LOGI(0, "AT_GC_CmdFunc_NVCFGARFCN, set nvmWriteStatic ok.\n");
        AT_CMD_RETURN(atCmdRespOK(cmd->engine));
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        OSI_LOGI(0, "AT_GC_CmdFunc_NVCFGARFCN, type = AT_CMD_TEST.\n");
        memset(str, 0, sizeof(str));
        sprintf(str, "+NVCFGARFCN: ARFCN_Num=[0-6], ARFCN=(arfcn,offset)");
        atCmdRespInfoText(cmd->engine, str);
        AT_CMD_RETURN(atCmdRespOK(cmd->engine));
    }
    else if (AT_CMD_READ == cmd->type)
    {
        OSI_LOGI(0, "AT_GC_CmdFunc_NVCFGARFCN read.\n");
        nbiot_nvGetFreqList(&freq_num, a_earfcn, a_offset);
        uPrintLen = sprintf(str, "%d in total:", freq_num);
        for (uNvIndex = 0; uNvIndex < freq_num; uNvIndex++)
        {
            uPrintLen += sprintf(str + uPrintLen, "(%lu %d) ",
                                 a_earfcn[uNvIndex],
                                 a_offset[uNvIndex]);
            if (uNvIndex + 1 < freq_num)
            {
                uPrintLen += sprintf(str + uPrintLen, ",");
            }
        }
        atCmdRespInfoText(cmd->engine, str);
        AT_CMD_RETURN(atCmdRespOK(cmd->engine));
    }
    else
    {
        OSI_LOGI(0, "AT_GC_CmdFunc_NVCFGARFCN, error.\n");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

#endif

void atCmdHandleRFTEMPERATURE(atCommand_t *cmd)
{
    char urcBuffer[30] = {0};
    if (cmd->type == AT_CMD_READ)
    {
        uint32_t rfTem = 0;
        uint32_t ret = CFW_GetRFTemperature(&rfTem);
        if (ret != 0)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        uint8_t isNegative = rfTem >> 31;
        uint16_t value = rfTem & 0xFFFF;
        OSI_LOGI(0, "atCmdHandleRFTEMPERATURE rfTem: %lu, value %d, isNegative %d", rfTem, value, isNegative);
        float outputValue = value * 1.0 / 100;
        OSI_LOGI(0, "atCmdHandleRFTEMPERATURE rfTem: %.2f", outputValue);
        if (isNegative == 0 || value == 0)
        {
            sprintf(urcBuffer, "+RFTEMPERATURE: %.2f", outputValue);
            atCmdRespInfoText(cmd->engine, urcBuffer);
        }
        else if (isNegative == 1)
        {
            sprintf(urcBuffer, "+RFTEMPERATURE: -%.2f", outputValue);
            atCmdRespInfoText(cmd->engine, urcBuffer);
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        atCmdRespOK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

#ifdef CONFIG_SOC_8910
void atCmdHandleNCESIM(atCommand_t *cmd)
{
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        // AT+NCESIM=<flag>
        bool paramok = true;

        uint8_t flag = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (cmd->param_count != 1 || !paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (CFW_NwSetnSecurityUsedFlag(flag, atCmdGetSim(cmd->engine)) != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        atCmdRespOK(cmd->engine);
        break;
    }
    case AT_CMD_TEST:
    {
        char urcBuffer[30] = {0};
        sprintf(urcBuffer, "%s: (0-1)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, urcBuffer);
        atCmdRespOK(cmd->engine);
        break;
    }
    default:
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
    }
}
#endif
/*
************************************************************************************************
*  possible input from ATM:      set LTE frequency range max power                             *
*  AT+SETRFFRP=?        -->not support                                                         *
*  AT+SETRFFRP?         -->not support                                                         *
*  AT+SETRFFRP=<X1>,<X2>,<X3> -->set                                                           *
    <X1>         freqlow:                                                                      *
    <X2>         freqhigh:
*   <X3>         power: 3~69, step=1dBm; 69 == 23dBm  46==0dBm 3 = -43dBm
 ***********************************************************************************************
*/
void atCmdHandleSETLTEFRP(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint16_t iFreqLow = 0, iFreqHigh = 0, iPower = 0;
        uint32_t iRet = 0;

        if (3 != cmd->param_count)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        iFreqLow = atParamUint(cmd->params[0], &paramok);
        iFreqHigh = atParamUint(cmd->params[1], &paramok);
        iPower = atParamUintInRange(cmd->params[2], 3, 69, &paramok);
        if (!paramok || (iFreqLow > iFreqHigh))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        iRet = CFW_SetLTEFreqPwrRange(iFreqLow, iFreqHigh, iPower);
        if (iRet != 0)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        atCmdRespOK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
/*
************************************************************************************************
*  possible input from ATM:         set RF frequency range max power                                                           *
*  AT+SETRFFRP=?        -->not support                                                         *
*  AT+SETRFFRP?         -->not support                                                         *
*  AT+SETRFFRP=<X1>,<X2>,<X3>,<X4> -->set                                                      *
    <X1>         mode: 0=GSM, 1=LTE                                                            *
*   mode = GSM:                                                                                *
    <X2>         band:0=850M, 1=900M, 2=1800M, 3=1900M
*   <X3>         powerlow: maxpwr_pcl: max pwr pcl 850/900:5~19;1800/1900:0~15
*   <X4>         powerhigh: minpwr_pcl: min pwr pcl 850/900:5~19;1800/1900:0~15
*   mode = LTE:
*   <X2>         band: LTE BAND,
                 if band == 28A, input value == 0x28A;if band == 28B, input value == 0x28B
*   <X3> <X4>    powerlow,powerhigh:0~69, step=1dBm; 69-> 23dBm, 46->0dBm, 3->-43dBm, 0~2->-43dBm.
 ***********************************************************************************************
*/
void atCmdHandleSETRFFRP(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint16_t iMode = 0, iBand = 0, iPowerlow = 0, iPowerhigh = 0;
        uint32_t iRet = 0;

        if (4 != cmd->param_count)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        iMode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (0 == iMode) //GSM
        {
            iBand = atParamUintInRange(cmd->params[1], 0, 3, &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            if (iBand > 1)
            {
                iPowerlow = atParamUintInRange(cmd->params[2], 0, 15, &paramok);
                iPowerhigh = atParamUintInRange(cmd->params[3], 0, 15, &paramok);
            }
            else
            {
                iPowerlow = atParamUintInRange(cmd->params[2], 5, 19, &paramok);
                iPowerhigh = atParamUintInRange(cmd->params[3], 5, 19, &paramok);
            }
        }
        else
        {
            iBand = atParamUint(cmd->params[1], &paramok);
            iPowerlow = atParamUintInRange(cmd->params[2], 0, 69, &paramok);
            iPowerhigh = atParamUintInRange(cmd->params[3], 0, 69, &paramok);
        }
        if ((!paramok) || (iPowerlow > iPowerhigh))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        iRet = CFW_SetRFFreqPwrRange(iMode, iBand, iPowerlow, iPowerhigh);
        if (0 != iRet)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        atCmdRespOK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
