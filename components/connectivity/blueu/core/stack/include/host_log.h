/**
 * @file     host_log.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef _HOST_LOG_H
#define _HOST_LOG_H

#include "bt_types.h"

#define LOG_VERBOSE 0x000000
#define LOG_DEBUG 0x400000
#define LOG_WARNING 0x800000
#define LOG_ERROR 0xC00000

//extern BOOL  osiDebugEvent(UINT32 event);

#ifndef CT_NO_DEBUG
//#define LOG_PRINT(value, low8)  OSI_LOGE(0, "blueu host_log=0x%x",  ((value) | ((low8) & 0xff)) )
#define LOG_PRINT(value, low8) //osiDebugEvent((value) | ((low8) & 0xff))
#define LOG_PRINT_L16(value, low16) //osiDebugEvent((value) | ((low16) & 0xffff))
#else
#define LOG_PRINT(value, low8) do{} while(0)
#endif

typedef enum
{
    TS_BOOT_MODE = 0x0100,
    TH_CMD_PAGE1 = 0x0200,
} TWS_SYS_LOG_T;

typedef enum
{
    TA_BT_ACTIVE_REQUEST = 0x0100,
} TWS_APP_LOG_T;

typedef enum
{
    TH_CALL_STATUS_IND_INVALID_PARAMTERS = 0x0100,
    TH_CALL_STEPUP = 0x0200,
    TH_CALL_STATUS = 0x0300,
    TH_CALL_HOLD = 0x0400,
    TH_HFG_CALL_STATUS_IND_ERROR = 0x0500,
    TH_HFG_AT_CMD_NORMAL = 0x0600,
    TH_HFG_AT_CMD_READ = 0x0700,
    TH_HFG_AT_CMD_TEST = 0x0800,
} TWS_HFP_LOG_T;

typedef enum
{
    UA_NOTIFI_CHANGED = 0x0100,
    UA_NOTIFI_CHANGED_ERROR = 0x0200,
} UWS_AVRCP_LOG_T;


typedef enum
{
    UG_SET_MODULE_STATE = 0x0100,
    UG_PACKAGE_MODULE = 0x0200,
    UG_UNPACKAGE_MODULE = 0x0300,
    UG_SYNC_INFO_ERROR = 0x0400,
    UG_SYNC_INFO_HANDLE = 0x0500,
    UG_SEND_PACKAGE = 0x0600,
    UG_RECV_PACKAGE = 0x0700,
    UG_UNPACKAGE_ERROR = 0x0800,
    UG_SEND_SYNC_INFO_ACK = 0x0900,
    UG_RECV_SYNC_INFO_ACK = 0x0a00,
    UG_PACKAGE_ERROR = 0x0b00,
    UG_CHECK_MUTILE_SYNC_INFO = 0x0c00,
    UG_SYNC_INFO_STATE = 0x0d00,
} UWS_GAMING_LOG_T;


typedef enum
{
    UPT_HCI_CMD_CREATE_CONNECTION = 0x00,
    UPT_HCI_EVT_CONNECTION_REQUEST = 0x01,
    UPT_HCI_EVT_CONNECTION_COMPLETE = 0x02,
    UPT_HCI_EVT_DISCONNECTION_COMPLETE = 0x03,
    UPT_HCI_CMD_OBSERVER_CREATE_CONNECTION = 0x04,
    UPT_HCI_EVT_OBSERVER_CONNECTION_PROCESSING = 0x05,
    UPT_HCI_EVT_OBSERVER_DISCONNECTION_COMPLETE = 0x06,
    UPT_HCI_EVT_OBSERVER_CONNECTION_COMPLETE = 0x07,
    UPT_HCI_EVT_SYNCHRONOUS_CONNECTION_COMPLETE = 0x08,
    UPT_HCI_EVT_OBSERVER_SYNCHRONOUS_CONNECTION_COMPLETE = 0x09,
    UPT_HCI_EVT_OBSERVER_SYNCHRONOUS_DISCONNECTION_COMPLETE = 0x0a,
    UPT_HCI_EVT_OBSERVER_ROLE_SWITCH = 0x0b,
    UPT_HCI_EVT_ACL_SCHEDULE_FREEZE = 0x0c,
    UPT_HCI_CMD_SUBRATING_SINFF_REDUCE = 0x0d,
    UPT_HCI_CMD_OBSERVER_ROLE_SWITCH = 0x0e,
    UPT_HCI_CMD_SET_DEVICE_INFORMATION = 0x0f,
    UPT_HCI_CMD_ACL_SCHEDULE_FREEZE = 0x10,
    UPT_HCI_CMD_SET_LOCAL_ADDRESS = 0x11,
} UWSPT_HCI_T;

typedef enum
{
    LINK_TYPE_SCO = 0,
    LINK_TYPE_ACL,
} UWSPT_LINK_TYPE_T;


typedef enum
{
    UPT_ENCODE_AVDTP_INFO = 0x00,
    UPT_ENCODE_AVDTP_INFO_WARNING = 0x01,
    UPT_ENCODE_AVDTP_INFO_FAILED = 0x02,
    UPT_ENCODE_AVDTP_INFO_SUCCESS = 0x03,
    UPT_DECODE_AVDTP_INFO = 0x04,
    UPT_DECODE_AVDTP_INFO_WARNING = 0x05,
    UPT_DECODE_AVDTP_INFO_FAILED = 0x06,
    UPT_DECODE_AVDTP_INFO_SUCCESS = 0x07,

    UPT_ENCODE_AVDTP_STATE = 0x08,
    UPT_ENCODE_AVDTP_STATE_WARNING = 0x09,
    UPT_ENCODE_AVDTP_STATE_FAILED = 0x0a,
    UPT_ENCODE_AVDTP_STATE_SUCCESS = 0x0b,
    UPT_DECODE_AVDTP_STATE = 0x0c,
    UPT_DECODE_AVDTP_STATE_WARNING = 0x0d,
    UPT_DECODE_AVDTP_STATE_FAILED = 0x0e,
    UPT_DECODE_AVDTP_STATE_SUCCESS = 0x0f,

    UPT_ENCODE_HFP_INFO = 0x10,
    UPT_ENCODE_HFP_INFO_WARNING = 0x11,
    UPT_ENCODE_HFP_INFO_FAILED = 0x12,
    UPT_ENCODE_HFP_INFO_SUCCESS = 0x13,
    UPT_DECODE_HFP_INFO = 0x14,
    UPT_DECODE_HFP_INFO_WARNING = 0x15,
    UPT_DECODE_HFP_INFO_FAILED = 0x16,
    UPT_DECODE_HFP_INFO_SUCCESS = 0x17,

    UPT_ENCODE_HFP_STATE = 0x18,
    UPT_ENCODE_HFP_STATE_WARNING = 0x19,
    UPT_ENCODE_HFP_STATE_FAILED = 0x1a,
    UPT_ENCODE_HFP_STATE_SUCCESS = 0x1b,
    UPT_DECODE_HFP_STATE = 0x1c,
    UPT_DECODE_HFP_STATE_WARNING = 0x1d,
    UPT_DECODE_HFP_STATE_FAILED = 0x1e,
    UPT_DECODE_HFP_STATE_SUCCESS = 0x1f,

    UPT_ENCODE_SPP_PACKAGE = 0x20,
    UPT_ENCODE_SPP_PACKAGE_FAILED = 0x21,
    UPT_ENCODE_SPP_PACKAGE_SUCCESS = 0x22,
    UPT_DECODE_SPP_PACKAGE = 0x23,
    UPT_DECODE_SPP_PACKAGE_FAILED = 0x24,
    UPT_DECODE_SPP_PACKAGE_SUCCESS = 0x25,

    UPT_UPDATE_APP_HFP_INFO = 0x26,
    UPT_UPDATE_APP_HFP_INFO_FAILED = 0x27,
    UPT_UPDATE_APP_HFP_INFO_SUCCESS = 0x28,
    UPT_UPDATE_APP_HFP_STATUS = 0x29,
    UPT_UPDATE_APP_HFP_STATUS_FAILED = 0x2a,
    UPT_UPDATE_APP_HFP_STATUS_SUCCESS = 0x2b,

    UPT_SET_AVDTP_INFO = 0x2c,
    UPT_SET_AVDTP_INFO_WARNING = 0x2d,
    UPT_SET_AVDTP_INFO_FAILED = 0x2e,
    UPT_SET_AVDTP_INFO_SUCCESS = 0x2f,

    UPT_SET_AVTDP_STATE = 0x30,
    UPT_SET_AVDTP_STATE_WARNING = 0x31,
    UPT_SET_AVDTP_STATE_FAILED = 0x32,
    UPT_SET_AVDTP_STATE_SUCCESS = 0x33,

    UPT_SET_HFP_INFO = 0x34,
    UPT_SET_HFP_INFO_WARNING = 0x35,
    UPT_SET_HFP_INFO_FAILED = 0x36,
    UPT_SET_HFP_INFO_SUCCESS = 0x37,

    UPT_SET_HFP_STATE = 0x38,
    UPT_SET_HFP_STATE_WARNING = 0x39,
    UPT_SET_HFP_STATE_FAILED = 0x3a,
    UPT_SET_HFP_STATE_SUCCESS = 0x3b,

    UPT_ENCODE_AVRCP_INFO = 0x3c,
    UPT_ENCODE_AVRCP_INFO_WARNING = 0x3d,
    UPT_ENCODE_AVRCP_INFO_FAILED = 0x3e,
    UPT_ENCODE_AVRCP_INFO_SUCCESS = 0x3f,
    UPT_DECODE_AVRCP_INFO = 0x40,
    UPT_DECODE_AVRCP_INFO_WARNING = 0x41,
    UPT_DECODE_AVRCP_INFO_FAILED = 0x42,
    UPT_DECODE_AVRCP_INFO_SUCCESS = 0x43,

    UPT_ENCODE_AVRCP_STATE = 0x44,
    UPT_ENCODE_AVRCP_STATE_WARNING = 0x45,
    UPT_ENCODE_AVRCP_STATE_FAILED = 0x46,
    UPT_ENCODE_AVRCP_STATE_SUCCESS = 0x47,
    UPT_DECODE_AVRCP_STATE = 0x48,
    UPT_DECODE_AVRCP_STATE_WARNING = 0x49,
    UPT_DECODE_AVRCP_STATE_FAILED = 0x4a,
    UPT_DECODE_AVRCP_STATE_SUCCESS = 0x4b,

} UWSPT_SYNC_EVENT_T;

#define UWSPT_HCI_EVENT(opcode, type, code) \
    BTINFO(0xe5310000                       \
           | ((uint16)opcode & 0xFF) << 8      \
           | ((uint16)type & 0x01) << 7        \
           | (code & 0x7F));

#define UWSPT_SYNC_EVENT(code) \
    BTINFO(0xe5320000 | code)
#if 0
#define L2CAP_LOGV(value, low8) LOG_PRINT(0xea200000 | ((value&0xFF)<<8), (low8))
#define L2CAP_LOGD(value, low8) LOG_PRINT(0xea240000 | ((value&0xFF)<<8), (low8))
#define L2CAP_LOGW(value, low8) LOG_PRINT(0xea280000 | ((value&0xFF)<<8), (low8))
#define L2CAP_LOGE(value, low8) LOG_PRINT(0xea2c0000 | ((value&0xFF)<<8), (low8))

#define RFCOM_LOGV(value, low8) LOG_PRINT(0xea300000 | ((value&0xFF)<<8), (low8))
#define RFCOM_LOGD(value, low8) LOG_PRINT(0xea340000 | ((value&0xFF)<<8), (low8))
#define RFCOM_LOGW(value, low8) LOG_PRINT(0xea380000 | ((value&0xFF)<<8), (low8))
#define RFCOM_LOGE(value, low8) LOG_PRINT(0xea3c0000 | ((value&0xFF)<<8), (low8))

#define SDP_LOGV(value, low8) LOG_PRINT(0xea400000 | ((value&0xFF)<<8), (low8))
#define SDP_LOGD(value, low8) LOG_PRINT(0xea440000 | ((value&0xFF)<<8), (low8))
#define SDP_LOGW(value, low8) LOG_PRINT(0xea480000 | ((value&0xFF)<<8), (low8))
#define SDP_LOGE(value, low8) LOG_PRINT(0xea4c0000 | ((value&0xFF)<<8), (low8))

#define GAP_LOGV(value, low8) LOG_PRINT(0xea500000 | ((value&0xFF)<<8), (low8))
#define GAP_LOGD(value, low8) LOG_PRINT(0xea540000 | ((value&0xFF)<<8), (low8))
#define GAP_LOGW(value, low8) LOG_PRINT(0xea580000 | ((value&0xFF)<<8), (low8))
#define GAP_LOGE(value, low8) LOG_PRINT(0xea5c0000 | ((value&0xFF)<<8), (low8))

#define AVDTP_LOGV(value, low8) LOG_PRINT(0xea600000 | ((value&0xFF)<<8), (low8))
#define AVDTP_LOGD(value, low8) LOG_PRINT(0xea640000 | ((value&0xFF)<<8), (low8))
#define AVDTP_LOGW(value, low8) LOG_PRINT(0xea680000 | ((value&0xFF)<<8), (low8))
#define AVDTP_LOGE(value, low8) LOG_PRINT(0xea6c0000 | ((value&0xFF)<<8), (low8))

#define AVCTP_LOGV(value, low8) LOG_PRINT(0xea700000 | ((value&0xFF)<<8), (low8))
#define AVCTP_LOGD(value, low8) LOG_PRINT(0xea740000 | ((value&0xFF)<<8), (low8))
#define AVCTP_LOGW(value, low8) LOG_PRINT(0xea780000 | ((value&0xFF)<<8), (low8))
#define AVCTP_LOGE(value, low8) LOG_PRINT(0xea7c0000 | ((value&0xFF)<<8), (low8))

#define SMP_LOGV(value, low8) LOG_PRINT(0xea800000 | ((value&0xFF)<<8), (low8))
#define SMP_LOGD(value, low8) LOG_PRINT(0xea840000 | ((value&0xFF)<<8), (low8))
#define SMP_LOGW(value, low8) LOG_PRINT(0xea880000 | ((value&0xFF)<<8), (low8))
#define SMP_LOGE(value, low8) LOG_PRINT(0xea8c0000 | ((value&0xFF)<<8), (low8))

#define A2DP_LOGV(value, low8) LOG_PRINT(0xea900000 | ((value&0xFF)<<8), (low8))
#define A2DP_LOGD(value, low8) LOG_PRINT(0xea940000 | ((value&0xFF)<<8), (low8))
#define A2DP_LOGW(value, low8) LOG_PRINT(0xea980000 | ((value&0xFF)<<8), (low8))
#define A2DP_LOGE(value, low8) LOG_PRINT(0xea9c0000 | ((value&0xFF)<<8), (low8))

#define AVRCP_LOGV(value, low8) LOG_PRINT(0xeaa00000 | ((value&0xFF)<<8), (low8))
#define AVRCP_LOGD(value, low8) LOG_PRINT(0xeaa40000 | ((value&0xFF)<<8), (low8))
#define AVRCP_LOGW(value, low8) LOG_PRINT(0xeaa80000 | ((value&0xFF)<<8), (low8))
#define AVRCP_LOGE(value, low8) LOG_PRINT(0xeaac0000 | ((value&0xFF)<<8), (low8))

#define HFP_LOGV(value, low8) LOG_PRINT(0xeab00000 | ((value&0xFF)<<8), (low8))
#define HFP_LOGD(value, low8) LOG_PRINT(0xeab40000 | ((value&0xFF)<<8), (low8))
#define HFP_LOGW(value, low8) LOG_PRINT(0xeab80000 | ((value&0xFF)<<8), (low8))
#define HFP_LOGE(value, low8) LOG_PRINT(0xeabc0000 | ((value&0xFF)<<8), (low8))

#define TWS_SYS_LOGV(value, low8) LOG_PRINT(0xeb100000 | ((value&0xFF)<<8), (low8))
#define TWS_SYS_LOGD(value, low8) LOG_PRINT(0xeb140000 | ((value&0xFF)<<8), (low8))
#define TWS_SYS_LOGW(value, low8) LOG_PRINT(0xeb180000 | ((value&0xFF)<<8), (low8))
#define TWS_SYS_LOGE(value, low8) LOG_PRINT(0xeb1c0000 | ((value&0xFF)<<8), (low8))

#define TWS_AUD_LOGV(value, low8) LOG_PRINT(0xeb200000 | ((value&0xFF)<<8), (low8))
#define TWS_AUD_LOGD(value, low8) LOG_PRINT(0xeb240000 | ((value&0xFF)<<8), (low8))
#define TWS_AUD_LOGW(value, low8) LOG_PRINT(0xeb280000 | ((value&0xFF)<<8), (low8))
#define TWS_AUD_LOGE(value, low8) LOG_PRINT(0xeb2c0000 | ((value&0xFF)<<8), (low8))

#define TWS_POT_LOGV(value, low8) LOG_PRINT(0xeb300000 | ((value&0xFF)<<8), (low8))
#define TWS_POT_LOGD(value, low8) LOG_PRINT(0xeb340000 | ((value&0xFF)<<8), (low8))
#define TWS_POT_LOGW(value, low8) LOG_PRINT(0xeb380000 | ((value&0xFF)<<8), (low8))
#define TWS_POT_LOGE(value, low8) LOG_PRINT(0xeb3c0000 | ((value&0xFF)<<8), (low8))

#define TWS_APP_LOGV(value, low8) LOG_PRINT(0xeb400000 | ((value&0xFF)<<8), (low8))
#define TWS_APP_LOGD(value, low8) LOG_PRINT(0xeb440000 | ((value&0xFF)<<8), (low8))
#define TWS_APP_LOGW(value, low8) LOG_PRINT(0xeb480000 | ((value&0xFF)<<8), (low8))
#define TWS_APP_LOGE(value, low8) LOG_PRINT(0xeb4c0000 | ((value&0xFF)<<8), (low8))

#define UWS_GAMING_LOGV(value, low8) LOG_PRINT(0xeb500000 | ((value&0xFF)<<8), (low8))
#define UWS_GAMING_LOGD(value, low8) LOG_PRINT(0xeb540000 | ((value&0xFF)<<8), (low8))
#define UWS_GAMING_LOGW(value, low8) LOG_PRINT(0xeb580000 | ((value&0xFF)<<8), (low8))
#define UWS_GAMING_LOGE(value, low8) LOG_PRINT(0xeb5c0000 | ((value&0xFF)<<8), (low8))

#define MAIN_LOGV(value, low8) LOG_PRINT(0xeb600000 | ((value&0xFF)<<8), (low8))
#define MAIN_LOGD(value, low8) LOG_PRINT(0xeb640000 | ((value&0xFF)<<8), (low8))
#define MAIN_LOGW(value, low8) LOG_PRINT(0xeb680000 | ((value&0xFF)<<8), (low8))
#define MAIN_LOGE(value, low8) LOG_PRINT(0xeb6c0000 | ((value&0xFF)<<8), (low8))

#define UKI_LOGV(value, low8) LOG_PRINT(0xeb700000 | ((value&0xFF)<<8), (low8))
#define UKI_LOGD(value, low8) LOG_PRINT(0xeb740000 | ((value&0xFF)<<8), (low8))
#define UKI_LOGW(value, low8) LOG_PRINT(0xeb780000 | ((value&0xFF)<<8), (low8))
#define UKI_LOGE(value, low8) LOG_PRINT(0xeb7c0000 | ((value&0xFF)<<8), (low8))

#define PAYLOAD_LOGV(low16) LOG_PRINT_L16(0xbdb00000 , (low16))
#define PAYLOAD_LOGD(low16) LOG_PRINT_L16(0xbdb40000 , (low16))
#define PAYLOAD_LOGW(low16) LOG_PRINT_L16(0xbdb80000 , (low16))
#define PAYLOAD_LOGE(low16) LOG_PRINT_L16(0xbdbc0000 , (low16))
#else
#define L2CAP_LOGV(value, low8) 
#define L2CAP_LOGD(value, low8) 
#define L2CAP_LOGW(value, low8) 
#define L2CAP_LOGE(value, low8) 

#define RFCOM_LOGV(value, low8) 
#define RFCOM_LOGD(value, low8) 
#define RFCOM_LOGW(value, low8) 
#define RFCOM_LOGE(value, low8) 

#define SDP_LOGV(value, low8) 
#define SDP_LOGD(value, low8) 
#define SDP_LOGW(value, low8) 
#define SDP_LOGE(value, low8) 

#define GAP_LOGV(value, low8) 
#define GAP_LOGD(value, low8) 
#define GAP_LOGW(value, low8) 
#define GAP_LOGE(value, low8) 

#define AVDTP_LOGV(value, low8) 
#define AVDTP_LOGD(value, low8) 
#define AVDTP_LOGW(value, low8) 
#define AVDTP_LOGE(value, low8) 

#define AVCTP_LOGV(value, low8) 
#define AVCTP_LOGD(value, low8) 
#define AVCTP_LOGW(value, low8) 
#define AVCTP_LOGE(value, low8) 

#define SMP_LOGV(value, low8) 
#define SMP_LOGD(value, low8) 
#define SMP_LOGW(value, low8) 
#define SMP_LOGE(value, low8) 

#define A2DP_LOGV(value, low8) 
#define A2DP_LOGD(value, low8) 
#define A2DP_LOGW(value, low8) 
#define A2DP_LOGE(value, low8) 

#define AVRCP_LOGV(value, low8) 
#define AVRCP_LOGD(value, low8) 
#define AVRCP_LOGW(value, low8) 
#define AVRCP_LOGE(value, low8) 

#define HFP_LOGV(value, low8) 
#define HFP_LOGD(value, low8) 
#define HFP_LOGW(value, low8) 
#define HFP_LOGE(value, low8) 

#define TWS_SYS_LOGV(value, low8) 
#define TWS_SYS_LOGD(value, low8) 
#define TWS_SYS_LOGW(value, low8) 
#define TWS_SYS_LOGE(value, low8) 

#define TWS_AUD_LOGV(value, low8) 
#define TWS_AUD_LOGD(value, low8) 
#define TWS_AUD_LOGW(value, low8) 
#define TWS_AUD_LOGE(value, low8) 

#define TWS_POT_LOGV(value, low8) 
#define TWS_POT_LOGD(value, low8) 
#define TWS_POT_LOGW(value, low8) 
#define TWS_POT_LOGE(value, low8) 

#define TWS_APP_LOGV(value, low8) 
#define TWS_APP_LOGD(value, low8) 
#define TWS_APP_LOGW(value, low8)
#define TWS_APP_LOGE(value, low8) 

#define UWS_GAMING_LOGV(value, low8) 
#define UWS_GAMING_LOGD(value, low8) 
#define UWS_GAMING_LOGW(value, low8) 
#define UWS_GAMING_LOGE(value, low8) 

#define MAIN_LOGV(value, low8) 
#define MAIN_LOGD(value, low8) 
#define MAIN_LOGW(value, low8) 
#define MAIN_LOGE(value, low8) 

#define UKI_LOGV(value, low8) 
#define UKI_LOGD(value, low8) 
#define UKI_LOGW(value, low8) 
#define UKI_LOGE(value, low8) 

#define PAYLOAD_LOGV(low16) 
#define PAYLOAD_LOGD(low16) 
#define PAYLOAD_LOGW(low16) 
#define PAYLOAD_LOGE(low16) 

#endif

#endif // _HOST_LOG_H

