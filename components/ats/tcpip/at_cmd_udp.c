#if 0
//#include "at_cfg.h"
//#include "at_cmd_nw.h"
//#include "at_utils.h"
//#include "unistd.h"

#include "cfw.h"
#ifdef CFW_GPRS_SUPPORT
#include "stdio.h"
#include "osi_api.h"
#include "osi_log.h"
#include "at_response.h"
#include "at_cfw.h"
#include "at_engine.h"
#include "at_command.h"
#include "sockets.h"
#include "netmain.h"
#include "cfw_errorcode.h"

#define AT_TCPIP_ADDRLEN 20
#define AT_TCPIP_DEFAULTAPN "CMNET"
#define AT_TCPIP_HOSTLEN 256
#define AT_CMD_LINE_BUFF_LEN 1024 * 5

typedef struct _stAT_Udp_Paras
{
    uint8_t uType;
    uint8_t uProtocol;
    uint8_t uSocket;
    uint8_t uDomain;
    uint16_t uPort;
    uint8_t uaIPAddress[AT_TCPIP_HOSTLEN];
    uint32_t packetLen;
    uint32_t waitms;
    atCmdEngine_t *aAtEngine;
} stAT_Udp_Paras;

typedef struct _stAT_Ping_Paras
{
    uint32_t nDlc;
    struct stAT_Udp_Paras *icmpInfo;
} stAT_Ping_Paras;

typedef struct udp_callback_param
{
    atCmdEngine_t *engine;
    uint8_t recvcon;
} udp_callback_param_t;

#define ERR_SENDFAIL 2
#define ERR_TIMEOUT 1

//uint16_t PING_TOTAL_NUM = 1;
//uint16_t PING_ECHO_NUM = 0;
//uint8_t PING_CID = 0;
ip_addr_t PING_IP;
uint32_t PING_TIMEOUT = 1000;
uint32_t PING_PACKET_LEN = 16;
uint8_t PING_TTL = 0;
uint16_t PING_ID = 0;
uint16_t PING_SEQNO = 0;
SOCKET PING_SOCKET = INVALID_SOCKET;
uint32_t PING_START_TIME = 0;
osiTimer_t *gTPingtimer = NULL;

extern struct netif *getGprsNetIf(uint8_t nSim, uint8_t nCid);
extern struct netif *getEtherNetIf(uint8_t nCid);

void pingip_timeout(void *param);
void pingip_continue(uint32_t pingparam);
extern uint16_t inet_chksum(const void *dataptr, uint16_t len);

stAT_Udp_Paras nTcpipParas[MEMP_NUM_NETCONN];
#if 0
static int8_t recv_udpdata(uint8_t uSocket, uint32_t uDataSize, uint8_t nDlc, uint8_t mode)
#else
static int8_t recv_udpdata(uint8_t uSocket, uint32_t uDataSize, atCmdEngine_t *engine, uint8_t mode)
#endif
{
    int32_t iResult = 0;
    //uint16_t uIPHlen = 0;
    char *pData = NULL;
    //uint8_t nSim = AT_SIM_ID(nDlc);
    //atCmdEngine_t *engine = at_CmdGetByChannel(nDlc);
    ip_addr_t from_addr_t;
    u16_t local_port;
    pData = malloc(uDataSize + 25);
    if (NULL == pData)
    {
        OSI_LOGI(0x10003f47, "EV_CFW_TCPIP_REV_DATA_IND, memory error");
        atCmdRespCmeError(engine, ERR_AT_CME_NO_MEMORY);
        return -1;
    }
    memset(pData, 0, uDataSize + 25);

    union sockaddr_aligned from_addr;
    int fromLen = sizeof(union sockaddr_aligned);
    iResult = CFW_TcpipSocketRecvfrom(uSocket, pData, uDataSize, 0, (CFW_TCPIP_SOCKET_ADDR *)&from_addr, &fromLen);
    if (SOCKET_ERROR == iResult)
    {
        free(pData);
        OSI_LOGI(0x10003f48, "EV_CFW_TCPIP_REV_DATA_IND, CFW_TcpipSocketRecv error");
        atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        return -1;
    }
    char *uaRspStr = malloc(iResult * 2 + 60);
    if (NULL == uaRspStr)
        return -1;
    memset(uaRspStr, 0, iResult * 2 + 60);
    SOCKADDR_TO_IPADDR_PORT(((const struct sockaddr *)&from_addr), &from_addr_t, local_port);
    sprintf(uaRspStr, "%d,%s,%hu,%ld,", uSocket, ipaddr_ntoa(&from_addr_t), local_port, uDataSize);
    char hexData[3] = {0};
    for (int i = 0; i < iResult; i++)
    {
        memset(hexData, 0, 3);
        sprintf(hexData, "%02x", pData[i]);
        //writeInfoNText(engine,hexData,2);
        strcat(uaRspStr, hexData);
    }
    OSI_LOGXI(OSI_LOGPAR_S, 0x10003fc3, "EV_CFW_TCPIP_REV_DATA_IND, recv:%s", uaRspStr);
    atCmdRespInfoNText(engine, uaRspStr, strlen(uaRspStr));
    free(pData);
    free(uaRspStr);
    return 0;
}

static void udp_rsp(void *param)
{

    uint8_t uSocket = (uint8_t)PING_SOCKET;

    //AT_TC(g_sw_TCPIP, "udp_rsp uSocket=%d,tcpipParas->uSocket=%d,nRecvconn=%d", uSocket, nRecvconn);

    osiEvent_t *pEvent = (osiEvent_t *)param;

    udp_callback_param_t *udpparam = (udp_callback_param_t *)pEvent->param3;
    //uint8_t nDlc = pEvent->param3;
    //uint8_t nMuxIndex = udpparam->nMuxIndex;
    atCmdEngine_t *engine = (atCmdEngine_t *)pEvent->param3;

    switch (pEvent->id)
    {
    case EV_CFW_TCPIP_SOCKET_CONNECT_RSP:

        break;
    case EV_CFW_TCPIP_ACCEPT_IND:

        break;
    case EV_CFW_TCPIP_SOCKET_CLOSE_RSP:
        PING_SOCKET = INVALID_SOCKET;
        break;
    case EV_CFW_TCPIP_ERR_IND:
        if (CFW_TcpipSocketClose(uSocket) != ERR_SUCCESS)
        {
            free(pEvent);
            AT_CMD_RETURN(atCmdRespInfoText(engine, "CFW_TcpipSocketClose error"));
        }
        break;

    case EV_CFW_TCPIP_CLOSE_IND:

        break;
    case EV_CFW_TCPIP_SOCKET_SEND_RSP:

        break;
    case EV_CFW_TCPIP_REV_DATA_IND:
    {
        uint8_t nRecvconn = udpparam->recvcon; //pEvent->param3 >> 16;
        if (1 == nRecvconn)
        {
            uint16_t uDataSize = 0;
            uDataSize = (uint16_t)pEvent->param2; // maybe contain check bits
#if 0            
            recv_udpdata(uSocket, uDataSize, nDlc, 0);
#else
            recv_udpdata(uSocket, uDataSize, engine, 0);
#endif
        }
    }
    break;

    case EV_CFW_ICMP_DATA_IND:
    {
        char Response[100] = {0};
        OSI_LOGI(0x100052a2, "pEvent->param1 = %d, pEvent->param2 = %d", pEvent->param1, pEvent->param2);

        OSI_LOGI(0x10003fc5, "AT_PING_ID = %d", PING_ID);

        osiTimerStop(gTPingtimer);
        osiTimerDelete(gTPingtimer);
        gTPingtimer = NULL;

        char *pcIpAddr = ipaddr_ntoa(&PING_IP);

        OSI_LOGXI(OSI_LOGPAR_S, 0x10003fc6, "AT_PING_IP = %s", pcIpAddr);
        uint16_t id = pEvent->param2 >> 16;
        uint16_t seqno = pEvent->param2 & 0xFFFF;
        if ((id != PING_ID) || (seqno != PING_SEQNO))
        {
            OSI_LOGI(0x10003fc7, "AT_PING_SEQNO = %d,seqno = %d", PING_SEQNO, seqno);
            OSI_LOGI(0x100052a3, ">>>pEvent->param2 = %d, AT_PING_ID = %d", id, PING_ID);
            CFW_TcpipSocketClose(PING_SOCKET);
            PING_SOCKET = INVALID_SOCKET;
            atCmdRespInfoText(engine, "PING_ID or PING_SEQNO error");
            break;
        }
        uint32_t ticks = (uint32_t)osiUpTime();

        ticks -= pEvent->param1;
        uint32_t mseconds = ticks * 1000 / 16384;

        //AT_Sprintf(Response, "Reply from %s: bytes= %d time = %d(ms), TTL = %d", pcIpAddr, 32, mseconds, PING_TTL);
        sprintf(Response, "+TPING: %s, %d, %ld\n", pcIpAddr, PING_TTL, mseconds);
        //PING_ECHO_NUM++;
        CFW_TcpipSocketClose(PING_SOCKET);
        PING_SOCKET = INVALID_SOCKET;
        atCmdRespInfoText(engine, Response);

        //pingip_continue(nDlc, Response);
        break;
    }
    break;
    default:
        OSI_LOGI(0x10003fc9, "UDP unexpect asynchrous event/response %d", pEvent->id);
        //atCmdRespInfoText(engine, "UDP unexpect asynchrous event/response");
        break;
    }
    free(pEvent);
}

static uint8_t udp_getCID(uint8_t nSim)
{
    int i;
    uint8_t uPDPState = CFW_GPRS_DEACTIVED;
    uint8_t m_uAttState = CFW_GPRS_DETACHED;

    if (CFW_GetGprsAttState(&m_uAttState, nSim) != ERR_SUCCESS || m_uAttState == CFW_GPRS_DETACHED)
    {
        OSI_LOGI(0, "FTP# sim%d not attached", nSim);
        return 0xFF;
    }

    for (i = 0; i < 8; i++)
    {
        if (CFW_GetGprsActState(i, &uPDPState, nSim) != ERR_SUCCESS || uPDPState == CFW_GPRS_DEACTIVED)
        {
            continue;
        }

        return i;
    }

    OSI_LOGI(0, "FTP# sim%d not actived", nSim);
    return 0xFF;
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_TSOCR
* Description: Start up udp connection
* Parameter:  AT_CMD_PARA *pParam
* Return:       void
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-15
******************************************************************************/

void AT_TCPIP_CmdFunc_TSOCR(atCommand_t *pParam)
{
    OSI_LOGI(0x100052a4, "AT+TSOCR ... ... pParam->nDLCI= NULL"); //, pParam->nDLCI);

    if (AT_CMD_SET == pParam->type)
    {
        if ((pParam->param_count != 3) && (pParam->param_count != 4))
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        bool paramok = true;
        uint8_t nType, nProtocol;
        uint16_t uPort = 0;
        uint8_t recvcon = 1;
        uint8_t sockfd = -1;
        const char *type = atParamStr(pParam->params[0], &paramok);
        const uint8_t protocol = atParamUint(pParam->params[1], &paramok);
        uPort = atParamUintInRange(pParam->params[2], 0, 65535, &paramok);
        if (pParam->param_count == 4)
            recvcon = atParamUintInRange(pParam->params[3], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        if (5683 == uPort || strcasecmp(type, "DGRAM"))
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        nType = CFW_TCPIP_SOCK_DGRAM;
        if (17 == protocol)
            nProtocol = CFW_TCPIP_IPPROTO_UDP;
        else
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        int i = 0;
        for (i = 0; i < MEMP_NUM_NETCONN; i++)
        {
            if (nTcpipParas[i].uProtocol == nProtocol && nTcpipParas[i].uPort == uPort)
                break;
        }
        if (i < MEMP_NUM_NETCONN)
        {
            OSI_LOGI(0x10003fcb, "sock %d,has been created with this protocal and port", i + 1);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        struct netif *netif = NULL;
        //struct ifreq iface;
        //memset(&iface, 0, sizeof(struct ifreq));
        uint8_t nSim, nCid;
        //at_ModuleGetSimCid(&nSim, &nCid, pParam->nDLCI);
        nSim = atCmdGetSim(pParam->engine);
        nCid = udp_getCID(nSim);
        if (nCid == 0x11)
            netif = getEtherNetIf(nCid);
        else
            netif = getGprsNetIf(nSim, nCid);
        if (netif == NULL)
        {
            atCmdRespInfoText(pParam->engine, "netif == NULL");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        int32_t iResult = 0;

        udp_callback_param_t *udpparam = malloc(sizeof(udp_callback_param_t));
        udpparam->engine = pParam->engine;
        udpparam->recvcon = recvcon;

        iResult =
            CFW_TcpipSocketEX(CFW_TCPIP_AF_INET,
                              nType,
                              nProtocol,
                              udp_rsp, (uint32_t)udpparam); //recvcon << 16| pParam->nDLCI);

        //iResult = socket( CFW_TCPIP_AF_INET, nType,nProtocol );
        if (SOCKET_ERROR == iResult)
        {
            OSI_LOGI(0x10003fcc, "AT_TCPIP_CmdFunc_TSOCR(), CFW_TcpipSocket() failed\n");
            free(udpparam);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        sockfd = iResult;

        //iface.ifr_name[0] = netif->name[0];
        //iface.ifr_name[1] = netif->name[1];
        //strcat(iface.ifr_name, i8tostring(netif->num));
        //AT_TC(g_sw_TCPIP, "AT_TCPIP_Connect() netif:%s\n", iface.ifr_name);

        ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(netif);

        CFW_TCPIP_SOCKET_ADDR stLocalAddr = {
            0,
        };
        stLocalAddr.sin_len = 0;
        stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
        stLocalAddr.sin_port = htons(uPort);

        stLocalAddr.sin_addr.s_addr = ip_addr->addr;

        iResult = CFW_TcpipSocketBind(sockfd, &stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
        if (SOCKET_ERROR == iResult)
        {
            free(udpparam);
            OSI_LOGI(0x10003fcd, "+TSOCR CFW_TcpipSocketBind() failed\n");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        nTcpipParas[sockfd - 1].uSocket = sockfd;
        nTcpipParas[sockfd - 1].uType = nType;
        nTcpipParas[sockfd - 1].uProtocol = nProtocol;
        nTcpipParas[sockfd - 1].uDomain = CFW_TCPIP_AF_INET;
        nTcpipParas[sockfd - 1].uPort = uPort;
        nTcpipParas[sockfd - 1].aAtEngine = pParam->engine;
        memset(nTcpipParas[sockfd - 1].uaIPAddress, 0, AT_TCPIP_HOSTLEN);

        char sock_id[4] = {0};
        sprintf(sock_id, "%d", sockfd);
        atCmdRespInfoText(pParam->engine, sock_id);
        free(udpparam);
        RETURN_OK(pParam->engine);
    }
    /*else if (AT_CMD_TEST == pParam->iType)
    {
        uint8_t *pRstStr = AT_MALLOC(85);
        if (NULL == pRstStr)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);

        AT_MemSet(pRstStr, 0, 85);
        AT_StrCpy(pRstStr, "+TSOCR:(\"DGRAM\"),(17),(0-65535 EXPECT 5683),(0-1)");

        at_CmdRespInfoText(pParam->engine, pRstStr);
        AT_FREE(pRstStr);
        RETURN_OK(pParam->engine);
    }*/
    else
    {
        OSI_LOGI(0x10003fce, "AT_TCPIP_CmdFunc_TSOCR(), command type not allowed");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/********************************************************************************************
* Name:                    AT_TCPIP_CmdFunc_CIPSEND
* Description:      Send data through TCP or UDP connection
* Parameter:        AT_CMD_PARA *pParam
* Return:               void
* Remark:               After cmds +CGATT,+CGDCONT,+CGACT,+CIPSTART return OK,this cmd can be run
* Author/Modifier:      YangYang,wulc
* Data:                 2008-5-21
********************************************************************************************/
#define MAX_PACKET_SIZE 1024

void AT_TCPIP_CmdFunc_TSOST(atCommand_t *pParam)
{
    OSI_LOGI(0x100052a5, "AT+TSOST: pParam->iType = %d, pParam->nDLCI = NULL", pParam->type); //, pParam->nDLCI);

    if (AT_CMD_SET == pParam->type)
    {
        if (pParam->param_count != 5)
        {
            OSI_LOGI(0x10003fd0, "AT+TSOST: pPara number error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        int32_t iResult = 0;
        bool paramok = true;
        int32_t sock_id;

        sock_id = atParamUint(pParam->params[0], &paramok);
        const char *ipaddress = atParamStr(pParam->params[1], &paramok);
        uint16_t uPort = atParamUintInRange(pParam->params[2], 0, 65535, &paramok);
        uint32_t uLength = atParamUintInRange(pParam->params[3], 1, AT_CMD_LINE_BUFF_LEN, &paramok);
        if (!paramok)
        {
            OSI_LOGI(0x10003fd1, "AT_TCPIP_CmdFunc_TSOST: get param error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
#if 0		
        const uint8_t *senddata = at_ParamHexData(pParam->params[4], uLength, &paramok);
#else
        const char *senddata = atParamRawText(pParam->params[4], &paramok);
#endif
        if (!paramok)
        {
            OSI_LOGI(0x10003fd2, "AT_TCPIP_CmdFunc_TSOST: get senddata error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (sock_id != nTcpipParas[sock_id - 1].uSocket)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (nTcpipParas[sock_id - 1].aAtEngine != pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        CFW_TCPIP_SOCKET_ADDR nDestAddr;
        nDestAddr.sin_family = CFW_TCPIP_AF_INET;
        nDestAddr.sin_port = htons(uPort);
        nDestAddr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);
        iResult = CFW_TcpipInetAddr(ipaddress);
        if (SOCKET_ERROR == iResult)
        {
            OSI_LOGI(0x10003fd3, "AT_TCPIP_CmdFunc_TSOST CFW_TcpipInetAddr() failed\n");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            nDestAddr.sin_addr.s_addr = iResult;
            OSI_LOGI(0x10003f7d, "dest_addr.sin_addr.s_addr send 0x%x \n", nDestAddr.sin_addr.s_addr);
            iResult = CFW_TcpipSocketSendto(sock_id, (void *)senddata, uLength, 0, &nDestAddr, nDestAddr.sin_len);
            if (iResult == SOCKET_ERROR)
            {

                OSI_LOGI(0x10003f7e, "UDP send socket data error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
        }

        char aucBuffer[40] = {0};
        AT_SetAsyncTimerMux(pParam->engine, 120);
        if (iResult != SOCKET_ERROR)
        {
            sprintf(aucBuffer, "%ld,%ld", sock_id, iResult);
            atCmdRespInfoText(pParam->engine, aucBuffer);
            RETURN_OK(pParam->engine);
        }
    }
    /*else if (AT_CMD_TEST == pParam->iType)
    {
        uint8_t aucBuffer[40] = {0};
        AT_Sprintf(aucBuffer, "+TSOST:(1-8),(\"(0-255).(0-255).(0-255).(0-255)\"),(0-65535),(1-1024),\"ABC...\"");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        RETURN_OK(pParam->engine);
    }*/
    else
    {
        OSI_LOGI(0x10003f7f, "AT_TCPIP_CmdFunc_CIPSEND(), command type not allowed");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_TSOSTF(atCommand_t *pParam)
{
    OSI_LOGI(0x100052a6, "AT+TSOSTF: pParam->iType = %d, pParam->nDLCI = NULL", pParam->type); //, pParam->nDLCI);

    if (AT_CMD_SET == pParam->type)
    {
        if (pParam->param_count != 6)
        {
            OSI_LOGI(0x10003fd5, "AT+TSOSTF: pPara number error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        int32_t iResult = 0;
        bool paramok = true;
        int32_t sock_id;
        sock_id = atParamUint(pParam->params[0], &paramok);
        const char *ipaddress = atParamStr(pParam->params[1], &paramok);
        uint16_t uPort = atParamUintInRange(pParam->params[2], 0, 65535, &paramok);
        uint8_t uFlags = atParamUintInRange(pParam->params[3], 0, 6, &paramok);
        uint32_t uLength = atParamUintInRange(pParam->params[4], 1, AT_CMD_LINE_BUFF_LEN, &paramok);
        if (!paramok || uFlags == 3)
        {
            OSI_LOGI(0x10003fd6, "TSOSTF: get param error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

#if 0		
        const uint8_t *senddata = at_ParamHexData(pParam->params[5], uLength, &paramok);
#else
        const char *senddata = atParamRawText(pParam->params[5], &paramok);
#endif
        if (!paramok)
        {
            OSI_LOGI(0x10003fd7, "TSOSTF: get senddata error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (sock_id != nTcpipParas[sock_id - 1].uSocket)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (nTcpipParas[sock_id - 1].aAtEngine != pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        CFW_TCPIP_SOCKET_ADDR nDestAddr;
        nDestAddr.sin_family = CFW_TCPIP_AF_INET;
        nDestAddr.sin_port = htons(uPort);
        nDestAddr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);
        iResult = CFW_TcpipInetAddr(ipaddress);
        if (SOCKET_ERROR == iResult)
        {
            OSI_LOGI(0x10003fd8, "TSOSTF CFW_TcpipInetAddr() failed\n");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            nDestAddr.sin_addr.s_addr = iResult;
            OSI_LOGI(0x10003f7d, "dest_addr.sin_addr.s_addr send 0x%x \n", nDestAddr.sin_addr.s_addr);
            iResult = CFW_TcpipSocketSendto(sock_id, (void *)senddata, uLength, uFlags, &nDestAddr, nDestAddr.sin_len);
            if (iResult == SOCKET_ERROR)
            {

                OSI_LOGI(0x10003f7e, "UDP send socket data error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
        }

        char aucBuffer[40] = {0};
        AT_SetAsyncTimerMux(pParam->engine, 120);
        if (iResult != SOCKET_ERROR)
        {
            sprintf(aucBuffer, "%ld,%ld", sock_id, iResult);
            atCmdRespInfoText(pParam->engine, aucBuffer);
            RETURN_OK(pParam->engine);
        }
    }
    /*else if (AT_CMD_TEST == pParam->iType)
    {
        uint8_t aucBuffer[40] = {0};
        AT_Sprintf(aucBuffer, "+TSOST:(1-8),(\"(0-255).(0-255).(0-255).(0-255)\"),(0-65535),(1-1024),\"ABC...\"");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        RETURN_OK(pParam->engine);
    }*/
    else
    {
        OSI_LOGI(0x10003fd9, "TSOSTF(), command type not allowed");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_TSORF(atCommand_t *pParam)

{
    OSI_LOGI(0x100052a7, "AT+TSORF: pParam->iType = %d, pParam->nDLCI = NULL", pParam->type); //, pParam->nDLCI);

    if (AT_CMD_SET == pParam->type)
    {
        if ((pParam->param_count < 2) || (pParam->param_count > 3))
        {
            OSI_LOGI(0x10003fdb, "AT+TSORF: pPara number error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool paramok = true;
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        fd_set readfds;
        uint32_t max_waits = 30;

        int32_t sock_id = atParamUint(pParam->params[0], &paramok);
        uint32_t uLength = atParamUintInRange(pParam->params[1], 1, AT_CMD_LINE_BUFF_LEN, &paramok);
        if (pParam->param_count > 2)
        {
            max_waits = atParamUint(pParam->params[2], &paramok);
        }
        if (!paramok)
        {
            OSI_LOGI(0x10003fdc, "AT_TCPIP_CmdFunc_TSORF: get param error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        //uint32_t max_wait = max_waits * 16384;
        tv.tv_sec = max_waits;
        if (sock_id != nTcpipParas[sock_id - 1].uSocket)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (nTcpipParas[sock_id - 1].aAtEngine != pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        FD_ZERO(&readfds);
        FD_SET(sock_id, &readfds);
        //uint32_t now = (uint32_t)osiUpTime(); //hal_TimGetUpTime();
        while (1)
        {

            /*
             * This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
             * with the precedent function)
             */
            int result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

            if (result < 0)
            {
                OSI_LOGXI(OSI_LOGPAR_IS, 0x10003fdd, "Error in select(): %d %s\r\n");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else if (result > 0)
            {
                int numBytes;
                OSI_LOGI(0x10003fde, "select read event happend");

                if (FD_ISSET(sock_id, &readfds))
                {
                    struct sockaddr_storage addr;
                    socklen_t addrLen;
                    addrLen = sizeof(addr);
                    char buffer[MAX_PACKET_SIZE] = {0};
                    //We retrieve the data received
                    numBytes = recvfrom(sock_id, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

                    if (0 > numBytes)
                    {
                        OSI_LOGXI(OSI_LOGPAR_IS, 0x10003fdf, "Error in recvfrom(): %d %s\r\n");
                    }
                    else if (0 < numBytes)
                    {
#if defined(LWIP_IPV4_ON) && defined(LWIP_IPV6_ON)
                        char s[INET6_ADDRSTRLEN];
#else
#ifdef LWIP_IPV4_ON
                        char s[INET_ADDRSTRLEN];
#else
                        char s[INET6_ADDRSTRLEN];
#endif
#endif
                        in_port_t port = 0;
#ifdef LWIP_IPV4_ON
                        if (AF_INET == addr.ss_family)
                        {
                            struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                            inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
                            port = saddr->sin_port;
                        }
#endif
#ifdef LWIP_IPV6_ON
                        if (AF_INET6 == addr.ss_family)
                        {
                            struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&addr;
                            inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
                            port = saddr->sin6_port;
                        }
#endif

                        char *aucBuffer = malloc(numBytes * 2 + 120);
                        memset(aucBuffer, 0, numBytes * 2 + 120);
                        if (uLength < numBytes)
                            buffer[uLength] = 0;
                        sprintf(aucBuffer, "%ld,%s,%hu,%ld,", sock_id, s, ntohs(port), (uLength > numBytes) ? numBytes : uLength);

                        // at_CmdRespInfoText(pParam->engine, aucBuffer);
                        char hexData[3] = {0};
                        int i = 0;
                        for (i = 0; i < ((uLength > numBytes) ? numBytes : uLength); i++)
                        {
                            memset(hexData, 0, 3);
                            sprintf(hexData, "%02x", buffer[i]);

                            strcat(aucBuffer, hexData);
                        }

                        char temp[6] = {0};
                        sprintf(temp, ",%ld", uLength > numBytes ? 0 : (numBytes - uLength));
                        strcat(aucBuffer, temp);
                        atCmdRespInfoText(pParam->engine, aucBuffer);
                        free(aucBuffer);
                        RETURN_OK(pParam->engine);
                    }
                    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
                }
                else
                {
                    OSI_LOGI(0x10003fe0, "no read event happend on this socket");
                    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            else
            {
                /* timeout No need to calc time again, Select() will control the timeout itself*/

                //uint32_t past = now;
                //now = osiUpTime();
                //if (past + max_wait <= now)
                {
                    OSI_LOGI(0x10003fe1, "receive data timeout!");
                    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
                }
                //else
                //{
                //    max_wait -= (now - past);
                //}
            }
        }
    }
    /*else if (AT_CMD_TEST == pParam->iType)
    {
        uint8_t aucBuffer[40] = {0};
        AT_Sprintf(aucBuffer, "+TSORF:(1-8),(1-1024)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        RETURN_OK(pParam->engine);
    }*/
    else
    {
        OSI_LOGI(0x10003fe2, "AT_TCPIP_CmdFunc_TSORF(), command type not allowed");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_TSOCL(atCommand_t *pParam)
{
    OSI_LOGI(0x100052a8, "AT+TSOCL ... ... pParam->nDLCI= NULL"); //, pParam->nDLCI);

    if (AT_CMD_SET == pParam->type)
    {
        if ((pParam->param_count != 1))
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        bool paramok = true;
        const uint8_t sock_id = atParamUint(pParam->params[0], &paramok);

        if (!paramok)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        if (nTcpipParas[sock_id - 1].uSocket == 0 || nTcpipParas[sock_id - 1].aAtEngine != pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        int32_t iRetValue;
        iRetValue = CFW_TcpipSocketClose(nTcpipParas[sock_id - 1].uSocket);
        if (iRetValue == ERR_SUCCESS)
        {
            OSI_LOGI(0x10003fe4, "AT+TSOCL: close success");
            nTcpipParas[sock_id - 1].uSocket = 0;
            nTcpipParas[sock_id - 1].uProtocol = 0;
            nTcpipParas[sock_id - 1].uPort = 0;
            nTcpipParas[sock_id - 1].aAtEngine = NULL;
            RETURN_OK(pParam->engine);
        }
        else
        {
            OSI_LOGI(0x10003fe5, "AT+TSOCLE: CSW execute wrong");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    /*else if (AT_CMD_TEST == pParam->iType)
    {
        uint8_t *pRstStr = AT_MALLOC(20);
        if (NULL == pRstStr)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);

        AT_MemSet(pRstStr, 0, 20);
        AT_StrCpy(pRstStr, "+TSOCL:(1-8)");

        at_CmdRespInfoText(pParam->engine, pRstStr);
        AT_FREE(pRstStr);
        RETURN_OK(pParam->engine);
    }*/
    else
    {
        OSI_LOGI(0x10003fe6, "AT_TCPIP_CmdFunc_TSOCL(), command type not allowed");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

uint8_t sendIP_ICMP_Ping(SOCKET socketfd, uint32_t pingparam);

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_PING
* Description:
* Parameter:  AT_CMD_PARA *pParam
* Return:       void
* Remark:      n/a
* Author:
* Data:          2016-3-28
******************************************************************************/

void AT_TCPIP_CmdFunc_TPING(atCommand_t *pParam)
{
    OSI_LOGI(0x10003fe7, "AT+PING starting");
    //PING_TOTAL_NUM = 5;
    //PING_CID = 0;
    //PING_PACKET_LEN = 8;
    //PING_ECHO_NUM = 0;
    //PING_TIMEOUT = 1;

    atCmdEngine_t *engine = pParam->engine;

    //uint8_t i = 0;
    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;
        uint32_t packetlen = 16;
        uint32_t waitms = 1000;
        const char *ipaddress = atParamStr(pParam->params[0], &paramok);

        if (pParam->param_count > 1)
        {
            packetlen = atParamDefUintInRange(pParam->params[1], 8, 8, 1460, &paramok);
        }
        if (pParam->param_count > 2)
            waitms = atParamUintInRange(pParam->params[2], 10, 60000, &paramok);

        if (!paramok)
            RETURN_CME_ERR(engine, ERR_AT_CME_PARAM_INVALID);
        if (PING_SOCKET != INVALID_SOCKET)
        {
            OSI_LOGI(0x10003fe8, "AT_TCPIP_CmdFunc_TPING(), ping in progress\n");
            RETURN_CME_ERR(engine, ERR_AT_CME_EXE_FAIL);
        }
        if (gTPingtimer != NULL)
        {
            osiTimerDelete(gTPingtimer);
            gTPingtimer = NULL;
        }
        gTPingtimer = osiTimerCreate(osiThreadCurrent(), pingip_timeout, (void *)engine);
        if (gTPingtimer == NULL)
            atCmdRespInfoText(engine, "no memory");

        PING_PACKET_LEN = (packetlen > 16) ? packetlen : 16;
        PING_TIMEOUT = waitms;
        PING_ID = 0;
        PING_TTL = 255;
        PING_SEQNO = 0;

        ip_addr_t pcIpAddr;
        ipaddr_aton(ipaddress, &pcIpAddr);
        ip_addr_copy(PING_IP, pcIpAddr);
        OSI_LOGI(0x10003fe9, "AT_TCPIP_CmdFunc_TPING(), GOTOCFW_TcpipSocket(),PING_TIMEOUT=%d ms ;PING_PACKET_LEN=%d\n", PING_TIMEOUT, PING_PACKET_LEN);
        SOCKET socketfd = INVALID_SOCKET;
#if LWIP_IPV6
        if (IP_IS_V4(&PING_IP) || ip6_addr_isipv4mappedipv6(ip_2_ip6(&PING_IP)))
        {
            socketfd = CFW_TcpipSocketEX(AF_INET, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP, udp_rsp, (uint32_t)engine);
        }
        else
        {
            socketfd = CFW_TcpipSocketEX(AF_INET6, CFW_TCPIP_SOCK_RAW, IPPROTO_ICMPV6, udp_rsp, (uint32_t)engine);
        }
#else
        socketfd = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP, udp_rsp, (uint32_t)engine);
#endif

        if (SOCKET_ERROR == socketfd)
        {
            OSI_LOGI(0x10003fea, "AT_TCPIP_CmdFunc_TPING(), CFW_TcpipSocket() failed\n");
            RETURN_CME_ERR(engine, ERR_AT_CME_EXE_FAIL);
        }

        PING_SOCKET = socketfd;
        OSI_LOGI(0x10003feb, "socketfd = %d, PING_SOCKET =%d", socketfd, PING_SOCKET);

        //Get TTL
        int len = 1;
        if (ERR_SUCCESS != CFW_TcpipSocketGetsockopt(socketfd, CFW_TCPIP_IPPROTO_IP, IP_TTL, &PING_TTL, &len))
        {
            OSI_LOGI(0x10003fec, "AT_TCPIP_CmdFunc_PING: Using DEFAULT TTL");
            PING_TTL = 255;
        }
        OSI_LOGI(0x10003fed, "AT_TCPIP_CmdFunc_PING: AT_PING_TTL = %d", PING_TTL);

        PING_ID++;

        if (ERR_SUCCESS != sendIP_ICMP_Ping(PING_SOCKET, (uint32_t)engine))
        {
            OSI_LOGI(0x10003fee, "AT_TCPIP_CmdFunc_PING: Cannot find IP address of this modem!");
            //at_CmdRespCmeError(engine, ERR_AT_CME_PARAM_INVALID);
            char Response[50] = {0};
            sprintf(Response, "+TPINGERR:%d\r\n", ERR_SENDFAIL);
            //atCmdRespInfoText(engine, Response);
            AT_CMD_RETURN(atCmdRespErrorText(engine, Response));
        }
        PING_START_TIME = (uint32_t)osiUpTime();
        OSI_LOGI(0x10003fef, "AT_TCPIP_CmdFunc_PING: OK,PING_START_TIME=%d", PING_START_TIME);

        RETURN_OK(engine);
    }
    /*else if (AT_CMD_TEST == pParam->iType)
    {
        uint8_t *pRstStr = AT_MALLOC(85);
        if (NULL == pRstStr)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);

        AT_MemSet(pRstStr, 0, 85);
        AT_StrCpy(pRstStr, "+TPING: (\"(0-255).(0-255).(0-255).(0-255)\"), (8-1460), (10-60000)");

        at_CmdRespInfoText(pParam->engine, pRstStr);
        AT_FREE(pRstStr);
        RETURN_OK(pParam->engine);
    }*/
    else
    {
        OSI_LOGI(0x10003ff0, "AT_TCPIP_CmdFunc_TPING(), command type not allowed");
        RETURN_CME_ERR(engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

uint8_t sendIP_ICMP_Ping(SOCKET socketfd, uint32_t pingparam)
{
    struct sockaddr_storage to;
    OSI_LOGI(0x10003ff1, "sendIP_ICMP_Ping: ");
    //atCmdEngine_t *engine = (atCmdEngine_t*)pingparam;
    uint8_t *ipData = malloc(PING_PACKET_LEN);
    memset(ipData, 0, PING_PACKET_LEN);

    struct icmp_echo_hdr *idur;
    idur = (struct icmp_echo_hdr *)ipData;

    ICMPH_CODE_SET(idur, 0);
    idur->id = htons(++PING_ID);
    idur->seqno = htons(++PING_SEQNO);
    // in 16384Hz ticks.
    *((uint32_t *)(ipData + 8)) = (uint32_t)osiUpTime();
    *((uint32_t *)(ipData + 12)) = socketfd;

    for (uint16_t i = 0; i < PING_PACKET_LEN - 16; i++)
    {
        ipData[i + 16] = i;
    }

    idur->chksum = 0;
    OSI_LOGI(0x10003ff2, "AT_TCPIP_CmdFunc_PING: Start time = %d, socketid = %d, AT_PING_SEQNO = %d", *((uint32_t *)(ipData + 8)), *((uint32_t *)(ipData + 12)), PING_SEQNO);

#if LWIP_IPV4
    if (IP_IS_V4(&PING_IP))
    {
        struct sockaddr_in *to4 = (struct sockaddr_in *)&to;
        to4->sin_len = sizeof(to4);
        to4->sin_family = AF_INET;
        inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&PING_IP));
        ICMPH_TYPE_SET(idur, ICMP_ECHO);

        idur->chksum = inet_chksum(idur, PING_PACKET_LEN);
    }
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    if (IP_IS_V6(&PING_IP) && !ip6_addr_isipv4mappedipv6(ip_2_ip6(&PING_IP)))
    {
        struct sockaddr_in6 *to6 = (struct sockaddr_in6 *)&to;
        to6->sin6_len = sizeof(to6);
        to6->sin6_family = AF_INET6;
        inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(&PING_IP));
        ICMPH_TYPE_SET(idur, ICMP6_TYPE_EREQ);
    }
#endif /* LWIP_IPV6 */

    if (-1 == CFW_TcpipSocketSendto(socketfd, ipData, PING_PACKET_LEN, 0, (CFW_TCPIP_SOCKET_ADDR *)&to, sizeof(to)))

    {
        CFW_TcpipSocketClose(PING_SOCKET);
        PING_SOCKET = INVALID_SOCKET;
        free(ipData);
        return -1;
    }

    osiTimerStart(gTPingtimer, PING_TIMEOUT * 1000);

    free(ipData);

    return ERR_SUCCESS;
}

void pingip_continue(uint32_t pingparam)
{
    atCmdEngine_t *engine = (atCmdEngine_t *)pingparam;
    OSI_LOGI(0x10003ff3, "pingip_continue");
    uint8_t nRetValue = sendIP_ICMP_Ping(PING_SOCKET, pingparam);
    if (nRetValue != ERR_SUCCESS)
    {
        OSI_LOGI(0x10003ff4, "ERROR: AT_SendIP_ICMP_Ping Return Failed!");
        CFW_TcpipSocketClose(PING_SOCKET);
        PING_SOCKET = INVALID_SOCKET;
        //at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        char Response[50] = {0};
        sprintf(Response, "+TPINGERR:%d\r\n", ERR_SENDFAIL);
        atCmdRespUrcText(engine, Response);
    }
}

void pingip_timeout(void *param)
{
    char Response[50] = {0};
    atCmdEngine_t *engine = (atCmdEngine_t *)param;
    uint32_t time = (uint32_t)osiUpTime();
    OSI_LOGI(0x10003ff5, "Request timed out.time last %d,PING_TIMEOUT=%d\r\n", (time - PING_START_TIME) * 1000 / 16384, PING_TIMEOUT);
    if ((time - PING_START_TIME) * 1000 / 16384 < PING_TIMEOUT)
    {

        pingip_continue((uint32_t)engine);
    }
    else
    {
        osiTimerStop(gTPingtimer);
        osiTimerDelete(gTPingtimer);
        gTPingtimer = NULL;
        CFW_TcpipSocketClose(PING_SOCKET);
        PING_SOCKET = INVALID_SOCKET;
        OSI_LOGI(0x10003ff6, "+TPINGERR:%d;PING_SOCKET=%d", ERR_TIMEOUT, PING_SOCKET);
        sprintf(Response, "+TPINGERR:%d\r\n", ERR_TIMEOUT);
        atCmdRespUrcText(engine, Response);
    }
}
#endif
#endif
