/**  @file
  ql_boot_spi_flash.h

  @brief
  This file is used to define boot spi flash api for different Quectel Project.

*/

/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------
08/12/2020        Neo         Init version
=================================================================*/


#ifndef QL_BOOT_SPI_FLASH_H
#define QL_BOOT_SPI_FLASH_H

#include "boot_spi_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QL_COMPONENT_STORAGE_INTERNALFLASH  0x8304

/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
typedef enum
{
    QL_BOOT_SPI_FLASH_SUCCESS                  =   0,
        
    QL_BOOT_SPI_FLASH_ERROR                    =   1 | ( QL_COMPONENT_STORAGE_INTERNALFLASH << 16),
    QL_BOOT_SPI_FLASH_READ_ERROR,
    QL_BOOT_SPI_FLASH_WRITE_ERROR,
    QL_BOOT_SPI_FLASH_ERASE_ERROR,
    QL_BOOT_SPI_FLASH_PARAM_NULL_ERROR
}ql_errcode_boot_spi_flash_e;


/*========================================================================
 *  function Definition
 *========================================================================*/
ql_errcode_boot_spi_flash_e ql_boot_spi_flash_init(bootSpiFlash_t* boot_spi);
ql_errcode_boot_spi_flash_e ql_boot_spi_flash_read(bootSpiFlash_t *boot_spi, uint32_t offset, void *data, size_t size);
ql_errcode_boot_spi_flash_e ql_boot_spi_flash_write(bootSpiFlash_t *boot_spi, uint32_t offset, const void *data, size_t size);
ql_errcode_boot_spi_flash_e ql_boot_spi_flash_erase_sector(bootSpiFlash_t *boot_spi, uint32_t offset, size_t size);
void ql_boot_spi_flash_demo(bootSpiFlash_t *boot_spi);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_BOOT_SPI_FLASH_H */



