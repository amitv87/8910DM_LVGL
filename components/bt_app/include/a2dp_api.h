/******************************************************************************
 ** File Name:    a2dp_api.h                                                  *
 ** Author:       yuhua.shi                                                   *
 ** DATE:         07/12/2006                                                  *
 ** Copyright:    2006 Spreatrum, Incoporated. All Rights Reserved.           *
 ** Description:  the api function for application                            *
 ******************************************************************************/
/*****************************************************************************
 **                   Edit    History                                         *
 **---------------------------------------------------------------------------* 
 ** DATE          NAME            DESCRIPTION                                 * 
 ** 07/12/2006    yuhua.shi       Create.                                     *
 ******************************************************************************/
#ifndef _A2DP_API_H_
#define _A2DP_API_H_

/*----------------------------------------------------------------------------*
 **                         DEPENDENCIES                                      *
 **-------------------------------------------------------------------------- */
#include "sci_types.h"
#include "audio_api.h"
#include "bt_abs.h"

/**---------------------------------------------------------------------------*
 **                         COMPILER FLAG                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**---------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                  *
 **---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                 *
 **---------------------------------------------------------------------------*/
/******************************************************************************
 * A2DP callback function
 ******************************************************************************/
typedef void (*A2DP_CALLBACK)(const BT_MSG_T *signal);

//define the state of A2DP device for application
typedef enum
{
    A2DP_DEVICE_STATE_INVALID, //Invalid state
    A2DP_DEVICE_STATE_ACTIVE,  //the device is active
    A2DP_DEVICE_STATE_OPEN,    //The device is open, we can send audio data
    A2DP_DEVICE_STATE_SUSPEND, //The device is suspend
    A2DP_DEVICE_STATE_CLOSE,   //The device is close
    A2DP_DEVICE_STATE_MAX
} A2DP_DEVICE_STATE_TYPE_E;

//Sample rate of PCM data after MP3 decoder
typedef enum
{
    A2DP_AUDIO_SAMPLERATE_16K, //sample rate:16000
    A2DP_AUDIO_SAMPLERATE_32K, //sample rate:32000
    A2DP_AUDIO_SAMPLERATE_44K, //sample rate:44100
    A2DP_AUDIO_SAMPLERATE_48K, //sample rate:48000
    A2DP_AUDIO_SAMPLERATE_MAX
} A2DP_AUDIO_SAMPLERATE_E;

//define for audio quality of A2DP
typedef enum
{
    A2DP_AUDIO_QUALITY_LOW,  //Low audio quality of A2DP
    A2DP_AUDIO_QUALITY_MID,  //Middle audio quality of A2DP
    A2DP_AUDIO_QUALITY_HIGH, //High audio quality of A2DP
    A2DP_AUDIO_QUALITY_MAX
} A2DP_AUDIO_QUALITY_E;

//define the struct of parameter for device initialization
typedef struct
{
    A2DP_AUDIO_SAMPLERATE_E sample_rate; //Sample rate of PCM data after MP3 decoder
    A2DP_AUDIO_QUALITY_E audio_quality;  //define for audio quality of A2DP
    BOOLEAN need_create_link;            //TRUE: Create the link when active the A2DP device
                                         //FALSE:Don't create the link when active the A2DP device
    BOOLEAN is_sbccodec_run_interram;    //TRUE: sbc codec run in internal ram
                                         //FALSE:sbc codec don't run in internal ram
    A2DP_CALLBACK callback;              //callback function of APP
    BOOLEAN support_trans_samplerate;
} A2DP_DEVICE_INIT_PARAM_T;

//define the struct of parameters for A2DP SBC config @cherry.liu
typedef struct
{

    uint8 channel_mode;
    uint8 alloc_method;
    uint16 sample_freq;
    uint8 nrof_blocks;
    uint8 nrof_subbands;
    uint8 bitpool;
    uint16 framesize;

} A2DP_SBC_PARAM_T;

/**---------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                               *
 **---------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description:    Init the A2DP Device Parameter.
//  Author:         yuhua.shi
//  Note:
//****************************************************************************/
PUBLIC BT_STATUS A2DP_InitDevice(A2DP_DEVICE_INIT_PARAM_T *param);

/*****************************************************************************/
//  Description:    Register A2DP device to audio service.
//  Author:         yuhua.shi
//  Note:
//****************************************************************************/
PUBLIC HAUDIODEV A2DP_RegAudioDevice(void);

/*****************************************************************************/
//  Description:    Connect the A2DP Device selected.
//  Author:         yuhua.shi
//  Note:
//****************************************************************************/
PUBLIC BT_STATUS A2DP_ConnectDevice(BT_ADDRESS addr);

/*****************************************************************************/
//  Description:    Disconnect the A2DP Device selected.
//  Author:         yuhua.shi
//  Note:
//****************************************************************************/
PUBLIC BT_STATUS A2DP_DisConnectDevice(void);

/*****************************************************************************/
//  Description:    Open A2DP Device.
//  Author:         yuhua.shi
//  Note:
//****************************************************************************/
PUBLIC BT_STATUS A2DP_OpenDevice(uint32 uiSampleRate);

/*****************************************************************************/
//  Description:    Close A2DP Device
//  Author:         yuhua.shi
//  Note:
//****************************************************************************/
PUBLIC BT_STATUS A2DP_CloseDevice(void);

/*****************************************************************************/
//  Description:    Pause A2DP Device.
//  Author:         yuhua.shi
//  Note:
//****************************************************************************/
PUBLIC BT_STATUS A2DP_PauseDevice(void);

/*****************************************************************************/
//  Description:    Resume A2DP Device.
//  Author:         yuhua.shi
//  Note:
//****************************************************************************/
PUBLIC BT_STATUS A2DP_ResumeDevice(void);

/*****************************************************************************/
//  Description:    Start the data sending after the a2dp stream is stated
//  Author:         yuhua.shi
//  Note:
//  Parameter:      None
//  Returns:        BT_SUCCESS: start data successful
//					BT_ERROR:   start data fail
//****************************************************************************/
PUBLIC BT_STATUS A2DP_StartStreamData(void);

/*****************************************************************************/
//  Description:    Send Abort request
//  Author:         ming.zhang
//  Note:
//  Parameter:      None
//****************************************************************************/
PUBLIC BT_STATUS A2DP_Abort(void);

/*****************************************************************************/
//  Description:    Get sample rate
//  Author:         ming.zhang
//  Note:
//  Parameter:      None
//****************************************************************************/
PUBLIC uint32 A2DP_GetSampleRate(void);

/*****************************************************************************/
//  Description:    Get SBC config parameters(temp interface)
//  Author:         Cherry.Liu
//  Note:
//  Parameter:
//****************************************************************************/
PUBLIC BOOLEAN A2DP_GetSbcConfig(A2DP_SBC_PARAM_T *config);

typedef void (*a2dp_data_callback_t)(int16 *left_data, int16 *right_data, uint32 data_len);
typedef uint32 (*a2dp_count_callback_t)();

PUBLIC BOOLEAN A2DP_audioStart(uint32 sampleRate, a2dp_data_callback_t getData, a2dp_count_callback_t getCount);
PUBLIC BOOLEAN A2DP_audioClose(void);
PUBLIC BOOLEAN A2DP_audioResume(uint32 sampleRate, a2dp_data_callback_t getData, a2dp_count_callback_t getCount);
PUBLIC BOOLEAN A2DP_audioPause(void);
PUBLIC uint32 SBC_GetRemainedPcmCount(void);

extern BOOLEAN A2DP_audioStart(uint32 sampleRate, a2dp_data_callback_t getData, a2dp_count_callback_t getCount);
/**---------------------------------------------------------------------------*
**                          COMPILER FLAG                                     *
**----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

/**---------------------------------------------------------------------------*/
#endif //_A2DP_API_H_
//end a2dp_api.h
