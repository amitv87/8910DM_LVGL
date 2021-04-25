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

#include "atr_config.h"
#include "cfw_config.h"
#include "osi_log.h"
#include "at_parse.h"
#include <stdlib.h>
#include <string.h>

typedef struct
{
    atCommandHead_t cmd_list;
    atCommand_t *curr_cmd;
} atParseCtx_t;

enum
{
    AT_PARSE_ERR_UNKNOWN_CMD = 2000,
    AT_PARSE_ERR_NOMEM,
    AT_PARSE_ERR_SYNTAX,
    AT_PARSE_ERR_NO_CMD,
    AT_PARSE_ERR_TOO_MANY_PARAM
};

void atCommandDestroy(atCommand_t *cmd)
{
    if (cmd == NULL)
        return;
    for (int n = 0; n < cmd->param_count; n++)
        free(cmd->params[n]);
    cmd->param_count = 0;
    free(cmd);
}

void atCommandDestroyAll(atCommandHead_t *cmd_list)
{
    while (!SLIST_EMPTY(cmd_list))
    {
        atCommand_t *cmd = SLIST_FIRST(cmd_list);
        SLIST_REMOVE_HEAD(cmd_list, iter);
        atCommandDestroy(cmd);
    }
}

void atCommandAsyncCtxFree(atCommand_t *cmd)
{
    free(cmd->async_ctx);
    cmd->async_ctx = NULL;
}

// =============================================================================
// at_parse_alloc: malloc used in flex/bison
// =============================================================================
void *at_parse_alloc(unsigned size, void *scanner)
{
    void *ptr = malloc(size);
    OSI_LOGD(0x1000527d, "at_parse_alloc %d %p", size, ptr);
    return ptr;
}

// =============================================================================
// at_parse_realloc: realloc used in flex/bison
// =============================================================================
void *at_parse_realloc(void *ptr, unsigned size, void *scanner)
{
    void *nptr = realloc(ptr, size);
    OSI_LOGD(0x1000527e, "at_parse_realloc %d %p %p", size, ptr, nptr);
    return nptr;
}

// =============================================================================
// at_parse_free: free used in flex/bison
// =============================================================================
void at_parse_free(void *ptr, void *scanner)
{
    OSI_LOGD(0x1000527f, "at_parse_free %p", ptr);
    free(ptr);
}

// =============================================================================
// atParseStartCmdText: start parsing a command
// =============================================================================
static int atParseStartCmdText(atParseCtx_t *ctx, char *text, unsigned length)
{
    OSI_LOGXV(OSI_LOGPAR_IS, 0x1000009c, "AT CMD parse start cmd len=%d: %s", length, text);
    const atCmdDesc_t *desc = atCommandSearchDesc(text, length);
    if (desc == NULL)
        return AT_PARSE_ERR_UNKNOWN_CMD;

    atCommand_t *cmd = (atCommand_t *)calloc(1, sizeof(*cmd));
    if (cmd == NULL)
        return AT_PARSE_ERR_NOMEM;

    cmd->desc = desc;
    cmd->engine = NULL;
    cmd->type = 0;
    cmd->param_count = 0;
    cmd->async_ctx = NULL;
    cmd->async_ctx_destroy = NULL;
    cmd->uti = AT_CFW_UTI_INVALID;
    memset(cmd->params, 0, sizeof(cmd->params));
    ctx->curr_cmd = cmd;

    SLIST_INSERT_HEAD(&ctx->cmd_list, cmd, iter);
    return 0;
}

// =============================================================================
// atParseStartCmd
// =============================================================================
int atParseStartCmd(void *scanner)
{
    atParseCtx_t *ctx = (atParseCtx_t *)at_parse_get_extra(scanner);
    return atParseStartCmdText(ctx, at_parse_get_text(scanner), at_parse_get_leng(scanner));
}

// =============================================================================
// atParseStartCmdNone: for "AT\r"
// =============================================================================
int atParseStartCmdNone(void *scanner)
{
    atParseCtx_t *ctx = (atParseCtx_t *)at_parse_get_extra(scanner);
    return atParseStartCmdText(ctx, "", 0);
}

// =============================================================================
// atParseSetCmdType
// =============================================================================
int atParseSetCmdType(void *scanner, uint8_t type)
{
    OSI_LOGV(0x1000009d, "AT CMD parse set cmd type=%d", type);
    atParseCtx_t *ctx = (atParseCtx_t *)at_parse_get_extra(scanner);
    if (ctx->curr_cmd == NULL)
        return AT_PARSE_ERR_NO_CMD;
    ctx->curr_cmd->type = type;
    return 0;
}

// =============================================================================
// atParsePushParamText
// =============================================================================
static int atParsePushParamText(atParseCtx_t *ctx, uint8_t type, char *text, unsigned leng)
{
    OSI_LOGXV(OSI_LOGPAR_IIS, 0x1000009e, "AT CMD parse push param type=%d len=%d: %s", type, leng, text);
    if (ctx->curr_cmd == NULL)
        return AT_PARSE_ERR_NO_CMD;
    if (ctx->curr_cmd->param_count >= CONFIG_ATR_CMD_PARAM_MAX)
        return AT_PARSE_ERR_TOO_MANY_PARAM;

    // HACK: the last ';' can be handled, so remove it here
    if (text[leng - 1] == ';')
        leng--;

    atCmdParam_t *param = (atCmdParam_t *)malloc(sizeof(*param) + leng);
    param->type = type;
    param->length = leng;
    memcpy(param->value, text, leng);
    param->value[leng] = 0;
    ctx->curr_cmd->params[ctx->curr_cmd->param_count++] = param;
    return 0;
}

// =============================================================================
// atParsePushRawText
// =============================================================================
int atParsePushRawText(void *scanner)
{
    atParseCtx_t *ctx = (atParseCtx_t *)at_parse_get_extra(scanner);
    return atParsePushParamText(ctx, AT_CMD_PARAM_TYPE_RAW,
                                at_parse_get_text(scanner), at_parse_get_leng(scanner));
}

// =============================================================================
// atParsePushEmpty
// =============================================================================
int atParsePushEmpty(void *scanner)
{
    atParseCtx_t *ctx = (atParseCtx_t *)at_parse_get_extra(scanner);
    return atParsePushParamText(ctx, AT_CMD_PARAM_TYPE_EMPTY, "", 0);
}

// =============================================================================
// at_ParseLine
// =============================================================================
int atParseLine(const char *cmdline, unsigned length, atCommandHead_t *cmd_list)
{
    int result = 0;
    void *scanner = NULL;
    atParseCtx_t ctx;
    atCommandHead_t rlist;
    atCommand_t *cmd;

    SLIST_INIT(&ctx.cmd_list);
    ctx.curr_cmd = NULL;

    if (at_parse_lex_init_extra(&ctx, &scanner) != 0)
    {
        result = AT_PARSE_ERR_NOMEM;
        goto destroy_exit;
    }

    if (at_parse__scan_bytes(cmdline, length, scanner) == NULL)
    {
        result = AT_PARSE_ERR_NOMEM;
        goto destroy_exit;
    }

    if (at_parse_parse(scanner) != 0)
    {
        result = AT_PARSE_ERR_SYNTAX;
        goto destroy_exit;
    }

destroy_exit:
    // cmds will always returned. Caller shall free them even on error.
    SLIST_INIT(&rlist);
    while ((cmd = SLIST_FIRST(&ctx.cmd_list)) != NULL)
    {
        SLIST_REMOVE_HEAD(&ctx.cmd_list, iter);
        SLIST_INSERT_HEAD(&rlist, cmd, iter);
    }
    SLIST_CONCAT(cmd_list, &rlist, atCommand, iter);

    at_parse_lex_destroy(scanner);
    return result;
}

#define DECLARE_CMD_HANDLER(h) extern void h(atCommand_t *)
#include "at_cmd_table.h"

size_t atCommandDescCount(void)
{
    return TOTAL_KEYWORDS;
}

const atCmdDesc_t *atCommandDescByIndex(size_t n)
{
    if (n >= TOTAL_KEYWORDS)
        return NULL;
    return &gAtCmdTable[n];
}

static const atCmdDesc_t gAtHandleEmpty = {
    .name = "",
    .handler = atCmdHandleAT,
};

const atCmdDesc_t *atCommandSearchDesc(const char *name, size_t length)
{
    OSI_LOGXD(OSI_LOGPAR_SI, 0x100000e2, "AT CMD search %s (%d)", name, length);

    if (length == 0)
        return &gAtHandleEmpty;

    if (length <= MAX_WORD_LENGTH && length >= MIN_WORD_LENGTH)
    {
        unsigned key = atCmdNameHash(name, length);
        if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
            unsigned idx = gAtCmdNameHashIndex[key];
            if (idx < TOTAL_KEYWORDS)
            {
                int cmp = strncasecmp(name, gAtCmdTable[idx].name, length);
                OSI_LOGD(0x100000e3, "AT CMD found key=%d index=%d cmp=%d", key, idx, cmp);
                return (cmp == 0) ? &gAtCmdTable[idx] : NULL;
            }
        }
    }
    return NULL;
}
