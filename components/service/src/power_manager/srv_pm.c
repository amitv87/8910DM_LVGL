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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('S', 'R', 'P', 'M')

#include "srv_pm.h"
#include "srv_keypad.h"
#include "osi_api.h"
#include "osi_log.h"
#include "srv_config.h"
#include "drv_charger.h"
#include <stdlib.h>
#include <sys/queue.h>
#include "quec_proj_config.h"
#include "quec_normalpowd.h"

typedef LIST_ENTRY(srv_pm_ev_token) srvPmEvIter_t;
typedef LIST_HEAD(srv_pm_ev_head, srv_pm_ev_token) srvPmEvHead_t;
struct srv_pm_ev_token
{
    srvPmEvIter_t iter;
    void *func;
    void *param;
};

typedef union
{
    struct
    {
#define PM_STATUS_INHERIT_MASK 0b11
        uint8_t charge_attached : 1;
        uint8_t pk_pressed : 1;
        // not inherit status
        uint8_t pk_long_pressed : 1;
        uint8_t pk_long_press_rel : 1;
        uint8_t __reserved : 4;
    };
    uint8_t v;
} srvPmStatus_t;

struct srv_pm
{
    osiWorkQueue_t *wq;
    osiWork_t *charge_plug_work;
    osiWork_t *pk_work;
    osiWork_t *mon_battery_work;
    osiTimer_t *mon_battery_timer;
    osiTimer_t *pk_timer;
    srvKpdLsn_t *pk_lsn;
    srvPmConfig_t config;
    enum pm_state pm_state;
    srvPmEvHead_t ev_head[SRVPM_EV_COUNT];
    srvPmStatus_t status[PM_RUN_COUNT];
    uint32_t vbat_total;
    uint16_t vbat_realtime[8];
    uint8_t vbat_idx;
    bool running;
};

static const uint32_t kMonitorBatteryInterval = 5000; // ms
#ifndef CONFIG_QUEC_PROJECT_FEATURE_PWK
static const uint32_t kKeyLongPressDuration = 2500;   // ms
#else
static const uint32_t kKeyLongPressDuration = 2500;   // ms
#endif

static void prvSetState(srvPm_t *p, enum pm_state ns);

static void prvChargeWork(void *param)
{
    srvPm_t *p = (srvPm_t *)param;
    const srvPmStatus_t *status = &p->status[p->pm_state];
    if (p->pm_state == PM_POWER_ON)
    {
        srvPmEvToken_t *t;
        LIST_FOREACH(t, &p->ev_head[SRVPM_EV_CHR_HOTPLUG], iter)
        {
            if (t->func)
                ((srvPmChargerNotfiy_t)(t->func))(status->charge_attached, t->param);
        }
    }
    else if (p->pm_state == PM_POWER_OFF_CHARGE)
    {
        if (!status->charge_attached)
        {
            prvSetState(p, PM_POWER_OFF);
        }
    }
    else
    {
        OSI_LOGE(0, "pm charge work invalid %d/%d", status->charge_attached, p->pm_state);
    }
}

static void prvPkTimer(void *param)
{
    srvPm_t *p = (srvPm_t *)param;
    srvPmStatus_t *status = &p->status[p->pm_state];
    if (status->pk_pressed)
    {
        status->pk_long_pressed = true;
        OSI_LOGI(0, "pk long pressed (%d) and release", kKeyLongPressDuration);
        osiWorkEnqueueLast(p->pk_work, p->wq);
    }
}

static void prvPkWork(void *param)
{
    srvPm_t *p = (srvPm_t *)param;
    const srvPmStatus_t *status = &p->status[p->pm_state];
    if (p->pm_state == PM_POWER_ON && status->pk_long_press_rel)
    {
        int ns = PM_POWER_OFF;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_PWK
        if (status->charge_attached)
            ns = PM_POWER_OFF_CHARGE;
#else
        if (status->charge_attached && p->config.poff_charge_enable)
        {
            ns = PM_POWER_OFF_CHARGE;
        }
#endif
        prvSetState(p, ns);
    }
    else if (p->pm_state == PM_POWER_OFF_CHARGE && status->pk_long_pressed)
    {
        prvSetState(p, PM_POWER_ON);
    }
}

static void prvPkChange(srvPm_t *p, bool pressed)
{
    osiTimerStop(p->pk_timer);
    srvPmStatus_t *status = &p->status[p->pm_state];
    status->pk_pressed = pressed;
    status->pk_long_press_rel = false;
    if (status->pk_long_pressed && !pressed)
        status->pk_long_press_rel = true;
    status->pk_long_pressed = false;

    OSI_LOGI(0, "power key state %d/%d/%d/%d",
             pressed, status->pk_pressed, status->pk_long_pressed, status->pk_long_press_rel);

    if (pressed)
        osiTimerStartRelaxed(p->pk_timer, kKeyLongPressDuration, OSI_WAIT_FOREVER);
    osiWorkEnqueueLast(p->pk_work, p->wq);
}

static void prvPkNotifyCB(keyMap_t id, keyState_t state, void *param)
{
    srvPm_t *p = (srvPm_t *)param;
    if (p->running)
    {
        prvPkChange(p, state == KEY_STATE_PRESS);
    }
}

static void prvMonBatteryWork(void *param)
{
    srvPm_t *p = (srvPm_t *)param;
    const uint16_t vol = drvChargerGetVbatRT();
    const uint8_t cnt = OSI_ARRAY_SIZE(p->vbat_realtime);
    const uint8_t idx = (p->vbat_idx++) % OSI_ARRAY_SIZE(p->vbat_realtime);
    p->vbat_total = p->vbat_total + vol - p->vbat_realtime[idx];
    p->vbat_realtime[idx] = vol;
    const uint32_t average = p->vbat_total / cnt;
    OSI_LOGI(0, "pm monitor battery %u/%u", vol, average);
    if (p->pm_state == PM_POWER_ON)
    {
#ifndef CONFIG_QUEC_PROJECT_FEATURE
        if (average < CONFIG_SRV_POWER_OFF_VOLTAGE)
        {
            OSI_LOGW(0, "pm vbat lower than threshold %u/%u", average, CONFIG_SRV_POWER_OFF_VOLTAGE);
            if (p->status[p->pm_state].charge_attached && p->config.poff_charge_enable)
                prvSetState(p, PM_POWER_OFF_CHARGE);
            else
                prvSetState(p, PM_POWER_OFF);
        }
#endif
    }
}

static void prvChargeIsrCB(void *param, bool attach)
{
    srvPm_t *p = (srvPm_t *)param;
    srvPmStatus_t *status = &p->status[p->pm_state];
    OSI_LOGI(0, "pm charge cb %d -> %d", status->charge_attached, attach);
    if (attach == status->charge_attached)
        return;
    status->charge_attached = attach;
    osiWorkEnqueue(p->charge_plug_work, p->wq);
}

static void prvSetState_(srvPm_t *p, enum pm_state ns, bool init)
{
    if (ns == p->pm_state && !init)
        return;

    OSI_LOGI(0, "pm state %d -> %d (%d)", p->pm_state, ns, init);
    if (ns == PM_POWER_OFF)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PWK
        NormalShutdownProcess(OSI_SHUTDOWN_POWER_OFF);
#else
        osiThreadSleep(100);
        osiShutdown(OSI_SHUTDOWN_POWER_OFF);
#endif
    }
    else
    {
        if (!init)
        {
            p->status[ns].v = p->status[p->pm_state].v & PM_STATUS_INHERIT_MASK;
            if (ns == PM_POWER_OFF_CHARGE)
            {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PWK
                NormalShutdownProcess(OSI_SHUTDOWN_RESET);
#else
                osiThreadSleep(100);
                osiShutdown(OSI_SHUTDOWN_RESET);
#endif
            }
        }

        p->pm_state = ns;
        if (ns == PM_POWER_ON)
        {
            p->config.pon_func(p->config.pon_args);
        }
    }
}

static void prvSetState(srvPm_t *p, enum pm_state ns)
{
    prvSetState_(p, ns, false);
}

srvPm_t *srvPmCreateInstance(const srvPmConfig_t *config)
{
    OSI_ASSERT(config->pon_func != NULL, "pon func non-null");
    srvPm_t *p = (srvPm_t *)calloc(1, sizeof(*p));
    if (p == NULL)
    {
        OSI_LOGD(0, "fail create srv pm (not enough memory)");
        return NULL;
    }

    bool result = false;
    do
    {
        p->wq = osiWorkQueueCreate("srv_pm", 1, OSI_PRIORITY_NORMAL, 2048);
        if (!p->wq)
            break;

        p->charge_plug_work = osiWorkCreate(prvChargeWork, NULL, p);
        if (!p->charge_plug_work)
            break;

        p->pk_work = osiWorkCreate(prvPkWork, NULL, p);
        if (!p->pk_work)
            break;

        p->mon_battery_work = osiWorkCreate(prvMonBatteryWork, NULL, p);
        if (!p->mon_battery_work)
            break;

        p->mon_battery_timer = osiTimerCreateWork(p->mon_battery_work, p->wq);
        if (!p->mon_battery_timer)
            break;

        p->pk_timer = osiTimerCreate(NULL, prvPkTimer, p);
        if (!p->pk_timer)
            break;

        p->pk_lsn = srvKpdListen(POWERKEY_KEYCODE, prvPkNotifyCB, p);
        if (p->pk_lsn == NULL)
            break;

        result = true;
    } while (0);

    if (!result)
    {
        srvKpdListenBreak(p->pk_lsn);
        osiTimerDelete(p->pk_timer);
        osiTimerDelete(p->mon_battery_timer);
        osiWorkDelete(p->mon_battery_work);
        osiWorkDelete(p->pk_work);
        osiWorkDelete(p->charge_plug_work);
        osiWorkQueueDelete(p->wq);
        free(p);
        return NULL;
    }

    for (unsigned i = 0; i < SRVPM_EV_COUNT; ++i)
        LIST_INIT(&p->ev_head[i]);

    p->config = *config;

    return p;
}

void srvPmRunInitCheck(srvPm_t *p)
{
    int state = PM_POWER_ON;
    srvPmStatus_t status = {.charge_attached = (drvChargerGetType() != DRV_CHARGER_TYPE_NONE)};
    if (p->config.poff_charge_enable)
    {
        const uint32_t boot_cause = osiGetBootCauses();
        const uint32_t pon_mask = OSI_BOOTCAUSE_PWRKEY | OSI_BOOTCAUSE_ALARM;
        OSI_LOGI(0, "boot cause %x", boot_cause);
        if ((boot_cause & OSI_BOOTCAUSE_CHARGE) && !(boot_cause & pon_mask))
        {
            if (status.charge_attached)
            {
                state = PM_POWER_OFF_CHARGE;
            }
        }
    }

    p->vbat_total = 0;
    for (unsigned i = 0; i < OSI_ARRAY_SIZE(p->vbat_realtime); ++i)
    {
        p->vbat_realtime[i] = drvChargerGetVbatRT();
        p->vbat_total += p->vbat_realtime[i];
    }
    const uint32_t vbat_average = p->vbat_total / OSI_ARRAY_SIZE(p->vbat_realtime);
    if (state == PM_POWER_ON)
    {
        OSI_LOGI(0, "pm start battery voltage %u", vbat_average);
#ifndef CONFIG_QUEC_PROJECT_FEATURE
        if (vbat_average < CONFIG_SRV_POWER_ON_VOLTAGE)
        {
            if (!status.charge_attached)
            {
                state = PM_POWER_OFF;
            }
            else if (p->config.poff_charge_enable)
            {
                state = PM_POWER_OFF_CHARGE;
            }
        }
#endif
    }

    OSI_LOGI(0, "pm start running state %d", state);
    status.pk_pressed = drvKeypadState(KEY_MAP_POWER);
    if (state != PM_POWER_OFF)
    {
        p->status[state].v = status.v;
        drvChargerSetCB(prvChargeIsrCB, p);
        osiTimerStartPeriodicRelaxed(p->mon_battery_timer, kMonitorBatteryInterval, OSI_WAIT_FOREVER);
    }

    prvSetState_(p, state, true);
    p->running = true;
}

srvPmEvToken_t *srvPmAddEventCB(srvPm_t *p, srvPmEv_t ev, void *func, void *ctx)
{
    if (ev >= SRVPM_EV_COUNT || p == NULL || func == NULL)
        return NULL;

    srvPmEvToken_t *t = (srvPmEvToken_t *)calloc(1, sizeof(*t));
    if (t)
    {
        t->func = func;
        t->param = ctx;
        LIST_INSERT_HEAD(&p->ev_head[ev], t, iter);
    }
    return t;
}

void srvPmRmEventCB(srvPm_t *p, srvPmEvToken_t *token)
{
    if (token != NULL)
    {
        LIST_REMOVE(token, iter);
        free(token);
    }
}
