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

#ifndef _AUDIO_PLAYER_H_
#define _AUDIO_PLAYER_H_

#include "osi_compiler.h"
#include "audio_types.h"
#include "audio_decoder.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief forward declaration
 */
struct auReader;

/**
 * \brief forward declaration
 */
struct osiPipe;

/**
 * \brief audio player status
 */
typedef enum
{
    AUPLAYER_STATUS_IDLE,     ///< playback not started
    AUPLAYER_STATUS_PLAY,     ///< playback is started
    AUPLAYER_STATUS_PAUSE,    ///< playback is paused
    AUPLAYER_STATUS_FINISHED, ///< playback is finished
} auPlayerStatus_t;

/**
 * \brief audio player event
 *
 * There are no separated event for error. When fatal errors occur, audio
 * player will be stopped automatically.
 */
typedef enum
{
    AUPLAYER_EVENT_FINISHED = 1, ///< playback finished, or fatal error
} auPlayerEvent_t;

/**
 * \brief opaque data struct for audio player
 */
typedef struct auPlayer auPlayer_t;

/**
 * \brief audio player callback prototype
 *
 * Audio player callback will be invoked in audio thread. <em>Don't call
 * audio player APIs, especially auPlayerDelete inside the callvack.</em>
 * Usually, it should just notify another thread to handle the event.
 *
 * \param param         audio player callback context
 * \param event         audio player event
 */
typedef void (*auPlayerEventCallback_t)(void *param, auPlayerEvent_t event);

/**
 * \brief create an audio player
 *
 * \return
 *      - audio player
 *      - NULL if out of memory
 */
auPlayer_t *auPlayerCreate(void);

/**
 * \brief delete the audio player
 *
 * It is recommended to call \p auPlayerStop before. In case playing
 * is started, it will try to stop playing in audio device.
 *
 * \param d             the audio player
 */
void auPlayerDelete(auPlayer_t *d);

/**
 * \brief set audio player event callback
 *
 * If there are callback already registered, the previous one will be
 * replaced.
 *
 * \param d             the audio player
 * \param cb            event callback
 * \param cb_ctx        event callback context
 */
void auPlayerSetEventCallback(auPlayer_t *d, auPlayerEventCallback_t cb, void *cb_ctx);

/**
 * \brief start audio player
 *
 * Before calling this, \p decoder should be ready for decoding audio
 * frame.
 *
 * \param d             audio player instance
 * \param decoder       decoder for the audio player
 * \return
 *      - true on success
 *      - false on failed
 */
bool auPlayerStartDecoder(auPlayer_t *d, auDecoder_t *decoder);

/**
 * \brief start audio player
 *
 * Before calling this, \p decoder should be ready for decoding audio
 * frame.
 *
 * \param d             audio player instance
 * \param type          audio playing type
 * \param decoder       decoder for the audio player
 * \return
 *      - true on success
 *      - false on failed
 */
bool auPlayerStartDecoderV2(auPlayer_t *d, audevPlayType_t type, auDecoder_t *decoder);

/**
 * \brief start file play
 *
 * For simplicity, the stream format shall be explicitly specified.
 *
 * \param d             the audio player
 * \param format        stream format
 * \param fname         file name to be played
 * \return
 *      - true on player start success
 *      - false on failed
 */
bool auPlayerStartFile(auPlayer_t *d, auStreamFormat_t format,
                       const auDecoderParamSet_t *params,
                       const char *fname);

/**
 * \brief start file play
 *
 * For simplicity, the stream format shall be explicitly specified.
 *
 * \param d             the audio player
 * \param type          audio playing type
 * \param format        stream format
 * \param fname         file name to be played
 * \return
 *      - true on player start success
 *      - false on failed
 */
bool auPlayerStartFileV2(auPlayer_t *d, audevPlayType_t type, auStreamFormat_t format,
                         const auDecoderParamSet_t *params,
                         const char *fname);

/**
 * \brief start memory play
 *
 * Before player start, the whole stream should be located in memory. And
 * the content shouldn't be changed during playback.
 *
 * \param d             the audio player
 * \param format        stream format
 * \param mem           memory to be played
 * \param size          memory size
 * \return
 *      - true on player start success
 *      - false on failed
 */
bool auPlayerStartMem(auPlayer_t *d, auStreamFormat_t format,
                      const auDecoderParamSet_t *params,
                      const void *buf, unsigned size);

/**
 * \brief start memory play
 *
 * Before player start, the whole stream should be located in memory. And
 * the content shouldn't be changed during playback.
 *
 * \param d             the audio player
 * \param type          audio playing type
 * \param format        stream format
 * \param mem           memory to be played
 * \param size          memory size
 * \return
 *      - true on player start success
 *      - false on failed
 */
bool auPlayerStartMemV2(auPlayer_t *d, audevPlayType_t type, auStreamFormat_t format,
                        const auDecoderParamSet_t *params,
                        const void *buf, unsigned size);

/**
 * \brief start pipe play
 *
 * \param d             the audio player
 * \param format        stream format
 * \param pipe          the pipe to be read from
 * \return
 *      - true on player start success
 *      - false on failed
 */
bool auPlayerStartPipe(auPlayer_t *d, auStreamFormat_t format,
                       const auDecoderParamSet_t *params,
                       struct osiPipe *pipe);

/**
 * \brief start pipe play
 *
 * \param d             the audio player
 * \param type          audio playing type
 * \param format        stream format
 * \param pipe          the pipe to be read from
 * \return
 *      - true on player start success
 *      - false on failed
 */
bool auPlayerStartPipeV2(auPlayer_t *d, audevPlayType_t type, auStreamFormat_t format,
                         const auDecoderParamSet_t *params,
                         struct osiPipe *pipe);

/**
 * \brief start play from an external audio reader
 *
 * It is used for playback from neither file, nor memory. The procedure to
 * get stream data is implemented externally.
 *
 * Player will just use \p reader, and the life-cycle should be handled
 * eternally.
 *
 * \param d             the audio player
 * \param format        stream format
 * \param reader        audio stream reader
 * \return
 *      - true on player start success
 *      - false on failed
 */
bool auPlayerStartReader(auPlayer_t *d, auStreamFormat_t format,
                         const auDecoderParamSet_t *params,
                         struct auReader *reader);

/**
 * \brief start play from an external audio reader
 *
 * It is used for playback from neither file, nor memory. The procedure to
 * get stream data is implemented externally.
 *
 * Player will just use \p reader, and the life-cycle should be handled
 * eternally.
 *
 * \param d             the audio player
 * \param type          audio playing type
 * \param format        stream format
 * \param reader        audio stream reader
 * \return
 *      - true on player start success
 *      - false on failed
 */
bool auPlayerStartReaderV2(auPlayer_t *d, audevPlayType_t type, auStreamFormat_t format,
                           const auDecoderParamSet_t *params,
                           struct auReader *reader);

/**
 * \brief playback pause
 *
 * At pause, the audio output device will be releases, and the audio device
 * can be used for other purpose.
 *
 * \param d             the audio player
 * \return
 *      - true on success
 *      - false on invalid parameter
 */
bool auPlayerPause(auPlayer_t *d);

/**
 * \brief playback resume
 *
 * \param d             the audio player
 * \return
 *      - true on success
 *      - false on invalid parameter
 */
bool auPlayerResume(auPlayer_t *d);

/**
 * \brief playback stop
 *
 * After stop, internal resources of audio player may be freed.
 *
 * \param d             the audio player
 * \return
 *      - true on success
 *      - false on invalid parameter
 */
bool auPlayerStop(auPlayer_t *d);

/**
 * \brief wait playback finish
 *
 * \p timeout can be \p OSI_WAIT_FOREVER for wait without timeout.
 * If audio player is already in \p AUPLAYER_STATUS_IDLE status, it will
 * return immediately.
 *
 * Pay attention to call this in \p AUPLAYER_STATUS_PAUSE status. It may
 * cause caller to wait forever (or timeout), if there are no other thread
 * to call \p auPlayerResume.
 *
 * \param d             the audio player
 * \return
 *      - true on playback finished
 *      - false on timeout
 */
bool auPlayerWaitFinish(auPlayer_t *d, unsigned timeout);

/**
 * \brief playback elapsed time in milliseconds
 *
 * When player is in \p AUPLAYER_STATUS_IDLE status, it will return 0.
 *
 * \param d             the audio player
 * \return
 *      - playback elapsed time in milliseconds
 *      - 0 on error
 */
unsigned auPlayerPlayTime(auPlayer_t *d);

/**
 * \brief get audio player playing type
 *
 * When audio player is not started, return \p AUDEV_PLAY_TYPE_NONE.
 *
 * \param d             audio player instance
 * \return
 *      - playing type
 */
audevPlayType_t auPlayerGetType(auPlayer_t *d);

/**
 * \brief check whether player is running
 *
 * Player will be paused implicitly. The typical case is: when there is a MT
 * call, active player will be paused implicitly at accepting the MT call.
 *
 * \return
 *      - true if player is running
 *      - false on otherwise
 */
auPlayerStatus_t auPlayerGetStatus(auPlayer_t *d);

/**
 * \brief get the audio decoder in using
 *
 * When audio player is not started, it may return NULL.
 *
 * \param d             the audio player
 * \return
 *      - audio decoder handler
 *      - NULL if audio player not started
 */
auDecoder_t *auPlayerGetDecoder(auPlayer_t *d);

/**
 * \brief get the audio reader in using
 *
 * When audio player is not started, or audio reader is not managed
 * by audio player, it may return NULL.
 *
 * \param d             the audio player
 * \return
 *      - audio reader handler
 *      - NULL if audio player not started, or not managed
 */
struct auReader *auPlayerGetReader(auPlayer_t *d);

OSI_EXTERN_C_END
#endif
