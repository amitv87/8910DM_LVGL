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
#define QUEC_NETIFWAN_LOG(msg, ...)  custom_log("QNETIFWAN", msg, ##__VA_ARGS__)
#endif
#if IP_NAT
#define SIM_CID(sim, cid) ((((sim)&0xf) << 4) | ((cid)&0xf))

extern osiThread_t *netGetTaskID();
extern uint32_t *getDNSServer(uint8_t nCid, uint8_t nSimID);
extern uint32_t do_send_stored_packet(uint8_t nCID, CFW_SIM_ID nSimID);
extern void dns_clean_entries(void);

#define MAX_SIM_ID 2
#define MAX_CID 7
static struct netif gprs_netif[MAX_SIM_ID][MAX_CID] = {0};

static void gprs_data_ipc_to_lwip_nat_wan(void *ctx)
{
    struct netif *inp_netif = (struct netif *)ctx;
    if (inp_netif == NULL)
        return;
    struct pbuf *p, *q;
    OSI_LOGI(0x10007538, "gprs_data_ipc_to_lwip");
    uint8_t *pData = malloc(1600);
    if (pData == NULL)
        return;
    int len = 0;
    int readLen = 0;
    int offset = 0;
    do
    {
        OSI_LOGI(0x10007539, "drvPsIntfRead in");
        readLen = drvPsIntfRead(inp_netif->pspathIntf, pData, 1600);
#ifdef CONFIG_NET_TRACE_IP_PACKET
        uint8_t *ipdata = pData;
        uint16_t identify = (ipdata[4] << 8) + ipdata[5];
        OSI_LOGI(0x0, "Wan DL read from IPC thread identify %04x", identify);
#endif
        len = readLen;
        OSI_LOGI(0x1000753a, "drvPsIntfRead out %d", len);
        if (len > 0)
        {
            sys_arch_dump(pData, len);
        }
        if (inp_netif != NULL && len > 0)
        {
            p = (struct pbuf *)pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
            offset = 0;
            if (p != NULL)
            {
                if (len > p->len)
                {
                    for (q = p; len > q->len; q = q->next)
                    {
                        memcpy(q->payload, &(pData[offset]), q->len);
                        len -= q->len;
                        offset += q->len;
                    }
                    if (len != 0)
                    {
                        memcpy(q->payload, &(pData[offset]), len);
                    }
                }
                else
                {
                    memcpy(p->payload, &(pData[offset]), len);
                }
#if LWIP_IPV6
                if (IP_HDR_GET_VERSION(p->payload) == 6)
                { //find lan netif with same SimCid and same IPV6 addr to send
                    struct netif *netif;
                    u8_t sim_cid = inp_netif->sim_cid;
                    u8_t taken = 0;
                    NETIF_FOREACH(netif)
                    {
                        if (sim_cid == netif->sim_cid && (NETIF_LINK_MODE_NAT_LWIP_LAN == netif->link_mode || NETIF_LINK_MODE_NAT_PPP_LAN == netif->link_mode || NETIF_LINK_MODE_NAT_NETDEV_LAN == netif->link_mode))
                        {
                            struct ip6_hdr *ip6hdr = p->payload;
                            ip6_addr_t current_iphdr_dest;
                            ip6_addr_t *current_netif_addr;
                            ip6_addr_copy_from_packed(current_iphdr_dest, ip6hdr->dest);
                            current_netif_addr = (ip6_addr_t *)netif_ip6_addr(netif, 0);
                            if (current_netif_addr->addr[2] == current_iphdr_dest.addr[2] && current_netif_addr->addr[3] == current_iphdr_dest.addr[3])
                            {
                                OSI_LOGI(0x0, "gprs_data_ipc_to_lwip_nat_wan IPV6 to Lan netif");
                                netif->input(p, netif);
                                taken = 1;
                                break;
                            }
                        }
                    }
                    if (taken == 0)
                    {
                        NETIF_FOREACH(netif)
                        {
                            if (sim_cid == netif->sim_cid && (NETIF_LINK_MODE_NAT_LWIP_LAN == netif->link_mode || NETIF_LINK_MODE_NAT_PPP_LAN == netif->link_mode || NETIF_LINK_MODE_NAT_NETDEV_LAN == netif->link_mode))
                            {
                                OSI_LOGI(0x0, "gprs_data_ipc_to_lwip_nat_wan brodcast IPV6 to Lan netif");
                                pbuf_ref(p);
                                netif->input(p, netif);
                            }
                        }
                        inp_netif->input(p, inp_netif);
                    }
                }
                else
#endif
                {
                    u8_t taken = 0;
#if LWIP_TCPIP_CORE_LOCKING
                    LOCK_TCPIP_CORE();
#endif
                    taken = ip4_nat_input(p);
#if LWIP_TCPIP_CORE_LOCKING
                    UNLOCK_TCPIP_CORE();
#endif
                    if (taken == 0)
                        inp_netif->input(p, inp_netif);
                }
                inp_netif->u32LwipDLSize += readLen;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
                quec_data_transmit_event_send(0, readLen);
#endif
            }
        }
    } while (readLen > 0);
    free(pData);
}

void lwip_nat_wan_pspathDataInput(void *ctx, drvPsIntf_t *p)
{
    OSI_LOGI(0x0, "lwip_nat_wan_pspathDataInput osiThreadCallback in ");
    osiThreadCallback(netGetTaskID(), gprs_data_ipc_to_lwip_nat_wan, (void *)ctx);
    OSI_LOGI(0x0, "lwip_nat_wan_pspathDataInput osiThreadCallback out");
}

static err_t nat_wan_data_output(struct netif *netif, struct pbuf *p,
                                 ip_addr_t *ipaddr)
{
    uint16_t offset = 0;
    struct pbuf *q = NULL;

#if 1
    OSI_LOGI(0x1000753d, "data_output ---------tot_len=%d, flags=0x%x---------", p->tot_len, p->flags);

    uint8_t *pData = malloc(p->tot_len);
    if (pData == NULL)
    {
        return !ERR_OK;
    }

    for (q = p; q != NULL; q = q->next)
    {
        memcpy(&pData[offset], q->payload, q->len);
        offset += q->len;
    }

    sys_arch_dump(pData, p->tot_len);

    OSI_LOGI(0x1000753e, "drvPsIntfWrite--in---");
    drvPsIntfWrite((drvPsIntf_t *)netif->pspathIntf, pData, p->tot_len);
    OSI_LOGI(0x1000753f, "drvPsIntfWrite--out---");
    netif->u32LwipULSize += p->tot_len;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
    quec_data_transmit_event_send(p->tot_len, 0);
#endif
    free(pData);
#endif

    OSI_LOGI(0x10007540, "data_output--return in netif_gprs---");

    return ERR_OK;
}

static err_t netif_gprs_nat_wan_init(struct netif *netif)
{
    /* initialize the snmp variables and counters inside the struct netif
   * ifSpeed: no assumption can be made!
   */
    netif->name[0] = 'G';
    netif->name[1] = 'P';

#if LWIP_IPV4
    netif->output = (netif_output_fn)nat_wan_data_output;
#endif
#if LWIP_IPV6
    netif->output_ip6 = (netif_output_ip6_fn)nat_wan_data_output;
#endif
    netif->mtu = GPRS_MTU;

    return ERR_OK;
}

extern uint8_t CFW_GprsGetPdpIpv6Addr(uint8_t nCid, uint8_t *nLength, uint8_t *ipv6Addr, CFW_SIM_ID nSimID);
extern uint8_t CFW_GprsGetPdpIpv4Addr(uint8_t nCid, uint8_t *nLength, uint8_t *ipv4Addr, CFW_SIM_ID nSimID);
extern err_t ND6_Lan_Send_RS(struct netif *lan_netif, struct netif *netif);
struct netif *TCPIP_nat_wan_netif_create(uint8_t nCid, uint8_t nSimId)
{
    struct netif *netif = NULL;
    uint8_t T_cid = nSimId << 4 | nCid;
    netif = getGprsNetIf(nSimId, nCid);
    if (netif != NULL)
    {
        OSI_LOGI(0x0, "TCPIP_nat_wan_netif_create netif already created cid %d simid %d\n", nCid, nSimId);
        goto end;
    }

    ip4_addr_t ip4 = {0};
    //uint8_t lenth;

    OSI_LOGI(0x0, "TCPIP_nat_wan_netif_create cid %d simid %d\n", nCid, nSimId);

    AT_Gprs_CidInfo *pCidInfo = &gAtCfwCtx.sim[nSimId].cid_info[nCid];

    if (pCidInfo->uCid != nCid)
    {
        pCidInfo->uCid = nCid;
    }

    CFW_GPRS_PDPCONT_INFO_V2 pdp_context;
    if (CFW_GprsGetPdpCxtV2(nCid, &pdp_context, nSimId) != ERR_SUCCESS)
        goto end;
    OSI_LOGI(0x0, "TCPIP_nat_wan_netif_create pdp_context.PdnType %d\n", pdp_context.PdnType);
    if ((pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IP) || (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_IPV4V6) || (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_X25) || (pdp_context.PdnType == CFW_GPRS_PDP_TYPE_PPP)) //add evade for nPdpType == 0, need delete later
    {
        //uint8_t ipv4[4] = {0};
        //CFW_GprsGetPdpIpv4Addr(nCid, &lenth, ipv4, nSimId);
        //IP4_ADDR(&ip4, ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
        IP4_ADDR(&ip4, pdp_context.pPdpAddr[0], pdp_context.pPdpAddr[1], pdp_context.pPdpAddr[2], pdp_context.pPdpAddr[3]);
        OSI_LOGXI(OSI_LOGPAR_S, 0x0, "TCPIP_nat_wan_netif_create IP4: %s", ip4addr_ntoa(&ip4));
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
    netif->link_mode = NETIF_LINK_MODE_NAT_WAN;
    netif->pdnType = pdp_context.PdnType;
    netif->is_used = 1;

    drvPsIntf_t *dataPsPath = drvPsIntfOpen(nSimId, nCid, lwip_nat_wan_pspathDataInput, (void *)netif);
    if (dataPsPath != NULL)
    {
        netif->pspathIntf = dataPsPath;
        /**if dataPsPath is NULL, it has been write in netif->pspathIntf. 
		   netif->pspathIntf need not update any more.**/
    }
#if LWIP_TCPIP_CORE_LOCKING
    LOCK_TCPIP_CORE();
#endif
    netif_add(netif, &ip4, NULL, NULL, NULL, netif_gprs_nat_wan_init, tcpip_input);
#if LWIP_TCPIP_CORE_LOCKING
    UNLOCK_TCPIP_CORE();
#endif
end:
    OSI_LOGI(0x0, "wan_netif_create, netif = %p", netif);
    return netif;
}

void TCPIP_nat_wan_netif_destory(uint8_t nCid, uint8_t nSimId)
{
    struct netif *netif = NULL;

    netif = getGprsWanNetIf(nSimId, nCid);
    if (netif != NULL)
    {
#if LWIP_TCPIP_CORE_LOCKING
        LOCK_TCPIP_CORE();
#endif
        drvPsIntf_t *dataPsPath = netif->pspathIntf;
        drvPsIntfClose(dataPsPath);
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
        OSI_LOGI(0x0, "TCPIP_nat_wan_netif_destory netif");
        tcp_debug_print_pcbs();
#if LWIP_TCPIP_CORE_LOCKING
        UNLOCK_TCPIP_CORE();
#endif
    }
    else
    {
        OSI_LOGI(0x1000754c, "Error ########### CFW_GPRS_DEACTIVED can't find netif for CID=0x%x\n", SIM_CID(nSimId, nCid));
    }
}

u8_t ip4_wan_forward(struct pbuf *p, struct netif *inp)
{
    u8_t taken = 0;
    if (IP_HDR_GET_VERSION(p->payload) == 4)
    {
        //find lan netif with same CID IPV4 addr to send
        struct netif *netif;
#if !IP_NAT_INTERNAL_FORWARD
        u8_t sim_cid = inp->sim_cid;
#endif
        NETIF_FOREACH(netif)
        {
            if (
#if !IP_NAT_INTERNAL_FORWARD
                sim_cid == netif->sim_cid &&
#endif
                (NETIF_LINK_MODE_NAT_LWIP_LAN == netif->link_mode || NETIF_LINK_MODE_NAT_PPP_LAN == netif->link_mode || NETIF_LINK_MODE_NAT_NETDEV_LAN == netif->link_mode))
            {
                struct ip_hdr *iphdr = p->payload;
                ip4_addr_t current_iphdr_dest;
                ip4_addr_t *current_netif_addr;
                ip4_addr_copy(current_iphdr_dest, iphdr->dest);
                current_netif_addr = (ip4_addr_t *)netif_ip4_addr(netif);
                if (current_iphdr_dest.addr == current_netif_addr->addr)
                {
                    OSI_LOGI(0x0, "ip4_wan_forward IPV4 to Lan netif");
                    pbuf_ref(p);
                    netif->input(p, netif);
                    taken = 1;
                    break;
                }
            }
        }
    }
    return taken;
}
#if IP_NAT_INTERNAL_FORWARD
extern err_t netif_gprs_nat_lan_lwip_init(struct netif *netif);
extern err_t nat_lan_lwip_tcpip_input(struct pbuf *p, struct netif *inp);

struct wan_netif *wan[MAX_SIM_ID][MAX_CID] = {0};
struct lan_netif *lan[MAX_SIM_ID][MAX_CID][NAT_MAX_LANNUM] = {0};
struct lan_netif *lan_internal[NAT_MAX_LANNUM] = {0};

uint8_t lantype[NAT_MAX_LANNUM] = {NETIF_LINK_MODE_NAT_LWIP_LAN, NETIF_LINK_MODE_NAT_PPP_LAN, NETIF_LINK_MODE_NAT_NETDEV_LAN};
uint8_t lanip[NAT_MAX_LANNUM] = {NAT_IPADDR_LWIP, NAT_IPADDR_PPP, NAT_IPADDR_RNDIS};
struct netif *nat_lan_netif_create(uint8_t nCid, uint8_t nSimId, uint8_t nIdx, netif_init_fn init, netif_input_fn input)
{
    uint8_t T_cid = nSimId << 4 | nCid;
    struct netif *netif = NULL;
    ip4_addr_t ip4;
    ip4_addr_t ip4_gw;
    OSI_LOGI(0x0, "nat_lan_netif_create ipaddress %d\n", lanip[nIdx]);

    IP4_ADDR(&ip4, 192, 168, nCid, lanip[nIdx]); //);2 + netif_num);
    IP4_ADDR(&ip4_gw, 192, 168, nCid, 1);

    ip4_addr_t ip4_netMast;
    IP4_ADDR(&ip4_netMast, 255, 255, 255, 0);
#if LWIP_IPV6
    ip6_addr_t ip6 = {0};
#endif
    //uint8_t lenth;

    OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_create cid %d simid %d\n", nCid, nSimId);

    CFW_GPRS_PDPCONT_INFO_V2 pdp_context = {0};

    if (CFW_GprsGetPdpCxtV2(nCid, &pdp_context, nSimId) == ERR_SUCCESS)
    {
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
    }
    else
        pdp_context.PdnType = CFW_GPRS_PDP_TYPE_PPP + 1;

    OSI_LOGI(0x0, "1111 pdp_context.PdnType = %d", pdp_context.PdnType);

    //if (nSimId < MAX_SIM_ID && nCid < MAX_CID)
    //{
    //    netif = &gprs_netif[nSimId][nCid];
    //}
    //else
    //{
    netif = malloc(sizeof(struct netif));
    //}
    memset(netif, 0, sizeof(struct netif));
    netif->sim_cid = T_cid;
    if (lantype[nIdx] == NETIF_LINK_MODE_NAT_PPP_LAN)
        netif->is_ppp_mode = 1;
    else
        netif->is_ppp_mode = 0;
    netif->link_mode = lantype[nIdx];
    netif->pdnType = pdp_context.PdnType;
    netif->is_used = 1;
    OSI_LOGI(0x0, "2222 pdp_context.PdnType = %d", pdp_context.PdnType);
#if LWIP_TCPIP_CORE_LOCKING
    LOCK_TCPIP_CORE();
#endif
    OSI_LOGI(0x0, "4444 pdp_context.PdnType = %d", pdp_context.PdnType);
    netif_add(netif, &ip4, &ip4_netMast, &ip4_gw, NULL, init, input);
    OSI_LOGI(0x0, "5555 pdp_context.PdnType = %d", pdp_context.PdnType);
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
    OSI_LOGI(0x0, "6666 pdp_context.PdnType = %d", pdp_context.PdnType);
#if LWIP_TCPIP_CORE_LOCKING
    UNLOCK_TCPIP_CORE();
#endif
    netif_set_up(netif);
    netif_set_link_up(netif);
    OSI_LOGI(0x0, "3333 netif = %p", netif);
    return netif;
}

void lan_setDefaultNetif(void *this)
{
    struct lan_netif *plan = (struct lan_netif *)this;

    if (plan == NULL)
        goto end;

    if ((plan->isUsing == 1) && (plan->netif != NULL))
    {
        netif_set_default(plan->netif);
        netif_reset_dns(plan->netif);
        plan->isDefault = 1;
        OSI_LOGI(0x0, "lan_setDefaultNetif");
    }
end:
    return;
}
void lan_setSimCid(void *lannetif, int8_t sim, uint8_t cid)
{
    struct lan_netif *plan = (struct lan_netif *)lannetif;

    if (plan == NULL)
        goto end;

    if ((plan->isUsing == 1) && (plan->netif != NULL))
    {
        plan->netif->sim_cid = sim << 4 | cid;
        if (sim == NAT_SIM && cid == NAT_CID)
            plan->netif->pdnType = CFW_GPRS_PDP_TYPE_PPP + 1;
        OSI_LOGI(0x0, "lan_setSimCid = %d pdnType = %d", plan->netif->sim_cid, plan->netif->pdnType);
    }
end:
    return;
}
uint8_t lan_create(void *lannetif, uint8_t sim, uint8_t cid, netif_init_fn init, netif_input_fn input)
{
    uint8_t iRet = false;
    struct lan_netif *plan = (struct lan_netif *)lannetif;
    if (plan == NULL)
        goto end;
    if ((plan->netif = nat_lan_netif_create(cid, sim, plan->iIdx, init, input)) == NULL)
        goto end;
    plan->netif->pstlan = plan;

    iRet = true;
end:
    OSI_LOGI(0x0, "lan_create iRet = %d", iRet);
    return iRet;
}
void lan_destroy(void *lannetif)
{
    struct lan_netif *plan = (struct lan_netif *)lannetif;

#if LWIP_TCPIP_CORE_LOCKING
    LOCK_TCPIP_CORE();
#endif
    if (plan == NULL)
        goto end;
    struct netif *netif = plan->netif;
    if (netif == NULL)
        goto end;

    if ((netif = plan->netif) != NULL)
    {
        //remove wan table
        lan_removeNATEntry(plan);

        //remove netif
        netif_set_link_down(netif);
        netif_remove(netif);
        dns_clean_entries();
        //if (sim < MAX_SIM_ID && cid < MAX_CID)
        //{
        //    netif->is_used = 0;
        //}
        //else
        //{
        free(netif);
        //}
        OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_destory netif");
        tcp_debug_print_pcbs();

        //set lan
        plan->netif = NULL;
        plan->isUsing = 0;
        plan->iIdx = NAT_MAX_LANNUM;
    }
end:
#if LWIP_TCPIP_CORE_LOCKING
    UNLOCK_TCPIP_CORE();
#endif
    return;
}

uint8_t lan_addNATEntry(void *lannetif, void *wannetif)
{
    uint8_t iRet = false;
    struct wan_netif *pWan = (struct wan_netif *)wannetif;
    struct lan_netif *plan = (struct lan_netif *)lannetif;
    OSI_LOGI(0x0, "lan_addNATEntry wannetif = %p", wannetif);
    if (pWan == NULL || plan == NULL)
        goto end;

    struct netif *lan_netif = plan->netif;
    struct netif *wan_netif = pWan->netif;

    ip4_addr_t *wan_ip4_addr = (ip4_addr_t *)netif_ip4_addr(wan_netif);
    ip4_addr_t *netif_ip4 = ip_2_ip4(&(lan_netif->ip_addr));

    //add nat entry for lan netif
    ip4_nat_entry_t *new_nat_entry = (ip4_nat_entry_t *)calloc(1, sizeof(ip4_nat_entry_t));
    if (new_nat_entry == NULL)
        goto end;

    new_nat_entry->in_if = plan->netif;
    new_nat_entry->out_if = pWan->netif;
    ip4_addr_copy(new_nat_entry->dest_net, *wan_ip4_addr);
    IP4_ADDR(&(new_nat_entry->dest_netmask), 255, 255, 255, 255);

    netif_ip4 = ip_2_ip4(&(lan_netif->ip_addr));
    ip4_addr_copy(new_nat_entry->source_net, *netif_ip4);
    IP4_ADDR(&(new_nat_entry->source_netmask), 255, 255, 255, 255);
    ip4_nat_add(new_nat_entry);
    lan_netif->nat_entry = new_nat_entry;

    //lan add to wan
    pWan->lan_netif[plan->iIdx] = plan;
    iRet = true;
end:
    OSI_LOGI(0x0, "lan_addNATEntry iRet = %d", iRet);
    return iRet;
}
uint8_t lan_removeNATEntry(void *lannetif)
{
    uint8_t iRet = false;
    struct lan_netif *plan = (struct lan_netif *)lannetif;
    if (plan == NULL)
        goto end;
    struct netif *netif = plan->netif;
    if (netif == NULL)
        goto end;

    ip4_nat_entry_t *new_nat_entry = netif->nat_entry;
    if (new_nat_entry != NULL)
    {
        netif->nat_entry = NULL;
        ip4_nat_remove(new_nat_entry);
        free(new_nat_entry);
        iRet = true;
    }
end:
    OSI_LOGI(0x0, "lan_removeNATEntry iRet = %d", iRet);
    return iRet;
}

uint8_t lan_add2wan(void *lannetif, uint8_t iIdx, void *wannetif)
{
    uint8_t iRet = false;
    struct wan_netif *pwan = (struct wan_netif *)wannetif;
    struct lan_netif *plan = (struct lan_netif *)lannetif;
    OSI_LOGI(0x0, "lan_add2wan wannetif = %p", wannetif);
    if (pwan == NULL || plan == NULL || iIdx >= NAT_MAX_LANNUM)
        goto end;

    //add new wan table
    OSI_LOGI(0x0, "lan_add2wan add lan %d to wan %p", iIdx, pwan);

    lan_addNATEntry(plan, pwan);
    pwan->lan_netif[iIdx] = plan;
    iRet = true;
end:
    OSI_LOGI(0x0, "lan_removeNATEntry iRet = %d", iRet);
    return iRet;
}

uint8_t lan_remove2wan(void *lannetif, uint8_t iIdx, void *wannetif)
{
    uint8_t iRet = false;
    struct wan_netif *pwan = (struct wan_netif *)wannetif;
    struct lan_netif *plan = (struct lan_netif *)lannetif;
    OSI_LOGI(0x0, "lan_remove2wan wannetif = %p", wannetif);
    if (pwan == NULL || plan == NULL || iIdx >= NAT_MAX_LANNUM)
        goto end;

    lan_removeNATEntry(plan);
    pwan->lan_netif[iIdx] = NULL;
    iRet = true;
end:
    OSI_LOGI(0x0, "lan_remove2wan iRet = %d", iRet);
    return iRet;
}

uint8_t lan_GetLinkModeIdx(void *lannetif)
{
    uint8_t iIdx = NAT_MAX_LANNUM;
    struct lan_netif *plan = (struct lan_netif *)lannetif;
    if (plan == NULL)
        goto end;

    iIdx = plan->iIdx;
end:
    OSI_LOGI(0x0, "LinkModeSwitch2 Idx %d\n", iIdx);
    return iIdx;
}
struct lan_netif *getLan(uint8_t nSimId, uint8_t nCid, uint8_t nIdx)
{
    struct lan_netif *plan = NULL;
    OSI_LOGI(0x0, "getLan SimId = %d nCid = %d nIdx = %d", nSimId, nCid, nIdx);
    //check sim cid
    if (nSimId >= MAX_SIM_ID || nCid >= MAX_CID || nIdx >= NAT_MAX_LANNUM)
    {
        if ((nSimId == NAT_SIM && nCid == NAT_CID) && nIdx < NAT_MAX_LANNUM)
        {
            plan = lan_internal[nIdx];
        }
    }
    else
        plan = lan[nSimId][nCid][nIdx];
    OSI_LOGI(0x0, "getLan plan = %p", plan);
    return plan;
}
static void Lan_init(struct lan_netif *plan, uint8_t nIdx)
{
    memset(plan, 0, sizeof(struct lan_netif));
    plan->iIdx = nIdx;
    plan->isUsing = 1;

    OSI_LOGI(0x0, "initLan");
}
struct lan_netif *newLan(uint8_t nSimId, uint8_t nCid, uint8_t nIdx)
{
    struct lan_netif *plan = NULL;
    OSI_LOGI(0x0, "newLan %d %d %d", nSimId, nCid, nIdx);
#if LWIP_TCPIP_CORE_LOCKING
    LOCK_TCPIP_CORE();
#endif
    if (nSimId == 0x0f && nCid == 0x0f && nIdx < NAT_MAX_LANNUM)
        plan = lan_internal[nIdx];
    else if (nSimId >= MAX_SIM_ID || nCid >= MAX_CID || nIdx >= NAT_MAX_LANNUM)
        goto end;
    else
        plan = lan[nSimId][nCid][nIdx];

    if (plan == NULL)
    {
        plan = malloc(sizeof(struct lan_netif));
        if (plan == NULL)
            goto end;

        if (nSimId == 0x0f && nCid == 0x0f)
            lan_internal[nIdx] = plan;
        else
            lan[nSimId][nCid][nIdx] = plan;
        memset(plan, 0, sizeof(struct lan_netif));
    }

    if (plan->isUsing == 0)
        Lan_init(plan, nIdx);
    else
        plan = NULL; //is in use
end:
#if LWIP_TCPIP_CORE_LOCKING
    UNLOCK_TCPIP_CORE();
#endif
    OSI_LOGI(0x0, "newLan plan = %p", plan);
    return plan;
}

void destoryLan(uint8_t nSimId, uint8_t nCid, uint8_t nIdx)
{
    struct lan_netif *plan = NULL;

    //check sim cid
    if (nSimId >= MAX_SIM_ID || nCid >= MAX_CID || nIdx >= NAT_MAX_LANNUM)
    {
        if ((nSimId == NAT_SIM && nCid == NAT_CID) && nIdx < NAT_MAX_LANNUM)
        {
            plan = lan_internal[nIdx];
        }
    }
    else
        plan = lan[nSimId][nCid][nIdx];

    if (plan == NULL)
        goto end;

    lan_destroy(plan);
end:
    OSI_LOGI(0x0, "destoryLan");
    return;
}
uint8_t wan_isUnique(uint8_t sim, uint8_t cid)
{
    uint8_t iRet = true;
    uint8_t i, j;
    //struct wan_netif *pwanlist[2]= &wan[0];

    //if (pwanlist == NULL)
    //    goto end;

    for (i = 0; i < MAX_SIM_ID; i++)
    {
        for (j = 0; j < MAX_CID; j++)
        {
            if (wan[i][j] != NULL)
            {
                OSI_LOGI(0x0, "wan[%d][%d]", i, j);
                if ((wan[i][j]->act == 1) && (i != sim || j != cid))
                {
                    iRet = false;
                    break;
                }
            }
        }
    }
    //end:
    OSI_LOGI(0x0, "wan_isUnique iRet = %d", iRet);
    return iRet;
}
uint8_t wan_create(void *wannetif, uint8_t sim, uint8_t cid)
{
    uint8_t iRet = false;
    struct wan_netif *pwan = (struct wan_netif *)wannetif;

    if ((pwan->netif = TCPIP_nat_wan_netif_create(cid, sim)) == NULL)
        goto end;

    wan_bindLan(pwan);

    //add one if no lwip lan
    struct lan_netif *plan = pwan->lan_netif[NAT_LAN_IDX_LWIP];
    if (plan == NULL)
    {
        plan = newLan(sim, cid, NAT_LAN_IDX_LWIP);
        if (plan != NULL)
        {
            if (lan_create(plan, sim, cid, netif_gprs_nat_lan_lwip_init, nat_lan_lwip_tcpip_input) != true)
                goto end;
            lan_addNATEntry(plan, pwan);
            //pwan->lan_netif[NAT_LAN_IDX_LWIP] = plan;
            if (netif_default == NULL)
                lan_setDefaultNetif(plan->netif);
        }
    }

    iRet = true;
end:
    OSI_LOGI(0x0, "wan_create iRet = %d", iRet);
    return iRet;
}
void wan_destroy(void *wannetif, uint8_t sim, uint8_t cid)
{
    struct wan_netif *pwan = (struct wan_netif *)wannetif;
    struct netif *netif = NULL;
    if (pwan == NULL)
        goto end;
    netif = pwan->netif;
    if (netif != NULL)
    {
#if LWIP_TCPIP_CORE_LOCKING
        LOCK_TCPIP_CORE();
#endif
        drvPsIntf_t *dataPsPath = netif->pspathIntf;
        drvPsIntfClose(dataPsPath);
        netif_set_link_down(netif);
        netif_remove(netif);
        dns_clean_entries();
        if (sim < MAX_SIM_ID && cid < MAX_CID)
        {
            netif->is_used = 0;
        }
        else
        {
            free(netif);
        }
        OSI_LOGI(0x0, "TCPIP_nat_wan_netif_destory netif");
        tcp_debug_print_pcbs();

        pwan->netif = NULL;

#if LWIP_TCPIP_CORE_LOCKING
        UNLOCK_TCPIP_CORE();
#endif
    }
    else
    {
        OSI_LOGI(0x1000754c, "Error ########### CFW_GPRS_DEACTIVED can't find netif for CID=0x%x\n", SIM_CID(sim, cid));
    }
end:
    return;
}
struct wan_netif *wan_getOneActivateWan(void)
{
    struct wan_netif *pwan = NULL;
    uint8_t i, j;

    for (i = 0; i < MAX_SIM_ID; i++)
    {
        for (j = 0; j < MAX_CID; j++)
        {
            if (wan[i][j] != NULL)
            {
                if (wan[i][j]->act == 1)
                {
                    pwan = wan[i][j];
                    break;
                }
            }
        }
    }
    //end:
    OSI_LOGI(0x0, "getOneActivateWan pwan = %p", pwan);
    return pwan;
}
void wan_unbindLan(void *wannetif, uint8_t nSimId, uint8_t nCid)
{
    uint8_t i = 0;
    uint8_t iUnique = 0;
    struct lan_netif *plan = NULL;
    struct lan_netif *act_plan = NULL;
    struct wan_netif *pwan = (struct wan_netif *)wannetif;
    if (pwan == NULL)
        goto end;
    OSI_LOGI(0x0, "wan_unbindLan nSimId = %d, nCid = %d", nSimId, nCid);
    iUnique = wan_isUnique(nSimId, nCid);

    //delete lan entry
    for (i = 0; i < NAT_MAX_LANNUM; i++)
    {
        if ((plan = pwan->lan_netif[i]) != NULL)
        {
            OSI_LOGI(0x0, "pwan->lan_netif[%d] = %p", i, pwan->lan_netif[i]);
            lan_remove2wan(plan, i, pwan);

            if (iUnique == true)
            {
                if (i != NAT_LAN_IDX_RNDIS && i != NAT_LAN_IDX_LWIP) //keep rndis and lwip
                {
                    lan_destroy(plan);
                    OSI_LOGI(0x0, "destroyWan remove lan %d", i);
                    pwan->lan_num--;
                }
                else if (i == NAT_LAN_IDX_RNDIS || i == NAT_LAN_IDX_LWIP)
                    lan_setSimCid(plan, NAT_SIM, NAT_CID);
            }
            else
            {
                //add lan to other activate wan
                struct wan_netif *act_wan = wan_getOneActivateWan();
                if (act_wan->netif != NULL)
                {
                    if ((act_plan = act_wan->lan_netif[i]) == NULL)
                        lan_add2wan(plan, i, act_wan);
                    else if (act_plan != NULL)
                    {
                        if (netif_default == plan->netif || (i == NAT_LAN_IDX_RNDIS))
                        {
                            lan_remove2wan(act_plan, i, act_wan); //??
                            lan_destroy(act_plan);
                            lan_add2wan(plan, i, act_wan);
                            lan_setSimCid(plan, act_wan->netif->sim_cid >> 4, act_wan->netif->sim_cid & 0x0f);
                            pwan->lan_num--;
                        }
                        else if (i != NAT_LAN_IDX_RNDIS) //keep rndis
                        {
                            lan_destroy(plan);
                            OSI_LOGI(0x0, "destroyWan remove lan %d", i);
                            pwan->lan_num--;
                        }
                    }
                }
            }
        }
    }
    OSI_LOGI(0x0, "destroyWan pwan->lan_num %d", pwan->lan_num);
    if (pwan->lan_num == 1)
    {
        for (i = 0; i < NAT_MAX_LANNUM; i++)
        {
            if (netif_default != NULL && pwan->lan_netif[i]->isDefault == 1)
                lan_setDefaultNetif(NULL);
        }
    }
end:
    return;
}
void wan_bindLan(void *wannetif)
{
    struct wan_netif *pwan = (struct wan_netif *)wannetif;
    struct lan_netif *plan = NULL;
    struct netif *wan_netif = pwan->netif;
    uint8_t nSimId, nCid;
    uint8_t iIdx = NAT_MAX_LANNUM;
#if LWIP_IPV6
    ip6_addr_t ip6 = {0};
#endif

    if (wan_netif == NULL)
        goto end;
    else
    {
        nSimId = pwan->netif->sim_cid >> 4;
        nCid = pwan->netif->sim_cid & 0x0f;
    }
    uint8_t T_cid = nSimId << 4 | nCid;

    ip4_addr_t *wan_ip4_addr = (ip4_addr_t *)netif_ip4_addr(wan_netif);
    OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_create wan_ip4_addr 0x%x\n", wan_ip4_addr->addr);
    ip4_addr_t ip4_netMast;
    IP4_ADDR(&ip4_netMast, 255, 255, 255, 0);

    CFW_GPRS_PDPCONT_INFO_V2 pdp_context = {0};
    CFW_GprsGetPdpCxtV2(nCid, &pdp_context, nSimId);
    OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_create pdp_context.PdnType %d\n", pdp_context.PdnType);

    //find lan
    struct netif *lan_netif = NULL;
    NETIF_FOREACH(lan_netif)
    {
        if (lan_netif->pdnType == CFW_GPRS_PDP_TYPE_PPP + 1)
        {
            //set default netif for lwip
            if (netif_default == NULL)
            {
                if (lan_netif->pdnType == CFW_GPRS_PDP_TYPE_PPP + 1 && (lan_netif->link_mode == NETIF_LINK_MODE_NAT_LWIP_LAN))
                {
                    OSI_LOGI(0x0, "netif_set_default %p", lan_netif);
                    netif_set_default(lan_netif); /* found! */
                }
            }

            OSI_LOGI(0x0, "return lan_netif %p, T_cid = %d", lan_netif, T_cid);

            //cid problem
            //if (lan_netif->sim_cid != 0xff)
            lan_netif->sim_cid = T_cid;
            lan_netif->pdnType = pdp_context.PdnType;
#if 0
            //change cid for netif without wan
            ip4_addr_t ip4;
            ip4_addr_t gw;
            ip4_addr_t *netif_ip4 = ip_2_ip4(&(lan_netif->ip_addr));
            IP4_ADDR(&ip4, ip4_addr1(netif_ip4), ip4_addr2(netif_ip4), nCid, ip4_addr4(netif_ip4));
            IP4_ADDR(&gw, ip4_addr1(netif_ip4), ip4_addr2(netif_ip4), nCid, 1);
            ip4_addr_t ip4_netMast;
            IP4_ADDR(&ip4_netMast, 255, 255, 255, 0);

            netif_set_addr(lan_netif, &ip4, &ip4_netMast, &gw);
#endif
            plan = (struct lan_netif *)(lan_netif->pstlan);
            if (plan != NULL)
            {
                if (lan_addNATEntry(lan_netif->pstlan, pwan) == true)
                {
                    if ((iIdx = lan_GetLinkModeIdx(plan)) < NAT_MAX_LANNUM)
                    {
                        pwan->lan_netif[iIdx] = plan;
                        pwan->lan_num++;

                        if (iIdx == NAT_LAN_IDX_LWIP)
                        {
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
                                netif_ip6_addr_set(lan_netif, 0, &ip6);
                                netif_ip6_addr_set_state(lan_netif, 0, IP6_ADDR_VALID);
                                OSI_LOGXI(OSI_LOGPAR_S, 0x0, "TCPIP_nat_lan_lwip_netif_create IP6: %s", ip6addr_ntoa(&ip6));
                            }
#endif
                        }

                        if (iIdx == NAT_LAN_IDX_RNDIS)
                            ND6_Lan_Send_RS(lan_netif, wan_netif);
                    }
                }
            }

            OSI_LOGI(0x0, "pwan->lan_num %d", pwan->lan_num);
        }
    }
end:
    return;
}
struct wan_netif *getWan(uint8_t nSimId, uint8_t nCid)
{
    struct wan_netif *pwan = NULL;
    //check sim cid
    if (nSimId >= MAX_SIM_ID || nCid >= MAX_CID)
        goto end;

    pwan = wan[nSimId][nCid];
end:
    OSI_LOGI(0x0, "getWan pwan = %p", pwan);
    return pwan;
}
static void Wan_init(struct wan_netif *pwan)
{
    memset(pwan, 0, sizeof(struct wan_netif));

    pwan->act = 1;

    OSI_LOGI(0x0, "initWan");
}
struct wan_netif *newWan(uint8_t nSimId, uint8_t nCid)
{
    struct wan_netif *pwan = NULL;

    //check sim cid
    if (nSimId >= MAX_SIM_ID || nCid >= MAX_CID)
        goto end;

    pwan = wan[nSimId][nCid];
    if (pwan == NULL)
    {
        pwan = malloc(sizeof(struct wan_netif));
        if (pwan == NULL)
            goto end;
        wan[nSimId][nCid] = pwan;
    }
    else
    {
        if (wan[nSimId][nCid]->act == 1)
            OSI_LOGI(0x0, "newWan pwan = %p", pwan);
        else
            pwan = wan[nSimId][nCid];
    }

    Wan_init(pwan);
    wan_create(pwan, nSimId, nCid);

end:
    OSI_LOGI(0x0, "newWan pwan = %p", pwan);
    return pwan;
}

void destroyWan(uint8_t nSimId, uint8_t nCid)
{
    //check sim cid
    if (nSimId >= MAX_SIM_ID || nCid >= MAX_CID)
        goto end;

    struct wan_netif *pwan = wan[nSimId][nCid];
    if (pwan == NULL)
        goto end;

    //set act status as deactivate
    pwan->act = 0;

    wan_unbindLan(pwan, nSimId, nCid);
    wan_destroy(pwan, nSimId, nCid);
end:
    OSI_LOGI(0x0, "destroyWan");
    return;
}
#if 0
void TCPIP_nat_wan_netif_addLan(uint8_t nCid, uint8_t nSimId)
{
    //struct netif *netif = NULL;
    uint8_t T_cid = nSimId << 4 | nCid;
    struct netif *wan_netif = netif_get_by_cid_type(T_cid, NETIF_LINK_MODE_NAT_WAN);
    if (wan_netif == NULL)
    {
        return;
    }
    ip4_addr_t *wan_ip4_addr = (ip4_addr_t *)netif_ip4_addr(wan_netif);
    //ip4_addr_t ip4;
    //ip4_addr_t ip4_gw;
    OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_create wan_ip4_addr 0x%x\n", wan_ip4_addr->addr);
    ip4_addr_t ip4_netMast;
    IP4_ADDR(&ip4_netMast, 255, 255, 255, 0);

    CFW_GPRS_PDPCONT_INFO_V2 pdp_context = {0};
    CFW_GprsGetPdpCxtV2(nCid, &pdp_context, nSimId);
    OSI_LOGI(0x0, "TCPIP_nat_lan_lwip_netif_create pdp_context.PdnType %d\n", pdp_context.PdnType);

    //find lan
    struct netif *lan_netif = NULL;
    NETIF_FOREACH(lan_netif)
    {
        if (lan_netif->pdnType == CFW_GPRS_PDP_TYPE_PPP + 1)
        {
            //set default netif for lwip
            if (netif_default == NULL)
            {
                if (lan_netif->pdnType == CFW_GPRS_PDP_TYPE_PPP + 1 && (lan_netif->link_mode == NETIF_LINK_MODE_NAT_LWIP_LAN))
                {
                    OSI_LOGI(0x0, "netif_set_default %p", lan_netif);
                    netif_set_default(lan_netif); /* found! */
                }
            }

            OSI_LOGI(0x0, "return lan_netif %p, T_cid = %d", lan_netif, T_cid);

            lan_netif->sim_cid = T_cid;
            lan_netif->pdnType = pdp_context.PdnType;

            //change cid for netif without wan
            ip4_addr_t ip4;
            ip4_addr_t gw;
            ip4_addr_t *netif_ip4 = ip_2_ip4(&(lan_netif->ip_addr));
            IP4_ADDR(&ip4, ip4_addr1(netif_ip4), ip4_addr2(netif_ip4), nCid, ip4_addr4(netif_ip4));
            IP4_ADDR(&gw, ip4_addr1(netif_ip4), ip4_addr2(netif_ip4), nCid, 1);
            ip4_addr_t ip4_netMast;
            IP4_ADDR(&ip4_netMast, 255, 255, 255, 0);

            netif_set_addr(lan_netif, &ip4, &ip4_netMast, &gw);

            //add nat entry for lan netif
            ip4_nat_entry_t *new_nat_entry = (ip4_nat_entry_t *)calloc(1, sizeof(ip4_nat_entry_t));
            if (new_nat_entry == NULL)
            {
                if (nSimId < MAX_SIM_ID && nCid < MAX_CID)
                {
                    lan_netif->is_used = 0;
                }
            }
            new_nat_entry->in_if = lan_netif;
            new_nat_entry->out_if = wan_netif;
            ip4_addr_copy(new_nat_entry->dest_net, *wan_ip4_addr);
            IP4_ADDR(&(new_nat_entry->dest_netmask), 255, 255, 255, 255);

            netif_ip4 = ip_2_ip4(&(lan_netif->ip_addr));
            ip4_addr_copy(new_nat_entry->source_net, *netif_ip4);
            IP4_ADDR(&(new_nat_entry->source_netmask), 255, 255, 255, 255);
            ip4_nat_add(new_nat_entry);
            lan_netif->nat_entry = new_nat_entry;

            OSI_LOGI(0x0, "return netif->nat_entry %p", lan_netif->nat_entry);
        }
    }
    return;
}
#endif
#endif

#endif
