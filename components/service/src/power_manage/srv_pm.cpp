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
#include "srv_config.h"
#include "drv_charger.h"
#include "osi_log.h"
#include "srv_keypad.h"
#include "quec_cust_feature.h"
#include "quec_normalpowd.h"

namespace srvpm
{

static constexpr uint32_t kMonitorVbatInterval = 5000; // 5 seconds
#ifndef CONFIG_QUEC_PROJECT_FEATURE
static constexpr uint32_t kLongPressDuration = 2500;
#else
static constexpr uint32_t kLongPressDuration = 2500;   // 2.5 seconds       //420 = 0.42 seconds
#endif

struct EventNotify
{
    uint32_t name;
    ENFunc callback;
    void *caller_ctx;
    ENIter iter;
    EventNotify(uint32_t n, ENFunc f, void *p) : name(n), callback(f), caller_ctx(p) {}
};

SrvPm::SrvPm() : work_q_(nullptr), charge_plug_work_(nullptr),
                 monitor_vbat_work_(nullptr), mon_battery_timer_(nullptr),
                 powerkey_notify_work_(nullptr), powerkey_long_press_timer_(nullptr),
                 event_lock_(nullptr)
{
    for (auto i = 0; i < STATE_RUN_COUNT; ++i)
        state_process_[i] = nullptr;

    for (auto i = 0; i < SRVPM_EVENT_COUNT; ++i)
        TAILQ_INIT(&event_list_[i]);
}

SrvPm::~SrvPm()
{
    for (auto i = 0; i < STATE_RUN_COUNT; ++i)
    {
        if (state_process_[i])
        {
            delete state_process_[i];
            state_process_[i] = nullptr;
        }
    }

    EventNotify *p, *tmp;
    for (auto i = 0; i < SRVPM_EVENT_COUNT; ++i)
    {
        TAILQ_FOREACH_SAFE(p, &event_list_[i], iter, tmp)
        {
            TAILQ_REMOVE(&event_list_[i], p, iter);
            delete p;
        }
    }

    srvKeypadAddHook(KEY_MAP_POWER, nullptr, nullptr, nullptr);
    drvChargerSetCB(nullptr, nullptr);

    if (powerkey_long_press_timer_)
        osiTimerDelete(powerkey_long_press_timer_);
    if (powerkey_notify_work_)
        osiWorkDelete(powerkey_notify_work_);
    if (event_lock_)
        osiMutexDelete(event_lock_);
    if (mon_battery_timer_)
        osiTimerDelete(mon_battery_timer_);
    if (monitor_vbat_work_)
        osiWorkDelete(monitor_vbat_work_);
    if (charge_plug_work_)
        osiWorkDelete(charge_plug_work_);
}

bool SrvPm::init(PmStartFunc func, void *arg)
{
    if (func == nullptr)
        return false;
    start_func_ = func;
    start_args_ = arg;

    work_q_ = osiWorkQueueCreate("srv_pm", 1, OSI_PRIORITY_NORMAL, 2048);
    if (work_q_ == nullptr)
        return false;

    charge_plug_work_ = osiWorkCreate(
        [](void *ctx) {
            auto pm = (SrvPm *)ctx;
            pm->chargePlugWork();
        },
        nullptr, this);
    if (charge_plug_work_ == nullptr)
        return false;

    monitor_vbat_work_ = osiWorkCreate(
        [](void *ctx) {
            auto pm = (SrvPm *)ctx;
            pm->monBatteryWork();
        },
        nullptr, this);
    if (monitor_vbat_work_ == nullptr)
        return false;

    mon_battery_timer_ = osiTimerCreateWork(monitor_vbat_work_, work_q_);
    if (mon_battery_timer_ == nullptr)
        return false;

    event_lock_ = osiMutexCreate();
    if (event_lock_ == nullptr)
        return false;

    powerkey_notify_work_ = osiWorkCreate(
        [](void *ctx) {
            auto p = (SrvPm *)ctx;
            p->notifyPowerkeyState();
        },
        nullptr, this);

    if (powerkey_notify_work_ == nullptr)
        return false;

    powerkey_long_press_timer_ = osiTimerCreate(nullptr,
                                                [](void *ctx) {
                                                    auto p = (SrvPm *)ctx;
                                                    p->longPressedTimer();
                                                },
                                                this);
    if (powerkey_long_press_timer_ == nullptr)
        return false;

    if (!srvKeypadAddHook(KEY_MAP_POWER,
                          [](keyMap_t id, keyState_t state, void *ctx) {
                              auto p = (SrvPm *)ctx;
                              p->powerkeyStateChange(state == KEY_STATE_PRESS);
                          },
                          this, &powerkey_pressed_))
        return false;

    powerkey_long_pressed_ = false;

    state_process_[PmState::POWER_ON] = new PonProcess(*this);
    if (state_process_[PmState::POWER_ON] == nullptr)
        return false;

    if (!state_process_[PmState::POWER_ON]->init())
        return false;

#ifdef CONFIG_SRV_POWER_OFF_CHARGE_SUPPORT
    state_process_[PmState::POWER_OFF_CHARGE] = new PoffChargeProcess(*this);
    if (state_process_[PmState::POWER_OFF_CHARGE] == nullptr)
        return false;
    if (!state_process_[PmState::POWER_OFF_CHARGE]->init())
        return false;
#endif

    return true;
}

void SrvPm::longPressedTimer()
{
    if (powerkey_pressed_)
    {
        powerkey_long_pressed_ = true;
        osiWorkEnqueueLast(powerkey_notify_work_, work_q_);
    }
}

void SrvPm::notifyPowerkeyState()
{
    auto state = PowerkeyState::RELEASE;
    if (powerkey_pressed_)
        state = powerkey_long_pressed_ ? PowerkeyState::LONG_PRESSED : PowerkeyState::PRESSED;
    state_process_[pm_state_]->powerkeyStateChangeInWork(state);
}

void SrvPm::powerkeyStateChange(bool pressed)
{
    OSI_LOGI(0, "power key state %d", pressed);
    if (powerkey_pressed_ == pressed)
        return;

    osiTimerStop(powerkey_long_press_timer_);
    powerkey_pressed_ = pressed;
    powerkey_long_pressed_ = false;
    if (pressed)
    {
        osiTimerStartRelaxed(powerkey_long_press_timer_, kLongPressDuration, OSI_WAIT_FOREVER);
    }
    osiWorkEnqueueLast(powerkey_notify_work_, work_q_);
}

void SrvPm::changePmState(enum PmState state)
{
    if (state == pm_state_)
        return;

    OSI_LOGI(0, "PM state change %d -> %d", pm_state_, state);
    pm_state_ = state;
    if (state == PmState::POWER_OFF)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        NormalShutdownProcess();
#else
        osiThreadSleep(100);
        osiShutdown(OSI_SHUTDOWN_POWER_OFF);
#endif
        return;
    }

    state_process_[state]->setState();
}

void SrvPm::runInitCheck()
{
    auto state = PmState::POWER_ON;
    charger_attched_ = drvChargerGetType() != DRV_CHARGER_TYPE_NONE;

#ifdef CONFIG_SRV_POWER_OFF_CHARGE_SUPPORT
    auto boot_cause = osiGetBootCauses();
    OSI_LOGI(0, "boot cause %x", boot_cause);
    if (!(boot_cause & OSI_BOOTCAUSE_PWRKEY) && (boot_cause & OSI_BOOTCAUSE_CHARGE))
    {
        if (charger_attched_)
        {
            state = PmState::POWER_OFF_CHARGE;
        }
    }
#endif

    if (state == PmState::POWER_ON)
    {
        auto vbat = drvChargerGetBatteryVoltage();
        OSI_LOGI(0, "pm start battery voltage %u", vbat);
        if (vbat < CONFIG_SRV_POWER_ON_VOLTAGE)
        {
            if (charger_attched_)
            {
#ifdef CONFIG_SRV_POWER_OFF_CHARGE_SUPPORT
                state = PmState::POWER_OFF_CHARGE;
#endif
            }
            else
            {
                state = PmState::POWER_OFF;
            }
        }
    }

    OSI_LOGI(0, "pm run state %d", state);
    pm_state_ = state;
    if (state == PmState::POWER_OFF)
    {
        osiThreadSleep(100);
        osiShutdown(OSI_SHUTDOWN_POWER_OFF);
        return;
    }

    auto charger_cb = [](void *ctx, bool plug) { ((SrvPm *)ctx)->chargePlugCallback(plug); };
    drvChargerSetCB(charger_cb, this);

    if (charger_attched_)
    {
        startbatteryMonitor();
    }

    state_process_[state]->setState();
}

void SrvPm::callEventNotify(EventId id)
{
    osiMutexLock(event_lock_);
    EventNotify *p;
    auto plist = &event_list_[id];
    TAILQ_FOREACH(p, plist, iter)
    {
        p->callback(id, p->name, p->caller_ctx);
    }
    osiMutexUnlock(event_lock_);
}

void SrvPm::startbatteryMonitor()
{
    osiTimerStartPeriodicRelaxed(mon_battery_timer_, kMonitorVbatInterval, OSI_WAIT_FOREVER);
}

void SrvPm::chargePlugWork()
{
    OSI_LOGI(0, "pm charge work %d", charger_attched_);
    if (!charger_attched_)
    {
        osiTimerStop(mon_battery_timer_);
    }
    else
    {
        startbatteryMonitor();
    }

    state_process_[pm_state_]->chargePlugInWork(charger_attched_);
}

void SrvPm::chargePlugCallback(bool plugged)
{
    OSI_LOGD(0, "pm charge plug cb %d/%d", plugged, charger_attched_);
    if (plugged == charger_attched_)
        return;
    charger_attched_ = plugged;
    osiWorkEnqueue(charge_plug_work_, work_q_);
}

void SrvPm::monBatteryWork()
{
    auto vbat = drvChargerGetBatteryVoltage();
    state_process_[pm_state_]->monitorBattery(vbat);
}

static EventNotify *FindEventNotifyByName(uint32_t name, ENHeader *head)
{
    EventNotify *p;
    TAILQ_FOREACH(p, head, iter)
    {
        if (p->name == name)
            return p;
    }
    return nullptr;
}

bool SrvPm::addEventNotify(EventId id, uint32_t name, ENFunc cb, void *ctx)
{
    if (cb == nullptr || id >= SRVPM_EVENT_COUNT)
        return false;

    osiMutexLock(event_lock_);
    bool result = false;
    auto plist = &event_list_[id];
    auto p = FindEventNotifyByName(name, plist);
    if (p == nullptr)
    {
        p = new EventNotify(name, cb, ctx);
        if (p)
        {
            TAILQ_INSERT_TAIL(plist, p, iter);
            result = true;
        }
    }
    osiMutexUnlock(event_lock_);

    if (!result)
    {
        OSI_LOGE(0, "%4c add event notify %p fail (already exist? cb/%p)",
                 name, cb, (p ? p->callback : NULL));
    }

    return result;
}

void SrvPm::removeEventNotify(EventId id, uint32_t name)
{
    if (id >= SRVPM_EVENT_COUNT)
        return;

    osiMutexLock(event_lock_);
    auto plist = &event_list_[id];
    auto p = FindEventNotifyByName(name, plist);
    if (p)
    {
        TAILQ_REMOVE(plist, p, iter);
        delete p;
    }
    osiMutexUnlock(event_lock_);
}

} // namespace srvpm
