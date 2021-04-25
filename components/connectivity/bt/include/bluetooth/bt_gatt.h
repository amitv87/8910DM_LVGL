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

#include "bluetooth/bt_types.h"

/* module id */
enum
{
    BT_MODULE_ATT_CLIENT = 24,
    BT_MODULE_ATT_SERVER = 25,
};
#define MAX_SRV 20
#define MAX_CHAR 100

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

//error code definitions
#define ATT_ERR_INVALID_HANDLE          0x01
#define ATT_ERR_READ_NOT_PERMITTED      0x02
#define ATT_ERR_WRITE_NOT_PERMITTED     0x03
#define ATT_ERR_INVALID_PDU             0x04
#define ATT_ERR_INSUFFICIENT_AUTHEN     0x05
#define ATT_ERR_REQUEST_NOT_SUPPORT     0x06
#define ATT_ERR_INVALID_OFFSET          0x07
#define ATT_ERR_INSUFFICIENT_AUTHOR     0x08
#define ATT_ERR_PREPARE_QUEUE_FULL      0x09
#define ATT_ERR_ATTRIBUTE_NOT_FOUND     0x0A
#define ATT_ERR_ATTRIBUTE_NOT_LONG      0x0B
#define ATT_ERR_INSUFFICIENT_EK_SIZE    0x0C
#define ATT_ERR_INVALID_ATTRI_VALUE_LEN 0x0D
#define ATT_ERR_UNLIKELY_ERROR          0x0E
#define ATT_ERR_INSUFFICIENT_ENCRYPTION 0x0F
#define ATT_ERR_UNSUPPORTED_GROUP_TYPE  0x10
#define ATT_ERR_INSUFFICIENT_RESOURCES  0x11
#define ATT_ERR_APPLICATION_ERROR       0x80

//message id to upper layer apps
#define GATT_CONNECT_CNF        0x11
#define GATT_DISCONNECT_IND     0x12
//#define GATT_CLIENT_REQ_RSP     0x13
#define GATT_NOTIFICATION       0x14
#define GATT_INDICATION         0x15

#define GATT_SMP_PAIR_REQ_IND   0x1A
#define GATT_SMP_PAIR_SUCCESS_IND   0x1B
#define GATT_SMP_PAIR_FAILED_IND    0x1C
#define GATT_SMP_RECONNECT_SUCCESS_IND  0x1D
#define GATT_COMMAND_FLAG       0x40


#define SYNC_SCAN_DISABLE 0
#define SYAN_SCAN_ENBLE 1

#define LE_KEY_MAGIC    (0x20160623)

/* task message define */
typedef enum
{
    ATT_CLIENT_FIND_PRIMARY_SERVICE_RSP = 0x03001304,//(EV_BT_EXTERNAL_MSG_MASK + BT_MSG_ATT + 4)
    ATT_CLIENT_CHAR_DATA,
    ATT_CLIENT_NOTIFY_DATA,
    ATT_CLIENT_INDICATION_DATA,
    ATT_CLIENT_RESERVED1,
    ATT_CLIENT_RESERVED2,
    ATT_CLIENT_CHAR_DISCRIPTOR_DATA,
    ATT_CLIENT_READ_DATA,
    ATT_CLIENT_RESERVED3,
    ATT_CLIENT_WRITE_RSP,
    ATT_CLIENT_FIND_PRIMARY_SERVICE_END,
    ATT_CLIENT_ERROR_RSP,
    ATT_CLIENT_FIND_PRIMARY_SERVICE_UUID_RSP,
    ATT_CLIENT_READ_BLOB_RSP,
    ATT_CLIENT_RESERVED4,
    ATT_CLIENT_READ_MULTIPLE_RSP,
    // ATT_CLIENT_CHAR_DATA_UUID,
} ATT_CLIENT_MSG_TYPE_T;

typedef struct
{
    bdaddr_t address;
    UINT16 aclHandle;
    UINT16 mtu;
    UINT8 result;
} gatt_connect_cnf_t;

typedef struct
{
    UINT8 value[5];
} gatt_chara_def_short_t;

typedef struct
{
    UINT8 value[19];
} gatt_chara_value_long_t;


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

/**The parameters of gatt adv*/
typedef struct
{
    UINT8 discov_mode;               /**< The mode of discovery */
    UINT8 adv_type;                    /**< The type of adv */
    UINT8 noBrEdr;                      /**< There are no basic rate / enhanced date rate */
    UINT8 enableAdv;                  /**< Adv is supported or not */
    UINT16  advMin;                    /**< The minimum adv interval, unit is 0.625ms, within range 20ms~10.24s */
    UINT16  advMax;                   /**< The maxmum adv interval, unit is 0.625ms, within range 20ms~10.24s */
    UINT8 advFilter;                    /**< The  policy of adv filter */
    UINT8 addr_type;                  /**< Own address type */
    UINT8 adv_map;                    /**< Adv channel map ,correspond 37,38,39 channel*/
    UINT8 peer_addr[6];              /**< Peer address */
    UINT8 peer_addr_type;          /**< Peer address type */
} gatt_adv_param_t;

typedef struct
{
    UINT16 att_handle;        /**< The att handle */
    gatt_uuid_t uuid;            /**< Uuid */
    UINT8 uuid_type;           /**< The type of uuid */
    UINT16 length;              /**< The length of  */
    UINT16 acl_handle;        /**< The acl handle */
    UINT8 *data;                 /**< The data information */
} gatt_le_data_info_t;

typedef struct
{
    UINT8 data[31];
    UINT8 length;
}gatt_le_adv_data_t;

/**All status of ble adv */
typedef enum
{
    BlE_ADV_DISABLE = 0x00,    /**< The status of ble adv is disable */
    BlE_ADV_ENABLE,                /**< The status of ble adv is enable */
} ble_adv_enable_state_t;

typedef enum
{
    UPDATE_SUCCESS = 0x00,
    UPDATE_FAILE ,
    UPDATE_AGAIN,
}ble_conn_param_update_state_t;

/**The parameters of gatt scan*/
typedef struct
{
    UINT8 scanType;            /**< The type of scan,0 means passive,1 means active */
    UINT16 scanInterval;      /**< The interval of scan,unit is 0.625ms, within range 2.5ms~10.24s */
    UINT16 scanWindow;      /**< The window of scan,unit is 0.625ms, within range 2.5ms~10.24s */
    UINT8 filterPolicy;           /**< The filter policy of scan */
} gatt_scan_param_t;

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
    UINT8 own_addr_type;
    UINT8 scan_type[2];
    UINT16 scan_interval[2];
    UINT16 scan_window[2];
    UINT8 filter_policy;
    UINT8 scan_phy;
} gatt_extend_scan_param_t;

typedef struct
{
    UINT8 enable;
    UINT8 filter_duplicates;
    UINT16 duration;
    UINT16 period;
    BOOL sync_scan;
} gatt_extend_scan_enabel_t;

typedef struct
{
    UINT8 adv_handle;
    UINT16 periodic_adv_interval_min;
    UINT16 periodic_adv_interval_max;
    UINT16 periodic_adv_properties;
}gatt_periodic_adv_param_t;

typedef struct
{
    UINT8 AdvHandle;
    UINT8 operation;
    UINT8 adv_data_len;
    UINT8 *adv_data;
}gatt_periodic_adv_data_t;

typedef struct
{
    UINT8 enable;
    UINT8 adv_handle;
}gatt_periodic_adv_enable_t;

typedef struct
{
    UINT8 adv_handle;
    UINT8 operation;
    UINT8 fragment_preference;
    UINT8 scan_rsp_data_len;
    UINT8 *scan_rsp_data;
}gatt_extend_scan_rsp_data_t;

typedef struct
{
    UINT8 filter;
    UINT8 adv_sid;
    UINT8 addr_type;
    bdaddr_t addr;
    UINT16 skip;
    UINT16 timeout;
} gatt_extend_periodic_sync_t;

typedef int (*ap_module_cb)(UINT32 msg, void *param, UINT8 finished, UINT16 acl_handle);
typedef struct
{
    UINT16 module;
    UINT16 uuid;
    ap_module_cb cb;
}ap_module_info_t;

/**The parameters of gatt connect*/
typedef struct {
    UINT16 connect_handle;            /**< The handle of connect */
    UINT16 connect_interval_min;    /**< The minmum interval of connect,unit is 1.25ms,the range is 7.5ms~4s */
    UINT16 connect_interval_max;   /**< The maxmum interval of connect,unit is 1.25ms,the range is 7.5ms~4s.The minimum
                                                    interval cannot be greater than the maximum interval and can be equal */
    UINT16 slave_latency;               /**< The slave latency */
    UINT16 super_timeout;             /**< The timeout of connect,the unit is 10ms, the range is 100ms~32s */
    UINT16 min_ce_len;                  /**< The expected minimum length of the connection events */
    UINT16 max_ce_len;                 /**< The expected minimum length of the connection events.The min_ce_len
                                                  shall be less than or equal to the max_ce_len */
}gatt_connect_param_t;

typedef struct
{
    UINT16 handle;
    UINT8 tx_phy;
    UINT8 rx_phy;
}le_phy_param_t;

typedef struct
{
    UINT16 uuid;
    UINT16 handle;
    UINT32 length;
    UINT8 *value;
} gatt_chara_desc_t;

typedef struct
{
    UINT16 handle;
    UINT16 uuid;
    UINT16 valueHandle;
    UINT8 properties;
    int descNum;
    UINT8 *value;
    gatt_chara_desc_t *pDescList;
} gatt_chara_t;

typedef struct
{
    UINT16 startHandle;
    UINT16 endHandle;
    UINT16 uuid;
    UINT16 charNum;
    gatt_chara_t *pCharaList;
} gatt_prime_service_t;

typedef struct
{
    UINT16 aclHandle;
    UINT16 cid;
    UINT16 startHandle;
    UINT16 endHandle;
    UINT16 offset;
    UINT16 attValuelen;//attribute value length
    UINT16 sentLen; //value length currently sent
    UINT16 mtu;
    UINT8  uuid[16];
    UINT8  uuidFmt;
    UINT16 max_attValuelen;
    UINT8   *attValue;//512
    void *tid;
    UINT8 flags;//used by Execute Write Request
} att_req_pdu_t;

typedef struct general_rsp
{
    UINT16 length;
    UINT8 *payLoad;
} general_rsp_t;

typedef struct att_server
{
    UINT16 aclhandle;
    UINT16  cid;
    UINT16 mtu;
    att_req_pdu_t lastReqPdu;
    general_rsp_t lastRsp;
    general_rsp_t lastRspAlt;
    general_rsp_t lastNoti;
    UINT8   valueInd;
    UINT8   reqTimerId;
} att_server_t;

typedef struct
{
    bdaddr_t bd_address;
    UINT8 addr_type;
    UINT16 device_state;
    UINT8 name_length;
    UINT8 name[LE_MAX_REMOTE_NAME_LEN];
    UINT8 ltk[LINKKEY_SIZE];
    UINT8 key_type;
    UINT16 ediv;
    UINT8 rand[8];
    UINT8 enc_size;
    UINT8 device_index;
    UINT8 csrk[LINKKEY_SIZE];
    UINT8 peer_csrk[LINKKEY_SIZE];
    UINT32 sign_counter;
    UINT8 irk[LINKKEY_SIZE];
    UINT8 peer_irk[LINKKEY_SIZE];
    void *p_conn_info;
}sm_le_device_info_t;

typedef struct
{
    UINT32 magic;
    UINT32 le_device_nums;
}sm_key_info_t;

/**
* @addtogroup bt_host
* @{
* @defgroup HCI_callback HCI command callback
* @{  */
typedef struct
{
    UINT16 opcode;
    //hci_callback cb;
    void (*hci_cb)(UINT8 event_status, UINT8 *pdu);
}bt_hci_callback_t;

typedef struct
{
    UINT8 init_filter_policy;
    UINT8 own_addr_type;
    UINT8 peer_addr_type;
    bdaddr_t peer_addr;
    UINT8 init_phys;
    UINT16 scan_interval[3];
    UINT16 scan_window[3];
    UINT16 con_intv_min[3];
    UINT16 con_intv_max[3];
    UINT16 con_latency[3];
    UINT16 superv_to[3];
    UINT16 ce_len_min[3];
    UINT16 ce_len_max[3];
} le_ext_create_con_param_t;

/**
* @addtogroup bt_host
* @{
* @defgroup gatt_interface Gatt Fuc Interface
* @{  */
typedef struct
{
    char *(*get_version)(void);
    bt_status_t (*add_services) (gatt_element_t *gatt_element, UINT16 size);
    bt_status_t (*clear_service) (void);
    gatt_element_t *(*get_local_service) (UINT16 uuid);
    gatt_element_t *(*get_local_char) (UINT16 uuid);
    UINT8 *(*build_manufacturer_data)(UINT8 *p_manufacture, UINT8 len);
    UINT8 *(*build_adv_name)(UINT8 *name, UINT8 len);
    bt_status_t (*set_adv_data) (UINT8 *data, UINT32 length);
    bt_status_t (*set_adv_param) (gatt_adv_param_t *adv, UINT8 ownAddrType);
    bt_status_t (*set_scanrsp_data) (UINT8 *data, UINT8 length);
    bt_status_t (*adv_enable) (void);
    bt_status_t (*adv_disable) (void);
    bt_status_t (*send_notification) (gatt_le_data_info_t *data_info, int type);
    gatt_le_adv_data_t *(*get_adv_data)(void);
    UINT8 (*get_adv_enable_state)(void);
    bt_status_t (*set_le_random_addr)(bdaddr_t *addr);
    bt_status_t (*get_le_random_addr)(bdaddr_t *addr);
    UINT8 (*get_le_white_list_size)(void);
    char *(*get_le_white_list)(void);
    bt_status_t (*add_device_to_white_list)(bdaddr_t *addr, UINT8 addr_type);
    bt_status_t (*remove_device_from_white_list)(bdaddr_t *addr, UINT8 addr_type);
    bt_status_t (*clear_white_list)(void);
    bt_status_t (*read_white_list)(void);
    bt_status_t (*set_le_ext_adv_random_addr)(bdaddr_t *addr, UINT8 handle);
    bt_status_t (*disconnect) (UINT16 handle);
    bt_status_t (*set_ext_adv_data) (gatt_extend_adv_data_t *data);
    bt_status_t (*set_ext_adv_param) (gatt_extend_adv_param_t *param);
    bt_status_t (*ext_adv_enable) (gatt_extend_adv_enable_t *enable);
    bt_status_t (*set_ext_scan_param) (gatt_extend_scan_param_t *param);
    bt_status_t (*set_ext_scan_rsp_data) (gatt_extend_scan_rsp_data_t *data);
    bt_status_t (*ext_scan_enable) (gatt_extend_scan_enabel_t *enable);
    bt_status_t (*set_periodic_adv_param) (gatt_periodic_adv_param_t *param);
    bt_status_t (*set_periodic_adv_data) (gatt_periodic_adv_data_t *param);
    bt_status_t (*set_periodic_adv_enable) (gatt_periodic_adv_enable_t *param);
    bt_status_t (*update_connect_param) (gatt_connect_param_t *param);
    bt_status_t (*set_le_data_length)(UINT16 handle, UINT16 tx_octets, UINT16 tx_time);
    char *(*get_le_device_name) (void);
    bt_status_t (*set_le_device_name) (const char *name, UINT8 len);
    bt_status_t (*exchange_mtu_req)(UINT16 acl_handle, UINT16 mtu_size);
    gatt_connect_param_t *(*get_ble_conn_param)(void);
    bt_status_t (*set_preferred_conn_param)(gatt_connect_param_t *param);
    bt_status_t (*update_preferred_conn_param)(UINT16 handle);
    bt_status_t (*set_le_phy)(UINT16 handle, UINT8 all_phys, UINT8 tx_phys, UINT8 rx_phys, UINT16 phy_options);
    bt_status_t (*switch_le_latency)(UINT16 handle, UINT8 enable);
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
    bt_status_t (*connect) (bdaddr_t *address, UINT8 module, UINT8 addr_type);//add
    bt_status_t (*connect_ext) (bdaddr_t *address, UINT8 module, UINT8 addr_type, UINT8 own_addr_type);//add
    bt_status_t (*connect_cancel) (bdaddr_t *address);
    bt_status_t (*disconnect) (UINT16 handle);
    bt_status_t (*discover_all_primary_service) (UINT16 module, UINT16 acl_handle);
    bt_status_t (*discover_primary_service_by_uuid) (UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 acl_handle);
    bt_status_t (*discover_all_characteristic) (UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);
    bt_status_t (*discover_characteristic_by_uuid) (UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 acl_handle);
    bt_status_t (*read_char_value_by_handle) (UINT16 module, UINT16 att_handle, UINT8 is_long_att, UINT16 offset, UINT16 acl_handle);
    bt_status_t (*read_char_value_by_uuid) (UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);
    bt_status_t (*read_multiple_char_value) (UINT16 module, UINT8* handle, UINT16 length, UINT16 acl_handle);
    bt_status_t (*write_char_value) (UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle, UINT16 offset, UINT8 islong);
    bt_status_t (*write_char_value_without_rsp) (UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);
    bt_status_t (*write_char_value_signed) (UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);
    bt_status_t (*write_char_value_reliable) (UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);
    bt_status_t (*execute_write)(UINT16 module, UINT16 offset, UINT16 acl_handle);
    bt_status_t (*find_include_service) (UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);
    bt_status_t (*get_char_descriptor) (UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);
    bt_status_t (*read_char_descriptor) (UINT16 module, UINT16 att_handle, UINT8 is_long_att, UINT16 acl_handle);
    bt_status_t (*write_char_descriptor) (UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);
    bt_status_t (*gc_init) (void);
} btgatt_client_interface_t;

typedef struct
{
    bt_status_t (*slave_sec_request) (void);
    bt_status_t (*master_sec_request) (bdaddr_t address);
    bt_status_t (*set_pair_mode) (UINT8 role,UINT8 io_cap);
    bt_status_t (*set_pair_passkey) (UINT32 passkey);
    bt_status_t (*delete_pair_device)(bdaddr_t addr);
    bt_status_t (*resolving_list_enable) (UINT8 privacy_enable);
    bt_status_t (*remove_device_from_resolving_list) (bdaddr_t addr);
    void* (*smp_rpa_calc) (bdaddr_t *rpa);/*sm_le_device_info_t*/
    void* (*smp_ida_calc) (bdaddr_t *ida);
    UINT32 (*get_pair_info)(void **);
    void (*restore_pair_info)(void *);
} btgatt_smp_interface_t;

typedef struct
{
    bt_status_t (*connection_state_change_cb) (int conn_id, int connected, bdaddr_t *addr, UINT8 reason);
    bt_status_t (*encrypt_change_cb) (void);
    bt_status_t (*adv_success_cb) (void);
    bt_status_t (*adv_failed_cb) (void);
    bt_status_t (*notification_send_cb) (UINT16 length);
    void        (*pair_complete_cb) (sm_le_device_info_t *pDevInfo);
    void        (*pair_failed_cb) (void);
    void        (*num_of_complete_cb) (UINT8 current_buffer_num,UINT8 complet_buffer_num);
    bt_status_t (*read_white_list_cb)(UINT16 size);
} btgatt_server_callbacks_t;

typedef struct
{
    bt_status_t (*connection_state_change_cb) (int conn_id, int connected, bdaddr_t *addr);
    bt_status_t (*encrypt_change_cb) (void);
    bt_status_t (*discover_service_by_uuid_cb) (gatt_prime_service_t *);
    bt_status_t (*discover_service_all_cb) (gatt_prime_service_t *);
    bt_status_t (*char_des_data) (att_server_t *);
    bt_status_t (*char_data) (att_server_t *, UINT8 more_data);
    bt_status_t (*read_cb) (att_server_t *);
    bt_status_t (*read_blob_cb) (att_server_t *);
    bt_status_t (*read_multi_cb) (att_server_t *);
    bt_status_t (*recv_notification_cb) (att_server_t *);
    bt_status_t (*recv_indication_cb) (att_server_t *);
    bt_status_t (*write_cb) (att_req_pdu_t *);
    bt_status_t (*write_rsp_cb) (UINT16 *);
    bt_status_t (*scan_cb) (UINT8 *, UINT8 adv_len);
    void        (*pair_complete_cb) (sm_le_device_info_t *pDevInfo);
    void        (*pair_failed_cb) (void);
    void        (*att_error_cb) (UINT8 error_code);
    void        (*scan_timeout) (void *param);
} btgatt_client_callbacks_t;

typedef struct
{
    const btgatt_client_callbacks_t *client;
    const btgatt_server_callbacks_t *server;
    void  (*mtu_exchange_result_cb)(UINT16 handle, UINT16 mtu);
    void  (*le_data_length_result_cb)(UINT16 handle,UINT16 max_tx_octets,UINT16 max_tx_time,UINT16 max_rx_octets,UINT16 max_rx_time);
    bt_status_t (*conn_param_update_result_cb)(UINT8 status,gatt_connect_param_t *param);
    bt_status_t (*l2_conn_param_update_req_cb)(UINT16 status);
    bt_status_t (*le_phy_update_result_cb)(UINT16 handle,UINT8 status,UINT8 tx_phy,UINT8 rx_phy);
    void (*ble_hci_cmd_complete_cb)(UINT16 opcode, UINT8 status, UINT8 *pdu);
    void (*ble_hci_cmd_status_cb)(UINT16 opcode, UINT8 status, UINT8 *pdu);
} btgatt_callback_t;

typedef struct
{
    bt_status_t (*init) (const btgatt_callback_t *callbacks);
    void (*cleanup) (void);
    const btgatt_client_interface_t *client;
    const btgatt_server_interface_t *server;
    const btgatt_smp_interface_t *smp;
} btgatt_interface_t;

#endif

