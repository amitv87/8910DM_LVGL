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

#ifndef _SRV_DTR_H_
#define _SRV_DTR_H_

#include "srv_config.h"
#include "osi_api.h"

/**
 * DTR is simulated by a GPIO input. Typical usages:
 * - AT&D1: At ON->OFF, escape from data mode.
 * - AT&D2: At ON->OFF, disconnect from data mode to command mode.
 *          And when DTR is OFF, auto-answer is off.
 * - AT+CSCLK=1: At HIGH, sleep is enabled. At low, should quit
 *          sleep mode.
 *
 * DTR service is a simple wrapper of GPIO. Two calbacks will be registered,
 * which will be called on DTR level is changed.
 *
 * If configured, DTR service will provide software debounce, besides
 * hardware debounce. For example, after a raising edge of DTR, and there
 * are no falling edge of DTR in 20ms, the rasing edge event will be sent
 * out.
 *
 * Due to DTR level is changing, it is possible DTR level is changed between
 * event send and event handling (for example, after a raising event is sent,
 * and just before event is handled, a falling event is triggered). So, user
 * should avoid to call API to get DTR level in most cases.
 */

/**
 * \brief DTR callback function type
 *
 * This callback will be triggered in DTR service thread (usually, wq_hi will
 * be reused), and the callback will be executed in specified thread.
 *
 * \param param     the registered callback parameter
 */
typedef void (*srvDtrCallback_t)(void *param);

/**
 * \brief start DTR service
 *
 * Both of \p rising_cb and \p falling_cb can be NULL. However, when one of
 * them is not NULL, \p thread can't be NULL.
 *
 * When DTR service is started, it will return false at calling \p srvDtrStart
 * again, even the parameters are the same.
 *
 * \param thread        the thread to execute callbacks
 * \param rising_cb     callback to be called at raising edge
 * \param falling_cb    callback to be called at falling edge
 * \param cb_ctx        parameter of callbacks
 * \return
 *      - true on success
 *      - false on invalid parameter
 */
bool srvDtrStart(osiThread_t *thread, srvDtrCallback_t rising_cb,
                 srvDtrCallback_t falling_cb, void *cb_ctx);

/**
 * \brief stop DTR service
 */
void srvDtrStop(void);

/**
 * \brief whether DTR level is high
 *
 * In most cases, it shouldn't be called. When DTR service is not started,
 * it will return false.
 *
 * \return
 *      - true if DTR level is high
 *      - false if DTR level is low, or not started
 */
bool srvDtrIsHigh(void);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO
void prvDtrRising(void *param);
void prvDtrFalling(void *param);
#endif

#endif
