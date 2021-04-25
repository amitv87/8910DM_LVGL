#ifndef __OSI_THREAD_H__
#define __OSI_THREAD_H__

#include "bt_types.h"
#include "osi_api.h"
#define OSI_NO_WAIT             0x0

#define OSI_MAX_PRIORITY        20
#define OSI_CREATE_DEFAULT      0
#define OSI_CREATE_SUSPENDED    1
#define OSI_CREATE_PREEMPT      2
#define OSI_CREATE_NO_PREEMPT   3

#define COS_WAIT_FOREVER        0xFFFFFFFF
#define COS_NO_WAIT             0x0

#define COS_EVENT_PRI_NORMAL    0
#define COS_EVENT_PRI_URGENT    1

#define EV_BT_BASE              (11000)

typedef void (*thread_cb) (void *args);

typedef enum
{
    MOD_TYPE_APP,
    MOD_TYPE_BT,
    TOTAL_MODULES
} OSI_MOD_ID;


typedef struct
{
    UINT32 nEventId;
    UINT32 nParam1;
} osi_event_t;

UINT32 osi_thread_create(
    thread_cb cb,
    OSI_MOD_ID mod,
    void *args,
    UINT16 stack_size,
    UINT8 priority,
    UINT16 flags,
    const INT8 *name
);

BOOL osi_wait_event(
    OSI_MOD_ID mod,
    osi_event_t *pEvent,
    UINT32 timeout
);

BOOL osi_send_event(
    OSI_MOD_ID  mod,
    osi_event_t *event,
    UINT32 timeout,
    UINT16 option
);


#endif /* __OSI_THREAD_H__ */

