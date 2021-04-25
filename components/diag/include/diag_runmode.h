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

#ifndef _DIAG_RUNMODE_H_
#define _DIAG_RUNMODE_H_

#include "drv_debug_port.h"
#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * @brief Diag run mode enumeration
 */
typedef enum
{
    DIAG_RM_NORMAL = 0,                    ///< normal mode
    DIAG_RM_CALIB = 0x01,                  ///< calibration mode
    DIAG_RM_BOOT = 0x02,                   ///< boot mode: reset and enter boot mode
    DIAG_RM_PIN_TEST = 0x03,               ///< production test mode
    DIAG_RM_IQC = 0x04,                    ///< IQC fixture test mode
    DIAG_RM_CALIB_POST = 0x05,             ///< calibration post mode
    DIAG_RM_CALIB_POST_NO_LCM_MODE = 0x06, ///< calibration post mode without lcm, app
    DIAG_RM_BBAT = 0x07,
    DIAG_RM_MAX_COUNT,
} diagRunMode_t;

/**
 * @breif check run mode
 *
 * @param port debug port
 * @return the run mode, refer to \p diagRunMode_t.
 */
diagRunMode_t diagRunModeCheck(drvDebugPort_t *port);

OSI_EXTERN_C_END
#endif
