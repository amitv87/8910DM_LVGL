/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "nvm.h"
#include "vfs.h"
#include "osi_log.h"
#include "nvm_config.h"
#include "fs_mount.h"
#include "bt_nv.h"

#include <stdlib.h>
#include <string.h>

#define FACTORYNV_DIR CONFIG_FS_FACTORY_MOUNT_POINT
#define RUNNINGNV_DIR CONFIG_FS_MODEM_NVM_DIR
#define MODEMNV_DIR CONFIG_FS_MODEM_MOUNT_POINT "/nvm"
#define OLD_BTCONFIG_FNAME RUNNINGNV_DIR "/bt_config.bin"
#define OLD_BTCONFIG_FIX_FNAME MODEMNV_DIR "/bt_config.bin"
#define NVID_BT_CONFIG 0x191
#define RD_ONLY 1

static int prvWriteFactoryNv(uint16_t nvid, void *buf, unsigned size)
{
    struct statvfs st;
    if (vfs_statvfs(FACTORYNV_DIR, &st) < 0)
    {
        OSI_LOGE(0, "nvm_migration: failed to stat vfs of temporal file");
        return -1;
    }
    if (st.f_flag == RD_ONLY)
    {
        OSI_LOGD(0, "factory remount read-write ");
        fsRemountFactory(0); // read-write
    }

    int w_size = nvmWriteFixedItem(nvid, buf, size);

    if (st.f_flag == RD_ONLY)
    {
        OSI_LOGD(0, "factory remount read-only ");
        fsRemountFactory(1); // read-only
    }
    return w_size;
}

static void prvBtConfigMigration(void)
{
    uint32_t size = 128; //bt config is less than 128
    uint8_t *btconfig = (uint8_t *)malloc(size);
    if (NULL == btconfig)
    {
        OSI_LOGE(0, "malloc btconfig fail");
        return;
    }
    memset(btconfig, 0, size);

    int old_size = nvmReadItem(NVID_BT_CONFIG, btconfig, size);
    if (old_size < 0)
    {
        // try to read running nv and fix nv in old store way
        old_size = vfs_sfile_read(OLD_BTCONFIG_FNAME, btconfig, size);
        if (old_size < 0)
            old_size = vfs_sfile_read(OLD_BTCONFIG_FIX_FNAME, btconfig, size);
    }

    uint32_t new_size = 0;
    if (btConfigNvCheck(btconfig, old_size, &new_size))
    {
        // check ok, need't migration
        goto end;
    }
    if (new_size <= 0)
    {
        OSI_LOGE(0, "bt_config migration fail, new_size %d", new_size);
        goto end;
    }
    OSI_LOGI(0, "check bt_config false, need migration old_size %d to new_size %d", old_size, new_size);

    uint8_t *new_btconfig = (uint8_t *)malloc(new_size);
    if (NULL == new_btconfig)
    {
        OSI_LOGE(0, "malloc new_btconfig fail");
        goto end;
    }

    if (btConfigNvMigration(btconfig, old_size, new_btconfig, new_size))
        prvWriteFactoryNv(NVID_BT_CONFIG, new_btconfig, new_size);

    free(new_btconfig);

end:
    if (btconfig)
        free(btconfig);
    return;
}

bool nvmMigration(void)
{
    prvBtConfigMigration();
    return true;
}