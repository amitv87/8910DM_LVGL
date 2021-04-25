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

#include "bluetooth/bt_types.h"

/** Avrcp connection state enumeration*/
typedef enum
{
    BTAVRCP_CONNECTION_STATE_DISCONNECTED = 0,     /**< Avrcp connection is disconnected */
    BTAVRCP_CONNECTION_STATE_CONNECTING,	         /**< Avrcp connection is connecting */
    BTAVRCP_CONNECTION_STATE_CONNECTED,                /**< Avrcp connection is connected */
    BTAVRCP_CONNECTION_STATE_DISCONNECTING,         /**< Avrcp connection is disconnecting */
} btavrcp_connection_state_t;

typedef enum
{
    BTAVRCP_SDP_STATE_SUCCESS = 0,
    BTAVRCP_SDP_STATE_FAILED = 1,
} btavrcp_sdp_state_t;

/** Avrcp play status enumeration*/
typedef enum
{
    AVRCP_PLAYBACK_STATUS_STOPPED,                /**< The play is stopped */
    AVRCP_PLAYBACK_STATUS_PLAYING,                 /**< The play is playing */
    AVRCP_PLAYBACK_STATUS_PAUSED,                  /**< The play is paused */
    AVRCP_PLAYBACK_STATUS_FWD_SEEK,              /**< The play is seeking forward */
    AVRCP_PLAYBACK_STATUS_REV_SEEK,               /**< The play is seeking rewind */
    AVRCP_PLAYBACK_STATUS_ERROR = 0xff          /**< Error */
} t_AVRCP_PLAYBACK_STATUS;


/** Avrcp event enumeration*/
typedef enum
{
    AVRCP_EVENT_PLAYBACK_STATUS_CHANGED = (0x01),                       /**< The playback status changed event */
    AVRCP_EVENT_TRACK_CHANGED = (0x02),                                          /**< The track changed event */
    AVRCP_EVENT_TRACK_REACHED_END = (0x03),                                  /**< The track reaches end event */
    AVRCP_EVENT_TRACK_REACHED_START = (0x04),                              /**< The track reaches start event */
    AVRCP_EVENT_PLAYBACK_POS_CHANGED = (0x05),                            /**< The palyback position changed event */
    AVRCP_EVENT_BATT_STATUS_CHANGED = (0x06),                              /**< The battery status changed event */
    AVRCP_EVENT_SYSTEM_STATUS_CHANGED = (0x07),                          /**< The system status changed event */
    AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED = (0x08),  /**< The player application setting changed event */
    AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED = (0x09),             /**< The now playing content changed event */
    AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED = (0x0A),                    /**< The available players changed event */
    AVRCP_EVENT_ADDRESSED_PLAYERS_CHANGED = (0x0B),                   /**< The addressed players changed event */
    AVRCP_EVENT_UIDS_CHANGED = (0x0C),                                             /**< The UIDs changed event */
    AVRCP_EVENT_VOLUME_CHANGED = (0x0D),                                        /**< The volume changed event */
} t_AVRCP_EVENT;

/** Avrcp device information structure */
typedef struct
{
    t_AVRCP_PLAYBACK_STATUS Cur_Play_Status;                            /**< Current play status */
    UINT32 Cur_Song_Pos;                                                            /**< Current song position */
    UINT8 numberofevents;                                                           /**< Number of events */
    UINT8 Registerd_numberofevents;                                           /**< Registered number of events */
    UINT8 supported_events_id[AVRCP_EVENT_VOLUME_CHANGED];  /**< Supported events id  */
    UINT8 numberofattribures;                                                       /**< Number of attributes  */
    UINT8 attribures_index;                                                           /**< Attributes index  */
    UINT8 supported_attributes_type[4];                                        /**< Supported attributes type  */
    UINT8 supported_attributes_value_type[4][4];                           /**< Supported attributes value type  */
    UINT8 UID_Counter;                                                                 /**< UID counter  */
    UINT16 avrcp_cid;                                                                     /**< Avrcp channel id  */
    UINT16 avrcp_browse_cid;                                                        /**< Avrcp browser channel id  */
    UINT8 NumberofItems;                                                             /**< Number of items  */
    UINT8 folder_depth;                                                                 /**< Folder depth */
    UINT8 avrcp_adp_state;                                                           /**< Avrcp adp state */
    bdaddr_t avrcp_addr;
    //UINT8 Cur_Play_pause_key_status;
    UINT8 volume_changed_event_support;                                   /**< Volume changed event support */

    UINT16 profile_ver;                                                                  /**< Profile version */
    UINT16 ct_sdp_feature;                                                            /**< CT sdp feature */
    UINT16 tg_sdp_feature;                                                            /**< TG sdp feature */
    UINT8 sdp_status;                                                                     /**< Sdp status */
    UINT8 need_connect_after_sdp;                                                /**<Need connect after sdp */
} avrcp_device_info_t;


/**
* @addtogroup bt_host
* @{
* @defgroup avrcp_interface Avrcp Fuc Interface
* @{  */

/** Avrcp device callback structure */
typedef struct
{
    void (*connection_state_callback) (btavrcp_connection_state_t state,bdaddr_t *addr);/**<Connection state callback */
    void (*volume_changed_callback) (UINT8 volume, bdaddr_t *addr);                          /**<Volume changed callback */
    void (*get_cur_play_attr_callback) (UINT8 num, void *item);                                   /**<Get current play attribute callback */

    /* sdp_status: see btavrcp_sdp_state_t
     * ct_feature: see btavrcp_sdp_ct_feature_t
     * tg_feature: see btavrcp_sdp_tg_feature_t
     * version: such as 0x0106
     */
    void (*get_sdp_feature_callback)(UINT8 sdp_status, bdaddr_t *addr, UINT32 ct_feature, UINT32 tg_feature, UINT16 version);
} btavrcp_callbacks_t;

//android callback

/*
typedef struct {
    size_t      size;//sizeof(BtRcCallbacks)
    btrc_remote_features_callback               remote_features_cb;
    btrc_get_play_status_callback               get_play_status_cb;
    btrc_list_player_app_attr_callback          list_player_app_attr_cb;
    btrc_list_player_app_values_callback        list_player_app_values_cb;
    btrc_get_player_app_value_callback          get_player_app_value_cb;
    btrc_get_player_app_attrs_text_callback     get_player_app_attrs_text_cb;
    btrc_get_player_app_values_text_callback    get_player_app_values_text_cb;
    btrc_set_player_app_value_callback          set_player_app_value_cb;
    btrc_get_element_attr_callback              get_element_attr_cb;
    btrc_register_notification_callback         register_notification_cb;
    btrc_volume_change_callback                 volume_change_cb;
    btrc_passthrough_cmd_callback               passthrough_cmd_cb;
} btrc_callbacks_t;
*/


/** Avrcp interface structure */
typedef struct
{
    bt_status_t (*init) (btavrcp_callbacks_t *callbacks); /**<Avrcp interface initialization function */
    bt_status_t (*connect) (bdaddr_t *addr);               /**<Avrcp connect function */
    bt_status_t (*disconnect) (bdaddr_t *addr);           /**<Avrcp disconnect function */
    avrcp_device_info_t* (*get_devices)(void);              /**<Avrcp get devices function */
    UINT8 (*get_max_devices_num)(void);                 /**<Avrcp get maximum device number function */
    bt_status_t (*send_key)(UINT8 key,UINT8 press, UINT8 param, bdaddr_t addr);/**<Avrcp send key function */
    bt_status_t (*sync_volume)( bdaddr_t addr,UINT8 volume);/**<Avrcp synchronize volume function */
    UINT8 (*get_volume)(void);/**<Avrcp get volume function */
    UINT8 (*get_play_state)(void);/**<Avrcp get play state function */
    UINT8 (*get_cur_play_info)(void);/**<Avrcp get current play information function */
    int (*is_support_volume_handling)(bdaddr_t addr);/**<Avrcp is support volume handling function */
    void (*reset_volume_change_state)(bdaddr_t addr);/**<Avrcp reset volume change state function */
    bt_status_t (*get_sdp_feature)(bdaddr_t *addr);
    bt_status_t (*sync_absolute_volume)(bdaddr_t addr, UINT8 volume);
    void  (*notify_volume_change)(bdaddr_t addr);
    bt_status_t (*send_key_volume_change)(bdaddr_t addr, UINT8 key);
} btavrcp_interface_t;
/** @} */
/** @} */

#endif

