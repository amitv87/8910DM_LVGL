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

#ifndef _DRV_MODEM_LOG_BUF_H_
#define _DRV_MODEM_LOG_BUF_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief dummy struct for modem log buf
 *
 * Usually, this is embedded in another data struct. The dummy struct is
 * just a placeholder. Don't access members inside.
 *
 * There should be only one owner for each modem log buf, and then thread
 * safe isn't considered in this module. If needed, caller should take
 * care of thread safe.
 */
typedef struct
{
    //! @cond Doxygen_Suppress
    void *dummy1;
    bool dummy2;
    //! @endcond
} drvModemLogBuf_t;

/**
 * \brief initialize cp log buffer data structure
 *
 * It will only search the shared memory, and won't check the content in
 * shared memory. It may be called before CP initialization.
 *
 * \param d modem log buf instance, must be valid
 * \return
 *      - true on success
 *      - false on fail, no shared memory for cp log
 */
bool drvModemLogBufInitCp(drvModemLogBuf_t *d);

/**
 * \brief initialize zsp log buffer data structure
 *
 * \param d modem log buf instance, must be valid
 * \return
 *      - true on success
 *      - false on fail, no shared memory for zsp log
 */
bool drvModemLogBufInitZsp(drvModemLogBuf_t *d);

/**
 * \brief initialize mos log buffer data structure
 *
 * \param d modem log buf instance, must be valid
 * \return
 *      - true on success
 *      - false on fail, no shared memory for mos log
 */
bool drvModemLogBufInitMos(drvModemLogBuf_t *d);

/**
 * \brief get linear buffer of modem log buf
 *
 * Typically, modem log buf is a cyclic buffer. \p is_tail will return
 * true when the linear buffer is at the tail, and log data has already
 * wrapped. In this case, the returned linear buffer won't be changed
 * in the following calls. In case there are size control (avoid to
 * process small data) in caller, this case should be handled specially.
 *
 * When modem log buf is invalid, the returned linear buffer size will
 * be 0, the same as buffer empty.
 *
 * The returned linear buffer pointer will be 4 bytes aligned, and size
 * is 4 bytes aligned.
 *
 * \param d modem log buf instance, must be valid
 * \param is_tail return true is the linear buffer is at the tail, and
 *              log data has been wrapped.
 * \return
 *      - true on success
 *      - false on fail, no shared memory for mos log
 */
osiBuffer_t drvModemLogLinearBuf(drvModemLogBuf_t *d, bool *is_tail);

/**
 * \brief update reader pointer of modem log buffer
 *
 * \p size should be 4 bytes aligned.
 *
 * \param d modem log buf instance, must be valid
 * \param size bytes to be advanced
 */
void drvModemLogBufAdvance(drvModemLogBuf_t *d, unsigned size);

OSI_EXTERN_C_END
#endif
