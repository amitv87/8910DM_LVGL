// //////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2012, Coolsand Technologies, Inc.
// All Rights Reserved
//
// FILENAME: http_client_test.h
//
// REVISION HISTORY:
// NAME              DATE                REMAKS
// suminchen      2017-05-27       Created initial version 1.0
//
// //////////////////////////////////////////////////////////////////////////////
#ifndef __HTTP_LUNCH_API_H__
#define __HTTP_LUNCH_API_H__

#include "mupnp/http/http.h"
#include "http_api.h"
//#include "http_types.h"

extern int finish_flag;
extern void *http_handle;

bool lunch_http_Authorization(Http_info *http_info1);
bool lunch_http_put(Http_info *http_info1);
bool lunch_http_get(Http_info *http_info1);
bool lunch_http_downLoad(Http_info *http_info1, packet_handler_t handler);
bool lunch_http_post(Http_info *http_info1);
bool lunch_http_head(Http_info *http_info1);
bool lunch_http_options(Http_info *http_info1);
bool lunch_http_trace(Http_info *http_info1);
bool lunch_http_delete(Http_info *http_info1);
void start_http_task(void);
bool lunch_http_para(nHttp_info *http_info1, char *tag, char *value);
bool lunch_http_getn(nHttp_info *http_info1);
bool lunch_http_postn(nHttp_info *http_info1);
bool lunch_http_headn(nHttp_info *http_info1);
bool lunch_http_deleten(nHttp_info *http_info1);
bool lunch_http_data(nHttp_info *http_info1, uint8_t *data, long data_len);
bool lunch_http_read(nHttp_info *http_info1, long offset, long length);
bool lunch_http_onenet_delete(nHttp_info *http_info1);
bool lunch_http_onenet_put(nHttp_info *http_info1);

typedef enum http_test_type
{
    EV_HTTP_CONNECT = 101,
    EV_HTTP_AUTHOR,
    EV_HTTP_GET,
    EV_HTTP_POST,
    EV_HTTP_PUT,
    EV_HTTP_HEAD,
    EV_HTTP_TRACE,
    EV_HTTP_DELETE,
    EV_HTTP_OPTIONS,
    EV_HTTP_DOWNLOAD,
    EV_HTTP_GETN,
    EV_HTTP_HEADN,
    EV_HTTP_DELETEN,
    EV_HTTP_PARA,
    EV_HTTP_POSTN,
    EV_HTTP_READ,
    EV_HTTP_DATA,
    EV_HTTP_ONENET_PUT,
    EV_HTTP_ONENET_DELETE,
} HTTP_TEST_TYPE;

#endif
