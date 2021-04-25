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

#ifdef CONFIG_AT_NEWMYFTP_SUPPORT

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

typedef enum _FTP_DATA_ID
{
    FTP_DATA_NULL,
    FTP_DATA_GET,
    FTP_DATA_PUT,
} FTP_DATA_ID_T;

typedef struct _atFtpCtx_t
{
    atFtpCmd_t FTPCmd;
    atFtpTimer_t AutoLogout;
    atCmdEngine_t *FTPengine;
    bool login;
    bool IsDataMode;
    bool getstop;
    char getpath[FTP_STRING_SIZE];
    uint32_t req_getoffset, req_getsize, getsize;
    bool req_getDelay;
    bool putstart, putcancel, puteof;
    char putpath[FTP_STRING_SIZE];
    ftp_txbuf_t txbuf;
} atFtpCtx_t;

#define FTP_CMD_CHECK(m)                                          \
    do                                                            \
    {                                                             \
        if (gAtMyFtpCtx->FTPCmd != m)                             \
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
static void ftp_data_mode_start(FTP_DATA_ID_T DataID);
static void ftp_data_mode_stop();
static int ftp_UartRecv(void *param, uint8_t *data, unsigned length);
static void ftp_msg_handler(uint32_t nEventId, uint32_t nResult, uint32_t nParam1, uint32_t nParam2);

static atFtpCtx_t *gAtMyFtpCtx = NULL;
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
    if (gAtMyFtpCtx->AutoLogout.tmr)
    {
        if (gAtMyFtpCtx->AutoLogout.start)
        {
            osiTimerStop(gAtMyFtpCtx->AutoLogout.tmr);
        }

        osiTimerDelete(gAtMyFtpCtx->AutoLogout.tmr);
        gAtMyFtpCtx->AutoLogout.tmr = NULL;
    }

    memset(gAtMyFtpCtx, 0, sizeof(atFtpCtx_t));
    free(gAtMyFtpCtx);
    gAtMyFtpCtx = NULL;
}

static void ftp_stop_autologout()
{
    if (gAtMyFtpCtx->AutoLogout.tmr && gAtMyFtpCtx->AutoLogout.start)
    {
        if (osiTimerStop(gAtMyFtpCtx->AutoLogout.tmr))
        {
            gAtMyFtpCtx->AutoLogout.start = false;
            FTPLOGI(FTPLOG_AT, "stop autologout timer success.");
        }
        else
        {
            FTPLOGI(FTPLOG_AT, "stop autologout timer failure.");
        }
    }
}

static void ftp_start_autologout(uint32_t timeout)
{
    if (gAtMyFtpCtx->AutoLogout.tmr && !gAtMyFtpCtx->AutoLogout.start)
    {
        if (osiTimerStart(gAtMyFtpCtx->AutoLogout.tmr, timeout * 1000))
        {
            gAtMyFtpCtx->AutoLogout.start = true;
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
    gAtMyFtpCtx->FTPCmd = cmd;

    if (cmd == CMD_NONE && gAtMyFtpCtx->login)
        ftp_start_autologout(gAtMyFtpCtx->AutoLogout.timeout);
    else
        ftp_stop_autologout();
}

static void ftp_autologout_callback(void *param)
{
    FTPLOGI(FTPLOG_AT, "enter ftp autologout timer callback");

    if (gAtMyFtpCtx && gAtMyFtpCtx->login && gAtMyFtpCtx->FTPCmd == CMD_NONE)
    {
        gAtMyFtpCtx->AutoLogout.start = false;
        if (FTPLib_logout() != FTP_RET_SUCCESS)
            ftp_start_autologout(1000);
    }
}

static void ftp_ReportInfo(char *data)
{
    atCmdEngine_t *engine = (atCmdEngine_t *)gAtMyFtpCtx->FTPengine;

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
    char tmpString[60] = {0};
    atCmdEngine_t *engine = (atCmdEngine_t *)gAtMyFtpCtx->FTPengine;

    if (engine == NULL || gAtMyFtpCtx->FTPCmd == CMD_NONE)
    {
        FTPLOGI(FTPLOG_AT, "ignore in ftp_CommandComplete");
        return;
    }

    ftp_data_mode_stop();

    if (succ)
    {
        atCmdRespOK(engine);
    }
    else
    {
        sprintf(tmpString, "ERROR:%d", code);
        atCmdRespErrorText(engine, tmpString);
    }

    ftp_setCmd(CMD_NONE);
}

static void ftp_QuitTimeout(atCommand_t *cmd)
{
    gAtMyFtpCtx->login = false;
    FTPLib_uninit();
    ftp_ReportInfo("$MYURCFTP:0");
    ftp_free();
}

static void ftp_AtTimer(atCommand_t *cmd)
{
    FTPLOGI(FTPLOG_AT, "at timeout, cmd : %s", cmd->desc->name);
    if (gAtMyFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "gAtMyFtpCtx is null in ftp_AtTimer.");
        return;
    }

    atFtpCmd_t ATcmd = gAtMyFtpCtx->FTPCmd;

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

    FTPLOGI(FTPLOG_AT, "[MYFTPNONE] Msg handler receive event[%s] with result[%d], param1:0x%x, param2:0x%x",
            ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtMyFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "[MYFTPNONE] Ignore the event!");
        return;
    }

    if (nEventId == FTP_MSG_PUT_STOP && gAtMyFtpCtx->putstart)
    {
        gAtMyFtpCtx->putstart = false;
        ftp_ReportInfo("$MYURCFTP:1");
    }

    if (nEventId == FTP_MSG_LOGOUT ||
        nEventId == FTP_MSG_DISCONNECT)
    {
        gAtMyFtpCtx->login = false;
        FTPLib_uninit();
        ftp_ReportInfo("$MYURCFTP:0");
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

    FTPLOGI(FTPLOG_AT, "[MYFTPOPEN] Msg handler receive event[%s] with result[%d], param1:%d, param2:%d",
            ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtMyFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "[MYFTPOPEN] Ignore the event!");
        return;
    }

    FTP_CMD_CHECK(CMD_OPEN);

    if (nEventId == FTP_MSG_LOGIN)
    {
        if (nResult == FTP_RET_SUCCESS)
        {
            gAtMyFtpCtx->login = true;
            ftp_CommandComplete(true, 0);
        }
        else
        {
            FTPLib_uninit();
            if (nResult == FTP_RET_CMD_CONNECT_FAIL)
                ftp_CommandComplete(false, ERR_AT_CME_TCPIP_CONNECT_TIMEOUT);
            else if (nResult == FTP_RET_CMD_DNS_ERR)
                ftp_CommandComplete(false, ERR_AT_CME_DNS_FAILED);
            else
                ftp_CommandComplete(false, ERR_AT_CME_UNKNOWN_ERROR);
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

    FTPLOGI(FTPLOG_AT, "[MYFTPCLOSE] Msg handler receive event[%s] with result[%d], param1:0x%x, param2:0x%x",
            ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtMyFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "[MYFTPCLOSE] Ignore the event!");
        return;
    }

    FTP_CMD_CHECK(CMD_CLOSE);

    if (nEventId == FTP_MSG_LOGOUT)
    {
        gAtMyFtpCtx->login = false;
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

    FTPLOGI(FTPLOG_AT, "[MYFTPSIZE] Msg handler receive event[%s] with result[%d], param1:%d, param2:%d",
            ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtMyFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "[MYFTPSIZE] Ignore the event!");
        return;
    }

    FTP_CMD_CHECK(CMD_SIZE);

    if (nEventId == FTP_MSG_SIZE)
    {
        if (nResult == FTP_RET_SUCCESS)
        {
            char ftpsize[32] = {0};
            sprintf(ftpsize, "$MYFTPSIZE:%ld", nParam1);
            ftp_ReportInfo(ftpsize);
            ftp_CommandComplete(true, 0);
        }
        else
        {
            ftp_CommandComplete(false, ERR_AT_CME_UNKNOWN_ERROR);
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
        FTPLOGI(FTPLOG_AT, "[MYFTPGET] Msg handler receive event[%s] with result[%d], param1:0x%x, param2:0x%x",
                ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtMyFtpCtx == NULL || !gAtMyFtpCtx->login)
    {
        FTPLOGI(FTPLOG_AT, "[MYFTPGET] Ignore the event!");
        return;
    }

    FTP_CMD_CHECK(CMD_GET);

    if (nEventId == FTP_MSG_CONNECT_DATA)
    {
        if (nResult == FTP_RET_SUCCESS)
        {
            ftp_ReportInfo("CONNECT");
            ftp_data_mode_start(FTP_DATA_GET);
        }
    }
    else if (nEventId == FTP_MSG_TRANSFER_DATA)
    {
        uint8_t *pbuffer = NULL;
        int32_t ByteNum = 0;
        uint32_t getLen = 0;
        uint32_t buflen = (uint32_t)nParam2;

        if (buflen == 0)
            return;

        if (gAtMyFtpCtx->getstop)
        {
            FTPLOGI(FTPLOG_AT, "discard this ftp download data");
            gAtMyFtpCtx->req_getDelay = false;
        }
        else
        {
            atDispatch_t *ch = atCmdGetDispatch(gAtMyFtpCtx->FTPengine);
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
                gAtMyFtpCtx->req_getDelay = true;
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
                    atCmdWrite(gAtMyFtpCtx->FTPengine, pbuffer, ByteNum);
                    free(pbuffer);
                }
                else
                {
                    FTPLOGI(FTPLOG_AT, "failed to get download data from ftplib");
                    gAtMyFtpCtx->req_getDelay = false;
                    free(pbuffer);
                    return;
                }
            }

            if (ByteNum != buflen)
            {
                gAtMyFtpCtx->req_getDelay = true;
                ftp_msg_handler(FTP_MSG_TRANSFER_DATA, FTP_RET_SUCCESS, (uint32_t)NULL, buflen - ByteNum);
            }
            else
            {
                gAtMyFtpCtx->req_getDelay = false;
            }
        }
    }
    else if (nEventId == FTP_MSG_GET)
    {
        if (gAtMyFtpCtx->req_getDelay)
        {
            FTPLOGI(FTPLOG_AT, "delay to close the transfer when data is available");
            ftp_msg_handler(nEventId, nResult, nParam1, nParam2);
            return;
        }

        if (gAtMyFtpCtx->getstop)
            gAtMyFtpCtx->getstop = false;

        if (nResult == FTP_RET_CMD_TIMEOUT || nResult == FTP_RET_CMD_BROKEN)
        {
            ftp_CommandComplete(false, ERR_AT_CME_UNKNOWN_ERROR);
            gAtMyFtpCtx->login = false;
            FTPLib_uninit();
            ftp_ReportInfo("$MYURCFTP:1");
            ftp_ReportInfo("$MYURCFTP:0");
            ftp_free();
        }
        else if (nResult == FTP_RET_CMD_REP_ERR || nResult == FTP_RET_INTERNAL_ERR ||
                 nResult == FTP_RET_DATA_CONNECT_FAIL || nResult == FTP_RET_DATA_BROKEN)
        {
            ftp_ReportInfo("$MYURCFTP:1");
            ftp_CommandComplete(false, ERR_AT_CME_UNKNOWN_ERROR);
        }
        else
        {
            ftp_CommandComplete(true, 0);
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

    FTPLOGI(FTPLOG_AT, "[MYFTPPUT] Msg handler receive event[%s] with result[%d], param1:0x%x, param2:0x%x",
            ftp_msg_names[nEventId - FTP_MSG_BASE], nResult, nParam1, nParam2);

    if (gAtMyFtpCtx == NULL || !gAtMyFtpCtx->login)
    {
        FTPLOGI(FTPLOG_AT, "[MYFTPPUT] Ignore the event!");
        return;
    }

    FTP_CMD_CHECK(CMD_PUT);

    if (nEventId == FTP_MSG_PUT_START)
    {
        if (nResult == FTP_RET_SUCCESS)
        {
            ftp_ReportInfo("CONNECT");
            ftp_data_mode_start(FTP_DATA_PUT);
            gAtMyFtpCtx->putstart = true;
        }
        else
        {
            ftp_CommandComplete(false, ERR_AT_CME_UNKNOWN_ERROR);
        }
    }
    else if (nEventId == FTP_MSG_TRANSFER_DATA)
    {
        if (nResult == FTP_RET_SUCCESS)
        {
            if (!gAtMyFtpCtx->puteof)
            {
                ftp_CommandComplete(true, 0);
            }
            else
            {
                if (FTPLib_putStop() == FTP_RET_SUCCESS)
                {
                    gAtMyFtpCtx->putcancel = true;
                }
                else
                {
                    ftp_CommandComplete(true, 0);
                }
            }
        }
        else
        {
            if (FTPLib_putStop() != FTP_RET_SUCCESS)
            {
                ftp_CommandComplete(false, ERR_AT_CME_UNKNOWN_ERROR);
            }
        }
    }
    else if (nEventId == FTP_MSG_PUT_STOP)
    {
        gAtMyFtpCtx->putstart = false;

        if (gAtMyFtpCtx->putcancel)
        {
            ftp_CommandComplete(true, 0);
        }
        else
        {
            ftp_ReportInfo("$MYURCFTP:1");
            ftp_CommandComplete(false, ERR_AT_CME_UNKNOWN_ERROR);
        }
    }
    else
    {
    }
}

static void ftp_msg_handler(uint32_t nEventId, uint32_t nResult, uint32_t nParam1, uint32_t nParam2)
{
    if (gAtMyFtpCtx == NULL)
    {
        FTPLOGI(FTPLOG_AT, "gAtMyFtpCtx is null in ftp_msg_handler.");
        return;
    }

    if (nEventId < FTP_MSG_BASE)
    {
        FTPLOGI(FTPLOG_AT, "invalid eventID.");
        return;
    }

    if (gAtMsgHandler[gAtMyFtpCtx->FTPCmd] != NULL)
    {
        osiEvent_t *ev = (osiEvent_t *)malloc(sizeof(osiEvent_t));
        ev->id = nEventId;
        ev->param1 = (uint32_t)nResult;
        ev->param2 = (uint32_t)nParam1;
        ev->param3 = (uint32_t)nParam2;

        if (nEventId == FTP_MSG_TRANSFER_DATA)
        {
            osiThreadSleep(50);
        }

        osiThreadCallback(atEngineGetThreadId(), (osiCallback_t)gAtMsgHandler[gAtMyFtpCtx->FTPCmd], (void *)ev);
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

static void ftp_data_mode_start(FTP_DATA_ID_T DataID)
{
    if (!gAtMyFtpCtx->IsDataMode)
    {
        atCmdSetBypassMode(gAtMyFtpCtx->FTPengine, (atCmdBypassCB_t)ftp_UartRecv, (void *)DataID);
        gAtMyFtpCtx->IsDataMode = true;
    }
}

static void ftp_data_mode_stop()
{
    if (gAtMyFtpCtx->IsDataMode)
    {
        atCmdSetLineMode(gAtMyFtpCtx->FTPengine);
        gAtMyFtpCtx->IsDataMode = false;
    }
}

static bool ftp_CheckPlus(uint8_t *data, unsigned length)
{
    unsigned i = 0;

    while ((i + 3) <= length)
    {
        if (data[i] == '+' && data[i + 1] == '+' && data[i + 2] == '+')
            return true;

        i++;
    }

    return false;
}

static int ftp_UartRecv(void *param, uint8_t *data, unsigned length)
{
    FTPLOGI(FTPLOG_AT, "Uart input data length = %d", length);

    FTP_DATA_ID_T type = (FTP_DATA_ID_T)param;

    if (ftp_CheckPlus(data, length))
    {
        FTPLOGI(FTPLOG_AT, "Uart will exit by +++");

        if (type == FTP_DATA_GET)
        {
            if (FTPLib_getStop() == FTP_RET_SUCCESS)
            {
                ftp_data_mode_stop();
                gAtMyFtpCtx->getstop = true;
            }
        }

        if (type == FTP_DATA_PUT)
        {
            if (FTPLib_putStop() == FTP_RET_SUCCESS)
            {
                ftp_data_mode_stop();
                gAtMyFtpCtx->putcancel = true;
            }
        }

        return length;
    }

    if (type == FTP_DATA_PUT)
    {
        int32_t left = ftp_txbufpush(&(gAtMyFtpCtx->txbuf), data, length);
        if (left == 0)
        {
            ftp_data_mode_stop();
            atCmdWrite(gAtMyFtpCtx->FTPengine, gAtMyFtpCtx->txbuf.buffer, gAtMyFtpCtx->txbuf.len);
            if (FTPLib_put(gAtMyFtpCtx->txbuf.buffer, gAtMyFtpCtx->txbuf.len) != FTP_RET_SUCCESS)
            {
                if (FTPLib_putStop() != FTP_RET_SUCCESS)
                {
                    ftp_CommandComplete(false, ERR_AT_CME_UNKNOWN_ERROR);
                }
            }
        }
    }

    return length;
}

void AT_MYFTP_CmdFunc_OPEN(atCommand_t *pParam)
{
    bool ret = false;
    bool paramok = true;
    int8_t iResult = -1;
    uint8_t uState = -1;
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
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PDP_DEACTIVED);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (gAtMyFtpCtx != NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, ftp is already open.");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_TCPIP_ALREADY_CONNECTED);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (pParam->param_count != 7)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        uint8_t channel = atParamUintInRange(pParam->params[0], 0, 5, &paramok);
        if (!paramok)
        {
            FTPLOGI(FTPLOG_AT, "at error, get channel error");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }
        iResult = CFW_GetGprsActState(channel + 1, &uState, atCmdGetSim(pParam->engine));
        if (iResult != ERR_SUCCESS)
        {
            FTPLOGI(FTPLOG_AT, "at error, CFW_GetGprsActState error\n");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_NETWORK_REFUSED);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }
        else
        {
            if (CFW_GPRS_ACTIVED != uState)
            {
                FTPLOGI(FTPLOG_AT, "at error, have not run MYNETACT\n");
                sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PDP_DEACTIVED);
                AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
            }
        }

        url = (char *)atParamStr(pParam->params[1], &paramok);
        if (paramok != true || strlen(url) >= FTP_STRING_SIZE)
        {
            FTPLOGI(FTPLOG_AT, "at error, get url error.");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }
        ret = ftp_gethostport(url, &host, &port);
        if (ret != true)
        {
            FTPLOGI(FTPLOG_AT, "at error, get host/port error.");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        username = (char *)atParamStr(pParam->params[2], &paramok);
        if (paramok != true || strlen(username) >= FTP_STRING_SIZE)
        {
            FTPLOGI(FTPLOG_AT, "at error, get username error.");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        passwd = (char *)atParamStr(pParam->params[3], &paramok);
        if (paramok != true || strlen(passwd) >= FTP_STRING_SIZE)
        {
            FTPLOGI(FTPLOG_AT, "at error, get password error");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        mode = atParamUintInRange(pParam->params[4], 0, 1, &paramok);
        if (paramok != true)
        {
            FTPLOGI(FTPLOG_AT, "at error, get mode error");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        Tout = atParamUintInRange(pParam->params[5], 5, 180, &paramok);
        if (paramok != true)
        {
            FTPLOGI(FTPLOG_AT, "at error, get Tout error");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        type = atParamUintInRange(pParam->params[6], 0, 1, &paramok);
        if (paramok != true)
        {
            FTPLOGI(FTPLOG_AT, "at error, get type error");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        FTPLOGI(FTPLOG_AT, "host    : %s", host);
        FTPLOGI(FTPLOG_AT, "port    : %s", port);
        FTPLOGI(FTPLOG_AT, "username: %s", username);
        FTPLOGI(FTPLOG_AT, "passwd  : %s", passwd);
        FTPLOGI(FTPLOG_AT, "mode    : %d", mode);
        FTPLOGI(FTPLOG_AT, "Tout    : %d", Tout);
        FTPLOGI(FTPLOG_AT, "type    : %d", type);

        gAtMyFtpCtx = (atFtpCtx_t *)malloc(sizeof(atFtpCtx_t));
        if (gAtMyFtpCtx == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, memory failure.");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }
        memset(gAtMyFtpCtx, 0, sizeof(atFtpCtx_t));
        gAtMyFtpCtx->FTPengine = (atCmdEngine_t *)pParam->engine;

        gAtMyFtpCtx->AutoLogout.tmr = osiTimerCreate(osiThreadCurrent(), ftp_autologout_callback, (void *)NULL);
        if (gAtMyFtpCtx->AutoLogout.tmr == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, create autologout timer failure.");
            goto FAIL;
        }
        gAtMyFtpCtx->AutoLogout.start = false;
        gAtMyFtpCtx->AutoLogout.timeout = Tout;

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
        sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        atCmdRespErrorText(pParam->engine, tmpString);
        break;
    }
    case AT_CMD_READ:
    {
        if (gAtMyFtpCtx && gAtMyFtpCtx->login && gAtMyFtpCtx->FTPengine == (atCmdEngine_t *)pParam->engine)
            strcpy(tmpString, "$MYFTPOPEN:1");
        else
            strcpy(tmpString, "$MYFTPOPEN:0");

        atCmdRespInfoText(pParam->engine, (char *)tmpString);
        atCmdRespOK(pParam->engine);
    }
    break;
    case AT_CMD_TEST:
    {
        atCmdRespOK(pParam->engine);
    }
    break;
    default:
    {
        sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
    }
    }
}

void AT_MYFTP_CmdFunc_CLOSE(atCommand_t *pParam)
{
    char tmpString[60] = {0};

    switch (pParam->type)
    {
    case AT_CMD_EXE:

        if (!gAtMyFtpCtx || !gAtMyFtpCtx->login || gAtMyFtpCtx->FTPengine != (atCmdEngine_t *)pParam->engine)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (FTPLib_logout() == FTP_RET_SUCCESS)
        {
            ftp_setCmd(CMD_CLOSE);
            atCmdSetTimeoutHandler(pParam->engine, 100 * 1000, ftp_AtTimer);
        }
        else
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }
        break;
    default:
        sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        break;
    }
}

void AT_MYFTP_CmdFunc_SIZE(atCommand_t *pParam)
{
    bool paramok = true;
    char *filename = NULL;
    char tmpString[60] = {0};

    switch (pParam->type)
    {
    case AT_CMD_SET:

        if (netif_default == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, no default netif.");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PDP_DEACTIVED);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (!gAtMyFtpCtx || !gAtMyFtpCtx->login || gAtMyFtpCtx->FTPengine != (atCmdEngine_t *)pParam->engine)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (pParam->param_count != 1)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        filename = (char *)atParamStr(pParam->params[0], &paramok);
        if (paramok != true || (strlen(filename) == 0 || strlen(filename) >= FTP_STRING_SIZE))
        {
            FTPLOGI(FTPLOG_AT, "at error, get filename error");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (FTPLib_size(filename, NULL) != FTP_RET_SUCCESS)
        {
            FTPLOGI(FTPLOG_AT, "at error, ftp size failure.");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        ftp_setCmd(CMD_SIZE);

        break;
    default:
        sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        break;
    }
}

void AT_MYFTP_CmdFunc_GET(atCommand_t *pParam)
{
    char *filepath = NULL;
    int32_t offset = 0;
    int32_t size = 0;

    char tmpString[60] = {0};

    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;

        if (netif_default == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, no default netif.");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PDP_DEACTIVED);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (!gAtMyFtpCtx || !gAtMyFtpCtx->login || gAtMyFtpCtx->FTPengine != (atCmdEngine_t *)pParam->engine)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (pParam->param_count < 1 || pParam->param_count > 3)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        filepath = (char *)atParamStr(pParam->params[0], &paramok);
        if (!paramok || (filepath == NULL) || (strlen(filepath) == 0 || strlen(filepath) >= FTP_STRING_SIZE))
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (pParam->param_count >= 2)
        {
            offset = atParamInt(pParam->params[1], &paramok);
            if (!paramok || offset < 0)
            {
                sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
            }

            if (pParam->param_count == 2 && offset == 0)
            {
                sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
            }
        }

        if (pParam->param_count == 3)
        {
            size = atParamInt(pParam->params[2], &paramok);
            if (!paramok || size <= 0)
            {
                sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
            }
        }

        strncpy(gAtMyFtpCtx->getpath, filepath, FTP_STRING_SIZE - 1);
        gAtMyFtpCtx->req_getoffset = offset;
        gAtMyFtpCtx->req_getsize = size;
        gAtMyFtpCtx->getsize = 0;
        gAtMyFtpCtx->getstop = false;
        gAtMyFtpCtx->req_getDelay = false;

        if (FTPLib_getStart(gAtMyFtpCtx->getpath, gAtMyFtpCtx->req_getoffset, gAtMyFtpCtx->req_getsize) != FTP_RET_SUCCESS)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        ftp_setCmd(CMD_GET);
    }
    else
    {
        sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
    }

    return;
}

void AT_MYFTP_CmdFunc_PUT(atCommand_t *pParam)
{
    char *filepath = NULL;
    uint32_t req_length = 0;
    uint32_t eof = 0;

    char tmpString[60] = {0};

    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;

        if (netif_default == NULL)
        {
            FTPLOGI(FTPLOG_AT, "at error, no default netif.");
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PDP_DEACTIVED);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (!gAtMyFtpCtx || !gAtMyFtpCtx->login || gAtMyFtpCtx->FTPengine != (atCmdEngine_t *)pParam->engine)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_TCPIP_NOT_CONNECTED);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        if (pParam->param_count != 3)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        filepath = (char *)atParamStr(pParam->params[0], &paramok);
        if (!paramok || (filepath == NULL) || (strlen(filepath) == 0 || strlen(filepath) >= FTP_STRING_SIZE))
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }
        strncpy(gAtMyFtpCtx->putpath, filepath, FTP_STRING_SIZE - 1);

        req_length = atParamUintInRange(pParam->params[1], 1, FTP_SEND_MAX_SIZE, &paramok);
        if (!paramok)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }

        eof = atParamUintInRange(pParam->params[2], 0, 1, &paramok);
        if (!paramok)
        {
            sprintf(tmpString, "ERROR:%d", ERR_AT_CME_PARAM_ERROR);
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
        }
        gAtMyFtpCtx->puteof = (eof == 1);

        if (gAtMyFtpCtx->putstart == false)
        {
            if (FTPLib_putStart(gAtMyFtpCtx->putpath) == FTP_RET_SUCCESS)
            {
                ftp_txbufferinit(&(gAtMyFtpCtx->txbuf), req_length);
                gAtMyFtpCtx->putcancel = false;
            }
            else
            {
                sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
                AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
            }
        }
        else
        {
            ftp_ReportInfo("CONNECT");
            ftp_txbufferinit(&(gAtMyFtpCtx->txbuf), req_length);
            ftp_data_mode_start(FTP_DATA_PUT);
        }

        ftp_setCmd(CMD_PUT);
    }
    else
    {
        sprintf(tmpString, "ERROR:%d", ERR_AT_CME_UNKNOWN_ERROR);
        AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, tmpString));
    }

    return;
}

#endif
