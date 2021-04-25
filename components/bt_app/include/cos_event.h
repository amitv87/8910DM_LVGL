/**
 * @file    cos_event.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#ifndef __COS_EVENT_H__
#define __COS_EVENT_H__

#define EV_ID_SYS 0   /* system event      */
#define EV_ID_AT 1    /* at event          */
#define EV_ID_CTRL 2  /* controller event  */
#define EV_ID_HOST 3  /* host event        */
#define EV_ID_AUDIO 4 /* audio event       */

#define EV_ID_MAX 255 /* max event         */

#define EVENT_STAT_MASK(id) ((id) << 24)

/* thease event are handled by the system module */
enum
{
    EV_OS_EXCLUSIVE = EVENT_STAT_MASK(EV_ID_SYS),
    EV_TIMER,
};

/* thease event are handled by the AT module */
enum
{
    EV_AT_CMD_RECV = EVENT_STAT_MASK(EV_ID_AT),
    EV_DIAG_CMD_RECV,
    EV_DIAG_MIC_DMA,
    EV_BLE_UART_SEND
};

/* thease event are handled by the controller module */
enum
{
    EV_BT_CTRL_NOTIFY_IND = EVENT_STAT_MASK(EV_ID_CTRL),
};

#endif // _H_
