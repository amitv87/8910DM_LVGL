#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define OSI_SEM_MAX_TIMEOUT 0xffffffffUL

//#define POSIX_INTERFACE

#ifdef POSIX_INTERFACE
#include <semaphore.h>
typedef sem_t osi_sem_t;
#else
#include "semphr.h"
typedef xSemaphoreHandle osi_sem_t;
#endif



#define osi_sem_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
#define osi_sem_set_invalid( x ) ( ( *x ) = NULL )

int osi_sem_new(osi_sem_t *sem, uint32_t max_count, uint32_t init_count);

void osi_sem_free(osi_sem_t *sem);

int osi_sem_take(osi_sem_t *sem, uint32_t timeout);

void osi_sem_give(osi_sem_t *sem);


#endif /* __SEMAPHORE_H__ */
