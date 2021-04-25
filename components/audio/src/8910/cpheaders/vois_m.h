#ifndef _VOIS_M_H_
#define _VOIS_M_H_

/// @page vois_mainpage  VOIce Service API
/// @mainpage VOIce Service API
///
/// This service provides the sound management for the phone calls. It takes care of the Rx
/// decoding, the Tx encoding, the configuration of the audio interfaces in a fully
/// integrated way. The only things to do is to start this service when needed
/// (#vois_Start), to stop it when it becomes unneeded (#vois_Stop), and configure on
/// the run when needed (#vois_Setup).
///
/// @par Configuration requirements
/// HAL must be poperly configured in order to use the aps service.
///
/// The API is detailed in the following group: @ref vois
///
/// @par Organisation
/// @image latex vois.png
/// @image html vois.png
///
/// @defgroup vois VOIce Service (VOIS)
/// @{
///

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================
#define AUD_VOCODE_MODE_NB_GSM 0x1
#define AUD_VOCODE_MODE_NB_VOLTE 0x2
#define AUD_VOCODE_MODE_WB_OA 0x3
#define AUD_VOCODE_MODE_WB_BE 0x4

// =============================================================================
// VOIS_AUDIO_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure.
///
/// A configuration structure allows to record a stream with the proper configuration
/// set as far as the audio interface and the decoding are concerned.
// =============================================================================
typedef struct
{
    /// Speaker level.
    AUD_SPK_LEVEL_T spkLevel;

    /// Mic level
    AUD_MIC_LEVEL_T micLevel;

    /// Side tone
    AUD_SIDE_LEVEL_T sideLevel;

    /// Tone level
    AUD_TONE_ATTENUATION_T toneLevel;

    /// encoder gain
    INT16 encMute;

    /// decoder gain
    INT16 decMute;

} VOIS_AUDIO_CFG_T;

// =============================================================================
// VOIS_STATUS_T
// -----------------------------------------------------------------------------
/// Status returned by the VOIS to the calling entity. The possible value
/// describes various information about the status of the play.
// =============================================================================
typedef enum
{
    VOIS_STATUS_MID_BUFFER_REACHED,
    VOIS_STATUS_END_BUFFER_REACHED,
    VOIS_STATUS_NO_MORE_DATA,
    VOIS_STATUS_ERR,

    VOIS_STATUS_QTY
} VOIS_STATUS_T;

typedef enum
{
    VOIS_LOOP_NONE,
    VOIS_LOOP_NORMAL,
    VOIS_LOOP_DEALY,
    VOIS_LOOP_SAMPLE,
    VOIS_LOOP_QTY
} VOIS_LOOP_T;

typedef enum
{
    MIXVOISE_STATE_IDLE,
    MIXVOISE_STATE_MIXLOCAL,
    MIXVOISE_STATE_MIXREMOTE,
    MIXVOISE_STATE_MIXALL,
} MIXVOISE_STATE;

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// vois_Open
// -----------------------------------------------------------------------------
/// Open the VOIS service. Must be called before any other VOIS function.
/// This function registers the VoiS context in HAL map engine.
// =============================================================================
PUBLIC VOID vois_Open(VOID);

// =============================================================================
// vois_Setup
// -----------------------------------------------------------------------------
/// Configure the VOIS service..
///
/// This functions configures the playing of the Rx stream and te recording of
/// the Tx stream on a given audio interface:
/// gain for the side tone, the microphone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker...
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #VOIS_AUDIO_CFG_T for more details.
/// @return #VOIS_ERR_NO, the configuration being applied.
// =============================================================================
PUBLIC VOIS_ERR_T vois_Setup(AUD_ITF_T itf, CONST VOIS_AUDIO_CFG_T *cfg);

// =============================================================================
// vois_Start
// -----------------------------------------------------------------------------
/// Start the VOIS service.
///
/// This function records from the mic and outputs sound on the speaker
/// on the audio interface specified as a parameter, using the input and output
/// set by the \c parameter. \n
///
/// @param itf Interface providing the audion input and output.
/// @param cfg The configuration set applied to the audio interface
/// @return #VOIS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #VOIS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC VOIS_ERR_T vois_Start(
    CONST AUD_ITF_T itf,
    CONST VOIS_AUDIO_CFG_T *cfg);

// =============================================================================
// vois_Stop
// -----------------------------------------------------------------------------
/// This function stops the VOIS service.
/// If the function returns
/// #VOIS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #VOIS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #VOIS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC VOIS_ERR_T vois_Stop(VOID);

// =============================================================================
// VOIS_USER_HANDLER_T
// -----------------------------------------------------------------------------
/// To give back status to the calling task, the VOIS needs to be given a
/// callback function. This function is called whenever an event occurs
/// which needs to be reported back to the VOIS used. This status is reported
/// as the parameter of the function. The function is passed as a parameter to
/// the function #ars_Record. This type is the type describing such a function.
///
/// Note: This function must be as minimalist as possible, as it will be
/// executed during an interrupt. It should for example only send one event
/// to a task, asking for data refilling of the buffer. The data copy is
/// then done by the task, when it is rescheduled, but not directly by the
/// user handler.
// =============================================================================
typedef VOID (*VOIS_USER_HANDLER_T)(VOIS_STATUS_T);

PUBLIC VOIS_ERR_T vois_RecordStart(INT32 *startAddress, INT32 length, VOIS_USER_HANDLER_T handler);
PUBLIC VOIS_ERR_T vois_RecordStop(VOID);
PUBLIC VOIS_ERR_T vois_RecordPush(INT16 *MicPcmBuffer, INT16 len);

#ifdef VOIS_DUMP_PCM
PUBLIC VOID vois_DumpPcmDataHandler(VOIS_STATUS_T status);
PUBLIC VOIS_ERR_T vois_DumpSpkPCMPush(INT16 *pcmBuffer, INT16 len);
PUBLIC VOIS_ERR_T vois_DumpResPCMPush(INT16 *pcmBuffer, INT16 len);
PUBLIC VOIS_ERR_T vois_DumpMicPCMPush(INT16 *pcmBuffer, INT16 len);
PUBLIC VOID vois_DumpPcmDataStart(VOIS_USER_HANDLER_T handler);
PUBLIC VOID vois_DumpPcmDataStop(VOID);

#endif
PUBLIC void vois_MixDataStop(void);

PUBLIC void vois_TaskStart(void);

PUBLIC AUD_ERR_T vois_TestModeSetup(CONST SND_ITF_T itf,
                                    CONST HAL_AIF_STREAM_T *stream,
                                    CONST AUD_LEVEL_T *cfg,
                                    AUD_TEST_T mode,
                                    UINT32 voismode);

PUBLIC VOIS_ERR_T vois_SetCfg(AUD_ITF_T itf, CONST VOIS_AUDIO_CFG_T *cfg,
                              AUD_DEVICE_CFG_EXT_T devicecfg, SND_BT_WORK_MODE_T btworkmode);

// =============================================================================
//vois_SetBtNRECFlag
// -----------------------------------------------------------------------------
/// vois_SetBtNRECFlag
// =============================================================================
PUBLIC VOID vois_SetBtNRECFlag(UINT32 flag);

///  @} <- End of the aps group

#if (AUDIO_CALIB_VER == 2)
// =============================================================================
// vois_GetLatestCfg
// -----------------------------------------------------------------------------
/// get the vois Latest config.
// =============================================================================
PUBLIC VOIS_AUDIO_CFG_T *vois_GetAudioCfg(VOID);

PUBLIC HAL_AIF_FREQ_T vois_GetSampleRate(VOID);

#endif

#endif // _VOIS_M_H_
