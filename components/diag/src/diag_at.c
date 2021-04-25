/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "diag_at.h"
#include "diag.h"
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#include "cmddef.h"
#include "osi_api.h"
#include "osi_log.h"
#include "osi_fifo.h"
#include <stdlib.h>
#include <stdbool.h>

struct diag_at
{
    osiFifo_t rxfifo;
    diagAtCfg_t cfg;
};

void diagAtSendOk(diagAt_t *d)
{
    diagMsgHead_t head = {
        .seq_num = 0,
        .len = sizeof(diagMsgHead_t),
        .type = MSG_SEND_OK,
        .subtype = 0,
    };
    diagOutputPacket(&head, sizeof(head));
}

int diagAtWrite(diagAt_t *d, const void *data, size_t length)
{
    diagMsgHead_t head = {
        .seq_num = 0,
        .len = length + sizeof(diagMsgHead_t),
        .type = PPP_PACKET_A,
        .subtype = 0,
    };

    if (diagOutputPacket2(&head, data, length))
        return length;
    else
    {
        OSI_LOGE(0, "Diag AT write error");
        return -1;
    }
}

int diagAtRead(diagAt_t *d, void *buf, size_t size)
{
    if (d == NULL)
        return -1;
    return osiFifoGet(&d->rxfifo, buf, size);
}

static bool _handleAtCmd(const diagMsgHead_t *cmd, void *ctx)
{
    diagAt_t *d = (diagAt_t *)ctx;
    const void *data = diagCmdData(cmd);
    uint32_t size = diagCmdDataSize(cmd);
    int len = osiFifoPut(&d->rxfifo, data, size);
    if ((uint32_t)len != size)
        OSI_LOGE(0, "Diag AT rx overflow, got %u, store %d", size, len);

    if (d->cfg.notify)
        d->cfg.notify(d->cfg.context);

    return true;
}

bool diagAtOpen(diagAt_t *d)
{
    if (d == NULL)
        return false;
    diagRegisterCmdHandle(PPP_PACKET_R, _handleAtCmd, d);
    return true;
}

void diagAtClose(diagAt_t *d)
{
    diagUnregisterCmdHandle(PPP_PACKET_R);
}

diagAt_t *diagAtCreate(const diagAtCfg_t *cfg)
{
    if (cfg == NULL)
        return NULL;

    const unsigned alloc_size = cfg->rxfifo_size + sizeof(diagAt_t);
    diagAt_t *d = (diagAt_t *)malloc(alloc_size);
    if (d == NULL)
        return NULL;

    uint8_t *rxbuf = (uint8_t *)d + sizeof(diagAt_t);
    osiFifoInit(&d->rxfifo, rxbuf, cfg->rxfifo_size);
    d->cfg = *cfg;

    return d;
}

void diagAtDestroy(diagAt_t *d)
{
    diagAtClose(d);
    free(d);
}
