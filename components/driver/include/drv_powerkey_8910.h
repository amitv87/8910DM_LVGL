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

#ifndef _DRV_POWERKEY_8910_H_
#define _DRV_POWERKEY_8910_H_

#include <stdint.h>

typedef enum
{
    PWR_STATE_PRESS = (1 << 0),
    PWR_STATE_RELEASE = (1 << 1),
} pwrState_t;

#define POWERKEY_KEYCODE (0)

/**
 * @brief power key init
 */
void drvPowerkeyInit();

/**
 * @brief function type, power key event handler
 */

typedef void (*powerkeyCb_t)(pwrState_t evt, void *p);

/**
 * @brief set powerkey event handler
 *
 * @param cb        the handler
 * @param evtmsk    the event mask caller care about
 * @param ctx       caller context
 */
//void drvPwrKeySetCB(powerkeyCb_t cb, uint32_t mask, void *ctx);

void drvPwrKeySetCB(powerkeyCb_t cb, uint32_t mask, void *ctx);

#endif
