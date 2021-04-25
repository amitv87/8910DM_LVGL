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

#ifndef _SRV_HEADSET_H_
#define _SRV_HEADSET_H_

#include "osi_compiler.h"
#include "drv_headset.h"
OSI_EXTERN_C_BEGIN

/**
 * \brief function type, override headset event
 */
typedef void (*headsetHook_t)(drvHeadsetNotifyMsg_t id, uint32_t param, void *ctx);

/**
 * \brief listener to monitor the headset action
 */
typedef struct srv_headset_listener srvHeadsetLsn_t;

/**
 * \brief listen a headset action, and notify the event with hook function,
 * \srv headset call all reg bfunc by uplayer and switch audio patch,then call
 * \all reg afunc by uplayer
 *
 * \param id        the msg id
 * \param bfunc     the process function when headset plug triggered,before audio path switch
 * \param afunc     the process function when headset plug triggered,after audio path switch
 * \param ctx       the callse context
 * \return
 *      - listener instance or NULL
 */
srvHeadsetLsn_t *srvHeadsetListen(drvHeadsetNotifyMsg_t id, headsetHook_t bfunc, headsetHook_t afunc, void *ctx);

/**
 * \brief stop listen a headset action
 *
 * \param lsn   the listener
 * \return
 *      - true on succeed else something wrong
 */
bool srvHeadsetListenBreak(srvHeadsetLsn_t *lsn);

/**
 * \brief initial headset service, the system will be crash if init fail
 */
void srvHeadsetInit();

OSI_EXTERN_C_END

#endif
