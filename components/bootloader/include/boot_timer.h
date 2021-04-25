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

#ifndef _BOOT_TIMER_IRQ_H_
#define _BOOT_TIMER_IRQ_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief timer callback function type
 *
 * Functions in bootloader shoudn't depend on interrupt. And there is
 * only one exception, using timer interrupt to feed external watchdog.
 * When external watchdog is used, there are no opportunity to feed
 * external watchdog during firmware update. So, it is needed to
 * enabled timer interrupt, and feed external watchdog in timer ISR.
 *
 * Add an exception in fdl2 download process, using timer interrupt to
 * check a normal uart communication for factory systematic process.
 */
typedef void (*bootTimerCallback_t)(void *ctx);

/**
 * \brief enable bootloader timer
 *
 * There is no critical section mechanism inside bootloader. So, the
 * timer callback shouldn't use shared resources with normal bootloader.
 *
 * Currently, the only purpose of timer interrupt callback is to feed
 * external watchdog.
 *
 * The callback shouldn't be located on flash, due to it is possible
 * there are flash erase and program in normal bootloader.
 *
 * \param cb        timer ISR callback
 * \paran ctx       the parameter callback function
 * \param period_ms timer period in milliseconds
 */
void bootEnableTimer(bootTimerCallback_t cb, void *ctx, int period_ms);

/**
 * \brief disable bootloader timer
 *
 * In order not to affect normal application, when bootloader timer is
 * enabled, it is needed to disable the timer before jump to normal
 * application.
 */
void bootDisableTimer(void);

#ifdef __cplusplus
}
#endif
#endif
