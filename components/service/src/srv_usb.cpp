/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('S', 'U', 'S', 'B')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "srv_usb.h"
#include "srv_power_manage.h"
#include "drv_charger.h"
#include "drv_usb.h"
#include "osi_log.h"

static void _chargePlugNotify(srvPmEvent_t event, uint32_t name, void *ctx)
{
    OSI_LOGI(0, "usb charge state %d/%d", event, drvChargerGetType());
    drvUsbSetAttach(drvChargerGetType() != DRV_CHARGER_TYPE_NONE);
}

bool srvUsbInit()
{
    drvUsbInit();
    srvPmAddEventNotfiy(SRVPM_EVENT_CHARGER_HOTPLUG, OSI_MAKE_TAG('S', 'U', 'S', 'B'),
                        _chargePlugNotify, nullptr);
    drvUsbSetAttach(drvChargerGetType() != DRV_CHARGER_TYPE_NONE);
    return true;
}
