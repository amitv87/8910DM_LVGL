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

#ifndef _OSI_PIPE_H_
#define _OSI_PIPE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief opaque data structure for pipe
 */
typedef struct osiPipe osiPipe_t;

/**
 * \brief pipe event type
 */
typedef enum
{
    OSI_PIPE_EVENT_RX_ARRIVED = (1 << 0),  ///< received new data
    OSI_PIPE_EVENT_TX_COMPLETE = (1 << 2), ///< all data had been sent
} osiPipeEvent_t;

/**
 * \brief pipe callback function prototype
 *
 * Reader callback will be invoked in writer thread, and writer
 * callback will be invoked in reader thread.
 */
typedef void (*osiPipeEventCallback_t)(void *param, unsigned event);

/**
 * \brief create a pipe
 *
 * Pipe buffer will be dynamic allocated.
 *
 * \param size      pipe buffer size, can't be 0
 * \return
 *      - the created pipe
 *      - NULL if parameter is invalid, or out of memory
 */
osiPipe_t *osiPipeCreate(unsigned size);

/**
 * \brief delete a pipe
 *
 * \param pipe      the pipe, must be valid
 */
void osiPipeDelete(osiPipe_t *pipe);

/**
 * \brief reset the pipe
 *
 * After reset, the pipe is empty, and running.
 *
 * \param pipe      the pipe, must be valid
 */
void osiPipeReset(osiPipe_t *pipe);

/**
 * \brief set writer callback
 *
 * \param pipe      the pipe, must be valid
 * \param mask      event mask, only masked event will trigger callback
 * \param cb        writer callback
 * \param ctx       writer callback context
 */
void osiPipeSetWriterCallback(osiPipe_t *pipe, unsigned mask, osiPipeEventCallback_t cb, void *ctx);

/**
 * \brief set reader callback
 *
 * \param pipe      the pipe, must be valid
 * \param mask      event mask, only masked event will trigger callback
 * \param cb        reader callback
 * \param ctx       reader callback context
 */
void osiPipeSetReaderCallback(osiPipe_t *pipe, unsigned mask, osiPipeEventCallback_t cb, void *ctx);

/**
 * \brief stop the pipe
 *
 * After stop, both read and write will return -1.
 *
 * \param pipe      the pipe, must be valid
 */
void osiPipeStop(osiPipe_t *pipe);

/**
 * \brief whether the pipe is stopped
 *
 * \param pipe      the pipe, must be valid
 * \return
 *      - true if the pipe is stopped
 *      - true if the pipe is running
 */
bool osiPipeIsStopped(osiPipe_t *pipe);

/**
 * \brief set eof for the pipe
 *
 * This should be called by writer, to indicate the end of pipe write.
 *
 * \param pipe      the pipe, must be valid
 */
void osiPipeSetEof(osiPipe_t *pipe);

/**
 * \brief whether the pipe write is endded
 *
 * This should be called by reader.
 *
 * \param pipe      the pipe, must be valid
 * \return
 *      - true if eof flag is set by writer
 *      - false if not
 */
bool osiPipeIsEof(osiPipe_t *pipe);

/**
 * \brief read data from pipe
 *
 * When data available in pipe is less than \p size, the return value will
 * be less than \p size.
 *
 * \param pipe      the pipe, must be valid
 * \param buf       buffer to store data
 * \param size      buffer size
 * \return
 *      - the number of bytes actually read from pipe
 *      - -1 on invalid parameter
 */
int osiPipeRead(osiPipe_t *pipe, void *buf, unsigned size);

/**
 * \brief write data to pipe
 *
 * When available space in pipe is less than \p size, the return value will
 * be less than \p size.
 *
 * \param pipe      the pipe, must be valid
 * \param buf       buffer to be sent
 * \param size      buffer size
 * \return
 *      - the number of bytes actually written to pipe
 *      - -1 on invalid parameter
 */
int osiPipeWrite(osiPipe_t *pipe, const void *buf, unsigned size);

/**
 * \brief read data from pipe, and wait with timeout
 *
 * When data available in pipe is less than \p size, it will be blocked to
 * wait data available.
 *
 * \param pipe      the pipe, must be valid
 * \param buf       buffer to store data
 * \param size      buffer size
 * \param timeout   wait timeout in milliseconds
 * \return
 *      - the number of bytes actually read from pipe
 *      - -1 on invalid parameter
 */
int osiPipeReadAll(osiPipe_t *pipe, void *buf, unsigned size, unsigned timeout);

/**
 * \brief write data to pipe, and wait with timeout
 *
 * When available space in pipe is less than \p size, it will be blocked to
 * wait available space.
 *
 * \param pipe      the pipe, must be valid
 * \param buf       buffer to be sent
 * \param size      buffer size
 * \param timeout   wait timeout in milliseconds
 * \return
 *      - the number of bytes actually written to pipe
 *      - -1 on invalid parameter
 */
int osiPipeWriteAll(osiPipe_t *pipe, const void *buf, unsigned size, unsigned timeout);

/**
 * \brief data available in pipe for read
 *
 * \param pipe      the pipe, must be valid
 * \return
 *      - the number of bytes available for read
 *      - -1 on invalid parameter
 */
int osiPipeReadAvail(osiPipe_t *pipe);

/**
 * \brief space available in pipe for write
 *
 * \param pipe      the pipe, must be valid
 * \return
 *      - the number of bytes available for write
 *      - -1 on invalid parameter
 */
int osiPipeWriteAvail(osiPipe_t *pipe);

/**
 * \brief wait available bytes for read
 *
 * \param pipe      the pipe, must be valid
 * \return
 *      - true if there are available bytes for read
 *      - false on timeout
 */
bool osiPipeWaitReadAvail(osiPipe_t *pipe, unsigned timeout);

/**
 * \brief wait available space for write
 *
 * \param pipe      the pipe, must be valid
 * \return
 *      - true if there are available space for write
 *      - false on timeout
 */
bool osiPipeWaitWriteAvail(osiPipe_t *pipe, unsigned timeout);

#ifdef __cplusplus
}
#endif
#endif
