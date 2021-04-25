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

#include "osi_api.h"
#include "osi_profile.h"
#include "osi_log.h"
#include "osi_trace.h"
#include "hwregs.h"
#include "hal_blue_screen.h"
#include "drv_md_ipc.h"
#include "hal_chip.h"
#include "kernel_config.h"
#include <assert.h>

typedef struct
{
    osiCallback_t enter;
    osiCallback_t poll;
    void *param;
} osiBlueScreenHandler_t;

#ifdef CONFIG_WDT_ENABLE
//quectel update: init as false
bool gBsWatchdogReset = false;
#endif

static bool gIsPanic = false;
static osiBlueScreenHandler_t gBlueScreenHandlers[CONFIG_KERNEL_BLUE_SCREEN_HANDLER_COUNT];

OSI_NO_RETURN OSI_SECTION_SRAM_TEXT void osiPanic(void)
{
    void *address = __builtin_return_address(0);
    OSI_LOGE(0, "!!! PANIC AT %p", address);
    osiProfileCode(PROFCODE_PANIC);
    __builtin_trap();
}

OSI_NO_RETURN OSI_SECTION_SRAM_TEXT void osiPanicAt(void *address)
{
    OSI_LOGE(0, "!!! PANIC AT %p", address);
    osiProfileCode(PROFCODE_PANIC);
    __builtin_trap();
}

bool OSI_SECTION_SRAM_TEXT osiIsPanic(void)
{
    return gIsPanic;
}

OSI_NO_RETURN void osiBlueScreen(void *regs)
{
    (void)osiIrqSave(); // disable interrupt

    bool first_panic = !gIsPanic;
    gIsPanic = true;

    if (first_panic)
    {
#ifdef CONFIG_SOC_8910
        ipc_show_cp_assert();
        ipc_notify_cp_assert();
#endif

        halBlueScreenSaveContext(regs);
        halBlueScreenSaveInfo(regs);
        halBscoreBufSave(regs);

        osiDCacheCleanInvalidateAll();
        osiProfileCode(PROFCODE_BLUE_SCREEN);
        osiDCacheCleanInvalidateAll();

        // sys_wdt is just to trigger blue screen
        halSysWdtStop();

        for (unsigned n = 0; n < CONFIG_KERNEL_BLUE_SCREEN_HANDLER_COUNT; n++)
        {
            if (gBlueScreenHandlers[n].enter != NULL)
                gBlueScreenHandlers[n].enter(gBlueScreenHandlers[n].param);
        }

        osiShowThreadState();
    }

#ifdef CONFIG_WDT_ENABLE
    if (gBsWatchdogReset)
        halShutdown(OSI_SHUTDOWN_PANIC, 0);
#endif

    for (;;)
    {
        for (unsigned n = 0; n < CONFIG_KERNEL_BLUE_SCREEN_HANDLER_COUNT; n++)
        {
            if (gBlueScreenHandlers[n].poll != NULL)
                gBlueScreenHandlers[n].poll(gBlueScreenHandlers[n].param);
        }
        osiDCacheCleanInvalidateAll();
    }
}

bool osiRegisterBlueScreenHandler(osiCallback_t enter, osiCallback_t poll, void *param)
{
    if (enter == NULL && poll == NULL)
        return false;

    for (unsigned n = 0; n < CONFIG_KERNEL_BLUE_SCREEN_HANDLER_COUNT; n++)
    {
        osiBlueScreenHandler_t *p = &gBlueScreenHandlers[n];
        if (p->enter == NULL && p->poll == NULL)
        {
            p->enter = enter;
            p->poll = poll;
            p->param = param;
            return true;
        }
    }
    return false;
}
