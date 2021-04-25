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

#ifdef CONFIG_AT_FTP_SUPPORT

#ifndef __AT_FTP_H__
#define __AT_FTP_H__

#include "sockets.h"

#define FTP_RXBUFFER_LEN (1)
#define FTP_GET_TIMEOUT_DUR 360
#define FTP_STRING_SIZE 256
#define FTP_SEND_MAX_SIZE 3072

enum FTP_COMMAND
{
    FTP_CONNECT = 1,
    FTP_USER,
    FTP_PASS,
    FTP_SIZE,
    FTP_TYPE,
    FTP_REST,
    FTP_PASV,
    FTP_RETR,
    FTP_LIST,
    FTP_CWD,
    FTP_PWD,
    FTP_DATA_TRANSFER,
    FTP_STOR,
    FTP_ABOR,
    FTP_PORT,
    FTP_QUIT,
};

enum FTP_STATE
{
    FTP_IDLE,
    CHANGEING_CUR_DIR,
    GET_FILE,
    PUT_FILE,
    LIST_DIR,
    LIST_FILE,
    FTP_LOGIN,
    FTP_LOGOUT,
    FTP_TRANSFER,
    FTP_WAIT_DATA,
    FTP_WAIT_DATA_PAUSE,
};

typedef struct
{
    uint8_t buffer[FTP_SEND_MAX_SIZE];
    uint16_t len;
    uint16_t sended;
    uint16_t request;
    uint16_t tx_work;
    osiTimer_t *tmrID;
} FTP_TXBUFFER_T;

typedef enum FTP_SOCKET_CLOSE
{
    FTP_SOCKET_NULL,
    FTP_SOCKET_GET,
    FTP_SOCKET_PUT,
} FTP_SOCKET_ID_T;

struct FTP_Param
{
    uint8_t host[FTP_STRING_SIZE];
    uint16_t port;
    uint8_t username[FTP_STRING_SIZE];
    uint8_t passwd[FTP_STRING_SIZE];
    uint8_t mode;
    uint8_t Tout;
    uint8_t FTPtype;
};

typedef struct _CFW_FTP_CTX
{
    enum FTP_STATE ftpState;
    enum FTP_COMMAND ftp_command;
    SOCKET ctrlSocket, dataSocket, listenSocket;
    uint8_t userName[FTP_STRING_SIZE];
    uint8_t passWord[FTP_STRING_SIZE];
    struct sockaddr_storage ser_ip;
    struct sockaddr_storage ser_ip_data;
    ip_addr_t serverIp;
    char getpath[256];
    bool get_closedelay;
    int32_t req_getoffset, req_getsize, getsize, getparam;
    char putpath[256];
    FTP_TXBUFFER_T tx_buf;
    bool connect;
    FTP_SOCKET_ID_T connected_socket;
    uint16_t port;
    uint16_t data_port;
    bool logined;
    osiTimer_t *FTPTimer;
    uint8_t nSimID;
    atCmdEngine_t *FTPengine;
    bool datamode;
} CFW_FTP_CTX_T;

typedef struct _CFW_FTP_CMD
{
    enum FTP_COMMAND cmd_id;
    uint8_t cmd_str[30];
    uint8_t cmd_exp_rst[30];
} CFW_FTP_CMD;

void AT_FTP_CmdFunc_OPEN(atCommand_t *pParam);
void AT_FTP_CmdFunc_CLOSE(atCommand_t *pParam);
void AT_FTP_CmdFunc_SIZE(atCommand_t *pParam);
void AT_FTP_CmdFunc_GET(atCommand_t *pParam);
void AT_FTP_CmdFunc_PUT(atCommand_t *pParam);
void AT_FTP_CmdFunc_PUTSET(atCommand_t *pParam);
void AT_FTP_CmdFunc_GETSET(atCommand_t *pParam);

#endif /* __CFW_FTP_H__ */
#endif
