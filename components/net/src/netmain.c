/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/
#include "osi_api.h"
#include "osi_log.h"
#include "cfw_dispatch.h"
#include "cfw.h"
#include "cfw_event.h"
#include "tcpip.h"
#include "lwipopts.h"
#include "netmain.h"
#include "ppp_interface.h"
#include "netdev_interface.h"
#include "sockets.h"
#if IP_NAT
#include "lwip/ip4_nat.h"
#endif

#include "quec_usbnet.h"
#include "quec_led_task.h"
#include "ql_datacall_internal.h"


osiThread_t *netThreadID = NULL;

extern void tcpip_thread(void *arg);
extern void dm_register();

#ifdef CONFIG_SOC_8910

#ifndef CONFIG_QUEC_PROJECT_FEATURE //not suitable for quectel
extern int vnet4gSelfRegister(uint8_t nCid, uint8_t nSimId);
#endif
#endif

#ifdef CONFIG_SOC_8910
#define NET_STACK_SIZE 8192 * 4
#else
#define NET_STACK_SIZE 8192 * 2
#endif

extern struct netif *TCPIP_nat_lan_lwip_netif_create(uint8_t nCid, uint8_t nSimId);
#if IP_NAT_INTERNAL_FORWARD
extern struct lan_netif *newLan(uint8_t nSimId, uint8_t nCid, uint8_t nIdx);
extern struct wan_netif *newWan(uint8_t nSimId, uint8_t nCid);
extern void destroyWan(uint8_t nSimId, uint8_t nCid);
#else
extern void TCPIP_nat_lan_lwip_netif_destory(uint8_t nCid, uint8_t nSimId);
extern struct netif *TCPIP_nat_wan_netif_create(uint8_t nCid, uint8_t nSimId);
extern void TCPIP_nat_wan_netif_destory(uint8_t nCid, uint8_t nSimId);
#endif
static void net_thread(void *arg)
{

    for (;;)
    {
        osiEvent_t event = {};
        osiEventWait(osiThreadCurrent(), &event);
        if (event.id == 0)
            continue;
        OSI_LOGI(0x1000752b, "Netthread get a event: 0x%08x/0x%08x/0x%08x/0x%08x", (unsigned int)event.id, (unsigned int)event.param1, (unsigned int)event.param2, (unsigned int)event.param3);

        OSI_LOGI(0x1000752c, "Netthread switch");
        if ((!cfwIsCfwIndicate(event.id)) && (cfwInvokeUtiCallback(&event)))
        {
            ; // handled by UTI
        }
        else
        {
            CFW_EVENT *cfw_event = (CFW_EVENT *)&event;
            switch (event.id)
            {
            case EV_INTER_APS_TCPIP_REQ:
            {
                struct tcpip_msg *msg;
                msg = (struct tcpip_msg *)event.param1;
                tcpip_thread(msg);
                break;
            }
            case EV_TCPIP_CFW_GPRS_ACT:
            {
                uint8_t nCid, nSimId;
#ifdef	CONFIG_QUEC_PROJECT_FEATURE
				osiSemaphore_t *wait_sema = (osiSemaphore_t *)(cfw_event->nParam2);
#endif				
                nCid = event.param1;
                nSimId = cfw_event->nFlag;

#if IP_NAT
                if (get_nat_enabled(nSimId, nCid))
                {
#if !IP_NAT_INTERNAL_FORWARD
                    TCPIP_nat_wan_netif_create(nCid, nSimId);
                    TCPIP_nat_lan_lwip_netif_create(nCid, nSimId);
#else
                    newWan(nSimId, nCid);
#endif
                }
                else
                {
#endif
                    TCPIP_netif_create(nCid, nSimId);
#if IP_NAT
                }
#endif
#ifdef	CONFIG_QUEC_PROJECT_FEATURE
				if(wait_sema != NULL){
					osiSemaphoreRelease(wait_sema);
				}
#endif

#ifdef CONFIG_SOC_8910
#if !IP_NAT_INTERNAL_FORWARD
                if (netdevIsConnected())
                {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_USBNET
                    quec_usbnet_sendevent(QUEC_USB_NET_MSG_UP, nSimId, nCid, 0, 0);
#else
                    netdevNetUp();
#endif
                }
#endif

#ifndef CONFIG_QUEC_PROJECT_FEATURE //not suitable for quectel
                vnet4gSelfRegister(nCid, nSimId); //dianxin4G×Ô×¢²á
#endif
#endif
#ifdef CONFIG_AT_DM_LWM2M_SUPPORT
                dm_register();
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE
				quec_set_datacall_status(nSimId, nCid, QL_DATACALL_ACTIVED);
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG
                quec_netlight_deal_pdp_change(1);
#endif
                break;
            }
            case EV_TCPIP_CFW_GPRS_DEACT:
            {
                uint8_t nCid, nSimId;
#ifdef	CONFIG_QUEC_PROJECT_FEATURE_TCPIP
				osiSemaphore_t *wait_sema = (osiSemaphore_t *)(cfw_event->nParam2);
#endif					
                nCid = event.param1;
                nSimId = cfw_event->nFlag;
#ifdef CONFIG_SOC_8910
#if !IP_NAT_INTERNAL_FORWARD
                if (netdevIsConnected())
                {
                    netdevNetDown(nSimId, nCid);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_USBNET
                    quec_usbnet_sendevent(QUEC_USB_NET_MSG_DOWN, nSimId, nCid, 0, 0);
#endif
                }
#endif
#endif

#if IP_NAT
                if (get_nat_enabled(nSimId, nCid))
                {
#if IP_NAT_INTERNAL_FORWARD
                    destroyWan(nSimId, nCid);
#else
                    TCPIP_nat_lan_lwip_netif_destory(nCid, nSimId);
                    TCPIP_nat_wan_netif_destory(nCid, nSimId);
#endif
                }
                else
                {
#endif
                    TCPIP_netif_destory(nCid, nSimId);
#if IP_NAT
                }
#endif
#ifdef	CONFIG_QUEC_PROJECT_FEATURE_TCPIP	
				{
					//OSI_PRINTFI("cb:%p",cb);
					if(wait_sema != NULL){
						osiSemaphoreRelease(wait_sema);
					}
				}
				
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE
				quec_set_datacall_status(nSimId, nCid, QL_DATACALL_IDLE);
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG
                quec_netlight_deal_pdp_change(0);
#endif
                break;
            }
#ifdef CONFIG_SOC_8910
            case EV_TCPIP_ETHCARD_CONNECT:
            {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_USBNET
                quec_usbnet_sendevent(QUEC_USB_NET_MSG_PORT_CONNECT, 0, 0, 0, 0);
#else
                netdevConnect();
#endif
                break;
            }
            case EV_TCPIP_ETHCARD_DISCONNECT:
            {
                OSI_LOGI(0, "EV_TCPIP_ETHCARD_DISCONNECT");
                netdevDisconnect();
#ifdef CONFIG_QUEC_PROJECT_FEATURE_USBNET
                quec_usbnet_sendevent(QUEC_USB_NET_MSG_PORT_DISCONNECT, 0, 0, 0, 0);
#endif
                break;
            }
#endif
            default:
                break;
            }
        }
    }
}

osiThread_t *netGetTaskID()
{
    return netThreadID;
}

void net_init()
{

    netThreadID = osiThreadCreate("net", net_thread, NULL, OSI_PRIORITY_NORMAL, NET_STACK_SIZE, 64);

    tcpip_init(NULL, NULL);

#if IP_NAT
    ip4_nat_init();
#endif
}
