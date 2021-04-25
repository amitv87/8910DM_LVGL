/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _SRV_KEYPAD_H_
#define _SRV_KEYPAD_H_

#include "osi_compiler.h"
#include "drv_keypad.h"
#include "drv_powerkey_8910.h"
OSI_EXTERN_C_BEGIN

/**
 * \brief function type, override keypad event
 */
typedef void (*kpdHook_t)(keyMap_t id, keyState_t event, void *ctx);

/**
 * \brief listener to monitor the keypad
 */
typedef struct srv_kpd_listener srvKpdLsn_t;

/**
 * \brief listen a key, and notify the event with hook function
 *
 * \param id        the key id
 * \param hook_func the process function when key event triggered
 * \param ctx       the callse context
 * \return
 *      - listener instance or NULL
 */
srvKpdLsn_t *srvKpdListen(keyMap_t id, kpdHook_t hook_func, void *ctx);

/**
 * \brief stop listen a key
 *
 * \param lsn   the listener
 * \return
 *      - true on succeed else something wrong
 */
bool srvKpdListenBreak(srvKpdLsn_t *lsn);

/**
 * \brief initial keypad service, the system will be crash if init fail
 */
void srvKpdInit();

OSI_EXTERN_C_END

#endif