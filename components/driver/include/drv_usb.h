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

#ifndef _DRV_USB_H_
#define _DRV_USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <osi_api.h>
#include "quec_proj_config.h"

#define USB_VID_SPRD (0x1782)
#define USB_VID_RDA_DOWNLOAD (0x0525)

#define USB_PID_RDA_DOWNLOAD (0xa4a7)
#define USB_PID_SERIALS (0x4d10)
#define USB_PID_RNDIS_AND_SERIALS (0x4d11)
#define USB_PID_NPI_SERIAL (0x4d12)
#define USB_PID_ECM_ACM_SERIAL (0x4d15)

#ifdef CONFIG_QUEC_PROJECT_FEATURE
#define QUEC_USB_VID_DEFAULT (0x2c7c)
#define QUEC_USB_PID_DEFAULT (0x0901)
#endif

enum usb_detect_mode
{
    USB_DETMODE_CHARGER = 0,
    USB_DETMODE_AON = 1,
};

typedef enum
{
    DRV_USB_CHARGER_ONLY = 0,
    DRV_USB_RDA_ROM_SERIAL,                         // [USL0], (0525, a4a7)
    DRV_USB_SERIALS,                                // [USL[0-7]], (1782, 4d10)
    DRV_USB_RNDIS_AND_SERIALS,                      // [RNDIS, USL[0-7]], (1782, 4d11)
    DRV_USB_NPI_SERIAL,                             // [USL1], (1782, 4d12)
    DRV_USB_ECM_AND_SERIALS,                        // [ECM, USL[0-7]], (1782, 4d11)
    DRV_USB_ECM_ACM_SERIALS,                        // [ECM, USL0, USL4, ACM0, ACM1, ACM2, ACM3], (1782, 4d15)
    DRV_USB_SINGLE_INTF_START,                      // single interface work mode start
    DRV_USB_RNDIS_ONLY = DRV_USB_SINGLE_INTF_START, // (1782, 4d11)
    DRV_USB_ECM_ONLY,                               // (1782, 4d11)
    DRV_USB_SINGLE_INTF_END = DRV_USB_ECM_ONLY,     // single interface work mode end
    DRV_USB_WORK_MODE_NUM,
} drvUsbWorkMode_t;

#ifdef CONFIG_QUEC_PROJECT_FEATURE
typedef struct
{
    uint32_t usb_detect_time;
    uint32_t charging_on_time;
    uint32_t charging_off_time;   
} quec_usb_setting_s;
#endif

/**
 * @brief Init usb module, assert if fail
 */
void drvUsbInit();

/**
 * @brief Get usb work queue
 *
 * This work queue should only be used by USB class drivers.
 *
 * @return usb work queue
 */
osiWorkQueue_t *drvUsbWorkQueue();

/**
 * @brief Set USB work mode
 *
 * If it is the same as current mode, do nothing.
 * Else if USB is working, stop it thereafter reset mode.
 *
 * @param mode  usb work mode
 * @return
 *      - true  success
 *      - false not support or fail
 */
bool drvUsbSetWorkMode(drvUsbWorkMode_t mode);

/**
 * @brief Get current USB work mode
 *
 * @return  current work mode
 */
drvUsbWorkMode_t drvUsbCurrentMode();

/**
 * @brief the Usb is connected to host or not
 * @return
 *      - true  connected
 *      - flase not connect
 */
bool drvUsbIsConnected();

/**
 * @brief Enable usb feature
 *
 * @param debounce  debounce time in milliseconds
 */
void drvUsbEnable(uint32_t debounce);

/**
 * @brief Disable usb feature
 */
void drvUsbDisable();

/**
 * @brief Check usb is enabled or not
 *
 * @return  enabled if true else false
 */
bool drvUsbIsEnabled();

/**
 * @brief wakeup the host
 *
 * @return false if not support else true
 */
bool drvUsbRemoteWakeup();

/**
 * @brief set usb cable attached
 *
 * @param attach    usb cable attached
 */
void drvUsbSetAttach(bool attach);

#ifdef __cplusplus
}
#endif

#endif // _DRV_USB_H_
