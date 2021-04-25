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

#ifndef _OSI_API_INSIDE_H_
#define _OSI_API_INSIDE_H_

#include "osi_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: It is not recommended to use APIs in this file, unless you really
// know what you are doing.

/**
 * \brief opaque data struct for static semaphore
 */
typedef osiSemaphore_t osiSemaphoreStatic_t;

/**
 * thread state
 */
typedef enum
{
    OSI_THREAD_STATE_RUNNING,   ///< running
    OSI_THREAD_STATE_READY,     ///< ready, and there are higher priority task in running
    OSI_THREAD_STATE_BLOCKED,   ///< waiting temporal or external events
    OSI_THREAD_STATE_SUSPENDED, ///< suspend explicitly
    OSI_THREAD_STATE_DELETED,   ///< deleted but not release resources
    OSI_THREAD_STATE_INVALID,   ///< placeholder for invalid
} osiThreadState_t;

/**
 * thread status
 */
typedef struct osiThreadStatus
{
    void *handler;               ///< implementation dependent handler
    const char *name;            ///< thread name
    unsigned thread_number;      ///< numeric number
    osiThreadState_t state;      ///< state
    unsigned curr_priority;      ///< current priority, may be inherited
    unsigned base_priority;      ///< base priority
    void *stack_base;            ///< stack base address (lowest address)
    unsigned stack_alloc_size;   ///< satck allocation size
    unsigned stack_min_remained; ///< minimal unused stack size
} osiThreadStatus_t;

/**
 * \brief semaphore data struct size
 *
 * This returns semaphore implementation size at runtime. It is by design
 * not to expose semaphore size in compiling time to ease binary
 * compatibility.
 *
 * \return
 *      - semaphore implementation size
 */
unsigned osiSemaphoreSize(void);

/**
 * \brief create a semaphore with static memory
 *
 * When the semaphore won't be used later, \p osiSemaphoreDelete must be
 * called still to avoid resource leakage.
 *
 * Usually, the return pointer is the same as \p buf.
 *
 * Due to the implementation size is not exposed, \p alloca is needed to
 * use static semaphore. For example:
 *
 * \code{cpp}
 * osiSemaphoreStatic_t *buf_sema = alloca(osiSemaphoreSize());
 * osiSemaphore_t *sema = osiSemaphoreCreateStatic(buf_sema, 1, 0);
 * // ......
 * osiSemaphoreDelete(sema);
 * \endcode
 *
 * \param buf           static memory for semaphore
 * \param max_count     maximum count of the semaphore
 * \param init_count    initial count of the semaphore
 * \return
 *      - semaphore pointer
 *      - NULL on invalid parameter
 */
osiSemaphore_t *osiSemaphoreCreateStatic(osiSemaphoreStatic_t *buf, uint32_t max_count, uint32_t init_count);

/**
 * \brief opaque data struct for static timer
 */
typedef osiTimer_t osiTimerStatic_t;

/**
 * \brief timer data struct size
 *
 * This returns timer implementation size at runtime. It is by design not
 * to expose timer size in compiling time to ease binary compatibility.
 *
 * \return
 *      - timer implementation size
 */
unsigned osiTimerSize(void);

/**
 * \brief create a timer with static memory
 *
 * When the timer won't be used later, \p osiTimerDelete must be called
 * still to avoid resource leakage.
 *
 * Usually, the return pointer is the same as \p buf.
 *
 * Due to the implementation size is not exposed, \p alloca is needed to
 * use static timer. For example:
 *
 * \code{cpp}
 * osiTimerStatic_t *buf_timer = alloca(osiTimerSize());
 * osiTimer_t *timer = osiTimerCreateStatic(buf_timer, ...);
 * // ......
 * osiTimerDelete(timer);
 * \endcode
 *
 * \param buf           static memory for timer
 * \param thread        thread to execute the callback
 * \param cb            callback to be executed after timer expire
 * \param ctx           callback context
 * \return
 *      - the created timer instance
 *      - NULL at invalid parameter
 */
osiTimer_t *osiTimerCreateStatic(osiTimerStatic_t *buf, osiThread_t *thread, osiCallback_t cb, void *ctx);

/**
 * \brief start a timer with relaxed timeout, in unit of hardware tick
 *
 * **Don't** call this in application code. It is only for legacy codes.
 *
 * The frequency of hardware tick is chip dependent, and implementation dependent.
 *
 * \param timer     the timer to be started
 * \param ticks     normal timeout period in hardware tick
 * \param relax_ticks   relaxed timeout period in hardware tick
 * \return
 *      - true on success
 *      - false on invalid parameter
 */
bool osiTimerStartHWTickRelaxed(osiTimer_t *timer, int64_t ticks, int64_t relax_ticks);

/**
 * \brief dump timer information to memory
 *
 * It is for debug only. The data format of timer information dump is
 * not stable, and may change. When \p mem is NULL, it will return the
 * estimated dump size.
 *
 * \param mem       memory for timer information dump
 * \param size      provided memory size
 * \return
 *      - dump memory size
 *      - -1 if memory size of not enough
 */
int osiTimerDump(void *mem, unsigned size);

/**
 * \brief monoclinic system hardware tick
 *
 * **Don't** call this in application code. It is only for legacy codes.
 *
 * The frequency of hardware tick is chip dependent, and implementation
 * dependent.
 *
 * \return      monoclinic system hardware tick
 */
int64_t osiUpHWTick(void);

/**
 * \brief monoclinic system hardware tick
 *
 * **Don't** call this in application code. It is only for legacy codes.
 *
 * Similar to \p osiUpHWTick, just with less instruction. It can be used
 * when performance is really cared.
 *
 * \return      monoclinic system hardware tick
 */
uint32_t osiUpHWTick32(void);

/**
 * \brief hardware tick count in 16384Hz
 *
 * The return value should be fulll 32bits value. That is, the next tick
 * of 0xffffffff will be 0. Then the simple substract will always provide
 * the tick delta.
 *
 * This is only for legacy codes. \p osiUpTime or \p osiUpTimeUS are
 * recommended to get monoclinic time, and \p osiHWTick32 is recommended
 * for timing measrement when performance is sensitive.
 *
 * \return      hardware tick count
 */
uint32_t osiHWTick16K(void);

/**
 * \brief dump PM source information to memory
 *
 * It is for debug only. The data format of timer information dump is
 * not stable, and may change. When \p mem is NULL, it will return the
 * estimated dump size.
 *
 * \param mem       memory for PM source information dump
 * \param size      provided memory size
 * \return
 *      - dump memory size
 *      - -1 if memory size of not enough
 */
int osiPmSourceDump(void *mem, unsigned size);

/**
 * \brief dump interrupt information to memory
 *
 * It is for debug only. The data format of interrupt information dump is
 * not stable, and may change. When \p mem is NULL, it will return the
 * estimated dump size.
 *
 * \param mem       memory for interrupt information dump
 * \param size      provided memory size
 * \return
 *      - dump memory size
 *      - -1 if memory size of not enough
 */
int osiIrqDump(void *mem, unsigned size);

/**
 * get current thread count
 *
 * \return thread count
 */
uint32_t osiThreadCount(void);

/**
 * get all thread status
 *
 * The memory of \p status should be enough to hold thread status of \p count.
 *
 * \param status    memory for thread status output
 * \param count     maximum thread count
 * \return  actual filled thread count
 */
int osiThreadGetAllStatus(osiThreadStatus_t *status, uint32_t count);

#ifdef __cplusplus
}
#endif
#endif
