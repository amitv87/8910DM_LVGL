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

#include "audio_reader.h"
#include "vfs.h"
#include "osi_log.h"
#include "osi_pipe.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void auReaderDelete(auReader_t *d)
{
    if (d != NULL && d->ops.destroy != NULL)
        d->ops.destroy(d);
}

int auReaderRead(auReader_t *d, void *buf, unsigned size)
{
    if (d != NULL && d->ops.read != NULL)
        return d->ops.read(d, buf, size);
    return -1;
}

int auReaderReadAt(auReader_t *d, int offset, void *buf, unsigned size)
{
    if (auReaderSeek(d, offset, SEEK_SET) < 0)
        return -1;
    return auReaderRead(d, buf, size);
}

bool auReaderIsSeekable(auReader_t *d)
{
    return (d != NULL && d->ops.seek != NULL);
}

int auReaderSeek(auReader_t *d, int offset, int whence)
{
    if (d != NULL && d->ops.seek != NULL)
        return d->ops.seek(d, offset, whence);
    return -1;
}

int auReaderDrop(auReader_t *d, unsigned size)
{
    if (d == NULL)
        return -1;

    if (d->ops.seek != NULL)
        return d->ops.seek(d, size, SEEK_CUR) < 0 ? -1 : 0;

    char buf[64];
    while (size > 0)
    {
        unsigned count = OSI_MIN(unsigned, size, 64);
        int rbytes = auReaderRead(d, buf, count);
        if (rbytes < 0)
            return -1;

        size -= count;
    }
    return 0;
}

bool auReaderIsEof(auReader_t *d)
{
    if (d != NULL && d->ops.is_eof != NULL)
        return d->ops.is_eof(d);
    return -1;
}

/**
 * file reader
 */
struct auFileReader
{
    auReaderOps_t ops;
    int fd;
};

static void prvFileDelete(auReader_t *d)
{
    auFileReader_t *p = (auFileReader_t *)d;
    if (p == NULL)
        return;

    OSI_LOGI(0, "audio file reader delete");
    vfs_close(p->fd);
    free(p);
}

static int prvFileRead(auReader_t *d, void *buf, unsigned size)
{
    auFileReader_t *p = (auFileReader_t *)d;
    return vfs_read(p->fd, buf, size);
}

static int prvFileSeek(auReader_t *d, int offset, int whence)
{
    auFileReader_t *p = (auFileReader_t *)d;
    return vfs_lseek(p->fd, offset, whence);
}

static bool prvFileEof(auReader_t *d)
{
    auFileReader_t *p = (auFileReader_t *)d;

    struct stat st;
    if (vfs_fstat(p->fd, &st) < 0)
        return true;

    long pos = vfs_lseek(p->fd, 0, SEEK_CUR);
    if (pos < 0)
        return true;

    return (pos >= st.st_size);
}

auFileReader_t *auFileReaderCreate(const char *fname)
{
    OSI_LOGI(0, "audio file reader create");

    int fd = vfs_open(fname, O_RDONLY);
    if (fd < 0)
        return NULL;

    auFileReader_t *d = (auFileReader_t *)calloc(1, sizeof(auFileReader_t));
    if (d == NULL)
        return NULL;

    d->ops.destroy = prvFileDelete;
    d->ops.read = prvFileRead;
    d->ops.seek = prvFileSeek;
    d->ops.is_eof = prvFileEof;
    d->fd = fd;
    return d;
}

/**
 * memory reader
 */
struct auMemReader
{
    auReaderOps_t ops;
    const void *buf;
    unsigned size;
    unsigned pos;
};

static void prvMemDelete(auReader_t *d)
{
    auMemReader_t *p = (auMemReader_t *)d;
    if (p == NULL)
        return;

    OSI_LOGI(0, "audio meory reader delete");
    free(p);
}

static int prvMemRead(auReader_t *d, void *buf, unsigned size)
{
    auMemReader_t *p = (auMemReader_t *)d;

    if (size == 0 || p->pos >= p->size)
        return 0;

    if (buf == NULL)
        return -1;

    size = OSI_MIN(unsigned, size, p->size - p->pos);
    memcpy(buf, (const char *)p->buf + p->pos, size);
    p->pos += size;
    return size;
}

static int prvMemSeek(auReader_t *d, int offset, int whence)
{
    auMemReader_t *p = (auMemReader_t *)d;

    switch (whence)
    {
    case SEEK_SET:
        p->pos = OSI_MAX(int, 0, OSI_MIN(int, p->size, offset));
        break;
    case SEEK_CUR:
        p->pos = OSI_MAX(int, 0, OSI_MIN(int, p->size, p->pos + offset));
        break;
    case SEEK_END:
        p->pos = OSI_MAX(int, 0, OSI_MIN(int, p->size, p->size + offset));
        break;
    default:
        return -1;
    }
    return p->pos;
}

static bool prvMemEof(auReader_t *d)
{
    auMemReader_t *p = (auMemReader_t *)d;
    return (p->pos >= p->size);
}

auMemReader_t *auMemReaderCreate(const void *buf, unsigned size)
{
    OSI_LOGI(0, "audio memory reader create, buf/0x%x size/%d", buf, size);

    if (size > 0 && buf == NULL)
        return NULL;

    auMemReader_t *p = (auMemReader_t *)calloc(1, sizeof(auMemReader_t));
    if (p == NULL)
        return NULL;

    p->ops.destroy = prvMemDelete;
    p->ops.read = prvMemRead;
    p->ops.seek = prvMemSeek;
    p->ops.is_eof = prvMemEof;
    p->buf = buf;
    p->size = size;
    p->pos = 0;
    return p;
}

/**
 * pipe reader
 */
struct auPipeReader
{
    auReaderOps_t ops;
    osiPipe_t *pipe;
    unsigned timeout;
};

#define PIPE_READ_WAIT_TIMEOUT (0)

static void prvPipeDelete(auReader_t *d)
{
    auPipeReader_t *p = (auPipeReader_t *)d;
    if (p == NULL)
        return;

    OSI_LOGI(0, "audio pipe reader delete");
    free(p);
}

static int prvPipeRead(auReader_t *d, void *buf, unsigned size)
{
    auPipeReader_t *p = (auPipeReader_t *)d;
    return osiPipeReadAll(p->pipe, buf, size, p->timeout);
}

static bool prvPipeEof(auReader_t *d)
{
    auPipeReader_t *p = (auPipeReader_t *)d;
    return osiPipeIsEof(p->pipe);
}

auPipeReader_t *auPipeReaderCreate(osiPipe_t *pipe)
{
    OSI_LOGI(0, "audio pipe reader create, pipe/0x%x", pipe);

    if (pipe == NULL)
        return NULL;

    auPipeReader_t *d = (auPipeReader_t *)calloc(1, sizeof(auPipeReader_t));
    if (d == NULL)
        return NULL;

    d->ops.destroy = prvPipeDelete;
    d->ops.read = prvPipeRead;
    d->ops.seek = NULL;
    d->ops.is_eof = prvPipeEof;
    d->pipe = pipe;
    d->timeout = PIPE_READ_WAIT_TIMEOUT;
    return d;
}

void auPipeReaderSetWait(auPipeReader_t *d, unsigned timeout)
{
    d->timeout = timeout;
}

/**
 * Read buffer initialization
 */
void auReadBufInit(auReadBuf_t *d, void *buf)
{
    d->buf = (uint8_t *)buf;
    d->size = 0;
    d->pos = 0;
    d->file_pos = 0;
    d->data_start = 0;
    d->data_end = UINT32_MAX;
}

/**
 * Read buffer reset
 */
void auReadBufReset(auReadBuf_t *d, unsigned file_pos)
{
    d->size = 0;
    d->pos = 0;
    d->file_pos = file_pos;
}

/**
 * Read buffer fetch, return valid data size
 */
unsigned auReadBufFetch(auReadBuf_t *d, auReader_t *reader, unsigned size)
{
    if (auReadBufSize(d) >= size)
        goto done;

    if (d->pos >= d->size)
    {
        d->pos = 0;
        d->size = 0;
    }
    else if (d->pos != 0)
    {
        memmove(&d->buf[0], &d->buf[d->pos], d->size - d->pos);
        d->size -= d->pos;
        d->pos = 0;
    }

    if (d->file_pos >= d->data_end)
        goto done;

    unsigned bytes = OSI_MIN(unsigned, size - d->size, d->data_end - d->file_pos);
    int rbytes = auReaderRead(reader, &d->buf[d->size], bytes);
    if (rbytes <= 0)
        goto done;

    d->size += rbytes;

done:
    return auReadBufSize(d);
}

/**
 * Read buffer skip
 */
bool auReadBufSkip(auReadBuf_t *d, auReader_t *reader, unsigned size)
{
    d->file_pos += size;
    if (auReadBufSize(d) >= size)
    {
        d->pos += size;
    }
    else
    {
        // coverity[check_return] return value ignored intentionally
        if (auReaderDrop(reader, size - (d->size - d->pos)) < 0)
            return false;
        d->size = 0;
        d->pos = 0;
    }
    return true;
}
