/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "osi_log.h"
#include "cfw_config.h"
#include "net_config.h"
#include "at_cfg.h"
#include "at_cfw.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "netmain.h"
#include "at_engine.h"
#include "ppp_interface.h"
#include "lwipopts.h"
#include "at_parse.h"
#include "cfw_nb_nv_api.h"
#include "cfw_errorcode.h"
#include "at_cmd_gprs.h"
#include "sockets.h"
#include "cfw_chset.h"
#include "drv_ps_path.h"
#include "lwip/netif.h"
#include "atr_config.h"
#include "netutils.h"

#include "cfw.h"
#include "cfw_errorcode.h"

#ifdef CONFIG_AT_DM_LWM2M_SUPPORT
#include "at_cmd_dm_lwm2m.h"
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
#include "ql_nw_internal.h"

#include "quec_atcmd_network.h"
#endif
#include "quec_modem_interface.h"
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP
#include "quec_ppp.h"
#endif
#include "quec_led_task.h"

#include "ql_datacall_internal.h"


const OPER_DEFAULT_APN_INFO OperatorDefaultApnInfo[] =
    {
        {{0x04, 0x06, 0x00, 0x00, 0x00, 0x0f}, "cmnet"},
        {{0x04, 0x06, 0x00, 0x00, 0x00, 0x04}, "cmnet"},
        {{0x04, 0x06, 0x00, 0x00, 0x00, 0x06}, "cmnet"},
        {{0x04, 0x06, 0x00, 0x00, 0x00, 0x07}, "cmnet"},
        {{0x04, 0x06, 0x00, 0x00, 0x02, 0x0f}, "cmnet"},
        {{0x04, 0x06, 0x00, 0x00, 0x04, 0x0f}, "cmnet"},
        {{0x04, 0x06, 0x00, 0x00, 0x07, 0x0f}, "cmnet"},
        {{0x04, 0x06, 0x00, 0x00, 0x08, 0x0f}, "cmnet"},
        {{0x04, 0x06, 0x00, 0x00, 0x01, 0x0f}, "3gnet"},
        {{0x04, 0x06, 0x00, 0x00, 0x06, 0x0f}, "3gnet"},
        {{0x04, 0x06, 0x00, 0x00, 0x03, 0x0f}, "ctnet"},
        {{0x04, 0x06, 0x00, 0x01, 0x01, 0x0f}, "ctnet"},
        {{0x04, 0x06, 0x00, 0x01, 0x03, 0x0f}, "ctnet"},
        {{0x04, 0x06, 0x00, 0x01, 0x05, 0x0f}, "ctnet"},
        {{0}, ""}};

extern uint8_t Mapping_Creg_From_PsType(uint8_t pstype);
uint32_t CFW_GetDefaultApn(OPER_DEFAULT_APN_INFO_V2 nDefaultApn[AT_DEFAULT_APN_MAX_NUM])
{
    memset(&nDefaultApn[0], 0, sizeof(OPER_DEFAULT_APN_INFO_V2) * AT_DEFAULT_APN_MAX_NUM);
    for (int i = 0; i < OSI_ARRAY_SIZE(OperatorDefaultApnInfo); i++)
    {
        memcpy(nDefaultApn[i].OperatorId, OperatorDefaultApnInfo[i].OperatorId, 6);
        memcpy(nDefaultApn[i].Defaultapn, OperatorDefaultApnInfo[i].Defaultapn, strlen((char *)OperatorDefaultApnInfo[i].Defaultapn));
        OSI_LOGXI(OSI_LOGPAR_IS, 0, "CFW_GetDefaultApn: %d %s", i, nDefaultApn[i].Defaultapn);
    }
    return ERR_SUCCESS;
}

uint32_t CFW_GetApnInfo(CFW_APNS_UNAME_UPWD *apn, char *plmn)
{
    OSI_LOGI(0, "CFW_GetApnInfo");

    int low = 0;
    int high = 0;
    int mid = 0;

    high = sizeof(mobileNetworkApns) / sizeof(mobileNetworkApns[0]);
    uint32_t oriValue = 0;
    uint32_t plmnValue = atoi(plmn);

    while (low < high)
    {
        mid = (low + high) / 2;
        oriValue = mobileNetworkApns[mid].plmn;
        if (oriValue == plmnValue)
        {
            goto COPY_APN_INFO;
        }
        else if (oriValue > plmnValue)
        {
            high = mid - 1;
        }
        else if (oriValue < plmnValue)
        {
            low = mid + 1;
        }
    }

    if (high == low)
    {
        if (mobileNetworkApns[low].plmn == plmnValue)
        {
            mid = low;
            goto COPY_APN_INFO;
        }
    }

    return 1;
COPY_APN_INFO:
    memset(apn, 0x00, sizeof(CFW_APNS_UNAME_UPWD));
    memcpy(apn->apn, mobileNetworkApns[mid].apn, strlen(mobileNetworkApns[mid].apn));
    memcpy(apn->username, mobileNetworkApns[mid].username, strlen(mobileNetworkApns[mid].username));
    memcpy(apn->password, mobileNetworkApns[mid].password, strlen(mobileNetworkApns[mid].password));
    return 0;
}

uint8_t CFW_GetDefaultLteCidCtoA()
{
    return CONFIG_AT_GPRS_DEFAULT_LTE_CID;
}

#if LWIP_IPV6
const ip6_addr_t *AT_GprsGetPdpIpv6Address(uint8_t nCid, uint8_t nSim)
{
    const ip6_addr_t *ip6 = NULL;
    struct netif *inp = getGprsNetIf(nSim, nCid);
    if (inp)
    {
        ip6 = ip6_try_to_select_source_address(inp);
        ip6_addr_debug_print(SOCKETS_DEBUG, ip6);
    }
    return ip6;
}
#endif

uint8_t AT_GprsGetActivePdpCount(uint8_t nSim)
{
    uint8_t cnt = 0;
    uint32_t ret = 0;
    uint8_t uState = CFW_GPRS_DEACTIVED;
    for (int uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
    {
        AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[uCid];

        if (pinfo->uCid != 0)
        {
            uState = CFW_GPRS_DEACTIVED;
            ret = CFW_GetGprsActState(pinfo->uCid, &uState, nSim);

            if (CFW_GPRS_ACTIVED == uState && ret == ERR_SUCCESS)
                cnt++;
        }
    }
    return cnt;
}
bool AT_Util_BitStrToByte(uint8_t *byteDest, const char *strSource)
{
    int iLen;
    uint8_t i;
    uint8_t outputByte = 0;

    if (NULL == strSource)
    {
        return false;
    }

    iLen = strlen(strSource);
    if (iLen != 8)
    {
        OSI_LOGI(0, "AT_Util_BitStrToByte: Strlen is NOT 8, [%s] length is %d",
                 strSource, iLen);
        return false;
    }

    for (i = 0; i < iLen; i++)
    {
        if (strSource[i] == '0')
        {
            outputByte = (outputByte << 1);
        }
        else if (strSource[i] == '1')
        {
            outputByte = ((outputByte << 1) | 0x01);
        }
        else
        {
            OSI_LOGI(0, "AT_Util_BitStrToByte: the %dth[%c] is illegal char of [%s]",
                     i, strSource[i], strSource);
            return false;
        }
    }

    *byteDest = outputByte;
    return true;
}

bool AT_Util_4BitStrToByte(uint8_t *byteDest, const char *strSource)
{
    int iLen;
    uint8_t i;
    uint8_t outputByte = 0;

    if (NULL == strSource)
    {
        return false;
    }

    iLen = strlen(strSource);
    if (iLen != 4)
    {
        OSI_LOGI(0, "AT_Util_BitStrToByte: Strlen is NOT 8, [%s] length is %d",
                 strSource, iLen);
        return false;
    }

    for (i = 0; i < iLen; i++)
    {
        if (strSource[i] == '0')
        {
            outputByte = (outputByte << 1);
        }
        else if (strSource[i] == '1')
        {
            outputByte = ((outputByte << 1) | 0x01);
        }
        else
        {
            OSI_LOGI(0, "AT_Util_BitStrToByte: the %dth[%c] is illegal char of [%s]",
                     i, strSource[i], strSource);
            return false;
        }
    }

    *byteDest = outputByte;
    return true;
}

bool AT_Util_IfBitStr(const char *strSource)
{
    int iLen;
    uint8_t i;

    if (NULL == strSource)
    {
        return false;
    }

    iLen = strlen(strSource);
    if (iLen != 8)
    {
        OSI_LOGI(0, "AT_Util_BitStrToByte: Strlen is larger than 8, [%s] length is %d", strSource, iLen);
        return false;
    }

    for (i = 0; i < iLen; i++)
    {
        if ((strSource[i] != '0') && (strSource[i] != '1'))
        {
            return false;
        }
    }
    return true;
    ;
}

bool AT_Util_ByteToBitStr(uint8_t *bitStrDest, uint8_t byteSource, uint8_t bitLen)
{
    uint8_t i = 0;
    uint8_t bit;

    if (NULL == bitStrDest)
    {
        OSI_LOGI(0, "AT_Util_ByteToBitStr: bitStrDest NULL");
        return false;
    }

    if (bitLen > 8 || bitLen == 0)
    {
        OSI_LOGI(0, "AT_Util_ByteToBitStr: bitLen=%d is illegal", bitLen);
        return false;
    }

    for (i = 0; i < bitLen; i++)
    {
        bit = (byteSource >> (bitLen - i - 1)) & 0x01;
        if (bit == 1)
        {
            bitStrDest[i] = '1';
        }
        else if (bit == 0)
        {
            bitStrDest[i] = '0';
        }
    }
    bitStrDest[i] = '\0';
    return true;
}

static const osiValueStrMap_t gPdpTypeVSMap[] = {
    {CFW_GPRS_PDP_TYPE_IP, "IP"},
    {CFW_GPRS_PDP_TYPE_IPV6, "IPV6"},
    {CFW_GPRS_PDP_TYPE_IPV4V6, "IPV4V6"},
#ifndef CONFIG_SOC_6760 //Lte not support PPP referrence 3gpp 24.301 9.9.4.10
    {CFW_GPRS_PDP_TYPE_PPP, "PPP"},
#endif
#if defined(LTE_NBIOT_SUPPORT) || defined(LTE_SUPPORT)
    {CFW_GPRS_PDP_TYPE_NONIP, "Non-IP"},
#endif
    {0, NULL},
};

OSI_UNUSED static osiValueStrMap_t gQosResidualBER[] = {
    {1, "5E2"},
    {2, "1E2"},
    {3, "5E3"},
    {4, "4E3"},
    {5, "1E3"},
    {6, "1E4"},
    {7, "1E5"},
    {8, "1E6"},
    {9, "6E8"},
    {0xf, "0E0"},
    {0, NULL},
};

OSI_UNUSED static osiValueStrMap_t gQosSDUErrorRatio[] = {
    {1, "1E2"},
    {2, "7E3"},
    {3, "1E3"},
    {4, "1E4"},
    {5, "1E5"},
    {6, "1E6"},
    {7, "1E1"},
    {0xf, "0E0"},
    {0, NULL},
};

//extern uint16_t nonIpMtu[7];

extern CFW_GPRS_PDPCONT_INFO *PdpContList[CFW_SIM_COUNT][7];
CFW_NW_STATUS_INFO lastNbPsInfo = {
    .nStatus = 0xff,
};
uint8_t g_DelayPdnAttach = 0;

#define ATTACH_TYPE (1 /*API_PS_ATTACH_GPRS*/ | 8 /*API_PS_ATTACH_FOR*/)

#ifdef PORTING_ON_GOING
uint32_t AT_CFW_GprsAct(uint8_t nState, uint8_t nCid, uint16_t nUTI,
                        CFW_SIM_ID nSimID, osiCallback_t func)
{
    return CFW_GprsActEX(nState, nCid, nUTI, nSimID, 0, func);
}
#endif

// Number of cids waiting for response from network for PDP activation
//uint8_t g_uAtWaitActRsp      = 0;
uint8_t gAtWaitActiveRsp[100 /*MAX_DLC_NUM*/] = {0};

// Attach State
uint8_t gAtAttachState = CFW_GPRS_DETACHED;

uint8_t gAtGprsCidNum = 0;    // yy [add] 2008-6-4 for bug ID 8678
uint8_t gAtGprsCGEVFlag = 0;  // for control auto report +CGEV to TE
uint8_t gAtGprsCGEREPBfr = 0; // for control auto report +CGEV to TE

OSI_UNUSED static bool _paramDotUint8(const char *s, int count, uint8_t *res)
{
    int chars;
    bool first = true;
    unsigned val;
    for (int n = 0; n < count; n++)
    {
        const char *fmt = first ? "%u%n" : ".%u%n";
        if (sscanf(s, fmt, &val, &chars) != 1)
            return false;
        if (val > 255)
            return false;
        *res++ = val;
        s += chars;
        first = false;
    }
    return (*s == '\0') ? true : false;
}

static uint32_t _pdpAddressFromStr(uint8_t pdptype, const char *str)
{
    const char *c1;
    bool addr_isdigit = true;
    for (c1 = str; *c1 != 0; c1++)
    {
        if (*c1 == '.' || *c1 == ':')
        {
            addr_isdigit = false;
            OSI_LOGI(0, "addr_isdigit=%d", addr_isdigit);
        }
    }
    if (addr_isdigit)
    {
        char *endptr1;
        unsigned long addr_digit = strtoul(str, &endptr1, 10);
        OSI_LOGI(0, "priDNS_digit=%lu", addr_digit);
        if (addr_digit >= 0xffffffff)
        {
            OSI_LOGI(0, "addr_digit>=4294967295");
            return ERR_AT_CME_PARAM_INVALID;
        }
    }
    ip_addr_t ip;
    if (ipaddr_aton(str, &ip) == 0)
    {
        OSI_LOGI(0, "ipaddr_aton(str, &ip) == 0");
        return ERR_AT_CME_PARAM_INVALID;
    }
    if (IP_IS_V4(&ip))
    {
        if (ip4_addr_get_u32(ip_2_ip4(&ip)) == 0xffffffff)
        {
            OSI_LOGI(0, "ip4_addr_get_u32(ip_2_ip4(&ip)) == 0xffffffff");
            return ERR_AT_CME_PARAM_INVALID;
        }
    }
    if (IP_IS_V4(&ip) && pdptype != CFW_GPRS_PDP_TYPE_IP)
    {
        return ERR_AT_CME_PARAM_INVALID;
    }
    if (IP_IS_V6(&ip) && pdptype != CFW_GPRS_PDP_TYPE_IPV6)
    {
        return ERR_AT_CME_PARAM_INVALID;
    }
    if (pdptype == CFW_GPRS_PDP_TYPE_IPV4V6)
    {
        return ERR_AT_CME_PARAM_INVALID;
    }
    return ERR_SUCCESS;
}

static void _pdpAddressToStr(const CFW_GPRS_PDPCONT_INFO_V2 *pdp_cont, char *str)
{
    ip4_addr_t ip4 = {0};
#if LWIP_IPV6
    ip6_addr_t ip6 = {0};
#endif
    char str_ipv4[20];
    char str_ipv6[40];

    if ((pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_IP) || (pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_IPV4V6) || (pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_X25) ||
        (pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_PPP)) //add evade for nPdpType == 0, need delete later
    {
        if (pdp_cont->nPdpAddrSize == 0)
            sprintf(str, "IPV4:0.0.0.0");
        else
        {
            IP4_ADDR(&ip4, pdp_cont->pPdpAddr[0], pdp_cont->pPdpAddr[1], pdp_cont->pPdpAddr[2], pdp_cont->pPdpAddr[3]);
            sprintf(str, ip4addr_ntoa(&ip4));
            sprintf(str_ipv4, ip4addr_ntoa(&ip4));
        }
    }

#if LWIP_IPV6
    if (((pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_IPV6) || (pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_IPV4V6)))
    {
        if (pdp_cont->nPdpAddrSize == 0)
            sprintf(str, "IPV6:  ::");
        else
        {
            uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpAddr[4], pdp_cont->pPdpAddr[5], pdp_cont->pPdpAddr[6], pdp_cont->pPdpAddr[7]));
            uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpAddr[8], pdp_cont->pPdpAddr[9], pdp_cont->pPdpAddr[10], pdp_cont->pPdpAddr[11]));
            uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpAddr[12], pdp_cont->pPdpAddr[13], pdp_cont->pPdpAddr[14], pdp_cont->pPdpAddr[15]));
            uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpAddr[16], pdp_cont->pPdpAddr[17], pdp_cont->pPdpAddr[18], pdp_cont->pPdpAddr[19]));
            IP6_ADDR(&ip6, addr0, addr1, addr2, addr3);
            sprintf(str, ip6addr_ntoa(&ip6));
            sprintf(str_ipv6, ip6addr_ntoa(&ip6));
        }
    }
#endif
    if (pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_IPV4V6)
    {
        if (pdp_cont->nPdpAddrSize == 0)
            sprintf(str, "IPV4:0.0.0.0    IPV6:  ::");
        else
        {
            sprintf(str, "IPV4:%s    IPV6:%s", str_ipv4, str_ipv6);
        }
    }
    // TODO: use inet_ntop
}

void _dnsAddressToStr(const CFW_GPRS_PDPCONT_INFO_V2 *pdp_cont, char *str)
{
    ip4_addr_t ip4_dns1 = {0};
    ip4_addr_t ip4_dns2 = {0};
#if LWIP_IPV6
    ip6_addr_t ip6_dns1 = {0};
    ip6_addr_t ip6_dns2 = {0};
#endif
    char str_ipv4[60];
    char str_ipv6[120];

    if ((pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_IP) || (pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_IPV4V6) || (pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_X25) ||
        (pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_PPP)) //add evade for nPdpType == 0, need delete later
    {
        if (pdp_cont->nPdpDnsSize == 0)
            sprintf(str, "IPV4 DNS1:0.0.0.0 IPV4 DNS2:0.0.0.0");
        else
        {
            IP4_ADDR(&ip4_dns1, pdp_cont->pPdpDns[0], pdp_cont->pPdpDns[1], pdp_cont->pPdpDns[2], pdp_cont->pPdpDns[3]);
            IP4_ADDR(&ip4_dns2, pdp_cont->pPdpDns[21], pdp_cont->pPdpDns[22], pdp_cont->pPdpDns[23], pdp_cont->pPdpDns[24]);
            OSI_LOGI(0, "DNS size:%d  DNS1:%d.%d.%d.%d", pdp_cont->nPdpDnsSize,
                     pdp_cont->pPdpDns[0], pdp_cont->pPdpDns[1], pdp_cont->pPdpDns[2], pdp_cont->pPdpDns[3]);
            OSI_LOGI(0, "DNS2:%d.%d.%d.%d",
                     pdp_cont->pPdpDns[21], pdp_cont->pPdpDns[22], pdp_cont->pPdpDns[23], pdp_cont->pPdpDns[24]);
            sprintf(str, "IPV4 DNS1:%s ", ip4addr_ntoa(&ip4_dns1));
            sprintf(str + strlen(str), "IPV4 DNS2:%s", ip4addr_ntoa(&ip4_dns2));
            sprintf(str_ipv4, str);
        }
    }

#if LWIP_IPV6
    if (((pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_IPV6) || (pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_IPV4V6)))
    {
        if (pdp_cont->nPdpAddrSize == 0)
            sprintf(str, "IPV6 DNS1:  :: IPV6 DNS2:  ::");
        else
        {
            uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpDns[4], pdp_cont->pPdpDns[5], pdp_cont->pPdpDns[6], pdp_cont->pPdpDns[7]));
            uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpDns[8], pdp_cont->pPdpDns[9], pdp_cont->pPdpDns[10], pdp_cont->pPdpDns[11]));
            uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpDns[12], pdp_cont->pPdpDns[13], pdp_cont->pPdpDns[14], pdp_cont->pPdpDns[15]));
            uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpDns[16], pdp_cont->pPdpDns[17], pdp_cont->pPdpDns[18], pdp_cont->pPdpDns[19]));
            uint32_t addr4 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpDns[25], pdp_cont->pPdpDns[26], pdp_cont->pPdpDns[27], pdp_cont->pPdpDns[28]));
            uint32_t addr5 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpDns[29], pdp_cont->pPdpDns[30], pdp_cont->pPdpDns[31], pdp_cont->pPdpDns[32]));
            uint32_t addr6 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpDns[33], pdp_cont->pPdpDns[34], pdp_cont->pPdpDns[35], pdp_cont->pPdpDns[36]));
            uint32_t addr7 = PP_HTONL(LWIP_MAKEU32(pdp_cont->pPdpDns[37], pdp_cont->pPdpDns[38], pdp_cont->pPdpDns[39], pdp_cont->pPdpDns[40]));
            OSI_LOGI(0, "DNS size:%d  ipv6 DNS 12-15:%d.%d.%d.%d", pdp_cont->nPdpDnsSize,
                     pdp_cont->pPdpDns[12], pdp_cont->pPdpDns[13], pdp_cont->pPdpDns[14], pdp_cont->pPdpDns[15]);
            OSI_LOGI(0, " ipv6 DNS 33-36:%d.%d.%d.%d",
                     pdp_cont->pPdpDns[33], pdp_cont->pPdpDns[34], pdp_cont->pPdpDns[35], pdp_cont->pPdpDns[36]);
            IP6_ADDR(&ip6_dns1, addr0, addr1, addr2, addr3);
            IP6_ADDR(&ip6_dns2, addr4, addr5, addr6, addr7);
            sprintf(str, "IPV6 DNS1:%s ", ip6addr_ntoa(&ip6_dns1));
            sprintf(str + strlen(str), "IPV6 DNS2:%s", ip6addr_ntoa(&ip6_dns2));
            sprintf(str_ipv6, str);
        }
    }
#endif
    if (pdp_cont->nPdpType == CFW_GPRS_PDP_TYPE_IPV4V6)
    {
        if (pdp_cont->nPdpAddrSize == 0)
        {
            sprintf(str, "IPV4 DNS1:0.0.0.0 IPV4 DNS2:0.0.0.0  IPV6 DNS1:  :: IPV6 DNS2:  ::");
        }
        else
        {
            sprintf(str, "%s  ", str_ipv4);
            sprintf(str + strlen(str), "%s", str_ipv6);
        }
    }
    // TODO: use inet_ntop
}

#if defined(LTE_NBIOT_SUPPORT) || defined(LTE_SUPPORT)
uint8_t AT_Gprs_Mapping_Act_From_PsType(uint8_t pstype);
void Cereg_UtoBinString(uint8_t *string, uint8_t value);
uint8_t AT_Cereg_respond(char *respond, CFW_NW_STATUS_INFO *sStatus, uint8_t stat, uint8_t nSim)
{
    uint8_t ret = 1;
    uint8_t reg = gAtSetting.sim[nSim].cereg;
    uint8_t activeTime[8];
    uint8_t periodicTau[8];
    Cereg_UtoBinString(activeTime, sStatus->activeTime);
    Cereg_UtoBinString(periodicTau, sStatus->periodicTau);
    respond += sprintf(respond, "+CEREG: %d,", reg);
    switch (reg)
    {
    case 0:
        ret = 0;
        break;
    case 1:
        respond += sprintf(respond, "%d", stat);
        break;
    case 2:
    CeregLabel2:
        respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\",%d",
                           stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                           sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                           AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType));
        break;
    case 3:
    CeregLabel3:
        if ((stat == 0) || (stat == 3) || (stat == 4))
        {
            respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\",%d,%d,%d",
                               stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                               sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                               AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType), sStatus->cause_type, sStatus->reject_cause);
        }
        else
        {
            goto CeregLabel2;
        }
        break;
    case 4:
    CeregLabel4:
        if ((sStatus->activeTime != 0xff) && (sStatus->periodicTau != 0xff))
        {

            respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,,,\"%c%c%c%c%c%c%c%c\",\"%c%c%c%c%c%c%c%c\"",
                               stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                               sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                               AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType),
                               activeTime[0], activeTime[1], activeTime[2], activeTime[3],
                               activeTime[4], activeTime[5], activeTime[6], activeTime[7],
                               periodicTau[0], periodicTau[1], periodicTau[2], periodicTau[3],
                               periodicTau[4], periodicTau[5], periodicTau[6], periodicTau[7]);
        }
        else if (sStatus->activeTime != 0xff)
        {

            respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,,,\"%c%c%c%c%c%c%c%c\"",
                               stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                               sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                               AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType),
                               activeTime[0], activeTime[1], activeTime[2], activeTime[3],
                               activeTime[4], activeTime[5], activeTime[6], activeTime[7]);
        }
        else if (sStatus->periodicTau != 0xff)
        {

            respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,,,,\"%c%c%c%c%c%c%c%c\"",
                               stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                               sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                               AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType),
                               periodicTau[0], periodicTau[1], periodicTau[2], periodicTau[3],
                               periodicTau[4], periodicTau[5], periodicTau[6], periodicTau[7]);
        }
        else
        {
            goto CeregLabel2;
        }
        break;
    case 5:
        if ((stat == 0) || (stat == 3) || (stat == 4))
        {
            if ((sStatus->activeTime != 0xff) && (sStatus->periodicTau != 0xff))
            {

                respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,%d,%d,\"%c%c%c%c%c%c%c%c\",\"%c%c%c%c%c%c%c%c\"",
                                   stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                                   sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                                   AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType), sStatus->cause_type, sStatus->reject_cause,
                                   activeTime[0], activeTime[1], activeTime[2], activeTime[3],
                                   activeTime[4], activeTime[5], activeTime[6], activeTime[7],
                                   periodicTau[0], periodicTau[1], periodicTau[2], periodicTau[3],
                                   periodicTau[4], periodicTau[5], periodicTau[6], periodicTau[7]);
            }
            else if (sStatus->activeTime != 0xff)
            {

                respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,%d,%d,\"%c%c%c%c%c%c%c%c\"",
                                   stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                                   sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                                   AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType), sStatus->cause_type, sStatus->reject_cause,
                                   activeTime[0], activeTime[1], activeTime[2], activeTime[3],
                                   activeTime[4], activeTime[5], activeTime[6], activeTime[7]);
            }
            else if (sStatus->periodicTau != 0xff)
            {

                respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,%d,%d,,\"%c%c%c%c%c%c%c%c\"",
                                   stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                                   sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                                   AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType), sStatus->cause_type, sStatus->reject_cause,
                                   periodicTau[0], periodicTau[1], periodicTau[2], periodicTau[3],
                                   periodicTau[4], periodicTau[5], periodicTau[6], periodicTau[7]);
            }
            else
            {
                goto CeregLabel3;
            }
        }
        else
        {
            goto CeregLabel4;
        }
        break;
    default:
        ret = 2;
        break;
    }
    return ret;
}
#endif

uint8_t DlciGetCid(uint8_t nDLCI)
{
#if PORTING_ON_GOING
    OSI_LOGXI(OSI_LOGPAR_SI, 0x100041e6, "%s, nDLCI = %d,", __func__, nDLCI);
    int i = AT_PDPCID_MIN;
    uint8_t nSim = AT_SIM_ID(nDLCI);
    AT_Gprs_CidInfo *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
#ifdef LTE_NBIOT_SUPPORT
    uint8_t uCidState = 0;
#endif
    for (i = AT_PDPCID_MIN; i <= AT_PDPCID_MAX; i++)
    {
        // if (g_staAtGprsCidInfo[i].nDLCI == nDLCI)
        // {
        //     AT_TC(g_sw_GPRS, "%s, nDLCI = %d,cid=%d", __func__, nDLCI, g_staAtGprsCidInfo[i].uCid);
        //     return g_staAtGprsCidInfo[i].uCid;
        // }
#ifdef LTE_NBIOT_SUPPORT
        CFW_GetGprsActState(g_staAtGprsCidInfo[i].uCid, &uCidState, nSim);
        if ((uCidState == CFW_GPRS_ACTIVED) && (g_staAtGprsCidInfo[i].uState == 1))
            return g_staAtGprsCidInfo[i].uCid;
#else
        if (g_staAtGprsCidInfo[i].uState == 1)
        {
            return g_staAtGprsCidInfo[i].uCid;
        }
#endif
    }
#endif
    return 0;
}
uint8_t AT_CgregRespond(char *respond, CFW_NW_STATUS_INFO *sStatus, uint8_t nSim)
{
    uint8_t ret = 1;
    uint8_t reg = gAtSetting.sim[nSim].cgreg;

    respond += sprintf(respond, "+CGREG: ");
    switch (reg)
    {
    case 0:
        ret = 0;
        break;
    case 1:
        respond += sprintf(respond, "%d", sStatus->nStatus);
        break;
    case 2:
        respond += sprintf(respond, "%d,\"%02X%02X\",\"%02X%02X\"",
                           sStatus->nStatus,
                           sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                           sStatus->nCellId[0], sStatus->nCellId[1]);
        break;
#ifdef CONFIG_SOC_8910
    case 3:
        respond += sprintf(respond, "%d,\"%02X%02X\",\"%02X%02X\",%d,%d,%d",
                           sStatus->nStatus,
                           sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                           sStatus->nCellId[0], sStatus->nCellId[1], Mapping_Creg_From_PsType(sStatus->PsType),
                           sStatus->cause_type, sStatus->reject_cause);
        break;
#endif
    default:
        ret = 2;
        break;
    }
    return ret;
}

#ifdef LTE_NBIOT_SUPPORT
static void AT_AutoPdnAttach(uint8_t dlci, CFW_SIM_ID nSim)
{
#ifdef PORTING_ON_GOING
    AT_CMD_ENGINE_T *th = at_CmdGetByChannel(dlci);
    AT_ModuleInfo *module = at_CmdGetModule(th);
    AT_CMD_BUF *pCmdBuf = &module->cmd_buf;
    if (module->cmds != NULL)
    {
        g_DelayPdnAttach = 1;
        return;
    }
    g_DelayPdnAttach = 0;
    if (nbiot_nvGetPdnAutoAttach() == 1)
    {
        uint8_t pdn_auto_attach[] = {'a', 't', '+', 'c', 'g', 'd', 'c', 'o', 'n', 't', '=', '5', ',', '"', 'I', 'P', 'V', '4', 'V', '6', '"', ';', '+', 'c', 'g', 'a', 'c', 't', '=', '1', ',', '5', '\r', '\0'};
        uint8_t pdn_auto_attach1[] = {'a', 't', '+', 'c', 'g', 'd', 'c', 'o', 'n', 't', '=', '5', ',', '"', 'N', 'o', 'n', '-', 'I', 'P', '"', ';', '+', 'c', 'g', 'a', 'c', 't', '=', '1', ',', '5', '\r', '\0'};
        uint8_t uCidState = 0;
        extern uint8_t esm11_64getPdnConnectioNum();
        uint8_t nasPdnCount = esm11_64getPdnConnectioNum();
        uint16_t length = 0;
        CFW_NW_STATUS_INFO sStatus;
        uint32_t ret;
        ret = CFW_GprsGetstatus(&sStatus, nSim);
        if (ret == ERR_SUCCESS)
        {
            OSI_LOGI(0x100052af, "nwStatus/nasPdnCount: %d/%d", sStatus.nStatus, nasPdnCount);
            if (((sStatus.nStatus == 1) || (sStatus.nStatus == 5)) && (nasPdnCount > 0))
            {
                ret = CFW_GetGprsActState(5, &uCidState, nSim);
                if (uCidState == 0 && ret == ERR_SUCCESS)
                {
                    if (nbiot_nvGetDefaultPdnType() == 5)
                    {
                        length = strlen(pdn_auto_attach1);
                        at_memcpy(module->cmd_buf.DataBuf, pdn_auto_attach1, length);
                    }
                    else
                    {
                        length = strlen(pdn_auto_attach);
                        at_memcpy(module->cmd_buf.DataBuf, pdn_auto_attach, length);
                    }
                    pCmdBuf->DataBuf[length] = '\0';
                    pCmdBuf->DataLen = length;
                    int result = at_ModuleProcessLine(pCmdBuf->DataBuf, pCmdBuf->DataLen, th);
                    if (result != 0) // parser error
                    {
                        //OSI_LOGXI(OSI_LOGPAR_SI, 0x100000e4, "AT CMD%s,result%d", pCmdBuf->DataBuf, result);
                        module->cmd_buf_bak.DataLen = 0;
                        at_CmdlistFreeAll(&module->cmds);
                        return;
                    }
                    at_ModuleRunCommand(th);
                    at_CmdSetLineMode(th);
                }
            }
        }
    }
#endif
}

void AT_TryAutoPdnAttach(uint8_t dlci, CFW_SIM_ID nSim)
{
    if (g_DelayPdnAttach == 1)
        AT_AutoPdnAttach(dlci, nSim);
}

#endif

#if defined(CFW_GPRS_SUPPORT) //&& !defined(AT_WITHOUT_GPRS)
static OSI_UNUSED void PsDownlinkDataHandler(CFW_GPRS_DATA *pGprsData, uint8_t cid, uint8_t nDLCI)
{
#ifdef PORTING_ON_GOING
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDLCI);
    at_CmdWrite(engine, pGprsData->pData, pGprsData->nDataLength);
#endif
}
#endif

// ---- ------------temp area begin --------------
// [[ yy [del] 2008-7-1 for CSW update
// [[ yy [del] 2008-7-1 for CSW update

static void _onEV_CFW_GPRS_CXT_ACTIVE_IND(const osiEvent_t *event)
{
    OSI_LOGI(0x100041ee, "We got EV_CFW_GPRS_CXT_ACTIVE_IND");
    osiEvent_t tcpip_event = *event;
    tcpip_event.id = EV_TCPIP_CFW_GPRS_ACT;
    CFW_EVENT *tcpip_cfw_event = (CFW_EVENT *)&tcpip_event;
    tcpip_cfw_event->nUTI = 0;
    osiEventSend(netGetTaskID(), (const osiEvent_t *)&tcpip_event);
#ifdef PORTING_ON_GOING
    OSI_LOGI(0x100041ee, "We got EV_CFW_GPRS_CXT_ACTIVE_IND");
    if (0xF0 == cfw_event.nType)
    {
        atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        break;
    }

    // Get matching pdp contex.
    nCid = cfw_event.nParam1;
    pEvent->nParam1 = 0;
    OSI_LOGI(0x100041ef, "KEN :: the mathing cid = %d", nCid);
    if (nCid == AT_PDPCID_ERROR)
    {
        atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        break;
    }
    if (AUTO_RSP_STU_OFF_PDONLY == gATCurrentu8nURCAuto)
    {
        SUL_StrPrint(buffer, "A activae request of cid=<%d> has araising\n Please type AT+CGANS=1 accept...\n", nCid);
        at_CmdReportUrcText(nSim, buffer);
    }
    else
    {
        nS0Staus = AT_GC_CfgGetS0Value();
        if (AUTO_RSP_STU_ON_PDONLY == gATCurrentu8nURCAuto ||
            (nS0Staus > 0 &&
             (AUTO_RSP_STU_MOD_CAP_PD_CS == gATCurrentu8nURCAuto ||
              AUTO_RSP_STU_MOD_CAP_PDONLY == gATCurrentu8nURCAuto)))
        {
            uUIT = 200 + 6 + nCid;
            iResult = AT_CFW_GprsAct(AT_GPRS_ACT, nCid, cfw_event.nUTI, nSim, (COS_CALLBACK_FUNC_T)AT_GPRS_AsyncEventProcess);
            OSI_LOGI(0x100041f0, "KEN :: AUTO_RSP_STU_ON_PDONLY cid = %d, AT_CFW_GprsAct result = 0X%8x", nCid, iResult);
            if (0 != iResult)
            {
                atCmdRespCmeError(engine, ERR_AT_CME_EXE_NOT_SURPORT);
                break;
            }

            if (gAtWaitActiveRsp[cfw_event.nUTI] > 0)
            {
                gAtWaitActiveRsp[cfw_event.nUTI]--;
            }

            OSI_LOGI(0x100041f1, "KEN :: Asyn g_uAtWaitActRsp = %d", gAtWaitActiveRsp[cfw_event.nUTI]);
        }
    }
#endif
}

extern bool at_bip_channel_status_event(uint8_t nSimId);
static void _onEV_CFW_GPRS_CXT_DEACTIVE_IND(const osiEvent_t *event)
{
    uint8_t nCid, nSimId;
    CFW_EVENT *cfw_event = (CFW_EVENT *)event;
    nCid = event->param1;
    nSimId = cfw_event->nFlag;
    OSI_LOGI(0x0, "We got EV_CFW_GPRS_CXT_DEACTIVE_IND cid %d simid %d", nCid, nSimId);
    at_bip_channel_status_event(nSimId);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
	//marvin.ning: 非VoLTE下,由网络异常导致的PDP去激活才输出URC
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOLTE
	if (((CFW_ImsIsSet(nSimId) == 1) && (CFW_NWGetStackRat(nSimId) == 4) && (nCid <= PROFILE_IDX_VOLTE_MAX) ) || //4G网络开启volte,但cid小于PROFILE_IDX_VOLTE_MAX
	     ((CFW_ImsIsSet(nSimId) == 0) && (CFW_NWGetStackRat(nSimId) == 4)) ||//4G网络,但未开启volte
	    (CFW_NWGetStackRat(nSimId) == 2))//2G网络
#endif
	{
		extern void ql_datacall_pdp_deactive_handler(uint8_t simId, uint8_t cid);
		ql_datacall_pdp_deactive_handler(nSimId, nCid);
	}
#endif

#ifdef LTE_NBIOT_SUPPORT
    if (nCid >= 1 && nCid <= 7 && gAtCfwCtx.sim[nSimId].cid_info[nCid].uPCid == IMS_PDP_CID)
    {
        gAtCfwCtx.sim[nSimId].cid_info[nCid].uPCid = 0;
    }
#endif
#if 0
    if (nCid >= 1 && nCid <= 7)
    {
        memset(&gAtCfwCtx.sim[nSimId].cid_info[nCid], 0x00, sizeof(AT_Gprs_CidInfo));
    }
    CFW_GprsRemovePdpCxt(nCid, nSimId);
#endif
#if 0
    pppSessionDeleteBySimCid(nSimId, nCid);
#else
    //Delete ppp Session first
    OSI_LOGI(0, "ppp session will be deleted!!! at CFW_GPRS_CXT_DEACTIVE_IND");
    pppSessionDeleteByNetifDestoryed(nSimId, nCid);

    osiEvent_t tcpip_event = *event;
    tcpip_event.id = EV_TCPIP_CFW_GPRS_DEACT;
    CFW_EVENT *tcpip_cfw_event = (CFW_EVENT *)&tcpip_event;
    tcpip_cfw_event->nUTI = 0;
    OSI_LOGI(0x0, "Send EV_TCPIP_CFW_GPRS_DEACT to NetThread!!!!");
    osiEventSend(netGetTaskID(), (const osiEvent_t *)&tcpip_event);
#endif
#if 0
#ifdef 	CONFIG_QUEC_PROJECT_FEATURE_TCPIP_AT
    {
		extern void at_tcpip_pdp_auto_deact_ind(uint8_t cid);
		at_tcpip_pdp_auto_deact_ind(nCid);
	}
#endif /*CONFIG_QUEC_PROJECT_FEATURE_TCPIP_AT*/
#endif

#ifdef PORTING_ON_GOING
    OSI_LOGI(0x100041f2, "We got EV_CFW_GPRS_CXT_DEACTIVE_IND g_uAtGprsCidNum %d", gAtGprsCidNum);
    if (gAtGprsCidNum)
    {
        gAtGprsCidNum--;
    }
    if (gAtWaitActiveRsp[cfw_event.nUTI] > 0)
    {
        gAtWaitActiveRsp[cfw_event.nUTI]--;
    }
#if 0
#ifdef LTE_NBIOT_SUPPORT
    if (cfw_event.nParam1 >= 1 && cfw_event.nParam1 <= 7)
    {
        memset(&g_staAtGprsCidInfo[cfw_event.nParam1], 0x00, sizeof(AT_Gprs_CidInfo));
    }
#else
    if (cfw_event.nParam1 >= 1 && cfw_event.nParam1 <= 7)
    {
        memset(&g_staAtGprsCidInfo[cfw_event.nUTI], 0x00, sizeof(AT_Gprs_CidInfo));
    }
#endif
#endif

    if (gAtGprsCGEVFlag == 1)
    {
        uint8_t nResp[50] = {
            0x00,
        };
        sprintf(nResp, "+CGEV: NW DEACT  \"IP\", ,%d", cfw_event.nParam1);

        OSI_LOGI(0x100041f2, "We got EV_CFW_GPRS_CXT_DEACTIVE_IND g_uAtGprsCidNum %d", gAtGprsCidNum);
        at_CmdReportUrcText(nSim, nResp);
        pEvent->nParam1 = 0;
    }

#if defined(AT_CMUX_SUPPORT) && defined(AT_PING_SUPPORT)
    extern void ping_timeout(uint32_t * param);
    extern bool ping_socket_is_invalid();
    extern void ping_result_statics(uint8_t nDLC, uint8_t * Response);
    uint8_t dlci = AT_ASYN_GET_DLCI(nSim);
    if (!ping_socket_is_invalid())
    {
        uint8_t reponse[100] = {0};
        COS_StopCallbackTimer(CSW_AT_TASK_HANDLE, ping_timeout, dlci);
        ping_result_statics(dlci, reponse);
    }
#endif
#endif
}

static void _onEV_CFW_GPRS_MOD_IND(const osiEvent_t *event)
{
#ifdef PORTING_ON_GOING
    CFW_GprsCtxModifyAcc(AT_ASYN_GET_DLCI(nSim), pEvent->nParam1, nSim);
    pEvent->nParam1 = 0;

    uint8_t nResp[30] = {
        0x00,
    };
    sprintf(nResp, "+CGDMODIND:%d", pEvent->nParam1);
    at_CmdReportUrcText(nSim, nResp);
#endif
}

static void _onEV_CFW_GPRS_STATUS_IND(const osiEvent_t *event)
{
    CFW_EVENT cfw_event = *(const CFW_EVENT *)event;
    uint8_t nSim = cfw_event.nFlag;
    uint8_t PsStatus = (cfw_event.nParam1 & 0xff);
#ifndef CONFIG_QUEC_PROJECT_FEATURE_NW
    char respond[100] = {0};
#endif

    OSI_LOGI(0x100041ea, "We got 111 EV_CFW_GPRS_STATUS_IND, cereg:%d, cgreg: %d", gAtSetting.sim[nSim].cereg, gAtSetting.sim[nSim].cgreg);
#if defined(LTE_NBIOT_SUPPORT) || defined(LTE_SUPPORT)
    CFW_NW_STATUS_INFO sStatus;
    uint8_t nCurrRat = CFW_NWGetStackRat(nSim);
    OSI_LOGI(0, "cfw_event.nParam2: %x,%d", cfw_event.nParam2, nCurrRat);
#ifndef CONFIG_QUEC_PROJECT_FEATURE_NW
    uint32_t ret = CFW_GprsGetstatus(&sStatus, nSim);
#endif

    if (CFW_NWGetStackRat(nSim) == 4 || cfw_event.nParam2 == 0xFFFFFFF4) //nbiot&lte branch
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
		ql_nw_send_event(QUEC_NW_DATA_REG_STATUS_IND, nSim, 0, 0);

		//for AT
        quec_deal_nw_gprs_status_ind(GPRS_IND, &cfw_event); //report CGREG in LTE network
        quec_deal_nw_gprs_status_ind(EPS_IND, &cfw_event);
        quec_deal_ratchg_ind(0, CFW_RAT_LTE_ONLY, PsStatus, nSim);

		(void)(sStatus);	//resolve the warning that unused variable "sStatus"
        gAtCfwCtx.sim[nSim].cereg_val = PsStatus;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_USBNET
        quec_modem_attach_notification(PsStatus);
#endif
#else
        if (ret == ERR_SUCCESS)
        {
            if (1 != AT_Cereg_respond(respond, &sStatus, PsStatus, nSim))
            {
                OSI_LOGI(0x100052b0, "gAtSetting.sim[nSim].cereg: %d", gAtSetting.sim[nSim].cereg);
                return;
            }
            atCmdRespSimUrcText(nSim, respond);
            if (1 == PsStatus)
                atCmdRespSimUrcText(nSim, "+E_UTRAN Service");
            gAtCfwCtx.sim[nSim].cereg_val = PsStatus;
        }
#endif        
    }
    else
#endif
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
		ql_nw_send_event(QUEC_NW_DATA_REG_STATUS_IND, nSim, 0, 0);

		//for AT
        quec_deal_nw_gprs_status_ind(GPRS_IND, &cfw_event);
        quec_deal_ratchg_ind(0, CFW_RAT_GSM_ONLY, PsStatus, nSim);

        gAtCfwCtx.sim[nSim].cgreg_val = PsStatus;
#else
        OSI_LOGI(0, "Rat cgreg : %d", gAtSetting.sim[nSim].cgreg);
        if (1 != AT_CgregRespond(respond, &sStatus, nSim))
        {
            OSI_LOGI(0, "gAtSetting.sim[nSim].cgreg: %d", gAtSetting.sim[nSim].cgreg);
            return;
        }
        atCmdRespSimUrcText(nSim, respond);

        if (1 == PsStatus)
            atCmdRespSimUrcText(nSim, "+GSM Service");
        gAtCfwCtx.sim[nSim].cgreg_val = PsStatus;
#endif
    }
}

#ifdef LTE_NBIOT_SUPPORT
#ifdef PORTING_ON_GOING
static void _onEV_CFW_GPRS_MOD_RSP(const osiEvent_t *event)
{
    OSI_LOGI(0x100052b1, "At get EV_CFW_GPRS_MOD_RSP,cfw_event.nParam1 is 0x%x, cfw_event.nParam2 is 0x%x", cfw_event.nParam1, cfw_event.nParam2);
    if (cfw_event.nParam2 == 0)
    {
        atCmdRespOK(at_CmdGetByChannel(cfw_event.nUTI));
    }
    else
    {
        atCmdRespCmeError(at_CmdGetByChannel(cfw_event.nUTI), ERR_AT_CME_EXE_FAIL);
    }
}
#endif

static void _onEV_CFW_GPRS_APNCR_IND(const osiEvent_t *event)
{
#ifdef PORTING_ON_GOING
    // AT_GprsSaveApnRateParams(&cfw_event);
    uint8_t nSim = cfw_event->nFlag;
    AT_Gprs_CidInfo *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    uint8_t state;
    OSI_LOGI(0x100041e7, "We got EV_CFW_GPRS_APNCR_IND");
    uint8_t cid = ((cfw_event->nParam2 >> 8) & 0xff);
    CFW_GetGprsActState(cid, &state, cfw_event->nFlag);
    if (state == CFW_GPRS_ACTIVED)
    {
        g_staAtGprsCidInfo[cid].apnacAer = cfw_event->nParam2 & 0xff;
        g_staAtGprsCidInfo[cid].apnacUnit = cfw_event->nType;
        g_staAtGprsCidInfo[cid].apnacRate = cfw_event->nParam1;
        OSI_LOGI(0x100041e8, "apn rate control, aer:%d, unit: %d, rate: %d", g_staAtGprsCidInfo[cid].apnacAer, g_staAtGprsCidInfo[cid].apnacUnit, g_staAtGprsCidInfo[cid].apnacRate);
    }
    else
    {
        OSI_LOGI(0x100041e9, "apn rate control, pdn is not active!");
    }
#endif
}

static void _onEV_CFW_GPRS_SEC_CTX_ACTIVE_IND(const osiEvent_t *event)
{
    OSI_LOGI(0x100041f4, "At got EV_CFW_GPRS_SEC_CTX_ACTIVE_IND");
    CFW_EVENT cfw_event = *(const CFW_EVENT *)event;
    uint8_t nSim = cfw_event.nFlag;
    if (gAtGprsCGEVFlag == 1)
    {
        char nResp[50] = {0};
        sprintf(nResp, "+CGEV: NW ACT  \"IP\", ,%ld", cfw_event.nParam1);
        atCmdRespSimUrcText(nSim, nResp);
    }
    if (cfw_event.nParam2 != IMS_PDP_CID)
        atCfwGprsSetPCid(cfw_event.nParam1, cfw_event.nParam2, nSim);
    cfw_event.nParam1 = 0;
}
#endif

static void _onEV_CFW_GPRS_DATA_IND(const osiEvent_t *event)
{
#ifdef PORTING_ON_GOING
    PsDownlinkDataHandler((CFW_GPRS_DATA *)pEvent->nParam1, LOUINT8(pEvent->nParam2), cfw_event.nUTI);
    pEvent->nParam1 = 0;
#endif
}

void atCfwGprsInit(void)
{
    OSI_LOGI(0, "atCfwGprsInit function start");
    CFW_GPRS_PDPCONT_INFO_V2 pdp_cont;
    memset(&pdp_cont, 0, sizeof(pdp_cont));
    for (uint8_t nSim = 0; nSim < 2; nSim++)
    {
        AT_Gprs_CidInfo *g_staAtGprsCidInfo = gAtCfwCtx.sim[nSim].cid_info;
        AT_Gprs_CidInfo *g_staAtGprsCidInfo_nv = gAtSetting.g_staAtGprsCidInfo[nSim];
        for (uint8_t cid = AT_PDPCID_MIN; cid <= AT_PDPCID_MAX; cid++)
        {
            pdp_cont.nAuthProt = g_staAtGprsCidInfo_nv[cid].nAuthProt;
            if (g_staAtGprsCidInfo_nv[cid].uCid != 0)
            {
                OSI_LOGI(0, "in atCfwGprsInit function seting pdp CONTEXT cid:%d", cid);
                g_staAtGprsCidInfo[cid].uCid = g_staAtGprsCidInfo_nv[cid].uCid;
                pdp_cont.nPdpType = g_staAtGprsCidInfo_nv[cid].nPdpType;
                pdp_cont.nAuthProt = g_staAtGprsCidInfo_nv[cid].nAuthProt;
                pdp_cont.PdnType = pdp_cont.nPdpType;
                pdp_cont.nDComp = g_staAtGprsCidInfo_nv[cid].nDComp;
                pdp_cont.nHComp = g_staAtGprsCidInfo_nv[cid].nHComp;
                pdp_cont.nApnSize = g_staAtGprsCidInfo_nv[cid].nApnSize;
                pdp_cont.nPdpAddrSize = g_staAtGprsCidInfo_nv[cid].nPdpAddrSize;
                memcpy(pdp_cont.pApn, g_staAtGprsCidInfo_nv[cid].pApn, g_staAtGprsCidInfo_nv[cid].nApnSize);
                if (pdp_cont.nPdpType == CFW_GPRS_PDP_TYPE_IPV6 && pdp_cont.nPdpAddrSize == 16)
                {
                    memcpy(&pdp_cont.pPdpAddr[4], &g_staAtGprsCidInfo_nv[cid].pPdpAddr[4], g_staAtGprsCidInfo_nv[cid].nPdpAddrSize);
                }
                else
                {
                    memcpy(pdp_cont.pPdpAddr, g_staAtGprsCidInfo_nv[cid].pPdpAddr, g_staAtGprsCidInfo_nv[cid].nPdpAddrSize);
                }
                pdp_cont.nApnUserSize = g_staAtGprsCidInfo_nv[cid].nUsernameSize;
                pdp_cont.nApnPwdSize = g_staAtGprsCidInfo_nv[cid].nPasswordSize;
                memcpy(pdp_cont.pApnUser, g_staAtGprsCidInfo_nv[cid].uaUsername, g_staAtGprsCidInfo_nv[cid].nUsernameSize);
                memcpy(pdp_cont.pApnPwd, g_staAtGprsCidInfo_nv[cid].uaPassword, g_staAtGprsCidInfo_nv[cid].nPasswordSize);
                if (CFW_GprsSetPdpCxtV2(cid, &pdp_cont, nSim) != 0)
                {
                    OSI_LOGI(0, "in atCfwGprsInit function CFW_GprsSetPdpCxtV2:%d error", cid);
                }
            }
        }
    }
    atEventsRegister(
        EV_CFW_GPRS_CXT_ACTIVE_IND, _onEV_CFW_GPRS_CXT_ACTIVE_IND,
        EV_CFW_GPRS_CXT_DEACTIVE_IND, _onEV_CFW_GPRS_CXT_DEACTIVE_IND,
        EV_CFW_GPRS_MOD_IND, _onEV_CFW_GPRS_MOD_IND,
        EV_CFW_GPRS_STATUS_IND, _onEV_CFW_GPRS_STATUS_IND,
#ifdef LTE_NBIOT_SUPPORT
        EV_CFW_GPRS_APNCR_IND, _onEV_CFW_GPRS_APNCR_IND,
        EV_CFW_GPRS_SEC_CTX_ACTIVE_IND, _onEV_CFW_GPRS_SEC_CTX_ACTIVE_IND,
#endif
        EV_CFW_GPRS_DATA_IND, _onEV_CFW_GPRS_DATA_IND,
        0);
}

uint32_t CFW_GetDftPdnInfoFromAP(CFW_GPRS_DFTPDN_INFO *nDftPdn, CFW_SIM_ID nSimID)
{
    if (nDftPdn == NULL || nSimID > 1 || nSimID < 0)
        return 1;
    AT_Gprs_CidInfo *g_staAtGprsCidInfo_nv = gAtSetting.g_staAtGprsCidInfo[nSimID];
    if (g_staAtGprsCidInfo_nv[0].uCid == 0xFF) //use CFGDFTPDN
    {
        nDftPdn->nPdpType = g_staAtGprsCidInfo_nv[0].nPdpType;
        nDftPdn->nAuthProt = g_staAtGprsCidInfo_nv[0].nAuthProt;
        nDftPdn->nApnSize = g_staAtGprsCidInfo_nv[0].nApnSize;
        if (nDftPdn->nApnSize > 0)
        {
            memcpy(nDftPdn->pApn, g_staAtGprsCidInfo_nv[0].pApn, nDftPdn->nApnSize);
        }
        nDftPdn->nApnUserSize = g_staAtGprsCidInfo_nv[0].nUsernameSize;
        if (nDftPdn->nApnUserSize > 0)
        {
            memcpy(nDftPdn->pApnUser, g_staAtGprsCidInfo_nv[0].uaUsername, nDftPdn->nApnUserSize);
        }
        nDftPdn->nApnPwdSize = g_staAtGprsCidInfo_nv[0].nPasswordSize;
        if (nDftPdn->nApnPwdSize > 0)
        {
            memcpy(nDftPdn->pApnPwd, g_staAtGprsCidInfo_nv[0].uaPassword, nDftPdn->nApnPwdSize);
        }
        return 0;
    }
    return 1;
}

#ifdef AUTO_GPRS_ACT
void AT_AutoGprsAct(uint8_t nDLCI, uint8_t nSim)
{
    int32_t iResult;
    uint8_t uCid = 1;
    uint8_t uCidState;

    OSI_LOGI(0x100041f5, "AT_AutoGprsAct");
    if (gAtAttachState == CFW_GPRS_ATTACHED)
    {
        OSI_LOGI(0x100041f6, "AT_AutoGprsAct complete");
        return;
    }
    CFW_GetGprsActState(uCid, &uCidState, nSim);
    if (uCidState != AT_GPRS_ACT)
    {
        iResult = AT_CFW_GprsAct(AT_GPRS_ACT, uCid, nDLCI, nSim, AT_GPRS_AsyncEventProcess);

        if (0 != iResult)
        {
            OSI_LOGI(0x100041f7, "AT_AutoGprsAct ERROR");
        }
        else
        {
            OSI_LOGI(0x100041f8, "AT_AutoGprsAct success cid %d", uCid);
        }
    }
}
#endif

static OSI_UNUSED void coap_Selfreg_dataCbk(uint16_t nUTI, char *code, int len)
{
#ifdef PORTING_ON_GOING
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nUTI);
    at_CmdRespInfoNText(engine, code, len);
#endif
}

void atCmdHandleCGAUTH(atCommand_t *cmd)
{
    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
    uint8_t uCid;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    AT_Gprs_CidInfo *g_staAtGprsCidInfo = gAtCfwCtx.sim[nSim].cid_info;
    AT_Gprs_CidInfo *g_staAtGprsCidInfo_nv = gAtSetting.g_staAtGprsCidInfo[nSim];
    memset(&sPdpCont, 0, sizeof(CFW_GPRS_PDPCONT_INFO_V2));

    if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CGAUTH: (1..7),(0..2),(0..20),(0..20)");
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count > 4 || cmd->param_count < 1)
            AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        if (1 <= cmd->param_count)
        {
            uCid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
            if (g_staAtGprsCidInfo_nv[uCid].uCid == 0) //undefied cid not allowed set authentication parameters
            {
                AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED));
            }
            if (1 == cmd->param_count)
            {
                /*if (CFW_GprsSetPdpCxtV2(uCid, &sPdpCont, nSim) != 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                CFW_GprsRemovePdpCxt(uCid, nSim);
                RETURN_OK(cmd->engine);*/
                AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID));
            }
        }

        uint8_t nAuthProt = atParamDefUintInRange(cmd->params[1], 0, 0, 2, &paramok);
        const char *pUserID = atParamOptStr(cmd->params[2], &paramok);
        const char *pUserPW = atParamOptStr(cmd->params[3], &paramok);
        if (!paramok)
            AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID));
        if (strlen(pUserID) > (THE_APN_USER_MAX_LEN - 1) || strlen(pUserPW) > THE_APN_PWD_MAX_LEN)
            AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID));

        if (CFW_GprsGetPdpCxtV2(uCid, &sPdpCont, nSim) != 0 || gAtCfwCtx.sim[nSim].cid_info[uCid].uCid == 0)
            memset(&sPdpCont, 0x00, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
        g_staAtGprsCidInfo[uCid].uCid = uCid;
        g_staAtGprsCidInfo[uCid].nAuthProt = sPdpCont.nAuthProt = nAuthProt;

        if (strlen(pUserID) != 0)
        {
            g_staAtGprsCidInfo[uCid].nUsernameSize = sPdpCont.nApnUserSize = strlen(pUserID);
            memcpy(g_staAtGprsCidInfo[uCid].uaUsername, pUserID, strlen(pUserID));
            if (sPdpCont.pApnUser)
                memset(sPdpCont.pApnUser, 0x00, THE_APN_USER_MAX_LEN);
            memcpy(sPdpCont.pApnUser, pUserID, strlen(pUserID));
        }

        if (strlen(pUserPW) != 0)
        {
            g_staAtGprsCidInfo[uCid].nPasswordSize = sPdpCont.nApnPwdSize = strlen(pUserPW);
            memcpy(g_staAtGprsCidInfo[uCid].uaPassword, pUserPW, strlen(pUserPW));
            if (sPdpCont.pApnPwd)
                memset(sPdpCont.pApnPwd, 0x00, THE_APN_PWD_MAX_LEN);
            memcpy(sPdpCont.pApnPwd, pUserPW, strlen(pUserPW));
        }

        //hal_HstSendEvent(0x88888800 + uCid);
        uint32_t iResult = CFW_GprsSetPdpCxtV2(uCid, &sPdpCont, nSim);
        if (ERR_SUCCESS != iResult)
            AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL));
        g_staAtGprsCidInfo_nv[uCid].uCid = uCid;
        g_staAtGprsCidInfo_nv[uCid].nAuthProt = sPdpCont.nAuthProt;
        g_staAtGprsCidInfo_nv[uCid].nUsernameSize = sPdpCont.nApnUserSize;
        g_staAtGprsCidInfo_nv[uCid].nPasswordSize = sPdpCont.nApnPwdSize;
        memcpy(g_staAtGprsCidInfo_nv[uCid].uaUsername, sPdpCont.pApnUser, sPdpCont.nApnUserSize);
        memcpy(g_staAtGprsCidInfo_nv[uCid].uaPassword, sPdpCont.pApnPwd, sPdpCont.nApnPwdSize);
        atCfgAutoSave();
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[64] = {0};
        for (uint8_t cid = AT_PDPCID_MIN; cid <= AT_PDPCID_MAX; cid++)
        {
            memset(&sPdpCont, 0, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
            memset(sPdpCont.pApnUser, 0, THE_APN_USER_MAX_LEN);
            memset(sPdpCont.pApnPwd, 0, THE_APN_PWD_MAX_LEN);
            if (CFW_GprsGetPdpCxtV2(cid, &sPdpCont, nSim) == 0)
            {
                char *prsp = &rsp[0];
                uint8_t nApnUser[THE_APN_USER_MAX_LEN + 1] = {
                    0x00,
                };
                uint8_t nApnPwd[THE_APN_PWD_MAX_LEN + 1] = {
                    0x00,
                };

                memcpy(nApnUser, sPdpCont.pApnUser, sPdpCont.nApnUserSize);
                memcpy(nApnPwd, sPdpCont.pApnPwd, sPdpCont.nApnPwdSize);

                prsp += sprintf(prsp, "+CGAUTH: %d,%d", cid, sPdpCont.nAuthProt);
                if (sPdpCont.nApnUserSize == 0)
                    prsp += sprintf(prsp, ",\"\"");
                else
                    prsp += sprintf(prsp, ",\"%s\"", nApnUser);
                if (sPdpCont.nApnPwdSize == 0)
                    prsp += sprintf(prsp, ",\"\"");
                else
                    prsp += sprintf(prsp, ",\"%s\"", nApnPwd);
                atCmdRespInfoText(cmd->engine, rsp);
            }
        }
        RETURN_OK(cmd->engine);
    }
    else
    {
        AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

void atCmdHandleCGDCONT(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t nActState;
    uint8_t cid = 0;
    CFW_GPRS_QOS Qos = {3, 4, 3, 4, 16};
    CFW_GPRS_QOS null_qos = {0, 0, 0, 0, 0};
    CFW_GPRS_PDPCONT_INFO_V2 pdp_cont;
    AT_Gprs_CidInfo *g_staAtGprsCidInfo = gAtCfwCtx.sim[nSim].cid_info;
    AT_Gprs_CidInfo *g_staAtGprsCidInfo_nv = gAtSetting.g_staAtGprsCidInfo[nSim];

    if (cmd->type == AT_CMD_SET)
    {
        // +CGDCONT=<cid>[,<PDP_type>[,<APN>[,<PDP_addr>[,<d_comp>[,<h_comp>]]]]]

        memset(&pdp_cont, 0, sizeof(pdp_cont));

        bool paramok = true;
        if (1 <= cmd->param_count)
        {
            cid = atParamUintInRange(
                cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
            OSI_LOGI(0, "CGDCONT set cid is: %d", cid);
            CFW_GetGprsActState(cid, &nActState, nSim);
            if (nActState == CFW_GPRS_ACTIVED)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            if (1 == cmd->param_count)
            {
                CFW_GprsSetReqQos(cid, &null_qos, nSim);
                if (CFW_GprsSetPdpCxtV2(cid, &pdp_cont, nSim) != 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                g_staAtGprsCidInfo[cid].uCid = 0;
                g_staAtGprsCidInfo_nv[cid].uCid = 0;
                g_staAtGprsCidInfo_nv[cid].nPdpType = 0;
                g_staAtGprsCidInfo_nv[cid].nDComp = 0;
                g_staAtGprsCidInfo_nv[cid].nHComp = 0;
                g_staAtGprsCidInfo_nv[cid].nApnSize = 0;
                g_staAtGprsCidInfo_nv[cid].nPdpAddrSize = 0;
                memset(g_staAtGprsCidInfo_nv[cid].pApn, 0, AT_GPRS_APN_MAX_LEN);
                memset(g_staAtGprsCidInfo_nv[cid].pPdpAddr, 0, AT_GPRS_PDPADDR_MAX_LEN);
                atCfgAutoSave();
                CFW_GprsRemovePdpCxt(cid, nSim);
                RETURN_OK(cmd->engine);
            }
        }
        pdp_cont.nPdpType = atParamDefUintByStrMap(
            cmd->params[1], CFW_GPRS_PDP_TYPE_IP, gPdpTypeVSMap, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (pdp_cont.nPdpType == CFW_GPRS_PDP_TYPE_PPP)
            pdp_cont.nPdpType = CFW_GPRS_PDP_TYPE_IP;
        pdp_cont.PdnType = pdp_cont.nPdpType;
        const char *apn = atParamDefStr(cmd->params[2], "", &paramok);
        const char *pdp_addr = atParamDefStr(cmd->params[3], "", &paramok);
        uint8_t dcomp = atParamDefUintInRange(cmd->params[4], CFW_GPRS_PDP_D_COMP_OFF,
                                              CFW_GPRS_PDP_D_COMP_OFF, CFW_GPRS_PDP_D_COMP_V44,
                                              &paramok);
        uint8_t hcomp = atParamDefUintInRange(cmd->params[5], CFW_GPRS_PDP_H_COMP_OFF,
                                              CFW_GPRS_PDP_H_COMP_OFF, CFW_GPRS_PDP_H_COMP_RFC3095,
                                              &paramok);
        //    uint8_t ipv4addralloc = atParamDefUintInRange(cmd->params[6], 0, 0, 1, &paramok);
        //    uint8_t request_type = atParamDefUintInRange(cmd->params[7], 2, 0, 4, &paramok);
        //   uint8_t cscf = atParamDefUintInRange(cmd->params[8], 1, 0, 2, &paramok);
        //     uint8_t imcn = atParamDefUintInRange(cmd->params[9], 0, 0, 1, &paramok);
        //    uint8_t nslpi = atParamDefUintInRange(cmd->params[10], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 6)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        //    (void)ipv4addralloc;
        //     (void)request_type;
        //    (void)cscf;
        //   (void)imcn;

        pdp_cont.nApnSize = strlen(apn);
        if (pdp_cont.nApnSize > THE_APN_MAX_LEN)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint8_t i = 0;
        for (i = 0; i < strlen(apn); i++)
        {
            /* if ((*(apn + i) < 0x2D) ||
                ((*(apn + i) > 0x2D && *(apn + i) < 0x30) && (*(apn + i) != 0x2E)) ||
                (*(apn + i) > 0x39 && *(apn + i) < 0x41) ||
                (*(apn + i) > 0x5A && *(apn + i) < 0x61) ||
                (*(apn + i) > 0x7A)) //not "A-Z"(0X41-5A) "a-z"(0X61-7A) "0-9" "-"(0X2D)*/

            if (!((*(apn + i) >= 'A' && *(apn + i) <= 'Z') ||
                  (*(apn + i) >= 'a' && *(apn + i) <= 'z') ||
                  (*(apn + i) >= '0' && *(apn + i) <= '9') ||
                  (*(apn + i) == '.') ||
                  (*(apn + i) == '-'))) //not "A-Z"(0X41-5A) "a-z"(0X61-7A) "0-9" "-"(0X2D) "."
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        memcpy(pdp_cont.pApn, apn, pdp_cont.nApnSize);
        uint8_t pdp_addr_len = strlen(pdp_addr);
        OSI_LOGI(0, "cgdcont:pdp_addr_len==%d", pdp_addr_len);
        if (_pdpAddressFromStr(pdp_cont.nPdpType, pdp_addr) != 0 && pdp_addr_len != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (pdp_addr_len != 0)
        {
#if LWIP_IPV6
            if (pdp_cont.nPdpType == CFW_GPRS_PDP_TYPE_IPV6)
            {
                ip6_addr_t dns1_ip6;
                ip6addr_aton(pdp_addr, &dns1_ip6);
                memcpy(&pdp_cont.pPdpAddr[4], &dns1_ip6.addr[0], 16);
                pdp_cont.nPdpAddrSize = 16;
            }
#endif
            if (pdp_cont.nPdpType == CFW_GPRS_PDP_TYPE_IP)
            {
                uint32_t Pro_DnsIp1 = (uint32_t)inet_addr((const char *)pdp_addr);
                pdp_cont.pPdpAddr[0] = Pro_DnsIp1;
                pdp_cont.pPdpAddr[1] = Pro_DnsIp1 >> 8;
                pdp_cont.pPdpAddr[2] = Pro_DnsIp1 >> 16;
                pdp_cont.pPdpAddr[3] = Pro_DnsIp1 >> 24;
                pdp_cont.nPdpAddrSize = 4;
            }
        }
        pdp_cont.nDComp = dcomp;
        pdp_cont.nHComp = hcomp;
        //     pdp_cont.nNSLPI = nslpi;

        g_staAtGprsCidInfo[cid].uCid = cid;
#ifdef LTE_NBIOT_SUPPORT
        g_staAtGprsCidInfo[cid].uPCid = 0;
#endif
        CFW_GprsSetReqQos(cid, &Qos, nSim);
        if (CFW_GprsSetPdpCxtV2(cid, &pdp_cont, nSim) != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        g_staAtGprsCidInfo_nv[cid].uCid = cid;
        g_staAtGprsCidInfo_nv[cid].nPdpType = pdp_cont.nPdpType;
        g_staAtGprsCidInfo_nv[cid].nDComp = pdp_cont.nDComp;
        g_staAtGprsCidInfo_nv[cid].nHComp = pdp_cont.nHComp;
        g_staAtGprsCidInfo_nv[cid].nApnSize = pdp_cont.nApnSize;
        g_staAtGprsCidInfo_nv[cid].nPdpAddrSize = pdp_cont.nPdpAddrSize;
        memcpy(g_staAtGprsCidInfo_nv[cid].pApn, pdp_cont.pApn, pdp_cont.nApnSize);
        if (pdp_cont.nPdpType == CFW_GPRS_PDP_TYPE_IPV6 && pdp_cont.nPdpAddrSize == 16)
        {
            memcpy(&g_staAtGprsCidInfo_nv[cid].pPdpAddr[4], &pdp_cont.pPdpAddr[4], pdp_cont.nPdpAddrSize);
        }
        else
        {
            memcpy(g_staAtGprsCidInfo_nv[cid].pPdpAddr, pdp_cont.pPdpAddr, pdp_cont.nPdpAddrSize);
        }
        atCfgAutoSave();
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[220];
        uint8_t pdpType = 0;
        uint8_t nTmpApn[THE_APN_MAX_LEN + 1] = {
            0,
        };
#ifndef CONFIG_QUEC_PROJECT_FEATURE_NW        
        if (CFW_NWGetStackRat(nSim) == 4)
        {
            CFW_GPRS_PDPCONT_DFTPDN_INFO PdnInfo = {
                0,
            };
            memset(&PdnInfo, 0, sizeof(CFW_GPRS_PDPCONT_DFTPDN_INFO));
            CFW_GprsGetDefaultPdnInfo(&PdnInfo, nSim);
            char *prsp2 = &rsp[0];
            const osiValueStrMap_t *pdp_type_vs2 = osiVsmapFindByVal(gPdpTypeVSMap, PdnInfo.nPdpType);
            if (pdp_type_vs2 != NULL)
            {
                prsp2 += sprintf(prsp2, "+CGDCONT: 0,\"%s\"", pdp_type_vs2->str);
                if (PdnInfo.APNLen == 0)
                    prsp2 += sprintf(prsp2, ",\"\"");
                else
                {
                    uint8_t i = 0;
                    for (i = 0; i < PdnInfo.APNLen; i++)
                    {
                        if (((PdnInfo.AccessPointName[i] == 0x2E) && (PdnInfo.AccessPointName[i + 1] == 0x4D) && (PdnInfo.AccessPointName[i + 2] == 0x4E) && (PdnInfo.AccessPointName[i + 3] == 0x43)) ||
                            ((PdnInfo.AccessPointName[i] == 0x2E) && (PdnInfo.AccessPointName[i + 1] == 0x6D) && (PdnInfo.AccessPointName[i + 2] == 0x6E) && (PdnInfo.AccessPointName[i + 3] == 0x63))) //find first ".mnc" or ".MNC"
                        {
                            PdnInfo.APNLen = i;
                            break;
                        }
                    }
                    memcpy(nTmpApn, PdnInfo.AccessPointName, PdnInfo.APNLen);
                    prsp2 += sprintf(prsp2, ",\"%s\"", nTmpApn);
                }
                char str[80] = {};
                //_pdpAddressToStr(&pdp_cont, addr_str);
                ip4_addr_t ip4 = {0};
#if LWIP_IPV6
                ip6_addr_t ip6 = {0};
#endif
                char str_ipv4[20];
                char str_ipv6[40];

                if ((PdnInfo.nPdpType == CFW_GPRS_PDP_TYPE_IP) || (PdnInfo.nPdpType == CFW_GPRS_PDP_TYPE_IPV4V6) || (PdnInfo.nPdpType == CFW_GPRS_PDP_TYPE_X25) ||
                    (PdnInfo.nPdpType == CFW_GPRS_PDP_TYPE_PPP)) //add evade for nPdpType == 0, need delete later
                {
                    if (PdnInfo.PDPAddLen == 0)
                        sprintf(str, "IPV4:0.0.0.0");
                    else
                    {
                        IP4_ADDR(&ip4, PdnInfo.PDPAddress[0], PdnInfo.PDPAddress[1], PdnInfo.PDPAddress[2], PdnInfo.PDPAddress[3]);
                        sprintf(str, ip4addr_ntoa(&ip4));
                        sprintf(str_ipv4, ip4addr_ntoa(&ip4));
                    }
                }

#if LWIP_IPV6
                if (((PdnInfo.nPdpType == CFW_GPRS_PDP_TYPE_IPV6) || (PdnInfo.nPdpType == CFW_GPRS_PDP_TYPE_IPV4V6)))
                {
                    if (PdnInfo.PDPAddLen == 0)
                        sprintf(str, "IPV6:  ::");
                    else
                    {
                        uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(PdnInfo.PDPAddress[4], PdnInfo.PDPAddress[5], PdnInfo.PDPAddress[6], PdnInfo.PDPAddress[7]));
                        uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(PdnInfo.PDPAddress[8], PdnInfo.PDPAddress[9], PdnInfo.PDPAddress[10], PdnInfo.PDPAddress[11]));
                        uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(PdnInfo.PDPAddress[12], PdnInfo.PDPAddress[13], PdnInfo.PDPAddress[14], PdnInfo.PDPAddress[15]));
                        uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(PdnInfo.PDPAddress[16], PdnInfo.PDPAddress[17], PdnInfo.PDPAddress[18], PdnInfo.PDPAddress[19]));
                        IP6_ADDR(&ip6, addr0, addr1, addr2, addr3);
                        sprintf(str, ip6addr_ntoa(&ip6));
                        sprintf(str_ipv6, ip6addr_ntoa(&ip6));
                    }
                }
#endif
                if (PdnInfo.nPdpType == CFW_GPRS_PDP_TYPE_IPV4V6)
                {
                    if (PdnInfo.PDPAddLen == 0)
                        sprintf(str, "IPV4:0.0.0.0    IPV6:  ::");
                    else
                    {
                        sprintf(str, "IPV4:%s    IPV6:%s", str_ipv4, str_ipv6);
                    }
                }
                prsp2 += sprintf(prsp2, ",\"%s\"", str);
                atCmdRespInfoText(cmd->engine, rsp);
            }
        }
#endif        
        for (uint8_t cid = AT_PDPCID_MIN; cid <= AT_PDPCID_MAX; cid++)
        {
            // uint8_t att_state;
            memset(&pdp_cont, 0, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
            memset(pdp_cont.pApn, 0, THE_APN_MAX_LEN);
            memset(nTmpApn, 0, THE_APN_MAX_LEN + 1);

            // CFW_GetGprsAttState(&att_state, nSim);
            // if (att_state == CFW_GPRS_ATTACHED)
            // {
            if (CFW_GprsGetPdpCxtV2(cid, &pdp_cont, nSim) == 0)
            {
                pdpType = pdp_cont.nPdpType;

                const osiValueStrMap_t *pdp_type_vs = osiVsmapFindByVal(gPdpTypeVSMap, pdpType);
                if (pdp_type_vs == NULL)
                    pdp_type_vs = &gPdpTypeVSMap[0];

                char *prsp = &rsp[0];
                prsp += sprintf(prsp, "+CGDCONT: %d,\"%s\"", cid, pdp_type_vs->str);

                if (pdp_cont.nApnSize == 0)
                    prsp += sprintf(prsp, ",\"\"");
                else
                {
                    uint8_t i = 0;
                    for (i = 0; i < pdp_cont.nApnSize; i++)
                    {
                        if (((pdp_cont.pApn[i] == 0x2E) && (pdp_cont.pApn[i + 1] == 0x4D) && (pdp_cont.pApn[i + 2] == 0x4E) && (pdp_cont.pApn[i + 3] == 0x43)) ||
                            ((pdp_cont.pApn[i] == 0x2E) && (pdp_cont.pApn[i + 1] == 0x6D) && (pdp_cont.pApn[i + 2] == 0x6E) && (pdp_cont.pApn[i + 3] == 0x63))) //find first ".mnc" or ".MNC"
                        {
                            pdp_cont.nApnSize = i;
                            break;
                        }
                    }
                    memcpy(nTmpApn, pdp_cont.pApn, pdp_cont.nApnSize);
                    prsp += sprintf(prsp, ",\"%s\"", nTmpApn);
                }
                char addr_str[80] = {0};
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW                
                quec_pdpAddressToStr(&pdp_cont, addr_str, cid, nSim);
#else                
                _pdpAddressToStr(&pdp_cont, addr_str);
#endif
                prsp += sprintf(prsp, ",\"%s\"", addr_str);

                prsp += sprintf(prsp, ",%d,%d", pdp_cont.nDComp, pdp_cont.nHComp);
                atCmdRespInfoText(cmd->engine, rsp);
            }
            // }
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        atCmdRespInfoText(cmd->engine, "+CGDCONT: (1-7),\"IP\",,,(0-3),(0-4)");
        atCmdRespInfoText(cmd->engine, "+CGDCONT: (1-7),\"PPP\",,,(0-3),(0-4)");
        atCmdRespInfoText(cmd->engine, "+CGDCONT: (1-7),\"IPV6\",,,(0-3),(0-4)");
        atCmdRespInfoText(cmd->engine, "+CGDCONT: (1-7),\"IPV4V6\",,,(0-3),(0-4)");
#else
        atCmdRespInfoText(cmd->engine, "+CGDCONT: (1-7),(IP,IPV6,IPV4V6,PPP,Non-IP),(0-3),(0-4)");
#endif
        RETURN_OK(cmd->engine);
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW    
    else if(AT_CMD_EXE == cmd->type)
    {
        RETURN_OK(cmd->engine);
    }
#endif    
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleCFGIMSPDN(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t cid = 0;
    CFW_GPRS_PDPCONT_INFO_V2 pdp_cont;
    if (cmd->type == AT_CMD_SET)
    {
        // +CFGIMSPDN=<cid>[,<PDP_type>[,<APN>[,<PDP_addr>[,<d_comp>[,<h_comp>]]]]]

        memset(&pdp_cont, 0, sizeof(pdp_cont));

        bool paramok = true;
        if (1 <= cmd->param_count)
        {
            cid = atParamUintInRange(cmd->params[0], IMS_PDP_CID, IMS_PDP_CID, &paramok);
            OSI_LOGI(0, "CFGIMSPDN set cid is: %d", cid);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (1 == cmd->param_count)
            {
                if (CFW_GprsSetInternalPdpCtx(PDP_CTX_TYPE_IMS, &pdp_cont, nSim) != 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                RETURN_OK(cmd->engine);
            }
        }
        pdp_cont.nPdpType = atParamDefUintByStrMap(
            cmd->params[1], CFW_GPRS_PDP_TYPE_IP, gPdpTypeVSMap, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (pdp_cont.nPdpType == CFW_GPRS_PDP_TYPE_PPP)
            pdp_cont.nPdpType = CFW_GPRS_PDP_TYPE_IP;
        pdp_cont.PdnType = pdp_cont.nPdpType;
        const char *apn = atParamDefStr(cmd->params[2], "", &paramok);
        const char *pdp_addr = atParamDefStr(cmd->params[3], "", &paramok);
        uint8_t dcomp = atParamDefUintInRange(cmd->params[4], CFW_GPRS_PDP_D_COMP_OFF,
                                              CFW_GPRS_PDP_D_COMP_OFF, CFW_GPRS_PDP_D_COMP_V44,
                                              &paramok);
        uint8_t hcomp = atParamDefUintInRange(cmd->params[5], CFW_GPRS_PDP_H_COMP_OFF,
                                              CFW_GPRS_PDP_H_COMP_OFF, CFW_GPRS_PDP_H_COMP_RFC3095,
                                              &paramok);
        if (!paramok || cmd->param_count > 6)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        pdp_cont.nApnSize = strlen(apn);
        if (pdp_cont.nApnSize > THE_APN_MAX_LEN)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint8_t i = 0;
        for (i = 0; i < strlen(apn); i++)
        {
            if (!((*(apn + i) >= 'A' && *(apn + i) <= 'Z') ||
                  (*(apn + i) >= 'a' && *(apn + i) <= 'z') ||
                  (*(apn + i) >= '0' && *(apn + i) <= '9') ||
                  (*(apn + i) == '.') ||
                  (*(apn + i) == '-'))) //not "A-Z"(0X41-5A) "a-z"(0X61-7A) "0-9" "-"(0X2D) "."
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        if (pdp_cont.nApnSize > 0)
            memcpy(pdp_cont.pApn, apn, pdp_cont.nApnSize);

        pdp_cont.nPdpAddrSize = strlen(pdp_addr);
        if (pdp_cont.nPdpAddrSize > 0 && _pdpAddressFromStr(pdp_cont.nPdpType, pdp_addr) != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (pdp_cont.nPdpAddrSize > 0)
        {
#if LWIP_IPV6
            if (pdp_cont.nPdpType == CFW_GPRS_PDP_TYPE_IPV6)
            {
                ip6_addr_t dns1_ip6;
                ip6addr_aton(pdp_addr, &dns1_ip6);
                memcpy(&pdp_cont.pPdpAddr[4], &dns1_ip6.addr[0], 16);
                pdp_cont.nPdpAddrSize = 16;
            }
#endif
            if (pdp_cont.nPdpType == CFW_GPRS_PDP_TYPE_IP)
            {
                uint32_t Pro_DnsIp1 = (uint32_t)inet_addr((const char *)pdp_addr);
                pdp_cont.pPdpAddr[0] = Pro_DnsIp1;
                pdp_cont.pPdpAddr[1] = Pro_DnsIp1 >> 8;
                pdp_cont.pPdpAddr[2] = Pro_DnsIp1 >> 16;
                pdp_cont.pPdpAddr[3] = Pro_DnsIp1 >> 24;
                pdp_cont.nPdpAddrSize = 4;
            }
        }
        pdp_cont.nDComp = dcomp;
        pdp_cont.nHComp = hcomp;
        if (CFW_GprsSetInternalPdpCtx(PDP_CTX_TYPE_IMS, &pdp_cont, nSim) != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[120];
        uint8_t pdpType = 0;
        uint8_t nTmpApn[THE_APN_MAX_LEN + 1] = {
            0,
        };
        CFW_GPRS_PDPCONT_INFO_V2 *pPdpCont = NULL;
        pPdpCont = CFW_GprsGetInternalPdpCtx(PDP_CTX_TYPE_IMS, nSim);
        if (pPdpCont != NULL)
        {
            pdpType = pPdpCont->nPdpType;
            const osiValueStrMap_t *pdp_type_vs = osiVsmapFindByVal(gPdpTypeVSMap, pdpType);
            if (pdp_type_vs == NULL)
            {
                RETURN_OK(cmd->engine);
            }
            char *prsp = &rsp[0];
            prsp += sprintf(prsp, "+CFGIMSPDN: 11,\"%s\"", pdp_type_vs->str);
            if (pPdpCont->nApnSize == 0)
                prsp += sprintf(prsp, ",\"\"");
            else
            {
                memcpy(nTmpApn, pPdpCont->pApn, THE_APN_MAX_LEN);
                prsp += sprintf(prsp, ",\"%s\"", nTmpApn);
            }
            char addr_str[80] = {};
            _pdpAddressToStr(pPdpCont, addr_str);
            prsp += sprintf(prsp, ",\"%s\"", addr_str);
            prsp += sprintf(prsp, ",%d,%d", pPdpCont->nDComp, pPdpCont->nHComp);
            atCmdRespInfoText(cmd->engine, rsp);
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CFGIMSPDN: 11,(IP,IPV6,IPV4V6,PPP),(0..3),(0..4)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

// 10.1.2 Define secondary PDP context
#ifdef LTE_NBIOT_SUPPORT

void atCmdHandleCGDSCONT(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t uResult = false;
    uint8_t uState = CFW_GPRS_DEACTIVED;

    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;

    AT_Gprs_CidInfo *pCidInfo = &gAtCfwCtx.sim[nSim].cid_info[AT_AUTO_ACT_CID];

    //judge cid 5 is actived or not

    if (ERR_SUCCESS != CFW_GprsGetPdpCxtV2(AT_AUTO_ACT_CID, &sPdpCont, nSim))
    {
        OSI_LOGI(0, "AT+CGDSCONT:CFW_GprsGetPdpCxtV2 return error,AT_AUTO_ACT_CID is not active!");
    }
    else
    {
        OSI_LOGI(0, "AT+CGDSCONT:CFW_GprsGetPdpCxtV2 return ok ,sPdpCont.nPdpType is : %d", sPdpCont.nPdpType);
        if (CFW_GPRS_PDP_TYPE_IPV4V6 == sPdpCont.nPdpType || CFW_GPRS_PDP_TYPE_IP == sPdpCont.nPdpType)
        {
            uResult = true;
        }
    }

    CFW_GetGprsActState(AT_AUTO_ACT_CID, &uState, nSim);
    OSI_LOGI(0, "defult ucid uState:%d", uState);
    if ((CFW_GPRS_ACTIVED == uState) && (uResult == true))
    {
        pCidInfo->uCid = AT_AUTO_ACT_CID;
        OSI_LOGI(0, "AT+CGDSCONT:AT_AUTO_ACT_CID is active! ");
    }

    if (AT_CMD_SET == cmd->type)
    {
        //+CGDSCONT=<cid>,<p_cid>[,<d_comp>[,<h_comp>]]
        CFW_GPRS_PDPCONT_INFO_V2 sPdpCont = {};
        CFW_GPRS_QOS stTmpQos = {0, 0, 0, 0, 0};
        CFW_GPRS_QOS stTmpNullQos = {3, 4, 3, 4, 16};

        bool paramok = true;
        uint8_t uCid = 0;
        uint8_t uP_Cid_set = 0;
        uint8_t uP_Cid[AT_PDPCID_MAX + 1] = {0};
        uint8_t uDComp = 0;
        uint8_t uHComp = 0;
        uint8_t nActState = CFW_GPRS_DEACTIVED;

        if (NULL == cmd) //AT+CGDCONT=
        {
            OSI_LOGI(0, "AT+CGDSCONT:NULL == pParam RETURN OK");
            RETURN_OK(cmd->engine);
        }

        if (cmd->param_count > 4 || cmd->param_count < 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        //<cid>
        uCid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        OSI_LOGI(0, "CGDSCONT set cid is: %d", uCid);
        CFW_GetGprsActState(uCid, &nActState, nSim);
        if (nActState == CFW_GPRS_ACTIVED)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        }

        CFW_GprsGetReqQos(uCid, &stTmpQos, nSim);
        if (CFW_GprsSetReqQos(uCid, &stTmpNullQos, nSim) != 0)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (cmd->param_count >= 2)
        {
            int flag_up_Cid = false;
            int ret = 0;
            uP_Cid_set = atParamUintInRange(cmd->params[1], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            if (uP_Cid_set == uCid)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            memset(&sPdpCont, 0, sizeof(CFW_GPRS_PDPCONT_INFO));

            for (int i = AT_PDPCID_MIN; i <= AT_PDPCID_MAX; i++)
            {
                AT_Gprs_CidInfo *pInfo = &gAtCfwCtx.sim[nSim].cid_info[i];

                if (pInfo->uCid != 0)
                {
                    uint8_t uState = CFW_GPRS_DEACTIVED;
                    ret = CFW_GetGprsActState(pInfo->uCid, &uState, nSim);

                    if (CFW_GPRS_ACTIVED != uState && ret == ERR_SUCCESS)
                        continue;
                    uP_Cid[i] = pInfo->uCid;
                }
            }

            for (int i = AT_PDPCID_MIN; i <= AT_PDPCID_MAX; i++)
            {
                if (uP_Cid_set == uP_Cid[i])
                {
                    flag_up_Cid = true;
                }
            }

            if (false == flag_up_Cid)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        if (cmd->param_count >= 3)
        {
            uDComp = atParamUintInRange(cmd->params[2], CFW_GPRS_PDP_D_COMP_OFF, CFW_GPRS_PDP_D_COMP_V44, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            sPdpCont.nDComp = uDComp;
        }

        if (cmd->param_count == 4)
        {
            uHComp = atParamUintInRange(cmd->params[3], CFW_GPRS_PDP_H_COMP_OFF, CFW_GPRS_PDP_H_COMP_RFC3095, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            sPdpCont.nHComp = uHComp;
        }

        if (cmd->param_count == 1) //if uParamCount == 1, sPdpCont has been clear. otherwise it hold context from uP_Cid
        {
            AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[uCid];
            if (pinfo->uCid == 0)
            {
                RETURN_OK(cmd->engine);
            }
            else
            {
                memset(&sPdpCont, 0, sizeof(CFW_GPRS_PDPCONT_INFO));
                CFW_GprsSetReqQos(uCid, &stTmpQos, nSim);
                if (CFW_GprsSetPdpCxtV2(uCid, &sPdpCont, nSim) != 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                CFW_GprsRemovePdpCxt(uCid, nSim);
                pinfo->uCid = 0;
                RETURN_OK(cmd->engine);
            }
        }

        if (CFW_GprsSetPdpCxtV2(uCid, &sPdpCont, nSim) != 0)
        {
            CFW_GprsSetReqQos(uCid, &stTmpQos, nSim);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[uCid];
        pinfo->uCid = (cmd->param_count > 1) ? uCid : 0;
        pinfo->uPCid = uP_Cid_set;
        pinfo->nDLCI = 0; // nDLCI;
        pinfo->nPdpType = sPdpCont.nPdpType;
        pinfo->nDComp = sPdpCont.nDComp;
        pinfo->nHComp = sPdpCont.nHComp;

        pinfo->nApnSize = sPdpCont.nApnSize;
        if (sPdpCont.nApnSize != 0)
            memcpy(pinfo->pApn, sPdpCont.pApn, sizeof(pinfo->pApn));
        pinfo->nPdpAddrSize = sPdpCont.nPdpAddrSize;
        if (sPdpCont.nPdpAddrSize != 0)
            memcpy(pinfo->pPdpAddr, sPdpCont.pPdpAddr, sizeof(pinfo->pApn));
        pinfo->nPasswordSize = sPdpCont.nApnPwdSize;
        if (sPdpCont.nApnPwdSize != 0)
            memcpy(pinfo->uaPassword, sPdpCont.pApnPwd, sizeof(pinfo->uaPassword));
        pinfo->nUsernameSize = sPdpCont.nApnUserSize;
        if (sPdpCont.nApnUserSize != 0)
            memcpy(pinfo->uaUsername, sPdpCont.pApnUser, sizeof(pinfo->uaUsername));

        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type) // Read command
    {
        char rsp[64];
        for (int uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[uCid];
            if (pinfo->uCid != 0)
            {
                sprintf(rsp, "+CGDSCONT: %d,%d,%d,%d", pinfo->uCid, pinfo->uPCid,
                        pinfo->nDComp, pinfo->nHComp);
                atCmdRespInfoText(cmd->engine, rsp);
            }
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type) // Test command.
    {
        char rsp[64];
        char *prsp = rsp;
        bool first = true;
        prsp += sprintf(prsp, "+CGDSCONT: (");
        uint8_t uState_arry[AT_PDPCID_MAX + 1] = {0};
        //cid
        for (int uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[uCid];

            if (pinfo->uCid != 0)
            {
                uState = CFW_GPRS_DEACTIVED;
                CFW_GetGprsActState(pinfo->uCid, &uState, nSim);

                if (CFW_GPRS_ACTIVED == uState)
                    uState_arry[uCid] = uState;
            }
            if (pinfo->uCid == 0 || ((pinfo->uCid != 0) && (uState_arry[uCid] != CFW_GPRS_ACTIVED)))
            {
                if (first)
                    prsp += sprintf(prsp, "%d", uCid);
                else
                    prsp += sprintf(prsp, ",%d", uCid);
                first = false;
            }
        }
        prsp += sprintf(prsp, "),(");

        //pcid
        first = true;
        for (int uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[uCid];

            if (pinfo->uCid != 0)
            {
                uState = CFW_GPRS_DEACTIVED;
                CFW_GetGprsActState(pinfo->uCid, &uState, nSim);

                if (CFW_GPRS_ACTIVED != uState)
                    continue;

                if (first)
                    prsp += sprintf(prsp, "%d", pinfo->uCid);
                else
                    prsp += sprintf(prsp, ",%d", pinfo->uCid);
                first = false;
            }
        }
        prsp += sprintf(prsp, "),(0-3),(0-4)");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
#endif
void atCmdHandleCGCONTRDP(atCommand_t *cmd)
{
    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont = {};
    CFW_GPRS_DYNAMIC_APN_INFO sDyncApn = {};
    CFW_GPRS_DYNAMIC_IP_INFO sDyncIP = {};
    memset(&sPdpCont, 0, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
    memset(&sDyncApn, 0, sizeof(CFW_GPRS_DYNAMIC_APN_INFO));
    memset(&sDyncIP, 0, sizeof(CFW_GPRS_DYNAMIC_IP_INFO));
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t len = 0;

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        char pResp[512] = {0};
        uint8_t nActState = 0;
        uint8_t Cid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        if (!paramok || cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        CFW_GetGprsActState(Cid, &nActState, nSim);
        if ((!CFW_GprsGetPdpCxtV2(Cid, &sPdpCont, nSim)) && (nActState == CFW_GPRS_ACTIVED))
        {
            if (!CFW_GprsGetDynamicAPN(Cid, &sDyncApn, nSim) && (sDyncApn.nApnSize > 0) && (sDyncApn.pApn))
#ifdef CONFIG_SOC_8910
                len += sprintf(pResp + len, "+CGCONTRDP: %d,%d,\"%s\",", Cid, CFW_GprsGetNsapi(Cid, nSim), sDyncApn.pApn);
#else
                len += sprintf(pResp + len, "+CGCONTRDP: %d,5,\"%s\",", Cid, sDyncApn.pApn);
#endif
            else
#ifdef CONFIG_SOC_8910
                len += sprintf(pResp + len, "+CGCONTRDP: %d,%d,,", Cid, CFW_GprsGetNsapi(Cid, nSim));
#else
                len += sprintf(pResp + len, "+CGCONTRDP: %d,5,,", Cid);
#endif
            if (!CFW_GprsGetDynamicIP(Cid, &sDyncIP, nSim) && (sDyncIP.nPdpAddrSize > 0) && (sDyncIP.pPdpAddr))
            {
                len += sprintf(pResp + len, "\"");
#if LWIP_IPV6
                if (sDyncIP.nPdpAddrSize == 16)
                {
                    const ip6_addr_t *ip6 = AT_GprsGetPdpIpv6Address(Cid, nSim);
                    sDyncIP.pPdpAddr[0] = (ip6->addr[0] & 0XFF);
                    sDyncIP.pPdpAddr[1] = (ip6->addr[0] >> 8) & 0XFF;
                    sDyncIP.pPdpAddr[2] = (ip6->addr[0] >> 16) & 0XFF;
                    sDyncIP.pPdpAddr[3] = (ip6->addr[0] >> 24) & 0XFF;
                    sDyncIP.pPdpAddr[4] = (ip6->addr[1] & 0XFF);
                    sDyncIP.pPdpAddr[5] = (ip6->addr[1] >> 8) & 0XFF;
                    sDyncIP.pPdpAddr[6] = (ip6->addr[1] >> 16) & 0XFF;
                    sDyncIP.pPdpAddr[7] = (ip6->addr[1] >> 24) & 0XFF;
                }
                else if (sDyncIP.nPdpAddrSize == 20)
                {
                    const ip6_addr_t *ip6 = AT_GprsGetPdpIpv6Address(Cid, nSim);
                    sDyncIP.pPdpAddr[4] = (ip6->addr[0] & 0XFF);
                    sDyncIP.pPdpAddr[5] = (ip6->addr[0] >> 8) & 0XFF;
                    sDyncIP.pPdpAddr[6] = (ip6->addr[0] >> 16) & 0XFF;
                    sDyncIP.pPdpAddr[7] = (ip6->addr[0] >> 24) & 0XFF;
                    sDyncIP.pPdpAddr[8] = (ip6->addr[1] & 0XFF);
                    sDyncIP.pPdpAddr[9] = (ip6->addr[1] >> 8) & 0XFF;
                    sDyncIP.pPdpAddr[10] = (ip6->addr[1] >> 16) & 0XFF;
                    sDyncIP.pPdpAddr[11] = (ip6->addr[1] >> 24) & 0XFF;
                }
#endif
                for (uint8_t uCnt = 0; uCnt < sDyncIP.nPdpAddrSize; uCnt++)
                {
                    len += sprintf(pResp + len, "%d.", (sDyncIP.pPdpAddr[uCnt] >> 4) * 16 + ((sDyncIP.pPdpAddr[uCnt]) & 0x0F));
                }
                for (uint8_t uCnt = 0; uCnt < sDyncIP.nPdpAddrSize; uCnt++)
                {
                    len += sprintf(pResp + len, "0.");
                }
                len += sprintf(pResp + len - 1, "\"");
            }
        }
        if (len > 0)
            atCmdRespInfoText(cmd->engine, pResp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_EXE == cmd->type)
    {
        char pResp[512] = {0};
        uint8_t nActState = 0;
        for (int Cid = AT_PDPCID_MIN; Cid <= AT_PDPCID_MAX; Cid++)
        {
            memset(pResp, 0, sizeof(pResp));
            len = 0;
            CFW_GetGprsActState(Cid, &nActState, nSim);
            if ((!CFW_GprsGetPdpCxtV2(Cid, &sPdpCont, nSim)) && (nActState == CFW_GPRS_ACTIVED))
            {
                uint8_t nActState = 0;
                CFW_GetGprsActState(Cid, &nActState, nSim);
                if (!CFW_GprsGetDynamicAPN(Cid, &sDyncApn, nSim) && (sDyncApn.nApnSize > 0) && (sDyncApn.pApn) && (nActState == CFW_GPRS_ACTIVED))
#ifdef CONFIG_SOC_8910
                    len += sprintf(pResp + len, "+CGCONTRDP: %d,%d,\"%s\",", Cid, CFW_GprsGetNsapi(Cid, nSim), sDyncApn.pApn);
#else
                    len += sprintf(pResp + len, "+CGCONTRDP: %d,5,\"%s\",", Cid, sDyncApn.pApn);
#endif
                else
#ifdef CONFIG_SOC_8910
                    len += sprintf(pResp + len, "+CGCONTRDP: %d,%d,,", Cid, CFW_GprsGetNsapi(Cid, nSim));
#else
                    len += sprintf(pResp + len, "+CGCONTRDP: %d,5,,", Cid);
#endif
                if (!CFW_GprsGetDynamicIP(Cid, &sDyncIP, nSim) && (sDyncIP.nPdpAddrSize > 0) && (sDyncIP.pPdpAddr))
                {
                    len += sprintf(pResp + len, "\"");
#if LWIP_IPV6
                    if (sDyncIP.nPdpAddrSize == 16)
                    {
                        const ip6_addr_t *ip6 = AT_GprsGetPdpIpv6Address(Cid, nSim);
                        sDyncIP.pPdpAddr[0] = (ip6->addr[0] & 0XFF);
                        sDyncIP.pPdpAddr[1] = (ip6->addr[0] >> 8) & 0XFF;
                        sDyncIP.pPdpAddr[2] = (ip6->addr[0] >> 16) & 0XFF;
                        sDyncIP.pPdpAddr[3] = (ip6->addr[0] >> 24) & 0XFF;
                        sDyncIP.pPdpAddr[4] = (ip6->addr[1] & 0XFF);
                        sDyncIP.pPdpAddr[5] = (ip6->addr[1] >> 8) & 0XFF;
                        sDyncIP.pPdpAddr[6] = (ip6->addr[1] >> 16) & 0XFF;
                        sDyncIP.pPdpAddr[7] = (ip6->addr[1] >> 24) & 0XFF;
                    }
                    else if (sDyncIP.nPdpAddrSize == 20)
                    {
                        const ip6_addr_t *ip6 = AT_GprsGetPdpIpv6Address(Cid, nSim);
                        sDyncIP.pPdpAddr[4] = (ip6->addr[0] & 0XFF);
                        sDyncIP.pPdpAddr[5] = (ip6->addr[0] >> 8) & 0XFF;
                        sDyncIP.pPdpAddr[6] = (ip6->addr[0] >> 16) & 0XFF;
                        sDyncIP.pPdpAddr[7] = (ip6->addr[0] >> 24) & 0XFF;
                        sDyncIP.pPdpAddr[8] = (ip6->addr[1] & 0XFF);
                        sDyncIP.pPdpAddr[9] = (ip6->addr[1] >> 8) & 0XFF;
                        sDyncIP.pPdpAddr[10] = (ip6->addr[1] >> 16) & 0XFF;
                        sDyncIP.pPdpAddr[11] = (ip6->addr[1] >> 24) & 0XFF;
                    }
#endif
                    for (uint8_t uCnt = 0; uCnt < sDyncIP.nPdpAddrSize; uCnt++)
                    {
                        len += sprintf(pResp + len, "%d.", (sDyncIP.pPdpAddr[uCnt] >> 4) * 16 + ((sDyncIP.pPdpAddr[uCnt]) & 0x0F));
                    }
                    // if()
                    for (uint8_t uCnt = 0; uCnt < sDyncIP.nPdpAddrSize; uCnt++)
                    {
                        len += sprintf(pResp + len, "0.");
                    }
                    len += sprintf(pResp + len - 1, "\"");
                }
            }
            if (len > 0)
                atCmdRespInfoText(cmd->engine, pResp);
        }

        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type) // Test command.
    {
        uint8_t nActState;
        int i = 0;
        bool has_cid_actived = false;
        for (i = 1; i <= AT_PDPCID_MAX; i++)
        {
            CFW_GetGprsActState(i, &nActState, nSim);
            if (nActState == CFW_GPRS_ACTIVED)
                has_cid_actived = true;
        }
        if (has_cid_actived)
        {
            char rsp[64];
            char *prsp = rsp;
            prsp += sprintf(prsp, "+CGCONTRDP: ");
            for (i = 1; i <= AT_PDPCID_MAX; i++)
            {
                CFW_GetGprsActState(i, &nActState, nSim);
                if (nActState == CFW_GPRS_ACTIVED)
                    prsp += sprintf(prsp, "%d,", i);
            }
            atCmdRespInfoNText(cmd->engine, (char *)rsp, strlen(rsp) - 1);
        }
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleCGSCONTRDP(atCommand_t *cmd)
{
    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont = {};
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t len = 0;
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        char pResp[100] = {0};
        uint8_t Cid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        if (!paramok || cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (!CFW_GprsGetPdpCxtV2(Cid, &sPdpCont, nSim))
        {
            AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[Cid];
            uint8_t uP_id = 0;
#ifdef LTE_NBIOT_SUPPORT
            uP_id = pinfo->uPCid;
#endif
#ifdef CONFIG_SOC_8910
            len += sprintf(pResp + len, "+CGSCONTRDP: %d,%d,%d", Cid, uP_id, CFW_GprsGetNsapi(Cid, nSim));
#else
            len += sprintf(pResp + len, "+CGSCONTRDP: %d,%d,5", Cid, uP_id);
#endif
            if (len > 0)
                atCmdRespInfoText(cmd->engine, pResp);
            pinfo->uCid = Cid;
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_EXE == cmd->type)
    {
        char pResp[100] = {0};
        for (int Cid = AT_PDPCID_MIN; Cid <= AT_PDPCID_MAX; Cid++)
        {
            AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[Cid];
            memset(pResp, 0, sizeof(pResp));
            uint8_t uP_id = 0;
#ifdef LTE_NBIOT_SUPPORT
            uP_id = pinfo->uPCid;
#endif
            len = 0;
            if (!CFW_GprsGetPdpCxtV2(Cid, &sPdpCont, nSim))
            {
#ifdef CONFIG_SOC_8910
                len += sprintf(pResp + len, "+CGSCONTRDP: %d,%d,%d", Cid, uP_id, CFW_GprsGetNsapi(Cid, nSim));
#else
                len += sprintf(pResp + len, "+CGSCONTRDP: %d,%d,5", Cid, uP_id);
#endif
                if (len > 0)
                    atCmdRespInfoText(cmd->engine, pResp);
                pinfo->uCid = Cid;
            }
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type) // Test command.
    {
        atCmdRespInfoText(cmd->engine, "+CGSCONTRDP: cid= (1,2,3,4,5,6,7)");
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

void atCmdHandleCGDEL(atCommand_t *cmd)
{
    uint8_t Cid;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t uCidState = 0;
    char rsp[100] = {
        0,
    };
    CFW_EQOS sEqos = {
        0,
    };
    CFW_GPRS_QOS sQos = {
        0,
    };
    CFW_GPRS_PDPCONT_INFO_V2 pdp_cont;
    memset(&sEqos, 0, sizeof(CFW_EQOS));
    memset(&pdp_cont, 0, sizeof(pdp_cont));
    AT_Gprs_CidInfo *g_staAtGprsCidInfo_nv = gAtSetting.g_staAtGprsCidInfo[nSim];

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;

        // This command parameter count  largest or equat 1.
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        Cid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        int ret = CFW_GetGprsActState(Cid, &uCidState, nSim);
        if (uCidState == CFW_GPRS_DEACTIVED && ret == ERR_SUCCESS)
        {
            if (CFW_GprsGetPdpCxtV2(Cid, &pdp_cont, nSim) == 0)
            {
                //CFW_GprsDelPFInDeactProc(NULL, pUserData->Cid2Index.nNsapi, pUserData->Cid2Index.nLinkedEbi, nSimID);
#if defined(LTE_NBIOT_SUPPORT) || defined(LTE_SUPPORT)
                CFW_GprsSetCtxEQos(Cid, &sEqos, nSim); //clr CGEQOS

                CFW_GprsSetReqQosUmts(Cid, &sQos, nSim); // clr CGEQREQ and CGEQMIN
#endif
                g_staAtGprsCidInfo_nv[Cid].uCid = 0;
                g_staAtGprsCidInfo_nv[Cid].nPdpType = 0;
                g_staAtGprsCidInfo_nv[Cid].nDComp = 0;
                g_staAtGprsCidInfo_nv[Cid].nHComp = 0;
                g_staAtGprsCidInfo_nv[Cid].nApnSize = 0;
                g_staAtGprsCidInfo_nv[Cid].nPdpAddrSize = 0;
                memset(g_staAtGprsCidInfo_nv[Cid].pApn, 0, AT_GPRS_APN_MAX_LEN);
                memset(g_staAtGprsCidInfo_nv[Cid].pPdpAddr, 0, AT_GPRS_PDPADDR_MAX_LEN);
                atCfgAutoSave();
                CFW_GprsRemovePdpCxt(Cid, nSim);
                memset(&gAtCfwCtx.sim[nSim].tft_set[Cid], 0, sizeof(CFW_TFT_SET));      //clr CGTFT
                memset(&gAtCfwCtx.sim[nSim].cid_info[Cid], 0, sizeof(AT_Gprs_CidInfo)); //clr CGDSCONT

                CFW_GprsSetMinQos(Cid, &sQos, nSim); //clr CGQMIN
                CFW_GprsSetReqQos(Cid, &sQos, nSim); //clr CGQREQ

                sprintf(rsp, "+CGDEL: %d", Cid);
                atCmdRespInfoText(cmd->engine, rsp);
            }
            RETURN_OK(cmd->engine);
        }
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
    else if (AT_CMD_EXE == cmd->type)
    {
        char *prsp = rsp;
        uint8_t del_flag = 0;
        prsp += sprintf(rsp, "+CGDEL: ");
        for (Cid = AT_PDPCID_MIN; Cid <= AT_PDPCID_MAX; Cid++)
        {
            int ret = CFW_GetGprsActState(Cid, &uCidState, nSim);
            if (uCidState == CFW_GPRS_DEACTIVED && ret == ERR_SUCCESS)
            {
                if (CFW_GprsGetPdpCxtV2(Cid, &pdp_cont, nSim) == 0)
                {
                    //CFW_GprsDelPFInDeactProc(NULL, pUserData->Cid2Index.nNsapi, pUserData->Cid2Index.nLinkedEbi, nSimID);
#if defined(LTE_NBIOT_SUPPORT) || defined(LTE_SUPPORT)
                    CFW_GprsSetCtxEQos(Cid, &sEqos, nSim);
                    CFW_GprsSetReqQosUmts(Cid, &sQos, nSim); // clr CGEQREQ and CGEQMIN
#endif
                    g_staAtGprsCidInfo_nv[Cid].uCid = 0;
                    g_staAtGprsCidInfo_nv[Cid].nPdpType = 0;
                    g_staAtGprsCidInfo_nv[Cid].nDComp = 0;
                    g_staAtGprsCidInfo_nv[Cid].nHComp = 0;
                    g_staAtGprsCidInfo_nv[Cid].nApnSize = 0;
                    g_staAtGprsCidInfo_nv[Cid].nPdpAddrSize = 0;
                    memset(g_staAtGprsCidInfo_nv[Cid].pApn, 0, AT_GPRS_APN_MAX_LEN);
                    memset(g_staAtGprsCidInfo_nv[Cid].pPdpAddr, 0, AT_GPRS_PDPADDR_MAX_LEN);
                    atCfgAutoSave();
                    CFW_GprsRemovePdpCxt(Cid, nSim);
                    memset(&gAtCfwCtx.sim[nSim].tft_set[Cid], 0, sizeof(CFW_TFT_SET));      //clr CGTFT
                    memset(&gAtCfwCtx.sim[nSim].cid_info[Cid], 0, sizeof(AT_Gprs_CidInfo)); //clr CGDSCONT

                    CFW_GprsSetMinQos(Cid, &sQos, nSim); //clr CGQMIN
                    CFW_GprsSetReqQos(Cid, &sQos, nSim); //clr CGQREQ
                    if (del_flag == 1)
                        prsp += sprintf(prsp, ",");
                    prsp += sprintf(prsp, "%d", Cid);
                    if (del_flag == 0)
                        del_flag = 1;
                }
            }
        }
        if (del_flag)
            atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type) // Test command.
    {
        atCmdRespInfoText(cmd->engine, "+CGDEL: (1-7)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void AT_SetPdpCfg(uint8_t nSimID)
{
#ifdef PORTING_ON_GOING
    uint8_t nSim = nSimID;
    uint8_t *pApn = NULL;
    uint8_t *userName = NULL;
    uint8_t *passWord = NULL;
    uint8_t *pPdpAddr = NULL;
    int32_t iResult;
    CFW_GPRS_PDPCONT_INFO sPdpCont;
    uint8_t uCid = 1;

    if (NULL == pApn)
    {
        pApn = AT_MALLOC(AT_GPRS_APN_MAX_LEN + 1);
    }
    if (NULL == pPdpAddr)
    {
        pPdpAddr = AT_MALLOC(AT_GPRS_PDPADDR_MAX_LEN + 1);
    }
    if (NULL == userName)
    {
        userName = AT_MALLOC(AT_GPRS_USR_MAX_LEN + 1);
    }
    if (NULL == passWord)
    {
        passWord = AT_MALLOC(AT_GPRS_PAS_MAX_LEN + 1);
    }
    for (uCid = 1; uCid <= 7; uCid++)
    {
        if (g_staAtGprsCidInfo_e[nSimID][uCid].uCid != uCid)
        {
            OSI_LOGI(0x10004237, "AT_SetPdpCfg 0:uCid  %d", g_staAtGprsCidInfo_e[nSimID][uCid].uCid);
            continue;
        }
        AT_MemSet(&sPdpCont, 0, SIZEOF(CFW_GPRS_PDPCONT_INFO));
        sPdpCont.nApnSize = g_staAtGprsCidInfo_e[nSimID][uCid].nApnSize;
        AT_MemSet(pApn, 0, AT_GPRS_APN_MAX_LEN + 1);
        AT_StrCpy(pApn, g_staAtGprsCidInfo_e[nSimID][uCid].pApn);
        if (0 != sPdpCont.nApnSize)
        {
            sPdpCont.pApn = pApn;
        }

        sPdpCont.nApnUserSize = g_staAtGprsCidInfo_e[nSimID][uCid].nUsernameSize;
        AT_MemSet(userName, 0, AT_GPRS_USR_MAX_LEN + 1);
        AT_StrCpy(userName, g_staAtGprsCidInfo_e[nSimID][uCid].uaUsername);
        if (0 != sPdpCont.nApnUserSize)
        {
            sPdpCont.pApnUser = userName;
        }
        sPdpCont.nApnPwdSize = g_staAtGprsCidInfo_e[nSimID][uCid].nPasswordSize;
        AT_MemSet(passWord, 0, AT_GPRS_PAS_MAX_LEN + 1);
        AT_StrCpy(passWord, g_staAtGprsCidInfo_e[nSimID][uCid].uaPassword);
        if (0 != sPdpCont.nApnPwdSize)
        {
            sPdpCont.pApnPwd = passWord;
        }
        sPdpCont.nPdpType = g_staAtGprsCidInfo_e[nSimID][uCid].nPdpType;
        sPdpCont.nDComp = g_staAtGprsCidInfo_e[nSimID][uCid].nDComp;
        sPdpCont.nHComp = g_staAtGprsCidInfo_e[nSimID][uCid].nHComp;

#ifndef AT_NO_GPRS
        // Call csw api to set the pdp cxt.
        iResult = CFW_GprsSetPdpCxt(uCid, &sPdpCont, nSim);
#endif

        OSI_LOGI(0x10004238, "AT_SetPdpCfg:**************uCid %d******************", uCid);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004239, "AT_SetPdpCfg:apn %s", g_staAtGprsCidInfo_e[nSimID][uCid].pApn);
        OSI_LOGI(0x1000423a, "AT_SetPdpCfg:nPdpType %d", g_staAtGprsCidInfo_e[nSimID][uCid].nPdpType);
    }
    if (NULL != pPdpAddr)
    {
        AT_FREE(pPdpAddr);
        pPdpAddr = NULL;
    }
    if (NULL != pApn)
    {
        AT_FREE(pApn);
        pApn = NULL;
    }

    if (NULL != passWord)
    {
        AT_FREE(passWord);
        passWord = NULL;
    }
    if (NULL != userName)
    {
        AT_FREE(userName);
        userName = NULL;
    }
#endif
}

extern uint8_t SUL_ascii2hex(uint8_t *pInput, uint8_t *pOutput);
//first parameter is ascii string holding MINvalue.MAXvalue
//Range is output parameter, High 16 bits hold  MINvalue, low 16 bits hold MAXvalue
//return 0 if error, other return 1
uint8_t at_GetRange(const char *StrRang, uint32_t *Range)
{
    uint8_t i = 0;
    uint16_t value = 0;
    uint8_t length = strlen(StrRang);
    OSI_LOGXI(OSI_LOGPAR_S, 0x1000423b, "StrRang = %s", StrRang);
    for (i = 0; i < length; i++)
    {
        uint8_t c = StrRang[i];
        if ((c >= '0') && (c <= '9'))
        {
            c = c - '0';
            value = value * 10 + c;
            OSI_LOGI(0x1000423c, "value = %d", value);
        }
        else if (c == '.')
        {
            if (i > 5)
                return 0;
            *Range = value;
            value = 0;
            OSI_LOGI(0x1000423d, "*Range = %d", *Range);
        }
        else
            return 0;
    }
    //*Range = htons(*Range) << 16;
    //*Range |= htons(value);
    *Range = *Range << 16;
    *Range |= value;
    //*Range = htonl(*Range);
    OSI_LOGI(0x1000423e, "High 16 Range = %d", *Range >> 16);
    OSI_LOGI(0x1000423f, "Low 16 Range = %d", (*Range) & 0xFFFF);
    return 1;
}

uint8_t at_ascii2hex(uint8_t *pInput, uint8_t *pOutput)
{
    uint16_t i = 0;
    uint16_t length = strlen((char *)pInput);
    uint16_t j = length;

    uint8_t nData;
    uint8_t *pBuffer = pOutput;
    uint8_t *pIn = pInput;

    if ((length & 0x01) == 0x01)
    {
        nData = pIn[0];
        if ((nData >= '0') && (nData <= '9'))
            *pBuffer = nData - '0';
        else if ((nData >= 'a') && (nData <= 'f'))
            *pBuffer = nData - 'a' + 10;
        else if ((nData >= 'A') && (nData <= 'F'))
            *pBuffer = nData - 'A' + 10;
        else
            return 0;
        length--;
        pBuffer++;
        pIn++;
    }

    for (i = 0; i < length; i++)
    {
        if ((i & 0x01) == 0x01) // even is high 4 bits, it should be shift 4 bits for left.
            *pBuffer = (*pBuffer) << 4;
        else
            *pBuffer = 0;

        nData = pIn[i];
        OSI_LOGI(0x10004241, "nData = %d", nData);
        if ((nData >= '0') && (nData <= '9'))
            *pBuffer |= nData - '0';
        else if ((nData >= 'a') && (nData <= 'f'))
            *pBuffer |= nData - 'a' + 10;
        else if ((nData >= 'A') && (nData <= 'F'))
            *pBuffer |= nData - 'A' + 10;
        else
            return 0;
        if ((i & 0x01) == 0x01)
            pBuffer++;
    }
    if ((j & 0x01) == 0x01)
        length++;
    OSI_LOGI(0, "pOutput = %x,%x,%x, length = %d", pOutput[0], pOutput[1], pOutput[2], (length + 1) >> 1);
    return (length + 1) >> 1;
}

#if defined(LTE_NBIOT_SUPPORT) || defined(LTE_SUPPORT)
// 10.1.3 Traffic flow template
void atCmdHandleCGTFT(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (AT_CMD_SET == cmd->type)
    {
        // +CGTFT=<cid>,[<packet filter identifier>,<evaluation precedence index>
        // [,<remote address and subnet mask>[,<protocol number (ipv4) / next header (ipv6)>
        // [,<local port range>[,<remote port range>[,<ipsec security parameter index (spi)>
        // [,<type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask>
        // [,<flow label (ipv6)>[,<direction>[,<local address and subnet mask>]]]]]]]]]]

        bool paramok = true;
        uint8_t cid = AT_PDPCID_MIN;
        if (cmd->param_count > 12)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (cmd->param_count > 0)
        {
            cid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN,
                                     AT_PDPCID_MAX, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            if (cmd->param_count == 1)
            {
                memset(&gAtCfwCtx.sim[nSim].tft_set[cid], 0, sizeof(CFW_TFT_SET));
                RETURN_OK(cmd->engine);
            }
        }

        CFW_TFT_SET *tft = (CFW_TFT_SET *)calloc(1, sizeof(CFW_TFT_SET));
        atMemFreeLater(tft);

        if (cmd->param_count > 1)
        {
            unsigned packet_filter_id = atParamUintInRange(cmd->params[1], 1, 16, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (atParamIsEmpty(cmd->params[2]))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            tft->Pf_ID = packet_filter_id;
            tft->Pf_BitMap |= 0x8000;
            OSI_LOGI(0x10004249, "CGTFT: packet_filter_id = %d", packet_filter_id);
        }

        if (cmd->param_count > 2)
        {
            unsigned eval_prece_index = atParamUintInRange(cmd->params[2], 0, 255, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            tft->Pf_EvaPreIdx = eval_prece_index;
            tft->Pf_BitMap |= 0x4000;
            OSI_LOGI(0x1000424b, "CGTFT: eval_prece_index = %d", eval_prece_index);
        }

        if (cmd->param_count > 3)
        {
            /* For Example: "192.168.100.100.255.255.255.255" */
            const char *SourceMask = atParamStr(cmd->params[3], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            int count = 1;
            for (int i = 0; i <= strlen(SourceMask); i++)
            {
                if (SourceMask[i] == '.' || SourceMask[i] == ':')
                    count++;
            }
            if (count != 8 && count != 32)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (count == 8)
            {
                if (!_paramDotUint8(SourceMask, 8, tft->Pf_RmtAddrIPv4))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                tft->Pf_BitMap |= 0x2000;
            }
            else
            {
                if (!_paramDotUint8(SourceMask, 32, tft->Pf_RmtAddrIPv6))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                tft->Pf_BitMap |= 0x1000;
            }
        }

        if (cmd->param_count > 4)
        {
            unsigned protocol = atParamDefUintInRange(cmd->params[4], 0, 0, 255, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            OSI_LOGI(0x10004257, "CGTFT: protocol = 0x%x", protocol);
            tft->Pf_PortNum_NH = protocol;
            tft->Pf_BitMap |= 0x0800;
        }

        if (cmd->param_count > 5)
        {
            /* For Example: "60000.60000" */
            const char *port_range = atParamDefStr(cmd->params[5], "00000.00000", &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            unsigned port_min, port_max;
            if (sscanf(port_range, "%u.%u", &port_min, &port_max) != 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGI(0, "port_min 6 = %u", port_min);
            OSI_LOGI(0, "port_max 6 = %u", port_max);

            if (port_min >= 0x10000 || port_max >= 0x10000 || port_min > port_max)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            tft->Pf_LocalPortRangeMin = port_min;
            tft->Pf_LocalPortRangeMax = port_max;
            tft->Pf_BitMap |= 0x0400;
        }

        if (cmd->param_count > 6)
        {
            const char *port_range = atParamDefStr(cmd->params[6], "00000.00000", &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            unsigned port_min, port_max;
            if (sscanf(port_range, "%u.%u", &port_min, &port_max) != 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            OSI_LOGI(0, "port_min 7 = %u", port_min);
            OSI_LOGI(0, "port_max 7 = %u", port_max);

            if (port_min >= 0x10000 || port_max >= 0x10000 || port_min > port_max)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            tft->Pf_RemotPortRangeMin = port_min;
            tft->Pf_RemotPortRangeMax = port_max;
            tft->Pf_BitMap |= 0x0200;
        }

        if (cmd->param_count > 7)
        {
            uint32_t ipsec_index = 0;
            if (!atParamIsEmpty(cmd->params[7]))
            {
                const char *ipsec = atParamRawText(cmd->params[7], &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t hex_len;
                uint8_t ipsec_hex[9] = {0};
                if ((ipsec != NULL) && strcmp(ipsec, ""))
                {
                    if (strlen(ipsec) > 8) //more than 0XFFFFFFFF
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    hex_len = at_ascii2hex((uint8_t *)ipsec, ipsec_hex);
                    if (hex_len == 0)
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    ipsec_index = 0;
                    for (int i = 0; i < hex_len; i++)
                        ipsec_index = (ipsec_index << 8) + ipsec_hex[i];
                }
            }

            OSI_LOGI(0, "CGTFT: ipsec_index  = %x", ipsec_index);
            tft->Pf_SPI = ipsec_index;
            tft->Pf_BitMap |= 0x0100;
        }

        if (cmd->param_count > 8)
        {
            const char *type_of_service = atParamOptStr(cmd->params[8], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if ((NULL != type_of_service) && strcmp(type_of_service, ""))
            {
                if (!_paramDotUint8(type_of_service, 2, tft->Pf_TOS_TC))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            tft->Pf_BitMap |= 0x0080;
        }

        if (cmd->param_count > 9)
        {
            uint32_t flow_label = 0;
            if (!atParamIsEmpty(cmd->params[9]))
            {
                const char *FlowLabel = atParamRawText(cmd->params[9], &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                OSI_LOGXI(OSI_LOGPAR_S, 0x100052b5, "CGTFT: FlowLabel  = %s", FlowLabel);

                uint8_t FlowLabel_hex[6] = {0};
                uint8_t hex_len;
                if ((NULL != FlowLabel) && strcmp(FlowLabel, ""))
                {
                    if (strlen(FlowLabel) > 5) //more than 0XFFFFF
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    hex_len = at_ascii2hex((uint8_t *)FlowLabel, FlowLabel_hex);
                    if (hex_len == 0)
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    flow_label = 0;
                    for (int i = 0; i < hex_len; i++)
                        flow_label = (flow_label << 8) + FlowLabel_hex[i];
                }
            }

            OSI_LOGI(0x10005665, "CGTFT: flow_label  = %x", flow_label);
            tft->Pf_FlowLabel = flow_label;
            tft->Pf_BitMap |= 0x0100;
        }

        if (cmd->param_count > 10)
        {
            unsigned direction = atParamDefUintInRange(cmd->params[10], 3, 0, 3, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            OSI_LOGI(0x10004266, "CGTFT: direction = %d", direction);
            tft->Pf_Direct = direction;
            tft->Pf_BitMap |= 0x0020;
        }

        if (cmd->param_count > 11)
        {
            const char *SourceMask = atParamOptStr(cmd->params[11], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            int count = 1;
            for (int i = 0; i <= strlen(SourceMask); i++)
            {
                if (SourceMask[i] == '.' || SourceMask[i] == ':')
                    count++;
            }
            if (count != 8 && count != 32)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (count == 8)
            {
                if (!_paramDotUint8(SourceMask, 8, tft->Pf_LocalAddrIPv4))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                tft->Pf_BitMap |= 0x0010;
            }
            else
            {
                if (!_paramDotUint8(SourceMask, 32, tft->Pf_LocalAddrIPv6))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                tft->Pf_BitMap |= 0x0008;
            }
        }

        if ((ret = CFW_GprsSetCtxTFT(cid, tft, nSim)) != 0)
            RETURN_CME_CFW_ERR(cmd->engine, ret);

        gAtCfwCtx.sim[nSim].tft_set[cid] = *tft;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char rsp[256];
        for (int cid = 1; cid < 8; cid++)
        {
            CFW_TFT_SET *tft = &gAtCfwCtx.sim[nSim].tft_set[cid];
            if (tft->Pf_ID == 0)
                continue;

            sprintf(rsp, "+CGTFT: %d, %d, %d, %d.%d.%d.%d.%d.%d.%d.%d, %d,%d.%d,%d.%d,%x,%d.%d,%x,%d,%d.%d.%d.%d.%d.%d.%d.%d",
                    cid, tft->Pf_ID, tft->Pf_EvaPreIdx,
                    tft->Pf_RmtAddrIPv4[0], tft->Pf_RmtAddrIPv4[1],
                    tft->Pf_RmtAddrIPv4[2], tft->Pf_RmtAddrIPv4[3],
                    tft->Pf_RmtAddrIPv4[4], tft->Pf_RmtAddrIPv4[5],
                    tft->Pf_RmtAddrIPv4[6], tft->Pf_RmtAddrIPv4[7],
                    tft->Pf_PortNum_NH,
                    tft->Pf_LocalPortRangeMin, tft->Pf_LocalPortRangeMax,
                    tft->Pf_RemotPortRangeMin, tft->Pf_RemotPortRangeMax,
                    (unsigned)tft->Pf_SPI,
                    tft->Pf_TOS_TC[0], tft->Pf_TOS_TC[1], (unsigned)tft->Pf_FlowLabel, tft->Pf_Direct,
                    tft->Pf_LocalAddrIPv4[0], tft->Pf_LocalAddrIPv4[1],
                    tft->Pf_LocalAddrIPv4[2], tft->Pf_LocalAddrIPv4[3],
                    tft->Pf_LocalAddrIPv4[4], tft->Pf_LocalAddrIPv4[5],
                    tft->Pf_LocalAddrIPv4[6], tft->Pf_LocalAddrIPv4[7]);
            atCmdRespInfoText(cmd->engine, rsp);
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        /* atCmdRespInfoText(cmd->engine, "+CGTFT:  (IP), (1-8),(0-255).(0-255).(0-255).(0-255)."
                                       "(0-255).(0-255).(0-255).(0-255),"
                                       "4,(0-65535).(0-65535),(0-65535).(0-65535),"
                                       "(0-FFFFFFFF),(0-255).(0-255)");*/
        atCmdRespInfoText(cmd->engine, "+CGTFT: (IP,IPV4V6,IPV6), (1-16),(0-255),(0-255).(0-255).(0-255).(0-255).(0-255).(0-255).(0-255).(0-255),"
                                       "(0-255),(0-65535).(0-65535),(0-65535).(0-65535),"
                                       "(0-FFFFFFFF),(0-255).(0-255),(0-FFFFF),(0-3),(0-255).(0-255).(0-255).(0-255).(0-255).(0-255).(0-255).(0-255)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleCGTFTRDP(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t t_cid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint8_t nActState = 0;
        CFW_GetGprsActState(t_cid, &nActState, nSim);
        if (nActState == CFW_GPRS_ACTIVED)
        {
            CFW_TFT_SET *tft = &gAtCfwCtx.sim[nSim].tft_set[t_cid];
            if (tft->Pf_ID == 0)
                RETURN_OK(cmd->engine);
            char rsp[256] = {0};
            sprintf(rsp, "+CGTFTRDP: %d,%d,%d,%d.%d.%d.%d.%d.%d.%d.%d,%d,%d.%d,%d.%d,%x,%d.%d,%x,%d,%d,%d.%d.%d.%d.%d.%d.%d.%d",
                    t_cid, tft->Pf_ID, tft->Pf_EvaPreIdx,
                    tft->Pf_RmtAddrIPv4[0], tft->Pf_RmtAddrIPv4[1],
                    tft->Pf_RmtAddrIPv4[2], tft->Pf_RmtAddrIPv4[3],
                    tft->Pf_RmtAddrIPv4[4], tft->Pf_RmtAddrIPv4[5],
                    tft->Pf_RmtAddrIPv4[6], tft->Pf_RmtAddrIPv4[7],
                    tft->Pf_PortNum_NH,
                    tft->Pf_LocalPortRangeMin, tft->Pf_LocalPortRangeMax,
                    tft->Pf_RemotPortRangeMin, tft->Pf_RemotPortRangeMax,
                    (unsigned)tft->Pf_SPI,
                    tft->Pf_TOS_TC[0], tft->Pf_TOS_TC[1], (unsigned)tft->Pf_FlowLabel, tft->Pf_Direct,
                    tft->Pf_ID,
                    tft->Pf_LocalAddrIPv4[0], tft->Pf_LocalAddrIPv4[1],
                    tft->Pf_LocalAddrIPv4[2], tft->Pf_LocalAddrIPv4[3],
                    tft->Pf_LocalAddrIPv4[4], tft->Pf_LocalAddrIPv4[5],
                    tft->Pf_LocalAddrIPv4[6], tft->Pf_LocalAddrIPv4[7]);
            atCmdRespInfoText(cmd->engine, rsp);
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_EXE == cmd->type)
    {
        char rsp[256] = {0};
        for (int cid = AT_PDPCID_MIN; cid <= AT_PDPCID_MAX; cid++)
        {
            uint8_t nActState = 0;
            CFW_GetGprsActState(cid, &nActState, nSim);
            if (nActState == CFW_GPRS_ACTIVED)
            {
                CFW_TFT_SET *tft = &gAtCfwCtx.sim[nSim].tft_set[cid];
                if (tft->Pf_ID == 0)
                    continue;

                sprintf(rsp, "+CGTFTRDP: %d,%d,%d,%d.%d.%d.%d.%d.%d.%d.%d,%d,%d.%d,%d.%d,%x,%d.%d,%x,%d,%d,%d.%d.%d.%d.%d.%d.%d.%d",
                        cid, tft->Pf_ID, tft->Pf_EvaPreIdx,
                        tft->Pf_RmtAddrIPv4[0], tft->Pf_RmtAddrIPv4[1],
                        tft->Pf_RmtAddrIPv4[2], tft->Pf_RmtAddrIPv4[3],
                        tft->Pf_RmtAddrIPv4[4], tft->Pf_RmtAddrIPv4[5],
                        tft->Pf_RmtAddrIPv4[6], tft->Pf_RmtAddrIPv4[7],
                        tft->Pf_PortNum_NH,
                        tft->Pf_LocalPortRangeMin, tft->Pf_LocalPortRangeMax,
                        tft->Pf_RemotPortRangeMin, tft->Pf_RemotPortRangeMax,
                        (unsigned)tft->Pf_SPI,
                        tft->Pf_TOS_TC[0], tft->Pf_TOS_TC[1], (unsigned)tft->Pf_FlowLabel, tft->Pf_Direct,
                        tft->Pf_ID,
                        tft->Pf_LocalAddrIPv4[0], tft->Pf_LocalAddrIPv4[1],
                        tft->Pf_LocalAddrIPv4[2], tft->Pf_LocalAddrIPv4[3],
                        tft->Pf_LocalAddrIPv4[4], tft->Pf_LocalAddrIPv4[5],
                        tft->Pf_LocalAddrIPv4[6], tft->Pf_LocalAddrIPv4[7]);
                atCmdRespInfoText(cmd->engine, rsp);
            }
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CGTFTRDP: cid=(1,2,3,4,5,6,7)");
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

#endif

#if defined(LTE_NBIOT_SUPPORT) || defined(LTE_SUPPORT)
typedef struct
{
    uint8_t cids[AT_PDPCID_MAX - AT_PDPCID_MIN + 1];
    uint8_t count;
    uint8_t pos;
} cgcmodAsyncContext_t;

static void CGCMOD_RspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    cgcmodAsyncContext_t *async = (cgcmodAsyncContext_t *)cmd->async_ctx;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cfw_event->nParam2 != 0)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

    async->pos++;
    if (async->pos >= async->count)
        RETURN_OK(cmd->engine);

    cmd->uti = cfwRequestUTI((osiEventCallback_t)CGCMOD_RspCB, cmd);
    if (0 != CFW_GprsCtxEpsModify(cmd->uti, async->cids[async->pos], nSim))
    {
        cfwReleaseUTI(cmd->uti);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    RETURN_FOR_ASYNC();
}

// 10.1.11 PDP context modify
void atCmdHandleCGCMOD(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (AT_CMD_SET == cmd->type)
    {
        // +CGCMOD[=<cid>[,<cid>[,...]]]
        bool paramok = true;
        if ((cmd->param_count > AT_PDPCID_MAX - AT_PDPCID_MIN + 1) || cmd->param_count == 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        cgcmodAsyncContext_t *async = (cgcmodAsyncContext_t *)calloc(1, sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        async->count = cmd->param_count;
        async->pos = 0;

        for (uint8_t uIndex = 0; uIndex < cmd->param_count; uIndex++)
        {
            uint8_t uCid = atParamUintInRange(cmd->params[uIndex], AT_PDPCID_MIN,
                                              AT_PDPCID_MAX, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            if (0 == gAtCfwCtx.sim[nSim].cid_info[uCid].uCid)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            for (int n = 0; n < uIndex; n++)
            {
                if (uCid == async->cids[n])
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            async->cids[uIndex] = uCid;
        }

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CGCMOD_RspCB, cmd);
        if (0 != CFW_GprsCtxEpsModify(cmd->uti, async->cids[0], nSim))
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        RETURN_FOR_ASYNC();
    }
    else if (AT_CMD_EXE == cmd->type)
    {
        cgcmodAsyncContext_t *async = (cgcmodAsyncContext_t *)calloc(1, sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        async->pos = 0;
        async->count = 0;
        uint8_t uState = CFW_GPRS_DEACTIVED;
        uint8_t uIndex = 0;
        for (int uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[uCid];
            if (pinfo->uCid != 0)
            {
                CFW_GetGprsActState(pinfo->uCid, &uState, nSim);
                if (CFW_GPRS_ACTIVED != uState)
                    continue;
                else
                {
                    async->cids[uIndex] = uCid;
                    async->count++;
                    uIndex++;
                }
            }
        }
        cmd->uti = cfwRequestUTI((osiEventCallback_t)CGCMOD_RspCB, cmd);
        if (0 != CFW_GprsCtxEpsModify(cmd->uti, async->cids[0], nSim))
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        RETURN_FOR_ASYNC();
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        uint8_t nActState;
        int i = 0;
        bool has_cid_actived = false;
        for (i = 1; i <= AT_PDPCID_MAX; i++)
        {
            CFW_GetGprsActState(i, &nActState, nSim);
            if (nActState == CFW_GPRS_ACTIVED)
                has_cid_actived = true;
        }
        if (has_cid_actived)
        {
            char rsp[64];
            char *prsp = rsp;
            prsp += sprintf(prsp, "+CGCMOD: ");
            for (i = 1; i <= AT_PDPCID_MAX; i++)
            {
                CFW_GetGprsActState(i, &nActState, nSim);
                if (nActState == CFW_GPRS_ACTIVED)
                    prsp += sprintf(prsp, "%d,", i);
            }
            atCmdRespInfoNText(cmd->engine, (char *)rsp, strlen(rsp) - 1);
        }
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
#endif

// 10.1.19 Packet domain event reporting
void atCmdHandleCGEREP(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (AT_CMD_SET == cmd->type)
    {
        // +CGEREP=[<mode>[,<bfr>]]
        bool paramok = true;
        uint8_t mode = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);
        uint8_t bfr = atParamDefUintInRange(cmd->params[1], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtGprsCGEVFlag = mode;
        gAtSetting.sim[nSim].cgerep_mode = mode;
        gAtSetting.sim[nSim].cgerep_bfr = bfr;
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type) // Read command
    {
        // +CGEREP: <mode>,<bfr>
        char rsp[64];
        sprintf(rsp, "+CGEREP: %d,%d", gAtSetting.sim[nSim].cgerep_mode,
                gAtSetting.sim[nSim].cgerep_bfr);

        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CGEREP: (0,1),(0,1)");
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

// This function to process the AT+CGQREQ command. include set,read and test command.
//
// Set command
// +CGQREQ=[<cid> [,<precedence > [,<delay> [,<reliability.> [,<peak> [,<mean>]]]]]]
// Possible response(s)
// OK
// ERROR
//
// Read command
// +CGQREQ?
// Possible response(s)
// +CGQREQ: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
// [<CR><LF>+CGQREQ: <cid>, <precedence >, <delay>, <reliability.>, <peak>, <mean>
// [��]]
//
// Test command
// +CGQREQ=?
// Possible response(s)
// +CGQREQ: <PDP_type>, (list of supported <precedence>s), (list of supported <delay>s),
// (list of supported <reliability>s) , (list of supported <peak>s), (list of supported <mean>s)
// [<CR><LF>+CGQREQ: <PDP_type>, (list of supported <precedence>s), (list of supported
// <delay>s), (list of supported <reliability>s) , (list of supported <peak>s), (list of supported <mean>s)
// [��]]
//
void atCmdHandleCGQREQ(atCommand_t *cmd)
{
    char *pResp = NULL;
    uint8_t uCid = 0;
    uint32_t uErrCode = 0;
    CFW_GPRS_QOS sQos = {0, 0, 0, 0, 0};
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t iResult = 0;

    if (NULL == cmd)
    {
        OSI_LOGI(0x10003ff9, "AT+CGQREQ:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }

    // Process the command. include icommand parameter checking,csw api calling,and responsing...
    // Set command.
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;

        // This command have 6 parameters.
        if (cmd->param_count < 1 || cmd->param_count > 6)
        {
            OSI_LOGI(0x10003ffa, "AT+CGQREQ:uParamCount error,uParamCount = %d.", cmd->param_count);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        // Get the parameter list.

        // 1#
        // Get cir
        if (cmd->param_count >= 1)
        {
            uCid = atParamDefUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
            OSI_LOGI(0x00000000, "uCid===%d", gAtCfwCtx.sim[nSim].cid_info[uCid].uCid);
            if ((!paramok) || (0 == gAtCfwCtx.sim[nSim].cid_info[uCid].uCid))
            {
                OSI_LOGI(0x10004290, "AT+CGQREQ:Get cir failed.");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }

        // 2#
        // Get precedence
        if (cmd->param_count >= 2)
        {
            sQos.nPrecedence = atParamDefUintInRange(cmd->params[1], 0, 0, 3, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x10004291, "AT+CGQREQ:Get precedence failed.");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }

        // 3#
        // Get delay
        if (cmd->param_count >= 3)
        {
            sQos.nDelay = atParamDefUintInRange(cmd->params[2], 0, 0, 4, &paramok);

            if (!paramok)
            {
                OSI_LOGI(0x10004292, "AT+CGQREQ:Get delay failed");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }

        // 4#
        // Get reliability
        if (cmd->param_count >= 4)
        {
            sQos.nReliability = atParamDefUintInRange(cmd->params[3], 0, 0, 5, &paramok);

            if (!paramok)
            {
                OSI_LOGI(0x10004293, "AT+CGQREQ:Get reliability failed.");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }

        // 5#
        // Get peak
        if (cmd->param_count >= 5)
        {
            sQos.nPeak = atParamDefUintInRange(cmd->params[4], 0, 0, 9, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x10004294, "AT+CGQREQ:Get peak failed.");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }

        // 6#
        // Get mean
        if (cmd->param_count >= 6)
        {
            sQos.nMean = atParamDefUintInRange(cmd->params[5], 0, 0, 31, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x10004295, "AT+CGQREQ:Get mean failed");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (18 < sQos.nMean && 31 != sQos.nMean)
            {
                OSI_LOGI(0x10004006, "AT+CGQREQ:The param sQos.nMean error,sQos.nMean = %d.", sQos.nMean);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
#ifndef AT_NO_GPRS
        iResult = CFW_GprsSetReqQos(uCid, &sQos, nSim);
#endif
        if (0 != iResult)
        {
            OSI_LOGI(0x10004007, "AT+CGQREQ:CFW_GprsSetReqQos() failed,iResult = 0x%x.", iResult);
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
#ifndef CONFIG_QUEC_PROJECT_FEATURE_NW            
            pResp = malloc((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);
            if (NULL == pResp)
            {
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }
            memset(pResp, 0, sizeof(*pResp));

            // +CGQREQ: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
            sprintf(pResp, "+CGQREQ:%d,%d,%d,%d,%d,%d",
                    uCid, sQos.nPrecedence, sQos.nDelay, sQos.nReliability, sQos.nPeak, sQos.nMean);
#endif            
            goto _func_success;
        }
    }

    // Read command
    else if (AT_CMD_READ == cmd->type)
    {
        for (uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
#ifndef AT_NO_GPRS
            iResult = CFW_GprsGetReqQos(uCid, &sQos, nSim);
#endif
            if (0 == iResult)
            {
                if (NULL == pResp)
                {
                    pResp = malloc((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);

                    if (NULL == pResp)
                    {
                        uErrCode = ERR_AT_CME_NO_MEMORY;
                        goto _func_fail;
                    }
                }

                memset(pResp, 0, sizeof(*pResp));

                // +CGQREQ: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
                sprintf(pResp, "+CGQREQ: %d,%d,%d,%d,%d,%d",
                        uCid, sQos.nPrecedence, sQos.nDelay, sQos.nReliability, sQos.nPeak, sQos.nMean);
#else                
                sprintf(pResp, "+CGQREQ:%d,%d,%d,%d,%d,%d",
                        uCid, sQos.nPrecedence, sQos.nDelay, sQos.nReliability, sQos.nPeak, sQos.nMean);
#endif
                atCmdRespInfoText(cmd->engine, pResp);
            }
            else
            {
                OSI_LOGI(0x10004008, "AT+CGQREQ:CFW_GprsGetReqQos() failed,iResult = 0x%x,uCid = %d.", iResult, uCid);
                // The cid of specified is NULL.
                // Do nothing.
            }
        }

        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp[0] = 0;
            goto _func_success;
        }
    }
    // Test command.
    else if (AT_CMD_TEST == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        atCmdRespInfoText(cmd->engine, "+CGQREQ: \"IP\",(0-3),(0-4),(0-5),(0-9),(0-18,31)");
        atCmdRespInfoText(cmd->engine, "+CGQREQ: \"PPP\",(0-3),(0-4),(0-5),(0-9),(0-18,31)");
        atCmdRespInfoText(cmd->engine, "+CGQREQ: \"IPV6\",(0-3),(0-4),(0-5),(0-9),(0-18,31)");
        atCmdRespInfoText(cmd->engine, "+CGQREQ: \"IPV4V6\",(0-3),(0-4),(0-5),(0-9),(0-18,31)");
#else        
        pResp = malloc(128);

        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }

        // [[ yy [mod] 2008-5-13 for bug ID 8472
        memset(pResp, 0, 128);

        strcpy(pResp, "+CGQREQ: IP, (0..3), (0..4), (0..5) , (0..9), (0..18,31)");

        atCmdRespInfoText(cmd->engine, pResp);
        memset(pResp, 0, 128);

        strcpy(pResp, "+CGQREQ: IPV6, (0..3), (0..4), (0..5) , (0..9), (0..18,31)");

        atCmdRespInfoText(cmd->engine, pResp);
        memset(pResp, 0, 128);

        strcpy(pResp, "+CGQREQ: PPP, (0..3), (0..4), (0..5) , (0..9), (0..18,31)");

        atCmdRespInfoText(cmd->engine, pResp);
        free(pResp);
#endif        
        RETURN_OK(cmd->engine);
        // ]] yy [mod] 2008-5-13 for bug ID 8472
    }
    else
    {
        // [[ yy [mod] 2008-6-3 for bug ID
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
    }

_func_success:
#ifndef CONFIG_QUEC_PROJECT_FEATURE_NW    
    atCmdRespInfoText(cmd->engine, pResp);
#endif
    atCmdRespOK(cmd->engine);
    if (NULL != pResp)
    {
        free(pResp);
        pResp = NULL;
    }
    return;

_func_fail:
    if (NULL != pResp)
    {
        free(pResp);
        pResp = NULL;
    }

    RETURN_CME_ERR(cmd->engine, uErrCode);
}

// 10.1.28 UE modes of operation for EPS
void atCmdHandleCEMODE(atCommand_t *cmd)
{
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        if (CFW_CfgGetPocEnable())
        {
            OSI_LOGI(0, "couldn't set cemode in POC mode");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        }
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        bool paramok = true;
        uint32_t mode = atParamDefUintInRange(cmd->params[0], 2, 0, 3, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        CFW_EmodSetCEMode(mode, nSim);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32] = {0};
        uint8_t pMode;
        CFW_EmodGetCEMode(&pMode, nSim);
        if (CFW_CfgGetPocEnable())
        {
            OSI_LOGI(0, "cemode alway be 0 in POC mode");
            pMode = 0;
        }
        sprintf(rsp, "+CEMODE: %d", pMode);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char rsp[32] = {0};
        sprintf(rsp, "+CEMODE: (0, 1, 2, 3)");
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

#if defined(LTE_NBIOT_SUPPORT) || defined(LTE_SUPPORT)
// 10.1.6 3G quality of service profile (requested)
void atCmdHandleCGEQREQ(atCommand_t *cmd)
{
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
#if 0
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);
    CFW_GPRS_QOS sQos;
    memset(&sQos, 0x00, sizeof(CFW_GPRS_QOS));

    if (AT_CMD_SET == cmd->type)
    {
        // +CGEQREQ=[<cid>[,<Traffic class>[,<Maximum bitrate UL>[,<Maximum bitrate DL>
        // [,<Guaranteed bitrate UL>[,<Guaranteed bitrate DL>[,<Delivery order>
        // [,<Maximum SDU size>[,<SDU error ratio>[,<Residual bit error ratio>
        // [,<Delivery of erroneous SDUs>[,<Transfer delay>[,<Traffic handling priority>
        // [,<Source statistics descriptor>[,<Signalling indication>]]]]]]]]]]]]]]]

        bool paramok = true;
        // make it mandatory
        uint8_t uCid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        sQos.nTrafficClass = atParamDefUintInRange(cmd->params[1], 0, 0, 4, &paramok);
        sQos.nMaximumbitrateUL = atParamDefUintInRange(cmd->params[2], 0, 0, 65535, &paramok);
        sQos.nMaximumbitrateDL = atParamDefUintInRange(cmd->params[3], 0, 0, 65535, &paramok);
        sQos.nGuaranteedbitrateUL = atParamDefUintInRange(cmd->params[4], 0, 0, 65535, &paramok);
        sQos.nGuaranteedbitrateDL = atParamDefUintInRange(cmd->params[5], 0, 0, 65535, &paramok);
        sQos.nDeliveryOrder = atParamDefUintInRange(cmd->params[6], 0, 0, 2, &paramok);
        sQos.nMaximumSDUSize = atParamDefUintInRange(cmd->params[7], 1500, 10, 1520, &paramok);
        sQos.nSDUErrorRatio = atParamDefUintByStrMap(cmd->params[8], 0xf, gQosSDUErrorRatio, &paramok);
        sQos.nResidualBER = atParamDefUintByStrMap(cmd->params[9], 0xf, gQosResidualBER, &paramok);
        sQos.nDeliveryOfErroneousSDU = atParamDefUintInRange(cmd->params[10], 0, 0, 3, &paramok);
        sQos.nTransferDelay = atParamDefUintInRange(cmd->params[11], 0, 0, 4000, &paramok);
        sQos.nTrafficHandlingPriority = atParamDefUintInRange(cmd->params[12], 1, 1, 3, &paramok);
        uint8_t source_state = atParamDefUintInRange(cmd->params[13], 0, 0, 1, &paramok);
        uint8_t signal_ind = atParamDefUintInRange(cmd->params[14], 0, 0, 1, &paramok);

        (void)source_state;
        (void)signal_ind;

        if (!paramok || cmd->param_count > 15)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if ((cmd->param_count < 1) || (cmd->param_count > 13))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (CFW_GprsSetReqQosUmts(uCid, &sQos, nSim) != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char rsp[160];
        for (uint8_t uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            if (CFW_GprsGetReqQosUmts(uCid, &sQos, nSim) != 0)
                continue;

            const osiValueStrMap_t *nSDUErrorRatio_m = osiVsmapFindByVal(gQosSDUErrorRatio, sQos.nSDUErrorRatio);
            const osiValueStrMap_t *nResidualBER_m = osiVsmapFindByVal(gQosResidualBER, sQos.nResidualBER);
            if (nSDUErrorRatio_m == NULL || nResidualBER_m == NULL)
            {
                sprintf(rsp, "+CGEQREQ: %d,%d,%d,%d,%d,%d,%d,%d, , ,%d,%d,%d",
                        uCid, sQos.nTrafficClass, sQos.nMaximumbitrateUL,
                        sQos.nMaximumbitrateDL, sQos.nGuaranteedbitrateUL,
                        sQos.nGuaranteedbitrateDL, sQos.nDeliveryOrder,
                        sQos.nMaximumSDUSize, sQos.nDeliveryOfErroneousSDU,
                        sQos.nTransferDelay, sQos.nTrafficHandlingPriority);
            }
            else
            {
                sprintf(rsp, "+CGEQREQ: %d,%d,%d,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,%d",
                        uCid, sQos.nTrafficClass, sQos.nMaximumbitrateUL,
                        sQos.nMaximumbitrateDL, sQos.nGuaranteedbitrateUL,
                        sQos.nGuaranteedbitrateDL, sQos.nDeliveryOrder,
                        sQos.nMaximumSDUSize, nSDUErrorRatio_m->str,
                        nResidualBER_m->str, sQos.nDeliveryOfErroneousSDU,
                        sQos.nTransferDelay, sQos.nTrafficHandlingPriority);
            }
            atCmdRespInfoText(cmd->engine, rsp);
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CGEQREQ: IP,(0,1,2,3,4),,,,,(0,1,2),,,,(0,1,2,3),,,(0,1),(0,1)");
        atCmdRespInfoText(cmd->engine, "+CGEQREQ: IPV6,(0,1,2,3,4),,,,,(0,1,2),,,,(0,1,2,3),,,(0,1),(0,1)");
        atCmdRespInfoText(cmd->engine, "+CGEQREQ: PPP,(0,1,2,3,4),,,,,(0,1,2),,,,(0,1,2,3),,,(0,1),(0,1)");
        atCmdRespInfoText(cmd->engine, "+CGEQREQ: IPV4V6,(0,1,2,3,4),,,,,(0,1,2),,,,(0,1,2,3),,,(0,1),(0,1)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
#endif
}

void atCmdHandleCGEQMIN(atCommand_t *cmd)
{
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
#if 0
    CFW_GPRS_QOS sQos;
    memset(&sQos, 0, sizeof(CFW_GPRS_QOS));
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if ((cmd->param_count < 1) || (cmd->param_count > 13))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t uCid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        sQos.nTrafficClass = atParamDefUintInRange(cmd->params[1], 0, 0, 3, &paramok);
        sQos.nMaximumbitrateUL = atParamDefUintInRange(cmd->params[2], 0, 0, 65535, &paramok);
        sQos.nMaximumbitrateDL = atParamDefUintInRange(cmd->params[3], 0, 0, 65535, &paramok);
        sQos.nGuaranteedbitrateUL = atParamDefUintInRange(cmd->params[4], 0, 0, 65535, &paramok);
        sQos.nGuaranteedbitrateDL = atParamDefUintInRange(cmd->params[5], 0, 0, 65535, &paramok);
        sQos.nDeliveryOrder = atParamDefUintInRange(cmd->params[6], 0, 0, 1, &paramok);
        sQos.nMaximumSDUSize = atParamDefUintInRange(cmd->params[7], 1500, 10, 1520, &paramok);
        sQos.nSDUErrorRatio = atParamDefUintByStrMap(cmd->params[8], 0xf, gQosSDUErrorRatio, &paramok);
        sQos.nResidualBER = atParamDefUintByStrMap(cmd->params[9], 0xf, gQosResidualBER, &paramok);
        sQos.nDeliveryOfErroneousSDU = atParamDefUintInRange(cmd->params[10], 0, 0, 2, &paramok);
        sQos.nTransferDelay = atParamDefUintInRange(cmd->params[11], 0, 0, 4000, &paramok);
        sQos.nTrafficHandlingPriority = atParamDefUintInRange(cmd->params[12], 1, 1, 3, &paramok);

        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (0 != CFW_GprsSetReqQosUmts(uCid, &sQos, nSim))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        RETURN_OK(cmd->engine);
    }

    else if (AT_CMD_READ == cmd->type)
    {
        char rsp[160];
        for (uint8_t uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            if (CFW_GprsGetReqQosUmts(uCid, &sQos, nSim) != 0)
                continue;

            const osiValueStrMap_t *nSDUErrorRatio_m = osiVsmapFindByVal(gQosSDUErrorRatio, sQos.nSDUErrorRatio);
            const osiValueStrMap_t *nResidualBER_m = osiVsmapFindByVal(gQosResidualBER, sQos.nResidualBER);
            if (nSDUErrorRatio_m == NULL || nResidualBER_m == NULL)
                continue;

            sprintf(rsp, "+CGEQREQ: %d,%d,%d,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,%d",
                    uCid, sQos.nTrafficClass, sQos.nMaximumbitrateUL,
                    sQos.nMaximumbitrateDL, sQos.nGuaranteedbitrateUL,
                    sQos.nGuaranteedbitrateDL, sQos.nDeliveryOrder,
                    sQos.nMaximumSDUSize, nSDUErrorRatio_m->str,
                    nResidualBER_m->str, sQos.nDeliveryOfErroneousSDU,
                    sQos.nTransferDelay, sQos.nTrafficHandlingPriority);
            atCmdRespInfoText(cmd->engine, rsp);
        }
        RETURN_OK(cmd->engine);
    }

    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CGEQMIN: IP,(0,1,2,3), , , , ,(0,1), , , ,(0,1,2), , ");
        atCmdRespInfoText(cmd->engine, "+CGEQMIN: IPV6,(0,1,2,3), , , , ,(0,1), , , ,(0,1,2), , ");
        atCmdRespInfoText(cmd->engine, "+CGEQMIN: PPP,(0,1,2,3), , , , ,(0,1), , , ,(0,1,2), , ");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
#endif
}
#endif

uint32_t CFW_getDnsServerbyPdp(uint8_t nCid, uint8_t nSimID);

void atCmdHandleCGPDNSADDR(atCommand_t *cmd)
{
    uint8_t i = 0;
    uint8_t uFlag = 0;
    bool paramok = true;
    char uaOutputStr[240] = {0};
    uint8_t nSim = atCmdGetSim(cmd->engine);
    AT_Gprs_CidInfo *g_staAtGprsCidInfo = gAtCfwCtx.sim[nSim].cid_info;

    switch (cmd->type)
    {
    case AT_CMD_SET:
        // do something...
        // whether we can trim the redunced cid in tail..?
        if (cmd->param_count > AT_PDPCID_MAX)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        // will return all the PDP ctx defined,now csw has defined 7 items[1-7].
        if (cmd->param_count == 0)
            RETURN_OK(cmd->engine);
        else
        {
            uint8_t cids[7] = {0};
            for (i = 0; i < cmd->param_count; i++)
            {
                cids[i] = atParamUintInRange(cmd->params[i], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
                if (!paramok)
                {
                    OSI_LOGI(0x1000429f, "AT_Util_GetParaWithRule");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }
            // disp specified cids
            for (i = 0; i < cmd->param_count; i++)
            {
                uint8_t uCid = cids[i];
                OSI_LOGI(0x100042a0, "g_staAtGprsCidInfo[uCid].uCid %d", g_staAtGprsCidInfo[uCid].uCid);

                if (g_staAtGprsCidInfo[uCid].uCid == 0)
                    continue;
                char addr_str[200] = {0};
                memset(uaOutputStr, 0, sizeof(uaOutputStr));
                CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
                CFW_GprsGetPdpCxtV2(uCid, &sPdpCont, nSim);
                _dnsAddressToStr(&sPdpCont, addr_str);
                sprintf(uaOutputStr, "+CGPDNSADDR: %d,\"%s\"", uCid, addr_str);
                atCmdRespInfoText(cmd->engine, uaOutputStr);
            }
            RETURN_OK(cmd->engine);
        }

        break;

    case AT_CMD_TEST:
    {
        uint16_t uLen = sizeof("+CGPDNSADDR:(");

        memset(uaOutputStr, 0, 100);

        memcpy(uaOutputStr, "+CGPDNSADDR:(", uLen);
        uLen -= 1;

        for (uint8_t uIndex = AT_PDPCID_MIN; uIndex <= AT_PDPCID_MAX; uIndex++)
        {
            if (g_staAtGprsCidInfo[uIndex].uCid)
            {
                sprintf(&uaOutputStr[uLen], "%d,", g_staAtGprsCidInfo[uIndex].uCid);
                uLen += 2;
                uFlag = 1;
            }
        }

        if (uFlag)
        {
            uaOutputStr[uLen - 1] = ')';
            uaOutputStr[uLen] = '\0';
        }
        else
            memcpy(uaOutputStr, "+CGPDNSADDR:()", sizeof("+CGPDNSADDR:()"));
        atCmdRespInfoText(cmd->engine, uaOutputStr);
        atCmdRespOK(cmd->engine);
        break;
    }

    default:
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
    return;
}

// This function to process the AT+CGQMIN command. include set,read and test command.
//
// Set command
// +CGQMIN=[<cid> [,<precedence > [,<delay> [,<reliability.> [,<peak> [,<mean>]]]]]]
// Possible response(s)
// OK
// ERROR
//
// Read command
// +CGQMIN?
// Possible response(s)
// +CGQMIN: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
// [<CR><LF>+CGQMIN: <cid>, <precedence >, <delay>, <reliability.>, <peak>, <mean>
// [��]]
//
// Test command
// +CGQMIN=?
// Possible response(s)
// +CGQMIN: <PDP_type>, (list of supported <precedence>s), (list of supported <delay>s),
// (list of supported <reliability>s) , (list of supported <peak>s), (list of supported <mean>s)
// [<CR><LF>+CGQMIN: <PDP_type>, (list of supported <precedence>s), (list of supported <delay>s),
// (list of supported <reliability>s) , (list of supported <peak>s), (list of supported <mean>s)
// [��]]
void atCmdHandleCGQMIN(atCommand_t *cmd)
{
    int32_t iResult;
    uint8_t uCid = 0;
    CFW_GPRS_QOS sQos = {0, 0, 0, 0, 0};
    uint8_t nSim = atCmdGetSim(cmd->engine);

    char *rsp = (char *)malloc(64);
    atMemFreeLater(rsp);

    // Process the command. include icommand parameter checking,csw api calling,and responsing...
    // Set command.
    if (AT_CMD_SET == cmd->type)
    {
        // This command have 6 parameters.
        if (cmd->param_count < 1 || cmd->param_count > 6)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        bool paramok = true;
        uCid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        //hal_HstSendEvent(0x0514aaaa);
        //hal_HstSendEvent(uCid);
        //hal_HstSendEvent(paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        OSI_LOGI(0, "CGDSCONT set cid is: %d", uCid);
        sQos.nPrecedence = atParamDefUintInRange(cmd->params[1], 0, 0, 3, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        sQos.nDelay = atParamDefUintInRange(cmd->params[2], 0, 0, 4, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        sQos.nReliability = atParamDefUintInRange(cmd->params[3], 0, 0, 5, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        sQos.nPeak = atParamDefUintInRange(cmd->params[4], 0, 0, 9, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        sQos.nMean = atParamDefUintInRange(cmd->params[5], 0, 0, 31, &paramok);
        if (!paramok || (sQos.nMean > 18 && sQos.nMean != 31))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#ifndef AT_NO_GPRS
        iResult = CFW_GprsSetMinQos(uCid, &sQos, nSim);
#endif
        if (0 != iResult)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        // +CGQREQ: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
#ifndef CONFIG_QUEC_PROJECT_FEATURE_NW
        sprintf(rsp, "+CGQMIN:%d,%d,%d,%d,%d,%d",
                uCid, sQos.nPrecedence, sQos.nDelay, sQos.nReliability, sQos.nPeak, sQos.nMean);
        atCmdRespInfoText(cmd->engine, rsp);
#endif        
        atCmdRespOK(cmd->engine);
    }

    // Read command
    else if (AT_CMD_READ == cmd->type)
    {
        for (uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
#ifndef AT_NO_GPRS
            iResult = CFW_GprsGetMInQos(uCid, &sQos, nSim);
#endif
            if (0 != iResult)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            // +CGQREQ: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
            sprintf(rsp, "+CGQMIN: %d,%d,%d,%d,%d,%d",
                    uCid, sQos.nPrecedence, sQos.nDelay, sQos.nReliability, sQos.nPeak, sQos.nMean);
#else
            sprintf(rsp, "+CGQMIN:%d,%d,%d,%d,%d,%d",
                    uCid, sQos.nPrecedence, sQos.nDelay, sQos.nReliability, sQos.nPeak, sQos.nMean);
#endif
            atCmdRespInfoText(cmd->engine, rsp);
        }
        atCmdRespOK(cmd->engine);
    }

    // Test command.
    else if (AT_CMD_TEST == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        atCmdRespInfoText(cmd->engine, "+CGQMIN: \"IP\",(0-3),(0-4),(0-5),(0-9),(0-18,31)");
        atCmdRespInfoText(cmd->engine, "+CGQMIN: \"PPP\",(0-3),(0-4),(0-5),(0-9),(0-18,31)");
        atCmdRespInfoText(cmd->engine, "+CGQMIN: \"IPV6\",(0-3),(0-4),(0-5),(0-9),(0-18,31)");
        atCmdRespInfoText(cmd->engine, "+CGQMIN: \"IPV4V6\",(0-3),(0-4),(0-5),(0-9),(0-18,31)");
#else
        atCmdRespInfoText(cmd->engine, "+CGQMIN: (IP,PPP,IPV6,IPV4V6), (0..3), (0..4), (0..5) , (0..9), (0..18,31)");
#endif
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

static void _cgattGprsAttRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    // EV_CFW_GPRS_ACGRQTT_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nParam1 != 0)
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);

    atCmdRespOK(cmd->engine);
}

void atCmdHandleCGATT(atCommand_t *cmd)
{
    uint32_t ret;
    uint8_t att_state = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    char rsp[32];
#if 0
    if (CLASS_TYPE_CC == gAtCfwCtx.g_uClassType)
    {
        OSI_LOGI(0x100042a2, "Gprs in class cc mode,can not attach");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
#endif
    if (AT_CMD_SET == cmd->type)
    {
#ifdef CONFIG_DUAL_SIM_SUPPORT
        if (CFW_CfgGetPocEnable())
        {
            if (atGetPocMasterCard() != nSim)
            {
                OSI_LOGI(0, "couldn't att another sim in POC mode");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
        }
#endif
        bool paramok = true;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        uint8_t uState = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
#else
        uint8_t uState = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
#endif
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        //if CFUN=0, not allowed do attach or detach
        CFW_COMM_MODE nFM = CFW_DISABLE_COMM;
        if (ERR_SUCCESS != CFW_GetComm(&nFM, nSim))
        {
            OSI_LOGI(0, "CGATT CFW_GetComm fail");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (nFM == CFW_DISABLE_COMM)
        {
            OSI_LOGI(0, "in flight mode,not allowed do attach or detach");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        }

        if (CFW_GetSimStatus(nSim) == CFW_SIM_ABSENT)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        ret = CFW_GetGprsAttState(&att_state, nSim);
        if ((0 != ret) || ((0 != att_state) && (1 != att_state) && (2 != att_state)))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

#if 0 //now no gct test, delete temporarily
        if (uState == 2) //Add for gcf test by wulc
        {
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cgattGprsAttRsp, cmd);
            uint8_t AttDetachType = 1 /*API_PS_ATTACH_GPRS*/ | 8 /*API_PS_ATTACH_FOR*/;
            if ((ret = CFW_AttDetach(CFW_GPRS_ATTACHED, cmd->uti, AttDetachType, nSim)) != 0)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
#endif
        CFW_NW_STATUS_INFO sStatus;
        ret = CFW_GprsGetstatus(&sStatus, nSim);
        if (ret != 0)
            RETURN_CME_CFW_ERR(cmd->engine, ret);
        if (sStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || sStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
        {
            sStatus.nStatus = 1;
        }
        else
        {
            sStatus.nStatus = 0;
        }
        if (uState == sStatus.nStatus)
        {
            if (CFW_CheckTestSim((CFW_SIM_ID)nSim) == 0)
                RETURN_OK(cmd->engine);
        }
        if (uState == CFW_GPRS_ATTACHED)
        {
            //GSMonly and in call not allowed CGATT=1
            CFW_CC_CURRENT_CALL_INFO call_info[AT_CC_MAX_NUM];
            uint8_t nCnt = 0;
            uint8_t nCurrRat = CFW_NWGetStackRat(nSim);
            CFW_CcGetCurrentCall(call_info, &nCnt, nSim);
            OSI_LOGI(0, "in atCmdHandleCGATT nCnt:%d  nCurrRat:%d", nCnt, nCurrRat);
            if ((nCnt != 0) && (nCurrRat == 2))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
        }
        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cgattGprsAttRsp, cmd);
        uint8_t AttDetachType = 1 /*API_PS_ATTACH_GPRS*/;
        if ((ret = CFW_AttDetach(uState, cmd->uti, AttDetachType, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_CFW_ERR(cmd->engine, ret);
        }
        RETURN_FOR_ASYNC();
        // TODO: AT_GPRS_ATTDELAY
    }
    else if (AT_CMD_READ == cmd->type)
    {
        CFW_NW_STATUS_INFO sStatus;
        ret = CFW_GprsGetstatus(&sStatus, nSim);
        if (ret != 0)
            RETURN_CME_CFW_ERR(cmd->engine, ret);
        if (sStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || sStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
        {
            sStatus.nStatus = 1;
        }
        else
        {
            sStatus.nStatus = 0;
        }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        sprintf(rsp, "+CGATT: %d", sStatus.nStatus);
#else
        sprintf(rsp, "+CGATT:%d", sStatus.nStatus);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }

    else if (AT_CMD_TEST == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        atCmdRespInfoText(cmd->engine, "+CGATT: (0,1)");
#else        
        atCmdRespInfoText(cmd->engine, "+CGATT:(0,1)");
#endif
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

typedef struct
{
    uint8_t act_state;
    uint8_t cid_count;
    uint8_t cid_index;
    uint8_t cids[AT_PDPCID_MAX];
} cgactAsyncCtx_t;

static void _cgactActHead(atCommand_t *cmd);
static void _cgactGprsActRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    uint8_t nCid, nSimId;
    // EV_CFW_GPRS_ACT_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    cgactAsyncCtx_t *async = (cgactAsyncCtx_t *)cmd->async_ctx;

    if (cfw_event->nType != async->act_state)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		quec_datacall_fail_handler(cfw_event->nFlag, event->param1);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG
		quec_netlight_deal_pdp_active_fail(async->act_state);
#endif
#endif
        AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED));
    }

    if (event->id == EV_CFW_GPRS_ACT_RSP)
    {
        if (cfw_event->nType == CFW_GPRS_ACTIVED)
        {
            osiEvent_t tcpip_event = *event;
            tcpip_event.id = EV_TCPIP_CFW_GPRS_ACT;
            CFW_EVENT *tcpip_cfw_event = (CFW_EVENT *)&tcpip_event;
            tcpip_cfw_event->nUTI = 0;
            osiEventSend(netGetTaskID(), (const osiEvent_t *)&tcpip_event);
        }
        else if (cfw_event->nType == CFW_GPRS_DEACTIVED)
        {
            //Delete ppp Session first
            OSI_LOGI(0, "ppp session will be deleted!!! GprsActRsp");
            nCid = event->param1;
            nSimId = cfw_event->nFlag;
            pppSessionDeleteByNetifDestoryed(nSimId, nCid);

            osiEvent_t tcpip_event = *event;
            tcpip_event.id = EV_TCPIP_CFW_GPRS_DEACT;
            CFW_EVENT *tcpip_cfw_event = (CFW_EVENT *)&tcpip_event;
            tcpip_cfw_event->nUTI = 0;
            osiEventSend(netGetTaskID(), (const osiEvent_t *)&tcpip_event);
        }
    }
    if (cfw_event->nType == CFW_GPRS_ACTIVED)
    {
        char pRsp[100] = {0};
        uint8_t nSim = atCmdGetSim(cmd->engine);
        CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
        if (CFW_GprsGetPdpCxtV2(async->cids[async->cid_index], &sPdpCont, nSim) == 0)
        {
            char addr_str[80] = {};
            uint8_t cid_act_state = 0;
            nCid = event->param1;
            CFW_GetGprsActState(async->cids[async->cid_index], &cid_act_state, nSim);
            OSI_LOGI(0, "cid_act_state:%d   sPdpCont.nPdpType:%d   sPdpCont.PdnType:%d   sPdpCont.nApnSize: %d", cid_act_state, sPdpCont.nPdpType, sPdpCont.PdnType, sPdpCont.nApnSize);
            _pdpAddressToStr(&sPdpCont, addr_str);
            sprintf(pRsp, "+CGACT: %d, %d, %s", async->cids[async->cid_index], async->act_state, addr_str);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
            OSI_LOGI(0, "_cgactGprsActRsp receive CFW_GPRS_ACTIVED pRsp: %d", pRsp);
#else
            atCmdRespInfoText(cmd->engine, pRsp);
#endif
        }
    }
    async->cid_index++;
    if (async->cid_index < async->cid_count)
        _cgactActHead(cmd);
    else
        RETURN_OK(cmd->engine);
}

static void _cgactGprsAttRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    // EV_CFW_GPRS_ATT_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nType != CFW_GPRS_ATTACHED)
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);

    _cgactActHead(cmd);
}

static void _cgactActHead(atCommand_t *cmd)
{
    cgactAsyncCtx_t *async = (cgactAsyncCtx_t *)cmd->async_ctx;

    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t cid = async->cids[async->cid_index];
    uint32_t uRet = ERR_SUCCESS;
    uint8_t count = AT_GprsGetActivePdpCount(nSim);
    OSI_LOGI(0, "_cgactActHead ActivePdpCount= %d", count);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
	if ((async->act_state == CFW_GPRS_ACTIVED) && (CFW_ImsIsSet(nSim) == 1) && (cid > PROFILE_IDX_VOLTE_MAX) && (CFW_NWGetStackRat(nSim) == 4))
    {
        atCmdRespInfoText(cmd->engine, "VoLTE on,the number of PDP can be activate is 5");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
#else
    if ((async->act_state == CFW_GPRS_ACTIVED) && (CFW_ImsIsSet(nSim) == 1) && (count >= 5) && (CFW_NWGetStackRat(nSim) == 4))
    {
        atCmdRespInfoText(cmd->engine, "The current setvolte=1,The number of can be activate is 5");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
#endif
    cmd->uti = cfwRequestUTI((osiEventCallback_t)_cgactGprsActRsp, cmd);
    AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[cid];
    OSI_LOGI(0, "pinfo->uCid:%d", pinfo->uCid);

#ifdef LTE_NBIOT_SUPPORT
    OSI_LOGI(0, "pinfo->uPCid:%d", pinfo->uPCid);
    if (pinfo->uPCid != 0)
    {
        uRet = CFW_GprsSecPdpAct(async->act_state, cid, pinfo->uPCid, cmd->uti, nSim);
    }
    else
#endif
    {
        uRet = CFW_GprsAct(async->act_state, cid, cmd->uti, nSim);
    }
    if (ERR_SUCCESS != uRet)
    {
        cfwReleaseUTI(cmd->uti);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    else
    {
		quec_set_datacall_status(nSim, cid, (async->act_state == CFW_GPRS_ACTIVED)?QL_DATACALL_ACTIVING:QL_DATACALL_DEACTIVING);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG
	    quec_netlight_deal_pdp_activing(async->act_state);
#endif
    }
#endif
    //AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[cid];
    pinfo->uCid = cid;
    pinfo->uState = async->act_state;
    RETURN_FOR_ASYNC();
}
static void _cgattRspTimeOutCB(atCommand_t *cmd)
{
    if (AT_CFW_UTI_INVALID != cmd->uti)
        cfwReleaseUTI(cmd->uti);
    AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_SEND_TIMEOUT));
}

bool AT_GetOperatorDefaultApn(uint8_t pOperatorId[6], const char **pOperatorDefaltApn);
// 10.1.10 PDP context activate or deactivate
void atCmdHandleCGACT(atCommand_t *cmd)
{
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (CFW_NWGetStackRat(nSim) == 2 && atCfwCcIsActive())
    {
        OSI_LOGI(0, "cgact isn't allowed, in cs call mode");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
#if 0
    if (CLASS_TYPE_CC == gAtCfwCtx.g_uClassType)
    {
        //class cc mode,can not active
        OSI_LOGI(0x100042b1, "Gprs in class cc mode,can not active");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
#endif
    // get gprs att state
    uint8_t att_state = 0;
    ret = CFW_GetGprsAttState(&att_state, nSim);
    if ((0 != ret) || ((0 != att_state) && (1 != att_state)))
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
    if (AT_CMD_SET == cmd->type)
    {
#ifdef CONFIG_DUAL_SIM_SUPPORT
        if (CFW_CfgGetPocEnable())
        {
            if (atGetPocMasterCard() != nSim)
            {
                OSI_LOGI(0, "couldn't act another sim in POC mode");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
        }
#endif
        bool paramok = true;
        CFW_GPRS_PDPCONT_INFO_V2 pPdpCont;
        uint8_t act_state = 0;
        uint8_t nCid = 0;
        uint8_t cid_act_state = 0;

        if(2 != cmd->param_count)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        act_state = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        nCid = atParamUintInRange(cmd->params[1], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        //if CFUN=0, not allowed do act or deact
        CFW_COMM_MODE nFM = CFW_DISABLE_COMM;
        if (ERR_SUCCESS != CFW_GetComm(&nFM, nSim))
        {
            OSI_LOGI(0, "CGATT CFW_GetComm fail");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (nFM == CFW_DISABLE_COMM)
        {
            OSI_LOGI(0, "in flight mode,not allowed do act or deact");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        }

        AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[nCid];
        if (ERR_SUCCESS != CFW_GprsGetPdpCxtV2(nCid, &pPdpCont, nSim))
        {
            memset(&pPdpCont, 0, sizeof(pPdpCont));
            ql_default_pdp_info_t dft_pdp = {0};
			if(quec_get_default_pdn_info(nSim, &dft_pdp))
			{
				pPdpCont.PdnType = dft_pdp.pdp_type;
				pPdpCont.nPdpType = dft_pdp.pdp_type;
				pPdpCont.nApnSize = dft_pdp.apn_size;
				memcpy(pPdpCont.pApn, dft_pdp.apn_name, dft_pdp.apn_size);
			}
			else
			{
				pPdpCont.PdnType = CFW_GPRS_PDP_TYPE_IP;
				pPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IP;
			}
			
			if (CFW_GprsSetPdpCxtV2(nCid, &pPdpCont, nSim) != 0)
			{
        		RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
			}
        }
        pinfo->uCid = nCid;

        // filter out non-touch CID
        cgactAsyncCtx_t *async = (cgactAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        async->act_state = act_state;
        async->cid_count = 0;
        async->cid_index = 0;

        CFW_GetGprsActState(nCid, &cid_act_state, nSim);
        if (cid_act_state != act_state)
        {
            async->cids[async->cid_count] = nCid;
            async->cid_count++;
        }

        if (async->cid_count == 0)
            RETURN_OK(cmd->engine);

        // if not attach, Attach GPRS.
        if ((att_state == CFW_GPRS_DETACHED) && (act_state == CFW_GPRS_ACTIVED))
        {
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cgactGprsAttRsp, cmd);
            atCmdSetTimeoutHandler(cmd->engine, 1000 * 60, _cgattRspTimeOutCB);
            if ((ret = CFW_AttDetach(CFW_GPRS_ATTACHED, cmd->uti, ATTACH_TYPE, nSim)) != 0)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
        else
        {
            _cgactActHead(cmd);
        }
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
        uint8_t act_state = 0;
        CFW_GPRS_PDPCONT_INFO_V2 pPdpCont;
        for (int n = AT_PDPCID_MIN; n <= AT_PDPCID_MAX; n++)
        {
            if (ERR_SUCCESS != CFW_GprsGetPdpCxtV2(n, &pPdpCont, nSim))
            {
                continue;
            }

            if (CFW_GetGprsActState(n, &act_state, nSim) != 0)
            {
                continue;
            }

            sprintf(rsp, "+CGACT: %d,%d", n, act_state == CFW_GPRS_ACTIVED? 1:0);
            atCmdRespInfoText(cmd->engine, rsp);
        }
        atCmdRespOK(cmd->engine);
    }  
#else
    if (AT_CMD_SET == cmd->type)
    {
#ifdef CONFIG_DUAL_SIM_SUPPORT
        if (CFW_CfgGetPocEnable())
        {
            if (atGetPocMasterCard() != nSim)
            {
                OSI_LOGI(0, "couldn't act another sim in POC mode");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
        }
#endif
        bool paramok = true;
        CFW_GPRS_PDPCONT_INFO_V2 pPdpCont;
        uint8_t act_state = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > AT_PDPCID_MAX + 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        //if CFUN=0, not allowed do act or deact
        CFW_COMM_MODE nFM = CFW_DISABLE_COMM;
        if (ERR_SUCCESS != CFW_GetComm(&nFM, nSim))
        {
            OSI_LOGI(0, "CGATT CFW_GetComm fail");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (nFM == CFW_DISABLE_COMM)
        {
            OSI_LOGI(0, "in flight mode,not allowed do act or deact");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        }

        uint8_t cids[AT_PDPCID_MAX - AT_PDPCID_MIN + 1];
        uint8_t cid_count = cmd->param_count - 1;

        if (cid_count > 0)
        {
            for (int n = 0; n < cid_count; n++)
                cids[n] = atParamUintInRange(cmd->params[n + 1], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        //undefined cid not allow active
        if (act_state)
        {
            for (int n = 0; n < cid_count; n++)
            {
                AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[cids[n]];
                if (ERR_SUCCESS != CFW_GprsGetPdpCxtV2(cids[n], &pPdpCont, nSim))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                else
                    pinfo->uCid = cids[n];
                OSI_LOGI(0, "pinfo->uCid %d :%d", n, pinfo->uCid);
                if (0 == pinfo->uCid)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
        }

        if (cid_count == 0)
        {
            if (act_state == CFW_GPRS_ACTIVED) //AT+CGACT=1
            {
                //if there are some defined cid, CGACT=1 active all defined cids
                for (int n = AT_PDPCID_MIN; n <= AT_PDPCID_MAX; n++)
                {
                    AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[n];
                    OSI_LOGI(0, "pinfo->uCid %d :%d", n, pinfo->uCid);
                    if (0 == pinfo->uCid || ERR_SUCCESS != CFW_GprsGetPdpCxtV2(pinfo->uCid, &pPdpCont, nSim))
                        continue;
                    else
                    {
                        cids[cid_count] = n;
                        cid_count++;
                    }
                }
                //if no difined cid, CGACT=1 active cid 1
                if (cid_count == 0)
                {
                    cids[0] = 1;
                    cid_count = 1;
                    //defined cid 1 PDP context
                    uint8_t oper_id[6];
                    uint8_t mode;
                    uint32_t ret;
                    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
                    memset(&sPdpCont, 0x00, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
                    ret = CFW_GprsGetPdpCxtV2(cids[0], &sPdpCont, nSim);
                    CFW_GPRS_PDPCONT_DFTPDN_INFO nPdnInfo;
                    memset(&nPdnInfo, 0, sizeof(CFW_GPRS_PDPCONT_DFTPDN_INFO));
                    CFW_GprsGetDefaultPdnInfo(&nPdnInfo, nSim);
                    if ((ret == ERR_SUCCESS && sPdpCont.nApnSize == 0) || (ret != ERR_SUCCESS)) //APN
                    {
                        if (CFW_NWGetStackRat(nSim) == 4 && nPdnInfo.APNLen > 0 && nPdnInfo.APNLen <= THE_APN_MAX_LEN) //LTE mode use defult pdn's IP type and APN
                        {
                            //use defult pdn info
                            uint8_t i = 0;
                            for (i = 0; i < nPdnInfo.APNLen; i++)
                            {
                                if (nPdnInfo.AccessPointName[i] == 0x2E) //find first "."
                                {
                                    OSI_LOGI(0, "nPdnInfo.AccessPointName[%d]:0x%x", i, nPdnInfo.AccessPointName[i]);
                                    nPdnInfo.APNLen = i;
                                    break;
                                }
                            }
                            sPdpCont.nApnSize = nPdnInfo.APNLen;
                            memcpy(sPdpCont.pApn, &(nPdnInfo.AccessPointName[0]), nPdnInfo.APNLen);
                        }
                        else //GSM mode use IPV4 type and auto adapted APN
                        {
                            if (CFW_NwGetCurrentOperator(oper_id, &mode, nSim) != 0)
                                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                            const char *defaultApnInfo = NULL;
                            if (AT_GetOperatorDefaultApn(oper_id, &defaultApnInfo))
                            {
                                sPdpCont.nApnSize = strlen(defaultApnInfo);
                                OSI_LOGI(0, "defult sPdpCont.nApnSize: %d APN:", sPdpCont.nApnSize);
                                for (int n = 0; n < sPdpCont.nApnSize; n++)
                                {
                                    OSI_LOGI(0, " %c", sPdpCont.pApn[n]);
                                }
                                if (sPdpCont.nApnSize >= THE_APN_MAX_LEN)
                                {
                                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                                }
                                memcpy(sPdpCont.pApn, defaultApnInfo, sPdpCont.nApnSize);
                            }
                        }
                    }
                    if ((ret == ERR_SUCCESS && sPdpCont.nPdpType == 0) || (ret != ERR_SUCCESS))
                    {
                        if (CFW_NWGetStackRat(nSim) == 4 &&
                            (nPdnInfo.nPdpType == CFW_GPRS_PDP_TYPE_IP ||
                             nPdnInfo.nPdpType == CFW_GPRS_PDP_TYPE_IPV6 ||
                             nPdnInfo.nPdpType == CFW_GPRS_PDP_TYPE_IPV4V6)) //LTE mode use defult pdn's IP type
                        {
                            //use defult pdn info
                            sPdpCont.nPdpType = nPdnInfo.nPdpType;
                        }
                        else //GSM mode use IPV4 type and auto adapted APN
                        {
                            sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IP; /* Default IPV4 */
                        }
                    }
                    sPdpCont.nDComp = 0;
                    sPdpCont.nHComp = 0;
                    sPdpCont.nNSLPI = 0;
                    CFW_GPRS_QOS Qos = {3, 4, 3, 4, 16};
                    CFW_GprsSetReqQos(cids[0], &Qos, nSim);
                    if (CFW_GprsSetPdpCxtV2(cids[0], &sPdpCont, nSim) != 0)
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            else //AT+CGACT=0 deactive all actived cid
            {
                uint8_t uState = CFW_GPRS_DEACTIVED;
                for (int n = AT_PDPCID_MIN; n <= AT_PDPCID_MAX; n++)
                {
                    CFW_GetGprsActState(n, &uState, nSim);
                    OSI_LOGI(0, " cids: %d   uState:%d ", n, uState);
                    if (CFW_GPRS_ACTIVED != uState)
                        continue;
                    else
                    {
                        cids[cid_count] = n;
                        cid_count++;
                        OSI_LOGI(0, " cids[ %d]:%d cid_count:%d", cid_count - 1, n, cid_count);
                    }
                }
            }
        }
        // filter out non-touch CID
        cgactAsyncCtx_t *async = (cgactAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        async->act_state = act_state;
        async->cid_count = 0;
        async->cid_index = 0;
        for (int n = 0; n < cid_count; n++)
        {
            uint8_t cid_act_state = 0;
            CFW_GetGprsActState(cids[n], &cid_act_state, nSim);
            if (cid_act_state != act_state)
            {
                async->cids[async->cid_count] = cids[n];
                async->cid_count++;
            }
        }

        if (async->cid_count == 0)
            RETURN_OK(cmd->engine);

        // if not attach, Attach GPRS.
        if ((att_state == CFW_GPRS_DETACHED) &&
            (act_state == CFW_GPRS_ACTIVED) &&
            (async->cid_count > 0))
        {
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cgactGprsAttRsp, cmd);
            atCmdSetTimeoutHandler(cmd->engine, 1000 * 60, _cgattRspTimeOutCB);
            if ((ret = CFW_AttDetach(CFW_GPRS_ATTACHED, cmd->uti, ATTACH_TYPE, nSim)) != 0)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
        else
        {
            _cgactActHead(cmd);
        }
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
        int act_count = 0;
        uint8_t act_state = 0;
        for (int n = AT_PDPCID_MIN; n <= AT_PDPCID_MAX; n++)
        {
            if (CFW_GetGprsActState(n, &act_state, nSim) != 0)
                continue;

            if (act_state == CFW_GPRS_ACTIVED)
            {
                sprintf(rsp, "+CGACT: %d, %d", n, act_state);
                atCmdRespInfoText(cmd->engine, rsp);
                act_count++;
            }
        }

        if (act_count == 0)
            atCmdRespInfoText(cmd->engine, "+CGACT: 0, 0");
        atCmdRespOK(cmd->engine);
    }
#endif    
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CGACT: (0,1)");
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandlePDNACTINFO(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    CFW_PDN_TIMER_MAXCOUNT_INFO pdnTimerAndMaxCount;
    memset(&pdnTimerAndMaxCount, 0, sizeof(CFW_PDN_TIMER_MAXCOUNT_INFO));
    uint8_t nCurrentRat = CFW_NWGetRat(nSim);
    if (AT_CMD_SET == cmd->type)
    {
        //AT^PDNACTINFO=<operwr>,<pdnmode>[,<retrytimer>[,<retrymaxcount>]]
        bool paramok = true;
        uint8_t operwr = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        uint8_t pdnmode = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        OSI_LOGI(0, "PDNACTINFO:operwr=%d pdnmode=%d nCurrentRat=%d", operwr, pdnmode, nCurrentRat);
        if (operwr) // write param
        {
            if (cmd->param_count > 4)
            {
                OSI_LOGI(0, "PDNACTINFO: Write Command param count > 4. error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            if (pdnmode) //active pdn
            {
                if (nCurrentRat == 2) //T3380
                    pdnTimerAndMaxCount.nTimeValue = atParamDefUintInRange(cmd->params[2], 10, 5, 30, &paramok);
                else //T3482
                    pdnTimerAndMaxCount.nTimeValue = atParamDefUintInRange(cmd->params[2], 8, 5, 8, &paramok);
                pdnTimerAndMaxCount.nMaxCount = atParamDefUintInRange(cmd->params[3], 4, 2, 4, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                OSI_LOGI(0, "PDNACTINFO:nTimeValue=%d nMaxCount=%d", pdnTimerAndMaxCount.nTimeValue, pdnTimerAndMaxCount.nMaxCount);
                if (CFW_SetPdnActTimerAndMaxCount(pdnTimerAndMaxCount, nCurrentRat, nSim) != 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            else //deactive pdn
            {
                if (nCurrentRat == 2) //T3390
                    pdnTimerAndMaxCount.nTimeValue = atParamDefUintInRange(cmd->params[2], 5, 2, 8, &paramok);
                else //T3492
                    pdnTimerAndMaxCount.nTimeValue = atParamDefUintInRange(cmd->params[2], 6, 2, 6, &paramok);
                pdnTimerAndMaxCount.nMaxCount = atParamDefUintInRange(cmd->params[3], 4, 2, 4, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                OSI_LOGI(0, "PDNACTINFO:nTimeValue=%d nMaxCount=%d", pdnTimerAndMaxCount.nTimeValue, pdnTimerAndMaxCount.nMaxCount);
                if (CFW_SetPdnDeactTimerAndMaxCount(pdnTimerAndMaxCount, nCurrentRat, nSim) != 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        else //read param
        {
            if (cmd->param_count > 2)
            {
                OSI_LOGI(0, "PDNACTINFO: Read Command param count > 2. error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            char rspStr[30] = {0};
            if (pdnmode)
            {
                if (CFW_GetPdnActTimerAndMaxCount(&pdnTimerAndMaxCount, nCurrentRat, nSim) != 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                sprintf(rspStr, "^PDNACTINFO: %d, %d", pdnTimerAndMaxCount.nTimeValue, pdnTimerAndMaxCount.nMaxCount);
                atCmdRespInfoText(cmd->engine, rspStr);
            }
            else
            {
                if (CFW_GetPdnDeactTimerAndMaxCount(&pdnTimerAndMaxCount, nCurrentRat, nSim) != 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                sprintf(rspStr, "^PDNACTINFO: %d, %d", pdnTimerAndMaxCount.nTimeValue, pdnTimerAndMaxCount.nMaxCount);
                atCmdRespInfoText(cmd->engine, rspStr);
            }
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "^PDNACTINFO:(0-1),(0-1),(GSM:[act:5-30 deact:2-8]/LTE:[act:5-8 deact:2-6]),(2-4)");
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

#ifdef GCF_TEST
struct CGSEND_Status
{
    uint8_t nDLCI;
    uint8_t nCid;
    uint32_t nDataLen;
    CFW_GPRS_DATA *pGprsData;
};

static struct CGSEND_Status s_CGSEND_Status = {0, 0, 0, NULL};

uint8_t AT_GPRS_Get_CGSEND_Channel()
{
    OSI_LOGI(0x100042c0, "AT_GPRS_Get_CGSEND_Channel = %d", s_CGSEND_Status.nDLCI);
    return s_CGSEND_Status.nDLCI;
}

void AT_GPRS_CGSEND_Stop()
{
    if (s_CGSEND_Status.pGprsData != NULL)
        CSW_TCPIP_FREE(s_CGSEND_Status.pGprsData);

    s_CGSEND_Status.nDLCI = 0;
    s_CGSEND_Status.pGprsData = NULL;
    s_CGSEND_Status.nDataLen = 0;
    AT_KillCgsendTimer();
}

void AT_GPRS_CGSEND_TimerHandler(uint8_t nSim)
{
    uint32_t uFixedSendSize = 2000;
    uint8_t nWaitCount = 0;
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(s_CGSEND_Status.nDLCI);
    OSI_LOGI(0x100042c1, "AT_GPRS_CGSEND_TimerHandler");
    if (s_CGSEND_Status.pGprsData == NULL)
    {
        s_CGSEND_Status.pGprsData = CSW_TCPIP_MALLOC(uFixedSendSize + sizeof(CFW_GPRS_DATA));
        SUL_ZeroMemory8(s_CGSEND_Status.pGprsData, uFixedSendSize);
    }

    if (s_CGSEND_Status.nDataLen > 0)
    {
        while (!CFW_GprsSendDataAvaliable(nSim))
        {
            /* check GPRS send ready for 7.5s. */
            if (nWaitCount > 5)
            {
                OSI_LOGI(0x100042c2, "ERROR, GPRS send data is not avaliable");
                RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
            }
            nWaitCount++;
            COS_Sleep(1500);
        }

        if (s_CGSEND_Status.nDataLen > uFixedSendSize)
            s_CGSEND_Status.pGprsData->nDataLength = uFixedSendSize;
        else
            s_CGSEND_Status.pGprsData->nDataLength = s_CGSEND_Status.nDataLen;

        s_CGSEND_Status.nDataLen -= s_CGSEND_Status.pGprsData->nDataLength;
#ifdef LTE_NBIOT_SUPPORT
        CFW_SetPsDataAttribute(&s_CGSEND_Status.pGprsData->attribute, 0, 0, 0);
#endif
        if (0 != CFW_GprsSendData(s_CGSEND_Status.nCid, s_CGSEND_Status.pGprsData, nSim))

        {
            OSI_LOGI(0x100042c3, "------CFW_GprsSendData error");
            return;
        }

        OSI_LOGI(0x100042c4, "AT_GPRS_CGSEND_TimerHandler left len=%d", s_CGSEND_Status.nDataLen);
        if (s_CGSEND_Status.nDataLen == 0)
            AT_GPRS_CGSEND_Stop();
        else
            AT_SetCgsendTimer(1500);
    }
}

bool AT_KillCgsendTimer(void)
{
    OSI_LOGI(0x100042c5, "AT_KillCgsendTimer");
    COS_StopCallbackTimer(CSW_AT_TASK_HANDLE, AT_GPRS_CGSEND_TimerHandler, 0);
    return true;
}

bool AT_SetCgsendTimer(uint32_t nElapse)
{
    OSI_LOGI(0x100042c6, "AT_SetCgsendTimer");
    COS_StartCallbackTimer(CSW_AT_TASK_HANDLE, nElapse * HAL_TICK1S, AT_GPRS_CGSEND_TimerHandler, 0);
    return true;
}

#endif

typedef struct
{
    uint8_t nCid;
    uint16_t nDataLen;
    CFW_GPRS_DATA *pGprsData;
} cgsendAsyncCtx_t;

extern struct netif *getGprsNetIf(uint8_t nSim, uint8_t nCid);
static void _cgsendSetRegRspTimeCB(atCommand_t *cmd)
{
    OSI_LOGI(0, "_cgsendSetRegRspTimeCB");
    uint32_t uFixedSendSize = 1500;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    cgsendAsyncCtx_t *async = (cgsendAsyncCtx_t *)cmd->async_ctx;
    struct netif *netif_send;
    netif_send = getGprsNetIf(nSim, async->nCid);
    if (netif_send == NULL)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    if (async->nDataLen > 0)
    {
        uint32_t nSendLen = 0x00;

        nSendLen = (async->nDataLen > uFixedSendSize) ? (uFixedSendSize) : (async->nDataLen);
        async->nDataLen -= nSendLen;
        void *pData = malloc(nSendLen);
        memset(pData, 0x1, nSendLen);
        extern bool ATGprsGetDPSDFlag(CFW_SIM_ID nSim);
        if (!ATGprsGetDPSDFlag(nSim))
            drvPsIntfWrite((drvPsIntf_t *)netif_send->pspathIntf, pData, nSendLen);
        free(pData);
        OSI_LOGI(0, "AT_GPRS_CGSEND_TimerHandler left length=%d", async->nDataLen);
        if (async->nDataLen == 0)
        {
            RETURN_OK(cmd->engine);
        }
        else
            atCmdSetTimeoutHandler(cmd->engine, 1000, _cgsendSetRegRspTimeCB);
    }
}

void atCmdHandleCGSEND(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        uint8_t uCid;
        bool paramok = true;
        uint32_t uDataLen;
        uCid = atParamDefUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        uDataLen = atParamUint(cmd->params[1], &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        cgsendAsyncCtx_t *async = (cgsendAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        async->nCid = uCid;
        async->nDataLen = uDataLen;

        atCmdSetTimeoutHandler(cmd->engine, 1000, _cgsendSetRegRspTimeCB);
        RETURN_FOR_ASYNC();
    }
    else
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}

void atCmdHandleCGPADDR(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    char rsp[128] = {0};
    AT_Gprs_CidInfo *g_staAtGprsCidInfo = gAtCfwCtx.sim[nSim].cid_info;

    if (cmd->type == AT_CMD_SET || cmd->type == AT_CMD_EXE)
    {
        uint8_t cid_count;
        uint8_t cids[AT_PDPCID_MAX - AT_PDPCID_MIN + 1];

        bool paramok = true;
        if (cmd->param_count == 0)
        {
            cid_count = AT_PDPCID_MAX - AT_PDPCID_MIN + 1;
            for (int n = AT_PDPCID_MIN; n <= AT_PDPCID_MAX; n++)
                cids[n - AT_PDPCID_MIN] = n;
        }
        else
        {
            if (cmd->param_count > AT_PDPCID_MAX - AT_PDPCID_MIN + 1)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            cid_count = cmd->param_count;
            for (int n = 0; n < cmd->param_count; n++)
                cids[n] = atParamUintInRange(cmd->params[n], AT_PDPCID_MIN,
                                             AT_PDPCID_MAX, &paramok);

            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        CFW_GPRS_PDPCONT_INFO_V2 pdp_cont;
        memset(&pdp_cont, 0x00, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
        for (int n = 0; n < cid_count; n++)
        {
            uint8_t att_state;
            CFW_GetGprsAttState(&att_state, nSim);
            if (att_state == CFW_GPRS_ATTACHED)
            {
                if (CFW_GprsGetPdpCxtV2(cids[n], &pdp_cont, nSim) != 0)
                    continue;
                OSI_LOGI(0, "PDP CTX nPdpType/%d nDComp/%d nHcomp/%d nApnSize/%d nIpType/%d nDnsType/%d",
                         pdp_cont.nPdpType, pdp_cont.nDComp, pdp_cont.nHComp,
                         pdp_cont.nApnSize, pdp_cont.nIpType, pdp_cont.nDnsType);
                OSI_LOGI(0, "PDP CTX nPdpDnsSize/%d nPdpAddrSize/%d nApnUserSize/%d nApnPwdSize/%d nNSLPI/%d PdnType/%d",
                         pdp_cont.nPdpDnsSize, pdp_cont.nPdpAddrSize, pdp_cont.nApnUserSize,
                         pdp_cont.nApnPwdSize, pdp_cont.nNSLPI, pdp_cont.PdnType);
                char addr_str[80] = {0};
                memset(&rsp[0], 0, sizeof(rsp));
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
                quec_pdpAddressToStr(&pdp_cont, addr_str, cids[n], nSim);
#else
                _pdpAddressToStr(&pdp_cont, addr_str);
#endif 
                sprintf(rsp, "+CGPADDR: %d,\"%s\"", cids[n], addr_str);
                atCmdRespInfoText(cmd->engine, rsp);
            }
        }
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char *prsp = &rsp[0];
        prsp += sprintf(prsp, "+CGPADDR: (");

        char *marker = prsp;
        for (uint8_t cid = AT_PDPCID_MIN; cid <= AT_PDPCID_MAX; cid++)
        {
            if (g_staAtGprsCidInfo[cid].uCid)
                prsp += sprintf(prsp, "%d,", g_staAtGprsCidInfo[cid].uCid);
        }

        if (prsp != marker)
            prsp--; // skip the last '.'
        prsp += sprintf(prsp, ")");
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

bool AT_GetOperatorDefaultApn(uint8_t pOperatorId[6], const char **pOperatorDefaltApn)
{
    uint32_t nIndex = 0;
    int32_t nCmpResult = 0;
    uint32_t i = 0;
    const uint8_t *pOperID = 0;
    const char *pApn = 0;
    do
    {
        pApn = OperatorDefaultApnInfo[nIndex].Defaultapn;
        pOperID = OperatorDefaultApnInfo[nIndex].OperatorId;
        if (pApn == 0)
        {
            *pOperatorDefaltApn = 0;
            return false;
        }
        nCmpResult = 0;
        for (i = 0; i < 6; i++)
        {
            if (*pOperID == pOperatorId[i])
                pOperID++;
            else
            {
                nCmpResult = 1;
                break;
            }
        }
        if (!nCmpResult)
        {
            *pOperatorDefaltApn = OperatorDefaultApnInfo[nIndex].Defaultapn;
            return true;
        }
        nIndex++;
    } while (pApn != 0);

    *pOperatorDefaltApn = 0;
    return false;
}

typedef struct
{
    uint8_t cid;
} cgdataContext_t;

static int _pppDataToAt(void *param, uint8_t *data, uint32_t size)
{
    atDispatch_t *ch = (atDispatch_t *)param;
    if (!atDispatchIsValid(ch))
        return 0;
    atDataEngine_t *dengine = atDispatchGetDataEngine(ch);
    if (dengine != NULL)
        atDataWrite(dengine, (const void *)data, (size_t)size);

#ifdef CONFIG_NET_TRACE_IP_PACKET
    if (size > 40)
    {
        uint8_t *ipdata = data;
        if (data[0] == 0x7e && data[1] == 0x21)
        {
            ipdata = (uint8_t *)data + 2;
        }
        else if (data[0] == 0x21)
        {
            ipdata = (uint8_t *)data + 1;
        }
        uint16_t identify = (ipdata[4] << 8) + ipdata[5];
        OSI_LOGI(0x10005666, "PPP DL AT write done identify %04x", identify);
    }
#endif
    return 0;
}

//_pppAtFlowControl return false is no space
static int _pppAtFlowControl(void *param)
{
    atDispatch_t *ch = (atDispatch_t *)param;
    if (ch == NULL)
    {
        OSI_LOGE(0, "failed to get PPP dispatch");
        return 2;
    }

    if (atDispatchIsValid(ch) == false)
    {
        OSI_LOGE(0, "failed to get PPP dispatch atDispatchIsValid");
        return 2;
    }

    atDevice_t *device = atDispatchGetDevice(ch);
    if (device == NULL)
    {
        OSI_LOGE(0, "failed to get device from PPP dispatch");
        return 2;
    }

    // When there are no pending events, AND there are no command running,
    // it is not needed to flow control. Even it is blocked at writing,
    // it doesn't matter.
    if (!osiEventPending(atEngineGetThreadId()) &&
        !atEngineIsCmdRunning())
        return 0;

    int writeAvail = atDeviceWriteAvail(device);
    if (writeAvail <= 2048)
        return 2;

    int readAvail = atDeviceReadAvail(device);
    if (readAvail > 0)
        return 1;

    return 0;
}

static void _pppDelete(void *ctx)
{
    OSI_LOGI(0, "_pppDelete enter");
    atDispatch_t *ch = (atDispatch_t *)ctx;
    atDataEngine_t *dengine = atDispatchGetDataEngine(ch);
    if (dengine == NULL)
        return;
    pppSession_t *ppp = atDataEngineGetPppSession(dengine);
    if (ppp != NULL)
    {
        if (!pppSessionDelete(ppp))
            return;
    }
    else
    {
        return;
    }
    if (ppp != NULL)
    {
        atDataClearPPPSession(dengine);
    }
    
#ifndef CONFIG_QUEC_PROJECT_FEATURE_PPP
    //PPP_DATA_END should be wait for GPRS deactive success
    atEngineModeSwitch(AT_MODE_SWITCH_DATA_PPP_END, ch);
    atDispatchSetModeSwitchHandler(ch, NULL);
#endif    
    OSI_LOGI(0, "_pppDelete return");
}

static void _pppEnd(void *param, uint8_t err_code)
{
    OSI_LOGI(0, "pppEnd enter");
    if (err_code == GPRS_UNACTIVE)
    {
        atDispatch_t *ch = (atDispatch_t *)param;
        if (ch == NULL)
        {
            return;
        }
        OSI_LOGI(0, "_pppDelete enter0");
        _pppDelete(ch);
        return;
    }
    osiThreadCallback(atEngineGetThreadId(), _pppDelete, param);
}

void atEnterPPPState(atCommand_t *cmd, uint8_t cid)
{
    //cgdataContext_t *async = (cgdataContext_t *)cmd->async_ctx;
    atDispatch_t *ch = atCmdGetDispatch(cmd->engine);
    uint8_t simId = atCmdGetSim(cmd->engine);
    uint8_t act_state = 0;
    int iNeddAct = 0;

    CFW_GetGprsActState(cid, &act_state, simId);
    if (act_state != CFW_GPRS_ACTIVED)
    {
        iNeddAct = 1;
    }
    else
    {
        OSI_LOGI(0, "ppp stop gprs Already Active, cid=%d \n", cid);
    }
    pppSession_t *ppp = pppSessionCreate(cid, simId, atEngineGetThreadId(),
                                         _pppDataToAt, ch,
                                         _pppEnd, ch,
                                         _pppAtFlowControl, ch,
                                         iNeddAct);

    if (ppp == NULL)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

    //pppSetCmdEngine(ppp, (void *)cmd);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP  
    atDispatchSetModeSwitchHandler(ch, quec_ppp_mode_switch_handler);
#else    
    atDispatchSetModeSwitchHandler(ch, pppAtModeSwitchHandler);
#endif
    atEngineModeSwitch(AT_MODE_SWITCH_DATA_START, ch);
    atDataEngine_t *dengine = atDispatchGetDataEngine(ch);
    atDataSetPPPMode(dengine, ppp);
}

void atCmdHandleCGDATA(atCommand_t *cmd)
{
    // #ifdef PORTING_ON_GOING
    uint8_t uCidState = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (NULL == cmd)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count > 2)
        {
            // uplimit of maxium PDP allowed to be activated
            OSI_LOGI(0x100042ca, "AT_GPRS_CmdFunc_CGDATA, paramCount=%d \n", cmd->param_count);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool paramok = true;
        const char *mode = atParamStr(cmd->params[0], &paramok);
        const uint8_t cid = atParamUintInRange(cmd->params[1], 0, AT_PDPCID_MAX, &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (strcmp(mode, "PPP") != 0)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        }

        //One at angine support one ppp session
        atDispatch_t *dispatch = atCmdGetDispatch(cmd->engine);
        if (dispatch != NULL)
        {
            atDataEngine_t *dengine = atDispatchGetDataEngine(dispatch);
            if (dengine != NULL)
            {
                if (atDataIsPPPMode(dengine))
                {
                    OSI_LOGI(0, "at engine ppp is opened");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
        }

        CFW_GetGprsActState(cid, &uCidState, nSim);
        if (uCidState != CFW_GPRS_ACTIVED)
        {
            OSI_LOGI(0x100042cb, "request <cid> not actived cid =%d", cid);
        }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP  
        uint32_t ret = quec_check_gprs_pdp_state(nSim, cid);
        if (CMD_RC_OK != ret)
        {
            RETURN_CME_ERR(cmd->engine, ret);
        }

        quec_netif *wan_if = NULL;
        quec_netif *ppp_netif = NULL;
        uint8_t todoAction = 0xFF;
#if IP_NAT
		if (get_nat_enabled(nSim, cid))
		{
			todoAction = quec_check_netif_exist_or_available(nSim, cid, &wan_if, &ppp_netif, NETIF_LINK_MODE_NAT_PPP_LAN);
		}
		else
#endif
		{
			todoAction = quec_check_netif_exist_or_available(nSim, cid, &wan_if, &ppp_netif, NETIF_LINK_MODE_PPP);
		}

        todoAction = quec_check_netif_exist_or_available(nSim, cid, &wan_if, &ppp_netif, NETIF_LINK_MODE_PPP);
        OSI_LOGI(0, "QUEC_PPP CGDATA %d %d todo: %d %p %p", nSim, cid, todoAction, ppp_netif, wan_if);
        if ((QUEC_NETIF_ACTION_BUSY == todoAction) || (QUEC_NETIF_ACTION_NONE == todoAction) || (QUEC_NETIF_ACTION_MATCH_MODE == todoAction))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        }
#endif        
        atEnterPPPState(cmd, cid);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CGDATA: (\"PPP\")");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
    // #endif
}

static void _cgautoAttRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    // EV_CFW_GPRS_ATT_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (cfw_event->nType != CFW_GPRS_ATTACHED)
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
    atCmdRespOK(cmd->engine);
}

// ------------------------------------------------------------------------------------------
// Set command         |   Read command          |   Test command                          |
// AT+CGAUTO=[<n>]   |     AT+ CGAUTO?         |     AT+ CGAUTO =?                       |
// Response(s)         |   Response(s)           |   Response(s)                           |
// Success:        |     Success:            |     Success:                            |
// OK          |       +CGAUTO: <n>      |       +CGAUTO: (list of supported <n>s) |
// Fail:           |       OK                |       OK                                |
// ERROR       |     Fail:               |     Fail:                               |
// |       ERROR             |       ERROR                             |
// ------------------------------------------------------------------------------------------
void atCmdHandleCGAUTO(atCommand_t *cmd)
{
    uint32_t iResult;
    bool paramok = true;
    uint8_t attState;
    char pOutPutBuff[20] = {0};
    uint8_t nSim = atCmdGetSim(cmd->engine);
    // input parameter check.

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        uint8_t uStatus = atParamUintInRange(cmd->params[0], 0, 3, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        // Set the status of the auto response..
        gATCurrentu8nURCAuto = uStatus;
        if (uStatus == AUTO_RSP_STU_ON_PDONLY)
        {
            iResult = CFW_GetGprsAttState(&attState, nSim);
            if (0 != iResult)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            if (attState == CFW_GPRS_DETACHED)
            {
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_cgautoAttRsp, cmd);
                if (0 != CFW_GprsAtt(CFW_GPRS_ATTACHED, cmd->uti, nSim))
                {
                    cfwReleaseUTI(cmd->uti);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
                RETURN_FOR_ASYNC();
            }
        }
        atCmdRespOK(cmd->engine);
        break;
    }
    case AT_CMD_READ:
        sprintf(pOutPutBuff, "+CGAUTO: %d", gATCurrentu8nURCAuto);
        atCmdRespInfoText(cmd->engine, pOutPutBuff);
        atCmdRespOK(cmd->engine);
        break;

    case AT_CMD_TEST:
        atCmdRespInfoText(cmd->engine, "+CGAUTO: (0-3)");
        atCmdRespOK(cmd->engine);
        break;

    default:
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
    return;
}

void atCmdHandleCGANS(atCommand_t *cmd)
{
    uint8_t iResult = 0;
    bool paramok = true;
    uint8_t uResponse = 0;
    uint8_t uCidState = 0;
    uint8_t uCid = 0;
    uint8_t uUTI = 0;
    CFW_GPRS_PDPCONT_INFO PdpCont_Info = {
        0,
    };
    uint8_t pPdpAddr[18] = {0};
    PdpCont_Info.pPdpAddr = pPdpAddr;
    static const osiValueStrMap_t PdpTypeVSMap[] = {
        {CFW_GPRS_PDP_TYPE_IP, "IP"},
        {CFW_GPRS_PDP_TYPE_IPV6, "IPV6"},
        {CFW_GPRS_PDP_TYPE_PPP, "PPP"},
        {CFW_GPRS_PDP_TYPE_PPP, "IPV4V6"},
    };

    uint8_t nSim = atCmdGetSim(cmd->engine);
    AT_Gprs_CidInfo *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];

    if (NULL == cmd)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (AT_CMD_SET == cmd->type || AT_CMD_EXE == cmd->type)
    {
        if (cmd->param_count > 3)
        {
            OSI_LOGI(0x100042cd, "exe       in AT_GPRS_CmdFunc_CGANS, parameters error or no parameters offered \n");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        // <response>
        uResponse = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        // <L2P>  ??? not supported, do nothing.
        if (cmd->param_count > 1)
        {
            const char *pdp = atParamStr(cmd->params[1], &paramok);
            if (!strcmp(pdp, "IP") || !strcmp(pdp, "IPV6") || !strcmp(pdp, "IPV4V6") || !strcmp(pdp, "PPP"))
            {
                PdpCont_Info.nPdpType = atParamUintByStrMap(cmd->params[1], PdpTypeVSMap, &paramok);
                if (!paramok)
                {
                    OSI_LOGI(0x100042ce, "exe       in AT_GPRS_CmdFunc_CGANS, PDP type error\n");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                OSI_LOGI(0, "ln 000 pdp-----error !!! ");
            }
        }

        // <cid>
        if (cmd->param_count > 2)
        {
            uCid = atParamUintInRange(cmd->params[2], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
#ifndef AT_NO_GPRS
            iResult = CFW_GetGprsActState(uCid, &uCidState, nSim);
#endif
            if (iResult != 0)
            {
                OSI_LOGI(0x100042cf, "exe       in AT_GPRS_CmdFunc_CGANS, get PDP state error.");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            if (CFW_GPRS_ACTIVED == uCidState)
            {
                OSI_LOGI(0x100042d0, "exe       in AT_GPRS_CmdFunc_CGANS, the speicified PDP is alread activated");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            /*
            if (PdpCont_Info.nPdpType != g_staAtGprsCidInfo[uCid].nPdpType) // yy [mod] 2008-6-27 for bug ID 8887
            {
                OSI_LOGI(0x100042d1, "exe       in AT_GPRS_CmdFunc_CGANS, PDP type not match: %d, %d\n", PdpCont_Info.nPdpType, g_staAtGprsCidInfo[uCid].nPdpType); // yy [mod] 2008-6-27 for bug ID 8887
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
	    */
            if (g_staAtGprsCidInfo[uCid].nPdpAddrSize != 0)
            {
                if (memcmp(PdpCont_Info.pPdpAddr, g_staAtGprsCidInfo[uCid].pPdpAddr, g_staAtGprsCidInfo[uCid].nPdpAddrSize))
                {
                    OSI_LOGI(0x100042d2, "exe       in AT_GPRS_CmdFunc_CGANS, PDP address not match\n");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }
            iResult = CFW_GetFreeUTI(CFW_GPRS_SRV_ID, &uUTI);

            if (iResult != 0)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            if (0 == uResponse)
            {
#ifndef AT_NO_GPRS
                iResult = CFW_GprsManualRej(uUTI, uCid, nSim);
#endif
                if (iResult != 0)
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            else
            {
#ifndef AT_NO_GPRS
                iResult = CFW_GprsManualAcc(uUTI, uCid, nSim);
#endif
                if (iResult != 0)
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            //AT_SetAsyncTimerMux(cmd->engine, 0);
        }
        // atCmdRespInfoText(cmd->engine, "CONNECT . . . (data transfer)");
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CGANS: (0,1),(\"PPP,IP,IPV6,IPV4V6\")");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void atCmdHandleCGCLASS(atCommand_t *cmd)
{
    bool paramok = true;
    const char *arrCharacterSet = NULL;
    char AtRet[50] = {
        0,
    };
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        arrCharacterSet = atParamStr(cmd->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if ((strcmp(arrCharacterSet, "CC")) == 0 || (strcmp(arrCharacterSet, "A") == 0))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        else if (strcmp(arrCharacterSet, "CG") == 0)
        {
            gAtCfwCtx.g_uClassType = CLASS_TYPE_CG;
            gAtSetting.CgClassType[nSim] = 1;
            atCfgAutoSave();
            CFW_CfgNwSetCgclassType(1, nSim);
            RETURN_OK(cmd->engine);
        }
        else if (strcmp(arrCharacterSet, "B") == 0)
        {
            if (CFW_CfgGetPocEnable())
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            gAtCfwCtx.g_uClassType = CLASS_TYPE_B;
            gAtSetting.CgClassType[nSim] = 0;
            atCfgAutoSave();
            CFW_CfgNwSetCgclassType(0, nSim);
            RETURN_OK(cmd->engine);
        }
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t nCgclsstyp = 0;
        if (0 != CFW_CfgNwGetCgclassType(&nCgclsstyp, nSim))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        if (CLASS_TYPE_B == nCgclsstyp)
            sprintf(AtRet, "+CGCLASS: \"B\"");
        else if (CLASS_TYPE_CG == nCgclsstyp)
            sprintf(AtRet, "+CGCLASS: \"CG\"");
#else
        if (CLASS_TYPE_B == nCgclsstyp)
            sprintf(AtRet, "+CGCLASS: B");
        else if (CLASS_TYPE_CG == nCgclsstyp)
            sprintf(AtRet, "+CGCLASS: CG");
#endif        
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        sprintf(AtRet, "+CGCLASS: (\"B\",\"CG\")");
#else
        sprintf(AtRet, "+CGCLASS: B,CG,A(NO SUPPORT),CC(NO SUPPORT)");
#endif
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    return;
}

// This function to process the AT+CGREG command. include set,read and test command.
//
// Set command
// +CGREG= [<n>]
// Possible response(s)
// OK
// ERROR
//
// Read command
// +CGREG?
// Possible response(s)
// +CGREG: <n>,<stat>[,<lac>,<ci>]
// +CME ERROR: <err>
// Test command
// +CGREG=?
// Possible response(s)
// +CGREG: (list of supported <n>s)
//

void atCmdHandleCGREG(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret = 0;

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
#ifdef CONFIG_SOC_8910
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
		uint8_t n = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
#else
        uint8_t n = atParamUintInRange(cmd->params[0], 0, 3, &paramok);
#endif
#else
        uint8_t n = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
#endif
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gAtSetting.sim[nSim].cgreg = n;
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type) // Read command
    {
        CFW_NW_STATUS_INFO sStatus;
        uint8_t att_state = 0;
        CFW_GetGprsAttState(&att_state, nSim);
        ret = CFW_GprsGetstatus(&sStatus, nSim);

        if (ret != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        char rsp[64];
        uint8_t cgreg_status = 0;
        uint8_t nCurrRat = CFW_NWGetStackRat(nSim);

        cgreg_status = quec_regstatus_rda2quec(sStatus.nStatus);
        if(gAtSetting.sim[nSim].cgreg == 2)
        {
            if(1 == cgreg_status || 5 == cgreg_status)
            {
                if(nCurrRat == CFW_RAT_LTE_ONLY)
                {
                    sprintf(rsp, "+CGREG: %d,%d,\"%02X%02X\",\"%X%02X%02X%02X\",%d",
                        gAtSetting.sim[nSim].cgreg, cgreg_status, 
                        sStatus.nAreaCode[3],sStatus.nAreaCode[4], 
                        sStatus.nCellId[0],sStatus.nCellId[1],sStatus.nCellId[2],sStatus.nCellId[3], 
                        AT_Gprs_Mapping_Act_From_PsType(sStatus.PsType));
                }
                else
                {
                    sprintf(rsp, "+CGREG: %d,%d,\"%02X%02X\",\"%02X%02X\",%d",
                        gAtSetting.sim[nSim].cgreg, cgreg_status, 
                        sStatus.nAreaCode[3],sStatus.nAreaCode[4], 
                        sStatus.nCellId[0],sStatus.nCellId[1], 
                        AT_Gprs_Mapping_Act_From_PsType(sStatus.PsType));
                }
            }
            else
            {
                sprintf(rsp, "+CGREG: %d,%d", gAtSetting.sim[nSim].cgreg, cgreg_status);
            }
        }
        else
        {
            sprintf(rsp, "+CGREG: %d,%d", gAtSetting.sim[nSim].cgreg, cgreg_status);
        }
#else
        char rsp[64];
        if (gAtSetting.sim[nSim].cgreg == 2)
        {
            sprintf(rsp, "+CGREG: %d,%d,\"%02X%02X\",\"%02X%02X\"",
                    gAtSetting.sim[nSim].cgreg, sStatus.nStatus,
                    sStatus.nAreaCode[3], sStatus.nAreaCode[4],
                    sStatus.nCellId[0], sStatus.nCellId[1]);
        }
#ifdef CONFIG_SOC_8910
        else if (gAtSetting.sim[nSim].cgreg == 3)
        {
            uint8_t nCurrRat = CFW_NWGetStackRat(nSim);
            sprintf(rsp, "+CGREG: %d,%d,\"%02X%02X\",\"%02X%02X\",%d,%d,%d",
                    gAtSetting.sim[nSim].cgreg, sStatus.nStatus,
                    sStatus.nAreaCode[3], sStatus.nAreaCode[4],
                    sStatus.nCellId[0], sStatus.nCellId[1], Mapping_Creg_From_PsType(nCurrRat),
                    sStatus.cause_type, sStatus.reject_cause);
        }
#endif
        else
        {
            sprintf(rsp, "+CGREG: %d,%d", gAtSetting.sim[nSim].cgreg, sStatus.nStatus);
        }
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
#ifdef CONFIG_SOC_8910
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
		atCmdRespInfoText(cmd->engine, "+CGREG: (0-2)");
#else
        atCmdRespInfoText(cmd->engine, "+CGREG: (0-3)");
#endif
#else
        atCmdRespInfoText(cmd->engine, "+CGREG: (0-2)");
#endif
        atCmdRespOK(cmd->engine);
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
    else if (AT_CMD_EXE == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
#endif
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

// 10.1.21 Select service for MO SMS messages
void atCmdHandleCGSMS(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        // +CGSMS=[<service>]
        bool paramok = true;
        uint8_t uService = atParamDefUintInRange(cmd->params[0], 0, 0, 3, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        CFW_GprsSetSmsSeviceMode(uService);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type) // Read command
    {
        uint8_t uService = CFW_GprsGetSmsSeviceMode();
        char rsp[32];
        sprintf(rsp, "+CGSMS: %d", uService);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
		atCmdRespInfoText(cmd->engine, "+CGSMS: (0-3)");
#else
        atCmdRespInfoText(cmd->engine, "+CGSMS: (0,1,2,3)");
#endif
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

void atCmdHandleCGCID(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    uint8_t nRet = 0;
    uint8_t nCID = 0;
    uint8_t pResp[20] = {
        0x00,
    };
    uint8_t uHelpString[30] = {
        0X00,
    };
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (NULL == pParam)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    switch (cmd->type)
    {
    case AT_CMD_EXE:
        if (pParam->pPara == NULL)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        else
        {
#ifndef AT_NO_GPRS
            nRet = CFW_GetFreeCID(&nCID, nSim);
#endif
            OSI_LOGI(0x100042da, "AT+CGCID:	Free CID = %d\n\r", nCID);
            if (0 == nRet)
            {
                sprintf(pResp, "+CGCID:%d", nCID);
                atCmdRespInfoText(cmd->engine, pResp);
                RETURN_OK(cmd->engine);
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        break;

    case AT_CMD_TEST:
        AT_StrCpy(uHelpString, "+CGCID: (1-7)");
        atCmdRespInfoText(cmd->engine, uHelpString);
        RETURN_OK(cmd->engine);

    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    return;
#endif
}

int32_t AT_GPRS_IPStringToPDPAddr(uint8_t *pPdpAddrStr, uint8_t uSize, uint8_t *pPdpAddr, uint8_t *pAddrSize)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t m = 0;
    uint8_t uBuf[4] = {0};
    uint8_t uIpNum = 0;
    uint8_t addrsize = 0;

    OSI_LOGXI(OSI_LOGPAR_SI, 0x100042db, "pPdpAddrStr: %s, uSize: %d", pPdpAddrStr, uSize);

    if ((NULL == pPdpAddrStr) || (0 == uSize))
    {
        return -1;
    }

    for (i = 0; i < 4; i++)
    {
        m = 0;

        while ((pPdpAddrStr[j] != '.') && (j < uSize))
        {
            uBuf[m++] = pPdpAddrStr[j++];
        }
        uIpNum = atoi((char *)uBuf);
        OSI_LOGXI(OSI_LOGPAR_SII, 0x100042dc, "uBuf: %s, uIpNum: %d, j: %d", uBuf, uIpNum, j);

        pPdpAddr[addrsize++] = uIpNum;
        memset(uBuf, 0, 4);

        j++;
    }

    *pAddrSize = addrsize;
    return 1;
}

#ifdef LTE_NBIOT_SUPPORT
int32_t AT_GPRS_IPv4v6AddrAnalyzer(uint8_t *pPdpAddr, uint8_t *iptype)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t m = 0;
    uint8_t uBuf[4] = {0};
    uint16_t uIpNum = 0;
    uint8_t uSize = 0;
    OSI_LOGXI(OSI_LOGPAR_SI, 0x100042dd, "pPdpAddr: %s, uSize: %d", pPdpAddr, uSize);

    if (NULL == pPdpAddr)
    {
        return -1;
    }
    else
    {
        if ((uSize = strlen((char *)pPdpAddr)) == 0)
            return -1;
    }

    for (i = 0; i < uSize; i++)
    {
        if (pPdpAddr[i] == '.')
        {
            *iptype = 0; //ip4
            break;
        }
        else if (pPdpAddr[i] == ':')
        {
            *iptype = 1; //ip6
            break;
        }
    }

    if (*iptype == 0)
    {
        for (i = 0; i < 4; i++)
        {
            m = 0;

            while ((pPdpAddr[j] != '.') && (j < uSize))
            {
                uBuf[m++] = pPdpAddr[j++];
            }

            uIpNum = atoi((char *)uBuf);

            OSI_LOGXI(OSI_LOGPAR_SII, 0x100042dc, "uBuf: %s, uIpNum: %d, j: %d", uBuf, uIpNum, j);

            if ((uBuf[0] != '0') && (uIpNum < 1 || uIpNum > 254))
            {
                return -1;
            }

            memset(uBuf, 0, 4);

            j++;
        }
    }
    else if (*iptype == 1)
    {
        for (i = 0; i < uSize; i++)
        {
            if (pPdpAddr[i] == ':')
                continue;
            else
            {
                if (!isxdigit(pPdpAddr[i]))
                {
                    return -1;
                }
            }
        }
        OSI_LOGXI(OSI_LOGPAR_S, 0x100042de, "ip6 addr: %s", pPdpAddr);
    }
    else
    {
        return 2;
    }

    return 1;
}
#endif

int32_t AT_GPRS_IPAddrAnalyzer(const uint8_t *pPdpAddr, uint8_t uSize)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t m = 0;
    uint8_t uBuf[4] = {0};
    uint8_t uIpNum = 0;

    OSI_LOGXI(OSI_LOGPAR_SI, 0x100042dd, "pPdpAddr: %s, uSize: %d", pPdpAddr, uSize);

    if ((NULL == pPdpAddr) || (0 == uSize))
    {
        return -1;
    }

    for (i = 0; i < 4; i++)
    {
        m = 0;

        while ((pPdpAddr[j] != '.') && (j < uSize))
        {
            uBuf[m++] = pPdpAddr[j++];
        }

        uIpNum = atoi((char *)uBuf);

        OSI_LOGXI(OSI_LOGPAR_SII, 0x100042dc, "uBuf: %s, uIpNum: %d, j: %d", uBuf, uIpNum, j);

        if ((uBuf[0] != '0') && (uIpNum < 1 || uIpNum > 254))
        {
            return -1;
        }

        memset(uBuf, 0, 4);

        j++;
    }

    return 1;
}

bool AT_GPRS_IPV4Chech(uint8_t *paddr, uint8_t nSize)
{
    uint8_t i = 0, j = 0, m = 0;
    uint8_t uBuf[4] = {0};
    uint8_t pointNum = 0;
    uint32_t uIpNum = 0;
    //check the IPV4 addr

    for (i = 0; i < nSize; i++)
    {
        if (paddr[i] == '.')
            pointNum++;
    }
    if (3 != pointNum)
    {
        return false;
    }
    for (i = 0; i < 4; i++)
    {
        m = 0;

        while ((paddr[j] != '.') && (j < nSize))
        {
            if (m > 3 || paddr[j] < '0' || paddr[j] > '9')
            {
                return false;
            }
            uBuf[m++] = paddr[j++];
        }
        uIpNum = atoi((const char *)uBuf);
        if (uIpNum > 255)
        {
            return false;
        }
        memset(uBuf, 0, 4);

        j++;
    }
    return true;
}
bool AT_GPRS_IPV6Chech(uint8_t *paddr, uint8_t nSize)
{
    uint8_t i = 0, j = 0, m = 0;
    uint8_t uBuf[5] = {0};
    uint8_t pointNum = 0;
    //check the IPV6 addr

    for (i = 0; i < nSize; i++)
    {
        if (paddr[i] == ':')
            pointNum++;
    }
    if (7 != pointNum)
    {
        return false;
    }
    for (i = 0; i < 8; i++)
    {
        m = 0;

        while ((paddr[j] != ':') && (j < nSize))
        {
            if (m > 4 || paddr[j] < '0' || paddr[j] > 'z' || (paddr[j] > '9' && paddr[j] < 'A') || (paddr[j] > 'Z' && paddr[j] < 'a'))
            {
                return false;
            }
            uBuf[m++] = paddr[j++];
        }
        memset(uBuf, 0, 5);

        j++;
    }
    return true;
}

bool AT_GPRS_AddrCheck(const char *paddr, uint8_t nSize)
{
    uint8_t uIpv4[16] = {0};
    uint8_t uIpv6[41] = {0};
    bool result = true;
    size_t uV4Size = 0;
    size_t uV6Size = 0;
    OSI_LOGXI(OSI_LOGPAR_SI, 0, "pPdpAddr: %s, uSize: %d", paddr, nSize);
    for (int i = 0; i < uV4Size + 1; i++)
    {
        if (uV4Size < nSize)
        {
            if (paddr[i] != ',')
            {
                uV4Size++;
                uIpv4[i] = paddr[i];
            }
        }
    }
    OSI_LOGI(0x00000000, "AT_GPRS_AddrCheckuIpv6uV4Size:%d", uV4Size);
    result = AT_GPRS_IPV4Chech(uIpv4, uV4Size);
    if (true != result)
        return false;

    if (uV4Size < nSize)
    {
        int m = 0;
        for (int i = uV4Size + 1; i < nSize; i++)
        {
            if (paddr[i] != '[' && paddr[i] != ']')
            {
                uV6Size++;
                uIpv6[m] = paddr[i];
                m++;
            }
        }
        OSI_LOGI(0x00000000, "AT_GPRS_AddrCheckuIpv6uV6Size:%d", uV6Size);
        result = AT_GPRS_IPV6Chech(uIpv6, uV6Size);
        if (true != result)
            return false;
    }
    return true;
}

void atCmdHandleXCAPIP(atCommand_t *cmd)
{
    uint8_t Cid;
    uint8_t uCidState;
    bool ipCheck = false;
    uint8_t uSize;
    uint8_t uAddrArr[128] = {
        0,
    };
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count != 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        Cid = atParamUintInRange(cmd->params[0], 5, 5, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        CFW_GetGprsActState(Cid, &uCidState, nSim);
        if (CFW_GPRS_ACTIVED != uCidState)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        const char *uAddr = atParamStr(cmd->params[1], &paramok);
        uSize = strlen(uAddr);
        if (!paramok || (uSize > 128))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        ipCheck = AT_GPRS_AddrCheck(uAddr, uSize);
        if (!ipCheck)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        for (int i = 0; i < uSize; i++)
            uAddrArr[i] = uAddr[i];
        uint32_t iResult = 0;
        iResult = CFW_ImsSsUtAddrSet(Cid, uSize, uAddrArr, nSim);
        OSI_LOGXI(OSI_LOGPAR_I, 0x00000000, "iResult: %d", iResult);
        if (iResult != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t att_state;
        CFW_GPRS_PDPCONT_INFO_V2 pdp_cont;
        char rsp[129] = {0};
        char *prsp = rsp;
        CFW_GetGprsAttState(&att_state, nSim);
        if (att_state == CFW_GPRS_ATTACHED)
        {
            if (CFW_GprsGetPdpCxtV2(5, &pdp_cont, nSim) == 0)
            {
                prsp += sprintf(prsp, "+XCAPIP:5");
                char addr_str[80] = {};
                _pdpAddressToStr(&pdp_cont, addr_str);
                prsp += sprintf(prsp, ",\"%s\"", addr_str);
                atCmdRespInfoText(cmd->engine, rsp);
            }
        }
        RETURN_OK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

void AT_CLearGprsCtx(uint8_t nDLCI)
{
    gAtWaitActiveRsp[nDLCI] = 0;
    if (gAtGprsCidNum)
        gAtGprsCidNum--;
}

#ifdef LTE_NBIOT_SUPPORT
void atCmdHandlePSDATAOFF(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    static uint8_t cid = 0;
    if (AT_CMD_SET == cmd->type)
    {
        uint8_t nSimId = atCmdGetSim(cmd->engine);
        uint8_t enablepsDataOff = 0;
        uint8_t exempted_service_type = 0;
        bool ueReqBearerResrcMod = false;
        bool nvChanged = false;
        bool paramok = true;
        if ((cmd->param_count > 3) || (cmd->param_count == 0))
        {
            OSI_LOGXE(OSI_LOGPAR_S, 0x100000e6, " AT_GPRS_CmdFunc_IPFLT: Parameter = %s\n", pParam->pPara);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        cid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        enablepsDataOff = atParamDefUintInRange(cmd->params[1], nbiot_nvGetPsDataOff(), 0, 1, &paramok);
        exempted_service_type = atParamDefUintInRange(cmd->params[2], nbiot_nvGetExemptedServiceType(), 0, 3, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        OSI_LOGI(0x100000e7, "AT+PSDATAOFF: enablepsDataOff:%d, exempted_service_type:%d", enablepsDataOff, exempted_service_type);
        if (enablepsDataOff != nbiot_nvGetPsDataOff())
        {
            nvChanged = true;
            nbiot_nvSetPsDataOff(enablepsDataOff);
            if (CFW_GetNwSupportPsDataOff(cid) == true)
            {
                ueReqBearerResrcMod = true;
                CFW_GprsCtxEpsModify(pParam->nDLCI, cid, 1, nSimId);
            }
        }
        if (exempted_service_type != nbiot_nvGetExemptedServiceType())
        {
            nvChanged = true;
            nbiot_nvSetExemptedServiceType(exempted_service_type);
        }
        if (true == nvChanged)
            nvmWriteStatic();
        if (true == ueReqBearerResrcMod)
            AT_CMD_RETURN(AT_SetAsyncTimerMux(cmd->engine, 5 * 188 + 5));
        else
            RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t read_psdataOff[20];
        AT_MemSet(read_psdataOff, 0, SIZEOF(read_psdataOff));
        sprintf(read_psdataOff, "+PSDATAOFF: %d,%d,%d", cid, nbiot_nvGetPsDataOff(), nbiot_nvGetExemptedServiceType());
        atCmdRespInfoText(cmd->engine, read_psdataOff);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        uint8_t *test_psdataOff = "+PSDATAOFF: cid(1~7), enablePsDataOff=(0,1), exempted_service_type=(0~3)";
        atCmdRespInfoText(cmd->engine, test_psdataOff);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGE(0x100000e8, "AT_GPRS_CmdFunc_PSDATAOFF: This type does not supported!");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
#endif
}
#ifdef CONFIG_SOC_6760

void atCmdHandleCarrierNum(atCommand_t *cmd)
{
    uint16_t carrier_num = 0;
    if (!cmd)
    {
        OSI_LOGI(0x100052b7, "AT+CARRIERNUM: pParam = NULL");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        carrier_num = atParamUintInRange(cmd->params[0], 1, 480, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        Cfw_SetCarrierNum(carrier_num);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        const char *pTestRsp = "+CARRIERNUM: [1~480]";
        //atCmdRespOKText(cmd->engine, pTestRsp);
        atCmdRespInfoText(cmd->engine, pTestRsp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x100042e1, "AT_GPRS_CmdFunc_IPFLT: This type does not supported!");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
#endif

#ifdef CONFIG_CFW_DEBUG_IPFILTER
// Filter IP packets inside CFW. DEBUG ONLY!!
void atCmdHandleIPFLT(atCommand_t *cmd)
{
    extern uint8_t gCfwDebugFilterSwitch;
    extern bool gCfwDebugFilterClass;

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t filter_switch = atParamUintInRange(cmd->params[0], 0, 15, &paramok);
        bool filter_class = atParamDefUintInRange(cmd->params[0], 1, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gCfwDebugFilterSwitch = filter_switch;
        gCfwDebugFilterClass = filter_class;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        const char *pTestRsp = "+IPFLT: [0~15], [0~1]";
        atCmdRespInfoText(cmd->engine, pTestRsp);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char pTestRsp[32];
        sprintf(pTestRsp, "+IPFLT: %d, %d", gCfwDebugFilterSwitch, gCfwDebugFilterClass);
        atCmdRespOKText(cmd->engine, pTestRsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
#endif

extern uint16_t CFW_GetNonIpMtu(uint8_t cid);
void atCmdHandleNIPDATA(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    bool paramok = true;
    uint32_t errCode = CMD_FUNC_SUCC;
    uint8_t uCidState = 0;
    uint8_t *non_ipdata = 0;
    uint8_t cid = 0;
    uint8_t pdnType = 0;
    CFW_GPRS_DATA *pGprsData = NULL;
    CFW_SIM_ID nSim = (CFW_SIM_ID)atCmdGetSim(cmd->engine);
    AT_Gprs_CidInfo *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    OSI_LOGI(0x100042e2, "+NIPDATA: nSim= %d; nDLCI= 0x%x", nSim, pParam->nDLCI);

    if (NULL == pParam)
    {
        OSI_LOGI(0x100042e3, "exe       in AT_TCPIP_CmdFunc_NIPDATA, parameters is NULL\n");
        errCode = ERR_AT_CME_PARAM_INVALID;
        goto nonip_result;
    }

    if (AT_CMD_SET == cmd->type)
    {
        // check paracount
        if (cmd->param_count != 2)
        {
            errCode = ERR_AT_CME_PARAM_INVALID;
            OSI_LOGI(0x100042e4, "paras count is invalid");
            goto nonip_result;
        }

        cid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        if (paramok == false)
        {
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto nonip_result;
        }
        if (0 == g_staAtGprsCidInfo[cid].uCid)
        {
            OSI_LOGI(0x100042e5, "request <cid> not defined ");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto nonip_result;
        }
#ifndef AT_NO_GPRS
        CFW_GetGprsActState(cid, &uCidState, nSim);
#endif
        if (uCidState != CFW_GPRS_ACTIVED)
        {
            OSI_LOGI(0x100042e6, "pdp have not be activated!!");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto nonip_result;
        }
        if (PdpContList[nSim][cid - 1] != NULL)
        {
            //pdnType = PdpContList[nSim][cid - 1]->PdnType;
            pdnType = g_staAtGprsCidInfo[cid].nPdpType;
            if (pdnType != CFW_GPRS_PDP_TYPE_NONIP)
            {
                OSI_LOGI(0x100042e7, "invalid pdn type ");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto nonip_result;
            }
        }
        if (pGprsData == NULL)
        {
            if ((pGprsData = AT_MALLOC(AT_GPRS_NONIP_DATA_MAX_LEN + sizeof(CFW_GPRS_DATA))) == NULL)
            {
                errCode = ERR_AT_CME_NO_MEMORY;
                goto nonip_result;
            }
        }
        non_ipdata = atParamStr(cmd->params[1], &paramok);
        pGprsData->nDataLength = strlen(non_ipdata);
        if ((paramok == false) || (pGprsData->nDataLength > CFW_GetNonIpMtu(cid)))
        {
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto nonip_result;
        }
        AT_StrCpy(pGprsData->pData, non_ipdata);
        CFW_SetPsDataAttribute(&pGprsData->attribute, 0, 0, 0);
        if (0 != CFW_GprsSendData(cid, pGprsData, nSim))
        {
            OSI_LOGI(0x100042c3, "------CFW_GprsSendData error");
            errCode = ERR_AT_CME_OTHOR_ERROR;
        }
        goto nonip_result;
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        uint8_t *AtRet = "+NIPDATA: (cid,\"This is Non-IP Data\")";
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto nonip_result;
    }

nonip_result:
    if (pGprsData != NULL)
    {
        AT_FREE(pGprsData);
    }
    if (errCode == CMD_FUNC_SUCC)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x100042e8, "errCode : %d", errCode);
        RETURN_CME_ERR(cmd->engine, errCode);
    }
#endif
}

void NonIpDataHandler(uint8_t cid, CFW_GPRS_DATA *pGprsData, uint8_t nSimId)
{
#ifdef PORTING_ON_GOING
    uint8_t *retRsp = NULL;
    INT16 length = 0;
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(AT_ASYN_GET_DLCI(nSimId));
    if (pGprsData == NULL)
        return;
    if (pGprsData->nDataLength > CFW_GetNonIpMtu(cid))
    {
        goto label_free_mem;
    }
    retRsp = CSW_GPRS_MALLOC(pGprsData->nDataLength * 2 + 30);
    if (retRsp == NULL)
    {
        goto label_free_mem;
    }
    memset(retRsp, 0x00, pGprsData->nDataLength * 2 + 30);
    if (pGprsData->nDataLength != 0)
    {
        length = sprintf(retRsp, "+NIPDATA:%d,%d,");
        if (false == AT_UtilOctetToString(pGprsData->pData, pGprsData->nDataLength, retRsp + length))
        {
            OSI_LOGI(0x10005668, "AT_UtilOctetToString return false");
            goto label_free_mem;
        }
    }
    else
    {
        CSW_GPRS_FREE(retRsp);
        retRsp = CSW_GPRS_MALLOC(30);
        memset(retRsp, 0x00, 30);
        sprintf(retRsp, "+NIPDATA:%d,0,\"\"", cid);
    }
    length = strlen(retRsp);
    retRsp[length] = '\r';
    retRsp[length + 1] = '\n';
    retRsp[length + 2] = '\0';
    length = strlen(retRsp);
    if (length > 0)
        at_CmdWrite(engine, retRsp, length);
label_free_mem:
    CSW_GPRS_FREE(pGprsData);
    if (retRsp)
        CSW_GPRS_FREE(retRsp);
#endif
}

// 10.1.26 Define EPS quality of service
void atCmdHandleCGEQOS(atCommand_t *cmd)
{
    static const osiUintRange_t qci_ranges[] = {
        {0, 9},
        {75, 75},
        {79, 79},
        {128, 254},
    };
    static const uint32_t qci_unsupported[] = {65, 66, 67, 69, 70};

    uint8_t nSim = atCmdGetSim(cmd->engine);
    CFW_EQOS sEqos = {};

    if (AT_CMD_SET == cmd->type)
    {
        // +CGEQOS=[<cid>[,<QCI>[,<DL_GBR>,<UL_GBR>[,<DL_MBR>,<UL_MBR]]]]
        // Though cid is optional in 3GPP, it is handled as required

        bool paramok = true;
        unsigned cid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        unsigned qci = atParamDefInt(cmd->params[1], 0, &paramok);
        unsigned dl_gbr = atParamDefUintInRange(cmd->params[2], 0, 0, 10000, &paramok);
        unsigned ul_gbr = atParamDefUintInRange(cmd->params[3], 0, 0, 5000, &paramok);
        unsigned dl_mbr = atParamDefUintInRange(cmd->params[4], 0, 0, 10000, &paramok);
        unsigned ul_mbr = atParamDefUintInRange(cmd->params[5], 0, 0, 5000, &paramok);

        AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[cid];
        if (0 == pinfo->uCid)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

        if (osiIsUintInList(qci, qci_unsupported, OSI_ARRAY_SIZE(qci_unsupported)))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_GPRS_UNSUPPORTED_QCI_VALUE);

        if (!osiIsUintInRanges(qci, qci_ranges, OSI_ARRAY_SIZE(qci_ranges)))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (!paramok || cmd->param_count > 6 || cmd->param_count == 3 || cmd->param_count == 5)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        //if (gAtCfwCtx.sim[nSim].cid_info[cid].uCid == 0)
        //    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        sEqos.nQci = qci;
        sEqos.nDlGbr = dl_gbr;
        sEqos.nUlGbr = ul_gbr;
        sEqos.nDlMbr = dl_mbr;
        sEqos.nUlMbr = ul_mbr;
        CFW_GprsSetCtxEQos(cid, &sEqos, nSim);

        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char rsp[64];
        for (int uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            if (0 != CFW_GprsGetCtxEQos(uCid, &sEqos, nSim))
                continue;

            sprintf(rsp, "+CGEQOS: %d,%d,%ld,%ld,%ld,%ld",
                    uCid, sEqos.nQci, sEqos.nDlGbr, sEqos.nUlGbr,
                    sEqos.nDlMbr, sEqos.nUlMbr);
            atCmdRespInfoText(cmd->engine, rsp);
        }
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        const char *pTest = "+CGEQOS: (1..7), (0..9,75,79,128..254), (0..10000), (0..5000), (0..10000), (0..5000)";
        atCmdRespInfoText(cmd->engine, pTest);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleCGEQOSRDP(atCommand_t *cmd)
{
    char pResp[400] = {0};
    uint8_t uCid = 0;
    CFW_EQOS sEqos = {0, 0, 0, 0, 0};
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count != 1)
        {
            OSI_LOGI(0x100052b8, "AT+CGEQOSRDP:uParamCount error,cmd->param_count = %d.", cmd->param_count);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        uCid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);

        if (paramok == false)
        {
            OSI_LOGI(0x100042ec, "AT+CGEQOS:The param uCid error,uCid = %d.", uCid);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (0 != CFW_GprsGetCtxEQos(uCid, &sEqos, nSim))
        {
            OSI_LOGI(0x100042f5, "AT+CGEQOSRDP:CFW_GprsSetReqQos() failed");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            uint8_t nActState = 0;
            CFW_GetGprsActState(uCid, &nActState, nSim);
            if (nActState == CFW_GPRS_ACTIVED)
            {
                sprintf(pResp, "+CGEQOSRDP:%d,%d,%ld,%ld,%ld,%ld",
                        uCid, sEqos.nQci, sEqos.nDlGbr, sEqos.nUlGbr, sEqos.nDlMbr, sEqos.nUlMbr);
                atCmdRespInfoText(cmd->engine, pResp);
            }
            RETURN_OK(cmd->engine);
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CGEQOSRDP: (1..7)");
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_EXE == cmd->type)
    {
        for (int uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            uint8_t nActState = 0;
            CFW_GetGprsActState(uCid, &nActState, nSim);
            if (nActState == CFW_GPRS_ACTIVED)
            {
                AT_Gprs_CidInfo *pinfo = &gAtCfwCtx.sim[nSim].cid_info[uCid];
                if (pinfo->uCid != 0)
                {
                    memset(&sEqos, 0, sizeof(CFW_EQOS));
                    if (0 != CFW_GprsGetCtxEQos(uCid, &sEqos, nSim))
                    {
                        OSI_LOGI(0x100042f5, "AT+CGEQOSRDP:CFW_GprsSetReqQos() failed");
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    else
                    {
                        sprintf(pResp, "+CGEQOSRDP:%d,%d,%ld,%ld,%ld,%ld",
                                uCid, sEqos.nQci, sEqos.nDlGbr, sEqos.nUlGbr, sEqos.nDlMbr, sEqos.nUlMbr);
                        atCmdRespInfoText(cmd->engine, pResp);
                    }
                }
            }
        }
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleCSODCP(atCommand_t *cmd)
{
    bool paramok = true;
    uint8_t uCidState = 0;
    uint8_t cid = 0;
    uint16_t cpdata_len = 0;
    uint8_t rai = 0;
    uint8_t type_of_user_data = 0;
    CFW_GPRS_DATA *pGprsData = NULL;
    const char *cpdata = NULL;
    CFW_SIM_ID nSim = (CFW_SIM_ID)atCmdGetSim(cmd->engine);

    if (AT_CMD_SET == cmd->type)
    {
        if ((cmd->param_count < 3) || (cmd->param_count > 5))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (cmd->param_count >= 3)
        {
            cid = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (gAtCfwCtx.sim[nSim].cid_info[cid].uCid == 0)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            CFW_GetGprsActState(cid, &uCidState, nSim);
            if (uCidState != CFW_GPRS_ACTIVED)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            cpdata_len = atParamUintInRange(cmd->params[1], 0, 400, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            cpdata = atParamStr(cmd->params[2], &paramok);
            if (!strcmp(cpdata, ""))
            {
                if (cpdata_len != 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            else
            {
                if (cpdata_len == 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (cpdata_len * 2 != strlen(cpdata))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (cmd->param_count >= 4)
                rai = atParamDefUintInRange(cmd->params[3], 0, 0, 2, &paramok);
            if (cmd->param_count >= 5)
                type_of_user_data = atParamDefUintInRange(cmd->params[4], 0, 0, 1, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            if ((pGprsData = malloc(cpdata_len + sizeof(CFW_GPRS_DATA))) == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            memset(pGprsData, 0, cpdata_len + sizeof(CFW_GPRS_DATA));
            if (cfwHexStrToBytes(cpdata, cpdata_len * 2, pGprsData->pData) < 0)
            {
                free(pGprsData);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_UNKNOWN_ERROR);
            }
            pGprsData->nDataLength = cpdata_len;
            CFW_SetPsDataAttribute(&pGprsData->attribute, rai, type_of_user_data, 0);
            if (0 != CFW_GprsSendData(cid, pGprsData, rai, nSim))
            {
                free(pGprsData);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_UNKNOWN_ERROR);
            }
            free(pGprsData);
            RETURN_OK(cmd->engine);
        }
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CSODCP: (1~7), (400), (0~2), (0~1)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

uint8_t gAtCrtdcpReportFlag = 0;
void atCmdHandleCRTDCP(atCommand_t *cmd)
{
    bool paramok = true;
    if (AT_CMD_SET == cmd->type)
    {
        gAtCrtdcpReportFlag = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CRTDCP: (0,1),(1-7),(1500)");
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char pTestRsp[20] = {0};
        sprintf(pTestRsp, "+CRTDCP: %d", gAtCrtdcpReportFlag);
        atCmdRespInfoText(cmd->engine, pTestRsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x100042fd, "AT_GPRS_CmdFunc_CRTDCP: This type does not supported!");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void CrtdcpDataHandler(uint8_t cid, CFW_GPRS_DATA *pGprsData, uint8_t nSimId)
{
#ifdef PORTING_ON_GOING
    uint8_t *retRsp = NULL;
    INT16 length = 0;
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(AT_ASYN_GET_DLCI(nSimId));
    if (pGprsData == NULL)
        return;
    if (pGprsData->nDataLength > 1600)
    {
        goto label_free_mem;
    }

    if (gAtCrtdcpReportFlag)
        goto label_free_mem;

    retRsp = CSW_GPRS_MALLOC(pGprsData->nDataLength * 2 + 30);

    if (retRsp == NULL)
    {
        goto label_free_mem;
    }
    memset(retRsp, 0x00, pGprsData->nDataLength * 2 + 30);

    if (pGprsData->nDataLength != 0)
    {
        length = sprintf(retRsp, "+CRTDCP:%d,%d,");
        if (false == AT_UtilOctetToString(pGprsData->pData, pGprsData->nDataLength, retRsp + length))
        {
            OSI_LOGI(0x10005668, "AT_UtilOctetToString return false");
            goto label_free_mem;
        }
    }
    else
    {
        CSW_GPRS_FREE(retRsp);
        retRsp = CSW_GPRS_MALLOC(30);
        memset(retRsp, 0x00, 30);
        sprintf(retRsp, "+CRTDCP:%d,0,\"\"", cid);
    }
    length = strlen(retRsp);
    retRsp[length] = '\r';
    retRsp[length + 1] = '\n';
    retRsp[length + 2] = '\0';
    length = strlen(retRsp);
    if (length > 0)
        at_CmdWrite(engine, retRsp, length);
label_free_mem:
    CSW_GPRS_FREE(pGprsData);
    if (retRsp)
        CSW_GPRS_FREE(retRsp);
#endif
}

void atCmdHandleDIRECTIP(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    bool paramok = true;
    uint32_t errCode = CMD_FUNC_SUCC;
    uint8_t ucByte = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t AtRet[100] = {0}; /* max 20 charactors per cid */
    OSI_LOGI(0x100042fe, "AT+DIRECTIP: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        OSI_LOGI(0x100042ff, "AT+DIRECTIP: parameters is NULL");
        errCode = ERR_AT_CME_PARAM_INVALID;
        goto directip_fail;
    }
    if (AT_CMD_SET == cmd->type)
    {
        if (1 != cmd->param_count)
        {
            OSI_LOGI(0x10004300, "AT+DIRECTIP: Parameter count error. count = %d.", cmd->param_count);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto directip_fail;
        }
        ucByte = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (paramok == false)
        {
            OSI_LOGI(0x10004301, "AT+DIRECTIP: get para directip error. value = %d", ucByte);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto directip_fail;
        }
        nbiot_nvSetDirectIpMode(ucByte);
        OSI_LOGI(0x10004302, "AT+DIRECTIP: get para directip=%d successfully", ucByte);

        nvmWriteStatic();
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+DIRECTIP: %d", nbiot_nvGetDirectIpMode());
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+DIRECTIP: value=[0-1](0-default, use internal lwip; 1-direct ip packet from external mcu)");
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto directip_fail;
    }
directip_fail:
    OSI_LOGI(0x10004303, "AT+DIRECTIP: errCode : %d", errCode);
    RETURN_CME_ERR(cmd->engine, errCode);
#endif
}

#if 0
void atCmdHandleVERCTRL(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    uint32_t errCode = CMD_FUNC_SUCC;
    uint8_t ucByte = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t AtRet[100] = {0}; /* max 20 charactors per cid */

    OSI_LOGI(0x10004304, "AT+VERCTRL: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        OSI_LOGI(0x10004305, "AT+VERCTRL: parameters is NULL");
        errCode = ERR_AT_CME_PARAM_INVALID;
        goto verctrl_fail;
    }

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        // check paracount
        if ((cmd->param_count == 0) || (cmd->param_count > 2))
        {
            OSI_LOGI(0x10004306, "AT+VERCTRL: Parameter count error. count = %d.", cmd->param_count);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto verctrl_fail;
        }

        //enable
        if (cmd->param_count >= 1)
        {
            ucByte = atParamDefUintInRange(cmd->params[0], nbiot_nvGetVersionControl(), 0, 2, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x10004307, "AT+VERCTRL: get para verctrl error. enable = %d", ucByte);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto verctrl_fail;
            }
            nbiot_nvSetVersionControl(ucByte);
            OSI_LOGI(0x10004308, "AT+VERCTRL: get para verctrl=%d successfully", ucByte);
        }
        if (cmd->param_count >= 2)
        {
            ucByte = atParamDefUintInRange(cmd->params[1], nbiot_nvGetPdnAutoAttach(), 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x10004309, "AT+VERCTRL: get para auto_pdn_attach error. enable = %d", ucByte);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto verctrl_fail;
            }
            nbiot_nvSetPdnAutoAttach(ucByte);
            OSI_LOGI(0x1000430a, "AT+VERCTRL: get para auto_pdn_attach=%d successfully", ucByte);
        }
        nvmWriteStatic();
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+VERCTRL: %d,%d", nbiot_nvGetVersionControl(), nbiot_nvGetPdnAutoAttach());
        AT_CMD_RETURN(at_CmdRespOKText(cmd->engine, AtRet));
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+VERCTRL: enable=[0-2](0-storeroom; 1-gcf; 2-product), pdn_auto_attach=[0-1](0-disable;1-enable)");
        AT_CMD_RETURN(at_CmdRespOKText(cmd->engine, AtRet));
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto verctrl_fail;
    }

verctrl_fail:
    OSI_LOGI(0x1000430b, "AT+VERCTRL: errCode : %d", errCode);
    RETURN_CME_ERR(cmd->engine, errCode);
#endif
}
#endif

#ifdef CONFIG_SOC_6760
void atCmdHandleCFGDFTPDN(atCommand_t *cmd)
{

    uint8_t uPdnTypeIdx = 0;
    uint8_t uPdnTypeTable[4] = {1, 2, 3, 5};
    uint8_t uApnIndexTable[6] = {0xff, 0, 1, 2, 0xff, 3};
    uint32_t errCode = 0;
    uint8_t ucByte = 0;
    const char *apn = NULL;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    char AtRet[200] = {0}; /* max 20 charactors per cid */

    OSI_LOGI(0x1000430c, "AT+CFGDFTPDN: nSim= %d; nDLCI= 0x%02x", nSim, cmd->uti);

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        // check paracount
        if ((cmd->param_count == 0) || (cmd->param_count > 2))
        {
            OSI_LOGI(0x1000430e, "AT+CFGDFTPDN: Parameter count error. count = %d.", cmd->param_count);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfgdftpdn_fail;
        }

        //pdnType
        if (cmd->param_count >= 1)
        {
            uint32_t paramList[] = {1, 2, 3, 5};
            ucByte = atParamDefUintInList(cmd->params[0], nbiot_nvGetDefaultPdnType(), paramList, 4, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x1000430f, "AT+CFGDFTPDN: get para pdnType fail");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }
            OSI_LOGI(0x10004310, "AT+CFGDFTPDN: get para pdnType=%d successfully", ucByte);
            nbiot_nvSetDefaultPdnType(ucByte);
        }

        //apn
        if (cmd->param_count >= 2)
        {
            //apn = atParamDefStr(cmd->params[1], (const char *)nbiot_nvGetDefaultApn(uApnIndexTable[ucByte]),&paramok);
            apn = atParamDefStr(cmd->params[1], "", &paramok);
            if (!paramok)
            {
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }
            if (!atParamIsEmpty(cmd->params[1]))
                nbiot_nvSetDefaultApn(uApnIndexTable[nbiot_nvGetDefaultPdnType()], apn, ((uint8_t)strlen(apn) + 1));
            OSI_LOGXI(OSI_LOGPAR_S, 0x10004311, "AT+CFGDFTPDN: get para apn=%s successfully", apn);
        }

        nvmWriteStatic();
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        memset(AtRet, 0, sizeof(AtRet));
        for (uPdnTypeIdx = 0; uPdnTypeIdx < 4; uPdnTypeIdx++)
        {
            sprintf(AtRet, "+CFGDFTPDN: defaultPdnType=%d; [%d]pdnType=%d,apn=%s; ", nbiot_nvGetDefaultPdnType(), uPdnTypeIdx, uPdnTypeTable[uPdnTypeIdx],
                    nbiot_nvGetDefaultApn(uPdnTypeIdx));
        }
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        memset(AtRet, 0, sizeof(AtRet));
        sprintf(AtRet, "+CFGDFTPDN: pdnType=[1,2,3,5], apn=\"string\"");
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto cfgdftpdn_fail;
    }

cfgdftpdn_fail:
    OSI_LOGI(0x10004312, "AT+CFGDFTPDN: errCode : %d", errCode);
    RETURN_CME_ERR(cmd->engine, errCode);
}

void atCmdHandleLOGRANK(atCommand_t *cmd)
{
    bool paramok = true;
    uint32_t logModule = 0xFFFFFFFF;
    uint8_t logRank;
    OSI_LOGI(0, "atCmdHandleLOGRANK in");
    if (AT_CMD_SET == cmd->type)
    {
        logRank = atParamDefUintInRange(cmd->params[0], nbiot_nvGetLogRank(), 0, 5, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        nbiot_nvSetLogRank(logRank);
        if (cmd->param_count > 1)
        {
            logModule = atParamInt(cmd->params[1], &paramok);
        }

        if (!paramok || cmd->param_count > 2)
            AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID));

        nbiot_nvSetLogModule(logModule);
        nvmWriteStatic();
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char ret[50];
        memset(ret, 0, 50);
        logRank = nbiot_nvGetLogRank();
        logModule = nbiot_nvGetLogModule();
        sprintf(ret, "+LOGRANK: %u, %lu", logRank, logModule);
        atCmdRespInfoText(cmd->engine, ret);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+LOGRANK: rank(0~5), module(bitmap)");
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}
#endif

#ifdef CONFIG_SOC_8909
void atCmdHandleCFGCIOT(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t nonip = atParamDefUintInRange(cmd->params[0], nbiot_nvGetCiotNonip(), 0, 1, &paramok);
        uint8_t cpciot = atParamDefUintInRange(cmd->params[1], nbiot_nvGetCiotCpciot(), 0, 1, &paramok);
        uint8_t upciot = atParamDefUintInRange(cmd->params[2], nbiot_nvGetCiotUpciot(), 0, 3, &paramok);
        uint8_t erwopdn = atParamDefUintInRange(cmd->params[3], nbiot_nvGetCiotErwopdn(), 0, 2, &paramok);
        uint8_t sms_wo_comb_att = atParamDefUintInRange(cmd->params[4], nbiot_nvGetCiotSmsWoCombAtt(), 0, 1, &paramok);
        uint8_t apn_rate_control = atParamDefUintInRange(cmd->params[5], nbiot_nvGetCiotApnRateCtrl(), 0, 1, &paramok);
        uint8_t epco = atParamDefUintInRange(cmd->params[6], nbiot_nvGetCiotEpco(), 0, 1, &paramok);
        uint8_t cpbackoff = atParamDefUintInRange(cmd->params[7], nbiot_nvGetCpBackOffEnable(), 0, 1, &paramok);
        uint8_t roam = atParamDefUintInRange(cmd->params[8], nbiot_nvGetCiotRoam(), 0, 1, &paramok);
        if (!paramok || cmd->param_count > 9)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        nbiot_nvSetCiotNonip(nonip);
        nbiot_nvSetCiotCpciot(cpciot);
        nbiot_nvSetCiotUpciot(upciot);
        nbiot_nvSetCiotErwopdn(erwopdn);
        nbiot_nvSetCiotSmsWoCombAtt(sms_wo_comb_att);
        nbiot_nvSetCiotApnRateCtrl(apn_rate_control);
        nbiot_nvSetCiotEpco(epco);
        nbiot_nvSetCpBackOffEnable(cpbackoff);
        nbiot_nvSetCiotRoam(roam);
        nvmWriteStatic();
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char resp[200];
        sprintf(resp, "+CFGCIOT: nonip=%d, cpciot=%d, upciot=%d, erwopdn=%d, "
                      "sms_wo_comb_att=%d, apn_rate_control=%d, epco=%d, cpbackoff=%d, roam=%d",
                nbiot_nvGetCiotNonip(),
                nbiot_nvGetCiotCpciot(),
                nbiot_nvGetCiotUpciot(),
                nbiot_nvGetCiotErwopdn(),
                nbiot_nvGetCiotSmsWoCombAtt(),
                nbiot_nvGetCiotApnRateCtrl(),
                nbiot_nvGetCiotEpco(),
                nbiot_nvGetCpBackOffEnable(),
                nbiot_nvGetCiotRoam());
        atCmdRespInfoText(cmd->engine, resp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char resp[200];
        sprintf(resp, "+CFGCIOT: nonip=[0-1], cpciot=[0-1](NBIoT Ignore), upciot=[0-3], "
                      "erwopdn=[0-2], sms_wo_comb_att=[0-1], apn_rate_control=[0-1], "
                      "epco=[0-1], cpbackoff=[0-1], roam=[0-1]");
        atCmdRespInfoText(cmd->engine, resp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
#endif

#ifdef CONFIG_SOC_8910
void atCmdHandleCFGCIOT(atCommand_t *cmd)
{
    uint8_t sim = atCmdGetSim(cmd->engine);
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t nonip = atParamDefUintInRange(cmd->params[0], CFW_nvGetCiotNonip(sim), 0, 1, &paramok);
        uint8_t cpciot = atParamDefUintInRange(cmd->params[1], CFW_nvGetCiotCpciot(sim), 0, 1, &paramok);
        uint8_t upciot = atParamDefUintInRange(cmd->params[2], CFW_nvGetCiotUpciot(sim), 0, 1, &paramok);
        uint8_t erwopdn = atParamDefUintInRange(cmd->params[3], CFW_nvGetCiotErwopdn(sim), 0, 1, &paramok);
        uint8_t sms_wo_comb_att = atParamDefUintInRange(cmd->params[4], CFW_nvGetCiotSmsWoCombAtt(sim), 0, 1, &paramok);
        uint8_t apn_rate_control = atParamDefUintInRange(cmd->params[5], CFW_nvGetCiotApnRateCtrl(sim), 0, 1, &paramok);
        //  uint8_t epco = atParamDefUintInRange(cmd->params[6], nbiot_nvGetCiotEpco(), 0, 1, &paramok);
        //  uint8_t cpbackoff = atParamDefUintInRange(cmd->params[7], nbiot_nvGetCpBackOffEnable(), 0, 1, &paramok);
        //  uint8_t roam = atParamDefUintInRange(cmd->params[8], nbiot_nvGetCiotRoam(), 0, 1, &paramok);
        if (!paramok || cmd->param_count > 6)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        CFW_nvSetCiotNonip(nonip, sim);
        CFW_nvSetCiotCpciot(cpciot, sim);
        CFW_nvSetCiotUpciot(upciot, sim);
        CFW_nvSetCiotErwopdn(erwopdn, sim);
        CFW_nvSetCiotSmsWoCombAtt(sms_wo_comb_att, sim);
        CFW_nvSetCiotApnRateCtrl(apn_rate_control, sim);
        CFW_nvmWriteStatic(sim);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char resp[200];
        sprintf(resp, "+CFGCIOT: nonip=%d, cpciot=%d, upciot=%d, erwopdn=%d, "
                      "sms_wo_comb_att=%d, apn_rate_control=%d",
                CFW_nvGetCiotNonip(sim),
                CFW_nvGetCiotCpciot(sim),
                CFW_nvGetCiotUpciot(sim),
                CFW_nvGetCiotErwopdn(sim),
                CFW_nvGetCiotSmsWoCombAtt(sim),
                CFW_nvGetCiotApnRateCtrl(sim));
        atCmdRespInfoText(cmd->engine, resp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char resp[200];
        sprintf(resp, "+CFGCIOT: nonip=[0-1], cpciot=[0-1](NBIoT Ignore), upciot=[0-1], "
                      "erwopdn=[0-1], sms_wo_comb_att=[0-1], apn_rate_control=[0-1]");
        atCmdRespInfoText(cmd->engine, resp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
#endif

/*void atCmdHandleCEDRXRDP(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    if (AT_CMD_EXE == cmd->type)
    {
        uint8_t nResp[200] = {0};
        uint8_t requested_eDrx[8] = {0};
        uint8_t nw_provided_eDrx[8] = {0};
        uint8_t ptw[8] = {0};
        memset(nResp, 0x00, sizeof(nResp));
        memset(requested_eDrx, 0x00, sizeof(requested_eDrx));
        memset(nw_provided_eDrx, 0x00, sizeof(nw_provided_eDrx));
        memset(ptw, 0x00, sizeof(ptw));
        uint8_t mode = nbiot_nvGetEdrxEnable();
        OSI_LOGI(0x100000ea, "AT+CEDRXRDP: mode = %d", mode);
        if ((mode == 0) || (mode == 3))
        {
            sprintf(nResp, "+CEDRXRDP: 0");
        }
        else
        {
            AT_Util_ByteToBitStr(requested_eDrx, nbiot_nvGetEdrxValue(), 4);
            AT_Util_ByteToBitStr(nw_provided_eDrx, gATCurrentNwEdrxValue, 4);
            AT_Util_ByteToBitStr(ptw, gATCurrentNwEdrxPtw, 4);
            sprintf(nResp, "+CEDRXRDP: 5, %s, %s, %s", requested_eDrx, nw_provided_eDrx, ptw);
        }
        atCmdRespInfoText(cmd->engine, nResp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
#endif
}*/

/*void atCmdHandleCFGEDRX(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    uint8_t AtRet[50] = {0}; // max 20 charactors per cid
    uint8_t nvWriten = 0;
    uint8_t mode = 0;
    uint8_t edrxPtw = 0;
    uint8_t edrxValue = 0;
    uint8_t ucGetEnable = nbiot_nvGetEdrxEnable();
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count > 3)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (cmd->param_count >= 1)
        {
            mode = atParamDefUintInRange(cmd->params[0], nbiot_nvGetEdrxEnable(), 0, 1, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            if (ucGetEnable != mode)
            {
                nvWriten |= 0x81;
            }

            if (cmd->param_count >= 2)
            {
                edrxPtw = atParamDefUintInRange(cmd->params[1], nbiot_nvGetEdrxPtw(), 0, 15, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                if (edrxPtw != nbiot_nvGetEdrxPtw())
                {
                    nvWriten |= 0x02;
                    if (mode != 0)
                        nvWriten |= 0x80;
                }
            }
        }
        if (cmd->param_count >= 3)
        {
            edrxValue = atParamDefUintInRange(cmd->params[2], nbiot_nvGetEdrxValue(), 0, 15, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (nbiot_nvGetEdrxValue() != edrxValue)
            {
                nvWriten |= 0x04;
                if (mode != 0)
                    nvWriten |= 0x80;
            }
        }

        if (nvWriten)
        {
            if (nvWriten & 0x01)
            {
                nbiot_nvSetEdrxEnable(mode);
                if ((ucGetEnable == 3) && (mode == 0) || (ucGetEnable == 2) && (mode == 1))
                    nvWriten &= ~0x80;
            }
            if (nvWriten & 0x02)
                nbiot_nvSetEdrxPtw(edrxPtw);
            if (nvWriten & 0x04)
                nbiot_nvSetEdrxValue(edrxValue);
            nvmWriteStatic();
            if (nvWriten & 0x80)
                CFW_NvParasChangedNotification(NV_CHANGED_EDRX);
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        if (ucGetEnable == 2)
            ucGetEnable = 1;
        else if (ucGetEnable == 3)
            ucGetEnable = 0;

        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+CFGEDRX: %d, ptw=%d, edrxVal=%d",
                ucGetEnable,
                nbiot_nvGetEdrxPtw(),
                nbiot_nvGetEdrxValue());
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+CFGEDRX: enable=[0-1], edrxPtw=[0-15], edrxValue=[0-15]");
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
#endif
}*/

/*void atCmdHandleCEDRXS(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    uint8_t *aucBitStr = NULL;
    uint8_t AtRet[50] = {0}; //max 20 charactors per cid
    uint8_t nvWriten = 0;
    uint8_t mode = 0;
    uint8_t edrxValue = 0;
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count > 3)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (cmd->param_count >= 1)
        {
            mode = atParamDefUintInRange(cmd->params[0], nbiot_nvGetEdrxEnable(), 0, 3, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            switch (mode)
            {
            case 0:
                if (nbiot_nvGetEdrxEnable() != 0)
                {
                    nvWriten |= 0x01;
                    if (nbiot_nvGetEdrxEnable() != 3)
                        nvWriten |= 0x80;
                }
                break;

            case 1:
            case 2:
                if (nbiot_nvGetEdrxEnable() != mode)
                {
                    nvWriten |= 0x01;
                    if (nbiot_nvGetEdrxEnable() != ((mode == 1) ? 2 : 1))
                        nvWriten |= 0x80;
                }
                break;

            case 3:
                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                if (nbiot_nvGetEdrxEnable() != 3)
                {
                    nvWriten |= 0x01;
                    if (nbiot_nvGetEdrxEnable() != 0)
                        nvWriten |= 0x80;
                }
                if (nbiot_nvGetEdrxValue() != 0)
                {
                    nvWriten |= 0x02;
                    edrxValue = 0;
                }
                goto edrx_result;
                break;
            }
        }
        if (cmd->param_count >= 2)
        {
            uint8_t actType = atParamDefUintInRange(cmd->params[1], 5, 5, 5, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (cmd->param_count >= 3)
        {
            aucBitStr = atParamOptStr(cmd->params[2], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->params[2]->type != AT_CMDPARAM_EMPTY)
            {
                if (!AT_Util_BitStrToByte(&edrxValue, aucBitStr))
                {
                    //ATCMDLOG(E, "AT+CEDRXS: para edrxValue illegal. edrxValue = %s", aucBitStr);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (edrxValue > 15)
                {
                    //ATCMDLOG(E, "AT+CEDRXS: get para edrxValue error. edrxValue = %d", edrxValue);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (nbiot_nvGetEdrxValue() != edrxValue)
                {
                    nvWriten |= 0x02;
                    if (mode != 0)
                        nvWriten |= 0x80;
                }
            }
        }
    edrx_result:
        if (nvWriten)
        {
            if (nvWriten & 0x01)
                nbiot_nvSetEdrxEnable(mode);
            if (nvWriten & 0x02)
                nbiot_nvSetEdrxValue(edrxValue);
            nvmWriteStatic();
            if (nvWriten & 0x80)
                CFW_NvParasChangedNotification(NV_CHANGED_EDRX);
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t destAucBitStr[8] = {0};
        AT_Util_ByteToBitStr(destAucBitStr, nbiot_nvGetEdrxValue(), 4);

        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+CEDRXS: %d,5,\"%s\"",
                nbiot_nvGetEdrxEnable(),
                destAucBitStr);
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        uint8_t *respInfo = "+CEDRXS: mode=[0-3], AcT-type=5, Requested_eDRX_value=\"4bitString eg.0100 \"";
        atCmdRespInfoText(cmd->engine, respInfo);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
#endif
}*/

void atCmdHandleCGAPNRC(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    uint32_t errCode = CMD_FUNC_SUCC;
    uint8_t ucByte = 0;
    uint8_t cid = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t AtRet[200] = {0}; /* max 20 charactors per cid */
    uint8_t aer, uState;
    uint8_t unit;
    uint32_t rate;
    uint8_t i = 0;
    uint16_t apnRetLen = 0;
    AT_Gprs_CidInfo *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    OSI_LOGI(0x10004325, "AT+CGAPNRC: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        OSI_LOGI(0x10004326, "AT+CGAPNRC: parameters is NULL");
        errCode = ERR_AT_CME_PARAM_INVALID;
        goto cgapnrc_fail;
    }

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        // check paracount
        if (1 != cmd->param_count)
        {
            OSI_LOGI(0x10004327, "AT+CGAPNRC: Parameter count error. count = %d.", cmd->param_count);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cgapnrc_fail;
        }

        //cid
        ucByte = atParamUintInRange(cmd->params[0], AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        if (!paramok)
        {
            OSI_LOGI(0x10004328, "AT+CGAPNRC: get para n fail.");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cgapnrc_fail;
        }

        cid = ucByte;
        CFW_GetGprsActState(cid, &uState, nSim);
        if (uState == AT_GPRS_ACT)
        {
            aer = g_staAtGprsCidInfo[cid].apnacAer;
            unit = g_staAtGprsCidInfo[cid].apnacUnit;
            rate = g_staAtGprsCidInfo[cid].apnacRate;
            AT_MemSet(AtRet, 0, SIZEOF(AtRet));
            sprintf(AtRet, "+CGAPNRC: %d,%d,%d,%d", cid, aer, unit, rate);
            atCmdRespInfoText(cmd->engine, AtRet);
            RETURN_OK(cmd->engine);
        }
        else
        {
            goto cgapnrc_fail;
        }
    }
    else if (AT_CMD_EXE == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        for (i = 1; i < AT_PDPCID_MAX + 1; i++)
        {
            CFW_GetGprsActState(g_staAtGprsCidInfo[i].uCid, &uState, nSim);
            if (uState == AT_GPRS_ACT)
            {
                g_staAtGprsCidInfo[i].uState = uState;
                apnRetLen = sprintf(AtRet + apnRetLen, "\r\n+CGAPNRC: %d,%d,%d,%d",
                                    g_staAtGprsCidInfo[i].uCid, g_staAtGprsCidInfo[i].apnacAer, g_staAtGprsCidInfo[i].apnacUnit, g_staAtGprsCidInfo[i].apnacRate);
            }
        }
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        apnRetLen += sprintf(AtRet + apnRetLen, "+CGAPNRC: ");
        for (i = 1; i < AT_PDPCID_MAX + 1; i++)
        {
            CFW_GetGprsActState(g_staAtGprsCidInfo[i].uCid, &uState, nSim);
            if ((uState == AT_GPRS_ACT) && (g_staAtGprsCidInfo[i].uCid > 0))
            {
                apnRetLen += sprintf(AtRet + apnRetLen, "%d,", g_staAtGprsCidInfo[i].uCid);
            }
        }
        AtRet[apnRetLen - 1] = 0;
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto cgapnrc_fail;
    }

cgapnrc_fail:
    OSI_LOGI(0x10004329, "AT+CGAPNRC: errCode : %d", errCode);
    RETURN_CME_ERR(cmd->engine, errCode);
#endif
}

/*void atCmdHandleCPSMS(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    uint8_t mode = 0;
    uint8_t T3412 = 0;
    uint8_t T3324 = 0;
    uint8_t *aucBitStr = NULL;
    uint8_t AtRet[50] = {0};
    uint8_t nvWriten = 0; //set hightest bit should notify nas do tau, bit 0 write mode to nvm, bit 1 write t3412, bit 2 write t3324

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count > 5)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (cmd->param_count >= 1)
        {
            mode = atParamDefUintInRange(cmd->params[0], nbiot_nvGetPsmEnable(), 0, 2, &paramok);
            //ATCMDLOG(I, "cpsms mode %d, paramCount %d", mode, cmd->param_count);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            switch (mode)
            {
            case 0:
                if (nbiot_nvGetPsmEnable() != 0)
                {
                    nvWriten |= 0x01;
                    if (nbiot_nvGetPsmEnable() == 1)
                        nvWriten |= 0x80;
                }
                break;

            case 1:
                if (nbiot_nvGetPsmEnable() != 1)
                {
                    nvWriten |= 0x81;
                }
                break;

            case 2:
                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                if (nbiot_nvGetPsmEnable() != 2)
                {
                    nvWriten |= 0x01;
                    if (nbiot_nvGetPsmEnable() == 1)
                        nvWriten |= 0x80;
                }

                if (nbiot_nvGetPsmT3412() != 0x45)
                {
                    nvWriten |= 0x02;
                    T3412 = 0x45;
                }
                if (nbiot_nvGetPsmT3324() != 0x00)
                {
                    nvWriten |= 0x04;
                    T3324 = 0;
                }
                //ATCMDLOG(I, "AT+CPSMS: reset para, mode=2, t3412val=0x45, t3324Val=0x00");
                goto cpsms_result;
                break;
            }
        }
        if (cmd->param_count >= 2) //only param checking
        {
            aucBitStr = atParamOptStr(cmd->params[1], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->params[1]->type != AT_CMDPARAM_EMPTY)
            {
                if (!AT_Util_IfBitStr(aucBitStr))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        if (cmd->param_count >= 3) //only param checking
        {
            aucBitStr = atParamOptStr(cmd->params[2], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->params[2]->type != AT_CMDPARAM_EMPTY)
            {
                if (!AT_Util_IfBitStr(aucBitStr))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        if (cmd->param_count >= 4) //T3412
        {
            aucBitStr = atParamOptStr(cmd->params[3], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->params[3]->type != AT_CMDPARAM_EMPTY)
            {
                if (!AT_Util_IfBitStr(aucBitStr))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                if (!AT_Util_BitStrToByte(&T3412, aucBitStr))
                {
                    //ATCMDLOG(E, "AT+CPSMS: para T3412 illegal. T3412 = %s", aucBitStr);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (nbiot_nvGetPsmT3412() != T3412)
                {
                    nvWriten |= 0x02;
                    if (mode == 1)
                        nvWriten |= 0x80;
                }
                //ATCMDLOG(I, "AT+CPSMS: get para t3412Val=0x%02x successfully", T3412);
            }
        }

        if (cmd->param_count >= 5) //T3324
        {
            aucBitStr = atParamOptStr(cmd->params[4], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->params[4]->type != AT_CMDPARAM_EMPTY)
            {
                if (!AT_Util_IfBitStr(aucBitStr))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                if (!AT_Util_BitStrToByte(&T3324, aucBitStr))
                {
                    //ATCMDLOG(E, "AT+CPSMS: para T3324 illegal. T3324 = %s", aucBitStr);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (nbiot_nvGetPsmT3324() != T3324)
                {
                    nvWriten |= 0x04;
                    if (mode == 1)
                        nvWriten |= 0x80;
                }
                //ATCMDLOG(I, "AT+CPSMS: get para T3324Val=0x%02x successfully", T3324);
            }
        }
    cpsms_result:
        if (nvWriten)
        {
            if (nvWriten & 0x01)
                nbiot_nvSetPsmEnable(mode);
            if (nvWriten & 0x02)
                nbiot_nvSetPsmT3412(T3412);
            if (nvWriten & 0x04)
                nbiot_nvSetPsmT3324(T3324);
            nvmWriteStatic();
            if (nvWriten & 0x80)
                CFW_NvParasChangedNotification(NV_CHANGED_PSM);
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t aucBitStrRead[12] = {0};
        uint8_t aucBitStrRead2[12] = {0};
        AT_Util_ByteToBitStr(&aucBitStrRead[0], nbiot_nvGetPsmT3412(), 8);
        AT_Util_ByteToBitStr(&aucBitStrRead2[0], nbiot_nvGetPsmT3324(), 8);

        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+CPSMS: %d,,,\"%s\",\"%s\"",
                nbiot_nvGetPsmEnable(),
                &aucBitStrRead[0], &aucBitStrRead2[0]);
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        uint8_t *testRsp = "+CPSMS: mode=[0-2],,,Requested_Periodic-TAU=\"8bitStringofByte eg. 01000111\", Requested_Active-Time=\"8bitStringofByte eg. 01100101\"";
        atCmdRespInfoText(cmd->engine, testRsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
#endif
}*/
#ifdef CONFIG_SOC_8910
#define NV_CHANGED_PSM 1
void atCmdHandleCPSMS(atCommand_t *cmd)
{
    uint8_t mode = 0;
    uint8_t T3412 = 0;
    uint8_t T3324 = 0;
    const char *aucBitStr = NULL;
    uint8_t AtRet[50] = {0};
    uint8_t nvWriten = 0; //set hightest bit should notify nas do tau, bit 0 write mode to nvm, bit 1 write t3412, bit 2 write t3324
    uint8_t bitmap = 0;   /*0x01: T3312 Value; 0x02: T3314 Value; 0x04: T3412EXT Value; 0x08: T3324 Value*/
    OSI_LOGI(0, "cmd->type :%d", cmd->type);
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count > 5)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (cmd->param_count >= 1)
        {
            mode = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
            OSI_LOGI(0, "mode: %d, paramCount: %d,CFW_nvGetPsmEnable():%d", mode, cmd->param_count, CFW_nvGetPsmEnable(nSim));
            OSI_LOGI(0, "CFW_nvGetPsmT3412(): %d,CFW_nvGetPsmT3324() :%d", CFW_nvGetPsmT3412(nSim), CFW_nvGetPsmT3324(nSim));
            //ATCMDLOG(I, "cpsms mode %d, paramCount %d", mode, cmd->param_count);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            gAtSetting.psm_mode = mode;

            switch (mode)
            {
            case 0:
                if (CFW_nvGetPsmEnable(nSim) != 0)
                {
                    nvWriten |= 0x01; //NVM
                    if (CFW_nvGetPsmEnable(nSim) == 1)
                        nvWriten |= 0x80;
                }
                break;

            case 1:
                if (CFW_nvGetPsmEnable(nSim) != 1)
                {
                    nvWriten |= 0x81;
                }
                break;

            case 2:
                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                mode = 0;
                if (CFW_nvGetPsmEnable(nSim) != 0)
                {
                    nvWriten |= 0x01;
                    if (CFW_nvGetPsmEnable(nSim) == 1)
                        nvWriten |= 0x80;
                }

                if (CFW_nvGetPsmT3412(nSim) != 0xA4)
                {
                    nvWriten |= 0x02;
                    bitmap |= 0x04;
                    T3412 = 0xA4;
                }
                if (CFW_nvGetPsmT3324(nSim) != 0x24)
                {
                    nvWriten |= 0x04;
                    bitmap |= 0x08;
                    T3324 = 0x24;
                }
                //ATCMDLOG(I, "AT+CPSMS: reset para, mode=2, t3412val=0x45, t3324Val=0x00");
                //  goto cpsms_result;
                //  break;
            }
        }
        if (cmd->param_count >= 2) //only param checking
        {
            aucBitStr = atParamOptStr(cmd->params[1], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->params[1]->type != AT_CMD_PARAM_TYPE_EMPTY)
            {
                if (!AT_Util_IfBitStr(aucBitStr))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        if (cmd->param_count >= 3) //only param checking
        {
            aucBitStr = atParamOptStr(cmd->params[2], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->params[2]->type != AT_CMD_PARAM_TYPE_EMPTY)
            {
                if (!AT_Util_IfBitStr(aucBitStr))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        if (cmd->param_count >= 4) //T3412
        {
            aucBitStr = atParamOptStr(cmd->params[3], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->params[3]->type != AT_CMD_PARAM_TYPE_EMPTY)
            {
                if (!AT_Util_IfBitStr(aucBitStr))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                if (!AT_Util_BitStrToByte(&T3412, aucBitStr))
                {
                    OSI_LOGI(0, "AT+CPSMS: para T3412 illegal. T3412 = %s", aucBitStr);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (CFW_nvGetPsmT3412(nSim) != T3412)
                {
                    nvWriten |= 0x02;
                    bitmap |= 0x04;
                    if (mode == 1)
                        nvWriten |= 0x80;
                }
                OSI_LOGI(0, "AT+CPSMS: get para t3412Val=0x%02x successfully", T3412);
            }
        }

        if (cmd->param_count >= 5) //T3324
        {
            aucBitStr = atParamOptStr(cmd->params[4], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->params[4]->type != AT_CMD_PARAM_TYPE_EMPTY)
            {
                if (!AT_Util_IfBitStr(aucBitStr))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                if (!AT_Util_BitStrToByte(&T3324, aucBitStr))
                {
                    //ATCMDLOG(E, "AT+CPSMS: para T3324 illegal. T3324 = %s", aucBitStr);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (CFW_nvGetPsmT3324(nSim) != T3324)
                {
                    nvWriten |= 0x04;
                    bitmap |= 0x08;
                    if (mode == 1)
                        nvWriten |= 0x80;
                }
                OSI_LOGI(0, "AT+CPSMS: get para T3324Val=0x%02x successfully", T3324);
            }
        }
        // cpsms_result:
        /* if (nvWriten)
        {
            if (nvWriten & 0x01)
                CFW_nvSetPsmEnable(mode);
            if (nvWriten & 0x02)
                CFW_nvSetPsmT3412(T3412);
            if (nvWriten & 0x04)
                CFW_nvSetPsmT3324(T3324);
            nvmWriteStatic();
            if (nvWriten & 0x80)
                CFW_NvParasChangedNotification(NV_CHANGED_PSM);
        }*/
        OSI_LOGI(0, "nvWriten=0x%02x ", nvWriten);
        OSI_LOGI(0, "bitmap=0x%02x ", bitmap);
        CFW_GprsSetPsm(mode, bitmap, T3324, T3412, nSim);
        if (nvWriten & 0x01)
        {
            gAtSetting.psm_mode = mode;
            CFW_nvSetPsmEnable(mode, nSim);
            CFW_nvmWriteStatic(nSim);
        }
        if (nvWriten & 0x02)
        {
            CFW_nvSetPsmT3412(T3412, nSim);
            CFW_nvmWriteStatic(nSim);
        }
        if (nvWriten & 0x04)
        {
            CFW_nvSetPsmT3324(T3324, nSim);
            CFW_nvmWriteStatic(nSim);
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t aucBitStrRead[12] = {0};
        uint8_t aucBitStrRead2[12] = {0};
        AT_Util_ByteToBitStr(&aucBitStrRead[0], CFW_nvGetPsmT3412(nSim), 8);
        AT_Util_ByteToBitStr(&aucBitStrRead2[0], CFW_nvGetPsmT3324(nSim), 8);

        memset(AtRet, 0, sizeof(AtRet));
        uint8_t mode = CFW_nvGetPsmEnable(nSim);
        //  if (mode == 0 && gAtSetting.psm_mode == 2)
        //     mode = 0;
        sprintf((char *)AtRet, (const char *)"+CPSMS: %d,,,\"%s\",\"%s\"",
                mode, &aucBitStrRead[0], &aucBitStrRead2[0]);
        atCmdRespInfoText(cmd->engine, (const char *)AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        const char *testRsp = "+CPSMS: mode=[0-2],,,Requested_Periodic-TAU=\"8bitStringofByte eg. 01000111\", Requested_Active-Time=\"8bitStringofByte eg. 01100101\"";
        atCmdRespInfoText(cmd->engine, testRsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

uint8_t g_edrx_type = 4;
void atCmdHandleCEDRXS(atCommand_t *cmd)
{
    const char *aucBitStr = NULL;
    uint8_t AtRet[50] = {0}; // max 20 charactors per cid
    uint8_t nvWriten = 0;
    uint8_t mode = 0;
    uint8_t actType = 0xFF;
    uint8_t edrxValue = 0xFF;
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count > 3)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (cmd->param_count == 0)
            RETURN_OK(cmd->engine);

        if (cmd->param_count >= 1)
        {
            mode = atParamDefUintInRange(cmd->params[0], CFW_nvGetEdrxEnable(nSim), 0, 3, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            gAtSetting.drx_mode = mode;
            OSI_LOGI(0, "mode: %d, paramCount: %d,CFW_nvGetEdrxEnable():%d", mode, cmd->param_count, CFW_nvGetEdrxEnable(nSim));
            OSI_LOGI(0, "CFW_nvGetEdrxValue(): %d", CFW_nvGetEdrxValue(nSim));
            switch (mode)
            {
            case 0:
                if (CFW_nvGetEdrxEnable(nSim) != 0)
                {
                    nvWriten |= 0x81;
                }
                break;

            case 1:
            case 2:
                if (CFW_nvGetEdrxEnable(nSim) != 1)
                {
                    nvWriten |= 0x81;
                }
                break;

            case 3:
                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                if (CFW_nvGetEdrxEnable(nSim) != 0)
                {
                    nvWriten |= 0x81;
                }
                if (CFW_nvGetEdrxValue(nSim) != 0)
                {
                    nvWriten |= 0x02;
                    edrxValue = 0;
                }
                g_edrx_type = 4;
                break;
            }
        }
        if (cmd->param_count >= 2)
        {
            actType = atParamUintInRange(cmd->params[1], 4, 4, &paramok);
            OSI_LOGI(0, "actType: %d, paramok:%d", actType, paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (cmd->param_count >= 3)
        {
            aucBitStr = atParamOptStr(cmd->params[2], &paramok);
            OSI_LOGI(0, "aucBitStr: %d, paramok:%d", aucBitStr, paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (cmd->params[2]->type != AT_CMD_PARAM_TYPE_EMPTY)
            {
                if (!AT_Util_4BitStrToByte(&edrxValue, aucBitStr))
                {
                    OSI_LOGI(0, "AT+CEDRXS: para edrxValue illegal. edrxValue = %s", aucBitStr);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (edrxValue > 15)
                {
                    OSI_LOGI(0, "AT+CEDRXS: get para edrxValue error. edrxValue = %d", edrxValue);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (CFW_nvGetEdrxValue(nSim) != edrxValue)
                {
                    nvWriten |= 0x02;
                    if (mode != 0)
                        nvWriten |= 0x80;
                }
            }
        }
        CFW_GprsSetEdrx(mode, actType, edrxValue, nSim);
        if (cmd->param_count >= 2)
        {
            g_edrx_type = actType;
        }
        else
        {
            g_edrx_type = 4;
        }
        if (nvWriten & 0x01)
        {
            if (mode == 2)
                mode = 1;
            if (mode == 3)
                mode = 0;
            gAtSetting.drx_mode = mode;
            CFW_nvSetEdrxEnable(mode, nSim);
            CFW_nvmWriteStatic(nSim);
        }
        if (nvWriten & 0x02)
        {
            CFW_nvSetEdrxValue(edrxValue, nSim);
            CFW_nvmWriteStatic(nSim);
        }
        OSI_LOGI(0, "edrx set success: gAtSetting.drx_mode: %d, mode:%d,edrxValue:%d ",
                 gAtSetting.drx_mode, CFW_nvGetEdrxEnable(nSim), CFW_nvGetEdrxValue(nSim));
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t destAucBitStr[8] = {0};
        AT_Util_ByteToBitStr(destAucBitStr, CFW_nvGetEdrxValue(nSim), 4);
        memset(AtRet, 0, sizeof(AtRet));
        sprintf((char *)AtRet, "+CEDRXS: %d,%d,\"%s\"",
                CFW_nvGetEdrxEnable(nSim), g_edrx_type, destAucBitStr);
        atCmdRespInfoText(cmd->engine, (const char *)AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        const char *respInfo = "+CEDRXS: mode=[0-3], AcT-type=[4], Requested_eDRX_value=\"4bitString eg.0100 \"";
        atCmdRespInfoText(cmd->engine, respInfo);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleCEDRXRDP(atCommand_t *cmd)
{
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);

    if (AT_CMD_EXE == cmd->type)
    {
        uint8_t nResp[200] = {0};
        uint8_t requested_eDrx[8] = {0};
        uint8_t nw_provided_eDrx[8] = {0};
        uint8_t ptw[8] = {0};
        memset(nResp, 0x00, sizeof(nResp));
        memset(requested_eDrx, 0x00, sizeof(requested_eDrx));
        memset(nw_provided_eDrx, 0x00, sizeof(nw_provided_eDrx));
        memset(ptw, 0x00, sizeof(ptw));
        uint8_t mode = CFW_nvGetEdrxEnable(nSim);
        OSI_LOGI(0x100000ea, "AT+CEDRXRDP: mode = %d", mode);
        if ((mode == 0) || (mode == 3))
        {
            sprintf((char *)nResp, "+CEDRXRDP: 0");
        }
        else
        {
            AT_Util_ByteToBitStr(requested_eDrx, CFW_nvGetEdrxValue(nSim), 4);
            //  AT_Util_ByteToBitStr(nw_provided_eDrx, gATCurrentNwEdrxValue, 4);
            AT_Util_ByteToBitStr(ptw, CFW_nvGetEdrxPtw(nSim), 4);
            sprintf((char *)nResp, "+CEDRXRDP: %d, %s, 0000,%s", g_edrx_type, requested_eDrx, ptw);
        }
        atCmdRespInfoText(cmd->engine, (const char *)nResp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleCFGEDRX(atCommand_t *cmd)
{
    uint8_t AtRet[60] = {0}; /* max 20 charactors per cid */
    uint8_t nvWriten = 0;
    uint8_t mode = 0;
    uint8_t edrxPtw = 0;
    uint8_t edrxValue = 0;
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);
    uint8_t ucGetEnable = CFW_nvGetEdrxEnable(nSim);

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count > 3)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        mode = CFW_nvGetEdrxEnable(nSim);
        edrxPtw = CFW_nvGetEdrxPtw(nSim);
        edrxValue = CFW_nvGetEdrxValue(nSim);
        if (cmd->param_count >= 1)
        {
            mode = atParamDefUintInRange(cmd->params[0], CFW_nvGetEdrxEnable(nSim), 0, 1, &paramok);
            OSI_LOGI(0x100052b9, "AT+CFGEDRX: mode = %d", mode);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            if (ucGetEnable != mode)
            {
                nvWriten |= 0x81;
            }

            if (cmd->param_count >= 2)
            {
                edrxPtw = atParamDefUintInRange(cmd->params[1], CFW_nvGetEdrxPtw(nSim), 0, 15, &paramok);
                OSI_LOGI(0x100052ba, "AT+CFGEDRX: edrxPtw = %d", edrxPtw);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                if (edrxPtw != CFW_nvGetEdrxPtw(nSim))
                {
                    nvWriten |= 0x02;
                    if (mode != 0)
                        nvWriten |= 0x80;
                }
            }
        }
        if (cmd->param_count >= 3)
        {
            edrxValue = atParamDefUintInRange(cmd->params[2], CFW_nvGetEdrxValue(nSim), 0, 15, &paramok);
            OSI_LOGI(0x100052bb, "AT+CFGEDRX: edrxValue = %d", edrxValue);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            if (CFW_nvGetEdrxValue(nSim) != edrxValue)
            {
                nvWriten |= 0x04;
                if (mode != 0)
                    nvWriten |= 0x80;
            }
        }

        /*if (nvWriten)
        {
            if (nvWriten & 0x01)
            {
                CFW_nvSetEdrxEnable(mode);
                if ((ucGetEnable == 3) && (mode == 0) || (ucGetEnable == 2) && (mode == 1))
                    nvWriten &= ~0x80;
            }
            if (nvWriten & 0x02)
                CFW_nvSetEdrxPtw(edrxPtw);
            if (nvWriten & 0x04)
                CFW_nvSetEdrxValue(edrxValue);
          //  nvmWriteStatic();
          //  if (nvWriten & 0x80)
             //   CFW_NvParasChangedNotification(NV_CHANGED_EDRX);
        }*/
        CFW_GprsSetEdrx(mode, 0, edrxValue, nSim);
        CFW_nvSetEdrxPtw(edrxPtw, nSim);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        if (ucGetEnable == 2)
            ucGetEnable = 1;
        else if (ucGetEnable == 3)
            ucGetEnable = 0;

        memset(AtRet, 0, sizeof(AtRet));
        sprintf((char *)AtRet, "+CFGEDRX: %d, ptw=%d, edrxVal=%d", ucGetEnable, CFW_nvGetEdrxPtw(nSim), CFW_nvGetEdrxValue(nSim));
        atCmdRespInfoText(cmd->engine, (const char *)AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        memset(AtRet, 0, sizeof(AtRet));
        sprintf((char *)AtRet, "+CFGEDRX: enable=[0-1], edrxPtw=[0-15], edrxValue=[0-15]");
        atCmdRespInfoText(cmd->engine, (const char *)AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

#endif

uint8_t cpsm_mode = 0xff;
void atCmdHandleCPSM(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t nvWriten = 0;
        if (1 != cmd->param_count)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        cpsm_mode = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        OSI_LOGI(0x10004f01, "cpsm mode %d, paramCount %d", cpsm_mode, cmd->param_count);

        if (0 == cpsm_mode)
        {
            if (1 == nbiot_nvGetPsmEnable())
            {
                nbiot_nvSetPsmEnable(0);
                nvWriten |= 0x81;
            }
            /*at+nvsetpm=0*/
            if (0 != nbiot_nvGetPM())
            {
                nbiot_nvSetPM(0);
                nvWriten |= 0x01;
            }
            if (0xff != gATCurrentForcedLpCtrlMode)
            {
                gATCurrentForcedLpCtrlMode = 0xff; // not forced
                nvWriten |= 0x02;
            }
        }
        else
        {
            if (1 != nbiot_nvGetPsmEnable())
            {
                /*at+cpsms=1,  using current T3324&T3412 to trigger TAU*/
                nbiot_nvSetPsmEnable(1);
                nvWriten |= 0x81;
            }
            if (1 == cpsm_mode)
            {
                /*at+nvsetpm=2*/
                if (2 != nbiot_nvGetPM())
                {
                    nvWriten |= 0x01;
                    nbiot_nvSetPM(2);
                }
            }
            else
            {
                /*at+nvsetpm=10*/
                if (10 != nbiot_nvGetPM())
                {
                    nvWriten |= 0x01;
                    nbiot_nvSetPM(10);
                }
            }
            if (AT_LP_CTRL_BY_RX != gATCurrentForcedLpCtrlMode)
            {
                gATCurrentForcedLpCtrlMode = AT_LP_CTRL_BY_RX;
                nvWriten |= 0x02;
            }
        }
        if (nvWriten != 0)
        {
            if (nvWriten & 0x01)
                nvmWriteStatic();
            if (nvWriten & 0x02)
            {
                at_LpCtrlForcedModeChanged();
                at_CfgStoreAutoSaveSetting();
            }
            if (nvWriten & 0x80) //trigger TAU to negotiate PSM timer
                CFW_NvParasChangedNotification(NV_CHANGED_PSM);
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        //uint8_t mode = 0;
        uint8_t read_cpsm[10];
        if (cpsm_mode == 0xFF)
        {
            if ((nbiot_nvGetPsmEnable() == 1) && (nbiot_nvGetPM() == 2) && (gATCurrentForcedLpCtrlMode == AT_LP_CTRL_BY_RX))
            {
                sprintf(read_cpsm, "+CPSM: 1");
            }
            else if ((nbiot_nvGetPsmEnable() == 1) && (nbiot_nvGetPM() == 10) && (gATCurrentForcedLpCtrlMode == AT_LP_CTRL_BY_RX))
            {
                sprintf(read_cpsm, "+CPSM: 2");
            }
            else
            {
                sprintf(read_cpsm, "+CPSM: 0");
            }
        }
        else
        {
            sprintf(read_cpsm, "+CPSM: %d", cpsm_mode);
        }

        atCmdRespInfoText(cmd->engine, read_cpsm);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        uint8_t *testRsp = "+CPSM: 0(disable pm), 1(try to enter pm3), 2(try to enter pm2)";
        atCmdRespInfoText(cmd->engine, testRsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
#endif
}

void atCmdHandleCFGHCCP(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    uint32_t errCode = CMD_FUNC_SUCC;
    uint32_t ucByte = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t AtRet[200] = {0}; /* max 20 charactors per cid */

    OSI_LOGI(0x1000432a, "AT+CFGHCCP: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        OSI_LOGI(0x1000432b, "AT+CFGHCCP: parameters is NULL");
        errCode = ERR_AT_CME_PARAM_INVALID;
        goto cfghccp_fail;
    }

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        // check paracount
        if (0 == cmd->param_count || 3 < cmd->param_count)
        {
            OSI_LOGI(0x1000432c, "AT+CFGHCCP: Parameter count error. count = %d.", cmd->param_count);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfghccp_fail;
        }

        //enable
        if (cmd->param_count >= 1)
        {
            ucByte = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x1000432d, "AT+CFGHCCP: get para enable fail.");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfghccp_fail;
            }
            nbiot_nvSetHccpEnable(ucByte);
            OSI_LOGI(0x1000432e, "AT+CFGHCCP: get para hccpEnable=%d successfully", ucByte);
        }

        //if (nbiot_nvGetHccpEnable() == 1)
        //{
        //profile
        if (cmd->param_count >= 2)
        {
            uint32_t profile_list[] = {0, 1, 4, 5, 8, 9, 12, 13}; //0000, 0001, 0100, 0101, 1000, 1001, 1100, 1101
            ucByte = atParamDefUintInList(cmd->params[1], nbiot_nvGetHccpProfile(), profile_list, 8, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x1000432f, "AT+CFGHCCP: get para profile fail.");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfghccp_fail;
            }
            nbiot_nvSetHccpProfile(ucByte);
            OSI_LOGI(0x10004331, "AT+CFGHCCP: get para profile=0x%02x successfully", ucByte);
        }

        //maxcid
        if (cmd->param_count >= 3)
        {
            ucByte = atParamDefUintInRange(cmd->params[2], nbiot_nvGetHccpMaxcid(), 1, 16383, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x10004332, "AT+CFGHCCP: get para maxcid fail.");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfghccp_fail;
            }
            nbiot_nvSetHccpMaxcid(ucByte);
            OSI_LOGI(0x10004333, "AT+CFGHCCP: get para maxcid=%d successfully", ucByte);
        }
        //}

        nvmWriteStatic();
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+CFGHCCP: %d, profile=%d, maxCid=%d",
                nbiot_nvGetHccpEnable(),
                nbiot_nvGetHccpProfile(),
                nbiot_nvGetHccpMaxcid());
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+CFGHCCP: enable=[0-1], profile=(0,1,4,5,8,9,12,13), maxcid=[1-16383]");
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto cfghccp_fail;
    }

cfghccp_fail:
    OSI_LOGI(0x10004334, "AT+CFGHCCP: errCode : %d", errCode);
    RETURN_CME_ERR(cmd->engine, errCode);
#endif
}

void atCmdHandleNASCFG(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    uint32_t errCode = CMD_FUNC_SUCC;
    uint8_t ucByte = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t AtRet[100] = {0}; /* max 20 charactors per cid */

    OSI_LOGI(0x10004335, "AT+NASCFG: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        OSI_LOGI(0x10004336, "AT+NASCFG: parameters is NULL");
        errCode = ERR_AT_CME_PARAM_INVALID;
        goto nascfg_fail;
    }

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        // check paracount
        if (0 == cmd->param_count || 3 < cmd->param_count)
        {
            OSI_LOGI(0x10004337, "AT+NASCFG: Parameter count error. count = %d.", cmd->param_count);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto nascfg_fail;
        }

        //LowPriority
        if (cmd->param_count >= 1)
        {
            ucByte = atParamDefUintInRange(cmd->params[0], 0, 0, 2, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x10004338, "AT+NASCFG: get para LowPriority fail.");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto nascfg_fail;
            }
            if (cmd->params[0]->type != AT_CMDPARAM_EMPTY)
            {
                if (ucByte == 0)
                {
                    nbiot_nvSetNasSigLowPri(0);
                    nbiot_nvSetNasSigDualPri(0);
                }
                else if (ucByte == 1)
                {
                    nbiot_nvSetNasSigLowPri(1);
                    nbiot_nvSetNasSigDualPri(0);
                }
                else if (ucByte == 2)
                {
                    nbiot_nvSetNasSigLowPri(1);
                    nbiot_nvSetNasSigDualPri(1);
                }
                OSI_LOGI(0x10004339, "AT+NASCFG: get para LowPriority=%d successfully", ucByte);
            }
        }

        //T3245
        if (cmd->param_count >= 2)
        {
            ucByte = atParamDefUintInRange(cmd->params[1], nbiot_nvGetT3245Behave(), 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x1000433a, "AT+NASCFG: get para T3245 fail.");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto nascfg_fail;
            }

            nbiot_nvSetT3245Behave(ucByte);
            OSI_LOGI(0x1000433b, "AT+NASCFG: get para T3245=%d successfully", ucByte);
        }

        //ExceptionData
        if (cmd->param_count >= 3)
        {
            ucByte = atParamDefUintInRange(cmd->params[2], nbiot_nvGetExceptionDataReporting(), 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x1000433c, "AT+NASCFG: get para ExceptionData fail.");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto nascfg_fail;
            }

            nbiot_nvSetExceptionDataReporting(ucByte);
            OSI_LOGI(0x1000433d, "AT+NASCFG: get para ExceptionData=%d successfully", ucByte);
        }

        nvmWriteStatic();
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+NASCFG: LowPriority=%d, OverrideLowPriority=%d, T3245=%d, ExceptionData=%d",
                nbiot_nvGetNasSigLowPri(),
                nbiot_nvGetNasSigDualPri(),
                nbiot_nvGetT3245Behave(),
                nbiot_nvGetExceptionDataReporting());
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+NASCFG: LowPriority=[0-2], T3245=[0-1], ExceptionData=[0-1]");
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto nascfg_fail;
    }

nascfg_fail:
    OSI_LOGI(0x1000433e, "AT+NASCFG: errCode : %d", errCode);
    RETURN_CME_ERR(cmd->engine, errCode);
#endif
}

extern uint16_t NL1cRachCELevelQuery(uint8_t *pucCE_Level);
void atCmdHandleCRCES(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    uint32_t errCode = CMD_FUNC_SUCC;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t AtRet[100] = {0}; /* max 20 charactors per cid */
    uint8_t ceLevel = 0;

    OSI_LOGI(0x1000433f, "AT+CRCES: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        OSI_LOGI(0x10004340, "AT+CRCES: parameters is NULL");
        errCode = ERR_AT_CME_PARAM_INVALID;
        goto crces_fail;
    }

    if (AT_CMD_EXE == cmd->type)
    {
        NL1cRachCELevelQuery(&ceLevel);
        if (ceLevel == 0xff)
            ceLevel = 0;
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+CRCES: 3,%d,0", ceLevel); //<AcT>=3: E-UTRAN (NB-S1 mode)
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto crces_fail;
    }

crces_fail:
    OSI_LOGI(0x10004341, "AT+CRCES: errCode : %d", errCode);
    RETURN_CME_ERR(cmd->engine, errCode);
#endif
}

extern void sdb_printVersionInfo(int8_t *strOutputVersion);
void atCmdHandleVER(atCommand_t *cmd)
{
#ifdef PORTING_ON_GOING
    uint32_t errCode = CMD_FUNC_SUCC;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t AtRet[100] = {0}; /* max 20 charactors per cid */
    INT8 strNbiotVersion[90];

    OSI_LOGI(0x10004342, "AT+VER: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        OSI_LOGI(0x10004343, "AT+VER: parameters is NULL");
        errCode = ERR_AT_CME_PARAM_INVALID;
        goto ver_fail;
    }

    if ((AT_CMD_READ == cmd->type) || (AT_CMD_TEST == cmd->type))
    {
        sdb_printVersionInfo(strNbiotVersion);
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        sprintf(AtRet, "+VER: %s", strNbiotVersion);
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto ver_fail;
    }

ver_fail:
    OSI_LOGI(0x10004344, "AT+VER: errCode : %d", errCode);
    RETURN_CME_ERR(cmd->engine, errCode);
#endif
}

// This function to process the AT+CEREG command. include set,read and test command.
//
// Set command
// +CEREG= [<n>]
// Possible response(s)
// OK
// ERROR
//
// Read command
// +CEREG?
// Possible response(s)
// +CEREG: <n>,<stat>[,<lac>,<ci>]
// +CME ERROR: <err>
// Test command
// +CEREG=?
// Possible response(s)
// +CGREG: (list of supported <n>s)
//

void atCmdHandleCEREG(atCommand_t *cmd)
{
    bool paramok = true;
    uint32_t uErrCode = 0;
    CFW_NW_STATUS_INFO sStatus;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t cereg = 0;

    if (AT_CMD_SET == cmd->type) // Set command.
    {
        if (cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        cereg = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
#else
        cereg = atParamUintInRange(cmd->params[0], 0, 5, &paramok);
#endif
        if (!paramok)
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        else
        {
            gAtSetting.sim[nSim].cereg = cereg;
            //at_CfgSetAtSettings(0);
            atCmdRespOK(cmd->engine);
        }
    }
    else if (AT_CMD_READ == cmd->type) // Read command
    {
        uErrCode = CFW_GprsGetstatus(&sStatus, nSim);
        if (uErrCode != 0)
        {
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
            return;
        }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        char rsp[64] = {0};
        uint8_t cereg_status = 0;
        if(CFW_NWGetStackRat(nSim) == CFW_RAT_LTE_ONLY)
        {
            cereg_status = quec_regstatus_rda2quec(sStatus.nStatus);
            if(gAtSetting.sim[nSim].cereg == 2)
            {
                if((1 == cereg_status || 5 == cereg_status) && (sStatus.nAreaCode[3] != 0 || sStatus.nAreaCode[4] != 0))
                {
                    sprintf(rsp, "+CEREG: %d,%d,\"%02X%02X\",\"%X%02X%02X%02X\",%d",
                        gAtSetting.sim[nSim].cereg, cereg_status, 
                        sStatus.nAreaCode[3],sStatus.nAreaCode[4], 
                        sStatus.nCellId[0], sStatus.nCellId[1], sStatus.nCellId[2], sStatus.nCellId[3], 
                        AT_Gprs_Mapping_Act_From_PsType(sStatus.PsType));
                    
                    atCmdRespInfoText(cmd->engine, rsp);
                    atCmdRespOK(cmd->engine);
                    return;
                }
            }
        }
        sprintf(rsp, "+CEREG: %d,%d", gAtSetting.sim[nSim].cereg, cereg_status);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
#else
        _ceregRespond(&sStatus, sStatus.nStatus, true, cmd);
        atCmdRespOK(cmd->engine);
#endif
    }
    else if (AT_CMD_TEST == cmd->type)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
        atCmdRespInfoText(cmd->engine, "+CEREG: (0-2)");
#else
        atCmdRespInfoText(cmd->engine, "+CEREG: (0-5)");
#endif
        atCmdRespOK(cmd->engine);
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
    else if (AT_CMD_EXE == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
#endif
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

uint8_t AT_Gprs_Mapping_Act_From_PsType(uint8_t pstype)
{
    switch (pstype)
    {
    case 4:
    case 5:
    case 6:
        return 7;
    case 1:
        return 0;
    case 2:
        return 3;
    default:
        return 0;
    }
}

void Cereg_UtoBinString(uint8_t *string, uint8_t value)
{
    uint8_t i = 0;
    while (i < 8)
    {
        string[7 - i] = ((value >> i) & 0x01) + '0';
        i++;
    }
}

void *Cereg_MemCopy8(void *dest, const void *src, uint32_t count)
{
    char *tmp = (char *)dest, *s = (char *)src;

    while (count--)
        *tmp++ = *s++;

    return dest;
}

uint16_t Cereg_MemCompare(const void *buf1, const void *buf2, uint16_t count)
{
    const char *su1, *su2;
    int res = 0;

    for (su1 = buf1, su2 = buf2; 0 < count; ++su1, ++su2, count--)
        if ((res = *su1 - *su2) != 0)
            break;
    return (uint16_t)res;
}

uint8_t _ceregRespond(CFW_NW_STATUS_INFO *sStatus, uint8_t stat, bool reportN, atCommand_t *cmd)
{
    uint8_t ret = 1;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t nCurrRat = CFW_NWGetStackRat(nSim);
    uint8_t reg = gAtSetting.sim[nSim].cereg;
    uint8_t activeTime[8];
    uint8_t periodicTau[8];
    Cereg_UtoBinString(activeTime, sStatus->activeTime);
    Cereg_UtoBinString(periodicTau, sStatus->periodicTau);
    bool nwInfoChange = false;
    char *rsp = (char *)malloc(64);
    atMemFreeLater(rsp);
    char *respond = rsp;
    if (reportN == true)
    {
        respond += sprintf(respond, "+CEREG: %d,", gAtSetting.sim[nSim].cereg);
        if ((gAtSetting.sim[nSim].cereg == 4) || (gAtSetting.sim[nSim].cereg == 5))
        {
            reg = 5;
        }
        else if (gAtSetting.sim[nSim].cereg == 1)
        {
            reg = 1;
        }
        else
        {
            reg = 3;
        }
    }
    else
    {
        respond += sprintf(respond, "+CEREG: ");
    }
    switch (reg)
    {
    case 0:
        ret = 0;
        break;

    case 1:
        if (stat != lastNbPsInfo.nStatus)
        {
            nwInfoChange = true;
        }
        if ((reportN == true) || ((reportN == false) && (nwInfoChange == true)))
            respond += sprintf(respond, "%d", stat);
        else
            ret = 0;
        break;

    case 2:
    CeregLabel2:
        if ((stat != lastNbPsInfo.nStatus) || !Cereg_MemCompare(lastNbPsInfo.nAreaCode, sStatus->nAreaCode, 2) || !Cereg_MemCompare(lastNbPsInfo.nCellId, sStatus->nCellId, 4))
        {
            nwInfoChange = true;
        }
        if ((reportN == true) || ((reportN == false) && (nwInfoChange == true)))
        {
            respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\",%d",
                               stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                               sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3], Mapping_Creg_From_PsType(nCurrRat));
        }
        else
            ret = 0;
        break;

    case 3:
    CeregLabel3:
        if ((stat != lastNbPsInfo.nStatus) || !Cereg_MemCompare(lastNbPsInfo.nAreaCode, sStatus->nAreaCode, 2) || !Cereg_MemCompare(lastNbPsInfo.nCellId, sStatus->nCellId, 4) ||
            (lastNbPsInfo.cause_type != sStatus->cause_type) || (lastNbPsInfo.reject_cause != sStatus->reject_cause))
        {
            nwInfoChange = true;
        }
        if ((stat == 0) || (stat == 3) || (stat == 4))
        {

            if ((reportN == true) || ((reportN == false) && (nwInfoChange == true)))
            {
                respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\",%d,%d,%d",
                                   stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                                   sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                                   Mapping_Creg_From_PsType(nCurrRat),
                                   sStatus->cause_type, sStatus->reject_cause);
            }
            else
                ret = 0;
        }
        else
        {
            goto CeregLabel2;
        }
        break;

    case 4:
    CeregLabel4:
        if ((stat != lastNbPsInfo.nStatus) || !Cereg_MemCompare(lastNbPsInfo.nAreaCode, sStatus->nAreaCode, 2) || !Cereg_MemCompare(lastNbPsInfo.nCellId, sStatus->nCellId, 4) ||
            (lastNbPsInfo.activeTime != sStatus->activeTime) || (lastNbPsInfo.periodicTau != sStatus->periodicTau))
        {
            nwInfoChange = true;
        }
        if ((sStatus->activeTime != 0xff) && (sStatus->periodicTau != 0xff))
        {

            if ((reportN == true) || ((reportN == false) && (nwInfoChange == true)))
            {
                respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,,,\"%c%c%c%c%c%c%c%c\",\"%c%c%c%c%c%c%c%c\"",
                                   stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                                   sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                                   Mapping_Creg_From_PsType(nCurrRat),
                                   activeTime[0], activeTime[1], activeTime[2], activeTime[3],
                                   activeTime[4], activeTime[5], activeTime[6], activeTime[7],
                                   periodicTau[0], periodicTau[1], periodicTau[2], periodicTau[3],
                                   periodicTau[4], periodicTau[5], periodicTau[6], periodicTau[7]);
            }
            else
                ret = 0;
        }
        else if (sStatus->activeTime != 0xff)
        {
            if ((reportN == true) || ((reportN == false) && (nwInfoChange == true)))
            {
                respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\",%d,,,\"%c%c%c%c%c%c%c%c\"",
                                   stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                                   sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                                   Mapping_Creg_From_PsType(nCurrRat),
                                   activeTime[0], activeTime[1], activeTime[2], activeTime[3],
                                   activeTime[4], activeTime[5], activeTime[6], activeTime[7]);
            }
            else
                ret = 0;
        }
        else if (sStatus->periodicTau != 0xff)
        {
            if ((reportN == true) || ((reportN == false) && (nwInfoChange == true)))
            {
                respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,,,,\"%c%c%c%c%c%c%c%c\"",
                                   stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                                   sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                                   Mapping_Creg_From_PsType(nCurrRat),
                                   periodicTau[0], periodicTau[1], periodicTau[2], periodicTau[3],
                                   periodicTau[4], periodicTau[5], periodicTau[6], periodicTau[7]);
            }
            else
                ret = 0;
        }
        else
        {
            goto CeregLabel2;
        }
        break;

    case 5:
        if ((stat != lastNbPsInfo.nStatus) || !Cereg_MemCompare(lastNbPsInfo.nAreaCode, sStatus->nAreaCode, 2) || !Cereg_MemCompare(lastNbPsInfo.nCellId, sStatus->nCellId, 4) ||
            (lastNbPsInfo.activeTime != sStatus->activeTime) || (lastNbPsInfo.periodicTau != sStatus->periodicTau) ||
            (lastNbPsInfo.cause_type != sStatus->cause_type) || (lastNbPsInfo.reject_cause != sStatus->reject_cause))
        {
            nwInfoChange = true;
        }
        if ((stat == 0) || (stat == 3) || (stat == 4))
        {
            if ((sStatus->activeTime != 0xff) && (sStatus->periodicTau != 0xff))
            {
                if ((reportN == true) || ((reportN == false) && (nwInfoChange == true)))
                {
                    respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,%d,%d,\"%c%c%c%c%c%c%c%c\",\"%c%c%c%c%c%c%c%c\"",
                                       stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                                       sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                                       Mapping_Creg_From_PsType(nCurrRat),
                                       sStatus->cause_type, sStatus->reject_cause,
                                       activeTime[0], activeTime[1], activeTime[2], activeTime[3],
                                       activeTime[4], activeTime[5], activeTime[6], activeTime[7],
                                       periodicTau[0], periodicTau[1], periodicTau[2], periodicTau[3],
                                       periodicTau[4], periodicTau[5], periodicTau[6], periodicTau[7]);
                }
                else
                    ret = 0;
            }
            else if (sStatus->activeTime != 0xff)
            {
                if ((reportN == true) || ((reportN == false) && (nwInfoChange == true)))
                {
                    respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,%d,%d,\"%c%c%c%c%c%c%c%c\"",
                                       stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                                       sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                                       Mapping_Creg_From_PsType(nCurrRat),
                                       sStatus->cause_type, sStatus->reject_cause,
                                       activeTime[0], activeTime[1], activeTime[2], activeTime[3],
                                       activeTime[4], activeTime[5], activeTime[6], activeTime[7]);
                }
            }
            else if (sStatus->periodicTau != 0xff)
            {
                if ((reportN == true) || ((reportN == false) && (nwInfoChange == true)))
                {
                    respond += sprintf(respond, "%d,\"%02x%02x\",\"%02x%02x%02x%02x\", %d,%d,%d,,\"%c%c%c%c%c%c%c%c\"",
                                       stat, sStatus->nAreaCode[3], sStatus->nAreaCode[4],
                                       sStatus->nCellId[0], sStatus->nCellId[1], sStatus->nCellId[2], sStatus->nCellId[3],
                                       Mapping_Creg_From_PsType(nCurrRat),
                                       sStatus->cause_type, sStatus->reject_cause,
                                       periodicTau[0], periodicTau[1], periodicTau[2], periodicTau[3],
                                       periodicTau[4], periodicTau[5], periodicTau[6], periodicTau[7]);
                }
                else
                    ret = 0;
            }
            else
            {
                goto CeregLabel3;
            }
        }
        else
        {
            goto CeregLabel4;
        }
        break;

    default:
        ret = 2;
        break;
    }
    atCmdRespInfoText(cmd->engine, rsp);
    Cereg_MemCopy8(&lastNbPsInfo, sStatus, sizeof(CFW_NW_STATUS_INFO));
    return ret;
}
#endif //LTE_NBIOT_SUPPORT

void atCmdHandleQGPCLASS(atCommand_t *cmd)
{
    if (NULL == cmd)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

    switch (cmd->type)
    {
    case AT_CMD_SET:
    case AT_CMD_EXE:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

    case AT_CMD_READ: // Read command
    case AT_CMD_TEST: // Test command
    {
        atCmdRespInfoText(cmd->engine, "+QGPCLASS: 12");
        RETURN_OK(cmd->engine);
    }
    }
}

#ifdef CONFIG_SOC_6760

void atCmdHandleSIGMOD(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;

        OSI_LOGI(0, "atCmdHandleSIGMOD in");
        uint8_t mode = atParamUint(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        Cfw_SetNl1cSigMod(mode);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

#define DOWNLINK_PARAM_NUM 9
#define UPLINK_PARAM_NUM 14
void atCmdHandleDLPARAM(atCommand_t *cmd)
{
    bool paramok = true;
    uint16_t param[DOWNLINK_PARAM_NUM];
    uint8_t i;

    OSI_LOGI(0, "atCmdHandleDLPARAM in");
    if (AT_CMD_SET == cmd->type)
    {
        for (i = 0; i < DOWNLINK_PARAM_NUM; i++)
        {
            param[i] = atParamUint(cmd->params[i], &paramok);
            if (!paramok)
                break;
        }

        if (!paramok || cmd->param_count > DOWNLINK_PARAM_NUM)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        Cfw_SetNl1cDlParas(param);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleDLRCV(atCommand_t *cmd)
{
    bool paramok = true;

    OSI_LOGI(0, "atCmdHandleDLRCV in");
    if (AT_CMD_SET == cmd->type)
    {
        uint8_t param = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        Cfw_SetNl1cDlRcv(param);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleULPARAM(atCommand_t *cmd)
{
    bool paramok = true;
    uint16_t param[UPLINK_PARAM_NUM];
    uint8_t i;

    OSI_LOGI(0, "atCmdHandleULPARAM in");
    if (AT_CMD_SET == cmd->type)
    {
        for (i = 0; i < UPLINK_PARAM_NUM; i++)
        {
            param[i] = atParamUint(cmd->params[i], &paramok);
            if (!paramok)
                break;
        }

        if (!paramok || cmd->param_count > UPLINK_PARAM_NUM)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        Cfw_SetNl1cUlParas(param);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleULRCV(atCommand_t *cmd)
{
    bool paramok = true;

    OSI_LOGI(0, "atCmdHandleULRCV in");
    if (AT_CMD_SET == cmd->type)
    {
        uint8_t param = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        Cfw_SetNl1cUlRcv(param);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleAPC(atCommand_t *cmd)
{
    bool paramok = true;

    OSI_LOGI(0, "atCmdHandleAPC in");
    if (AT_CMD_SET == cmd->type)
    {
        uint8_t flag = atParamUint(cmd->params[0], &paramok);
        int16_t val = atParamInt(cmd->params[1], &paramok);
        if (!paramok || cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        Cfw_SetNl1cAPC(flag, val);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleAGC(atCommand_t *cmd)
{
    bool paramok = true;

    OSI_LOGI(0, "atCmdHandleAGC in");
    if (AT_CMD_SET == cmd->type)
    {
        uint8_t flag = atParamUint(cmd->params[0], &paramok);
        int16_t val = atParamInt(cmd->params[1], &paramok);
        if (!paramok || cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        Cfw_SetNl1cAGC(flag, val);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}
#endif

#ifdef CONFIG_SOC_8910
SPLTEDUMMYPARA_T gSpLteDummyPara = {
    0,
};
static uint32_t _lteDummyPara(atCommand_t *cmd)
{
    char pOutStr[200] = {
        0x00,
    };

    bool paramok = true;
    if (cmd->param_count >= 2)
    {
        uint8_t nFunId = atParamUintInRange(cmd->params[0], 0, 39, &paramok);
        uint8_t nValue = atParamDefUint(cmd->params[1], 0, &paramok);
        if (!paramok)
        {
            return ERR_AT_CME_PARAM_INVALID;
        }

        uint8_t rsrp_flag = 0;
        uint8_t rsrp = 0;
        uint8_t rsrq_flag = 0;
        uint8_t rsrq = 0;
        uint16_t band = 0;
        uint8_t nArfcn = 0;
        uint16_t arfcn[8] = {
            0,
        };
        uint32_t lteArfcn[8] = {0};
        CFW_EmodSpLteDummyPara_t para;
        memset(&para, 0, sizeof(para));
        OSI_LOGI(0, "nFunId : %d", nFunId);

        switch (nFunId)
        {
        case 0:
        case 3:
        case 4:
        case 5:
        case 10:
        case 13:
        case 18:
        case 24:
        case 27:
        case 35:
            if (cmd->param_count != 2)
            {
                return ERR_AT_CME_PARAM_INVALID;
            }
            DUMMY_VALUE_OUT_RANGE(nValue, 0, 1);
            gSpLteDummyPara.value[nFunId] = nValue;
            break;

        case 19:
            if (cmd->param_count != 2)
            {
                return ERR_AT_CME_PARAM_INVALID;
            }
            DUMMY_VALUE_OUT_RANGE(nValue, 1, 97);
            gSpLteDummyPara.value[nFunId] = nValue;
            break;
        case 20:
            if (cmd->param_count != 2)
            {
                return ERR_AT_CME_PARAM_INVALID;
            }
            DUMMY_VALUE_OUT_RANGE(nValue, 1, 34);
            gSpLteDummyPara.value[nFunId] = nValue;
            break;
        case 23:
            if (cmd->param_count != 2)
            {
                return ERR_AT_CME_PARAM_INVALID;
            }
            DUMMY_VALUE_OUT_RANGE(nValue, 1, 63);
            gSpLteDummyPara.value[nFunId] = nValue;
            break;
        // AT+ SPLTEDUMMYPARA=<Index>,<rsrp_flg>,<rsrp>,<rsrq_flg>,<rsrq>
        case 25:
        case 26:
            if (cmd->param_count == 5)
            {
                rsrp_flag = atParamDefIntInRange(cmd->params[1], 0, 0, 1, &paramok);
                rsrp = atParamDefIntInRange(cmd->params[2], 0, 0, 114, &paramok);
                rsrq_flag = atParamDefIntInRange(cmd->params[3], 0, 0, 1, &paramok);
                rsrq = atParamDefIntInRange(cmd->params[4], 0, 0, 76, &paramok);
                if (!paramok)
                {
                    return ERR_AT_CME_PARAM_INVALID;
                }

                if (nFunId == 25)
                {
                    gSpLteDummyPara.rsrp25 = rsrp;
                    gSpLteDummyPara.rsrp_flag25 = rsrp_flag;
                    gSpLteDummyPara.rsrq25 = rsrq;
                    gSpLteDummyPara.rsrq_flag25 = rsrq_flag;
                }
                else
                {
                    gSpLteDummyPara.rsrp26 = rsrp;
                    gSpLteDummyPara.rsrp_flag26 = rsrp_flag;
                    gSpLteDummyPara.rsrq26 = rsrq;
                    gSpLteDummyPara.rsrq_flag26 = rsrq_flag;
                }
            }
            else
            {
                return ERR_AT_CME_PARAM_INVALID;
            }
            break;
        // AT+SPLTEDUMMYPARA=<atFuncIndex>,<enable>,<gsmband>,<num>,<arfcn1>,<arfcn2>,<arfcn3>
        case 31:
            DUMMY_VALUE_OUT_RANGE(nValue, 0, 1);
            if (cmd->param_count >= 5 && cmd->param_count <= 7)
            {
                band = atParamDefUint(cmd->params[2], 1800, &paramok);
                nArfcn = atParamDefIntInRange(cmd->params[3], 1, 1, 3, &paramok);

                if (!paramok || (band != 1800 && band != 1900))
                {
                    return ERR_AT_CME_PARAM_INVALID;
                }

                arfcn[0] = atParamDefIntInRange(cmd->params[4], 0, 0, 1023, &paramok);
                if (!paramok || (cmd->param_count == 5 && nArfcn != 1) || (cmd->param_count != 5 && nArfcn == 1))
                {
                    return ERR_AT_CME_PARAM_INVALID;
                }
                else if (cmd->param_count != 5 && nArfcn != 1) // parameter count is equal to or more than 5, and nArfcn is equal to or more than 2
                {
                    if (cmd->param_count == 6 && nArfcn == 2)
                    {
                        arfcn[1] = atParamIntInRange(cmd->params[5], 0, 1023, &paramok);
                        if (!paramok)
                        {
                            return ERR_AT_CME_PARAM_INVALID;
                        }
                    }
                    else if (cmd->param_count == 7 && nArfcn == 3)
                    {
                        arfcn[1] = atParamIntInRange(cmd->params[5], 0, 1023, &paramok);
                        arfcn[2] = atParamIntInRange(cmd->params[6], 0, 1023, &paramok);
                        if (!paramok)
                        {
                            return ERR_AT_CME_PARAM_INVALID;
                        }
                    }
                    else
                    {
                        return ERR_AT_CME_PARAM_INVALID;
                    }
                }

                OSI_LOGI(0, "case 31 band :%d,nArfcn:%d,arfcn:%d,%d,%d", band, nArfcn, arfcn[0], arfcn[1], arfcn[2]);
                gSpLteDummyPara.value[31] = nValue;
                gSpLteDummyPara.band31 = band;
                if (band == 1800)
                {
                    band = 0;
                }
                else if (band == 1900)
                {
                    band = 1;
                }
                gSpLteDummyPara.nArfcn31 = nArfcn;
                memcpy(gSpLteDummyPara.arfcn31, arfcn, sizeof(gSpLteDummyPara.arfcn31));
            }
            else
            {
                return ERR_AT_CME_PARAM_INVALID;
            }
            break;
        // AT+SPLTEDUMMYPARA=<atFuncIndex>,<enable>,<gsmband>,<num>,<arfcn1>,<arfcn2>,<arfcn3>
        case 32:
            DUMMY_VALUE_OUT_RANGE(nValue, 0, 1);
            if (cmd->param_count >= 5 && cmd->param_count <= 7)
            {
                band = atParamDefUint(cmd->params[2], 1800, &paramok);
                nArfcn = atParamDefIntInRange(cmd->params[3], 1, 1, 3, &paramok);

                if (!paramok || (band != 1800 && band != 1900))
                {
                    return ERR_AT_CME_PARAM_INVALID;
                }

                arfcn[0] = atParamDefIntInRange(cmd->params[4], 0, 0, 1023, &paramok);
                if (!paramok || (cmd->param_count == 5 && nArfcn != 1) || (cmd->param_count != 5 && nArfcn == 1))
                {
                    return ERR_AT_CME_PARAM_INVALID;
                }
                else if (cmd->param_count != 5 && nArfcn != 1) // parameter count is equal to or more than 5, and nArfcn is equal to or more than 2
                {
                    if (cmd->param_count == 6 && nArfcn == 2)
                    {
                        arfcn[1] = atParamIntInRange(cmd->params[5], 0, 1023, &paramok);
                        if (!paramok)
                        {
                            return ERR_AT_CME_PARAM_INVALID;
                        }
                    }
                    else if (cmd->param_count == 7 && nArfcn == 3)
                    {
                        arfcn[1] = atParamIntInRange(cmd->params[5], 0, 1023, &paramok);
                        arfcn[2] = atParamIntInRange(cmd->params[6], 0, 1023, &paramok);
                        if (!paramok)
                        {
                            return ERR_AT_CME_PARAM_INVALID;
                        }
                    }
                    else
                    {
                        return ERR_AT_CME_PARAM_INVALID;
                    }
                }

                OSI_LOGI(0, "case 32 band :%d,nArfcn:%d,arfcn:%d,%d,%d", band, nArfcn, arfcn[0], arfcn[1], arfcn[2]);
                gSpLteDummyPara.value[nFunId] = nValue;
                gSpLteDummyPara.band32 = band;
                if (band == 1800)
                {
                    band = 0;
                }
                else if (band == 1900)
                {
                    band = 1;
                }
                gSpLteDummyPara.nArfcn32 = nArfcn;
                memcpy(gSpLteDummyPara.arfcn32, arfcn, sizeof(gSpLteDummyPara.arfcn32));
            }
            else
            {
                return ERR_AT_CME_PARAM_INVALID;
            }
            break;
        default:
            return ERR_AT_CME_PARAM_INVALID;
        }

        para.atFuncIndex = nFunId;
        para.value = nValue;
        para.gsmBand = band;
        memcpy(para.gsmArfcn, arfcn, sizeof(para.gsmArfcn));
        para.gsmNum = nArfcn;
        memcpy(para.lteArfcn, lteArfcn, sizeof(para.lteArfcn));
        para.lteNum = nArfcn;
        para.rsrp = rsrp;
        para.rsrp_flag = rsrp_flag;
        para.rsrq = rsrq;
        para.rsrq_flag = rsrq_flag;

        OSI_LOGI(0, "para.atFuncIndex:%d", para.atFuncIndex);
        OSI_LOGI(0, "para.value:%d", para.value);
        OSI_LOGI(0, "para.rsrp_flag:%d", para.rsrp_flag);
        OSI_LOGI(0, "para.rsrp:%d", para.rsrp);
        OSI_LOGI(0, "para.rsrq_flag:%d", para.rsrq_flag);
        OSI_LOGI(0, "para.rsrq :%d", para.rsrq);
        OSI_LOGI(0, "para.gsmBand:%d", para.gsmBand);
        OSI_LOGI(0, "para.gsmNum:%d", para.gsmNum);
        OSI_LOGI(0, "para.gsmArfcn:%d,%d,%d", para.gsmArfcn[0], para.gsmArfcn[1], para.gsmArfcn[2]);
        OSI_LOGI(0, "para.lteNum:%d", para.lteNum);
        OSI_LOGI(0, "para.lteArfcn:%d,%d,%d,%d,%d,%d,%d,%d", para.lteArfcn[0], para.lteArfcn[1], para.lteArfcn[2], para.lteArfcn[3], para.lteArfcn[4], para.lteArfcn[5], para.lteArfcn[6], para.lteArfcn[7]);

        CFW_EmodSpLteDummyPara(para, atCmdGetSim(cmd->engine));
    }
    else if (cmd->param_count == 1)
    {
        uint8_t nFunId = atParamDefIntInRange(cmd->params[0], 0, 0, 39, &paramok);
        if (!paramok)
        {
            return ERR_AT_CME_PARAM_INVALID;
        }

        uint8_t nValue;
        uint8_t rsrp_flag;
        uint8_t rsrp;
        uint8_t rsrq_flag;
        uint8_t rsrq;
        uint16_t band;
        uint8_t nArfcn;
        uint16_t arfcn[3] = {
            0,
        };

        switch (nFunId)
        {
        case 0:
        case 3:
        case 4:
        case 5:
        case 10:
        case 13:
        case 18:
        case 19:
        case 20:
        case 23:
        case 24:
        case 27:
        case 35:
            nValue = gSpLteDummyPara.value[nFunId];
            sprintf((char *)pOutStr, "%s:%d,%d",
                    "+SPLTEDUMMYPARA", nFunId, nValue);
            break;
        case 25:
            rsrp_flag = gSpLteDummyPara.rsrp_flag25;
            rsrp = gSpLteDummyPara.rsrp25;
            rsrq_flag = gSpLteDummyPara.rsrq_flag25;
            rsrq = gSpLteDummyPara.rsrq25;
            sprintf((char *)pOutStr, "%s:%d,%d,%d,%d,%d",
                    "+SPLTEDUMMYPARA", nFunId, rsrp_flag, rsrp, rsrq_flag, rsrq);
            break;

        case 26:
            rsrp_flag = gSpLteDummyPara.rsrp_flag26;
            rsrp = gSpLteDummyPara.rsrp26;
            rsrq_flag = gSpLteDummyPara.rsrq_flag26;
            rsrq = gSpLteDummyPara.rsrq26;
            sprintf((char *)pOutStr, "%s:%d,%d,%d,%d,%d",
                    "+SPLTEDUMMYPARA", nFunId, rsrp_flag, rsrp, rsrq_flag, rsrq);
            break;
        case 31:
            nValue = gSpLteDummyPara.value[31];
            band = gSpLteDummyPara.band31;
            nArfcn = gSpLteDummyPara.nArfcn31;
            memcpy(arfcn, gSpLteDummyPara.arfcn31, sizeof(gSpLteDummyPara.arfcn31));
            sprintf((char *)pOutStr, "%s:%d,%d,%d,%d,%d,%d,%d",
                    "+SPLTEDUMMYPARA", nFunId, nValue, band, nArfcn, arfcn[0], arfcn[1], arfcn[2]);
            break;
        case 32:
            nValue = gSpLteDummyPara.value[32];
            band = gSpLteDummyPara.band32;
            nArfcn = gSpLteDummyPara.nArfcn32;
            memcpy(arfcn, gSpLteDummyPara.arfcn32, sizeof(gSpLteDummyPara.arfcn32));
            sprintf((char *)pOutStr, "%s:%d,%d,%d,%d,%d,%d,%d",
                    "+SPLTEDUMMYPARA", nFunId, nValue, band, nArfcn, arfcn[0], arfcn[1], arfcn[2]);
            break;
        default:
            return ERR_AT_CME_PARAM_INVALID;
        }
        atCmdRespInfoText(cmd->engine, pOutStr);
    }
    else
    {
        return ERR_AT_CME_PARAM_INVALID;
    }
    return ERR_SUCCESS;
}

void atCmdHandleSPLTEDUMMYPARA(atCommand_t *cmd)
{
    char pOutStr[200] = {0x00};

    if (cmd->type == AT_CMD_SET)
    {
        // AT+SPLTEDUMMYPARA=<atFuncIndex>,<para_value>
        OSI_LOGI(0, "atCmdHandleSPLTEDUMMYPARA");

        if (ERR_SUCCESS == _lteDummyPara(cmd))
            RETURN_OK(cmd->engine);
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        sprintf((char *)pOutStr, "+SPLTEDUMMYPARA: (0,3-5,10,13,18-20,23-27,31-32,35),(),(),(),(),(),(),(),(),(),(),(),(),(),(),()");
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

static RRTPARAM_T gRrtParam = {0};
void atCmdHandleRRTMPARAM(atCommand_t *cmd)
{
    char pOutStr[200] = {0x00};

    switch (cmd->type)
    {
    case AT_CMD_SET:
        // AT+RRTMPARAM=<ParamIndex>,<enable>[,<value1>,<value2>]
        if (cmd->param_count >= 2 && cmd->param_count <= 4)
        {
            bool paramok = true;
            uint8_t nIndex = atParamUintInRange(cmd->params[0], 0, 14, &paramok);
            bool nEnable = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
            uint16_t nValue1 = 0;
            uint16_t nValue2 = 0;
            CFW_EmodRrtmParam_t para;

            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            if (!nEnable && cmd->param_count > 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            memset(&para, 0x00, sizeof(CFW_EmodRrtmParam_t));
            switch (nIndex)
            {
            case 0:
                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                gRrtParam.nEnable[nIndex] = nEnable;
                sprintf((char *)pOutStr, "%s:%d,%d",
                        "+RRTMPARAM", nIndex, nEnable);
                break;
            case 1:
                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                gRrtParam.nEnable[nIndex] = nEnable;
                sprintf((char *)pOutStr, "%s:%d,%d",
                        "+RRTMPARAM", nIndex, nEnable);
                break;
            case 2:
                if (nEnable && (cmd->param_count == 3))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 0, 1, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_2, nValue1);

                    gRrtParam.nValue1_2 = nValue1;
                }
                else if (!nEnable)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 3:
                if (nEnable && (cmd->param_count == 4))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 1, 65535, &paramok);
                    nValue2 = atParamUintInRange(cmd->params[3], 0, 503, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_2, nValue1, gRrtParam.nValue2_3, nValue2);

                    gRrtParam.nValue1_3 = nValue1;
                    gRrtParam.nValue2_3 = nValue2;
                }
                else if (nEnable == 0)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 4:
                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                gRrtParam.nEnable[nIndex] = nEnable;
                sprintf((char *)pOutStr, "%s:%d,%d",
                        "+RRTMPARAM", nIndex, nEnable);
                break;
            case 5:
                if (nEnable && (cmd->param_count == 3))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 0, 63, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_5, nValue1);

                    gRrtParam.nValue1_5 = nValue1;
                }
                else if (!nEnable)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 6:
                if (nEnable && (cmd->param_count == 3))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 0, 63, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_6, nValue1);

                    gRrtParam.nValue1_6 = nValue1;
                }
                else if (!nEnable)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 7:
                if (nEnable && (cmd->param_count == 3))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 0, 7, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_7, nValue1);

                    gRrtParam.nValue1_7 = nValue1;
                }
                else if (!nEnable)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 8:
                if (nEnable && (cmd->param_count == 3))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 0, 7, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_8, nValue1);

                    gRrtParam.nValue1_8 = nValue1;
                }
                else if (nEnable == 0)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 9:
                if (nEnable && (cmd->param_count == 3))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 0, 63, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_9, nValue1);

                    gRrtParam.nValue1_9 = nValue1;
                }
                else if (!nEnable)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 10:
                if (nEnable && (cmd->param_count == 3))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 0, 63, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_10, nValue1);

                    gRrtParam.nValue1_10 = nValue1;
                }
                else if (!nEnable)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 11:
                if (nEnable && (cmd->param_count == 3))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 0, 7, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_11, nValue1);

                    gRrtParam.nValue1_11 = nValue1;
                }
                else if (!nEnable)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 12:
                if (nEnable && (cmd->param_count == 4))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 1, 65535, &paramok);
                    nValue2 = atParamUintInRange(cmd->params[3], 0, 503, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_12, nValue1, gRrtParam.nValue2_12, nValue2);

                    gRrtParam.nValue1_12 = nValue1;
                    gRrtParam.nValue2_12 = nValue2;
                }
                else if (!nEnable)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 13:
                if (nEnable && (cmd->param_count == 3))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 0, 15, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_13, nValue1);

                    gRrtParam.nValue1_13 = nValue1;
                }
                else if (!nEnable)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            case 14:
                if (nEnable && (cmd->param_count == 3))
                {
                    nValue1 = atParamUintInRange(cmd->params[2], 0, 15, &paramok);
                    if (!paramok)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    gRrtParam.nEnable[nIndex] = nEnable;

                    sprintf((char *)pOutStr, "%s:%d,%d,%d->%d",
                            "+RRTMPARAM", nIndex, nEnable, gRrtParam.nValue1_14, nValue1);

                    gRrtParam.nValue1_14 = nValue1;
                }
                else if (!nEnable)
                {
                    gRrtParam.nEnable[nIndex] = nEnable;
                    sprintf((char *)pOutStr, "%s:%d,%d",
                            "+RRTMPARAM", nIndex, nEnable);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                break;
            default:
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            para.enable = nEnable;
            para.paramIndex = nIndex;
            para.value1 = nValue1;
            para.value2 = nValue2;
            CFW_EmodRrtmParam(para, atCmdGetSim(cmd->engine));
            atCmdRespInfoText(cmd->engine, pOutStr);
            RETURN_OK(cmd->engine);
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        break;
    case AT_CMD_TEST:
        sprintf((char *)pOutStr, "+RRTMPARAM: (0-14),(0-1),,");
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleGRRLTEFREQ(atCommand_t *cmd)
{
    char pOutStr[200] = {0x00};

    switch (cmd->type)
    {
    case AT_CMD_SET:
        // AT+GRRLTEFREQ=<earfcn>,<prior>,<thresh_h>,<thresh_l>
        if (cmd->param_count == 4)
        {
            bool paramok = true;
            CFW_EmodGrrLteFreq_t para;
            para.earfcn = atParamUintInRange(cmd->params[0], 0, 65535, &paramok);
            para.prior = atParamUintInRange(cmd->params[1], 0, 7, &paramok);
            para.thresh_h = atParamUintInRange(cmd->params[2], 0, 31, &paramok);
            para.thresh_l = atParamUintInRange(cmd->params[3], 0, 31, &paramok);

            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            CFW_EmodGrrLteFreq(para, atCmdGetSim(cmd->engine));
            RETURN_OK(cmd->engine);
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        break;
    case AT_CMD_TEST:
        sprintf((char *)pOutStr, "+GRRLTEFREQ: (0-65535),(0-7),(0-31),(0-31)");
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleL1PARAM(atCommand_t *cmd)
{
    char pOutStr[200] = {0x00};

    switch (cmd->type)
    {
    case AT_CMD_SET:
        // AT+L1PARAM=< ParamIndex>,[ParmaValue]
        if (cmd->param_count == 1 || cmd->param_count == 2)
        {
            bool paramok = true;
            CFW_EmodL1Param_t para = {0};
            uint16_t nIndex = atParamDefUintInRange(cmd->params[0], 0, 0, 9, &paramok);
            uint16_t nValue = 0;

            if (nIndex == 3)
            {
                nValue = atParamUintInRange(cmd->params[1], 0, 65535, &paramok);
            }
            else if (cmd->param_count != 1)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            para.index = nIndex;
            para.value = nValue;
            CFW_EmodL1Param(para, atCmdGetSim(cmd->engine));
            RETURN_OK(cmd->engine);
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        break;
    case AT_CMD_TEST:
        sprintf((char *)pOutStr, "+L1PARAM: (0-9),(0-65535)");
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleRRDMPARAM(atCommand_t *cmd)
{
    RETURN_OK(cmd->engine);
}

void atCmdHandleSPCLEANINFO(atCommand_t *cmd)
{
    char pOutStr[200] = {0x00};

    switch (cmd->type)
    {
    case AT_CMD_SET:
        // AT+SPCLEANINFO=<type>
        if (cmd->param_count == 1)
        {
            bool paramok = true;
            CFW_EmodSpCleanInfo_t para;

            uint8_t nType = atParamDefUintInRange(cmd->params[0], 1, 1, 7, &paramok);
            if (!paramok || (nType != 1 && nType != 4 && nType != 7))
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            memset(&para, 0x00, sizeof(CFW_EmodSpCleanInfo_t));
            para.type = nType;
            CFW_EmodSpCleanInfo(para, atCmdGetSim(cmd->engine));
            RETURN_OK(cmd->engine);
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        break;
    case AT_CMD_TEST:
        sprintf((char *)pOutStr, "+SPCLEANINFO: (1,4,7)");
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleVERCTRL(atCommand_t *cmd)
{
    uint32_t errCode = ERR_AT_CME_PARAM_INVALID;
    uint8_t ucByte = 0;
    char AtRet[200] = {0}; /* max 20 charactors per cid */

    OSI_LOGI(0, "atCmdHandleVERCTRL");

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        // check paracount
        if (cmd->param_count > 2)
        {
            OSI_LOGI(0, "AT+VERCTRL: Parameter count error. count = %d.", cmd->param_count);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto verctrl_fail;
        }

        // enable
        if (cmd->param_count >= 1)
        {
            ucByte = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto verctrl_fail;
            }
            CFW_NwSetFTA(ucByte, atCmdGetSim(cmd->engine));
        }

        if (cmd->param_count >= 2)
        {
            ucByte = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
            if (!paramok)
            {
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto verctrl_fail;
            }
            CFW_NwSetAutoAttachFlag(ucByte, atCmdGetSim(cmd->engine));
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        sprintf(AtRet, "+VERCTRL: %d, %d", CFW_NwGetFTA(atCmdGetSim(cmd->engine)), CFW_NwGetAutoAttachFlag(atCmdGetSim(cmd->engine)));
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        sprintf(AtRet, "+VERCTRL: mode=[0-1](0-storeroom; 1-gcf), pdn_auto_attach=[0-1](0-disable;1-enable)");
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto verctrl_fail;
    }
    return;
verctrl_fail:
    OSI_LOGI(0, "AT+VERCTRL: errCode : %d", errCode);
    RETURN_CME_ERR(cmd->engine, errCode);
}

void atCmdHandleCFGDFTPDN(atCommand_t *cmd)
{
    uint32_t errCode = ERR_AT_CME_PARAM_INVALID;
    uint8_t pdptype = 0;
    uint8_t nAuthProt = 0;
    const char *apn = NULL;
    const char *Username = NULL;
    const char *Password = NULL;
    char AtRet[300] = {0}; /* max 20 charactors per cid */
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);

    OSI_LOGI(0, "atCmdHandleCFGDFTPDN");
    AT_Gprs_CidInfo *g_staAtGprsCidInfo_nv = gAtSetting.g_staAtGprsCidInfo[nSim];

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        CFW_GPRS_DFTPDN_INFO nDftPdn;
        memset(&nDftPdn, 0, sizeof(CFW_GPRS_DFTPDN_INFO));
        // check paracount
        if (cmd->param_count > 5)
        {
            OSI_LOGI(0, "AT+CFGDFTPDN: Parameter count error. count = %d.", cmd->param_count);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfgdftpdn_fail;
        }

        // enable
        if (cmd->param_count >= 1)
        {
            static const uint32_t paramList[] = {0, 1, 2, 3};
            pdptype = atParamUintInList(cmd->params[0], paramList, 4, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0, "AT+CFGDFTPDN: get para pdnType fail");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }
            OSI_LOGI(0, "AT+CFGDFTPDN: get para pdnType=%d successfully", pdptype);
            if (pdptype == 0)
            {
                if (cmd->param_count == 1) //clear defult pdn
                {
                    g_staAtGprsCidInfo_nv[0].uCid = 0;
                    g_staAtGprsCidInfo_nv[0].nPdpType = 0;
                    memset(g_staAtGprsCidInfo_nv[0].pApn, 0, AT_GPRS_APN_MAX_LEN);
                    memset(g_staAtGprsCidInfo_nv[0].uaUsername, 0, AT_GPRS_USR_MAX_LEN);
                    memset(g_staAtGprsCidInfo_nv[0].uaPassword, 0, AT_GPRS_PAS_MAX_LEN);
                    atCfgAutoSave();
                    //send to stack
                    nDftPdn.nPdpType = CFW_GPRS_PDP_TYPE_IPV4V6;
                    nDftPdn.nApnSize = 0;
                    nDftPdn.nApnUserSize = 0;
                    nDftPdn.nApnPwdSize = 0;
                    CFW_GprsSendCtxCfg_V2(&nDftPdn, nSim);
                    RETURN_OK(cmd->engine);
                }
                else
                {
                    OSI_LOGI(0, "AT+CFGDFTPDN: pdptype is 0 param_count error");
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgdftpdn_fail;
                }
            }
        }

        if (cmd->param_count >= 2)
        {
            static const uint32_t paramList[] = {0, 1, 2};
            nAuthProt = atParamUintInList(cmd->params[1], paramList, 3, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0, "AT+CFGDFTPDN: get para pdnType fail");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }
            OSI_LOGI(0, "AT+CFGDFTPDN: get para nAuthProt=%d successfully", nAuthProt);
        }

        if (cmd->param_count >= 3)
        {
            apn = atParamDefStr(cmd->params[2], "", &paramok);
            if (!paramok || strlen(apn) > AT_GPRS_APN_MAX_LEN)
            {
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }
            OSI_LOGI(0, "AT+CFGDFTPDN: get para apnsize:%d  apn:%p %d %d %d %d %d ", strlen(apn), apn, *(apn), *(apn + 1), *(apn + 2), *(apn + 3), *(apn + 4));
            OSI_LOGXI(OSI_LOGPAR_S, 0x00000000, "AT+CFGDFTPDN: get para apn=%s", apn);
            if (atParamIsEmpty(cmd->params[1]))
            {
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }
            uint8_t i = 0;
            for (i = 0; i < strlen(apn); i++)
            {
                if (!((*(apn + i) >= 'A' && *(apn + i) <= 'Z') ||
                      (*(apn + i) >= 'a' && *(apn + i) <= 'z') ||
                      (*(apn + i) >= '0' && *(apn + i) <= '9') ||
                      (*(apn + i) == '.') ||
                      (*(apn + i) == '-'))) //not "A-Z"(0X41-5A) "a-z"(0X61-7A) "0-9" "-"(0X2D) "."
                {
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgdftpdn_fail;
                }
            }
        }

        if (cmd->param_count >= 4)
        {
            Username = atParamDefStr(cmd->params[3], "", &paramok);
            if (!paramok || strlen(Username) > AT_GPRS_APN_MAX_LEN)
            {
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }
            //OSI_LOGXI(OSI_LOGPAR_IS, 0, "AT+CFGDFTPDN: get para Username=%s size:%d", Username,strlen(Username));
            OSI_LOGI(0, "AT+CFGDFTPDN: get para Usernamesize:%d ", strlen(Username));
            if (atParamIsEmpty(cmd->params[2]))
            {
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }
        }
        if (cmd->param_count >= 5)
        {
            Password = atParamDefStr(cmd->params[4], "", &paramok);
            if (!paramok || strlen(Password) > AT_GPRS_APN_MAX_LEN)
            {
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }
            OSI_LOGI(0, "AT+CFGDFTPDN: get para Passwordsize:%d ", strlen(Password));
            if (atParamIsEmpty(cmd->params[3]))
            {
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }
        }
        g_staAtGprsCidInfo_nv[0].uCid = 0xFF;
        g_staAtGprsCidInfo_nv[0].nPdpType = pdptype;
        nDftPdn.nPdpType = g_staAtGprsCidInfo_nv[0].nPdpType;

        if (cmd->param_count >= 2)
        {
            g_staAtGprsCidInfo_nv[0].nAuthProt = nAuthProt;
            nDftPdn.nAuthProt = nAuthProt;
        }

        if (cmd->param_count >= 3)
        {
            g_staAtGprsCidInfo_nv[0].nApnSize = strlen(apn);
            nDftPdn.nApnSize = g_staAtGprsCidInfo_nv[0].nApnSize;
            memset(g_staAtGprsCidInfo_nv[0].pApn, 0, AT_GPRS_APN_MAX_LEN);
            if (nDftPdn.nApnSize > 0)
            {
                memcpy(g_staAtGprsCidInfo_nv[0].pApn, apn, g_staAtGprsCidInfo_nv[0].nApnSize);
                memcpy(nDftPdn.pApn, g_staAtGprsCidInfo_nv[0].pApn, nDftPdn.nApnSize);
            }
        }
        if (cmd->param_count >= 4)
        {
            g_staAtGprsCidInfo_nv[0].nUsernameSize = strlen(Username);
            nDftPdn.nApnUserSize = g_staAtGprsCidInfo_nv[0].nUsernameSize;
            memset(g_staAtGprsCidInfo_nv[0].uaUsername, 0, AT_GPRS_USR_MAX_LEN);
            if (nDftPdn.nApnUserSize > 0)
            {
                memcpy(g_staAtGprsCidInfo_nv[0].uaUsername, Username, g_staAtGprsCidInfo_nv[0].nUsernameSize);
                memcpy(nDftPdn.pApnUser, g_staAtGprsCidInfo_nv[0].uaUsername, nDftPdn.nApnUserSize);
            }
        }
        if (cmd->param_count >= 5)
        {
            g_staAtGprsCidInfo_nv[0].nPasswordSize = strlen(Password);
            nDftPdn.nApnPwdSize = g_staAtGprsCidInfo_nv[0].nPasswordSize;
            memset(g_staAtGprsCidInfo_nv[0].uaPassword, 0, AT_GPRS_PAS_MAX_LEN);
            if (nDftPdn.nApnPwdSize > 0)
            {
                memcpy(g_staAtGprsCidInfo_nv[0].uaPassword, Password, g_staAtGprsCidInfo_nv[0].nPasswordSize);
                memcpy(nDftPdn.pApnPwd, g_staAtGprsCidInfo_nv[0].uaPassword, nDftPdn.nApnPwdSize);
            }
        }
        atCfgAutoSave();
        CFW_GprsSendCtxCfg_V2(&nDftPdn, nSim);

        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        if (g_staAtGprsCidInfo_nv[0].uCid == 0xFF)
        {
            sprintf(AtRet, "+CFGDFTPDN: %d, %d, \"%s\", \"%s\", \"%s\"", g_staAtGprsCidInfo_nv[0].nPdpType, g_staAtGprsCidInfo_nv[0].nAuthProt,
                    g_staAtGprsCidInfo_nv[0].pApn, g_staAtGprsCidInfo_nv[0].uaUsername, g_staAtGprsCidInfo_nv[0].uaPassword);
            atCmdRespInfoText(cmd->engine, AtRet);
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        sprintf(AtRet, "+CFGDFTPDN: pdptype=[1,2,3], nAuthProt=[0,1,2], \"apn\", \"Username\", \"Password\"");
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        errCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto cfgdftpdn_fail;
    }
    return;
cfgdftpdn_fail:
    OSI_LOGI(0, "AT+CFGDFTPDN: errCode : %d", errCode);
    RETURN_CME_ERR(cmd->engine, errCode);
}

//Set "Discard PS Data" flag
//  TRUE: Discard ps data
//  FALSE: Do not Discard ps data
bool gGprsDPDFlag[CFW_SIM_COUNT] = {
    false,
};
void ATGprsSetDPSDFlag(bool bDPD, CFW_SIM_ID nSim)
{
    gGprsDPDFlag[nSim] = bDPD;
}
bool ATGprsGetDPSDFlag(CFW_SIM_ID nSim)
{
    return gGprsDPDFlag[nSim];
}

void atCmdHandleDiscardPSData(atCommand_t *cmd)
{
    uint16_t DiscardPSData = 0;
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);
    if (!cmd)
    {
        OSI_LOGI(0, "AT+DPSD: pParam = NULL");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        DiscardPSData = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        ATGprsSetDPSDFlag(DiscardPSData, nSim);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        const char *pTestRsp = "+DPSD: [0~1]";
        atCmdRespInfoText(cmd->engine, pTestRsp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char AtRet[20] = {
            0x00,
        };
        sprintf(AtRet, "+DPSD: %d", ATGprsGetDPSDFlag(nSim));
        atCmdRespInfoText(cmd->engine, AtRet);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "not supported!");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleSETCSPAGFLAG(atCommand_t *cmd)
{
    char urcBuffer[32] = {0};
    CFW_SIM_ID nSim = atCmdGetSim(cmd->engine);

    if (AT_CMD_SET == cmd->type)
    {
        // AT+SETCSPAGFLAG=<flag>
        bool paramok = true;

        bool nEnable = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (CFW_SetCSPagingFlag(nEnable, nSim) != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t flag = 0;
        if ((CFW_GetCSPagingFlag(&flag, nSim) != 0) || flag > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        sprintf(urcBuffer, "+SETCSPAGFLAG: %d", flag);
        atCmdRespInfoText(cmd->engine, urcBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        sprintf(urcBuffer, "+SETCSPAGFLAG: (0-1)");
        atCmdRespInfoText(cmd->engine, urcBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

#endif
