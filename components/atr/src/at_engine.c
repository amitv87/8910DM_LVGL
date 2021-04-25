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

#include "quec_common.h"
#include "osi_log.h"
#include "osi_api.h"
#include "at_engine_imp.h"
#include "at_parse.h"
#include "at_response.h"
#include "at_cfg.h"
#include "drv_uart.h"
#include "drv_serial.h"
#include "cfw.h"
#include "at_pm.h"
#include "at_cmd_audio.h"
#include "cfw_event.h"
#include "cfw_dispatch.h"
#include "at_cfw.h"
#include "hal_config.h"
#include "srv_sim_detect.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifdef CONFIG_TWOLINE_WAKEUP_ENABLE
#include "srv_2line_wakeup.h"
#endif
#include "tb_config.h"
#include "tb.h"

#include "quec_cust_feature.h"

#ifdef CONFIG_QUEC_PROJECT_FEATURE
#define QUEC_AT_ENGINE_LOG(msg, ...)  custom_log("ATEngine", msg, ##__VA_ARGS__)
#include "quec_urc.h"
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
#include "ql_uart_internal.h"
#include "quec_atcmd_general.h"
extern quec_uart_setting_s quec_uart_current_setting[QL_UART_MAX_NUM];
extern uint32_t quec_main_uart_cbaud;
extern uint32_t quec_get_dev_by_drv_name(uint32_t drv_name);
extern bool quec_uart_setting_read();
#endif

#define AT_ENGINE_THREAD_PRIORITY (OSI_PRIORITY_NORMAL)
#define AT_ENGINE_STACK_SIZE (8192 * 4)
#ifdef CONFIG_SOC_8910
#define AT_ENGINE_EVENT_QUEUE_SIZE (64 * 4)
#else
#define AT_ENGINE_EVENT_QUEUE_SIZE (64)
#endif

#ifdef CONFIG_SOC_6760
extern const char *AT_GMI_ID;
extern const char *AT_GMM_ID;
extern const char *AT_GMR_ID;
#endif

#ifdef CONFIG_ATR_ECHO_COMMAND_ONLY
const bool gAtEchoCommandOnly = true;
#else
const bool gAtEchoCommandOnly = false;
#endif
#ifdef CONFIG_ATR_CMD_TPLUS_ENABLE
const bool gAtCmdTplusEnable = true;
#else
const bool gAtCmdTplusEnable = false;
#endif
#ifdef CONFIG_ATR_URC_BUFF_ENABLE
const bool gAtUrcBuffEnable = true;
const unsigned gAtUrcBuffSize = CONFIG_ATR_URC_BUFF_SIZE;
const unsigned gAtUrcBuffCount = CONFIG_ATR_URC_BUFF_COUNT;
#else
const bool gAtUrcBuffEnable = false;
const unsigned gAtUrcBuffSize = 0;
const unsigned gAtUrcBuffCount = 0;
#endif
const unsigned gAtCmdlineMax = CONFIG_ATR_CMDLINE_MAX;
const unsigned gAtLfWaitMs = CONFIG_ATR_LF_WAIT_MS;
#ifdef CONFIG_ATR_CMUX_SUPPORT
const unsigned gAtCmuxOutBuffSize = CONFIG_ATR_CMUX_OUT_BUFF_SIZE;
const unsigned gAtCmuxInBuffSize = CONFIG_ATR_CMUX_IN_BUFF_SIZE;
const unsigned gAtCmuxSubMinInBuffSize = CONFIG_ATR_CMUX_SUBCHANNEL_MIN_IN_BUFF_SIZE;
const unsigned gAtCmuxDlcNum = CONFIG_ATR_CMUX_DLC_NUM;
#else
const unsigned gAtCmuxOutBuffSize = 0;
const unsigned gAtCmuxInBuffSize = 0;
const unsigned gAtCmuxSubMinInBuffSize = 0;
const unsigned gAtCmuxDlcNum = 0;
#endif
const unsigned gAtDataBuffSize = CONFIG_ATR_DATA_BUFF_SIZE;
const unsigned gAtPppEndCheckTimeout = CONFIG_ATR_PPP_END_CHECK_TIMEOUT;
const unsigned gAtCmdWorkerStackSize = CONFIG_ATR_CMD_WORKER_STACK_SIZE;

static atEngine_t gAtEngine = {
    .thread_id = NULL,
    .event_hub = NULL,
    .mem_recycler = NULL,
};

void atEngineInit(void)
{
    gAtEngine.id_man = osiEventDispatchCreate(CONFIG_ATR_CFW_PENDING_UTI_COUNT);
    gAtEngine.event_hub = osiEventHubCreate(CONFIG_ATR_EVENT_MAX_COUNT);
    gAtEngine.mem_recycler = osiMemRecyclerCreate(CONFIG_ATR_MEM_FREE_LATER_COUNT);
}

void atCmdHandleAT(atCommand_t *cmd)
{
    atCmdRespOK(cmd->engine);
}

void atCmdHandleSetSim(atCommand_t *cmd)
{
    char rsp[80] = {
        0,
    };

    if (cmd->type == AT_CMD_TEST)
    {
        sprintf(rsp, "%s: (0-1)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        sprintf(rsp, "%s: %d", cmd->desc->name, atCmdGetSim(cmd->engine));
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        const char *param = atParamRawText(cmd->params[0], &paramok);
        size_t param_size = strlen(param);
        if (!paramok || cmd->param_count > 1 || param_size > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (strcmp("0", param) == 0 || strcmp("1", param) == 0)
        {
            uint8_t sim = atoi(param);
            atCmdSetSim(cmd->engine, sim);
            atCmdRespOK(cmd->engine);
        }
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

#ifdef CONFIG_ATR_CMD_TPLUS_ENABLE
/**
 * Command handler for non-standard +++ in command mode
 */
void atCmdHandleTplus(atCommand_t *cmd)
{
    atCmdRespOKText(cmd->engine, "NO CARRIER");
}
#endif

/**
 * Command description for non-standard +++ in command mode
 */
const atCmdDesc_t gAtTplusCmdDesc = {
    .name = "+++",
#ifdef CONFIG_ATR_CMD_TPLUS_ENABLE
    .handler = atCmdHandleTplus,
#endif
};

#ifdef CONFIG_ATR_CMUX_SUPPORT
void atCmdHandleCMUX(atCommand_t *cmd)
{
    static const atCmuxConfig_t def_cfg = {
        .transparency = 0,
        .subset = 0,
        .port_speed = 5,
        .max_frame_size = 31,
        .ack_timer = 10,
        .max_retrans_count = 3,
        .resp_timer = 30,
        .wakeup_resp_timer = 10,
        .window_size = 2,
    };

    char rsp[80];
    if (cmd->type == AT_CMD_TEST)
    {
#ifdef QUEC_CMUX_ENABLE
		sprintf(rsp, "%s: (0),(0),(1-8),(1-2048),(1-255),(0-100),(2-255),(1-255),(1-7)", cmd->desc->name);
#else
        sprintf(rsp, "%s: (0,1),(0),(1-6),(1-2048),(1-255),(0-100),(2-255),(1-255),(1-7)", cmd->desc->name);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        atDispatch_t *dispatch = atCmdGetDispatch(cmd->engine);
        atCmuxEngine_t *cmux = atDispatchGetParentCmuxEngine(dispatch);
        atCmuxConfig_t muxcfg = def_cfg;
        if (cmux != NULL)
            muxcfg = atCmuxGetConfig(cmux);

        sprintf(rsp, "%s: %d,%d,%d,%d,%d,%d,%d,%d,%d", cmd->desc->name,
                muxcfg.transparency, muxcfg.subset, muxcfg.port_speed, muxcfg.max_frame_size,
                muxcfg.ack_timer, muxcfg.max_retrans_count, muxcfg.resp_timer, muxcfg.wakeup_resp_timer,
                muxcfg.window_size);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        atCmuxConfig_t muxcfg = def_cfg;

        bool paramok = true;
#ifdef QUEC_CMUX_ENABLE
        muxcfg.transparency = atParamUintInRange(cmd->params[0], 0, 0, &paramok);
#else
		muxcfg.transparency = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
#endif
        muxcfg.subset = atParamDefUintInRange(cmd->params[1], muxcfg.subset, 0, 0, &paramok);
#ifdef QUEC_CMUX_ENABLE
        muxcfg.port_speed = atParamDefUintInRange(cmd->params[2], muxcfg.port_speed, 1, 8, &paramok);
#else
		muxcfg.port_speed = atParamDefUintInRange(cmd->params[2], muxcfg.port_speed, 1, 6, &paramok);
#endif
        muxcfg.max_frame_size = atParamDefUintInRange(cmd->params[3], muxcfg.max_frame_size, 1, 2048, &paramok);
        muxcfg.ack_timer = atParamDefUintInRange(cmd->params[4], muxcfg.ack_timer, 1, 255, &paramok);
        muxcfg.max_retrans_count = atParamDefUintInRange(cmd->params[5], muxcfg.max_retrans_count, 0, 100, &paramok);
        muxcfg.resp_timer = atParamDefUintInRange(cmd->params[6], muxcfg.resp_timer, 2, 255, &paramok);
        muxcfg.wakeup_resp_timer = atParamDefUintInRange(cmd->params[7], muxcfg.wakeup_resp_timer, 1, 255, &paramok);
        muxcfg.window_size = atParamDefUintInRange(cmd->params[8], muxcfg.window_size, 1, 7, &paramok);
        if (!paramok || cmd->param_count > 9)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        atDispatch_t *dispatch = atCmdGetDispatch(cmd->engine);
        atCmdRespOK(cmd->engine);
        atDispatchSetCmuxMode(dispatch, &muxcfg);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
#endif

void atEngineModeSwitch(atModeSwitchCause_t cause, atDispatch_t *d)
{
    atCmdEngine_t *engine = atDispatchGetCmdEngine(d);

    switch (cause)
    {
    case AT_MODE_SWITCH_DATA_END:
        atDispatchEndDataMode(d);
        atDispatchInvokeModeSwitchHandle(d, cause);
        break;

    case AT_MODE_SWITCH_DATA_PPP_END:
        atDispatchEndDataPppMode(d);
        atDispatchInvokeModeSwitchHandle(d, cause);
        break;

    case AT_MODE_SWITCH_DATA_START:
        atDispatchInvokeModeSwitchHandle(d, cause);
        // Before switch to data mode, the buffered URC will be dropped.
        // It is very possible that peer don't want to receive URC now.
        // It should be called before final response, due to URC may be
        // flushed inside final response.
        atCmdUrcClear(engine);
        atCmdRespFinish(engine);
        atDispatchSetDataMode(d);
        break;

    case AT_MODE_SWITCH_DATA_ESCAPE:
        atDispatchSetCmdMode(d);
        atCmdSetLineMode(engine);
        atDispatchInvokeModeSwitchHandle(d, cause);
        break;

    case AT_MODE_SWITCH_DATA_RESUME:
        atDispatchInvokeModeSwitchHandle(d, cause);
        atCmdUrcClear(engine);
        atCmdRespFinish(engine);
        atDispatchSetDataMode(d);
        break;

    default:
        break;
    }
}

void atEngineSchedule(osiCallback_t cb, void *cb_ctx)
{
    osiThreadCallback(gAtEngine.thread_id, cb, cb_ctx);
}

bool atEngineCreateUartChannel(uint32_t name)
{
    atDeviceUartConfig_t uart_cfg = {
        .name = name,
        .baud = gAtSetting.ipr,
        .format = gAtSetting.icf_format, // AT_DEVICE_FORMAT_8N1,
        .parity = gAtSetting.icf_parity, // AT_DEVICE_PARITY_ODD,
    };
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
    uint32_t uart_dev;
    uart_dev = quec_get_dev_by_drv_name(name);
    if(QL_UART_PORT_2 == uart_dev || QL_UART_PORT_3 == uart_dev)
    {
        uart_cfg.baud   = quec_uart_current_setting[uart_dev].baudrate;
        uart_cfg.format = quec_uart_current_setting[uart_dev].icf_format; // AT_DEVICE_FORMAT_8N1,
        uart_cfg.parity = quec_uart_current_setting[uart_dev].icf_parity; // AT_DEVICE_PARITY_ODD,
    }
#endif
    QUEC_AT_ENGINE_LOG("Init at uart port %d", name);
    bool result = false;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
	if(quec_uart_is_allow_create(uart_cfg.name) == false)
	{
		QUEC_AT_ENGINE_LOG("%4c is not allow to open", uart_cfg.name);
		return result;
	}
#endif
    atDevice_t *uart = atDeviceUartCreate(&uart_cfg);
    if (uart)
    {
        atDispatch_t *dispatch = atDispatchCreate(uart);
        if (dispatch)
        {
            atDeviceSetDispatch(uart, dispatch);
            atDeviceOpen(uart);
#ifndef CONFIG_QUEC_PROJECT_FEATURE_UART             
            atDeviceSetFlowCtrl(uart, gAtSetting.ifc_rx, gAtSetting.ifc_tx);
#else
            if(QL_UART_PORT_2 == uart_dev || QL_UART_PORT_3 == uart_dev)
            {
                atDeviceSetFlowCtrl(uart, quec_uart_current_setting[uart_dev].ifc_rx, quec_uart_current_setting[uart_dev].ifc_tx);
            }
            else 
            {
                atDeviceSetFlowCtrl(uart, gAtSetting.ifc_rx, gAtSetting.ifc_tx);
            }
			quec_at_device_create_success(uart_cfg.name, uart, dispatch);
#endif
            result = true;
        }
        else
        {
            atDeviceDelete(uart);
        }
    }
    return result;
}

#ifdef CONFIG_SOC_8910
bool atEngineCreateUSerialChannel(uint32_t name)
{
    QUEC_AT_ENGINE_LOG("Init at usb port %d", name);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
	if(quec_uart_is_allow_create(name) == false)
	{
		QUEC_AT_ENGINE_LOG("%4c is not allow to open", name);
		return false;
	}
#endif

    atDevice_t *usrl = atDeviceUserialCreate(name);
    if (!usrl)
        return false;

    atDispatch_t *usrl_dispatch = atDispatchCreate(usrl);
    if (!usrl_dispatch)
    {
        atDeviceDelete(usrl);
        return false;
    }
    atDeviceSetDispatch(usrl, usrl_dispatch);
    atDeviceOpen(usrl);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
	quec_at_device_create_success(name, usrl, usrl_dispatch);
#endif
    return true;
}
#endif

#ifdef CONFIG_ATR_CREATE_UART
static inline void _atDeviceUartInit()
{
    atEngineCreateUartChannel(CONFIG_ATR_DEFAULT_UART);
}
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE
//jensen.fang: add interface to init more usb port as at port
static inline void _atDeviceUserialInit_Ext(unsigned port)
{
	atEngineCreateUSerialChannel(port);
}

bool quec_SetPendingIdCmd_via_atEngine(void *ctx, uint32_t id, void *handler)
{
    return osiEventDispatchRegister(gAtEngine.id_man, id, (osiEventCallback_t)handler, ctx);
}

#endif

#ifdef CONFIG_ATR_CREATE_USB_SERIAL
static inline void _atDeviceUserialInit()
{
    if (!atEngineCreateUSerialChannel(DRV_NAME_USRL_COM0))
        osiPanic();
}
#endif

static OSI_UNUSED void _atDeviceDiagInit()
{
    atDevice_t *diag = atDeviceDiagCreate();
    if (!diag)
        osiPanic();

    atDispatch_t *diag_dispatch = atDispatchCreate(diag);
    if (!diag_dispatch)
        osiPanic();

    atCmdEngine_t *cmd = atDispatchGetCmdEngine(diag_dispatch);
    atChannelSetting_t *setting = atCmdChannelSetting(cmd);
    setting->ate = 0; // Autotester tool needs to turn off ate echo by default.

    atDeviceSetDispatch(diag, diag_dispatch);
    atDeviceOpen(diag);
}

#ifdef CONFIG_SOC_6760
static void atLogOutVersion(void)
{
    OSI_LOGI(0, "ATI:  version");
    OSI_LOGI(0, AT_GMI_ID);
    OSI_LOGI(0, AT_GMM_ID);
    OSI_LOGI(0, AT_GMR_ID);
}
#endif

static void atEngineTaskEntry(void *argument)
{
    atEngine_t *p = &gAtEngine;
    p->thread_id = osiThreadCurrent();

    atCfgInit();
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
    quec_uart_setting_read();
    quec_main_uart_cbaud = gAtSetting.ipr;
#endif
    
#ifdef CONFIG_ATR_CMD_OUTPUT_CACHE_ENABLE
    atCmdCreateOutputLineCache(CONFIG_ATR_CMD_OUTPUT_CACHE_SIZE);
#endif

    if (osiGetBootMode() != OSI_BOOTMODE_BBAT && osiGetBootMode() != OSI_BOOTMODE_CALIB_POST)
    {
#ifdef CONFIG_ATR_CREATE_UART
        _atDeviceUartInit();
#endif
#ifdef __QUEC_OEM_VER_LD__
		quec_uart_at_device_create(QL_UART_PORT_3);
#endif

#ifdef CONFIG_ATR_CREATE_USB_SERIAL
        _atDeviceUserialInit();
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        _atDeviceUserialInit_Ext(DRV_NAME_USRL_COM5);
        _atDeviceUserialInit_Ext(DRV_NAME_USRL_COM6);
        //_atDeviceUserialInit_Ext(DRV_NAME_USRL_COM7);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_URC        
        quec_urc_init();
#endif        
#endif
    }

#ifdef CONFIG_ATR_CREATE_DIAG
    if (osiGetBootMode() != OSI_BOOTMODE_NORMAL)
        _atDeviceDiagInit();
#endif

#ifdef CONFIG_TWOLINE_WAKEUP_ENABLE
    srv2LineWakeUpInit(gAtSetting.mcuNotifySleepMode, gAtSetting.mcuNotifySleepDelayMs);
#endif

    cfwDispatchSetMainThread(p->thread_id);
    atAlarmInit();
    atCfwInit();
#ifdef CONFIG_SOC_8910
    CFW_EnableLpsLock(gAtSetting.csclk == 0, 0);
#endif
    atEventRegister(EV_DM_POWER_ON_IND, atCfwPowerOn);

    atPmInit();

#ifdef CONFIG_AT_AUDIO_SUPPORT
    atAudioInit();
#endif

#ifdef CONFIG_ATR_CFW_AUTO_POWER_ON
    osiEvent_t cfw_pon_event = {.id = EV_DM_POWER_ON_IND};
    osiEventSend(atEngineGetThreadId(), &cfw_pon_event);
#endif

#ifdef CONFIG_SOC_6760
    atLogOutVersion(); // test engneer want version infomation through log
#endif

    if (osiGetBootMode() != OSI_BOOTMODE_BBAT)
    {
#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPIO
#ifdef CONFIG_BOARD_SUPPORT_SIM1_DETECT
        srvSimDetectRegister(1, CONFIG_BOARD_SIM1_DETECT_GPIO, atCfwSimHotPlugCB);
#endif

#ifdef CONFIG_BOARD_SUPPORT_SIM2_DETECT
        srvSimDetectRegister(2, CONFIG_BOARD_SIM2_DETECT_GPIO, atCfwSimHotPlugCB);
#endif
#endif
    }

    for (;;)
    {
        osiEvent_t event = {};
        osiEventWait(p->thread_id, &event);

        if (event.id != 0)
        {
            OSI_LOGXI(OSI_LOGPAR(I, S, I, I, I), 0x1000527a, "AT get event: %d(%s)/0x%08x/0x%08x/0x%08x",
                      event.id, CFW_EventName(event.id),
                      event.param1, event.param2, event.param3);
#ifdef CONFIG_TB_API_SUPPORT
            tb_handle_cfw_event(&event);
#endif
            if (osiEventDispatchRun(gAtEngine.id_man, &event))
            {
                OSI_LOGW(0x1000527b, "AT RESPONSE EVENT HANDLED BY ID!");
            }
            else if ((!cfwIsCfwIndicate(event.id)) && (cfwInvokeUtiCallback(&event)))
            {
                ; // handled by UTI
            }
            else if (osiEventHubRun(gAtEngine.event_hub, &event))
            {
                ; // handled by static register
            }
            else
            {
#ifdef CONFIG_QUEC_PROJECT_ATCMD_APP_SUPPORT
                extern int quec_at_app_event_proc(osiEvent_t * event);
                if(quec_at_app_event_proc(&event) != 0)
#endif
                OSI_LOGE(0x1000527c, "AT: unregistered event");
            }
        }

        // It should be unnecessary, but in case atCmdWrite are called directly,
        // rather than atCmdRespXXX.
        atCmdWriteFlushAll();

        osiMemRecyclerEmpty(gAtEngine.mem_recycler);
    }
}

void atEngineStart(void)
{
    atEngineInit();

    gAtEngine.thread_id = osiThreadCreate(
        "atr", atEngineTaskEntry, NULL,
        AT_ENGINE_THREAD_PRIORITY, AT_ENGINE_STACK_SIZE,
        AT_ENGINE_EVENT_QUEUE_SIZE);
}

osiThread_t *atEngineGetThreadId(void)
{
    return gAtEngine.thread_id;
}

void atEngineCallback(osiCallback_t cb, void *ctx)
{
    osiThreadCallback(gAtEngine.thread_id, cb, ctx);
}

bool atEventRegister(uint32_t id, osiEventHandler_t handler)
{
    return osiEventHubRegister(gAtEngine.event_hub, id, handler);
}

bool atEventsRegister(uint32_t id, ...)
{
    va_list ap;
    va_start(ap, id);
    bool ok = osiEventHubVBatchRegister(gAtEngine.event_hub, id, ap);
    va_end(ap);
    return ok;
}

bool atSetPendingIdCmd(atCommand_t *cmd, uint32_t id, atCommandAsyncCB_t handler)
{
    return osiEventDispatchRegister(gAtEngine.id_man, id, (osiEventCallback_t)handler, cmd);
}

void AT_SetAsyncTimerMux(atCmdEngine_t *cmd, uint32_t timeout)
{
}

void atMemFreeLater(void *ptr)
{
    if (!osiMemRecyclerPut(gAtEngine.mem_recycler, ptr))
        osiPanic();
}

void atMemUndoFreeLater(void *ptr)
{
    osiMemRecyclerUndoPut(gAtEngine.mem_recycler, ptr);
}

void atEngineSetDeviceAutoSleep(bool enabled)
{
    int timeout = enabled ? CONFIG_ATR_UART_AUTO_SLEEP_TIMEOUT : -1;

    osiSlistHead_t *list = atDispatchGetList();
    osiSlistItem_t *item;
    SLIST_FOREACH(item, list, iter)
    {
        atDispatch_t *dispatch = (atDispatch_t *)item;
        atDevice_t *device = atDispatchGetDevice(dispatch);
        atDeviceSetAutoSleep(device, timeout);
    }
}

bool atCommandCheckConstrains(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_TEST)
        return true;

    osiBootMode_t boot_mode = osiGetBootMode();
    if ((cmd->desc->constrains & AT_CON_CALIB_MODE) && boot_mode != OSI_BOOTMODE_CALIB)
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        return false;
    }

    if ((cmd->desc->constrains & AT_CON_NOT_CALIB_MODE) && boot_mode == OSI_BOOTMODE_CALIB)
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        return false;
    }

    return true;
}

void pppAtModeSwitchHandler(atDispatch_t *th, atModeSwitchCause_t cause)
{
    atCmdEngine_t *engine = atDispatchGetCmdEngine(th);
    switch (cause)
    {
    case AT_MODE_SWITCH_DATA_START:
        if (engine != NULL)
            atCmdRespIntermCode(engine, CMD_RC_CONNECT);
        break;
    case AT_MODE_SWITCH_DATA_ESCAPE:
        if (engine != NULL)
            atCmdRespInfoText(engine, "OK");
        break;
    case AT_MODE_SWITCH_DATA_RESUME:
        if (engine != NULL)
            atCmdRespIntermCode(engine, CMD_RC_CONNECT);
        break;
    default:
        break;
    }
}
