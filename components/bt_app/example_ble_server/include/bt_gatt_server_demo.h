/**
 * @file     bt_gatt_server_demo.h
 * @brief    bt gatt server demo
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2020-09-30
 * @license  Copyright (C) 2020 Unisoc Communications Inc
 */
#ifndef _BT_GATT_SERVER_DEMO_H
#define _BT_GATT_SERVER_DEMO_H

int bt_8910_gatt_process_msg(UINT16 msg_id, void *data);
void bt_8910_save_smp_key(uint8_t *p_data, uint32_t size);

void Gatt_server_demo_function(void);
void bt_8910_ble_SendTpData(uint16 datalen, uint8 *data);
void example_ble_server_info_get(btgatt_server_callbacks_t **p_server_callbacks);

#endif
