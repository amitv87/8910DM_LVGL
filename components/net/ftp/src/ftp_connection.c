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
#include "netdb.h"
#include "lwip/sockets.h"
#include "ftp_utils.h"
#include "ftp_connection.h"

typedef enum _ftp_conn_msg_t
{
    FTP_CONN_MSG_CONNECT = 1000,
    FTP_CONN_MSG_DISCONNECT,
    FTP_CONN_MSG_DESTROY,
    FTP_CONN_MSG_SELECT,
} ftp_conn_msg_t;

typedef struct _ftp_connection_param_t
{
    char host[256];
    char port[64];
} ftp_connection_param_t;

typedef struct ftpEvent
{
    uint32_t id;     ///< event identifier
    uint32_t param1; ///< 1st parameter
    uint32_t param2; ///< 2nd parameter
    uint32_t param3; ///< 3rd parameter
    uint32_t param4; ///< 4th parameter
} ftpEvent_t;

typedef struct _ftp_ipaddr_t
{
    char *ipaddr;
    uint16_t port;
} ftp_ipaddr_t;

#define MAX_PACKET_SIZE 1024
#define IPC_DEFAULT_PORT 10100
#define TRANS_DEBUG 0

#define FTP_CONN_LOCK(conn)           \
    do                                \
    {                                 \
        if (conn->lock)               \
            osiMutexLock(conn->lock); \
    } while (0)

#define FTP_CONN_UNLOCK(conn)           \
    do                                  \
    {                                   \
        if (conn->lock)                 \
            osiMutexUnlock(conn->lock); \
    } while (0)

static ftp_connection_t *gFtpCon = NULL;

extern int gettimeofday(struct timeval *tv, void *tz);
static void initCacheData(ftp_sock_t *sockset);
uint32_t ftp_check_Recvtimeout(ftp_connection_t *con, uint32_t def);

static ftp_ipaddr_t *AddrtoStr(struct sockaddr_storage *sa)
{
    ip_addr_t ipaddr;
    char *s_addr = NULL;
    uint16_t uPort = 0;
    struct sockaddr_in *sa4 = NULL;
#ifdef LWIP_IPV6_ON
    struct sockaddr_in6 *sa6 = NULL;
#endif
    static ftp_ipaddr_t ftp_domain;

    if (!sa)
        return NULL;

    switch (sa->ss_family)
    {
    case AF_INET:
    {
        sa4 = (struct sockaddr_in *)sa;
        inet_addr_to_ip4addr(ip_2_ip4(&ipaddr), &sa4->sin_addr);
        s_addr = ip4addr_ntoa(ip_2_ip4(&ipaddr));
        uPort = ntohs(sa4->sin_port);
    }
    break;
#ifdef LWIP_IPV6_ON
    case AF_INET6:
    {
        sa6 = (struct sockaddr_in6 *)sa;
        inet6_addr_to_ip6addr(ip_2_ip6(&ipaddr), &sa6->sin6_addr);
        s_addr = ip6addr_ntoa(ip_2_ip6(&ipaddr));
        uPort = ntohs(sa6->sin6_port);
    }
    break;
#endif
    default:
        return NULL;
    }
    memset(&ftp_domain, 0, sizeof(ftp_ipaddr_t));
    ftp_domain.ipaddr = s_addr;
    ftp_domain.port = uPort;

    return (ftp_ipaddr_t *)&ftp_domain;
}

static sa_family_t _do_dns(ftp_sock_t *sockset, char *host, char *port)
{
    struct addrinfo hints;
    struct addrinfo *servinfo = NULL;

    if (!sockset || !host || !port)
        return AF_UNSPEC;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    /* getaddrinfo() can work with IPV4&IPV6 to replace the gethostbyname() */
    FTPLOGI(FTPLOG_CON, "waiting for getaddrinfo()...");
    if (0 != getaddrinfo(host, port, &hints, &servinfo) || servinfo == NULL)
    {
        FTPLOGI(FTPLOG_CON, "getaddrinfo failed with param %s:%s", host, port);
        return AF_UNSPEC;
    }
    FTPLOGI(FTPLOG_CON, "getaddrinfo() success.");

    if (servinfo != NULL && servinfo->ai_addr != NULL)
    {
        memcpy(&(sockset->addr), servinfo->ai_addr, servinfo->ai_addrlen);
        sockset->addrLen = servinfo->ai_addrlen;
        freeaddrinfo(servinfo);
        return sockset->addr.ss_family;
    }

    FTPLOGI(FTPLOG_CON, "_do_dns() failed.");

    return AF_UNSPEC;
}

static int _do_connect(ftp_sock_t *sockset, sa_family_t family)
{
    int sock = -1;
    uint32_t NBIO = 1;

    if (sockset == NULL || family == AF_UNSPEC)
        return -1;

    sock = socket(family, SOCK_STREAM, 0);
    if (sock < 0)
    {
        FTPLOGI(FTPLOG_CON, "socket() failed");
        return -1;
    }
    sockset->enable = true;

    // initial the available buffer and size
    FTP_CONN_LOCK(gFtpCon);
    initCacheData(sockset);
    FTP_CONN_UNLOCK(gFtpCon);

    // set the socket works with unblock mode default
    if (ioctl(sock, FIONBIO, &NBIO) < 0)
    {
        FTPLOGI(FTPLOG_CON, "ioctl() failed, error: %d(%s)", errno, strerror(errno));
        sockset->errcode = errno;
        close(sock);
        sock = -1;
        goto EXIT;
    }

    ftp_ipaddr_t *dest = AddrtoStr(&sockset->addr);
    if (dest && dest->ipaddr)
        FTPLOGI(FTPLOG_CON, "waiting for connecting host: %s:%d", dest->ipaddr, dest->port);

    int ret = connect(sock, (const struct sockaddr *)&sockset->addr, sockset->addrLen);
    if (ret == 0)
    {
        sockset->status = FTP_CON_CONNECTED;
    }
    else if (ret < 0 && errno == EINPROGRESS)
    {
        sockset->status = FTP_CON_CONNECTING;
    }
    else
    {
        FTPLOGI(FTPLOG_CON, "connect() failed, error: %d(%s)", errno, strerror(errno));
        sockset->status = FTP_CON_NONE;
        sockset->errcode = errno;
        close(sock);
        sock = -1;
    }

EXIT:
    return sock;
}

static bool _do_select(ftp_sock_t *sockset)
{
    ftpEvent_t event = {0};

    FTPLOGI(FTPLOG_CON, "send select msg to ftp clinetd");

    ftp_connection_t *con = gFtpCon;
    if (!con || !con->threadConn || con->sockset[FTP_IPC_SOCK].sock == -1 || sockset->sock == -1)
        return false;

    event.id = FTP_CONN_MSG_SELECT;
    event.param1 = (uint32_t)sockset;

    struct sockaddr_in to4 = {0};
    to4.sin_len = sizeof(to4);
    to4.sin_family = AF_INET;
    to4.sin_port = htons(IPC_DEFAULT_PORT);
    to4.sin_addr.s_addr = PP_HTONL(INADDR_LOOPBACK);
    sendto(con->sockset[FTP_IPC_SOCK].sock, (const void *)&event, sizeof(event), 0, (const struct sockaddr *)&to4, sizeof(to4));

    return true;
}

static void initSocketSet(ftp_sock_t *sockset)
{
    sockset->context = NULL;
    sockset->errcode = 0;
    sockset->status = FTP_CON_NONE;
    sockset->enable = false;
    memset((void *)&(sockset->handler), 0, sizeof(ftp_net_callback_t));
    sockset->recvTimeout = 0;
    sockset->recvTimestamp = 0;
    sockset->addrLen = 0;
    memset((void *)&(sockset->addr), 0, sizeof(struct sockaddr_storage));
}

static void initCacheData(ftp_sock_t *sockset)
{
    if (sockset->availBuf != NULL)
    {
        free(sockset->availBuf);
        sockset->availBuf = NULL;
        sockset->availableBufSize = 0;
    }
    sockset->availableSize = 0;
    sockset->availableoffset = 0;
}

static bool notifyDataInd(ftp_sock_t *sockset)
{
    uint32_t availSize = 0;

    lwip_ioctl(sockset->sock, FIONREAD, &availSize);

    if ((availSize != 0) && sockset->handler.onRead)
    {
        sockset->availableSize = availSize;
        sockset->handler.onRead(FTP_RECV_SUCC, sockset->context, NULL, availSize);
        sockset->enable = false;
        return true;
    }

    return false;
}

static void getMoreData(ftp_sock_t *sockset)
{
    uint32_t availSize = 0;

    if (sockset->availableSize != 0)
        return;

    lwip_ioctl(sockset->sock, FIONREAD, &availSize);

    if ((availSize != 0) && sockset->handler.onRead)
    {
        sockset->availableSize = availSize;
        sockset->handler.onRead(FTP_RECV_SUCC, sockset->context, NULL, availSize);
    }
    else
    {
        _do_select(sockset);
    }
}

static void getRemainData(ftp_sock_t *sockset)
{
    uint32_t numBytes = 0;
    struct sockaddr_storage addr;
    socklen_t addrLen;
    addrLen = sizeof(addr);

    if (sockset->availableSize != 0)
    {
        FTPLOGI(FTPLOG_CON, "read data remaining when disconnect");
        if (sockset->availBuf != NULL)
        {
            free(sockset->availBuf);
            sockset->availBuf = NULL;
            sockset->availableBufSize = 0;
        }

        sockset->availBuf = malloc(sockset->availableSize);
        if (sockset->availBuf != NULL)
        {
            numBytes = recvfrom(sockset->sock, (void *)sockset->availBuf, sockset->availableSize, 0, (struct sockaddr *)&addr, &addrLen);
            if (numBytes > 0)
            {
                sockset->availableSize = numBytes;
                sockset->availableoffset = 0;
                sockset->availableBufSize = sockset->availableSize;
            }
            else
            {
                FTPLOGI(FTPLOG_CON, "read data remaining failed");
                free(sockset->availBuf);
                sockset->availBuf = NULL;
                sockset->availableBufSize = 0;
            }
        }
        else
        {
            FTPLOGI(FTPLOG_CON, "malloc to read data remaining failed");
        }
    }
}
static void ftp_conn_clientd(void *param)
{
    int i = 0;
    uint32_t NBIO = 0;
    ftp_connection_t *connP = (ftp_connection_t *)param;

    FTPLOGI(FTPLOG_CON, "ftp connection thread is running.");

    while (connP->threadEnable)
    {
        struct timeval tv = {5, 0};
        int maxfdp1 = -1;
        fd_set readfds;
        fd_set writefds;
        fd_set exceptfds;
        int result = -1;

        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);

        ftp_sock_t *sockset = connP->sockset;

        for (i = 0; i < FTP_MAX_SOCK; i++)
        {
            if (sockset[i].sock != -1 && sockset[i].enable)
            {
                FD_SET(sockset[i].sock, &readfds);
                if (sockset[i].status == FTP_CON_CONNECTING)
                    FD_SET(sockset[i].sock, &writefds);
                FD_SET(sockset[i].sock, &exceptfds);
                maxfdp1 = maxfdp1 > sockset[i].sock ? maxfdp1 : sockset[i].sock;
            }
        }

        if (maxfdp1 == -1)
        {
            FTPLOGI(FTPLOG_CON, "Error: no socket is avaliable");
            break;
        }

        uint32_t timeout = (time_t)ftp_check_Recvtimeout(connP, (uint32_t)tv.tv_sec);

        /* return -1 means select() failure
        *  when success return the fd size which is ready
        *  return 0 when select() timeout
        */
        tv.tv_sec = (time_t)timeout;
#if TRANS_DEBUG
        FTPLOGI(FTPLOG_CON, "select() tv_sec = %ld", (uint32_t)tv.tv_sec);
#endif
        result = select(maxfdp1 + 1, &readfds, &writefds, &exceptfds, &tv);
#if TRANS_DEBUG
        FTPLOGI(FTPLOG_CON, "select() result = %d", result);
#endif
        if (result < 0)
        {
            FTPLOGI(FTPLOG_CON, "select() failed, errno: %d(%s)", errno, strerror(errno));
            osiThreadSleep(tv.tv_sec * 1000);
        }
        else if (result > 0)
        {
            /*
			* If an exception event happens on the socket, 
			* 1. cfun = 0
			* 2. connect async failed.(eg. wrong IP)
			*/
            for (i = 0; i < FTP_MAX_SOCK; i++)
            {
                if (sockset[i].sock != -1 && FD_ISSET(sockset[i].sock, &exceptfds))
                {
                    FTPLOGI(FTPLOG_CON, "An exception occurred on socket[%d]", i);

                    if (sockset[i].status == FTP_CON_CONNECTED)
                    {
                        if (sockset[i].handler.onException)
                            sockset[i].handler.onException(sockset[i].context);
                        close(sockset[i].sock);
                        if (sockset[i].handler.onDisconnect)
                            sockset[i].handler.onDisconnect(sockset[i].context);
                    }
                    else if (sockset[i].status == FTP_CON_CONNECTING)
                    {
                        if (sockset[i].handler.onConnect)
                            sockset[i].handler.onConnect(sockset[i].context, false);
                        close(sockset[i].sock);
                    }
                    else
                    {
                        FTPLOGI(FTPLOG_CON, "Never come here!");
                        close(sockset[i].sock);
                    }

                    sockset[i].sock = -1;
                    initSocketSet(&sockset[i]);
                }
            }

            // If the socket is set in writefds when connecting, means connect success
            // If the socket is set in writefds & readfds when connecting, means connect
            // success with coming data or connect error. Call the getsockop() to distinguish
            // between the two situations.
            for (i = 0; i <= FTP_DATA_SOCK; i++)
            {
                if (sockset[i].sock != -1 && FD_ISSET(sockset[i].sock, &writefds))
                {
                    if (sockset[i].status == FTP_CON_CONNECTING)
                    {
                        if (FD_ISSET(sockset[i].sock, &readfds))
                        {
                            int error;
                            socklen_t len = sizeof(error);
                            if (getsockopt(sockset[i].sock, SOL_SOCKET, SO_ERROR, (char *)&error, &len) == 0 && error < 0) // connect() error
                            {
                                FTPLOGI(FTPLOG_CON, "connect() failed on socket[%d], error: %d(%s)", i, errno, strerror(errno));
                                close(sockset[i].sock);
                                sockset[i].sock = -1;
                                if (sockset[i].handler.onConnect)
                                    sockset[i].handler.onConnect(sockset[i].context, false);

                                initSocketSet(&sockset[i]);
                                continue;
                            }
                        }

                        FTPLOGI(FTPLOG_CON, "connect() success on socket[%d]", i);
                        sockset[i].status = FTP_CON_CONNECTED;
                        ioctl(sockset[i].sock, FIONBIO, &NBIO);
                        if (sockset[i].handler.onConnect)
                            sockset[i].handler.onConnect(sockset[i].context, true);
                    }
                }
            }

            for (i = 0; i < FTP_MAX_SOCK; i++)
            {
                if (sockset[i].sock != -1 && FD_ISSET(sockset[i].sock, &readfds))
                {
                    if (sockset[i].status == FTP_CON_CONNECTED)
                    {
                        bool ret = false;
                        uint8_t buffer[MAX_PACKET_SIZE] = {0};
                        int numBytes = 0;
                        struct sockaddr_storage addr;
                        socklen_t addrLen;

                        addrLen = sizeof(addr);
#if 1
                        if (i == FTP_DATA_SOCK)
                        {
                            FTP_CONN_LOCK(gFtpCon);
                            ret = notifyDataInd(&sockset[i]);
                            FTP_CONN_UNLOCK(gFtpCon);
                            if (ret)
                                continue;
                        }
#endif

                        /*
        				* We retrieve the data received,
        				* return -1 means receive failed
        				* return receive bytes number when success
        				* return 0 means connection closed
        				*/
                        numBytes = recvfrom(sockset[i].sock, (void *)buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);
                        if (numBytes < 0)
                        {
                            FTPLOGI(FTPLOG_CON, "recvfrom() failed on socket[%d], errno: %d(%s)", i, errno, strerror(errno));
                            sockset[i].errcode = errno;
                            if (sockset[i].handler.onRead)
                                sockset[i].handler.onRead(FTP_RECV_FAIL, sockset[i].context, NULL, 0);

                            /* connection abort (eg, cfun = 0) */
                            if (errno == 113)
                            {
                                close(sockset[i].sock);
                                sockset[i].sock = -1;
                                if (sockset[i].handler.onDisconnect)
                                    sockset[i].handler.onDisconnect(sockset[i].context);

                                initSocketSet(&sockset[i]);
                            }
                        }
                        else if (numBytes > 0)
                        {
#if TRANS_DEBUG
#ifdef LWIP_IPV6_ON
                            char s[INET6_ADDRSTRLEN] = {0};
#else
                            char s[INET_ADDRSTRLEN] = {0};
#endif
                            in_port_t port = 0;
                            if (AF_INET == addr.ss_family)
                            {
                                struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                                inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
                                port = saddr->sin_port;
                            }
#ifdef LWIP_IPV6_ON
                            else if (AF_INET6 == addr.ss_family)
                            {
                                struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&addr;
                                inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
                                port = saddr->sin6_port;
                            }
#endif
                            else
                                FTPLOGI(FTPLOG_CON, "ftp clientd unkown addr.ss_family=%d", addr.ss_family);

                            if (strlen(s) && port)
                                FTPLOGI(FTPLOG_CON, "socket[%d]: %d bytes received from [%s]:%d", i, numBytes, s, ntohs(port));
#endif

                            if (sockset[i].handler.onRead)
                                sockset[i].handler.onRead(FTP_RECV_SUCC, sockset[i].context, buffer, numBytes);
                        }
                        else
                        {
                            FTPLOGI(FTPLOG_CON, "connection closed by peer on socket[%d]", i);

                            close(sockset[i].sock);
                            sockset[i].sock = -1;

                            if (sockset[i].handler.onDisconnect)
                                sockset[i].handler.onDisconnect(sockset[i].context);

                            initSocketSet(&sockset[i]);
                        }
                    }
                }
            }
        }
        else
        {
            // select() timeout
        }
    }
    FTPLOGI(FTPLOG_CON, "ftp clientd will exit...");
    osiSemaphoreRelease(connP->sem);
    osiThreadExit();
}

void ftp_ipc_onRead(ftp_recv_ret_e ret, void *context, uint8_t *buf, uint32_t buflen)
{
    int i = 0;
    uint32_t NBIO = 0;

    ftp_connection_t *connP = (ftp_connection_t *)context;

    if (ret != FTP_RECV_SUCC || buf == NULL || buflen % sizeof(ftpEvent_t) != 0)
    {
        FTPLOGI(FTPLOG_CON, "IPC onRead failed");
        return;
    }

    while (buflen != 0)
    {
        ftp_sock_t *sockset = NULL;
        ftpEvent_t *event = (ftpEvent_t *)buf;

        if (event->id == FTP_CONN_MSG_CONNECT)
        {
            FTPLOGI(FTPLOG_CON, "IPC receive connecting message");

            sockset = &(connP->sockset[event->param3]);
            sockset->context = (void *)event->param4;
            memcpy((void *)(&sockset->handler), (void *)event->param2, sizeof(ftp_net_callback_t));
            free((void *)event->param2);

            ftp_connection_param_t *pm = (ftp_connection_param_t *)event->param1;
            connP->family = _do_dns(sockset, pm->host, pm->port);
            free((void *)pm);

            if (sockset->handler.onDns)
                sockset->handler.onDns(sockset->context, (connP->family != AF_UNSPEC));

            if (connP->family != AF_UNSPEC)
            {
                sockset->sock = _do_connect(sockset, connP->family);
                if (sockset->sock == -1)
                {
                    if (sockset->handler.onConnect)
                        sockset->handler.onConnect(sockset->context, false);

                    initSocketSet(sockset);
                }
                if ((sockset->sock > 0) && sockset->status == FTP_CON_CONNECTED)
                {
                    ioctl(sockset->sock, FIONBIO, &NBIO);
                    if (sockset->handler.onConnect)
                        sockset->handler.onConnect(sockset->context, true);
                }
            }
        }
        else if (event->id == FTP_CONN_MSG_DISCONNECT)
        {
            FTPLOGI(FTPLOG_CON, "IPC receive disconnecting message");

            sockset = &(connP->sockset[event->param1]);
            if (sockset->sock > 0)
            {
                FTP_CONN_LOCK(gFtpCon);
                getRemainData(sockset);
                close(sockset->sock);
                sockset->sock = -1;
                FTP_CONN_UNLOCK(gFtpCon);

                if (sockset->status == FTP_CON_CONNECTING && sockset->handler.onConnect)
                    sockset->handler.onConnect(sockset->context, false);

                if (sockset->status == FTP_CON_CONNECTED && sockset->handler.onDisconnect)
                    sockset->handler.onDisconnect(sockset->context);

                initSocketSet(sockset);
            }
            else
            {
                FTPLOGI(FTPLOG_CON, "discard the message");
            }
        }
        else if (event->id == FTP_CONN_MSG_DESTROY)
        {
            FTPLOGI(FTPLOG_CON, "IPC receive destroy message");

            for (i = 0; i <= FTP_DATA_SOCK; i++)
            {
                sockset = &(connP->sockset[i]);
                if (sockset->sock > 0)
                {
                    close(sockset->sock);
                    sockset->sock = -1;
                    initSocketSet(sockset);
                }
            }

            connP->threadEnable = false;
        }
        else if (event->id == FTP_CONN_MSG_SELECT)
        {
            FTPLOGI(FTPLOG_CON, "IPC receive select message");
            ftp_sock_t *sockset = (ftp_sock_t *)event->param1;
            sockset->enable = true;
        }
        else
        {
            FTPLOGI(FTPLOG_CON, "IPC onRead unkown eventID:%d", event->id);
        }

        buf += sizeof(ftpEvent_t);
        buflen -= sizeof(ftpEvent_t);
    }
}

uint32_t ftp_check_Recvtimeout(ftp_connection_t *con, uint32_t def)
{
    int i = 0;
    int32_t tm = 0;
    uint32_t ret = def;
    struct timeval tv;

    for (i = 0; i < FTP_IPC_SOCK; i++)
    {
        if (con->sockset[i].recvTimestamp > 0)
        {
            gettimeofday(&tv, NULL);
            tm = (uint32_t)tv.tv_sec - con->sockset[i].recvTimestamp;
            tm = con->sockset[i].recvTimeout - tm;
            if (tm <= 0)
            {
                con->sockset[i].recvTimestamp = 0;
                con->sockset[i].recvTimeout = 0;
                con->sockset[i].handler.onRead(FTP_RECV_TIMEOUT, con->sockset[i].context, NULL, 0);
            }
            else
            {
                ret = (tm < ret ? tm : ret);
            }
        }
    }

    return ret;
}

void ftp_conn_SetTimeout(uint32_t tm, ftp_sock_e flag)
{
    struct timeval tv;
    ftp_connection_t *con = gFtpCon;

    if (!con || !con->threadConn)
        return;

    if (flag != FTP_CTRL_SOCK && flag != FTP_DATA_SOCK)
        return;

    if (con->sockset[flag].sock == -1 || con->sockset[flag].status != FTP_CON_CONNECTED)
        return;

    if (0 == gettimeofday(&tv, NULL))
    {
        con->sockset[flag].recvTimestamp = (uint32_t)tv.tv_sec;
        con->sockset[flag].recvTimeout = tm;
    }
}

void ftp_conn_KillTimeout(ftp_sock_e flag)
{
    ftp_connection_t *con = gFtpCon;

    if (!con || !con->threadConn)
        return;

    if (flag != FTP_CTRL_SOCK && flag != FTP_DATA_SOCK)
        return;

    con->sockset[flag].recvTimestamp = 0;
    con->sockset[flag].recvTimeout = 0;
}

bool ftp_conn_create()
{
    int i = 0;
    int ipc_socket = -1;
    ftp_connection_t *connP = NULL;

    FTPLOGI(FTPLOG_CON, "create ftp conneciton");

    if (netif_default == NULL)
        return false;

    if (gFtpCon != NULL)
        return false;

    connP = (ftp_connection_t *)malloc(sizeof(ftp_connection_t));
    if (connP != NULL)
    {
        memset(connP, 0, sizeof(ftp_connection_t));

        for (i = 0; i < FTP_MAX_SOCK; i++)
        {
            connP->sockset[i].sock = -1;
        }

        connP->sem = osiSemaphoreCreate(1, 0);
        if (connP->sem == NULL)
        {
            FTPLOGI(FTPLOG_CON, "create sem failed");
            goto FAILED;
        }

        connP->lock = osiMutexCreate();
        if (connP->lock == NULL)
        {
            FTPLOGI(FTPLOG_CON, "create lock failed");
            goto FAILED;
        }

        ipc_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (ipc_socket < 0)
        {
            FTPLOGI(FTPLOG_CON, "create socket failed");
            goto FAILED;
        }
        struct sockaddr_in to4;
        memset((void *)&to4, 0, sizeof(struct sockaddr_in));
        to4.sin_len = sizeof(struct sockaddr_in);
        to4.sin_family = AF_INET;
        to4.sin_port = htons(IPC_DEFAULT_PORT);
        to4.sin_addr.s_addr = PP_HTONL(INADDR_LOOPBACK);
        if (bind(ipc_socket, (const struct sockaddr *)&to4, sizeof(to4)) < 0)
        {
            FTPLOGI(FTPLOG_CON, "bind socket failed");
            goto FAILED;
        }
        connP->sockset[FTP_IPC_SOCK].sock = ipc_socket;
        connP->sockset[FTP_IPC_SOCK].enable = true;
        connP->sockset[FTP_IPC_SOCK].context = (void *)connP;
        connP->sockset[FTP_IPC_SOCK].handler.onRead = ftp_ipc_onRead;
        connP->sockset[FTP_IPC_SOCK].status = FTP_CON_CONNECTED;

        connP->threadEnable = true;
        connP->threadConn = osiThreadCreate("ftp clientd", ftp_conn_clientd, connP, OSI_PRIORITY_NORMAL, 2048 * 4, 32);
        if (connP->threadConn == NULL)
        {
            FTPLOGI(FTPLOG_CON, "create ftp task failed");
            goto FAILED;
        }

        gFtpCon = connP;
        return true;
    }

FAILED:
    if (connP != NULL)
    {
        if (connP->sem)
        {
            osiSemaphoreDelete(connP->sem);
            connP->sem = NULL;
        }

        if (connP->lock)
        {
            osiMutexDelete(connP->lock);
            connP->lock = NULL;
        }

        if (ipc_socket > 0)
        {
            close(ipc_socket);
            ipc_socket = -1;
        }

        free(connP);
        connP = NULL;
    }

    gFtpCon = NULL;
    return false;
}

void ftp_conn_destroy()
{
    uint32_t i = 0;
    ftpEvent_t event = {0};
    ftp_connection_t *conP = gFtpCon;

    FTPLOGI(FTPLOG_CON, "destroy ftp connection");

    if (conP)
    {
        event.id = FTP_CONN_MSG_DESTROY;

        struct sockaddr_in to4 = {0};
        to4.sin_len = sizeof(to4);
        to4.sin_family = AF_INET;
        to4.sin_port = htons(IPC_DEFAULT_PORT);
        to4.sin_addr.s_addr = PP_HTONL(INADDR_LOOPBACK);
        sendto(conP->sockset[FTP_IPC_SOCK].sock, (const void *)&event, sizeof(event), 0, (const struct sockaddr *)&to4, sizeof(to4));

        // block here to waiting for ftpcliend thread exit.
        FTPLOGI(FTPLOG_CON, "waiting for ftp clientd exit...");
        osiSemaphoreAcquire(conP->sem);

        for (i = 0; i < FTP_MAX_SOCK; i++)
        {
            initCacheData(&(conP->sockset[i]));
        }

        if (conP->sockset[FTP_IPC_SOCK].sock != -1)
        {
            close(conP->sockset[FTP_IPC_SOCK].sock);
            conP->sockset[FTP_IPC_SOCK].sock = -1;
        }

        osiSemaphoreDelete(conP->sem);
        conP->sem = NULL;

        osiMutexDelete(conP->lock);
        conP->lock = NULL;

        free(conP);
        conP = NULL;
        gFtpCon = conP;
    }
}

bool ftp_conn_connect(char *host, char *port, ftp_net_callback_t *cb, void *context, ftp_sock_e flag)
{
    ftpEvent_t event = {0};

    FTPLOGI(FTPLOG_CON, "send connect msg to ftp clinetd");

    if (!host || !port || !cb)
        return false;
    if (flag != FTP_CTRL_SOCK && flag != FTP_DATA_SOCK)
        return false;

    ftp_connection_t *con = gFtpCon;
    if (!con || !con->threadConn || con->sockset[FTP_IPC_SOCK].sock == -1 || con->sockset[flag].sock != -1 || con->sockset[flag].status != FTP_CON_NONE)
        return false;

    ftp_connection_param_t *con_param = (ftp_connection_param_t *)malloc(sizeof(ftp_connection_param_t));
    if (con_param == NULL)
        return false;
    memset(con_param, 0, sizeof(ftp_connection_param_t));
    strncpy(con_param->host, host, sizeof(con_param->host) - 1);
    strncpy(con_param->port, port, sizeof(con_param->port) - 1);

    ftp_net_callback_t *callback = (ftp_net_callback_t *)malloc(sizeof(ftp_net_callback_t));
    if (callback == NULL)
    {
        free(con_param);
        return false;
    }
    memcpy(callback, cb, sizeof(ftp_net_callback_t));

    event.id = FTP_CONN_MSG_CONNECT;
    event.param1 = (uint32_t)con_param;
    event.param2 = (uint32_t)callback;
    event.param3 = (uint32_t)flag;
    event.param4 = (uint32_t)context;

    struct sockaddr_in to4 = {0};
    to4.sin_len = sizeof(to4);
    to4.sin_family = AF_INET;
    to4.sin_port = htons(IPC_DEFAULT_PORT);
    to4.sin_addr.s_addr = PP_HTONL(INADDR_LOOPBACK);
    sendto(con->sockset[FTP_IPC_SOCK].sock, (const void *)&event, sizeof(event), 0, (const struct sockaddr *)&to4, sizeof(to4));

    return true;
}

bool ftp_conn_disconnect(ftp_sock_e flag)
{
    ftpEvent_t event = {0};

    if (flag != FTP_CTRL_SOCK && flag != FTP_DATA_SOCK)
        return false;

    ftp_connection_t *con = gFtpCon;
    if (!con || !con->threadConn || con->sockset[FTP_IPC_SOCK].sock == -1 || con->sockset[flag].sock == -1)
        return false;

    FTPLOGI(FTPLOG_CON, "send disconnect msg to ftp clinetd");

    event.id = FTP_CONN_MSG_DISCONNECT;
    event.param1 = (uint32_t)flag;

    struct sockaddr_in to4 = {0};
    to4.sin_len = sizeof(to4);
    to4.sin_family = AF_INET;
    to4.sin_port = htons(IPC_DEFAULT_PORT);
    to4.sin_addr.s_addr = PP_HTONL(INADDR_LOOPBACK);
    sendto(con->sockset[FTP_IPC_SOCK].sock, (const void *)&event, sizeof(event), 0, (const struct sockaddr *)&to4, sizeof(to4));

    return true;
}

bool ftp_conn_send(uint8_t *buf, uint32_t buflen, ftp_sock_e flag)
{
    int nbSent = 0;

#if TRANS_DEBUG
#ifdef LWIP_IPV6_ON
    char s[INET6_ADDRSTRLEN] = {0};
#else
    char s[INET_ADDRSTRLEN] = {0};
#endif
    in_port_t port = 0;
#endif

    ftp_connection_t *con = gFtpCon;

    if (!con || !con->threadConn)
        return false;
    if (flag != FTP_CTRL_SOCK && flag != FTP_DATA_SOCK)
        return false;

    ftp_sock_t *sockset = &(con->sockset[flag]);

    if (sockset->sock == -1)
        return false;

#if TRANS_DEBUG
    if (AF_INET == con->family)
    {
        struct sockaddr_in *saddr = (struct sockaddr_in *)&sockset->addr;
        inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
        port = saddr->sin_port;
    }
#ifdef LWIP_IPV6_ON
    else if (AF_INET6 == con->family)
    {
        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&sockset->addr;
        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
        port = saddr->sin6_port;
    }
#endif
    else
    {
        FTPLOGI(FTPLOG_CON, "ftp_conn_send unkown family=%d", con->family);
        return false;
    }

    if (strlen(s) && port)
        FTPLOGI(FTPLOG_CON, "sending %d bytes to %s:%d", buflen, s, ntohs(port));
#endif

    int size = CFW_TcpipAvailableBuffer(sockset->sock);
    if (size < buflen)
    {
        FTPLOGI(FTPLOG_CON, "send() failed, available %d size isn't enough", size);
        return false;
    }

    nbSent = send(sockset->sock, buf, buflen, 0);
    if (nbSent == -1)
    {
        FTPLOGI(FTPLOG_CON, "send() failed, error: %d(%s)", errno, strerror(errno));
        sockset->errcode = errno;
        return false;
    }

    return true;
}

int32_t ftp_conn_read(uint8_t *buf, uint32_t buflen, ftp_sock_e flag)
{
    int numBytes = 0;
    uint32_t Readsize = 0;
    struct sockaddr_storage addr;
    socklen_t addrLen;
    addrLen = sizeof(addr);

    ftp_connection_t *con = gFtpCon;

    if (!buf || buflen == 0)
        return -1;

    if (!con || !con->threadConn)
        return -1;

    if (flag != FTP_DATA_SOCK)
        return -1;

    ftp_sock_t *sockset = &(con->sockset[flag]);

    FTP_CONN_LOCK(gFtpCon);

    if ((sockset->availableSize == 0) && (sockset->sock != -1))
    {
        getMoreData(sockset);
        FTP_CONN_UNLOCK(gFtpCon);
        return 0;
    }

    Readsize = (buflen > sockset->availableSize ? sockset->availableSize : buflen);

    if (sockset->sock != -1)
    {
        numBytes = recvfrom(sockset->sock, (void *)buf, Readsize, 0, (struct sockaddr *)&addr, &addrLen);
        if (numBytes > 0)
        {
            sockset->availableSize -= numBytes;
            if (sockset->availableSize == 0)
                getMoreData(sockset);

            FTP_CONN_UNLOCK(gFtpCon);
            return numBytes;
        }
        else
        {
            FTPLOGI(FTPLOG_CON, "read data failed, errno: %d(%s)", errno, strerror(errno));
        }
    }
    else
    {
        if (sockset->availBuf != NULL)
        {
            if (sockset->availableoffset + Readsize <= sockset->availableBufSize)
            {
                memcpy(buf, sockset->availBuf + sockset->availableoffset, Readsize);
                sockset->availableSize -= Readsize;
                sockset->availableoffset += Readsize;
                FTP_CONN_UNLOCK(gFtpCon);
                return Readsize;
            }
            else
            {
                FTPLOGI(FTPLOG_CON, "available buffer read over flow, offset(%d), Readsize(%d), availableSize(%d)", sockset->availableoffset, Readsize, sockset->availableSize);
            }
        }
        else
        {
            FTPLOGI(FTPLOG_CON, "no available buffer when socket is close");
        }
    }

    FTP_CONN_UNLOCK(gFtpCon);
    ftp_conn_disconnect(flag);

    return -1;
}

#endif
