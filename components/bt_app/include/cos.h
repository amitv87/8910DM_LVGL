/**
 * @file    cos.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#ifndef _COS_H_
#define _COS_H_

/************************************************************************************
 * COS MACRO definition                                                  *
 ***********************************************************************************/
#define COS_FUNCTION_TIMER_POS 14
#define COS_FUNCTION_TIMER ((uint16_t)1 << COS_FUNCTION_TIMER_POS)
#define COS_MBX_TIMER_POS 15
#define COS_MBX_TIMER ((uint16_t)1 << COS_MBX_TIMER_POS)

#define COS_TIMER_UNUSED 0
#define COS_MAX_TIMERS 20

#define COS_MSG_LEN 10

#define COS_WAIT_FOREVER 0xFFFFFFFF
#define COS_NO_WAIT 0x0

///event priority decription
#define COS_EVENT_PRI_NORMAL 0
#define COS_EVENT_PRI_URGENT 1
#define COS_EVENT_PRI_ISR 2

#define COS_TIMER_MODE_SINGLE 1
#define COS_TIMER_MODE_PERIODIC 2

#define COS_TIMER_ID_BASE 0x000
#define COS_TIMER_ID_END 0x0FF

/// Number of tick timer ticks during one second
#define HAL_TICK1S 16384

/// Number of delay timer during one millisecond
#define MS_WAITING *HAL_TICK1S / 1000

#define SECOND *HAL_TICK1S
#define MILLI_SECOND SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
#define SECONDS SECOND
#define MINUTE *(60 SECOND)
#define MINUTES MINUTE
#define HOUR *(60 MINUTE)
#define HOURS HOUR

/************************************************************************************
 * COS structure definition                                                  *
 ***********************************************************************************/
#pragma pack(1)
typedef struct _TASK_DESC
{
    void (*task_body)(void *);
    const char *name;
    uint16_t stack_size;
    uint8_t priority;
} TASK_DESC;
#pragma pack()

typedef enum
{
    MOD_NULL = 0,
    MOD_SYS,
    MOD_BLE, // ble task
    MOD_BT,  // bt task
    MOD_APP, // application task

    TOTAL_MODULES
} COS_MOD_ID;

#pragma pack(1)
typedef struct _TASK_HANDLE
{
    TASK_DESC task_desc;
    COS_MOD_ID task_mod;
    void *queue;
    void *task_handle;
    bool exclusive;
} TASK_HANDLE;

typedef struct T_COS_TIMER_INFO
{
    uint8_t flags;
    void (*callback_func)(void *);
    void *callback_param;
} T_COS_TIMER_INFO;

#pragma pack()

typedef struct _COS_EVENT
{
    uint32_t event_id;
    uint32_t param;
} COS_EVENT;

#define HEAP_REGION_SIZE 0x8000 //0x04000
//extern HeapRegion_t g_heap_regions[];
extern uint8_t g_heap_region_buff[HEAP_REGION_SIZE];
extern TASK_HANDLE g_task_list[TOTAL_MODULES];

/**function point type*/
typedef void (*PTASK_ENTRY)(void *parameter);

/**QueueHandle_t*/
typedef void *COS_SEMAPHORE_HANDLE;

/// to allow not using cos.h but sx direclty for timers
#define HVY_TIMER_IN (0xFFFFF000)

/************************************************************************************
 * COS TIMER FREERTOS FUNC DECLARATION                                               *
 ***********************************************************************************/
#define cos_timer_set_cnt hal_aon_timer_set_expiration_time
#define cos_timer_get_cnt hal_aon_timer_get_reminder_time
#define cos_timer_delay hal_timer_delay_cycle

/**
 * @brief  cos timer init
 * @param  none
 * @return none
 */
void cos_timer_initialise(void);

/**
 * @brief  cos timer set
 * @param  n_ms     interval
 * @param  callback_func    cos timer cb func
 * @param  callback_arg
 * @param  flags cos timer mode
 * @return none
 */
uint8_t cos_set_timer(uint32_t n_ms, void (*callback_func)(void *), void *callback_arg, uint8_t flags);

/**
 * @brief  cos timer change
 * @param  timer_handle     cos timer handler
 * @param  n_ms    interval
 * @return none
 */
bool cos_change_timer(uint8_t timer_handle, uint32_t n_ms);

/**
 * @brief  cos timer change
 * @param  timer_handle     cos timer handler
 * @param  secondes    interval
 * @return none
 */
bool cos_change_timer_use_second(uint8_t timer_handle, uint32_t secondes);

/**
 * @brief  stop and realse timer
 * @param  timer_handle     cos timer handler
 * @return none
 */
bool cos_kill_timer(uint8_t timer_handle);

/**
 * @brief  cos handle expired timer
 * @param  timer_handle     cos timer handler
 * @return none
 */
void cos_handle_expired_timers(uint8_t timer_handle);

/************************************************************************************
 * COS TASK FREERTOS FUNC DECLARATION                                              *
 ***********************************************************************************/
/**
 * @brief  Create task.
 * @param  p_task_entry  Pointer to the task entry function.  Tasks
 * must be implemented to never return (i.e. continuous loop).
 * @param  mod  task ID
 * @param  *p_parameter Pointer that will be used as the parameter for the task
 * being created.
 * @param  stack_size The size of the task stack specified as the number of
 * variables the stack can hold
 * @param  priority The priority at which the task should run.
 * @param  p_task_name A descriptive name for the task.
 * @return none
 */
uint32_t cos_create_task(
    PTASK_ENTRY p_task_entry,
    COS_MOD_ID mod,
    void *p_parameter,
    uint16_t stack_size,
    uint8_t priority,
    const char *p_task_name);

/**
 * @brief  Delete Task. Release Mailbox, task, and the handle.
 * @param  mod  task ID
 * @return none
 */
void cos_delete_task(COS_MOD_ID mod);

/**
 * @brief  Suspsend Task.
 * @param  mod  task ID
 * @return none
 */
uint32_t cos_suspend_task(COS_MOD_ID mod);

/**
 * @brief  Resume Task.
 * @param  mod  task ID
 * @return none
 */
bool cos_resume_task(COS_MOD_ID mod);

/**
 * @brief  Delay Task.
 * cos_delay_task() specifies a time at which the task wishes to unblock relative to
 * the time at which cos_delay_task() is called.  For example, specifying a block
 * period of 100 ms will cause the task to unblock ms after
 * cos_delay_task() is called.
 *
 * @param  n_ms  duration, by ms.
 * @return none
 *
 * Example:
 <pre>
 void example_task_entry(void *p_parameter)
 {
     COS_EVENT ev;

     while(1)
     {
         //delay 500ms before receiving messages from queue.
         cos_delay_task(500);
         cos_wait_event(MOD_APP, &ev, COS_WAIT_FOREVER);
     }
 }
 </pre>
 *
 * @ingroup cos_task
 */
void cos_delay_task(uint32_t n_ms);

/**
 * @brief  get task queue
 * @param  none
 * @return none
 */
void *cos_get_sys_task_queue(void);

/**
 * @brief  get task handler
 * @param  task_id task ID
 * @return task handler
 */
uint32_t cos_get_task_handler(COS_MOD_ID task_id);

/**
 * @brief  get current task handler
 * @param  none
 * @return task handler
 */
TASK_HANDLE *cos_get_current_task(void);

/**
 * @brief  get current task mod ID
 * @param  none
 * @return mod ID
 */
COS_MOD_ID cos_get_current_task_mod_id(void);

/**
 * @brief  cos crate queue
 * @param  ux_queue_length The maximum number of items that the queue can contain.
 * @param  ux_item_size The number of bytes each item in the queue will require.
 * @return NULL - queue create fail otherwise success
 */
void *cos_create_queue(const uint32_t ux_queue_length, const uint32_t ux_item_size);

/**
 * @brief  wait event
 * @param  mod  task ID
 * @param  p_event  task event,take event id and parameters
 * @param  time_out
 * @return true/false
 */
bool cos_wait_event(COS_MOD_ID mod, COS_EVENT *p_event, uint32_t time_out);

/**
 * @brief  Send message to the specified task.
 * @param  mod  task ID
 * @param  p_event  task event,take event id and parameters
 * @param  time_out
 * @param  option  priority of event send
 * @return true/false
 */
bool cos_send_event(COS_MOD_ID mod, COS_EVENT *p_event, uint32_t time_out, uint16_t option);

/**
 * @brief  check task if available
 * @param  mod  task ID
 * @return true/false
 */
bool cos_is_event_available(COS_MOD_ID mod);

/**
 * @brief  creat semaphore binary
 * @param  none
 * @return Handle to the created semaphore.
 *
 * @ingroup cos_semaphore
 */
COS_SEMAPHORE_HANDLE cos_create_semaphore_binary(void);

/**
 * @brief  creat semaphore counting
 *
 * @param max_count The maximum count value that can be reached.  When the
 *        semaphore reaches this value it can no longer be 'given'.
 *
 * @param init_count The count value assigned to the semaphore when it is
 *        created.
 *
 * @return Handle to the created semaphore.  Null if the semaphore could not be
 *         created.
 *
 *
 * @ingroup cos_semaphore
 */
COS_SEMAPHORE_HANDLE cos_create_semaphore_conunting(uint32_t max_count, uint32_t init_count);

/**
 * @brief  delete semaphore
 * @param  h_sem
 * @return true/false
 *
 * @ingroup cos_semaphore
 */
bool cos_delete_semaphore(COS_SEMAPHORE_HANDLE h_sem);

/**
 * @brief  wait for semaphore/mutex
 * @param  h_sem  get Specify the handle to a binary/counting semaphore or mutex
 * but not include recursive mutex
 * @param  time_out the time-out value
 * @return true/false
 *
 * @ingroup cos_semaphore
 */
bool cos_wait_for_semaphore(COS_SEMAPHORE_HANDLE h_sem, uint32_t time_out);

/**
 * @brief This API is used to  take a semaphore from an ISR.  The semaphore must have
 * previously been created with a call to cos_create_semaphore_binary() or
 * cos_create_semaphore_conunting().
 *
 * Mutex type semaphores (those created using a call to cos_create_recursive_mutex())
 * must not be used with this API.
 *
 * @param  h_sem  get Specify the handle to a binary/counting semaphore or mutex
 * but not include recursive mutex
 * @param  time_out the time-out value
 * @return true/false
 *
 * @ingroup cos_semaphore
 *
 */
bool cos_wait_for_semaphore_from_ISR(COS_SEMAPHORE_HANDLE h_sem, int32_t *pxHigherPriorityTaskWoken);

/**
 * @brief  release binary/counting semaphore or mutex, but not include recursive mutex
 * @param  h_sem  handle to the semaphore being released.
 * @return true/false
 *
 * @ingroup cos_semaphore
 */
bool cos_release_semaphore(COS_SEMAPHORE_HANDLE h_sem);

/**
 * @brief  to  take a semaphore from an ISR.  The semaphore must have
 * previously been created with a call to cos_create_semaphore_binary() or
 * cos_create_semaphore_conunting().
 *
 * Mutex type semaphores (those created using a call to cos_create_recursive_mutex())
 * must not be used with this API.
 *
 * @param  h_sem  handle to the semaphore being released.
 *
 * @param pxHigherPriorityTaskWoken xSemaphoreGiveFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if giving the semaphore caused a task
 * to unblock, and the unblocked task has a priority higher than the currently
 * running task.  If xSemaphoreGiveFromISR() sets this value to pdTRUE then
 * a context switch should be requested before the interrupt is exited.
 *
 * portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() can be invoked to request a context switch.
 *
 * @return TRUE if semaphore was successfully taken, otherwise FALSE
 *
 * @ingroup cos_semaphore
 */
bool cos_release_semaphore_from_ISR(COS_SEMAPHORE_HANDLE h_sem, int32_t *pxHigherPriorityTaskWoken);

/**
 * @brief  create a mutex
 * @param  none
 * @return Handle to the created semaphore.
 *
 * @ingroup cos_mutex
 */
COS_SEMAPHORE_HANDLE cos_create_mutex(void);

/**
 * @brief  create a recursive mutex
 * @param  none
 * @return TRUE/FALSE
 *
 * @ingroup cos_mutex
 */
COS_SEMAPHORE_HANDLE cos_create_recursive_mutex(void);

/**
 * @brief  get specified mutex
 * @param  handle handle to the mutex being obtained.
 * @param  time_out The time in ticks to wait for the semaphore to become available
 * @return TRUE-if the semaphore was obtained.
 *
 * @ingroup cos_mutex
 */
bool cos_get_recursive_mutex(COS_SEMAPHORE_HANDLE handle, uint32_t time_out);

/**
 * @brief  release specified recursive mutex
 * @param  handle handle to the mutex being released
 * @return TRUE-if the semaphore was given.
 *
 * @ingroup cos_mutex
 */
bool cos_release_recursive_mutex(COS_SEMAPHORE_HANDLE handle);

/**
 * @brief  idle task, handle low power
 * @param  none
 * @return none
 *
 * @ingroup cos_task
 */
void cos_app_idle_task(void);

/************************************************************************************
 * COS MEM FREERTOS FUNC DECLARATION                                               *
 ***********************************************************************************/
/**
 * @brief  cos mem init
 * @param  none
 * @return none
 */
void cos_mem_init(void);

/**
 * @brief  cos mem malloc
 * @param  size mem space size
 * @return none
 */
void *cos_malloc(uint32_t size);

/**
 * @brief  free cos mem
 * @param  ptr the allocated mem adr
 * @return none
 *
 * @ingroup cos_mem
 */
void cos_free(void *ptr);

/**
 * @brief  	rom code os func cb register
 * @param  	none
 * @return 	none
 */
void cos_cb_register(void);

/**
 * @brief  cos init
 * @param  none
 * @return none
 */
void cos_init(void);

/**
 * @brief  platform task creation
 * @param  none
 * @return task create TRUE OR FAIL
 */
bool cos_task_init(void);

#endif // H
