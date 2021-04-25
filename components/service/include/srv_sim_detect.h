/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _SRV_SIM_DETECT_H_
#define _SRV_SIM_DETECT_H_

#include "osi_compiler.h"

#include "quec_proj_config.h"
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO
#include "drv_gpio.h"
#endif

OSI_EXTERN_C_BEGIN

#include <stdbool.h>

/**
 * @brief function type to report sim plug in/out
 */
typedef void (*simDetectCB_t)(int id, bool connect);

/**
 * @brief sim detect register callback
 *
 * @param id        sim id (must be 1/2)
 * @param gpio_id   gpio to detect sim hot plug
 * @param cb        callback after sim plug in/out detect
 * @return
 *      - true      success
 *      - false     fail
 */
bool srvSimDetectRegister(int id, int gpio_id, simDetectCB_t cb);

/**
 * @brief enable/disable sim detect
 *
 * @param id            sim id (must be 1/2)
 * @param int_status    enable or disable
 * @return
 *      - true on success else fail
 */
bool srvSimDetectSwitch(int id, bool int_status);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO
typedef struct
{
    int sim_id;
    simDetectCB_t cb;
    bool connected;
    bool connect_debounce;
    osiTimer_t *debounce_timer;
    drvGpio_t *gpio;
} simDetect_t;
#endif

OSI_EXTERN_C_END

#endif