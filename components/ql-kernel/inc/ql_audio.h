#ifndef _QL_AUDIO_H_
#define _QL_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ql_api_osi.h"
#include "ql_api_common.h"
#include "ql_codec_config.h"


#define QL_PCM_BLOCK_FLAG		(0x01)
#define QL_PCM_NONBLOCK_FLAG	(0x02)
#define QL_PCM_READ_FLAG    	(0x04)
#define QL_PCM_WRITE_FLAG		(0x08)

#define QL_AUDIO_IIC_CHANNEL     i2c_2
#define PACKET_WRITE_MAX_SIZE 	 3*1024

typedef enum
{
    AUDIOHAL_SPK_MUTE         = 0,
    AUDIOHAL_SPK_VOL_1,	     
    AUDIOHAL_SPK_VOL_2,
    AUDIOHAL_SPK_VOL_3,
    AUDIOHAL_SPK_VOL_4,
    AUDIOHAL_SPK_VOL_5,
    AUDIOHAL_SPK_VOL_6,
    AUDIOHAL_SPK_VOL_7,
    AUDIOHAL_SPK_VOL_8,
    AUDIOHAL_SPK_VOL_9,
    AUDIOHAL_SPK_VOL_10,
    AUDIOHAL_SPK_VOL_11,	// 11

	AUDIOHAL_SPK_VOL_QTY	// 12
} AUDIOHAL_SPK_LEVEL_T;

typedef enum
{
	QL_AUDIO_SUCCESS 			    = 0,

	QL_AUDIO_UNKNOWN_ERROR		    = (QL_COMPONENT_AUDIO << 16) | 901,
	QL_AUDIO_INVALID_PARAM		    = (QL_COMPONENT_AUDIO << 16) | 902,
	QL_AUDIO_OPER_NOT_SUPPORTED     = (QL_COMPONENT_AUDIO << 16) | 903,
	QL_AUDIO_DEVICE_BUSY		    = (QL_COMPONENT_AUDIO << 16) | 904,
	QL_AUDIO_FILE_NOT_EXIST 	    = (QL_COMPONENT_AUDIO << 16) | 905,
	QL_AUDIO_FREE_SIZE_NOT_ENOUGH   = (QL_COMPONENT_AUDIO << 16) | 906,
	QL_AUDIO_NO_MEMORY  		    = (QL_COMPONENT_AUDIO << 16) | 907, 			
	QL_AUDIO_SET_PATH_FAIL  	    = (QL_COMPONENT_AUDIO << 16) | 908,
	QL_AUDIO_FILE_IN_OPERATION      = (QL_COMPONENT_AUDIO << 16) | 909,
	QL_AUDIO_ERROR_AUDIO_PATH       = (QL_COMPONENT_AUDIO << 16) | 910,
	QL_AUDIO_FILE_TYPE_NOT_SUPPORT  = (QL_COMPONENT_AUDIO << 16) | 911,
	QL_AUDIO_DEVICE_NOT_EXIST 	    = (QL_COMPONENT_AUDIO << 16) | 912,
	QL_AUDIO_PLAY_FAIL              = (QL_COMPONENT_AUDIO << 16) | 913,
	QL_AUDIO_PARAM_SET_FAIL         = (QL_COMPONENT_AUDIO << 16) | 914,
	QL_AUDIO_OPEN_FILE_FAIL         = (QL_COMPONENT_AUDIO << 16) | 915,
	QL_AUDIO_RECORD_SYS_FAIL 		= (QL_COMPONENT_AUDIO << 16) | 916,
	QL_AUDIO_PLAYER_SYS_FAIL 		= (QL_COMPONENT_AUDIO << 16) | 917,
	QL_AUDIO_END_FAIL 				= (QL_COMPONENT_AUDIO << 16) | 918,
	QL_AUDIO_NOT_INIT				= (QL_COMPONENT_AUDIO << 16) | 919,

//codec
	QL_AUDIO_CODEC_WR_FAIL          = (QL_COMPONENT_AUDIO << 16) | 2000,
	QL_AUDIO_CODEC_RD_FAIL			= (QL_COMPONENT_AUDIO << 16) | 2001,
	QL_AUDIO_CODEC_INIT_FAIL		= (QL_COMPONENT_AUDIO << 16) | 2002,
}ql_audio_errcode_e;

typedef enum
{
	AUD_RECORD_ERROR = -1,
	AUD_RECORD_START = 0,   //audio recorder start record
	AUD_RECORD_DATA,		//record data comming
	AUD_RECORD_PAUSE,	    //record has been paused by call or pause api
	AUD_RECORD_CLOSE,		//record closed
	AUD_RECORD_RESUME,		//resume from pause
}enum_aud_record_state;

typedef enum
{
	AUD_PLAYER_ERROR = -1,
	AUD_PLAYER_START = 0,	//audio player start play
	AUD_PLAYER_PAUSE,		//audio player has been paused by call or pause api
	AUD_PLAYER_FINISHED,    //audio player finish current playing
	AUD_PLAYER_CLOSE,       //audio player closed
	AUD_PLAYER_RESUME,      //resume player from pause
}enum_aud_player_state;

typedef enum
{
    QL_AUDIO_PLAY_TYPE_NONE = 0,
    QL_AUDIO_PLAY_TYPE_LOCAL,
    QL_AUDIO_PLAY_TYPE_VOICE,
    QL_AUDIO_PLAY_TYPE_POC,
    QL_AUDIO_PLAY_TYPE_MAX
} AudPlayType_e;

typedef enum
{
    QL_OUTPUT_RECEIVER = 0,  ///< receiver
    QL_OUTPUT_HEADPHONE = 1, ///< headphone
    QL_OUTPUT_SPEAKER = 2,   ///< speaker

	QL_OUTPUT_MAX,
} AudOutputType_e;

typedef enum
{
    QL_INPUT_MAINMIC = 0, ///< main mic
    QL_INPUT_AUXMIC  = 1,  ///< auxilary mic
    QL_INPUT_DUALMIC = 2, ///< dual mic
    QL_INPUT_HPMIC_L = 3, ///< headphone mic left
    QL_INPUT_HPMIC_R = 4, ///< headphone mic right
} AudInputType_e;

typedef enum
{
    QL_AUDIO_FORMAT_UNKNOWN, ///< placeholder for unknown format
    QL_AUDIO_FORMAT_PCM,     ///< raw PCM data
    QL_AUDIO_FORMAT_WAVPCM,  ///< WAV, PCM inside
    QL_AUDIO_FORMAT_MP3,     ///< MP3
    QL_AUDIO_FORMAT_AMRNB,   ///< AMR-NB
    QL_AUDIO_FORMAT_AMRWB,   ///< AMR_WB

	QL_AUDIO_FORMAT_MAX
} AudStreamFormat_e;

typedef enum
{
    QL_AUDIO_STATUS_IDLE,     ///< playback not started
    QL_AUDIO_STATUS_RUNNING,  ///< playback is started
    QL_AUDIO_STATUS_PAUSE,    ///< playback is paused
    QL_AUDIO_STATUS_FINISHED, ///< playback is finished
} AudioStatus_e;

typedef enum
{
    QL_REC_TYPE_NONE = 0,
    QL_REC_TYPE_MIC,
    QL_REC_TYPE_VOICE,
} AudRecType_e;

typedef enum
{
	QL_PCM_NONE = 0,
	QL_PCM_LOCAL,
	QL_PCM_VOICE_CALL,
}pcm_path_e;

//ex codec
typedef enum
{
    HAL_AIF_RX_DELAY_ALIGN,
    HAL_AIF_RX_DELAY_1,
    HAL_AIF_RX_DELAY_2,
    HAL_AIF_RX_DELAY_3,
    HAL_AIF_RX_DELAY_QTY
} AUD_AIF_RX_DELAY_E;

typedef enum
{
    HAL_AIF_RX_MODE_STEREO_STEREO,
    HAL_AIF_RX_MODE_STEREO_MONO_FROM_L,
    HAL_AIF_RX_MODE_QTY
} AUD_AIF_RX_MODE_E;

typedef enum
{
    HAL_SERIAL_MODE_I2S,
    HAL_SERIAL_MODE_VOICE,
    HAL_SERIAL_MODE_DAI,
    HAL_SERIAL_MODE_QTY
} AUD_SERIAL_MODE_E;

typedef enum
{
    HAL_AIF_TX_DELAY_ALIGN,
    HAL_AIF_TX_DELAY_1,
    HAL_AIF_TX_DELAY_QTY
} AUD_AIF_TX_DELAY_E;

typedef enum
{
    HAL_AIF_TX_MODE_STEREO_STEREO,
    HAL_AIF_TX_MODE_MONO_STEREO_CHAN_L,
    HAL_AIF_TX_MODE_MONO_STEREO_DUPLI,
    HAL_AIF_TX_MODE_QTY
} AUD_AIF_TX_MODE_E;

typedef enum
{
    QL_CODEC_CLK_FREQ_12M = 12,
    QL_CODEC_CLK_FREQ_13M = 13,
    QL_CODEC_CLK_FREQ_24M = 24,
    QL_CODEC_CLK_FREQ_26M = 26,
    QL_CODEC_CLK_FREQ_MAX
} QL_CODEC_CLK_FREQ_E;

/**********************************************************************************************
								       Typedef
**********************************************************************************************/
typedef struct
{
	int channels;
	int samplerate;
	int len_size;
}ql_aud_config;

typedef void    *PCM_HANDLE_T ;

typedef struct {
	unsigned int channels;
	unsigned int samplerate;
	unsigned int periodcnt; // 20ms buffersize/per period, value:2~8
} QL_PCM_CONFIG_T;

//ext codec
typedef struct
{
    int32_t id;
    uint32_t freq;
    int32_t clockMode;
} AUD_I2C_CFG_T;

typedef struct
{
    uint32_t regAddr;
    uint16_t val;
    uint16_t delay;
} AUD_CODEC_REG_T;

typedef struct
{
    AUD_SERIAL_MODE_E mode;
    bool aifIsMaster;
    bool lsbFirst;
    bool polarity;
    AUD_AIF_RX_DELAY_E rxDelay;
    AUD_AIF_TX_DELAY_E txDelay;
    AUD_AIF_RX_MODE_E rxMode;
    AUD_AIF_TX_MODE_E txMode;
    uint32_t fs;
    uint32_t bckLrckRatio;
    bool invertBck;
    bool outputHalfCycleDelay;
    bool inputHalfCycleDelay;
    bool enableBckOutGating;
}AUD_AIF_SERIAL_CFG_T;

typedef struct
{
    uint32_t codecAddr;
    uint16_t codecIsMaster;
    uint16_t dataFormat;
    uint16_t reserved;
	uint16_t iic_data_width;
    uint16_t initFlag;
    uint16_t externalCodec;
}AUD_BASIC_CFG_T;

typedef struct
{
	AUD_BASIC_CFG_T	basicCfg;
    uint16_t reserved[24];
    AUD_I2C_CFG_T i2cCfg;
    AUD_AIF_SERIAL_CFG_T i2sAifcfg;
    AUD_CODEC_REG_T initRegCfg[100];
    AUD_CODEC_REG_T closeRegCfg[50];
    AUD_CODEC_REG_T sampleRegCfg[10][12];
    AUD_CODEC_REG_T inpathRegCfg[6][20];
    AUD_CODEC_REG_T outpathRegCfg[4][20];
} QL_HAL_CODEC_CFG_T;

typedef int(*cb_on_player)(char *p_data, int len, enum_aud_player_state state);
typedef int(*cb_on_record)(char *p_data, int len, enum_aud_record_state state);


/**********************************************************************************************
					API (support local and voice call play, record)
**********************************************************************************************/

/*****************************************************************
* Function: ql_get_audio_state
*
* Description: get audio initial state
* 
* Parameters:
*
* Return:
* 	1: initial ok
*	0: have not initialed ok, or initial failed
*****************************************************************/
int ql_get_audio_state(void);

/*****************************************************************
* Function: ql_get_file_format
*
* Description: get format of file
* 
* Parameters:
* 	file_name	  		[in] file name
*
* Return:
* 	AUSTREAM_FORMAT_UNKNOWN : unknown format 
*	others : file format 
*****************************************************************/
AudStreamFormat_e ql_get_file_format(char *file_name);

/*****************************************************************
* Function: ql_aud_set_volume
*
* Description: set the play volume
* 
* Parameters:
* 	type	  		[in] play type: local/voice call
*	volume          [in] play volume, range is 0~100
*
* Return:
* 	0:         		successful
* 	others          ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_set_volume(AudPlayType_e type, AUDIOHAL_SPK_LEVEL_T volume);

/*****************************************************************
* Function: ql_aud_get_volume
*
* Description: get the play volume
* 
* Parameters:
* 	type	  		[in] play type: local/voice call
*
* Return:
* 	volume:         play volume（0~100）
*   others:			ql_audio_errcode_e
*****************************************************************/
AUDIOHAL_SPK_LEVEL_T ql_aud_get_volume(AudPlayType_e type);

/*****************************************************************
* Function: ql_aud_play_file_start
*
* Description: play file
* 
* Parameters:
* 	type	  		[in] play type: local/voice call
*	file_name       [in] file name, full path required
*	play_cb         [in] The callback function, will be called at the beginning and end of the play
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_play_file_start(char *file_name, AudPlayType_e type, cb_on_player play_cb);

/*****************************************************************
* Function: ql_aud_play_stream_start
*
* Description: play audio stream data, no more than 3K at a time
* 
* Parameters:
* 	type	  		[in] play type: local/voice call
*	format          [in] stream format: wav/mp3/amr/pcm
*	play_cb         [in] The callback function, will be called at the beginning and end of the play
*   buf             [in] Audio data address, cannot be released until the end of playback
*	size            [in] stream size, no more than 3K at a time
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_play_stream_start(AudStreamFormat_e format, const void *buf, unsigned size, AudPlayType_e type, cb_on_player play_cb);

/*****************************************************************
* Function: ql_aud_data_done
*
* Description: to tell module that no data need to be write, player will stop after cache empty 
* 
* Parameters:
* 	
* Return: none
*****************************************************************/
void ql_aud_data_done(void);

/*****************************************************************
* Function: ql_aud_player_pause
*
* Description: play pause 
* 
* Parameters:
* 	
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_player_pause(void);

/*****************************************************************
* Function: ql_aud_player_resume
*
* Description: play resume
* 
* Parameters:
* 	
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_player_resume(void);

/*****************************************************************
* Function: ql_aud_player_stop
*
* Description: Stop playing, this function will release the playing 
  resource (due to the need to communicate with CP ,It will takes 
  50~70ms to execute this function)
* 
* Parameters:
* 	
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_player_stop(void);

/*****************************************************************
* Function: ql_aud_get_play_state
*
* Description: get current play state
* 
* Parameters:
* 	
* Return:
* 	AudioStatus_e:  play state
*   others:         ql_audio_errcode_e
*****************************************************************/
AudioStatus_e ql_aud_get_play_state(void);

/*****************************************************************
* Function: ql_set_record_packet_size
*
* Description: Set the packet size read by the recording callback function each time
* 
* Parameters:
* 	packet_size	  	[in] packet size; for amrwb, one packet is 41 bytes; for amrnb, one packet is 21 bytes; for PCM, 
*			 		one packet is 320bytes in 8000HZ samprate, is 640 bytes in 16000HZ, is 1280bytes
*			 		in 16000HZ, 2 channels
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_record_packet_size(int packet_size);

/*****************************************************************
* Function: ql_aud_record_file_start
*
* Description: record to file
* 
* Parameters:
* 	type	  		[in] record type: local/voice call
*	file_name       [in] file name, full path required
*	play_cb         [in] The callback function, will be called at the beginning and end of the record
*	config          [in] Recording configuration, default 8K sampling rate, single channel
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_record_file_start(char *file_name, ql_aud_config *config, AudRecType_e type,  cb_on_record record_cb);

/*****************************************************************
* Function: ql_aud_record_stream_start
*
* Description: record to memory,format is PCM
* 
* Parameters:
* 	type	  		[in] record type: local/voice call
*	play_cb         [in] The callback function, will be called at the beginning, end of the record, and data comming 
*	config          [in] Recording configuration, default 8K sampling rate, single channel
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_record_stream_start(ql_aud_config *config, AudRecType_e type, cb_on_record record_cb);

/*****************************************************************
* Function: ql_aud_record_stream_start_ex
*
* Description: record to memory, and format need be pointed
* 
* Parameters:
* 	type	  		[in] record type: local/voice call
*	play_cb         [in] The callback function, will be called at the beginning, end of the record, and data comming 
*	config          [in] Recording configuration, default 8K sampling rate, single channel
*	format			[in] record format
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_record_stream_start_ex(ql_aud_config *config, AudRecType_e type, AudStreamFormat_e format, cb_on_record record_cb);

/*****************************************************************
* Function: ql_aud_wait_play_finish
*
* Description: Wait for the playback to complete (blocks the current task until the playback is complete)
* 
* Parameters:
* 	timeout         [in] waiting timeout
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_wait_play_finish(int timeout);

/*****************************************************************
* Function: ql_aud_get_record_state
*
* Description: get current record state
* 
*
* Return:
	AudioStatus_e： state
	others:         ql_audio_errcode_e
*****************************************************************/
AudioStatus_e ql_aud_get_record_state(void);

/*****************************************************************
* Function: ql_aud_recorder_pause
*
* Description: record pause
* 
* Parameters:
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_recorder_pause(void);

/*****************************************************************
* Function: ql_aud_recorder_resume
*
* Description: resume record
* 
* Parameters:
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_recorder_resume(void);

/*****************************************************************
* Function: ql_aud_record_stop
*
* Description: Stop recording, this function will release the recording 
  resource (due to the need to communicate with CP ,It will takes 
  50~70ms to execute this function)
* 
* Parameters:
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_record_stop(void);


/*****************************************************************
* Function: ql_play_file_start
*
* Description: play file locally
* 
* Parameters:
*	file_name       [in] file name, full path required
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_play_file_start(char *file_name);

/*****************************************************************
* Function: ql_play_mp3_stream_start
*
* Description: play audio stream data locally, no more than 3K at a time
* 
* Parameters:
*   mp3_buff        [in] Audio data address, cannot be released until the end of playback
*	mp3_size        [in] stream size, no more than 3K at a time
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_play_mp3_stream_start(unsigned char *mp3_buff, int mp3_size );

/*****************************************************************
* Function: ql_set_volume
*
* Description: set the volume level of local play
* 
* Parameters:
*	vol             [in] volume level
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_volume(AUDIOHAL_SPK_LEVEL_T vol);

/*****************************************************************
* Function: ql_get_volume
*
* Description:      Gets the volume level of local play
*
* Return:
* 	0~11:         	volume level
*   others:         ql_audio_errcode_e
*****************************************************************/
AUDIOHAL_SPK_LEVEL_T ql_get_volume(void);

/*****************************************************************
* Function: ql_aud_get_output_type
*
* Description: 		get output path, speaker, earphone, or receiver
*
* Return:
* 	AudOutputType_e
****************************************************************/
AudOutputType_e ql_aud_get_output_type(void);

/*****************************************************************
* Function: ql_aud_get_input_type
*
* Description: 		get input path, main mic, HPL, ....
*
* Return:
* 	AudInputType_e
****************************************************************/
AudInputType_e ql_aud_get_input_type(void);

/*****************************************************************
* Function: ql_aud_set_output_type
*
* Description: 		set output path to speaker, earphone, or receiver
* Parameters:
*	output_type     [in] output path, speaker, earphone, or receiver
* Return:
* 	ql_audio_errcode_e
****************************************************************/
ql_audio_errcode_e ql_aud_set_output_type(AudOutputType_e output_type);

/*****************************************************************
* Function: ql_aud_set_input_type
*
* Description: 		set input path, main mic, HPL, ....
* Parameters:
*	output_type     [in] input path, main mic, HPL, ....
* Return:
* 	ql_audio_errcode_e
****************************************************************/
ql_audio_errcode_e ql_aud_set_input_type(AudInputType_e input_type);

/*****************************************************************
* Function: ql_set_audio_path_receiver
*
* Description:	    set output mode to receiver, input type 
					to main mic
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_audio_path_receiver(void);

/*****************************************************************
* Function: ql_set_audio_path_earphone
*
* Description: 	    set output mode to earphone, input type 
					to main HPL mic
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_audio_path_earphone(void);

/*****************************************************************
* Function: ql_set_audio_path_speaker
*
* Description: 		set output mode to loud speaker, input type 
					to main mic
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_audio_path_speaker(void);


/*****************************************************************
* Function: ql_play_file_stop
*
* Description: stop play file
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_play_file_stop(void);

/*****************************************************************
* Function: ql_aud_pcm_open
*
* Description: Enable local play/recording, format can be amr, wav, mp3, pcm
*
* Parameters:
*	config       [in] play/record configuration
*   flags        
	QL_PCM_BLOCK_FLAG		(0x01) ： block
	QL_PCM_NONBLOCK_FLAG	(0x02) ： not block
	QL_PCM_READ_FLAG    	(0x04) ： record
	QL_PCM_WRITE_FLAG		(0x08) ： play
*
* notice: in read mode, it will start record after ql_aud_pcm_open!!!
*
* Return:
* 	NULL:        failed
* 	others:      PCM handler
*****************************************************************/
PCM_HANDLE_T ql_aud_pcm_open(QL_PCM_CONFIG_T *config, AudStreamFormat_e format, unsigned int flags, pcm_path_e type);

/*****************************************************************
* Function: ql_pcm_open
*
* Description: Enable local play/recording, format is PCM
*
* Parameters:
*	config       [in] play/record configuration
*   flags        
	QL_PCM_BLOCK_FLAG		(0x01) ： block
	QL_PCM_NONBLOCK_FLAG	(0x02) ： not block
	QL_PCM_READ_FLAG    	(0x04) ： record
	QL_PCM_WRITE_FLAG		(0x08) ： play
*
* notice: in read mode, it will start record after ql_pcm_open!!!
*
* Return:
* 	NULL:        failed
* 	others:      PCM handler
*****************************************************************/
PCM_HANDLE_T ql_pcm_open(QL_PCM_CONFIG_T *config, unsigned int flags);

/*****************************************************************
* Function: ql_pcm_write
*
* Description: play data of pcm format locally, no more than 3K at a time
*
* Parameters:
*	handle       [in] PCM handler，get by ql_pcm_open
*   data         [in] PCM data address, cannot be released until the end of playback
*	count        [in] data size, no more than 3K at a time
*
* Return:
* 	>0:          actual write size
* 	others:      ql_audio_errcode_e
*****************************************************************/
int ql_pcm_write(PCM_HANDLE_T handle, void *data, unsigned int count);

/*****************************************************************
* Function: ql_pcm_read
*
* Description: get recording data locally
*
* Parameters:
*	handle       [in] PCM handler，get by ql_pcm_open
*   data         [in] PCM data address, cannot be released until the end of playback
*	count        [in] data size
* Return:
* 	>0:          actual read size
* 	others:      ql_audio_errcode_e
*****************************************************************/
int ql_pcm_read(PCM_HANDLE_T handle, void *data, unsigned int count);

/*****************************************************************
* Function: ql_pcm_close
*
* Description: close pcm record/play
*
* Parameters:
*	handle       [in] PCM handler, get by ql_pcm_open
*
* Return:
* 	0:         	 successful
*   others:      ql_audio_errcode_e
*****************************************************************/
int ql_pcm_close(PCM_HANDLE_T handle);

/*****************************************************************
* Function: ql_amr_stream_write
*
* Description: play amr stream locally, no more than 3K at a time
*
* Parameters:
*   data         [in] amr data address, cannot be released until the end of playback
*	count        [in] stream size, no more than 3K at a time
*
* Return:
* 	0:         	 successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_amr_stream_write(void *data, unsigned int count);

/*****************************************************************
* Function: ql_amr_stream_close
*
* Description:   close amr play, and release play resource
*
* Return:
* 	0:         	 successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_amr_stream_close(void);

/*****************************************************************
* Function: ql_amr_file_start
*
* Description: play amr file locally
*
* Parameters:
*	file_name    [in] file name, full path required
*	play_cb      [in] The callback function, will be called at the beginning and end of the play
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_amr_file_start(char *file_name, cb_on_player aud_cb);

/*****************************************************************
* Function: ql_amr_file_pause
*
* Description:   pause or resume amr player
*
* Parameters:
*	pause        [in] 1 for pause, 0 for resume
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_amr_file_pause(bool pause);

/*****************************************************************
* Function: ql_amr_file_end
*
* Description:   end amr player
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_amr_file_end(void);

/*****************************************************************
* Function: ql_aud_loop_back_start
*
* Description:   loop test input device and output device
*
* Parameters:
*	out_type     [in] output device
*   in_type 	 [in] input device
*   volume       [in] test volume
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_loop_back_start(AudOutputType_e out_type, AudInputType_e in_type, AUDIOHAL_SPK_LEVEL_T volume);

/*****************************************************************
* Function: ql_aud_loop_back_stop
*
* Description:   stop test loopback
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_loop_back_stop(void);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_EXT_CODEC

/*****************************************************************
* Function: ql_audio_iic_write
*
* Description:   write register data to codec
*
*Parameters:
*	dev_addr: iic slave addr, for alc5616 is 0x1B 
*	reg_addr: codec register addr
*	size: data size, for 16 bit register is 2, for 8bit register is 1
*	val: data to be write
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_audio_iic_write(uint8 dev_addr, uint8 reg_addr, uint8 size, uint16 val);

/*****************************************************************
* Function: ql_audio_iic_read
*
* Description:   read register data from codec
*
*Parameters:
*	dev_addr: iic slave addr, for alc5616 is 0x1B 
*	reg_addr: codec register addr
*	size: data size, for 16 bit register is 2, for 8bit register is 1
*	val: data to be read
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_audio_iic_read(uint8 dev_addr, uint8 reg_addr, uint8 size, uint16 *p_val);

/*****************************************************************
* Function: ql_aud_codec_write_nv
*
* Description:   write codec nv data, need reset to active
*
*Parameters:
*	nv_cfg: codec nv structure
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_codec_write_nv(QL_HAL_CODEC_CFG_T *nv_cfg);

/*****************************************************************
* Function: ql_aud_codec_read_nv
*
* Description:   read codec nv data
*
*Parameters:
*	nv_cfg: codec nv structure
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_codec_read_nv(QL_HAL_CODEC_CFG_T *nv_cfg);

/*****************************************************************
* Function: ql_aud_codec_clk_enable
*
* Description:   enable 26M codec clock
*
* Return: none
*****************************************************************/
void ql_aud_codec_clk_enable(void);

/*****************************************************************
* Function: ql_aud_codec_clk_enable_ex
*
* Description:   enable codec clock with Freq
*
*Parameters:
*	Freq: QL_CODEC_CLK_FREQ_E
*
* Return: none
*****************************************************************/
void ql_aud_codec_clk_enable_ex(QL_CODEC_CLK_FREQ_E Freq);

/*****************************************************************
* Function: ql_aud_codec_clk_disable
*
* Description:   disable codec clock
*
* Return: none
*****************************************************************/
void ql_aud_codec_clk_disable(void);

#endif

#ifdef __cplusplus
} /*"C" */
#endif

#endif
