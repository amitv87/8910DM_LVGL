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

#ifndef _DIAG_H_
#define _DIAG_H_

#include "diag_config.h"
#include "drv_debug_port.h"
#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#define LOG_TAG_DIAG OSI_MAKE_LOG_TAG('D', 'I', 'A', 'G')

/**
 * @brief diag command header
 *
 * The diag command header is common. The data after the heade is
 * called diag command data. The diag command header and the followed
 * command data is called command packet. \p len is the total length
 * of diag packet.
 *
 * When pointer of \p diagMsgHead_t is used ad parameter, it can be safely
 * assumed that the pointer is 4 bytes aligned. The memory from the
 * beginning of command header with size of \p diagMsgHead_t.len is
 * accessible.
 */
typedef struct diagMsgHead
{
    uint32_t seq_num; ///< Message sequence number, used for flow control
    uint16_t len;     ///< The totoal size of the packet
    uint8_t type;     ///< Main command type
    uint8_t subtype;  ///< Sub command type
} diagMsgHead_t;

/**
 * @brief diag command handle function type
 *
 * @param cmd       the diag command packet
 * @param ctx       caller context
 * @return
 *      - true if the command is handled
 *      - false if the command isn't handled
 */
typedef bool (*diagCmdHandle_t)(const diagMsgHead_t *cmd, void *ctx);

/**
 * \brief diag device type
 */
typedef enum
{
    DIAG_DEVICE_UNKNOWN = 0, ///< unknown
    DIAG_DEVICE_UART,        ///< uart
    DIAG_DEVICE_USERIAL,     ///< usb serial
} diagDevType_t;

/**
 * @brief get diag command data from command header
 *
 * @param cmd       the diag command packet
 * @return      the diag command data
 */
static inline const void *diagCmdData(const diagMsgHead_t *cmd) { return cmd + 1; }

/**
 * @brief get diag command data size from command header
 *
 * @param cmd       the diag command packet
 * @return      the diag command data size
 */
static inline unsigned diagCmdDataSize(const diagMsgHead_t *cmd) { return cmd->len - sizeof(diagMsgHead_t); }

/**
 * \brief diag device name
 *
 * It may be affected by configuration and sysnv.
 *
 * \return diag device name
 */
unsigned diagDeviceName(void);

/**
 * @brief initialize diag module
 */
void diagInit(drvDebugPort_t *port);

/**
 * @brief register diag command handler by type
 *
 * @param type      main command type
 * @param handler   diag command handler
 * @param ctx       caller context
 * @return
 *      - true on success
 *      - false if \p type is invalid
 */
bool diagRegisterCmdHandle(uint8_t type, diagCmdHandle_t handler, void *ctx);

/**
 * @brief unregister diag command type handler
 *
 * @param type      main command type
 * @return
 *      - true on success
 *      - false if \p type is invalid
 */
bool diagUnregisterCmdHandle(uint8_t type);

/**
 * @brief output diag packet
 *
 * \p data should be a complete diag packet.
 *
 * @param data      diag packet pointer
 * @param size      diag packet size
 * @return
 *      - true on success
 *      - false if \p data is NULL or size is zero
 */
bool diagOutputPacket(const void *data, unsigned size);

/**
 * @brief output diag packet, header followed by data
 *
 * \p size is permitted to be zero. That means there are no data.
 *
 * This just to make diag output easier. When there are data following
 * command header, it is not needed to allocate memory for the whole
 * packet, and concate the header and data together.
 *
 * \p len in the header will be changed to \code sizeof(diagMsgHead_t) + size \endcode.
 *
 * @param cmd       diag command header, must be valid
 * @param data      diag data following header, must be valid when \p size if not zero
 * @param size      diag data size
 * @return
 *      - true on success
 *      - false if \p head is NULL
 */
bool diagOutputPacket2(const diagMsgHead_t *cmd, const void *data, unsigned size);

/**
 * @brief output diag packet, header followed by sub-header and data
 *
 * \p sub_header_size is permitted to be zero. That means there are no sub-header.
 *
 * \p size is permitted to be zero. That means there are no data.
 *
 * This just to make diag output easier. When there are sub-header and data
 * following command header, it is not needed to concate them together.
 *
 * \p len in the header will be changed to
 * \code sizeof(diagMsgHead_t) + sub_header_size + size \endcode.
 *
 * @param cmd       diag command header, must be valid
 * @param sub_header        sub-header, must be valid when \p sub_header_size if not zero
 * @param sub_header_size   sub-header size
 * @param size      diag data size
 * @param data      diag data following sub-header, must be valid when \p size if not zero
 * @param size      diag data size
 * @return
 *      - true on success
 *      - false if \p head is NULL
 */
bool diagOutputPacket3(const diagMsgHead_t *cmd, const void *sub_header, unsigned sub_header_size, const void *data, unsigned size);

/**
 * @brief output response for bad command
 *
 * @param cmd       diag command header, must be valid
 */
void diagBadCommand(const diagMsgHead_t *cmd);

OSI_EXTERN_C_END
#endif
