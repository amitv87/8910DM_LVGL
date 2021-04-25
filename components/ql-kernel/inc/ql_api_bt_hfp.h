/**  @file
  ql_api_bt_hfp.h

  @brief
  This file is used to define bt hfp api for different Quectel Project.

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


#ifndef QL_API_BT_HFP_H
#define QL_API_BT_HFP_H
#include "ql_api_bt_common.h"
/*========================================================================
 *  Variable Definition
 *========================================================================*/
typedef enum
{
    QL_BT_HFP_CONNECTION_STATE_DISCONNECTED = 0,    //disconnect
    QL_BT_HFP_CONNECTION_STATE_CONNECTING,          //now is connecting
    QL_BT_HFP_CONNECTION_STATE_CONNECTED,           //connectecd
    QL_BT_HFP_CONNECTION_STATE_DISCONNECTING,       //now is disnonecting
}ql_bt_hfp_connection_state_e;

/**Hfp audio state enumeration*/
typedef enum
{
    QL_BT_HFP_AUDIO_STATE_DISCONNECTED = 0,         /**< Audio is disconnected */
    QL_BT_HFP_AUDIO_STATE_CONNECTING,               /**< Audio is connecting */
    QL_BT_HFP_AUDIO_STATE_CONNECTED,                /**< Audio is connected */
    QL_BT_HFP_AUDIO_STATE_DISCONNECTING             /**< Audio is disconnecting */
}ql_bt_hfp_audio_state_e;

/**Hfp volume type enumeration*/
typedef enum
{
    QL_BT_HFP_VOLUME_TYPE_SPK = 0,                  /**< Volume type is speaker */
    QL_BT_HFP_VOLUME_TYPE_MIC                       /**< Volume type is microphone */
}ql_bt_hfp_volume_type_e;

/**Hfp call state enumeration*/
typedef enum
{
    QL_BT_HFP_CALL_NO_CALL_IN_PROGRESS = 0,         /**<  No calls is in progress */
    QL_BT_HFP_CALL_CALL_IN_PROGRESS,                /**<  At least one call is in progress */
}ql_bt_hfp_call_state_e;

/**Hfp callsetup state enumeration*/
typedef enum
{
    QL_BT_HFP_CALLSETUP_NONE = 0,                   /**<  Not currently in call set up */
    QL_BT_HFP_CALLSETUP_INCOMING,                   /**<  An incoming call process ongoing */
    QL_BT_HFP_CALLSETUP_OUTGOING,                   /**<  An outcoming call set up is ongoing */
    QL_BT_HFP_CALLSETUP_ALERTING                    /**<  Remote party being alerted in an outgoing call */
}ql_bt_hfp_callsetup_state_e;

/**Hfp callheld state enumeration*/
typedef enum
{
    QL_BT_HFP_CALLHELD_NONE = 0,                    /**<  No call held */
    QL_BT_HFP_CALLHELD_HOLD_AND_ACTIVE,             /**<  Call is placed on held or active/held calls swapped */
    QL_BT_HFP_CALLHELD_HOLD,                        /**<  Call on hold,no active call */
}ql_bt_hfp_callheld_state_e;

/**Hfp network state enumeration*/
typedef enum
{
    QL_BT_HFP_NETWORK_STATE_NOT_AVAILABLE = 0,      /**< Network is not available */
    QL_BT_HFP_NETWORK_STATE_AVAILABLE,              /**< Network is available */
}ql_bt_hfp_network_state_e;

/**Hfp service type enumeration*/
typedef enum
{
    QL_BT_HFP_SERVICE_TYPE_HOME = 0,                /**< Service type is home */
    QL_BT_HFP_SERVICE_TYPE_ROAMING,                 /**< Service type is roaming */
}ql_bt_hfp_service_type_e;

/**Hfp codec type enumeration*/
typedef enum
{
    QL_BT_HFP_CODEC_TYPE_CVSD = 1,                           /**< Codec type is CVSD */
    QL_BT_HFP_CODEC_TYPE_MSBC                                 /**< Codec type is mSBC */
}ql_bt_hfp_codec_type_e;

typedef struct
{
    int state;                      //different from evnet type
    ql_bt_addr_s addr;              //remote address
}ql_bt_hfp_event_info_t;


/*========================================================================
 *  function Definition
 *========================================================================*/
/*****************************************************************
* Function: ql_bt_hfp_connect
*
* Description:
*   连接HFP
* 
* Parameters:
*   addr            [in]    AG地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_connect(ql_bt_addr_s addr);

/*****************************************************************
* Function: ql_bt_hfp_connect
*
* Description:
*   断开HFP连接
* 
* Parameters:
*   addr            [in]    建立连接的AG地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_disconnect(ql_bt_addr_s addr);

/*****************************************************************
* Function: ql_bt_hfp_set_volume
*
* Description:
*   设置通话语音
* 
* Parameters:
*   addr            [in]    建立连接的AG地址
*   vol             [in]    音量大小，范围1-15
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_set_volume(ql_bt_addr_s addr, unsigned char vol);

/*****************************************************************
* Function: ql_bt_hfp_reject_call
*
* Description:
*   接通电话后挂机
* 
* Parameters:
*   addr            [in]    建立连接的AG地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_reject_call(ql_bt_addr_s addr);

/*****************************************************************
* Function: ql_bt_hfp_answer_accept_call
*
* Description:
*   接通电话
* 
* Parameters:
*   addr            [in]    建立连接的AG地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_answer_accept_call(ql_bt_addr_s addr);

/*****************************************************************
* Function: ql_bt_hfp_answer_reject_call
*
* Description:
*   拒接电话
* 
* Parameters:
*   addr            [in]    建立连接的AG地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_answer_reject_call(ql_bt_addr_s addr);

/*****************************************************************
* Function: ql_bt_hfp_dial
*
* Description:
*   拨打电话
* 
* Parameters:
*   addr            [in]    建立连接的AG地址
*   phone_number    [in]    电话号码
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_dial(ql_bt_addr_s addr,unsigned char *phone_number);

/*****************************************************************
* Function: ql_bt_hfp_redial
*
* Description:
*   重复拨号
* 
* Parameters:
*   addr            [in]    建立连接的AG地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_redial(ql_bt_addr_s addr);

/*****************************************************************
* Function: ql_bt_hfp_vr_enable
*
* Description:
*   开启AG语音助手
* 
* Parameters:
*   addr            [in]    建立连接的AG地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_vr_enable(ql_bt_addr_s addr);

/*****************************************************************
* Function: ql_bt_hfp_vr_disable
*
* Description:
*   关闭AG语音助手
* 
* Parameters:
*   addr            [in]    建立连接的AG地址
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_vr_disable(ql_bt_addr_s addr);

/*****************************************************************
* Function: ql_bt_hfp_ctrl_three_way_call
*
* Description:
*   控制三方通话
* 
* Parameters:
*   addr            [in]    建立连接的AG地址
*   cmd             [in]    三方通话的控制命令
*
* Return:ql_errcode_bt_e
*
*****************************************************************/
ql_errcode_bt_e ql_bt_hfp_ctrl_three_way_call(ql_bt_addr_s addr, unsigned char cmd);

#endif /* QL_API_BT_HFP_H */

