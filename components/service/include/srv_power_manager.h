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

#ifndef _SRV_POWER_MANAGE_H_
#define _SRV_POWER_MANAGE_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include <stdbool.h>
#include <stdint.h>

/**
 * \brief function type for power on sequence
 */
typedef void (*srvPmPonFunc_t)(void *arg);

/**
 * \brief function type for event `SRVPM_EV_CHR_HOTPLUG`
 *
 * \param on    charger plugin(true) or plugout(false)
 * \param ctx   caller context
 */
typedef void (*srvPmChargerNotfiy_t)(bool on, void *ctx);

/**
 * \brief EVENTs defination
 */
typedef enum
{
    // use callback `srvPmChargerNotfiy_t`
    SRVPM_EV_CHR_HOTPLUG = 0,

    SRVPM_EV_COUNT,
} srvPmEv_t;

/**
 * \brief EVENTs notify token, create by `srvPmAddEvNotify` as an identifier
 */
typedef struct srv_pm_ev_token srvPmEvToken_t;

/**
 * \brief function type for event notify
 */
typedef void (*srvPmEventNotify_t)(srvPmEv_t event, uint32_t name, void *ctx);

/**
 * \brief initial power manager
 *
 * \param poweron_func  poweron function, will be called when pm allow poweron
 * \param arg           poweron function argument
 * \return
 *      - true on success else false
 */
bool srvPmInit(srvPmPonFunc_t poweron_func, void *arg);

/**
 * \brief run power manager
 */
void srvPmRun();

/**
 * \brief add event notify function to monitor the `ev` trigger
 *
 * \param ev    the event id
 * \param func  the callback function, caller should set proper function type according
 *              to `ev`. srvpm will do type conversion before calling the `func`
 * \param ctx   caller context
 * \return
 *      - NULL for fail
 *      - the identifier for this notifier
 */
srvPmEvToken_t *srvPmAddEvNotify(srvPmEv_t ev, void *func, void *ctx);

/**
 * \brief remove a ev notifier
 *
 * \param token     the identifier of the registered notifier
 */
void srvPmRemoveEvNotify(srvPmEvToken_t *token);

OSI_EXTERN_C_END

#endif