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

#ifndef _HAL_PSRAM_DDR_H_
#define _HAL_PSRAM_DDR_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_SOC_8910
#if defined(CONFIG_USE_DDR) && (CONFIG_DEFAULT_MEMBUS_FREQ == 200000000) && (CONFIG_RAM_SIZE == (32 << 20))
#include "8910/hal_ddr_32m_200mhz_cfg.h"
#elif defined(CONFIG_USE_DDR) && (CONFIG_DEFAULT_MEMBUS_FREQ == 400000000) && (CONFIG_RAM_SIZE == (32 << 20))
#include "8910/hal_ddr_32m_400mhz_cfg.h"
#elif defined(CONFIG_USE_DDR) && (CONFIG_DEFAULT_MEMBUS_FREQ == 200000000) && (CONFIG_RAM_SIZE == (64 << 20))
#include "8910/hal_ddr_64m_200mhz_cfg.h"
#elif defined(CONFIG_USE_PSRAM) && (CONFIG_DEFAULT_MEMBUS_FREQ == 200000000)
#include "8910/hal_psram_200mhz_cfg.h"
#else
#error "Invalid PSRAM/DDR configuration"
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif
