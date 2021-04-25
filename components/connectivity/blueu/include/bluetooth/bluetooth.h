/**
 * @file    bluetooth.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "bt_types.h"
#include "bt_driver.h"
#include "bt_gatt.h"

#define BT_PROFILE_GATT_ID "gatt"
#define BT_PROFILE_A2DP_ID "a2dp"
#define BT_PROFILE_AVRCP_ID "avrcp"
#define BT_PROFILE_SPP_ID "spp"


typedef enum
{
    BT_STATE_OFF,
    BT_STATE_ON,
} bt_state_t;

typedef enum
{
    BT_ACL_STATE_DISCONNECTED,
    BT_ACL_STATE_CONNECTED,
} bt_acl_state_t;

typedef enum
{
    BT_BOND_STATE_NONE,
    BT_BOND_STATE_BONDING,
    BT_BOND_STATE_BONDED,
} bt_bond_state_t;


typedef enum
{
    BT_PRO_BDNAME = 0x01,
    BT_PRO_BDADDR,
    BT_PRO_UUIDS,
} BT_PRO_TYPE_T;

typedef struct
{
    BT_PRO_TYPE_T type;
    int len;
    void *val;
} bt_property_t;


typedef struct
{
    void (*state_changed_callback) (bt_state_t state);
    void (*acl_state_changed_callback) (bt_acl_state_t state, bdaddr_t *addr);
    void (*bond_state_changed_callback) (bt_bond_state_t state, bdaddr_t *addr);
} bt_callbacks_t;

typedef enum
{
    BT_STATE_DISABLE = 0x00,
    BT_STATE_ENABLE,
    BT_STATE_NOUSE,
} bt_enable_state_t;


typedef struct
{
    int (*init) (const bt_callbacks_t *bt_callbacks);
    int (*enable) (void);
    int (*disable) (void);
    int (*cleanup) (void);
    const void *(*get_profile_interface) (const char *profile_id);
    int (*set_osi_callbacks) (void *args);
    int (*set_property) (bt_property_t *property);
    void (*set_visible)(int is_visiable);
    void (*change_local_name)(char *name);
    void (*change_local_addr)(UINT8 addr[]);
    UINT8 (*get_visable)(void);
    char* (*get_local_name)(void);
    bdaddr_t* (*get_local_addr)(void);
    UINT8 (*get_enable_state)(void);
    int (*inquiry) (UINT8 max_result, UINT8 is_liac,int (*result_cb)(UINT8* data,UINT8 device_num));
} bt_interface_t;

const bt_interface_t *get_bt_interface(void);

#endif
