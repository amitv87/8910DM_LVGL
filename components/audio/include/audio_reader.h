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

#ifndef _AUDIO_READER_H_
#define _AUDIO_READER_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief forward declaration
 */
struct auReader;
struct osiPipe;

/**
 * \brief audio reader operations
 */
typedef struct
{
    /** delete the audio reader */
    void (*destroy)(struct auReader *d);
    /** refer to \p auReaderRead */
    int (*read)(struct auReader *d, void *buf, unsigned size);
    /** refer to \p auReaderSeek */
    int (*seek)(struct auReader *d, int offset, int whence);
    /** refer to \p auReaderIsEof */
    bool (*is_eof)(struct auReader *d);
} auReaderOps_t;

/**
 * \brief audio reader base
 *
 * Audio reader is an abstraction of file. The basic operation is to read
 * from the reader. And the reader doen't care about the content.
 *
 * When implement an audio reader, the first field should be
 * <tt>auReaderOps_t</tt>.
 */
typedef struct auReader
{
    auReaderOps_t ops;
} auReader_t;

/**
 * \brief delete the audio reader
 *
 * When \p d is NULL, nothing will be done.
 *
 * \param d         audio reader
 */
void auReaderDelete(auReader_t *d);

/**
 * \brief read data from audio reader
 *
 * When the reader reaches end of file, it will return 0.
 *
 * \param d         audio reader
 * \param buf       buf to be fetched, can't be NULL if \p size is not 0.
 * \param size      buf size
 * \return
 *      - the size fetched from audio reader, which may be less than \p size.
 *      - -1 for invalid parameters, or file read error.
 */
int auReaderRead(auReader_t *d, void *buf, unsigned size);

/**
 * \brief read data from audio reader, at specified offset
 *
 * This is the same as \p auReaderSeek followed by \p auReaderRead.
 *
 * \param d         audio reader
 * \param offset    specified offset
 * \param buf       buf to be fetched, can't be NULL if \p size is not 0.
 * \param size      buf size
 * \return
 *      - the size fetched from audio reader, which may be less than \p size.
 *      - -1 for invalid parameters, or file read error.
 */
int auReaderReadAt(auReader_t *d, int offset, void *buf, unsigned size);

/**
 * \brief whether the audio reader support seek
 *
 * When \p seek function pointer is NULL, it is not seekable.
 *
 * \param d         audio reader
 * \return
 *      - true if the audio reader supports seek
 *      - false if the audio reader doesn't support seek
 */
bool auReaderIsSeekable(auReader_t *d);

/**
 * \brief audio reader seek
 *
 * Macros for \p whence are defined in <tt>stdio.h</tt>.
 *
 * \param d         audio reader
 * \param offset    offset according to the directive whence
 * \param whence    SEEK_SET, SEEK_CUR, SEEK_END
 * \return
 *      - offset location from the beginning of reader on success.
 *      - -1 on error.
 */
int auReaderSeek(auReader_t *d, int offset, int whence);

/**
 * \brief audio reader drop data
 *
 * When audio reader supports seek, it is the same to seek forward.
 * Otherwise, it will try to read-and-drop specified size.
 *
 * \param d         audio reader
 * \param size      byte count to be dropped
 * \return
 *      - 0 on success.
 *      - -1 on error.
 */
int auReaderDrop(auReader_t *d, unsigned size);

/**
 * \brief whether the reader reaches end of file
 *
 * \param d         audio reader
 * \return
 *      - true when the reader reaches end of file
 *      - false if not
 */
bool auReaderIsEof(auReader_t *d);

/**
 * \brief opaque data strcture of file based audio reader
 */
typedef struct auFileReader auFileReader_t;

/**
 * \brief create a file based audio reader
 *
 * \param fname     file name
 * \return
 *      - the created audio reader
 *      - NULL if invalid parameter, or out of memory
 */
auFileReader_t *auFileReaderCreate(const char *fname);

/**
 * \brief opaque data structure of meory based audio reader
 */
typedef struct auMemReader auMemReader_t;

/**
 * \brief create a memory based audio reader
 *
 * When \p buf is NULL or \p size is 0, it will create an empty audio reader.
 *
 * \param buf       buffer
 * \param size      buffer size
 * \return
 *      - the created audio reader
 *      - NULL if out of memory
 */
auMemReader_t *auMemReaderCreate(const void *buf, unsigned size);

/**
 * \brief opaque data structure for pip based audio reader
 */
typedef struct auPipeReader auPipeReader_t;

/**
 * \brief create a pipe based audio reader
 *
 * Pipe base audio reader will decouple audio data producing and consuming.
 * Seek is unsupported in pipe based audio reader. Audio reader will just
 * access the reader side of pipe.
 *
 * \param pipe      the pipe to be read
 * \return
 *      - the created audio reader
 *      - NULL if out of memory, or invalid parameter
 */
auPipeReader_t *auPipeReaderCreate(struct osiPipe *pipe);

/**
 * \brief set wait timeout for audio pipe reader
 *
 * By default, audio pipe read will wait a while when the pipe is empty, or
 * not enough for the requested byte count. This can set the wait timeout,
 * 0 for not waiting and \p OSI_WAIT_FOREVER for infinite wait until the
 * requested data are read.
 *
 * When \p timeout is set to 0, the pipe will be read repeatedly. In most
 * cases, it is not wanted.
 *
 * \param d         the audio pipe reader
 * \param timeout   wait timeout
 */
void auPipeReaderSetWait(auPipeReader_t *d, unsigned timeout);

/**
 * \brief helper to manage input buffer for audio reader
 *
 * The struct definition is just to make it easier to embed data struct.
 * DON'T acess members directly.
 */
typedef struct
{
    uint8_t *buf;        ///< the real input buffer
    unsigned size;       ///< byte count in the buffer
    unsigned pos;        ///< read position in the buffer
    unsigned file_pos;   ///< file position for the read position
    unsigned data_start; ///< valid data start in file
    unsigned data_end;   ///< valid data end in file
} auReadBuf_t;

/**
 * \brief initialize read buffer
 *
 * \param d         read buffer
 * \param buf       real input buffer pointer
 */
void auReadBufInit(auReadBuf_t *d, void *buf);

/**
 * \brief clear read buffer, and set file position
 *
 * \param d         read buffer
 * \param file_pos  file position
 */
void auReadBufReset(auReadBuf_t *d, unsigned file_pos);

/**
 * \brief fetch from reader
 *
 * When needed, it will read from audio reader, and try to ensure
 * valid data size is greater than \p size.
 *
 * \param d         read buffer
 * \param reader    audio reader
 * \param size      requested valid data size
 * \return
 *      - valid data size in read buffer
 */
unsigned auReadBufFetch(auReadBuf_t *d, auReader_t *reader, unsigned size);

/**
 * \brief skip bytes in reader buffer
 *
 * \p size may be larger than valid data size in read buffer. In this
 * case, it will call \p auReaderDrop.
 *
 * \param d         read buffer
 * \param reader    audio reader
 * \param size      requested byte count to be skipped
 * \return
 *      - true on success.
 *      - false on error.
 */
bool auReadBufSkip(auReadBuf_t *d, auReader_t *reader, unsigned size);

/**
 * \brief valid data pointer
 *
 * \param d         read buffer
 * \return
 *      - valid data pointer
 */
static inline uint8_t *auReadBufData(auReadBuf_t *d) { return &d->buf[d->pos]; }

/**
 * \brief valid data size
 *
 * \param d         read buffer
 * \return
 *      - valid data size
 */
static inline unsigned auReadBufSize(auReadBuf_t *d) { return d->size - d->pos; }

/**
 * \brief file position
 *
 * \param d         read buffer
 * \return
 *      - file position
 */
static inline unsigned auReadBufFilePos(auReadBuf_t *d) { return d->file_pos; }

/**
 * \brief data start position in file
 *
 * \param d         read buffer
 * \return
 *      - data start position in file
 */
static inline unsigned auReadBufDataStart(auReadBuf_t *d) { return d->data_start; }

/**
 * \brief data end position in file
 *
 * \param d         read buffer
 * \return
 *      - data end position in file
 */
static inline unsigned auReadBufDataEnd(auReadBuf_t *d) { return d->data_end; }

/**
 * \brief set data start position in file
 *
 * \param d             read buffer
 * \param data_start    data start position in file
 */
static inline void auReadBufSetDataStart(auReadBuf_t *d, unsigned data_start) { d->data_start = data_start; }

/**
 * \brief set data end position in file
 *
 * \param d             read buffer
 * \param data_end      data end position in file
 */
static inline void auReadBufSetDataEnd(auReadBuf_t *d, unsigned data_end) { d->data_end = data_end; }

/**
 * \brief data read is finish
 *
 * \param d         read buffer
 * \return
 *      - true  finish
 *      - false not finish
 */
static inline bool auReadBufFetchAllData(auReadBuf_t *d) { return (d->file_pos >= d->data_end); }

OSI_EXTERN_C_END
#endif
