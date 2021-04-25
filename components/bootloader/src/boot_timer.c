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

#include "boot_timer.h"
#include "boot_irq.h"
#include "hwregs.h"
#include <stdlib.h>

#ifdef CONFIG_SOC_8910
#define TIMER_IRQN HAL_SYSIRQ_NUM(SYS_IRQ_ID_TIMER_4_OS)
#define TIMER_IRQ_PRIO SYS_IRQ_PRIO_TIMER_4_OS

typedef struct
{
    bootTimerCallback_t cb;
    void *ctx;
} bootTimerCbPrv_t;

static bootTimerCbPrv_t gTimerCb;

void bootTimerISR(void)
{
    hwp_timer4->timer_irq_clr = hwp_timer4->timer_irq_cause;
    if (gTimerCb.cb != NULL)
        gTimerCb.cb(gTimerCb.ctx);
}

void bootEnableTimer(bootTimerCallback_t cb, void *ctx, int period_ms)
{
    if (cb == NULL || period_ms == 0)
        return;

    gTimerCb.cb = cb;
    gTimerCb.ctx = ctx;

    REG_TIMER_AP_TIMER_IRQ_MASK_SET_T timer_irq_mask_set = {.b.ostimer_mask = 1};
    REG_TIMER_AP_OSTIMER_CTRL_T ostimer_ctrl = {
        .b.loadval_h = 0,
        .b.enable = 1,
        .b.repeat = 1,
        .b.wrap = 0,
        .b.load = 1,
    };
    hwp_timer4->ostimer_ctrl = 0;
    hwp_timer4->timer_irq_mask_set = timer_irq_mask_set.v;
    hwp_timer4->ostimer_loadval_l = period_ms * 2000; // 2MHz
    hwp_timer4->ostimer_ctrl = ostimer_ctrl.v;

    bootEnableIrq(TIMER_IRQN, TIMER_IRQ_PRIO);
}

void bootDisableTimer(void)
{
    bootDisableIrq(TIMER_IRQN);

    REG_TIMER_AP_TIMER_IRQ_MASK_CLR_T timer_irq_mask_clr = {.b.ostimer_mask = 1};
    hwp_timer4->timer_irq_mask_clr = timer_irq_mask_clr.v;

    gTimerCb.cb = NULL;
    gTimerCb.ctx = NULL;
}
#endif

#ifdef CONFIG_SOC_8811
void bootEnableTimer(bootTimerCallback_t cb, void *ctx, int period_ms)
{
}
#endif
