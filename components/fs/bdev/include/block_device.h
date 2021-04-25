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

#ifndef _BLOCK_DEVICE_H_
#define _BLOCK_DEVICE_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct blockDevice blockDevice_t;

typedef struct
{
    unsigned read_block_count;
    unsigned write_block_count;
    unsigned erase_block_count;
    unsigned min_erase_count;
    unsigned max_erase_count;
} blockDeviceStat_t;

typedef struct
{
    int (*read)(blockDevice_t *dev, uint64_t nr, int count, void *buf);
    int (*write)(blockDevice_t *dev, uint64_t nr, int count, const void *data);
    int (*erase)(blockDevice_t *dev, uint64_t nr, int count);
    void (*stat)(blockDevice_t *dev, blockDeviceStat_t *stat);
    void (*destroy)(blockDevice_t *dev);
} blockDeviceOps_t;

struct blockDevice
{
    blockDeviceOps_t ops;
    size_t block_size;
    uint64_t block_count;
    void *priv;
};

int blockDeviceRead(blockDevice_t *dev, uint64_t nr, int count, void *buf);
int blockDeviceWrite(blockDevice_t *dev, uint64_t nr, int count, const void *data);
int blockDeviceErase(blockDevice_t *dev, uint64_t nr, int count);
void blockDeviceStat(blockDevice_t *dev, blockDeviceStat_t *stat);
void blockDeviceDestroy(blockDevice_t *dev);

#ifdef __cplusplus
}
#endif

#endif
