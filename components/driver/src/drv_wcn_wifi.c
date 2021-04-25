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

#define CONFIG_SOC_8910

#include "drv_wifi.h"
#include "hwregs.h"
#include "hwreg_access.h"
#include "osi_api.h"
#include "osi_log.h"
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "drv_wcn.h"
#include "quec_cust_patch.h"

#define WIFI_SCAN_MIN_INTERVAL (120)
#define WIFI_RSSI_CALIB_OFFSET (25)

struct drv_wifi
{
    uint32_t irqn;
    drvWcn_t *wcn;
    osiMutex_t *lock;
    osiSemaphore_t *sema;
    bool async;
    bool running;
#if QUEC_PATCH_WIFISCAN_RSSI
    bool found_any;
#endif
    union {
        wifiScanRequest_t *cur_req;
        struct
        {
            wifiScanAsyncCb_t async_cb;
            void *async_ctx;
        };
    };
};

#if QUEC_PATCH_WIFISCAN_RSSI
typedef struct
{
    int8_t rssiraw;
    uint32_t gain;
    int8_t gain_rssi;
} wifirssi_t;
#endif

#ifdef CONFIG_SOC_8910

#if QUEC_PATCH_WIFISCAN_RSSI
static const int8_t kRssiThreshold[] = {INT8_MIN, -111, -99, -54, -46};

#ifdef CONFIG_QL_PROJECT_DEF_EC600U
static const int8_t kRssiOffset[] = {15, 17, 19, 18, 17};
#elif defined CONFIG_QL_PROJECT_DEF_EG700U
static const int8_t kRssiOffset[] = {15, 17, 19, 18, 17};
#else
static const int8_t kRssiOffset[] = {17, 18, 24, 24, 21};
#endif

#else
static const int8_t kRssiThreshold[] = {INT8_MIN,
                                        -117, -112, -111, -110, -108,
                                        -104, -96, -87, -83, -80,
                                        -71, -69, -66, -63, -60,
                                        -57, -54, -51, -41, -35,
                                        -30, -27, -20, -18, -13,
                                        -9};
static const int8_t kRssiOffset[] = {21,
                                     19, 20, 22, 25, 26,
                                     27, 28, 29, 23, 29,
                                     28, 29, 28, 29, 28,
                                     29, 28, 29, 28, 27,
                                     26, 22, 21, 17, 16,
                                     14};
#endif

static_assert(OSI_ARRAY_SIZE(kRssiThreshold) == OSI_ARRAY_SIZE(kRssiOffset), "");

static inline int8_t prvOffsetByRaw(int8_t rssi)
{
    unsigned i = 1;
    for (; i < OSI_ARRAY_SIZE(kRssiThreshold); ++i)
    {
        if (kRssiThreshold[i] >= rssi)
            break;
    }
    return kRssiOffset[i - 1];
}

#else

static inline int8_t prvOffsetByRaw(int8_t rssi)
{
    return WIFI_RSSI_CALIB_OFFSET;
}

#endif

static inline int8_t prvCalibRssi(int8_t rssi)
{
    return rssi + prvOffsetByRaw(rssi);
}

static inline void prvWifiOpen(drvWifi_t *d)
{
    drvWcnRequest(d->wcn);
    hwp_wcnRfIf->sys_control = 0x267;
    hwp_wcnWlan->config_reg = 0x802;
}

static inline void prvWifiClose(drvWifi_t *d)
{
    hwp_wcnRfIf->wf_control = 0x2;
    hwp_wcnRfIf->sys_control = 0x63;
    drvWcnRelease(d->wcn);
}

static inline void prvWifiSetChannel(uint8_t ch)
{
    REG_WCN_RF_IF_WF_CONTROL_T wf_control = {};
    wf_control.b.wf_tune = 0;
    wf_control.b.wf_chn = (ch & 0xf);
    hwp_wcnRfIf->wf_control = wf_control.v;
    OSI_NOP;
    REG_WAIT_FIELD_EQZ(wf_control, hwp_wcnRfIf->wf_control, wf_tune);

    wf_control.b.wf_tune = 1;
    hwp_wcnRfIf->wf_control = wf_control.v;
}

static inline void prvWifiClearReady()
{
    REG_WCN_WLAN_CONFIG_REG_T conf_reg = {hwp_wcnWlan->config_reg};
    conf_reg.b.apb_clear = 1;
    hwp_wcnWlan->config_reg = conf_reg.v;
    REG_WAIT_FIELD_NEZ(conf_reg, hwp_wcnWlan->config_reg, apb_clear);

    conf_reg.b.apb_clear = 0;
    hwp_wcnWlan->config_reg = conf_reg.v;
    REG_WAIT_FIELD_EQZ(conf_reg, hwp_wcnWlan->config_reg, apb_clear);
}

static void prvWifiDestroy(drvWifi_t *d)
{
    osiSemaphoreDelete(d->sema);
    osiMutexDelete(d->lock);
    drvWcnClose(d->wcn);
    free(d);
}

/**
 * @return continue scaning
 */

static bool prvSaveApInfo(wifiScanRequest_t *req, wifiApInfo_t *info)
{
    if (req->found >= req->max)
        return false;

    uint32_t critical = osiEnterCritical();
    wifiApInfo_t *s;
    bool newap = true;
    for (unsigned i = req->found; i > 0; --i)
    {
        s = &req->aps[i - 1];
        if (s->bssid_high == info->bssid_high && s->bssid_low == info->bssid_low)
        {
            if (info->rssival > s->rssival)
            {
                s->rssival = info->rssival;
                s->channel = info->channel;
            }
            newap = false;
            break;
        }
    }

    if (newap)
    {
        req->aps[req->found++] = *info;
    }
    osiExitCritical(critical);
    return true;
}

static void prvWcnWlanISR(void *param)
{
#if !QUEC_PATCH_WIFISCAN_RSSI
    REG_WCN_WLAN_CONFIG_REG_T conf = {hwp_wcnWlan->config_reg};
    conf.b.apb_clear = 1;
    hwp_wcnWlan->config_reg = conf.v;
    REG_WAIT_FIELD_NEZ(conf, hwp_wcnWlan->config_reg, apb_clear);
	REG_WCN_RF_IF_WF_CONTROL_T wf_control = {hwp_wcnRfIf->wf_control};
#endif
    wifiApInfo_t info = {
#if !QUEC_PATCH_WIFISCAN_RSSI
		.bssid_low = hwp_wcnWlan->bssidaddr_l,
		.bssid_high = (hwp_wcnWlan->bssidaddr_h & 0xffff),
		.channel = wf_control.b.wf_chn,
#endif
        .rssival = prvCalibRssi((int8_t)hwp_wcnWlan->rssival),
    };
#if QUEC_PATCH_WIFISCAN_RSSI
	if (info.rssival <= -8)
	{
		REG_WCN_RF_IF_WF_CONTROL_T wf_control = {hwp_wcnRfIf->wf_control};
		info.bssid_low = hwp_wcnWlan->bssidaddr_l;
	    info.bssid_high = (hwp_wcnWlan->bssidaddr_h & 0xffff);
	    info.channel = wf_control.b.wf_chn;
#endif
	    drvWifi_t *d = (drvWifi_t *)param;
	    if (d->async)
	    {
	        if (d->async_cb)
	            d->async_cb(&info, d->async_ctx);
	    }
	    else
	    {
	        if (!prvSaveApInfo(d->cur_req, &info))
	        {
	            osiSemaphoreRelease(d->sema);
#if !QUEC_PATCH_WIFISCAN_RSSI
	            return;
#endif
	        }
	    }
#if QUEC_PATCH_WIFISCAN_RSSI
	}
#endif

#if QUEC_PATCH_WIFISCAN_RSSI
    REG_WCN_WLAN_CONFIG_REG_T conf = {hwp_wcnWlan->config_reg};
    conf.b.apb_clear = 1;
    hwp_wcnWlan->config_reg = conf.v;
    REG_WAIT_FIELD_NEZ(conf, hwp_wcnWlan->config_reg, apb_clear);
#endif

    conf.b.apb_clear = 0;
    hwp_wcnWlan->config_reg = conf.v;
    REG_WAIT_FIELD_EQZ(conf, hwp_wcnWlan->config_reg, apb_clear);
}

static inline void prvStopScan(drvWifi_t *d)
{
#if QUEC_PATCH_WIFISCAN_RSSI
    osiIrqDisable(d->irqn);
#endif
    REG_WCN_WLAN_CONFIG_REG_T conf = {hwp_wcnWlan->config_reg};
    conf.b.apb_clear = 1;
    hwp_wcnWlan->config_reg = conf.v;
#if !QUEC_PATCH_WIFISCAN_RSSI
	osiIrqDisable(d->irqn);
#endif
}

static inline void prvStartScan(drvWifi_t *d)
{
    REG_WCN_WLAN_CONFIG_REG_T conf = {hwp_wcnWlan->config_reg};
    conf.b.apb_hold = 1;
    conf.b.apb_clear = 0;
#if QUEC_PATCH_WIFISCAN_RSSI
    d->found_any = false;
#endif
    hwp_wcnWlan->config_reg = conf.v;

    osiIrqEnable(d->irqn);
}

static void prvScanChannel_(drvWifi_t *d, uint8_t ch, wifiScanRequest_t *req, uint32_t tout)
{
    // acquire sema anyway
    osiSemaphoreTryAcquire(d->sema, 0);
    prvStartScan(d);

    osiSemaphoreTryAcquire(d->sema, tout);
    prvStopScan(d);
}

static void prvScanChannel(drvWifi_t *d, uint8_t ch, wifiScanRequest_t *req)
{
    uint32_t tout1 = WIFI_SCAN_MIN_INTERVAL;
    if (tout1 > req->maxtimeout)
        tout1 = req->maxtimeout;

    d->cur_req = req;
#if QUEC_PATCH_WIFISCAN_RSSI
    d->found_any = false;
#else
    uint32_t found = req->found;
#endif
    prvWifiSetChannel(ch);
    prvScanChannel_(d, ch, req, tout1);
	
#if !QUEC_PATCH_WIFISCAN_RSSI
    if (found != req->found)
#else
    if (d->found_any)
#endif
    {
        uint32_t tout2 = req->maxtimeout - tout1;
        if (tout2 != 0)
            prvScanChannel_(d, ch, req, tout2);
    }
}

static void prvScanChannelAsync(drvWifi_t *d, uint8_t ch)
{
    prvWifiSetChannel(ch);
    prvStartScan(d);
}

static void prvScanAllChannels(drvWifi_t *d, wifiScanRequest_t *req, uint32_t round)
{
    for (unsigned n = 0; n < round; ++n)
    {
        for (uint8_t c = 1; c <= WIFI_CHANNEL_MAX; ++c)
        {
            prvScanChannel(d, c, req);
        }
    }
}

drvWifi_t *drvWifiOpen()
{
    drvWifi_t *d = (drvWifi_t *)calloc(1, sizeof(drvWifi_t));
    if (d == NULL)
        return NULL;

    d->wcn = drvWcnOpen(WCN_USER_WIFI);
    if (d->wcn == NULL)
        goto fail;

    d->lock = osiMutexCreate();
    if (d->lock == NULL)
        goto fail;

    d->sema = osiSemaphoreCreate(1, 0);
    if (d->sema == NULL)
        goto fail;

    d->irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_WCN_WLAN);
    osiIrqSetHandler(d->irqn, prvWcnWlanISR, d);
    osiIrqSetPriority(d->irqn, SYS_IRQ_PRIO_WCN_WLAN);
    return d;

fail:

    prvWifiDestroy(d);
    return NULL;
}

void drvWifiClose(drvWifi_t *d)
{
    if (d)
    {
        prvWifiClose(d);
        osiIrqDisable(d->irqn);
        osiIrqSetHandler(d->irqn, NULL, NULL);
        prvWifiDestroy(d);
    }
}

static bool prvLockCheckSetRunning(drvWifi_t *d)
{
    osiMutexLock(d->lock);
    if (d->running)
    {
        osiMutexUnlock(d->lock);
        return false;
    }
    d->running = true;
    return true;
}

bool drvWifiScanAllChannel(drvWifi_t *d, wifiScanRequest_t *req, uint32_t round)
{
    if (d == NULL || req == NULL)
        return false;

    if (!prvLockCheckSetRunning(d))
        return false;

    prvWifiOpen(d);
    prvScanAllChannels(d, req, round);
    prvWifiClose(d);

    d->running = false;
    osiMutexUnlock(d->lock);

    return true;
}

bool drvWifiScanChannel(drvWifi_t *d, wifiScanRequest_t *req, uint32_t channel)
{
    if (d == NULL || req == NULL || channel < 1 || channel > WIFI_CHANNEL_MAX)
        return false;

    if (!prvLockCheckSetRunning(d))
        return false;

    prvWifiOpen(d);
    prvScanChannel(d, channel, req);
    prvWifiClose(d);

    d->running = false;
    osiMutexUnlock(d->lock);
    return true;
}

bool drvWifiScanAsyncStart(drvWifi_t *d, uint32_t ch, wifiScanAsyncCb_t cb, void *param)
{
    if (d == NULL || cb == NULL || !prvLockCheckSetRunning(d))
        return false;

    d->async = true;
    d->async_cb = cb;
    d->async_ctx = param;

    prvWifiOpen(d);
    prvScanChannelAsync(d, ch);
    osiMutexUnlock(d->lock);

    return true;
}

void drvWifiScanAsyncStop(drvWifi_t *d)
{
    osiMutexLock(d->lock);
    if (d->running && d->async)
    {
        prvStopScan(d);
        prvWifiClose(d);
        d->async = false;
        d->running = false;
    }
    osiMutexUnlock(d->lock);
}
