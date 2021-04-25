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

#ifndef _SRV_WDT_H_
#define _SRV_WDT_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include "srv_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * \brief service start watchdog
 *
 * \param max_interval_ms   max interval before re-feed the watchdog
 * \param feed_interval_ms  normal interval to feed the watchdog
 * \return true on succeed else false
 */
bool srvWdtInit(uint32_t max_interval_ms, uint32_t feed_interval_ms);

/**
 * \brief start watchdog
 * \return true on succeed else false
 */
bool srvWdtStart();

/**
 * \brief stop watchdog
 */
void srvWdtStop();

/**
 * @brief Reset watch dog when entering deep sleep
 *
 * @param reset_ms   set  the next feed time of the watchdog
 */
void srvWdtEnterDeepSleep(int64_t reset_ms);

/**
 * @brief Reset watch dog when exit from deep sleep
 *
 */
void srvWdtExitDeepSleep(void);

OSI_EXTERN_C_END

#endif
