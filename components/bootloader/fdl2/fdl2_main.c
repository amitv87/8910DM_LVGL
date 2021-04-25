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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('F', 'D', 'L', '2')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "boot_fdl.h"
#include "boot_entry.h"
#include "boot_platform.h"
#include "boot_debuguart.h"
#include "boot_mem.h"
#include "boot_mmu.h"
#include "hal_adi_bus.h"
#include "hal_chip.h"
#include "boot_bsl_cmd.h"
#include "hal_config.h"
#include "cmsis_core.h"
#include "osi_log.h"
#include "drv_names.h"
#include <sys/reent.h>

void bootStart(uint32_t param)
{
    OSI_CLEAR_SECTION(bss);

    halClockInit();
    halRamInit();

    extern unsigned __mmu_ttbl1_start[];
    extern unsigned __mmu_ttbl2_start[];
    bootMmuEnable((uint32_t *)__mmu_ttbl1_start, (uint32_t *)__mmu_ttbl2_start);

    __FPU_Enable();
    _impure_ptr = _GLOBAL_REENT;

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
    bootHeapInit((uint32_t *)__sram_heap_start, sram_heap_size,
                 (uint32_t *)CONFIG_RAM_PHY_ADDRESS, CONFIG_RAM_SIZE);
    bootHeapDefaultExtRam();

    OSI_LOGI(0, "FDL2RUN device 0x%x ......", param);

    fdlChannel_t *ch = NULL;
    if (param == BOOT_DOWNLOAD_UART1)
        ch = fdlOpenUart(DRV_NAME_UART1, 0, false);
    else if (param == BOOT_DOWNLOAD_UART2)
        ch = fdlOpenUart(DRV_NAME_UART2, 0, false);
    else if (param == BOOT_DOWNLOAD_USERIAL)
        ch = fdlOpenUsbSerial();

    if (ch == NULL)
    {
        OSI_LOGE(0, "FDL2 fail to open device");
        osiPanic();
    }

    fdlEngine_t *fdl = fdlEngineCreate(ch, CONFIG_FDL_PACKET_MAX_LEN);
    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);

    fdlDnldStart(fdl, param);

    // never return here
    osiPanic();
}
