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

#include "boot_platform.h"
#include "boot_spi_flash.h"

// osi mutex dummy functions
typedef struct osiMutex osiMutex_t;
osiMutex_t *osiMutexCreate(void) { return (osiMutex_t *)1; }
void osiMutexDelete(osiMutex_t *mutex) {}
void osiMutexLock(osiMutex_t *mutex) {}
void osiMutexUnlock(osiMutex_t *mutex) {}

// drvFlash api
typedef struct drvSpiFlash drvSpiFlash_t;

drvSpiFlash_t *drvSpiFlashOpen(uint32_t name)
{
    return (drvSpiFlash_t *)bootSpiFlashOpen(name);
}

const void *drvSpiFlashMapAddress(drvSpiFlash_t *d, uint32_t offset)
{
    bootSpiFlash_t *flash = (bootSpiFlash_t *)d;
    return bootSpiFlashMapAddress(flash, offset);
}

bool drvSpiFlashWrite(drvSpiFlash_t *d, uint32_t offset, const void *data, size_t size)
{
    bootSpiFlash_t *flash = (bootSpiFlash_t *)d;
    return bootSpiFlashWrite(flash, offset, data, size);
}

bool drvSpiFlashErase(drvSpiFlash_t *d, uint32_t offset, size_t size)
{
    bootSpiFlash_t *flash = (bootSpiFlash_t *)d;
    return bootSpiFlashErase(flash, offset, size);
}

bool drvSpiFlashRead(drvSpiFlash_t *d, uint32_t offset, void *data, uint32_t size)
{
    bootSpiFlash_t *flash = (bootSpiFlash_t *)d;
    return bootSpiFlashRead(flash, offset, data, size);
}

bool drvSpiFlashReadCheck(drvSpiFlash_t *d, uint32_t offset, const void *data, uint32_t size)
{
    bootSpiFlash_t *flash = (bootSpiFlash_t *)d;
    return bootSpiFlashReadCheck(flash, offset, data, size);
}

uint32_t drvSpiFlashCapacity(drvSpiFlash_t *d)
{
    bootSpiFlash_t *flash = (bootSpiFlash_t *)d;
    return bootSpiFlashCapacity(flash);
}
