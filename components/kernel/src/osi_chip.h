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
#define __OSI_CHIP_H__

#include "kernel_config.h"
#include "hal_chip.h"
#include "osi_api.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

OSI_EXTERN_C_BEGIN

void osiChipPmReorder(void);

/**
 * \brief whether suspend is permitted
 *
 * \return
 *      - true if suspend is permitted
 *      - false if not permitted
 */
bool osiChipSuspendPermitted(void);

/**
 * \brief called before light sleep
 */
void osiChipLightSleepEnter(void);

/**
 * \brief called after light sleep
 */
void osiChipLightSleepExit(void);

/**
 * \brief called after all suspend callbacks
 */
void osiChipSuspend(osiSuspendMode_t mode);

/**
 * \brief called before all resume callbacks
 */
void osiChipResume(osiSuspendMode_t mode, uint32_t source);

/**
 * \brief called after all resume callbacks
 */
void osiChipResumePost(osiSuspendMode_t mode, uint32_t source);

/**
 * \brief CPU suspend
 *
 * Usually, it shouldn't be called directly by application. Rather, it is
 * implemented by BSP, and called by \p osiPmSuspend.
 *
 * \param mode  suspend mode
 * \param ms    suspend time in milliseconds
 * \return
 *      suspend resume source. \p OSI_RESUME_ABORT bit indicates suspend
 *      aborted.
 */
uint32_t osiPmCpuSuspend(osiSuspendMode_t mode, int64_t ms);

/**
 * \brief wakeup entry
 */
void OSI_NO_RETURN osiWakeEntry(void);

static inline bool osiChipShutdownModeSupported(osiShutdownMode_t mode) { return true; }

OSI_EXTERN_C_END

#if defined(CONFIG_SOC_8910)
#include "8910/osi_chip_8910.h"
#elif defined(CONFIG_SOC_8955)
#include "8955/osi_chip_8955.h"
#elif defined(CONFIG_SOC_8909)
#include "8909/osi_chip_8909.h"
#endif

#endif
