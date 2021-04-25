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

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_host_cmd.h"
#include "drv_host_cmd_imp.h"
#include "drv_config.h"
#include "osi_api.h"
#include "osi_trace.h"
#include "osi_log.h"
#include "osi_mem.h"
#include "osi_sysnv.h"
#include "osi_byte_buf.h"
#include "hal_hw_map.h"
#include "hal_adi_bus.h"
#include "hal_chip.h"
#include <string.h>
#include <stdlib.h>

#define HOST_CMD_RI8 (0x00 | 0x0 | 0x4 | 0x0)
#define HOST_CMD_R8 (0x00 | 0x0 | 0x0 | 0x0)
#define HOST_CMD_R16 (0x00 | 0x0 | 0x0 | 0x1)
#define HOST_CMD_R32 (0x00 | 0x0 | 0x0 | 0x2)
#define HOST_CMD_RBLK (0x00 | 0x0 | 0x0 | 0x3)         // SW only
#define HOST_CMD_SPECIAL_R32 (0x00 | 0x8 | 0x0 | 0x2)  // SW only
#define HOST_CMD_SPECIAL_RBLK (0x00 | 0x8 | 0x0 | 0x3) // SW only
#define HOST_CMD_WI8 (0x80 | 0x0 | 0x4 | 0x0)
#define HOST_CMD_W8 (0x80 | 0x0 | 0x0 | 0x0)
#define HOST_CMD_W16 (0x80 | 0x0 | 0x0 | 0x1)
#define HOST_CMD_W32 (0x80 | 0x0 | 0x0 | 0x2)
#define HOST_CMD_WBLK (0x80 | 0x0 | 0x0 | 0x3)
#define HOST_CMD_SPECIAL_W32 (0x80 | 0x8 | 0x0 | 0x2)  // SW only
#define HOST_CMD_SPECIAL_WBLK (0x80 | 0x8 | 0x0 | 0x3) // SW only

static void prvHostRead(drvHostCmdEngine_t *cmd, uint8_t *result, uint8_t flowid, uint8_t rid, uint32_t address, uint16_t size)
{
    OSI_LOGD(0, "host: read/%p size/%d", (void *)address, size);

    // block size align down to 1KB
    int block_size = OSI_ALIGN_DOWN(CONFIG_HOST_CMD_ENGINE_MAX_PACKET_SIZE, 1024);
    while (size > 0)
    {
        int trans = OSI_MIN(uint16_t, size, block_size);
        int frame_len = trans + 2; // rid, crc
        int packet_len = sizeof(osiHostPacketHeader_t) + frame_len;
        osiFillHostHeader((osiHostPacketHeader_t *)result, flowid, frame_len);

        // It is needed to validate the address. At invalid address, fill "0xEE"
        if (halHwMapValid(address, trans))
            memcpy(&result[5], (void *)address, trans);
        else
            memset(&result[5], 0xee, trans);

        result[4] = rid;
        drvHostCmdSendResponse(cmd, result, packet_len);

        address += trans;
        size -= trans;
    }
}

static bool prvHostAdiRead(drvHostCmdEngine_t *cmd, uint8_t *result, uint8_t flowid, uint8_t rid, uint32_t address, uint16_t size)
{
#ifdef CONFIG_SOC_8910
    if (!HAL_ADDR_IS_ADI(address) ||
        !halHwMapValid(address, size) ||
        !OSI_IS_ALIGNED(address, 4) ||
        !OSI_IS_ALIGNED(size, 4))
        return false;

    OSI_LOGD(0, "host: read adi/%p size/%d", (void *)address, size);

    int frame_len = size + 2; // rid, crc
    int packet_len = sizeof(osiHostPacketHeader_t) + frame_len;
    osiFillHostHeader((osiHostPacketHeader_t *)result, flowid, frame_len);

    for (int n = 0; n < size; n += 4)
        osiBytesPutLe32(&result[n + 5], halAdiBusRead((volatile uint32_t *)(address + n)));

    result[4] = rid;
    drvHostCmdSendResponse(cmd, result, packet_len);
    return true;
#else
    return false;
#endif
}

static void prvHostReadSpecial(drvHostCmdEngine_t *cmd, uint8_t *result, uint8_t flowid, uint8_t rid, uint32_t address, uint16_t size)
{
    if (prvHostAdiRead(cmd, result, flowid, rid, address, size))
        return;

    // fall through
    prvHostRead(cmd, result, flowid, rid, address, size);
}

static void prvHostWrite8(uint32_t address, const void *buf, uint16_t size)
{
    if (!halHwMapValid(address, size)) // avoid invalid address size
        return;

    const uint8_t *ps = (const uint8_t *)buf;
    uint8_t *pd = (uint8_t *)address;
    for (int n = 0; n < size; n++)
        *pd++ = *ps++;
}

static void prvHostWrite16(uint32_t address, const void *buf, uint16_t size)
{
    // make address and size aligned silently
    address = OSI_ALIGN_DOWN(address, 2);
    size = OSI_ALIGN_DOWN(address, 2);
    if (!halHwMapValid(address, size)) // avoid invalid address size
        return;

    const uint8_t *ps = (const uint8_t *)buf;
    uint16_t *pd = (uint16_t *)address;
    for (int n = 0; n < size; n += 2, ps += 2)
        *pd++ = osiBytesGetLe16(ps);
}

static void prvHostWrite32(uint32_t address, const void *buf, uint16_t size)
{
    // make address and size aligned silently
    address = OSI_ALIGN_DOWN(address, 4);
    size = OSI_ALIGN_DOWN(address, 4);
    if (!halHwMapValid(address, size)) // avoid invalid address size
        return;

    const uint8_t *ps = (const uint8_t *)buf;
    uint32_t *pd = (uint32_t *)address;
    for (int n = 0; n < size; n += 4, ps += 4)
        *pd++ = osiBytesGetLe32(ps);
}

static bool prvHostAdiWrite(uint32_t address, const void *buf, uint16_t size)
{
#ifdef CONFIG_SOC_8910
    if (!HAL_ADDR_IS_ADI(address) ||
        !halHwMapValid(address, size) ||
        !OSI_IS_ALIGNED(address, 4) ||
        !OSI_IS_ALIGNED(size, 4))
        return false;

    const uint8_t *ps = (const uint8_t *)buf;
    for (int n = 0; n < size; n += 4, address += 4, ps += 4)
        halAdiBusWrite((volatile uint32_t *)address, osiBytesGetLe32(ps));
    return true;
#else
    return false;
#endif
}

static void prvHostWriteSpecial(uint32_t address, const void *buf, uint16_t size)
{
    if (prvHostAdiWrite(address, buf, size))
        return;

    // fall through
    prvHostWrite32(address, buf, size);
}

void drvHostReadWriteHandler(drvHostCmdEngine_t *cmd, uint8_t *packet, unsigned packet_len)
{
#if 0
Read 1/2/4
+------------+-----------+-----+-------------+-----+-----+
| header (4) | 0xfe/0xff | cmd | address (4) | rid | crc |
+------------+-----------+-----+-------------+-----+-----+
Read block
+------------+-----------+-----+-------------+-----+---------+-----+
| header (4) | 0xfe/0xff | cmd | address (4) | rid | size(2) | crc |
+------------+-----------+-----+-------------+-----+---------+-----+
    Read output
+------------+-----------+-----+----------+-----+
| header (4) | 0xfe/0xff | rid | data (n) | crc |
+------------+-----------+-----+----------+-----+
Write:
+------------+-----------+-----+-------------+---------+-----+
| header (4) | 0xfe/0xff | cmd | address (4) | data(n) | crc |
+------------+-----------+-----+-------------+---------+-----+
#endif

    uint8_t flowid = packet[3];
    uint8_t cmd_code = packet[4];
    const uint8_t *p = packet + sizeof(osiHostPacketHeader_t);

    if ((cmd_code & 0x80) == 0x00) // read command
    {
        uint32_t address = osiBytesGetLe32(p + 1);
        uint8_t rid = p[5];

        uint16_t read_size = osiBytesGetLe16(p + 6);

#ifdef CONFIG_CPU_MIPS
        if (!OSI_IS_KSEG0(address) && !OSI_IS_KSEG1(address))
            address = OSI_KSEG1(address);
#endif

        uint8_t dummy = 0xee;
        if (cmd_code == HOST_CMD_RI8)
            prvHostRead(cmd, (uint8_t *)packet, flowid, rid, (uint32_t)&dummy, 1); // return dummy
        else if (cmd_code == HOST_CMD_R8)
            prvHostRead(cmd, (uint8_t *)packet, flowid, rid, address, 1);
        else if (cmd_code == HOST_CMD_R16)
            prvHostRead(cmd, (uint8_t *)packet, flowid, rid, address, 2);
        else if (cmd_code == HOST_CMD_R32)
            prvHostRead(cmd, (uint8_t *)packet, flowid, rid, address, 4);
        else if (cmd_code == HOST_CMD_RBLK)
            prvHostRead(cmd, (uint8_t *)packet, flowid, rid, address, read_size);
        else if (cmd_code == HOST_CMD_SPECIAL_R32)
            prvHostReadSpecial(cmd, (uint8_t *)packet, flowid, rid, address, 4);
        else if (cmd_code == HOST_CMD_SPECIAL_RBLK)
            prvHostReadSpecial(cmd, (uint8_t *)packet, flowid, rid, address, read_size);
        else
            drvHostInvalidCmd(cmd, packet, packet_len);
    }
    else
    {
        uint32_t address = osiBytesGetLe32(p + 1);
        const void *data = &p[5];
        uint16_t write_size = packet_len - (4 + 1 + 4 + 1);

#ifdef CONFIG_CPU_MIPS
        if (!OSI_IS_KSEG0(address) && !OSI_IS_KSEG1(address))
            address = OSI_KSEG1(address);
#endif

        if (cmd_code == HOST_CMD_WI8)
            ; // ignore write internal register
        else if (cmd_code == HOST_CMD_W8)
            prvHostWrite8(address, data, 1);
        else if (cmd_code == HOST_CMD_W16)
            prvHostWrite16(address, data, 2);
        else if (cmd_code == HOST_CMD_W32)
            prvHostWrite32(address, data, 4);
        else if (cmd_code == HOST_CMD_WBLK)
            prvHostWrite32(address, data, write_size);
        else if (cmd_code == HOST_CMD_SPECIAL_W32)
            prvHostWriteSpecial(address, data, 4);
        else if (cmd_code == HOST_CMD_SPECIAL_WBLK)
            prvHostWriteSpecial(address, data, write_size);
        else
            drvHostInvalidCmd(cmd, packet, packet_len);
    }
}
