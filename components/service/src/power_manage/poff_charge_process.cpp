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

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "srv_pm.hpp"
#include "state_process.hpp"
#include "srv_config.h"
#include "srv_keypad.h"
#include "osi_api.h"
#include "osi_log.h"

namespace srvpm
{

void PoffChargeProcess::chargePlugInWork(bool on)
{
    if (!on)
    {
        pm.changePmState(PmState::POWER_OFF);
    }
}

void PoffChargeProcess::monitorBattery(uint16_t vol, bool on)
{
    OSI_LOGI(0, "monitor 1 battery %d/%u", on, vol);
}

void PoffChargeProcess::powerkeyStateChangeInWork(enum PowerkeyState state)
{
    if (state == PowerkeyState::LONG_PRESSED)
    {
        pm.changePmState(PmState::POWER_ON);
    }
}

} // namespace srvpm
