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

#ifndef _HTTP_API_H_
#define _HTTP_API_H_

#ifdef CONFIG_LWIP_HTTP_TEST
#define LWIP_HTTP_TEST
#endif
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

/****************************************
Define nHttp_info
****************************************/

#define CID_1 1
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
Define nHttp_info
****************************************/

#define HTTP_SEND_MAX 65534

#define CG_HTTP_READLINE_BUFSIZE 512
#define CG_HTTP_SEVERNAME_MAXLEN 64
#define CG_HTTP_DATE_MAXLEN 128

#define CG_HTTP_REQUESTLINE_DELIM "\r\n "
#define CG_HTTP_STATUSLINE_DELIM "\r\n "
#define CG_HTTP_HEADERLINE_DELIM "\r\n :"

#define CG_HTTP_CR "\r"
#define CG_HTTP_LF "\n"
#define CG_HTTP_CRLF "\r\n"
#define CG_HTTP_SP " "
#define CG_HTTP_COLON ":"

#define CG_HTTP_VER10 "HTTP/1.0"
#define CG_HTTP_VER11 "HTTP/1.1"

#if !defined(CG_HTTP_CURL)
#define CG_HTTP_USERAGENT_DEFAULT "CyberGarage-HTTP/1.0"
#else
#define CG_HTTP_USERAGENT_DEFAULT "libcurl-agent/1.0 (Cyberlink for C)"
#endif

#define CG_HTTP_USERAGENT_DEVICE "8910"

#define CG_HTTP_DEFAULT_PORT 80
#define CG_HTTPS_DEFAULT_PORT 443

#define CG_HTTP_STATUS_CONTINUE 100
#define CG_HTTP_STATUS_OK 200
#define CG_HTTP_STATUS_PARTIAL_CONTENT 206
#define CG_HTTP_STATUS_BAD_REQUEST 400
#define CG_HTTP_STATUS_NOT_FOUND 404
#define CG_HTTP_STATUS_METHOD_NOT_ALLOWED 405
#define CG_HTTP_STATUS_PRECONDITION_FAILED 412
#define CG_HTTP_STATUS_INVALID_RANGE 416
#define CG_HTTP_STATUS_INTERNAL_SERVER_ERROR 500

#define CG_HTTP_POST "POST"
#define CG_HTTP_GET "GET"
#define CG_HTTP_HEAD "HEAD"
#define CG_HTTP_PUT "PUT"
#define CG_HTTP_OPTIONS "OPTIONS"
#define CG_HTTP_TRACE "TRACE"
#define CG_HTTP_DELETE "DELETE"

#define CG_HTTP_HOST "HOST"
#define CG_HTTP_DATE "Date"
#define CG_HTTP_CACHE_CONTROL "Cache-Control"
#define CG_HTTP_NO_CACHE "no-cache"
#define CG_HTTP_MAX_AGE "max-age"
#define CG_HTTP_CONNECTION "Connection"
#define CG_HTTP_CLOSE "close"
#define CG_HTTP_KEEP_ALIVE "Keep-Alive"
#define CG_HTTP_CONTENT_TYPE "Content-Type"
#define CG_HTTP_CONTENT_LENGTH "Content-Length"
#define CG_HTTP_CONTENT_RANGE "Content-Range"
#define CG_HTTP_CONTENT_RANGE_BYTES "bytes"
#define CG_HTTP_LOCATION "Location"
#define CG_HTTP_SERVER "Server"
#define CG_HTTP_RANGE "Range"
#define CG_HTTP_TRANSFER_ENCODING "Transfer-Encoding"
#define CG_HTTP_CHUNKED "Chunked"
#define CG_HTTP_USERAGENT "User-Agent"
#define CG_HTTP_AUTHOR "Authorization"
/**** SOAP Extention ****/
#define CG_HTTP_SOAP_ACTION "SOAPACTION"
#define CG_HTTP_SOAP_ACTION_WITH_NS "01-SOAPACTION"
#define CG_HTTP_SOAP_MAN_VALUE "\"http://schemas.xmlsoap.org/soap/envelope/\"; ns=01"

/**** UPnP Extention ****/
#define CG_HTTP_MPOST "M-POST"
#define CG_HTTP_MSEARCH "M-SEARCH"
#define CG_HTTP_NOTIFY "NOTIFY"
#define CG_HTTP_SUBSCRIBE "SUBSCRIBE"
#define CG_HTTP_UNSUBSCRIBE "UNSUBSCRIBE"

#define CG_HTTP_ST "ST"
#define CG_HTTP_MX "MX"
#define CG_HTTP_MAN "MAN"
#define CG_HTTP_NT "NT"
#define CG_HTTP_NTS "NTS"
#define CG_HTTP_USN "USN"
#define CG_HTTP_EXT "EXT"
#define CG_HTTP_SID "SID"
#define CG_HTTP_SEQ "SEQ"
#define CG_HTTP_CALLBACK "CALLBACK"
#define CG_HTTP_TIMEOUT "TIMEOUT"

/**** CURL connection timeout: 2 seconds ****/
#define CG_HTTP_CURL_CONNECTTIMEOUT 2

/**** CURL total timeout: 30 seconds (UPnP DA: whole transfer: 30sec ****/
#define CG_HTTP_CONN_TIMEOUT 30

/* HTTP server - client thread blocking timeout */
#define CG_HTTP_SERVER_READ_TIMEOUT 120

/**** HTTP Status code reason phrases ****/
#define CG_HTTP_REASON_100 "Continue"
#define CG_HTTP_REASON_101 "Switching Protocols"
#define CG_HTTP_REASON_200 "OK"
#define CG_HTTP_REASON_201 "Created"
#define CG_HTTP_REASON_202 "Accepted"
#define CG_HTTP_REASON_203 "Non-Authoritative Information"
#define CG_HTTP_REASON_204 "No Content"
#define CG_HTTP_REASON_205 "Reset Content"
#define CG_HTTP_REASON_206 "Partial Content"
#define CG_HTTP_REASON_300 "Multiple Choices"
#define CG_HTTP_REASON_301 "Moved Permanently"
#define CG_HTTP_REASON_302 "Found"
#define CG_HTTP_REASON_303 "See Other"
#define CG_HTTP_REASON_304 "Not Modified"
#define CG_HTTP_REASON_305 "Use Proxy"
#define CG_HTTP_REASON_307 "Temporary Redirect"
#define CG_HTTP_REASON_400 "Bad Request"
#define CG_HTTP_REASON_401 "Unauthorized"
#define CG_HTTP_REASON_402 "Payment Required"
#define CG_HTTP_REASON_403 "Forbidden"
#define CG_HTTP_REASON_404 "Not Found"
#define CG_HTTP_REASON_405 "Method Not Allowed"
#define CG_HTTP_REASON_406 "Not Acceptable"
#define CG_HTTP_REASON_407 "Proxy Authentication Required"
#define CG_HTTP_REASON_408 "Request Time-out"
#define CG_HTTP_REASON_409 "Conflict"
#define CG_HTTP_REASON_410 "Gone"
#define CG_HTTP_REASON_411 "Length Required"
#define CG_HTTP_REASON_412 "Precondition Failed"
#define CG_HTTP_REASON_413 "Request Entity Too Large"
#define CG_HTTP_REASON_414 "Request-URI Too Large"
#define CG_HTTP_REASON_415 "Unsupported Media Type"
#define CG_HTTP_REASON_416 "Requested range not satisfiable"
#define CG_HTTP_REASON_417 "Expectation Failed"
#define CG_HTTP_REASON_500 "Internal Server Error"
#define CG_HTTP_REASON_501 "Not Implemented"
#define CG_HTTP_REASON_502 "Bad Gateway"
#define CG_HTTP_REASON_503 "Service Unavailable"
#define CG_HTTP_REASON_504 "Gateway Time-out"
#define CG_HTTP_REASON_505 "HTTP Version not supported"

/****************************************
* Data Type
****************************************/
typedef struct _CgHttpApi
{
    mUpnpNetURI *url;
    mUpnpHttpRequest *g_httpReq;
    char *host;
    int port;
    char *uri_path;
    char *content_type;
    char *content_body;
    mUpnpHttpResponse *response;
    int contentLen;
    bool is_http;
    bool is_https;
    bool ret;
    uint8_t nSIM;
    uint8_t nCID;
} CgHttpApi;

typedef struct _http_inf
{
    char *username;
    char *password;
    long contentLen;
    char *content_type;
    char *body_content;
    char *content_name;
    char *url;
    CgHttpApi *cg_http_api;
} Http_info;

typedef struct _nhttp_inf
{
    uint8_t CID;
    char *url;
    char *USER_AGENT;
    char *pro_host;
    char *tag;
    char *value;
    long offset;
    long length;
    int pro_port;
    int REDIR;
    int BREAK;
    int BREAKEND;
    int TIMEOUT;
    char *CONTENT_TYPE;
    char *API_KEY;
    void *user_data;
    long data_length;
    CgHttpApi *cg_http_api;
    struct ifreq device;
    int method_code;
    int function_status;
    int status_code;
    long content_length;
    long data_finish;
    long data_remain;
    bool user_data_used;
    bool breakFlag;
} nHttp_info;

/****************************************
* Function (Data Post)
****************************************/
#define SIZE_HTTP_USR 256
bool cg_http_api_post(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure);

/****************************************
* Function (Http api)
****************************************/
void set_fota_data_handler(packet_handler_t handler);

bool Http_init(CgHttpApi *cg_http_api, char *url_char);

mUpnpHttpResponse *Http_get(Http_info *http_info1);

mUpnpHttpResponse *Http_head(Http_info *http_info1);

mUpnpHttpResponse *Http_post(Http_info *http_info1);
bool Http_put(Http_info *http_info1);
mUpnpHttpResponse *Http_options(Http_info *http_info1);

mUpnpHttpResponse *Http_trace(Http_info *http_info1);

bool Http_delete(Http_info *http_info1);
bool cg_http_api_post(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure);
char *Http_post_reg(char *url_char, char *content_type, char *body_content);

char *Http_iccid_reg(uint8_t *url_char, char *content_type, char *body_content, char *regver, char *meid, char *modelsms,
                     char *swver, char *SIM1Iccid, char *SIM1LteImsi, bool issecret);

char *Json_Base64(char *regver, char *meid, char *modelsms,
                  char *swver, char *SIM1Iccid, char *SIM1LteImsi);
char *cg_http_api_post_reg(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure);
mUpnpHttpResponse *Http_authorization(Http_info *http_info1);

char *Normal_Base64(char *input_buffer);
long cg_http_api_post_contentLen(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure);
mUpnpHttpResponse *cg_http_api_response(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure);
#if defined(MUPNP_USE_OPENSSL)
bool Https_saveCrttoFile(char *pemData, uint32_t pemLen, uint32_t pemtype);
bool Https_setCrt(uint32_t pemtype, uint8_t **crtPem);
#endif

Http_info *RD_Http_Init(void);
bool RDHttpTerm(Http_info *Http_inf);

nHttp_info *Init_Http(void);
bool Term_Http(nHttp_info *nHttp_inf);
int Http_strcmp(char *tag);
bool Http_para(nHttp_info *nHttp_inf, char *tag, char *value);
long Http_headn_contentlength(nHttp_info *http_info1);
bool Http_getn_break(nHttp_info *http_info1);
bool Http_getn(nHttp_info *http_info1);
bool Http_headn(nHttp_info *http_info1);
bool Http_postn(nHttp_info *http_info1);
bool Http_postnreg(nHttp_info *http_info1);
bool Http_deleten(nHttp_info *http_info1);
bool Http_onenet_put(nHttp_info *http_info1);
bool Http_onenet_delete(nHttp_info *http_info1);

mUpnpHttpResponse *cg_nhttp_api_response(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure, nHttp_info *http_info);

bool http_setUserdata(nHttp_info *at_nHttp_info, char *userData, long dataLen);
bool Http_read(nHttp_info *nHttp_info, long offset, long length);

bool Write_to_file(uint8_t *filename, char *pemData, uint32_t pemLen);
bool Read_from_file(uint8_t *filename, uint8_t **crtPem);
void http_response_print(mUpnpHttpPacket *httpPkt);

#ifdef __cplusplus
}
#endif

#endif
