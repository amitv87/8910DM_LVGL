

#ifndef _HAL_ZSP_H_
#define _HAL_ZSP_H_

// Music input buffer size.
#define AUDIO_INPUT_BUF_SIZE (7 * 1024) // 7K Bytes
// Music output buffer size.
#define AUDIO_OUTPUT_BUF_SIZE (9 * 1024) // 9K Bytes
// Number of music post processing IIR bands.
#define EQ_MAX_BANDS 10
// Number of audio call IIR bands.
#define CALIB_AUDIO_CALL_IIR_BANDS (7)
// MSBC buf between wcn and zsp
#define MSBC_BUF_SIZE 30 // 30 Word
// MSBC ring buf in zsp
#define MSBC_TO_PCM_BUF_SIZE (120) // 120 Word
#define MSBC_TO_PCM_QUEUE_LEN (16)
#define MSBC_PCM_BUF_SIZE (MSBC_TO_PCM_QUEUE_LEN * MSBC_TO_PCM_BUF_SIZE)
#define MSBC_PCM_BUF_MASK (MSBC_PCM_BUF_SIZE - 1)

// BT Work Mode
#define BT_WORK_MODE_NB (0)
#define BT_WORK_MODE_WB (1)

typedef enum
{
    NORMAL_VOICE_WB_REQ = 0x1111, //0x1111
    NORMAL_VOICE_NB_REQ,          //0x1112

    BT_VOICE_WB_REQ, //0x1113
    BT_VOICE_NB_REQ, //0x1114

    PLAY_MP3_INIT_REQ, //0x1115
    PLAY_AAC_INIT_REQ, //0x1116
    PLAY_WMA_INIT_REQ, //0x1117
    PLAY_AMR_INIT_REQ, //0x1118

    MUSIC_VIBRATE_START_REQ, //0x1119
    MUSIC_VIBRATE_STOP_REQ,  //0x111a
    PLAY_WMA_PRO_REQ,        //0x111b
    PLAY_AMR_PRO_REQ,        //0x111c

    MUSIC_POST_INIT_REQ, //0x111d
    MUSIC_POST_PRO_REQ,  //0x111e

    RECORD_AAC_INIT_REQ, //0x111f
    RECORD_AMR_INIT_REQ, //0x1120
    RECORD_WAV_INIT_REQ, //0x1121

    RECORD_AAC_PRO_REQ, //0x1122
    RECORD_AMR_PRO_REQ, //0x1123
    RECORD_WAV_PRO_REQ, //0x1124

    PLAY_FM_INIT_REQ, //0x1125

    VOICE_VIBRATE_START_REQ, //0x1126
    VOICE_VIBRATE_STOP_REQ,  //0x1127

    VOICE_RECORD_START_REQ, //0x1128
    VOICE_RECORD_STOP_REQ,  //0x1129
    MUSIC_SBC_START_REQ,    //0x112a
    MUSIC_SBC_STOP_REQ,     //0x112b

    BT_MUSIC_CON_REQ, //0x112c

    LOOPBACK_REQ,  //0x112d
    CLOSE_ALL_REQ, //0x112e
    END_SHARECMD_REQ

} HAL_ZSP_WAKEUP_ID_T;

typedef enum
{
    NORMAL_VOICE_WB_IND = 0x2111,
    NORMAL_VOICE_NB_IND, // 0x2112

    BT_VOICE_WB_IND, // 0x2113
    BT_VOICE_NB_IND, // 0x2114

    PLAY_MP3_INIT_IND, // 0x2115
    PLAY_AAC_INIT_IND, // 0x2116
    PLAY_WMA_INIT_IND, // 0x2117
    PLAY_AMR_INIT_IND, // 0x2118

    PLAY_MP3_PRO_IND, // 0x2119
    PLAY_AAC_PRO_IND, // 0x211a
    PLAY_WMA_PRO_IND, // 0x211b
    PLAY_AMR_PRO_IND, // 0x211c

    MUSIC_POST_INIT_IND, // 0x211d
    MUSIC_POST_PRO_IND,  // 0x211e

    RECORD_AAC_INIT_IND, // 0x211f
    RECORD_AMR_INIT_IND, // 0x2120
    RECORD_WAV_INIT_IND, // 0x2121

    RECORD_AAC_PRO_IND, // 0x2122
    RECORD_AMR_PRO_IND, // 0x2123
    RECORD_WAV_PRO_IND, // 0x2124

    PLAY_FM_INIT_IND, // 0x2125

    CHANGE_VOICE_IND, // 0x2126
    DTMF_DECODE_IND,  // 0x2127

    VOICE_RECORD_START_IND, // 0x2128
    VOICE_RECORD_STOP_IND,  // 0x2129
    MUSIC_SBC_START_IND,    // 0x212a
    MUSIC_SBC_STOP_IND,     // 0x212b

    BT_MUSIC_CON_IND, // 0x212c

    LOOPBACK_IND,  // 0x212d
    CLOSE_ALL_IND, // 0x212e
    END_SHARECMD_IND
} HAL_ZSP2MCU_MB_ID_T;

typedef enum
{
    audIdle = 0xaabc,
    audWork = 0x1abc,
    audBusy = 0x2abc,
} AUD_STATUS_E;

typedef void (*HAL_ZSP_IRQ_HANDLER_T)(void);

// ===================================================================
// AUD_VSI_DigGain_T
// -------------------------------------------------------------------
/// VSI dig gain.
///
// ===================================================================
#if 0
typedef struct
{
    INT16 in_enable;
    INT16 out_enable;
    INT16 scal_in;
    INT16 scal_out;
} AUD_VSI_DigGain_T;

// ===================================================================
// AUD_VSI_AEC_CTRL_T
// -------------------------------------------------------------------
/// VSI AEC CTRL.
///
// ===================================================================
typedef struct
{
    INT16 EchoTaillen;
    INT16 AECEnable;
    INT16 PPEnable;
    INT16 HPFEnable;
    INT16 CNGEnable;
    INT16 CNGInitialLvl;
    INT16 pTime;
    INT16 BulkDelay;
    INT16 IsNSRequired;
    INT16 SamplingRate;
} AUD_VSI_AEC_CTRL_T;

// ===================================================================
// AUD_VSI_AEC_RTCTRL_T
// -------------------------------------------------------------------
/// VSI AEC RTCTRL.
///
// ===================================================================
typedef struct
{
    INT16 AdaptationControl;
    INT16 DTControl;
    INT16 PPLevel;
    INT16 CNGAdjustLvl;
    INT16 InitialConvergence;
    INT16 SteadyStateConvergence;
    INT16 FrequencyAttenuationTbl[2][16];
} AUD_VSI_AEC_RTCTRL_T;

// ===================================================================
// AUD_VSI_AEC_T
// -------------------------------------------------------------------
/// VSI AEC.
///
// ===================================================================
typedef struct
{
	INT16 a[3];
	INT16 b[3];
	AUD_VSI_AEC_CTRL_T ECCtrl;
	AUD_VSI_AEC_RTCTRL_T ECRTCtrl;
} AUD_VSI_AEC_T;

// ===================================================================
// AUD_VSI_NS_RTCTRL_T
// -------------------------------------------------------------------
/// VSI NS RTCTRL.
///
// ===================================================================
typedef struct
{
	INT16 NSEnable;
	INT16 NSLevel;
	INT16 SNRLimit;
	INT16 InitialConvergenceTime;
	INT16 SteadyStateConvergenceTime;
} AUD_VSI_NS_RTCTRL_T;

// ===================================================================
// CALIB_AUDIO_VSI_AGC_CTRL_T
// -------------------------------------------------------------------
///VSI AGC CTRL.
// ===================================================================
typedef struct
{
	INT16 TartgetLvl;
	INT16 SamplingRate;
	INT16 Mode;

} AUD_VSI_AGC_CTRL_T;

// ===================================================================
// AUD_VSI_AGC_RTCTRL_T
// -------------------------------------------------------------------
/// VSI AGC RTCTRL.
///
// ===================================================================
typedef struct
{
	INT16 AttackTime;
	INT16 ReleaseTime;
	INT16 NoiseGateLvl;
} AUD_VSI_AGC_RTCTRL_T;

// ===================================================================
// AUD_VSI_AGC_RTCTRL_T
// -------------------------------------------------------------------
/// VSI AGC.
///
// ===================================================================
typedef struct
{
	INT16 AGCEnable;
	INT16 reserved;
	AUD_VSI_AGC_CTRL_T AGCCtrl;
	AUD_VSI_AGC_RTCTRL_T AGCRTCtrl;
} AUD_VSI_AGC_T;

// ===================================================================
// AUD_CALL_IIR_EQ_BANDS_T
// -------------------------------------------------------------------
/// .
///
// ===================================================================
typedef struct
{
	INT16 num[3];
	INT16 den[3];

} AUD_EQ_FilterCoefs_T;

// ===================================================================
// AUD_VSI_EQ_T
// -------------------------------------------------------------------
/// VSI EQ.
///
// ===================================================================
typedef struct
{
	INT16 flag;
	INT16 reserved;
	AUD_EQ_FilterCoefs_T band[CALIB_AUDIO_CALL_IIR_BANDS];
} AUD_VSI_EQ_T;

// ===================================================================
// AUD_VSI_AVC_T
// -------------------------------------------------------------------
///VSI AVC.
// ===================================================================
 typedef struct
{
	INT16 AVCEnable;
	INT16 SNRI;
} AUD_VSI_AVC_T;

// ===================================================================
// AUD_VSI_VQE_PARAM_T
// -------------------------------------------------------------------
/// VSI VQE.
///
// ===================================================================
typedef struct
{
	INT16 SampleRate;
	INT16 reserved[3];

	AUD_VSI_DigGain_T gain;
	AUD_VSI_AEC_T AECParams;
	AUD_VSI_NS_RTCTRL_T TxNSParams;
	AUD_VSI_NS_RTCTRL_T RxNSParams;
	AUD_VSI_AGC_T TxAGCParams;
	AUD_VSI_AGC_T RxAGCParams;
	AUD_VSI_EQ_T TxEQParams;
	AUD_VSI_EQ_T RxEQParams;
	AUD_VSI_AVC_T AVCParams;
} AUD_VSI_VQE_PARAM_T;

#endif
/// RDA VQE.
///
// ===================================================================
typedef struct
{
    //for AF
    INT16 enable_af;
    INT16 Mu;
    INT16 MIN_LEAK;
    INT16 Delta_ctrl;
    INT16 delay;

    //for VAD
    INT16 amth;
    INT16 sil2spe;
    INT16 spe2sil;

    //for RES
    INT16 enable_res;
    INT16 res_gain;

    //for DTD
    INT16 enable_dtd;
    INT16 up_coeff;
    INT16 down_coeff;
    INT16 confident_coeff;
    INT16 lowest_coeff;
    INT16 gamma;
    INT16 DTD_protect_gain;
    INT16 dtd_hangover_out_max;
    INT16 dtd_hangover_out_max2;
    INT16 ERLE_small_thr;
    INT32 FPH_sum_thr;

    //for NS
    INT16 enable_ns;
    INT16 Gmin;
    INT16 Gmin_echo;
    INT16 Gmin_noise;
    INT16 alpha_ph; //�������ڸ���ƽ����ϵ����Խ������Խ���ײ���
    INT16 alpha_S1; //֡��ƽ����ϵ����Խ����Сֵ���Ƶ�Խ��

    //for NLP
    INT16 enable_nlp;
    INT16 clip_process_flag;
    INT16 thd_process_flag;
    INT16 Gain_clip_value;
    INT16 clip_holdFrame;
    INT16 HL_thr;
    INT16 band1;

    // thd process
    INT16 GainThr;
    INT16 GainNbThr;
    INT16 thd_holdFrame;
    INT16 GainDetectRangeStart;
    INT16 GainDetectRangeEnd;
    INT16 HighBinsReduce;
    INT16 HighBinsStart;
    INT16 LowBinsReduce;
    INT16 LowBinsStart;
    INT16 HighBinsEnd;

    //for CNG
    INT16 enable_cng;
    INT16 cng_thr_amp;    //-36dB
    INT16 cng_thr_amp_up; //-30dB
} AUD_RDA_AEC_T;

typedef struct
{

    //for NS
    INT16 enable_ns;
    INT16 Gmin;
    INT16 Gmin_echo;
    INT16 Gmin_noise;
    INT16 alpha_ph; //�������ڸ���ƽ����ϵ����Խ������Խ���ײ���
    INT16 alpha_S1; //֡��ƽ����ϵ����Խ����Сֵ���Ƶ�Խ��
    //for CNG
    INT16 enable_cng;
    INT16 cng_thr_amp;    //-36dB
    INT16 cng_thr_amp_up; //-30dB
    INT16 reserved;

} AUD_RDA_RxNS_T; //opened parameters

typedef struct
{
    INT16 enable;
    INT16 targetLevelDbfs;   // default 3 (-3 dBOv)
    INT16 compressionGaindB; // default 9 dB
    INT16 limiterEnable;     // default kAgcTrue (on)
    INT16 upperThr;
    INT16 lowerThr;
    INT16 decayValue;
    INT16 capacitorSlowFactor;
    INT16 capacitorFastFactor;
    INT16 noiseCompressFactor;
    INT16 noiseGateThr;
    INT16 noiseStdShortTermFactor;
} AUD_RDA_AGC_T; ////

#define EQ_VOICE_MAX_BANDS 7

typedef struct
{
    INT16 num[3];
    INT16 den[3];

} AUD_EQ_FilterCoefs_T;

/*typedef enum
{
	EQ_SKIP = 0,
	EQ_LPF,
	EQ_HPF,
	EQ_peakingEQ,
	EQ_lowShelf,
	EQ_highShelf,
} EQ_filter_type;*/

typedef struct
{
    INT16 freq;
    INT16 type;
    INT16 gain;
    INT16 qual;
} AUD_EQ_iir_param_T;

typedef struct
{
    INT16 eqEnable;
    INT16 bands;
    AUD_EQ_iir_param_T param[EQ_VOICE_MAX_BANDS];
    AUD_EQ_FilterCoefs_T coeffs[EQ_VOICE_MAX_BANDS];
} AUD_RDA_EQ_T;

typedef struct
{
    INT16 in_enable;
    INT16 out_enable;
    INT16 scal_in;
    INT16 scal_out;
} AUD_RDA_DigGain_T;

typedef struct
{
    INT16 SpeechMode;
    INT16 aecEnable;
    INT16 rxNsEnable;
    INT16 reserved;

    AUD_RDA_DigGain_T DigtalGain;
    AUD_RDA_AEC_T AECParams;
    AUD_RDA_RxNS_T RxNSParams;
    AUD_RDA_AGC_T TxAGCParams;
    AUD_RDA_AGC_T RxAGCParams;
    AUD_RDA_EQ_T TxEQParams;
    AUD_RDA_EQ_T RxEQParams;
} AUD_RDA_VQE_PARAM_T;

typedef struct
{
    AUD_RDA_VQE_PARAM_T rdaVqe;
} AUD_VQE_PARAM_T;

// ===================================================================
// AUD_MUSIC_DRC_PARAM_T
// -------------------------------------------------------------------
/// music drc.
///
// ===================================================================
typedef struct
{
    INT16 alc_enable;

    INT16 thres;
    INT16 width;
    INT16 R;
    INT16 R1;
    INT16 R2;
    INT16 limit;
    INT16 M_drc;
    INT16 alpha1;
    INT16 alpha2;
    INT16 noise_gate;
    INT16 alpha_max;
    INT16 Thr_dB;
    INT16 mm_gain;
    INT16 channel;
    INT16 reserved;
} AUD_MUSIC_DRC_T;

// ===================================================================
// AUD_MUSIC_IIR_EQ_BANDS_T
// -------------------------------------------------------------------
/// music iir eq.
///
// ===================================================================
/*typedef struct
{
	INT16 freq;
	INT16 qual;
	INT16 type;
	INT16 gain;
} AUD_MUSIC_IIR_EQ_BANDS_T;*/

// ===================================================================
// AUD_MUSIC_IIR_EQ_BANDS_T
// -------------------------------------------------------------------
/// music iir eq.
///
// ===================================================================
typedef struct
{
    INT16 eqEnable;
    INT16 bands;
    AUD_EQ_iir_param_T bandInfo[EQ_MAX_BANDS];
} AUD_MUSIC_EQ_PARAM_T;

typedef struct
{
    AUD_EQ_FilterCoefs_T coeffs[EQ_MAX_BANDS];
    AUD_MUSIC_EQ_PARAM_T param;
} AUD_MUSIC_EQ_T;

// ===================================================================
// AUD_ZSP_CFG_T
// -------------------------------------------------------------------
/// Structure defining
///
// ===================================================================

// ===================================================================
// AUDIO_INPUT_PARAM_T
// -------------------------------------------------------------------
/// Structure defining the Audio Encode/Decode input parameters
///
// ===================================================================
typedef struct
{
    UINT16 readOffset;
    UINT16 writeOffset;
    UINT16 fileEndFlag;
    UINT16 inLenth;   //Input stream buffer length, 7K byte
    UINT16 outLength; //Output stream buffer length, (1152*4*2)byte
    UINT16 postProFlag;
    UINT16 sbcOutFlag;
    UINT16 musicVibFlag;

    // only for audio encode
    UINT16 recFormat;
    UINT16 channelNb;
    UINT32 sampleRate;
    UINT32 bitsPerSample;
} AUDIO_INPUT_PARAM_T;

// ===================================================================
// AUDIO_OUTPUT_PARAM_T
// -------------------------------------------------------------------
/// Structure defining the audio Encode/Decode output parameters
///
// ===================================================================
typedef struct
{
    //common paras
    UINT32 samplerate;
    UINT16 channelNb;    /* number of channels */
    UINT16 length;       /* number of samples per channel */
    UINT32 consumedLen;  /* size of the consumed data since last frame */
    UINT32 streamStatus; /* stream status */

    //other paras
    //UINT16 channelMode;
    //UINT16 layer;
    /* reuse channelMode and layer param space for poc mode sharemem ctrl*/
    UINT16 readOffset;
    UINT16 writeOffset;
    UINT32 ipcrate;
} AUDIO_OUTPUT_PARAM_T;

// ===================================================================
// AUDIO_MSBC_BUF_T
// -------------------------------------------------------------------
/// Structure defining the msbc buffer between wcn and zsp
///
// ===================================================================
typedef struct
{
    UINT32 wcn2zsp_mutex;
    UINT32 wcn2zsp_seq;
    UINT16 wcn2zsp_data[MSBC_BUF_SIZE];

    UINT32 zsp2wcn_mutex;
    UINT32 zsp2wcn_seq;
    UINT16 zsp2wcn_data[MSBC_BUF_SIZE];

    UINT16 reverd[512];

    UINT16 ul_ridx;
    UINT16 ul_widx;
    UINT16 ul_pcm_queue[MSBC_TO_PCM_QUEUE_LEN * MSBC_TO_PCM_BUF_SIZE];

    UINT16 dl_ridx;
    UINT16 dl_widx;
    UINT16 dl_pcm_queue[MSBC_TO_PCM_QUEUE_LEN * MSBC_TO_PCM_BUF_SIZE];

    UINT16 ul_pcm_8k[160];
    UINT16 dl_pcm_16k[320];
} AUDIO_MSBC_BUF_T;

// ===================================================================
// AUDIO_NXP_BUF_T
// -------------------------------------------------------------------
/// Structure defining the nxp buffer between cp and zsp
///
// ===================================================================
typedef struct
{
    UINT16 tx_zsp2nxp_pcm[320];
    UINT16 tx_nxp2zsp_pcm[320];

    UINT16 rx_zsp2nxp_pcm[320];
    UINT16 rx_nxp2zsp_pcm[320];
} AUDIO_NXP_BUF_T;

// ===================================================================
// AUD_SHAREMEM_T
// -------------------------------------------------------------------
/// Structure defining the audio share memory (CPARM <-> ZSP900m)
///
// ===================================================================
//#define SPEECH_SUPPORT
//#define MUSIC_SUPPORT
typedef struct
{
    /* ZSP Ctrl Info */
    UINT16 audCurrStatus;
    UINT16 updateParaInd;
    UINT16 loopBackType;
    UINT16 voiceRecFormat;
    UINT16 volteFlag;
    UINT16 musicMode;
    UINT32 traceDataFlag;
    UINT32 BTWorkMode;

#ifdef SPEECH_SUPPORT
    union {
        /* Speech Parameters */
        HAL_SPEECH_ENC_OUT_T txAMRBuffer;
        HAL_VOLTE_ENC_OUT_T txAMRVolte;
    };

    union {
        /* Speech Parameters */
        HAL_SPEECH_DEC_IN_T rxAMRBuffer;
        HAL_VOLTE_DEC_IN_T rxAMRVolte;
    };
#endif

    union {
        HAL_SPEECH_PCM_BUF_T txPcmBuffer;
        HAL_VOLTE_PCM_BUF_T txPcmVolte;
    };

    union {
        HAL_SPEECH_PCM_BUF_T rxPcmBuffer;
        HAL_VOLTE_PCM_BUF_T rxPcmVolte;
    };

    /* VQE Parameters*/
    AUD_VQE_PARAM_T vePara;

#ifdef MUSIC_SUPPORT
    /* Music Post Process Parameters */
    AUD_MUSIC_DRC_T drcPara;
    AUD_MUSIC_EQ_T eqPara;
    /* Audio(mp3/aac/wma/bt/amr) */
    AUDIO_INPUT_PARAM_T audInPara;
    AUDIO_OUTPUT_PARAM_T audOutPara;

    union {
        INT16 audInput[AUDIO_INPUT_BUF_SIZE / 2]; //CPU send audio data to decoded/code
        AUDIO_NXP_BUF_T nxp_buf;
    };

    union {
        INT16 audOutput[AUDIO_OUTPUT_BUF_SIZE / 2]; //zsp return the decode/code to CPU
        AUDIO_MSBC_BUF_T msbc;
    };
#else //for speech record
    union {
        INT16 audOutput[320];
        AUDIO_MSBC_BUF_T msbc;
    };
#endif
} AUD_ZSP_SHAREMEM_T;

typedef struct
{
    UINT16 musicMode;
    UINT16 sbcOutFlag;
} AUD_AP2CP_PARA_T;

extern HAL_ERR_T hal_zspAudOpen(HAL_VOC_IRQ_HANDLER_T IrqHandler);
extern VOID hal_zspAudClose(VOID);
extern HAL_ERR_T hal_zspAudWakeup(HAL_ZSP_WAKEUP_ID_T wakeupId);
extern VOID hal_zspAudTraceData(UINT32 dataFlag);
extern HAL_ERR_T hal_zspMusicPlayStart(void);
extern HAL_ERR_T hal_zspMusicPlayStop(void);
extern HAL_ERR_T hal_zspMusicPlayStartEx(AUD_AP2CP_PARA_T *Aud_Para);
extern HAL_ERR_T hal_zspMusicPlayStopEx(AUD_AP2CP_PARA_T *Aud_Para);
extern HAL_ERR_T hal_zspVoiceRecordStart(void);
extern HAL_ERR_T hal_zspVoiceRecordStop(void);
extern HAL_ERR_T hal_zspVoiceVibrateStart(UINT32 time);
extern HAL_ERR_T hal_zspVoiceVibrateStop(void);
extern HAL_ERR_T hal_zspVoicePlayStart(void);
extern HAL_ERR_T hal_zspVoicePlayStop(void);
extern HAL_ERR_T hal_btVoiceStart(void);
extern HAL_ERR_T hal_btVoiceStop(void);
extern void hal_LpsAudioWkUpReq(void);

extern AUD_ZSP_SHAREMEM_T *g_aud_sharemem;
extern HAL_VOC_IRQ_HANDLER_T g_zspIrqHandler;

#endif
