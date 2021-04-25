/**  
  @file
  keypad_demo.c

  @brief
  quectel keypad_demo.

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
16/11/2020        Neo         Init version
=================================================================*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_keypad.h"
#include "ql_api_osi.h"
#include "ql_log.h"
#include "keypad_demo.h"


/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_KEYPADDEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_KEYPADDEMO_LOG(msg, ...)         QL_LOG(QL_KEYPADDEMO_LOG_LEVEL, "ql_KEYPADDEMO", msg, ##__VA_ARGS__)
#define QL_KEYPADDEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("ql_KEYPADDEMO", msg, ##__VA_ARGS__)


/*========================================================================
 *  function Definition
 *========================================================================*/
void ql_keypad_callback(ql_keymatrix_t keymatrix)
{   
    QL_KEYPADDEMO_LOG("keymap:%d keyout:%d keyin:%d pressd:%d", keymatrix.keymap, keymatrix.keyout, keymatrix.keyin, keymatrix.keystate);
}

void ql_keypad_demo_thread(void *param)
{
    QlOSStatus err = 0;
    uint32_t pressed;

    ql_keypad_out_e row[QL_KEYPAD_ROW_LENGTH] = {QL_KP_OUT0, QL_KP_OUT1, QL_KP_OUT2, QL_KP_OUT3, QL_KP_OUT4, QL_KP_OUT5};
	ql_keypad_in_e  col[QL_KEYPAD_COL_LENGTH] = {QL_KP_IN1,QL_KP_IN2, QL_KP_IN3, QL_KP_IN_NO_VALID, QL_KP_IN_NO_VALID};

    err = ql_keypad_init(ql_keypad_callback, row, col);
    
    if(err == QL_KEYPAD_SUCCESS)
    {
        while(1)
        {
            printf("keypad_test \r\n"); 
            err = ql_keypad_state(&pressed, QL_KEY_MAP_17);
            if(err == QL_KEYPAD_SUCCESS)
            {
                printf("keymap17 pressed: %d", pressed);
            }
            else 
            {
                printf("keymap id read error!");
                return;
            }
            ql_rtos_task_sleep_s(3);
        }  
    }
    
    err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_KEYPADDEMO_LOG("task deleted failed");
	}

}

void ql_keypad_app_init()
{
    QlOSStatus err = QL_OSI_SUCCESS;
    ql_task_t keypad_task = NULL;

    err = ql_rtos_task_create(&keypad_task, 1024, APP_PRIORITY_NORMAL, "ql_keypaddemo", ql_keypad_demo_thread, NULL, 1);
    if( err != QL_OSI_SUCCESS )
    {
        QL_KEYPADDEMO_LOG("keypad demo task created failed");
    }
}


