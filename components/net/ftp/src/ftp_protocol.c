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

#include "ftp_config.h"

#ifdef CONFIG_FTP_MODULE_SUPPORT

#include "osi_api.h"
#include "osi_log.h"
#include "at_engine.h"
#include "ftp_utils.h"
#include "ftp_protocol.h"
#include "ftp_connection.h"

#define TIMEOUT_RECV_CMD 40
#define TIMEOUT_RECV_DATA 20

#define state(x, y) _state(x, y, __LINE__)
#define setError(x) _setError(x, __LINE__)

typedef enum _ftpstate_t
{
    FTP_STOP,    /* do nothing state, stops the state machine */
    FTP_WAIT220, /* waiting for the initial 220 response immediately after
                  a connect */
    FTP_AUTH,
    FTP_USER,
    FTP_PASS,
    FTP_SIZE,
    FTP_RETR_SIZE,
    FTP_TYPE,
    FTP_TYPE_RETRY,
    FTP_PASV,
    FTP_RETR,
    FTP_STOR,
    FTP_REST,
    FTP_DATA_WAIT_CONNECT,
    FTP_DATA_TRANSFER,
    FTP_DATA_WAIT_RETR,
    FTP_DATA_WAIT_STOR,
    FTP_DATA_WAIT_226,
    FTP_DATA_WAIT_CLOSE,
    FTP_QUIT,
    FTP_IDLE,
} ftpstate_t;

static const char *const ftp_state_names[] = {
    "STOP",
    "WAIT220",
    "AUTH",
    "USER",
    "PASS",
    "SIZE",
    "RETR_SIZE",
    "TYPE",
    "TYPE_RETRY",
    "PASV",
    "RETR",
    "STOR",
    "REST",
    "DATA_WAIT_CONNECT",
    "DATA_TRANSFER",
    "DATA_WAIT_RETR",
    "DATA_WAIT_STOR",
    "DATA_WAIT_226",
    "DATA_WAIT_CLOSE",
    "QUIT",
    "IDLE"};

typedef bool (*ftpconn_create_t)();
typedef void (*ftpconn_destroy_t)();
typedef bool (*ftpconn_connect_t)(char *host, char *port, ftp_net_callback_t *cb, void *context, ftp_sock_e flag);
typedef bool (*ftpconn_disconnect_t)(ftp_sock_e flag);
typedef bool (*ftpconn_send_t)(uint8_t *buf, uint32_t buflen, ftp_sock_e flag);
typedef int32_t (*ftpconn_read_t)(uint8_t *buf, uint32_t buflen, ftp_sock_e flag);
typedef void (*ftpconn_setTimeout_t)(uint32_t tm, ftp_sock_e flag);
typedef void (*ftpconn_killTimeout_t)(ftp_sock_e flag);

typedef struct _ftpconn_op_t
{
    const char *scheme;
    ftpconn_create_t create;
    ftpconn_destroy_t destroy;
    ftpconn_connect_t connect1;
    ftpconn_disconnect_t disconnect;
    ftpconn_send_t send1;
    ftpconn_read_t read1;
    ftpconn_setTimeout_t setTimeout;
    ftpconn_killTimeout_t killTimeout;
} ftpconn_op_t;

static const ftpconn_op_t gFTPConnOp =
    {
        "FTP",
        ftp_conn_create,
        ftp_conn_destroy,
        ftp_conn_connect,
        ftp_conn_disconnect,
        ftp_conn_send,
        ftp_conn_read,
        ftp_conn_SetTimeout,
        ftp_conn_KillTimeout};

static const ftpconn_op_t gFTPSConnOp =
    {
        "FTPS",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
};

typedef struct _ftpconnection_t
{
    const ftpconn_op_t *op;
} ftpconnection_t;

typedef struct _ftpc_t
{
    ftp_api_t apiType;
    osiSemaphore_t *apisem;
    bool encrypto;
    ftp_mode_t mode;
    ftp_transfert_t type;
    uint32_t af_family;
    bool typeRetry;
} ftpc_t;

typedef enum _ftp_datamode_t
{
    FTP_DATA_NULL = 0,
    FTP_DATA_GET,
    FTP_DATA_PUT
} ftp_datamode_t;

typedef struct _ftp_getopt_t
{
    char getpath[FTP_STRING_SIZE];
    uint32_t req_getoffset, req_getsize, getsize;
    uint32_t req_ReadSize;
    bool getCancel;
} ftp_getopt_t;

typedef struct _ftp_putopt_t
{
    char putpath[FTP_STRING_SIZE];
} ftp_putopt_t;

typedef struct _ftpctx_t
{
    bool login;
    ftpc_t ftpc;
    ftpstate_t state;
    ftpconnection_t ftpCon;
    ftp_datamode_t dataCmd;
    ftp_loginopt_t loginOpt;
    ftp_getopt_t getOpt;
    ftp_putopt_t putOpt;
    ftplib_cb_t callback;
} ftpctx_t;

static ftpctx_t *gFTPLibCtx = NULL;
static int gFTPErrCode = 0;
static uint32_t gFTPSize = 0;

static ftp_msg_t ftp_getDataMsg(ftpctx_t *ctx);
static void ftp_DataTransferFinish(ftpctx_t *ctx);
static void ftp_error_rollback(ftpctx_t *ctx);
static void ftp_ctrl_onDns(void *context, bool ret);
static void ftp_ctrl_onConnect(void *context, bool ret);
static void ftp_ctrl_onDisconnect(void *context);
static void ftp_ctrl_onRead(ftp_recv_ret_e ret, void *context, uint8_t *buf, uint32_t buflen);
static void ftp_ctrl_onException(void *context);
static void ftp_data_onDns(void *context, bool ret);
static void ftp_data_onConnect(void *context, bool ret);
static void ftp_data_onDisconnect(void *context);
static void ftp_data_onRead(ftp_recv_ret_e ret, void *context, uint8_t *buf, uint32_t buflen);
static void ftp_data_onException(void *context);

static bool ftp_state_quit(ftpctx_t *ctx);

static ftp_net_callback_t ctrl_netCalback = {
    .onDns = ftp_ctrl_onDns,
    .onConnect = ftp_ctrl_onConnect,
    .onDisconnect = ftp_ctrl_onDisconnect,
    .onRead = ftp_ctrl_onRead,
    .onException = ftp_ctrl_onException};

static ftp_net_callback_t data_netCalback = {
    .onDns = ftp_data_onDns,
    .onConnect = ftp_data_onConnect,
    .onDisconnect = ftp_data_onDisconnect,
    .onRead = ftp_data_onRead,
    .onException = ftp_data_onException};

static uint32_t ftp_getline(const uint8_t *buf, uint32_t buflen)
{
    uint32_t i = 0;

    while (i + 1 < buflen)
    {
        if ((buf[i] == '\r') && (buf[i + 1] == '\n'))
            return (i + 2);
        else
            i++;
    }

    return 0;
}
/* This is the ONLY way to change FTP state! */
static void _state(ftpctx_t *ctx, ftpstate_t newstate, int lineno)
{
    if (ctx->state != newstate)
        FTPLOGI(FTPLOG_LIB, "PSM(line %d) change state %s to %s",
                lineno, ftp_state_names[ctx->state], ftp_state_names[newstate]);

    ctx->state = newstate;
}

static void _setError(int code, int lineno)
{
    if (gFTPErrCode != code)
        FTPLOGI(FTPLOG_LIB, "ERRNO(line %d) change code %d to %d",
                lineno, gFTPErrCode, code);
    gFTPErrCode = code;
}

static void ftp_set_idle(ftpctx_t *ctx)
{
    if (ctx)
    {
        ctx->dataCmd = FTP_DATA_NULL;
        ctx->getOpt.getCancel = false;
        const ftpconn_op_t *op = ctx->ftpCon.op;
        if (op && op->killTimeout)
        {
            op->killTimeout(FTP_CTRL_SOCK);
            op->killTimeout(FTP_DATA_SOCK);
        }

        state(ctx, FTP_IDLE);
    }
}

static void ftp_result_output(ftpctx_t *ctx, uint32_t nEventId, uint32_t nResult, uint32_t nParam1, uint32_t nParam2)
{
    //FTPLOGI(FTPLOG_LIB, "ftp get result of event[0x%x] with result[%d], param1:%d, param2:%d", nEventId, nResult, nParam1, nParam2);

    if (!ctx)
        return;

    if (nEventId == FTP_MSG_TRANSFER_DATA)
    {
        if (ctx->callback)
        {
            ctx->callback(nEventId, nResult, nParam1, nParam2);
        }

        return;
    }

    if (ctx->ftpc.apiType == FTP_API_ASYNC)
    {
        if (ctx->callback)
            ctx->callback(nEventId, nResult, nParam1, nParam2);
    }
    else
    {
        if (nEventId == FTP_MSG_CONNECT || nEventId == FTP_MSG_DISCONNECT)
            return;
        if (nEventId == FTP_MSG_CONNECT_DATA || nEventId == FTP_MSG_DISCONNECT_DATA)
            return;

        osiSemaphoreRelease(ctx->ftpc.apisem);
    }
}

/* Need to rollback when transfered timeout on command connection */
static void ftp_command_timeout(ftpctx_t *ctx)
{
    if (ctx->state == FTP_IDLE)
    {
        FTPLOGI(FTPLOG_LIB, "ftp state is idle to ignore it");
        return;
    }

    setError(FTP_RET_CMD_TIMEOUT);

    ftp_error_rollback(ctx);
}

static void ftp_data_timeout(ftpctx_t *ctx)
{
    if (ctx->dataCmd != FTP_DATA_GET && ctx->dataCmd != FTP_DATA_PUT)
    {
        FTPLOGI(FTPLOG_LIB, "ftp data state to ignore it");
        return;
    }

    setError(FTP_RET_DATA_TIMEOUT);

    ftp_error_rollback(ctx);
}

static bool ftp_command_sendf(ftpctx_t *ctx, const char *format, ...)
{
    char command[256] = {0};

    if (!ctx)
        return false;

    const ftpconn_op_t *op = ctx->ftpCon.op;
    if (op && op->send1)
    {
        va_list args;
        va_start(args, format);
        vsnprintf(command, 253, format, args);
        va_end(args);
        strcat(command, "\r\n");
        FTPLOGI(FTPLOG_LIB, "Request: %s", command);
        if (op->send1((uint8_t *)command, strlen(command), FTP_CTRL_SOCK))
        {
            op->setTimeout(TIMEOUT_RECV_CMD, FTP_CTRL_SOCK);
            ;
            return true;
        }
    }

    return false;
}

#define CMDSENDF(x, y, z)                \
    result = ftp_command_sendf(x, y, z); \
    if (!result)                         \
    return result

static bool ftp_state_user(ftpctx_t *ctx)
{
    bool result;
    if (!ctx)
        return false;

    /* send USER */
    CMDSENDF(ctx, "USER %s", ctx->loginOpt.username);
    state(ctx, FTP_USER);

    return true;
}

static bool ftp_state_quit(ftpctx_t *ctx)
{
    bool result;
    if (!ctx)
        return false;

    /* send USER */
    CMDSENDF(ctx, "%s", "QUIT");
    state(ctx, FTP_QUIT);

    return true;
}

static bool ftp_state_size(ftpctx_t *ctx, char *file)
{
    bool result;
    if (!ctx)
        return false;

    /* send USER */
    CMDSENDF(ctx, "SIZE %s", file);
    state(ctx, FTP_SIZE);

    return true;
}

static bool ftp_state_type(ftpctx_t *ctx, ftp_datamode_t datamode)
{
    bool result;
    if (!ctx || (datamode != FTP_DATA_GET && datamode != FTP_DATA_PUT))
        return false;

    /* send TYPE */
    if (ctx->ftpc.type == FTP_ASCII)
    {
        CMDSENDF(ctx, "TYPE %s", "A");
    }
    else if (ctx->ftpc.type == FTP_BINARY)
    {
        CMDSENDF(ctx, "TYPE %s", "I");
    }
    else
    {
        return false;
    }

    state(ctx, FTP_TYPE);
    ctx->dataCmd = datamode;
    ctx->ftpc.typeRetry = false;

    return true;
}

static bool ftp_state_get(ftpctx_t *ctx)
{
    bool result;
    if (!ctx)
        return false;

    ftp_getopt_t *getOpt = (ftp_getopt_t *)(&(ctx->getOpt));

    if (getOpt->req_getoffset == 0 && getOpt->req_getsize == 0)
    {
        CMDSENDF(ctx, "RETR %s", getOpt->getpath);
        state(ctx, FTP_RETR);
    }
    else
    {
        CMDSENDF(ctx, "SIZE %s", getOpt->getpath);
        state(ctx, FTP_RETR_SIZE);
    }

    return true;
}

static bool ftp_state_put(ftpctx_t *ctx)
{
    bool result;
    if (!ctx)
        return false;

    ftp_putopt_t *putOpt = (ftp_putopt_t *)(&(ctx->putOpt));

    CMDSENDF(ctx, "STOR %s", putOpt->putpath);
    state(ctx, FTP_STOR);

    return true;
}

static bool ftp_state_use_pasv(ftpctx_t *ctx)
{
    /*
    Here's the excecutive summary on what to do:

    PASV is RFC959, expect:
    227 Entering Passive Mode (a1,a2,a3,a4,p1,p2)

    LPSV is RFC1639, expect:
    228 Entering Long Passive Mode (4,4,a1,a2,a3,a4,2,p1,p2)

    EPSV is RFC2428, expect:
    229 Entering Extended Passive Mode (|||port|)
    */

    bool result;
    if (!ctx)
        return false;

    static const char mode[][5] = {"PASV", "EPSV"};

    if (ctx->ftpc.af_family == AF_INET)
    {
        CMDSENDF(ctx, "%s", mode[0]);
    }
    else if (ctx->ftpc.af_family == AF_INET6)
    {
        CMDSENDF(ctx, "%s", mode[1]);
    }
    else
    {
        return false;
    }

    state(ctx, FTP_PASV);

    return true;
}

static bool ftp_state_user_resp(ftpctx_t *ctx, int ftpcode)
{
    bool result = false;

    /* some need password anyway, and others just return 2xx ignored */
    if ((ftpcode == 331) && (ctx->state == FTP_USER))
    {
        /* 331 Password required for ...
       (the server requires to send the user's password too) */
        CMDSENDF(ctx, "PASS %s", ctx->loginOpt.passwd);
        state(ctx, FTP_PASS);
    }
    else if (ftpcode == 220)
    {
        /* to ignore welcom message is coming */
    }
    else if (ftpcode == 230)
    {
        /* 230 User ... logged in.
       (the user logged in with or without password) */
        ctx->login = true;
        ftp_set_idle(ctx);
        ftp_result_output(ctx, FTP_MSG_LOGIN, gFTPErrCode, 0, 0);
    }
    else if (ftpcode == 332)
    {
        FTPLOGI(FTPLOG_LIB, "ACCT requested but none available");
        return false;
    }
    else
    {
        FTPLOGI(FTPLOG_LIB, "Access denied: %03d", ftpcode);
        return false;
    }

    return true;
}

static bool ftp_state_size_resp(ftpctx_t *ctx, int ftpcode, uint8_t *buf)
{
    bool result;
    uint32_t ftpsize = 0;

    /* 213 (number)
     * 550 File not found 
     */
    if (ftpcode == 213)
    {
        ftpsize = atoi((char *)buf + 4);

        if (ctx->state == FTP_SIZE)
        {
            ftp_set_idle(ctx);
            gFTPSize = ftpsize;
            ftp_result_output(ctx, FTP_MSG_SIZE, gFTPErrCode, gFTPSize, 0);
            return true;
        }
        else if (ctx->state == FTP_RETR_SIZE)
        {
            if (ctx->getOpt.req_getoffset >= ftpsize)
            {
                return false;
            }
            else
            {

                if (ctx->getOpt.req_getoffset == 0)
                {
                    CMDSENDF(ctx, "RETR %s", ctx->getOpt.getpath);
                    state(ctx, FTP_RETR);
                }
                else
                {
                    CMDSENDF(ctx, "REST %d", ctx->getOpt.req_getoffset);
                    state(ctx, FTP_REST);
                }

                return true;
            }
        }
        else
        {
            FTPLOGI(FTPLOG_LIB, "Unkown the state: %s", ftp_state_names[ctx->state]);
            return false;
        }
    }
    else
    {
        return false;
    }
}

static bool ftp_state_type_resp(ftpctx_t *ctx, int ftpcode)
{
    bool result = false;

    /* 220 Type set to I/A */
    if (ftpcode != 200)
    {
        FTPLOGI(FTPLOG_LIB, "Got a %03d response code instead of the assumed 200", ftpcode);

        if (!ctx->ftpc.typeRetry)
        {
            /* send TYPE */
            if (ctx->ftpc.type == FTP_ASCII)
            {
                CMDSENDF(ctx, "TYPE %s", "I");
            }
            else if (ctx->ftpc.type == FTP_BINARY)
            {
                CMDSENDF(ctx, "TYPE %s", "A");
            }
            else
            {
                return false;
            }

            ctx->ftpc.typeRetry = true;

            return true;
        }
        else
            return false;
    }

    if (ctx->state == FTP_TYPE)
    {
        if (ctx->ftpc.mode == FTP_PASSIVE)
        {
            result = ftp_state_use_pasv(ctx);
        }
        else
        {
            return false;
        }
    }
    else if (ctx->state == FTP_TYPE_RETRY)
    {
        if (ctx->dataCmd == FTP_DATA_GET)
            result = ftp_state_get(ctx);
    }
    else
    {
        return false;
    }

    return result;
}

static bool ftp_state_pasv_resp(ftpctx_t *ctx, int ftpcode, uint8_t *buf)
{
    bool ret = false;
    char *str = (char *)(buf + 4);

    uint16_t newport = 0;
    char *newhost = NULL;
    //char s_newhost[FTP_STRING_SIZE] = {0};
    char s_newport[FTP_STRING_SIZE] = {0};

    /* 229 Entering Extended Passive Mode (|||10024|) */
    if (ftpcode == 229)
    {
        char *ptr = strchr(str, '(');
        if (ptr == NULL)
        {
            FTPLOGI(FTPLOG_LIB, "No '(' in Passive Mode reply");
            return false;
        }
        unsigned int num;
        char separator[4];
        ptr++;
        if (5 == sscanf(ptr, "%c%c%c%u%c",
                        &separator[0],
                        &separator[1],
                        &separator[2],
                        &num,
                        &separator[3]))
        {
            const char sep1 = separator[0];
            int i;

            /* The four separators should be identical, or else this is an oddly
               formatted reply and we bail out immediately. */
            for (i = 1; i < 4; i++)
            {
                if (separator[i] != sep1)
                {
                    FTPLOGI(FTPLOG_LIB, "The four separators is not the same");
                    return false;
                }
            }

            if (num > 0xffff)
            {
                FTPLOGI(FTPLOG_LIB, "Illegal port number in EPSV reply");
                return false;
            }

            newport = (unsigned short)(num & 0xffff);
            newhost = ctx->loginOpt.host;
        }
        else
        {
            FTPLOGI(FTPLOG_LIB, "Weirdly formatted EPSV reply");
            return false;
        }
    }
    else if (ftpcode == 227)
    {
        /* positive PASV response */
        unsigned int ip[4];
        unsigned int port[2];

        /*
        * Scan for a sequence of six comma-separated numbers and use them as
        * IP+port indicators.
        *
        * Found reply-strings include:
        * "227 Entering Passive Mode (127,0,0,1,4,51)"
        * "227 Data transfer will passively listen to 127,0,0,1,4,51"
        * "227 Entering passive mode. 127,0,0,1,4,51"
        */

        while (*str)
        {
            if (6 == sscanf(str, "%u,%u,%u,%u,%u,%u",
                            &ip[0], &ip[1], &ip[2], &ip[3],
                            &port[0], &port[1]))
                break;
            str++;
        }

        if (!*str || (ip[0] > 255) || (ip[1] > 255) || (ip[2] > 255) ||
            (ip[3] > 255) || (port[0] > 255) || (port[1] > 255))
        {
            FTPLOGI(FTPLOG_LIB, "Couldn't interpret the 227-response");
            return false;
        }
#if 0
        snprintf(s_newhost, FTP_STRING_SIZE - 1, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
        newhost = s_newhost;
#else
        newhost = ctx->loginOpt.host;
#endif
        newport = (unsigned short)(((port[0] << 8) + port[1]) & 0xffff);
    }
    else
    {
        FTPLOGI(FTPLOG_LIB, "Bad PASV/EPSV response: %03d", ftpcode);
        return false;
    }

    ret = ctx->ftpCon.op->connect1(newhost, itoa(newport, s_newport, 10), &data_netCalback, (void *)&gFTPLibCtx, FTP_DATA_SOCK);
    if (!ret)
        return false;

    state(ctx, FTP_DATA_WAIT_CONNECT);

    return true;
}

static bool ftp_state_rest_resp(ftpctx_t *ctx, int ftpcode)
{
    bool result;

    if (ftpcode == 350)
    {
        CMDSENDF(ctx, "RETR %s", ctx->getOpt.getpath);
        state(ctx, FTP_RETR);
        return true;
    }
    else
        return false;
}

static bool ftp_state_get_resp(ftpctx_t *ctx, int ftpcode, uint8_t *buf)
{
    bool result = false;

    if ((ftpcode == 150) || (ftpcode == 125))
    {

        /*
        A;
        150 Opening BINARY mode data connection for /etc/passwd (2241
        bytes).  (ok, the file is being transferred)

        B:
        150 Opening ASCII mode data connection for /bin/ls

        C:
        150 ASCII data connection for /bin/ls (137.167.104.91,37445) (0 bytes).

        D:
        150 Opening ASCII mode data connection for [file] (0.0.0.0,0) (545 bytes)

        E:
        125 Data connection already open; Transfer starting. */

        if (ctx->state == FTP_RETR)
        {
            state(ctx, FTP_DATA_TRANSFER);
            if (ctx->getOpt.getsize == 0)
                ctx->ftpCon.op->setTimeout(TIMEOUT_RECV_DATA, FTP_DATA_SOCK);
        }
        else if (ctx->state == FTP_DATA_WAIT_RETR)
        {
            state(ctx, FTP_DATA_WAIT_226);
        }
        else
        {
            FTPLOGI(FTPLOG_LIB, "Unkown state in RETR response: %s", ftp_state_names[ctx->state]);
        }

        return true;
    }
    else
    {
        FTPLOGI(FTPLOG_LIB, "Bad RETR response: %03d", ftpcode);

        /* 550 No support for resume of ASCII transfer */
        if ((strstr((char *)buf, "ASCII") != NULL) && !ctx->ftpc.typeRetry)
        {
            CMDSENDF(ctx, "TYPE %s", "I");
            state(ctx, FTP_TYPE_RETRY);
            ctx->ftpc.typeRetry = true;
        }
        /* 550 No support for resume of BINARY transfer */
        else if ((strstr((char *)buf, "BINARY") != NULL) && !ctx->ftpc.typeRetry)
        {
            CMDSENDF(ctx, "TYPE %s", "A");
            state(ctx, FTP_TYPE_RETRY);
            ctx->ftpc.typeRetry = true;
        }
        else
            return false;

        return result;
    }
}

static bool ftp_state_stor_resp(ftpctx_t *ctx, int ftpcode)
{
    if (ftpcode >= 400)
    {
        FTPLOGI(FTPLOG_LIB, "Failed FTP upload: %0d", ftpcode);
        return false;
    }

    state(ctx, FTP_DATA_TRANSFER);
    ftp_result_output(ctx, FTP_MSG_PUT_START, gFTPErrCode, 0, 0);
    ctx->ftpCon.op->setTimeout(TIMEOUT_RECV_DATA * 2, FTP_DATA_SOCK);

    return true;
}

static bool ftp_state_transfer_resp(ftpctx_t *ctx, int ftpcode)
{
    bool result = false;

    /* 226 Transfer complete, 
     * 250 Requested file action okay, completed. 
     * 426 Failure writing network stream 
     *     a) Disconnect tcp connection activly when received enough data
     *     b) Ftp download is canceled by user
     */
    if ((ftpcode == 226) || (ftpcode == 250) ||
        ((ftpcode == 426) && (ctx->getOpt.req_getsize == ctx->getOpt.getsize)) ||
        ((ftpcode == 426) && ctx->getOpt.getCancel))
    {
        result = true;

        if (ctx->state == FTP_DATA_WAIT_226)
        {
            ftp_DataTransferFinish(ctx);
        }
    }

    if (ctx->state == FTP_DATA_TRANSFER)
    {
        state(ctx, FTP_DATA_WAIT_CLOSE);

        /* Don't set the timer to close the connection when ftp download. 
        *  Because the tcp data is cached to wait AT read.
        *  Close the socket will clear the tcp recv buffer.
        *  To set the timer When AT read empty the cache in FTPLib_get()
        */
        if (ctx->dataCmd != FTP_DATA_GET)
            ctx->ftpCon.op->setTimeout(10, FTP_DATA_SOCK);
    }

    return result;
}

static int ftp_readcode(uint8_t *buf, uint32_t buflen)
{
    int i = 0;
    int code = 0;
    char *space = NULL;

    if (buf == NULL || buflen < 3)
        return 0;

    space = strstr((char *)buf, " ");
    if (!space)
        return 0;

    while ((char *)(buf + i) != space)
    {
        if ((buf[i] < '0') || (buf[i] > '9'))
            break;
        code = code * 10 + (buf[i] - '0');
        i++;
    }

    return code;
}

static ftp_msg_t ftp_getDataMsg(ftpctx_t *ctx)
{
    ftp_msg_t msg;

    if (ctx->dataCmd == FTP_DATA_GET)
    {
        msg = FTP_MSG_GET;
    }
    else if (ctx->dataCmd == FTP_DATA_PUT)
    {
        switch (ctx->state)
        {
        case FTP_DATA_TRANSFER:
        case FTP_DATA_WAIT_226:
        case FTP_DATA_WAIT_CLOSE:
            msg = FTP_MSG_PUT_STOP;
            break;
        default:
            msg = FTP_MSG_PUT_START;
            break;
        }
    }
    else
    {
        msg = FTP_MSG_UNKOWN;
    }

    return msg;
}

static void ftp_DataTransferFinish(ftpctx_t *ctx)
{
    ftp_msg_t msg;

    msg = ftp_getDataMsg(ctx);
    ftp_set_idle(ctx);
    ftp_result_output(ctx, msg, gFTPErrCode, 0, 0);
}

/*
 * 1. ftp command timeout
 * 2. ftp data timeout
 * 3. ftp response error
*/
static void ftp_error_rollback(ftpctx_t *ctx)
{

    FTPLOGI(FTPLOG_LIB, "ftp error handler, state : %s", ftp_state_names[ctx->state]);

    switch (ctx->state)
    {
    case FTP_WAIT220:
    case FTP_USER:
    case FTP_PASS:
        setError(FTP_RET_CMD_USER_PASS_ERR);
        ctx->ftpCon.op->disconnect(FTP_CTRL_SOCK);
        break;
    case FTP_QUIT:
        ctx->ftpCon.op->disconnect(FTP_DATA_SOCK);
        ctx->ftpCon.op->disconnect(FTP_CTRL_SOCK);
        break;
    case FTP_SIZE:
    {
        if (gFTPErrCode == FTP_RET_CMD_REP_ERR)
        {
            ftp_set_idle(ctx);
            ftp_result_output(ctx, FTP_MSG_SIZE, gFTPErrCode, 0, 0);
        }
        else if (gFTPErrCode == FTP_RET_CMD_TIMEOUT)
        {
            if (!ctx->ftpCon.op->disconnect(FTP_CTRL_SOCK))
                FTPLOGI(FTPLOG_LIB, "ftp error handler, disconnect ctrl failure");
        }
        else
        {
            FTPLOGI(FTPLOG_LIB, "ftp error handler, unkown errCode : %d", gFTPErrCode);
        }
    }
    break;
    case FTP_TYPE:
    case FTP_PASV:
    case FTP_DATA_WAIT_RETR:
    case FTP_DATA_WAIT_STOR:
    case FTP_DATA_WAIT_226:
    {
        if (gFTPErrCode == FTP_RET_CMD_REP_ERR)
        {
            ftp_DataTransferFinish(ctx);
        }
        else if (gFTPErrCode == FTP_RET_CMD_TIMEOUT)
        {
            if (!ctx->ftpCon.op->disconnect(FTP_CTRL_SOCK))
                FTPLOGI(FTPLOG_LIB, "ftp error handler, disconnect ctrl failure");
        }
        else
        {
            FTPLOGI(FTPLOG_LIB, "ftp error handler, unkown errCode : %d", gFTPErrCode);
        }
    }
    break;
    case FTP_RETR_SIZE:
    case FTP_REST:
    case FTP_RETR:
    case FTP_TYPE_RETRY:
    case FTP_STOR:
    {
        if (gFTPErrCode == FTP_RET_CMD_REP_ERR)
        {
            if (!ctx->ftpCon.op->disconnect(FTP_DATA_SOCK))
                FTPLOGI(FTPLOG_LIB, "ftp error handler, disconnect data failure");
        }
        else if (gFTPErrCode == FTP_RET_CMD_TIMEOUT)
        {
            if (!ctx->ftpCon.op->disconnect(FTP_DATA_SOCK))
                FTPLOGI(FTPLOG_LIB, "ftp error handler, disconnect data failure");

            if (!ctx->ftpCon.op->disconnect(FTP_CTRL_SOCK))
                FTPLOGI(FTPLOG_LIB, "ftp error handler, disconnect ctrl failure");
        }
        else
        {
            FTPLOGI(FTPLOG_LIB, "ftp error handler, unkown errCode : %d", gFTPErrCode);
        }
    }
    break;
    case FTP_DATA_TRANSFER:
    case FTP_DATA_WAIT_CLOSE:
        if (gFTPErrCode == FTP_RET_DATA_TIMEOUT)
        {
            if (!ctx->ftpCon.op->disconnect(FTP_DATA_SOCK))
                FTPLOGI(FTPLOG_LIB, "ftp error handler, disconnect data failure");
        }
        break;
    default:
        break;
    }
}
static bool ftp_statemach_act(ftpctx_t *ctx, uint8_t *buf, uint32_t buflen)
{
    int ftpcode = 0;
    bool result = false;
    FTPLOGI(FTPLOG_LIB, "Repsonse: %s", buf);
    ftpcode = ftp_readcode(buf, buflen);
    if (ftpcode == 0)
    {
        FTPLOGI(FTPLOG_LIB, "read ftp code failed.");
        return false;
    }

    /* 421 connection timeout */
    if (ftpcode == 421)
    {
        setError(FTP_RET_CMD_TIMEOUT);
        return true;
    }

    if (ftpcode)
    {
        /* we have now received a full FTP server response */
        switch (ctx->state)
        {
        case FTP_WAIT220:
            if (ftpcode == 230)
            {
                FTPLOGI(FTPLOG_LIB, "230 User logged in - already!");
                return false;
            }
            else if (ftpcode != 220)
            {
                FTPLOGI(FTPLOG_LIB, "Got a %03d ftp-server response when 220 was expected", ftpcode);
                return false;
            }
            if (ctx->ftpc.encrypto)
            {
                FTPLOGI(FTPLOG_LIB, "Not support ftps yet.");
                return false;
            }
            else
            {
                result = ftp_state_user(ctx);
            }
            break;
        case FTP_USER:
        case FTP_PASS:
        {
            result = ftp_state_user_resp(ctx, ftpcode);
        }
        break;
        case FTP_QUIT:
            /* 221 Goodbye! */
            if (ftpcode != 221)
                FTPLOGI(FTPLOG_LIB, "Got a non-221 response code when quit ftp!");
            ctx->ftpCon.op->disconnect(FTP_DATA_SOCK);
            ctx->ftpCon.op->disconnect(FTP_CTRL_SOCK);
            result = true;
            break;
        case FTP_SIZE:
        case FTP_RETR_SIZE:
            result = ftp_state_size_resp(ctx, ftpcode, buf);
            break;
        case FTP_TYPE:
        case FTP_TYPE_RETRY:
            result = ftp_state_type_resp(ctx, ftpcode);
            break;
        case FTP_PASV:
            result = ftp_state_pasv_resp(ctx, ftpcode, buf);
            break;
        case FTP_REST:
            result = ftp_state_rest_resp(ctx, ftpcode);
            break;
        case FTP_RETR:
        case FTP_DATA_WAIT_RETR:
            result = ftp_state_get_resp(ctx, ftpcode, buf);
            break;
        case FTP_STOR:
        case FTP_DATA_WAIT_STOR:
            result = ftp_state_stor_resp(ctx, ftpcode);
            break;
        case FTP_DATA_TRANSFER:
        case FTP_DATA_WAIT_226:
            result = ftp_state_transfer_resp(ctx, ftpcode);
            break;
        case FTP_IDLE:
            result = true;
            break;
        default:
            /* internal error */
            setError(FTP_RET_INTERNAL_ERR);
            result = false;
            break;
        }
    }

    return result;
}

static void ftp_ctrl_onDns(void *context, bool ret)
{
    ftpctx_t *ctx = *((ftpctx_t **)context);
    if (!ctx)
    {
        FTPLOGI(FTPLOG_LIB, "[onDns][CTRL]: ftplib is not initialized.");
        return;
    }

    FTPLOGI(FTPLOG_LIB, "[onDns][CTRL]: %s", (ret ? "success" : "failure"));

    if (!ret)
    {
        setError(FTP_RET_CMD_DNS_ERR);
        ftp_result_output(ctx, FTP_MSG_LOGIN, gFTPErrCode, 0, 0);
    }
}

static void ftp_ctrl_onConnect(void *context, bool ret)
{
    ftpctx_t *ctx = *((ftpctx_t **)context);
    if (!ctx)
    {
        FTPLOGI(FTPLOG_LIB, "[onConnect][CTRL]: ftplib is not initialized.");
        return;
    }

    FTPLOGI(FTPLOG_LIB, "[onConnect][CTRL]: %s", (ret ? "success" : "failure"));

    ftp_result_output(ctx, FTP_MSG_CONNECT, (ret ? FTP_RET_SUCCESS : FTP_RET_CMD_CONNECT_FAIL), 0, 0);

    if (ret)
    {
        state(ctx, FTP_WAIT220);
        ctx->ftpCon.op->setTimeout(TIMEOUT_RECV_CMD, FTP_CTRL_SOCK);
    }
    else
    {
        setError(FTP_RET_CMD_CONNECT_FAIL);
        ftp_result_output(ctx, FTP_MSG_LOGIN, gFTPErrCode, 0, 0);
    }
}

static void ftp_ctrl_onDisconnect(void *context)
{
    ftpctx_t *ctx = *((ftpctx_t **)context);
    if (!ctx)
    {
        FTPLOGI(FTPLOG_LIB, "[onDisconnect][CTRL]: ftplib is not initialized.");
        return;
    }

    FTPLOGI(FTPLOG_LIB, "[onDisconnect][CTRL]: ftp connection is closed.");

    switch (ctx->state)
    {
    case FTP_WAIT220:
    case FTP_USER:
    case FTP_PASS:
        ftp_result_output(ctx, FTP_MSG_LOGIN, gFTPErrCode, 0, 0);
        break;
    case FTP_QUIT:
        ftp_result_output(ctx, FTP_MSG_LOGOUT, gFTPErrCode, 0, 0);
        break;
    case FTP_SIZE:
        setError(FTP_RET_CMD_BROKEN);
        ftp_result_output(ctx, FTP_MSG_SIZE, gFTPErrCode, 0, 0);
        break;
    case FTP_TYPE:
    case FTP_PASV:
    case FTP_DATA_WAIT_CLOSE:
    case FTP_DATA_WAIT_CONNECT:
    case FTP_DATA_TRANSFER:
    case FTP_DATA_WAIT_RETR:
    case FTP_DATA_WAIT_STOR:
    case FTP_DATA_WAIT_226:
    case FTP_RETR_SIZE:
    case FTP_REST:
    case FTP_RETR:
    case FTP_STOR:
        setError(FTP_RET_CMD_BROKEN);
        ftp_result_output(ctx, ftp_getDataMsg(ctx), gFTPErrCode, 0, 0);
        break;
    case FTP_IDLE:
        break;
    default:
        FTPLOGI(FTPLOG_LIB, "[onDisconnect][CTRL]: Unkown ftp state: %s", ftp_state_names[ctx->state]);
        break;
    }

    ctx->dataCmd = FTP_DATA_NULL;
    ctx->getOpt.getCancel = false;
    state(ctx, FTP_STOP);

    ftp_result_output(ctx, FTP_MSG_DISCONNECT, FTP_RET_SUCCESS, 0, 0);
}

static void ftp_ctrl_onRead(ftp_recv_ret_e ret, void *context, uint8_t *buf, uint32_t buflen)
{
    bool res = false;
    ftpctx_t *ctx = *((ftpctx_t **)context);
    if (!ctx)
    {
        FTPLOGI(FTPLOG_LIB, "[onRead][CTRL]: ftplib is not initialized.");
        return;
    }

    switch (ret)
    {
    case FTP_RECV_SUCC:
    {
        uint32_t linesize = 0;

        /* to kill the command waiting timer */
        ctx->ftpCon.op->killTimeout(FTP_CTRL_SOCK);

        while ((linesize = ftp_getline(buf, buflen)) != 0)
        {
            res = ftp_statemach_act(ctx, buf, linesize);
            if (!res)
            {
                setError(FTP_RET_CMD_REP_ERR);
                ftp_error_rollback(ctx);
            }

            buf += linesize;
            buflen -= linesize;
        }
    }
    break;
    case FTP_RECV_FAIL:
    {
        FTPLOGI(FTPLOG_LIB, "[onRead][CTRL]: failure");
    }
    break;
    case FTP_RECV_TIMEOUT:
    {
        FTPLOGI(FTPLOG_LIB, "[onRead][CTRL]: timeout");
        ftp_command_timeout(ctx);
    }
    break;
    default:
        break;
    }
}

static void ftp_ctrl_onException(void *context)
{
    ftpctx_t *ctx = *((ftpctx_t **)context);
    if (!ctx)
    {
        FTPLOGI(FTPLOG_LIB, "[onException][CTRL]: ftplib is not initialized.");
        return;
    }

    FTPLOGI(FTPLOG_LIB, "[onException][CTRL]: ftp connection is aborted.");

    setError(FTP_RET_CMD_BROKEN);
}

static void ftp_data_onDns(void *context, bool ret)
{
    ftpctx_t *ctx = *((ftpctx_t **)context);
    if (!ctx)
    {
        FTPLOGI(FTPLOG_LIB, "[onDns][DATA]: ftplib is not initialized.");
        return;
    }

    FTPLOGI(FTPLOG_LIB, "[onDns][DATA]: %s", (ret ? "success" : "failure"));

    if (!ret)
    {
        setError(FTP_RET_DATA_DNS_ERR);
        ftp_DataTransferFinish(ctx);
    }
}

static void ftp_data_onConnect(void *context, bool ret)
{
    ftpctx_t *ctx = *((ftpctx_t **)context);
    if (!ctx)
    {
        FTPLOGI(FTPLOG_LIB, "[onConnect][DATA]: ftplib is not initialized.");
        return;
    }

    if (ctx->state == FTP_STOP)
    {
        FTPLOGI(FTPLOG_LIB, "[onConnect][DATA]: ftplib is stop.");
        return;
    }

    FTPLOGI(FTPLOG_LIB, "[onConnect][DATA]: %s", (ret ? "success" : "failure"));

    ftp_result_output(ctx, FTP_MSG_CONNECT_DATA, (ret ? FTP_RET_SUCCESS : FTP_RET_DATA_CONNECT_FAIL), 0, 0);

    if (ret)
    {
        if (ctx->dataCmd == FTP_DATA_GET)
            ftp_state_get(ctx);
        else if (ctx->dataCmd == FTP_DATA_PUT)
            ftp_state_put(ctx);
        else
        {
            FTPLOGI(FTPLOG_LIB, "ERROR ctx->dataCmd:%d", ctx->dataCmd);
            setError(FTP_RET_INTERNAL_ERR);
            ctx->ftpCon.op->disconnect(FTP_DATA_SOCK);
        }
    }
    else
    {
        setError(FTP_RET_DATA_CONNECT_FAIL);
        ftp_DataTransferFinish(ctx);
    }
}

static void ftp_data_onDisconnect(void *context)
{
    ftpctx_t *ctx = *((ftpctx_t **)context);
    if (!ctx)
    {
        FTPLOGI(FTPLOG_LIB, "[onDisconnect][DATA]: ftplib is not initialized.");
        return;
    }

    if (ctx->state == FTP_STOP)
    {
        FTPLOGI(FTPLOG_LIB, "[onDisconnect][DATA]: ftplib is stop.");
        return;
    }

    FTPLOGI(FTPLOG_LIB, "[onDisconnect][DATA]: ftp connection is closed.");

    ftp_result_output(ctx, FTP_MSG_DISCONNECT_DATA, FTP_RET_SUCCESS, 0, 0);

    /*
        1. SIZE/REST/RETR/STOR response code is incorrect, then to disconnect the connection to get here.(RESPONSE ERR)
        2. Connection abort when data transfer.(BROKEN PIPE)
        3. Data received timeout will come here.(DATA TIMEOUT)
        4. Data transfer is finished.
    */

    if (gFTPErrCode != FTP_RET_SUCCESS)
    {
        // data command isn't get or put when connected the data channel.
        if (ctx->state == FTP_DATA_WAIT_CONNECT && gFTPErrCode == FTP_RET_INTERNAL_ERR)
        {
            ftp_DataTransferFinish(ctx);
        }

        // command response error
        if ((ctx->state == FTP_RETR_SIZE || ctx->state == FTP_REST ||
             ctx->state == FTP_RETR || ctx->state == FTP_TYPE_RETRY ||
             ctx->state == FTP_STOR || ctx->state == FTP_DATA_WAIT_CLOSE) &&
            gFTPErrCode == FTP_RET_CMD_REP_ERR)
        {
            ftp_DataTransferFinish(ctx);
        }

        // ftp data timeout
        if (gFTPErrCode == FTP_RET_DATA_TIMEOUT)
        {
            /* Received the data timeout during the transfer */
            if (ctx->state == FTP_DATA_TRANSFER)
            {
                state(ctx, FTP_DATA_WAIT_226);
                ctx->ftpCon.op->setTimeout(TIMEOUT_RECV_CMD, FTP_CTRL_SOCK);
            }
            /* The connection isn't closed by server after receive the 226 */
            if (ctx->state == FTP_DATA_WAIT_CLOSE)
            {
                ftp_DataTransferFinish(ctx);
            }
        }

        // ftp data broken pipe
        if (gFTPErrCode == FTP_RET_DATA_BROKEN)
        {
            if (ctx->state == FTP_DATA_TRANSFER)
            {
                state(ctx, FTP_DATA_WAIT_226);
                ctx->ftpCon.op->setTimeout(TIMEOUT_RECV_CMD, FTP_CTRL_SOCK);
            }
            else
            {
                ftp_DataTransferFinish(ctx);
            }
        }
    }
    else
    {
        if (ctx->state == FTP_RETR)
        {
            state(ctx, FTP_DATA_WAIT_RETR);
        }
        else if (ctx->state == FTP_STOR)
        {
            state(ctx, FTP_DATA_WAIT_STOR);
        }
        else if (ctx->state == FTP_DATA_TRANSFER)
        {
            state(ctx, FTP_DATA_WAIT_226);
            ctx->ftpCon.op->setTimeout(TIMEOUT_RECV_CMD, FTP_CTRL_SOCK);
        }
        else if (ctx->state == FTP_DATA_WAIT_CLOSE)
        {
            ftp_DataTransferFinish(ctx);
        }
        else
        {
            FTPLOGI(FTPLOG_LIB, "[onDisconnect][DATA]: Unkown the state: %s", ftp_state_names[ctx->state]);
        }
    }
}

static void ftp_data_onRead(ftp_recv_ret_e ret, void *context, uint8_t *buf, uint32_t buflen)
{
    ftpctx_t *ctx = *((ftpctx_t **)context);
    if (!ctx)
    {
        FTPLOGI(FTPLOG_LIB, "[onRead][DATA]: ftplib is not initialized.");
        return;
    }

    if (ctx->state == FTP_STOP)
    {
        FTPLOGI(FTPLOG_LIB, "[onRead][DATA]: ftplib is stop.");
        return;
    }

    switch (ret)
    {
    case FTP_RECV_SUCC:
    {
        //FTPLOGI(FTPLOG_LIB, "[onRead][DATA]: success, buflen = %d, getCancel:%d", buflen, (uint32_t)ctx->getOpt.getCancel);

        if (ctx->dataCmd == FTP_DATA_GET && !ctx->getOpt.getCancel)
        {
            ctx->ftpCon.op->killTimeout(FTP_DATA_SOCK);

            if (ctx->getOpt.req_getsize != 0)
            {
                uint32_t lastsize = ctx->getOpt.req_getsize - ctx->getOpt.getsize;
                if (lastsize == 0)
                    return;

                if (buflen >= lastsize)
                {
                    ctx->getOpt.getsize += lastsize;
                    ctx->ftpCon.op->disconnect(FTP_DATA_SOCK);
                    ftp_result_output(ctx, FTP_MSG_TRANSFER_DATA, gFTPErrCode, (uint32_t)NULL, lastsize);
                    return;
                }
            }

            ctx->getOpt.getsize += buflen;
            ftp_result_output(ctx, FTP_MSG_TRANSFER_DATA, gFTPErrCode, (uint32_t)NULL, buflen);
        }
    }
    break;
    case FTP_RECV_FAIL:
    {
        FTPLOGI(FTPLOG_LIB, "[onRead][DATA]: failure");
    }
    break;
    case FTP_RECV_TIMEOUT:
    {
        FTPLOGI(FTPLOG_LIB, "[onRead][DATA]: timeout");
        ftp_data_timeout(ctx);
    }
    break;
    default:
        break;
    }
}

static void ftp_data_onException(void *context)
{
    ftpctx_t *ctx = *((ftpctx_t **)context);
    if (!ctx)
    {
        FTPLOGI(FTPLOG_LIB, "[onException][DATA]: ftplib is not initialized.");
        return;
    }

    if (ctx->state == FTP_STOP)
    {
        FTPLOGI(FTPLOG_LIB, "[onException][DATA]: ftplib is stop.");
        return;
    }

    FTPLOGI(FTPLOG_LIB, "[onException][DATA]: ftp connection is aborted.");

    setError(FTP_RET_DATA_BROKEN);
}

bool FTPLib_init(ftpconfig_t *cfg)
{
    FTPLOGI(FTPLOG_LIB, "api ftplib init.");
    osiSemaphore_t *sem = NULL;

    if (cfg == NULL)
        return false;
    if (cfg->mode != FTP_PASSIVE)
        return false;
    if (cfg->apiType != FTP_API_ASYNC && cfg->apiType != FTP_API_SYNC)
        return false;
    if (cfg->type != FTP_ASCII && cfg->type != FTP_BINARY)
        return false;
    if (cfg->callback == NULL)
        return false;

    if (gFTPLibCtx != NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is busy.");
        return false;
    }

    if (cfg->apiType == FTP_API_SYNC)
    {
        sem = osiSemaphoreCreate(1, 0);
        if (sem == NULL)
        {
            FTPLOGI(FTPLOG_CON, "ftplib create sem failed");
            return false;
        }
    }

    gFTPLibCtx = (ftpctx_t *)malloc(sizeof(ftpctx_t));
    if (gFTPLibCtx == NULL)
        return false;

    memset(gFTPLibCtx, 0, sizeof(ftpctx_t));

    gFTPLibCtx->state = FTP_STOP;
    gFTPLibCtx->ftpc.apiType = cfg->apiType;
    if (cfg->apiType == FTP_API_SYNC)
        gFTPLibCtx->ftpc.apisem = sem;
    gFTPLibCtx->ftpc.mode = cfg->mode;
    gFTPLibCtx->ftpc.type = cfg->type;
    gFTPLibCtx->ftpc.encrypto = cfg->encrypto;
    gFTPLibCtx->ftpc.af_family = AF_INET;
    gFTPLibCtx->callback = cfg->callback;

    const ftpconn_op_t *op = (cfg->encrypto ? &gFTPSConnOp : &gFTPConnOp);
    if (op->create && !op->create())
    {
        FTPLOGI(FTPLOG_LIB, "ftp connection created failed.");
        goto FAIL;
    }
    gFTPLibCtx->ftpCon.op = op;
    gFTPErrCode = FTP_RET_SUCCESS;
    gFTPSize = 0;

    return true;

FAIL:
    memset(gFTPLibCtx, 0, sizeof(ftpctx_t));
    free(gFTPLibCtx);
    gFTPLibCtx = NULL;
    return false;
}

bool FTPLib_uninit(void)
{
    FTPLOGI(FTPLOG_LIB, "api ftplib uninit.");

    if (gFTPLibCtx == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is not initialized.");
        return false;
    }

    const ftpconn_op_t *op = gFTPLibCtx->ftpCon.op;
    if (op == NULL || op->destroy == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib destroy operation is not initialized.");
        return false;
    }

    op->destroy();

    if (gFTPLibCtx->ftpc.apisem != NULL)
    {
        osiSemaphoreDelete(gFTPLibCtx->ftpc.apisem);
        gFTPLibCtx->ftpc.apisem = NULL;
    }

    free(gFTPLibCtx);
    gFTPLibCtx = NULL;

    return true;
}

ftp_ret_t FTPLib_login(char *host, char *port, char *user, char *pass)
{
    FTPLOGI(FTPLOG_LIB, "api ftplib login.");

    if (!host || !port || !user || !pass)
        return FTP_RET_API_ERR;

    if (strlen(host) > (FTP_STRING_SIZE - 1) || strlen(port) > (FTP_STRING_SIZE - 1) ||
        strlen(user) > (FTP_STRING_SIZE - 1) || strlen(pass) > (FTP_STRING_SIZE - 1))
        return FTP_RET_API_ERR;

    if (gFTPLibCtx == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is not initialized.");
        return FTP_RET_FTP_NOT_INITED;
    }

    if (gFTPLibCtx->login)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib had logined already");
        return FTP_RET_FTP_HAD_CONNECTED;
    }

    const ftpconn_op_t *op = gFTPLibCtx->ftpCon.op;
    if (op == NULL || op->connect1 == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib connect operation is not initialized.");
        return FTP_RET_API_ERR;
    }

    ftp_loginopt_t *opt = (ftp_loginopt_t *)&gFTPLibCtx->loginOpt;
    strncpy(opt->host, host, FTP_STRING_SIZE - 1);
    strncpy(opt->port, port, FTP_STRING_SIZE - 1);
    strncpy(opt->username, user, FTP_STRING_SIZE - 1);
    strncpy(opt->passwd, pass, FTP_STRING_SIZE - 1);

    if (!(op->connect1(host, port, &ctrl_netCalback, (void *)&gFTPLibCtx, FTP_CTRL_SOCK)))
        return FTP_RET_API_ERR;

    setError(FTP_RET_SUCCESS);

    if (gFTPLibCtx->ftpc.apiType == FTP_API_SYNC)
        osiSemaphoreAcquire(gFTPLibCtx->ftpc.apisem);

    return gFTPErrCode;
}

ftp_ret_t FTPLib_logout()
{
    FTPLOGI(FTPLOG_LIB, "api ftplib logout.");

    if (gFTPLibCtx == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is not initialized.");
        return FTP_RET_FTP_NOT_INITED;
    }

    if (!gFTPLibCtx->login)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib didn't login to server yet.");
        return FTP_RET_FTP_NOT_CONNECTED;
    }

    if (gFTPLibCtx->state != FTP_IDLE)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is busy.");
        return FTP_RET_FTP_IS_BUSY;
    }

    if (!ftp_state_quit(gFTPLibCtx))
    {
        FTPLOGI(FTPLOG_LIB, "ftp state quit failed.");
        return FTP_RET_API_ERR;
    }

    setError(FTP_RET_SUCCESS);

    if (gFTPLibCtx->ftpc.apiType == FTP_API_SYNC)
        osiSemaphoreAcquire(gFTPLibCtx->ftpc.apisem);

    return gFTPErrCode;
}

ftp_ret_t FTPLib_size(char *file, uint32_t *size)
{
    if (!file || (strlen(file) > (FTP_STRING_SIZE - 1)))
        return FTP_RET_API_ERR;

    if (gFTPLibCtx == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is not initialized.");
        return FTP_RET_FTP_NOT_INITED;
    }

    if ((gFTPLibCtx->ftpc.apiType == FTP_API_SYNC) && (!size))
        return FTP_RET_API_ERR;

    if (!gFTPLibCtx->login)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib didn't login to server yet.");
        return FTP_RET_FTP_NOT_CONNECTED;
    }

    if (gFTPLibCtx->state != FTP_IDLE)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is busy.");
        return FTP_RET_FTP_IS_BUSY;
    }

    gFTPSize = 0;

    if (!ftp_state_size(gFTPLibCtx, file))
    {
        FTPLOGI(FTPLOG_LIB, "ftp state quit failed.");
        return FTP_RET_API_ERR;
    }
    setError(FTP_RET_SUCCESS);

    if (gFTPLibCtx->ftpc.apiType == FTP_API_SYNC)
    {
        osiSemaphoreAcquire(gFTPLibCtx->ftpc.apisem);
        *size = gFTPSize;
    }

    return gFTPErrCode;
}

ftp_ret_t FTPLib_getStart(char *file, uint32_t offset, uint32_t size)
{
    if (!file || (strlen(file) > (FTP_STRING_SIZE - 1)))
        return FTP_RET_API_ERR;

    if (gFTPLibCtx == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is not initialized.");
        return FTP_RET_FTP_NOT_INITED;
    }

    if (!gFTPLibCtx->login)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib didn't login to server yet.");
        return FTP_RET_FTP_NOT_CONNECTED;
    }

    if (gFTPLibCtx->state != FTP_IDLE || gFTPLibCtx->dataCmd != FTP_DATA_NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is busy.");
        return FTP_RET_FTP_IS_BUSY;
    }

    ftp_getopt_t *opt = (ftp_getopt_t *)&gFTPLibCtx->getOpt;
    strncpy(opt->getpath, file, FTP_STRING_SIZE - 1);
    opt->req_getoffset = offset;
    opt->req_getsize = size;
    opt->req_ReadSize = 0;
    opt->getsize = 0;
    opt->getCancel = false;

    if (!ftp_state_type(gFTPLibCtx, FTP_DATA_GET))
    {
        FTPLOGI(FTPLOG_LIB, "ftp state type failed.");
        return FTP_RET_API_ERR;
    }
    setError(FTP_RET_SUCCESS);

    if (gFTPLibCtx->ftpc.apiType == FTP_API_SYNC)
        osiSemaphoreAcquire(gFTPLibCtx->ftpc.apisem);

    return gFTPErrCode;
}

int32_t FTPLib_get(uint8_t *buf, uint32_t buflen)
{
    int32_t ByteNum = 0;

    if (gFTPLibCtx == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is not initialized.");
        return FTP_RET_FTP_NOT_INITED;
    }

    if (!gFTPLibCtx->login)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib didn't login to server yet.");
        return FTP_RET_FTP_NOT_CONNECTED;
    }

    if (gFTPLibCtx->ftpCon.op->read1)
        ByteNum = gFTPLibCtx->ftpCon.op->read1(buf, buflen, FTP_DATA_SOCK);

    if (ByteNum >= 0)
        gFTPLibCtx->getOpt.req_ReadSize += ByteNum;

    if ((gFTPLibCtx->state == FTP_DATA_TRANSFER) || (gFTPLibCtx->state == FTP_DATA_WAIT_CLOSE))
    {
        if ((gFTPLibCtx->getOpt.req_ReadSize == gFTPLibCtx->getOpt.getsize))
            gFTPLibCtx->ftpCon.op->setTimeout(TIMEOUT_RECV_DATA, FTP_DATA_SOCK);
        else
            gFTPLibCtx->ftpCon.op->killTimeout(FTP_DATA_SOCK);
    }

    return ByteNum;
}

ftp_ret_t FTPLib_getStop()
{
    if (gFTPLibCtx == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is not initialized.");
        return FTP_RET_FTP_NOT_INITED;
    }

    if (!gFTPLibCtx->login)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib didn't login to server yet.");
        return FTP_RET_FTP_NOT_CONNECTED;
    }

    if ((gFTPLibCtx->state != FTP_DATA_TRANSFER && gFTPLibCtx->state != FTP_DATA_WAIT_CLOSE) || gFTPLibCtx->dataCmd != FTP_DATA_GET)
    {
        FTPLOGI(FTPLOG_LIB, "ftp internal state error.");
        return FTP_RET_INTERNAL_ERR;
    }

    if (gFTPLibCtx->getOpt.getCancel == true)
    {
        FTPLOGI(FTPLOG_LIB, "getStop is on the way.");
        return FTP_RET_API_ERR;
    }

    gFTPLibCtx->getOpt.getCancel = true;

    gFTPLibCtx->ftpCon.op->killTimeout(FTP_DATA_SOCK);
    if (gFTPLibCtx->ftpCon.op->disconnect(FTP_DATA_SOCK) != true)
        return FTP_RET_API_ERR;

    if (gFTPLibCtx->ftpc.apiType == FTP_API_SYNC)
    {
        while (gFTPLibCtx->dataCmd != FTP_DATA_NULL)
            osiThreadSleep(100);
    }

    return FTP_RET_SUCCESS;
}

ftp_ret_t FTPLib_putStart(char *file)
{
    if (!file || (strlen(file) == 0) || (strlen(file) > (FTP_STRING_SIZE - 1)))
        return FTP_RET_API_ERR;

    if (gFTPLibCtx == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is not initialized.");
        return FTP_RET_FTP_NOT_INITED;
    }

    if (!gFTPLibCtx->login)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib didn't login to server yet.");
        return FTP_RET_FTP_NOT_CONNECTED;
    }

    if (gFTPLibCtx->state != FTP_IDLE || gFTPLibCtx->dataCmd != FTP_DATA_NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is busy.");
        return FTP_RET_FTP_IS_BUSY;
    }

    ftp_putopt_t *opt = (ftp_putopt_t *)&gFTPLibCtx->putOpt;
    strncpy(opt->putpath, file, FTP_STRING_SIZE - 1);
    gFTPLibCtx->getOpt.getCancel = false;

    if (!ftp_state_type(gFTPLibCtx, FTP_DATA_PUT))
    {
        FTPLOGI(FTPLOG_LIB, "ftp state type failed.");
        return FTP_RET_API_ERR;
    }
    setError(FTP_RET_SUCCESS);

    if (gFTPLibCtx->ftpc.apiType == FTP_API_SYNC)
        osiSemaphoreAcquire(gFTPLibCtx->ftpc.apisem);

    return gFTPErrCode;
}

ftp_ret_t FTPLib_put(uint8_t *buf, uint32_t buflen)
{
    bool ret = false;

    if (buf == NULL || buflen > 3072)
        return FTP_RET_API_ERR;

    if (gFTPLibCtx == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is not initialized.");
        return FTP_RET_FTP_NOT_INITED;
    }

    if (!gFTPLibCtx->login)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib didn't login to server yet.");
        return FTP_RET_FTP_NOT_CONNECTED;
    }

    if (gFTPLibCtx->state != FTP_DATA_TRANSFER || gFTPLibCtx->dataCmd != FTP_DATA_PUT)
    {
        FTPLOGI(FTPLOG_LIB, "ftp internal state error");
        return FTP_RET_INTERNAL_ERR;
    }

    gFTPLibCtx->ftpCon.op->killTimeout(FTP_DATA_SOCK);
    ret = gFTPLibCtx->ftpCon.op->send1(buf, buflen, FTP_DATA_SOCK);
    if (ret)
    {
        setError(FTP_RET_SUCCESS);
    }
    else
    {
        setError(FTP_RET_API_ERR);
    }
    gFTPLibCtx->ftpCon.op->setTimeout(TIMEOUT_RECV_DATA * 2, FTP_DATA_SOCK);

    if (gFTPLibCtx->ftpc.apiType == FTP_API_ASYNC)
    {
        ftp_result_output(gFTPLibCtx, FTP_MSG_TRANSFER_DATA, gFTPErrCode, 0, 0);
        return FTP_RET_SUCCESS;
    }
    else
        return gFTPErrCode;
}

ftp_ret_t FTPLib_putStop()
{
    if (gFTPLibCtx == NULL)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib is not initialized.");
        return FTP_RET_FTP_NOT_INITED;
    }

    if (!gFTPLibCtx->login)
    {
        FTPLOGI(FTPLOG_LIB, "ftplib didn't login to server yet.");
        return FTP_RET_FTP_NOT_CONNECTED;
    }

    if ((gFTPLibCtx->state != FTP_DATA_TRANSFER && gFTPLibCtx->state != FTP_DATA_WAIT_CLOSE) || gFTPLibCtx->dataCmd != FTP_DATA_PUT)
    {
        FTPLOGI(FTPLOG_LIB, "ftp internal state error");
        return FTP_RET_INTERNAL_ERR;
    }

    setError(FTP_RET_SUCCESS);

    gFTPLibCtx->ftpCon.op->killTimeout(FTP_DATA_SOCK);
    if (gFTPLibCtx->ftpCon.op->disconnect(FTP_DATA_SOCK) != true)
        return FTP_RET_API_ERR;

    if (gFTPLibCtx->ftpc.apiType == FTP_API_SYNC)
    {
        while (gFTPLibCtx->dataCmd != FTP_DATA_NULL)
            osiThreadSleep(100);
    }

    return FTP_RET_SUCCESS;
}

#endif
