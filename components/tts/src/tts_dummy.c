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

#include "tts_player.h"
#ifdef CONFIG_TTS_DUMMY_SUPPORT

#include "osi_log.h"
#include "osi_api.h"
#include "audio_tonegen.h"
#include "audio_types.h"
#include "tts_engine_intf.h"
#include <string.h>
#include <stdlib.h>

#define TTS_DUMMY_TONE_BYTES (256)
#define TTS_DUMMY_TONE_DURATION (90)
#define TTS_DUMMY_TONE_SILENCE (210)
#define TTS_DUMMY_TONE_GAIN (0x2000) // -6db

struct ttsEngineIntf
{
    unsigned dummy;
};

ttsEngineIntf_t *ttsEngineIntfCreate(void)
{
    ttsEngineIntf_t *d = (ttsEngineIntf_t *)calloc(1, sizeof(ttsEngineIntf_t));
    return d;
}

void ttsEngineIntfDelete(ttsEngineIntf_t *d)
{
    free(d);
}

static bool prvSendTone(auToneGen_t *tonegen, unsigned tone)
{
    short samples[TTS_DUMMY_TONE_BYTES / 2];
    auFrame_t frame;
    frame.data = (uintptr_t)samples;

    OSI_LOGI(0, "tts dummy, tone %d", tone);

    if (!auToneGenStart(tonegen, TTS_DUMMY_TONE_GAIN,
                        gDtmfToneFreq[tone][0], gDtmfToneFreq[tone][1],
                        TTS_DUMMY_TONE_DURATION, TTS_DUMMY_TONE_SILENCE))
        return false;

    for (;;)
    {
        frame.bytes = 0;
        int bytes = auToneGenSamples(tonegen, &frame, TTS_DUMMY_TONE_BYTES);
        if (bytes <= 0)
            break;

        if (!ttsOutputPcmData(samples, bytes))
            return false;
    }
    return true;
}

bool ttsEngineIntfSynthText(ttsEngineIntf_t *d, const void *text, int size)
{
    if (d == NULL || text == NULL)
        return false;

    const char *str = (const char *)text;
    if (size < 0)
        size = strlen(str);
    if (size == 0)
        return true;

    auToneGen_t *tonegen = auToneGenCreate(CONFIG_TTS_PCM_SAMPLE_RATE);
    while (size-- > 0)
    {
        int tone = (unsigned)(*str++) & 0xf;
        if (!prvSendTone(tonegen, tone))
        {
            auToneGenDelete(tonegen);
            return false;
        }
    }

    auToneGenDelete(tonegen);
    return true;
}

#endif
