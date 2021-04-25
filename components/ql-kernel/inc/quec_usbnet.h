/**  @file
  quec_usbnet.h

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



#ifndef QUEC_USBNET_H
#define QUEC_USBNET_H

#include "quec_modem_interface.h"
#include "quec_common.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_USBNET

#define QUEC_USBNET_DEBUG 0


/*========================================================================
 *  Macro Definition
 *========================================================================*/
#define QUEC_USBNET_TIMER_PERIOD_MIN 2
#define QUEC_USBNET_TIMER_PERIOD_MAX 300


/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
typedef enum 
{
    QUEC_USB_NET_NONE = -1,
    QUEC_USB_NET_RMNET = 0,
    QUEC_USB_NET_ECM,
    QUEC_USB_NET_MBIM,
    QUEC_USB_NET_RNDIS,
    QUEC_USB_NET_MAX
}quec_usb_net_type_e;
    
typedef enum 
{
    QUEC_USB_NET_CONNECT_DISABLE = 0,
    QUEC_USB_NET_CONNECT_MANAUAL = 1,
    QUEC_USB_NET_CONNECT_AUTO = 3,
    QUEC_USB_NET_CONNECT_MAX,
}quec_usb_net_connect_e;
    
typedef enum 
{
    QUEC_USB_NET_STATE_NONE = 0,
    QUEC_USB_NET_STATE_CONNECT,
    QUEC_USB_NET_STATE_START,
    QUEC_USB_NET_STATE_PORT_DISCONNECT,
    QUEC_USB_NET_STATE_ERROR,
    QUEC_USB_NET_STATE_MAX,
}quec_usb_net_state_e;
    
typedef enum 
{
    QUEC_USB_NET_MSG_NONE = 0,
    QUEC_USB_NET_MSG_MANAUAL,
    QUEC_USB_NET_MSG_AUTO_INIT,
    QUEC_USB_NET_MSG_AUTO_URC_SET,
    QUEC_USB_NET_MSG_AUTO_TIMER_START,
    QUEC_USB_NET_MSG_AUTO_TIMER_EXPIRED,
    QUEC_USB_NET_MSG_AUTO_TIMER_STOP,
    QUEC_USB_NET_MSG_DISABLE,//disable usbnet by at cmd
    QUEC_USB_NET_MSG_UP,
    QUEC_USB_NET_MSG_DOWN,
    QUEC_USB_NET_MSG_PORT_CONNECT,
    QUEC_USB_NET_MSG_PORT_DISCONNECT,
    QUEC_USB_NET_MSG_NETIF_SET,
    QUEC_USB_NET_MSG_PS_ATTACH,
    QUEC_USB_NET_MSG_MAX,
}quec_usb_net_msg_e;


/*========================================================================
 *	Type Definition
 *========================================================================*/
typedef struct
{
    quec_usb_net_connect_e connect_type;
    uint8_t connect_sim_cid;
    quec_enable_e connect_urc;
}quec_usb_net_dev_cfg_s;

typedef struct
{
    quec_usb_net_dev_cfg_s usbnetdevcfg;
    quec_usb_net_state_e connect_state;
    quec_netif *connect_netif;
    quec_netif *connect_wan_netif;
    uint16_t timer_period;//unit: s
    uint32_t retry_cnt;
}quec_usb_net_connect_ctx_s;

typedef struct
{
    uint32_t id;
    uint32_t nMsg;
    uint32_t nParam1;
    uint8_t nParam2;
    uint8_t nParam3;
    uint16_t nParam4;
}quec_usbnet_event_s;


/*========================================================================
*  Variable Definition
*========================================================================*/


/*========================================================================
 *	Utilities Definition
 *========================================================================*/


/*========================================================================
 *	function Definition
 *========================================================================*/
void quectel_exec_qcfg_usbnet_cmd(atCommand_t *cmd);
void quectel_exec_qdbgcfg_usbnet_cmd(atCommand_t *cmd);


void quec_netdev_init(void);
quec_netif *quec_usbnet_connect_netif_get(void);
quec_netif *quec_usbnet_wan_netif_get(void);
bool quec_usbnet_sendevent(uint32_t nMsg, uint32_t nParam1, uint8_t nParam2, uint8_t nParam3, uint16_t nParam4);
bool quec_usbnet_send_osi_event_set_if(quec_netif *net_if, quec_netif *wan_if);



#endif

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_USBNET_H */






