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
17/12/2020        Fei         Init version
=================================================================*/


#ifndef QL_BOOT_SPI_FLASH_H
#define QL_BOOT_SPI_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif
#define QL_FILE_MAX_PATH_LEN         (172)

typedef struct boot_fota_nv_cfg_param_t{
    char PackFileName[QL_FILE_MAX_PATH_LEN];
    int updata_urc_num;
    int data_reserve1;
    int data_reserve2;
    int data_reserve3;
    int data_reserve4;
}boot_fota_nv_cfg_param_t;


bool prvQuec_Load_Fota_nv_Cfg(boot_fota_nv_cfg_param_t* boot_fota_param);
#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_BOOT_SPI_FLASH_H */




