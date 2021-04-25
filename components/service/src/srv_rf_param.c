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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('R', 'F', 'P', 'R')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "srv_rf_param.h"
#include "osi_log.h"
#include "osi_api.h"
#include "hwregs.h"
#include "vfs.h"
#include "drv_md_ipc.h"
#include <stdlib.h>

#define V0_DEFAULT_VALUE (0xffffffff)
#define V1_DEFAULT_VALUE (0xffffffff)
#define V2_DEFAULT_VALUE (0xffffffff)
#define V3_DEFAULT_VALUE (0xffffffff)
#define V4_DEFAULT_VALUE (0xffffffff)

#define RF_PARAM_FILE_NAME "/modemnvm/rf_param.bin"

#define RFPARAM_LOCK(lock) osiMutexLock(lock)
#define RFPARAM_UNLOCK(lock) osiMutexUnlock(lock)

#define RF_PARAM_TASK_PRIORITY (OSI_PRIORITY_NORMAL)
#define RF_PARAM_STACK_SIZE (512 * 4)

typedef struct
{
    uint32_t v0;
    uint32_t v1;
    uint32_t v2;
    uint32_t v3;
    uint32_t v4;
} rfParam_t;

typedef struct
{
    rfParam_t rf_param;
    osiMutex_t *lock;
    osiWorkQueue_t *work_queue;
    osiWork_t *work;
} svcRfParamContext_t;

static svcRfParamContext_t *gSvcRfParamCtx;

// load rf param file
static bool prvLoadRfParam(svcRfParamContext_t *d)
{
    ssize_t file_size = vfs_sfile_size(RF_PARAM_FILE_NAME);
    if (file_size <= 0)
    {
        OSI_LOGI(0, "rf param file is not existed or invalid");
        return false;
    }

    bool ok = (vfs_sfile_read(RF_PARAM_FILE_NAME, &d->rf_param, sizeof(d->rf_param)) == file_size);
    return ok;
}

// store rf param file
static bool prvStoreRfParam(svcRfParamContext_t *d)
{
    size_t length = sizeof(rfParam_t);

    if (vfs_sfile_write(RF_PARAM_FILE_NAME, &(d->rf_param), length) != length)
        return false;

    OSI_LOGI(0, "rf param store length %d", length);
    return true;
}

// when receive hwp->intcr0 bit 17 irq from cp, or update it, call it
static void prvRfParamUpdate(void *ctx)
{
    svcRfParamContext_t *d = (svcRfParamContext_t *)ctx;

    if (!prvStoreRfParam(d))
    {
        OSI_LOGI(0, "rf param update failed");
    }
}

static void prvRfParamNotify(void *ctx1, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4)
{
    svcRfParamContext_t *d = (svcRfParamContext_t *)ctx1;
    d->rf_param.v0 = v0;
    d->rf_param.v1 = v1;
    d->rf_param.v2 = v2;
    d->rf_param.v3 = v3;
    d->rf_param.v4 = v4;

    osiWorkEnqueueLast(d->work, d->work_queue);
}

bool svcRfParamInit(void)
{
    svcRfParamContext_t *d = (svcRfParamContext_t *)calloc(1, sizeof(*d));
    if (d == NULL)
        return false;

    vfs_sfile_init(RF_PARAM_FILE_NAME);
    if (!prvLoadRfParam(d))
    {
        OSI_LOGI(0, "rf param load failed");

        d->rf_param.v0 = V0_DEFAULT_VALUE;
        d->rf_param.v1 = V1_DEFAULT_VALUE;
        d->rf_param.v2 = V2_DEFAULT_VALUE;
        d->rf_param.v3 = V3_DEFAULT_VALUE;
        d->rf_param.v4 = V4_DEFAULT_VALUE;
    }

    d->lock = osiMutexCreate();
    if (d->lock == NULL)
        goto fail;

    d->work_queue = osiWorkQueueCreate("rfparam", 1, RF_PARAM_TASK_PRIORITY, RF_PARAM_STACK_SIZE);
    if (d->work_queue == NULL)
        goto fail;

    d->work = osiWorkCreate(prvRfParamUpdate, NULL, d);
    if (d->work == NULL)
        goto fail;

    ipc_cpRfParamInit(d->rf_param.v0, d->rf_param.v1, d->rf_param.v2, d->rf_param.v3, d->rf_param.v4);
    ipc_register_rfParam_notify(prvRfParamNotify, d);
    gSvcRfParamCtx = d;

    return true;

fail:
    osiMutexDelete(d->lock);
    osiWorkQueueDelete(d->work_queue);
    osiWorkDelete(d->work);
    free(d);
    return false;
}
