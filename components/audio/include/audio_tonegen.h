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

#ifndef _AUDIO_TONEGEN_H_
#define _AUDIO_TONEGEN_H_

#include "audio_types.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief opaque data structure for audio tone generator
 */
typedef struct auToneGen auToneGen_t;

/**
 * \brief create audio tone generator
 *
 * The sample format of audio tone generator is \p AUSAMPLE_FORMAT_S16,
 * the channel count is 1.
 *
 * \param sample_rate   sample rate of generated samples
 * \return
 *      - audio tone generator instance
 *      - NULL on fail, no memory or invalid parameter
 */
auToneGen_t *auToneGenCreate(unsigned sample_rate);

/**
 * \brief delete the audio tone generator instance
 *
 * \param d             audio tone generator instance
 */
void auToneGenDelete(auToneGen_t *d);

/**
 * \brief start audio tone generator
 *
 * The format of \p gain is 1.14. That is 0x4000 is for full range
 * without attenuation, inrange of [32767, -32768].
 *
 * Audio tone generator can support one frequency or dual frequencies.
 * For single frequency, \p freq2 should be the same with \p freq1.
 * Both of \p freq1 and \p frea2 can not be zero, and must be less than
 * sample rate.
 *
 * \p period is the period of tone, in milliseconds, \p silence is the
 * period of silence after tone. \p period can not be zero, and
 * \p silence can be zero.
 *
 * When \p period or \p silence is \p OSI_DELAY_MAX, it will never
 * finish.
 *
 * If audio tone generator is already started, this will restart audio
 * tone generator.
 *
 * \param d             audio tone generator instance
 * \param gain          gain of generated samples
 * \param freq1         the first frequency
 * \param freq2         the second frequency
 * \param period        tone period in milliseconds
 * \param silence       silence period in milliseconds
 * \return
 *      - true on success
 *      - false on fail, invalid parameters
 */
bool auToneGenStart(auToneGen_t *d, unsigned gain, unsigned freq1, unsigned freq2,
                    unsigned period, unsigned silence);

/**
 * \brief get audio tone samples
 *
 * When period (both tone period and silence period) is finished, it
 * will set \p frame->flags to \p AUFRAME_FLAG_END, and return true.
 *
 * This will fill sample into \p frame->data+frame->bytes. The maximum
 * byte count to be filled is \p bytes. Caller should avoid memory
 * overflow.
 *
 * On success, \p frame->bytes will be updated and format parameters
 * will be filled.
 *
 * \param d             audio tone generator instance
 * \param frame         audio frame
 * \param bytes         maximum byte count to be filled
 * \return
 *      - filled byte count
 *      - -1 on fail, invalid parameter
 */
int auToneGenSamples(auToneGen_t *d, auFrame_t *frame, unsigned bytes);

OSI_EXTERN_C_END
#endif
