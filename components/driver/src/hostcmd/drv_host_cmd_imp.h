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

#ifndef _DRV_HOST_CMD_IMP_H_
#define _DRV_HOST_CMD_IMP_H_

#include "osi_api.h"

OSI_EXTERN_C_BEGIN

enum
{
    HOST_FLOWID_SYSCMD = 0xfd,
    HOST_FLOWID_READWRITE2 = 0xfe,
    HOST_FLOWID_READWRITE = 0xff,
};

enum
{
    HOST_SYSCMD_PING = 0x01,
    HOST_SYSCMD_PANIC = 0x02,
    HOST_SYSCMD_SHUTDOWN = 0x03,
    HOST_SYSCMD_READ_SYSNV = 0x04,
    HOST_SYSCMD_WRITE_SYSNV = 0x05,
    HOST_SYSCMD_SAVE_CURR_SYSNV = 0x06,
    HOST_SYSCMD_CLEAR_SYSNV = 0x07,
    HOST_SYSCMD_HEAPINFO = 0x08,
    HOST_SYSCMD_TIMERINFO = 0x09,
    HOST_SYSCMD_PMSOURCEINFO = 0x0a,
    HOST_SYSCMD_CLOCKINFO = 0x0b,
    HOST_SYSCMD_FILEINFO = 0x0c,
    HOST_SYSCMD_READFILE = 0x0d,
    HOST_SYSCMD_WRITEFILE = 0x0e,
    HOST_SYSCMD_DELFILE = 0x0f,
    HOST_SYSCMD_LISTDIR = 0x10,
    HOST_SYSCMD_MKDIR = 0x11,
    HOST_SYSCMD_RMDIR = 0x12,
    HOST_SYSCMD_MKPATH = 0x13,
    HOST_SYSCMD_RMPATH = 0x14,
    HOST_SYSCMD_RENAME = 0x15,
    HOST_SYSCMD_APPIMG_FLASH_GET = 0x16,
    HOST_SYSCMD_APPIMG_FLASH_SET = 0x17,
    HOST_SYSCMD_APPIMG_FILE_GET = 0x18,
    HOST_SYSCMD_APPIMG_FILE_SET = 0x19,
    HOST_SYSCMD_NBTIMERINFO = 0x1a,
    HOST_SYSCMD_SXTIMERINFO = 0x1b,
    HOST_SYSCMD_LISTPARTION = 0x1c,
    HOST_SYSCMD_PROFILEMODE = 0x1d,
    HOST_SYSCMD_INVALID = 0xff,
};

/**
 * \brief host command handler function type
 *
 * In command handling, the memory of \p packet can be reused. It will
 * simplify memory management of command handling, and in most cases,
 * there are no extra memory needed. It is useful, especially in panic
 * mode.
 *
 * The size of \p packet is \p CONFIG_HOST_CMD_ENGINE_MAX_PACKET_SIZE.
 *
 * The host packet CRC is already checked, and it is not needed to check
 * it in command handler.
 *
 * \param cmd       the host command engine
 * \param packet    host command packet
 * \param packet_len    host command packet length
 */
typedef void (*drvHostCmdHandler_t)(drvHostCmdEngine_t *cmd, uint8_t *packet, unsigned packet_len);

/**
 * \brief set host command handler for specified flow id
 *
 * When \p handler is NULL, the specified flow id won't be handled.
 * And the unknown command response will be sent.
 *
 * \param cmd       the host command engine
 * \param flow_id   flow id
 * \param handler   host command handler for the specified flow id
 */
void drvHostCmdRegisterHander(drvHostCmdEngine_t *cmd, uint8_t flow_id, drvHostCmdHandler_t handler);

/**
 * \brief send out response packet
 *
 * It will be called by host command handler to send out response packet.
 * Inside, the registered \p sender will be called.
 *
 * Inside, the host packet frame length and CRC will be calculated and filled.
 * Host command handler is not needed to fill packet CRC.
 *
 * \param cmd       the host command engine
 * \param packet    reponse packet
 * \param packet_len    reponse packet length
 */
void drvHostCmdSendResponse(drvHostCmdEngine_t *cmd, uint8_t *packet, unsigned packet_len);

/**
 * \brief send out response packet with error code
 *
 * The first 5 bytes will be kept, and put 2 bytes error code.
 *
 * \param cmd       the host command engine
 * \param packet    reponse packet
 */
void drvHostCmdSendResultCode(drvHostCmdEngine_t *cmd, uint8_t *packet, uint16_t code);

/**
 * \brief host command handler for invalid command packet
 *
 * This is the default command handler for unknown or invalid command
 * packet.
 *
 * \param cmd       the host command engine
 * \param packet    host command packet
 * \param packet_len    host command packet length
 */
void drvHostInvalidCmd(drvHostCmdEngine_t *cmd, uint8_t *packet, unsigned packet_len);

/**
 * \brief read/write host command handler
 *
 * The command is compatible with debughost hardware read/write command,
 * with extensions:
 * - block read
 * - special address read/write, such ADI registers
 *
 * \param cmd       the host command engine
 * \param packet    host command packet
 * \param packet_len    host command packet length
 */
void drvHostReadWriteHandler(drvHostCmdEngine_t *cmd, uint8_t *packet, unsigned packet_len);

/**
 * \brief system command handler
 *
 * \param cmd       the host command engine
 * \param packet    host command packet
 * \param packet_len    host command packet length
 */
void drvHostSyscmdHandler(drvHostCmdEngine_t *cmd, uint8_t *packet, unsigned packet_len);

OSI_EXTERN_C_END
#endif
