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

#ifndef _OSI_BYTE_BUF_H_
#define _OSI_BYTE_BUF_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief byte buffer with external assigned memory
 */
typedef struct
{
    uint8_t *buff; ///< memory
    unsigned size; ///< memory byte count
    unsigned len;  ///< valid data length
} osiByteBuf_t;

/**
 * \brief decrease buffer data length, return the byte at tail
 *
 * At buffer empty, data length is unchanged and return 0
 *
 * \param p     byte buffer
 * \return      byte at tail, or 0 at buffer empty
 */
static inline uint8_t osiByteBufPop(osiByteBuf_t *p)
{
    if (p->len > 0)
    {
        p->len--;
        return p->buff[p->len];
    }
    return 0;
}

/**
 * \brief incease buffer data length, set the byte at tail
 *
 * At buffer full, nothing will be done
 *
 * \param p     byte buffer
 * \param c     byte to be put to the end of buffer
 */
static inline void osiByteBufPush(osiByteBuf_t *p, uint8_t c)
{
    if (p->len < p->size)
        p->buff[p->len++] = c;
}

/**
 * \brief get last byte of the buffer
 *
 * \param p     byte buffer
 * \return      byte at the end, or 0 at buffer empty
 */
static inline uint8_t osiByteBufTail(osiByteBuf_t *p, uint16_t ch)
{
    return p->len > 0 ? p->buff[p->len - 1] : 0;
}

/**
 * \brief clear data in buffer
 *
 * \param p     byte buffer
 */
static inline void osiByteBufClear(osiByteBuf_t *p)
{
    p->len = 0;
}

/**
 * \brief whether the buffer is full
 *
 * \param p     byte buffer
 * \return      true if the buffer is full
 */
static inline bool osiByteBufIsFull(osiByteBuf_t *p)
{
    return p->len >= p->size;
}

/**
 * \brief whether the buffer is empty
 *
 * \param p     byte buffer
 * \return      true if the buffer is empty
 */
static inline bool osiByteBufIsEmpty(osiByteBuf_t *p)
{
    return (p->len == 0);
}

/**
 * \brief set a bit in bitmap
 *
 * Bitmap will use 32bits word, and each word holds 32 bits.
 * Caller should ensure parameters are valid.
 *
 * \param bitmap        word array for bitmap
 * \param n             the bit to be set
 */
static inline void osiBitmapSet(uint32_t *bitmap, int n)
{
    bitmap[n / 32] |= (1 << (n % 32));
}

/**
 * \brief clear a bit in bitmap
 *
 * Bitmap will use 32bits word, and each word holds 32 bits.
 * Caller should ensure parameters are valid.
 *
 * \param bitmap        word array for bitmap
 * \param n             the bit to be cleared
 */
static inline void osiBitmapClear(uint32_t *bitmap, int n)
{
    bitmap[n / 32] &= ~(1 << (n % 32));
}

/**
 * \brief check whether a bit in bitmap is set
 *
 * Bitmap will use 32bits word, and each word holds 32 bits.
 * Caller should ensure parameters are valid.
 *
 * \param bitmap        word array for bitmap
 * \param n             the bit to be checked
 */
static inline bool osiBitmapIsSet(uint32_t *bitmap, int n)
{
    return bitmap[n / 32] & (1 << (n % 32));
}

/**
 * \brief get a byte (8bits) from a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer
 * \return              the byte value
 */
static inline uint8_t osiBytesGet8(const void *ptr)
{
    const uint8_t *p = (const uint8_t *)ptr;
    return p[0];
}

/**
 * \brief put a byte (8bits) to a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer
 * \param v             the byte value
 */
static inline void osiBytesPut8(void *ptr, uint8_t v)
{
    uint8_t *p = (uint8_t *)ptr;
    p[0] = v;
}

/**
 * \brief get a big endian short (16bits) from a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer, may be unaligned
 * \return              the short value
 */
static inline uint16_t osiBytesGetBe16(const void *ptr)
{
    const uint8_t *p = (const uint8_t *)ptr;
    return (p[0] << 8) | p[1];
}

/**
 * \brief put a big endian short (16bits) to a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer, may be unaligned
 * \param v             the short value
 */
static inline void osiBytesPutBe16(void *ptr, uint16_t v)
{
    uint8_t *p = (uint8_t *)ptr;
    p[0] = (v >> 8) & 0xff;
    p[1] = v & 0xff;
}

/**
 * \brief get a big endian word (32bits) from a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer, may be unaligned
 * \return              the word value
 */
static inline uint32_t osiBytesGetBe32(const void *ptr)
{
    const uint8_t *p = (const uint8_t *)ptr;
    return (p[0] << 24) | (p[1] << 24) | (p[2] << 8) | p[3];
}

/**
 * \brief put a big endian word (32bits) to a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer, may be unaligned
 * \param v             the word value
 */
static inline void osiBytesPutBe32(void *ptr, uint16_t v)
{
    uint8_t *p = (uint8_t *)ptr;
    p[0] = (v >> 24) & 0xff;
    p[1] = (v >> 16) & 0xff;
    p[2] = (v >> 8) & 0xff;
    p[3] = v & 0xff;
}

/**
 * \brief get a little endian short (16bits) from a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer, may be unaligned
 * \return              the short value
 */
static inline uint16_t osiBytesGetLe16(const void *ptr)
{
    const uint8_t *p = (const uint8_t *)ptr;
    return p[0] | (p[1] << 8);
}

/**
 * \brief put a little endian short (16bits) to a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer, may be unaligned
 * \param v             the short value
 */
static inline void osiBytesPutLe16(void *ptr, uint16_t v)
{
    uint8_t *p = (uint8_t *)ptr;
    p[0] = v & 0xff;
    p[1] = (v >> 8) & 0xff;
}

/**
 * \brief get a little endian word (32bits) from a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer, may be unaligned
 * \return              the word value
 */
static inline uint32_t osiBytesGetLe32(const void *ptr)
{
    const uint8_t *p = (const uint8_t *)ptr;
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

/**
 * \brief put a little endian word (32bits) to a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer, may be unaligned
 * \param v             the word value
 */
static inline void osiBytesPutLe32(void *ptr, uint32_t v)
{
    uint8_t *p = (uint8_t *)ptr;
    p[0] = v & 0xff;
    p[1] = (v >> 8) & 0xff;
    p[2] = (v >> 16) & 0xff;
    p[3] = (v >> 24) & 0xff;
}

/**
 * \brief get a little endian long long (64bits) from a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer, may be unaligned
 * \return              the long long value
 */
static inline uint64_t osiBytesGetLe64(const void *ptr)
{
    const uint8_t *p = (const uint8_t *)ptr;
    return osiBytesGetLe32(p) | ((uint64_t)osiBytesGetLe32(p + 4) << 32);
}

/**
 * \brief put a little endian long long (64bits) to a pointer
 *
 * Caller should ensure parameters are valid.
 *
 * \param ptr           the pointer, may be unaligned
 * \param v             the long long value
 */
static inline void osiBytesPutLe64(void *ptr, uint64_t v)
{
    uint8_t *p = (uint8_t *)ptr;
    osiBytesPutLe32(p, v & 0xffffffff);
    osiBytesPutLe32(p + 4, (v >> 32) & 0xffffffff);
}

/**
 * \brief get a block memory from a pointer
 *
 * Caller should ensure parameters are valid. It is just \p memcpy,
 * and just for similar style.
 *
 * \param ptr           the pointer, may be unaligned
 * \param data          memory for get
 * \param size          get size
 */
static inline void osiBytesGetMem(const void *ptr, void *data, unsigned size)
{
    memcpy(data, ptr, size);
}

/**
 * \brief put a block memory to a pointer
 *
 * Caller should ensure parameters are valid. It is just \p memcpy,
 * and just for similar style.
 *
 * \param ptr           the pointer, may be unaligned
 * \param data          memory for put
 * \param size          put size
 */
static inline void osiBytesPutMem(void *ptr, const void *data, unsigned size)
{
    memcpy(ptr, data, size);
}

/**
 * \brief read byte from stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \return      byte value
 */
#define OSI_STRM_R8(p) ({ const void *_orig = (const void*)(p); (p) += 1; osiBytesGet8(_orig); })

/**
 * \brief read big endian short from stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \return      short value
 */
#define OSI_STRM_RBE16(p) ({ const void *_orig = (const void*)(p); (p) += 2; osiBytesGetBe16(_orig); })

/**
 * \brief read big endian word from stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \return      word value
 */
#define OSI_STRM_RBE32(p) ({ const void *_orig = (const void*)(p); (p) += 4; osiBytesGetBe32(_orig); })

/**
 * \brief read little endian short from stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \return      short value
 */
#define OSI_STRM_RLE16(p) ({ const void *_orig = (const void*)(p); (p) += 2; osiBytesGetLe16(_orig); })

/**
 * \brief read little endian word from stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \return      word value
 */
#define OSI_STRM_RLE32(p) ({ const void *_orig = (const void*)(p); (p) += 4; osiBytesGetLe32(_orig); })

/**
 * \brief read little endian long long from stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \return      long long value
 */
#define OSI_STRM_RLE64(p) ({ const void *_orig = (const void*)(p); (p) += 8; osiBytesGetLe64(_orig); })

/**
 * \brief write byte to stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \param v     byte value
 */
#define OSI_STRM_W8(p, v) ({ void *_orig = (void*)(p); (p) += 1; osiBytesPut8(_orig, v); })

/**
 * \brief write big endian short to stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \param v     short value
 */
#define OSI_STRM_WBE16(p, v) ({ void *_orig = (void*)(p); (p) += 2; osiBytesPutBe16(_orig, v); })

/**
 * \brief write big endian word to stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \param v     word value
 */
#define OSI_STRM_WBE32(p, v) ({ void *_orig = (void*)(p); (p) += 4; osiBytesPutBe32(_orig, v); })

/**
 * \brief write little endian short to stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \param v     short value
 */
#define OSI_STRM_WLE16(p, v) ({ void *_orig = (void*)(p); (p) += 2; osiBytesPutLe16(_orig, v); })

/**
 * \brief write little endian word to stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \param v     word value
 */
#define OSI_STRM_WLE32(p, v) ({ void *_orig = (void*)(p); (p) += 4; osiBytesPutLe32(_orig, v); })

/**
 * \brief write little endian long long to stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \param v     long long value
 */
#define OSI_STRM_WLE64(p, v) ({ void *_orig = (void*)(p); (p) += 8; osiBytesPutLe64(_orig, v); })

/**
 * \brief write memory to stream, and increase pointer afterward
 *
 * \param p     pointer, which can be <tt>char*, uintptr_t</tt>
 * \param v     memory pointer
 * \param s     memory size
 */
#define OSI_STRM_WMEM(p, v, s) ({ unsigned _s = (s); memcpy((void*)p, (v), _s); (p) += _s; })

#ifdef __cplusplus
}
#endif
#endif
