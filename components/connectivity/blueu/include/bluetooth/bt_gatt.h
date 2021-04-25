/**
 * @file    bt_gatt.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_GATT_H__
#define __BT_GATT_H__

#include "bt_types.h"

#define BT_GATT_CCC_MAX  4

#define ATT_PM_READABLE                    0x0001
#define ATT_PM_WRITEABLE                   0x0002
#define ATT_PM_R_AUTHENT_REQUIRED          0x0004
#define ATT_PM_R_AUTHORIZE_REQUIRED        0x0008
#define ATT_PM_R_ENCRYPTION_REQUIRED       0x0010
#define ATT_PM_R_AUTHENT_MITM_REQUERED     0x0020
#define ATT_PM_W_AUTHENT_REQUIRED          0x0040
#define ATT_PM_W_AUTHORIZE_REQUIRED        0x0080
#define ATT_PM_W_ENCRYPTION_REQUIRED       0x0100
#define ATT_PM_W_AUTHENT_MITM_REQUERED     0x0200
#define ATT_PM_BR_ACCESS_ONLY              0x0400

#define ATT_CHARA_PROP_BROADCAST           0x01
#define ATT_CHARA_PROP_READ         0x02
#define ATT_CHARA_PROP_WWP          0x04    // WWP short for "write without response"
#define ATT_CHARA_PROP_WRITE               0x08
#define ATT_CHARA_PROP_NOTIFY              0x10
#define ATT_CHARA_PROP_INDICATE     0x20
#define ATT_CHARA_PROP_ASW          0x40    // ASW short for "Authenticated signed write"
#define ATT_CHARA_PROP_EX_PROP      0x80

//GATT Attribute Types
#define ATT_UUID_PRIMARY            0x2800
#define ATT_UUID_SECONDARY      0x2801
#define ATT_UUID_INCLUDE            0x2802
#define ATT_UUID_CHAR               0x2803

#define ATT_FMT_INITAL_LEN(x)   ((x<<22)&0xff800000)
#define ATT_FMT_SHORT_UUID      0x00000001
#define ATT_FMT_FIXED_LENGTH    0x00000002
#define ATT_FMT_WRITE_NOTIFY    0x00000004
#define ATT_FMT_GROUPED         0x00000008
#define ATT_FMT_CONFIRM_NOTITY  0x00000010
#define ATT_FMT_APPEND_WRITE     0x00000020

//GATT Characteristic Descriptors
#define ATT_UUID_CHAR_EXT           0x2900
#define ATT_UUID_CHAR_USER      0x2901
#define ATT_UUID_CLIENT         0x2902
#define ATT_UUID_SERVER         0x2903
#define ATT_UUID_CHAR_FORMAT        0x2904
#define ATT_UUID_CHAR_AGGREGATE 0x2905
#define ATT_UUID_EXTERNAL_REF       0x2907
#define ATT_UUID_REPORT_RE      0x2908

#define GATT_NOTIFICATION       0x14
#define BT_GATT_CCC_NOTIFY      0x0001

typedef struct
{
    UINT8 value[5];
} gatt_chara_def_short_t;


typedef union
{
    UINT16  uuid_s;
    UINT8  *uuid_l;
} gatt_uuid_t;

typedef UINT8 (*gatt_value_change_cb_t)(void *param);
typedef UINT8 (*gatt_value_read_cb_t)(void *param);


typedef struct
{
    UINT16 length;
    UINT16 permisssion;
    gatt_uuid_t uuid;
    UINT32  fmt;
    void *attValue;
    gatt_value_change_cb_t  cb;
    gatt_value_read_cb_t  read_cb;
} gatt_element_t;

typedef struct
{
    UINT16 handle;
    UINT16 valueLen;
    UINT16 proxy_addr;
    gatt_element_t *attribute;
} gatt_srv_write_notify_t;


typedef struct
{
    UINT8 discov_mode;
    UINT8 adv_type;
    UINT8 noBrEdr;
    UINT8 enableAdv;
    UINT16  advMin;
    UINT16  advMax;
    UINT8 advFilter;
    UINT8 addr_type;
    UINT8 adv_map;
    UINT8 peer_addr[6];
    UINT8 peer_addr_type;
} gatt_adv_param_t;

typedef struct
{
    UINT16 att_handle;
    gatt_uuid_t uuid;
    UINT8 uuid_type;
    UINT16 length;
    UINT16 acl_handle;
    UINT8 *data;
} gatt_le_data_info_t;

typedef struct
{
    UINT8 data[31];
    UINT8 length;
}gatt_le_adv_data_t;

typedef enum
{
    BlE_ADV_DISABLE = 0x00,
    BlE_ADV_ENABLE,
    BlE_ADV_NOUSE,
} ble_adv_enable_state_t;

typedef struct
{
    UINT8 handle;
    UINT16 properties;
    UINT8 adv_interval_min[3];
    UINT8 adv_interval_max[3];
    UINT8 adv_channel_map;
    UINT8 own_addr_type;
    UINT8 peer_addr_type;
    bdaddr_t peer_addr;
    UINT8 adv_filter_policy;
    UINT8 adv_tx_power;
    UINT8 primary_adv_phy;
    UINT8 adv_max_skip;
    UINT8 secondary_adv_phy;
    UINT8 adv_sid;
    UINT8 scan_req_notification_enable;
}gatt_extend_adv_param_t;

typedef struct
{
    UINT8 adv_handle;
    UINT8 operation;
    UINT8 fragment_preference;
    UINT8 adv_data_len;
    UINT8 *adv_data;
}gatt_extend_adv_data_t;

typedef struct
{
    UINT8 enable;
    UINT8 number_of_sets;
    UINT8 *adv_handle;
    UINT16 *duration;
    UINT8 *max_event;
}gatt_extend_adv_enable_t;

typedef struct
{
    UINT8 scanType;
    UINT16 scanInterval;
    UINT16 scanWindow;
    UINT8 filterPolicy;
} gatt_scan_param_t;

typedef struct{
    UINT8 own_addr_type;
    UINT8 scan_type;
    UINT16 scan_interval;
    UINT16 scan_window;
    UINT8 filter_policy;
    UINT8 scan_phy;
}gatt_extend_scan_param_t;

typedef struct{
    UINT8 enable;
    UINT8 filter_duplicates;
    UINT16 duration;
    UINT16 period;
}gatt_extend_scan_enabel_t;

typedef int (*ap_module_cb)(UINT32 msg, void *param, UINT8 finished, UINT16 acl_handle);

typedef struct
{
    UINT16 module;
    UINT16 uuid;
    ap_module_cb cb;
}ap_module_info_t;

typedef struct
{
    bt_status_t (*add_services) (const gatt_element_t *gatt_element, UINT32 size);
    bt_status_t (*set_adv_data) (UINT8 *data, UINT32 length);
    bt_status_t (*set_scanrsp_data) (UINT8 *data, UINT8 length);
    bt_status_t (*adv_enable) (gatt_adv_param_t *param);
    bt_status_t (*adv_disable) (void);
    bt_status_t (*send_notification) (gatt_le_data_info_t *data_info, int type);
    gatt_le_adv_data_t * (*get_adv_data)(void);
    UINT8 (*get_adv_enable_state)(void);
    bt_status_t (*set_ext_adv_data) (gatt_extend_adv_data_t *data);
    bt_status_t (*set_ext_adv_param) (gatt_extend_adv_param_t *param);
    bt_status_t (*ext_adv_enable) (gatt_extend_adv_enable_t *enable);
    bt_status_t (*set_ext_scan_param) (gatt_extend_scan_param_t *param);
    bt_status_t (*ext_scan_enable) (gatt_extend_scan_enabel_t *enable);
} btgatt_server_interface_t;

typedef struct
{
    bt_status_t (*ble_client_init) (UINT16 module, UINT16 uuid, ap_module_cb func);
    bt_status_t (*set_scan_param) (gatt_scan_param_t *param, UINT8 own_addr_type);
    bt_status_t (*start_scan) (void);
    bt_status_t (*stop_scan) (void);
    bt_status_t (*filter_manufacturer) (UINT8 *data);
    bt_status_t (*filter_name) (UINT8 *name);
    bt_status_t (*filter_uuid) (UINT8 *uuid);
    bt_status_t (*client_connect) (bdaddr_t *address, UINT8 module, UINT8 addr_type);//add 
    bt_status_t (*connect_cancel) (bdaddr_t *address);
    bt_status_t (*disconnect) (UINT16 handle);
	
    bt_status_t (*discover_all_primary_service) (UINT16 module, UINT16 acl_handle);
    bt_status_t (*discover_primary_service_by_uuid) (UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 acl_handle);
    bt_status_t (*discover_all_characteristic) (UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);
    bt_status_t (*read_char_value_by_handle) (UINT16 module, UINT16 att_handle, UINT8 is_long_att, UINT16 acl_handle);
    bt_status_t (*read_char_value_by_uuid) (UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);
    bt_status_t (*read_multiple_char_value) (UINT16 module, UINT8* handle, UINT8 length, UINT16 acl_handle);
    bt_status_t (*write_char_value) (UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);
    bt_status_t (*write_char_value_without_rsp) (UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);
    bt_status_t (*write_char_value_signed) (UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);
    bt_status_t (*write_char_value_reliable) (UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);
    bt_status_t (*find_include_service) (UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);
    bt_status_t (*get_char_descriptor) (UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);
    bt_status_t (*read_char_descriptor) (UINT16 module, UINT16 att_handle, UINT8 is_long_att, UINT16 acl_handle);
    bt_status_t (*write_char_descriptor) (UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);
    bt_status_t (*gc_init) (void);
} btgatt_client_interface_t;

typedef struct
{
    void (*connection_callback) (int conn_id, int connected, bdaddr_t *addr);
} btgatt_server_callbacks_t;

typedef struct
{
    bt_status_t (*connection_state_change_cb) (int conn_id, int connected, bdaddr_t *addr);
    bt_status_t (*discover_service_by_uuid_cb) (void *);
    bt_status_t (*discover_service_all_cb) (void *);
    bt_status_t (*char_des_data) (void *);
    bt_status_t (*char_data) (void *, UINT8 more_data);
    bt_status_t (*read_cb) (void *);
    bt_status_t (*read_blob_cb) (void *);
    bt_status_t (*read_multi_cb) (void *);
    bt_status_t (*recv_notification_cb) (void *);
    bt_status_t (*recv_indication_cb) (void *);
    bt_status_t (*write_cb) (void *);
    bt_status_t (*write_rsp_cb) (void *);
    bt_status_t (*scan_cb) (void *,UINT8 adv_len);
    void 		(*smp_pair_success_cb) (void);
    void 		(*smp_pair_failed_cb) (void);
    void		(*att_error_cb) (UINT8 error_code);
} btgatt_client_callbacks_t;

typedef struct
{
    const btgatt_client_callbacks_t *client;
    const btgatt_server_callbacks_t *server;
    void  (*mtu_exchange_result_cb)(UINT16 handle, UINT16 mtu);
} btgatt_callback_t;

typedef struct
{
    bt_status_t (*init) (const btgatt_callback_t *callbacks);
    void (*cleanup) (void);
    const btgatt_client_interface_t *client;
    const btgatt_server_interface_t *server;
} btgatt_interface_t;

#endif

