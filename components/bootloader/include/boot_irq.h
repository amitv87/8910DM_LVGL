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

#ifndef _BOOT_IRQ_H_
#define _BOOT_IRQ_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief function type of irq handler
 */
typedef void (*bootIrqHandler_t)(void *ctx);

/**
 * \brief enable interrupt
 */
void bootEnableInterrupt(void);

/**
 * \brief disable interrupt
 *
 * If interrupt is enabled in bootloader, it is needed to disable interrupt
 * before jump to the application.
 */
void bootDisableInterrupt(void);

/**
 * \brief enable specified IRQ
 *
 * \param irqn      IRQ number
 * \param prio      IRQ priority
 */
void bootEnableIrq(uint32_t irqn, uint32_t prio);

/**
 * \brief disable specified IRQ
 *
 * Before bootloader jumps to application, all enabled IRQ should be disabled.
 *
 * \param irqn      IRQ number
 */
void bootDisableIrq(uint32_t irqn);

/**
 * \brief set irq handler
 *
 * \param irqn      IRQ number
 * \param handler   IRQ handler
 * \param ctx       IRQ handler context pointer
 * \return
 *      - true on success
 *      - false on invalid parameters
 */
void bootIrqSetHandler(uint32_t irqn, bootIrqHandler_t handler, void *ctx);

#ifdef __cplusplus
}
#endif
#endif
