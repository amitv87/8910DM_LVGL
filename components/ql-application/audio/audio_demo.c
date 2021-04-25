/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"

#include "ql_log.h"
#include "audio_demo.h"
#include "ql_osi_def.h"
#include "ql_audio.h"
#include "ql_fs.h"
#include "ql_i2c.h"

#define QL_AUDIO_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_AUDIO_LOG(msg, ...)			QL_LOG(QL_AUDIO_LOG_LEVEL, "ql_audio", msg, ##__VA_ARGS__)
#define QL_AUDIO_LOG_PUSH(msg, ...)		QL_LOG_PUSH("ql_AUDIO", msg, ##__VA_ARGS__)

#if !defined(audio_demo_no_err)
	#define audio_demo_no_err(x, action, str)																		\
			do                                                                                                  \
			{                                                                                                   \
				if(x != 0)                                                                        				\
				{                                                                                               \
					QL_AUDIO_LOG(str);																			\
					{action;}																					\
				}                                                                                               \
			} while( 1==0 )
#endif

static uint8 *pcm_buffer = NULL;
static uint  pcm_data_size = 0;

static int play_callback(char *p_data, int len, enum_aud_player_state state)
{
	if(state == AUD_PLAYER_START)
	{
		QL_AUDIO_LOG("player start run");
	}
	else if(state == AUD_PLAYER_FINISHED)
	{
		QL_AUDIO_LOG("player stop run");
	}
	else
	{
		QL_AUDIO_LOG("type is %d", state);
	}

	return QL_AUDIO_SUCCESS;
}

static int record_callback(char *p_data, int len, enum_aud_record_state state)
{	
	if(state == AUD_RECORD_START)
	{
		QL_AUDIO_LOG("recorder start run");
	}
	else if(state == AUD_RECORD_CLOSE)
	{
		QL_AUDIO_LOG("recorder stop run");
	}
	else if(state == AUD_RECORD_DATA)
	{
		if(len <= 0)
			return -1;

		if(pcm_data_size > RECORD_BUFFER_MAX){
			return -1;
		}
		else{
			memcpy(pcm_buffer+pcm_data_size, p_data, len);
			pcm_data_size += len;
		}
	}

	return QL_AUDIO_SUCCESS;
}

static void ql_audio_demo_thread(void *param)
{
	QL_AUDIO_LOG("enter audio demo");

#ifdef QL_APP_FEATURE_EXT_CODEC
	test_codec();
#endif

//	ql_set_volume(TEST_PLAY_VOLUME);
//	test_pcm();
//	test_mp3();
//	test_wav();
//	test_amr();
//	test_record_file();
//	test_record_stream();
//	test_amr_stream();

	QL_AUDIO_LOG("test done, exit audio demo");
	ql_rtos_task_delete(NULL);
}

void ql_audio_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	ql_task_t ql_audio_task = NULL;
	
    QL_AUDIO_LOG("audio demo enter");
	
    err = ql_rtos_task_create(&ql_audio_task, 4096*2, APP_PRIORITY_NORMAL, "ql_audio", ql_audio_demo_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
    {
		QL_AUDIO_LOG("audio task create failed");
	}
}

void test_amr_stream(void)
{
	PCM_HANDLE_T PCM = NULL;
	QL_PCM_CONFIG_T config;
	void *data = NULL;
	int size, total_size = 0, cnt=0, write_cnt=0;

	config.channels = 1; //单声道
	config.samplerate = 16000;
	
	PCM = ql_aud_pcm_open(&config, QL_AUDIO_FORMAT_AMRWB, QL_PCM_BLOCK_FLAG|QL_PCM_READ_FLAG, QL_PCM_LOCAL);
	if(PCM == NULL)
	{
		QL_AUDIO_LOG("open pcm failed");
		goto exit;		
	}
	data = malloc(100*1024);
	if(data == NULL)
	{
		goto exit;	
	}

	QL_AUDIO_LOG("start read");

//start record
	while(total_size < 10*1024)
	{
		size = ql_pcm_read(PCM, data+total_size, 41);

		if(size <= 0)
		{
			break;
		}
		
		total_size += size;
	}	

	QL_AUDIO_LOG("exit record");

	if(total_size <= 0)
	{
		QL_AUDIO_LOG("read pcm failed");
		goto exit;			
	}
	QL_AUDIO_LOG("size is %d", total_size);

	if(ql_pcm_close(PCM) != 0)
	{
		QL_AUDIO_LOG("close pcm failed");
		goto exit;			
	}
	PCM = NULL;

	PCM = ql_aud_pcm_open(&config, QL_AUDIO_FORMAT_AMRWB, QL_PCM_BLOCK_FLAG|QL_PCM_WRITE_FLAG, QL_PCM_LOCAL);
	if(PCM == NULL)
	{
		QL_AUDIO_LOG("open pcm failed");
		goto exit;		
	}

	while(write_cnt < total_size)
	{
		if(total_size - write_cnt > PACKET_WRITE_MAX_SIZE)  //单次最多可写3K字节
		{
			cnt = ql_pcm_write(PCM, data+write_cnt, PACKET_WRITE_MAX_SIZE);
		}
		else
		{
			cnt = ql_pcm_write(PCM, data+write_cnt, total_size - write_cnt);
		}
		if(cnt <= 0)
		{
			QL_AUDIO_LOG("write failed");
			goto exit;	
		}
		write_cnt += cnt;
	}

	ql_aud_data_done();
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	
	QL_AUDIO_LOG("play finish");	

exit:
	if(PCM != NULL)
	{
		ql_pcm_close(PCM);
	}
	if(data != NULL)
	{
		free(data);
		data = NULL;
	}
}


void test_record_stream(void)
{
	ql_aud_config config = {0};
	int cnt = 0, total_cnt=0, err;

	pcm_buffer = malloc(RECORD_BUFFER_MAX);
	if(!pcm_buffer){
		return;
	}
	
/*	录音    */	
	if(ql_aud_record_stream_start_ex(&config, QL_REC_TYPE_MIC, QL_AUDIO_FORMAT_AMRWB, record_callback))
	{
		QL_AUDIO_LOG("record fail");
		goto exit;
	}
	ql_rtos_task_sleep_s(5);   //record 5s
	ql_aud_record_stop();

	if(pcm_data_size <= 0){
		QL_AUDIO_LOG("data invalid");
		goto exit;			
	}

/*	读取录音文件用于播放，此处也可调用	ql_aud_play_file_start,或者ql_pcm_open+ql_pcm_write去播放    				  */
	ql_set_audio_path_speaker();
	ql_set_volume(TEST_PLAY_VOLUME);

	while(total_cnt < pcm_data_size)
	{
		if(pcm_data_size - total_cnt > PACKET_WRITE_MAX_SIZE)  //单次最多可写3K字节
		{
			cnt = PACKET_WRITE_MAX_SIZE;   
			err = ql_aud_play_stream_start(QL_AUDIO_FORMAT_AMRWB, pcm_buffer+total_cnt, cnt, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback);
		}
		else
		{
			cnt = pcm_data_size - total_cnt;
			err = ql_aud_play_stream_start(QL_AUDIO_FORMAT_AMRWB, pcm_buffer+total_cnt, cnt, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback);
		}
		
		if(err < 0)
		{
			QL_AUDIO_LOG("start failed");
			goto exit;
		}
		else
		{
			QL_AUDIO_LOG("play %d bytes, total %d", cnt, total_cnt);
			total_cnt += cnt;
		}
	}

	ql_aud_data_done();	
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	ql_aud_player_stop();
	
	QL_AUDIO_LOG("test successful");

exit:

	if(pcm_buffer){
		free(pcm_buffer);
		pcm_buffer = NULL;
		pcm_data_size = 0;
	}
}

void test_record_file(void)
{
	ql_aud_config config = {0};
	config.samplerate = 8000;

	if(ql_aud_record_file_start(TEST_RECORD_WAV_NAME, &config, QL_REC_TYPE_MIC, NULL) != QL_AUDIO_SUCCESS)
	{
		QL_AUDIO_LOG("record failed");
		return;
	}
	QL_AUDIO_LOG("record start");
	ql_rtos_task_sleep_s(5);   //record 5s

	ql_aud_record_stop();
	QL_AUDIO_LOG("record finish, start play");

	ql_set_audio_path_speaker();
	if(ql_aud_play_file_start(TEST_RECORD_WAV_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
		return;
	}

	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	ql_aud_player_stop();
	QL_AUDIO_LOG("test successful");
}

void test_mp3(void)
{
	if(ql_aud_play_file_start(TEST_MP3_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
		return;
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);

	if(ql_aud_play_file_start(TEST_MP3_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
		return;
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);

	ql_aud_player_stop(); //播放结束，释放播放资源
	
	QL_AUDIO_LOG("test mp3 successful");
}


void test_wav(void)
{
	int cnt = 0;

	if(ql_aud_play_file_start(TEST_WAV_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
		return;
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	QL_AUDIO_LOG("play %d times ok", ++cnt);

	if(ql_aud_play_file_start(TEST_WAV_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
		return;
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	
	QL_AUDIO_LOG("play %d times ok", ++cnt);
	
	ql_aud_player_stop(); //播放结束，释放播放资源
	QL_AUDIO_LOG("test wav successful");
}

void test_amr(void)
{
	int cnt = 0;
	
	if(ql_aud_play_file_start(TEST_AMR_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	QL_AUDIO_LOG("play %d times ok", ++cnt);

	if(ql_aud_play_file_start(TEST_AMR_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	QL_AUDIO_LOG("play %d times ok", ++cnt);

	ql_aud_player_stop(); //播放结束，释放播放资源
	QL_AUDIO_LOG("test wav successful");
}

void test_pcm(void)
{
	PCM_HANDLE_T PCM = NULL;
	QL_PCM_CONFIG_T config;
	void *data = NULL;
	int size=0, write_cnt=0, cnt=0;

	config.channels = 1; //单声道
	config.samplerate = 8000;
	
	PCM = ql_pcm_open(&config, QL_PCM_BLOCK_FLAG|QL_PCM_READ_FLAG);
	if(PCM == NULL)
	{
		QL_AUDIO_LOG("open pcm failed");
		goto exit;		
	}
	data = malloc(50*1024);
	if(data == NULL)
	{
		goto exit;	
	}

	QL_AUDIO_LOG("start read");
	size = ql_pcm_read(PCM, data, 50*1024);
	if(size <= 0)
	{
		QL_AUDIO_LOG("read pcm failed");
		goto exit;			
	}
	QL_AUDIO_LOG("size is %d", size);

	if(ql_pcm_close(PCM) != 0)
	{
		QL_AUDIO_LOG("close pcm failed");
		goto exit;			
	}
	PCM = NULL;
	
	ql_set_audio_path_speaker();
	
	PCM = ql_pcm_open(&config, QL_PCM_BLOCK_FLAG|QL_PCM_WRITE_FLAG);
	if(PCM == NULL)
	{
		QL_AUDIO_LOG("open pcm failed");
		goto exit;		
	}

	QL_AUDIO_LOG("start write");

	while(write_cnt < size)
	{
		if(size - write_cnt > PACKET_WRITE_MAX_SIZE)  //单次最多可写3K字节
		{
			cnt = ql_pcm_write(PCM, data+write_cnt, PACKET_WRITE_MAX_SIZE);
		}
		else
		{
			cnt = ql_pcm_write(PCM, data+write_cnt, size - write_cnt);
		}
		if(cnt <= 0)
		{
			QL_AUDIO_LOG("write failed");
			goto exit;	
		}
		write_cnt += cnt;
	}

	ql_aud_data_done();
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);

	QL_AUDIO_LOG("play done");

	
	if(ql_pcm_close(PCM) != 0)
	{
		QL_AUDIO_LOG("close pcm failed");
		goto exit;			
	}
	PCM = NULL;

	QL_AUDIO_LOG("play finish");	

exit:
	if(PCM != NULL)
	{
		ql_pcm_close(PCM);
	}
	if(data != NULL)
	{
		free(data);
		data = NULL;
	}
}

#ifdef  QL_APP_FEATURE_EXT_CODEC
void test_codec(void)
{
	ql_aud_codec_clk_enable();
	ql_I2cInit(QL_AUDIO_IIC_CHANNEL, STANDARD_MODE);

	aud_codec_init();
	ql_switch_ext_codec(TRUE);    //need reset, as dsp only read audio nv when start
}

void ALC5616_reg_nv_load(QL_HAL_CODEC_CFG_T *nv_cfg)
{
	AUD_CODEC_REG_T init_reg[]  		= RT5616_NV_INIT_REG,       \
					close_reg[] 		= RT5616_NV_CLOSE_REG,	    \
					sample_8k_reg[]     = RT5616_NV_SAMPLE_8K_REG,  \
					sample_16k_reg[]    = RT5616_NV_SAMPLE_16K_REG,	\
					sample_32k_reg[]	= RT5616_NV_SAMPLE_32K_REG;
	
	memset(&nv_cfg->initRegCfg[0], 0, 100*sizeof(AUD_CODEC_REG_T));         //clear codec init register nv
	memset(&nv_cfg->closeRegCfg[0], 0, 50*sizeof(AUD_CODEC_REG_T));   		//clear codec close register nv
	memset(&nv_cfg->sampleRegCfg[0][0], 0, 120*sizeof(AUD_CODEC_REG_T));    //clear codec samprate register nv
	memset(&nv_cfg->inpathRegCfg[0][0], 0, 120*sizeof(AUD_CODEC_REG_T));    //clear codec in path register nv
	memset(&nv_cfg->outpathRegCfg[0][0], 0, 80*sizeof(AUD_CODEC_REG_T));    //clear codec output path register nv

	memcpy(nv_cfg->initRegCfg, init_reg, sizeof(init_reg));
	memcpy(nv_cfg->closeRegCfg, close_reg, sizeof(close_reg));
	memcpy(&nv_cfg->sampleRegCfg[0], sample_8k_reg, sizeof(sample_8k_reg));
	memcpy(&nv_cfg->sampleRegCfg[3], sample_16k_reg, sizeof(sample_16k_reg));
	memcpy(&nv_cfg->sampleRegCfg[6], sample_32k_reg, sizeof(sample_32k_reg));
}

//switch to ext codec ,need reset
ql_audio_errcode_e ql_switch_ext_codec(bool enable)
{
	ql_audio_errcode_e err = QL_AUDIO_SUCCESS;
	uint16 codec_id;
	QL_HAL_CODEC_CFG_T *halCodecCfg = NULL;

	halCodecCfg = calloc(1, sizeof(QL_HAL_CODEC_CFG_T));
	if(!halCodecCfg){
		return QL_AUDIO_UNKNOWN_ERROR;
		goto exit;
	}

	err = ql_aud_codec_read_nv(halCodecCfg);
    audio_demo_no_err(err, goto exit, "read failed");
	
	switch(enable)
	{
	case 1:   //ext codec
		err = ql_audio_iic_read(RT5616_I2C_SLAVE_ADDR, RT5616_VENDOR_ID_REG, 2, &codec_id);  //read 5616 firstly, if is not 5616, then read other codec id 
		audio_demo_no_err(err, goto exit, "read codec failed");

		if(codec_id == RT5616_VENDOR_ID)  //init codec 5616 nv
		{	
			QL_AUDIO_LOG("ext codec is ALC5616, start config");

			halCodecCfg->i2cCfg.id = (QL_AUDIO_IIC_CHANNEL == i2c_1 ? 0:1);  //0 for use iic1, 1 for use iic 2, and iic 2 is default 
			halCodecCfg->basicCfg.iic_data_width = 3;  //iic register 16bit, hsb first
			halCodecCfg->basicCfg.codecAddr = RT5616_I2C_SLAVE_ADDR;   //iic addr, dsp will use it to connect codec
			ALC5616_reg_nv_load(halCodecCfg);  //load 5616 register to struct
		}
		else
		{
			err = QL_AUDIO_OPER_NOT_SUPPORTED;
			audio_demo_no_err(err, goto exit, "not support current codec");
			//NAU8810 NOT support now
		}
		halCodecCfg->basicCfg.externalCodec = 1;   //switch to ext codec
		
		err = ql_aud_codec_write_nv(halCodecCfg);
		audio_demo_no_err(err, goto exit, "write nv failed");	
	break;

	case 0:   //internal codec
		halCodecCfg->basicCfg.externalCodec = 0;			
		err = ql_aud_codec_write_nv(halCodecCfg);
		audio_demo_no_err(err, goto exit, "write nv failed");
	break;	
	}

exit:
	if(halCodecCfg){
		free(halCodecCfg);
	}

	return err;
}

ql_audio_errcode_e aud_codec_init(void)
{
	int err = QL_AUDIO_SUCCESS, i=0;
	uint16 codec_id;
	QL_HAL_CODEC_CFG_T *halCodecCfg = NULL;
	
	halCodecCfg = calloc(1, sizeof(QL_HAL_CODEC_CFG_T));
	audio_demo_no_err(!halCodecCfg, return QL_AUDIO_UNKNOWN_ERROR, "malloc memory failed");

	err = ql_aud_codec_read_nv(halCodecCfg);
	audio_demo_no_err(err, goto exit, "read failed");

	if(halCodecCfg->basicCfg.externalCodec)  //1 for use external codec
	{
		QL_AUDIO_LOG("codec iic addr is %p, clk mode is %d", halCodecCfg->basicCfg.codecAddr, halCodecCfg->i2cCfg.clockMode);

		ql_aud_codec_clk_enable();

		err = ql_audio_iic_read(RT5616_I2C_SLAVE_ADDR, RT5616_VENDOR_ID_REG, 2, &codec_id);
		audio_demo_no_err(err, goto exit, "read codec failed");

		if(codec_id == RT5616_VENDOR_ID)  //init codec 5616
		{	
			AUD_CODEC_REG_T reg_list[] = RT5616_INIT_REG;	
			for (i = 0; i < sizeof(reg_list)/sizeof(reg_list[0]); i++)
			{
				err = ql_audio_iic_write(RT5616_I2C_SLAVE_ADDR, reg_list[i].regAddr, 2, reg_list[i].val);
				audio_demo_no_err(err, goto exit, "write codec register fail");
			}
		}
		else  //NAU8810 not support now
		{
			err = QL_AUDIO_UNKNOWN_ERROR;
			audio_demo_no_err(err, goto exit, "codec not valid");
		}
	}
	
exit:
	if(halCodecCfg){
		free(halCodecCfg);
	}
	return err;
}

static ql_audio_errcode_e ql_rt5616_write_reg(uint8 RegAddr, uint16 RegData)
{
    ql_audio_errcode_e status = QL_AUDIO_SUCCESS;
    uint8 param_data[3] = {0x00};
    uint8 retry_count = 5;

    param_data[0] = (uint8)((RegData >> 8) & 0xFF);
    param_data[1] = (uint8)(RegData & 0xff);

    do 
    {
        status = (ql_audio_errcode_e)ql_I2cWrite(QL_AUDIO_IIC_CHANNEL, RT5616_I2C_SLAVE_ADDR, RegAddr, param_data, 2);
        if (status != QL_AUDIO_SUCCESS)
        {
            QL_AUDIO_LOG("Error:[%dth] device[0x%x] addr[0x%x] data[0x%x] failed", retry_count, RT5616_I2C_SLAVE_ADDR, RegAddr, RegData);
        }
		else
		{
			break;
		}
    } while (--retry_count);

    return (status == 0 ? QL_AUDIO_SUCCESS :  QL_AUDIO_CODEC_WR_FAIL);
}

static ql_audio_errcode_e ql_rt5616_read_reg(uint8 RegAddr, uint16 *p_value)
{
	ql_audio_errcode_e status = QL_AUDIO_SUCCESS;
	uint8 temp_buf[2];
	uint8 retry_count = 5;

	do 
    {
        status = (ql_audio_errcode_e)ql_I2cRead(QL_AUDIO_IIC_CHANNEL, RT5616_I2C_SLAVE_ADDR, RegAddr, temp_buf, 2);
		if (status != QL_AUDIO_SUCCESS)
		{
            QL_AUDIO_LOG("Error:[%dth] device[0x%x] addr[0x%x] failed", retry_count, RT5616_I2C_SLAVE_ADDR, RegAddr);
        }	
		else
        {
			*p_value = (((uint16)temp_buf[0]) << 8) | temp_buf[1];
			break;
		}
	} while (--retry_count);

	return (status == 0 ? QL_AUDIO_SUCCESS :  QL_AUDIO_CODEC_RD_FAIL);
}

ql_audio_errcode_e ql_audio_iic_read(uint8 dev_addr, uint8 reg_addr, uint8 size, uint16 *p_val)
{
	ql_audio_errcode_e err = 0;

    /* 5616 */
    if(0x1B == dev_addr)
    {
        err = ql_rt5616_read_reg(reg_addr, p_val);
    }
    /* 8810 */
    else if (0x1A == dev_addr)
    {
        //not support now
        return QL_AUDIO_INVALID_PARAM;
    }
	else
	{
		return QL_AUDIO_INVALID_PARAM;
	}

    return err;	
}


ql_audio_errcode_e ql_audio_iic_write(uint8 dev_addr, uint8 reg_addr, uint8 size, uint16 val)
{
    ql_audio_errcode_e err = QL_AUDIO_SUCCESS;

    /* 5616 */
    if (0x1B == dev_addr)
    {
        err = ql_rt5616_write_reg(reg_addr, val);
    }
    /* 8810 */
    else if (0x1A == dev_addr)
    {
        //not support now
        return QL_AUDIO_INVALID_PARAM;
    }
	else
	{
		return QL_AUDIO_INVALID_PARAM;
	}

    return err;
}

#endif

