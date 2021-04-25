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

#ifndef _AUDIO_DECODER_H_
#define _AUDIO_DECODER_H_

#include "osi_api.h"
#include "audio_types.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief forward declaration
 */
struct auDecoder;

/**
 * \brief forward declaration
 */
struct auReader;

/**
 * \brief audio decoder parameter id and value pair
 *
 * This will be used to set multiple parameters for audio decoder.
 */
typedef struct
{
    int id;          ///< parameter id
    const void *val; ///< parameter value pointer
} auDecoderParamSet_t;

/**
 * \brief common decoder parameter id
 */
typedef enum
{
    /** placeholder for param_id start */
    AU_DEC_PARAM_START = AUSTREAM_FORMAT_UNKNOWN << 16,
    /** \p auFrame_t, format only, all should support get */
    AU_DEC_PARAM_FORMAT,
    /** placeholder for param_id end */
    AU_DEC_PARAM_END
} auDecoderParamId_t;

/**
 * \brief audio decoder operations
 */
typedef struct
{
    /** delete the audio decoder */
    void (*destroy)(struct auDecoder *d);
    /** refer to \p auDecoderDecode */
    int (*decode)(struct auDecoder *d, auFrame_t *frame);
    /** refer to \p auDecoderSeek */
    bool (*seek)(struct auDecoder *d, unsigned ms);
    /** refer to \p auDecoderSetParam */
    bool (*set_param)(struct auDecoder *d, int id, const void *val);
    /** refer to \p auDecoderGetParam */
    bool (*get_param)(struct auDecoder *d, int id, void *val);
} auDecoderOps_t;

/**
 * \brief opaque data structure of audio decoder
 *
 * When implement an audio decoder, the first field should be
 * <tt>const auDecoderOps_t *</tt>.
 */
typedef struct auDecoder
{
    auDecoderOps_t ops;
} auDecoder_t;

/**
 * \brief create audio decoder by stream format
 *
 * \param r         audio reader to be used in audio decoder
 * \param format    stream format
 * \return
 *      - created audio decoder
 *      - NULL if invalid parameter, or out of memory
 */
auDecoder_t *auDecoderCreate(struct auReader *r, auStreamFormat_t format);

/**
 * \brief delete the audio decoder
 *
 * \param d         audio decoder
 */
void auDecoderDelete(auDecoder_t *d);

/**
 * \brief decode an audio frame
 *
 * Audio decoder shall read data from \p auReader_t. After decoding, the
 * decoded audio frame data will be writen to \p frame. Memory of
 * \p frame->samples is owned by audio decoder. Before next call of
 * \p auDecoderDecode, the content will be kept.
 *
 * \param d         audio decoder
 * \param frame     decoded audio frame, can't be NULL
 * \return
 *      - decoded audio frame byte count
 *      - -1 on error
 */
int auDecoderDecode(auDecoder_t *d, auFrame_t *frame);

/**
 * \brief seek to specified time
 *
 * After \p auDecoderSeek, the audio frame of next \p auDecoderDecode
 * should be the samples starting from the specified time. If the audio
 * stream is frame based, the decoded audio frame may be started from the
 * middle of audio stream frame.
 *
 * It is not an error that \p ms is larger than total time. In that case,
 * the decoder should be located to the end of stream.
 *
 * When seek is not supported, the audio frame of next \p auDecoderDecode
 * should start from current position.
 *
 * \param d         audio decoder
 * \param ms        seek time in milliseconds
 * \return
 *      - true on success
 *      - false for invalid parameters, or seek is not supported
 */
bool auDecoderSeek(auDecoder_t *d, unsigned ms);

/**
 * \brief set decoder parameter
 *
 * Though it is rare to set decoder parameter, this mechanism is provided.
 * Refer to \p auEncoderSetParam for the usage model.
 *
 * All pamameter id should be positive, zero and negative values are
 * invalid.
 *
 * \param d         audio decoder
 * \param param_id  parameter id
 * \param val       parameter val to be set
 * \return
 *      - true on success
 *      - false on fail, invalud paramter id or invalid parameter value
 */
bool auDecoderSetParam(auDecoder_t *d, int param_id, const void *val);

/**
 * \brief set multiple decoder parameter
 *
 * Set multiple parameters of audio decoder. When \p params is not NULL,
 * it should be an array, and the \p id of the last element must be zero
 * to indicate the end.
 *
 * When one parameter set fails, it will return false. However, the
 * remaining parameters in the array will be set still.
 *
 * \param d         audio decoder
 * \param params    array of parameter id and value
 * \return
 *      - true on success
 *      - false when one set fails
 */
bool auDecoderSetMultiParams(auDecoder_t *d, const auDecoderParamSet_t *params);

/**
 * \brief get decoder parameter
 *
 * \param d         audio decoder
 * \param param_id  parameter id
 * \param val       parameter val to be get
 * \return
 *      - true on success
 *      - false on fail, invalud paramter id
 */
bool auDecoderGetParam(auDecoder_t *d, int param_id, void *val);

/**
 * \brief opaque data structure of MP3 decoder
 */
typedef struct auMp3Decoder auMp3Decoder_t;

/**
 * \brief create a MP3 decoder
 *
 * \param r         audio reader to be used in audio decoder
 * \return
 *      - created audio decoder
 *      - NULL if invalid parameter, or out of memory
 */
auMp3Decoder_t *auMp3DecoderCreate(struct auReader *r);

/**
 * \brief opaque data structure of WAV decoder
 */
typedef struct auWavDecoder auWavDecoder_t;

/**
 * \brief create a WAV decoder
 *
 * \param r         audio reader to be used in audio decoder
 * \return
 *      - created audio decoder
 *      - NULL if invalid parameter, or out of memory
 */
auWavDecoder_t *auWavDecoderCreate(struct auReader *r);

/**
 * \brief opaque data structure of PCM decoder
 */
typedef struct auPcmDecoder auPcmDecoder_t;

/**
 * \brief create a PCM decoder
 *
 * \param r         audio reader to be used in audio decoder
 * \return
 *      - created audio decoder
 *      - NULL if invalid parameter, or out of memory
 */
auPcmDecoder_t *auPcmDecoderCreate(struct auReader *r);

/**
 * \brief opaque data structure of AMR-NB decoder
 */
typedef struct auAmrnbDecoder auAmrnbDecoder_t;

/**
 * \brief create a AMR-NB decoder
 *
 * \param r         audio reader to be used in audio decoder
 * \return
 *      - created audio decoder
 *      - NULL if invalid parameter, or out of memory
 */
auAmrnbDecoder_t *auAmrnbDecoderCreate(struct auReader *r);

/**
 * \brief opaque data structure of AMR-WB decoder
 */
typedef struct auAmrwbDecoder auAmrwbDecoder_t;

/**
 * \brief create a AMR-WB decoder
 *
 * \param r         audio reader to be used in audio decoder
 * \return
 *      - created audio decoder
 *      - NULL if invalid parameter, or out of memory
 */
auAmrwbDecoder_t *auAmrwbDecoderCreate(struct auReader *r);

/**
 * \brief opaque data structure of SBC decoder
 */
typedef struct auSbcDecoder auSbcDecoder_t;

/**
 * \brief create a SBC decoder
 *
 * \param r         audio reader to be used in audio decoder
 * \return
 *      - created audio decoder
 *      - NULL if invalid parameter, or out of memory
 */
auSbcDecoder_t *auSbcDecoderCreate(struct auReader *r);

OSI_EXTERN_C_END
#endif
