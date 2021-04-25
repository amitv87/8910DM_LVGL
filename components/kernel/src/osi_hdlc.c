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

#include "osi_hdlc.h"
#include "osi_trace.h"
#include <string.h>

#define HDLC_FLAG (0x7E)
#define HDLC_ESCAPE (0x7D)
#define HDLC_ESCAPE_MASK (0x20)

#define OSI_HDLC_DEC_ST_SEEK_FLAG (0x10)
#define OSI_HDLC_DEC_ST_FEED_DATA (0x11)
#define OSI_HDLC_DEC_ST_ESCAPED (0x12)

#define PUSH_BYTE_MASK(ch)          \
    do                              \
    {                               \
        *(buf + (pos & mask)) = ch; \
        pos = (pos + 1);            \
    } while (0)

#define PUSH_BYTE_MASK_CHECK(ch)                   \
    do                                             \
    {                                              \
        if (ch == HDLC_FLAG || ch == HDLC_ESCAPE)  \
        {                                          \
            PUSH_BYTE_MASK(HDLC_ESCAPE);           \
            PUSH_BYTE_MASK(ch ^ HDLC_ESCAPE_MASK); \
        }                                          \
        else                                       \
        {                                          \
            PUSH_BYTE(ch);                         \
        }                                          \
    } while (0)

#define PUSH_BYTE(ch)      \
    do                     \
    {                      \
        *(buf + pos) = ch; \
        pos = (pos + 1);   \
    } while (0)

#define PUSH_BYTE_CHECK(ch)                       \
    do                                            \
    {                                             \
        if (ch == HDLC_FLAG || ch == HDLC_ESCAPE) \
        {                                         \
            PUSH_BYTE(HDLC_ESCAPE);               \
            PUSH_BYTE(ch ^ HDLC_ESCAPE_MASK);     \
        }                                         \
        else                                      \
        {                                         \
            PUSH_BYTE(ch);                        \
        }                                         \
    } while (0)

bool osiHdlcDecodeInit(osiHdlcDecode_t *d, void *buf, unsigned size, uint16_t flags)
{
    if (d == NULL || buf == NULL)
        return false;

    d->buf = (char *)buf;
    d->size = size;
    d->flags = flags;
    d->len = 0;
    d->state = OSI_HDLC_DEC_ST_SEEK_FLAG;
    return true;
}

void osiHdlcDecodeReset(osiHdlcDecode_t *d)
{
    d->len = 0;
    d->state = OSI_HDLC_DEC_ST_SEEK_FLAG;
}

bool osiHdlcDecodeChangeBuf(osiHdlcDecode_t *d, void *buf, unsigned size)
{
    if (buf == NULL || size == 0 || size < d->len)
        return false;

    memcpy(buf, d->buf, d->len);
    d->buf = buf;
    d->len = size;
    return true;
}

int osiHdlcDecodePush(osiHdlcDecode_t *d, const void *data, unsigned size)
{
    if (data == NULL)
        return -1;
    if (size == 0)
        return 0;

    if (d->state == OSI_HDLC_DEC_ST_PACKET ||
        d->state == OSI_HDLC_DEC_ST_OVERFLOW ||
        d->state == OSI_HDLC_DEC_ST_DIAG_TOO_LARGE)
        return 0;

    const char *psdata = (const char *)data;
    for (unsigned n = 0; n < size; n++)
    {
        char ch = *psdata++;
        if (d->state == OSI_HDLC_DEC_ST_SEEK_FLAG)
        {
            if (ch == HDLC_FLAG)
            {
                d->state = OSI_HDLC_DEC_ST_FEED_DATA;
                d->len = 0;
            }
        }
        else if (d->state == OSI_HDLC_DEC_ST_ESCAPED)
        {
            ch ^= HDLC_ESCAPE_MASK;
            if (d->len >= d->size)
            {
                d->state = OSI_HDLC_DEC_ST_OVERFLOW;
                if (d->flags & OSI_HDLC_DEC_CHECK_OVERFLOW)
                    return OSI_PTR_DIFF(psdata, data) - 1; // last byte not consumed

                d->state = OSI_HDLC_DEC_ST_SEEK_FLAG;
                d->len = 0;
            }
            else
            {
                d->state = OSI_HDLC_DEC_ST_FEED_DATA;
                d->buf[d->len++] = ch;
            }
        }
        else if (d->state == OSI_HDLC_DEC_ST_FEED_DATA)
        {
            if (ch == HDLC_FLAG)
            {
                if (d->len == 0)
                    continue;

                d->state = OSI_HDLC_DEC_ST_PACKET;
                break;
            }

            if (ch == HDLC_ESCAPE)
            {
                d->state = OSI_HDLC_DEC_ST_ESCAPED;
                continue;
            }

            if (d->len >= d->size)
            {
                d->state = OSI_HDLC_DEC_ST_OVERFLOW;
                if (d->flags & OSI_HDLC_DEC_CHECK_OVERFLOW)
                    return OSI_PTR_DIFF(psdata, data) - 1; // last byte not consumed

                d->state = OSI_HDLC_DEC_ST_SEEK_FLAG;
                d->len = 0;
            }
            else
            {
                d->state = OSI_HDLC_DEC_ST_FEED_DATA;
                d->buf[d->len++] = ch;
            }
        }

        if (d->flags & OSI_HDLC_DEC_CHECK_DIAG_TOO_LARGE)
        {
            if (d->state == OSI_HDLC_DEC_ST_FEED_DATA &&
                d->len == sizeof(osiDiagPacketHeader_t))
            {
                osiDiagPacketHeader_t *cmd = (osiDiagPacketHeader_t *)d->buf;
                if (cmd->len > d->size)
                {
                    d->state = OSI_HDLC_DEC_ST_DIAG_TOO_LARGE;
                    break;
                }
            }
        }
    }

    return OSI_PTR_DIFF(psdata, data);
}

osiHdlcDecodeState_t osiHdlcDecodeGetState(osiHdlcDecode_t *d)
{
    if (d->state >= OSI_HDLC_DEC_ST_SEEK_FLAG)
        return OSI_HDLC_DEC_ST_DECODING;

    return d->state;
}

osiBuffer_t osiHdlcDecodeFetchPacket(osiHdlcDecode_t *d)
{
    osiBuffer_t buf = {
        .ptr = (uintptr_t)d->buf,
        .size = 0,
    };

    if (d->state == OSI_HDLC_DEC_ST_PACKET)
    {
        buf.size = d->len;
        d->state = OSI_HDLC_DEC_ST_FEED_DATA;
        d->len = 0;
    }
    return buf;
}

osiBuffer_t osiHdlcDecodeGetData(osiHdlcDecode_t *d)
{
    osiBuffer_t buf = {
        .ptr = (uintptr_t)d->buf,
        .size = d->len,
    };
    return buf;
}

OSI_ATTRIBUTE_OPTIMIZE(3)
int osiHdlcEncodeMultiLen(const osiBuffer_t *bufs, unsigned count)
{
    if (bufs == NULL)
        return -1;

    unsigned enc_size = 1;
    for (unsigned n = 0; n < count; n++, bufs++)
    {
        uint8_t *pstart = (uint8_t *)bufs->ptr;
        uint8_t *pend = pstart + bufs->size;
        while (pstart < pend)
        {
            uint8_t ch = *pstart++;
            enc_size += (ch == HDLC_FLAG || ch == HDLC_ESCAPE) ? 2 : 1;
        }
    }

    enc_size++;
    return enc_size;
}

OSI_ATTRIBUTE_OPTIMIZE(3)
int osiHdlcEncodeMulti(void *dst, const osiBuffer_t *bufs, unsigned count)
{
    if (dst == NULL || bufs == NULL)
        return -1;

    char *buf = dst;
    unsigned pos = 0;

    PUSH_BYTE(HDLC_FLAG);
    for (unsigned n = 0; n < count; n++, bufs++)
    {
        const char *psdata = (const char *)bufs->ptr;
        const char *pedata = psdata + bufs->size;
        while (psdata < pedata)
        {
            char ch = *psdata++;
            PUSH_BYTE_CHECK(ch);
        }
    }

    PUSH_BYTE(HDLC_FLAG);
    return pos;
}

OSI_ATTRIBUTE_OPTIMIZE(3)
int osiHdlcEncodeLen(const void *data, unsigned size)
{
    if (data == NULL)
        return -1;

    unsigned enc_size = 1;
    const uint8_t *pstart = (const uint8_t *)data;
    const uint8_t *pend = pstart + size;
    while (pstart < pend)
    {
        uint8_t ch = *pstart++;
        enc_size += (ch == HDLC_FLAG || ch == HDLC_ESCAPE) ? 2 : 1;
    }

    enc_size++;
    return enc_size;
}

OSI_ATTRIBUTE_OPTIMIZE(3)
int osiHdlcEncode(void *dst, const void *data, unsigned size)
{
    if (dst == NULL || data == NULL)
        return -1;

    char *buf = dst;
    unsigned pos = 0;

    PUSH_BYTE(HDLC_FLAG);
    const char *psdata = (const char *)data;
    const char *pedata = psdata + size;
    while (psdata < pedata)
    {
        char ch = *psdata++;
        PUSH_BYTE_CHECK(ch);
    }

    PUSH_BYTE(HDLC_FLAG);
    return pos;
}
