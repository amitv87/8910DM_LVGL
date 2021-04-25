/**  @file
  ql_api_bt_commom.h

  @brief
  This file is used to define bt api commom for different Quectel Project.
  ql_api_bt.h can use this common define.

*/

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


#ifndef QL_API_BT_COMMON_H
#define QL_API_BT_COMMON_H
#include "ql_api_common.h"

/*========================================================================
 *  Variable Definition
 *========================================================================*/
typedef enum
{
    QL_BT_SUCCESS                   =   0,
    //
    QL_BT_ERROR                 =   1 | (QL_COMPONENT_BSP_BT << 16),
    QL_BT_PENDING,
    QL_BT_BUSY_ERR,
    QL_BT_NO_RESOURCE_ERR,
    QL_BT_NOT_FOUND_ERR,
    QL_BT_DEVICE_NOT_FOUND_ERR,
    QL_BT_CONNECTION_FAILED_ERR,
    QL_BT_TIMEOUT_ERR,
    QL_BT_NO_CONNECTION_ERR,
    QL_BT_INVALID_PARM_ERR,
    QL_BT_NOT_SUPPORTED_ERR,
    QL_BT_CANCELLED_ERR,
    QL_BT_IN_PROGRESS_ERR,
    QL_BT_RESTRICTED_ERR,
    QL_BT_INVALID_HANDLE_ERR,
    QL_BT_PACKET_TOO_SMALL_ERR,
    QL_BT_NO_PAIR_ERR,
    QL_BT_FAILED_ERR,
    QL_BT_DISCONNECT_ERR,
    QL_BT_NO_CONNECT_ERR,
    QL_BT_IN_USE_ERR,
    QL_BT_MEDIA_BUSY_ERR,
    QL_BT_OFF_ERR,

    //蓝牙协议栈返回的错误
    QL_BT_SYS_FAIL_ERR,                     //操作失败
    QL_BT_SYS_DONE_ERR,                     //处于完成状态
    QL_BT_SYS_PENDING_ERR,                  //处于等待状态
    QL_BT_SYS_INVAUED_PARAM_ERR,            //输入的参数无效
    QL_BT_SYS_ISECFAIL_ERR,                 //安全连接失败
    QL_BT_SYS_FLOWCTRL_ERR,                 //处于流控状态
    QL_BT_SYS_NORESOURCES_ERR,              //资源不足
    QL_BT_SYS_UNSUPPORTED_ERR,              //设备或能力不支持
    QL_BT_SYS_HW_ERR,                       //蓝牙硬件错误
    QL_BT_SYS_HOST_ERR,                     //蓝牙主机错误
    QL_BT_SYS_UNKOWN_ERR,                   //蓝牙未知错误
    QL_BT_SYS_NOT_READY_ERR,                //处于未就绪状态
    QL_BT_SYS_READY_ERR,                    //处于就绪状态
    QL_BT_SYS_AUTHORISE_FAIL_ERR,           //授权失败
    QL_BT_SYS_AUTHEN_FAIL_ERR,              //认证失败
    QL_BT_SYS_ENCRYPT_FAIL_ERR,             //加密失败
    QL_BT_SYS_TIMEOUT_ERR,                  //操作超时
    QL_BT_SYS_PROTOCOL_ERR,                 //协议错误
    QL_BT_SYS_DISALLOWED_ERR,               //操作不允许
    QL_BT_SYS_BUFF_SAMLL_ERR,               //蓝牙协议栈buffer太小
    QL_BT_SYS_DISCONNECT_ERR,               //处于未连接状态
    QL_BT_SYS_MORE_ERR,                     //更多错误

    QL_BT_REGISTER_MSG_ERR,
    QL_BT_DISCONNECT,
    QL_BT_ALREADY_CONNECTED_ERR,            //已经建立了连接
    QL_BT_ALREADY_STARTED_ERR,
    QL_BT_ALREADY_STOPED_ERR,
    QL_BT_NOT_START_ERR,
    QL_BT_NOT_STOP_ERR,
    QL_BT_PRAMA_TYPE_ERR,
    QL_BT_PRAMA_LENGTH_ERR,
    QL_BT_PRAMA_DATA_ERR,                   //BT/BLE input parameter error
    QL_BT_PRAMA_SIZE_ERR,
    QL_BT_PRAMA_NULL_ERR,
    QL_BT_NOT_INQURY_ERR,
    QL_BT_SPP_NOT_CONNECT_ERR,
    QL_BT_SPP_NOT_SUPPORT_ERR,
    QL_BT_GET_PAIR_INFO_ERR,
    QL_BT_REMVOE_PAIR_INFO_ERR,
    QL_BT_GET_WHITELIST_ERR,
    QL_BT_MEM_ADDR_NULL_ERR,
    QL_BT_GET_FILE_ADDR_ERR,
    QL_BT_WIFI_USING_ERR,
    QL_BT_NOT_INIT_ERR,
    QL_BT_NOT_REGISTER_CB_ERR,
    QL_BT_TASK_NOT_EXIT_ERR,
    QL_BT_IS_USING_ERR,
    QL_BT_ALREADY_REGISTERED_ERR,
    QL_BT_SERVICE_ALREADY_EXITED_ERR,
    QL_BT_SERVICE_NOT_EXITED_ERR,
    QL_BT_CHARA_ALREADY_EXITED_ERR,
    QL_BT_CHARA_NOT_EXITED_ERR,
    QL_BT_NOT_CONN_ID_ERR,
    QL_BLE_PUBLIC_ADDR_NOT_USED_ERR,                //cannot use ble public address
    QL_BLE_CID_ERR,                                 //没有对应cid的通道
    QL_BLE_HANDLE_ERR,                              //使用了错误的句柄
    QL_BLE_PROP_ERR,                                //属性错误
    QL_BLE_WRITE_NAME_FILE_ERR,                     //写蓝牙名称文件错误
    QL_BLE_READ_NAME_FILE_ERR,                      //读蓝牙名称文件错误

    //bt hfp error
    QL_BT_HFP_CONNECTED_ERR = 100 | (QL_COMPONENT_BSP_BT << 16),    //bt hfp:already connected
    QL_BT_HFP_NOT_CONN_ADDR_ERR,                                    //bt hfp:not the same address whih connected address
    QL_BT_HFP_NOT_CONNECTED_ERR,                                    //bt hfp:not connected
    QL_BT_HFP_NOT_OPERATION_ERR,                                    //bt hfp:operation not support
    
    //bt audio error
    QL_BT_AUDIO_SEND_MSG_ERR = 200 | (QL_COMPONENT_BSP_BT << 16),    //bt audio:seng msg to queue failed

    //bt a2dp error
    QL_BT_A2DP_CONNECT_ERR = 300 | (QL_COMPONENT_BSP_BT << 16),     //bt a2dp:connect failed
    QL_BT_A2DP_DISCONNECT_ERR,                                      //bt a2dp:disconnect failed

    //bt avrcp error
    QL_BT_AVRCP_START_ERR = 400 | (QL_COMPONENT_BSP_BT << 16),      //bt avrcp:start failed
    QL_BT_AVRCP_PAUSE_ERR,                                          //bt avrcp:pause failed
    QL_BT_AVRCP_PREVI_ERR,                                          //bt avrcp:previ failed
    QL_BT_AVRCP_NEXT_ERR,                                           //bt avrcp:next failed
    QL_BT_AVRCP_VOL_SET_ERR,                                        //bt avrcp:set vol failed
    QL_BT_AVRCP_VOL_GET_ERR,                                        //bt avrcp:get vol failed
    QL_BT_AVRCP_PLAY_STATE_GET_ERR,                                 //bt avrcp:get play state failed
}ql_errcode_bt_e;

#define QL_BT_MAC_ADDRESS_SIZE              6

//bt/ble address
typedef struct
{
    unsigned char addr[QL_BT_MAC_ADDRESS_SIZE];
} ql_bt_addr_s;

//connection parameter
typedef struct
{
    unsigned short conn_id;                         //connection id
    unsigned short max_interval;                    //最大的间隔，用来维持连接(可以理解成心跳)，间隔：1.25ms，时间范围：7.5ms~4s
    unsigned short min_interval;                    //最小的间隔，间隔：1.25ms，时间范围：7.5ms~4s
    unsigned short latency;                         //从机可以忽略的维持的时间，一般10个心跳可以忽略6个
    unsigned short timeout;                         //没有交互，超时断开时间，间隔：10ms，时间范围：100ms~32s
}ql_ble_update_conn_infos_s;

//ble gatt uiid information
#define QL_BLE_LONG_UUID_SIZE                   16
typedef enum 
{
    QL_BLE_GATT_LONG_UUID = 0,                      //长UUID，128bit
    QL_BLE_GATT_SHORT_UUID,                         //短UUID，16bit
}ql_ble_gatt_uuid_state_e;

typedef struct
{
    ql_ble_gatt_uuid_state_e uuid_type;
    unsigned char uuid_l[QL_BLE_LONG_UUID_SIZE];
    unsigned short uuid_s; 
}ql_ble_gatt_uuid_s;

#define QL_BT_BLE_NAME_MAX_SIZE             22

//bt/ble local name
typedef struct
{                             
    unsigned char name[QL_BT_BLE_NAME_MAX_SIZE+1];  //蓝牙名称实际长度，最大长度为22
}ql_bt_ble_local_name_s;

//BLE IBEACON信息
typedef struct
{
    unsigned char uuid_l[QL_BLE_LONG_UUID_SIZE];
    unsigned short major;
    unsigned short minor;
}ql_ble_ibeacon_cfg_s;

/*========================================================================
 *  function Definition
 *========================================================================*/

#endif /* QL_API_BT_COMMON_H */

