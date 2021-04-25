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

#ifndef _DRV_HEADSET_H_
#define _DRV_HEADSET_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DRV_HEADSET_TYPE_UNKNOWN_MIC_TYPE,
    DRV_HEADSET_TYPE_NO_MIC,
    DRV_HEADSET_TYPE_4POLE_NORMAL,
    DRV_HEADSET_TYPE_4POLE_NOT_NORMAL,
    DRV_HEADSET_TYPE_APPLE,
    DRV_HEADSET_TYPE_ERR
} drvHeadsetType_t;

typedef enum
{
    HEADSET_DISCONNECT = 0,
    HEADSET_CONNECT = 1,
} drvHeadsetStus_t;

typedef enum
{
    MSG_HEADSET_PLUGIN = 1,
    MSG_HEADSET_PLUGOUT = 2,
    MSG_HEADSET_BTN_DOWN = 3,
    MSG_HEADSET_BTN_UP = 4,
    MSG_HEADSET_MAX_COUNT ///< total count
} drvHeadsetNotifyMsg_t;

typedef struct
{
    drvHeadsetStus_t isplugin;
    drvHeadsetType_t mictype;
} drvHeadSetStatus_t;

typedef struct drvHeadsetState HEADSET_T;
typedef struct drvHeadsetContext drvHeadsetContext_t;

/**
 * get the status of headset and mic
 * this function can be called after Handset Init
*/
void drvGetHeadsetStatus(drvHeadSetStatus_t *status);

/**
 * callback function for HeadsetPlug detect work result notify
 *
 * It is called in headset detect work.
 */
typedef void (*drvHeadsetNotifyCB_t)(void *ctx, drvHeadsetNotifyMsg_t id, uint32_t param);

/**
 * customer can set callback function for HeadsetPlugin out event
 *
 *
 */

void drvHandsetSetNotifyCustCB(drvHeadsetNotifyCB_t cb, void *ctx);

/**
 * Handset Init call when system poweron
 *
 */
void drvHandsetInit(void);

#ifdef __cplusplus
}
#endif
#endif
