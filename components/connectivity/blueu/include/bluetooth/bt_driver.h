/**
 * @file    bt_driver.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_DRIVER_H__
#define __BT_DRIVER_H__

#include "bt_types.h"

typedef enum
{
    BT_DRV_STATE_OFF,
    BT_DRV_STATE_ON,
} bt_drv_state_t;


typedef struct
{
    void (*malloc_cb) (int size);
    void (*free_cb) (void *p_buf);
    void (*recv_cb) (unsigned char type, void *p_buf, unsigned int length);
    void (*state_changed_callback)(bt_drv_state_t state);
} bt_driver_callback_t;


typedef struct
{
    int (*init) (const bt_driver_callback_t *p_cb);
    void (*send) (unsigned char type, unsigned int handle, unsigned int length, void *p_buf);
    void (*cleanup) (void);
} bt_driver_interface_t;


const bt_driver_interface_t *get_bt_driver_interface(void);

#endif
