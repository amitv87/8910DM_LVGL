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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "diag_config.h"
#include "diag.h"
#include "osi_log.h"
#include "osi_api.h"
#include "cmddef.h"
#include "drv_efuse.h"
#include "drv_secure.h"

#define RDA_EFUSE_UNIQUE_ID_LOW_INDEX (24)
#define RDA_EFUSE_UNIQUE_ID_HIGH_INDEX (26)

static int _bcd2Ascii(uint8_t hex)
{
    if (hex >= 0 && hex <= 9)
        return (hex + '0');
    if (hex >= 0xa && hex <= 0xf)
        return (hex + 'A' - 10);
    return '0';
}

static void _uid2Ascii(uint8_t hex, uint8_t *ascii)
{
    uint8_t low_hex = hex & 0xf;
    uint8_t high_hex = (hex >> 4) & 0xf;
    ascii[0] = _bcd2Ascii(high_hex);
    ascii[1] = _bcd2Ascii(low_hex);
}

static void _handleReadUIDReq(const diagMsgHead_t *cmd)
{
    diagMsgHead_t head = *cmd;
    uint32_t id_low, id_high, length;
    uint8_t uData[8] = {0};
    uint8_t uDataAscii[32] = {0};

    bool result = drvEfuseBatchRead(true,
                                    RDA_EFUSE_DOUBLE_BLOCK_UNIQUE_ID_LOW_INDEX, &id_low,
                                    RDA_EFUSE_DOUBLE_BLOCK_UNIQUE_ID_HIGH_INDEX, &id_high,
                                    DRV_EFUSE_ACCESS_END);

    if (result)
    {
        length = 8;
        uint8_t *uid = uData;
        *uid++ = (id_low & 0xff000000) >> 24;
        *uid++ = (id_low & 0x00ff0000) >> 16;
        *uid++ = (id_low & 0x0000ff00) >> 8;
        *uid++ = (id_low & 0x000000ff) >> 0;

        *uid++ = (id_high & 0xff000000) >> 24;
        *uid++ = (id_high & 0x00ff0000) >> 16;
        *uid++ = (id_high & 0x0000ff00) >> 8;
        *uid++ = (id_high & 0x000000ff) >> 0;

        int j = 0;
        for (int i = 0; i < length; i++)
        {
            _uid2Ascii(uData[i], &uDataAscii[j]);
            j += 2;
        }
        length = j;
        head.len = length + sizeof(diagMsgHead_t);
        diagOutputPacket2(&head, uDataAscii, length);

        OSI_LOGD(0, "EFUSE Read id_low = 0x%08x", id_low);
        OSI_LOGD(0, "EFUSE Read id_high = 0x%08x", id_high);
    }
    else
    {
        uint8_t data = 0;
        length = 1;
        head.len = length + sizeof(diagMsgHead_t);
        diagOutputPacket2(&head, &data, length);
        OSI_LOGD(0, "EFUSE Read UID fail...");
    }
}

static void _handleWriteSecurityFlag(const diagMsgHead_t *cmd)
{
    diagMsgHead_t head = *cmd;
    uint8_t ret;

    // return 1 when success
    if (drvSecureWriteSecureFlags())
    {
        ret = 1;
        OSI_LOGV(0, "EFUSE wirte security flag ok...");
    }
    else
    {
        ret = 0;
        OSI_LOGD(0, "EFUSE wirte security flag fail...");
    }
    head.len = 1 + sizeof(diagMsgHead_t);
    diagOutputPacket2(&head, &ret, 1);
    osiShutdown(OSI_SHUTDOWN_RESET);
}

static void _handleReadSecurityFlag(const diagMsgHead_t *cmd)
{
    diagMsgHead_t head = *cmd;
    uint8_t ret;
    // return 1 when enable secureboot
    if (drvSecureBootEnable())
        ret = 1;
    else
        ret = 0;
    head.len = 1 + sizeof(diagMsgHead_t);
    diagOutputPacket2(&head, &ret, 1);
}

static void _handleWriteEfuseInfo(const diagMsgHead_t *cmd)
{
    diagMsgHead_t head = *cmd;
    uint8_t ret;
    ret = 1;
    head.len = 1 + sizeof(diagMsgHead_t);
    diagOutputPacket2(&head, &ret, 1);
}

static bool _handleSysInfo(const diagMsgHead_t *cmd, void *ctx)
{
    switch (cmd->subtype)
    {
    case DIAG_READ_UID_V2_F:
        _handleReadUIDReq(cmd);
        break;

    case DIAG_WRITE_EFUSE_INFO_F:
        _handleWriteEfuseInfo(cmd);
        break;

    case DIAG_WRITE_SECURITY_FLAG:
        _handleWriteSecurityFlag(cmd);
        break;

    case DIAG_READ_SECURITY_FLAG:
        _handleReadSecurityFlag(cmd);
        break;

    default:
        diagOutputPacket2(cmd, NULL, 0);
        break;
    }
    return true;
}

void diagSysInit(void)
{
    diagRegisterCmdHandle(DIAG_SYSTEM_F, _handleSysInfo, NULL);
}
