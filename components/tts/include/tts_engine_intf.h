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

#ifndef _TTS_ENGINE_INTF_H_
#define _TTS_ENGINE_INTF_H_

#include "osi_compiler.h"
#include "tts_config.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief opaque data structure for TTS engine interface
 */
typedef struct ttsEngineIntf ttsEngineIntf_t;

/**
 * \brief create TTS engine interface
 *
 * \return
 *      - TTS engine interface instance
 *      - NULL on error, out of memory
 */
ttsEngineIntf_t *ttsEngineIntfCreate(void);

/**
 * \brief delete TTS engine interface
 *
 * When \p d is NULL, nothing will be done.
 *
 * \param d     TTS engine interface instance, must be valid
 */
void ttsEngineIntfDelete(ttsEngineIntf_t *d);

/**
 * \brief synthesis text
 *
 * The character encoding of \p text is UTF-8. When \p size is greater than
 * or equal to 0, \p text should be NUL terminated and the string length
 * will be used.
 *
 * It is blocking, that is, it will return after all text synthesis is done.
 * During that, \p ttsOutputPcmData can be called to output sythesis PCM
 * data. When \p ttsOutputPcmData returns false, it shall return false
 * immediately.
 *
 * \param d     TTS engine interface instance, must be valid
 * \param text  text in UTF-8
 * \param size  text size, NUL terminated length will be used when <= 0
 * \return
 *      - true on success
 *      - false on fail
 */
bool ttsEngineIntfSynthText(ttsEngineIntf_t *d, const void *text, int size);

OSI_EXTERN_C_END
#endif
