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

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "osi_log.h"
#include "osi_api.h"
#include "osi_profile.h"
#include "osi_internal.h"
#include "osi_chip.h"
#include "osi_sysnv.h"
#include "osi_byte_buf.h"
#include "cmsis_core.h"
#include "hwregs.h"
#include "hal_chip.h"
#include "drv_rtc.h"
#include "srv_wdt.h"
#include <string.h>
#include <stdlib.h>
#include <sys/queue.h>

#include "quec_cust_patch.h"
#include "quec_common.h"

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
#include "quec_power_mg.h"
#include "ql_api_osi.h"

//extern void ql_data_lock_timer_start(int wakeup_source);
extern void ql_enter_sleep_delay_timer_start(void);
extern void ql_data_lock_not_allow_sleep(void);
extern bool ql_is_data_lock_allow_sleep(void);


extern ql_timer_t data_lock_timer;

#define osi_sleep_log(msg, ...)  custom_log("quec_power_mg", msg, ##__VA_ARGS__)
#endif

#define DEBUG_SUSPEND_MODE OSI_SUSPEND_PM1
#define SUSPEND_MIN_TIME (5)            // ms
#define SUSPEND_WDT_MARGIN_TIME (20000) // 20s
#define PSM_MIN_TIME (150)

#ifdef CONFIG_WDT_ENABLE
#define WDT_ENTER_DEEPSLEEP(reset_ms) srvWdtEnterDeepSleep(reset_ms)
#define WDT_EXIT_DEEPSLEEP() srvWdtExitDeepSleep()
#else
#define WDT_ENTER_DEEPSLEEP(reset_ms)
#define WDT_EXIT_DEEPSLEEP()
#endif

typedef TAILQ_ENTRY(osiPmSource) osiPmSourceIter_t;
typedef TAILQ_HEAD(osiPmSourceHead, osiPmSource) osiPmSourceHead_t;
struct osiPmSource
{
    osiPmSourceIter_t resume_iter;
    osiPmSourceIter_t state_iter;

    uint32_t tag;
    void *cb_ctx;
    bool active;
    osiPmSourceOps_t ops;
};

typedef SLIST_ENTRY(osiShutdownReg) osiShutdownRegIter_t;
typedef SLIST_HEAD(osiShutdownRegHead, osiShutdownReg) osiShutdownRegHead_t;
typedef struct osiShutdownReg
{
    osiShutdownRegIter_t iter;
    osiShutdownCallback_t cb;
    void *ctx;
} osiShutdownReg_t;

typedef struct
{
    bool started;
    osiBootMode_t boot_mode;
    osiPmSourceHead_t resume_list;
    osiPmSourceHead_t active_list;
    osiPmSourceHead_t inactive_list;
    osiPmSourceHead_t prepare_list;
    osiShutdownRegHead_t shutdown_reg_list;

    uint32_t boot_causes;
    uint32_t sleep32k_flags;
} osiPmContext_t;

static osiPmContext_t gOsiPmCtx;

static inline void prvCpuSleep(void)
{
#ifdef CONFIG_CPU_ARM
    osiChipLightSleepEnter();

    __DSB();
    __ISB();
    __WFI();
    osiChipLightSleepExit();
#else
    asm volatile(".align 4\n"
                 ".set noreorder\n"
                 "li $6, 1\n"
                 "sw $6, (%0)\n"
                 "lw $6, (%0)\n"
                 "nop\n"
                 ".set reorder\n"
                 :
                 : "r"(&(hwp_sysIrq->cpu_sleep))
                 : "$6");
#endif
}

static osiPmSource_t *prvFindSourceByTag(osiPmContext_t *d, uint32_t tag)
{
    osiPmSource_t *p;
    TAILQ_FOREACH(p, &d->active_list, state_iter)
    {
        if (p->tag == tag)
            return p;
    }
    TAILQ_FOREACH(p, &d->inactive_list, state_iter)
    {
        if (p->tag == tag)
            return p;
    }
    TAILQ_FOREACH(p, &d->prepare_list, state_iter)
    {
        if (p->tag == tag)
            return p;
    }
    return NULL;
}

void osiPmInit(void)
{
    osiPmContext_t *d = &gOsiPmCtx;
    d->started = false;
    TAILQ_INIT(&d->resume_list);
    TAILQ_INIT(&d->active_list);
    TAILQ_INIT(&d->inactive_list);
    TAILQ_INIT(&d->prepare_list);
    SLIST_INIT(&d->shutdown_reg_list);
}

void osiPmStart(void)
{
    osiPmContext_t *d = &gOsiPmCtx;
    osiChipPmReorder();
    d->started = true;
}

void osiPmStop(void)
{
    osiPmContext_t *d = &gOsiPmCtx;
    d->started = false;
}

osiPmSource_t *osiPmSourceCreate(uint32_t tag, const osiPmSourceOps_t *ops, void *ctx)
{
    uint32_t critical = osiEnterCritical();

    osiPmContext_t *d = &gOsiPmCtx;
    osiPmSource_t *p = prvFindSourceByTag(d, tag);
    if (p != NULL)
    {
        osiExitCritical(critical);
        return p;
    }

    p = (osiPmSource_t *)calloc(1, sizeof(osiPmSource_t));
    if (p == NULL)
    {
        osiExitCritical(critical);
        return NULL;
    }

    p->tag = tag;
    p->cb_ctx = ctx;
    p->active = false;
    if (ops != NULL)
        p->ops = *ops;

    TAILQ_INSERT_TAIL(&d->inactive_list, p, state_iter);
    if (p->ops.suspend != NULL || p->ops.resume != NULL)
        TAILQ_INSERT_TAIL(&d->resume_list, p, resume_iter);

    osiExitCritical(critical);
    return p;
}

void osiPmSourceDelete(osiPmSource_t *ps)
{
    if (ps == NULL)
        return;

    uint32_t critical = osiEnterCritical();

    osiPmContext_t *d = &gOsiPmCtx;
    if (!ps->active)
        TAILQ_REMOVE(&d->inactive_list, ps, state_iter);
    else if (ps->ops.prepare != NULL)
        TAILQ_REMOVE(&d->prepare_list, ps, state_iter);
    else
        TAILQ_REMOVE(&d->active_list, ps, state_iter);

    if (ps->ops.suspend != NULL || ps->ops.resume != NULL)
        TAILQ_REMOVE(&d->resume_list, ps, resume_iter);

    free(ps);
    osiExitCritical(critical);
}

void osiPmWakeLock(osiPmSource_t *ps)
{
    if (ps == NULL)
        return;

    uint32_t critical = osiEnterCritical();

    osiPmContext_t *d = &gOsiPmCtx;
    if (!ps->active)
    {
        TAILQ_REMOVE(&d->inactive_list, ps, state_iter);
        ps->active = true;
        if (ps->ops.prepare != NULL)
            TAILQ_INSERT_TAIL(&d->prepare_list, ps, state_iter);
        else
            TAILQ_INSERT_TAIL(&d->active_list, ps, state_iter);
    }

    osiExitCritical(critical);
}

void osiPmWakeUnlock(osiPmSource_t *ps)
{
    if (ps == NULL)
        return;

    uint32_t critical = osiEnterCritical();

    osiPmContext_t *d = &gOsiPmCtx;
    if (ps->active)
    {
        if (ps->ops.prepare != NULL)
            TAILQ_REMOVE(&d->prepare_list, ps, state_iter);
        else
            TAILQ_REMOVE(&d->active_list, ps, state_iter);
        ps->active = false;
        TAILQ_INSERT_TAIL(&d->inactive_list, ps, state_iter);
    }

    osiExitCritical(critical);
}

void osiPmResumeReorder(uint32_t tag_later, uint32_t tag_earlier)
{
    if (tag_later == tag_earlier)
        return;

    uint32_t critical = osiEnterCritical();

    osiPmContext_t *d = &gOsiPmCtx;
    osiPmSource_t *p_earlier = NULL;
    osiPmSource_t *p_later = NULL;
    osiPmSource_t *p;
    TAILQ_FOREACH(p, &d->resume_list, resume_iter)
    {
        if (p->tag == tag_earlier)
        {
            p_earlier = p;
            if (p_later == NULL)
            {
                osiExitCritical(critical);
                return;
            }

            TAILQ_REMOVE(&d->resume_list, p_earlier, resume_iter);
            TAILQ_INSERT_BEFORE(p_earlier, p_later, resume_iter);
            osiExitCritical(critical);
            return;
        }

        if (p->tag == tag_later)
            p_later = p;
    }

    osiExitCritical(critical);
    return;
}

void osiPmResumeFirst(uint32_t tag)
{
    uint32_t critical = osiEnterCritical();

    osiPmContext_t *d = &gOsiPmCtx;
    osiPmSource_t *p;
    TAILQ_FOREACH(p, &d->resume_list, resume_iter)
    {
        if (p->tag == tag)
        {
            TAILQ_REMOVE(&d->resume_list, p, resume_iter);
            TAILQ_INSERT_HEAD(&d->resume_list, p, resume_iter);
            break;
        }
    }

    osiExitCritical(critical);
    return;
}

int osiPmSourceDump(void *mem, unsigned size)
{
    uint32_t critical = osiEnterCritical();
    osiPmContext_t *d = &gOsiPmCtx;

    int prepare_count = 0;
    int active_count = 0;
    int inactive_count = 0;
    int resume_count = 0;

    osiPmSource_t *p;
    TAILQ_FOREACH(p, &d->prepare_list, state_iter)
    {
        prepare_count++;
    }
    TAILQ_FOREACH(p, &d->active_list, state_iter)
    {
        active_count++;
    }
    TAILQ_FOREACH(p, &d->inactive_list, state_iter)
    {
        inactive_count++;
    }
    TAILQ_FOREACH(p, &d->resume_list, resume_iter)
    {
        resume_count++;
    }

    int count = prepare_count + active_count + inactive_count + resume_count;
    int total = 4 + count * 4;
    if (mem == NULL)
    {
        osiExitCritical(critical);
        return total;
    }
    if (total > size)
    {
        osiExitCritical(critical);
        return -1;
    }

    uint8_t *pmem = (uint8_t *)mem;
    OSI_STRM_W8(pmem, prepare_count);
    OSI_STRM_W8(pmem, active_count);
    OSI_STRM_W8(pmem, inactive_count);
    OSI_STRM_W8(pmem, resume_count);

    TAILQ_FOREACH(p, &d->prepare_list, state_iter)
    {
        OSI_STRM_WLE32(pmem, p->tag);
    }
    TAILQ_FOREACH(p, &d->active_list, state_iter)
    {
        OSI_STRM_WLE32(pmem, p->tag);
    }
    TAILQ_FOREACH(p, &d->inactive_list, state_iter)
    {
        OSI_STRM_WLE32(pmem, p->tag);
    }
    TAILQ_FOREACH(p, &d->resume_list, resume_iter)
    {
        OSI_STRM_WLE32(pmem, p->tag);
    }

    osiExitCritical(critical);
    return total;
}

static bool prvSuspendPermitted(osiPmContext_t *d)
{
    if (!d->started)
        return false;

    if (!TAILQ_EMPTY(&d->active_list))
        return false;

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
    if (!osiIsSlowSysClkAllowed())
        return false;
#endif

    if (!osiChipSuspendPermitted())
        return false;

    bool prepare_ok = true;
    osiPmSource_t *p;
    TAILQ_FOREACH(p, &d->prepare_list, state_iter)
    {
        if (p->ops.prepare == NULL)
            osiPanic();

        if (!p->ops.prepare(p->cb_ctx))
        {
            OSI_LOGD(0, "prepare cb %4c failed", p->tag);
            prepare_ok = false;
            break;
        }
        OSI_LOGD(0, "prepare cb %4c ok", p->tag);
    }

    if (prepare_ok)
        return true;

    TAILQ_FOREACH_REVERSE_FROM(p, &d->prepare_list, osiPmSourceHead, state_iter)
    {
        if (p->ops.prepare_abort != NULL)
        {
            OSI_LOGD(0, "prepare abort cb %4c", p->tag);
            p->ops.prepare_abort(p->cb_ctx);
        }
    }
    return false;
}

static bool prv32KSleepPermitted(osiPmContext_t *d)
{

#ifndef CONFIG_QUEC_PROJECT_FEATURE_USB_SUSPEND
	return false;
#endif

    if (!d->started)
        return false;

    if (!TAILQ_EMPTY(&d->active_list))
        return false;

    if (!osiChipSuspendPermitted())
        return false;

    return true;
}

static void prvSuspend(osiPmContext_t *d, osiSuspendMode_t mode, int64_t sleep_ms)
{
    // when PSM sleep condition isn't satisfied, it will return harmless
    osiShutdown(OSI_SHUTDOWN_PSM_SLEEP);

    OSI_LOGD(0, "deep sleep mode/%d sleep/%u", mode, (unsigned)sleep_ms);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
	quec_enter_sleep_cb();
#endif

    osiPmSource_t *p;
    TAILQ_FOREACH_REVERSE(p, &d->resume_list, osiPmSourceHead, resume_iter)
    {
        if (p->ops.suspend != NULL)
        {
            OSI_LOGD(0, "suspend cb %4c", p->tag);
            p->ops.suspend(p->cb_ctx, mode);
        }
    }

    osiProfileEnter(PROFCODE_DEEP_SLEEP);
    WDT_ENTER_DEEPSLEEP(OSI_MIN(int64_t, osiCpDeepSleepTime(), sleep_ms) + SUSPEND_WDT_MARGIN_TIME);
    halSysWdtStop();

    osiChipSuspend(mode);

    uint32_t source = osiPmCpuSuspend(mode, sleep_ms);
    OSI_LOGI(0, "suspend resume source 0x%08x", source);

    osiChipResume(mode, source);

    halSysWdtStart();
    WDT_EXIT_DEEPSLEEP();
    osiProfileExit(PROFCODE_DEEP_SLEEP);

    TAILQ_FOREACH(p, &d->resume_list, resume_iter)
    {
        if (p->ops.resume != NULL)
        {
            OSI_LOGD(0, "resume cb %4c", p->tag);
            p->ops.resume(p->cb_ctx, mode, source);
        }
    }
    osiChipResumePost(mode, source);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
	//ql_data_lock_timer_start(source);
	ql_data_lock_not_allow_sleep();
	quec_exit_sleep_cb(source);
#endif

    osiTimerWakeupProcess();
}

static void prv32KSleep(osiPmContext_t *d, int64_t sleep_ms)
{
    OSI_LOGI(0, "32K sleep sleep/%u", (unsigned)sleep_ms);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
	quec_enter_sleep_cb();
#endif

    osiProfileEnter(PROFCODE_DEEP_SLEEP);
    halSysWdtStop();

    WDT_ENTER_DEEPSLEEP(OSI_MIN(int64_t, osiCpDeepSleepTime(), sleep_ms) + SUSPEND_WDT_MARGIN_TIME);
    uint32_t source = osiChip32KSleep(sleep_ms);
    OSI_LOGI(0, "suspend resume source 0x%08x", source);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
	//ql_data_lock_timer_start(source);
	ql_data_lock_not_allow_sleep();
	quec_exit_sleep_cb(source);
#endif

    halSysWdtStart();
    WDT_EXIT_DEEPSLEEP();
    osiProfileExit(PROFCODE_DEEP_SLEEP);
}

static void prvLightSleep(osiPmContext_t *d, uint32_t idle_tick)
{
    bool timer_moved = osiTimerLightSleep(idle_tick);

    osiProfileEnter(PROFCODE_LIGHT_SLEEP);
    prvCpuSleep();
    osiProfileExit(PROFCODE_LIGHT_SLEEP);

    if (timer_moved)
        osiTimerWakeupProcess();
}

static void prvSleepLocked(uint32_t idle_tick)
{
    osiPmContext_t *d = &gOsiPmCtx;
    osiSuspendMode_t mode = DEBUG_SUSPEND_MODE;

    if (osiIsSleepAbort())
        return;

    if (gOsiPmCtx.sleep32k_flags != 0)
    {
        if (prv32KSleepPermitted(d))
        {        
            int64_t deep_sleep_ms = osiTimerDeepSleepTime(idle_tick);
            if (deep_sleep_ms > SUSPEND_MIN_TIME)
            {
//add by sum.li 2021/2/25 after permitted to sleep,delay ATS24=delaytime enter sleep operation
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
            	if(FALSE == ql_is_data_lock_allow_sleep())
            	{
                    ql_enter_sleep_delay_timer_start();   
            		prvLightSleep(d, idle_tick);        
            		return;
            	}
#endif         
                prv32KSleep(d, deep_sleep_ms);
                return;
            }
        }
    }
    else
    {
        if (prvSuspendPermitted(d))
        {        
            int64_t deep_sleep_ms = osiTimerDeepSleepTime(idle_tick);
            if (deep_sleep_ms > SUSPEND_MIN_TIME)
            {
//add by sum.li 2021/2/25 
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
            	if(FALSE == ql_is_data_lock_allow_sleep())
            	{
                    ql_enter_sleep_delay_timer_start();   
            		prvLightSleep(d, idle_tick);        
            		return;
            	}
#endif            
                prvSuspend(d, mode, deep_sleep_ms);
                return;
            }
        }
    }

    prvLightSleep(d, idle_tick);
}

void osiPmSleep(uint32_t idle_tick)
{
    uint32_t critical = osiEnterCritical();
    prvSleepLocked(idle_tick);
    osiExitCritical(critical);
}

bool osiRegisterShutdownCallback(osiShutdownCallback_t cb, void *ctx)
{
    if (cb == NULL)
        return false;

    uint32_t critical = osiEnterCritical();
    osiPmContext_t *d = &gOsiPmCtx;

    osiShutdownReg_t *reg;
    SLIST_FOREACH(reg, &d->shutdown_reg_list, iter)
    {
        if (reg->cb == cb && reg->ctx == ctx)
        {
            osiExitCritical(critical);
            return false;
        }
    }

    reg = (osiShutdownReg_t *)malloc(sizeof(osiShutdownReg_t));
    if (reg == NULL)
    {
        osiExitCritical(critical);
        return false;
    }

    reg->cb = cb;
    reg->ctx = ctx;
    SLIST_INSERT_HEAD(&d->shutdown_reg_list, reg, iter);
    osiExitCritical(critical);
    return true;
}

bool osiUnregisterShutdownCallback(osiShutdownCallback_t cb, void *ctx)
{
    uint32_t critical = osiEnterCritical();
    osiPmContext_t *d = &gOsiPmCtx;

    osiShutdownReg_t *reg = SLIST_FIRST(&d->shutdown_reg_list);
    if (reg != NULL)
    {
        if (reg->cb == cb && reg->ctx == ctx)
        {
            SLIST_REMOVE_HEAD(&d->shutdown_reg_list, iter);
            osiExitCritical(critical);
            return true;
        }

        osiShutdownReg_t *preg = reg;
        reg = SLIST_NEXT(reg, iter);
        SLIST_FOREACH(reg, &d->shutdown_reg_list, iter)
        {
            if (reg->cb == cb && reg->ctx == ctx)
            {
                SLIST_REMOVE_AFTER(preg, iter);
                osiExitCritical(critical);
                return true;
            }
            preg = reg;
        }
    }

    osiExitCritical(critical);
    return false;
}

static void prvInvokeShutdownCb(osiPmContext_t *d, osiShutdownMode_t mode)
{
    osiShutdownReg_t *reg;
    SLIST_FOREACH(reg, &d->shutdown_reg_list, iter)
    {
        reg->cb(reg->ctx, mode);
    }
}

bool osiShutdown(osiShutdownMode_t mode)
{
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO
	if(mode == OSI_SHUTDOWN_FORCE_DOWNLOAD)
	{
	    /* Power Domain OFF */
	    halPmuSwitchPower(HAL_POWER_LCD, false, false);
	    halPmuSwitchPower(HAL_POWER_SD, false, false);
	    halPmuSwitchPower(HAL_POWER_SIM1, false, false);
	}
#endif

    if (mode == OSI_SHUTDOWN_PSM_SLEEP && !gSysnvPsmSleepEnabled)
        return false;

    uint32_t critical = osiEnterCritical();

    if (!osiChipShutdownModeSupported(mode))
    {
        osiExitCritical(critical);
        return false;
    }

    int64_t psm_wake_uptime = osiGetPsmWakeUpTime();
    if (mode == OSI_SHUTDOWN_PSM_SLEEP && psm_wake_uptime == INT64_MAX)
    {
        // Though it should be checked before calling this, check it again
        osiExitCritical(critical);
        return false;
    }

    int64_t wake_uptime = INT64_MAX;
    if (mode == OSI_SHUTDOWN_PSM_SLEEP || mode == OSI_SHUTDOWN_POWER_OFF)
    {
        int64_t rtc_epoch = drvRtcGetWakeupTime();
        if (rtc_epoch != INT64_MAX)
        {
            int64_t rtc_uptime = osiEpochToUpTime(rtc_epoch);
            wake_uptime = OSI_MIN(int64_t, wake_uptime, rtc_uptime);
        }
    }

    if (mode == OSI_SHUTDOWN_PSM_SLEEP)
    {
        wake_uptime = OSI_MIN(int64_t, wake_uptime, psm_wake_uptime);

        // timer shall only be considered in PSM sleep
        int64_t timer_uptime = osiTimerPsmWakeUpTime();
        wake_uptime = OSI_MIN(int64_t, wake_uptime, timer_uptime);
    }

    int64_t sleep_time = wake_uptime - osiUpTime();
    if (sleep_time < PSM_MIN_TIME)
    {
        osiExitCritical(critical);
        return false;
    }

    osiPsmSavePrepare(mode);

    osiPmContext_t *d = &gOsiPmCtx;
    prvInvokeShutdownCb(d, mode);

    osiPsmSave(mode);
    halShutdown(mode, wake_uptime);
}

uint32_t osiGetBootCauses(void)
{
    return gOsiPmCtx.boot_causes;
}

void osiSetBootCause(osiBootCause_t cause)
{
    gOsiPmCtx.boot_causes |= cause;
}

void osiClearBootCause(osiBootCause_t cause)
{
    gOsiPmCtx.boot_causes &= ~cause;
}

osiBootMode_t osiGetBootMode(void)
{
    return gOsiPmCtx.boot_mode;
}

void osiSetBootMode(osiBootMode_t mode)
{
    gOsiPmCtx.boot_mode = mode;
}

void osiSet32KSleepFlag(uint32_t flag)
{
    gOsiPmCtx.sleep32k_flags |= flag;
}

void osiClear32KSleepFlag(uint32_t flag)
{
    gOsiPmCtx.sleep32k_flags &= ~flag;
}

OSI_WEAK void osiChipResumePost(osiSuspendMode_t mode, uint32_t source)
{
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
void quec_sleep_tag_to_name(int tag, char *name)
{
	name[3] = (unsigned char)(tag >> 24);
	name[2] = (unsigned char)(tag >> 16);
	name[1] = (unsigned char)(tag >> 8);
	name[0] = (unsigned char)(tag >> 0);
	name[4] = '\0';
}

void quec_print_os_pm_state(void)   
{
	osiPmContext_t *d = &gOsiPmCtx;

	if(osiIsSleepAbort())
	{
		osi_sleep_log("there are tasks in ready state, not sleep");
	}
	else if(FALSE == ql_is_data_lock_allow_sleep())
	{
		osi_sleep_log("uart1 or usb wakeuped system in %d s", QUEC_DATA_LOCK_TIME);
	}
	else if (!TAILQ_EMPTY(&d->active_list))
	{
		osiPmSource_t *p;
		char name[4];
		
		TAILQ_FOREACH(p, &d->active_list, state_iter)
		{
			if (p->active == 1)
			{
				//open项目下，为兼容其他模块，tag为代码分配，非标准字符转数字，因此不能用sleep_tag_to_name
				if((p->tag & 0x00FFFFFF) == (DRV_NAME_OPEN_SLEEP_TAG & 0x00FFFFFF)) //open项目下，tag低24位给定 
				{
					pm_info_t pm_info;
					if(pmsoucre_get_list(p->tag, &pm_info, NULL) != 0)
					{
						osi_sleep_log("cannot find the source");
						continue;
					}
					else
					{
						osi_sleep_log("source %s not allow sleep", pm_info.name);
					}					
				}
				else
				{
					quec_sleep_tag_to_name(p->tag, name);
					osi_sleep_log("source %s not allow sleep", name);
				}
			}
		}
	}
	else if (!osiChipSuspendPermitted())
	{
		osi_sleep_log("wake tick not larger than current tick than 10 ms");
	}
}
#endif
