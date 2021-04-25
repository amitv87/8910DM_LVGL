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

#ifndef _AT_COMMAND_H_
#define _AT_COMMAND_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/queue.h>
#include "atr_config.h"
#include "at_param.h"
#include "osi_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

struct atCommand;
struct atCmdEngine;
struct osiEvent;

#define AT_CMD_RETURN(r) OSI_DO_WHILE0(r; return;)
#define AT_CFW_UTI_INVALID 0xff

/**
 * \brief AT command type
 *
 * set: AT+CMD=1
 * exe: AT+CMD
 * read: AT+CMD?
 * test: AT+CMD=?
 */
typedef enum
{
    AT_CMD_SET,
    AT_CMD_TEST,
    AT_CMD_READ,
    AT_CMD_EXE
} atCmdType_t;

/**
 * \brief AT command constrain
 *
 * Some AT commands are valid only when certain conditions are meet.
 * Each required condition is defined as a constrain.
 *
 * For each AT command, zero or more contrains can be declared. Only
 * when all contrains are meet, \p cmd->desc->handler will be called.
 * The constrain check is performed inside \p atCommandCheckConstrains.
 */
enum
{
    AT_CONSTRAINT_NONE = 0,           ///< no constrain
    AT_CON_CALIB_MODE = (1 << 0),     ///< only in calibration mode, not in normal mode
    AT_CON_NOT_CALIB_MODE = (1 << 1), ///< only in normal mode, not in calibration mode
    AT_CON_NOT_CLAC = (1 << 2),       ///< ignore in CLAC
};

/**
 * \brief AT command description
 */
typedef struct atCmdDesc
{
    const char *name;                    ///< AT command name, such as "+IPR"
    void (*handler)(struct atCommand *); ///< At command handler
    uint32_t constrains;                 ///< AT command constrains
} atCmdDesc_t;

/**
 * \brief function prototype to delete async context
 */
typedef void (*atCommandAsyncCtxDestroy_t)(struct atCommand *cmd);

typedef SLIST_ENTRY(atCommand) atCommandIter_t;
typedef SLIST_HEAD(atCommandHead, atCommand) atCommandHead_t;

/**
 * \brief AT command context data struct
 *
 * AT engine will parse a whole command line, and put all commands in the
 * command line into a list. So, all commands are located on a list.
 *
 * Each AT command is bind to an AT command engine, which receives the
 * AT command.
 *
 * AT command can be executed asynchronously. For example, an event will be
 * sent to another thread, and then return. After a response event is
 * received, the command will be finished. \p async_ctx is a placeholder
 * for asynchronous context. \p async_ctx_destroy will be called to delete
 * \p async_ctx by AT command engine. It is not needed to delete \p async_ctx
 * by command handler explicitly.
 */
typedef struct atCommand
{
    atCommandIter_t iter;
    const atCmdDesc_t *desc;
    struct atCmdEngine *engine;
    atCmdType_t type;
    uint8_t param_count;
    atCmdParam_t *params[CONFIG_ATR_CMD_PARAM_MAX];
    uint16_t uti;
    void *async_ctx;
    atCommandAsyncCtxDestroy_t async_ctx_destroy;
} atCommand_t;

/**
 * \brief AT command description count
 *
 * AT command decriptions are stored in a global array. This will return
 * the count.
 *
 * \return      AT command description count
 */
size_t atCommandDescCount(void);

/**
 * \brief AT command description by index
 *
 * The index is the index for internal global AT command description
 * array.
 *
 * \param n     AT command decription index
 * \return
 *      - AT command description
 *      - NULL is \p n is invalid
 */
const atCmdDesc_t *atCommandDescByIndex(size_t n);

/**
 * \brief search AT command description by name
 *
 * The search is case insensitive.
 *
 * The terminating \0 is not needed in \p name, however \p length parameter
 * is required. It is designed to permit sub-string parameter without string
 * copy.
 *
 * \param name      AT command name
 * \param length    AT command name length
 * \return
 *      - AT command description
 *      - NULL is \p n is invalid
 */
const atCmdDesc_t *atCommandSearchDesc(const char *name, size_t length);

/**
 * \brief check AT command constrains
 *
 * It is called by AT command engine. Only when it returns true,
 * \p cmd->desc->handler will be called. So, when it returns false, final
 * response should be called inside.
 *
 * \param cmd       AT command, must be valid
 * \return
 *      - true if the command is permitted
 *      - false if the command is not permitted, and final response will be
 *        called inside.
 */
bool atCommandCheckConstrains(atCommand_t *cmd);

/**
 * \brief delete an AT command
 *
 * It should be called inside AT command engine only.
 *
 * When \p cmd is NULL, nothing will be done.
 *
 * \param cmd       AT command, must be valid
 */
void atCommandDestroy(atCommand_t *cmd);

/**
 * \brief delete all AT commands in the command line
 *
 * It should be called inside AT command engine only.
 *
 * \p cmd_list itself must be a valid pointer for list head.
 *
 * \param cmd_list  AT command list
 */
void atCommandDestroyAll(atCommandHead_t *cmd_list);

/**
 * \brief default dtor of AT command async context
 *
 * When \p cmd->asynx_ctx is a plain memory pointer, this can be set to
 * \p cmd->async_ctx_destroy.
 *
 * \param cmd       AT command, must be valid
 */
void atCommandAsyncCtxFree(atCommand_t *cmd);

#ifdef __cplusplus
}
#endif
#endif
