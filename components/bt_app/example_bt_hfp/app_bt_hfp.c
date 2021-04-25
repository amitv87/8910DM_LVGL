#include <assert.h>
#include "stdlib.h"
#include "bluetooth/bluetooth.h"
#include "bluetooth/bt_hfp.h"
#include "bt_abs.h"
#include "sci_types.h"
#include "ddb.h"
#include "bt_drv.h"
#include "osi_log.h"
#include "audio_player.h"
#include "audio_types.h"
#include "audio_device.h"
#include "app_bt_hfp.h"

static uint32 s_bt_hfp_call_state = BTHFP_CALL_NO_CALL_IN_PROGRESS;
static bdaddr_t s_bt_hfp_connected_peer_addr = {0};
static bthfp_connection_state_t s_bt_hfp_connection_state = BTHFP_CONNECTION_STATE_DISCONNECTED;
static auBtVoiceWorkMode_t s_audio_work_mode = AU_BT_VOICE_WORK_MODE_NB;
static bthfp_callsetup_state_t s_bt_hfp_call_setup_state = BTHFP_CALLSETUP_NONE;

extern void app_msg_to_at(unsigned int msg_id, char status, void *data_ptr);

void bt_app_hfp_stop(void)
{
    audevStopTone();
    audevBtVoiceStop();

    s_bt_hfp_call_state = BTHFP_CALL_NO_CALL_IN_PROGRESS;
    s_bt_hfp_connection_state = BTHFP_CONNECTION_STATE_DISCONNECTED;
    s_audio_work_mode = AU_BT_VOICE_WORK_MODE_NB;
    s_bt_hfp_call_setup_state = BTHFP_CALLSETUP_NONE;
    memset(&s_bt_hfp_connected_peer_addr, 0, SIZE_OF_BDADDR);
}

static void bt_app_hfp_connection_state_callback(bthfp_connection_state_t state, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_connection_state_callbackstate=%d", state);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }

    if (BTHFP_CONNECTION_STATE_DISCONNECTED == state)
    {
        memset(s_bt_hfp_connected_peer_addr.bytes, 0x00, SIZE_OF_BDADDR);
        audevStopTone();
        audevBtVoiceStop();
        // AT ASSERT !!
     //   app_msg_to_at(ID_STATUS_HFP_DISCONNECT_RES, BT_SUCCESS, NULL);
    }
    if (BTHFP_CONNECTION_STATE_CONNECTED == state)
    {
        if (addr)
        {
            memcpy(s_bt_hfp_connected_peer_addr.bytes, addr->bytes, SIZE_OF_BDADDR);
        }
      //  audevStopTone();
     //   audevBtVoiceStop();
		OSI_LOGI(0, "BT HFP audevBtVoiceSetWorkMode %d line 47", s_audio_work_mode);
     //   audevBtVoiceSetWorkMode(s_audio_work_mode);
     //   audevBtVoiceStart();
        // AT ASSERT !!
     //   app_msg_to_at(ID_STATUS_HFP_CONNECT_RES, BT_SUCCESS, NULL);
    }

    s_bt_hfp_connection_state = state;
    
    return;
}

static void bt_app_hfp_audio_state_callback(bthfp_audio_state_t state, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_audio_state_callbackstate=%d", state);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }

    return;
}

static void bt_app_hfp_volume_change_callback(bthfp_volume_type_t type, int volume, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_volume_change_callbackstate=%d volume=%d", type, volume);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }

    audevSetVoiceVolume(((volume * 100) / 15));

    return;
}

static void bt_app_hfp_call_callback(bthfp_call_state_t state, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_call_callbackstate=%d", state);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }
    if (BTHFP_CALL_CALL_IN_PROGRESS == state)
    {
        audevStopTone();
        audevBtVoiceStop();
		OSI_LOGI(0, "BT HFP audevBtVoiceSetWorkMode %d line 94", s_audio_work_mode);
        audevBtVoiceSetWorkMode(s_audio_work_mode);
        audevBtVoiceStart();
    }
    s_bt_hfp_call_state = state;
    return;
}

static void bt_app_hfp_callsetup_callback(bthfp_callsetup_state_t state, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_callsetup_callbackstate=%d", state);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }
    s_bt_hfp_call_setup_state = state;
    if (BTHFP_CALLSETUP_OUTGOING == state)
    {
        audevStopTone();
        audevBtVoiceStop();
		OSI_LOGI(0, "BT HFP audevBtVoiceSetWorkMode %d line 114", s_audio_work_mode);
        audevBtVoiceSetWorkMode(s_audio_work_mode);
        audevBtVoiceStart();
    }
    return;
}

static void bt_app_hfp_callheld_callback(bthfp_callheld_state_t state, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_callheld_callbackstate=%d", state);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }

    return;
}

static void bt_app_hfp_battery_level_callback(int level, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_battery_level_callback level=%d", level);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }

    return;
}

static void bt_app_hfp_network_signal_callback(int signal_strength, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_network_signal_callbacksignal_strength=%d", signal_strength);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }

    return;
}

static void bt_app_hfp_network_state_callback(bthfp_network_state_t state, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_network_state_callback state=%d", state);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }

    return;
}

static void bt_app_hfp_network_type_callback(bthfp_service_type_t type, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_network_type_callbacktype=%d", type);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }

    return;
}

static void bt_app_hfp_ring_indication_callback(bdaddr_t *addr)
{
    hfp_device_info *tmp = bt_hfp_device_info_get(addr);

    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_ring_indication_callback");
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }
    if (tmp)
    {
   //     OSI_LOGI(0, "[BT][HFP]  AG_FEATURE_IN_BAND_RING=%d", tmp->supportedFeatures & AG_FEATURE_IN_BAND_RING);
    }
 //   if (tmp && (!(tmp->supportedFeatures & AG_FEATURE_IN_BAND_RING)))
    {
        audevStopTone();
        audevBtVoiceStop();
        audevPlayTone(AUDEV_TONE_DIAL, 200);
    }
    return;
}

static void bt_app_hfp_cmd_response_callback(char *response, UINT32 length, bdaddr_t *addr)
{
#if 0
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_cmd_response_callback length=%d", length);
    if(addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }
#endif
    return;
}

static void bt_app_hfp_codec_type_callback(bthfp_codec_type_t type, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT][HFP] enter bt_app_hfp_codec_type_callback type=%d", type);
    if (addr)
    {
        OSI_LOGI(0, "[BT][HFP]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    }

    if (BTHFP_CODEC_TYPE_MSBC == type)
    {
        s_audio_work_mode = AU_BT_VOICE_WORK_MODE_WB;
    }
    else
    {
        s_audio_work_mode = AU_BT_VOICE_WORK_MODE_NB;
    }
    OSI_LOGI(0, "BT HFP change s_audio_work_mode %d", s_audio_work_mode);
    return;
}

bthfp_callbacks_t bthfp_callbacks = {
    bt_app_hfp_connection_state_callback,
    bt_app_hfp_audio_state_callback,
    bt_app_hfp_volume_change_callback,
    bt_app_hfp_call_callback,
    bt_app_hfp_callsetup_callback,
    bt_app_hfp_callheld_callback,
    bt_app_hfp_battery_level_callback,
    bt_app_hfp_network_signal_callback,
    bt_app_hfp_network_state_callback,
    bt_app_hfp_network_type_callback,
    bt_app_hfp_ring_indication_callback,
    bt_app_hfp_cmd_response_callback,
    bt_app_hfp_codec_type_callback};

void app_bt_hfp_init(void)
{
    bt_hfp_callback_register(&bthfp_callbacks);

    s_bt_hfp_call_state = BTHFP_CALL_NO_CALL_IN_PROGRESS;
    s_bt_hfp_connection_state = BTHFP_CONNECTION_STATE_DISCONNECTED;
    s_audio_work_mode = AU_BT_VOICE_WORK_MODE_NB;
    s_bt_hfp_call_setup_state = BTHFP_CALLSETUP_NONE;
    memset(&s_bt_hfp_connected_peer_addr, 0, SIZE_OF_BDADDR);
}

uint8 app_bt_hfp_call_state_get(void)
{
    return s_bt_hfp_call_state;
}

bool app_bt_hfp_connected_check(bdaddr_t* bdAddr)
{
    if(0 != memcmp(bdAddr->bytes, s_bt_hfp_connected_peer_addr.bytes, SIZE_OF_BDADDR))
    {
        return false;
    }
    
    if(s_bt_hfp_connection_state == BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return true;
    }

    return false;
}

bthfp_connection_state_t app_bt_hfp_connection_state_get(void)
{
    return s_bt_hfp_connection_state;
}

bool app_bt_hfp_connect_req(bdaddr_t *pAddr)
{
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_DISCONNECTED)
    {
        return false;
    }
    
    if(bt_hfp_connect(pAddr) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}

bool app_bt_hfp_disconnect_req(bdaddr_t *pAddr)
{
    if(0 != memcmp(pAddr->bytes, s_bt_hfp_connected_peer_addr.bytes, SIZE_OF_BDADDR))
    {
        return false;
    }
    
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }
    
    if(bt_hfp_disconnect(pAddr) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}

bthfp_callsetup_state_t app_bt_hfp_call_setup_state_get(void)
{
    return s_bt_hfp_call_setup_state;
}

bool app_bt_hfp_volume_set(uint8 vol)
{
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_hfp_vol_set(s_bt_hfp_connected_peer_addr, vol) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}

bool app_bt_hfp_call_reject(void)
{
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_hfp_call_reject(&s_bt_hfp_connected_peer_addr) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}

bool app_bt_hfp_call_answer_accept(void)
{
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_hfp_call_answer(&s_bt_hfp_connected_peer_addr, HFP_CALL_ACCEPT) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}

bool app_bt_hfp_call_answer_reject(void)
{
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_hfp_call_answer(&s_bt_hfp_connected_peer_addr, HFP_CALL_REJECT) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}

bool app_bt_hfp_dial_req(UINT8 *phone_number)
{
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_hfp_dial(&s_bt_hfp_connected_peer_addr, phone_number) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}

bool app_bt_hfp_redial_req(void)
{
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_hfp_redial(&s_bt_hfp_connected_peer_addr) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}

bool app_bt_hfp_vr_enable(void)
{
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_hfp_voice_recognition_enable(&s_bt_hfp_connected_peer_addr) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}

bool app_bt_hfp_vr_disable(void)
{
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_hfp_voice_recognition_disable(&s_bt_hfp_connected_peer_addr) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}

bool app_bt_hfp_three_way_call_ctrl(UINT8 CtrlCmd)
{
    if(s_bt_hfp_connection_state != BTHFP_CONNECTION_STATE_CONNECTED)
    {
        return false;
    }

    if(bt_hfp_three_way_call_ctrl(&s_bt_hfp_connected_peer_addr, HFP_CALL_ACCEPT, CtrlCmd) != BT_SUCCESS)
    {
        return false;
    }

    return true;
}
