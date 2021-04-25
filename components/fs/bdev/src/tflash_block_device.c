/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('T', 'B', 'L', 'K')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "tflash_block_device.h"
#include "drv_sdmmc.h"
#include "hal_config.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hal_chip.h"
#include <sys/errno.h>
#include <string.h>
#include <stdlib.h>

#include "quec_proj_config.h"


#define TCARD_SECTOR_SIZE (512)
#define TCARD_CACHE_SECTOR_COUNT (64)

typedef struct
{
    blockDevice_t bdev;
    osiMutex_t *lock;
    drvSdmmc_t *sdmmc;
    void *buf;
} tfBdev_t;

static tfBdev_t *prvBdevToTf(blockDevice_t *bdev)
{
    return (tfBdev_t *)bdev->priv;
}

static int prvTflashRead(blockDevice_t *dev, uint64_t nr, int count, void *data)
{
    OSI_LOGD(0, "read (0x%x) nr/%u count/%d", data, (unsigned)nr, count);

    if (count == 0)
        return 0;
    if (data == NULL || !HAL_ADDR_IS_SRAM_RAM(data))
        return -1;

    tfBdev_t *d = prvBdevToTf(dev);
    if (OSI_IS_ALIGNED(data, CONFIG_CACHE_LINE_SIZE))
    {
        unsigned rsize = count * TCARD_SECTOR_SIZE;
        return drvSdmmcRead(d->sdmmc, (uint32_t)nr, data, rsize) ? count : -1;
    }

    unsigned total_count = count;
    osiMutexLock(d->lock);

    while (count > 0)
    {
        int rcount = OSI_MIN(unsigned, count, TCARD_CACHE_SECTOR_COUNT);
        unsigned rsize = rcount * TCARD_SECTOR_SIZE;
        if (!drvSdmmcRead(d->sdmmc, (uint32_t)nr, d->buf, rsize))
        {
            osiMutexUnlock(d->lock);
            return -1;
        }

        memcpy(data, d->buf, rsize);
        data = (char *)data + rsize;
        count -= rcount;
        nr += rcount;
    }

    osiMutexUnlock(d->lock);
    return total_count;
}

static int prvTflashWrite(blockDevice_t *dev, uint64_t nr, int count, const void *data)
{
    OSI_LOGD(0, "write (0x%x) nr/%u count/%d", data, (unsigned)nr, count);

    if (count == 0)
        return 0;
    if (data == NULL)
        return -1;

    tfBdev_t *d = prvBdevToTf(dev);
    if (HAL_ADDR_IS_SRAM_RAM(data) && OSI_IS_ALIGNED(data, 4))
    {
        unsigned wsize = count * TCARD_SECTOR_SIZE;
        return drvSdmmcWrite(d->sdmmc, (uint32_t)nr, data, wsize) ? count : -1;
    }

    unsigned total_count = count;
    osiMutexLock(d->lock);

    while (count > 0)
    {
        int wcount = OSI_MIN(unsigned, count, TCARD_CACHE_SECTOR_COUNT);
        unsigned wsize = wcount * TCARD_SECTOR_SIZE;
        memcpy(d->buf, data, wsize);
        if (!drvSdmmcWrite(d->sdmmc, (uint32_t)nr, d->buf, wsize))
        {
            osiMutexUnlock(d->lock);
            return -1;
        }

        data = (char *)data + wsize;
        count -= wcount;
        nr += wcount;
    }

    osiMutexUnlock(d->lock);
    return total_count;
}

static void prvTflashDestroy(blockDevice_t *dev)
{
    tfBdev_t *d = prvBdevToTf(dev);
    osiMutexDelete(d->lock);
    drvSdmmcClose(d->sdmmc);
    drvSdmmcDestroy(d->sdmmc);
    free(dev);
}

blockDevice_t *tflash_device_create(uint32_t name)
{
    unsigned buf_size = TCARD_CACHE_SECTOR_COUNT * TCARD_SECTOR_SIZE;
    unsigned alloc_len = sizeof(tfBdev_t) + buf_size + CONFIG_CACHE_LINE_SIZE;
    uintptr_t mem = (uintptr_t)calloc(1, alloc_len);
    if (mem == 0)
        return NULL;

    tfBdev_t *d = (tfBdev_t *)OSI_PTR_INCR_POST(mem, sizeof(tfBdev_t));
    mem = OSI_ALIGN_UP(mem, CONFIG_CACHE_LINE_SIZE);
    d->buf = (void *)mem;

    d->sdmmc = drvSdmmcCreate(name);
    if (d->sdmmc == NULL)
    {
        OSI_LOGE(0, "fail to create %4c sdmmc instance", name);
        goto fail;
    }

//add by sum.li  2021/02/26 
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
    extern uint8_t quec_tflash_device_create_Sdmmc(uint32_t name,drvSdmmc_t *d);
    uint8_t ret = quec_tflash_device_create_Sdmmc(name,d->sdmmc);
    if(1==ret)goto fail;
    else if(2==ret)goto fail_open;
#else    
    if (!drvSdmmcOpen(d->sdmmc))
    {
       OSI_LOGE(0, "fail to open sdmmc");
       goto fail_open;
    }
#endif

    d->lock = osiMutexCreate();
    d->bdev.block_count = drvSdmmcGetBlockNum(d->sdmmc);
    d->bdev.block_size = TCARD_SECTOR_SIZE;
    d->bdev.ops.read = prvTflashRead;
    d->bdev.ops.write = prvTflashWrite;
    d->bdev.ops.destroy = prvTflashDestroy;
    d->bdev.priv = d;
    return &d->bdev;

fail_open:
    drvSdmmcDestroy(d->sdmmc);

fail:
    free(d);
    return NULL;
}
