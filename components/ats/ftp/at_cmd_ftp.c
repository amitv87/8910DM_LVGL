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

#ifdef CONFIG_AT_FTP_SUPPORT
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
#include "at_cmd_ftp.h"

#define FTP_SPRINTF(s1, s2, ...) sprintf((char *)s1, (const char *)s2, ##__VA_ARGS__)
#define FTP_STRLEN(s) strlen((const char *)s)
#define FTP_STRSTR(s1, s2) strstr((const char *)s1, (const char *)s2)
#define FTP_STRCPY(des, src) strcpy((char *)des, (const char *)src)
#define FTP_STRCAT(des, src) strcat((char *)des, (const char *)src)
#define FTP_STRNCPY(des, src, len) strncpy((char *)des, (const char *)src, len)
#define FTP_STRNCMP(s1, s2, len) strncmp((const char *)s1, (const char *)s2, len)
#define FTP_ATOI(s) atoi((const char *)s)

int32_t api_FTPLogout();
int32_t api_FTPLogin(ip_addr_t *serIp, uint16_t serPort, uint8_t *user, uint8_t *pswd);
void ftp_MainProcess(osiEvent_t ev);
static void ftp_commandComplete(uint8_t succ, uint8_t *data);
static void ftp_get_disconnect();
static void ftp_put_disconnect();
static void ftp_TransferStart();
static int ftp_UartRecv(void *param, uint8_t *data, unsigned length);
static void ftp_OpenTimer(atCommand_t *cmd);

static CFW_FTP_CTX_T gFTPCtx = {
    .connect = false,
    .ftpState = FTP_LOGOUT,
    .FTPTimer = NULL,
    .FTPengine = NULL};

static struct FTP_Param *gFTP_Param = NULL;

#define FTP_BUFFER_LEN (1024 * 10)

typedef struct
{
    uint8_t data[FTP_SEND_MAX_SIZE];
    uint8_t *cur;
    uint8_t *end;
} FTP_TXBUFF_T;

#define FTP_WAITING_EVENT_MAX 20

struct FTPWaitingEvent
{
    uint32_t nEvent;
    uint8_t nSim;
    uint8_t nDLCI;
    uint8_t enable;
};

struct FTPWaitingEvent gFTP_WaitingEvent[FTP_WAITING_EVENT_MAX];

void FTP_AddWaitingEvent(uint32_t event, uint8_t sim, uint8_t dlci)
{
    for (int i = 0; i < FTP_WAITING_EVENT_MAX; i++)
    {
        if (gFTP_WaitingEvent[i].enable == 0)
        {
            gFTP_WaitingEvent[i].nEvent = event;
            gFTP_WaitingEvent[i].nSim = sim;
            gFTP_WaitingEvent[i].nDLCI = dlci;
            gFTP_WaitingEvent[i].enable = 1;
            OSI_LOGI(0, "FTP add waiting to #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            return;
        }
    }
    OSI_LOGI(0, "FTP fail add waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return;
}

bool FTP_isWaitingEvent(uint32_t event, uint8_t sim, uint8_t dlci)
{
    for (int i = 0; i < FTP_WAITING_EVENT_MAX; i++)
    {
        if ((gFTP_WaitingEvent[i].enable) &&
            (gFTP_WaitingEvent[i].nEvent == event) &&
            (gFTP_WaitingEvent[i].nSim == sim))
        {
            OSI_LOGI(0, "FTP is waiting #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            return true;
        }
    }
    OSI_LOGI(0, "FTP not waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return false;
}

void FTP_DelWaitingEvent(uint32_t event, uint8_t sim, uint8_t dlci)
{
    for (int i = 0; i < FTP_WAITING_EVENT_MAX; i++)
    {
        if ((gFTP_WaitingEvent[i].enable) &&
            (gFTP_WaitingEvent[i].nEvent == event) &&
            (gFTP_WaitingEvent[i].nSim == sim))
        {
            OSI_LOGI(0, "FTP delete waiting #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            gFTP_WaitingEvent[i].nEvent = 0;
            gFTP_WaitingEvent[i].nSim = 0;
            gFTP_WaitingEvent[i].nDLCI = 0;
            gFTP_WaitingEvent[i].enable = 0;
            return;
        }
    }
    OSI_LOGI(0, "FTP fail delete waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
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

static void ftp_data_mode_start()
{
    if (gFTPCtx.FTPengine != NULL && gFTPCtx.datamode == false)
    {
        atCmdRespOutputPrompt(gFTPCtx.FTPengine);
        atCmdSetBypassMode(gFTPCtx.FTPengine, (atCmdBypassCB_t)ftp_UartRecv, NULL);
        gFTPCtx.datamode = true;
    }
}

static void ftp_data_mode_stop()
{
    if (gFTPCtx.FTPengine != NULL && gFTPCtx.datamode == true)
    {
        atCmdSetLineMode(gFTPCtx.FTPengine);
        gFTPCtx.datamode = false;
    }
}

static void ftp_reportFTPState(char *data)
{
    atCmdEngine_t *engine = gFTPCtx.FTPengine;
    if (engine == NULL)
    {
        OSI_LOGI(0, "FTP# ftp_reportFTPState engine is null!");
        return;
    }
    if (data)
        atCmdRespInfoText(engine, data);
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
    gFTPCtx.datamode = false;
}

static void ftp_send_txbuf_finish(FTP_TXBUFFER_T *tx, bool ret)
{
    if (tx->tmrID != NULL)
    {
        if (osiTimerStop(tx->tmrID) != true)
        {
            OSI_LOGI(0, "FTP# ftp_send_txbuf_finish stop txtimer failed");
        }

        osiTimerDelete(tx->tmrID);
        tx->tmrID = NULL;

        OSI_LOGI(0, "FTP# ftp_send_txbuf_finish delete txtimer success");
    }

    ftp_txbufferinit(tx, 0);

    if (FTP_isWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0))
    {
        if (ret)
            ftp_commandComplete(0, NULL);
        else
            ftp_commandComplete(1, NULL);

        FTP_DelWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
    }
}

static void ftp_send_txbuf(FTP_TXBUFFER_T *tx)
{
    OSI_LOGI(0x1000412d, "FTP# in ftp_send_txbuf");
    if (tx->sended == tx->request)
    {
        ftp_send_txbuf_finish(tx, true);
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
        ftp_send_txbuf_finish(tx, false);
        OSI_LOGI(0x1000412e, "FTP# ftp_send_txbuf error");
        return;
    }

    if (tx->tmrID != NULL)
        osiTimerStart(tx->tmrID, 1000);
}

#define CHAR_ESC 0x1b

static bool ftp_UartEscCheck(uint8_t *data, unsigned length)
{
    int i = 0;

    while (i < length)
    {
        if (data[i] == CHAR_ESC)
        {
            OSI_LOGI(0, "FTP# Uart recv ESC!");
            return true;
        }

        i++;
    }

    return false;
}
static int ftp_UartRecv(void *param, uint8_t *data, unsigned length)
{
    OSI_LOGI(0x1000412f, "FTP# ftp_UartRecv =%d", length);

    if (ftp_UartEscCheck(data, length))
    {
        OSI_LOGI(0, "FTP# ftp_UartRecv will exit by ESC char");
        ftp_send_txbuf_finish(&gFTPCtx.tx_buf, true);
        ftp_data_mode_stop();
        return length;
    }

    if (gFTPCtx.FTPengine == NULL)
    {
        OSI_LOGI(0, "FTP# ftp_UartRecv will exit caused by unavilable ftpengine.");
        ftp_send_txbuf_finish(&gFTPCtx.tx_buf, false);
        return length;
    }

    int32_t left = ftp_txbufpush(&gFTPCtx.tx_buf, data, length);
    if (left == 0)
    {
        ftp_data_mode_stop();
        atCmdWrite(gFTPCtx.FTPengine, gFTPCtx.tx_buf.buffer, gFTPCtx.tx_buf.len);

        if (gFTPCtx.tx_buf.tmrID == NULL)
        {
            gFTPCtx.tx_buf.tmrID = osiTimerCreate(atEngineGetThreadId(), (osiCallback_t)ftp_send_txbuf, (void *)&gFTPCtx.tx_buf);

            if (gFTPCtx.tx_buf.tmrID == NULL)
            {
                OSI_LOGI(0, "FTP# ftp_UartRecv create txtimer failed");
            }
            else
            {
                OSI_LOGI(0, "FTP# ftp_UartRecv create txtimer successed");
            }
        }

        ftp_send_txbuf(&gFTPCtx.tx_buf);
    }
    return length;
}

CFW_FTP_CMD ftp_cmd[] =
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
        {FTP_ABOR, "ABOR", "426,225"},
        {FTP_PORT, "PORT", "200"},
        {FTP_QUIT, "QUIT", "221"},
};

static void ftp_dnsReq_callback(osiEvent_t *ev)
{
    if (gFTPCtx.FTPengine == NULL)
        return;

    if (ev->id == EV_CFW_DNS_RESOLV_SUC_IND)
    {
        ip_addr_t *ipaddr = (ip_addr_t *)ev->param1;
        if (ERR_SUCCESS != api_FTPLogin(ipaddr, gFTP_Param->port, gFTP_Param->username, gFTP_Param->passwd))
        {
            OSI_LOGI(0, "FTP# ftp_dnsReq_callback: api_FTPLogin failed");
            atCmdRespCmeError(gFTPCtx.FTPengine, ERR_AT_CME_EXE_FAIL);
            gFTPCtx.FTPengine = NULL;
        }
    }
    else if (ev->id == EV_CFW_DNS_RESOLV_ERR_IND)
    {
        OSI_LOGI(0, "FTP# ftp_dnsReq_callback: EV_CFW_DNS_RESOLV_ERR_IND failed");
        atCmdRespCmeError(gFTPCtx.FTPengine, ERR_AT_CME_EXE_FAIL);
        gFTPCtx.FTPengine = NULL;
    }

    free(ev);
}

static void ftp_FTPTimmerHandler(void *param)
{
    OSI_LOGI(0x10004130, "FTP# ftp_FTPTimmerHandler\n");

    api_FTPLogout();

    osiTimerDelete(gFTPCtx.FTPTimer);
    gFTPCtx.FTPTimer = NULL;
    OSI_LOGI(0, "FTP# ftp_FTPTimmerHandler delete FTPTimer success");

    gFTPCtx.ftpState = FTP_IDLE;
}

static void ftp_killFTPTimmer()
{
    if (gFTPCtx.FTPTimer != NULL)
    {
        if (osiTimerStop(gFTPCtx.FTPTimer) != true)
        {
            OSI_LOGI(0, "FTP# ftp_killFTPTimmer stop FTPTimer failed");
        }

        osiTimerDelete(gFTPCtx.FTPTimer);
        gFTPCtx.FTPTimer = NULL;

        OSI_LOGI(0, "FTP# ftp_killFTPTimmer delete FTPTimer success");
    }
}

static void ftp_setFTPTimmer()
{
    ftp_killFTPTimmer();

    gFTPCtx.FTPTimer = osiTimerCreate(atEngineGetThreadId(), (osiCallback_t)ftp_FTPTimmerHandler, (void *)NULL);
    if (gFTPCtx.FTPTimer == NULL)
    {
        OSI_LOGI(0, "FTP# ftp_setFTPTimmer create FTPTimer failed");
        return;
    }

    osiTimerStart(gFTPCtx.FTPTimer, gFTP_Param->Tout * 1000);
    OSI_LOGI(0, "FTP# ftp_setFTPTimmer create FTPTimer success");
}

static void ftp_setFTPIdle()
{
    gFTPCtx.ftpState = FTP_IDLE;
    ftp_setFTPTimmer();
}

static void ftp_CommandNotResponse(atCommand_t *cmd)
{
    ftp_data_mode_stop();
    FTP_ClearWaitingEvent();
    AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_NETWORK_TIMOUT));
}

static void ftp_OpenTimer(atCommand_t *cmd)
{
    OSI_LOGI(0, "FTP# FtpOpen timeout");
    atCmdRespCmeError(cmd->engine, ERR_AT_CME_NETWORK_TIMOUT);

    CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
    gFTPCtx.ctrlSocket = 0xFF;
    gFTPCtx.ftp_command = FTP_QUIT;
    gFTPCtx.ftpState = FTP_LOGOUT;
    gFTPCtx.connect = false;
    gFTPCtx.FTPengine = NULL;
}

static void ftp_commandComplete(uint8_t succ, uint8_t *data)
{
    if (gFTPCtx.tx_buf.tmrID != NULL)
    {

        osiTimerDelete(gFTPCtx.tx_buf.tmrID);
        gFTPCtx.tx_buf.tmrID = NULL;
        OSI_LOGI(0, "FTP# ftp_commandComplete delete txtimer success");
    }

    atCmdEngine_t *engine = gFTPCtx.FTPengine;
    if (engine == NULL)
    {
        OSI_LOGI(0, "FTP# ftp_commandComplete engine is null!");
        return;
    }

    if (succ == 0)
    {
        if (data)
            atCmdRespInfoText(engine, (char *)data);
        atCmdRespOK(engine);
    }
    else
    {
        if (data)
            atCmdRespErrorText(engine, (char *)data);
        else
            atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
    }
}

void ftp_Init(void)
{
    memset(&gFTPCtx, 0x00, sizeof(CFW_FTP_CTX_T));
    gFTPCtx.dataSocket = 0xFF;
    gFTPCtx.listenSocket = 0xFF;
    gFTPCtx.ftpState = FTP_LOGOUT;
    gFTPCtx.ctrlSocket = 0xFF;
    gFTPCtx.connected_socket = FTP_SOCKET_NULL;
    ftp_txbufferinit(&gFTPCtx.tx_buf, 0);
    FTP_ClearWaitingEvent();
}

int32_t ftp_sendCommand(SOCKET nSocket, uint8_t *cmd)
{
    OSI_LOGXI(OSI_LOGPAR_S, 0x10004133, "FTP# FTP CMD:%s\n", cmd);
    FTP_STRCAT(cmd, "\r\n");
    return CFW_TcpipSocketSend(nSocket, cmd, FTP_STRLEN(cmd), 0);
}

static void ftp_msg_callback(void *param)
{
    osiEvent_t *pEvent = (osiEvent_t *)param;
    ftp_MainProcess(*pEvent);
    free(pEvent);
}

uint32_t ftp_ctrl_connect(CFW_FTP_CTX_T *ftp_ptr)
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
void ftpget_setWaitDataConnect()
{
    g_waitDataConnect = true;
}

bool ftpget_WaitDataConnect()
{
    return g_waitDataConnect;
}

void ftpget_clearWaitDataConnect()
{
    g_waitDataConnect = false;
}

static int32_t ftp_get_ctrl_connected()
{
    return gFTPCtx.connect;
}

static int32_t ftp_set_ctrl_connect(bool connect)
{
    return gFTPCtx.connect = connect;
}

static uint8_t ftp_connect_socket()
{
    return gFTPCtx.connected_socket;
}

static void ftp_get_connect_error()
{
    ftp_reportFTPState("^FTPGET:1,0");

    if (gFTPCtx.dataSocket != 0xFF)
    {
        gFTPCtx.connected_socket = FTP_SOCKET_GET;
        ftp_get_disconnect();
    }
}

static void ftp_get_disconnect()
{
    CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    gFTPCtx.ftp_command = FTP_ABOR;
}

void ftp_get_connect_finish()
{
    ftp_reportFTPState("^FTPGET:1,1");
    gFTPCtx.connected_socket = FTP_SOCKET_GET;
}

void ftp_get_disconnect_finish()
{
    gFTPCtx.dataSocket = 0xFF;
    ftp_reportFTPState("^FTPGET:2,0");
    gFTPCtx.connected_socket = FTP_SOCKET_NULL;
}

static void ftp_put_connect_error()
{
    ftp_reportFTPState("^FTPPUT:1,0");

    if (gFTPCtx.dataSocket != 0xFF)
    {
        gFTPCtx.connected_socket = FTP_SOCKET_PUT;
        ftp_put_disconnect();
    }
}

void ftp_put_disconnect()
{
    CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    gFTPCtx.ftp_command = FTP_ABOR;
}

void ftp_put_connect_finish()
{
    ftp_reportFTPState("^FTPPUT:1,3072");
    gFTPCtx.connected_socket = FTP_SOCKET_PUT;
}

void ftp_put_disconnect_finish()
{
    gFTPCtx.dataSocket = 0xFF;
    ftp_reportFTPState("^FTPPUT:2,0");
    gFTPCtx.connected_socket = FTP_SOCKET_NULL;
}

int32_t ftp_get_connect(char *filename)
{
    uint8_t command[FTP_STRING_SIZE * 2] = {
        0,
    };
    FTP_STRCAT(command, "RETR ");
    FTP_STRCAT(command, filename);
    ftp_sendCommand(gFTPCtx.ctrlSocket, command);
    ftpget_setWaitDataConnect();
    gFTPCtx.ftp_command = FTP_RETR;
    return 0;
}

int32_t ftp_size(char *filename)
{
    uint8_t command[FTP_STRING_SIZE * 2] = {
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
    if (strlen(ctx->getpath) == 0)
        return false;
    if (gFTPCtx.ftpState != FTP_IDLE)
        return false;
    uint8_t command[200] = {
        0,
    };
    FTP_STRCAT(command, "TYPE I");
    ftp_sendCommand(ctx->ctrlSocket, command);
    ctx->ftp_command = FTP_TYPE;
    ctx->ftpState = GET_FILE;
    ctx->getsize = 0;
    ctx->get_closedelay = false;
    return true;
}

static int32_t ftp_PutStart(CFW_FTP_CTX_T *ctx)
{
    uint8_t command[200] = {
        0,
    };

    if (strlen(ctx->putpath) == 0)
        return false;
    if (ftp_connect_socket() != FTP_SOCKET_NULL)
        return false;
    ctx->ftp_command = FTP_TYPE;
    ctx->ftpState = PUT_FILE;
    FTP_STRCAT(command, "TYPE I");
    ftp_sendCommand(ctx->ctrlSocket, command);
    return true;
}

static int32_t ftp_put_connect(char *filename)
{
    uint8_t command[FTP_STRING_SIZE * 2] = {
        0,
    };
    FTP_STRCAT(command, "STOR ");
    FTP_STRCAT(command, filename);
    ftp_sendCommand(gFTPCtx.ctrlSocket, command);
    gFTPCtx.ftp_command = FTP_STOR;
    return 0;
}

uint8_t ftp_listenPort(struct sockaddr_storage *localaddr)
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
        OSI_LOGXI(OSI_LOGPAR_SI, 0x10004134, "FTP# FTP local addr=%s,port:%d", inet_ntoa(addr->sin_addr), addr->sin_port);
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
        OSI_LOGXI(OSI_LOGPAR_SI, 0x10004134, "FTP# FTP local addr=%s,port:%d", inet6_ntoa(addr->sin6_addr), addr->sin6_port);
        return 1;
    }
#endif

    return 0;
}

static int32_t ftp_get_data_ind(uint32_t len)
{
    int32_t getLen = 0;

    if (gFTPCtx.FTPengine == NULL)
    {
        OSI_LOGE(0, "FTP# ftp engine is unavailable when get data");
        return -1;
    }

    atDispatch_t *ch = atCmdGetDispatch(gFTPCtx.FTPengine);
    if (ch == NULL)
    {
        OSI_LOGE(0, "FTP# failed to get dispatch from ftp engine");
        return -1;
    }

    atDevice_t *device = atDispatchGetDevice(ch);
    if (device == NULL)
    {
        OSI_LOGE(0, "FTP# failed to get device from ftp dispatch");
        return -1;
    }

    int writeAvail = atDeviceWriteAvail(device);
    getLen = (writeAvail < len ? writeAvail : len);
    if (getLen <= 0)
    {
        OSI_LOGE(0, "FTP# failed %d bytes is available in uart", getLen);
        CFW_TcpipSocketDataNotify(gFTPCtx.dataSocket);
        return -1;
    }

    uint8_t *buffer = (uint8_t *)malloc(getLen);
    if (buffer == NULL)
    {
        OSI_LOGI(0x10004154, "FTP# malloc failed");
        return -1;
    }

    uint32_t output = 0;
    int32_t rec_len = CFW_TcpipSocketRecv(gFTPCtx.dataSocket, buffer, getLen, 0);
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
        {
            output = rec_len;
            gFTPCtx.getsize += output;
        }

        OSI_LOGI(0x100052bc, "FTP# output=%ld", output);
        OSI_LOGI(0, "FTP# getsize=%ld", gFTPCtx.getsize);

        uint8_t rsp[50] = {
            0,
        };
        atCmdEngine_t *engine = gFTPCtx.FTPengine;
        if (engine == NULL)
        {
            OSI_LOGI(0, "FTP# ftp_get_data_ind engine is null!");
            free(buffer);
            return -1;
        }
        FTP_SPRINTF(rsp, "^FTPGET:2,%ld", output);
        atCmdRespInfoText(engine, (char *)rsp);

#if 0
        uint32_t sendlen = 1024;
        uint32_t readLen = 0;
        uint8_t *content = &buffer[0];

        while (output > 0)
        {
            if (output > sendlen)
                readLen = sendlen;
            else
                readLen = output;

            atCmdWrite(engine, content, readLen);
            atCmdWrite(engine, "\r\n", FTP_STRLEN("\r\n"));
            output -= readLen;
            content += readLen;
            osiThreadSleep(100);
        }
#else
        atCmdWrite(engine, buffer, output);
        atCmdWrite(engine, "\r\n", FTP_STRLEN("\r\n"));
#endif
        if (rec_len != len)
        {
            CFW_TcpipSocketDataNotify(gFTPCtx.dataSocket);
        }
        else
        {
            if ((0 == CFW_TcpipGetRecvAvailable(gFTPCtx.dataSocket)) && gFTPCtx.get_closedelay)
                CFW_TcpipSocketClose(gFTPCtx.dataSocket);
        }
    }
    else if (rec_len == 0)
        OSI_LOGI(0x10004137, "FTP# rec_len == 0, ignore");
    else
    {
        OSI_LOGI(0x10004138, "FTP# rec_len < 0, transfer complete, abort transefer");
        ftp_get_disconnect();
    }

    free(buffer);
    return 0;
}

uint8_t ftp_errorHandler(enum FTP_COMMAND cmd)
{
    OSI_LOGI(0x10004139, "FTP# ftp_errorHandler cmd=%d\n", cmd);

    switch (cmd)
    {
    case FTP_CONNECT:
    case FTP_USER:
    case FTP_PASS:
        ftp_commandComplete(1, NULL);
        if (ftp_get_ctrl_connected())
        {
            CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
        }

        break;
    case FTP_REST:
    case FTP_RETR:
        OSI_LOGI(0x100052bd, "FTP# error ftp_connect_socket()=%d", ftp_connect_socket());
        if (ftpget_WaitDataConnect())
        {
            ftp_get_connect_error();
            ftpget_clearWaitDataConnect();
        }
        break;
    case FTP_TYPE:
    case FTP_PASV:
    {
        if (GET_FILE == gFTPCtx.ftpState)
            ftp_reportFTPState("^FTPGET:1,0");
        if (PUT_FILE == gFTPCtx.ftpState)
            ftp_reportFTPState("^FTPPUT:1,0");
        gFTPCtx.dataSocket = 0xFF;
        gFTPCtx.connected_socket = FTP_SOCKET_NULL;
        ftp_setFTPIdle();
    }
    break;
    case FTP_STOR:
        ftp_put_connect_error();
        break;
    case FTP_DATA_TRANSFER:
        OSI_LOGI(0, "FTP# error when FTP_DATA_TRANSFER");
        if (gFTPCtx.dataSocket != 0xFF)
        {
            gFTPCtx.connected_socket = FTP_SOCKET_GET;
            ftp_get_disconnect();
        }
        break;
    case FTP_ABOR:
        ftp_setFTPIdle();
        break;
    default:
        ftp_commandComplete(1, NULL);
        ftp_setFTPIdle();
        break;
    }
    return 0;
}

static void ftp_setOffset(int32_t offset)
{
    OSI_LOGI(0x1000413b, "FTP# ftp_setOffset");

    uint8_t command[200] = {0};
    FTP_SPRINTF(command, "REST %ld", offset);
    ftp_sendCommand(gFTPCtx.ctrlSocket, command);
    ftpget_setWaitDataConnect();
    gFTPCtx.ftp_command = FTP_REST;
}

static int32_t ftp_ctrl_data_ind(void)
{
    uint32_t recv_num = 0;
    uint8_t command[FTP_STRING_SIZE * 2] = {
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
    OSI_LOGI(0, "FTP# ftp_command:%d", gFTPCtx.ftp_command);
    if ((FTP_STRNCMP(gRecBuf, ftp_cmd[gFTPCtx.ftp_command - 1].cmd_exp_rst, 3) == 0) || (FTP_STRNCMP(gRecBuf, ftp_cmd[gFTPCtx.ftp_command - 1].cmd_exp_rst + 4, 3) == 0))
    {
        OSI_LOGI(0x1000413e, "FTP# CMD RSP OK");
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
    else if (FTP_STRNCMP(gRecBuf, "500", 3) == 0 || FTP_STRNCMP(gRecBuf, "425", 3) == 0)
    {
        if (gFTPCtx.listenSocket != 0xFF)
            CFW_TcpipSocketClose(gFTPCtx.listenSocket);
        return ftp_errorHandler(gFTPCtx.ftp_command);
    }
    else if ((FTP_STRNCMP(gRecBuf, "150", 3) == 0) &&
             (gFTPCtx.ftp_command == FTP_ABOR) &&
             (gFTPCtx.getsize == gFTPCtx.req_getsize))
    {
        OSI_LOGI(0, "FTP# ignore 150 when download had finished");
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
        break;

    case FTP_SIZE:
        OSI_LOGI(0x10004141, "FTP# FTP_SIZE END\n");
        uint8_t ftpsize[50] = {
            0,
        };
        FTP_SPRINTF(ftpsize, "^FTPSIZE:%s", gRecBuf + 4);
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
        FTP_STRNCPY(numBuf, ptr2 + 1, (ptr1 - ptr2 - 1) > 8 ? 8 : (ptr1 - ptr2 - 1));
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

        FTP_STRNCPY(numBuf, ptr1 + 1, (ptr2 - ptr1 - 1) > 8 ? 8 : (ptr2 - ptr1 - 1));
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

bool ftp_isHandleEvent(osiEvent_t ev)
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

int32_t ftp_data_msg_handler(osiEvent_t ev)
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
        OSI_LOGI(0, "FTP# EV_CFW_TCPIP_REV_DATA_IND data len %d\n", ev.param2);
        if (ftpget_WaitDataConnect())
        {
            ftp_get_connect_finish();
            ftpget_clearWaitDataConnect();
        }
        ftp_get_data_ind(ev.param2);
    }
    else if (EV_CFW_TCPIP_SOCKET_CLOSE_RSP == ev.id)
    {
        OSI_LOGI(0x1000414b, "FTP# EV_CFW_TCPIP_SOCKET_CLOSE_RSP data exit data mode\n");

        /* exit data mode */
        ftp_data_mode_stop();

        if (FTP_isWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0))
        {
            ftp_commandComplete(0, NULL);
            FTP_DelWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
        }
        if (ftpget_WaitDataConnect())
        {
            ftp_get_connect_error();
            ftpget_clearWaitDataConnect();
        }
        if (ftp_connect_socket() == FTP_SOCKET_GET)
        {
            ftp_get_disconnect_finish();
        }
        else if (ftp_connect_socket() == FTP_SOCKET_PUT)
        {
            if (!ftp_put_tx_work(&gFTPCtx.tx_buf))
                ftp_put_disconnect_finish();
        }
        else
        {
            gFTPCtx.dataSocket = 0xFF;
        }

        if (gFTPCtx.ctrlSocket != 0xFF)
        {
            ftp_setFTPIdle();
        }
    }
    else if (EV_CFW_TCPIP_CLOSE_IND == ev.id)
    {
        OSI_LOGI(0x1000414c, "FTP# EV_CFW_TCPIP_CLOSE_IND data\n");
        if (ftp_connect_socket() == FTP_SOCKET_GET && CFW_TcpipGetRecvAvailable(gFTPCtx.dataSocket) != 0)
        {
            OSI_LOGI(0, "FTP# Available received data is not empty when download.");
            gFTPCtx.get_closedelay = true;
            return 0;
        }
        CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    }
    return 0;
}

int32_t ftp_ctrl_msg_handler(osiEvent_t ev)
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
        ftp_reportFTPState("^URCFTP:0");
        ftp_killFTPTimmer();
        ftp_set_ctrl_connect(false);
        ftp_data_mode_stop();
        gFTPCtx.FTPengine = NULL;
    }
    else if (EV_CFW_TCPIP_CLOSE_IND == ev.id)
    {
        OSI_LOGI(0x1000414f, "FTP# EV_CFW_TCPIP_CLOSE_IND ctrl\n");
        CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
        ftp_killFTPTimmer();
        gFTPCtx.ftpState = FTP_LOGOUT;
    }
    else if (EV_CFW_TCPIP_ERR_IND == ev.id)
    {
        OSI_LOGI(0, "FTP# EV_CFW_TCPIP_ERR_IND ctrl\n");

        if (gFTPCtx.ftp_command == FTP_CONNECT ||
            gFTPCtx.ftp_command == FTP_USER ||
            (gFTPCtx.ftp_command == FTP_PASS && gFTPCtx.logined != true))
        {
            OSI_LOGI(0, "FTP# FTP_CONNECT err\n");
            ftp_commandComplete(1, NULL);
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
        ftp_data_mode_stop();
        gFTPCtx.FTPengine = NULL;
    }
    else
    {
        OSI_LOGI(0x10004150, "FTP# event=%d", ev.id);
    }

    return 0;
}

int32_t ftp_listen_msg_handler(osiEvent_t ev)
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

void ftp_MainProcess(osiEvent_t ev)
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

int32_t api_FTPLogin(ip_addr_t *serIp, uint16_t serPort, uint8_t *user, uint8_t *pswd)
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

int32_t api_FTPLogout()
{

    uint8_t command[200] = {
        0,
    };

    if (gFTPCtx.dataSocket != 0xFF)
        CFW_TcpipSocketClose(gFTPCtx.dataSocket);

    if (gFTPCtx.listenSocket != 0xFF)
        CFW_TcpipSocketClose(gFTPCtx.listenSocket);

    FTP_STRCAT(command, "QUIT");
    if (gFTPCtx.ctrlSocket != 0xFF)
    {
        ftp_sendCommand(gFTPCtx.ctrlSocket, command);
        CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
    }

    gFTPCtx.ftp_command = FTP_QUIT;
    gFTPCtx.ftpState = FTP_LOGOUT;
    return ERR_SUCCESS;
}

char *ftp_strstr(const char *s1, const char *s2)
{
    uint8_t len2;
    if (!(len2 = FTP_STRLEN(s2))) // 此种情况下s2不能指向空，否则strlen无法测出长度，这条语句错误
        return (char *)s1;

    for (; *s1; ++s1)
    {
        if (*s1 == *s2 && FTP_STRNCMP(s1, s2, len2))
            return (char *)s1;
    }
    return NULL;
}

struct FTP_Param *ftp_getFTPParam()
{
    return gFTP_Param;
}

static bool ftp_setPUTINFO(CFW_FTP_CTX_T *ctx, const char *path, uint16_t length)
{
    if (sizeof(ctx->putpath) <= length || length == 0)
        return false;

    memset(ctx->putpath, 0x00, length + 1);
    FTP_STRNCPY(ctx->putpath, path, length);
    OSI_LOGXI(OSI_LOGPAR_S, 0x10004155, "FTP# ftp_setPUTINFO %s", path);
    return true;
}

static bool ftp_setGETINFO(CFW_FTP_CTX_T *ctx, const char *path, uint16_t length, int32_t offset, int32_t size, int32_t num)
{
    if (sizeof(ctx->getpath) <= length)
        return false;

    memset(ctx->getpath, 0x00, length + 1);
    FTP_STRNCPY(ctx->getpath, path, length);
    ctx->req_getoffset = offset;
    ctx->req_getsize = size;
    ctx->getparam = num;
    ctx->getsize = 0;
    ctx->get_closedelay = false;
    OSI_LOGXI(OSI_LOGPAR_SII, 0x10004156, "FTP# ftp_setGETINFO path=%s, offset=%d,size=%d", path, offset, size);
    return true;
}

static void ftp_gethostport(char *url, uint8_t *host, uint16_t *port)
{
    char *p;
    ip_addr_t addr;

    if (ipaddr_aton(url, &addr))
    {
        if (IP_IS_V6(&addr))
        {
            FTP_STRCPY(host, url);
            *port = 21;
        }
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

void AT_FTP_CmdFunc_OPEN(atCommand_t *pParam)
{
    uint32_t err;
    bool paramok = true;
    uint8_t tmpString[60] = {
        0x00,
    };
    char *url = NULL;
    char *username = NULL;
    char *passwd = NULL;

    ip_addr_t nIpAddr;
    struct FTP_Param ftp_param = {{}, 0, {}, {}, 1, 30, 0};
    uint8_t nSim = atCmdGetSim(pParam->engine);
    uint8_t nCid = ftp_getCID(nSim);

    if (nCid == 0xFF)
    {
        OSI_LOGI(0, "FTP# FTPOPEN get CID error");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
    }
    else
    {
        OSI_LOGI(0, "FTP# FTPOPEN SIM = %d CID = %d", nSim, nCid);
    }

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        if (gFTPCtx.FTPengine != NULL)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (ftp_get_ctrl_connected())
        {
            OSI_LOGI(0x100043f2, "FTP# FTPOPEN ftp have login");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (pParam->param_count != 6)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        url = (char *)atParamStr(pParam->params[0], &paramok);
        if ((paramok != true) || (url == NULL) || strlen(url) >= FTP_STRING_SIZE)
        {
            OSI_LOGI(0x100043ec, "FTP# FTPOPEN get url error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        ftp_gethostport(url, ftp_param.host, &ftp_param.port);

        username = (char *)atParamStr(pParam->params[1], &paramok);
        if ((paramok != true) || (username == NULL) || strlen(username) >= FTP_STRING_SIZE)
        {
            OSI_LOGI(0x100043ed, "FTP# FTPOPEN get username error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        FTP_STRCPY(ftp_param.username, username);

        passwd = (char *)atParamStr(pParam->params[2], &paramok);
        if ((paramok != true) || (passwd == NULL) || strlen(passwd) >= FTP_STRING_SIZE)
        {
            OSI_LOGI(0x100043ee, "FTP# FTPOPEN get password error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        FTP_STRCPY(ftp_param.passwd, passwd);

        ftp_param.mode = atParamUint(pParam->params[3], &paramok);
        if (paramok != true || (ftp_param.mode != 0 && ftp_param.mode != 1))
        {
            OSI_LOGI(0x100043ef, "FTP# FTPOPEN get mode error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        ftp_param.Tout = atParamUint(pParam->params[4], &paramok);
        if (paramok != true || (ftp_param.Tout < 5 || ftp_param.Tout > 180))
        {
            OSI_LOGI(0x100043f0, "FTP# FTPOPEN get timeout error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        ftp_param.FTPtype = atParamUint(pParam->params[5], &paramok);
        if (paramok != true || (ftp_param.FTPtype != 0 && ftp_param.FTPtype != 1))
        {
            OSI_LOGI(0x100043f1, "FTP# FTPOPEN get ftp type error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (gFTP_Param == NULL)
        {
            gFTP_Param = (struct FTP_Param *)malloc(sizeof(struct FTP_Param));
        }

        memcpy(gFTP_Param, &ftp_param, sizeof(struct FTP_Param));

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
            gFTPCtx.FTPengine = NULL;
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else if (err == RESOLV_COMPLETE)
        {
            if (ERR_SUCCESS == api_FTPLogin(&nIpAddr, gFTP_Param->port, gFTP_Param->username, gFTP_Param->passwd))
                AT_CMD_RETURN(atCmdSetTimeoutHandler(pParam->engine, 100 * 1000, ftp_OpenTimer));
            else
            {
                gFTPCtx.FTPengine = NULL;
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else if (err == RESOLV_QUERY_QUEUED)
        {
            AT_CMD_RETURN(atCmdSetTimeoutHandler(pParam->engine, 100 * 1000, ftp_OpenTimer));
        }
    }
    break;
    case AT_CMD_READ:
        OSI_LOGI(0x1000415b, "FTP# FTPOPEN READ");
        if (gFTPCtx.ftpState == FTP_LOGOUT || gFTPCtx.FTPengine != (atCmdEngine_t *)pParam->engine)
        {
            FTP_STRCPY(tmpString, "^FTPOPEN:0");
            atCmdRespInfoText(pParam->engine, (char *)tmpString);
            atCmdRespOK(pParam->engine);
        }
        else
        {
            FTP_STRCPY(tmpString, "^FTPOPEN:1");
            atCmdRespInfoText(pParam->engine, (char *)tmpString);
            atCmdRespOK(pParam->engine);
        }
        break;
    case AT_CMD_TEST:
        FTP_STRCPY(tmpString, "^FTPOPEN:<url>,<username>,<password>,<mode>,<tout>,<type>");
        atCmdRespInfoText(pParam->engine, (char *)tmpString);
        atCmdRespOK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x1000415c, "FTP# FTPOPEN ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

void AT_FTP_CmdFunc_SIZE(atCommand_t *pParam)
{
    bool paramok = true;
    char *filename = NULL;

    switch (pParam->type)
    {
    case AT_CMD_SET:
        if (gFTPCtx.FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (!ftp_get_ctrl_connected())
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (pParam->param_count != 1)
        {
            OSI_LOGI(0x100043f3, "FTP# FTPSIZE get param count error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        filename = (char *)atParamStr(pParam->params[0], &paramok);
        if (!paramok || (strlen(filename) == 0) || (strlen(filename) >= FTP_STRING_SIZE))
        {
            OSI_LOGI(0x100043f4, "FTP# FTPSIZE get filename error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (gFTPCtx.ftpState != FTP_IDLE)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        ftp_size(filename);
        AT_SetAsyncTimerMux(pParam->engine, 60);
        break;
    default:
        OSI_LOGI(0x1000415e, "FTP# FTPSIZE ERROR");
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        break;
    }
}

void AT_FTP_CmdFunc_GETSET(atCommand_t *pParam)
{
    char *filepath = NULL;
    int32_t offset = 0;
    int32_t size = 0;

    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;

        if (gFTPCtx.FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (pParam->param_count < 1 || pParam->param_count > 3)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (!ftp_get_ctrl_connected())
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (pParam->param_count >= 1)
        {
            filepath = (char *)atParamStr(pParam->params[0], &paramok);
            if (!paramok || (strlen(filepath) == 0) || (strlen(filepath) >= FTP_STRING_SIZE))
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (pParam->param_count >= 2)
        {
            offset = atParamInt(pParam->params[1], &paramok);
            if (!paramok || offset < 0)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (pParam->param_count >= 3)
        {
            size = atParamInt(pParam->params[2], &paramok);
            if (!paramok || size <= 0)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (ftp_setGETINFO(&gFTPCtx, filepath, FTP_STRLEN(filepath), offset, size, pParam->param_count))
            atCmdRespOK(pParam->engine);
        else
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
    else if (AT_CMD_READ == pParam->type)
    {
        uint8_t info[300] = {
            0x00,
        };

        if (strlen(gFTPCtx.getpath) != 0)
        {
            if (gFTPCtx.getparam == 1)
                FTP_SPRINTF(info, "^FTPGETSET:%s", gFTPCtx.getpath);
            if (gFTPCtx.getparam == 2)
                FTP_SPRINTF(info, "^FTPGETSET:%s,%ld", gFTPCtx.getpath, gFTPCtx.req_getoffset);
            if (gFTPCtx.getparam == 3)
                FTP_SPRINTF(info, "^FTPGETSET:%s,%ld,%ld", gFTPCtx.getpath, gFTPCtx.req_getoffset, gFTPCtx.req_getsize);

            atCmdRespInfoText(pParam->engine, (char *)info);
            atCmdRespOK(pParam->engine);
        }
        else
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        atCmdRespInfoText(pParam->engine, "^FTPGETSET:<filename>, [offset, [size]]");
        atCmdRespOK(pParam->engine);
        return;
    }
    else
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    return;
}

void AT_FTP_CmdFunc_GET(atCommand_t *pParam)
{
    bool paramok = true;

    switch (pParam->type)
    {
    case AT_CMD_SET:

        if (gFTPCtx.FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (!ftp_get_ctrl_connected())
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (pParam->param_count > 2)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        uint32_t mode = atParamUint(pParam->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (mode == 1)
        {
            if (pParam->param_count != 1)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            if (ftp_GetStart(&gFTPCtx))
                atCmdRespOK(pParam->engine);
            else
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else if (mode == 2)
        {
            if (pParam->param_count != 2)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            uint32_t reqlength = atParamUint(pParam->params[1], &paramok);
            if (!paramok)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            if (reqlength == 0)
            {
                if (ftp_connect_socket() == FTP_SOCKET_GET)
                {
                    FTP_AddWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
                    ftp_get_disconnect();
                }
                else
                {
                    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            else
            {
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        else
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        break;
    default:
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

void AT_FTP_CmdFunc_PUTSET(atCommand_t *pParam)
{
    if (AT_CMD_SET == pParam->type)
    {
        if (gFTPCtx.FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (!ftp_get_ctrl_connected())
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (pParam->param_count != 1)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        bool paramok = true;
        const char *filepath = atParamStr(pParam->params[0], &paramok);
        if (!paramok || (strlen(filepath) == 0) || (strlen(filepath) >= FTP_STRING_SIZE))
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (ftp_setPUTINFO(&gFTPCtx, filepath, FTP_STRLEN(filepath)))
            atCmdRespOK(pParam->engine);
        else
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
    else if (AT_CMD_READ == pParam->type)
    {
        uint8_t info[300] = {
            0x00,
        };
        if (strlen(gFTPCtx.putpath) != 0)
        {
            FTP_SPRINTF(info, "^FTPPUTSET:%s", gFTPCtx.putpath);
            atCmdRespInfoText(pParam->engine, (char *)info);
            atCmdRespOK(pParam->engine);
        }
        else
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        atCmdRespInfoText(pParam->engine, "^FTPPUTSET:<filename>");
        atCmdRespOK(pParam->engine);
        return;
    }
    else
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    return;
}

void AT_FTP_CmdFunc_PUT(atCommand_t *pParam)
{
    bool paramok = true;
    OSI_LOGI(0x100043f5, "FTP# in FTPPUT gFTPCtx.ftpState %d", gFTPCtx.ftpState);

    switch (pParam->type)
    {
    case AT_CMD_SET:
        if (gFTPCtx.FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (!ftp_get_ctrl_connected())
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (pParam->param_count > 2)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        uint32_t mode = atParamUint(pParam->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (mode == 1)
        {
            if (pParam->param_count != 1)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            if (ftp_PutStart(&gFTPCtx))
                atCmdRespOK(pParam->engine);
            else
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        else if (mode == 2)
        {
            if (gFTPCtx.dataSocket == 0xFF || gFTPCtx.ftp_command != FTP_DATA_TRANSFER)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

            if (pParam->param_count != 2)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            uint32_t req_length = atParamUintInRange(pParam->params[1], 0, FTP_SEND_MAX_SIZE, &paramok);
            if (!paramok)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            if (req_length == 0)
            {
                if (ftp_connect_socket() == FTP_SOCKET_PUT)
                {
                    FTP_AddWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
                    ftp_put_disconnect();
                    return;
                }
                else
                {
                    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
                }
            }

            ftp_txbufferinit(&gFTPCtx.tx_buf, req_length);
            ftp_data_mode_start();
            FTP_AddWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
            atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, ftp_CommandNotResponse);
        }
        else
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        break;
    case AT_CMD_TEST:
        atCmdRespInfoText(pParam->engine, "^FTPPUT: mode[,<reqlength>]");
        atCmdRespOK(pParam->engine);
        break;
    default:
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

void AT_FTP_CmdFunc_CLOSE(atCommand_t *pParam)
{
    if (gFTPCtx.ftpState == FTP_LOGOUT)
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

    if (gFTPCtx.FTPengine != (atCmdEngine_t *)pParam->engine)
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

    switch (pParam->type)
    {
    case AT_CMD_EXE:
        api_FTPLogout();
        FTP_AddWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, 0);
        atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, ftp_CommandNotResponse);
        break;
    default:
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        break;
    }
}

#endif
