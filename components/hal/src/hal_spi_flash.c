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

#include "hal_spi_flash.h"
#include "hal_chip.h"
#include "hwregs.h"
#include "osi_api.h"
#include "osi_profile.h"
#include "osi_byte_buf.h"
#include "hal_spi_flash_defs.h"
#include <alloca.h>

#define FLASHRAM_CODE OSI_SECTION_LINE(.ramtext.flashhal)
#define FLASHRAM_API FLASHRAM_CODE OSI_NO_INLINE
#include "hal_spi_flash_internal.h"

#define SECTOR_COUNT_4K (SIZE_4K / SIZE_4K)
#define SECTOR_COUNT_8K (SIZE_8K / SIZE_4K)
#define SECTOR_COUNT_16K (SIZE_16K / SIZE_4K)
#define SECTOR_COUNT_32K (SIZE_32K / SIZE_4K)
#define SECTOR_COUNT_1M ((1 << 20) / SIZE_4K)
#define SECTOR_COUNT_2M ((2 << 20) / SIZE_4K)
#define SECTOR_COUNT_4M ((4 << 20) / SIZE_4K)
#define SECTOR_COUNT_8M ((8 << 20) / SIZE_4K)
#define SECTOR_COUNT_16M ((16 << 20) / SIZE_4K)

typedef struct
{
    uint16_t offset; // the unit of offset depends on user
    uint16_t wp;     // sr bits for wp
} halSpiFlashWpMap_t;

/**
 * Flash property table. It is not necessary to put it in ram.
 */
static const halSpiFlash_t gSpiFlashProps[] = {
#include "hal_spi_flash_prop.h"
};

/**
 * Table for GD 1MB, offset unit in 4K
 */
FLASHRAM_CODE static const halSpiFlashWpMap_t gGD8MWpMap[] = {
    {SECTOR_COUNT_1M, GD_WP8M_ALL},
    {SECTOR_COUNT_1M - SECTOR_COUNT_1M / 16, GD_WP8M_15_16},
    {SECTOR_COUNT_1M - SECTOR_COUNT_1M / 8, GD_WP8M_7_8},
    {SECTOR_COUNT_1M - SECTOR_COUNT_1M / 4, GD_WP8M_3_4},
    {SECTOR_COUNT_1M / 2, GD_WP8M_1_2},
    {SECTOR_COUNT_1M / 4, GD_WP8M_1_4},
    {SECTOR_COUNT_1M / 8, GD_WP8M_1_8},
    {SECTOR_COUNT_1M / 16, GD_WP8M_1_16},
    {SECTOR_COUNT_32K, GD_WP8M_32K},
    {SECTOR_COUNT_16K, GD_WP8M_16K},
    {SECTOR_COUNT_8K, GD_WP8M_8K},
    {SECTOR_COUNT_4K, GD_WP8M_4K},
    {0, GD_WP8M_NONE},
};

/**
 * Table for GD 2MB, offset unit in 4K
 */
FLASHRAM_CODE static const halSpiFlashWpMap_t gGD16MWpMap[] = {
    {SECTOR_COUNT_2M, GD_WP16M_ALL},
    {SECTOR_COUNT_2M - SECTOR_COUNT_2M / 32, GD_WP16M_31_32},
    {SECTOR_COUNT_2M - SECTOR_COUNT_2M / 16, GD_WP16M_15_16},
    {SECTOR_COUNT_2M - SECTOR_COUNT_2M / 8, GD_WP16M_7_8},
    {SECTOR_COUNT_2M - SECTOR_COUNT_2M / 4, GD_WP16M_3_4},
    {SECTOR_COUNT_2M / 2, GD_WP16M_1_2},
    {SECTOR_COUNT_2M / 4, GD_WP16M_1_4},
    {SECTOR_COUNT_2M / 8, GD_WP16M_1_8},
    {SECTOR_COUNT_2M / 16, GD_WP16M_1_16},
    {SECTOR_COUNT_2M / 32, GD_WP16M_1_32},
    {SECTOR_COUNT_32K, GD_WP16M_32K},
    {SECTOR_COUNT_16K, GD_WP16M_16K},
    {SECTOR_COUNT_8K, GD_WP16M_8K},
    {SECTOR_COUNT_4K, GD_WP16M_4K},
    {0, GD_WP16M_NONE},
};

/**
 * Table for GD 4MB, offset unit in 4K
 */
FLASHRAM_CODE static const halSpiFlashWpMap_t gGD32MWpMap[] = {
    {SECTOR_COUNT_4M, GD_WP32M_ALL},
    {SECTOR_COUNT_4M - SECTOR_COUNT_4M / 64, GD_WP32M_63_64},
    {SECTOR_COUNT_4M - SECTOR_COUNT_4M / 32, GD_WP32M_31_32},
    {SECTOR_COUNT_4M - SECTOR_COUNT_4M / 16, GD_WP32M_15_16},
    {SECTOR_COUNT_4M - SECTOR_COUNT_4M / 8, GD_WP32M_7_8},
    {SECTOR_COUNT_4M - SECTOR_COUNT_4M / 4, GD_WP32M_3_4},
    {SECTOR_COUNT_4M / 2, GD_WP32M_1_2},
    {SECTOR_COUNT_4M / 4, GD_WP32M_1_4},
    {SECTOR_COUNT_4M / 8, GD_WP32M_1_8},
    {SECTOR_COUNT_4M / 16, GD_WP32M_1_16},
    {SECTOR_COUNT_4M / 32, GD_WP32M_1_32},
    {SECTOR_COUNT_4M / 64, GD_WP32M_1_64},
    {SECTOR_COUNT_32K, GD_WP32M_32K},
    {SECTOR_COUNT_16K, GD_WP32M_16K},
    {SECTOR_COUNT_8K, GD_WP32M_8K},
    {SECTOR_COUNT_4K, GD_WP32M_4K},
    {0, GD_WP32M_NONE},
};

/**
 * Table for GD 8MB, offset unit in 4K
 */
FLASHRAM_CODE static const halSpiFlashWpMap_t gGD64MWpMap[] = {
    {SECTOR_COUNT_8M, GD_WP32M_ALL},
    {SECTOR_COUNT_8M - SECTOR_COUNT_8M / 64, GD_WP32M_63_64},
    {SECTOR_COUNT_8M - SECTOR_COUNT_8M / 32, GD_WP32M_31_32},
    {SECTOR_COUNT_8M - SECTOR_COUNT_8M / 16, GD_WP32M_15_16},
    {SECTOR_COUNT_8M - SECTOR_COUNT_8M / 8, GD_WP32M_7_8},
    {SECTOR_COUNT_8M - SECTOR_COUNT_8M / 4, GD_WP32M_3_4},
    {SECTOR_COUNT_8M / 2, GD_WP32M_1_2},
    {SECTOR_COUNT_8M / 4, GD_WP32M_1_4},
    {SECTOR_COUNT_8M / 8, GD_WP32M_1_8},
    {SECTOR_COUNT_8M / 16, GD_WP32M_1_16},
    {SECTOR_COUNT_8M / 32, GD_WP32M_1_32},
    {SECTOR_COUNT_8M / 64, GD_WP32M_1_64},
    {SECTOR_COUNT_32K, GD_WP32M_32K},
    {SECTOR_COUNT_16K, GD_WP32M_16K},
    {SECTOR_COUNT_8K, GD_WP32M_8K},
    {SECTOR_COUNT_4K, GD_WP32M_4K},
    {0, GD_WP32M_NONE},
};

/**
 * Table for GD 16MB, offset unit in 4K
 */
FLASHRAM_CODE static const halSpiFlashWpMap_t gGD128MWpMap[] = {
    {SECTOR_COUNT_16M, GD_WP32M_ALL},
    {SECTOR_COUNT_16M - SECTOR_COUNT_16M / 64, GD_WP32M_63_64},
    {SECTOR_COUNT_16M - SECTOR_COUNT_16M / 32, GD_WP32M_31_32},
    {SECTOR_COUNT_16M - SECTOR_COUNT_16M / 16, GD_WP32M_15_16},
    {SECTOR_COUNT_16M - SECTOR_COUNT_16M / 8, GD_WP32M_7_8},
    {SECTOR_COUNT_16M - SECTOR_COUNT_16M / 4, GD_WP32M_3_4},
    {SECTOR_COUNT_16M / 2, GD_WP32M_1_2},
    {SECTOR_COUNT_16M / 4, GD_WP32M_1_4},
    {SECTOR_COUNT_16M / 8, GD_WP32M_1_8},
    {SECTOR_COUNT_16M / 16, GD_WP32M_1_16},
    {SECTOR_COUNT_16M / 32, GD_WP32M_1_32},
    {SECTOR_COUNT_16M / 64, GD_WP32M_1_64},
    {SECTOR_COUNT_32K, GD_WP32M_32K},
    {SECTOR_COUNT_16K, GD_WP32M_16K},
    {SECTOR_COUNT_8K, GD_WP32M_8K},
    {SECTOR_COUNT_4K, GD_WP32M_4K},
    {0, GD_WP32M_NONE},
};

/**
 * Table for XMCA, offset unit in 1/128
 */
FLASHRAM_CODE static const halSpiFlashWpMap_t gXmcaWpMap[] = {
    {128, XMCA_WP_ALL},
    {127, XMCA_WP_127_128},
    {126, XMCA_WP_126_128},
    {124, XMCA_WP_124_128},
    {120, XMCA_WP_120_128},
    {112, XMCA_WP_112_128},
    {96, XMCA_WP_96_128},
    {64, XMCA_WP_64_128},
    {32, XMCA_WP_32_128},
    {16, XMCA_WP_16_128},
    {8, XMCA_WP_8_128},
    {4, XMCA_WP_4_128},
    {2, XMCA_WP_2_128},
    {1, XMCA_WP_1_128},
    {0, XMCA_WP_NONE},
};

/**
 * Trivial byte copy, avoid to use memcpy
 */
FLASHRAM_CODE static void prvByteCopy(void *dst, const void *src, unsigned size)
{
    uint8_t *pdst = (uint8_t *)dst;
    const uint8_t *psrc = (const uint8_t *)src;
    while (size-- > 0)
        *pdst++ = *psrc++;
}

/**
 * Find WP bits from offset. \p wpmap->offset is in decending order
 */
FLASHRAM_CODE static uint16_t prvFindFromWpMap(const halSpiFlashWpMap_t *wpmap, uint32_t offset)
{
    for (;;)
    {
        if (offset >= wpmap->offset)
            return wpmap->wp;
        ++wpmap;
    }
}

/**
 * Find real offset from offset. \p wpmap->offset is in decending order
 */
FLASHRAM_CODE static uint16_t prvFindFromWpOffset(const halSpiFlashWpMap_t *wpmap, uint32_t offset)
{
    for (;;)
    {
        if (offset >= wpmap->offset)
            return wpmap->offset;
        ++wpmap;
    }
}

/**
 * GD: SR with proper WP bits
 */
FLASHRAM_CODE static uint16_t prvStatusWpLowerGD(const halSpiFlash_t *d, uint16_t sr, uint32_t offset)
{
    uint32_t capacity = d->capacity;
    unsigned scount = (offset / SIZE_4K);
    if (capacity == (1 << 20))
        return (sr & ~GD_WP8M_MASK) | prvFindFromWpMap(gGD8MWpMap, scount);

    if (capacity == (2 << 20))
        return (sr & ~GD_WP16M_MASK) | prvFindFromWpMap(gGD16MWpMap, scount);

    if (capacity == (4 << 20))
        return (sr & ~GD_WP32M_MASK) | prvFindFromWpMap(gGD32MWpMap, scount);

    if (capacity == (8 << 20))
        return (sr & ~GD_WP32M_MASK) | prvFindFromWpMap(gGD64MWpMap, scount);

    if (capacity == (16 << 20))
        return (sr & ~GD_WP32M_MASK) | prvFindFromWpMap(gGD128MWpMap, scount);

    return sr;
}

/**
 * GD: SR with protect all
 */
FLASHRAM_CODE static uint16_t prvStatusWpAllGD(const halSpiFlash_t *d, uint16_t sr)
{
    if (d->capacity == (1 << 20))
        return (sr & ~GD_WP8M_MASK) | GD_WP8M_ALL;
    if (d->capacity == (2 << 20))
        return (sr & ~GD_WP16M_MASK) | GD_WP16M_ALL;
    return (sr & ~GD_WP32M_MASK) | GD_WP32M_ALL;
}

/**
 * XMCA: SR with proper WP bits
 */
FLASHRAM_CODE static uint16_t prvStatusWpLowerXMCA(const halSpiFlash_t *d, uint8_t sr, uint32_t offset)
{
    unsigned num = offset >> (MID_CAPBITS(d->mid) - 7); // unit in 1/128
    return (sr & ~XMCA_WP_MASK) | prvFindFromWpMap(gXmcaWpMap, num);
}

/**
 * XMCA: SR with protect all
 */
FLASHRAM_CODE static uint8_t prvStatusWpAllXMCA(const halSpiFlash_t *d, uint8_t sr)
{
    return (sr & ~XMCA_WP_MASK) | XMCA_WP_ALL;
}

/**
 * WP range from offset
 */
osiUintRange_t halSpiFlashWpRange(const halSpiFlash_t *d, uint32_t offset, uint32_t size)
{
    osiUintRange_t range = {0, 0};
    uint32_t capacity = d->capacity;
    if (d->wp_type == HAL_SPI_FLASH_WP_GD)
    {
        unsigned scount = (offset / SIZE_4K);
        if (capacity == (1 << 20))
            range.maxval = prvFindFromWpOffset(gGD8MWpMap, scount) * SIZE_4K;
        else if (capacity == (2 << 20))
            range.maxval = prvFindFromWpOffset(gGD16MWpMap, scount) * SIZE_4K;
        else if (capacity == (4 << 20))
            range.maxval = prvFindFromWpOffset(gGD32MWpMap, scount) * SIZE_4K;
        else if (capacity == (8 << 20))
            range.maxval = prvFindFromWpOffset(gGD64MWpMap, scount) * SIZE_4K;
        else if (capacity == (16 << 20))
            range.maxval = prvFindFromWpOffset(gGD128MWpMap, scount) * SIZE_4K;
    }
    else if (d->wp_type == HAL_SPI_FLASH_WP_XMCA)
    {
        unsigned num = offset >> (MID_CAPBITS(d->mid) - 7);
        range.maxval = prvFindFromWpOffset(gXmcaWpMap, num) << (MID_CAPBITS(d->mid) - 7);
    }
    return range;
}

/**
 * RDSR: 05H
 */
FLASHRAM_CODE static uint8_t prvReadSR1(const halSpiFlash_t *d)
{
    return prvCmdOnlyReadback(d->hwp, EXTCMD_SRX(0x05), 1);
}

/**
 * RDSR: 35H
 */
FLASHRAM_CODE static uint8_t prvReadSR2(const halSpiFlash_t *d)
{
    return prvCmdOnlyReadback(d->hwp, EXTCMD_SRX(0x35), 1);
}

/**
 * Read SR1 (at LSB) and SR2 (at MSB)
 */
FLASHRAM_CODE static uint16_t prvReadSR12(const halSpiFlash_t *d)
{
    return (prvReadSR2(d) << 8) | prvReadSR1(d);
}

/**
 * WRSR: 01H, write both SR1 and SR2
 */
FLASHRAM_CODE static void prvWriteSR12(const halSpiFlash_t *d, uint16_t sr)
{
    uint8_t data[2] = {sr & 0xff, (sr >> 8) & 0xff};
    prvCmdNoRx(d->hwp, EXTCMD_NORX(0x01), data, 2);
}

/**
 * WRSR: 01H, write SR1 only
 */
FLASHRAM_CODE static void prvWriteSR1(const halSpiFlash_t *d, uint8_t sr)
{
    prvCmdNoRx(d->hwp, EXTCMD_NORX(0x01), &sr, 1);
}

/**
 * WRSR: 31H, write SR2 only
 */
FLASHRAM_CODE static void prvWriteSR2(const halSpiFlash_t *d, uint8_t sr)
{
    prvCmdNoRx(d->hwp, EXTCMD_NORX(0x31), &sr, 1);
}

/**
 * Write Enable for Volatile Status Register: 50H
 */
FLASHRAM_CODE static void prvWriteVolatileSREnable(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0x50));
}

/**
 * Write volatile SR 1/2, with check
 */
FLASHRAM_CODE static void prvWriteVolatileSR12(const halSpiFlash_t *d, uint16_t sr)
{
    for (;;)
    {
        if (d->write_sr12)
        {
            prvWriteVolatileSREnable(d);
            prvWriteSR12(d, sr);
        }
        else
        {
            prvWriteVolatileSREnable(d);
            prvWriteSR1(d, sr & 0xff);
            prvWriteVolatileSREnable(d);
            prvWriteSR2(d, (sr >> 8) & 0xff);
        }

        // check whether volatile SR are written correctly
        uint16_t new_sr = prvReadSR12(d);
        if (new_sr == sr)
            break;
    }
}

/**
 * Write volatile SR1, with check
 */
FLASHRAM_CODE static void prvWriteVolatileSR1(const halSpiFlash_t *d, uint8_t sr)
{
    for (;;)
    {
        prvWriteVolatileSREnable(d);
        prvWriteSR1(d, sr);

        // check whether volatile SR are written correctly
        uint8_t new_sr = prvReadSR1(d);
        if (new_sr == sr)
            break;
    }
}

/**
 * Prepare for erase/program
 */
FLASHRAM_API void halSpiFlashPrepareEraseProgram(const halSpiFlash_t *d, uint32_t offset, uint32_t size)
{
    if (d->volatile_sr_en)
    {
        if (d->wp_type == HAL_SPI_FLASH_WP_GD)
        {
            uint16_t sr = prvReadSR12(d);
            uint16_t sr_open = prvStatusWpLowerGD(d, sr, offset);
            if (sr != sr_open)
                prvWriteVolatileSR12(d, sr_open);
        }
        else if (d->wp_type == HAL_SPI_FLASH_WP_XMCA)
        {
            uint8_t sr = prvReadSR1(d);
            uint8_t sr_open = prvStatusWpLowerXMCA(d, sr, offset);
            if (sr != sr_open)
                prvWriteVolatileSR1(d, sr_open);
        }
    }
    halSpiFlashWriteEnable(d);
}

/**
 * Finish for erase/program
 */
FLASHRAM_API void halSpiFlashFinishEraseProgram(const halSpiFlash_t *d)
{
    if (d->volatile_sr_en && d->wp_type == HAL_SPI_FLASH_WP_GD)
    {
        uint16_t sr = prvReadSR12(d);
        uint16_t sr_close = prvStatusWpAllGD(d, sr);
        if (sr != sr_close)
            prvWriteVolatileSR12(d, sr_close);
    }
    else if (d->volatile_sr_en && d->wp_type == HAL_SPI_FLASH_WP_XMCA)
    {
        uint8_t sr = prvReadSR1(d);
        uint8_t sr_close = prvStatusWpAllXMCA(d, sr);
        if (sr != sr_close)
            prvWriteVolatileSR1(d, sr_close);
    }
}

/**
 * PP: 02H
 */
FLASHRAM_API void halSpiFlashPageProgram(const halSpiFlash_t *d, uint32_t offset, const void *data, uint32_t size)
{
#ifdef CONFIG_SOC_8811
    hwp_med->med_clr = 0xffffffff;
#endif
    prvCmdNoRx(d->hwp, CMD_ADDRESS(0x02, offset), data, size);
}

/**
 * SE: 20H, 4K sector
 */
FLASHRAM_API static void prvFlashErase4K(const halSpiFlash_t *d, uint32_t offset)
{
#ifdef CONFIG_SOC_8811
    hwp_med->med_clr = 0xffffffff;
#endif
    prvCmdOnlyNoRx(d->hwp, CMD_ADDRESS(0x20, offset));
}

/**
 * BE: 52H, 32K block
 */
FLASHRAM_API static void prvFlashErase32K(const halSpiFlash_t *d, uint32_t offset)
{
#ifdef CONFIG_SOC_8811
    hwp_med->med_clr = 0xffffffff;
#endif
    prvCmdOnlyNoRx(d->hwp, CMD_ADDRESS(0x52, offset));
}

/**
 * BE: D8H, 64K block
 */
FLASHRAM_API static void prvFlashErase64K(const halSpiFlash_t *d, uint32_t offset)
{
#ifdef CONFIG_SOC_8811
    hwp_med->med_clr = 0xffffffff;
#endif
    prvCmdOnlyNoRx(d->hwp, CMD_ADDRESS(0xd8, offset));
}

/**
 * PES: 75H
 */
FLASHRAM_API void halSpiFlashProgramSuspend(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0x75));
}

/**
 * PES: 75H
 */
FLASHRAM_API void halSpiFlashEraseSuspend(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0x75));
}

/**
 * PER: 7AH
 */
FLASHRAM_API void halSpiFlashProgramResume(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0x7a));
}

/**
 * PER: 7AH
 */
FLASHRAM_API void halSpiFlashEraseResume(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0x7a));
}

/**
 * CE: C7H. Most flash can use 60H for CE also.
 */
FLASHRAM_API void halSpiFlashChipErase(const halSpiFlash_t *d)
{
#ifdef CONFIG_SOC_8811
    hwp_med->med_clr = 0xffffffff;
#endif
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0xc7));
}

/**
 * PD: B9H
 */
FLASHRAM_API void halSpiFlashDeepPowerDown(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0xb9));
}

/**
 * RDI: ABH
 */
FLASHRAM_API void halSpiFlashReleaseDeepPowerDown(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0xab));
    osiDelayUS(DELAY_AFTER_RELEASE_DEEP_POWER_DOWN);
}

/**
 * Read security register: 48H
 */
FLASHRAM_CODE static void prvSregRead48H(const halSpiFlash_t *d, uint32_t address, void *data, uint32_t size)
{
    uint8_t tx_data[4] = {(address >> 16) & 0xff, (address >> 8) & 0xff, address & 0xff, 0};
    uint32_t val = prvCmdRxReadback(d->hwp, EXTCMD_SRX(0x48), size, tx_data, 4);
    prvByteCopy(data, &val, size);
}

/**
 * Read security register, 68H
 */
FLASHRAM_CODE static void prvSregRead68H(const halSpiFlash_t *d, uint32_t address, void *data, uint32_t size)
{
    uint8_t tx_data[4] = {(address >> 16) & 0xff, (address >> 8) & 0xff, address & 0xff, 0};
    prvCmdRxFifo(d->hwp, EXTCMD_SRX(0x68), tx_data, 4, data, size);
}

/**
 * Program security register: 42H
 */
FLASHRAM_CODE static void prvSregProgram42H(const halSpiFlash_t *d, uint32_t address, const void *data, uint32_t size)
{
    uint8_t tx_data[3] = {(address >> 16) & 0xff, (address >> 8) & 0xff, address & 0xff};
    prvCmdNoRxDualTx(d->hwp, EXTCMD_NORX(0x42), tx_data, 3, data, size);
}

/**
 * Program security register: 62H
 */
FLASHRAM_CODE static void prvSregProgram62H(const halSpiFlash_t *d, uint32_t address, const void *data, uint32_t size)
{
    uint8_t tx_data[3] = {(address >> 16) & 0xff, (address >> 8) & 0xff, address & 0xff};
    prvCmdNoRxDualTx(d->hwp, EXTCMD_NORX(0x62), tx_data, 3, data, size);
}

/**
 * Erase security register: 44H
 */
FLASHRAM_CODE static void prvSregErase44H(const halSpiFlash_t *d, uint32_t address)
{
    uint8_t tx_data[3] = {(address >> 16) & 0xff, (address >> 8) & 0xff, address & 0xff};
    prvCmdNoRx(d->hwp, EXTCMD_NORX(0x44), tx_data, 3);
}

/**
 * Erase security register: 64H
 */
FLASHRAM_CODE static void prvSregErase64H(const halSpiFlash_t *d, uint32_t address)
{
    uint8_t tx_data[3] = {(address >> 16) & 0xff, (address >> 8) & 0xff, address & 0xff};
    prvCmdNoRx(d->hwp, EXTCMD_NORX(0x64), tx_data, 3);
}

/**
 * GD: Lock security register
 */
FLASHRAM_CODE static void prvSregLockGD(const halSpiFlash_t *d, uint8_t num)
{
    uint16_t sr12 = halSpiFlashReadSR(d);
    sr12 |= (GD_SR_LB1 << (num - 1));
    halSpiFlashWriteSR(d, sr12);
}

/**
 * GD: Unlock security register
 */
FLASHRAM_CODE static void prvSregUnlockGD(const halSpiFlash_t *d, uint8_t num)
{
    uint16_t sr12 = halSpiFlashReadSR(d);
    sr12 &= ~(GD_SR_LB1 << (num - 1));
    halSpiFlashWriteSR(d, sr12);
}

/**
 * GD: Is security register locked
 */
FLASHRAM_CODE static bool prvSregIsLockedGD(const halSpiFlash_t *d, uint8_t num)
{
    uint16_t sr12 = halSpiFlashReadSR(d);
    return (sr12 & (GD_SR_LB1 << (num - 1)));
}

/**
 * XTX: Lock security register
 */
FLASHRAM_CODE static void prvSregLockXTX(const halSpiFlash_t *d, uint8_t num)
{
    uint16_t sr12 = halSpiFlashReadSR(d);
    sr12 |= XTX_SR_LB;
    halSpiFlashWriteSR(d, sr12);
}

/**
 * XTX: Unlock security register
 */
FLASHRAM_CODE static void prvSregUnlockXTX(const halSpiFlash_t *d, uint8_t num)
{
    uint16_t sr12 = halSpiFlashReadSR(d);
    sr12 &= ~XTX_SR_LB;
    halSpiFlashWriteSR(d, sr12);
}

/**
 * XTX: Is security register locked
 */
FLASHRAM_CODE static bool prvSregIsLockedXTX(const halSpiFlash_t *d, uint8_t num)
{
    uint16_t sr12 = halSpiFlashReadSR(d);
    return (sr12 & XTX_SR_LB);
}

/**
 * XMCB: Lock security register
 */
FLASHRAM_CODE static void prvSregLockXMCB(const halSpiFlash_t *d, uint8_t num)
{
    uint8_t fr = prvCmdOnlyReadback(d->hwp, EXTCMD_SRX(0x48), 1); // RDFR
    fr |= (XMCB_FR_IRL0 << num);
    halSpiFlashWriteEnable(d);
    prvCmdNoRx(d->hwp, EXTCMD_NORX(0x42), &fr, 1); // WRFR
    halSpiFlashWaitWipFinish(d);
}

/**
 * XMCB: Unlock security register
 */
FLASHRAM_CODE static void prvSregUnlockXMCB(const halSpiFlash_t *d, uint8_t num)
{
    uint8_t fr = prvCmdOnlyReadback(d->hwp, EXTCMD_SRX(0x48), 1); // RDFR
    fr &= ~(XMCB_FR_IRL0 << num);
    halSpiFlashWriteEnable(d);
    prvCmdNoRx(d->hwp, EXTCMD_NORX(0x42), &fr, 1); // WRFR
    halSpiFlashWaitWipFinish(d);
}

/**
 * XMCB: Is security register locked
 */
FLASHRAM_CODE static bool prvSregIsLockedXMCB(const halSpiFlash_t *d, uint8_t num)
{
    uint8_t fr = prvCmdOnlyReadback(d->hwp, EXTCMD_SRX(0x48), 1); // RDFR
    return (fr & (XMCB_FR_IRL0 << num));
}

/**
 * Read security register, 4 byte at most
 */
FLASHRAM_API bool halSpiFlashReadSecurityRegister(
    const halSpiFlash_t *d,
    uint8_t num, uint16_t address,
    void *data, uint32_t size)
{
    if (num < d->sreg_min_num || num > d->sreg_max_num)
        return false;
    if ((address + size) > d->sreg_block_size)
        return false;

    switch (d->type)
    {
    case HAL_SPI_FLASH_TYPE_GD:
    case HAL_SPI_FLASH_TYPE_WINBOND:
    case HAL_SPI_FLASH_TYPE_XMCC:
    case HAL_SPI_FLASH_TYPE_XTX:
    case HAL_SPI_FLASH_TYPE_PUYA:
        prvSregRead48H(d, (num << 12) | address, data, size);
        return true;

    case HAL_SPI_FLASH_TYPE_XMCB:
        prvSregRead68H(d, (num << 12) | address, data, size);
        return true;

    default:
        return false;
    }
}

/**
 * Program security register
 */
FLASHRAM_API bool halSpiFlashProgramSecurityRegister(
    const halSpiFlash_t *d,
    uint8_t num, uint16_t address,
    const void *data, uint32_t size)
{
    if (num < d->sreg_min_num || num > d->sreg_max_num)
        return false;
    if ((address + size) > d->sreg_block_size)
        return false;

    switch (d->type)
    {
    case HAL_SPI_FLASH_TYPE_GD:
    case HAL_SPI_FLASH_TYPE_WINBOND:
    case HAL_SPI_FLASH_TYPE_XMCC:
    case HAL_SPI_FLASH_TYPE_XTX:
    case HAL_SPI_FLASH_TYPE_PUYA:
        prvSregProgram42H(d, (num << 12) | address, data, size);
        return true;

    case HAL_SPI_FLASH_TYPE_XMCB:
        prvSregProgram62H(d, (num << 12) | address, data, size);
        return true;

    default:
        return false;
    }
}

/**
 * Erase security register
 */
FLASHRAM_API bool halSpiFlashEraseSecurityRegister(const halSpiFlash_t *d, uint8_t num)
{
    if (num < d->sreg_min_num || num > d->sreg_max_num)
        return false;
    if (d->sreg_block_size == 0)
        return false;

    switch (d->type)
    {
    case HAL_SPI_FLASH_TYPE_GD:
    case HAL_SPI_FLASH_TYPE_WINBOND:
    case HAL_SPI_FLASH_TYPE_XMCC:
    case HAL_SPI_FLASH_TYPE_XTX:
    case HAL_SPI_FLASH_TYPE_PUYA:
        prvSregErase44H(d, num << 12);
        return true;

    case HAL_SPI_FLASH_TYPE_XMCB:
        prvSregErase64H(d, num << 12);
        return true;

    default:
        return false;
    }
}

/**
 * Lock security register
 */
FLASHRAM_API bool halSpiFlashLockSecurityRegister(const halSpiFlash_t *d, uint8_t num)
{
    if (num < d->sreg_min_num || num > d->sreg_max_num)
        return false;

    switch (d->type)
    {
    case HAL_SPI_FLASH_TYPE_GD:
    case HAL_SPI_FLASH_TYPE_WINBOND:
    case HAL_SPI_FLASH_TYPE_XMCC:
    case HAL_SPI_FLASH_TYPE_PUYA:
        prvSregLockGD(d, num);
        return true;

    case HAL_SPI_FLASH_TYPE_XTX:
        prvSregLockXTX(d, num);
        return true;

    case HAL_SPI_FLASH_TYPE_XMCB:
        prvSregLockXMCB(d, num);
        return true;

    default:
        return false;
    }
}

/**
 * (DEBUG ONLY) It is not a real feature, just to verify that security
 * registers can't be unlocked.
 */
FLASHRAM_API bool halSpiFlashUnlockSecurityRegister(const halSpiFlash_t *d, uint8_t num)
{
    if (num < d->sreg_min_num || num > d->sreg_max_num)
        return false;

    switch (d->type)
    {
    case HAL_SPI_FLASH_TYPE_GD:
    case HAL_SPI_FLASH_TYPE_WINBOND:
    case HAL_SPI_FLASH_TYPE_XMCC:
    case HAL_SPI_FLASH_TYPE_PUYA:
        prvSregUnlockGD(d, num);
        return true;

    case HAL_SPI_FLASH_TYPE_XTX:
        prvSregUnlockXTX(d, num);
        return true;

    case HAL_SPI_FLASH_TYPE_XMCB:
        prvSregUnlockXMCB(d, num);
        return true;

    default:
        return false;
    }
}

/**
 * Is security register locked
 */
FLASHRAM_API bool halSpiFlashIsSecurityRegisterLocked(const halSpiFlash_t *d, uint8_t num)
{
    if (num < d->sreg_min_num || num > d->sreg_max_num)
        return false;

    switch (d->type)
    {
    case HAL_SPI_FLASH_TYPE_GD:
    case HAL_SPI_FLASH_TYPE_WINBOND:
    case HAL_SPI_FLASH_TYPE_XMCC:
    case HAL_SPI_FLASH_TYPE_PUYA:
        return prvSregIsLockedGD(d, num);

    case HAL_SPI_FLASH_TYPE_XTX:
        return prvSregIsLockedXTX(d, num);

    case HAL_SPI_FLASH_TYPE_XMCB:
        return prvSregIsLockedXMCB(d, num);

    default:
        return false;
    }
}

/**
 * Erase (64K/32K/4K)
 */
FLASHRAM_API void halSpiFlashErase(const halSpiFlash_t *d, uint32_t offset, uint32_t size)
{
    if (size == SIZE_64K)
        prvFlashErase64K(d, offset);
    else if (size == SIZE_32K)
        prvFlashErase32K(d, offset);
    else
        prvFlashErase4K(d, offset);
}

/**
 * Read unique ID
 */
FLASHRAM_API int halSpiFlashReadUniqueId(const halSpiFlash_t *d, uint8_t *uid)
{
    if (d->uid_type == HAL_SPI_FLASH_UID_4BH_8)
    {
        uint8_t tx_data[4] = {};
        prvCmdRxFifo(d->hwp, EXTCMD_SRX(0x4b), tx_data, 4, uid, 8);
        return 8;
    }

    if (d->uid_type == HAL_SPI_FLASH_UID_4BH_16)
    {
        uint8_t tx_data[4] = {};
        prvCmdRxFifo(d->hwp, EXTCMD_SRX(0x4b), tx_data, 4, uid, 16);
        return 16;
    }

    if (d->uid_type == HAL_SPI_FLASH_UID_SFDP_80H_12)
    {
        halSpiFlashReadSFDP(d, 0x80, uid, 12);
        return 12;
    }

    if (d->uid_type == HAL_SPI_FLASH_UID_SFDP_194H_16)
    {
        halSpiFlashReadSFDP(d, 0x194, uid, 16);
        return 16;
    }

    if (d->uid_type == HAL_SPI_FLASH_UID_SFDP_94H_16)
    {
        halSpiFlashReadSFDP(d, 0x94, uid, 16);
        return 16;
    }

    return 0;
}

/**
 * Read cp id
 */
FLASHRAM_API uint16_t halSpiFlashReadCpId(const halSpiFlash_t *d)
{
    if (d->cpid_type == HAL_SPI_FLASH_CPID_4BH)
    {
        uint8_t uid[18];
        uint8_t tx_data[4] = {};
        prvCmdRxFifo(d->hwp, EXTCMD_SRX(0x4b), tx_data, 4, uid, 18);
        return osiBytesGetLe16(&uid[16]);
    }

    return 0;
}

/**
 * Read Serial Flash Discoverable Parameters: 5AH
 */
FLASHRAM_API bool halSpiFlashReadSFDP(const halSpiFlash_t *d, uint32_t address, void *data, uint32_t size)
{
    if (!d->sfdp_en)
        return false;

    uint8_t tx_data[4] = {(address >> 16) & 0xff, (address >> 8) & 0xff, address & 0xff, 0};
    prvCmdRxFifo(d->hwp, EXTCMD_SRX(0x5a), tx_data, 4, data, size);
    return true;
}

/**
 * Generic flash command
 */
FLASHRAM_API void halSpiFlashCmd(
    uintptr_t hwp, uint32_t cmd,
    const void *tx_data, unsigned tx_size,
    void *rx_data, unsigned rx_size,
    unsigned flags)
{
    prvWaitNotBusy(hwp);
    prvClearFifo(hwp);
    prvSetRxSize(hwp, rx_size);
    prvSetFifoWidth(hwp, (flags & HAL_SPI_FLASH_RX_READBACK) ? rx_size : 1);
    prvWriteFifo8(hwp, tx_data, tx_size, (flags & HAL_SPI_FLASH_TX_QUAD) ? TX_QUAD_MASK : 0);
    prvWriteCommand(hwp, cmd);

    if (!(flags & HAL_SPI_FLASH_RX_READBACK))
        prvReadFifo8(hwp, rx_data, rx_size);

    prvWaitNotBusy(hwp);

    if (flags & HAL_SPI_FLASH_RX_READBACK)
    {
        uint32_t rword = prvReadBack(hwp) >> ((4 - rx_size) * 8);
        uint8_t *rxb = (uint8_t *)rx_data;
        for (unsigned n = 0; n < rx_size; n++)
        {
            *rxb++ = rword & 0xff;
            rword >>= 8;
        }
    }
    prvSetRxSize(hwp, 0);
}

/**
 * Generic flash command with dual tx data, either for different quad
 * property, or avoid to concate together.
 */
void halSpiFlashCmdDualTx(
    uintptr_t hwp, uint32_t cmd,
    const void *tx_data, unsigned tx_size,
    const void *tx_data2, unsigned tx_size2,
    void *rx_data, unsigned rx_size,
    unsigned flags)
{
    prvWaitNotBusy(hwp);
    prvClearFifo(hwp);
    prvSetRxSize(hwp, rx_size);
    prvSetFifoWidth(hwp, (flags & HAL_SPI_FLASH_RX_READBACK) ? rx_size : 1);
    prvWriteFifo8(hwp, tx_data, tx_size, (flags & HAL_SPI_FLASH_TX_QUAD) ? TX_QUAD_MASK : 0);
    prvWriteFifo8(hwp, tx_data2, tx_size2, (flags & HAL_SPI_FLASH_TX_QUAD2) ? TX_QUAD_MASK : 0);
    prvWriteCommand(hwp, cmd);

    if (!(flags & HAL_SPI_FLASH_RX_READBACK))
        prvReadFifo8(hwp, rx_data, rx_size);

    prvWaitNotBusy(hwp);

    if (flags & HAL_SPI_FLASH_RX_READBACK)
    {
        uint32_t rword = prvReadBack(hwp) >> ((4 - rx_size) * 8);
        uint8_t *rxb = (uint8_t *)rx_data;
        for (unsigned n = 0; n < rx_size; n++)
        {
            *rxb++ = rword & 0xff;
            rword >>= 8;
        }
    }
    prvSetRxSize(hwp, 0);
}

/**
 * RDID: 9FH
 */
FLASHRAM_CODE static uint32_t prvReadId(const halSpiFlash_t *d)
{
    return prvCmdOnlyReadback(d->hwp, EXTCMD_SRX(0x9f), 3);
}

/**
 * WREN: 06H
 */
FLASHRAM_API void halSpiFlashWriteEnable(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0x06));
}

/**
 * WRDI: 04H
 */
FLASHRAM_API void halSpiFlashWriteDisable(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0x04));
}

/**
 * ENABLE RESET: 66H
 */
FLASHRAM_API void halSpiFlashResetEnable(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0x66));
}

/**
 * RESET: 99H
 */
FLASHRAM_API void halSpiFlashReset(const halSpiFlash_t *d)
{
    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0x99));
}

/**
 * Read status registers
 */
FLASHRAM_API uint16_t halSpiFlashReadSR(const halSpiFlash_t *d)
{
    return (d->has_sr2) ? prvReadSR12(d) : prvReadSR1(d);
}

/**
 * write SR1, and SR2 if supported
 */
FLASHRAM_API void halSpiFlashWriteSR(const halSpiFlash_t *d, uint16_t sr)
{
    if (!d->has_sr2)
    {
        halSpiFlashWriteEnable(d);
        prvWriteSR1(d, sr & 0xff);
        halSpiFlashWaitWipFinish(d);
    }
    else if (d->write_sr12)
    {
        halSpiFlashWriteEnable(d);
        prvWriteSR12(d, sr);
        halSpiFlashWaitWipFinish(d);
    }
    else
    {
        halSpiFlashWriteEnable(d);
        prvWriteSR1(d, sr & 0xff);
        halSpiFlashWaitWipFinish(d);
        halSpiFlashWriteEnable(d);
        prvWriteSR2(d, (sr >> 8) & 0xff);
        halSpiFlashWaitWipFinish(d);
    }
}

/**
 * whether WIP is 0
 */
FLASHRAM_API bool halSpiFlashIsWipFinished(const halSpiFlash_t *d)
{
    osiDelayUS(1);
    if (prvReadSR1(d) & STREG_WIP)
        return false;
    if (prvReadSR1(d) & STREG_WIP)
        return false;
    return true;
}

/**
 * wait WIP to be 0
 */
FLASHRAM_API void halSpiFlashWaitWipFinish(const halSpiFlash_t *d)
{
    OSI_LOOP_WAIT(halSpiFlashIsWipFinished(d));
}

/**
 * XMCA: initial SR check
 */
OSI_FORCE_INLINE static void prvStatusCheckXMCA(const halSpiFlash_t *d)
{
    // always reset
    halSpiFlashResetEnable(d);
    halSpiFlashReset(d);
    osiDelayUS(DELAY_AFTER_RESET);

    prvCmdOnlyNoRx(d->hwp, EXTCMD_NORX(0x3a)); // Enter OTP Mode
    uint8_t sr_otp = prvReadSR1(d);
    if ((sr_otp & XMCA_SR_OTP_TB) == 0)
    {
        halSpiFlashWriteEnable(d);
        prvWriteSR1(d, sr_otp | XMCA_SR_OTP_TB);
        halSpiFlashWaitWipFinish(d);
    }
    halSpiFlashWriteDisable(d); // Exit OTP Mode

    uint8_t sr = prvReadSR1(d);
    uint8_t sr_needed = sr | (XMCA_SR_BP0 | XMCA_SR_BP1 | XMCA_SR_BP2 | XMCA_SR_BP3);
    sr_needed &= ~(XMCA_SR_EBL | XMCA_SR_SRP);
    if (sr != sr_needed)
    {
        halSpiFlashWriteEnable(d);
        prvWriteSR1(d, sr_needed);
        halSpiFlashWaitWipFinish(d);
    }
}

/**
 * XMCB: initial SR check
 */
OSI_FORCE_INLINE static void prvStatusCheckXMCB(const halSpiFlash_t *d)
{
    // always reset
    halSpiFlashResetEnable(d);
    halSpiFlashReset(d);
    osiDelayUS(DELAY_AFTER_RESET);

    // Due to volatile BP is not supported, SR will be set to wp-none status
    uint8_t sr = prvReadSR1(d);
    if (sr != XMCB_SR_QE)
    {
        halSpiFlashWriteEnable(d);
        prvWriteSR1(d, XMCB_SR_QE);
        halSpiFlashWaitWipFinish(d);
    }
}

/**
 * GD: initial SR check
 */
OSI_FORCE_INLINE static void prvStatusCheckGD(const halSpiFlash_t *d)
{
    uint16_t sr = halSpiFlashReadSR(d);
    uint16_t need_reset_mask = (STREG_WEL | STREG_WIP);
    if (d->has_sus1)
        need_reset_mask |= GD_SR_SUS1;
    if (d->has_sus2)
        need_reset_mask |= GD_SR_SUS2;

    if (sr & need_reset_mask)
    {
        halSpiFlashResetEnable(d);
        halSpiFlashReset(d);
        osiDelayUS(DELAY_AFTER_RESET);

        sr = halSpiFlashReadSR(d);
    }

    uint16_t sr_needed = sr | GD_SR_QE;
    // Write volatile BP as protect all
    if (d->wp_type == HAL_SPI_FLASH_WP_GD)
        sr_needed = prvStatusWpAllGD(d, sr_needed);

    if (sr != sr_needed)
        halSpiFlashWriteSR(d, sr_needed);
}

/**
 * Initial SR check
 */
FLASHRAM_API void halSpiFlashStatusCheck(const halSpiFlash_t *d)
{
    switch (d->type)
    {
    case HAL_SPI_FLASH_TYPE_GD:
    case HAL_SPI_FLASH_TYPE_WINBOND:
    case HAL_SPI_FLASH_TYPE_XMCC:
    case HAL_SPI_FLASH_TYPE_XTX:
    case HAL_SPI_FLASH_TYPE_PUYA:
        prvStatusCheckGD(d);
        break;

    case HAL_SPI_FLASH_TYPE_XMCA:
        prvStatusCheckXMCA(d);
        break;

    case HAL_SPI_FLASH_TYPE_XMCB:
        prvStatusCheckXMCB(d);
        break;
    }
}

/**
 * Fill property by mid, search property table
 */
FLASHRAM_CODE static void prvFlashPropsByMid(halSpiFlash_t *d, unsigned mid)
{
    int found = -1;
    // match MID
    for (unsigned n = 0; n < OSI_ARRAY_SIZE(gSpiFlashProps); n++)
    {
        if (gSpiFlashProps[n].mid == mid)
        {
            found = n;
            break;
        }
    }

    if (found < 0)
    {
        // match memtype and manufacture
        for (unsigned n = 0; n < OSI_ARRAY_SIZE(gSpiFlashProps); n++)
        {
            if (gSpiFlashProps[n].mid == (mid & 0xffff))
            {
                found = n;
                break;
            }
        }
    }

    if (found < 0)
    {
        // match manufacture
        for (unsigned n = 0; n < OSI_ARRAY_SIZE(gSpiFlashProps); n++)
        {
            if (gSpiFlashProps[n].mid == (mid & 0xff))
            {
                found = n;
                break;
            }
        }
    }

    // something is wrong, we can't go further
    if (found < 0)
        osiPanic();

    prvByteCopy(&d->capacity, &gSpiFlashProps[found].capacity,
                sizeof(halSpiFlash_t) - OSI_OFFSETOF(halSpiFlash_t, capacity));
    d->mid = mid;
    d->capacity = (1 << MID_CAPBITS(mid));
}

/**
 * Initialize flash
 */
FLASHRAM_API void halSpiFlashInit(halSpiFlash_t *d)
{
    uint32_t mid = prvReadId(d);
    prvFlashPropsByMid(d, mid);
    halSpiFlashStatusCheck(d);
}

/**
 * (DEBUG ONLY) Clear QE bits in SR. In real application, there are no
 * reasons to call this.
 */
FLASHRAM_API bool halSpiFlashUnsetQuadEnable(const halSpiFlash_t *d)
{
    switch (d->type)
    {
    case HAL_SPI_FLASH_TYPE_GD:
    case HAL_SPI_FLASH_TYPE_WINBOND:
    case HAL_SPI_FLASH_TYPE_XMCC:
    case HAL_SPI_FLASH_TYPE_XTX:
    case HAL_SPI_FLASH_TYPE_PUYA:
        halSpiFlashWriteSR(d, halSpiFlashReadSR(d) & ~GD_SR_QE);
        return true;

    case HAL_SPI_FLASH_TYPE_XMCB:
        halSpiFlashWriteSR(d, halSpiFlashReadSR(d) & ~XMCB_SR_QE);
        return true;

    default:
        return false;
    }
}
