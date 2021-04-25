/**  @file
  audio_demo.h

  @brief
  This file is used to define audio demo for different Quectel Project.

*/

/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/

#ifndef AUDIO_DEMO_H
#define AUDIO_DEMO_H


#ifdef __cplusplus
extern "C" {
#endif

#include "ql_audio.h"
#include "ql_api_common.h"
#include "ql_app_feature_config.h"



/*========================================================================
 *  Variable Definition
 *========================================================================*/
#define RECORD_BUFFER_MAX   (uint)100*1024
#define TEST_PLAY_VOLUME    AUDIOHAL_SPK_VOL_11

#define TEST_MP3_FILE_NAME    "UFS:test_mp3.mp3"
#define TEST_WAV_FILE_NAME    "UFS:test_wav.wav"
#define TEST_AMR_FILE_NAME    "UFS:test_amr.amr"
#define TEST_RECORD_WAV_NAME  "UFS:test_record_wav.wav"
#define TEST_RECORD_PCM_NAME  "UFS:test_record_pcm.pcm"

/*========================================================================
 *  function Definition
 *========================================================================*/
void ql_audio_app_init(void);

void test_pcm(void);
void test_mp3(void);
void test_wav(void);
void test_amr(void);
void test_record_file(void);
void test_record_stream(void);
void test_amr_stream(void);

#ifdef QL_APP_FEATURE_EXT_CODEC
void test_codec(void);
ql_audio_errcode_e ql_switch_ext_codec(bool enable);
ql_audio_errcode_e aud_codec_init(void);
#endif


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* AUDIO_DEMO_H */


