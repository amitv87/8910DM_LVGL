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

#include "kernel_config.h"
#include "osi_log.h"
#include "hwregs.h"
#include "osi_api.h"
#include "osi_api_inside.h"
#include "osi_mem.h"
#include "osi_internal.h"
#include "cmsis_core.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>
#include "quec_common.h"

#define OSI_THREAD_LOCAL_EVENTQUEUE_ID 0
#define CONFIG_KERNEL_IDLE_THREAD_STACK_SIZE 4096
#define MAX_THREAD_COUNT (64)
#define MAX_THREAD_NAME_SIZE (32)
static TaskStatus_t gTaskStatus[MAX_THREAD_COUNT];
static const char gDefaultThreadName[] = "(task)";
const char gBuildRevision[] OSI_SECTION_RW_KEEP = CONFIG_BUILD_IDENTIFY;

uint32_t osiMsToOSTick(uint32_t ms)
{
    if (ms == OSI_WAIT_FOREVER)
        return OSI_WAIT_FOREVER;
    if (ms == 0)
        return 0;

    // Tick will be round up. With the checker, 32bits divide
    // will be used in most cases.
    if (ms <= ((0xffffffff - 999) / CONFIG_KERNEL_TICK_HZ))
        return (ms * CONFIG_KERNEL_TICK_HZ + 999) / 1000;

    return ((uint64_t)ms * CONFIG_KERNEL_TICK_HZ + 999) / 1000;
}

OSI_NO_RETURN void osiKernelStart(void)
{
    osiIrqInit();
    osiTimerInit();
    osiSysWorkQueueInit();
    osiClockManInit();

    vTaskStartScheduler();
    OSI_DEAD_LOOP;
}

uint32_t osiSchedulerSuspend(void)
{
    vTaskSuspendAll();
    return 0;
}

void osiSchedulerResume(uint32_t flag)
{
    (void)xTaskResumeAll();
}

static void _setEventQueue(TaskHandle_t hTask, QueueHandle_t hQueue)
{
    // NULL handle is checked inside
    vTaskSetThreadLocalStoragePointer(hTask, OSI_THREAD_LOCAL_EVENTQUEUE_ID, hQueue);
}

osiThread_t *osiThreadCreate(const char *name, osiCallback_t func, void *argument,
                             uint32_t priority, uint32_t stack_size,
                             uint32_t event_count)
{
    TaskHandle_t hTask = NULL;
    QueueHandle_t hQueue = NULL;

    if (func == NULL)
        return NULL;

    if (name == NULL)
        name = gDefaultThreadName;

    if (event_count != 0)
    {
        hQueue = xQueueCreate(event_count, sizeof(osiEvent_t));
        if (hQueue == NULL)
            return NULL;
    }

    // disable scheduler to avoid new thread run before event queue is set
    uint32_t flag = osiSchedulerSuspend();

    // convert byte count to word count
    stack_size = (stack_size + 3) / 4;
    if (xTaskCreate((TaskFunction_t)func, name, (uint16_t)stack_size,
                    argument, priority, &hTask) != pdPASS)
    {
        if (hQueue != NULL)
            vQueueDelete(hQueue);
    }
    else
    {
        _setEventQueue(hTask, hQueue);
    }

    osiSchedulerResume(flag);
    return (osiThread_t *)hTask;
}

osiThread_t *osiThreadCreateWithStack(const char *name, osiCallback_t func, void *argument,
                                      uint32_t priority, void *stack, uint32_t stack_size,
                                      uint32_t event_count)
{
    TaskHandle_t hTask = NULL;
    QueueHandle_t hQueue = NULL;
    StaticTask_t *tcb = NULL;

    if (func == NULL)
        return NULL;

    if (name == NULL)
        name = gDefaultThreadName;

    if (event_count != 0)
    {
        hQueue = xQueueCreate(event_count, sizeof(osiEvent_t));
        if (hQueue == NULL)
            return NULL;
    }

    // disable scheduler to avoid new thread run before event queue is set
    uint32_t flag = osiSchedulerSuspend();

    tcb = (StaticTask_t *)malloc(sizeof(StaticTask_t));
    if (tcb != NULL)
    {
        // convert byte count to word count
        stack_size = (stack_size + 3) / 4;
        hTask = xTaskCreateStatic((TaskFunction_t)func, name, stack_size,
                                  argument, priority, stack, tcb);
        if (hTask == NULL)
        {
            free(tcb);
            if (hQueue != NULL)
                vQueueDelete(hQueue);
        }
        else
        {
            // TODO: change TCB to indicate TCB is dynamic. Otherwise, there
            //       are memory leak at thread delete.
            _setEventQueue(hTask, hQueue);
        }
    }

    osiSchedulerResume(flag);
    return (osiThread_t *)hTask;
}

osiEventQueue_t *osiThreadEventQueue(osiThread_t *thread)
{
    return pvTaskGetThreadLocalStoragePointer((TaskHandle_t)thread, OSI_THREAD_LOCAL_EVENTQUEUE_ID);
}

osiThread_t *osiThreadCurrent(void)
{
    return (osiThread_t *)xTaskGetCurrentTaskHandle();
}

void osiThreadSetFPUEnabled(bool enabled)
{
}

uint32_t osiThreadPriority(osiThread_t *thread)
{
    return uxTaskPriorityGet((TaskHandle_t)thread);
}

bool osiThreadSetPriority(osiThread_t *thread, uint32_t priority)
{
    vTaskPrioritySet((TaskHandle_t)thread, priority);
    return true;
}

void osiThreadSuspend(osiThread_t *thread)
{
    vTaskSuspend((TaskHandle_t)thread);
}

void osiThreadResume(osiThread_t *thread)
{
    if (IS_IRQ())
    {
        BaseType_t yield = xTaskResumeFromISR((TaskHandle_t)thread);
        portYIELD_FROM_ISR(yield);
    }
    else
    {
        vTaskResume((TaskHandle_t)thread);
    }
}

void osiThreadYield(void)
{
    taskYIELD();
}

void osiThreadSleep(uint32_t ms)
{
    vTaskDelay(osiMsToOSTick(ms));
}

void osiThreadSleepUS(uint32_t us)
{
    osiSemaphoreStatic_t *buf_sema = alloca(osiSemaphoreSize());
    osiTimerStatic_t *buf_timer = alloca(osiTimerSize());

    osiSemaphore_t *sema = osiSemaphoreCreateStatic(buf_sema, 1, 0);
    osiTimer_t *timer = osiTimerCreateStatic(buf_timer, NULL, (osiCallback_t)osiSemaphoreRelease, sema);
    osiTimerStartMicrosecond(timer, us);
    osiSemaphoreAcquire(sema);

    osiSemaphoreDelete(sema);
    osiTimerDelete(timer);
}

void osiThreadSleepRelaxed(uint32_t ms, uint32_t relax_ms)
{
    osiThread_t *thread = osiThreadCurrent();
    osiTimer_t *timer = osiTimerCreate(NULL, (osiCallback_t)osiThreadResume, thread);
    if (timer != NULL)
    {
        uint32_t critical = osiEnterCritical();
        osiTimerStartRelaxed(timer, ms, relax_ms);
        vTaskSuspend(NULL);
        osiExitCritical(critical);
        osiTimerDelete(timer);
    }
}

OSI_NO_RETURN void osiThreadExit(void)
{
    // event queue will be deleted on vPortCleanUpTCB
    vTaskDelete(NULL);
    for (;;)
        ;
}

uint32_t osiThreadStackCurrentSpace(bool refill)
{
    TaskStatus_t xTaskDetails;
    vTaskGetInfo(NULL, &xTaskDetails, pdFALSE, eInvalid);

    uintptr_t sp = (uintptr_t)__builtin_frame_address(0);
    uintptr_t base = (uintptr_t)xTaskDetails.pxStackBase;

#if (INCLUDE_uxTaskGetStackHighWaterMark == 1)
    if (refill)
    {
        // refer to tskSTACK_FILL_BYTE, defined in tasks.c
        uint32_t *p = (uint32_t *)base;
        while ((uintptr_t)p < sp)
            *p++ = 0xa5a5a5a5;
    }
#endif
    return sp - base;
}

uint32_t osiThreadStackUnused(osiThread_t *thread)
{
#if (INCLUDE_uxTaskGetStackHighWaterMark == 1)
    UBaseType_t space = uxTaskGetStackHighWaterMark((TaskHandle_t)thread);
    return space * sizeof(StackType_t);
#else
    return 0;
#endif
}

osiMessageQueue_t *osiMessageQueueCreate(uint32_t msg_count, uint32_t msg_size)
{
    if (msg_count == 0 || msg_size == 0)
        return NULL;

    return (osiMessageQueue_t *)xQueueCreate(msg_count, msg_size);
}

void osiMessageQueueDelete(osiMessageQueue_t *mq)
{
    if (mq != NULL)
        vQueueDelete((QueueHandle_t)mq);
}

bool osiMessageQueuePut(osiMessageQueue_t *mq, const void *msg)
{
    if (mq == NULL || msg == NULL)
        return false;

    if (IS_IRQ())
    {
        BaseType_t yield = pdFALSE;
        if (xQueueSendToBackFromISR((QueueHandle_t)mq, msg, &yield) != pdPASS)
            return false;

        portYIELD_FROM_ISR(yield);
        return true;
    }

    return xQueueSendToBack((QueueHandle_t)mq, msg, portMAX_DELAY) == pdPASS;
}

bool osiMessageQueueTryPut(osiMessageQueue_t *mq, const void *msg, uint32_t timeout)
{
    if (mq == NULL || msg == NULL)
        return false;

    if (IS_IRQ())
    {
        BaseType_t yield = pdFALSE;
        if (xQueueSendToBackFromISR((QueueHandle_t)mq, msg, &yield) != pdPASS)
            return false;

        portYIELD_FROM_ISR(yield);
        return true;
    }

    return xQueueSendToBack((QueueHandle_t)mq, msg, osiMsToOSTick(timeout)) == pdPASS;
}

bool osiMessageQueueGet(osiMessageQueue_t *mq, void *msg)
{
    if (mq == NULL || msg == NULL)
        return false;

    if (IS_IRQ())
    {
        BaseType_t yield = pdFALSE;
        if (xQueueReceiveFromISR((QueueHandle_t)mq, msg, &yield) != pdPASS)
            return false;

        portYIELD_FROM_ISR(yield);
        return true;
    }

    return xQueueReceive((QueueHandle_t)mq, msg, OSI_WAIT_FOREVER) == pdPASS;
}

bool osiMessageQueueTryGet(osiMessageQueue_t *mq, void *msg, uint32_t timeout)
{
    if (mq == NULL || msg == NULL)
        return false;

    if (IS_IRQ())
    {
        BaseType_t yield = pdFALSE;
        if (xQueueReceiveFromISR((QueueHandle_t)mq, msg, &yield) != pdPASS)
            return false;

        portYIELD_FROM_ISR(yield);
        return true;
    }

    return xQueueReceive((QueueHandle_t)mq, msg, osiMsToOSTick(timeout)) == pdPASS;
}

uint32_t osiMessageQueuePendingCount(osiMessageQueue_t *mq)
{
    if (mq == NULL)
        return 0;

    // Though it is not designed to be called in ISR in FreeRTOS original
    // design, this implementation can support this.
    return uxQueueMessagesWaiting((QueueHandle_t)mq);
}

uint32_t osiMessageQueueSpaceCount(osiMessageQueue_t *mq)
{
    if (mq == NULL)
        return 0;

    // Though it is not designed to be called in ISR in FreeRTOS original
    // design, this implementation can support this.
    return uxQueueSpacesAvailable((QueueHandle_t)mq);
}

bool osiEventSend(osiThread_t *thread, const osiEvent_t *event)
{
    if (thread == NULL || event == NULL)
        return false;

    osiEventQueue_t *queue = osiThreadEventQueue(thread);
    if (queue == NULL)
        return false;

    if (IS_IRQ())
    {
        BaseType_t yield = pdFALSE;
        if (xQueueSendToBackFromISR((QueueHandle_t)queue, event, &yield) != pdPASS)
            return false;

        portYIELD_FROM_ISR(yield);
        return true;
    }

    if ((TaskHandle_t)thread == xTaskGetCurrentTaskHandle())
    {
        if (xQueueSendToBack((QueueHandle_t)queue, event, 0) != pdPASS)
        {
            OSI_LOGE(0, "failed to send event to current thread");
            osiPanic();
        }
        return true;
    }

#ifndef CONFIG_QUEC_PROJECT_FEATURE
    return xQueueSendToBack((QueueHandle_t)queue, event, portMAX_DELAY) == pdPASS;
#else
	return xQueueSendToBack((QueueHandle_t)queue, event, 1000) == pdPASS;    //avoid task bloced forever here
#endif

}

bool osiEventTrySend(osiThread_t *thread, const osiEvent_t *event, uint32_t timeout)
{
    if (event == NULL || thread == NULL)
        return false;

    osiEventQueue_t *queue = osiThreadEventQueue(thread);
    if (queue == NULL)
        return false;

    if (IS_IRQ())
    {
        BaseType_t yield = pdFALSE;
        if (xQueueSendToBackFromISR((QueueHandle_t)queue, event, &yield) != pdPASS)
            return false;

        portYIELD_FROM_ISR(yield);
        return true;
    }

    return xQueueSendToBack((QueueHandle_t)queue, event, osiMsToOSTick(timeout)) == pdPASS;
}

bool osiSendQuitEvent(osiThread_t *thread, bool wait)
{
    if (thread == NULL)
        return false;

    osiEvent_t event = {
        .id = OSI_EVENT_ID_QUIT,
        .param1 = 0,
        .param2 = 0,
        .param3 = 0,
    };

    if (wait)
    {
        if (thread == osiThreadCurrent())
            return false;

        osiSemaphoreStatic_t *buf_sema = alloca(osiSemaphoreSize());
        osiSemaphore_t *sema = osiSemaphoreCreateStatic(buf_sema, 1, 0);

        event.param1 = (uint32_t)sema;
        osiEventSend(thread, &event);

        osiSemaphoreAcquire(sema);
        osiSemaphoreDelete(sema);
    }
    else
    {
        osiEventSend(thread, &event);
    }
    return true;
}

bool osiEventWait(osiThread_t *thread, osiEvent_t *event)
{
    return osiEventTryWait(thread, event, OSI_WAIT_FOREVER);
}

bool osiEventTryWait(osiThread_t *thread, osiEvent_t *event, uint32_t timeout)
{
    if (IS_IRQ() || thread == NULL || event == NULL)
        return false;

    osiEventQueue_t *queue = osiThreadEventQueue(thread);
    if (queue == NULL)
        return false;

    if (xQueueReceive((QueueHandle_t)queue, event, osiMsToOSTick(timeout)) == pdPASS)
    {
        if (event->id == OSI_EVENT_ID_TIMER)
        {
            osiTimerEventInvoke(event);
        }
        else if (event->id == OSI_EVENT_ID_CALLBACK)
        {
            osiCallback_t cb = (osiCallback_t)event->param1;
            if (cb != NULL)
                cb((void *)event->param2);
            event->id = OSI_EVENT_ID_NONE;
        }
        else if (event->id == OSI_EVENT_ID_NOTIFY)
        {
            uint32_t critical = osiEnterCritical();
            osiCallback_t cb = NULL;
            osiNotify_t *notify = (osiNotify_t *)event->param1;
            if (notify->status == OSI_NOTIFY_QUEUED_DELETE)
            {
                free(notify);
            }
            else if (notify->status == OSI_NOTIFY_QUEUED_ACTIVE)
            {
                cb = notify->cb;
                notify->status = OSI_NOTIFY_IDLE;
            }
            else
            {
                notify->status = OSI_NOTIFY_IDLE;
            }
            osiExitCritical(critical);

            if (cb != NULL)
                cb(notify->ctx);
            event->id = OSI_EVENT_ID_NONE;
        }
        else if (event->id == OSI_EVENT_ID_QUIT)
        {
            // the first parameter is semaphore
            osiSemaphore_t *sema = (osiSemaphore_t *)event->param1;
            if (sema != NULL)
                osiSemaphoreRelease(sema);
        }
        return true;
    }

    return false;
}

bool osiEventPending(osiThread_t *thread)
{
    if (thread == NULL)
        return false;

    osiEventQueue_t *queue = osiThreadEventQueue(thread);
    if (queue == NULL)
        return false;

    // Though it is not designed to be called in ISR in FreeRTOS original
    // design, this implementation can support this.
    return (uxQueueMessagesWaiting((QueueHandle_t)queue) > 0);
}

uint32_t osiEventPendingCount(osiThread_t *thread)
{
    if (thread == NULL)
        return 0;

    osiEventQueue_t *queue = osiThreadEventQueue(thread);
    if (queue == NULL)
        return 0;

    // Though it is not designed to be called in ISR in FreeRTOS original
    // design, this implementation can support this.
    return uxQueueMessagesWaiting((QueueHandle_t)queue);
}

uint32_t osiEventSpaceCount(osiThread_t *thread)
{
    if (thread == NULL)
        return 0;

    osiEventQueue_t *queue = osiThreadEventQueue(thread);
    if (queue == NULL)
        return 0;

    // Though it is not designed to be called in ISR in FreeRTOS original
    // design, this implementation can support this.
    return uxQueueSpacesAvailable((QueueHandle_t)queue);
}

bool osiThreadCallback(osiThread_t *thread, osiCallback_t cb, void *cb_ctx)
{
    if (thread == NULL || cb == NULL)
        return false;

    osiEvent_t event = {
        .id = OSI_EVENT_ID_CALLBACK,
        .param1 = (uint32_t)cb,
        .param2 = (uint32_t)cb_ctx,
        .param3 = 0,
    };

    if (IS_IRQ())
        return osiEventTrySend(thread, &event, 0);

    return osiEventSend(thread, &event);
}

osiMutex_t *osiMutexCreate(void)
{
    return (osiMutex_t *)xSemaphoreCreateRecursiveMutex();
}

void osiMutexLock(osiMutex_t *mutex)
{
    if (IS_IRQ())
        return;

    xSemaphoreTakeRecursive((QueueHandle_t)mutex, portMAX_DELAY);
}

bool osiMutexTryLock(osiMutex_t *mutex, uint32_t timeout)
{
    if (IS_IRQ())
        return false;

    return xSemaphoreTakeRecursive((QueueHandle_t)mutex, osiMsToOSTick(timeout));
}

void osiMutexUnlock(osiMutex_t *mutex)
{
    if (IS_IRQ())
        return;

    xSemaphoreGiveRecursive((QueueHandle_t)mutex);
}

void osiMutexDelete(osiMutex_t *mutex)
{
    if (mutex != NULL)
        vSemaphoreDelete((QueueHandle_t)mutex);
}

unsigned osiSemaphoreSize(void)
{
    return sizeof(StaticSemaphore_t);
}

osiSemaphore_t *osiSemaphoreCreate(uint32_t max_count, uint32_t init_count)
{
    if (max_count == 1)
    {
        SemaphoreHandle_t sem = xSemaphoreCreateBinary();
        if (sem == NULL)
            return NULL;

        if (init_count == 1)
            xSemaphoreGive(sem);
        return (osiSemaphore_t *)sem;
    }

    return (osiSemaphore_t *)xSemaphoreCreateCounting(max_count, init_count);
}

osiSemaphore_t *osiSemaphoreCreateStatic(osiSemaphoreStatic_t *buf, uint32_t max_count, uint32_t init_count)
{
    if (max_count == 1)
    {
        SemaphoreHandle_t sem = xSemaphoreCreateBinaryStatic((StaticSemaphore_t *)buf);
        if (sem == NULL)
            return NULL;

        if (init_count == 1)
            xSemaphoreGive(sem);
        return (osiSemaphore_t *)sem;
    }

    return (osiSemaphore_t *)xSemaphoreCreateCountingStatic(max_count, init_count, (StaticSemaphore_t *)buf);
}

bool osiSemaphoreAcquire(osiSemaphore_t *sem)
{
    if (sem == NULL)
        return false;

    if (IS_IRQ())
    {
        BaseType_t yield = pdFALSE;
        if (xSemaphoreTakeFromISR((QueueHandle_t)sem, &yield) != pdPASS)
            return false;

        portYIELD_FROM_ISR(yield);
        return true;
    }

    return xSemaphoreTake((QueueHandle_t)sem, portMAX_DELAY) == pdPASS;
}

bool osiSemaphoreTryAcquire(osiSemaphore_t *sem, uint32_t timeout)
{
    if (sem == NULL)
        return false;

    if (IS_IRQ())
    {
        BaseType_t yield = pdFALSE;
        if (xSemaphoreTakeFromISR((QueueHandle_t)sem, &yield) != pdPASS)
            return false;

        portYIELD_FROM_ISR(yield);
        return true;
    }

    return xSemaphoreTake((QueueHandle_t)sem, osiMsToOSTick(timeout)) == pdPASS;
}

void osiSemaphoreRelease(osiSemaphore_t *sem)
{
    if (IS_IRQ())
    {
        BaseType_t yield = pdFALSE;
        xSemaphoreGiveFromISR((QueueHandle_t)sem, &yield);
        portYIELD_FROM_ISR(yield);
    }
    else
    {
        xSemaphoreGive((QueueHandle_t)sem);
    }
}

void osiSemaphoreDelete(osiSemaphore_t *sem)
{
    if (sem != NULL)
        vSemaphoreDelete((QueueHandle_t)sem);
}

void vPortCleanUpTCB(void *pxTCB)
{
    osiThread_t *thread = pxTCB;
    osiEventQueue_t *queue = osiThreadEventQueue(thread);
    if (queue != NULL)
        vQueueDelete((QueueHandle_t)queue);
    _setEventQueue((TaskHandle_t)thread, NULL);
}

bool osiIsSleepAbort(void)
{
#ifdef CONFIG_SOC_8910
    return (eTaskConfirmSleepModeStatus() == eAbortSleep);
#else
    return true; // sleep is not ready now
#endif
}

#if (configCHECK_FOR_STACK_OVERFLOW > 0)
OSI_WEAK void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
    osiPanic();
}
#endif

/* Idle task control block and stack */
static StaticTask_t Idle_TCB;
static StackType_t Idle_Stack[configMINIMAL_STACK_SIZE] OSI_ALIGNED(8);

/* Timer task control block and stack */
static StaticTask_t Timer_TCB;
static StackType_t Timer_Stack[configTIMER_TASK_STACK_DEPTH] OSI_ALIGNED(8);

/*
  vApplicationGetIdleTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    Idle_Stack[0] = 0;
    Idle_Stack[1] = (sizeof(Idle_Stack) >> 3);
    *ppxIdleTaskTCBBuffer = &Idle_TCB;
    *ppxIdleTaskStackBuffer = &Idle_Stack[2];
    *pulIdleTaskStackSize = (uint32_t)CONFIG_KERNEL_IDLE_THREAD_STACK_SIZE / 4;
}

/*
  vApplicationGetTimerTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    Timer_Stack[0] = 0;
    Timer_Stack[1] = (sizeof(Timer_Stack) >> 3);
    *ppxTimerTaskTCBBuffer = &Timer_TCB;
    *ppxTimerTaskStackBuffer = &Timer_Stack[2];
    *pulTimerTaskStackSize = (uint32_t)configTIMER_TASK_STACK_DEPTH;
}

void osiShowThreadState(void)
{
    extern void *pxCurrentTCB;
    if (pxCurrentTCB == NULL)
        return;

    int count = uxTaskGetSystemState(gTaskStatus, MAX_THREAD_COUNT, NULL);
    OSI_LOGI(0, "TASK count %d", count);
    for (int n = 0; n < count; n++)
    {
        OSI_LOGXI(OSI_LOGPAR(I, S, I, I, I), 0, "TASK %d (%s) state/%d prio/%d/%d",
                  gTaskStatus[n].xTaskNumber,
                  gTaskStatus[n].pcTaskName,
                  gTaskStatus[n].eCurrentState,
                  gTaskStatus[n].uxCurrentPriority,
                  gTaskStatus[n].uxBasePriority);
    }
}

uint32_t osiThreadCount(void)
{
    return uxTaskGetNumberOfTasks();
}

static_assert(sizeof(osiThreadStatus_t) == sizeof(TaskStatus_t), "osiThreadStatus_t error");
int osiThreadGetAllStatus(osiThreadStatus_t *status, uint32_t count)
{
    if (status == NULL)
        return -1;

    count = uxTaskGetSystemState((TaskStatus_t *)status, count, NULL);
    for (unsigned n = 0; n < count; n++)
    {
        TaskStatus_t fstatus = *(TaskStatus_t *)status;
        status->handler = fstatus.xHandle;
        status->name = fstatus.pcTaskName;
        status->thread_number = fstatus.xTaskNumber;
        status->state = fstatus.eCurrentState;
        status->curr_priority = fstatus.uxCurrentPriority;
        status->base_priority = fstatus.uxBasePriority;
        status->stack_base = fstatus.pxStackBase;
        status->stack_alloc_size = osiMemAllocSize(fstatus.pxStackBase);
        status->stack_min_remained = fstatus.usStackHighWaterMark * sizeof(StackType_t);
        status++;
    }
    return count;
}

void osiTickHandler(uint32_t ostick)
{
    static uint32_t prev_ostick = 0;
    int delta = ostick - prev_ostick;

    OSI_LOGD(0, "OS tick %u/%u", prev_ostick, ostick);

    // Though it shouldn't happen, it is hard to avoid completely
    // due to rounding error.
    if (delta == 0)
        return;

    if (delta < 0)
        osiPanic();

    if (delta > 1)
        vTaskStepTick(delta - 1);

    prev_ostick = ostick;
    BaseType_t yield = xTaskIncrementTick();
    portYIELD_FROM_ISR(yield);
}

void osiTickSetInitial(uint32_t ostick)
{
    // It should only be called at boot, and xTickCount should be 0.
    // This will set xTickCount aligned with osiUpHWTick. This should
    // be called before any API calls with timeout.

    OSI_LOGI(0, "OS tick init value %u", ostick);
    vTaskStepTick(ostick);
}

void exit(int status)
{
    osiPanic();
}

void abort(void)
{
    osiPanic();
}

void _assert(void)
{
    osiPanic();
}

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler)
{
    errno = EINVAL;
    return SIG_ERR;
}

int isatty(int fd)
{
    errno = EINVAL;
    return 0;
}

pid_t getpid(void) { return 1; }
pid_t getppid(void) { return 1; }
