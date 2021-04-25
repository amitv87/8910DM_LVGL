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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('D', 'G', 'R', 'M')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "diag_runmode.h"
#include "diag.h"
#include "osi_api.h"
#ifndef CONFIG_QUEC_PROJECT_FEATURE
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#endif
#include "cmddef.h"
#include "drv_names.h"
#include "drv_charger.h"
#include "drv_serial.h"
#include "diag_config.h"
#include "drv_usb.h"
#include "osi_log.h"
#include "osi_sysnv.h"
#include "osi_hdlc.h"

#define CHECK_MODE_BUF_SIZE (24)
#define CHECK_MODE_USB_READY_TIMEOUT (500)
#define CHECK_MODE_COMMAND_TIMEOUT (200)

typedef struct
{
    diagRunMode_t mode;
    drvDebugPort_t *port;
    osiHdlcDecode_t hdlc;
    char decbuf[CHECK_MODE_BUF_SIZE];
} diagRunModeContext_t;

static void prvSendPacket(diagRunModeContext_t *d, const void *packet, unsigned size)
{
    char buf[CHECK_MODE_BUF_SIZE];
    int enc_size = osiHdlcEncodeLen(packet, size);
    if (enc_size > CHECK_MODE_BUF_SIZE)
        return;

    enc_size = osiHdlcEncode(buf, packet, size);
    OSI_LOGD(0, "diag run mode send: %d/%d", size, enc_size);
    drvDebugPortSendPacket(d->port, buf, enc_size);
}

static bool prvRxProcess(diagRunModeContext_t *d, unsigned timeout)
{
    uint8_t buf[32];
    osiElapsedTimer_t timer;
    osiElapsedTimerStart(&timer);

    while (osiElapsedTime(&timer) < timeout)
    {
        int bytes = drvDebugPortRead(d->port, buf, 32);
        while (bytes > 0)
        {
            OSI_LOGD(0, "diag run mode received: %d", bytes);
            int pbytes = osiHdlcDecodePush(&d->hdlc, buf, bytes);
            bytes -= pbytes;

            osiHdlcDecodeState_t state = osiHdlcDecodeGetState(&d->hdlc);
            if (state == OSI_HDLC_DEC_ST_PACKET)
            {
                osiBuffer_t decpacket = osiHdlcDecodeFetchPacket(&d->hdlc);
                diagMsgHead_t *cmd = (diagMsgHead_t *)decpacket.ptr;

                OSI_LOGD(0, "diag run mode cmd: %d 0x%02x 0x%02x", cmd->len, cmd->type, cmd->subtype);
                if (cmd->type == MSG_RUNMODE)
                {
                    uint8_t mode = DIAG_RM_NORMAL;
                    if ((cmd->subtype & 0xF0) == 0x80)
                        mode = cmd->subtype & 0x0F;

                    // CALIB MODE
                    if (cmd->subtype == 0x90)
                        mode = DIAG_RM_CALIB;
                    // BBAT MODE
                    if (cmd->subtype == 0x95)
                        mode = DIAG_RM_BBAT;

                    d->mode = mode;

                    if (drvDebugPortIsUsb(d->port))
                        prvSendPacket(d, cmd, 8);
                    return true;
                }
            }
        }
        osiThreadSleepUS(100);
    }
    return false;
}

static bool prvDeviceReady(diagRunModeContext_t *d, unsigned ms)
{
    if (!drvDebugPortIsUsb(d->port))
        return true;

#ifdef CONFIG_DIAG_DEVICE_USRL_SUPPORT
    if (gSysnvUsbDetMode == USB_DETMODE_CHARGER &&
        drvChargerGetType() == DRV_CHARGER_TYPE_NONE)
        return false;

    if (!drvUsbSetWorkMode(DRV_USB_NPI_SERIAL))
        return false;

    drvUsbEnable(0);

    osiElapsedTimer_t timeout;
    osiElapsedTimerStart(&timeout);
    while (osiElapsedTime(&timeout) < ms)
    {
        if (drvDebugPortIsUsbHostOpened(d->port))
            return true;

        osiThreadSleepUS(100);
    }

    drvUsbDisable();
#endif
    return false;
}

diagRunMode_t diagRunModeCheck(drvDebugPort_t *port)
{
    diagRunModeContext_t rc = {
        .mode = DIAG_RM_NORMAL,
        .port = port,
    };

    OSI_LOGI(0, "diag run mode check");
    osiHdlcDecodeInit(&rc.hdlc, rc.decbuf, CHECK_MODE_BUF_SIZE, 0);

    if (!prvDeviceReady(&rc, CHECK_MODE_USB_READY_TIMEOUT))
    {
        OSI_LOGI(0, "diag run mode: device not ready");
        return rc.mode;
    }

    if (!drvDebugPortIsUsb(rc.port))
    {
        // send ping message if use uart diag channel
        diagMsgHead_t h;
        h.seq_num = 0;
        h.len = 8;
        h.type = MSG_RUNMODE;
        h.subtype = 0x01;
        prvSendPacket(&rc, &h, 8);
    }

    prvRxProcess(&rc, CHECK_MODE_COMMAND_TIMEOUT);

#ifdef CONFIG_DIAG_DEFAULT_USERIAL
    if (drvDebugPortIsUsb(rc.port))
    {
        if (rc.mode != DIAG_RM_NORMAL)
        {
            drvUsbDisable();
#ifdef CONFIG_QUEC_PROJECT_FEATURE
//to fix the problem at factory line when used multi usb port with IQ-M Instrument
//USB进校准模式，需要模块控制USB断开再连接，由于pc的差异，有些不能识别出usb口断开再连接的过程，导致无法进入校准，增加延时
            osiThreadSleepUS(2000 * 1000);
#else
            osiThreadSleepUS(1000 * 1000);
#endif
            drvUsbEnable(0);
        }
        else
        {
            drvUsbDisable();
        }
    }
#endif
    return rc.mode;
}
