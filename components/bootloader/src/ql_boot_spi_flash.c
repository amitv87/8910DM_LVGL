/**  
  @file
  ql_boot_spi_flash.c

  @brief
  quectel boot spi flash interface.

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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_boot_spi_flash.h"
#include "osi_log.h"
#include "drv_names.h"


/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define SECTOR_SIZE_4K (4 * 1024)
#define OSI_ALIGN_TAIL(v, n) ((unsigned long)(v) & ((n)-1))

/*========================================================================
 *  function Definition
 *========================================================================*/
ql_errcode_boot_spi_flash_e ql_boot_spi_flash_init(bootSpiFlash_t *boot_spi)
{
    if (boot_spi == NULL)
    {
        OSI_LOGI(0,"boot spi flash param is null");
        return QL_BOOT_SPI_FLASH_PARAM_NULL_ERROR;
    }

    boot_spi = bootSpiFlashOpen(DRV_NAME_SPI_FLASH);

    if(boot_spi == NULL)
    {
        OSI_LOGI(0,"boot spi flash init error");
        return QL_BOOT_SPI_FLASH_ERROR;
    }
    
    return QL_BOOT_SPI_FLASH_SUCCESS;
}

ql_errcode_boot_spi_flash_e ql_boot_spi_flash_read(bootSpiFlash_t *boot_spi, uint32_t offset, void *data, size_t size)
{
    if (boot_spi == NULL || data == NULL || size < 1)
    {
        OSI_LOGI(0,"boot spi flash read param is null");
        return QL_BOOT_SPI_FLASH_PARAM_NULL_ERROR;
    }
    
    bool ret = bootSpiFlashRead(boot_spi, offset, data, size);

    if(!ret)
    {
        OSI_LOGI(0,"boot spi flash read error");
        return QL_BOOT_SPI_FLASH_READ_ERROR;
    }
    
    return QL_BOOT_SPI_FLASH_SUCCESS;
}

ql_errcode_boot_spi_flash_e ql_boot_spi_flash_write(bootSpiFlash_t *boot_spi, uint32_t offset, const void *data, size_t size)
{    
    if (boot_spi == NULL || data == NULL || size < 1)
    {
        OSI_LOGI(0,"boot spi flash write param is null");
        return QL_BOOT_SPI_FLASH_PARAM_NULL_ERROR;
    }
    
    bool ret = bootSpiFlashWrite(boot_spi, offset, data, size);
    
    if(!ret)
    {
        OSI_LOGI(0,"boot spi flash write error");
        return QL_BOOT_SPI_FLASH_WRITE_ERROR;
    }
    
    return QL_BOOT_SPI_FLASH_SUCCESS;
}

//The minimum erasing unit of flash is 4KB. 
//The characteristics of flash memory determine that it can only rewrite the data bit of "1" to "0", 
//while the data bit of "0" cannot be directly rewritten to "1", so it must be erased before writing
ql_errcode_boot_spi_flash_e ql_boot_spi_flash_erase_sector(bootSpiFlash_t *boot_spi, uint32_t offset, size_t size)
{
    int32_t tail_size   = 0;
    int32_t ret = -1;
    
    if (boot_spi == NULL || size < 1)
    {
        OSI_LOGI(0,"boot spi flash erase param is null");
        return QL_BOOT_SPI_FLASH_PARAM_NULL_ERROR;
    }

    if(!OSI_IS_ALIGNED(offset, SECTOR_SIZE_4K))
    {
        OSI_LOGI(0,"offset is not aligned with 4K");
        size   = size + OSI_ALIGN_TAIL(offset, SECTOR_SIZE_4K); 
        offset = offset - OSI_ALIGN_TAIL(offset, SECTOR_SIZE_4K);
        OSI_LOGI(0,"offset: 0x%x , size: %d ",offset, size);
    }

    tail_size = OSI_ALIGN_TAIL(size, SECTOR_SIZE_4K);
        
    if(tail_size > 0)
    {              
        ret = bootSpiFlashErase(boot_spi, offset, size - tail_size + SECTOR_SIZE_4K);
        if (ret < 0)
        {
            OSI_LOGI(0,"boot spi flash erase error");
            return QL_BOOT_SPI_FLASH_ERASE_ERROR;
        }
    }
    else
    {
        ret = bootSpiFlashErase(boot_spi, offset, size);
        if (ret < 0)
        {
            OSI_LOGI(0,"boot spi flash erase error");
            return QL_BOOT_SPI_FLASH_ERASE_ERROR;
        }
    }               
    
    return QL_BOOT_SPI_FLASH_SUCCESS;
}

void ql_boot_spi_flash_demo(bootSpiFlash_t *boot_spi)
{
    //be careful to override code data
    unsigned int addr = 0x00290005;
    unsigned int len = 0;
    //unsigned char *data_in = NULL; 
    unsigned char *data_out = NULL; 
    ql_errcode_boot_spi_flash_e err;
    
    int i = 0;
    len = 50;

    OSI_LOGI(0,"enter boot_spi_flash_demo");
    
    
    /*data_in = malloc(len+1);
    
    memset(data_in,0x00,len+1);
    
    for (i = 0; i < len; i++)
    {
        data_in[i] = 'a';
        OSI_LOGI(0, "boot spi flash data come in: %1c", data_in[i]);
    }*/
    
    //The minimum erasing unit of flash is 4KB. 
    //The characteristics of flash memory determine that it can only rewrite the data bit of "1" to "0", 
    //while the data bit of "0" cannot be directly rewritten to "1", so it must be erased before writing
    /*err = ql_boot_spi_flash_erase_sector(boot_spi, addr, len);  
    
    if (err != QL_BOOT_SPI_FLASH_SUCCESS)
    {
        free(data_in);
        OSI_LOGI(0,"erase error");
        return;
    }
    
    err = ql_boot_spi_flash_write(boot_spi, addr, (const void*)data_in, len);
    
    if (err != QL_BOOT_SPI_FLASH_SUCCESS)
    {
        free(data_in);
        OSI_LOGI(0,"write error");
        return;
    }*/
    
    data_out = malloc(len+1);
    memset(data_out,0x00,len+1);
    err = ql_boot_spi_flash_read(boot_spi, addr, data_out, len);
    
    if (err != QL_BOOT_SPI_FLASH_SUCCESS)
    {
        //free(data_in);
        free(data_out);
        OSI_LOGI(0,"read error");
        return;
    }

    for (i = 0; i < len; i++)
    {
        OSI_LOGI(0, "boot spi flash data come out: %1c", data_out[i]);
    }
    
    //free(data_in); 
    free(data_out); 
}


