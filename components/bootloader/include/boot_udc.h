/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef __BOOT_UDC_H_
#define __BOOT_UDC_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct boot_udc;
typedef struct boot_udc bootUdc_t;

typedef struct
{
    uint16_t mps;
    union {
        struct
        {
            uint8_t num : 7;
            uint8_t dirin : 1;
        };
        uint8_t address;
    };
} bootUsbEp_t;

struct boot_usb_xfer;

typedef void (*bootUsbXferComp_t)(bootUsbEp_t *ep, struct boot_usb_xfer *xfer);

typedef struct boot_usb_xfer
{
    void *buf;
    uint32_t length;
    uint32_t actual;
    int status;
    uint8_t zlp;
    void *param;
    bootUsbXferComp_t complete;
} bootUsbXfer_t;

/**
 * \brief udc xfer enqueue
 *
 * \param udc   the udc
 * \param ep    the endpoint
 * \param xfer  the usb transfer
 * \return true on success else false
 */
bool bootUdcXferEnqueue(bootUdc_t *udc, bootUsbEp_t *ep, bootUsbXfer_t *xfer);

/**
 * \brief udc xfer dequeue
 *
 * \param udc   the udc
 * \param ep    the endpoint
 * \param xfer  the usb transfer
 */
void bootUdcXferDequeue(bootUdc_t *udc, bootUsbEp_t *ep, bootUsbXfer_t *xfer);

/**
 * \brief open the usb controller and got the controller instance
 *
 * \param name          the controller device name
 * \param continue_rom  do not restart the controller and keep connection from rom code or not
 * \return NULL if fail else the pointer of controller instance
 */
bootUdc_t *bootUdcOpen(uint32_t name, bool continue_rom);

/**
 * \brief close the usb controller and release the instance
 *
 * \param udc   the udc
 */
void bootUdcClose(bootUdc_t *udc);

/**
 * \breif polling the usb controller interrupt and process them
 *        (can only poll 'cause there is no INTERRUPT in bootloader codes)
 * \param udc   the udc
 */
bool bootUdcIsrPoll(bootUdc_t *udc);

/**
 * \brief allocate and init an usb transfer from usb controller
 *
 * \param udc   the udc
 * \param ep    the endpoint
 * \return NULL if fail else the pointer of transfer
 */
bootUsbXfer_t *bootUdcXferAlloc(bootUdc_t *udc, bootUsbEp_t *ep);

/**
 * \brief free an usb transfer
 *
 * \param udc   the udc
 * \param xfer  the usb transfer
 */
void bootUdcXferFree(bootUdc_t *udc, bootUsbXfer_t *xfer);

/**
 * \brief check if the usb controller connected to a host
 *
 * \return true if connected
 */
bool bootUdcConnected(bootUdc_t *udc);

/**
 * \brief rom usb serial in endpoint
 *
 * \param udc   the udc
 * \return NULL if not exist else usb endpoint
 */
bootUsbEp_t *bootUdcRomSerialInEp(bootUdc_t *udc);

/**
 * \brief rom usb serial out endpoint
 *
 * \param udc   the udc
 * \return NULL if not exist else usb endpoint
 */
bootUsbEp_t *bootUdcRomSerialOutEp(bootUdc_t *udc);

#ifdef __cplusplus
}
#endif

#endif