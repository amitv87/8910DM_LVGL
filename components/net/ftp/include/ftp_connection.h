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

#ifndef _FTP_CONNECTION_H_
#define _FTP_CONNECTION_H_

#include "sockets.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _ftp_recv_ret_e
{
    FTP_RECV_SUCC = 0,
    FTP_RECV_FAIL,
    FTP_RECV_TIMEOUT
} ftp_recv_ret_e;

typedef void (*ftp_net_callback_dns_t)(void *context, bool ret);
typedef void (*ftp_net_callback_connect_t)(void *context, bool ret);
typedef void (*ftp_net_callback_disconnect_t)(void *context);
typedef void (*ftp_net_callback_recv_t)(ftp_recv_ret_e ret, void *context, uint8_t *buf, uint32_t buflen);
typedef void (*ftp_net_callback_exception_t)(void *context);

typedef struct _ftp_net_callback_t
{
    ftp_net_callback_dns_t onDns;
    ftp_net_callback_connect_t onConnect;
    ftp_net_callback_disconnect_t onDisconnect;
    ftp_net_callback_recv_t onRead;
    ftp_net_callback_exception_t onException;
} ftp_net_callback_t;

typedef enum _ftp_sock_e
{
    FTP_CTRL_SOCK = 0,
    FTP_DATA_SOCK,
    FTP_IPC_SOCK,
    FTP_MAX_SOCK
} ftp_sock_e;

typedef enum _ftp_con_status_t
{
    FTP_CON_NONE = 0,
    FTP_CON_CONNECTING,
    FTP_CON_CONNECTED,
} ftp_con_status_t;

typedef struct _ftp_sock_t
{
    void *context;
    int sock;
    bool enable;
    int errcode;
    ftp_con_status_t status;
    uint8_t *availBuf;
    uint32_t availableBufSize;
    uint32_t availableSize;
    uint32_t availableoffset;
    struct sockaddr_storage addr;
    size_t addrLen;
    uint32_t recvTimestamp;
    uint32_t recvTimeout;
    ftp_net_callback_t handler;
} ftp_sock_t;

typedef struct _ftp_connection_t
{
    ftp_sock_t sockset[FTP_MAX_SOCK];
    sa_family_t family;
    osiSemaphore_t *sem;
    osiMutex_t *lock;
    osiThread_t *threadConn;
    bool threadEnable;
} ftp_connection_t;

bool ftp_conn_create();
void ftp_conn_destroy();
bool ftp_conn_connect(char *host, char *port, ftp_net_callback_t *cb, void *context, ftp_sock_e flag);
bool ftp_conn_disconnect(ftp_sock_e flag);
bool ftp_conn_send(uint8_t *buf, uint32_t buflen, ftp_sock_e flag);
int32_t ftp_conn_read(uint8_t *buf, uint32_t buflen, ftp_sock_e flag);
void ftp_conn_SetTimeout(uint32_t tm, ftp_sock_e flag);
void ftp_conn_KillTimeout(ftp_sock_e flag);

#ifdef __cplusplus
}
#endif

#endif
