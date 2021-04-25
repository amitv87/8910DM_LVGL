/**
 ******************************************************************************
 * @file    quec_rtos.h
 * @author  kevin.wang
 * @version V1.0.0
 * @date    
 * @brief   This file contains the common definitions, macros and functions to
 *          be shared throughout the project.
 ******************************************************************************
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#ifndef _QUEC_RTOS_H_
#define _QUEC_RTOS_H_

/*> include header files here*/

#include <stdio.h>
#include "osi_api.h"
#include "quec_log.h"
#include "quec_common.h"
#include "drv_names.h"

#define QUEC_WAIT_FOREVER    	0XFFFFFFFF
#define QUEC_EVENT_ID_TIMER 	1
#define QUEC_EVENT_ID_CALLBACK 	2
#define QUEC_EVENT_ID_NOTIFY 	3
#define QUEC_EVENT_ID_QUIT      8


typedef osiThread_t                     quec_thread_t;
typedef osiSemaphore_t					quec_sem_t;
typedef osiMutex_t	   					quec_mutex_t;
typedef osiMessageQueue_t		   		quec_queue_t;
typedef osiTimer_t              		quec_timer_t;

typedef uint32_t						quec_ElapsedTimer_t;
typedef uint32_t 						quec_osiElapsedTimer_t;
typedef void *							quec_ThreadHandle_t;

#ifndef TaskHandle_t
typedef void *                          TaskHandle_t;
#endif


/**
 * opaque data structure for timer pool
 */
typedef osiTimerPool_t                  quec_timer_pool_t;


/**
 * opaque data structure for event queue
 *
 * Event queue is just a message queue, and the message is \p osiEvent_t
 * (event itself rather than pointer).
 */
typedef osiEventQueue_t                 quec_EventQueue_t;

/**
 * opaque data structure for work
 */
 
//typedef osiWork 						quec_work_t;

/**
 * opaque data structure for work queue
 */
typedef osiWorkQueue_t                  quec_WorkQueue_t;

/**
 * opaque data structure for thread notify
 */
typedef osiNotify_t                     quec_notify_t;

/**
 * function type of callback
 */
typedef void (*quec_callback_t)(void *ctx);

/**
 * function type of thread entry
 */
typedef void (*quec_threadEntry_t)(void *argument);

/**
 * function type of interrupt handler
 */
typedef void (*quec_IrqHandler_t)(void *ctx);


typedef osiEvent_t quec_event_t;


typedef enum
{
    QUEC_NOTIFY_IDLE = 0,
    QUEC_NOTIFY_QUEUED_ACTIVE = 1,
    QUEC_NOTIFY_QUEUED_CANCEL = 2,
    QUEC_NOTIFY_QUEUED_DELETE = 3,
} quec_NotifyStatus_e;


typedef enum
{
	eRunning = 0,	/* A task is querying the state of itself, so must be running. */
	eReady,			/* The task being queried is in a read or pending ready list. */
	eBlocked,		/* The task being queried is in the Blocked state. */
	eSuspended,		/* The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
	eDeleted,		/* The task being queried has been deleted, but its TCB has not yet been freed. */
	eInvalid			/* Used as an 'invalid state' value. */
} quec_task_state_e;


typedef struct 
{
	quec_thread_t 		*xHandle;			/* The handle of the task to which the rest of the information in the structure relates. */
	const char *		pcTaskName;			/* A pointer to the task's name.  This value will be invalid if the task was deleted since the structure was populated! */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
	uint		   		xTaskNumber;		/* A number unique to the task. */
	quec_task_state_e 	eCurrentState;		/* The state in which the task existed when the structure was populated. */
	uint 				uxCurrentPriority;	/* The priority at which the task was running (may be inherited) when the structure was populated. */
	uint 				uxBasePriority;		/* The priority to which the task will return if the task's current priority has been inherited to avoid unbounded priority inversion when obtaining a mutex.  Only valid if configUSE_MUTEXES is defined as 1 in FreeRTOSConfig.h. */
	uint				ulRunTimeCounter;	/* The total run time allocated to the task so far, as defined by the run time stats clock.  See http://www.freertos.org/rtos-run-time-stats.html.  Only valid when configGENERATE_RUN_TIME_STATS is defined as 1 in FreeRTOSConfig.h. */
	uint 				*pxStackBase;		/* Points to the lowest address of the task's stack area. */
	uint 				usStackHighWaterMark;	/* The minimum amount of stack space that has remained for the task since the task was created.  The closer this value is to zero the closer the task has come to overflowing its stack. */
} quec_task_status_t;




uint quec_ms_to_ostick(uint ms);

/****************************************  Thread  **************************************************************/
quec_thread_t *quec_thread_creat(const char *name, quec_callback_t func, void *argument,
                             uint priority, uint stack_size,
                             uint event_count);

quec_thread_t * quec_thread_creat_static(const char *name, quec_callback_t func, void *argument,
								   uint priority, void *stack, uint stack_size,
								   uint event_count);

void quec_thread_delete(quec_thread_t *thread);

void quec_thread_suspend(quec_thread_t *thread);

void quec_thread_resume(quec_thread_t *thread);

void quec_thread_yield();

void quec_thread_sleep(int32_t ms);

void quec_thread_sleep_us(uint us);

void quec_thread_sleep_relaxed(uint ms, uint relax_ms);

uint quec_get_thread_stack_CurrentSize(bool refill);

quec_thread_t *quec_get_current_thread(void);             //get current thread handle

uint quec_get_thread_priority(quec_thread_t *thread);

bool quec_set_thread_priority(quec_thread_t *thread, uint priority);

int quec_thread_status_get(quec_thread_t *task, quec_task_status_t *stat);



/****************************************queue***********************************************/

quec_queue_t *quec_queue_create(uint msg_count, uint msg_size);

void quec_queue_delete(quec_queue_t *mq);

bool quec_queue_send(quec_queue_t *mq, const void *msg) ;             //not in ISR

bool quec_queue_try_send(quec_queue_t *mq, const void *msg, uint timeout) ; //maybe the process is in ISR

bool quec_queue_receive(quec_queue_t *mq, void *msg);

bool quec_queue_try_receive(quec_queue_t *mq, void *msg, uint timeout);

int quec_queue_get_cnt(quec_queue_t *mq);



/*************************************************** Timer ***************************************************/
quec_timer_t *quec_timer_creat(quec_callback_t cb, void *ctx);                 //callback will run at current task

quec_timer_t *quec_timer_creat_ex(quec_thread_t *thread, quec_callback_t cb, void *ctx); //callback will run at the allocate task 

void quec_timer_delete(quec_timer_t *timer);

bool quec_timer_start(quec_timer_t *timer, uint ms);

bool quec_timer_stop(quec_timer_t *timer);


// The last period is the period set by  quec_timer_set_period, or other start APIs with period parameter.
bool quec_timer_start_last(quec_timer_t *timer);                  

bool quec_timer_start_relaxed(quec_timer_t *timer, uint ms, uint relaxed_ms);

bool quec_timer_start_us(quec_timer_t *timer, uint us) ;

bool quec_timer_start_periodic(quec_timer_t *timer, uint ms);

bool quec_timer_start_periodic_relaxed(quec_timer_t *timer, uint ms, uint relaxed_ms); 

bool quec_timer_start_periodic_us(quec_timer_t *timer, uint us);

void *quec_timer_get_cb(quec_timer_t *timer);

bool quec_timer_set_period(quec_timer_t *timer, uint ms, bool periodic); // periodic: true for periodic, false for one shot

bool quec_timer_set_period_relaxed(quec_timer_t *timer, uint ms, uint relaxed_ms, bool periodic);

bool quec_timer_is_running(quec_timer_t *timer);              //return true: timer is running   false: not started or invalid 

uint64 quec_timer_remaining(quec_timer_t *timer);



/*********************************************** Event ****************************************************/


bool quec_event_send(quec_thread_t *thread, quec_event_t *event);

bool quec_event_try_send(quec_thread_t *thread, quec_event_t *event, uint timeout);



/********************************************************************************************************
enent id:
	QUEC_EVENT_ID_TIMER 1
 	QUEC_EVENT_ID_CALLBACK 2
 	QUEC_EVENT_ID_NOTIFY 3
*********************************************************************************************************/

bool quec_event_try_wait(quec_thread_t *thread, quec_event_t *event, uint timeout);

bool quec_event_wait(quec_thread_t *thread, quec_event_t *event) ;

bool quec_event_pending(quec_thread_t *thread);            //waiting for the thread queue





/************************************************* Semaphore/Mutex ****************************************/

quec_sem_t *quec_semaphore_create(uint max_cnt, uint init_count);

bool quec_semaphore_get(quec_sem_t *sem);

bool quec_semaphore_try_get(quec_sem_t *sem, uint timeout);

void quec_semaphore_release(quec_sem_t *sem);

void quec_semaphore_delete(quec_sem_t *sem);


quec_mutex_t *quec_mutex_create(void);

bool quec_mutex_lock(quec_mutex_t *mutex, uint timeout);

void quec_mutex_unlock(quec_mutex_t *mutex);

void quec_mutex_delete(quec_mutex_t *mutex);

int quec_semaphore_get_cnt(quec_sem_t *sem);

/**************************************************** others *************************************************/
uint quec_scheduler_suspend(void);

void quec_scheduler_resume(uint flag);

uint quec_enter_critical();

void quec_exit_critical(uint critical);

void quec_enter_critical_from_isr();

void quec_exit_critical_from_isr();


QuecOSStatus quec_send_event_to_api(quec_event_t *event);          //send event to api task

#endif
