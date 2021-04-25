/**  
  @file
  charger_demo.c

  @brief
  quectel charger_demo.

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
15/01/2021        Neo         Init version
=================================================================*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_power.h"


/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_CHARGERDEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_CHARGERDEMO_LOG(msg, ...)         QL_LOG(QL_CHARGERDEMO_LOG_LEVEL, "ql_CHARGER", msg, ##__VA_ARGS__)
#define QL_CHARGERDEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("qlCHARGER", msg, ##__VA_ARGS__)


/*===========================================================================
 * Functions
 ===========================================================================*/
static void ql_charge_demo_thread(void *param)
{
    QL_CHARGERDEMO_LOG("charger demo thread enter, param 0x%x", param);

    uint32_t chg_vol = 0;
    uint32_t vbus_state = 0;
	uint32_t battery_vol = 0;
	uint32_t battery_level = 0;
	uint32_t charger_cur = 0;   
	uint32_t battemp_vol_mv = 0;
    uint32_t battery_state = 0;
    ql_charge_status_e charge_status;
    
    ql_set_battery_detect_channel(QL_CHARGE_ADC0);
    ql_battery_detect_enable(true);
	
	while(1)
	{
        ql_get_vchg_vol(&chg_vol);
        ql_get_vbus_state(&vbus_state);
        ql_get_battery_vol(&battery_vol);
        ql_get_battery_level(&battery_level);
        ql_get_charge_cur(&charger_cur);
        ql_get_battemp_vol_mv(&battemp_vol_mv);
        ql_get_battery_state(&battery_state);
        ql_get_charge_status(&charge_status);
		
		QL_CHARGERDEMO_LOG("battery_vol:%d, battery_level:%d\r\n", battery_vol, battery_level);
		QL_CHARGERDEMO_LOG("charger_status:%d, charger_cur:%d\r\n",charge_status,charger_cur);
		QL_CHARGERDEMO_LOG("battery_state:%d, battemp_vol_mv:%d\r\n",battery_state,battemp_vol_mv);
		QL_CHARGERDEMO_LOG("chg_vol:%d, vbus_state:%d\r\n",chg_vol, vbus_state);
		
		if(charge_status==QL_CHG_IDLE)
		{
            QL_CHARGERDEMO_LOG("battery idle");
		}	
		else 
		{
            QL_CHARGERDEMO_LOG("battery charging");
		}		

		ql_rtos_task_sleep_s(5);
	}

    ql_rtos_task_delete(NULL);
}

void ql_charge_app_init(void)
{
    QlOSStatus err = QL_SUCCESS;
    ql_task_t charge_task = NULL;
    err = ql_rtos_task_create(&charge_task, 1024, APP_PRIORITY_NORMAL, "ql_chargedemo", ql_charge_demo_thread, NULL, 1);
	if( err != QL_SUCCESS )
    {
        QL_CHARGERDEMO_LOG("charger demo task created failed");
    }
}


