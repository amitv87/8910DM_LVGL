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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('F', 'S', 'S', 'D')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include <stdbool.h>
#include "tflash_block_device.h"
#include "drv_names.h"
#include "fatfs_vfs.h"
#include "vfs.h"
#include "fs_config.h"
#include "osi_log.h"
#include "osi_api.h"

blockDevice_t *gSdBdev = NULL;

static bool prvMountSdcard(bool format_on_fail)
{
    blockDevice_t *bdev = tflash_device_create(DRV_NAME_SDMMC1);
    if (bdev == NULL)
    {
        OSI_LOGE(0, "fail to create block device: %4c", DRV_NAME_SDMMC1);
        return false;
    }

    int r = fatfs_vfs_mount(CONFIG_FS_SDCARD_MOUNT_POINT, bdev);
    if (r != 0 && format_on_fail)
    {
        r = fatfs_vfs_mkfs(bdev);
        if (r == 0)
            r = fatfs_vfs_mount(CONFIG_FS_SDCARD_MOUNT_POINT, bdev);
    }

    if (r != 0)
    {
        OSI_LOGE(0, "fail to mount sdcard");
        blockDeviceDestroy(bdev);
        return false;
    }

    gSdBdev = bdev;
    return true;
}

void fsUmountSdcard()
{
    vfs_umount(CONFIG_FS_SDCARD_MOUNT_POINT);

    uint32_t critical = osiEnterCritical();
    blockDevice_t *bdev = gSdBdev;
    gSdBdev = NULL;
    osiExitCritical(critical);

    if (bdev)
    {
        blockDeviceDestroy(bdev);
    }
}

bool fsMountSdcard()
{
    bool format_on_fail = false;
#ifdef CONFIG_FS_FORMAT_SDCARD_ON_MOUNT_FAIL
    format_on_fail = true;
#endif
    return prvMountSdcard(format_on_fail);
}
