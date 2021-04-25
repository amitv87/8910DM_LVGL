/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _USB_ETHER_H_
#define _USB_ETHER_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include <stdbool.h>
#include <stdint.h>
#include "drv_ether.h"
#include "drv_if_ether.h"
#include "usb/usb_device.h"

typedef struct usb_ether usbEther_t;

/**
 * usbether packet layout
 *
 * |+------------------------------------------------+|
 * | usbe packet                                      |
 * |+-------------------------------+----------------+|
 * | usbe specific header( if have) | ethernet packet |
 * |+-------------------------------+----------------+|
 */
typedef struct
{
    int (*netup_cb)(usbEther_t *usbe);
    void (*netdown_cb)(usbEther_t *usbe);

    /**
     * \brief wrap the ether packet, mostly add a header
     * \param[in]   usbe      the usb ether instance
     * \param[in]   req       ether packet with valid data
     * \param[out]  raw_buf   buffer for full usbe packet, reserved space
     *                        enough for usbe header
     */
    void (*wrap)(usbEther_t *usbe, const drvEthReq_t *req, uint8_t *raw_buf);

    /**
     * \brief unwrap the usb packet, mostly remove a header
     * \param[in]   usbe        the usb ether instance
     * \param[in]   raw_buf     raw data buffer form with full usbe packet,
     *                          include usbe header and ethernet data payload
     * \param[in]   valid_size  valid data size in raw buffer
     * \param[out]  req         set proper payload data address and payload size
     * \return
     *      - true if succeed
     *      - false if the usbe packet not valid
     */
    bool (*unwrap)(usbEther_t *usbe, uint8_t *raw_buf, uint32_t valid_size, drvEthReq_t *req);

    /**
     * \brief pre-set immutable usbe packet head members for quick wrap
     *        driver assure buffer space is enough for usbe packet header
     * \param[in]   usbe    the usb ether instance
     * \param[out]  raw_buf data buffer for full packet
     */
    void (*wrap_pre)(usbEther_t *usbe, uint8_t *raw_buf);
} usbEtherDevOps_t;

typedef struct
{
    usbEtherDevOps_t ops;
    uint32_t name;
    udc_t *udc;
    usbEp_t *tx_ep;
    usbEp_t *rx_ep;
    uint32_t payload_max_size;
    uint32_t dev_proto_head_len;
    const uint8_t *host_mac;
    const uint8_t *dev_mac;
    void *dev_priv;
} usbEtherConfig_t;

/**
 * \brief create usb ether device
 *
 * \param config    usb ether configurations
 * \return
 *      - usb ether instance or NULL
 */
usbEther_t *usbEtherCreate(const usbEtherConfig_t *config);

/**
 * \brief get ether instance from usb ether
 *
 * \param usb_ether usb ether
 * \return
 *      - ether instance or NULL if `usb_ether` is NULL
 */
drvEther_t *usbEtherGetEther(usbEther_t *usb_ether);

/**
 * \brief get usb ether config
 *
 * \param usb_ether usb ether
 * \return
 *      - ether config or NULL if `usb_ether` is NULL
 */
const usbEtherConfig_t *usbEtherConfig(const usbEther_t *usb_ether);

/**
 * \brief usb ether device is ready to work, start usb ether
 *
 * \param usb_ether usb ether
 * \return
 *      - true on succeed else false
 */
bool usbEtherStart(usbEther_t *usb_ether);

/**
 * \brief usb ether device stop, and stop the usb ether
 *
 * \param usb_ether usb ether
 */
void usbEtherStop(usbEther_t *usb_ether);

/**
 * \brief destroy the usb ether
 *
 * \param usb_ether usb ether
 */
void usbEtherDestroy(usbEther_t *usb_ether);

OSI_EXTERN_C_END

#endif