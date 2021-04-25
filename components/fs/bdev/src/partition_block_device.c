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

#include "partition_block_device.h"
#include <stdlib.h>

typedef struct
{
    blockDevice_t *parent;
    uint64_t offset;
    uint64_t count;
} partBlockDevicePriv_t;

struct partBlockDevice
{
    blockDevice_t dev;
    partBlockDevicePriv_t priv;
};

static int _read(blockDevice_t *dev, uint64_t nr, int count, void *buf)
{
    partBlockDevicePriv_t *p = (partBlockDevicePriv_t *)dev->priv;
    return blockDeviceRead(p->parent, nr + p->offset, count, buf);
}

static int _write(blockDevice_t *dev, uint64_t nr, int count, const void *data)
{
    partBlockDevicePriv_t *p = (partBlockDevicePriv_t *)dev->priv;
    return blockDeviceWrite(p->parent, nr + p->offset, count, data);
}

static int _erase(blockDevice_t *dev, uint64_t nr, int count)
{
    partBlockDevicePriv_t *p = (partBlockDevicePriv_t *)dev->priv;
    return blockDeviceErase(p->parent, nr + p->offset, count);
}

static void _destroy(blockDevice_t *dev)
{
    free(dev);
}

static const blockDeviceOps_t _ops = {
    .read = _read,
    .write = _write,
    .erase = _erase,
    .destroy = _destroy,
};

blockDevice_t *partBlockDeviceCreate(
    blockDevice_t *parent,
    uint64_t offset,
    uint64_t count)
{
    if (parent == NULL)
        return NULL;
    if (offset + count > parent->block_count)
        return NULL;

    struct partBlockDevice *bdev = (struct partBlockDevice *)malloc(sizeof(*bdev));
    if (bdev == NULL)
        return NULL;

    blockDevice_t *dev = &bdev->dev;
    dev->ops = _ops;
    dev->block_size = parent->block_size;
    dev->block_count = count;
    dev->priv = &bdev->priv;

    bdev->priv.parent = parent;
    bdev->priv.offset = offset;
    bdev->priv.count = count;
    return dev;
}
