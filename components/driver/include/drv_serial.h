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

#ifndef _DRV_SERIAL_H_
#define _DRV_SERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <sys/queue.h>
#include <osi_compiler.h>

typedef struct drv_serial drvSerial_t;

typedef enum
{
    DRV_SERIAL_FLAG_TXBUF_UNCACHED = (1 << 0), ///< TX buffer is uncached
} drvSerialFlag_t;

typedef enum
{
    DRV_SERIAL_EVENT_RX_ARRIVED = (1 << 0),  ///< received new data
    DRV_SERIAL_EVENT_RX_OVERFLOW = (1 << 1), ///< rx overflow
    DRV_SERIAL_EVENT_TX_COMPLETE = (1 << 2), ///< all data had been sent
    DRV_SERIAL_EVENT_BROKEN = (1 << 3),      ///< serial broken
    DRV_SERIAL_EVENT_READY = (1 << 4),       ///< serial ready
    DRV_SERIAL_EVENT_OPEN = (1 << 5),        ///< open by host
    DRV_SERIAL_EVENT_CLOSE = (1 << 6),       ///< close by host
} drvSerialEvent_t;

/**
 * @brief function type seraial event callback
 */
typedef void (*drvSerialEventCB_t)(drvSerialEvent_t event, unsigned long p);

/**
 * @brief type to convey serial config
 */
typedef struct drv_serial_config
{
    uint32_t tx_buf_size;        ///< tx buffer size
    uint32_t rx_buf_size;        ///< rx buffer size
    uint32_t tx_dma_size;        ///< tx dma buffer size, 0 for autoset
    uint32_t rx_dma_size;        ///< rx dma buffer size, 0 for autoset
    drvSerialEvent_t event_mask; ///< event mask
    unsigned long param;         ///< caller context
    drvSerialEventCB_t event_cb; ///< event callback, it won't be called in ISR
} drvSerialCfg_t;

/**
 * @brief check the serial is ready or not from serial name
 * @param name      the serial name
 * @return
 *      - true      ready
 *      - false     not ready
 */
bool drvSerialCheckReadyByName(uint32_t name);

/**
 * @brief acquire and hold a serial by port name
 *
 * @param name  the port name
 * @param cfg   the serial config
 * @return
 *      - serial pointer
 *      - NULL on failed
 *          - no serial matches the name
 *          - serial is in use, even by myself
 */
drvSerial_t *drvSerialAcquire(uint32_t name, const drvSerialCfg_t *cfg);

/**
 * @brief check the serial is ready or not
 * @param serial    the serial
 * @return
 *      - true      ready
 *      - false     not ready
 */
bool drvSerialCheckReady(drvSerial_t *serial);

/**
 * @brief release the serial
 *
 * @param serial    the serial
 */
void drvSerialRelease(drvSerial_t *serial);

/**
 * @brief open a serial
 *
 * @param serial    the serial
 * @param
 *      - (-1)  fail
 *      - 0     success
 */
bool drvSerialOpen(drvSerial_t *serial);

/**
 * @brief close a serial
 *
 * @param serial    the serial
 */
void drvSerialClose(drvSerial_t *serial);

/**
 * @brief write to the serial
 *
 * @param serial    the serial
 * @param data      data address
 * @param size      data buffer size
 * @return
 *      - (-1)       fail
 *      - (positive) the bytes of data actually writen
 */
int drvSerialWrite(drvSerial_t *serial, const void *data, size_t size);

/**
 * @brief write to serial directly (do not cache data to software fifo)
 *
 * @param serial    the serial
 * @param data      the data buffer, max be 4 bytes aligned
 * @param size      data buffer size
 * @param to_ms     timeout in ms
 * @param flags     write special flags
 * @return          true if success else false
 */
bool drvSerialWriteDirect(drvSerial_t *serial, const void *data, size_t size, size_t to_ms, uint32_t flags);

/**
 * @brief wait read bytes available
 *
 * @param serial    the serial
 * @param timeout   wait timeout in milliseconds
 * @return
 *      - true on success
 *      - false on timeout
 */
bool drvSerialWaitReadAvail(drvSerial_t *serial, unsigned timeout);

/**
 * @brief get how many data avail to be read
 *
 * @param serial    the serial
 * @return
 *      - (-1)  fail
 *      - other avail size in bytes
 */
int drvSerialReadAvail(drvSerial_t *serial);

/**
 * @brief get how many data can be writen
 *
 * @param serial    the serial
 * @return
 *      - (-1)  fail
 *      - other avail size in bytes
 */
int drvSerialWriteAvail(drvSerial_t *serial);

/**
 * @brief wait write space available
 *
 * @param serial    the serial
 * @param timeout   wait timeout in milliseconds
 * @return
 *      - true on success
 *      - false on timeout
 */
bool drvSerialWaitWriteAvail(drvSerial_t *serial, unsigned timeout);

/**
 * @brief wait write finish
 * @param serial    the serial
 * @param timeout   wait timeout in milliseconds
 * @return
 *      - ture      success
 *      - false     timeout
 */
bool drvSerialWaitWriteFinish(drvSerial_t *serial, unsigned timeout);

/**
 * @brief read data from the serial
 *
 * @param serial    the serial
 * @param buffer    the buffer to read data
 * @param size      the size of the buffer
 * @return
 *      - (-1)       fail
 *      - (positive) the bytes of data actually read
 */
int drvSerialRead(drvSerial_t *serial, void *buffer, size_t size);

#ifdef __cplusplus
}
#endif
#endif // _DRV_SERIAL_H_
