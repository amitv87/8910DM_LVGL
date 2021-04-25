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

#include "bluetooth/bt_types.h"


/**
 * @brief BT DRIVER Data Structures
 * @defgroup bt_driver_api_data BT DRIVER Data Structures
 * @ingroup bt_api
 * @{
 */
/**
 * @brief bt hci_rx thread priority config
 */
#define BT_DRIVER_RX_PRIORITY  13

/**
 * @enum bt_drv_state_t
 * @brief Enumeration with possible bt driver state.
 *
 */
typedef enum
{
    BT_DRV_STATE_OFF,
    BT_DRV_STATE_ON,
} bt_drv_state_t;

/**
* @addtogroup bt_host
* @{
* @defgroup driver_interface Driver Fuc Interface
* @{  */
/** callback function*/
typedef struct
{
    void* (*malloc_cb) (int size);
    void (*free_cb) (void *p_buf);
    void (*recv_cb) (unsigned char type, unsigned char *data, unsigned int length);
    void (*state_changed_callback)(bt_drv_state_t state);
} bt_driver_callback_t;

/** driver interface function*/
typedef struct
{
    int (*init) (const bt_driver_callback_t *p_cb);
    void (*send) (unsigned char type, void *p_buf, unsigned int length);
    void (*cleanup) (void);
} bt_driver_interface_t;

/**
 * @brief get bt driver interface
 * @return bt driver interface
 */
const bt_driver_interface_t *get_bt_driver_interface(void);
/** @} */
/** @} */

#endif
