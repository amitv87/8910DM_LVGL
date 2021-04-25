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
#include "mupnp/util/string.h"
#include "http_api.h"
#include "mupnp/http/http.h"
#include "mupnp/net/socket.h"
//#include "at_cmd_http.h"
#include "string.h"
//#include "dsm_cf.h"
#include "mbedtls/base64.h"
#include "http_json.h"
#include "sockets.h"
#include "at_cmd_http.h"
//#include "at_utils.h"
#include "vfs.h"
#include "osi_log.h"
bool gContentTypeFlag = false;
bool gApiKeyFlag = false;
char *vnetregdata = NULL;

packet_handler_t fota_data_handler = NULL;

int CFW_TcpipSocketSetsockopt(SOCKET nSocket, int level, int optname, const void *optval, int optlen);

void set_fota_data_handler(packet_handler_t handler)
{

    fota_data_handler = handler;
}

bool cg_http_api_post(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure);

char *http_dns_analysis(CgHttpApi *cg_http_api)
{
#if 0
    int ret;
    ip_addr_t nIpAddr;

	if(strlen(cg_http_api->host)<4){

		return NULL;
	}

    ret = CFW_GethostbynameEX(cg_http_api->host, &nIpAddr, cg_http_api->nCID, cg_http_api->nSIM, NULL, NULL);

    if (ret == RESOLV_QUERY_INVALID)
    {
        mupnp_log_info("dns error RESOLV_QUERY_INVALID");

        return NULL;
    }
    else if (ret == RESOLV_COMPLETE)
    {
        mupnp_log_info("ipString : %s", ipaddr_ntoa(&nIpAddr));
        return ipaddr_ntoa(&nIpAddr);
    }
    else if (ret = RESOLV_QUERY_QUEUED)
    {

        return NULL;
    }else
#endif
    return NULL;
}

bool Http_init(CgHttpApi *cg_http_api, char *url_char)
{
    char *host = NULL;
    char *uri_path = NULL;
    char *cg_host = NULL;
    int try
        = 0;

    mupnp_net_uri_set(cg_http_api->url, url_char);
    OSI_LOGXI(OSI_LOGPAR_S, 0x100075e3, "url_char: %s \n", url_char);

    cg_http_api->is_http = mupnp_net_url_ishttpprotocol(cg_http_api->url);
    cg_http_api->is_https = mupnp_net_url_ishttpsprotocol(cg_http_api->url);
    if ((cg_http_api->is_http == false) && (cg_http_api->is_https == false))
    {
        mupnp_log_error("not http/https protocol : %s\n", url_char);

        return false;
    }
    cg_host = mupnp_net_url_gethost(cg_http_api->url);
    if (cg_host == NULL)
    {

        return false;
    }
    else
    {

        if (strlen(cg_host) > 255)
        {

            return false;
        }

        else
        {

            strncpy(cg_http_api->host, cg_host, strlen(cg_host) + 1);
        }
    }

    cg_http_api->port = mupnp_net_url_getport(cg_http_api->url);
    while (host == NULL && try < 20)
    {
        host = http_dns_analysis(cg_http_api);
        try
            ++;
    }
    try
        = 0;
    OSI_LOGXI(OSI_LOGPAR_S, 0x100075e4, "cg_net_url_gethost %s\n", cg_http_api->host);
    OSI_LOGI(0x100075e5, "cg_net_url_getport %d\n", cg_http_api->port);
    if (host == NULL)
    {

        mupnp_log_info("Dns,fail,try url\r\n");
    }
    else
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0x100075e6, "gethost %s\n", host);

        if (strlen(host) > 255)
        {
            mupnp_log_info("Dns,fail\r\n");
            return false;
        }
        else
        {

            strncpy(cg_http_api->host, host, strlen(host) + 1);
        }
    }

    OSI_LOGXI(OSI_LOGPAR_S, 0x100075e7, "ip_url_host %s\n", cg_http_api->host);
    if (cg_http_api->port <= 0)
    {
        if (cg_http_api->is_http)
            cg_http_api->port = CG_HTTP_DEFAULT_PORT;
        else if (cg_http_api->is_https)
            cg_http_api->port = CG_HTTPS_DEFAULT_PORT;
    }

    uri_path = mupnp_net_url_getpath(cg_http_api->url);
    if (cg_http_api->uri_path == NULL || uri_path == NULL)
    {

        mupnp_log_info("uri_path pointer error \n");
        return false;
    }

    if (strlen(uri_path) > 255)
    {

        mupnp_log_info("uri_path too long \n");

        return false;
    }
    memset(cg_http_api->uri_path, 0, 256);
    strncpy(cg_http_api->uri_path, uri_path, strlen(uri_path) + 1);

    if (mupnp_net_url_getquery(cg_http_api->url) != NULL)
    {
        mupnp_strncat(cg_http_api->uri_path, "?", (256 - mupnp_strlen(cg_http_api->uri_path) - 1));
        mupnp_strncat(cg_http_api->uri_path, mupnp_net_url_getquery(cg_http_api->url), (256 - mupnp_strlen(cg_http_api->uri_path) - 1));
    }

    OSI_LOGXI(OSI_LOGPAR_S, 0x100075e8, "cg_net_url_getrequest %s\n", cg_http_api->uri_path);

    return true;
}

Http_info *RD_Http_Init(void)
{

    Http_info *RDHttpInit;

    RDHttpInit = (Http_info *)malloc(sizeof(Http_info));
    if (NULL == RDHttpInit)
    {
        mupnp_log_info("at_Http_info malloc error!! \n");
        return NULL;
    }

    RDHttpInit->cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
    if (NULL == RDHttpInit->cg_http_api)
    {
        free(RDHttpInit);
        RDHttpInit = NULL;
        mupnp_log_info("cg_http_api malloc error!! \n");
        return NULL;
    }

    RDHttpInit->body_content = (char *)malloc(256);
    if (NULL == RDHttpInit->body_content)
    {

        free(RDHttpInit->cg_http_api);
        RDHttpInit->cg_http_api = NULL;
        free(RDHttpInit);
        RDHttpInit = NULL;
        mupnp_log_info("body_content malloc error!! \n");
        return NULL;
    }

    RDHttpInit->content_type = (char *)malloc(256);
    if (NULL == RDHttpInit->content_type)
    {

        free(RDHttpInit->body_content);
        free(RDHttpInit->cg_http_api);
        RDHttpInit->body_content = NULL;
        RDHttpInit->cg_http_api = NULL;
        free(RDHttpInit);
        RDHttpInit = NULL;
        mupnp_log_info("content_type malloc error!! \n");
        return NULL;
    }

    RDHttpInit->content_name = (char *)malloc(256);
    if (NULL == RDHttpInit->content_name)
    {
        free(RDHttpInit->content_type);
        free(RDHttpInit->body_content);
        free(RDHttpInit->cg_http_api);
        RDHttpInit->content_type = NULL;
        RDHttpInit->body_content = NULL;
        RDHttpInit->cg_http_api = NULL;
        free(RDHttpInit);
        RDHttpInit = NULL;

        mupnp_log_info("content_name malloc error!! \n");
        return NULL;
    }

    RDHttpInit->username = (char *)malloc(256);
    if (NULL == RDHttpInit->username)
    {
        free(RDHttpInit->content_name);
        free(RDHttpInit->content_type);
        free(RDHttpInit->body_content);
        free(RDHttpInit->cg_http_api);
        RDHttpInit->content_name = NULL;
        RDHttpInit->content_type = NULL;
        RDHttpInit->body_content = NULL;
        RDHttpInit->cg_http_api = NULL;
        free(RDHttpInit);
        RDHttpInit = NULL;
        mupnp_log_info("username malloc error!! \n");
        return NULL;
    }

    RDHttpInit->password = (char *)malloc(256);
    if (NULL == RDHttpInit->password)
    {
        free(RDHttpInit->username);
        free(RDHttpInit->content_name);
        free(RDHttpInit->content_type);
        free(RDHttpInit->body_content);
        free(RDHttpInit->cg_http_api);
        RDHttpInit->username = NULL;
        RDHttpInit->content_name = NULL;
        RDHttpInit->content_type = NULL;
        RDHttpInit->body_content = NULL;
        RDHttpInit->cg_http_api = NULL;
        free(RDHttpInit);
        RDHttpInit = NULL;

        mupnp_log_info("password malloc error!! \n");
        return NULL;
    }

    RDHttpInit->url = (char *)malloc(256);
    if (NULL == RDHttpInit->url)
    {
        free(RDHttpInit->password);
        free(RDHttpInit->username);
        free(RDHttpInit->content_name);
        free(RDHttpInit->content_type);
        free(RDHttpInit->body_content);
        free(RDHttpInit->cg_http_api);
        RDHttpInit->password = NULL;
        RDHttpInit->username = NULL;
        RDHttpInit->content_name = NULL;
        RDHttpInit->content_type = NULL;
        RDHttpInit->body_content = NULL;
        RDHttpInit->cg_http_api = NULL;
        free(RDHttpInit);
        RDHttpInit = NULL;
        mupnp_log_info("url malloc error!! \n");
        return NULL;
    }

    (RDHttpInit->cg_http_api)->url = mupnp_net_uri_new();

    if ((RDHttpInit->cg_http_api)->url == NULL)
    {

        free(RDHttpInit->url);
        free(RDHttpInit->password);
        free(RDHttpInit->username);
        free(RDHttpInit->content_name);
        free(RDHttpInit->content_type);
        free(RDHttpInit->body_content);
        free(RDHttpInit->cg_http_api);
        RDHttpInit->url = NULL;
        RDHttpInit->password = NULL;
        RDHttpInit->username = NULL;
        RDHttpInit->content_name = NULL;
        RDHttpInit->content_type = NULL;
        RDHttpInit->body_content = NULL;
        RDHttpInit->cg_http_api = NULL;
        free(RDHttpInit);
        RDHttpInit = NULL;
        mupnp_log_info("mupnp_net_uri_new  error!! \n");
        return NULL;
    }

    (RDHttpInit->cg_http_api)->uri_path = (char *)malloc(256);
    if (NULL == (RDHttpInit->cg_http_api)->uri_path)
    {
        mupnp_net_uri_delete((RDHttpInit->cg_http_api)->url);
        free(RDHttpInit->url);
        free(RDHttpInit->password);
        free(RDHttpInit->username);
        free(RDHttpInit->content_name);
        free(RDHttpInit->content_type);
        free(RDHttpInit->body_content);
        free(RDHttpInit->cg_http_api);
        RDHttpInit->url = NULL;
        RDHttpInit->password = NULL;
        RDHttpInit->username = NULL;
        RDHttpInit->content_name = NULL;
        RDHttpInit->content_type = NULL;
        RDHttpInit->body_content = NULL;
        RDHttpInit->cg_http_api = NULL;
        free(RDHttpInit);
        RDHttpInit = NULL;

        mupnp_log_info("uri_path malloc error!! \n");
        return NULL;
    }
    (RDHttpInit->cg_http_api)->host = (char *)malloc(256);
    if (NULL == (RDHttpInit->cg_http_api)->host)
    {
        free((RDHttpInit->cg_http_api)->uri_path);
        (RDHttpInit->cg_http_api)->uri_path = NULL;
        mupnp_net_uri_delete((RDHttpInit->cg_http_api)->url);
        free(RDHttpInit->url);
        free(RDHttpInit->password);
        free(RDHttpInit->username);
        free(RDHttpInit->content_name);
        free(RDHttpInit->content_type);
        free(RDHttpInit->body_content);
        free(RDHttpInit->cg_http_api);
        RDHttpInit->url = NULL;
        RDHttpInit->password = NULL;
        RDHttpInit->username = NULL;
        RDHttpInit->content_name = NULL;
        RDHttpInit->content_type = NULL;
        RDHttpInit->body_content = NULL;
        RDHttpInit->cg_http_api = NULL;
        free(RDHttpInit);
        RDHttpInit = NULL;
        mupnp_log_info("host malloc error!! \n");
        return NULL;
    }

    (RDHttpInit->cg_http_api)->g_httpReq = mupnp_http_request_new();
    if ((RDHttpInit->cg_http_api)->g_httpReq == NULL)
    {

        free((RDHttpInit->cg_http_api)->host);
        free((RDHttpInit->cg_http_api)->uri_path);
        (RDHttpInit->cg_http_api)->host = NULL;
        (RDHttpInit->cg_http_api)->uri_path = NULL;
        mupnp_net_uri_delete((RDHttpInit->cg_http_api)->url);
        free(RDHttpInit->url);
        free(RDHttpInit->password);
        free(RDHttpInit->username);
        free(RDHttpInit->content_name);
        free(RDHttpInit->content_type);
        free(RDHttpInit->body_content);
        free(RDHttpInit->cg_http_api);
        RDHttpInit->url = NULL;
        RDHttpInit->password = NULL;
        RDHttpInit->username = NULL;
        RDHttpInit->content_name = NULL;
        RDHttpInit->content_type = NULL;
        RDHttpInit->body_content = NULL;
        RDHttpInit->cg_http_api = NULL;
        free(RDHttpInit);
        RDHttpInit = NULL;
        mupnp_log_info("mupnp_http_request_new error!! \n");
        return NULL;
    }
    return RDHttpInit;
}

bool RDHttpTerm(Http_info *Http_inf)
{

    mupnp_net_uri_clear((Http_inf->cg_http_api)->url);

    mupnp_net_uri_delete((Http_inf->cg_http_api)->url);

    mupnp_http_request_clear((Http_inf->cg_http_api)->g_httpReq);

    mupnp_http_request_delete((Http_inf->cg_http_api)->g_httpReq);

    if (Http_inf->body_content != NULL)
    {
        free(Http_inf->body_content);
        Http_inf->body_content = NULL;
    }
    if (Http_inf->content_name != NULL)
    {
        free(Http_inf->content_name);
        Http_inf->content_name = NULL;
    }
    if (Http_inf->content_type != NULL)
    {
        free(Http_inf->content_type);
        Http_inf->content_type = NULL;
    }
    if (Http_inf->url != NULL)
    {
        free(Http_inf->url);
        Http_inf->url = NULL;
    }
    if (Http_inf->username != NULL)
    {
        free(Http_inf->username);
        Http_inf->username = NULL;
    }

    if (Http_inf->password != NULL)
    {
        free(Http_inf->password);
        Http_inf->password = NULL;
    }
    if (Http_inf->cg_http_api->uri_path != NULL)
    {
        free(Http_inf->cg_http_api->uri_path);
        Http_inf->cg_http_api->uri_path = NULL;
    }

    if (Http_inf->cg_http_api->host != NULL)
    {
        free(Http_inf->cg_http_api->host);
        Http_inf->cg_http_api->host = NULL;
    }

    if (Http_inf->cg_http_api != NULL)
    {
        free(Http_inf->cg_http_api);
        Http_inf->cg_http_api = NULL;
    }
    if (Http_inf != NULL)
    {
        free(Http_inf);
        Http_inf = NULL;
    }

    return true;
}

nHttp_info *Init_Http(void)
{
    nHttp_info *InitHttp;

    CgHttpApi *cg_http_api;

    InitHttp = (nHttp_info *)malloc(sizeof(nHttp_info));
    if (NULL == InitHttp)
    {
        mupnp_log_info("InitHttp malloc error!! \n");
        return NULL;
    }

    cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
    if (NULL == cg_http_api)
    {
        free(InitHttp);
        InitHttp = NULL;
        mupnp_log_info("cg_http_api malloc error!! \n");
        return NULL;
    }

    cg_http_api->url = mupnp_net_uri_new();

    if (cg_http_api->url == NULL)
    {

        free(cg_http_api);
        free(InitHttp);
        cg_http_api = NULL;
        InitHttp = NULL;
        mupnp_log_info("mupnp_net_uri_new error!! \n");
        return NULL;
    }

    cg_http_api->g_httpReq = mupnp_http_request_new();

    if (cg_http_api->g_httpReq == NULL)
    {

        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api);
        free(InitHttp);
        cg_http_api = NULL;
        InitHttp = NULL;
        mupnp_log_info("mupnp_net_uri_new error!! \n");
        return NULL;
    }

    InitHttp->url = (char *)malloc(sizeof(char) * 256);
    if (NULL == InitHttp->url)
    {
        mupnp_http_request_delete(cg_http_api->g_httpReq);
        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api);
        free(InitHttp);
        cg_http_api = NULL;
        InitHttp = NULL;

        mupnp_log_info("url malloc error!! \n");
        return NULL;
    }
    memset(InitHttp->url, 0, sizeof(char) * 256);

    cg_http_api->host = (char *)malloc(sizeof(char) * 256);
    if (NULL == cg_http_api->host)
    {
        free(InitHttp->url);
        InitHttp->url = NULL;
        mupnp_http_request_delete(cg_http_api->g_httpReq);
        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api);
        free(InitHttp);
        cg_http_api = NULL;
        InitHttp = NULL;
        mupnp_log_info("host malloc error!! \n");
        return NULL;
    }
    memset(cg_http_api->host, 0, sizeof(char) * 256);

    cg_http_api->uri_path = (char *)malloc(sizeof(char) * 256);
    if (NULL == cg_http_api->uri_path)
    {
        free(cg_http_api->host);
        cg_http_api->host = NULL;
        free(InitHttp->url);
        InitHttp->url = NULL;
        mupnp_http_request_delete(cg_http_api->g_httpReq);
        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api);
        cg_http_api = NULL;
        free(InitHttp);
        InitHttp = NULL;
        mupnp_log_info("host malloc error!! \n");
        return NULL;
    }
    memset(cg_http_api->uri_path, 0, sizeof(char) * 256);

    InitHttp->CID = 0;
    InitHttp->pro_host = (char *)malloc(sizeof(char) * SIZE_HTTP_USR);
    if (NULL == InitHttp->pro_host)
    {
        free(cg_http_api->uri_path);
        cg_http_api->uri_path = NULL;
        free((cg_http_api)->host);
        cg_http_api->host = NULL;
        free(InitHttp->url);
        InitHttp->url = NULL;
        mupnp_http_request_delete(cg_http_api->g_httpReq);
        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api);
        cg_http_api = NULL;
        free(InitHttp);
        InitHttp = NULL;
        mupnp_log_info("pro_host malloc error!! \n");
        return NULL;
    }
    memset(InitHttp->pro_host, 0, sizeof(char) * 256);

    InitHttp->tag = (char *)malloc(SIZE_HTTP_USR);
    if (NULL == InitHttp->tag)
    {
        free(InitHttp->pro_host);
        InitHttp->pro_host = NULL;
        free(InitHttp->url);
        InitHttp->url = NULL;
        free(cg_http_api->uri_path);
        cg_http_api->uri_path = NULL;
        free((cg_http_api)->host);
        cg_http_api->host = NULL;
        mupnp_http_request_delete(cg_http_api->g_httpReq);
        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api);
        cg_http_api = NULL;
        free(InitHttp);
        InitHttp = NULL;
        mupnp_log_info("tag malloc error!! \n");
        return NULL;
    }
    memset(InitHttp->tag, 0, SIZE_HTTP_USR);

    InitHttp->value = (char *)malloc(SIZE_HTTP_USR);
    if (NULL == InitHttp->value)
    {
        free(InitHttp->tag);
        InitHttp->tag = NULL;
        free(InitHttp->pro_host);
        InitHttp->pro_host = NULL;
        free(InitHttp->url);
        InitHttp->url = NULL;
        mupnp_http_request_delete(cg_http_api->g_httpReq);
        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api->uri_path);
        cg_http_api->uri_path = NULL;
        free((cg_http_api)->host);
        cg_http_api->host = NULL;
        free(cg_http_api);
        cg_http_api = NULL;
        free(InitHttp);
        InitHttp = NULL;
        mupnp_log_info("value malloc error!! \n");
        return NULL;
    }
    memset(InitHttp->value, 0, SIZE_HTTP_USR);

    InitHttp->pro_port = 0;
    InitHttp->REDIR = 0;
    InitHttp->BREAK = 0;
    InitHttp->BREAKEND = 0;
    InitHttp->TIMEOUT = 0;
    InitHttp->cg_http_api = cg_http_api;
    InitHttp->user_data_used = false;
    //InitHttp->user_data = NULL;
    InitHttp->data_finish = 0;
    InitHttp->data_remain = 0;
    InitHttp->content_length = 0;
    InitHttp->CONTENT_TYPE = (char *)malloc(sizeof(char) * 256);
    if (NULL == InitHttp->CONTENT_TYPE)
    {
        free(InitHttp->value);
        InitHttp->value = NULL;
        free(InitHttp->tag);
        InitHttp->tag = NULL;
        free(InitHttp->pro_host);
        InitHttp->pro_host = NULL;
        free(InitHttp->url);
        InitHttp->url = NULL;
        free(cg_http_api->uri_path);
        cg_http_api->uri_path = NULL;
        free((cg_http_api)->host);
        cg_http_api->host = NULL;
        mupnp_http_request_delete(cg_http_api->g_httpReq);
        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api);
        cg_http_api = NULL;
        free(InitHttp);
        InitHttp = NULL;
        mupnp_log_info("CONTENT_TYPE malloc error!! \n");
        return NULL;
    }
    memset(InitHttp->CONTENT_TYPE, 0, sizeof(char) * 256);

    InitHttp->API_KEY = (char *)malloc(sizeof(char) * 256);
    if (NULL == InitHttp->API_KEY)
    {
        free(InitHttp->CONTENT_TYPE);
        InitHttp->CONTENT_TYPE = NULL;
        free(InitHttp->value);
        InitHttp->value = NULL;
        free(InitHttp->tag);
        InitHttp->tag = NULL;
        free(InitHttp->pro_host);
        InitHttp->pro_host = NULL;
        free(InitHttp->url);
        InitHttp->url = NULL;
        free(cg_http_api->uri_path);
        cg_http_api->uri_path = NULL;
        free((cg_http_api)->host);
        cg_http_api->host = NULL;
        mupnp_http_request_delete(cg_http_api->g_httpReq);
        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api);
        cg_http_api = NULL;
        free(InitHttp);
        InitHttp = NULL;
        mupnp_log_info("API_KEY malloc error!! \n");
        return NULL;
    }
    memset(InitHttp->API_KEY, 0, sizeof(char) * 256);

    InitHttp->USER_AGENT = (char *)malloc(sizeof(char) * 256);
    if (NULL == InitHttp->USER_AGENT)
    {
        free(InitHttp->API_KEY);
        InitHttp->API_KEY = NULL;
        free(InitHttp->CONTENT_TYPE);
        InitHttp->CONTENT_TYPE = NULL;
        free(InitHttp->value);
        InitHttp->value = NULL;
        free(InitHttp->tag);
        InitHttp->tag = NULL;
        free(InitHttp->pro_host);
        InitHttp->pro_host = NULL;
        free(InitHttp->url);
        InitHttp->url = NULL;
        free(cg_http_api->uri_path);
        cg_http_api->uri_path = NULL;
        free((cg_http_api)->host);
        cg_http_api->host = NULL;
        mupnp_http_request_delete(cg_http_api->g_httpReq);
        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api);
        cg_http_api = NULL;
        free(InitHttp);
        InitHttp = NULL;
        mupnp_log_info("USER_AGENT malloc error!! \n");
        return NULL;
    }
    strcpy(InitHttp->USER_AGENT, CG_HTTP_USERAGENT_DEVICE);
    InitHttp->user_data = (char *)malloc(sizeof(char) * 512);
    if (NULL == InitHttp->user_data)
    {
        free(InitHttp->USER_AGENT);
        InitHttp->USER_AGENT = NULL;
        free(InitHttp->API_KEY);
        InitHttp->API_KEY = NULL;
        free(InitHttp->CONTENT_TYPE);
        InitHttp->CONTENT_TYPE = NULL;
        free(InitHttp->value);
        InitHttp->value = NULL;
        free(InitHttp->tag);
        InitHttp->tag = NULL;
        free(InitHttp->pro_host);
        InitHttp->pro_host = NULL;
        free(InitHttp->url);
        InitHttp->url = NULL;
        free(cg_http_api->uri_path);
        cg_http_api->uri_path = NULL;
        free((cg_http_api)->host);
        cg_http_api->host = NULL;
        mupnp_http_request_delete(cg_http_api->g_httpReq);
        mupnp_net_uri_delete(cg_http_api->url);
        free(cg_http_api);
        cg_http_api = NULL;
        free(InitHttp);
        InitHttp = NULL;
        mupnp_log_info("USER_AGENT malloc error!! \n");
        return NULL;
    }
    return InitHttp;
}
bool Term_Http(nHttp_info *nHttp_inf)
{

    nHttp_inf->user_data_used = false;
    mupnp_net_uri_delete((nHttp_inf->cg_http_api)->url);

    mupnp_http_request_delete((nHttp_inf->cg_http_api)->g_httpReq);

    if (nHttp_inf->url != NULL)
    {
        free(nHttp_inf->url);
        nHttp_inf->url = NULL;
    }
    if (nHttp_inf->cg_http_api->host != NULL)
    {
        free(nHttp_inf->cg_http_api->host);
        nHttp_inf->cg_http_api->host = NULL;
    }
    if (nHttp_inf->cg_http_api->uri_path != NULL)
    {
        free(nHttp_inf->cg_http_api->uri_path);
        nHttp_inf->cg_http_api->uri_path = NULL;
    }

    if (nHttp_inf->cg_http_api != NULL)
    {
        free(nHttp_inf->cg_http_api);
        nHttp_inf->cg_http_api = NULL;
    }

    if (nHttp_inf->user_data != NULL)
    {
        free(nHttp_inf->user_data);
        nHttp_inf->user_data = NULL;
    }

    if (nHttp_inf->pro_host != NULL)
    {
        free(nHttp_inf->pro_host);
    }

    if (nHttp_inf->tag != NULL)
    {
        free(nHttp_inf->tag);
        nHttp_inf->tag = NULL;
    }

    if (nHttp_inf->value != NULL)
    {
        free(nHttp_inf->value);
        nHttp_inf->value = NULL;
    }
    if (nHttp_inf->CONTENT_TYPE != NULL)
    {
        free(nHttp_inf->CONTENT_TYPE);
        nHttp_inf->CONTENT_TYPE = NULL;
    }

    if (nHttp_inf->API_KEY != NULL)
    {
        free(nHttp_inf->API_KEY);
        nHttp_inf->API_KEY = NULL;
    }
    if (nHttp_inf->USER_AGENT != NULL)
    {
        free(nHttp_inf->USER_AGENT);
        nHttp_inf->USER_AGENT = NULL;
    }

    if (nHttp_inf != NULL)
    {
        free(nHttp_inf);
        nHttp_inf = NULL;
    }

    return true;
}
int Http_strcmp(char *tag)
{
    if (strcmp(tag, "CID") == 0)
    {
        return CID_1;
    }
    else if (strcmp(tag, "UA") == 0)
    {
        return USER_AGENT_1;
    }
    else if (strcmp(tag, "PROIP") == 0)
    {
        return PROIP_1;
    }
    else if (strcmp(tag, "PROPORT") == 0)
    {
        return PROPORT_1;
    }
    else if (strcmp(tag, "REDIR") == 0)
    {
        return REDIR_1;
    }
    else if (strcmp(tag, "BREAK") == 0)
    {
        return BREAK_1;
    }
    else if (strcmp(tag, "BREAKEND") == 0)
    {
        return BREAKEND_1;
    }
    else if (strcmp(tag, "TIMEOUT") == 0)
    {
        return TIMEOUT_1;
    }
    else if (strcmp(tag, "CONTENT") == 0)
    {
        return CONTENT_TYPE_1;
    }
    else if (strcmp(tag, "USERDATA") == 0)
    {
        return USERDATA_1;
    }
    else if (strcmp(tag, "URL") == 0)
    {
        return URL_1;
    }
    else if (strcmp(tag, "API_KEY") == 0)
    {
        return API_KEY_1;
    }
    else
    {
        return 14;
    }
}
bool Http_para(nHttp_info *nHttp_inf, char *tag, char *value)
{
    bool para = false;
    nHttp_inf->user_data_used = true;
    switch (Http_strcmp(tag))
    {
    case CID_1:
        nHttp_inf->CID = atoi(value);
        para = true;
        break;
    case URL_1:
        strcpy(nHttp_inf->url, value);
        para = true;
        break;
    case USER_AGENT_1:
        strcpy(nHttp_inf->USER_AGENT, value);
        para = true;
        break;
    case PROIP_1:
        strcpy(nHttp_inf->pro_host, value);
        para = true;
        break;
    case PROPORT_1:
        nHttp_inf->pro_port = atoi(value);
        para = true;
        break;
    case REDIR_1:
        nHttp_inf->REDIR = atoi(value);
        para = true;
        break;
    case BREAK_1:
        nHttp_inf->BREAK = atoi(value);
        para = true;
        break;
    case BREAKEND_1:
        nHttp_inf->BREAKEND = atoi(value);
        para = true;
        break;
    case TIMEOUT_1:
        nHttp_inf->TIMEOUT = atoi(value);
        para = true;
        break;
    case CONTENT_TYPE_1:
        strcpy(nHttp_inf->CONTENT_TYPE, value);
        para = true;
        gContentTypeFlag = true;
        break;
    case USERDATA_1:
        if (NULL != nHttp_inf->user_data)
        {
            free(nHttp_inf->user_data);
            nHttp_inf->user_data = NULL;
        }
        nHttp_inf->user_data = (uint8_t *)malloc(strlen(value) + 1);
        if (NULL == nHttp_inf->user_data)
        {
            mupnp_log_info("malloc error!! \n");
            return false;
        }

        memset(nHttp_inf->user_data, 0, strlen(value) + 1);
        strncpy(nHttp_inf->user_data, value, strlen(value) + 1);
        para = true;
        break;

    case API_KEY_1:
        strcpy(nHttp_inf->API_KEY, value);
        para = true;
        gApiKeyFlag = true;
        break;

    default:
        para = false;
        break;
    }

    return para;
}

#ifdef CONFIG_AT_HTTP_SUPPORT
bool Http_read(nHttp_info *nHttp_info1, long offset, long length)
{

    long isEnough = 0;
    char OutStr[20];
    long send = 512;

    isEnough = (nHttp_info1->content_length) - offset - length;

    mupnp_log_info("isEnough: %d\n", isEnough);

    mupnp_log_info("offset: %d\n", offset);

    mupnp_log_info("length: %d\n", length);

    if (isEnough >= 0)
    {

        sprintf(OutStr, "+HTTPREAD: %ld\r\n", length);
        Http_WriteUart(OutStr, strlen(OutStr));

        if (NULL == nHttp_info1->user_data)
        {
            mupnp_log_info("user_data is null !! \n");
            return false;
        }

        while (length > 0)
        {

            if (length > send)
            {

                length -= send;
                Http_WriteUart(((nHttp_info1->user_data) + offset), send);
                offset += send;
            }
            else
            {
                Http_WriteUart(((nHttp_info1->user_data) + offset), length);
                length = 0;
            }
        }
        return true;
    }
    else
    {

        length = (nHttp_info1->content_length) - offset;

        if (length <= 0)
        {

            Http_WriteUart("Error params, pelase check your params!", strlen("Error params, pelase check your params!"));

            return false;
        }
        sprintf(OutStr, "+HTTPREAD: %ld\r\n", length);
        Http_WriteUart(OutStr, strlen(OutStr));

        while (length > 0)
        {

            mupnp_log_info("nHttp_info->user_data: %s\n", (nHttp_info1->user_data) + offset);

            if (length > send)
            {

                length -= send;
                Http_WriteUart(((nHttp_info1->user_data) + offset), send);
                offset += send;
            }
            else
            {

                Http_WriteUart(((nHttp_info1->user_data) + offset), length);
                length = 0;
                mupnp_log_info("length: %d\n", length);
            }
        }
        return true;
    }
}
long Http_headn_contentlength(nHttp_info *http_info1)
{
    mupnp_log_info("Enter http_headn\n");

    CgHttpApi *cg_http_api;

    char *url_char;

    cg_http_api = http_info1->cg_http_api;

    url_char = http_info1->url;

    mUpnpHttpResponse *rep;

    if (Http_init(cg_http_api, url_char) != false)
    {

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, 0);

        rep = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        if (rep == NULL)
        {
            return -1;
        }

        return mupnp_http_packet_getcontentlength((mUpnpHttpPacket *)rep);
    }
    else
    {
        return 0;
    }
}

bool Http_getn_break(nHttp_info *http_info1)
{
    uint32_t readLen = 50 * 1000;
    uint8_t count, i;
    char tmpString[30] = {0};

    long contentLen = Http_headn_contentlength(http_info1);
    if (contentLen > readLen)
    {
        count = contentLen / readLen;
        //remain = contentLen % readLen;
        for (i = 0; i <= count; i++)
        {
            http_info1->BREAK = i * readLen;
            http_info1->BREAKEND = (i + 1) * readLen - 1;
            if (Http_getn(http_info1))
            {
                //sprintf(tmpString, "%d %d %d", i,count,(http_info1->BREAKEND-http_info1->BREAK));
                // Http_WriteUart(tmpString, strlen(tmpString));
                Http_WriteUart(http_info1->user_data, http_info1->content_length);
                http_info1->breakFlag = true;
            }
            else
            {
                http_info1->breakFlag = false;
                return false;
            }
        }
        return true;
    }
    else if (contentLen < 0)
    {
        http_info1->content_length = 0;
        http_info1->status_code = 400;
        sprintf(tmpString, "%d %d %ld", 0, http_info1->status_code, http_info1->content_length);
        Http_WriteUart(tmpString, strlen(tmpString));
        return false;
    }
    else
    {
        return Http_getn(http_info1);
    }
}

bool Http_getn(nHttp_info *http_info1)
{

    //http_t h_status;

    CgHttpApi *cg_http_api;

    char *url_char;

    char contentRange[32];

    char begin_downLoad[16];

    char end_downLoad[16];

    char tmpString[30] = {
        0x00,
    };

    mUpnpHttpResponse *rep;

    cg_http_api = http_info1->cg_http_api;

    url_char = http_info1->url;
    OSI_LOGI(0x100075e9, "+++++++enter Http_getn");
    if (Http_init(cg_http_api, url_char) != false)
    {
        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_GET);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        //mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, 0);
        if (gApiKeyFlag)
        {

            mupnp_http_request_setapikey(cg_http_api->g_httpReq, http_info1->API_KEY);

            gApiKeyFlag = false;
        }

        if (http_info1->pro_port != 0)
        {

            strncpy(cg_http_api->host, http_info1->pro_host, strlen(http_info1->pro_host) + 1);
            cg_http_api->port = http_info1->pro_port;
        }

        if ((http_info1->BREAK) >= 0 && (http_info1->BREAKEND) > 0)
        {

            itoa(http_info1->BREAK, begin_downLoad, 10);

            itoa(http_info1->BREAKEND, end_downLoad, 10);

            strcpy(contentRange, "bytes=");

            strcat(contentRange, begin_downLoad);

            strcat(contentRange, "-");

            strcat(contentRange, end_downLoad);

            mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)cg_http_api->g_httpReq, CG_HTTP_RANGE, contentRange);
        }

        mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)cg_http_api->g_httpReq, CG_HTTP_USERAGENT, http_info1->USER_AGENT);

        if (gContentTypeFlag)
        {

            mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)cg_http_api->g_httpReq, CG_HTTP_CONTENT_TYPE, http_info1->CONTENT_TYPE);

            gContentTypeFlag = false;
        }

        (cg_http_api->g_httpReq)->timeout = http_info1->TIMEOUT;

        rep = cg_nhttp_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https, http_info1);

        if (rep == NULL)
        {

            http_info1->content_length = 0;

            http_info1->status_code = 400;

            sprintf(tmpString, "%d %d %ld", 0, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return false;
        }

        if (NULL == http_info1->user_data)
        {
            http_info1->user_data = (uint8_t *)malloc(http_info1->content_length + 1);
        }
        else
        {
            free(http_info1->user_data);
            http_info1->user_data = NULL;
            http_info1->user_data = (uint8_t *)malloc(http_info1->content_length + 1);
        }
        if (NULL == http_info1->user_data)
        {
            OSI_LOGI(0x100075ea, "malloc error!! \n");
            Http_WriteUart("file is too large,no enough memory.\r\n", strlen("file is too large,no enough memory.\r\n"));

            http_info1->content_length = 0;

            http_info1->status_code = 400;

            sprintf(tmpString, "%d %d %ld", 0, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return false;
        }
        memset(http_info1->user_data, 0, http_info1->content_length + 1);

        if ((rep->content->value) == NULL)
        {

            sprintf(tmpString, "%d %d %ld", 0, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return true;
        }

        memcpy(http_info1->user_data, mupnp_http_response_getcontent(rep), (http_info1->content_length) + 1);

        http_info1->data_finish = http_info1->data_remain;

        http_info1->data_remain = 0;

        //mupnp_http_response_delete(rep);

        OSI_LOGI(0x100075eb, "cg_http_request_post done\n");

        sprintf(tmpString, "%d %d %ld", 0, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return true;
    }
    else
    {
        OSI_LOGI(0x100075ec, "cg_http_get error ...\n");
        http_info1->content_length = 0;

        http_info1->status_code = 400;

        sprintf(tmpString, "%d %d %ld", 0, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return false;
    }
}

bool Http_onenet_delete(nHttp_info *http_info1)
{

    OSI_LOGI(0x100075ed, "Enter http_deleten\n");

    CgHttpApi *cg_http_api;
    char *url_char;
    char tmpString[30] = {
        0x00,
    };
    //int statuscode = 0;

    url_char = http_info1->url;
    cg_http_api = http_info1->cg_http_api;

    if (Http_init(cg_http_api, url_char) != false)
    {

        mUpnpHttpResponse *rep;

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_DELETE);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        if (gApiKeyFlag)
        {

            mupnp_http_request_setapikey(cg_http_api->g_httpReq, http_info1->API_KEY);

            gApiKeyFlag = false;
        }

        if (http_info1->pro_port != 0)
        {

            strncpy(cg_http_api->host, http_info1->pro_host, strlen(http_info1->pro_host) + 1);
            cg_http_api->port = http_info1->pro_port;
        }

        mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)(cg_http_api->g_httpReq), CG_HTTP_USERAGENT, http_info1->USER_AGENT);

        (cg_http_api->g_httpReq)->timeout = http_info1->TIMEOUT;

        rep = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        if (rep == NULL)
        {
            http_info1->content_length = 0;

            http_info1->status_code = 400;

            goto DELETEEND;
        }

        http_info1->content_length = rep->content->valueSize;
        if (NULL == http_info1->user_data)
        {
            http_info1->user_data = (uint8_t *)malloc(http_info1->content_length + 1);
        }
        else
        {
            free(http_info1->user_data);
            http_info1->user_data = NULL;
            http_info1->user_data = (uint8_t *)malloc(http_info1->content_length + 1);
        }
        if (NULL == http_info1->user_data)
        {
            mupnp_log_info("malloc error!! \n");
            http_info1->content_length = 0;

            http_info1->status_code = 400;

            sprintf(tmpString, "%d %d %ld", 4, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));
            return false;
        }

        memset(http_info1->user_data, 0, http_info1->content_length + 1);

        if ((rep->content->value) == NULL)
        {

            sprintf(tmpString, "%d %d %ld", 4, rep->statusCode, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return true;
        }

        strncpy(http_info1->user_data, mupnp_http_response_getcontent(rep), (http_info1->content_length) + 1);

        http_info1->status_code = rep->statusCode;

    DELETEEND:

        sprintf(tmpString, "%d %d %ld", 4, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return true;
    }
    else
    {
        OSI_LOGI(0x100075ee, "cg_http_deleten error ...\n");
        http_info1->content_length = 0;

        http_info1->status_code = 400;

        sprintf(tmpString, "%d %d %ld", 4, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return false;
    }
}

bool Http_onenet_put(nHttp_info *http_info1)
{

    OSI_LOGI(0x100075ef, "Enter http_postn\n");

    CgHttpApi *cg_http_api;
    char *url_char;
    char *body_content;
    char *content_type;
    mUpnpHttpResponse *rep;

    char tmpString[30] = {
        0x00,
    };

    url_char = http_info1->url;
    body_content = http_info1->user_data;
    content_type = http_info1->CONTENT_TYPE;
    cg_http_api = http_info1->cg_http_api;

    if (Http_init(cg_http_api, url_char) != false)
    {

        cg_http_api->contentLen = mupnp_strlen(body_content);

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_PUT);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        if (gContentTypeFlag)
        {

            mupnp_http_request_setcontenttype(cg_http_api->g_httpReq, content_type);

            gContentTypeFlag = false;
        }

        if (gApiKeyFlag)
        {

            mupnp_http_request_setapikey(cg_http_api->g_httpReq, http_info1->API_KEY);

            gApiKeyFlag = false;
        }

        mupnp_http_request_setcontent(cg_http_api->g_httpReq, body_content);

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, cg_http_api->contentLen);

        if (http_info1->pro_port != 0)
        {

            strncpy(cg_http_api->host, http_info1->pro_host, strlen(http_info1->pro_host) + 1);
            cg_http_api->port = http_info1->pro_port;
        }

        mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)(cg_http_api->g_httpReq), CG_HTTP_USERAGENT, http_info1->USER_AGENT);

        (cg_http_api->g_httpReq)->timeout = http_info1->TIMEOUT;

        rep = cg_nhttp_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https, http_info1);

        if (rep == NULL)
        {
            http_info1->content_length = 0;

            http_info1->status_code = 400;

            sprintf(tmpString, "%d %d %ld", 5, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return false;
        }

        http_info1->data_finish = http_info1->data_remain;

        http_info1->data_remain = 0;

        OSI_LOGI(0x100075eb, "cg_http_request_post done\n");

        if (NULL == http_info1->user_data)
        {
            http_info1->user_data = (uint8_t *)malloc(http_info1->content_length + 1);
        }
        else
        {
            free(http_info1->user_data);
            http_info1->user_data = NULL;
            http_info1->user_data = (uint8_t *)malloc(http_info1->content_length + 1);
        }
        if (NULL == http_info1->user_data)
        {
            mupnp_log_info("malloc error!! \n");
            http_info1->content_length = 0;

            http_info1->status_code = 400;

            sprintf(tmpString, "%d %d %ld", 5, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));
            return false;
        }

        memset(http_info1->user_data, 0, http_info1->content_length + 1);

        if ((rep->content->value) == NULL)
        {

            sprintf(tmpString, "%d %d %ld", 5, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return true;
        }

        strncpy(http_info1->user_data, mupnp_http_response_getcontent(rep), (http_info1->content_length) + 1);

        sprintf(tmpString, "%d %d %ld", 5, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return true;
    }
    else
    {
        OSI_LOGI(0x100075f0, "cg_http_postn error ...\n");

        http_info1->content_length = 0;

        http_info1->status_code = 400;

        sprintf(tmpString, "%d %d %ld", 5, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return false;
    }
}

bool Http_headn(nHttp_info *http_info1)
{
    mupnp_log_info("Enter http_headn\n");

    CgHttpApi *cg_http_api;

    char *url_char;
    int http_action = 2;
    cg_http_api = http_info1->cg_http_api;

    url_char = http_info1->url;

    mUpnpHttpResponse *rep;

    char tmpString[30] = {
        0x00,
    };

    if (Http_init(cg_http_api, url_char) != false)
    {

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, 0);

        rep = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        if (rep == NULL)
        {

            http_info1->content_length = 0;

            http_info1->status_code = 400;

            sprintf(tmpString, "%d %d %ld", http_action, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return false;
        }

        http_info1->status_code = mupnp_http_response_getstatuscode(rep);

        mupnp_log_info("cg_http_request_post done\n");

        sprintf(tmpString, "%d %d %d", http_action, http_info1->status_code, 0);

        Http_WriteUart(tmpString, strlen(tmpString));

        return true;
    }
    else
    {
        mupnp_log_info("cg_http_headn error ...\n");
        http_info1->content_length = 0;

        http_info1->status_code = 400;

        sprintf(tmpString, "%d %d %ld", http_action, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return false;
    }
}

bool Http_postn(nHttp_info *http_info1)
{

    OSI_LOGI(0x100075ef, "Enter http_postn\n");

    //http_t h_status;

    CgHttpApi *cg_http_api;
    char *url_char;
    char *body_content;
    char *content_type;
    char *api_key;

    mUpnpHttpResponse *rep;

    char tmpString[30] = {
        0x00,
    };

    url_char = http_info1->url;
    body_content = http_info1->user_data;
    content_type = http_info1->CONTENT_TYPE;
    api_key = http_info1->API_KEY;
    cg_http_api = http_info1->cg_http_api;

    if (Http_init(cg_http_api, url_char) != false)
    {

        if (NULL == body_content)
        {
            OSI_LOGI(0x100075f1, "error:body_content is NULL!! \n");

            http_info1->content_length = 0;

            http_info1->status_code = 400;

            sprintf(tmpString, "%d %d %ld", 0, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return false;
        }
        OSI_LOGXI(OSI_LOGPAR_S, 0x100075f2, "body_content :%s \n", body_content);
        cg_http_api->contentLen = mupnp_strlen(body_content);

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_POST);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        if (gContentTypeFlag)
        {

            mupnp_http_request_setcontenttype(cg_http_api->g_httpReq, content_type);

            gContentTypeFlag = false;
        }

        mupnp_http_request_setcontent(cg_http_api->g_httpReq, body_content);

        if (gApiKeyFlag)
        {

            mupnp_http_request_setapikey(cg_http_api->g_httpReq, api_key);

            gApiKeyFlag = false;
        }

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, cg_http_api->contentLen);

        if (http_info1->pro_port != 0)
        {

            strncpy(cg_http_api->host, http_info1->pro_host, strlen(http_info1->pro_host) + 1);
            cg_http_api->port = http_info1->pro_port;
        }

        mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)(cg_http_api->g_httpReq), CG_HTTP_USERAGENT, http_info1->USER_AGENT);

        (cg_http_api->g_httpReq)->timeout = http_info1->TIMEOUT;

        rep = cg_nhttp_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https, http_info1);

        if (rep == NULL)
        {

            http_info1->content_length = 0;

            http_info1->status_code = 400;

            sprintf(tmpString, "%d %d %ld", 1, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return false;
        }
        http_info1->status_code = rep->statusCode;
        http_info1->content_length = mupnp_http_response_getcontentlength(rep);
        http_info1->data_finish = http_info1->data_remain;
        http_info1->data_remain = 0;

        mupnp_log_info("cg_http_request_post done\n");

        if (NULL == http_info1->user_data)
        {
            http_info1->user_data = (uint8_t *)malloc(http_info1->content_length + 1);
        }
        else
        {
            free(http_info1->user_data);
            http_info1->user_data = NULL;
            http_info1->user_data = (uint8_t *)malloc(http_info1->content_length + 1);
        }
        if (NULL == http_info1->user_data)
        {
            mupnp_log_info("malloc error!! \n");
            Http_WriteUart("no enough memory.\r\n", strlen("no enough memory.\r\n"));
            http_info1->content_length = 0;

            http_info1->status_code = 400;

            sprintf(tmpString, "%d %d %ld", 1, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return false;
        }

        memset(http_info1->user_data, 0, http_info1->content_length + 1);

        if (mupnp_http_response_getcontent(rep) == NULL)
        {

            sprintf(tmpString, "%d %d %ld", 1, http_info1->status_code, http_info1->content_length);

            Http_WriteUart(tmpString, strlen(tmpString));

            return true;
        }

        strncpy(http_info1->user_data, mupnp_http_response_getcontent(rep), (http_info1->content_length) + 1);

        sprintf(tmpString, "%d %d %ld", 1, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return true;
    }
    else
    {
        mupnp_log_info("cg_http_postn error ...\n");
        http_info1->content_length = 0;

        http_info1->status_code = 400;

        sprintf(tmpString, "%d %d %ld", 1, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return false;
    }
}

bool Http_postnreg(nHttp_info *http_info1)
{
    OSI_LOGI(0x100075ef, "Enter http_postn\n");
    //http_t h_status;
    CgHttpApi *cg_http_api;
    char *url_char;
    char *body_content;
    char *content_type;
    char *api_key;
    mUpnpHttpResponse *rep;
#if 0
    unsigned char *buffer;
    char *out = NULL;
    size_t out_len;
    size_t len;
    size_t buffer_len;
    out = httpgenRegJsonData();
    out_len = strlen(out);
    OSI_LOGXI(OSI_LOGPAR_IS, 0x100075f3, "guangzuincoming_data(%d): %s", out_len, out);
    OSI_LOGI(0x100075f4, "guangzuhttpgenRegJsonDataout:%d", out_len);
    mbedtls_base64_encode(NULL, 0, &buffer_len, (const unsigned char *)out, out_len);
    buffer = (unsigned char *)malloc(buffer_len + 1);
    OSI_LOGI(0x100075f5, "guangzuhttpgenRegJsonDatabufferlen:%d", buffer_len);
    if (buffer != NULL && mbedtls_base64_encode(buffer, (buffer_len + 1), &len, (const unsigned char *)out, out_len) != 0)
    {
    	OSI_LOGI(0x1000750b, "base64 error...");
	}
	OSI_LOGI(0x100075f6, "guangzuhttpgenRegJsonDatalen:%d", len);
	//free(out);
	body_content = (char *)buffer;
#endif
    url_char = http_info1->url;
    body_content = http_info1->user_data;
    content_type = http_info1->CONTENT_TYPE;
    api_key = http_info1->API_KEY;
    cg_http_api = http_info1->cg_http_api;
    if (Http_init(cg_http_api, url_char) != false)
    {
        if (NULL == body_content)
        {
            OSI_LOGI(0x100075f1, "error:body_content is NULL!! \n");
            return false;
        }
        OSI_LOGXI(OSI_LOGPAR_S, 0x100075f2, "body_content :%s \n", body_content);
        cg_http_api->contentLen = mupnp_strlen(body_content);

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_POST);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        mupnp_http_request_setcontenttype(cg_http_api->g_httpReq, content_type);

        mupnp_http_request_setcontent(cg_http_api->g_httpReq, body_content);

        if (gApiKeyFlag)
        {

            mupnp_http_request_setapikey(cg_http_api->g_httpReq, api_key);
            gApiKeyFlag = false;
        }

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, cg_http_api->contentLen);

        if (http_info1->pro_port != 0)
        {

            strncpy(cg_http_api->host, http_info1->pro_host, strlen(http_info1->pro_host) + 1);
            cg_http_api->port = http_info1->pro_port;
        }

        mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)(cg_http_api->g_httpReq), CG_HTTP_USERAGENT, http_info1->USER_AGENT);

        (cg_http_api->g_httpReq)->timeout = http_info1->TIMEOUT;
        rep = cg_nhttp_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https, http_info1);
        if (rep == NULL)
        {
            OSI_LOGI(0x100075f7, "httpcg_nhttp_api_response failed");
            return false;
        }
        http_info1->status_code = rep->statusCode;
        http_info1->content_length = mupnp_http_response_getcontentlength(rep);
        http_info1->data_finish = http_info1->data_remain;
        http_info1->data_remain = 0;
        mupnp_log_info("cg_http_request_post done\n");
        if (mupnp_http_response_getcontent(rep) == NULL)
        {
            OSI_LOGI(0x100075f8, "mupnp_http_response_getcontent NULL");
            return true;
        }
        if (NULL == vnetregdata)
        {
            vnetregdata = malloc(http_info1->content_length + 1);
        }
        else
        {
            free(vnetregdata);
            vnetregdata = NULL;
            vnetregdata = malloc(http_info1->content_length + 1);
        }
        if (NULL == vnetregdata)
        {
            OSI_LOGI(0x100075f7, "httpcg_nhttp_api_response failed");
            return false;
        }
        memset(vnetregdata, 0, http_info1->content_length + 1);
        strncpy(vnetregdata, mupnp_http_response_getcontent(rep), (http_info1->content_length) + 1);
        OSI_LOGI(0x100075f9, "httpreg_nhttp_api_response success");

        return true;
    }
    else
    {
        OSI_LOGI(0x100075fa, "http_init  failed");
        return false;
    }
}

bool Http_deleten(nHttp_info *http_info1)
{

    mupnp_log_info("Enter http_deleten\n");

    CgHttpApi *cg_http_api;
    char *url_char;
    char tmpString[30] = {
        0x00,
    };

    bool ret = true;
    url_char = http_info1->url;
    cg_http_api = http_info1->cg_http_api;

    if (Http_init(cg_http_api, url_char) != false)
    {
        mUpnpHttpResponse *resp;

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_DELETE);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        if (http_info1->pro_port != 0)
        {
            strncpy(cg_http_api->host, http_info1->pro_host, strlen(http_info1->pro_host) + 1);
            cg_http_api->port = http_info1->pro_port;
        }

        mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)(cg_http_api->g_httpReq), CG_HTTP_USERAGENT, http_info1->USER_AGENT);

        (cg_http_api->g_httpReq)->timeout = http_info1->TIMEOUT;

        resp = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        if (resp == NULL)
        {
            http_info1->status_code = 400;
            http_info1->content_length = 0;
            ret = false;
            goto DELETEEND;
        }
        http_info1->status_code = resp->statusCode;
        http_info1->content_length = mupnp_http_response_getcontentlength(resp);

        if (http_info1->content_length <= 0)
        {
            OSI_LOGI(0x100075fb, "cg_http_api_response content length is %ld", http_info1->content_length);
            http_info1->content_length = 0;
        }
        else
        {
            if (NULL == http_info1->user_data)
            {
                http_info1->user_data = (uint8_t *)malloc(http_info1->content_length + 1);
            }
            else
            {
                free(http_info1->user_data);
                http_info1->user_data = NULL;
                http_info1->user_data = (uint8_t *)malloc(http_info1->content_length + 1);
            }

            if (NULL == http_info1->user_data)
            {
                OSI_LOGI(0x100075ea, "malloc error!! \n");
                http_info1->content_length = 0;
                //http_info1->status_code = 400;
                goto DELETEEND;
            }
            memset(http_info1->user_data, 0, http_info1->content_length + 1);

            if (mupnp_http_response_getcontent(resp) == NULL)
            {
                http_info1->content_length = 0;
                goto DELETEEND;
            }

            strncpy(http_info1->user_data, mupnp_http_response_getcontent(resp), (http_info1->content_length) + 1);

            http_info1->data_finish = http_info1->data_remain;

            http_info1->data_remain = 0;
        }

    DELETEEND:
        OSI_LOGI(0x100075fc, "http delete done code %d length %ld\n", http_info1->status_code, http_info1->content_length);
        sprintf(tmpString, "%d %d %ld", 3, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return ret;
    }
    else
    {
        mupnp_log_info("cg_http_deleten error ...\n");
        http_info1->content_length = 0;

        http_info1->status_code = 400;

        sprintf(tmpString, "%d %d %ld", 3, http_info1->status_code, http_info1->content_length);

        Http_WriteUart(tmpString, strlen(tmpString));

        return false;
    }
}

mUpnpHttpResponse *cg_nhttp_api_response(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure, nHttp_info *http_info)
{

    char *method, *uri, *version;
    mUpnpString *firstLine;
    mUpnpSocket *g_sock = NULL;
    mUpnpHttpResponse *response = NULL;
    int statuscode = 0;
    bool is_read = false;
    //int iResult;

    OSI_LOGI(0x100075fd, "+++Entering.cg_nhttp_api_response..\n");

    if (ipaddr == NULL)
    {
        OSI_LOGI(0x100075fe, "ipaddr is NULL");
        return NULL;
    }
    mupnp_http_response_clear(httpReq->httpRes);
    mupnp_string_setnvalue((httpReq->httpRes)->content, '\0', 0);

    mupnp_log_info("+++(HTTP) Posting:\n");
    mupnp_http_request_print(httpReq);

#if defined(MUPNP_USE_OPENSSL)
    if (isSecure == false)
        g_sock = mupnp_socket_stream_new();
    else
        g_sock = mupnp_socket_ssl_new();
#else
    g_sock = mupnp_socket_stream_new();
#endif

    OSI_LOGI(0x100075ff, "new socket FINISH+++");

    if (g_sock == NULL)
    {
        OSI_LOGI(0x10007600, "new socket fail");
        return NULL;
    }

    OSI_LOGI(0x10007601, "new socket done+++");

    CFW_TcpipSocketSetsockopt(g_sock->id, SOL_SOCKET, SO_BINDTODEVICE, &(http_info->device), sizeof(struct ifreq));

    OSI_LOGXI(OSI_LOGPAR_S, 0x10007602, "ipaddr %s+++", ipaddr);
    OSI_LOGI(0x10007603, "port %d+++", port);

    if (mupnp_socket_connect(g_sock, ipaddr, port) == false)
    {
        mupnp_socket_delete(g_sock);
        if (isSecure == true)
        {
            Http_WriteUart("failure, pelase check your network or certificate!\n", 51);
        }
        else
        {
            //Http_WriteUart("failure, pelase check your network!\n", 36);
        }
        return NULL;
    }

    mupnp_socket_settimeout(g_sock, mupnp_http_request_gettimeout(httpReq));
    mupnp_http_request_sethost(httpReq, ipaddr, port);
    /* add headers here */

    /* prepare to send firstline */
    method = mupnp_http_request_getmethod(httpReq);
    uri = mupnp_http_request_geturi(httpReq);
    version = mupnp_http_request_getversion(httpReq);

    OSI_LOGXI(OSI_LOGPAR_SSS, 0x10007604, "method %s uri %s version %s+++", method, uri, version);

    if (method == NULL || uri == NULL || version == NULL)
    {
        //cg_socket_close(sock);
        mupnp_socket_delete(g_sock);
        OSI_LOGI(0x10007605, "method  uri  version error+++");
        g_sock = NULL;
        return NULL;
    }
Redir:

    /**** send first line ****/
    firstLine = mupnp_string_new();
    mupnp_string_addvalue(firstLine, method);
    mupnp_string_addvalue(firstLine, CG_HTTP_SP);
    mupnp_string_addvalue(firstLine, uri);
    mupnp_string_addvalue(firstLine, CG_HTTP_SP);
    mupnp_string_addvalue(firstLine, version);
    mupnp_string_addvalue(firstLine, CG_HTTP_CRLF);
    mupnp_socket_write(g_sock, mupnp_string_getvalue(firstLine), mupnp_string_length(firstLine));
    OSI_LOGXI(OSI_LOGPAR_S, 0x10007606, "++++++firstLine:%s", firstLine->value);
    mupnp_string_delete(firstLine);

    /**** send header and content ****/
    mupnp_http_packet_post((mUpnpHttpPacket *)httpReq, g_sock);

    OSI_LOGI(0x10007607, "RESPONSE READ BEGIN++:\n");

    is_read = mupnp_http_response_read(httpReq->httpRes, g_sock, mupnp_http_request_isheadrequest(httpReq));
    if (is_read == false)
    {
        OSI_LOGI(0x10007608, "cg_nhttp_api_response RESPONSE READ END++ failed:\n");
        mupnp_socket_delete(g_sock);
        g_sock = NULL;
        return NULL;
    }

    OSI_LOGI(0x10007609, "RESPONSE READ END++:\n");

    statuscode = mupnp_http_response_getstatuscode(httpReq->httpRes);

    http_info->status_code = statuscode;

    http_info->content_length = httpReq->httpRes->content->valueSize;

    http_info->data_remain = httpReq->httpRes->content->valueSize;

    if (http_info->REDIR == 1)
    {

        if (statuscode >= 300 && statuscode < 400)
        {
            uri = (char *)mupnp_http_headerlist_getvalue((httpReq->httpRes)->headerList, CG_HTTP_LOCATION);
            goto Redir;
        }
    }

    response = httpReq->httpRes;
    mupnp_socket_close(g_sock);
    mupnp_socket_delete(g_sock);
    g_sock = NULL;
    //cg_http_request_delete(httpReq);

    OSI_LOGI(0x1000760a, "cg_nhttp_api_response: Done\n");

    return response;
}

//set user data for post option
bool http_setUserdata(nHttp_info *at_nHttp_info, char *userData, long dataLen)
{
    OSI_LOGI(0x1000760b, "enter http_setUserdata.dataLen:%ld\n", dataLen);
    OSI_LOGXI(OSI_LOGPAR_S, 0x1000760c, "enter http_setUserdata.userData:%s\n", userData);

    //already malloc in the init_http()
    if (NULL == at_nHttp_info->user_data)
    {
        at_nHttp_info->user_data = (uint8_t *)malloc(dataLen + 1);
    }
    else
    {
        free(at_nHttp_info->user_data);
        at_nHttp_info->user_data = NULL;
        at_nHttp_info->user_data = (uint8_t *)malloc(dataLen + 1);
    }

    if (NULL == at_nHttp_info->user_data)
    {
        mupnp_log_info("malloc error!! \n");
        return false;
    }

    memset(at_nHttp_info->user_data, 0, dataLen + 1);
    at_nHttp_info->data_length = dataLen;
    strncpy(at_nHttp_info->user_data, (char *)userData, dataLen);
    at_nHttp_info->data_remain = dataLen;
    mupnp_log_info("enter http_setUserdata.at_nHttp_info->user_data:%s\n", at_nHttp_info->user_data);
    return true;
}

mUpnpHttpResponse *Http_authorization(Http_info *http_info1)
{

    char user_data[512] = "";

    CgHttpApi *cg_http_api = NULL;

    char *username = NULL;

    char *password = NULL;

    char *url_char = NULL;

    char *base_buffer = NULL;

    cg_http_api = http_info1->cg_http_api;

    username = http_info1->username;

    password = http_info1->password;

    url_char = http_info1->url;

    strcat(user_data, username);

    strcat(user_data, ":");

    strcat(user_data, password);

    char user_pwd[700] = "Basic ";

    base_buffer = Normal_Base64(user_data);

    strcat(user_pwd, base_buffer);

    free(base_buffer);

    mupnp_log_info("b64+++ %s", user_pwd);

    if (Http_init(cg_http_api, url_char) != false)
    {
        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_GET);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, 0);

        mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)cg_http_api->g_httpReq, CG_HTTP_AUTHOR, user_pwd);

        mUpnpHttpResponse *ret;

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        mUpnpHttpResponse *res_copy = NULL;

        if (ret != NULL)
        {
            res_copy = mupnp_http_response_new();
            if (res_copy != NULL)
                mupnp_http_response_copy(res_copy, ret);
        }

        mupnp_log_info("cg_http_request_post done\n");

        return res_copy;
    }
    else
    {
        mupnp_log_info("cg_http_authorization error ...\n");

        return NULL;
    }
}

mUpnpHttpResponse *Http_head(Http_info *http_info1)
{
    CgHttpApi *cg_http_api;
    char *url_char;
    cg_http_api = http_info1->cg_http_api;
    url_char = http_info1->url;

    if (Http_init(cg_http_api, url_char) != false)
    {

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, 0);

        mUpnpHttpResponse *ret;

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        mUpnpHttpResponse *res_copy = NULL;

        if (ret != NULL)
        {
            res_copy = mupnp_http_response_new();
            if (res_copy != NULL)
                mupnp_http_response_copy(res_copy, ret);
        }

        mupnp_log_info("cg_http_request_post done\n");

        return res_copy;
    }
    else
    {
        mupnp_log_info("cg_http_head error ...\n");

        return NULL;
    }
}

mUpnpHttpResponse *Http_post(Http_info *http_info1)
{

    CgHttpApi *cg_http_api;
    char *url_char;
    char *body_content;
    char *content_type;

    url_char = http_info1->url;
    body_content = http_info1->body_content;
    content_type = http_info1->content_type;
    cg_http_api = http_info1->cg_http_api;

    if (Http_init(cg_http_api, url_char) != false)
    {

        cg_http_api->contentLen = mupnp_strlen(body_content);

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_POST);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        mupnp_http_request_setcontenttype(cg_http_api->g_httpReq, content_type);

        mupnp_http_request_setcontent(cg_http_api->g_httpReq, body_content);

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, cg_http_api->contentLen);

        mUpnpHttpResponse *ret;

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        mUpnpHttpResponse *res_copy = NULL;

        if (ret != NULL)
        {
            res_copy = mupnp_http_response_new();
            if (res_copy != NULL)
                mupnp_http_response_copy(res_copy, ret);
        }

        mupnp_log_info("cg_http_request_post done\n");

        return res_copy;
    }
    else
    {
        mupnp_log_info("cg_http_post error ...\n");

        return NULL;
    }
}
bool Http_put(Http_info *http_info1)
{

    CgHttpApi *cg_http_api;
    char *content_name;
    char *body_content;
    char *content_type;
    char *url_char;
    char uri_path[512] = {0};
    int statuscode = 0;

    cg_http_api = http_info1->cg_http_api;
    content_name = http_info1->content_name;
    content_type = http_info1->content_type;
    body_content = http_info1->body_content;
    url_char = http_info1->url;

    if (Http_init(cg_http_api, url_char) != false)
    {

        mUpnpHttpResponse *ret;

        mupnp_log_info("+++content_name %s", content_name);

        mupnp_log_info("+++cg_http_api->uri_path %s", cg_http_api->uri_path);

        strcat(uri_path, cg_http_api->uri_path);

        strcat(uri_path, content_name);

        mupnp_log_info("+++cg_http_api->uri_path %s", uri_path);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, uri_path);

        mupnp_log_info("after set uri %s", mupnp_http_request_geturi(cg_http_api->g_httpReq));

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);

        mupnp_log_info("after method set uri %s", mupnp_http_request_geturi(cg_http_api->g_httpReq));

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        if (ret == NULL)
        {

            goto PUTEND;
        }
        statuscode = ret->statusCode;

        if (statuscode == 200)
        {

            Http_WriteUart("failure,file already exist,please delete first\n", 47);

            goto PUTEND;
        }

        cg_http_api->contentLen = mupnp_strlen(body_content);

        //mupnp_http_request_clear(cg_http_api->g_httpReq);

        //mupnp_http_request_seturi(cg_http_api->g_httpReq,cg_http_api->uri_path);

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_PUT);

        mupnp_http_request_setcontenttype(cg_http_api->g_httpReq, content_type);

        mupnp_http_request_setcontent(cg_http_api->g_httpReq, body_content);

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, cg_http_api->contentLen);

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        if (ret == NULL)
        {

            goto PUTEND;
        }

        statuscode = ret->statusCode;

        if (statuscode == 201)
        {

            Http_WriteUart("success,file created\n", 21);
        }
        else
        {

            Http_WriteUart("failure,file created fail\n", 26);
        }

    PUTEND:
        mupnp_log_info("cg_http_request_post done\n");

        return true;
    }
    else
    {

        mupnp_log_info("init error ...\n");

        return false;
    }
}
mUpnpHttpResponse *Http_options(Http_info *http_info1)
{

    CgHttpApi *cg_http_api;
    char *url_char;

    cg_http_api = http_info1->cg_http_api;
    url_char = http_info1->url;

    if (Http_init(cg_http_api, url_char) != false)
    {

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_OPTIONS);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        mUpnpHttpResponse *ret;

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        mUpnpHttpResponse *res_copy = NULL;

        if (ret != NULL)
        {
            res_copy = mupnp_http_response_new();
            if (res_copy != NULL)
                mupnp_http_response_copy(res_copy, ret);
        }

        mupnp_log_info("cg_http_request_post done\n");

        return res_copy;
    }
    else
    {
        mupnp_log_info("init error  ...\n");

        return NULL;
    }
}

mUpnpHttpResponse *Http_trace(Http_info *http_info1)
{

    CgHttpApi *cg_http_api;
    char *url_char;

    cg_http_api = http_info1->cg_http_api;
    url_char = http_info1->url;

    if (Http_init(cg_http_api, url_char) != false)
    {

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_TRACE);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        mUpnpHttpResponse *ret;

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        mUpnpHttpResponse *res_copy = NULL;

        if (ret != NULL)
        {
            res_copy = mupnp_http_response_new();
            if (res_copy != NULL)
                mupnp_http_response_copy(res_copy, ret);
        }

        mupnp_log_info("cg_http_request_post done\n");

        return res_copy;
    }
    else
    {
        mupnp_log_info("init error ...\n");

        return NULL;
    }
}
bool Http_delete(Http_info *http_info1)
{

    CgHttpApi *cg_http_api;
    char *url_char;
    char *content_name;
    int statuscode = 0;
    char uri_path[512] = {0};

    url_char = http_info1->url;
    content_name = http_info1->content_name;
    cg_http_api = http_info1->cg_http_api;

    if (Http_init(cg_http_api, url_char) != false)
    {

        mUpnpHttpResponse *ret;

        strcat(uri_path, cg_http_api->uri_path);

        strcat(uri_path, content_name);

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, uri_path);

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        if (ret == NULL)
        {

            goto DELETEEND;
        }

        statuscode = ret->statusCode;

        if (statuscode == 404)
        {

            Http_WriteUart("failure,file not exist,please create first\n", 43);

            goto DELETEEND;
        }

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_DELETE);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, uri_path);

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);
        if (ret == NULL)
        {
            mupnp_log_info("cg_http_api_response delete return null ...\n");
        }

        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, uri_path);

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        if (ret == NULL)
        {

            goto DELETEEND;
        }

        statuscode = ret->statusCode;

        if (statuscode == 404)
        {

            Http_WriteUart("success,file deleted\n", 21);
        }
        else
        {

            Http_WriteUart("failure,file deleted fail\n", 26);
        }

    DELETEEND:
        mupnp_log_info("cg_http_request_post done\n");

        return true;
    }
    else
    {
        mupnp_log_info("init error ...\n");

        return false;
    }
}
bool cg_http_api_post(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure)
{

    char *method, *uri, *version;
    mUpnpString *firstLine;
    mUpnpSocket *g_sock = NULL;
    bool readStatus = false;
    mupnp_log_debug_l4("Entering...\n");

    if (ipaddr == NULL)
    {
        mupnp_log_info("ipaddr is null");
        return false;
    }

    mupnp_http_response_clear(httpReq->httpRes);

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

    mupnp_http_response_print(httpReq->httpRes);
    mupnp_socket_close(g_sock);
    mupnp_socket_delete(g_sock);
    g_sock = NULL;
    mupnp_http_request_delete(httpReq);

    mupnp_log_debug_l4("Leaving...\n");
    mupnp_log_info("cg_http_request_post_test: Done\n");

    return readStatus;
}

char *Normal_Base64(char *input_buffer)
{

    unsigned char *buffer;

    int buffer_len;

    int y;

    size_t len;

    size_t input_len;

    input_len = strlen(input_buffer);

    y = input_len % 3;

    if (y != 0)

    {
        buffer_len = ((input_len) + (3 - y)) * 4;
    }
    else
    {
        buffer_len = (input_len)*4;
    }

    buffer_len = buffer_len / 3;

    buffer = (unsigned char *)malloc(buffer_len + 1);

    if (buffer == NULL)
    {
        return NULL;
    }

    if (mbedtls_base64_encode(buffer, (buffer_len + 1), &len, (const unsigned char *)input_buffer, input_len) != 0)
    {
        free(buffer);
        mupnp_log_info("base64 error...");
        return NULL;
    }
    else
    {

        return (char *)buffer;
    }
}

char *Json_Base64(char *regver, char *meid, char *modelsms,
                  char *swver, char *SIM1Iccid, char *SIM1LteImsi)
{

    unsigned char *buffer;

    int buffer_len;

    int y;

    size_t len;

    size_t out_len;

    char *out = NULL;
#ifdef LWIP_HTTP_TEST
    out = httpgenerateJsonString(regver, meid, modelsms, swver, SIM1Iccid, SIM1LteImsi);
#endif
    if (out == NULL)
    {
        mupnp_log_info("JsonString error...");
        return NULL;
    }

    out_len = strlen(out);

    y = out_len % 3;

    if (y != 0)

    {
        buffer_len = ((out_len) + (3 - y)) * 4;
    }
    else
    {
        buffer_len = (out_len)*4;
    }

    buffer_len = (int)(buffer_len / 3);

    buffer = (unsigned char *)malloc(buffer_len + 1);

    if (mbedtls_base64_encode(buffer, (buffer_len + 1), &len, (const unsigned char *)out, out_len) != 0)
    {
        free(out);
        free(buffer);
        mupnp_log_info("base64 error...");
        return NULL;
    }
    else
    {
        free(out);
        return (char *)buffer;
    }
}

char *Http_iccid_reg(uint8_t *url_char, char *content_type, char *body_content, char *regver, char *meid, char *modelsms,
                     char *swver, char *SIM1Iccid, char *SIM1LteImsi, bool issecret)
{

    char *post_content;

    char *reg_result;

    if (issecret == false)
    {

        post_content = Json_Base64(regver, meid, modelsms, swver, SIM1Iccid, SIM1LteImsi);

        if (post_content == NULL)
        {

            mupnp_log_info("post_content error...");

            return false;
        }

        if ((reg_result = Http_post_reg((char *)url_char, content_type, post_content)) != NULL)
        {

            mupnp_log_info("reg_result+++ %s", reg_result);

            return reg_result;
        }
        else
        {

            return NULL;
        }
    }
    else
    {
        post_content = body_content;
        if ((reg_result = Http_post_reg((char *)url_char, content_type, post_content)) != NULL)
        {

            mupnp_log_info("reg_result %s", reg_result);

            return reg_result;
        }
        else
        {

            return NULL;
        }
    }
}
char *Http_post_reg(char *url_char, char *content_type, char *body_content)
{
    return NULL; //no use code, for coverity 39624
#if 0
    CgHttpApi *cg_http_api = NULL;

    char *content = NULL;

    cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));

    if (cg_http_api == NULL)
    {
        mupnp_log_info("MALLOC ERROR \n");
        return NULL;
    }
    else
    {
        if (Http_init(cg_http_api, url_char) != false)
        {
            cg_http_api->contentLen = mupnp_strlen(body_content);
            cg_http_api->g_httpReq = mupnp_http_request_new();

            if (cg_http_api->g_httpReq == NULL)
            {
                free(cg_http_api);
                mupnp_log_info("MALLOC ERROR \n");
                return NULL;
            }

            mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_POST);

            mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

            mupnp_http_request_setcontenttype(cg_http_api->g_httpReq, content_type);

            mupnp_http_request_setcontent(cg_http_api->g_httpReq, body_content);

            mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, cg_http_api->contentLen);

            content = cg_http_api_post_reg(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            mupnp_http_request_delete(cg_http_api->g_httpReq);

            mupnp_log_info("cg_http_request_post_test done\n");

            free(cg_http_api);

            mupnp_log_info("ram is freed, prepare to return... \n");

            return content;
        }
        else
        {
            free(cg_http_api);
            mupnp_log_info("init error ...\n");
            return NULL;
        }
    }
#endif
}
char *cg_http_api_post_reg(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure)
{

    char *content;

    char *method, *uri, *version;
    mUpnpString *firstLine;
    mUpnpSocket *g_sock = NULL;
    bool readStatus = false;
    mupnp_log_debug_l4("Entering...\n");

    if (ipaddr == NULL)
    {
        mupnp_log_info("ipaddr is NULL");
        return NULL;
    }
    mupnp_http_response_clear(httpReq->httpRes);

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
        return NULL;
    }

    if (mupnp_socket_connect(g_sock, ipaddr, port) == false)
    {
        mupnp_socket_delete(g_sock);
        return NULL;
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
        return NULL;
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
        g_sock = NULL;
        return NULL;
    }
    mupnp_http_response_print(httpReq->httpRes);

    //FIX ME:May cause content leak
    content = mupnp_string_getvalue((mUpnpString *)(httpReq->httpRes)->content);

    mupnp_socket_close(g_sock);
    mupnp_socket_delete(g_sock);
    g_sock = NULL;

    mupnp_log_debug_l4("Leaving...\n");
    mupnp_log_info("cg_http_request_post_test: Done\n");
    return content;
}
long cg_http_api_post_contentLen(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure)
{

    long contentLen;
    char *method, *uri, *version;
    mUpnpString *firstLine;
    mUpnpSocket *g_sock = NULL;
    bool readStatus = false;
    mupnp_log_debug_l4("Entering...\n");

    if (ipaddr == NULL)
    {
        mupnp_log_info("ipaddr is NULL");
        return 0;
    }

    mupnp_http_response_clear(httpReq->httpRes);

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
        return 0;
    }

    if (mupnp_socket_connect(g_sock, ipaddr, port) == false)
    {
        mupnp_socket_delete(g_sock);
        return 0;
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
        mupnp_socket_delete(g_sock);
        g_sock = NULL;
        return 0;
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
        g_sock = NULL;
        return 0;
    }
    contentLen = mupnp_http_packet_getcontentlength((mUpnpHttpPacket *)httpReq->httpRes);

    // cg_http_response_print(httpReq->httpRes);
    mupnp_socket_close(g_sock);
    mupnp_socket_delete(g_sock);
    g_sock = NULL;
    //    cg_http_request_delete(httpReq);

    mupnp_log_debug_l4("Leaving...\n");
    mupnp_log_info("cg_http_request_post_test: Done\n");
    return contentLen;
}

mUpnpHttpResponse *Http_get(Http_info *http_info1)
{
    CgHttpApi *cg_http_api;

    char *url_char;

    cg_http_api = http_info1->cg_http_api;

    url_char = http_info1->url;

    if (Http_init(cg_http_api, url_char) != false)
    {
        mupnp_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_GET);

        mupnp_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

        mupnp_http_request_setcontentlength(cg_http_api->g_httpReq, 0);

        mUpnpHttpResponse *ret;

        ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

        if (ret == NULL)
        {
            OSI_LOGI(0x1000760d, "Http_get ret NULL\n");
            return NULL;
        }

        OSI_LOGI(0x1000760e, "Http_get done\n");

        return ret;
    }
    else
    {
        OSI_LOGI(0x1000760f, "Http_get init error ...\n");

        return NULL;
    }
}
mUpnpHttpResponse *cg_http_api_response(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure)
{

    char *method, *uri, *version;
    //char *api_key;
    mUpnpString *firstLine;
    mUpnpSocket *g_sock = NULL;
    mUpnpHttpResponse *response = NULL;
    bool is_read = false;

    OSI_LOGI(0x10007610, "Entering cg_http_api_response...\n");

    if (ipaddr == NULL)
    {
        OSI_LOGI(0x100075fe, "ipaddr is NULL");
        return NULL;
    }
    mupnp_http_response_clear(httpReq->httpRes);

    mupnp_string_setnvalue((httpReq->httpRes)->content, '\0', 0);

    OSI_LOGI(0x10007611, "(HTTP) Posting:\n");

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
        OSI_LOGI(0x10007600, "new socket fail");
        return NULL;
    }

    if (mupnp_socket_connect(g_sock, ipaddr, port) == false)
    {
        mupnp_socket_delete(g_sock);
        if (isSecure == true)
        {
            Http_WriteUart("failure, pelase check your network or certificate!\n", 51);
        }
        else
        {
            //Http_WriteUart("failure, pelase check your network!\n", 36);
        }
        return NULL;
    }

    mupnp_socket_settimeout(g_sock, mupnp_http_request_gettimeout(httpReq));
    mupnp_http_request_sethost(httpReq, ipaddr, port);
    /* add headers here */
    mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)httpReq, CG_HTTP_USERAGENT, mupnp_http_request_getuseragent(httpReq));
    /* prepare to send firstline */
    method = mupnp_http_request_getmethod(httpReq);
    uri = mupnp_http_request_geturi(httpReq);
    version = mupnp_http_request_getversion(httpReq);
    //api_key = (char *)mupnp_http_request_getapikey(httpReq);

    if (method == NULL || uri == NULL || version == NULL)
    {
        //cg_socket_close(sock);
        mupnp_socket_delete(g_sock);
        g_sock = NULL;
        Http_WriteUart("failure, parm error!\n", 22);
        return NULL;
    }
    OSI_LOGXI(OSI_LOGPAR_SS, 0x10007612, "uri method get %s %s", uri, method);
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

    is_read = mupnp_http_response_read(httpReq->httpRes, g_sock, mupnp_http_request_isheadrequest(httpReq));
    if (is_read == true)
        response = httpReq->httpRes;

    mupnp_socket_close(g_sock);
    mupnp_socket_delete(g_sock);
    g_sock = NULL;

    OSI_LOGI(0x10007613, "cg_http_api_response read status is %d: Done\n", is_read);

    return response;
}

#if defined(MUPNP_USE_OPENSSL)
#define AT_HTTPS_FILE_NAME_LEN 32
static uint8_t caCrt_filename[] = "/caCrt.pem";
static uint8_t clientCrt_filename[] = "/clientCrt.pem";
;
static uint8_t clientPrivateKey_filename[] = "/privateKey.pem";

bool Write_to_file(uint8_t *filename, char *pemData, uint32_t pemLen)
{
    mupnp_log_info("enter  Write_to_file. file: %s,pemLen:%d \n", filename, pemLen);

    int fd = -1;
    fd = vfs_open((const char *)filename, O_WRONLY | O_TRUNC, 0);
    if (fd < 0)
    {
        fd = vfs_open((const char *)filename, O_CREAT | O_WRONLY | O_TRUNC);
        if (fd < 0)
        {
            mupnp_log_info("[%s]openfailing.h=%d", filename, fd);
            return false;
        }
    }

    if (vfs_write(fd, pemData, pemLen) != pemLen)
    {
        mupnp_log_info("write [%s] error", filename);
        vfs_close(fd);
        return false;
    }
    vfs_close(fd);
    return true;
}

bool Https_saveCrttoFile(char *pemData, uint32_t pemLen, uint32_t pemtype)
{
    mupnp_log_info("enter  Https_saveCrttoFile. pemLen:%d,pemtype:%d \n", pemLen, pemtype);

    switch (pemtype)
    {
    case CG_HTTPS_CA_CRT: //set ca certificate filename
        return Write_to_file(caCrt_filename, pemData, pemLen);
    case CG_HTTPS_CLIENT_CRT: //set client certificate filename
        return Write_to_file(clientCrt_filename, pemData, pemLen);
    case CG_HTTPS_CLIENT_PRIVATE_KEY: //set client private key filename
        return Write_to_file(clientPrivateKey_filename, pemData, pemLen);
    default:
        mupnp_log_info("pemtype ERROR \n");
        return false;
    }
}

bool Read_from_file(uint8_t *filename, uint8_t **crtPem)
{
    mupnp_log_info("filename:%s. \n", filename);
    int fd = -1;
    uint32_t fileSize = 0;
    int result;
    //ileSize = vfs_file_size(filename);
    fd = vfs_open((const char *)filename, O_RDONLY, 0);
    if (fd < 0)
    {
        //Http_WriteUart("ERROR! not set certificate.\n",strlen("ERROR! not set certificate.\n"));
        //Http_WriteUart("\n",strlen("\n"));
        mupnp_log_info("vfs_open error,fd:%d\n", fd);
        return false;
    }
    struct stat st;
    vfs_fstat(fd, &st);
    fileSize = st.st_size;
    mupnp_log_info("fd:%d,vfs_file_size:%d\n", fd, fileSize);
    if (fileSize < 0)
    {
        mupnp_log_info("fileSize error\n");
        vfs_close(fd);
        return false;
    }
    if (*crtPem != NULL)
    {
        mupnp_log_info("free crtPem!! \n");
        free(*crtPem);
        *crtPem = NULL;
    }
    *crtPem = (uint8_t *)malloc(fileSize + 1);
    if (NULL == *crtPem)
    {
        mupnp_log_info("malloc error!! \n");
        vfs_close(fd);
        return false;
    }
    memset(*crtPem, 0, fileSize + 1);
    // Read  crt data from file to buffer
    result = vfs_read(fd, *crtPem, fileSize);
    if (result != fileSize)
    {
        mupnp_log_info("not read all data!! \n");
        free(*crtPem);
        *crtPem = NULL;
        vfs_close(fd);
        return false;
    }
    vfs_close(fd);
    return true;
}
bool Https_setCrt(uint32_t pemtype, uint8_t **crtPem)
{
    mupnp_log_info("+++++++enter  Https_setCrt. pemtype:%d \n", pemtype);

    switch (pemtype)
    {
    case CG_HTTPS_CA_CRT: //set ca certificate
        return Read_from_file(caCrt_filename, crtPem);
    case CG_HTTPS_CLIENT_CRT: //set client certificate
        return Read_from_file(clientCrt_filename, crtPem);
    case CG_HTTPS_CLIENT_PRIVATE_KEY: //set client private key
        return Read_from_file(clientPrivateKey_filename, crtPem);
    default:
        mupnp_log_info("pemtype ERROR \n");
        return false;
    }
}

void http_response_print(mUpnpHttpPacket *httpPkt)
{
    mUpnpHttpHeader *header;
    char *content;
    long contentLen;
    char *headerName;
    char *headerValue;

    /**** print headers ****/
    for (header = mupnp_http_packet_getheaders(httpPkt); header != NULL; header = mupnp_http_header_next(header))
    {
        headerName = (char *)mupnp_http_header_getname(header);
        headerValue = (char *)mupnp_http_header_getvalue(header);

        Http_WriteUart(headerName, strlen(headerName));
        Http_WriteUart(headerValue, strlen(headerValue));
    }

    /**** print content ****/
    content = mupnp_http_packet_getcontent(httpPkt);
    contentLen = mupnp_http_packet_getcontentlength(httpPkt);

    if (content != NULL && 0 < contentLen)
    {

        Http_WriteUart(content, contentLen);
    }
}
#endif

#endif
