/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('F', 'D', 'L', '1')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "boot_fdl.h"
#include "boot_entry.h"
#include "boot_platform.h"
#include "boot_debuguart.h"
#include "boot_mem.h"
#include "boot_spi_flash.h"
#include "boot_bsl_cmd.h"
#include "hal_chip.h"
#include "boot_secure.h"
#include "hal_config.h"
#include "hal_spi_flash.h"
#include "hal_adi_bus.h"
#include "cmsis_core.h"
#include "osi_log.h"
#include "osi_api.h"
#include "drv_names.h"
#include <sys/reent.h>
#include <stdint.h>
#include <string.h>
#include <machine/endian.h>
#include "image_header.h"

typedef struct
{
    unsigned dnld_device;
    fdlDataDnld_t dnld;
} fdl1Context_t;

static void prvSetBaud(fdlEngine_t *fdl, fdlPacket_t *pkt)
{
    uint32_t *ptr = (uint32_t *)pkt->content;
    uint32_t baud = __ntohl(*ptr++);
    OSI_LOGD(0, "FDL1: change baud %d", baud);

    // This is special, ACK must be sent in old baud rate.
    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
    osiDelayUS(200);

    fdlEngineSetBaud(fdl, baud);
}

static void prvDataStart(fdlEngine_t *fdl, fdlPacket_t *pkt, fdlDataDnld_t *dnld)
{
    uint32_t *ptr = (uint32_t *)pkt->content;
    uint32_t start_addr = __ntohl(*ptr++);
    uint32_t file_size = __ntohl(*ptr++);

    OSI_LOGI(0, "FDL1: start_addr 0x%x, size %d ......", start_addr, file_size);
    if (CONFIG_FDL2_IMAGE_START == start_addr)
    {
        if (file_size > CONFIG_FDL2_IMAGE_SIZE)
        {
            OSI_LOGE(0, "fdl2 size beyond the limit...");

            fdlEngineSendRespNoData(fdl, BSL_REP_DOWN_SIZE_ERROR);
            osiPanic();
        }

        memset((void *)start_addr, 0, file_size);

        fdlEngineSendRespNoData(fdl, BSL_REP_ACK);

        dnld->start_address = start_addr;
        dnld->write_address = start_addr;
        dnld->total_size = file_size;
        dnld->received_size = 0;
        dnld->stage = SYS_STAGE_START;
    }
    else
    {
        OSI_LOGE(0, "fdl2 addr error...");
        fdlEngineSendRespNoData(fdl, BSL_REP_DOWN_DEST_ERROR);
        osiPanic();
    }
}

static void prvDataMidst(fdlEngine_t *fdl, fdlPacket_t *pkt, fdlDataDnld_t *dnld)
{
    uint16_t data_len = pkt->size;
    if ((dnld->stage != SYS_STAGE_START) && (dnld->stage != SYS_STAGE_GATHER))
    {
        fdlEngineSendRespNoData(fdl, BSL_REP_DOWN_NOT_START);
        OSI_LOGE(0, "FDL1: down start err...");
        osiPanic();
    }

    if (dnld->received_size + data_len > dnld->total_size)
    {
        fdlEngineSendRespNoData(fdl, BSL_REP_DOWN_SIZE_ERROR);
        OSI_LOGE(0, "FDL1: down size err...");
        osiPanic();
    }

    OSI_LOGV(0, "write addr %x, size %d", dnld->write_address, data_len);
    memcpy((void *)dnld->write_address, (const void *)pkt->content, data_len);
    dnld->write_address += data_len;
    dnld->received_size += data_len;
    dnld->stage = SYS_STAGE_GATHER;

    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
}

static void prvDataEnd(fdlEngine_t *fdl, fdlDataDnld_t *dnld)
{
    if (bootSecureUimageSigCheck((void *)dnld->start_address))
    {
        //recv all packets
        OSI_LOGD(0, "FDL1: write file ok...");
        fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
        dnld->stage = SYS_STAGE_END;
    }
    else
    {
        OSI_LOGD(0, "FDL1: secure boot verify fail...");
        fdlEngineSendRespNoData(fdl, BSL_REP_SEC_VERIFY_ERROR);
    }
}

static void prvDataExec(uint32_t param)
{
    const image_header_t *header = (const image_header_t *)CONFIG_FDL2_IMAGE_START;
    if (__ntohl(header->ih_magic) == IH_MAGIC)
    {
        bootJumpFunc_t entry = (bootJumpFunc_t)__ntohl(header->ih_ep);
        bootJumpImageEntry(param, entry);
    }
}

static void prvProcess(fdlEngine_t *fdl, fdlPacket_t *pkt, void *f_)
{
    if (fdl == NULL || pkt == NULL)
        osiPanic();

    fdl1Context_t *f = (fdl1Context_t *)f_;
    switch (pkt->type)
    {
    case BSL_CMD_CONNECT:
        fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
        break;

    case BSL_CMD_CHANGE_BAUD:
        prvSetBaud(fdl, pkt);
        break;

    case BSL_CMD_START_DATA:
        prvDataStart(fdl, pkt, &f->dnld);
        break;

    case BSL_CMD_MIDST_DATA:
        prvDataMidst(fdl, pkt, &f->dnld);
        break;

    case BSL_CMD_END_DATA:
        prvDataEnd(fdl, &f->dnld);
        break;

    case BSL_CMD_EXEC_DATA:
        prvDataExec(f->dnld_device);
        break;

    default:
        OSI_LOGE(0, "FDL1, cmd not support yet %x", pkt->type);
        osiPanic();
        break;
    }
}

void bootStart(uint32_t param)
{
    OSI_CLEAR_SECTION(bss);

    halClockInit();

    __FPU_Enable();
    _impure_ptr = _GLOBAL_REENT;

    // It is MAGIC!! ROM will jump here with parameter of 0
    if (param == 0)
    {
        if (OSI_REG32_READ(0x80c6c4) == 0x80c140)
            param = BOOT_DOWNLOAD_UART2;
        else if (OSI_REG32_READ(0x80c6c4) == 0x80c264)
            param = BOOT_DOWNLOAD_USERIAL;
    }

    bool trace_enable = false;
#ifdef CONFIG_FDL_LOG_ENABLED
    trace_enable = true;
#if defined(CONFIG_KERNEL_DIAG_TRACE) && defined(CONFIG_DIAG_DEFAULT_UART)
    if ((param == BOOT_DOWNLOAD_UART1 && CONFIG_DIAG_DEFAULT_UART == DRV_NAME_UART1) ||
        (param == BOOT_DOWNLOAD_UART2 && CONFIG_DIAG_DEFAULT_UART == DRV_NAME_UART2))
        trace_enable = false;
#endif
#endif
    bootTraceInit(trace_enable);
    halAdiBusInit();

    extern unsigned __sram_heap_start[];
    extern unsigned __sram_heap_end[];
    unsigned sram_heap_size = OSI_PTR_DIFF(__sram_heap_end, __sram_heap_start);
    bootHeapInit((uint32_t *)__sram_heap_start, sram_heap_size, 0, 0);

    OSI_LOGI(0, "FDL1RUN ......");

    bootResetPinEnable();

    OSI_LOGI(0, "FDL1 device 0x%x", param);

    fdlChannel_t *ch;
    if (param == BOOT_DOWNLOAD_UART1)
        ch = fdlOpenUart(DRV_NAME_UART1, 0, false);
    else if (param == BOOT_DOWNLOAD_UART2)
        ch = fdlOpenUart(DRV_NAME_UART2, 0, false);
    else
        ch = fdlOpenUsbSerial();

    if (ch == NULL)
    {
        OSI_LOGE(0, "FDL1 failed to open device");
        osiPanic();
    }

    fdlEngine_t *fdl = fdlEngineCreate(ch, CONFIG_FDL1_PACKET_MAX_LEN);
    if (fdl == NULL)
    {
        OSI_LOGE(0, "FDL1 failed to create engine");
        osiPanic();
    }

    // Wait sync and send version
    for (;;)
    {
        uint8_t sync;
        if (fdlEngineReadRaw(fdl, &sync, 1) > 0 && sync == HDLC_FLAG)
        {
            if (!fdlEngineSendVersion(fdl))
            {
                OSI_LOGE(0, "FDL fail to send version string");
                osiPanic();
            }
            break;
        }
    }

    fdl1Context_t ctx = {.dnld_device = param};
    fdlEngineProcess(fdl, prvProcess, NULL, &ctx);

    // never return here
    osiPanic();
}
