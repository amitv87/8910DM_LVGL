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

#ifndef _OSI_HDLC_H_
#define _OSI_HDLC_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief hdlc decoding state
 */
typedef enum
{
    /** decode on going */
    OSI_HDLC_DEC_ST_DECODING,
    /** complete packet decoded */
    OSI_HDLC_DEC_ST_PACKET,
    /** buffer overflow */
    OSI_HDLC_DEC_ST_OVERFLOW,
    /** length in the diag header is too large */
    OSI_HDLC_DEC_ST_DIAG_TOO_LARGE,
} osiHdlcDecodeState_t;

/**
 * \brief hdlc decoding flags
 */
typedef enum
{
    /** report buffer overflow */
    OSI_HDLC_DEC_CHECK_OVERFLOW = (1 << 0),
    /** report length in diag header is too large */
    OSI_HDLC_DEC_CHECK_DIAG_TOO_LARGE = (1 << 1),
} osiHdlcDecFlag_t;

/**
 * \brief hdlc decoding data struct
 */
typedef struct
{
    //! @cond Doxygen_Suppress
    char *buf;
    unsigned size;
    unsigned len;
    uint16_t flags;
    uint16_t state;
    //! @endcond
} osiHdlcDecode_t;

/**
 * \brief initialize hdlc decoding
 *
 * \param d hdlc decoder instance
 * \param buf buffer for decoded packet
 * \param size buffer size
 * \param flags refer to \p osiHdlcDecFlag_t
 * \return
 *      - true on success
 *      - false on error, invalid parameters
 */
bool osiHdlcDecodeInit(osiHdlcDecode_t *d, void *buf, unsigned size, uint16_t flags);

/**
 * \brief reset hdlc decoding buffer and state
 *
 * \param d hdlc decoder instance, must be valid
 */
void osiHdlcDecodeReset(osiHdlcDecode_t *d);

/**
 * \brief change packet buffer
 *
 * This is a little unusual. The purpose is when hdlc decoding reports
 * \p OSI_HDLC_DEC_CHECK_DIAG_TOO_LARGE, caller will allocate another
 * bigger buffer.
 *
 * This will change the used buffer and size. Also, the decoded data will
 * be copied from original buffer to new assigned buffer. The new buffer
 * should be larger than the existed decoded data size.
 *
 * \param d hdlc decoder instance, must be valid
 * \param buf buffer for decoded packet
 * \param size buffer size
 * \return
 *      - true on success
 *      - false on fail, invalid parameter
 */
bool osiHdlcDecodeChangeBuf(osiHdlcDecode_t *d, void *buf, unsigned size);

/**
 * \brief put hdlc encoded stream
 *
 * When there is complete packet inside, or overflow, it won't accept any
 * input data. Caller should avoid infinite loop.
 *
 * \param d hdlc decoder instance, must be valid
 * \param data memory for hdlc encoded stream
 * \param size size of hdlc encoded stream
 * \return
 *      - consumed encoded data size, 0 is possible
 *      - -1 on error, invalid parameters
 */
int osiHdlcDecodePush(osiHdlcDecode_t *d, const void *data, unsigned size);

/**
 * \brief get hdlc decoding state
 *
 * \param d hdlc decoder instance, must be valid
 * \return hdlc decoding state
 */
osiHdlcDecodeState_t osiHdlcDecodeGetState(osiHdlcDecode_t *d);

/**
 * \brief fetch decoded hdlc packet
 *
 * When there doesn't exist complete decoded hdlc packet, the returned
 * buffer size is 0.
 *
 * When there is a complete packet, it will clear the packet, and it can
 * accept input data after that.
 *
 * \param d hdlc decoder instance, must be valid
 * \return decoded hdlc packet
 */
osiBuffer_t osiHdlcDecodeFetchPacket(osiHdlcDecode_t *d);

/**
 * \brief get hdlc decoding buffer data
 *
 * It will return the buffer data, no matter what state is. And it won't
 * change hdlc decoding state.
 *
 * \param d hdlc decoder instance, must be valid
 * \return decoded hdlc data
 */
osiBuffer_t osiHdlcDecodeGetData(osiHdlcDecode_t *d);

int osiHdlcEncodeLen(const void *data, unsigned size);
int osiHdlcEncode(void *dst, const void *data, unsigned size);

/**
 * \brief estimate hdlc encoding size
 *
 * The return value is the same as \p osiHdlcEncodeMulti, and it won't
 * ensure the alignment of returned size.
 *
 * \param bufs buffer array
 * \param count buffer count
 * \return
 *      - encoded size
 *      - -1 on error, invalid parameter
 */
int osiHdlcEncodeMultiLen(const osiBuffer_t *bufs, unsigned count);

/**
 * \brief hdlc encoding to output buffer
 *
 * \p dst should be large enough to hold the whole encoded stream.
 *
 * \param dst output buffer, must be valid
 * \param bufs buffer array
 * \param count buffer count
 * \return
 *      - encoded size
 *      - -1 on error, invalid parameter
 */
int osiHdlcEncodeMulti(void *dst, const osiBuffer_t *bufs, unsigned count);

OSI_EXTERN_C_END
#endif
