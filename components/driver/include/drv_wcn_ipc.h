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

#ifndef _DRV_WCN_IPC_H_
#define _DRV_WCN_IPC_H_

#include "osi_compiler.h"
#include "drv_uart.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief opaque data struct of wcn ipc uart
 */
typedef struct drvWcnIpcUart drvWcnIpcUart_t;

/**
 * \brief initialize wcn ipc
 *
 * The main purpose of wcn ipc is to provide one instance of
 * \p drvWcnIpcUart_t.
 */
void drvWcnIpcInit(void);

/**
 * \brief reset wcn ipc
 *
 * This should be called at reload wcn firmware and reset wcn.
 */
void drvWcnIpcReset(void);

/**
 * \brief open wcn ipc uart
 *
 * There is buffer for rx (from wcn to ap). It can reduce the latency to
 * fetch rx data from shared memory.
 *
 * At open, the rx data in shared memory will be kept.
 *
 * \param cb event callback
 * \param rx_size rx fifo size
 * \param cb_ctx event callback context
 * \param event_mask event mask
 * \return
 *      - wcn ipc uart instance
 *      - NULL on error, out of memory
 */
drvWcnIpcUart_t *drvWcnIpcUartOpen(unsigned rx_size, drvUartEventCB_t cb,
                                   void *cb_ctx, unsigned event_mask);

/**
 * \brief close wcn ipc usrt
 *
 * After close, the instance will be freed. So, don't access the instance
 * after close.
 *
 * \param uart wcn ipc uart instance
 */
void drvWcnIpcUartClose(drvWcnIpcUart_t *uart);

/**
 * \brief read data from uart channel
 *
 * When the read size may be less than \p size.
 *
 * \param uart wcn ipc uart instance
 * \param data memory for read data
 * \param size memory size
 * \return
 *      - real read size
 *      = -1 on error
 */
int drvWcnIpcUartRead(drvWcnIpcUart_t *uart, void *data, unsigned size);

/**
 * \brief write data to uart channel
 *
 * The the written size may be less than \p size.
 *
 * \param uart wcn ipc uart instance
 * \param data data to be written
 * \param size data size to be written
 * \return
 *      - real written size
 *      - -1 on error
 */
int drvWcnIpcUartWrite(drvWcnIpcUart_t *uart, const void *data, unsigned size);

/**
 * \brief write data to uart channel with waiting
 *
 * \param uart wcn ipc uart instance
 * \param data data to be written
 * \param size data size to be written
 * \param timeout timeout in milliseconds
 * \return
 *      - real written size
 *      - -1 on error
 */
int drvWcnIpcUartWriteAll(drvWcnIpcUart_t *uart, const void *data, unsigned size, unsigned timeout);

/**
 * \brief uart channel read available byte count
 *
 * \param uart wcn ipc uart instance
 * \return
 *      - read available byte count
 *      - -1 on error
 */
int drvWcnIpcUartReadAvail(drvWcnIpcUart_t *uart);

/**
 * \brief uart channel write space byte count
 *
 * \param uart wcn ipc uart instance
 * \return
 *      - write space byte count
 *      - -1 on error
 */
int drvWcnIpcUartWriteAvail(drvWcnIpcUart_t *uart);

OSI_EXTERN_C_END
#endif
