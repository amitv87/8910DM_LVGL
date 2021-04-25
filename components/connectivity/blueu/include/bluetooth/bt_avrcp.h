/**
 * @file    bt_avrcp.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_AVRCP_H__
#define __BT_AVRCP_H__

#include "bt_types.h"

typedef enum
{
    BTAVRCP_CONNECTION_STATE_DISCONNECTED = 0,
    BTAVRCP_CONNECTION_STATE_CONNECTING,
    BTAVRCP_CONNECTION_STATE_CONNECTED,
    BTAVRCP_CONNECTION_STATE_DISCONNECTING,
} btavrcp_connection_state_t;

typedef struct
{
    void (*connection_state_callback) (btavrcp_connection_state_t state,bdaddr_t *addr);
    void (*volume_changed_callback) (UINT8 volume, bdaddr_t *addr);
} btavrcp_callbacks_t;

//typedef enum
//{
//    BTAVRCP_MUSIC_PAUSE = 0x00,
//    BTAVRCP_MUSIC_PLAYING,
//    BTAVRCP_MUSIC_NOUSE,
//}btavrcp_music_state_t;

typedef struct
{
    bt_status_t (*init) (btavrcp_callbacks_t *callbacks);
    bt_status_t (*connect) (bdaddr_t *addr);
    bt_status_t (*disconnect) (bdaddr_t *addr);
    bt_status_t (*send_key)(UINT8 key,UINT8 press, UINT8 param, bdaddr_t addr);
    bt_status_t (*sync_volume)( bdaddr_t addr,UINT8 volume);
    UINT8 (*get_volume)(void);
    UINT8 (*get_play_state)(void);
    UINT8 (*get_cur_play_info)(void);
} btavrcp_interface_t;

#endif

