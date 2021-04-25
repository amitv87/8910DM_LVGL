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

#ifndef _RPC_DAEMON_H_
#define _RPC_DAEMON_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct rpcChannel;

/**
 * Opaque type for RPC channel
 */
typedef struct rpcChannel rpcChannel_t;

/**
 * RPC packet header
 */
typedef struct
{
    uint32_t opcode; ///< operation in RPC packet
    uint32_t size;   ///< the whole RPC packet size
} rpcHeader_t;

/**
 * RPC call packet header
 */
typedef struct
{
    rpcHeader_t h;           ///< common RPC packet header
    uint32_t api_tag;        ///< tag for RPC function
    uint32_t caller_sync;    ///< sync primitive of caller, usually is a semaphore
    uint32_t caller_rsp_ptr; ///< response pointer of caller
    uint16_t seq;            ///< sequence number, just for debug
    uint16_t rsp_size;       ///< response packet size
} rpcCallHeader_t;

/**
 * RPC response packet header
 */
typedef struct
{
    rpcHeader_t h;           ///< common RPC packet header
    uint32_t api_tag;        ///< tag for RPC function, copied from "call", just for debug
    uint32_t caller_sync;    ///< sync primitive of caller, copied from "call"
    uint32_t caller_rsp_ptr; ///< response pointer of caller
    uint16_t seq;            ///< sequence number, just for debug
    uint16_t rpc_error_code; ///< 0 or ENOENT(2)
} rpcRespHeader_t;

/**
 * RPC event packet header
 *
 * The 4 words after common RPC packet header is just osiEvent_t.
 * It is to reduce header file dependency t expand them.
 */
typedef struct
{
    rpcHeader_t h; ///< common RPC packet header
    uint32_t id;   ///< event id
    uint32_t par1; ///< 1st word parameter
    uint32_t par2; ///< 2nd word parameter
    uint32_t par3; ///< 3rd word parameter
} rpcEventHeader_t;

/**
 * Function type to route (send out) event
 */
typedef void (*rpcEventRouter_t)(void *ctx, const void *event);

/**
 * Function type of RPC wrapper for local implemented API
 */
typedef void (*rpcFunction_t)(void *in, void *out);

/**
 * Function type to pack event
 */
typedef void (*rpcEventSender_t)(rpcChannel_t *ch, const void *event);

/**
 * Function type to unpack event
 */
typedef bool (*rpcEventUnpacker_t)(rpcEventHeader_t *event);

/**
 * @brief open RPC channel
 *
 * Though only one RPC channel is designed now (that is the reason there
 * are no parameter at open), RPC channel pointer is used in all RPC APIs.
 *
 * RPC channel can be shared. So, it will return the same pointer at
 * further open.
 *
 * There are no "close" API. In current design, RPC channel won't be closed.
 *
 * @return the RPC channel pointer
 */
rpcChannel_t *rpcChannelOpen(void);

/**
 * @brief send an event to peer
 *
 * The event is generic event (4 words), and the first word MUST be event ID.
 * Sending method will depend on event ID, and event packer will be called
 * inside.
 *
 * @param   ch    the RPC channel
 * @param   event the event to be send
 */
void rpcSendEvent(rpcChannel_t *ch, const void *event);

/**
 * @brief register event router
 *
 * RPC daemon itself doesn't know how to route the event from peer.
 * Application should register router for specified event range.
 *
 * The router should only send the event to corresponding thread.
 * The router itself shouldn't handle the event.
 *
 * When range and callback are existed, no duplicated router will be added.
 * It is application's duty to decide whether overlapped ranges are permitted.
 *
 * @param   ch        the RPC channel
 * @param   start     event range start (inclusive)
 * @param   end       event range end (inclusive)
 * @param   router    event router
 * @param   router_ctx  context for router callback
 * @return
 *      - true  registration success
 *      - false registration failed, due to invalid parameters or
 *              there are too many routers.
 */
bool rpcRegisterEvents(rpcChannel_t *ch, uint32_t start, uint32_t end,
                       rpcEventRouter_t router, void *router_ctx);

/**
 * @brief send function call packet to peer
 *
 * RPC packets are 8 bytes aligned. When the size in input header is not
 * 8 bytes aligned, it will be changed to 8 bytes aligned. That is,
 * event->h.size may be changed inside.
 *
 * Most likely, it won't be called directly by application. It will be
 * called only in RPC daemon and RPC stubs.
 *
 * @param   ch    the RPC channel
 * @param   call  the constructed call header
 * @return
 *      - 0         success
 *      - others    error. Only when peer can't find the function, it will
 *                  return error (-ENOENT).
 */
int rpcSendCall(rpcChannel_t *ch, rpcCallHeader_t *call);

/**
 * @brief send a packed event to peer
 *
 * When all content needed to send to peer are packed after the header, it
 * is "packed event". event->h.size is the data size needed to send to peer.
 *
 * RPC packets are 8 bytes aligned. When the size in input header is not
 * 8 bytes aligned, it will be changed to 8 bytes aligned. That is,
 * event->h.size may be changed inside.
 *
 * Most likely, it won't be called directly by application. It will be
 * called only in RPC daemon and RPC stubs.
 *
 * @param   ch    the RPC channel
 * @param   event the packed event
 */
void rpcSendPackedEvent(rpcChannel_t *ch, rpcEventHeader_t *event);

/**
 * @brief send a plain event to peer
 *
 * "plain event" is event without pointer. The 4 words can be send to peer
 * directly. @a event should be osiEvent_t.
 *
 * Most likely, it won't be called directly by application. It will be
 * called only in RPC daemon and RPC stubs.
 *
 * @param ch    the RPC channel
 * @param event the event to be send
 */
void rpcSendPlainEvent(rpcChannel_t *ch, const void *event);

/**
 * @brief send an event with pointer to peer
 *
 * "pointer event" is event with pointer. The content of the pointer shall
 * be sent to peer rather than the pointer itself. Also, the pointer should
 * be freed after the event is sent to peer (not after peer handled the event).
 *
 * When par1 or par2 is not a pointer, or the pointer is NULL, the parameter
 * ptr1_size or ptr2_size must be zero.
 *
 * Though it is possible to get the allocated size of pointer, and event
 * the manual packer will use this method, it is not recommended programming
 * style here (can be regarded as a hack).
 *
 * Most likely, it won't be called directly by application. It will be
 * called only in RPC daemon and RPC stubs.
 *
 * @param ch        the RPC channel
 * @param event     the event to be send
 * @param ptr1_size the memory size when par1 is a pointer, otherwise 0
 * @param ptr2_size the memory size when par1 is a pointer, otherwise 0
 */
void rpcSendPointerEvent(rpcChannel_t *ch, const void *event,
                         uint32_t ptr1_size, uint32_t ptr2_size);

/**
 * @brief route the event from peer
 *
 * Search registered router, and call all matched routers.
 *
 * Most likely, it won't be called directly by application. It will be
 * called only in RPC daemon and RPC stubs (such as customized router).
 *
 * @param ch        the RPC channel
 * @param event     the unpacked event
 */
void rpcRouteEvent(rpcChannel_t *ch, rpcEventHeader_t *event);

/**
 * @brief unpack pointer event
 *
 * Unpack pointer event from peer. The event itself is "packed" event.
 * The pointer content will be copied to local malloc memory.
 *
 * Most likely, it won't be called directly by application. It will be
 * called only in RPC daemon and RPC stubs.
 *
 * @param event the event to be send
 * @param ptr1_size the memory size when par1 is a pointer, otherwise 0
 * @param ptr2_size the memory size when par1 is a pointer, otherwise 0
 * @return
 *      -true   success
 *      -false  unpack fail, may due to incorrect event header, or
 *              malloc failed.
 */
bool rpcUnpackPointerEvent(rpcEventHeader_t *event,
                           uint32_t ptr1_size, uint32_t ptr2_size);

#ifdef __cplusplus
}
#endif
#endif
