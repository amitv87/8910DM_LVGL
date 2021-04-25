/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: chttp_test.cpp
*
*	Revision:
*
*	17/02/15
*		- first revision
*
******************************************************************/
#include "mupnp/net/interface.h"
#include "mupnp/net/url.h"
#include "mupnp/util/log.h"
#include "mupnp/http/http.h"
#include "mupnp/util/string.h"
#include "http_api.h"
#include "http_download.h"
//#include "at_cmd_http.h"
#include "string.h"

bool Http_downLoad(Http_info *http_info1)
{

    CgHttpApi *cg_http_api;
    char *url_Char;

    url_Char = http_info1->url;
    cg_http_api = http_info1->cg_http_api;

    if (Http_init(cg_http_api, url_Char) != false)
    {

        bool down_flag = false;

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_GET);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, 0);

        down_flag = cg_http_api_downLoad(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https, http_info1);

        return down_flag;
    }
    else
    {
        mupnp_log_info("cg_http_init error ...\n");

        return false;
    }
}
bool cg_http_api_downLoad(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure, Http_info *http_info1)
{

    long contentLen;
    char *content;
    char *method, *uri, *version;
    mUpnpString *firstLine;
    mUpnpSocket *g_sock = NULL;
    bool readStatus = false;
    mupnp_log_debug_l4("Entering...\n");

    if (ipaddr == NULL)
    {
        mupnp_log_info("ipaddr is NULL");
        return false;
    }
    mupnp_http_response_clear(httpReq->httpRes);
    mupnp_string_setnvalue((httpReq->httpRes)->content, '\0', 0);

    mupnp_log_info("(HTTP) Posting:\n");
    mupnp_http_request_print(httpReq);

#if defined(MUPNP_USE_OPENSSL)
    if (isSecure == false)
        g_sock = mupnp_socket_stream_new();
    else
        g_sock = mupnp_socket_ssl_new();
#else
    g_sock = mupnp_socket_stream_new();
#endif

    if (g_sock == NULL)
    {
        mupnp_log_info("new socket fail");
        return false;
    }

    if (mupnp_socket_connect(g_sock, ipaddr, port) == false)
    {
        mupnp_socket_delete(g_sock);
        if (isSecure == true)
        {
            //          Http_WriteUart("failure, pelase check your network or certificate!\n", 51);
        }
        else
        {
            //         Http_WriteUart("failure, pelase check your network!\n", 36);
        }
        return false;
    }

    mupnp_socket_settimeout(g_sock, mupnp_http_request_gettimeout(httpReq));
    mupnp_http_request_sethost(httpReq, ipaddr, port);
    /* add headers here */
    mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)httpReq, CG_HTTP_USERAGENT, mupnp_http_request_getuseragent(httpReq));
    /* prepare to send firstline */
    method = mupnp_http_request_getmethod(httpReq);
    uri = mupnp_http_request_geturi(httpReq);
    version = mupnp_http_request_getversion(httpReq);

    if (method == NULL || uri == NULL || version == NULL)
    {
        //cg_socket_close(sock);
        mupnp_socket_delete(g_sock);
        g_sock = NULL;
        return false;
    }

    /**** send first line ****/
    firstLine = mupnp_string_new();
    mupnp_string_addvalue(firstLine, method);
    mupnp_string_addvalue(firstLine, CG_HTTP_SP);
    mupnp_string_addvalue(firstLine, uri);
    mupnp_string_addvalue(firstLine, CG_HTTP_SP);
    mupnp_string_addvalue(firstLine, version);
    mupnp_string_addvalue(firstLine, CG_HTTP_CRLF);
    mupnp_socket_write(g_sock, mupnp_string_getvalue(firstLine), mupnp_string_length(firstLine));
    mupnp_string_delete(firstLine);

    /**** send header and content ****/
    mupnp_http_packet_post((mUpnpHttpPacket *)httpReq, g_sock);

    readStatus = mupnp_http_response_read(httpReq->httpRes, g_sock, mupnp_http_request_isheadrequest(httpReq));
    if (readStatus == false)
    {
        mupnp_socket_delete(g_sock);
        mupnp_log_info("cg_http_api_downLoad: failed\n");
        return false;
    }
    content = mupnp_string_getvalue((mUpnpString *)(httpReq->httpRes)->content);

    contentLen = mupnp_http_packet_getcontentlength((mUpnpHttpPacket *)httpReq->httpRes);

    mupnp_log_info("+++the sizeof file will be downloaded is %d \n", contentLen);

    http_info1->contentLen = contentLen;
    if (http_info1->body_content != NULL && content != NULL)
    {
        memset(http_info1->body_content, 0, 255);
        if (strlen(content) < 255)
        {
            memcpy(http_info1->body_content, content, strlen(content));
        }
    }

    while (contentLen > 0)
    {
        uint32_t SENDLEN = 500;
        //UINT8 readLen;

        if (contentLen > SENDLEN)
        {

            //readLen = SENDLEN;
        }
        else
        {

            //readLen = contentLen;
        }
        //  Http_WriteUart((uint8_t *)content, readLen);
        contentLen -= SENDLEN;
        content += SENDLEN;
        osiThreadSleep(50);
    }

    mupnp_socket_close(g_sock);
    mupnp_socket_delete(g_sock);
    g_sock = NULL;
    //mupnp_http_request_delete(httpReq);
    mupnp_log_debug_l4("Leaving...\n");
    mupnp_log_info("cg_http_api_downLoad: Done\n");
    return true;
}
