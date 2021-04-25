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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('D', 'R', 'N', 'G')

#include "hal_config.h"
#include "hal_chip.h"
#include "osi_api.h"
#include "osi_log.h"
#include <string.h>
#include <stdlib.h>

typedef struct
{
    bool (*rng_gen)(void *buf, unsigned len);
    osiMutex_t *lock;
} drvRng_t;

static bool drvRngGenerateInit(void *buf, unsigned len);
static bool drvRngGenerate_(void *buf, unsigned len);

static drvRng_t gDrvRng = {
    .rng_gen = drvRngGenerateInit,
    .lock = NULL,
};

#ifdef CONFIG_SOC_8910

static bool prvAesTrngGen2(uint32_t *d0, uint32_t *d1, uint32_t tous, uint32_t retry_count)
{
    osiElapsedTimer_t elapsed;
retry:
    halAesTrngEnable();
    osiElapsedTimerStart(&elapsed);
    while (osiElapsedTimeUS(&elapsed) < tous)
    {
        if (halAesCheckTrngComplete())
        {
            halAesTrngGetValue(d0, d1);
            return true;
        }
        osiThreadSleepUS(1);
    }

    if (retry_count--)
    {
        halAesTrngReset();
        osiThreadSleepUS(300);
        goto retry;
    }

    return false;
}

static bool prvAesTrngGenerate(void *buf, unsigned len)
{
    halAesTrngInit();
    halAesTrngReset();
    osiThreadSleepUS(300);
    uint32_t tmp[2];
    bool result = false;
    uint8_t lead = (uint32_t)buf % 8;
    if (lead != 0)
    {
        result = prvAesTrngGen2(&tmp[0], &tmp[1], 3000, 3);
        if (!result)
        {
            OSI_LOGE(0, "AES TRNG lead gen fail %u", lead);
            return false;
        }
        memcpy(buf, tmp, lead);
    }
    uint32_t *data = (uint32_t *)((uint32_t)buf & ~7);
    uint32_t l8 = (len - lead) / 8;
    for (unsigned i = 0; i < 2 * l8; i += 2)
    {
        result = prvAesTrngGen2(&data[i], &data[i + 1], 3000, 3);
        if (!result)
        {
            OSI_LOGE(0, "AES TRNG gen fail %u/%u", i, l8);
            return false;
        }
    }

    uint8_t tail = (len - lead) % 8;
    if (tail != 0)
    {
        result = prvAesTrngGen2(&tmp[0], &tmp[1], 3000, 3);
        if (!result)
        {
            OSI_LOGE(0, "AES TRNG tail gen fail %u", tail);
            return false;
        }
        memcpy(data + 2 * l8, tmp, tail);
    }

    return true;
}

#endif

static bool drvRngGenerateInit(void *buf, unsigned len)
{
    uint32_t critical = osiEnterCritical();
    drvRng_t *d = &gDrvRng;
    d->lock = osiMutexCreate();
    if (d->lock != NULL)
        d->rng_gen = drvRngGenerate_;
    osiExitCritical(critical);
    if (d->lock == NULL)
        return false;
    return drvRngGenerate_(buf, len);
}

static bool drvRngGenerate_(void *buf, unsigned len)
{
    drvRng_t *d = &gDrvRng;
    osiMutexLock(d->lock);
    bool result = false; // TODO other chip
#ifdef CONFIG_SOC_8910
    result = prvAesTrngGenerate(buf, len);
#endif
    osiMutexUnlock(d->lock);

    return result;
}

bool drvRngGenerate(void *buf, unsigned len)
{
    return gDrvRng.rng_gen(buf, len);
}