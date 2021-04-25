#include "osi_timer.h"
#include "osi_allocator.h"
#include "osi_list.h"
#include "osi_mutex.h"
#include "bt_types.h"



static list_t *os_timer_list = NULL;
static osi_mutex_t timer_mutex;

int osi_timer_id_compare(void *element_t, void *xTimer_id)
{
    return ((osi_timer_t *)element_t)->id != (TimerHandle_t)xTimer_id;
}

void osi_timer_callback(TimerHandle_t xTimer)
{
    osi_timer_t *timer = NULL;
    list_node_t *node = NULL;

    osi_mutex_lock(&timer_mutex);
    node = list_foreach(os_timer_list, osi_timer_id_compare, xTimer);
    osi_mutex_unlock(&timer_mutex);

    if (node == NULL)
    {
        return;
    }

    timer = node->data;

    if (timer == NULL)
    {
        return;
    }

    timer->cb(timer->args);

    if (TIMER_MODE_SINGLE == timer->type)
    {
        osi_mutex_lock(&timer_mutex);
        list_remove(os_timer_list, timer);
        osi_mutex_unlock(&timer_mutex);
        osi_free(timer);
    }
}


osi_timer_t *osi_timer_create(TIMER_TYPE_T type, unsigned int milli_seconds, timer_cb cb, void *args)
{
    BaseType_t base_type = pdFALSE;

    osi_timer_t *timer = osi_malloc(sizeof(osi_timer_t));
    timer->cb = cb;
    timer->args = args;
    timer->type = type;

    if (TIMER_MODE_SINGLE == type)
    {
        base_type = pdFALSE;
    }
    else if (TIMER_MODE_PERIODIC == type)
    {
        base_type = pdTRUE;
    }

    timer->id = xTimerCreate(NULL, pdMS_TO_TICKS(milli_seconds), base_type, 0, (TimerCallbackFunction_t)(osi_timer_callback));
    xTimerStart(timer->id, 0);

    osi_mutex_lock(&timer_mutex);
    list_append(os_timer_list, timer);
    osi_mutex_unlock(&timer_mutex);

    return timer;
}

int osi_timer_change(osi_timer_t *timer, unsigned int milli_seconds)
{
    xTimerChangePeriod(timer->id, pdMS_TO_TICKS(milli_seconds), 0);
    return 0;
}

int osi_timer_cancel(osi_timer_t *timer)
{
    list_node_t *node = NULL;

    xTimerStop(timer->id, 0);
    xTimerDelete(timer->id, 0);

    osi_mutex_lock(&timer_mutex);

    node = list_foreach(os_timer_list, osi_timer_id_compare, timer->id);

	if(node)
	{
    	list_remove(os_timer_list, node->data);
    	osi_free(node->data);
	}

    osi_mutex_unlock(&timer_mutex);

    return 0;
}

int osi_timer_init(void)
{
    if (os_timer_list == NULL)
    {
        os_timer_list = list_new();
    }

    osi_mutex_new(&timer_mutex);

    return 0;
}

int osi_timer_reset(void)
{
    list_node_t *node = NULL;
    osi_timer_t *timer = NULL;
//    osi_mutex_lock(&timer_mutex);
    
    if(os_timer_list == NULL)
    {
        return 0;
    }

    node = os_timer_list->head;
    
    while(node != NULL)
    {
        timer = (osi_timer_t*)node->data;
        if(timer != NULL)
            osi_timer_cancel(timer);

        node = os_timer_list->head;
    }

    return 0;

//    osi_mutex_unlock(&timer_mutex);
}

void osi_sleep(unsigned   int  milli_secondes)
{
    vTaskDelay(milli_secondes);
}
