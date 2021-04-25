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

#ifndef _OSI_BLOCKED_FIFO_H_
#define _OSI_BLOCKED_FIFO_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief opaque blocked FIFO data struct
 */
typedef struct osiBlockedFifo osiBlockedFifo_t;

/**
 * \brief create a blocked FIFO
 *
 * \p alignment should be power of 2. \p data, \p size and \p block_size
 * should be aligned with \p alignment. \p size should be multiple of
 * \p block_size.
 *
 * \p block_count is the count for block management count. Usually it is
 * larger than \p size/block_size.
 *
 * \param data      memory for blocked FIFO
 * \param size      memory size
 * \param alignment block starting address alignment
 * \param block_size    maximum block size
 * \param block_count   block management count
 * \return
 *      - blocked FIFO
 *      - NULL if parameters are invalid or out of memory
 */
osiBlockedFifo_t *osiBlockedFifoCreate(void *data, unsigned size, unsigned alignment,
                                       unsigned block_size, unsigned block_count);

/**
 * \brief reset the blocked FIFO
 *
 * \param fifo      the blocked FIFO pointer
 */
void osiBlockedFifoReset(osiBlockedFifo_t *fifo);

/**
 * \brief delete the blocked FIFO
 *
 * When \p fifo is NULL, nothing will be done
 *
 * \param fifo      the blocked FIFO pointer
 */
void osiBlockedFifoDelete(osiBlockedFifo_t *fifo);

/**
 * \brief put data into blocked FIFO
 *
 * \p size can be larger than \p block_size. In this case, multiple blocks
 * will be used.
 *
 * The byte count put into blocked FIFO may be less than the specified
 * \p size.
 *
 * When there is pending external put, it will return 0.
 *
 * \param fifo      the blocked FIFO pointer
 * \param data      data to be put
 * \param size      data size to be put
 * \return
 *      - byte count put into the blocked FIFO.
 *      - -1 on invalid parameters
 */
int osiBlockedFifoPut(osiBlockedFifo_t *fifo, const void *data, unsigned size);

/**
 * \brief get data from blocked FIFO
 *
 * \p size can be larger than \p block_size. In this case, multiple blocks
 * will be used.
 *
 * The byte count get from blocked FIFO may be less than the specified
 * \p size.
 *
 * When there is pending external get, it will return 0.
 *
 * \param fifo      the blocked FIFO pointer
 * \param data      memory for output data
 * \param size      memory size
 * \return
 *      - byte count get from the blocked FIFO.
 *      - -1 on invalid parameters
 */
int osiBlockedFifoGet(osiBlockedFifo_t *fifo, void *data, unsigned size);

/**
 * \brief request a bufer for external put
 *
 * Request a buffer for external put. When returned buffer is not NULL,
 * the buffer size is \p block_size, and the buffer is linear.
 *
 * When there are no spaces and \p take_lst is false, it will return
 * NULL. When \p take_last is true, it will drop the latest put buffer.
 * With proper configuration (\p block_count is not less than 4), it never
 * returns NULL.
 *
 * Only one pending request is permitted. That is, after
 * \p osiBlockedFifoPutRequest is called, and before
 * \p osiBlockedFifoPutDone is called, it will return NULL.
 *
 * \param fifo      the blocked FIFO pointer
 * \param take_last drop and take the latest put buffer if set as true
 * \return
 *      - linear put buffer address
 *      - NULL if no spaces
 */
void *osiBlockedFifoPutRequest(osiBlockedFifo_t *fifo, bool take_last);

/**
 * \brief external put is done
 *
 * This should be called after external put is done.
 *
 * \param fifo      the blocked FIFO pointer
 * \param size      the real put size
 * \return
 *      - true on success
 *      - false on invalid parameter, or not requested
 */
bool osiBlockedFifoPutDone(osiBlockedFifo_t *fifo, unsigned size);

/**
 * \brief request buffer for external get
 *
 * When the return buffer is not NULL, \p size will not exceed \p block_size.
 *
 * Only one pending request is permitted. That is, after
 * \p osiBlockedFifoGutRequest is called, and before
 * \p osiBlockedFifoGutDone is called, it will return NULL.
 *
 * To ensure that the returned address is aligned:
 * - Not to call \p osiBlockedFifoGet
 * - At \p osiBlockedFifoGetDone, \p size is the same as output value of
 *   \p osiBlockedFifoGetRequest. That is, the buffer of
 *   \p osiBlockedFifoGetRequest should be consumed as a whole.
 *
 * \param fifo      the blocked FIFO pointer
 * \param size      requested buffer size
 * \return
 *      - requested buffer
 *      - NULL on blocked FIFO empty, or there is a pending get
 */
const void *osiBlockedFifoGetRequest(osiBlockedFifo_t *fifo, unsigned *size);

/**
 * \brief external get is done
 *
 * This should be called after external get is done. \p size is the real
 * consumed byte count.
 *
 * When there is no pending external get, it will return false.
 *
 * \param fifo      the blocked FIFO pointer
 * \param size      the real consumed size
 * \return
 *      - true on success
 *      - false on invalid parameter, or not requested
 */
bool osiBlockedFifoGetDone(osiBlockedFifo_t *fifo, unsigned size);

/**
 * \brief external get block is done
 *
 * This should be called after external get is done. Comparing to
 * \p osiBlockedFifoGetDone, \p size is not needed. And it means the
 * requested is consumed, and caller doesn't need to keep requested size.
 *
 * When there is no pending external get, it will return false.
 *
 * \param fifo      the blocked FIFO pointer
 * \return
 *      - true on success
 *      - false on invalid parameter, or not requested
 */
bool osiBlockedFifoGetBlockDone(osiBlockedFifo_t *fifo);

/**
 * \brief get block size
 *
 * \param fifo      the blocked FIFO pointer
 * \return      block size, constant property
 */
unsigned osiBlockedFifoBlockSize(osiBlockedFifo_t *fifo);

/**
 * \brief whether the blocked FIFO is empty
 *
 * No byte count will be counted for pending external put.
 *
 * \param fifo      the blocked FIFO pointer
 * \return
 *      - true if the blocked FIFO is empty
 */
bool osiBlockedFifoIsEmpty(osiBlockedFifo_t *fifo);

/**
 * \brief whether the blocked FIFO is full
 *
 * When there is pending external put, it will be considered as *not full*.
 *
 * There are 2 senerio of full:
 * - There are no memory spaces.
 * - Block count reaches \p block_count.
 *
 * \param fifo      the blocked FIFO pointer
 * \return
 *      - true if the blocked FIFO is full
 */
bool osiBlockedFifoIsFull(osiBlockedFifo_t *fifo);

/**
 * \brief byte count in blocked FIFO
 *
 * \param fifo      the blocked FIFO pointer
 * \return      approximate byte count
 */
unsigned osiBlockedFifoBytes(osiBlockedFifo_t *fifo);

/**
 * \brief space in blocked FIFO
 *
 * Due to there are holes in blocked FIFO, the space is inaccurate.
 *
 * \param fifo      the blocked FIFO pointer
 * \return      approximate space
 */
unsigned osiBlockedFifoSpace(osiBlockedFifo_t *fifo);

/**
 * \brief show blocked FIFO blocks in trace
 *
 * It is for debug only.
 *
 * \param fifo      the blocked FIFO pointer
 */
void osiBlockedFifoDumpInfo(osiBlockedFifo_t *fifo);

#ifdef __cplusplus
}
#endif
#endif
