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

#include "bt_types.h"

#define EFFUSE_VALID_FLAG 0xe2

#define MAX_BT_DEVICE_LIST 10
#define MAX_BT_DEVICE_NAME 32

#define BT_DEVICE_OF_CLASS 0x240404
#define SNIFF_INTERVAL 0x320

#define BT_DEVICE_LIST_MAX 40

typedef struct
{
    UINT32 support_spp : 1;
    UINT32 support_a2dp : 1;
    UINT32 support_aac : 1;
    UINT32 support_avrcp : 1;
    UINT32 support_avrcp_vol_sync : 1;
    UINT32 support_hid : 1;
    UINT32 support_hfp : 1;
    UINT32 support_number_voice : 1;
    UINT32 support_only_one_link : 1;
    UINT32 support_dc_offset : 1;
    UINT8 bt_addr[6];
    UINT8 bt_name[16];

    UINT8 reserved2 : 5;
} bt_vendor_app_config_t; // Size: 0x1c

typedef struct
{
    UINT32 cod : 3; // 0x00000000
    UINT32 io_capability : 2;
    UINT32 linkto_value : 3;
    UINT32 bt_mode : 2; // 0x00000001
    UINT32 switch_role : 1;
    UINT32 max_frame_size : 2;
    UINT32 max_device_nb : 2;
    UINT32 ssp_support : 1;
    UINT32 interval_win : 4; // 0x00000002
    UINT32 reserved1 : 4;
    UINT8 sniff_timeout;   // 0x00000003
    UINT16 sniff_interval; // 0x00000004
    UINT16 hf_feature;     // 0x00000006
    UINT32 bit_pool : 5;
    UINT32 reserved2 : 27; // 0x00000008
} bt_vendor_config_t;      // Size: 0x0c

/**************************************
** Stack Config
***************************************/

typedef struct
{
    UINT8 max_timers;
    UINT8 max_buff;

    UINT8 max_message;
    UINT16 message_length;

    UINT8 host_mem_support;
    UINT16 host_mem_size; // in block(256Bytes)
} host_ctrl_config_t;

/******************* HCI ******************/
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

/******************* Manager ***************/
typedef struct
{
    /* max support device/connects */
    UINT8 max_acl_links;
    UINT8 max_device_list;

    /* suport multi connections or not */
    UINT8 multi_connect_support;

    /* class of local device */
    UINT32 device_class;

    /* enable host security */
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

    /* process sco packet by hci or not */
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
    UINT8 support_soundbar;
    UINT8 support_switch_role;
    UINT8 support_all_event_report;
    UINT16 scan_interval_value;
    UINT16 scan_window_value;
} manager_config_t;

/****************** L2CAP ****************/
typedef struct
{
    UINT8 auto_config;
    UINT8 fragment_acl_support;
    UINT8 config_to_core;

    UINT8 max_channel;
    UINT8 max_links;

    UINT16 max_in_mtu;
    UINT16 max_out_mtu;

    /* If 0, send connectionless traffic as ACTIVE broadcast, if 1 use PICONET broadcast */
    UINT8 use_piconet_broadcast;
    UINT16 max_broadcast_mtu;
    /* handle used for broadcast acl data send */
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

/******************** SDP *****************/
typedef struct
{
    UINT16 mtu; // normal 48
} sdp_config_t;

/******************** OBEX ***************/
typedef struct
{
    UINT16 local_mtu;
    UINT8 server_transport;
    //UINT16     max_path;
    UINT16 pbap_buf_size;
} obex_config_t;

/******************* RFCOMM ***************/
typedef struct
{
    UINT8 init_credit;
    UINT8 max_credit;
    UINT8 min_credit;
    UINT8 credit_issue;

    UINT16 max_frame_size;

} rfcomm_config_t;

/******************************************* Profiles ****************************************************/
typedef struct
{
    UINT16 hf_feature;
    UINT16 ag_feature;
    UINT8 handle_at_cmd; // handle at commond by application
    UINT8 max_device_nb;
    UINT8 msbc_support;
} hfp_config_t;

typedef struct
{
    UINT16 media_mtu;
    UINT16 signal_mtu;
    UINT8 accetp_connect;
    UINT8 max_device_nb;
    UINT8 bit_pool;
} a2dp_config_t;

typedef struct
{
    UINT8 max_ports;     // max active spp virtual port
    UINT8 server_num;    // max server number
    UINT16 max_data_len; // max data length
} spp_config_t;

typedef struct
{
    UINT8 min_mtu;
    UINT16 max_mtu;
} pan_config_t;

//added for SMP of bt4.0 2012-12-11
typedef struct
{
    UINT8 ioCap;
    UINT8 oobDataFlg;
    UINT8 authReq;
    UINT8 maxEncryptKeySize;
    UINT8 initKeyDistribution;
    UINT8 rspKeyDistribution;
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
    pan_config_t pan_config;
    smp_config_t smp_config;
    le_client_config_t le_client_config;
    le_config_t le_config;
} bt_config_t;

typedef struct
{
    bdaddr_t addr;
    UINT32 cod;
    UINT32 sys_build_date;
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
    UINT32 sys_buid_date;
} bt_dm_adapter_t;

typedef struct
{
    bt_dm_adapter_t adapter;
    ////////////////////////////////////
    //used for app
    INT8 volume;
    UINT8 last_device;
    UINT16 magic;
    UINT8 old_device;
    UINT16 times_num; //reconnect start
    UINT16 times_value;
    UINT8 connect_retry_cnt;
    UINT32 auto_connect_timer; //reconnect end
    UINT8 eq_type;
    UINT8 last_device_two;
    INT8 volume_hfp;
    INT8 volume_a2dp;
} app_local_storage_t;

typedef struct
{
    UINT16 activated;
    UINT8 DEVICE_ADDR[6];
    UINT8 DEVICENAME[32];
    UINT8 PINCODE[16];
} TGT_BT_INFO_T;

#define BT_LOG_MENU_SUPPORT
/*
 # Option1:NONE      #define BLUETOOTH_SUPPORT_NONE
 # Option2:CSR_BC4_6 #define BLUETOOTH_SUPPORT_CRS_BC4_6
 # Option3:CSR_BC4_6_LOW_MEM: low mem and code size , only support OPP HFG
 # Option4:RTL8723   Realtek RTL8723AS wifi/bt combo chip
 # Option5:IS1662    ISSC IS1662S bt chip
 # Option6:SPRD_BT			#define BLUETOOTH_SUPPORT_SPRD_BT

*/
#define BLUETOOTH_SUPPORT_SPRD_BT
//#define BT_HL_PATCH_SUPPORT
/*
# Option1:PCM 
# Option2:UART 
*/
#define BT_SCO_OVER_UART
#define BT_USB_LOG
#define BT_NONSIG_SUPPORT
#define BT_BQB_SUPPORT
#define BT_OPP_SUPPORT
#define BT_FTS_SUPPORT
//#define BT_DUN_SUPPORT
#define BT_SPP_SUPPORT
//#define BT_SPP_128BIT_UUID_SUPPORT
//#define BT_SPP_2ND_CHANNEL_SUPPORT
//#define BT_MAP_SUPPORT
//#define BT_AV_SNK_SUPPORT
//#define BT_AV_CT_SUPPORT
#define BT_HFG_SUPPORT
//#define BT_DIALER_SUPPORT
//#define BT_HFU_SUPPORT
//#define BT_PBAP_SUPPORT
//#define BT_HSP_SUPPORT
//#define BT_A2DP_SUPPORT
//#define BT_FIXPIN_SUPPORT
//#define BT_A2DP_SINK_AUDIO_SUPPORT
//#define BT_A2DP_SAMPLERATE_RECONFIG_SUPPORT
#define BT_ARM_NXP_ENABLE
#define BT_POWEROFF_CONTROLLER

#define UIS8910_BT_PLATFORM
//#define CONFIG_AT_BT_SCANMODE_SELECT   //∆¡±Œ π”√scanmod AT
#endif
