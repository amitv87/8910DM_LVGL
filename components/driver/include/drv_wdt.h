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

#ifndef _DRV_WDT_H_
#define _DRV_WDT_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief start the watchdog
 *
 * \param resetms   reset the chip if not feed the watchdog more than reset ms
 * \param feedms    feed watchdog every feed ms
 *                  if feedms == OSI_WAIT_FOREVER not feed the watchdog
 * \return
 *      - true on success else fail
 */
bool drvWatchdogStart(uint32_t resetms, uint32_t feedms);

/**
 * \brief stop the watchdog
 */
void drvWatchdogStop();

#ifdef __cplusplus
}
#endif

#endif
