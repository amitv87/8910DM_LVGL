/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "err.h"
#include "sys.h"
#include "osi_log.h"
#include "lwipports.h"
#include "tcpip.h"
#include "at_response.h"
#include "at_engine.h"
#include "drv_rtc.h"
#include "drv_rng.h"

#define NET_ENGINE_EVENT_QUEUE_SIZE 32

extern osiThread_t *netThreadID;

uint32_t sys_get_srand()
{
    uint32_t srand = 0;
    bool success = true;
    success = drvRngGenerate((void *)&srand, 4);
    if (success == false || srand == 0)
    {
        srand = (uint32_t)drvRtcGetWakeupTime();
    }
    return srand;
}

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    if (NULL == sem)
        return ERR_ARG;

    sem->sem_id = (u32_t)osiSemaphoreCreate(16, count);
    sem->is_vail = 1; // TRUE;

    return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem)
{
    if (NULL == sem)
        return;

    osiSemaphoreDelete((osiSemaphore_t *)(sem->sem_id));
}

void sys_sem_signal(sys_sem_t *sem)
{
    if (NULL == sem)
        return;

    osiSemaphoreRelease((osiSemaphore_t *)(sem->sem_id));
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    if (NULL == sem)
        return 0;

    if (timeout == 0)
        timeout = -1U;

    return (u32_t)osiSemaphoreTryAcquire((osiSemaphore_t *)(sem->sem_id), timeout);
}

int sys_sem_valid(sys_sem_t *sem)
{
    if (NULL == sem)
        return 0;

    return sem->is_vail;
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
    sem->is_vail = 0;
}

#if !LWIP_COMPAT_MUTEX

err_t sys_mutex_new(sys_mutex_t *mutex)
{
    mutex->id = (u32_t)osiMutexCreate();
    return ERR_OK;
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
    osiMutexLock((osiMutex_t *)(mutex->id));
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
    osiMutexUnlock((osiMutex_t *)(mutex->id));
}

void sys_mutex_free(sys_mutex_t *mutex)
{
    osiMutexDelete((osiMutex_t *)(mutex->id));
}

#endif

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    osiMessageQueue_t *hQueue = NULL;
    hQueue = osiMessageQueueCreate(size, sizeof(void *));
    if (hQueue == NULL)
        return ERR_MEM;

    mbox->id = (u32_t)hQueue;
    mbox->is_vail = 1;
    return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    if (NULL == mbox)
        return;

    osiMessageQueueDelete((osiMessageQueue_t *)(mbox->id));
    mbox->is_vail = 0;
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    if (osiMessageQueueTryPut((osiMessageQueue_t *)(mbox->id), (void *)&msg, 0) != true)
        return ERR_BUF;
    else
        return ERR_OK;
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    osiMessageQueuePut((osiMessageQueue_t *)(mbox->id), (void *)&msg);
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    if (osiMessageQueueTryGet((osiMessageQueue_t *)(mbox->id), (void *)msg, 0) == true)
        return 0;
    else
        return SYS_MBOX_EMPTY;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    u32_t wait_time = 0;
    wait_time = sys_now();
    if (timeout == 0)
    {
        if (osiMessageQueueGet((osiMessageQueue_t *)(mbox->id), (void *)msg) == true)
        {
            return sys_now() - wait_time;
        }
        else
        {
            return SYS_ARCH_TIMEOUT;
        }
    }
    else
    {
        if (osiMessageQueueTryGet((osiMessageQueue_t *)(mbox->id), (void *)msg, timeout) == true)
        {
            return sys_now() - wait_time;
        }
        else
        {
            return SYS_ARCH_TIMEOUT;
        }
    }
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
    if (NULL == mbox)
        return 0;

    return mbox->is_vail;
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    mbox->is_vail = 0;
}

#if LWIP_DEBUG
void sys_arch_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    osiTraceVprintf(LOG_TAG_NET, format, args);
    va_end(args);
}

void sys_arch_dump(const void *Data, uint16_t Size)
{
    //osiTracePrintf(LOG_TAG_NET, "Dump : %*s", Size, Data);
    OSI_LOGXI(OSI_LOGPAR_M, 0x10005236, "Dump : %*s", Size, Data);
    //OSI_LOGXI(OSI_LOGPAR_M, 0, "Dump : %*s", Size, Data);
}
#endif

u32_t sys_now(void)

{
    return (u32_t)osiUpTime();
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    // TODO: Stack size of both sys_thread_new and osiThreadCreate are in bytes.
    // However, caller of sys_thread_new may assume stack size is in words.
    // After double check, the following line should be removed.
    stacksize *= 4;
    return (sys_thread_t)osiThreadCreate(name, thread, arg, prio, stacksize, NET_ENGINE_EVENT_QUEUE_SIZE);
}

#if !LWIP_TIMERS

typedef TAILQ_ENTRY(sys_timer) sys_timer_iter_t;
typedef TAILQ_HEAD(sys_timer_head, sys_timer) sys_timer_head_t;
typedef struct sys_timer
{
    sys_timer_iter_t iter;
    sys_timeout_handler handler;
    void *arg;
    int64_t expire_ms;
} sys_timer_t;

typedef struct
{
    osiTimer_t *osi_timer;
    sys_timer_head_t timer_list;
    int64_t last_expire_ms;
} sys_timer_context_t;

static sys_timer_context_t gSysTimerCtx;

static void prvSysTimerInsertLocked(sys_timer_t *timer)
{
    sys_timer_t *p;
    TAILQ_FOREACH(p, &gSysTimerCtx.timer_list, iter)
    {
        if (timer->expire_ms < p->expire_ms)
        {
            TAILQ_INSERT_BEFORE(p, timer, iter);
            return;
        }
    }

    TAILQ_INSERT_TAIL(&gSysTimerCtx.timer_list, timer, iter);
}

static void prvSysTimerSetLocked(void)
{
    sys_timer_t *p = TAILQ_FIRST(&gSysTimerCtx.timer_list);
    if (p == NULL)
    {
        if (gSysTimerCtx.last_expire_ms != 0)
        {
            gSysTimerCtx.last_expire_ms = 0;
            osiTimerStop(gSysTimerCtx.osi_timer);
        }
    }
    else if (p->expire_ms != gSysTimerCtx.last_expire_ms)
    {
        int64_t curr = osiUpTime();
        int64_t delta = p->expire_ms - curr;
        if (delta < 0)
            delta = 0;
        if (delta > UINT32_MAX)
            delta = UINT32_MAX;
        gSysTimerCtx.last_expire_ms = curr + delta;
        osiTimerStartRelaxed(gSysTimerCtx.osi_timer, delta, OSI_WAIT_FOREVER);
    }
}

static void prvSysTimerProcess(void *param)
{
    int64_t curr = osiUpTime();

    LOCK_TCPIP_CORE();

    sys_timer_t *timer;
    while ((timer = TAILQ_FIRST(&gSysTimerCtx.timer_list)) != NULL)
    {
        if (timer->expire_ms > curr)
            break;

        sys_timeout_handler handler = timer->handler;
        void *arg = timer->arg;
        TAILQ_REMOVE(&gSysTimerCtx.timer_list, timer, iter);
        free(timer);

        if (handler != NULL)
            handler(arg);
    }

    prvSysTimerSetLocked();

    UNLOCK_TCPIP_CORE();
}

void sys_timeout(u32_t msecs, sys_timeout_handler handler, void *arg)
{
    LOCK_TCPIP_CORE();

    sys_timer_t *p = NULL;

    // when the timer (handle & arg) exists, remove it from list
    TAILQ_FOREACH(p, &gSysTimerCtx.timer_list, iter)
    {
        if (p->handler == handler && p->handler == arg)
        {
            TAILQ_REMOVE(&gSysTimerCtx.timer_list, p, iter);
            break;
        }
    }

    // otherwise, create a new one
    if (p == NULL)
    {
        p = (sys_timer_t *)malloc(sizeof(sys_timer_t));
        if (p == NULL)
        {
            UNLOCK_TCPIP_CORE();
            return;
        }
        p->handler = handler;
        p->arg = arg;
    }

    // set the expire time of the timer
    int64_t curr = osiUpTime();
    p->expire_ms = curr + msecs;

    // insert the timer and set osi timer if needed
    prvSysTimerInsertLocked(p);
    prvSysTimerSetLocked();

    UNLOCK_TCPIP_CORE();
}

void sys_untimeout(sys_timeout_handler handler, void *arg)
{
    LOCK_TCPIP_CORE();

    sys_timer_t *p;
    TAILQ_FOREACH(p, &gSysTimerCtx.timer_list, iter)
    {
        if (p->handler == handler && p->arg == arg)
        {
            TAILQ_REMOVE(&gSysTimerCtx.timer_list, p, iter);
            {
                free(p);
                break;
            }
        }
    }

    UNLOCK_TCPIP_CORE();
}

void sys_settime(uint32_t sec, uint32_t frac)
{
    OSI_LOGI(0x1000752d, "sys_settime sec is %lu frac is %lu\n", sec, frac);
    osiSetEpochTime(sec * 1000LL + frac);
    drvRtcUpdateTime();
}

#endif

void sys_init(void)
{
#if !LWIP_TIMERS
    gSysTimerCtx.osi_timer = osiTimerCreate(netThreadID, prvSysTimerProcess, NULL);
    gSysTimerCtx.last_expire_ms = 0;
    TAILQ_INIT(&gSysTimerCtx.timer_list);
#endif
}
