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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('H', 'E', 'A', 'D')
//#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE

#include "drv_config.h"
#ifdef CONFIG_HEADSET_DETECT_SUPPORT
#include "drv_headset.h"
#include "drv_pmic_intr.h"
#include "drv_pmic_codec.h"
#include "stdio.h"
#include "osi_api.h"
#include "osi_log.h"
#include "audio_device.h"
#include <string.h>

#define DRV_HEADSET_WQ_PRIO OSI_PRIORITY_HIGH
#define SCI_GetTickCount() (uint32_t) osiUpTime()

struct drvHeadsetState
{
    drvHeadsetStus_t status;
    uint32_t button_status;
    uint32_t time;
};

struct drvHeadsetContext
{
    drvHeadsetType_t type;
    osiWorkQueue_t *wq;
    osiWork_t *hdetect_work;
    osiWork_t *hbtndetect_work;
    osiTimer_t *shakingTimer;
    drvHeadsetNotifyCB_t notify_cb;
    void *notify_cb_ctx;
    bool headset_flag;
    HEADSET_T s_headset_ctl;
};

static drvHeadsetContext_t gDrvHeadsetCtx;

#define HEADSET_DETECT_DEBOUNCE_TIME 300
#define HEADSET_BUTTON_DEBOUNCE_TIME 100

static void prvHButtonDetectWork(void *ctx)
{
    drvHeadsetContext_t *d = ctx;
    uint32_t hmic_channel_adc_value;

    if (drvPmicAudHeadButtonPressConfirm())
    {
        hmic_channel_adc_value = drvPmicAudGetHmicAdcValue(0);
        if (d->notify_cb != NULL)
            d->notify_cb(d->notify_cb_ctx, MSG_HEADSET_BTN_DOWN, hmic_channel_adc_value);
    }
    else
    {

        hmic_channel_adc_value = drvPmicAudGetHmicAdcValue(1);
        if (d->notify_cb != NULL)
            d->notify_cb(d->notify_cb_ctx, MSG_HEADSET_BTN_UP, hmic_channel_adc_value);
    }
}

static void drvHButtonIsrCB(void *ctx)
{
    drvHeadsetContext_t *d = (drvHeadsetContext_t *)ctx;
    if (drvPmicAudHeadButtonPressConfirm())
    {
        drvPmicEicTrigger(DRV_PMIC_EIC_AUDIO_HEAD_BUTTON, HEADSET_BUTTON_DEBOUNCE_TIME, false);
    }
    else
    {
        drvPmicEicTrigger(DRV_PMIC_EIC_AUDIO_HEAD_BUTTON, HEADSET_BUTTON_DEBOUNCE_TIME, true);
    }
    osiWorkEnqueue(d->hbtndetect_work, d->wq);
}

static void prvHeadsetDetectWork(void *ctx)
{
    drvHeadsetContext_t *d = ctx;
    if (d->headset_flag)
    {
        if (d->s_headset_ctl.status == HEADSET_DISCONNECT)
        {
            d->s_headset_ctl.status = HEADSET_CONNECT;
            __sprd_codec_set_headset_connection_status(true);
            d->type = __sprd_codec_headset_type_detect();
            __sprd_codec_headset_related_oper(d->type);
            if ((d->type == DRV_HEADSET_TYPE_4POLE_NORMAL))
            {
                drvPmicEicTrigger(DRV_PMIC_EIC_AUDIO_HEAD_BUTTON, HEADSET_BUTTON_DEBOUNCE_TIME, true);
            }
            else
                drvPmicEicDisable(DRV_PMIC_EIC_AUDIO_HEAD_BUTTON);

            if (d->notify_cb != NULL)
                d->notify_cb(d->notify_cb_ctx, MSG_HEADSET_PLUGIN, d->type);
        }
    }
    else
    {
        if (d->s_headset_ctl.status == HEADSET_CONNECT)
        {
            d->s_headset_ctl.status = HEADSET_DISCONNECT;

            __sprd_codec_set_headset_connection_status(false);
            d->type = DRV_HEADSET_TYPE_UNKNOWN_MIC_TYPE;
            drvPmicEicDisable(DRV_PMIC_EIC_AUDIO_HEAD_BUTTON);

            if (d->notify_cb != NULL)
                d->notify_cb(d->notify_cb_ctx, MSG_HEADSET_PLUGOUT, 0);
        }
    }

    if (HEADSET_CONNECT == d->s_headset_ctl.status)
    {
        d->s_headset_ctl.time = SCI_GetTickCount();
    }
    else
    {
        d->s_headset_ctl.time = 0;
    }
}

static void drvHeadsetShakingTimerHandler(void *ctx)
{
    drvHeadsetContext_t *d = (drvHeadsetContext_t *)ctx;
    osiTimerStop(d->shakingTimer);
    if (d->headset_flag)
    {
        if (d->s_headset_ctl.status == HEADSET_DISCONNECT)
        {
            osiWorkEnqueue(d->hdetect_work, d->wq);
        }
    }
    else
    {
        if (d->s_headset_ctl.status == HEADSET_CONNECT)
        {
            osiWorkEnqueue(d->hdetect_work, d->wq);
        }
    }
    return;
}

static void drvHeadsetPlugIsrCB(void *ctx)
{
    drvHeadsetContext_t *d = (drvHeadsetContext_t *)ctx;
    if (drvPmicAudHeadsetInsertConfirm())
    {
        d->headset_flag = true;
        //set headset plugout trigger
        drvPmicEicTrigger(DRV_PMIC_EIC_AUDIO_HEAD_INSERT_ALL, 5, false);
    }
    else
    {
        d->headset_flag = false;
        //set headset plugin trigger
        drvPmicEicTrigger(DRV_PMIC_EIC_AUDIO_HEAD_INSERT_ALL, HEADSET_DETECT_DEBOUNCE_TIME, true);
    }
    osiTimerStop(d->shakingTimer);
    if (d->headset_flag)
        osiTimerStart(d->shakingTimer, HEADSET_DETECT_DEBOUNCE_TIME);
    else
        osiTimerStart(d->shakingTimer, 2);
}

void drvHandsetSetNotifyCustCB(drvHeadsetNotifyCB_t cb, void *ctx)
{
    drvHeadsetContext_t *d = &gDrvHeadsetCtx;
    d->notify_cb = cb;
    d->notify_cb_ctx = ctx;
}

void drvHandsetInit(void)
{
    drvHeadsetContext_t *d = &gDrvHeadsetCtx;
    OSI_LOGXD(OSI_LOGPAR_SI, 0, "[%s][%d] enter\n", __FUNCTION__, __LINE__);
    d->wq = osiWorkQueueCreate("headset", 1, DRV_HEADSET_WQ_PRIO, CONFIG_HEADSET_STACK_SIZE);
    d->hdetect_work = osiWorkCreate(prvHeadsetDetectWork, NULL, d);
    d->shakingTimer = osiTimerCreate(NULL, drvHeadsetShakingTimerHandler, d);
    d->hbtndetect_work = osiWorkCreate(prvHButtonDetectWork, NULL, d);

    //Pmic HeadsetDect Reg Init
    drvPmicAudHeadsetDectRegInit();

    //disable headset irq
    drvPmicEicDisable(DRV_PMIC_EIC_AUDIO_HEAD_INSERT);
    drvPmicEicDisable(DRV_PMIC_EIC_AUDIO_HEAD_INSERT2);
    drvPmicEicDisable(DRV_PMIC_EIC_AUDIO_HEAD_INSERT3);
    drvPmicEicDisable(DRV_PMIC_EIC_AUDIO_HEAD_BUTTON);
    //set headset plugin out isrCB
    drvPmicEicSetCB(DRV_PMIC_EIC_AUDIO_HEAD_INSERT_ALL, drvHeadsetPlugIsrCB, d);
    //set headset button press isrCB
    drvPmicEicSetCB(DRV_PMIC_EIC_AUDIO_HEAD_BUTTON, drvHButtonIsrCB, d);
    //dummyCustCb
    drvHandsetSetNotifyCustCB(NULL, d);

    //poweron check if the headset is plugin
    if (drvPmicAudHeadsetInsertConfirm())
    {
        //set headset plugout trigger
        drvPmicEicTrigger(DRV_PMIC_EIC_AUDIO_HEAD_INSERT_ALL, HEADSET_DETECT_DEBOUNCE_TIME, false);
        d->s_headset_ctl.status = HEADSET_CONNECT;
        d->s_headset_ctl.time = SCI_GetTickCount();
    }
    else
    {
        drvPmicEicTrigger(DRV_PMIC_EIC_AUDIO_HEAD_INSERT_ALL, HEADSET_DETECT_DEBOUNCE_TIME, true);
        d->s_headset_ctl.status = HEADSET_DISCONNECT;
    }

    __sprd_codec_set_headset_connection_status(d->s_headset_ctl.status);

    if (HEADSET_CONNECT == d->s_headset_ctl.status)
    {
        d->type = __sprd_codec_headset_type_detect();
        __sprd_codec_headset_related_oper(d->type);
        if (d->notify_cb != NULL)
            d->notify_cb(d->notify_cb_ctx, MSG_HEADSET_PLUGIN, d->type);
    }
    else
    {
        if (d->notify_cb != NULL)
            d->notify_cb(d->notify_cb_ctx, MSG_HEADSET_PLUGOUT, 0);
    }
}

void drvGetHeadsetStatus(drvHeadSetStatus_t *status)
{
    drvHeadsetContext_t *d = &gDrvHeadsetCtx;
    status->isplugin = d->s_headset_ctl.status;
    status->mictype = d->type;
}
#endif
