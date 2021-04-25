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
#include "cfw.h"
#ifdef CFW_GPRS_SUPPORT
#include "stdio.h"
#include "osi_api.h"
#include "osi_log.h"
#include "at_response.h"
#include "at_cfw.h"
#include "at_engine.h"
#include "at_command.h"
#include "at_cmd_tcpip.h"
#include "sockets.h"
#include "netmain.h"
#include "ppp_interface.h"

#define AT_TCPIP_ADDRLEN 40 //extend from 20 to 40 for ipv6 128bit address
#define AT_TCPIP_HOSTLEN 255
#define AT_CMD_LINE_BUFF_LEN 1024 * 5
#define CHAR_CTRL_Z 0x1a
#define CHAR_ESC 0x1b
#define CHAR_BACKSPACE 8
#define ERR_SUCCESS 0
#define CIPSEND_MAXSIZE 1460

#define min(a, b) ((a) < (b) ? (a) : (b))

typedef enum
{
    CIP_INITIAL,
    CIP_START,
    CIP_CONFIG,
    CIP_GPRSACT,
    CIP_STATUS,
    CIP_PROCESSING,
    CIP_CONNECTED,
    CIP_CLOSING,
    CIP_CLOSED,
    CIP_PDPDEACT
} CIP_STATUS_T;

typedef enum
{
    CIP_SERVER_OPENING,
    CIP_SERVER_LISTENING,
    CIP_SERVER_CLOSING
} CIP_SERVER_STATE_T;

typedef enum
{
    CIPSERVER_CLOSE,
    CIPSERVER_OPEN,
    CIPSERVER_OPENING
} CIPSERVER_MODE;

typedef enum
{
    BEARER_GPRS_DEF,
    BEARER_GPRS_2,
    BEARER_WIFI,
    BEARER_MAX_NUM,
} CIP_BEARER_T;

typedef struct
{
    uint8_t *recvBuf;
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
    char uaIPAddress[AT_TCPIP_HOSTLEN];
    uint8_t udpmode; // 0: udp normal mode 1: udp extended mode 2: set udp address
    CIP_STATUS_T uState;
    CIP_BEARER_T uConnectBearer;
    uint32_t sendSize;
    uint32_t sentSize;
    osiTimer_t *transpSendTimer;
    CM_RECV_BUF_T uRecvBuf;
    union sockaddr_aligned from_addr;
    osiTimer_t *autoSendTimer;
    atCmdEngine_t *engine;
    uint8_t listening_socket;
} stAT_Tcpip_Paras;

typedef struct
{
    uint8_t nCid;
    CIP_STATUS_T nState;
} stAT_Bearer_Paras;

typedef struct
{
    uint8_t mode;
    uint8_t channelid;
    uint8_t nSocketId;
    CIP_BEARER_T nConnectBearer;
    uint32_t port;
    atCmdEngine_t *engine;
    uint8_t isIPV6;
} CIPSERVER;

typedef struct
{
    uint16_t tcp_port;
    uint16_t udp_port;
} CLOCAL_PORT;

typedef struct
{
    stAT_Tcpip_Paras nTcpipParas[MEMP_NUM_NETCONN];
    stAT_Bearer_Paras nBearerParas[BEARER_MAX_NUM]; //0,1:GPRS 2:WIFI
} CIP_CONTEXT_T;

#define ATTACH_TYPE (1 /*API_PS_ATTACH_GPRS*/ | 8 /*API_PS_ATTACH_FOR*/)

// typedef struct at_tcpip_setting
// {
//     uint8_t cipSCONT_unSaved;   //AT+CIPSCONT Save TCPIP Application Context
//     uint8_t cipMux_multiIp;     //AT+CIPMUX Start Up Multi-IP Connection
//     uint8_t cipHEAD_addIpHead;  //AT+CIPHEAD Add an IP Head at the Beginning of a Package Received
//     uint8_t cipSHOWTP_dispTP;   //AT+CIPSHOWTP Display Transfer Protocol in IP Head When Received Data
//     uint8_t cipSRIP_showIPPort; //AT+CIPSRIP Show Remote IP Address and Port When Received Data
//     uint8_t cipATS_setTimer;    //AT+CIPATS Set Auto Sending Timer
//     uint8_t cipATS_time;
//     uint8_t cipSPRT_sendPrompt;  //AT+CIPSPRT Set Prompt of > When Module Sends Data
//     uint8_t cipQSEND_quickSend;  //AT+CIPQSEND Select Data Transmitting Mode
//     uint8_t cipMODE_transParent; //AT+CIPMODE Select TCPIP Application Mode
//     uint8_t cipCCFG_NmRetry;     //AT+CIPCCFG Configure Transparent Transfer Mode
//     uint8_t cipCCFG_WaitTm;
//     uint16_t cipCCFG_SendSz;
//     uint8_t cipCCFG_esc;
//     uint8_t cipCCFG_Rxmode;
//     uint16_t cipCCFG_RxSize;
//     uint16_t cipCCFG_Rxtimer;
//     uint8_t cipDPDP_detectPDP; //AT+CIPDPDP Set Whether to Check State of GPRS Network Timing
//     uint8_t cipDPDP_interval;
//     uint8_t cipDPDP_timer;
//     uint8_t cipCSGP_setGprs;        //AT+CIPCSGP Set CSD or GPRS for Connection Mode
//     uint16_t cipRDTIMER_rdsigtimer; //AT+CIPRDTIMER Set Remote Delay Timer
//     uint16_t cipRDTIMER_rdmuxtimer;
//     uint8_t cipRXGET_manualy; //AT+CIPRXGET Get Data from Network Manually
//     uint8_t cipApn[51];
//     uint8_t cipUserName[51];
//     uint8_t cipPassWord[51];
//     uint8_t qishowla_showlocaddr;
//     uint8_t qidnsip_dnsip;
// } at_tcpip_setting_t;
extern bool AT_GetOperatorDefaultApn(uint8_t pOperatorId[6], const char **pOperatorDefaltApn);
extern void _dnsAddressToStr(const CFW_GPRS_PDPCONT_INFO_V2 *pdp_cont, char *str);
extern struct netif *getGprsNetIf(uint8_t nSim, uint8_t nCid);
extern struct netif *getEtherNetIf(uint8_t nCid);
extern uint32_t CFW_GprsRemovePdpCxt(uint8_t nCid, CFW_SIM_ID nSimID);
//extern uint32_t CFW_SetDnsServerbyPdp(uint8_t nCid, uint8_t nSimID, uint8_t *Pro_DnsIp, uint8_t *Sec_DnsIp);
//extern uint32_t *getDNSServer(uint8_t nCid, uint8_t nSimID);

static void _cipstartGprsAttRsp(atCommand_t *cmd, const osiEvent_t *event);
static void _cipstartGprsActRsp(atCommand_t *cmd, const osiEvent_t *event);

CIP_CONTEXT_T g_uCipContexts;
CIP_BEARER_T gCipBearer = BEARER_GPRS_DEF; //0,1:GPRS 2:WIFI
CIPSERVER gCipserver;
CLOCAL_PORT gClocalport[MEMP_NUM_NETCONN];

atCmdEngine_t *cipshutengine;

//keepalive Parameters
int g_keepalive = 0;
int g_keepidle = 7200;
int g_keepinterval = 75;
int g_keepcount = 9;

//RD Timer
int g_rdsigtimer = 2000;
int g_rdmuxtimer = 3500;

static at_tcpip_setting_t gCIPSettings = {
    .cipSCONT_unSaved = 1,
    .cipMux_multiIp = 0,
    .cipHEAD_addIpHead = 0,
    .cipSHOWTP_dispTP = 0,
    .cipSRIP_showIPPort = 0,
    .cipATS_setTimer = 0,
    .cipATS_time = 1,
    .cipSPRT_sendPrompt = 1,
    .cipQSEND_quickSend = 0,
    .cipMODE_transParent = 0,
    .cipCCFG_NmRetry = 5,
    .cipCCFG_WaitTm = 2,
    .cipCCFG_SendSz = 128,
    .cipCCFG_esc = 1,
    .cipCCFG_Rxmode = 0,
    .cipCCFG_RxSize = 1460,
    .cipCCFG_Rxtimer = 50,
    .cipDPDP_detectPDP = 0,
    .cipDPDP_interval = 0,
    .cipDPDP_timer = 3,
    .cipCSGP_setGprs = 1,
    .cipRDTIMER_rdsigtimer = 2000,
    .cipRDTIMER_rdmuxtimer = 3500,
    .cipRXGET_manualy = 0,
    .cipApn = {0},
    .cipUserName = {0},
    .cipPassWord = {0},
};

void cipSettings_Init()
{
    //at_CfgLoadTcpipSetting(&gCIPSettings);
}

#define cipSCONT_unSaved gCIPSettings.cipSCONT_unSaved
#define cipMux_multiIp gCIPSettings.cipMux_multiIp
#define cipQSEND_quickSend gCIPSettings.cipQSEND_quickSend
#define cipRXGET_manualy gCIPSettings.cipRXGET_manualy
#define cipMODE_transParent gCIPSettings.cipMODE_transParent // 0: normal mode 1: transparent mode
#define cipCCFG_esc gCIPSettings.cipCCFG_esc                 // wether handle escape character; 0: no 1: yes
#define cipCCFG_NmRetry gCIPSettings.cipCCFG_NmRetry         // 3 - 8
#define cipCCFG_WaitTm gCIPSettings.cipCCFG_WaitTm           // 1 - 10
#define cipCCFG_SendSz gCIPSettings.cipCCFG_SendSz           // 1 - 1460
#define cipCCFG_Rxmode gCIPSettings.cipCCFG_Rxmode           //0: output data to serial port without interval  1: within <Rxtimer> interval
#define cipCCFG_RxSize gCIPSettings.cipCCFG_RxSize           //50 - 1460
#define cipCCFG_Rxtimer gCIPSettings.cipCCFG_Rxtimer         //20 - 1000
#define cipATS_setTimer gCIPSettings.cipATS_setTimer         // 0: not set timer 1: set timer
#define cipATS_time gCIPSettings.cipATS_time                 // value of data send timer
#define cipSPRT_sendPrompt gCIPSettings.cipSPRT_sendPrompt   // prompt for IPSEND, 0: no ">" but "ok"  1: ">" + "ok"  2: no ">", no "ok"
#define cipHEAD_addIpHead gCIPSettings.cipHEAD_addIpHead     // CIPHEAD, 0: not add; 1: add
#define cipSRIP_showIPPort gCIPSettings.cipSRIP_showIPPort   // CIPSRIP 0:don't disp 1:disp
#define cipCSGP_setGprs gCIPSettings.cipCSGP_setGprs         // 1: GPRS(Default) 0: CSD
#define cipApn gCIPSettings.cipApn
#define cipUserName gCIPSettings.cipUserName
#define cipPassWord gCIPSettings.cipPassWord
#define cipDPDP_detectPDP gCIPSettings.cipDPDP_detectPDP // 0: don't detect pdp 1: detect pdp
#define cipDPDP_interval gCIPSettings.cipDPDP_interval   // 1 < interval <= 180(s)
#define cipDPDP_timer gCIPSettings.cipDPDP_timer         // 1 < timer <= 10
#define cipSHOWTP_dispTP gCIPSettings.cipSHOWTP_dispTP   // 0: disabled 1: enabled

#define RECV_BUF_UNIT_LEN 1024

// uint8_t g_uATTcpipValid;
const char g_strATTcpipStatus[10][32] = {"IP INITIAL", "IP START", "IP CONFIG", "IP GPRSACT",
                                         "IP STATUS", "IP PROCESSING", "CONNECT OK", "IP CLOSING",
                                         "CLOSED", "IP PDPDEACT"};

typedef struct
{
    uint8_t *buff;     // buffer, shared by all mode
    uint32_t buffLen;  // existed buffer data byte count
    uint32_t buffSize; // buffer size
    osiMutex_t *mutex;
} BYPASS_BUFFER_T;

BYPASS_BUFFER_T *g_bypass_buf = NULL;

BYPASS_BUFFER_T *at_TCPIPBufCreate()
{
    BYPASS_BUFFER_T *bypass_buff = (BYPASS_BUFFER_T *)malloc(sizeof(*bypass_buff));
    if (bypass_buff == NULL)
        return NULL;
    bypass_buff->buff = (uint8_t *)malloc(AT_CMD_LINE_BUFF_LEN);
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
void at_TCPIPBufDestroy(BYPASS_BUFFER_T *bypass_buff)
{
    if (bypass_buff == NULL)
        return;
    osiMutexDelete(bypass_buff->mutex);
    free(bypass_buff->buff);
    free(bypass_buff);
}
#if 0

void at_delayAsync(void *param)
{
    atCmdEngine_t *engine = (atCmdEngine_t *)param;
    if (g_bypass_buf == NULL)
    {
        OSI_LOGI(0x10003f27, "at_delayAsync error,param == null");
        at_StartCallbackTimer(cipATS_time * 1000, at_delayAsync, param);
        return;
    }
    OSI_LOGI(0x10003f28, "at_delayAsync,engine=%x,th->len=%d", engine, g_bypass_buf->buffLen);
    at_ModuleRunCommandExtra(engine, g_bypass_buf->buff, g_bypass_buf->buffLen);
}
#endif

static int sBypassDataSendAll = 0;
void bypassDataSend(void *param)
{
    atDispatch_t *dispatch = (atDispatch_t *)param;
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[0]);
    if (!atDispatchIsDataMode(dispatch))
    {
        OSI_LOGI(0, "bypassDataSend error,not data mode");
        goto restart;
    }
    if (g_bypass_buf == NULL)
    {
        OSI_LOGI(0x10003f29, "bypassDataSend error,g_bypass_buf == null");
        goto restart;
    }
    if (g_bypass_buf->buffLen < cipCCFG_SendSz && g_bypass_buf->buffLen != 0)
    {
        sBypassDataSendAll++;
    }
    if (g_bypass_buf->buffLen < cipCCFG_SendSz && sBypassDataSendAll < 2)
    {
        OSI_LOGI(0x10003f2a, "bypassDataSend do nothing buffLen=%d", g_bypass_buf->buffLen);
        goto restart;
    }
    else
    {
        uint16_t sendLen = cipCCFG_SendSz > g_bypass_buf->buffLen ? g_bypass_buf->buffLen : cipCCFG_SendSz;
        uint8_t *data = malloc(sendLen);
        if (data == NULL)
        {
            OSI_LOGI(0x10003f2b, "bypassDataSend MALLOC FAILS cipCCFG_SendSz=%d", sendLen);
            return;
        }
        memset(data, 0, sendLen);
        memcpy(data, g_bypass_buf->buff, sendLen);
        osiMutexLock(g_bypass_buf->mutex);
        memmove(g_bypass_buf->buff, g_bypass_buf->buff + sendLen, g_bypass_buf->buffLen - sendLen);
        g_bypass_buf->buffLen -= sendLen;
        osiMutexUnlock(g_bypass_buf->mutex);
        if (CFW_TcpipSocketSend(tcpipParas->uSocket, data, sendLen, 0) == SOCKET_ERROR)
        {
            OSI_LOGI(0x10003f2c, "TCPIP send socket data error");
        }
        free(data);
        sBypassDataSendAll = 0;
    }
restart:
    osiTimerStart(tcpipParas->transpSendTimer, cipCCFG_WaitTm * 100);
}

static int _transparentBypassDataRecv(void *param, const void *data, size_t length)
{
    OSI_LOGI(0x10003f2d, "bypassDataRecv,length=%d ", length);
    if (g_bypass_buf == NULL)
        g_bypass_buf = at_TCPIPBufCreate();
    if (g_bypass_buf == NULL)
        return 0;
    osiMutexLock(g_bypass_buf->mutex);
    uint8_t *data_u8 = (uint8_t *)data;
    size_t length_input = length;
    while (length > 0)
    {
        uint8_t c = *data_u8++;
        length--;

        // Remove previous byte for BACKSPACE

        if (c == CHAR_BACKSPACE)
        {
            if (g_bypass_buf->buffLen > 0)
                --g_bypass_buf->buffLen;
            continue;
        }
        // Drop whole buffer at overflow, maybe caused by data error
        // or buffer too small
        if (g_bypass_buf->buffLen >= g_bypass_buf->buffSize)
        {
            OSI_LOGI(0x10003f2e, "bypassDataRecv bypass mode overflow, drop all");
            g_bypass_buf->buffLen = 0;
        }
        g_bypass_buf->buff[g_bypass_buf->buffLen++] = c;
    }
    osiMutexUnlock(g_bypass_buf->mutex);

    return length_input;
}

static uint8_t *i8tostring(uint8_t value)
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
}

#define DEF_TCPIP 0
#define TCPIP_PARAMS(muxid) (&g_uCipContexts.nTcpipParas[muxid])

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
            atCmdRespInfoText(engine, "CONNECT OK");
        break;
    default:
        break;
    }
}

static void start_transparent_mode(atCmdEngine_t *engine)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[0]);
    atDispatchSetModeSwitchHandler(dispatch, transparentModeSwitchHandler);
    atEngineModeSwitch(AT_MODE_SWITCH_DATA_START, dispatch);
    if (tcpipParas->transpSendTimer == NULL)
        tcpipParas->transpSendTimer = osiTimerCreate(osiThreadCurrent(), bypassDataSend, (void *)dispatch);
    osiTimerStart(tcpipParas->transpSendTimer, cipCCFG_WaitTm * 100);
    atDataEngine_t *dataEngine = atDispatchGetDataEngine(dispatch);
    atDataSetBypassMode(dataEngine, _transparentBypassDataRecv, (void *)engine);
}

static void stop_transparent_mode(atCmdEngine_t *engine)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[0]);
    if (atDispatchIsDataMode(dispatch))
    {
        atDispatchSetModeSwitchHandler(dispatch, NULL);
        atEngineModeSwitch(AT_MODE_SWITCH_DATA_END, dispatch);
    }
    osiTimerStop(tcpipParas->transpSendTimer);
    osiTimerDelete(tcpipParas->transpSendTimer);
    tcpipParas->transpSendTimer = NULL;
    at_TCPIPBufDestroy(g_bypass_buf);
    g_bypass_buf = NULL;
}

/*****************************************************************************
* Name:         tcpip_rsp
* Description:  AT TCPIP module asynchronous event dispatch and process;
* Parameter:    osiEvent_t *pEvent
* Return:       void
* Remark:       n/a
* Author:       YangYang
* Data:         2008-5-15
******************************************************************************/
int AT_TCPIP_Connect(atCmdEngine_t *engine, uint8_t nMuxIndex);
int AT_TCPIP_ServerStart(atCmdEngine_t *engine);

typedef struct tcpip_dns_param
{
    atCmdEngine_t *engine;
    uint8_t nMuxIndex;
} TCPIP_DNS_PARAM;

typedef struct tcpip_callback_param
{
    atCmdEngine_t *engine;
    uint8_t nMuxIndex;
} tcpip_callback_param_t;

static void tcpip_dnsReq_callback(void *paramCos)
{
    osiEvent_t *ev = (osiEvent_t *)paramCos;
    TCPIP_DNS_PARAM *param = (TCPIP_DNS_PARAM *)ev->param3;
    char aucBuffer[40] = {0};
    uint8_t nMuxIndex = param->nMuxIndex;
    atCmdEngine_t *engine = param->engine;
    OSI_LOGI(0x10003f30, "AT_TCPIP_Connect(), CFW_GethostbynameEX() tcpip_dnsReq_callback\n");
    if (ev->id == EV_CFW_DNS_RESOLV_SUC_IND)
    {
        int iResult = 0;
        ip_addr_t *ipaddr = (ip_addr_t *)ev->param1;
        char *pcIpAddr = ipaddr_ntoa(ipaddr);
        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
        strncpy(tcpipParas->uaIPAddress, pcIpAddr, AT_TCPIP_ADDRLEN);
        iResult = AT_TCPIP_Connect(engine, nMuxIndex);
        if (iResult != ERR_SUCCESS)
        {
            if (!cipMux_multiIp)
                sprintf(aucBuffer, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
            else
                sprintf(aucBuffer, "%d,CONNECT FAIL", nMuxIndex);
            atCmdRespUrcText(engine, aucBuffer);
            atCmdRespOKText(engine, "");
            tcpipParas->uState = CIP_CLOSED;
            tcpipParas->engine = NULL;
        }
    }
    else if (ev->id == EV_CFW_DNS_RESOLV_ERR_IND)
    {
        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
        if (!cipMux_multiIp)
        {
            sprintf(aucBuffer, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
        }
        else
        {
            sprintf(aucBuffer, "%d,CONNECT FAIL", nMuxIndex);
        }
        atCmdRespUrcText(engine, aucBuffer);
        atCmdRespOKText(engine, "");
        tcpipParas->uState = CIP_CLOSED;
        tcpipParas->engine = NULL;
    }
    free(param);
    free(ev);
}
#if 1
typedef struct
{
    uint8_t act_state;
    uint8_t cid;
} cipAsyncCtx_t;

static void _cipstartGprsAttRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    atCmdEngine_t *engine = cmd->engine;
    OSI_LOGI(0x0, "enter _cipstartGprsAttRsp event id = %ld p1 = %ld p2 = %ld p3 = %ld",
             event->id, event->param1, event->param2, event->param3);

    if (event->id == EV_CFW_GPRS_ATT_RSP)
    {
        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);
        OSI_LOGI(0, "_cipstartGprsAttRsp bearerParas->nState:%d", bearerParas->nState);
        if ((CFW_GPRS_ATTACHED != event->param1) && (CFW_GPRS_DETACHED != event->param1))
        {
            OSI_LOGI(0, "_cipstartGprsAttRsp event->param1 ERROR");
            atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (CFW_GPRS_ATTACHED == event->param1)
        {
            uint8_t nSim = atCmdGetSim(cmd->engine);
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cipstartGprsActRsp, cmd);
            if (CFW_GprsAct(CFW_GPRS_ACTIVED, bearerParas->nCid, cmd->uti, nSim) != ERR_SUCCESS)
            {
                cfwReleaseUTI(cmd->uti);
                OSI_LOGI(0x10003f33, "attact_rsp Activate PDP error while TCP/IP connection");
                atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            }
            RETURN_FOR_ASYNC();
        }
        else
        {
            if (bearerParas->nState == CIP_INITIAL)
            {
                if (gCipserver.mode == CIPSERVER_OPENING)
                {
                    OSI_LOGI(0x10003f34, "attact_rsp Activate PDP error gCipserver");
                    gCipserver.mode = CIPSERVER_CLOSE;
                    atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                }
                else
                    atCmdRespErrorText(engine, "STATE:PDP DEACT\r\n\r\nCONNECT FAIL");
            }
            else if (bearerParas->nState == CIP_START)
            {
                OSI_LOGI(0x10003f35, "attact_rsp Activate PDP error CSTT");
                atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                OSI_LOGI(0, "attact_rsp bearerParas->nState=%d", bearerParas->nState);
                atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            }
        }
    }
}

static void _cipstartGprsActRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    uint8_t nSimId;
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    atCmdEngine_t *engine = cmd->engine;
    OSI_LOGI(0x0, "enter _cipstartGprsActRsp event id = %ld p1 = %ld p2 = %ld p3 = %ld",
             event->id, event->param1, event->param2, event->param3);
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);
    char aucBuffer[40] = {0};

    OSI_LOGI(0x0, "_cipstartGprsActRsp bearerParas->nCid =%d,cfw_event.nType=%d ", bearerParas->nCid, cfw_event->nType);

    if (EV_CFW_GPRS_ACT_RSP == event->id)
    {
        uint8_t nCid = event->param1;
        if (cfw_event->nType == CFW_GPRS_ACTIVED)
        {
            osiEvent_t tcpip_event = *event;
            tcpip_event.id = EV_TCPIP_CFW_GPRS_ACT;
            CFW_EVENT *tcpip_cfw_event = (CFW_EVENT *)&tcpip_event;
            tcpip_cfw_event->nUTI = 0;
            osiEventSend(netGetTaskID(), (const osiEvent_t *)&tcpip_event);
            osiThreadSleep(100);
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
            osiThreadSleep(100);
        }
        if (CFW_GPRS_ACTIVED == cfw_event->nType)
        {
            if (bearerParas->nState == CIP_INITIAL)
            {
                OSI_LOGI(0x0, "_cipstartGprsActRsp bearerParas->nState set to CIP_GPRSACT ");
                if (bearerParas->nCid != nCid)
                {
                    OSI_LOGI(0x0, "_cipstartGprsActRsp CFW_GPRS_ACTIVED bearerParas->nCid=%d, nCid=%d", bearerParas->nCid, nCid);
                    bearerParas->nCid = nCid;
                }
                bearerParas->nState = CIP_GPRSACT;
                if (CIPSERVER_OPENING == gCipserver.mode)
                {
                    bearerParas->nState = CIP_GPRSACT;
                    if (ERR_SUCCESS == AT_TCPIP_ServerStart(engine))
                    {
                        gCipserver.mode = CIPSERVER_OPEN;
                        atCmdRespOK(engine);
                        atCmdRespUrcText(engine, "SERVER OK");
                    }
                    else
                    {
                        gCipserver.mode = CIPSERVER_CLOSE;
                        atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                    }
                }
                else
                {
                    int iResult = AT_TCPIP_Connect(engine, 0);
                    if (iResult != ERR_SUCCESS)
                    {
                        OSI_LOGI(0x0, "_cipstartGprsActRsp TCPIP connect failed: %d", iResult);
                        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[0]);
                        sprintf(aucBuffer, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
                        atCmdRespErrorText(engine, aucBuffer);
                        tcpipParas->uState = CIP_CLOSED;
                        tcpipParas->engine = NULL;
                    }
                }
            }
            else if (bearerParas->nState == CIP_CONFIG)
            {
                bearerParas->nState = CIP_GPRSACT;
                atCmdRespOK(engine);
            }
        }
        else if (CFW_GPRS_DEACTIVED == cfw_event->nType)
        {
            /* if (cipMODE_transParent && atDispatchIsDataMode(at_DispatchGetByChannel(at_CmdGetChannel(engine))))
                {
                    stop_transparent_mode(engine);
                }*/

            bearerParas->nCid = 0;
            bearerParas->nState = CIP_INITIAL;
            for (int i = 0; i < MEMP_NUM_NETCONN; i++)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[i]);
                if (tcpipParas->uConnectBearer == gCipBearer)
                {
                    memset(tcpipParas, 0, sizeof(stAT_Tcpip_Paras));
                }
            }
            if (cipshutengine != NULL)
                cipshutengine = NULL;
            atCmdRespOKText(engine, "SHUT OK");
        }
        else
        {
            OSI_LOGI(0x0, "_cipstartGprsActRsp CFW_GPRS_DEACTIVED get event->id: %x", event->id);
            if (CIPSERVER_OPENING == gCipserver.mode)
            {
                gCipserver.mode = CIPSERVER_CLOSE;
            }
            if (cipshutengine != NULL)
                cipshutengine = NULL;
            atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        }
    }
}

static int set_userpwdapn(atCommand_t *cmd, uint8_t *nUser, uint8_t *nPwd, uint8_t *nApn)
{
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);
    uint8_t nSim = atCmdGetSim(cmd->engine);

    uint8_t nCid = bearerParas->nCid;
    OSI_LOGI(0, "set_userpwdapn(), the cid:%d", nCid);
    if (nApn == NULL && strlen((const char *)cipApn) < 1)
        return 0;
    else if (nApn == NULL)
        nApn = cipApn;
    if (nUser == NULL)
        nUser = cipUserName;
    if (nPwd == NULL)
        nPwd = cipPassWord;
    uint8_t nActState = 0;
    if (0 != CFW_GetGprsActState(nCid, &nActState, nSim))
    {
        OSI_LOGI(0, "set_userpwdapn(), CFW_GetGprsActState error!");
        return -1;
    }
    if (nActState == CFW_GPRS_ACTIVED)
    {
        OSI_LOGI(0, "set_userpwdapn(), the cid:%d,is actived already", nCid);
        return -1;
    }
    CFW_GPRS_PDPCONT_INFO_V2 *pPDPCnt = (CFW_GPRS_PDPCONT_INFO_V2 *)malloc(sizeof(CFW_GPRS_PDPCONT_INFO_V2));
    if (pPDPCnt == NULL)
    {
        OSI_LOGI(0x10003f3d, "set_userpwdapn(), no MORE MEMORY");
        return -1;
    }
    memset(pPDPCnt, 0, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
    int ret = CFW_GprsGetPdpCxtV2(nCid, pPDPCnt, nSim);
    if (ERR_SUCCESS != ret)
        memset(pPDPCnt, 0, sizeof(CFW_GPRS_PDPCONT_INFO_V2));

    OSI_LOGI(0x10003f3e, "pPDPCnt = %x(%d)", pPDPCnt, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
    //pPDPCnt->nPdpType = CFW_GPRS_PDP_TYPE_IP;
    pPDPCnt->nApnSize = strlen((const char *)nApn);
    memcpy(pPDPCnt->pApn, nApn, min(pPDPCnt->nApnSize, THE_APN_MAX_LEN));

    pPDPCnt->nApnUserSize = strlen((const char *)nUser);
    memcpy(pPDPCnt->pApnUser, nUser, min(pPDPCnt->nApnUserSize, THE_APN_USER_MAX_LEN));

    pPDPCnt->nApnPwdSize = strlen((const char *)nPwd);
    memcpy(pPDPCnt->pApnPwd, nPwd, min(pPDPCnt->nApnPwdSize, THE_APN_PWD_MAX_LEN));
    if (CFW_GprsSetPdpCxtV2(nCid, pPDPCnt, nSim) != ERR_SUCCESS)
    {
        OSI_LOGI(0x10003f3f, "set_userpwdapn(), CFW_GprsSetPdpCxt error");
        free(pPDPCnt);
        return -1;
    }
    free(pPDPCnt);
    return 0;
}

static int cip_getCid(atCommand_t *cmd)
{
    uint8_t cid = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    // begin execute...
    if (0 == bearerParas->nCid)
    {
        uint32_t ret = CFW_GetFreeCID(&cid, nSim);
        if (ret != ERR_SUCCESS)
        {
            OSI_LOGI(0x10003f40, "cip_getCid, no cid allowed for TCPIP\n");
            return -1;
        }
        bearerParas->nCid = cid;
    }
    return bearerParas->nCid;
}

static int do_attact(atCommand_t *cmd)
{
    int iResult = 0;
    uint8_t m_uAttState = CFW_GPRS_DETACHED;
    uint8_t uPDPState = 0;
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);
    atCmdEngine_t *engine = cmd->engine;
    uint8_t nCid = bearerParas->nCid;
    uint8_t nSim = atCmdGetSim(engine);
    if (0 == nCid)
    {
        OSI_LOGI(0x10003f41, "do_attact, no cid defined for TCPIP\n");
        return -1;
    }
    if (CFW_GetGprsAttState(&m_uAttState, nSim) != ERR_SUCCESS)
    {
        OSI_LOGI(0x10003f42, "do_attact, get GPRS attach state error\n");
        return -1;
    }
    if (CFW_GPRS_DETACHED == m_uAttState)
    {
        CFW_COMM_MODE nFM = CFW_DISABLE_COMM;
        if (0 != CFW_GetComm(&nFM, nSim))
        {
            return -1;
        }

        OSI_LOGI(0x0, "CFW_GetComm nFM %d", nFM);
        if (nFM != CFW_ENABLE_COMM)
        {
            return -1;
        }

        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cipstartGprsAttRsp, cmd);
        iResult = CFW_AttDetach(CFW_GPRS_ATTACHED, cmd->uti, ATTACH_TYPE, nSim);
        if (iResult != ERR_SUCCESS)
        {
            cfwReleaseUTI(cmd->uti);
            OSI_LOGI(0x10003f43, "do_attact, attach failed: %d\n", iResult);
            return -1;
        }
        return 0;
    }
    else if (CFW_GetGprsActState(nCid, &uPDPState, nSim) != ERR_SUCCESS)
    {
        OSI_LOGI(0x10003f44, "do_attact, get GPRS act state error\n");
        return -1;
    }
    if (uPDPState == CFW_GPRS_ACTIVED)
    {

        OSI_LOGI(0x10003f45, "do_attact,######################## maybe wrong state ##############################\n");
        return -1;
    }
    else
    {
        uint8_t oper_id[6];
        uint8_t mode;
        uint32_t ret;
        CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
        CFW_GPRS_PDPCONT_INFO_V2 sPdpContSet;
        memset(&sPdpContSet, 0x00, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
        memset(&sPdpCont, 0x00, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
        ret = CFW_GprsGetPdpCxtV2(nCid, &sPdpCont, nSim);
        if (ret != ERR_SUCCESS)
        {
            if (CFW_GprsSetPdpCxtV2(nCid, &sPdpContSet, nSim) != 0)
            {
                return -1;
            }
        }
        CFW_GprsGetPdpCxtV2(nCid, &sPdpCont, nSim);
        if (sPdpCont.nApnSize == 0)
        {
            if (CFW_NwGetCurrentOperator(oper_id, &mode, nSim) != 0)
            {
                return -1;
            }
            const char *defaultApnInfo = NULL;
            if (AT_GetOperatorDefaultApn(oper_id, &defaultApnInfo))
            {
                sPdpCont.nApnSize = strlen(defaultApnInfo);
                OSI_LOGXI(OSI_LOGPAR_S, 0x00000000, "sPdpCont.defaultApnInfo %s", defaultApnInfo);
                OSI_LOGI(0, "sPdpCont.nApnSize: %d", sPdpCont.nApnSize);
                if (sPdpCont.nApnSize >= THE_APN_MAX_LEN)
                {
                    return -1;
                }
                memcpy(sPdpCont.pApn, defaultApnInfo, sPdpCont.nApnSize);
            }
        }
        if (sPdpCont.nPdpType == 0)
        {
            sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IP; /* Default IPV4 */
        }
        sPdpCont.nDComp = 0;
        sPdpCont.nHComp = 0;
        sPdpCont.nNSLPI = 0;
        CFW_GPRS_QOS Qos = {3, 4, 3, 4, 16};
        CFW_GprsSetReqQos(nCid, &Qos, nSim);
        if (CFW_GprsSetPdpCxtV2(nCid, &sPdpCont, nSim) != 0)
        {
            return -1;
        }

        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cipstartGprsActRsp, cmd);
        iResult = CFW_GprsAct(CFW_GPRS_ACTIVED, nCid, cmd->uti, nSim);
        if (iResult != ERR_SUCCESS)
        {
            cfwReleaseUTI(cmd->uti);
            OSI_LOGI(0x10003f46, "do_attact, activate failed: %d\n", iResult);
            return -1;
        }
        return 0;
    }
}
#endif
void writeInfoNText(atCmdEngine_t *engine, const char *text, unsigned length)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    if (atDispatchIsDataMode(dispatch) && atDispatchGetDataEngine(dispatch) != NULL)
    {
        atDataWrite(atDispatchGetDataEngine(dispatch), text, length);
    }
    else
    {
        atCmdRespInfoNText(engine, text, length);
    }
}

static int recv_data(uint8_t uSocket, uint32_t uDataSize, atCmdEngine_t *engine, uint8_t nMuxIndex, uint8_t mode)
{
    int iResult = 0;
    uint16_t uIPHlen = 0;
    char *pData = NULL;
    char uaRspStr[128] = {
        0,
    };
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
    pData = malloc(uDataSize + 25);
    if (NULL == pData)
    {
        OSI_LOGI(0x10003f47, "EV_CFW_TCPIP_REV_DATA_IND, memory error");
        //atCmdRespCmeError(engine, ERR_AT_CME_NO_MEMORY);
        return -1;
    }
    memset(pData, 0, uDataSize + 25);
    if (mode == 0 && cipSRIP_showIPPort && (tcpipParas->uProtocol != CFW_TCPIP_IPPROTO_UDP))
    {
        if (!cipMux_multiIp)
        {
            sprintf(uaRspStr, "+RECV FROM:%s:%d", tcpipParas->uaIPAddress, tcpipParas->uPort);
        }
        else
        {
            sprintf(uaRspStr, "+RECEIVE,%d,%ld,%s:%d", nMuxIndex, uDataSize, tcpipParas->uaIPAddress, tcpipParas->uPort);
        }
        writeInfoNText(engine, uaRspStr, strlen(uaRspStr));
    }
    if (mode != 4)
    {
        if (cipHEAD_addIpHead)
        {
            if (!cipMux_multiIp)
            {
                if (cipSHOWTP_dispTP)
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
                uIPHlen = strlen(pData);
            }
        }
        union sockaddr_aligned from_addr = {
            0,
        };
        if (mode <= 1)
        {
            OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND,tcpipParas->uProtocol=%d ", tcpipParas->uProtocol);
            if (tcpipParas->uProtocol != CFW_TCPIP_IPPROTO_UDP)
            {
                iResult = CFW_TcpipSocketRecv(uSocket, (uint8_t *)(pData + uIPHlen), uDataSize, 0);
                if (SOCKET_ERROR == iResult)
                {
                    OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND, CFW_TcpipSocketRecv error");
                    //atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                    free(pData);
                    return -1;
                }
            }
            else
            {
                int fromLen = sizeof(from_addr); //union sockaddr_aligned addr;
                int udp_total_len = 0;
                int udp_want_len = uDataSize;
                while (udp_total_len < uDataSize)
                {
                    iResult = CFW_TcpipSocketRecvfrom(uSocket, (uint8_t *)(pData + uIPHlen + udp_total_len), udp_want_len, 0, (CFW_TCPIP_SOCKET_ADDR *)&from_addr, &fromLen);
                    OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND, CFW_TcpipSocketRecvfrom eiResult %d udp_want_len %d", iResult, udp_want_len);
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

                if (cipSRIP_showIPPort)
                {
                    ip_addr_t from_addr_t;
                    u16_t from_port;
                    SOCKADDR_TO_IPADDR_PORT(((const struct sockaddr *)&from_addr), &from_addr_t, from_port);
                    if (!cipMux_multiIp)
                    {
                        sprintf(uaRspStr, "+RECV FROM:%s:%hu", ipaddr_ntoa(&from_addr_t), from_port);
                    }
                    else
                    {
                        sprintf(uaRspStr, "+RECEIVE,%d,%ld,%s:%hu", nMuxIndex, uDataSize, ipaddr_ntoa(&from_addr_t), from_port);
                    }
                    writeInfoNText(engine, uaRspStr, strlen(uaRspStr));
                }
            }
        }
        if (mode == 1)
        {
            if (tcpipParas->uRecvBuf.recvBuf == NULL)
            {
                tcpipParas->uRecvBuf.recvBuf = malloc(RECV_BUF_UNIT_LEN);
                if (tcpipParas->uRecvBuf.recvBuf == NULL)
                {
                    OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND, CFW_TcpipSocketRecv malloc error");
                    return -1;
                }
                memset(tcpipParas->uRecvBuf.recvBuf, 0, RECV_BUF_UNIT_LEN);
                tcpipParas->uRecvBuf.readnum = 0;
                tcpipParas->uRecvBuf.recvBufLen = 0;
                tcpipParas->uRecvBuf.recvBufMaxSize = RECV_BUF_UNIT_LEN;
            }
            OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND, udp cache mode tcpipParas->uRecvBuf.recvBufMaxSize=%d", tcpipParas->uRecvBuf.recvBufMaxSize);
            if ((tcpipParas->uRecvBuf.readnum + tcpipParas->uRecvBuf.recvBufLen + iResult) <= tcpipParas->uRecvBuf.recvBufMaxSize)
            {

                memcpy(tcpipParas->uRecvBuf.recvBuf + tcpipParas->uRecvBuf.readnum + tcpipParas->uRecvBuf.recvBufLen, pData + uIPHlen, iResult);
                tcpipParas->uRecvBuf.recvBufLen += iResult;
                tcpipParas->from_addr = from_addr;
            }
            else
            {
                uint8_t *recvBufndi = malloc(tcpipParas->uRecvBuf.recvBufLen + iResult);
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
            //AT_Sprintf(uaRspStr, "+READ:%d,%d,%d", nMuxIndex,iResult,tcpipParas->uRecvBuf.recvBufLen);
            //at_CmdRespUrcText(engine, uaRspStr);
        }
        if (mode == 2 || mode == 3)
        {
            char uaIpStr[30] = {
                0,
            };
            int getLen = uDataSize < tcpipParas->uRecvBuf.recvBufLen ? uDataSize : tcpipParas->uRecvBuf.recvBufLen;
            int remainLen = tcpipParas->uRecvBuf.recvBufLen - getLen;
            if (!cipMux_multiIp)
            {
                sprintf(uaRspStr, "+CIPRXGET:%d,%d,%d", mode, getLen, remainLen);
            }
            else
            {
                sprintf(uaRspStr, "+CIPRXGET:%d,%d,%d,%d", mode, nMuxIndex, getLen, remainLen);
            }
            if (cipSRIP_showIPPort)
            {
                sprintf(uaIpStr, ",%s:%u", tcpipParas->uaIPAddress, tcpipParas->uPort);
                strcat(uaRspStr, uaIpStr);
            }
            writeInfoNText(engine, uaRspStr, strlen(uaRspStr));
        }
        if (mode == 0)
        {
            OSI_LOGI(0, "mode=%d", mode);
            writeInfoNText(engine, pData, uIPHlen + iResult);
        }
        if (mode == 2)
        {
            int readlen = 0;
            // writeInfoNText(engine, pData, uIPHlen + iResult);
            if (tcpipParas->uRecvBuf.recvBuf != NULL)
            {
                OSI_LOGI(0, "AT_CMIOT_TCPIP_CmdFunc_CMRD, len = %d,tcpipParas->uRecvBuf.recvBufLen=%d", uDataSize, tcpipParas->uRecvBuf.recvBufLen);
                if (uDataSize < tcpipParas->uRecvBuf.recvBufLen)
                {
                    readlen = uDataSize;
                }
                else
                {
                    readlen = tcpipParas->uRecvBuf.recvBufLen;
                }
                writeInfoNText(engine, pData, uIPHlen);
                writeInfoNText(engine, (const char *)(tcpipParas->uRecvBuf.recvBuf + tcpipParas->uRecvBuf.readnum), readlen);
                tcpipParas->uRecvBuf.readnum += readlen;
                tcpipParas->uRecvBuf.recvBufLen -= readlen;

                if (tcpipParas->uRecvBuf.recvBufLen == 0)
                {
                    OSI_LOGI(0, "AT_CMIOT_TCPIP_CmdFunc_CMRD, len = %d,tcpipParas->uRecvBuf.recvBufLen=%d", readlen, tcpipParas->uRecvBuf.recvBufLen);
                    free(tcpipParas->uRecvBuf.recvBuf);

                    tcpipParas->uRecvBuf.recvBuf = NULL;
                    tcpipParas->uRecvBuf.readnum = 0;
                    //tcpipParas->uRecvBuf.recvBufLen = 0;
                    tcpipParas->uRecvBuf.recvBufMaxSize = 0;
                }
                else
                {
                    OSI_LOGI(0, "AT_CMIOT_TCPIP_CmdFunc_CMRD, null");
                }
                //}
            }
            else
            {
                OSI_LOGI(0, "recv_data,there is no data in buff");
                //at_CmdRespOK(engine);
            }
        }
        else if (mode == 3)
        {
            int readlen = 0;
            if (tcpipParas->uRecvBuf.recvBuf != NULL)
            {
                OSI_LOGI(0, "AT_CMIOT_TCPIP_CmdFunc_CMRD, len = %d,tcpipParas->uRecvBuf.recvBufLen=%d", uDataSize, tcpipParas->uRecvBuf.recvBufLen);
                if (uDataSize < tcpipParas->uRecvBuf.recvBufLen)
                {
                    readlen = uDataSize;
                }
                else
                {
                    readlen = tcpipParas->uRecvBuf.recvBufLen;
                }
                //if (uDataSize < tcpipParas->uRecvBuf.recvBufLen){
                char *pHexData = malloc(2 * readlen + 25);
                if (pHexData == NULL)
                {
                    OSI_LOGI(0, "recv_data malloc fails for hex data");
                    //atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                    free(pData);
                    return -1;
                }
                memset(pHexData, 0, 2 * readlen + 25);
                memcpy(pHexData, pData, uIPHlen);
                for (int i = 0; i < readlen; i++)
                {
                    uint8_t tmp[3] = {0};
                    sprintf((char *)tmp, "%02x", (tcpipParas->uRecvBuf.recvBuf + tcpipParas->uRecvBuf.readnum)[i]);
                    strcat(pHexData, (char *)tmp);
                }
                writeInfoNText(engine, (const char *)pHexData, uIPHlen + 2 * readlen);
                free(pHexData);
                tcpipParas->uRecvBuf.readnum += readlen;
                tcpipParas->uRecvBuf.recvBufLen -= readlen;
                //at_CmdRespOK(engine);

                if (tcpipParas->uRecvBuf.recvBufLen == 0)
                {
                    OSI_LOGI(0, "AT_CMIOT_TCPIP_CmdFunc_CMRD, len = %d,tcpipParas->uRecvBuf.recvBufLen=%d", uDataSize, tcpipParas->uRecvBuf.recvBufLen);
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
                    OSI_LOGI(0, "AT_CMIOT_TCPIP_CmdFunc_CMRD,there have data left");
                }
            }
            else
            {
                OSI_LOGI(0, "recv_data,there is no data in buff");
                //at_CmdRespOK(engine);
            }
        }
    }
    else
    {
        //iResult = CFW_TcpipGetRecvAvailable(uSocket);
        if (!cipMux_multiIp)
        {
            sprintf(uaRspStr, "+CIPRXGET:%d,%ld", mode, tcpipParas->uRecvBuf.recvBufLen);
        }
        else
        {
            sprintf(uaRspStr, "+CIPRXGET:%d,%d,%ld", mode, nMuxIndex, tcpipParas->uRecvBuf.recvBufLen);
        }
        writeInfoNText(engine, uaRspStr, strlen(uaRspStr));
    }
    free(pData);
    return 0;
}

static void tcpip_connect_done(atCmdEngine_t *engine, uint8_t nMuxIndex)
{
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
    tcpipParas->uState = CIP_CONNECTED;

    if (cipMODE_transParent)
    {
        start_transparent_mode(engine);
    }
    else
    {
        char uaRspStr[30] = {
            0,
        };
        if (!cipMux_multiIp)
            sprintf(uaRspStr, "\r\nCONNECT OK");
        else
            sprintf(uaRspStr, "\r\n%d,CONNECT OK", nMuxIndex);
        atCmdRespInfoText(engine, uaRspStr);
        atCmdRespOKText(engine, "");
    }
}

#if 0
static void _cipCloseTimeout(atCommand_t *cmd)
{
    atCmdRespErrorCode(cmd->engine, ERR_AT_CME_EXE_FAIL);
    uint8_t uMuxIndex = 0;
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
    tcpipParas->uState = CIP_CLOSED;
    tcpipParas->engine = NULL;
}
#endif

static void tcpip_rsp(void *param)
{
    osiEvent_t *pEvent = (osiEvent_t *)param;
    char uaRspStr[60] = {
        0,
    };
    /*
    OSI_LOGXI(OSI_LOGPAR(S, I, I, I, I), 0x10003f4b, "tcpip_rsp Got %s: 0x%x,0x%x,0x%x,0x%x",
              TS_GetEventName(pEvent->id), pEvent->id, pEvent->param1, pEvent->param2, pEvent->param3);*/
    tcpip_callback_param_t *tcpparam = (tcpip_callback_param_t *)pEvent->param3;
    //uint8_t nDlc = pEvent->param3;
    uint8_t nMuxIndex = tcpparam->nMuxIndex;
    uint8_t uSocket = (uint8_t)pEvent->param1;
    atCmdEngine_t *engine = tcpparam->engine;

    stAT_Tcpip_Paras *tcpipParas = NULL;
    if (0xff != nMuxIndex)
        tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
    else
        tcpipParas = &(g_uCipContexts.nTcpipParas[0]);

    OSI_LOGI(0x10003f4c, "tcpip_rsp uSocket=%d,tcpipParas->uSocket=%d,state=%d,nMuxIndex=%d", uSocket, tcpipParas->uSocket, tcpipParas->uState, nMuxIndex);
    OSI_LOGI(0, "pEvent->id %d", pEvent->id);
    switch (pEvent->id)
    {
    case EV_CFW_TCPIP_SOCKET_CONNECT_RSP:
        if (tcpipParas->uSocket == uSocket)
        {
            tcpip_connect_done(engine, nMuxIndex);
        }
        break;
    case EV_CFW_TCPIP_ACCEPT_IND:
    {
        int haveIndex = 0;
        for (haveIndex = 0; haveIndex < MEMP_NUM_NETCONN; haveIndex++)
        {
            if (g_uCipContexts.nTcpipParas[haveIndex].uState == CIP_INITIAL || g_uCipContexts.nTcpipParas[haveIndex].uState == CIP_CLOSED)
            {
                break;
            }
        }
        if (haveIndex >= MEMP_NUM_NETCONN)
        {
            OSI_LOGI(0, "EV_CFW_TCPIP_ACCEPT_IND, NO used Index\n");
            break;
        }

        union sockaddr_aligned addr;
        uint32_t addr_len = sizeof(union sockaddr_aligned);
        tcpip_callback_param_t *tcpparam = malloc(sizeof(tcpip_callback_param_t));
        if (tcpparam == NULL)
        {
            OSI_LOGI(0, "EV_CFW_TCPIP_ACCEPT_IND, malloc tcpparam failed\n");
            break;
        }
        tcpparam->engine = engine;
        tcpparam->nMuxIndex = haveIndex;
        SOCKET newSocket = CFW_TcpipSocketAcceptEx(uSocket, (CFW_TCPIP_SOCKET_ADDR *)&addr, &addr_len, tcpip_rsp, (uint32_t)tcpparam);
        if (newSocket < 0)
        {
            free(tcpparam);
            atCmdRespUrcText(engine, "no invalid socket!");
            break;
        }

        g_uCipContexts.nTcpipParas[haveIndex].uState = CIP_CONNECTED;
        g_uCipContexts.nTcpipParas[haveIndex].uSocket = newSocket;
        g_uCipContexts.nTcpipParas[haveIndex].engine = engine;
        g_uCipContexts.nTcpipParas[haveIndex].listening_socket = uSocket;
        gCipserver.channelid = haveIndex;
        ip_addr_t from_addr_t;
        u16_t local_port;
        SOCKADDR_TO_IPADDR_PORT(((const struct sockaddr *)&addr), &from_addr_t, local_port);
        strncpy(g_uCipContexts.nTcpipParas[haveIndex].uaIPAddress, ipaddr_ntoa(&from_addr_t), AT_TCPIP_HOSTLEN);
        g_uCipContexts.nTcpipParas[haveIndex].uPort = local_port;
        OSI_LOGI(0, "accept local_port=%d", local_port);
        struct linger _linger;
        _linger.l_onoff = 1;
        _linger.l_linger = 0;
        CFW_TcpipSocketSetsockopt(newSocket, SOL_SOCKET, SO_LINGER, (void *)&_linger, sizeof(_linger));

        /*if (gCipserver.isIPV6 == 0)
        {
            ip4_addr_t ip4;
            inet_addr_to_ip4addr(&ip4, &(((struct sockaddr_in *)&addr)->sin_addr));
            sprintf(uaRspStr, "%d, REMOTE IP: %s", gCipserver.channelid, inet_ntoa(ip4));
        }
        else
        {
            ip6_addr_t ip6;
            inet6_addr_to_ip6addr(&ip6, &(((struct sockaddr_in6 *)&addr)->sin6_addr));
            sprintf(uaRspStr, "%d, REMOTE IP: %s", gCipserver.channelid, inet6_ntoa(ip6));
        }*/
        //sprintf(uaRspStr, "%d, REMOTE IP: %s", gCipserver.channelid, ip4addr_ntoa((ip4_addr_t *)&(addr.sin_addr)));

        sprintf(uaRspStr, "%d, REMOTE IP: %s", gCipserver.channelid, g_uCipContexts.nTcpipParas[haveIndex].uaIPAddress);
        atCmdRespUrcText(engine, uaRspStr);
    }
    break;
    case EV_CFW_TCPIP_SOCKET_CLOSE_RSP:
    {
        OSI_LOGI(0x10003f4d, "RECEIVED EV_CFW_TCPIP_SOCKET_CLOSE_RSP ...");
        if (0xff != nMuxIndex && tcpipParas->uRecvBuf.recvBuf != NULL)
        {
            free(tcpipParas->uRecvBuf.recvBuf);
            tcpipParas->uRecvBuf.recvBuf = NULL;
            tcpipParas->uRecvBuf.readnum = 0;
            tcpipParas->uRecvBuf.recvBufLen = 0;
            tcpipParas->uRecvBuf.recvBufMaxSize = 0;
        }
        if (0xff != nMuxIndex && tcpipParas->autoSendTimer != NULL)
        {
            osiTimerDelete(tcpipParas->autoSendTimer);
            tcpipParas->autoSendTimer = NULL;
            atDispatch_t *dispatch = atCmdGetDispatch(engine);
            if (atDispatchIsDataMode(dispatch) && atDispatchGetDataEngine(dispatch) != NULL)
            {
                atDataEngine_t *dataEngie = atDispatchGetDataEngine(dispatch);
                if (atDataIsBypassMode(dataEngie))
                {
                    atCmdSetLineMode(engine);
                    if (!cipMux_multiIp)
                        sprintf(uaRspStr, "SEND FAIL");
                    else
                        sprintf(uaRspStr, "%d,SEND FAIL", nMuxIndex);
                    free(tcpparam);
                    free(pEvent);
                    AT_CMD_RETURN(atCmdRespErrorText(engine, uaRspStr));
                }
            }
        }
        if (cipMODE_transParent)
        {
            stop_transparent_mode(engine);
        }
        if (0xff == nMuxIndex)
        {
            atCmdRespUrcText(engine, "SERVER CLOSED");
            gCipserver.nSocketId = 0;
            free(tcpparam);
            break;
        }
        else if (tcpipParas->uState == CIP_CLOSING || tcpipParas->uState == CIP_INITIAL)
        {
            if (!cipMux_multiIp)
                sprintf(uaRspStr, "CLOSED");
            else
                sprintf(uaRspStr, "%d,CLOSED", nMuxIndex);
            atCmdRespUrcText(engine, uaRspStr);
        }
        else if (tcpipParas->uState == CIP_CONNECTED || tcpipParas->uState == CIP_PROCESSING)
        {
            if (!cipMux_multiIp)
                sprintf(uaRspStr, "CLOSE OK");
            else
                sprintf(uaRspStr, "%d,CLOSE OK", nMuxIndex);
            atCmdRespOKText(engine, uaRspStr);
        }
        if (tcpipParas->uState != CIP_INITIAL)
            tcpipParas->uState = CIP_CLOSED;
        tcpipParas->listening_socket = 0;

        tcpipParas->engine = NULL;
        free(tcpparam);
    }
    break;
    case EV_CFW_TCPIP_ERR_IND:
    case EV_CFW_TCPIP_BEARER_LOSING_IND:
        if (pEvent->id == EV_CFW_TCPIP_BEARER_LOSING_IND)
        {
            stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);
            bearerParas->nState = CIP_INITIAL;
            OSI_LOGI(0, "tcpip_rsp:EV_CFW_TCPIP_BEARER_LOSING_IND bearerParas->nState: %d\n", bearerParas->nState);
        }
        if (tcpipParas->uSocket == uSocket)
        {
            if (tcpipParas->uState == CIP_PROCESSING)
            {
                if (pEvent->id == EV_CFW_TCPIP_BEARER_LOSING_IND)
                {
                    tcpipParas->uState = CIP_INITIAL;
                }
                if (!cipMux_multiIp)
                    sprintf(uaRspStr, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
                else
                    sprintf(uaRspStr, "%d,CONNECT FAIL", nMuxIndex);

                free(pEvent);
                AT_CMD_RETURN(atCmdRespErrorText(engine, uaRspStr));
            }
            if (CFW_TcpipSocketClose(uSocket) == ERR_SUCCESS)
            {
                tcpipParas->uState = CIP_CLOSING;
            }
        }
        break;

    case EV_CFW_TCPIP_CLOSE_IND:
        if (tcpipParas->uSocket == uSocket)
        {
            if (CFW_TcpipSocketClose(uSocket) == ERR_SUCCESS)
            {
                tcpipParas->uState = CIP_CLOSING;
            }
        }
        break;
    case EV_CFW_TCPIP_SOCKET_SEND_RSP:
        OSI_LOGI(0x10003f4e, "EV_CFW_TCPIP_SOCKET_SEND_RSP");
        if (!cipMODE_transParent && tcpipParas->uSocket == uSocket)
        {
            memset(uaRspStr, 0, 30);
            uint16_t uSendDataSize = 0;
            uSendDataSize = (uint16_t)pEvent->param2;
            tcpipParas->sentSize += uSendDataSize;
            OSI_LOGI(0x0, "uSendDataSize =%d tcpipParas->sentSize = %d tcpipParas->sendSize= %d",
                     uSendDataSize, tcpipParas->sentSize, tcpipParas->sendSize);
            if (tcpipParas->sentSize == tcpipParas->sendSize)
            {
                if (!cipQSEND_quickSend && cipSPRT_sendPrompt != 2)
                {
                    if (!cipMux_multiIp)
                        sprintf(uaRspStr, "SEND OK");
                    else
                        sprintf(uaRspStr, "%d,SEND OK", nMuxIndex);
                }
                atCmdRespUrcText(engine, uaRspStr);
            }
        }
        break;
    case EV_CFW_TCPIP_REV_DATA_IND:
        OSI_LOGI(0x10003f4f, "EV_CFW_TCPIP_REV_DATA_IND");
        if (tcpipParas->uSocket == uSocket)
        {
            uint16_t uDataSize = 0;
            uDataSize = (uint16_t)pEvent->param2; // maybe contain check bits
            if (cipRXGET_manualy == 1)
            {
                sprintf(uaRspStr, "+CIPRXGET:%d", nMuxIndex);
                atCmdRespUrcText(engine, uaRspStr);
                recv_data(uSocket, uDataSize, engine, nMuxIndex, 1);
            }
            else
            {
                recv_data(uSocket, uDataSize, engine, nMuxIndex, 0);
            }
        }
        break;
    default:
        OSI_LOGI(0x10003f50, "TCPIP unexpect asynchrous event/response %d", pEvent->id);
        //atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        break;
    }
    free(pEvent);
}

static void _timeoutabortTimeout(atCommand_t *cmd)
{
    OSI_LOGI(0, "_timeoutabortTimeout");
    if (AT_CFW_UTI_INVALID != cmd->uti)
        cfwReleaseUTI(cmd->uti);
    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, "COMMAND NOT RESPONSE"));
}

static void _timeoutabort_shut_Timeout(atCommand_t *cmd)
{
    OSI_LOGI(0, "_timeoutabort_shut_Timeout");
    if (AT_CFW_UTI_INVALID != cmd->uti)
        cfwReleaseUTI(cmd->uti);
    cipshutengine = NULL;
    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, "COMMAND NOT RESPONSE"));
}

void AT_TCPIP_CmdFunc_CIPMUX(atCommand_t *cmd)
{
    OSI_LOGI(0x10005290, "AT+CIPSTART ... ...");

    if (AT_CMD_SET == cmd->type)
    {

        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        bool paramok = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if ((mode == cipMux_multiIp) || (CIP_INITIAL == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState && CIP_INITIAL == g_uCipContexts.nBearerParas[BEARER_GPRS_2].nState && CIP_STATUS > g_uCipContexts.nBearerParas[BEARER_WIFI].nState))
        {
            if (mode != cipMux_multiIp)
            {
                for (int i = 0; i < MEMP_NUM_NETCONN; i++)
                {
                    memset(&(g_uCipContexts.nTcpipParas[i]), 0, sizeof(stAT_Tcpip_Paras));
                }
                if ((mode == 1) && (cipMODE_transParent == 1))
                {
                    OSI_LOGI(0x10003f52, "AT+CIPMUX ERROR,current cipMux_multiIp:%d, cipMODE_transParent:%d", cipMux_multiIp, cipMODE_transParent);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            cipMux_multiIp = mode;
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0x10003f53, "AT+CIPMUX ERROR,current cipMux_multiIp:%d, status:%d", cipMux_multiIp, g_uCipContexts.nBearerParas[gCipBearer].nState);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPMUX:%d", cipMux_multiIp);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPMUX:(0,1)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f54, "AT_TCPIP_CmdFunc_CIPMUX, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CIPSGTXT(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        bool paramok = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (mode == 2)
        {
            struct netif *netif = getEtherNetIf(0x11);
            if (netif == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            if (g_uCipContexts.nBearerParas[mode].nState < CIP_GPRSACT)
                g_uCipContexts.nBearerParas[mode].nState = CIP_GPRSACT;
            g_uCipContexts.nBearerParas[mode].nCid = 0x11;
        }
        else if (!cipMux_multiIp)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        gCipBearer = mode;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[60] = {0};
        char *status1 = "DEACTIVE";
        char *status2 = "DEACTIVE";
        char *status3 = "DEACTIVE";
        if (g_uCipContexts.nBearerParas[0].nState > CIP_CONFIG && g_uCipContexts.nBearerParas[0].nState < CIP_PDPDEACT)
            status1 = "ACTIVE";
        if (g_uCipContexts.nBearerParas[1].nState > CIP_CONFIG && g_uCipContexts.nBearerParas[1].nState < CIP_PDPDEACT)
            status2 = "ACTIVE";
        if (getEtherNetIf(0x11) != NULL)
            status3 = "ACTIVE";

        sprintf(aucBuffer, "+CIPSGTXT:%d,(0,%s),(1,%s),(2,%s)", gCipBearer, status1, status2, status3);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPSGTXT:(0,1,2)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f55, "AT_TCPIP_CmdFunc_CIPSGTXT, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CIPSTART
* Description: Start up TCPIP connection
* Parameter:  atCommand_t *cmd
* Return:       void
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-15
******************************************************************************/
#if 1

static uint8_t gOutsidePdpCid = 0;

static bool useOutsidePdp(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t m_uAttState = CFW_GPRS_DETACHED;
    uint8_t uPDPState = 0;
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    if (CFW_GetGprsAttState(&m_uAttState, nSim) != ERR_SUCCESS || CFW_GPRS_DETACHED == m_uAttState)
    {
        OSI_LOGI(0x10003f56, "useExistPdp, not attached\n");
        goto error;
    }
    int i = 1;
    for (i = 1; i < 8; i++)
    {
        if (CFW_GetGprsActState(i, &uPDPState, nSim) != ERR_SUCCESS || uPDPState == CFW_GPRS_DEACTIVED)
        {
            OSI_LOGI(0x10003f57, "useExistPdp, not actived\n");
            continue;
        }
        struct netif *netif = getGprsNetIf(nSim, i);
#if IP_NAT
        if (netif != NULL && (netif->link_mode == NETIF_LINK_MODE_LWIP || netif->link_mode == NETIF_LINK_MODE_NAT_LWIP_LAN))
#else
        if (netif != NULL && netif->link_mode == NETIF_LINK_MODE_LWIP)
#endif
            break;
    }
    if (i >= 8)
        goto error;
    if (bearerParas->nCid != 0)
        CFW_ReleaseCID(bearerParas->nCid, nSim);

    bearerParas->nCid = i;
    gOutsidePdpCid = i;
    return true;
error:
    gOutsidePdpCid = 0;
    return false;
}
#endif
void AT_TCPIP_CmdFunc_CIPSTART(atCommand_t *cmd)
{
    //uint8_t nSimStatus = 0;

    OSI_LOGI(0x10005290, "AT+CIPSTART ... ...");
    atCmdEngine_t *cmdEngine = cmd->engine;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (AT_CMD_SET == cmd->type)
    {
        if (CFW_GetSimStatus(nSim) == CFW_SIM_ABSENT)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

        if ((!cipMux_multiIp && (CIP_INITIAL == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState || CIP_STATUS == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState || CIP_GPRSACT == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState)) || (cipMux_multiIp && (CIP_STATUS == bearerParas->nState || CIP_GPRSACT == bearerParas->nState)))
        {
#if 0
            if (gAtGprsCidNum > AT_PDPCID_MAX)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#endif

            if ((!cipMux_multiIp && cmd->param_count != 3) || (cipMux_multiIp && cmd->param_count != 4))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            bool paramok = true;
            uint8_t uMuxIndex = 0;
            uint8_t nType, nProtocol;
            uint16_t uPort = 0;
            if (cipMux_multiIp)
            {
                uMuxIndex = atParamUintInRange(cmd->params[0], 0, 7, &paramok);
            }
            char *mode = (char *)atParamStr(cmd->params[0 + cipMux_multiIp], &paramok);
            char *ipaddress = (char *)atParamStr(cmd->params[1 + cipMux_multiIp], &paramok);
            uPort = atParamUintInRange(cmd->params[2 + cipMux_multiIp], 0, 65535, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            if ((strcmp(mode, "TCP") == 0) || strcmp(mode, "tcp") == 0)
            {
                nType = CFW_TCPIP_SOCK_STREAM;
                nProtocol = CFW_TCPIP_IPPROTO_TCP;
            }
            else if ((strcmp(mode, "UDP") == 0) || strcmp(mode, "udp") == 0)
            {
                nType = CFW_TCPIP_SOCK_DGRAM;
                nProtocol = CFW_TCPIP_IPPROTO_UDP;
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            //atCmdRespOK(cmd->engine); //Result Ok when param is right. This will delete cmd
            atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
            if (chsetting->atv)
            {
                atCmdRespInfoText(cmd->engine, "OK");
            }
            else
            {
                atCmdRespInfoText(cmd->engine, "0"); //CMD_RC_OK
            }

            stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);

            if (tcpipParas->engine != NULL && tcpipParas->engine != cmd->engine)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            tcpipParas->engine = cmd->engine;
            char aucBuffer[40] = {0};
            if (tcpipParas->uState > CIP_STATUS &&
                tcpipParas->uState < CIP_CLOSED)
            {
                if (tcpipParas->uState == CIP_CONNECTED)
                {
                    if (cipMux_multiIp)
                        sprintf(aucBuffer, "%d,ALREADY CONNECT", uMuxIndex);
                    else
                        sprintf(aucBuffer, "ALREADY CONNECT");
                }
                else
                {
                    if (cipMux_multiIp)
                        sprintf(aucBuffer, "%d,ALREADY OPENED", uMuxIndex);
                    else
                        sprintf(aucBuffer, "ALREADY OPENED");
                }
                AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
            }

            if ((strcmp(mode, "UDP") == 0) || strcmp(mode, "udp") == 0)
            {
                update_udp_remote_socket(uMuxIndex, (uint8_t *)ipaddress, strlen(ipaddress), uPort);
            }
            tcpipParas->listening_socket = 0;
            tcpipParas->uType = nType;
            tcpipParas->uProtocol = nProtocol;
            tcpipParas->uDomain = CFW_TCPIP_AF_INET;
            memset(tcpipParas->uaIPAddress, 0, AT_TCPIP_HOSTLEN);
            strncpy(tcpipParas->uaIPAddress, ipaddress, AT_TCPIP_HOSTLEN); //20181214 fixed by SUN wei for bug 983610
            tcpipParas->uPort = uPort;
            tcpipParas->uConnectBearer = gCipBearer;
            tcpipParas->uState = bearerParas->nState;

            int iResult = 0;
            OSI_LOGI(0, "AT_TCPIP_CmdFunc_CIPSTART() bearerParas->nState: %d\n", bearerParas->nState);
            if (bearerParas->nState == CIP_GPRSACT || bearerParas->nState == CIP_STATUS)
            {
                uint8_t uPDPState = 0;
                if (CFW_GetGprsActState(bearerParas->nCid, &uPDPState, nSim) != ERR_SUCCESS || uPDPState == CFW_GPRS_DEACTIVED)
                {
                    OSI_LOGI(0x0, "useExibearerParas->nState error, not actived\n");
                    bearerParas->nState = CIP_INITIAL;
                }
            }
            if ((CIP_INITIAL == bearerParas->nState || gOutsidePdpCid != 0) && !useOutsidePdp(cmd))
            {
                bearerParas->nState = CIP_INITIAL;
                if ((uint8_t)atGetPocMasterCard() == nSim || atGetPocMasterCard() == CFW_SIM_END)
                {
                    if (cip_getCid(cmd) < 0 || set_userpwdapn(cmd, NULL, NULL, NULL) < 0 || do_attact(cmd) < 0)
                        iResult = -1;
                }
                else
                {
                    OSI_LOGI(0x0, "The PocMasterCard is not equal to nSim.");
                    iResult = -1;
                }
                atCmdSetTimeoutHandler(cmd->engine, 60000, _timeoutabortTimeout);
            }
            else
            {
                if (CIP_INITIAL == bearerParas->nState)
                {
                    bearerParas->nState = CIP_GPRSACT;
                }
                iResult = AT_TCPIP_Connect(cmdEngine, uMuxIndex);
                if (iResult != ERR_SUCCESS)
                {
                    OSI_LOGI(0x10003f58, "AT_TCPIP_CmdFunc_CIPSTART(), connect failed: %d\n", iResult);
                }
            }
            if (iResult != ERR_SUCCESS)
            {
                if (!cipMux_multiIp)
                    sprintf(aucBuffer, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
                else
                    sprintf(aucBuffer, "%d,CONNECT FAIL", uMuxIndex);
                atCmdRespErrorText(cmd->engine, aucBuffer);
                tcpipParas->uState = CIP_CLOSED;
                tcpipParas->engine = NULL;
            }
        }
        else
        {
            OSI_LOGI(0x10003f59, "AT_TCPIP_CmdFunc_CIPSTART(), state error: %d\n", bearerParas->nState);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char *pRstStr = malloc(85);
        if (NULL == pRstStr)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        memset(pRstStr, 0, 85);
        if (cipMux_multiIp)
            strcpy(pRstStr, "+CIPSTART: (0-7), (\"TCP\", \"UDP\"), (\"(0-255).(0-255).(0-255).(0-255)\"), (0-65535)");
        else
            strcpy(pRstStr, "+CIPSTART: (\"TCP\", \"UDP\"), (\"(0-255).(0-255).(0-255).(0-255)\"), (0-65535)");

        atCmdRespInfoText(cmd->engine, pRstStr);
        atCmdRespOK(cmd->engine);
        free(pRstStr);
    }
    else
    {
        OSI_LOGI(0x10003f5a, "AT_TCPIP_CmdFunc_CIPSTART(), command type not allowed");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************

* Name:        void AT_TCPIP_CmdFunc_CIFSR

* Description: get local IP address. Only at the status of activated scene:
IP GPRSACT, TCP/UDP connecting,
                    CONNECT OK, IP CLOSE can get local IP address by AT+CIFSR
. otherwise response ERROR.

* Parameter:  atCommand_t *cmd

* Return:       void

* Remark:      n/a

* Author:       Chang Yuguang

* Data:          2008-5-19

******************************************************************************/
void AT_TCPIP_CmdFunc_CIFSR(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    OSI_LOGI(0x10003f5b, "CIFSR processing\n");

    if (cmd->type == AT_CMD_EXE)
    {
        if (0 != cmd->param_count)
        {
            OSI_LOGI(0x10003f5c, "AT+CIFSR: too parameter presented");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

        if (CIP_GPRSACT > bearerParas->nState ||
            CIP_CLOSED < bearerParas->nState)
        {
            OSI_LOGI(0x10003f5d, "AT+CIFSR: execute the command at wrong state:%d", bearerParas->nState);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            struct netif *netif;
            if (bearerParas->nCid == 0x11)
                netif = getEtherNetIf(bearerParas->nCid);
            else
                netif = getGprsNetIf(nSim, bearerParas->nCid);

            if (netif == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            if (netif->pdnType == CFW_GPRS_PDP_TYPE_IP)
            {
                ip4_addr_t *ip_addr4 = (ip4_addr_t *)netif_ip4_addr(netif);
                bearerParas->nState = CIP_STATUS;
                char pTempStr[100] = {0};
                sprintf(pTempStr, "IPV4:%s", inet_ntoa(*ip_addr4));
                atCmdRespInfoText(cmd->engine, pTempStr);
            }
#if LWIP_IPV6
            else if (netif->pdnType == CFW_GPRS_PDP_TYPE_IPV6)
            {
                ip6_addr_t *ip_addr = (ip6_addr_t *)netif_ip6_addr(netif, 0);
                ip6_addr_t *ip_addr_1 = (ip6_addr_t *)netif_ip6_addr(netif, 1);
                if (ip6_addr_isany_val(*(ip_addr_1)) != true)
                {
                    ip_addr = ip_addr_1;
                }
                bearerParas->nState = CIP_STATUS;
                char pTempStr[100] = {0};
                sprintf(pTempStr, "IPV6:%s", inet6_ntoa(*ip_addr));
                atCmdRespInfoText(cmd->engine, pTempStr);
            }
            else if (netif->pdnType == CFW_GPRS_PDP_TYPE_IPV4V6)
            {
                char pTempStr[100] = {0};
                char pResp[128] = {0};
                ip4_addr_t *ip_addr4 = (ip4_addr_t *)netif_ip4_addr(netif);
                bearerParas->nState = CIP_STATUS;
                memset(pTempStr, 0, sizeof(pTempStr));
                sprintf(pTempStr, "IPV4:%s", inet_ntoa(*ip_addr4));
                strcat(pResp, pTempStr);
                ip6_addr_t *ip_addr = (ip6_addr_t *)netif_ip6_addr(netif, 0);
                ip6_addr_t *ip_addr_1 = (ip6_addr_t *)netif_ip6_addr(netif, 1);
                if (ip6_addr_isany_val(*(ip_addr_1)) != true)
                {
                    ip_addr = ip_addr_1;
                }
                if (ip_addr != NULL)
                {
                    memset(pTempStr, 0, sizeof(pTempStr));
                    sprintf(pTempStr, " IPV6:%s", inet6_ntoa(*ip_addr));
                    strcat(pResp, pTempStr);
                }
                atCmdRespInfoText(cmd->engine, pResp);
            }
#endif
            atCmdRespOK(cmd->engine);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f5e, "AT+CIFSR: command not supported");
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************

* Name:        void AT_TCPIP_CmdFunc_CIPCLOSE

* Description: Only close the connection at the status of TCP/UDP CONNECTING or CONNECT OK. otherwise
                    response error. after close the connection, the status is IP CLOSE.

* Parameter:  atCommand_t *cmd

* Return:       void

* Remark:      n/a

* Author:       Chang Yuguang

* Data:          2008-5-19

******************************************************************************/
void AT_TCPIP_CmdFunc_CIPCLOSE(atCommand_t *cmd)
{
    OSI_LOGI(0x10003f5f, "CIPSTATUS processing\n");

    if (cmd->type == AT_CMD_EXE || cmd->type == AT_CMD_SET)
    {
        if (AT_CMD_EXE == cmd->type && cipMux_multiIp)
        {
            OSI_LOGI(0x10003f60, "AT+CIPCLOSE: AT_CMD_EXE Only work at nomal mode");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (AT_CMD_SET == cmd->type && (((cmd->param_count > 1) && (!cipMux_multiIp)) || ((cmd->param_count > 2) && (cipMux_multiIp == 1))))
        {
            OSI_LOGI(0x10003f61, "AT+CIPCLOSE: pPara is error cipMux_multiIp=%d,uParaCount=%d", cipMux_multiIp, cmd->param_count);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        uint8_t uMuxIndex = 0;
        bool paramok = true;
        uint8_t isQuickClose = 0;

        if (cmd->param_count > 0)
        {
            if (cipMux_multiIp)
            {
                uMuxIndex = atParamUintInRange(cmd->params[0], 0, 7, &paramok);
                if (cmd->param_count > 1)
                {
                    isQuickClose = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
                }
            }
            else
            {
                isQuickClose = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
            }
        }
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
        // only on connecting state , the command can be executed.
        if (CIP_PROCESSING != tcpipParas->uState && CIP_CONNECTED != tcpipParas->uState)
        {
            OSI_LOGI(0x10003f62, "AT+CIPCLOSE: can't run at this state:%d", tcpipParas->uState);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (tcpipParas->engine != NULL && atCmdEngineIsValid(tcpipParas->engine) && tcpipParas->engine != cmd->engine)
        {
            OSI_LOGI(0x0, "tcpipParas->engine  %x, cmd->engine %x", tcpipParas->engine, cmd->engine);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        int iRetValue;
        iRetValue = CFW_TcpipSocketClose(tcpipParas->uSocket);
        if (iRetValue == ERR_SUCCESS)
        {
            OSI_LOGI(0x10003f63, "AT+IPCLOSE: close success");
            if (isQuickClose || tcpipParas->engine != cmd->engine)
            {
                char uaRspStr[60] = {
                    0,
                };
                if (!cipMux_multiIp)
                    sprintf(uaRspStr, "CLOSE OK");
                else
                    sprintf(uaRspStr, "%d,CLOSE OK", uMuxIndex);
                tcpipParas->uState = CIP_CLOSED;
                tcpipParas->engine = NULL;
                atCmdRespOKText(cmd->engine, uaRspStr);
                return;
            }
            atCmdSetTimeoutHandler(cmd->engine, 30000, _timeoutabortTimeout);
            return;
        }
        else
        {
            OSI_LOGI(0x10003f64, "AT+CIPCLOSE: CSW execute wrong");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f65, "AT+CIPCLOSE: command not supported");
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************

* Name:        void AT_TCPIP_CmdFunc_CIPSHUT

* Description: Disconnect the wireless connection. Except at the status of IP INITIAL, you can close moving
                   scene by AT+CIPSHUT. After closed, the status is IP INITIAL.

* Parameter:  atCommand_t *cmd

* Return:       void

* Remark:      n/a

* Author:       Chang Yuguang

* Data:          2008-5-19

******************************************************************************/

void AT_TCPIP_CmdFunc_CIPSHUT(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    OSI_LOGI(0x10003f66, "CIPSHUT processing\n");

    if (cmd->type == AT_CMD_EXE)
    {
        if (0 != cmd->param_count)
        {
            OSI_LOGI(0x10003f67, "AT+CIPSHUT: too many parameter");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);
        OSI_LOGI(0x10003f68, "AT+CIPSHUT: bearerParas.status %d\n", bearerParas->nState);

        // only on connecting state , the command can be executed.
        if (CIP_INITIAL == bearerParas->nState)
        {
            OSI_LOGI(0x10003f69, "AT+CIPSHUT: can't ran in CIP_INITIAL");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (cipshutengine == NULL)
        {
            cipshutengine = cmd->engine;
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        int nOperationRet = 0;

        if (CIP_INITIAL < bearerParas->nState &&
            CIP_PDPDEACT > bearerParas->nState)
        {
            for (int i = 0; i < MEMP_NUM_NETCONN; i++)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[i]);
                if ((tcpipParas->engine != cmd->engine) && (tcpipParas->uState == CIP_PROCESSING))
                {
                    OSI_LOGI(0x0, "AT+CIPSHUT: tcpipParas->engine != cmd->engine and tcpipParas->uState == CIP_PROCESSING");
                    cipshutengine = NULL;
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            for (int i = 0; i < MEMP_NUM_NETCONN; i++)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[i]);
                if ((CIP_PROCESSING == tcpipParas->uState || CIP_CONNECTED == tcpipParas->uState) && tcpipParas->uConnectBearer == gCipBearer)
                {
                    tcpipParas->uState = CIP_INITIAL;
                    if (CFW_TcpipSocketClose(tcpipParas->uSocket) != ERR_SUCCESS)
                    {
                        cipshutengine = NULL;
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                    }
                }
                tcpipParas->uState = CIP_INITIAL;
            }
            if (gCipBearer != BEARER_WIFI)
            {
                OSI_LOGI(0x0, "AT+CIPSHUT: gOutsidePdpCid %d , bearerParas->nCid %d", gOutsidePdpCid, bearerParas->nCid);
#if 1
                if (bearerParas->nState < CIP_GPRSACT || gOutsidePdpCid == bearerParas->nCid)
                {
                    if (gOutsidePdpCid == bearerParas->nCid)
                    {
                        gOutsidePdpCid = 0;
                    }
                    else
                    {
                        CFW_ReleaseCID(bearerParas->nCid, nSim);
                    }
                    bearerParas->nCid = 0;
                    bearerParas->nState = CIP_INITIAL;
                    cipshutengine = NULL;
                    atCmdRespOKText(cmd->engine, "SHUT OK");
                }
                else
                {
                    uint8_t att_state = 0;
                    uint8_t ret = 0;
                    uint8_t act_state = 0;
                    ret = CFW_GetGprsAttState(&att_state, nSim);
                    OSI_LOGI(0x0, "AT+CIPSHUT: ret %d , att_state %d", ret, att_state);
                    if ((0 != ret) || ((CFW_GPRS_DETACHED != att_state) && (CFW_GPRS_ATTACHED != att_state)))
                    {
                        cipshutengine = NULL;
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    if (CFW_GPRS_DETACHED == att_state)
                    {
                        bearerParas->nCid = 0;
                        bearerParas->nState = CIP_INITIAL;
                        cipshutengine = NULL;
                        atCmdRespOKText(cmd->engine, "SHUT OK");
                        return;
                    }
                    ret = CFW_GetGprsActState(bearerParas->nCid, &act_state, nSim);
                    OSI_LOGI(0x0, "AT+CIPSHUT: ret %d , act_state %d", ret, act_state);
                    if ((0 != ret) || ((CFW_GPRS_DEACTIVED != act_state) && (CFW_GPRS_ACTIVED != act_state)))
                    {
                        cipshutengine = NULL;
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    if (CFW_GPRS_DEACTIVED == act_state)
                    {
                        bearerParas->nCid = 0;
                        bearerParas->nState = CIP_INITIAL;
                        cipshutengine = NULL;
                        atCmdRespOKText(cmd->engine, "SHUT OK");
                        return;
                    }
                    osiThreadSleep(500);
                    cmd->uti = cfwRequestUTI((osiEventCallback_t)_cipstartGprsActRsp, cmd);
                    nOperationRet = CFW_GprsAct(CFW_GPRS_DEACTIVED, bearerParas->nCid, cmd->uti, nSim);
                    if (nOperationRet != ERR_SUCCESS)
                    {
                        cfwReleaseUTI(cmd->uti);
                        cipshutengine = NULL;
                        OSI_LOGI(0x10003f6a, "exe       in AT_TCPIP_CmdFunc_CIPSHUT(), Socket closing failed: %x\n", nOperationRet);
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    atCmdSetTimeoutHandler(cmd->engine, 60000, _timeoutabort_shut_Timeout);
                }
#endif
            }
            else
            {
                bearerParas->nState = CIP_GPRSACT;
                cipshutengine = NULL;
                atCmdRespOKText(cmd->engine, "SHUT OK");
            }
        }
        else
        {
            cipshutengine = NULL;
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f6b, "AT+CIPSHUT: command not support");
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CIICR
* Description: Bring up wireless connection with GPRS
* Parameter:  atCommand_t *cmd
* Return:       void
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-19
******************************************************************************/

void AT_TCPIP_CmdFunc_CIICR(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    if (cmd->type == AT_CMD_EXE)
    {

        if (CFW_GetSimStatus(nSim) == CFW_SIM_ABSENT)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

        if (bearerParas->nState != CIP_START)
        {
            OSI_LOGI(0x10003f6c, "AT_TCPIP_CmdFunc_CIICR, TCPIP state error:%d\n", bearerParas->nState);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (gCipBearer == BEARER_WIFI)
        {
            bearerParas->nState = CIP_GPRSACT;
            RETURN_OK(cmd->engine);
        }
        if (0 != cmd->param_count)
        {
            OSI_LOGI(0x10003f6d, "exe       in AT_TCPIP_CmdFunc_CIICR, too many parameters\n");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (gOutsidePdpCid != 0 && bearerParas->nCid == gOutsidePdpCid)
        {
            bearerParas->nState = CIP_GPRSACT;
            RETURN_OK(cmd->engine);
        }

        if ((uint8_t)atGetPocMasterCard() == nSim || atGetPocMasterCard() == CFW_SIM_END)
        {
            if (do_attact(cmd) < 0)
            {
                atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                OSI_LOGI(0, "exe       in AT_TCPIP_CmdFunc_CIICR, change nState to CIP_CONFIG\n");
                bearerParas->nState = CIP_CONFIG;
                atCmdSetTimeoutHandler(cmd->engine, 90000, _timeoutabortTimeout);
            }
        }
        else
        {
            OSI_LOGI(0x0, "The PocMasterCard is not equal to nSim.");
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CSTT
* Description:  Set TCPIP moving scence. Start task and set APN, USER, PWD
* Parameter:    atCommand_t *cmd
* Return:       void
* Remark:       n/a
* Author:       YangYang
* Data:         2008-5-19
******************************************************************************/
void AT_TCPIP_CmdFunc_CSTT(atCommand_t *cmd)
{
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    if (AT_CMD_SET == cmd->type)
    {
        if (bearerParas->nState != CIP_INITIAL && bearerParas->nState != CIP_START)
        {
            OSI_LOGI(0x10003f6e, "AT_TCPIP_CmdFunc_CSTT, TCPIP state error:%d\n", bearerParas->nState);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (gCipBearer == BEARER_WIFI)
            RETURN_OK(cmd->engine);
        if (cmd->param_count != 3)
        {
            OSI_LOGI(0x10003f6f, "AT_TCPIP_CmdFunc_CSTT, parameter error\n");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        bool paramok = true;
        uint8_t *uaApn = NULL;
        uint8_t *uaPwd = NULL;
        uint8_t *uaUsr = NULL;
        if (cmd->param_count > 0)
            uaApn = (uint8_t *)atParamStr(cmd->params[0], &paramok);
        if (!paramok || strlen((char *)uaApn) > THE_APN_MAX_LEN)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (cmd->param_count > 1)
            uaUsr = (uint8_t *)atParamStr(cmd->params[1], &paramok);
        if (!paramok || strlen((char *)uaUsr) > THE_APN_USER_MAX_LEN)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (cmd->param_count > 2)
            uaPwd = (uint8_t *)atParamStr(cmd->params[2], &paramok);
        if (!paramok || strlen((char *)uaPwd) > THE_APN_PWD_MAX_LEN)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t i = 0;
        for (i = 0; i < strlen((const char *)uaApn); i++)
        {
            if ((*(uaApn + i) < 0x2D) ||
                ((*(uaApn + i) > 0x2D && *(uaApn + i) < 0x30) && (*(uaApn + i) != 0x2E)) ||
                (*(uaApn + i) > 0x39 && *(uaApn + i) < 0x41) ||
                (*(uaApn + i) > 0x5A && *(uaApn + i) < 0x61) ||
                (*(uaApn + i) > 0x7A)) //not "A-Z"(0X41-5A) "a-z"(0X61-7A) "0-9" "-"(0X2D)
            {
                OSI_LOGI(0, "cstt apn error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        if (true == useOutsidePdp(cmd))
            OSI_LOGI(0, "AT_TCPIP_CmdFunc_CSTT, use outside pdp:bearerParas->nCid:%d\n", bearerParas->nCid);
        if (cip_getCid(cmd) < 0 || set_userpwdapn(cmd, uaUsr, uaPwd, uaApn) < 0)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        bearerParas->nState = CIP_START;
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_EXE == cmd->type)
    {
        if (bearerParas->nState != CIP_INITIAL && bearerParas->nState != CIP_START)
        {
            OSI_LOGI(0x10003f6e, "AT_TCPIP_CmdFunc_CSTT, TCPIP state error:%d\n", bearerParas->nState);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (true == useOutsidePdp(cmd))
        {
            OSI_LOGI(0, "AT_TCPIP_CmdFunc_CSTT, use outside pdp:bearerParas->nCid:%d\n", bearerParas->nCid);
        }
        else
        {
            if (cip_getCid(cmd) < 0 || set_userpwdapn(cmd, NULL, NULL, NULL) < 0)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        bearerParas->nState = CIP_START;
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        int iResult = 0;
        char *pRsp = NULL;
        CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
        uint8_t nSim = atCmdGetSim(cmd->engine);
        pRsp = (char *)malloc(AT_GPRS_APN_MAX_LEN + AT_GPRS_USR_MAX_LEN + AT_GPRS_USR_MAX_LEN + 15);
        if (NULL == pRsp)
        {
            OSI_LOGI(0x10003f70, "AT_TCPIP_CmdFunc_CSTT(), read mode, memory failed");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        }
        if (0 == bearerParas->nCid)
        {
            memset(pRsp, 0, AT_GPRS_APN_MAX_LEN + AT_GPRS_USR_MAX_LEN + AT_GPRS_USR_MAX_LEN + 15);
            sprintf(pRsp, "+CSTT: \"\", \"\", \"\"");
            atCmdRespInfoText(cmd->engine, pRsp);
            free(pRsp);
            RETURN_OK(cmd->engine);
        }

        if ((iResult = CFW_GprsGetPdpCxtV2(bearerParas->nCid, &sPdpCont, nSim)) != ERR_SUCCESS)
        {
            OSI_LOGI(0x10003f71, "AT_TCPIP_CmdFunc_CSTT(), read mode, CFW_GprsGetPdpCxt:%d,nSim=%d,bearerParas->nCid=%d", iResult, nSim, bearerParas->nCid);
            //atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
            memset(pRsp, 0, AT_GPRS_APN_MAX_LEN + AT_GPRS_USR_MAX_LEN + AT_GPRS_USR_MAX_LEN + 15);
            sprintf(pRsp, "+CSTT: \"\", \"\", \"\"");
            atCmdRespInfoText(cmd->engine, pRsp);
            free(pRsp);
            RETURN_OK(cmd->engine);
        }
        else
        {

            char *tempApn = (char *)malloc(THE_APN_MAX_LEN + 1);
            if (NULL == tempApn)
            {
                OSI_LOGI(0x10003f70, "AT_TCPIP_CmdFunc_CSTT(), tempApn memory failed");
                free(pRsp);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            }
            memset(tempApn, 0, THE_APN_MAX_LEN + 1);
            char *tempUsr = (char *)malloc(THE_APN_USER_MAX_LEN + 1);
            if (NULL == tempUsr)
            {
                OSI_LOGI(0x10003f70, "AT_TCPIP_CmdFunc_CSTT(), tempPwd memory failed");
                free(tempApn);
                tempApn = NULL;
                free(pRsp);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            }
            memset(tempUsr, 0, THE_APN_USER_MAX_LEN + 1);
            char *tempPwd = (char *)malloc(THE_APN_PWD_MAX_LEN + 1);
            if (NULL == tempPwd)
            {
                OSI_LOGI(0x10003f70, "AT_TCPIP_CmdFunc_CSTT(), tempUsr memory failed");
                free(tempApn);
                tempApn = NULL;
                free(tempUsr);
                tempUsr = NULL;
                free(pRsp);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            }
            memset(tempPwd, 0, THE_APN_PWD_MAX_LEN + 1);

            snprintf(tempApn, THE_APN_MAX_LEN + 1, "%s", sPdpCont.pApn);
            snprintf(tempUsr, THE_APN_USER_MAX_LEN + 1, "%s", sPdpCont.pApnUser);
            snprintf(tempPwd, THE_APN_PWD_MAX_LEN + 1, "%s", sPdpCont.pApnPwd);

            memset(pRsp, 0, AT_GPRS_APN_MAX_LEN + AT_GPRS_USR_MAX_LEN + AT_GPRS_USR_MAX_LEN + 15);
            sprintf(pRsp, "+CSTT: \"%s\", \"%s\", \"%s\"", tempApn, tempUsr, tempPwd);

            // free memory
            free(tempApn);
            tempApn = NULL;
            free(tempUsr);
            tempUsr = NULL;
            free(tempPwd);
            tempPwd = NULL;

            atCmdRespInfoText(cmd->engine, pRsp);
            atCmdRespOK(cmd->engine);
        }
        free(pRsp);
        pRsp = NULL;
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char *pRsp = malloc(30);
        if (NULL == pRsp)
        {
            OSI_LOGI(0x10003f70, "AT_TCPIP_CmdFunc_CSTT(), read mode, memory failed");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        }
        memset(pRsp, 0, 30);
        sprintf(pRsp, "+CSTT: \"APN\", \"USER\", \"PWD\"");

        atCmdRespInfoText(cmd->engine, pRsp);
        atCmdRespOK(cmd->engine);
        free(pRsp);
    }
    else
    {
        OSI_LOGI(0x10003f72, "AT_TCPIP_CmdFunc_CSTT(), command type not allowed");
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

#define TCPIP_DATA_MAX_SIZE (10 * 1024)

typedef struct
{
    uint32_t uMuxIndex;
    uint32_t pos;
    uint32_t size;
    uint8_t *data;
} tcpipAsyncCtx_t;

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

static void _tcpipSend(atCommand_t *cmd, uint32_t uMuxIndex, uint8_t *send_data, uint32_t uLength)
{
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
    int32_t iResult;

    if (tcpipParas->uProtocol != CFW_TCPIP_IPPROTO_UDP)
    {
        iResult = CFW_TcpipSocketSend(tcpipParas->uSocket, (uint8_t *)send_data, (uint16_t)uLength, 0);
        if (iResult == SOCKET_ERROR)
        {
            OSI_LOGI(0x10003f2c, "TCPIP send socket data error");
        }
        else
        {
            tcpipParas->sendSize = iResult;
            tcpipParas->sentSize = 0;
        }
    }
    else
    {
        union sockaddr_aligned nDestAddr;
        ip_addr_t nIpAddr;
        ipaddr_aton(tcpipParas->uaIPAddress, &nIpAddr);
#ifdef LWIP_IPV4
        if (IP_IS_V4(&(nIpAddr)))
        {
            struct sockaddr_in *to4 = (struct sockaddr_in *)&(nDestAddr);
            to4->sin_len = sizeof(struct sockaddr_in);
            to4->sin_family = AF_INET;
            to4->sin_port = htons(tcpipParas->uPort);
            inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&(nIpAddr)));
        }
#endif

#if LWIP_IPV6
        if (IP_IS_V6(&(nIpAddr)))
        {
            struct sockaddr_in6 *to6 = (struct sockaddr_in6 *)&(nDestAddr);
            to6->sin6_len = sizeof(struct sockaddr_in6);
            to6->sin6_family = AF_INET6;
            to6->sin6_port = htons(tcpipParas->uPort);
            inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(&(nIpAddr)));
        }
#endif

        iResult = CFW_TcpipSocketSendto(tcpipParas->uSocket, (uint8_t *)send_data, (uint16_t)uLength, 0, (CFW_TCPIP_SOCKET_ADDR *)&nDestAddr, sizeof(nDestAddr));
        if (iResult == SOCKET_ERROR)
        {

            OSI_LOGI(0x10003f7e, "UDP send socket data error");
        }
    }
    if (cipATS_setTimer)
    {
        at_TCPIPBufDestroy(g_bypass_buf);
        g_bypass_buf = NULL;
    }
    char aucBuffer[40] = {0};
    atCmdSetTimeoutHandler(cmd->engine, 120000, _timeoutabortTimeout);
    if (iResult != SOCKET_ERROR)
    {
        if (cipQSEND_quickSend)
        {
            if (!cipMux_multiIp)
                sprintf(aucBuffer, "DATA ACCEPT:%ld", uLength);
            else
                sprintf(aucBuffer, "DATA ACCEPT:%ld,%ld", uMuxIndex, uLength);
        }
        else if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_UDP)
        {
            if (cipSPRT_sendPrompt != 2)
            {
                if (!cipMux_multiIp)
                    sprintf(aucBuffer, "SEND OK");
                else
                    sprintf(aucBuffer, "%ld,SEND OK", uMuxIndex);
            }
        }
        AT_CMD_RETURN(atCmdRespOKText(cmd->engine, aucBuffer));
    }
    else
    {
        if (!cipMux_multiIp)
            sprintf(aucBuffer, "SEND FAIL");
        else
            sprintf(aucBuffer, "%ld,SEND FAIL", uMuxIndex);
        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
    }
}

static void _tcpipDataAutoSendTimeoutCB(void *param)
{
    atCommand_t *cmd = (atCommand_t *)param;
    char aucBuffer[40] = {0};
    tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)cmd->async_ctx;
    if (async->pos == 0)
    {
        snprintf(aucBuffer, sizeof(aucBuffer), "input data timeout");
        atCmdSetLineMode(cmd->engine);
        AT_CMD_RETURN(atCmdRespOKText(cmd->engine, aucBuffer));
    }
    _tcpipSend(cmd, async->uMuxIndex, async->data, async->pos);
    // atCmdSetLineMode(cmd->engine);
}

static int _tcpipDataAutoSendBypassCB(void *param, const void *data, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)cmd->async_ctx;
    size_t len = 0;
    OSI_LOGI(0, "AT CIPSEND size/%d pos/%d data_size/%d",
             async->size, async->pos, size);
    if (async->size != TCPIP_DATA_MAX_SIZE)
    {
        len = async->size - async->pos;
        if (len > size)
            len = size;
        memcpy(async->data + async->pos, data, len);
        async->pos += len;

        atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
        if (chsetting->ate)
            atCmdWrite(cmd->engine, data, len);

        if (async->pos >= async->size)
        {
            stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[async->uMuxIndex]);
            osiTimerStop(tcpipParas->autoSendTimer);
            osiTimerDelete(tcpipParas->autoSendTimer);
            tcpipParas->autoSendTimer = NULL;
            _tcpipSend(cmd, async->uMuxIndex, async->data, async->size);
        }
    }
    else
    {
        len = size;
        uint8_t *cData = (uint8_t *)data;
        uint8_t c = cData[size - 1];
        uint8_t getCTRL_Z = 0;
        uint8_t getESC = 0;
        uint32_t pos = 0;
        while (pos < size)
        {
            if (CHAR_CTRL_Z == cData[pos])
            {
                getCTRL_Z = 1;
                len = pos + 1;
                break;
            }
            if (CHAR_ESC == cData[pos])
            {
                getESC = 1;
                len = pos + 1;
                break;
            }
            pos++;
        }
        if (async->pos + len < async->size)
        {
            memcpy(async->data + async->pos, data, len);
            async->pos += len;

            atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
            if (chsetting->ate)
                atCmdWrite(cmd->engine, data, len);
            if (c == CHAR_CTRL_Z || getCTRL_Z != 0)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[async->uMuxIndex]);
                osiTimerStop(tcpipParas->autoSendTimer);
                osiTimerDelete(tcpipParas->autoSendTimer);
                tcpipParas->autoSendTimer = NULL;
                if (async->pos > 1)
                {
                    _tcpipSend(cmd, async->uMuxIndex, async->data, async->pos - 1);
                    // atCmdSetLineMode(cmd->engine);
                }
                else
                {
                    atCmdRespOK(cmd->engine);
                }
            }
            else if (c == CHAR_ESC || getESC != 0)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[async->uMuxIndex]);
                osiTimerStop(tcpipParas->autoSendTimer);
                osiTimerDelete(tcpipParas->autoSendTimer);
                tcpipParas->autoSendTimer = NULL;
                atCmdRespOK(cmd->engine);
            }
        }
        else
        {
            len = async->size - async->pos;
            if (len == 0)
                return len;
            memcpy(async->data + async->pos, data, len);
            async->pos += len;
            atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
            if (chsetting->ate)
                atCmdWrite(cmd->engine, data, len);
            stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[async->uMuxIndex]);
            osiTimerStop(tcpipParas->autoSendTimer);
            osiTimerDelete(tcpipParas->autoSendTimer);
            tcpipParas->autoSendTimer = NULL;
            _tcpipSend(cmd, async->uMuxIndex, async->data, async->pos);
            // atCmdSetLineMode(cmd->engine);
        }
    }
    return len;
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

    if (async->pos >= async->size)
        _tcpipSend(cmd, async->uMuxIndex, async->data, async->size);
    return len;
}

static void _tcpipDataPromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)cmd->async_ctx;

    if (end_mode == AT_PROMPT_END_ESC)
        RETURN_OK(cmd->engine);
    if (end_mode == AT_PROMPT_END_OVERFLOW)
        AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID));

    OSI_LOGI(0, "AT CIPSEND send size/%d", size);
    if (end_mode == AT_PROMPT_END_CTRL_Z && 1 == size)
        AT_CMD_RETURN(atCmdRespCmsError(cmd->engine, ERR_AT_CMS_INVALID_PARA));
    _tcpipSend(cmd, async->uMuxIndex, async->data, size - 1);
}

/********************************************************************************************
* Name:                    AT_TCPIP_CmdFunc_CIPSEND
* Description:      Send data through TCP or UDP connection
* Parameter:        atCommand_t *cmd
* Return:               void
* Remark:               After cmds +CGATT,+CGDCONT,+CGACT,+CIPSTART return OK,this cmd can be run
* Author/Modifier:      YangYang,wulc
* Data:                 2008-5-21
********************************************************************************************/
void AT_TCPIP_CmdFunc_CIPSEND(atCommand_t *cmd)
{
    OSI_LOGI(0x10005291, "AT+CIPSEND: cmd->type = %d", cmd->type);
    uint32_t uMuxIndex;
    if (AT_CMD_EXE == cmd->type || AT_CMD_SET == cmd->type)
    {
        uMuxIndex = 0;
        if (AT_CMD_EXE == cmd->type && cipMux_multiIp)
        {
            OSI_LOGI(0x10003f74, "AT+CIPSEND: AT_CMD_EXE Only work at nomal mode");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (AT_CMD_EXE == cmd->type && cmd->param_count != 0)
        {
            OSI_LOGI(0x10003f75, "AT+CIPSEND: pPara number is error");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if ((AT_CMD_SET == cmd->type) && ((cmd->param_count == 2 && !cipMux_multiIp) || (cmd->param_count > 2)))
        {
            OSI_LOGI(0x10003f76, "AT+CIPSEND: pPara is error cipMux_multiIp=%d,uParaCount=%d", cipMux_multiIp, cmd->param_count);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        uint32_t uLength = 0;
        bool paramok = true;

        if (cmd->param_count > 0)
        {
            if (cipMux_multiIp)
            {
                uMuxIndex = atParamUintInRange(cmd->params[0], 0, MEMP_NUM_NETCONN - 1, &paramok);
                if (cmd->param_count > 1)
                {
                    uLength = atParamUint(cmd->params[1], &paramok);
                    if (uLength == 0)
                    {
                        OSI_LOGI(0x10003f77, "AT_TCPIP_CmdFunc_CIPSEND: get uLength error");
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                }
            }
            else
            {
                uLength = atParamUint(cmd->params[0], &paramok);
                if (uLength == 0)
                {
                    OSI_LOGI(0x10003f77, "AT_TCPIP_CmdFunc_CIPSEND: get uLength error");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }
            if (!paramok)
            {
                OSI_LOGI(0x10003f77, "AT_TCPIP_CmdFunc_CIPSEND: get ref error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
        if (CIPSEND_MAXSIZE < uLength || tcpipParas->uState != CIP_CONNECTED)
        {
            OSI_LOGI(0x10003f78, "AT+CIPSEND: uLength is %d, tcpipParas->uState=%d", uLength, tcpipParas->uState);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (tcpipParas->engine != cmd->engine)
        {
            OSI_LOGI(0x0, "tcpipParas->engine  %x, cmd->engine %x", tcpipParas->engine, cmd->engine);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        OSI_LOGI(0x10005292, "AT+CIPSEND: cmd->type = %d, uMuxIndex = %d,uLength=%d", cmd->type, uMuxIndex, uLength);
        if (cipATS_setTimer == 1)
        {
            tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)malloc(sizeof(*async));
            uint32_t length = (uLength == 0) ? TCPIP_DATA_MAX_SIZE : uLength;
            if (async == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            cmd->async_ctx = async;
            cmd->async_ctx_destroy = tcpipAsyncCtxDestroy;
            async->uMuxIndex = uMuxIndex;
            async->size = length;
            async->pos = 0;
            async->data = (uint8_t *)calloc(1, length);
            if (async->data == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            if (cipSPRT_sendPrompt == 1)
                atCmdRespOutputPrompt(cmd->engine);
            if (tcpipParas->autoSendTimer != NULL)
                osiTimerDelete(tcpipParas->autoSendTimer);
            tcpipParas->autoSendTimer = osiTimerCreate(osiThreadCurrent(), _tcpipDataAutoSendTimeoutCB, (void *)cmd);
            if (tcpipParas->autoSendTimer == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            osiTimerStart(tcpipParas->autoSendTimer, cipATS_time * 1000);
            atCmdSetBypassMode(cmd->engine, _tcpipDataAutoSendBypassCB, cmd);
        }
        else if (uLength == 0)
        {
            tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)malloc(sizeof(*async));
            if (async == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            memset(async, 0, sizeof(*async));
            cmd->async_ctx = async;
            cmd->async_ctx_destroy = tcpipAsyncCtxDestroy;
            async->data = (uint8_t *)malloc(TCPIP_DATA_MAX_SIZE);
            if (async->data == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            async->uMuxIndex = uMuxIndex;
            if (cipSPRT_sendPrompt == 1)
                atCmdRespOutputPrompt(cmd->engine);
            atCmdSetPromptMode(cmd->engine, _tcpipDataPromptCB, cmd, async->data, TCPIP_DATA_MAX_SIZE);
        }
        else
        {
            tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)malloc(sizeof(*async));
            if (async == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

            cmd->async_ctx = async;
            cmd->async_ctx_destroy = tcpipAsyncCtxDestroy;
            async->uMuxIndex = uMuxIndex;
            async->size = uLength;
            async->pos = 0;
            async->data = (uint8_t *)malloc(uLength);
            if (async->data == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
            if (cipSPRT_sendPrompt == 1)
                atCmdRespOutputPrompt(cmd->engine);
            atCmdSetBypassMode(cmd->engine, _tcpipDataBypassCB, cmd);
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[40] = {0};
        if (!cipMux_multiIp)
            sprintf(aucBuffer, "+CIPSEND:<length>");
        else
            sprintf(aucBuffer, "+CIPSEND:<0-7>,<length>");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[160] = {0};
        char spf_buf[20] = {0};
        uint8_t i;
        if (!cipMux_multiIp)
            sprintf(aucBuffer, "+CIPSEND:%d", CIPSEND_MAXSIZE);
        else
        {
            for (i = 0; i < 8; i++)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[i]);
                if (tcpipParas->uState == CIP_CONNECTED)
                {
                    sprintf(spf_buf, "+CIPSEND:%d,%d\n", i, CIPSEND_MAXSIZE);
                    strcat(aucBuffer, spf_buf);
                }
            }
            if (strlen(aucBuffer) == 0)
            {
                sprintf(aucBuffer, "+CIPSEND:%s", "NO CONNECTION");
            }
        }
        atCmdRespInfoText(cmd->engine, aucBuffer);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f7f, "AT_TCPIP_CmdFunc_CIPSEND(), command type not allowed");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CIPQSEND(atCommand_t *cmd)
{
    OSI_LOGI(0x10005293, "AT+CIPQSEND ... ... ");

    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        bool paramok = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        else
            cipQSEND_quickSend = mode;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPQSEND:%d", cipQSEND_quickSend);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPQSEND:(0,1)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f81, "AT_TCPIP_CmdFunc_CIPQSEND, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CIPRXGET(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 4, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if ((mode == 4 || mode == 0 || mode == 1) && !cipMux_multiIp && cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if ((mode == 4 || mode == 0 || mode == 1) && cipMux_multiIp && cmd->param_count != 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (mode == 1 || mode == 0)
        {
            if ((cipRXGET_manualy == 1 && mode == 1) || (cipRXGET_manualy == 0 && mode == 0))
            {
                char uaIpStr[300] = {
                    0,
                };
                if (cipSRIP_showIPPort)
                {
                    if (cmd->param_count == 1)
                    {
                        char *ipAddr = (char *)&(g_uCipContexts.nTcpipParas[0].uaIPAddress[0]);
                        sprintf(uaIpStr, "+CIPRXGET:%d,%s:%d", cipRXGET_manualy, ipAddr, g_uCipContexts.nTcpipParas[0].uPort);
                    }
                    else
                    {
                        uint8_t uMuxIndex = 0;
                        uMuxIndex = atParamUintInRange(cmd->params[1], 0, 7, &paramok);
                        if (!paramok)
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                        char *ipAddr = (char *)&(g_uCipContexts.nTcpipParas[uMuxIndex].uaIPAddress[0]);
                        sprintf(uaIpStr, "+CIPRXGET:%d,%d,%s:%d", cipRXGET_manualy, uMuxIndex, ipAddr, g_uCipContexts.nTcpipParas[uMuxIndex].uPort);
                    }
                }
                else
                {
                    if (cmd->param_count == 1)
                        sprintf(uaIpStr, "+CIPRXGET:%d", cipRXGET_manualy);
                    else
                    {
                        uint8_t uMuxIndex = 0;
                        uMuxIndex = atParamUintInRange(cmd->params[1], 0, 7, &paramok);
                        if (!paramok)
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                        sprintf(uaIpStr, "+CIPRXGET:%d,%d", cipRXGET_manualy, uMuxIndex);
                    }
                }
                atCmdRespInfoText(cmd->engine, uaIpStr);
            }
            cipRXGET_manualy = mode;
            RETURN_OK(cmd->engine);
        }
        else if (((mode == 2 || mode == 3) && cmd->param_count == 2 + cipMux_multiIp) ||
                 ((mode == 4) && cmd->param_count == 1 + cipMux_multiIp))

        {
            uint8_t uMuxIndex = 0;
            uint32_t requestLen = 0;
            uint32_t range = 0;
            if (cipMux_multiIp)
                uMuxIndex = atParamUintInRange(cmd->params[1], 0, 7, &paramok);
            if (mode == 2)
                range = 1460;
            else if (mode == 3)
                range = 730;
            if (range != 0)
                requestLen = atParamUintInRange(cmd->params[1 + cipMux_multiIp], 1, range, &paramok);

            stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
            if (paramok)
            {
                if (recv_data(tcpipParas->uSocket, requestLen, cmd->engine, uMuxIndex, mode) == ERR_SUCCESS)
                {
                    RETURN_OK(cmd->engine);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            else
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        else
        {
            OSI_LOGI(0x10003f82, "AT_TCPIP_CmdFunc_CIPRXGET, mode:%d,cipRXGET_manualy:%d,cipMux_multiIp:%d", mode, cipRXGET_manualy, cipMux_multiIp);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPRXGET:%d", cipRXGET_manualy);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[40] = {0};
        if (cipMux_multiIp)
        {
            sprintf(aucBuffer, "+CIPRXGET:(0-4), (0-7), (1-1460)");
            atCmdRespInfoText(cmd->engine, aucBuffer);
        }
        else
        {
            sprintf(aucBuffer, "+CIPRXGET:(0-4), (1-1460)");
            atCmdRespInfoText(cmd->engine, aucBuffer);
        }
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f83, "AT_TCPIP_CmdFunc_CIPRXGET, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CDNSCFG
* Description:  Config DNS
* Parameter:    atCommand_t *cmd
* Return:       void
* Remark:       n/a
* Author:       
* Data:         20170907
******************************************************************************/

void AT_TCPIP_CmdFunc_CDNSCFG(atCommand_t *cmd)
{
    OSI_LOGI(0x10005294, "AT+CDNSCFG ... ...");

    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count > 2 || cmd->param_count < 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (netif_default == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t cid = netif_default->sim_cid & 0x0f;
        uint8_t nSim = (netif_default->sim_cid & 0xf0) >> 4;
        char *priDNS = NULL;
        char *secDNS = NULL;

        bool paramok = true;
        if (cmd->param_count > 0)
        {
            priDNS = (char *)atParamStr(cmd->params[0], &paramok);
            if (cmd->param_count > 1)
                secDNS = (char *)atParamStr(cmd->params[1], &paramok);
            else
                secDNS = "0.0.0.0";
        }
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        bool dns1_isdigit = true;
        bool dns2_isdigit = true;
        char *c1;
        char *c2;
        for (c1 = priDNS; *c1 != 0; c1++)
        {
            if (*c1 == '.' || *c1 == ':')
            {
                dns1_isdigit = false;
                break;
            }
        }
        for (c2 = secDNS; *c2 != 0; c2++)
        {
            if (*c1 == '.' || *c1 == ':')
            {
                dns2_isdigit = false;
                break;
            }
        }
        OSI_LOGI(0, "dns1_isdigit=%d,dns2_isdigit=%d", dns1_isdigit, dns2_isdigit);
        if (dns1_isdigit)
        {
            char *endptr1;
            unsigned long priDNS_digit = strtoul(priDNS, &endptr1, 10);
            OSI_LOGI(0, "priDNS_digit=%lu", priDNS_digit);
            if (priDNS_digit >= 0xffffffff)
            {
                OSI_LOGI(0, "priDNS_digit>=4294967295");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        if (dns2_isdigit)
        {
            char *endptr2;
            unsigned long secDNS_digit = strtoul(secDNS, &endptr2, 10);
            OSI_LOGI(0, "secDNS_digit=%lu", secDNS_digit);
            if (secDNS_digit >= 0xffffffff)
            {
                OSI_LOGI(0, "secDNS_digit>=4294967295");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        OSI_LOGXI(OSI_LOGPAR_ISS, 0x10003f86, "AT_TCPIP_CmdFunc_CDNSCFG(), cid = %d, priDNS = %s, secDNS=%s ", cid, priDNS, secDNS);
        ip_addr_t dns_server;
        ip_addr_t dns_server2;
        if (ipaddr_aton(priDNS, &dns_server) == 0 || ipaddr_aton(secDNS, &dns_server2) == 0)
        {
            OSI_LOGI(0x10003f85, "AT_TCPIP_CmdFunc_CDNSCFG(),param error");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (IP_IS_V4(&dns_server))
        {
            if (ip4_addr_get_u32(ip_2_ip4(&dns_server)) == 0xffffffff)
            {
                //OSI_LOGI(0, "dns_server.u_addr.ip4.addr=%lu", dns_server.u_addr.ip4.addr);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        if (IP_IS_V4(&dns_server2))
        {
            if (ip4_addr_get_u32(ip_2_ip4(&dns_server2)) == 0xffffffff)
            {
                //OSI_LOGI(0, "dns_server2.u_addr.ip4.addr=%lu", dns_server2.u_addr.ip4.addr);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        if (cid > 0 && cid < 8)
        {
            CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
            if (CFW_GprsGetPdpCxtV2(cid, &sPdpCont, nSim) == ERR_SUCCESS)
            {
                //if ((IP_IS_V4(&dns_server)) && (IP_IS_V4(&dns_server2)) && ((sPdpCont.PdnType == CFW_GPRS_PDP_TYPE_IP) || (sPdpCont.PdnType == CFW_GPRS_PDP_TYPE_IPV4V6)))
                if (sPdpCont.PdnType == CFW_GPRS_PDP_TYPE_IP)
                {
                    uint32_t Pro_DnsIp1 = (uint32_t)inet_addr((const char *)priDNS);
                    uint32_t Sec_DnsIp1 = (uint32_t)inet_addr((const char *)secDNS);
                    if (IP_IS_V4(&dns_server))
                    {
                        sPdpCont.pPdpDns[0] = Pro_DnsIp1;
                        sPdpCont.pPdpDns[1] = Pro_DnsIp1 >> 8;
                        sPdpCont.pPdpDns[2] = Pro_DnsIp1 >> 16;
                        sPdpCont.pPdpDns[3] = Pro_DnsIp1 >> 24;
                        if (cmd->param_count > 1)
                        {
                            if (IP_IS_V4(&dns_server2))
                            {
                                sPdpCont.pPdpDns[21] = Sec_DnsIp1;
                                sPdpCont.pPdpDns[22] = Sec_DnsIp1 >> 8;
                                sPdpCont.pPdpDns[23] = Sec_DnsIp1 >> 16;
                                sPdpCont.pPdpDns[24] = Sec_DnsIp1 >> 24;
                            }
                            else
                            {
                                OSI_LOGI(0, "secDNS is not ipv4 address");
                                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                            }
                        }
                        CFW_GprsSetPdpCxtV2(cid, &sPdpCont, nSim);
                    }
                    else
                    {
                        OSI_LOGI(0, "priDNS is not ipv4 address");
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    ip_addr_t dns_server[2] = {0};
                    IP_ADDR4(&dns_server[0], sPdpCont.pPdpDns[0], sPdpCont.pPdpDns[1], sPdpCont.pPdpDns[2], sPdpCont.pPdpDns[3]);
                    IP_ADDR4(&dns_server[1], sPdpCont.pPdpDns[THE_PDP_DNS_MAX_LEN + 0], sPdpCont.pPdpDns[THE_PDP_DNS_MAX_LEN + 1],
                             sPdpCont.pPdpDns[THE_PDP_DNS_MAX_LEN + 2], sPdpCont.pPdpDns[THE_PDP_DNS_MAX_LEN + 3]);
                    dns_setserver(0, &dns_server[0]);
                    if (cmd->param_count > 1)
                    {
                        dns_setserver(1, &dns_server[1]);
                    }
                }
#if LWIP_IPV6
                //else if (IP_IS_V6(&dns_server) && ((sPdpCont.PdnType == CFW_GPRS_PDP_TYPE_IPV6) || (sPdpCont.PdnType == CFW_GPRS_PDP_TYPE_IPV4V6)))
                else if (sPdpCont.PdnType == CFW_GPRS_PDP_TYPE_IPV6)
                {
                    if (IP_IS_V6(&dns_server))
                    {
                        if (cmd->param_count == 1)
                        {
                            secDNS = "::";
                            ipaddr_aton(secDNS, &dns_server2);
                        }
                        if (!IP_IS_V6(&dns_server2))
                        {
                            OSI_LOGI(0, "secDNS is not ipv6 address");
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                        }
                    }
                    else
                    {
                        OSI_LOGI(0, "priDNS is not ipv6 address");
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    //ipaddr_aton(secDNS, &dns_server2);
                    ip6_addr_t dns1_ip6;
                    ip6_addr_t dns2_ip6;
                    ip6addr_aton(priDNS, &dns1_ip6);
                    ip6addr_aton(secDNS, &dns2_ip6);
                    memcpy(&sPdpCont.pPdpDns[4], &dns1_ip6.addr[0], 16);
                    if (cmd->param_count > 1)
                    {
                        memcpy(&sPdpCont.pPdpDns[25], &dns2_ip6.addr[0], 16);
                    }
                    //CFW_GprsSetPdpCxtV2(cid, &sPdpCont, nSim);
                    if (ERR_SUCCESS != CFW_GprsSetPdpCxtV2(cid, &sPdpCont, nSim))
                    {
                        OSI_LOGI(0, "Set PDP context not success!");
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    ip_addr_t dns_server[2] = {0};
                    uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[4], sPdpCont.pPdpDns[5], sPdpCont.pPdpDns[6], sPdpCont.pPdpDns[7]));
                    uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[8], sPdpCont.pPdpDns[9], sPdpCont.pPdpDns[10], sPdpCont.pPdpDns[11]));
                    uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[12], sPdpCont.pPdpDns[13], sPdpCont.pPdpDns[14], sPdpCont.pPdpDns[15]));
                    uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[16], sPdpCont.pPdpDns[17], sPdpCont.pPdpDns[18], sPdpCont.pPdpDns[19]));
                    uint32_t addr4 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[25], sPdpCont.pPdpDns[26], sPdpCont.pPdpDns[27], sPdpCont.pPdpDns[28]));
                    uint32_t addr5 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[29], sPdpCont.pPdpDns[30], sPdpCont.pPdpDns[31], sPdpCont.pPdpDns[32]));
                    uint32_t addr6 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[33], sPdpCont.pPdpDns[34], sPdpCont.pPdpDns[35], sPdpCont.pPdpDns[36]));
                    uint32_t addr7 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[37], sPdpCont.pPdpDns[38], sPdpCont.pPdpDns[39], sPdpCont.pPdpDns[40]));
                    OSI_LOGI(0, "DNS size:%d  ipv6 DNS 12-15:%d.%d.%d.%d", sPdpCont.nPdpDnsSize,
                             sPdpCont.pPdpDns[12], sPdpCont.pPdpDns[13], sPdpCont.pPdpDns[14], sPdpCont.pPdpDns[15]);
                    OSI_LOGI(0, " ipv6 DNS 33-36:%d.%d.%d.%d",
                             sPdpCont.pPdpDns[33], sPdpCont.pPdpDns[34], sPdpCont.pPdpDns[35], sPdpCont.pPdpDns[36]);

                    IP_ADDR6(&dns_server[0], addr0, addr1, addr2, addr3);
                    IP_ADDR6(&dns_server[1], addr4, addr5, addr6, addr7);
                    dns_setserver(0, &dns_server[0]);
                    if (cmd->param_count > 1)
                    {
                        dns_setserver(1, &dns_server[1]);
                    }
                }
                else if (sPdpCont.PdnType == CFW_GPRS_PDP_TYPE_IPV4V6)
                {
                    if (IP_IS_V4(&dns_server))
                    {
                        uint32_t Pro_DnsIp1 = (uint32_t)inet_addr((const char *)priDNS);
                        sPdpCont.pPdpDns[0] = Pro_DnsIp1;
                        sPdpCont.pPdpDns[1] = Pro_DnsIp1 >> 8;
                        sPdpCont.pPdpDns[2] = Pro_DnsIp1 >> 16;
                        sPdpCont.pPdpDns[3] = Pro_DnsIp1 >> 24;
                        if (cmd->param_count > 1)
                        {
                            if (IP_IS_V4(&dns_server2))
                            {
                                uint32_t Sec_DnsIp1 = (uint32_t)inet_addr((const char *)secDNS);
                                sPdpCont.pPdpDns[21] = Sec_DnsIp1;
                                sPdpCont.pPdpDns[22] = Sec_DnsIp1 >> 8;
                                sPdpCont.pPdpDns[23] = Sec_DnsIp1 >> 16;
                                sPdpCont.pPdpDns[24] = Sec_DnsIp1 >> 24;
                            }
                            else
                            {
                                if (IP_IS_V6(&dns_server2))
                                {
                                    ip6_addr_t dns2_ip6;
                                    ip6addr_aton(secDNS, &dns2_ip6);
                                    memcpy(&sPdpCont.pPdpDns[4], &dns2_ip6.addr[0], 16);
                                }
                            }
                        }
                        if (CFW_GprsSetPdpCxtV2(cid, &sPdpCont, nSim) != ERR_SUCCESS)
                        {
                            OSI_LOGI(0, "Set PDP context not success!");
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                        }
                        ip_addr_t dns_server[2] = {0};
                        IP_ADDR4(&dns_server[0], sPdpCont.pPdpDns[0], sPdpCont.pPdpDns[1], sPdpCont.pPdpDns[2], sPdpCont.pPdpDns[3]);
                        dns_setserver(0, &dns_server[0]);
                        if (cmd->param_count > 1)
                        {
                            if (IP_IS_V4(&dns_server2))
                            {
                                IP_ADDR4(&dns_server[1], sPdpCont.pPdpDns[21], sPdpCont.pPdpDns[22], sPdpCont.pPdpDns[23], sPdpCont.pPdpDns[24]);
                            }
                            else
                            {
                                uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[4], sPdpCont.pPdpDns[5], sPdpCont.pPdpDns[6], sPdpCont.pPdpDns[7]));
                                uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[8], sPdpCont.pPdpDns[9], sPdpCont.pPdpDns[10], sPdpCont.pPdpDns[11]));
                                uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[12], sPdpCont.pPdpDns[13], sPdpCont.pPdpDns[14], sPdpCont.pPdpDns[15]));
                                uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[16], sPdpCont.pPdpDns[17], sPdpCont.pPdpDns[18], sPdpCont.pPdpDns[19]));
                                IP_ADDR6(&dns_server[1], addr0, addr1, addr2, addr3);
                            }
                            dns_setserver(1, &dns_server[1]);
                        }
                    }
                    else if (IP_IS_V6(&dns_server))
                    {
                        ip6_addr_t dns1_ip6;
                        ip6addr_aton(priDNS, &dns1_ip6);
                        memcpy(&sPdpCont.pPdpDns[4], &dns1_ip6.addr[0], 16);

                        if (cmd->param_count > 1)
                        {
                            if (IP_IS_V4(&dns_server2))
                            {
                                uint32_t Sec_DnsIp1 = (uint32_t)inet_addr((const char *)secDNS);
                                sPdpCont.pPdpDns[0] = Sec_DnsIp1;
                                sPdpCont.pPdpDns[1] = Sec_DnsIp1 >> 8;
                                sPdpCont.pPdpDns[2] = Sec_DnsIp1 >> 16;
                                sPdpCont.pPdpDns[3] = Sec_DnsIp1 >> 24;
                            }
                            else if (IP_IS_V6(&dns_server2))
                            {
                                ip6_addr_t dns2_ip6;
                                ip6addr_aton(secDNS, &dns2_ip6);
                                memcpy(&sPdpCont.pPdpDns[25], &dns2_ip6.addr[0], 16);
                            }
                        }
                        if (CFW_GprsSetPdpCxtV2(cid, &sPdpCont, nSim) != ERR_SUCCESS)
                        {
                            OSI_LOGI(0, "Set PDP context not success!");
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                        }
                        ip_addr_t dns_server[2] = {0};
                        uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[4], sPdpCont.pPdpDns[5], sPdpCont.pPdpDns[6], sPdpCont.pPdpDns[7]));
                        uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[8], sPdpCont.pPdpDns[9], sPdpCont.pPdpDns[10], sPdpCont.pPdpDns[11]));
                        uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[12], sPdpCont.pPdpDns[13], sPdpCont.pPdpDns[14], sPdpCont.pPdpDns[15]));
                        uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[16], sPdpCont.pPdpDns[17], sPdpCont.pPdpDns[18], sPdpCont.pPdpDns[19]));
                        IP_ADDR6(&dns_server[0], addr0, addr1, addr2, addr3);
                        dns_setserver(0, &dns_server[0]);
                        if (cmd->param_count > 1)
                        {
                            if (IP_IS_V4(&dns_server2))
                            {
                                IP_ADDR4(&dns_server[1], sPdpCont.pPdpDns[0], sPdpCont.pPdpDns[1], sPdpCont.pPdpDns[2], sPdpCont.pPdpDns[3]);
                            }
                            else if (IP_IS_V6(&dns_server2))
                            {
                                uint32_t addr4 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[25], sPdpCont.pPdpDns[26], sPdpCont.pPdpDns[27], sPdpCont.pPdpDns[28]));
                                uint32_t addr5 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[29], sPdpCont.pPdpDns[30], sPdpCont.pPdpDns[31], sPdpCont.pPdpDns[32]));
                                uint32_t addr6 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[33], sPdpCont.pPdpDns[34], sPdpCont.pPdpDns[35], sPdpCont.pPdpDns[36]));
                                uint32_t addr7 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[37], sPdpCont.pPdpDns[38], sPdpCont.pPdpDns[39], sPdpCont.pPdpDns[40]));
                                IP_ADDR6(&dns_server[1], addr4, addr5, addr6, addr7);
                            }
                            dns_setserver(1, &dns_server[1]);
                        }
                    }
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
#endif
            }
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char outString[256] = {
            0x00,
        };
        char addr_str[200] = {
            0x00,
        };
        if (netif_default == NULL)
        {
            sprintf(outString, "+CDNSCFG: DNS1:0.0.0.0 DNS2:0.0.0.0");
            atCmdRespInfoText(cmd->engine, outString);
            RETURN_OK(cmd->engine);
        }

        uint8_t cid = netif_default->sim_cid & 0x0f;
        uint8_t sim = (netif_default->sim_cid & 0xf0) >> 4;
        OSI_LOGI(0, "+CDNSCFG sim %d cid %d", sim, cid);
        CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
        CFW_GprsGetPdpCxtV2(cid, &sPdpCont, sim);
        _dnsAddressToStr(&sPdpCont, addr_str);
        sprintf(outString, "+CDNSCFG: %s", addr_str);
        atCmdRespInfoText(cmd->engine, outString);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char outString[50] = {
            0x00,
        };
        sprintf(outString, "+CDNSCFG: \"PrimaryDNS\",\"SecondaryDNS\"");
        atCmdRespInfoText(cmd->engine, outString);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f87, "AT_TCPIP_CmdFunc_CDNSCFG, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CIPSPRT
* Description:  Config DNS
* Parameter:    atCommand_t *cmd
* Return:       void
* Remark:       n/a
* Author:       
* Data:         20170907
******************************************************************************/

void AT_TCPIP_CmdFunc_CIPSPRT(atCommand_t *cmd)
{
    OSI_LOGI(0x10005295, "AT+CIPSPRT ... ...");

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        uint8_t prompt = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        cipSPRT_sendPrompt = prompt;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPSPRT:%d", cipSPRT_sendPrompt);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPSPRT:(0,1,2)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f89, "AT_TCPIP_CmdFunc_CIPSPRT, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CIPHEAD
* Description:  
* Parameter:    atCommand_t *cmd
* Return:       void
* Remark:       n/a
* Author:       
* Data:         20170907
******************************************************************************/

void AT_TCPIP_CmdFunc_CIPHEAD(atCommand_t *cmd)
{

    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        bool paramok = true;
        uint8_t header = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        cipHEAD_addIpHead = header;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPHEAD:%d", cipHEAD_addIpHead);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPHEAD:(0,1)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f8a, "AT_TCPIP_CmdFunc_CIPHEAD, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CIPSRIP(atCommand_t *cmd)
{

    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        bool paramok = true;
        uint8_t header = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        cipSRIP_showIPPort = header;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPSRIP:%d", cipSRIP_showIPPort);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPSRIP:(0,1)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f8b, "AT_TCPIP_CmdFunc_CIPSRIP, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:        AT_TCPIP_CmdFunc_CIPATS
* Description: Set auto sending timer
* Parameter:  atCommand_t *cmd
* Return:       void
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-21
******************************************************************************/
void AT_TCPIP_CmdFunc_CIPATS(atCommand_t *cmd)
{
    uint16_t tm;

    OSI_LOGI(0x10005296, "AT+CIPATS ... ...");
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if ((mode == 0 && cmd->param_count != 1) || (mode == 1 && cmd->param_count != 2))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (1 == mode)
            tm = atParamUintInRange(cmd->params[1], 1, 100, &paramok);
        else
            tm = 1;
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        cipATS_setTimer = mode;
        cipATS_time = tm;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPATS:%d,%d", cipATS_setTimer, cipATS_time);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[40] = {0};

        sprintf(aucBuffer, "+CIPATS:(0,1),(1-100)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f8d, "AT_TCPIP_CmdFunc_CIPATS, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************

* Name:        void AT_TCPIP_CmdFunc_CIPSTATUS

* Description: Query the current connection status.

* Parameter:  atCommand_t *cmd

* Return:       void

* Remark:      n/a

* Author:       Chang Yuguang

* Data:          2008-5-19

******************************************************************************/
void AT_TCPIP_CmdFunc_CIPSTATUS(atCommand_t *cmd)
{
    char aucBuffer[256] = {0};

    OSI_LOGI(0x10005297, "AT+CIPSTATUS ... ...");
    atCmdEngine_t *cmdEngine = cmd->engine;

    if (cmd->type == AT_CMD_SET)
    {
        if (!cipMux_multiIp)
        {
            OSI_LOGI(0x10003f8f, "AT_TCPIP_CmdFunc_CIPSTATUS, command type:%d not allowed", cmd->type);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        }
        else
        {
            if (cmd->param_count != 1)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            bool paramok = true;
            uint8_t mode = atParamUintInRange(cmd->params[0], 0, 7, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            sprintf(aucBuffer, "+CIPSTATUS:%d,%d,%s,%s,%d,%s", mode,
                    g_uCipContexts.nTcpipParas[mode].uConnectBearer,
                    g_uCipContexts.nTcpipParas[mode].uProtocol == CFW_TCPIP_IPPROTO_UDP ? "UDP" : "TCP",
                    g_uCipContexts.nTcpipParas[mode].uaIPAddress,
                    g_uCipContexts.nTcpipParas[mode].uPort,
                    g_strATTcpipStatus[g_uCipContexts.nTcpipParas[mode].uState]);
            //atCmdRespUrcText(cmd->engine, aucBuffer);
            atCmdRespInfoText(cmd->engine, aucBuffer);
            RETURN_OK(cmd->engine);
        }
    }
    else if (cmd->type == AT_CMD_EXE)
    {
        if (cmd->param_count != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        CIP_STATUS_T state = CIP_INITIAL;
        CIP_STATUS_T tcp_state = CIP_INITIAL;
        CIP_STATUS_T bearer_state = CIP_INITIAL;
        for (int i = 0; i < MEMP_NUM_NETCONN; i++)
        {
            CIP_STATUS_T s2 = g_uCipContexts.nTcpipParas[i].uState;
            if (tcp_state < s2)
                tcp_state = s2;
        }
        for (int i = 0; i < BEARER_MAX_NUM; i++)
        {
            CIP_STATUS_T s = g_uCipContexts.nBearerParas[i].nState;
            if (bearer_state < s)
                bearer_state = s;
        }
        state = tcp_state > bearer_state ? tcp_state : bearer_state;
        OSI_LOGI(0x0, "AT+CIPSTATUS ... ... tcp_state= %d bearer_state= %d", tcp_state, bearer_state);
        if (tcp_state == CIP_CLOSED)
        {
            if (bearer_state == CIP_GPRSACT || bearer_state == CIP_STATUS)
                state = tcp_state;
            else
                state = bearer_state;
        }
        if (cipMux_multiIp || gCipserver.mode == CIPSERVER_OPEN)
        {
            atCmdRespInfoText(cmd->engine, "OK");
            sprintf(aucBuffer, "\r\nSTATE:%s", g_strATTcpipStatus[bearer_state]);
            atCmdRespInfoText(cmdEngine, aucBuffer);

            if (gCipserver.mode == CIPSERVER_OPEN)
            {
                sprintf(aucBuffer, "\r\nS:0,0,\"%ld\",\"%s\"", gCipserver.port, "LISTENING");
                atCmdRespInfoText(cmdEngine, aucBuffer);
            }

            for (int i = 0; i < MEMP_NUM_NETCONN - (CIPSERVER_OPEN == gCipserver.mode); i++)
            {
                sprintf(aucBuffer, "\r\nC:%d,%d,%s,%s,%d,%s", i,
                        g_uCipContexts.nTcpipParas[i].uConnectBearer,
                        g_uCipContexts.nTcpipParas[i].uProtocol == CFW_TCPIP_IPPROTO_UDP ? "UDP" : "TCP",
                        g_uCipContexts.nTcpipParas[i].uaIPAddress,
                        g_uCipContexts.nTcpipParas[i].uPort,
                        g_strATTcpipStatus[g_uCipContexts.nTcpipParas[i].uState]);
                atCmdRespInfoText(cmdEngine, aucBuffer);
            }
            atCmdRespOKText(cmd->engine, "");
        }
        else
        {
            atCmdRespInfoText(cmdEngine, "OK");
            sprintf(aucBuffer, "STATE:%s", g_strATTcpipStatus[state]);
            atCmdRespOKText(cmdEngine, aucBuffer);
            //atCmdRespOKText(cmdEngine, "");
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        //sprintf(aucBuffer, "+CIPSTATUS:");
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f8f, "AT_TCPIP_CmdFunc_CIPSTATUS, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:        AT_TCPIP_CmdFunc_CIPSCONT
* Description: Save TCP/IP parameter into flash/Display TCPIP settings.
* Parameter:  atCommand_t *cmd
* Return:       void
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-21
******************************************************************************/
void AT_TCPIP_CmdFunc_CIPSCONT(atCommand_t *cmd)
{
    //uint8_t nSim = 0;

    if (AT_CMD_EXE == cmd->type)
    {
        //at_CfgStoreTcpipSetting(&gCIPSettings);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char *pRsp = NULL;
        pRsp = malloc(1024);

        if (NULL == pRsp)
        {
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_NO_MEMORY);
            return;
        }

        sprintf(pRsp, "+CIPTKA: %d,%d,%d,%d", g_keepalive, g_keepidle, g_keepinterval, g_keepcount);
        //atCmdRespUrcText(cmd->engine, pRsp);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPSCONT: %d", cipSCONT_unSaved);
        atCmdRespInfoText(cmd->engine, pRsp);
        char *cipApnC = (char *)&(cipApn[0]);
        char *cipUserNameC = (char *)&(cipUserName[0]);
        char *cipPassWordC = (char *)&(cipPassWord[0]);
        sprintf(pRsp, "+CIPCSGP: %d\r\nGprs Config APN: %s\r\nGprs Config UserId: %s\r\nGprs Config Password: %s", cipCSGP_setGprs, cipApnC, cipUserNameC, cipPassWordC);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPHEAD: %d", cipHEAD_addIpHead);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPSHOWTP: %d", cipSHOWTP_dispTP);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPSRIP: %d", cipSRIP_showIPPort);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPATS: %d,%d", cipATS_setTimer, cipATS_time);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPSPRT: %d", cipSPRT_sendPrompt);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPQSEND: %d", cipQSEND_quickSend);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPMODE: %d", cipMODE_transParent);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPCCFG: %d,%d,%d,%d,%d,%d,%d", cipCCFG_NmRetry, cipCCFG_WaitTm,
                cipCCFG_SendSz, cipCCFG_esc, cipCCFG_Rxmode, cipCCFG_RxSize, cipCCFG_Rxtimer);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPMUX: %d", cipMux_multiIp);
        atCmdRespInfoText(cmd->engine, pRsp);
        //sprintf(pRsp, "+CIPDPDP:");
        //atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPRXGET: %d", cipRXGET_manualy);
        atCmdRespInfoText(cmd->engine, pRsp);
        sprintf(pRsp, "+CIPRDTIMER: %d,%d", g_rdsigtimer, g_rdmuxtimer);
        atCmdRespInfoText(cmd->engine, pRsp);

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

void AT_TCPIP_CmdFunc_CIPACK(atCommand_t *cmd)
{
    OSI_LOGI(0x10005298, "AT+CIPACK ... ...");
    if (cmd->type == AT_CMD_EXE)
    {
        if (!cipMux_multiIp)
        {
            stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[0]);
            int acklen = CFW_TcpipSocketGetAckedSize(tcpipParas->uSocket);
            int sendlen = CFW_TcpipSocketGetSentSize(tcpipParas->uSocket);
            char aucBuffer[60] = {0};
            if (acklen < 0)
                acklen = 0;
            if (sendlen < 0)
                sendlen = 0;
            sprintf(aucBuffer, "+CIPACK:%d,%d,%d", sendlen, acklen, sendlen - acklen);
            atCmdRespInfoText(cmd->engine, aucBuffer);
            RETURN_OK(cmd->engine);
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    else if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t uMuxIndex = 0;
        if (cipMux_multiIp && cmd->param_count == 1)
        {
            uMuxIndex = atParamUintInRange(cmd->params[0], 0, 7, &paramok);
            if (paramok)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
                int acklen = CFW_TcpipSocketGetAckedSize(tcpipParas->uSocket);
                int sendlen = CFW_TcpipSocketGetSentSize(tcpipParas->uSocket);
                char aucBuffer[100] = {0};
                if (acklen < 0)
                    acklen = 0;
                if (sendlen < 0)
                    sendlen = 0;
                sprintf(aucBuffer, "+CIPACK:%d,%d,%d", sendlen, acklen, sendlen - acklen);
                atCmdRespInfoText(cmd->engine, aucBuffer);
                RETURN_OK(cmd->engine);
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f91, "AT_TCPIP_CmdFunc_CIPACK, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CLPORT(atCommand_t *cmd)
{
    OSI_LOGI(0x10005299, "AT+CLPORT ... ...");

    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint16_t uPort = 0;
        uint8_t uMuxIndex = 0;
        if (cipMux_multiIp)
        {
            if (cmd->param_count != 3)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            uMuxIndex = atParamUintInRange(cmd->params[0], 0, 7, &paramok);
        }
        else
        {
            if (cmd->param_count != 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        char *mode = (char *)atParamStr(cmd->params[0 + cipMux_multiIp], &paramok);
        uPort = atParamUintInRange(cmd->params[1 + cipMux_multiIp], 0, 65535, &paramok);
        if (paramok)
        {
            if ((strcmp(mode, "TCP") == 0) || (strcmp(mode, "tcp") == 0))
            {
                gClocalport[uMuxIndex].tcp_port = uPort;
            }
            else if ((strcmp(mode, "UDP") == 0) || (strcmp(mode, "udp") == 0))
            {
                gClocalport[uMuxIndex].udp_port = uPort;
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            RETURN_OK(cmd->engine);
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[40] = {0};
        if (!cipMux_multiIp)
        {
            sprintf(aucBuffer, "+CLPORT:TCP:%d,UDP:%d", gClocalport[0].tcp_port, gClocalport[0].udp_port);
            atCmdRespInfoText(cmd->engine, aucBuffer);
        }
        else
        {
            for (int i = 0; i < MEMP_NUM_NETCONN; i++)
            {
                sprintf(aucBuffer, "+CLPORT:%d,TCP:%d,UDP:%d", i, gClocalport[i].tcp_port, gClocalport[i].udp_port);
                atCmdRespInfoText(cmd->engine, aucBuffer);
            }
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[40] = {0};
        if (!cipMux_multiIp)
            sprintf(aucBuffer, "+CLPORT: (\"TCP\",\"UDP\"),(0-65535)");
        else
            sprintf(aucBuffer, "+CLPORT: (0-7),(\"TCP\",\"UDP\"),(0-65535)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f93, "AT_TCPIP_CmdFunc_CLPORT, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CIPSERVER(atCommand_t *cmd)
{
    OSI_LOGI(0x10003f94, "AT+CIPSERVER ...gCipserver.channelid= %d", gCipserver.channelid);
    uint8_t nSim = atCmdGetSim(cmd->engine);
    if (AT_CMD_SET == cmd->type)
    {
        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);
        if (CFW_GetSimStatus(nSim) == CFW_SIM_ABSENT)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        if (cipMux_multiIp && (CIP_STATUS == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState || CIP_GPRSACT == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState))
        {
            bool paramok = true;
            uint8_t mode = 0;
            uint32_t port = 0;
            mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
            if (cmd->param_count == 1)
            {
                if (gCipserver.mode == CIPSERVER_OPEN && mode == CIPSERVER_CLOSE && paramok)
                {
                    if (gCipserver.engine != cmd->engine)
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    for (int i = 0; i < 8; i++)
                    {
                        stAT_Tcpip_Paras *tmp_tcpipParas = &(g_uCipContexts.nTcpipParas[i]);
                        if (tmp_tcpipParas->listening_socket == gCipserver.nSocketId)
                        {
                            int iRetValue = CFW_TcpipSocketClose(tmp_tcpipParas->uSocket);
                            if (iRetValue == ERR_SUCCESS)
                            {
                                tmp_tcpipParas->uState = CIP_CLOSING;
                                OSI_LOGI(0, "AT+CIPSERVER: close client %d success", tmp_tcpipParas->uSocket);
                            }
                        }
                    }
                    if (CFW_TcpipSocketClose(gCipserver.nSocketId) != ERR_SUCCESS)
                    {
                        OSI_LOGI(0x10003f95, "AT+CIPSERVER: CSW execute wrong");
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    else
                    {
                        gCipserver.mode = mode;
                        atCmdRespOK(cmd->engine);
                    }
                }
                else
                {
#if 0 //Delete by SUN Wei for bug 976191                 
                    if (gCipserver.mode == CIPSERVER_CLOSE && mode == CIPSERVER_CLOSE)
                    {
                        char aucBuffer[40] = {0};
                        sprintf(aucBuffer, "Already closed");
                        atCmdRespInfoText(cmd->engine, aucBuffer);
                    }
#endif
                    OSI_LOGI(0, "AT+CIPSERVER: error gCipserver.mode=%d", gCipserver.mode);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }
            else if (cmd->param_count >= 2)
            {
                port = atParamUintInRange(cmd->params[1], 1, 65535, &paramok);
                if (cmd->param_count == 3)
                    gCipserver.isIPV6 = atParamDefUintInRange(cmd->params[2], 0, 0, 1, &paramok);
                if (gCipserver.mode == CIPSERVER_CLOSE && mode == CIPSERVER_OPEN && paramok)
                {
                    if (bearerParas->nState == CIP_GPRSACT || bearerParas->nState == CIP_STATUS)
                    {
                        uint8_t uPDPState = 0;
                        if (CFW_GetGprsActState(bearerParas->nCid, &uPDPState, nSim) != ERR_SUCCESS || uPDPState == CFW_GPRS_DEACTIVED)
                        {
                            OSI_LOGI(0x0, "useExibearerParas->nState error, not actived\n");
                            bearerParas->nState = CIP_INITIAL;
                        }
                    }
                    if ((CIP_INITIAL == bearerParas->nState || gOutsidePdpCid != 0) && !useOutsidePdp(cmd))
                    {
                        gCipserver.port = port;
                        gCipserver.mode = CIPSERVER_OPENING;
                        if (cip_getCid(cmd) < 0 || set_userpwdapn(cmd, NULL, NULL, NULL) < 0 || do_attact(cmd) < 0)
                        {
                            gCipserver.mode = CIPSERVER_CLOSE;
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                        }
                        atCmdSetTimeoutHandler(cmd->engine, 60000, _timeoutabortTimeout);
                    }
                    else
                    {
                        if (bearerParas->nState != CIP_STATUS)
                            bearerParas->nState = CIP_GPRSACT;
                        gCipserver.port = port;
                        if (ERR_SUCCESS == AT_TCPIP_ServerStart(cmd->engine))
                        {
                            gCipserver.mode = mode;
                            gCipserver.channelid = 0xff;
                            atCmdRespInfoText(cmd->engine, "OK");
                            atCmdRespOKText(cmd->engine, "SERVER OK");
                        }
                        else
                        {
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                        }
                    }
                }
                else
                {
                    if (gCipserver.mode == CIPSERVER_OPEN && mode == CIPSERVER_OPEN)
                    {
                        char aucBuffer[40] = {0};
                        sprintf(aucBuffer, "Already opened");
                        atCmdRespInfoText(cmd->engine, aucBuffer);
                    }
                    if (CIPSERVER_OPENING == gCipserver.mode)
                        gCipserver.mode = CIPSERVER_CLOSE;
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char aucBuffer[40] = {0};
        if (gCipserver.mode == CIPSERVER_OPEN)
        {
            //sprintf(aucBuffer, "+CIPSERVER:%d,%ld,%d", gCipserver.mode, gCipserver.port, gCipserver.isIPV6);//cause not support ipv6
            sprintf(aucBuffer, "+CIPSERVER:%d,%ld", gCipserver.mode, gCipserver.port);
        }
        else
        {
            sprintf(aucBuffer, "+CIPSERVER:%d", gCipserver.mode);
        }
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char aucBuffer[256] = {0};
        sprintf(aucBuffer, "+CIPSERVER:(0-CLOSE SERVER,1-OPEN SERVER),(1-65535),(0-IPV4,1-IPV6)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003f96, "AT_TCPIP_CmdFunc_CIPSERVER, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
void AT_TCPIP_CmdFunc_CIPCSGP(atCommand_t *cmd)
{

    OSI_LOGI(0x1000529a, "AT+CIPCSGP >>");

    if (AT_CMD_SET == cmd->type)
    {
        uint8_t index = 0;
        uint8_t mode;
        uint8_t *apn = NULL;
        uint8_t *user = NULL;
        uint8_t *password = NULL;
        uint8_t param_count = cmd->param_count;
        bool paramRet = true;
        if (param_count < 2 || param_count > 4)
        {
            OSI_LOGI(0x10003f98, "AT+CIPCSGP invalid params count param_count:%d", param_count);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        // get mode
        mode = atParamUintInRange(cmd->params[index], 0, 1, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0x10003f99, "AT+CIPCSGP fail to get mode param");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        // don't support CSD
        if (mode == 0)
        {
            OSI_LOGI(0x10003f9a, "AT+CIPCSGP don't support CSD");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        }
        index++;
        // get apn
        apn = (uint8_t *)atParamStr(cmd->params[index], &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0x10003f9b, "AT+CIPCSGP fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (param_count > 2)
        {
            index++;
            // get user name
            user = (uint8_t *)atParamStr(cmd->params[index], &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0x10003f9b, "AT+CIPCSGP fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        if (param_count > 3)
        {
            index++;
            // get password
            password = (uint8_t *)atParamStr(cmd->params[index], &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0x10003f9b, "AT+CIPCSGP fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        cipCSGP_setGprs = mode;
        int apnlen = apn != NULL ? strlen((const char *)apn) : -1;
        int usrlen = user != NULL ? strlen((const char *)user) : -1;
        int pwlen = password != NULL ? strlen((const char *)password) : -1;

        if (apnlen > 50 || usrlen > 50 || pwlen > 50)
        {
            OSI_LOGI(0x10003f9c, "CIPCSGP apnlen=%d,srlen=%d,pwlen=%d", apnlen, usrlen, pwlen);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            if (apnlen == 0)
                memset(cipApn, 0, 51);
            else if (apnlen > 0)
            {
                memset(cipApn, 0, 51);
                memcpy(cipApn, apn, apnlen);
            }
            if (usrlen == 0)
                memset(cipUserName, 0, 51);
            else if (usrlen > 0)
            {
                memset(cipUserName, 0, 51);
                memcpy(cipUserName, user, usrlen);
            }
            if (pwlen == 0)
                memset(cipPassWord, 0, 51);
            else if (pwlen > 0)
            {
                memset(cipPassWord, 0, 51);
                memcpy(cipPassWord, password, pwlen);
            }

            RETURN_OK(cmd->engine);
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CIPCSGP: 0-CSD,DIALNUMBER,USER NAME,PASSWORD,RATE(0-3) -- NOT SUPPORT");
        atCmdRespInfoText(cmd->engine, "+CIPCSGP: 1-GPRS,APN,USER NAME,PASSWORD");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char tmp_buff[180] = {0};
        if (cipCSGP_setGprs == 1)
        {
            sprintf(tmp_buff, "+CIPCSGP:1,\"%s\",\"%s\",\"%s\"",
                    cipApn, cipUserName, cipPassWord);
        }
        else
        {
            sprintf(tmp_buff, "+CIPCSGP:<mode>,<apn>,<user name>,<password>"); //[<rate>]
        }
        atCmdRespInfoText(cmd->engine, tmp_buff);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0x10003f9d, "exe in %s(), command type not allowed", __func__);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CIPDPDP(atCommand_t *cmd)
{
    OSI_LOGXI(OSI_LOGPAR_S, 0x10003f9d, "exe in %s(), command type not allowed", __func__);
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}

void AT_TCPIP_CmdFunc_CIPMODE(atCommand_t *cmd)
{
    OSI_LOGI(0x1000529b, "AT+CIPMODE >>");

    if (AT_CMD_SET == cmd->type)
    {
        if (cipMux_multiIp)
        {
            OSI_LOGI(0x10003f9f, "AT+CIPMODE must be used in single connection mode");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[0]);
        if (tcpipParas->uState == CIP_PROCESSING || tcpipParas->uState == CIP_CONNECTED)
        {
            OSI_LOGI(0, "AT+CIPMODE can not be used when tcp connected");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (cmd->param_count != 1)
        {
            OSI_LOGI(0x10003fa0, "AT+CIPMODE invalid param_count:%d", cmd->param_count);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        bool paramRet = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramRet);
        if (!paramRet)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        uint8_t currentMode = cipMODE_transParent;
        cipMODE_transParent = mode;
        if (currentMode && mode == 0)
        {
            stop_transparent_mode(cmd->engine);
            atCmdRespOK(cmd->engine);
        }
        else
            RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CIPMODE: (0-NORMAL MODE, 1-TRANSPARENT MODE)");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char rspstr[32] = {0};
        sprintf(rspstr, "+CIPMODE:%d", cipMODE_transParent);
        atCmdRespInfoText(cmd->engine, rspstr);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0x10003f9d, "exe in %s(), command type not allowed", __func__);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CIPCCFG(atCommand_t *cmd)
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
    bool paramRet = true;

    OSI_LOGI(0x1000529c, "AT+CIPCCFG >> ");

    if (cipMux_multiIp)
    {
        OSI_LOGI(0x10003fa2, "AT+CIPCCFG must be used in single connection mode");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }

    if (AT_CMD_SET == cmd->type)
    {
        if (param_count < 4 || param_count > 7)
        {
            OSI_LOGI(0x10003fa3, "AT+CIPCCFG invalid param_count:%d", param_count);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        // get retry number
        retry_number = atParamUintInRange(cmd->params[index], 3, 8, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0x10003fa4, "AT+CIPCCFG fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        index++;
        // get wait time
        wait_time = atParamUintInRange(cmd->params[index], 1, 10, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0x10003fa4, "AT+CIPCCFG fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        index++;
        // get send size
        send_size = atParamUintInRange(cmd->params[index], 1, 1460, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0x10003fa4, "AT+CIPCCFG fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        index++;
        // get escape character flag
        escape = atParamUintInRange(cmd->params[index], 0, 1, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0x10003fa4, "AT+CIPCCFG fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        cipCCFG_esc = escape;
        cipCCFG_WaitTm = wait_time;
        cipCCFG_NmRetry = retry_number;
        cipCCFG_SendSz = send_size;

        if (param_count > 4)
        {
            index++;
            // get rxmode
            rxmode = atParamUintInRange(cmd->params[index], 0, 1, &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0x10003fa4, "AT+CIPCCFG fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            cipCCFG_Rxmode = rxmode;

            index++;
            // get rxsize
            rxsize = atParamUintInRange(cmd->params[index], 50, 1460, &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0x10003fa4, "AT+CIPCCFG fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            cipCCFG_RxSize = rxsize;

            index++;
            // get rxtimer
            rxtimer = atParamUintInRange(cmd->params[index], 20, 1000, &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0x10003fa4, "AT+CIPCCFG fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            cipCCFG_Rxtimer = rxtimer;
        }

        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CIPCCFG: (NmRetry:3-8),(WaitTm:1-10),(SendSz:1-1460),(esc:0,1),(Rxmode:0,1),(RxSize:50-1460),(Rxtimer:20-1000)");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char rspstr[64] = {0};
        sprintf(rspstr, "+CIPCCFG:%d,%d,%d,%d,%d,%d,%d", cipCCFG_NmRetry, cipCCFG_WaitTm,
                cipCCFG_SendSz, cipCCFG_esc, cipCCFG_Rxmode, cipCCFG_RxSize, cipCCFG_Rxtimer);
        atCmdRespInfoText(cmd->engine, rspstr);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0x10003f9d, "exe in %s(), command type not allowed", __func__);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CIPSHOWTP(atCommand_t *cmd)
{
    uint8_t index = 0;
    uint8_t mode;
    bool paramRet = true;

    OSI_LOGI(0x1000529d, "AT+CIPSHOWTP >>");

    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        // get mode
        mode = atParamUintInRange(cmd->params[index], 0, 1, &paramRet);
        if (!paramRet)
        {
            OSI_LOGI(0x10003fa6, "AT+CIPSHOWTP fail to get param %d", index);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        cipSHOWTP_dispTP = mode;
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+CIPSHOWTP: (0,1)");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char rspstr[32] = {0};
        sprintf(rspstr, "+CIPSHOWTP:%d", cipSHOWTP_dispTP);
        atCmdRespInfoText(cmd->engine, rspstr);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0x10003f9d, "exe in %s(), command type not allowed", __func__);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CIPUDPMODE(atCommand_t *cmd)
{
    uint8_t param_count = cmd->param_count;
    uint8_t index = 0;
    uint8_t mode;
    uint8_t *ipaddr = NULL;
    uint8_t iplen;
    uint16_t port;
    bool paramRet = true;

    OSI_LOGI(0x1000529e, "AT+CIPUDPMODE >>");

    if (AT_CMD_SET == cmd->type)
    {
        if (cipMux_multiIp)
        {
            if ((param_count != 2 && param_count != 4))
            {
                OSI_LOGI(0x10003fa8, "AT+CIPUDPMODE invalid param_count:%d", param_count);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            uint8_t uMuxIndex = 0;
            // get mode
            uMuxIndex = atParamUintInRange(cmd->params[index], 0, 7, &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0x10003fa9, "AT+CIPUDPMODE fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            // get mode
            index++;
            mode = atParamUintInRange(cmd->params[index], 0, 2, &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0x10003fa9, "AT+CIPUDPMODE fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if ((mode == 2 && param_count != 4) || ((mode == 1 || mode == 0) && (param_count != 2)))
            {
                OSI_LOGI(0x10003faa, "AT+CIPUDPMODE dismatched mode:%d and param_count:%d", mode, param_count);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            if (mode == 2)
            {
                index++;
                // get ip address
                ipaddr = (uint8_t *)atParamStr(cmd->params[index], &paramRet);
                if (!paramRet)
                {
                    OSI_LOGI(0x10003fa9, "AT+CIPUDPMODE fail to get param %d", index);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                iplen = strlen((const char *)ipaddr);
                if (iplen >= AT_TCPIP_HOSTLEN)
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (IPADDR_NONE == CFW_TcpipInetAddr((const char *)ipaddr))
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0x10003fab, "AT+CIPUDPMODE invalid ipaddr param:%s", ipaddr);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                index++;
                // get port
                port = atParamUintInRange(cmd->params[index], 1, 65535, &paramRet);
                if (!paramRet)
                {
                    OSI_LOGI(0x10003fa9, "AT+CIPUDPMODE fail to get param %d", index);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                update_udp_remote_socket(uMuxIndex, ipaddr, iplen, port);
            }
            else
            {
                set_udpmode(uMuxIndex, mode);
            }
            RETURN_OK(cmd->engine);
        }
        else
        {
            if ((param_count != 1 && param_count != 3))
            {
                OSI_LOGI(0x10003fa8, "AT+CIPUDPMODE invalid param_count:%d", param_count);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            // get mode
            mode = atParamUintInRange(cmd->params[index], 0, 2, &paramRet);
            if (!paramRet)
            {
                OSI_LOGI(0x10003fa9, "AT+CIPUDPMODE fail to get param %d", index);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if ((mode == 2 && param_count != 3) || ((mode == 1 || mode == 0) && (param_count != 1)))
            {
                OSI_LOGI(0x10003faa, "AT+CIPUDPMODE dismatched mode:%d and param_count:%d", mode, param_count);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            if (mode == 2)
            {
                index++;
                // get ip address
                ipaddr = (uint8_t *)atParamStr(cmd->params[index], &paramRet);
                if (!paramRet)
                {
                    OSI_LOGI(0x10003fa9, "AT+CIPUDPMODE fail to get param %d", index);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                iplen = strlen((const char *)ipaddr);
                if (IPADDR_NONE == CFW_TcpipInetAddr((const char *)ipaddr))
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0x10003fab, "AT+CIPUDPMODE invalid ipaddr param:%s", ipaddr);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                index++;
                // get port
                port = atParamUintInRange(cmd->params[index], 1, 65535, &paramRet);
                if (!paramRet)
                {
                    OSI_LOGI(0x10003fa9, "AT+CIPUDPMODE fail to get param %d", index);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                update_udp_remote_socket(DEF_TCPIP, ipaddr, iplen, port);
            }
            else
            {
                set_udpmode(DEF_TCPIP, mode);
            }
            RETURN_OK(cmd->engine);
        }
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        if (cipMux_multiIp)
            atCmdRespInfoText(cmd->engine, "+CIPUDPMODE: (0-7),(0-2),(\"(0-255).(0-255).(0-255).(0-255)\"),(1-65535)");
        else
            atCmdRespInfoText(cmd->engine, "+CIPUDPMODE: (0-2),(\"(0-255).(0-255).(0-255).(0-255)\"),(1-65535)");

        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char tmp_buff[350] = {0};
        if (cipMux_multiIp)
        {
            for (int i = 0; i < MEMP_NUM_NETCONN; i++)
            {
                if (g_uCipContexts.nTcpipParas[i].uPort == 0)
                {
                    sprintf(tmp_buff, "+CIPUDPMODE:%d,%d,%s,", i,
                            g_uCipContexts.nTcpipParas[i].udpmode,
                            &(g_uCipContexts.nTcpipParas[i].uaIPAddress[0]));
                    atCmdRespInfoText(cmd->engine, tmp_buff);
                }
                else
                {
                    sprintf(tmp_buff, "+CIPUDPMODE:%d,%d,%s,%d", i,
                            g_uCipContexts.nTcpipParas[i].udpmode,
                            &(g_uCipContexts.nTcpipParas[i].uaIPAddress[0]),
                            g_uCipContexts.nTcpipParas[i].uPort);
                    atCmdRespInfoText(cmd->engine, tmp_buff);
                }
            }
        }
        else
        {
            if (g_uCipContexts.nTcpipParas[0].uPort == 0)
            {
                sprintf(tmp_buff, "+CIPUDPMODE:%d,%s,",
                        g_uCipContexts.nTcpipParas[0].udpmode,
                        &(g_uCipContexts.nTcpipParas[0].uaIPAddress[0]));
                atCmdRespInfoText(cmd->engine, tmp_buff);
            }
            else
            {
                sprintf(tmp_buff, "+CIPUDPMODE:%d,%s,%d",
                        g_uCipContexts.nTcpipParas[0].udpmode,
                        &(g_uCipContexts.nTcpipParas[0].uaIPAddress[0]),
                        g_uCipContexts.nTcpipParas[0].uPort);
                atCmdRespInfoText(cmd->engine, tmp_buff);
            }
        }
        atCmdRespOK(cmd->engine);
    }
    else
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0x10003f9d, "exe in %s(), command type not allowed", __func__);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:        AT_TCPIP_Connect
* Description: TCP/IP module TCP/IP establish connection process.
* Parameter:   none
* Return:      void
* Remark:      May be call either during command process or in asynchronous process.
* Author:      YangYang
* Data:        2008-5-16
******************************************************************************/
int AT_TCPIP_Connect(atCmdEngine_t *engine, uint8_t nMuxIndex)
{
    int iResult;
    int iResultBind;
    uint8_t nSim = atCmdGetSim(engine);
    union sockaddr_aligned nDestAddr;
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    tcpipParas->uState = CIP_PROCESSING;
    ip_addr_t nIpAddr;
    //IP4_ADDR(&nIpAddr, 111, 205, 140, 139);
    TCPIP_DNS_PARAM *dnsparam = malloc(sizeof(TCPIP_DNS_PARAM));
    dnsparam->engine = engine;
    dnsparam->nMuxIndex = nMuxIndex;
    iResult = CFW_GethostbynameEX(tcpipParas->uaIPAddress, &nIpAddr, bearerParas->nCid, nSim, tcpip_dnsReq_callback, dnsparam);
    //iResult = RESOLV_COMPLETE;
    if (iResult == RESOLV_QUERY_INVALID)
    {
        OSI_LOGI(0x10003fac, "tcpip connect gethost by name error");
        tcpipParas->uState = CIP_CLOSED;
        tcpipParas->engine = NULL;
        free(dnsparam);
        return -1;
    }
    else if (iResult == RESOLV_COMPLETE)
    {
        OSI_LOGI(0x10003fad, "AT_TCPIP_Connect(), CFW_GethostbynameEX() RESOLV_COMPLETE\n");
        free(dnsparam);
        tcpipParas->uSocket = INVALID_SOCKET;
        struct netif *netif = NULL;
        struct ifreq iface;
        memset(&iface, 0, sizeof(struct ifreq));
        if (bearerParas->nCid == 0x11)
            netif = getEtherNetIf(bearerParas->nCid);
        else
            netif = getGprsNetIf(nSim, bearerParas->nCid);
        if (netif == NULL)
        {
            tcpipParas->uState = CIP_CLOSED;
            tcpipParas->engine = NULL;
            return -1;
        }

        iface.ifr_name[0] = netif->name[0];
        iface.ifr_name[1] = netif->name[1];
        strcat(iface.ifr_name, (const char *)i8tostring(netif->num));
        OSI_LOGXI(OSI_LOGPAR_S, 0x10003faf, "AT_TCPIP_Connect() netif:%s\n", iface.ifr_name);

#if LWIP_IPV4
        if (IP_IS_V4(&(nIpAddr)))
        {
            struct sockaddr_in *to4 = (struct sockaddr_in *)&(nDestAddr);
            to4->sin_len = sizeof(struct sockaddr_in);
            to4->sin_family = AF_INET;
            to4->sin_port = htons(tcpipParas->uPort);
            inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&(nIpAddr)));
            tcpip_callback_param_t *tcpparam = malloc(sizeof(tcpip_callback_param_t));
            tcpparam->engine = engine;
            tcpparam->nMuxIndex = nMuxIndex;

            iResult = CFW_TcpipSocketEX(AF_INET, tcpipParas->uType, tcpipParas->uProtocol, tcpip_rsp, (uint32_t)tcpparam);
            if (SOCKET_ERROR == iResult)
            {
                OSI_LOGI(0x10003fae, "AT_TCPIP_Connect(), CFW_TcpipSocket() failed\n");
                free(tcpparam);
                tcpipParas->uState = CIP_CLOSED;
                tcpipParas->engine = NULL;
                return iResult;
            }
            int n = 1;
            CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_REUSEADDR, (void *)&n, sizeof(n));
            ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(netif);

            struct sockaddr_in stLocalAddr = {0};
            stLocalAddr.sin_len = sizeof(struct sockaddr_in);
            stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
            if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_UDP)
                stLocalAddr.sin_port = htons(gClocalport[nMuxIndex].udp_port);
            else
                stLocalAddr.sin_port = htons(gClocalport[nMuxIndex].tcp_port);
            stLocalAddr.sin_addr.s_addr = ip_addr->addr;

            iResultBind = CFW_TcpipSocketBind(iResult, (CFW_TCPIP_SOCKET_ADDR *)&stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
            if (SOCKET_ERROR == iResultBind)
            {
                OSI_LOGI(0x10003fb0, "AT_TCPIP_Connect(), CFW_TcpipSocketBind() failed\n");
                int iRetValue;
                iRetValue = CFW_TcpipSocketClose(iResult);
                if (iRetValue == ERR_SUCCESS)
                {
                    OSI_LOGI(0, "AT_TCPIP_Connect(): close success");
                }
                else
                {
                    OSI_LOGI(0, "AT_TCPIP_Connect(): CSW execute wrong");
                }
                return iResultBind;
            }
        }
#endif

#if LWIP_IPV6
        if (IP_IS_V6(&(nIpAddr)))
        {
            struct sockaddr_in6 *to6 = (struct sockaddr_in6 *)&(nDestAddr);
            to6->sin6_len = sizeof(struct sockaddr_in6);
            to6->sin6_family = AF_INET6;
            to6->sin6_port = htons(tcpipParas->uPort);
            inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(&(nIpAddr)));
            tcpip_callback_param_t *tcpparam = malloc(sizeof(tcpip_callback_param_t));
            tcpparam->engine = engine;
            tcpparam->nMuxIndex = nMuxIndex;

            iResult = CFW_TcpipSocketEX(AF_INET6, tcpipParas->uType, tcpipParas->uProtocol, tcpip_rsp, (uint32_t)tcpparam);
            if (SOCKET_ERROR == iResult)
            {
                OSI_LOGI(0x10003fae, "AT_TCPIP_Connect(), CFW_TcpipSocket() failed\n");
                free(tcpparam);
                tcpipParas->uState = CIP_CLOSED;
                tcpipParas->engine = NULL;
                return iResult;
            }
            int n = 1;
            CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_REUSEADDR, (void *)&n, sizeof(n));
            ip6_addr_t *ip_addr = (ip6_addr_t *)netif_ip6_addr(netif, 0);
            ip6_addr_t *ip_addr_1 = (ip6_addr_t *)netif_ip6_addr(netif, 1);
            if (ip6_addr_isany_val(*(ip_addr_1)) != true)
            {
                ip_addr = ip_addr_1;
            }
            struct sockaddr_in6 stLocalAddr = {0};
            stLocalAddr.sin6_len = sizeof(struct sockaddr_in6);
            stLocalAddr.sin6_family = AF_INET6;
            if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_UDP)
                stLocalAddr.sin6_port = htons(gClocalport[nMuxIndex].udp_port);
            else
                stLocalAddr.sin6_port = htons(gClocalport[nMuxIndex].tcp_port);

            inet6_addr_from_ip6addr(&stLocalAddr.sin6_addr, ip_addr);
            iResultBind = CFW_TcpipSocketBind(iResult, (CFW_TCPIP_SOCKET_ADDR *)&stLocalAddr, sizeof(struct sockaddr_in6));
            if (SOCKET_ERROR == iResultBind)
            {
                CFW_TcpipSocketClose(tcpipParas->uSocket);
                OSI_LOGI(0x10003fb0, "AT_TCPIP_Connect(), CFW_TcpipSocketBind() failed\n");
                return iResultBind;
            }
        }
#endif

        char *pcIpAddr = ipaddr_ntoa(&nIpAddr);
        strncpy(tcpipParas->uaIPAddress, pcIpAddr, AT_TCPIP_ADDRLEN);
        tcpipParas->uSocket = iResult;
        tcpipParas->engine = engine;

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

        if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_TCP || tcpipParas->udpmode == 0)
        {
            iResult = CFW_TcpipSocketConnect(tcpipParas->uSocket, (CFW_TCPIP_SOCKET_ADDR *)&nDestAddr, sizeof(nDestAddr));
            if (SOCKET_ERROR == iResult)
            {
                OSI_LOGI(0x10003fb2, "AT_TCPIP_Connect, CFW_TcpipSocketConnect() failed\n");
                CFW_TcpipSocketClose(tcpipParas->uSocket);
                return iResult;
            }
            atCmdSetTimeoutHandler(engine, 60000, _timeoutabortTimeout);
        }
        else
        {
            tcpip_connect_done(engine, nMuxIndex);
        }
    }
    else if (iResult == RESOLV_QUERY_QUEUED)
    {
        //atCmdSetTimeoutHandler(engine, 60000, _timeoutabortTimeout);
        return ERR_SUCCESS;
    }

    return ERR_SUCCESS;
}
int AT_TCPIP_ServerStart(atCmdEngine_t *engine)
{
    int32_t iResult;
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    struct netif *netif = NULL;
    struct ifreq iface;
    memset(&iface, 0, sizeof(struct ifreq));
    uint8_t nSim = atCmdGetSim(engine);

    if (bearerParas->nCid == 0x11)
        netif = getEtherNetIf(bearerParas->nCid);
    else
        netif = getGprsNetIf(nSim, bearerParas->nCid);
    if (netif == NULL)
    {
        return -1;
    }

    iface.ifr_name[0] = netif->name[0];
    iface.ifr_name[1] = netif->name[1];
    strcat(iface.ifr_name, (char *)i8tostring(netif->num));
    OSI_LOGI(0, "AT+CIPSERVER netif: %s", iface.ifr_name);
    tcpip_callback_param_t *tcpparam = malloc(sizeof(tcpip_callback_param_t));
    tcpparam->engine = engine;
    tcpparam->nMuxIndex = 0xff;

    gCipserver.engine = engine;
    if (gCipserver.isIPV6 == 1)
    {

#if LWIP_IPV6

        iResult = CFW_TcpipSocketEX(AF_INET6, CFW_TCPIP_SOCK_STREAM, CFW_TCPIP_IPPROTO_TCP, tcpip_rsp, (uint32_t)tcpparam);
        if (SOCKET_ERROR == iResult)
        {
            OSI_LOGI(0, "AT_TCPIP_ServerStart(), CFW_TcpipSocket() failed\n");
            free(tcpparam);
            return SOCKET_ERROR;
        }
        int n = 1;
        CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_REUSEADDR, (void *)&n, sizeof(n));
        ip6_addr_t *ip_addr = (ip6_addr_t *)netif_ip6_addr(netif, 0);
        ip6_addr_t *ip_addr_1 = (ip6_addr_t *)netif_ip6_addr(netif, 1);
        if (ip6_addr_isany_val(*(ip_addr_1)) != true)
        {
            ip_addr = ip_addr_1;
        }
        struct sockaddr_in6 stLocalAddr = {
            0,
        };
        stLocalAddr.sin6_len = sizeof(struct sockaddr_in6);
        stLocalAddr.sin6_family = AF_INET6;
        stLocalAddr.sin6_port = htons(gCipserver.port);

        //inet6_addr_from_ip6addr(&stLocalAddr.sin6_addr, ip_addr);
        OSI_LOGI(0, "AT_TCPIP_ServerStart ip_addr=%p", ip_addr);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "AT_TCPIP_ServerStart addr=%s", inet6_ntoa(stLocalAddr.sin6_addr));
        OSI_LOGI(0, "AT_TCPIP_ServerStart port:%d", htons(stLocalAddr.sin6_port));
        if (SOCKET_ERROR == CFW_TcpipSocketBind(iResult, (CFW_TCPIP_SOCKET_ADDR *)&stLocalAddr, sizeof(struct sockaddr_in6)))
        {
            OSI_LOGI(0, "AT_TCPIP_ServerStart,CFW_TcpipSocketBind() failed\n");
            CFW_TcpipSocketClose(iResult);
            return -1;
        }
#else
        OSI_LOGI(0, "AT_TCPIP_ServerStart, LWIP_IPV6 is defined bug isIPV4\n");
        return -1;
#endif
    }
    else
    {

#if LWIP_IPV4

        iResult = CFW_TcpipSocketEX(AF_INET, CFW_TCPIP_SOCK_STREAM, CFW_TCPIP_IPPROTO_TCP, tcpip_rsp, (uint32_t)tcpparam);
        if (SOCKET_ERROR == iResult)
        {
            free(tcpparam);
            OSI_LOGI(0, "AT_TCPIP_ServerStart(), CFW_TcpipSocket() failed \n");
            return SOCKET_ERROR;
        }
        int n = 1;
        CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_REUSEADDR, (void *)&n, sizeof(n));
        ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(netif);

        struct sockaddr_in stLocalAddr = {
            0,
        };
        stLocalAddr.sin_len = sizeof(struct sockaddr_in);
        stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
        stLocalAddr.sin_port = htons(gCipserver.port);

        inet_addr_from_ip4addr(&stLocalAddr.sin_addr, ip_addr);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "AT_TCPIP_ServerStart addr=%s", inet_ntoa(stLocalAddr.sin_addr));
        OSI_LOGI(0, "AT_TCPIP_ServerStart addr=%s", inet_ntoa(stLocalAddr.sin_addr));
        OSI_LOGI(0, "AT_TCPIP_ServerStart port:%d", htons(stLocalAddr.sin_port));
        if (SOCKET_ERROR == CFW_TcpipSocketBind(iResult, (CFW_TCPIP_SOCKET_ADDR *)&stLocalAddr, sizeof(struct sockaddr_in)))
        {

            OSI_LOGI(0, "AT_TCPIP_ServerStart(), CFW_TcpipSocketBind() failed\n");
            CFW_TcpipSocketClose(iResult);
            return SOCKET_ERROR;
        }
#else
        OSI_LOGI(0, "AT_TCPIP_ServerStart, LWIP_IPV6 is defined bug isIPV4\n");
        return -1;
#endif
    }

    gCipserver.nSocketId = iResult;
    if (g_keepalive == 1)
    {
        CFW_TcpipSocketSetsockopt(gCipserver.nSocketId, SOL_SOCKET, SO_KEEPALIVE, (void *)&g_keepalive, sizeof(g_keepalive));
        CFW_TcpipSocketSetsockopt(gCipserver.nSocketId, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&g_keepidle, sizeof(g_keepidle));
        CFW_TcpipSocketSetsockopt(gCipserver.nSocketId, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&g_keepinterval, sizeof(g_keepinterval));
        CFW_TcpipSocketSetsockopt(gCipserver.nSocketId, IPPROTO_TCP, TCP_KEEPCNT, (void *)&g_keepcount, sizeof(g_keepcount));
    }
    else
    {
        CFW_TcpipSocketSetsockopt(gCipserver.nSocketId, SOL_SOCKET, SO_KEEPALIVE, (void *)&g_keepalive, sizeof(g_keepalive));
    }

    if (CFW_TcpipSocketListen(gCipserver.nSocketId, 0) != ERR_SUCCESS)
    {
        OSI_LOGI(0, "CFW_TcpipSocketListen() failed\n");
        return -1;
    }
    return ERR_SUCCESS;
}

/*****************************************************************************
* Name          :   AT_TCPIP_CmdFunc_CDNSGIP
* Description       :   T+CDNSCFG command
* Parameter     :   atCommand_t *cmd
* Return            :   void
* Remark        :   n/a
* Author            :   wulc
* Data          :     20013-1-31
******************************************************************************/
static void dnsReq_callback(void *param)
{
    osiEvent_t *ev = (osiEvent_t *)param;
    atCmdEngine_t *engine = (atCmdEngine_t *)ev->param3;
    if (ev->id == EV_CFW_DNS_RESOLV_SUC_IND)
    {
        char PromptBuff[130] = {0};
        ip_addr_t *ipaddrs = (ip_addr_t *)ev->param1;
        char acHostname[101] = {0};
        char tmpString[50] = {0};
        char *pcIpAddr;
        memcpy(acHostname, (const void *)(ev->param2), sizeof(acHostname));
        atCmdRespOK(engine);
        sprintf(PromptBuff, "+CDNSGIP: 1,\"%s\"", acHostname);
        for (uint8_t j = 0; j < DNS_MAX_ADDR_ANSWER; j++)
        {
            ip_addr_t *ipaddr = &ipaddrs[j];
            if (!ip_addr_isany(ipaddr))
            {
                pcIpAddr = ipaddr_ntoa(ipaddr);
                sprintf(tmpString, ",\"%s\"", pcIpAddr);
                strcat(PromptBuff, tmpString);
            }
        }
        atCmdRespUrcText(engine, PromptBuff);
    }
    else if (ev->id == EV_CFW_DNS_RESOLV_ERR_IND)
    {
        uint32_t ulRet = 8; //DNS common error
        char PromptBuff[20] = {0};
        //atCmdRespError(engine);
        sprintf(PromptBuff, "+CDNSGIP: 0,%ld", ulRet);
        atCmdRespUrcText(engine, PromptBuff);
        atCmdRespError(engine);
    }
    free(ev);
}

void AT_TCPIP_CmdFunc_CDNSGIP(atCommand_t *cmd)
{
    OSI_LOGI(0x1000529f, "AT+CDNSGIP ... ...");
    atCmdEngine_t *cmdEngine = cmd->engine;
    uint32_t ulRet = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        bool paramok = true;
        //uint32_t ulIpAddr = 0;
        const char *acHostname = atParamStr(cmd->params[0], &paramok);

        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        ip_addr_t nIpAddr[DNS_MAX_ADDR_ANSWER];
        memset((void *)(&nIpAddr), 0, sizeof(ip_addr_t) * DNS_MAX_ADDR_ANSWER);
        OSI_LOGI(0x10003fb8, "nIpAddr %x", &nIpAddr);
        OSI_LOGI(0x10003fb9, "nIpAddr[0] %x", &nIpAddr[0]);
        for (uint8_t j = 0; j < DNS_MAX_ADDR_ANSWER; j++)
        {
            ip_addr_t *ipaddr = &nIpAddr[j];
            OSI_LOGI(0x10003fba, "ipaddr %x", ipaddr);
        }

        if (gCipBearer != BEARER_WIFI && gOutsidePdpCid == 0 && useOutsidePdp(cmd) == false)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        ulRet = CFW_GetallhostbynameEX((char *)acHostname, &nIpAddr[0], g_uCipContexts.nBearerParas[gCipBearer].nCid, nSim, dnsReq_callback, cmd->engine);
        OSI_LOGI(0x10003fbb, "CDNSGIP CFW_Gethostbyname ret %d\n", ulRet);

        if (ulRet == RESOLV_COMPLETE)
        {
            char *pcIpAddr = NULL;
            char PromptBuff[130] = {0};
            char tmpString[50] = {0};
            sprintf(PromptBuff, "+CDNSGIP: 1,\"%s\"", acHostname);
            atCmdRespOK(cmd->engine);
            for (uint8_t j = 0; j < DNS_MAX_ADDR_ANSWER; j++)
            {
                ip_addr_t *ipaddr = &nIpAddr[j];
                if (!ip_addr_isany(ipaddr))
                {
                    pcIpAddr = ipaddr_ntoa(ipaddr);
                    sprintf(tmpString, ",\"%s\"", pcIpAddr);
                    strcat(PromptBuff, tmpString);
                }
            }
            atCmdRespUrcText(cmdEngine, PromptBuff);
        }
        else if (ulRet == RESOLV_QUERY_QUEUED)
        {
            atCmdSetTimeoutHandler(cmd->engine, 60000, _timeoutabortTimeout);
        }
        else
        {
            char PromptBuff[20] = {0};
            uint32_t retErrno = 8; // DNS common error
            atCmdRespInfoText(cmd->engine, "ERROR");
            sprintf(PromptBuff, "+CDNSGIP: 0,%ld", retErrno);
            atCmdRespErrorText(cmdEngine, PromptBuff);
        }
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003fbc, "AT_TCPIP_CmdFunc_CDNSGIP, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

//set tcp keepalive
void AT_TCPIP_CmdFunc_CIPTKA(atCommand_t *cmd)
{
    char aucBuffer[256] = {0};

    OSI_LOGI(0x100052a0, "AT+CIPTKA ... ...");

    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count < 1 || cmd->param_count > 4)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        bool paramok = true;
        int keepalive = g_keepalive;
        int keepidle = g_keepidle;
        int keepinterval = g_keepinterval;
        int keepcount = g_keepcount;

        keepalive = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (cmd->param_count > 1)
            keepidle = atParamUintInRange(cmd->params[1], 30, 7200, &paramok);
        if (cmd->param_count > 2)
            keepinterval = atParamUintInRange(cmd->params[2], 30, 600, &paramok);
        if (cmd->param_count > 3)
            keepcount = atParamUintInRange(cmd->params[3], 1, 9, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        g_keepalive = keepalive;
        g_keepidle = keepidle;
        g_keepinterval = keepinterval;
        g_keepcount = keepcount;
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        sprintf(aucBuffer, "+CIPTKA: <%d>,<%d>,<%d>,<%d>", g_keepalive, g_keepidle, g_keepinterval, g_keepcount);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        sprintf(aucBuffer, "+CIPTKA: (0-Disable,1-Enable),(30-7200),(30-600),(1-9)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003fbe, "AT_TCPIP_CmdFunc_CIPTKA, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_TCPIP_CmdFunc_CIPRDTIMER(atCommand_t *cmd)
{
    char aucBuffer[128] = {0};

    OSI_LOGI(0x100052a1, "AT+CIPRDTIMER ... ...");

    if (cmd->type == AT_CMD_SET)
    {
        if (cmd->param_count != 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        bool paramok = true;
        int rdsigtimer = 0;
        int rdmuxtimer = 0;

        rdsigtimer = atParamUintInRange(cmd->params[0], 100, 4000, &paramok);
        rdmuxtimer = atParamUintInRange(cmd->params[1], 100, 7000, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        g_rdsigtimer = rdsigtimer;
        g_rdmuxtimer = rdmuxtimer;
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        sprintf(aucBuffer, "+CIPRDTIMER: <%d>,<%d>", g_rdsigtimer, g_rdmuxtimer);
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        sprintf(aucBuffer, "+CIPRDTIMER: (100-4000),(100-7000)");
        atCmdRespInfoText(cmd->engine, aucBuffer);
        RETURN_OK(cmd->engine);
    }
    else
    {
        OSI_LOGI(0x10003fc0, "AT_TCPIP_CmdFunc_CIPRDTIMER, command type:%d not allowed", cmd->type);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

#endif
