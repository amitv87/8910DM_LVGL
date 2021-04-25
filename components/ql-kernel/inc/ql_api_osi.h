#ifndef _QL_API_OSI_H_
#define _QL_API_OSI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ql_osi_def.h"

#define QL_TIMER_IN_SERVICE ((void *)0xffffffff)

typedef int    QlOSStatus;
typedef void * ql_task_t;
typedef void * ql_sem_t;
typedef void * ql_mutex_t;
typedef void * ql_queue_t;
typedef void * ql_timer_t;
typedef void * ql_flag_t;

typedef enum
{
	QL_WAIT_FOREVER = 0xFFFFFFFF,
	QL_NO_WAIT	  	= 0
} ql_wait_e;

typedef enum
{
	Running = 0,	/* A task is querying the state of itself, so must be running. */
	Ready,			/* The task being queried is in a read or pending ready list. */
	Blocked,		/* The task being queried is in the Blocked state. */
	Suspended,		/* The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
	Deleted,		/* The task being queried has been deleted, but its TCB has not yet been freed. */
	Invalid			/* Used as an 'invalid state' value. */
} ql_task_state_e;

typedef struct 
{
	ql_task_t			xHandle;			/* The handle of the task to which the rest of the information in the structure relates. */
	const char *		pcTaskName;			/* A pointer to the task's name.  This value will be invalid if the task was deleted since the structure was populated! */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
	uint		   		xTaskNumber;		/* A number unique to the task. */
	ql_task_state_e 	eCurrentState;		/* The state in which the task existed when the structure was populated. */
	uint 				uxCurrentPriority;	/* The priority at which the task was running (may be inherited) when the structure was populated. */
	uint 				uxBasePriority;		/* The priority to which the task will return if the task's current priority has been inherited to avoid unbounded priority inversion when obtaining a mutex.  Only valid if configUSE_MUTEXES is defined as 1 in FreeRTOSConfig.h. */
	uint				ulRunTimeCounter;	/* The total run time allocated to the task so far, as defined by the run time stats clock.  See http://www.freertos.org/rtos-run-time-stats.html.  Only valid when configGENERATE_RUN_TIME_STATS is defined as 1 in FreeRTOSConfig.h. */
	uint 				*pxStackBase;		/* Points to the lowest address of the task's stack area. */
	uint 				usStackHighWaterMark;	/* The minimum amount of stack space that has remained for the task since the task was created.  The closer this value is to zero the closer the task has come to overflowing its stack. */
} ql_task_status_t;

typedef struct
{
    uint32 id;     ///< event identifier
    uint32 param1; ///< 1st parameter
    uint32 param2; ///< 2nd parameter
    uint32 param3; ///< 3rd parameter
} ql_event_t;

typedef struct
{
	uint32 sec;
	uint32 usec;
}ql_timeval_t;

/*========================================================================
 *	function Definition
 *========================================================================*/

typedef void (*ql_swdog_callback)(uint32 id_type, void *ctx);
typedef void (*lvgl_Callback_t)(void *ctx);

extern QlOSStatus ql_rtos_task_create
(
	ql_task_t	 * taskRef, 			 /* OS task reference										   */
	uint32		   stackSize,			 /* number of bytes in task stack area						   */
	uint8 		   priority,			 /* task priority											   */
	char		 * taskName,			 /* task name												   */
	void	   ( * taskStart)(void*),	 /* pointer to task entry point 			 				   */
	void*		   argv,				 /* task entry argument pointer 							   */
	uint32         event_count,           /* the max event count that the task can waiting to process   */ 
 	...
);

extern QlOSStatus ql_rtos_task_create_default
(
	ql_task_t	 * taskRef, 			 /* OS task reference										   */
	uint32		   stackSize,			 /* number of bytes in task stack area						   */
	uint8 		   priority,			 /* task priority											   */
	char		 * taskName,			 /* task name												   */
	void	   ( * taskStart)(void*),	 /* pointer to task entry point 			 				   */
	void*		   argv,				 /* task entry argument pointer 							   */
 	...
);

extern QlOSStatus ql_rtos_event_send
(
	ql_task_t	task_ref,
	ql_event_t	*event
);

extern QlOSStatus ql_event_wait
(
	ql_event_t	 *event_strc,      /* event structure     */
	uint32		 timeout           /* event wait timeout  */
);

extern QlOSStatus ql_event_try_wait
(
	ql_event_t	 *event_strc	/* event structure     */
);

extern QlOSStatus ql_rtos_task_delete
(
	ql_task_t taskRef		/* OS task reference	*/
);

extern QlOSStatus ql_rtos_task_suspend
(
	ql_task_t taskRef		/* OS task reference	*/
);

extern QlOSStatus ql_rtos_task_resume
(
	ql_task_t taskRef		/* OS task reference	*/
);

extern void ql_rtos_task_yield(void);

extern QlOSStatus ql_rtos_task_get_current_ref
(
	ql_task_t * taskRef		/* OS task reference	*/
);

extern QlOSStatus ql_rtos_task_change_priority
(
	ql_task_t 	taskRef,			/* OS task reference			*/
	uint8 		new_priority,		/* OS task new priority	for in	*/
	uint8	   *old_priority		/* OS task old priority	for out	*/
);

extern QlOSStatus ql_rtos_task_get_priority
(
	ql_task_t 	taskRef,		/* OS task reference			*/
	uint8 * 	priority_ptr	/* OS task priority for out		*/
);

extern QlOSStatus ql_rtos_task_get_status
(
	ql_task_t      		 task_ref,
	ql_task_status_t 	 *status
);

extern void ql_rtos_task_sleep_ms
(
	uint32 ms	   /* OS task sleep time for ms	*/
);

extern void ql_rtos_task_sleep_s
(
	uint32 s	   /* OS task sleep time for s		*/
);

extern uint32_t ql_rtos_enter_critical(void);

//the parameter is the return value from ql_rtos_enter_critical()
extern void ql_rtos_exit_critical(uint32_t critical);

extern QlOSStatus ql_rtos_semaphore_create
(
	ql_sem_t  	*semaRef,       /* OS semaphore reference                     	*/
	uint32      initialCount    /* initial count of the semaphore             	*/
);

extern QlOSStatus ql_rtos_semaphore_wait
(
	ql_sem_t  	semaRef,       /* OS semaphore reference                     	*/
	uint32      timeout    	   /* QL_WAIT_FOREVER, QL_NO_WAIT, or timeout	*/
);

extern QlOSStatus ql_rtos_semaphore_release
(
	ql_sem_t   semaRef        /* OS semaphore reference						*/
);

extern QlOSStatus ql_rtos_semaphore_get_cnt
(
	ql_sem_t  	semaRef,       /* OS semaphore reference           				*/
	uint32    * cnt_ptr    	   /* out-parm to save the cnt of semaphore      	*/
);

extern QlOSStatus ql_rtos_semaphore_delete
(
	ql_sem_t   semaRef        /* OS semaphore reference                  		*/
);

extern QlOSStatus ql_rtos_mutex_create
(
    ql_mutex_t  *mutexRef        /* OS mutex reference                         */
);

extern QlOSStatus ql_rtos_mutex_lock
(
	ql_mutex_t    mutexRef,       /* OS mutex reference                         */
	uint32        timeout   	  /* mutex wait timeout		             		*/
);

extern QlOSStatus ql_rtos_mutex_try_lock
(
	ql_mutex_t  mutexRef        /* OS mutex reference                         */
);

extern QlOSStatus ql_rtos_mutex_unlock
(
	ql_mutex_t  mutexRef        /* OS mutex reference                         */
);

extern QlOSStatus ql_rtos_mutex_delete
(
	ql_mutex_t  mutexRef        /* OS mutex reference                         */
);

extern QlOSStatus ql_rtos_queue_create
(
	ql_queue_t   	*msgQRef,       	/* OS message queue reference              */
	uint32         	maxSize,        	/* max message size the queue supports     */
	uint32         	maxNumber	      	/* max # of messages in the queue          */
);

extern QlOSStatus ql_rtos_queue_wait
(
	ql_queue_t   	msgQRef,		/* message queue reference                 		*/
	uint8  	      	*recvMsg,       /* pointer to the message received         		*/
	uint32         	size, 			/* size of the message                     		*/
	uint32         	timeout         /* QL_WAIT_FOREVER, QL_NO_WAIT, or timeout  */
);

extern QlOSStatus ql_rtos_queue_release
(
    ql_queue_t		msgQRef,        /* message queue reference                 			*/
    uint32         	size,           /* size of the message                     			*/
    uint8          	*msgPtr,        /* start address of the data to be sent    			*/
    uint32         	timeout         /* QL_WAIT_FOREVER, QL_NO_WAIT, or timeout   	*/
);

extern QlOSStatus ql_rtos_queue_get_cnt
(
	ql_queue_t		msgQRef,        /* message queue reference                 		*/
	uint32     		*cnt_ptr    	/* out-parm to save the cnt of message queue	*/
);

extern QlOSStatus ql_rtos_queue_delete
(
	ql_queue_t	msgQRef         /* message queue reference                 		*/
);

//taskRef取值: 
//若指定task去运行定时器回调函数，则填入任务句柄；若在系统service中运行定时器回调函数，填入QL_TIMER_IN_SERVICE
//若需要在中断中运行定时器回调函数，则填入NULL，不建议使用该方法，若使用必须快进快出，更不可在中断中阻塞
extern QlOSStatus ql_rtos_timer_create
(
	ql_timer_t	   * timerRef,					/* OS supplied timer reference	*/
	ql_task_t	   taskRef,
	void		   (*callBackRoutine)(void *),	   /* timer call-back routine						   */
	void		   *timerArgc				   /* argument to be passed to call-back on expiration */
);

extern QlOSStatus ql_rtos_timer_start
(
	ql_timer_t		timerRef,					/* OS supplied timer reference						*/
	uint32			set_Time,					/* timer set value									*/
	unsigned char	cyclicalEn					/* wether to enable the cyclical mode or not		*/
);

extern QlOSStatus ql_rtos_timer_start_relaxed   		 /* for sleep mode, if timer finished but system is sleeping, timer will wakeup system after relax_time */
(
	ql_timer_t 		 timerRef,					 /* OS supplied timer reference 					 */
	uint32			 set_Time,				     /* timer set value									 */
	unsigned char	 cyclicalEn,  				 /* wether to enable the cyclical mode or not		 */
	uint32    		 relax_time					 /* the timer wakeup time, if not want timer to wake up system, input  QL_WAIT_FOREVER */
);

QlOSStatus ql_rtos_timer_start_us
(
	ql_timer_t 		 timerRef,					 /* OS supplied timer reference 					 */
	uint32			 set_Time_us				 /* timer set value									 */
);

extern QlOSStatus ql_rtos_timer_stop
(
	ql_timer_t timerRef 				/* OS supplied timer reference	*/
);

extern QlOSStatus ql_rtos_timer_delete
(
	ql_timer_t timerRef 				/* OS supplied timer reference	*/
);

QlOSStatus ql_rtos_swdog_register
(
	ql_swdog_callback callback,			/* software watch dog callback  */
	ql_task_t *taskRef					/* OS task reference  			*/
);

QlOSStatus ql_rtos_swdog_unregister
(
	ql_swdog_callback callback			/* software watch dog callback  */
);

QlOSStatus ql_rtos_feed_dog(void);

QlOSStatus ql_rtos_sw_dog_enable
(
	uint32 period_ms, 
	uint32 missed_cnt
);

QlOSStatus ql_rtos_sw_dog_disable(void);

QlOSStatus ql_gettimeofday
(
	ql_timeval_t *timeval
);

int ql_rtos_get_system_tick(void);


#ifdef __cplusplus
} /*"C" */
#endif

#endif
