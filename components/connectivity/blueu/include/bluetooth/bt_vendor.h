/**
 * @file    bt_vendor.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_VENDOR_H__
#define __BT_VENDOR_H__

#include "bt_types.h"
#include "bt_config.h"

typedef enum
{
    BT_VND_OP_HCI_DEV_CLOSE,
    BT_VND_OP_HCI_DEV_OPEN,
    BT_VND_OP_HCI_DEV_CONFIG,
} bt_vendor_opcode_t;

typedef enum
{
    BT_VND_OP_RESULT_SUCCESS,
    BT_VND_OP_RESULT_FAIL,
} bt_vendor_op_result_t;

typedef void (*cmd_callback_t) (void *p_mem);

typedef struct
{
    void* (*malloc_cb) (int size);
    void (*free_cb) (void *p_buf);
    void (*cmd_xmit_cb) (hci_data_t *pdu, cmd_callback_t cb);
    void (*recv_hci_buf) (hci_data_t *hci_data);
    int (*get_local_address) (bdaddr_t *address);
    void (*op_cb) (bt_vendor_opcode_t opcode, bt_vendor_op_result_t result, void *param);
} bt_vendor_callback_t;


typedef struct
{
    int (*init) (const bt_vendor_callback_t *p_cb);
    int (*op) (bt_vendor_opcode_t opcode, void *param);
    hci_data_t* (*get_hci_buf) (UINT8 type, UINT16 handle, UINT16 length);
    int (*send_hci_buf) (hci_data_t *hci_data);
    int (*hci_buf_free) (hci_data_t *hci_data);
    void (*cleanup) (void);
} bt_vendor_interface_t;

const bt_vendor_interface_t *get_bt_vendor_interface(void);

#endif
