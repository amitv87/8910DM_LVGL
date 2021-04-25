/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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
#include "lwip/ip_addr.h"
#include "lwip/ip4.h"
#include "lwip/ip6_addr.h"
#include "lwip/prot/icmp6.h"
#include "lwip/prot/ip.h"
#include "lwip/prot/ip6.h"
#include "lwip/prot/nd6.h"
#include "lwip/inet_chksum.h"
#include "lwip/dns.h"
#include "drv_ether.h"
#include <machine/endian.h>

static u8_t router_advertisement[] = {
    0x60, 0x0f, 0x4e, 0x00, 0x00, 0x58, 0x3a, 0xff,
    0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, /* IP src */
    0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbd, 0xb7, 0x35, 0x25, 0x47, 0x98, 0x99, 0x66, /* IP dst */
    0x86, 0x00, 0x00, 0x00, 0x40, 0x08, 0x0e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ICMPV6 RA*/

    0x01, 0x01, 0x02, 0x4b, 0xb3, 0xb9, 0xeb, 0xe5,                                                 /*ICMPV6 Opt source link-layer addr*/
    0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x05, 0x78,                                                 /*ICMPV6 Opt MTU 1400*/
    0x03, 0x04, 0x40, 0xc0, 0x00, 0x00, 0x0e, 0x10, 0x00, 0x00, 0x0e, 0x10, 0x00, 0x00, 0x00, 0x00, /*ICMPV6 Opt Prefix*/
    0x24, 0x08, 0x84, 0xe3, 0x00, 0x0c, 0xb4, 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*ICMPV6 Opt Prefix addr*/
    0x19, 0x03, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x10,                                                 /*ICMPV6 Opt DNS*/
    0x24, 0x08, 0x84, 0xe3, 0x00, 0x0c, 0xb4, 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa9, /*ICMPV6 Opt DNS addr*/
};

static u8_t Neighbor_advertisement[] = {
    0x60, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3a, 0xff,
    0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, /* IP src */
    0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbd, 0xb7, 0x35, 0x25, 0x47, 0x98, 0x99, 0x66, /* IP dst */
    0x88, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00,                                                 /* ICMPV6 NA*/
    0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, /* ICMPV6 NA Target Addr*/

    0x02, 0x01, 0x86, 0x27, 0x52, 0x72, 0x36, 0xc1, /*ICMPV6 Opt Target link-layer addr*/
};

bool isRouterSolicitationPackage(drvEthPacket_t *pkt, uint32_t size)
{
    bool isIPV6package = false;
    if (__ntohs(pkt->ethhd.h_proto) == ETH_P_IPV6)
    {
        isIPV6package = true;
    }

    if (!isIPV6package)
        return false;

    bool isV6 = false;
    bool isICMP6 = false;
    uint8_t *ipData = pkt->data;
    if (ipData[0] == 0x60) //V6
    {
        isV6 = true;
    }
    if (ipData[6] == 0x3a)
    {
        isICMP6 = true;
    }
    if (!isV6)
        return false;
    if (!isICMP6)
        return false;

    struct icmp6_hdr *icmp_pkg = (struct icmp6_hdr *)(ipData + 40);
    if (icmp_pkg->type == ICMP6_TYPE_RS)
    {
        OSI_LOGI(0x100075a7, "isRSPackage isICMP6 RS");
        return true;
    }
    return false;
}

bool isRouterAdvertisementPackage(drvEthPacket_t *pkt, uint32_t size)
{
    bool isV6 = false;
    bool isICMP6 = false;
    uint8_t *ipData = pkt->data;
    if (ipData[0] == 0x60) //V6
    {
        isV6 = true;
    }
    if (ipData[6] == 0x3a)
    {
        isICMP6 = true;
    }
    if (!isV6)
        return false;
    if (!isICMP6)
        return false;

    struct icmp6_hdr *icmp_pkg = (struct icmp6_hdr *)(ipData + 40);
    if (icmp_pkg->type == ICMP6_TYPE_RA)
    {
        return true;
    }
    return false;
}

void routerAdvertisement_reply(drvEther_t *ether, drvEthPacket_t *pkt, uint32_t size)
{
    drvEthReq_t *tx_req = drvEtherTxReqAlloc(ether);
    if (!tx_req)
        return;

    uint8_t *RS_reply = tx_req->payload->data;
    memcpy(RS_reply, router_advertisement, sizeof(router_advertisement));

    struct ip6_hdr *ip6_pkg_in = (struct ip6_hdr *)(pkt->data);
    struct ip6_hdr *ip6_pkg_out = (struct ip6_hdr *)(RS_reply);

    memcpy(&(ip6_pkg_out->dest), &(ip6_pkg_in->src), sizeof(ip6_addr_p_t));
    ip6_addr_t ip6addr_prefix;
    IP6_ADDR(&ip6addr_prefix, netif_ip6_addr(netif_default, 1)->addr[0], netif_ip6_addr(netif_default, 1)->addr[1], 0, 0);
    memcpy(RS_reply + 88, &(ip6addr_prefix), sizeof(ip6_addr_p_t));

    struct pbuf *p, *q;
    int totalLen = sizeof(router_advertisement) - 40;
    uint8_t *pData = RS_reply;
    int offset = 40;
    p = (struct pbuf *)pbuf_alloc(PBUF_RAW, totalLen, PBUF_POOL);
    struct ra_header *ra_hdr;
    ra_hdr = (struct ra_header *)(pData + 40);
    ra_hdr->chksum = 0;
    if (p != NULL)
    {
        if (totalLen > p->len)
        {
            for (q = p; totalLen > q->len; q = q->next)
            {
                memcpy(q->payload, &(pData[offset]), q->len);
                totalLen -= q->len;
                offset += q->len;
            }
            if (totalLen != 0)
            {
                memcpy(q->payload, &(pData[offset]), totalLen);
            }
        }
        else
        {
            memcpy(p->payload, &(pData[offset]), totalLen);
        }

        ip6_addr_t src_ip = {0};
        ip6_addr_copy_from_packed(src_ip, ip6_pkg_out->src);
        ip6_addr_t dst_ip = {0};
        ip6_addr_copy_from_packed(dst_ip, ip6_pkg_out->dest);

        ra_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, &src_ip, &dst_ip);
        OSI_LOGI(0x100075a8, "RA chksum 0x%x", ra_hdr->chksum);
        pbuf_free(p);
    }

    drvEtherTxReqSubmit(ether, tx_req, sizeof(router_advertisement));
    sys_arch_dump(RS_reply, sizeof(router_advertisement));
}

bool isNeighborAdvertisementPackage(drvEthPacket_t *pkt, uint32_t size)
{
    bool isV6 = false;
    bool isICMP6 = false;
    uint8_t *ipData = pkt->data;
    if (ipData[0] == 0x60) //V6
    {
        isV6 = true;
    }
    if (ipData[6] == 0x3a)
    {
        isICMP6 = true;
    }
    if (!isV6)
        return false;
    if (!isICMP6)
        return false;

    struct icmp6_hdr *icmp_pkg = (struct icmp6_hdr *)(ipData + 40);
    if (icmp_pkg->type == ICMP6_TYPE_NA)
    {
        return true;
    }
    return false;
}

bool isNeighborSolicitationPackage(drvEthPacket_t *pkt, uint32_t size)
{
    bool isIPV6package = false;
    if (__ntohs(pkt->ethhd.h_proto) == ETH_P_IPV6)
    {
        isIPV6package = true;
    }
    if (!isIPV6package)
        return false;

    bool isV6 = false;
    bool isICMP6 = false;
    uint8_t *ipData = pkt->data;
    if (ipData[0] == 0x60) //V6
    {
        isV6 = true;
    }
    if (ipData[6] == 0x3a)
    {
        isICMP6 = true;
    }
    if (!isV6)
        return false;
    if (!isICMP6)
        return false;

    struct icmp6_hdr *icmp_pkg = (struct icmp6_hdr *)(ipData + 40);
    if (icmp_pkg->type == ICMP6_TYPE_NS)
    {
        OSI_LOGI(0x100075a9, "isRSPackage isICMP6 NS");
        return true;
    }
    return false;
}

void neighborAdvertisement_reply(drvEther_t *ether, drvEthPacket_t *pkt, uint32_t size)
{
    drvEthReq_t *tx_req = drvEtherTxReqAlloc(ether);
    if (!tx_req)
        return;

    uint8_t *NS_reply = tx_req->payload->data;
    memcpy(NS_reply, Neighbor_advertisement, sizeof(Neighbor_advertisement));

    struct ip6_hdr *ip6_pkg_in = (struct ip6_hdr *)(pkt->data);
    struct ip6_hdr *ip6_pkg_out = (struct ip6_hdr *)(NS_reply);
    ip6_addr_t src_ip = {0};
    ip6_addr_copy_from_packed(src_ip, ip6_pkg_in->src);
    if (ip6_addr_isany_val(src_ip))
    {
        drvEtherTxReqFree(ether, tx_req);
        return;
    }

    memcpy(&(ip6_pkg_out->dest), &(ip6_pkg_in->src), sizeof(ip6_addr_p_t));

    struct ns_header *ns_hdr = (struct ns_header *)(pkt->data + 40);

    memcpy(&(ip6_pkg_out->src), &(ns_hdr->target_address), sizeof(ip6_addr_p_t));
    memcpy(NS_reply + 48, &(ns_hdr->target_address), sizeof(ip6_addr_p_t));
    memcpy(NS_reply + 80, ether->host_mac, 6); /*ICMPV6 Opt Target link-layer addr*/

    struct pbuf *p, *q;
    int totalLen = sizeof(Neighbor_advertisement) - 40;
    uint8_t *pData = NS_reply;
    int offset = 40;
    p = (struct pbuf *)pbuf_alloc(PBUF_RAW, totalLen, PBUF_POOL);
    struct na_header *na_hdr;
    na_hdr = (struct na_header *)(pData + 40);
    na_hdr->chksum = 0;
    if (p != NULL)
    {
        if (totalLen > p->len)
        {
            for (q = p; totalLen > q->len; q = q->next)
            {
                memcpy(q->payload, &(pData[offset]), q->len);
                totalLen -= q->len;
                offset += q->len;
            }
            if (totalLen != 0)
            {
                memcpy(q->payload, &(pData[offset]), totalLen);
            }
        }
        else
        {
            memcpy(p->payload, &(pData[offset]), totalLen);
        }

        ip6_addr_t src_ip = {0};
        ip6_addr_copy_from_packed(src_ip, ip6_pkg_out->src);
        ip6_addr_t dst_ip = {0};
        ip6_addr_copy_from_packed(dst_ip, ip6_pkg_out->dest);

        na_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, &src_ip, &dst_ip);
        OSI_LOGI(0x100075aa, "NA chksum 0x%x", na_hdr->chksum);
        pbuf_free(p);
    }

    drvEtherTxReqSubmit(ether, tx_req, sizeof(Neighbor_advertisement));
    sys_arch_dump(NS_reply, sizeof(Neighbor_advertisement));
}
//support DHCPv6
static u8_t dhcpv6_info_reply[] = {
    0x60, 0x00, 0x00, 0x00, 0x00, 0x50, 0x11, 0xff,
    0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, /* IP src */
    0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb8, 0xa0, 0x3c, 0x12, 0x10, 0x79, 0x7b, 0xef, /* IP dst */
    0x02, 0x23,
    0x02, 0x22,                                                                                                 /* UDP src dst port */
    0x00, 0x50, 0x00, 0x00,                                                                                     /* check sum */
    0x07,                                                                                                       /* Message Type: Reply */
    0x3f, 0x11, 0x74,                                                                                           /* Transaction ID*/
    0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, 0x00, 0x01, 0x25, 0x66, 0x5a, 0x73, 0xf8, 0xca, 0xb8, 0x37, 0xdf, 0x36, /* Client ID */
    0x00, 0x02, 0x00, 0x0a, 0x00, 0x02, 0x00, 0x00, 0x07, 0xdb, 0x74, 0x4f, 0xd2, 0xcb,                         /* Server ID */
    0x00, 0x17,                                                                                                 /* Option: DNS recursive name server */
    0x00, 0x20,                                                                                                 /* length */
    0x24, 0x08, 0x86, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,             /* DNS address 1 */
    0x24, 0x08, 0x86, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01              /* DNS address 2 */
};
bool ND6_isDhcp6infoReqPackage(drvEthPacket_t *pkt)
{
    OSI_LOGI(0, "isDhcp6infoReqPackage");
    //if (pb->tot_len <= 0)
    //    return false;

    //bool isIPpackage = false;
    //bool isUDP = false;
    uint8_t *ipData = pkt->data;
    OSI_LOGI(0, "isDhcp6infoReqPackage 2");
    if (ipData[0] == 0x60) //V6
    {
        OSI_LOGI(0, "is ip v6");
    }
    else
        return false;

    if (ipData[6] == 0x11) //UDP
    {
        OSI_LOGI(0, "isDhcpPackage cookie isUDP");
        //isUDP = true;
    }
    OSI_LOGI(0, "isDhcp6infoReqPackage 3");
    uint8_t *udpData = ipData + 40;
    bool isUDPPortOK = false;
    if (udpData[0] == 0x02 && udpData[1] == 0x22 && udpData[2] == 0x02 && udpData[3] == 0x23) //port 546->547
    {
        isUDPPortOK = true;
        OSI_LOGI(0, "isDhcpPackage isUDPPortOK");
    }
    OSI_LOGI(0, "isDhcp6infoReqPackage 4");
    if (!isUDPPortOK)
        return false;
    OSI_LOGI(0, "isDhcp6infoReqPackage 5");
    if (udpData[8] == 0x0b) // DHCP info request
        return true;
    return false;
}
void ND6_Dhcp6_Info_req_reply(drvEther_t *ether, drvEthPacket_t *pkt, uint32_t size, struct netif *netif)
{
    drvEthReq_t *tx_req = drvEtherTxReqAlloc(ether);
    if (tx_req == NULL)
        return;
    struct ip6_hdr *ip6hdr = NULL;

    uint8_t *dhcp6_reply = tx_req->payload->data; //p->payload;
    memcpy(dhcp6_reply, dhcpv6_info_reply, sizeof(dhcpv6_info_reply));

    //build src ip and dst ip
    //memcpy(dhcp6_reply + 8, pb->payload + 8 + 16, 16); //src ip
    memcpy(dhcp6_reply + 8 + 16, pkt->data + 8, 16); //dst ip

    //XID CID
    uint8_t *dhcpv6 = dhcp6_reply + 48;
    uint8_t *dhcpv6_ul = pkt->data + 48;

    memcpy(dhcpv6 + 1, dhcpv6_ul + 1, 3);   //Transaction ID
    memcpy(dhcpv6 + 4, dhcpv6_ul + 10, 18); //CID

    //DNS recursive name Server
    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
    OSI_LOGI(0, "dhcpv6 sim = %d, cid = %d", (netif->sim_cid >> 4), (netif->sim_cid & 0x0F));
    CFW_GprsGetPdpCxtV2((netif->sim_cid & 0x0F), &sPdpCont, (netif->sim_cid >> 4));
    ip6_addr_t ip6addr_dns;
    uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[4], sPdpCont.pPdpDns[5], sPdpCont.pPdpDns[6], sPdpCont.pPdpDns[7]));
    uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[8], sPdpCont.pPdpDns[9], sPdpCont.pPdpDns[10], sPdpCont.pPdpDns[11]));
    uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[12], sPdpCont.pPdpDns[13], sPdpCont.pPdpDns[14], sPdpCont.pPdpDns[15]));
    uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[16], sPdpCont.pPdpDns[17], sPdpCont.pPdpDns[18], sPdpCont.pPdpDns[19]));
    uint32_t addr4 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[25], sPdpCont.pPdpDns[26], sPdpCont.pPdpDns[27], sPdpCont.pPdpDns[28]));
    uint32_t addr5 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[29], sPdpCont.pPdpDns[30], sPdpCont.pPdpDns[31], sPdpCont.pPdpDns[32]));
    uint32_t addr6 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[33], sPdpCont.pPdpDns[34], sPdpCont.pPdpDns[35], sPdpCont.pPdpDns[36]));
    uint32_t addr7 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[37], sPdpCont.pPdpDns[38], sPdpCont.pPdpDns[39], sPdpCont.pPdpDns[40]));
    OSI_LOGI(0, "DNS size:%d  ipv6 DNS 12-15:%d.%d.%d.%d", sPdpCont.nPdpDnsSize, sPdpCont.pPdpDns[12], sPdpCont.pPdpDns[13], sPdpCont.pPdpDns[14], sPdpCont.pPdpDns[15]);
    OSI_LOGI(0, " ipv6 DNS 33-36:%d.%d.%d.%d", sPdpCont.pPdpDns[33], sPdpCont.pPdpDns[34], sPdpCont.pPdpDns[35], sPdpCont.pPdpDns[36]);
    IP6_ADDR(&ip6addr_dns, addr0, addr1, addr2, addr3);
    memcpy(dhcpv6 + 40, &(ip6addr_dns), sizeof(ip6_addr_p_t));
    IP6_ADDR(&ip6addr_dns, addr4, addr5, addr6, addr7);
    memcpy(dhcpv6 + 40 + 16, &(ip6addr_dns), sizeof(ip6_addr_p_t));

    //udp checksum
    struct pbuf *pbuf, *q;
    int totalLen = sizeof(dhcpv6_info_reply) - 40;
    uint8_t *pData = dhcp6_reply;
    int offset = 40;
    pbuf = (struct pbuf *)pbuf_alloc(PBUF_RAW, totalLen, PBUF_POOL);
    uint8_t *upd_hdr = (pData + 40);
    u16_t chksum = 0;

    if (pbuf != NULL)
    {
        if (totalLen > pbuf->len)
        {
            for (q = pbuf; totalLen > q->len; q = q->next)
            {
                memcpy(q->payload, &(pData[offset]), q->len);
                totalLen -= q->len;
                offset += q->len;
            }
            if (totalLen != 0)
            {
                memcpy(q->payload, &(pData[offset]), totalLen);
            }
        }
        else
        {
            memcpy(pbuf->payload, &(pData[offset]), totalLen);
        }
        ip6hdr = (void *)(tx_req->payload->data); //p->payload;
        ip6_addr_t src_ip = {0};
        ip6_addr_copy_from_packed(src_ip, ip6hdr->src);
        ip6_addr_t dst_ip = {0};
        ip6_addr_copy_from_packed(dst_ip, ip6hdr->dest);

        chksum = ip6_chksum_pseudo(pbuf, IP6_NEXTH_UDP, pbuf->len, &src_ip, &dst_ip);
        memcpy(((u8_t *)upd_hdr) + 6, &chksum, sizeof(u16_t));
        OSI_LOGI(0, "udp reply chksum 0x%x", chksum);
        pbuf_free(pbuf);
    }

    sys_arch_dump(dhcp6_reply, sizeof(dhcpv6_info_reply));
    drvEtherTxReqSubmit(ether, tx_req, sizeof(dhcpv6_info_reply));
}
bool ND6_isRAPackage(drvEthPacket_t *pkt)
{
    bool isV6 = false;
    bool isICMP6 = false;

    //if (pb->tot_len <= 0)
    //    return false;

    uint8_t *ipData = pkt->data;
    if (ipData[0] == 0x60) //V6
    {
        isV6 = true;
    }
    if (ipData[6] == 0x3a)
    {
        isICMP6 = true;
    }
    if (!isV6)
        return false;
    if (!isICMP6)
        return false;

    struct icmp6_hdr *icmp_pkg = (struct icmp6_hdr *)(ipData + 40);
    if (icmp_pkg->type == ICMP6_TYPE_RA)
    {
        OSI_LOGI(0, "RA package catched %d", icmp_pkg->type);
        return true;
    }
    return false;
}

void ND6_RA_reply(uint8_t *pkt, uint32_t size)
{
    uint8_t *RS_reply = pkt; //pb->payload;
    struct ip6_hdr *ip6_pkg_out = (struct ip6_hdr *)(RS_reply);
    struct pbuf *p, *q;
    int totalLen = size - 40;
    uint8_t *pData = RS_reply;
    int offset = 40;

    p = (struct pbuf *)pbuf_alloc(PBUF_RAW, totalLen, PBUF_POOL);
    struct ra_header *ra_hdr;
    ra_hdr = (struct ra_header *)(pData + 40);
    ra_hdr->chksum = 0;

    ra_hdr->flags = 0x40;

    if (p != NULL)
    {
        if (totalLen > p->len)
        {
            for (q = p; totalLen > q->len; q = q->next)
            {
                memcpy(q->payload, &(pData[offset]), q->len);
                totalLen -= q->len;
                offset += q->len;
            }
            if (totalLen != 0)
            {
                memcpy(q->payload, &(pData[offset]), totalLen);
            }
        }
        else
        {
            memcpy(p->payload, &(pData[offset]), totalLen);
        }

        ip6_addr_t src_ip = {0};
        ip6_addr_copy_from_packed(src_ip, ip6_pkg_out->src);
        ip6_addr_t dst_ip = {0};
        ip6_addr_copy_from_packed(dst_ip, ip6_pkg_out->dest);

        ra_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, &src_ip, &dst_ip);
        OSI_LOGI(0x100075a8, "RA chksum 0x%x", ra_hdr->chksum);
        pbuf_free(p);
    }
    sys_arch_dump(RS_reply, size);
}
//for NAT
struct pbuf *Dhcp6_Info_req_reply(struct netif *netif, struct pbuf *pb)
{
    struct ip6_hdr *ip6hdr = NULL;
    struct pbuf *p = (struct pbuf *)pbuf_alloc(PBUF_RAW, sizeof(dhcpv6_info_reply), PBUF_POOL);
    if (p == NULL)
    {
        return NULL;
    }
    uint8_t *dhcp6_reply = p->payload;
    memcpy(dhcp6_reply, dhcpv6_info_reply, sizeof(dhcpv6_info_reply));

    //build src ip and dst ip
    //memcpy(dhcp6_reply + 8, pb->payload + 8 + 16, 16); //src ip
    memcpy(dhcp6_reply + 8 + 16, pb->payload + 8, 16); //dst ip

    //XID CID
    uint8_t *dhcpv6 = dhcp6_reply + 48;
    uint8_t *dhcpv6_ul = pb->payload + 48;

    memcpy(dhcpv6 + 1, dhcpv6_ul + 1, 3);   //Transaction ID
    memcpy(dhcpv6 + 4, dhcpv6_ul + 10, 18); //CID

    //DNS recursive name Server
    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
    OSI_LOGI(0, "dhcpv6 sim = %d, cid = %d", (netif->sim_cid >> 4), (netif->sim_cid & 0x0F));
    CFW_GprsGetPdpCxtV2((netif->sim_cid & 0x0F), &sPdpCont, (netif->sim_cid >> 4));
    ip6_addr_t ip6addr_dns;
    uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[4], sPdpCont.pPdpDns[5], sPdpCont.pPdpDns[6], sPdpCont.pPdpDns[7]));
    uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[8], sPdpCont.pPdpDns[9], sPdpCont.pPdpDns[10], sPdpCont.pPdpDns[11]));
    uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[12], sPdpCont.pPdpDns[13], sPdpCont.pPdpDns[14], sPdpCont.pPdpDns[15]));
    uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[16], sPdpCont.pPdpDns[17], sPdpCont.pPdpDns[18], sPdpCont.pPdpDns[19]));
    uint32_t addr4 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[25], sPdpCont.pPdpDns[26], sPdpCont.pPdpDns[27], sPdpCont.pPdpDns[28]));
    uint32_t addr5 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[29], sPdpCont.pPdpDns[30], sPdpCont.pPdpDns[31], sPdpCont.pPdpDns[32]));
    uint32_t addr6 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[33], sPdpCont.pPdpDns[34], sPdpCont.pPdpDns[35], sPdpCont.pPdpDns[36]));
    uint32_t addr7 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[37], sPdpCont.pPdpDns[38], sPdpCont.pPdpDns[39], sPdpCont.pPdpDns[40]));
    OSI_LOGI(0, "DNS size:%d  ipv6 DNS 12-15:%d.%d.%d.%d", sPdpCont.nPdpDnsSize, sPdpCont.pPdpDns[12], sPdpCont.pPdpDns[13], sPdpCont.pPdpDns[14], sPdpCont.pPdpDns[15]);
    OSI_LOGI(0, " ipv6 DNS 33-36:%d.%d.%d.%d", sPdpCont.pPdpDns[33], sPdpCont.pPdpDns[34], sPdpCont.pPdpDns[35], sPdpCont.pPdpDns[36]);
    IP6_ADDR(&ip6addr_dns, addr0, addr1, addr2, addr3);
    memcpy(dhcpv6 + 40, &(ip6addr_dns), sizeof(ip6_addr_p_t));
    IP6_ADDR(&ip6addr_dns, addr4, addr5, addr6, addr7);
    memcpy(dhcpv6 + 40 + 16, &(ip6addr_dns), sizeof(ip6_addr_p_t));

    //udp checksum
    struct pbuf *pbuf, *q;
    int totalLen = sizeof(dhcpv6_info_reply) - 40;
    uint8_t *pData = dhcp6_reply;
    int offset = 40;
    pbuf = (struct pbuf *)pbuf_alloc(PBUF_RAW, totalLen, PBUF_POOL);
    uint8_t *upd_hdr = (pData + 40);
    u16_t chksum = 0;

    if (pbuf != NULL)
    {
        if (totalLen > pbuf->len)
        {
            for (q = pbuf; totalLen > q->len; q = q->next)
            {
                memcpy(q->payload, &(pData[offset]), q->len);
                totalLen -= q->len;
                offset += q->len;
            }
            if (totalLen != 0)
            {
                memcpy(q->payload, &(pData[offset]), totalLen);
            }
        }
        else
        {
            memcpy(pbuf->payload, &(pData[offset]), totalLen);
        }
        ip6hdr = p->payload;
        ip6_addr_t src_ip = {0};
        ip6_addr_copy_from_packed(src_ip, ip6hdr->src);
        ip6_addr_t dst_ip = {0};
        ip6_addr_copy_from_packed(dst_ip, ip6hdr->dest);

        chksum = ip6_chksum_pseudo(pbuf, IP6_NEXTH_UDP, pbuf->len, &src_ip, &dst_ip);
        memcpy(((u8_t *)upd_hdr) + 6, &chksum, sizeof(u16_t));
        OSI_LOGI(0, "udp reply chksum 0x%x", chksum);
        pbuf_free(pbuf);
    }

    //ppp_netif_output_ip6(netif, p, NULL);
    //pbuf_free(p);
    //sys_arch_dump(RS_reply, sizeof(router_advertisement));
    sys_arch_dump(dhcp6_reply, p->tot_len);
    return p;
}