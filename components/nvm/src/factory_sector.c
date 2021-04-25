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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('F', 'A', 'C', 'T')

#include "factory_sector.h"
#include "nvm.h"
#include "osi_api.h"
#include "drv_spi_flash.h"
#include "calclib/crc32.h"
#include "hal_chip.h"
#include <stdlib.h>
#include <string.h>

#define FACTORY_BLOCK_COUNT (8)
#define FACTORY_SECTOR_SIZE (4096)
#define FACTORY_BLOCK_PER_SECTOR (4)

#define FACTORY_BLOCK_ST_INVALID (0)
#define FACTORY_BLOCK_ST_CURRENT (1)
#define FACTORY_BLOCK_ST_EMPTY (2)

typedef struct
{
    int8_t curr_block;
    uint8_t state[FACTORY_BLOCK_COUNT];
    drvSpiFlash_t *flash;
    osiSemaphore_t *lock;
} factoryContex_t;

static factoryContex_t gFactoryBlockCtx;

static int _factoryBlockSequence(const void *ptr)
{
    const factoryBlock_t *p = (const factoryBlock_t *)ptr;

    if (p->version != FACTORY_VERSION)
        return -1;

    if ((p->sequence & 1) != 0)
        return -1;

    if (p->crc != ~p->crc_inv)
        return -1;

    if (p->crc != crc32VariantCalc(p, OSI_OFFSETOF(factoryBlock_t, crc)))
        return -1;

    return p->sequence;
}

static const void *_blockMapAddress(factoryContex_t *d, int block)
{
    unsigned offset = HAL_FLASH_OFFSET(CONFIG_FACTORY_FLASH_ADDRESS) + block * sizeof(factoryBlock_t);
    return drvSpiFlashMapAddress(d->flash, offset);
}

void factorySectorInit(void)
{
    factoryContex_t *d = &gFactoryBlockCtx;

    d->curr_block = -1;
    d->flash = drvSpiFlashOpen(DRV_NAME_SPI_FLASH);
    d->lock = osiSemaphoreCreate(1, 1);

    int max_seq = 0;
    for (int n = 0; n < FACTORY_BLOCK_COUNT; n++)
    {
        d->state[n] = FACTORY_BLOCK_ST_INVALID;

        const void *fact = _blockMapAddress(d, n);
        int seq = _factoryBlockSequence(fact);
        if (seq > max_seq)
        {
            d->curr_block = n;
            max_seq = seq;
        }
    }

    if (d->curr_block >= 0)
        d->state[d->curr_block] = FACTORY_BLOCK_ST_CURRENT;
}

bool factoryBlockRead(factoryBlock_t *block)
{
    if (block == NULL)
        return false;

    factoryContex_t *d = &gFactoryBlockCtx;

    osiSemaphoreAcquire(d->lock);
    if (d->curr_block < 0)
    {
        osiSemaphoreRelease(d->lock);
        return false;
    }

    const char *fact = (const char *)_blockMapAddress(d, d->curr_block);
    memcpy(block, fact, sizeof(factoryBlock_t));
    osiSemaphoreRelease(d->lock);
    return true;
}

static int _findEmptyBlock(factoryContex_t *d)
{
    for (int n = 0; n < FACTORY_BLOCK_COUNT; n++)
    {
        if (d->state[n] == FACTORY_BLOCK_ST_EMPTY)
            return n;
    }
    return -1;
}

static int _findBlockForWrite(factoryContex_t *d)
{
    int block = _findEmptyBlock(d);
    if (block >= 0)
        return block;

    int erase_sector;
    if (d->curr_block < 0)
        erase_sector = 0;
    else if (d->curr_block < FACTORY_BLOCK_COUNT / 2)
        erase_sector = 1;
    else
        erase_sector = 0;

    unsigned erase_offset = HAL_FLASH_OFFSET(CONFIG_FACTORY_FLASH_ADDRESS) + erase_sector * FACTORY_SECTOR_SIZE;
    drvSpiFlashErase(d->flash, erase_offset, FACTORY_SECTOR_SIZE);

    block = erase_sector * FACTORY_BLOCK_PER_SECTOR;
    for (int n = 0; n < FACTORY_BLOCK_PER_SECTOR; n++)
        d->state[n + block] = FACTORY_BLOCK_ST_EMPTY;

    return block;
}

static void _blockWrite(factoryContex_t *d, factoryBlock_t *block)
{
    int n = _findBlockForWrite(d);
    int sequence = 0;

    if (d->curr_block >= 0)
    {
        const factoryBlock_t *fact = _blockMapAddress(d, d->curr_block);
        sequence = fact->sequence;
    }

    block->version = FACTORY_VERSION;
    block->sequence = sequence + 2;
    block->crc = crc32VariantCalc(block, OSI_OFFSETOF(factoryBlock_t, crc));
    block->crc_inv = ~block->crc;

    d->curr_block = n;
    drvSpiFlashWrite(d->flash, HAL_FLASH_OFFSET(CONFIG_FACTORY_FLASH_ADDRESS) + n * sizeof(factoryBlock_t),
                     block, sizeof(factoryBlock_t));
}

bool factoryBlockWrite(factoryBlock_t *block)
{
    if (block == NULL)
        return false;

    factoryContex_t *d = &gFactoryBlockCtx;

    osiSemaphoreAcquire(d->lock);
    _blockWrite(d, block);
    osiSemaphoreRelease(d->lock);
    return true;
}

bool factoryBlockPartialWrite(size_t offset, const void *data_buf, size_t size)
{
    if (data_buf == NULL || (offset + size) > sizeof(factoryBlock_t))
        return false;

    factoryBlock_t *block = (factoryBlock_t *)malloc(sizeof(factoryBlock_t));
    if (block == NULL)
        return false;

    factoryContex_t *d = &gFactoryBlockCtx;
    osiSemaphoreAcquire(d->lock);

    if (d->curr_block < 0)
    {
        memset(block, 0xff, sizeof(factoryBlock_t));
    }
    else
    {
        const char *fact = _blockMapAddress(d, d->curr_block);
        memcpy(block, fact, sizeof(factoryBlock_t));
    }

    memcpy((char *)block + offset, data_buf, size);
    _blockWrite(d, block);

    osiSemaphoreRelease(d->lock);
    free(block);
    return true;
}

static bool _isImeiValid(const void *imei)
{
    const uint8_t *p = imei;
    if (((p[0] >> 4) & 0xf) > 9)
        return false;

    for (int n = 1; n < 8; n++)
    {
        if (((p[n] >> 0) & 0xf) > 9)
            return false;
        if (((p[n] >> 4) & 0xf) > 9)
            return false;
    }
    return true;
}

bool factoryGetImei(int idx, void *imei)
{
    if ((unsigned)idx > FACTORY_SIM_COUNT || imei == NULL)
        return false;

    factoryContex_t *d = &gFactoryBlockCtx;
    osiSemaphoreAcquire(d->lock);

    if (d->curr_block < 0)
    {
        osiSemaphoreRelease(d->lock);
        return false;
    }

    const factoryBlock_t *fact = _blockMapAddress(d, d->curr_block);
    memcpy(imei, fact->imei_sv[idx], FACTORY_IMEI_LEN);
    osiSemaphoreRelease(d->lock);
    return _isImeiValid(imei);
}

bool factorySetImei(int idx, const void *imei)
{
    if ((unsigned)idx > FACTORY_SIM_COUNT || imei == NULL)
        return false;

    if (!_isImeiValid(imei))
        return false;

    return factoryBlockPartialWrite(OSI_OFFSETOF(factoryBlock_t, imei_sv[0][0]), imei, 8);
}

int nvmReadImei(int idx, nvmImei_t *imei)
{
    if (idx < 0 || idx > FACTORY_SIM_COUNT || imei == NULL)
        return -1;

    uint8_t raw_imei[FACTORY_IMEI_LEN];
    if (!factoryGetImei(idx, raw_imei))
        return -1;

    uint8_t *rd_imei = (uint8_t *)imei;
    *rd_imei++ = ((raw_imei[0] >> 4) & 0xf) + (uint8_t)'0';
    for (int n = 1; n < 8; n++)
    {
        *rd_imei++ = ((raw_imei[n] >> 0) & 0xf) + (uint8_t)'0';
        *rd_imei++ = ((raw_imei[n] >> 4) & 0xf) + (uint8_t)'0';
    }
    return sizeof(nvmImei_t);
}

int nvmWriteImei(int idx, const nvmImei_t *imei)
{
    if (idx < 0 || idx > FACTORY_SIM_COUNT || imei == NULL)
        return -1;

    const uint8_t *pimei = (const uint8_t *)imei;
    for (int n = 0; n < 15; n++)
    {
        if (pimei[n] < '0' || pimei[n] > '9')
            return -1;
    }

    uint8_t raw_imei[FACTORY_IMEI_LEN];
    raw_imei[0] = 0x03 | ((pimei[0] - '0') << 4);
    raw_imei[8] = 0xff;
    for (int n = 1; n < 15; n += 2)
        raw_imei[n / 2 + 1] = (pimei[n] - '0') | ((pimei[n + 1] - '0') << 4);

    factoryBlockPartialWrite(OSI_OFFSETOF(factoryBlock_t, imei_sv[idx][0]), raw_imei, 8);
    return true;
}

static int _snLength(const uint8_t *sn)
{
    for (int n = 0; n < FACTORY_SN_LEN; n++)
    {
        if (sn[n] == 0xff || sn[n] == 0)
            return n;
    }
    return FACTORY_SN_LEN;
}

int nvmReadSN(int idx, void *sn, int size)
{
    if ((unsigned)idx > 2 || sn == NULL)
        return -1;

    factoryContex_t *d = &gFactoryBlockCtx;
    osiSemaphoreAcquire(d->lock);

    if (d->curr_block < 0)
    {
        osiSemaphoreRelease(d->lock);
        return -1;
    }

    const factoryBlock_t *fact = _blockMapAddress(d, d->curr_block);
    if (idx == 0)
    {
        int sn_size = _snLength(fact->mb_sn);
        if (size > sn_size)
            size = sn_size;

        memcpy(sn, fact->mb_sn, size);
    }
    else
    {
        int sn_size = _snLength(fact->mp_sn);
        if (size > sn_size)
            size = sn_size;

        memcpy(sn, fact->mp_sn, size);
    }
    osiSemaphoreRelease(d->lock);
    return size;
}

int nvmWriteSN(int idx, const void *sn, int size)
{
    if ((unsigned)idx > 2 || sn == NULL)
        return -1;

    uint8_t fsn[FACTORY_SN_LEN] = {};
    if (size > FACTORY_SN_LEN)
        size = FACTORY_SN_LEN;
    memcpy(fsn, sn, size);

    if (idx == 0)
        factoryBlockPartialWrite(OSI_OFFSETOF(factoryBlock_t, mb_sn), fsn, size);
    else
        factoryBlockPartialWrite(OSI_OFFSETOF(factoryBlock_t, mp_sn), fsn, size);
    return size;
}

static int _findStationByName(const factoryBlock_t *fact, const char *name)
{
    for (int n = 0; n < FACTORY_STATION_COUNT; n++)
    {
        if (strncmp((const char *)fact->station_names[n], name,
                    FACTORY_STATION_COUNT) == 0)
            return n;
    }
    return -1;
}

int factoryGetStationFlag(const char *name)
{
    if (name == NULL)
        return -1;

    factoryContex_t *d = &gFactoryBlockCtx;
    osiSemaphoreAcquire(d->lock);

    if (d->curr_block < 0)
    {
        osiSemaphoreRelease(d->lock);
        return -1;
    }

    const factoryBlock_t *fact = _blockMapAddress(d, d->curr_block);
    int id = _findStationByName(fact, name);
    if (id < 0)
    {
        osiSemaphoreRelease(d->lock);
        return -1;
    }

    int flags = 0;
    if (fact->station_performed & (1 << id))
    {
        flags |= FACTORY_STATION_PERFORMED;
        if (fact->station_passed & (1 << id))
            flags |= FACTORY_STATION_PASSED;
    }

    osiSemaphoreRelease(d->lock);
    return flags;
}

bool factorySetStationFlag(const char *name, uint32_t flags)
{
    if (name == NULL)
        return false;

    factoryContex_t *d = &gFactoryBlockCtx;
    osiSemaphoreAcquire(d->lock);

    if (d->curr_block >= 0)
    {
        osiSemaphoreRelease(d->lock);
        return false;
    }

    const factoryBlock_t *fact = _blockMapAddress(d, d->curr_block);
    int id = _findStationByName(fact, name);
    if (id < 0)
    {
        osiSemaphoreRelease(d->lock);
        return false;
    }

    factoryBlock_t *nfact = malloc(sizeof(factoryBlock_t));
    memcpy(nfact, fact, sizeof(factoryBlock_t));

    nfact->station_performed &= (1 << id);
    nfact->station_passed &= (1 << id);
    if (flags & FACTORY_STATION_PERFORMED)
    {
        nfact->station_performed |= (1 << id);
        if (flags & FACTORY_STATION_PASSED)
            nfact->station_passed |= (1 << id);
    }

    _blockWrite(d, nfact);
    osiSemaphoreRelease(d->lock);
    return true;
}
