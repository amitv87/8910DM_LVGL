/**
 * @file    bt_hfp.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_HFP_H__
#define __BT_HFP_H__

#include "bt_types.h"

typedef enum
{
    BTHFP_CONNECTION_STATE_DISCONNECTED = 0,
    BTHFP_CONNECTION_STATE_CONNECTING,
    BTHFP_CONNECTION_STATE_CONNECTED,
    BTHFP_CONNECTION_STATE_DISCONNECTING,
} bthfp_connection_state_t;

typedef enum
{
    BTHFP_AUDIO_STATE_DISCONNECTED = 0,
    BTHFP_AUDIO_STATE_CONNECTING,
    BTHFP_AUDIO_STATE_CONNECTED,
    BTHFP_AUDIO_STATE_DISCONNECTING
} bthfp_audio_state_t;

typedef enum
{
    BTHFP_VOLUME_TYPE_SPK = 0,
    BTHFP_VOLUME_TYPE_MIC
} bthfp_volume_type_t;

typedef enum
{
    BTHFP_CODEC_TYPE_CVSD = 1,
    BTHFP_CODEC_TYPE_MSBC
} bthfp_codec_type_t;

typedef enum
{
    BTHFP_SERVICE_TYPE_HOME = 0,
    BTHFP_SERVICE_TYPE_ROAMING,
} bthfp_service_type_t;

typedef enum
{
    BTHFP_NETWORK_STATE_NOT_AVAILABLE = 0,
    BTHFP_NETWORK_STATE_AVAILABLE,
} bthfp_network_state_t;

typedef enum
{
    BTHFP_CALL_NO_CALL_IN_PROGRESS = 0,
    BTHFP_CALL_CALL_IN_PROGRESS,
} bthfp_call_state_t;

typedef enum
{
    BTHFP_CALLSETUP_NONE = 0,
    BTHFP_CALLSETUP_INCOMING,
    BTHFP_CALLSETUP_OUTGOING,
    BTHFP_CALLSETUP_ALERTING
} bthfp_callsetup_state_t;

typedef enum
{
    BTHFP_CALLHELD_NONE = 0,
    BTHFP_CALLHELD_HOLD_AND_ACTIVE,
    BTHFP_CALLHELD_HOLD,
} bthfp_callheld_state_t;

typedef struct
{
    void (*connection_state_callback) (bthfp_connection_state_t state, bdaddr_t *addr);
    void (*audio_state_callback) (bthfp_audio_state_t state, bdaddr_t *addr);
    void (*volume_change_callback) (bthfp_volume_type_t type, int volume, bdaddr_t *addr);
    void (*call_callback) (bthfp_call_state_t state, bdaddr_t *addr);
    void (*callsetup_callback) (bthfp_callsetup_state_t state, bdaddr_t *addr);
    void (*callheld_callback) (bthfp_callheld_state_t state, bdaddr_t *addr);
    void (*battery_level_callback) (int level, bdaddr_t *addr);
    void (*network_signal_callback) (int signal_strength, bdaddr_t *addr);
    void (*network_state_callback) (bthfp_network_state_t state, bdaddr_t *addr);
    void (*network_type_callback) (bthfp_service_type_t type, bdaddr_t *addr);
    void (*ring_indication_callback) (bdaddr_t *addr);
    void (*cmd_response_callback) (char *response, UINT32 length, bdaddr_t *addr);
    void (*codec_type_callback) (bthfp_codec_type_t type, bdaddr_t *addr);
} bthfp_callbacks_t;


typedef struct
{
    bt_status_t (*init) (bthfp_callbacks_t *callbacks);
    bt_status_t (*connect) (bdaddr_t *addr);
    bt_status_t (*disconnect) (bdaddr_t *addr);
    void (*cleanup) (void);
} bthfp_interface_t;


#endif
