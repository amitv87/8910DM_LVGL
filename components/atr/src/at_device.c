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

#include "at_engine_imp.h"

void atDeviceDelete(atDevice_t *th)
{
    if (th != NULL && th->destroy != NULL)
        th->destroy(th);
}

bool atDeviceOpen(atDevice_t *th)
{
    if (th->open != NULL)
    {
        return th->isReady = th->open(th);
    }
    return false;
}

void atDeviceClose(atDevice_t *th)
{
    if (th->close != NULL)
        th->close(th);
    th->isReady = false;
}

int atDeviceWrite(atDevice_t *th, const void *data, size_t size)
{
    if (th->write != NULL)
    {
        return th->write(th, data, size);
    }
    return 0;
}

int atDeviceRead(atDevice_t *th, void *data, size_t size)
{
    if (th->read != NULL)
        return th->read(th, data, size);
    return 0;
}

void atDeviceSetBaudrate(atDevice_t *th, size_t baud)
{
    if (th->set_baud != NULL)
        th->set_baud(th, baud);
}

void atDeviceSetFormat(atDevice_t *th, atDeviceFormat_t format, atDeviceParity_t parity)
{
    if (th->set_format != NULL)
        th->set_format(th, format, parity);
}

bool atDeviceSetFlowCtrl(atDevice_t *th, atDeviceRXFC_t rxfc, atDeviceTXFC_t txfc)
{
    if (th->set_flow_ctrl != NULL)
        return th->set_flow_ctrl(th, rxfc, txfc);
    return false;
}

void atDeviceSetAutoSleep(atDevice_t *th, int timeout)
{
    if (th->set_auto_sleep != NULL)
        th->set_auto_sleep(th, timeout);
}

int atDeviceReadAvail(atDevice_t *th)
{
    if (th->read_avail != NULL)
        return th->read_avail(th);
    return 0;
}

int atDeviceWriteAvail(atDevice_t *th)
{
    if (th->write_avail != NULL)
        return th->write_avail(th);
    return 0;
}
