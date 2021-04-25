/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _AT_PARSE_H_
#define _AT_PARSE_H_

#include <stdint.h>
#include <stdbool.h>
#include "at_command.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    AT_CMD_PARAM_TYPE_EMPTY,
    AT_CMD_PARAM_TYPE_RAW,

    AT_CMD_PARAM_TYPE_PARSED_STRING,
    AT_CMD_PARAM_TYPE_PARSED_DTMF
} atCmdParamType_t;

// =============================================================================
// at_ParseLine
// -----------------------------------------------------------------------------
/// Parse a command line (start with 'AT', and end with '\r' or "\r\n").
///
/// @return 0 if success, others if fail
// =============================================================================
int atParseLine(const char *cmdline, unsigned length, atCommandHead_t *cmd_list);

// APIs from bison/flex
int at_parse_parse(void *scanner);
int at_parse_lex_init_extra(void *extra, void **scanner);
void *at_parse__scan_bytes(const char *bytes, unsigned len, void *scanner);
int at_parse_lex_destroy(void *scanner);
extern unsigned at_parse_get_leng(void *scanner);
char *at_parse_get_text(void *scanner);
void *at_parse_get_extra(void *yyscanner);

#ifdef __cplusplus
}
#endif
#endif
