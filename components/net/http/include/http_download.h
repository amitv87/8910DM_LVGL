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

#ifndef _HTTP_DEMO_H_
#define _HTTP_DEMO_H_

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
#include "http_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define AT_HTTP_DOWNLOAD_DELAY 1000
#define AT_HTTP_GETN_DELAY 1000
#define AT_HTTP_POSTN_DELAY 1000

/****************************************
* Function (Http api)
****************************************/
bool Http_downLoad(Http_info *http_info1);

bool cg_http_api_downLoad(mUpnpHttpRequest *httpReq, char *ipaddr, int port, bool isSecure, Http_info *http_info1);

#ifdef __cplusplus
}
#endif

#endif
