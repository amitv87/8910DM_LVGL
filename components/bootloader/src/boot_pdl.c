/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "boot_pdl.h"
#include "osi_byte_buf.h"
#include <stdlib.h>

/* host define, for receiving packet */
#define HOST_PACKET_TAG 0xAE
#define HOST_PACKET_FLOWID 0xFF

/* client define, for sending packet */
#define PACKET_TAG 0xAE
#define FLOWID_DATA 0xBB
#define FLOWID_ACK 0xFF
#define FLOWID_ERROR 0xEE

typedef struct
{
    uint8_t tag;         // HOST_PACKET_TAG
    uint8_t pkt_size[4]; // packet size (LE) from the end of packet header
    uint8_t flowid;      // HOST_PACKET_FLOWID
    uint8_t res[2];      // for alignment
} pdlPacketHeader_t;

struct pdlEngine
{
    fdlChannel_t *ch;
    unsigned max_len;
};

static pdlEngine_t gPdlEngine;

static int prvPdlRead(pdlEngine_t *d, void *buf, unsigned size)
{
    int r = 0;
    while (r < size && d->ch->avail(d->ch) > 0)
    {
        int len = d->ch->read(d->ch, (char *)buf + r, size - r);
        if (len < 0)
            break;
        r += len;
    }
    return r;
}

static void prvPdlWriteAll(pdlEngine_t *d, const void *data, unsigned size)
{
    while (size > 0)
    {
        int wsize = d->ch->write(d->ch, data, size);
        size -= wsize;
        data = (const char *)data + wsize;
    }
}

static void prvPdlSendData(pdlEngine_t *d, const void *data, unsigned size, uint8_t flowid)
{
    pdlPacketHeader_t header;
    header.tag = PACKET_TAG;
    header.flowid = flowid;
    osiBytesPutLe32(&header.pkt_size[0], size);

    prvPdlWriteAll(d, &header, sizeof(header));
    prvPdlWriteAll(d, data, size);
}

void pdlEngineSendResp(pdlEngine_t *d, pdlRespType_t resp)
{
    unsigned vresp = resp;
    uint8_t flowid = (resp == PDL_RESP_ACK) ? FLOWID_ACK : FLOWID_ERROR;
    prvPdlSendData(d, &vresp, sizeof(int), flowid);
}

pdlEngine_t *pdlEngineCreate(fdlChannel_t *ch, unsigned max_len)
{
    pdlEngine_t *d = &gPdlEngine;
    d->ch = ch;
    d->max_len = max_len;
    return d;
}

void pdlEngineDelete(pdlEngine_t *d)
{
}

bool pdlEngineLoop(pdlEngine_t *d, pdlCmdProcess_t proc, pdlPolling_t polling, void *param)
{
    if (d == NULL || proc == NULL)
        return false;

    uint8_t *pkt = calloc(1, d->max_len + 32);
    if (pkt == NULL)
        return false;

    pdlPacketHeader_t *ph = (pdlPacketHeader_t *)pkt;
    uint8_t recv_buf[128];
    unsigned len = 0;
    unsigned pkt_size = 0;
    for (;;)
    {
        int r = prvPdlRead(d, recv_buf, sizeof(recv_buf));
        if (r <= 0 && polling != NULL)
        {
            if (!polling(d, param))
                break;
        }

        for (int i = 0; i < r; i++)
        {
            uint8_t b = recv_buf[i];
            if (len == 0 && b != HOST_PACKET_TAG)
                continue;

            pkt[len++] = b;
            if (len < sizeof(pdlPacketHeader_t))
            {
                ; // do nothing and wait packet header
            }
            else if (len == sizeof(pdlPacketHeader_t))
            {
                pkt_size = osiBytesGetLe32(&ph->pkt_size[0]);
                if (ph->flowid != HOST_PACKET_FLOWID ||
                    pkt_size > d->max_len ||
                    pkt_size < 4)
                    len = 0;
            }
            else if (len == sizeof(pdlPacketHeader_t) + pkt_size)
            {
                pdlCommand_t *cmd = (pdlCommand_t *)&ph[1];
                proc(d, param, cmd, pkt_size);

                len = 0;
                pkt_size = 0;
            }
        }
    }

    free(pkt);
    return true;
}
