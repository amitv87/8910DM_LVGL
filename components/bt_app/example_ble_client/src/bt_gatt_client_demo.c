/**
 * @file     bt_gatt_client_demo.c
 * @brief    gatt client demo
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020-08-22
 * @license  Copyright (C) 2020 Unisoc Communications Inc
 */

// #include "osi.h"  // build err
#include <stdio.h>
#include <stdlib.h>
#include "vfs.h"
#include <stdint.h>
// #include "utrace_dsp_log.h"  // build err
#include "osi_log.h"
#include "bluetooth/bt_driver.h"
#include "bluetooth/bt_gatt.h"
#include "bluetooth/bluetooth.h"
#include "sci_types.h"
#include "bt_abs.h"
#include "bluetooth/bt_types.h"
#include "bt_gatt_client_demo.h"
#include "bt_app.h"
#include "ddb.h"

#if GATT_CLIENT_DEMO_ENABLE

UINT16 gatt_client_state = GATT_CLIENT_LINK_DISCONNECT;
UINT16 ble_handle_c = 0xFF;

gatt_prime_service_t gatt_client_prime_service;

gatt_chara_t gatt_client_chara[5];
uint8 chara_count = 0;

gatt_chara_desc_t gatt_client_chara_desc[5];
uint8 chara_desc_count = 0;

uint8 gatt_client_demo_init = FALSE;
uint8 gatt_client_role = FALSE;
uint8 find_info_index = 0;
uint8 config_index = 0;
osiTimer_t *gatt_client_timer = NULL;

uint16 target_uuid = 0x180F;


void Gatt_client_demo_run_statemachine(GATT_CLIENT_STATE next_state)
{
    UINT8 test_value[2] = {0};
    SCI_TRACE_LOW("[BLE_client] Gatt_client_demo_run_statemachine = 0x%x", next_state);
    if((gatt_client_state == GATT_CLIENT_LINK_DISCONNECT) ||
      (gatt_client_state == GATT_CLIENT_LINK_CONNECTING))
    {
        SCI_TRACE_LOW("[BLE_client] Gatt_client_demo_run_statemachine - state error- state= 0x%x",gatt_client_state);
        return;
    }

    switch (next_state)
    {
    case GATT_CLIENT_DISCOVER_PRME_SER:
        gatt_client_state = GATT_CLIENT_DISCOVER_PRME_SER;
        ble_client_discover_primary_service_by_uuid(BT_MODULE_GATT, NULL, target_uuid, ble_handle_c);
        break;
    case GATT_CLIENT_READ_INCLUDE:
        gatt_client_state = GATT_CLIENT_READ_INCLUDE;
        ble_client_find_include_service(BT_MODULE_GATT, gatt_client_prime_service.startHandle, gatt_client_prime_service.endHandle, ble_handle_c);
        break;
    case GATT_CLIENT_READ_CHARA_DEC:
        gatt_client_state = GATT_CLIENT_READ_CHARA_DEC;
        ble_client_discover_all_characteristic(BT_MODULE_GATT, gatt_client_prime_service.startHandle, gatt_client_prime_service.endHandle, ble_handle_c);
        break;
    case GATT_CLIENT_FIND_INFO:
        gatt_client_state = GATT_CLIENT_FIND_INFO;

        while (((find_info_index + 1) < gatt_client_prime_service.charNum) && ((gatt_client_chara[find_info_index].valueHandle + 1) == gatt_client_chara[find_info_index + 1].handle))
        {
            find_info_index++;
        }

        //find information
        //midle of chara
        SCI_TRACE_LOW("[BLE_client] find_info_index = 0x%x", find_info_index);
        SCI_TRACE_LOW("[BLE_client] chara_count = 0x%x", chara_count);
        if (find_info_index < (chara_count - 1))
        {
            SCI_TRACE_LOW("[BLE_client] (gatt_client_chara[find_info_index].valueHandle + 1) = 0x%x", (gatt_client_chara[find_info_index].valueHandle + 1));
            SCI_TRACE_LOW("[BLE_client] (gatt_client_chara[find_info_index + 1].handle -1) = 0x%x", (gatt_client_chara[find_info_index + 1].handle - 1));
            ble_client_get_char_descriptor(BT_MODULE_GATT, (gatt_client_chara[find_info_index].valueHandle + 1), (gatt_client_chara[find_info_index + 1].handle - 1), ble_handle_c);
        }
        else //end of chara
        {
            if ((find_info_index < chara_count)&&
            (gatt_client_prime_service.endHandle >= (gatt_client_chara[find_info_index].valueHandle + 1)))
            {
                SCI_TRACE_LOW("[BLE_client] (gatt_client_chara[find_info_index].valueHandle + 1) = 0x%x", (gatt_client_chara[find_info_index].valueHandle + 1));
                SCI_TRACE_LOW("[BLE_client] gatt_client_prime_service.endHandle = 0x%x", gatt_client_prime_service.endHandle);
                ble_client_get_char_descriptor(BT_MODULE_GATT, (gatt_client_chara[find_info_index].valueHandle + 1), gatt_client_prime_service.endHandle, ble_handle_c);
            }
            else
            {
                gatt_client_state = GATT_CLIENT_SET_CONFIG;
                Gatt_client_demo_run_statemachine(GATT_CLIENT_SET_CONFIG);
            }
        }
        break;
    case GATT_CLIENT_SET_CONFIG:
        test_value[0] = 0x01;
        test_value[1] = 0x00;
        for (; config_index < chara_desc_count; config_index++)
        {
            if (gatt_client_chara_desc[config_index].uuid == ATT_UUID_CLIENT)
            {
                ble_client_write_char_value(BT_MODULE_GATT, gatt_client_chara_desc[config_index].handle, test_value, 2, ble_handle_c, 0, 0);
            }
        }
        break;
    default:
        break;
    }

}

void Gatt_client_demo_start_client(void *pArgs)
{
    SCI_TRACE_LOW("[BLE_client] Gatt_client_demo_start_client");
    osiTimerDelete(gatt_client_timer);
    gatt_client_timer = NULL;
    bt_SendMsgToSchedTask(BT_START_GATT_CLIENT_MSG, NULL);
}

void Gatt_client_demo_mtu_exchange_result_cb(UINT16 handle, UINT16 mtu)
{
    SCI_TRACE_LOW("[BLE_client]mtu_exchange - MTU = 0x%x", mtu);
}

bt_status_t Gatt_client_demo_connection_cb(int conn_id, int connected, bdaddr_t *addr)
{
    char rsp_buf[32];

    switch (connected)
    {
    case TRUE: //GATT_CONNECT_CNF:
        SCI_TRACE_LOW("[BLE_client]CLIENT: GATT_CONNECT_CNF");
        SCI_TRACE_LOW("[BLE_client]CLIENT: handle is %d", conn_id);
        ble_handle_c = conn_id;
        gatt_client_state = GATT_CLIENT_LINK_CONNECTED;

        //print state to uart
        memset(rsp_buf,0,sizeof(rsp_buf));
        sprintf(rsp_buf, "%s\r\n", "connect OK");
        bt_app_print_data(rsp_buf, strlen(rsp_buf));

        if ((gatt_client_role == TRUE) && (gatt_client_demo_init == TRUE))
        {
            //after connected 500 ms,we discover all primary service
            if (gatt_client_timer == NULL)
                gatt_client_timer= osiTimerCreate(NULL,Gatt_client_demo_start_client,NULL);
            osiTimerStart(gatt_client_timer,500);
        }

        break;
    case FALSE: //GATT_DISCONNECT_IND:
        SCI_TRACE_LOW("[BLE_client]CLIENT: GATT_DISCONNECT_IND");

        ble_handle_c = 0xFF;
        gatt_client_state = GATT_CLIENT_LINK_DISCONNECT;

        chara_count = 0;
        chara_desc_count = 0;
        find_info_index = 0;
        config_index = 0;

        gatt_client_role = FALSE;
        gatt_client_demo_init = FALSE;
        gatt_client_timer = NULL;

#ifdef CONFIG_AT_BT_APP_SUPPORT
        //print state to uart
        memset(rsp_buf,0,sizeof(rsp_buf));
        sprintf(rsp_buf, "%s\r\n", "disconnected");
        bt_app_print_data(rsp_buf, strlen(rsp_buf));
#endif
        break;

    default:
        break;
    }
    return BT_SUCCESS;
}

bt_status_t Gatt_client_demo_discover_service_by_uuid_cb(gatt_prime_service_t *param)
{
    gatt_prime_service_t *p_service_info = (gatt_prime_service_t *)param;

    gatt_client_prime_service.startHandle = p_service_info->startHandle;
    gatt_client_prime_service.endHandle = p_service_info->endHandle;
    gatt_client_prime_service.uuid = target_uuid;
    SCI_TRACE_LOW("[BLE_client] Gatt_client_demo_discover_service_by_uuid_cb");
    SCI_TRACE_LOW("[BLE_client] startHandle = 0x%x", gatt_client_prime_service.startHandle);
    SCI_TRACE_LOW("[BLE_client] endHandle = 0x%x", gatt_client_prime_service.endHandle);

    Gatt_client_demo_run_statemachine(GATT_CLIENT_READ_INCLUDE);
    return BT_SUCCESS;
}

bt_status_t Gatt_client_demo_discover_service_all_cb(gatt_prime_service_t *param)
{

    SCI_TRACE_LOW("[BLE_client] Gatt_client_demo__discover_service_all_cb");

    return BT_SUCCESS;
}

bt_status_t Gatt_client_demo_char_des_data_cb(att_server_t *param)
{
    att_server_t *pServer = (att_server_t *)param;
    UINT8 i = 0;
    UINT8 fmt = pServer->lastRsp.payLoad[0];
    UINT8 length = pServer->lastRsp.length;

    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_char_des_data_cb");
    //fmt:1 - 16 bit uuid
    if (fmt == 1)
    {
        while (i < (length - 1) / 4)
        {
            gatt_client_chara_desc[chara_desc_count].handle = (pServer->lastRsp.payLoad[i * 4 + 2] << 8) | pServer->lastRsp.payLoad[i * 4 + 1];
            gatt_client_chara_desc[chara_desc_count].uuid = (pServer->lastRsp.payLoad[i * 4 + 4] << 8) | pServer->lastRsp.payLoad[i * 4 + 3];
            chara_desc_count++;
            i++;
        }
        //update number
        gatt_client_chara[find_info_index].descNum = gatt_client_chara[find_info_index].descNum + i;
        //end of find infor of this time
        if ((gatt_client_chara_desc[chara_desc_count - 1].handle == gatt_client_prime_service.endHandle) || (gatt_client_chara_desc[chara_desc_count - 1].handle == (gatt_client_chara[find_info_index + 1].handle - 1)))
        {
            //update list
            gatt_client_chara[find_info_index].pDescList = &gatt_client_chara_desc[chara_desc_count - gatt_client_chara[find_info_index].descNum];
            find_info_index++;
            Gatt_client_demo_run_statemachine(GATT_CLIENT_FIND_INFO);
        }
    }
    return BT_SUCCESS;
}

bt_status_t Gatt_client_demo_char_data_cb(att_server_t *param, UINT8 more_data)
{
    att_server_t *pServer = (att_server_t *)param;
    UINT8 i = 0;
    UINT8 pair_len = pServer->lastRsp.payLoad[0];
    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_char_data_cb");

    while (i < (pServer->lastRsp.length - 1) / pair_len)
    {
        gatt_client_chara[chara_count].handle = (pServer->lastRsp.payLoad[i * pair_len + 2] << 8) | pServer->lastRsp.payLoad[i * pair_len + 1];
        gatt_client_chara[chara_count].properties = pServer->lastRsp.payLoad[i * pair_len + 3];
        gatt_client_chara[chara_count].valueHandle = (pServer->lastRsp.payLoad[i * pair_len + 5] << 8) | pServer->lastRsp.payLoad[i * pair_len + 4];
        gatt_client_chara[chara_count].uuid = (pServer->lastRsp.payLoad[i * pair_len + 7] << 8) | pServer->lastRsp.payLoad[i * pair_len + 6];
        SCI_TRACE_LOW("[BLE_client] att_handle = 0x%x", gatt_client_chara[chara_count].handle);
        SCI_TRACE_LOW("[BLE_client] properties = 0x%x", gatt_client_chara[chara_count].properties);
        SCI_TRACE_LOW("[BLE_client] value_handle = 0x%x", gatt_client_chara[chara_count].valueHandle);
        SCI_TRACE_LOW("[BLE_client] char_uuid = 0x%x", gatt_client_chara[chara_count].uuid);

        chara_count++;
        i++;
    }

    gatt_client_prime_service.charNum = gatt_client_prime_service.charNum + i;
    SCI_TRACE_LOW("[BLE_client] charNum = 0x%x", gatt_client_prime_service.charNum);
    //read this prime service characteristic dedaration done
    //todo check (chara_count-1)<0
    if (gatt_client_chara[chara_count - 1].valueHandle == gatt_client_prime_service.endHandle)
    {
        //find information request
        find_info_index = 0;
        Gatt_client_demo_run_statemachine(GATT_CLIENT_FIND_INFO);
    }
    return BT_SUCCESS;
}

bt_status_t Gatt_client_demo_read_cb(att_server_t *param)
{
    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_read_cb");
    att_server_t *pServer = (att_server_t *)param;
    UINT16 i = pServer->lastRsp.length;
    UINT16 j = 0;
    for(; i > 0; i--)
    {
        SCI_TRACE_LOW("[BLE_client] value [%d] = 0x%x", j, pServer->lastRsp.payLoad[j]);
        j++;
    }
    return BT_SUCCESS;
}

bt_status_t Gatt_client_demo_read_blob_cb(att_server_t *param)
{
    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_read_blob_cb");
    return BT_SUCCESS;
}

bt_status_t Gatt_client_demo_read_multi_cb(att_server_t *param)
{
    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_read_multi_cb");
    return BT_SUCCESS;
}

/*
lastNoti.payLoad[0 -- 1] - att handle = lastNoti.payLoad[1] << 16 | lastNoti.payLoad[0]
lastNoti.payLoad[2 -- x] -data
*/
bt_status_t Gatt_client_demo_recv_notification_cb(att_server_t *param)
{
    SCI_TRACE_LOW("[BLE_client]bt_vendor_cli_recv_notification_cb_demo");
    att_server_t *pServer = (att_server_t *)param;
    UINT16 i = pServer->lastNoti.length;
    UINT16 j = 0;
    for(; i > 0; i--)
    {
        SCI_TRACE_LOW("[BLE_client] value [%d] = 0x%x", j, pServer->lastNoti.payLoad[j]);
        j++;
    }
    return BT_SUCCESS;
}

bt_status_t Gatt_client_demo_recv_indication_cb(att_server_t *param)
{
    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_recv_indication_cb");
    return BT_SUCCESS;
}

bt_status_t Gatt_client_demo_write_cb(att_req_pdu_t *param)
{
    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_write_cb");
    return BT_SUCCESS;
}

bt_status_t Gatt_client_demo_write_rsp_cb(UINT16 *param)
{
    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_write_rsp_cb");
#if 0 //special data for special remote devices
    uint8 test_value1[46] = {0xef,0xfe,0x00,0x28,0xc2,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x38,0x66,0xb1,0x7e,0x11,0x9b,0x1e,0xb3,0x72,0x20,0xf8,0x3b,0xbf,
    0x85,0xc0,0x93,0xb5,0x92,0x6d,0x66,0x2d,0x8c,0xba,0xf2,0x00,0xf9,0x1b,0xf1,
    0x61,0xf5,0x1b,0x1d,0xcc,0x6f};
    ble_client_write_char_value_without_rsp(BT_MODULE_GATT,16,test_value1,46,ble_handle_c);
#endif
    return BT_SUCCESS;
}
/*
pdu[0]:event type
pdu[1]:address type
pdu[2--7]:peer address
pdu[8]:data length
pdu[9--(8 + data_lenght)]:data
pdu[9 + data_lenght]:rssi
*/
bt_status_t Gatt_client_demo_scan_cb(UINT8 *pdu, UINT8 len)
{
    char rsp_buf[30];
    UINT8 length = 0;
    UINT8 index = 0;
    UINT8 type = 0;
    UINT8 ext_num = 0;
    UINT8 rssi;
    char target_name[13] = "UNISOC-8910";
    bdaddr_t addr;
    st_scan_report_info pstScanRspInfo = {0};

    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_scan_cb");
    pstScanRspInfo.event_type = pdu[0];
    pstScanRspInfo.addr_type = pdu[1];
    pstScanRspInfo.data_length = pdu[8];
    rssi= pdu[9+pstScanRspInfo.data_length];
    memcpy(pstScanRspInfo.bdAddress.addr, (pdu + 2), SIZE_OF_BDADDR);
    memcpy(addr.bytes, (pdu + 2), SIZE_OF_BDADDR);
    //get name and name lenght in adv data
    while (index < pstScanRspInfo.data_length)
    {
        length = pdu[9 + index];
        type = pdu[10 + index];
        if (type == 0x08 || type == 0x09)
        {
            pstScanRspInfo.name_length = length - 1;
            memcpy(pstScanRspInfo.name, &pdu[index + 11], pstScanRspInfo.name_length);
            pstScanRspInfo.name[pstScanRspInfo.name_length] = '\0';
            ext_num++;
        }
        if (ext_num < 2)
        {
            index += (length + 1);
        }
        else
        {
            break;
        }
     }

    SCI_TRACE_LOW("[BLE_client] scan_cb - event_type : 0x%2x\n", pstScanRspInfo.event_type);
    SCI_TRACE_LOW("[BLE_client] scan_cb - addr_type : 0x%2x\n", pstScanRspInfo.addr_type);
    SCI_TRACE_LOW("[BLE_client] scan_cb - addr : 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x",
    pstScanRspInfo.bdAddress.addr[0], pstScanRspInfo.bdAddress.addr[1], pstScanRspInfo.bdAddress.addr[2], pstScanRspInfo.bdAddress.addr[3], pstScanRspInfo.bdAddress.addr[4], pstScanRspInfo.bdAddress.addr[5]);
   //get rssi
    UINT8 rssi_flag = rssi >> 7;
    rssi = rssi & 0x7f;
    if(rssi_flag == 1)
    {
        pstScanRspInfo.rssi = 128-rssi;
        SCI_TRACE_LOW("[BLE_client] RSSI:-%ddBm", pstScanRspInfo.rssi);
    }
    else
    {
     pstScanRspInfo.rssi =  rssi;
     SCI_TRACE_LOW("[BLE_client] RSSI:%ddBm", pstScanRspInfo.rssi);
    }
#if BLE_SMP_SUPPORT
    /*if the slave is found in device list(means you had connected with it before), so you can reconnect
     *with it now. for smp function, it's different from the first connect. beacuse in first connect, you
     *have to generate the key(ltk/irk) before start encrypt. but in the reconnect, the key exists in device
     *list, so the stack will start encrypt directly after connect successfully.
     */
    if ((gatt_client_state == GATT_CLIENT_LINK_DISCONNECT) && (ble_smp_ida_calc(&addr) != NULL))
    {
        SCI_TRACE_LOW("[BLE_client]disable ble scan");
        ble_scan_disable();

        //unencrypted adv
        if ((pstScanRspInfo.addr_type == 0) || (pstScanRspInfo.addr_type == 1))
        {
            SCI_TRACE_LOW("[BLE_client]ida addr find in device list");
            //for unencrypted adv, we should remove it from resolving list before connect
            ble_smp_remove_device_from_resolving_list(addr);
            //connect with slave, you can also use ble_connect
            ble_connect_ext((bdaddr_t *)pstScanRspInfo.bdAddress.addr, BT_MODULE_GATT, pstScanRspInfo.addr_type, 0);
        }
        //encrypted adv
        else if ((pstScanRspInfo.addr_type == 2) || (pstScanRspInfo.addr_type == 3))
        {
            SCI_TRACE_LOW("[BLE_client]rpa addr find in device list");
            /*connect with encrypted slave, you can not use ble_connect,
             *if the own address type of master is 0(public), you should set the fourth param(own_addr_type) to 2,
             *if the own address type of master is 1(random), you should set the fourth param(own_addr_type) to 3.
             */
            ble_connect_ext((bdaddr_t *)pstScanRspInfo.bdAddress.addr, BT_MODULE_GATT, pstScanRspInfo.addr_type, 2);
        }

        gatt_client_state = GATT_CLIENT_LINK_CONNECTING;
        return BT_SUCCESS;
    }
#endif
    if ((0 != len) && (gatt_client_state == GATT_CLIENT_LINK_DISCONNECT))
    {
       SCI_TRACE_LOW("[BLE_client] scan_cb -device_name = %s", pstScanRspInfo.name);
       if (!strncasecmp(target_name, (char *)pstScanRspInfo.name, strlen(target_name)))
       {
           SCI_TRACE_LOW("[BLE_client] creat ble connecton with: %s", pstScanRspInfo.name);
           ble_scan_disable();
           ble_connect((bdaddr_t *)pstScanRspInfo.bdAddress.addr, BT_MODULE_GATT, pstScanRspInfo.addr_type);
           gatt_client_state = GATT_CLIENT_LINK_CONNECTING;
           memset(rsp_buf,0,sizeof(rsp_buf));
           sprintf(rsp_buf, "%s\r\n", "scan OK");
           bt_app_print_data(rsp_buf, strlen(rsp_buf));
       }
    }

   return BT_SUCCESS;
}

#if BLE_SMP_SUPPORT
/*this function will be called when smp flow finished successfully
 *you need to get the device list from stack and save it to nv
 */
void Gatt_client_demo_smp_pair_success_cb(sm_le_device_info_t *pDevInfo)
{
    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_smp_pair_success_cb");
    SCI_TRACE_LOW("[BLE_client]ipa addr type: %x", pDevInfo->addr_type);
    SCI_TRACE_LOW("[BLE_client]ipa: %02x|%02x|%02x|%02x|%02x|%02x", pDevInfo->bd_address.bytes[5],
      pDevInfo->bd_address.bytes[4], pDevInfo->bd_address.bytes[3],
      pDevInfo->bd_address.bytes[2], pDevInfo->bd_address.bytes[1],
      pDevInfo->bd_address.bytes[0]);

    DB_SaveSmpKeyInfo(pDevInfo);
}

//this function will be called when smp pair failed
void Gatt_client_demo_smp_pair_failed_cb(void)
{
    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_smp_pair_failed_cb");
}

/*this function will be called when reconnect and smp encrypted(not need to generate key) successfully
 *it only called when master reconnect with slave(the key exists in device list)
 */
bt_status_t Gatt_client_demo_encrypt_change_cb(void)
{
    SCI_TRACE_LOW("[BLE_client]Gatt_client_demo_encrypt_change_cb");

    return BT_SUCCESS;
}
#endif

void Gatt_client_demo_att_error_cb(UINT8 error_code)
{
    SCI_TRACE_LOW("[BLE_client] Gatt_client_demo_att_error_cb error = 0x%x", error_code);
    switch (error_code)
    {
    case ATT_ERR_INVALID_HANDLE:
        break;

    case ATT_ERR_READ_NOT_PERMITTED:
        break;

    case ATT_ERR_WRITE_NOT_PERMITTED:
        break;

    case ATT_ERR_INVALID_PDU:
        break;

    case ATT_ERR_INSUFFICIENT_AUTHEN:
        break;

    case ATT_ERR_REQUEST_NOT_SUPPORT:
        break;

    case ATT_ERR_INVALID_OFFSET:
        break;

    case ATT_ERR_INSUFFICIENT_AUTHOR:
        break;

    case ATT_ERR_PREPARE_QUEUE_FULL:
        break;

    case ATT_ERR_ATTRIBUTE_NOT_FOUND:

        switch (gatt_client_state)
        {
        case GATT_CLIENT_DISCOVER_PRME_SER:
            SCI_TRACE_LOW("[BLE_client]error - do not find target uuid, disconnect ble link");
            ble_disconnect(ble_handle_c);
            break;
        case GATT_CLIENT_READ_INCLUDE:
            //gatt_client_state = GATT_CLIENT_READ_INCLUDE_DONE;
            Gatt_client_demo_run_statemachine(GATT_CLIENT_READ_CHARA_DEC);
            break;
        case GATT_CLIENT_READ_CHARA_DEC:
            //gatt_client_state = GATT_CLIENT_READ_CHARA_DEC_DONE;
            find_info_index = 0;
            Gatt_client_demo_run_statemachine(GATT_CLIENT_FIND_INFO);
            break;
        default:
            break;
        }

        break;

    case ATT_ERR_ATTRIBUTE_NOT_LONG:
        break;

    case ATT_ERR_INSUFFICIENT_EK_SIZE:
        break;

    case ATT_ERR_INVALID_ATTRI_VALUE_LEN:
        break;

    case ATT_ERR_UNLIKELY_ERROR:
        break;

    case ATT_ERR_INSUFFICIENT_ENCRYPTION:
        break;

    case ATT_ERR_UNSUPPORTED_GROUP_TYPE:
        break;

    case ATT_ERR_INSUFFICIENT_RESOURCES:
        break;

    default:
        if ((error_code >= 0x12) && (error_code <= 0x7f))
        {
            //Reserved for future
        }
        else //0x80<=error_code<=0xff
        {
            //Defined by higher layer application
        }
        break;
    }
}

btgatt_client_callbacks_t btgatt_client_cb_interface = {
    .connection_state_change_cb = Gatt_client_demo_connection_cb,
    .discover_service_by_uuid_cb = Gatt_client_demo_discover_service_by_uuid_cb,
    .discover_service_all_cb = Gatt_client_demo_discover_service_all_cb,
    .char_des_data = Gatt_client_demo_char_des_data_cb,
    .char_data = Gatt_client_demo_char_data_cb,
    .read_cb = Gatt_client_demo_read_cb,
    .read_blob_cb = Gatt_client_demo_read_blob_cb,
    .read_multi_cb = Gatt_client_demo_read_multi_cb,
    .recv_notification_cb = Gatt_client_demo_recv_notification_cb,
    .recv_indication_cb = Gatt_client_demo_recv_indication_cb,
    .write_cb = Gatt_client_demo_write_cb,
    .write_rsp_cb = Gatt_client_demo_write_rsp_cb,
    .scan_cb = Gatt_client_demo_scan_cb,
#if BLE_SMP_SUPPORT
    .pair_complete_cb = Gatt_client_demo_smp_pair_success_cb,
    .pair_failed_cb = Gatt_client_demo_smp_pair_failed_cb,
    .encrypt_change_cb = Gatt_client_demo_encrypt_change_cb,
#endif
    .att_error_cb = Gatt_client_demo_att_error_cb,
};

void example_ble_client_info_get(btgatt_client_callbacks_t **p_client_callbacks)
{
    *p_client_callbacks = &btgatt_client_cb_interface;
}

UINT8 Gatt_client_demo_msg_cb(UINT32 msg, void *param, UINT8 finished, UINT16 acl_handle)
{
    return BT_SUCCESS;
}
void Gatt_client_demo_init()
{
    SCI_TRACE_LOW("[BLE_client] Gatt_client_demo_init");
    chara_count = 0;
    chara_desc_count = 0;
    find_info_index = 0;
    config_index = 0;
    gatt_client_role = TRUE;
    gatt_client_demo_init = TRUE;
    gatt_client_timer = NULL;
    ble_handle_c = 0xFF;
    gatt_client_state = GATT_CLIENT_LINK_DISCONNECT;

    /*Master have folllowing four pair mode:

     *SECURE_CONNECTION_PASSKEY, KEYBOARD_DISP* //highest level
     *SECURE_CONNECTION_JUSTWORK, NOIN_NOOUT*
     *LEGACY_PASSKEY, KEYBOARD_DISP*
     *LEGACY_JUSTWORK, NOIN_NOOUT* //lowest level

     *We just need to set the pair mode to the highest level for master.
     */
    ble_smp_pair_mode_set(SECURE_CONNECTION_PASSKEY, KEYBOARD_DISP);

    ble_client_init(BT_MODULE_GATT, 0, (att_client_module_cb)Gatt_client_demo_msg_cb);
}

void Gatt_client_demo_function(void)
{
    gatt_scan_param_t scan_param;

    scan_param.filterPolicy = 0x00;
    scan_param.scanInterval = 0x30;
    scan_param.scanType = 0x01;
    scan_param.scanWindow = 0x06;

    Gatt_client_demo_init();

    //start scan
    ble_scan_disable();
    ble_scan_param_set(&scan_param, 0);
    ble_scan_enable();
}
#endif
