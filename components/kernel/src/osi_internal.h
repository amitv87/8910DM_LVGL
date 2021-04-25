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

#ifndef _OSI_INTERNAL_H_
#define _OSI_INTERNAL_H_

#include "osi_api.h"
#include "hal_config.h"
#include "cmsis_core.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_CPU_ARMV7A
#define IS_IRQ() ({ unsigned _mode = __get_mode(); !(_mode == CPSR_M_USR || _mode == CPSR_M_SYS); })
#endif

#ifdef CONFIG_CPU_ARMV7M
#define IS_IRQ() (__get_IPSR() != 0)
#endif

#ifdef CONFIG_CPU_ARMV8M
#define IS_IRQ() (__get_IPSR() != 0)
#endif

#define OSI_EVENT_ID_TIMER 1
#define OSI_EVENT_ID_CALLBACK 2
#define OSI_EVENT_ID_NOTIFY 3

typedef enum
{
    OSI_NOTIFY_IDLE = 0,
    OSI_NOTIFY_QUEUED_ACTIVE = 1,
    OSI_NOTIFY_QUEUED_CANCEL = 2,
    OSI_NOTIFY_QUEUED_DELETE = 3,
} osiNotifyStatus_t;

struct osiNotify
{
    osiThread_t *thread;
    osiCallback_t cb;
    void *ctx;
    osiNotifyStatus_t status;
};

typedef struct
{
    uint32_t caller;
    uint32_t ptr;
} osiMemRecord_t;

extern const unsigned gOstickRelaxedTick;
extern const unsigned gTimerWqStackSize;
extern const unsigned gPeriodicTimerMinPeriod;
extern int64_t gOsiEpochOffset;
extern int64_t gOsiUpTimeOffset;
extern int64_t gOsiLocalOffset;
extern const unsigned gOsiMemRecordCount;
extern unsigned gOsiMemRecordPos;
extern osiMemRecord_t gOsiMemRecords[];

void osiSetUpHWTick(int64_t tick);
int64_t osiEpochTick(void);
uint32_t osiMsToOSTick(uint32_t ms);
void osiTimerEventInvoke(osiEvent_t *event);
void osiIrqInit(void);
void osiTimerInit(void);
void osiSysWorkQueueInit(void);
osiCallback_t osiWorkFunction(osiWork_t *work);
void *osiWorkContext(osiWork_t *work);

/**
 * \brief tickless light sleep for timer
 *
 * When OS tick is implemented in timer, \p idle_tick is the maximum
 * idle OS tick count (not hardware tick count) for sleep.
 *
 * The timer interrupt will be moved to minimum of:
 * * OS timer, after \p idle_tick
 * * other timers timeout time
 *
 * When there are no timers, timer interrupt will be disabled.
 *
 * The normal timeout time of timer, rather than the relaxed timeout
 * time of timer, will be used.
 *
 * When the timer interrupt is moved, it will return true. In that case,
 * the timer interrupt will be moved back after light sleep.
 *
 * \param idle_tick     OS tick sleep count
 * \return
 *      - true if timer interrupt is moved
 *      = false if timer interrupt is not moved
 */
bool osiTimerLightSleep(uint32_t idle_tick);

/**
 * \brief calculate the deep sleep time of all timers
 *
 * When OS tick is implemented in timer, \p idle_tick is the maximum
 * idle OS tick count (not hardware tick count) for sleep.
 *
 * When there are timers which will wakeup system, the return value is
 * the earliest timer wakeup time from now in milliseconds.
 *
 * When there are no timer will wakeup system, return \p INT64_MAX.
 *
 * The relaxed timeout time of timer, rather than the normal timeout
 * time, is used in checking sleep time.
 *
 * Usually, it will be called by system sleep module. And it is not
 * prohibited to be called in other cases.
 *
 * It **must** be called with interrupt disabled. The implementation
 * won't perform protection.
 *
 * \param idle_tick     OS tick sleep count
 * \return
 *      - the earliest sleep timer from now in milliseconds.
 *        \p INT64_MAX for no need to wakeup.
 */
int64_t osiTimerDeepSleepTime(uint32_t idle_tick);

/**
 * \brief calculate the PSM wakup time of all timers
 *
 * When OS tick is implemented in timer, OS tick is not considered.
 * That is, PSM wont't be waken by OS tick.
 *
 * The there are times which will wakeup system, the return value is
 * the earliest timer wakeup time in milliseconds.
 *
 * When there are no timer will wakeup system, return \p INT64_MAX.
 *
 * The relaxed timeout time of timer, rather than the normal timeout
 * time, is used in checking sleep time.
 *
 * It **must** be called with interrupt disabled. The implementation
 * won't perform protection.
 *
 * \return
 *      - the earliest wakeup timer from now in milliseconds.
 *        \p INT64_MAX for no need to wakeup.
 */
int64_t osiTimerPsmWakeUpTime(void);

/**
 * \brief timer module processing after wakeup
 *
 * In case \p osiUpHWTick will be discontinued at sleep, it should be called
 * after \p osiUpHWTick is stable.
 *
 * Usually, it will be called by system sleep module. And it is not
 * prohibited to be called in other cases.
 *
 * It **must** be called with interrupt disabled. The implementation
 * won't perform protection.
 */
void osiTimerWakeupProcess(void);

/**
 * \brief power management suspend
 *
 * Usually, it shouldn't be called directly by application. Rather, it will
 * be called after suspend criteria are met.
 */
void osiPmSleep(uint32_t idle_tick);

/**
 * \brief whether sleep should be abort by RTOS
 *
 * In FreeRTOS, it is needed to check whether sleep should be aborted before
 * enter sleep. It is caused by pending-ready list is not empty, and it should
 * be caused by ISR after \p vTaskSuspendAll.
 *
 * It should be called with interrupt disabled.
 *
 * \return
 *      - true if sleep should be aborted
 *      - false if sleep is permitted
 */
bool osiIsSleepAbort(void);
void osiTickHandler(uint32_t ostick);
void osiTickSetInitial(uint32_t ostick);

void osiIrqSuspend(osiSuspendMode_t mode);
void osiIrqResume(osiSuspendMode_t mode, bool aborted);

void osiMemRecordAllocLocked(const void *ptr, const void *caller);
void osiMemRecordFreeLocked(const void *ptr, const void *caller);

#ifdef __cplusplus
}
#endif
#endif
