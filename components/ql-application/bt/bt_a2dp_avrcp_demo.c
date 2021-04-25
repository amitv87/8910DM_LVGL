/**  @file
  bt_a2dp_demo.c

  @brief
  This file is demo of bt a2dp.

*/

/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/

/*===========================================================================
 * include files
 ===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_api_bt.h"
#include "ql_api_bt_a2dp.h"
#include "ql_api_bt_avrcp.h"
#include "ql_log.h"
#include "bt_demo.h"
#include "bt_a2dp_avrcp_demo.h"
/*===========================================================================
 * Macro Definition
 ===========================================================================*/
 
#define QL_A2DP_AVRCP_DEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_A2DP_AVRCP_DEMO_LOG(msg, ...)         QL_LOG(QL_A2DP_AVRCP_DEMO_LOG_LEVEL, "QL_A2DP_AVRCP_DEMO", msg, ##__VA_ARGS__)
#define QL_A2DP_AVRCP_DEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("QL_A2DP_AVRCP_DEMO", msg, ##__VA_ARGS__)

#define QL_A2DP_AVRCP_TASK_STACK_SIZE     		(4*1024)
#define QL_A2DP_AVRCP_TASK_PRIO          	 	APP_PRIORITY_NORMAL
#define QL_A2DP_AVRCP_TASK_EVENT_CNT      		5


/*===========================================================================
 * Struct
 ===========================================================================*/

/*===========================================================================
 * Enum
 ===========================================================================*/

/*===========================================================================
 * Variate
 ===========================================================================*/

ql_task_t bt_a2dp_avrcp_task = NULL;
ql_bt_addr_s bt_local_addr = {{0x7e, 0x5c, 0x91, 0xa4, 0x40, 0xb4}};



/*===========================================================================
 * Functions
 ===========================================================================*/

static void ql_bt_a2dp_avrcp_notify_cb(void *ind_msg_buf, void *ctx)
{
    ql_event_t *ql_event = NULL;
    
    
    if (ind_msg_buf == NULL)
    {
        return ;
        
    }

    ql_event = (ql_event_t *)ind_msg_buf;
    if (ql_event->id != 0)
    {
        //ql_rtos_event_send(bt_a2dp_avrcp_task,ql_event);
    }
}

static void ql_a2dp_avrcp_demo_thread(void *argv)
{
    QlOSStatus err = QL_OSI_SUCCESS;
    ql_errcode_bt_e ret;
    QUEC_AVRCP_PLAYBACK_STATUS_e avrcp_state;
    ql_bt_ble_local_name_s bt_name={0};
    uint8_t vol;
    ql_bt_visible_mode_e scanmode = QL_BT_PAGE_AND_INQUIRY_ENABLE;
    unsigned char str_ptr[32];

    ret = ql_bt_a2dp_avrcp_init(ql_bt_a2dp_avrcp_notify_cb);
    if (ret != QL_BT_SUCCESS)
    {
        QL_A2DP_AVRCP_DEMO_LOG("init error");
        goto QL_BT_A2DP_AVRCP_NOT_INIT_EXIT;
    }

    ret = ql_bt_start();
    if (ret != QL_BT_SUCCESS)
    {
        QL_A2DP_AVRCP_DEMO_LOG("start error");
        goto QL_BT_A2DP_AVRCP_INIT_EXIT;
    }
    ql_rtos_task_sleep_ms(2000);
    memcpy(bt_name.name, "QL_A2DP_AVRCP_DEMO", 18);
    ret = ql_bt_ble_set_localname(bt_name);
    if (ret != QL_BT_SUCCESS)
    {
        QL_A2DP_AVRCP_DEMO_LOG("set name error");
        goto QL_BT_A2DP_AVRCP_INIT_EXIT;
    }
    
    ret = ql_classic_bt_set_scanmode(scanmode);
    if (ret != QL_BT_SUCCESS)
    {
        QL_A2DP_AVRCP_DEMO_LOG("set scanmode error");
        goto QL_BT_A2DP_AVRCP_INIT_EXIT;
    }
    while (1)
    {
        QL_A2DP_AVRCP_DEMO_LOG("checking connect");
        
        if((ql_bt_avrcp_connection_state_get() == QUEC_BTAVRCP_CONNECTION_STATE_CONNECTED) && (ql_bt_a2dp_connection_state_get() == QUEC_BTA2DP_CONNECTION_STATE_CONNECTED))
        {
            QL_A2DP_AVRCP_DEMO_LOG("connect success");
            QL_A2DP_AVRCP_DEMO_LOG("get address");
            ret = ql_bt_a2dp_get_addr(&bt_local_addr);
            if (ret == QL_BT_SUCCESS)
            {
                memset(str_ptr,0x00,sizeof(str_ptr));
                sprintf((char *)str_ptr, "%02x:%02x:%02x:%02x:%02x:%02x", bt_local_addr.addr[0], bt_local_addr.addr[1], bt_local_addr.addr[2], bt_local_addr.addr[3], bt_local_addr.addr[4], bt_local_addr.addr[5]);
                QL_A2DP_AVRCP_DEMO_LOG("addr=%s",str_ptr);
            }
            else 
            {
                QL_A2DP_AVRCP_DEMO_LOG("error=%x", ret);
            }
            
            while(1)
            {
                if((ql_bt_avrcp_connection_state_get() != QUEC_BTAVRCP_CONNECTION_STATE_CONNECTED) || (ql_bt_a2dp_connection_state_get() != QUEC_BTA2DP_CONNECTION_STATE_CONNECTED))
                {
                    QL_A2DP_AVRCP_DEMO_LOG("disconnect");
                    break;
                }
                QL_A2DP_AVRCP_DEMO_LOG("checking paly");
                ql_bt_avrcp_play_state_get(&avrcp_state);
                if(avrcp_state == QUEC_AVRCP_PLAYBACK_STATUS_PLAYING)
                {
                    QL_A2DP_AVRCP_DEMO_LOG("playing");
                    ql_rtos_task_sleep_ms(3000);
                    QL_A2DP_AVRCP_DEMO_LOG("pause");
                    ql_bt_avrcp_pause();
                    ql_rtos_task_sleep_ms(3000);
                    QL_A2DP_AVRCP_DEMO_LOG("start");
                    ql_bt_avrcp_start();
                    ql_rtos_task_sleep_ms(3000);
                    QL_A2DP_AVRCP_DEMO_LOG("previ");
                    ql_bt_avrcp_previ();
                    ql_rtos_task_sleep_ms(3000);
                    QL_A2DP_AVRCP_DEMO_LOG("next");
                    ql_bt_avrcp_next();
                    ql_rtos_task_sleep_ms(3000);
                    ql_bt_avrcp_vol_get(&vol);
                    QL_A2DP_AVRCP_DEMO_LOG("get vol %d", vol);
                    ql_rtos_task_sleep_ms(3000);
                    vol = 100;
                    ql_bt_avrcp_vol_set(vol);
                    QL_A2DP_AVRCP_DEMO_LOG("set vol %d", vol);
                    ql_rtos_task_sleep_ms(3000);
                    vol = 70;
                    ql_bt_avrcp_vol_set(vol);
                    QL_A2DP_AVRCP_DEMO_LOG("set vol %d", vol);
                    ql_rtos_task_sleep_ms(3000);
                    QL_A2DP_AVRCP_DEMO_LOG("end");
                    ql_bt_a2dp_disconnect(bt_local_addr);
                    ql_bt_a2dp_avrcp_release();
                    ql_bt_a2dp_avrcp_init(ql_bt_a2dp_avrcp_notify_cb);
                    break;
                }
                ql_rtos_task_sleep_s(1);
            }
        }
        ql_rtos_task_sleep_s(1);
    }

QL_BT_A2DP_AVRCP_INIT_EXIT:
    ql_bt_a2dp_avrcp_release();
    ql_bt_stop();
QL_BT_A2DP_AVRCP_NOT_INIT_EXIT:	
	err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_A2DP_AVRCP_DEMO_LOG("deleted failed");
	}
}

void ql_bt_a2dp_avrcp_app_init(void)
{
    QlOSStatus err = QL_OSI_SUCCESS;
    
    err = ql_rtos_task_create(&bt_a2dp_avrcp_task, QL_A2DP_AVRCP_TASK_STACK_SIZE, QL_A2DP_AVRCP_TASK_PRIO, "bt a2dp avrcp", ql_a2dp_avrcp_demo_thread, NULL, QL_A2DP_AVRCP_TASK_EVENT_CNT);
    if (err != QL_OSI_SUCCESS)
    {
        QL_A2DP_AVRCP_DEMO_LOG("Create bt audio task Fail");
    }
}



