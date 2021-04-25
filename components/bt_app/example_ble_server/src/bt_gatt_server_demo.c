/**
 * @file     bt_gatt_server_demo.c
 * @brief    bt gatt server demo
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2020-09-30
 * @license  Copyright (C) 2020 Unisoc Communications Inc
 */
//#include "osi.h"  // build err
#include <stdio.h>
#include "vfs.h"
#include <stdint.h>
#include "osi_log.h"
#include "bluetooth/bt_driver.h"
#include "bluetooth/bt_gatt.h"
#include "bluetooth/bluetooth.h"
#include "sci_types.h"
#include "bt_abs.h"
#include "bt_app.h"
#include "bt_gatt_server_demo.h"
#include "ddb.h"


#define BT_8910_TP_UUID 0x18FE
#define BT_8910_TP_UUID_CHAR 0xFFFE
#define BT_8910_FEEDBACK_CHAR 0x8910
#define ATT_UUID_BAS 0x180F //Battery Service
#define ATT_UUID_BATTERY_LEVEL 0x2A19
#define UNISOC_CID 0x073F

#define GATT_CONNECT_CNF 0x11
#define GATT_DISCONNECT_IND 0x12
#define GATT_WRITE_NOTIFY 0x17
#define GATT_INDICATION_CONF 0x18
#define GATT_NOTIFICATION_CNF 0x19

typedef struct
{
    UINT16 configurationBits;
    UINT16 aclHandle;
} gatt_chara_ccb_t;

UINT8 bas_timer;
//static UINT16 ble_handle;
//battery service
static UINT8 ble_battery_uuid[2] = {ATT_UUID_BAS & 0xff, ATT_UUID_BAS >> 8};
//static UINT8 bt_8910_tp_data_uuid[2] = {BT_8910_TP_UUID & 0xff, BT_8910_TP_UUID >> 8};
static UINT8 bt_8910_tp_data_uuid[16]= {0xf5,0x89,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,BT_8910_TP_UUID&0xff, BT_8910_TP_UUID>>8,0x00,0x00};
#if __BT_UNUSED_WARNING_CLEAR__
static UINT8 bt_8910_feedback_uuid[2] = {BT_8910_FEEDBACK_CHAR & 0xff, BT_8910_FEEDBACK_CHAR >> 8};
#endif
static gatt_chara_def_short_t bt_8910_bas_char = {{ATT_CHARA_PROP_READ | ATT_CHARA_PROP_NOTIFY | ATT_CHARA_PROP_INDICATE, 0, 0, ATT_UUID_BATTERY_LEVEL & 0xff, ATT_UUID_BATTERY_LEVEL >> 8}};
//static gatt_chara_def_short_t bt_8910_tp_data_char = {{ATT_CHARA_PROP_READ | ATT_CHARA_PROP_WRITE, 0, 0, BT_8910_TP_UUID_CHAR & 0xff, BT_8910_TP_UUID_CHAR >> 8}};
/*bt_8910_tp_data_char value:value(short:2B/long:16B)*/
static UINT8 bt_8910_tp_data_char_value_uuid[16] = {0xf5,0x89,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,BT_8910_TP_UUID_CHAR&0xff, BT_8910_TP_UUID_CHAR>>8,0x11,0x11};
/*char value uuid format:properties(1B);handle(2B);value(short:2B/long:16B)*/
static gatt_chara_value_long_t bt_8910_tp_data_char= {{ATT_CHARA_PROP_READ | ATT_CHARA_PROP_WRITE, 0, 0,0xf5,0x89,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,BT_8910_TP_UUID_CHAR&0xff, BT_8910_TP_UUID_CHAR>>8,0x11,0x11}};
static gatt_chara_def_short_t bt_8910_feedback_char = {{ATT_CHARA_PROP_READ | ATT_CHARA_PROP_NOTIFY | ATT_CHARA_PROP_INDICATE, 0, 0, BT_8910_FEEDBACK_CHAR & 0xff, BT_8910_FEEDBACK_CHAR >> 8}};

static UINT8 bt_8910_bas_value = 20;
static UINT8 bt_8910_tp_data_value[244] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x21, 0x22, 0x23, 0x24, 0x25};
static UINT8 bt_8910_feedback_value[244] = {0x31, 0x32, 0x33, 0x34, 0x35, 0};

static gatt_chara_ccb_t bt_8910_bas_client_desc = {0x0000, 0x00};      //client characteristic configuration descriptor, bit0: notification enable, bit1:indications enable, other:RFU
static gatt_chara_ccb_t bt_8910_feedback_client_desc = {0x0000, 0x00}; //client characteristic configuration descriptor, bit0: notification enable, bit1:indications enable, other:RFU
#if 0
UINT8 peer_ida_addr[6] = {0};
UINT8 peer_ida_addr_type = 0;
#endif
extern uint8 gatt_client_role;
UINT8 bt_8910_bas_client_cb(void *param);
UINT8 bt_8910_feedback_client_cb(void *param);

UINT8 bt_8910_bas_write_cb(void *param);
UINT8 bt_8910_tp_data_write_cb(void *param);
UINT8 bt_8910_feedback_write_cb(void *param);

gatt_le_data_info_t notify_data;

static gatt_element_t bt_8910_ble_service[] =
    {
        //primary service declaration
        {
            .length = sizeof(ble_battery_uuid),
            .permisssion = ATT_PM_READABLE,
            .uuid.uuid_s = ATT_UUID_PRIMARY,
            .fmt = ATT_FMT_SHORT_UUID | ATT_FMT_GROUPED,
            .attValue = (void *)ble_battery_uuid,
            .cb = NULL,
            .read_cb = NULL},
        //battery char
        {
            .length = sizeof(bt_8910_bas_char),
            .permisssion = ATT_PM_READABLE,
            .uuid.uuid_s = ATT_UUID_CHAR,
            .fmt = ATT_FMT_SHORT_UUID | ATT_FMT_FIXED_LENGTH,
            .attValue = (void *)&bt_8910_bas_char,
            .cb = NULL,
            .read_cb = NULL},

        {.length = sizeof(bt_8910_bas_value),
         .permisssion = ATT_PM_READABLE,
         .uuid.uuid_s = ATT_UUID_BATTERY_LEVEL,
         .fmt = ATT_FMT_WRITE_NOTIFY | ATT_FMT_SHORT_UUID | ATT_FMT_FIXED_LENGTH | ATT_FMT_CONFIRM_NOTITY,
         .attValue = (void *)&bt_8910_bas_value,
         .cb = bt_8910_bas_write_cb,
         .read_cb = NULL},

        {.length = sizeof(bt_8910_bas_client_desc),
         .permisssion = ATT_PM_READABLE | ATT_PM_WRITEABLE,
         .uuid.uuid_s = ATT_UUID_CLIENT,
         .fmt = ATT_FMT_SHORT_UUID | ATT_FMT_WRITE_NOTIFY | ATT_FMT_FIXED_LENGTH,
         .attValue = (void *)&bt_8910_bas_client_desc,
         .cb = bt_8910_bas_client_cb,
         .read_cb = NULL},
        //throughput data char
        {
            .length = sizeof(bt_8910_tp_data_uuid),
            .permisssion = ATT_PM_READABLE,
            .uuid.uuid_s = ATT_UUID_PRIMARY,
            //.fmt = ATT_FMT_SHORT_UUID | ATT_FMT_GROUPED,
            .fmt = ATT_FMT_GROUPED,
            .attValue = (void *)bt_8910_tp_data_uuid,
            .cb = NULL,
            .read_cb = NULL},

        /*char uuid start:ATT_UUID_CHAR(0x2803)*/
        {.length = sizeof(bt_8910_tp_data_char),
         .permisssion = ATT_PM_READABLE,
         .uuid.uuid_s = ATT_UUID_CHAR,
         .fmt = ATT_FMT_SHORT_UUID | ATT_FMT_FIXED_LENGTH,
         .attValue = (void *)&bt_8910_tp_data_char,
         .cb = NULL,
         .read_cb = NULL},

        /*true char uuid content:shout=0,use long uuid*/
        {.length = sizeof(bt_8910_tp_data_value),
         .permisssion = ATT_PM_READABLE | ATT_PM_WRITEABLE, //|ATT_PM_R_ENCRYPTION_REQUIRED,
         //.uuid.uuid_s = BT_8910_TP_UUID_CHAR,
         .uuid.uuid_l = bt_8910_tp_data_char_value_uuid,
         .fmt = ATT_FMT_WRITE_NOTIFY | ATT_FMT_FIXED_LENGTH | ATT_FMT_CONFIRM_NOTITY,//| ATT_FMT_SHORT_UUID
         .attValue = (void *)&bt_8910_tp_data_value,
         .cb = bt_8910_tp_data_write_cb,
         .read_cb = NULL},
        //feedback char
        {
            .length = sizeof(bt_8910_feedback_char),
            .permisssion = ATT_PM_READABLE,
            .uuid.uuid_s = ATT_UUID_CHAR,
            .fmt = ATT_FMT_SHORT_UUID | ATT_FMT_FIXED_LENGTH,
            .attValue = (void *)&bt_8910_feedback_char,
            .cb = NULL,
            .read_cb = NULL},

        {.length = sizeof(bt_8910_feedback_value),
         .permisssion = ATT_PM_READABLE, //| ATT_PM_R_ENCRYPTION_REQUIRED,
         .uuid.uuid_s = BT_8910_FEEDBACK_CHAR,
         .fmt = ATT_FMT_SHORT_UUID | ATT_FMT_FIXED_LENGTH | ATT_FMT_CONFIRM_NOTITY,
         .attValue = (void *)&bt_8910_feedback_value,
         .cb = bt_8910_feedback_write_cb,
         .read_cb = NULL},

        {.length = sizeof(bt_8910_feedback_client_desc),
         .permisssion = ATT_PM_READABLE | ATT_PM_WRITEABLE,
         .uuid.uuid_s = ATT_UUID_CLIENT,
         .fmt = ATT_FMT_SHORT_UUID | ATT_FMT_WRITE_NOTIFY | ATT_FMT_FIXED_LENGTH,
         .attValue = (void *)&bt_8910_feedback_client_desc,
         .cb = bt_8910_feedback_client_cb,
         .read_cb = NULL}};

#if BLE_SMP_SUPPORT
osiTimer_t *ble_server_timer = NULL;
//the slave send security request to start smp flow
void app_smp_sec_req(void *pArgs)
{
    OSI_LOGI(0, "[BLE_server]timer app_smp_sec_req");
    //server have following four pair modes, test one by one
    ble_smp_pair_mode_set(LEGACY_JUSTWORK, NOIN_NOOUT);
    //ble_smp_pair_mode_set(LEGACY_PASSKEY, DISPLAY_ONLY);
    //ble_smp_pair_mode_set(SECURE_CONNECTION_JUSTWORK, NOIN_NOOUT);
    //ble_smp_pair_mode_set(SECURE_CONNECTION_PASSKEY, DISPLAY_ONLY);
    ble_smp_slave_sec_request();
}
#endif

bt_status_t app_gatt_connection_callback(int conn_id, int connected, bdaddr_t *addr, UINT8 reason)
{
    OSI_LOGI(0, "[BLE_server]app_gatt_connection_callback conn_id:%d connected:%d", conn_id, connected);

    notify_data.acl_handle = conn_id;

    if (TRUE == connected)
    {
        //set_bt_protocol_state(BT_GATT_POS, BT_PROTOCOL_CONNECTED);
        if (gatt_client_role == FALSE)
        {
#if BLE_SMP_SUPPORT
            ble_server_timer = osiTimerCreate(NULL, app_smp_sec_req, NULL);
            //start smp flow after four seconds
            osiTimerStartRelaxed(ble_server_timer, 40 * 100, OSI_WAIT_FOREVER);
#endif
        }
    }
    else
    {
        if (gatt_client_role == FALSE)
        {
        /*it's only for encrypted adv(ownaddrtype is 2 or 3, peer_addr_type and peer_addr are valid value).
         *if you want to use encrypted adv, you should use unencrypted adv(ownaddrtype is 0 or 1) first
         *in prder to generate the irk/ltk.
         *if ownaddrtype is 0 in unencrypted adv, then ownaddrtype is 2 in encrypted adv.
         *if ownaddrtype is 1 in unencrypted adv, then ownaddrtype is 3 in encrypted adv.
         *peer_addr_type and peer_addr are coming from unencrypted adv's pair_complete_cb.
         *Once you use encrypted adv(after using unencrypted adv), only the peer device in unencrypted adv
         *can connect to the server.*/
        #if 0
            gatt_adv_param_t adv;
            //adv para
            adv.advMin = 0x80;
            adv.advMax = 0xa0;
            adv.discov_mode = 2;  //generic mode
            adv.adv_type = 0;     //adv type is ADV_IND
            adv.noBrEdr = 1;
            adv.enableAdv = 1;
            adv.adv_map = 0x07;   //channel 37/38/39
            adv.advFilter = 0;
            adv.peer_addr_type = peer_ida_addr_type;
            memcpy(adv.peer_addr, peer_ida_addr, 6);
            ble_adv_param_set(&adv, 3);//0:public 1:random
        #endif
            //ble_adv_enable();
        }
    }

    return BT_SUCCESS;
}

bt_status_t app_gatt_notification_send_cb(UINT16 length)
{
    OSI_LOGI(0, "[BLE_server]app_gatt_notification_send_cb length:%d", length);

    return BT_SUCCESS;
}

void app_gatt_packets_complete_cb(UINT8 current_buffer_num, UINT8 complete_buffer_num)
{
    //printf("\n sum_num: %d,complete_num:%d\n", current_buffer_num,complete_buffer_num);
}

#if BLE_SMP_SUPPORT
/*this function will be called when smp flow finished successfully
 *you need to get the device list from stack and save it to nv
 */
void gatt_server_demo_smp_pair_success_cb(sm_le_device_info_t *pDevInfo)
{
    SCI_TRACE_LOW("[BLE_server]gatt_server_demo_smp_pair_success_cb");
    SCI_TRACE_LOW("[BLE_server]service ipa addr type: %x", pDevInfo->addr_type);
    SCI_TRACE_LOW("[BLE_server]service ipa: %02x|%02x|%02x|%02x|%02x|%02x", pDevInfo->bd_address.bytes[5],
      pDevInfo->bd_address.bytes[4], pDevInfo->bd_address.bytes[3],
      pDevInfo->bd_address.bytes[2], pDevInfo->bd_address.bytes[1],
      pDevInfo->bd_address.bytes[0]);
#if 0
    memcpy(peer_ida_addr, pDevInfo->bd_address.bytes, 6);
    peer_ida_addr_type = pDevInfo->addr_type;
#endif

    DB_SaveSmpKeyInfo(pDevInfo);
}

/*this function will be called when reconnect and smp encrypted(not need to generate key) successfully
 *it only called when master reconnect with slave(the key exists in device list)
 */
bt_status_t gatt_server_demo_encrypt_change_cb(void)
{
    SCI_TRACE_LOW("[BLE_server]gatt_server_demo_encrypt_change_cb");
    osiTimerDelete(ble_server_timer);
    ble_server_timer = NULL;
    return BT_SUCCESS;
}
#endif

btgatt_server_callbacks_t btgatt_server_callbacks = {
#if BLE_SMP_SUPPORT
    .encrypt_change_cb = gatt_server_demo_encrypt_change_cb,
    .pair_complete_cb = gatt_server_demo_smp_pair_success_cb,
#endif
    .connection_state_change_cb = app_gatt_connection_callback,
    .notification_send_cb = app_gatt_notification_send_cb,
    .num_of_complete_cb = app_gatt_packets_complete_cb,
};

void example_ble_server_info_get(btgatt_server_callbacks_t **p_server_callbacks)
{
    *p_server_callbacks = &btgatt_server_callbacks;
}

//this function is used to start the ble server(set adv para and adv data, add services, enable adv),
//you should call it in app.
void Gatt_server_demo_function(void)
{
    gatt_adv_param_t adv;
    int i = 0;
    char *name = "UNISOC-8910";
    UINT8 Manufacture_Data[8] = {UNISOC_CID & 0xff, UNISOC_CID >> 8, 0}; //cid(2byte)+device_addr(6byte)
    UINT8 Server_uuid[2] = {ATT_UUID_BAS & 0xff, ATT_UUID_BAS >> 8};
    UINT8 peer_addr[6] = {0};
    UINT8 adv_data[31] = {0x02,0x01,0x06};
    UINT8 scanrsp_data[31] = {0x02,0x01,0x06};
    UINT8 adv_len = 3;
    UINT8 scanrsp_len = 0;

    //adv para
    adv.advMin = 0x80;
    adv.advMax = 0xa0;
    adv.discov_mode = 2;  //generic mode
    adv.adv_type = 0;     //adv type is ADV_IND
    adv.noBrEdr = 1;
    adv.enableAdv = 1;
    adv.adv_map = 0x07;   //channel 37/38/39
    adv.advFilter = 0;
    adv.peer_addr_type = 0;
    memcpy(adv.peer_addr, peer_addr, 6);

    OSI_LOGI(0, "[BLE_server]Gatt_server_demo_function enter");

    //set the adv data
    adv_data[adv_len++] = strlen(name) + 1;
    adv_data[adv_len++] = 0x09; //GAP_ADTYPE_LOCAL_NAME_COMPLETE;
    memcpy(adv_data + adv_len, name, strlen(name));
    adv_len += strlen(name);

    adv_data[adv_len++] = sizeof(Manufacture_Data) + 1;
    adv_data[adv_len++] = 0xff;
    memcpy(adv_data + adv_len, Manufacture_Data, sizeof(Manufacture_Data));
    adv_len += sizeof(Manufacture_Data);

    adv_data[adv_len++] = sizeof(Server_uuid) + 1;
    adv_data[adv_len++] = 0x02;
    memcpy(adv_data + adv_len, Server_uuid, sizeof(Server_uuid));
    adv_len += sizeof(Server_uuid);

    for (i = 0; i < adv_len; i++)
    {
        OSI_LOGI(0, "[BLE_server]adv_data[%d] = 0x%x", i, adv_data[i]);
    }

    ble_adv_data_set (adv_data, adv_len);

    scanrsp_data[scanrsp_len++] = strlen(name) + 1;
    scanrsp_data[scanrsp_len++] = 0x09; //GAP_ADTYPE_LOCAL_NAME_COMPLETE;
    memcpy(scanrsp_data + scanrsp_len, name, strlen(name));
    scanrsp_len += strlen(name);

    ble_scan_rsp_data_set (scanrsp_data, scanrsp_len);

    ble_add_service(bt_8910_ble_service, sizeof(bt_8910_ble_service) / sizeof(gatt_element_t));
    ble_adv_param_set(&adv, 1);//0:public 1:random
    ble_adv_enable();
}

UINT8 bt_8910_bas_write_cb(void *param)
{
    OSI_LOGI(0, "[BLE_server]bt_8910_bas_write_cb bt_8910_bas_value =%x ", bt_8910_bas_value);
    return 0;
}

UINT8 bt_8910_tp_data_write_cb(void *param)
{
    UINT8 datalen = 0;
    gatt_srv_write_notify_t *pAttr = (gatt_srv_write_notify_t *)param;
    datalen = strlen((const char *)bt_8910_tp_data_value);

    OSI_LOGI(0, "[BLE_server]bt_8910_tp_data_write_cb len=%d,tplen=%d", pAttr->valueLen, datalen);

    //for(i = 0; i < sizeof(bt_8910_tp_data_value); i++)
    {
        //OSI_LOGI(0,"bt_8910_tp_data_write_cb[%d]=%x",i,bt_8910_tp_data_value[i]);
    }

    memset(bt_8910_tp_data_value, 0, sizeof(bt_8910_tp_data_value));
    //app_bt_send_notify(ble_handle, ((bt_8910_feedback_char.value[2]<<8)|bt_8910_feedback_char.value[1]),
    //sizeof(bt_8910_feedback_value), &bt_8910_feedback_value[0]);

    return 0;
}

/**
* @brief  Send indication callback confirm
* @param [in] param if the parameter is Null,server send indication will callback confirm
* @param [in] param if the parameter is not Null,the data will be analyzed when client sends data
* @return The status of bt_8910_feedback_write_cb
*/
UINT8 bt_8910_feedback_write_cb(void *param)
{
    if (param == NULL)
    {
        OSI_LOGI(0, "[BLE_server]bt_8910_indication_confirm");
    }
    else
    {
        UINT8 datalen = 0;
        gatt_srv_write_notify_t *pAttr = (gatt_srv_write_notify_t *)param;
        datalen = strlen((const char *)bt_8910_feedback_value);
        OSI_LOGI(0, "[BLE_server]bt_8910_feedback_write_cb len=%d,tplen=%d", pAttr->valueLen, datalen);

        memset(bt_8910_feedback_value, 0, sizeof(bt_8910_feedback_value));
    }
    return 0;
}

UINT8 bt_8910_feedback_client_cb(void *param)
{
    UINT8 value[2] = {0, 0};
    gatt_srv_write_notify_t *msg = (gatt_srv_write_notify_t *)param;
    OSI_LOGI(0, "[BLE_server]bt_8910_feedback_client_cb, msg->valueLen=%x,configurationBits=%x",
             msg->valueLen, bt_8910_feedback_client_desc.configurationBits);

    if (msg == NULL || msg->attribute == NULL || msg->attribute->attValue == NULL)
    {
        return 0;
    }

    if (msg->valueLen > 2)
    {
        msg->valueLen = 2;
    }
    if (msg->valueLen)
    {
        memcpy(value, msg->attribute->attValue, msg->valueLen);
    }

    if (bt_8910_feedback_client_desc.configurationBits & 0x01)
    {
        //app_bt_send_notify(ble_handle, ((bt_8910_tp_data_char.value[2]<<8)|bt_8910_tp_data_char.value[1]),
        //                             sizeof(bt_8910_feedback_value), &bt_8910_feedback_value[0]);
    }

    return 0;
}

UINT8 bt_8910_bas_client_cb(void *param)
{
    UINT8 value[2] = {0, 0};
    gatt_srv_write_notify_t *msg = (gatt_srv_write_notify_t *)param;

    OSI_LOGI(0, "[BLE_server]bt_8910_bas_client_cb msg->valueLen=%d,msg->handle=%x",
             msg->valueLen, msg->handle);

    if (msg == NULL || msg->attribute == NULL || msg->attribute->attValue == NULL)
    {
        return 0;
    }

    if (msg->valueLen > 2)
    {
        msg->valueLen = 2;
    }
    if (msg->valueLen)
    {
        memcpy(value, msg->attribute->attValue, msg->valueLen);
    }
    OSI_LOGI(0, "[BLE_server]btbas_modify_client_desc_cbs value=%x,%x,bt_8910_bas_client_desc=%x",
             value[0], value[1], bt_8910_bas_client_desc.configurationBits);

    if (bt_8910_bas_client_desc.configurationBits & 0x01)
    {
        //app_bt_send_notify(ble_handle, bt_8910_bas_char.value[2]<<8|bt_8910_bas_char.value[1], 1, &bt_8910_bas_value);
        bt_8910_bas_value += 1;
    }
    if (bt_8910_bas_client_desc.configurationBits & 0x02)
    {
        //app_bt_send_indication(ble_handle, bt_8910_bas_char.value[2]<<8|bt_8910_bas_char.value[1], 1, &bt_8910_bas_value);
        bt_8910_bas_value += 2;
    }

    return 0;
}

UINT32 bt_8910_feedback_change(const UINT8 *value, const UINT8 len)
{
    memset(bt_8910_feedback_value, 0, 240);
    memcpy(bt_8910_feedback_value, value, len);
    return 0;
}

UINT32 bt_8910_feedback_get(UINT8 *value, const UINT8 len)
{
    memcpy(value, bt_8910_feedback_value, len);
    return 0;
}

//this function is used to send notify.
void bt_8910_ble_Send_Notify(uint16 datalen, uint8 *data)
{
    gatt_le_data_info_t le_data_info;

    OSI_LOGI(0, "[BLE_server]bt_8910_ble_Send_Notify enter");

    le_data_info.acl_handle = notify_data.acl_handle;
    le_data_info.att_handle = ((bt_8910_feedback_char.value[2] << 8) | bt_8910_feedback_char.value[1]);
    le_data_info.data = data;
    le_data_info.length = datalen;
    le_data_info.uuid.uuid_s = ATT_UUID_CHAR;
    le_data_info.uuid_type = 0;

    ble_send_notification(&le_data_info);
}

//this function is used to send Indication.
void bt_8910_ble_Send_Indication(uint16 datalen, uint8 *data)
{
    gatt_le_data_info_t le_data_info;

    OSI_LOGI(0, "[BLE_server]bt_8910_ble_Send_Indication enter");

    le_data_info.acl_handle = notify_data.acl_handle;
    le_data_info.att_handle = ((bt_8910_feedback_char.value[2] << 8) | bt_8910_feedback_char.value[1]);
    le_data_info.data = data;
    le_data_info.length = datalen;
    le_data_info.uuid.uuid_s = ATT_UUID_CHAR;
    le_data_info.uuid_type = 0;

    ble_send_indication(&le_data_info);
}
