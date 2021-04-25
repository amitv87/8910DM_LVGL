/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _BT_HCI_DEV_H_
#define _BT_HCI_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief opaque data struct for hci device
 */
typedef struct bt_hci_dev bt_hci_dev_t;

/**
 * \brief hci device rx data available callback
 *
 * \param d hci device instance
 * \param param callback context
 */
typedef void (*bt_hci_dev_rx_avail_cb_t)(bt_hci_dev_t *d, void *param);

/**
 * \brief open hci device instance
 *
 * \param cb hci device rx available callback
 * \param cb_ctx callback context
 * \return
 *      - hci device instance
 *      - NULL on error
 */
bt_hci_dev_t *bt_hci_dev_open(bt_hci_dev_rx_avail_cb_t cb, void *cb_ctx);

/**
 * \brief close hci device instance
 *
 * \param d hci device instance
 */
void bt_hci_dev_close(bt_hci_dev_t *d);

/**
 * \brief read data from hci device
 *
 * \param d hci device instance
 * \param data memory for read
 * \param size memory size
 * \return real read size
 */
int bt_hci_dev_read(bt_hci_dev_t *d, void *data, unsigned size);

/**
 * \brief write data to hci device
 *
 * \param d hci device instance
 * \param data data for write
 * \param size write size
 * \return real written size
 */
int bt_hci_dev_write(bt_hci_dev_t *d, const void *data, unsigned size);

/**
 * \brief set hci uart baud rate
 *
 * \param baud baud rate
 */
void bt_hci_dev_set_baud(unsigned baud);

#ifdef __cplusplus
}
#endif
#endif
