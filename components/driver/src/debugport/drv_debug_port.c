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

#include "drv_debug_port.h"
#include "drv_debug_port_imp.h"

// Host packet for event 0x9db00000
const uint8_t gBlueScreenEventData[GDB_EVENT_DATA_SIZE] = {0xAD, 0x00, 0x06, 0xFF, 0x00, 0x00, 0x00, 0xB0, 0x9D, 0xD2};

static SLIST_HEAD(, drvDebugPort) gDebugPortList = SLIST_HEAD_INITIALIZER(&gDebugPortList);

void drvDebugPortRegister(drvDebugPort_t *d)
{
    SLIST_INSERT_HEAD(&gDebugPortList, d, iter);
}

drvDebugPort_t *drvDebugPortFindByName(uint32_t name)
{
    drvDebugPort_t *d;
    SLIST_FOREACH(d, &gDebugPortList, iter)
    {
        if (d->name == name)
            return d;
    }
    return NULL;
}

bool drvDebugPortSendPacket(drvDebugPort_t *d, const void *data, unsigned size)
{
    if (d != NULL && d->ops->send_packet != NULL)
        return d->ops->send_packet(d, data, size);
    return false;
}

void drvDebugPortSetRxCallback(struct drvDebugPort *d, osiCallback_t cb, void *param)
{
    if (d != NULL && d->ops->set_rx_callback != NULL)
        d->ops->set_rx_callback(d, cb, param);
}

void drvDebugPortSetTraceEnable(drvDebugPort_t *d, bool enable)
{
    if (d != NULL && d->ops->set_trace_enable != NULL)
        d->ops->set_trace_enable(d, enable);
}

int drvDebugPortRead(struct drvDebugPort *d, void *data, unsigned size)
{
    if (d != NULL && d->ops->read != NULL)
        return d->ops->read(d, data, size);
    return -1;
}
