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

#ifndef _SRV_2LINE_WAKEUP_H_
#define _SRV_2LINE_WAKEUP_H_

#include "srv_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SRV_2LINE_WAKEUP_WAKEUP_IN,
    SRV_2LINE_WAKEUP_AT_CMD,
} mcuNotifySleepMode_t;

/**
 * @brief Initialize 2line wakeup service
 *
 * @param mode  0 : notify by the gpio of wakeup_in,
 *              1 : notify by the at command and the gpio of wakeup_in
 * @param delay_ms the module should into deep sleep after the delay time.
 *
 */
void srv2LineWakeUpInit(int mode, uint32_t delay_ms);

/**
 * @brief Delete the resource when no use the service.
 *
 */
void srv2LineWakeUpDelete(void);

/**
 * @brief Check the gpio of wakeup_out if it is low level pull it up
 *          and hold the pm_wakeup_out lock.
 *
 */
void srv2LineWakeUpCheckWakeUpOutState(void);

/**
 * @brief Set the wakeup_out sleep timer, if the tiemer out, it will
 *        release the pm_wakeup_out lock.
 *
 */
void srv2LineWakeUpOutSetSleepTimer(void);

/**
 * @brief Mcu notify module should into sleep
 *
 * @param mode  0 : notify by the gpio of wakeup_in,
 *              1 : notify by the at command and the gpio of wakeup_in
 * @param delay_ms the module should into deep sleep after the delay time.
 * @return
 */
void srv2LineWakeUpMcuNotifySleep(int mode, uint32_t delay_ms);

#ifdef __cplusplus
}
#endif
#endif
