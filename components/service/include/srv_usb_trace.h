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

#ifndef _SRV_USB_TRACE_H_
#define _SRV_USB_TRACE_H_

#include "srv_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * \brief initialize USB trace module
 *
 * Besides trace output, USB trace module can also handle host commands
 * from PC.
 *
 * \return
 *      - true if USB trace module initialization success
 *      - fail on failed
 */
bool srvUsbTraceInit(void);

/**
 * \brief send trace data
 *
 * This will be called by trace module when the trace device is switched
 * to USB. When this returns, all of \p data are output, or output fails.
 * Usually, trace module will drop the trace data at return, no matter
 * success or fail.
 *
 * \param data      trace data
 * \param size      trace data size
 * \return
 *      - true if the trace data is output
 *      - false if output fails
 */
bool srvUsbTraceSendAll(const void *data, unsigned size);

/**
 * \brief USB trace module enter blue screen mode
 *
 * In blue screen mode, USB trace output is the same as normal. And host
 * command handling will work in poll mode.
 */
void srcUsbTraceBlueScreenEnter(void);

/**
 * \brief send trace data in blue screen mode
 *
 * \param data      trace data
 * \param size      trace data size
 * \return
 *      - true if the trace data is output
 *      - false if output fails
 */
bool srvUsbTraceBlueScreenSend(const void *data, unsigned size);

/**
 * \brief USB trace module polling in blue screen mode
 *
 * In blue screen mode, there are no interrupt and thread scheduling. USB
 * host command should work in polling mode.
 */
void srvUsbTraceBlueScreenPoll(void);

#endif
