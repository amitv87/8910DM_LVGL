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

#ifndef _OSI_EVENT_HUB_H_
#define _OSI_EVENT_HUB_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include "kernel_config.h"
#include "osi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * opaque data structure for event dispatch
 */
typedef struct osiEventDispatch osiEventDispatch_t;

/**
 * opaque data structure for event hub
 */
typedef struct osiEventHub osiEventHub_t;

/**
 * function type of event handler
 */
typedef void (*osiEventHandler_t)(const osiEvent_t *event);

/**
 * function type of event callback
 *
 * Comparing to \p osiEventHandler_t, there is a callback
 * context pointer.
 */
typedef void (*osiEventCallback_t)(void *ctx, const osiEvent_t *event);

/**
 * @brief create an event hub
 *
 * To avoid dynamic memory complexity, static memory with specified
 * \p depth will be used.
 *
 * @param depth     maximum registry count
 * @return
 *      - event hub pointer
 *      - NULL if out of memory
 */
osiEventHub_t *osiEventHubCreate(size_t depth);

/**
 * @brief delete the event hub
 *
 * When \p p is NULL, nothing will be done.
 *
 * @param p         the event hub
 */
void osiEventHubDelete(osiEventHub_t *p);

/**
 * @brief register an event handler to event hub
 *
 * When the event ID is already registered, it will be replaced
 * silently.
 *
 * Though \p handler of NULL is not useful, it is permitted.
 *
 * @param p         the event hub, must be valid
 * @param id        registered event ID
 * @param handler   event handler
 * @return
 *      - true on success
 *      - false on too many registrations
 */
bool osiEventHubRegister(osiEventHub_t *p, uint32_t id, osiEventHandler_t handler);

/**
 * @brief batch register events handler to event hub
 *
 * The variadic parameters are pairs of (id, handler), and ended
 * with id of zero.
 *
 * @param p         the event hub, must be valid
 * @param id        registered event ID
 * @return
 *      - true on success
 *      - false on too many registrations
 */
bool osiEventHubBatchRegister(osiEventHub_t *p, uint32_t id, ...);

/**
 * @brief batch register events handler to event hub
 *
 * It is the same as \p osiEventHubBatchRegister.
 *
 * @param p         the event hub, must be valid
 * @param id        registered event ID
 * @param args      variadic parameters
 * @return
 *      - true on success
 *      - false on too many registrations
 */
bool osiEventHubVBatchRegister(osiEventHub_t *p, uint32_t id, va_list args);

/**
 * @brief dispatch an event
 *
 * Found the registered event handler, and invoke the handler.
 *
 * Handler of NULL is regarded as unregistered, and return false.
 *
 * @param p         the event hub, must be valid
 * @param event     event to be dispatched
 * @return
 *      - true if the registered handler is invoked
 *      - false if the event is not registered
 */
bool osiEventHubRun(osiEventHub_t *p, const osiEvent_t *event);

/**
 * @brief create an event hdispatch
 *
 * To avoid dynamic memory complexity, static memory with specified
 * \p depth will be used.
 *
 * @param depth     maximum registry count
 * @return
 *      - event dispatch pointer
 *      - NULL if out of memory
 */
osiEventDispatch_t *osiEventDispatchCreate(size_t depth);

/**
 * @brief delete the event dispatch
 *
 * When \p p is NULL, nothing will be done.
 *
 * @param p         the event dispatch
 */
void osiEventDispatchDelete(osiEventDispatch_t *p);

/**
 * @brief register an event handler to event hub
 *
 * When the event ID is already registered, it will return
 * false.
 *
 * @param p         the event dispatch, must be valid
 * @param id        registered event ID
 * @param cb        event callback, can't be NULL
 * @param cb_ctx    event callback context
 * @return
 *      - true on success
 *      - false on fail
 *          - too many registrations
 *          - event already registered
 *          - invalid event callback
 */
bool osiEventDispatchRegister(osiEventDispatch_t *p, uint32_t id, osiEventCallback_t cb, void *cb_ctx);

/**
 * @brief dispatch an event
 *
 * Found the registered event callback, and invoke the callback.
 *
 * @param p         the event dispatch, must be valid
 * @param event     event to be dispatched
 * @return
 *      - true if the registered callback is invoked
 *      - false if the event is not registered
 */
bool osiEventDispatchRun(osiEventDispatch_t *p, const osiEvent_t *event);

#ifdef __cplusplus
}
#endif
#endif
