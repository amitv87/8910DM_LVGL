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

#include "audio_types.h"
#include <string.h>

const auAmrnbMode_t gAmrnbEncodeDefaultMode = AU_AMRNB_MODE_795;
const auAmrwbMode_t gAmrwbEncodeDefaultMode = AU_AMRWB_MODE_1585;
const uint16_t gDtmfToneFreq[16][2] = {
    {941, 1336}, // 0
    {697, 1209}, // 1
    {697, 1336}, // 2
    {697, 1447}, // 3
    {770, 1209}, // 4
    {770, 1336}, // 5
    {770, 1447}, // 6
    {852, 1209}, // 7
    {852, 1336}, // 8
    {852, 1447}, // 9
    {697, 1633}, // A
    {770, 1633}, // B
    {852, 1633}, // C
    {941, 1633}, // D
    {941, 1447}, // #
    {941, 1209}, // *
};

bool auSampleFormatMatch(const auFrame_t *a, const auFrame_t *b)
{
    return (a->sample_format == b->sample_format &&
            a->channel_count == b->channel_count &&
            a->sample_rate == b->sample_rate);
}

void auSampleFormatCopy(auFrame_t *dest, const auFrame_t *src)
{
    dest->sample_format = src->sample_format;
    dest->channel_count = src->channel_count;
    dest->sample_rate = src->sample_rate;
}

void auInitSampleFormat(auFrame_t *frame)
{
    frame->sample_format = AUSAMPLE_FORMAT_S16;
    frame->channel_count = 1;
    frame->sample_rate = 8000;
}

unsigned auFrameByteToTime(const auFrame_t *frame, unsigned bytes)
{
    unsigned block_size = AUFRAME_BYTE_PER_BLOCK(frame);
    return OSI_DIV_ROUND((bytes / block_size) * 1000, frame->sample_rate);
}

unsigned auFrameTimeToByte(const auFrame_t *frame, unsigned ms)
{
    unsigned block_size = AUFRAME_BYTE_PER_BLOCK(frame);
    return OSI_DIV_ROUND(ms * frame->sample_rate, 1000) * block_size;
}

unsigned auFrameStartTime(const auFrame_t *frame)
{
    return OSI_DIV_ROUND(frame->sample_stamp * 1000, frame->sample_rate);
}

auStreamFormat_t auStreamFormatBySuffix(const char *fname)
{
    if (fname == NULL)
        return AUSTREAM_FORMAT_UNKNOWN;

    char *dot = strrchr(fname, '.');
    if (dot == NULL)
        return AUSTREAM_FORMAT_UNKNOWN;

    if (strcasecmp(dot, ".pcm") == 0)
        return AUSTREAM_FORMAT_PCM;

    if (strcasecmp(dot, ".wav") == 0)
        return AUSTREAM_FORMAT_WAVPCM;

    if (strcasecmp(dot, ".mp3") == 0)
        return AUSTREAM_FORMAT_MP3;

    if (strcasecmp(dot, ".amr") == 0)
        return AUSTREAM_FORMAT_AMRNB;

    if (strcasecmp(dot, ".awb") == 0)
        return AUSTREAM_FORMAT_AMRWB;

    if (strcasecmp(dot, ".sbc") == 0)
        return AUSTREAM_FORMAT_SBC;

    return AUSTREAM_FORMAT_UNKNOWN;
}
