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

#ifndef _USB__DEVICE_H_
#define _USB__DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "usb/usb_ch9.h"

typedef enum
{
    EP0_SETUP = 0,
#define EP0_STATE_DIRIN_MASK 0b10
#define EP0_STATE_DATA_MASK 0b1
    EP0_DATA_IN = 0b111,
    EP0_DATA_OUT = 0b101,
    EP0_STATUS_IN = 0b110,
    EP0_STATUS_OUT = 0b100,
} ep0State_t;

typedef enum
{
    UDC_IDLE = 0,
    UDC_CONNECTED,
    UDC_DISCONNECT,
} udcEvent_t;

typedef void (*udcNotifier_t)(void *param, udcEvent_t event);

/**
 * @brief USB device controller
 */
typedef struct udc_s udc_t;

/**
 * @brief USB device driver
 *
 * Such as a specific class, or a composite driver
 */
typedef struct usb_dev_driver_s udevDrv_t;

/**
 * @brief usb endpoint
 */
typedef struct usb_ep
{
    union {
        struct
        {
            uint8_t num : 7;
            uint8_t dirin : 1;
        };
        uint8_t address;
    };
    uint16_t mps;
} usbEp_t;

/**
 * @brief usb string
 */
typedef struct usb_string
{
    uint8_t id;
    uint8_t ref_count;
    const char *s;
} usbString_t;

/**
 * @brief the usb transfer request
 */
typedef struct usb_xfer
{
    void *buf;                                            ///< transfer buffer
    uint32_t length;                                      ///< transfer data length
    uint32_t actual;                                      ///< actual transfered length, udc set it, caller only read
    int status;                                           ///< transfer status, (0 - success; other negative - fail)
    uint8_t zlp;                                          ///< may send zero packet or not, in transfer only
    uint8_t uncached;                                     ///< address is uncached
    void *param;                                          ///< caller context
    void (*complete)(usbEp_t *ep, struct usb_xfer *xfer); ///< transfer done callback
} usbXfer_t;

typedef struct
{
    void (*setclk)(udc_t *udc, bool on);   ///< set controller clock
    void (*setpower)(udc_t *udc, bool on); ///< set controller power
    void (*enable)(udc_t *udc);            ///< enable the usb controller
    void (*disable)(udc_t *udc);           ///< disable the usb controller
    void (*suspend)(udc_t *udc);           ///< suspend the usb controller
    void (*resume)(udc_t *udc);            ///< resume the usb controller
} udcPlatOps_t;

/**
 * @brief struct for udc operations
 */
typedef struct
{
    void (*start)(udc_t *udc);                                         ///< start the udc
    void (*stop)(udc_t *udc);                                          ///< stop the udc
    bool (*remote_wakeup)(udc_t *udc);                                 ///< remote wakeup
    int (*enqueue)(udc_t *udc, usbEp_t *ep, usbXfer_t *req);           ///< enqueue an usb request
    void (*dequeue)(udc_t *udc, usbEp_t *ep, usbXfer_t *req);          ///< dequeue an usb request
    void (*dequeue_all)(udc_t *udc, usbEp_t *ep);                      ///< dequeue all for specific endpoint
    usbEp_t *(*ep_alloc)(udc_t *udc, usb_endpoint_descriptor_t *desc); ///< allocate an usb endpoint
    void (*ep_free)(udc_t *udc, usbEp_t *ep);                          ///< free an usb endpoint
    int (*ep_enable)(udc_t *udc, usbEp_t *ep);                         ///< enable the endpoint
    void (*ep_disable)(udc_t *udc, usbEp_t *ep);                       ///< disable the endpoint
    void (*ep_stall)(udc_t *udc, usbEp_t *ep, bool halt);              ///< stall or clear stall for an endpoint
    usbXfer_t *(*xfer_alloc)(udc_t *udc);                              ///< allocate an usb xfer
    void (*xfer_free)(udc_t *udc, usbXfer_t *xfer);                    ///< free the usb xfer
    // gdb mode
    void (*gdb_poll)(udc_t *udc); ///< poll intr in gdb mode
} udcOps_t;

enum udc_feature
{
#define UDC_FEATURE_ATTRIBUTE_MASK 0xff
    /// UC Attribute
    UDC_FEATURE_REMOTE_WAKEUP = UC_REMOTE_WAKEUP,
    UDC_FEATURE_SELF_POWERED = UC_SELF_POWERED,
    UDC_FEATURE_BUS_POWERED = UC_BUS_POWERED,
    /// Software defined
    UDC_FEATURE_WAKEUP_ON_WRITE = (1 << 8),
};

struct udc_s
{
    int speed;                     ///< controller speed, from controller
    int feature;                   ///< controller feature defined in udc_feature
    udcPlatOps_t plat_ops;         ///< operates for specific platform
    udcOps_t ops;                  ///< controller operates
    unsigned long platform_priv;   ///< platform private data
    unsigned long controller_priv; ///< controller private data
    usbEp_t *ep0;                  ///< ep0, from controller
};

typedef struct
{
    int (*bind)(udevDrv_t *driver);
    void (*unbind)(udevDrv_t *driver);
    int (*setup)(udevDrv_t *driver, const usb_device_request_t *setup);
    void (*start)(udevDrv_t *driver);
    void (*stop)(udevDrv_t *driver);
    void (*connect)(udevDrv_t *driver);
    void (*disconnect)(udevDrv_t *driver);
} udevDrvOps_t;

struct usb_dev_driver_s
{
    udc_t *udc;
    udevDrvOps_t ops;
};

#define USB_DUMMY_RX_BUF_SIZE (512)
extern char gDummyUsbRxBuf[USB_DUMMY_RX_BUF_SIZE];

/**
 * @brief Create the usb device controller instance
 *
 * @param name  the controller device name
 * @return
 *      - NULL  fail
 *      - the usb device controller
 */
udc_t *udcCreate(uint32_t name);

/**
 * @brief Destroy the udc
 *
 * @param udc   the udc
 */
void udcDestroy(udc_t *udc);

/**
 * @brief Bind the udc driver to the udc
 *
 * @param udc   the udc
 * @param drv   the udc driver
 */
void udcBindDriver(udc_t *udc, udevDrv_t *drv);

/**
 * @brief Get the binding driver
 *
 * @param udc   the udc
 * @return
 *      - NULL  No driver bind to this udc
 *      - other the udc driver
 */
udevDrv_t *udcGetDriver(udc_t *udc);

/**
 * @brief udc set event notifier
 *
 * @param udc   the udc
 * @param n     the notifier
 * @param param caller context
 */
void udcSetNotifier(udc_t *udc, udcNotifier_t n, void *param);

/**
 * @brief udc connect
 * @param udc   the udc
 */
void udcConnect(udc_t *udc);

/**
 * @brief udc disconnect
 * @param udc   the udc
 */
void udcDisconnect(udc_t *udc);

/**
 * @brief udc remote wakeup
 * @param udc   the udc
 */
bool udcRemoteWakeup(udc_t *udc);

#define CHECK_UDC_PLAT_OPS(u, o) ((u) != NULL && (u)->plat_ops.o != NULL)
#define CHECK_UDC_OPS(u, o) ((u) != NULL && (u)->ops.o != NULL)
#define CHECK_DRV_OPS(d, o) ((d) != NULL && (d)->ops.o != NULL)

/**
 * @brief The platform turn on/off usb power
 *
 * @param udc   the udc
 * @param on    on or off the power
 */
static inline void udcPlatSetPower(udc_t *udc, bool on)
{
    if (CHECK_UDC_PLAT_OPS(udc, setpower))
        return udc->plat_ops.setpower(udc, on);
}

/**
 * @brief The platform turn on/off clock
 *
 * @param udc   the udc
 * @param on    on or off the clock
 */
static inline void udcPlatSetClock(udc_t *udc, bool on)
{
    if (CHECK_UDC_PLAT_OPS(udc, setclk))
        return udc->plat_ops.setclk(udc, on);
}

/**
 * @brief The platform process on usb controller suspend
 *
 * @param udc   the udc
 */
static inline void udcPlatSuspend(udc_t *udc)
{
    if (CHECK_UDC_PLAT_OPS(udc, suspend))
        udc->plat_ops.suspend(udc);
}

/**
 * @brief The platform process on usb controller resume
 *
 * @param udc   the udc
 */
static inline void udcPlatResume(udc_t *udc)
{
    if (CHECK_UDC_PLAT_OPS(udc, resume))
        udc->plat_ops.resume(udc);
}

/**
 * @brief The platform enable usb module
 *
 * Init usb phy and others
 * @param udc   the udc
 */
static inline void udcPlatEnable(udc_t *udc)
{
    if (CHECK_UDC_PLAT_OPS(udc, enable))
        return udc->plat_ops.enable(udc);
}

/**
 * @brief The platform disable usb module
 *
 * @param udc   the udc
 */
static inline void udcPlatDisable(udc_t *udc)
{
    if (CHECK_UDC_PLAT_OPS(udc, disable))
        return udc->plat_ops.disable(udc);
}

/**
 * @brief Start udc, make the controller works
 *
 * @param udc   the udc
 */
void udcStart(udc_t *udc);

/**
 * @brief Stop udc, the controller will be closed
 *
 * @param udc   the udc
 */
void udcStop(udc_t *udc);

/**
 * @brief Enqueue an usb transfer
 *
 * Before call the method, caller should be sure ep is a valid
 * usb endpoint, and make sure full fill the usb transfer.
 *
 * @param udc   the udc
 * @param ep    the usb endpoint
 * @param xfer  the usb transfer
 * @return
 *      - 0     success
 *      - (-1)  fail
 */
static inline int udcEpQueue(udc_t *udc, usbEp_t *ep, usbXfer_t *xfer)
{
    if (CHECK_UDC_OPS(udc, enqueue) && ep != NULL && xfer != NULL)
        return udc->ops.enqueue(udc, ep, xfer);
    return -1;
}

/**
 * @brief Dequeue an usb transfer
 *
 * @param udc   the udc
 * @param ep    the usb endpoint
 * @param xfer  the usb transfer
 */
static inline void udcEpDequeue(udc_t *udc, usbEp_t *ep, usbXfer_t *xfer)
{
    if (CHECK_UDC_OPS(udc, dequeue) && ep != NULL && xfer != NULL)
        udc->ops.dequeue(udc, ep, xfer);
}

/**
 * @brief Dequeue all transfers for an usb endpoint
 *
 * @param udc   the udc
 * @param ep    the usb endpoint
 */
static inline void udcEpDequeueAll(udc_t *udc, usbEp_t *ep)
{
    if (CHECK_UDC_OPS(udc, dequeue_all) && ep != NULL)
        udc->ops.dequeue_all(udc, ep);
}

/**
 * @brief Allocate an usb endpoint
 *
 * @param udc   the udc
 * @param desc  the usb endpoint descriptor
 * @return
 *      - NULL  fail
 *      - other usb endpoint
 */
static inline usbEp_t *udcEpAlloc(udc_t *udc, usb_endpoint_descriptor_t *desc)
{
    if (CHECK_UDC_OPS(udc, ep_alloc) && desc != NULL)
        return udc->ops.ep_alloc(udc, desc);
    return NULL;
}

/**
 * @brief Free an usb endpoint
 *
 * @param udc   the udc
 * @param ep    the usb endpoint
 */
static inline void udcEpFree(udc_t *udc, usbEp_t *ep)
{
    if (CHECK_UDC_OPS(udc, ep_free) && ep != NULL)
        return udc->ops.ep_free(udc, ep);
}

/**
 * @brief Allocate an usb xfer
 *
 * @param udc the udc
 * @return
 *      - NULL  fail
 *      - other the xfer
 */
static inline usbXfer_t *udcXferAlloc(udc_t *udc)
{
    if (CHECK_UDC_OPS(udc, xfer_alloc))
        return udc->ops.xfer_alloc(udc);
    return NULL;
}

/**
 * @brief Free an usb xfer
 *
 * @param udc   the udc
 * @param xfer  the xfer
 */
static inline void udcXferFree(udc_t *udc, usbXfer_t *xfer)
{
    if (CHECK_UDC_OPS(udc, xfer_free) && xfer)
        return udc->ops.xfer_free(udc, xfer);
}

/**
 * @brief Enable the endpoint
 *
 * @param udc   the udc
 * @param ep    the endpoint
 * @return
 *      - (-1)  fail
 *      - other success
 */
static inline int udcEpEnable(udc_t *udc, usbEp_t *ep)
{
    if (CHECK_UDC_OPS(udc, ep_enable) && ep != NULL)
        return udc->ops.ep_enable(udc, ep);
    return -1;
}

/**
 * @brief Disable the endpoint
 *
 * @param udc   the udc
 * @param ep    the endpoint
 */
static inline void udcEpDisable(udc_t *udc, usbEp_t *ep)
{
    if (CHECK_UDC_OPS(udc, ep_disable) && ep != NULL)
        return udc->ops.ep_disable(udc, ep);
}

/**
 * @brief Stall an endpoint
 *
 * @param udc   the udc
 * @param ep    the usb endpoint
 * @param halt  true: stall; false: clear stall
 */
static inline void udcEpStall(udc_t *udc, usbEp_t *ep, bool halt)
{
    if (CHECK_UDC_OPS(udc, ep_stall) && ep != NULL)
        udc->ops.ep_stall(udc, ep, halt);
}

/**
 * @brief Poll usb interrupt in gdb mode
 *
 * @param udc   the usb device controller
 */
static inline void udcGdbPollIntr(udc_t *udc)
{
    if (CHECK_UDC_OPS(udc, gdb_poll))
        udc->ops.gdb_poll(udc);
}

/**
 * @brief usb deivce driver process setup packet
 *
 * @param drv   the driver
 * @param setup the setup packet
 * @return
 *      - (-1)      process failed
 *      - 0         success
 */
static inline int udevDriverSetup(udevDrv_t *drv, const usb_device_request_t *setup)
{
    if (CHECK_DRV_OPS(drv, setup))
        return drv->ops.setup(drv, setup);
    return -1;
}

/**
 * @brief usb device driver bind the usb controller
 *
 * @param drv   the driver
 * @return
 *      - (-1)  failed
 *      - 0     success
 */
static inline int udevDriverBind(udevDrv_t *drv)
{
    if (CHECK_DRV_OPS(drv, bind))
        return drv->ops.bind(drv);
    return -1;
}

/**
 * @brief usb device driver unbind the controller
 *
 * @param drv   the driver
 */
static inline void udevDriverUnbind(udevDrv_t *drv)
{
    if (CHECK_DRV_OPS(drv, unbind))
        drv->ops.unbind(drv);
}

/**
 * @brief usb device driver start.
 *
 * Prepare environment, etc.
 *
 * @param drv   the driver
 */
static inline void udevDriverStart(udevDrv_t *drv)
{
    if (CHECK_DRV_OPS(drv, start))
        drv->ops.start(drv);
}

/**
 * @brief usb device driver stop.
 *
 * Release resource and etc.
 *
 * @param drv   the driver
 */
static inline void udevDriverStop(udevDrv_t *drv)
{
    if (CHECK_DRV_OPS(drv, stop))
        drv->ops.stop(drv);
}

/**
 * @brief usb device connect to a host
 *
 * @param drv   the driver
 */
static inline void udevDriverConnect(udevDrv_t *drv)
{
    if (CHECK_DRV_OPS(drv, connect))
        drv->ops.connect(drv);
}

/**
 * @brief usb device disconnect to a host
 *
 * @param drv   the driver
 */
static inline void udevDriverDisconnect(udevDrv_t *drv)
{
    if (CHECK_DRV_OPS(drv, disconnect))
        drv->ops.disconnect(drv);
}

#ifdef __cplusplus
}
#endif

#endif
