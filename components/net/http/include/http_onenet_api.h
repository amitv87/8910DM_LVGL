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
*	File: chttp.h
*
*	Revision:
*
*	01/25/05
*		- first revision
*
*	10/31/05
*		- Changed CHttpHeader* pointers to CHttpServer* pointers
*		  in CHttpServer struct.
*	
*	16-Jan-06 Aapo Makela
*		- Fixed header comparisons to be case-insensitive
*		- Fixed iskeepaliveconnection to check "Connection:close"
*		- Added M-POST specific stuff
*	04/03/06 Theo Beisch
*		- added (CgHttpHeaderList*) cast (see below)
*		- added LONGLONG macro for platform independence
*		- some re-sorting of get/set macros (see below)
******************************************************************/

#ifndef _HTTP_ONENET_API_H_
#define _HTTP_ONENET_API_H_

#include "http_types.h"
#include "http_if.h"
#include "mupnp/util/string.h"
#include "mupnp/util/list.h"
#include "mupnp/util/thread.h"
#include "mupnp/util/time.h"
#include "mupnp/util/mutex.h"
#include "mupnp/net/socket.h"
#include "mupnp/net/url.h"
#include "mupnp/http/http.h"
#include "lwip/sockets.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/
typedef void (*http_socket_handler)(mUpnpSocket *sock_http);
typedef void (*http_response_handler)(mUpnpHttpResponse *response_http);
typedef void (*http_finish_handler)(bool flag);
typedef void (*http_connectFAIL_handler)(mUpnpSocket *sock_http);

/****************************************
Define nHttp_info
****************************************/
#define BODY_CONTENT 1
#define USER_AGENT_1 2
#define PROIP_1 3
#define PROPORT_1 4
#define REDIR_1 5
#define BREAK_1 6
#define BREAKEND_1 7
#define TIMEOUT_1 8
#define CONTENT_TYPE_1 9
#define URL_1 10
#define USERDATA_1 11
#define API_KEY_1 12

/****************************************
* Data Type
****************************************/
typedef enum at_httpx_onenet_event
{
    HTTPX_EV_ONENET_GET = 1,
    HTTPX_EV_ONENET_POST,
    HTTPX_EV_ONENET_DELETE,
    HTTPX_EV_ONENET_PUT,
} HTTPX_ONENET_EVT;

typedef struct _http_onenet_inf
{
    /**url info**/
    char *url;
    char *uri_path;
    char *host;
    int port;
    bool ishttps;
    /** headers info **/
    long content_length;
    char *USER_AGENT;
    int REDIR;
    int BREAK;
    int BREAKEND;
    int TIMEOUT;
    char *CONTENT_TYPE;
    char *API_KEY;
    /** content info**/
    char *body_content;
    long data_length;
    /** cm_status info **/
    long total;
    long sum;
    long cur;
    long delay;
    int urc;
    char *title;
    char *desc;
    char *dev_id;
    char *dev_apikey;
    int lon;
    int lat;
    char *auth_info;
    char *error_string;
    /**finish flag**/
    bool falg_done;
    bool flag_pro;
    /** aworker handler **/
    http_aworker_handler hah;
    /**callback handler**/
    http_socket_handler sock_h;
    http_response_handler resp_h;
    http_finish_handler fin_h;
    http_connectFAIL_handler conFAIL_h;
} Http_onenet_info;

mUpnpHttpResponse *onenet_http_api_response(mUpnpHttpRequest *httpReq, Http_onenet_info *onenet_info1);
void create_request(char *name, char *value);
bool http_active(Http_onenet_info *onenet_info1);
Http_onenet_info *Init_onenet_Http(void);
bool Term_onenet_Http(Http_onenet_info *nHttp_inf);
void str_escape(char *dest, char *str);
//static INT32 http_onenet_active(uint32_t delay, void *arg);
bool http_start(char *method, uint32_t val, bool *result);

#ifdef __cplusplus
}
#endif

#endif
