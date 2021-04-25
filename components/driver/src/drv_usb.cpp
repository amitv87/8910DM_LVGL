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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('D', 'U', 'S', 'B')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "drv_usb.h"
#include "drv_charger.h"
#include "drv_names.h"
#include "osi_api.h"
#include "osi_log.h"
#include "osi_sysnv.h"
#include "usb/usb_device.h"
#include "usb/usb_composite_device.h"
#include "usb/usb_cops_func.h"
#include "diag_config.h"
#include "drv_config.h"
#include "quec_proj_config.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define USB_TASK_PRIORITY (OSI_PRIORITY_HIGH)
#define USB_TASK_STACK_SIZE (512 * 4)
#define USB_TASK_QUEUE_SIZE (8)

#ifdef CONFIG_QUEC_PROJECT_FEATURE
extern quec_usb_setting_s quec_usb_current_setting;
#endif


enum class DrvUsbState
{
    IDLE = 0,    // usb cable not connected
    DEBOUNCE,    // usb cable has already connected, but not start enumerating
    VBUS_STABLE, // usb vbus stable
    CONNECT_WIP, // usb try enumerating
    CONNECTED,   // usb enumerated
};

struct DrvUsb final
{
    explicit DrvUsb() {}
    ~DrvUsb();

    bool init(uint32_t name, int detmode);
    inline void changeState(DrvUsbState newstate)
    {
        OSI_LOGI(0, "usb change state %d->%d (%d/%d)", mState, newstate, mMode, mDebounceTimeMs);
        mState = newstate;
    }

    inline bool readyStart() const
    {
        return mEnable && mState == DrvUsbState::VBUS_STABLE && mMode != DRV_USB_CHARGER_ONLY;
    }

    inline void lock()
    {
        osiMutexLock(mLock);
    }

    inline void unlock()
    {
        osiMutexUnlock(mLock);
    }

    bool mEnable = false;
    uint32_t mDebounceTimeMs = 0;
    uint64_t mStableTime = 0;
    enum DrvUsbState mState = DrvUsbState::IDLE;
    drvUsbWorkMode_t mMode = DRV_USB_CHARGER_ONLY;
    int mDetMode;
    udc_t *mUdc = nullptr;
    cops_t *mCops = nullptr;
    osiTimer_t *mTimerDebounce = nullptr;
    osiTimer_t *mTimerNotRecognize = nullptr;
    osiWork_t *mWorkChargerOn = nullptr;
    osiWork_t *mWorkNotRecognize = nullptr;
    osiWorkQueue_t *mWorkQueue = nullptr;
    osiMutex_t *mLock = nullptr;
};

static DrvUsb *gDrvUsb = nullptr;

static void _chargerOff(DrvUsb *d)
{
    auto state = d->mState;
    d->changeState(DrvUsbState::IDLE);
    if (state == DrvUsbState::CONNECT_WIP)
        osiTimerStop(d->mTimerNotRecognize);
    else if (state == DrvUsbState::DEBOUNCE)
    {
        if (d->mDebounceTimeMs != 0)
            osiTimerStop(d->mTimerDebounce);
        osiWorkCancel(d->mWorkChargerOn);
    }
    udcStop(d->mUdc);
}

static inline void _vbusDebounceStart(DrvUsb *d)
{
    if (d->mDebounceTimeMs == 0)
        osiWorkEnqueue(d->mWorkChargerOn, d->mWorkQueue);
    else
        osiTimerStart(d->mTimerDebounce, d->mDebounceTimeMs);
}

static void _startUsb(DrvUsb *d)
{
    d->changeState(DrvUsbState::CONNECT_WIP);
    if (d->mDetMode != USB_DETMODE_AON)
    {   
#ifndef CONFIG_QUEC_PROJECT_FEATURE    
        osiTimerStart(d->mTimerNotRecognize, CONFIG_USB_CONNECT_TIMEOUT);  
#else
        uint32_t usb_dect_timeout = quec_usb_current_setting.usb_detect_time; 
        if(0 != usb_dect_timeout)
        {
            osiTimerStart(d->mTimerNotRecognize, usb_dect_timeout);  
        }
#endif
    }
    udcStart(d->mUdc);
}

static void _chargerOnWork(void *ctx)
{
    DrvUsb *d = (DrvUsb *)ctx;
    d->lock();
    drvChargerType_t type = drvChargerGetType();
    OSI_LOGI(0, "usb charger on type %d", type);
    bool vbus_new = true;
    if (type == DRV_CHARGER_TYPE_DCP)
        vbus_new = false; // pure charger
    else if (type == DRV_CHARGER_TYPE_NONE)
        vbus_new = false; // charger disappeared

    if (vbus_new && d->mState < DrvUsbState::VBUS_STABLE)
    {
        d->mStableTime = osiUpTime();
        d->changeState(DrvUsbState::VBUS_STABLE);
    }

    if (d->readyStart())
        _startUsb(d);
    d->unlock();
}

static void _notRecognizeWork(void *ctx)
{
    DrvUsb *d = (DrvUsb *)ctx;
    d->lock();
    d->changeState(DrvUsbState::VBUS_STABLE);
    udcStop(d->mUdc);
    d->unlock();
}

static void _udcEventNotifier(void *ctx, udcEvent_t event)
{
    DrvUsb *d = (DrvUsb *)ctx;
    OSI_LOGI(0, "udc event %x", event);
    if (event == UDC_CONNECTED)
    {
        if (d->mState == DrvUsbState::CONNECT_WIP)
        {
            osiTimerStop(d->mTimerNotRecognize);
            d->changeState(DrvUsbState::CONNECTED);
        }
    }
}

DrvUsb::~DrvUsb()
{
    osiTimerStop(mTimerDebounce);
    osiTimerDelete(mTimerDebounce);
    osiWorkDelete(mWorkChargerOn);

    osiTimerStop(mTimerNotRecognize);
    osiTimerDelete(mTimerNotRecognize);
    osiWorkDelete(mWorkNotRecognize);

    copsDestroy(mCops);
    udcDestroy(mUdc);
    osiWorkQueueDelete(mWorkQueue);
    osiMutexDelete(mLock);
}

// All resource will be released in destructure function
bool DrvUsb::init(uint32_t name, int detmode)
{
    mLock = osiMutexCreate();
    if (!mLock)
        return false;

    mUdc = udcCreate(name);
    mCops = copsCreate();
    if (!mUdc || !mCops)
        return false;

    udcStop(mUdc);
    udcSetNotifier(mUdc, _udcEventNotifier, this);
    udcBindDriver(mUdc, copsGetGadgetDriver(mCops));

    mWorkQueue = osiWorkQueueCreate("usb", 1, USB_TASK_PRIORITY, USB_TASK_STACK_SIZE);
    mWorkChargerOn = osiWorkCreate(_chargerOnWork, nullptr, this);
    mWorkNotRecognize = osiWorkCreate(_notRecognizeWork, nullptr, this);
    if (!(mWorkQueue && mWorkChargerOn && mWorkNotRecognize))
        return false;

    mTimerDebounce = osiTimerCreateWork(mWorkChargerOn, mWorkQueue);
    mTimerNotRecognize = osiTimerCreateWork(mWorkNotRecognize, mWorkQueue);
    if (!(mTimerDebounce && mTimerNotRecognize))
        return false;

    mMode = DRV_USB_CHARGER_ONLY;
    mDetMode = detmode;
    if (detmode == USB_DETMODE_AON)
    {
        mStableTime = osiUpTime();
        mState = DrvUsbState::VBUS_STABLE;
    }
    else
    {
        mState = DrvUsbState::IDLE;
    }
    return true;
}

void drvUsbInit()
{
    if (gDrvUsb != nullptr)
        return;

    DrvUsb *d = new DrvUsb();
    if (d)
    {
        if (d->init(DRV_NAME_USB20, gSysnvUsbDetMode))
            gDrvUsb = d;
        else
            delete d;
    }

    if (!gDrvUsb)
    {
        OSI_LOGE(0, "usb module init fail");
        osiPanic();
    }
}

osiWorkQueue_t *drvUsbWorkQueue()
{
    return gDrvUsb->mWorkQueue;
}

static bool _setEcmAcmSerials(DrvUsb *d)
{
    copsFunc_t *funcs[7] = {
        createEcmFunc(),
        createSerialFunc(DRV_NAME_USRL_COM0),
        createDebugSerialFunc(DRV_NAME_USRL_COM4), // ap log
        createCdcAcmFunc(DRV_NAME_ACM0),
        createCdcAcmFunc(DRV_NAME_ACM1),
        createCdcAcmFunc(DRV_NAME_ACM2),
        createCdcAcmFunc(DRV_NAME_ACM3),
    };

    for (auto f : funcs)
    {
        if (f == nullptr)
            goto fail;
    }

    if (copsAddFunctions(d->mCops, &funcs[0], 7))
    {
        copsSetVendorId(d->mCops, USB_VID_SPRD);
        copsSetProductId(d->mCops, USB_PID_ECM_ACM_SERIAL);
        return true;
    }

fail:
    for (auto f : funcs)
    {
        if (f != nullptr)
            cfDestroy(f);
    }
    return false;
}

static bool _setSingleSerial(DrvUsb *d, drvUsbWorkMode_t mode)
{
    uint32_t name;
    uint16_t vid, pid;
    if (mode == DRV_USB_RDA_ROM_SERIAL)
    {
        name = DRV_NAME_USRL_COM0;
        vid = USB_VID_RDA_DOWNLOAD;
        pid = USB_PID_RDA_DOWNLOAD;
    }
    else
    {
        name = DRV_NAME_USRL_COM1;
        vid = USB_VID_SPRD;
        pid = USB_PID_NPI_SERIAL;
    }

    auto f = createDebugSerialFunc(name);
    if (!f)
        return false;

    if (copsAddFunction(d->mCops, f))
    {
        copsSetVendorId(d->mCops, vid);
        copsSetProductId(d->mCops, pid);
        return true;
    }
    else
    {
        cfDestroy(f);
        return false;
    }
}

static bool _setSerials(DrvUsb *d)
{
    copsFunc_t *funcs[8] = {
        createSerialFunc(DRV_NAME_USRL_COM0),
        createDebugSerialFunc(DRV_NAME_USRL_COM1), // diag
        createDebugSerialFunc(DRV_NAME_USRL_COM2), // mos log
        createDebugSerialFunc(DRV_NAME_USRL_COM3), // modem log
        createDebugSerialFunc(DRV_NAME_USRL_COM4), // ap log
        createSerialFunc(DRV_NAME_USRL_COM5),
        createSerialFunc(DRV_NAME_USRL_COM6),
        createSerialFunc(DRV_NAME_USRL_COM7),
    };

    for (auto f : funcs)
    {
        if (f == nullptr)
            goto fail;
    }

    if (copsAddFunctions(d->mCops, &funcs[0], 8))
    {
        copsSetVendorId(d->mCops, USB_VID_SPRD);
        copsSetProductId(d->mCops, USB_PID_SERIALS);
        return true;
    }

fail:
    for (auto f : funcs)
    {
        if (f != nullptr)
            cfDestroy(f);
    }
    return false;
}

static bool _setSingleMode(DrvUsb *d, drvUsbWorkMode_t mode)
{
    uint16_t vid = 0x1782;
    uint16_t pid = 0x4d11;
    copsFunc_t *f = nullptr;

    switch (mode)
    {
    case DRV_USB_RNDIS_ONLY:
        f = createRndisFunc();
        break;

    case DRV_USB_ECM_ONLY:
        f = createEcmFunc();
        break;

    default:
        break;
    }

    if (!f)
        return false;

    if (!copsAddFunction(d->mCops, f))
    {
        cfDestroy(f);
        return false;
    }

    copsSetVendorId(d->mCops, vid);
    copsSetProductId(d->mCops, pid);
    return true;
}

static bool _setUetherAndSerials(DrvUsb *d, drvUsbWorkMode_t mode)
{
    copsFunc_t *funcs[9] = {
        mode == DRV_USB_RNDIS_AND_SERIALS ? createRndisFunc() : createEcmFunc(),
        createSerialFunc(DRV_NAME_USRL_COM0),
        createDebugSerialFunc(DRV_NAME_USRL_COM1), // diag
        createDebugSerialFunc(DRV_NAME_USRL_COM2), // mos log
        createDebugSerialFunc(DRV_NAME_USRL_COM3), // modem log
        createDebugSerialFunc(DRV_NAME_USRL_COM4), // ap log
        createSerialFunc(DRV_NAME_USRL_COM5),
        createSerialFunc(DRV_NAME_USRL_COM6),
        createSerialFunc(DRV_NAME_USRL_COM7),
    };

    for (auto f : funcs)
    {
        if (f == nullptr)
            goto fail;
    }

    if (copsAddFunctions(d->mCops, &funcs[0], 9))
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        copsSetVendorId(d->mCops, QUEC_USB_VID_DEFAULT);
        copsSetProductId(d->mCops, QUEC_USB_PID_DEFAULT);
#else
        copsSetVendorId(d->mCops, USB_VID_SPRD);
        copsSetProductId(d->mCops, USB_PID_RNDIS_AND_SERIALS);
#endif
        return true;
    }

fail:
    for (auto f : funcs)
    {
        if (f != nullptr)
            cfDestroy(f);
    }
    return false;
}

static void _stopUsbBySoft(DrvUsb *d)
{
    auto state = d->mState;
    if (state > DrvUsbState::VBUS_STABLE)
    {
        d->changeState(DrvUsbState::VBUS_STABLE);
        if (state == DrvUsbState::CONNECT_WIP)
            osiTimerStop(d->mTimerNotRecognize);
        udcStop(d->mUdc);
    }
}

bool drvUsbSetWorkMode(drvUsbWorkMode_t mode)
{
    auto d = gDrvUsb;
    OSI_LOGI(0, "usb set work mode %d/%d", mode, d->mMode);
    if (mode == d->mMode)
        return true;

    d->lock();
    _stopUsbBySoft(d);
    copsRemoveAllFunction(d->mCops);

    bool result = (mode == DRV_USB_CHARGER_ONLY);
    if (mode == DRV_USB_RDA_ROM_SERIAL || mode == DRV_USB_NPI_SERIAL)
        result = _setSingleSerial(d, mode);
    else if (mode == DRV_USB_SERIALS)
        result = _setSerials(d);
    else if (mode == DRV_USB_RNDIS_AND_SERIALS || mode == DRV_USB_ECM_AND_SERIALS)
        result = _setUetherAndSerials(d, mode);
    else if (mode == DRV_USB_ECM_ACM_SERIALS)
        result = _setEcmAcmSerials(d);
    else if (mode >= DRV_USB_SINGLE_INTF_START && mode <= DRV_USB_SINGLE_INTF_END)
        result = _setSingleMode(d, mode);

    if (result)
    {
        d->mMode = mode;
        if (d->readyStart())
            _startUsb(d);
    }
    else
    {
        d->mMode = DRV_USB_CHARGER_ONLY;
    }

    d->unlock();
    return result;
}

drvUsbWorkMode_t drvUsbCurrentMode()
{
    return gDrvUsb->mMode;
}

bool drvUsbIsConnected()
{
    return gDrvUsb->mState == DrvUsbState::CONNECTED;
}

void drvUsbEnable(uint32_t debounce)
{
    auto d = gDrvUsb;
    if (d->mEnable)
        return;

    d->lock();
    d->mDebounceTimeMs = debounce;
    d->mEnable = true;
    if (d->mDetMode != USB_DETMODE_AON)
    {
        osiTimerStop(d->mTimerDebounce);
        OSI_ASSERT(d->mState <= DrvUsbState::VBUS_STABLE, "invalid usb state when enable");
        if (d->mState != DrvUsbState::IDLE)
        {
            d->changeState(DrvUsbState::DEBOUNCE);
            _vbusDebounceStart(d);
        }
    }

    if (d->readyStart())
        _startUsb(d);

    d->unlock();
}

void drvUsbDisable()
{
    auto d = gDrvUsb;
    if (!d->mEnable)
        return;
    d->lock();
    d->mEnable = false;
    _stopUsbBySoft(d);
    d->unlock();
}

bool drvUsbIsEnabled()
{
    return gDrvUsb->mEnable;
}

bool drvUsbRemoteWakeup()
{
    auto d = gDrvUsb;
    if (d->mState == DrvUsbState::CONNECTED)
    {
        return udcRemoteWakeup(d->mUdc);
    }
    return false;
}

void drvUsbSetAttach(bool attach)
{
    auto d = gDrvUsb;
    if (d->mDetMode == USB_DETMODE_AON)
        return;

    if (attach)
    {
        d->changeState(DrvUsbState::DEBOUNCE);
        _vbusDebounceStart(d);
    }
    else
    {
        _chargerOff(d);
    }
}
