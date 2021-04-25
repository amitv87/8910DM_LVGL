#include "cfw.h"
#ifdef CFW_GPRS_SUPPORT
#include "stdio.h"
#include "osi_api.h"
#include "osi_log.h"
#include "at_cfg.h"
#include "at_response.h"
#include "at_cfw.h"
#include "at_engine.h"
#include "at_command.h"
#include "at_cmd_tcpip.h"
#include "sockets.h"
#include "netmain.h"

//following global variables just used in AT+PING command
#define ERR_SUCCESS 0
#define AT_StrLen(str) strlen((const char *)str)
#define AT_StrCpy(des, src) strcpy((char *)des, (const char *)src)
#define AT_StrNCpy(des, src, len) strncpy((char *)des, (const char *)src, (size_t)len)
#define AT_PING_MAX_HOSTLEN 256

uint16_t AT_PING_TOTAL_NUM = 5;
uint16_t AT_PING_ECHO_NUM = 0;
ip_addr_t AT_PING_IP;
uint8_t AT_PING_TIMEOUT = 5;
int16_t AT_PING_PACKET_LEN = 16;
uint8_t AT_PING_TTL = 0;
uint16_t AT_PING_ID = 0;
uint16_t AT_PING_SEQNO = 0;
SOCKET AT_PING_SOCKET = INVALID_SOCKET;
uint8_t AT_PING_CID = 0;
uint32_t AT_PING_TIME_SUM = 0;
int32_t AT_PING_MAX_DELAY = -1;
int32_t AT_PING_MIN_DELAY = 0x7fffffff;
uint8_t AT_PING_CONTINUE_TIMEOUT = 1;
osiTimer_t *gPingtimer = NULL;
osiTimer_t *gPingContinuetimer = NULL;
static uint8_t gPingReplyed = 0;
static bool gPingstatus = false;

static atCmdEngine_t *gEngine = NULL;

void ping_timeout(void *param);
void ping_continue_timeout(void *param);
void ping_continue(atCmdEngine_t *engine, char *response);
extern uint16_t inet_chksum(const void *dataptr, uint16_t len);
bool ping_socket_is_invalid();
int8_t AT_SendIP_ICMP_Ping(atCmdEngine_t *engine, uint32_t pingparam);
void ping_result_statics(atCmdEngine_t *engine, char *response);
//static int8_t sendIP_ICMP_Ping(uint32_t pingparam);

static void AT_PING_AsyncEventProcess(void *param)
{
    osiEvent_t *osiEv = (osiEvent_t *)param;
    OSI_LOGI(0, "enter AT_PING_AsyncEventProcess");
    OSI_LOGI(0x100052a9, "AT_PING_AsyncEventProcess Got %d,0x%x,0x%x,0x%x",
             osiEv->id, osiEv->param1, osiEv->param2, osiEv->param3);
    uint8_t uSocket = AT_PING_SOCKET;
    atCmdEngine_t *engine = (atCmdEngine_t *)osiEv->param3;
    if (!atCmdEngineIsValid(engine))
    {
        if (gPingtimer != NULL)
        {
            osiTimerStop(gPingtimer);
            osiTimerDelete(gPingtimer);
            gPingtimer = NULL;
        }
        if (gPingContinuetimer != NULL)
        {
            osiTimerStop(gPingContinuetimer);
            osiTimerDelete(gPingContinuetimer);
            gPingContinuetimer = NULL;
        }
        AT_PING_TIME_SUM = 0;
        AT_PING_SEQNO = 0;
        AT_PING_ECHO_NUM = 0;
        gPingstatus = false;
        gEngine = NULL;
        free(osiEv);
        return;
    }

    //int8_t nRetValue;
    switch (osiEv->id)
    {
    case EV_CFW_TCPIP_SOCKET_CONNECT_RSP:
        break;
    case EV_CFW_TCPIP_SOCKET_CLOSE_RSP:
        OSI_LOGI(0, "RECEIVED EV_CFW_TCPIP_CLOSE_IND ...");
        break;
    case EV_CFW_TCPIP_BEARER_LOSING_IND:
    {
        //char response[200] = {0};
        OSI_LOGI(0, "EV_CFW_TCPIP_BEARER_LOSING_IND coming");
        //ping_result_statics(engine, response);
        break;
    }
    case EV_CFW_TCPIP_ERR_IND:
        atCmdRespInfoText(engine, "EV_CFW_TCPIP_ERR_IND error");
        break;
    case EV_CFW_TCPIP_REV_DATA_IND:
    {
        uint16_t uDataSize = (uint16_t)osiEv->param2;
        uint8_t tmp[2];
        OSI_LOGI(0x1000400a, "EV_CFW_TCPIP_REV_DATA_IND socket=%d,len=%d", uSocket, uDataSize);
        CFW_TcpipSocketRecv(uSocket, tmp, 2, 0); // just for consumer raw pcb
    }
    break;
    case EV_CFW_TCPIP_CLOSE_IND:
        OSI_LOGI(0, "RECEIVED EV_CFW_TCPIP_CLOSE_IND ...");
        if (CFW_TcpipSocketClose(uSocket) == SOCKET_ERROR)
        {
            OSI_LOGI(0, "close socket error");
            atCmdRespInfoText(engine, "close socket error");
        }
        break;
    case EV_CFW_ICMP_DATA_IND:
    {

        char response[200] = {0};
        OSI_LOGI(0x100052a2, "pEvent->param1 = %d, pEvent->param2 = %d", osiEv->param1, osiEv->param2);

        OSI_LOGI(0x10003fc5, "AT_PING_ID = %d", AT_PING_ID);
        OSI_LOGI(0x1000400b, "AT_PING_ECHO_NUM = %d", AT_PING_ECHO_NUM);
        OSI_LOGI(0x1000400c, "AT_PING_TOTAL_NUM = %d", AT_PING_TOTAL_NUM);

        if (ping_socket_is_invalid() == true)
        {
            osiTimerStop(gPingtimer);
            //osiTimerDelete(gPingtimer);
            //gPingtimer = NULL;
            OSI_LOGI(0x1000400d, "ping socket is invalid, ignore this ping response!");
            free(osiEv);
            return;
        }
        char *pcIpAddr = ipaddr_ntoa(&AT_PING_IP);

        OSI_LOGXI(OSI_LOGPAR_S, 0x10003fc6, "AT_PING_IP = %s", pcIpAddr);
        uint16_t id = osiEv->param2 >> 16;
        uint16_t seqno = osiEv->param2 & 0xFFFF;
        uint32_t ticks = (uint32_t)osiUpTime();
        OSI_LOGI(0x1000400e, "Old Time = %d", osiEv->param1);
        OSI_LOGI(0x1000400f, "New Time = %d", ticks);
        ticks -= osiEv->param1;
        int32_t mseconds = ticks;
        if ((id != AT_PING_ID) || (seqno != AT_PING_SEQNO))
        {
            OSI_LOGI(0x10004010, "maybe old ping response AT_PING_SEQNO = %d,seqno = %d, p2_id: %d, AT_PING_ID: %d", AT_PING_SEQNO, seqno, id, AT_PING_ID);
            OSI_LOGI(0x10004011, "actually ping delay is %d ms", mseconds);
            free(osiEv);
            return;
        }
        OSI_LOGI(0, "gPingReplyed %x", gPingReplyed);
        if (gPingReplyed == 1)
        {
            free(osiEv);
            return;
        }
        gPingReplyed = 1;
        //stop timer
        osiTimerStop(gPingtimer);
        //osiTimerDelete(gPingtimer);
        //gPingtimer = NULL;

        AT_PING_TIME_SUM += mseconds;
        if (mseconds > AT_PING_MAX_DELAY)
        {
            AT_PING_MAX_DELAY = mseconds;
        }
        if (mseconds < AT_PING_MIN_DELAY)
        {
            AT_PING_MIN_DELAY = mseconds;
        }
        OSI_LOGXI(OSI_LOGPAR_SIII, 0x10004012, "Reply from %s: bytes= %d time = %d(ms), TTL = %d", pcIpAddr, AT_PING_PACKET_LEN + ((IP_IS_V4(&AT_PING_IP)) ? 20 : 40), mseconds, AT_PING_TTL);
        sprintf(response, "Reply from %s: bytes= %d time = %ld(ms), TTL = %d", pcIpAddr, AT_PING_PACKET_LEN + ((IP_IS_V4(&AT_PING_IP)) ? 20 : 40), mseconds, AT_PING_TTL);
        AT_PING_ECHO_NUM++;
        atCmdRespInfoText(engine, response);
        if (mseconds > AT_PING_CONTINUE_TIMEOUT * 1000)
        {
            ping_continue(engine, response);
        }
        else
        {
            if (false == osiTimerStart(gPingContinuetimer, (AT_PING_CONTINUE_TIMEOUT * 1000 - mseconds) + 1))
            {
                OSI_LOGI(0, "AT_PING_AsyncEventProcess,gPingContinuetimer:osiTimerStart failed!");
            }
        }
        break;
    }
    break;
    default:
        OSI_LOGI(0x10004013, "AT_NET_AsyncEventProcess NOT PROCESS THIS EVENT:%d", osiEv->id);
        break;
    }
    free(osiEv);
}
static void dnsReq_callback(void *param)
{
    osiEvent_t *ev = (osiEvent_t *)param;
    ip_addr_t *ipaddr;
    char *pcIpAddr = NULL;
    atCmdEngine_t *engine = (atCmdEngine_t *)ev->param3;
    if (ev->id == EV_CFW_DNS_RESOLV_SUC_IND)
    {
        ipaddr = (ip_addr_t *)ev->param1;
        pcIpAddr = ipaddr_ntoa(ipaddr);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004014, "IP address is %s", pcIpAddr);
        ip_addr_copy(AT_PING_IP, *ipaddr);

        if (AT_PING_PACKET_LEN != 16)
        {
            if (IP_IS_V4(&AT_PING_IP))
                AT_PING_PACKET_LEN -= 20;
            else if (IP_IS_V6(&AT_PING_IP))
                AT_PING_PACKET_LEN -= 40;
        }
        if (AT_PING_PACKET_LEN < 16)
        {
            OSI_LOGI(0x10004f0e, "AT_TCPIP_CmdFunc_PING:  dnsReq_callback AT_PING_PACKET_LEN < 16");
            free(ev);
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        }
        AT_PING_ID++;

        //create timer
        if (gPingtimer != NULL)
        {
            osiTimerDelete(gPingtimer);
            gPingtimer = NULL;
        }
        gPingtimer = osiTimerCreate(osiThreadCurrent(), ping_timeout, (void *)engine);
        if (gPingtimer == NULL)
        {
            free(ev);
            RETURN_CME_ERR(engine, ERR_AT_CME_EXE_FAIL);
        }

        if (gPingContinuetimer != NULL)
        {
            osiTimerDelete(gPingContinuetimer);
            gPingContinuetimer = NULL;
        }
        gPingContinuetimer = osiTimerCreate(osiThreadCurrent(), ping_continue_timeout, (void *)engine);
        if (gPingContinuetimer == NULL)
        {
            free(ev);
            if (gPingtimer != NULL)
            {
                osiTimerDelete(gPingtimer);
                gPingtimer = NULL;
            }
            RETURN_CME_ERR(engine, ERR_AT_CME_EXE_FAIL);
        }

        int8_t nRetValue = AT_SendIP_ICMP_Ping(engine, (uint32_t)engine);
        if (nRetValue != ERR_SUCCESS)
        {
            if (gPingtimer != NULL)
            {
                osiTimerDelete(gPingtimer);
                gPingtimer = NULL;
            }
            if (gPingContinuetimer != NULL)
            {
                osiTimerDelete(gPingContinuetimer);
                gPingContinuetimer = NULL;
            }

            OSI_LOGI(0, "dnsReq_callback: AT_SendIP_ICMP_Ping Return Failed!");
            atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            gPingstatus = true;
            atCmdRespOK(engine);
        }
    }
    else if (ev->id == EV_CFW_DNS_RESOLV_ERR_IND)
    {
        CFW_TcpipSocketClose(AT_PING_SOCKET);
        atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
    }
    free(ev);
}

void AT_TCPIP_CmdFunc_PINGSTOP(atCommand_t *pParam)
{
    char response[200] = {0};
    if (NULL == pParam)
    {
        OSI_LOGI(0x10004016, "AT+SOCKET: pParam = NULL");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
    if (AT_CMD_EXE == pParam->type)
    {
        if (atCmdEngineIsValid(gEngine) && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (gPingstatus == false)
        {
            atCmdRespOK(pParam->engine);
            return;
        }
        if (NULL != gPingtimer)
        {
            osiTimerStop(gPingtimer);
            osiTimerDelete(gPingtimer);
            gPingtimer = NULL;
        }
        if (NULL != gPingContinuetimer)
        {
            osiTimerStop(gPingContinuetimer);
            osiTimerDelete(gPingContinuetimer);
            gPingContinuetimer = NULL;
        }
        ping_result_statics(pParam->engine, response);

        gEngine = NULL;
        RETURN_OK(pParam->engine);
    }
    else
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
    return;
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_PING
* Description:
* Parameter:  AT_CMD_PARA *pParam
* Return:       void
* Remark:      n/a
* Author:
* Data:          2016-3-28
******************************************************************************/
void AT_TCPIP_CmdFunc_PING(atCommand_t *pParam)
{
    atCmdEngine_t *engine = pParam->engine;
    int32_t ret;

    if (AT_CMD_SET == pParam->type)
    {
        if (gPingstatus)
        {
            OSI_LOGI(0, " AT_TCPIP_CmdFunc_PING had already started.");
            RETURN_CME_ERR(engine, ERR_AT_CME_EXE_FAIL);
        }
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        //int32_t iResult = 0;
        uint8_t nNumParamter = pParam->param_count;

        OSI_LOGI(0x10003fe7, "AT+PING starting");
        AT_PING_TTL = 255;
        AT_PING_SEQNO = 0;
        AT_PING_TOTAL_NUM = 5;
        AT_PING_CID = 0;
        //AT_PING_ID = 0;
        AT_PING_PACKET_LEN = 16;
        AT_PING_ECHO_NUM = 0;
        AT_PING_TIMEOUT = 5;
        AT_PING_MAX_DELAY = -1;
        AT_PING_MIN_DELAY = 0x7fffffff;

        gEngine = pParam->engine;

        ip_addr_t IpAddr;
        ip_addr_set_zero(&IpAddr);

        if (NULL == pParam)
        {
            OSI_LOGI(0x10004016, "AT+SOCKET: pParam = NULL");
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        }
        uint8_t nSim = atCmdGetSim(pParam->engine);

        //uint8_t i = 0;

        bool paramok = true;
        if ((nNumParamter > 4) || (nNumParamter < 1))
        {
            OSI_LOGI(0x10004017, " AT_TCPIP_CmdFunc_PING: nNumParamter=%d\n", nNumParamter);
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        }
        char nDNS[AT_PING_MAX_HOSTLEN] = {0};

        const char *tmpStr = atParamStr(pParam->params[0], &paramok);
        if (!paramok)
        {
            OSI_LOGI(0x100052aa, "AT_TCPIP_CmdFunc_PING: param0 error\n");
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (AT_StrLen(tmpStr) > AT_PING_MAX_HOSTLEN)
        {
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        }
        AT_StrNCpy(nDNS, tmpStr, AT_PING_MAX_HOSTLEN);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004019, "AT_TCPIP_CmdFunc_PING: nDNS = %s", nDNS);
        if (nNumParamter >= 2)
        {
            AT_PING_TIMEOUT = atParamDefUintInRange(pParam->params[1], AT_PING_TIMEOUT, 1, 255, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x100052ab, "AT_TCPIP_CmdFunc_PING: pParam1  error!\n");
                RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        if (nNumParamter >= 3)
        {
            AT_PING_PACKET_LEN = atParamDefUintInRange(pParam->params[2], 36, 36, 1500, &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        if (nNumParamter >= 4)
        {
            AT_PING_TOTAL_NUM = atParamDefUintInRange(pParam->params[3], 5, 1, 65535, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0x100052ac, "AT_TCPIP_CmdFunc_PING: pParam4 error\n");
                RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        if (netif_default != NULL)
        {
            AT_PING_CID = netif_default->sim_cid & 0x0f;
        }
        uint32_t nReturnValue = CFW_GethostbynameEX(nDNS, &AT_PING_IP, AT_PING_CID, nSim, dnsReq_callback, engine);
        if (nReturnValue == RESOLV_QUERY_QUEUED)
        {
            AT_SetAsyncTimerMux(engine, 60);
            return;
        }
        else if (nReturnValue == RESOLV_COMPLETE)
        {
            OSI_LOGI(0x1000401a, "AT_TCPIP_CmdFunc_PING: The IP Address = ");
            ip_addr_debug_print(LWIP_DBG_ON, &AT_PING_IP);
            if (AT_PING_PACKET_LEN != 16)
            {
                if (IP_IS_V4(&AT_PING_IP))
                    AT_PING_PACKET_LEN -= 20;
                else if (IP_IS_V6(&AT_PING_IP))
                {
                    if (atParamIsEmpty(pParam->params[2]))
                        AT_PING_PACKET_LEN = 56;
                    AT_PING_PACKET_LEN -= 40;
                }
            }
            if (AT_PING_PACKET_LEN < 16)
            {
                OSI_LOGI(0x10004f0f, "AT_TCPIP_CmdFunc_PING: AT_PING_PACKET_LEN < 16");
                RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        else
        {
            OSI_LOGI(0x1000401b, "AT_TCPIP_CmdFunc_PING: CFW_Gethostbyname ERROR(%x)", nReturnValue);
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (gPingtimer != NULL)
        {
            osiTimerDelete(gPingtimer);
            gPingtimer = NULL;
        }
        gPingtimer = osiTimerCreate(osiThreadCurrent(), ping_timeout, (void *)engine);
        if (gPingtimer == NULL)
            RETURN_CME_ERR(engine, ERR_AT_CME_EXE_FAIL);

        if (gPingContinuetimer != NULL)
        {
            osiTimerDelete(gPingContinuetimer);
            gPingContinuetimer = NULL;
        }
        gPingContinuetimer = osiTimerCreate(osiThreadCurrent(), ping_continue_timeout, (void *)engine);
        if (gPingContinuetimer == NULL)
        {
            if (gPingtimer != NULL)
            {
                osiTimerDelete(gPingtimer);
                gPingtimer = NULL;
            }
            RETURN_CME_ERR(engine, ERR_AT_CME_EXE_FAIL);
        }

        if (ERR_SUCCESS != (ret = AT_SendIP_ICMP_Ping(engine, (uint32_t)engine)))
        {
            if (gPingtimer != NULL)
            {
                osiTimerDelete(gPingtimer);
                gPingtimer = NULL;
            }
            if (gPingContinuetimer != NULL)
            {
                osiTimerDelete(gPingContinuetimer);
                gPingContinuetimer = NULL;
            }
            gEngine = NULL;
            OSI_LOGI(0x10003fee, "AT_TCPIP_CmdFunc_PING: Cannot find IP address of this modem!", nDNS);
            RETURN_CME_ERR(engine, ERR_AT_CME_EXE_FAIL);
        }

        OSI_LOGI(0x10004020, "AT_TCPIP_CmdFunc_PING: OK");
        gPingstatus = true;
        RETURN_OK(engine);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        char *pTestRsp = "+PING: DNS/IP address, timeout(1~255), packet_lenght(36~1500,ipv4)(56~1500,ipv6), ping_count(1~65535)";
        //atCmdRespUrcText(engine, pTestRsp);
        atCmdRespInfoText(engine, pTestRsp);
        RETURN_OK(engine);
    }
    else
    {
        OSI_LOGI(0x10004021, "AT_TCPIP_CmdFunc_PING: This type does not supported!");
        RETURN_CME_ERR(engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }

    return;
}
int8_t AT_SendIP_ICMP_Ping(atCmdEngine_t *engine, uint32_t pingparam)
{
    uint8_t uCidState = 0;
    CFW_SIM_ID nSim = (CFW_SIM_ID)atCmdGetSim((atCmdEngine_t *)pingparam);
    int32_t iResult = -1;
    int uaCid = 0;

    for (uaCid = AT_PDPCID_MIN; uaCid <= AT_PDPCID_MAX; uaCid++)
    {
        iResult = CFW_GetGprsActState(uaCid, &uCidState, nSim);
        if (iResult != ERR_SUCCESS || uCidState == 0)
        {
            OSI_LOGI(0, "get cid :%d,iResult = %d,uCidState = %d !", uaCid, iResult, uCidState);
            continue;
        }
        else
        {
            break;
        }
    }
    OSI_LOGI(0, "after check ,uaCid=%d netif_default = %p", uaCid, netif_default);
    if (netif_default != NULL)
        OSI_LOGI(0, "netif_default->sim_cid = %d", netif_default->sim_cid);
    if (uaCid > AT_PDPCID_MAX
#if IP_NAT_INTERNAL_FORWARD
        && (netif_default != NULL) && (netif_default->sim_cid != 0xff)
#endif
    )
    {
        ++AT_PING_SEQNO;
        return -2;
    }

    SOCKET socketfd = INVALID_SOCKET;
    int iResultBind = ERR_OK;
    if (!ping_socket_is_invalid())
    {
        OSI_LOGI(0, "AT_SendIP_ICMP AT_PING_SOCKET is valid,AT_PING_SOCKET=%d", AT_PING_SOCKET);
        CFW_TcpipSocketClose(AT_PING_SOCKET);
        AT_PING_SOCKET = INVALID_SOCKET;
    }
#if LWIP_IPV6
    if (IP_IS_V4(&AT_PING_IP) || ip6_addr_isipv4mappedipv6(ip_2_ip6(&AT_PING_IP)))
    {
        socketfd = CFW_TcpipSocket(AF_INET, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP);
        if (netif_default != NULL)
        {
            ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(netif_default);
            struct sockaddr_in stLocalAddr = {0};
            stLocalAddr.sin_len = sizeof(struct sockaddr_in);
            stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
            stLocalAddr.sin_addr.s_addr = ip_addr->addr;
            iResultBind = CFW_TcpipSocketBind(socketfd, (CFW_TCPIP_SOCKET_ADDR *)&stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
        }
    }
    else
    {
        socketfd = CFW_TcpipSocket(AF_INET6, CFW_TCPIP_SOCK_RAW, IPPROTO_ICMPV6);
        if (netif_default != NULL)
        {
            ip6_addr_t *ip_addr = (ip6_addr_t *)netif_ip6_addr(netif_default, 0);
            ip6_addr_t *ip_addr_1 = (ip6_addr_t *)netif_ip6_addr(netif_default, 1);
            if (ip6_addr_isany_val(*(ip_addr_1)) != true)
            {
                ip_addr = ip_addr_1;
            }
            struct sockaddr_in6 stLocalAddr = {0};
            stLocalAddr.sin6_len = sizeof(struct sockaddr_in6);
            stLocalAddr.sin6_family = AF_INET6;
            inet6_addr_from_ip6addr(&stLocalAddr.sin6_addr, ip_addr);
            iResultBind = CFW_TcpipSocketBind(socketfd, (CFW_TCPIP_SOCKET_ADDR *)&stLocalAddr, sizeof(struct sockaddr_in6));
        }
    }
#else
    socketfd = CFW_TcpipSocket(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP);
    if (netif_default != NULL)
    {
        ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(netif_default);
        struct sockaddr_in stLocalAddr = {0};
        stLocalAddr.sin_len = sizeof(struct sockaddr_in);
        stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
        stLocalAddr.sin_addr.s_addr = ip_addr->addr;
        iResultBind = CFW_TcpipSocketBind(socketfd, (CFW_TCPIP_SOCKET_ADDR *)&stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
    }
#endif
    if (SOCKET_ERROR == iResultBind)
    {
        if (socketfd > 0)
            CFW_TcpipSocketClose(socketfd);
        socketfd = INVALID_SOCKET;
    }
    if (INVALID_SOCKET == socketfd)
    {
        ++AT_PING_SEQNO;
        return -1;
    }
    AT_PING_SOCKET = socketfd;
    OSI_LOGI(0, "socketfd = %d, AT_PING_SOCKET =%d, &engine =0x%x ", socketfd, AT_PING_SOCKET, pingparam);
    //Get TTL
    int len = 1;
    if (ERR_SUCCESS != CFW_TcpipSocketGetsockopt(socketfd, CFW_TCPIP_IPPROTO_IP, IP_TTL, &AT_PING_TTL, &len))
    {
        OSI_LOGI(0x10003fec, "AT_TCPIP_CmdFunc_PING: Using DEFAULT TTL");
        AT_PING_TTL = 255;
    }
    OSI_LOGI(0, "AT_TCPIP_CmdFunc_PING: AT_PING_TTL = %d", AT_PING_TTL);

    struct sockaddr_storage to;
    uint8_t ipData[1500] = {0};
    struct icmp_echo_hdr *idur;
    idur = (struct icmp_echo_hdr *)ipData;

    ICMPH_CODE_SET(idur, 0);
    idur->id = htons(++AT_PING_ID);
    idur->seqno = htons(++AT_PING_SEQNO);
    // in 16384Hz ticks.
    *((uint32_t *)(ipData + 8)) = (uint32_t)osiUpTime();
    *((uint32_t *)(ipData + 12)) = socketfd;
    for (uint16_t i = 0; i < AT_PING_PACKET_LEN - 16; i++)
    {
        ipData[i + 16] = i;
    }
    idur->chksum = 0;
    OSI_LOGI(0x10003ff2, "AT_TCPIP_CmdFunc_PING: Start time = %d, socketid = %d, AT_PING_SEQNO = %d", *((uint32_t *)(ipData + 8)), *((uint32_t *)(ipData + 12)), AT_PING_SEQNO);
    CFW_TcpipSocketSetParam(socketfd, AT_PING_AsyncEventProcess, pingparam);
#if LWIP_IPV4
    if (IP_IS_V4(&AT_PING_IP))
    {
        struct sockaddr_in *to4 = (struct sockaddr_in *)&to;
        to4->sin_len = sizeof(to4);
        to4->sin_family = AF_INET;
        inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&AT_PING_IP));
        ICMPH_TYPE_SET(idur, ICMP_ECHO);
        idur->chksum = inet_chksum(idur, AT_PING_PACKET_LEN);
    }
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    if (IP_IS_V6(&AT_PING_IP) && !ip6_addr_isipv4mappedipv6(ip_2_ip6(&AT_PING_IP)))
    {
        struct sockaddr_in6 *to6 = (struct sockaddr_in6 *)&to;
        to6->sin6_len = sizeof(to6);
        to6->sin6_family = AF_INET6;
        inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(&AT_PING_IP));
        ICMPH_TYPE_SET(idur, ICMP6_TYPE_EREQ);
    }
#endif /* LWIP_IPV6 */
    gPingReplyed = 0;
    if (-1 == CFW_TcpipSocketSendto(socketfd, ipData, AT_PING_PACKET_LEN, 0, (CFW_TCPIP_SOCKET_ADDR *)&to, sizeof(to)))
    {
        CFW_TcpipSocketClose(AT_PING_SOCKET);
        AT_PING_SOCKET = INVALID_SOCKET;
        return -1;
    }
    //uint32_t nDLCI32 = nDLCI;
    //COS_StartCallbackTimer(CSW_AT_TASK_HANDLE, AT_PING_TIMEOUT * 1000, ping_timeout, (void *)nDLCI32);

    if (false == osiTimerStart(gPingtimer, AT_PING_TIMEOUT * 1000))
    {
        OSI_LOGI(0, "AT_TCPIP_CmdFunc_PING: start timer failed!");
        return -1;
    }
    return ERR_SUCCESS;
}

bool ping_socket_is_invalid()
{
    return (AT_PING_SOCKET == INVALID_SOCKET) ? true : false;
}

void ping_result_statics(atCmdEngine_t *engine, char *response)
{

    char *pcIpAddr = ipaddr_ntoa(&AT_PING_IP);
    if (!ping_socket_is_invalid())
    {
        OSI_LOGI(0, "ping_result_statics/AT_PING_SOCKET is valid");
        CFW_TcpipSocketClose(AT_PING_SOCKET);
        AT_PING_SOCKET = INVALID_SOCKET;
    }
    gPingstatus = false;

    sprintf(response, "\n\rPing statistics for %s", pcIpAddr);
    if (atCmdEngineIsValid(engine))
        atCmdRespInfoText(engine, response);
    if (AT_PING_MIN_DELAY == 0x7fffffff)
        AT_PING_MIN_DELAY = -1;
    if (AT_PING_ECHO_NUM == 0)
    {
        sprintf(response, "Packets: Sent = %d, Received = 0, Lose = %d <100\%%>", AT_PING_SEQNO, AT_PING_SEQNO);
    }
    else
    {
        sprintf(response, "Packets: Sent = %d, Received = %d, Lose = %d <%d\%%>, max_delay = %ld ms, min_delay = %ld ms, average delay = %ld ms", AT_PING_SEQNO, AT_PING_ECHO_NUM,
                AT_PING_SEQNO - AT_PING_ECHO_NUM, 100 - AT_PING_ECHO_NUM * 100 / AT_PING_SEQNO, AT_PING_MAX_DELAY, AT_PING_MIN_DELAY, AT_PING_TIME_SUM / AT_PING_ECHO_NUM);
    }
    if (atCmdEngineIsValid(engine))
        atCmdRespInfoText(engine, response);
    if (gPingtimer != NULL)
    {
        osiTimerStop(gPingtimer);
        osiTimerDelete(gPingtimer);
        gPingtimer = NULL;
    }
    if (gPingContinuetimer != NULL)
    {
        osiTimerStop(gPingContinuetimer);
        osiTimerDelete(gPingContinuetimer);
        gPingContinuetimer = NULL;
    }
    AT_PING_TIME_SUM = 0;
    AT_PING_SEQNO = 0;
    AT_PING_ECHO_NUM = 0;
    //gPingstatus = false;
    gEngine = NULL;
}

void ping_continue(atCmdEngine_t *engine, char *response)
{
    if (AT_PING_SEQNO < AT_PING_TOTAL_NUM && atCmdEngineIsValid(engine))
    {
        int8_t nRetValue = AT_SendIP_ICMP_Ping(engine, (uint32_t)engine);
        if (nRetValue != ERR_SUCCESS)
        {
            OSI_LOGI(0, "ping_continue: AT_SendIP_ICMP_Ping return:%d", nRetValue);
            if (false == osiTimerStart(gPingtimer, AT_PING_TIMEOUT * 1000))
            {
                OSI_LOGI(0, "AT_TCPIP_CmdFunc_PING: start timer failed!");
                char response[200] = {0};
                ping_result_statics(engine, response);
            }
            return;
        }
    }
    else
    {
        ping_result_statics(engine, response);
    }
}

void ping_timeout(void *param)
{
    char response[200] = {0};
    atCmdEngine_t *engine = (atCmdEngine_t *)param;
    sprintf(response, "Request timed out.\r\n");
    atCmdRespInfoText(engine, response);
    ping_continue(engine, response);
}

void ping_continue_timeout(void *param)
{
    char response[200] = {0};
    atCmdEngine_t *engine = (atCmdEngine_t *)param;
    ping_continue(engine, response);
}
#if 0
//#ifdef LTE_NBIOT_SUPPORT
CFW_CMCCTEST_PSMDATA    cmcc_psm_data;
#define PSMDATA_ALARM_INDEX 10
bool psmDataStartRtcTimer(CFW_CMCCTEST_PSMDATA *psmdata)
{
    uint8_t arm_num = 0xff;
    if (psmdata->sleep_time > 0)
    {
        psmdata->isRunning = 1;
        TM_ALARM psmdata_alarm;
        TM_FILETIME ctime;
        memset(&psmdata_alarm,0,sizeof(TM_ALARM));
        if (TM_GetSystemFileTime (&ctime))
        {
            psmdata_alarm.nFileTime = ctime.DateTime + psmdata->sleep_time;
            psmdata_alarm.nIndex = PSMDATA_ALARM_INDEX;
            psmdata_alarm.nRecurrent = 0x1;
            psmdata_alarm.nTextLength = 0;
            psmdata_alarm.nType = 0;
            if (!TM_SetAlarm(&psmdata_alarm, &arm_num))
                return false;
        }
        else
            return false;
    }
    return true;
}


void AT_PsmData_TimerExpiry()
{
    int32_t socket = 0;
    struct netif *pNetif = NULL;
    CFW_CMCCTEST_PSMDATA *psm_data = &cmcc_psm_data;
    //COS_Sleep(1000);
    hal_HstSendEvent(0x20180550|psm_data->isRunning);
    if(!psm_data->isRunning)
    {
        return;
    }
    uint8_t nSim, nCid;
    at_ModuleGetSimCid(&nSim, &nCid, psm_data->dlci);

//    OSI_LOGI(0, "AT_PsmData_TimerExpiry nSim = %d, nCid =%d", nSim, nCid);
    pNetif = getGprsNetIf(nSim, nCid);
    uint8_t *send_data = NULL;
    uint8_t hint_text[100];
    memset(hint_text, 0x00, sizeof(hint_text));

    if(pNetif)
    {   
        if(psm_data->type == 0)  //for ping
        {
#define AT_PING_MAX_HOSTLEN 63
            //uint16_t nBuffer = AT_PING_MAX_HOSTLEN;
            uint8_t nDNS[AT_PING_MAX_HOSTLEN] = {0};

            if (AT_StrLen(psm_data->host) > AT_PING_MAX_HOSTLEN)
                return;        
            AT_StrCpy(nDNS, psm_data->host);
            OSI_LOGXI(OSI_LOGPAR_S, 0x100052ae, "AT_PsmData_TimerExpiry: nDNS = %s", nDNS);
            AT_PING_TIMEOUT = 20;
            AT_PING_PACKET_LEN = psm_data->length;
            AT_PING_TOTAL_NUM = 5;

            uint32_t nReturnValue = CFW_GethostbynameEX(nDNS, &AT_PING_IP, 1, nSim, dnsReq_callback, (void *)psm_data->dlci);
            if (nReturnValue == RESOLV_QUERY_QUEUED)
            {
                return;
            }
            else if (nReturnValue == RESOLV_COMPLETE)
            {
                ip_addr_debug_print(LWIP_DBG_ON, &AT_PING_IP);
                if (AT_PING_PACKET_LEN !=16)
                {
                    if (IP_IS_V4(&AT_PING_IP))
                        AT_PING_PACKET_LEN  -= 20;
                    else if (IP_IS_V6(&AT_PING_IP))
                        AT_PING_PACKET_LEN  -= 40;
                }
                if (AT_PING_PACKET_LEN < 16)
                {
                    return;
                }
            }
            else
            {
                return;
            }


            SOCKET socketfd = INVALID_SOCKET;
#if LWIP_IPV6
            if (IP_IS_V4(&AT_PING_IP) || ip6_addr_isipv4mappedipv6(ip_2_ip6(&AT_PING_IP)))
            {
                socketfd = CFW_TcpipSocket(AF_INET, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP);
            }
            else
            {
                socketfd = CFW_TcpipSocket(AF_INET6, CFW_TCPIP_SOCK_RAW, IPPROTO_ICMPV6);
            }
#else
            socketfd = CFW_TcpipSocket(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP);
#endif
            if (INVALID_SOCKET == socketfd)
            {
                return;
            }
            AT_PING_SOCKET = socketfd;
            
            CFW_TcpipSocketSetParam(socketfd, AT_PING_AsyncEventProcess, psm_data->dlci);
            int32_t len = 1;
            if (ERR_SUCCESS != CFW_TcpipSocketGetsockopt(socketfd, CFW_TCPIP_IPPROTO_IP, IP_TTL, &AT_PING_TTL, &len))
            {
                OSI_LOGI(0x10003fec, "AT_TCPIP_CmdFunc_PING: Using DEFAULT TTL");
                AT_PING_TTL = 255;
            }

            if (ERR_SUCCESS != AT_SendIP_ICMP_Ping(AT_PING_SOCKET, psm_data->dlci))
            {
                OSI_LOGI(0x10003fee, "AT_TCPIP_CmdFunc_PING: Cannot find IP address of this modem!", nDNS);
                return;
            }
            sprintf(hint_text, "PING %s", psm_data->host);
            
        }
        else    //for udp
        {
            socket = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, SOCK_DGRAM,CFW_TCPIP_IPPROTO_UDP,NULL,psm_data->dlci);
            if(socket != SOCKET_ERROR)
            {
                ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(pNetif);
                CFW_TCPIP_SOCKET_ADDR stLocalAddr = {0,};
                stLocalAddr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);;
                stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
                stLocalAddr.sin_port = 0;//htons(psm_data->port);

                stLocalAddr.sin_addr.s_addr = ip_addr->addr;

                if(SOCKET_ERROR != CFW_TcpipSocketBind(socket, &stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR)))
                {
                    CFW_TCPIP_SOCKET_ADDR nDestAddr;
                    uint32_t  destAddr = CFW_TcpipInetAddr(psm_data->host);
                    if(destAddr != SOCKET_ERROR)
                    {
                        nDestAddr.sin_addr.s_addr = destAddr;
                        nDestAddr.sin_port = htons(psm_data->port);
                        nDestAddr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);
                        nDestAddr.sin_family = CFW_TCPIP_AF_INET;
                        send_data = at_malloc(psm_data->length);
                        if(send_data)
                        {
                            CFW_TcpipSocketSendto(socket, (void *)send_data, psm_data->length, 0, &nDestAddr, nDestAddr.sin_len);
                            at_free(send_data);
                            CFW_TcpipSocketClose(socket);
                            sprintf(hint_text, "Send UDP to %s", psm_data->host);
                        }
                    }
                }
            }
        }
        at_CmdReportUrcText(nSim, hint_text);
        psm_data->current_count++;
        if(psm_data->current_count < psm_data->retx_count)
        {
            psmDataStartRtcTimer(psm_data);
        }
        else
        {
            psm_data->isRunning = 0;
        }
        return;
    }

    
}

bool isAlarmForPsmData(uint8_t index)
{
    return index == PSMDATA_ALARM_INDEX;
}

void AT_TCPIP_CmdFunc_PSMDATA(AT_CMD_PARA *pParam)
{
    uint8_t     *host = NULL;
    uint8_t     type = 0;
    uint16_t    port = 0;
    uint16_t    length = 0;
    uint32_t    sleep_time = 0;
    uint16_t      retx_count = 0;
    TM_ALARM   pAlarm;
    if(!pParam)
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    if(AT_CMD_SET == pParam->type)
    {
        bool  paramok = true;
        uint8_t index = 0;
        type = at_ParamUintInRange(pParam->params[index++], 0, 1, &paramok);
        host = at_ParamStr(pParam->params[index++], &paramok);
        port = at_ParamUint(pParam->params[index++], &paramok);
        length = at_ParamUintInRange(pParam->params[index++], 0, 1500, &paramok);
        sleep_time = at_ParamUint(pParam->params[index++], &paramok);
        retx_count = at_ParamUint(pParam->params[index], &paramok);
        if(!paramok)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        
        memcpy(cmcc_psm_data.host, host, AT_StrLen(host));
        cmcc_psm_data.type = type;
        cmcc_psm_data.port = port;
        cmcc_psm_data.length = length;
        cmcc_psm_data.sleep_time = sleep_time;
        cmcc_psm_data.retx_count = retx_count;
        cmcc_psm_data.dlci= pParam->nDLCI;
        cmcc_psm_data.isRunning = 0;
        psmDataStartRtcTimer(&cmcc_psm_data);
        RETURN_OK(pParam->engine);
    }
    else if (AT_CMD_READ == pParam->type)
    {
        uint8_t psmdataRead[50];
        sprintf(psmdataRead, "^PSMDATA: %d,\"%s\",%d,%d,%d,%d", 
            cmcc_psm_data.type, cmcc_psm_data.host, cmcc_psm_data.port, cmcc_psm_data.length, cmcc_psm_data.sleep_time/1000, cmcc_psm_data.retx_count);
        at_CmdRespInfoText(pParam->engine, psmdataRead);
        RETURN_OK(pParam->engine);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        uint8_t *psmdataTest = "^PSMDATA: type, remote_ip, remote_port, packet_length, interval_time, max_count";
        at_CmdRespInfoText(pParam->engine, psmdataTest);
        RETURN_OK(pParam->engine);
    }
    else
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
#endif
#endif
