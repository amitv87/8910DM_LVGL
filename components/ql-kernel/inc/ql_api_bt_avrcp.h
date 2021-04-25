/**  @file
  ql_api_bt_avrcp.h

  @brief
  This file is used to define bt a2dp api for different Quectel Project.

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

#ifndef _QL_API_BT_AVRCP_H
#define _QL_API_BT_AVRCP_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * include files
 ===========================================================================*/

#include "ql_api_bt_common.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

/*===========================================================================
 * Struct
 ===========================================================================*/

typedef struct
{
    int state;                      //different from evnet type
    ql_bt_addr_s addr;              //remote address
}ql_bt_avrcp_event_info_t;

/*===========================================================================
 * Enum
 ===========================================================================*/

typedef enum
{
    QUEC_BTAVRCP_CONNECTION_STATE_DISCONNECTED = 0,     /**< Avrcp connection is disconnected */
    QUEC_BTAVRCP_CONNECTION_STATE_CONNECTING,	         /**< Avrcp connection is connecting */
    QUEC_BTAVRCP_CONNECTION_STATE_CONNECTED,                /**< Avrcp connection is connected */
    QUEC_BTAVRCP_CONNECTION_STATE_DISCONNECTING,         /**< Avrcp connection is disconnecting */
} quec_btavrcp_connection_state_t;

typedef enum
{
    QUEC_AVRCP_PLAYBACK_STATUS_STOPPED,                /**< The play is stopped */
    QUEC_AVRCP_PLAYBACK_STATUS_PLAYING,                 /**< The play is playing */
    QUEC_AVRCP_PLAYBACK_STATUS_PAUSED,                  /**< The play is paused */
    QUEC_AVRCP_PLAYBACK_STATUS_FWD_SEEK,              /**< The play is seeking forward */
    QUEC_AVRCP_PLAYBACK_STATUS_REV_SEEK,               /**< The play is seeking rewind */
    QUEC_AVRCP_PLAYBACK_STATUS_ERROR = 0xff          /**< Error */
} QUEC_AVRCP_PLAYBACK_STATUS_e;

/*===========================================================================
 * Variate
 ===========================================================================*/
 
/*===========================================================================
 * Functions
 ===========================================================================*/

/*****************************************************************
* Function: ql_bt_avrcp_start
*
* Description: start playing
* 
* Parameters:
*
* Return:
*   QL_BT_SUCCESS
*   QL_BT_AVRCP_START_ERR
*****************************************************************/
ql_errcode_bt_e ql_bt_avrcp_start(void);

/*****************************************************************
* Function: ql_bt_avrcp_pause
*
* Description: pause playing
* 
* Parameters:
*
* Return:
*   QL_BT_SUCCESS
*   QL_BT_AVRCP_PAUSE_ERR
*****************************************************************/
ql_errcode_bt_e ql_bt_avrcp_pause(void);

/*****************************************************************
* Function: ql_bt_avrcp_previ
*
* Description: play previous
* 
* Parameters:
*
* Return:
*   QL_BT_SUCCESS
*   QL_BT_AVRCP_PREVI_ERR
*****************************************************************/
ql_errcode_bt_e ql_bt_avrcp_previ(void);

/*****************************************************************
* Function: ql_bt_avrcp_next
*
* Description: play next
* 
* Parameters:
*
* Return:
*   QL_BT_SUCCESS
*   QL_BT_AVRCP_NEXT_ERR
*****************************************************************/
ql_errcode_bt_e ql_bt_avrcp_next(void);

/*****************************************************************
* Function: ql_bt_avrcp_vol_set
*
* Description: set the volume
* 
* Parameters:
*   vol         [in]    the volume you want to set
* Return:
*   QL_BT_SUCCESS
*   QL_BT_AVRCP_VOL_SET_ERR
*****************************************************************/
ql_errcode_bt_e ql_bt_avrcp_vol_set(uint8 vol);

/*****************************************************************
* Function: ql_bt_avrcp_vol_get
*
* Description: get the volume
* 
* Parameters:
*   *vol         [out]    the volume of the device
* Return:
*   QL_BT_SUCCESS
*   QL_BT_AVRCP_VOL_GET_ERR
*****************************************************************/
ql_errcode_bt_e ql_bt_avrcp_vol_get(uint8_t* vol);

/*****************************************************************
* Function: ql_bt_avrcp_play_state_get
*
* Description: get the play state
* 
* Parameters:
*   *ret         [out]    the play state of the device
* Return:
*   QL_BT_SUCCESS
*   QL_BT_AVRCP_PLAY_STATE_GET_ERR
*****************************************************************/
ql_errcode_bt_e ql_bt_avrcp_play_state_get(QUEC_AVRCP_PLAYBACK_STATUS_e *ret);

/*****************************************************************
* Function: ql_bt_avrcp_connection_state_get
*
* Description: get the connect state
* 
* Parameters:
*
* Return:
*   QUEC_BTAVRCP_CONNECTION_STATE_DISCONNECTED = 0
*   QUEC_BTAVRCP_CONNECTION_STATE_CONNECTING
*   QUEC_BTAVRCP_CONNECTION_STATE_CONNECTED
*   QUEC_BTAVRCP_CONNECTION_STATE_DISCONNECTING
*****************************************************************/
quec_btavrcp_connection_state_t ql_bt_avrcp_connection_state_get(void);




#ifdef __cplusplus
    } /*"C" */
#endif
    
#endif /* _QL_API_BT_AVRCP_H */

