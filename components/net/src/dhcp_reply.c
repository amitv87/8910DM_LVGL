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
#include "lwip/ip_addr.h"
#include "lwip/ip4.h"
#include "lwip/prot/dhcp.h"
#include "lwip/prot/udp.h"
#include "lwip/prot/ip.h"
#include "lwip/inet_chksum.h"
#include "lwip/dns.h"
#include "drv_ether.h"
#include <machine/endian.h>

typedef struct arp_packet
{
    uint16_t ar_hrd; /* arp hardware type be16 */
    uint16_t ar_pro; /* arp protocol type be16 */
    uint8_t ar_hln;  /* arp hardware address len */
    uint8_t ar_pln;  /* arp protocol address len */
    uint16_t ar_op;  /* arp opcode(command) be16 */

    /* only for ethernet */
    uint8_t ar_sha[6]; /* arp sender hardware address */
    uint8_t ar_sip[4]; /* arp sender ip address */
    uint8_t ar_tha[6]; /* arp target hardware address */
    uint8_t ar_tip[4]; /* arp target ip address */
} arp_pkt_t;

static uint8_t g_dhcpDiscover_received = 0;

static u8_t dhcp_offer[] = {
    0x45, 0x00, 0x01, 0x48, 0x00, 0x00, 0x00, 0x00, 0x80, 0x11, 0x39, 0xa6, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, /* IP header */
    0x00, 0x43, 0x00, 0x44, 0x01, 0x34, 0x00, 0x00,                                                                         /* UDP header */

    0x02,                                                                                                 /* Type == Boot reply */
    0x01, 0x06,                                                                                           /* Hw Ethernet, 6 bytes addrlen */
    0x00,                                                                                                 /* 0 hops */
    0xAA, 0xAA, 0xAA, 0xAA,                                                                               /* Transaction id, will be overwritten */
    0x00, 0x00,                                                                                           /* 0 seconds elapsed */
    0x00, 0x00,                                                                                           /* Flags (unicast) */
    0x00, 0x00, 0x00, 0x00,                                                                               /* Client ip */
    0xc3, 0xaa, 0xbd, 0xc8,                                                                               /* Your IP */
    0xc3, 0xaa, 0xbd, 0xab,                                                                               /* DHCP server ip */
    0x00, 0x00, 0x00, 0x00,                                                                               /* relay agent */
    0x00, 0x23, 0xc1, 0xde, 0xd0, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* MAC addr + padding */

    /* Empty server name and boot file name */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,

    0x63, 0x82, 0x53, 0x63,             /* Magic cookie */
    0x35, 0x01, 0x02,                   /* Message type: Offer */
    0x36, 0x04, 0xc0, 0xa8, 0x01, 0x01, /* Server identifier (IP) */
    0x33, 0x04, 0x00, 0x00, 0x78, 0x78, /* Lease time 8 days */
    0x03, 0x04, 0x0a, 0x00, 0x00, 0x01, /* Router IP */
    0x01, 0x04, 0xff, 0xff, 0xff, 0x00, /* Subnet mask */
    0x06, 0x04, 0xc0, 0xa8, 0x01, 0x01, /* DNS IP */
    0xff,                               /* End option */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Padding */
};

static u8_t dhcp_ack[] = {
    0x45, 0x00, 0x01, 0x48, 0x00, 0x00, 0x00, 0x00, 0x80, 0x11, 0x39, 0xa6, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, /* IP header */
    0x00, 0x43, 0x00, 0x44, 0x01, 0x34, 0x00, 0x00,                                                                         /* UDP header */
    0x02,                                                                                                                   /* Bootp reply */
    0x01, 0x06,                                                                                                             /* Hw type Eth, len 6 */
    0x00,                                                                                                                   /* 0 hops */
    0xAA, 0xAA, 0xAA, 0xAA,                                                                                                 /* Transaction id, will be overwritten */
    0x00, 0x00,                                                                                                             /* 0 seconds elapsed */
    0x00, 0x00,                                                                                                             /* Flags (unicast) */
    0x00, 0x00, 0x00, 0x00,                                                                                                 /* Client IP */
    0xc3, 0xaa, 0xbd, 0xc8,                                                                                                 /* Your IP */
    0xc3, 0xaa, 0xbd, 0xab,                                                                                                 /* DHCP server IP */
    0x00, 0x00, 0x00, 0x00,                                                                                                 /* Relay agent */
    0x00, 0x23, 0xc1, 0xde, 0xd0, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                   /* Macaddr + padding */

    /* Empty server name and boot file name */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,

    0x63, 0x82, 0x53, 0x63,             /* Magic cookie */
    0x35, 0x01, 0x05,                   /* Dhcp message type ack */
    0x36, 0x04, 0xc0, 0xa8, 0x01, 0x01, /* DHCP server identifier */
    0x33, 0x04, 0x00, 0x00, 0x78, 0x78, /* Lease time 8 days */
    0x03, 0x04, 0x0a, 0x00, 0x00, 0x01, /* Router IP */
    0x01, 0x04, 0xff, 0xff, 0xff, 0x00, /* Netmask */
    0x06, 0x04, 0xc0, 0xa8, 0x01, 0x01, /* DNS IP */
    0xff,                               /* End marker */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Padding */
};

static u8_t dhcp_nak[] = {
    0x45, 0x00, 0x01, 0x48, 0x00, 0x00, 0x00, 0x00, 0x80, 0x11, 0x39, 0xa6, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, /* IP header */
    0x00, 0x43, 0x00, 0x44, 0x01, 0x34, 0x00, 0x00,                                                                         /* UDP header */
    0x02,                                                                                                                   /* Bootp reply */
    0x01, 0x06,                                                                                                             /* Hw type Eth, len 6 */
    0x00,                                                                                                                   /* 0 hops */
    0xAA, 0xAA, 0xAA, 0xAA,                                                                                                 /* Transaction id, will be overwritten */
    0x00, 0x00,                                                                                                             /* 0 seconds elapsed */
    0x00, 0x00,                                                                                                             /* Flags (unicast) */
    0x00, 0x00, 0x00, 0x00,                                                                                                 /* Client IP */
    0xc3, 0xaa, 0xbd, 0xc8,                                                                                                 /* Your IP */
    0xc3, 0xaa, 0xbd, 0xab,                                                                                                 /* DHCP server IP */
    0x00, 0x00, 0x00, 0x00,                                                                                                 /* Relay agent */
    0x00, 0x23, 0xc1, 0xde, 0xd0, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                   /* Macaddr + padding */

    /* Empty server name and boot file name */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,

    0x63, 0x82, 0x53, 0x63,             /* Magic cookie */
    0x35, 0x01, 0x06,                   /* Dhcp message type nak */
    0x36, 0x04, 0xc0, 0xa8, 0x01, 0x01, /* DHCP server identifier */
    0x33, 0x04, 0x00, 0x00, 0x78, 0x78, /* Lease time 8 days */
    0x03, 0x04, 0x0a, 0x00, 0x00, 0x01, /* Router IP */
    0x01, 0x04, 0xff, 0xff, 0xff, 0x00, /* Netmask */
    0x06, 0x04, 0xc0, 0xa8, 0x01, 0x01, /* DNS IP */
    0xff,                               /* End marker */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Padding */
};

#if 0
static u8_t arp_reply[] = {
    0x00, 0x23, 0xc1, 0xde, 0xd0, 0x0d, /* To unit */
    0x00, 0x0f, 0xEE, 0x30, 0xAB, 0x22, /* From remote host */
    0x08, 0x06,                         /* Proto ARP */
    0x00, 0x01,                         /* Hardware type eth */
    0x08, 0x00,                         /* Protocol IPV4 */
    0x06, 0x04,                         /* Hardware size Protocol size*/
    0x00, 0x02,                         /* Opcode reply */
    0xfa, 0x32, 0x47, 0x15, 0xe1, 0x88, /* Sender MAC */
    0xc0, 0xa8, 0x01, 0x01,             /* Sender IP */
    0x08, 0x30, 0x6b, 0xbe, 0x04, 0x1a, /* Target MAC */
    0xc0, 0xa8, 0x01, 0x01,             /* Target IP */
};
#endif

bool isDhcpPackage(drvEthPacket_t *pkt, uint32_t size)
{
    if (size < 266)
        return false;

    const uint8_t brodcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    if (memcmp(pkt->ethhd.h_dest, brodcast, 6) == 0)
    {
        OSI_LOGI(0, "isDhcpPackage isEthBrodcast");
    }
    bool isIPpackage = false;
    if (__ntohs(pkt->ethhd.h_proto) == ETH_P_IP)
    {
        isIPpackage = true;
        //OSI_LOGI(0, "isDhcpPackage isIPpackage");
    }
    if (!isIPpackage)
        return false;

    bool isUDP = false;
    uint8_t *ipData = pkt->data;
    if (ipData[9] == 0x11) //UDP
    {
        OSI_LOGI(0, "isDhcpPackage cookie isUDP");
        isUDP = true;
    }
    if (ipData[16] == 0xff && ipData[17] == 0xff && ipData[18] == 0xff && ipData[19] == 0xff)
    {
        OSI_LOGI(0, "isDhcpPackage isIPBrodcast");
    }
    if (!isUDP)
        return false;

    uint8_t *udpData = pkt->data + 20;
    bool isUDPPortOK = false;
    if (udpData[0] == 0x00 && udpData[1] == 0x44 && udpData[2] == 0x00 && udpData[3] == 0x43) //port 68->67
    {
        isUDPPortOK = true;
        OSI_LOGI(0, "isDhcpPackage isUDPPortOK");
    }
    if (!isUDPPortOK)
        return false;
    struct dhcp_msg *dhcp_pkg = (struct dhcp_msg *)(pkt->data + 20 + 8);
    OSI_LOGI(0, "isDhcpPackage cookie 0x%x", dhcp_pkg->cookie);
    if (dhcp_pkg->cookie == 0x63538263) // DHCP magic cookie
        return true;
    return false;
}

void dhcps_reply(drvEther_t *ether, drvEthPacket_t *pkt, uint32_t size, struct netif *netif)
{
    struct dhcp_msg *dhcp_pkg = (struct dhcp_msg *)(pkt->data + 20 + 8);
    u8_t *options = dhcp_pkg->options;
    uint16_t offset = 0;
    uint16_t offset_max = size - ((uint32_t)options - (uint32_t)dhcp_pkg) - (14 + 20 + 8);
    uint8_t dhcpType = 0;
    ip4_addr_t request_ip = {0};
    OSI_LOGI(0, "dhcps_reply offset_max 0x%x", offset_max);
    while ((offset < offset_max) && (options[offset] != DHCP_OPTION_END))
    {
        u8_t op = options[offset];
        u8_t len;
        len = options[offset + 1];
        OSI_LOGI(0, "dhcps_reply op %x len %d", op, len);
        switch (op)
        {

        case (DHCP_OPTION_MESSAGE_TYPE):
        {
            if (len != 1)
                OSI_LOGI(0, "dhcps_reply DHCP_OPTION_MESSAGE_TYPE len error");
            dhcpType = options[offset + 2];
            offset += (len + 2);
            break;
        }

        case (DHCP_OPTION_REQUESTED_IP):
        {
            if (len != 4)
                OSI_LOGI(0, "dhcps_reply DHCP_OPTION_REQUESTED_IP len error");

            IP4_ADDR(&request_ip, options[offset + 2], options[offset + 3], options[offset + 4], options[offset + 5]);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "dhcps_reply DHCP_OPTION_REQUESTED_IP %s", ip4addr_ntoa(&request_ip));
            offset += (len + 2);
            break;
        }

        default:
        {
            offset += (len + 2);
            break;
        }
        }
    }
    OSI_LOGI(0, "dhcps_reply dhcpType %x", dhcpType);
    if (dhcpType == DHCP_DISCOVER)
    {
        if (netif == NULL)
            return;

        drvEtherSetHostMac(ether, pkt->ethhd.h_source);
        drvEthReq_t *tx_req = drvEtherTxReqAlloc(ether);
        if (!tx_req)
            return;

        uint8_t *dhcp_offer_reply = tx_req->payload->data;
        memcpy(dhcp_offer_reply, dhcp_offer, sizeof(dhcp_offer));
        OSI_LOGI(0, "DHCP reply OFFER");

        int cid = netif->sim_cid & 0x0f;
        int sim = (netif->sim_cid & 0xf0) >> 4;
        struct dhcp_msg *dhcp_pkg_reply = (struct dhcp_msg *)(dhcp_offer_reply + 20 + 8);
        dhcp_pkg_reply->xid = dhcp_pkg->xid;
        memcpy(dhcp_pkg_reply->chaddr, dhcp_pkg->chaddr, DHCP_CHADDR_LEN);
        ip4_addr_t GATE_ip;
        if (netif->is_ppp_mode == 0)
        {
            const ip4_addr_t *your_ip = netif_ip4_addr(netif);
            memcpy(&(dhcp_pkg_reply->yiaddr), your_ip, sizeof(ip4_addr_p_t));
            GATE_ip.addr = your_ip->addr;
        }
        else
        {
            uint8_t length;
            uint8_t addr[THE_PDP_ADDR_MAX_LEN];
            CFW_GprsGetPdpAddr(cid, &length, addr, sim);
            uint32_t ip_addr = (addr[3] << 24) | (addr[2] << 16) | (addr[1] << 8) | addr[0];
            OSI_LOGI(0, "dhcps_reply ppp mode");
            ip4_addr_t your_ip = {0};
            your_ip.addr = ip_addr;
            memcpy(&(dhcp_pkg_reply->yiaddr), &your_ip, sizeof(ip4_addr_p_t));
            GATE_ip.addr = your_ip.addr;
        }

        ip_addr_t DHCP_server_ip = {0};
        IP_ADDR4(&DHCP_server_ip, 192, 168, 1, 1);
        ip4_addr_t *pDHCP_server_ip4 = ip_2_ip4(&DHCP_server_ip);
        memcpy(&(dhcp_pkg_reply->siaddr), pDHCP_server_ip4, sizeof(ip4_addr_p_t));

        memcpy(dhcp_pkg_reply->options + 3 + 6 + 6 + 2, &GATE_ip, sizeof(ip4_addr_p_t));
        if (*((uint8_t *)(&GATE_ip) + 3) != 0x01)
            *(dhcp_pkg_reply->options + 3 + 6 + 6 + 2 + 3) = 0x01;
        else
            *(dhcp_pkg_reply->options + 3 + 6 + 6 + 2 + 3) = 0x02;

        ip_addr_t DHCP_dns_server_ip = {0};
        const ip_addr_t *dns1 = NULL, *dns2 = NULL;
        
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        ip_addr_t dns_server0, dns_server1;
        CFW_GPRS_PDPCONT_INFO_V2 pdp_context;
        CFW_GprsGetPdpCxtV2(cid, &pdp_context, sim);
        dns1 = &dns_server0;
        dns2 = &dns_server1;
        IP_ADDR4(&dns_server0, pdp_context.pPdpDns[0], pdp_context.pPdpDns[1],
                 pdp_context.pPdpDns[2], pdp_context.pPdpDns[3]);
        IP_ADDR4(&dns_server1, pdp_context.pPdpDns[21], pdp_context.pPdpDns[22],
                 pdp_context.pPdpDns[23], pdp_context.pPdpDns[24]);
#else
        dns1 = dns_getserver(0);
        dns2 = dns_getserver(1);
#endif
        if (!ip_addr_isany(dns1))
            ip_addr_copy(DHCP_dns_server_ip, *dns1);
        else if (!ip_addr_isany(dns2))
            ip_addr_copy(DHCP_dns_server_ip, *dns2);
        else
            IP_ADDR4(&DHCP_dns_server_ip, 114, 114, 114, 114);

        ip4_addr_t *pDHCP_dns_server_ip4 = ip_2_ip4(&DHCP_dns_server_ip);
        memcpy(dhcp_pkg_reply->options + 3 + 6 + 6 + 6 + 6 + 2, pDHCP_dns_server_ip4, sizeof(ip4_addr_p_t));

        struct pbuf *p, *q;
        int totalLen = sizeof(dhcp_offer);
        uint8_t *pData = dhcp_offer_reply;
        int offset = 0;
        p = (struct pbuf *)pbuf_alloc(PBUF_RAW, totalLen, PBUF_POOL);
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
            struct udp_hdr *udp_hdr_reply = (struct udp_hdr *)(dhcp_offer_reply + 20);
            ip_addr_t src_ip = {0};
            IP_ADDR4(&src_ip, 0, 0, 0, 0);
            ip_addr_t dst_ip = {0};
            IP_ADDR4(&dst_ip, 255, 255, 255, 255);
            udp_hdr_reply->chksum = ip_chksum_pseudo(p, IP_PROTO_UDP, p->tot_len - 20, &src_ip, &dst_ip);
            OSI_LOGI(0, "UDP chksum 0x%x", udp_hdr_reply->chksum);
            pbuf_free(p);
        }

        drvEtherTxReqSubmit(ether, tx_req, sizeof(dhcp_offer));
        sys_arch_dump(dhcp_offer_reply, sizeof(dhcp_offer));
        g_dhcpDiscover_received = 1;
    }

    if (dhcpType == DHCP_REQUEST)
    {
        drvEthReq_t *tx_req = drvEtherTxReqAlloc(ether);
        if (!tx_req)
            return;

        uint8_t *dhcp_offer_reply = tx_req->payload->data;
        if (netif == NULL)
        {
            drvEtherTxReqFree(ether, tx_req);
            return;
        }

        int cid = netif->sim_cid & 0x0f;
        int sim = (netif->sim_cid & 0xf0) >> 4;
        ip4_addr_t GATE_ip;
        ip4_addr_t your_ip = {0};
        if (netif->is_ppp_mode == 0)
        {
            const ip4_addr_t *netif_ip = netif_ip4_addr(netif);
            ip4_addr_copy(your_ip, *netif_ip);
            ip4_addr_copy(GATE_ip, *netif_ip);
        }
        else
        {
            uint8_t length;
            uint8_t addr[THE_PDP_ADDR_MAX_LEN];
            CFW_GprsGetPdpAddr(cid, &length, addr, sim);
            uint32_t ip_addr = (addr[3] << 24) | (addr[2] << 16) | (addr[1] << 8) | addr[0];
            OSI_LOGI(0, "dhcps_reply ppp mode");
            your_ip.addr = ip_addr;
            GATE_ip.addr = your_ip.addr;
        }
        if (g_dhcpDiscover_received == 0 && (your_ip.addr != request_ip.addr))
        {
            memcpy(dhcp_offer_reply, dhcp_nak, sizeof(dhcp_nak));
            OSI_LOGI(0, "DHCP reply NAK");
        }
        else
        {
            memcpy(dhcp_offer_reply, dhcp_ack, sizeof(dhcp_ack));
            OSI_LOGI(0, "DHCP reply ACK");
            g_dhcpDiscover_received = 0;
        }
        struct dhcp_msg *dhcp_pkg_reply = (struct dhcp_msg *)(dhcp_offer_reply + 20 + 8);
        dhcp_pkg_reply->xid = dhcp_pkg->xid;
        memcpy(dhcp_pkg_reply->chaddr, dhcp_pkg->chaddr, DHCP_CHADDR_LEN);
        memcpy(&(dhcp_pkg_reply->yiaddr), &your_ip, sizeof(ip4_addr_p_t));

        ip_addr_t DHCP_server_ip = {0};
        IP_ADDR4(&DHCP_server_ip, 192, 168, 1, 1);
        ip4_addr_t *pDHCP_server_ip4 = ip_2_ip4(&DHCP_server_ip);
        memcpy(&(dhcp_pkg_reply->siaddr), pDHCP_server_ip4, sizeof(ip4_addr_p_t));
        memcpy(dhcp_pkg_reply->options + 3 + 6 + 6 + 2, &GATE_ip, sizeof(ip4_addr_p_t));
        if (*((uint8_t *)(&GATE_ip) + 3) != 0x01)
            *(dhcp_pkg_reply->options + 3 + 6 + 6 + 2 + 3) = 0x01;
        else
            *(dhcp_pkg_reply->options + 3 + 6 + 6 + 2 + 3) = 0x02;

        ip_addr_t DHCP_dns_server_ip = {0};
        const ip_addr_t *dns1 = NULL, *dns2 = NULL;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        ip_addr_t dns_server0, dns_server1;
        CFW_GPRS_PDPCONT_INFO_V2 pdp_context;
        CFW_GprsGetPdpCxtV2(cid, &pdp_context, sim);
        dns1 = &dns_server0;
        dns2 = &dns_server1;
        IP_ADDR4(&dns_server0, pdp_context.pPdpDns[0], pdp_context.pPdpDns[1],
                 pdp_context.pPdpDns[2], pdp_context.pPdpDns[3]);
        IP_ADDR4(&dns_server1, pdp_context.pPdpDns[21], pdp_context.pPdpDns[22],
                 pdp_context.pPdpDns[23], pdp_context.pPdpDns[24]);
#else
        dns1 = dns_getserver(0);
        dns2 = dns_getserver(1);
#endif
        if (!ip_addr_isany(dns1))
            ip_addr_copy(DHCP_dns_server_ip, *dns1);
        else if (!ip_addr_isany(dns2))
            ip_addr_copy(DHCP_dns_server_ip, *dns2);
        else
            IP_ADDR4(&DHCP_dns_server_ip, 114, 114, 114, 114);
        ip4_addr_t *pDHCP_dns_server_ip4 = ip_2_ip4(&DHCP_dns_server_ip);
        memcpy(dhcp_pkg_reply->options + 3 + 6 + 6 + 6 + 6 + 2, pDHCP_dns_server_ip4, sizeof(ip4_addr_p_t));

        struct pbuf *p, *q;
        int totalLen = sizeof(dhcp_ack);
        uint8_t *pData = dhcp_offer_reply;
        int offset = 0;
        p = (struct pbuf *)pbuf_alloc(PBUF_RAW, totalLen, PBUF_POOL);
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
            struct udp_hdr *udp_hdr_reply = (struct udp_hdr *)(dhcp_offer_reply + 20);
            ip_addr_t src_ip = {0};
            IP_ADDR4(&src_ip, 0, 0, 0, 0);
            ip_addr_t dst_ip = {0};
            IP_ADDR4(&dst_ip, 255, 255, 255, 255);
            udp_hdr_reply->chksum = ip_chksum_pseudo(p, IP_PROTO_UDP, p->tot_len - 20, &src_ip, &dst_ip);
            OSI_LOGI(0, "UDP chksum 0x%x", udp_hdr_reply->chksum);
            pbuf_free(p);
        }
        drvEtherTxReqSubmit(ether, tx_req, sizeof(dhcp_ack));
        sys_arch_dump(dhcp_offer_reply, sizeof(dhcp_ack));
    }
}

bool isARPPackage(drvEthPacket_t *pkt, uint32_t size)
{
    if (__ntohs(pkt->ethhd.h_proto) == ETH_P_ARP)
    {
        OSI_LOGI(0, "isARPPackage");
        return true;
    }
    return false;
}

void ARP_reply(drvEther_t *ether, drvEthPacket_t *pkt, uint32_t size, struct netif *netif)
{
    OSI_LOGI(0, "ARP_reply");
    static uint8_t idx = 0;
    arp_pkt_t *arp_recv = (arp_pkt_t *)(pkt->data);
    if (__ntohs(arp_recv->ar_op) == 0x1) /* arp request */
    {
        drvEthReq_t *tx_req = drvEtherTxReqAlloc(ether);
        if (!tx_req)
            return;

        const uint8_t *client_ip = (uint8_t *)netif_ip4_addr(netif);
        arp_pkt_t *arp_rely = (arp_pkt_t *)(tx_req->payload->data);
        arp_rely->ar_hrd = arp_recv->ar_hrd;
        arp_rely->ar_pro = arp_recv->ar_pro;
        arp_rely->ar_hln = arp_recv->ar_hln;
        arp_rely->ar_pln = arp_recv->ar_pln;
        arp_rely->ar_op = __htons(2);
        memcpy(arp_rely->ar_tip, arp_recv->ar_sip, 4);
        memcpy(arp_rely->ar_tha, arp_recv->ar_sha, 6);
        memcpy(arp_rely->ar_sip, arp_recv->ar_tip, 4);

        if (!memcmp(arp_recv->ar_tip, client_ip, 4))
        {
            memcpy(arp_rely->ar_sha, ether->host_mac, 6);
        }
        else if (!memcmp(arp_recv->ar_tip, client_ip, 3) &&
                 (arp_recv->ar_tip[3] == 1))
        {
            memcpy(arp_rely->ar_sha, ether->dev_mac, 6);
        }
        else
        {
            memcpy(arp_rely->ar_sha, ether->dev_mac, 6);
            arp_rely->ar_sha[5] += ++idx;
        }
        if (!memcmp(arp_recv->ar_tip, client_ip, 4))
        {
            OSI_LOGI(0, "ARP_reply Do Not replay for same dest IP");
            drvEtherTxReqFree(ether, tx_req);
        }
        else
        {
            drvEtherTxReqSubmit(ether, tx_req, sizeof(arp_pkt_t));
            sys_arch_dump(arp_rely, sizeof(arp_pkt_t));
        }
    }
}
