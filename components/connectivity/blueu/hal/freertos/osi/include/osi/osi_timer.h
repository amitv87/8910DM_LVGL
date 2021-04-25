#ifndef __OSI_TIMER_H__
#define __OSI_TIMER_H__


#include "FreeRTOS.h"
#include "timers.h"

typedef void (*timer_cb) (void *args);

typedef enum
{
    TIMER_MODE_SINGLE = 1,
    TIMER_MODE_PERIODIC,
} TIMER_TYPE_T;

typedef struct
{
    TimerHandle_t id;
    timer_cb cb;
    void *args;
    unsigned int type;
} osi_timer_t;


int osi_timer_init(void);
osi_timer_t *osi_timer_create(TIMER_TYPE_T type, unsigned int milli_seconds, timer_cb cb, void *args);
int osi_timer_change(osi_timer_t *timerm, unsigned int milli_seconds);
int osi_timer_cancel(osi_timer_t *timer);
void osi_sleep(unsigned int milli_secondes);
int osi_timer_reset(void);

#endif /* __OSI_TIMER_H__ */

