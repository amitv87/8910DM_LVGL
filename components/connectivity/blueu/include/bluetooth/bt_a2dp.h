/**
 * @file    bt_a2dp.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_A2DP_H__
#define __BT_A2DP_H__

#include "bt_types.h"

typedef enum
{
    BTA2DP_CONNECTION_STATE_DISCONNECTED = 0,
    BTA2DP_CONNECTION_STATE_CONNECTING,
    BTA2DP_CONNECTION_STATE_CONNECTED,
    BTA2DP_CONNECTION_STATE_DISCONNECTING,
} bta2dp_connection_state_t;


typedef enum
{
    BTA2DP_AUDIO_STATE_REMOTE_SUSPEND = 0,
    BTA2DP_AUDIO_STATE_STOPPED,
    BTA2DP_AUDIO_STATE_START,
} bta2dp_audio_state_t;


typedef struct
{
    void (*connection_state_callback) (bta2dp_connection_state_t state, bdaddr_t *addr);
    void (*audio_state_callback) (bta2dp_audio_state_t state, bdaddr_t *addr);
    void (*audio_config_callback) (UINT32 sample_rate, UINT8 channel_count, bdaddr_t *addr);
    void (*audio_stream_callback) (UINT8 codec_type, UINT8 *data, UINT32 length);
} bta2dp_callbacks_t;


typedef struct
{
    bt_status_t (*init) (bta2dp_callbacks_t *callbacks);
    bt_status_t (*connect) (bdaddr_t *addr);
    bt_status_t (*disconnect) (bdaddr_t *addr);
    void (*cleanup) (void);
} bta2dp_interface_t;



#endif
