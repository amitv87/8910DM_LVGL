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

#include "osi_log.h"
#include "diag_at.h"
#include "at_engine_imp.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    atDevice_t ops; // API
    diagAt_t *diag;
} atDeviceDiag_t;

static void _dataComing(void *atd_)
{
    atDeviceDiag_t *atd = (atDeviceDiag_t *)atd_;
    atDispatchReadLater(atd->ops.recv);
}

static void _destroy(atDevice_t *th)
{
    atDeviceDiag_t *atd = (atDeviceDiag_t *)th;
    diagAtDestroy(atd->diag);
    free(atd);
}

static bool _open(atDevice_t *th)
{
    atDeviceDiag_t *atd = (atDeviceDiag_t *)th;
    return diagAtOpen(atd->diag);
}

static void _close(atDevice_t *th)
{
    atDeviceDiag_t *atd = (atDeviceDiag_t *)th;
    diagAtClose(atd->diag);
}

static int _write(atDevice_t *th, const void *data, size_t length)
{
    if (length == 0)
        return 0;

    if (data == NULL)
        return -1;

    atDeviceDiag_t *atd = (atDeviceDiag_t *)th;
    int len = diagAtWrite(atd->diag, data, length);
    if (len < 0)
        OSI_LOGE(0x1000526a, "Diag AT write error");
    else
        OSI_LOGI(0x1000526b, "Diag AT write %d, done %d", length, len);

    return len;
}

static int _read(atDevice_t *th, void *buf, size_t size)
{
    if (size == 0)
        return 0;

    if (buf == NULL)
        return -1;

    atDeviceDiag_t *atd = (atDeviceDiag_t *)th;
    int len = diagAtRead(atd->diag, buf, size);
    if (len < 0)
        OSI_LOGE(0x1000526c, "Diag AT read error");
    else
        OSI_LOGI(0x1000526d, "Diag AT read %u, got %d", size, len);

    return len;
}

atDevice_t *atDeviceDiagCreate()
{
    atDeviceDiag_t *atd = (atDeviceDiag_t *)calloc(1, sizeof(atDeviceDiag_t));
    if (atd == NULL)
        return NULL;

    diagAtCfg_t cfg = {
        .rxfifo_size = 5120,
        .notify = _dataComing,
        .context = atd,
    };

    diagAt_t *diag = diagAtCreate(&cfg);
    if (diag == NULL)
        goto fail;

    atd->diag = diag;
    atd->ops.destroy = _destroy;
    atd->ops.open = _open;
    atd->ops.close = _close;
    atd->ops.write = _write;
    atd->ops.read = _read;
    atd->ops.isReady = false;

    return (atDevice_t *)atd;

fail:
    free(atd);
    return NULL;
}
