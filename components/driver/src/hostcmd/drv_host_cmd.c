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

#include "drv_host_cmd.h"
#include "drv_host_cmd_imp.h"
#include "drv_config.h"
#include "osi_log.h"
#include "osi_trace.h"
#include "osi_byte_buf.h"
#include <stdlib.h>

#define PACKET_TIMEOUT (1000)

struct drvHostCmdEngine
{
    uint8_t packet[CONFIG_HOST_CMD_ENGINE_MAX_PACKET_SIZE];
    drvDebugPort_t *port;
    unsigned pos;
    osiElapsedTimer_t elapsed;
    drvHostCmdHandler_t handlers[256];
    osiWork_t *rx_work;
};

static uint8_t prvHostCrc(const uint8_t *p, unsigned size)
{
    uint8_t crc = 0;
    for (int n = 0; n < size; n++)
        crc ^= p[n];
    return crc;
}

static void prvHostCmdPushData(drvHostCmdEngine_t *d, const void *data, unsigned size)
{
    const uint8_t *data8 = (const uint8_t *)data;

    OSI_LOGD(0, "HOST push data size/%d pos/%d", size, d->pos);

    if (d->pos > 0 && osiElapsedTime(&d->elapsed) > PACKET_TIMEOUT)
    {
        OSI_LOGD(0, "HOST packet timeout pos/%d elapsed/%d", d->pos, osiElapsedTime(&d->elapsed));
        d->pos = 0;
    }

    uint16_t frame_len = (d->pos >= 4) ? osiBytesGetBe16(&d->packet[1]) : 0;
    for (unsigned n = 0; n < size; n++)
    {
        uint8_t c = *data8++;
        if (d->pos == 0)
        {
            if (c != 0xAD)
                continue;
            d->packet[d->pos++] = c;
            osiElapsedTimerStart(&d->elapsed);
        }
        else if (d->pos == 1 || d->pos == 2)
        {
            d->packet[d->pos++] = c;
        }
        else if (d->pos == 3)
        {
            d->packet[d->pos++] = c;
            frame_len = osiBytesGetBe16(&d->packet[1]);
            if (frame_len > CONFIG_HOST_CMD_ENGINE_MAX_PACKET_SIZE - 4)
                d->pos = 0;
        }
        else if (d->pos < frame_len + 4 - 1)
        {
            d->packet[d->pos++] = c;
        }
        else
        {
            d->packet[d->pos++] = c;
            uint8_t flowid = d->packet[3];

            OSI_LOGD(0, "HOST packet flowid/0x%02x framelen/%d", flowid, frame_len);
            if (c == prvHostCrc(&d->packet[3], frame_len))
            {
                drvHostCmdHandler_t handler = d->handlers[flowid];
                handler(d, d->packet, d->pos);
            }
            else
            {
                OSI_LOGD(0, "HOST packet CRC mismatch");
            }
            d->pos = 0;
        }
    }
}

static void prvHostCmdRxProcess(void *param)
{
    drvHostCmdEngine_t *d = (drvHostCmdEngine_t *)param;

    char buf[32];
    for (;;)
    {
        int rsize = drvDebugPortRead(d->port, buf, 32);
        if (rsize <= 0)
            break;

        prvHostCmdPushData(d, buf, rsize);
    }
}

static void prvHostCmdRxCallback(void *param)
{
    drvHostCmdEngine_t *d = (drvHostCmdEngine_t *)param;
    osiWorkEnqueue(d->rx_work, osiSysWorkQueueLowPriority());
}

static void prvHostCmdBsEnter(void *param)
{
    drvHostCmdEngine_t *d = (drvHostCmdEngine_t *)param;
    d->pos = 0;
}

drvHostCmdEngine_t *drvHostCmdEngineCreate(drvDebugPort_t *port)
{
    drvHostCmdEngine_t *d = calloc(1, sizeof(drvHostCmdEngine_t));
    if (d == NULL)
        return NULL;

    d->port = port;
    d->pos = 0;
    d->rx_work = osiWorkCreate(prvHostCmdRxProcess, NULL, d);
    osiElapsedTimerStart(&d->elapsed);
    for (int n = 0; n < 256; n++)
        d->handlers[n] = drvHostInvalidCmd;

    drvHostCmdRegisterHander(d, HOST_FLOWID_SYSCMD, drvHostSyscmdHandler);
    drvHostCmdRegisterHander(d, HOST_FLOWID_READWRITE2, drvHostReadWriteHandler);
    drvHostCmdRegisterHander(d, HOST_FLOWID_READWRITE, drvHostReadWriteHandler);

    drvDebugPortSetRxCallback(port, prvHostCmdRxCallback, d);
    osiRegisterBlueScreenHandler(prvHostCmdBsEnter, prvHostCmdRxProcess, d);
    return d;
}

void drvHostCmdRegisterHander(drvHostCmdEngine_t *d, uint8_t flow_id, drvHostCmdHandler_t handler)
{
    if (d == NULL)
        return;

    d->handlers[flow_id] = (handler == NULL) ? drvHostInvalidCmd : handler;
}

void drvHostCmdSendResponse(drvHostCmdEngine_t *d, uint8_t *packet, unsigned packet_len)
{
    if (d == NULL || packet == NULL || packet_len == 0)
        return;

    osiBytesPutBe16(packet + 1, packet_len - 4);
    packet[packet_len - 1] = prvHostCrc(&packet[3], packet_len - 4);

    drvDebugPortSendPacket(d->port, packet, packet_len);
}

void drvHostCmdSendResultCode(drvHostCmdEngine_t *d, uint8_t *packet, uint16_t error)
{
    if (d == NULL || packet == NULL)
        return;

    osiBytesPutLe16(packet + 5, error);
    drvHostCmdSendResponse(d, packet, 5 + 2 + 1);
}

void drvHostInvalidCmd(drvHostCmdEngine_t *cmd, uint8_t *packet, unsigned packet_len)
{
    osiHostPacketHeader_t *header = (osiHostPacketHeader_t *)packet;
    osiFillHostHeader(header, HOST_FLOWID_SYSCMD, 2);
    packet[4] = HOST_SYSCMD_INVALID;
    drvHostCmdSendResponse(cmd, packet, 6);
}
