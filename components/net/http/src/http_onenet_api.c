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
#include "http_onenet_api.h"
#include "http_api.h"
#include "mupnp/http/http.h"
#include "mupnp/net/socket.h"
#include "string.h"
//#include "dsm_cf.h"
#include "mbedtls/base64.h"
#include "http_json.h"
//#include "at_utils.h"
//#include "async_worker.h"

extern Http_onenet_info *at_Http_info1;

mUpnpHttpRequest *onenet_req;
//static AWORKER_REQ *s_http_onenet_aworker_req;
static bool is_onenet_aworker_runing = false;

#if 0
static AWORKER_RC http_onenet_worker_handler(AWORKER_REQ *req)
{
    uint32_t event = req->event;
    AWORKER_RC rc = AWRC_SUCCESS;
    bool paramret = true;
    uint16_t index = 0;
    mUpnpHttpResponse *response = NULL;
    Http_onenet_info *Http_info1;
    nHttp_info *nHttp_info1;
    switch (event)
    {
    case HTTPX_EV_ONENET_GET:
    case HTTPX_EV_ONENET_DELETE:
    case HTTPX_EV_ONENET_POST:
    case HTTPX_EV_ONENET_PUT:
    {

		Http_info1 = (Http_onenet_info *)aworker_param_getu32(req, index, &paramret);
        if (!paramret)
        {
            rc = AWRC_FAIL;
			is_onenet_aworker_runing = false;
            break;
        }

        if (http_active(Http_info1) == false)
        {
            rc = AWRC_FAIL;
			is_onenet_aworker_runing = false;
            break;
        }else{

			is_onenet_aworker_runing = false;
			rc = AWRC_SUCCESS;

		}
        break;
    }

    default:
		{
			rc = AWRC_FAIL;
			is_onenet_aworker_runing = false;
		}
        break;
    }
    if (rc != AWRC_PROCESSING)
    {
        // if return value was not AWRC_PROCESSING,
        // the request will be deleted by aworker, and become invalid
        s_http_onenet_aworker_req = NULL;
    }
    return rc;
}
static AWORKER_REQ *http_onenet_create_async_req(uint32_t event, void *param, uint32_t bufflen)
{
    AWORKER_REQ *areq = aworker_create_request(COS_GetCurrentTaskHandle(),
                                               http_onenet_worker_handler, NULL, event, param, bufflen);
    return areq;
}
bool http_post_req_delay(AWORKER_REQ *req, uint32_t delay_ms, bool *result)
{

    return aworker_post_req_delay(req, delay_ms, result);
}
#endif
HTTPX_ONENET_EVT http_event_check(char *method)
{

    if (strcmp(method, CG_HTTP_POST) == 0)
    {
        return HTTPX_EV_ONENET_POST;
    }
    else if (strcmp(method, CG_HTTP_GET) == 0)
    {
        return HTTPX_EV_ONENET_GET;
    }
    else if (strcmp(method, CG_HTTP_DELETE) == 0)
    {
        return HTTPX_EV_ONENET_DELETE;
    }
    else if (strcmp(method, CG_HTTP_PUT) == 0)
    {

        return HTTPX_EV_ONENET_PUT;
    }
    else
    {

        return 0;
    }
}

bool http_start(char *method, uint32_t val, bool *result)
{

    Http_onenet_info *Http_info2;

    if (netif_default == NULL)
    {

        return false;
    }

    Http_info2 = (Http_onenet_info *)val;

    if (strlen(Http_info2->host) < 2)
    {

        return false;
    }

    //HTTPX_ONENET_EVT ONENET_EVENT;

    if (is_onenet_aworker_runing)
    {

        return false;
    }

    if (http_event_check(method))
    {

        //	ONENET_EVENT = http_event_check(method);
    }
    else
    {

        return false;
    }
#if 0
    s_http_onenet_aworker_req = http_onenet_create_async_req(ONENET_EVENT, NULL, 0);

    if (s_http_onenet_aworker_req == NULL)
    {
        return false;
    }

    aworker_param_putu32(s_http_onenet_aworker_req, val, result);

    if (!(*result))
    {

        return false;
    }

	is_onenet_aworker_runing = true;
	if (!http_post_req_delay(s_http_onenet_aworker_req, 0, result))
    {
		is_onenet_aworker_runing = false;
		return false;
    }
#endif
    return true;
}

void str_escape(char *dest, char *str)
{

    char *des;
    des = dest;

    mupnp_log_info("str+++ %d", mupnp_strlen(str));

    if (str == NULL || dest == NULL)
    {

        return;
    }

    while (*str != '\0')
    {

        if (*str == '\\' && *(str++) == '\"')
        {

            *des = '\"';
            str++;
        }
        else
        {

            *des = *str;
        }
        str++;
        des++;
    }
}

void create_request(char *name, char *value)
{

    if ((name != NULL) && (value != NULL))
    {

        if (strcmp(name, "content") == 0)
        {

            mupnp_http_request_setcontent(onenet_req, value);
            mupnp_log_info("length %d", mupnp_strlen(value));
            mupnp_http_request_setcontentlength(onenet_req, mupnp_strlen(value));
        }
        else if (strcmp(name, "method") == 0)
        {

            mupnp_http_request_setmethod(onenet_req, value);
        }
        else
        {

            mupnp_http_packet_setheadervalue((mUpnpHttpPacket *)onenet_req, name, value);
        }
    }
}
bool http_active(Http_onenet_info *onenet_info1)
{

    bool ret_val = false;

    mUpnpHttpResponse *ret = NULL;

    mupnp_http_request_seturi(onenet_req, onenet_info1->uri_path);

    ret = onenet_http_api_response(onenet_req, onenet_info1);

    if (onenet_info1->resp_h != NULL && ret != NULL)
    {
        onenet_info1->resp_h(ret);
    }

    if (ret == NULL)
    {
        onenet_info1->falg_done = false;
        ret_val = false;
    }
    else
    {
        onenet_info1->falg_done = true;
        ret_val = true;
    }
    if (onenet_info1->fin_h != NULL)
    {

        onenet_info1->fin_h(onenet_info1->falg_done);
    }
    return ret_val;
}

mUpnpHttpResponse *onenet_http_api_response(mUpnpHttpRequest *httpReq, Http_onenet_info *onenet_info1)
{

    char *method, *uri, *version;
    mUpnpString *firstLine;
    mUpnpSocket *g_sock = NULL;
    mUpnpHttpResponse *response = NULL;

    char *ipaddr;
    int port;
    bool isSecure;
    bool isRead = false;

    ipaddr = onenet_info1->host;
    port = onenet_info1->port;
    isSecure = onenet_info1->ishttps;

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
        if (onenet_info1->conFAIL_h != NULL)
        {

            onenet_info1->conFAIL_h(g_sock);
        }
        mupnp_socket_delete(g_sock);
        return NULL;
    }
    if (onenet_info1->sock_h != NULL)
    {

        onenet_info1->sock_h(g_sock);
    }
    mupnp_socket_settimeout(g_sock, mupnp_http_request_gettimeout(httpReq));
    mupnp_http_request_sethost(httpReq, ipaddr, port);

    /* prepare to send firstline */
    method = mupnp_http_request_getmethod(httpReq);
    uri = mupnp_http_request_geturi(httpReq);
    version = mupnp_http_request_getversion(httpReq);

    mupnp_log_info("uri method get %s %s", uri, method);

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

    isRead = mupnp_http_response_read(httpReq->httpRes, g_sock, mupnp_http_request_isheadrequest(httpReq));
    if (isRead == true)
        response = httpReq->httpRes;

    mupnp_socket_close(g_sock);
    mupnp_socket_delete(g_sock);
    g_sock = NULL;

    mupnp_log_info("cg_http_request_post_test read status is %d: Done\n", isRead);

    return response;
}

Http_onenet_info *Init_onenet_Http(void)
{
    Http_onenet_info *InitHttp;

    //mupnp_log_add_target("stdout", SEV_INFO | SEV_DEBUG_L1 | SEV_DEBUG_L2 | SEV_DEBUG_L3);

    InitHttp = (Http_onenet_info *)malloc(sizeof(Http_onenet_info));
    if (InitHttp == NULL)
    {

        return NULL;
    }

    onenet_req = mupnp_http_request_new();

    if (onenet_req == NULL)
    {

        free(InitHttp);
        return NULL;
    }

    InitHttp->USER_AGENT = "RDA_8955";
    InitHttp->host = (char *)malloc(64);
    if (InitHttp->host == NULL)
    {

        mupnp_http_request_delete(onenet_req);
        free(InitHttp);
        return NULL;
    }
    memset(InitHttp->host, 0, 64);
    InitHttp->port = 0;
    InitHttp->TIMEOUT = 0;
    InitHttp->body_content = (char *)malloc(sizeof(char) * 1024);
    if (InitHttp->body_content == NULL)
    {

        free(InitHttp->host);
        mupnp_http_request_delete(onenet_req);
        free(InitHttp);
        return NULL;
    }
    memset(InitHttp->body_content, 0, 1024);
    InitHttp->uri_path = (char *)malloc(256);
    if (InitHttp->uri_path == NULL)
    {

        free(InitHttp->body_content);
        free(InitHttp->host);
        mupnp_http_request_delete(onenet_req);
        free(InitHttp);
        return NULL;
    }
    memset(InitHttp->uri_path, 0, 256);
    InitHttp->API_KEY = (char *)malloc(256);
    if (InitHttp->API_KEY == NULL)
    {

        free(InitHttp->uri_path);
        free(InitHttp->body_content);
        free(InitHttp->host);
        mupnp_http_request_delete(onenet_req);
        free(InitHttp);
        return NULL;
    }
    memset(InitHttp->API_KEY, 0, 256);
    InitHttp->title = (char *)malloc(256);
    if (InitHttp->title == NULL)
    {

        free(InitHttp->API_KEY);
        free(InitHttp->uri_path);
        free(InitHttp->body_content);
        free(InitHttp->host);
        mupnp_http_request_delete(onenet_req);
        free(InitHttp);
        return NULL;
    }
    memset(InitHttp->title, 0, 256);
    InitHttp->desc = (char *)malloc(256);
    if (InitHttp->desc == NULL)
    {

        free(InitHttp->title);
        free(InitHttp->API_KEY);
        free(InitHttp->uri_path);
        free(InitHttp->body_content);
        free(InitHttp->host);
        mupnp_http_request_delete(onenet_req);
        free(InitHttp);
        return NULL;
    }
    memset(InitHttp->desc, 0, 256);
    InitHttp->dev_apikey = (char *)malloc(256);
    if (InitHttp->dev_apikey == NULL)
    {
        free(InitHttp->desc);
        free(InitHttp->title);
        free(InitHttp->API_KEY);
        free(InitHttp->uri_path);
        free(InitHttp->body_content);
        free(InitHttp->host);
        mupnp_http_request_delete(onenet_req);
        free(InitHttp);
        return NULL;
    }
    memset(InitHttp->dev_apikey, 0, 256);
    InitHttp->dev_id = (char *)malloc(256);
    if (InitHttp->dev_id == NULL)
    {
        free(InitHttp->dev_apikey);
        free(InitHttp->desc);
        free(InitHttp->title);
        free(InitHttp->API_KEY);
        free(InitHttp->uri_path);
        free(InitHttp->body_content);
        free(InitHttp->host);
        mupnp_http_request_delete(onenet_req);
        free(InitHttp);
        return NULL;
    }
    memset(InitHttp->dev_id, 0, 256);
    InitHttp->auth_info = (char *)malloc(256);
    if (InitHttp->auth_info == NULL)
    {
        free(InitHttp->dev_id);
        free(InitHttp->dev_apikey);
        free(InitHttp->desc);
        free(InitHttp->title);
        free(InitHttp->API_KEY);
        free(InitHttp->uri_path);
        free(InitHttp->body_content);
        free(InitHttp->host);
        mupnp_http_request_delete(onenet_req);
        free(InitHttp);
        return NULL;
    }
    memset(InitHttp->auth_info, 0, 256);
    InitHttp->error_string = (char *)malloc(256);
    if (InitHttp->error_string == NULL)
    {
        free(InitHttp->auth_info);
        free(InitHttp->dev_id);
        free(InitHttp->dev_apikey);
        free(InitHttp->desc);
        free(InitHttp->title);
        free(InitHttp->API_KEY);
        free(InitHttp->uri_path);
        free(InitHttp->body_content);
        free(InitHttp->host);
        mupnp_http_request_delete(onenet_req);
        free(InitHttp);
        return NULL;
    }
    memset(InitHttp->error_string, 0, 256);
    InitHttp->ishttps = false;
    InitHttp->falg_done = false;
    InitHttp->flag_pro = true;
    InitHttp->sock_h = NULL;
    InitHttp->resp_h = NULL;
    InitHttp->fin_h = NULL;
    InitHttp->conFAIL_h = NULL;

    return InitHttp;
}
bool Term_onenet_Http(Http_onenet_info *nHttp_inf)
{

    if (is_onenet_aworker_runing)
    {

        return false;
    }
    if (onenet_req != NULL)
    {

        mupnp_http_request_delete(onenet_req);
    }

    if (nHttp_inf->hah != NULL)
    {

        nHttp_inf->hah = NULL;
    }

    if (nHttp_inf->sock_h != NULL)
    {

        nHttp_inf->sock_h = NULL;
    }

    if (nHttp_inf->resp_h != NULL)
    {

        nHttp_inf->resp_h = NULL;
    }

    if (nHttp_inf->fin_h != NULL)
    {

        nHttp_inf->fin_h = NULL;
    }

    if (nHttp_inf->conFAIL_h != NULL)
    {

        nHttp_inf->conFAIL_h = NULL;
    }

    if (nHttp_inf->host != NULL)
    {

        free(nHttp_inf->host);
    }
    if (nHttp_inf->desc != NULL)
    {

        free(nHttp_inf->desc);
    }
    if (nHttp_inf->title != NULL)
    {

        free(nHttp_inf->title);
    }
    if (nHttp_inf->dev_apikey != NULL)
    {

        free(nHttp_inf->dev_apikey);
    }
    if (nHttp_inf->dev_id != NULL)
    {

        free(nHttp_inf->dev_id);
    }

    if (nHttp_inf->body_content != NULL)
    {

        free(nHttp_inf->body_content);
    }
    if (nHttp_inf->uri_path != NULL)
    {

        free(nHttp_inf->uri_path);
    }
    if (nHttp_inf->API_KEY != NULL)
    {

        free(nHttp_inf->API_KEY);
    }
    if (nHttp_inf->auth_info != NULL)
    {

        free(nHttp_inf->auth_info);
    }
    if (nHttp_inf->error_string != NULL)
    {

        free(nHttp_inf->error_string);
    }
    if (nHttp_inf != NULL)
    {

        free(nHttp_inf);
    }
    return true;
}
