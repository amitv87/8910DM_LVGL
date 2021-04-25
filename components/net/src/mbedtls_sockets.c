
#include "mbedtls_sockets.h"
#include "osi_log.h"
/*******************************************************************************
**  MACRO
********************************************************************************/
#define TLSSOCK_HANDLE_MAX_NUM 6

#define MBEDTLS_HANDSHAKE_READ_TIMEOUT 40000
#define MBEDTLS_READ_TIMEOUT 100

#define MBEDSOCKET_TASK_STACK_SIZE (2048)

#define TCPIP_SOCKET_INVALID (-1) /* invalid socket */

#define TLS_READ_TIME 300

#define BUF_SIZE 1024

/*******************************************************************************
**  Type definition
********************************************************************************/

/*******************************************************************************
**  Variables
*******************************************************************************/
static osiThread_t *s_tlssock_task_id = NULL;
static TLSSOCK_CONTEXT *g_tlssock_context[TLSSOCK_HANDLE_MAX_NUM];
static mbedtls_ssl_session s_savedSession;

const char *pers = "rda_sock";
static osiSemaphore_t *s_tlssock_sema[TLSSOCK_HANDLE_MAX_NUM] = {NULL};
static osiSemaphore_t *s_read_sema = NULL;
uint8_t g_tlsConnectStopFlag[TLSSOCK_HANDLE_MAX_NUM] = {0};
uint8_t g_sockFreeIsUsedFlag[TLSSOCK_HANDLE_MAX_NUM] = {0};
uint32_t g_tlsInterRxTime = 300;

const mbedtls_x509_crt_profile tlssock_x509_crt_profile =
    {
        /* Hashes from SHA-1 and above */
        MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA1) |
            MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_RIPEMD160) |
            MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA224) |
            MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA256) |
            MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA384) |
            MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA512),
        0xFFFFFFF, /* Any PK alg    */
        0xFFFFFFF, /* Any curve     */
        1024,
};

extern struct netif *getGprsNetIf(uint8_t nSim, uint8_t nCid);

#define MBEDSOCKET_TRACE(fmt, ...)                                \
    do                                                            \
    {                                                             \
        sys_arch_printf("[mbedtls_socket] " #fmt, ##__VA_ARGS__); \
    } while (0)

static void Mbedsocket_CloseEvent(TLSSOCK_HANDLE handle);
static void Mbedsocket_DataSentEvent(TLSSOCK_HANDLE handle);
static void Mbedsocket_ConnectEvent(TLSSOCK_HANDLE handle, int32_t error_code);

/** Return pointer to ring buffer get position */
static u8_t *
data_buf_get(struct data_buf_t *rb, uint32_t len)
{
    rb->get += len;
    return rb->buf;
}

/** Return number of bytes in ring buffer */
static u16_t
data_buf_len(struct data_buf_t *rb)
{
    u32_t len = rb->put - rb->get;
    if (len > 0xFFFF)
    {
        len += DATA_OUTPUT_RINGBUF_SIZE;
    }
    return (u16_t)len;
}

static void
data_output_append_buf(struct data_buf_t *rb, const void *data, u16_t length)
{
    MBEDSOCKET_TRACE("append_buf,length=%d", length);
    if (rb->get == 0)
    {
        memcpy(rb->buf + rb->put, data, length);
        rb->put += length;
    }
    else
    {
        uint32_t len = rb->put - rb->get;
        memcpy(rb->buf, rb->buf + rb->get, len);
        memcpy(rb->buf + len, data, length);
        rb->put = len + length;
    }
    rb->get = 0;
}

static void data_buf_init(struct data_buf_t *rb)
{

    memset(rb, 0, sizeof(struct data_buf_t));
}
/** Return number of bytes free in ring buffer */
#define data_ringbuf_free(rb) (DATA_OUTPUT_RINGBUF_SIZE - data_buf_len(rb))

/** Return number of bytes possible to read without wrapping around */
#define data_ringbuf_linear_read_length(rb) LWIP_MIN(data_buf_len(rb), (DATA_OUTPUT_RINGBUF_SIZE - (rb)->get))

/*******************************************************************************
 ** Function Delcarations
 ******************************************************************************/
static void Mbedsocket_TaskEntry(void *pData);
static void Mbedsocket_InternalRxTimerFun(uint8_t);
int32_t mbedtls_connect(TLSSOCK_HANDLE handle);
static void Mbedsocket_ConnectEvent(TLSSOCK_HANDLE handle, int32_t error_code);
static void tcpip_rsp(osiEvent_t *pEvent);

static int8_t Mbedsocket_GetSSIDBySocket(SOCKET nSocket)
{
    uint8_t i = 0;
    TLSSOCK_CONTEXT *tlssock_ptr = NULL;

    for (i = 0; i < TLSSOCK_HANDLE_MAX_NUM; i++)
    {
        if (g_tlssock_context[i] != NULL)
        {
            tlssock_ptr = (TLSSOCK_CONTEXT *)g_tlssock_context[i];
            if (tlssock_ptr->server_fd.fd == nSocket)
            {
                return i;
            }
        }
    }
    return -1;
}

static int8_t Mbedsocket_GetSSIDByHandle(TLSSOCK_HANDLE handle)
{
    uint8_t i = 0;
    for (i = 0; i < TLSSOCK_HANDLE_MAX_NUM; i++)
    {
        if (g_tlssock_context[i] != NULL)
        {
            if (g_tlssock_context[i] == (TLSSOCK_CONTEXT *)handle)
            {
                return i;
            }
        }
    }
    return -1;
}

int mbedtls_rda_recv_timeout(void *ctx, unsigned char *buf, size_t len,
                             uint32_t timeout)
{
    int ret = 0;
    int fd = ((mbedtls_net_context *)ctx)->fd;
    uint16_t recv_avail;
    uint32_t time_count = 0; //10ms count
    int8_t ssid;

    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    ssid = Mbedsocket_GetSSIDBySocket(fd);
    if (ssid == -1)
    {
        MBEDSOCKET_TRACE("mbedtls_rda_recv_timeout,can not match Socket");
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    while (time_count < ((timeout + 9) / 10))
    {
        if ((CFW_TcpipSocketIoctl(fd, FIONREAD, (void *)&recv_avail) != 0) || (g_tlsConnectStopFlag[ssid] == 1))
        {
            //g_tlsConnectStopFlag = 0;

            return (MBEDTLS_ERR_NET_RECV_FAILED);
        }
        if (recv_avail > 0)
        {
            ret = recv_avail;
            break;
        }

        if (CFW_TcpipSocketGetStatus(fd) != CFW_SOCKET_CONNECTED)
        {
            return (MBEDTLS_ERR_NET_RECV_FAILED);
        }

        osiThreadSleep(10);
        time_count++;
    }
    /* Zero fds ready means we timed out */
    if (ret == 0)
        return (MBEDTLS_ERR_SSL_TIMEOUT);

    if (ret < 0)
    {
        return (MBEDTLS_ERR_NET_RECV_FAILED);
    }

    /* This call will not block */
    return (mbedtls_net_recv(ctx, buf, len));
}

static void MBED_InternalRxTimerTimeout(void *param)
{
    int32_t ssid = (int32_t)param;
    OSI_LOGI(0, "MBED_InternalRxTimerTimeout,ssid:%d", (int8_t)ssid);
    Mbedsocket_InternalRxTimerFun((int8_t)ssid);
}

osiTimer_t *autoSendTimer;
static void MBEDTLS_TcpipRsp(void *param)
{
    osiEvent_t *pEvent = (osiEvent_t *)param;
    MBEDSOCKET_TRACE("MBEDTLS_TcpipRsp Got : %ld,%ld,%ld,%ld",
                     pEvent->id, pEvent->param1, pEvent->param2, pEvent->param3);

    uint8_t uSocket = (uint8_t)pEvent->param1;
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)pEvent->param3;
    int32_t ssid;

    switch (pEvent->id)
    {
    case EV_CFW_TCPIP_SOCKET_CONNECT_RSP:
        if (tlssock_ptr->server_fd.fd == uSocket)
        {
            int ret;
            CFW_TcpipSocketSetParam(uSocket, NULL, (TLSSOCK_HANDLE)tlssock_ptr);

            ret = mbedtls_connect((TLSSOCK_HANDLE)tlssock_ptr);
            CFW_TcpipSocketSetParam(uSocket, (osiCallback_t)tcpip_rsp, (TLSSOCK_HANDLE)tlssock_ptr);

            Mbedsocket_ConnectEvent((TLSSOCK_HANDLE)tlssock_ptr, ret);
            ssid = (int32_t)Mbedsocket_GetSSIDByHandle((TLSSOCK_HANDLE)tlssock_ptr);
            if (ret == 0 && (-1 != ssid))
            {
                if (autoSendTimer != NULL)
                {
                    MBEDSOCKET_TRACE("EV_CFW_TCPIP_SOCKET_CONNECT_RSP,delete autoSendTimer");
                    osiTimerDelete(autoSendTimer);
                    autoSendTimer = NULL;
                }
                autoSendTimer = osiTimerCreate(osiThreadCurrent(), MBED_InternalRxTimerTimeout, (void *)ssid);
                MBEDSOCKET_TRACE("MBEDTLS_SOCKET: Connect Success, start InternalRximer :%ld ms", g_tlsInterRxTime);
                osiTimerStart(autoSendTimer, g_tlsInterRxTime);
            }
        }
        break;

    case EV_CFW_TCPIP_SOCKET_CLOSE_RSP:
        MBEDSOCKET_TRACE("RECEIVED EV_CFW_TCPIP_SOCKET_CLOSE_RSP ...");
        //tlssock_ptr->server_fd.fd = INVALID_SOCKET;
        break;
    case EV_CFW_TCPIP_ERR_IND:
        if (tlssock_ptr->server_fd.fd == uSocket)
        {
            int8_t err = (int8_t)pEvent->param2;
            const char *string = lwip_strerr(err);

            if (tlssock_ptr->server_fd.fd != INVALID_SOCKET)
            {
                CFW_TcpipSocketClose(tlssock_ptr->server_fd.fd);
            }
            tlssock_ptr->server_fd.fd = INVALID_SOCKET;

            if (tlssock_ptr->state == TLSSOCK_STATE_CONNECTING)
            {
                Mbedsocket_ConnectEvent((TLSSOCK_HANDLE)tlssock_ptr, TLSSOCK_ERR_SOCK_CONNECT_FAIL);
            }
            if (tlssock_ptr->state == TLSSOCK_STATE_CONNECTED)
            {
                Mbedsocket_CloseEvent((TLSSOCK_HANDLE)tlssock_ptr);
            }
            MBEDSOCKET_TRACE("EV_CFW_TCPIP_ERR_IND:%s", string);
        }
        break;

    case EV_CFW_TCPIP_CLOSE_IND:
        MBEDSOCKET_TRACE("EV_CFW_TCPIP_CLOSE_IND");
        if (tlssock_ptr->server_fd.fd == uSocket)
        {
            if (tlssock_ptr->server_fd.fd != INVALID_SOCKET)
            {
                CFW_TcpipSocketClose(tlssock_ptr->server_fd.fd);
            }
            tlssock_ptr->server_fd.fd = INVALID_SOCKET;
            if (tlssock_ptr->state == TLSSOCK_STATE_CONNECTING)
            {
                Mbedsocket_ConnectEvent((TLSSOCK_HANDLE)tlssock_ptr, TLSSOCK_ERR_SOCK_CONNECT_FAIL);
            }
            if (tlssock_ptr->state == TLSSOCK_STATE_CONNECTED)
            {
                Mbedsocket_CloseEvent((TLSSOCK_HANDLE)tlssock_ptr);
            }
            MBEDSOCKET_TRACE("EV_CFW_TCPIP_CLOSE_IND");
        }
        break;

    case EV_CFW_TCPIP_SOCKET_SEND_RSP:
        if (tlssock_ptr->server_fd.fd == uSocket)
        {
            if (tlssock_ptr->state == TLSSOCK_STATE_CONNECTED)
            {
                MBEDSOCKET_TRACE("goto Mbedsocket_DataSentEvent");
                Mbedsocket_DataSentEvent((TLSSOCK_HANDLE)tlssock_ptr);
            }
        }
        MBEDSOCKET_TRACE("EV_CFW_TCPIP_SOCKET_SEND_RSP");
        break;

    case EV_CFW_TCPIP_REV_DATA_IND:
        MBEDSOCKET_TRACE("EV_CFW_TCPIP_REV_DATA_IND");
        if (tlssock_ptr->server_fd.fd == uSocket)
        {
            ssid = Mbedsocket_GetSSIDByHandle((TLSSOCK_HANDLE)tlssock_ptr);
            if (-1 != ssid)
            {
                MBEDSOCKET_TRACE("goto Mbedsocket_InternalRxTimerFun");
                Mbedsocket_InternalRxTimerFun(ssid);
            }
        }
        break;
    default:
        MBEDSOCKET_TRACE("unexpect asynchrous event/response %ld", pEvent->id);
        break;
    }
    free(pEvent);
}

static void tcpip_rsp(osiEvent_t *pEvent)
{
    osiThreadCallback(s_tlssock_task_id, MBEDTLS_TcpipRsp, (void *)pEvent);
}

static bool socket_connect(TLSSOCK_HANDLE handle, uint32_t ip, uint16_t port)
{
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    CFW_TCPIP_SOCKET_ADDR sockAddr;
    int result;
    //uint8_t nSim = AT_SIM_ID(g_TLS_DLCI);

    mbedtls_net_context *ctx;
    uint8_t nCid;
    uint8_t nSimid;

    ctx = &(tlssock_ptr->server_fd);
    nCid = tlssock_ptr->nCid;
    nSimid = tlssock_ptr->nSimid;

    if (ctx->fd != INVALID_SOCKET)
    {
        CFW_TcpipSocketClose(ctx->fd);
        ctx->fd = TCPIP_SOCKET_INVALID;
    }

    ctx->fd = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_STREAM, CFW_TCPIP_IPPROTO_TCP,
                                (COS_CALLBACK_FUNC_T)tcpip_rsp, (TLSSOCK_HANDLE)handle);
    if (ctx->fd == INVALID_SOCKET)
    {
        OSI_LOGI(0, "socket_connect: Create socket[%d] fail", ctx->fd);
        return false;
    }
    tlssock_ptr->sock_id = ctx->fd;
    OSI_LOGI(0, "socket_connect: Create socket nCid[%d] nSimid[%d]", nCid, nSimid);
    struct netif *netif_bind = NULL;

    if (nCid == TLSSOCK_INVALID_CID)
    {
        OSI_LOGI(0, "socket_connect: TLSSOCK_INVALID_CID");
        netif_bind = netif_default;
    }
    else
    {
        netif_bind = getGprsNetIf(nSimid, nCid);
    }
    if (netif_bind == NULL)
    {
        OSI_LOGI(0, "socket_connect: netif_bind = NULL");
        return FALSE;
    }
    ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(netif_bind);
    struct sockaddr_in to4 = {0};
    to4.sin_len = sizeof(to4);
    to4.sin_family = AF_INET;
    to4.sin_port = 0;
    inet_addr_from_ip4addr(&(to4.sin_addr), ip_addr);

    result = CFW_TcpipSocketBind(ctx->fd, (CFW_TCPIP_SOCKET_ADDR *)&to4, sizeof(sockAddr));
    if (0 != result)
    {
        uint32_t sock_error_code = 0;
        sock_error_code = CFW_TcpipGetLastError();
        CFW_TcpipSocketClose(ctx->fd);
        ctx->fd = INVALID_SOCKET;

        OSI_LOGI(0, "socket_connect: CFW_TcpipSocketBind result[%d-%ld]", result, sock_error_code);
        return FALSE;
    }

    sockAddr.sin_len = 0;
    sockAddr.sin_family = CFW_TCPIP_AF_INET;
    sockAddr.sin_port = htons(port);
    sockAddr.sin_addr.s_addr = ip;
    tlssock_ptr->state = TLSSOCK_STATE_CONNECTING;
    result = CFW_TcpipSocketConnect(ctx->fd, &sockAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
    if (SOCKET_ERROR == result)
    {
        OSI_LOGI(0, "Socket[%d] connect fail", ctx->fd);
        CFW_TcpipSocketClose(ctx->fd);
        ctx->fd = INVALID_SOCKET;
        return FALSE;
    }

    return TRUE;
}

void mbedtlsSocket_Init(void)
{
    if (s_tlssock_task_id != NULL)
        return;
    //s_read_sema = COS_CreateSemaphore(1);
    if (s_read_sema != NULL)
    {
        osiSemaphoreDelete(s_read_sema);
        s_read_sema = NULL;
    }
    s_read_sema = osiSemaphoreCreate(1, 1);
    memset(&s_savedSession, 0, sizeof(mbedtls_ssl_session));
    mbedtls_debug_set_threshold(3);

    s_tlssock_task_id = osiThreadCreate("AT TLS Task", Mbedsocket_TaskEntry, NULL,
                                        MBEDSOCK_TASK_PRIORITY, MBEDSOCKET_TASK_STACK_SIZE * 10,
                                        32);
}

static void mbedtls_debug(void *ctx, int level,
                          const char *file, int line,
                          const char *str)
{
    //((void) level);
    MBEDSOCKET_TRACE("%s:%04d: %s", file, line, str);
}

static void Mbedsocket_ConnectEvent(TLSSOCK_HANDLE handle, int32_t error_code)
{

    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    int8_t ssid;

    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("Mbedsocket_ConnectEvent,tlssock_ptr is null");
        return;
    }

    ssid = Mbedsocket_GetSSIDByHandle(handle);
    if (ssid == -1)
    {
        MBEDSOCKET_TRACE("Mbedsocket_ConnectEvent,can not match handle");
        return;
    }

    if (1 == g_tlsConnectStopFlag[ssid])
    {
        MBEDSOCKET_TRACE("Mbedsocket_ConnectEvent 1 == g_tlsConnectStopFlag !!!!");
        return;
    }
    MBEDSOCKET_TRACE("++++++Mbedsocket_ConnectEvent send,error_code=%ld", error_code);
    COS_CALLBACK_FUNC_T func = tlssock_ptr->notify_callback;
    if (func == NULL)
    {
        osiEvent_t ev = {0, 0, 0, 0};
        ev.id = EV_CFW_TLSSOCK_CONNECT_EVENT_IND;
        ev.param1 = error_code;
        ev.param2 = handle;
        osiEventSend(tlssock_ptr->notify_task, &ev);
    }
    else
    {
        osiEvent_t *ev = malloc(sizeof(osiEvent_t));
        ev->id = EV_CFW_TLSSOCK_CONNECT_EVENT_IND;
        ev->param1 = error_code;
        ev->param2 = handle;
        ev->param3 = (uint32_t)tlssock_ptr->funcParam;
        osiThreadCallback((osiThread_t *)tlssock_ptr->notify_task, func, ev);
    }
}

static void Mbedsocket_CloseEvent(TLSSOCK_HANDLE handle)
{
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("Mbedsocket_CloseEvent,tlssock_ptr is null");
        return;
    }
    COS_CALLBACK_FUNC_T func = tlssock_ptr->notify_callback;
    if (func == NULL)
    {
        osiEvent_t ev = {0, 0, 0, 0};
        ev.id = EV_CFW_TLSSOCK_CLOSE_EVENT_IND;
        ev.param2 = handle;

        osiEventSend(tlssock_ptr->notify_task, &ev);
    }
    else
    {
        osiEvent_t *ev = malloc(sizeof(osiEvent_t));
        ev->id = EV_CFW_TLSSOCK_CLOSE_EVENT_IND;
        ev->param1 = 0;
        ev->param2 = handle;
        ev->param3 = (uint32_t)tlssock_ptr->funcParam;
        osiThreadCallback(tlssock_ptr->notify_task, func, ev);
    }
}

static void Mbedsocket_DataSentEvent(TLSSOCK_HANDLE handle)
{
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("Mbedsocket_DataSentEvent,tlssock_ptr is null");
        return;
    }
    COS_CALLBACK_FUNC_T func = tlssock_ptr->notify_callback;
    if (func == NULL)
    {
        osiEvent_t ev = {0, 0, 0, 0};
        ev.id = EV_CFW_TLSSOCK_DATA_SENT_EVENT_IND;
        ev.param2 = handle;

        osiEventSend(tlssock_ptr->notify_task, &ev);
    }
    else
    {
        MBEDSOCKET_TRACE("Mbedsocket_DataSentEvent,osiThreadCallback send event");
        osiEvent_t *ev = malloc(sizeof(osiEvent_t));
        ev->id = EV_CFW_TLSSOCK_DATA_SENT_EVENT_IND;
        ev->param1 = 0;
        ev->param2 = handle;
        ev->param3 = (uint32_t)tlssock_ptr->funcParam;
        osiThreadCallback(tlssock_ptr->notify_task, func, ev);
    }
}

static void Mbedsocket_InternalRxTimerFun(uint8_t ssid)
{
    osiEvent_t ev = {0, 0, 0, 0};

    ev.id = EV_CFW_TLSSOCK_INTERNAL_READ_REQ;
    ev.param1 = ssid;

    osiEventSend(s_tlssock_task_id, &ev);
}

static void Mbedsocket_ReadEvent(TLSSOCK_HANDLE handle)
{
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;

    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("Mbedsocket_ReadEvent,tlssock_ptr is null");
        return;
    }
    COS_CALLBACK_FUNC_T func = tlssock_ptr->notify_callback;
    if (func == NULL)
    {
        osiEvent_t ev = {0, 0, 0, 0};
        ev.id = EV_CFW_TLSSOCK_READ_EVENT_IND;
        ev.param2 = handle;
        osiEventSend(tlssock_ptr->notify_task, &ev);
    }
    else
    {
        MBEDSOCKET_TRACE("++++Mbedsocket_ReadEvent,EV_CFW_TLSSOCK_READ_EVENT_IND");
        osiEvent_t *ev = malloc(sizeof(osiEvent_t));
        ev->id = EV_CFW_TLSSOCK_READ_EVENT_IND;
        ev->param1 = 0;
        ev->param2 = handle;
        ev->param3 = (uint32_t)tlssock_ptr->funcParam;
        osiThreadCallback(tlssock_ptr->notify_task, func, ev);
    }
}

static void Mbedsocket_InternalRead(TLSSOCK_HANDLE handle)
{
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    int space = 0;
    struct data_buf_t *rx_decrypt_buf = &(tlssock_ptr->rx_buf);
    char buf[DATA_OUTPUT_RINGBUF_SIZE];
    uint32_t len;
    int32_t ret = -1;
    MBEDSOCKET_TRACE("Mbedsocket: Mbedsocket_InternalRead");

    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("Mbedsocket_InternalRead,tlssock_ptr is null");
        return;
    }

    if (CFW_TcpipSocketGetStatus(tlssock_ptr->server_fd.fd) != CFW_SOCKET_CONNECTED)
    {
        //Mbedsocket_CloseEvent(handle);

        MBEDSOCKET_TRACE("CFW_TcpipSocketGetStatus(tlssock_ptr->server_fd.fd)=%d", CFW_TcpipSocketGetStatus(tlssock_ptr->server_fd.fd));
        return;
    }

    do
    {
        MBEDSOCKET_TRACE("wait s_read_sema");
        osiSemaphoreAcquire(s_read_sema);
        space = DATA_OUTPUT_RINGBUF_SIZE - data_buf_len(rx_decrypt_buf);
        MBEDSOCKET_TRACE("ringbuflen=%d,space=%d", data_buf_len(rx_decrypt_buf), space);
        osiSemaphoreRelease(s_read_sema);
        MBEDSOCKET_TRACE("release s_read_sema");
        if (space <= 0)
        {
            return;
        }
        else
        {
            memset(buf, 0, DATA_OUTPUT_RINGBUF_SIZE);
            len = (space < DATA_OUTPUT_RINGBUF_SIZE) ? space : DATA_OUTPUT_RINGBUF_SIZE;
        }
        CFW_TcpipSocketSetParam(tlssock_ptr->sock_id, (osiCallback_t)tcpip_rsp, handle);
        ret = mbedtls_ssl_read(&(tlssock_ptr->ssl), (unsigned char *)buf, len);
        MBEDSOCKET_TRACE("Mbedsocket: mbedtls_ssl_read ret = %ld", ret);

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            return;
        }

        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
        {
            //Mbedsocket_CloseEvent(handle);
            return;
        }

        if (ret < 0)
        {
            break;
        }

        if (ret == 0)
        {
            break;
        }
        else
        {
            MBEDSOCKET_TRACE("wait s_read_sema");
            osiSemaphoreAcquire(s_read_sema);
            data_output_append_buf(rx_decrypt_buf, buf, ret);
            osiSemaphoreRelease(s_read_sema);
            MBEDSOCKET_TRACE("release s_read_sema");
            if (mbedtls_ssl_get_bytes_avail(&(tlssock_ptr->ssl)) == 0 || data_buf_len(rx_decrypt_buf) == DATA_OUTPUT_RINGBUF_SIZE)
            {
                MBEDSOCKET_TRACE("Mbedsocket: break");
                break;
            }
        }
    } while (1);
    MBEDSOCKET_TRACE("Mbedsocket: tlssock_ptr->rx_data_len=%ld;data_buf_len(rx_decrypt_buf)=%u", tlssock_ptr->rx_data_len, data_buf_len(rx_decrypt_buf));

    MBEDSOCKET_TRACE("wait s_read_sema");
    osiSemaphoreAcquire(s_read_sema);
    if (((tlssock_ptr->rx_data_len == 0) && (data_buf_len(rx_decrypt_buf) > 0)) ||
        ((tlssock_ptr->report_flag == 1) && (data_buf_len(rx_decrypt_buf) > 0)))
    {
        Mbedsocket_ReadEvent(handle);
    }
    tlssock_ptr->rx_data_len = data_buf_len(rx_decrypt_buf); //AT_RingBuf_DataSize
    osiSemaphoreRelease(s_read_sema);
    MBEDSOCKET_TRACE("release s_read_sema");
}

static int32_t tls_init(TLSSOCK_HANDLE handle, HANDLE notify_task)
{
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    //char *pBuf = NULL;
    int ret;
    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("_create,tlssock_ptr is null");
        return -1;
    }

    tlssock_ptr->server_fd.fd = INVALID_SOCKET;
#if 0
    pBuf = (char *)malloc(rx_buf_size);
    if (NULL == pBuf)
    {
        tlssock_ptr->is_used = FALSE;
        return -1;
    }
#endif
    //AT_RingBuf_Init(&(tlssock_ptr->rx_buf), pBuf, rx_buf_size);
    data_buf_init(&(tlssock_ptr->rx_buf));

    /* Initialize the RNG and the session data */
    MBEDSOCKET_TRACE("MbedtlsSock: mbedtls init start!!!");
    mbedtls_net_init(&(tlssock_ptr->server_fd));
    mbedtls_ssl_init(&(tlssock_ptr->ssl));
    mbedtls_ssl_config_init(&(tlssock_ptr->conf));
    mbedtls_x509_crt_init(&(tlssock_ptr->cacert));
    mbedtls_x509_crt_init(&(tlssock_ptr->clicert));
    mbedtls_pk_init(&(tlssock_ptr->pkey));
    mbedtls_ctr_drbg_init(&(tlssock_ptr->ctr_drbg));

    mbedtls_ssl_conf_dbg(&(tlssock_ptr->conf), mbedtls_debug, NULL);

    MBEDSOCKET_TRACE("MbedtlsSock: Seeding the random number generator...");
    mbedtls_entropy_init(&(tlssock_ptr->entropy));
    MBEDSOCKET_TRACE("tlssock_ptr->entropy.source_count:%d", tlssock_ptr->entropy.source_count);
    if ((ret = mbedtls_ctr_drbg_seed(&(tlssock_ptr->ctr_drbg), mbedtls_entropy_func,
                                     &(tlssock_ptr->entropy), (const unsigned char *)pers,
                                     strlen(pers))) != 0)
    {
        MBEDSOCKET_TRACE("MbedtlsSock: failed --- mbedtls_ctr_drbg_seed returned %d", ret);
        //free(pBuf);
        return -1;
    }
#if 0
    /**  Initialize CA certificates **/
    MBEDSOCKET_TRACE("MbedtlsSock: Loading the CA root certificate ...len=%d", strlen(iotx_ca_crt) + 1);
    MBEDSOCKET_TRACE("MbedtlsSock: +++&(tlssock_ptr->cacert)=%p,tlssock_ptr=%p", &(tlssock_ptr->cacert), tlssock_ptr);

    ret = mbedtls_x509_crt_parse(&(tlssock_ptr->cacert),
                                 (const unsigned char *)iotx_ca_crt,
                                 strlen(iotx_ca_crt) + 1);
    if (ret < 0)
    {
        MBEDSOCKET_TRACE("MbedtlsSock: failed --- mbedtls_x509_crt_parse returned -0x%x", -ret);
        //free(pBuf);
        return -1;
    }

    ret = mbedtls_x509_crt_parse(&(tlssock_ptr->clicert),
                                 (const unsigned char *)client_cert,
                                 strlen(client_cert) + 1);
    if (ret < 0)
    {
        MBEDSOCKET_TRACE("TLSSOCK_CFG_TYPE_CLI_CERT: failed --- mbedtls_x509_crt_parse returned -0x%x", -ret);
        return -1;
    }

    ret = mbedtls_pk_parse_key(&(tlssock_ptr->pkey), (const unsigned char *)client_key,
                               strlen(client_key) + 1, NULL, 0);
    if (ret < 0)
    {
        MBEDSOCKET_TRACE("TLSSOCK_CFG_TYPE_CLI_KEY: failed --- mbedtls_pk_parse_key returned -0x%x", -ret);
        return -1;
    }

    MBEDSOCKET_TRACE("MbedtlsSock: Loading the certificate ...\r\n");
#endif

    /* Setup default config */
    MBEDSOCKET_TRACE("MbedtlsSock: Setting up the SSL/TLS structure...\r\n");
    if ((ret = mbedtls_ssl_config_defaults(&(tlssock_ptr->conf),
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        MBEDSOCKET_TRACE("MbedtlsSock: failed --- mbedtls_ssl_config_defaults returned %d", ret);
        return ret;
    }

    mbedtls_ssl_conf_min_version(&(tlssock_ptr->conf),
                                 MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_1);

    mbedtls_ssl_conf_max_version(&(tlssock_ptr->conf), MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_1);

    mbedtls_ssl_conf_cert_profile(&(tlssock_ptr->conf), &tlssock_x509_crt_profile);

    /* Enable ARC4 */
    mbedtls_ssl_conf_arc4_support(&(tlssock_ptr->conf), MBEDTLS_SSL_ARC4_ENABLED);

    /* OPTIONAL is not optimal for security,
        * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode(&(tlssock_ptr->conf), MBEDTLS_SSL_VERIFY_OPTIONAL);

    tlssock_ptr->notify_task = notify_task;
    tlssock_ptr->report_flag = 0;
    tlssock_ptr->nCid = TLSSOCK_INVALID_CID;
    tlssock_ptr->nSimid = 0;
    tlssock_ptr->state = TLSSOCK_STATE_CREATED;

    return 0;
}

void _free(TLSSOCK_HANDLE handle)
{
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("_free,tlssock_ptr is null");
        return;
    }

    if (tlssock_ptr->server_fd.fd != INVALID_SOCKET)
    {
        CFW_TcpipSocketSetParam(tlssock_ptr->server_fd.fd, NULL, (TLSSOCK_HANDLE)tlssock_ptr);
        CFW_TcpipSocketClose(tlssock_ptr->server_fd.fd);
        tlssock_ptr->server_fd.fd = INVALID_SOCKET;
    }

    mbedtls_ssl_close_notify(&(tlssock_ptr->ssl));

    mbedtls_net_free(&(tlssock_ptr->server_fd));

    mbedtls_x509_crt_free(&(tlssock_ptr->cacert));
    mbedtls_x509_crt_free(&(tlssock_ptr->clicert));
    mbedtls_pk_free(&(tlssock_ptr->pkey));
    if (tlssock_ptr->ssl.p_timer)
    {
        free(tlssock_ptr->ssl.p_timer);
        tlssock_ptr->ssl.p_timer = NULL;
    }
    mbedtls_ssl_free(&(tlssock_ptr->ssl));
    mbedtls_ssl_config_free(&(tlssock_ptr->conf));
    mbedtls_ctr_drbg_free(&(tlssock_ptr->ctr_drbg));
    mbedtls_entropy_free(&(tlssock_ptr->entropy));
}

int32_t ConnectStart(TLSSOCK_HANDLE handle, IP_ADDR ip, uint16_t port)
{
    int ret = 0;
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    char *ip_str_ptr;
    char port_str[6] = {0};
    in_addr s_ip;

    tlssock_ptr->fip = ip;
    tlssock_ptr->fport = port;

    s_ip.s_addr = ip;
    ip_str_ptr = inet_ntoa(s_ip);
    sprintf(port_str, "%d", port);

    /** 1. Start the connection */
    MBEDSOCKET_TRACE("ConnectStart: Connecting to tcp/%s/%s...\r\n", ip_str_ptr, port_str);
    if ((ret = socket_connect(handle, ip, port)) != TRUE)
    {
        MBEDSOCKET_TRACE("socket_connect: failed --- mbedtls_net_connect returned %d", ret);
        ret = TLSSOCK_ERR_SOCK_CONNECT_FAIL;
        goto exit;
    }
    ret = 0;

exit:
    return ret;
}

int32_t mbedtls_connect(TLSSOCK_HANDLE handle)
{
    int ret = 0;
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    uint32_t flags;
    mbedtls_net_context *ctx;

    ctx = &(tlssock_ptr->server_fd);

    //mbedtls_net_set_block(ctx);
    mbedtls_net_set_block(ctx);

    /** 2. Setup stuff **/
#if defined(MBEDTLS_SSL_SESSION_TICKETS)
    mbedtls_ssl_conf_session_tickets(&(tlssock_ptr->conf), MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#endif
    mbedtls_ssl_conf_ca_chain(&(tlssock_ptr->conf), &(tlssock_ptr->cacert), NULL);
    mbedtls_ssl_conf_rng(&(tlssock_ptr->conf), mbedtls_ctr_drbg_random, &(tlssock_ptr->ctr_drbg));
    //mbedtls_ssl_conf_read_timeout(&(tlssock_ptr->conf), MBEDTLS_READ_TIMEOUT);

    if ((ret = mbedtls_ssl_conf_own_cert(&(tlssock_ptr->conf), &(tlssock_ptr->clicert),
                                         &(tlssock_ptr->pkey))) != 0)
    {
        MBEDSOCKET_TRACE(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
        goto exit;
    }

    if ((ret = mbedtls_ssl_setup(&(tlssock_ptr->ssl), &(tlssock_ptr->conf))) != 0)
    {
        MBEDSOCKET_TRACE("TlsSock: failed --- mbedtls_ssl_setup returned %d", ret);
        //ret = TLSSOCK_ERR_SSL_SETUP_FAIL;
        goto exit;
    }
    mbedtls_ssl_conf_read_timeout(&(tlssock_ptr->conf), MBEDTLS_HANDSHAKE_READ_TIMEOUT);
    ;
    mbedtls_ssl_set_bio(&(tlssock_ptr->ssl), &(tlssock_ptr->server_fd),
                        mbedtls_net_send, mbedtls_net_recv, mbedtls_rda_recv_timeout);

    /*if ((ret = mbedtls_ssl_set_session(&(tlssock_ptr->ssl), &s_savedSession)) != 0)
    {
        MBEDSOCKET_TRACE(" failed\n  ! mbedtls_ssl_conf_session returned %d\n\n", ret);
    }*/

    /** 3. Handshake **/
    MBEDSOCKET_TRACE("TlsSock: Performing the SSL/TLS handshake...\r\n");

    while ((ret = mbedtls_ssl_handshake(&(tlssock_ptr->ssl))) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            MBEDSOCKET_TRACE("TlsSock: failed --- mbedtls_ssl_handshake returned -0x%x", -ret);

            goto exit;
        }
    }
    mbedtls_net_set_nonblock(ctx);
    MBEDSOCKET_TRACE("TlsSock: Handshake ok\r\n");

    /** 4. Verify the server certificate **/
    MBEDSOCKET_TRACE("TlsSock:  Verifying peer X.509 certificate...");

    /* In real life, we probably want to bail out when ret != 0 */
    if ((flags = mbedtls_ssl_get_verify_result(&(tlssock_ptr->ssl))) != 0)
    {
        char vrfy_buf[512];

        MBEDSOCKET_TRACE("TlsSock: Verifying peer X.509 certificate failed\n");

        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);

        MBEDSOCKET_TRACE("TlsSock: %s\n", vrfy_buf);
    }
    else
    {
        MBEDSOCKET_TRACE("TlsSock: Verifying peer X.509 certificate ok\n");
    }

    /*if ((ret = mbedtls_ssl_get_session(&(tlssock_ptr->ssl), &s_savedSession)) != 0)
    {
        MBEDSOCKET_TRACE(" failed\n  ! mbedtls_ssl_get_session returned -0x%x\n\n", -ret);
        goto exit;
    }*/

    mbedtls_ssl_conf_read_timeout(&(tlssock_ptr->conf), MBEDTLS_READ_TIMEOUT);
    tlssock_ptr->state = TLSSOCK_STATE_CONNECTED;

exit:
    return ret;
}

static void Mbedsocket_TaskEntry(void *pData)
{
    uint32_t i;
    TLSSOCK_CONTEXT *tlssock_ptr;
    TLSSOCK_HANDLE handle;
    HANDLE notify_task;
    //uint32_t rx_buf_size;
    int32_t ret;
    IP_ADDR ip;
    uint16_t port;
    osiEvent_t event = {0, 0, 0, 0};
    int8_t ssid;

    osiThreadSleep(10);

    while (1)
    {
        MBEDSOCKET_TRACE("Mbedsocket_TaskEntry enter ");
        if (osiEventWait(s_tlssock_task_id, &event) != true)
        {
            MBEDSOCKET_TRACE("Mbedsocket_TaskEntry osiEventWait is false ");
        }
        else
        {
            //AT_CosEvent2CfwEvent(&event, &CfwEv);
            MBEDSOCKET_TRACE("Mbedsocket_TaskEntry nEventId:%ld", event.id);

            switch (event.id)
            {
            case EV_CFW_TLSSOCK_CREATE_REQ:
                MBEDSOCKET_TRACE("EV_CFW_TLSSOCK_CREATE_REQ");
                handle = (TLSSOCK_HANDLE)event.param2;
                notify_task = (HANDLE)event.param1;
                tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
                //rx_buf_size = event.nParam3;
                ret = tls_init((TLSSOCK_HANDLE)tlssock_ptr, notify_task);

                if (ret != 0)
                {
                    tlssock_ptr->is_used = FALSE;
                }
                ssid = Mbedsocket_GetSSIDByHandle(handle);
                MBEDSOCKET_TRACE("EV_CFW_TLSSOCK_CREATE_REQ,COS_ReleaseSemaphore,s_tlssock_sema[%d]:%p", ssid, s_tlssock_sema[ssid]);
                osiSemaphoreRelease(s_tlssock_sema[ssid]);
                break;

            case EV_CFW_TLSSOCK_FREE_REQ:
                MBEDSOCKET_TRACE("EV_CFW_TLSSOCK_FREE_REQ");
                handle = (TLSSOCK_HANDLE)event.param2;
                _free(handle);
                osiSemaphoreDelete(s_read_sema);
                s_read_sema = NULL;
                ssid = Mbedsocket_GetSSIDByHandle(handle);
                if (ssid == -1)
                {
                    MBEDSOCKET_TRACE("EV_CFW_TLSSOCK_FREE_REQ,can not match handle");
                    ssid = (int8_t)event.param3;
                    osiSemaphoreRelease(s_tlssock_sema[ssid]);
                    break;
                }
                osiTimerStop(autoSendTimer);
                if (autoSendTimer != NULL)
                {
                    MBEDSOCKET_TRACE("EV_CFW_TLSSOCK_FREE_REQ,delete autoSendTimer");
                    osiTimerDelete(autoSendTimer);
                    autoSendTimer = NULL;
                }

                if (g_tlssock_context[ssid] != NULL)
                {
                    free(g_tlssock_context[ssid]);
                    g_tlssock_context[ssid] = NULL;
                    MBEDSOCKET_TRACE("free g_tlssock_context[%d]", ssid);
                }
                for (i = 0; i < TLSSOCK_HANDLE_MAX_NUM; i++)
                {
                    if (NULL != g_tlssock_context[i])
                    {
                        break;
                    }
                }
                if (i == TLSSOCK_HANDLE_MAX_NUM)
                {
                    MBEDSOCKET_TRACE("EV_CFW_TLSSOCK_FREE_REQ,COS_StopCallbackTimer");
                }

                osiSemaphoreRelease(s_tlssock_sema[ssid]);
                osiThreadExit();
                break;

            case EV_CFW_TLSSOCK_CONNECT_REQ:
                MBEDSOCKET_TRACE("EV_CFW_TLSSOCK_CONNECT_REQ");
                handle = (TLSSOCK_HANDLE)event.param2;

                ip = event.param1;
                port = (uint16_t)event.param3;
                ret = ConnectStart(handle, ip, port);
                if (ret != 0)
                {
                    Mbedsocket_ConnectEvent(handle, ret);
                }
                break;

            case EV_CFW_TLSSOCK_INTERNAL_READ_REQ:

                ssid = (uint8_t)event.param1;
                MBEDSOCKET_TRACE("EV_CFW_TLSSOCK_INTERNAL_READ_REQ,ssid:%d", ssid);
                //COS_StopCallbackTimer(COS_GetCurrentTaskHandle(), MBED_InternalRxTimerTimeout, (void *)ssid);
                osiTimerStop(autoSendTimer);

                if (g_tlssock_context[ssid] != NULL && g_tlssock_context[ssid]->is_used && g_tlssock_context[ssid]->state == TLSSOCK_STATE_CONNECTED)
                {
                    Mbedsocket_InternalRead((TLSSOCK_HANDLE)(g_tlssock_context[ssid]));
                }

                for (i = 0; i < TLSSOCK_HANDLE_MAX_NUM; i++)
                {
                    if (NULL != g_tlssock_context[i])
                    {
                        break;
                    }
                }
                if (i != TLSSOCK_HANDLE_MAX_NUM)
                {
                    MBEDSOCKET_TRACE("EV_CFW_TLSSOCK_INTERNAL_READ_REQ start InternalRxTimer__");
                    osiTimerStart(autoSendTimer, g_tlsInterRxTime);
                }
                break;

            default:
                MBEDSOCKET_TRACE("Mbedsocket_TaskEntry sig=%ld isn't handled!!!", event.id);
                break;
            }
        }
    }
}

TLSSOCK_HANDLE mbedtlsSocket_Create(HANDLE notify_task, COS_CALLBACK_FUNC_T func, void *funcParam)
{
    TLSSOCK_CONTEXT *tlssock_ptr = NULL;
    uint32_t i;
    osiEvent_t ev = {0, 0, 0, 0};
    uint8_t result = FALSE;
    int8_t ssid;

    MBEDSOCKET_TRACE("mbedtlsSocket_Create");

    for (i = 0; i < TLSSOCK_HANDLE_MAX_NUM; i++)
    {
        if (g_tlssock_context[i] == NULL)
        {
            g_tlssock_context[i] = malloc(sizeof(TLSSOCK_CONTEXT));
            if (g_tlssock_context[i] == NULL)
            {
                return TLSSOCK_INVALID_HANDLE;
            }
            memset(g_tlssock_context[i], 0, sizeof(TLSSOCK_CONTEXT));
            tlssock_ptr = g_tlssock_context[i];
            tlssock_ptr->is_used = TRUE;
            tlssock_ptr->notify_callback = func;
            tlssock_ptr->funcParam = funcParam;
            result = TRUE;
            ssid = i;
            break;
        }
    }

    if (result != TRUE)
    {
        return TLSSOCK_INVALID_HANDLE;
    }

    ev.id = EV_CFW_TLSSOCK_CREATE_REQ;
    ev.param1 = (uint32_t)notify_task;
    ev.param2 = (TLSSOCK_HANDLE)tlssock_ptr;
    //ev.nParam3 = rx_buf_size;

    if (s_tlssock_sema[ssid] == NULL)
    {
        s_tlssock_sema[ssid] = osiSemaphoreCreate(1, 0);
        MBEDSOCKET_TRACE("TlsSock_Createa:s_tlssock_sema[%d]=%p", ssid, s_tlssock_sema[ssid]);
    }
    osiEventSend(s_tlssock_task_id, &ev);
    osiSemaphoreAcquire(s_tlssock_sema[ssid]);
    osiSemaphoreDelete(s_tlssock_sema[ssid]);
    MBEDSOCKET_TRACE("TlsSock_Createa: delete s_tlssock_sema");
    s_tlssock_sema[ssid] = NULL;
    g_tlsConnectStopFlag[ssid] = 0;
    g_sockFreeIsUsedFlag[ssid] = 0;

    return (TLSSOCK_HANDLE)tlssock_ptr;
}

void mbedtlsSocket_Free(TLSSOCK_HANDLE handle)
{
    osiEvent_t ev = {0, 0, 0, 0};
    int8_t ssid;

    MBEDSOCKET_TRACE("TlsSock_Free");
    ssid = Mbedsocket_GetSSIDByHandle(handle);
    if (ssid == -1)
    {
        MBEDSOCKET_TRACE("TlsSock_Free,can not match handle");
        return;
    }

    if (g_sockFreeIsUsedFlag[ssid])
    {
        MBEDSOCKET_TRACE("TlsSock_Free is used !!!!");
        return;
    }

    g_sockFreeIsUsedFlag[ssid] = 1;
    ev.id = EV_CFW_TLSSOCK_FREE_REQ;
    ev.param2 = handle;
    ev.param3 = ssid;

    if (s_tlssock_sema[ssid] == NULL)
    {
        s_tlssock_sema[ssid] = osiSemaphoreCreate(1, 0);
    }
    osiEventSend(s_tlssock_task_id, &ev);

    osiSemaphoreAcquire(s_tlssock_sema[ssid]);

    osiSemaphoreDelete(s_tlssock_sema[ssid]);
    s_tlssock_sema[ssid] = NULL;
    s_tlssock_task_id = NULL;
    g_sockFreeIsUsedFlag[ssid] = 0;
}

int32_t mbedtlsSocket_Cfg(TLSSOCK_HANDLE handle, TLSSOCK_CFG_TYPE_E type, uint32_t param)
{
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    int32_t ret = 0;
    TLSSOCK_CRT_T *pCrt;
    uint32_t min_ver, max_ver;

    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("TlsSock_Cfg,tlssock_ptr is null");
        return -1;
    }

    if (tlssock_ptr->state != TLSSOCK_STATE_CREATED)
    {
        return -1;
    }

    switch (type)
    {
    case TLSSOCK_CFG_TYPE_VERSION:
        min_ver = (param & 0x0000FFFF);
        max_ver = ((param & 0xFFFF0000) >> 16);

        if ((min_ver > MBEDTLS_SSL_MINOR_VERSION_3) || (max_ver > MBEDTLS_SSL_MINOR_VERSION_3))
        {
            ret = -1;
            break;
        }
        mbedtls_ssl_conf_min_version(&(tlssock_ptr->conf), MBEDTLS_SSL_MAJOR_VERSION_3, (int)min_ver);
        mbedtls_ssl_conf_max_version(&(tlssock_ptr->conf), MBEDTLS_SSL_MAJOR_VERSION_3, (int)max_ver);
        break;
    case TLSSOCK_CFG_TYPE_AUTHMODE:
        if (param > TLSSOCK_AUTH_MODE_SERVER_REQUIRED)
        {
            ret = -1;
            break;
        }
        if (param == TLSSOCK_AUTH_MODE_NONE)
        {
            mbedtls_ssl_conf_authmode(&(tlssock_ptr->conf), MBEDTLS_SSL_VERIFY_NONE);
        }
        else if (param == TLSSOCK_AUTH_MODE_SERVER_OPTIONAL)
        {
            mbedtls_ssl_conf_authmode(&(tlssock_ptr->conf), MBEDTLS_SSL_VERIFY_OPTIONAL);
        }
        else if (param == TLSSOCK_AUTH_MODE_SERVER_REQUIRED)
        {
            mbedtls_ssl_conf_authmode(&(tlssock_ptr->conf), MBEDTLS_SSL_VERIFY_REQUIRED);
        }
        break;

    case TLSSOCK_CFG_TYPE_CIPHER_SUITE:
    {
        uint32_t *supportedCiphersuites = NULL;

        supportedCiphersuites = (uint32_t *)param;
        if (supportedCiphersuites[0] == 0XFFFF)
        {
            mbedtls_ssl_conf_ciphersuites(&(tlssock_ptr->conf), mbedtls_ssl_list_ciphersuites());
        }
        else
        {
            mbedtls_ssl_conf_ciphersuites(&(tlssock_ptr->conf), (const int *)supportedCiphersuites);
        }
    }
    break;

    case TLSSOCK_CFG_TYPE_CA:
        pCrt = (TLSSOCK_CRT_T *)param;
        mbedtls_x509_crt_init(&(tlssock_ptr->cacert));
        ret = mbedtls_x509_crt_parse(&(tlssock_ptr->cacert),
                                     (const unsigned char *)pCrt->crt,
                                     pCrt->len);
        if (ret < 0)
        {
            MBEDSOCKET_TRACE("TlsSock Cfg: failed --- mbedtls_x509_crt_parse returned -0x%lx", -ret);
        }

        break;

    case TLSSOCK_CFG_TYPE_CLI_CERT:
        pCrt = (TLSSOCK_CRT_T *)param;
        mbedtls_x509_crt_init(&(tlssock_ptr->clicert));
        ret = mbedtls_x509_crt_parse(&(tlssock_ptr->clicert),
                                     (const unsigned char *)pCrt->crt,
                                     pCrt->len);
        if (ret < 0)
        {
            MBEDSOCKET_TRACE("TLSSOCK_CFG_TYPE_CLI_CERT: failed --- mbedtls_x509_crt_parse returned -0x%lx", -ret);
        }
        break;
    case TLSSOCK_CFG_TYPE_CLI_KEY:
        pCrt = (TLSSOCK_CRT_T *)param;
        mbedtls_pk_init(&(tlssock_ptr->pkey));
        ret = mbedtls_pk_parse_key(&(tlssock_ptr->pkey), (const unsigned char *)pCrt->crt,
                                   pCrt->len, NULL, 0);
        if (ret < 0)
        {
            MBEDSOCKET_TRACE("TLSSOCK_CFG_TYPE_CLI_KEY: failed --- mbedtls_pk_parse_key returned -0x%lx", -ret);
        }
        break;
    case TLSSOCK_CFG_TYPE_IGNORE_RTC_TIME:
    case TLSSOCK_CFG_TYPE_HTTPS:
    case TLSSOCK_CFG_TYPE_HTTPS_CTXI:
    case TLSSOCK_CFG_TYPE_SMTPS_TYPE:
    case TLSSOCK_CFG_TYPE_SMTPS_CTXI:
        break;

    default:
        ret = -1;
        break;
    }

    return ret;
}

int32_t mbedtlsSocket_Connect(TLSSOCK_HANDLE handle, IP_ADDR ip, uint16_t port)
{
    int32_t ret = 0;
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    osiEvent_t ev = {0, 0, 0, 0};

    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("TlsSock_Connect,tlssock_ptr is null");
        return -1;
    }

    if (tlssock_ptr->is_used == FALSE)
    {
        ret = -1;
        goto exit;
    }
    else if (TLSSOCK_STATE_CREATED != tlssock_ptr->state)
    {
        ret = -1;
        goto exit;
    }

    ev.id = EV_CFW_TLSSOCK_CONNECT_REQ;
    ev.param1 = ip;
    ev.param2 = handle;
    ev.param3 = port;
    osiEventSend(s_tlssock_task_id, &ev);

exit:
    return ret;
}

int32_t mbedtlsSocket_Send(TLSSOCK_HANDLE handle, uint8_t *buf, uint32_t len)
{
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    int32_t ret = -1;

    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("TlsSock_Send,tlssock_ptr is null");
        return -1;
    }

    if (TLSSOCK_STATE_CONNECTED == tlssock_ptr->state)
    {
        MBEDSOCKET_TRACE("CFW_TcpipSocketSetParam,socket=%ld,%p", tlssock_ptr->sock_id, tcpip_rsp);
        CFW_TcpipSocketSetParam(tlssock_ptr->sock_id, (osiCallback_t)tcpip_rsp, handle);
        while ((ret = mbedtls_ssl_write(&(tlssock_ptr->ssl), buf, len)) <= 0)
        {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                MBEDSOCKET_TRACE("TlsSock: failed\n  ! mbedtls_ssl_write returned %ld\n\n", ret);
                goto exit;
            }
        }
    }

exit:
    MBEDSOCKET_TRACE("TlsSock: Send ret =%ld", ret);
    return ret;
}

int32_t MbedtlsSocket_Recv(TLSSOCK_HANDLE handle, uint8_t *buf, uint32_t len)
{
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    int32_t ret = -1;
    struct data_buf_t *rx_decrypt_buf; //AT_RINGBUF_T
    MBEDSOCKET_TRACE("MbedtlsSocket_Recv,len=%ld", len);
    int8_t ssid = Mbedsocket_GetSSIDByHandle(handle);
    if (ssid < 0 || tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("MbedtlsSocket_Recv,tlssock_ptr is null or ssid=%d", ssid);
        return -1;
    }
    if (tlssock_ptr->server_fd.fd <= 0 || NULL == s_read_sema)
    {
        MBEDSOCKET_TRACE("MbedtlsSocket_Recv,tlssock_ptr->server_fd.fd=%d;s_read_sema=%p", tlssock_ptr->server_fd.fd, s_read_sema);
        return -1;
    }
    if (tlssock_ptr->state >= TLSSOCK_STATE_CONNECTED)
    {
        rx_decrypt_buf = &(tlssock_ptr->rx_buf);
        osiSemaphoreAcquire(s_read_sema);
        int length = data_buf_len(rx_decrypt_buf);
        MBEDSOCKET_TRACE("MbedtlsSocket_Recv,length=%d", length);
        u8_t *buffer;
        if (length >= len) //AT_RingBuf_DataSize
        {
            buffer = data_buf_get(rx_decrypt_buf, len);
            memcpy(buf, buffer, len);
            ret = len;
        }
        else
        {
            buffer = data_buf_get(rx_decrypt_buf, length);
            memcpy(buf, buffer, length);
            ret = length;
        }
        tlssock_ptr->rx_data_len = data_buf_len(rx_decrypt_buf); //AT_RingBuf_DataSize
        osiSemaphoreRelease(s_read_sema);
    }

    MBEDSOCKET_TRACE("MBEDtlsSock: Recv ret =%ld", ret);

    return ret;
}

int32_t MbedtlsSocket_SetReportFlag(TLSSOCK_HANDLE handle, uint8_t value)
{
    int32_t ret = 0;
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;

    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("MbedtlsSocket_SetReportFlag,tlssock_ptr is null");
        return -1;
    }

    if (tlssock_ptr->is_used == false)
    {
        ret = -1;
        goto exit;
    }

    tlssock_ptr->report_flag = value;

exit:
    return ret;
}

int32_t mbedtlsSocket_SetSimCid(TLSSOCK_HANDLE handle, uint8_t nSimid, uint8_t nCid)
{
    int32_t ret = 0;
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("mbedtlsSocket_SetSimCid,tlssock_ptr is null");
        return -1;
    }
    if (tlssock_ptr->is_used == false)
    {
        MBEDSOCKET_TRACE("mbedtlsSocket_SetSimCid, tlssock_ptr->is_used == FALSE");
        ret = -1;
        goto exit;
    }
    tlssock_ptr->nCid = nCid;
    tlssock_ptr->nSimid = nSimid;

exit:
    return ret;
}

int32_t MbedtlsSocket_ConnectStop(TLSSOCK_HANDLE handle)
{
    int32_t ret = 0;
    TLSSOCK_CONTEXT *tlssock_ptr = (TLSSOCK_CONTEXT *)handle;
    int8_t ssid;

    MBEDSOCKET_TRACE("MbedtlsSocket_ConnectStop");

    if (tlssock_ptr == NULL)
    {
        MBEDSOCKET_TRACE("MbedtlsSocket_ConnectStop,tlssock_ptr is null");
        return -1;
    }

    ssid = Mbedsocket_GetSSIDByHandle(handle);
    if (ssid == -1)
    {
        MBEDSOCKET_TRACE("MbedtlsSocket_ConnectStop,can not match handle");
        return -1;
    }

    if (tlssock_ptr->is_used == false)
    {
        ret = -1;
        goto exit;
    }

    g_tlsConnectStopFlag[ssid] = 1;
    if (CFW_SOCKET_CONNECTING == CFW_TcpipSocketGetStatus((&(tlssock_ptr->server_fd))->fd))
    {
        MBEDSOCKET_TRACE("MbedtlsSocket_ConnectStop, CFW_SOCKET_CONNECTING");
        if (tlssock_ptr->server_fd.fd != INVALID_SOCKET)
        {
            CFW_TcpipSocketClose((&(tlssock_ptr->server_fd))->fd);
            tlssock_ptr->server_fd.fd = INVALID_SOCKET;
        }
    }

exit:
    return ret;
}
