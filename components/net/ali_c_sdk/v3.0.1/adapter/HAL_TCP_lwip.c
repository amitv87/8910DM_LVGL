/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
//#include <netinet/tcp.h>
#include <netdb.h>
#include "infra_config.h"
#include "infra_log.h"

static uint64_t _linux_get_time_ms(void)
{
    uint64_t time_ms = (uint64_t)osiUpTime();

    HAL_Printf("_linux_get_time_ms %lu\n", (uint32_t)time_ms);
    return time_ms;
}

static uint64_t _linux_time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now)
    {
        t_left = t_end - t_now;
    }
    else
    {
        t_left = 0;
    }

    return t_left;
}

/**
 * @brief Establish a TCP connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the TCP server
 * @param [in] port: @n Specify the TCP port of TCP server
 *
 * @return The handle of TCP connection.
   @retval (uintptr_t)(-1): Fail.
   @retval All other values(0 included): Success, the value is handle of this TCP connection.
 */
uintptr_t HAL_TCP_Establish(const char *host, uint16_t port)
{
    struct addrinfo hints;
    struct addrinfo *addrInfoList = NULL;
    struct addrinfo *cur = NULL;
    int fd = 0;
    int rc = 0;
    char service[6];
    uint8_t dns_retry = 0;

    memset(&hints, 0, sizeof(hints));

    HAL_Printf("establish tcp connection with server(host='%s', port=[%u])\n", host, port);

    hints.ai_family = AF_INET; /* only IPv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sprintf(service, "%u", port);

    while (dns_retry++ < 8)
    {
        rc = getaddrinfo(host, service, &hints, &addrInfoList);
        if (rc != 0)
        {
            HAL_Printf("getaddrinfo error[%d], res: %d, host: %s, port: %s\n", dns_retry, rc, host, service);
            osiThreadSleep(1);
            continue;
        }
        else
        {
            break;
        }
    }

    if (rc != 0)
    {
        HAL_Printf("getaddrinfo error(%d), host = '%s', port = [%d]\n", rc, host, port);
        return (uintptr_t)(-1);
    }

    for (cur = addrInfoList; cur != NULL; cur = cur->ai_next)
    {
        if (cur->ai_family != AF_INET)
        {
            HAL_Printf("socket type error\n");
            rc = -1;
            continue;
        }

        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0)
        {
            HAL_Printf("create socket error\n");
            rc = -1;
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0)
        {
            rc = fd;
            break;
        }

        close(fd);
        HAL_Printf("connect error\n");
        rc = -1;
    }

    if (-1 == rc)
    {
        HAL_Printf("fail to establish tcp\n");
    }
    else
    {
        HAL_Printf("success to establish tcp, fd=%d\n", rc);
    }
    freeaddrinfo(addrInfoList);

    return (uintptr_t)rc;
}

/**
 * @brief Destroy the specific TCP connection.
 *
 * @param [in] fd: @n Specify the TCP connection by handle.
 *
 * @return The result of destroy TCP connection.
 * @retval < 0 : Fail.
 * @retval   0 : Success.
 */
int HAL_TCP_Destroy(uintptr_t fd)
{
    int rc;

    /* Shutdown both send and receive operations. */
    rc = shutdown((int)fd, 2);
    if (0 != rc)
    {
        HAL_Printf("shutdown error\n");
        return -1;
    }

    rc = close((int)fd);
    if (0 != rc)
    {
        HAL_Printf("closesocket error\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Write data into the specific TCP connection.
 *        The API will return immediately if 'len' be written into the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval      < 0 : TCP connection error occur..
 * @retval        0 : No any data be write into the TCP connection in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be written in 'timeout_ms' timeout period.

 * @see None.
 */
int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms)
{
    int ret, tcp_fd;
    uint32_t len_sent;
    uint64_t t_end, t_left;
    fd_set sets;
    int net_err = 0;

    t_end = _linux_get_time_ms() + timeout_ms;
    len_sent = 0;
    ret = 1; /* send one time if timeout_ms is value 0 */

    if (fd >= FD_SETSIZE)
    {
        return -1;
    }
    tcp_fd = (int)fd;

    do
    {
        t_left = _linux_time_left(t_end, _linux_get_time_ms());

        if (0 != t_left)
        {
            struct timeval timeout;

            FD_ZERO(&sets);
            FD_SET(tcp_fd, &sets);

            timeout.tv_sec = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;

            ret = select(tcp_fd + 1, NULL, &sets, NULL, &timeout);
            if (ret > 0)
            {
                if (0 == FD_ISSET(tcp_fd, &sets))
                {
                    HAL_Printf("Should NOT arrive\n");
                    /* If timeout in next loop, it will not sent any data */
                    ret = 0;
                    continue;
                }
            }
            else if (0 == ret)
            {
                HAL_Printf("select-write timeout %d\n", tcp_fd);
                break;
            }
            else
            {
                if (EINTR == errno)
                {
                    HAL_Printf("EINTR be caught\n");
                    continue;
                }

                HAL_Printf("select-write fail, ret = select() = %d\n", ret);
                net_err = 1;
                break;
            }
        }

        if (ret > 0)
        {
            ret = send(tcp_fd, buf + len_sent, len - len_sent, 0);
            if (ret > 0)
            {
                len_sent += ret;
            }
            else if (0 == ret)
            {
                HAL_Printf("No data be sent\n");
            }
            else
            {
                if (EINTR == errno)
                {
                    HAL_Printf("EINTR be caught\n");
                    continue;
                }

                HAL_Printf("send fail, ret = send() = %d\n", ret);
                net_err = 1;
                break;
            }
        }
    } while (!net_err && (len_sent < len) && (_linux_time_left(t_end, _linux_get_time_ms()) > 0));

    if (net_err)
    {
        return -1;
    }
    else
    {
        return len_sent;
    }
}

/**
 * @brief Read data from the specific TCP connection with timeout parameter.
 *        The API will return immediately if 'len' be received from the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a TCP connection.
 * @param [out] buf @n A pointer to a buffer to receive incoming data.
 * @param [out] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval       -2 : TCP connection error occur.
 * @retval       -1 : TCP connection be closed by remote server.
 * @retval        0 : No any data be received in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be received in 'timeout_ms' timeout period.

 * @see None.
 */
int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    int ret, err_code, tcp_fd;
    uint32_t len_recv;
    uint64_t t_end, t_left;
    fd_set sets;
    struct timeval timeout;

    t_end = _linux_get_time_ms() + timeout_ms;
    len_recv = 0;
    err_code = 0;

    if (fd >= FD_SETSIZE)
    {
        return -1;
    }
    tcp_fd = (int)fd;

    do
    {
        t_left = _linux_time_left(t_end, _linux_get_time_ms());
        if (0 == t_left)
        {
            break;
        }
        FD_ZERO(&sets);
        FD_SET(tcp_fd, &sets);

        timeout.tv_sec = t_left / 1000;
        timeout.tv_usec = (t_left % 1000) * 1000;

        ret = select(tcp_fd + 1, &sets, NULL, NULL, &timeout);
        if (ret > 0)
        {
            ret = recv(tcp_fd, buf + len_recv, len - len_recv, 0);
            if (ret > 0)
            {
                len_recv += ret;
            }
            else if (0 == ret)
            {
                HAL_Printf("connection is closed\n");
                err_code = -1;
                break;
            }
            else
            {
                if (EINTR == errno)
                {
                    continue;
                }
                HAL_Printf("recv fail\n");
                err_code = -2;
                break;
            }
        }
        else if (0 == ret)
        {
            break;
        }
        else
        {
            if (EINTR == errno)
            {
                continue;
            }
            HAL_Printf("select-recv fail\n");
            err_code = -2;
            break;
        }
    } while ((len_recv < len));

    /* priority to return data bytes if any data be received from TCP connection. */
    /* It will get error code on next calling */
    return (0 != len_recv) ? len_recv : err_code;
}
