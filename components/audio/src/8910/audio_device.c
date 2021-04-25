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

#include "audio_device.h"
#include "audio_player.h"
#include "audio_recorder.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hal_chip.h"
#include "hal_shmem_region.h"
#include "hal_config.h"
#include "drv_md_ipc.h"
#include "pb_util.h"
#include "audio_device.pb.h"
#include "vfs.h"
#include <stdlib.h>
#include <string.h>
#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE
#include "drv_names.h"
#include "drv_i2c.h"
#endif

#include "ql_api_common.h"

#define INT8 int8_t
#define UINT8 uint8_t
#define INT16 int16_t
#define UINT16 uint16_t
#define INT32 int32_t
#define UINT32 uint32_t
#define BOOL bool
#define VOID void
#define CONST
#define PUBLIC
#define SRVAPI
typedef void *HAL_VOC_IRQ_HANDLER_T; // not care
typedef uint32_t CALIB_AUD_PATH_T;   // not care
typedef uint32_t CALIB_AUD_MODE_T;   // not care
#define MUSIC_SUPPORT
#define SPEECH_SUPPORT
#undef CHIP_HAS_AP
#define AUDIO_CALIB_VER 2
#define BT_A2DP_ANA_DAC_MUTE 0
#include "cpheaders/aud_snd_types.h"
#include "cpheaders/hal_error.h"
#include "cpheaders/hal_speech.h"
#include "cpheaders/hal_volte.h"
#include "cpheaders/hal_aif.h"
#include "cpheaders/snd_types.h"
#include "cpheaders/aud_m.h"
#include "cpheaders/vois_m.h"
#include "cpheaders/dm_audio.h"
#include "cpheaders/hal_aud_zsp.h"
#include "cfw.h"

// CP APIs
INT32 DM_VoisSetCfg(AUD_ITF_T itf, VOIS_AUDIO_CFG_T *cfg, AUD_DEVICE_CFG_EXT_T devicecfg, SND_BT_WORK_MODE_T btworkmode);
INT32 DM_StartAudioEx(void);
INT32 DM_RestartAudioEx(void);
INT32 DM_StopAudioEx(void);
INT32 DM_PlayToneEx(UINT8 nToneType, UINT8 nToneAttenuation, UINT16 nDuration, DM_SpeakerGain volumn);
INT32 DM_StopToneEx(void);
INT32 DM_ZspMusicPlayStart(void);
INT32 DM_ZspMusicPlayStop(void);
INT32 DM_VoisTestModeSetup(SND_ITF_T itf, HAL_AIF_STREAM_T *stream, AUD_LEVEL_T *cfg, AUD_TEST_T mode, UINT32 voismode);
AUD_ERR_T aud_bbatPcmBufferPlayStop(SND_ITF_T itf);
AUD_ERR_T aud_bbatPcmBufferPlayStart(SND_ITF_T itf, BBAT_PCM_STREAM_T *playbuffer);

#define AUDEV_NV_FNAME "/nvm/audio_device.nv"
#define AUDEV_INPUT_COUNT (5)
#define AUDEV_OUTPUT_COUNT (3)
#define AUDIO_LEVEL_MAX (15)
#define AUDEV_VOL_MAX (100)
#define AUDEV_WQ_PRIO OSI_PRIORITY_ABOVE_NORMAL
#define AUDEV_RECORD_BUF_SIZE (1024)
#define AUDEV_PLAY_HIDDEN_BUF_SIZE (512)
#define AUDEV_PLAY_OUT_BUF_SIZE (1152 * 2)
#define AUDEV_VOICE_DUMP_CHANNELS (6)

#ifdef CONFIG_QUEC_PROJECT_FEATURE_AUDIO
extern uint8 ql_aud_pa_type;
extern int ql_aud_pa_init(void);
#else
#define AUDEV_AUDIO_PA_TYPE (AUD_INPUT_TYPE_CLASSD)
#endif

typedef enum
{
    AUDEV_CALIB_AUD_CODEC_SET_INGAIN = 0x00000000,
    AUDEV_CALIB_AUD_CODEC_GET_INGAIN = 0x00000001,
    AUDEV_CALIB_AUD_CODEC_SET_OUTGAIN = 0x00000002,
    AUDEV_CALIB_AUD_CODEC_GET_OUTGAIN = 0x00000003,
    AUDEV_CALIB_AUD_CODEC_SET_SIDETONEGAIN = 0x00000004,
    AUDEV_CALIB_AUD_CODEC_GET_SIDETONEGAIN = 0x00000005,
    AUDEV_CALIB_AUD_CODEC_SET_RECORDERGAIN = 0x00000006,
    AUDEV_CALIB_AUD_CODEC_GET_RECORDERGAIN = 0x00000007,
} audevCalibAudCodecCtrl_t;

enum
{
    ZSP_MUSIC_MODE_PLAY_PCM = 1,
    ZSP_MUSIC_MODE_PLAY_MP3 = 2,
    ZSP_MUSIC_MODE_PLAY_WMA = 3,
    ZSP_MUSIC_MODE_PLAY_AAC = 4,
    ZSP_MUSIC_MODE_RECORD_WAV = 5,
    ZSP_MUSIC_MODE_RECORD_AAC = 6,
    ZSP_MUSIC_MODE_RECORD_AMR = 7,
    ZSP_MUSIC_MODE_PLAY_FM = 8,
    ZSP_MUSIC_MODE_DUPLEX_POC = 10,
};

enum
{
    ZSP_VOICE_RECORD_FORMAT_PCM = 0,
    ZSP_VOICE_RECORD_FORMAT_AMR = 1,
    ZSP_VOICE_RECORD_FORMAT_DUMP = 3,
};

/**
 * audio device poc mode,
 * 1\POC_MODE_RECORD half duplex record
 * 2\AUDEV_POC_MODE_PLAY half duplex play
 * 3\AUDEV_POC_MODE_PLAY_RECORD full duplex play and record
 */
typedef enum
{
    AUDEV_POC_MODE_RECORD = (1 << 8),
    AUDEV_POC_MODE_PLAY = (1 << 12),
    AUDEV_POC_MODE_PLAY_RECORD = (1 << 8) | (1 << 12),
} audevPocMode_t;

/**
 * audio device poc mode cp ipc notify event,
 * 1\CP record data is comming
 * 2\CP player need data
 */
typedef enum
{
    AUDEV_POC_IPC_GETFRAME_IND = (1 << 0),
    AUDEV_POC_IPC_PUTFRAME_IND = (1 << 4),
} audevPocIpcNotifyInd_t;

/**
 * audio device work in four state,
 * 1\voice call user state
 * 2\other single user state,mutex
 * 3\voice call user + other single user state
 * 4\full duplex poc state
 */
typedef enum
{
    AUDEV_CLK_USER_VOICE = (1 << 0),
    AUDEV_CLK_USER_TONE = (1 << 1),
    AUDEV_CLK_USER_PLAY = (1 << 2),
    AUDEV_CLK_USER_RECORD = (1 << 3),
    AUDEV_CLK_USER_LOOPBACK = (1 << 4),
    AUDEV_CLK_USER_PLAYTEST = (1 << 5),
    AUDEV_CLK_USER_POC = (1 << 6),
} audevClkUser_t;

typedef struct
{
    audevInput_t indev;      // input device
    audevOutput_t outdev;    // output device
    uint8_t voice_vol;       // volume for voice call
    uint8_t play_vol;        // volume for play and tone
    bool out_mute;           // output mute for voice call, play, tone
    uint32_t sample_rate;    // sample freq for mic record and voice call record
    uint8_t sample_interval; // sample time interval for mic record
    uint8_t ext_i2s_en;      // sample time interval for mic record
} audevSetting_t;

typedef struct
{
    volatile AUD_CP_STATUS_E cpstatus;
    osiMutex_t *lock;
    osiWorkQueue_t *wq;
    osiWork_t *ipc_work;
    osiTimer_t *finish_timer;
    osiWork_t *finish_work;
    osiWork_t *tone_end_work;
    osiTimer_t *simu_toneend_timer;
    osiSemaphore_t *cptoneend_sema;
    osiSemaphore_t *cpstatus_sema;
    AUD_ZSP_SHAREMEM_T *shmem;
    SND_BT_WORK_MODE_T btworkmode;
    uint32_t clk_users;
    audevSetting_t cfg;
    bool voice_uplink_mute;
#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE
    osiWork_t *i2s_play_work;
    osiWork_t *i2s_record_work;
#endif
#ifdef CONFIG_AUDIO_IPC_SET_TRIGGER_MODE_ENABLE
    uint32_t ipc_trigger_mode;
#endif
    struct
    {
        audevPlayType_t type;
        bool eos_error;
        uint8_t channel_count;
        unsigned sample_rate;
        unsigned total_bytes;
        audevPlayOps_t ops;
        void *ops_ctx;
        AUD_LEVEL_T level;
        osiElapsedTimer_t time;
    } play;

    struct
    {
        audevRecordType_t type;
        bool enc_error;
        auFrame_t frame;
        audevRecordOps_t ops;
        void *ops_ctx;
        char buf[AUDEV_RECORD_BUF_SIZE];
    } record;

    struct
    {
        audevOutput_t outdev;
    } bbat;

    struct
    {
        audevOutput_t outdev;
        audevInput_t indev;
        uint8_t volume;
    } loopback;
} audevContext_t;

static audevContext_t gAudevCtx;

static inline int prvHex2Num(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 0xa + (c - 'A');
    if (c >= 'a' && c <= 'f')
        return 0xa + (c - 'a');
    return -1;
}

static inline char prvNum2Hex(uint8_t c)
{
    if (c < 0xA)
        return c + '0';
    if (c >= 0xA && c <= 0xF)
        return (c - 0xA) + 'A';
    return -1;
}

/**
 * Convert volume [0,100] to AUD_SPK_LEVEL_T
 */
static inline AUD_SPK_LEVEL_T prvVolumeToLevel(unsigned vol, bool mute)
{
    if (mute || vol == 0)
        return AUD_SPK_MUTE;

    unsigned level = (vol * AUDIO_LEVEL_MAX + (AUDEV_VOL_MAX / 2)) / AUDEV_VOL_MAX;
    level = OSI_MIN(unsigned, AUDIO_LEVEL_MAX, level);
    level = OSI_MAX(unsigned, 1, level);
    return (AUD_SPK_LEVEL_T)level;
}

static inline uint8_t prvVolumeToToneAttenuatio(unsigned vol)
{
    int attenuation = 4 - (vol * 4 / AUDEV_VOL_MAX);
    attenuation = OSI_MIN(int, 3, attenuation);
    attenuation = OSI_MAX(int, 0, attenuation);
    return attenuation;
}

/**
 * Convert audevOutput_t to AUD_ITF_T
 */
static inline AUD_ITF_T prvOutputToAudItf(audevOutput_t dev)
{
    return (AUD_ITF_T)dev; // the enum is the same
}

/**
 * Convert audevOutput_t to SND_ITF_T
 */
static inline SND_ITF_T prvOutputToSndItf(audevOutput_t dev)
{
    return (SND_ITF_T)dev; // the enum is the same
}

/**
 * Convert audevInput_t to AUD_INPUT_TYPE_T
 */
static inline AUD_INPUT_TYPE_T prvInputToType(audevInput_t dev)
{
    return (AUD_INPUT_TYPE_T)dev; // the enum is the same
}

/**
 * Convert audevTone_t to tone type
 */
static inline uint8_t prvToneToDmTone(audevTone_t tone)
{
    return tone; // the value is the same
}

/**
 * Whether audevOutput_t is valid
 */
static inline bool prvIsOutputValid(audevOutput_t dev)
{
    return (dev < AUDEV_OUTPUT_COUNT);
}

/**
 * Whether audevInput_t is valid
 */
static inline bool prvIsInputValid(audevInput_t dev)
{
    return (dev < AUDEV_INPUT_COUNT);
}

/**
 * Enable (request) audio clock
 */
static inline void prvEnableAudioClk(audevClkUser_t user)
{
    audevContext_t *d = &gAudevCtx;
    if (d->clk_users == 0)
    {
#ifdef CONFIG_CAMA_CLK_FOR_AUDIO
        halCameraClockRequest(CLK_CAMA_USER_AUDIO, CAMA_CLK_OUT_FREQ_26M);
#else
        halClock26MRequest(CLK_26M_USER_AUDIO);
#endif
    }
    d->clk_users |= user;
}

/**
 * Disable (release) audio clock
 */
static inline void prvDisableAudioClk(audevClkUser_t user)
{
    audevContext_t *d = &gAudevCtx;
    d->clk_users &= ~user;
    if (d->clk_users == 0)
    {
#ifdef CONFIG_CAMA_CLK_FOR_AUDIO
        halCameraClockRelease(CLK_CAMA_USER_AUDIO);
#else
        halClock26MRelease(CLK_26M_USER_AUDIO);
#endif
    }
}

/**
 * Set CP status, called by CP.
 */
void AUD_SetCodecOpStatus(AUD_CP_STATUS_E status)
{
    audevContext_t *d = &gAudevCtx;

    OSI_LOGI(0, "audio cp status %d -> %d", d->cpstatus, status);
    d->cpstatus = status; // mutex is not needed
    osiSemaphoreRelease(d->cpstatus_sema);
}

/**
 * Triggered by CP call for tone end.
 */
static void prvToneEndWork(void *param)
{
    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);
    if (DM_StopToneEx() != 0)
        OSI_LOGE(0, "audio stop tone failed");

    prvDisableAudioClk(AUDEV_CLK_USER_TONE);
    osiSemaphoreRelease(d->cptoneend_sema);
    osiMutexUnlock(d->lock);
}

/**
 * Called by CP, use work queue for quicker CP response
 */
void AUD_ToneStopEnd(void)
{
    OSI_LOGI(0, "audio cp tone end");

    audevContext_t *d = &gAudevCtx;
    osiWorkEnqueue(d->tone_end_work, d->wq);
}

/**
 * Wait CP status. AUD_SetCodecOpStatus will be called by CP to
 * update status. Return false at timeout.
 */
static bool prvWaitStatus(AUD_CP_STATUS_E flag)
{
    audevContext_t *d = &gAudevCtx;

    osiElapsedTimer_t elapsed;
    osiElapsedTimerStart(&elapsed);

    for (;;)
    {
        if (d->cpstatus == flag)
        {
            d->cpstatus = AUD_NULL;
            return true;
        }

        int timeout = 3000 - osiElapsedTime(&elapsed);
        if (timeout <= 0)
            break;

        osiSemaphoreTryAcquire(d->cpstatus_sema, timeout);
    }

    OSI_LOGW(0, "audio device wait status timeout, expected/%d status/%d",
             flag, d->cpstatus);
    return false;
}

/**
 * Set configuration for voice
 */
static bool prvSetVoiceConfig(void)
{
    audevContext_t *d = &gAudevCtx;

    VOIS_AUDIO_CFG_T cfg = {
        .spkLevel = prvVolumeToLevel(d->cfg.voice_vol, d->cfg.out_mute),
        .micLevel = d->voice_uplink_mute ? AUD_MIC_MUTE : AUD_MIC_ENABLE,
        .sideLevel = AUD_SIDE_MUTE,
        .toneLevel = AUD_TONE_0dB,
        .encMute = 0,
        .decMute = 0,
    };
    AUD_DEVICE_CFG_EXT_T device_ext = {
        .inputType = prvInputToType(d->cfg.indev),
        .inputCircuityType = AUD_INPUT_MIC_CIRCUITY_TYPE_DIFFERENTIAL,
        .inputPath = AUD_INPUT_PATH1,
        .linePath = AUD_LINE_PATH2,
#ifndef CONFIG_QUEC_PROJECT_FEATURE_AUDIO
        .spkpaType = AUDEV_AUDIO_PA_TYPE,
#else
		.spkpaType = ql_aud_pa_type,
#endif
    };

    if (DM_VoisSetCfg(prvOutputToAudItf(d->cfg.outdev), &cfg, device_ext,
                      d->btworkmode) != 0)
        return false;

    if (!prvWaitStatus(VOIS_SETUP_DONE))
        return false;

    return true;
}

/**
 * Set configuration for play
 */
static bool prvSetPlayConfig(void)
{
    SND_SPK_LEVEL_T FadeStartLevel, FadeEndLevel;
    audevContext_t *d = &gAudevCtx;
    uint32_t time_ms = osiElapsedTime(&d->play.time);

    if (time_ms < 100)
        return true;
    osiElapsedTimerStart(&d->play.time);

    FadeStartLevel = (d->play.level.spkLevel);
    FadeEndLevel = prvVolumeToLevel(d->cfg.play_vol, d->cfg.out_mute);
    OSI_LOGI(0, "audio device prvSetPlayConfig , old Level/%d new Level/%d",
             FadeStartLevel, FadeEndLevel);

    if (FadeStartLevel == FadeEndLevel)
        return true;

    AUD_LEVEL_T level = {
        .spkLevel = FadeEndLevel,
        .micLevel = SND_MIC_ENABLE,
        .sideLevel = SND_SIDE_VOL_15,
        .toneLevel = SND_TONE_0DB,
        .appMode = SND_APP_MODE_MUSIC,
    };

    bool isIncrease = (FadeEndLevel > FadeStartLevel) ? true : false;

#define STEPLEVEL (7)
    {
        if (isIncrease)
        {
            FadeStartLevel = FadeStartLevel + STEPLEVEL;
            level.spkLevel = OSI_MIN(unsigned, FadeStartLevel, FadeEndLevel);
            level.spkLevel = OSI_MIN(unsigned, AUDIO_LEVEL_MAX, level.spkLevel);
            OSI_LOGI(0, "audio device prvSetPlayConfig , level.spkLevel/%d ", level.spkLevel);
            if (DM_AudSetup(prvOutputToSndItf(d->cfg.outdev), &level) != 0)
                return false;

            if (!prvWaitStatus(AUD_CODEC_SETUP_DONE))
                return false;

            //update para in play
            d->play.level.spkLevel = level.spkLevel;

            if ((level.spkLevel == AUDIO_LEVEL_MAX) || (level.spkLevel == FadeEndLevel))
                return true;
        }
        else
        {
            FadeStartLevel = (FadeStartLevel < STEPLEVEL) ? 0 : (FadeStartLevel - STEPLEVEL);
            level.spkLevel = OSI_MAX(unsigned, FadeStartLevel, FadeEndLevel);
            OSI_LOGI(0, "audio device prvSetPlayConfig , level.spkLevel/%d ", level.spkLevel);

            if (DM_AudSetup(prvOutputToSndItf(d->cfg.outdev), &level) != 0)
                return false;

            if (!prvWaitStatus(AUD_CODEC_SETUP_DONE))
                return false;

            //update para in play
            d->play.level.spkLevel = level.spkLevel;

            if ((level.spkLevel == 0) || (level.spkLevel == FadeEndLevel))
                return true;
        }
    }

    OSI_LOGI(0, "audio device prvSetPlayConfig , d->play.level.spkLevel/%d",
             d->play.level.spkLevel);

    return true;
}

/**
 * Set input device configuration
 */
static bool prvSetDeviceExt(void)
{
    audevContext_t *d = &gAudevCtx;

    AUD_DEVICE_CFG_EXT_T device_ext = {
        .inputType = prvInputToType(d->cfg.indev),
        .inputCircuityType = AUD_INPUT_MIC_CIRCUITY_TYPE_DIFFERENTIAL,
        .inputPath = AUD_INPUT_PATH1,
        .linePath = AUD_LINE_PATH2,
#ifndef CONFIG_QUEC_PROJECT_FEATURE_AUDIO
		.spkpaType = AUDEV_AUDIO_PA_TYPE,
#else
		.spkpaType = ql_aud_pa_type,
#endif

    };
    aud_SetAudDeviceCFG(device_ext);
    return true;
}

// Operations on AUDIO_INPUT_PARAM_T, which is located in shared memory.
// There are 2 use cases: PCM play and record
//
// PCM play:
// * AP write to shared memory, update writeOffset
// * ZSP read from shared memory, update readOffset -> it is volatile
//
// Record:
// * AP read from shared memory, update readOffset
// * ZSP write to shared memory, update writeOffset -> it is volatile

#define AUDIOIN_IS_EMPTY (writeOffset == readOffset)
#define AUDIOIN_IS_FULL (writeOffset == (readOffset + inLenth - 4) % inLenth)
#define AUDIOIN_SPACE (AUDIOIN_IS_EMPTY ? inLenth - 4 : AUDIOIN_IS_FULL ? 0 : (writeOffset > readOffset) ? inLenth - (writeOffset - readOffset) - 4 : readOffset - writeOffset - 4)
#define AUDIOIN_BYTES (AUDIOIN_IS_FULL ? inLenth - 4 : AUDIOIN_IS_EMPTY ? 0 : (writeOffset > readOffset) ? writeOffset - readOffset : inLenth - (readOffset - writeOffset))

/**
 * Whether audInPara is empty
 */
OSI_UNUSED static inline bool prvAudioInIsEmpty(AUD_ZSP_SHAREMEM_T *p)
{
    uint16_t readOffset = *(volatile uint16_t *)&p->audInPara.readOffset;
    uint16_t writeOffset = *(volatile uint16_t *)&p->audInPara.writeOffset;
    OSI_BARRIER();
    return AUDIOIN_IS_EMPTY;
}

/**
 * Whether audInPara is full
 */
OSI_UNUSED static inline bool prvAudioInIsFull(AUD_ZSP_SHAREMEM_T *p)
{
    uint16_t readOffset = *(volatile uint16_t *)&p->audInPara.readOffset;
    uint16_t writeOffset = *(volatile uint16_t *)&p->audInPara.writeOffset;
    uint16_t inLenth = *(volatile uint16_t *)&p->audInPara.inLenth;
    OSI_BARRIER();
    return AUDIOIN_IS_FULL;
}

/**
 * Available space (for write) in audInPara
 */
OSI_UNUSED static unsigned prvAudioInSpace(AUD_ZSP_SHAREMEM_T *p)
{
    uint16_t readOffset = *(volatile uint16_t *)&p->audInPara.readOffset;
    uint16_t writeOffset = *(volatile uint16_t *)&p->audInPara.writeOffset;
    uint16_t inLenth = *(volatile uint16_t *)&p->audInPara.inLenth;
    OSI_BARRIER();
    return AUDIOIN_SPACE;
}

/**
 * Available bytes (for read) in audInPara
 */
OSI_UNUSED static unsigned prvAudioInBytes(AUD_ZSP_SHAREMEM_T *p)
{
    uint16_t readOffset = *(volatile uint16_t *)&p->audInPara.readOffset;
    uint16_t writeOffset = *(volatile uint16_t *)&p->audInPara.writeOffset;
    uint16_t inLenth = *(volatile uint16_t *)&p->audInPara.inLenth;
    OSI_BARRIER();
    return AUDIOIN_BYTES;
}

/**
 * Put data to audInPara
 */
OSI_UNUSED static unsigned prvAudioInPut(AUD_ZSP_SHAREMEM_T *p, const void *buf, unsigned size)
{
    uint16_t readOffset = *(volatile uint16_t *)&p->audInPara.readOffset;
    uint16_t writeOffset = *(volatile uint16_t *)&p->audInPara.writeOffset;
    uint16_t inLenth = *(volatile uint16_t *)&p->audInPara.inLenth;
    OSI_BARRIER();

    char *bytePtr = (char *)&p->audInput[0];
    unsigned space = AUDIOIN_SPACE;
    if (space == 0)
        return 0;

    if (size > space)
        size = space;

    unsigned tail = inLenth - writeOffset;
    if (tail >= size)
    {
        memcpy(bytePtr + writeOffset, buf, size);
    }
    else
    {
        memcpy(bytePtr + writeOffset, buf, tail);
        memcpy(bytePtr, (const char *)buf + tail, size - tail);
    }

    OSI_BARRIER();
    *(volatile uint16_t *)&p->audInPara.writeOffset = (writeOffset + size) % inLenth;
    return size;
}

/**
 * Get data from audInPara
 */
OSI_UNUSED static unsigned prvAudioInGet(AUD_ZSP_SHAREMEM_T *p, void *buf, unsigned size)
{
    uint16_t readOffset = *(volatile uint16_t *)&p->audInPara.readOffset;
    uint16_t writeOffset = *(volatile uint16_t *)&p->audInPara.writeOffset;
    uint16_t inLenth = *(volatile uint16_t *)&p->audInPara.inLenth;
    OSI_BARRIER();

    char *bytePtr = (char *)&p->audInput[0];
    unsigned bytes = AUDIOIN_BYTES;
    if (bytes == 0)
        return 0;

    if (size > bytes)
        size = bytes;
    unsigned tail = inLenth - readOffset;
    if (tail >= size)
    {
        memcpy(buf, bytePtr + readOffset, size);
    }
    else
    {
        memcpy(buf, bytePtr + readOffset, tail);
        memcpy((char *)buf + tail, bytePtr, size - tail);
    }

    OSI_BARRIER();
    *(volatile uint16_t *)&p->audInPara.readOffset = (readOffset + size) % inLenth;
    return size;
}

OSI_UNUSED static unsigned prvAudioPocInGet(AUD_ZSP_SHAREMEM_T *p, void *buf, unsigned size)
{
    uint16_t readOffset = *(volatile uint16_t *)&p->audOutPara.readOffset;
    uint16_t writeOffset = *(volatile uint16_t *)&p->audOutPara.writeOffset;
    //uint16_t inLenth = *(volatile uint16_t *)&p->audOutPara.length;
    uint16_t inLenth = AUDIO_OUTPUT_BUF_SIZE;
    OSI_BARRIER();

    char *bytePtr = (char *)&p->audOutput[0];
    unsigned bytes = AUDIOIN_BYTES;

    if (bytes == 0)
        return 0;

    if (size > bytes)
        size = bytes;
    unsigned tail = inLenth - readOffset;
    if (tail >= size)
    {
        memcpy(buf, bytePtr + readOffset, size);
    }
    else
    {
        memcpy(buf, bytePtr + readOffset, tail);
        memcpy((char *)buf + tail, bytePtr, size - tail);
    }

    OSI_BARRIER();
    *(volatile uint16_t *)&p->audOutPara.readOffset = (readOffset + size) % inLenth;

    return size;
}

/**
 * Get audio frame from player, and put yto audInPara.
 */
static bool prvPlayGetFramesLocked(void)
{
    audevContext_t *d = &gAudevCtx;

    auFrame_t frame = {
        .sample_format = AUSAMPLE_FORMAT_S16,
        .channel_count = d->play.channel_count,
        .sample_rate = d->play.sample_rate,
    };

    while (!d->play.eos_error)
    {
        unsigned space = prvAudioInSpace(d->shmem);
        if (space == 0)
            return true;

        if (!d->play.ops.get_frame(d->play.ops_ctx, &frame))
        {
            d->play.eos_error = true;
        }
        else
        {
            if ((frame.bytes == 0) && ((frame.flags & AUFRAME_FLAG_END) == 0))
                return true;

            unsigned bytes = prvAudioInPut(d->shmem, (void *)frame.data, frame.bytes);
            d->play.ops.data_consumed(d->play.ops_ctx, bytes);

            d->play.total_bytes += bytes;
            if (frame.flags & AUFRAME_FLAG_END)
            {
                d->play.eos_error = true;
                d->shmem->audInPara.fileEndFlag = 1;
            }

            OSI_LOGI(0, "audio play space/%d frame/%d put/%d total/%d flags/%d", space,
                     frame.bytes, bytes, d->play.total_bytes, frame.flags);
        }

        if (d->play.eos_error)
        {
            unsigned bytes = prvAudioInBytes(d->shmem);
            unsigned ms = auFrameByteToTime(&frame, bytes + AUDEV_PLAY_HIDDEN_BUF_SIZE);
            osiTimerStart(d->finish_timer, ms);
        }
    }

    return true;
}

/**
 * Get from audInPara, and send the data to recorder.
 */
static void prvRecPutFramesLocked(void)
{
    audevContext_t *d = &gAudevCtx;

    for (;;)
    {
        if (d->clk_users & AUDEV_CLK_USER_POC)
            d->record.frame.bytes = prvAudioPocInGet(d->shmem, (void *)d->record.frame.data,
                                                     AUDEV_RECORD_BUF_SIZE);
        else
            d->record.frame.bytes = prvAudioInGet(d->shmem, (void *)d->record.frame.data,
                                                  AUDEV_RECORD_BUF_SIZE);
        if (d->record.frame.bytes == 0)
            break;

        // We shouldn't break even put_frame failed, to avoid overflow
        // just send finsh event to notify recorder app to stop
        if (!d->record.enc_error)
        {
            d->record.enc_error = !d->record.ops.put_frame(d->record.ops_ctx, &d->record.frame);
            if (d->record.enc_error)
            {
                if (((d->clk_users & AUDEV_CLK_USER_RECORD) || (d->clk_users & AUDEV_CLK_USER_POC)) &&
                    d->record.enc_error &&
                    d->record.ops.handle_event != NULL)
                    d->record.ops.handle_event(d->record.ops_ctx, AUDEV_RECORD_EVENT_FINISH);
            }
        }
    }
}

/**
 * Work triggered by IPC notify, to be executed in audio thread
 */
static void prvIpcWork(void *param)
{
    audevContext_t *d = &gAudevCtx;

    osiMutexLock(d->lock);

    if (d->clk_users & AUDEV_CLK_USER_PLAY)
    {
        prvPlayGetFramesLocked();
        prvSetPlayConfig();
    }
    if (d->clk_users & AUDEV_CLK_USER_RECORD)
        prvRecPutFramesLocked();

    if (d->clk_users & AUDEV_CLK_USER_POC)
    {
        if (!(d->shmem->traceDataFlag & AUDEV_POC_MODE_PLAY))
        {
            memset(&d->shmem->rxPcmVolte, 0, sizeof(HAL_SPEECH_PCM_BUF_T) * 2);
        }

        if (d->shmem->traceDataFlag & AUDEV_POC_IPC_GETFRAME_IND)
        {
            d->shmem->traceDataFlag &= ~AUDEV_POC_IPC_GETFRAME_IND;
            prvPlayGetFramesLocked();
        }
        if (d->shmem->traceDataFlag & AUDEV_POC_IPC_PUTFRAME_IND)
        {
            d->shmem->traceDataFlag &= ~AUDEV_POC_IPC_PUTFRAME_IND;
            prvRecPutFramesLocked();
        }
    }
    osiMutexUnlock(d->lock);
}

/**
 * IPC notify, called in IPC driver
 */
static void prvIpcNotify(void)
{
    audevContext_t *d = &gAudevCtx;
    osiWorkEnqueue(d->ipc_work, d->wq);
}

/**
 * Playback finish timer timeout work
 */
static void prvFinishWork(void *param)
{
    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);

    if ((d->clk_users & AUDEV_CLK_USER_PLAY) &&
        d->play.eos_error &&
        d->play.ops.handle_event != NULL)
        d->play.ops.handle_event(d->play.ops_ctx, AUDEV_PLAY_EVENT_FINISH);
    osiMutexUnlock(d->lock);
}

/**
 * Playback finish timer timeout
 */
static void prvFinishTimeout(void *param)
{
    audevContext_t *d = &gAudevCtx;
    osiWorkEnqueue(d->finish_work, d->wq);
}

/**
 * simu tone play end timer timeout
 */
static void prvToneStopEndTimeout(void *param)
{
    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);
    prvDisableAudioClk(AUDEV_CLK_USER_TONE);
    osiSemaphoreRelease(d->cptoneend_sema);
    osiMutexUnlock(d->lock);
}

/**
 * Decode nv into context
 */
static bool prvNvDecode(audevSetting_t *p, uint8_t *buffer, unsigned length)
{
    const pb_field_t *fields = pbAudev_fields;
    pbAudev pbdata = {};
    pbAudev *pbs = &pbdata;

    pb_istream_t is = pb_istream_from_buffer(buffer, length);
    if (!pb_decode(&is, fields, pbs))
        return false;

    PB_OPT_DEC_ASSIGN(p->outdev, outdev);
    PB_OPT_DEC_ASSIGN(p->indev, indev);
    PB_OPT_DEC_ASSIGN(p->voice_vol, voice_vol);
    PB_OPT_DEC_ASSIGN(p->play_vol, play_vol);
    PB_OPT_DEC_ASSIGN(p->out_mute, out_mute);
    PB_OPT_DEC_ASSIGN(p->sample_rate, sample_rate);
    PB_OPT_DEC_ASSIGN(p->sample_interval, sample_interval);
    PB_OPT_DEC_ASSIGN(p->ext_i2s_en, ext_i2s_en);
    return true;
}

/**
 * Encode nv from context
 */
static int prvNvEncode(const audevSetting_t *p, void *buffer, unsigned length)
{
    const pb_field_t *fields = pbAudev_fields;
    pbAudev pbdata = {};
    pbAudev *pbs = &pbdata;

    PB_OPT_ENC_ASSIGN(p->outdev, outdev);
    PB_OPT_ENC_ASSIGN(p->indev, indev);
    PB_OPT_ENC_ASSIGN(p->voice_vol, voice_vol);
    PB_OPT_ENC_ASSIGN(p->play_vol, play_vol);
    PB_OPT_ENC_ASSIGN(p->out_mute, out_mute);
    PB_OPT_ENC_ASSIGN(p->sample_rate, sample_rate);
    PB_OPT_ENC_ASSIGN(p->sample_interval, sample_interval);
    PB_OPT_ENC_ASSIGN(p->ext_i2s_en, ext_i2s_en);
    return pbEncodeToMem(fields, pbs, buffer, length);
}

/**
 * Load nv from file to context
 */
static bool prvLoadNv(audevSetting_t *p)
{
    OSI_LOGI(0, "audio load nv");

    ssize_t file_size = vfs_sfile_size(AUDEV_NV_FNAME);
    if (file_size <= 0)
        return false;

    uint8_t *buffer = (uint8_t *)calloc(1, file_size);
    if (buffer == NULL)
        return false;

    if (vfs_sfile_read(AUDEV_NV_FNAME, buffer, file_size) != file_size)
        goto failed;

    if (!prvNvDecode(p, buffer, file_size))
        goto failed;

    free(buffer);
    return true;

failed:
    free(buffer);
    OSI_LOGE(0, "audio load nv failed");
    return false;
}

/**
 * Store nv to file from context
 */
static bool prvStoreNv(const audevSetting_t *p)
{
    OSI_LOGI(0, "audio store nv");

    int length = prvNvEncode(p, NULL, 0);
    if (length < 0)
        return false;

    void *buffer = calloc(1, length);
    if (buffer == NULL)
        return false;

    length = prvNvEncode(p, buffer, length);
    if (length < 0)
        goto failed;

    if (vfs_sfile_write(AUDEV_NV_FNAME, buffer, length) != length)
        goto failed;

    free(buffer);
    return true;

failed:
    free(buffer);
    OSI_LOGE(0, "audio store nv failed");
    return false;
}

/**
 * Set to default audio device settings
 */
static void prvSetDefaultSetting(audevSetting_t *p)
{
    p->indev = AUDEV_INPUT_MAINMIC;
    p->outdev = AUDEV_OUTPUT_SPEAKER;
    p->voice_vol = 60;
    p->play_vol = 60;
    p->out_mute = false;
    p->sample_rate = 8000;
    p->sample_interval = 20;
    p->ext_i2s_en = 0;
}

#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE

//When use external device,you should set audio device ext i2s mode:1.(audevSetExtI2sMode)
//You should open the macros : CONFIG_AUDIO_EXT_I2S_ENABLE,(audio)
//                                            CONFIG_CAMA_POWER_ON_INIT, (HAL  configurations:)
//                                            CONFIG_CAMA_CLK_FOR_AUDIO.(HAL  configurations:)
// ext i2s only support in local audioplay and local soundrecorder

extern uint32_t g_halPingPangFlag;
unsigned g_pingpanghalfbytes = 0;
HAL_AIF_CONFIG_T g_audCodecAifCfg;
drvI2cMaster_t *g_audExtDevI2c = NULL;

//External device address,you can modify this address according to the actual situation.
#define EXT_DEV_I2C_ADDR (0x10)

typedef struct
{
    uint8_t addr;
    uint8_t data;
} extDevReg_t;

//This is an example for ES8374.You can modify them.
//g_audPlayRegList: Initilize register list when playing music.(8k)
static const extDevReg_t g_audPlayRegList[] =
    {
        {0x00, 0x3f},
        {0x00, 0x03},
        {0x01, 0xff},
        {0x05, 0x33},
        {0x06, 0x03},
        {0x6f, 0xa0},
        {0x72, 0x41},
        {0x09, 0x01},
        {0x0c, 0x17},
        {0x0d, 0xa3},
        {0x0e, 0x30},
        {0x0f, 0x81},
        {0x0a, 0x8a},
        {0x0b, 0x07},
        {0x09, 0x41},
        {0x10, 0x0c},
        {0x11, 0x0c},
        {0x24, 0x28},
        {0x36, 0x00},
        {0x12, 0x30},
        {0x13, 0x20},
        {0x2c, 0x05},
        {0x2d, 0x05},
        {0x00, 0x80},
        {0x02, 0x08},
        {0x14, 0x0a},
        {0x15, 0x40},
        {0x16, 0x00},
        {0x17, 0xa0},
        {0x1b, 0x10},
        {0x28, 0x00},
        {0x25, 0xc0},
        {0x38, 0xc0},
        {0x6d, 0x15},

        {0x0b, 0x07},
        {0x0c, 0x17},
        {0x0d, 0xa3},
        {0x0e, 0x30},
        {0x0f, 0x9d},
        {0x05, 0x66},
        {0x06, 0x06},
        {0x07, 0x00},
        {0x03, 0x20}, //8k

        {0x1c, 0x90},
        {0x1d, 0x00},
        {0x1e, 0xa0},
        {0x1f, 0x0c},
        {0x36, 0x00},
        {0x37, 0x00},
        {0x38, 0x00},

};

//This is an example for ES8374.You can modify them.
//g_audRecRegList: Initilize register list when recording.(8k)
static const extDevReg_t g_audRecRegList[] =
    {
        {0x00, 0x3f},
        {0x00, 0x03},
        {0x01, 0xff},
        {0x05, 0x33},
        {0x06, 0x03},
        {0x6f, 0xa0},
        {0x72, 0x41},
        {0x09, 0x01},
        {0x0c, 0x17},
        {0x0d, 0xa3},
        {0x0e, 0x30},
        {0x0f, 0x81},
        {0x0a, 0x8a},
        {0x0b, 0x07},
        {0x09, 0x41},
        {0x10, 0x0c},
        {0x11, 0x0c},
        {0x24, 0x28},
        {0x36, 0x00},
        {0x12, 0x30},
        {0x13, 0x20},
        {0x2c, 0x05},
        {0x2d, 0x05},
        {0x00, 0x80},
        {0x02, 0x08},
        {0x14, 0x0a},
        {0x15, 0x40},
        {0x16, 0x00},
        {0x17, 0xa0},
        {0x1b, 0x10},
        {0x28, 0x00},
        {0x25, 0xc0},
        {0x38, 0xc0},
        {0x6d, 0x15},

        {0x0b, 0x07},
        {0x0c, 0x17},
        {0x0d, 0xa3},
        {0x0e, 0x30},
        {0x0f, 0x9d},
        {0x05, 0x66},
        {0x06, 0x06},
        {0x07, 0x00},
        {0x03, 0x20},

        {0x21, 0x24},
        {0x22, 0x00},
        {0x24, 0x08},
        {0x25, 0x00},
        {0x28, 0x00},

};

//This is an example for ES8374.You can modify them.
//g_audCloseRegList:  register list when closing.
static const extDevReg_t g_audCloseRegList[] =
    {
        {0x38, 0xc0},
        {0x25, 0xc0},
        {0x24, 0x28},
        {0x36, 0x20},
        {0x37, 0x21},
        {0x1a, 0x08},
        {0x1c, 0x10},
        {0x1d, 0x10},
        {0x1e, 0x40},
        {0x21, 0xd4},
        {0x15, 0xbf},
        {0x14, 0x14},
        {0x09, 0x80},
        {0x01, 0x03},

};

//This is an example for ES8374.You can modify them and add lists,such as
//8k,11.025k,12k,16k,22.05k,24k,32k,44.1k,48k,96k.

/*static const extDevReg_t g_aud8kRegList[] =
{
    {0x0b, 0x07},
    {0x0c, 0x17},
    {0x0d, 0xa3},
    {0x0e, 0x30},
    {0x0f, 0x9d},
    {0x05, 0x66},
    {0x06, 0x06},
    {0x07, 0x00},
    {0x03, 0x20},
};*/

static const extDevReg_t g_aud16kRegList[] =
    {
        {0x0b, 0x07},
        {0x0c, 0x17},
        {0x0d, 0xa3},
        {0x0e, 0x30},
        {0x0f, 0x95},
        {0x05, 0x33},
        {0x06, 0x03},
        {0x07, 0x00},
        {0x03, 0x20},
};

static const extDevReg_t g_aud44kRegList[] =
    {
        {0x0b, 0x06},
        {0x0c, 0x27},
        {0x0d, 0xdc},
        {0x0e, 0x2b},
        {0x0f, 0x88},
        {0x05, 0x11},
        {0x06, 0x01},
        {0x07, 0x00},
        {0x03, 0x20},
};

//This is an example for ES8374.(You can modify when recording.)
static const extDevReg_t g_audinpathRegList[] =
    {
        {0x21, 0x24},
        {0x22, 0x00},
        {0x24, 0x08},
        {0x25, 0x00},
        {0x28, 0x00},
};

//This is an example for ES8374.(You can modify when playing.)
static const extDevReg_t g_audoutpathRegList[] =
    {
        {0x1c, 0x90},
        {0x1d, 0x00},
        {0x1e, 0xa0},
        {0x1f, 0x0c},
        {0x36, 0x00},
        {0x37, 0x00},
        {0x38, 0x00},

};

/*
//keep for debug
static void prvAudWriteOneReg(uint8_t addr, uint8_t data)
{
    drvI2cSlave_t idAddress = {EXT_DEV_I2C_ADDR, addr, 0, false};
    if (g_audExtDevI2c != NULL)
    {
        drvI2cWrite(g_audExtDevI2c, &idAddress, &data, 1);
    }
    else
    {
        OSI_LOGE(0, "i2c is not open");
    }
}
static void prvAudReadReg(uint8_t addr, uint8_t *data, uint32_t len)
{
    drvI2cSlave_t idAddress = {EXT_DEV_I2C_ADDR, addr, 0, false};
    if (g_audExtDevI2c != NULL)
    {
        drvI2cRead(g_audExtDevI2c, &idAddress, data, len);
    }
    else
    {
        OSI_LOGE(0, "i2c is not open");
    }
}
*/
static bool prvAudWriteRegList(const extDevReg_t *regList, uint16_t len)
{
    uint16_t regCount;
    drvI2cSlave_t wirte_data = {EXT_DEV_I2C_ADDR, 0, 0, false};
    for (regCount = 0; regCount < len; regCount++)
    {
        OSI_LOGI(0, "aud write reg %x,%x", regList[regCount].addr, regList[regCount].data);
        wirte_data.addr_data = regList[regCount].addr;
        if (drvI2cWrite(g_audExtDevI2c, &wirte_data, &regList[regCount].data, 1))
            osiDelayUS(5);
        else
            return false;
    }

    return true;
}

/**
 * Set configuration for play
 */
static bool prvExtI2sSetPlayConfig(void)
{
    audevContext_t *d = &gAudevCtx;

    AUD_LEVEL_T level = {
        .spkLevel = prvVolumeToLevel(d->cfg.play_vol, d->cfg.out_mute),
        .micLevel = SND_MIC_ENABLE,
        .sideLevel = SND_SIDE_VOL_15,
        .toneLevel = SND_TONE_0DB,
        .appMode = SND_APP_MODE_MUSIC,
    };

    //SND_ITF_T itf = prvOutputToSndItf(d->cfg.outdev);

    //set outpath reg,you can modify.
    prvAudWriteRegList(g_audoutpathRegList, sizeof(g_audoutpathRegList) / sizeof(extDevReg_t));

    //update para in play
    d->play.level.spkLevel = level.spkLevel;

    return true;
}

/**
 * Set input device configuration
 */
static bool prvExtI2sSetDeviceExt(void)
{
    /*
    audevContext_t *d = &gAudevCtx;
    AUD_DEVICE_CFG_EXT_T device_ext = {
        .inputType = prvInputToType(d->cfg.indev),
        .inputCircuityType = AUD_INPUT_MIC_CIRCUITY_TYPE_DIFFERENTIAL,
        .inputPath = AUD_INPUT_PATH1,
        .linePath = AUD_LINE_PATH2,
        .spkpaType = AUDEV_AUDIO_PA_TYPE,
    };
	*/

    //set inputpath reg,you can modify.
    prvAudWriteRegList(g_audinpathRegList, sizeof(g_audinpathRegList) / sizeof(extDevReg_t));

    return true;
}

static void prvAudI2sAifCfg(CONST HAL_AIF_STREAM_T *stream, HAL_AIF_SERIAL_CFG_T *serialCfg)
{
    switch (stream->sampleRate)
    {
    case HAL_AIF_FREQ_8000HZ:
        serialCfg->bckLrckRatio = 32; //32 AIF_BCK_LRCK_BCK_LRCK_32
        break;

    case HAL_AIF_FREQ_11025HZ:
        serialCfg->bckLrckRatio = 36;
        break;

    case HAL_AIF_FREQ_12000HZ:
        serialCfg->bckLrckRatio = 38;
        break;

    case HAL_AIF_FREQ_16000HZ:
        serialCfg->bckLrckRatio = 50;
        break;

    case HAL_AIF_FREQ_22050HZ:
        serialCfg->bckLrckRatio = 40;
        break;

    case HAL_AIF_FREQ_24000HZ:
        serialCfg->bckLrckRatio = 38;
        break;

    case HAL_AIF_FREQ_32000HZ:
        serialCfg->bckLrckRatio = 56;
        break;

    case HAL_AIF_FREQ_44100HZ:
        serialCfg->bckLrckRatio = 62;
        break;

    case HAL_AIF_FREQ_48000HZ:
        serialCfg->bckLrckRatio = 36;
        break;

    case HAL_AIF_FREQ_96000HZ:
        serialCfg->bckLrckRatio = 36;
        break;

    default:
        OSI_ASSERT(false, "Improper stream frequency.");
        // Die
        break;
    }

    if (stream->channelNb == HAL_AIF_MONO)
    {
        serialCfg->rxMode = HAL_AIF_RX_MODE_STEREO_MONO_FROM_L;
        serialCfg->txMode = HAL_AIF_TX_MODE_MONO_STEREO_DUPLI; //HAL_AIF_TX_MODE_MONO_STEREO_DUPLI,HAL_AIF_TX_MODE_MONO_STEREO_CHAN_L
    }
    else
    {
        serialCfg->rxMode = HAL_AIF_RX_MODE_STEREO_STEREO;
        serialCfg->txMode = HAL_AIF_TX_MODE_STEREO_STEREO;
    }

    g_audCodecAifCfg.interface = HAL_AIF_IF_SERIAL_1; //I2S2:HAL_AIF_IF_SERIAL_1,I2S1:HAL_AIF_IF_SERIAL
    g_audCodecAifCfg.sampleRate = stream->sampleRate;
    g_audCodecAifCfg.channelNb = stream->channelNb;
    g_audCodecAifCfg.PARALLEL_FOR_FM = false;
    g_audCodecAifCfg.serialCfg = serialCfg;

    hal_AifSetI2sGpio();
}

static bool prvAudExtCodecI2cOpen(uint32_t name, drvI2cBps_t bps)
{
    if (name == 0 || g_audExtDevI2c != NULL)
    {
        return false;
    }
    g_audExtDevI2c = drvI2cMasterAcquire(name, bps);
    if (g_audExtDevI2c == NULL)
    {
        OSI_LOGE(0, "audio i2c open fail");
        return false;
    }

    hwp_iomux->pad_gpio_14_cfg_reg |= IOMUX_PAD_GPIO_14_SEL_V_FUN_I2C_M2_SCL_SEL;
    hwp_iomux->pad_gpio_15_cfg_reg |= IOMUX_PAD_GPIO_15_SEL_V_FUN_I2C_M2_SDA_SEL;

    return true;
}

static void prvAudExtCodecI2cClose()
{
    if (g_audExtDevI2c != NULL)
        drvI2cMasterRelease(g_audExtDevI2c);
    g_audExtDevI2c = NULL;
}

OSI_UNUSED static unsigned prvAudInUpdate(AUD_ZSP_SHAREMEM_T *p, void *buf, unsigned size)
{
    uint16_t readOffset = *(volatile uint16_t *)&p->audInPara.readOffset;
    uint16_t writeOffset = *(volatile uint16_t *)&p->audInPara.writeOffset;
    uint16_t inLenth = *(volatile uint16_t *)&p->audInPara.inLenth;
    OSI_BARRIER();

    char *bytePtr = (char *)&p->audInput[0];
    unsigned bytes = AUDIOIN_BYTES;

    unsigned tail = inLenth - readOffset;
    if (tail >= size)
    {
        memcpy(buf, bytePtr + readOffset, size);
    }
    else //tail<size
    {
        if (bytes >= size)
        {
            memcpy(buf, bytePtr + readOffset, tail);
            memcpy((char *)buf + tail, bytePtr, size - tail);
        }
        else
        {
            memcpy(buf, bytePtr + readOffset, tail);
            memset((char *)buf + tail, 0, size - tail);

            if (p->audInPara.fileEndFlag == 0)
            {
                prvIpcNotify(); //update data
                OSI_LOGI(0, "prvIpcNotify-1");
                return -1;
            }
        }
    }

    OSI_BARRIER();
    *(volatile uint16_t *)&p->audInPara.readOffset = (readOffset + size) % inLenth;

    if ((bytes - size) < (size * 4)) //advance 4 frame
    {
        if (p->audInPara.fileEndFlag == 0) //notify
        {
            prvIpcNotify(); //update data
            OSI_LOGI(0, "prvIpcNotify0");
        }
    }

    /*OSI_LOGI(0, "1read_availBytes = %d,need_bytes = %d,readoffset:%d, writeoffset:%d \n", 
	bytes-size,size,p->audInPara.readOffset,p->audInPara.writeOffset);*/

    return size;
}

int16_t prvExtI2sOutLevel2Val(SND_SPK_LEVEL_T spkLevel)
{
    int16_t level2Val = 16384;

    switch (spkLevel)
    {
    case SND_SPK_MUTE: //mute
        level2Val = 0;
        break;
    case SND_SPK_VOL_1:
        level2Val = 130;
        break;
    case SND_SPK_VOL_2:
        level2Val = 184;
        break;
    case SND_SPK_VOL_3:
        level2Val = 260;
        break;
    case SND_SPK_VOL_4:
        level2Val = 367;
        break;
    case SND_SPK_VOL_5:
        level2Val = 518;
        break;
    case SND_SPK_VOL_6:
        level2Val = 732;
        break;
    case SND_SPK_VOL_7:
        level2Val = 1034;
        break;
    case SND_SPK_VOL_8:
        level2Val = 1460;
        break;
    case SND_SPK_VOL_9:
        level2Val = 2063;
        break;
    case SND_SPK_VOL_10:
        level2Val = 2914;
        break;
    case SND_SPK_VOL_11:
        level2Val = 4115;
        break;
    case SND_SPK_VOL_12:
        level2Val = 5813;
        break;
    case SND_SPK_VOL_13:
        level2Val = 8211;
        break;
    case SND_SPK_VOL_14:
        level2Val = 11599;
        break;
    case SND_SPK_VOL_15:
        level2Val = 16384;
        break;
    default:
        level2Val = 16384;
        break;
    }

    return level2Val;
}

#define SHR(a, shift) ((a) >> (shift))
#define ADD32(a, b) ((INT32)(a) + (INT32)(b))
#define MULT16_16(a, b) (((INT32)(INT16)(a)) * ((INT32)(INT16)(b)))
#define MULT16_16_P14(a, b) (SHR(ADD32(8192, MULT16_16((a), (b))), 14))

static bool prvExtI2sOutputDataLocked(void)
{
    audevContext_t *d = &gAudevCtx;

    unsigned space = g_pingpanghalfbytes;
    unsigned offset = 0;

    if (g_halPingPangFlag == 1)
    {
        offset = space;
    }

    unsigned bytes = prvAudInUpdate(d->shmem, (char *)d->shmem->audOutput + offset, space); //importtant
    if (bytes == -1)
    {
        OSI_LOGI(0, "prvAudInUpdate err\n");
    }

    //music level
    SND_SPK_LEVEL_T spkLevel = prvVolumeToLevel(d->cfg.play_vol, d->cfg.out_mute);
    int16_t val = prvExtI2sOutLevel2Val(spkLevel);

    for (int16_t i = 0; i < (space / 2); i++)
    {
        d->shmem->audOutput[offset / 2 + i] = MULT16_16_P14(d->shmem->audOutput[offset / 2 + i], val);
    }

    OSI_LOGI(0, "audio spkLevel/%d, val/%d,offset/%d", spkLevel, val, offset);

    return true;
}

static void prvExtI2sInPutDataLocked(void)
{
    audevContext_t *d = &gAudevCtx;

    unsigned space = g_pingpanghalfbytes;
    unsigned offset = 0;
    uintptr_t srcdata;

    if (g_halPingPangFlag == 1)
    {
        offset = space;
    }

    srcdata = ((d->cfg.sample_rate == 8000) ? (uintptr_t)d->shmem->txPcmBuffer.pcmBuf : (uintptr_t)d->shmem->txPcmVolte.pcmBuf);

    for (;;)
    {
        d->record.frame.bytes = OSI_MIN(unsigned, AUDEV_RECORD_BUF_SIZE, space);
        if (d->record.frame.bytes == 0)
            break;

        memcpy((void *)d->record.frame.data, (void *)srcdata + offset, d->record.frame.bytes);
        offset += d->record.frame.bytes;
        space -= d->record.frame.bytes;

        // We shouldn't break even put_frame failed, to avoid overflow
        d->record.ops.put_frame(d->record.ops_ctx, &d->record.frame);
    }
}

static void prvExtI2sOutputWork(void *param)
{
    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);
    prvExtI2sOutputDataLocked();
    osiMutexUnlock(d->lock);
}

static void prvExtI2sInputWork(void *param)
{
    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);
    prvExtI2sInPutDataLocked();
    osiMutexUnlock(d->lock);
}

static void audevExtI2sOutputISR(void *ctx)
{
    audevContext_t *d = &gAudevCtx;
    hal_AifIrqHandler(1);
    osiWorkEnqueue(d->i2s_play_work, d->wq);
    hal_AifIrqClrStatus(1);
}
static void audevExtI2sInputISR(void *ctx)
{
    audevContext_t *d = &gAudevCtx;
    hal_AifIrqHandler(0);
    osiWorkEnqueue(d->i2s_record_work, d->wq);
    hal_AifIrqClrStatus(0);
}

static void prvExtI2sOutputSetIrqHandler(void)
{
    uint32_t irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_AIF_APB_1); //AIF_PLAY_INTERUPT_ID

    osiIrqDisable(irqn);
    osiIrqSetHandler(irqn, audevExtI2sOutputISR, NULL);
    osiIrqSetPriority(irqn, SYS_IRQ_PRIO_AIF_APB_1);
    osiIrqEnable(irqn);
}

static void prvExtI2sOutputDisableIrq(void)
{
    uint32_t irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_AIF_APB_1);
    osiIrqDisable(irqn);
}

static void prvExtI2sInputSetIrqHandler(void)
{
    uint32_t irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_AIF_APB_0); //AIF_RECORD_INTERUPT_ID

    osiIrqDisable(irqn);
    osiIrqSetHandler(irqn, audevExtI2sInputISR, NULL);
    osiIrqSetPriority(irqn, SYS_IRQ_PRIO_AIF_APB_0);
    osiIrqEnable(irqn);
}

static void prvExtI2sInputDisableIrq(void)
{
    uint32_t irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_AIF_APB_0);
    osiIrqDisable(irqn);
}

/**
 * Get audio device ext i2s mode status
 */
uint32_t audevIsExtI2sMode(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->cfg.ext_i2s_en;
}

/**
 * Set audio device ext i2s mode
 */
bool audevSetExtI2sMode(uint8_t I2sMode)
{
    audevContext_t *d = &gAudevCtx;
    if ((I2sMode != 0) && (I2sMode != 1))
        return false;

    osiMutexLock(d->lock);
    d->cfg.ext_i2s_en = I2sMode;
    audevSetting_t cfg = d->cfg;
    osiMutexUnlock(d->lock);

    prvStoreNv(&cfg);
    return true;
}
#endif

/**
 * Initialize audio device
 */
void audevInit(void)
{
    audevContext_t *d = &gAudevCtx;

#ifdef CONFIG_QUEC_PROJECT_FEATURE_AUDIO
	ql_aud_pa_init();
#endif

    const halShmemRegion_t *region = halShmemGetRegion(MEM_AUDIO_SM_NAME);
    if (region == NULL || region->address == 0)
        osiPanic();

    OSI_LOGI(0, "audio device init, shared memory 0x%x", region->address);

    d->shmem = (AUD_ZSP_SHAREMEM_T *)region->address;
    d->cpstatus = AUD_NULL;
    d->lock = osiMutexCreate();
    d->wq = osiWorkQueueCreate("audio", 1, AUDEV_WQ_PRIO, CONFIG_AUDIO_WQ_STACK_SIZE);
    d->ipc_work = osiWorkCreate(prvIpcWork, NULL, NULL);
    d->finish_work = osiWorkCreate(prvFinishWork, NULL, NULL);
    d->finish_timer = osiTimerCreate(NULL, prvFinishTimeout, NULL);
    d->tone_end_work = osiWorkCreate(prvToneEndWork, NULL, NULL);
    d->simu_toneend_timer = osiTimerCreate(OSI_TIMER_IN_SERVICE, prvToneStopEndTimeout, NULL);
    d->cptoneend_sema = osiSemaphoreCreate(1, 0);
    d->cpstatus_sema = osiSemaphoreCreate(1, 0);
#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE
    d->i2s_play_work = osiWorkCreate(prvExtI2sOutputWork, NULL, NULL);
    d->i2s_record_work = osiWorkCreate(prvExtI2sInputWork, NULL, NULL);
#endif
#ifdef CONFIG_AUDIO_IPC_SET_TRIGGER_MODE_ENABLE
    audevSetPlayerIpcTriggerMode(0);
#endif
    prvSetDefaultSetting(&d->cfg);
    prvLoadNv(&d->cfg);
    d->voice_uplink_mute = false;
    d->btworkmode = SND_BT_WORK_MODE_NO;
    d->clk_users = 0;
    d->record.frame.data = (uintptr_t)d->record.buf;
    ipc_register_audio_notify(prvIpcNotify);
    prvSetDeviceExt();
    osiElapsedTimerStart(&d->play.time);
}

/**
 * Reset audio device setting, and clear nv
 */
void audevResetSettings(void)
{
    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);

    prvSetDefaultSetting(&d->cfg);
    vfs_unlink(AUDEV_NV_FNAME);
    osiMutexUnlock(d->lock);
}

/**
 * Return the internal mutex
 */
osiMutex_t *audevMutex(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->lock;
}

/**
 * Set voice call uplink mute/unmute
 */
bool audevSetVoiceUplinkMute(bool mute)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio uplink mute %d", mute);

    osiMutexLock(d->lock);

    d->voice_uplink_mute = mute;

    if ((d->clk_users & AUDEV_CLK_USER_VOICE) || (d->clk_users & AUDEV_CLK_USER_POC))
        prvSetVoiceConfig();

    osiMutexUnlock(d->lock);
    return true;
}

/**
 * Whether voice call uplink is muted
 */
bool audevIsVoiceUplinkMute(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->voice_uplink_mute;
}

/**
 * Set output interface
 */
bool audevSetOutput(audevOutput_t dev)
{
    OSI_LOGI(0, "audio set output %d", dev);

    if (!prvIsOutputValid(dev))
        return false;

    audevContext_t *d = &gAudevCtx;
    if (d->cfg.outdev == dev)
        return true;

    osiMutexLock(d->lock);

    if ((d->clk_users & AUDEV_CLK_USER_VOICE) || (d->clk_users & AUDEV_CLK_USER_POC))
    {
        d->cfg.outdev = dev;
        prvSetVoiceConfig();
    }
    else
    {
        d->cfg.outdev = dev;
    }

    audevSetting_t cfg = d->cfg;
    osiMutexUnlock(d->lock);

    prvStoreNv(&cfg);
    return true;
}

/**
 * Get output interface
 */
audevOutput_t audevGetOutput(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->cfg.outdev;
}

/**
 * Set input interface
 */
bool audevSetInput(audevInput_t dev)
{
    OSI_LOGI(0, "audio set input %d", dev);

    if (!prvIsInputValid(dev))
        return false;

    audevContext_t *d = &gAudevCtx;
    if (d->cfg.indev == dev)
        return true;

    osiMutexLock(d->lock);

    d->cfg.indev = dev;
    prvSetDeviceExt();

    if ((d->clk_users & AUDEV_CLK_USER_VOICE) || (d->clk_users & AUDEV_CLK_USER_POC))
        prvSetVoiceConfig();

    audevSetting_t cfg = d->cfg;
    osiMutexUnlock(d->lock);

    prvStoreNv(&cfg);
    return true;
}

/**
 * Get input interface
 */
audevInput_t audevGetInput(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->cfg.indev;
}

/**
 * Set volume for voice call
 */
bool audevSetVoiceVolume(unsigned vol)
{
    if (vol > AUDEV_VOL_MAX)
        return false;

    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);

    d->cfg.voice_vol = vol;
    if ((d->clk_users & AUDEV_CLK_USER_VOICE) || (d->clk_users & AUDEV_CLK_USER_POC))
        prvSetVoiceConfig();

    audevSetting_t cfg = d->cfg;
    osiMutexUnlock(d->lock);

    prvStoreNv(&cfg);
    return true;
}

/**
 * Set volume for voice call
 */
bool audevSetPlayVolume(unsigned vol)
{
    if (vol > AUDEV_VOL_MAX)
        return false;

    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);

    d->cfg.play_vol = vol;
    if (d->clk_users & AUDEV_CLK_USER_PLAY)
        prvSetPlayConfig();

    audevSetting_t cfg = d->cfg;
    osiMutexUnlock(d->lock);

    prvStoreNv(&cfg);
    return true;
}

/**
 * Get volume for voice call
 */
unsigned audevGetVoiceVolume(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->cfg.voice_vol;
}

/**
 * Get volume for play
 */
unsigned audevGetPlayVolume(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->cfg.play_vol;
}

/**
 * Get current output mute/unmute
 */
bool audevSetOutputMute(bool mute)
{
    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);

    d->cfg.out_mute = mute;
    if ((d->clk_users & AUDEV_CLK_USER_VOICE) || (d->clk_users & AUDEV_CLK_USER_POC))
        prvSetVoiceConfig();
    else if (d->clk_users & AUDEV_CLK_USER_PLAY)
        prvSetPlayConfig();

    audevSetting_t cfg = d->cfg;
    osiMutexUnlock(d->lock);

    prvStoreNv(&cfg);
    return true;
}

/**
 * Is current output muted
 */
bool audevIsOutputMute(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->cfg.out_mute;
}

/**
 * Get sample freq for mic record and voice call record
 */
uint32_t audevGetRecordSampleRate(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->cfg.sample_rate;
}

/**
 * Set sample rate for mic record and voice call record
 */
bool audevSetRecordSampleRate(uint32_t samplerate)
{
    audevContext_t *d = &gAudevCtx;
    if ((samplerate != 8000) && (samplerate != 16000))
        return false;

    osiMutexLock(d->lock);
    d->cfg.sample_rate = samplerate;
    audevSetting_t cfg = d->cfg;
    osiMutexUnlock(d->lock);

    prvStoreNv(&cfg);
    return true;
}

/**
 * Get sample time interval for mic record
 */
uint8_t audevGetRecordSampleInterval(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->cfg.sample_interval;
}

/**
 * Set sample time interval for mic record
 */
bool audevSetRecordSampleInterval(uint8_t time_ms)
{
    audevContext_t *d = &gAudevCtx;
    // 1. local mic recorder sample time interval config range in 5ms 10ms 20ms Buffer size
    if ((time_ms != 20) && (time_ms != 10) && (time_ms != 5))
        return false;

    osiMutexLock(d->lock);
    d->cfg.sample_interval = time_ms;
    audevSetting_t cfg = d->cfg;
    osiMutexUnlock(d->lock);

    prvStoreNv(&cfg);
    return true;
}

#ifdef CONFIG_AUDIO_IPC_SET_TRIGGER_MODE_ENABLE
/**
 * Set ipc trigger mode for audio player
 */
bool audevSetPlayerIpcTriggerMode(uint32_t tmode)
{
    audevContext_t *d = &gAudevCtx;
    // default in normal mode 0,cust spec use in mode 1
    if ((tmode != 0) && (tmode != 1))
        return false;

    osiMutexLock(d->lock);
    d->ipc_trigger_mode = tmode;
    osiMutexUnlock(d->lock);
    return true;
}

uint32_t audevGetPlayerIpcTriggerMode(void)
{
    audevContext_t *d = &gAudevCtx;
    return d->ipc_trigger_mode;
}
#endif

/**
 * Get Call mode Mic Gain
 */
bool audevGetMicGain(uint8_t mode, uint8_t path, uint16_t *anaGain, uint16_t *adcGain)
{
    uint8_t resultCode = 0;
    uint8_t hasMsg = 0;
    uint8_t strParam[4] = {0};
    uint8_t resultMsg[900] = {0};
    //only support in mode:VOICECALLNB,mode VOICECALLWB
    if ((mode != 0) && (mode != 5))
        return false;
    if (path > 5)
        return false;
    strParam[0] = 0;
    CSW_SetAudioCodecCalibParam(&resultCode, &hasMsg, resultMsg, mode, path,
                                AUDEV_CALIB_AUD_CODEC_GET_INGAIN, strParam, strlen((const char *)strParam));

    if (resultCode)
    {
        OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d] resultCode:%d\n", __FUNCTION__, __LINE__, resultCode);
        return false;
    }
    else
    {
        if (hasMsg)
        {
            OSI_LOGXD(OSI_LOGPAR_SIS, 0, "[%s][%d] resultMsg:%s\n", __FUNCTION__, __LINE__, resultMsg);
            *anaGain = (prvHex2Num(resultMsg[9]) << 4 | prvHex2Num(resultMsg[10])) | ((prvHex2Num(resultMsg[11]) << 4 | prvHex2Num(resultMsg[12])) << 8);
            *adcGain = (prvHex2Num(resultMsg[13]) << 4 | prvHex2Num(resultMsg[14])) | ((prvHex2Num(resultMsg[15]) << 4 | prvHex2Num(resultMsg[16])) << 8);
            OSI_LOGXD(OSI_LOGPAR_SIII, 0, "[%s][%d] anaGain:%d adcGain:%d\n", __FUNCTION__, __LINE__, *anaGain, *adcGain);
        }
        return true;
    }
}

/**
 * Set Call mode Mic Gain
 */
bool audevSetMicGain(uint8_t mode, uint8_t path, uint16_t anaGain, uint16_t adcGain)
{
    uint8_t resultCode = 0;
    uint8_t hasMsg = 0;
    uint8_t strParam[12];
    uint8_t resultMsg[900];
    //only support in mode:VOICECALLNB,mode VOICECALLWB
    if ((mode != 0) && (mode != 5))
        return false;
    if (path > 5)
        return false;
    if (anaGain > 7)
        return false;
    if (adcGain > 15)
        return false;

    strParam[0] = prvNum2Hex((anaGain >> 4) & 0xF);
    strParam[1] = prvNum2Hex(anaGain & 0xF);
    strParam[2] = prvNum2Hex((anaGain >> 12) & 0xF);
    strParam[3] = prvNum2Hex((anaGain >> 8) & 0xF);
    strParam[4] = prvNum2Hex((adcGain >> 4) & 0xF);
    strParam[5] = prvNum2Hex(adcGain & 0xF);
    strParam[6] = prvNum2Hex((adcGain >> 12) & 0xF);
    strParam[7] = prvNum2Hex((adcGain >> 8) & 0xF);
    strParam[8] = 0x0;
    OSI_LOGXD(OSI_LOGPAR_SIS, 0, "[%s][%d] strParam:%s \n", __FUNCTION__, __LINE__, strParam);
    memset(resultMsg, 0, sizeof(resultMsg));
    CSW_SetAudioCodecCalibParam(&resultCode, &hasMsg, resultMsg, mode, path,
                                AUDEV_CALIB_AUD_CODEC_SET_INGAIN, strParam, strlen((const char *)strParam));
    OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d] resultCode:%d\n", __FUNCTION__, __LINE__, resultCode);
    if (resultCode)
        return false;
    else
        return true;
}

/**
 * Get record mode Mic Gain
 */
bool audevGetRecordMicGain(uint8_t mode, uint8_t path, uint16_t *anaGain, uint16_t *adcGain)
{
    uint8_t resultCode = 0;
    uint8_t hasMsg = 0;
    uint8_t strParam[4] = {0};
    uint8_t resultMsg[900] = {0};
    //only support mode in 2:MUSICRECORD,4:FMRECORD
    if ((mode != 2) && (mode != 4))
        return false;
    //only support path in 1:Handfree,2:Handset4P,3:Handset3P
    if ((path != 1) && (mode != 2) && (mode != 3))
        return false;
    strParam[0] = 0;
    CSW_SetAudioCodecCalibParam(&resultCode, &hasMsg, resultMsg, mode, path,
                                AUDEV_CALIB_AUD_CODEC_GET_RECORDERGAIN, strParam, strlen((char *)strParam));

    if (resultCode)
    {
        OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d] resultCode:%d\n", __FUNCTION__, __LINE__, resultCode);
        return false;
    }
    else
    {
        if (hasMsg)
        {
            OSI_LOGXD(OSI_LOGPAR_SIS, 0, "[%s][%d] resultMsg:%s\n", __FUNCTION__, __LINE__, resultMsg);
            *anaGain = (prvHex2Num(resultMsg[9]) << 4 | prvHex2Num(resultMsg[10])) | ((prvHex2Num(resultMsg[11]) << 4 | prvHex2Num(resultMsg[12])) << 8);
            *adcGain = (prvHex2Num(resultMsg[13]) << 4 | prvHex2Num(resultMsg[14])) | ((prvHex2Num(resultMsg[15]) << 4 | prvHex2Num(resultMsg[16])) << 8);
            OSI_LOGXD(OSI_LOGPAR_SIII, 0, "[%s][%d] anaGain:%d adcGain:%d\n", __FUNCTION__, __LINE__, *anaGain, *adcGain);
        }
        return true;
    }
}

/**
 * Set record mode Mic Gain
 */
bool audevSetRecordMicGain(uint8_t mode, uint8_t path, uint16_t anaGain, uint16_t adcGain)
{
    uint8_t resultCode = 0;
    uint8_t hasMsg = 0;
    uint8_t strParam[12] = {0};
    uint8_t resultMsg[900] = {0};
    //only support mode in 2:MUSICRECORD,4:FMRECORD
    if ((mode != 2) && (mode != 4))
        return false;
    //only support path in 1:Handfree,2:Handset4P,3:Handset3P
    if ((path != 1) && (mode != 2) && (mode != 3))
        return false;
    if (anaGain > 7)
        return false;
    if (adcGain > 15)
        return false;

    strParam[0] = prvNum2Hex((anaGain >> 4) & 0xF);
    strParam[1] = prvNum2Hex(anaGain & 0xF);
    strParam[2] = prvNum2Hex((anaGain >> 12) & 0xF);
    strParam[3] = prvNum2Hex((anaGain >> 8) & 0xF);
    strParam[4] = prvNum2Hex((adcGain >> 4) & 0xF);
    strParam[5] = prvNum2Hex(adcGain & 0xF);
    strParam[6] = prvNum2Hex((adcGain >> 12) & 0xF);
    strParam[7] = prvNum2Hex((adcGain >> 8) & 0xF);
    strParam[8] = 0x0;
    OSI_LOGXD(OSI_LOGPAR_SIS, 0, "[%s][%d] strParam:%s \n", __FUNCTION__, __LINE__, strParam);
    memset(resultMsg, 0, sizeof(resultMsg));
    CSW_SetAudioCodecCalibParam(&resultCode, &hasMsg, resultMsg, mode, path,
                                AUDEV_CALIB_AUD_CODEC_SET_RECORDERGAIN, strParam, strlen((const char *)strParam));

    OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d] resultCode:%d\n", __FUNCTION__, __LINE__, resultCode);
    if (resultCode)
        return false;
    else
        return true;
}

/**
 * Get Call mode sidetone Gain
 */
bool audevGetSideToneGain(uint8_t mode, uint8_t path, uint8_t level, int8_t *sideGain)
{
    uint8_t resultCode = 0;
    uint8_t hasMsg = 0;
    uint8_t strParam[4] = {0};
    uint8_t resultMsg[900] = {0};
    //only support in mode:VOICECALLNB,mode VOICECALLWB
    if ((mode != 0) && (mode != 5))
        return false;
    if (path > 5)
        return false;
    if (level > 15)
        return false;

    strParam[0] = 0;
    CSW_SetAudioCodecCalibParam(&resultCode, &hasMsg, resultMsg, mode, path,
                                AUDEV_CALIB_AUD_CODEC_GET_SIDETONEGAIN, strParam, strlen((const char *)strParam));

    if (resultCode)
    {
        OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d] resultCode:%d\n", __FUNCTION__, __LINE__, resultCode);
        return false;
    }
    else
    {
        if (hasMsg)
        {
            OSI_LOGXD(OSI_LOGPAR_SIS, 0, "[%s][%d] resultMsg:%s \n", __FUNCTION__, __LINE__, resultMsg);
            *sideGain = (int8_t)(prvHex2Num(resultMsg[9 + level * 2]) << 4 | prvHex2Num(resultMsg[10 + level * 2]));
            OSI_LOGXD(OSI_LOGPAR_SIII, 0, "[%s][%d] level:%d sideGain:%d \n", __FUNCTION__, __LINE__, level, *sideGain);
        }
        return true;
    }
}

/**
 * Set Call mode sidetone Gain
 */
bool audevSetSideToneGain(uint8_t mode, uint8_t path, uint8_t level, int8_t sideGain)
{
    uint8_t resultCode = 0;
    uint8_t hasMsg = 0;
    uint8_t strParam[64] = {0};
    uint8_t resultMsg[900] = {0};
    //only support in mode:AUDEV_CALIB_AUD_MODE_VOICECALLNB,mode AUDEV_CALIB_AUD_MODE_VOICECALLWB
    if ((mode != 0) && (mode != 5))
        return false;
    if (path > 5)
        return false;
    if (level > 15)
        return false;

    if ((sideGain < -39) || (sideGain > 6))
        return false;
    if (((sideGain + 39) % 3) != 0)
        return false;

    strParam[0] = 0;
    CSW_SetAudioCodecCalibParam(&resultCode, &hasMsg, resultMsg, mode, path,
                                AUDEV_CALIB_AUD_CODEC_GET_SIDETONEGAIN, strParam, strlen((const char *)strParam));
    if (resultCode)
    {
        OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d] resultCode:%d\n", __FUNCTION__, __LINE__, resultCode);
        return false;
    }

    OSI_LOGXD(OSI_LOGPAR_SIS, 0, "[%s][%d] resultMsg:%s \n", __FUNCTION__, __LINE__, resultMsg);

    memcpy(strParam, &resultMsg[9], 32);

    strParam[0 + level * 2] = prvNum2Hex((sideGain >> 4) & 0xF);
    strParam[1 + level * 2] = prvNum2Hex(sideGain & 0xF);
    strParam[32] = 0x0;
    OSI_LOGXD(OSI_LOGPAR_SIS, 0, "[%s][%d] strParam:%s \n", __FUNCTION__, __LINE__, strParam);

    CSW_SetAudioCodecCalibParam(&resultCode, &hasMsg, resultMsg, mode, path,
                                AUDEV_CALIB_AUD_CODEC_SET_SIDETONEGAIN, strParam, strlen((const char *)strParam));

    OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d] resultCode:%d\n", __FUNCTION__, __LINE__, resultCode);
    if (resultCode)
        return false;
    else
        return true;
}

/**
 * Start voice call
 */
bool audevStartVoice(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio start voice, user/0x%x", d->clk_users);

    osiMutexLock(d->lock);

    if ((d->clk_users & ~AUDEV_CLK_USER_VOICE) != 0) // disable when any other users is working
        goto failed;

    if (d->clk_users & AUDEV_CLK_USER_VOICE)
        goto success;

    prvEnableAudioClk(AUDEV_CLK_USER_VOICE);
    if (!prvSetVoiceConfig())
        goto failed_disable_clk;

    if (DM_StartAudioEx() != 0)
        goto failed_disable_clk;

    if (!prvWaitStatus(CODEC_VOIS_START_DONE))
        goto failed_disable_clk;

success:
    osiMutexUnlock(d->lock);
    return true;

failed_disable_clk:
    prvDisableAudioClk(AUDEV_CLK_USER_VOICE);
failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio start voice failed");
    return false;
}

/**
 * Stop voice call
 */
bool audevStopVoice(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio stop voice, user/0x%x", d->clk_users);

    osiMutexLock(d->lock);

    if ((d->clk_users & AUDEV_CLK_USER_VOICE) == 0)
        goto success;

    if (DM_StopAudioEx() != 0)
        goto failed;

    if (!prvWaitStatus(CODEC_VOIS_STOP_DONE))
        goto failed;

    prvDisableAudioClk(AUDEV_CLK_USER_VOICE);

success:
    osiMutexUnlock(d->lock);
    return true;

failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio stop voice failed");
    return false;
}

/**
 * Restart voice call
 */
bool audevRestartVoice(void)
{
    OSI_LOGI(0, "audio restart voice");

    if (DM_RestartAudioEx() != 0)
        return false;

    if (!prvWaitStatus(CODEC_VOIS_START_DONE))
        return false;

    return true;
}

/**
 * Play audio tone
 */
bool audevPlayTone(audevTone_t tone, unsigned duration)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio play tone %d duration/%d user/0x%x", tone, duration, d->clk_users);
    if (duration == 0)
        return true;

    osiMutexLock(d->lock);

    if ((d->clk_users & ~AUDEV_CLK_USER_VOICE) != 0) // disable when any other users is working
        goto failed;

    prvEnableAudioClk(AUDEV_CLK_USER_TONE);
    if (!prvSetVoiceConfig())
        goto failed_disable_clk;

    unsigned vol = 0;
    if ((d->clk_users & AUDEV_CLK_USER_VOICE) != 0)
        vol = d->cfg.voice_vol;
    else
        vol = d->cfg.play_vol;
    //for volume 0,cp do nothing,ap simu the duration for tone play
    if (prvVolumeToLevel(vol, d->cfg.out_mute) == AUD_SPK_MUTE)
    {
        osiTimerStart(d->simu_toneend_timer, (duration + 200));
        goto success;
    }
    if (DM_PlayToneEx(prvToneToDmTone(tone), prvVolumeToToneAttenuatio(vol),
                      duration, prvVolumeToLevel(vol, d->cfg.out_mute)) != 0)
        goto failed_disable_clk;

success:
    osiSemaphoreTryAcquire(d->cptoneend_sema, 0);
    osiMutexUnlock(d->lock);
    return true;

failed_disable_clk:
    prvDisableAudioClk(AUDEV_CLK_USER_TONE);
failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio play tone failed");
    return false;
}

/**
 * Stop audio tone
 */
bool audevStopTone(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio stop tone, user/0x%x", d->clk_users);
    if ((d->clk_users & AUDEV_CLK_USER_TONE) == 0)
        goto success;
    OSI_LOGI(0, "audio stop tone, wait cptoneend_sema");
    osiSemaphoreAcquire(d->cptoneend_sema);
success:
    OSI_LOGI(0, "audio stop tone success");
    return true;
}

/**
 * Fill configurations for loopback test
 */
static void prvLoopbackCfg(VOIS_AUDIO_CFG_T *vois, AUD_DEVICE_CFG_EXT_T *device_ext,
                           AUD_LEVEL_T *level, HAL_AIF_STREAM_T *stream)
{
    audevContext_t *d = &gAudevCtx;

    vois->spkLevel = prvVolumeToLevel(d->loopback.volume, false);
    vois->micLevel = AUD_MIC_ENABLE;
    vois->sideLevel = AUD_SIDE_MUTE;
    vois->toneLevel = AUD_TONE_0dB;
    vois->encMute = 0;
    vois->decMute = 0;

    device_ext->inputType = prvInputToType(d->loopback.indev);
    device_ext->inputCircuityType = AUD_INPUT_MIC_CIRCUITY_TYPE_DIFFERENTIAL;
    device_ext->inputPath = AUD_INPUT_PATH1;
    device_ext->linePath = AUD_LINE_PATH2;
	
#ifndef CONFIG_QUEC_PROJECT_FEATURE_AUDIO
	device_ext->spkpaType = AUDEV_AUDIO_PA_TYPE;
#else
	device_ext->spkpaType = ql_aud_pa_type;
#endif

    level->spkLevel = prvVolumeToLevel(d->loopback.volume, false);
    level->micLevel = SND_MIC_ENABLE;
    level->sideLevel = SND_SIDE_MUTE;
    level->toneLevel = SND_TONE_0DB;
    level->appMode = SND_APP_MODE_VOICENB;

    stream->startAddress = NULL;
    stream->length = 0;
    stream->sampleRate = HAL_AIF_FREQ_8000HZ;
    stream->channelNb = HAL_AIF_MONO;
    stream->voiceQuality = 0;
    stream->playSyncWithRecord = 0;
    stream->halfHandler = NULL;
    stream->endHandler = NULL;
}

/**
 * Start loopback test
 */
bool audevStartLoopbackTest(audevOutput_t outdev, audevInput_t indev, unsigned volume)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio start loopback out/%d in/%d user/0x%x", outdev, indev, d->clk_users);

    if (!prvIsOutputValid(outdev) || !prvIsInputValid(indev))
        return false;

    osiMutexLock(d->lock);

    if ((d->clk_users & ~AUDEV_CLK_USER_VOICE) != 0) // disable when any other users is working
        goto failed;

    d->loopback.outdev = outdev;
    d->loopback.indev = indev;
    d->loopback.volume = volume;

    VOIS_AUDIO_CFG_T vois = {};
    AUD_DEVICE_CFG_EXT_T device_ext = {};
    AUD_LEVEL_T level = {};
    HAL_AIF_STREAM_T stream = {};
    prvLoopbackCfg(&vois, &device_ext, &level, &stream);

    prvEnableAudioClk(AUDEV_CLK_USER_LOOPBACK);
    if (DM_VoisSetCfg(prvOutputToAudItf(outdev), &vois, device_ext, d->btworkmode) != 0)
        goto failed_disable_clk;

    if (DM_VoisTestModeSetup(prvOutputToSndItf(outdev), &stream, &level,
                             AUD_TEST_SIDE_TEST, AUD_TEST_VOICE_MODE_PCM) != 0)
        goto failed_disable_clk;

    osiMutexUnlock(d->lock);
    return true;

failed_disable_clk:
    prvDisableAudioClk(AUDEV_CLK_USER_LOOPBACK);
failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio start loopback failed");
    return false;
}

/**
 * Stop loopback test
 */
bool audevStopLoopbackTest(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio stop loopback, user/0x%x", d->clk_users);

    osiMutexLock(d->lock);

    if ((d->clk_users & AUDEV_CLK_USER_LOOPBACK) == 0)
        goto success;

    VOIS_AUDIO_CFG_T vois = {};
    AUD_DEVICE_CFG_EXT_T device_ext = {};
    AUD_LEVEL_T level = {};
    HAL_AIF_STREAM_T stream = {};
    prvLoopbackCfg(&vois, &device_ext, &level, &stream);

    if (DM_VoisTestModeSetup(prvOutputToSndItf(d->loopback.outdev), &stream, &level,
                             AUD_TEST_NO, AUD_TEST_VOICE_MODE_PCM) != 0)
        goto failed;

    prvDisableAudioClk(AUDEV_CLK_USER_LOOPBACK);

success:
    osiMutexUnlock(d->lock);
    return true;

failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio stop loopback failed");
    return false;
}

/**
 * Get remained play time (samples in buffer)
 */
int audevPlayRemainTime(void)
{
    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);

    int ms = 0;
    if ((d->clk_users & AUDEV_CLK_USER_PLAY) && !d->play.eos_error)
    {
        unsigned bytes = prvAudioInBytes(d->shmem) + AUDEV_PLAY_HIDDEN_BUF_SIZE;
        ms = (bytes * 1000) / (d->play.sample_rate * d->play.channel_count * sizeof(int16_t));
    }
    osiMutexUnlock(d->lock);
    return ms;
}

/**
 * Start Poc Mode
 */
bool audevStartPocMode(bool duplexmode)
{
    audevContext_t *d = &gAudevCtx;
    if ((d->play.ops_ctx == NULL) || (d->record.ops_ctx == NULL))
        return false;

    OSI_LOGI(0, "audio audevStartPocMode, mode/%d ", duplexmode);

    osiMutexLock(d->lock);
    {
        if (d->clk_users != 0) // disable when any other users is working
            goto failed;
        if ((d->play.sample_rate != 8000) && (d->play.sample_rate != 16000)) //only support 8k 16k
            goto failed;
        if (d->play.sample_rate != d->cfg.sample_rate) //only support play and record in same sample rate
            goto failed;

        d->play.total_bytes = 0;
        d->play.eos_error = false;
        // 1. half buffer size is determined by channel count and sample rate, for 20ms
        // 2. half buffer size should be 32 bytes aligned
        // 3. 2x half buffer can't exceed AUDIO_OUTPUT_BUF_SIZE
        // 4. set a minimal value, based on 8000Hz mono
        // 5. the unit of audOutPara.length is word (2 bytes)
        unsigned half_buffer_bytes = (d->record.frame.channel_count * d->cfg.sample_rate * 2 / 50);
        half_buffer_bytes = OSI_ALIGN_UP(half_buffer_bytes, 32);
        half_buffer_bytes = OSI_MIN(unsigned, AUDIO_OUTPUT_BUF_SIZE / 2, half_buffer_bytes);
        half_buffer_bytes = OSI_MAX(unsigned, 320, half_buffer_bytes);
        unsigned half_buffer_words = half_buffer_bytes / 2;

        HAL_AIF_STREAM_T recStream = {
            .startAddress = ((d->cfg.sample_rate == 8000) ? (uint32_t *)d->shmem->txPcmBuffer.pcmBuf : (uint32_t *)d->shmem->txPcmVolte.pcmBuf),
            .length = half_buffer_bytes * 2,
            .channelNb = HAL_AIF_MONO,
            .playSyncWithRecord = true,
            .sampleRate = d->cfg.sample_rate,
            .voiceQuality = true,
            .halfHandler = NULL,
            .endHandler = NULL,
        };
        HAL_AIF_STREAM_T playStream = {
            .startAddress = ((d->cfg.sample_rate == 8000) ? (uint32_t *)d->shmem->rxPcmBuffer.pcmBuf : (uint32_t *)d->shmem->rxPcmVolte.pcmBuf),
            .length = half_buffer_bytes * 2,
            .channelNb = HAL_AIF_MONO,
            .playSyncWithRecord = true,
            .sampleRate = d->cfg.sample_rate,
            .voiceQuality = true,
            .halfHandler = NULL,
            .endHandler = NULL,
        };

        AUD_LEVEL_T level = {
            .spkLevel = prvVolumeToLevel(d->cfg.voice_vol, d->cfg.out_mute),
            .micLevel = d->voice_uplink_mute ? AUD_MIC_MUTE : AUD_MIC_ENABLE,
            .sideLevel = SND_SIDE_MUTE,
            .toneLevel = SND_TONE_0DB,
            .appMode = ((d->cfg.sample_rate == 8000) ? SND_APP_MODE_VOICENB : SND_APP_MODE_VOICEWB),
        };

        d->shmem->audInPara.sbcOutFlag = 0;
        d->shmem->audInPara.sampleRate = d->cfg.sample_rate;
        d->shmem->audOutPara.samplerate = d->cfg.sample_rate;
        d->shmem->audInPara.channelNb = playStream.channelNb;
        d->shmem->audOutPara.channelNb = recStream.channelNb;
        d->shmem->audInPara.bitsPerSample = sizeof(int16_t) * 8; // S16, in bits
        d->shmem->musicMode = ZSP_MUSIC_MODE_DUPLEX_POC;
        d->shmem->audOutPara.length = half_buffer_words;
        d->shmem->updateParaInd = 0;
        d->shmem->audInPara.inLenth = AUDIO_INPUT_BUF_SIZE;
        d->shmem->audInPara.fileEndFlag = 0;
        d->shmem->audInPara.readOffset = 0;
        d->shmem->audInPara.writeOffset = 0;
        d->shmem->traceDataFlag = (duplexmode) ? AUDEV_POC_MODE_PLAY_RECORD : AUDEV_POC_MODE_PLAY;
        memset(d->shmem->audOutput, 0, AUDIO_OUTPUT_BUF_SIZE);

        d->shmem->audOutPara.readOffset = 0;
        d->shmem->audOutPara.writeOffset = 0;

        osiDCacheCleanAll();

        prvEnableAudioClk(AUDEV_CLK_USER_POC);

        if (DM_ZspMusicPlayStart() != 0)
            goto failed_disable_clk;

        if (!prvWaitStatus(ZSP_START_DONE))
            goto failed_disable_clk;

        if (!prvSetVoiceConfig())
            goto failed_disable_clk;

        if (DM_AudStreamStart(prvOutputToSndItf(d->cfg.outdev), &playStream, &level) != 0)
            goto failed_disable_clk;

        if (!prvWaitStatus(CODEC_STREAM_START_DONE))
            goto failed_disable_clk;

        if (DM_AudStreamRecord(prvOutputToSndItf(d->cfg.outdev), &recStream, &level) != 0)
            goto failed_disable_clk;

        if (!prvWaitStatus(CODEC_RECORD_START_DONE))
            goto failed_disable_clk;

        d->shmem->traceDataFlag |= AUDEV_POC_IPC_GETFRAME_IND;
        osiWorkEnqueue(d->ipc_work, d->wq);

        osiMutexUnlock(d->lock);
        return true;
    }

failed_disable_clk:
    prvDisableAudioClk(AUDEV_CLK_USER_POC);
failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio start play failed");
    return false;
}

bool audevStopPocMode(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio record stop, user/0x%x", d->clk_users);

    osiMutexLock(d->lock);

    if ((d->clk_users & AUDEV_CLK_USER_POC) == 0)
        goto success;

    if (DM_AudStreamStop(prvOutputToSndItf(d->cfg.outdev)) != 0)
        goto failed;

    if (!prvWaitStatus(CODEC_STREAM_STOP_DONE))
        goto failed;

    if (DM_ZspMusicPlayStop() != 0)
        goto failed;

    if (!prvWaitStatus(ZSP_STOP_DONE))
        goto failed;

    prvDisableAudioClk(AUDEV_CLK_USER_POC);

success:
    osiMutexUnlock(d->lock);
    return true;

failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio stop record failed");
    return false;
}
bool audevPocModeSwitch(uint8_t mode)
{
    audevContext_t *d = &gAudevCtx;
    //1:half duplex mode record send 2:half duplex mode play recv
    if ((mode != 1) && (mode != 2))
        return false;

    if (d->play.ops_ctx == NULL || d->record.ops_ctx == NULL)
        return false;

    OSI_LOGI(0, "audio audevPocModeSwitch, mode/%d ", mode);

    osiMutexLock(d->lock);
    if (mode == 1)
    {
        //clear play shmem in half duplex record mode
        memset(&d->shmem->rxPcmVolte, 0, sizeof(HAL_SPEECH_PCM_BUF_T) * 2);
        d->shmem->traceDataFlag = AUDEV_POC_MODE_RECORD;
    }
    if (mode == 2)
        d->shmem->traceDataFlag = AUDEV_POC_MODE_PLAY;
    osiMutexUnlock(d->lock);
    return true;
}

/**
 * Start play
 */
bool audevStartPlayV2(audevPlayType_t type, const audevPlayOps_t *play_ops, void *play_ctx,
                      const auFrame_t *frame)
{
    audevContext_t *d = &gAudevCtx;
    if (play_ops == NULL || play_ops->get_frame == NULL || play_ops->data_consumed == NULL)
        return false;
    if (frame == NULL)
        return false;

    OSI_LOGI(0, "audio start play, type/%d sample/%d channels/%d rate/%d user/0x%x", type,
             frame->sample_format, frame->channel_count,
             frame->sample_rate, d->clk_users);

    const uint32_t valid_samplerate[] = {
        8000, 9600, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 96000};

    if (!osiIsUintInList(frame->sample_rate, valid_samplerate, OSI_ARRAY_SIZE(valid_samplerate)))
        return false;
    if (frame->sample_format != AUSAMPLE_FORMAT_S16)
        return false;
    if (frame->channel_count != 1 && frame->channel_count != 2)
        return false;

    osiMutexLock(d->lock);
    d->play.type = type;
    if (type == AUDEV_PLAY_TYPE_LOCAL)
    {
        if ((d->clk_users & ~AUDEV_CLK_USER_VOICE) != 0) // disable when any other users is working
            goto failed;

        d->play.channel_count = frame->channel_count;
        d->play.sample_rate = frame->sample_rate;
        d->play.total_bytes = 0;
        d->play.eos_error = false;
        d->play.ops = *play_ops;
        d->play.ops_ctx = play_ctx;

        // 1. half buffer size is determined by channel count and sample rate, for 20ms
        // 2. half buffer size should be 32 bytes aligned
        // 3. 2x half buffer can't exceed AUDIO_OUTPUT_BUF_SIZE
        // 4. set a minimal value, based on 8000Hz mono
        // 5. the unit of audOutPara.length is word (2 bytes)
        unsigned half_buffer_bytes = (frame->channel_count * frame->sample_rate * 2 / 50);
        half_buffer_bytes = OSI_ALIGN_UP(half_buffer_bytes, 32);
        half_buffer_bytes = OSI_MIN(unsigned, AUDIO_OUTPUT_BUF_SIZE / 2, half_buffer_bytes);
        half_buffer_bytes = OSI_MAX(unsigned, 320, half_buffer_bytes);
        unsigned half_buffer_words = half_buffer_bytes / 2;

        HAL_AIF_STREAM_T stream = {
            .startAddress = (UINT32 *)d->shmem->audOutput,
            .length = half_buffer_bytes * 2,
            .channelNb = (frame->channel_count == 1) ? HAL_AIF_MONO : HAL_AIF_STEREO,
            .playSyncWithRecord = 0,
            .sampleRate = frame->sample_rate,
            .voiceQuality = false,
            .halfHandler = NULL,
            .endHandler = NULL,
        };
        AUD_LEVEL_T level = {
            .spkLevel = prvVolumeToLevel(d->cfg.play_vol, d->cfg.out_mute),
            .micLevel = SND_MIC_MUTE,
            .sideLevel = SND_SIDE_MUTE,
            .toneLevel = SND_TONE_0DB,
            .appMode = SND_APP_MODE_MUSIC,
        };

        d->shmem->audInPara.sbcOutFlag = 0;
        d->shmem->audInPara.sampleRate = frame->sample_rate;
        d->shmem->audOutPara.samplerate = frame->sample_rate;
        d->shmem->audInPara.channelNb = stream.channelNb;
        d->shmem->audOutPara.channelNb = stream.channelNb;
        d->shmem->audInPara.bitsPerSample = sizeof(int16_t) * 8; // S16, in bits
        d->shmem->musicMode = ZSP_MUSIC_MODE_PLAY_PCM;
        d->shmem->audOutPara.length = half_buffer_words;
#ifdef CONFIG_AUDIO_IPC_SET_TRIGGER_MODE_ENABLE
        d->shmem->audOutPara.ipcrate = audevGetPlayerIpcTriggerMode();
#else
        d->shmem->audOutPara.ipcrate = 0;
#endif
        d->shmem->updateParaInd = 0;
        d->shmem->audInPara.inLenth = AUDIO_INPUT_BUF_SIZE;
        d->shmem->audInPara.fileEndFlag = 0;
        d->shmem->audInPara.readOffset = 0;
        d->shmem->audInPara.writeOffset = 0;
        memset(d->shmem->audOutput, 0, AUDIO_OUTPUT_BUF_SIZE);
        osiDCacheCleanAll();

        prvEnableAudioClk(AUDEV_CLK_USER_PLAY);

#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE
        if (d->cfg.ext_i2s_en)
        {

            g_pingpanghalfbytes = half_buffer_bytes;
            HAL_AIF_SERIAL_CFG_T serialCfg = {
                .mode = 0,
                .aifIsMaster = false,
                .lsbFirst = false,
                .polarity = false,
                .rxDelay = 0,
                .txDelay = 1,
                .rxMode = 1,
                .txMode = 2,
                .fs = frame->sample_rate,
                .bckLrckRatio = 32,
                .invertBck = false,
                .outputHalfCycleDelay = true,
                .inputHalfCycleDelay = false,
                .enableBckOutGating = false,
            };

            prvExtI2sOutputSetIrqHandler();
            prvAudExtCodecI2cOpen(DRV_NAME_I2C2, DRV_I2C_BPS_100K);

            SND_ITF_T itf = prvOutputToSndItf(d->cfg.outdev);

            if ((itf != SND_ITF_BLUETOOTH) && (level.appMode != SND_APP_MODE_FMPLAY))
                prvAudI2sAifCfg(&stream, &serialCfg);

            if (hal_AifOpen(&g_audCodecAifCfg) != HAL_ERR_NO) //BCLK LRCK
            {
                OSI_LOGE(0, "hal_AifOpen fail\n");
                goto failed_disable_clk;
            }

            //set external device reg
            prvAudWriteRegList(g_audPlayRegList, sizeof(g_audPlayRegList) / sizeof(extDevReg_t));

            //You can add other samplerates,such as 32k,48k.
            if (frame->sample_rate == 44100)
            {
                prvAudWriteRegList(g_aud44kRegList, sizeof(g_aud44kRegList) / sizeof(extDevReg_t));
            }
            else if (frame->sample_rate == 16000)
            {
                prvAudWriteRegList(g_aud16kRegList, sizeof(g_aud16kRegList) / sizeof(extDevReg_t));
            }

            //set outpath Reg
            prvExtI2sSetPlayConfig();
            // Send the stream through the IFC
            if (hal_AifPlayStream(&stream) != HAL_ERR_NO)
            {
                OSI_LOGE(0, "hal_AifPlayStream Resource Busy");
                goto failed_disable_clk;
            }
        }
        else
#endif
        {
            if (DM_ZspMusicPlayStart() != 0)
                goto failed_disable_clk;

            if (!prvWaitStatus(ZSP_START_DONE))
                goto failed_disable_clk;

            aud_SetSharkingMode(0);
            if (DM_AudStreamStart(prvOutputToSndItf(d->cfg.outdev), &stream, &level) != 0)
                goto failed_disable_clk;

            if (!prvWaitStatus(CODEC_STREAM_START_DONE))
                goto failed_disable_clk;
        }

        //for volume level change
        memcpy(&(d->play.level), &level, sizeof(AUD_LEVEL_T));

        osiWorkEnqueue(d->ipc_work, d->wq);
        osiMutexUnlock(d->lock);
        return true;
    }
    else if (type == AUDEV_PLAY_TYPE_VOICE)
    {
        if ((d->clk_users & AUDEV_CLK_USER_VOICE) == 0) // only available in voice call
            goto failed;
        if ((d->clk_users & ~AUDEV_CLK_USER_VOICE) != 0) // disable when any other users is working
            goto failed;

        d->play.channel_count = frame->channel_count;
        d->play.sample_rate = frame->sample_rate;
        d->play.total_bytes = 0;
        d->play.eos_error = false;
        d->play.ops = *play_ops;
        d->play.ops_ctx = play_ctx;

        // 1. half buffer size is determined by channel count and sample rate, for 20ms
        // 2. half buffer size should be 32 bytes aligned
        // 3. 2x half buffer can't exceed AUDIO_OUTPUT_BUF_SIZE
        // 4. set a minimal value, based on 8000Hz mono
        // 5. the unit of audOutPara.length is word (2 bytes)
        unsigned half_buffer_bytes = (frame->channel_count * frame->sample_rate * 2 / 50);
        half_buffer_bytes = OSI_ALIGN_UP(half_buffer_bytes, 32);
        half_buffer_bytes = OSI_MIN(unsigned, AUDIO_OUTPUT_BUF_SIZE / 2, half_buffer_bytes);
        half_buffer_bytes = OSI_MAX(unsigned, 320, half_buffer_bytes);
        unsigned half_buffer_words = half_buffer_bytes / 2;

        d->shmem->audInPara.sbcOutFlag = 0;
        d->shmem->audInPara.sampleRate = frame->sample_rate;
        d->shmem->audOutPara.samplerate = frame->sample_rate;
        d->shmem->audInPara.channelNb = (frame->channel_count == 1) ? HAL_AIF_MONO : HAL_AIF_STEREO;
        d->shmem->audOutPara.channelNb = (frame->channel_count == 1) ? HAL_AIF_MONO : HAL_AIF_STEREO;
        d->shmem->audInPara.bitsPerSample = sizeof(int16_t) * 8; // S16, in bits
        //d->shmem->musicMode = ZSP_MUSIC_MODE_PLAY_PCM;
        d->shmem->audOutPara.length = half_buffer_words;
        d->shmem->audOutPara.ipcrate = 0;
        d->shmem->updateParaInd = 0;
        d->shmem->audInPara.inLenth = AUDIO_INPUT_BUF_SIZE;
        d->shmem->audInPara.fileEndFlag = 0;
        d->shmem->audInPara.readOffset = 0;
        d->shmem->audInPara.writeOffset = 0;
        memset(d->shmem->audOutput, 0, AUDIO_OUTPUT_BUF_SIZE);
        osiDCacheCleanAll();

        prvEnableAudioClk(AUDEV_CLK_USER_PLAY);

        if (hal_zspVoicePlayStart() != 0)
        {
            goto failed;
        }
        else
        {
            osiWorkEnqueue(d->ipc_work, d->wq);
            osiMutexUnlock(d->lock);
        }
        return true;
    }
    else if (type == AUDEV_PLAY_TYPE_POC) //poc
    {
        if (d->clk_users != 0) // disable when any other users is working
            goto failed;
        if ((frame->sample_rate != 8000) && (frame->sample_rate != 16000)) //only support 8k 16k
            goto failed;
        if (frame->sample_rate != d->cfg.sample_rate) //only support play and record in same sample rate
            goto failed;

        d->play.channel_count = frame->channel_count;
        d->play.sample_rate = frame->sample_rate;
        d->play.total_bytes = 0;
        d->play.eos_error = false;
        d->play.ops = *play_ops;
        d->play.ops_ctx = play_ctx;

        osiMutexUnlock(d->lock);
        return true;
    }

failed_disable_clk:
#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE
    if (d->cfg.ext_i2s_en)
        prvExtI2sOutputDisableIrq();
#endif
    prvDisableAudioClk(AUDEV_CLK_USER_PLAY);
failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio start play failed");
    return false;
}

/**
 * Start play
 */
bool audevStartPlay(const audevPlayOps_t *play_ops, void *play_ctx,
                    const auFrame_t *frame)
{
    return (audevStartPlayV2(AUDEV_PLAY_TYPE_LOCAL, play_ops, play_ctx, frame));
}

/**
 * Stop play
 */
bool audevStopPlayV2(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio play stop, user/0x%x,type=%d", d->clk_users, d->play.type);

    osiMutexLock(d->lock);
    if (d->play.type == AUDEV_PLAY_TYPE_LOCAL)
    {
        if ((d->clk_users & AUDEV_CLK_USER_PLAY) == 0)
            goto success;

#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE
        if (d->cfg.ext_i2s_en)
        {

            //I2C set external codec
            prvAudWriteRegList(g_audCloseRegList, sizeof(g_audCloseRegList) / sizeof(extDevReg_t));

            if (hal_AifStopPlay() != HAL_ERR_NO)
            {
                OSI_LOGE(0, "hal_AifStopPlay Resource Busy");
                goto failed;
            }

            hal_AifClose();

            prvExtI2sOutputDisableIrq();

            prvAudExtCodecI2cClose();
        }
        else
#endif
        {
            osiTimerStop(d->finish_timer);

            if (DM_AudStreamStop(prvOutputToSndItf(d->cfg.outdev)) != 0)
                goto failed;

            if (!prvWaitStatus(CODEC_STREAM_STOP_DONE))
                goto failed;

            if (DM_ZspMusicPlayStop() != 0)
                goto failed;

            if (!prvWaitStatus(ZSP_STOP_DONE))
                goto failed;
        }

        prvDisableAudioClk(AUDEV_CLK_USER_PLAY);
    }
    else if (d->play.type == AUDEV_PLAY_TYPE_POC)
    {
        osiTimerStop(d->finish_timer);
    }
    else
    {
        if (hal_zspVoicePlayStop() != 0)
            goto failed;

        prvDisableAudioClk(AUDEV_CLK_USER_PLAY);
    }

success:
    osiMutexUnlock(d->lock);
    return true;

failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio stop play failed");
    return false;
}

/**
 * Stop play
 */
bool audevStopPlay(void)
{
    return (audevStopPlayV2());
}

/**
 * Start record
 */
bool audevStartRecord(audevRecordType_t type, const audevRecordOps_t *rec_ops, void *rec_ctx)
{
    audevContext_t *d = &gAudevCtx;
    SND_ITF_T itf = SND_ITF_RECEIVER;

    if (rec_ops == NULL || rec_ops->put_frame == NULL)
        return false;

    OSI_LOGI(0, "audio record start, type/%d user/0x%x", type, d->clk_users);

    osiMutexLock(d->lock);

    if (type == AUDEV_RECORD_TYPE_MIC)
    {
        if ((d->clk_users & AUDEV_CLK_USER_VOICE)) // not support in voice call
            goto failed;
        if ((d->clk_users & ~(AUDEV_CLK_USER_VOICE | AUDEV_CLK_USER_PLAYTEST)) != 0) // disable when any other users is working
            goto failed;

        // 1. local mic recorder sample time interval config range in 5ms - 20ms Buffer size
        // 2. pcm_buffer_bytes >=5ms buffersize <= 20ms buffersize
        unsigned max_buffer_bytes = ((d->cfg.sample_rate == 8000) ? sizeof(HAL_SPEECH_PCM_BUF_T) : sizeof(HAL_VOLTE_PCM_BUF_T));
        unsigned pcm_buffer_bytes = max_buffer_bytes / (20 / d->cfg.sample_interval);
        pcm_buffer_bytes = OSI_MIN(unsigned, max_buffer_bytes, pcm_buffer_bytes);
        pcm_buffer_bytes = OSI_MAX(unsigned, (max_buffer_bytes / 4), pcm_buffer_bytes);

        HAL_AIF_STREAM_T stream = {
            .startAddress = ((d->cfg.sample_rate == 8000) ? (uint32_t *)d->shmem->txPcmBuffer.pcmBuf : (uint32_t *)d->shmem->txPcmVolte.pcmBuf),
            .length = pcm_buffer_bytes,
            .sampleRate = d->cfg.sample_rate,
            .channelNb = HAL_AIF_MONO,
            .voiceQuality = 0,
            .playSyncWithRecord = 0,
            .halfHandler = NULL,
            .endHandler = NULL,
        };
        AUD_LEVEL_T level = {
            .spkLevel = prvVolumeToLevel(d->cfg.play_vol, d->cfg.out_mute),
            .micLevel = SND_MIC_ENABLE,
            .sideLevel = SND_SIDE_VOL_15,
            .toneLevel = SND_SIDE_VOL_15,
            .appMode = SND_APP_MODE_RECORD,
        };

        d->record.type = type;
        d->record.ops = *rec_ops;
        d->record.ops_ctx = rec_ctx;
        d->record.enc_error = false;

        d->record.frame.sample_format = AUSAMPLE_FORMAT_S16;
        d->record.frame.channel_count = 1;
        d->record.frame.sample_rate = d->cfg.sample_rate;

#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE
        if (d->cfg.ext_i2s_en)
        {
            g_pingpanghalfbytes = stream.length / 2;
            //osiIrqSetHandler	//register irq
            prvExtI2sInputSetIrqHandler();
        }
        else
#endif
        {
            d->shmem->updateParaInd = 0;
            d->shmem->audInPara.sampleRate = d->cfg.sample_rate;
            d->shmem->audInPara.channelNb = HAL_AIF_MONO;
            d->shmem->audOutPara.channelNb = HAL_AIF_MONO;
            d->shmem->audInPara.bitsPerSample = sizeof(int16_t) * 8; // S16, in bits
            d->shmem->musicMode = ZSP_MUSIC_MODE_RECORD_WAV;
            d->shmem->audInPara.readOffset = 0;
            d->shmem->audInPara.writeOffset = 0;
            d->shmem->audInPara.sbcOutFlag = 0;
            d->shmem->audInPara.fileEndFlag = 0;
            d->shmem->audInPara.inLenth = AUDIO_INPUT_BUF_SIZE;
            d->shmem->audInPara.outLength = pcm_buffer_bytes / 2; //ping/pang length
            d->shmem->audOutPara.samplerate = HAL_AIF_FREQ_8000HZ;
            d->shmem->audOutPara.length = AUDEV_PLAY_OUT_BUF_SIZE;
            memset(d->shmem->audOutput, 0, AUDIO_OUTPUT_BUF_SIZE);
        }
        prvEnableAudioClk(AUDEV_CLK_USER_RECORD);

#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE
        if (d->cfg.ext_i2s_en)
        {
            HAL_AIF_SERIAL_CFG_T serialCfg = {
                .mode = 0,
                .aifIsMaster = false,
                .lsbFirst = false,
                .polarity = false,
                .rxDelay = 0,
                .txDelay = 1,
                .rxMode = 1,
                .txMode = 2,
                .fs = d->cfg.sample_rate,
                .bckLrckRatio = 32,
                .invertBck = false,
                .outputHalfCycleDelay = true,
                .inputHalfCycleDelay = false,
                .enableBckOutGating = false,
            };

            prvAudExtCodecI2cOpen(DRV_NAME_I2C2, DRV_I2C_BPS_100K);
            OSI_LOGE(0, "audio start record itf = %d", prvOutputToSndItf(d->cfg.outdev));
            if ((d->clk_users & AUDEV_CLK_USER_PLAYTEST) != 0)
            {
                itf = prvOutputToSndItf(d->bbat.outdev);
            }
            else
            {
                itf = prvOutputToSndItf(d->cfg.outdev);
            }

            if ((itf != SND_ITF_BLUETOOTH) && (level.appMode != SND_APP_MODE_FMPLAY))
                prvAudI2sAifCfg(&stream, &serialCfg);

            if (hal_AifOpen(&g_audCodecAifCfg) != HAL_ERR_NO) //BCLK LRCK
            {
                OSI_LOGE(0, "hal_AifOpen fail\n");
                goto failed_disable_clk;
            }

            //I2C set external codec
            prvAudWriteRegList(g_audRecRegList, sizeof(g_audRecRegList) / sizeof(extDevReg_t));

            if (d->cfg.sample_rate == 16000)
            {
                prvAudWriteRegList(g_aud16kRegList, sizeof(g_aud16kRegList) / sizeof(extDevReg_t));
            }

            prvExtI2sSetDeviceExt();

            // Send the stream through the IFC
            if (hal_AifRecordStream(&stream) != HAL_ERR_NO)
            {
                OSI_LOGE(0, "hal_AiRecordStream Resource Busy");
                goto failed_disable_clk;
            }
        }
        else
#endif
        {
            prvSetDeviceExt();
            if (DM_ZspMusicPlayStart() != 0)
                goto failed_disable_clk;

            if (!prvWaitStatus(ZSP_START_DONE))
                goto failed_disable_clk;

            OSI_LOGE(0, "audio start record itf = %d", prvOutputToSndItf(d->cfg.outdev));
            if ((d->clk_users & AUDEV_CLK_USER_PLAYTEST) != 0)
            {
                itf = prvOutputToSndItf(d->bbat.outdev);
            }
            else
            {
                itf = prvOutputToSndItf(d->cfg.outdev);
            }

            if (DM_AudStreamRecord(itf, &stream, &level) != 0)
                goto failed_disable_clk;

            if (!prvWaitStatus(CODEC_RECORD_START_DONE))
                goto failed_disable_clk;
        }
    }
    else if (type == AUDEV_RECORD_TYPE_VOICE ||
             type == AUDEV_RECORD_TYPE_VOICE_DUAL ||
             type == AUDEV_RECORD_TYPE_DEBUG_DUMP)
    {
        if ((d->clk_users & AUDEV_CLK_USER_VOICE) == 0) // only available in voice call
            goto failed;
        if ((d->clk_users & ~AUDEV_CLK_USER_VOICE) != 0) // disable when any other users is working
            goto failed;

        unsigned out_channel_count;
        if (type == AUDEV_RECORD_TYPE_VOICE)
            out_channel_count = 1;
        else if (type == AUDEV_RECORD_TYPE_VOICE_DUAL)
            out_channel_count = 2;
        else
            out_channel_count = 6;

        unsigned recformat = (type == AUDEV_RECORD_TYPE_VOICE)
                                 ? ZSP_VOICE_RECORD_FORMAT_PCM
                                 : ZSP_VOICE_RECORD_FORMAT_DUMP;

        d->record.type = type;
        d->record.ops = *rec_ops;
        d->record.ops_ctx = rec_ctx;
        d->record.enc_error = false;

        d->record.frame.sample_format = AUSAMPLE_FORMAT_S16;
        d->record.frame.channel_count = out_channel_count;
        d->record.frame.sample_rate = d->cfg.sample_rate;

        d->shmem->audInPara.sampleRate = d->cfg.sample_rate;
        d->shmem->audInPara.readOffset = 0;
        d->shmem->audInPara.writeOffset = 0;
        d->shmem->audInPara.inLenth = AUDIO_INPUT_BUF_SIZE;
        d->shmem->voiceRecFormat = recformat;

        prvEnableAudioClk(AUDEV_CLK_USER_RECORD);
        if (hal_zspVoiceRecordStart() != 0)
            goto failed_disable_clk;
    }
    else if (AUDEV_RECORD_TYPE_POC == type)
    {
        if (d->clk_users != 0) // disable when any other users is working
            goto failed;

        d->record.type = type;
        d->record.ops = *rec_ops;
        d->record.ops_ctx = rec_ctx;

        d->record.frame.sample_format = AUSAMPLE_FORMAT_S16;
        d->record.frame.channel_count = 1;
        d->record.frame.sample_rate = d->cfg.sample_rate;
    }
    else
    {
        goto failed;
    }

    osiMutexUnlock(d->lock);
    return true;

failed_disable_clk:
#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE
    if (d->cfg.ext_i2s_en)
        prvExtI2sInputDisableIrq();
#endif
    prvDisableAudioClk(AUDEV_CLK_USER_RECORD);
failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio start record failed");
    return false;
}

/**
 * Stop record
 */
bool audevStopRecord(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio record stop, user/0x%x", d->clk_users);

    osiMutexLock(d->lock);

    if ((d->clk_users & AUDEV_CLK_USER_RECORD) == 0)
        goto success;

    if (d->record.type == AUDEV_RECORD_TYPE_MIC)
    {
#ifdef CONFIG_AUDIO_EXT_I2S_ENABLE
        if (d->cfg.ext_i2s_en)
        {

            //I2C set external codec
            prvAudWriteRegList(g_audCloseRegList, sizeof(g_audCloseRegList) / sizeof(extDevReg_t));

            if (hal_AifStopRecord() != HAL_ERR_NO)
            {
                OSI_LOGE(0, "hal_AifStopRecord Resource Busy");
                goto failed;
            }

            hal_AifClose();

            prvExtI2sInputDisableIrq();

            prvAudExtCodecI2cClose();
        }
        else
#endif
        {
            if (DM_AudStreamStop(prvOutputToSndItf(d->cfg.outdev)) != 0)
                goto failed;

            if (!prvWaitStatus(CODEC_STREAM_STOP_DONE))
                goto failed;

            if (DM_ZspMusicPlayStop() != 0)
                goto failed;

            if (!prvWaitStatus(ZSP_STOP_DONE))
                goto failed;
        }

        prvDisableAudioClk(AUDEV_CLK_USER_RECORD);
    }
    else if (d->record.type == AUDEV_RECORD_TYPE_POC)
    {
    }
    else
    {
        if (hal_zspVoiceRecordStop() != 0)
            goto failed;

        prvDisableAudioClk(AUDEV_CLK_USER_RECORD);
    }

success:
    osiMutexUnlock(d->lock);
    return true;

failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio stop record failed");
    return false;
}

bool audevRestartVoiceRecord(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio restart voice record, user/0x%x", d->clk_users);
    osiMutexLock(d->lock);
    if ((d->clk_users & AUDEV_CLK_USER_VOICE) == 0) // only available in voice call
        goto failed;
    if ((d->clk_users & AUDEV_CLK_USER_RECORD) == 0) // only available in voice call record
        goto failed;

    unsigned recformat = (d->record.type == AUDEV_RECORD_TYPE_VOICE)
                             ? ZSP_VOICE_RECORD_FORMAT_PCM
                             : ZSP_VOICE_RECORD_FORMAT_DUMP;

    d->shmem->audInPara.sampleRate = d->cfg.sample_rate;
    d->shmem->audInPara.readOffset = 0;
    d->shmem->audInPara.writeOffset = 0;
    d->shmem->audInPara.inLenth = AUDIO_INPUT_BUF_SIZE;
    d->shmem->voiceRecFormat = recformat;

    if (hal_zspVoiceRecordStart() != 0)
        goto failed;

    osiMutexUnlock(d->lock);
    return true;

failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio restart voice record failed");
    return false;
}

/**
 * Start bbat play
 */
bool audevStartPlayTest(audevOutput_t outdev, const osiBufSize32_t *buf)
{
    audevContext_t *d = &gAudevCtx;
    if (buf == NULL || buf->size == 0 || buf->size != 704 || !prvIsOutputValid(outdev))
        return false;

    OSI_LOGI(0, "audio start bbat play, output/%d user/0x%x", outdev, d->clk_users);

    osiMutexLock(d->lock);

    if ((d->clk_users & AUDEV_CLK_USER_PLAYTEST) != 0) // disable when any user is working
        goto failed;

    d->bbat.outdev = outdev;

    prvEnableAudioClk(AUDEV_CLK_USER_PLAYTEST);
    OSI_LOGE(0, "audio play start itf = %d", prvOutputToSndItf(outdev));
    if (aud_bbatPcmBufferPlayStart(prvOutputToSndItf(outdev), (BBAT_PCM_STREAM_T *)buf) != 0)
        goto failed_disable_clock;

    if (!prvWaitStatus(CODEC_STREAM_START_DONE))
        goto failed_disable_clock;

    osiMutexUnlock(d->lock);
    return true;

failed_disable_clock:
    prvDisableAudioClk(AUDEV_CLK_USER_PLAYTEST);
failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio start bbat play failed");
    return false;
}

/**
 * Stop bbat play
 */
bool audevStopPlayTest(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio play stop bbat play, user/0x%x", d->clk_users);

    osiMutexLock(d->lock);

    if ((d->clk_users & AUDEV_CLK_USER_PLAYTEST) == 0)
        goto success;

    if (aud_bbatPcmBufferPlayStop(prvOutputToSndItf(d->bbat.outdev)) != 0)
        goto failed;

    if (!prvWaitStatus(CODEC_STREAM_STOP_DONE))
        goto failed;

    prvDisableAudioClk(AUDEV_CLK_USER_PLAYTEST);

success:
    osiMutexUnlock(d->lock);
    return true;

failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio stop bbat play failed");
    return false;
}

void audSetLdoVB(uint32_t en)
{
    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);
    aud_SetLdoVB(en);
    osiMutexUnlock(d->lock);
}
void audHeadSetBBATMode(uint8_t mictype, bool en)
{
    aud_HeadsetConfig(mictype, en);
}

bool audIsCodecOpen(void)
{
    audevContext_t *d = &gAudevCtx;
    return (d->clk_users != 0);
}

void audHeadsetdepop_en(bool en, uint8_t mictype)
{
    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);
    if (en)
    {
        //four seg heaset set 1,three seg headset set 2
        if (mictype == 2)
            aud_HeadsetConfig(1, 0);
        else
            aud_HeadsetConfig(2, 0);
    }
    else
        aud_HeadsetConfig(0, 0);
    osiMutexUnlock(d->lock);
}

bool audevBtVoiceStart(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio bt voice start, user/0x%x, btworkmode/%d", d->clk_users, d->btworkmode);

    osiMutexLock(d->lock);

    if (d->btworkmode == SND_BT_WORK_MODE_NO)
        goto failed;

    if ((d->clk_users & ~AUDEV_CLK_USER_VOICE) != 0) // disable when any other users is working
        goto failed;

    if (d->clk_users & AUDEV_CLK_USER_VOICE) // not support in voice call,just bt call speaker need this state
        goto success;

    prvEnableAudioClk(AUDEV_CLK_USER_VOICE);
    if (!prvSetVoiceConfig())
        goto failed_disable_clk;

    if (hal_btVoiceStart() == 0)
        goto success;
    else
        goto failed_disable_clk;

success:
    osiMutexUnlock(d->lock);
    return true;

failed_disable_clk:
    prvDisableAudioClk(AUDEV_CLK_USER_VOICE);
failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio bt voice start failed");
    return false;
}

bool audevBtVoiceStop(void)
{
    audevContext_t *d = &gAudevCtx;
    OSI_LOGI(0, "audio bt voice stop, user/0x%x", d->clk_users);

    osiMutexLock(d->lock);

    if ((d->clk_users & AUDEV_CLK_USER_VOICE) == 0)
        goto success;

    if (hal_btVoiceStop() != 0)
        goto failed;

    prvDisableAudioClk(AUDEV_CLK_USER_VOICE);
success:
    osiMutexUnlock(d->lock);
    return true;

failed:
    osiMutexUnlock(d->lock);
    OSI_LOGE(0, "audio bt voice stop failed");
    return false;
}

/**
 * Set btvoice work mode interface
 */
bool audevBtVoiceSetWorkMode(auBtVoiceWorkMode_t workmode)
{
    OSI_LOGI(0, "audio set bt voice work mode %d", workmode);

    if (workmode > AU_BT_VOICE_WORK_MODE_NO)
        return false;

    audevContext_t *d = &gAudevCtx;
    osiMutexLock(d->lock);
    d->btworkmode = (SND_BT_WORK_MODE_T)workmode;
    if (d->clk_users & AUDEV_CLK_USER_VOICE)
        prvSetVoiceConfig();

    osiMutexUnlock(d->lock);
    return true;
}

/**
 * Get btvoice work mode interface
 */
auBtVoiceWorkMode_t audevBtVoiceGetWorkMode(void)
{
    audevContext_t *d = &gAudevCtx;
    return (auBtVoiceWorkMode_t)d->btworkmode;
}
