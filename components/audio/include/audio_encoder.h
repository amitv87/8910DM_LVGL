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

#ifndef _AUDIO_ENCODER_H_
#define _AUDIO_ENCODER_H_

#include "osi_api.h"
#include "audio_types.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief forward declaration
 */
struct auEncoder;

/**
 * \brief forward declaration
 */
struct auWriter;

/**
 * \brief audio encoder parameter id and value pair
 *
 * This will be used to set multiple parameters for audio encoder.
 */
typedef struct
{
    int id;          ///< parameter id
    const void *val; ///< parameter value pointer
} auEncoderParamSet_t;

/**
 * \brief audio encoder operations
 */
typedef struct
{
    /** delete the audio encoder */
    void (*destroy)(struct auEncoder *d);
    /** refer to \p auEncoderEncode */
    int (*encode)(struct auEncoder *d, const auFrame_t *frame);
    /** refer to \p auEncoderSetParam */
    bool (*set_param)(struct auEncoder *d, int id, const void *val);
    /** refer to \p auEncoderGetParam */
    bool (*get_param)(struct auEncoder *d, int id, void *val);
} auEncoderOps_t;

/**
 * \brief opaque data structure of audio encoder
 *
 * When implement an audio encoder, the first field should be
 * <tt>const auEncoderOps_t *</tt>.
 */
typedef struct auEncoder
{
    auEncoderOps_t ops;
} auEncoder_t;

/**
 * \brief create audio encoder by stream format
 *
 * The audio frame information is unknown at creation. The audio frame
 * information in the first frame to be encoded will be used.
 *
 * \param w         audio writer to be used in audio encoder
 * \param format    stream format
 * \return
 *      - created audio encoder
 *      - NULL if invalid parameter, or out of memory
 */
auEncoder_t *auEncoderCreate(struct auWriter *w, auStreamFormat_t format);

/**
 * \brief delete the audio encoder
 *
 * \param d         audio encoder
 */
void auEncoderDelete(auEncoder_t *d);

/**
 * \brief encoder an audio frame
 *
 * Audio decoder shall write data to \p auWriter_t. Encoder should consume
 * all frame data. When audio encoder is frame based, the remaining data
 * should be backup by encoder.
 *
 * \param d         audio encoder
 * \param frame     audio frame to be encoded, can't be NULL
 * \return
 *      - encoded audio stream byte count
 *      - -1 on error
 */
int auEncoderEncode(auEncoder_t *d, const auFrame_t *frame);

/**
 * \brief set encoder parameter
 *
 * Audio encoder may support configurable parameter. This is the unified
 * API to set parameter. Each audio encoder will have independent param_id
 * definition, and the data type may be different for each parameter.
 * For example:
 *
 * \code{.cpp}
 *      auEncoder_t *encoder = auEncoderCreare(w, AUSTREAM_FORMAT_AMRNB);
 *      auAmrnbMode_t mode = AU_AMRNB_MODE_795;
 *      auEncoderSetParam(encoder, AU_AMBNB_ENC_PARAM_MODE, &mode);
 *      auEncoderGetParam(encoder, AU_AMBNB_ENC_PARAM_MODE, &mode);
 * \endcode
 *
 * All pamameter id should be positive, zero and negative values are
 * invalid.
 *
 * \param d         audio encoder
 * \param param_id  parameter id
 * \param val       parameter val to be set
 * \return
 *      - true on success
 *      - false on fail, invalud paramter id or invalid parameter value
 */
bool auEncoderSetParam(auEncoder_t *d, int param_id, const void *val);

/**
 * \brief set multiple encoder parameter
 *
 * Set multiple parameters of audio encoder. When \p params is not NULL,
 * it should be an array, and the \p id of the last element must be zero
 * to indicate the end.
 *
 * When one parameter set fails, it will return false. However, the
 * remaining parameters in the array will be set still.
 *
 * \param d         audio encoder
 * \param params    array of parameter id and value
 * \return
 *      - true on success
 *      - false when one set fails
 */
bool auEncoderSetMultiParams(auEncoder_t *d, const auEncoderParamSet_t *params);

/**
 * \brief get encoder parameter
 *
 * \param d         audio encoder
 * \param param_id  parameter id
 * \param val       parameter val to be get
 * \return
 *      - true on success
 *      - false on fail, invalud paramter id
 */
bool auEncoderGetParam(auEncoder_t *d, int param_id, void *val);

/**
 * \brief opaque data structure of WAV encoder
 */
typedef struct auWavEncoder auWavEncoder_t;

/**
 * \brief create a WAV encoder
 *
 * \param w         audio writer to be used in audio encoder
 * \return
 *      - created audio encoder
 *      - NULL if invalid parameter, or out of memory
 */
auWavEncoder_t *auWavEncoderCreate(struct auWriter *w);

/**
 * \brief opaque data structure of PCM encoder
 */
typedef struct auPcmEncoder auPcmEncoder_t;

/**
 * \brief create a PCM encoder
 *
 * \param w         audio writer to be used in audio encoder
 * \return
 *      - created audio encoder
 *      - NULL if invalid parameter, or out of memory
 */
auPcmEncoder_t *auPcmEncoderCreate(struct auWriter *w);

/**
 * \brief opaque data structure of AMR-NB encoder
 */
typedef struct auAmrnbEncoder auAmrnbEncoder_t;

/**
 * \brief AMR-NB encoder parameter id
 */
typedef enum
{
    /** placeholder for param_id start */
    AU_AMRNB_ENC_PARAM_START = AUSTREAM_FORMAT_AMRNB << 16,
    /** \p auAmrnbMode_t, mode */
    AU_AMRNB_ENC_PARAM_MODE,
    /** placeholder for param_id end */
    AU_AMRNB_ENC_PARAM_END
} auAmrnbEncoderParamId_t;

/**
 * \brief AMR-NB encoder default mode at create
 */
extern const auAmrnbMode_t gAmrnbEncodeDefaultMode;

/**
 * \brief create a AMR-NB encoder
 *
 * \param w         audio writer to be used in audio encoder
 * \return
 *      - created audio encoder
 *      - NULL if invalid parameter, or out of memory
 */
auAmrnbEncoder_t *auAmrnbEncoderCreate(struct auWriter *w);

/**
 * \brief opaque data structure of AMR-WB encoder
 */
typedef struct auAmrwbEncoder auAmrwbEncoder_t;

/**
 * \brief AMR-WB encoder parameter id
 */
typedef enum
{
    /** placeholder for param_id start */
    AU_AMRWB_ENC_PARAM_START = AUSTREAM_FORMAT_AMRWB << 16,
    /** \p auAmrnbMode_t, mode */
    AU_AMRWB_ENC_PARAM_MODE,
    /** placeholder for param_id end */
    AU_AMRWB_ENC_PARAM_END
} auAmrwbEncoderParamId_t;

/**
 * \brief AMR-WB encoder default mode at create
 */
extern const auAmrwbMode_t gAmrwbEncodeDefaultMode;

/**
 * \brief create a AMR-WB encoder
 *
 * \param w         audio writer to be used in audio encoder
 * \return
 *      - created audio encoder
 *      - NULL if invalid parameter, or out of memory
 */
auAmrwbEncoder_t *auAmrwbEncoderCreate(struct auWriter *w);

OSI_EXTERN_C_END
#endif
