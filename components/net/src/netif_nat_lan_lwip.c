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

#include "cfw.h"
#include "osi_api.h"
#include "osi_log.h"
#include "sockets.h"
#include "lwip/prot/ip.h"
#include "lwip/netif.h"
#include "lwip/dns.h"
#include "lwip/tcpip.h"
#include "lwip/ip4_nat.h"

#include "drv_ps_path.h"
#include "netif_ppp.h"
#include "netutils.h"
#include "at_cfw.h"
#include "quec_usbnet.h"
#include "quec_led_task.h"


#ifdef CONFIG_QUEC_PROJECT_FEATURE_NETIF
#define QUEC_NETIFLAN_LOG(msg, ...)  custom_log("QNETIFLAN", msg, ##__VA_ARGS__)
#endif

#if IP_NAT
#define SIM_CID(sim, cid) ((((sim)&0xf) << 4) | ((cid)&0xf))

extern osiThread_t *netGetTaskID();
extern uint32_t *getDNSServer(uint8_t nCid, uint8_t nSimID);
extern uint32_t do_send_stored_packet(uint8_t nCID, CFW_SIM_ID nSimID);
extern void dns_clean_entries(void);
extern u8_t ip4_wan_forward(struct pbuf *p, struct netif *inp);
extern u8_t netif_num;

#define MAX_SIM_ID 2
#define MAX_CID 7
static struct netif gprs_netif[MAX_SIM_ID][MAX_CID] = {0};

err_t nat_lan_lwip_tcpip_input(struct pbuf *p, struct netif *inp)
{
    if (p == NULL || inp == NULL)
        return 0;
    inp->u32LwipDLSize += p->tot_len;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
	quec_data_transmit_event_send(0, p->tot_len);
#endif  

    return tcpip_input(p, inp);
}

static err_t nat_lan_lwip_data_output(struct netif *netif, struct pbuf *p,
                                      ip_addr_t *ipaddr)
{
    if (p != NULL)
    {
#if LWIP_IPV6
        if (IP_HDR_GET_VERSION(p->payload) == 6)
        {
            struct ip6_hdr *ip6hdr;
            ip6hdr = (struct ip6_hdr *)p->payload;
            u8_t nexth;
            pbuf_realloc(p, IP6_HLEN + IP6H_PLEN(ip6hdr));
            nexth = IP6H_NEXTH(ip6hdr);
            if (nexth == IP6_NEXTH_ICMP6)
            {
                struct icmp6_hdr *icmp6hdr;
                icmp6hdr = (struct icmp6_hdr *)p->payload;
                if (icmp6hdr->type == ICMP6_TYPE_RS)
                {
                    //save IPV6 local addr to lan netif
                }
            }
            //find Wan netif with same SimCid to send IPV6 packeage out
            struct netif *Wannetif = netif_get_by_cid_type(netif->sim_cid, NETIF_LINK_MODE_NAT_WAN);
            if (Wannetif)
            {
                OSI_LOGI(0x0, "nat_lan_lwip_data_output IPV6 to Wan netif");
#if LWIP_TCPIP_CORE_LOCKING
                LOCK_TCPIP_CORE();
#endif
                Wannetif->output_ip6(Wannetif, p, NULL);
#if LWIP_TCPIP_CORE_LOCKING
                UNLOCK_TCPIP_CORE();
#endif
            }
        }
        else
#endif
        {
#if LWIP_TCPIP_CORE_LOCKING
            LOCK_TCPIP_CORE();
#endif
            u8_t taken = ip4_wan_forward(p, netif);
            if (taken == 0)
                taken = ip4_nat_out(p);
#if LWIP_TCPIP_CORE_LOCKING
            UNLOCK_TCPIP_CORE();
#endif
            OSI_LOGI(0x0, "nat_lan_lwip_data_output %d", taken);
        }
        netif->u32LwipULSize += p->tot_len;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        quec_data_transmit_event_send(p->tot_len, 0);
#endif
    }

    return ERR_OK;
}

err_t netif_gprs_nat_lan_lwip_init(struct netif *netif)
{
    /* initialize the snmp variables and counters inside the struct netif
   * ifSpeed: no assumption can be made!
   */
    netif->name[0] = 'G';
    netif->name[1] = 'P';

#if LWIP_IPV4
    netif->output = (netif_output_fn)nat_lan_lwip_data_output;
#endif
#if LWIP_IPV6
    netif->output_ip6 = (netif_output_ip6_fn)nat_lan_lwip_data_output;
#endif
    netif->mtu = GPRS_MTU;
#if IP_NAT_INTERNAL_FORWARD
    if (netif->pdnType > CFW_GPRS_PDP_TYPE_PPP)
        return ERR_OK;
#endif
#ifndef CONFIG_QUEC_PROJECT_FEATURE_NETIF
    if (NULL == netif_default)
    {
        int nCid = netif->sim_cid & 0x0f;
        int nSimId = (netif->sim_cid & 0xf0) >> 4;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_NETIF
        netif_set_default(netif);
#endif
        CFW_GPRS_PDPCONT_INFO_V2 pdp_context;
        ip_addr_t ip4_dns1 = {0};
        ip_addr_t ip4_dns2 = {0};
#if LWIP_IPV6
        ip_addr_t ip6_dns1 = {0};
        ip_addr_t ip6_dns2 = {0};
#endif
        if (CFW_GprsGetPdpCxtV2(nCid, &pdp_context, nSimId) == ERR_SUCCESS)
        {
            if ((pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IP) || (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IPV4V6) || (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_X25) ||
                (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_PPP)) //add evade for nPdpType == 0, need delete later
            {
                if (pdp_context.nPdpDnsSize != 0)
                {
                    IP_ADDR4(&ip4_dns1, pdp_context.pPdpDns[0], pdp_context.pPdpDns[1], pdp_context.pPdpDns[2], pdp_context.pPdpDns[3]);
                    IP_ADDR4(&ip4_dns2, pdp_context.pPdpDns[21], pdp_context.pPdpDns[22], pdp_context.pPdpDns[23], pdp_context.pPdpDns[24]);
                    OSI_LOGI(0x10007541, "DNS size:%d  DNS1:%d.%d.%d.%d", pdp_context.nPdpDnsSize,
                             pdp_context.pPdpDns[0], pdp_context.pPdpDns[1], pdp_context.pPdpDns[2], pdp_context.pPdpDns[3]);
                    OSI_LOGI(0x10007542, "DNS2:%d.%d.%d.%d",
                             pdp_context.pPdpDns[21], pdp_context.pPdpDns[22], pdp_context.pPdpDns[23], pdp_context.pPdpDns[24]);
                }
            }
            dns_setserver(0, &ip4_dns1);
            dns_setserver(1, &ip4_dns2);
#if LWIP_IPV6
            if (((pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IPV6) || (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IPV4V6)))
            {
                if (pdp_context.nPdpDnsSize != 0)
                {
                    uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpDns[4], pdp_context.pPdpDns[5], pdp_context.pPdpDns[6], pdp_context.pPdpDns[7]));
                    uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpDns[8], pdp_context.pPdpDns[9], pdp_context.pPdpDns[10], pdp_context.pPdpDns[11]));
                    uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpDns[12], pdp_context.pPdpDns[13], pdp_context.pPdpDns[14], pdp_context.pPdpDns[15]));
                    uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpDns[16], pdp_context.pPdpDns[17], pdp_context.pPdpDns[18], pdp_context.pPdpDns[19]));
                    uint32_t addr4 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpDns[25], pdp_context.pPdpDns[26], pdp_context.pPdpDns[27], pdp_context.pPdpDns[28]));
                    uint32_t addr5 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpDns[29], pdp_context.pPdpDns[30], pdp_context.pPdpDns[31], pdp_context.pPdpDns[32]));
                    uint32_t addr6 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpDns[33], pdp_context.pPdpDns[34], pdp_context.pPdpDns[35], pdp_context.pPdpDns[36]));
                    uint32_t addr7 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpDns[37], pdp_context.pPdpDns[38], pdp_context.pPdpDns[39], pdp_context.pPdpDns[40]));
                    OSI_LOGI(0x10007543, "DNS size:%d  ipv6 DNS 12-15:%d.%d.%d.%d", pdp_context.nPdpDnsSize,
                             pdp_context.pPdpDns[12], pdp_context.pPdpDns[13], pdp_context.pPdpDns[14], pdp_context.pPdpDns[15]);
                    OSI_LOGI(0x10007544, " ipv6 DNS 33-36:%d.%d.%d.%d",
                             pdp_context.pPdpDns[33], pdp_context.pPdpDns[34], pdp_context.pPdpDns[35], pdp_context.pPdpDns[36]);
                    IP_ADDR6(&ip6_dns1, addr0, addr1, addr2, addr3);
                    IP_ADDR6(&ip6_dns2, addr4, addr5, addr6, addr7);
                }
            }
#endif

#if LWIP_IPV6
            if (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IPV6)
            {
                dns_setserver(0, &ip6_dns1);
                dns_setserver(1, &ip6_dns2);
            }
            if (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IPV4V6)
            {
                if (IP_IS_V6(&ip6_dns1))
                    dns_setserver(1, &ip6_dns1);
            }
#endif
        }
    }
#endif
    return ERR_OK;
}

struct netif *TCPIP_nat_lan_lwip_netif_create(uint8_t nCid, uint8_t nSimId)
{
    struct netif *netif = NULL;
    uint8_t T_cid = nSimId << 4 | nCid;
    struct netif *wan_netif = netif_get_by_cid_type(T_cid, NETIF_LINK_MODE_NAT_WAN);
    if (wan_netif == NULL)
    {
        return NULL;
    }
    ip4_addr_t *wan_ip4_addr = (ip4_addr_t *)netif_ip4_addr(wan_netif);
    ip4_addr_t ip4;
    ip4_addr_t ip4_gw;
    OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_create wan_ip4_addr 0x%x\n", wan_ip4_addr->addr);
    if ((wan_ip4_addr->addr & 0xff) != 0xc0) //192.xxx.xxx.xxx
    {
        IP4_ADDR(&ip4, 192, 168, nCid, 2 + netif_num);
        IP4_ADDR(&ip4_gw, 192, 168, nCid, 1);
    }
    else
    {
        IP4_ADDR(&ip4, 10, 0, nCid, 2 + netif_num);
        IP4_ADDR(&ip4_gw, 10, 0, nCid, 1);
    }
    ip4_addr_t ip4_netMast;
    IP4_ADDR(&ip4_netMast, 255, 255, 255, 0);
#if LWIP_IPV6
    ip6_addr_t ip6 = {0};
#endif
    //uint8_t lenth;

    OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_create cid %d simid %d\n", nCid, nSimId);

    CFW_GPRS_PDPCONT_INFO_V2 pdp_context;
    CFW_GprsGetPdpCxtV2(nCid, &pdp_context, nSimId);
    OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_create pdp_context.PdnType %d\n", pdp_context.PdnType);
    if ((pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IP) || (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IPV4V6) || (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_X25) || (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_PPP)) //add evade for nPdpType == 0, need delete later
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0x10007548, "TCPIP_netif_create IP4: %s", ip4addr_ntoa(&ip4));
    }
    else
    {
        ip4.addr = 0;
        OSI_LOGXI(OSI_LOGPAR_S, 0x10007548, "TCPIP_netif_create IP4: %s", ip4addr_ntoa(&ip4));
    }
    if (nSimId < MAX_SIM_ID && nCid < MAX_CID)
    {
        netif = &gprs_netif[nSimId][nCid];
    }
    else
    {
        netif = malloc(sizeof(struct netif));
    }
    memset(netif, 0, sizeof(struct netif));
    netif->sim_cid = T_cid;
    netif->is_ppp_mode = 0;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NETIF
    netif->link_mode = NETIF_LINK_MODE_AVAILABLE;
#else
    netif->link_mode = NETIF_LINK_MODE_NAT_LWIP_LAN;
#endif
    netif->pdnType = pdp_context.PdnType;
    netif->is_used = 1;

#if LWIP_TCPIP_CORE_LOCKING
    LOCK_TCPIP_CORE();
#endif
    netif_add(netif, &ip4, &ip4_netMast, &ip4_gw, NULL, netif_gprs_nat_lan_lwip_init, nat_lan_lwip_tcpip_input);
#if LWIP_IPV6
    if (((pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IPV6) || (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IPV4V6)))
    {
        uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpAddr[4], pdp_context.pPdpAddr[5], pdp_context.pPdpAddr[6], pdp_context.pPdpAddr[7]));
        uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpAddr[8], pdp_context.pPdpAddr[9], pdp_context.pPdpAddr[10], pdp_context.pPdpAddr[11]));
        uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpAddr[12], pdp_context.pPdpAddr[13], pdp_context.pPdpAddr[14], pdp_context.pPdpAddr[15]));
        uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(pdp_context.pPdpAddr[16], pdp_context.pPdpAddr[17], pdp_context.pPdpAddr[18], pdp_context.pPdpAddr[19]));
        IP6_ADDR(&ip6, addr0, addr1, addr2, addr3);
        if (ip6.addr[0] == 0x0)
        {
            ip6.addr[0] = PP_HTONL(0xfe800000ul);
        }
        netif_ip6_addr_set(netif, 0, &ip6);
        netif_ip6_addr_set_state(netif, 0, IP6_ADDR_VALID);
        OSI_LOGXI(OSI_LOGPAR_S, 0x0, "TCPIP_nat_lan_lwip_netif_create IP6: %s", ip6addr_ntoa(&ip6));
    }
#endif
    netif_set_up(netif);
    netif_set_link_up(netif);

    ip4_nat_entry_t *new_nat_entry = (ip4_nat_entry_t *)calloc(1, sizeof(ip4_nat_entry_t));
    if (new_nat_entry == NULL)
    {
        if (nSimId < MAX_SIM_ID && nCid < MAX_CID)
        {
            netif->is_used = 0;
        }
        else
        {
            free(netif);
        }
        return NULL;
    }
    new_nat_entry->in_if = netif;
    new_nat_entry->out_if = wan_netif;
    ip4_addr_copy(new_nat_entry->dest_net, *wan_ip4_addr);
    IP4_ADDR(&(new_nat_entry->dest_netmask), 255, 255, 255, 255);
    ip4_addr_copy(new_nat_entry->source_net, ip4);
    IP4_ADDR(&(new_nat_entry->source_netmask), 255, 255, 255, 255);
    ip4_nat_add(new_nat_entry);
    netif->nat_entry = new_nat_entry;
#if LWIP_TCPIP_CORE_LOCKING
    UNLOCK_TCPIP_CORE();
#endif
    OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_create, netif->num: 0x%x\n", netif->num);
    return netif;
}
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NETIF
void TCPIP_nat_lan_lwip_available_netif_destory(uint8_t nCid, uint8_t nSimId)
{
    struct netif *netif = NULL;

    netif = netif_get_by_cid_type(nSimId << 4 | nCid, NETIF_LINK_MODE_AVAILABLE);
    if (netif != NULL)
    {
#if LWIP_TCPIP_CORE_LOCKING
        LOCK_TCPIP_CORE();
#endif
        ip4_nat_entry_t *new_nat_entry = netif->nat_entry;
        if (new_nat_entry != NULL)
        {
            netif->nat_entry = NULL;
            ip4_nat_remove(new_nat_entry);
            free(new_nat_entry);
        }
        netif_set_link_down(netif);
        netif_remove(netif);
        dns_clean_entries();
        if (nSimId < MAX_SIM_ID && nCid < MAX_CID)
        {
            netif->is_used = 0;
        }
        else
        {
            free(netif);
        }
        OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_destory netif");
        tcp_debug_print_pcbs();
#if LWIP_TCPIP_CORE_LOCKING
        UNLOCK_TCPIP_CORE();
#endif
    }
    else
    {
        QUEC_NETIFLAN_LOG("Error ########### CFW_GPRS_DEACTIVED can't find netif for CID=0x%x\n", SIM_CID(nSimId, nCid));
    }
}
#endif

void TCPIP_nat_lan_lwip_netif_destory(uint8_t nCid, uint8_t nSimId)
{
    struct netif *netif = NULL;

    netif = netif_get_by_cid_type(nSimId << 4 | nCid, NETIF_LINK_MODE_NAT_LWIP_LAN);
    if (netif != NULL)
    {
#if LWIP_TCPIP_CORE_LOCKING
        LOCK_TCPIP_CORE();
#endif
        ip4_nat_entry_t *new_nat_entry = netif->nat_entry;
        if (new_nat_entry != NULL)
        {
            netif->nat_entry = NULL;
            ip4_nat_remove(new_nat_entry);
            free(new_nat_entry);
        }
        netif_set_link_down(netif);
        netif_remove(netif);
        dns_clean_entries();
        if (nSimId < MAX_SIM_ID && nCid < MAX_CID)
        {
            netif->is_used = 0;
        }
        else
        {
            free(netif);
        }
        OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_destory netif");
        tcp_debug_print_pcbs();
#if LWIP_TCPIP_CORE_LOCKING
        UNLOCK_TCPIP_CORE();
#endif
    }
    else
    {
        OSI_LOGI(0x1000754c, "Error ########### CFW_GPRS_DEACTIVED can't find netif for CID=0x%x\n", SIM_CID(nSimId, nCid));
    }
    
#ifdef CONFIG_QUEC_PROJECT_FEATURE_USBNET
    TCPIP_nat_lan_lwip_available_netif_destory(nCid, nSimId);
#endif
}

void netif_setup_lwip_lanOnly()
{
#if IP_NAT_INTERNAL_FORWARD
    OSI_LOGI(0x0, "net_nat\n");
    if (get_nat_enabled(0, 1))
    {
        //TCPIP_nat_lanOnly_lwip_netif_create(0x0f, 0x0f);
        struct lan_netif *plan = newLan(NAT_SIM, NAT_CID, NAT_LAN_IDX_LWIP);
        if (plan == NULL)
            return;
        lan_create(plan, NAT_SIM, NAT_CID, netif_gprs_nat_lan_lwip_init, nat_lan_lwip_tcpip_input);
    }
#endif
    return;
}
#endif //IP_NAT
