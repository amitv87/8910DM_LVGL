#include <assert.h>
#include "stdlib.h"
#include "bluetooth/bluetooth.h"
#include "bluetooth/bt_a2dp.h"
#include "bt_abs.h"
#include "sci_types.h"
#include "ddb.h"
#include "bt_drv.h"
#include "osi_log.h"
#include "audio_player.h"
#include "audio_types.h"
#include "audio_device.h"
#include "osi_pipe.h"
#include "audio_player.h"



#define PIPEPLAYER_PIPE_SIZE (2048)
#define PIPEPLAYER_WRITE_PIPE_TMEOUT (100) //(500)
#define PIPEPLAYER_AUSTREAM_FORMAT_PCM_SRATE 16000
#define AG_FEATURE_IN_BAND_RING (1 << 3)


uint32_t bt_protocol_connect_state = 0x0000;
UINT8 audio_channel_mod = 0;
UINT32 audio_sample_rate = 0;

static bta2dp_connection_state_t s_bt_a2dp_connection_state = BTA2DP_CONNECTION_STATE_DISCONNECTED;


typedef struct
{
    auStreamFormat_t format;
    auPlayer_t *player;
    osiPipe_t *pipe;
} auPipePlayerContext_t;

typedef struct
{
    bt_handle_avrcp_state_t state;
    unsigned error_code;
    auPipePlayerContext_t *d;
} a2dp_player_ctrl_t;
a2dp_player_ctrl_t a2dp_player_cb = {
    .state = BT_AVRCP_PAUSE,
    .error_code = 0,
    .d = NULL,
};

bdaddr_t a2dp_addr;

bta2dp_connection_state_t app_bt_a2dp_connection_state_get(void)
{
    return s_bt_a2dp_connection_state;
}


//auPipePlayerPipeWrite
static bool bt_app_a2dp_player_init_write_data(auPipePlayerContext_t *d, const void *data, unsigned size)
{
    if (d->pipe == NULL)
        return false;
    int written = osiPipeWriteAll(d->pipe, data, size, PIPEPLAYER_WRITE_PIPE_TMEOUT);
    // OSI_LOGD(0, "a2dp player sbc write size/%d written/%d", size, written);
    OSI_LOGI(0, "BT A2DP player_written=%d,size=%d", written, size);
    return (written == size);
}

static void auPipePlayerDelete(auPipePlayerContext_t *d)
{
    auPlayerDelete(d->player);
    osiPipeDelete(d->pipe);
    d->player = NULL;
    d->pipe = NULL;
    free(d);
    d = NULL;
}

static auPipePlayerContext_t *auPipePlayerCreate(auStreamFormat_t format)
{
    if (format != AUSTREAM_FORMAT_PCM &&
        format != AUSTREAM_FORMAT_WAVPCM &&
        format != AUSTREAM_FORMAT_MP3 &&
        format != AUSTREAM_FORMAT_AMRNB &&
        format != AUSTREAM_FORMAT_AMRWB &&
        format != AUSTREAM_FORMAT_SBC)
        return NULL;

    auPipePlayerContext_t *d = (auPipePlayerContext_t *)calloc(1, sizeof(*d));
    if (d == NULL)
        goto failed_nomem;

    d->pipe = osiPipeCreate(PIPEPLAYER_PIPE_SIZE);
    if (d->pipe == NULL)
        goto failed_nomem;

    osiPipeReset(d->pipe);

    d->player = auPlayerCreate();
    if (d->player == NULL)
        goto failed_nomem;

    if (format == AUSTREAM_FORMAT_SBC)
    {
        auFrame_t frame = {.sample_format = AUSAMPLE_FORMAT_S16, .sample_rate = audio_sample_rate, .channel_count = audio_channel_mod};
        auDecoderParamSet_t params[2] = {{AU_DEC_PARAM_FORMAT, &frame}, {0}};

        if (!auPlayerStartPipeV2(d->player, AUDEV_PLAY_TYPE_LOCAL, format, params, d->pipe))
            goto failed;
        return d;
    }
    else
    {
        if (!auPlayerStartPipeV2(d->player, AUDEV_PLAY_TYPE_LOCAL, format, NULL, d->pipe))
            goto failed;
        return d;
    }
failed_nomem:
    auPipePlayerDelete(d);
    return NULL;

failed:
    auPipePlayerDelete(d);
    return NULL;
}
/***********************/
void set_bt_protocol_state(uint8_t pos, int flag)
{
    uint32_t n = 1;
    n = n << pos;
    if (flag == BT_PROTOCOL_DISCONNECTED)
    {
        bt_protocol_connect_state = bt_protocol_connect_state & (~n);
    }
    else if (flag == BT_PROTOCOL_CONNECTED)
    {
        bt_protocol_connect_state = bt_protocol_connect_state | n;
    }
}
uint8_t get_bt_specific_protocol_state(uint8_t pos)
{
    if ((bt_protocol_connect_state >> pos) & 0x0001)
    {
        return BT_PROTOCOL_CONNECTED;
    }
    else
    {
        return BT_PROTOCOL_DISCONNECTED;
    }
}

typedef struct bt_a2dp_audio_info_tag
{
    uint16    seq_num;
    uint16    reserved;
    UINT32   length;
    uint8*    data;
}bt_a2dp_audio_info_t;

extern bool bt_send_msg_btAud_task(uint16 msg_id, void *data_ptr);

UINT8 bt_app_a2dp_audio_stream_callback(UINT16 seq_num, hci_data_t *p_buf, UINT32 length)
{
    OSI_LOGI(0,"BT A2DP bt_app_a2dp_audio_stream_callback a2dp_player_cb.state %d",a2dp_player_cb.state);
    if (BT_AVRCP_START == a2dp_player_cb.state)
    {
        bt_a2dp_audio_info_t* bt_a2dp_audio_info = (bt_a2dp_audio_info_t*)calloc(1,sizeof(bt_a2dp_audio_info_t)); 
        bt_a2dp_audio_info->seq_num = seq_num;
        bt_a2dp_audio_info->data = (uint8*)calloc(1,length);
        memcpy(bt_a2dp_audio_info->data, p_buf->data, length);
        bt_a2dp_audio_info->length = length;
        bt_send_msg_btAud_task(BT_AUD_A2DP_MSG, bt_a2dp_audio_info);

    }
    return 1;
}
void bt_app_a2dp_audio_config_callback(UINT8 codec_type, UINT32 sample_rate, bdaddr_t *addr)
{
    //todo
    OSI_LOGI(0,"BT A2DP bt_app_a2dp_audio_config_callback codec_type %d, sample_rate %d",codec_type,sample_rate);
     audio_sample_rate = sample_rate;
     audio_channel_mod = 2;
}
void bt_app_a2dp_audio_state_callback(bta2dp_audio_state_t state, bdaddr_t *addr)
{
   //todo
   OSI_LOGI(0,"BT A2DP bt_app_a2dp_audio_state_callback %d, a2dp_player_cb.state %d",state,a2dp_player_cb.state);
   
   if (BTA2DP_AUDIO_STATE_START == state)
    {
        if (BT_AVRCP_START == a2dp_player_cb.state)
        {
            a2dp_player_cb.error_code = 0x1010;
            //error
        }
        else
        {
            //create pipe player
            audevStopTone();
            audevBtVoiceStop();
            a2dp_player_cb.d = auPipePlayerCreate(AUSTREAM_FORMAT_SBC); //should be sbc
            if (a2dp_player_cb.d == NULL)
            {
                a2dp_player_cb.state = BT_AVRCP_PAUSE;
                //error;
                //OSI_LOGI(0, "a2dp_player_cb.state1");
            }
            else
            {
                a2dp_player_cb.state = BT_AVRCP_START;
                //OSI_LOGI(0, "a2dp_player_cb.state2");
            }
        }
    }
    else if (BTA2DP_AUDIO_STATE_STOPPED == state)
    {
        //delete pipe player
        if (BT_AVRCP_START == a2dp_player_cb.state)
        {
            osiPipeSetEof(a2dp_player_cb.d->pipe); //
            osiPipeStop(a2dp_player_cb.d->pipe);   //
            auPipePlayerDelete(a2dp_player_cb.d);
            a2dp_player_cb.d = NULL;
        }
        a2dp_player_cb.state = BT_AVRCP_PAUSE;
    }
}

void bt_app_a2dp_stop(void)
{
     //delete pipe player
     if (BT_AVRCP_START == a2dp_player_cb.state)
     {
         if((a2dp_player_cb.d) && (a2dp_player_cb.d->pipe))
         {
            osiPipeSetEof(a2dp_player_cb.d->pipe); //
            osiPipeStop(a2dp_player_cb.d->pipe);   //
            auPipePlayerDelete(a2dp_player_cb.d);
         }
            a2dp_player_cb.d = NULL;
            a2dp_player_cb.error_code = 0;
            a2dp_player_cb.state = BT_AVRCP_PAUSE;
     }

  audio_channel_mod = 0;
  audio_sample_rate = 0;
  s_bt_a2dp_connection_state = BTA2DP_CONNECTION_STATE_DISCONNECTED;

}

void bt_app_a2dp_connection_state_callback(bta2dp_connection_state_t state, bdaddr_t *addr)
{
  //todo
  OSI_LOGI(0,"BT A2DP bt_app_a2dp_connection_state_callback state %d addr %x,%x,%x,%x,%x,%x",state,addr->bytes[0],addr->bytes[1],addr->bytes[2],addr->bytes[3],addr->bytes[4],addr->bytes[5]);
  if (state == BTA2DP_CONNECTION_STATE_CONNECTED)
    {
        set_bt_protocol_state(BT_A2DP_POS, BT_PROTOCOL_CONNECTED);
		memcpy(a2dp_addr.bytes, addr->bytes, 6);
		
		OSI_LOGI(0,"BT A2DP connect success");
    }
    else if (state == BTA2DP_CONNECTION_STATE_DISCONNECTED)
    {
        set_bt_protocol_state(BT_A2DP_POS, BT_PROTOCOL_DISCONNECTED);
        if (BT_AVRCP_START == a2dp_player_cb.state)
        {
            osiPipeSetEof(a2dp_player_cb.d->pipe); //
            osiPipeStop(a2dp_player_cb.d->pipe);   //
            auPipePlayerDelete(a2dp_player_cb.d);
            a2dp_player_cb.d = NULL;
            a2dp_player_cb.state = BT_AVRCP_PAUSE;
        }
		OSI_LOGI(0,"BT A2DP disconnect success");
    }
    //    printf("\n%s,state:%d,addr:%x:%x:%x:%x:%x:%x\n", __func__,get_bt_specific_protocol_state(BT_A2DP_POS),addr->bytes[0],addr->bytes[1],addr->bytes[2],
    //        addr->bytes[3],addr->bytes[4],addr->bytes[5]);
    s_bt_a2dp_connection_state = state;
}
bta2dp_callbacks_t bta2dp_callbacks = {
    bt_app_a2dp_connection_state_callback,
    bt_app_a2dp_audio_state_callback,
    bt_app_a2dp_audio_config_callback,
    bt_app_a2dp_audio_stream_callback
  };

void app_bt_a2dp_init(void)
{
  bt_a2dp_callback_register(&bta2dp_callbacks);

  bt_protocol_connect_state = 0x0000;
  audio_channel_mod = 0;
  audio_sample_rate = 0;
  s_bt_a2dp_connection_state = BTA2DP_CONNECTION_STATE_DISCONNECTED;

  a2dp_player_cb.state = BT_AVRCP_PAUSE;
  a2dp_player_cb.error_code = 0;
  a2dp_player_cb.d = NULL;

}


bool app_bt_a2dp_connect_req(bdaddr_t* bdAddr)
{
    if(get_bt_specific_protocol_state(BT_A2DP_POS) != BT_PROTOCOL_DISCONNECTED)
        return false;
    
    if(bt_a2dp_connect(bdAddr) == BT_SUCCESS)
    {
        memcpy(a2dp_addr.bytes, bdAddr->bytes, SIZE_OF_BDADDR);
        return true;
    }

    return false;
}

bool app_bt_a2dp_disconnect_req(void)
{
    if(get_bt_specific_protocol_state(BT_A2DP_POS) != BT_PROTOCOL_CONNECTED)
        return false;
    if(bt_a2dp_disconnect(&a2dp_addr) == BT_SUCCESS)
    {
        return true;
    }

    return false;
}

void bt_a2dp_msg_handle(void* msg_data)
{
     bool error_code;
     bt_a2dp_audio_info_t* bt_a2dp_audio_info = (bt_a2dp_audio_info_t*)msg_data;
        //write pipe
        error_code = bt_app_a2dp_player_init_write_data(a2dp_player_cb.d, (void *)bt_a2dp_audio_info->data, (unsigned)bt_a2dp_audio_info->length); // length = ? bits
        if (false == error_code)
        {
            if (BT_AVRCP_PAUSE != a2dp_player_cb.state)
            {
                a2dp_player_cb.state = BT_AVRCP_PAUSE;
                if (get_bt_specific_protocol_state(BT_AVRCP_POS) == BT_PROTOCOL_CONNECTED)
                {
                    //printf("\nsend pause to phone!\n");//
                    app_bt_avrcp_handle(BT_AVRCP_PAUSE);
                }
            }
            OSI_LOGI(0, "audio_stream data failed");
        }

        free(bt_a2dp_audio_info->data);
 }

