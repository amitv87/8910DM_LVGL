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

#ifndef _AUDIO_RECORDER_H_
#define _AUDIO_RECORDER_H_

#include "osi_compiler.h"
#include "audio_types.h"
#include "audio_encoder.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief forward declaration
 */
struct auWriter;

/**
 * \brief forward declaration
 */
struct osiPipe;

/**
 * \brief audio recorder status
 */
typedef enum
{
    AURECORDER_STATUS_IDLE,     ///< recorder not started
    AURECORDER_STATUS_RECORD,   ///< recorder is started
    AURECORDER_STATUS_PAUSE,    ///< recorder is paused
    AURECORDER_STATUS_FINISHED, ///< recorder is finished
} auRecorderStatus_t;

/**
 * \brief audio recorder event
 *
 * There are no separated event for error. When fatal errors occur, audio
 * recorder will be stopped automatically.
 */
typedef enum
{
    AURECORDER_EVENT_FINISHED = 1, ///< voice call finished, or fatal error
} auRecorderEvent_t;

/**
 * \brief opaque data struct of audio recorder
 */
typedef struct auRecorder auRecorder_t;

/**
 * \brief audio recorder callback prototype
 *
 * Audio recorder callback will be invoked in audio thread. <em>Don't call
 * audio recorder APIs, especially auRecorderDelete inside the callvack.</em>
 * Usually, it should just notify another thread to handle the event.
 *
 * \param param         audio recorder callback context
 * \param event         audio recorder event
 */
typedef void (*auRecorderEventCallback_t)(void *param, auRecorderEvent_t event);

/**
 * \brief create an audio recorder
 *
 * \return
 *      - created audio recorder
 *      - NULL if out of memory
 */
auRecorder_t *auRecorderCreate(void);

/**
 * \brief delete the audio recorder
 *
 * It is recommended to call \p auRecorderStop before. In case recording
 * is started, it will try to stop recording in audio device.
 *
 * \param d             audio recoder instance
 */
void auRecorderDelete(auRecorder_t *d);

/**
 * \brief set audio recorder event callback
 *
 * If there are callback already registered, the previous one will be
 * replaced.
 *
 * \param d             the audio recorder
 * \param cb            event callback
 * \param cb_ctx        event callback context
 */
void auRecorderSetEventCallback(auRecorder_t *d, auRecorderEventCallback_t cb, void *cb_ctx);

/**
 * \brief get audio recorder recording type
 *
 * When audio player is not started, return \p AUDEV_RECORD_TYPE_NONE.
 *
 * \param d             audio recoder instance
 * \return
 *      - recording type
 */
audevRecordType_t auRecorderGetType(auRecorder_t *d);

/**
 * \brief check whether recorder is running
 *
 * Recorder will be paused implicitly.
 *
 * \return
 *      - 0 if recorder is stop
 *      - 1 if recorder is running
 *      - 2 if recorder is pause
 *      - 3 if recorder is finish
 */
auRecorderStatus_t auRecorderGetStatus(auRecorder_t *d);

/**
 * \brief start audio recoder
 *
 * Before calling this, \p encoder should be ready for encoding audio
 * frame.
 *
 * Audio recoder will call \p auEncoderEncode inside the audio device
 * thread. It should be fast enough. When it takes too long, it is
 * possible lose input audio samples, and even worse, may trigger
 * assert. The audio writer should be considered also. For example,
 * when encoded data will be write to SFFS (which is very slow), it
 * is suggest to use \p auPipeWriter_t.
 *
 * \param d             audio recorder instance
 * \param type          audio recording type
 * \param encoder       encoder for the audio recorder
 * \return
 *      - true on success
 *      - false on failed
 */
bool auRecorderStartEncoder(auRecorder_t *d, audevRecordType_t type, auEncoder_t *encoder);

/**
 * \brief start audio recoder, and write to file
 *
 * Inside, recorder will manage encoder and writer. And it is provided
 * for simpler usage.
 *
 * The write performace of file system on NOR flash may be not enough for
 * audio recorder, especially for PCM or WAV. It will cause underflow,
 * and even assert. So, don't write recorded PCM to NOR flash file system
 * directly.
 *
 * When \p params is not NULL, it should be an array. And the \p id of
 * the last element must be zero to indicate the end.
 *
 * \param d             audio recorder instance
 * \param type          audio recording type
 * \param format        recorded stream format
 * \param params        encoder parameters
 * \param fname         file name
 * \return
 *      - true on success
 *      - false on failed
 */
bool auRecorderStartFile(auRecorder_t *d, audevRecordType_t type, auStreamFormat_t format,
                         const auEncoderParamSet_t *params, const char *fname);

/**
 * \brief start audio recoder, and write to pipe
 *
 * Inside, recorder will manage encoder and writer. And it is provided
 * for simpler usage.
 *
 * When \p params is not NULL, it should be an array. And the \p id of
 * the last element must be zero to indicate the end.
 *
 * \param d             audio recorder instance
 * \param type          audio recording type
 * \param format        recorded stream format
 * \param params        encoder parameters
 * \param pipe          pipe for audio pipe write
 * \return
 *      - true on success
 *      - false on failed
 */
bool auRecorderStartPipe(auRecorder_t *d, audevRecordType_t type, auStreamFormat_t format,
                         const auEncoderParamSet_t *params, struct osiPipe *pipe);

/**
 * \brief start audio recoder, and write to memory
 *
 * Inside, recorder will manage encoder and writer. And it is provided
 * for simpler usage.
 *
 * When \p params is not NULL, it should be an array. And the \p id of
 * the last element must be zero to indicate the end.
 *
 * \param d             audio recorder instance
 * \param type          audio recording type
 * \param format        recorded stream format
 * \param params        encoder parameters
 * \param max_size      maximum size for audio memory write
 * \return
 *      - true on success
 *      - false on failed
 */
bool auRecorderStartMem(auRecorder_t *d, audevRecordType_t type, auStreamFormat_t format,
                        const auEncoderParamSet_t *params, unsigned max_size);

/**
 * \brief start audio recoder, and write to external writer
 *
 * Inside, recorder will manage encoder. Writer should be managed externally,
 * It is provided for simpler usage.
 *
 * When \p params is not NULL, it should be an array. And the \p id of
 * the last element must be zero to indicate the end.
 *
 * \param d             audio recorder instance
 * \param type          audio recording type
 * \param format        recorded stream format
 * \param params        encoder parameters
 * \param writer        external writer to be used
 * \return
 *      - true on success
 *      - false on failed
 */
bool auRecorderStartWriter(auRecorder_t *d, audevRecordType_t type, auStreamFormat_t format,
                           const auEncoderParamSet_t *params, struct auWriter *writer);

/**
 * \brief audio recorder pause
 *
 * At pause, the audio input device will be releases, and the audio device
 * can be used for other purpose.
 *
 * \param d 			the audio recorder
 * \return
 *		- true on success
 *		- false on invalid parameter
 */

bool auRecorderPause(auRecorder_t *d);

/**
 * \brief audio recorder resume
 *
 * \param d 			the audio recorder
 * \return
 *		- true on success
 *		- false on invalid parameter
 */

bool auRecorderResume(auRecorder_t *d);

/**
 * \brief stop audio recorder
 *
 * After stop, internal resources of audio recorder may be freed.
 *
 * \param d             audio recorder instance
 * \return
 *      - true on success
 *      - false on invalid parameter
 */
bool auRecorderStop(auRecorder_t *d);

/**
 * \brief get the audio encoder in using
 *
 * When audio recorder is not started, it may return NULL.
 *
 * \param d             audio recorder instance
 * \return
 *      - audio encoder handler
 *      - NULL if audio recorder not started
 */
auEncoder_t *auRecorderGetEncoder(auRecorder_t *d);

/**
 * \brief get the audio writer in using
 *
 * When audio recorder is not started, or audio writer is not managed
 * by audio recorder, it may return NULL.
 *
 * \param d             audio recorder instance
 * \return
 *      - audio writer handler
 *      - NULL if audio recorder not started, or not managed
 */
struct auWriter *auRecorderGetWriter(auRecorder_t *d);

/**
 * \brief fix wav header by file name
 *
 * In the wav file header, there are information about the data size.
 * In some cases, such as recording with pipe, seek is not supported.
 * So, the header of wav file is incorrect. This will try to fix the
 * header size information.
 *
 * It will check the wav header, it will change the data size base on
 * file size. And it requires there are only RIFF and DAT in the
 * beginning, and assuming all of the rest are data.
 *
 * \param fname     file name to be fixed
 * \return
 *      - true on success
 *      - false on fail, RIFF and DAT check fail
 */
bool auFixWavHeaderByName(const char *fname);

/**
 * \brief fix wav header by file descriptor
 *
 * It is the same as \p auFixWavHeaderByName, just the parameter is file
 * descriptor rather than file name.
 *
 * \param fd        file descriptor to be fixed
 * \return
 *      - true on success
 *      - false on fail, RIFF and DAT check fail
 */
bool auFixWavHeaderByDesc(int fd);

OSI_EXTERN_C_END
#endif
