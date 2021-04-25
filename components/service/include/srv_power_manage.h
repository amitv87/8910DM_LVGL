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
 * \brief PM_EVENTs defination
 */
typedef enum
{
    SRVPM_EVENT_CHARGER_HOTPLUG = 0,
    SRVPM_EVENT_COUNT,
} srvPmEvent_t;

/**
 * \brief function type for event notify
 */
typedef void (*srvPmEventNotify_t)(srvPmEvent_t event, uint32_t name, void *ctx);

/**
 * \brief function type for power on sequence
 */
typedef void (*srvPmPowerOn_t)(void *arg);

/**
 * \brief initial power manager
 *
 * \param poweron_func  poweron function, will be called when pm allow poweron
 * \param arg           poweron function argument
 * \return
 *      - true on success else false
 */
bool srvPmInit(srvPmPowerOn_t poweron_func, void *arg);

/**
 * \brief run power manager
 */
void srvPmRun();

/**
 * \brief Add event notify for specific caller
 *        Not exclusive API, allow multiple modules to monitor the same event together
 *
 * \param event     the event id
 * \param caller    the caller identifier(, Recommend FOURCC name)
 * \param cb        the notify callback(, must non-null)
 * \param ctx       caller context
 * \param
 *      - true      success
 *      - false     invalid event id; cb is null;
 *                  caller have already set callback for this event
 */
bool srvPmAddEventNotfiy(srvPmEvent_t event, uint32_t caller, srvPmEventNotify_t cb, void *ctx);

/**
 * \brief remove the registered event notify callback for a caller
 *
 * \param event     the event id
 * \param caller    the caller identifier
 */
void srvPmRemoveEventNotify(srvPmEvent_t event, uint32_t caller);

OSI_EXTERN_C_END

#endif