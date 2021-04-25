/**  @file
  ql_api_bt.h

  @brief
  This file is used to define bt api for different Quectel Project.

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


#ifndef QL_API_BT_H
#define QL_API_BT_H
#include "ql_api_common.h"
#include "ql_api_bt_common.h"
#ifdef __cplusplus
extern "C" {
#endif


/*========================================================================
 *  Variable Definition
 *========================================================================*/

typedef unsigned int QL_BT_STATUS;
#define QL_BT_STATUS_SUCCESS               0
#define QL_BT_STATUS_ERROR                 1

#define QL_BT_DEVICE_NAME_SIZE              41
#define QL_BT_SPP_SEND_SIZE                 128

#define QL_BLE_VERSION_SIZE                 128
#define QL_BLE_SEND_DATA_MAX_SIZE           512

#define QL_BLE_UPDATE_CONN_MIN_INTERVAL     60
#define QL_BLE_UPDATE_CONN_MAX_INTERVAL     80
#define QL_BLE_UPDATE_CONN_LATENCY          4
#define QL_BLE_UPDATE_CONN_TIMEOUT          500


#define QL_BLEADV_REAL_DATA_LEN             254         //256 - 2
#define QL_BLESCAN_MAX_REMOTE_NAME_LEN      32
#define QL_BLE_ADV_MIN_ADV                  0x300
#define QL_BLE_ADV_MAX_ADV                  0x320
#define QL_BLE_ADV_MAP                      0x07
#define QL_BLEADV_ADV_MAX                   0x4000
#define QL_BLEADV_ADV_MIN                   0x20
#define QL_BLE_ADV_DATA_SIZE                31


#define QL_BLE_SCAN_INTERVAL                0x100
#define QL_BLE_SCAN_WINDOW                  0x50
#define QL_BLESCAN_SCAN_MAX                 0x4000
#define QL_BLESCAN_SCAN_MIN                 0x4

#define QL_GATT_MTU_MAX                     247 
#define QL_BLE_WHITELIST_NUM_MAX            12


typedef enum 
{
    QL_BT_STOPED                        = 0,            //bt/ble is stoped
    QL_BT_STARTED                       = 1,            //bt/ble is started
}ql_bt_state_e;

typedef struct 
{
    unsigned short name[QL_BT_DEVICE_NAME_SIZE];        //bt name
    ql_bt_addr_s addr;                                  //bt address
    unsigned int dev_class;                             //clasee level
    unsigned int rssi;                                  //rssi
}ql_bt_info_s;


typedef enum 
{
    QL_BT_NONE_ENABLE                   = 0,            //can't be discovered and connectable
    QL_BT_INQUIRY_ENABLE,                               //can be discovered,but can't be connected
    QL_BT_PAGE_ENABLE,                                  //can't be discovered,but can be connected
    QL_BT_PAGE_AND_INQUIRY_ENABLE,                      //can be discovered and connected
}ql_bt_visible_mode_e;

typedef enum 
{
    QL_BT_CONNECT_SPP,
}ql_bt_connect_mode_e;

typedef struct
{
    ql_bt_connect_mode_e mode;                  //连接模式
    ql_bt_addr_s addr;                          //需要连接的BT地址
}ql_bt_connect_info_s;

typedef enum
{
    QL_BT_SERVICE_NONE = 0,
    QL_BT_SERVICE_HANDFREE,
    QL_BT_SERVICE_HANDSET,
    QL_BT_SERVICE_OPP,
    QL_BT_SERVICE_FTP,
    QL_BT_SERVICE_A2DP,
    QL_BT_SERVICE_AVRCP,
    QL_BT_SERVICE_SPP,
    QL_BT_SERVICE_A2DP_SRC,
    QL_BT_SERVICE_AVRCP_TG,
    QL_BT_SERVICE_MAP_SERVER,
    QL_BT_SERVICE_PBAP_SERVER,
    QL_BT_SERVICE_HANDFREE_GATEWAY,
    QL_BT_SERVICE_HEADSET_GATEWAY,
    QL_BT_SERVICE_BLE_HID,
    QL_BT_SERVICE_ALL,
}ql_bt_inquiry_type_e;

typedef enum
{
    QL_BT_AVRCP_PAUSE = 0x00,
    QL_BT_AVRCP_START,
    QL_BT_AVRCP_PREVI,
    QL_BT_AVRCP_NEXT,
}ql_bt_handle_avrcp_state_t;


typedef struct
{
    unsigned char len;                                          //max 128
    unsigned char data[QL_BT_SPP_SEND_SIZE];                    //data will be sent
}ql_bt_spp_info_s;

typedef struct
{
    unsigned char *data;
    unsigned short data_len;
} ql_bt_spp_data_s;

typedef enum 
{
    QL_BLE_PUBLIC_ADDRESS                       = 0,        //public address
    QL_BLE_RANDOM_ADDRESS                       = 1,        //random address
}ql_ble_address_mode_e;

//ble whitelist information
typedef struct
{
    ql_ble_address_mode_e mode;             //地址模式，0-公共地址，1-随机地址
    ql_bt_addr_s addr;                      //BLE地址
}ql_ble_whitelist_info_s;

//unisoc bt/ble nv information,must the same with 
typedef struct
{
    ql_bt_addr_s bd_addr;
    unsigned char local_name[32];
    unsigned short xtal_dac;
}ql_bt_nv_param_s;

typedef enum 
{
    QL_BLE_CONN_NONE                            = 0,
    QL_BLE_CONN_ACTIVE                          = 1,
    QL_BLE_CONN_CLOSED                          = 2,
    QL_BLE_CONN_PENDING                         = 3,
    QL_BLE_CONN_RELEASEPENDING                  = 4,
    QL_BLE_CONN_PENDINGHOLDALLACLS              = 5,
    QL_BLE_CONN_REQPENDING                      = 6,
}ql_ble_connection_state_e;


typedef enum
{
    QL_BLE_ADV_NONE_STATE = 0x00,                       //不处于广播状态
    QL_BLE_ADV_STATE,                                   //处于广播状态
}ql_ble_adv_state_e;

typedef enum 
{
    QL_BLEADV_INV                               = 0,    //可连接的非定向广播，默认选择
    QL_BLEADV_HDCDA                             = 1,    //可连接高占空比的定向广播    
    QL_BLEADV_SCAN_IND                          = 2,    //可扫描的非定向广播
    QL_BLEADV_NONCONN_IND                       = 3,    //不可连接的非定向广播
    QL_BLEADV_LDCDA                             = 4,    //可连接低占空比的定向广播
}ql_bleadv_type_info_e;

typedef enum 
{
    QL_BLEADV_FILTER_0                          = 0,            //处理所有设备的扫描和连接请求
    QL_BLEADV_FILTER_1                          = 1,            //处理所有设备的连接请求和只处理白名单设备的扫描请求
    QL_BLEADV_FILTER_2                          = 2,            //处理所有设备的扫描请求和只处理白名单设备的连接请求
    QL_BLEADV_FILTER_3                          = 3,            //只处理白名单设备的连接和扫描请求
}ql_bleadv_fliter_info_e;

typedef struct
{
    unsigned int max_adv;                                       //广播最大间隔：范围0x0020-0x4000。gap 0.625ms, 20ms-10.24s
    unsigned int min_adv;                                       //广播最小间隔：范围0x0020-0x4000。gap 0.625ms, 20ms-10.24s
    ql_bleadv_type_info_e adv_type;                             //广播类型
    unsigned char own_addr_type;                                //本地地址类型。0-公共地址，1-随机地址
    ql_ble_address_mode_e direct_address_mode;                  //定向地址类型:0 public address, 1 random address
    ql_bt_addr_s direct_addr;                                   //定向地址
    unsigned char channel_map;                                  //广播通道。bit0-37，bit1-38，bit2-39。默认3个信道都选择
    ql_bleadv_fliter_info_e filter;                             //广播过滤策略
    unsigned char discov_mode;                                  //发现模式，GAP协议使用，默认为1(普通发现模式)
    unsigned char no_br_edr;                                    //不用BR/EDR，默认为1
    unsigned char enable_adv;                                   //使能广播，默认为1
}ql_bleadv_typical_addr_param_s;

typedef struct
{
    unsigned int max_adv;                                       //广播最大间隔：范围0x0020-0x4000。gap 0.625ms, 20ms-10.24s
    unsigned int min_adv;                                       //广播最小间隔：范围0x0020-0x4000。gap 0.625ms, 20ms-10.24s
    ql_bleadv_type_info_e adv_type;                             //广播类型
    unsigned char own_addr_type;                                //本地地址类型。0-公共地址，1-随机地址
    unsigned char channel_map;                                  //广播通道。bit0-37，bit1-38，bit2-39。默认3个信道都选择
    ql_bleadv_fliter_info_e filter;                             //广播过滤策略
    unsigned char discov_mode;                                  //发现模式，GAP协议使用，默认为1(普通发现模式)
    unsigned char no_br_edr;                                    //不用BR/EDR，默认为1
    unsigned char enable_adv;                                   //使能广播，默认为1
}ql_bleadv_param_s;

typedef struct
{
    unsigned char date_len;                                     //广播数据长度，不超过31个字节
    unsigned char data[QL_BLE_ADV_DATA_SIZE];                   //广播数据             
}ql_bleadv_set_data_s;

typedef enum
{
    QL_BLE_SCAN_NONE_STATE = 0x00,                          //不处于扫描状态
    QL_BLE_SCAN_STATE,                                      //处于扫描状态
}ql_ble_scan_state_e;


typedef enum 
{
    QL_BLESCAN_PASSIVE                          = 0,        //消极扫描
    QL_BLESCAN_ACTIVE                           = 1,        //积极扫描，广播端设置的扫描回复数据才会有意义
}ql_blescan_scan_mode_e;
    
typedef enum 
{
    QL_BLESCAN_FILTER_0                         = 0,        //除了不是本设备的定向广播，其他所有的广播包
    QL_BLESCAN_FILTER_1                         = 1,        //除了不是本设备的定向广播，白名单设备的广播包
    QL_BLESCAN_FILTER_2                         = 2,        //非定向广播，指向本设备的定向广播或使用Resolvable private address的定向广播
    QL_BLESCAN_FILTER_3                         = 3,        //白名单设备非定向广播，指向本设备的定向广播或使用Resolvable private address的定向广播
}ql_blescan_fliter_e;

typedef struct
{
    ql_blescan_scan_mode_e scan_mode;                       //默认为积极扫描
    unsigned int interval;                                  //扫描间隔。范围：0x0004-0x4000，gap 0.625ms, 2.5ms-10.24s
    unsigned int window;                                    //扫描时间。范围：0x0004-0x4000，gap 0.625ms, 2.5ms-10.24s
    ql_blescan_fliter_e filter;                             //扫描过滤策略，默认为0
    unsigned char own_addr_type;                            //本地地址类型，0-公共地址，1-随机地址
}ql_blescan_scan_s;
#if 0
typedef struct
{
    unsigned char name_length;
    unsigned char name[QL_BLESCAN_MAX_REMOTE_NAME_LEN];
    unsigned char addr_type;
    ql_bt_addr_s addr;
}ql_ble_scan_report_info_s;
#else
#define QL_BLE_MAX_REMOTE_NAME_LEN 32
typedef struct
{
    unsigned char name_length;
    unsigned char name[QL_BLE_MAX_REMOTE_NAME_LEN];
    unsigned char addr_type;
    ql_bt_addr_s addr;
    unsigned char event_type;
    unsigned char data_length;
    unsigned char rssi;
} ql_ble_scan_report_info_s;
#endif

typedef enum
{
    QL_BT_AUD_AVRCP_MSG = 0xA500,
    QL_BT_AUD_A2DP_MSG,
} QL_BT_AUDIO_MSG_E;

typedef enum
{
    QL_A2DP_HANDLE = 0,
    QL_AVRCP_HANDLE,
}ql_bt_a2dp_avrcp_handle_e;

/*========================================================================
 *  function Definition
 *========================================================================*/
typedef void (*ql_bt_callback)(void *ind_msg_buf, void *ctx);

/*****************************************************************
* Function: ql_bt_register_cb
*
* Description:
*   注册回调函数
* 
* Parameters:
*   bt_cb           [in]    回调函数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_register_cb(ql_bt_callback bt_cb);

/*****************************************************************
* Function: ql_bt_spp_init
*
* Description:
*   初始化BT SPP并注册回调函数。初始化会设置BT的mac地址
* 
* Parameters:
*   bt_cb           [in]    回调函数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_spp_init(ql_bt_callback bt_cb);

/*****************************************************************
* Function: ql_bt_spp_release
*
* Description:
*   BT SPP释放资源
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_spp_release();

/*****************************************************************
* Function: ql_bt_spp_init
*
* Description:
*   初始化BT HFP并注册回调函数。初始化会设置BT的mac地址
* 
* Parameters:
*   bt_cb           [in]    回调函数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_init(ql_bt_callback bt_cb);

/*****************************************************************
* Function: ql_bt_hfp_release
*
* Description:
*   BT HFP释放资源
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_release();

/*****************************************************************
* Function: ql_bt_a2dp_avrcp_init
*
* Description:
*   初始化BT A2DP和AVRCP并注册回调函数。初始化会设置BT的mac地址
* 
* Parameters:
*	bt_cb           [in]    回调函数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_a2dp_avrcp_init(ql_bt_callback bt_cb);

/*****************************************************************
* Function: ql_bt_a2dp_avrcp_release
*
* Description:
*   BT A2DP和AVRCP释放资源
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_a2dp_avrcp_release();

//以下API为BT/BLE 共有
/*****************************************************************
* Function: ql_bt_start
*
* Description:
*   开启BT/BLE
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_start();

/*****************************************************************
* Function: ql_bt_stop
*
* Description:
*   关闭BT/BLE
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_stop();

/*****************************************************************
* Function: ql_bt_get_state
*
* Description:
*   获取BT/BLE的开启状态
* 
* Parameters:
*   bt_state        [out]   BT/BLE开启状态
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_get_state(ql_bt_state_e *bt_state);

/*****************************************************************
* Function: ql_classic_bt_set_scanmode
*
* Description:
*   设置BT可见性
* 
* Parameters:
*   scanmode_info   [in]   BT可见性
*
* Return:ql_errcode_bt_e
*
*****************************************************************/

/*****************************************************************
* Function: ql_bt_ble_get_localname
*
* Description:
*   从NVM获取BT/BLE名称
* 
* Parameters:
*   local_name   [out]   BT名称
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_ble_get_localname(ql_bt_ble_local_name_s *local_name);

/*****************************************************************
* Function: ql_bt_ble_set_localname
*
* Description:
*   设置BT/BLE名称到NVM
* 
* Parameters:
*   local_name   [in]   BT名称
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_ble_set_localname(ql_bt_ble_local_name_s local_name);


//以下API为BT所有
/*****************************************************************
* Function: ql_classic_bt_get_scanmode
*
* Description:
*   设置BT可见性
* 
* Parameters:
*   scanmode_info   [in]   BT可见性
*
* Return:ql_errcode_bt_e
*
*****************************************************************/

ql_errcode_bt_e ql_classic_bt_set_scanmode(ql_bt_visible_mode_e scanmode_info);

/*****************************************************************
* Function: ql_classic_bt_get_scanmode
*
* Description:
*   获取BT可见性
* 
* Parameters:
*   scanmode_info   [out]   BT可见性
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_get_scanmode(ql_bt_visible_mode_e *scanmode_info);

/*****************************************************************
* Function: ql_classic_bt_get_localaddr
*
* Description:
*   获取BT地址(BT地址与BLE的公共地址是同一个)
* 
* Parameters:
*   local_name   [in]   BT可见性
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_get_localaddr(ql_bt_addr_s *local_addr);

/*****************************************************************
* Function: ql_classic_bt_start_inquiry
*
* Description:
*   开始搜索，搜索到的内容需要在回调里解析
* 
* Parameters:
*   inquiry_type    [in]   搜索的类型，默认QL_BT_SERVICE_ALL(搜索全部)
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_start_inquiry(ql_bt_inquiry_type_e inquiry_type);

/*****************************************************************
* Function: ql_classic_bt_cancel_inquiry
*
* Description:
*   取消搜索
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_cancel_inquiry();

/*****************************************************************
* Function: ql_classic_bt_start_bond
*
* Description:
*   开始配对
* 
* Parameters:
*   bond_addr   [in]   配对设备的地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_start_bond(ql_bt_addr_s bond_addr);

/*****************************************************************
* Function: ql_classic_bt_cancel_bond
*
* Description:
*   取消配对
* 
* Parameters:
*   bond_addr   [in]   配对设备的地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_cancel_bond(ql_bt_addr_s bond_addr);

/*****************************************************************
* Function: ql_classic_bt_get_bond_info
*
* Description:
*   获取所有已配对设备的信息
* 
* Parameters:
*   number          [in]   需要获取多少个配对设备信息
*   device_number   [out]  实际获取配对设备信息的个数，小于等于number
*   info            [out]  存放配对设备的信息
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_get_bond_info(unsigned int number,unsigned int * device_number,ql_bt_info_s info[]);

/*****************************************************************
* Function: ql_classic_bt_remove_typical_bond_device
*
* Description:
*   删除一个已配对设备
* 
* Parameters:
*   bond_addr       [in]   已配对设备地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_remove_typical_bond_device(ql_bt_addr_s bond_addr);

/*****************************************************************
* Function: ql_classic_bt_remove_all_bond_device
*
* Description:
*   删除所有的已配对设备
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_remove_all_bond_device();

/*****************************************************************
* Function: ql_classic_bt_connect
*
* Description:
*   建立连接
* 
* Parameters:
*   connect_info    [in]   连接远端设备的信息
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_connect(ql_bt_connect_info_s connect_info);

/*****************************************************************
* Function: ql_classic_bt_disconnect
*
* Description:
*   断开连接
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_disconnect();

/*****************************************************************
* Function: ql_classic_bt_get_connection
*
* Description:
*   获取指定设备的连接状态
* 
* Parameters:
*   connect_addr    [in]   远端设备的地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_get_connection(ql_bt_addr_s connect_addr);

/*****************************************************************
* Function: ql_classic_bt_spp_send
*
* Description:
*   BT SPP发送数据
* 
* Parameters:
*   spp_info        [in]   发送信息
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_classic_bt_spp_send(ql_bt_spp_info_s spp_info);



//以下API用于BLE
/*****************************************************************
* Function: ql_ble_get_version
*
* Description:
*   BLE 获取版本
* 
* Parameters:
*   version         [out]   版本数据
*   get_version_len [in]    读取版本的最大长度
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_get_version(char *version, unsigned int get_version_len);

/*****************************************************************
* Function: ql_ble_get_public_addr
*
* Description:
*   获取BLE公共地址(与BT mac地址一致)
* 
* Parameters:
*   public_addr     [out]   公共地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_get_public_addr(ql_bt_addr_s * public_addr);

/*****************************************************************
* Function: ql_ble_get_random_addr
*
* Description:
*   获取BLE随机地址
* 
* Parameters:
*   random_addr     [out]   随机地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_get_random_addr(ql_bt_addr_s *random_addr);

/*****************************************************************
* Function: ql_ble_add_public_whitelist
*
* Description:
*   增加一条白名单
* 
* Parameters:
*   public_addr     [in]    公共地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_add_public_whitelist(ql_bt_addr_s public_addr);

/*****************************************************************
* Function: ql_ble_add_random_whitelist
*
* Description:
*   增加一条白名单
* 
* Parameters:
*   random_addr     [in]    随机地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_add_random_whitelist(ql_bt_addr_s random_addr);


/*****************************************************************
* Function: ql_ble_get_whitelist_info
*
* Description:
*   获取白名单信息，白名单最多只有12个
* 
* Parameters:
*   whitelist_count         [in]    期望获得白名单个数
*   real_whitelist_count    [in]    实际获得白名单个数
*   whitelist               [in]    白名单信息
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_get_whitelist_info(unsigned char whitelist_count,unsigned char *real_whitelist_count,ql_ble_whitelist_info_s whitelist[]);

/*****************************************************************
* Function: ql_ble_remove_whitelist
*
* Description:
*   删除一条白名单
* 
* Parameters:
*   whitelist       [in]    白名单信息
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_remove_whitelist(ql_ble_whitelist_info_s whitelist);

/*****************************************************************
* Function: ql_ble_clean_whitelist
*
* Description:
*   删除所有的白名单
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_clean_whitelist();

/*****************************************************************
* Function: ql_ble_conncet_public_addr
*
* Description:
*   建立连接
* 
* Parameters:
*   public_addr     [in]    公共地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_conncet_public_addr(ql_bt_addr_s public_addr);

/*****************************************************************
* Function: ql_ble_conncet_random_addr
*
* Description:
*   建立连接
* 
* Parameters:
*   random_addr     [in]    随机地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_conncet_random_addr(ql_bt_addr_s random_addr);

/*****************************************************************
* Function: ql_ble_cancel_connect
*
* Description:
*   取消正在建立的连接
* 
* Parameters:
*   addr    [in]    建立连接时使用的地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_cancel_connect(ql_bt_addr_s addr);

/*****************************************************************
* Function: ql_ble_cancel_connect
*
* Description:
*   断开已建立的连接
* 
* Parameters:
*   conn_id     [in]    连接ID，建立连接时得到的连接ID
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_disconnect(unsigned short conn_id);

/*****************************************************************
* Function: ql_ble_get_connection_state
*
* Description:
*   获取指定地址的连接状态
* 
* Parameters:
*   addr        [in]   BLE地址
*   state       [out]  BLE地址
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_get_connection_state(ql_bt_addr_s addr, ql_ble_connection_state_e *state);

/*****************************************************************
* Function: ql_ble_update_conn_param
*
* Description:
*   更新连接参数
* 
* Parameters:
*   conn_param  [in]   连接参数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_update_conn_param(ql_ble_update_conn_infos_s conn_param);

/*****************************************************************
* Function: ql_ble_update_conn_param
*
* Description:
*   请求交互MTU，主机和从机都是取最小的MTU,最大为247
* 
* Parameters:
*   conn_id     [in]    连接ID，建立连接时得到的连接ID
*   mtu         [in]    MTU
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_exchage_mtu(unsigned short conn_id, unsigned short mtu);



//以下为BLE广播的API
/*****************************************************************
* Function: ql_bleadv_set_typical_addr_param
*
* Description:
*   设置定向广播参数
* 
* Parameters:
*   adv_param   [in]    定向广播参数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bleadv_set_typical_addr_param(ql_bleadv_typical_addr_param_s adv_param);

/*****************************************************************
* Function: ql_bleadv_get_typical_addr_param
*
* Description:
*   获取定向广播参数
* 
* Parameters:
*   adv_param   [out]   定向广播参数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bleadv_get_typical_addr_param(ql_bleadv_typical_addr_param_s *adv_param);

/*****************************************************************
* Function: ql_bleadv_set_param
*
* Description:
*   设置广播参数
* 
* Parameters:
*   adv_param   [in]   广播参数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bleadv_set_param(ql_bleadv_param_s adv_param);

/*****************************************************************
* Function: ql_bleadv_get_param
*
* Description:
*   获取广播参数
* 
* Parameters:
*   adv_param   [out]  广播参数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/

ql_errcode_bt_e ql_bleadv_get_param(ql_bleadv_param_s *adv_param);

/*****************************************************************
* Function: ql_bleadv_set_data
*
* Description:
*   设置广播数据，广播数据最长不超过31个字节
* 
* Parameters:
*   avd_data   [in]  广播数据
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bleadv_set_data(ql_bleadv_set_data_s avd_data);

/*****************************************************************
* Function: ql_bleadv_set_scan_rsp_data
*
* Description:
*   设置扫描回复数据，扫描回复数据最长不超过31个字节
* 
* Parameters:
*   avd_data   [in]  扫描回复数据，格式与广播数据一样，实际上也是广播出去的
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bleadv_set_scan_rsp_data(ql_bleadv_set_data_s adv_data);

/*****************************************************************
* Function: ql_ble_set_ibeacon_data
*
* Description:
*   设置ibeacon数据
* 
* Parameters:
*   uuid_l   [in]  16字节的长UUID
*   major    [in]  主要部分
*   minor    [in]  次要部分
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_set_ibeacon_data(unsigned char uuid_l[QL_BLE_LONG_UUID_SIZE], unsigned short major, unsigned short minor);

/*****************************************************************
* Function: ql_ble_write_ibeacon_cfg
*
* Description:
*   设置ibeacon数据到NVM中
* 
* Parameters:
*   info     [in]  ibeacon数据

*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_write_ibeacon_cfg(ql_ble_ibeacon_cfg_s info);

/*****************************************************************
* Function: ql_ble_read_ibeacon_cfg
*
* Description:
*   将ibeacon数据从NVM读取出来
* 
* Parameters:
*   info     [out]  ibeacon数据

*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_read_ibeacon_cfg(ql_ble_ibeacon_cfg_s *info);

/*****************************************************************
* Function: ql_bleadv_start
*
* Description:
*   开启广播
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bleadv_start();

/*****************************************************************
* Function: ql_bleadv_stop
*
* Description:
*   停止广播
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bleadv_stop();

/*****************************************************************
* Function: ql_bleadv_get_state
*
* Description:
*   获取广播状态
* 
* Parameters:
*   adv_state  [out]  广播状态
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bleadv_get_state(ql_ble_adv_state_e *adv_state);

/*****************************************************************
* Function: ql_ble_gatt_server_init
*
* Description:
*   初始化BLE GATT从机并注册回调函数
* 
* Parameters:
*   bt_cb  [out]  BLE GATT回调函数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_gatt_server_init(ql_bt_callback bt_cb);

/*****************************************************************
* Function: ql_ble_gatt_server_release
*
* Description:
*   BLE GATT从机释放资源
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_gatt_server_release();

/*****************************************************************
* Function: ql_ble_gatt_client_init
*
* Description:
*   初始化BLE GATT主机并注册回调函数
* 
* Parameters:
*   bt_cb  [out]  BLE GATT回调函数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_gatt_client_init(ql_bt_callback bt_cb);

/*****************************************************************
* Function: ql_ble_gatt_client_release
*
* Description:
*   BLE GATT主机释放资源
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_ble_gatt_client_release();


//以下为BLE扫描使用API
/*****************************************************************
* Function: ql_ble_gatt_client_init
*
* Description:
*   设置扫描参数
* 
* Parameters:
*   scan_param      [in]   扫描参数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_blescan_set_param(ql_blescan_scan_s scan_param);

/*****************************************************************
* Function: ql_ble_gatt_client_init
*
* Description:
*   获取扫描参数
* 
* Parameters:
*   scan_param      [out]  扫描参数
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_blescan_get_param(ql_blescan_scan_s *scan_param);

/*****************************************************************
* Function: ql_blescan_start
*
* Description:
*   开启扫描
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_blescan_start();

/*****************************************************************
* Function: ql_blescan_stop
*
* Description:
*   停止扫描
* 
* Parameters:
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_blescan_stop();

/*****************************************************************
* Function: ql_blescan_stop
*
* Description:
*   获取扫描状态
* 
* Parameters:
*   scan_state      [out]  扫描状态
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_blescan_get_state(ql_ble_scan_state_e *scan_state);



#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_API_BT_H */




