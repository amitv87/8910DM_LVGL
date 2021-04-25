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

#ifndef _TTS_PLAYER_H_
#define _TTS_PLAYER_H_

#include "osi_compiler.h"
#include "tts_config.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief initialize tts player
 *
 * TTS player is designed as singleton.
 */
void ttsPlayerInit(void);

/**
 * \brief TTS play text
 *
 * This is asynchronized, that is it will return immediately after the play
 * event is sent.
 *
 * When \p len is -1, the actual text size will be determined by terminating
 * NUL character.
 *
 * \p text is the string to be played, encoded with \p encoding. Refer to
 * ml.h for the value of \p encoding. Not all encodings are supported.
 *
 * \param text      text to be played
 * \param len       text len, -1 for terminated with NUL
 * \param encoding  text encoding
 * \return
 *      - true on success
 *      - false on failed
 */
bool ttsPlayText(const void *text, int len, unsigned encoding);

/**
 * \brief stop tts play
 */
void ttsStop(void);

/**
 * \brief whether tts is playing
 *
 * \return
 *      - true if there are text in playing
 *      - false if no text in playing
 */
bool ttsIsPlaying(void);

/**
 * \brief output tts pcm data to output
 *
 * It is designed to be called by TTS engine, don't call it in application.
 * When parameter \p size is 0, it indicates the end of pcm data.
 *
 * \param data      pcm data, can't be NULL when \p size is not 0
 * \param size      pcm data byte count
 * \return
 *      - true if all pcm data are output
 *      - false on error, such as tts player is about to be stopped
 */
bool ttsOutputPcmData(const void *data, unsigned size);

OSI_EXTERN_C_END

#endif
