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

#include "block_device.h"
#include <errno.h>

int blockDeviceRead(blockDevice_t *dev, uint64_t nr, int count, void *buf)
{
    if (dev->ops.read == NULL)
        return -ENOSYS;
    return dev->ops.read(dev, nr, count, buf);
}

int blockDeviceWrite(blockDevice_t *dev, uint64_t nr, int count, const void *data)
{
    if (dev->ops.write == NULL)
        return -ENOSYS;
    return dev->ops.write(dev, nr, count, data);
}

int blockDeviceErase(blockDevice_t *dev, uint64_t nr, int count)
{
    if (dev->ops.erase == NULL)
        return -ENOSYS;
    return dev->ops.erase(dev, nr, count);
}

void blockDeviceDestroy(blockDevice_t *dev)
{
    if (dev->ops.destroy == NULL)
        return;
    dev->ops.destroy(dev);
}

void blockDeviceStat(blockDevice_t *dev, blockDeviceStat_t *stat)
{
    if (dev->ops.stat != NULL)
        dev->ops.stat(dev, stat);
}
