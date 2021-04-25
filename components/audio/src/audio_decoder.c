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

#include "audio_decoder.h"
#include "audio_reader.h"
#include "osi_log.h"

void auDecoderDelete(auDecoder_t *d)
{
    if (d != NULL && d->ops.destroy != NULL)
        d->ops.destroy(d);
}

int auDecoderDecode(auDecoder_t *d, auFrame_t *frame)
{
    if (d != NULL && d->ops.decode != NULL)
        return d->ops.decode(d, frame);
    return -1;
}

bool auDecoderSeek(auDecoder_t *d, unsigned ms)
{
    if (d != NULL && d->ops.seek != NULL)
        return d->ops.seek(d, ms);
    return false;
}

bool auDecoderSetParam(auDecoder_t *d, int param_id, const void *val)
{
    if (d != NULL && d->ops.set_param != NULL)
        return d->ops.set_param(d, param_id, val);
    return false;
}

bool auDecoderSetMultiParams(auDecoder_t *d, const auDecoderParamSet_t *params)
{
    if (params == NULL)
        return false;

    bool ok = true;
    for (; params->id != 0; ++params)
    {
        if (!auDecoderSetParam(d, params->id, params->val))
            ok = false;
    }
    return ok;
}

bool auDecoderGetParam(auDecoder_t *d, int param_id, void *val)
{
    if (d != NULL && d->ops.get_param != NULL)
        return d->ops.get_param(d, param_id, val);
    return false;
}

auDecoder_t *auDecoderCreate(auReader_t *r, auStreamFormat_t format)
{
    switch (format)
    {
    case AUSTREAM_FORMAT_PCM:
        return (auDecoder_t *)auPcmDecoderCreate(r);

    case AUSTREAM_FORMAT_WAVPCM:
        return (auDecoder_t *)auWavDecoderCreate(r);

#ifdef CONFIG_AUDIO_MP3_DEC_ENABLE
    case AUSTREAM_FORMAT_MP3:
        return (auDecoder_t *)auMp3DecoderCreate(r);
#endif

#ifdef CONFIG_AUDIO_AMRNB_DEC_ENABLE
    case AUSTREAM_FORMAT_AMRNB:
        return (auDecoder_t *)auAmrnbDecoderCreate(r);
#endif

#ifdef CONFIG_AUDIO_AMRWB_DEC_ENABLE
    case AUSTREAM_FORMAT_AMRWB:
        return (auDecoder_t *)auAmrwbDecoderCreate(r);
#endif

#ifdef CONFIG_AUDIO_SBC_DEC_ENABLE
    case AUSTREAM_FORMAT_SBC:
        return (auDecoder_t *)auSbcDecoderCreate(r);
#endif

    default:
        OSI_LOGE(0, "audio decoder unknown fotmat/%d", format);
        return NULL;
    }
}
