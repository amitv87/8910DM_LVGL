/**
 * @file     bt_abs.h
 * @brief    bt interface adaptor
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2020-09-30
 * @license  Copyright (C) 2020 Unisoc Communications Inc
 */
#ifndef BT_ABS_H
#define BT_ABS_H

#include <assert.h>
#include "bluetooth/bt_types.h"
#include "bluetooth/bluetooth.h"
#include "bluetooth/bt_config.h"
#include "bluetooth/bt_avrcp.h"
#include "bluetooth/bt_a2dp.h"
#include "bluetooth/bt_gatt.h"
#include "bluetooth/bt_spp.h"
#include "bluetooth/bt_hfp.h"
#include "sci_types.h"

/********************************All structures ***********************/
typedef struct _BT_ADDRESS
{
    uint8 addr[SIZE_OF_BDADDR];
} BT_ADDRESS;

typedef struct hfp_info
{
    bdaddr_t address;
    UINT8 inuse;
    UINT8 scm_state;         // Current State of the Service link to the peer
    UINT8 operation_pending; // handled in OK responsed by AG
    UINT8 call_handling;     //
    UINT8 serverChannel;
    UINT8 local_serverChannel;
    UINT16 versionInfo;
    UINT8 call_state;
    UINT8 can_redial;
    UINT16 max_memory;

    UINT8 HFP_Browse_Pending;
    UINT8 service_ind_order;
    UINT8 signal_ind_order;
    UINT8 battchg_ind_order;
    UINT8 call_setup_ind_order;
    UINT8 call_hold_ind_order;
    UINT8 call_ind_order;
    UINT8 service_ind;    //service availability indicator. 0/1: Home/Roam network not/available
    UINT8 call_ind;       //call status indicator: 0:no call active, 1:a call is active
    UINT8 call_setup_ind; //call setup indicator  // 0: not currently in call setup, 1: an incoming call process ongoing
    // 2: an outgoing call setup process ongoing, 3: remote party being alerted in an outgoing call
    UINT8 call_hold_ind;

    UINT16 dlci;
    UINT8 rf_role;
    UINT8 remote_feature_status; //it's not from AT+BRSF,but it's same as the values of the Bits 0 to 4 of the AT+BRSF.
    UINT32 supportedFeatures;    // Features supported by the peer
    UINT8 *pending_data;         //UINT8* pending_data; was used to save the AT command, so 50 bytes maybe enough. zhujianguo.
    UINT8 pending_data_len;
    UINT16 acl_handle;
    UINT8 HFP_TxOK;
    UINT8 slc_setup_timer_handle;
    UINT8 ring_timer;
    UINT8 active_profile;
    UINT8 num_Phone[MAX_PHONE_NUMBER];
    UINT8 num_length;
    UINT8 net_operator[16];
    UINT8 ops_length;
    UINT8 indicators_status[7];
    UINT8 flags;
    UINT8 current_call_setup;
    UINT8 current_call_status;
    UINT8 current_have_hold_call;
    UINT8 flag_chup;
    UINT8 current_pbap;
    UINT8 current_pbap_which;
    UINT8 set_code;
    UINT8 device_index_flag;
    UINT8 bvra_flag;
} hfp_device_info;

/************************All function declarations*******************************/
//Classic
void bt_core_init(const bt_callbacks_t *bt_callbacks, const bt_config_t *bt_config, bt_storage_t *bt_storage);

bt_status_t bt_start(void);

bt_status_t bt_stop(void);

//bt_status_t bt_reopen(void);

bt_state_t bt_state_get(void);

bt_status_t bt_device_search_start(uint8 max_result, uint8 is_liac);

bt_status_t bt_device_search_cancel(void);

bt_status_t bt_device_pair(bdaddr_t *addr);

bt_status_t bt_pair_pin_code_input(bdaddr_t *addr, uint8 accept, uint8 pin_len, uint8 *pin_code);

bt_status_t bt_device_pair_cancel(bdaddr_t *addr);

bt_status_t bt_inquiry_scan_param_set(UINT16 interval, UINT16 window);

bt_status_t bt_page_scan_param_set(UINT16 interval, UINT16 window);

bt_status_t bt_paired_device_remove(bdaddr_t *addr);

void bt_visibility_set(uint8 visible);

uint8 bt_visibility_get(void);

//bt_status_t bt_local_name_set(const uint8 *name);

bt_status_t bt_local_name_change(const uint8 *name);

bt_status_t bt_local_name_get(char *name);

//bt_status_t bt_public_addr_set(uint8 *addr);

bt_status_t bt_public_addr_get(bdaddr_t *addr);

//HFP
bt_status_t bt_hfp_callback_register(bthfp_callbacks_t *callbacks);

bt_status_t bt_hfp_connect(bdaddr_t *pAddr);

bt_status_t bt_hfp_disconnect(bdaddr_t *pAddr);

UINT8 bt_hfp_call_status_get(void);

bt_status_t bt_hfp_dial(bdaddr_t *pAddr, UINT8 *phone_number);

bt_status_t bt_hfp_redial(bdaddr_t *pAddr);

bt_status_t bt_hfp_call_answer(bdaddr_t *pAddr, UINT8 call_status);

bt_status_t bt_hfp_call_reject(bdaddr_t *pAddr);

bt_status_t bt_hfp_vol_set(bdaddr_t addr, UINT8 vol);

bt_status_t bt_hfp_voice_recognition_enable(bdaddr_t *addr);

bt_status_t bt_hfp_voice_recognition_disable(bdaddr_t *addr);

bt_status_t bt_hfp_three_way_call_ctrl(bdaddr_t *pAddr, UINT8 call_type, UINT8 param);

hfp_device_info *bt_hfp_device_info_get(bdaddr_t *addr);

bt_status_t bt_hfp_disconnect_audio(void);

//Avrcp
bt_status_t bt_avrcp_callback_register(btavrcp_callbacks_t *callbacks);

bt_status_t bt_avrcp_volume_get(bdaddr_t addr, uint8 *vol);

bt_status_t bt_avrcp_key_send(uint8 key, uint8 press, uint8 param, bdaddr_t addr);

t_AVRCP_PLAYBACK_STATUS bt_avrcp_play_state_get(void);

bt_status_t bt_avrcp_volume_set(bdaddr_t addr, UINT8 volume);

//Spp
bt_status_t bt_spp_callback_register(btspp_callbacks_t *callbacks);

bt_status_t bt_spp_connect(bdaddr_t *addr);

bt_status_t bt_spp_disconnect(bdaddr_t *addr);

bt_status_t bt_spp_data_send(bdaddr_t *addr, uint8 *data, uint16 length);

//BLE
uint8 ble_adv_state_get(void);

bt_status_t ble_adv_enable(void);

bt_status_t ble_adv_disable(void);

bt_status_t ble_random_addr_set(bdaddr_t *addr);

bt_status_t ble_random_addr_get(bdaddr_t *addr);

uint8 ble_white_list_add(bdaddr_t *addr, UINT8 addr_type);

bt_status_t ble_white_list_remove(bdaddr_t *addr, UINT8 addr_type);

bt_status_t ble_white_list_clear(void);

bt_status_t ble_local_name_set(const char *name, uint8 len);

char *ble_local_name_get(void);

bt_status_t ble_connect(bdaddr_t *address, uint8 module, uint8 addr_type);

bt_status_t ble_connect_ext(bdaddr_t *address, uint8 module, uint8 addr_type, uint8 own_addr_type);

bt_status_t ble_disconnect(uint16 conn_handle);

bt_status_t ble_connect_para_update(gatt_connect_param_t *param);

bt_status_t ble_adv_param_set(gatt_adv_param_t *adv_param, uint8 ownAddrType);

bt_status_t ble_adv_data_set(uint8 *data, uint32 len);

//set iBeacon data
//uuid is 16 bytes, such as 0xb9407f30f5f8466eaff925556b57fe6d;
//major is 2 bytes, such as 0x0001;
//minor is 2 bytes, such as 0x0002;
bt_status_t ble_beacon_data_set(UINT8 uuid[16], UINT8 major[2], UINT8 minor[2]);

bt_status_t ble_scan_rsp_data_set(uint8 *data, uint8 length);

bt_status_t ble_scan_param_set(gatt_scan_param_t *param, uint8 own_addr_type);

bt_status_t ble_scan_enable(void);

bt_status_t ble_scan_disable(void);

bt_status_t ble_send_notification(gatt_le_data_info_t *data_info);

bt_status_t ble_send_indication(gatt_le_data_info_t *data_info);

bt_status_t ble_add_service(gatt_element_t *pElementArray, UINT16 numElement);

bt_status_t ble_gc_init(void);

bt_status_t ble_client_init(UINT16 module, UINT16 uuid, ap_module_cb func);

bt_status_t ble_client_init(UINT16 module, UINT16 uuid, ap_module_cb func);

bt_status_t ble_client_discover_all_primary_service(UINT16 module, UINT16 acl_handle);

bt_status_t ble_client_discover_primary_service_by_uuid(UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 acl_handle);

bt_status_t ble_client_discover_all_characteristic(UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);

bt_status_t ble_client_read_char_value_by_handle(UINT16 module, UINT16 att_handle, UINT8 is_long_att, UINT16 offset, UINT16 acl_handle);

bt_status_t ble_client_read_char_value_by_uuid(UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);

bt_status_t ble_client_read_multiple_char_value(UINT16 module, UINT8 *handle, UINT8 length, UINT16 acl_handle);

bt_status_t ble_client_write_char_value(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle, UINT16 offset, UINT8 islong);

bt_status_t ble_client_write_char_value_without_rsp(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);

bt_status_t ble_client_write_char_value_signed(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);

bt_status_t ble_client_write_char_value_reliable(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);

bt_status_t ble_client_find_include_service(UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);

bt_status_t ble_client_get_char_descriptor(UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);

bt_status_t ble_client_read_char_descriptor(UINT16 module, UINT16 att_handle, UINT8 is_long_att, UINT16 acl_handle);

bt_status_t ble_client_write_char_descriptor(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);

bt_status_t ble_client_discover_all_primary_service(UINT16 module, UINT16 acl_handle);

bt_status_t ble_client_discover_primary_service_by_uuid(UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 acl_handle);

bt_status_t ble_client_discover_all_characteristic(UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);

bt_status_t ble_client_read_char_value_by_uuid(UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);

bt_status_t ble_client_read_multiple_char_value(UINT16 module, UINT8 *handle, UINT8 length, UINT16 acl_handle);

bt_status_t ble_client_write_char_value_without_rsp(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);

bt_status_t ble_client_write_char_value_signed(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);

bt_status_t ble_client_write_char_value_reliable(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);

bt_status_t ble_client_find_include_service(UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);

bt_status_t ble_client_get_char_descriptor(UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle);

bt_status_t ble_client_read_char_descriptor(UINT16 module, UINT16 att_handle, UINT8 is_long_att, UINT16 acl_handle);

bt_status_t ble_client_write_char_descriptor(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle);

bt_status_t ble_smp_pair_mode_set(uint8 pair_mode, uint8 io_cap);

bt_status_t ble_smp_slave_sec_request();

bt_status_t ble_smp_delete_pair_device(bdaddr_t address);

bt_status_t ble_smp_remove_device_from_resolving_list(bdaddr_t address);

bt_status_t ble_smp_resolving_list_enable(UINT8 privacy_enable);

void *ble_smp_rpa_calc(bdaddr_t *rpa);
void *ble_smp_ida_calc(bdaddr_t *ida);

UINT32 ble_smp_get_pair_info(void **buf);

void ble_smp_restore_pair_info(void *buf);

bt_status_t bt_a2dp_callback_register(bta2dp_callbacks_t *callbacks);
bt_status_t bt_a2dp_connect(bdaddr_t *pAddr);
bt_status_t bt_a2dp_disconnect(bdaddr_t *pAddr);

#endif /* BT_ABS_H */
