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
17/12/2020        Fei         Init version
=================================================================*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "osi_log.h"
#include "drv_names.h"
#include "vfs.h"
#include "quec_boot_fota.h"
#define QUEC_CONFIG_DIR_ROOT	    "/config"
#define QUEC_FOAT_PACK_NAME_CFG  QUEC_CONFIG_DIR_ROOT"/quec_fota_packname.nv"

#define QUEC_BOOT_LEVEL OSI_LOG_LEVEL_INFO
#define QUEC_LOG_TAG OSI_MAKE_LOG_TAG('Q', 'U', 'E', 'C')
#define QUEC_BOOT_PRINTF(level, fmt, ...)                                        \
    do                                                                          \
    {                                                                           \
        if (QUEC_BOOT_LEVEL >= level)                                            \
            osiTracePrintf((level << 28) | (QUEC_LOG_TAG), fmt, ##__VA_ARGS__); \
    } while (0)
			
#define custom_log(user_str, msg, ...)				QUEC_BOOT_PRINTF(QUEC_BOOT_LEVEL, "["user_str"][%s, %d] "msg"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define QUEC_BOOT_FOTA_LOG(msg, ...)                custom_log("qboot_fota", msg, ##__VA_ARGS__)


/*===========================================================================
 * Macro Definition
 ===========================================================================*/

bool prvQuec_Load_Fota_nv_Cfg(    boot_fota_nv_cfg_param_t* boot_fota_param)
{
    memset(boot_fota_param,0,sizeof(boot_fota_nv_cfg_param_t));
    vfs_sfile_init(QUEC_FOAT_PACK_NAME_CFG);
    ssize_t file_size = vfs_sfile_size(QUEC_FOAT_PACK_NAME_CFG);
    if (file_size < 0)
    {
        QUEC_BOOT_FOTA_LOG("boot quec load fota nv config size error");
        return false;
    }
    file_size = vfs_sfile_read(QUEC_FOAT_PACK_NAME_CFG, (void*)boot_fota_param, sizeof(boot_fota_nv_cfg_param_t));
    if(file_size < 0)
    {
        QUEC_BOOT_FOTA_LOG("boot quec load fota nv config  error = %d",file_size);
        return false;
    }
    QUEC_BOOT_FOTA_LOG("boot quec load fota nv config :%d, %s",boot_fota_param->updata_urc_num,boot_fota_param->PackFileName);
    return true; 
}


