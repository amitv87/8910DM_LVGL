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

#include "srv_pm.hpp"
#include "state_process.hpp"
#include "srv_config.h"
#include "osi_api.h"
#include "osi_log.h"

namespace srvpm
{

void PonProcess::setState()
{
    pm.start_func_(pm.start_args_);
}

void PonProcess::monitorBattery(uint16_t vol, bool on)
{
    OSI_LOGI(0, "monitor 0 battery %d/%u", on, vol);
    if (!on || vol < CONFIG_SRV_POWER_OFF_VOLTAGE)
    {
        OSI_LOGW(0, "bat power off threshold %u", CONFIG_SRV_POWER_OFF_VOLTAGE);
        pm.changePmState(PmState::POWER_OFF);
    }
}

void PonProcess::chargePlugInWork(bool on)
{
    OSI_LOGI(0, "pon charge state %d", on);
    pm.callEventNotify(SRVPM_EVENT_CHARGER_HOTPLUG);
}

void PonProcess::powerkeyStateChangeInWork(enum PowerkeyState state)
{
    if (long_pressed && state == PowerkeyState::RELEASE)
    {
        OSI_LOGI(0, "pon powerkey pressed long enough %d", pm.charger_attched_);
        auto nextstate = PmState::POWER_OFF;
#ifdef CONFIG_SRV_POWER_OFF_CHARGE_SUPPORT
        if (pm.charger_attched_)
            nextstate = PmState::POWER_OFF_CHARGE;
#endif
        pm.changePmState(nextstate);
    }
    long_pressed = (state == PowerkeyState::LONG_PRESSED);
}

} // namespace srvpm
