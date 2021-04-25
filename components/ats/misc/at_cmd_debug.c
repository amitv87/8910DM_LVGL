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
#include "osi_mem.h"
#include "osi_sysnv.h"
#include "osi_trace.h"
#include "at_cfw.h"
#include "at_cfg.h"
#include "at_engine.h"
#include "at_command.h"
#include "at_response.h"
#include "cfw_event.h"
#include "hal_iomux.h"
#include "hal_chip.h"
#include "drv_md_ipc.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "hal_config.h"
#include "drv_config.h"
#include "kernel_config.h"
#include "drv_spi_flash.h"
#include "srv_simlock.h"
#include "simlock.h"
#include "block_device.h"
#include "fs_mount.h"
#ifdef CONFIG_SOC_6760
#include "cfw_nb_nv_api.h"
#endif

#ifdef CONFIG_SOC_6760
void nbiot_SetNetworkStatusInd(uint32_t switch_flag);
#endif

void atCmdHandleSWJTAG(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        const char *mode = atParamStr(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (strcasecmp(mode, "AP") == 0)
        {
#if defined(CONFIG_SOC_8910)
            halIomuxSetFunction(HAL_IOMUX_FUN_AP_JTAG_TCK);
            halIomuxSetFunction(HAL_IOMUX_FUN_AP_JTAG_TDI);
            halIomuxSetFunction(HAL_IOMUX_FUN_AP_JTAG_TDO);
            halIomuxSetFunction(HAL_IOMUX_FUN_AP_JTAG_TMS);
            halIomuxSetFunction(HAL_IOMUX_FUN_AP_JTAG_TRST);
#endif
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "^SWJTAG: (\"AP\",\"CP\")");
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void atCmdHandlePMSTART(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET || cmd->type == AT_CMD_EXE)
    {
        atCmdRespOK(cmd->engine);
        osiPmStart();
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void atCmdHandlePMSTOP(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET || cmd->type == AT_CMD_EXE)
    {
        atCmdRespOK(cmd->engine);
        osiPmStop();
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void atCmdHandleSPREF(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        const char *operation = atParamStr(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (strcasecmp(operation, "AUTODLOADER") == 0)
        {
            atCmdRespOK(cmd->engine);
            osiShutdown(OSI_SHUTDOWN_FORCE_DOWNLOAD);
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void atCmdHandleFORCEDNLD(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        atCmdRespOK(cmd->engine);
        osiShutdown(OSI_SHUTDOWN_FORCE_DOWNLOAD);
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    else if (cmd->type == AT_CMD_SET)
    {
        atCmdRespOK(cmd->engine);
        osiShutdown(OSI_SHUTDOWN_FORCE_DOWNLOAD);
    }
#endif
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#ifndef CONFIG_SOC_6760
void atCmdHandleHEAPINFO(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_EXE)
    {
        osiMemPoolStat_t stat;
        if (!osiMemPoolStat(NULL, &stat))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        char rsp[128];
        sprintf(rsp, "%s: %p,%ld,%ld,%ld", cmd->desc->name,
                stat.start, stat.size, stat.avail_size, stat.max_block_size);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void atCmdHandleBLKDEVINFO(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_EXE)
    {
        blockDeviceStat_t stat;
        unsigned names[16];
        char rsp[128];
        int count = fsGetBlockDeviceNames(names, 16);
        for (int n = 0; n < count; n++)
        {
            blockDevice_t *bdev = fsFindBlockDeviceByName(names[n]);
            if (bdev == NULL)
                continue;

            blockDeviceStat(bdev, &stat);
            sprintf(rsp, "%s: %c%c%c%c,%d,%d,%d", cmd->desc->name,
                    (char)(names[n]), (char)(names[n] >> 8),
                    (char)(names[n] >> 16), (char)(names[n] >> 24),
                    stat.write_block_count, stat.min_erase_count,
                    stat.max_erase_count);
            atCmdRespInfoText(cmd->engine, rsp);
        }
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
#endif

static void _timeoutabortTimeout(atCommand_t *cmd)
{
    atCmdRespInfoText(cmd->engine, "^TIMEOUTABORT: timeout");
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

static void _timeoutabortAbort(atCommand_t *cmd)
{
    atCmdRespInfoText(cmd->engine, "^TIMEOUTABORT: aborted");
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

void atCmdHandleTIMEOUTABORT(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_EXE)
    {
        atCmdRespInfoText(cmd->engine, "^TIMEOUTABORT: wait for 5s timeout or abort");
        atCmdSetTimeoutHandler(cmd->engine, 5000, _timeoutabortTimeout);
        atCmdSetAbortHandler(cmd->engine, _timeoutabortAbort);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#ifndef CONFIG_SOC_6760
void atCmdHandleTrace(atCommand_t *cmd)
{
    static uint32_t ap_valid_devices[] = {
        OSI_TRACE_DEVICE_NONE,
        OSI_TRACE_DEVICE_DEBUGHOST,
        OSI_TRACE_DEVICE_USBSERIAL,
    };
#ifdef CONFIG_SOC_8910
    static uint32_t cp_valid_devices[] = {
        0, // cp uart
        2, // close
        5, // ap usb
        6, // sd Card
    };
#endif

    if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char resp[64];
        snprintf(resp, 64, "%s: 0,%d,%d", cmd->desc->name, gSysnvTraceEnabled, gSysnvTraceDevice);
        atCmdRespInfoText(cmd->engine, resp);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        // ^TRACECTRL=0,enable[,device]

        bool paramok = true;
        uint8_t index = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        uint8_t enable = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (index == 0)
        {
            if (!atParamIsEmpty(cmd->params[2]))
            {
                uint8_t device = atParamUintInList(cmd->params[2], ap_valid_devices,
                                                   OSI_ARRAY_SIZE(ap_valid_devices),
                                                   &paramok);
                if (!paramok || cmd->param_count > 3)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                gSysnvTraceDevice = device;
            }

            gSysnvTraceEnabled = (enable != 0);
            osiTraceSetEnable(gSysnvTraceEnabled);
            osiSysnvSave();
            RETURN_OK(cmd->engine);
        }

#ifdef CONFIG_SOC_8910
        if (index == 1)
        {
            if (!atParamIsEmpty(cmd->params[2]))
            {
                uint8_t device = atParamUintInList(cmd->params[2], cp_valid_devices,
                                                   OSI_ARRAY_SIZE(cp_valid_devices),
                                                   &paramok);
                if (!paramok || cmd->param_count > 3)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                CFW_EmodSetTracePort(device);
            }

            ipc_switch_cp_trace(enable != 0);
            RETURN_OK(cmd->engine);
        }
#endif

        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
#endif

#ifdef CONFIG_SOC_6760
extern unsigned int upper_limit;
void atCmdHandleSETLIMIT(atCommand_t *cmd)
{
    bool paramok = true;

    OSI_LOGI(0, "atCmdHandleSETLIMIT in");
    if (AT_CMD_SET == cmd->type)
    {
        int16_t value = atParamInt(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        upper_limit = value;
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleNWSTATUS(atCommand_t *cmd)
{
    bool paramok = true;

    OSI_LOGI(0, "atCmdHandleNWSTATUS type: %d", cmd->type);
    if (AT_CMD_SET == cmd->type)
    {
        int16_t value = atParamInt(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1 || value > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        nbiot_SetNetworkStatusInd(value);
        nvmWriteStatic();
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char buf[256];
        snprintf(buf, 256, "+NWSTATUS: (0-1)\r\n"
                           "0. close network status ind.\r\n"
                           "1. open network status ind.\r\n");

        atCmdRespInfoText(cmd->engine, buf);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

#endif

#ifdef CONFIG_SOC_8910

#include "drv_usb.h"

void atCmdHandleUSBSWITCH(atCommand_t *cmd)
{
    char resp[64];
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t en = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        atCmdRespOK(cmd->engine);
        if (!!en != drvUsbIsEnabled())
        {
            if (en == 1)
                drvUsbEnable(CONFIG_USB_DETECT_DEBOUNCE_TIME);
            else
                drvUsbDisable();
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        sprintf(resp, "%s: (0,1)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, resp);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        sprintf(resp, "%s: %d", cmd->desc->name, drvUsbIsEnabled());
        atCmdRespInfoText(cmd->engine, resp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void atCmdHandleUSBRMTWK(atCommand_t *cmd)
{
    drvUsbRemoteWakeup();
    atCmdRespOK(cmd->engine);
}

void simlockVerifyTest(void)
{
    if (simlockDataVerify())
        OSI_LOGI(0, "simlock verify ok");
}

void atCmdHandleSimlockTest(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        simlockVerifyTest();
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char rspStr[20] = {0};
        sprintf(rspStr, "+SIMLOCKTEST");
        atCmdRespInfoText(cmd->engine, rspStr);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void atCmdHandleSet7sReset(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t enable7sReset = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (enable7sReset == 1)
            halPmuSet7sReset(true);
        else
            halPmuSet7sReset(false);

        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char rspStr[20] = {0};
        sprintf(rspStr, "+SET7SRESET");
        atCmdRespInfoText(cmd->engine, rspStr);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
#endif

#ifdef CONFIG_TWOLINE_WAKEUP_ENABLE
#include "srv_2line_wakeup.h"
void atCmdHandleMcuNotifySleep(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t mode = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);
        uint8_t delay_ms = atParamUint(cmd->params[1], &paramok);
        if (!paramok || cmd->param_count != 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.mcuNotifySleepDelayMs = delay_ms;
        gAtSetting.mcuNotifySleepMode = mode == 0 ? SRV_2LINE_WAKEUP_WAKEUP_IN : SRV_2LINE_WAKEUP_AT_CMD;
        srv2LineWakeUpMcuNotifySleep(gAtSetting.mcuNotifySleepMode, gAtSetting.mcuNotifySleepDelayMs);

        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char rsp[32];
        sprintf(rsp, "%s:(0,1),(0-500)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
#endif

#ifdef CONFIG_AT_EMMCDDRSIZE_SUPPORT
void atCmdHandleEmmcDDRSize(atCommand_t *cmd)
{
    if (AT_CMD_READ == cmd->type || AT_CMD_EXE == cmd->type)
    {
        drvSpiFlash_t *flash_dev = drvSpiFlashOpen(DRV_NAME_SPI_FLASH);
        if (flash_dev == NULL)
        {
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        char buf[256];
        uint32_t flash_size = drvSpiFlashCapacity(flash_dev);
        uint32_t flash_size_m = flash_size / 1024 / 1024;
        uint32_t exsram_size = CONFIG_RAM_SIZE;
        uint32_t exsram_size_m = exsram_size / 1024 / 1024;
        snprintf(buf, 256, "%luM+%luM",
                 flash_size_m, exsram_size_m);
        atCmdRespInfoText(cmd->engine, buf);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
#endif

// CMUXEND: exit cmux mode forcedly
void atCmdHandleCMUXEND(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        // +CMUXEND=0/1
        // 0: end current cmux channel
        // 1: end all cmux channel
        bool paramok = true;
        unsigned mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (mode == 0) // end current
        {
            atDispatch_t *cmux_dispatch = atCmdGetDispatchInCmux(cmd->engine);
            if (cmux_dispatch == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            atDispatchEndCmuxMode(cmux_dispatch);
            RETURN_OK(cmd->engine);
        }
        else
        {
            unsigned count = atDispatchEndCmuxModeAll();
            if (count == 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            RETURN_OK(cmd->engine);
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#ifdef CONFIG_WDT_ENABLE

#include "srv_wdt.h"

extern bool gBsWatchdogReset;

void atCmdUWdtCtlHandle(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+UWDTCTL: (0,1)");
    }
    else if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        const bool wdten = atParamUintInRange(cmd->params[0], 0, 1, &paramok) == 1;
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (wdten != gBsWatchdogReset)
        {
            gBsWatchdogReset = wdten;
            if (wdten)
                srvWdtStart();
            else
                srvWdtStop();
        }
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char resp[32];
        snprintf(resp, 32, "+UWDTCTL: %d", gBsWatchdogReset);
        atCmdRespInfoText(cmd->engine, resp);
    }
    RETURN_OK(cmd->engine);
}

#endif
