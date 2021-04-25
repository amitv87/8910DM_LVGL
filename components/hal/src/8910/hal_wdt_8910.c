/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "hal_chip.h"
#include "hwregs.h"
#include "osi_api.h"

#define WDT_TICK_FREQ (16384)
#define WDT_TICK_MAX (0xffffff)
#define WDT_MS_TO_TICK(ms) OSI_UMULDIV(ms, WDT_TICK_FREQ, 1000)
#define WDT_MS_MAX OSI_UMULDIV(WDT_TICK_MAX, 1000, WDT_TICK_FREQ)

void halSysWdtConfig(unsigned intr_ms)
{
#ifdef CONFIG_SYS_WDT_ENABLE
    unsigned intr_tick = (intr_ms > WDT_MS_MAX) ? WDT_TICK_MAX : WDT_MS_TO_TICK(intr_ms);

    REGT_FIELD_WRITE(hwp_timer2->ostimer_ctrl,
                     REG_TIMER_OSTIMER_CTRL_T,
                     loadval, intr_tick,
                     enable, 0,
                     repeat, 0,
                     wrap, 0,
                     load, 1);

    REGT_FIELD_WRITE(hwp_timer2->timer_irq_mask_set,
                     REG_TIMER_TIMER_IRQ_MASK_CLR_T,
                     ostimer_mask, 1);
#endif
}

void halSysWdtStart(void)
{
#ifdef CONFIG_SYS_WDT_ENABLE
    REGT_FIELD_CHANGE(hwp_timer2->ostimer_ctrl,
                      REG_TIMER_OSTIMER_CTRL_T,
                      enable, 1,
                      load, 1);
#endif
}

void halSysWdtStop(void)
{
#ifdef CONFIG_SYS_WDT_ENABLE
    REGT_FIELD_CHANGE(hwp_timer2->ostimer_ctrl,
                      REG_TIMER_OSTIMER_CTRL_T,
                      enable, 0);
#endif
}
