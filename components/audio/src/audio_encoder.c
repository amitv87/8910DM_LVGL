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

#include "audio_encoder.h"
#include "osi_log.h"

void auEncoderDelete(auEncoder_t *d)
{
    if (d != NULL && d->ops.destroy != NULL)
        d->ops.destroy(d);
}

int auEncoderEncode(auEncoder_t *d, const auFrame_t *frame)
{
    if (d != NULL && d->ops.encode != NULL)
        return d->ops.encode(d, frame);
    return -1;
}

bool auEncoderSetParam(auEncoder_t *d, int param_id, const void *val)
{
    if (d != NULL && d->ops.set_param != NULL)
        return d->ops.set_param(d, param_id, val);
    return false;
}

bool auEncoderSetMultiParams(auEncoder_t *d, const auEncoderParamSet_t *params)
{
    if (params == NULL)
        return false;

    bool ok = true;
    for (; params->id != 0; ++params)
    {
        if (!auEncoderSetParam(d, params->id, params->val))
            ok = false;
    }
    return ok;
}

bool auEncoderGetParam(auEncoder_t *d, int param_id, void *val)
{
    if (d != NULL && d->ops.get_param != NULL)
        return d->ops.get_param(d, param_id, val);
    return false;
}

auEncoder_t *auEncoderCreate(struct auWriter *w, auStreamFormat_t format)
{
    switch (format)
    {
    case AUSTREAM_FORMAT_PCM:
        return (auEncoder_t *)auPcmEncoderCreate(w);

    case AUSTREAM_FORMAT_WAVPCM:
        return (auEncoder_t *)auWavEncoderCreate(w);

#ifdef CONFIG_AUDIO_AMRNB_ENC_ENABLE
    case AUSTREAM_FORMAT_AMRNB:
        return (auEncoder_t *)auAmrnbEncoderCreate(w);
#endif

#ifdef CONFIG_AUDIO_AMRWB_ENC_ENABLE
    case AUSTREAM_FORMAT_AMRWB:
        return (auEncoder_t *)auAmrwbEncoderCreate(w);
#endif

    default:
        OSI_LOGE(0, "audio encoder unknown fotmat/%d", format);
        return NULL;
    }
}
