/*============================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------

=============================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_api_nw.h"
#include "ql_api_datacall.h"

#define QL_DATACALL_LOG_LEVEL	QL_LOG_LEVEL_INFO
#define QL_DATACALL_DEMO_LOG(msg, ...)			QL_LOG(QL_DATACALL_LOG_LEVEL, "ql_datacall_demo", msg, ##__VA_ARGS__)

typedef struct
{
	bool        state; 
	ql_sem_t    act_sem;
	ql_sem_t    deact_sem;
}datacall_context_s;

ql_task_t datacall_task = NULL;
ql_sem_t ql_data_reg_sem = NULL;
datacall_context_s datacall_ctx[PROFILE_IDX_NUM] = {0};


void ql_nw_ind_callback(uint8_t sim_id, unsigned int ind_type, void *ind_msg_buf)
{
    if(QUEC_NW_DATA_REG_STATUS_IND == ind_type)
    {
        ql_nw_common_reg_status_info_s  *data_reg_status=(ql_nw_common_reg_status_info_s  *)ind_msg_buf;
        QL_DATACALL_DEMO_LOG("Sim%d data reg status changed, current status is %d", sim_id, data_reg_status->state);
        if((QL_NW_REG_STATE_HOME_NETWORK == data_reg_status->state) || (QL_NW_REG_STATE_ROAMING == data_reg_status->state))
        {
            ql_rtos_semaphore_release(ql_data_reg_sem);
        }
    }
}

void ql_datacall_ind_callback(uint8_t sim_id, unsigned int ind_type, int profile_idx, bool result, void *ctx)
{
    QL_DATACALL_DEMO_LOG("nSim = %d, profile_idx=%d, ind_type=0x%x, result=%d", sim_id, profile_idx, ind_type, result);
    if((profile_idx < PROFILE_IDX_MIN) || (profile_idx > PROFILE_IDX_MAX))
    {
        return;
    }
    
    switch(ind_type)
    {
        case QUEC_DATACALL_ACT_RSP_IND:  //only received in asyn mode
        {
            datacall_ctx[profile_idx - 1].state = (result == true)? QL_PDP_ACTIVED:QL_PDP_DEACTIVED;
            ql_rtos_semaphore_release(datacall_ctx[profile_idx - 1].act_sem);
            break;
        }
        case QUEC_DATACALL_DEACT_RSP_IND:  //only received in asyn mode
        {
            datacall_ctx[profile_idx - 1].state = (result == true)? QL_PDP_DEACTIVED:QL_PDP_ACTIVED;
            ql_rtos_semaphore_release(datacall_ctx[profile_idx - 1].deact_sem);
            break;
        }
        case QUEC_DATACALL_PDP_DEACTIVE_IND:  //received in both asyn mode and sync mode
        {
            datacall_ctx[profile_idx - 1].state = QL_PDP_DEACTIVED;
            
            ql_event_t event={0};
            event.id = QUEC_DATACALL_PDP_DEACTIVE_IND;
            event.param1 = profile_idx;
            ql_rtos_event_send(datacall_task, &event);
            break;
        }
    }
}

static void datacall_app_thread(void * arg)
{
    int ret = 0, retry_count = 0;
	uint8_t sim_id = 0;
    int profile_idx = 1;   //range 1 to 7
    ql_nw_reg_status_info_s nw_info = {0};
    ql_data_call_info_s info ={0};
    ql_event_t event;

    ql_rtos_semaphore_create(&ql_data_reg_sem, 0);
    
    ret = ql_nw_register_cb(ql_nw_ind_callback);
    if(0 != ret)
    {
        QL_DATACALL_DEMO_LOG("ql_nw_register_cb err, ret=0x%x", ret);
        goto exit;
    }

    ret = ql_nw_get_reg_status(sim_id, &nw_info);
    QL_DATACALL_DEMO_LOG("ret: 0x%x, current data reg status=%d", ret, nw_info.data_reg.state);
    if((QL_NW_REG_STATE_HOME_NETWORK != nw_info.data_reg.state) && (QL_NW_REG_STATE_ROAMING != nw_info.data_reg.state))   
    {
        ql_rtos_semaphore_wait(ql_data_reg_sem, QL_WAIT_FOREVER);  //wait network registered
    }

    ret = ql_datacall_register_cb(sim_id, profile_idx, ql_datacall_ind_callback, NULL);
    QL_DATACALL_DEMO_LOG("ql_datacall_register_cb ret=0x%x", ret);
    
    ret = ql_set_data_call_asyn_mode(sim_id, profile_idx, 1);   //If set to asynchronous mode, act_sem and deact_sem are need to be created
    ql_rtos_semaphore_create(&datacall_ctx[profile_idx-1].act_sem, 0);
    ql_rtos_semaphore_create(&datacall_ctx[profile_idx-1].deact_sem, 0);

    ret=ql_start_data_call(sim_id, profile_idx, QL_PDP_TYPE_IP, "cmnet", NULL, NULL, 0);
    if(0 != ret)
    {
        QL_DATACALL_DEMO_LOG("ql_start_data_call err, ret=0x%x", ret);
        goto exit;
    }

    ql_rtos_semaphore_wait(datacall_ctx[profile_idx-1].act_sem, QL_WAIT_FOREVER);  //wait QUEC_DATACALL_ACT_RSP_IND
    QL_DATACALL_DEMO_LOG("datacall state=%d", datacall_ctx[profile_idx-1].state);

    ret = ql_get_data_call_info(sim_id, profile_idx, &info);
    QL_DATACALL_DEMO_LOG("ql_get_data_call_info ret: 0x%x", ret);
    QL_DATACALL_DEMO_LOG("info.profile_idx: %d, info.ip_version: %d", info.profile_idx, info.ip_version);
	QL_DATACALL_DEMO_LOG("info->v4.state: %d, info.v6.state: %d", info.v4.state, info.v6.state);
    if(info.v4.state)
    {
		QL_DATACALL_DEMO_LOG("info.v4.addr.ip: %s", ip4addr_ntoa(&(info.v4.addr.ip)));
		QL_DATACALL_DEMO_LOG("info.v4.addr.pri_dns: %s", ip4addr_ntoa(&(info.v4.addr.pri_dns)));
		QL_DATACALL_DEMO_LOG("info.v4.addr.sec_dns: %s", ip4addr_ntoa(&(info.v4.addr.sec_dns)));
    }
    if(info.v6.state)
    {
		QL_DATACALL_DEMO_LOG("info.v6.addr.ip: %s", ip6addr_ntoa(&(info.v6.addr.ip)));
		QL_DATACALL_DEMO_LOG("info.v6.addr.pri_dns: %s", ip6addr_ntoa(&(info.v6.addr.pri_dns)));
		QL_DATACALL_DEMO_LOG("info.v6.addr.sec_dns: %s", ip6addr_ntoa(&(info.v6.addr.sec_dns)));
    }
    
	while(1)
	{
		if(ql_event_try_wait(&event) != 0)
		{
			continue;
		}

        if(QUEC_DATACALL_PDP_DEACTIVE_IND == event.id)
        {
            ql_rtos_task_sleep_ms(5000);   //wait network status update

            ret = ql_nw_get_reg_status(sim_id, &nw_info);
            if((QL_NW_REG_STATE_HOME_NETWORK != nw_info.data_reg.state) && (QL_NW_REG_STATE_ROAMING != nw_info.data_reg.state))   
            {
                ql_rtos_semaphore_wait(ql_data_reg_sem, QL_WAIT_FOREVER);
            }

            ret = ql_set_data_call_asyn_mode(sim_id, profile_idx, 0);   //Set to sync mode
            //Try reconnect 10 times, time interval is 20 seconds
            while(((ret = ql_start_data_call(sim_id, profile_idx, QL_PDP_TYPE_IP, "cmnet", NULL, NULL, 0)) != QL_DATACALL_SUCCESS) && (retry_count < 10))
            {
            	retry_count++;
                QL_DATACALL_DEMO_LOG("gprs reconnect fail, the retry count is %d", retry_count);
        		ql_rtos_task_sleep_ms(20000);
        	}

            if(QL_DATACALL_SUCCESS == ret)
            {
                retry_count = 0;
                datacall_ctx[profile_idx - 1].state = QL_PDP_ACTIVED;
                ret = ql_get_data_call_info(sim_id, profile_idx, &info);
                QL_DATACALL_DEMO_LOG("ql_get_data_call_info ret: 0x%x", ret);
                QL_DATACALL_DEMO_LOG("info.profile_idx: %d, info.ip_version: %d", info.profile_idx, info.ip_version);
            	QL_DATACALL_DEMO_LOG("info->v4.state: %d, info.v6.state: %d", info.v4.state, info.v6.state);
                if(info.v4.state)
                {
            		QL_DATACALL_DEMO_LOG("info.v4.addr.ip: %s", ip4addr_ntoa(&(info.v4.addr.ip)));
            		QL_DATACALL_DEMO_LOG("info.v4.addr.pri_dns: %s", ip4addr_ntoa(&(info.v4.addr.pri_dns)));
            		QL_DATACALL_DEMO_LOG("info.v4.addr.sec_dns: %s", ip4addr_ntoa(&(info.v4.addr.sec_dns)));
                }
                if(info.v6.state)
                {
            		QL_DATACALL_DEMO_LOG("info.v6.addr.ip: %s", ip6addr_ntoa(&(info.v6.addr.ip)));
            		QL_DATACALL_DEMO_LOG("info.v6.addr.pri_dns: %s", ip6addr_ntoa(&(info.v6.addr.pri_dns)));
            		QL_DATACALL_DEMO_LOG("info.v6.addr.sec_dns: %s", ip6addr_ntoa(&(info.v6.addr.sec_dns)));
                }
            }
        }
	}
    
exit:
    ql_rtos_semaphore_delete(ql_data_reg_sem);
    ql_rtos_semaphore_delete(datacall_ctx[profile_idx-1].act_sem);
    ql_rtos_semaphore_delete(datacall_ctx[profile_idx-1].deact_sem);
	ql_datacall_unregister_cb(sim_id, profile_idx, ql_datacall_ind_callback, NULL);
    
    ql_rtos_task_delete(datacall_task);
}


void ql_datacall_app_init(void)
{
    QlOSStatus err = QL_OSI_SUCCESS;
    	
	err = ql_rtos_task_create(&datacall_task, 4*1024, APP_PRIORITY_NORMAL, "QDATACALLDEMO", datacall_app_thread, NULL, 10);
    if(err != QL_OSI_SUCCESS)
	{
		QL_DATACALL_DEMO_LOG("task created failed");
        return;
	}
    
}

