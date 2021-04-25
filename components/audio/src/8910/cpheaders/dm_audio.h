#if !defined(__DM_AUDIO_H__)
#define __DM_AUDIO_H__

/* Codec type */
typedef enum DM_CodecMode
{
    DM_DICTAPHONE_MR475,
    DM_DICTAPHONE_MR515,
    DM_DICTAPHONE_MR59,
    DM_DICTAPHONE_MR67,
    DM_DICTAPHONE_MR74,
    DM_DICTAPHONE_MR795,
    DM_DICTAPHONE_MR102,
    DM_DICTAPHONE_MR122,
    DM_DICTAPHONE_FR,
    DM_DICTAPHONE_HR,
    DM_DICTAPHONE_EFR,
    DM_DICTAPHONE_PCM,
    DM_DICTAPHONE_AMR_RING = 13,
    DM_DICTAPHONE_MP3,
    DM_DICTAPHONE_AAC,
    DM_DICTAPHONE_WAV,
    DM_DICTAPHONE_MID

} DM_PlayRingType;

#define DM_TONE_DTMF_0 0
#define DM_TONE_DTMF_1 1
#define DM_TONE_DTMF_2 2
#define DM_TONE_DTMF_3 3
#define DM_TONE_DTMF_4 4
#define DM_TONE_DTMF_5 5
#define DM_TONE_DTMF_6 6
#define DM_TONE_DTMF_7 7
#define DM_TONE_DTMF_8 8
#define DM_TONE_DTMF_9 9
#define DM_TONE_DTMF_A 10
#define DM_TONE_DTMF_B 11
#define DM_TONE_DTMF_C 12
#define DM_TONE_DTMF_D 13
#define DM_TONE_DTMF_S 14
#define DM_TONE_DTMF_P 15
#define DM_TONE_DIAL 16
#define DM_TONE_SUBSCRIBER_BUSY 17
#define DM_TONE_CONGESTION 18
#define DM_TONE_RADIO_PATHACKNOWLEDGEMENT 19
#define DM_TONE_CALL_DROPPED 20
#define DM_TONE_SPECIAL_INFORMATION 21
#define DM_TONE_CALL_WAITING 22
#define DM_TONE_RINGING 23

// Tone attenuations
typedef enum
{
    DM_TONE_0dB = 0,
    DM_TONE_m3dB,
    DM_TONE_m9dB,
    DM_TONE_m15dB,

    DM_TONE_GAIN_QTY
} DM_ToneGain;

/* Microphone volume. */
typedef enum
{
    DM_AUDIO_MIC_MUTE = 0, // mute
    DM_AUDIO_MIC_ENABLE,   // unmute

    DM_AUDIO_MIC_GAIN_QTY
} DM_MicGain;

/* Side tone gain. */
typedef enum
{
    DM_AUDIO_SIDE_MUTE = 0, // mute
    DM_AUDIO_SIDE_m36dB,    //  min
    DM_AUDIO_SIDE_m33dB,
    DM_AUDIO_SIDE_m30dB,
    DM_AUDIO_SIDE_m27dB,
    DM_AUDIO_SIDE_m24dB,
    DM_AUDIO_SIDE_m21dB,
    DM_AUDIO_SIDE_m18dB,
    DM_AUDIO_SIDE_m15dB,
    DM_AUDIO_SIDE_m12dB,
    DM_AUDIO_SIDE_m9dB,
    DM_AUDIO_SIDE_m6dB,
    DM_AUDIO_SIDE_m3dB,
    DM_AUDIO_SIDE_0dB,
    DM_AUDIO_SIDE_3dB,
    DM_AUDIO_SIDE_6dB, //  max

    DM_AUDIO_SIDE_GAIN_QTY
} DM_SideToneGain;

/* Speaker volume */
typedef enum
{
    DM_AUDIO_SPK_MUTE = 0,
    DM_AUDIO_SPK_VOL_1,
    DM_AUDIO_SPK_VOL_2,
    DM_AUDIO_SPK_VOL_3,
    DM_AUDIO_SPK_VOL_4,
    DM_AUDIO_SPK_VOL_5,
    DM_AUDIO_SPK_VOL_6,
    DM_AUDIO_SPK_VOL_7,
    DM_AUDIO_SPK_VOL_8,
    DM_AUDIO_SPK_VOL_9,
    DM_AUDIO_SPK_VOL_10,
    DM_AUDIO_SPK_VOL_11,
    DM_AUDIO_SPK_VOL_12,
    DM_AUDIO_SPK_VOL_13,
    DM_AUDIO_SPK_VOL_14,
    DM_AUDIO_SPK_VOL_15,
    DM_AUDIO_SPK_GAIN_QTY,
    DM_AUDIO_SPK_ALIGNED_4BYTES = 0xF0000000
} DM_SpeakerGain;

#define DM_AUDIO_MODE_HANDSET 0
#define DM_AUDIO_MODE_EARPIECE 1
#define DM_AUDIO_MODE_LOUDSPEAKER 2
#define DM_AUDIO_MODE_BLUETOOTH 3

#define DM_AUD_IOCTL_MUTE_MIC 0
#define DM_AUD_IOCTL_UNMUTE_MIC 1
#define DM_AUD_IOCTL_MUTE_SPK 4
#define DM_AUD_IOCTL_UNMUTE_SPK 8

#define DM_DEVICE_SPEAKER 0    /* Tone, Keypad sound to play on audio device. */
#define DM_DEVICE_MICROPHONE 1 /* Microphone sound to send on audio device. */

BOOL DM_SpeakerOpen(void);
BOOL DM_MicOpen(void);
BOOL DM_SpeakerClose(void);
BOOL DM_MicClose(void);

BOOL DM_SetMicGain(DM_MicGain nGain);
BOOL DM_SetSpeakerGain(DM_SpeakerGain nGain);
BOOL DM_SetSideGain(DM_SideToneGain nGain);

BOOL SRVAPI DM_SetAudioMode(UINT8 nAudioMode);

BOOL SRVAPI DM_SetAudioMode_From_Calibration(UINT8 nAudioMode);

BOOL SRVAPI DM_GetAudioMode(UINT8 *pAudioMode);
BOOL SRVAPI DM_GetSpeakerVolume_From_Calibration(UINT8 *pVolume);

BOOL SRVAPI DM_SetAudioVolume(DM_SpeakerGain nVolume);
BOOL SRVAPI DM_GetSpeakerVolume(UINT8 *pVolume);

BOOL SRVAPI DM_SetAudio_From_Calibration_Volume(DM_SpeakerGain nVolume);

BOOL DM_Audio_StartStream(UINT32 *buffer, UINT32 len, DM_PlayRingType codec, BOOL loop_mode);
BOOL DM_Audio_StopStream(VOID);
BOOL DM_Audio_PauseStream(VOID);
BOOL DM_Audio_ResumeStream(VOID);
VOID DM_Set_SpeechingFlag(BOOL flag);
BOOL DM_Close_All_Audio(VOID);

/*
================================================================================
Function   : DM_Audio_RemainingStream
--------------------------------------------------------------------------------
Scope      : Returns the number of words that can still be written in the record buffer.
Parameters : none
Return     : The number of words that can still be written in the record buffer.
================================================================================
*/

UINT32 DM_Audio_RemainingRecord(VOID);
/*
================================================================================
Function   : DM_Audio_RemainingStream
--------------------------------------------------------------------------------
Scope      : Returns the number of words yet to be played in the stream buffer.
Parameters : none
Return     : The number of words yet to be played in the stream buffer.
================================================================================
*/
UINT32 DM_Audio_RemainingStream(VOID);

BOOL SRVAPI DM_SetAudioControl(UINT8 nCmd, UINT16 nValue);

BOOL DM_MicStartRecord(UINT32 *pBuffer, UINT32 nBufferSize, DM_PlayRingType codec, BOOL loop_mode);

BOOL SRVAPI DM_MicStopRecord(
    VOID);

BOOL SRVAPI DM_PlayTone(
    UINT8 nToneType,
    UINT8 nToneAttenuation,
    UINT16 nDuration,
    DM_SpeakerGain volumn);

BOOL SRVAPI DM_StopTone(
    VOID);

BOOL DM_StartAudio(VOID);
BOOL DM_StopAudio(VOID);
BOOL DM_RestartAudio(VOID);

BOOL DM_SetAudioVolume(DM_SpeakerGain nVolume);
BOOL DM_SetMicGain(DM_MicGain nGain);
BOOL DM_SetAudioMode(UINT8 nAudioMode);

INT32 DM_AudSetup(CONST SND_ITF_T itf, CONST AUD_LEVEL_T *cfg);
INT32 DM_AudStreamStart(CONST SND_ITF_T itf, CONST HAL_AIF_STREAM_T *stream, CONST AUD_LEVEL_T *cfg);
INT32 DM_AudStreamStop(CONST SND_ITF_T itf);
INT32 DM_AudStreamRecordStop(CONST SND_ITF_T itf);
BOOL DM_AudStreamPause(BOOL pause);
INT32 DM_AudStreamRecord(CONST SND_ITF_T itf, CONST HAL_AIF_STREAM_T *stream, CONST AUD_LEVEL_T *cfg);

BOOL DM_AudTestModeSetup(CONST HAL_AIF_STREAM_T *stream, CONST AUD_LEVEL_T *cfg, AUD_TEST_T mode);

BOOL DM_AudForceReceiverMicSelection(BOOL on);
BOOL DM_AudSetCurOutputDevice(SND_ITF_T itf);
SND_ITF_T DM_AudGetCurOutputDevice(VOID);
BOOL DM_AudMuteOutputDevice(BOOL mute);
BOOL DM_AudLoudspeakerWithEarpiece(BOOL on);

#if (BT_A2DP_ANA_DAC_MUTE == 1)
BOOL DM_AudGetFMWorkingFlag(VOID);
#endif

#endif // __DM_AUDIO_H__
