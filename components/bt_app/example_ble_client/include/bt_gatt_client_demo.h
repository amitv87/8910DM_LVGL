/**
 * @file    bt_gatt_client_demo.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020-6-13
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#ifndef _BT_GATT_CLIENT_DEMO_H
#define _BT_GATT_CLIENT_DEMO_H

#define GATT_CLIENT_DEMO_ENABLE 1

#if GATT_CLIENT_DEMO_ENABLE

typedef enum
{
    GATT_CLIENT_LINK_DISCONNECT,
    GATT_CLIENT_LINK_CONNECTING,
    GATT_CLIENT_LINK_CONNECTED,
    GATT_CLIENT_DISCOVER_PRME_SER,
    GATT_CLIENT_DISCOVER_PRME_SER_DONE,
    GATT_CLIENT_READ_INCLUDE,
    GATT_CLIENT_READ_INCLUDE_DONE,
    GATT_CLIENT_READ_CHARA_DEC,
    GATT_CLIENT_READ_CHARA_DEC_DONE,
    GATT_CLIENT_FIND_INFO,
    GATT_CLIENT_FIND_INFO_DONE,
    GATT_CLIENT_SET_CONFIG,
    GATT_CLIENT_SET_CONFIG_DONE,
} GATT_CLIENT_STATE;

/* module id */

#endif

typedef int (*att_client_module_cb)(UINT32 msg, void *param, UINT8 finished, UINT16 acl_handle);

void Gatt_client_demo_function(void);
void example_ble_client_info_get(btgatt_client_callbacks_t **p_client_callbacks);
void Gatt_client_demo_run_statemachine(GATT_CLIENT_STATE next_state);

#endif
