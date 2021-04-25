/*
 * coap_mbeddtls.c -- Datagram Transport Layer Support for libcoap
 *
 * Copyright (C) 2016 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */
#include "coap_mbeddtls.h"
#include "coap_config.h"
#include "address.h"
#include "debug.h"
#include "mem.h"
#include "coap_keystore.h"
#include "utlist.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/timing.h"
#include "mbedtls/debug.h"

#include <netdb.h>

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else /* __GNUC__ */
#define UNUSED
#endif /* __GNUC__ */

#ifdef WITH_MBEDDTLS
/* Data item in the DTLS send queue. */
struct queue_t
{
    struct queue_t *next;
    coap_tid_t id;
    size_t data_length;
    unsigned char data[];
};

/* This structure takes a tinydtls peer object to represent a session
 * with a remote peer. Note that session_t objects in tinydtls are
 * less useful to pass around because in the end, you will always need
 * to find the corresponding dtls_peer_t object. dtls_session must be
 * * the first component in this structure. */
typedef struct coap_dtls_session_t
{
    /* socklen_t size;
  union {
    struct sockaddr     sa;
    struct sockaddr_storage st;
    struct sockaddr_in  sin;
#ifdef LWIP_IPV6_ON
    struct sockaddr_in6 sin6;
#endif
  } addr;
  uint8_t ifindex;
  */
    // mbedtls_ssl_session dtls_session;
    struct coap_dtls_session_t *next;
    struct queue_t *sendqueue;
} coap_dtls_session_t;

/* This structure encapsulates the dtls_context_t object from tinydtls
 * which must always be the first component. */
typedef struct coap_dtls_context_t
{
    //coap_dtls_session_t *sessions;
    struct coap_dtls_context_t *next;
    int sock_id;
    mbedtls_ssl_context ssl;
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    coap_address_t *coap_address;
} coap_dtls_context_t;

#define MAX_PACKET_SIZE 1024

int coap_dtls_is_supported(void)
{
    return 1;
}

void coap_dtls_set_log_level(int level)
{
    mbedtls_debug_set_threshold(level);
}
static int net_would_block(int fd)
{
    /*
     * Never return 'WOULD BLOCK' on a non-blocking socket
     */
    if ((fcntl(fd, F_GETFL, 0) & O_NONBLOCK) != O_NONBLOCK)
        return (0);

    switch (errno)
    {
#if defined EAGAIN
    case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:
#endif
        return (1);
    }
    return (0);
}

/*
 * Write at most 'len' characters
 */
int mbeddtls_net_send(void *connP, const unsigned char *buf, size_t len)
{
    coap_log(LOG_WARNING, "mbeddtls_net_send enter");
    struct coap_dtls_context_t *ctx = (coap_dtls_context_t *)connP;
    int fd = ctx->sock_id;
    if (fd < 0)
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    int nbSent = -1;
    size_t offset;

//char s[INET6_ADDRSTRLEN];add LWIP_IPV4_ON/LWIP_IPV6_ON
#if defined(LWIP_IPV4_ON) && defined(LWIP_IPV6_ON)
    char s[INET6_ADDRSTRLEN];
#else
#ifdef LWIP_IPV4_ON
    char s[INET_ADDRSTRLEN];
#else
    char s[INET6_ADDRSTRLEN];
#endif
#endif
    in_port_t port = 0;

    s[0] = 0;
#ifdef LWIP_IPV4_ON

    if (AF_INET == ctx->coap_address->addr.sin.sin_family)
    {
        struct sockaddr_in *saddr = (struct sockaddr_in *)&ctx->coap_address->addr.sin;
        inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
        port = saddr->sin_port;
    }
#endif
#ifdef LWIP_IPV6_ON
    if (AF_INET6 == ctx->coap_address->addr.sin6.sin6_family)
    {
        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&ctx->coap_address->addr.sin6;
        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
        port = saddr->sin6_port;
    }
#endif
    coap_log(LOG_WARNING, "s:%s,port:%d:Sending %d bytes\r\n", s, port, len);

    offset = 0;
    while (offset != len)
    {
        nbSent = sendto(fd, buf + offset, len - offset, 0, (struct sockaddr *)&(ctx->coap_address->addr.sin), ctx->coap_address->size);
        if (nbSent == -1)
            return -1;
        offset += nbSent;
    }
    coap_log(LOG_WARNING, "return len = %d", len);
    return nbSent;
}

/*
 * Read at most 'len' characters
 */


/*
 * Read at most 'len' characters
 */
int mbeddtls_net_recv(void *ctx, unsigned char *buffer, size_t len)
{
    coap_log(LOG_WARNING, "mbeddtls_net_recv enter");

    coap_dtls_context_t *dtls_context = (coap_dtls_context_t *)ctx;
    int fd = dtls_context->sock_id;
    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    int numBytes;
    coap_log(LOG_WARNING, "select read event happend,fd=%d",fd);

    /*
     * If an event happens on the socket
     */
    struct sockaddr_storage addr;
    socklen_t addrLen;
    addrLen = sizeof(addr);

    //We retrieve the data received
    numBytes = recvfrom(fd, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

    if (0 > numBytes)
    {
        coap_log(LOG_WARNING, "Error in recvfrom(): %d %s\r\n");
if (net_would_block(fd) != 0)
            return (MBEDTLS_ERR_SSL_WANT_READ);

#if (defined(_WIN32) || defined(_WIN32_WCE)) && !defined(EFIX64) && \
    !defined(EFI32)
        if (WSAGetLastError() == WSAECONNRESET)
            return (MBEDTLS_ERR_NET_CONN_RESET);
#else
        if (errno == EPIPE || errno == ECONNRESET)
            return (MBEDTLS_ERR_NET_CONN_RESET);

        if (errno == EINTR)
            return (MBEDTLS_ERR_SSL_WANT_READ);
#endif

        return (MBEDTLS_ERR_NET_RECV_FAILED);
    }
    else if (0 <= numBytes)
    {
#if defined(LWIP_IPV4_ON) && defined(LWIP_IPV6_ON)
        char s[INET6_ADDRSTRLEN];
#else
#ifdef LWIP_IPV4_ON
        char s[INET_ADDRSTRLEN];
#else
        char s[INET6_ADDRSTRLEN];
#endif
#endif
        in_port_t port = 0;
#ifdef LWIP_IPV4_ON
        if (AF_INET == addr.ss_family)
        {
            struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
            inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
            port = saddr->sin_port;
        }
#endif
#ifdef LWIP_IPV6_ON
        if (AF_INET6 == addr.ss_family)
        {
            struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&addr;
            inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
            port = saddr->sin6_port;
        }
#endif
        coap_log(LOG_WARNING, "%d bytes received from [%s]:%hu\r\n", numBytes, s, ntohs(port));
        memcpy(&dtls_context->coap_address->addr.sa, (struct sockaddr *)&addr, addrLen);
    }
    return numBytes;
}

int mbeddtls_net_handshake_recv(void *ctx, unsigned char *buffer, size_t len, uint32_t timeout)
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    coap_dtls_context_t *dtls_context = (coap_dtls_context_t *)ctx;
    int fd = dtls_context->sock_id;
    coap_log(LOG_WARNING, "mbeddtls_net_handshake_recv entering");
    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    coap_log(LOG_WARNING, "mbeddtls_net_handshake_recv timeout: %d:%d", tv.tv_sec, tv.tv_usec);
    ret = select(fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv);

    /* Zero fds ready means we timed out */
    if (ret == 0)
        return (MBEDTLS_ERR_SSL_TIMEOUT);

    if (ret < 0)
    {
#if (defined(_WIN32) || defined(_WIN32_WCE)) && !defined(EFIX64) && \
    !defined(EFI32)
        if (WSAGetLastError() == WSAEINTR)
            return (MBEDTLS_ERR_SSL_WANT_READ);
#else
        if (errno == EINTR)
            return (MBEDTLS_ERR_SSL_WANT_READ);
#endif

        return (MBEDTLS_ERR_NET_RECV_FAILED);
    }

    /* This call will not block */
    return (mbeddtls_net_recv(ctx, buffer, len));
}

static void coap_debug(void *ctx, int level,
                       const char *file, int line,
                       const char *param)
{
    ((void)level);
    coap_log(LOG_INFO, "%s", param);
}

struct coap_dtls_context_t *
    coap_dtls_new_context(void)
{

    coap_log(LOG_WARNING, "Enterring coap_dtls_new_context");
    coap_dtls_context_t *dtls_context = coap_malloc(sizeof(struct coap_dtls_context_t));
    if (NULL == dtls_context)
    {
        coap_log(LOG_WARNING, "coap_dtls_new_context,coapmallock failed");
        return NULL;
    }
    memset(dtls_context, 0, sizeof(struct coap_dtls_context_t));
    return dtls_context;
}

void coap_dtls_free_context(struct coap_dtls_context_t *dtls_context)
{
    struct coap_dtls_context_t *nextQ;

    nextQ = dtls_context;
    while (nextQ)
    {
        coap_log(LOG_WARNING, "coap_dtls_free_context,nextQ!=NULL");
        mbedtls_net_free(&nextQ->server_fd);

        mbedtls_ssl_config_free(&nextQ->conf);

        mbedtls_entropy_free(&nextQ->entropy);

        mbedtls_x509_crt_free(&nextQ->cacert);

        mbedtls_ssl_free(&nextQ->ssl);

        mbedtls_ctr_drbg_free(&nextQ->ctr_drbg);
         if (nextQ->coap_address)
        {
            coap_free(nextQ->coap_address);
            nextQ->coap_address = NULL;
        }
        struct coap_dtls_context_t *tmp = nextQ->next;
        coap_free(nextQ);
        nextQ = tmp;
    }
}

#ifdef LWIP_IPV6_ON
/* Convenience macro to copy IPv6 addresses without garbage. */
#define COAP_COPY_ADDRESS(DST, SRC)                                      \
    do                                                                   \
    {                                                                    \
        (DST)->size = (SRC)->size;                                       \
        if ((SRC)->addr.sa.sa_family == AF_INET6)                        \
        {                                                                \
            (DST)->addr.sin6.sin6_family = (SRC)->addr.sin6.sin6_family; \
            (DST)->addr.sin6.sin6_addr = (SRC)->addr.sin6.sin6_addr;     \
            (DST)->addr.sin6.sin6_port = (SRC)->addr.sin6.sin6_port;     \
        }                                                                \
        else                                                             \
        {                                                                \
            (DST)->addr.st = (SRC)->addr.st;                             \
        }                                                                \
    } while (0);
#else
#define COAP_COPY_ADDRESS(DST, SRC)      \
    do                                   \
    {                                    \
        (DST)->size = (SRC)->size;       \
        (DST)->addr.st = (SRC)->addr.st; \
    } while (0);
#endif
struct coap_dtls_context_t *dtlscontext_new_incoming(struct coap_dtls_context_t *ctx, int sock, coap_address_t *addr)
{
    struct coap_dtls_context_t *ctxP = coap_malloc(sizeof(struct coap_dtls_context_t));
    if (ctxP == NULL)
    {
        coap_log(LOG_WARNING, "coap_malloc failed");
        return NULL;
    }
    else
    {
        coap_log(LOG_WARNING, "Enterring dtlscontext_new_incoming,sock_id=%d", sock);
        ctxP->sock_id = sock;
        ctxP->coap_address = coap_malloc(sizeof(struct coap_address_t));
        if (ctxP->coap_address == NULL)
        {
            coap_free(ctxP);
            return NULL;
        }
        memset(ctxP->coap_address, 0, sizeof(struct coap_address_t));

        memcpy(ctxP->coap_address, addr, sizeof(struct coap_address_t));

        //ctxP->coap_address->size = sl;
        ctxP->next = ctx;
        return ctxP;
    }
}
struct coap_dtls_context_t *coap_dtls_connect_create(struct coap_dtls_context_t *ctx, int sock, coap_address_t *addr)
{
    coap_log(LOG_WARNING, "Enterring coap_dtls_connect_create");

    struct coap_dtls_context_t *ctxP = NULL;
    ctxP = dtlscontext_new_incoming(ctx, sock, addr);
    return ctxP;
}

int coap_dtls_cfg_context(coap_context_t *ctx, coap_address_t *addr, void *timer)
{
    coap_log(LOG_WARNING, "Enterring coap_dtls_cfg_context,port=%hu", addr->addr.sin.sin_port);
    int ret;
    const char *pers = "coapclient";

    coap_dtls_context_t *dtls_context = NULL;

    if ((dtls_context = coap_dtls_connect_create(ctx->dtls_context, ctx->endpoint->handle.fd, addr)) == NULL)
    {
        return -1;
    }

    ctx->dtls_context = dtls_context;

    mbedtls_net_init(&dtls_context->server_fd);

    mbedtls_ssl_init(&dtls_context->ssl);

    mbedtls_ssl_config_init(&dtls_context->conf);

    mbedtls_x509_crt_init(&dtls_context->cacert);

    mbedtls_ctr_drbg_init(&dtls_context->ctr_drbg);

    mbedtls_entropy_init(&dtls_context->entropy);
    //dtls_context->coap_address = malloc(sizeof(coap_address_t));
    //memset(dtls_context->coap_address,0,sizeof(coap_address_t));

    dtls_context->server_fd.fd = dtls_context->sock_id;
    if ((ret = mbedtls_ctr_drbg_seed(&dtls_context->ctr_drbg, mbedtls_entropy_func, &dtls_context->entropy,
                                     (const unsigned char *)pers, strlen(pers))) != 0)
    {
        coap_log(LOG_WARNING, "mbedtls_ctr_drbg_seed failed...,ret=%d\n", ret);
        return ret;
    }
    // coap_endpoint_t *ep;
    //LL_FOREACH(ctx->endpoint, ep) {
    //dtls_context->sock_id = ctx->endpoint->handle.fd;
    //}

    if ((ret = mbedtls_ssl_config_defaults(&dtls_context->conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        coap_log(LOG_WARNING, "mbedtls_ssl_config_defaults failed ret = %d\n", ret);
        return ret;
    }
    mbedtls_net_set_block(&dtls_context->server_fd);
    //mbedtls_timing_delay_context timer;
    mbedtls_ssl_set_timer_cb(&dtls_context->ssl, timer, mbedtls_timing_set_delay,
                             mbedtls_timing_get_delay);

    coap_keystore_item_t *psk;
    size_t id_len = MBEDTLS_PSK_MAX_LEN;
    unsigned char id[MBEDTLS_PSK_MAX_LEN];
    unsigned char psk_id[MBEDTLS_PSK_MAX_LEN];
    unsigned char psk_key[MBEDTLS_PSK_MAX_LEN];

    psk = coap_keystore_find_psk(ctx->keystore, id, id_len,
                                 NULL, 0, &ctx->endpoint->addr);
    if (!psk)
    {
        coap_log(LOG_WARNING, "no PSK identity for given realm\n");
        return -1;
    }
    size_t idlen = coap_psk_set_identity(psk, psk_id, MBEDTLS_PSK_MAX_LEN);
    coap_log(LOG_WARNING, "identity=%s", psk_id);
    psk = coap_keystore_find_psk(ctx->keystore, NULL, 0,
                                 psk_id, idlen, &ctx->endpoint->addr);
    if (!psk)
    {
        coap_log(LOG_WARNING, "no PSK KEY for given realm\n");
        return -1;
    }

    size_t keylen = coap_psk_set_key(psk, psk_key, MBEDTLS_PSK_MAX_LEN);
    if ((ret = mbedtls_ssl_conf_psk(&dtls_context->conf, psk_key, keylen,
                                    psk_id,
                                    idlen)) != 0)
    {
        coap_log(LOG_WARNING, " failed! mbedtls_ssl_conf_psk returned %d\n\n", -ret);
        return ret;
    }
    /* OPTIONAL is not optimal for security,
    * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode(&dtls_context->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&dtls_context->conf, &dtls_context->cacert, NULL);
    mbedtls_ssl_conf_rng(&dtls_context->conf, mbedtls_ctr_drbg_random, &dtls_context->ctr_drbg);
    mbedtls_ssl_conf_dbg(&dtls_context->conf, coap_debug, NULL);

    if ((ret = mbedtls_ssl_setup(&dtls_context->ssl, &dtls_context->conf)) != 0)
    {
        coap_log(LOG_WARNING, "mbedtls_ssl_setup failed ret = %d\n", ret);
        return ret;
    }
    //mbedtls_ssl_set_bio(&dtls_context->ssl, dtls_context, mbeddtls_net_send, mbeddtls_net_handshake_recv, NULL);
    mbedtls_ssl_set_bio(&dtls_context->ssl, dtls_context, mbeddtls_net_send, mbeddtls_net_recv, mbeddtls_net_handshake_recv);
    /* 4. Handshake*/

    while ((ret = mbedtls_ssl_handshake(&dtls_context->ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            coap_log(LOG_WARNING, " failed ! mbedtls_ssl_handshake returned %x\n\n", -ret);
            return ret;
        }
    }

    mbedtls_ssl_set_bio(&dtls_context->ssl, dtls_context, mbeddtls_net_send, mbeddtls_net_recv, NULL);
    coap_log(LOG_WARNING, "coap_dtls_cfg_context ok,return 0\n");
    return 0;
}

int coap_dtls_send(struct coap_context_t *coap_context,
                   //struct coap_dtls_session_t *session,
                   const coap_pdu_t *pdu)
{
    int res = -2;
/*    assert(coap_context && coap_context->dtls_context);*/
    coap_log(LOG_DEBUG, "call dtls_write\n");

    res = mbedtls_ssl_write(&coap_context->dtls_context->ssl, (unsigned char *)pdu->hdr, pdu->length);

    if (res <= 0)
    {
        coap_log(LOG_WARNING, "coap_dtls_send: cannot send PDU\n");
    }

    return res;
}

int coap_dtls_handle_message(struct coap_context_t *coap_context,
                             const coap_endpoint_t *local_interface,
                             //const coap_address_t *dst,
                             unsigned char *data, size_t data_len)
{
    int numBytes = 0;
    int res = -1;
    coap_dtls_context_t *ep, *tmp;
    coap_dtls_context_t *dtls_context = NULL;
    LL_FOREACH_SAFE(coap_context->dtls_context, ep, tmp)
    {
        if (ep->sock_id == local_interface->handle.fd)
        {
            dtls_context = ep;
        }
    }
    if (dtls_context == NULL)
        return -1;
    mbedtls_net_set_nonblock((mbedtls_net_context*)&dtls_context->server_fd);
    numBytes = mbedtls_ssl_read(&dtls_context->ssl, (unsigned char *)data, data_len);
    if (numBytes <= 0)
    {
        coap_log(LOG_DEBUG, "error dtls handling message %d\n", res);
        return numBytes;
    }
    //to do coap_address should be assign when read
    res = coap_handle_message(coap_context, local_interface, dtls_context->coap_address, data, numBytes);
    return res;
}

#else /* WITH_MBEDDTLS */

int coap_dtls_is_supported(void)
{
    return 0;
}

void coap_dtls_set_log_level(int level)
{
}

struct coap_dtls_context_t *
coap_dtls_new_context(VOID)
{
    return NULL;
}

int coap_dtls_cfg_context(coap_context_t *ctx, coap_address_t *addr, void *timer)
{
    return 0;
}

void coap_dtls_free_context(struct coap_dtls_context_t *dtls_context)
{
}

struct coap_dtls_session_t *
coap_dtls_get_session(struct coap_context_t *coap_context UNUSED,
                      const coap_endpoint_t *local_interface UNUSED,
                      const coap_address_t *dst UNUSED)
{
    return NULL;
}

int coap_dtls_send(struct coap_context_t *coap_context,
                   //struct coap_dtls_session_t *session,
                   const coap_pdu_t *pdu)
{
    return -1;
}

struct coap_dtls_session_t *
coap_dtls_new_session(struct coap_dtls_context_t *dtls_context,
                      const coap_endpoint_t *local_interface,
                      const coap_address_t *remote)
{
    return NULL;
}

struct coap_dtls_session_t;
void coap_dtls_free_session(struct coap_dtls_context_t *dtls_context,
                            struct coap_dtls_session_t *session) {}

int coap_dtls_handle_message(struct coap_context_t *coap_context,
                             const coap_endpoint_t *local_interface,
                             //const coap_address_t *dst,
                             unsigned char *data, size_t data_len)
{
    return -1;
}

#endif /* WITH_MBEDDTLS */

#undef UNUSED
