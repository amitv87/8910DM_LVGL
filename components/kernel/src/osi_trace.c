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

#include "osi_trace.h"
#include "osi_api.h"
#include "osi_hdlc.h"
#include "drv_debug_port.h"
#include <string.h>
#include <sys/queue.h>
#include <assert.h>
#include <stdlib.h>

#define LOG_RAMCODE OSI_SECTION_LINE(.ramtext.osi_trace)

static_assert(OSI_IS_ALIGNED(CONFIG_KERNEL_TRACE_BUF_SIZE, CONFIG_CACHE_LINE_SIZE),
              "CONFIG_KERNEL_TRACE_BUF_SIZE must be cache line aligned");
static_assert(CONFIG_KERNEL_TRACE_BUF_COUNT >= 2,
              "CONFIG_KERNEL_TRACE_BUF_COUNT must be greater than or equal to 2");

#define TRACE_BUF_SIZE (CONFIG_KERNEL_TRACE_BUF_SIZE * CONFIG_KERNEL_TRACE_BUF_COUNT)
#define TRACE_PREBUF_AVAIL(tb) (CONFIG_KERNEL_TRACE_BUF_SIZE - tb->size)
#define TRACE_BUF_PREALLOC (0)
#define TRACE_BUF_MALLOC (1)
#define TRACE_BUF_EXTERNAL (2)

typedef TAILQ_ENTRY(osiTraceBuf) osiTraceBufIter_t;
typedef TAILQ_HEAD(osiTraceBufHead, osiTraceBuf) osiTraceBufHead_t;
typedef struct osiTraceBuf
{
    osiTraceBufIter_t iter;
    uint8_t *data;
    unsigned size;
    uint8_t type;
} osiTraceBuf_t;

typedef struct
{
    drvDebugPort_t *port;            // debug port
    unsigned rd_count;               // total read (get from) byte count
    unsigned wr_count;               // total write (put to) byte count
    osiTraceBuf_t *buf_xfer;         // buffer in transfer
    osiTraceBufHead_t work_buf_list; // buffers with (various) data
    osiTraceBufHead_t idle_buf_list; // prealloc buffers, not used
    osiTraceBufHead_t ext_buf_list;  // container for external data
    osiTraceBuf_t bufs[CONFIG_KERNEL_TRACE_BUF_COUNT + CONFIG_KERNEL_TRACE_EXTERNAL_BUF_COUNT];
} osiTraceContext_t;

#ifdef CONFIG_SOC_8955
static uint8_t gTraceBuf[TRACE_BUF_SIZE] OSI_ALIGNED(4) OSI_SECTION_SRAM_UC_BSS;
#elif defined(CONFIG_SOC_8909)
static uint8_t gTraceBuf[TRACE_BUF_SIZE] OSI_ALIGNED(4) OSI_SECTION_RAM_UC_BSS;
#else
static uint8_t gTraceBuf[TRACE_BUF_SIZE] OSI_CACHE_LINE_ALIGNED;
#endif
static osiTraceContext_t gTraceCtx;

/**
 * Initialize trace buffer
 */
void osiTraceBufInit(void)
{
    osiTraceContext_t *d = &gTraceCtx;

    TAILQ_INIT(&d->work_buf_list);
    TAILQ_INIT(&d->idle_buf_list);
    TAILQ_INIT(&d->ext_buf_list);
    d->rd_count = 0;
    d->wr_count = 0;

    for (int n = 0; n < CONFIG_KERNEL_TRACE_BUF_COUNT; n++)
    {
        d->bufs[n].data = &gTraceBuf[n * CONFIG_KERNEL_TRACE_BUF_SIZE];
        d->bufs[n].size = 0;
        d->bufs[n].type = TRACE_BUF_PREALLOC;
        TAILQ_INSERT_TAIL(&d->idle_buf_list, &d->bufs[n], iter);
    }
    for (int n = CONFIG_KERNEL_TRACE_BUF_COUNT;
         n < CONFIG_KERNEL_TRACE_BUF_COUNT + CONFIG_KERNEL_TRACE_EXTERNAL_BUF_COUNT;
         n++)
    {
        d->bufs[n].data = NULL;
        d->bufs[n].size = 0;
        d->bufs[n].type = TRACE_BUF_EXTERNAL;
        TAILQ_INSERT_TAIL(&d->ext_buf_list, &d->bufs[n], iter);
    }

    // enable trace after trace buffer are initialized
    gTraceEnabled = true;
}

/**
 * Put a trace packet into trace buffer
 */
LOG_RAMCODE bool osiTraceBufPut(const void *data, unsigned size)
{
#ifdef CONFIG_KERNEL_HOST_TRACE
    if (size == 0)
        return true;
    if (data == NULL || size > CONFIG_KERNEL_TRACE_BUF_SIZE)
        return false;

    osiTraceContext_t *d = &gTraceCtx;
    unsigned critical = osiEnterCritical();

    // Whether can ammend to last used buffer
    size = OSI_ALIGN_UP(size, 4);
    osiTraceBuf_t *tb = TAILQ_LAST(&d->work_buf_list, osiTraceBufHead);
    bool can_ammend = (tb != NULL && tb->type == TRACE_BUF_PREALLOC && TRACE_PREBUF_AVAIL(tb) >= size);
    if (!can_ammend)
    {
        tb = TAILQ_FIRST(&d->idle_buf_list);
        if (tb == NULL)
            goto failed_unlock;

        tb->size = 0;
        TAILQ_REMOVE(&d->idle_buf_list, tb, iter);
        TAILQ_INSERT_TAIL(&d->work_buf_list, tb, iter);
    }

    *(uint32_t *)(tb->data + tb->size + size - 4) = 0;
    memcpy(tb->data + tb->size, data, size);
    tb->size += size;
    d->wr_count += size;

    osiExitCritical(critical);
    return true;

failed_unlock:
    osiExitCritical(critical);
    return false;
#endif

#ifdef CONFIG_KERNEL_DIAG_TRACE
    if (size == 0)
        return true;
    if (data == NULL)
        return false;

    unsigned worst_size = size * 2 + 2;
    if (worst_size > CONFIG_KERNEL_TRACE_BUF_SIZE)
        return false;

    osiTraceContext_t *d = &gTraceCtx;
    unsigned critical = osiEnterCritical();

    // Whether can ammend to last used buffer
    osiTraceBuf_t *tb = TAILQ_LAST(&d->work_buf_list, osiTraceBufHead);
    bool can_ammend = (tb != NULL && tb->type == TRACE_BUF_PREALLOC && TRACE_PREBUF_AVAIL(tb) >= worst_size);
    if (!can_ammend)
    {
        tb = TAILQ_FIRST(&d->idle_buf_list);
        if (tb == NULL)
            goto failed_unlock;

        tb->size = 0;
        TAILQ_REMOVE(&d->idle_buf_list, tb, iter);
        TAILQ_INSERT_TAIL(&d->work_buf_list, tb, iter);
    }

    int enc_size = osiHdlcEncode(tb->data + tb->size, data, size);
    tb->size += enc_size;
    d->wr_count += enc_size;

    osiExitCritical(critical);
    return true;

failed_unlock:
    osiExitCritical(critical);
    return false;
#endif
}

/**
 * Put a trace packet into trace buffer
 */
LOG_RAMCODE bool osiTraceBufPutMulti(const osiBuffer_t *bufs, unsigned count, int size)
{
#ifdef CONFIG_KERNEL_HOST_TRACE
    if (size == 0)
        return true;
    if (bufs == NULL)
        return false;

    if (size < 0)
    {
        size = 0;
        for (unsigned n = 0; n < count; n++)
            size += bufs[n].size;

        if (size == 0)
            return true;
    }

    if (size > CONFIG_KERNEL_TRACE_BUF_SIZE)
        return false;

    osiTraceContext_t *d = &gTraceCtx;
    unsigned critical = osiEnterCritical();

    // Whether can ammend to last used buffer
    size = OSI_ALIGN_UP(size, 4);
    osiTraceBuf_t *tb = TAILQ_LAST(&d->work_buf_list, osiTraceBufHead);
    bool can_ammend = (tb != NULL && tb->type == TRACE_BUF_PREALLOC && TRACE_PREBUF_AVAIL(tb) >= size);
    if (!can_ammend)
    {
        tb = TAILQ_FIRST(&d->idle_buf_list);
        if (tb == NULL)
            goto failed_unlock;

        tb->size = 0;
        TAILQ_REMOVE(&d->idle_buf_list, tb, iter);
        TAILQ_INSERT_TAIL(&d->work_buf_list, tb, iter);
    }

    *(uint32_t *)(tb->data + tb->size + size - 4) = 0;
    unsigned pos = tb->size;
    tb->size += size;
    d->wr_count += size;

    for (unsigned n = 0; n < count; n++)
    {
        memcpy(tb->data + pos, (const void *)bufs[n].ptr, bufs[n].size);
        pos += bufs[n].size;
    }

    osiExitCritical(critical);
    return true;

failed_unlock:
    osiExitCritical(critical);
    return false;
#endif

#ifdef CONFIG_KERNEL_DIAG_TRACE
    if (size == 0)
        return true;
    if (bufs == NULL)
        return false;

    if (size < 0)
    {
        size = 0;
        for (unsigned n = 0; n < count; n++)
            size += bufs[n].size;

        if (size == 0)
            return true;
    }

    unsigned worst_size = size * 2 + 2;
    if (worst_size > CONFIG_KERNEL_TRACE_BUF_SIZE)
        return false;

    osiTraceContext_t *d = &gTraceCtx;
    unsigned critical = osiEnterCritical();

    // Whether can ammend to last used buffer
    osiTraceBuf_t *tb = TAILQ_LAST(&d->work_buf_list, osiTraceBufHead);
    bool can_ammend = (tb != NULL && tb->type == TRACE_BUF_PREALLOC && TRACE_PREBUF_AVAIL(tb) >= worst_size);
    if (!can_ammend)
    {
        tb = TAILQ_FIRST(&d->idle_buf_list);
        if (tb == NULL)
            goto failed_unlock;

        tb->size = 0;
        TAILQ_REMOVE(&d->idle_buf_list, tb, iter);
        TAILQ_INSERT_TAIL(&d->work_buf_list, tb, iter);
    }

    int enc_size = osiHdlcEncodeMulti(tb->data + tb->size, bufs, count);
    tb->size += enc_size;
    d->wr_count += enc_size;

    osiExitCritical(critical);
    return true;

failed_unlock:
    osiExitCritical(critical);
    return false;

#endif
}

/**
 * Put an external trace packet into trace buffer
 */
bool osiTraceBufPutPacket(const void *data, unsigned size)
{
    if (size == 0)
        return true;
    if (data == NULL)
        return false;

    osiTraceContext_t *d = &gTraceCtx;
    unsigned critical = osiEnterCritical();

    osiTraceBuf_t *tb = TAILQ_FIRST(&d->ext_buf_list);
    if (tb == NULL)
        goto failed_unlock;

    TAILQ_REMOVE(&d->ext_buf_list, tb, iter);
    tb->data = (uint8_t *)data;
    tb->size = size;
    tb->type = TRACE_BUF_EXTERNAL;
    TAILQ_INSERT_TAIL(&d->work_buf_list, tb, iter);

    d->wr_count += size;
    osiExitCritical(critical);
    return true;

failed_unlock:
    osiExitCritical(critical);
    return false;
}

/**
 * Whether the external trace packet is transfered
 */
bool osiTraceBufPacketFinished(const void *data)
{
    osiTraceContext_t *d = &gTraceCtx;
    unsigned critical = osiEnterCritical();

    osiTraceBuf_t *tb = d->buf_xfer;
    if (tb != NULL && tb->data == data)
        goto not_finish;

    TAILQ_FOREACH(tb, &d->work_buf_list, iter)
    {
        if (tb->data == data)
            goto not_finish;
    }
    osiExitCritical(critical);
    return true;

not_finish:
    osiExitCritical(critical);
    return false;
}

/**
 * Fetch a piece of trace buffer
 */
osiBuffer_t osiTraceBufFetch(void)
{
    osiTraceContext_t *d = &gTraceCtx;
    osiBuffer_t buf = {};

    unsigned critical = osiEnterCritical();
    if (d->buf_xfer != NULL)
        goto finish;

    osiTraceBuf_t *tb = TAILQ_FIRST(&d->work_buf_list);
    if (tb == NULL)
        goto finish;

    TAILQ_REMOVE(&d->work_buf_list, tb, iter);
    d->buf_xfer = tb;

    buf.ptr = (uintptr_t)tb->data;
    buf.size = tb->size;

finish:
    osiExitCritical(critical);
    return buf;
}

/**
 * Indicate trace buffer is handled
 */
void osiTraceBufHandled(void)
{
    osiTraceContext_t *d = &gTraceCtx;

    unsigned critical = osiEnterCritical();
    if (d->buf_xfer != NULL)
    {
        osiTraceBuf_t *tb = d->buf_xfer;
        d->buf_xfer = NULL;
        d->rd_count += tb->size;

        if (tb->type == TRACE_BUF_EXTERNAL)
        {
            TAILQ_INSERT_TAIL(&d->ext_buf_list, tb, iter);
        }
        else if (tb->type == TRACE_BUF_MALLOC)
        {
            free(tb->data);
            TAILQ_INSERT_TAIL(&d->ext_buf_list, tb, iter);
        }
        else
        {
            tb->size = 0;
            TAILQ_INSERT_TAIL(&d->idle_buf_list, tb, iter);
        }
    }
    osiExitCritical(critical);
}

/**
 * Get existed trace data size
 */
unsigned osiTraceDataSize(void)
{
    osiTraceContext_t *d = &gTraceCtx;
    return d->wr_count - d->rd_count;
}

/**
 * Set trace debug port
 */
void osiTraceSetDebugPort(drvDebugPort_t *port)
{
    osiTraceContext_t *d = &gTraceCtx;
    d->port = port;
}

/**
 * Set trace enable or not
 */
void osiTraceSetEnable(bool enable)
{
    osiTraceContext_t *d = &gTraceCtx;

    if (gTraceEnabled != enable)
    {
        gTraceEnabled = enable;
        drvDebugPortSetTraceEnable(d->port, enable); // NULL will check inside
    }
}
