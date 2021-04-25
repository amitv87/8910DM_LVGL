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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('G', 'U', 'D', 'C')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include <osi_log.h>
#include <osi_api.h>
#include <usb/usb_device.h>
#include "drv_config.h"
#include "drv_names.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "udc_platform_rda.h"
#include "udc_dwc.h"

#include "usb_utils.h"

typedef struct
{
    udc_t udc;
    udevDrv_t *driver;
    void *notify_param;
    udcNotifier_t notifier;
} udcPriv_t;

char gDummyUsbRxBuf[USB_DUMMY_RX_BUF_SIZE] OSI_CACHE_LINE_ALIGNED;

static inline udcPriv_t *_getPriv(udc_t *udc)
{
    return (udcPriv_t *)udc;
}

udc_t *udcCreate(uint32_t name)
{
    udcPriv_t *p = calloc(1, sizeof(udcPriv_t));
    if (p == NULL)
        return NULL;

    p->udc.feature = CONFIG_USB_DEVICE_CONTROLLER_FEATURE;
    // platform
    if (!udcPlatInit_rda(&p->udc))
    {
        OSI_LOGE(0, "udc platform init failed");
        free(p);
        return NULL;
    }

    // controller
    dwcUdcInit(&p->udc);

    return &p->udc;
}

void udcDestroy(udc_t *udc)
{
    if (udc == NULL)
        return;

    udcStop(udc);
    dwcUdcExit(udc);
    udcPlatExit_rda(udc);

    free(udc);
}

udevDrv_t *udcGetDriver(udc_t *udc)
{
    if (!udc)
        return NULL;

    udcPriv_t *p = _getPriv(udc);
    return p->driver;
}

void udcBindDriver(udc_t *udc, udevDrv_t *driver)
{
    if (udc == NULL || driver == NULL)
        return;

    udcPriv_t *p = _getPriv(udc);
    p->driver = driver;
    driver->udc = udc;

    int r = udevDriverBind(driver);
    if (r < 0)
        OSI_LOGE(0, "UDC bind driver fail");
}

void udcStart(udc_t *udc)
{
    if (!udc)
        return;

    if (CHECK_UDC_OPS(udc, start))
        return udc->ops.start(udc);

    udcPriv_t *p = _getPriv(udc);
    if (p->driver)
        udevDriverStart(p->driver);
}

void udcStop(udc_t *udc)
{
    if (!udc)
        return;

    udcPriv_t *p = _getPriv(udc);
    if (p->driver)
    {
        udevDriverDisconnect(p->driver);
        udevDriverStop(p->driver);
    }

    if (CHECK_UDC_OPS(udc, stop))
        return udc->ops.stop(udc);
}

void udcSetNotifier(udc_t *udc, udcNotifier_t n, void *param)
{
    if (!udc)
        return;

    uint32_t critical = osiEnterCritical();
    udcPriv_t *p = _getPriv(udc);
    p->notifier = n;
    p->notify_param = param;
    osiExitCritical(critical);
}

void udcConnect(udc_t *udc)
{
    if (!udc)
        return;

    udcPriv_t *p = _getPriv(udc);
    if (p->driver)
        udevDriverConnect(p->driver);

    if (p->notifier)
        p->notifier(p->notify_param, UDC_CONNECTED);
}

void udcDisconnect(udc_t *udc)
{
    if (!udc)
        return;

    udcPriv_t *p = _getPriv(udc);
    if (p->driver)
        udevDriverDisconnect(p->driver);

    if (p->notifier)
        p->notifier(p->notify_param, UDC_DISCONNECT);
}

bool udcRemoteWakeup(udc_t *udc)
{
    if (!udc || !CHECK_UDC_OPS(udc, remote_wakeup))
        return false;

    return udc->ops.remote_wakeup(udc);
}
