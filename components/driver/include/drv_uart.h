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

#ifndef _DRV_UART_H_
#define _DRV_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "drv_names.h"
#include "drv_config.h"

#ifdef __cplusplus
extern "C" {
#endif

struct drvUart;

/**
 * @brief UART struct
 *
 * Only a declare here, caller have no necessary to know
 * how it is implemented. And it is a anchor to identify
 * which uart device is to be handled.
 */
typedef struct drvUart drvUart_t;

/**
 * @brief UART data event
 */
enum
{
    DRV_UART_EVENT_RX_ARRIVED = (1 << 0),  ///< Received new data
    DRV_UART_EVENT_RX_OVERFLOW = (1 << 1), ///< Rx fifo overflowed
    DRV_UART_EVENT_TX_COMPLETE = (1 << 2)  ///< All data had been sent
};

/**
 * @brief UART data bits
 */
typedef enum
{
    DRV_UART_DATA_BITS_7 = 7,
    DRV_UART_DATA_BITS_8 = 8
} drvUartDataBits_t;

/**
 * @brief UART stop bits
 */
typedef enum
{
    DRV_UART_STOP_BITS_1 = 1,
    DRV_UART_STOP_BITS_2 = 2
} drvUartStopBits_t;

/**
 * @brief UART parity check mode
 */
typedef enum
{
    DRV_UART_NO_PARITY,   ///< No parity check
    DRV_UART_ODD_PARITY,  ///< Parity check is odd
    DRV_UART_EVEN_PARITY, ///< Parity check is even
//8910 ARM UART hardware not support Mark Space
#ifndef CONFIG_SOC_8910
    DRV_UART_SPACE_PARITY, ///< Parity check is always 0 (space)
    DRV_UART_MARK_PARITY   ///< Parity check is always 1 (mark)
#endif
} drvUartParity_t;

/**
 * @brief function type to notify data event
 */
typedef void (*drvUartEventCB_t)(void *param, uint32_t evt);

/**
 * @brief UART config
 */
typedef struct drvUartCfg
{
    uint32_t baud;               ///< baudrate, 0 for auto baud
    drvUartDataBits_t data_bits; ///< data bits
    drvUartStopBits_t stop_bits; ///< stop bits
    drvUartParity_t parity;      ///< parity check mode
    bool auto_baud_lc;           ///< "at" ot "AT" for auto baud. 8910 can detect both "at" and "AT". So, it will be ignored in 8910.
    bool cts_enable;             ///< enable cts or not
    bool rts_enable;             ///< enable rts or not
    size_t rx_buf_size;          ///< rx buffer size
    size_t tx_buf_size;          ///< tx buffer size
    uint32_t event_mask;         ///< event mask, identify which event will be notified
    drvUartEventCB_t event_cb;   ///< event notify callback function
    void *event_cb_ctx;          ///< app private param (like a context)
} drvUartCfg_t;

/**
 * @brief create an UART driver
 *
 * Create an UART driver, caller should provide an valid UART FOURCC device name
 * and an valid UART config.
 *
 * @param name  FOURCC UART name (DRV_NAME_UART{1, 2, 3})
 * @param cfg   UART config
 * @return
 *      - NULL      Fail to create an UART driver
 *      - non-null  Create an UART driver
 */
drvUart_t *drvUartCreate(uint32_t name, drvUartCfg_t *cfg);

/**
 * @brief open UART driver
 *
 * Open the UART driver, done it can send/receive data via the UART.
 *
 * @param uart  the UART driver
 * @return
 *      - true  success
 *      - false fail
 */
bool drvUartOpen(drvUart_t *uart);

/**
 * @brief close UART driver
 *
 * @note All data in tx/rx buffer will be purged.
 *
 * Close the UART driver, stop all data transfer.
 * But do not release resource.
 *
 * @param uart  the UART driver
 */
void drvUartClose(drvUart_t *uart);

/**
 * @brief destroy the UART driver
 *
 * @param uart  the UART driver
 */
void drvUartDestroy(drvUart_t *uart);

/**
 * @brief send data via the UART
 *
 * @note data may not send to hardware right away
 *
 * Send data via UART, the data may not send to hardware directly
 * because the UART hw may be busy, and the data not sent to hw will
 * be cached to a software tx buffer(see, drvUartCfg_t->tx_buf_size).
 *
 * Therefore the return value include the data had been sent and the
 * data had been cached.
 *
 * @param uart  the UART driver
 * @param data  data buffer to be sent
 * @param size  data buffer size
 * @return
 *      - (-1)  Parameter error
 *      - OTHERS (>=0)  The number of bytes actually sent (or cached)
 */
int drvUartSend(drvUart_t *uart, const void *data, size_t size);

/**
 * @brief send data via the UART without cache in n milliseconds
 *
 * @note data will not cached to tx fifo
 *
 * @param uart          the UART driver
 * @param data          data buffer to be sent
 * @param size          data buffer size
 * @param timeout_ms    timeout milliseconds
 * @return
 *      - (-1) Parameter error
 *      - OTHERS (>=0)  The number of bytes actually sent
 */
int drvUartSendAll(drvUart_t *uart, const void *data, size_t size, uint32_t timeout_ms);

/**
 * @brief receive data from the UART
 *
 * Receive data from the UART driver. Actually caller got data
 * from a software rx fifo but not hw fifo directly.
 *
 * After drvUartOpen the driver will store data from UART hw rx fifo
 * automatically and trigger a DRV_UART_EVENT_RX_ARRIVED, app who use
 * this driver can call the api after receive the event.
 *
 * @param uart  the UART driver
 * @param buf   buffer to store data
 * @param size  buffer size
 * @return
 *      - (-1) Parameter error
 *      - OTHERS (>=0)  The number of bytes actually receive from UART
 */
int drvUartReceive(drvUart_t *uart, void *buf, size_t size);

/**
 * @brief inquire avalable bytes in rx buffer
 *
 * @param uart  the UART driver
 * @return
 *      - (-1) Parameter error
 *      - OTHERS (>=0) Available size in byte
 */
int drvUartReadAvail(drvUart_t *uart);

/**
 * @brief inquire avalable space in tx buffer
 *
 * @param uart  the UART driver
 * @return
 *      - (-1) Parameter error
 *      - OTHERS (>=0) Available size in byte
 */
int drvUartWriteAvail(drvUart_t *uart);

/**
 * @brief set whether UART will auto sleep
 *
 * When enabled, UART will sleep after all bytes are transfered , with
 * specified timeout.
 *
 * To disable auto sleep feature, set \p timeout to -1 (or other negative
 * values).
 *
 * It maybe different among chips, or even different UARTs of one chip
 * whether system will be waken up when there are UART input.
 *
 * @param uart      the UART driver
 * @param timeout   auto sleep wait time after transfer done. It can be
 *                  0 but not recommended. Negative value to disable
 *                  auto sleep feature.
 */
void drvUartSetAutoSleep(drvUart_t *uart, int timeout);

/**
 * @brief get uart configuration, will never return null
 *
 * @param uart  the UART driver
 * @return
 *      - NULL      fail, only when UART is NULL
 *      - otherwise the UART configuration struct point.
 */
const drvUartCfg_t *drvUartConfig(drvUart_t *uart);

/**
 * @brief reconfig the uart
 *
 * @note caller should make sure the UART had already closed,
 *       or it will return false
 *       if `cfg` is NULL, it will do nothing and return true
 *
 * @param uart  the UART driver
 * @param cfg   the configuration
 * @return
 *      - true  sucess
 *      - false fail
 */
bool drvUartReconfig(drvUart_t *uart, drvUartCfg_t *cfg);

/**
 * @brief wait rx available, with optional timeout
 *
 * @param uart      the UART driver
 * @param timeout   wait timeout
 * @return
 *      - true on success
 *      - false on fail, invalid prameter or timeout
 */
bool drvUartWaitRxAvail(drvUart_t *uart, unsigned timeout);

/**
 * @brief wait tx available, with optional timeout
 *
 * @param uart      the UART driver
 * @param timeout   wait timeout
 * @return
 *      - true on success
 *      - false on fail, invalid prameter or timeout
 */
bool drvUartWaitTxAvail(drvUart_t *uart, unsigned timeout);

/**
 * @brief wait tx data send finished
 *
 * @param uart      the UART driver
 * @param timeout   wait time in millisecond
 * @return
 *      - true      data all sent
 *      - false     data residual
 */
bool drvUartWaitTxFinish(drvUart_t *uart, uint32_t timeout);

/**
 * @brief bind UART to stdio
 *
 * @param uart      the UART driver
 * @return
 *      - true      on success
 *      - false     on fail
 */
bool drvUartBindStdio(drvUart_t *uart);

#ifdef __cplusplus
}
#endif
#endif
