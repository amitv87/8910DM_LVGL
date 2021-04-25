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
#ifdef CONFIG_TTS_SUPPORT

#include "osi_log.h"
#include "osi_api.h"
#include "osi_pipe.h"
#include "audio_player.h"
#include "ml.h"
#include "tts_engine_intf.h"
#include <string.h>
#include <stdlib.h>

#define TTS_PIPE_SIZE (2048)
#define TTS_WRITE_PIPE_TMEOUT (500)
#define TTS_THREAD_PRIORITY (OSI_PRIORITY_ABOVE_NORMAL)

typedef struct
{
    osiPipe_t *pipe;
    osiWorkQueue_t *wq;
    osiWork_t *play_work;
    osiMutex_t *lock;
    auPlayer_t *player;
    void *playtext;
} ttsContext_t;

static ttsContext_t gTtsCtx;

static void prvttsPlayerDelete(ttsContext_t *d);

static void prvPlay(void *param)
{
    ttsContext_t *d = (ttsContext_t *)param;

    ttsEngineIntf_t *impl = ttsEngineIntfCreate();
    if (impl == NULL)
        goto failed_nomem;

    if (!ttsEngineIntfSynthText(impl, d->playtext, -1))
    {
        OSI_LOGE(0, "tts synth failed, stop pipe");
        osiPipeStop(d->pipe);
        goto failed;
    }

    osiPipeSetEof(d->pipe);
    auPlayerWaitFinish(d->player, OSI_WAIT_FOREVER);
    osiPipeStop(d->pipe);
    ttsEngineIntfDelete(impl);
    prvttsPlayerDelete(d);
    return;

failed_nomem:
    OSI_LOGI(0, "prvPlay failed_nomem");
    ttsEngineIntfDelete(impl);
    prvttsPlayerDelete(d);
    return;

failed:
    OSI_LOGI(0, "prvPlay failed");
    ttsEngineIntfDelete(impl);
    return;
}

static void prvttsPlayerDelete(ttsContext_t *d)
{
    OSI_LOGI(0, "ttsPlayer Delete");
    auPlayerDelete(d->player);
    osiPipeDelete(d->pipe);
    free(d->playtext);
    osiWorkDelete(d->play_work);
    d->player = NULL;
    d->pipe = NULL;
    d->playtext = NULL;
    d->play_work = NULL;
}

static bool prvttsPlayerStart(ttsContext_t *d, const void *text, int size, unsigned encoding)
{
    osiMutexLock(d->lock);

    if ((d->player != NULL) || (d->playtext != NULL))
    {
        OSI_LOGE(0, "ttsPlayer is busy");
        osiMutexUnlock(d->lock);
        return false;
    }

    if (size < 0)
        size = strlen((const char *)text);

    if (encoding == ML_UTF8)
    {
        d->playtext = (char *)calloc(1, size + 1);
        if (d->playtext == NULL)
            goto failed_nomem;

        memcpy(d->playtext, text, size);
    }
    else
    {
        d->playtext = (char *)mlConvertStr(text, size, encoding, ML_UTF8, NULL);
        if (d->playtext == NULL)
            goto failed_nomem;
    }
    OSI_LOGI(0, "tts play size/%d encoding/%d utf8/%d", size, encoding, strlen(d->playtext));

    d->pipe = osiPipeCreate(TTS_PIPE_SIZE);
    if (d->pipe == NULL)
        goto failed_nomem;

    d->player = auPlayerCreate();
    if (d->player == NULL)
        goto failed_nomem;

    d->play_work = osiWorkCreate(prvPlay, NULL, d);
    if (d->play_work == NULL)
        goto failed_nomem;

    auFrame_t frame = {
        .sample_format = AUSAMPLE_FORMAT_S16,
        .channel_count = 1,
        .sample_rate = CONFIG_TTS_PCM_SAMPLE_RATE,
    };
    auDecoderParamSet_t params[2] = {{AU_DEC_PARAM_FORMAT, &frame}, {0}};

    if (!auPlayerStartPipe(d->player, AUSTREAM_FORMAT_PCM, params, d->pipe))
        goto failed;

    osiWorkEnqueue(d->play_work, d->wq);
    osiMutexUnlock(d->lock);
    return true;

failed_nomem:
    OSI_LOGE(0, "prvttsPlayerStart failed_nomem");
    osiMutexUnlock(d->lock);
    prvttsPlayerDelete(d);
    return false;
failed:
    OSI_LOGE(0, "prvttsPlayerStart failed");
    osiMutexUnlock(d->lock);
    prvttsPlayerDelete(d);
    return false;
}

void ttsPlayerInit(void)
{
    OSI_LOGI(0, "ttsPlayerInit");
    ttsContext_t *d = &gTtsCtx;
    d->wq = osiWorkQueueCreate("tts", 1, TTS_THREAD_PRIORITY, CONFIG_TTS_STACK_SIZE);
    if (d->wq == NULL)
    {
        osiPanic();
    }
    d->lock = osiMutexCreate();
    if (d->lock == NULL)
    {
        osiPanic();
    }
}

bool ttsPlayText(const void *text, int size, unsigned encoding)
{
    if (text == NULL || size == 0)
        return false;
    return (prvttsPlayerStart(&gTtsCtx, text, size, encoding));
}

bool ttsIsPlaying(void)
{
    ttsContext_t *d = &gTtsCtx;
    return (d->playtext != NULL);
}

void ttsStop(void)
{
    OSI_LOGI(0, "tts stop");
    ttsContext_t *d = &gTtsCtx;
    if (d->player == NULL)
        return;

    osiMutexLock(d->lock);
    osiPipeSetEof(d->pipe);
    osiPipeStop(d->pipe);
    prvttsPlayerDelete(d);
    osiMutexUnlock(d->lock);
    return;
}

bool ttsOutputPcmData(const void *data, unsigned size)
{
    ttsContext_t *d = &gTtsCtx;
    if (d->pipe == NULL)
        return false;

    int written = osiPipeWriteAll(d->pipe, data, size, TTS_WRITE_PIPE_TMEOUT);
    OSI_LOGI(0, "pcm write size/%d written/%d", size, written);
    return (written == size);
}

#endif
