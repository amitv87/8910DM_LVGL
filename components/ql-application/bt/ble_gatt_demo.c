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
#include "ql_api_ble_gatt.h"
#include "ql_power.h"
#include "ql_api_dev.h"

#define QL_BLE_GATT_LOG_LEVEL         	            QL_LOG_LEVEL_INFO
#define QL_BLE_GATT_LOG(msg, ...)	                QL_LOG(QL_BLE_GATT_LOG_LEVEL, "ql_BT_GATT_DEMO", msg, ##__VA_ARGS__)
#define QL_BLE_GATT_LOG_PUSH(msg, ...)	            QL_LOG_PUSH("ql_BT_GATT_DEMO", msg, ##__VA_ARGS__)
#define QL_BLE_DEMO_ADDR_MAX_SIZE                    17

//BLE run in low power mode rot not
#define QL_BLE_DEMO_LOW_POWER_USE                   0       //not run in lower power mode
extern ql_errcode_bt_e ql_bt_demo_start();
extern ql_errcode_bt_e ql_bt_demo_stop();
extern ql_errcode_bt_e ql_bt_demo_get_state();

extern ql_errcode_dev_e ql_cfun_comm_set(uint8_t at_dst_cfun, uint8_t nSim);

ql_task_t ble_demo_task = NULL;

ql_bt_ble_local_name_s ble_name = 
{
    .name = "UNISOC-8910",
};

ql_bt_addr_s bt_public_whitelist = {{0x66, 0xCA, 0xC9, 0xA2, 0x3E, 0x38}};
ql_bt_addr_s bt_random_whitelist = {{0x66, 0xCA, 0xC9, 0xA2, 0x3E, 0x39}};
ql_ble_whitelist_info_s bt_remove_whiltelist = 
{
    .mode = QL_BLE_PUBLIC_ADDRESS,
    .addr = {{0x66, 0xCA, 0xC9, 0xA2, 0x3E, 0x38}},
};

//gatt server
ql_bt_addr_s ble_adv_addr = {{0xC0, 0x58, 0x65, 0x01, 0x02, 0x05}};
ql_bleadv_typical_addr_param_s ble_adv_typical_param =
{
    .max_adv = QL_BLE_ADV_MAX_ADV,
    .min_adv = QL_BLE_ADV_MIN_ADV,
    .adv_type = QL_BLEADV_INV,
    .own_addr_type = QL_BLE_RANDOM_ADDRESS,
    .direct_address_mode = QL_BLE_RANDOM_ADDRESS,
    .direct_addr = {{0x66, 0xCA, 0xC9, 0xA2, 0x3E, 0x38}},
    .channel_map = QL_BLE_ADV_MAP,
    .filter = QL_BLEADV_FILTER_0,
    .discov_mode = 2,
    .no_br_edr = 1,
    .enable_adv = 1,
};

ql_bleadv_param_s ble_adv_param = 
{
    .min_adv = QL_BLE_ADV_MIN_ADV,
    .max_adv = QL_BLE_ADV_MAX_ADV,
    .adv_type = QL_BLEADV_INV,
    .own_addr_type = QL_BLE_RANDOM_ADDRESS,
    .channel_map = QL_BLE_ADV_MAP,
    .filter = QL_BLEADV_FILTER_0,
    .discov_mode = 2,
    .no_br_edr = 1,
    .enable_adv = 1,
};

#define QL_BLE_ADV_DATA_ORIGAL_LEN      3
ql_bleadv_set_data_s ble_adv_data = 
{
    .date_len = QL_BLE_ADV_DATA_ORIGAL_LEN,
    .data = {0x02,0x01,0x05},
};
ql_bleadv_set_data_s ble_adv_scan_rsp_data;

unsigned short ql_bt_ble_mtu = QL_BT_GATT_MTU_DEFAULT_SIZE;
unsigned char ql_ble_gatt_chara_value[247];
ql_bt_addr_s ble_connection_addr;
unsigned short ble_conn_handle = 0;
unsigned short ble_server_hanle = QL_GATT_START_HANDLE_WITHOUT_SYS;

//ibeacon
unsigned short ble_ibeacon_major = 20;
unsigned short ble_ibeacon_minor = 25;
ql_ble_update_conn_infos_s ble_conn_param = 
{
    .conn_id = 0,
    .min_interval = QL_BLE_UPDATE_CONN_MIN_INTERVAL,
    .max_interval = QL_BLE_UPDATE_CONN_MAX_INTERVAL,
    .latency = QL_BLE_UPDATE_CONN_LATENCY,
    .timeout = QL_BLE_UPDATE_CONN_TIMEOUT,
};



//gatt client
ql_blescan_scan_s ble_scan_param = 
{
    .scan_mode = QL_BLESCAN_ACTIVE,
    .interval = QL_BLE_SCAN_INTERVAL,
    .window = QL_BLE_SCAN_WINDOW,
    .filter = QL_BLESCAN_FILTER_0,
    .own_addr_type = QL_BLE_PUBLIC_ADDRESS,
};    


ql_bt_addr_s ble_scan_report_addr;
ql_ble_address_mode_e ble_scan_report_addr_type;
unsigned char ble_scan_report_name[] = "UNISOC-8910";//"vivo Y79";//"8910_BTDM";

ql_ble_gatt_state_e ql_ble_gatt_state = QL_BLE_GATT_IDLE;



ql_blegatt_prime_service_s ql_ble_gatt_target_service = 
{
    .uuid = 0x180F,     //short uuid
    //.uuid = 0x18FE,   //long uuid
};

ql_ble_gatt_chara_s ql_ble_gatt_characteristic[QL_BLE_CHARA_NUM_MAX] = 
{
    {
        .uuid = 0x2A19,  //
    },
};
unsigned char ql_ble_gatt_uuid_l[QL_BLE_LONG_UUID_SIZE] = {0xf5,0x89,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xFE,0x18,0x00,0x00};
ql_ble_gatt_chara_desc_s ql_ble_gatt_chara_desc[QL_BLE_DESC_NUM_MAX] = {0};
unsigned char ql_ble_gatt_chara_desc_value[2] = {0x12, 0x34};
ql_ble_gatt_discover_service_mode_e ql_ble_gatt_discover_service_mode = QL_BLE_GATT_DISCOVER_SERVICE_ALL;
ql_ble_gatt_uuid_state_e ql_ble_gatt_uuid_state = QL_BLE_GATT_SHORT_UUID;

unsigned short ql_ble_gatt_chara_count = 0;
unsigned short ql_ble_gatt_cur_chara = 0;

unsigned short ql_ble_gatt_chara_desc_index = 0;
unsigned short ql_ble_gatt_chara_cur_desc = 0;
unsigned short ql_ble_gatt_chara_desc_count = 0;
ql_ble_sys_service_mode_e ql_ble_gatt_sys_service = QL_RESERVED_SERVICE_DEL;

#if QL_BLE_DEMO_LOW_POWER_USE
int bt_ble_power_lock = 0;
#endif
static ql_errcode_bt_e ql_ble_demo_get_state()
{
    ql_errcode_bt_e ret;
    ql_bt_state_e bt_state;
    
    ret = ql_bt_get_state(&bt_state);
    if (ret != QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }
    else 
    {
        QL_BLE_GATT_LOG("bt_state=%d",(int)bt_state);
    }

    return ret;
}

static ql_errcode_bt_e ql_ble_demo_get_public_addr()
{
    ql_errcode_bt_e ret;
    ql_bt_addr_s bt_public_addr;
    
    ret = ql_ble_get_public_addr(&bt_public_addr);
    if (ret == QL_BT_SUCCESS)
    {
        unsigned char addr_string[QL_BLE_DEMO_ADDR_MAX_SIZE + 1] = {0};
       
        memset(addr_string,0x00,sizeof(addr_string));
        sprintf((char *)addr_string, "%02x:%02x:%02x:%02x:%02x:%02x", bt_public_addr.addr[0], bt_public_addr.addr[1], bt_public_addr.addr[2], bt_public_addr.addr[3], bt_public_addr.addr[4], bt_public_addr.addr[5]);
        QL_BLE_GATT_LOG("addr_string=%s",addr_string);
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_ble_demo_get_version()
{
    ql_errcode_bt_e ret;
    char version[QL_BLE_VERSION_SIZE];
    
    ret = ql_ble_get_version(version,sizeof(version));
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("version=%s", version);
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_ble_demo_get_random_addr()
{
    ql_errcode_bt_e ret;
    ql_bt_addr_s bt_random_addr;
    
    ret = ql_ble_get_random_addr(&bt_random_addr);
    if (ret == QL_BT_SUCCESS)
    {
        unsigned char addr_string[QL_BLE_DEMO_ADDR_MAX_SIZE + 1] = {0};
       
        memset(addr_string,0x00,sizeof(addr_string));
        sprintf((char *)addr_string, "%02x:%02x:%02x:%02x:%02x:%02x", bt_random_addr.addr[0], bt_random_addr.addr[1], bt_random_addr.addr[2], bt_random_addr.addr[3], bt_random_addr.addr[4], bt_random_addr.addr[5]);
        QL_BLE_GATT_LOG("addr_string=%s",addr_string); 
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_demo_add_public_whitelist()
{
    ql_errcode_bt_e ret;
    
    ret = ql_ble_add_public_whitelist(bt_public_whitelist);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_demo_add_random_whitelist()
{
    ql_errcode_bt_e ret;
   
    ret = ql_ble_add_random_whitelist(bt_random_whitelist);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_ble_demo_get_whitelist_info()
{
    ql_errcode_bt_e ret;
    ql_ble_whitelist_info_s whitelist[QL_BLE_WHITELIST_NUM_MAX];
    unsigned char whitelist_count = 0;

    ret = ql_ble_get_whitelist_info(QL_BLE_WHITELIST_NUM_MAX,&whitelist_count,whitelist);
    if (ret == QL_BT_SUCCESS)
    {
        unsigned char i= 0;
        unsigned char addr_string[QL_BLE_DEMO_ADDR_MAX_SIZE + 1] = {0};

        QL_BLE_GATT_LOG("whitelist_cont=%d", whitelist_count);
        for (i=0; i<whitelist_count && i<QL_BLE_WHITELIST_NUM_MAX; i++)
        {
            memset(addr_string,0x00,sizeof(addr_string));
            sprintf((char *)addr_string, "%02x:%02x:%02x:%02x:%02x:%02x", whitelist[i].addr.addr[0], whitelist[i].addr.addr[1], whitelist[i].addr.addr[2], whitelist[i].addr.addr[3], whitelist[i].addr.addr[4], whitelist[i].addr.addr[5]);
            QL_BLE_GATT_LOG("addr_string[%d]=%s", i, addr_string);
        }
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_ble_demo_remove_whitelist()
{
    ql_errcode_bt_e ret;
    
    ret = ql_ble_remove_whitelist(bt_remove_whiltelist);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_ble_demo_clean_whitelist()
{
    ql_errcode_bt_e ret;
    
    ret = ql_ble_clean_whitelist();
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_ble_demo_get_name()
{
    ql_errcode_bt_e ret;
    ql_bt_ble_local_name_s bt_name;
    
    ret = ql_bt_ble_get_localname(&bt_name);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("bt_name.name=%s",bt_name.name);
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_ble_demo_set_name()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bt_ble_set_localname(ble_name);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bleadv_demo_set_param()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bleadv_set_param(ble_adv_param);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bleadv_demo_set_typical_addr_param()
{
    ql_errcode_bt_e ret;
 
    ret = ql_bleadv_set_typical_addr_param(ble_adv_typical_param);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}



ql_errcode_bt_e ql_bleadv_demo_set_data()
{
    ql_errcode_bt_e ret = QL_BT_SUCCESS;
    unsigned char init_len = QL_BLE_ADV_DATA_ORIGAL_LEN;//ble_adv_data.date_len;
    unsigned char length = strlen((char *)ble_name.name);
    
    ble_adv_data.date_len = init_len+length+2;
    ble_adv_data.data[init_len] = length+1;
    ble_adv_data.data[init_len+1] = 0x09;
    memcpy(&ble_adv_data.data[init_len+2], ble_name.name, length);

    ret = ql_bleadv_set_data(ble_adv_data);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_bleadv_demo_set_ibeacon_data()
{
    ql_errcode_bt_e ret = QL_BT_SUCCESS;
    ql_ble_ibeacon_cfg_s cfg = {0};

    //memcpy(cfg.uuid_l, ql_ble_gatt_uuid_l, QL_BLE_LONG_UUID_SIZE);
    //cfg.major = ble_ibeacon_major;
    //cfg.minor = ble_ibeacon_minor;
    //ret = ql_ble_write_ibeacon_cfg(cfg);

    ret = ql_ble_read_ibeacon_cfg(&cfg);
    if (ret == QL_BT_SUCCESS)
    {
        ret = ql_ble_set_ibeacon_data(cfg.uuid_l, cfg.major, cfg.minor);
    }
    else 
    {
       ret = ql_ble_set_ibeacon_data(ql_ble_gatt_uuid_l, ble_ibeacon_major, ble_ibeacon_minor); 
    }
    
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}


ql_errcode_bt_e ql_bleadv_demo_set_scan_rsp_data()
{
    ql_errcode_bt_e ret;
    unsigned char length = strlen((char *)ble_name.name);
    
    ble_adv_scan_rsp_data.date_len = length+2;
    ble_adv_scan_rsp_data.data[0] = length+1;
    ble_adv_scan_rsp_data.data[1] = 0x09;
    memcpy(&ble_adv_scan_rsp_data.data[2], ble_name.name, length);
    
    ret = ql_bleadv_set_scan_rsp_data(ble_adv_scan_rsp_data);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);;
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_add_service()
{
    ql_errcode_bt_e ret;
    ql_ble_gatt_uuid_s uuid = 
    {
        .uuid_type = 1,
        .uuid_l = {0x00},
        .uuid_s = 0x180F,
    };
    
    ret = ql_ble_gatt_add_service(0x01, uuid, 1);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_add_chara()
{
    ql_errcode_bt_e ret;
    ql_ble_gatt_uuid_s uuid = 
    {
        .uuid_type = 1,
        .uuid_l = {0x00},
        .uuid_s = 0x2A19,
    };
   
    ret = ql_ble_gatt_add_chara(0x01, 0x01, QL_ATT_CHARA_PROP_READ | QL_ATT_CHARA_PROP_NOTIFY | QL_ATT_CHARA_PROP_INDICATE, uuid);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_add_chara_value()
{
    ql_errcode_bt_e ret;
    ql_ble_gatt_uuid_s uuid = 
    {
        .uuid_type = 1,
        .uuid_l = {0x00},
        .uuid_s = 0x2A19,
    };

    memset(ql_ble_gatt_chara_value, 0x00, sizeof(ql_ble_gatt_chara_value));
    ret = ql_ble_gatt_add_chara_value(0x01, 0x01, QL_ATT_PM_READABLE | QL_ATT_PM_WRITEABLE, uuid, sizeof(ql_ble_gatt_chara_value), (unsigned char *)ql_ble_gatt_chara_value);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_add_chara_desc()
{
    ql_errcode_bt_e ret;
    unsigned int value = 0;
    ql_ble_gatt_uuid_s uuid = 
    {
        .uuid_type = 1,
        .uuid_l = {0x00},
        .uuid_s = 0x2902,
    };
    
    ret = ql_ble_gatt_add_chara_desc(0x01, 0x01, QL_ATT_PM_READABLE | QL_ATT_PM_WRITEABLE, uuid, sizeof(value), (unsigned char *)&value);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_bleadv_demo_start()
{
    ql_errcode_bt_e ret;
    
    ret = ql_bleadv_start();
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_ble_demo_get_conection_state()
{
    ql_errcode_bt_e ret;
    ql_ble_connection_state_e state;

    ret = ql_ble_get_connection_state(ble_connection_addr,&state);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("state=%d", state);
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return 0;
}

static ql_errcode_bt_e ql_ble_demo_update_conn_param()
{
    ql_errcode_bt_e ret;

    ble_conn_param.conn_id = ble_conn_handle;

    ret = ql_ble_update_conn_param(ble_conn_param);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else if (ret == QL_BT_PENDING)
    {
        QL_BLE_GATT_LOG("pending");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_ble_demo_send_data()
{
    ql_errcode_bt_e ret = 0;
    unsigned char send_data[128] = {0};
    int i;
    
    for (i=0; i<sizeof(send_data)-1; i++)
    {
        send_data[i] = '3';
    }
    
    //memcpy(send_data.data,"12345678",8);
    //send_data.len = 8;
    //send_data.data[8] = 0;
    QL_BLE_GATT_LOG("send_data.data=%s", send_data);
    ret = ql_ble_send_notification_data(0, ble_server_hanle+2, sizeof(send_data), send_data);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_demo_disconect_device()
{
    ql_errcode_bt_e ret;

    ret = ql_ble_disconnect(ble_conn_handle);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}


ql_errcode_bt_e ql_ble_gatt_server_handle_event()
{
    ql_event_t test_event;
    ql_errcode_bt_e ret = QL_BT_SUCCESS;
    QL_BT_STATUS status = 0;
    
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
                    QL_BLE_GATT_LOG("start sucess");
#if QL_BLE_DEMO_LOW_POWER_USE
                    ql_lpm_wakelock_unlock(bt_ble_power_lock);
#endif
                    
                    ret = ql_ble_demo_get_state();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }
#if 1
                    ret = ql_ble_demo_get_public_addr();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_demo_get_version();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_demo_get_random_addr();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_demo_add_public_whitelist();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_demo_add_random_whitelist();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }
                    
                    ret = ql_ble_demo_get_whitelist_info();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_demo_remove_whitelist();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_demo_clean_whitelist();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    //ret = ql_ble_demo_get_name();
                    //if (ret != QL_BT_SUCCESS)
                    //{
                    //    goto QL_BLE_ADV_DEMO_STOP;
                    //}

                    ret = ql_ble_demo_set_name();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_demo_get_name();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }
#endif
                    ret = ql_bleadv_demo_set_param();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_bleadv_demo_set_ibeacon_data();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    //ret = ql_bleadv_demo_set_data();
                    //if (ret != QL_BT_SUCCESS)
                    //{
                    //    goto QL_BLE_ADV_DEMO_STOP;
                    //}

                    ret = ql_bleadv_demo_set_scan_rsp_data();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_gatt_demo_add_service();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_gatt_demo_add_chara();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret  = ql_ble_gatt_demo_add_chara_value();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_gatt_demo_add_chara_desc();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }

                    ret = ql_ble_gatt_add_or_clear_service_complete(QL_BLE_SERVICE_ADD_COMPLETE, ql_ble_gatt_sys_service);
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }
                    
                    if (ql_ble_gatt_sys_service == QL_RESERVED_SERVICE_DEL)
                    {
                        ble_server_hanle = QL_GATT_START_HANDLE_WITHOUT_SYS;
                    }
                    else 
                    {
                        ble_server_hanle = QL_GATT_START_HANDLE_WITH_SYS;
                    }
                    
                    
                    ret = ql_bleadv_demo_start();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("start failed"); 
                }
            }
            break;
            case QUEC_BT_STOP_STATUS_IND:
            {
#if QL_BLE_DEMO_LOW_POWER_USE
                ql_lpm_wakelock_unlock(bt_ble_power_lock);
#endif
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("stop sucess");
                    ret = QL_BT_ALREADY_STOPED_ERR;
                }
                else
                {
                    QL_BLE_GATT_LOG("stop failed");
                    ret = QL_BT_ALREADY_STOPED_ERR;
                }
            }
            break;
            case QUEC_BLE_CONNECT_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("connect sucess");

                    ql_bt_addr_s * addr = (ql_bt_addr_s *)test_event.param2;
                    if (addr)
                    {
                        //addr need to free
                        unsigned char addr_string[QL_BLE_DEMO_ADDR_MAX_SIZE + 1] = {0};
                        memset(addr_string,0x00,sizeof(addr_string));
                        sprintf((char *)addr_string, "%02x:%02x:%02x:%02x:%02x:%02x", addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5]);
                        QL_BLE_GATT_LOG("addr_string=%s", addr_string); 
                        memcpy(ble_connection_addr.addr, addr->addr, QL_BT_MAC_ADDRESS_SIZE);
                        
                        free(addr);
                    }
                    ble_conn_handle = test_event.param3;
                    QL_BLE_GATT_LOG("ble_conn_handle=%d", ble_conn_handle);

                    ret = ql_ble_demo_get_conection_state();
                    if (ret == QL_BT_SUCCESS)
                    {  
                        ql_rtos_task_sleep_s(3);
                        ret = ql_ble_demo_send_data();
                        //ret = ql_ble_demo_update_conn_param();
                        if (ret != QL_BT_SUCCESS)
                        {
                            ret = ql_ble_demo_disconect_device();
                            if (ret != QL_BT_SUCCESS)
                            {
                                goto QL_BLE_ADV_DEMO_STOP;
                            }
                        }
                    }
                    else 
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }
                   
                }
                else
                {
                    QL_BLE_GATT_LOG("connect failed");
                    goto QL_BLE_ADV_DEMO_STOP; 
                }
            }
            break;
            case QUEC_BLE_GATT_MTU:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("connect mtu sucess:handle=%d,mtu=%d", test_event.param2, test_event.param3);
                    ql_bt_ble_mtu = test_event.param3; 
                }
                else
                {
                    QL_BLE_GATT_LOG("connect mtu failed");
                    goto QL_BLE_ADV_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_DISCONNECT_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("disconnect sucess"); 
                    ql_bt_addr_s * addr = (ql_bt_addr_s *)test_event.param2;
                    if (addr)
                    {
                        unsigned char addr_string[QL_BLE_DEMO_ADDR_MAX_SIZE + 1] = {0};
                        memset(addr_string,0x00,sizeof(addr_string));
                        sprintf((char *)addr_string, "%02x:%02x:%02x:%02x:%02x:%02x", addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5]);
                        QL_BLE_GATT_LOG("addr_string=%s", addr_string); 
                        
                        free(addr);
                    }

                    //ret = ql_ble_demo_disconect_device();
                    //ql_rtos_task_sleep_s(5);
                    goto QL_BLE_ADV_DEMO_STOP;
                }
                else
                {
                    QL_BLE_GATT_LOG("disconnect failed");
                    goto QL_BLE_ADV_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_UPDATE_CONN_PARAM_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("update conn param sucess");
                    ql_ble_update_conn_infos_s *conn_param = (ql_ble_update_conn_infos_s *)test_event.param2;
                    if (conn_param)
                    {
                        QL_BLE_GATT_LOG("conn_id=%d,min_interval=%d,max_interval=%d,latency=%d,timeout=%d", conn_param->conn_id, \
                            conn_param->min_interval, conn_param->max_interval, conn_param->latency, conn_param->timeout);
                        
                        free(conn_param);
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("update conn param failed");
                    goto QL_BLE_ADV_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_GATT_SEND_END:
            {
                //send notification complete
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("send data sucess"); 
                    ret = ql_ble_demo_update_conn_param();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_ADV_DEMO_STOP;
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("send data failed");
                    goto QL_BLE_ADV_DEMO_STOP; 
                }
            }
            break;
            case QUEC_BLE_GATT_RECV_IND:
            {
                //client write characteristic
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("ble recv sucess");
                    ql_ble_gatt_data_s *ble_data = (ql_ble_gatt_data_s *)test_event.param2;
                    if (ble_data && ble_data->data)
                    {
                        QL_BLE_GATT_LOG("ble_data->len=%d,data=%s", ble_data->len, ble_data->data);
                        free(ble_data->data);
                        free(ble_data);
                    }
                    
                    //ret = ql_ble_demo_disconect_device();
                    //if (ret != QL_BT_SUCCESS)
                    //{
                    //    goto QL_BLE_ADV_DEMO_STOP;
                    //}
                }
                else
                {
                    QL_BLE_GATT_LOG("ble recv failed");
                    goto QL_BLE_ADV_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_GATT_RECV_READ_IND:
            {
                //client read characterisc
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("ble recv sucess");
                    ql_ble_gatt_data_s *ble_data = (ql_ble_gatt_data_s *)test_event.param2;
                    if (ble_data && ble_data->data)
                    {
                        if (ble_data->data)
                        {
                            QL_BLE_GATT_LOG("data=%s", ble_data->data);
                            free(ble_data->data);
                        }
                        
                        free(ble_data);
                    }
                    QL_BLE_GATT_LOG("ble_data->len=%d", ble_data->len);
                    //ret = ql_ble_demo_disconect_device();
                    //if (ret != QL_BT_SUCCESS)
                    //{
                    //    goto QL_BLE_ADV_DEMO_STOP;
                    //}
                }
                else
                {
                    QL_BLE_GATT_LOG("ble recv failed");
                    goto QL_BLE_ADV_DEMO_STOP;
                }
            }
            break;
            default:
            break;
        }
        return ret;
QL_BLE_ADV_DEMO_STOP:
#if QL_BLE_DEMO_LOW_POWER_USE
        ql_lpm_wakelock_lock(bt_ble_power_lock);
#endif
        ret = ql_bt_demo_stop();
	}

    return ret;
}

static void ql_ble_notify_cb(void *ind_msg_buf, void *ctx)
{
    ql_event_t *ql_event = NULL;
    
    if (ind_msg_buf == NULL)
    {
        return ;
        
    }

    ql_event = (ql_event_t *)ind_msg_buf;
    if (ql_event->id != 0)
    {
        ql_rtos_event_send(ble_demo_task,ql_event);
    }
}

void ql_ble_gatt_server_demo_task_pthread(void *ctx)
{
	QlOSStatus err = 0;
    ql_errcode_bt_e ret = QL_BT_SUCCESS;
    
#if QL_BLE_DEMO_LOW_POWER_USE
    //ql_dev_set_modem_fun(0, 0, 0);
    ql_autosleep_enable(QL_ALLOW_SLEEP);
    QL_BLE_GATT_LOG("i am in sleep mode");
#endif
    
GATT_SERVER_RETRY:
    ret = ql_ble_gatt_server_init(ql_ble_notify_cb);
    if (ret != QL_BT_SUCCESS)
    {
        goto QL_BLT_GATT_SERVER_NOT_INIT_EXIT;
    }
#if QL_BLE_DEMO_LOW_POWER_USE
    ql_lpm_wakelock_lock(bt_ble_power_lock);
#endif
    ret = ql_bt_demo_start();
    if (ret != QL_BT_SUCCESS)
    {
#if QL_BLE_DEMO_LOW_POWER_USE
        ql_lpm_wakelock_unlock(bt_ble_power_lock);
#endif
       goto QL_BLT_GATT_SERVER_INIT_EXIT;
    }
    
	while(1)
	{
        ret = ql_ble_gatt_server_handle_event();
        if (ret != QL_BT_SUCCESS)
        {
            break;
        }
	}
QL_BLT_GATT_SERVER_INIT_EXIT:
	ql_ble_gatt_server_release();
    goto GATT_SERVER_RETRY;
QL_BLT_GATT_SERVER_NOT_INIT_EXIT:
#if QL_BLE_DEMO_LOW_POWER_USE
    ql_lpm_wakelock_delete(bt_ble_power_lock);
#endif
	err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_BLE_GATT_LOG("task deleted failed");
	}
}

QlOSStatus ql_ble_gatt_server_demo_init(void)
{	
	QlOSStatus err = QL_OSI_SUCCESS;

    QL_BLE_GATT_LOG("enter ql_ble_gatt_server_demo_init");
#if QL_BLE_DEMO_LOW_POWER_USE
    bt_ble_power_lock = ql_lpm_wakelock_create("bt_ble", strlen("bt_ble"));
#endif
	err = ql_rtos_task_create(&ble_demo_task, BT_BLE_DEMO_TASK_STACK_SIZE, BT_BLE_DEMO_TASK_PRIO, "gatt_server", ql_ble_gatt_server_demo_task_pthread, NULL, BT_BLE_DEMO_TASK_EVENT_CNT);
	
    return err;
}

//gatt client
ql_errcode_bt_e ql_blescan_demo_set_param()
{
    ql_errcode_bt_e ret;    
    
    ret = ql_blescan_set_param(ble_scan_param);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_blescan_demo_start()
{
    ql_errcode_bt_e ret = 0;
   
    ret = ql_blescan_start();
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

static ql_errcode_bt_e ql_blescan_demo_stop()
{
    ql_errcode_bt_e ret;
   
    ret = ql_blescan_stop();
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_demo_connect_public_addr()
{
    ql_errcode_bt_e ret;

    ret = ql_ble_conncet_public_addr(ble_scan_report_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_demo_connect_random_addr()
{
    ql_errcode_bt_e ret;

    ret = ql_ble_conncet_random_addr(ble_scan_report_addr);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}


ql_errcode_bt_e ql_ble_gatt_demo_discover_all_service()
{
    ql_errcode_bt_e ret = 0;
   
    ret = ql_ble_gatt_discover_all_service(ble_conn_handle);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_discover_service_by_uuid()
{
    ql_errcode_bt_e ret = 0;
    ql_ble_gatt_uuid_s uuid = {0};

    uuid.uuid_type = ql_ble_gatt_uuid_state;
    if (ql_ble_gatt_uuid_state == QL_BLE_GATT_SHORT_UUID)
    {
        uuid.uuid_s = ql_ble_gatt_target_service.uuid;
        ret = ql_ble_gatt_discover_by_uuid(ble_conn_handle, uuid);
    }
    else 
    {
        memcpy(uuid.uuid_l, ql_ble_gatt_uuid_l, QL_BLE_LONG_UUID_SIZE);
        ret = ql_ble_gatt_discover_by_uuid(ble_conn_handle, uuid);
    }
    
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}



ql_errcode_bt_e ql_ble_gatt_demo_discover_all_includes()
{
    ql_errcode_bt_e ret = 0;
   
    ret = ql_ble_gatt_discover_all_includes( ble_conn_handle, ql_ble_gatt_target_service.start_handle, ql_ble_gatt_target_service.end_handle);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_discover_all_characteristic()
{
    ql_errcode_bt_e ret = 0;
   
    ret = ql_ble_gatt_discover_all_characteristic(ble_conn_handle, ql_ble_gatt_target_service.start_handle, ql_ble_gatt_target_service.end_handle);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_discover_chara_desc()
{
    ql_errcode_bt_e ret = 0;

    QL_BLE_GATT_LOG("value_handle=%x,end_handle=%x", ql_ble_gatt_characteristic[ql_ble_gatt_chara_desc_index].value_handle, 
        ql_ble_gatt_target_service.end_handle);
    QL_BLE_GATT_LOG("ql_ble_gatt_chara_desc_index=%d,ql_ble_gatt_chara_count=%x", ql_ble_gatt_chara_desc_index, ql_ble_gatt_chara_count);

    if (ql_ble_gatt_chara_desc_index == (ql_ble_gatt_chara_count - 1))
    {
        ret = ql_ble_gatt_discover_chara_desc(ble_conn_handle, ql_ble_gatt_characteristic[ql_ble_gatt_chara_desc_index].value_handle+1, 
            ql_ble_gatt_target_service.end_handle);  
    }
    else 
    {
        ret = ql_ble_gatt_discover_chara_desc(ble_conn_handle, ql_ble_gatt_characteristic[ql_ble_gatt_chara_desc_index].value_handle+1, 
            ql_ble_gatt_characteristic[ql_ble_gatt_chara_desc_index+1].handle-1);
    }
    
    ql_ble_gatt_chara_desc_index++;
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_read_chara_value_by_uuid()
{
    ql_errcode_bt_e ret = 0;
    ql_ble_gatt_uuid_s uuid =
    {
        .uuid_type = 1,
        .uuid_l = {0x00},
        .uuid_s = 0,
    };
#if 0
    QL_BT_LOG("uuid=%x,start_handle=%x,end_handle=%x", ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].uuid, ql_ble_gatt_target_service.start_handle, 
        ql_ble_gatt_target_service.end_handle);
   
    ret = ql_ble_gatt_read_chara_value_by_uuid(ble_conn_handle, NULL, ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].uuid, ql_ble_gatt_target_service.start_handle, 
        ql_ble_gatt_target_service.end_handle);
#endif
    QL_BLE_GATT_LOG("uuid=%x,handle=%x,value_handle=%x", ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].uuid, ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].handle, 
            ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].value_handle);
    uuid.uuid_s = ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].uuid;
    ret = ql_ble_gatt_read_chara_value_by_uuid(ble_conn_handle, uuid, ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].handle, 
        ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].value_handle);

    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_read_chara_value_by_handle()
{
    ql_errcode_bt_e ret = 0;
    
    QL_BLE_GATT_LOG("value_handle=%x", ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].value_handle);
    ret = ql_ble_gatt_read_chara_value_by_handle(ble_conn_handle, ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].value_handle, 0, 0);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_read_mul_chara_value()
{
    ql_errcode_bt_e ret = 0;
    
    QL_BLE_GATT_LOG("value_handle=%x", ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].value_handle);
    ret = ql_ble_gatt_read_mul_chara_value(ble_conn_handle, (unsigned char *)&ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].value_handle, 2);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}


ql_errcode_bt_e ql_ble_gatt_demo_read_chara_desc()
{
    ql_errcode_bt_e ret = 0;

    QL_BLE_GATT_LOG("handle=%x", ql_ble_gatt_chara_desc[ql_ble_gatt_chara_cur_desc].handle);
    ret = ql_ble_gatt_read_chara_desc(ble_conn_handle, ql_ble_gatt_chara_desc[ql_ble_gatt_chara_cur_desc].handle, 0);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_write_chara_desc()
{
    ql_errcode_bt_e ret = 0;
    unsigned char value[2] = {0x01,0x02};

    //value[2] = (ble_conn_handle >>8) & 0xFF;
    //value[3] = ble_conn_handle & 0xFF;
    QL_BLE_GATT_LOG("handle=%x", ql_ble_gatt_chara_desc[ql_ble_gatt_chara_cur_desc].handle);
    ret = ql_ble_gatt_write_chara_desc(ble_conn_handle, ql_ble_gatt_chara_desc[ql_ble_gatt_chara_cur_desc].handle, sizeof(value), value);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}


ql_errcode_bt_e ql_ble_gatt_demo_write_chara_value()
{
    ql_errcode_bt_e ret = 0;
    unsigned char value[2] = {0x40};

    QL_BLE_GATT_LOG("value_handle=%x,uuid=%x", ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].value_handle,
        ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].uuid);
    
    ret = ql_ble_gatt_write_chara_value(ble_conn_handle, ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].value_handle, 
        sizeof(value), value, 0, 0);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}

ql_errcode_bt_e ql_ble_gatt_demo_write_chara_value_no_rsp()
{
    ql_errcode_bt_e ret = 0;
    unsigned char value[1] = {0x20};

    QL_BLE_GATT_LOG("value_handle=%x,uuid=%x", ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].value_handle,
        ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].uuid);
    
    ret = ql_ble_gatt_write_chara_value_no_rsp(ble_conn_handle, ql_ble_gatt_characteristic[ql_ble_gatt_cur_chara].value_handle, 
        sizeof(value), value);
    if (ret == QL_BT_SUCCESS)
    {
        QL_BLE_GATT_LOG("sucess");
    }
    else 
    {
        QL_BLE_GATT_LOG("error=%x", ret);
    }

    return ret;
}


static ql_errcode_bt_e ql_ble_gatt_client_handle_event()
{
    ql_event_t test_event;
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
                    QL_BLE_GATT_LOG("start sucess");
#if QL_BLE_DEMO_LOW_POWER_USE
                    ql_lpm_wakelock_unlock(bt_ble_power_lock);
#endif
                    ret = ql_bt_demo_get_state();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_SCAN_DEMO_STOP;
                    }

                    ret = ql_blescan_demo_set_param();
                    if (ret != QL_BT_SUCCESS)
                    { 
                        goto QL_BLE_SCAN_DEMO_STOP;
                    }

                    ret = ql_blescan_demo_start();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_SCAN_DEMO_STOP;
                    } 
                }
                else
                {
                    QL_BLE_GATT_LOG("start failed"); 
                }
            }
            break;
            case QUEC_BT_STOP_STATUS_IND:
            {
#if QL_BLE_DEMO_LOW_POWER_USE
                ql_lpm_wakelock_unlock(bt_ble_power_lock);
#endif
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("stop sucess");
                    ret = QL_BT_ALREADY_STOPED_ERR;
                }
                else
                {
                    QL_BLE_GATT_LOG("stop failed");
                    ret = QL_BT_ALREADY_STOPED_ERR;
                }
            }
            break;
            case QUEC_BLESCAN_REPORT_IND:
            {
                //scan and report other devices 
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("scan report sucess");

                    ql_ble_scan_report_info_s *bt_scan_report = NULL;
                    bt_scan_report = (ql_ble_scan_report_info_s *)test_event.param2;
                    if (bt_scan_report)
                    {
                        char scan_rsp_info[128] = {'\0'};
                        sprintf(scan_rsp_info, "%s:%d,%02x:%02x:%02x:%02x:%02x:%02x", bt_scan_report->name, bt_scan_report->addr_type, bt_scan_report->addr.addr[0], bt_scan_report->addr.addr[1], bt_scan_report->addr.addr[2], bt_scan_report->addr.addr[3], bt_scan_report->addr.addr[4], bt_scan_report->addr.addr[5]);
                        QL_BLE_GATT_LOG("scan_rsp_info=%s", scan_rsp_info);
                        
                        if (0 == memcmp(ble_scan_report_name, bt_scan_report->name, sizeof(ble_scan_report_name)))
                        {
                            ble_scan_report_addr_type = bt_scan_report->addr_type;
                            memcpy(ble_scan_report_addr.addr, bt_scan_report->addr.addr, QL_BT_MAC_ADDRESS_SIZE);
                            ret = ql_blescan_demo_stop();
                            if (ret != QL_BT_SUCCESS)
                            {
                                goto QL_BLE_SCAN_DEMO_STOP;
                            }

                            if (ble_scan_report_addr_type == QL_BLE_PUBLIC_ADDRESS)
                            {
                                ret = ql_ble_demo_connect_public_addr();
                            }
                            else 
                            {
                                ret = ql_ble_demo_connect_random_addr();
                            }
                            if (ret != QL_BT_SUCCESS)
                            {
                                goto QL_BLE_SCAN_DEMO_STOP;
                            }  
                        }
                        free(bt_scan_report);
                        bt_scan_report = NULL;
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("scan report failed");
                    ret = ql_blescan_demo_stop();
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_SCAN_DEMO_STOP;
                    }
                }
            }
            break;
            case QUEC_BLE_CONNECT_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("connect sucess");
                    ql_bt_addr_s * addr = (ql_bt_addr_s *)test_event.param2;
                    if (addr)
                    {
                        //addr need to free
                        unsigned char addr_string[QL_BLE_DEMO_ADDR_MAX_SIZE + 1] = {0};
                        memset(addr_string,0x00,sizeof(addr_string));
                        sprintf((char *)addr_string, "%02x:%02x:%02x:%02x:%02x:%02x", addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5]);
                        QL_BLE_GATT_LOG("addr_string=%s", addr_string); 
                        memcpy(ble_connection_addr.addr, addr->addr, QL_BT_MAC_ADDRESS_SIZE);
                        
                        free(addr);
                    }
                    ble_conn_handle = test_event.param3;
                    QL_BLE_GATT_LOG("ble_conn_handle=%d", ble_conn_handle);
                    ret = ql_ble_demo_get_conection_state();
                    if (ret == QL_BT_SUCCESS)
                    {
                        //ret = ql_ble_demo_send_data();
                        //if (ret != QL_BT_SUCCESS)
                        //{
                        //    goto QL_BLE_SCAN_DEMO_STOP;
                        //}
                    }
                    else 
                    {
                        goto QL_BLE_SCAN_DEMO_STOP;
                    }               
                }
                else
                {
                    QL_BLE_GATT_LOG("connect failed");
                    //goto QL_BLE_SCAN_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_UPDATE_CONN_PARAM_IND:
            {
                //update connection parameter
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("update conn param sucess");
                    ql_ble_update_conn_infos_s *conn_param = (ql_ble_update_conn_infos_s *)test_event.param2;
                    if (conn_param)
                    {
                        QL_BLE_GATT_LOG("conn_id=%d,min_interval=%d,max_interval=%d,latency=%d,timeout=%d", conn_param->conn_id, \
                            conn_param->min_interval, conn_param->max_interval, conn_param->latency, conn_param->timeout);
                        
                        free(conn_param);
                    }
                    #if 0
                    ret = ql_ble_demo_send_data();
                    if (ret == QL_BT_SUCCESS)
                    {
                        ret = ql_ble_demo_disconect_device();
                        if (ret != QL_BT_SUCCESS)
                        {
                            goto QL_BLE_SCAN_DEMO_STOP;
                        }
                    }
                    else 
                    {
                        goto QL_BLE_SCAN_DEMO_STOP;
                    }
                    #endif
                }
                else
                {
                    QL_BLE_GATT_LOG("update conn param failed");
                    goto QL_BLE_SCAN_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_GATT_MTU:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("connect mtu sucess,handle=%d,mtu=%d", test_event.param2, test_event.param3);  
                    ql_bt_ble_mtu = test_event.param3;
                }
                else
                {
                    QL_BLE_GATT_LOG("connect mtu failed");
                    goto QL_BLE_SCAN_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_DISCONNECT_IND:
            {
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("disconnect sucess");
                    ql_bt_addr_s * addr = (ql_bt_addr_s *)test_event.param2;
                    if (addr)
                    {
                        //addr need to free
                        unsigned char addr_string[QL_BLE_DEMO_ADDR_MAX_SIZE + 1] = {0};
                        memset(addr_string,0x00,sizeof(addr_string));
                        sprintf((char *)addr_string, "%02x:%02x:%02x:%02x:%02x:%02x", addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5]);
                        QL_BLE_GATT_LOG("addr_string=%s", addr_string); 
                        
                        free(addr);
                    }
                    goto QL_BLE_SCAN_DEMO_STOP;
                    
                }
                else
                {
                    QL_BLE_GATT_LOG("disconnect failed");
                    goto QL_BLE_SCAN_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_GATT_START_DISCOVER_SERVICE_IND:
            {
                //discover all service or discover service by uuid
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("start discover sucess");
                    ql_ble_gatt_chara_count = 0;
                    ql_ble_gatt_chara_desc_index = 0;
                    ql_ble_gatt_chara_desc_count = 0;
                    ql_ble_gatt_state = QL_BLE_GATT_DISCOVER_SERVICE;
                    if (ql_ble_gatt_discover_service_mode == QL_BLE_GATT_DISCOVER_SERVICE_ALL)
                    {
                        ret = ql_ble_gatt_demo_discover_all_service();
                    }
                    else 
                    {
                        ret = ql_ble_gatt_demo_discover_service_by_uuid();
                    }
                    
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_SCAN_DEMO_STOP;
                    } 
                                         
                }
                else
                {
                    QL_BLE_GATT_LOG("start discover failed");
                    goto QL_BLE_SCAN_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_GATT_DISCOVER_SERVICE_IND:
            {
                //discorver service
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("discover servie sucess");

                    ql_blegatt_prime_service_s *service = (ql_blegatt_prime_service_s *)test_event.param2;
                    if (service)
                    {
                        if (ql_ble_gatt_discover_service_mode == QL_BLE_GATT_DISCOVER_SERVICE_ALL)
                        {
                            QL_BLE_GATT_LOG("start_handle=%x,end_handle=%x,uuid=%x", service->start_handle, service->end_handle, service->uuid);
                                                   
                            if (ql_ble_gatt_target_service.uuid == service->uuid)
                            {
                                ql_ble_gatt_target_service.start_handle = service->start_handle;
                                ql_ble_gatt_target_service.end_handle = service->end_handle;
                                
                                
                                //ql_ble_gatt_demo_discover_all_includes();
                                ql_ble_gatt_state = QL_BLE_GATT_DISCOVER_CHARACTERISTIC;
                                ret = ql_ble_gatt_demo_discover_all_characteristic();
                                if (ret != QL_BT_SUCCESS)
                                {
                                    goto QL_BLE_SCAN_DEMO_STOP;
                                } 
                            }
                        }
                        else 
                        {
                            QL_BLE_GATT_LOG("start_handle=%x,end_handle=%x", service->start_handle, service->end_handle);
                            
                            ql_ble_gatt_target_service.start_handle = service->start_handle;
                            ql_ble_gatt_target_service.end_handle = service->end_handle;
                            ql_ble_gatt_state = QL_BLE_GATT_DISCOVER_CHARACTERISTIC;
                            ret = ql_ble_gatt_demo_discover_all_characteristic();
                            if (ret != QL_BT_SUCCESS)
                            {
                                goto QL_BLE_SCAN_DEMO_STOP;
                            } 
                        }
                       
                        
                        free(service);
                    }
                    
                }
                else
                {
                    QL_BLE_GATT_LOG("discover servie failed");
                    goto QL_BLE_SCAN_DEMO_STOP;
                }
            }
            break; 
            case QUEC_BLE_GATT_DISCOVER_CHARACTERISTIC_DATA_IND:
            {
                //discover characteristic
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("get charactersictic sucess");
                    ql_att_general_rsp_s * last_rsp = (ql_att_general_rsp_s *)test_event.param2;
                    int i = 0;
                    if (last_rsp && last_rsp->pay_load)
                    {
                        if (ql_ble_gatt_state == QL_BLE_GATT_DISCOVER_CHARACTERISTIC)
                        {
                            unsigned char pair_len = last_rsp->pay_load[0];
                            QL_BLE_GATT_LOG("pair_len=%d,last_rsp.length=%d", pair_len, (int)last_rsp->length);
                            while (i < (last_rsp->length - 1) / pair_len)
                            {
                                ql_ble_gatt_characteristic[ql_ble_gatt_chara_count].handle = (last_rsp->pay_load[i * pair_len + 2] << 8) | last_rsp->pay_load[i * pair_len + 1];
                                ql_ble_gatt_characteristic[ql_ble_gatt_chara_count].properties = last_rsp->pay_load[i * pair_len + 3];
                                ql_ble_gatt_characteristic[ql_ble_gatt_chara_count].value_handle = (last_rsp->pay_load[i * pair_len + 5] << 8) | last_rsp->pay_load[i * pair_len + 4];
                                
                                if (pair_len == QL_BLE_SHORT_UUID_PAIR_LEN)
                                {
                                    ql_ble_gatt_characteristic[ql_ble_gatt_chara_count].uuid = (last_rsp->pay_load[i * pair_len + 7] << 8) | last_rsp->pay_load[i * pair_len + 6];
                                    QL_BLE_GATT_LOG("uuid = %x", ql_ble_gatt_characteristic[ql_ble_gatt_chara_count].uuid);
                                }
                                else if (pair_len == QL_BLE_LONG_UUID_PAIR_LEN)
                                {
                                    memcpy(ql_ble_gatt_uuid_l, &last_rsp->pay_load[i * pair_len + 6], QL_BLE_LONG_UUID_SIZE);
                                }
                                QL_BLE_GATT_LOG("handle=0x%x,properties=0x%x,value_handle=0x%x", ql_ble_gatt_characteristic[ql_ble_gatt_chara_count].handle, 
                                    ql_ble_gatt_characteristic[ql_ble_gatt_chara_count].properties, ql_ble_gatt_characteristic[ql_ble_gatt_chara_count].value_handle);        
                                i++;
                                
                                if (ql_ble_gatt_chara_count < QL_BLE_CHARA_NUM_MAX)
                                {
                                    ql_ble_gatt_chara_count++;
                                }
                                 
                            }

                            QL_BLE_GATT_LOG("ql_ble_gatt_chara_count=%x", ql_ble_gatt_chara_count);
                        }
                        else if (ql_ble_gatt_state == QL_BLE_GATT_READ_CHARA_VALUE)
                        {
                            QL_BLE_GATT_LOG("last_rsp.length=%d", (int)last_rsp->length);
                            QL_BLE_GATT_LOG("last_rsp.pay_load=%x,%x,%x,%x", last_rsp->pay_load[0], last_rsp->pay_load[1], last_rsp->pay_load[2], last_rsp->pay_load[3]);
                        }
                        

                        free(last_rsp->pay_load);
                        free(last_rsp);                       
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("get charactersictic failed");
                    goto QL_BLE_SCAN_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_GATT_DISCOVER_CHARA_DESC_IND:
            {
                //discover characteristic descriptor
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("get chara desc sucess");

                    ql_att_general_rsp_s * last_rsp = (ql_att_general_rsp_s *)test_event.param2;
                    if (last_rsp && last_rsp->pay_load)
                    {
                        
                        unsigned char fmt = last_rsp->pay_load[0];
                        unsigned char length = last_rsp->length;
                        int i = 0;
                    
                        QL_BLE_GATT_LOG("fmt=%d,length=%d", fmt, length);
                        //fmt:1 - 16 bit uuid
                        if (fmt == 1)
                        {
                            while (i < (length - 1) / 4)
                            {
                                ql_ble_gatt_chara_desc[ql_ble_gatt_chara_cur_desc].handle= (last_rsp->pay_load[i * 4 + 2] << 8) | last_rsp->pay_load[i * 4 + 1];
                                ql_ble_gatt_chara_desc[ql_ble_gatt_chara_cur_desc].uuid= (last_rsp->pay_load[i * 4 + 4] << 8) | last_rsp->pay_load[i * 4 + 3];
                                QL_BLE_GATT_LOG("handle=%x,uuid=%x", ql_ble_gatt_chara_desc[ql_ble_gatt_chara_cur_desc].handle, ql_ble_gatt_chara_desc[ql_ble_gatt_chara_cur_desc].uuid);
                                
                                i++;
                                if (ql_ble_gatt_chara_desc_count < QL_BLE_DESC_NUM_MAX)
                                {
                                    ql_ble_gatt_chara_desc_count++;
                                }
                            }
                        }
                        QL_BLE_GATT_LOG("ql_ble_gatt_chara_desc_count=%x", ql_ble_gatt_chara_desc_count);
                        free(last_rsp->pay_load);
                        free(last_rsp);
                    }

                    
                    if (ql_ble_gatt_chara_desc_index == ql_ble_gatt_chara_count)
                    {
                        ql_ble_gatt_state = QL_BLE_GATT_WRITE_CHARA_VALUE;
                        //ret = ql_ble_gatt_demo_write_chara_value();
                        //ret = ql_ble_gatt_demo_write_chara_value_no_rsp();
                        
                        ql_ble_gatt_state = QL_BLE_GATT_READ_CHARA_VALUE;
                        ret = ql_ble_gatt_demo_read_chara_value_by_uuid();
                        //ret = ql_ble_gatt_demo_read_chara_value_by_handle();
                        //ret = ql_ble_gatt_demo_read_mul_chara_value();
                        
                        //ql_ble_gatt_state = QL_BLE_GATT_READ_CHARA_DESC;
                        //ret = ql_ble_gatt_demo_read_chara_desc();

                        //ql_ble_gatt_state = QL_BLE_GATT_WRITE_CHARA_DESC;
                        //ret = ql_ble_gatt_demo_write_chara_desc();
                    }
                    else 
                    {
                        ret = ql_ble_gatt_demo_discover_chara_desc();
                    }
                    
                    if (ret != QL_BT_SUCCESS)
                    {
                        goto QL_BLE_SCAN_DEMO_STOP;
                    } 
                }
                else
                {
                    QL_BLE_GATT_LOG("get chara desc failed");
                    goto QL_BLE_SCAN_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_GATT_CHARA_WRITE_WITH_RSP_IND:
            case QUEC_BLE_GATT_DESC_WRITE_WITH_RSP_IND:
            {
                //write charactersictic value with response
                //write charactersictic descricptor with response
                QL_BLE_GATT_LOG("chara write sucess");
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    if (ql_ble_gatt_state == QL_BLE_GATT_WRITE_CHARA_VALUE)
                    {
                    }
                    else if (ql_ble_gatt_state == QL_BLE_GATT_WRITE_CHARA_DESC)
                    {
                    }                
                }
                else
                {
                    QL_BLE_GATT_LOG("chara write failed");
                }
            }
            break;
            case QUEC_BLE_GATT_CHARA_WRITE_WITHOUT_RSP_IND:
            {
                //write characteristic value without response
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("no rsp chara write sucess");
                }
                else
                {
                    QL_BLE_GATT_LOG("no rsp chara write failed");
                }
            }
            break;
            case QUEC_BLE_GATT_CHARA_READ_IND:
            case QUEC_BLE_GATT_DESC_READ_IND:
            {
                //read characteristic value by handle
                //read characteristic descriptor
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("chara read sucess");
                    ql_att_general_rsp_s * last_rsp = (ql_att_general_rsp_s *)test_event.param2;
                    if (last_rsp && last_rsp->pay_load)
                    {
                        unsigned short length = last_rsp->length;
                        QL_BLE_GATT_LOG("length=%d", length);
                        if (ql_ble_gatt_state == QL_BLE_GATT_READ_CHARA_VALUE)
                        {
                            QL_BLE_GATT_LOG("last_rsp.pay_load=%x", last_rsp->pay_load[0]);
                        }
                        else if (ql_ble_gatt_state == QL_BLE_GATT_READ_CHARA_DESC)
                        {
                            QL_BLE_GATT_LOG("last_rsp.pay_load=%x,%x", last_rsp->pay_load[0], last_rsp->pay_load[1]);
                        }
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("chara read failed");
                }
            }
            break;
            case QUEC_BLE_GATT_CHARA_READ_BY_UUID_IND:
            {
                //read characteristic value by uuid
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("chara read sucess");
                    ql_att_general_rsp_s * last_rsp = (ql_att_general_rsp_s *)test_event.param2;
                    if (last_rsp && last_rsp->pay_load)
                    {
                        unsigned short length = length;
                        QL_BLE_GATT_LOG("length=%d", length);
                        unsigned short handle = (last_rsp->pay_load[2] << 8) | last_rsp->pay_load[1];
                        QL_BLE_GATT_LOG("handle=%d", handle);
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("chara read failed");
                }
            }
            break;
            case QUEC_BLE_GATT_CHARA_MULTI_READ_IND:
            {
                //read miltiple characteristic value
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("chara multi read sucess");
                    ql_att_general_rsp_s * last_rsp = (ql_att_general_rsp_s *)test_event.param2;
                    if (last_rsp && last_rsp->pay_load)
                    {
                        unsigned char length = last_rsp->length;
                        QL_BLE_GATT_LOG("length=%d", length);
                        QL_BLE_GATT_LOG("last_rsp.pay_load=%x", last_rsp->pay_load[0]);
                        
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("chara multi read failed");
                }
            }
            break;
            case QUEC_BLE_GATT_ATT_ERROR_IND:
            {
                //attribute error
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    UINT8 error_code = test_event.param2;
                    QL_BLE_GATT_LOG("get att error sucess error_code=%x", error_code);
                    if (ql_ble_gatt_state == QL_BLE_GATT_DISCOVER_INCLUDES)
                    {
                        ql_ble_gatt_state = QL_BLE_GATT_DISCOVER_CHARACTERISTIC;
                        ret = ql_ble_gatt_demo_discover_all_characteristic();
                        if (ret != QL_BT_SUCCESS)
                        {
                            goto QL_BLE_SCAN_DEMO_STOP;
                        } 
                    }
                    else if (ql_ble_gatt_state == QL_BLE_GATT_DISCOVER_CHARACTERISTIC)
                    {
                        ql_ble_gatt_state = QL_BLE_GATT_IDLE;
                        ret = ql_ble_gatt_demo_discover_chara_desc();
                        if (ret != QL_BT_SUCCESS)
                        {
                            goto QL_BLE_SCAN_DEMO_STOP;
                        } 
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("get att error failed");
                    goto QL_BLE_SCAN_DEMO_STOP;
                }
            }
            break;
            case QUEC_BLE_GATT_RECV_NOTIFICATION_IND:
            {
                //recieve notification
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("recv notifi sucess");
                    ql_att_general_rsp_s * last_rsp = (ql_att_general_rsp_s *)test_event.param2;
                    if (last_rsp && last_rsp->pay_load)
                    {
                        unsigned short length = last_rsp->length;
                        
                        QL_BLE_GATT_LOG("length=%d", length);
                        QL_BLE_GATT_LOG("last_rsp.pay_load=%x,%x,%x,%x", last_rsp->pay_load[0], last_rsp->pay_load[1], last_rsp->pay_load[2], last_rsp->pay_load[3]);
                        
                        free(last_rsp->pay_load);
                        free(last_rsp);
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("recv notifi failed");
                }
            }
            break;
            case QUEC_BLE_GATT_RECV_INDICATION_IND:
            {
                //recieve indication
                if (QL_BT_STATUS_SUCCESS == status)
                {
                    QL_BLE_GATT_LOG("recv notifi sucess");
                    ql_att_general_rsp_s * last_rsp = (ql_att_general_rsp_s *)test_event.param2;
                    if (last_rsp && last_rsp->pay_load)
                    {
                        unsigned short length = last_rsp->length;
                        
                        QL_BLE_GATT_LOG("length=%d", length);
                        QL_BLE_GATT_LOG("last_rsp.pay_load=%x,%x,%x,%x", last_rsp->pay_load[0], last_rsp->pay_load[1], last_rsp->pay_load[2], last_rsp->pay_load[3]);
                        
                        free(last_rsp->pay_load);
                        free(last_rsp);
                    }
                }
                else
                {
                    QL_BLE_GATT_LOG("recv notifi failed");
                }
            }
            break;
            default:
            break;
        }
        return ret;
QL_BLE_SCAN_DEMO_STOP:
#if QL_BLE_DEMO_LOW_POWER_USE
        ql_lpm_wakelock_lock(bt_ble_power_lock);
#endif
        ret = ql_bt_demo_stop();
	}

    return ret;
}

static void ql_ble_gatt_client_demo_task_pthread(void *ctx)
{
	QlOSStatus err = 0;
    ql_errcode_bt_e ret = QL_BT_SUCCESS;
    
#if QL_BLE_DEMO_LOW_POWER_USE
    ql_dev_set_modem_fun(0, 0, 0);
    ql_autosleep_enable(QL_ALLOW_SLEEP);
    QL_BLE_GATT_LOG("i am in sleep mode");
#endif
GATT_CLIENT_RETRY:   
    ret = ql_ble_gatt_client_init(ql_ble_notify_cb);
    if (ret != QL_BT_SUCCESS)
    {
        goto QL_BLE_GATT_CLINET_NOT_INIT_EXIT;
    }
    
#if QL_BLE_DEMO_LOW_POWER_USE
    ql_lpm_wakelock_lock(bt_ble_power_lock);
#endif
    ret = ql_bt_demo_start();
    if (ret != QL_BT_SUCCESS)
    {
#if QL_BLE_DEMO_LOW_POWER_USE
        ql_lpm_wakelock_unlock(bt_ble_power_lock);
#endif
        goto QL_BLE_GATT_CLINET_INIT_EXIT;
    }
    
	while(1)
	{
        ret = ql_ble_gatt_client_handle_event(); 
        if (ret != QL_BT_SUCCESS)
        {
            break;
        }
	}
    
QL_BLE_GATT_CLINET_INIT_EXIT:
    ql_ble_gatt_client_release();
    goto GATT_CLIENT_RETRY;
QL_BLE_GATT_CLINET_NOT_INIT_EXIT:	
#if QL_BLE_DEMO_LOW_POWER_USE
    ql_lpm_wakelock_delete(bt_ble_power_lock);
#endif
	err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_BLE_GATT_LOG("task deleted failed");
	}
}

QlOSStatus ql_ble_gatt_client_demo_init(void)
{	
	QlOSStatus err = QL_OSI_SUCCESS;

    QL_BLE_GATT_LOG("enter ql_ble_gatt_client_demo_init");
#if QL_BLE_DEMO_LOW_POWER_USE
    bt_ble_power_lock = ql_lpm_wakelock_create("bt_ble", strlen("bt_ble"));
#endif
	err = ql_rtos_task_create(&ble_demo_task, BT_BLE_DEMO_TASK_STACK_SIZE, BT_BLE_DEMO_TASK_PRIO, "gatt_client", ql_ble_gatt_client_demo_task_pthread, NULL, BT_BLE_DEMO_TASK_EVENT_CNT);
	
    return err;
}

