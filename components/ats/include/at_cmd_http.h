// //////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2012, Coolsand Technologies, Inc.
// All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmission of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: at_cmd_gprs.h
//
// DESCRIPTION:
// TODO: ...
//
// REVISION HISTORY:
// NAME              DATE                REMAKS
// Lixp      2012-2-20       Created initial version 1.0
//
// //////////////////////////////////////////////////////////////////////////////
#ifndef __AT_CMD_HTTP_H__
#define __AT_CMD_HTTP_H__
#include "ats_config.h"
#include "at_engine.h"
#include "at_command.h"

#ifdef CONFIG_AT_HTTP_SUPPORT

void AT_CmdFunc_HTTPHEAD(atCommand_t *pParam);
void process_HTTP_html(const unsigned char *data, size_t data_len);
void process_HTTPDLD_data(const unsigned char *data, size_t len);
void AT_CmdFunc_HTTPPARA(atCommand_t *pParam);
void AT_CmdFunc_HTTPACTION(atCommand_t *pParam);
void AT_CmdFunc_HTTPSTATUS(atCommand_t *pParam);
void AT_CmdFunc_HTTPREAD(atCommand_t *pParam);
void AT_CmdFunc_HTTPDATA(atCommand_t *pParam);
void AT_CmdFunc_HTTPINIT(atCommand_t *pParam);
void AT_CmdFunc_HTTPTERM(atCommand_t *pParam);
void SetHttpEngine(atCmdEngine_t *Engine);
void Http_WriteUart(const char *data, unsigned length);

//void memory_Free_Http(Http_info *http_info);
void AT_CmdFunc_HTTPSSETCRT(atCommand_t *pParam);

#endif

#endif
