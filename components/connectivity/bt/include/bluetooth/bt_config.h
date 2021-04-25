/**
 * @file    bt_config.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_CONFIG_H__
#define __BT_CONFIG_H__

#include "bluetooth/bt_types.h"


#define EFFUSE_VALID_FLAG    0xe2

#define MAX_BT_DEVICE_LIST      10  ///< max number of device
#define MAX_ACL_LINK			4

//#define TWS_TEST
#ifdef TWS_TEST
#define IS_RIGHT
#define BD_RIGHT  {0x11, 0x22, 0x33, 0xda, 0x45, 0x40}
#define BD_LEFT   {0x44, 0x55, 0x66, 0xda, 0x45, 0x40}
#endif

#define BT_TASK_PRIORITY  13
#define BT_PASSWORD "0000"

#define BT_DEVICE_OF_CLASS (COD_SERVICE_AUDIO | COD_SERVICE_RENDERING | COD_MAJOR_DEVICE_AUDIO | COD_MINOR_DEVICE_AUDIO_WEARABLE_HEADSET_DEVICE)
//0x240404 //audio device//0x40020C phone ///< local device class


#define SNIFF_INTERVAL      0x320
#define BT_DEVICES_LIST_COUNT 4
#define BT_UNISOC_CID 0x073F

/* the maximum number of indicator assigned numbers (defined on the Bluetooth SIG Assigned Numbers)
 * currently supported in blue.
*/
#define MAX_SPPORTED_HF_INDICATOR_ASSIGNED_NUMBER 2

#define MAX_SPPORTED_CLASSIC_LINKS    8
#define MAX_SPPORTED_LE_LINKS    16

#define APP_MSBC_SUPPORT 0

/** sizeof(bt_vendor_app_config_t)= 0x1c*/
typedef struct
{
    UINT32   support_spp: 1;
    UINT32   support_a2dp: 1;
    UINT32   support_aac: 1;
    UINT32   support_avrcp: 1;
    UINT32   support_hid: 1;
    UINT32   support_hfp: 1;
    UINT32   support_reserved;
} bt_vendor_app_config_t;

/**************************************
** Stack Config
***************************************/
typedef struct
{
    UINT8 max_timers;		///< max number of timer
    UINT8 max_buff;

    UINT8 max_message;		///< the capacity of the message queue
    UINT16 message_length;	///< max length of each message

    UINT8 host_mem_support;
    UINT16 host_mem_size; ///< in block(256Bytes)
} host_ctrl_config_t;

/******************* HCI config******************/
typedef struct
{
    UINT8 flow_control_support;
    UINT8 flow_control_timeout;
    UINT8 flow_control_threshold;

    UINT8 max_acl_number;
    UINT16 max_acl_size;
    UINT8 max_sco_number;
    UINT8 max_sco_size;

    UINT32 max_acl_queue_size;
} hci_config_t;

/******************* Manager config***************/
typedef struct
{
    /** max support device/connects */
    UINT8 max_acl_links;
    UINT8 max_device_list;

    /** suport multi connections or not */
    UINT8 multi_connect_support;

    /** class of local device */
    UINT32 device_class;

    /** enable host security */
    UINT8 security_support;

    /* packet types for SCO/eSCO link
    avilable value of following bit or:
    #define HCI_SYN_HV1         0x0001
    #define HCI_SYN_HV2         0x0002
    #define HCI_SYN_HV3         0x0004
    #define HCI_SYN_EV3         0x0008
    #define HCI_SYN_EV4         0x0010
    #define HCI_SYN_EV5         0x0020
    #define HCI_SYN__2EV3          0x0040
    #define HCI_SYN__3EV3          0x0080
    #define HCI_SYN__2EV5          0x0100
    #define HCI_SYN__3EV5          0x0200
    */
    UINT16 sco_packets;
    UINT16 voice_setting;

    /** process sco packet by hci or not */
    UINT8 sco_via_hci;

    UINT8 ssp_support;

    /* local io capability for simple security pairing,
    for mobile, select MGR_IOCAP_DISPLAY_YESNO or  MGR_IOCAP_KEYBOARD_ONLY
    for other device, MGR_IOCAP_DISPLAY_ONLY and MGR_IOCAP_NOINPUT_NOOUTPUT can be selected */
    UINT8 io_capability;

    UINT8 sniff_timeout;
    UINT16 sniff_interval;
    UINT8 unsniff_packets;
    UINT8 linkto_support;
    UINT32 linkto_value;
    UINT8 auto_repair_support;
    UINT8 default_le_mode;
    UINT8 support_switch_role;
    UINT8 support_all_event_report;
    UINT16 scan_interval_value;
    UINT16 scan_window_value;
    UINT32 support_ssp_debug:1;
} manager_config_t;


/****************** L2CAP config****************/
typedef struct
{
    UINT8 auto_config;
    UINT8 fragment_acl_support;
    UINT8 config_to_core;

    UINT8 max_channel;
    UINT8 max_links;

    UINT16 max_in_mtu;
    UINT16 max_out_mtu;

    /** If 0, send connectionless traffic as ACTIVE broadcast, if 1 use PICONET broadcast */
    UINT8 use_piconet_broadcast;
    UINT16 max_broadcast_mtu;
    /** handle used for broadcast acl data send */
    UINT16 broadcast_handle;
    /****************************************************************
     *   L2CAP  extended features bit mask   4 octs
     *   bit 0              |   bit 1               |   bit 2               |   bit 3                           |
     *   flow control mode  | retransmission mode   | Bi-directional QoS1   | Enhanced retransmission Mode  |
     *
     *   bit 4              |   bit 5               |   bit 6               |   bit 7                       |
     *   Streaming Mode | FCS Option            | Ex flow Spec BR/EDR   | Fixed Channels                |
     *
     *   bit 8              |   bit 9                           |   bit 9-31    |
     *   ex window size     | unicast connectionless Data reception| reserved       |
    */
    UINT32 extended_features;
} l2cap_config_t;


/******************** SDP config*****************/
typedef struct
{
    UINT16 mtu;        ///< normal 48
} sdp_config_t;

/******************** OBEX config***************/
typedef struct
{
    UINT16 local_mtu;
    UINT8 server_transport;
    //UINT16     max_path;
    UINT16 pbap_buf_size;
} obex_config_t;

/******************* RFCOMM config***************/
typedef struct
{
    UINT8 init_credit;
    UINT8 max_credit;
    UINT8 min_credit;
    UINT8 credit_issue;

    UINT16 max_frame_size;
} rfcomm_config_t;


//******************************************* Profiles ****************************************************/
/*hf/ag support indicator and it's status */
typedef struct
{
    UINT16 support_inindicator;
    UINT8 support_inindicator_status;
} hf_ag_support_indicator;

/** hfp config*/
typedef struct
{
    UINT16 hf_feature;
    UINT16 ag_feature;
    UINT8 handle_at_cmd; ///< handle at commond by application
    UINT8 max_device_nb;
    UINT8 msbc_support;

    /* add for hf/ag indicators*/
    hf_ag_support_indicator hf_indicator[MAX_SPPORTED_HF_INDICATOR_ASSIGNED_NUMBER];
    hf_ag_support_indicator ag_indicator[MAX_SPPORTED_HF_INDICATOR_ASSIGNED_NUMBER];
} hfp_config_t;

/**a2dp config*/
typedef struct
{
    UINT16 media_mtu;
    UINT16 signal_mtu;
    UINT8 accetp_connect;
    UINT8 max_device_nb;
    UINT8 bit_pool;
} a2dp_config_t;

/**spp config*/
typedef struct
{
    UINT8 max_ports; ///< max active spp virtual port
    UINT8 max_devices;
    UINT8 server_num; ///< max server number
    UINT16 max_data_len; ///< max data length
} spp_config_t;

/**pan config*/
typedef struct
{
    UINT8 min_mtu;
    UINT16 max_mtu;
} pan_config_t;

//added for SMP of bt4.0 2012-12-11
/**smp config*/
typedef struct
{
    UINT8 ioCap;
    UINT8 oobDataFlg;
    UINT8 authReq;
    UINT8 maxEncryptKeySize;
    UINT8 initKeyDistribution;
    UINT8 rspKeyDistribution;
    UINT8 host_calc;
    UINT32 passkey;
    UINT32 pairing_timeout;
} smp_config_t;

typedef struct
{
    UINT8 max_module_num;
    UINT8 max_server_connection;
    UINT16 max_server_rsp_length;
    UINT16 max_server_attribute_length;
    UINT16 max_server_notification_length;
    UINT16 max_server_att_value_length;
} le_client_config_t;

typedef struct
{
    UINT16 client_mtu;
    UINT16 server_mtu;
    UINT16 default_mtu;
    UINT8 max_client_num;
    UINT8 max_prepare_write_num;
    UINT8 max_app_module_num;
} le_config_t;

typedef struct _mesh_config
{
    UINT8 network_cache_size;
    UINT8 friendship_size;
    UINT8 subscription_list_size;
    UINT8 pdu_list_size;
    UINT8 gatt_enable;
    UINT8 mesh_feature;
	UINT8 relay;
    UINT8 relay_cnt;
    UINT8 relay_interval;
	UINT8 max_net_key;
	UINT8 max_app_key;
	UINT16 max_node;
} mesh_config_t;

/**coonfiguration of each protocol layer*/
typedef struct
{
    host_ctrl_config_t host_ctrl_config;

    hci_config_t hci_config;
    manager_config_t manager_config;
    l2cap_config_t l2cap_config;
    sdp_config_t sdp_config;
    obex_config_t obex_config;
    rfcomm_config_t rfcomm_config;

    hfp_config_t hfp_config;
    a2dp_config_t a2dp_config;
    spp_config_t spp_config;
	mesh_config_t mesh_config;
    pan_config_t pan_config;
    smp_config_t smp_config;
    le_client_config_t le_client_config;
    le_config_t le_config;
} bt_stack_config_t;

typedef struct
{
    bdaddr_t addr;
    UINT32 cod;/// device of calss
    UINT8 device_name[MAX_BT_DEVICE_NAME];
    UINT8 link_key[SIZE_OF_LINKKEY];
    UINT8 rssi;
    UINT8 adp_state;
    UINT8 acl_state;
    UINT8 key_type;
    UINT8 peer_volume_hfp;
    UINT8 peer_volume_a2dp;
} bt_dm_device_t;

typedef struct
{
    bt_dm_device_t *device_list;
    UINT8 max_device_num;
    UINT8 connect_index;
    UINT8 password[SIZE_OF_LINKKEY];
    bdaddr_t local_addr;
    UINT8 local_name[MAX_BT_DEVICE_NAME];
    UINT8 bt_mode;
    bdaddr_t le_addr;
    UINT32 device_class;
    UINT8 le_name[MAX_LE_DEVICE_NAME];
} bt_dm_adapter_t;

typedef struct
{
    UINT8 task_priority;
    UINT8 max_device_num;
    UINT32 support_spp:1;
    UINT32 support_a2dp:1;
    UINT32 support_aac:1;
    UINT32 support_avrcp:1;
    UINT32 support_hid:1;
    UINT32 support_hfp:1;
    UINT32 support_ssp_debug:1;
    UINT32 support_uws:1;
    UINT32 support_reserved;
} bt_config_t;


typedef struct
{
    bt_dm_adapter_t adapter;
    ////////////////////////////////////
    //used for app
    INT8 volume;
    UINT8 last_device;
    UINT16 magic;
    UINT8 old_device;
    UINT16 times_num;          ///<reconnect start
    UINT16 times_value;
    UINT8 connect_retry_cnt;
    UINT32 auto_connect_timer;  ///<reconnect end
    UINT8 eq_type;
    UINT8 last_device_two;
    INT8 volume_hfp;
    INT8 volume_a2dp;
    UINT16 bt_default_visable:1;
    UINT16 support_LE_2M_AUTOSET:1;
    UINT16 other_reserved:14;
} bt_storage_t;

///< bt_flash struct
typedef struct
{
    bt_dm_device_t device_list[BT_DEVICES_LIST_COUNT];
    UINT8 max_device_num;
    UINT8 connect_index;
    UINT8 password[SIZE_OF_LINKKEY];
    bdaddr_t local_addr;
    UINT8 local_name[MAX_BT_DEVICE_NAME];
    UINT8 bt_mode;
    bdaddr_t le_addr;
    UINT32 device_class;
    UINT8 le_name[MAX_LE_DEVICE_NAME];
} bt_dm_adapter_flash_t;


typedef struct
{
    bt_dm_adapter_flash_t adapter;
    //used for app
    INT8 volume;
    UINT8 last_device;
    UINT16 magic;
    UINT8 old_device;
    UINT16 times_num;          ///<reconnect start
    UINT16 times_value;
    UINT8 connect_retry_cnt;
    UINT32 auto_connect_timer;  ///<reconnect end
    UINT8 eq_type;
    UINT8 last_device_two;
    INT8 volume_hfp;
    INT8 volume_a2dp;
    UINT16 bt_default_visable:1;
    UINT16 support_LE_2M_AUTOSET:1;
    UINT16 other_reserved:14;
} bt_storage_flash_info_t;

typedef struct
{
    UINT16 activated;
    UINT8 DEVICE_ADDR[6];
    UINT8 DEVICENAME[32];
    UINT8 PINCODE[16];
} TGT_BT_INFO_T;

#endif
