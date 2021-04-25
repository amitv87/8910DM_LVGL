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
#include "osi_sysnv.h"
#include "osi_trace.h"
#include "app_config.h"
#include "at_engine.h"
#include "fs_config.h"
#include "fs_mount.h"
#include "fs_mount_sdcard.h"
#include "ml.h"
#include "hal_shmem_region.h"
#include "hal_cp_ctrl.h"
#include "hal_iomux.h"
#include "net_config.h"
#include "hal_config.h"
#include "hal_chip.h"
#include "hal_blue_screen.h"
#include "hwregs.h"
#include "netmain.h"
#include "drv_ps_path.h"
#include "drv_pmic_intr.h"
#include "drv_charger.h"
#include "drv_rtc.h"
#include "drv_adc.h"
#include "drv_gpio.h"
#include "drv_usb.h"
#include "drv_axidma.h"
#include "drv_md_nvm.h"
#include "drv_md_ipc.h"
#include "drv_debug_port.h"
#include "drv_host_cmd.h"
#include "drv_spi_flash.h"
#include "drv_config.h"
#include "nvm.h"
#include "vfs.h"
#include "diag.h"
#include "diag_runmode.h"
#include "diag_auto_test.h"
#include "srv_trace.h"
#include "srv_rf_param.h"
#include "fupdate.h"
#include "srv_wdt.h"
#include "app_loader.h"
#include "mal_api.h"
#include "audio_device.h"
#include <stdlib.h>
#include "connectivity_config.h"
#ifdef CONFIG_TTS_SUPPORT
#include "tts_player.h"
#endif
#include "srv_simlock.h"
#include "srv_power_manager.h"
#include "srv_keypad.h"
#include "srv_usb.h"
#include "ql_api_dev.h"
#include "quec_atcmd_general.h"

#ifdef CONFIG_HEADSET_DETECT_SUPPORT
#include "srv_headset.h"
#endif

#include "quec_init.h"
#include "quec_cust_patch.h"

#ifdef CONFIG_AT_BT_APP_SUPPORT
extern void bt_ap_init(void);
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO
extern void quec_pin_cfg_init(void);
extern void quec_gpio_interruput_init(void);
extern void quec_ext_flash_init(void);
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE    
extern bool quec_usb_setting_read();
#endif 

// TODO:
extern const char gBuildRevision[];
extern void ipcInit(void);
extern int32_t ipc_at_init(void);
extern void CFW_RpcInit(void);
extern bool aworker_start();

static void _checkGpioCalibMode(void)
{
#ifdef CONFIG_BOARD_ENTER_CALIB_BY_GPIO
    drvGpioConfig_t cfg = {
        .mode = DRV_GPIO_INPUT,
        .intr_enabled = false,
        .intr_level = false,
        .rising = true,
        .falling = false,
        .debounce = false,
    };

    struct drvGpio *gpio = drvGpioOpen(CONFIG_BOARD_ENTER_CALIB_GPIO, &cfg, NULL, NULL);
    if (drvGpioRead(gpio))
        osiSetBootMode(OSI_BOOTMODE_CALIB);
#endif
}

static void prvSetFlashWriteProhibitByAddress(unsigned address, unsigned size)
{
    if (size == 0)
        return;

    drvSpiFlash_t *flash = drvSpiFlashOpen(HAL_FLASH_DEVICE_NAME(address));
    if (flash != NULL)
    {
        drvSpiFlashSetRangeWriteProhibit(flash, HAL_FLASH_OFFSET(address),
                                         HAL_FLASH_OFFSET(address) + size);
    }
}

static void prvSetFlashWriteProhibit(void)
{
    // ATTENTION: This will set write prohibit for bootloader and AP codes
    //
    // If there are requiement (though not reasonable) to change bootloader
    // or AP codes, the followings should be changed. And when there are
    // more regions are known never changed, more regions can be added.

    prvSetFlashWriteProhibitByAddress(CONFIG_BOOT_FLASH_ADDRESS, CONFIG_BOOT_FLASH_SIZE);
    prvSetFlashWriteProhibitByAddress(CONFIG_APP_FLASH_ADDRESS, CONFIG_APP_FLASH_SIZE);

    // Modem partition is designed to be read only also.
    prvSetFlashWriteProhibitByAddress(CONFIG_FS_MODEM_FLASH_ADDRESS, CONFIG_FS_MODEM_FLASH_SIZE);
}

static void prvTraceInit(void)
{
    bool fstrace = false;
#ifdef CONFIG_FS_TRACE_ENABLE
    if (gSysnvFstraceMask & SRV_FSTRACE_OPT_AP_ENABLE)
        fstrace = true;
#endif

#ifdef CONFIG_KERNEL_HOST_TRACE
    drvDebugPortMode_t dhost_mode = {
        .protocol = DRV_DEBUG_PROTOCOL_HOST,
        .trace_enable = !fstrace && (gSysnvTraceDevice == OSI_TRACE_DEVICE_DEBUGHOST),
        .cmd_enable = true,
        .bs_enable = true,
    };
    drvDebugPort_t *dhost_port = drvDhostCreate(dhost_mode);
    drvHostCmdEngineCreate(dhost_port);

    drvDebugPortMode_t aplog_mode = {
        .protocol = DRV_DEBUG_PROTOCOL_HOST,
        .trace_enable = !fstrace && (gSysnvTraceDevice == OSI_TRACE_DEVICE_USBSERIAL),
        .cmd_enable = true,
        .bs_enable = true,
    };
    drvDebugPort_t *aplog_port = drvDebugUserialPortCreate(DRV_NAME_USRL_COM4, aplog_mode);
    drvHostCmdEngineCreate(aplog_port);

    unsigned diag_device = diagDeviceName();
#ifdef CONFIG_DIAG_DEVICE_USRL_SUPPORT
    if (DRV_NAME_IS_USRL(diag_device))
    {
        drvDebugPortMode_t diag_mode = {
            .protocol = DRV_DEBUG_PROTOCOL_DIAG,
            .trace_enable = false,
            .cmd_enable = true,
            .bs_enable = true,
        };
        drvDebugUserialPortCreate(diag_device, diag_mode);
    }
#endif
    if (DRV_NAME_IS_UART(diag_device))
    {
        drvDebugPortMode_t diag_mode = {
            .protocol = DRV_DEBUG_PROTOCOL_DIAG,
            .trace_enable = false,
            .cmd_enable = true,
            .bs_enable = true,
        };
        drvDebugUartPortCreate(diag_device, diag_mode);
    }

#ifdef CONFIG_ATR_UART_BLUE_SCREEN_DEBUG
    drvDebugPortMode_t atuart_mode = {
        .protocol = DRV_DEBUG_PROTOCOL_HOST,
        .trace_enable = false,
        .cmd_enable = true,
        .bs_enable = true,
        .bs_only = true,
    };
    drvDebugPort_t *atuart_port = drvDebugUartPortCreate(CONFIG_ATR_DEFAULT_UART, atuart_mode);
    drvHostCmdEngineCreate(atuart_port);
#endif
#endif

#ifdef CONFIG_KERNEL_DIAG_TRACE
    drvDebugPortMode_t dhost_mode = {
        .protocol = DRV_DEBUG_PROTOCOL_HOST,
        .trace_enable = false,
        .cmd_enable = true,
        .bs_enable = true,
    };
    drvDebugPort_t *dhost_port = drvDhostCreate(dhost_mode);
    drvHostCmdEngineCreate(dhost_port);

    unsigned diag_device = diagDeviceName();
#ifdef CONFIG_DIAG_DEVICE_USRL_SUPPORT
    if (DRV_NAME_IS_USRL(diag_device))
    {
        drvDebugPortMode_t diag_mode = {
            .protocol = DRV_DEBUG_PROTOCOL_DIAG,
            .trace_enable = !fstrace,
            .cmd_enable = true,
            .bs_enable = true,
        };
        drvDebugUserialPortCreate(diag_device, diag_mode);
    }
#endif
    if (DRV_NAME_IS_UART(diag_device))
    {
        drvDebugPortMode_t diag_mode = {
            .protocol = DRV_DEBUG_PROTOCOL_DIAG,
            .trace_enable = !fstrace,
            .cmd_enable = true,
            .bs_enable = true,
        };
        drvDebugUartPortCreate(diag_device, diag_mode);
    }
#endif
}

static void prvPowerOn(void *arg)
{
    ipcInit();
    drvNvmIpcInit();

    svcRfParamInit();

    ipc_at_init();
    drvPsPathInit();

#ifdef CONFIG_FS_MOUNT_SDCARD
    bool mount_sd = fsMountSdcard();
    OSI_LOGI(0, "application mount sd card %d", mount_sd);
#endif

#ifdef CONFIG_FS_TRACE_ENABLE
    if (gSysnvFstraceMask != 0)
        srvFstraceInit(gSysnvFstraceMask);
#endif

#ifdef CONFIG_SRV_SIMLOCK_ENABLE
    srvSimlockInit();
#endif

    _checkGpioCalibMode();

    unsigned diag_device = diagDeviceName();
    drvDebugPort_t *diag_port = drvDebugPortFindByName(diag_device);
    if (osiGetBootMode() == OSI_BOOTMODE_NORMAL)
    {
        diagRunMode_t run_mode = diagRunModeCheck(diag_port);
        if (run_mode == DIAG_RM_CALIB)
            osiSetBootMode(OSI_BOOTMODE_CALIB);
        else if (run_mode == DIAG_RM_CALIB_POST)
            osiSetBootMode(OSI_BOOTMODE_CALIB_POST);
        else if (run_mode == DIAG_RM_BBAT)
            osiSetBootMode(OSI_BOOTMODE_BBAT);
    }

    diagInit(diag_port);

    if (osiGetBootMode() == OSI_BOOTMODE_CALIB)
    {
        OSI_LOGI(0, "application enter calibration mode");
        drvChargeDisable();
        fsRemountFactory(0); // read-write
    }
    else if (osiGetBootMode() == OSI_BOOTMODE_CALIB_POST)
    {
        OSI_LOGI(0, "application enter calibration post mode");
        fsRemountFactory(0); // read-write
        diagCalibPostInit();
    }
    else if (osiGetBootMode() == OSI_BOOTMODE_BBAT)
    {
        OSI_LOGI(0, "application enter BBAT mode");
        fsRemountFactory(0); // read-write
        diagAutoTestInit();
    }
    else
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE    
        quec_usb_setting_read();
#endif        
        bool usbok = drvUsbSetWorkMode(gSysnvUsbWorkMode);
        OSI_LOGI(0, "application start usb mode %d/%d/%d", gSysnvUsbWorkMode, gSysnvUsbDetMode, usbok);
        if (usbok)
        {
            drvUsbEnable(CONFIG_USB_DETECT_DEBOUNCE_TIME);
        }
    }

    bool fstrace_cplog = false, fstrace_moslog = false;
#ifdef CONFIG_FS_TRACE_ENABLE
    if (gSysnvFstraceMask & (SRV_FSTRACE_OPT_CP_ENABLE | SRV_FSTRACE_OPT_ZSP_ENABLE))
        fstrace_cplog = true;
    if (gSysnvFstraceMask & SRV_FSTRACE_OPT_MOS_ENABLE)
        fstrace_moslog = true;
#endif

    if (!fstrace_moslog)
    {
        drvDebugPortMode_t moslog_mode = {
            .protocol = DRV_DEBUG_PROTOCOL_MOS_LOG,
            .bs_enable = true,
        };
        drvDebugUserialPortCreate(DRV_NAME_USRL_COM2, moslog_mode);
    }

    if (!fstrace_cplog)
    {
        drvDebugPortMode_t modemlog_mode = {
            .protocol = DRV_DEBUG_PROTOCOL_MODEM_LOG,
            .bs_enable = true,
        };
        drvDebugUserialPortCreate(DRV_NAME_USRL_COM3, modemlog_mode);
    }

#ifdef CONFIG_AT_BT_APP_SUPPORT
    if (!srvBtTraceInit())
        OSI_LOGE(0, "init bt log fail.");
#endif

    CFW_RpcInit();
    malInit();

#ifdef CONFIG_APP_START_ATR
    // asynchrous worker, start before at task
    aworker_start();
    atEngineStart();
#endif

    // zsp_uart & uart_3 are both for cp
    halIomuxSetFunction(HAL_IOMUX_FUN_ZSP_UART_TXD);
    halIomuxSetFunction(HAL_IOMUX_FUN_UART_3_TXD);
	
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO
    OSI_LOGI(0, "QUEC GPIO SET");
    quec_pin_cfg_init();
#ifndef CONFIG_QL_OPEN_EXPORT_PKG /* Standard */
    quec_gpio_interruput_init();
#endif
#ifdef CONFIG_BOARD_WITH_EXT_FLASH
    quec_ext_flash_init();
#endif
#endif


    if (!halCpLoad())
        osiPanic();

    audevInit();

#ifdef CONFIG_TTS_SUPPORT
    ttsPlayerInit();
#endif

#ifdef CONFIG_NET_TCPIP_SUPPORT
    net_init();
#ifdef CONFIG_NET_NAT_SUPPORT
    OSI_LOGE(0, "init nat to %d", gSysnvNATCfg);
    extern void set_nat_enable(uint32_t natCfg);
    set_nat_enable(gSysnvNATCfg);
    extern void netif_setup_lwip_lanOnly();
    netif_setup_lwip_lanOnly();
#endif
#endif

#ifndef CONFIG_QUEC_PROJECT_FEATURE

#if defined(CONFIG_APPIMG_LOAD_FLASH) && defined(CONFIG_APPIMG_FLASH_ADDRESS)
    const void *flash_img_address = (const void *)CONFIG_APPIMG_FLASH_ADDRESS;
    if (appImageFromMem(flash_img_address, &gAppImgFlash))
    {
        OSI_LOGI(0, "Find app image at 0x%x", flash_img_address);
        gAppImgFlash.enter(NULL);
    }
#endif

#if defined(CONFIG_APPIMG_LOAD_FILE) && defined(CONFIG_APPIMG_LOAD_FILE_NAME)
    if (appImageFromFile(CONFIG_APPIMG_LOAD_FILE_NAME, &gAppImgFile))
    {
        OSI_LOGI(0, "Find file image at " CONFIG_APPIMG_LOAD_FILE_NAME);
        gAppImgFile.enter(NULL);
    }
#endif

#endif

#ifdef CONFIG_AT_BT_APP_SUPPORT
    bt_ap_init();
#endif

    // HACK: Now CP will change hwp_debugUart->irq_mask. After CP is changed,
    // the followings should be removed.
    osiThreadSleep(1000);
    drvDhostRestoreConfig();
#ifdef CONFIG_HEADSET_DETECT_SUPPORT
    srvHeadsetInit();
#endif
}

void osiAppStart(void)
{
    OSI_LOGXI(OSI_LOGPAR_S, 0, "application start (%s)", gBuildRevision);

#ifdef CONFIG_SYS_WDT_ENABLE
    // Feed sys_wdt in low priority work queue, it is always enabled.
    halSysWdtStart();
    osiWork_t *sys_wdt_work = osiWorkCreate((osiCallback_t)halSysWdtStart, NULL, NULL);
    osiTimer_t *sys_wdt_timer = osiTimerCreateWork(sys_wdt_work, osiSysWorkQueueLowPriority());
    osiTimerStartPeriodicRelaxed(sys_wdt_timer, CONFIG_SYS_WDT_FEED_INTERVAL, OSI_DELAY_MAX);
#endif

#ifdef CONFIG_WDT_ENABLE
    srvWdtInit(CONFIG_APP_WDT_MAX_INTERVAL, CONFIG_APP_WDT_FEED_INTERVAL);
#ifndef CONFIG_QUEC_PROJECT_FEATURE
	srvWdtStart();
#endif
#endif

    osiInvokeGlobalCtors();
    prvSetFlashWriteProhibit();

    mlInit();
    if (!fsMountAll())
        osiPanic();

    osiBuffer_t bscore_buf = halGetBscoreBuf();
    if (bscore_buf.size > 0)
    {
        OSI_LOGI(0, "restart from blue screen");

        // When needed, the buffer can be copied to another place, and
        // send to server after network is ready.
    }
    halClearBscoreBuf(); // clear it no matter it is valid or not

    // besure ap nvm directory exist
    vfs_mkdir(CONFIG_FS_AP_NVM_DIR, 0);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    quec_fs_dir_check();
#endif
    osiSysnvLoad();
    drvAxidmaInit();
    prvTraceInit();
    nvmInit();
    nvmMigration();

#ifdef CONFIG_QUEC_PROJECT_FEATURE
	bool wdtrst = quec_wdt_cfg_read();
	if(ql_dev_cfg_wdt(wdtrst) != QL_DEV_SUCCESS)
	{
		OSI_LOGI(0, "cfg wdt failed");
	}
#endif

#ifndef CONFIG_QUEC_PROJECT_FEATURE
    fupdateStatus_t fup_status = fupdateGetStatus();
    if (fup_status == FUPDATE_STATUS_FINISHED)
    {
        char *old_version = NULL;
        char *new_version = NULL;
        if (fupdateGetVersion(&old_version, &new_version))
        {
            OSI_LOGXI(OSI_LOGPAR_SS, 0, "FUPDATE: %s -> %s",
                      old_version, new_version);
            free(old_version);
            free(new_version);
        }
        // fupdateInvalidate(true);
    }
#endif
    if (!halShareMemRegionLoad())
        osiPanic();

    osiPsmRestore();

    drvGpioInit();
    drvPmicIntrInit();
    drvRtcInit();
    drvAdcInit();
    drvChargerInit();
    srvKpdInit();
    if (!srvPmInit(prvPowerOn, NULL))
        osiPanic();

    srvUsbInit();
    srvPmRun();

    // wait a while for PM source created
    osiThreadSleep(10);
    osiPmStart();
	
#ifdef CONFIG_QUEC_PROJECT_FEATURE
	quec_startup();
#endif
}

#ifdef CONFIG_QL_OPEN_EXPORT_PKG
void quec_app_start(void)
{
#if defined(CONFIG_APPIMG_LOAD_FLASH) && defined(CONFIG_APPIMG_FLASH_ADDRESS)
    const void *flash_img_address = (const void *)CONFIG_APPIMG_FLASH_ADDRESS;
    if (appImageFromMem(flash_img_address, &gAppImgFlash))
    {
        OSI_LOGI(0, "Find app image at 0x%x", flash_img_address);
        gAppImgFlash.enter(NULL);
    }
#endif

#if defined(CONFIG_APPIMG_LOAD_FILE) && defined(CONFIG_APPIMG_LOAD_FILE_NAME)
    if (appImageFromFile(CONFIG_APPIMG_LOAD_FILE_NAME, &gAppImgFile))
    {
        OSI_LOGI(0, "Find file image at " CONFIG_APPIMG_LOAD_FILE_NAME);
        gAppImgFile.enter(NULL);
    }
#endif
}
#endif

