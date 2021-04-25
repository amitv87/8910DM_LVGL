#ifndef _FIXED_QUEUE_H_
#define _FIXED_QUEUE_H_

#include <stdbool.h>
#include "osi_list.h"
#include "osi_semaphore.h"
#include "osi_mutex.h"


#ifndef QUEUE_SIZE_MAX
#define QUEUE_SIZE_MAX                    254
#endif

struct fixed_queue_t;
typedef void (*fixed_queue_cb)(struct fixed_queue_t *queue);

typedef struct fixed_queue_t
{

    list_t *list;
    osi_sem_t enqueue_sem;
    osi_sem_t dequeue_sem;
    osi_mutex_t lock;
    size_t capacity;

    fixed_queue_cb dequeue_ready;
} fixed_queue_t;



typedef void (*fixed_queue_free_cb)(void *data);

fixed_queue_t *fixed_queue_new(size_t capacity);


void fixed_queue_free(fixed_queue_t *queue, fixed_queue_free_cb free_cb);


bool fixed_queue_is_empty(fixed_queue_t *queue);


size_t fixed_queue_length(fixed_queue_t *queue);


size_t fixed_queue_capacity(fixed_queue_t *queue);


void fixed_queue_enqueue(fixed_queue_t *queue, void *data);


void *fixed_queue_dequeue(fixed_queue_t *queue);


bool fixed_queue_try_enqueue(fixed_queue_t *queue, void *data);


void *fixed_queue_try_dequeue(fixed_queue_t *queue);


void *fixed_queue_try_peek_first(fixed_queue_t *queue);


void *fixed_queue_try_peek_last(fixed_queue_t *queue);


void *fixed_queue_try_remove_from_queue(fixed_queue_t *queue, void *data);


list_t *fixed_queue_get_list(fixed_queue_t *queue);


void fixed_queue_register_dequeue(fixed_queue_t *queue, fixed_queue_cb ready_cb);


void fixed_queue_unregister_dequeue(fixed_queue_t *queue);

void fixed_queue_process(fixed_queue_t *queue);

list_t *fixed_queue_get_list(fixed_queue_t *queue);

#endif
