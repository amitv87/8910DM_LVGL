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

#include "osi_fifo.h"
#include "osi_api.h"
#include <string.h>
#include <stdlib.h>

bool osiFifoInit(osiFifo_t *fifo, void *data, size_t size)
{
    if (fifo == NULL || data == NULL || size == 0)
        return false;

    fifo->data = data;
    fifo->size = size;
    fifo->rd = 0;
    fifo->wr = 0;
    return true;
}

int osiFifoPut(osiFifo_t *fifo, const void *data, size_t size)
{
    if (fifo == NULL || data == NULL || size == 0)
        return 0;

    uint32_t cs = osiEnterCritical();
    size_t len = osiFifoSpace(fifo);
    if (len > size)
        len = size;

    size_t offset = fifo->wr % fifo->size;
    size_t tail = fifo->size - offset;
    if (tail >= len)
    {
        memcpy((char *)fifo->data + offset, data, len);
    }
    else
    {
        memcpy((char *)fifo->data + offset, data, tail);
        memcpy(fifo->data, (char *)data + tail, len - tail);
    }

    fifo->wr += len;
    osiExitCritical(cs);
    return len;
}

static int _peekInternal(osiFifo_t *fifo, void *data, size_t size)
{
    size_t len = osiFifoBytes(fifo);
    if (len > size)
        len = size;

    size_t offset = fifo->rd % fifo->size;
    size_t tail = fifo->size - offset;
    if (tail >= len)
    {
        memcpy(data, (char *)fifo->data + offset, len);
    }
    else
    {
        memcpy(data, (char *)fifo->data + offset, tail);
        memcpy((char *)data + tail, fifo->data, len - tail);
    }
    return len;
}

void osiFifoSkipBytes(osiFifo_t *fifo, size_t size)
{
    if (fifo == NULL || size == 0)
        return;

    uint32_t sc = osiEnterCritical();
    size_t len = osiFifoBytes(fifo);
    if (len > size)
        len = size;
    fifo->rd += len;
    osiExitCritical(sc);
}

int osiFifoGet(osiFifo_t *fifo, void *data, size_t size)
{
    if (fifo == NULL || data == NULL || size == 0)
        return 0;

    uint32_t cs = osiEnterCritical();
    int len = _peekInternal(fifo, data, size);
    fifo->rd += len;
    osiExitCritical(cs);
    return len;
}

int osiFifoPeek(osiFifo_t *fifo, void *data, size_t size)
{
    if (fifo == NULL || data == NULL || size == 0)
        return 0;

    uint32_t cs = osiEnterCritical();
    int len = _peekInternal(fifo, data, size);
    osiExitCritical(cs);
    return len;
}

void osiFifoReset(osiFifo_t *fifo)
{
    if (fifo != NULL)
    {
        uint32_t cs = osiEnterCritical();
        fifo->wr = fifo->rd = 0;
        osiExitCritical(cs);
    }
}

bool osiFifoSearch(osiFifo_t *fifo, uint8_t byte, bool keep)
{
    if (fifo == NULL)
        return false;

    uint8_t *p = (uint8_t *)fifo->data + (fifo->rd % fifo->size);
    uint8_t *pstart = (uint8_t *)fifo->data;
    uint8_t *pend = (uint8_t *)fifo->data + fifo->size;
    size_t wr = fifo->wr;
    for (int n = fifo->rd; n < wr; n++)
    {
        uint8_t ch = *p++;
        if (ch == byte)
        {
            fifo->rd = keep ? n : n + 1;
            return true;
        }
        if (p == pend)
            p = pstart;
    }
    fifo->rd = wr;
    return false;
}
