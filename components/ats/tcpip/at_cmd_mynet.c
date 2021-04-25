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
#include "ats_config.h"
#include "cfw.h"

#ifdef CONFIG_AT_MYNET_TCPIP_SUPPORT
#include "osi_api.h"
#include "osi_log.h"
#include "stdio.h"
#include "string.h"
#include "at_response.h"
#include "at_cfw.h"
#include "at_engine.h"
#include "at_command.h"
#include "cfw.h"
#include "at_cmd_mynet.h"
#include "at_cfg.h"
#include "sockets.h"
#include "at_cfg.h"
#include "ppp_interface.h"

#define AT_MYNET_TCPIP_ADDRLEN 20
#define AT_MYNET_TCPIP_HOSTLEN 256
#define CHAR_BACKSPACE 8

typedef enum
{
    //MYNET_INITIAL,
    //MYNET_START,
    //MYNET_CONFIG,
    //MYNET_GPRSACT,
    // MYNET_STATUS,
    MYNET_CLOSED,
    MYNET_CLOSING,
    MYNET_PROCESSING,
    MYNET_CONNECTED,
    MYNET_ACCEPT,
    //MYNET_PDPDEACT
} MYNET_STATUS_T;

typedef enum
{
    MYNETSERVER_CLOSE,
    MYNETSERVER_OPEN,
    MYNETSERVER_OPENING
} MYNET_SERVER_STATE_T;

typedef enum
{
    BEARER_GPRS_DEF,
    BEARER_GPRS_2,
    BEARER_WIFI,
    BEARER_MAX_NUM,
} MYNET_BEARER_T;

typedef struct
{
    uint16_t tcp_port;
    uint16_t udp_port;
} CLOCAL_PORT;

typedef struct
{
    ip4_addr_t ip;

} CLOCAL_IP;

typedef struct
{
    char *recvBuf;
    uint32_t readnum;
    uint32_t recvBufLen;
    uint32_t recvBufMaxSize;
} CM_RECV_BUF_T;

typedef struct _stAT_Tcpip_Paras
{
    uint8_t uType;
    uint8_t uProtocol;
    uint8_t uSocket;
    uint8_t uDomain;
    uint16_t uPort;
    uint8_t uaIPAddress[AT_MYNET_TCPIP_HOSTLEN];
    uint8_t udpmode; // 0: udp normal mode 1: udp extended mode 2: set udp address
    MYNET_STATUS_T uState;
    MYNET_SERVER_STATE_T uServerState;
    MYNET_BEARER_T uConnectBearer;
    uint32_t sendSize;
    uint32_t sentSize;
    uint8_t isText;
    uint8_t uMode;
    uint8_t uCid;
    CLOCAL_PORT uLocalport;
    uint8_t uLocalip[AT_MYNET_TCPIP_HOSTLEN];
    uint8_t isRefuse;
    uint8_t isTrans;
    osiTimer_t *sendTimerID;
    CM_RECV_BUF_T uRecvBuf;
    CFW_TCPIP_SOCKET_ADDR from_addr;
    uint8_t listening_socket;
    struct atCmdEngine *engine;
    uint8_t isOnlySocketClose;
    osiTimer_t *sendTOtimer;
    uint8_t isSendTimeout;
} stAT_Tcpip_Paras;

typedef struct
{
    uint8_t isValid;
    char ipAddr[AT_MYNET_TCPIP_ADDRLEN];
    char netMask[AT_MYNET_TCPIP_ADDRLEN];
} stAT_IP_FILTER;

typedef struct
{
    uint8_t nMuxIndex;
    atDispatch_t *dispatch;

} stAT_Trans_Paras;

typedef struct
{
    uint8_t nCid;
    MYNET_STATUS_T nState;
} stAT_Bearer_Paras;

typedef struct
{
    uint8_t mode;
    uint8_t channelid;
    uint8_t nSocketId;
    MYNET_BEARER_T nConnectBearer;
    uint32_t port;
} MYNETSERVER;

#define CHANNEL_NUM_MAX 6

typedef struct
{
    stAT_Tcpip_Paras nTcpipParas[CHANNEL_NUM_MAX];
    stAT_Bearer_Paras nBearerParas[BEARER_MAX_NUM]; //0,1:GPRS 2:WIFI
} MYNET_CONTEXT_T;

typedef struct
{
    uint8_t act_state;
    uint8_t cid_count;
    uint8_t cid_index;
    uint8_t cids[AT_PDPCID_MAX];
} cgactAsyncCtx_t;

typedef struct
{
    uint32_t uMuxIndex;
    uint32_t pos;
    uint32_t size;
    uint8_t *data;
} tcpipAsyncCtx_t;

typedef struct tcpip_callback_param
{
    atCmdEngine_t *engine;
    uint8_t nMuxIndex;
} tcpip_callback_param_t;

typedef struct
{
    char *buff;        // buffer, shared by all mode
    uint32_t buffLen;  // existed buffer data byte count
    uint32_t buffSize; // buffer size
    osiMutex_t *mutex;
} BYPASS_BUFFER_T;

#define AT_StrCpy(des, src) strcpy((char *)des, (const char *)src)
#define AT_StrNCpy(des, src, len) strncpy((char *)des, (const char *)src, (size_t)len)
#define AT_MemZero(str, len) memset((str), 0, (len))
#define AT_StrLen(str) strlen((const char *)(str))

extern struct netif *getGprsNetIf(uint8_t nSim, uint8_t nCid);
extern struct netif *getEtherNetIf(uint8_t nCid);
CFW_SIM_STATUS CFW_GetSimStatus(CFW_SIM_ID nSimID);
extern uint32_t CFW_GprsRemovePdpCxt(uint8_t nCid, CFW_SIM_ID nSimID);
extern uint32_t CFW_SetDnsServerbyPdp(uint8_t nCid, uint8_t nSimID, uint8_t *Pro_DnsIp, uint8_t *Sec_DnsIp);
extern uint32_t *getDNSServer(uint8_t nCid, uint8_t nSimID);
extern osiThread_t *netGetTaskID();
extern void drvTurnOnSocketLed(signed int value);

MYNET_CONTEXT_T g_uMynetContexts;
MYNET_BEARER_T gMynetBearer = BEARER_GPRS_DEF; //0,1:GPRS 2:WIFI
MYNETSERVER gMynetserver;
CLOCAL_PORT gMynetlocalport[CHANNEL_NUM_MAX] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
CLOCAL_IP gMynetlocalip[CHANNEL_NUM_MAX];

//keepalive Parameters
static int g_keepalive = 0;
static int g_keepidle = 7200;
static int g_keepinterval = 75;
static int g_keepcount = 9;
uint32_t g_mynetUrc = 1;
uint8_t g_maxclientnum = 5;

struct atCmdEngine *g_engine = NULL;

typedef struct _channel_param
{
    uint32_t trans_waittm;
    uint32_t trans_sendSz;
} channel_param;

static channel_param gChannelParam[CHANNEL_NUM_MAX] = {{100, 1024}, {100, 1024}, {100, 1024}, {100, 1024}, {100, 1024}, {100, 1024}};

static at_tcpip_setting_t gMYNETSettings = {1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 5, 100, 1024, 1, 0, 1460, 50, 0, 0, 3, 1, 200, 300, 1, "CMNET", {0}, {0}};

#define mynetSCONT_unSaved gMYNETSettings.cipSCONT_unSaved
#define mynetMux_multiIp gMYNETSettings.cipMux_multiIp
#define mynetQSEND_quickSend gMYNETSettings.cipQSEND_quickSend
#define mynetRXGET_manualy gMYNETSettings.cipRXGET_manualy
#define mynetMODE_transParent gMYNETSettings.cipMODE_transParent // 0: normal mode 1: transparent mode
#define mynetCCFG_esc gMYNETSettings.cipCCFG_esc                 // wether handle escape character; 0: no 1: yes
#define mynetCCFG_NmRetry gMYNETSettings.cipCCFG_NmRetry         // 3 - 8
#define mynetCCFG_WaitTm gMYNETSettings.cipCCFG_WaitTm           // 1 - 10
#define mynetCCFG_SendSz gMYNETSettings.cipCCFG_SendSz           // 1 - 1460
#define mynetCCFG_Rxmode gMYNETSettings.cipCCFG_Rxmode           //0: output data to serial port without interval  1: within <Rxtimer> interval
#define mynetCCFG_RxSize gMYNETSettings.cipCCFG_RxSize           //50 - 1460
#define mynetCCFG_Rxtimer gMYNETSettings.cipCCFG_Rxtimer         //20 - 1000
#define mynetATS_setTimer gMYNETSettings.cipATS_setTimer         // 0: not set timer 1: set timer
#define mynetATS_time gMYNETSettings.cipATS_time                 // value of data send timer
#define mynetSPRT_sendPrompt gMYNETSettings.cipSPRT_sendPrompt   // prompt for IPSEND, 0: no ">" but "ok"  1: ">" + "ok"  2: no ">", no "ok"
#define mynetHEAD_addIpHead gMYNETSettings.cipHEAD_addIpHead     // MYNETHEAD, 0: not add; 1: add
#define mynetSRIP_showIPPort gMYNETSettings.cipSRIP_showIPPort   // MYNETSRIP 0:don't disp 1:disp
#define mynetCSGP_setGprs gMYNETSettings.cipCSGP_setGprs         // 1: GPRS(Default) 0: CSD
#define mynetApn gMYNETSettings.cipApn
#define mynetUserName gMYNETSettings.cipUserName
#define mynetPassWord gMYNETSettings.cipPassWord
#define mynetDPDP_detectPDP gMYNETSettings.cipDPDP_detectPDP // 0: don't detect pdp 1: detect pdp
#define mynetDPDP_interval gMYNETSettings.cipDPDP_interval   // 1 < interval <= 180(s)
#define mynetDPDP_timer gMYNETSettings.cipDPDP_timer         // 1 < timer <= 10
#define mynetSHOWTP_dispTP gMYNETSettings.cipSHOWTP_dispTP   // 0: disabled 1: enabled

#define MYNET_NUM_NETCONN 6
#define RECV_BUF_UNIT_LEN 1024

uint8_t mynetCCFG_Auth = 1;
#define ATTACH_TYPE (1 /*API_PS_ATTACH_GPRS*/ | 8 /*API_PS_ATTACH_FOR*/)
#define ERR_SUCCESS 0
#define AT_CMD_LINE_BUFF_LEN 1024 * 5

// uint8_t g_uATTcpipValid;
static const uint8_t g_strATTcpipStatus[10][32] = {"IP INITIAL", "IP START", "IP CONFIG", "IP GPRSACT",
                                                   "IP STATUS", "IP PROCESSING", "CONNECT OK", "IP CLOSING",
                                                   "CLOSED", "IP PDPDEACT"};
#define IPFILTER_NUM_MAX 5

stAT_IP_FILTER g_ipFilter[IPFILTER_NUM_MAX];

static BYPASS_BUFFER_T *g_mynetbypass_buf = NULL;

uint8_t g_socketLed = 1;

BYPASS_BUFFER_T *at_mynetTCPIPBufCreate()
{
    BYPASS_BUFFER_T *bypass_buff = (BYPASS_BUFFER_T *)malloc(sizeof(*bypass_buff));
    if (bypass_buff == NULL)
        return NULL;
    bypass_buff->buff = malloc(AT_CMD_LINE_BUFF_LEN);
    if (bypass_buff->buff == NULL)
    {
        free(bypass_buff);
        return NULL;
    }
    bypass_buff->buffLen = 0;
    bypass_buff->buffSize = AT_CMD_LINE_BUFF_LEN;
    bypass_buff->mutex = osiMutexCreate();
    return bypass_buff;
}
void at_mynetTCPIPBufDestroy(BYPASS_BUFFER_T *bypass_buff)
{
    if (bypass_buff == NULL)
        return;
    osiMutexDelete(bypass_buff->mutex);
    free(bypass_buff->buff);
    free(bypass_buff);
}
#if 0

static void at_delayAsync(void *param)
{

    atCmdEngine_t *engine = (atCmdEngine_t *)param;
    if (g_mynetbypass_buf == NULL)
    {
        OSI_LOGI(0, "at_delayAsync error,param == null");
        at_StartCallbackTimer(mynetATS_time * 1000, at_delayAsync, param);
        return;
    }
    OSI_LOGI(0, "at_delayAsync,engine=%x,th->len=%d", engine, g_mynetbypass_buf->buffLen);
    at_ModuleRunCommandExtra(engine, g_mynetbypass_buf->buff, g_mynetbypass_buf->buffLen);

}
#endif
static int sBypassDataSendAll = 0;

static void mynet_bypassDataSend(void *param)
{
    OSI_LOGI(0, "bypassDataSend enter");
    stAT_Trans_Paras *trans = (stAT_Trans_Paras *)param;
    atDispatch_t *dispatch = trans->dispatch;
    stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[trans->nMuxIndex]);
    if (!atDispatchIsDataMode(dispatch))
    {
        OSI_LOGI(0, "bypassDataSend error,not data mode");
        goto restart;
    }
    if (g_mynetbypass_buf == NULL)
    {
        OSI_LOGI(0, "mynet_bypassDataSend error,g_mynetbypass_buf == null");
        goto restart;
    }
    if (g_mynetbypass_buf->buffLen < gChannelParam[tcpipParas->uCid].trans_sendSz && g_mynetbypass_buf->buffLen != 0)
    {
        sBypassDataSendAll++;
    }
    if (g_mynetbypass_buf->buffLen < gChannelParam[tcpipParas->uCid].trans_sendSz && sBypassDataSendAll < 2)
    {
        OSI_LOGI(0, "mynet_bypassDataSend do nothing buffLen=%d", g_mynetbypass_buf->buffLen);
        goto restart;
    }
    else
    {
        uint16_t sendLen = gChannelParam[tcpipParas->uCid].trans_sendSz > g_mynetbypass_buf->buffLen ? g_mynetbypass_buf->buffLen : gChannelParam[tcpipParas->uCid].trans_sendSz;
        uint8_t *data = malloc(sendLen);
        if (data == NULL)
        {
            OSI_LOGI(0, "mynet_bypassDataSend MALLOC FAILS mynetCCFG_SendSz=%d", sendLen);
            return;
        }
        memset(data, 0, sendLen);
        memcpy(data, g_mynetbypass_buf->buff, sendLen);
        osiMutexLock(g_mynetbypass_buf->mutex);
        memmove(g_mynetbypass_buf->buff, g_mynetbypass_buf->buff + sendLen, g_mynetbypass_buf->buffLen - sendLen);
        g_mynetbypass_buf->buffLen -= sendLen;
        osiMutexUnlock(g_mynetbypass_buf->mutex);
        //stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[0]);
        if (CFW_TcpipSocketSend(tcpipParas->uSocket, data, sendLen, 0) == SOCKET_ERROR)
        {
            OSI_LOGI(0, "TCPIP send socket data error");
        }
        free(data);
        sBypassDataSendAll = 0;
    }
restart:
    //if (atDispatchIsDataMode(dispatch))
    osiTimerStart(tcpipParas->sendTimerID, gChannelParam[tcpipParas->uCid].trans_waittm);
}

static int mynet_bypassDataRecv(void *param, const void *data, size_t length)
{
    OSI_LOGI(0, "mynet_bypassDataRecv,length=%d ", length);
    if (g_mynetbypass_buf == NULL)
        g_mynetbypass_buf = at_mynetTCPIPBufCreate();
    if (g_mynetbypass_buf == NULL)
        return 0;
    osiMutexLock(g_mynetbypass_buf->mutex);
    uint8_t *data_u8 = (uint8_t *)data;
    size_t length_input = length;
    while (length > 0)
    {
        uint8_t c = *data_u8++;
        length--;

        // Remove previous byte for BACKSPACE

        if (c == CHAR_BACKSPACE)
        {
            if (g_mynetbypass_buf->buffLen > 0)
                --g_mynetbypass_buf->buffLen;
            continue;
        }
        // Drop whole buffer at overflow, maybe caused by data error
        // or buffer too small
        if (g_mynetbypass_buf->buffLen >= g_mynetbypass_buf->buffSize)
        {
            OSI_LOGI(0, "mynet_bypassDataRecv bypass mode overflow, drop all");
            g_mynetbypass_buf->buffLen = 0;
        }
        g_mynetbypass_buf->buff[g_mynetbypass_buf->buffLen++] = c;
    }
    osiMutexUnlock(g_mynetbypass_buf->mutex);

    return length_input;
}

/*static uint8_t *i8tostring(uint8_t value)
{
    static uint8_t buff[4];
    uint8_t b, s, g, i = 0;
    b = value / 100;
    s = value % 100 / 10;
    g = value % 100 % 10;
    if (b > 0)
        buff[i++] = '0' + b;
    if (b + s > 0)
        buff[i++] = '0' + s;
    buff[i++] = '0' + g;
    buff[i] = 0;
    return buff;
}*/

#define DEF_TCPIP 0
#define TCPIP_PARAMS(muxid) (&g_uMynetContexts.nTcpipParas[muxid])

static inline void set_udpmode(uint8_t muxid, uint16_t mode)
{
    TCPIP_PARAMS(muxid)->udpmode = mode;
}
static inline void update_udp_remote_socket(uint8_t muxid, uint8_t *ipaddr, uint8_t iplen, uint16_t port)
{
    memset(TCPIP_PARAMS(muxid)->uaIPAddress, 0, sizeof(TCPIP_PARAMS(muxid)->uaIPAddress));
    memcpy(TCPIP_PARAMS(muxid)->uaIPAddress, ipaddr, iplen);
    TCPIP_PARAMS(muxid)->uPort = port;
}
stAT_Trans_Paras transParas;

static void transparentModeSwitchHandler(atDispatch_t *th, atModeSwitchCause_t cause)
{
    atCmdEngine_t *engine = atDispatchGetCmdEngine(th);
    switch (cause)
    {
    case AT_MODE_SWITCH_DATA_ESCAPE:
        if (engine != NULL)
            atCmdRespInfoText(engine, "OK");
        break;
    case AT_MODE_SWITCH_DATA_RESUME:
        if (engine != NULL)
            atCmdRespIntermCode(engine, CMD_RC_CONNECT);
        break;
    case AT_MODE_SWITCH_DATA_START:
        if (engine != NULL)
            atCmdRespInfoText(engine, "CONNECT");
        break;
    default:
        break;
    }
}

static void start_transparent_mode(atCmdEngine_t *engine, uint8_t nMuxIndex)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[nMuxIndex]);

    transParas.nMuxIndex = nMuxIndex;
    transParas.dispatch = dispatch;
    atDispatchSetModeSwitchHandler(dispatch, transparentModeSwitchHandler);
    atEngineModeSwitch(AT_MODE_SWITCH_DATA_START, dispatch);
    tcpipParas->sendTimerID = osiTimerCreate(osiThreadCurrent(), mynet_bypassDataSend, (void *)&transParas);
    osiTimerStart(tcpipParas->sendTimerID, gChannelParam[tcpipParas->uCid].trans_waittm);
    atDataEngine_t *dataEngine = atDispatchGetDataEngine(dispatch);
    atDataSetBypassMode(dataEngine, mynet_bypassDataRecv, (void *)engine);
}

static void stop_transparent_mode(atCmdEngine_t *engine, uint8_t nMuxIndex)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[nMuxIndex]);
    atDispatchSetModeSwitchHandler(dispatch, NULL);
    atEngineModeSwitch(AT_MODE_SWITCH_DATA_END, dispatch);
    if (tcpipParas->sendTimerID != NULL)
    {
        osiTimerStop(tcpipParas->sendTimerID);
        osiTimerDelete(tcpipParas->sendTimerID);
        tcpipParas->sendTimerID = NULL;
    }
    at_mynetTCPIPBufDestroy(g_mynetbypass_buf);
    g_mynetbypass_buf = NULL;
}

/*****************************************************************************
* Name:         tcpip_rsp
* Description:  AT TCPIP module asynchronous event dispatch and process;
* Parameter:    COS_EVENT *pEvent
* Return:       void
* Remark:       n/a
* Author:       YangYang
* Data:         2008-5-15
******************************************************************************/
static int32_t AT_TCPIP_Connect(atCmdEngine_t *engine, uint8_t nMuxIndex);
static int32_t AT_TCPIP_ServerStart(atCmdEngine_t *engine, uint8_t uMuxIndex);
//static void start_tcpip_close_timer(atCmdEngine_t *engine, uint8_t nMuxIndex);
//static void stop_tcpip_close_timer(atCmdEngine_t *engine, uint8_t nMuxIndex);
uint32_t CFW_getDnsServerbyPdp(uint8_t nCid, uint8_t nSimID);
extern void _dnsAddressToStr(const CFW_GPRS_PDPCONT_INFO_V2 *pdp_cont, char *str);

typedef struct tcpip_dns_param
{
    atCmdEngine_t *engine;
    uint8_t nMuxIndex;
} TCPIP_DNS_PARAM;

static void tcpip_dnsReq_callback(void *paramCos)
{
    osiEvent_t *ev = (osiEvent_t *)paramCos;
    TCPIP_DNS_PARAM *param = (TCPIP_DNS_PARAM *)ev->param3;
    char aucBuffer[40] = {0};
    uint8_t nMuxIndex = param->nMuxIndex;
    atCmdEngine_t *engine = param->engine;
    OSI_LOGI(0, "tcpip_dnsReq_callback(), CFW_GethostbynameEX() tcpip_dnsReq_callback\n");
    if (ev->id == EV_CFW_DNS_RESOLV_SUC_IND)
    {
        int32_t iResult = 0;
        ip_addr_t *ipaddr = (ip_addr_t *)ev->param1;
        char *pcIpAddr = ipaddr_ntoa(ipaddr);
        stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[nMuxIndex]);
        AT_StrNCpy(tcpipParas->uaIPAddress, pcIpAddr, AT_MYNET_TCPIP_HOSTLEN);
        tcpipParas->uaIPAddress[AT_MYNET_TCPIP_HOSTLEN - 1] = '\0';
        iResult = AT_TCPIP_Connect(engine, nMuxIndex);
        if (iResult != ERR_SUCCESS)
        {
            tcpipParas->engine = NULL;
            if (!mynetMux_multiIp)
                sprintf(aucBuffer, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
            else
            {
                if (iResult == -3)
                    sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_TCPIP_ALREADY_CONNECTED);
                else
                    sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
            }
            atCmdRespErrorText(engine, aucBuffer);
        }
    }
    else if (ev->id == EV_CFW_DNS_RESOLV_ERR_IND)
    {
        stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[nMuxIndex]);
        if (!mynetMux_multiIp)
        {
            sprintf(aucBuffer, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
        }
        else
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_DNS_FAILED);
        }
        atCmdRespErrorText(engine, aucBuffer);
        tcpipParas->uState = MYNET_CLOSED;
        tcpipParas->engine = NULL;
    }
    free(param);
    free(ev);
}

static void writeInfoNText(atCmdEngine_t *engine, const char *text, unsigned length)
{

    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    ;
    if (atDispatchIsDataMode(dispatch) && atDispatchGetDataEngine(dispatch) != NULL)
    {
        OSI_LOGI(0, "writeInfoNText, dataMode,length = %d", length);
        atDataWrite(atDispatchGetDataEngine(dispatch), text, length);
    }
    else

        atCmdRespInfoNText(engine, text, length);
}

static int8_t recv_data(uint8_t uSocket, uint32_t uDataSize, atCmdEngine_t *engine, uint8_t nMuxIndex, uint8_t mode)
{
    int32_t iResult = 0;
    uint16_t uIPHlen = 0;
    char *pData = NULL;
    char uaRspStr[60] = {
        0,
    };
    stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[nMuxIndex]);
    pData = malloc(uDataSize + 25);
    if (NULL == pData)
    {
        OSI_LOGI(0x10003f47, "EV_CFW_TCPIP_REV_DATA_IND, memory error");
        sprintf(uaRspStr, "ERROR:%d", ERR_AT_CME_BUFFER_FULL);
        //atCmdRespErrorText(engine, uaRspStr);
        return -1;
    }
    AT_MemZero(pData, uDataSize + 25);
    if (mode == 0 && mynetSRIP_showIPPort && (tcpipParas->uProtocol != CFW_TCPIP_IPPROTO_UDP))
    {
        if (!mynetMux_multiIp)
        {
            sprintf(uaRspStr, "+RECV FROM:%s:%d", tcpipParas->uaIPAddress, tcpipParas->uPort);
        }
        else
        {
            sprintf(uaRspStr, "+RECEIVE,%d,%ld,%s:%d", nMuxIndex, uDataSize, tcpipParas->uaIPAddress, tcpipParas->uPort);
        }
        writeInfoNText(engine, (const char *)uaRspStr, strlen(uaRspStr));
    }
    if (mode != 4)
    {
        if (mynetHEAD_addIpHead)
        {
            if (!mynetMux_multiIp)
            {
                if (mynetSHOWTP_dispTP)
                {
                    char *tp = tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_TCP ? "TCP" : "UDP";
                    sprintf(pData, "+IPD,%ld,%s:", uDataSize, tp);
                }
                else
                {
                    sprintf(pData, "+IPD,%ld:", uDataSize);
                }
                uIPHlen = strlen(pData);
            }
            else
            {
                sprintf(pData, "+RECEIVE,%d,%ld:", nMuxIndex, uDataSize);
                uIPHlen = strlen((const char *)pData);
            }
        }
        CFW_TCPIP_SOCKET_ADDR from_addr = {
            0,
        };
        if (mode <= 1)
        {
            if (tcpipParas->uProtocol != CFW_TCPIP_IPPROTO_UDP)
            {
                iResult = CFW_TcpipSocketRecv(uSocket, (uint8_t *)(pData + uIPHlen), uDataSize, 0);
                if (SOCKET_ERROR == iResult)
                {
                    OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND, CFW_TcpipSocketRecv error");
                    free(pData);
                    return -1;
                }
            }
            else
            {
                int fromLen = sizeof(CFW_TCPIP_SOCKET_ADDR);
                int udp_total_len = 0;
                int udp_want_len = uDataSize;
                while (udp_total_len < uDataSize)
                {
                    iResult = CFW_TcpipSocketRecvfrom(uSocket, (uint8_t *)(pData + uIPHlen + udp_total_len), udp_want_len, 0, &from_addr, &fromLen);
                    if (SOCKET_ERROR == iResult)
                    {
                        OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND, CFW_TcpipSocketRecv error");
                        free(pData);
                        return -1;
                    }
                    if (0 == iResult)
                        break;
                    udp_total_len += iResult;
                    udp_want_len -= iResult;
                }
                iResult = udp_total_len;
                if (mynetSRIP_showIPPort)
                {
                    if (!mynetMux_multiIp)
                    {
                        sprintf(uaRspStr, "+RECV FROM:%s:%hu", inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port));
                    }
                    else
                    {
                        sprintf(uaRspStr, "+RECEIVE,%d,%ld,%s:%hu", nMuxIndex, uDataSize, inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port));
                    }
                    writeInfoNText(engine, (const char *)uaRspStr, strlen((const char *)uaRspStr));
                }
            }
        }
        if (mode == 1)
        {
            if (tcpipParas->uRecvBuf.recvBuf == NULL)
            {
                OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND, tcpipParas->uRecvBuf.recvBuf == NULL");
                tcpipParas->uRecvBuf.recvBuf = malloc(RECV_BUF_UNIT_LEN);
                if (tcpipParas->uRecvBuf.recvBuf == NULL)
                {
                    OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND, CFW_TcpipSocketRecv malloc error");
                    return -1;
                }
                AT_MemZero(tcpipParas->uRecvBuf.recvBuf, RECV_BUF_UNIT_LEN);
                tcpipParas->uRecvBuf.readnum = 0;
                tcpipParas->uRecvBuf.recvBufLen = 0;
                tcpipParas->uRecvBuf.recvBufMaxSize = RECV_BUF_UNIT_LEN;
            }
            OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND, udp cache mode tcpipParas->uRecvBuf.recvBufMaxSize=%d,tcpipParas->uRecvBuf.recvBufLen=%d", tcpipParas->uRecvBuf.recvBufMaxSize, tcpipParas->uRecvBuf.recvBufLen);
            if ((tcpipParas->uRecvBuf.readnum + tcpipParas->uRecvBuf.recvBufLen + iResult) <= tcpipParas->uRecvBuf.recvBufMaxSize)
            {

                memcpy(tcpipParas->uRecvBuf.recvBuf + tcpipParas->uRecvBuf.readnum + tcpipParas->uRecvBuf.recvBufLen, pData + uIPHlen, iResult);
                tcpipParas->uRecvBuf.recvBufLen += iResult;
                tcpipParas->from_addr = from_addr;
            }
            else
            {
                char *recvBufndi = malloc(tcpipParas->uRecvBuf.recvBufLen + iResult);
                if (recvBufndi == NULL)
                {
                    OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND,CFW_TcpipSocketRecv recvBufndi malloc error");
                    return -1;
                }
                memcpy(recvBufndi, tcpipParas->uRecvBuf.recvBuf + tcpipParas->uRecvBuf.readnum, tcpipParas->uRecvBuf.recvBufLen);
                memcpy(recvBufndi + tcpipParas->uRecvBuf.recvBufLen, pData + uIPHlen, iResult);
                tcpipParas->from_addr = from_addr;
                free(tcpipParas->uRecvBuf.recvBuf);
                tcpipParas->uRecvBuf.recvBuf = recvBufndi;
                tcpipParas->uRecvBuf.readnum = 0;
                tcpipParas->uRecvBuf.recvBufMaxSize = tcpipParas->uRecvBuf.recvBufLen + iResult;
                tcpipParas->uRecvBuf.recvBufLen = tcpipParas->uRecvBuf.recvBufMaxSize;
            }
            //sprintf(uaRspStr, "+READ:%d,%d,%d", nMuxIndex,iResult,tcpipParas->uRecvBuf.recvBufLen);
            //atCmdRespUrcText(engine, uaRspStr);
        }
        if (mode == 2 || mode == 3)
        {
            char uaIpStr[30] = {
                0,
            };
            if (!mynetMux_multiIp)
            {
                //sprintf(uaRspStr, "+MYNETRXGET:%d,%ld,%ld", mode, uDataSize, iResult);
            }
            else
            {
                // sprintf(uaRspStr, "+MYNETREAD:%d,%ld", nMuxIndex, iResult);
            }
            if (mynetSRIP_showIPPort)
            {
                sprintf(uaIpStr, ",%s:%d", tcpipParas->uaIPAddress, tcpipParas->uPort);
                strcat(uaRspStr, uaIpStr);
            }
            writeInfoNText(engine, (const char *)uaRspStr, strlen((const char *)uaRspStr));
        }
        if (mode == 0)
        {
            OSI_LOGI(0, "mode=%d", mode);
            //sprintf(uaRspStr, "$MYNETREAD:%d,%ld\r\n", nMuxIndex, iResult);
            //writeInfoNText(engine, uaRspStr, strlen(uaRspStr));
            writeInfoNText(engine, (const char *)pData, uIPHlen + iResult);
        }
        if (mode == 2)
        {
            int readlen = 0;
            // writeInfoNText(engine, pData, uIPHlen + iResult);
            if (tcpipParas->uRecvBuf.recvBuf != NULL)
            {
                OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETRD, len = %d,tcpipParas->uRecvBuf.recvBufLen=%d", uDataSize, tcpipParas->uRecvBuf.recvBufLen);
                if (uDataSize < tcpipParas->uRecvBuf.recvBufLen)
                {
                    readlen = uDataSize;
                }
                else
                {
                    readlen = tcpipParas->uRecvBuf.recvBufLen;
                }
                sprintf(uaRspStr, "$MYNETREAD:%d,%d", nMuxIndex, readlen);

                writeInfoNText(engine, (const char *)uaRspStr, strlen((const char *)uaRspStr));
                writeInfoNText(engine, (const char *)pData, uIPHlen);
                writeInfoNText(engine, (const char *)(tcpipParas->uRecvBuf.recvBuf + tcpipParas->uRecvBuf.readnum), readlen);
                tcpipParas->uRecvBuf.readnum += readlen;
                tcpipParas->uRecvBuf.recvBufLen -= readlen;

                if (tcpipParas->uRecvBuf.recvBufLen == 0)
                {
                    OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETRD, len = %d,tcpipParas->uRecvBuf.recvBufLen=%d", readlen, tcpipParas->uRecvBuf.recvBufLen);
                    free(tcpipParas->uRecvBuf.recvBuf);

                    tcpipParas->uRecvBuf.recvBuf = NULL;
                    tcpipParas->uRecvBuf.readnum = 0;
                    //tcpipParas->uRecvBuf.recvBufLen = 0;
                    tcpipParas->uRecvBuf.recvBufMaxSize = 0;
                }
                else
                {
                    OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETRD,there have data left");
                }
                //}
            }
            else
            {
                OSI_LOGI(0, "recv_data,there is no data in buff");
                sprintf(uaRspStr, "$MYNETREAD:%d,0", nMuxIndex);
                writeInfoNText(engine, (const char *)uaRspStr, strlen((const char *)uaRspStr));
                //atCmdRespOK(engine);
            }
        }
        else if (mode == 3)
        {
            int readlen = 0;
            if (tcpipParas->uRecvBuf.recvBuf != NULL)
            {
                OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETRD, len = %d,tcpipParas->uRecvBuf.recvBufLen=%d", uDataSize, tcpipParas->uRecvBuf.recvBufLen);
                if (uDataSize < tcpipParas->uRecvBuf.recvBufLen)
                {
                    readlen = uDataSize;
                }
                else
                {
                    readlen = tcpipParas->uRecvBuf.recvBufLen;
                }
                sprintf(uaRspStr, "$MYNETREAD:%d,%d", nMuxIndex, readlen);

                writeInfoNText(engine, (const char *)uaRspStr, strlen((const char *)uaRspStr));
                //if (uDataSize < tcpipParas->uRecvBuf.recvBufLen){
                uint8_t *pHexData = malloc(2 * readlen + 25);
                if (pHexData == NULL)
                {
                    OSI_LOGI(0x10003f49, "recv_data malloc fails for hex data");
                    char AtRet[128] = {0};
                    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_BUFFER_FULL);
                    //atCmdRespErrorText(engine, AtRet);
                    free(pData);
                    return -1;
                }
                AT_MemZero(pHexData, 2 * readlen + 25);
                memcpy(pHexData, pData, uIPHlen);
                for (int i = 0; i < readlen; i++)
                {
                    char tmp[3] = {0};
                    sprintf(tmp, "%02x", (tcpipParas->uRecvBuf.recvBuf + tcpipParas->uRecvBuf.readnum)[i]);
                    strcat((char *)pHexData, tmp);
                }
                OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETRD,uIPHlen=%d,pHexData=%s", uIPHlen, pHexData);
                writeInfoNText(engine, (const char *)pHexData, uIPHlen + 2 * readlen);
                free(pHexData);

                tcpipParas->uRecvBuf.readnum += readlen;
                tcpipParas->uRecvBuf.recvBufLen -= readlen;
                //atCmdRespOK(engine);

                if (tcpipParas->uRecvBuf.recvBufLen == 0)
                {
                    OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETRD, len = %d,tcpipParas->uRecvBuf.recvBufLen=%d", readlen, tcpipParas->uRecvBuf.recvBufLen);
                    if (tcpipParas->uRecvBuf.recvBuf)
                    {
                        free(tcpipParas->uRecvBuf.recvBuf);

                        tcpipParas->uRecvBuf.recvBuf = NULL;
                    }
                    tcpipParas->uRecvBuf.readnum = 0;

                    tcpipParas->uRecvBuf.recvBufMaxSize = 0;
                }
                else
                {
                    OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETRD,there have data left");
                }
            }
            else
            {
                OSI_LOGI(0, "recv_data,there is no data in buff");
                sprintf(uaRspStr, "$MYNETREAD:%d,0", nMuxIndex);
                writeInfoNText(engine, (const char *)uaRspStr, strlen((const char *)uaRspStr));
                //atCmdRespOK(engine);
            }
        }
    }
    else
    {
        //iResult = CFW_TcpipGetRecvAvailable(uSocket);
        if (!mynetMux_multiIp)
        {
            sprintf(uaRspStr, "+MYNETRXGET:%d,%ld", mode, tcpipParas->uRecvBuf.recvBufLen);
        }
        else
        {
            sprintf(uaRspStr, "+MYNETRXGET:%d,%d,%ld", mode, nMuxIndex, tcpipParas->uRecvBuf.recvBufLen);
        }
        writeInfoNText(engine, (const char *)uaRspStr, strlen((const char *)uaRspStr));
    }
    free(pData);
    return 0;
}

static void tcpip_connect_done(atCmdEngine_t *engine, uint8_t nMuxIndex)
{
    stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[nMuxIndex]);
    tcpipParas->uState = MYNET_CONNECTED;
    char uaRspStr[50] = {
        0,
    };
    if (tcpipParas->isTrans)
    {
        //atCmdRespInfoText(engine, "OK");
        start_transparent_mode(engine, nMuxIndex);
    }
    else
    {
        if (!mynetMux_multiIp)
            sprintf(uaRspStr, "CONNECT OK");
        else
        {
            if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_TCP)
                sprintf(uaRspStr, "$MYNETOPEN:%d,%ld", nMuxIndex, CFW_TcpipSocketGetMss(tcpipParas->uSocket));
            else
                sprintf(uaRspStr, "$MYNETOPEN:%d", nMuxIndex);
        }
        atCmdRespInfoText(engine, uaRspStr);
        atCmdRespOK(engine);
    }
}
osiTimer_t *g_socketLedTimer;
uint8_t socketLedStatus = 0; //0:dark;1:flight
uint8_t isSocketLedFlicker = 0;
uint8_t socketLedFlickerMode = 0; //0:pdp active mode;1:socket connect mode

void socketLed_flicker(void *param)
{
    OSI_LOGI(0, "socketLed_flicker");
    int flicker_time = 1000;
    if (socketLedStatus == 0)
    {
        socketLedStatus = 1;
        flicker_time = socketLedFlickerMode ? 1800 : 200;
    }
    else if (socketLedStatus == 1)
    {
        socketLedStatus = 0;
        flicker_time = socketLedFlickerMode ? 200 : 1800;
    }
    else
    {
        return;
    }
    OSI_LOGI(0, "socketLed_flicker,led:%d", socketLedStatus);
#ifdef CONFIG_SOC_6760
    drvTurnOnSocketLed(socketLedStatus);
#endif
    osiTimerStop(g_socketLedTimer);
    osiTimerStart(g_socketLedTimer, flicker_time);
}

void socketLed_startFlicker(void *engine, uint8_t mode)
{
    OSI_LOGI(0, "socketLed_startFlicker,mode=%d", mode);
    if (isSocketLedFlicker == 1 && socketLedFlickerMode == mode)
    {
        OSI_LOGI(0, "socketLed_startFlicker,led already flickered in mode:%d", mode);
        return;
    }

    //if (isSocketLedFlicker == 1 && socketLedFlickerMode != mode)
    // osiTimerStop(g_socketLedTimer);
    socketLedFlickerMode = mode;
    if (g_socketLedTimer == NULL)
    {
        g_socketLedTimer = osiTimerCreate(osiThreadCurrent(), socketLed_flicker, (void *)engine);
        if (g_socketLedTimer == NULL)
        {
            OSI_LOGI(0, "socketLed_startFlicker, can not create timers");
            return;
        }
    }
    socketLed_flicker((void *)engine);
    isSocketLedFlicker = 1;
}
void socketLed_stopFlicker()
{
    if (isSocketLedFlicker == 0)
    {
        OSI_LOGI(0, "socketLed_stopFlicker,led already dark");
        return;
    }
    osiTimerStop(g_socketLedTimer);
    if (g_socketLedTimer != NULL)
    {
        osiTimerDelete(g_socketLedTimer);
        g_socketLedTimer = NULL;
    }
    isSocketLedFlicker = 0;
#ifdef CONFIG_SOC_6760
    drvTurnOnSocketLed(1);
#endif
    OSI_LOGI(0, "socketLed_stopFlicker,isSocketLedFlicker=%d", isSocketLedFlicker);
}

static void tcpip_rsp(void *param)
{
    if (param == NULL)
        return;
    osiEvent_t *pEvent = (osiEvent_t *)param;
    char uaRspStr[60] = {
        0,
    };
    //OSI_LOGI(0, "tcpip_rsp Got %s: 0x%x,0x%x,0x%x,0x%x",
    //TS_GetEventName(pEvent->id), pEvent->id, pEvent->nParam1, pEvent->param2, pEvent->nParam3);

    tcpip_callback_param_t *tcpparam = (tcpip_callback_param_t *)pEvent->param3;
    //uint8_t nDlc = pEvent->param3;
    uint8_t nMuxIndex = tcpparam->nMuxIndex;
    uint8_t uSocket = (uint8_t)pEvent->param1;
    atCmdEngine_t *engine = tcpparam->engine;

    stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[nMuxIndex]);

    OSI_LOGI(0, "tcpip_rsp uSocket=%d,tcpipParas->uSocket=%d,state=%d,nMuxIndex=%d,pEvent->id=%ld", uSocket, tcpipParas->uSocket, tcpipParas->uState, nMuxIndex, pEvent->id);

    switch (pEvent->id)
    {
    case EV_CFW_TCPIP_SOCKET_CONNECT_RSP:

        socketLed_startFlicker(engine, 1);

        if (tcpipParas->uSocket == uSocket)
        {
            tcpip_connect_done(engine, nMuxIndex);
        }
        break;
    case EV_CFW_TCPIP_ACCEPT_IND:
    {

        int haveIndex = 0;
        for (haveIndex = 0; haveIndex < CHANNEL_NUM_MAX; haveIndex++)
        {
            if (g_uMynetContexts.nTcpipParas[haveIndex].uState == MYNET_CLOSED && g_uMynetContexts.nTcpipParas[haveIndex].uServerState == MYNETSERVER_CLOSE)
            {
                break;
            }
        }
        (haveIndex == CHANNEL_NUM_MAX) ? (haveIndex -= 1) : haveIndex;
        if (haveIndex >= MEMP_NUM_NETCONN)
        {
            OSI_LOGI(0, "EV_CFW_TCPIP_ACCEPT_IND, NO used Index\n");
            break;
        }
        uint8_t cur_clientnum = 0;
        CFW_TCPIP_SOCKET_ADDR addr;
        uint32_t addr_len = sizeof(addr);
        TCPIP_DNS_PARAM *dnsparam = malloc(sizeof(TCPIP_DNS_PARAM));
        if (dnsparam == NULL)
        {
            OSI_LOGI(0, "mynet tcpip_rsp,malloc failed\n");
            break;
        }
        memset(dnsparam, 0, sizeof(TCPIP_DNS_PARAM));
        dnsparam->engine = engine;
        dnsparam->nMuxIndex = haveIndex;
        int8_t newSocket = CFW_TcpipSocketAcceptEx(uSocket, &addr, &addr_len, tcpip_rsp, (uint32_t)dnsparam);
        if (newSocket <= 0)
        {
            free(dnsparam);
            OSI_LOGI(0, "newSocket=%d,break", newSocket);
            break;
        }
        int i;
        int isValid = 0;
        int unValidNum = 0;
        for (i = 0; i < IPFILTER_NUM_MAX; i++)
        {
            if (g_ipFilter[i].isValid == 1)
            {
                uint32_t filterip = CFW_TcpipInetAddr((const char *)g_ipFilter[i].ipAddr);
                if (SOCKET_ERROR == filterip)
                {
                    OSI_LOGI(0, "filter ip CFW_TcpipInetAddr() failed\n");
                }
                uint32_t mask = CFW_TcpipInetAddr((const char *)g_ipFilter[i].netMask);
                if ((0 != strcmp(g_ipFilter[i].netMask, "255.255.255.255")) && SOCKET_ERROR == mask)
                {
                    OSI_LOGI(0, "mask CFW_TcpipInetAddr() failed\n");
                    continue;
                }
                OSI_LOGI(0, "i=%d,filterip=%x,mask=%x,clientaddr=%x\n", i, filterip, mask, (uint32_t)(addr.sin_addr.s_addr));
                if (0 == strcmp(g_ipFilter[i].netMask, "255.255.255.255"))
                {
                    if (filterip == ((uint32_t)(addr.sin_addr.s_addr)))
                    {
                        OSI_LOGI(0, "mask 255.255.255.255 match set isValid to 1\n");
                        isValid = 1;
                        break;
                    }
                    else
                    {
                        OSI_LOGI(0, "mask 255.255.255.255 not match\n");
                        continue;
                    }
                }
                if ((filterip & mask) == (((uint32_t)(addr.sin_addr.s_addr)) & mask))
                {
                    OSI_LOGI(0, "mask match set isValid to 1\n");
                    isValid = 1;
                    break;
                }
            }
            else
            {
                OSI_LOGI(0, "tcpip_rsp accept g_ipFilter[i].isValid= %d,continue...\n", g_ipFilter[i].isValid);
                unValidNum++;
                continue;
            }
        }

        if (isValid == 0 && unValidNum < IPFILTER_NUM_MAX)
        {
            CFW_TcpipSocketSetParam(newSocket, NULL, (uint32_t)0);
            if (CFW_TcpipSocketClose(newSocket) == ERR_SUCCESS)
            {
                OSI_LOGI(0, "tcpip_rsp accept IP do not in IPFilter! newSocket:%d closed", newSocket);
                //start_tcpip_close_timer(nDlc, nMuxIndex);
            }
            break;
        }

        g_uMynetContexts.nTcpipParas[haveIndex].uMode = 0;
        g_uMynetContexts.nTcpipParas[haveIndex].isText = tcpipParas->isText;
        g_uMynetContexts.nTcpipParas[haveIndex].uState = MYNET_CONNECTED;
        g_uMynetContexts.nTcpipParas[haveIndex].uSocket = newSocket;
        AT_StrNCpy(g_uMynetContexts.nTcpipParas[haveIndex].uaIPAddress, ip4addr_ntoa((ip4_addr_t *)&(addr.sin_addr)), AT_MYNET_TCPIP_HOSTLEN);
        g_uMynetContexts.nTcpipParas[haveIndex].uaIPAddress[AT_MYNET_TCPIP_HOSTLEN - 1] = '\0';
        g_uMynetContexts.nTcpipParas[haveIndex].uPort = ntohs(addr.sin_port);
        g_uMynetContexts.nTcpipParas[haveIndex].listening_socket = uSocket;
        g_uMynetContexts.nTcpipParas[haveIndex].uCid = tcpipParas->uCid;
        //gMynetserver.channelid = i;
        struct linger _linger;
        _linger.l_onoff = 1;
        _linger.l_linger = 0;
        CFW_TcpipSocketSetsockopt(newSocket, SOL_SOCKET, SO_LINGER, (void *)&_linger, sizeof(_linger));
        OSI_LOGI(0, "mynet CFW_TcpipSocketSetsockopt done,get bind ip and port\n");
        CFW_TCPIP_SOCKET_ADDR stLocalAddr = {
            0,
        };
        int addrlen = sizeof(struct sockaddr_storage);
        CFW_TcpipSocketGetsockname(g_uMynetContexts.nTcpipParas[haveIndex].uSocket, &stLocalAddr, (int *)(&addrlen));
        OSI_LOGI(0, " port:%d %d", stLocalAddr.sin_port, ntohs(stLocalAddr.sin_port));
        AT_StrNCpy(g_uMynetContexts.nTcpipParas[haveIndex].uLocalip, inet_ntoa(stLocalAddr.sin_addr), AT_MYNET_TCPIP_HOSTLEN);
        g_uMynetContexts.nTcpipParas[haveIndex].uLocalip[AT_MYNET_TCPIP_HOSTLEN - 1] = '\0';
        OSI_LOGXI(OSI_LOGPAR_S, 0, "tcpipParas->uLocalip=%s", tcpipParas->uLocalip);
        gMynetlocalport[haveIndex].tcp_port = ntohs(stLocalAddr.sin_port);

        uint8_t lsocket = 0;
        for (int k = 0; k < CHANNEL_NUM_MAX; k++)
        {
            if (g_uMynetContexts.nTcpipParas[k].listening_socket != 0)
            {
                lsocket = g_uMynetContexts.nTcpipParas[k].listening_socket;
                OSI_LOGI(0, "EV_CFW_TCPIP_ACCEPT_IND, listen socket=%d\n", lsocket);
                cur_clientnum++;
            }
        }
        if (cur_clientnum >= g_maxclientnum)
        {
            OSI_LOGI(0, "EV_CFW_TCPIP_ACCEPT_IND, close server socket cur_clientnum=%d;g_maxclientnum=%d\n", cur_clientnum, g_maxclientnum);
            for (int j = 0; j < CHANNEL_NUM_MAX; j++)
            {
                if (g_uMynetContexts.nTcpipParas[j].uSocket == lsocket && g_uMynetContexts.nTcpipParas[j].uServerState == MYNETSERVER_OPEN)
                {
                    OSI_LOGI(0, "EV_CFW_TCPIP_SOCKET_ACCEPT_RSP, goto close server socket\n");
                    g_uMynetContexts.nTcpipParas[j].isOnlySocketClose = 1;
                    CFW_TcpipSocketClose(lsocket);
                    break;
                }
            }
        }
        if (g_mynetUrc == 1)
        {
            sprintf(uaRspStr, "$MYURCCLIENT:%d,\"%s\",%d", haveIndex, g_uMynetContexts.nTcpipParas[haveIndex].uaIPAddress, g_uMynetContexts.nTcpipParas[haveIndex].uPort);
            atCmdRespInfoText(engine, uaRspStr);
        }
    }
    break;
    case EV_CFW_TCPIP_SOCKET_CLOSE_RSP:
    {
        OSI_LOGI(0, "RECEIVED EV_CFW_TCPIP_SOCKET_CLOSE_RSP ...");
        tcpipParas->engine = NULL;
        if ((mynetRXGET_manualy == 1) && (tcpipParas->uRecvBuf.recvBuf != NULL)) //clear recv buffer
        {
            free(tcpipParas->uRecvBuf.recvBuf);
            tcpipParas->uRecvBuf.recvBuf = NULL;
            tcpipParas->uRecvBuf.readnum = 0;
            tcpipParas->uRecvBuf.recvBufLen = 0;
            tcpipParas->uRecvBuf.recvBufMaxSize = 0;
        }
        if (1 == tcpipParas->uMode && tcpipParas->uServerState == MYNETSERVER_OPEN && tcpipParas->isOnlySocketClose == 1)
        {

            OSI_LOGI(0, "SERVER SOCKET CLOSED");
            free(tcpparam);
            break;
        }
        atDispatch_t *dispatch = atCmdGetDispatch(engine);
        if (tcpipParas->isTrans && atDispatchIsDataMode(dispatch)) //stop transparent mode
        {
            stop_transparent_mode(engine, nMuxIndex);
            tcpipParas->isTrans = 0;
        }
        if (tcpipParas->uState == MYNET_CLOSING)
        {
            if (g_mynetUrc == 1)
            {
                if (!mynetMux_multiIp)
                    sprintf(uaRspStr, "CLOSED");
                else
                    sprintf(uaRspStr, "$MYURCCLOSE:%d", nMuxIndex);
                //stop_tcpip_close_timer(nDlc, nMuxIndex);
                atCmdRespInfoText(engine, uaRspStr);
            }
            if (tcpipParas->sentSize != tcpipParas->sendSize)
            {
                OSI_LOGI(0, "tcpipParas->sentSize=%d,tcpipParas->sendSize=%d", tcpipParas->sentSize, tcpipParas->sendSize);
                sprintf(uaRspStr, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
                atCmdRespErrorText(engine, uaRspStr);
                if (tcpipParas->sendTOtimer != NULL)
                {
                    osiTimerStop(tcpipParas->sendTOtimer);
                    osiTimerDelete(tcpipParas->sendTOtimer);
                    tcpipParas->sendTOtimer = NULL;
                    tcpipParas->isSendTimeout = 0;
                }
            }
        }
        else
        {
            if (!mynetMux_multiIp)
            {
                sprintf(uaRspStr, "CLOSE OK");
                atCmdRespOKText(engine, uaRspStr);
            }
            else
            {
                sprintf(uaRspStr, "$MYNETCLOSE:%d", nMuxIndex);
                //stop_tcpip_close_timer(nDlc, nMuxIndex);
                atCmdRespInfoText(engine, uaRspStr);
                atCmdRespOK(engine);
            }
        }
        //memset(tcpipParas, 0, sizeof(stAT_Tcpip_Paras));
        tcpipParas->engine = NULL;
        tcpipParas->uServerState = MYNETSERVER_CLOSE;
        tcpipParas->uState = MYNET_CLOSED;
        tcpipParas->uSocket = 0;
        tcpipParas->sendSize = 0;
        tcpipParas->sentSize = 0;

        memset(tcpipParas->uLocalip, 0, AT_MYNET_TCPIP_HOSTLEN);
        gMynetlocalport[nMuxIndex].tcp_port = 0;
        gMynetlocalport[nMuxIndex].udp_port = 0;

        if (tcpipParas->sendTimerID != NULL)
        {
            osiTimerStop(tcpipParas->sendTimerID);
            osiTimerDelete(tcpipParas->sendTimerID);
            tcpipParas->sendTimerID = NULL;
        }
        memset(&(tcpipParas->from_addr), 0, sizeof(CFW_TCPIP_SOCKET_ADDR));
        if (0 != tcpipParas->listening_socket)
        {
            OSI_LOGI(0, "EV_CFW_TCPIP_SOCKET_CLOSE_RSP, tcpipParas->listening_socket=%d", tcpipParas->listening_socket);
            uint8_t lsocket = tcpipParas->listening_socket;
            int i = 0;
            tcpipParas->listening_socket = 0;
            for (i = 0; i < CHANNEL_NUM_MAX; i++)
            {
                if (g_uMynetContexts.nTcpipParas[i].isOnlySocketClose == 1)
                {
                    OSI_LOGI(0, "EV_CFW_TCPIP_SOCKET_CLOSE_RSP, server socket=%d,i=%d\n", lsocket, i);
                    break;
                }
            }
            OSI_LOGI(0, "EV_CFW_TCPIP_SOCKET_CLOSE_RSP, i=%d,channel_num_max=%d", i, CHANNEL_NUM_MAX);
            if (i >= CHANNEL_NUM_MAX)
            {
                free(tcpparam);
                break;
            }
            OSI_LOGI(0, "EV_CFW_TCPIP_SOCKET_CLOSE_RSP, goto check client num\n");
            uint8_t cur_clientnum = 0;

            for (int j = 0; j < CHANNEL_NUM_MAX; j++)
            {
                if (g_uMynetContexts.nTcpipParas[j].listening_socket != 0)
                {
                    lsocket = g_uMynetContexts.nTcpipParas[j].listening_socket;
                    OSI_LOGI(0, "EV_CFW_TCPIP_SOCKET_CLOSE_RSP, listen socket=%d\n", lsocket);
                    cur_clientnum++;
                }
            }
            OSI_LOGI(0, "EV_CFW_TCPIP_SOCKET_CLOSE_RSP, cur_clientnum=%d,g_maxclientnum=%d\n", cur_clientnum, g_maxclientnum);
            if (cur_clientnum < g_maxclientnum)
            {

                if (ERR_SUCCESS == AT_TCPIP_ServerStart(engine, i))
                {
                    g_uMynetContexts.nTcpipParas[i].uMode = MYNETSERVER_OPEN;
                    g_uMynetContexts.nTcpipParas[i].isOnlySocketClose = 0;
                    OSI_LOGI(0, "tcpip_rsp EV_CFW_TCPIP_SOCKET_CLOSE_RSP AT_TCPIP_ServerStart success");
                }
                else
                {
                    OSI_LOGI(0, "tcpip_rsp AT_TCPIP_ServerStart failed");
                }
            }
        }
        int j = 0;
        for (j = 0; j < CHANNEL_NUM_MAX; j++)
        {
            if (g_uMynetContexts.nTcpipParas[j].uState == MYNET_CLOSED && g_uMynetContexts.nTcpipParas[j].uMode == MYNETSERVER_CLOSE)
                continue;
            else
                break;
        }
        if (j >= CHANNEL_NUM_MAX)
            socketLed_startFlicker(engine, 0);

        free(tcpparam);
    }
    break;
    case EV_CFW_TCPIP_ERR_IND:
    case EV_CFW_TCPIP_BEARER_LOSING_IND:
        if (tcpipParas->uSocket == uSocket)
        {
            if (tcpipParas->uState == MYNET_PROCESSING)
            {
                int32_t err_num = (int32_t)pEvent->param2;
                OSI_LOGI(0, "RECEIVED EV_CFW_TCPIP_ERR_IND err_num=%ld", err_num);
                if (err_num == -13)
                    sprintf(uaRspStr, "ERROR:%d", ERR_AT_CME_TCPIP_CONNECT_TIMEOUT);
                else
                    sprintf(uaRspStr, "ERROR:%d", ERR_AT_CME_TCPIP_PEER_REFUSED);
                atCmdRespErrorText(engine, uaRspStr);
            }
            if (CFW_TcpipSocketClose(uSocket) == ERR_SUCCESS)
            {
                tcpipParas->uState = MYNET_CLOSING;
            }
        }
        break;

    case EV_CFW_TCPIP_CLOSE_IND:
        if (tcpipParas->uSocket == uSocket)
        {
            if (CFW_TcpipSocketClose(uSocket) == ERR_SUCCESS)
            {
                tcpipParas->uState = MYNET_CLOSING;
                //start_tcpip_close_timer(nDlc, nMuxIndex);
            }
        }
        break;
    case EV_CFW_TCPIP_SOCKET_SEND_RSP:
        OSI_LOGI(0, "EV_CFW_TCPIP_SOCKET_SEND_RSP");
        if (!tcpipParas->isTrans)
        {
            memset(uaRspStr, 0, 30);
            uint16_t uSendDataSize = 0;
            uSendDataSize = (uint16_t)pEvent->param2;
            tcpipParas->sentSize += uSendDataSize;
            OSI_LOGI(0, "uSendDataSize =%d tcpipParas->sentSize = %d tcpipParas->sendSize= %d",
                     uSendDataSize, tcpipParas->sentSize, tcpipParas->sendSize);
            if (tcpipParas->sentSize == tcpipParas->sendSize)
            {
                /*if (!mynetQSEND_quickSend && mynetSPRT_sendPrompt != 2)
                {
                    if (!mynetMux_multiIp)
                        sprintf(uaRspStr, "SEND OK");
                    else
                        sprintf(uaRspStr, "%d,SEND OK", nMuxIndex);
                }*/
                if (tcpipParas->isSendTimeout != 1)
                {
                    atCmdRespOK(engine);
                    if (tcpipParas->sendTOtimer != NULL)
                    {
                        osiTimerStop(tcpipParas->sendTOtimer);
                        osiTimerDelete(tcpipParas->sendTOtimer);
                        tcpipParas->sendTOtimer = NULL;
                        tcpipParas->isSendTimeout = 0;
                    }
                }
            }
        }
        break;
    case EV_CFW_TCPIP_REV_DATA_IND:
        OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND");
        if (tcpipParas->uSocket == uSocket)
        {
            uint16_t uDataSize = 0;
            uDataSize = (uint16_t)pEvent->param2; // maybe contain check bits
            OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND,mynetRXGET_manualy=%d,tcpipParas->uMode=%d,state=%d", mynetRXGET_manualy, tcpipParas->uMode, tcpipParas->uState);
            if (mynetRXGET_manualy == 1 && tcpipParas->isTrans != 1)
            {
                if (((tcpipParas->uMode != 1) && (tcpipParas->listening_socket == 0)) || ((tcpipParas->uMode != 1) && (tcpipParas->listening_socket > 0) && (tcpipParas->uState == MYNET_ACCEPT)))
                {
                    OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND,g_mynetUrc=%d,tcpipParas->uRecvBuf.recvBufLen=%d,tcpipParas->uRecvBuf.recvBufMaxSize=%d", g_mynetUrc, tcpipParas->uRecvBuf.recvBufLen, tcpipParas->uRecvBuf.recvBufMaxSize);
                    if ((g_mynetUrc == 1) && (0 == tcpipParas->uRecvBuf.recvBufLen))
                    {
                        OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND,$MYURCREAD");
                        sprintf(uaRspStr, "$MYURCREAD:%d", nMuxIndex);
                        atCmdRespInfoText(engine, uaRspStr);
                    }
                }
                recv_data(uSocket, uDataSize, engine, nMuxIndex, 1);
            }
            else
            {
                recv_data(uSocket, uDataSize, engine, nMuxIndex, 0);
            }
        }
        break;
    default:
        OSI_LOGI(0, "TCPIP unexpect asynchrous event/response %d", pEvent->id);
        //char AtRet[128] = {0};
        //sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        //atCmdRespErrorText(engine, AtRet);
        break;
    }
    free(pEvent);
}
static void _pdpAddressToStr(const CFW_GPRS_PDPCONT_INFO_V2 *pdp_cont, char *str)
{
    if (pdp_cont->nPdpAddrSize == 0)
        sprintf(str, "0.0.0.0");
    else
        sprintf(str, "%d.%d.%d.%d", pdp_cont->pPdpAddr[0], pdp_cont->pPdpAddr[1],
                pdp_cont->pPdpAddr[2], pdp_cont->pPdpAddr[3]);
    // TODO: use inet_ntop
}

static void _cgactActHead(atCommand_t *cmd);
static void _cgactGprsActRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    uint8_t nCid = 0;
    uint8_t nSimId = 0;
    cgactAsyncCtx_t *async = (cgactAsyncCtx_t *)cmd->async_ctx;
    char AtRet[128] = {0};
    // ignore return code silently

    if (async->cid_index >= async->cid_count)
    {
        if (event->id == EV_CFW_GPRS_ACT_RSP)
        {
            CFW_EVENT *cfw_event = (CFW_EVENT *)event;
            OSI_LOGI(0, "atcstatus=%d,cfw_event->nEventId=%d", cfw_event->nType, cfw_event->nEventId);
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
                OSI_LOGI(0, "ppp session will be deleted!!! MYNET");
                nCid = event->param1;
                nSimId = cfw_event->nFlag;
                pppSessionDeleteByNetifDestoryed(nSimId, nCid);

                osiEvent_t tcpip_event = *event;
                tcpip_event.id = EV_TCPIP_CFW_GPRS_DEACT;
                CFW_EVENT *tcpip_cfw_event = (CFW_EVENT *)&tcpip_event;
                tcpip_cfw_event->nUTI = 0;
                osiEventSend(netGetTaskID(), (const osiEvent_t *)&tcpip_event);
            }
            else
            {
                g_engine = NULL;
                OSI_LOGI(0, "atcstatus=%d,cfw_event->nEventId=%d", cfw_event->nType, cfw_event->nEventId);
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            if (CFW_GPRS_ACTIVED == cfw_event->nType)
            {
                atCmdRespInfoText(cmd->engine, "OK");
                OSI_LOGI(0, "cgactGprsActRsp CFW_GPRS_ACTIVED,g_mynetUrc=%ld", g_mynetUrc);
                if (g_mynetUrc == 1)
                {
                    CFW_SIM_ID nSim = (CFW_SIM_ID)atCmdGetSim(cmd->engine);
                    OSI_LOGI(0, "async->cids[async->cid_count - 1]:%d", async->cids[async->cid_count - 1]);
                    //netif = getGprsNetIf(nSim, async->cids[async->cid_count - 1]);
                    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont; //, sPdpOld;
                    if (CFW_GprsGetPdpCxtV2(async->cids[async->cid_count - 1], &sPdpCont, nSim) != 0)
                    {
                        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                    }
                    char addr_str[32];
                    _pdpAddressToStr(&sPdpCont, addr_str);
                    sprintf(AtRet, "$MYURCACT: %d, %d, \"%s\"", async->cids[async->cid_count - 1] - 1, async->act_state, addr_str);
                    atCmdRespInfoText(cmd->engine, AtRet);
                }
                socketLed_startFlicker(cmd->engine, 0); //pdp active flicker mode
                g_engine = NULL;
                AT_CMD_RETURN(atCmdRespOKText(cmd->engine, ""));
            }
            else if (CFW_GPRS_DEACTIVED == cfw_event->nType)
            {
                atCmdRespInfoText(cmd->engine, "OK");
                if (g_mynetUrc == 1)
                {
                    sprintf(AtRet, "$MYURCACT: %d, %d", async->cids[async->cid_count - 1] - 1, async->act_state);
                    atCmdRespInfoText(cmd->engine, AtRet);
                }
                g_engine = NULL;
                int i = 1;
                for (i = 1; i <= 7; i++)
                {
                    uint8_t cid_act_state = 0;
                    CFW_GetGprsActState(i, &cid_act_state, nSimId);
                    if (cid_act_state == CFW_GPRS_ACTIVED)
                    {
                        OSI_LOGI(0, "_cgactGprsActRsp cid:%d;nSimId:%d is actived", i, nSimId);
                        break;
                    }
                }
                if (i > 7)
                {
                    OSI_LOGI(0, "_cgactGprsActRsp no pdp is actived,stop flicker");
                    socketLed_stopFlicker();
                    uint8_t att_state = 0;
                    uint32_t iResult = CFW_GetGprsAttState(&att_state, nSimId);
                    OSI_LOGI(0, "_cgactGprsActRsp CFW_GPRS_DEACTIVED,get att status iResult=%d,att_state=%d", iResult, att_state);
                    if (0 == att_state)
                    {
#ifdef CONFIG_SOC_6760
                        drvTurnOnSocketLed(0);
#endif
                    }
                    else if (1 == att_state)
                    {
#ifdef CONFIG_SOC_6760
                        drvTurnOnSocketLed(1);
#endif
                    }
                }
                AT_CMD_RETURN(atCmdRespOKText(cmd->engine, ""));
            }
        }
        RETURN_OK(cmd->engine);
    }

    _cgactActHead(cmd);
}

static void _cgactGprsAttRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    //	OSI_LOGI("enter event id = %ld p1 = %ld p2 = %ld p3 = %ld",
    //		event->id,event->param1,event->param2,event->param3);
    char AtRet[128] = {0};
    if (cfw_event->nParam1 != 0)
    {
        g_engine = NULL;
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }

    _cgactActHead(cmd);
}

static void _cgactActHead(atCommand_t *cmd)
{
    cgactAsyncCtx_t *async = (cgactAsyncCtx_t *)cmd->async_ctx;

    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t cid = async->cids[async->cid_index];

    cmd->uti = cfwRequestUTI((osiEventCallback_t)_cgactGprsActRsp, cmd);
    uint32_t ret = CFW_GprsAct(async->act_state, cid, cmd->uti, nSim);
    if (ret != 0)
    {
        g_engine = NULL;
        cfwReleaseUTI(cmd->uti);
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG
    else
    {
    	quec_netlight_deal_pdp_activing(async->act_state);
    }
#endif

    async->cid_index++;
    RETURN_FOR_ASYNC();
}
static void _mynetRspTimeOutCB(atCommand_t *cmd)
{
    char AtRet[128] = {0};
    if (AT_CFW_UTI_INVALID != cmd->uti)
        cfwReleaseUTI(cmd->uti);
    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
}

static void _timeoutabortTimeout(atCommand_t *cmd)
{
    OSI_LOGI(0, "_timeoutabortTimeout");
    if (CFW_UTI_INVALID != cmd->uti)
        cfwReleaseUTI(cmd->uti);
    //tcpipParas->engine = NULL;
    char AtRet[128] = {0};
    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_TCPIP_CONNECT_TIMEOUT);
    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
}

void AT_GPRS_CmdFunc_MYNETACT(atCommand_t *cmd)
{
    int32_t iResult = 0;
    uint8_t uState = 0;
    uint8_t uaCid = 0;
    char AtRet[128] = {0}; /* max 20 charactors per cid */
    uint8_t i = 0;
    if (cmd == NULL)
    {
        OSI_LOGI(0, "AT_GPRS_CmdFunc_MYNETACT,cmd == NULL");
        return;
    }

    CFW_SIM_ID nSim = (CFW_SIM_ID)atCmdGetSim(cmd->engine);
    //AT_Gprs_CidInfo *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    //OSI_LOGI(0, "+MYNETACT: nSim= %d; nDLCI= 0x%x", nSim, cmd->nDLCI);

    //if (NULL == cmd)
    //{
    //    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
    //    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    //}
#if 0
    if (CLASS_TYPE_CC == gAtCfwCtx.g_uClassType)
    {
        //class cc mode,can not active
        OSI_LOGI(0, "Gprs in class cc mode,can not active");
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
#endif
    // get gprs att state
    uint8_t att_state = 0;
    iResult = CFW_GetGprsAttState(&att_state, nSim);

    if ((ERR_SUCCESS != iResult) || ((0 != att_state) && (1 != att_state)))
    {
        OSI_LOGI(0x100042a6, "CFW_GetGprsAttState() failed,iResult = 0x%x. ", iResult);
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        // check paracount
        if (cmd->param_count != 2)
        {
            // the param "<state>" become a must
            OSI_LOGI(0, "exe       in AT_GPRS_CmdFunc_CGACT, parameters error or no parameters offered \n");
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }

        uaCid = atParamUintInRange(cmd->params[0], 0, CHANNEL_NUM_MAX - 1, &paramok) + 1;

        uState = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
        if (!paramok)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }

        if (g_engine != NULL && g_engine != cmd->engine)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        g_engine = cmd->engine;
        // filter out non-touch CID
        cgactAsyncCtx_t *async = (cgactAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
        {
            g_engine = NULL;
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_BUFFER_FULL);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        async->act_state = uState;
        async->cid_count = 0;
        async->cid_index = 0;

        uint8_t cid_act_state = 0;
        CFW_GetGprsActState(uaCid, &cid_act_state, nSim);
        if (cid_act_state != uState)
        {
            async->cids[async->cid_count] = uaCid;
            async->cid_count++;
        }

        if (async->cid_count == 0 && cid_act_state == 1)
        {
            g_engine = NULL;
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PDP_ACTIVED);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        if ((async->cid_count == 0 && cid_act_state == 0))
        {
            g_engine = NULL;
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PDP_DEACTIVED);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        atCmdSetTimeoutHandler(cmd->engine, 1000 * 60, _mynetRspTimeOutCB);
        // if not attach, Attach GPRS.
        if ((att_state == CFW_GPRS_DETACHED) &&
            (uState == CFW_GPRS_ACTIVED) &&
            (async->cid_count > 0))
        {
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cgactGprsAttRsp, cmd);
            uint32_t ret;
            if ((ret = CFW_AttDetach(CFW_GPRS_ATTACHED, cmd->uti, ATTACH_TYPE, nSim)) != 0)
            {
                g_engine = NULL;
                cfwReleaseUTI(cmd->uti);
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            RETURN_FOR_ASYNC();
        }
        else
        {
            _cgactActHead(cmd);
        }
    }
    else if (AT_CMD_READ == cmd->type)
    {
        for (i = 1; i <= 6; i++) // yy [mod] 2008-5-26 for bug ID 8588
        {
            OSI_LOGI(0, "+CGACT: i= %d; nSim= %d", i, nSim);
#ifndef AT_NO_GPRS
            iResult = CFW_GetGprsActState(i, &uState, nSim);
#endif
            if (iResult != ERR_SUCCESS)
            {
                continue;
            }
            else
            {
                if (CFW_GPRS_ACTIVED == uState)
                {
                    /*struct netif *netif;
                    netif = getGprsNetIf(nSim, i);
                    if (netif == NULL)
                    {
                        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                    }

                    ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(netif);
                    memset(AtRet, 0, sizeof(AtRet));
                    sprintf(AtRet, "$MYNETACT: %d, %d, \"%s\"", i - 1, uState, inet_ntoa(*ip_addr));
                    atCmdRespInfoText(cmd->engine, AtRet);*/

                    //netif = getGprsNetIf(nSim, async->cids[async->cid_count - 1]);
                    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont; //, sPdpOld;
                    if (CFW_GprsGetPdpCxtV2(i, &sPdpCont, nSim) != 0)
                    {
                        //sprintf(AtRet, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                        //AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                        memset(AtRet, 0, sizeof(AtRet));
                        sprintf(AtRet, "$MYNETACT: %d, %d, \"0.0.0.0\"", i - 1, uState);
                        atCmdRespInfoText(cmd->engine, AtRet);
                    }
                    char addr_str[32];
                    _pdpAddressToStr(&sPdpCont, addr_str);
                    sprintf(AtRet, "$MYNETACT: %d, %d, \"%s\"", i - 1, uState, addr_str);
                    atCmdRespInfoText(cmd->engine, AtRet);
                }
                else
                {
                    memset(AtRet, 0, sizeof(AtRet));
                    sprintf(AtRet, "$MYNETACT: %d, %d, \"0.0.0.0\"", i - 1, uState);
                    atCmdRespInfoText(cmd->engine, AtRet);
                }
            }
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        for (i = 1; i <= 6; i++)
        {
            OSI_LOGI(0, "+CGACT: i= %d; nSim= %d", i, nSim);
#ifndef AT_NO_GPRS
            iResult = CFW_GetGprsActState(i, &uState, nSim);
#endif
            if (iResult != ERR_SUCCESS)
            {
                OSI_LOGI(0, "+CGACT: CFW_GetGprsActState failed");
                continue;
            }
            else
            {
                memset(AtRet, 0, sizeof(AtRet));
                sprintf(AtRet, "$MYNETACT: %d, %d", uState, i - 1);
                atCmdRespInfoText(cmd->engine, AtRet);
            }
        }
        RETURN_OK(cmd->engine);
    }
    else
    {
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYNETACCEPT(atCommand_t *cmd)
{
    OSI_LOGI(0, "AT+MYNETACCEPT >> ENTER");
    bool paramok = true;
    char AtRet[128] = {0};

    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count != 3)
        {
            OSI_LOGI(0, "AT+MYNETACCEPT invalid param_count:%d", cmd->param_count);
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        uint8_t uMuxIndex = atParamUintInRange(cmd->params[0], 0, 5, &paramok); //socket index
        uint8_t refuse = atParamUintInRange(cmd->params[1], 0, 1, &paramok);    //0:accept;1:refuse
        uint8_t isTrans = atParamUintInRange(cmd->params[2], 0, 1, &paramok);   //0:cmd;1:transparent mode

        if (!paramok)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        //atCmdRespOK(cmd->engine);
        stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[uMuxIndex]);
        if (tcpipParas->listening_socket == 0)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        if (tcpipParas->uState != MYNET_CONNECTED && tcpipParas->uState != MYNET_ACCEPT)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        if (tcpipParas->engine != NULL && tcpipParas->engine != cmd->engine)
        {
            OSI_LOGI(0x0, "tcpipParas->engine  %x, cmd->engine %x", tcpipParas->engine, cmd->engine);
            char AtRet[128] = {0};
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        tcpipParas->isRefuse = refuse;
        tcpipParas->isTrans = isTrans;
        if (refuse == 1)
        {
            int32_t iRetValue;
            iRetValue = CFW_TcpipSocketClose(tcpipParas->uSocket);
            if (iRetValue == ERR_SUCCESS)
            {
                OSI_LOGI(0, "AT+MYNETCLOSE: close success");
                atCmdSetTimeoutHandler(cmd->engine, 60000, _timeoutabortTimeout);
                return;
            }
            else
            {
                OSI_LOGI(0, "AT+MYNETCLOSE: CSW execute wrong");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
        }
        else
        {
            tcpipParas->isTrans = isTrans;
            tcpipParas->uState = MYNET_ACCEPT;
            if (tcpipParas->isTrans == 0)
            {
                stop_transparent_mode(cmd->engine, uMuxIndex);
                atCmdRespInfoText(cmd->engine, "OK");
                if (tcpipParas->uRecvBuf.recvBuf != NULL)
                {
                    char uaRspStr[60] = {0};
                    sprintf(uaRspStr, "$MYURCREAD:%d", uMuxIndex);
                    atCmdRespInfoText(cmd->engine, uaRspStr);
                }
                atCmdRespOKText(cmd->engine, "");
            }
            else
            {
                atCmdRespInfoText(cmd->engine, "OK");
                start_transparent_mode(cmd->engine, uMuxIndex);
                int readlen = 0;

                if (tcpipParas->uRecvBuf.recvBuf != NULL)
                {
                    OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETACCEPT, tcpipParas->uRecvBuf.recvBufLen=%d", tcpipParas->uRecvBuf.recvBufLen);

                    readlen = tcpipParas->uRecvBuf.recvBufLen;

                    writeInfoNText(cmd->engine, (const char *)(tcpipParas->uRecvBuf.recvBuf + tcpipParas->uRecvBuf.readnum), readlen);
                    tcpipParas->uRecvBuf.readnum += readlen;
                    tcpipParas->uRecvBuf.recvBufLen -= readlen;

                    if (tcpipParas->uRecvBuf.recvBufLen == 0)
                    {
                        OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETACCEPT, len = %d,tcpipParas->uRecvBuf.recvBufLen=%d", readlen, tcpipParas->uRecvBuf.recvBufLen);
                        free(tcpipParas->uRecvBuf.recvBuf);

                        tcpipParas->uRecvBuf.recvBuf = NULL;
                        tcpipParas->uRecvBuf.readnum = 0;
                        //tcpipParas->uRecvBuf.recvBufLen = 0;
                        tcpipParas->uRecvBuf.recvBufMaxSize = 0;
                    }
                    else
                    {
                        OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETACCEPT,there have data left");
                    }
                }
            }
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        //atCmdRespInfoText(cmd->engine, "+MYNETACCEPT: (0-NORMAL MODE, 1-TRANSPARENT MODE)");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        /*char aucBuffer[256] = {0};
        for (int i = 0; i < MYNET_NUM_NETCONN; i++)
        {
            stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[i]);
            sprintf(aucBuffer, "$MYNETACCEPT: %d, %d, %d", i, tcpipParas->isRefuse, tcpipParas->isTrans);
            atCmdRespInfoText(cmd->engine, aucBuffer);
        }*/
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "exe in %s(), command type not allowed", __func__);
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

int stricmp(const char *str1, const char *str2)
{
    char c1, c2;

    do
    {
        c1 = *str1++;
        c2 = *str2++;
        if (c1 != c2)
        {
            char c1_upc = c1 | 0x20;
            if ((c1_upc >= 'a') && (c1_upc <= 'z'))
            {
                /* characters are not equal an one is in the alphabet range:
        downcase both chars and check again */
                char c2_upc = c2 | 0x20;
                if (c1_upc != c2_upc)
                {
                    /* still not equal */
                    /* don't care for < or > */
                    return 1;
                }
            }
            else
            {
                /* characters are not equal but none is in the alphabet range */
                return 1;
            }
        }
    } while (c1 != 0);
    return 0;
}

void AT_TCPIP_CmdFunc_MYNETCON(atCommand_t *cmd)
{

    uint8_t uCid = 0;
    uint32_t WaitTm;
    uint32_t SendSz;
    uint8_t AUTH;
    const char *type;
    char AtRet[128] = {0};
    if (cmd == NULL)
    {
        OSI_LOGI(0, "AT_GPRS_CmdFunc_MYNETCON,cmd == NULL");
        return;
    }

    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont; //, sPdpOld;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    memset(&sPdpCont, 0, sizeof(CFW_GPRS_PDPCONT_INFO_V2));

    //if (NULL == cmd)
    //{
    //    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
    //    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    //}

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;

        if (cmd->param_count != 3)

        {
            OSI_LOGI(0x10005287, "AT+MYNETCON:Parameter count error. count  = %d.", cmd->param_count);
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }

        // Get cid
        //if (1 <= cmd->param_count)
        //{
        uCid = atParamDefUintInRange(cmd->params[0], 0, 0, CHANNEL_NUM_MAX - 1, &paramok) + 1;
        if (!paramok)
        {
            OSI_LOGI(0x10005288, "ERROR! AT+MYNETCON: get cid error");
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        int ret = CFW_GprsGetPdpCxtV2(uCid, &sPdpCont, nSim);
        if (ERR_SUCCESS != ret)
        {
            OSI_LOGI(0, "ERROR! AT+MYNETCON:CFW_GprsGetPdpCxtV2,ret=%d", ret);
            memset(&sPdpCont, 0, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
        }

        // 2#
        // Get PDP_type

        type = atParamStr(cmd->params[1], &paramok);

        if (!paramok)
        {
            OSI_LOGI(0, "ERROR! AT+CGDCONT: get type error");
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }

        if (stricmp(type, "userpwd") == 0)
        {
            if (sPdpCont.nPdpType == 0)
                sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IP;
            const char *userpwd = atParamStr(cmd->params[2], &paramok);
            if (!paramok)
            {
                OSI_LOGI(0, "ERROR! AT+CGDCONT: get type value error");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            const char *delim = ","; //分隔符字符串
            char *tmpUsrName = NULL;
            char *tmpPassWd = NULL;
            tmpUsrName = strtok((char *)userpwd, delim);
            if (tmpUsrName == NULL)
            {
                OSI_LOGI(0, "ERROR! AT+CGDCONT: get tmpUserName error");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            tmpPassWd = strtok(NULL, delim); //继续调用strtok，分解剩下的字符串
            if (tmpPassWd == NULL)
            {
                OSI_LOGI(0, "ERROR! AT+CGDCONT: get tmpPassWd error");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }

            if ((NULL != tmpUsrName) && stricmp(tmpUsrName, "none"))
            {

                sPdpCont.nApnUserSize = strlen(tmpUsrName);
                if (sPdpCont.nApnUserSize >= THE_APN_USER_MAX_LEN)
                {
                    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                }
                memcpy(sPdpCont.pApnUser, tmpUsrName, sPdpCont.nApnUserSize);

                OSI_LOGI(0, "userName=%s", sPdpCont.pApnUser);
            }

            if ((NULL != tmpPassWd) && stricmp(tmpPassWd, "none"))
            {
                sPdpCont.nApnPwdSize = strlen(tmpPassWd);
                if (sPdpCont.nApnPwdSize >= THE_APN_PWD_MAX_LEN)
                {
                    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                }
                memcpy(sPdpCont.pApnPwd, tmpPassWd, sPdpCont.nApnPwdSize);
                OSI_LOGI(0, "passWord=%s", sPdpCont.pApnPwd);
            }
        }
        else if (stricmp(type, "APN") == 0)
        {
            if (sPdpCont.nPdpType == 0)
                sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IP;
            const char *tmpApn = atParamStr(cmd->params[2], &paramok);
            if (!paramok)
            {
                OSI_LOGI(0, "ERROR! AT+CGDCONT: get type value error");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            if ((tmpApn != NULL) && strcmp(tmpApn, ""))
            {
                sPdpCont.nApnSize = strlen(tmpApn);
                if (sPdpCont.nApnSize >= THE_APN_MAX_LEN)
                {
                    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                }
                memcpy(sPdpCont.pApn, tmpApn, sPdpCont.nApnSize);
                OSI_LOGI(0, "sPdpCont.nApnSize=%d", sPdpCont.nApnSize);
            }
        }
        else if (stricmp(type, "CFGT") == 0) //transparent mode time 1-65535ms/default 100ms
        {
            WaitTm = atParamDefUintInRange(cmd->params[2], 100, 1, 65535, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0, "ERROR! AT+MYNETCON: get type value error");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            gChannelParam[uCid - 1].trans_waittm = WaitTm;
            RETURN_OK(cmd->engine);
        }
        else if (stricmp(type, "CFGP") == 0) //transparent mode 1-1460/default 1024
        {
            SendSz = atParamDefUintInRange(cmd->params[2], 1024, 1, 1460, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0, "ERROR! AT+MYNETCON: get type value error");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            gChannelParam[uCid - 1].trans_sendSz = SendSz;

            RETURN_OK(cmd->engine);
        }
        else if (stricmp(type, "AUTH") == 0) //0：NONE；1：PAP；2：CHAP
        {
            AUTH = atParamDefUintInRange(cmd->params[2], 1, 0, 2, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0, "ERROR! AT+MYNETCON: get type value error");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            sPdpCont.nAuthProt = AUTH;
        }
        else
        {
            OSI_LOGI(0, "ERROR! AT+MYNETCON: get Type error");
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        CFW_GPRS_QOS null_qos = {3, 4, 3, 4, 16};
        CFW_GprsSetReqQos(uCid, &null_qos, nSim);
        OSI_LOGI(0, "AT+MYNETCON: CFW_GprsSetPdpCxtV2,uCid=%d,nSim=%d,sPdpCont.nApnSize=%d", uCid, nSim, sPdpCont.nApnSize);
        if (CFW_GprsSetPdpCxtV2(uCid, &sPdpCont, nSim) != 0)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        RETURN_OK(cmd->engine);
    }

    else if (AT_CMD_READ == cmd->type)
    {
        for (uCid = AT_PDPCID_MIN; uCid <= 6; uCid++)
        {
            uint8_t act_state;
            char pResp[600] = {0};
            char *tmpStr = pResp;

            if (CFW_GetGprsActState(uCid, &act_state, nSim) != 0)
                continue;
            if (act_state != CFW_GPRS_ACTIVED)
                continue;
            if (CFW_GprsGetPdpCxtV2(uCid, &sPdpCont, nSim) != 0)
            {
                OSI_LOGI(0, "ERROR! AT$MYNETCON?:CFW_GprsGetPdpCxtV2,error");
                memset(&sPdpCont, 0, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
                continue;
            }
            //const osiValueStrMap_t *pdp_type_vs = osiVsmapFindByVal(gPdpTypeVSMap, sPdpCont.PdnType);
            //if (pdp_type_vs == NULL)
            //continue;

            tmpStr += sprintf(tmpStr, "$MYNETCON:%d,", uCid - 1);
            if (sPdpCont.nApnSize != 0)
            {
                tmpStr += sprintf(tmpStr, "APN,\"%s\",", sPdpCont.pApn);
            }

            if (sPdpCont.nApnUserSize != 0 && sPdpCont.nApnPwdSize != 0)
            {
                tmpStr += sprintf(tmpStr, "USERPWD,\"%s,%s\",", sPdpCont.pApnUser, sPdpCont.pApnPwd);
            }

            OSI_LOGI(0, "AT+MYNETCON: sPdpCont.nPdpAddrSize:%d", sPdpCont.nPdpAddrSize);

            tmpStr += sprintf(tmpStr, "CFGT,%ld,", gChannelParam[uCid - 1].trans_waittm);

            tmpStr += sprintf(tmpStr, "CFGP,%ld,", gChannelParam[uCid - 1].trans_sendSz);

            tmpStr += sprintf(tmpStr, "AUTH,%d", sPdpCont.nAuthProt);

            atCmdRespInfoText(cmd->engine, pResp);
        }
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "ERROR! AT+MYNETCON: unknown CMD type");
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYNETSRV(atCommand_t *cmd)
{
    OSI_LOGI(0, "AT+MYNETSRV ... ... ENTER");

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t uMuxIndex = 0;
        uint8_t cid = 0;
        int8_t iResult = -1;
        char AtRet[128] = {0};
        if (cmd->param_count != 5)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        if (mynetMux_multiIp)
        {
            cid = atParamUintInRange(cmd->params[0], 0, 5, &paramok) + 1; //channel id
        }
        uMuxIndex = atParamUintInRange(cmd->params[1], 0, 5, &paramok);                             //socket index
        const uint8_t mode = atParamUintInRange(cmd->params[1 + mynetMux_multiIp], 0, 2, &paramok); //mode: 0:TCPCLIENT;1:TCPSERVER;2:UDPCLIENT
        uint8_t isText = atParamUintInRange(cmd->params[2 + mynetMux_multiIp], 0, 1, &paramok);     //1:TEXT;0:HEX
        const char *IpPortStr = atParamStr(cmd->params[3 + mynetMux_multiIp], &paramok);            //IP:PORT
        if (paramok)
        {
            uint8_t uState = 0;
            //stAT_Bearer_Paras *bearerParas = &(g_uMynetContexts.nBearerParas[gMynetBearer]);
            stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[uMuxIndex]);
            if ((tcpipParas->uMode != 1 && tcpipParas->uState != MYNET_CLOSED) || (tcpipParas->uMode == 1 && tcpipParas->uServerState != MYNETSERVER_CLOSE))
            {
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_TCPIP_ALREADY_CONNECTED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
#ifndef AT_NO_GPRS
            iResult = CFW_GetGprsActState(cid, &uState, atCmdGetSim(cmd->engine));
#endif
            if (iResult != ERR_SUCCESS)
            {
                OSI_LOGI(0, "CFW_GetGprsActState error\n");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            else
            {
                if (CFW_GPRS_ACTIVED != uState)
                {
                    OSI_LOGI(0, "have not run MYNETACT\n");
                    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PDP_DEACTIVED);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                }
            }
            const char *delim = ":"; //分隔符字符串
            char *port = NULL;
            char *p = NULL;
            p = strtok((char *)IpPortStr, delim);
            if (p == NULL)
            {
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            port = strtok(NULL, delim); //继续调用strtok，分解剩下的字符串
            if (port == NULL)
            {
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }

            //memset(tcpipParas->uaIPAddress, 0, AT_MYNET_TCPIP_HOSTLEN);
            //AT_StrNCpy(tcpipParas->uaIPAddress, p, strlen(p));
            OSI_LOGI(0, "AT+MYNETSRV:ip:%s\n", p);
            uint32_t ip = CFW_TcpipInetAddr((const char *)p);
            if (SOCKET_ERROR == ip)
            {
                OSI_LOGI(0, "the ip addr is invalid\n");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            if (mode == 1)
            {
                if (0 != strcmp(p, "127.0.0.1"))
                {
                    OSI_LOGI(0, "the ip addr is not 127.0.0.1\n");
                    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                }
                memset(tcpipParas, 0, sizeof(stAT_Tcpip_Paras));
                int32_t tmp_port = atoi(port);
                if (tmp_port > 65535 || tmp_port <= 0)
                {
                    OSI_LOGI(0, "the port is invalid\n");
                    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                }
                gMynetlocalport[uMuxIndex].tcp_port = tmp_port;
                OSI_LOGI(0, "AT+MYNETSRV:port:%d\n", gMynetlocalport[uMuxIndex].tcp_port);
                tcpipParas->uMode = 1;
                tcpipParas->uType = CFW_TCPIP_SOCK_STREAM;
                tcpipParas->uProtocol = CFW_TCPIP_IPPROTO_TCP;
            }
            else
            {
                memset(tcpipParas, 0, sizeof(stAT_Tcpip_Paras));
                gMynetlocalport[uMuxIndex].tcp_port = 0;
                gMynetlocalport[uMuxIndex].udp_port = 0;
                AT_StrNCpy(tcpipParas->uaIPAddress, p, AT_MYNET_TCPIP_HOSTLEN);
                tcpipParas->uaIPAddress[AT_MYNET_TCPIP_HOSTLEN - 1] = '\0';
                OSI_LOGI(0, "AT+MYNETSRV:ip:%s\n", p);
                int32_t tmp_port = atoi(port);
                if (tmp_port > 65535 || tmp_port <= 0)
                {
                    OSI_LOGI(0, "the port is invalid\n");
                    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                }
                tcpipParas->uPort = tmp_port;
                OSI_LOGI(0, "AT+MYNETSRV:port:%d\n", tcpipParas->uPort);
                if (mode == 0)
                {
                    tcpipParas->uMode = 0;
                    tcpipParas->uType = CFW_TCPIP_SOCK_STREAM;
                    tcpipParas->uProtocol = CFW_TCPIP_IPPROTO_TCP;
                }
                else
                {
                    tcpipParas->uMode = 2;
                    tcpipParas->uType = CFW_TCPIP_SOCK_DGRAM;
                    tcpipParas->uProtocol = CFW_TCPIP_IPPROTO_UDP;
                }
            }

            tcpipParas->uCid = cid;
            tcpipParas->isText = isText;
            //tcpipParas->uState = MYNET_CONFIG;
            RETURN_OK(cmd->engine);
        }
        else
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[128] = {0};
        if (!mynetMux_multiIp)
        {
            sprintf(aucBuffer, "$MYNETSRV:");
            atCmdRespInfoText(cmd->engine, aucBuffer);
        }
        else
        {
            for (int i = 0; i < MYNET_NUM_NETCONN; i++)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[i]);
                uint16_t port = 0;
                char *ip = NULL;
                if (tcpipParas->uMode == 1)
                    port = gMynetlocalport[i].tcp_port;
                else
                    port = tcpipParas->uPort;
                if (tcpipParas->uMode == 1)
                    ip = "127.0.0.1";
                else
                    ip = (char *)tcpipParas->uaIPAddress;
                sprintf(aucBuffer, "$MYNETSRV: %d, %d, %d, %d, %s:%d", tcpipParas->uCid <= 0 ? 0 : (tcpipParas->uCid - 1), i, tcpipParas->uMode, tcpipParas->isText, ip, port);
                atCmdRespInfoText(cmd->engine, aucBuffer);
            }
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        //char aucBuffer[128] = {0};
        //if (!mynetMux_multiIp)
        //sprintf(aucBuffer, "$MYNETSRV: <Channel>,<SocketID>,<nettype>,<viewMode>,<ip:port>");
        //else
        //sprintf(aucBuffer, "+CLPORT: (0-7),(\"TCP\",\"UDP\"),(0-65535)");
        //atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10005289, "AT_TCPIP_CmdFunc_+MYNETSRV, command type:%d not allowed", cmd->type);
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYNETOPEN(atCommand_t *cmd)
{
    int8_t iResult;
    uint8_t uState;
    char AtRet[128] = {0};
    if (AT_CMD_SET == cmd->type)
    {
        OSI_LOGI(0, "AT+MYNETOPEN ... ... enter");
        if (cmd->param_count != 1)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }

        bool paramok = true;
        uint8_t uMuxIndex = -1;

        uMuxIndex = atParamUintInRange(cmd->params[0], 0, 5, &paramok); //SOCKET INDEX
        if (!paramok)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }

        stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[uMuxIndex]);
        if (tcpipParas->uCid == 0)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }

        if (tcpipParas->engine != NULL && tcpipParas->engine != cmd->engine)
        {
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        tcpipParas->engine = cmd->engine;

        iResult = CFW_GetGprsActState(tcpipParas->uCid, &uState, atCmdGetSim(cmd->engine));

        if (iResult != ERR_SUCCESS)
        {
            tcpipParas->engine = NULL;
            OSI_LOGI(0, "CFW_GetGprsActState error\n");
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        else
        {
            if (CFW_GPRS_ACTIVED != uState)
            {
                tcpipParas->engine = NULL;
                OSI_LOGI(0, "have not run MYNETACT\n");
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_PDP_DEACTIVED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
        }
        if (tcpipParas->uMode == 1)
        {
            if (tcpipParas->uServerState != MYNETSERVER_CLOSE)
            {
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_TCPIP_ALREADY_CONNECTED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            tcpipParas->isTrans = 0;
            tcpipParas->isOnlySocketClose = 0;
            if (ERR_SUCCESS == AT_TCPIP_ServerStart(cmd->engine, uMuxIndex))
            {
                tcpipParas->uServerState = MYNETSERVER_OPEN;
                sprintf(AtRet, "$MYNETOPEN:%d", uMuxIndex);
                atCmdRespInfoText(cmd->engine, AtRet);
                atCmdRespOK(cmd->engine);
            }
            else
            {
                tcpipParas->engine = NULL;
                OSI_LOGI(0, "AT_TCPIP_ServerStart failed\n");
#if 0
                if (iResult == -3)
                {
                    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_TCPIP_ALREADY_CONNECTED);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, (const char *)AtRet));
                }
                else
#endif
                {
                    sprintf(AtRet, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
                }
            }
        }
        else
        {
            if (tcpipParas->uState != MYNET_CLOSED)
            {
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_TCPIP_ALREADY_CONNECTED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            char aucBuffer[40] = {0};

            tcpipParas->uDomain = CFW_TCPIP_AF_INET;
            //tcpipParas->uConnectBearer = gMynetBearer;
            //tcpipParas->uState = bearerParas->nState;
            tcpipParas->isTrans = 0;

            int32_t iResult = 0;

            iResult = AT_TCPIP_Connect(cmd->engine, uMuxIndex);
            atCmdSetTimeoutHandler(cmd->engine, 90000, _timeoutabortTimeout);
            if (iResult != ERR_SUCCESS)
            {
                tcpipParas->engine = NULL;
                if (iResult == -2)
                {
                    sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_DNS_FAILED);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, (const char *)aucBuffer));
                }
                if (iResult == -3)
                {
                    sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_TCPIP_ALREADY_CONNECTED);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, (const char *)aucBuffer));
                }
                if (!mynetMux_multiIp)
                    sprintf(aucBuffer, "%d,CONNECT FAIL", uMuxIndex);
                else
                    sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                atCmdRespErrorText(cmd->engine, (const char *)aucBuffer);
            }
            //AT_SetAsyncTimerMux(cmd->engine, 90);
        }
    }
    else if (AT_CMD_READ == cmd->type)
    {
        for (int i = 0; i < MYNET_NUM_NETCONN; i++)
        {
            stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[i]);
            if (tcpipParas->uState != MYNET_CLOSED || tcpipParas->uServerState != MYNETSERVER_CLOSE)
            {
                char aucBuffer[256] = {0};

                uint8_t cid = tcpipParas->uCid;
                uint8_t nSim = atCmdGetSim(cmd->engine);
                struct netif *netif = netif_get_by_cid(cid);
                if (netif == NULL)
                    continue;

                CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
                CFW_GprsGetPdpCxtV2(cid, &sPdpCont, nSim);

                ip4_addr_t ip4_dns1 = {0};
                ip4_addr_t ip4_dns2 = {0};

                if (sPdpCont.nPdpDnsSize != 0)
                {
                    IP4_ADDR(&ip4_dns1, sPdpCont.pPdpDns[0], sPdpCont.pPdpDns[1], sPdpCont.pPdpDns[2], sPdpCont.pPdpDns[3]);
                    IP4_ADDR(&ip4_dns2, sPdpCont.pPdpDns[21], sPdpCont.pPdpDns[22], sPdpCont.pPdpDns[23], sPdpCont.pPdpDns[24]);
                    OSI_LOGI(0, "DNS size:%d  DNS1:%d.%d.%d.%d", sPdpCont.nPdpDnsSize,
                             sPdpCont.pPdpDns[0], sPdpCont.pPdpDns[1], sPdpCont.pPdpDns[2], sPdpCont.pPdpDns[3]);
                    OSI_LOGI(0, "DNS2:%d.%d.%d.%d",
                             sPdpCont.pPdpDns[21], sPdpCont.pPdpDns[22], sPdpCont.pPdpDns[23], sPdpCont.pPdpDns[24]);
                }
                CFW_TCPIP_SOCKET_ADDR stLocalAddr = {
                    0,
                };

                int addrlen = sizeof(struct sockaddr_storage);
                CFW_TcpipSocketGetsockname(tcpipParas->uSocket, &stLocalAddr, (int *)(&addrlen));
                OSI_LOGI(0, " port:%d %d", stLocalAddr.sin_port, ntohs(stLocalAddr.sin_port));
                AT_StrNCpy(tcpipParas->uLocalip, inet_ntoa(stLocalAddr.sin_addr), AT_MYNET_TCPIP_HOSTLEN);
                tcpipParas->uLocalip[AT_MYNET_TCPIP_HOSTLEN - 1] = '\0';
                OSI_LOGXI(OSI_LOGPAR_S, 0, "tcpipParas->uLocalip=%s", tcpipParas->uLocalip);
                if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_UDP)
                    gMynetlocalport[i].udp_port = ntohs(stLocalAddr.sin_port);
                else
                    gMynetlocalport[i].tcp_port = ntohs(stLocalAddr.sin_port);
                sprintf(aucBuffer, "$MYNETOPEN: %d,%s,%d",
                        i,
                        tcpipParas->uLocalip,
                        (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_UDP) ? gMynetlocalport[i].udp_port : gMynetlocalport[i].tcp_port);
                char tmpbuf[128] = {0};
                sprintf(tmpbuf, ",%s", ipaddr_ntoa(&(netif->gw)));
                strcat(aucBuffer, tmpbuf);
                memset(tmpbuf, 0, 128);
                sprintf(tmpbuf, ",%s", sPdpCont.nPdpDnsSize == 0 ? "0.0.0.0" : ip4addr_ntoa(&ip4_dns1));
                strcat(aucBuffer, tmpbuf);
                memset(tmpbuf, 0, 128);
                sprintf(tmpbuf, ",%s", sPdpCont.nPdpDnsSize == 0 ? "0.0.0.0" : ip4addr_ntoa(&ip4_dns2));
                strcat(aucBuffer, tmpbuf);
                memset(tmpbuf, 0, 128);
                sprintf(tmpbuf, ",%d,%s,%d", tcpipParas->uMode,
                        0 == strcmp((char *)(tcpipParas->uaIPAddress), "") ? "0.0.0.0" : (char *)(tcpipParas->uaIPAddress),
                        tcpipParas->uPort);
                strcat(aucBuffer, tmpbuf);
                atCmdRespInfoText(cmd->engine, (const char *)aucBuffer);
            }
        }

        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETOPEN(), command type not allowed");
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

static int ParamHex2Num(uint8_t c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 0xa + (c - 'A');
    if (c >= 'a' && c <= 'f')
        return 0xa + (c - 'a');
    return -1;
}

int32_t AT_MYNET_STRtoHEX(uint8_t *p, uint32_t len)
{
    const uint8_t *pv = p;
    for (unsigned n = 0; n < len; n++)
    {
        if (ParamHex2Num(*pv++) < 0)
            return -1;
    }

    uint8_t *s = p;
    uint8_t *d = p;
    uint32_t i = len % 2;
    uint32_t j = len / 2;
    if (i == 1)
    {
        *d++ = ParamHex2Num(s[0]);
        s += 1;
    }
    for (unsigned n = 0; n < j; n++)
    {
        uint32_t num0 = (uint32_t)ParamHex2Num(s[0]);
        uint32_t num1 = (uint32_t)ParamHex2Num(s[1]);
        *d++ = (num0 << 4) | num1;
        s += 2;
    }
    return j + i;
}

static void tcpipAsyncCtxDestroy(atCommand_t *cmd)
{
    tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)cmd->async_ctx;
    if (async == NULL)
        return;
    if (async->data != NULL)
        free(async->data);
    free(async);
    cmd->async_ctx = NULL;
}

static void _sendTimeout(void *cmd)
{
    //atCmdRespErrorCode(cmd->engine, ERR_AT_CME_SEND_TIMEOUT);
    char AtRet[128] = {0};
    if (cmd == NULL)
    {
        OSI_LOGI(0, "_sendTimeout cmd == NULL");
        return;
    }
    uint32_t nMuxIndex = (uint32_t)cmd;

    stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[nMuxIndex]);
    if (tcpipParas->isSendTimeout == 0 && tcpipParas->sendTOtimer != NULL)
    {
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_SEND_TIMEOUT);
        atCmdRespErrorText(tcpipParas->engine, AtRet);
        tcpipParas->isSendTimeout = 1;
    }
    if (tcpipParas->sendTOtimer != NULL)
    {
        osiTimerStop(tcpipParas->sendTOtimer);
        osiTimerDelete(tcpipParas->sendTOtimer);
        tcpipParas->sendTOtimer = NULL;
    }
}

static void _tcpipSend(atCommand_t *cmd, uint32_t uMuxIndex, uint8_t *send_data, uint32_t uLength)
{
    stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[uMuxIndex]);
    int32_t iResult;

    if (tcpipParas->uProtocol != CFW_TCPIP_IPPROTO_UDP)
    {
        iResult = CFW_TcpipSocketSend(tcpipParas->uSocket, (uint8_t *)send_data, (uint16_t)uLength, 0);
        if (iResult == SOCKET_ERROR)
        {
            OSI_LOGI(0, "TCPIP send socket data error");
        }
        else
        {
            tcpipParas->sendSize = iResult;
            tcpipParas->sentSize = 0;
        }
    }
    else
    {
        CFW_TCPIP_SOCKET_ADDR nDestAddr;
        nDestAddr.sin_family = CFW_TCPIP_AF_INET;
        nDestAddr.sin_port = htons(tcpipParas->uPort);
        nDestAddr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);
        iResult = CFW_TcpipInetAddr((const char *)tcpipParas->uaIPAddress);
        if (SOCKET_ERROR == iResult)
        {
            OSI_LOGI(0, "AT_TCPIP_CmdFunc_CIPSEND CFW_TcpipInetAddr() failed\n");
        }
        else
        {
            nDestAddr.sin_addr.s_addr = iResult;
            OSI_LOGI(0, "dest_addr.sin_addr.s_addr send 0x%x \n", nDestAddr.sin_addr.s_addr);
            iResult = CFW_TcpipSocketSendto(tcpipParas->uSocket, (uint8_t *)send_data, (uint16_t)uLength, 0, &nDestAddr, nDestAddr.sin_len);
            if (iResult == SOCKET_ERROR)
            {

                OSI_LOGI(0, "UDP send socket data error");
            }
        }
    }
    if (mynetATS_setTimer)
    {
        at_mynetTCPIPBufDestroy(g_mynetbypass_buf);
        g_mynetbypass_buf = NULL;
    }
    char aucBuffer[40] = {0};
    //AT_SetSendTimerMux(cmd->engine, 60);
    //atCmdSetTimeoutHandler(cmd->engine, 60000, _sendTimeout);

    if (iResult != SOCKET_ERROR)
    {
        if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_UDP)
        {
            AT_CMD_RETURN(atCmdRespOKText(cmd->engine, "OK"));
        }
        else
        {
            if (tcpipParas->sendTOtimer != NULL)
            {
                osiTimerDelete(tcpipParas->sendTOtimer);
                tcpipParas->sendTOtimer = NULL;
            }
            tcpipParas->isSendTimeout = 0;
            tcpipParas->sendTOtimer = osiTimerCreate(osiThreadCurrent(), _sendTimeout, (void *)uMuxIndex);
            if (NULL == tcpipParas->sendTOtimer)
            {
                OSI_LOGI(0, "tcpipParas->sendTOtimer create failed");
            }
            else
            {
                osiTimerStart(tcpipParas->sendTOtimer, 60000);
            }
        }
    }
    else
    {
        if (!mynetMux_multiIp)
            sprintf(aucBuffer, "SEND FAIL");
        else
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
    }
}

static int _tcpipDataBypassCB(void *param, const void *data, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)cmd->async_ctx;

    OSI_LOGI(0, "AT CIPSEND size/%d pos/%d data_size/%d",
             async->size, async->pos, size);

    size_t len = async->size - async->pos;
    if (len <= 0)
    {
        OSI_LOGI(0, "AT CIPSEND len=%d", len);
        return 0;
    }
    if (len > size)
        len = size;
    memcpy(async->data + async->pos, data, len);
    async->pos += len;

    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    if (chsetting->ate)
        atCmdWrite(cmd->engine, data, len);
    int hexModelen = 0;
    if (async->pos >= async->size)
    {
        stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[async->uMuxIndex]);
        if (tcpipParas->isText == 1)
        {
            hexModelen = AT_MYNET_STRtoHEX(async->data, async->size);
            if (hexModelen < 0)
            {
                char aucBuffer[40] = {0};
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                atCmdRespErrorText(cmd->engine, aucBuffer);
                return len;
            }
        }
        _tcpipSend(cmd, async->uMuxIndex, async->data, (tcpipParas->isText == 1) ? hexModelen : async->size);
    }
    return len;
}

/********************************************************************************************
* Name:                    AT_TCPIP_CmdFunc_MYNETSEND
* Description:      Send data through TCP or UDP connection
* Parameter:        AT_CMD_PARA *cmd
* Return:               void
* Remark:               After cmds +CGATT,+CGDCONT,+CGACT,+MYNETSTART return OK,this cmd can be run
* Author/Modifier:      YangYang,wulc
* Data:                 2008-5-21
********************************************************************************************/
void AT_TCPIP_CmdFunc_MYNETSEND(atCommand_t *cmd)
{
    // OSI_LOGI(0, "AT+MYNETSEND: cmd->type = %d, cmd->nDLCI = %d", cmd->type, cmd->nDLCI);

    if (AT_CMD_SET == cmd->type)
    {
        uint32_t uLength = 0;
        uint32_t uMuxIndex = 0;
        bool paramok = true;
        char outputText[128] = {0};

        if (cmd->param_count != 2)
        {
            sprintf(outputText, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, outputText));
        }
        uMuxIndex = atParamUintInRange(cmd->params[0], 0, 5, &paramok);
        uLength = atParamUintInRange(cmd->params[1], 1, 1460, &paramok);

        if (!paramok)
        {
            sprintf(outputText, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, outputText));
        }

        sprintf(outputText, "$MYNETWRITE:%ld,%ld", uMuxIndex, uLength);
        atCmdRespInfoText(cmd->engine, outputText);

        stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[uMuxIndex]);
        if (tcpipParas->engine != NULL && tcpipParas->engine != cmd->engine)
        {
            OSI_LOGI(0x0, "tcpipParas->engine  %x, cmd->engine %x", tcpipParas->engine, cmd->engine);
            char AtRet[128] = {0};
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        if (tcpipParas->uState != MYNET_CONNECTED && tcpipParas->uState != MYNET_ACCEPT)
        {
            OSI_LOGI(0, "AT+MYNETSEND: uLength is %d, tcpipParas->uState=%d", uLength, tcpipParas->uState);
            sprintf(outputText, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, outputText));
        }
        if (AT_CMD_LINE_BUFF_LEN < uLength)
        {
            OSI_LOGI(0, "AT+MYNETSEND: uLength is %d, tcpipParas->uState=%d", uLength, tcpipParas->uState);
            sprintf(outputText, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, outputText));
        }
        OSI_LOGI(0, "AT+MYNETSEND: cmd->type = %d, uMuxIndex = %d,uLength=%d", cmd->type, uMuxIndex, uLength);
        tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
        {
            sprintf(outputText, "ERROR:%d", ERR_AT_CME_BUFFER_FULL);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, outputText));
        }

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = tcpipAsyncCtxDestroy;
        if (tcpipParas->isText == 1)
        {
            async->size = 2 * uLength;
            async->data = (uint8_t *)malloc(2 * uLength);
        }
        else
        {
            async->size = uLength;
            async->data = (uint8_t *)malloc(uLength);
        }
        async->pos = 0;

        if (async->data == NULL)
        {
            sprintf(outputText, "ERROR:%d", ERR_AT_CME_BUFFER_FULL);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, outputText));
        }
        async->uMuxIndex = uMuxIndex;
        //atCmdRespOutputPrompt(cmd->engine);
        atCmdSetBypassMode(cmd->engine, _tcpipDataBypassCB, cmd);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETSEND(), command type not allowed");
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYNETREAD(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t uMuxIndex = 0;
        uint32_t requestLen = 0;
        int32_t iResult = -1;
        char outputText[128] = {0};

        uMuxIndex = atParamUintInRange(cmd->params[0], 0, 5, &paramok);

        requestLen = atParamUintInRange(cmd->params[1], 1, 1460, &paramok);

        if (paramok)
        {
            stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[uMuxIndex]);
            if (tcpipParas->engine != NULL && tcpipParas->engine != cmd->engine)
            {
                OSI_LOGI(0x0, "tcpipParas->engine  %x, cmd->engine %x", tcpipParas->engine, cmd->engine);
                char AtRet[128] = {0};
                sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
            }
            if (tcpipParas->uState != MYNET_CONNECTED && tcpipParas->uState != MYNET_ACCEPT)
            {
                OSI_LOGI(0, "AT+MYNETREAD: tcpipParas->uState=%d", tcpipParas->uState);
                sprintf(outputText, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, outputText));
            }
            if (tcpipParas->isText == 0)
                iResult = recv_data(tcpipParas->uSocket, requestLen, cmd->engine, uMuxIndex, 2);
            else
                iResult = recv_data(tcpipParas->uSocket, requestLen, cmd->engine, uMuxIndex, 3);
            if (iResult == ERR_SUCCESS)
            {
                RETURN_OK(cmd->engine);
            }
            else
            {
                sprintf(outputText, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, outputText));
            }
        }
        else
        {
            sprintf(outputText, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, outputText));
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETRXGET, command type:%d not allowed", cmd->type);
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYNETCREATE(atCommand_t *cmd)
{
    int8_t iResult = -1;
    uint8_t uState = -1;
    uint8_t cid = 0;
    char aucBuffer[128] = {0};

    // *bearerParas = &(g_uMynetContexts.nBearerParas[gMynetBearer]);
    //OSI_LOGI(0, "AT+MYNETCREATE ... ... cmd->nDLCI= %d", cmd->nDLCI);

    if (AT_CMD_SET == cmd->type)
    {
        OSI_LOGI(0, "AT+MMYNETCREATE ... set");

        bool paramok = true;
        uint8_t uMuxIndex = -1;
        uint16_t localPort = 0;
        if (cmd->param_count < 5 || cmd->param_count > 6)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }

        if (mynetMux_multiIp)
        {
            cid = atParamUintInRange(cmd->params[0], 0, CHANNEL_NUM_MAX - 1, &paramok) + 1; //channel id
        }
        const uint8_t mode = atParamUintInRange(cmd->params[0 + mynetMux_multiIp], 0, 2, &paramok); //mode: 0:TCPCLIENT;1:TCPSERVER;2:UDPCLIENT
        uMuxIndex = atParamUintInRange(cmd->params[1 + mynetMux_multiIp], 0, 5, &paramok);          //socket index
        //uint8_t isText = atParamUintInRange(cmd->params[2 + mynetMux_multiIp], 0, 1, &paramok);//1:TEXT;0:HEX
        const char *IpStr = atParamStr(cmd->params[2 + mynetMux_multiIp], &paramok);               //IP:PORT
        uint16_t port = atParamUintInRange(cmd->params[3 + mynetMux_multiIp], 0, 65535, &paramok); //port
        if (cmd->param_count > 5)
        {
            localPort = atParamUintInRange(cmd->params[4 + mynetMux_multiIp], 0, 65535, &paramok); //socket index
        }
        if (!paramok)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }
        if (mode == 1 && localPort == 0)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }
        stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[uMuxIndex]);
        if ((tcpipParas->uMode != 1 && tcpipParas->uState != MYNET_CLOSED) || (tcpipParas->uMode == 1 && tcpipParas->uServerState != MYNETSERVER_CLOSE))
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_TCPIP_ALREADY_CONNECTED);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }

        iResult = CFW_GetGprsActState(cid, &uState, atCmdGetSim(cmd->engine));

        if (iResult != ERR_SUCCESS)
        {
            OSI_LOGI(0, "CFW_GetGprsActState error\n");
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }
        else
        {
            if (CFW_GPRS_ACTIVED != uState)
            {
                OSI_LOGI(0, "have not run MYNETACT\n");
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PDP_DEACTIVED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
        }

        if (tcpipParas->engine != NULL && tcpipParas->engine != cmd->engine)
        {
            char AtRet[128] = {0};
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        tcpipParas->engine = cmd->engine;
        tcpipParas->uCid = cid;
        tcpipParas->isText = 0;
        memset(tcpipParas->uaIPAddress, 0, AT_MYNET_TCPIP_HOSTLEN);
        AT_StrNCpy(tcpipParas->uaIPAddress, IpStr, AT_MYNET_TCPIP_HOSTLEN);
        tcpipParas->uaIPAddress[AT_MYNET_TCPIP_HOSTLEN - 1] = '\0';
        OSI_LOGI(0, "AT+MYNETCREATE:ip:%s\n", IpStr);
        tcpipParas->uPort = port;
        tcpipParas->isTrans = 1;

        if (mode == 0)
        {
            tcpipParas->uMode = 0;
            tcpipParas->uType = CFW_TCPIP_SOCK_STREAM;
            tcpipParas->uProtocol = CFW_TCPIP_IPPROTO_TCP;
            if (localPort != 0)
                gMynetlocalport[uMuxIndex].tcp_port = localPort;
        }
        else if (mode == 1)
        {
            tcpipParas->uMode = 1;
            tcpipParas->uType = CFW_TCPIP_SOCK_STREAM;
            tcpipParas->uProtocol = CFW_TCPIP_IPPROTO_TCP;
            gMynetlocalport[uMuxIndex].tcp_port = localPort;
        }
        else
        {
            tcpipParas->uMode = 2;
            tcpipParas->uType = CFW_TCPIP_SOCK_DGRAM;
            tcpipParas->uProtocol = CFW_TCPIP_IPPROTO_UDP;
            if (localPort != 0)
                gMynetlocalport[uMuxIndex].udp_port = localPort;
        }

        if (tcpipParas->uMode != 1)
        {
            tcpipParas->uDomain = CFW_TCPIP_AF_INET;
            //tcpipParas->uConnectBearer = gMynetBearer;
            //tcpipParas->uState = bearerParas->nState;
            iResult = AT_TCPIP_Connect(cmd->engine, uMuxIndex);
            if (iResult != ERR_SUCCESS)
            {
                OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETCREATE(), connect failed: %d\n", iResult);
                tcpipParas->engine = NULL;
                if (iResult == -2)
                {
                    sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_DNS_FAILED);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, (const char *)aucBuffer));
                }
                if (iResult == -3)
                {
                    sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_TCPIP_ALREADY_CONNECTED);
                    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, (const char *)aucBuffer));
                }

                if (!mynetMux_multiIp)
                    sprintf(aucBuffer, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
                else
                    sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, (const char *)aucBuffer));
            }

            else
            {
                atCmdSetTimeoutHandler(cmd->engine, 90000, _timeoutabortTimeout);
            }
        }
        else
        {
            if (ERR_SUCCESS == (iResult = AT_TCPIP_ServerStart(cmd->engine, uMuxIndex)))
            {
                tcpipParas->uServerState = MYNETSERVER_OPEN;
                atCmdRespOK(cmd->engine);
            }
            else
            {
                tcpipParas->engine = NULL;
                if (iResult == -3)
                    sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_TCPIP_ALREADY_CONNECTED);
                else
                    sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
        }
    }
    else if (AT_CMD_READ == cmd->type)
    {
        /*for (int i = 0; i < MYNET_NUM_NETCONN; i++)
        {
            stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[i]);
            sprintf(aucBuffer, "$MYNETREADY: %d,%d", tcpipParas->uMode, i);
            atCmdRespInfoText(cmd->engine, (const char *)aucBuffer);
        }*/
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETOPEN(), command type not allowed");
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYNETACK(atCommand_t *cmd)
{
    OSI_LOGI(0, "AT+MYNETACK ... ... ENTER");
    char aucBuffer[128] = {0};
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;

        uint8_t uMuxIndex = atParamUintInRange(cmd->params[0], 0, 5, &paramok);
        if (!paramok)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }
        stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[uMuxIndex]);
        int32_t acklen = tcpipParas->sendSize - tcpipParas->sentSize;
        int32_t iResult = CFW_TcpipAvailableBuffer(tcpipParas->uSocket);
        char aucBuffer[40] = {0};
        if (acklen < 0)
            acklen = 0;
        if (iResult < 0)
            iResult = 0;
        sprintf(aucBuffer, "$MYNETACK:%d,%ld,%ld", uMuxIndex, acklen, iResult);
        atCmdRespInfoText(cmd->engine, (const char *)aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x1000528a, "AT_TCPIP_CmdFunc_MYNETACK, command type:%d not allowed", cmd->type);
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

/*****************************************************************************

* Name:        void AT_TCPIP_CmdFunc_MYNETCLOSE

* Description: Only close the connection at the status of TCP/UDP CONNECTING or CONNECT OK. otherwise
                    response error. after close the connection, the status is IP CLOSE.

* Parameter:  AT_CMD_PARA *cmd

* Return:       void

* Remark:      n/a

* Author:       Chang Yuguang

* Data:          2008-5-19

******************************************************************************/
void AT_TCPIP_CmdFunc_MYNETCLOSE(atCommand_t *cmd)
{
    OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYNETCLOSE processing\n");
    char aucBuffer[128] = {0};
    if (cmd->type == AT_CMD_SET)
    {

        if (cmd->param_count != 1)
        {
            OSI_LOGI(0, "AT+MYNETCLOSE: pPara is error uParaCount=%d", cmd->param_count);

            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }

        uint8_t uMuxIndex = 0;
        bool paramok = true;
        int32_t iRetValue;

        uMuxIndex = atParamUintInRange(cmd->params[0], 0, 5, &paramok);

        if (!paramok)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }

        stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[uMuxIndex]);
        if (tcpipParas->engine != NULL && atCmdEngineIsValid(tcpipParas->engine) && tcpipParas->engine != cmd->engine)
        {
            OSI_LOGI(0x0, "tcpipParas->engine  %x, cmd->engine %x", tcpipParas->engine, cmd->engine);
            char AtRet[128] = {0};
            sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
        }
        if (tcpipParas->uMode != 1)
        {
            if (tcpipParas->uState <= MYNET_CLOSING)
            {
                OSI_LOGI(0, "AT+MYNETCLOSE: can't run at this state:%d", tcpipParas->uState);
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }

            iRetValue = CFW_TcpipSocketClose(tcpipParas->uSocket);
            if (iRetValue == ERR_SUCCESS)
            {
                OSI_LOGI(0, "AT+MYNETCLOSE: close success");
                atCmdSetTimeoutHandler(cmd->engine, 60000, _timeoutabortTimeout);
                return;
            }
            else
            {
                OSI_LOGI(0, "AT+MYNETCLOSE: CSW execute wrong");
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
        }
        else
        {
            if (tcpipParas->uServerState == MYNETSERVER_CLOSE)
            {
                OSI_LOGI(0, "AT+MYNETCLOSE: can't run at this state:%d", tcpipParas->uState);
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
            uint8_t cur_serverSock = tcpipParas->isOnlySocketClose;
            if (tcpipParas->isOnlySocketClose == 1)
            {
                OSI_LOGI(0, "AT+CIPSERVER: gCipserver.isOnlySocketClose == 1 set to 0");
                tcpipParas->isOnlySocketClose = 0;
            }
            for (int i = 0; i < MYNET_NUM_NETCONN; i++)
            {
                stAT_Tcpip_Paras *tmp_tcpipParas = &(g_uMynetContexts.nTcpipParas[i]);
                if (tmp_tcpipParas->listening_socket == tcpipParas->uSocket)
                {
                    iRetValue = CFW_TcpipSocketClose(tmp_tcpipParas->uSocket);
                    if (iRetValue == ERR_SUCCESS)
                    {
                        OSI_LOGI(0, "AT+MYNETCLOSE: close success");
                        tmp_tcpipParas->uState = MYNET_CLOSING;
                        //start_tcpip_close_timer(cmd->nDLCI, uMuxIndex);
                    }
                }
            }
            if ((cur_serverSock != 1) && ERR_SUCCESS != (iRetValue = CFW_TcpipSocketClose(tcpipParas->uSocket)))
            {
                OSI_LOGI(0, "AT+MYNETCLOSE: CSW execute wrong");
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
            else if (cur_serverSock == 1)
            {
                //memset(tcpipParas, 0, sizeof(stAT_Tcpip_Paras));
                tcpipParas->uServerState = MYNETSERVER_CLOSE;
                tcpipParas->uState = MYNET_CLOSED;
                tcpipParas->uSocket = 0;
                tcpipParas->sendSize = 0;
                tcpipParas->sentSize = 0;

                memset(tcpipParas->uLocalip, 0, AT_MYNET_TCPIP_HOSTLEN);
                gMynetlocalport[uMuxIndex].tcp_port = 0;
                gMynetlocalport[uMuxIndex].udp_port = 0;

                if (tcpipParas->sendTimerID != NULL)
                {
                    osiTimerStop(tcpipParas->sendTimerID);
                    osiTimerDelete(tcpipParas->sendTimerID);
                    tcpipParas->sendTimerID = NULL;
                }
                memset(&(tcpipParas->from_addr), 0, sizeof(CFW_TCPIP_SOCKET_ADDR));
                sprintf(aucBuffer, "$MYNETCLOSE:%d", uMuxIndex);
                //stop_tcpip_close_timer(nDlc, nMuxIndex);
                atCmdRespInfoText(cmd->engine, aucBuffer);
                atCmdRespOK(cmd->engine);
            }
            else
            {

                OSI_LOGI(0, "AT+MYNETCLOSE: close success");
                //start_tcpip_close_timer(cmd->nDLCI, uMuxIndex);
                atCmdSetTimeoutHandler(cmd->engine, 60000, _timeoutabortTimeout);
                return;
            }
        }
    }
    else if (cmd->type == AT_CMD_READ)
    {
        /*char aucBuffer[40] = {0};
        for (int i = 0; i < MYNET_NUM_NETCONN; i++)
        {
            stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[i]);
            if (tcpipParas->uState == MYNET_CLOSED)
            {
                sprintf(aucBuffer, "$MYNETCLOSE: %d", i);
                atCmdRespInfoText(cmd->engine, aucBuffer);
            }
        }*/
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "AT+MYNETCLOSE: command not supported");
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYIPFILTER(atCommand_t *cmd)
{

    bool paramok = true;
    uint8_t action = 0;
    uint8_t id = 0;
    char aucBuffer[128] = {0};
    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count < 2)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }
        id = atParamUintInRange(cmd->params[0], 0, 4, &paramok);
        action = atParamUintInRange(cmd->params[1], 0, 2, &paramok); //action: 0:delete;1:add;2:delete all
        if (!paramok)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }
        if (action == 2)
        {
            if (cmd->param_count != 2)
            {
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
            for (int i = 0; i < 5; i++)
            {
                g_ipFilter[i].isValid = 0;
                memset(g_ipFilter[i].ipAddr, 0, AT_MYNET_TCPIP_ADDRLEN);
                memset(g_ipFilter[i].netMask, 0, AT_MYNET_TCPIP_ADDRLEN);
            }
        }
        else if (action == 0)
        {
            if (cmd->param_count > 4)
            {
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
            g_ipFilter[id].isValid = 0;
            memset(g_ipFilter[id].ipAddr, 0, AT_MYNET_TCPIP_ADDRLEN);
            memset(g_ipFilter[id].netMask, 0, AT_MYNET_TCPIP_ADDRLEN);
        }
        else
        {
            if (cmd->param_count != 4)
            {
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
            const char *ipAddr = atParamStr(cmd->params[2], &paramok);
            const char *netMask = atParamStr(cmd->params[3], &paramok);
            if (!paramok)
            {
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
            uint32_t filterip = CFW_TcpipInetAddr((const char *)ipAddr);
            if (SOCKET_ERROR == filterip)
            {
                OSI_LOGI(0, "ipAddr CFW_TcpipInetAddr() failed\n");
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
            uint32_t mask = CFW_TcpipInetAddr((const char *)netMask);
            if ((0 != strcmp(netMask, "255.255.255.255")) && (SOCKET_ERROR == mask))
            {
                OSI_LOGI(0, "netMask  CFW_TcpipInetAddr() failed\n");
                sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }
            g_ipFilter[id].isValid = 1;
            strncpy(g_ipFilter[id].ipAddr, ipAddr, AT_MYNET_TCPIP_ADDRLEN);
            g_ipFilter[id].ipAddr[AT_MYNET_TCPIP_ADDRLEN - 1] = '\0';
            strncpy(g_ipFilter[id].netMask, netMask, AT_MYNET_TCPIP_ADDRLEN);
            g_ipFilter[id].netMask[AT_MYNET_TCPIP_ADDRLEN - 1] = '\0';
        }
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char aucBuffer[128] = {0};
        for (int i = 0; i < 5; i++)
        {
            if (g_ipFilter[i].isValid == 1)
            {
                sprintf(aucBuffer, "$MYIPFILTER: %d, %s, %s", i, g_ipFilter[i].ipAddr, (g_ipFilter[i].isValid == 1) ? g_ipFilter[i].netMask : "0.0.0.0");
                atCmdRespInfoText(cmd->engine, aucBuffer);
            }
        }
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char aucBuffer[128] = {0};
        sprintf(aucBuffer, "$MYIPFILTER: <id>,<ip_address>,<net_mask>");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "AT+MYNETCLOSE: command not supported");
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYNETURC(atCommand_t *cmd)
{

    bool paramok = true;
    char aucBuffer[128] = {0};
    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count != 1)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }

        uint32_t enable = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }
        g_mynetUrc = enable;
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char aucBuffer[128] = {0};
        sprintf(aucBuffer, "$MYNETURC: %ld", g_mynetUrc);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "AT+MYNETCLOSE: command not supported");
        char AtRet[128] = {0};
        sprintf(AtRet, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, AtRet));
    }
}

void AT_TCPIP_CmdFunc_MYSOCKETLED(atCommand_t *cmd)
{

    bool paramok = true;
    char aucBuffer[128] = {0};
    int i = 0;
    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count != 1)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }

        uint8_t socketLed = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
        {
            sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }
        OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYSOCKETLED,socketled=%d", socketLed);
        CFW_SIM_ID nSim = (CFW_SIM_ID)atCmdGetSim(cmd->engine);
        for (i = 1; i <= AT_PDPCID_MAX; i++)
        {
            struct netif *netif = getGprsNetIf(nSim, i);
            OSI_LOGI(0, "AT_TCPIP_CmdFunc_MYSOCKETLED,netif=%p", netif);
            if (netif != NULL && netif->is_ppp_mode == 1)
            {
                break;
            }
        }
        if (i > AT_PDPCID_MAX)
            RETURN_OK(cmd->engine);

        g_socketLed = socketLed;
        if (g_socketLed == 0)
        {
            socketLed_startFlicker(cmd->engine, 0);
        }
        else
        {
            socketLed_startFlicker(cmd->engine, 1);
        }
        /*int i = 0;i
        for (i = 0; i < CHANNEL_NUM_MAX; i++)
        {
            if (g_uMynetContexts.nTcpipParas[i].uState != MYNET_CLOSED)
                break;
        }
        if (i >= CHANNEL_NUM_MAX)
        {
            if (g_socketLed == 1 && isSocketLedFlicker == 1)
            {
                socketLed_stopFlicker();
            }
            else if (g_socketLed == 0 && isSocketLedFlicker != 1)
            {
                socketLed_startFlicker(cmd->engine);
            }
        }
        else
        {

            if (g_socketLed == 0 && isSocketLedFlicker == 1)
            {
                socketLed_stopFlicker();
            }
            else if (g_socketLed == 1 && isSocketLedFlicker != 1)
            {
                socketLed_startFlicker(cmd->engine);
            }
        }*/
        RETURN_OK(cmd->engine);
    }
    else
    {
        sprintf(aucBuffer, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
    }
}

#if 0
/*****************************************************************************
* Name:        AT_TCPIP_CmdFunc_MYNETSCONT
* Description: Save TCP/IP parameter into flash/Display TCPIP settings.
* Parameter:  AT_CMD_PARA *cmd
* Return:       void
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-21
******************************************************************************/
void AT_TCPIP_CmdFunc_MYNETSCONT(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (AT_CMD_EXE == cmd->type)
    {
        at_CfgStoreTcpipSetting(&gMYNETSettings);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t *pRsp = NULL;
        pRsp = malloc(150);

        if (NULL == pRsp)
        {
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_BUFFER_FULL);
            return;
        }

        sprintf(pRsp, "+MYNETTKA: %d,%d,%d,%d", g_keepalive, g_keepidle, g_keepinterval, g_keepcount);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETSCONT: %d", mynetSCONT_unSaved);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETCSGP: %d\r\nGprs Config APN: %s\r\nGprs Config UserId: %s\r\nGprs Config Password: %s", mynetCSGP_setGprs, mynetApn, mynetUserName, mynetPassWord);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETHEAD: %d", mynetHEAD_addIpHead);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETSHOWTP: %d", mynetSHOWTP_dispTP);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETSRIP: %d", mynetSRIP_showIPPort);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETATS: %d,%d", mynetATS_setTimer, mynetATS_time);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETSPRT: %d", mynetSPRT_sendPrompt);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETQSEND: %d", mynetQSEND_quickSend);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETMODE: %d", mynetMODE_transParent);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETCCFG: %d,%d,%d,%d,%d,%d,%d", mynetCCFG_NmRetry, mynetCCFG_WaitTm,
                   mynetCCFG_SendSz, mynetCCFG_esc, mynetCCFG_Rxmode, mynetCCFG_RxSize, mynetCCFG_Rxtimer);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETMUX: %d", mynetMux_multiIp);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETDPDP:");
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETRXGET: %d", mynetRXGET_manualy);
        atCmdRespUrcText(cmd->engine, pRsp);
        sprintf(pRsp, "+MYNETRDTIMER: %d,%d", g_rdsigtimer, g_rdmuxtimer);
        atCmdRespUrcText(cmd->engine, pRsp);

        atCmdRespOK(cmd->engine);
        free(pRsp);
        return;
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        return;
    }
}
void AT_TCPIP_CmdFunc_MYNETCSGP(atCommand_t *cmd)
{

OSI_LOGI(0x1000528b, "AT+MYNETCSGP >> cmd->nDLCI= %d", cmd->nDLCI);

    if (AT_CMD_SET == cmd->type)
    {
        uint8_t index = 0;
        uint8_t mode;
        const char *apn = NULL;
        const *user = NULL;
        const *password = NULL;
        uint8_t param_count = cmd->param_count;
        bool paramRet = TRUE;
        if (param_count < 2)
        {
            OSI_LOGI(0x1000528c, "AT+MYNETCSGP invalid params count param_count:%d", param_count);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
        }
        // get mode
        mode = atParamUintInRange(cmd->params[index], 0, 1, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0x1000528d, "AT+MYNETCSGP fail to get mode param");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
        }
        // don't support CSD
        if (mode == 0)
        {
            OSI_LOGI(0x1000528e, "AT+MYNETCSGP don't support CSD");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        }
        index++;
        // get apn
        apn = (uint8_t *)atParamStr(cmd->params[index], &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0x1000528f, "AT+MYNETCSGP fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
        }

        if (param_count > 2)
        {
            index++;
            // get user name
            user = (uint8_t *)atParamStr(cmd->params[index], &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0x1000528f, "AT+MYNETCSGP fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
            }
        }
        if (param_count > 3)
        {
            index++;
            // get password
            password = (uint8_t *)atParamStr(cmd->params[index], &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0x1000528f, "AT+MYNETCSGP fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
            }
        }
        mynetCSGP_setGprs = mode;
        int8_t apnlen = apn != NULL ? AT_StrLen(apn) : -1;
        int8_t usrlen = user != NULL ? AT_StrLen(user) : -1;
        int8_t pwlen = password != NULL ? AT_StrLen(password) : -1;

        if (apnlen > 50 || usrlen > 50 || pwlen > 50)
        {
            OSI_LOGI(0, "MYNETCSGP apnlen=%d,srlen=%d,pwlen=%d", apnlen, usrlen, pwlen);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            if (apnlen == 0)
                memset(mynetApn, 0, 51);
            else if (apnlen > 0)
                memcpy(mynetApn, apn, apnlen);
            if (usrlen == 0)
                memset(mynetUserName, 0, 51);
            else if (usrlen > 0)
                memcpy(mynetUserName, user, usrlen);
            if (pwlen == 0)
                memset(mynetPassWord, 0, 51);
            else if (pwlen > 0)
                memcpy(mynetPassWord, password, pwlen);

            RETURN_OK(cmd->engine);
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+MYNETCSGP: 0-CSD,DIALNUMBER,USER NAME,PASSWORD,RATE(0-3) -- NOT SUPPORT");
        atCmdRespInfoText(cmd->engine, "+MYNETCSGP: 1-GPRS,APN,USER NAME,PASSWORD");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t *rspstr = NULL;
        if (mynetCSGP_setGprs == 1)
        {
            uint8_t tmp_buff[180] = {0};
            sprintf(tmp_buff, "+MYNETCSGP:1,\"%s\",\"%s\",\"%s\"",
                       mynetApn, mynetUserName, mynetPassWord);
            rspstr = tmp_buff;
        }
        else
        {
            rspstr = "+MYNETCSGP:<mode>,<apn>,<user name>,<password>"; //[<rate>]
        }
        atCmdRespInfoText(cmd->engine, rspstr);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "exe in %s(), command type not allowed", __func__);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_MYNETCCFG(atCommand_t *cmd)
{
    uint8_t param_count = cmd->param_count;
    uint8_t index = 0;
    uint8_t retry_number;
    uint8_t wait_time;
    uint16_t send_size;
    uint8_t escape = 1;
    uint8_t rxmode;
    uint16_t rxsize = 1460;
    uint16_t rxtimer = 50;
    uint8_t paramRet = TRUE;

    //OSI_LOGI(0, "AT+MYNETCCFG >> cmd->nDLCI= %d", cmd->nDLCI);

    if (mynetMux_multiIp)
    {
        OSI_LOGI(0, "AT+MYNETCCFG must be used in single connection mode");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }

    if (AT_CMD_SET == cmd->type)
    {
        if (param_count < 4 || param_count > 7)
        {
            OSI_LOGI(0, "AT+MYNETCCFG invalid param_count:%d", param_count);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
        }
        // get retry number
        retry_number = atParamUintInRange(cmd->params[index], 3, 8, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0, "AT+MYNETCCFG fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
        }

        index++;
        // get wait time
        wait_time = atParamUintInRange(cmd->params[index], 1, 10, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0, "AT+MYNETCCFG fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
        }

        index++;
        // get send size
        send_size = atParamUintInRange(cmd->params[index], 1, 1460, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0, "AT+MYNETCCFG fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
        }

        index++;
        // get escape character flag
        escape = atParamUintInRange(cmd->params[index], 0, 1, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0, "AT+MYNETCCFG fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
        }

        mynetCCFG_esc = escape;
        mynetCCFG_WaitTm = wait_time;
        mynetCCFG_NmRetry = retry_number;
        mynetCCFG_SendSz = send_size;

        if (param_count > 4)
        {
            index++;
            // get rxmode
            rxmode = atParamUintInRange(cmd->params[index], 0, 1, &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0, "AT+MYNETCCFG fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
            }
            mynetCCFG_Rxmode = rxmode;

            index++;
            // get rxsize
            rxsize = atParamUintInRange(cmd->params[index], 50, 1460, &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0, "AT+MYNETCCFG fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
            }
            mynetCCFG_RxSize = rxsize;

            index++;
            // get rxtimer
            rxtimer = atParamUintInRange(cmd->params[index], 20, 1000, &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0, "AT+MYNETCCFG fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
            }
            mynetCCFG_Rxtimer = rxtimer;
        }

        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+MYNETCCFG: (NmRetry:3-8),(WaitTm:1-10),(SendSz:1-1460),(esc:0,1),(Rxmode:0,1),(RxSize:50-1460),(Rxtimer:20-1000)");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t rspstr[64] = {0};
        sprintf(rspstr, "+MYNETCCFG:%d,%d,%d,%d,%d,%d,%d", mynetCCFG_NmRetry, mynetCCFG_WaitTm,
                   mynetCCFG_SendSz, mynetCCFG_esc, mynetCCFG_Rxmode, mynetCCFG_RxSize, mynetCCFG_Rxtimer);
        atCmdRespInfoText(cmd->engine, rspstr);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0, "exe in %s(), command type not allowed", __func__);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
#endif
/*****************************************************************************
* Name:        AT_TCPIP_Connect
* Description: TCP/IP module TCP/IP establish connection process.
* Parameter:   none
* Return:      void
* Remark:      May be call either during command process or in asynchronous process.
* Author:      YangYang
* Data:        2008-5-16
******************************************************************************/
static int32_t AT_TCPIP_Connect(atCmdEngine_t *engine, uint8_t nMuxIndex)
{
    int32_t iResult;
    int32_t iResultBind;
    uint8_t nSim = atCmdGetSim(engine);
    CFW_TCPIP_SOCKET_ADDR nDestAddr;
    stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[nMuxIndex]);
    stAT_Bearer_Paras *bearerParas = &(g_uMynetContexts.nBearerParas[gMynetBearer]);

    tcpipParas->uState = MYNET_PROCESSING;
    ip_addr_t nIpAddr;
    TCPIP_DNS_PARAM *dnsparam = malloc(sizeof(TCPIP_DNS_PARAM));
    if (dnsparam == NULL)
    {
        OSI_LOGI(0, "AT_TCPIP_Connect(), dnsparam malloc failed\n");
        return -1;
    }
    memset(dnsparam, 0, sizeof(TCPIP_DNS_PARAM));
    dnsparam->engine = engine;
    dnsparam->nMuxIndex = nMuxIndex;
    iResult = CFW_GethostbynameEX((char *)tcpipParas->uaIPAddress, &nIpAddr, bearerParas->nCid, nSim, tcpip_dnsReq_callback, (void *)dnsparam);
    if (iResult == RESOLV_QUERY_INVALID)
    {
        OSI_LOGI(0, "tcpip connect gethost by name error");
        tcpipParas->uState = MYNET_CLOSED;
        free(dnsparam);
        return -2; //ERR_AT_CME_DNS_FAILED
    }
    else if (iResult == RESOLV_COMPLETE)
    {
        OSI_LOGI(0, "AT_TCPIP_Connect(), CFW_GethostbynameEX() RESOLV_COMPLETE\n");
        free(dnsparam);
        tcpip_callback_param_t *tcpparam = malloc(sizeof(tcpip_callback_param_t));
        if (tcpparam == NULL)
        {
            OSI_LOGI(0, "AT_TCPIP_Connect(), malloc failed\n");
            return -1;
        }
        memset(tcpparam, 0, sizeof(tcpip_callback_param_t));
        tcpparam->engine = engine;
        tcpparam->nMuxIndex = nMuxIndex;
        iResult =
            CFW_TcpipSocketEX(tcpipParas->uDomain,
                              tcpipParas->uType,
                              tcpipParas->uProtocol,
                              tcpip_rsp, (uint32_t)tcpparam);
        if (SOCKET_ERROR == iResult)
        {
            OSI_LOGI(0, "AT_TCPIP_Connect(), CFW_TcpipSocket() failed\n");
            free(tcpparam);
            return iResult;
        }
        int n = 1;
        CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_REUSEADDR, (void *)&n, sizeof(n));
        struct linger _linger;
        _linger.l_onoff = 1;
        _linger.l_linger = 0;
        CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_LINGER, (void *)&_linger, sizeof(_linger));
        if (g_keepalive == 1)
        {
            CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_KEEPALIVE, (void *)&g_keepalive, sizeof(g_keepalive));
            CFW_TcpipSocketSetsockopt(iResult, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&g_keepidle, sizeof(g_keepidle));
            CFW_TcpipSocketSetsockopt(iResult, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&g_keepinterval, sizeof(g_keepinterval));
            CFW_TcpipSocketSetsockopt(iResult, IPPROTO_TCP, TCP_KEEPCNT, (void *)&g_keepcount, sizeof(g_keepcount));
        }
        else
        {
            CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_KEEPALIVE, (void *)&g_keepalive, sizeof(g_keepalive));
        }

        tcpipParas->uSocket = iResult;
        OSI_LOGI(0, "AT_TCPIP_Connect(), go to get netif\n");
        struct netif *netif = NULL;

        if (tcpipParas->uCid == 0x11)
            netif = getEtherNetIf(tcpipParas->uCid);
        else
            netif = getGprsNetIf(nSim, tcpipParas->uCid); //bearerParas->nCid
        if (netif == NULL)
        {
            OSI_LOGI(0, "AT_TCPIP_Connect(), get netif failed,tcpipParas->nCid=%d\n", tcpipParas->uCid);
            return -1;
        }
        OSI_LOGI(0, "mynet AT_TCPIP_Connect(), get netif done\n");
        ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(netif);

        CFW_TCPIP_SOCKET_ADDR stLocalAddr = {
            0,
        };
        OSI_LOGI(0, "AT_TCPIP_Connect(), gMynetlocalport[nMuxIndex].udp_port=%d,gMynetlocalport[nMuxIndex].tcp_port=%d\n", gMynetlocalport[nMuxIndex].udp_port, gMynetlocalport[nMuxIndex].tcp_port);
        stLocalAddr.sin_len = 0;
        stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
        if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_UDP)
            stLocalAddr.sin_port = htons(gMynetlocalport[nMuxIndex].udp_port);
        else
            stLocalAddr.sin_port = htons(gMynetlocalport[nMuxIndex].tcp_port);
        stLocalAddr.sin_addr.s_addr = ip_addr->addr; //PP_HTONL(INADDR_LOOPBACK);
        AT_StrNCpy(tcpipParas->uLocalip, ipaddr_ntoa(&(netif->ip_addr)), AT_MYNET_TCPIP_HOSTLEN);
        tcpipParas->uLocalip[AT_MYNET_TCPIP_HOSTLEN - 1] = '\0';
        //OSI_LOGI(0, "AT_TCPIP_Connect(),addr=%s, LOCALIP:%s\n", ipaddr_ntoa(&(netif->ip_addr)), tcpipParas->uLocalip);
        iResultBind = CFW_TcpipSocketBind(iResult, &stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
        if (SOCKET_ERROR == iResultBind)
        {
            if (ERR_SUCCESS != CFW_TcpipSocketClose(tcpipParas->uSocket))
            {
                OSI_LOGI(0, "AT_TCPIP_Connect(), connect failed,and close socket failed:%d\n", iResultBind);
            }
            else
            {
                tcpipParas->uState = MYNET_CLOSING;
                OSI_LOGI(0, "AT_TCPIP_Connect(), connect failed,and close socket success\n");
            }

            OSI_LOGI(0, "AT_TCPIP_Connect(), CFW_TcpipSocketBind() failed\n");
            return -3; //bind error
        }
        OSI_LOGI(0, "mynet CFW_TcpipSocketBind done,get bind ip and port\n");
        int addrlen = sizeof(struct sockaddr_storage);
        CFW_TcpipSocketGetsockname(tcpipParas->uSocket, &stLocalAddr, (int *)(&addrlen));
        OSI_LOGI(0, " port:%d %d", stLocalAddr.sin_port, ntohs(stLocalAddr.sin_port));
        if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_UDP)
            gMynetlocalport[nMuxIndex].udp_port = ntohs(stLocalAddr.sin_port);
        else
            gMynetlocalport[nMuxIndex].tcp_port = ntohs(stLocalAddr.sin_port);
        //gMynetlocalip[nMuxIndex].ip = *ip_addr;
        char *pcIpAddr = ipaddr_ntoa(&nIpAddr);
        AT_StrNCpy((char *)tcpipParas->uaIPAddress, pcIpAddr, AT_MYNET_TCPIP_HOSTLEN);
        tcpipParas->uaIPAddress[AT_MYNET_TCPIP_HOSTLEN - 1] = '\0';
        if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_TCP || tcpipParas->udpmode == 0)
        {
            nDestAddr.sin_family = CFW_TCPIP_AF_INET;
            nDestAddr.sin_port = htons(tcpipParas->uPort);
            iResult = CFW_TcpipInetAddr((const char *)tcpipParas->uaIPAddress);
            if (SOCKET_ERROR == iResult)
            {
                OSI_LOGI(0x10003fb1, "AT_TCPIP_Connect(), CFW_TcpipInetAddr() failed\n");

                if (ERR_SUCCESS != CFW_TcpipSocketClose(tcpipParas->uSocket))
                {
                    OSI_LOGI(0, "AT_TCPIP_Connect(), connect failed,and close socket failed\n");
                }
                else
                {
                    tcpipParas->uState = MYNET_CLOSING;
                    OSI_LOGI(0, "AT_TCPIP_Connect(), connect failed,and close socket success\n");
                }
                return iResult;
            }
            else
            {
                nDestAddr.sin_addr.s_addr = iResult;
            }
            iResult = CFW_TcpipSocketConnect(tcpipParas->uSocket, &nDestAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
            if (SOCKET_ERROR == iResult)
            {
                OSI_LOGI(0, "AT_TCPIP_Connect, CFW_TcpipSocketConnect() failed\n");

                if (ERR_SUCCESS != CFW_TcpipSocketClose(tcpipParas->uSocket))
                {
                    OSI_LOGI(0, "AT_TCPIP_Connect(), connect failed,and close socket failed\n");
                }
                else
                {
                    tcpipParas->uState = MYNET_CLOSING;
                    OSI_LOGI(0, "AT_TCPIP_Connect(), connect failed,and close socket success\n");
                }
                return iResult;
            }
            OSI_LOGI(0, "mynet CFW_TcpipSocketConnect done\n");
        }
        else
        {
            tcpip_connect_done(engine, nMuxIndex);
        }
    }
    else if (iResult == RESOLV_QUERY_QUEUED)
    {
        AT_SetAsyncTimerMux(engine, 60);
        return ERR_SUCCESS;
    }

    return ERR_SUCCESS;
}

static int32_t AT_TCPIP_ServerStart(atCmdEngine_t *engine, uint8_t uMuxIndex)
{
    int32_t iResult;
    OSI_LOGI(0, "AT_TCPIP_ServerStart() enter\n");
    stAT_Tcpip_Paras *tcpipParas = &(g_uMynetContexts.nTcpipParas[uMuxIndex]);
    tcpip_callback_param_t *tcpparam = malloc(sizeof(tcpip_callback_param_t));
    if (tcpparam == NULL)
    {
        OSI_LOGI(0, "AT_TCPIP_ServerStart(), malloc failed\n");
        return -1;
    }
    memset(tcpparam, 0, sizeof(tcpip_callback_param_t));
    tcpparam->engine = engine;
    tcpparam->nMuxIndex = uMuxIndex;
    iResult = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_STREAM, CFW_TCPIP_IPPROTO_TCP, tcpip_rsp, (uint32_t)tcpparam);
    if (SOCKET_ERROR == iResult)
    {
        OSI_LOGI(0, "CFW_TcpipSocketEX() failed\n");
        free(tcpparam);
        return iResult;
    }
    tcpipParas->uSocket = iResult;
    int n = 1;
    CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_REUSEADDR, (void *)&n, sizeof(n));
    //gMynetserver.nSocketId = iResult;
    if (g_keepalive == 1)
    {
        CFW_TcpipSocketSetsockopt(tcpipParas->uSocket, SOL_SOCKET, SO_KEEPALIVE, (void *)&g_keepalive, sizeof(g_keepalive));
        CFW_TcpipSocketSetsockopt(tcpipParas->uSocket, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&g_keepidle, sizeof(g_keepidle));
        CFW_TcpipSocketSetsockopt(tcpipParas->uSocket, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&g_keepinterval, sizeof(g_keepinterval));
        CFW_TcpipSocketSetsockopt(tcpipParas->uSocket, IPPROTO_TCP, TCP_KEEPCNT, (void *)&g_keepcount, sizeof(g_keepcount));
    }
    else
    {
        CFW_TcpipSocketSetsockopt(tcpipParas->uSocket, SOL_SOCKET, SO_KEEPALIVE, (void *)&g_keepalive, sizeof(g_keepalive));
    }
    struct netif *netif = NULL;

    uint8_t nSim = atCmdGetSim(engine);

    if (tcpipParas->uCid == 0x11)
        netif = getEtherNetIf(tcpipParas->uCid);
    else
        netif = getGprsNetIf(nSim, tcpipParas->uCid);
    if (netif == NULL)
    {
        return -1;
    }

    ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(netif);

    CFW_TCPIP_SOCKET_ADDR stLocalAddr = {
        0,
    };
    stLocalAddr.sin_len = 0;
    stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
    stLocalAddr.sin_port = htons(gMynetlocalport[uMuxIndex].tcp_port); //htons();
    stLocalAddr.sin_addr.s_addr = ip_addr->addr;                       //PP_HTONL(INADDR_LOOPBACK);
    //at_memcpy(&(tcpipParas->uLocalip), ip_addr, sizeof(ip4_addr_t));
    AT_StrNCpy(tcpipParas->uLocalip, ipaddr_ntoa(&(netif->ip_addr)), AT_MYNET_TCPIP_HOSTLEN);
    tcpipParas->uLocalip[AT_MYNET_TCPIP_HOSTLEN - 1] = '\0';
    if (SOCKET_ERROR == CFW_TcpipSocketBind(tcpipParas->uSocket, &stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR)))
    {
        OSI_LOGI(0, "CFW_TcpipSocketBind() failed\n");

        if (ERR_SUCCESS != CFW_TcpipSocketClose(tcpipParas->uSocket))
        {
            OSI_LOGI(0, "AT_TCPIP_ServerStart(), connect failed,and close socket failed\n");
        }
        else
        {
            tcpipParas->uState = MYNET_CLOSING;
            OSI_LOGI(0, "AT_TCPIP_ServerStart(), connect failed,and close socket success\n");
        }
        return -3; //bind error
    }
    OSI_LOGI(0, "CFW_TcpipSocketBind() success\n");
    if (CFW_TcpipSocketListen(tcpipParas->uSocket, 0) != ERR_SUCCESS)
    {
        OSI_LOGI(0, "CFW_TcpipSocketListen() failed\n");
        if (ERR_SUCCESS != CFW_TcpipSocketClose(tcpipParas->uSocket))
        {
            OSI_LOGI(0, "AT_TCPIP_ServerStart(), connect failed,and close socket failed\n");
        }
        else
        {
            tcpipParas->uState = MYNET_CLOSING;
            OSI_LOGI(0, "AT_TCPIP_ServerStart(), connect failed,and close socket success\n");
        }
        return -1;
    }
    return ERR_SUCCESS;
}

#endif
