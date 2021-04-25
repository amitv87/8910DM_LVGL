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

#ifndef _MAL_INTERNAL_H_
#define _MAL_INTERNAL_H_

#include "osi_api.h"
#include "mal_config.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief macro to terminate transaction and return error code
 */
#define MAL_TRANS_RETURN_ERR(trans, err) OSI_DO_WHILE0(malTerminateTrans(trans); return (err);)

/**
 * \brief opaque data structure for transaction
 */
typedef struct malTransaction malTransaction_t;

/**
 * \brief transaction event callback function prototype
 *
 * The callback will be invoked in separated thread.
 */
typedef void (*malTransCallback_t)(malTransaction_t *trans, const osiEvent_t *event);

/**
 * \brief transaction context delete function prototype
 *
 * When a pointer is registered as transaction context, it will be deleted
 * when the transaction is terminated. In \p malTerminateTrans, the
 * registered context delete function will be called. It is helpful to
 * avoid tedious context delete in various return points, and avoid memory
 * leak.
 *
 * When the context is not dynamic allocated, the registered context delete
 * function should be NULL.
 *
 * When the context is just a dynamic allocated memory, \p malContextFree
 * can be used as context delete function. Don't use \p free as context
 * delete function, due to it is possible that \p free is a macro, which
 * will be expanded.
 *
 * When complex handling is needed in context delete, and \p free is not
 * enough to release all resources, self implemented delete function should
 * be registered.
 */
typedef void (*malContextDelete_t)(void *ctx);

/**
 * \brief create a transaction
 *
 * \return
 *      - created transaction
 *      - NULL if out of memory
 */
malTransaction_t *malCreateTrans(void);

/**
 * \brief set transaction context
 *
 * Transaction context is a concept that:
 * - mechnism to access the context inside transaction callback.
 * - automatically deleted when the transaction is terminated.
 *
 * \param trans     the transaction
 * \param ctx       context pointer
 * \param destroy   context delete function, NULL for not to call
 *                  delete function
 */
void malSetTransContext(malTransaction_t *trans, void *ctx, malContextDelete_t destroy);

/**
 * \brief helper for free memory
 *
 * Most likely, it will just free the memory by calling \p free.
 *
 * \param ctx       the context pointer to be freed.
 */
void malContextFree(void *ctx);

/**
 * \brief start transaction waiting UTI event
 *
 * This is a typical CFW transaction. After CFW API is called, an asynchronous
 * event with the same UTI will be responsed.
 *
 * UTI to be used can be get by \p malTransUti.
 *
 * \param trans     the transaction
 * \param cb        event callback for asynchronous event
 */
void malStartUtiTrans(malTransaction_t *trans, malTransCallback_t cb);

/**
 * \brief transaction finished
 *
 * This will be called inside callback to indicate the transaction is
 * finished. Only after it is called, \p malTransWait can return.
 *
 * \param trans     the transaction
 */
void malTransFinished(malTransaction_t *trans);

/**
 * \brief wait tranaction finish
 *
 * \param trans     the transaction
 * \param timeout   waiting timeout
 * \return
 *      - true if the transaction is finished
 *      - false if waiting timeout
 */
bool malTransWait(malTransaction_t *trans, unsigned timeout);

/**
 * \brief terminate the transaction
 *
 * This should be called when the transaction will be terminated.
 * \p trans will be destroyed inside, and it shouldn't be access
 * any more.
 *
 * The registered context will be deleted inside also.
 *
 * \param trans     the transaction
 */
void malTerminateTrans(malTransaction_t *trans);

/**
 * \brief abort transaction
 *
 * This should be called when the transaction is finished abnormally.
 * It just performs necessary cleanup, and \p trans won't be deleted.
 * That is, \p trans can be access still.
 *
 * \param trans     the transaction
 */
void malAbortTrans(malTransaction_t *trans);

/**
 * \brief UTI of the transaction
 *
 * The transaction must be started with \p malStartUtiTrans. For non-UTI
 * transaction, the return value is undefined.
 *
 * \param trans     the transaction
 * \return
 *      - UTI of the transaction
 *      - undefined for non-UTI transaction
 */
uint16_t malTransUti(malTransaction_t *trans);

/**
 * \brief get the registered transaction context
 *
 * The context is registered by \p malSetTransContext
 *
 * \param trans     the transaction
 * \return
 *      - the registered transaction context
 */
void *malTransContext(malTransaction_t *trans);

OSI_EXTERN_C_END
#endif
