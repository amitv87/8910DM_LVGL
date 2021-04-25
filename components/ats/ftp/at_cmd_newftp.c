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

#include "ats_config.h"

#ifdef CONFIG_AT_NEWFTP_SUPPORT

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "osi_api.h"
#include "osi_log.h"
#include "cfw.h"
#include "cfw_errorcode.h"
#include "at_engine.h"
#include "at_command.h"
#include "at_response.h"

#include "sockets.h"
#include "ftp_utils.h"
#include "ftp_protocol.h"

typedef enum _atFtpCmd_t
{
    CMD_NONE = 0,
    CMD_OPEN,
    CMD_CLOSE,
    CMD_SIZE,
    CMD_GET,
    CMD_PUT,
    CMD_END = 6,
} atFtpCmd_t;

typedef struct _atFtpTimer_t
{
    osiTimer_t *tmr;
    uint32_t timeout;
    bool start;
} atFtpTimer_t;

#define FTP_SEND_MAX_SIZE 3072

typedef struct _ftp_txbuf_t
{
    uint8_t buffer[FTP_SEND_MAX_SIZE];
    uint16_t len;
    uint16_t request;
} ftp_txbuf_t;

typedef struct _atFtpCtx_t
{
    atFtpCmd_t FTPCmd;
    atFtpTimer_t AutoLogout;
    atCmdEngine_t *FTPengine;
    bool login;
    bool getstop;
    char getpath[FTP_STRING_SIZE];
    uint32_t req_getoffset, req_getsize, getsize, getparam;
    bool req_getDelay;
    bool putstart, putcancel;
    char putpath[FTP_STRING_SIZE];
    ftp_txbuf_t txbuf;
} atFtpCtx_t;

#define FTP_CMD_CHECK(m)                                          \
    do                                                            \
    {                                                             \
        if (gAtFtpCtx->FTPCmd != m)                               \
        {                                                         \
            FTPLOGI(FTPLOG_AT, "[%s] pass the event!", #m);       \
            ftp_msg_handler(nEventId, nResult, nParam1, nParam2); \
            return;                                               \
        }                                                         \
    } while (0)

typedef void (*ftp_timeout_t)(atCommand_t *cmd);
static void ftp_QuitTimeout(atCommand_t *cmd);
static void ftp_none_MsgHandler(void *param);
static void ftp_open_MsgHandler(void *param);
static void ftp_close_MsgHandler(void *param);
static void ftp_size_MsgHandler(void *param);
static void ftp_get_MsgHandler(void *param);
static void ftp_put_MsgHandler(void *param);
static int ftp_UartRecv(void *param, uint8_t *data, unsigned length);
static void ftp_msg_handler(uint32_t nEventId, uint32_t nResult, uint32_t nParam1, uint32_t nParam2);

static atFtpCtx_t *gAtFtpCtx = NULL;
static ftp_timeout_t gAtTimeoutHandler[CMD_END] =
    {
        NULL,
        ftp_QuitTimeout,
        ftp_QuitTimeout,
        ftp_QuitTimeout,
        ftp_QuitTimeout,
        ftp_QuitTimeout,
};

static osiCallback_t gAtMsgHandler[CMD_END] =
    {
        ftp_none_MsgHandler,
        ftp_open_MsgHandler,
        ftp_close_MsgHandler,
        ftp_size_MsgHandler,
        ftp_get_MsgHandler,
        ftp_put_MsgHandler,
};

static void ftp_free()
{
    if (gAtFtpCtx->AutoLogout.tmr)
    {
        if (gAtFtpCtx->AutoLogout.start)
        {
            osiTimerStop(gAtFtpCtx->AutoLogout.tmr);
        }

        osiTimerDelete(gAtFtpCtx->AutoLogout.tmr);
        gAtFtpCtx->AutoLogout.tmr = NULL;
    }

    memset(gAtFtpCtx, 0, sizeof(atFtpCtx_t));
    free(gAtFtpCtx);
    gAtFtpCtx = NULL;
}

static void ftp_stop_autologout()
{
    if (gAtFtpCtx->AutoLogout.tmr && gAtFtpCtx->AutoLogout.start)
    {
        if (osiTimerStop(gAtFtpCtx->AutoLogout.tmr))
        {
            gAtFtpCtx->AutoLogout.start = false;
            FTPLOGI(FTPLOG_AT, "stop autologout timer success.");
        }
        else
        {
            FTPLOGI(FTPLOG_AT, "stop autologout timer failure.");
        }
    }
}

static void ftp_start_autologout()
{
    if (gAtFtpCtx->AutoLogout.tmr && !gAtFtpCtx->AutoLogout.start)
    {
        if (osiTimerStart(gAtFtpCtx->AutoLogout.tmr, gAtFtpCtx->AutoLogout.timeout * 1000))
        {
            gAtFtpCtx->AutoLogout.start = true;
            FTPLOGI(FTPLOG_AT, "set autologout timer success.");
        }
        else
        {
            FTPLOGI(FTPLOG_AT, "set autologout timer failure.");
        }
    }
}

static void ftp_setCmd(uint32_t cmd)
{
    gAtFtpCtx->FTPCmd = cmd;

    if (cmd == CMD_NONE && gAtFtpCtx->login)
        ftp_start_autologout();
    else
        ftp_stop_autologout();
}

static void ftp_autologout_callback(void *param)
{
    FTPLOGI(FTPLOG_AT, "enter ftp autologout timer callback");

    if (gAtFtpCtx && gAtFtpCtx->login && gAtFtpCtx->FTPCmd == CMD_NONE)
    {
        FTPLib_logout();
    }
}

static void ftp_ReportInfo(char *data)
{
    atCmdEngine_t *engine = (atCmdEngine_t *)gAtFtpCtx->FTPengine;

    if (engine == NULL)
    {
        FTPLOGI(FTPLOG_AT, "engine is null in ftp_reportFtpInfo");
        return;
    }

    if (data)
        atCmdRespInfoText(engine, data);
}

static void ftp_CommandComplete(bool succ, int code)
{
    atCmdEngine_t *engine = (atCmdEngine_t *)gAtFtpCtx->FTPengine;

    if (engine == NULL || gAtFtpCtx->FTPCmd == CMD_NONE)
    {
        FTPLOGI(FTPLOG_AT, "ignore in ftp_CommandComplete");
        return;
    }

    if (succ)
    {
        atCmdRespOK(engine);
    }
    else
    {
        atCmdRespCmeError(engine, code);
    }

    ftp_setCmd(CMD_NONE);
}

static void ftp_QuitTimeout(atCommand_t *cmd)
{
    gAtFtpCtx->login = false;
    FTPLib_uninit();
    ftp_ReportInfo("^URCFTP:0");
    ftp_free();
}

static void ftp_AtTimer(atCommand_t *cmd)
{
    FTPLOGI(FTPLOG_AT, "at timeout, cmd : %s", cmd->desc->name);
    if (gAtFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "gAtFtpCtx is null in ftp_AtTimer.");
        return;
    }

    atFtpCmd_t ATcmd = gAtFtpCtx->FTPCmd;

    ftp_CommandComplete(false, ERR_AT_CME_NETWORK_TIMOUT);

    if (gAtTimeoutHandler[ATcmd] != NULL)
        gAtTimeoutHandler[ATcmd](cmd);
}

static void ftp_none_MsgHandler(void *param)
{
    osiEvent_t *ev = (osiEvent_t *)param;

    if (!ev)
        return;

    uint32_t nEventId = ev->id;
    uint32_t nResult = ev->param1;
    uint32_t nParam1 = ev->param2;
    uint32_t nParam2 = ev->param3;
    free(ev);

    FTPLOGI(FTPLOG_AT, "[FTPNONE] Msg handler receive event[%s] with result[%d], param1:0x%x, param2:0x%x",
            ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "[FTPNONE] Ignore the event!");
        return;
    }

    if (nEventId == FTP_MSG_PUT_STOP && gAtFtpCtx->putstart)
    {
        gAtFtpCtx->putstart = false;
        ftp_ReportInfo("^FTPPUT:2,0");
    }

    if (nEventId == FTP_MSG_LOGOUT ||
        nEventId == FTP_MSG_DISCONNECT)
    {
        gAtFtpCtx->login = false;
        FTPLib_uninit();
        ftp_ReportInfo("^URCFTP:0");
        ftp_free();
    }

    return;
}

static void ftp_open_MsgHandler(void *param)
{
    osiEvent_t *ev = (osiEvent_t *)param;

    if (!ev)
        return;

    uint32_t nEventId = ev->id;
    uint32_t nResult = ev->param1;
    uint32_t nParam1 = ev->param2;
    uint32_t nParam2 = ev->param3;
    free(ev);

    FTPLOGI(FTPLOG_AT, "[FTPOPEN] Msg handler receive event[%s] with result[%d], param1:%d, param2:%d",
            ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "[FTPOPEN] Ignore the event!");
        return;
    }

    FTP_CMD_CHECK(CMD_OPEN);

    if (nEventId == FTP_MSG_LOGIN)
    {
        if (nResult == FTP_RET_SUCCESS)
        {
            gAtFtpCtx->login = true;
            ftp_CommandComplete(true, 0);
        }
        else
        {
            FTPLib_uninit();
            ftp_CommandComplete(false, ERR_AT_CME_EXE_FAIL);
            ftp_free();
        }
    }

    return;
}

static void ftp_close_MsgHandler(void *param)
{
    osiEvent_t *ev = (osiEvent_t *)param;

    if (!ev)
        return;

    uint32_t nEventId = ev->id;
    uint32_t nResult = ev->param1;
    uint32_t nParam1 = ev->param2;
    uint32_t nParam2 = ev->param3;
    free(ev);

    FTPLOGI(FTPLOG_AT, "[FTPCLOSE] Msg handler receive event[%s] with result[%d], param1:0x%x, param2:0x%x",
            ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "[FTPCLOSE] Ignore the event!");
        return;
    }

    FTP_CMD_CHECK(CMD_CLOSE);

    if (nEventId == FTP_MSG_LOGOUT)
    {
        gAtFtpCtx->login = false;
        FTPLib_uninit();
        ftp_CommandComplete(true, 0);
        ftp_free();
    }

    return;
}

static void ftp_size_MsgHandler(void *param)
{
    osiEvent_t *ev = (osiEvent_t *)param;

    if (!ev)
        return;

    uint32_t nEventId = ev->id;
    uint32_t nResult = ev->param1;
    uint32_t nParam1 = ev->param2;
    uint32_t nParam2 = ev->param3;
    free(ev);

    FTPLOGI(FTPLOG_AT, "[FTPSIZE] Msg handler receive event[%s] with result[%d], param1:%d, param2:%d",
            ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "[FTPSIZE] Ignore the event!");
        return;
    }

    FTP_CMD_CHECK(CMD_SIZE);

    if (nEventId == FTP_MSG_SIZE)
    {
        if (nResult == FTP_RET_SUCCESS)
        {
            char ftpsize[32] = {0};
            sprintf(ftpsize, "^FTPSIZE:%ld", nParam1);
            ftp_ReportInfo(ftpsize);
            ftp_CommandComplete(true, 0);
        }
        else
        {
            ftp_CommandComplete(false, ERR_AT_CME_EXE_FAIL);
        }
    }

    return;
}

static void ftp_get_MsgHandler(void *param)
{
    static bool uartBlock = false;
    osiEvent_t *ev = (osiEvent_t *)param;

    if (!ev)
        return;

    uint32_t nEventId = ev->id;
    uint32_t nResult = ev->param1;
    uint32_t nParam1 = ev->param2;
    uint32_t nParam2 = ev->param3;
    free(ev);

    if (!uartBlock)
        FTPLOGI(FTPLOG_AT, "[FTPGET] Msg handler receive event[%s] with result[%d], param1:0x%x, param2:0x%x",
                ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtFtpCtx == NULL || !gAtFtpCtx->login)
    {
        FTPLOGI(FTPLOG_AT, "[FTPGET] Ignore the event!");
        return;
    }

    FTP_CMD_CHECK(CMD_GET);

    if (nEventId == FTP_MSG_CONNECT_DATA)
    {
        if (nResult == FTP_RET_SUCCESS)
        {
            ftp_ReportInfo("^FTPGET:1,1");
        }
        else
        {
            ftp_ReportInfo("^FTPGET:1,0");
        }
    }
    else if (nEventId == FTP_MSG_TRANSFER_DATA)
    {
        uint8_t *pbuffer = NULL;
        int32_t ByteNum = 0;
        uint32_t getLen = 0;
        char rsp[50] = {0};
        uint32_t buflen = (uint32_t)nParam2;

        if (buflen == 0)
            return;

        if (gAtFtpCtx->getstop)
        {
            FTPLOGI(FTPLOG_AT, "discard this ftp download data");
            gAtFtpCtx->req_getDelay = false;
        }
        else
        {
            atDispatch_t *ch = atCmdGetDispatch(gAtFtpCtx->FTPengine);
            if (ch == NULL)
            {
                FTPLOGI(FTPLOG_AT, "failed to get dispatch from ftp engine");
                return;
            }

            atDevice_t *device = atDispatchGetDevice(ch);
            if (device == NULL)
            {
                FTPLOGI(FTPLOG_AT, "failed to get device from ftp dispatch");
                return;
            }

            int writeAvail = atDeviceWriteAvail(device);
            getLen = (writeAvail < buflen ? writeAvail : buflen);
            if (getLen <= 0)
            {
                if (!uartBlock)
                    FTPLOGI(FTPLOG_AT, "%d bytes is available in uart", getLen);
                uartBlock = true;
                gAtFtpCtx->req_getDelay = true;
                ftp_msg_handler(FTP_MSG_TRANSFER_DATA, FTP_RET_SUCCESS, (uint32_t)NULL, buflen);
                return;
            }

            uartBlock = false;
            pbuffer = malloc(getLen);
            if (pbuffer != NULL)
            {
                ByteNum = FTPLib_get(pbuffer, getLen);
                if (ByteNum > 0)
                {
                    sprintf(rsp, "^FTPGET:2,%ld", ByteNum);
                    ftp_ReportInfo(rsp);
                    atCmdWrite(gAtFtpCtx->FTPengine, pbuffer, ByteNum);
                    ftp_ReportInfo("\r\n");
                    free(pbuffer);
                }
                else
                {
                    FTPLOGI(FTPLOG_AT, "failed to get download data from ftplib");
                    gAtFtpCtx->req_getDelay = false;
                    free(pbuffer);
                    return;
                }
            }

            if (ByteNum != buflen)
            {
                gAtFtpCtx->req_getDelay = true;
                ftp_msg_handler(FTP_MSG_TRANSFER_DATA, FTP_RET_SUCCESS, (uint32_t)NULL, buflen - ByteNum);
            }
            else
            {
                gAtFtpCtx->req_getDelay = false;
            }
        }
    }
    else if (nEventId == FTP_MSG_GET)
    {
        if (gAtFtpCtx->req_getDelay)
        {
            FTPLOGI(FTPLOG_AT, "delay to close the transfer when data is available");
            ftp_msg_handler(nEventId, nResult, nParam1, nParam2);
            return;
        }

        if (gAtFtpCtx->getstop)
        {
            atCmdRespOK(gAtFtpCtx->FTPengine);
            gAtFtpCtx->getstop = false;
        }

        if (nResult == FTP_RET_CMD_TIMEOUT || nResult == FTP_RET_CMD_BROKEN)
        {
            ftp_ReportInfo("^FTPGET:1,0");
            ftp_ReportInfo("^FTPGET:2,0");
            gAtFtpCtx->login = false;
            FTPLib_uninit();
            ftp_ReportInfo("^URCFTP:0");
            ftp_free();
        }
        else if (nResult == FTP_RET_CMD_REP_ERR || nResult == FTP_RET_INTERNAL_ERR ||
                 nResult == FTP_RET_DATA_CONNECT_FAIL || nResult == FTP_RET_DATA_BROKEN)
        {
            ftp_ReportInfo("^FTPGET:1,0");
            ftp_ReportInfo("^FTPGET:2,0");
            ftp_setCmd(CMD_NONE);
        }
        else
        {
            ftp_ReportInfo("^FTPGET:2,0");
            ftp_setCmd(CMD_NONE);
        }
    }
    else
    {
    }
}

static void ftp_put_MsgHandler(void *param)
{
    osiEvent_t *ev = (osiEvent_t *)param;

    if (!ev)
        return;

    uint32_t nEventId = ev->id;
    uint32_t nResult = ev->param1;
    uint32_t nParam1 = ev->param2;
    uint32_t nParam2 = ev->param3;
    free(ev);

    FTPLOGI(FTPLOG_AT, "[FTPPUT] Msg handler receive event[%s] with result[%d], param1:0x%x, param2:0x%x",
            ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtFtpCtx == NULL || !gAtFtpCtx->login)
    {
        FTPLOGI(FTPLOG_AT, "[FTPPUT] Ignore the event!");
        return;
    }

    FTP_CMD_CHECK(CMD_PUT);

    if (nEventId == FTP_MSG_PUT_START)
    {
        if (nResult == FTP_RET_SUCCESS)
        {
            gAtFtpCtx->putstart = true;
            ftp_CommandComplete(true, 0);
            ftp_ReportInfo("^FTPPUT:1,3072");
        }
        else
        {
            ftp_CommandComplete(false, ERR_AT_CME_EXE_FAIL);
            ftp_ReportInfo("^FTPPUT:1,0");
        }
    }
    else if (nEventId == FTP_MSG_TRANSFER_DATA)
    {
        if (nResult == FTP_RET_SUCCESS)
            ftp_CommandComplete(true, 0);
        else
            ftp_CommandComplete(false, ERR_AT_CME_EXE_FAIL);
    }
    else if (nEventId == FTP_MSG_PUT_STOP)
    {
        gAtFtpCtx->putstart = false;

        if (gAtFtpCtx->putcancel)
            ftp_CommandComplete(true, 0);
        else
            ftp_CommandComplete(false, ERR_AT_CME_EXE_FAIL);

        ftp_ReportInfo("^FTPPUT:2,0");
    }
    else
    {
    }
}

static void ftp_msg_handler(uint32_t nEventId, uint32_t nResult, uint32_t nParam1, uint32_t nParam2)
{
    if (gAtFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "gAtFtpCtx is null in ftp_msg_handler.");
        return;
    }

    if (nEventId < FTP_MSG_BASE)
    {
        FTPLOGI(FTPLOG_AT, "invalid eventID.");
        return;
    }

    if (gAtMsgHandler[gAtFtpCtx->FTPCmd] != NULL)
    {
        osiEvent_t *ev = (osiEvent_t *)malloc(sizeof(osiEvent_t));
        ev->id = nEventId;
        ev->param1 = (uint32_t)nResult;
        ev->param2 = (uint32_t)nParam1;
        ev->param3 = (uint32_t)nParam2;

        osiThreadCallback(atEngineGetThreadId(), (osiCallback_t)gAtMsgHandler[gAtFtpCtx->FTPCmd], (void *)ev);
    }
}

static bool ftp_gethostport(char *url, char **host, char **port)
{
    char *p = NULL;
    ip_addr_t addr;

    static char s_host[FTP_STRING_SIZE] = {0};
    static char s_port[FTP_STRING_SIZE] = {0};
    memset(s_host, 0, FTP_STRING_SIZE);
    memset(s_port, 0, FTP_STRING_SIZE);

    if (url[0] == '[' && (p = strstr(url, "]:"))) //[3ffe:2a00:100:7031::31]:1021
    {
        strncpy(s_host, url + 1, p - url - 1);
        strcpy(s_port, p + 2);
        goto CHECK1;
    }
    else if (ipaddr_aton(url, &addr)) // "3ffe:2a00:100:7031::31" or "192.168.10.23"
    {
        strcpy(s_host, url);
        strcpy(s_port, "21");
        goto EXIT;
    }
    else if (strstr(url, ".com") || strstr(url, ".net") || strstr(url, ".cn") || strstr(url, ".org"))
    {
        if ((p = strstr(url, ":"))) // "baidu.com:80"
        {
            strncpy(s_host, url, p - url);
            strcpy(s_port, p + 1);
            goto CHECK2;
        }
        else // "baidu.com"
        {
            strcpy(s_host, url);
            strcpy(s_port, "21");
            goto EXIT;
        }
    }
    else if ((p = strstr(url, ":"))) //"192.168.10.23:1021"
    {
        strncpy(s_host, url, p - url);
        strcpy(s_port, p + 1);
        goto CHECK1;
    }
    else
    {
        *host = NULL;
        *port = NULL;
        return false;
    }

CHECK1:
    if (!ipaddr_aton(s_host, &addr))
    {
        *host = NULL;
        *port = NULL;
        return false;
    }
CHECK2:
    if (atoi(s_port) <= 0)
    {
        *host = NULL;
        *port = NULL;
        return false;
    }
EXIT:
    *host = s_host;
    *port = s_port;
    return true;
}

static void ftp_txbufferinit(ftp_txbuf_t *tx, uint32_t request)
{
    tx->len = 0;
    tx->request = request;
}

static int32_t ftp_txbufpush(ftp_txbuf_t *tx, uint8_t *data, uint16_t size)
{
    if (size + tx->len > tx->request)
        size = tx->request - tx->len;

    memcpy(tx->buffer + tx->len, data, size);
    tx->len += size;
    return tx->request - tx->len;
}

#define CHAR_ESC 0x1b

static void ftp_data_mode_start()
{
    atCmdRespOutputPrompt(gAtFtpCtx->FTPengine);
    atCmdSetBypassMode(gAtFtpCtx->FTPengine, (atCmdBypassCB_t)ftp_UartRecv, NULL);
}

static void ftp_data_mode_stop()
{
    atCmdSetLineMode(gAtFtpCtx->FTPengine);
}

static bool ftp_UartEscCheck(uint8_t *data, unsigned length)
{
    int i = 0;

    while (i < length)
    {
        if (data[i] == CHAR_ESC)
        {
            return true;
        }

        i++;
    }

    return false;
}

static int ftp_UartRecv(void *param, uint8_t *data, unsigned length)
{
    FTPLOGI(FTPLOG_AT, "Uart input data length = %d", length);

    if (ftp_UartEscCheck(data, length))
    {
        FTPLOGI(FTPLOG_AT, "Uart will exit by ESC char");
        ftp_CommandComplete(true, 0);
        ftp_data_mode_stop();
        return length;
    }

    int32_t left = ftp_txbufpush(&(gAtFtpCtx->txbuf), data, length);
    if (left == 0)
    {
        ftp_data_mode_stop();
        atCmdWrite(gAtFtpCtx->FTPengine, gAtFtpCtx->txbuf.buffer, gAtFtpCtx->txbuf.len);
        if (FTPLib_put(gAtFtpCtx->txbuf.buffer, gAtFtpCtx->txbuf.len) != FTP_RET_SUCCESS)
        {
            ftp_CommandComplete(false, ERR_AT_CME_EXE_FAIL);
        }
    }

    return length;
}

void AT_FTP_CmdFunc_OPEN(atCommand_t *pParam)
{
    bool ret = false;
    bool paramok = true;
    ftp_ret_t ftpRet = FTP_RET_SUCCESS;

    char *url = NULL;
    char *host = NULL;
    char *port = NULL;
    char *username = NULL;
    char *passwd = NULL;
    char tmpString[60] = {0};

    uint32_t mode = 0xff;
    uint32_t Tout = 0xff;
    uint32_t type = 0xff;

    ftpconfig_t cfg;

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        if (netif_default == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, no default netif.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        if (gAtFtpCtx != NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, ftp is already open.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (pParam->param_count != 6)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        url = (char *)atParamStr(pParam->params[0], &paramok);
        if (paramok != true || strlen(url) >= FTP_STRING_SIZE)
        {
            FTPLOGI(FTPLOG_AT, "at error, get url error.");
            RETURN_CME_ERR(pParam->engine, paramok ? ERR_AT_CME_TEXT_LONG : ERR_AT_CME_PARAM_INVALID);
        }
        ret = ftp_gethostport(url, &host, &port);
        if (ret != true)
        {
            FTPLOGI(FTPLOG_AT, "at error, get host/port error.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        username = (char *)atParamStr(pParam->params[1], &paramok);
        if (paramok != true || strlen(username) >= FTP_STRING_SIZE)
        {
            FTPLOGI(FTPLOG_AT, "at error, get username error.");
            RETURN_CME_ERR(pParam->engine, paramok ? ERR_AT_CME_TEXT_LONG : ERR_AT_CME_PARAM_INVALID);
        }

        passwd = (char *)atParamStr(pParam->params[2], &paramok);
        if (paramok != true || strlen(passwd) >= FTP_STRING_SIZE)
        {
            FTPLOGI(FTPLOG_AT, "at error, get password error");
            RETURN_CME_ERR(pParam->engine, paramok ? ERR_AT_CME_TEXT_LONG : ERR_AT_CME_PARAM_INVALID);
        }

        mode = atParamUintInRange(pParam->params[3], 0, 1, &paramok);
        if (paramok != true)
        {
            FTPLOGI(FTPLOG_AT, "at error, get mode error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        Tout = atParamUintInRange(pParam->params[4], 5, 180, &paramok);
        if (paramok != true)
        {
            FTPLOGI(FTPLOG_AT, "at error, get Tout error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        type = atParamUintInRange(pParam->params[5], 0, 1, &paramok);
        if (paramok != true)
        {
            FTPLOGI(FTPLOG_AT, "at error, get type error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        FTPLOGI(FTPLOG_AT, "host    : %s", host);
        FTPLOGI(FTPLOG_AT, "port    : %s", port);
        FTPLOGI(FTPLOG_AT, "username: %s", username);
        FTPLOGI(FTPLOG_AT, "passwd  : %s", passwd);
        FTPLOGI(FTPLOG_AT, "mode    : %d", mode);
        FTPLOGI(FTPLOG_AT, "Tout    : %d", Tout);
        FTPLOGI(FTPLOG_AT, "type    : %d", type);

        gAtFtpCtx = (atFtpCtx_t *)malloc(sizeof(atFtpCtx_t));
        if (gAtFtpCtx == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, memory failure.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);
        }
        memset(gAtFtpCtx, 0, sizeof(atFtpCtx_t));
        gAtFtpCtx->FTPengine = (atCmdEngine_t *)pParam->engine;

        gAtFtpCtx->AutoLogout.tmr = osiTimerCreate(osiThreadCurrent(), ftp_autologout_callback, (void *)NULL);
        if (gAtFtpCtx->AutoLogout.tmr == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, create autologout timer failure.");
            goto FAIL;
        }
        gAtFtpCtx->AutoLogout.start = false;
        gAtFtpCtx->AutoLogout.timeout = Tout;

        cfg.apiType = FTP_API_ASYNC;
        cfg.encrypto = false;
        cfg.mode = (ftp_mode_t)mode;
        cfg.type = (ftp_transfert_t)type;
        cfg.callback = ftp_msg_handler;

        ret = FTPLib_init(&cfg);
        if (!ret)
        {
            FTPLOGI(FTPLOG_AT, "at error, ftp init failure.");
            goto FAIL;
        }

        ftpRet = FTPLib_login(host, port, username, passwd);
        if (ftpRet != FTP_RET_SUCCESS)
        {
            FTPLOGI(FTPLOG_AT, "at error, ftp login failure.");
            FTPLib_uninit();
            goto FAIL;
        }

        ftp_setCmd(CMD_OPEN);
        atCmdSetTimeoutHandler(pParam->engine, 100 * 1000, ftp_AtTimer);
        break;

    FAIL:
        ftp_free();
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        break;
    }
    case AT_CMD_READ:
    {
        if (gAtFtpCtx && gAtFtpCtx->login && gAtFtpCtx->FTPengine == (atCmdEngine_t *)pParam->engine)
        {
            strcpy(tmpString, "^FTPOPEN:1");
            atCmdRespInfoText(pParam->engine, (char *)tmpString);
            atCmdRespOK(pParam->engine);
        }
        else
        {
            strcpy(tmpString, "^FTPOPEN:0");
            atCmdRespInfoText(pParam->engine, (char *)tmpString);
            atCmdRespOK(pParam->engine);
        }
    }
    break;
    case AT_CMD_TEST:
    {
        strcpy(tmpString, "^FTPOPEN:<url>,<username>,<password>,<mode>,<tout>,<type>");
        atCmdRespInfoText(pParam->engine, (char *)tmpString);
        atCmdRespOK(pParam->engine);
    }
    break;
    default:
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
    }
}

void AT_FTP_CmdFunc_CLOSE(atCommand_t *pParam)
{

    switch (pParam->type)
    {
    case AT_CMD_EXE:

        if (!gAtFtpCtx || !gAtFtpCtx->login || gAtFtpCtx->FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (FTPLib_logout() == FTP_RET_SUCCESS)
        {
            ftp_setCmd(CMD_CLOSE);
            atCmdSetTimeoutHandler(pParam->engine, 100 * 1000, ftp_AtTimer);
        }
        else
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_TEST:
        atCmdRespInfoText(pParam->engine, "^FTPCLOSE");
        atCmdRespOK(pParam->engine);
        break;
    default:
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

void AT_FTP_CmdFunc_SIZE(atCommand_t *pParam)
{
    bool paramok = true;
    char *filename = NULL;

    switch (pParam->type)
    {
    case AT_CMD_SET:

        if (netif_default == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, no default netif.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        if (!gAtFtpCtx || !gAtFtpCtx->login || gAtFtpCtx->FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (pParam->param_count != 1)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        filename = (char *)atParamStr(pParam->params[0], &paramok);
        if (paramok != true || (strlen(filename) == 0 || strlen(filename) >= FTP_STRING_SIZE))
        {
            FTPLOGI(FTPLOG_AT, "at error, get filename error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (FTPLib_size(filename, NULL) != FTP_RET_SUCCESS)
        {
            FTPLOGI(FTPLOG_AT, "at error, ftp size failure.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }

        ftp_setCmd(CMD_SIZE);

        break;
    case AT_CMD_TEST:
        atCmdRespInfoText(pParam->engine, "^FTPSIZE:<filename>");
        atCmdRespOK(pParam->engine);
        break;
    default:
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
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

        if (netif_default == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, no default netif.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        if (!gAtFtpCtx || !gAtFtpCtx->login || gAtFtpCtx->FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (pParam->param_count < 1 || pParam->param_count > 3)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        filepath = (char *)atParamStr(pParam->params[0], &paramok);
        if (!paramok || (filepath == NULL) || (strlen(filepath) == 0 || strlen(filepath) >= FTP_STRING_SIZE))
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (pParam->param_count >= 2)
        {
            offset = atParamInt(pParam->params[1], &paramok);
            if (!paramok || offset < 0)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            if (pParam->param_count == 2 && offset == 0)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (pParam->param_count == 3)
        {
            size = atParamInt(pParam->params[2], &paramok);
            if (!paramok || size <= 0)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        strncpy(gAtFtpCtx->getpath, filepath, FTP_STRING_SIZE - 1);
        gAtFtpCtx->req_getoffset = offset;
        gAtFtpCtx->req_getsize = size;
        gAtFtpCtx->getsize = 0;
        gAtFtpCtx->getparam = pParam->param_count;

        atCmdRespOK(pParam->engine);
    }
    else if (AT_CMD_READ == pParam->type)
    {
        char info[300] = {0};

        if (gAtFtpCtx == NULL)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (gAtFtpCtx->getparam == 1)
        {
            sprintf(info, "^FTPGETSET:%s", gAtFtpCtx->getpath);
        }
        else if (gAtFtpCtx->getparam == 2)
        {
            sprintf(info, "^FTPGETSET:%s,%ld", gAtFtpCtx->getpath, gAtFtpCtx->req_getoffset);
        }
        else if (gAtFtpCtx->getparam == 3)
        {
            sprintf(info, "^FTPGETSET:%s,%ld,%ld", gAtFtpCtx->getpath, gAtFtpCtx->req_getoffset, gAtFtpCtx->req_getsize);
        }
        else
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }

        atCmdRespInfoText(pParam->engine, (char *)info);
        atCmdRespOK(pParam->engine);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        atCmdRespInfoText(pParam->engine, "^FTPGETSET:<filename>, [offset, [size]]");
        atCmdRespOK(pParam->engine);
        return;
    }
    else
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }

    return;
}

void AT_FTP_CmdFunc_GET(atCommand_t *pParam)
{
    bool paramok = true;
    uint32_t mode, reqlength;

    switch (pParam->type)
    {
    case AT_CMD_SET:

        if (netif_default == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, no default netif.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        if (!gAtFtpCtx || !gAtFtpCtx->login || gAtFtpCtx->FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (pParam->param_count == 1)
        {
            mode = atParamUint(pParam->params[0], &paramok);
            if (!paramok || mode != 1)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            if (gAtFtpCtx->getparam == 0 || strlen(gAtFtpCtx->getpath) == 0)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

            gAtFtpCtx->getstop = false;
            gAtFtpCtx->req_getDelay = false;

            if (FTPLib_getStart(gAtFtpCtx->getpath, gAtFtpCtx->req_getoffset, gAtFtpCtx->req_getsize) != FTP_RET_SUCCESS)
            {
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }

            ftp_setCmd(CMD_GET);

            atCmdRespOK(pParam->engine);
        }
        else if (pParam->param_count == 2)
        {
            mode = atParamUint(pParam->params[0], &paramok);
            if (!paramok || mode != 2)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            reqlength = atParamUint(pParam->params[1], &paramok);
            if (!paramok || reqlength != 0)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            if (FTPLib_getStop() != FTP_RET_SUCCESS)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

            gAtFtpCtx->getstop = true;

            atCmdSetTimeoutHandler(pParam->engine, 100 * 1000, ftp_AtTimer);
        }
        else
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        break;
    case AT_CMD_TEST:
        atCmdRespInfoText(pParam->engine, "^FTPGET: <mode> [,<0>]");
        atCmdRespOK(pParam->engine);
        break;
    default:
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        break;
    }
}

void AT_FTP_CmdFunc_PUTSET(atCommand_t *pParam)
{
    char *filepath = NULL;

    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;

        if (netif_default == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, no default netif.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        if (!gAtFtpCtx || !gAtFtpCtx->login || gAtFtpCtx->FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        if (pParam->param_count != 1)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        filepath = (char *)atParamStr(pParam->params[0], &paramok);
        if (!paramok || (filepath == NULL) || (strlen(filepath) == 0 || strlen(filepath) >= FTP_STRING_SIZE))
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        strncpy(gAtFtpCtx->putpath, filepath, FTP_STRING_SIZE - 1);

        atCmdRespOK(pParam->engine);
    }
    else if (AT_CMD_READ == pParam->type)
    {
        char info[300] = {0};

        if (gAtFtpCtx == NULL)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (strlen(gAtFtpCtx->putpath) != 0)
        {
            sprintf(info, "^FTPPUTSET:%s", gAtFtpCtx->putpath);
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
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }

    return;
}

void AT_FTP_CmdFunc_PUT(atCommand_t *pParam)
{
    bool paramok = true;

    switch (pParam->type)
    {
    case AT_CMD_SET:
        if (netif_default == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, no default netif.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_NET_SERVICE);
        }

        if (!gAtFtpCtx || !gAtFtpCtx->login || gAtFtpCtx->FTPengine != (atCmdEngine_t *)pParam->engine)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

        uint32_t mode = atParamUint(pParam->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

        if (mode == 1)
        {
            if (pParam->param_count != 1)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            if (gAtFtpCtx->putstart || strlen(gAtFtpCtx->putpath) == 0)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

            if (FTPLib_putStart(gAtFtpCtx->putpath) == FTP_RET_SUCCESS)
            {
                gAtFtpCtx->putcancel = false;
                ftp_setCmd(CMD_PUT);
            }
            else
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else if (mode == 2)
        {
            if (pParam->param_count != 2)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            uint32_t req_length = atParamUintInRange(pParam->params[1], 0, FTP_SEND_MAX_SIZE, &paramok);
            if (!paramok)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

            if (!gAtFtpCtx->putstart)
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

            if (req_length == 0)
            {
                if (FTPLib_putStop() == FTP_RET_SUCCESS)
                {
                    gAtFtpCtx->putcancel = true;
                    ftp_setCmd(CMD_PUT);
                }
                else
                    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                ftp_txbufferinit(&(gAtFtpCtx->txbuf), req_length);
                ftp_data_mode_start();
                ftp_setCmd(CMD_PUT);
            }
        }
        else
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        break;
    case AT_CMD_TEST:
        atCmdRespInfoText(pParam->engine, "^FTPPUT: <mode> [,<reqlength>]");
        atCmdRespOK(pParam->engine);
        break;
    default:
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

#endif
