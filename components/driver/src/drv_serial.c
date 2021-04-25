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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('D', 'S', 'R', 'L')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE

#include "drv_serial_imp.h"
#include "drv_names.h"
#include "drv_serial.h"

#include <string.h>
#include <stdlib.h>
#include "osi_api.h"
#include "osi_log.h"
#include "osi_compiler.h"

struct drv_serial
{
    drvSerialInfo_t info;
    drvSerialImpl_t *port;
    bool occupied;
    bool open;
};

#define CHECK_READY_OPS(s, op) (s && s->occupied && s->open && s->port && s->port->ops.op)
// #define CHECK_REG_PORT_CRITICAL_OPS

static const uint32_t kSerialNames[] = {
    DRV_NAME_USRL_COM0,
    DRV_NAME_USRL_COM5,
    DRV_NAME_USRL_COM6,
    DRV_NAME_USRL_COM7,
    DRV_NAME_ACM0,
    DRV_NAME_ACM1,
    DRV_NAME_ACM2,
    DRV_NAME_ACM3,
};

#define SERIAL_COUNT OSI_ARRAY_SIZE(kSerialNames)
static drvSerial_t gSerials[SERIAL_COUNT] = {};

static drvSerial_t *prvGetSerialInit(uint32_t name);
static drvSerial_t *(*prvGetSerial)(uint32_t) = prvGetSerialInit;

static inline drvSerial_t *prvGetSerial_(uint32_t name)
{
    for (unsigned i = 0; i < SERIAL_COUNT; ++i)
    {
        if (gSerials[i].info.name == name)
            return &gSerials[i];
    }
    return NULL;
}

static drvSerial_t *prvGetSerialInit(uint32_t name)
{
    drvSerial_t *s = NULL;
    for (unsigned i = 0; i < SERIAL_COUNT; ++i)
    {
        gSerials[i].info.name = kSerialNames[i];
        if (gSerials[i].info.name == name)
            s = &gSerials[i];
    }
    prvGetSerial = prvGetSerial_;
    return s;
}

drvSerial_t *drvSerialAcquire(uint32_t name, const drvSerialCfg_t *cfg)
{
    drvSerial_t *s = prvGetSerial(name);
    if (!s)
        return NULL;

    if (s->occupied)
    {
        OSI_LOGW(0, "Serial %4c acquire, find but in use.", name);
        return NULL;
    }

    s->info.cfg = *cfg;
    s->occupied = true;

    return s;
}

void drvSerialRelease(drvSerial_t *s)
{
    if (!s || !s->occupied)
        return;
    drvSerialClose(s);
    s->occupied = false;
    memset(&s->info.cfg, 0, sizeof(s->info.cfg));
}

bool drvSerialOpen(drvSerial_t *s)
{
    if (!s || !s->occupied)
        return false;

    if (s->open)
        return true;

    if (s->port)
    {
        int r = s->port->ops.open(s->port);
        if (r < 0)
        {
            OSI_LOGE(0, "serial %4c call open fail", s->info.name);
            return false;
        }
        s->open = true;
    }
    else
    {
        OSI_LOGW(0, "serial %4c open no instance", s->info.name);
        // set OPEN state and the port will call open on an implementation
        // register to this port
        s->open = true;
    }

    return s->open;
}

void drvSerialClose(drvSerial_t *s)
{
    if (!s || !s->occupied || !s->open)
        return;

    s->open = false;
    if (s->port)
        s->port->ops.close(s->port);
}

int drvSerialWrite(drvSerial_t *s, const void *data, size_t size)
{
    if (CHECK_READY_OPS(s, write))
        return s->port->ops.write(s->port, data, size);
    return -1;
}

bool drvSerialWriteDirect(drvSerial_t *s, const void *data, size_t size, size_t to_ms, uint32_t flags)
{
    if (CHECK_READY_OPS(s, write_direct))
        return s->port->ops.write_direct(s->port, data, size, to_ms, flags);
    return false;
}

int drvSerialRead(drvSerial_t *s, void *buffer, size_t size)
{
    if (CHECK_READY_OPS(s, read))
        return s->port->ops.read(s->port, buffer, size);
    return -1;
}

int drvSerialReadAvail(drvSerial_t *s)
{
    if (CHECK_READY_OPS(s, read_avail))
        return s->port->ops.read_avail(s->port);
    return 0;
}

int drvSerialWriteAvail(drvSerial_t *s)
{
    if (CHECK_READY_OPS(s, write_avail))
        return s->port->ops.write_avail(s->port);
    return 0;
}

bool drvSerialWaitWriteAvail(drvSerial_t *s, unsigned timeout)
{
    if (CHECK_READY_OPS(s, wait_write_avail))
        return s->port->ops.wait_write_avail(s->port, timeout);
    return false;
}

bool drvSerialWaitReadAvaid(drvSerial_t *s, unsigned timeout)
{
    if (CHECK_READY_OPS(s, wait_read_avail))
        return s->port->ops.wait_read_avail(s->port, timeout);
    return false;
}

bool drvSerialWaitWriteFinish(drvSerial_t *s, unsigned timeout)
{
    if (CHECK_READY_OPS(s, wait_write_finish))
        return s->port->ops.wait_write_finish(s->port, timeout);
    return false;
}

bool drvSerialCheckReady(drvSerial_t *s)
{
    if (!s || !s->occupied || !s->port)
        return false;
    return s->port->ops.ready(s->port);
}

bool drvSerialCheckReadyByName(uint32_t name)
{
    drvSerial_t *s = prvGetSerial(name);
    return drvSerialCheckReady(s);
}

bool drvSerialRegisterImpl(uint32_t name, drvSerialImpl_t *port)
{
    drvSerial_t *s = prvGetSerial(name);
    if (!s)
        return false;

    if (s->port == port)
        return true;

    if (s->port)
    {
        OSI_LOGE(0, "serial %4c already registered. (current port %p)", name, s->port);
        return false;
    }

#ifdef CHECK_REG_PORT_CRITICAL_OPS
    OSI_ASSERT(port->ops.open, "port check open");
    OSI_ASSERT(port->ops.close, "port check close");
    OSI_ASSERT(port->ops.ready, "port check ready");
#endif

    uint32_t sc = osiEnterCritical();
    s->port = port;
    port->info = &s->info;
    osiExitCritical(sc);

    // recall open on port registered
    if (s->open)
        port->ops.open(port);

    return true;
}

void drvSerialUnregisterImpl(drvSerialImpl_t *port)
{
    if (!port || !port->info)
        return;

    drvSerial_t *s = prvGetSerial(port->info->name);
    if (s)
    {
        OSI_ASSERT(s->port == port, "serial unregister sanity check fail");

        // call close to release resources, but not change serial state
        if (s->open)
            port->ops.close(port);

        uint32_t sc = osiEnterCritical();
        port->info = NULL;
        s->port = NULL;
        osiExitCritical(sc);
    }
}
