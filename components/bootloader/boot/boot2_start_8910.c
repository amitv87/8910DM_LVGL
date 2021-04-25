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

#include <stdint.h>
#include <machine/endian.h>
#include "hal_config.h"
#include "boot_config.h"
#include "boot_platform.h"
#include "boot_fdl.h"
#include <sys/reent.h>
#include "hal_chip.h"
#include "boot_debuguart.h"
#include "boot_mem.h"
#include "boot_entry.h"
#include "cmsis_core.h"
#include "boot_secure.h"
#include "boot_spi_flash.h"
#include "boot_vfs.h"
#include "boot_mmu.h"
#include "boot_timer.h"
#include "boot_irq.h"
#include "boot_bsl_cmd.h"
#include "boot_pdl.h"
#include "hal_adi_bus.h"
#include "hal_spi_flash.h"
#include "flash_block_device.h"
#include "fupdate.h"
#include "fs_mount.h"
#include "osi_log.h"
#include "osi_api.h"
#include "drv_names.h"
#include "image_header.h"
#include "quec_boot_fota.h"

#ifdef CONFIG_QUEC_PROJECT_FEATURE
#include "quec_boot_uart.h"
#include "stdlib.h"
#include "string.h"
#endif

#define PMU_BOOT_MODE_REG (hwp_rda2720mGlobal->por_rst_monitor)

static struct _reent gZeroedReent;
struct _reent *_impure_ptr = &gZeroedReent;
int updata_urc_num = 0;

static void prvFirmwareUpdateProgress(int block_count, int block)
{
    OSI_LOGI(0, "FUPDATE block: %d/%d", block, block_count);

#ifdef CONFIG_QUEC_PROJECT_FEATURE
    if(quec_verify_output(block_count, block) && updata_urc_num > 0)
    {
        char percent_ch[4]; 
        char buf_uart[40] = "+QIND: \"FOTA\",\"UPDATING\",";    
        int  percent = block*100/block_count;    

        //sprintf cannot be used in bootloader, otherwise it will flash overflow
        itoa(percent, percent_ch, 10); 
        strcat(buf_uart, percent_ch);
        strcat(buf_uart, "\r\n\r\n");    
        quec_boot_urc_send((unsigned char*)buf_uart, strlen((char *)buf_uart));
    }
#endif
}

static void prvFirmwareUpdate(void)
{
    boot_fota_nv_cfg_param_t boot_fota_param;
    fsMountSetScenario(FS_SCENRARIO_BOOTLOADER);
    if (!fsMountAll())
        return;
    OSI_LOGI(0, "FUPDATE PACKNAME: %s", gFupdatePackFileName);
    if(prvQuec_Load_Fota_nv_Cfg(&boot_fota_param) == false)
    {
        OSI_LOGI(0, "FUPDATE pack name use default!");
    }
    else
    {
        gFupdatePackFileName = (const char*)boot_fota_param.PackFileName;
        updata_urc_num = boot_fota_param.updata_urc_num;
        OSI_LOGI(0, "FUPDATE NV CFG :%d, %x",boot_fota_param.updata_urc_num,*gFupdatePackFileName);
    }   
    
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    if(updata_urc_num > 0)
    {
        quec_boot_urc_init();
    }
#endif 

    //urc 上报fota start
    fupdateResult_t result = fupdateRun(prvFirmwareUpdateProgress);

    OSI_LOGI(0, "FUPDATE: %d", result);
    //urc 上报fota end err_code  
    if (result == FUPDATE_RESULT_FAILED)
        bootReset(BOOT_RESET_NORMAL);
}

#ifdef CONFIG_BOOT_TIMER_IRQ_ENABLE
static void prvTimerCallback(void)
{
    // usually, just feed external watchdog here
}
#endif

static const image_header_t *prvAppUimageHeader(void)
{
    return (const image_header_t *)CONFIG_APP_FLASH_ADDRESS;
}

static void prvSetFlashWriteProhibit(void)
{
    // ATTENTION: This will set write prohibit for bootloader
    //
    // If there are requiement (though not reasonable) to change bootloader
    // the followings should be changed. And when there are more regions are
    // known never changed, more regions can be added.

    bootSpiFlash_t *flash = bootSpiFlashOpen(HAL_FLASH_DEVICE_NAME(CONFIG_BOOT_FLASH_ADDRESS));
    bootSpiFlashSetRangeWriteProhibit(flash, HAL_FLASH_OFFSET(CONFIG_BOOT_FLASH_ADDRESS),
                                      HAL_FLASH_OFFSET(CONFIG_BOOT_FLASH_ADDRESS) + CONFIG_BOOT_FLASH_SIZE);
}

static bool prvCheckNormalPowerUp()
{
#ifndef CONFIG_CHARGER_POWERUP
    return (bootPowerOnCause() != OSI_BOOTCAUSE_CHARGE);
#endif
    return true;
}

#ifdef CONFIG_BOOT_SMPL_ENABLE
static bool prvSmplEnable(void)
{
    uint32_t smpl_mode;

#define TIMER_THRESHOLD_MASK 0x7
#define SMPL_ENBLAE 0x1935

    smpl_mode = ((CONFIG_BOOT_SMPL_THRESHOLD & TIMER_THRESHOLD_MASK) << 13) | SMPL_ENBLAE;

    REG_RDA2720M_GLOBAL_SMPL_CTRL0_T smpl_ctrl0 = {};
    smpl_ctrl0.b.smpl_mode = smpl_mode;
    halAdiBusWrite(&hwp_rda2720mGlobal->smpl_ctrl0, smpl_ctrl0.v);

    REG_RDA2720M_GLOBAL_SMPL_CTRL1_T smpl_ctrl1 = {halAdiBusRead(&hwp_rda2720mGlobal->smpl_ctrl1)};

    REG_ADI_WAIT_FIELD_NEZ(smpl_ctrl1, hwp_rda2720mGlobal->smpl_ctrl1, smpl_mode_wr_ack_flag);

    return smpl_ctrl1.b.smpl_en == 1 ? true : false;
}
#endif

#ifdef CONFIG_BOOT_UART1_PDL_ENABLE
static void prvUart1Download(void)
{
    bool download = false;
    uint32_t boot_mode_val = halAdiBusRead(&PMU_BOOT_MODE_REG);
    if (boot_mode_val == OSI_SHUTDOWN_BL_DOWNLOAD)
    {
        download = true;
        halAdiBusWrite(&PMU_BOOT_MODE_REG, 0);
    }

    uint32_t rom_flags = hwp_idle->idle_res5;
    if (rom_flags == 0xdeadcafe)
        download = true;

    if (!download)
        return;

    OSI_LOGI(0, "UART1 download, magic/0x%04x/0x%08x", boot_mode_val, rom_flags);
    fdlChannel_t *ch = fdlOpenUart(DRV_NAME_UART1, 921600, true);
    if (ch == NULL)
    {
        OSI_LOGE(0, "Failed to open %4c", DRV_NAME_UART1);
        osiPanic();
    }

    if (!pdlDnldUimage(ch, BOOT_DOWNLOAD_UART1, CONFIG_FDL1_IMAGE_START, CONFIG_FDL1_IMAGE_SIZE, 10000))
    {
        OSI_LOGE(0, "%4c PDL download failed", DRV_NAME_UART1);
        osiPanic();
    }

    OSI_LOGI(0, "%4c PDL download timeout", DRV_NAME_UART1);
}
#endif

void bootStart(uint32_t param)
{
    OSI_CLEAR_SECTION(bss);

    bootSpiFlashOpen(DRV_NAME_SPI_FLASH); // ensure accessible
    halClockInit();
    halRamInit();

    extern unsigned __mmu_ttbl1_start[];
    extern unsigned __mmu_ttbl2_start[];
    bootMmuEnable((uint32_t *)__mmu_ttbl1_start, (uint32_t *)__mmu_ttbl2_start);

    __FPU_Enable();
    _REENT_INIT_PTR_ZEROED(_impure_ptr);

    halAdiBusInit();

#ifdef CONFIG_BOOT_SMPL_ENABLE
    if (!prvSmplEnable())
        OSI_LOGI(0, "SMPL enable fail");
#endif

    bootResetPinEnable();

    const image_header_t *header = prvAppUimageHeader();
    if (bootIsFromPsmSleep())
    {
        bootJumpFunc_t entry = (bootJumpFunc_t)__ntohl(header->ih_ep);
        osiDCacheCleanInvalidateAll();
        bootJumpImageEntry(param, entry);
    }

    if (!prvCheckNormalPowerUp())
    {
        osiDelayUS(1000 * 10);
        bootPowerOff();
    }

    bool trace_enable = false;
#ifdef CONFIG_BOOT_LOG_ENABLED
    trace_enable = true;
#endif
    bootTraceInit(trace_enable);

    extern unsigned __sram_heap_start[];
    extern unsigned __sram_heap_end[];
    unsigned sram_heap_size = OSI_PTR_DIFF(__sram_heap_end, __sram_heap_start);
    bootHeapInit((uint32_t *)__sram_heap_start, sram_heap_size,
                 (uint32_t *)CONFIG_RAM_PHY_ADDRESS, CONFIG_RAM_SIZE);
    bootHeapDefaultExtRam();

#ifdef CONFIG_BOOT_TIMER_IRQ_ENABLE
    bootEnableInterrupt();
    bootEnableTimer(prvTimerCallback, CONFIG_BOOT_TIMER_PERIOD);
#endif

#ifdef CONFIG_BOOT_UART1_PDL_ENABLE
    prvUart1Download();
#endif

    halPmuExtFlashPowerOn();
    prvSetFlashWriteProhibit();
    prvFirmwareUpdate();

    uint32_t sync_cnt = 0;
    uint32_t timeout = 50; // ms

    fdlChannel_t *ch = fdlOpenUart(CONFIG_FDL_DEFAULT_UART, CONFIG_FDL_UART_BAUD, true);
    if (ch == NULL)
    {
        OSI_LOGE(0, "BOOT fail, can't open uart fdl channel");
        osiPanic();
    }

    fdlEngine_t *fdl = fdlEngineCreate(ch, CONFIG_FDL_PACKET_MAX_LEN);
    if (fdl == NULL)
    {
        OSI_LOGE(0, "BOOT fail, can not create fdl engine");
        osiPanic();
    }

    osiElapsedTimer_t elapsed;
    osiElapsedTimerStart(&elapsed);
    for (;;)
    {
        uint8_t sync;
        if (fdlEngineReadRaw(fdl, &sync, 1))
        {
            if (sync == HDLC_FLAG)
            {
                sync_cnt++;
                timeout = 150; // ms
            }
            OSI_LOGI(0, "BOOT recv 0x%x , recv 0x7e cnt %d", sync, sync_cnt);
        }
        if (sync_cnt >= 3)
        {
            fdlEngineSendVersion(fdl);

            OSI_LOGI(0, "ENTER DOWNLOAD MODE ");
            osiDelayUS(50 * 1000);
            bootReset(BOOT_RESET_FORCE_DOWNLOAD);
            //or download fdl1 and run fdl1 TODO
            break;
        }

        if (osiElapsedTime(&elapsed) > timeout)
            break;
    }

    if (__ntohl(header->ih_magic) == IH_MAGIC && bootSecureUimageSigCheck((void *)header))
    {
#ifdef CONFIG_BOOT_TIMER_IRQ_ENABLE
        bootDisableTimer();
        bootDisableInterrupt();
#endif
        bootJumpFunc_t entry = (bootJumpFunc_t)__ntohl(header->ih_ep);
        osiDCacheCleanInvalidateAll();
        bootJumpImageEntry(param, entry);
    }

    osiDebugEvent(0xdead0000);
    OSI_DEAD_LOOP;
}
