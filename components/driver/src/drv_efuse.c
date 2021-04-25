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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('E', 'F', 'U', 'S')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include <sys/queue.h>
#include <stdlib.h>
#include "drv_efuse.h"
#include "hal_config.h"
#include "hal_efuse.h"
#include "osi_api.h"
#include "osi_log.h"
#include "osi_byte_buf.h"
#include <stdarg.h>

#ifdef CONFIG_SOC_8910
#define EFUSE_SINGLE_BIT_COUNT (256)
#elif defined(CONFIG_SOC_8811)
#define EFUSE_SINGLE_BIT_COUNT (64)
#endif

#define EFUSE_DOUBLE_BIT_COUNT (EFUSE_SINGLE_BIT_COUNT / 2)

typedef struct
{
    bool init;
    osiMutex_t *lock;
    unsigned read_count;
    uint32_t single_bit_data[EFUSE_SINGLE_BIT_COUNT];
    uint32_t double_bit_data[EFUSE_DOUBLE_BIT_COUNT];
    uint32_t single_bit_bitmap[(EFUSE_SINGLE_BIT_COUNT + 31) / 32];
    uint32_t double_bit_bitmap[(EFUSE_DOUBLE_BIT_COUNT + 31) / 32];
} drvEfuseContext_t;

static drvEfuseContext_t gDrvEfuseCtx;

/**
 * Access lock, and initialize context if needed
 */
static void prvEfuseAccessLock(void)
{
    drvEfuseContext_t *d = &gDrvEfuseCtx;

    uint32_t critical = osiEnterCritical();
    if (!d->init)
    {
        d->lock = osiMutexCreate();
        d->init = true;
    }
    osiExitCritical(critical);

    osiMutexLock(d->lock);
}

/**
 * Access unlock
 */
static inline void prvEfuseAccessUnlock(void)
{
    drvEfuseContext_t *d = &gDrvEfuseCtx;
    osiMutexUnlock(d->lock);
}

/**
 * Batch read
 */
bool drvEfuseBatchRead(bool double_block, uint32_t index, uint32_t *value, ...)
{
    bool (*read_func)(uint32_t index, uint32_t * value);
    unsigned efuse_count;
    uint32_t *bitmap;
    uint32_t *cache_data;

    drvEfuseContext_t *d = &gDrvEfuseCtx;
    if (double_block)
    {
        read_func = halEfuseDoubleRead;
        efuse_count = EFUSE_DOUBLE_BIT_COUNT;
        bitmap = d->double_bit_bitmap;
        cache_data = d->double_bit_data;
    }
    else
    {
        read_func = halEfuseRead;
        efuse_count = EFUSE_SINGLE_BIT_COUNT;
        bitmap = d->single_bit_bitmap;
        cache_data = d->single_bit_data;
    }

    va_list ap;
    va_start(ap, value);

    bool result = true;
    bool opened = false;
    prvEfuseAccessLock();
    while (index != DRV_EFUSE_ACCESS_END)
    {
        if (index >= efuse_count)
        {
            result = false;
            break;
        }

        if (osiBitmapIsSet(bitmap, index))
        {
            *value = cache_data[index];
        }
        else
        {
            if (!opened)
            {
                // delayed open, call hal open only if really needed
                halEfuseOpen();
                opened = true;
            }

            if (!read_func(index, value))
            {
                result = false;
                break;
            }

            OSI_LOGI(0, "efuse read double/%d index/%d value/0x%08x",
                     double_block, index, *value);

            d->read_count++;
            cache_data[index] = *value;
            osiBitmapSet(bitmap, index);
        }

        index = va_arg(ap, uint32_t);
        value = va_arg(ap, uint32_t *); // this may get one more word, but harmless
    }

    if (opened)
        halEfuseClose();
    prvEfuseAccessUnlock();
    va_end(ap);
    return result;
}

/**
 * Batch write
 */
bool drvEfuseBatchWrite(bool double_block, uint32_t index, uint32_t value, ...)
{
    bool (*write_func)(uint32_t index, uint32_t value);
    unsigned efuse_count;
    uint32_t *bitmap;

    drvEfuseContext_t *d = &gDrvEfuseCtx;
    if (double_block)
    {
        write_func = halEfuseDoubleWrite;
        efuse_count = EFUSE_DOUBLE_BIT_COUNT;
        bitmap = d->double_bit_bitmap;
    }
    else
    {
        write_func = halEfuseWrite;
        efuse_count = EFUSE_SINGLE_BIT_COUNT;
        bitmap = d->single_bit_bitmap;
    }

    va_list ap;
    va_start(ap, value);

    bool result = true;
    prvEfuseAccessLock();
    halEfuseOpen();
    while (index != DRV_EFUSE_ACCESS_END)
    {
        OSI_LOGI(0, "efuse write double/%d index/%d value/0x%08x",
                 double_block, index, value);

        if (index >= efuse_count)
        {
            result = false;
            break;
        }

        if (!write_func(index, value))
        {
            result = false;
            break;
        }

        osiBitmapClear(bitmap, index);

        index = va_arg(ap, uint32_t);
        value = va_arg(ap, uint32_t); // this may get one more word, but harmless
    }

    halEfuseClose();
    prvEfuseAccessUnlock();
    va_end(ap);
    return result;
}

/**
 * Read one word
 */
bool drvEfuseRead(bool double_block, uint32_t index, uint32_t *value)
{
    return drvEfuseBatchRead(double_block, index, value, DRV_EFUSE_ACCESS_END);
}

/**
 * Write one word
 */
bool drvEfuseWrite(bool double_block, uint32_t index, uint32_t value)
{
    return drvEfuseBatchWrite(double_block, index, value, DRV_EFUSE_ACCESS_END);
}

/**
 * Invalidate all cache
 */
void drvEfuseInvalidateCache(void)
{
    drvEfuseContext_t *d = &gDrvEfuseCtx;
    prvEfuseAccessLock();
    memset(d->single_bit_bitmap, 0, sizeof(d->single_bit_bitmap));
    memset(d->double_bit_bitmap, 0, sizeof(d->double_bit_bitmap));
    prvEfuseAccessUnlock();
}

/**
 * Fake write, just set cache and not to access real hardware
 */
void drvEfuseFakeWrite(bool double_block, uint32_t index, uint32_t value)
{
    drvEfuseContext_t *d = &gDrvEfuseCtx;
    prvEfuseAccessLock();
    if (double_block && index < EFUSE_DOUBLE_BIT_COUNT)
    {
        d->double_bit_data[index] = value;
        osiBitmapSet(d->double_bit_bitmap, index);
    }
    if (!double_block && index < EFUSE_SINGLE_BIT_COUNT)
    {
        d->single_bit_data[index] = value;
        osiBitmapSet(d->single_bit_bitmap, index);
    }
    prvEfuseAccessUnlock();
}

/**
 * Show cache info by log (hidden API)
 */
void drvEfuseDumpCache(void)
{
    drvEfuseContext_t *d = &gDrvEfuseCtx;
    prvEfuseAccessLock();

    for (int n = 0; n < EFUSE_SINGLE_BIT_COUNT; n++)
    {
        if (!osiBitmapIsSet(d->single_bit_bitmap, n))
            continue;

        OSI_LOGI(0, "efuse s%d 0x%08x", n, d->single_bit_data[n]);
    }

    for (int n = 0; n < EFUSE_DOUBLE_BIT_COUNT; n++)
    {
        if (!osiBitmapIsSet(d->double_bit_bitmap, n))
            continue;

        OSI_LOGI(0, "efuse d%d 0x%08x", n, d->double_bit_data[n]);
    }

    OSI_LOGI(0, "efuse read count %d", d->read_count);
    prvEfuseAccessUnlock();
}
