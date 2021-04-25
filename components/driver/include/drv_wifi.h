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

#ifndef _DRV_WIFI_
#define _DRV_WIFI_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include <stdint.h>
#include <stdbool.h>

#define WIFI_CHANNEL_MAX 13

typedef struct drv_wifi drvWifi_t;

/**
 * @brief WIFI AP information
 */
typedef struct
{
    uint32_t bssid_low;  ///< mac address low
    uint16_t bssid_high; ///< mac address high
    uint8_t channel;     ///< channel id
    int8_t rssival;      ///< signal strength
} wifiApInfo_t;

typedef struct
{
    uint32_t max;        ///< set by caller, must room count for store aps
    uint32_t found;      ///< set by wifi, actual found ap count
    uint32_t maxtimeout; ///< max timeout in milliseconds for each channel
    wifiApInfo_t *aps;   ///< room for aps
} wifiScanRequest_t;

/**
 * \brief function type, will be call in IRQ.
 *        report new scanned ap information
 * \param[out] info wifi ap info
 */
typedef void (*wifiScanAsyncCb_t)(const wifiApInfo_t *info, void *ctx);

/**
 * \brief open wifi
 *
 * \return
 *      - NULL if fail else wifi context
 */
drvWifi_t *drvWifiOpen(void);

/**
 * \brief close wifi
 *
 * \param d     wifi context
 */
void drvWifiClose(drvWifi_t *d);

/**
 * \brief scan all channels, this API is deprecated
 *
 * \param d     wifi context
 * \param req   wifi scan request context
 * \param round scan all channel for \round times
 * \return
 *      - true on succeed else fail
 */
bool drvWifiScanAllChannel(drvWifi_t *d, wifiScanRequest_t *req, uint32_t round) __attribute__((deprecated));

/**
 * \brief scan a specific channel
 *
 * \param d     wifi context
 * \param req   wifi scan request context
 * \param ch    the channel id
 * \return
 *      - true on succeed else fail
 */
bool drvWifiScanChannel(drvWifi_t *d, wifiScanRequest_t *req, uint32_t ch);

/**
 * \brief start scan a channel and capture the result by cb
 *        this API is asynchornous and not stop scan before call `drvWifiScanAsyncStop`
 * \note the `cb` maybe called in ISR, do not do time-consuming operations
 *
 * \param d     wifi device
 * \param cb    the scan callback
 * \param ctx   caller context
 * \return
 *      - true on success else fail
 */
bool drvWifiScanAsyncStart(drvWifi_t *d, uint32_t ch, wifiScanAsyncCb_t cb, void *ctx);

/**
 * \brief stop the async scan
 *
 * \param d     wifi device
 */
void drvWifiScanAsyncStop(drvWifi_t *d);

OSI_EXTERN_C_END

#endif
