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

//#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('S', 'H', 'E', 'D')
#include "drv_config.h"
#include "srv_headset.h"
#include "osi_api.h"
#include "osi_log.h"
#include <stdlib.h>
#include <sys/queue.h>
#include <assert.h>
#include "audio_types.h"
#include "audio_device.h"
#include "stdio.h"

typedef LIST_ENTRY(srv_headset_listener) srvHeadsetLsnerIter_t;
typedef LIST_HEAD(srv_headset_head, srv_headset_listener) srvHeadsetLsnerHead_t;
struct srv_headset_listener
{
    srvHeadsetLsnerIter_t iter;
    uint32_t id;
    headsetHook_t bfunc; //before audio path switch ,app callback function
    headsetHook_t afunc; //after audio path switch ,app callback function
    void *ctx;
};

typedef struct
{
    srvHeadsetLsnerHead_t lsners;
} srvHeadset_t;

typedef struct
{
    osiMutex_t *lock;
    srvHeadsetLsn_t *hs_in_lsn;
    srvHeadsetLsn_t *hs_out_lsn;
    srvHeadset_t *actions[MSG_HEADSET_MAX_COUNT];
} srvHeadsetMgr_t;

static srvHeadsetMgr_t *gHeadsetMgr = NULL;

static void prvSetAudioInOutPath(drvHeadsetNotifyMsg_t id, uint32_t param)
{
    char rsp[64];
    memset(rsp, 0, sizeof(rsp));

    if ((id != MSG_HEADSET_PLUGIN) && (id != MSG_HEADSET_PLUGOUT))
        return;
    if (id == MSG_HEADSET_PLUGIN)
    {
        if (param == DRV_HEADSET_TYPE_NO_MIC)
            sprintf(rsp, "+HEADSET IN: TYPE 3POLE NO MIC");
        else if (param == DRV_HEADSET_TYPE_4POLE_NORMAL)
            sprintf(rsp, "+HEADSET IN: TYPE 4POLE NORMAL");
        else if (param == DRV_HEADSET_TYPE_4POLE_NOT_NORMAL)
            sprintf(rsp, "+HEADSET IN: TYPE 4POLE NOT NORMAL");
        else if (param == DRV_HEADSET_TYPE_ERR)
            sprintf(rsp, "+HEADSET IN: TYPE UNKNOWN");

        OSI_LOGXI(OSI_LOGPAR_S, 0, "%s\n", rsp);

        if (param == DRV_HEADSET_TYPE_4POLE_NORMAL)
            audevSetInput(AUDEV_INPUT_HPMIC_L);
        else
            audevSetInput(AUDEV_INPUT_MAINMIC);
        audevSetOutput(AUDEV_OUTPUT_HEADPHONE);
    }
    else if (id == MSG_HEADSET_PLUGOUT)
    {
        sprintf(rsp, "+HEADSET OUT");
        OSI_LOGXI(OSI_LOGPAR_S, 0, "%s\n", rsp);
        audevSetInput(AUDEV_INPUT_MAINMIC);
        audevSetOutput(AUDEV_OUTPUT_SPEAKER);
    }
    else if (id == MSG_HEADSET_BTN_DOWN)
    {
        sprintf(rsp, "+HEADBTN DOWN: %d", (int)param);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "%s\n", rsp);
    }
    else if (id == MSG_HEADSET_BTN_UP)
    {
        sprintf(rsp, "+HEADBTN UP: %d", (int)param);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "%s\n", rsp);
    }
}

#ifdef CONFIG_HEADSET_DETECT_SUPPORT
static void prvHeadsetSrvCB(void *p, drvHeadsetNotifyMsg_t id, uint32_t param)
{
    if (id >= MSG_HEADSET_MAX_COUNT)
        return;

    srvHeadsetMgr_t *km = (srvHeadsetMgr_t *)p;
    srvHeadset_t *k = km->actions[id];
    OSI_LOGD(0, "srvheadset id/%d param/%x", id, param);
    if (k == NULL)
        return;

    if (!LIST_EMPTY(&k->lsners))
    {
        osiMutexLock(km->lock);
        srvHeadsetLsn_t *lsn;
        LIST_FOREACH(lsn, &k->lsners, iter)
        {
            if (lsn->bfunc)
                lsn->bfunc(id, param, lsn->ctx);
        }
        prvSetAudioInOutPath(id, param);
        LIST_FOREACH(lsn, &k->lsners, iter)
        {
            if (lsn->afunc)
                lsn->afunc(id, param, lsn->ctx);
        }
        osiMutexUnlock(km->lock);
    }
}
#endif
srvHeadsetLsn_t *srvHeadsetListen(drvHeadsetNotifyMsg_t id, headsetHook_t bfunc, headsetHook_t afunc, void *ctx)
{
    srvHeadsetMgr_t *km = gHeadsetMgr;
    if (id >= MSG_HEADSET_MAX_COUNT || km == NULL)
        return NULL;

    osiMutexLock(km->lock);
    srvHeadsetLsn_t *lsn = NULL;
    srvHeadset_t *k = km->actions[id];
    if (k == NULL)
    {
        k = (srvHeadset_t *)calloc(1, sizeof(*k));
        km->actions[id] = k;
    }

    if (k)
    {
        lsn = calloc(1, sizeof(*lsn));
        if (lsn == NULL)
        {
            if (LIST_EMPTY(&k->lsners))
            {
                km->actions[id] = NULL;
                free(k);
            }
        }
        else
        {
            lsn->bfunc = bfunc;
            lsn->afunc = afunc;
            lsn->ctx = ctx;
            lsn->id = id;
            LIST_INSERT_HEAD(&k->lsners, lsn, iter);
        }
    }

    osiMutexUnlock(km->lock);
    return lsn;
}

bool srvHeadsetListenBreak(srvHeadsetLsn_t *lsn)
{
    srvHeadsetMgr_t *km = gHeadsetMgr;
    if (km == NULL || lsn == NULL || lsn->id >= MSG_HEADSET_MAX_COUNT)
        return false;

    osiMutexLock(km->lock);
    bool result = false;
    uint32_t id = lsn->id;
    srvHeadset_t *k = km->actions[id];
    if (k)
    {
        bool des = false;
        uint32_t critical = osiEnterCritical();
        LIST_REMOVE(lsn, iter);
        if (LIST_EMPTY(&k->lsners))
        {
            km->actions[id] = NULL;
            des = true;
        }
        osiExitCritical(critical);

        result = true;
        free(lsn);
        if (des)
            free(k);
    }
    osiMutexUnlock(km->lock);
    return result;
}

static void prvDefaultBeforeHeadsetActionCB(drvHeadsetNotifyMsg_t id, uint32_t param, void *ctx)
{
    OSI_LOGI(0, "Default CB Before HeadsetAction,id/%d,param/%d ", id, param);
}

static void prvDefaultAfterHeadsetActionCB(drvHeadsetNotifyMsg_t id, uint32_t param, void *ctx)
{
    OSI_LOGI(0, "Default CB After HeadsetAction,id/%d,param/%d ", id, param);
}

void srvHeadsetInit()
{
    if (gHeadsetMgr != NULL)
        return;

    srvHeadsetMgr_t *hm = calloc(1, sizeof(*hm));
    bool result = false;
    if (hm)
    {
        do
        {
            hm->lock = osiMutexCreate();
            if (!hm->lock)
                break;

#ifdef CONFIG_HEADSET_DETECT_SUPPORT
            drvHandsetInit();
            drvHandsetSetNotifyCustCB(prvHeadsetSrvCB, hm);
#endif
            gHeadsetMgr = hm;
            result = true;
        } while (0);

        if (!result)
        {
            osiMutexDelete(hm->lock);
            free(hm);
        }
    }

    if (!result)
    {
        OSI_LOGE(0, "fail init headset srv");
        osiPanic();
    }
    else
    {
        drvHeadSetStatus_t status;
        drvGetHeadsetStatus(&status);
        drvHeadsetNotifyMsg_t id = (status.isplugin) ? MSG_HEADSET_PLUGIN : MSG_HEADSET_PLUGOUT;
        uint32_t param = status.mictype;
        prvSetAudioInOutPath(id, param);
        gHeadsetMgr->hs_in_lsn = srvHeadsetListen(MSG_HEADSET_PLUGIN, prvDefaultBeforeHeadsetActionCB, prvDefaultAfterHeadsetActionCB, NULL);
        if (gHeadsetMgr->hs_in_lsn == NULL)
        {
            OSI_LOGE(0, "fail reg default MSG_HEADSET_PLUGIN srvHeadsetListen");
            osiPanic();
        }

        gHeadsetMgr->hs_out_lsn = srvHeadsetListen(MSG_HEADSET_PLUGOUT, prvDefaultBeforeHeadsetActionCB, prvDefaultAfterHeadsetActionCB, NULL);
        if (gHeadsetMgr->hs_out_lsn == NULL)
        {
            OSI_LOGE(0, "fail reg default MSG_HEADSET_PLUGOUT srvHeadsetListen");
            osiPanic();
        }
    }
}
