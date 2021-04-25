#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "bt_types.h"

#define OSI_MUTEX_MAX_TIMEOUT 0xffffffffUL

#define osi_mutex_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
#define osi_mutex_set_invalid( x ) ( ( *x ) = NULL )

typedef xSemaphoreHandle osi_mutex_t;

int osi_mutex_new(osi_mutex_t *mutex);

int osi_mutex_lock(osi_mutex_t *mutex);
int osi_mutex_wait_lock(osi_mutex_t *mutex, UINT32 timeout);

void osi_mutex_unlock(osi_mutex_t *mutex);

void osi_mutex_free(osi_mutex_t *mutex);

/* Just for a global mutex */
int osi_mutex_global_init(void);

void osi_mutex_global_deinit(void);

void osi_mutex_global_lock(void);

void osi_mutex_global_unlock(void);

#define  pMutexCreate(initState) (osi_mutex_t)xSemaphoreCreateRecursiveMutex()
#define  pMutexFree(mutex) vSemaphoreDelete((QueueHandle_t)mutex)
#define  pMutexLock(mutex) xSemaphoreTakeRecursive((QueueHandle_t)mutex, portMAX_DELAY)
#define  pMutexUnlock(mutex) xSemaphoreGiveRecursive((QueueHandle_t)mutex)

#endif /* __MUTEX_H__ */

