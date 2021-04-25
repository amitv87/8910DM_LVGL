/*******************************************************************************
 * Copyright (c) 2009, 2018 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs, Allan Stockdill-Mander - initial implementation
 *    Ian Craggs - fix for bug #409702
 *    Ian Craggs - allow compilation for OpenSSL < 1.0
 *    Ian Craggs - fix for bug #453883
 *    Ian Craggs - fix for bug #480363, issue 13
 *    Ian Craggs - SNI support
 *    Ian Craggs - fix for issues #155, #160
 *******************************************************************************/

/**
 * @file
 * \brief SSL  related functions
 *
 */
#include "PortingPaho.h"
#if defined(MBEDTLS)

#include "SocketBuffer.h"
#include "MQTTClient.h"
#include "MQTTProtocolOut.h"
#include "TLSSocket.h"
/*zyzyzy 20180929 #include "Log.h"*/
/*zyzyzy 20180929 #include "StackTrace.h"*/
#include "Socket.h"

/*zyzyzy 20180929 #include "Heap.h"*/

#include <string.h>

extern Sockets s;

int SSL_set_session(SSL *ssl, SSL_SESSION *session)
{
    return 1;
}

SSL_SESSION *SSL_get1_session(SSL *ssl)
{
    SSL_SESSION session;
    return &session;
}

static void paho_debug(void *ctx, int level,
                       const char *file, int line,
                       const char *param)
{
    ((void)level);
    COS_LOGI(0, "%s", param);
}

int SSLSocket_createContext(networkHandles *net, MQTTClient_SSLOptions *opts)
{
    int ret = -1;

    const char *pers = "paho_client";

    net->ssl = malloc(sizeof(mbedtls_ssl_context));
    if (net->ssl == NULL)
    {
        COS_LOGI(0, "net->ssl MALLOC FAILED:%d!\n", sizeof(mbedtls_ssl_context));
        goto err;
    }
    mbedtls_ssl_init(net->ssl);

    net->ctx->server_fd = malloc(sizeof(mbedtls_net_context));
    if (net->ctx->server_fd == NULL)
    {
        COS_LOGI(0, "net->server_fd MALLOC FAILED:%d!\n", sizeof(mbedtls_net_context));
        goto err;
    }
    mbedtls_net_init(net->ctx->server_fd);

    net->ctx->conf = malloc(sizeof(mbedtls_ssl_config));
    if (net->ctx->conf == NULL)
    {
        COS_LOGI(0, "net->conf MALLOC FAILED:%d!\n", sizeof(mbedtls_ssl_config));
        goto err;
    }
    mbedtls_ssl_config_init(net->ctx->conf);

    net->ctx->cacert = malloc(sizeof(mbedtls_x509_crt));
    if (net->ctx->cacert == NULL)
    {
        COS_LOGI(0, "net->cacert MALLOC FAILED:%d!\n", sizeof(mbedtls_x509_crt));
        goto err;
    }
    mbedtls_x509_crt_init(net->ctx->cacert);

    net->ctx->ctr_drbg = malloc(sizeof(mbedtls_ctr_drbg_context));
    if (net->ctx->ctr_drbg == NULL)
    {
        COS_LOGI(0, "net->ctr_drbg MALLOC FAILED:%d!\n", sizeof(mbedtls_ctr_drbg_context));
        goto err;
    }
    mbedtls_ctr_drbg_init(net->ctx->ctr_drbg);

    net->ctx->entropy = malloc(sizeof(mbedtls_entropy_context));
    if (net->ctx->entropy == NULL)
    {
        COS_LOGI(0, "net->entropy MALLOC FAILED:%d!\n", sizeof(mbedtls_entropy_context));
        goto err;
    }
    mbedtls_entropy_init(net->ctx->entropy);

    net->ctx->pkey = malloc(sizeof(mbedtls_pk_context));
    if (net->ctx->pkey == NULL)
    {
        COS_LOGI(0, "net->pkey MALLOC FAILED:%d!\n", sizeof(mbedtls_pk_context));
        goto err;
    }
    mbedtls_pk_init(net->ctx->pkey);

    net->ctx->clicert = malloc(sizeof(mbedtls_x509_crt));
    if (net->ctx->clicert == NULL)
    {
        COS_LOGI(0, "net->clicert MALLOC FAILED:%d!\n", sizeof(mbedtls_x509_crt));
        goto err;
    }
    mbedtls_pk_init(net->ctx->clicert);

    if ((ret = mbedtls_ctr_drbg_seed(net->ctx->ctr_drbg, mbedtls_entropy_func, net->ctx->entropy,
                                     (const unsigned char *)pers, strlen(pers))) != 0)
    {
        COS_LOGI(0, "mbedtls_ctr_drbg_seed failed...,ret=%d\n", ret);
        goto err;
    }
    ret = mbedtls_x509_crt_parse(net->ctx->cacert, (const unsigned char *)opts->trustStore,
                                 strlen(opts->trustStore));
    if (ret < 0)
    {

        COS_LOGI(0, "mbedtls_x509_crt_parse ca_pem failed...,ret=0x%x\n", -ret);
        goto err;
    }

    ret = mbedtls_x509_crt_parse(net->ctx->clicert, (const unsigned char *)opts->keyStore,
                                 strlen(opts->keyStore));
    if (ret < 0)
    {
        COS_LOGI(0, "mbedtls_x509_crt_parse cert_pem failed...,ret=0x%x\n", -ret);
        goto err;
    }

    ret = mbedtls_pk_parse_key(net->ctx->pkey, (const unsigned char *)opts->privateKey,
                               strlen(opts->privateKey), NULL, 0);

    if (ret < 0)
    {
        COS_LOGI(0, "mbedtls_pk_parse_key key_pem failed...,ret=0x%x\n", -ret);
        goto err;
    }

    mbedtls_ssl_conf_ca_chain(net->ctx->conf, net->ctx->cacert, NULL);
    ret = mbedtls_ssl_conf_own_cert(net->ctx->conf, net->ctx->clicert, net->ctx->pkey);
    if (ret < 0)
    {
        COS_LOGI(0, "mbedtls_ssl_conf_own_cert failed...,ret=0x%x\n", -ret);
        goto err;
    }

    if ((ret = mbedtls_ssl_setup(net->ssl, net->ctx->conf)) != 0)
    {
        COS_LOGI(0, "mbedtls_ssl_setup failed ret = %d\n", ret);
        goto err;
    }

    if (opts->enabledCipherSuites)
    {
        mbedtls_ssl_conf_ciphersuites(net->ctx->conf, opts->enabledCipherSuites);
    }
    mbedtls_ssl_conf_authmode(net->ctx->conf, MBEDTLS_SSL_VERIFY_NONE);
    if ((ret = mbedtls_ssl_config_defaults(net->ctx->conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        COS_LOGI(0, "mbedtls_ssl_config_defaults failed ret = %d\n", ret);
        goto err;
    }

    mbedtls_ssl_conf_rng(net->ctx->conf, mbedtls_ctr_drbg_random, net->ctx->ctr_drbg);
    mbedtls_debug_set_threshold(5);
    mbedtls_ssl_conf_dbg(net->ctx->conf, paho_debug, NULL);

    //mbedtls_timing_delay_context *timer = malloc(sizeof (mbedtls_timing_delay_context));
    mbedtls_ssl_conf_read_timeout(net->ctx->conf, 30000);
    //mbedtls_ssl_set_timer_cb(sock->ssl, timer, mbedtls_timing_set_delay,
    //  mbedtls_timing_get_delay );

    mbedtls_ssl_set_bio(net->ssl, net->ctx->server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

exit:
    return ret;
err:
    if (net->ssl)
    {
        if (net->ssl->p_timer)
        {
            free(net->ssl->p_timer);
            net->ssl->p_timer = NULL;
        }
        mbedtls_ssl_free(net->ssl);
        free(net->ssl);
        net->ssl = NULL;
    }
    if (net->ctx->server_fd)
    {
        mbedtls_net_free(net->ctx->server_fd);
        free(net->ctx->server_fd);
        net->ctx->server_fd = NULL;
    }

    if (net->ctx->entropy)
    {
        mbedtls_entropy_free(net->ctx->entropy);
        free(net->ctx->entropy);
        net->ctx->entropy = NULL;
    }
    if (net->ctx->conf)
    {
        mbedtls_ssl_config_free(net->ctx->conf);
        free(net->ctx->conf);
        net->ctx->conf = NULL;
    }
    if (net->ctx->cacert)
    {
        mbedtls_x509_crt_free(net->ctx->cacert);
        free(net->ctx->cacert);
        net->ctx->cacert = NULL;
    }

    if (net->ctx->ctr_drbg)
    {
        mbedtls_ctr_drbg_free(net->ctx->ctr_drbg);
        free(net->ctx->ctr_drbg);
        net->ctx->ctr_drbg = NULL;
    }
    if (net->ctx->clicert)
    {
        mbedtls_x509_crt_free(net->ctx->clicert);
        free(net->ctx->clicert);
        net->ctx->clicert = NULL;
    }
    if (net->ctx->pkey)
    {
        mbedtls_pk_free(net->ctx->pkey);
        free(net->ctx->pkey);
        net->ctx->pkey = NULL;
    }
    return ret;
}

int SSLSocket_setSocketForSSL(networkHandles *net, MQTTClient_SSLOptions *opts,
                              const char *hostname, size_t hostname_len)
{
    int rc = 1;

    if ((rc = SSLSocket_createContext(net, opts)) == 0)
    {
        char *hostname_plus_null;
        int i;

        if (opts->enableServerCertAuth)
            mbedtls_ssl_conf_authmode(net->ctx->conf, MBEDTLS_SSL_VERIFY_REQUIRED);

        net->ctx->server_fd->fd = net->socket;

        hostname_plus_null = malloc(hostname_len + 1u);
        MQTTStrncpy(hostname_plus_null, hostname, hostname_len + 1u);
        if ((rc = mbedtls_ssl_set_hostname(net->ssl, hostname_plus_null)) == 0)
        {
            rc = 1;
        }
        else
        {

            COS_LOGI(0, "mbedtls_ssl_set_hostname failed...,ret=-0x%x\n", -rc);
        }
        free(hostname_plus_null);
    }

    return rc;
}

/*
 * Return value: 1 - success, TCPSOCKET_INTERRUPTED - try again, anything else is failure
 */
int SSLSocket_connect(SSL *ssl, int sock, const char *hostname, int verify, int (*cb)(const char *str, size_t len, void *u), void *u)
{
    int rc = -1;

    while ((rc = mbedtls_ssl_handshake(ssl)) != 0)
    {
        if (rc != MBEDTLS_ERR_SSL_WANT_READ && rc != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            COS_LOGI(0, " failed ! mbedtls_ssl_handshake returned -%x\n\n", -rc);
            return rc;
        }
    }
    return 1;
}

/**
 *  Reads one byte from a socket
 *  @param socket the socket to read from
 *  @param c the character read, returned
 *  @return completion code
 */
int SSLSocket_getch(SSL *ssl, int socket, char *c)
{
    int rc = SOCKET_ERROR;

    if ((rc = SocketBuffer_getQueuedChar(socket, c)) != SOCKETBUFFER_INTERRUPTED)
        goto exit;

    if ((rc = mbedtls_ssl_read(ssl, c, (size_t)1)) < 0)
    {
        COS_LOGI(0, "mbedtls_ssl_read error = -0X%x", -rc);
    }
    else if (rc == 0)
        rc = SOCKET_ERROR; /* The return value from recv is 0 when the peer has performed an orderly shutdown. */
    else if (rc == 1)
    {
        SocketBuffer_queueChar(socket, *c);
        rc = TCPSOCKET_COMPLETE;
    }
exit:

    return rc;
}

/**
 *  Attempts to read a number of bytes from a socket, non-blocking. If a previous read did not
 *  finish, then retrieve that data.
 *  @param socket the socket to read from
 *  @param bytes the number of bytes to read
 *  @param actual_len the actual number of bytes read
 *  @return completion code
 */
char *SSLSocket_getdata(SSL *ssl, int socket, size_t bytes, size_t *actual_len)
{
    int rc;
    char *buf;

    if (bytes == 0)
    {
        buf = SocketBuffer_complete(socket);
        goto exit;
    }

    buf = SocketBuffer_getQueuedData(socket, bytes, actual_len);

    if ((rc = mbedtls_ssl_read(ssl, buf + (*actual_len), (int)(bytes - (*actual_len)))) < 0)
    {
        COS_LOGI(0, "mbedtls_ssl_read error = -0X%x", -rc);
        goto exit;
    }
    else if (rc == 0) /* rc 0 means the other end closed the socket */
    {
        buf = NULL;
        goto exit;
    }
    else
        *actual_len += rc;

    if (*actual_len == bytes)
    {
        SocketBuffer_complete(socket);
        /* if we read the whole packet, there might still be data waiting in the SSL buffer, which
		isn't picked up by select.  So here we should check for any data remaining in the SSL buffer, and
		if so, add this socket to a new "pending SSL reads" list.
		*/
        //if (SSL_pending(ssl) > 0) /* return no of bytes pending */
        //SSLSocket_addPendingRead(socket);
    }
    else /* we didn't read the whole packet */
    {
        SocketBuffer_interrupted(socket, *actual_len);
        Log(TRACE_MAX, -1, "SSL_read: %d bytes expected but %d bytes now received", bytes, *actual_len);
    }
exit:
    return buf;
}

void SSLSocket_destroyContext(networkHandles *net)
{
    if (net->ssl)
    {
        if (net->ssl->p_timer)
        {
            free(net->ssl->p_timer);
            net->ssl->p_timer = NULL;
        }
        mbedtls_ssl_free(net->ssl);
        free(net->ssl);
        net->ssl = NULL;
    }
    if (net->ctx->server_fd)
    {
        mbedtls_net_free(net->ctx->server_fd);
        free(net->ctx->server_fd);
        net->ctx->server_fd = NULL;
    }

    if (net->ctx->entropy)
    {
        mbedtls_entropy_free(net->ctx->entropy);
        free(net->ctx->entropy);
        net->ctx->entropy = NULL;
    }
    if (net->ctx->conf)
    {
        mbedtls_ssl_config_free(net->ctx->conf);
        free(net->ctx->conf);
        net->ctx->conf = NULL;
    }
    if (net->ctx->cacert)
    {
        mbedtls_x509_crt_free(net->ctx->cacert);
        free(net->ctx->cacert);
        net->ctx->cacert = NULL;
    }

    if (net->ctx->ctr_drbg)
    {
        mbedtls_ctr_drbg_free(net->ctx->ctr_drbg);
        free(net->ctx->ctr_drbg);
        net->ctx->ctr_drbg = NULL;
    }
    if (net->ctx->clicert)
    {
        mbedtls_x509_crt_free(net->ctx->clicert);
        free(net->ctx->clicert);
        net->ctx->clicert = NULL;
    }
    if (net->ctx->pkey)
    {
        mbedtls_pk_free(net->ctx->pkey);
        free(net->ctx->pkey);
        net->ctx->pkey = NULL;
    }
}

static List pending_reads = {NULL, NULL, NULL, 0, 0};

int SSLSocket_close(networkHandles *net)
{
    int rc = 1;

    /* clean up any pending reads for this socket */
    if (pending_reads.count > 0 && ListFindItem(&pending_reads, &net->socket, intcompare))
        ListRemoveItem(&pending_reads, &net->socket, intcompare);

    SSLSocket_destroyContext(net);

    return rc;
}

/* No SSL_writev() provided by OpenSSL. Boo. */
int SSLSocket_putdatas(SSL *ssl, int socket, char *buf0, size_t buf0len, int count, char **buffers, size_t *buflens, int *frees)
{
    int rc = 0;
    int i;
    char *ptr;
    iobuf iovec;

    iovec.iov_len = (ULONG)buf0len;
    for (i = 0; i < count; i++)
        iovec.iov_len += (ULONG)buflens[i];

    ptr = iovec.iov_base = (char *)malloc(iovec.iov_len);
    memcpy(ptr, buf0, buf0len);
    ptr += buf0len;
    for (i = 0; i < count; i++)
    {
        memcpy(ptr, buffers[i], buflens[i]);
        ptr += buflens[i];
    }

    if ((rc = mbedtls_ssl_write(ssl, iovec.iov_base, iovec.iov_len)) == iovec.iov_len)
        rc = TCPSOCKET_COMPLETE;
    else
    {
        rc = SOCKET_ERROR;
    }

    if (rc != TCPSOCKET_INTERRUPTED)
        free(iovec.iov_base);
    else
    {
        int i;
        free(buf0);
        for (i = 0; i < count; ++i)
        {
            if (frees[i])
            {
                free(buffers[i]);
                buffers[i] = NULL;
            }
        }
    }

    return rc;
}

void SSLSocket_addPendingRead(int sock)
{

    if (ListFindItem(&pending_reads, &sock, intcompare) == NULL) /* make sure we don't add the same socket twice */
    {
        int *psock = (int *)malloc(sizeof(sock));
        *psock = sock;
        ListAppend(&pending_reads, psock, sizeof(sock));
    }
    else
        COS_LOGI(0, "SSLSocket_addPendingRead: socket %d already in the list", sock);
}

int SSLSocket_getPendingRead(void)
{
    int sock = -1;

    if (pending_reads.count > 0)
    {
        sock = *(int *)(pending_reads.first->content);
        ListRemoveHead(&pending_reads);
    }
    return sock;
}

int SSLSocket_continueWrite(pending_writes *pw)
{
    int rc = 0;

    if ((rc = mbedtls_ssl_write(pw->ssl, pw->iovecs[0].iov_base, pw->iovecs[0].iov_len)) == pw->iovecs[0].iov_len)
    {
        /* topic and payload buffers are freed elsewhere, when all references to them have been removed */
        free(pw->iovecs[0].iov_base);
        COS_LOGI(0, "SSL continueWrite: partial write now complete for socket %d", pw->socket);
        rc = 1;
    }
    else
    {
        COS_LOGI(0, "SSL continueWrite: haven't finished writing the payload yet");
        //int sslerror = SSLSocket_error("SSL_write", pw->ssl, pw->socket, rc, NULL, NULL);
        //if (sslerror == SSL_ERROR_WANT_WRITE)
        rc = 0; /* indicate we haven't finished writing the payload yet */
    }

    return rc;
}
#endif
