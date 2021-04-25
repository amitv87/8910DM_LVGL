#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_api_bt.h"
#include "ql_api_bt_hfp.h"

#include "ql_log.h"
#include "bt_demo.h"

#define QL_BT_HFP_LOG_LEVEL         	QL_LOG_LEVEL_INFO
#define QL_BT_HFP_LOG(msg, ...)			QL_LOG(QL_BT_HFP_LOG_LEVEL, "ql_BT_HFP_DEMO", msg, ##__VA_ARGS__)
#define QL_BT_HFP_LOG_PUSH(msg, ...)	QL_LOG_PUSH("ql_BT_HFP_DEMO", msg, ##__VA_ARGS__)

ql_task_t bt_hfp_demo_task = NULL;
static ql_bt_addr_s ql_bt_hfp_remote_addr = {{0x66, 0xCA, 0xC9, 0xA2, 0x3E, 0x38}};
static unsigned char ql_bt_hfp_volume = 15;  //range:1-15
static unsigned char ql_bt_hfp_phone_nubmer[] = "13249166530";
static unsigned char ql_bt_hfp_ctrl_cmd = 2;
static ql_bt_hfp_call_state_e ql_bt_hfp_call = QL_BT_HFP_CALL_NO_CALL_IN_PROGRESS;    
static ql_bt_hfp_connection_state_e ql_bt_hfp_connect_state = QL_BT_HFP_CONNECTION_STATE_DISCONNECTED;

extern ql_errcode_bt_e ql_bt_demo_start();
extern ql_errcode_bt_e ql_bt_demo_stop();
extern ql_errcode_bt_e ql_bt_demo_get_state();
extern ql_errcode_bt_e ql_bt_demo_get_local_name();
extern ql_errcode_bt_e ql_bt_demo_set_local_name();
extern ql_errcode_bt_e ql_bt_demo_get_scanmde();
extern ql_errcode_bt_e ql_be_demo_set_scanmode();

ql_errcode_bt_e ql_bt_hfp_demo_connect()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_connect(ql_bt_hfp_remote_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_hfp_demo_disconnect()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_disconnect(ql_bt_hfp_remote_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_hfp_demo_set_volume()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_set_volume(ql_bt_hfp_remote_addr, ql_bt_hfp_volume);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_hfp_demo_reject_call()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_reject_call(ql_bt_hfp_remote_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}


ql_errcode_bt_e ql_bt_hfp_demo_answer_accept_call()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_answer_accept_call(ql_bt_hfp_remote_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_hfp_demo_answer_reject_call()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_answer_reject_call(ql_bt_hfp_remote_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_hfp_demo_dial()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_dial(ql_bt_hfp_remote_addr, ql_bt_hfp_phone_nubmer);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_hfp_demo_redial()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_redial(ql_bt_hfp_remote_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_hfp_demo_vr_enable()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_vr_enable(ql_bt_hfp_remote_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_hfp_demo_vr_disable()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_vr_disable(ql_bt_hfp_remote_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_hfp_demo_three_way_call()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_hfp_ctrl_three_way_call(ql_bt_hfp_remote_addr, ql_bt_hfp_ctrl_cmd);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_HFP_LOG("sucess");
    }
    else
    {
        QL_BT_HFP_LOG("error=%x", ret);
    }

    return ret;
}




static void ql_bt_hfp_notify_cb(void *ind_msg_buf, void *ctx)
{
    ql_event_t *ql_event = NULL;
    
    if (ind_msg_buf == NULL)
    {
        return ;
        
    }

    ql_event = (ql_event_t *)ind_msg_buf;
    if (ql_event->id != 0)
    {
        ql_rtos_event_send(bt_hfp_demo_task,ql_event);
    }
}

static ql_errcode_bt_e ql_bt_hfp_event()
{
    ql_event_t test_event = {0};
    ql_errcode_bt_e ret = QL_BT_SUCCESS;
    QL_BT_STATUS status = (QL_BT_STATUS)(test_event.param1);
    if (ql_event_try_wait(&test_event) == 0)
	{
        if(test_event.id == 0)
        {
            return ret;
        }
        
        status = (QL_BT_STATUS)(test_event.param1);
        switch (test_event.id)
        {
            case QUEC_BT_START_STATUS_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("start sucess");
                    ret = ql_bt_demo_get_state();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_HFP_STOP;
                    }

                    ret = ql_bt_demo_get_local_name();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_HFP_STOP;
                    } 
                    ret = ql_bt_demo_set_local_name();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_HFP_STOP;
                    }
                    ret = ql_bt_demo_get_local_name();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_HFP_STOP;
                    }
                    
                    ret = ql_bt_demo_get_scanmde();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_HFP_STOP;
                    }
                    ret = ql_be_demo_set_scanmode();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_HFP_STOP;
                    }
                    ret = ql_bt_demo_get_scanmde();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_HFP_STOP;
                    }

                    //
                    
                }
                else
                {
                    QL_BT_HFP_LOG("start failed");
                    
                }
            }
            break;
            case QUEC_BT_STOP_STATUS_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("stop sucess");
                }
                else
                {
                    QL_BT_HFP_LOG("stop failed");
                }
            }
            break;
            case QUEC_BT_HFP_CONNECT_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("conn sucess");
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        QL_BT_HFP_LOG("addr=%02x%02x%02x%02x%02x%02x", hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 

                        memcpy(ql_bt_hfp_remote_addr.addr, hfp_info->addr.addr, QL_BT_MAC_ADDRESS_SIZE);
                        free(hfp_info);

                        ql_bt_hfp_connect_state = QL_BT_HFP_CONNECTION_STATE_CONNECTED;
                        
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("conn failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_DISCONNECT_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("disconn sucess");
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        QL_BT_HFP_LOG("addr=%02x%02x%02x%02x%02x%02x", hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 
                        
                        free(hfp_info);
                        
#if 1
                        if (ql_bt_hfp_connect_state == QL_BT_HFP_CONNECTION_STATE_DISCONNECTING)
                        {
                            ql_rtos_task_sleep_s(3);
                            ret = ql_bt_hfp_demo_connect();
                            if (ret != QL_BT_SUCCESS)
                            {
                                ql_bt_hfp_connect_state = QL_BT_HFP_CONNECTION_STATE_DISCONNECTED;
                                goto QL_BT_HFP_STOP;
                            }
                            else 
                            {
                                ql_bt_hfp_connect_state = QL_BT_HFP_CONNECTION_STATE_CONNECTING;
                                
                            }
                        }
#else
                        ql_bt_hfp_connect_state = QL_BT_HFP_CONNECTION_STATE_DISCONNECTED;
#endif
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("disconn failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_CALL_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("call sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        ql_bt_hfp_call_state_e call_state = (ql_bt_hfp_call_state_e)hfp_info->state;
                        QL_BT_HFP_LOG("call_state=%d,addr=%02x%02x%02x%02x%02x%02x", call_state, hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 

                        if (call_state == QL_BT_HFP_CALL_NO_CALL_IN_PROGRESS)
                        {
                            if (ql_bt_hfp_call == QL_BT_HFP_CALL_CALL_IN_PROGRESS)
                            {
                                ql_bt_hfp_call = call_state;
                                if (ql_bt_hfp_connect_state == QL_BT_HFP_CONNECTION_STATE_CONNECTED)
                                {
                                    ret = ql_bt_hfp_demo_disconnect();
                                    if (ret != QL_BT_SUCCESS)
                                    {
                                        goto QL_BT_HFP_STOP;
                                    }  
                                    else 
                                    {
                                        ql_bt_hfp_connect_state = QL_BT_HFP_CONNECTION_STATE_DISCONNECTING;
                                    }
                                }
                            }
                        }
                        else 
                        {
                            if (ql_bt_hfp_call == QL_BT_HFP_CALL_NO_CALL_IN_PROGRESS)
                            {
                                ql_bt_hfp_call = call_state;

                                ret = ql_bt_hfp_demo_set_volume();
                                if (ret != QL_BT_SUCCESS)
                                {
                                    goto QL_BT_HFP_STOP;
                                }
#if 0
                                ql_rtos_task_sleep_ms(100);
                                ret = ql_bt_hfp_demo_vr_disable();
                                if (ret != QL_BT_SUCCESS)
                                {
                                    goto QL_BT_HFP_STOP;
                                }

                                ql_rtos_task_sleep_ms(100);
                                ret = ql_bt_hfp_demo_vr_enable();
                                if (ret != QL_BT_SUCCESS)
                                {
                                    goto QL_BT_HFP_STOP;
                                }
#endif
                                //ql_rtos_task_sleep_ms(100);
                                //ret = ql_bt_hfp_demo_three_way_call();
                                //if (ret != QL_BT_SUCCESS)
                                //{
                                //    goto QL_BT_HFP_STOP;
                                //}

                                ql_rtos_task_sleep_s(30);
                                ret = ql_bt_hfp_demo_reject_call();
                                if (ret != QL_BT_SUCCESS)
                                {
                                    goto QL_BT_HFP_STOP;
                                }
                            }
                        }
                            
                        free(hfp_info);
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("call failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_CALL_SETUP_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("call set sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        ql_bt_hfp_callsetup_state_e calls_state = (ql_bt_hfp_callsetup_state_e)hfp_info->state;
                        QL_BT_HFP_LOG("calls_state=%d,addr=%02x%02x%02x%02x%02x%02x", calls_state, hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 

                        free(hfp_info);
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("call set failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_NETWORK_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("network sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        ql_bt_hfp_network_state_e network = (ql_bt_hfp_network_state_e)hfp_info->state;
                        QL_BT_HFP_LOG("network=%d,addr=%02x%02x%02x%02x%02x%02x", network, hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 

                        free(hfp_info);
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("network failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_NETWORK_SIGNAL_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("net signal sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        unsigned char signal = (unsigned char)hfp_info->state;
                        QL_BT_HFP_LOG("signal=%d,addr=%02x%02x%02x%02x%02x%02x", signal, hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 

                        free(hfp_info);
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("net signal failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_BATTERY_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("battery sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        int level = (int)hfp_info->state;
                        QL_BT_HFP_LOG("level=%d,addr=%02x%02x%02x%02x%02x%02x", level, hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 

                        free(hfp_info);
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("battery failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_CALLHELD_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("callheld sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        ql_bt_hfp_callheld_state_e state = (ql_bt_hfp_callheld_state_e)hfp_info->state;
                        QL_BT_HFP_LOG("state=%d,addr=%02x%02x%02x%02x%02x%02x", state, hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 

                        free(hfp_info);
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("callheld failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_AUDIO_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("audio sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        ql_bt_hfp_audio_state_e state = (ql_bt_hfp_audio_state_e)hfp_info->state;
                        QL_BT_HFP_LOG("state=%d,addr=%02x%02x%02x%02x%02x%02x", state, hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 

                        free(hfp_info);
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("audio failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_VOLUME_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("volume sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        ql_bt_hfp_volume_type_e type = (ql_bt_hfp_volume_type_e)hfp_info->state;
                        unsigned char volume = test_event.param3;
                        QL_BT_HFP_LOG("type=%d,volume", type, volume); 
                        free(hfp_info);
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("volume failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_NETWORK_TYPE_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("net type sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        ql_bt_hfp_service_type_e type = (ql_bt_hfp_service_type_e)hfp_info->state;
                        QL_BT_HFP_LOG("type=%d,addr=%02x%02x%02x%02x%02x%02x", type, hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 
                        free(hfp_info);
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("net type failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_CODEC_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("codec sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        ql_bt_hfp_codec_type_e type = (ql_bt_hfp_codec_type_e)hfp_info->state;
                        QL_BT_HFP_LOG("type=%d,addr=%02x%02x%02x%02x%02x%02x", type, hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 
                        free(hfp_info);
                    }
                }
                else
                {
                    QL_BT_HFP_LOG("codec failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            case QUEC_BT_HFP_RING_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_HFP_LOG("ring sucess"); 
                    ql_bt_hfp_event_info_t *hfp_info = (ql_bt_hfp_event_info_t *)test_event.param2;
                    if (hfp_info)
                    {   
                        //hfp_info point to a malloc addrss,need to free
                        
                        QL_BT_HFP_LOG("addr=%02x%02x%02x%02x%02x%02x", hfp_info->addr.addr[0], hfp_info->addr.addr[1], \
                            hfp_info->addr.addr[2], hfp_info->addr.addr[3], hfp_info->addr.addr[4], hfp_info->addr.addr[5]); 
                        free(hfp_info);
                    }
                     
                    ret = ql_bt_hfp_demo_answer_accept_call();
                    //ret = ql_bt_hfp_demo_reject_call();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_HFP_STOP;
                    }
                    
                }
                else
                {
                    QL_BT_HFP_LOG("ring failed");
                    goto QL_BT_HFP_STOP;
                }
            }  
            break;
            default:
            break;
        }
        return ret;
QL_BT_HFP_STOP:
        ret = ql_bt_demo_stop();
	}

    return ret;
}


void ql_bt_hfp_task_pthread(void *ctx)
{
	QlOSStatus err = 0;
    ql_errcode_bt_e ret = QL_BT_SUCCESS;

    ret = ql_bt_hfp_init(ql_bt_hfp_notify_cb);
    if (ret != QL_BT_SUCCESS)
    {
        goto QL_BT_HFP_NOT_INIT_EXIT;
    }

    ret = ql_bt_demo_start();
    if (ret != QL_BT_SUCCESS)
    {
        goto QL_BT_HFP_INIT_EXIT;
    }
    
	while(1)
	{
        ret = ql_bt_hfp_event(); 
        if (ret != QL_BT_SUCCESS)
        {
            break;
        }
	}
    
QL_BT_HFP_INIT_EXIT:
    ql_bt_hfp_release();
QL_BT_HFP_NOT_INIT_EXIT:	
	err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_BT_HFP_LOG("task deleted failed");
	}
}

QlOSStatus ql_bt_hfp_demo_init(void)
{	
	QlOSStatus err = QL_OSI_SUCCESS;

    QL_BT_HFP_LOG("enter ql_bt_hfp_demo_init");
	err = ql_rtos_task_create(&bt_hfp_demo_task, BT_BLE_DEMO_TASK_STACK_SIZE, BT_BLE_DEMO_TASK_PRIO, "ql_bt_hfp", ql_bt_hfp_task_pthread, NULL, BT_BLE_DEMO_TASK_EVENT_CNT);
	
    return err;
}


