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
#include "osi_api.h"
#include "osi_trace.h"
#include "osi_sysnv.h"
#include "unity_fixture.h"
#include "drv_uart.h"
#include "drv_axidma.h"
#include "drv_debug_port.h"
#include "drv_host_cmd.h"
#include "hal_config.h"
#include "atr_config.h"
#include "unity_port.h"

static void prvUnityWorkEntry(void *ctx)
{
    OSI_LOGI(0, "unity start");

    osiInvokeGlobalCtors();
#ifdef CONFIG_SOC_8910
    drvAxidmaInit();
#endif
    osiTraceSetEnable(gSysnvTraceEnabled);

    drvDebugPortMode_t dhost_mode = {
        .protocol = DRV_DEBUG_PROTOCOL_HOST,
        .trace_enable = true,
        .cmd_enable = true,
        .bs_enable = true,
    };
    drvDebugPort_t *dhost_port = drvDhostCreate(dhost_mode);
    drvHostCmdEngineCreate(dhost_port);

    unityInit();
    unityOpenUart(CONFIG_ATR_DEFAULT_UART, CONFIG_ATR_DEFAULT_UART_BAUD);

    const char *argv[] = {"", "-v"};
    UnityMain(2, argv, UnityRunAllTests);
}

void unityAppStart(void)
{
    osiWork_t *work = osiWorkCreate(prvUnityWorkEntry, NULL, NULL);
    osiWorkQueue_t *wq = osiWorkQueueCreate("unity", 1, OSI_PRIORITY_BELOW_NORMAL, 32 * 1024);
    osiWorkEnqueue(work, wq);
}
