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

#ifndef _HAL_SPI_FLASH_INTERNAL_H_
#define _HAL_SPI_FLASH_INTERNAL_H_

#include "osi_api.h"
#include "hwregs.h"

#ifdef __cplusplus
extern "C" {
#endif

// spi_config.tx_rx_size
enum
{
    RX_FIFO_WIDTH_8BIT = 0,
    RX_FIFO_WIDTH_16BIT = 1,
    RX_FIFO_WIDTH_32BIT = 2,
    RX_FIFO_WIDTH_24BIT = 3,
};

#define CMD_ADDRESS(opcode, address) ((opcode) | ((address) << 8))
#define EXTCMD_NORX(opcode) ((opcode) << 8)
#define EXTCMD_SRX(opcode) (((opcode) << 8) | (1 << 16))             // rx single mode
#define EXTCMD_QRX(opcode) (((opcode) << 8) | (1 << 16) | (1 << 17)) // rx quad mode
#define TX_QUAD_MASK (1 << 8)

/**
 * \brief flag to indicate tx data will be sent in quad mode
 */
#define HAL_SPI_FLASH_TX_QUAD (1 << 0)

/**
 * \brief flag to indicate using readback for rx
 */
#define HAL_SPI_FLASH_RX_READBACK (1 << 1)

/**
 * \brief flag to indicate 2nd tx data will be sent in quad mode
 */
#define HAL_SPI_FLASH_TX_QUAD2 (1 << 2)

/**
 * \brief generic flash command
 *
 * \param hwp flash controller address
 * \param cmd flash command, see above macros
 * \param cmd flash command
 * \param tx_data tx data
 * \param tx_size tx data size
 * \param rx_data rx data to be read
 * \param rx_size rx data size
 * \param flags TX_QUAD, RX_READBACK
 */
void halSpiFlashCmd(uintptr_t hwp, uint32_t cmd, const void *tx_data, unsigned tx_size,
                    void *rx_data, unsigned rx_size, unsigned flags);

/**
 * \brief generic flash command, with dual tx memory
 *
 * The data to be send are located in two memory pointer, and the property
 * of quad send can be different.
 *
 * \param hwp flash controller address
 * \param cmd flash command, see above macros
 * \param cmd flash command
 * \param tx_data tx data
 * \param tx_size tx data size
 * \param rx_data rx data to be read
 * \param rx_size rx data size
 * \param flags TX_QUAD, RX_READBACK, TX_QUAD2
 */
void halSpiFlashCmdDualTx(uintptr_t hwp, uint32_t cmd, const void *tx_data, unsigned tx_size,
                          const void *tx_data2, unsigned tx_size2,
                          void *rx_data, unsigned rx_size, unsigned flags);

static OSI_FORCE_INLINE void prvWaitNotBusy(uintptr_t d)
{
#if defined(CONFIG_SOC_8910)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    REG_SPI_FLASH_SPI_STATUS_T status;
    REG_WAIT_FIELD_EQZ(status, hwp->spi_status, spi_flash_busy);
    REG_WAIT_FIELD_EQZ(status, hwp->spi_status, spi_flash_busy);
#endif

#ifdef CONFIG_SOC_8955
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    REG_SPI_FLASH_SPI_DATA_FIFO_RO_T status;
    REG_WAIT_FIELD_EQZ(status, hwp->spi_data_fifo_ro, spi_flash_busy);
    REG_WAIT_FIELD_EQZ(status, hwp->spi_data_fifo_ro, spi_flash_busy);
#endif

#if defined(CONFIG_SOC_8909) || defined(CONFIG_SOC_8811)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    REG_SPI_FLASH_SPI_FIFO_STATUS_T status;
    REG_WAIT_FIELD_EQZ(status, hwp->spi_fifo_status, spi_flash_busy);
    REG_WAIT_FIELD_EQZ(status, hwp->spi_fifo_status, spi_flash_busy);
#endif
}

static OSI_FORCE_INLINE void prvClearFifo(uintptr_t d)
{
#if defined(CONFIG_SOC_8910) || defined(CONFIG_SOC_8811) || defined(CONFIG_SOC_8955) || defined(CONFIG_SOC_8909)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    REG_SPI_FLASH_SPI_FIFO_CONTROL_T fifo_control = {
        .b.rx_fifo_clr = 1,
        .b.tx_fifo_clr = 1,
    };
    hwp->spi_fifo_control = fifo_control.v;
#endif
}

static OSI_FORCE_INLINE void prvSetRxSize(uintptr_t d, unsigned size)
{
#if defined(CONFIG_SOC_8910) || defined(CONFIG_SOC_8811) || defined(CONFIG_SOC_8955) || defined(CONFIG_SOC_8909)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    REG_SPI_FLASH_SPI_BLOCK_SIZE_T block_size = {hwp->spi_block_size};
    block_size.b.spi_rw_blk_size = size;
    hwp->spi_block_size = block_size.v;
#endif
}

static OSI_FORCE_INLINE void prvSetFifoWidth(uintptr_t d, unsigned width)
{
#if defined(CONFIG_SOC_8910) || defined(CONFIG_SOC_8811) || defined(CONFIG_SOC_8955) || defined(CONFIG_SOC_8909)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    REG_SPI_FLASH_SPI_CONFIG_T config = {hwp->spi_config};
    if (width == 1)
        config.b.tx_rx_size = RX_FIFO_WIDTH_8BIT;
    else if (width == 2)
        config.b.tx_rx_size = RX_FIFO_WIDTH_16BIT;
    else if (width == 3)
        config.b.tx_rx_size = RX_FIFO_WIDTH_24BIT;
    else
        config.b.tx_rx_size = RX_FIFO_WIDTH_32BIT;
    hwp->spi_config = config.v;
#endif
}

static OSI_FORCE_INLINE void prvWriteCommand(uintptr_t d, uint32_t cmd)
{
#if defined(CONFIG_SOC_8910) || defined(CONFIG_SOC_8811) || defined(CONFIG_SOC_8955) || defined(CONFIG_SOC_8909)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    hwp->spi_cmd_addr = cmd;
    (void)hwp->spi_cmd_addr;
#endif
}

static OSI_FORCE_INLINE uint32_t prvReadBack(uintptr_t d)
{
#if defined(CONFIG_SOC_8910)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    return hwp->rx_status;
#endif

#if defined(CONFIG_SOC_8955) || defined(CONFIG_SOC_8909) || defined(CONFIG_SOC_8811)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    return hwp->spi_read_back;
#endif
}

static OSI_FORCE_INLINE void prvWriteFifo8(uintptr_t d, const uint8_t *txdata, unsigned txsize, unsigned quad)
{
#if defined(CONFIG_SOC_8910) || defined(CONFIG_SOC_8811) || defined(CONFIG_SOC_8909)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    for (unsigned n = 0; n < txsize; n++)
        hwp->spi_data_fifo = txdata[n] | quad;
#endif

#if defined(CONFIG_SOC_8955)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    for (unsigned n = 0; n < txsize; n++)
        hwp->spi_data_fifo_wo = txdata[n] | quad;
#endif
}

static OSI_FORCE_INLINE void prvReadFifo8(uintptr_t d, uint8_t *data, unsigned size)
{
#if defined(CONFIG_SOC_8910)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    while (size > 0)
    {
        REG_SPI_FLASH_SPI_STATUS_T spi_status = {hwp->spi_status};
        int count = OSI_MIN(int, spi_status.b.rx_fifo_count, size);
        for (int n = 0; n < count; n++)
            *data++ = hwp->spi_data_fifo;
        size -= count;
    }
#endif

#if defined(CONFIG_SOC_8955)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    while (size > 0)
    {
        REG_SPI_FLASH_SPI_DATA_FIFO_RO_T spi_status = {hwp->spi_data_fifo_ro};
        int count = OSI_MIN(int, spi_status.b.rx_fifo_count, size);
        for (int n = 0; n < count; n++)
            *data++ = hwp->spi_data_fifo_wo;
        size -= count;
    }
#endif

#if defined(CONFIG_SOC_8909) || defined(CONFIG_SOC_8811)
    HWP_SPI_FLASH_T *hwp = (HWP_SPI_FLASH_T *)d;
    while (size > 0)
    {
        REG_SPI_FLASH_SPI_FIFO_STATUS_T spi_status = {hwp->spi_fifo_status};
        int count = OSI_MIN(int, spi_status.b.rx_fifo_count, size);
        for (int n = 0; n < count; n++)
            *data++ = hwp->spi_data_fifo;
        size -= count;
    }
#endif
}

// Send command (basic or extended), with additional tx data, without rx data
FLASHRAM_CODE OSI_UNUSED static void prvCmdNoRx(uintptr_t hwp, uint32_t cmd, const void *tx_data, unsigned tx_size)
{
    halSpiFlashCmd(hwp, cmd, tx_data, tx_size, NULL, 0, 0);
}

// Send command (basic or extended), with additional tx data, without rx data
FLASHRAM_CODE OSI_UNUSED static void prvCmdNoRxDualTx(uintptr_t hwp, uint32_t cmd, const void *tx_data, unsigned tx_size, const void *tx_data2, unsigned tx_size2)
{
    halSpiFlashCmdDualTx(hwp, cmd, tx_data, tx_size, tx_data2, tx_size2, NULL, 0, 0);
}

// Send command (basic or extended), without additional tx data, without rx data
FLASHRAM_CODE OSI_UNUSED static void prvCmdOnlyNoRx(uintptr_t hwp, uint32_t cmd)
{
    halSpiFlashCmd(hwp, cmd, NULL, 0, NULL, 0, 0);
}

// Send command (basic or extended), without additional tx data, get rx data by readback
FLASHRAM_CODE OSI_UNUSED static uint32_t prvCmdOnlyReadback(uintptr_t hwp, uint32_t cmd, unsigned rx_size)
{
    uint32_t result = 0;
    halSpiFlashCmd(hwp, cmd, NULL, 0, &result, rx_size, HAL_SPI_FLASH_RX_READBACK);
    return result;
}

// Send command (basic or extended), with additional tx data, get rx data by readback
FLASHRAM_CODE OSI_UNUSED static uint32_t prvCmdRxReadback(uintptr_t hwp, uint32_t cmd, unsigned rx_size,
                                                          const void *tx_data, unsigned tx_size)
{
    uint32_t result = 0;
    halSpiFlashCmd(hwp, cmd, tx_data, tx_size, &result, rx_size, HAL_SPI_FLASH_RX_READBACK);
    return result;
}

// Send command (basic or extended), without additional tx data, get rx data by fifo
FLASHRAM_CODE OSI_UNUSED static void prvCmdRxFifo(uintptr_t hwp, uint32_t cmd, const void *tx_data, unsigned tx_size,
                                                  void *rx_data, unsigned rx_size)
{
    halSpiFlashCmd(hwp, cmd, tx_data, tx_size, rx_data, rx_size, 0);
}

#ifdef __cplusplus
}
#endif
#endif
