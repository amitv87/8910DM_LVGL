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

#include "audio_writer.h"
#include "vfs.h"
#include "osi_pipe.h"
#include "osi_log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void auWriterDelete(auWriter_t *d)
{
    if (d != NULL && d->ops.destroy != NULL)
        d->ops.destroy(d);
}

int auWriterWrite(auWriter_t *d, const void *buf, unsigned size)
{
    if (d != NULL && d->ops.write != NULL)
        return d->ops.write(d, buf, size);
    return -1;
}

bool auWriterIsSeekable(auWriter_t *d)
{
    return (d != NULL && d->ops.seek != NULL);
}

int auWriterSeek(auWriter_t *d, int offset, int whence)
{
    if (d != NULL && d->ops.seek != NULL)
        return d->ops.seek(d, offset, whence);
    return -1;
}

/**
 * File writer
 */
struct auFileWriter
{
    auWriterOps_t ops;
    int fd;
};

static void prvFileWriterDelete(auWriter_t *d)
{
    auFileWriter_t *p = (auFileWriter_t *)d;
    if (p == NULL)
        return;

    OSI_LOGI(0, "audio file writer deleted");
    vfs_close(p->fd);
    free(p);
}

static int prvFileWriterWrite(auWriter_t *d, const void *buf, unsigned size)
{
    auFileWriter_t *p = (auFileWriter_t *)d;
    return vfs_write(p->fd, buf, size);
}

static int prvFileWriterSeek(auWriter_t *d, int offset, int whence)
{
    auFileWriter_t *p = (auFileWriter_t *)d;
    return vfs_lseek(p->fd, offset, whence);
}

auFileWriter_t *auFileWriterCreate(const char *fname)
{
    OSI_LOGI(0, "audio file writer create");

    int fd = vfs_open(fname, O_RDWR | O_CREAT | O_TRUNC);
    if (fd < 0)
        return NULL;

    auFileWriter_t *d = (auFileWriter_t *)calloc(1, sizeof(auFileWriter_t));
    if (d == NULL)
        return NULL;

    d->ops.destroy = prvFileWriterDelete;
    d->ops.write = prvFileWriterWrite;
    d->ops.seek = prvFileWriterSeek;
    d->fd = fd;
    return d;
}

/**
 * Memory writer
 */
struct auMemWriter
{
    auWriterOps_t ops;
    unsigned max_size;
    unsigned size;
    unsigned pos;
    char buf[0];
};

static void prvMemWriterDelete(auWriter_t *d)
{
    auMemWriter_t *p = (auMemWriter_t *)d;
    if (p == NULL)
        return;

    OSI_LOGI(0, "audio memory writer deleted");
    free(p);
}

static int prvMemWriterWrite(auWriter_t *d, const void *buf, unsigned size)
{
    auMemWriter_t *p = (auMemWriter_t *)d;

    if (size == 0)
        return 0;

    if (p->pos >= p->max_size)
        return -1; // ENOSPC

    if (buf == NULL)
        return -1;

    size = OSI_MIN(unsigned, size, p->max_size - p->pos);
    memcpy((char *)p->buf + p->pos, buf, size);
    p->size = OSI_MAX(unsigned, p->size, p->pos + size);
    p->pos += size;
    return size;
}

static int prvMemWriterSeek(auWriter_t *d, int offset, int whence)
{
    auMemWriter_t *p = (auMemWriter_t *)d;

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

auMemWriter_t *auMemWriterCreate(unsigned max_size)
{
    OSI_LOGI(0, "audio memory writer create, max size/%d", max_size);

    if (max_size == 0)
        return NULL;

    auMemWriter_t *p = (auMemWriter_t *)calloc(1, sizeof(auMemWriter_t) + max_size);
    if (p == NULL)
        return NULL;

    p->ops.destroy = prvMemWriterDelete;
    p->ops.write = prvMemWriterWrite;
    p->ops.seek = prvMemWriterSeek;
    p->max_size = max_size;
    p->size = 0;
    p->pos = 0;
    return p;
}

osiBuffer_t auMemWriterGetBuf(auMemWriter_t *d)
{
    osiBuffer_t buf = {
        .ptr = (uintptr_t)d->buf,
        .size = d->size,
    };
    return buf;
}

/**
 * Dummy writer
 */
struct auDummyWriter
{
    auWriterOps_t ops;
};

static void prvDummyDelete(auWriter_t *d)
{
    OSI_LOGI(0, "audio dummy writer deleted");
    free(d);
}

static int prvDummyWrite(auWriter_t *d, const void *buf, unsigned size)
{
    return size;
}

static int prvDummySeek(auWriter_t *d, int offset, int whence)
{
    return 0;
}

auDummyWriter_t *auDummyWriterCreate(void)
{
    OSI_LOGI(0, "audio dummy writer create");

    auDummyWriter_t *p = (auDummyWriter_t *)calloc(1, sizeof(auDummyWriter_t));
    if (p == NULL)
        return NULL;

    p->ops.destroy = prvDummyDelete;
    p->ops.write = prvDummyWrite;
    p->ops.seek = prvDummySeek;
    return p;
}

/**
 * pipe writer
 */
struct auPipeWriter
{
    auWriterOps_t ops;
    osiPipe_t *pipe;
    unsigned timeout;
};

static void prvPipeDelete(auWriter_t *d)
{
    auPipeWriter_t *p = (auPipeWriter_t *)d;
    if (p == NULL)
        return;

    OSI_LOGI(0, "audio pipe writer deleted");
    free(p);
}

static int prvPipeWrite(auWriter_t *d, const void *buf, unsigned size)
{
    auPipeWriter_t *p = (auPipeWriter_t *)d;
    return osiPipeWriteAll(p->pipe, buf, size, p->timeout);
}

auPipeWriter_t *auPipeWriterCreate(osiPipe_t *pipe)
{
    OSI_LOGI(0, "audio pipe writer create, pipe/0x%x", pipe);

    if (pipe == NULL)
        return NULL;

    auPipeWriter_t *d = (auPipeWriter_t *)calloc(1, sizeof(auPipeWriter_t));
    if (d == NULL)
        return NULL;

    d->ops.destroy = prvPipeDelete;
    d->ops.write = prvPipeWrite;
    d->ops.seek = NULL;
    d->pipe = pipe;
    return d;
}

void auPipeWriterSetWait(auPipeWriter_t *d, unsigned timeout)
{
    d->timeout = timeout;
}
