/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "boot_fdl.h"
#include "boot_fdl_channel.h"
#include "boot_platform.h"
#include "boot_bsl_cmd.h"
#include "osi_log.h"
#include "osi_api.h"
#include "calclib/crc32.h"
#include <string.h>
#include <stdlib.h>

#define IDENTIFY_COUNT (3)

enum hdlc_parser_state
{
    HDLC_STATE_START,
    HDLC_STATE_RUN,
    HDLC_STATE_ESCAPE,
    HDLC_STATE_END,
};

struct fdl_engine
{
    fdlChannel_t *ch;
    unsigned max_packet_len;
};

static inline uint16_t _cpu_to_be16(uint16_t num)
{
    return ((num << 8) | ((num >> 8) & 0xff));
}

// TODO generic interface
static inline uint16_t _be16_to_cpu(uint16_t num)
{
    return ((num << 8) | ((num >> 8) & 0xff));
}

uint16_t _calc_crc16_frm(const uint16_t *src, int len)
{
    uint32_t sum = 0;

    while (len > 3)
    {
        sum += *src++;
        sum += *src++;
        len -= 4;
    }

    switch (len & 0x03)
    {
    case 2:
        sum += *src++;
        break;

    case 3:
        sum += *src++;
        sum += *((uint8_t *)src);
        break;

    case 1:
        sum += *((uint8_t *)src);
        break;

    default:
        break;
    }

    sum = (sum >> 16) + (sum & 0x0FFFF);
    sum += (sum >> 16);
    return (~sum);
}

fdlEngine_t *fdlEngineCreate(fdlChannel_t *channel, unsigned max_packet_len)
{
    if (channel == NULL)
        return NULL;

    fdlEngine_t *fdl = (fdlEngine_t *)calloc(1, sizeof(fdlEngine_t));
    if (fdl == NULL)
    {
        OSI_LOGE(0, "FDL create fail: not enough memory");
        return NULL;
    }

    fdl->ch = channel;
    fdl->max_packet_len = max_packet_len;
    return fdl;
}

void fdlEngineDestroy(fdlEngine_t *fdl)
{
    if (fdl)
    {
        fdl->ch->flush_input(fdl->ch);
        free(fdl);
    }
}

fdlChannel_t *fdlEngineGetChannel(fdlEngine_t *fdl)
{
    return fdl->ch;
}

unsigned fdlEngineGetMaxPacketLen(fdlEngine_t *fdl)
{
    return fdl->max_packet_len;
}

static int prvGetChar(fdlEngine_t *fdl)
{
    uint8_t ch;
    if (fdl->ch->read(fdl->ch, &ch, 1) < 0)
        return -1;
    return ch;
}

static inline uint32_t _fdl_read(fdlEngine_t *fdl, void *buf_, unsigned size)
{
    uint32_t r = 0;
    uint8_t *buf = (uint8_t *)buf_;
    while (r < size && fdl->ch->avail(fdl->ch) > 0)
    {
        int len = fdl->ch->read(fdl->ch, buf + r, size - r);
        if (len >= 0)
            r += len;
        else
            break;
    }
    return r;
}

static inline void _fdl_write_all(fdlEngine_t *fdl, const void *data_, unsigned len)
{
    const uint8_t *data = (const uint8_t *)data_;
    while (len > 0)
    {
        int r = fdl->ch->write(fdl->ch, data, len);
        len -= r;
        data += r;
    }
}

uint32_t fdlEngineReadRaw(fdlEngine_t *fdl, void *buf, unsigned size)
{
    if (fdl == NULL || fdl->ch == NULL)
        return 0;

    return _fdl_read(fdl, buf, size);
}

uint32_t fdlEngineWriteRaw(fdlEngine_t *fdl, const void *data, unsigned length)
{
    if (fdl == NULL || fdl->ch == NULL)
        return 0;

    _fdl_write_all(fdl, data, length);
    return length;
}

// Assume dst room at least 2 bytes
static inline unsigned _fdl_fill_send_byte(uint8_t b, uint8_t dst[2])
{
    if (b == HDLC_FLAG || b == HDLC_ESCAPE)
    {
        dst[0] = HDLC_ESCAPE;
        dst[1] = (b & ~HDLC_ESCAPE_MASK);
        return 2;
    }
    else
    {
        dst[0] = b;
        return 1;
    }
}

bool fdlEngineSendPacket(fdlEngine_t *fdl, fdlPacket_t *pkt)
{
    if (fdl == NULL || fdl->ch == NULL || pkt == NULL)
        return false;

    uint32_t size = pkt->size + 4;
    pkt->size = OSI_FROM_BE16(pkt->size);
    pkt->type = OSI_FROM_BE16(pkt->type);

    uint8_t *data = (uint8_t *)pkt;
    uint16_t crc = _calc_crc16_frm((uint16_t *)data, size);
    uint8_t *crc_buf = (uint8_t *)&crc;

    const uint32_t threshold = 128;
    uint8_t send_buf[128 + 8];
    uint32_t len = 0;

    send_buf[len++] = HDLC_FLAG;
    for (uint32_t i = 0; i < size; ++i)
    {
        len += _fdl_fill_send_byte(data[i], &send_buf[len]);
        if (len >= threshold)
        {
            _fdl_write_all(fdl, send_buf, len);
            len = 0;
        }
    }

    len += _fdl_fill_send_byte(crc_buf[0], &send_buf[len]);
    len += _fdl_fill_send_byte(crc_buf[1], &send_buf[len]);

    send_buf[len++] = HDLC_FLAG;

    _fdl_write_all(fdl, send_buf, len);
    fdl->ch->flush(fdl->ch);
    return true;
}

bool fdlEngineSendVersion(fdlEngine_t *fdl)
{
    if (fdl == NULL)
        return false;

    const char fdl_version_string[] = "Spreadtrum Boot Block version 1.2";

    uint8_t buf[64]; // enough room
    fdlPacket_t *pkt = (fdlPacket_t *)buf;

    pkt->type = BSL_REP_VER;
    pkt->size = sizeof(fdl_version_string);
    memcpy(&pkt->content[0], &fdl_version_string[0], pkt->size);

    return fdlEngineSendPacket(fdl, pkt);
}

bool fdlEngineSendRespNoData(fdlEngine_t *fdl, uint16_t type)
{
    if (fdl == NULL)
        return false;

    fdlPacket_t pkt;
    pkt.type = type;
    pkt.size = 0;

    return fdlEngineSendPacket(fdl, &pkt);
}

bool fdlEngineSendRespData(fdlEngine_t *fdl, uint16_t type, const void *data, unsigned length)
{
    if (fdl == NULL)
        return false;

    bool result = false;
    fdlPacket_t *pkt = (fdlPacket_t *)malloc(sizeof(fdlPacket_t) + length + 2);
    if (pkt == NULL)
    {
        OSI_LOGE(0, "FDL: send rsp malloc fail %d", length);
        return result;
    }

    memset(pkt, 0, length + sizeof(fdlPacket_t) + 2);

    pkt->type = type;
    pkt->size = length;
    memcpy(&pkt->content[0], (uint8_t *)data, pkt->size);

    result = fdlEngineSendPacket(fdl, pkt);

    free(pkt);
    return result;
}

bool fdlEngineSetBaud(fdlEngine_t *fdl, unsigned baud)
{
    if (fdl == NULL)
        return false;

    return fdl->ch->set_baud(fdl->ch, baud);
}

bool fdlEngineProcess(fdlEngine_t *fdl, fdlProc_t proc, fdlPolling_t polling, void *ctx)
{
    if (fdl == NULL || proc == NULL)
        return false;

    const unsigned pkt_mem_len = fdl->max_packet_len + 32;
    fdlPacket_t *pkt = malloc(pkt_mem_len);
    if (pkt == NULL)
        return false;
    memset(pkt, 0xff, pkt_mem_len);

    uint8_t *data = (uint8_t *)pkt;
    uint8_t recv_buf[128];
    uint32_t recv_len = sizeof(recv_buf);
    uint32_t len = 0;
    int state = HDLC_STATE_START;
    for (;;)
    {
        int r = _fdl_read(fdl, &recv_buf[0], recv_len);
        if (r <= 0)
        {
            if (polling != NULL)
                polling(fdl, ctx);
            continue;
        }

        for (unsigned i = 0; i < r; ++i)
        {
            uint8_t b = recv_buf[i];
            if (HDLC_STATE_START == state)
            {
                if (b != HDLC_FLAG)
                    continue;
                OSI_LOGD(0, "FDL: start found");
                state = HDLC_STATE_RUN;
                len = 0;
            }
            else if (HDLC_STATE_END == state)
            {
                if (b == HDLC_FLAG)
                {
                    OSI_LOGD(0, "FDL: end found");
                    pkt->size = _be16_to_cpu(pkt->size);

                    uint16_t crc = *((uint16_t *)&data[len - 2]);
                    uint16_t calc_crc = _calc_crc16_frm((uint16_t *)data, len - 2);

                    pkt->type = _be16_to_cpu(pkt->type);
                    pkt->size = _be16_to_cpu(pkt->size);

                    if (crc != calc_crc)
                    {
                        OSI_LOGE(0, "FDL process, crc mismatch[type: %x size: %x, Got: 0x%04x, Calc: 0x%04x]",
                                 pkt->type, pkt->size, crc, calc_crc);
                        osiPanic();
                    }
                    else
                    {
                        proc(fdl, pkt, ctx);
                        len = 0;
                    }
                    state = HDLC_STATE_RUN;
                }
                else
                {
                    OSI_LOGI(0, "FDL: end not found");
                    state = HDLC_STATE_START;
                }
            }
            else
            {
                if (b == HDLC_FLAG)
                {
                    OSI_LOGD(0, "FDL: unexpected flag len/%d", len);
                    state = HDLC_STATE_RUN;
                    len = 0;
                    continue;
                }
                if (b == HDLC_ESCAPE && state == HDLC_STATE_RUN)
                {
                    state = HDLC_STATE_ESCAPE;
                    continue;
                }

                if (state == HDLC_STATE_ESCAPE)
                {
                    b |= HDLC_ESCAPE_MASK;
                    state = HDLC_STATE_RUN;
                }

                if (len >= pkt_mem_len)
                {
                    OSI_LOGE(0, "FDL: packet length overflow len/%d", len);

                    // Drop all data, and search HDLC_FLAG. ResearchDowload may
                    // be halt or timeout, and it is needed to check log for
                    // this error.
                    len = 0;
                    state = HDLC_STATE_START;
                    break;
                }

                data[len++] = b;
                int head_len = sizeof(pkt->type) + sizeof(pkt->size);
                if (len == head_len)
                {
                    pkt->size = _be16_to_cpu(pkt->size);
                    OSI_LOGD(0, "FDL: fdl head %x %x %d", pkt->type, pkt->size, len);
                }
                if (len >= (head_len + sizeof(uint16_t)))
                {
                    if (pkt->size == (len - head_len - sizeof(uint16_t)))
                    {
                        state = HDLC_STATE_END;
                    }
                }
            }
        }
    }

    free(pkt);
    return false;
}

bool fdlEngineIdentify(fdlEngine_t *fdl, unsigned timeout)
{
    int count = 0;
    osiElapsedTimer_t elapsed;

    osiElapsedTimerStart(&elapsed);
    while (timeout == 0 || osiElapsedTime(&elapsed) < timeout)
    {
        int ch = prvGetChar(fdl);
        if (ch < 0)
            continue;

        if (ch != HDLC_FLAG)
            count = 0;
        else
            count++;

        if (count >= IDENTIFY_COUNT)
            return true;
    }

    return false;
}
