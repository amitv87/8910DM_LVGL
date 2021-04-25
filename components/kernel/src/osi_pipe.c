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

#include "osi_pipe.h"
#include "osi_api.h"
#include <stdlib.h>
#include <string.h>

#include "quec_proj_config.h"

struct osiPipe
{
    volatile bool running;
    volatile bool eof;
    unsigned size;
    unsigned rd;
    unsigned wr;
    osiSemaphore_t *rd_avail_sema;
    osiSemaphore_t *wr_avail_sema;
    unsigned rd_cb_mask;
    osiPipeEventCallback_t rd_cb;
    void *rd_cb_ctx;
    unsigned wr_cb_mask;
    osiPipeEventCallback_t wr_cb;
    void *wr_cb_ctx;
	char data_done;
    char data[];
};

#ifdef CONFIG_QUEC_PROJECT_FEATURE_AUDIO
void osiPipeDataEnd(osiPipe_t *pipe)
{
	pipe->data_done = 1;
}
#endif

osiPipe_t *osiPipeCreate(unsigned size)
{
    if (size == 0)
        return NULL;

    osiPipe_t *pipe = (osiPipe_t *)calloc(1, sizeof(osiPipe_t) + size);
    if (pipe == NULL)
        return NULL;

    pipe->running = true;
    pipe->eof = false;
    pipe->size = size;
    pipe->wr_avail_sema = osiSemaphoreCreate(1, 1);
    pipe->rd_avail_sema = osiSemaphoreCreate(1, 0);
    if (pipe->wr_avail_sema == NULL || pipe->rd_avail_sema == NULL)
    {
        osiSemaphoreDelete(pipe->wr_avail_sema);
        osiSemaphoreDelete(pipe->rd_avail_sema);
        free(pipe);
        return NULL;
    }
    return pipe;
}

void osiPipeDelete(osiPipe_t *pipe)
{
    if (pipe == NULL)
        return;

    osiSemaphoreDelete(pipe->wr_avail_sema);
    osiSemaphoreDelete(pipe->rd_avail_sema);
    free(pipe);
}

void osiPipeReset(osiPipe_t *pipe)
{
    if (pipe == NULL)
        return;

    uint32_t critical = osiEnterCritical();
    pipe->rd = 0;
    pipe->wr = 0;
    pipe->running = true;
    pipe->eof = false;
    osiExitCritical(critical);
}

void osiPipeStop(osiPipe_t *pipe)
{
    uint32_t critical = osiEnterCritical();
    pipe->running = false;
    osiSemaphoreRelease(pipe->wr_avail_sema);
    osiSemaphoreRelease(pipe->rd_avail_sema);
    osiExitCritical(critical);
}

bool osiPipeIsStopped(osiPipe_t *pipe)
{
    return !pipe->running;
}

void osiPipeSetEof(osiPipe_t *pipe)
{
    uint32_t critical = osiEnterCritical();
    pipe->eof = true;
    osiSemaphoreRelease(pipe->wr_avail_sema);
    osiSemaphoreRelease(pipe->rd_avail_sema);
    osiExitCritical(critical);
}

bool osiPipeIsEof(osiPipe_t *pipe)
{
    return pipe->eof;
}

void osiPipeSetWriterCallback(osiPipe_t *pipe, unsigned mask, osiPipeEventCallback_t cb, void *ctx)
{
    if (pipe == NULL)
        return;

    pipe->wr_cb_mask = mask;
    pipe->wr_cb = cb;
    pipe->wr_cb_ctx = ctx;
}

void osiPipeSetReaderCallback(osiPipe_t *pipe, unsigned mask, osiPipeEventCallback_t cb, void *ctx)
{
    if (pipe == NULL)
        return;

    pipe->rd_cb_mask = mask;
    pipe->rd_cb = cb;
    pipe->rd_cb_ctx = ctx;
}

int osiPipeRead(osiPipe_t *pipe, void *buf, unsigned size)
{
    if (size == 0)
        return 0;
    if (pipe == NULL || buf == NULL)
        return -1;

    uint32_t critical = osiEnterCritical();
    unsigned bytes = pipe->wr - pipe->rd;
    unsigned len = OSI_MIN(unsigned, size, bytes);
    unsigned rd = pipe->rd;

    if (!pipe->running)
    {
        osiExitCritical(critical);
        return -1;
    }

#ifdef CONFIG_QUEC_PROJECT_FEATURE_AUDIO
	if(pipe->data_done == 1 && bytes == 0)
	{
		osiPipeSetEof(pipe);
		return -1;
	}
#endif

    if (len == 0)
    {
        osiExitCritical(critical);
        return 0;
    }

    unsigned offset = rd % pipe->size;
    unsigned tail = pipe->size - offset;
    if (tail >= len)
    {
        memcpy(buf, &pipe->data[offset], len);
    }
    else
    {
        memcpy(buf, &pipe->data[offset], tail);
        memcpy((char *)buf + tail, pipe->data, len - tail);
    }

    pipe->rd += len;
    osiExitCritical(critical);

    if (len == bytes)
    {
        if (pipe->wr_cb != NULL && (pipe->wr_cb_mask & OSI_PIPE_EVENT_TX_COMPLETE))
            pipe->wr_cb(pipe->wr_cb_ctx, OSI_PIPE_EVENT_TX_COMPLETE);
    }

    osiSemaphoreRelease(pipe->wr_avail_sema);
    return len;
}

int osiPipeWrite(osiPipe_t *pipe, const void *buf, unsigned size)
{
    if (size == 0)
        return 0;
    if (pipe == NULL || buf == NULL)
        return -1;

    uint32_t critical = osiEnterCritical();
    unsigned space = pipe->size - (pipe->wr - pipe->rd);
    unsigned len = OSI_MIN(unsigned, size, space);
    unsigned wr = pipe->wr;

    if (!pipe->running || pipe->eof)
    {
        osiExitCritical(critical);
        return -1;
    }

    if (len == 0)
    {
        osiExitCritical(critical);
        return 0;
    }

    unsigned offset = wr % pipe->size;
    unsigned tail = pipe->size - offset;
    if (tail >= len)
    {
        memcpy(&pipe->data[offset], buf, len);
    }
    else
    {
        memcpy(&pipe->data[offset], buf, tail);
        memcpy(pipe->data, (const char *)buf + tail, len - tail);
    }

    pipe->wr += len;
    osiExitCritical(critical);

    if (len > 0)
    {
        if (pipe->rd_cb != NULL && (pipe->rd_cb_mask & OSI_PIPE_EVENT_RX_ARRIVED))
            pipe->rd_cb(pipe->rd_cb_ctx, OSI_PIPE_EVENT_RX_ARRIVED);
    }

    osiSemaphoreRelease(pipe->rd_avail_sema);
    return len;
}

int osiPipeReadAll(osiPipe_t *pipe, void *buf, unsigned size, unsigned timeout)
{
    if (size == 0)
        return 0;
    if (pipe == NULL || buf == NULL)
        return -1;

    unsigned len = 0;
    osiElapsedTimer_t timer;
    osiElapsedTimerStart(&timer);
    for (;;)
    {	
        int bytes = osiPipeRead(pipe, buf, size);
        if (bytes < 0)
            return -1;

        len += bytes;
        size -= bytes;
        buf = (char *)buf + bytes;
        if (size == 0 || timeout == 0 || pipe->eof)
            break;

        if (timeout == OSI_WAIT_FOREVER)
        {
            osiSemaphoreAcquire(pipe->rd_avail_sema);
        }
        else
        {
            int wait = timeout - osiElapsedTime(&timer);
            if (wait < 0 || !osiSemaphoreTryAcquire(pipe->rd_avail_sema, wait))
                break;
        }
    }

    return len;
}

int osiPipeWriteAll(osiPipe_t *pipe, const void *buf, unsigned size, unsigned timeout)
{
    if (size == 0)
        return 0;
    if (pipe == NULL || buf == NULL)
        return -1;

    unsigned len = 0;
    osiElapsedTimer_t timer;
    osiElapsedTimerStart(&timer);
    for (;;)
    {
        int bytes = osiPipeWrite(pipe, buf, size);
        if (bytes < 0)
            return -1;

        len += bytes;
        size -= bytes;
        buf = (const char *)buf + bytes;
        if (size == 0 || timeout == 0)
            break;

        if (timeout == OSI_WAIT_FOREVER)
        {
            osiSemaphoreAcquire(pipe->wr_avail_sema);
        }
        else
        {
            int wait = timeout - osiElapsedTime(&timer);
            if (wait < 0 || !osiSemaphoreTryAcquire(pipe->wr_avail_sema, wait))
                break;
        }
    }

    return len;
}

int osiPipeReadAvail(osiPipe_t *pipe)
{
    if (pipe == NULL)
        return -1;

    uint32_t critical = osiEnterCritical();
    unsigned bytes = pipe->wr - pipe->rd;
    osiExitCritical(critical);
    return bytes;
}

int osiPipeWriteAvail(osiPipe_t *pipe)
{
    if (pipe == NULL)
        return -1;

    uint32_t critical = osiEnterCritical();
    unsigned space = pipe->size - (pipe->wr - pipe->rd);
    osiExitCritical(critical);
    return space;
}

bool osiPipeWaitReadAvail(osiPipe_t *pipe, unsigned timeout)
{
    if (pipe == NULL)
        return false;

    osiElapsedTimer_t timer;
    osiElapsedTimerStart(&timer);
    for (;;)
    {
        if (!pipe->running)
            return false;

        if (osiPipeReadAvail(pipe) > 0)
            return true;

        if (pipe->eof)
            return false;

        if (timeout == OSI_WAIT_FOREVER)
        {
            osiSemaphoreAcquire(pipe->rd_avail_sema);
        }
        else
        {
            int wait = timeout - osiElapsedTime(&timer);
            if (wait < 0 || !osiSemaphoreTryAcquire(pipe->rd_avail_sema, wait))
                return false;
        }
    }

    // never reach
}

bool osiPipeWaitWriteAvail(osiPipe_t *pipe, unsigned timeout)
{
    if (pipe == NULL)
        return false;

    osiElapsedTimer_t timer;
    osiElapsedTimerStart(&timer);
    for (;;)
    {
        if (!pipe->running)
            return false;

        if (osiPipeWriteAvail(pipe) > 0)
            return true;

        if (timeout == OSI_WAIT_FOREVER)
        {
            osiSemaphoreAcquire(pipe->wr_avail_sema);
        }
        else
        {
            int wait = timeout - osiElapsedTime(&timer);
            if (wait < 0 || !osiSemaphoreTryAcquire(pipe->wr_avail_sema, wait))
                return false;
        }
    }

    // never reach
}
