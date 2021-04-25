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

// AUTO GENERATED

#ifndef _RPC_AUD_API_H_
#define _RPC_AUD_API_H_

AUD_ERR_T aud_StreamStart(SND_ITF_T itf, HAL_AIF_STREAM_T *stream, AUD_LEVEL_T *cfg);
AUD_ERR_T aud_StreamStop(SND_ITF_T itf);
AUD_ERR_T aud_Setup(SND_ITF_T itf, AUD_LEVEL_T *cfg);
AUD_ERR_T aud_StreamRecord(SND_ITF_T itf, HAL_AIF_STREAM_T *stream, AUD_LEVEL_T *cfg);
AUD_ERR_T aud_StreamRecordStop(SND_ITF_T itf);
HAL_ERR_T hal_zspMusicPlayStart(void);
HAL_ERR_T hal_zspMusicPlayStop(void);
void aud_SetAudDeviceCFG(AUD_DEVICE_CFG_EXT_T cfg);
VOIS_ERR_T vois_SetCfg(AUD_ITF_T itf, VOIS_AUDIO_CFG_T *cfg, AUD_DEVICE_CFG_EXT_T devicecfg, SND_BT_WORK_MODE_T btworkmode);
void vois_SetBtNRECFlag(UINT32 flag);
AUD_ERR_T vois_TestModeSetup(SND_ITF_T itf, HAL_AIF_STREAM_T *stream, AUD_LEVEL_T *cfg, AUD_TEST_T mode, UINT32 voismode);
INT32 DM_StartAudioEx(void);
INT32 DM_RestartAudioEx(void);
INT32 DM_StopAudioEx(void);
INT32 DM_PlayToneEx(UINT8 nToneType, UINT8 nToneAttenuation, UINT16 nDuration, DM_SpeakerGain volumn);
INT32 DM_StopToneEx(void);
HAL_ERR_T hal_zspVoiceRecordStart(void);
HAL_ERR_T hal_zspVoiceRecordStop(void);
AUD_ERR_T aud_SetBtClk(void);
AUD_ERR_T aud_ClrBtClk(void);
INT32 DM_AudStreamStart(SND_ITF_T itf, HAL_AIF_STREAM_T *stream, AUD_LEVEL_T *cfg);
INT32 DM_AudStreamStop(SND_ITF_T itf);
INT32 DM_AudSetup(SND_ITF_T itf, AUD_LEVEL_T *cfg);
INT32 DM_AudStreamRecord(SND_ITF_T itf, HAL_AIF_STREAM_T *stream, AUD_LEVEL_T *cfg);
INT32 DM_VoisSetCfg(AUD_ITF_T itf, VOIS_AUDIO_CFG_T *cfg, AUD_DEVICE_CFG_EXT_T devicecfg, SND_BT_WORK_MODE_T btworkmode);
INT32 DM_AudSetAudDeviceCFG(AUD_DEVICE_CFG_EXT_T cfg);
INT32 DM_VoisSetBtNRECFlag(UINT32 flag);
INT32 DM_VoisTestModeSetup(SND_ITF_T itf, HAL_AIF_STREAM_T *stream, AUD_LEVEL_T *cfg, AUD_TEST_T mode, UINT32 voismode);
AUD_ERR_T aud_HeadsetConfig(UINT32 isInsert, UINT32 isBbat);
INT32 DM_ZspMusicPlayStart(void);
INT32 DM_ZspMusicPlayStop(void);
void aud_SetSharkingMode(UINT32 isSharkingMode);
AUD_ERR_T aud_bbatPcmBufferPlayStart(SND_ITF_T itf, BBAT_PCM_STREAM_T * playbuffer);
AUD_ERR_T aud_bbatPcmBufferPlayStop(SND_ITF_T itf);
INT32 DM_AudStreamRecordStop(SND_ITF_T itf);
void DM_AudSetSharkingMode(UINT32 isSharkingMode);
INT32 DM_ZspVoiceVibrateStart(UINT32 time);
INT32 DM_ZspVoiceVibrateStop(void);
INT32 DM_ZspMusicVibrateStart(UINT32 time);
INT32 DM_ZspMusicVibrateStop(void);
INT32 DM_ZspMusicPlayStartEx(AUD_AP2CP_PARA_T *Aud_Para);
INT32 DM_ZspMusicPlayStopEx(AUD_AP2CP_PARA_T *Aud_Para);
void DM_AudSetToneMode(UINT32 isToneMode);
HAL_ERR_T hal_zspVoicePlayStart(void);
HAL_ERR_T hal_zspVoicePlayStop(void);
void aud_SetLdoVB(UINT32 en);
HAL_ERR_T hal_btVoiceStart(void);
HAL_ERR_T hal_btVoiceStop(void);
void aud_SetCloseDelayFlag(HAL_ERR_T  en);

#endif // _RPC_AUD_API_H_
// AUTO GENERATED END
