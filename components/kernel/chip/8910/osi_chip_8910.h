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

#ifndef __OSI_CHIP_H__
#error "osi_chip_8910.h can only be included by osi_chip.h"
#endif

#include "hwregs.h"
#include "osi_tick_unit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOADVAL_MIN 0 // 0 can trigger timer interrupt also
#define TIMER_IRQN HAL_SYSIRQ_NUM(SYS_IRQ_ID_TIMER_4_OS)
#define TIMER_IRQ_PRIO SYS_IRQ_PRIO_TIMER_4_OS
#define IDLE_IRQN HAL_SYSIRQ_NUM(SYS_IRQ_ID_CP_IDLE_2_H)
#define IDLE_IRQ_PRIO SYS_IRQ_PRIO_CP_IDLE_H

void osiChipIdleISR(void *param);

static inline void osiChipTimerDisable(void)
{
    hwp_timer4->ostimer_ctrl = 0;
}

static inline void osiChipTimerIrqClear(void)
{
    hwp_timer4->timer_irq_clr = hwp_timer4->timer_irq_cause;
}

static inline void osiChipTimerReload(int64_t loadval)
{
    if (loadval < LOADVAL_MIN)
        loadval = LOADVAL_MIN;

    REG_TIMER_AP_OSTIMER_CTRL_T ctrl = {
        .b.loadval_h = loadval >> 32,
        .b.enable = 1,
        .b.repeat = 0,
        .b.wrap = 1,
        .b.load = 1,
    };
    hwp_timer4->ostimer_ctrl = 0;
    hwp_timer4->ostimer_loadval_l = (uint32_t)loadval & 0xffffffff;
    hwp_timer4->ostimer_ctrl = ctrl.v;
}

static inline void osiChipTimerStart(osiCallback_t cb, void *ctx)
{
    REG_TIMER_AP_TIMER_IRQ_MASK_SET_T timer_irq_mask_set;
    hwp_timer4->ostimer_ctrl = 0;
    REG_FIELD_WRITE1(hwp_timer4->timer_irq_mask_set, timer_irq_mask_set, ostimer_mask, 1);
    osiIrqSetHandler(TIMER_IRQN, cb, ctx);
    osiIrqSetPriority(TIMER_IRQN, TIMER_IRQ_PRIO);
    osiIrqEnable(TIMER_IRQN);

    osiIrqSetHandler(IDLE_IRQN, osiChipIdleISR, NULL);
    osiIrqSetPriority(IDLE_IRQN, IDLE_IRQ_PRIO);
    osiIrqEnable(IDLE_IRQN);
}

uint32_t osiChip32KSleep(int64_t sleep_ms);

#ifdef __cplusplus
}
#endif
