/**
 * @file    bt_spp.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_SPP_H__
#define __BT_SPP_H__

#include "bt_types.h"

#define SPP_FLOW_STOP 0
#define SPP_FLOW_GO 1

typedef struct
{
    bdaddr_t addr;
    UINT8 port;
    UINT8 spp_state;
} RDABT_SPP_ADP_INFO;


typedef struct
{
    data_buf_t *data;
    UINT16 data_length;
    UINT16 spp_port;
} RDABT_SPP_SEND_DATA_STRUCT;

typedef enum
{
    BTSPP_CONNECTION_STATE_DISCONNECTED = 0,
    BTSPP_CONNECTION_STATE_CONNECTING,
    BTSPP_CONNECTION_STATE_CONNECTED,
    BTSPP_CONNECTION_STATE_DISCONNECTING,
} btspp_connection_state_t;

typedef struct
{
    void (*connection_state_callback) (btspp_connection_state_t state,bdaddr_t *addr);
    UINT8 (*data_recv_callback) (RDABT_SPP_SEND_DATA_STRUCT *data);
    UINT8 (*flowctrl_ind_callback)(UINT32 result);
} btspp_callbacks_t;


typedef struct
{
    bt_status_t (*init) (btspp_callbacks_t *callbacks);
    bt_status_t (*connect) (bdaddr_t *addr);
    bt_status_t (*disconnect) (bdaddr_t *addr);
} btspp_interface_t;

const btspp_interface_t *get_btspp_interface(void);


#endif


