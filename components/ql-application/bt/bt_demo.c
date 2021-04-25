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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_api_bt.h"

#include "ql_log.h"
#include "bt_demo.h"

#define QL_BT_LOG_LEVEL         	QL_LOG_LEVEL_INFO
#define QL_BT_LOG(msg, ...)			QL_LOG(QL_BT_LOG_LEVEL, "ql_BT_DEMO", msg, ##__VA_ARGS__)
#define QL_BT_LOG_PUSH(msg, ...)	QL_LOG_PUSH("ql_BT_DEMO", msg, ##__VA_ARGS__)

#define QL_BT_DEMO_ADDR_MAX_SIZE                    17
ql_task_t bt_demo_task = NULL;

ql_bt_addr_s bt_local_addr = {{0x2E, 0x58, 0x9E, 0xDA, 0x45, 0x41}};

ql_bt_visible_mode_e scan_mode = QL_BT_PAGE_AND_INQUIRY_ENABLE;
ql_bt_ble_local_name_s bt_local_name = 
{
    .name = "UNISOC-8910"
};
ql_bt_inquiry_type_e bt_inquiry_type = QL_BT_SERVICE_ALL;
ql_bt_addr_s bt_bond_addr = {{0x66, 0xCA, 0xC9, 0xA2, 0x3E, 0x38}};
ql_bt_addr_s bt_remove_bond_addr = {{0x66, 0xCA, 0xC9, 0xA2, 0x3E, 0x38}};
ql_bt_addr_s bt_connection_addr = {{0x66, 0xCA, 0xC9, 0xA2, 0x3E, 0x38}};


ql_errcode_bt_e ql_bt_demo_get_state()
{
    ql_errcode_bt_e ret;
    ql_bt_state_e bt_state;
    
    ret = ql_bt_get_state(&bt_state);
    if (ret != QL_BT_SUCCESS)
    {
        QL_BT_LOG("error=%x", ret);
    }
    else 
    {
        QL_BT_LOG("bt_state=%d",(int)bt_state);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_demo_start()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_start();
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else if (ret == QL_BT_ALREADY_STARTED_ERR)
    {
        QL_BT_LOG("already started");
    }
    else
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_demo_get_scanmde()
{
    ql_errcode_bt_e ret;
    ql_bt_visible_mode_e scan_mode;
    
    ret = ql_classic_bt_get_scanmode(&scan_mode);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("scan_mode=%d",(int)scan_mode); 
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    } 

    return ret;
}

ql_errcode_bt_e ql_be_demo_set_scanmode()
{
    ql_errcode_bt_e ret;
    
    ret = ql_classic_bt_set_scanmode(scan_mode);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_demo_get_local_name()
{
    ql_errcode_bt_e ret;
    ql_bt_ble_local_name_s bt_local_name;
    
    memset(bt_local_name.name,0x00,sizeof(bt_local_name.name));
    ret = ql_bt_ble_get_localname(&bt_local_name);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("bt_local_name:%s",bt_local_name.name);
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_demo_set_local_name()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_ble_set_localname(bt_local_name);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_bt_demo_get_local_addr()
{
    ql_errcode_bt_e ret;
    unsigned char str_ptr[32];
    ql_bt_addr_s bt_local_addr;
    
    ret = ql_classic_bt_get_localaddr(&bt_local_addr);
    if (ret == QL_BT_SUCCESS)
    {
        memset(str_ptr,0x00,sizeof(str_ptr));
        sprintf((char *)str_ptr, "%02x:%02x:%02x:%02x:%02x:%02x", bt_local_addr.addr[0], bt_local_addr.addr[1], bt_local_addr.addr[2], bt_local_addr.addr[3], bt_local_addr.addr[4], bt_local_addr.addr[5]);
        QL_BT_LOG("localaddr=%s",str_ptr);
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_demo_start_inquiry()
{
    ql_errcode_bt_e ret;
    
    ret = ql_classic_bt_start_inquiry(bt_inquiry_type);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_demo_cancel_inquiry()
{
    ql_errcode_bt_e ret;
    
    ret = ql_classic_bt_cancel_inquiry();
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_bt_demo_start_bond()
{
    ql_errcode_bt_e ret;
   
    ret = ql_classic_bt_start_bond(bt_bond_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bt_demo_cancel_bond()
{
    ql_errcode_bt_e ret;;
    ret = ql_classic_bt_cancel_bond(bt_bond_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_bt_demo_get_bond_info()
{
    ql_errcode_bt_e ret;
    unsigned int bond_num;
    ql_bt_info_s bond_info[6];
    unsigned char str_ptr[256];
    
    ret = ql_classic_bt_get_bond_info(sizeof(bond_info)/sizeof(bond_info[0]),&bond_num,bond_info);
    if (ret == QL_BT_PRAMA_SIZE_ERR)
    {
        QL_BT_LOG("bond_num=%d",bond_num);
        ql_bt_info_s * info = (ql_bt_info_s *)malloc(sizeof(ql_bt_info_s)*bond_num);
        unsigned int num;
        ql_bt_info_s * bt_info = NULL;

        if (info == NULL)
        {
            QL_BT_LOG("malloc info error");
            return QL_BT_ERROR;
        }
        
        ret = ql_classic_bt_get_bond_info(bond_num,&num,info);
        if (ret != QL_BT_PRAMA_SIZE_ERR)
        {
            unsigned char addr_string[QL_BT_DEMO_ADDR_MAX_SIZE + 1] = {0};
            unsigned int index;

            for (index=0; index<num; index++)
            {
                bt_info = (ql_bt_info_s *)&info[index*sizeof(ql_bt_info_s)];
                memset(addr_string,0x00,sizeof(addr_string));
                sprintf((char *)addr_string, "%02x:%02x:%02x:%02x:%02x:%02x", bt_info->addr.addr[0], bt_info->addr.addr[1], bt_info->addr.addr[2], bt_info->addr.addr[3], bt_info->addr.addr[4], bt_info->addr.addr[5]);
                //QL_BT_LOG("addr_string=%s",addr_string);
                //QL_BT_LOG("pdev_info->name[0]=%d",bt_info->name[0]);
                memset(str_ptr,0x00,sizeof(str_ptr));
                sprintf((char *)str_ptr, "%s%s,%s,%x", "INQ:", addr_string, (char *)bt_info->name, bt_info->dev_class);
                QL_BT_LOG("str_ptr=%s",str_ptr);
            }  
        }
        free(info);
    }
    else 
    {
        unsigned char addr_string[QL_BT_DEMO_ADDR_MAX_SIZE + 1] = {0};
        unsigned int index;

        for (index=0; index<bond_num; index++)
        {
            memset(addr_string,0x00,sizeof(addr_string));
            sprintf((char *)addr_string, "%02x:%02x:%02x:%02x:%02x:%02x", bond_info[index].addr.addr[0], bond_info[index].addr.addr[1], bond_info[index].addr.addr[2], bond_info[index].addr.addr[3], bond_info[index].addr.addr[4], bond_info[index].addr.addr[5]);
            //QL_BT_LOG("addr_string=%s",addr_string);
            //QL_BT_LOG("pdev_info->name[0]=%d",bond_info[index].name[0]);
            memset(str_ptr,0x00,sizeof(str_ptr));
            sprintf((char *)str_ptr, "%s%s,%s,%x", "INQ:", addr_string, (char *)bond_info[index].name, bond_info[index].dev_class);
            QL_BT_LOG("str_ptr=%s",str_ptr);
        }  
    }
    return QL_BT_SUCCESS;
}

static ql_errcode_bt_e ql_bt_demo_remove_typical_bond_device()
{
    ql_errcode_bt_e ret;
    ret = ql_classic_bt_remove_typical_bond_device(bt_remove_bond_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_bt_demo_remove_all_bond_device()
{
    ql_errcode_bt_e ret;
    
    ret = ql_classic_bt_remove_all_bond_device();
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_bt_demo_connect()
{
    ql_errcode_bt_e ret;
    ql_bt_connect_info_s connect_info;

    connect_info.mode = QL_BT_CONNECT_SPP;
    connect_info.addr.addr[0] = 0x66;
    connect_info.addr.addr[1] = 0xCA;
    connect_info.addr.addr[2] = 0xC9;
    connect_info.addr.addr[3] = 0xA2;
    connect_info.addr.addr[4] = 0x3E;
    connect_info.addr.addr[5] = 0x38;
    
    ret = ql_classic_bt_connect(connect_info);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_bt_demo_disconnect()
{
    ql_errcode_bt_e ret;
    
    ret = ql_classic_bt_disconnect();
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_bt_demo_get_connection()
{
    ql_errcode_bt_e ret;
    
    ret = ql_classic_bt_get_connection(bt_connection_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else
    {
        QL_BT_LOG("disconnet");
    }

    return ret;
}

static ql_errcode_bt_e ql_bt_demo_spp_send_data()
{
    ql_errcode_bt_e ret;
    ql_bt_spp_info_s spp_info;

    memcpy(spp_info.data,"123456",6);
    spp_info.len = 6;
    
    ret = ql_classic_bt_spp_send(spp_info);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}


ql_errcode_bt_e ql_bt_demo_stop()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_stop();
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bleadv_demo_stop()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bleadv_stop();
    if (ret == QL_BT_SUCCESS)
    {
        QL_BT_LOG("sucess");
    }
    else 
    {
        QL_BT_LOG("error=%x", ret);
    }

    return ret;
}

void ql_bt_notify_cb(void *ind_msg_buf, void *ctx)
{
    ql_event_t *ql_event = NULL;
    
    if (ind_msg_buf == NULL)
    {
        return ;
        
    }

    ql_event = (ql_event_t *)ind_msg_buf;
    if (ql_event->id != 0)
    {
        ql_rtos_event_send(bt_demo_task,ql_event);
    }
}

static ql_errcode_bt_e ql_bt_spp_server_event()
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
                    QL_BT_LOG("start sucess");
                    ret = ql_bt_demo_get_state();
                    if (ret == QL_BT_SUCCESS)
                    {
                        ql_be_demo_set_scanmode();
                        ret = ql_bt_demo_get_scanmde();
                        if (ret == QL_BT_SUCCESS)
                        {
                            ret = ql_be_demo_set_scanmode();
                            if (ret != QL_BT_SUCCESS)
                            {
                                goto QL_BT_SPP_SERVER_STOP;
                            }
                            else 
                            {
                                test_event.id = QUEC_BT_VISIBILE_IND;
                                test_event.param1 = QL_BT_STATUS_SUCCESS;
                                ql_rtos_event_send(bt_demo_task,&test_event);
                            }
                        }
                        else 
                        {
                            goto QL_BT_SPP_SERVER_STOP;
                        }
                    }
                    else 
                    {
                        goto QL_BT_SPP_SERVER_STOP;
                    }  
                }
                else
                {
                    QL_BT_LOG("start failed");
                    
                }
            }
            break;
            case QUEC_BT_STOP_STATUS_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("stop sucess");
                    
                }
                else
                {
                    QL_BT_LOG("stop failed");
                }
            }
            break;
            case QUEC_BT_VISIBILE_IND:
            case QUEC_BT_HIDDEN_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("set scanmode sucess"); 
                    ret = ql_bt_demo_get_scanmde();
                    if (ret == QL_BT_SUCCESS)
                    { 
                        ret = ql_bt_demo_get_local_name();
                        if (ret == QL_BT_SUCCESS)
                        {
                            ret = ql_bt_demo_set_local_name();
                            if (ret != QL_BT_SUCCESS)
                            {
                                goto QL_BT_SPP_SERVER_STOP;
                            }
                            else 
                            {
                                test_event.id = QUEC_BT_SET_LOCALNAME_IND;
                                test_event.param1 = QL_BT_STATUS_SUCCESS;
                                ql_rtos_event_send(bt_demo_task,&test_event);
                            }
                        }
                        else 
                        {
                            goto QL_BT_SPP_SERVER_STOP;
                        }
                    }
                    else 
                    {
                        goto QL_BT_SPP_SERVER_STOP;
                    }
                }
                else
                {
                    QL_BT_LOG("set scanmode failed");
                    goto QL_BT_SPP_SERVER_STOP;
                }
            }
            break;
            case QUEC_BT_SET_LOCALNAME_IND: 
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("set local name sucess");
                    ret = ql_bt_demo_get_local_name();
                    if (ret == QL_BT_SUCCESS)
                    {
                        ret = ql_bt_demo_get_local_addr();
                        if (ret != QL_BT_SUCCESS)
                        {
                            goto QL_BT_SPP_SERVER_STOP;
                        }
                    }
                    else 
                    {
                        goto QL_BT_SPP_SERVER_STOP;
                    }
                }
                else
                {
                    QL_BT_LOG("set local name failed");
                    goto QL_BT_SPP_SERVER_STOP;
                }
            }
            break;
            case QUEC_BT_SET_LOCALADDR_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("set local address sucess");
                    ret = ql_bt_demo_get_local_addr();
                    if (ret == QL_BT_SUCCESS)
                    {
                        ret = ql_bt_demo_start();
                        if (ret == QL_BT_ALREADY_STARTED_ERR)
                        {
                            test_event.id = QUEC_BT_START_STATUS_IND;
                            test_event.param1 = QL_BT_STATUS_SUCCESS;
                            ql_rtos_event_send(bt_demo_task,&test_event);
                        }
                    }
                }
                else
                {
                    QL_BT_LOG("set local address failed");
                   
                }
            }
            break;
            case QUEC_BT_INQURY_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("inqury sucess");
                }
                else
                {
                    QL_BT_LOG("inqury failed");
                }
            }
            break;
            case QUEC_BT_INQURY_END_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("inqury end sucess");
                }
                else
                {
                    QL_BT_LOG("inqury end failed");
                }
            }
            break;
            case QUEC_BT_CANCEL_INQURY_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("cancel inqury sucess");
                }
                else
                {
                    QL_BT_LOG("cancel inqury failed");
                } 
            }
            break;
            case QUEC_BT_BOND_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("bond sucess");
                }
                else
                {
                    QL_BT_LOG("bond failed");
                }
            }
            break;
            case QUEC_BT_CANCELBOND_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("cancel bond sucess");
                }
                else
                {
                    QL_BT_LOG("cancel bond failed");
                }
            }
            break;
            case QUEC_BT_CONNECT_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("connect sucess");
                }
                else
                {
                    QL_BT_LOG("connect failed");
                    goto QL_BT_SPP_SERVER_STOP; 
                }
            }
            break;
            case QUEC_BT_SPP_SEND_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("spp send data sucess");
                    ret = ql_bt_demo_disconnect();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_SPP_SERVER_STOP;
                    }   
                }
                else
                {
                    QL_BT_LOG("spp send data failed");
                    goto QL_BT_SPP_SERVER_STOP;
                }
            }
            break;
            case QUEC_BT_SPP_RECV_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("spp recv data sucess");
                    void *pData = (void *)(test_event.param2);
                    if (pData == NULL)
                    {
                        QL_BT_LOG("inqury pData is NULL");
                        break;
                    }
                    
                    ql_bt_spp_data_s *ql_spp_data = (ql_bt_spp_data_s *)pData;
                    char *disp_data = (char *)malloc(ql_spp_data->data_len+1);
                    if (disp_data == NULL)
                    {
                        QL_BT_LOG("malloc disp_data error");
                        free(ql_spp_data->data);
                        free(ql_spp_data);
                        break;
                    }
                    memset(disp_data, 0x00, ql_spp_data->data_len+1);
                    memcpy(disp_data, ql_spp_data->data, ql_spp_data->data_len);
                    QL_BT_LOG("spp recv len=%d:data:%s", ql_spp_data->data_len, disp_data);
                    
                    free(disp_data);
                    free(ql_spp_data->data);
                    ql_spp_data->data = NULL;
                    free(ql_spp_data);
                    ql_spp_data = NULL;
                    pData = NULL;   

                    ret = ql_bt_demo_spp_send_data();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_SPP_SERVER_STOP;
                    }
                }
                else
                {
                    QL_BT_LOG("spp send data failed");
                    goto QL_BT_SPP_SERVER_STOP;
                }
            }
            break;
            case QUEC_BT_DISCONNECT_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("disconnect sucess");
                    goto QL_BT_SPP_SERVER_STOP;   
                }
                else
                {
                    QL_BT_LOG("disconnect failed");
                    goto QL_BT_SPP_SERVER_STOP;
                }
            }
            break;
            default:
            break;
        }
        return ret;
QL_BT_SPP_SERVER_STOP:
        ret = ql_bt_demo_stop();
	}

    return ret;
}

void ql_bt_spp_server_demo_task_pthread(void *ctx)
{
	QlOSStatus err = 0;
    ql_errcode_bt_e ret = QL_BT_SUCCESS;
   
    ret = ql_bt_register_cb(ql_bt_notify_cb);
    if (ret != QL_BT_SUCCESS)
    {
        goto QL_BT_SPP_SERVER_NOT_INIT_EIXT;
    }
    
    ret = ql_bt_spp_init(ql_bt_notify_cb);
    if (ret != QL_BT_SUCCESS)
    {
        goto QL_BT_SPP_SERVER_NOT_INIT_EIXT;
    }

    ret = ql_bt_demo_start();
    if (ret != QL_BT_SUCCESS)
    {
        goto QL_BT_SPP_SERVER_INIT_EIXT;
    }
    
	while(1)
	{
        ret = ql_bt_spp_server_event(); 
        if (ret != QL_BT_SUCCESS)
        {
            break;
        }
	}
QL_BT_SPP_SERVER_INIT_EIXT:  
	ql_bt_spp_release();
QL_BT_SPP_SERVER_NOT_INIT_EIXT:
	err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_BT_LOG("task deleted failed");
	}
}



QlOSStatus ql_bt_spp_server_demo_init(void)
{	
	QlOSStatus err = QL_OSI_SUCCESS;

    QL_BT_LOG("enter ql_bt_spp_server_demo_init");
	err = ql_rtos_task_create(&bt_demo_task, BT_BLE_DEMO_TASK_STACK_SIZE, BT_BLE_DEMO_TASK_PRIO, "ql_spp_server", ql_bt_spp_server_demo_task_pthread, NULL, BT_BLE_DEMO_TASK_EVENT_CNT);
	
    return err;
}

static ql_errcode_bt_e ql_bt_spp_client_event()
{
    ql_event_t test_event = {0};
    ql_errcode_bt_e ret = QL_BT_SUCCESS;
    QL_BT_STATUS status;
    if (ql_event_try_wait(&test_event) == 0)
	{
        if (test_event.id == 0)
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
                    QL_BT_LOG("start sucess");
                    ret = ql_bt_demo_get_state();
                    if (ret == QL_BT_SUCCESS)
                    {  
                        ql_be_demo_set_scanmode();
                        ret = ql_bt_demo_get_local_name();
                        if (ret == QL_BT_SUCCESS)
                        {
                            ret = ql_bt_demo_set_local_name();
                            if (ret != QL_BT_SUCCESS)
                            {
                                goto QL_BT_SPP_CLIENT_STOP;
                            }
                            else 
                            {
                                test_event.id = QUEC_BT_SET_LOCALNAME_IND;
                                test_event.param1 = QL_BT_STATUS_SUCCESS;
                                ql_rtos_event_send(bt_demo_task,&test_event);
                            }
                        }
                        else 
                        {
                            goto QL_BT_SPP_CLIENT_STOP;
                        }
                    }
                    else 
                    {
                        goto QL_BT_SPP_CLIENT_STOP;
                    }    
                }
                else
                {
                    QL_BT_LOG("start failed");
                }
            }
            break;
            case QUEC_BT_STOP_STATUS_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("stop sucess");
                    
                }
                else
                {
                    QL_BT_LOG("stop failed");
                }
            }
            break;
            case QUEC_BT_SET_LOCALNAME_IND: 
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("set local name sucess");
                    ret = ql_bt_demo_get_local_name();
                    if (ret == QL_BT_SUCCESS)
                    {  
                        ret = ql_bt_demo_get_local_addr();
                        if (ret == QL_BT_SUCCESS)
                        {
                            ret = ql_bt_demo_start_inquiry();
                            if (ret != QL_BT_SUCCESS)
                            {
                                goto QL_BT_SPP_CLIENT_STOP;
                            }
                            else 
                            {
                                //ret = ql_bt_demo_cancel_inquiry();
                                //if (ret != QL_BT_SUCCESS)
                                //{
                                //    goto QL_BT_DEMO_STOP;
                                //}
                            }
                        }
                        else 
                        {
                            goto QL_BT_SPP_CLIENT_STOP;
                        }
                    }
                    else 
                    {
                        goto QL_BT_SPP_CLIENT_STOP;
                    }
                }
                else
                {
                    QL_BT_LOG("set local name failed");
                    goto QL_BT_SPP_CLIENT_STOP;
                }
            }
            break;
            case QUEC_BT_SET_LOCALADDR_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("set local address sucess");
                    ret = ql_bt_demo_get_local_addr();
                    ret = ql_bt_demo_get_state();
                    if (ret == QL_BT_SUCCESS)
                    {
                        ret = ql_bt_demo_start();
                        if (ret == QL_BT_ALREADY_STARTED_ERR)
                        {
                            test_event.id = QUEC_BT_START_STATUS_IND;
                            test_event.param1 = QL_BT_STATUS_SUCCESS;
                            ql_rtos_event_send(bt_demo_task,&test_event);
                        }
                    }
                }
                else
                {
                    QL_BT_LOG("set local address failed");
                   
                }
            }
            break;
            case QUEC_BT_INQURY_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("inqury sucess");
                    ql_bt_info_s *pdev_info = NULL;
                    unsigned char addr_string[QL_BT_DEMO_ADDR_MAX_SIZE + 1] = {0};
                    unsigned char str_ptr[256];
                    //unsigned char ql_local_name[QL_BT_NAME_MAX_SIZE + 1] = {0};

                    void *pData = (void *)(test_event.param2);
                    if (pData == NULL)
                    {
                        QL_BT_LOG("inqury pData is NULL");
                        break;
                    }
                    pdev_info = (ql_bt_info_s *)pData;
                    memset(addr_string,0x00,sizeof(addr_string));
                    sprintf((char *)addr_string, "%02x:%02x:%02x:%02x:%02x:%02x", pdev_info->addr.addr[0], pdev_info->addr.addr[1], pdev_info->addr.addr[2], pdev_info->addr.addr[3], pdev_info->addr.addr[4], pdev_info->addr.addr[5]);
                    //QL_BT_LOG("inqury addr_string=%s",addr_string);
                    //QL_BT_LOG("inqury pdev_info->name[0]=%d",pdev_info->name[0]);
                    memset(str_ptr,0x00,sizeof(str_ptr));
                    sprintf((char *)str_ptr, "%s%s,%s,%x", "INQ:", addr_string, (char *)pdev_info->name, pdev_info->dev_class);
                    QL_BT_LOG("inqury str_ptr=%s",str_ptr);  

                    free(pData);
                }
                else
                {
                    QL_BT_LOG("inqury failed");
                    goto QL_BT_SPP_CLIENT_STOP;
                }
            }
            break;
            case QUEC_BT_INQURY_END_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("inqury end sucess");
                    ret = ql_bt_demo_start_bond();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_SPP_CLIENT_STOP;
                    }
                    else 
                    {
                        //ql_rtos_task_sleep_ms(500);
                        //ret = ql_bt_demo_cancel_bond();
                        //if (ret != QL_BT_SUCCESS)
                        //{
                        //    goto QL_BT_DEMO_STOP;
                        //}
                    }
                    
                }
                else
                {
                    QL_BT_LOG("inqury end failed");
                    goto QL_BT_SPP_CLIENT_STOP; 
                }
            }
            break;
            case QUEC_BT_CANCEL_INQURY_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("cancel inqury sucess"); 
                    goto QL_BT_SPP_CLIENT_STOP;
                }
                else
                {
                    QL_BT_LOG("cancel inqury failed");
                    goto QL_BT_SPP_CLIENT_STOP; 
                } 
            }
            break;
            case QUEC_BT_BOND_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("bond sucess");
                   ret = ql_bt_demo_get_bond_info();
                    if (ret == QL_BT_SUCCESS)
                    {
                        ret = ql_bt_demo_remove_typical_bond_device();
                        if (ret == QL_BT_SUCCESS)
                        {
                            ret = ql_bt_demo_remove_all_bond_device();
                            if (ret == QL_BT_SUCCESS)
                            {
                                ret = ql_bt_demo_connect();
                                if (ret != QL_BT_SUCCESS)
                                {
                                    goto QL_BT_SPP_CLIENT_STOP;
                                }
                            }
                            else 
                            {
                                goto QL_BT_SPP_CLIENT_STOP;
                            }
                        }
                        else 
                        {
                            goto QL_BT_SPP_CLIENT_STOP;
                        }
                    }
                    else 
                    {
                        goto QL_BT_SPP_CLIENT_STOP;
                    }
                }
                else
                {
                    QL_BT_LOG("bond failed");
                    goto QL_BT_SPP_CLIENT_STOP; 
                }
            }
            break;
            case QUEC_BT_CANCELBOND_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("cancel bond sucess");
                    goto QL_BT_SPP_CLIENT_STOP;
                }
                else
                {
                    QL_BT_LOG("cancel bond failed");
                    goto QL_BT_SPP_CLIENT_STOP;
                }
            }
            break;
            case QUEC_BT_CONNECT_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    ret = ql_bt_demo_get_connection();
                    if (ret == QL_BT_SUCCESS)
                    {
                        ret = ql_bt_demo_spp_send_data();
                        if (ret != QL_BT_SUCCESS)
                        {
                            goto QL_BT_SPP_CLIENT_STOP;
                        }
                    }
                    else 
                    {
                        goto QL_BT_SPP_CLIENT_STOP; 
                    }
                }
                else
                {
                    QL_BT_LOG("connect failed");
                    goto QL_BT_SPP_CLIENT_STOP; 
                }
            }
            break;
            case QUEC_BT_SPP_SEND_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("spp send data sucess");
                    ret = ql_bt_demo_disconnect();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_SPP_CLIENT_STOP;
                    }   
                }
                else
                {
                    QL_BT_LOG("spp send data failed");
                    goto QL_BT_SPP_CLIENT_STOP;
                }
            }
            break;
            case QUEC_BT_SPP_RECV_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("spp recv data sucess");
                    void *pData = (void *)(test_event.param2);
                    if (pData == NULL)
                    {
                        QL_BT_LOG("inqury pData is NULL");
                        break;
                    }
                    
                    ql_bt_spp_data_s *ql_spp_data = (ql_bt_spp_data_s *)pData;
                    char *disp_data = (char *)malloc(ql_spp_data->data_len+1);
                    if (disp_data == NULL)
                    {
                        QL_BT_LOG("malloc disp_data error");
                        free(ql_spp_data->data);
                        free(ql_spp_data);
                        break;
                    }
                    memset(disp_data, 0x00, ql_spp_data->data_len+1);
                    memcpy(disp_data, ql_spp_data->data, ql_spp_data->data_len);
                    QL_BT_LOG("spp recv data len=%s:%s", ql_spp_data->data_len, disp_data);
                    free(disp_data);
                    free(ql_spp_data->data);
                    ql_spp_data->data = NULL;
                    free(ql_spp_data);
                    ql_spp_data = NULL;
                    pData = NULL;   

                    ret = ql_bt_demo_spp_send_data();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BT_SPP_CLIENT_STOP;
                    }
                }
                else
                {
                    QL_BT_LOG("spp send data failed");
                    goto QL_BT_SPP_CLIENT_STOP;
                }
            }
            break;
            case QUEC_BT_DISCONNECT_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BT_LOG("disconnect sucess");
                    goto QL_BT_SPP_CLIENT_STOP;   
                }
                else
                {
                    QL_BT_LOG("disconnect failed");
                    goto QL_BT_SPP_CLIENT_STOP;
                }
            }
            break;
            default:
            break;
        }
        return ret;
QL_BT_SPP_CLIENT_STOP:
        ql_bt_demo_stop();
	}
    return ret;
}

void ql_bt_spp_client_demo_task_pthread(void *ctx)
{
	QlOSStatus err = 0;
    ql_errcode_bt_e ret = QL_BT_SUCCESS;
   
    ret = ql_bt_register_cb(ql_bt_notify_cb);
    if (ret != QL_BT_SUCCESS)
    {
        goto QL_BT_SPP_SERVER_NOT_INIT_EIXT;
    }
    
    ret = ql_bt_spp_init(ql_bt_notify_cb);
    if (ret != QL_BT_SUCCESS)
    {
        goto QL_BT_SPP_SERVER_NOT_INIT_EIXT;
    }

    ret = ql_bt_demo_start();
    if (ret != QL_BT_SUCCESS)
    {
        goto QL_BT_SPP_SERVER_INIT_EIXT;
    }
    
	while(1)
	{
        ql_bt_spp_client_event(); 
        //if (ret != QL_BT_SUCCESS)
        //{
        //    break;
        //}
	}
QL_BT_SPP_SERVER_INIT_EIXT:  
	ql_bt_spp_release();
QL_BT_SPP_SERVER_NOT_INIT_EIXT:
	err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_BT_LOG("task deleted failed");
	}
}



QlOSStatus ql_bt_spp_client_demo_init(void)
{	
	QlOSStatus err = QL_OSI_SUCCESS;

    QL_BT_LOG("enter ql_bt_spp_client_demo_init");
	err = ql_rtos_task_create(&bt_demo_task, BT_BLE_DEMO_TASK_STACK_SIZE, BT_BLE_DEMO_TASK_PRIO, "ql_spp_client", ql_bt_spp_client_demo_task_pthread, NULL, BT_BLE_DEMO_TASK_EVENT_CNT);
	
    return err;
}


