/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _BOOT_PDL_H_
#define _BOOT_PDL_H_

#include "osi_compiler.h"
#include "boot_fdl_channel.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief opaque data struct for pdl engine
 *
 * PDL protocol is only used in 8910 ROM now. and 8910 bootloader
 * implements a minimal feature set to mimic ROM. It won't be used later,
 * and many irrelevent features are ignored.
 */
typedef struct pdlEngine pdlEngine_t;

/**
 * \brief pdl command header
 *
 * The is common to all commands. That is, the first word of command is
 * the command type.
 */
typedef struct
{
    uint32_t cmd_type;   ///< command type
    uint32_t payload[0]; ///< command dependent data
} pdlCommand_t;

/**
 * \brief pdl data command header
 */
typedef struct
{
    uint32_t cmd_type; ///< command type
    uint32_t address;  ///< data address
    uint32_t size;     ///< size, different meaning on varioud command types
    uint32_t data[0];  ///< command dependent data
} pdlDataCommand_t;

typedef enum
{
    PDL_CMD_CONNECT = 0,
    PDL_CMD_START_DATA = 4,
    PDL_CMD_MID_DATA,
    PDL_CMD_END_DATA,
    PDL_CMD_EXEC_DATA,
} pdlCommandType_t;

typedef enum
{
    PDL_RESP_ACK = 0,
    PDL_RESP_PACKET_ERROR,
    PDL_RESP_INVALID_CMD,
    PDL_RESP_UNKNOWN_CMD,
    PDL_RESP_INVALID_ADDR,
    PDL_RESP_INVALID_BAUDRATE,
    PDL_RESP_INVALID_PARTITION,
    PDL_RESP_INVALID_SIZE,
    PDL_RESP_WAIT_TIMEOUT,
    PDL_RESP_VERIFY_ERROR,
    PDL_RESP_CHECKSUM_ERROR,
    PDL_RESP_OPERATION_FAILED,
    PDL_RESP_DEVICE_ERROR,
} pdlRespType_t;

/**
 * \brief pdl command process callback
 *
 * \param pdl the pdl engine
 * \param param callback context
 * \param cmd pdl command
 * \param size total size of command, including command header
 */
typedef void (*pdlCmdProcess_t)(pdlEngine_t *pdl, void *param, pdlCommand_t *cmd, unsigned size);

/**
 * \brief pdl polling callback
 *
 * \param pdl the pdl engine
 * \param param callback context
 * \return true for continnue, false for quit
 */
typedef bool (*pdlPolling_t)(pdlEngine_t *pdl, void *param);

/**
 * \brief create pdl engine
 *
 * \param ch uart or usb channel
 * \param max_len maximum payload len
 * \return
 *      - the create pdl engine
 *      - NULL on failed, out of memory
 */
pdlEngine_t *pdlEngineCreate(fdlChannel_t *ch, unsigned max_len);

/**
 * \brief delete the pdl engine
 *
 * \param d pdl engine
 */
void pdlEngineDelete(pdlEngine_t *d);

/**
 * \brief start pdl engine loop
 *
 * \param d pdl engine
 * \param proc command process callback
 * \param polling polling callback
 * \param param callback context
 * \return false on failed, never return true
 */
bool pdlEngineLoop(pdlEngine_t *d, pdlCmdProcess_t proc, pdlPolling_t polling, void *param);

/**
 * \brief send response without data
 *
 * \param d pdl engine
 * \param resp response code
 */
void pdlEngineSendResp(pdlEngine_t *d, pdlRespType_t resp);

/**
 * \brief download uimage with pdl protocol
 *
 * \param ch uart or usb channel
 * \param callparam parameter at jump to uimage
 * \param start starting address of reserved region
 * \param size size of reserved region
 * \param timeout timeout for connection, 0 for wait forever
 * \return
 *      - true for timeout
 *      - false on error
 */
bool pdlDnldUimage(fdlChannel_t *ch, unsigned callparam, unsigned start, unsigned size, unsigned timeout);

OSI_EXTERN_C_END
#endif
