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

#include "osi_blocked_fifo.h"
#include "osi_api.h"
#include "osi_log.h"
#include <string.h>
#include <stdlib.h>
#include <sys/queue.h>

typedef TAILQ_ENTRY(osiBlockBuf) osiBlockBufIter_t;
typedef TAILQ_HEAD(osiBlockBufHead, osiBlockBuf) osiBlockBufHead_t;
typedef struct osiBlockBuf
{
    osiBlockBufIter_t iter; ///< iterator for list
    char *data;             ///< start address of the block
    unsigned size;          ///< block size
    unsigned rpos;          ///< block read position
    unsigned wpos;          ///< block write position
} osiBlockBuf_t;

struct osiBlockedFifo
{
    char *data;                   ///< fifo memory
    unsigned size;                ///< fifo memory size
    unsigned block_size;          ///< maximum block size
    unsigned alignment;           ///< block alignmemnt
    unsigned rpos;                ///< read position
    unsigned wpos;                ///< write position
    bool put_pending;             ///< put requested
    bool get_pending;             ///< get requested
    osiBlockBufHead_t ready_list; ///< ready (used) list
    osiBlockBufHead_t avail_list; ///< available (unused) list
};

osiBlockedFifo_t *osiBlockedFifoCreate(void *data, unsigned size, unsigned alignment,
                                       unsigned block_size, unsigned block_count)
{
    if (data == NULL || size < block_size || alignment == 0 ||
        block_size == 0 || block_count == 0)
        return NULL;

    if (!OSI_IS_POW2(alignment))
        return NULL;

    if (!OSI_IS_ALIGNED(data, alignment) ||
        !OSI_IS_ALIGNED(size, alignment) ||
        !OSI_IS_ALIGNED(block_size, alignment))
        return NULL;

    osiBlockedFifo_t *fifo = calloc(1, sizeof(osiBlockedFifo_t) + sizeof(osiBlockBuf_t) * block_count);
    if (fifo == NULL)
        return NULL;

    fifo->data = (char *)data;
    fifo->size = size;
    fifo->block_size = block_size;
    fifo->alignment = alignment;
    TAILQ_INIT(&fifo->ready_list);
    TAILQ_INIT(&fifo->avail_list);

    osiBlockBuf_t *blocks = (osiBlockBuf_t *)((char *)fifo + sizeof(osiBlockedFifo_t));
    for (unsigned n = 0; n < block_count; n++)
        TAILQ_INSERT_HEAD(&fifo->avail_list, &blocks[n], iter);

    osiBlockedFifoReset(fifo);
    return fifo;
}

void osiBlockedFifoReset(osiBlockedFifo_t *fifo)
{
    fifo->rpos = 0;
    fifo->wpos = 0;
    fifo->put_pending = false;
    fifo->get_pending = false;

    osiBlockBuf_t *p;
    while ((p = TAILQ_FIRST(&fifo->ready_list)) != NULL)
    {
        TAILQ_REMOVE(&fifo->ready_list, p, iter);
        TAILQ_INSERT_HEAD(&fifo->avail_list, p, iter);
    }
}

void osiBlockedFifoDelete(osiBlockedFifo_t *fifo)
{
    free(fifo);
}

static inline unsigned prvBytes(osiBlockedFifo_t *fifo)
{
    return (unsigned)(fifo->wpos - fifo->rpos);
}

static inline unsigned prvSpace(osiBlockedFifo_t *fifo)
{
    return fifo->size - prvBytes(fifo);
}

// Allocate buffer, return NULL at full. Ready and avail list aren't changed.
// And fifo->wpos is updated.
static osiBlockBuf_t *prvAllocBuf(osiBlockedFifo_t *fifo)
{
    osiBlockBuf_t *p = TAILQ_FIRST(&fifo->avail_list);
    if (p == NULL)
        return NULL;

    if (TAILQ_EMPTY(&fifo->ready_list))
    {
        p->data = fifo->data;
        p->size = fifo->block_size;
        p->rpos = p->wpos = 0;
        fifo->rpos = 0;
        fifo->wpos = fifo->block_size;
        return p;
    }

    unsigned space = prvSpace(fifo);
    if (space < fifo->block_size)
        return NULL;

    unsigned rpos = fifo->rpos % fifo->size;
    unsigned wpos = fifo->wpos % fifo->size;
    unsigned tail = fifo->size - wpos;
    if (rpos > wpos || tail >= fifo->block_size)
    {
        p->data = fifo->data + wpos;
        p->size = fifo->block_size;
        p->rpos = p->wpos = 0;
        fifo->wpos += fifo->block_size;
        return p;
    }

    if (rpos >= fifo->block_size)
    {
        osiBlockBuf_t *ready = TAILQ_LAST(&fifo->ready_list, osiBlockBufHead);
        ready->size += tail;

        p->data = fifo->data;
        p->size = fifo->block_size;
        p->rpos = p->wpos = 0;
        fifo->wpos += fifo->block_size + tail;
        return p;
    }

    return NULL;
}

// Shrink buffer p, it must be the last ready buffer.
static void prvShrinkBuf(osiBlockedFifo_t *fifo, osiBlockBuf_t *p)
{
    unsigned allocated = p->size;
    p->size = OSI_ALIGN_UP(p->wpos, fifo->alignment);
    fifo->wpos -= (allocated - p->size);
}

unsigned osiBlockedFifoBlockSize(osiBlockedFifo_t *fifo)
{
    return fifo->block_size;
}

bool osiBlockedFifoIsEmpty(osiBlockedFifo_t *fifo)
{
    return TAILQ_EMPTY(&fifo->ready_list);
}

bool osiBlockedFifoIsFull(osiBlockedFifo_t *fifo)
{
    uint32_t critical = osiEnterCritical();

    if (TAILQ_EMPTY(&fifo->avail_list))
        goto full;
    if (prvSpace(fifo) < fifo->block_size)
        goto full;

    // This checking matches prvAllocBuf, to make sure prvAllocBuf can success
    // when this return false.
    unsigned rpos = fifo->rpos % fifo->size;
    unsigned wpos = fifo->wpos % fifo->size;
    unsigned tail = fifo->size - wpos;
    if (rpos > wpos || tail >= fifo->block_size)
        goto not_full;
    if (rpos >= fifo->block_size)
        goto not_full;

    // It is full when comes here
full:
    osiExitCritical(critical);
    return true;

not_full:
    osiExitCritical(critical);
    return false;
}

unsigned osiBlockedFifoBytes(osiBlockedFifo_t *fifo)
{
    uint32_t critical = osiEnterCritical();
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
	osiBlockBuf_t *p = NULL;
	osiBlockBuf_t *last_ready = TAILQ_LAST(&fifo->ready_list, osiBlockBufHead);
	unsigned bytes = 0;
	TAILQ_FOREACH(p, &fifo->ready_list, iter)
	{
		if (fifo->put_pending && p == last_ready)
			break;
		bytes += (p->wpos - p->rpos);
	}
#else		
    unsigned bytes = prvBytes(fifo);
    if (fifo->put_pending)
    {
        osiBlockBuf_t *p = TAILQ_LAST(&fifo->ready_list, osiBlockBufHead);
        bytes -= p->size;
    }
#endif	
    osiExitCritical(critical);
    return bytes;
}

unsigned osiBlockedFifoSpace(osiBlockedFifo_t *fifo)
{
    uint32_t critical = osiEnterCritical();
    unsigned space = prvSpace(fifo);
    osiExitCritical(critical);
    return space;
}

int osiBlockedFifoGet(osiBlockedFifo_t *fifo, void *data, unsigned size)
{
    if (data == NULL || size == 0)
        return 0;

    uint32_t critical = osiEnterCritical();

    if (fifo->get_pending)
    {
        osiExitCritical(critical);
        return 0;
    }

    osiBlockBuf_t *last_ready = TAILQ_LAST(&fifo->ready_list, osiBlockBufHead);
    unsigned rsize = 0;
    while (size > 0)
    {
        osiBlockBuf_t *p = TAILQ_FIRST(&fifo->ready_list);
        if (p == NULL || (fifo->put_pending && p == last_ready))
            break;

        unsigned bsize = OSI_MIN(unsigned, p->wpos - p->rpos, size);
        memcpy(data, p->data + p->rpos, bsize);

        data = (char *)data + bsize;
        size -= bsize;
        rsize += bsize;

        p->rpos += bsize;
        if (p->rpos >= p->wpos)
        {
            TAILQ_REMOVE(&fifo->ready_list, p, iter);
            TAILQ_INSERT_HEAD(&fifo->avail_list, p, iter);
            fifo->rpos += p->size;
        }
    }

    osiExitCritical(critical);
    return rsize;
}

int osiBlockedFifoPut(osiBlockedFifo_t *fifo, const void *data, unsigned size)
{
    if (data == NULL || size == 0)
        return 0;

    uint32_t critical = osiEnterCritical();

    if (fifo->put_pending)
    {
        osiExitCritical(critical);
        return 0;
    }

    unsigned wsize = 0;

    osiBlockBuf_t *p = TAILQ_LAST(&fifo->ready_list, osiBlockBufHead);
    if (p != NULL)
    {
        // When the last is get pending, shrink it. Otherwise, try to
        // append to it.
        if (fifo->get_pending && p == TAILQ_FIRST(&fifo->ready_list))
        {
            prvShrinkBuf(fifo, p);
        }
        else
        {
            // the memory size at tail may exceed block size
            unsigned buf_size = OSI_MIN(unsigned, p->size, fifo->block_size);
            unsigned bsize = OSI_MIN(unsigned, buf_size - p->wpos, size);
            memcpy(p->data + p->wpos, data, bsize);

            data = (char *)data + bsize;
            size -= bsize;
            wsize += bsize;
            p->wpos += bsize;
        }
    }

    while (size > 0)
    {
        p = prvAllocBuf(fifo);
        if (p == NULL)
            break;

        TAILQ_REMOVE(&fifo->avail_list, p, iter);
        unsigned bsize = OSI_MIN(unsigned, p->size - p->wpos, size);
        memcpy(p->data + p->wpos, data, bsize);

        data = (char *)data + bsize;
        size -= bsize;
        wsize += bsize;
        p->wpos += bsize;

        TAILQ_INSERT_TAIL(&fifo->ready_list, p, iter);
    }

    osiExitCritical(critical);
    return wsize;
}

void *osiBlockedFifoPutRequest(osiBlockedFifo_t *fifo, bool take_last)
{
    uint32_t critical = osiEnterCritical();

    if (fifo->put_pending)
    {
        osiExitCritical(critical);
        return NULL;
    }

    osiBlockBuf_t *p = prvAllocBuf(fifo);
    if (p == NULL && take_last)
    {
        p = TAILQ_LAST(&fifo->ready_list, osiBlockBufHead);
        if (fifo->get_pending && p == TAILQ_FIRST(&fifo->ready_list))
        {
            osiExitCritical(critical);
            return NULL;
        }

        // Drop the last, and reallocate
        TAILQ_REMOVE(&fifo->ready_list, p, iter);
        TAILQ_INSERT_HEAD(&fifo->avail_list, p, iter);
        fifo->wpos -= p->size;

        p = prvAllocBuf(fifo);
    }

    void *data = NULL;
    if (p != NULL)
    {
        TAILQ_REMOVE(&fifo->avail_list, p, iter);
        TAILQ_INSERT_TAIL(&fifo->ready_list, p, iter);

        fifo->put_pending = true;
        data = p->data;
    }
    osiExitCritical(critical);
    return data;
}

bool osiBlockedFifoPutDone(osiBlockedFifo_t *fifo, unsigned size)
{
    uint32_t critical = osiEnterCritical();

    if (!fifo->put_pending)
    {
        osiExitCritical(critical);
        return false;
    }

    osiBlockBuf_t *p = TAILQ_LAST(&fifo->ready_list, osiBlockBufHead);
    if (p == NULL || size > p->size)
    {
        osiExitCritical(critical);
        return false;
    }

    fifo->put_pending = false;
    p->wpos = size;
    prvShrinkBuf(fifo, p);
    osiExitCritical(critical);
    return true;
}

const void *osiBlockedFifoGetRequest(osiBlockedFifo_t *fifo, unsigned *size)
{
    uint32_t critical = osiEnterCritical();

    const void *data = NULL;
    osiBlockBuf_t *p = TAILQ_FIRST(&fifo->ready_list);
    if (!fifo->get_pending && p != NULL)
    {
        fifo->get_pending = true;
        data = p->data + p->rpos;
        if (size != NULL)
            *size = p->wpos - p->rpos;
    }

    osiExitCritical(critical);
    return data;
}

bool osiBlockedFifoGetDone(osiBlockedFifo_t *fifo, unsigned size)
{
    uint32_t critical = osiEnterCritical();

    if (!fifo->get_pending)
    {
        osiExitCritical(critical);
        return false;
    }

    osiBlockBuf_t *p = TAILQ_FIRST(&fifo->ready_list);
    if (p == NULL || size > p->wpos - p->rpos)
    {
        osiExitCritical(critical);
        return false;
    }

    fifo->get_pending = false;
    p->rpos += size;
    if (p->rpos >= p->wpos)
    {
        TAILQ_REMOVE(&fifo->ready_list, p, iter);
        TAILQ_INSERT_HEAD(&fifo->avail_list, p, iter);
        fifo->rpos += p->size;
    }

    osiExitCritical(critical);
    return true;
}

bool osiBlockedFifoGetBlockDone(osiBlockedFifo_t *fifo)
{
    uint32_t critical = osiEnterCritical();

    if (!fifo->get_pending)
    {
        osiExitCritical(critical);
        return false;
    }

    osiBlockBuf_t *p = TAILQ_FIRST(&fifo->ready_list);
    if (p == NULL)
    {
        osiExitCritical(critical);
        return false;
    }

    fifo->get_pending = false;
    TAILQ_REMOVE(&fifo->ready_list, p, iter);
    TAILQ_INSERT_HEAD(&fifo->avail_list, p, iter);
    fifo->rpos += p->size;

    osiExitCritical(critical);
    return true;
}

void osiBlockedFifoDumpInfo(osiBlockedFifo_t *fifo)
{
    OSI_LOGI(0, "BFIFO(0x%08x) data/0x%08x size/0x%x block_size/0x%x alignment/%d rpos/0x%x wpos/0x%x",
             fifo, fifo->data, fifo->size, fifo->block_size,
             fifo->alignment, fifo->rpos, fifo->wpos);

    osiBlockBuf_t *p;
    unsigned n = 0;
    TAILQ_FOREACH(p, &fifo->ready_list, iter)
    {
        OSI_LOGI(0, "  #%d data/0x%08x size/%d rpos/%d wpos/%d",
                 n, p->data, p->size, p->rpos, p->wpos);
        n++;
    }
}
