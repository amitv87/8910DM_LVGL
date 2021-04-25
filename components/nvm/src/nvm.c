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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('N', 'V', 'M', ' ')

#include "nvm.h"
#include "vfs.h"
#include "osi_log.h"

#define IMEI_FILE_NAME "/nvm/imei.bin"
#define SN_FILE_NAME "/nvm/sn.bin"
#define SN_MAX_LENGTH (64)

int nvmReadImei(int index, nvmImei_t *imei)
{
    if (imei == NULL)
        return -1;

    const char *imei_file = IMEI_FILE_NAME;
    vfs_sfile_init(imei_file);
    int len = vfs_sfile_read(imei_file, imei, sizeof(nvmImei_t));
    if (len != sizeof(nvmImei_t))
    {
        OSI_LOGE(0, "imei - read fail. (not enough data)");
        return -1;
    }

    return sizeof(nvmImei_t);
}

int nvmWriteImei(int index, const nvmImei_t *imei)
{
    if (imei == NULL)
        return -1;

    const char *imei_file = IMEI_FILE_NAME;
    vfs_sfile_init(imei_file);
    int len = vfs_sfile_write(imei_file, imei, sizeof(nvmImei_t));
    if (len != sizeof(nvmImei_t))
    {
        OSI_LOGE(0, "iemi - write fail.");
        return -1;
    }

    return sizeof(nvmImei_t);
}

int nvmReadSN(int index, void *sn, int size)
{
    if (sn == NULL)
        return -1;
    if (size > SN_MAX_LENGTH)
        size = SN_MAX_LENGTH;
    const char *snfile = SN_FILE_NAME;
    vfs_sfile_init(snfile);
    int len = vfs_sfile_read(snfile, sn, size);

    return len;
}
