#include "osi_semaphore.h"

int osi_sem_new(osi_sem_t *sem, uint32_t max_count, uint32_t init_count)
{
    int ret = -1;

#ifdef POSIX_INTERFACE
    sem_init(sem, max_count, init_count);
#else

    if (sem)
    {
        *sem = xSemaphoreCreateCounting(max_count, init_count);

        if ((*sem) != NULL)
        {
            ret = 0;
        }
    }

#endif

    return ret;
}

/*-----------------------------------------------------------------------------------*/
// Give a semaphore
void osi_sem_give(osi_sem_t *sem)
{
#ifdef POSIX_INTERFACE
    sem_post(sem);
#else
    xSemaphoreGive(*sem);
#endif
}

/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

*/
int osi_sem_take(osi_sem_t *sem, uint32_t timeout)
{
    int ret = 0;

#ifdef POSIX_INTERFACE

    if (timeout ==  OSI_SEM_MAX_TIMEOUT)
    {
        ret = sem_wait(sem);
    }
    else
    {
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);

        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000;

        ret = sem_timedwait(sem, &ts);
    }

#else

    if (timeout ==  OSI_SEM_MAX_TIMEOUT)
    {
        if (xSemaphoreTake(*sem, portMAX_DELAY) != pdTRUE)
        {
            ret = -1;
        }
    }
    else
    {
        if (xSemaphoreTake(*sem, timeout / portTICK_PERIOD_MS) != pdTRUE)
        {
            ret = -2;
        }
    }

#endif

    return ret;
}

// Deallocates a semaphore
void osi_sem_free(osi_sem_t *sem)
{
#ifdef POSIX_INTERFACE
    sem_destroy(sem);
#else
    vSemaphoreDelete(*sem);
    *sem = NULL;
#endif
}
