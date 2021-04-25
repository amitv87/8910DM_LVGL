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

#ifndef _AUDIO_WRITER_H_
#define _AUDIO_WRITER_H_

#include "osi_api.h"
#include "osi_byte_buf.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief forward declaration
 */
struct auWriter;

/**
 * \brief forward declaration
 */
struct osiPipe;

/**
 * \brief audio writer operations
 */
typedef struct
{
    /** delete the audio writer */
    void (*destroy)(struct auWriter *d);
    /** refer to \p auWriterWrite */
    int (*write)(struct auWriter *d, const void *buf, unsigned size);
    /** refer to \p auWriterSeek */
    int (*seek)(struct auWriter *d, int offset, int whence);
} auWriterOps_t;

/**
 * \brief audio writer base
 *
 * Audio writer is an abstraction of file. The basic operation is to write
 * to the writer. And the writer doesn't care about the content.
 *
 * When implement an audio writer, the first field should be
 * <tt>const auWriterOps_t *</tt>.
 */
typedef struct auWriter
{
    auWriterOps_t ops;
} auWriter_t;

/**
 * \brief delete the audio writer
 *
 * When \p d is NULL, nothing will be done.
 *
 * \param d         audio writer
 */
void auWriterDelete(auWriter_t *d);

/**
 * \brief write data to audio writer
 *
 * \param d         audio writer
 * \param buf       buf to be written, can't be NULL if \p size is not 0.
 * \param size      buf size
 * \return
 *      - the size written to audio writer, which may be less than \p size.
 *      - -1 for invalid parameters, or write error
 */
int auWriterWrite(auWriter_t *d, const void *buf, unsigned size);

/**
 * \brief whether the audio writer support seek
 *
 * When \p seek function pointer is NULL, it is not seekable.
 *
 * \param d         audio writer
 * \return
 *      - true if the audio writer supports seek
 *      - false if the audio writer doesn't support seek
 */
bool auWriterIsSeekable(auWriter_t *d);

/**
 * \brief audio writer seek
 *
 * Macros for \p whence are defined in <tt>stdio.h</tt>.
 *
 * \param d         audio writer
 * \param offset    offset according to the directive whence
 * \param whence    SEEK_SET, SEEK_CUR, SEEK_END
 * \return
 *      - offset location from the beginning of writer on success.
 *      - -1 on error.
 */
int auWriterSeek(auWriter_t *d, int offset, int whence);

/**
 * \brief opaque data strcture of file based audio writer
 */
typedef struct auFileWriter auFileWriter_t;

/**
 * \brief create a file based audio writer
 *
 * When \p fname doesn't exist, it will be created. When \p fname exists,
 * the file will be truncated to zero length.
 *
 * \param fname     file name
 * \return
 *      - the created audio writer
 *      - NULL if invalid parameter, or out of memory
 */
auFileWriter_t *auFileWriterCreate(const char *fname);

/**
 * \brief opaque data structure of memory based audio writer
 */
typedef struct auMemWriter auMemWriter_t;

/**
 * \brief create a memory based audio writer
 *
 * The memory is dynamic allocated at initialization.
 *
 * \param max_size  maximum memory size, 0 for no limitation
 * \return
 *      - the created audio writer
 *      - NULL if out of memory
 */
auMemWriter_t *auMemWriterCreate(unsigned max_size);

/**
 * \brief get buffer of memory based audio writer
 *
 * The buffer pointer may be changed, except the special case mentioned above,
 * \p init_size is the same as \p max_size.
 *
 * \param d         memory based audio writer
 * \return
 *      - buffer pointer and size.
 */
osiBuffer_t auMemWriterGetBuf(auMemWriter_t *d);

/**
 * \brief opaque data structure for pip based audio writer
 */
typedef struct auPipeWriter auPipeWriter_t;

/**
 * \brief create a pipe based audio writer
 *
 * Pipe base audio writer will decouple audio data producing and consuming.
 * Seek is unsupported in pipe based audio writer. Audio writer will just
 * access the writer side of pipe.
 *
 * \param pipe      the pipe to be write
 * \return
 *      - the created audio writer
 *      - NULL if out of memory, or invalid parameter
 */
auPipeWriter_t *auPipeWriterCreate(struct osiPipe *pipe);

/**
 * \brief set wait timeout for audio pipe writer
 *
 * By default, audio pipe write won't wait and will return immediately even
 * the pipe is full, or not enough for the requested byte count. This can
 * set the wait timeout, 0 for not waiting and \p OSI_WAIT_FOREVER for
 * infinite wait until the requested data are written.
 *
 * \param d         the audio pipe writer
 * \param timeout   wait timeout
 */
void auPipeWriterSetWait(auPipeWriter_t *d, unsigned timeout);

/**
 * \brief opaque data structure of dummy audio writer
 *
 * Dummy writer will drop all incoming data, just provides writer APIs.
 * Typically, it will only be used for debug purpose.
 */
typedef struct auDummyWriter auDummyWriter_t;

/**
 * \brief create a dummy audio writer
 *
 * \return
 *      - the created audio writer
 *      - NULL if out of memory
 */
auDummyWriter_t *auDummyWriterCreate(void);

OSI_EXTERN_C_END
#endif
