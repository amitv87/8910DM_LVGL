#include <assert.h>
#include "stdlib.h"
#include "bluetooth/bluetooth.h"
#include "bluetooth/bt_avrcp.h"
#include "bt_abs.h"
#include "sci_types.h"
#include "ddb.h"
#include "bt_drv.h"
#include "osi_log.h"
#include "audio_player.h"
#include "audio_types.h"
#include "audio_device.h"

static bdaddr_t s_bt_avrcp_connected_peer_addr = {0};
static btavrcp_connection_state_t s_bt_avrcp_connection_state = BTAVRCP_CONNECTION_STATE_DISCONNECTED;

#define BT_OPID_PLAY  (0x44)
#define BT_OPID_PAUSE  (0x46)
#define BT_OPID_FORWARD (0x4b)
#define BT_OPID_BACKWARD (0x4c)

extern void app_bt_protocol_state_set(uint8 pos, int flag);

void bt_app_avrcp_connection_state_callback(btavrcp_connection_state_t state, bdaddr_t *addr)
{
    OSI_LOGI(0,"BT AVRCP bt_app_avrcp_connection_state_callback %d",state);
    if (state == BTAVRCP_CONNECTION_STATE_CONNECTED)
    {
        app_bt_protocol_state_set(BT_AVRCP_POS, BT_PROTOCOL_CONNECTED);
		OSI_LOGI(0,"BT AVRCP CONNECTED");
    }
    else if (state == BTAVRCP_CONNECTION_STATE_DISCONNECTED)
    {
        app_bt_protocol_state_set(BT_AVRCP_POS, BT_PROTOCOL_DISCONNECTED);
		OSI_LOGI(0,"BT AVRCP DISCONNECTED");
    }

    s_bt_avrcp_connection_state = state;

    OSI_LOGI(0,"BT AVRCP bt_app_avrcp_connection_state_callback,state:%d,addr:%x:%x:%x:%x:%x:%x\n",state,addr->bytes[0],addr->bytes[1],addr->bytes[2],
            addr->bytes[3],addr->bytes[4],addr->bytes[5]);
    memcpy(s_bt_avrcp_connected_peer_addr.bytes, addr->bytes, 6);
    //    printf("\n%s,state:%d,avrcp_addr:%x:%x:%x:%x:%x:%x\n", __func__,state,avrcp_addr.bytes[0],avrcp_addr.bytes[1],avrcp_addr.bytes[2],
    //        avrcp_addr.bytes[3],avrcp_addr.bytes[4],avrcp_addr.bytes[5]);
}

typedef struct bt_avrcp_data_info_tag
{
    uint8 volume;
}bt_avrcp_data_info_t;

extern bool bt_send_msg_btAud_task(uint16 msg_id, void *data_ptr);
void bt_app_avrcp_volume_changed_callback(UINT8 volume, bdaddr_t *addr)
{
    bt_avrcp_data_info_t* data = (bt_avrcp_data_info_t*)calloc(1,sizeof(bt_avrcp_data_info_t));
    data->volume = volume;

    OSI_LOGI(0,"BT AVRCP bt_app_avrcp_volume_changed_callback volume %d", volume);

    bt_send_msg_btAud_task(BT_AUD_AVRCP_MSG, data);
}

void bt_avrcp_msg_handler(void* msg_data)
{
    bt_avrcp_data_info_t* bt_avrcp_data_info = (bt_avrcp_data_info_t*)msg_data;
    unsigned level = bt_avrcp_data_info->volume & 0x7F; // onle used 7LSB
    unsigned level_old;
    level = level * 100 / 127;
    OSI_LOGI(0,"bt_avrcp_msg_handler level %d",level);
    if (level > 100)
    {
        level = 100;
    }
    level_old = audevGetPlayVolume();
    if (level_old != level)
    {
        if (!audevSetPlayVolume(level))
        {
            OSI_LOGI(0, "audio_volum_error=%d", level);
        }
    }
    //OSI_LOGI(0, "audio_volum_change =%d",level);
}


btavrcp_callbacks_t btavrcp_callbacks = {

    .connection_state_callback = bt_app_avrcp_connection_state_callback,
    .volume_changed_callback = bt_app_avrcp_volume_changed_callback,
};

void app_bt_avrcp_init(void)
{
    bt_avrcp_callback_register(&btavrcp_callbacks);

    memset(&s_bt_avrcp_connected_peer_addr, 0, SIZE_OF_BDADDR);

    s_bt_avrcp_connection_state = BTAVRCP_CONNECTION_STATE_DISCONNECTED;
}

void app_bt_avrcp_stop(void)
{
    memset(&s_bt_avrcp_connected_peer_addr, 0, SIZE_OF_BDADDR);

    s_bt_avrcp_connection_state = BTAVRCP_CONNECTION_STATE_DISCONNECTED;
}

bool app_bt_avrcp_start(void)
{
    uint8 invalid_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    if(0 == memcmp(invalid_addr, s_bt_avrcp_connected_peer_addr.bytes, SIZE_OF_BDADDR))
    {
        return false;
    }

    if(s_bt_avrcp_connection_state != BTAVRCP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_avrcp_key_send(BT_OPID_PLAY, 0, 0, s_bt_avrcp_connected_peer_addr) == BT_SUCCESS)
    {
        return true;
    }
    osiThreadSleep(100);
    if(bt_avrcp_key_send(BT_OPID_PLAY, 2, 0, s_bt_avrcp_connected_peer_addr) == BT_SUCCESS)
    {
        return true;
    }

    return false;
}

bool app_bt_avrcp_pause(void)
{
    uint8 invalid_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    if(0 == memcmp(invalid_addr, s_bt_avrcp_connected_peer_addr.bytes, SIZE_OF_BDADDR))
    {
        return false;
    }

    if(s_bt_avrcp_connection_state != BTAVRCP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_avrcp_key_send(BT_OPID_PAUSE, 0, 0, s_bt_avrcp_connected_peer_addr) == BT_SUCCESS)
    {
        return true;
    }
    osiThreadSleep(100);
    if(bt_avrcp_key_send(BT_OPID_PAUSE, 2, 0, s_bt_avrcp_connected_peer_addr) == BT_SUCCESS)
    {
        return true;
    }

    return false;
}

bool app_bt_avrcp_previ(void)
{
    uint8 invalid_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    if(0 == memcmp(invalid_addr, s_bt_avrcp_connected_peer_addr.bytes, SIZE_OF_BDADDR))
    {
        return false;
    }

    if(s_bt_avrcp_connection_state != BTAVRCP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_avrcp_key_send(BT_OPID_BACKWARD, 0, 0, s_bt_avrcp_connected_peer_addr) == BT_SUCCESS)
    {
        return true;
    }

    osiThreadSleep(100);
    if(bt_avrcp_key_send(BT_OPID_BACKWARD, 2, 0, s_bt_avrcp_connected_peer_addr) == BT_SUCCESS)
    {
        return true;
    }

    return false;
}

bool app_bt_avrcp_next(void)
{
    uint8 invalid_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    if(0 == memcmp(invalid_addr, s_bt_avrcp_connected_peer_addr.bytes, SIZE_OF_BDADDR))
    {
        return false;
    }

    if(s_bt_avrcp_connection_state != BTAVRCP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_avrcp_key_send(BT_OPID_FORWARD, 0, 0, s_bt_avrcp_connected_peer_addr) == BT_SUCCESS)
    {
        return true;
    }
    osiThreadSleep(100);
    if(bt_avrcp_key_send(BT_OPID_FORWARD, 2, 0, s_bt_avrcp_connected_peer_addr) == BT_SUCCESS)
    {
        return true;
    }

    return false;
}

t_AVRCP_PLAYBACK_STATUS app_bt_avrcp_play_state_get(void)
{
    if(s_bt_avrcp_connection_state != BTAVRCP_CONNECTION_STATE_CONNECTED)
    {
        return AVRCP_PLAYBACK_STATUS_ERROR;
    }

    return bt_avrcp_play_state_get();
}

bool app_bt_avrcp_vol_set(uint8 volume)
{
    uint8 invalid_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    UINT8 audio_volume = volume*100/127;
    
    if(0 == memcmp(invalid_addr, s_bt_avrcp_connected_peer_addr.bytes, SIZE_OF_BDADDR))
    {
        return false;
    }

    if(s_bt_avrcp_connection_state != BTAVRCP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_avrcp_volume_set(s_bt_avrcp_connected_peer_addr, volume) != BT_SUCCESS)
    {
        return false;
    }

    if (!audevSetPlayVolume(audio_volume))
    {
        return false;
    }

    return true;
}

int8 app_bt_avrcp_vol_get(void)
{
    uint8 vol;
    bt_status_t status;

    if(s_bt_avrcp_connection_state != BTAVRCP_CONNECTION_STATE_CONNECTED)
    {
        return -1;
    }

    status = bt_avrcp_volume_get(s_bt_avrcp_connected_peer_addr, &vol);

    if(status == BT_SUCCESS)
    {
        return (int8)vol;
    }
    else if(status == BT_UNSUPPORTED)
    {
        return -2;
    }
    else
    {
        return -1;
    }
    
}

