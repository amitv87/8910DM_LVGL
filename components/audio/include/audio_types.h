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

#ifndef _AUDIO_TYPES_H_
#define _AUDIO_TYPES_H_

#include "osi_api.h"
#include "audio_config.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief audio sample format
 */
typedef enum
{
    AUSAMPLE_FORMAT_S16, ///< int16_t, native endian (little endian)
} ausampleFormat_t;

/**
 * \brief audio stream format
 */
typedef enum
{
    AUSTREAM_FORMAT_UNKNOWN, ///< placeholder for unknown format
    AUSTREAM_FORMAT_PCM,     ///< raw PCM data
    AUSTREAM_FORMAT_WAVPCM,  ///< WAV, PCM inside
    AUSTREAM_FORMAT_MP3,     ///< MP3
    AUSTREAM_FORMAT_AMRNB,   ///< AMR-NB
    AUSTREAM_FORMAT_AMRWB,   ///< AMR_WB
    AUSTREAM_FORMAT_SBC,     ///< bt SBC
} auStreamFormat_t;

/**
 * \brief flags in audio frame
 */
typedef enum
{
    AUFRAME_FLAG_START = (1 << 0), ///< indicate start of stream
    AUFRAME_FLAG_END = (1 << 1),   ///< indicate end of stream
} auframeFlags_t;

/**
 * \brief audio frame
 *
 * When it is used for format only, only \p sample_format, \p channel_count
 * and \p sample_rate are used.
 */
typedef struct
{
    /**
     * sample format
     */
    ausampleFormat_t sample_format;
    /**
     * channel count, only 1 and 2 are valid now
     */
    uint8_t channel_count;
    /**
     * audio frame flags
     */
    uint8_t flags;
    /**
     * sample rate in Hz
     */
    unsigned long long sample_rate;
    /**
     * time stamp of the beginning of data, in unit of sample count.
     * <tt>time_stamp = sample_stamp/sample_rate</tt>
     */
    unsigned long long sample_stamp;
    /**
     * sample pointer, it can be casted to the data type according to \p sample_format.
     * For 2 channels, the format is LRLR...
     */
    uintptr_t data;
    /**
     * byte count (not sample count)
     */
    unsigned bytes;
} auFrame_t;

#define AUFRAME_BIT_PER_SAMPLE(frame) (16)
#define AUFRAME_BIT_PER_BLOCK(frame) (AUFRAME_BIT_PER_SAMPLE(frame) * (frame)->channel_count)
#define AUFRAME_BYTE_PER_BLOCK(frame) ((AUFRAME_BIT_PER_SAMPLE(frame) / 8) * (frame)->channel_count)

#define AUDIO_WAV_RIFF_ID (0x46464952) // "RIFF"
#define AUDIO_WAV_WAVE_ID (0x45564157) // "WAVE"
#define AUDIO_WAV_FMT_ID (0x20746d66)  // "fmt "
#define AUDIO_WAV_DATA_ID (0x61746164) // "data"

/**
 * \brief wav header
 */
typedef struct
{
    uint32_t chunk_id;       ///< AUDIO_WAV_RIFF_ID
    uint32_t chunk_size;     ///< file size minus 8
    uint32_t format_id;      ///< AUDIO_WAV_WAVE_ID
    uint32_t chunk_fmt_id;   ///< AUDIO_WAV_FMT_ID
    uint32_t chunk_fmt_size; ///< 16,18
    uint16_t audio_fmt;      ///< 1: PCM
    uint16_t channels;       ///< channel number
    uint32_t sample_rate;    ///< sample rate in Hz
    uint32_t byte_rate;      ///< <tt>sample_rate*channels*bit_per_sample/8</tt>
    uint16_t block_align;    ///< <tt>channels*bit_per_sample/8</tt>
    uint16_t bit_per_sample; ///< bit per sample
} auWavHeader_t;

/**
 * \brief RIFF chunk header
 */
typedef struct
{
    uint32_t id;   ///< FOURCC id
    uint32_t size; ///< size, not including header
} auRiffChunkHeader_t;

/**
 * \brief AMR-NB modes
 */
typedef enum
{
    AU_AMRNB_MODE_475 = 0, ///< 4.75 kbps
    AU_AMRNB_MODE_515,     ///< 5.15 kbps
    AU_AMRNB_MODE_590,     ///< 5.90 kbps
    AU_AMRNB_MODE_670,     ///< 6.70 kbps
    AU_AMRNB_MODE_740,     ///< 7.40 kbps
    AU_AMRNB_MODE_795,     ///< 7.95 kbps
    AU_AMRNB_MODE_1020,    ///< 10.2 kbps
    AU_AMRNB_MODE_1220,    ///< 12.2 kbps
} auAmrnbMode_t;

/**
 * \brief AMR-WB modes
 */
typedef enum
{
    AU_AMRWB_MODE_660 = 0, ///< 6.60 kbps
    AU_AMRWB_MODE_885,     ///< 8.85 kbps
    AU_AMRWB_MODE_1265,    ///< 12.65 kbps
    AU_AMRWB_MODE_1425,    ///< 14.25 kbps
    AU_AMRWB_MODE_1585,    ///< 15.85 kbps
    AU_AMRWB_MODE_1825,    ///< 18.25 kbps
    AU_AMRWB_MODE_1985,    ///< 19.85 kbps
    AU_AMRWB_MODE_2305,    ///< 23.05 kbps
    AU_AMRWB_MODE_2385,    ///< 23.85 kbps
} auAmrwbMode_t;

/**
 * \brief audio output device
 */
typedef enum
{
    AUDEV_OUTPUT_RECEIVER = 0,  ///< receiver
    AUDEV_OUTPUT_HEADPHONE = 1, ///< headphone
    AUDEV_OUTPUT_SPEAKER = 2,   ///< speaker
} audevOutput_t;

/**
 * \brief audio input device
 */
typedef enum
{
    AUDEV_INPUT_MAINMIC = 0, ///< main mic
    AUDEV_INPUT_AUXMIC = 1,  ///< auxilary mic
    AUDEV_INPUT_DUALMIC = 2, ///< dual mic
    AUDEV_INPUT_HPMIC_L = 3, ///< headphone mic left
    AUDEV_INPUT_HPMIC_R = 4, ///< headphone mic right
} audevInput_t;

/**
 * \brief audio tone type
 */
typedef enum
{
    AUDEV_TONE_DTMF_0 = 0,                ///< DTMF '0'
    AUDEV_TONE_DTMF_1,                    ///< DTMF '1'
    AUDEV_TONE_DTMF_2,                    ///< DTMF '2'
    AUDEV_TONE_DTMF_3,                    ///< DTMF '3'
    AUDEV_TONE_DTMF_4,                    ///< DTMF '4'
    AUDEV_TONE_DTMF_5,                    ///< DTMF '5'
    AUDEV_TONE_DTMF_6,                    ///< DTMF '6'
    AUDEV_TONE_DTMF_7,                    ///< DTMF '7'
    AUDEV_TONE_DTMF_8,                    ///< DTMF '8'
    AUDEV_TONE_DTMF_9,                    ///< DTMF '9'
    AUDEV_TONE_DTMF_A,                    ///< DTMF 'A'
    AUDEV_TONE_DTMF_B,                    ///< DTMF 'B'
    AUDEV_TONE_DTMF_C,                    ///< DTMF 'C'
    AUDEV_TONE_DTMF_D,                    ///< DTMF 'D'
    AUDEV_TONE_DTMF_SHARP,                ///< DTMF '#'
    AUDEV_TONE_DTMF_STAR,                 ///< DTMF '*'
    AUDEV_TONE_DIAL = 16,                 ///< predefined for dailing
    AUDEV_TONE_SUBSCRIBER_BUSY,           ///< predefined for busy
    AUDEV_TONE_RADIO_PATHACKNOWLEDGEMENT, ///< predefine for radio ack
    AUDEV_TONE_CALL_DROPPED,              ///< predefined for call drop
    AUDEV_TONE_SPECIAL_INFORMATION,       ///< predefined for special information
    AUDEV_TONE_CALL_WAITING,              ///< predefined for call waiting
    AUDEV_TONE_RINGING,                   ///< predefined for ringing
} audevTone_t;

/**
 * \brief DTMF tone (0..9, A-D, *, #) frequencies
 *
 * The first one is row frequency, the second is column frequency.
 */
extern const uint16_t gDtmfToneFreq[16][2];

/**
 * \brief audio playing type
 */
typedef enum
{
    /**
     * Placeholder for not in playing.
     */
    AUDEV_PLAY_TYPE_NONE = 0,
    /**
     * Play to local audio path.
     */
    AUDEV_PLAY_TYPE_LOCAL,
    /**
     * Play to uplink remote during voice call.
     */
    AUDEV_PLAY_TYPE_VOICE,
    /**
     * Play to local audio path during poc mode.
     */
    AUDEV_PLAY_TYPE_POC,
} audevPlayType_t;

/**
 * \brief audio recording type
 */
typedef enum
{
    /**
     * Placeholder for not in recording.
     */
    AUDEV_RECORD_TYPE_NONE = 0,
    /**
     * Record from microphone.
     */
    AUDEV_RECORD_TYPE_MIC,
    /**
     * Record for voice call. The recorded stream is the mixed with
     * uplink and downlink channels.
     */
    AUDEV_RECORD_TYPE_VOICE,
    /**
     * Record for voice call. The recorded stream is dual channels with
     * separated uplink and downlink channels.
     */
    AUDEV_RECORD_TYPE_VOICE_DUAL,
    /**
     * PCM dump, for debug only.
     */
    AUDEV_RECORD_TYPE_DEBUG_DUMP,
    /**
     * Record from microphone, during poc mode.
     */
    AUDEV_RECORD_TYPE_POC,
} audevRecordType_t;

/**
 * \brief audio encoder pre-defined quality
 *
 * With different quality setting, some encoders may output the same result.
 * For example, the output of WAV/PCM encoder will be the same for all
 * quality setting.
 */
typedef enum
{
    AUENCODER_QUALITY_LOW,    ///< quality low
    AUENCODER_QUALITY_MEDIUM, ///< quality medium
    AUENCODER_QUALITY_HIGH,   ///< quality high
    AUENCODER_QUALITY_BEST,   ///< quality best
} auEncodeQuality_t;

/**
 * \brief BT Work modes
 */
typedef enum
{
    AU_BT_VOICE_WORK_MODE_NB = 0,
    AU_BT_VOICE_WORK_MODE_WB,
    AU_BT_VOICE_WORK_MODE_NO,
} auBtVoiceWorkMode_t;

/**
 * \brief check whether sample format matches of 2 audio frames
 *
 * \param a         audio frame
 * \param b         the other audio frame
 * \return
 *      - true if sample format matches
 *      - false if sample format mismatches
 */
bool auSampleFormatMatch(const auFrame_t *a, const auFrame_t *b);

/**
 * \brief copy sample format
 *
 * \param dest      destination audio frame
 * \param src       source audio frame
 */
void auSampleFormatCopy(auFrame_t *dest, const auFrame_t *src);

/**
 * \brief calculate byte count to milliseconds
 *
 * At calculation, the sample format in \p frame will be used.
 *
 * \param frame     audio frame
 * \param bytes     byte count
 * \return
 *      - time in milliseconds
 */
unsigned auFrameByteToTime(const auFrame_t *frame, unsigned bytes);

/**
 * \brief calculate milliseconds to byte count
 *
 * At calculation, the sample format in \p frame will be used.
 *
 * \param frame     audio frame
 * \param ms        milliseconds
 * \return
 *      - byte count
 */
unsigned auFrameTimeToByte(const auFrame_t *frame, unsigned ms);

/**
 * \brief audio frame start time in milliseconds
 *
 * \param frame     audio frame
 * \return
 *      - start time in milliseconds
 */
unsigned auFrameStartTime(const auFrame_t *frame);

/**
 * \brief set default sample format
 *
 * The purpose of default sample format just to make it looks "reasonable".
 *
 * \param frame     audio frame
 */
void auInitSampleFormat(auFrame_t *frame);

/**
 * \brief detect stream format by file name suffix
 *
 * \param fname     file name
 * \return
 *      - stream format
 *      - \p AUSTREAM_FORMAT_UNKNOWN for unknown suffix
 */
auStreamFormat_t auStreamFormatBySuffix(const char *fname);

OSI_EXTERN_C_END
#endif
