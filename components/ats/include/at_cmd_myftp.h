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

#ifndef __AT_MYFTP_H__
#define __AT_MYFTP_H__

#include "at_command.h"
#include "sockets.h"

#define FTP_GET_DATA_BLOCK_LEN_IN_BUFFER(_start, _end, _cycle) ((_start <= _end) ? (_end - _start) : (_end + _cycle - _start))
#define FTP_MOD_BUFF_LEN(_val, _length) ((_val) & (_length - 1))

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

typedef struct _CFW_FTP_CTX
{
    enum FTP_STATE ftpState;
    enum FTP_COMMAND ftp_command;
    SOCKET ctrlSocket, dataSocket, listenSocket;
    uint8_t userName[50];
    uint8_t passWord[50];
    struct sockaddr_storage ser_ip;
    struct sockaddr_storage ser_ip_data;
    ip_addr_t serverIp;
    char *getpath;
    uint32_t req_getoffset, req_getsize, getsize;
    uint8_t *putpath;
    uint32_t req_putsize, req_puteof;
    FTP_TXBUFFER_T tx_buf;
    bool connect;
    FTP_SOCKET_ID_T connected_socket;
    uint16_t port;
    uint16_t data_port;
    bool logined;
    osiTimer_t *FTPTimer;
    uint8_t nDLCI;
    uint8_t nSimID;
    atCmdEngine_t *FTPengine;
} CFW_FTP_CTX_T;

typedef struct _CFW_FTP_CMD
{
    enum FTP_COMMAND cmd_id;
    uint8_t cmd_str[30];
    uint8_t cmd_exp_rst[30];
} CFW_FTP_CMD;

typedef struct
{
    uint8_t CircuBuf[FTP_RXBUFFER_LEN];
    uint32_t Buf_len;
    uint32_t Get;
    uint32_t Put;
    uint8_t NotEmpty;
} FTP_RXBUFF_T;

struct FTP_Param
{
    uint8_t channel;
    uint8_t host[FTP_STRING_SIZE];
    uint16_t port;
    uint8_t username[FTP_STRING_SIZE];
    uint8_t passwd[FTP_STRING_SIZE];
    uint8_t mode;
    uint8_t Tout;
    uint8_t FTPtype;
};

void AT_GPRS_CmdFunc_MYFTPOPEN(atCommand_t *pParam);
void AT_GPRS_CmdFunc_MYFTPCLOSE(atCommand_t *pParam);
void AT_GPRS_CmdFunc_MYFTPSIZE(atCommand_t *pParam);
void AT_GPRS_CmdFunc_MYFTPGET(atCommand_t *pParam);
void AT_GPRS_CmdFunc_MYFTPPUT(atCommand_t *pParam);

#endif
