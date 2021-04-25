#include "osi_mutex.h"


/* static section */
static osi_mutex_t gl_mutex; /* Recursive Type */


int osi_mutex_new(osi_mutex_t *mutex)
{
    int xReturn = -1;

    *mutex = xSemaphoreCreateMutex();

    if (*mutex != NULL)
    {
        xReturn = 0;
    }

    return xReturn;
}

int osi_mutex_wait_lock(osi_mutex_t *mutex, UINT32 timeout)
{
    int ret = 0;

    if (timeout == OSI_MUTEX_MAX_TIMEOUT)
    {
        if (xSemaphoreTake(*mutex, portMAX_DELAY) != pdTRUE)
        {
            ret = -1;
        }
    }
    else
    {
        if (xSemaphoreTake(*mutex, timeout / portTICK_PERIOD_MS) != pdTRUE)
        {
            ret = -2;
        }
    }

    return ret;
}


int osi_mutex_lock(osi_mutex_t *mutex)
{
    return osi_mutex_wait_lock(mutex, OSI_MUTEX_MAX_TIMEOUT);
}



void osi_mutex_unlock(osi_mutex_t *mutex)
{
    xSemaphoreGive(*mutex);
}


void osi_mutex_free(osi_mutex_t *mutex)
{
    vSemaphoreDelete(*mutex);
    *mutex = NULL;
}

int osi_mutex_global_init(void)
{
    gl_mutex = xSemaphoreCreateRecursiveMutex();

    if (gl_mutex == NULL)
    {
        return -1;
    }

    return 0;
}

void osi_mutex_global_deinit(void)
{
    vSemaphoreDelete(gl_mutex);
}

void osi_mutex_global_lock(void)
{
    xSemaphoreTakeRecursive(gl_mutex, portMAX_DELAY);
}

void osi_mutex_global_unlock(void)
{
    xSemaphoreGiveRecursive(gl_mutex);
}
