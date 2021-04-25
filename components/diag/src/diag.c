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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "diag_config.h"
#include "diag.h"
#include "diag_internal.h"
#include "drv_names.h"
#include "osi_api.h"
#include "osi_log.h"
#include "osi_sysnv.h"
#include "osi_hdlc.h"
#ifndef CONFIG_QUEC_PROJECT_FEATURE
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#endif
#include "cmddef.h"
#include <stdlib.h>
#include <string.h>

typedef struct diag_handle
{
    diagCmdHandle_t cb;
    void *ctx;
} diagHandlerCtx_t;

typedef struct diagContext
{
    diagHandlerCtx_t handlers[REQ_MAX_F];
    osiWork_t *rx_work;
    drvDebugPort_t *port;
    osiHdlcDecode_t hdlc;
    unsigned prebuf_size;
    void *dyn_mem;
    void *prebuf;
} diagContext_t;

static diagContext_t *gDiagCtx;

/**
 * Default handler for unknown command
 */
static bool prvDiagUnknownHandler(const diagMsgHead_t *cmd, void *ctx)
{
    diagBadCommand(cmd);
    return true;
}

bool diagRegisterCmdHandle(uint8_t type, diagCmdHandle_t handler, void *ctx)
{
    diagContext_t *d = gDiagCtx;
    if (type >= REQ_MAX_F)
        return false;

    d->handlers[type].cb = (handler == NULL) ? prvDiagUnknownHandler : handler;
    d->handlers[type].ctx = ctx;
    return true;
}

bool diagUnregisterCmdHandle(uint8_t type)
{
    diagContext_t *d = gDiagCtx;
    if (type >= REQ_MAX_F)
        return false;

    d->handlers[type].cb = prvDiagUnknownHandler;
    d->handlers[type].ctx = NULL;
    return true;
}

void diagBadCommand(const diagMsgHead_t *cmd)
{
    diagMsgHead_t head = *cmd;
    head.len = sizeof(diagMsgHead_t);
    diagOutputPacket(&head, sizeof(head));
}

static bool prvDiagSendMulti(const osiBuffer_t *bufs, unsigned count, unsigned size)
{
    diagContext_t *d = gDiagCtx;
    int enc_size = osiHdlcEncodeMultiLen(bufs, count);
    if (enc_size < 0)
        return false;

    void *enc_buf = malloc(enc_size);
    if (enc_buf != NULL)
    {
        enc_size = osiHdlcEncodeMulti(enc_buf, bufs, count);
        drvDebugPortSendPacket(d->port, enc_buf, enc_size);
        free(enc_buf);
    }
    return true;
}

bool diagOutputPacket(const void *data, unsigned size)
{
    if (data == NULL || size == 0)
        return false;

    osiBuffer_t bufs[1] = {{(uintptr_t)data, size}};
    return prvDiagSendMulti(bufs, 1, size);
}

bool diagOutputPacket2(const diagMsgHead_t *cmd, const void *data, unsigned size)
{
    if (cmd == NULL)
        return false;

    diagMsgHead_t head = *cmd;
    head.len = sizeof(diagMsgHead_t) + size;

    osiBuffer_t bufs[2] = {
        {(uintptr_t)&head, sizeof(diagMsgHead_t)},
        {(uintptr_t)data, size},
    };
    return prvDiagSendMulti(bufs, 2, head.len);
}

bool diagOutputPacket3(const diagMsgHead_t *cmd, const void *sub_header, unsigned sub_header_size,
                       const void *data, unsigned size)
{
    if (cmd == NULL)
        return false;

    diagMsgHead_t head = *cmd;
    head.len = sizeof(diagMsgHead_t) + sub_header_size + size;

    osiBuffer_t bufs[3] = {
        {(uintptr_t)&head, sizeof(diagMsgHead_t)},
        {(uintptr_t)sub_header, sub_header_size},
        {(uintptr_t)data, size},
    };
    return prvDiagSendMulti(bufs, 3, head.len);
}

static void prvResponseSendOk(const diagMsgHead_t *cmd)
{
    diagMsgHead_t head = {
        .seq_num = cmd->seq_num,
        .len = sizeof(diagMsgHead_t),
        .type = MSG_SEND_OK,
        .subtype = 0,
    };
    diagOutputPacket(&head, sizeof(head));
}

static void prvDiagHandle(diagContext_t *d, const diagMsgHead_t *cmd)
{
    uint8_t type = cmd->type;
    if (type < REQ_MAX_F)
    {
        diagHandlerCtx_t *hc = &d->handlers[type];
        OSI_LOGD(0, "diag cmd %d/%p", type, hc->cb);
        hc->cb(cmd, hc->ctx);
    }
    else
    {
        prvResponseSendOk(cmd);
    }
}

static void prvDiagDropDynMem(diagContext_t *d)
{
    free(d->dyn_mem);
    d->dyn_mem = NULL;
    osiHdlcDecodeChangeBuf(&d->hdlc, d->prebuf, d->prebuf_size);
}

static void prvDiagRxProcess(void *param)
{
    diagContext_t *d = (diagContext_t *)param;
    char buf[32];

    for (;;)
    {
        int size = drvDebugPortRead(d->port, buf, 32);
        if (size <= 0)
            break;

        char *pbuf = &buf[0];
        while (size > 0)
        {
            int pbytes = osiHdlcDecodePush(&d->hdlc, pbuf, size);
            size -= pbytes;
            pbuf += pbytes;

            osiHdlcDecodeState_t state = osiHdlcDecodeGetState(&d->hdlc);
            if (state == OSI_HDLC_DEC_ST_PACKET)
            {
                osiBuffer_t decpacket = osiHdlcDecodeFetchPacket(&d->hdlc);
                diagMsgHead_t *cmd = (diagMsgHead_t *)decpacket.ptr;
                prvDiagHandle(d, cmd);

                if (cmd == d->dyn_mem)
                    prvDiagDropDynMem(d);
            }
            else if (state == OSI_HDLC_DEC_ST_DIAG_TOO_LARGE)
            {
                osiBuffer_t decdata = osiHdlcDecodeGetData(&d->hdlc);
                diagMsgHead_t *cmd = (diagMsgHead_t *)decdata.ptr;
                d->dyn_mem = malloc(cmd->len);
                if (d->dyn_mem != NULL)
                    osiHdlcDecodeChangeBuf(&d->hdlc, d->dyn_mem, cmd->len);
                else
                    osiHdlcDecodeReset(&d->hdlc);
            }
            else if (state == OSI_HDLC_DEC_ST_OVERFLOW)
            {
                prvDiagDropDynMem(d);
                osiHdlcDecodeReset(&d->hdlc);
            }
        }
    }
}

static void _diagDataCB(void *param)
{
    diagContext_t *d = (diagContext_t *)param;
    osiWorkEnqueue(d->rx_work, osiSysWorkQueueLowPriority());
}

void diagInit(drvDebugPort_t *port)
{
    unsigned prebuf_size = (osiGetBootMode() == OSI_BOOTMODE_NORMAL) ? CONFIG_DIAG_NORMAL_MODE_BUF_SIZE : CONFIG_DIAG_CALIB_MODE_BUF_SIZE;
    uintptr_t mem = (uintptr_t)calloc(1, sizeof(diagContext_t) + prebuf_size);

    gDiagCtx = (diagContext_t *)OSI_PTR_INCR_POST(mem, sizeof(diagContext_t));
    gDiagCtx->prebuf = (void *)OSI_PTR_INCR_POST(mem, prebuf_size);
    gDiagCtx->prebuf_size = prebuf_size;
    diagContext_t *d = (diagContext_t *)gDiagCtx;

    for (unsigned n = 0; n < REQ_MAX_F; n++)
        d->handlers[n].cb = prvDiagUnknownHandler;

#if defined(CONFIG_SOC_8811)
#else
    Layer1_RegCaliDiagHandle();
    diagCurrentTestInit();
    diagSwverInit();
    diagLogInit();
    diagNvmInit();
    diagSysInit();
    diagPsInit();
    diagChargeInit();
#endif
    d->rx_work = osiWorkCreate(prvDiagRxProcess, NULL, d);
    d->port = port;
    drvDebugPortSetRxCallback(d->port, _diagDataCB, d);
    osiHdlcDecodeInit(&d->hdlc, d->prebuf, d->prebuf_size,
                      OSI_HDLC_DEC_CHECK_OVERFLOW | OSI_HDLC_DEC_CHECK_DIAG_TOO_LARGE);
    _diagDataCB(d);
}

unsigned diagDeviceName(void)
{
#ifdef CONFIG_DIAG_DEVICE_USRL_SUPPORT
    if (gSysnvDiagDevice == DIAG_DEVICE_USERIAL)
        return CONFIG_DIAG_DEFAULT_USERIAL;
#endif

    return CONFIG_DIAG_DEFAULT_UART;
}
