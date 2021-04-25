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
#include "atr_config.h"
#include "hal_config.h"

#ifdef CONFIG_AT_MYFTP_SUPPORT

/*
//#include <csw.h>
//#include "sxs_io.h"
#include "string.h"
#include "at_cmd_myftp.h"
#include "at_cmd_gprs.h"
#include "at_cmd_tcpip.h"
#include "at_module.h"
#include "sockets.h"
#include "at_utils.h"
#include "at_dispatch.h"
#include "at_data_engine.h"*/

#include "osi_api.h"
#include "osi_log.h"
#include "cfw.h"
#include "cfw_errorcode.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "lwipopts.h"
#include "sockets.h"
#include "at_engine.h"
//#include "at_engine_imp.h"
#include "at_command.h"
#include "at_response.h"
#include "at_cmd_myftp.h"

#define FTP_SPRINTF(s1, s2, ...) sprintf((char *)s1, (const char *)s2, ##__VA_ARGS__)
#define FTP_STRLEN(s) strlen((const char *)s)
#define FTP_STRSTR(s1, s2) strstr((const char *)s1, (const char *)s2)
#define FTP_STRCPY(des, src) strcpy((char *)des, (const char *)src)
#define FTP_STRCAT(des, src) strcat((char *)des, (const char *)src)
#define FTP_STRNCPY(des, src, len) strncpy((char *)des, (const char *)src, len)
#define FTP_STRNCMP(s1, s2, len) strncmp((const char *)s1, (const char *)s2, len)
#define FTP_ATOI(s) atoi((const char *)s)

#define FTP_BUFFER_LEN (1024 * 10)
static void ftp_put_disconnect();
static int32_t api_FTPLogout();
static int32_t api_FTPLogin(ip_addr_t *serIp, uint16_t serPort, uint8_t *user, uint8_t *pswd);
static void ftp_commandComplete(uint8_t succ, uint8_t *data);
static void ftp_get_disconnect();
static void ftp_TransferStart();
static int ftp_UartRecv(void *param, uint8_t *data, unsigned length);
static void ftp_CommandNotResponse(atCommand_t *cmd);
static void ftp_setFTPIdle();

extern uint32_t g_mynetUrc;
static bool g_myftpUrc = false;

static CFW_FTP_CTX_T gFTPCtx;
static struct FTP_Param *gFTP_Param = NULL;

typedef struct
{
    uint8_t data[FTP_SEND_MAX_SIZE];
    uint8_t *cur;
    uint8_t *end;
} FTP_TXBUFF_T;

static CFW_FTP_CTX_T gFTPCtx = {
    .connect = false,
    .ftpState = FTP_LOGOUT};

static bool gFtpWaitUart = false;

static void ftp_data_mode_start()
{
    //atCmdRespOutputPrompt(gFTPCtx.FTPengine);
    gFtpWaitUart = true;
    atCmdSetBypassMode(gFTPCtx.FTPengine, (atCmdBypassCB_t)ftp_UartRecv, NULL);
}

static void ftp_data_mode_stop()
{
    atCmdSetLineMode(gFTPCtx.FTPengine);
}

#define FTP_WAITING_EVENT_MAX 20

struct FTPWaitingEvent
{
    uint32_t nEvent;
    uint8_t nSim;
    uint8_t nDLCI;
    uint8_t enable;
};

struct FTPWaitingEvent gFTP_WaitingEvent[FTP_WAITING_EVENT_MAX];

static void FTP_AddWaitingEvent(uint32_t event, uint8_t sim, uint8_t dlci)
{
    for (int i = 0; i < FTP_WAITING_EVENT_MAX; i++)
    {
        if (gFTP_WaitingEvent[i].enable == 0)
        {
            gFTP_WaitingEvent[i].nEvent = event;
            gFTP_WaitingEvent[i].nSim = sim;
            gFTP_WaitingEvent[i].nDLCI = dlci;
            gFTP_WaitingEvent[i].enable = 1;
            OSI_LOGI(0, "FTP# FTP add waiting to #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            return;
        }
    }
    OSI_LOGI(0, "FTP# FTP fail add waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return;
}

static bool FTP_isWaitingEvent(uint32_t event, uint8_t sim, uint8_t dlci)
{
    for (int i = 0; i < FTP_WAITING_EVENT_MAX; i++)
    {
        if ((gFTP_WaitingEvent[i].enable) &&
            (gFTP_WaitingEvent[i].nEvent == event) &&
            (gFTP_WaitingEvent[i].nSim == sim))
        {
            OSI_LOGI(0, "FTP# FTP is waiting #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            return true;
        }
    }
    OSI_LOGI(0, "FTP# FTP not waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return false;
}

static void FTP_DelWaitingEvent(uint32_t event, uint8_t sim, uint8_t dlci)
{
    for (int i = 0; i < FTP_WAITING_EVENT_MAX; i++)
    {
        if ((gFTP_WaitingEvent[i].enable) &&
            (gFTP_WaitingEvent[i].nEvent == event) &&
            (gFTP_WaitingEvent[i].nSim == sim))
        {
            OSI_LOGI(0, "FTP# FTP delete waiting #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            gFTP_WaitingEvent[i].nEvent = 0;
            gFTP_WaitingEvent[i].nSim = 0;
            gFTP_WaitingEvent[i].nDLCI = 0;
            gFTP_WaitingEvent[i].enable = 0;
            return;
        }
    }
    OSI_LOGI(0, "FTP# FTP fail delete waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return;
}

static void FTP_ClearWaitingEvent()
{
    for (int i = 0; i < FTP_WAITING_EVENT_MAX; i++)
    {
        gFTP_WaitingEvent[i].nEvent = 0;
        gFTP_WaitingEvent[i].nSim = 0;
        gFTP_WaitingEvent[i].nDLCI = 0;
        gFTP_WaitingEvent[i].enable = 0;
    }
    OSI_LOGI(0, "FTP# FTP clear all waiting event");
    return;
}

static void ftp_put_tx_work_start(FTP_TXBUFFER_T *tx)
{
    tx->tx_work = true;
}

static void ftp_put_tx_work_stop(FTP_TXBUFFER_T *tx)
{
    tx->tx_work = false;
}

static bool ftp_put_tx_work(FTP_TXBUFFER_T *tx)
{
    return tx->tx_work;
}

static int32_t ftp_txbufpush(FTP_TXBUFFER_T *tx, uint8_t *data, uint16_t size)
{
    if (size + tx->len > tx->request)
        size = tx->request - tx->len;

    memcpy(tx->buffer + tx->len, data, size);
    tx->len += size;
    return tx->request - tx->len;
}

static void ftp_txbufferinit(FTP_TXBUFFER_T *tx, uint32_t request)
{
    tx->len = 0;
    tx->sended = 0;
    tx->request = request;
    tx->tmrID = NULL;
}

static void ftp_reportFTPState(char *data)
{
    atCmdEngine_t *engine = gFTPCtx.FTPengine;
    if (data)
        atCmdRespInfoText(engine, (const char *)data);
}

static void ftp_send_txbuf_finish(FTP_TXBUFFER_T *tx)
{
    if (tx->tmrID != NULL)
    {
        if (osiTimerStop(tx->tmrID) != true)
        {
            OSI_LOGI(0, "FTP# ftp_send_txbuf_finish stop timer failed");
        }

        osiTimerDelete(tx->tmrID);
        tx->tmrID = NULL;

        OSI_LOGI(0, "FTP# ftp_send_txbuf_finish delete timer success");
    }

    ftp_data_mode_stop();

    atCmdEngine_t *engine = gFTPCtx.FTPengine;
    atCmdWrite(engine, tx->buffer, tx->request);

    if (gFTPCtx.req_puteof == 0)
    {
        atCmdRespOK(engine);
    }
    else if (gFTPCtx.req_puteof == 1)
    {
        FTP_ClearWaitingEvent();

        if (gFTPCtx.dataSocket == 0xFF)
        {
            ftp_commandComplete(1, NULL);
            ftp_setFTPIdle();
        }
        else
        {
            FTP_AddWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
            ftp_put_disconnect();
        }
    }

    ftp_txbufferinit(tx, 0);
}

static void ftp_send_txbuf(FTP_TXBUFFER_T *tx)
{
    OSI_LOGI(0x1000412d, "FTP# in ftp_send_txbuf");
    if (tx->sended == tx->request)
    {
        ftp_send_txbuf_finish(tx);
        return;
    }

    ftp_put_tx_work_start(tx);

    int32_t ret = CFW_TcpipSocketSend(gFTPCtx.dataSocket, tx->buffer + tx->sended, tx->request - tx->sended, 0);
    if (ret >= 0)
    {
        tx->sended += ret;
    }
    else
    {
        ftp_send_txbuf_finish(tx);
        OSI_LOGI(0x1000412e, "FTP# ftp_send_txbuf error");
        return;
    }

    if (tx->tmrID != NULL)
        osiTimerStart(tx->tmrID, 1000);
}

static int ftp_UartRecv(void *param, uint8_t *data, unsigned length)
{
    OSI_LOGI(0x1000412f, "FTP# ftp_UartRecv =%d", length);
    int32_t left = ftp_txbufpush(&gFTPCtx.tx_buf, data, length);
    if (left == 0)
    {
        gFtpWaitUart = false;
        if (gFTPCtx.tx_buf.tmrID == NULL)
        {
            gFTPCtx.tx_buf.tmrID = osiTimerCreate(atEngineGetThreadId(), (osiCallback_t)ftp_send_txbuf, (void *)&gFTPCtx.tx_buf);
            if (gFTPCtx.tx_buf.tmrID == NULL)
            {
                OSI_LOGI(0, "FTP# ftp_send_txbuf create timer failed");
            }
        }

        ftp_send_txbuf(&gFTPCtx.tx_buf);
    }

    return length;
}

static void ftp_MainProcess(osiEvent_t ev);

static CFW_FTP_CMD ftp_cmd[] =
    {
        {FTP_CONNECT, "CONNECT", "220"},
        {FTP_USER, "USER", "331"},
        {FTP_PASS, "PASS", "230"},
        {FTP_SIZE, "SIZE", "213"},
        {FTP_TYPE, "TYPE", "200"},
        {FTP_REST, "REST", "350"},
        {FTP_PASV, "PASV", "227"},
        {FTP_RETR, "RETR", "150,125"},
        {FTP_LIST, "LIST", "150,125"},
        {FTP_CWD, "CWD", "250"},
        {FTP_PWD, "PWD", "257"},
        {FTP_DATA_TRANSFER, "FTP_DATA_TRANSFER", "226"},
        {FTP_STOR, "STOR", "150,125"},
        {FTP_ABOR, "ABOR", "426,225,150"},
        {FTP_PORT, "PORT", "200"},
        {FTP_QUIT, "QUIT", "221"},
};

static void ftp_dnsReq_callback(osiEvent_t *ev)
{
    if (ev->id == EV_CFW_DNS_RESOLV_SUC_IND)
    {
        ip_addr_t *ipaddr = (ip_addr_t *)ev->param1;
        if (ERR_SUCCESS == api_FTPLogin(ipaddr, gFTP_Param->port, gFTP_Param->username, gFTP_Param->passwd))
        {
            atCmdSetTimeoutHandler(gFTPCtx.FTPengine, 60 * 1000, ftp_CommandNotResponse);
        }
        else
        {
            atCmdRespCmeError(gFTPCtx.FTPengine, ERR_AT_CME_EXE_FAIL);
        }
    }
    else if (ev->id == EV_CFW_DNS_RESOLV_ERR_IND)
    {
        atCmdRespCmeError(gFTPCtx.FTPengine, ERR_AT_CME_EXE_FAIL);
    }

    free(ev);
}

static void ftp_FTPTimmerHandler(void *param)
{
    OSI_LOGI(0x10004130, "FTP# ftp_FTPTimmerHandler\n");

    if (gFtpWaitUart)
    {
        OSI_LOGI(0, "FTP# failed when waiting uart data");
        gFtpWaitUart = false;
        atCmdRespCmeError(gFTPCtx.FTPengine, ERR_AT_CME_EXE_FAIL);
    }
    g_myftpUrc = true;
    api_FTPLogout();
    osiTimerDelete(gFTPCtx.FTPTimer);
    gFTPCtx.FTPTimer = NULL;
    gFTPCtx.ftpState = FTP_IDLE;
}

static void ftp_killFTPTimmer()
{
    if (gFTPCtx.FTPTimer != NULL)
    {
        if (osiTimerStop(gFTPCtx.FTPTimer) != true)
        {
            OSI_LOGI(0, "FTP# ftp_killFTPTimmer stop timer failed");
        }

        osiTimerDelete(gFTPCtx.FTPTimer);
        gFTPCtx.FTPTimer = NULL;

        OSI_LOGI(0, "FTP# ftp_killFTPTimmer delete timer success");
    }
}

static void ftp_setFTPTimmer()
{
    ftp_killFTPTimmer();

    gFTPCtx.FTPTimer = osiTimerCreate(atEngineGetThreadId(), (osiCallback_t)ftp_FTPTimmerHandler, (void *)NULL);
    if (gFTPCtx.FTPTimer == NULL)
    {
        OSI_LOGI(0, "FTP# ftp_setFTPTimmer create timer failed");
        return;
    }

    osiTimerStart(gFTPCtx.FTPTimer, gFTP_Param->Tout * 1000);
    OSI_LOGI(0, "FTP# ftp_setFTPTimmer create timer success");
}

static void ftp_setFTPIdle()
{
    gFTPCtx.ftpState = FTP_IDLE;
    ftp_setFTPTimmer();
}

static void ftp_CommandNotResponse(atCommand_t *cmd)
{
    gFtpWaitUart = false;
    ftp_data_mode_stop();
    AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_NETWORK_TIMOUT));
}

static void ftp_commandComplete(uint8_t succ, uint8_t *data)
{
    atCmdEngine_t *engine = gFTPCtx.FTPengine;
    if (succ == 0)
    {
        if (data)
            atCmdRespInfoText(engine, (const char *)data);
        atCmdRespOK(engine);
    }
    else
    {
        if (data)
            atCmdRespErrorText(engine, (const char *)data);
        else
            atCmdRespError(engine);
    }
}

static void ftp_Init(void)
{
    memset(&gFTPCtx, 0x00, sizeof(CFW_FTP_CTX_T));
    gFTPCtx.dataSocket = 0xFF;
    gFTPCtx.listenSocket = 0xFF;
    gFTPCtx.ftpState = FTP_LOGOUT;
    gFTPCtx.ctrlSocket = 0xFF;
    gFTPCtx.connected_socket = FTP_SOCKET_NULL;
    ftp_txbufferinit(&gFTPCtx.tx_buf, 0);
    g_myftpUrc = false;
    gFtpWaitUart = false;
}

static int32_t ftp_sendCommand(SOCKET nSocket, uint8_t *cmd)
{
    OSI_LOGXI(OSI_LOGPAR_S, 0x10004133, "FTP# FTP CMD:%s\n", cmd);
    FTP_STRCAT(cmd, "\r\n");
    return CFW_TcpipSocketSend(nSocket, cmd, FTP_STRLEN(cmd), 0);
}

static void ftp_msg_callback(void *param)
{
    osiEvent_t *pEvent = (osiEvent_t *)param;
    OSI_LOGI(0, "FTP# socket callback EventID = %d", pEvent->id);
    ftp_MainProcess(*pEvent);
    free(pEvent);
}

static uint32_t ftp_ctrl_connect(CFW_FTP_CTX_T *ftp_ptr)
{
    uint32_t ret = 0;

#if LWIP_IPV4
    if (IP_IS_V4(&(ftp_ptr->serverIp)))
    {
        struct sockaddr_in *to4 = (struct sockaddr_in *)&(ftp_ptr->ser_ip);
        to4->sin_len = sizeof(to4);
        to4->sin_family = AF_INET;
        to4->sin_port = htons(ftp_ptr->port);
        inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&(ftp_ptr->serverIp)));
        ftp_ptr->ctrlSocket = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, 0);
    }
#endif
#if LWIP_IPV6
    if (IP_IS_V6(&(ftp_ptr->serverIp)) && !ip6_addr_isipv4mappedipv6(ip_2_ip6(&(ftp_ptr->serverIp))))
    {
        struct sockaddr_in6 *to6 = (struct sockaddr_in6 *)&(ftp_ptr->ser_ip);
        to6->sin6_len = sizeof(to6);
        to6->sin6_family = AF_INET6;
        to6->sin6_port = htons(ftp_ptr->port);
        inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(&(ftp_ptr->serverIp)));
        ftp_ptr->ctrlSocket = CFW_TcpipSocketEX(AF_INET6, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, 0);
    }
#endif /* LWIP_IPV6 */

    ret = CFW_TcpipSocketConnect(ftp_ptr->ctrlSocket, (CFW_TCPIP_SOCKET_ADDR *)&ftp_ptr->ser_ip, sizeof(ftp_ptr->ser_ip));
    if (ret != 0)
    {
        OSI_LOGI(0, "FTP# ftp_ctrl_connect failed!\n");
        CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
        gFTPCtx.ctrlSocket = 0xFF;
        return ret;
    }

    ftp_ptr->ftp_command = FTP_CONNECT;
    return ret;
}

/* when download file, if we set offset use REST and then
 * use RETR to get file. the response of RETR and the file data
 * came not ordered. so when we get one of them we will report CONNECT
 */
static bool g_waitDataConnect = 0;
static void ftpget_setWaitDataConnect()
{
    g_waitDataConnect = true;
}

static bool ftpget_WaitDataConnect()
{
    return g_waitDataConnect;
}

static void ftpget_clearWaitDataConnect()
{
    g_waitDataConnect = false;
}

static void ftp_put_connect_finish()
{
    ftp_reportFTPState("CONNECT");
    gFTPCtx.connected_socket = FTP_SOCKET_PUT;
}

#if 0
static void ftp_get_connect_error()
{
    ftp_reportFTPState("ERROR");
    gFTPCtx.connected_socket = FTP_SOCKET_GET;
    ftp_get_disconnect();
}
#endif

static void ftp_get_connect_finish()
{
    ftp_reportFTPState("CONNECT");
    gFTPCtx.connected_socket = FTP_SOCKET_GET;
}

static uint8_t ftp_data_connected()
{
    return gFTPCtx.connected_socket;
}

static void ftp_put_disconnect()
{
    CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    gFTPCtx.ftp_command = FTP_ABOR;
}

static void ftp_put_disconnect_finish()
{
    gFTPCtx.dataSocket = 0xFF;
    gFTPCtx.connected_socket = FTP_SOCKET_NULL;
}

static void ftp_get_disconnect_finish()
{
    gFTPCtx.dataSocket = 0xFF;
    gFTPCtx.connected_socket = FTP_SOCKET_NULL;
}

static void ftp_get_disconnect()
{
    CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    gFTPCtx.ftp_command = FTP_ABOR;
}

static int32_t ftp_get_connect(char *filename)
{
    uint8_t command[200] = {
        0,
    };
    FTP_STRCAT(command, "RETR ");
    FTP_STRCAT(command, filename);
    ftp_sendCommand(gFTPCtx.ctrlSocket, command);
    ftpget_setWaitDataConnect();
    gFTPCtx.ftp_command = FTP_RETR;
    return 0;
}

static int32_t ftp_ctrl_connected()
{
    return gFTPCtx.connect;
}

static int32_t ftp_set_ctrl_connect(bool connect)
{
    return gFTPCtx.connect = connect;
}

static int32_t ftp_size(uint8_t *filename)
{
    uint8_t command[200] = {
        0,
    };
    FTP_STRCAT(command, "SIZE ");
    FTP_STRCAT(command, filename);
    ftp_sendCommand(gFTPCtx.ctrlSocket, command);
    gFTPCtx.ftp_command = FTP_SIZE;
    return 0;
}

static bool ftp_GetStart(CFW_FTP_CTX_T *ctx)
{
    if (ctx->getpath == NULL)
        return false;
    if (ftp_data_connected() != FTP_SOCKET_NULL)
        return false;
    if (gFTPCtx.dataSocket != 0xFF)
    {
        OSI_LOGI(0, "FTP# dataSocket is not null when run ^ftpget");
        CFW_TcpipSocketClose(gFTPCtx.dataSocket);
        gFTPCtx.dataSocket = 0xFF;
        return false;
    }
    uint8_t command[200] = {
        0,
    };
    FTP_STRCAT(command, "TYPE I");
    ftp_sendCommand(ctx->ctrlSocket, command);
    ctx->ftp_command = FTP_TYPE;
    ctx->ftpState = GET_FILE;
    return true;
}

static int32_t ftp_PutStart(CFW_FTP_CTX_T *ctx)
{
    uint8_t command[200] = {
        0,
    };

    if (ctx->putpath == NULL)
        return false;
    if (ftp_data_connected() != FTP_SOCKET_NULL)
        return false;
    ctx->ftp_command = FTP_TYPE;
    ctx->ftpState = PUT_FILE;
    FTP_STRCAT(command, "TYPE I");
    ftp_sendCommand(ctx->ctrlSocket, command);
    return true;
}

static int32_t ftp_put_connect(uint8_t *filename)
{
    uint8_t command[200] = {
        0,
    };
    FTP_STRCAT(command, "STOR ");
    FTP_STRCAT(command, filename);
    ftp_sendCommand(gFTPCtx.ctrlSocket, command);
    gFTPCtx.ftp_command = FTP_STOR;
    return 0;
}

static uint8_t ftp_listenPort(struct sockaddr_storage *localaddr)
{
    int addrlen = sizeof(struct sockaddr_storage);
#if LWIP_IPV4
    if (IP_IS_V4(&(gFTPCtx.serverIp)))
    {
        struct sockaddr_in *addr = (struct sockaddr_in *)localaddr;
        gFTPCtx.listenSocket = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, 0);
        CFW_TcpipSocketGetsockname(gFTPCtx.ctrlSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, &addrlen);
        addr->sin_port = 0;
        CFW_TcpipSocketBind(gFTPCtx.listenSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, sizeof(struct sockaddr_in));
        CFW_TcpipSocketGetsockname(gFTPCtx.listenSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, &addrlen);
        CFW_TcpipSocketListen(gFTPCtx.listenSocket, 0);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004134, "FTP# FTP local addr=%s,port:%d", inet_ntoa(addr->sin_addr), addr->sin_port);
        return 0;
    }
#endif
#if LWIP_IPV6
    if (IP_IS_V6(&(gFTPCtx.serverIp)) && !ip6_addr_isipv4mappedipv6(ip_2_ip6(&(gFTPCtx.serverIp))))
    {
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *)localaddr;
        gFTPCtx.listenSocket = CFW_TcpipSocketEX(AF_INET6, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, 0);
        CFW_TcpipSocketGetsockname(gFTPCtx.ctrlSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, &addrlen);
        addr->sin6_port = 0;
        CFW_TcpipSocketBind(gFTPCtx.listenSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, sizeof(struct sockaddr_in6));
        CFW_TcpipSocketGetsockname(gFTPCtx.listenSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, &addrlen);
        CFW_TcpipSocketListen(gFTPCtx.listenSocket, 0);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004134, "FTP# FTP local addr=%s,port:%d", inet6_ntoa(addr->sin6_addr), addr->sin6_port);
        return 1;
    }
#endif /* LWIP_IPV6 */

    return 0;
}

static int32_t ftp_get_data_ind()
{
    uint8_t buffer[FTP_BUFFER_LEN] = {
        0x00,
    };
    uint32_t output = 0;

    uint32_t rec_len = CFW_TcpipSocketRecv(gFTPCtx.dataSocket, buffer, FTP_BUFFER_LEN, 0);
    OSI_LOGI(0x10004135, "FTP# rec_len =%d", rec_len);
    if (rec_len > 0)
    {
        if (gFTPCtx.req_getsize > 0)
        {
            if (gFTPCtx.req_getsize >= (gFTPCtx.getsize + rec_len))
                output = rec_len;
            else
                output = gFTPCtx.req_getsize - gFTPCtx.getsize;

            gFTPCtx.getsize += output;

            if (gFTPCtx.getsize == gFTPCtx.req_getsize)
                ftp_get_disconnect();
        }
        else
            output = rec_len;

        OSI_LOGI(0x10004136, "FTP# output=%d", output);
        atCmdEngine_t *engine = gFTPCtx.FTPengine;
        atCmdWrite(engine, buffer, output);
    }
    else if (rec_len == 0)
        OSI_LOGI(0x10004137, "FTP# rec_len == 0, ignore");
    else
    {
        OSI_LOGI(0x10004138, "FTP# rec_len < 0, transfer complete, abort transefer");
        ftp_get_disconnect();
    }
    return 0;
}

static uint8_t ftp_errorHandler(enum FTP_COMMAND cmd)
{
    OSI_LOGI(0x10004139, "FTP# ftp_errorHandler cmd=%d\n", cmd);

    switch (cmd)
    {
    case FTP_USER:
    case FTP_PASS:
        ftp_commandComplete(1, NULL);
        if (ftp_ctrl_connected())
        {
            CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
        }

        break;
    case FTP_RETR:
    case FTP_DATA_TRANSFER:
        if (ftpget_WaitDataConnect())
            ftpget_clearWaitDataConnect();
        if (gFTPCtx.dataSocket != 0xFF)
        {
            CFW_TcpipSocketClose(gFTPCtx.dataSocket);
            gFTPCtx.dataSocket = 0xFF;
        }
        gFTPCtx.connected_socket = FTP_SOCKET_NULL;
    default:
        ftp_commandComplete(1, NULL);
        FTP_ClearWaitingEvent();
        ftp_setFTPIdle();
        break;
    }
    return 0;
}

static void ftp_setOffset(uint32_t offset)
{
    OSI_LOGI(0x1000413b, "FTP# ftp_setOffset");
    uint8_t command[200] = {
        0,
    };

    FTP_SPRINTF(command, "REST %ld", offset);
    ftp_sendCommand(gFTPCtx.ctrlSocket, command);
    gFTPCtx.ftp_command = FTP_REST;
}

static int32_t ftp_ctrl_data_ind(void)
{
    uint32_t recv_num = 0;
    uint8_t command[200] = {
        0,
    };
    uint8_t *ptr1;
    uint8_t *ptr2;
    uint16_t i;
    uint16_t j;
    uint8_t numBuf[8] = {
        0,
    };
    uint8_t gRecBuf[1024] = {
        0,
    };

    recv_num = CFW_TcpipSocketRecv(gFTPCtx.ctrlSocket, gRecBuf, 1024, 0);
    if (recv_num < 3)
    {
        OSI_LOGI(0x1000413c, "FTP# warning!!! ctrl recv < 3 byte");
        return 0;
    }

    OSI_LOGXI(OSI_LOGPAR_S, 0x1000413d, "FTP# REP:%s", gRecBuf);
    if ((FTP_STRNCMP(gRecBuf, ftp_cmd[gFTPCtx.ftp_command - 1].cmd_exp_rst, 3) == 0) || (FTP_STRNCMP(gRecBuf, ftp_cmd[gFTPCtx.ftp_command - 1].cmd_exp_rst + 4, 3) == 0) || (FTP_STRNCMP(gRecBuf, ftp_cmd[gFTPCtx.ftp_command - 1].cmd_exp_rst + 8, 3) == 0))
    {
        OSI_LOGI(0x1000413e, "FTP# CMD REP OK");
    }
    else if (FTP_STRNCMP(gRecBuf, "226", 3) == 0)
    {
        /* trans finished report to ap */
        if (ftp_put_tx_work(&gFTPCtx.tx_buf))
        {
            ftp_put_tx_work_stop(&gFTPCtx.tx_buf);
            ftp_put_disconnect_finish();
        }

        OSI_LOGI(0, "FTP# 226 ignore");
        return 0;
    }
    else if (FTP_STRNCMP(gRecBuf, "421", 3) == 0)
    {
        OSI_LOGI(0, "FTP# 421 ignore");
        return 0;
    }
    else
    {
        OSI_LOGI(0x1000413f, "FTP# CMD RSP ERROR rec=%d !!!", recv_num);
        return ftp_errorHandler(gFTPCtx.ftp_command);
    }

    switch (gFTPCtx.ftp_command)
    {
    case FTP_CONNECT:
        FTP_STRCAT(command, "USER ");
        FTP_STRCAT(command, gFTPCtx.userName);
        ftp_sendCommand(gFTPCtx.ctrlSocket, command);
        gFTPCtx.ftp_command = FTP_USER;
        break;
    case FTP_USER:
        FTP_STRCAT(command, "PASS ");
        FTP_STRCAT(command, gFTPCtx.passWord);
        ftp_sendCommand(gFTPCtx.ctrlSocket, command);
        gFTPCtx.ftp_command = FTP_PASS;
        break;

    case FTP_PASS:
        gFTPCtx.logined = true;
        /* FTPOPEN */
        ftp_commandComplete(0, NULL);
        gFTPCtx.ftpState = FTP_LOGIN;
        ftp_setFTPIdle();
        break;
    case FTP_STOR:
        OSI_LOGI(0x10004140, "FTP# FTP_STOR\n");
        gFTPCtx.ftpState = FTP_WAIT_DATA;
        ftp_killFTPTimmer();
        gFTPCtx.ftp_command = FTP_DATA_TRANSFER;
        ftp_put_connect_finish();
        ftp_data_mode_start();
        break;

    case FTP_SIZE:
        OSI_LOGI(0x10004141, "FTP# FTP_SIZE END\n");
        uint8_t ftpsize[50] = {
            0,
        };
        FTP_SPRINTF(ftpsize, "$MYFTPSIZE:%s", gRecBuf + 4);
        /* FTPSIZE */
        ftp_commandComplete(0, ftpsize);
        ftp_setFTPIdle();
        break;

    case FTP_TYPE:
        if ((gFTPCtx.ftpState == GET_FILE) || (gFTPCtx.ftpState == PUT_FILE))
        {
            if (gFTP_Param->mode == 1)
            {
                FTP_STRCAT(command, "PASV ");
                ftp_sendCommand(gFTPCtx.ctrlSocket, command);
                gFTPCtx.ftp_command = FTP_PASV;
            }
            else
            {
                struct sockaddr_storage localaddr;
                uint8_t *p = NULL;
                uint8_t isIpv6 = 0;
                isIpv6 = ftp_listenPort(&localaddr);
                if (!isIpv6)
                {
                    struct sockaddr_in *addr_in = (struct sockaddr_in *)&localaddr;
                    FTP_SPRINTF(command, "PORT %s,%d,%d",
                                inet_ntoa(addr_in->sin_addr),
                                addr_in->sin_port % 256, addr_in->sin_port / 256);
                }
                else
                {
#if LWIP_IPV6
                    struct sockaddr_in6 *addr_in = (struct sockaddr_in6 *)&localaddr;
                    FTP_SPRINTF(command, "PORT %s,%d,%d",
                                inet_ntoa(addr_in->sin6_addr),
                                addr_in->sin6_port % 256, addr_in->sin6_port / 256);
#endif
                }

                p = command + 6;
                while (*p != '\0')
                {
                    if (*p == '.')
                        *p = ',';
                    p++;
                }

                ftp_sendCommand(gFTPCtx.ctrlSocket, command);
                gFTPCtx.ftp_command = FTP_PORT;
            }
        }
        break;

    case FTP_ABOR:
        OSI_LOGI(0x10004144, "FTP# FTP_ABOR END\n");
        /* ignore reponse OK in data socket close response */
        //ftp_commandComplete(0, NULL);
        ftp_setFTPIdle();
        break;

    case FTP_REST:
        ftp_TransferStart();
        break;

    case FTP_PASV:
        ptr1 = gRecBuf + FTP_STRLEN(gRecBuf);
        while ((*(ptr1) != ')') && (ptr1 != gRecBuf))
        {
            ptr1--;
        }
        ptr2 = ptr1;

        while ((*(ptr2) != ',') && (ptr2 != gRecBuf))
        {
            ptr2--;
        }
        memset(numBuf, 0x00, 8);
        FTP_STRNCPY(numBuf, ptr2 + 1, ptr1 - ptr2 - 1);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004145, "FTP#  pasv receive:   %s \n\r", numBuf);

        j = FTP_ATOI(numBuf);

        ptr2--;
        ptr1 = ptr2;
        ptr2++;

        while ((*(ptr1) != ',') && (ptr1 != gRecBuf))
        {
            ptr1--;
        }
        memset(numBuf, 0x00, 8);

        FTP_STRNCPY(numBuf, ptr1 + 1, ptr2 - ptr1 - 1);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004145, "FTP#  pasv receive:   %s \n\r", numBuf);

        i = FTP_ATOI(numBuf);
        OSI_LOGXI(OSI_LOGPAR_SII, 0x10004146, "FTP#  pasv receive:   %s      j: %d, i: %d\n\r", gRecBuf, i, j);

        gFTPCtx.data_port = i * 256 + j;

#if LWIP_IPV4
        if (IP_IS_V4(&(gFTPCtx.serverIp)))
        {
            struct sockaddr_in *to4 = (struct sockaddr_in *)&(gFTPCtx.ser_ip_data);
            to4->sin_len = sizeof(to4);
            to4->sin_family = AF_INET;
            to4->sin_port = htons(gFTPCtx.data_port);
            inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&(gFTPCtx.serverIp)));
            gFTPCtx.dataSocket = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, 0);
        }
#endif
#if LWIP_IPV6
        if (IP_IS_V6(&(gFTPCtx.serverIp)) && !ip6_addr_isipv4mappedipv6(ip_2_ip6(&(gFTPCtx.serverIp))))
        {
            struct sockaddr_in6 *to6 = (struct sockaddr_in6 *)&(gFTPCtx.ser_ip_data);
            to6->sin6_len = sizeof(to6);
            to6->sin6_family = AF_INET6;
            to6->sin6_port = htons(gFTPCtx.data_port);
            inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(&(gFTPCtx.serverIp)));
            gFTPCtx.dataSocket = CFW_TcpipSocketEX(AF_INET6, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, 0);
        }
#endif /* LWIP_IPV6 */

        CFW_TcpipSocketConnect(gFTPCtx.dataSocket, (CFW_TCPIP_SOCKET_ADDR *)&gFTPCtx.ser_ip_data, sizeof(gFTPCtx.ser_ip_data));
        break;

    case FTP_PORT:
        OSI_LOGI(0x10004147, "FTP# FTP PORT command complete");
        if (gFTPCtx.req_getoffset > 0)
            ftp_setOffset(gFTPCtx.req_getoffset);
        else
            ftp_TransferStart();

        break;
    case FTP_RETR:
        gFTPCtx.ftp_command = FTP_DATA_TRANSFER;
        gFTPCtx.ftpState = FTP_WAIT_DATA;
        if (ftpget_WaitDataConnect())
        {
            ftp_get_connect_finish();
            ftpget_clearWaitDataConnect();
        }
        ftp_killFTPTimmer();
        break;

    case FTP_DATA_TRANSFER:
        ftp_setFTPIdle();
        break;
    case FTP_QUIT:
        /* report OK until ctrl socket close response, ignore this response */
        break;
    default:
        ftp_setFTPIdle();
        ftp_commandComplete(1, NULL);
        break;
    }
    return 0;
}

static bool ftp_isHandleEvent(osiEvent_t ev)
{
    if ((EV_CFW_TCPIP_REV_DATA_IND <= ev.id) && (ev.id <= EV_CFW_TCPIP_ACCEPT_IND))
    {
        return true;
    }
    else if ((EV_CFW_TCPIP_SOCKET_CONNECT_RSP <= ev.id) && (ev.id <= EV_CFW_TCPIP_SOCKET_SEND_RSP))
    {
        return true;
    }
    else
        return false;
}

static void ftp_TransferStart()
{
    OSI_LOGI(0x10004148, "FTP# ftp_TransferStart");
    if (GET_FILE == gFTPCtx.ftpState)
        ftp_get_connect(gFTPCtx.getpath);
    else if (PUT_FILE == gFTPCtx.ftpState)
        ftp_put_connect(gFTPCtx.putpath);
    gFTPCtx.ftpState = FTP_WAIT_DATA;
    ftp_killFTPTimmer();
}

static int32_t ftp_data_msg_handler(osiEvent_t ev)
{
    if (EV_CFW_TCPIP_SOCKET_CONNECT_RSP == ev.id)
    {
        OSI_LOGI(0x10004149, "FTP# EV_CFW_TCPIP_SOCKET_CONNECT_RSP data\n");
        if (gFTPCtx.req_getoffset > 0)
            ftp_setOffset(gFTPCtx.req_getoffset);
        else
            ftp_TransferStart();
    }
    else if (EV_CFW_TCPIP_REV_DATA_IND == ev.id)
    {
        OSI_LOGI(0x1000414a, "FTP# EV_CFW_TCPIP_REV_DATA_IND data\n");
        if (ftpget_WaitDataConnect())
        {
            ftp_get_connect_finish();
            ftpget_clearWaitDataConnect();
        }
        ftp_get_data_ind();
    }
    else if (EV_CFW_TCPIP_SOCKET_CLOSE_RSP == ev.id)
    {
        OSI_LOGI(0x1000414b, "FTP# EV_CFW_TCPIP_SOCKET_CLOSE_RSP data exit data mode\n");
        /* exit data mode */
        gFTPCtx.ftpState = FTP_IDLE;
        if (FTP_isWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0))
        {
            ftp_commandComplete(0, NULL);
            FTP_DelWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
            /* FTPCLOSE */
            ftp_setFTPIdle();
        }
        if (ftp_data_connected() == FTP_SOCKET_GET)
            ftp_get_disconnect_finish();
        else if (ftp_data_connected() == FTP_SOCKET_PUT)
        {
            if (!ftp_put_tx_work(&gFTPCtx.tx_buf))
                ftp_put_disconnect_finish();
        }
        if (gFTPCtx.ctrlSocket == 0xFF)
            gFTPCtx.ftpState = FTP_LOGOUT;
    }
    else if (EV_CFW_TCPIP_CLOSE_IND == ev.id)
    {
        /* get file complete */
        //ftp_commandComplete(0, NULL);
        OSI_LOGI(0x1000414c, "FTP# EV_CFW_TCPIP_CLOSE_IND data\n");
        CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    }
    return 0;
}

static int32_t ftp_ctrl_msg_handler(osiEvent_t ev)
{
    if (EV_CFW_TCPIP_SOCKET_CONNECT_RSP == ev.id)
    {
        ftp_set_ctrl_connect(true);
        OSI_LOGI(0x1000414d, "FTP# EV_CFW_TCPIP_SOCKET_CONNECT_RSP ctrl\n");
    }
    else if (EV_CFW_TCPIP_REV_DATA_IND == ev.id)
    {
        ftp_ctrl_data_ind();
    }
    else if (EV_CFW_TCPIP_SOCKET_CLOSE_RSP == ev.id)
    {
        OSI_LOGI(0x1000414e, "FTP# EV_CFW_TCPIP_SOCKET_CLOSE_RSP ctrl\n");
        gFTPCtx.ctrlSocket = 0xFF;
        gFTPCtx.ftpState = FTP_LOGOUT;
        if (FTP_isWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0))
        {
            ftp_commandComplete(0, NULL);
            FTP_DelWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
        }
        else
        {
            if (g_mynetUrc && g_myftpUrc)
                ftp_reportFTPState("$MYURCFTP: 0");
        }
        ftp_killFTPTimmer();
        ftp_set_ctrl_connect(false);
    }
    else if (EV_CFW_TCPIP_CLOSE_IND == ev.id)
    {
        OSI_LOGI(0x1000414f, "FTP# EV_CFW_TCPIP_CLOSE_IND ctrl\n");
        CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
        ftp_killFTPTimmer();
        gFTPCtx.ftpState = FTP_LOGOUT;
        g_myftpUrc = true;
    }
    else if (EV_CFW_TCPIP_ERR_IND == ev.id)
    {
        OSI_LOGI(0, "FTP# EV_CFW_TCPIP_ERR_IND ctrl\n");

        bool ftpURC = true;
        if (gFTPCtx.ftp_command == FTP_CONNECT ||
            gFTPCtx.ftp_command == FTP_USER ||
            (gFTPCtx.ftp_command == FTP_PASS && gFTPCtx.logined != true))
        {
            OSI_LOGI(0, "FTP# FTP_CONNECT err\n");
            ftp_commandComplete(1, NULL);
            ftpURC = false;
        }
        else if (FTP_isWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0))
        {
            ftp_commandComplete(1, NULL);
            FTP_DelWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
        }

        CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
        gFTPCtx.ctrlSocket = 0xFF;
        CFW_TcpipSocketClose(gFTPCtx.dataSocket);
        gFTPCtx.dataSocket = 0xFF;
        gFTPCtx.ftpState = FTP_LOGOUT;
        ftp_killFTPTimmer();
        ftp_set_ctrl_connect(false);
        if (g_mynetUrc && ftpURC)
            ftp_reportFTPState("$MYURCFTP: 0");
    }
    else
        OSI_LOGI(0x10004150, "FTP# event=%d", ev.id);
    return 0;
}

static int32_t ftp_listen_msg_handler(osiEvent_t ev)
{
    if (EV_CFW_TCPIP_ACCEPT_IND == ev.id)
    {
        OSI_LOGI(0x10004151, "FTP# EV_CFW_TCPIP_ACCEPT_IND listen\n");

        CFW_TCPIP_SOCKET_ADDR from;
        int32_t addrlen = sizeof(CFW_TCPIP_SOCKET_ADDR);
        gFTPCtx.dataSocket = CFW_TcpipSocketAccept(gFTPCtx.listenSocket, (CFW_TCPIP_SOCKET_ADDR *)&from, (uint32_t *)&addrlen);
        CFW_TcpipSocketClose(gFTPCtx.listenSocket);
    }
    else if (EV_CFW_TCPIP_SOCKET_CLOSE_RSP == ev.id)
    {
        OSI_LOGI(0x10004152, "FTP# EV_CFW_TCPIP_SOCKET_CLOSE_RSP listen\n");
        gFTPCtx.listenSocket = 0xFF;
    }
    return 0;
}

static void ftp_MainProcess(osiEvent_t ev)
{
    if (!ftp_isHandleEvent(ev))
    {
        OSI_LOGI(0x10004153, "FTP# invalid event=%d", ev.id);
        return;
    }

    SOCKET sock_id = ev.param1;
    if (sock_id == gFTPCtx.dataSocket)
        ftp_data_msg_handler(ev);
    else if (sock_id == gFTPCtx.ctrlSocket)
        ftp_ctrl_msg_handler(ev);
    else if (sock_id == gFTPCtx.listenSocket)
        ftp_listen_msg_handler(ev);
    else
        return;
}

static int32_t api_FTPLogin(ip_addr_t *serIp, uint16_t serPort, uint8_t *user, uint8_t *pswd)
{

    if (gFTPCtx.logined)
        return ERR_DEVICE_NOT_FOUND;

    FTP_STRCPY(gFTPCtx.userName, user);
    FTP_STRCPY(gFTPCtx.passWord, pswd);
    gFTPCtx.serverIp = *serIp;
    gFTPCtx.port = serPort;

    if (SOCKET_ERROR != ftp_ctrl_connect(&gFTPCtx))
        return ERR_SUCCESS;
    else
        return ERR_INVALID_HANDLE;
}

static int32_t api_FTPLogout()
{

    uint8_t command[200] = {
        0,
    };
    FTP_STRCAT(command, "QUIT");
    ftp_sendCommand(gFTPCtx.ctrlSocket, command);
    if (gFTPCtx.dataSocket != 0xFF)
        CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
    gFTPCtx.ftp_command = FTP_QUIT;
    gFTPCtx.ftpState = FTP_LOGOUT;
    return ERR_SUCCESS;
}

static bool ftp_setPUTINFO(CFW_FTP_CTX_T *ctx, const uint8_t *path, uint16_t length, uint32_t size, uint32_t eof)
{
    if (ctx->putpath != NULL)
        free(ctx->putpath);

    ctx->putpath = (uint8_t *)malloc(length + 1);
    if (ctx->putpath == NULL)
    {
        OSI_LOGI(0x10004154, "FTP# malloc failed");
        return false;
    }
    memset(ctx->putpath, 0x00, length + 1);
    FTP_STRNCPY(ctx->putpath, path, length);
    ctx->req_putsize = size;
    ctx->req_puteof = eof;
    OSI_LOGXI(OSI_LOGPAR_S, 0x10004155, "FTP# ftp_setPUTINFO %s", path);
    OSI_LOGI(0, "FTP# ftp_setPUTINFO size=%d, eof=%d", ctx->req_putsize, ctx->req_puteof);
    return true;
}

static bool ftp_setGETINFO(CFW_FTP_CTX_T *ctx, const uint8_t *path, uint16_t length, uint32_t offset, uint32_t size)
{
    if (length > 255)
        return false;

    if (ctx->getpath != NULL)
        free(ctx->getpath);

    ctx->getpath = (char *)malloc(length + 1);
    if (ctx->getpath == NULL)
    {
        OSI_LOGI(0x10004154, "FTP# malloc failed");
        return false;
    }
    memset(ctx->getpath, 0x00, length + 1);
    FTP_STRNCPY(ctx->getpath, path, length);
    ctx->req_getoffset = offset;
    ctx->req_getsize = size;
    ctx->getsize = 0;
    OSI_LOGXI(OSI_LOGPAR_S, 0x10004156, "FTP# ftp_setGETINFO path=%s, offset=%d,size=%d", path, offset, size);
    return true;
}

static void ftp_gethostport(char *url, uint8_t *host, uint16_t *port)
{
    char *p;
    ip_addr_t addr;

    if (ipaddr_aton(url, &addr))
    {
        FTP_STRCPY(host, url);
        *port = 21;
    }
    else
    {
        p = FTP_STRSTR(url, ":");
        if (p != NULL)
        {
            FTP_STRNCPY(host, url, p - url);
            *port = FTP_ATOI(p + 1);
        }
        else
        {
            FTP_STRCPY(host, url);
            *port = 21;
        }
    }
}

static uint8_t ftp_getCID(uint8_t nSim)
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

void AT_GPRS_CmdFunc_MYFTPOPEN(atCommand_t *pParam)
{
    uint32_t err;
    bool paramok = true;
    char tmpString[60] = {
        0x00,
    };
    char url[FTP_STRING_SIZE] = {
        0,
    };
    const char *tempstr;
    ip_addr_t nIpAddr;
    struct FTP_Param ftp_param = {0, {}, 0, {}, {}, 1, 30, 0};
    uint8_t nSim = atCmdGetSim(pParam->engine);
    uint8_t nCid = ftp_getCID(nSim);

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        if (netif_default == NULL)
        {
            OSI_LOGI(0, "FTP# AT_GPRS_CmdFunc_MYFTPOPEN netif_default is NULL");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        if (pParam->param_count != 7)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        ftp_param.channel = atParamUint(pParam->params[0], &paramok);
        if (!paramok)
        {
            OSI_LOGI(0x100052be, "FTP# FTPOPEN get channel error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        tempstr = atParamStr(pParam->params[1], &paramok);
        if (!paramok)
        {
            OSI_LOGI(0x100043ec, "FTP# FTPOPEN get url error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        FTP_STRCPY(url, tempstr);
        ftp_gethostport(url, ftp_param.host, &ftp_param.port);

        tempstr = atParamStr(pParam->params[2], &paramok);
        if (!paramok)
        {
            OSI_LOGI(0x100043ed, "FTP# FTPOPEN get username error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        FTP_STRCPY(ftp_param.username, tempstr);

        tempstr = atParamStr(pParam->params[3], &paramok);
        if (!paramok)
        {
            OSI_LOGI(0x100043ee, "FTP# FTPOPEN get password error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        FTP_STRCPY(ftp_param.passwd, tempstr);

        ftp_param.mode = atParamUint(pParam->params[4], &paramok);
        if (!paramok || (ftp_param.mode != 0 && ftp_param.mode != 1))
        {
            OSI_LOGI(0x100043ef, "FTP# FTPOPEN get mode error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        ftp_param.Tout = atParamUintInRange(pParam->params[5], 5, 30, &paramok);
        if (!paramok)
        {
            OSI_LOGI(0x100043f0, "FTP# FTPOPEN get timeout error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        ftp_param.FTPtype = atParamUint(pParam->params[6], &paramok);
        if (!paramok || (ftp_param.FTPtype != 0 && ftp_param.FTPtype != 1))
        {
            OSI_LOGI(0x100043f1, "FTP# FTPOPEN get ftp type error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (gFTP_Param == NULL)
        {
            gFTP_Param = (struct FTP_Param *)malloc(sizeof(struct FTP_Param));
        }

        if (ftp_ctrl_connected())
        {
            if (strcmp((const char *)ftp_param.host, (const char *)gFTP_Param->host) == 0 && ftp_param.port == gFTP_Param->port)
            {
                ftp_setFTPTimmer();
                RETURN_OK(pParam->engine);
            }
            else
            {
                OSI_LOGI(0x100043f2, "FTP# FTPOPEN ftp have login");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        memcpy(gFTP_Param, &ftp_param, sizeof(struct FTP_Param));

        OSI_LOGI(0x10004158, "FTP# FTPOPEN %d", gFTP_Param->channel);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004159, "FTP# FTPOPEN %s", gFTP_Param->host);
        OSI_LOGI(0x10004158, "FTP# FTPOPEN %d", gFTP_Param->port);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004159, "FTP# FTPOPEN %s", gFTP_Param->username);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004159, "FTP# FTPOPEN %s", gFTP_Param->passwd);
        OSI_LOGI(0x10004158, "FTP# FTPOPEN %d", gFTP_Param->mode);
        OSI_LOGI(0x10004158, "FTP# FTPOPEN %d", gFTP_Param->Tout);
        OSI_LOGI(0x10004158, "FTP# FTPOPEN %d", gFTP_Param->FTPtype);

        ftp_Init();
        gFTPCtx.nSimID = nSim;
        gFTPCtx.FTPengine = (atCmdEngine_t *)pParam->engine;

        err = CFW_GethostbynameEX((char *)gFTP_Param->host, &nIpAddr, nCid, nSim, (osiCallback_t)ftp_dnsReq_callback, NULL);
        if (err == RESOLV_QUERY_INVALID)
        {
            OSI_LOGI(0x1000415a, "FTP# FTPOPEN gethost by name error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else if (err == RESOLV_COMPLETE)
        {
            if (ERR_SUCCESS == api_FTPLogin(&nIpAddr, gFTP_Param->port, gFTP_Param->username, gFTP_Param->passwd))
                AT_CMD_RETURN(atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, ftp_CommandNotResponse));
            else
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else if (err == RESOLV_QUERY_QUEUED)
        {
            AT_CMD_RETURN(atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, ftp_CommandNotResponse));
        }
    }
    break;
    case AT_CMD_READ:
        OSI_LOGI(0x1000415b, "FTP# FTPOPEN READ");
        if (gFTPCtx.ftpState == FTP_LOGOUT)
        {
            FTP_STRCPY(tmpString, "$MYFTPOPEN:0");
            atCmdRespInfoText(pParam->engine, (char *)tmpString);
            atCmdRespOK(pParam->engine);
        }
        else
        {
            FTP_STRCPY(tmpString, "$MYFTPOPEN:1");
            atCmdRespInfoText(pParam->engine, (char *)tmpString);
            atCmdRespOK(pParam->engine);
        }

        break;
    case AT_CMD_TEST:
        //strcpy(tmpString, "$MYFTPOPEN:(<destination_ip/url>),(<username>),(<password>)[,(<account>),(range of supported port's),(range of supported port's),(range of supported port's)]");
        atCmdRespInfoText(pParam->engine,
                          "$MYFTPOPEN:(<destination_ip/url>),(<username>),(<password>)[,(<account>),(range of supported port's),(range of supported port's),(range of supported port's)]");
        atCmdRespOK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x1000415c, "FTP# FTPOPEN ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        return;
    }
}

void AT_GPRS_CmdFunc_MYFTPSIZE(atCommand_t *pParam)
{
    bool paramok = true;
    uint8_t filename[FTP_STRING_SIZE] = {
        0x00,
    };

    switch (pParam->type)
    {
    case AT_CMD_SET:

        if (netif_default == NULL)
        {
            OSI_LOGI(0, "FTP# AT_GPRS_CmdFunc_MYFTPSIZE netif_default is NULL");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        if (!ftp_ctrl_connected())
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (pParam->param_count != 1)
        {
            OSI_LOGI(0x100043f3, "FTP# FTPSIZE get param count error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        FTP_STRCPY(filename, atParamStr(pParam->params[0], &paramok));
        if (!paramok)
        {
            OSI_LOGI(0x100043f4, "FTP# FTPSIZE get filename error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        ftp_size(filename);
        FTP_ClearWaitingEvent();
        atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, ftp_CommandNotResponse);
        break;
    default:
        OSI_LOGI(0x1000415e, "FTP# FTPSIZE ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        break;
    }
}

void AT_GPRS_CmdFunc_MYFTPGET(atCommand_t *pParam)
{
    uint32_t offset = 0;
    uint32_t size = 0;
    bool paramok = true;

    if (AT_CMD_SET == pParam->type)
    {
        if (netif_default == NULL)
        {
            OSI_LOGI(0, "FTP# AT_GPRS_CmdFunc_MYFTPGET netif_default is NULL");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        if (!ftp_ctrl_connected())
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        const uint8_t *filepath = (uint8_t *)atParamStr(pParam->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (pParam->param_count >= 2)
            offset = atParamUint(pParam->params[1], &paramok);

        if (pParam->param_count >= 3)
            size = atParamUint(pParam->params[2], &paramok);

        if (ftp_setGETINFO(&gFTPCtx, filepath, FTP_STRLEN(filepath), offset, size))
        {
            if (ftp_GetStart(&gFTPCtx))
            {
                FTP_ClearWaitingEvent();
                FTP_AddWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
                atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, ftp_CommandNotResponse);
            }
            else
            {
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        atCmdRespInfoText(pParam->engine, "$MYFTPGET=<File_Name>,[<data_offset>,<data_Length>]");
        atCmdRespOK(pParam->engine);
        return;
    }
    else
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    return;
}

void AT_GPRS_CmdFunc_MYFTPPUT(atCommand_t *pParam)
{
    if (AT_CMD_SET == pParam->type)
    {
        if (netif_default == NULL)
        {
            OSI_LOGI(0, "FTP# AT_GPRS_CmdFunc_MYFTPPUT netif_default is NULL");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        if (pParam->param_count != 3)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        if (!ftp_ctrl_connected())
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        bool paramok = true;
        const uint8_t *filepath = (uint8_t *)atParamStr(pParam->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        uint32_t req_length = atParamUint(pParam->params[1], &paramok);
        if (!paramok || req_length == 0 || req_length > FTP_SEND_MAX_SIZE)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        uint32_t eof = atParamUint(pParam->params[2], &paramok);
        if (!paramok || (eof != 0 && eof != 1))
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (!ftp_setPUTINFO(&gFTPCtx, filepath, FTP_STRLEN(filepath), req_length, eof))
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (ftp_data_connected() == FTP_SOCKET_NULL)
        {
            ftp_PutStart(&gFTPCtx);
            ftp_txbufferinit(&gFTPCtx.tx_buf, req_length);
        }
        else if (ftp_data_connected() == FTP_SOCKET_PUT)
        {
            ftp_txbufferinit(&gFTPCtx.tx_buf, req_length);
            ftp_data_mode_start();
        }
        else
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        FTP_ClearWaitingEvent();
        atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, ftp_CommandNotResponse);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        atCmdRespInfoText(pParam->engine, "$MYFTPPUT=<File_Name>,<data_length>,<EOF>");
        atCmdRespOK(pParam->engine);
        return;
    }
    else
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
    return;
}

void AT_GPRS_CmdFunc_MYFTPCLOSE(atCommand_t *pParam)
{
    if (gFTPCtx.ftpState == FTP_LOGOUT)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    switch (pParam->type)
    {
    case AT_CMD_EXE:

        if (netif_default == NULL)
        {
            OSI_LOGI(0, "FTP# AT_GPRS_CmdFunc_MYFTPCLOSE netif_default is NULL");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        api_FTPLogout();
        FTP_ClearWaitingEvent();
        FTP_AddWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
        atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, ftp_CommandNotResponse);
        break;
    default:
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        break;
    }
}

#endif
