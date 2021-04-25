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

#ifndef _SRV_PM_HPP_
#define _SRV_PM_HPP_

#include "srv_power_manage.h"
#include "state_process.hpp"
#include <stdint.h>
#include <sys/queue.h>
#include "osi_api.h"

namespace srvpm
{

using PmStartFunc = srvPmPowerOn_t;
using EventId = srvPmEvent_t;
using ENFunc = srvPmEventNotify_t;

struct EventNotify;
using ENIter = TAILQ_ENTRY(EventNotify);
using ENHeader = TAILQ_HEAD(ENHeader_, EventNotify);

enum PmState
{
    POWER_OFF = -1,
    POWER_ON = 0,
    POWER_OFF_CHARGE,
    STATE_RUN_COUNT, // software keep running state count
};

class SrvPm final
{
    friend struct PonProcess;
    friend struct PoffChargeProcess;

  private:
    SrvPm(SrvPm &) = delete;

    osiWorkQueue_t *work_q_;
    osiWork_t *charge_plug_work_;
    osiWork_t *monitor_vbat_work_;
    osiTimer_t *mon_battery_timer_;
    osiWork_t *powerkey_notify_work_;
    osiTimer_t *powerkey_long_press_timer_;
    osiMutex_t *event_lock_;

    enum PmState pm_state_;
    bool charger_attched_;
    bool powerkey_pressed_;
    bool powerkey_long_pressed_;
    PmStartFunc start_func_;
    void *start_args_;
    ENHeader event_list_[SRVPM_EVENT_COUNT];
    StateProcess *state_process_[STATE_RUN_COUNT];

    void startbatteryMonitor();
    void callEventNotify(EventId id);
    void changePmState(enum PmState state);

  public:
    explicit SrvPm();
    ~SrvPm();

    bool init(PmStartFunc func, void *arg);
    void runInitCheck();
    bool addEventNotify(EventId id, uint32_t name, ENFunc cb, void *ctx);
    void removeEventNotify(EventId id, uint32_t name);

    void chargePlugWork();
    void chargePlugCallback(bool plug);
    void monBatteryWork();
    void powerkeyStateChange(bool pressed);
    void notifyPowerkeyState();
    void longPressedTimer();
};

} // namespace srvpm

#endif