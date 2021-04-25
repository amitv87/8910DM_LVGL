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

#ifndef _DRV_SDMMC_H_
#define _DRV_SDMMC_H_

#include "osi_compiler.h"
#include "drv_names.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief opaque data struct of sdmmc driver
 */
typedef struct drvSdmmc drvSdmmc_t;

/**
 * \brief create sdmmc driver instance
 *
 * \param name device name
 * \return
 *      - created driver instance
 *      - NULL on error, invalid parameter or out of memory
 */
drvSdmmc_t *drvSdmmcCreate(uint32_t name);

/**
 * \brief delete sdmmc driver instance
 *
 * \param d the sdmmc driver instance
 */
void drvSdmmcDestroy(drvSdmmc_t *d);

/**
 * \brief open sdmmc driver instance
 *
 * Open the sdmmc driver, done it can read/write data via the sdmmc.
 *
 * \param d the sdmmc driver instance
 * \return
 *      - true  success
 *      - false fail
 */
bool drvSdmmcOpen(drvSdmmc_t *d);

/**
 * \brief close sdmmc driver
 *
 * Close the sdmmc driver, stop all data transfer.
 * release resource.
 *
 * \param d the sdmmc driver
 */
void drvSdmmcClose(drvSdmmc_t *d);

/**
 * \brief get sdmmc block number
 *
 * \param d the sdmmc driver
 * \return block number
 */
uint32_t drvSdmmcGetBlockNum(drvSdmmc_t *d);

/**
 * \brief Write blocks of data to sdmmc
 *
 * This function is used to write blocks of data on the sdmmc.
 *
 * \p buffer should be 32 bits (4 bytes) aligned. \p size should be sector
 * size aligned.
 *
 * \p buffer should be SRAM or external RAM. Otherwise, it will return
 * false. Though hardware may support DMA from other addresses, it is
 * unreasonable to rely on this feature.
 *
 * \param block_number Start address of the SD memory block for writing
 * \param buffer Pointer to the block of data to write.
 * \param size Number of bytes to write.
 * \return
 *      - true on sucess.
 *      - false on fail.
 */
bool drvSdmmcWrite(drvSdmmc_t *d, uint32_t block_number, const void *buffer, uint32_t size);

/**
 * \brief Read blocks of data from sdmmc
 *
 * This function is used to Read blocks of data on the MMC.
 *
 * \p buffer should be 32 bits (4 bytes) aligned. \p size should be sector
 * size aligned.
 *
 * \p buffer should be SRAM or external RAM. Otherwise, it will return
 * false.
 *
 * D-cache invalidate will be performed on \p buffer. Caller should take care
 * the effects of D-cache invalidate.
 *
 * \param block_number Start address of the SD memory block for reading
 * \param buffer Pointer to the block of data to read
 * \param size Number of bytes to read
 * \return
 *      - true on success.
 *      - false on fail.
 */
bool drvSdmmcRead(drvSdmmc_t *d, uint32_t block_number, void *buffer, uint32_t size);

OSI_EXTERN_C_END
#endif
