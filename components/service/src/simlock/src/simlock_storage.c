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

#include "osi_api.h"
#include "osi_log.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "simlock_storage.h"
#include "vfs.h"
#include "srv_config.h"
#define SIMLOCK_DATA_FILE "simlock_data.bin"

#define STOR_LEN_ANY 1

void simlockDump(uint8_t *pbuf, uint32_t len)
{
    uint32_t i = 0;
    uint32_t *data = (uint32_t *)pbuf;

    OSI_LOGI(0, "simlock dump data the valid data len is %d", len);
    len = (len + 3) / 4;
    for (i = 0; i < len; i += 8)
    {
        OSI_LOGI(0, "simlock %d: %04x %04x %04x %04x %04x %04x %04x %04x", i * 8,
                 __ntohl(data[i]), __ntohl(data[i + 1]), __ntohl(data[i + 2]), __ntohl(data[i + 3]),
                 __ntohl(data[i + 4]), __ntohl(data[i + 5]), __ntohl(data[i + 6]), __ntohl(data[i] + 7));
    }
}

void simlockDumpFile(void)
{
    uint32_t read_len;
    char *buf = malloc(sizeof(SIMLOCK_STORAGE_STR));

    uint32_t file_size = vfs_file_size(SIMLOCK_DATA_FILE);
    if (file_size < 0)
    {
        OSI_LOGI(0, "SIMLOCK: file is nor exist");
        return;
    }
    read_len = vfs_file_read(SIMLOCK_DATA_FILE, buf, file_size);

    if (read_len != file_size)
        OSI_LOGI(0, "SIMLOCK: read file fail read length=%d, read_len = %d", file_size, read_len);

    simlockDump((uint8_t *)buf, file_size);

    free(buf);
}

bool simlockStorageRead(void *buffer,
                        uint32_t stor_offset, uint32_t stor_length,
                        uint32_t offset, uint32_t length)
{
    bool ret = false;
    int fd;
    int32_t read_len;
    if (stor_length != STOR_LEN_ANY)
    {
        if (!(offset + length <= stor_length))
        {
            OSI_LOGI(0, "SIMLOCK: read error offset=%d length=%d stor_length = %d\n",
                     offset, length, stor_length);
            return false;
        }
    }

    fd = vfs_open(SIMLOCK_DATA_FILE, O_WRONLY);
    if (fd < 0)
    {
        OSI_LOGI(0, "SIMLOCK: open file fail");
        return false;
    }

    vfs_lseek(fd, stor_offset + offset, SEEK_SET);
    memset(buffer, 0, length);
    if ((read_len = vfs_read(fd, buffer, length)) != length)
    {
        OSI_LOGI(0, "SIMLOCK: read file fail read length=%d, read_len = %d", length, read_len);
        vfs_close(fd);
        ret = false;
    }
    else
        ret = true;

    vfs_close(fd);

    return ret;
}

bool simlockStorageWrite(void *buffer,
                         uint32_t stor_offset, uint32_t stor_length,
                         uint32_t offset, uint32_t length)
{
    bool ret = false;
    int fd;
    int32_t write_len;

    if (stor_length != STOR_LEN_ANY)
    {
        if (!(offset + length <= stor_length))
        {
            OSI_LOGI(0, "SIMLOCK: write error offset=%d length=%d stor_length = %d\n", offset, length, stor_length);
            return false;
        }
    }
    fd = vfs_open(SIMLOCK_DATA_FILE, O_WRONLY);
    if (fd < 0)
    {
        OSI_LOGI(0, "SIMLOCK: open file fail");
        return false;
    }

    vfs_lseek(fd, stor_offset + offset, SEEK_SET);

    if ((write_len = vfs_write(fd, buffer, length)) != length)
    {
        OSI_LOGI(0, "SIMLOCK: write file fail write length=%d, write_len = %d", length, write_len);
        ret = false;
    }
    else
        ret = true;

    vfs_close(fd);

    return ret;
}

bool simlockStorageInit(void)
{
    int32_t fd;
    int32_t file_size = vfs_file_size(SIMLOCK_DATA_FILE);
    int32_t simlock_storage_size = sizeof(SIMLOCK_STORAGE_STR);
    int32_t write_len;

    if (file_size < 0)
    {
        fd = vfs_open(SIMLOCK_DATA_FILE, O_CREAT | O_WRONLY);
        if (fd < 0)
        {
            OSI_LOGI(0, "SIMLOCK: simlock create file fail");
            return false;
        }
        char *buf = malloc(simlock_storage_size);
        memset(buf, 0, simlock_storage_size);
        if ((write_len = vfs_write(fd, buf, simlock_storage_size)) != simlock_storage_size)
            OSI_LOGI(0, "SIMLOCK: simlock init file fail length=%d, write_len = %d", simlock_storage_size, write_len);

        vfs_close(fd);
    }

    return true;
}
