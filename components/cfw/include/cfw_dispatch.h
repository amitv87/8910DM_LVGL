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

#ifndef _CFW_DISPATCH_H_
#define _CFW_DISPATCH_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "osi_api.h"
#include "osi_event_hub.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief indicate invalid UTI or UTI unavailable
 */
#define CFW_UTI_INVALID (0xffff)

/**
 * @brief CFW event dispatch initialization
 */
void cfwDispatchInit(void);

/**
 * @brief set CFW event dispatch main thread
 *
 * When the UTI of CFW event is not registered, the event
 * will be sent to the *main thread*.
 *
 * @param thread    main thread to be set
 */
void cfwDispatchSetMainThread(osiThread_t *thread);

/**
 * @brief request a UTI, response to current thread
 *
 * Request a UTI, and set the CFW event with the specified UTI will be send to
 * current thread, that is the caller thread.
 *
 * @param cb        callback to be invoked when UTI matches
 * @param cb_ctx    callback context
 * @return
 *      - requested UTI
 *      - CFW_UTI_INVALID if no free UTI
 */
uint16_t cfwRequestUTI(osiEventCallback_t cb, void *cb_ctx);

/**
 * @brief request a UTI with more options
 *
 * The thread for the arriving matched UTI can be specified.
 *
 * Normally, the UTI registration will be removed automatically when the first
 * event is handled in \a cfwInvokeUtiCallback. In case there are more than one
 * event with the UTI will arrive, \a auto_remove can be set to false. Then the
 * UTI registration won't be removed automatically. Rather, it should be
 * released manually.
 *
 * @param cb        callback to be invoked when UTI matches
 * @param cb_ctx    callback context
 * @param thread    the thread should receive response event, NULL for current thread
 * @param auto_remove   auto remove the UTI registration after callback is invoked
 * @return
 *      - requested UTI
 *      - CFW_UTI_INVALID if no free UTI
 */
uint16_t cfwRequestUTIEx(osiEventCallback_t cb, void *cb_ctx, osiThread_t *thread, bool auto_remove);

/**
 * @brief request a UTI, and not register response thread
 *
 * Only when the caller doesn't care about the response event,
 * it should be called. When the response arrived, the caller
 * thread may not be able to receive the response event.
 *
 * The requested UTI will be reused automatically. It is not
 * needed and not permitted to call \p cfwReleaseUTI.
 *
 * @return
 *      - requested UTI
 *      - CFW_UTI_INVALID if no free UTI
 */
uint16_t cfwRequestNoWaitUTI(void);

/**
 * @brief request a registered UTI with response to current thread
 *
 * When it is known that response event won't arrive, for example,
 * the call returns error, it *must* be called. Otherwise, the
 * UTI resource will be leaked.
 *
 * When there are registered (UTI, thread) combination can match,
 * it will return silently.
 *
 * @param uti   the UTI to be released
 */
void cfwReleaseUTI(uint16_t uti);

/**
 * @brief dispatch CFW event
 *
 * When CFW event is about to be sent, this *must* be called.
 * And it will ensure the event to be sent to proper thread.
 *
 * The event *MUST* be CFW event, that is, the third parameter follows
 * CFW event convention.
 *
 * It won't fail to send. When the UTI is registered, it will be
 * sent to the registered thread. Otherwise, it will be sent to
 * the main thread. And if main thread is not set, it will be
 * dropped.
 *
 * @param event     the event to be sent, and must be CFW event
 */
void cfwDispatchSendEvent(const osiEvent_t *event);

/**
 * @brief check whether the event ID is CFW event
 *
 * @param id        the event ID
 * @return
 *      - true if it is CFW event
 *      - false if it is not CFW event
 */
bool cfwIsCfwEvent(uint32_t id);

/**
 * @brief check whether the indicate ID is CFW indicate event
 *
 * @param id        the indicate ID
 * @return
 *      - true if it is CFW indicate event
 *      - false if it is not CFW indicate event
 */
bool cfwIsCfwIndicate(uint32_t id);

/**
 * @brief invoke registered callback
 *
 * When a registered callback is invoked, and the event id matches the registered
 * last UTI or the registration doesn't specify event id, it will be unregistered
 * automatically
 *
 * @param event     input event, must be \p CFW_EVENT
 * @return
 *      - true if the registered callback is invoked
 *      - false if \p event is not a CFW_EVENT, or the UTI is not registered
 */
bool cfwInvokeUtiCallback(const osiEvent_t *event);

#ifdef __cplusplus
}
#endif
#endif
