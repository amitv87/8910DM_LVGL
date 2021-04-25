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

#ifndef _HAL_HWSPINLOCK_H_
#define _HAL_HWSPINLOCK_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * HW spinlock ID for ADI bus access
 */
#define HAL_HWSPINLOCK_ID_ADIBUS 0

/**
 * HW spinlock ID for CP sleep
 *
 * It is not a typical spinlock. CP will acquire the spinlock during normal
 * mode. It will only release spinlock when cp is ready for sleep. Also,
 * after WFI, cp will acquire spinlock before go forward. AP can use the
 * spinlock to prevent CP go forward when shared resources (such as PSRAM)
 * is unavailable.
 *
 * CP sleep protocol is through IPC bit10. This spinlock can be used for
 * double checking.
 */
#define HAL_HWSPINLOCK_ID_CPSLEEP 5

/**
 * HW spinlock ID for EFUSE access
 */
#define HAL_HWSPINLOCK_ID_EFUSE 8

/**
 * HW spinlock ID for IPC
 */
#define HAL_HWSPINLOCK_ID_IPC 9

/**
 * @brief acquire HW spinlock and enter critical
 *
 * @param id        HW spinlock id
 * @return  critical flag
 */
uint32_t halHwspinlockAcquire(uint32_t id);

/**
 * @brief release HW spinlock and exit critical
 *
 * @param critical  critical flag
 * @param id        HW spinlock id
 */
void halHwspinlockRelease(uint32_t critical, uint32_t id);

/**
 * @brief try acquire HW spinlock inside critical section
 *
 * @param id        HW spinlock id
 * @return
 *      - true if HW spinlock is acquired
 *      - false if HW spinlock is busy
 */
bool halHwspinlockTryAcquireInCritical(uint32_t id);

/**
 * @brief acquire HW spinlock inside critical section
 *
 * @param id        HW spinlock id
 */
void halHwspinlockAcquireInCritical(uint32_t id);

/**
 * @brief release HW spinlock inside critical section
 *
 * @param id        HW spinlock id
 */
void halHwspinlockReleaseInCritical(uint32_t id);

#ifdef __cplusplus
}
#endif
#endif
