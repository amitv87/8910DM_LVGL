#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#include "bt_types.h"
#include "osi.h"
#include "osi_thread.h"
#include "log.h"

#define EV_SYS_BASE                   0
#define EV_OS_EXCLUSIVE               (EV_SYS_BASE+12)

#define OSI_EVENT_PRI_NORMAL   0
#define OSI_EVENT_PRI_URGENT   1

#define IS_INTERRUPT_SET  (1)

typedef struct _TaskDesc
{
    void (*TaskBody)(void *);
    const char *Name;
    UINT16 stack_size;
    UINT8 priority;
} TaskDesc;

typedef struct _TASK_HANDLE
{
    TaskDesc sTaskDesc;
    OSI_MOD_ID    taskMod;
    void         *queue;
    void         *taskHandle;
    BOOL           exclusive;
} TASK_HANDLE;

static TASK_HANDLE btu_task_list[TOTAL_MODULES];
UINT32 g_hCosMmiTask = 0;
void *app_queue = NULL;
void *bt_queue = NULL;

BOOL osi_wait_event(
    OSI_MOD_ID mod,
    osi_event_t *event,
    UINT32 timeout
)
{
    TASK_HANDLE *pHTask  = &(btu_task_list[mod]);
    UINT32 Evt [4] = {0, 0, 0, 0};
    BOOL ret = FALSE;

    //BD_PLT_CALL_3PAR_RTN(RTOS_COS_WAIT_EVENT, G3, TRUE, (void*)(UINT32)mod, (void*)&event, (void*)timeout);

    if (timeout == OSI_WAIT_FOREVER)
    {

        if (!pHTask->exclusive)
        {
            xQueueReceive(pHTask->queue, &Evt, timeout);
        }
        else
        {
            if (event)
            {
                memset(event, 0, sizeof(osi_event_t));
                event->nEventId = EV_OS_EXCLUSIVE;
            }

            return TRUE;
        }

    }
    else
    {
        return FALSE;
    }

    event->nEventId = Evt[0];
    event->nParam1  = Evt[1];

    return ret;
}

BOOL osi_send_event(
    OSI_MOD_ID  mod,
    osi_event_t *event,
    UINT32 timeout,
    UINT16 option
)
{
    TASK_HANDLE *pHTask  = &(btu_task_list[mod]);

    //BD_PLT_CALL_4PAR_RTN(RTOS_COS_SEND_EVENT, G3, TRUE, (void*)(UINT32)mod, (void*)&event, (void*)timeout, (void*)(UINT32)option);

    if ( pHTask == NULL || event == NULL || NULL == pHTask->queue)
    {
        OSI_LOGE(0,"osi_send_event pHTask:%p, event:%p, queue:%p\n\r", pHTask, event, pHTask->queue);
        return FALSE;
    }

    OSI_LOGI(0, "[BT TASK] send evtid: 0x%x", event->nEventId);

    if (!IS_INTERRUPT_SET)
    {
        if (xQueueSendToFrontFromISR(pHTask->queue, ( void * )event, NULL) == FALSE)
        {
            OSI_LOGI(0,"xQueueSendToFrontFromISR failed %d\n\r", pHTask->taskMod);
        }
    }
    else
    {
        if (option == OSI_EVENT_PRI_URGENT)
        {
            if (xQueueSendToFront(pHTask->queue, ( void * )event, timeout) == FALSE)
            {
                OSI_LOGI(0,"xQueueSendToFrontFromISR failed %d\n\r", pHTask->taskMod);
            }
        }
        else
        {
            if (xQueueSend(pHTask->queue, ( void * )event,  timeout) == FALSE)
            {
                OSI_LOGI(0,"xQueueSend failed %d\n\r", pHTask->taskMod);
            }
        }
    }

    return TRUE;
}


UINT32 osi_thread_create(
    thread_cb cb,
    OSI_MOD_ID mod,
    void *args,
    UINT16 stack_size,
    UINT8 priority,
    UINT16 flags,
    const INT8 *name
)
{
    TASK_HANDLE *pHTask = NULL;
    unsigned int status = 0x00;

    //BT_ERR("COS_CreateTask mod = %d \n", mod);

    pHTask = &(btu_task_list[mod]);

    pHTask->sTaskDesc.Name = name;

    if (name == NULL)
    {
        pHTask->sTaskDesc.Name = "noname";
    }

    pHTask->sTaskDesc.priority   = priority;
    pHTask->sTaskDesc.stack_size  = stack_size;
    pHTask->sTaskDesc.TaskBody    = cb;
    pHTask->taskMod = mod;
    pHTask->exclusive = false;


    pHTask->queue = xQueueCreate(30, ( UBaseType_t ) sizeof(osi_event_t)); //40

    if (!pHTask->queue)
    {
        OSI_LOGE(0,"BT create queue faile\n");
    }

    status = xTaskCreate(cb, pHTask->sTaskDesc.Name, stack_size, ( void * ) args, priority,  (TaskHandle_t *)&pHTask->taskHandle);

    if (status)
    {
        //BT_INF("create task ok status = %d\n", status);
    }
    else
    {
        OSI_LOGW(0,"BT create task faile status = %d\n", status);
    }


    if (flags & OSI_CREATE_SUSPENDED)
    {
        return (HANDLE)NULL;
    }
    else if (flags & OSI_CREATE_PREEMPT)
    {
        return (HANDLE)NULL;
    }
    else if (flags & OSI_CREATE_NO_PREEMPT)
    {
        return (HANDLE)NULL;
    }

    if ( MOD_TYPE_BT ==  mod)
    {
        bt_queue = pHTask->queue;
    }

    return (HANDLE)pHTask;
}

