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

#ifndef _OSI_FIFO_H_
#define _OSI_FIFO_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief OSI FIFO data structure
 *
 * Don't access the field members directly. Rather FIFO APIs should be used.
 */
typedef struct osiFifo
{
    void *data;  ///< FIFO buffer
    size_t size; ///< FIFO buffer size
    size_t rd;   ///< FIFO read pointer
    size_t wr;   ///< FIFO write pointer
} osiFifo_t;

/**
 * \brief initialize FIFO
 *
 * \param fifo      the FIFO pointer
 * \param data      FIFO buffer
 * \param size      FIFO buffer size
 * \return
 *      - true on success
 *      - false on invalid parameter
 */
bool osiFifoInit(osiFifo_t *fifo, void *data, size_t size);

/**
 * \brief reset FIFO
 *
 * After reset, the internal state indicates there are no data in the
 * FIFO.
 *
 * \param fifo      the FIFO pointer
 */
void osiFifoReset(osiFifo_t *fifo);

/**
 * \brief put data into FIFO
 *
 * The returned actual put size may be less than \a size.
 *
 * \param fifo      the FIFO pointer
 * \param data      data to be put into FIFO
 * \param size      data size
 * \return      actually put size
 */
int osiFifoPut(osiFifo_t *fifo, const void *data, size_t size);

/**
 * \brief get data from FIFO
 *
 * The returned actual get size may be less than \a size.
 *
 * \param fifo      the FIFO pointer
 * \param data      data buffer for get
 * \param size      data buffer size
 * \return      actually get size
 */
int osiFifoGet(osiFifo_t *fifo, void *data, size_t size);

/**
 * \brief peek data from FIFO
 *
 * On peek, the read position of FIFO won't be updated.
 *
 * \param fifo      the FIFO pointer
 * \param data      data buffer for peek
 * \param size      data buffer size
 * \return      actually peek size
 */
int osiFifoPeek(osiFifo_t *fifo, void *data, size_t size);

/**
 * \brief update read position to skip bytes in FIFO
 *
 * When \a size is larger than byte count in FIFO, only available bytes will
 * be skipped.
 *
 * \param fifo      the FIFO pointer
 * \param size      byte count to be skipped
 */
void osiFifoSkipBytes(osiFifo_t *fifo, size_t size);

/**
 * \brief search a byte in FIFO
 *
 * At search, the non-matching bytes will be dropped from the FIFO.
 * When \a byte is found, it is configurable to keep the byte or
 * drop the byte.
 *
 * \param fifo      the FIFO pointer
 * \param byte      the byte to be searched
 * \param keep      true to keep the found byte, false to drop the found byte
 * \return
 *      - true if \a byte is found
 *      - false if \a byte is not found
 */
bool osiFifoSearch(osiFifo_t *fifo, uint8_t byte, bool keep);

/**
 * \brief byte count in the FIFO
 *
 * \param fifo      the FIFO pointer
 * \return      the byte count of the FIFO
 */
static inline size_t osiFifoBytes(osiFifo_t *fifo) { return fifo->wr - fifo->rd; }

/**
 * \brief available space in the FIFO
 *
 * \param fifo      the FIFO pointer
 * \return      the available space byte count of the FIFO
 */
static inline size_t osiFifoSpace(osiFifo_t *fifo) { return fifo->size - osiFifoBytes(fifo); }

/**
 * \brief chech whether the FIFO is full
 *
 * \param fifo      the FIFO pointer
 * \return
 *      - true if the FIFO is full
 *      - false if the FIFO is not full
 */
static inline bool osiFifoIsFull(osiFifo_t *fifo) { return osiFifoSpace(fifo) == 0; }

/**
 * \brief chech whether the FIFO is empty
 *
 * \param fifo      the FIFO pointer
 * \return
 *      - true if the FIFO is empty
 *      - false if the FIFO is not empty
 */
static inline bool osiFifoIsEmpty(osiFifo_t *fifo) { return osiFifoBytes(fifo) == 0; }

#ifdef __cplusplus
}
#endif
#endif
