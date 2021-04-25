/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt
 *
 */
#ifndef LWIP_HDR_LWIPOPTS_H
#define LWIP_HDR_LWIPOPTS_H

#include "stdlib.h"

#include "net_config.h"
#include "lwipports.h"
#include "quec_proj_config.h"

#ifdef CONFIG_NET_LWIP_IPV4_ON
#define LWIP_IPV4_ON
#endif

#ifdef LWIP_IPV6_ON
#define LWIP_IPV6 1
#define LWIP_IPV6_FRAG 1
#else
#ifdef LTE_NBIOT_SUPPORT
#define LWIP_IPV6 1
#define LWIP_IPV6_FRAG 1
#else
#define LWIP_IPV6 0
#endif //LTE_NBIOT_SUPPORT
#endif //LWIP_IPV6_ON

#ifdef LWIP_IPV4_ON
#define LWIP_IPV4 1
#else
#ifdef LTE_NBIOT_SUPPORT
#define LWIP_IPV4 1
#else
#define LWIP_IPV4 0
#endif //LTE_NBIOT_SUPPORT
#endif //LWIP_IPV4_ON

#ifdef CONFIG_NET_NAT_SUPPORT
#define IP_NAT 1
#define IP_NAT_INTERNAL_FORWARD 0
#else
#define IP_NAT 0
#endif

#ifdef CONFIG_NET_LWIP_DEBUG
#define LWIP_DEBUG 1
#define ALTCP_MBEDTLS_DEBUG 1
#else
#define LWIP_DEBUG 0
#define ALTCP_MBEDTLS_DEBUG 0
#endif
#if 1
#define ETHARP_DEBUG 0
#define NETIF_DEBUG 0
#define PBUF_DEBUG 0
#define API_LIB_DEBUG 0
#define API_MSG_DEBUG 0
#define SOCKETS_DEBUG 1
#define ICMP_DEBUG 0
#define IGMP_DEBUG 0
#define INET_DEBUG 0
#define IP_DEBUG 0
#define IP_REASS_DEBUG 0
#define RAW_DEBUG 0
#define MEM_DEBUG 0
#define MEMP_DEBUG 0
#define TCP_INPUT_DEBUG 0
#define TCP_FR_DEBUG 0
#define TCP_RTO_DEBUG 0
#define TCP_CWND_DEBUG 0
#define TCP_WND_DEBUG 0
#define TCP_OUTPUT_DEBUG 0
#define TCP_RST_DEBUG 0
#define TCP_QLEN_DEBUG 0
#define UDP_DEBUG 0
#define TCPIP_DEBUG 1
#define SLIP_DEBUG 0
#define DHCP_DEBUG 0
#define AUTOIP_DEBUG 0
#define IP6_DEBUG 0
#define NAT_DEBUG 0
#endif
#ifdef CONFIG_NET_LWIP_PPP_ON
#define PPP_SUPPORT 1
#else
#define PPP_SUPPORT 0
#endif

#define PPP_SERVER PPP_SUPPORT
#define CCP_SUPPORT 0
#define PPP_PROTOCOLNAME PPP_SUPPORT
#define PRINTPKT_SUPPORT PPP_SUPPORT
#define PPP_NOTIFY_PHASE PPP_SUPPORT
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP
#define PAP_SUPPORT 1
#define CHAP_SUPPORT 1
#else
#define PAP_SUPPORT 0
#define CHAP_SUPPORT 0
#endif
#define EAP_SUPPORT 0
#define MPPE_SUPPORT 0
#define VJ_SUPPORT      0
#define LWIP_USE_EXTERNAL_MBEDTLS PPP_SUPPORT
#define SLIPIF_THREAD_STACKSIZE 8192
#define SLIPIF_THREAD_PRIO OSI_PRIORITY_NORMAL
#define PAP_FOR_SIM_AUTH 1

#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP
#define PPP_AUTHGPRS_SUPPORT 1
#else
#define PPP_AUTHGPRS_SUPPORT 0
#endif

#define LWIP_TCP_KEEPALIVE 1
#ifdef LTE_NBIOT_SUPPORT
#define LWIP_IPV6_DUP_DETECT_ATTEMPTS 0
#endif

#define LWIP_NETIF_LOOPBACK 1
#define LWIP_HAVE_LOOPIF 1

/** LWIP_ALTCP_TLS_MBEDTLS==1: use mbedTLS for TLS support for altcp API
 * mbedtls include directory must be reachable via include search path
 */

#define LWIP_ALTCP_TLS_MBEDTLS 1
/** LWIP_ALTCP==1: enable the altcp API
 * altcp is an abstraction layer that prevents applications linking against the
 * tcp.h functions but provides the same functionality. It is used to e.g. add
 * SSL/TLS or proxy-connect support to an application written for the tcp callback
 * API without that application knowing the protocol details.
 * Applications written against the altcp API are directly linked against the
 * tcp callback API for LWIP_ALTCP==0, but then cannot use layered protocols.
 */

#define LWIP_ALTCP 0
/** LWIP_ALTCP_TLS==1: enable TLS support for altcp API.
 * This needs a port of the functions in altcp_tls.h to a TLS library.
 * A port to ARM mbedtls is provided with lwIP, see apps/altcp_tls/ directory
 * and LWIP_ALTCP_TLS_MBEDTLS option.
 */

#define LWIP_ALTCP_TLS 1

/* Prevent having to link sys_arch.c (we don't test the API layers in unit tests) */
#define NO_SYS 0
//#define LWIP_PROVIDE_ERRNO
#define LWIP_ERRNO_STDINCLUDE
#define SYS_LIGHTWEIGHT_PROT 1
#define LWIP_NETCONN 0
#define LWIP_SOCKET 1
#define LWIP_DNS 1
#define LWIP_RAW 1
#define LWIP_NETIF_API 0
#define LWIP_TCPIP_CORE_LOCKING 1
#define LWIP_TCPIP_CORE_LOCKING_INPUT 1
#define LWIP_ARP 1
#define LWIP_TIMERS 0
#define LWIP_TIMEVAL_PRIVATE 0

/* Enable DHCP to test it, disable UDP checksum to easier inject packets */
#define LWIP_DHCP 1
#define LWIP_IPV6_MLD 1
#ifdef LTE_NBIOT_SUPPORT
#define MEMP_MEM_MALLOC 1
#else
#define MEMP_MEM_MALLOC 1
#endif
#define MEM_LIBC_MALLOC 1
#define mem_clib_free free
#define mem_clib_malloc malloc
#define mem_clib_calloc

//#define malloc COS_MALLOC
#define DEFAULT_UDP_RECVMBOX_SIZE       16
#define DEFAULT_TCP_RECVMBOX_SIZE       32
#define DEFAULT_RAW_RECVMBOX_SIZE       16

#define LWIP_SO_RCVTIMEO 1    //quectel larson.li 2021.01.29   ali_linkSDK support
#define LWIP_SO_RCVBUF 1
#define LWIP_MPU_COMPATIBLE 1

#define SO_REUSE                        1

#define MEM_SIZE 81920

#ifdef CONFIG_QUEC_PROJECT_FEATURE_TCPIP
#define MEMP_NUM_NETCONN  15
#else
#define MEMP_NUM_NETCONN  8
#endif

#define MEMP_NUM_UDP_PCB MEMP_NUM_NETCONN
#define MEMP_NUM_TCP_PCB MEMP_NUM_NETCONN

#define LWIP_SO_LINGER                  1

/* Minimal changes to opt.h required for tcp unit tests: */
#if 0
#define TCP_SND_QUEUELEN 40
#define MEMP_NUM_TCP_SEG TCP_SND_QUEUELEN
#define TCP_SND_BUF (12 * TCP_MSS)
#define TCP_WND (10 * TCP_MSS)
#define LWIP_WND_SCALE 1
#define TCP_RCV_SCALE 0
#define PBUF_POOL_SIZE 40 /* pbuf tests need ~200KByte */
#define MEMP_NUM_SYS_TIMEOUT 16
#else
#define TCP_MSS 1500
#define TCP_WND (16 * TCP_MSS)
#define TCP_SND_BUF (8 * TCP_MSS)
#define MEMP_NUM_TCP_SEG TCP_SND_QUEUELEN
#define LWIP_WND_SCALE 1
#define TCP_RCV_SCALE 0
#define PBUF_POOL_SIZE 40
#endif
#define LWIP_STATS 0
#define LWIP_ICMP 1

#define MEM_ALIGNMENT 4
#define ETH_PAD_SIZE 0

/* Enable IGMP and MDNS for MDNS tests */
#define LWIP_IGMP 0
#define LWIP_MDNS_RESPONDER 0
#define LWIP_NUM_NETIF_CLIENT_DATA (LWIP_MDNS_RESPONDER)

/* Minimal changes to opt.h required for etharp unit tests: */
#define ETHARP_SUPPORT_STATIC_ENTRIES 0

#define USE_CUSTOMER_THREAD !LWIP_TIMERS
#define TCPIP_THREAD_STACKSIZE 8192
#define TCPIP_THREAD_PRIO OSI_PRIORITY_NORMAL

#if !LWIP_DEBUG
#define tcp_debug_print_pcbs()
#define sys_arch_assert(x)
#define sys_arch_printf(format, ...)
#define sys_arch_dump(Data, Size)
#define LWIP_DBG_ALL 0
#define LWIP_NOASSERT 1
#else
extern void tcp_debug_print_pcbs(void);
#define sys_arch_assert(x) do {sys_arch_printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__); fflush(NULL); abort();} while(0)
extern void sys_arch_printf(const char *format, ...) __attribute__((format(printf, 1, 2)));
extern void sys_arch_dump(const void *Data, uint16_t Size);
#define LWIP_DBG_ALL LWIP_DBG_ON
#define TCP_DEBUG_PCB_LISTS 1
#endif

#define LWIP_HOOK_IP4_ROUTE_SRC(dest, src) ip4_src_route(dest, src)
extern struct netif *getEtherNetIf(uint8_t nCid);
extern void sys_post_ICMPevent_to_APP(uint32_t nsocketId, uint32_t nParam1, uint32_t nParam2);
extern uint32_t wifi_IpaddrUpdateInd(struct netif *netif_tmp);
extern uint32_t *getDNSServer(uint8_t nCid, uint8_t nSimID);

#define socklen_t int

#endif /* LWIP_HDR_LWIPOPTS_H */
