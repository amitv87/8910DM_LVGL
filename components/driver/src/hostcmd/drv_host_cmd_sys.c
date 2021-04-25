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

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_host_cmd.h"
#include "drv_host_cmd_imp.h"
#include "hal_config.h"
#include "drv_config.h"
#include "osi_api.h"
#include "osi_api_inside.h"
#include "osi_trace.h"
#include "osi_log.h"
#include "osi_mem.h"
#include "osi_sysnv.h"
#include "osi_byte_buf.h"
#include "osi_clock.h"
#include "osi_profile.h"
#include "vfs.h"
#include "calclib/crc32.h"
#include "app_loader.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PACKET_HEAD_LEN (5) // sync, frame_len(2), id, cmd
#define PACKET_OVERHEAD (6) // sync, frame_len(2), id, cmd, ..., crc
#define PAYLOAD_MAX (CONFIG_HOST_CMD_ENGINE_MAX_PACKET_SIZE - PACKET_OVERHEAD)

#define FILE_CRC_CHUNK_SIZE (128)

extern int osi_nbtimer_dump(void *mem, unsigned size);
extern int sxr_TimerDump(void *mem, unsigned size);

static unsigned prvFileCrc(const char *fname)
{
    int fd = vfs_open(fname, O_RDONLY);
    if (fd < 0)
        return 0;

    char buf[FILE_CRC_CHUNK_SIZE];
    unsigned crc = crc32Init();
    for (;;)
    {
        int rsize = vfs_read(fd, buf, FILE_CRC_CHUNK_SIZE);
        if (rsize <= 0)
            break;

        crc = crc32Update(crc, buf, rsize);
    }
    vfs_close(fd);
    return crc;
}

static int prvReadFile(const char *fname, int offset, int size, void *data)
{
    if (size == 0)
        return 0;

    int fd = vfs_open(fname, O_RDONLY);
    if (fd < 0)
        return false;

    if (vfs_lseek(fd, offset, SEEK_SET) != offset)
    {
        vfs_close(fd);
        return -1;
    }

    int rsize = vfs_read(fd, data, size);
    vfs_close(fd);
    return rsize;
}

static int prvWriteFile(const char *fname, int offset, int size, const void *data)
{
    if (size == 0)
        return 0;

    // create parent directories, and not care whether it can success
    vfs_mkfilepath(fname, 0);

    int fd = vfs_open(fname, O_RDWR | O_CREAT);
    if (fd < 0)
        return false;

    if (vfs_lseek(fd, offset, SEEK_SET) != offset)
    {
        vfs_close(fd);
        return -1;
    }

    int wsize = vfs_write(fd, data, size);
    vfs_close(fd);
    return wsize;
}

static int prvListDir(const char *dname, uint8_t *result, int size)
{
    DIR *dir = vfs_opendir(dname);
    if (dir == NULL)
        return -1;

    unsigned dnamelen = strlen(dname);
    char *childname = (char *)malloc(VFS_PATH_MAX);
    struct dirent *ent = (struct dirent *)calloc(1, sizeof(struct dirent));
    if (childname == NULL || ent == NULL)
        goto close_exit;

    // dname maybe destroyed at writing result
    memcpy(childname, dname, dnamelen);
    if (dname[dnamelen - 1] != '/')
        childname[dnamelen++] = '/';

    struct dirent *entout = NULL;
    int rpos = 0;
    struct stat st;
    while (vfs_readdir_r(dir, ent, &entout) >= 0)
    {
        if (entout == NULL)
            break;

        strcpy(childname + dnamelen, ent->d_name);
        unsigned childlen = strlen(childname);
        if (rpos + 4 + 4 + childlen + 1 > size)
            break;

        if (ent->d_type == DT_REG)
        {
            if (vfs_stat(childname, &st) != 0)
                goto close_exit;

            osiBytesPutLe32(result + rpos, st.st_mode);
            osiBytesPutLe32(result + rpos + 4, st.st_size);
        }
        else
        {
            osiBytesPutLe32(result + rpos, 0);
            osiBytesPutLe32(result + rpos + 4, 0);
        }

        memcpy(result + rpos + 8, childname, childlen + 1);
        rpos += 4 + 4 + childlen + 1;
    }

    free(childname);
    free(ent);
    vfs_closedir(dir);
    return rpos;

close_exit:
    free(childname);
    free(ent);
    vfs_closedir(dir);
    return -1;
}

static int prvListPartition(uint8_t *result, unsigned size)
{
    int part_count = vfs_mount_count();
    char **mps = alloca(part_count * sizeof(char *));
    vfs_mount_points(mps, part_count);

    uint8_t *result_start = result;
    for (int n = 0; n < part_count; n++)
    {
        struct statvfs st;
        if (vfs_statvfs(mps[n], &st) != 0)
            continue;

        int mps_len = strlen(mps[n]);
        OSI_STRM_WMEM(result, mps[n], mps_len + 1);
        OSI_STRM_WLE32(result, st.f_bsize);
        OSI_STRM_WLE32(result, st.f_blocks);
        OSI_STRM_WLE32(result, st.f_bfree);
        OSI_STRM_WLE32(result, st.f_bavail);
        OSI_STRM_WLE32(result, st.f_flag);
    }
    return OSI_PTR_DIFF(result, result_start);
}

void drvHostSyscmdHandler(drvHostCmdEngine_t *cmd, uint8_t *packet, unsigned packet_len)
{
    uint8_t cmd_code = packet[4];
    uint8_t *payload = packet + PACKET_HEAD_LEN;

    if (cmd_code == HOST_SYSCMD_PING)
    {
        // send the packet back
        drvHostCmdSendResponse(cmd, packet, packet_len);
    }
    else if (cmd_code == HOST_SYSCMD_PANIC)
    {
        osiPanic();
    }
    else if (cmd_code == HOST_SYSCMD_SHUTDOWN)
    {
        osiShutdownMode_t mode = osiBytesGetLe16(payload);
        osiShutdown(mode);
    }
    else if (cmd_code == HOST_SYSCMD_READ_SYSNV)
    {
        int nvsize = osiSysnvSize();
        if (nvsize < 0 || nvsize + 2 > PAYLOAD_MAX)
        {
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        }
        else
        {
            osiBytesPutLe16(payload, nvsize);
            osiSysnvLoadToMem(payload + 2, nvsize);
            drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + 2 + nvsize);
        }
    }
    else if (cmd_code == HOST_SYSCMD_WRITE_SYSNV) // save system NV
    {
        uint16_t nvsize = osiBytesGetLe16(payload);
        if (osiSysnvSaveFromMem(payload + 2, nvsize) < 0)
        {
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        }
        else
        {
            drvHostCmdSendResultCode(cmd, packet, nvsize);
        }
    }
    else if (cmd_code == HOST_SYSCMD_SAVE_CURR_SYSNV)
    {
        drvHostCmdSendResultCode(cmd, packet, osiSysnvSave() ? 0 : 0xffff);
    }
    else if (cmd_code == HOST_SYSCMD_CLEAR_SYSNV)
    {
        osiSysnvClear();
        osiBytesPutLe16(packet + 5, 0);
        drvHostCmdSendResultCode(cmd, packet, 0);
    }
    else if (cmd_code == HOST_SYSCMD_HEAPINFO) // heap info
    {
        osiMemPoolStat_t stat;
        osiMemPoolStat(NULL, &stat);

        osiBytesPut8(payload, 1);
        osiBytesPutLe32(payload + 1, (uint32_t)stat.start);
        osiBytesPutLe32(payload + 5, stat.size);
        osiBytesPutLe32(payload + 9, stat.avail_size);
        osiBytesPutLe32(payload + 13, stat.max_block_size);
        drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + 17);
    }
    else if (cmd_code == HOST_SYSCMD_TIMERINFO)
    {
        int size = osiTimerDump(payload, PAYLOAD_MAX);
        if (size <= 0)
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        else
            drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + size);
    }
    else if (cmd_code == HOST_SYSCMD_NBTIMERINFO)
    {
#if defined(CONFIG_SOC_8811) || defined(CONFIG_SOC_8909)
        int size = osi_nbtimer_dump(payload, PAYLOAD_MAX);
        if (size <= 0)
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        else
            drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + size);
#else
        osiBytesPutLe16(payload, 0);
        drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + 2);
#endif
    }
    else if (cmd_code == HOST_SYSCMD_SXTIMERINFO)
    {
#if defined(CONFIG_SOC_8811) || defined(CONFIG_SOC_8909) || defined(CONFIG_SOC_8955)
        int size = sxr_TimerDump(payload, PAYLOAD_MAX);
        if (size <= 0)
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        else
            drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + size);
#else
        osiBytesPutLe16(payload, 0);
        drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + 2);
#endif
    }
    else if (cmd_code == HOST_SYSCMD_PMSOURCEINFO)
    {
        int size = osiPmSourceDump(payload, PAYLOAD_MAX);
        if (size <= 0)
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        else
            drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + size);
    }
    else if (cmd_code == HOST_SYSCMD_CLOCKINFO)
    {
        int size = osiClockConstrainDump(payload, PAYLOAD_MAX);
        if (size <= 0)
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        else
            drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + size);
    }
    else if (cmd_code == HOST_SYSCMD_FILEINFO)
    {
        const char *fname = (const char *)payload;
        struct stat st;
        if (vfs_stat(fname, &st) < 0)
        {
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        }
        else
        {
            unsigned crc = prvFileCrc(fname); // fname will be overwritten at put result
            osiBytesPutLe32(payload, st.st_mode);
            osiBytesPutLe32(payload + 4, st.st_size);
            osiBytesPutLe32(payload + 8, crc);
            drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + 12);
        }
    }
    else if (cmd_code == HOST_SYSCMD_READFILE)
    {
        int offset = osiBytesGetLe32(payload);
        int size = osiBytesGetLe16(payload + 4);
        const char *fname = (const char *)(payload + 6);

        if (2 + size > PAYLOAD_MAX ||
            prvReadFile(fname, offset, size, payload + 2) != size)
        {
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        }
        else
        {
            osiBytesPutLe16(payload, size);
            drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + 2 + size);
        }
    }
    else if (cmd_code == HOST_SYSCMD_WRITEFILE)
    {
        int offset = osiBytesGetLe32(payload);
        int size = osiBytesGetLe16(payload + 4);
        const char *fname = (const char *)(payload + 6);
        int fname_len = strnlen(fname, PAYLOAD_MAX - 6);
        uint8_t *data = payload + 4 + 2 + fname_len + 1;

        if (4 + 2 + fname_len + 1 + size > PAYLOAD_MAX ||
            prvWriteFile(fname, offset, size, data) != size)
        {
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        }
        else
        {
            drvHostCmdSendResultCode(cmd, packet, 0);
        }
    }
    else if (cmd_code == HOST_SYSCMD_LISTDIR)
    {
        const char *dname = (const char *)payload;
        int size = prvListDir(dname, payload + 2, PAYLOAD_MAX - 2);
        if (size < 0)
        {
            drvHostCmdSendResultCode(cmd, packet, 0xffff);
        }
        else
        {
            osiBytesPutLe16(payload, size);
            drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + 2 + size);
        }
    }
    else if (cmd_code == HOST_SYSCMD_DELFILE)
    {
        const char *fname = (const char *)payload;
        uint16_t error = (vfs_unlink(fname) < 0) ? 0xffff : 0;
        drvHostCmdSendResultCode(cmd, packet, error);
    }
    else if (cmd_code == HOST_SYSCMD_MKDIR)
    {
        const char *dname = (const char *)payload;
        uint16_t error = (vfs_mkdir(dname, 0) < 0) ? 0xffff : 0;
        drvHostCmdSendResultCode(cmd, packet, error);
    }
    else if (cmd_code == HOST_SYSCMD_RMDIR)
    {
        const char *dname = (const char *)payload;
        uint16_t error = (vfs_rmdir(dname) < 0) ? 0xffff : 0;
        drvHostCmdSendResultCode(cmd, packet, error);
    }
    else if (cmd_code == HOST_SYSCMD_MKPATH)
    {
        const char *dname = (const char *)payload;
        uint16_t error = (vfs_mkpath(dname, 0) < 0) ? 0xffff : 0;
        drvHostCmdSendResultCode(cmd, packet, error);
    }
    else if (cmd_code == HOST_SYSCMD_RMPATH)
    {
        const char *dname = (const char *)payload;
        uint16_t error = (vfs_rmdir_recursive(dname) < 0) ? 0xffff : 0;
        drvHostCmdSendResultCode(cmd, packet, error);
    }
    else if (cmd_code == HOST_SYSCMD_RENAME)
    {
        const char *src = (const char *)payload;
        unsigned src_len = strnlen(src, VFS_PATH_MAX);
        const char *dst = src + src_len + 1;
        uint16_t error = (vfs_rename(src, dst) < 0) ? 0xffff : 0;
        drvHostCmdSendResultCode(cmd, packet, error);
    }
    else if (cmd_code == HOST_SYSCMD_LISTPARTION)
    {
        int size = prvListPartition(payload + 2, PAYLOAD_MAX - 2);
        osiBytesPutLe16(payload, size);
        drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + 2 + size);
    }
#ifdef CONFIG_APPIMG_LOAD_FLASH
    else if (cmd_code == HOST_SYSCMD_APPIMG_FLASH_GET)
    {
        if (gAppImgFlash.get_param != NULL)
        {
            // When non-zero, expected return pointer content size
            unsigned value_size = osiBytesGetLe16(payload);
            unsigned id = osiBytesGetLe32(payload + 2);
            void *raw_value = payload + 6;
            void *value = (void *)OSI_ALIGN_UP((uintptr_t)raw_value, 8);
            if (gAppImgFlash.get_param(id, value))
            {
                osiBytesPutLe16(payload, value_size);
                if (value != payload + 2)
                    memmove(payload + 2, value, value_size);
                drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + value_size + 2);
                return;
            }
        }
        drvHostCmdSendResultCode(cmd, packet, 0xffff);
    }
    else if (cmd_code == HOST_SYSCMD_APPIMG_FLASH_SET)
    {
        if (gAppImgFlash.set_param != NULL)
        {
            unsigned value_size = osiBytesGetLe16(payload);
            unsigned id = osiBytesGetLe32(payload + 2);
            void *raw_value = payload + 6;
            void *value = (void *)OSI_ALIGN_UP((uintptr_t)raw_value, 8);
            if (value != raw_value)
                memmove(value, raw_value, value_size);

            if (gAppImgFlash.set_param(id, value))
            {
                osiBytesPutLe16(payload, value_size);
                drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + 2);
                return;
            }
        }
        drvHostCmdSendResultCode(cmd, packet, 0xffff);
    }
#endif
#ifdef CONFIG_APPIMG_LOAD_FILE
    else if (cmd_code == HOST_SYSCMD_APPIMG_FILE_GET)
    {
        if (gAppImgFile.get_param != NULL)
        {
            // When non-zero, expected return pointer content size
            unsigned value_size = osiBytesGetLe16(payload);
            unsigned id = osiBytesGetLe32(payload + 2);
            void *raw_value = payload + 6;
            void *value = (void *)OSI_ALIGN_UP((uintptr_t)raw_value, 8);
            if (gAppImgFile.get_param(id, value))
            {
                osiBytesPutLe16(payload, value_size);
                if (value != payload + 2)
                    memmove(payload + 2, value, value_size);
                drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + value_size + 2);
                return;
            }
        }
        drvHostCmdSendResultCode(cmd, packet, 0xffff);
    }
    else if (cmd_code == HOST_SYSCMD_APPIMG_FILE_SET)
    {
        if (gAppImgFile.set_param != NULL)
        {
            unsigned value_size = osiBytesGetLe16(payload);
            unsigned id = osiBytesGetLe32(payload + 2);
            void *raw_value = payload + 6;
            void *value = (void *)OSI_ALIGN_UP((uintptr_t)raw_value, 8);
            if (value != raw_value)
                memmove(value, raw_value, value_size);

            if (gAppImgFile.set_param(id, value))
            {
                osiBytesPutLe16(payload, value_size);
                drvHostCmdSendResponse(cmd, packet, PACKET_OVERHEAD + 2);
                return;
            }
        }
        drvHostCmdSendResultCode(cmd, packet, 0xffff);
    }
#endif
    else if (cmd_code == HOST_SYSCMD_PROFILEMODE)
    {
        osiProfileMode_t mode = (osiProfileMode_t)osiBytesGet8(payload);
        osiProfileSetMode(mode);
        drvHostCmdSendResultCode(cmd, packet, 0);
    }
    else
    {
        drvHostInvalidCmd(cmd, packet, packet_len);
    }
}
