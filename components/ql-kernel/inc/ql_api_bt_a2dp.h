/**  @file
  ql_api_bt_a2dp.h

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

#ifndef _QL_API_BT_A2DP_H
#define _QL_API_BT_A2DP_H

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
}ql_bt_a2dp_event_info_t;

/*===========================================================================
 * Enum
 ===========================================================================*/

typedef enum
{
    QUEC_BTA2DP_CONNECTION_STATE_DISCONNECTED = 0,
    QUEC_BTA2DP_CONNECTION_STATE_CONNECTING,
    QUEC_BTA2DP_CONNECTION_STATE_CONNECTED,
    QUEC_BTA2DP_CONNECTION_STATE_DISCONNECTING,
} quec_bta2dp_connection_state_e;

/*===========================================================================
 * Variate
 ===========================================================================*/
 
/*===========================================================================
 * Functions
 ===========================================================================*/

/*****************************************************************
* Function: ql_bt_a2dp_disconnect
*
* Description: disconnect the bt a2dp
* 
* Parameters:
*
* Return:
*
*****************************************************************/
ql_errcode_bt_e ql_bt_a2dp_disconnect(ql_bt_addr_s addr);

/*****************************************************************
* Function: ql_bt_a2dp_stop
*
* Description: get the addr of the client
* 
* Parameters:
* addr          [out]    the addr of the client 
*
* Return:
*   QL_BT_SUCCESS
*****************************************************************/
ql_errcode_bt_e ql_bt_a2dp_get_addr(ql_bt_addr_s *addr);

/*****************************************************************
* Function: ql_bt_a2dp_connection_state_get
*
* Description: get the connection state of the a2dp
* 
* Parameters:
*
* Return:
*   QUEC_BTA2DP_CONNECTION_STATE_DISCONNECTED
*   QUEC_BTA2DP_CONNECTION_STATE_CONNECTING
*   QUEC_BTA2DP_CONNECTION_STATE_CONNECTED
*   QUEC_BTA2DP_CONNECTION_STATE_DISCONNECTING
*****************************************************************/
quec_bta2dp_connection_state_e ql_bt_a2dp_connection_state_get(void);





#ifdef __cplusplus
    } /*"C" */
#endif
    
#endif /* _QL_API_BT_A2DP_H */

