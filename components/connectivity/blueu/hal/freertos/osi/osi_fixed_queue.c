#include "osi_allocator.h"
#include "osi_fixed_queue.h"
#include "osi_list.h"
#include "osi.h"
#include "osi_mutex.h"
#include "osi_semaphore.h"




fixed_queue_t *fixed_queue_new(size_t capacity)
{
    fixed_queue_t *ret = osi_malloc(sizeof(fixed_queue_t));

    if (!ret)
    {
        goto error;
    }

    osi_mutex_new(&ret->lock);
    ret->capacity = capacity;

    ret->list = list_new();

    if (!ret->list)
    {
        goto error;
    }


    osi_sem_new(&ret->enqueue_sem, capacity, capacity);

#ifndef POSIX_INTERFACE

    if (!ret->enqueue_sem)
    {
        goto error;
    }

#endif

    osi_sem_new(&ret->dequeue_sem, capacity, 0);

#ifndef POSIX_INTERFACE

    if (!ret->dequeue_sem)
    {
        goto error;
    }

#endif

    return ret;

error:
    ;
    fixed_queue_free(ret, NULL);
    return NULL;
}

void fixed_queue_free(fixed_queue_t *queue, fixed_queue_free_cb free_cb)
{
    const list_node_t *node;

    if (queue == NULL)
    {
        return;
    }

    fixed_queue_unregister_dequeue(queue);

    if (free_cb)
    {
        for (node = list_begin(queue->list); node != list_end(queue->list); node = list_next(node))
        {
            free_cb(list_node(node));
        }
    }

    list_free(queue->list);
    #if 1
    // bug memory leek -- 8910
    osi_free(queue->list);
    // bug -- end
    #endif
    osi_sem_free(&queue->enqueue_sem);
    osi_sem_free(&queue->dequeue_sem);
    osi_mutex_free(&queue->lock);
    osi_free(queue);
}

bool fixed_queue_is_empty(fixed_queue_t *queue)
{
    bool is_empty = false;

    if (queue == NULL)
    {
        return true;
    }

    osi_mutex_lock(&queue->lock);
    is_empty = list_is_empty(queue->list);
    osi_mutex_unlock(&queue->lock);

    return is_empty;
}

size_t fixed_queue_length(fixed_queue_t *queue)
{
    size_t length;

    if (queue == NULL)
    {
        return 0;
    }

    osi_mutex_lock(&queue->lock);
    length = list_length(queue->list);
    osi_mutex_unlock(&queue->lock);

    return length;
}
size_t fixed_queue_capacity(fixed_queue_t *queue)
{
    osi_assert(queue != NULL);

    return queue->capacity;
}

void fixed_queue_enqueue(fixed_queue_t *queue, void *data)
{
    osi_assert(queue != NULL);
    osi_assert(data != NULL);

    osi_sem_take(&queue->enqueue_sem, OSI_SEM_MAX_TIMEOUT);

    osi_mutex_lock(&queue->lock);

    list_append(queue->list, data);
    osi_mutex_unlock(&queue->lock);

    osi_sem_give(&queue->dequeue_sem);
}

void *fixed_queue_dequeue(fixed_queue_t *queue)
{
    void *ret = NULL;

    osi_assert(queue != NULL);

    osi_sem_take(&queue->dequeue_sem, OSI_SEM_MAX_TIMEOUT);

    osi_mutex_lock(&queue->lock);
    ret = list_front(queue->list);
    list_remove(queue->list, ret);
    osi_mutex_unlock(&queue->lock);

    osi_sem_give(&queue->enqueue_sem);

    return ret;
}

bool fixed_queue_try_enqueue(fixed_queue_t *queue, void *data)
{
    osi_assert(queue != NULL);
    osi_assert(data != NULL);

    if (osi_sem_take(&queue->enqueue_sem, 0) != 0)
    {
        return false;
    }

    osi_mutex_lock(&queue->lock);

    list_append(queue->list, data);
    osi_mutex_unlock(&queue->lock);

    osi_sem_give(&queue->dequeue_sem);

    return true;
}

void *fixed_queue_try_dequeue(fixed_queue_t *queue)
{
    void *ret = NULL;

    if (queue == NULL)
    {
        return NULL;
    }

    if (osi_sem_take(&queue->dequeue_sem, 0) != 0)
    {
        return NULL;
    }

    osi_mutex_lock(&queue->lock);
    ret = list_front(queue->list);
    list_remove(queue->list, ret);
    osi_mutex_unlock(&queue->lock);

    osi_sem_give(&queue->enqueue_sem);

    return ret;
}

void *fixed_queue_try_peek_first(fixed_queue_t *queue)
{
    void *ret = NULL;

    if (queue == NULL)
    {
        return NULL;
    }

    osi_mutex_lock(&queue->lock);
    ret = list_is_empty(queue->list) ? NULL : list_front(queue->list);
    osi_mutex_unlock(&queue->lock);

    return ret;
}

void *fixed_queue_try_peek_last(fixed_queue_t *queue)
{
    void *ret = NULL;

    if (queue == NULL)
    {
        return NULL;
    }

    osi_mutex_lock(&queue->lock);
    ret = list_is_empty(queue->list) ? NULL : list_back(queue->list);
    osi_mutex_unlock(&queue->lock);

    return ret;
}

void *fixed_queue_try_remove_from_queue(fixed_queue_t *queue, void *data)
{
    bool removed = false;

    if (queue == NULL)
    {
        return NULL;
    }

    osi_mutex_lock(&queue->lock);

    if (list_contains(queue->list, data) &&
            osi_sem_take(&queue->dequeue_sem, 0) == 0)
    {
        removed = list_remove(queue->list, data);
        osi_assert(removed);
    }

    osi_mutex_unlock(&queue->lock);

    if (removed)
    {
        osi_sem_give(&queue->enqueue_sem);
        return data;
    }

    return NULL;
}

list_t *fixed_queue_get_list(fixed_queue_t *queue)
{
    osi_assert(queue != NULL);

    // NOTE: This function is not thread safe, and there is no point for
    // calling osi_mutex_lock() / osi_mutex_unlock()
    return queue->list;
}

void fixed_queue_register_dequeue(fixed_queue_t *queue, fixed_queue_cb ready_cb)
{
    osi_assert(queue != NULL);
    osi_assert(ready_cb != NULL);

    queue->dequeue_ready = ready_cb;
}

void fixed_queue_unregister_dequeue(fixed_queue_t *queue)
{
    osi_assert(queue != NULL);

    queue->dequeue_ready = NULL;
}

void fixed_queue_process(fixed_queue_t *queue)
{
    osi_assert(queue != NULL);

    if (queue->dequeue_ready)
    {
        queue->dequeue_ready(queue);
    }
}

