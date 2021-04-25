/**  @file
  quec_modem_interface.h

  @brief
  This file is used to define version information for different Quectel Project.

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


#ifndef QUEC_MODEM_INTERFACE_H
#define QUEC_MODEM_INTERFACE_H

#include "at_cfw.h"
#include "lwip/netif.h"
#include "quec_common.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_NETIF

/*========================================================================
 *  Macro Definition
 *========================================================================*/


/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
typedef enum 
{
    QUEC_NETIF_ACTION_NONE = 0,
    QUEC_NETIF_ACTION_BUSY,
    QUEC_NETIF_ACTION_MATCH_MODE,
    QUEC_NETIF_ACTION_TOCONNECT,
    QUEC_NETIF_ACTION_TOACTIVE,
    QUEC_NETIF_ACTION_MAX,
}quec_netif_action_e;


/*========================================================================
 *	Type Definition
 *========================================================================*/
typedef struct
{
    uint8_t nSim;
    uint8_t nCid;
    void (*quec_modem_exception_handler)(void);
}quec_modem_connect_ctx_s;


/*========================================================================
 *	Variable Definition
 *========================================================================*/


/*========================================================================
 *	Utilities Definition
 *========================================================================*/


/*========================================================================
 *	function Definition
 *========================================================================*/
uint8_t quec_check_netif_exist_or_available(uint8_t nSim, uint8_t nCid, quec_netif **wan_if, quec_netif **lwip_if, uint8_t dst_mode);
quec_netif *quec_getAvailableNetIfBySIMCID(uint8_t nSim, uint8_t nCid);
quec_netif *quec_get_NAT_wan_NetIf(uint8_t nSim, uint8_t nCid);


void quec_modem_attach_notification(uint8_t ps_status);
uint32_t quec_check_gprs_pdp_state(uint8_t nSim, uint8_t cid);
uint32_t quec_try_to_active_pdp_context_by_cid(quec_modem_connect_ctx_s *ctx);

#endif

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_MODEM_INTERFACE_H */




