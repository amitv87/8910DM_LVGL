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

#include "atr_config.h"
#ifdef CONFIG_ATS_FS_SUPPORT

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('A', 'T', 'F', 'S')
#define FILE_NAME_MAX (64)

#include "osi_log.h"
#include "at_command.h"
#include "at_response.h"
#include "vfs.h"
#include <sys/errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
    uint32_t size;
    uint32_t pos;
    char *mem;
    const char *file_name;
} fsdwnfileAsyncCtx_t;

static void _dwnfile(atCommand_t *cmd, const char *file_name, const void *data, size_t size)
{
    ssize_t wlen = vfs_file_write(file_name, data, size);
    if (wlen != size)
    {
        OSI_LOGXE(OSI_LOGPAR_SIII, 0, "AT FS write %s %d failed: %d/%d",
                  file_name, size, wlen, errno);
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
    {
        atCmdRespOK(cmd->engine);
    }
}

static int _dwnfilePushData(void *param, const void *data, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    fsdwnfileAsyncCtx_t *async = (fsdwnfileAsyncCtx_t *)cmd->async_ctx;

    OSI_LOGI(0, "AT download file size/%d pos/%d data_size/%d",
             async->size, async->pos, size);

    size_t len = async->size - async->pos;
    if (len > size)
        len = size;
    memcpy(async->mem + async->pos, data, len);
    async->pos += len;

    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    if (chsetting->ate)
        atCmdWrite(cmd->engine, data, len);

    if (async->pos >= async->size)
        _dwnfile(cmd, async->file_name, async->mem, async->size);
    return len;
}

// +FSDWNFILE: Download file
void atCmdHandleFSDWNFILE(atCommand_t *cmd)
{
    if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+FSDWNFILE: file_name,size[,tag]");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_SET == cmd->type)
    {
        // +FSDWNFILE: file_name,size
        bool paramok = true;
        const char *file_name = atParamStr(cmd->params[0], &paramok);
        uint32_t length = atParamUintInRange(cmd->params[1], 0, CONFIG_ATS_FS_DWN_SIZE_MAX, &paramok);
        if (!paramok || cmd->param_count > 2 || strcmp(file_name, "") == 0 || strlen(file_name) > FILE_NAME_MAX)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (length == 0)
        {
            _dwnfile(cmd, file_name, NULL, 0); // final result code will be sent
            return;
        }

        fsdwnfileAsyncCtx_t *async = (fsdwnfileAsyncCtx_t *)malloc(sizeof(*async) + length);
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        async->size = length;
        async->pos = 0;
        async->file_name = file_name;
        async->mem = (char *)async + sizeof(*async);

        atCmdRespOutputPrompt(cmd->engine);
        atCmdSetBypassMode(cmd->engine, _dwnfilePushData, cmd);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// +FSRDFILE: Read file
void atCmdHandleFSRDFILE(atCommand_t *cmd)
{
    if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+FSRDFILE: file_name[,tag]");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_SET == cmd->type)
    {
        // +FSRDFILE: file_name

        bool paramok = true;
        const char *file_name = atParamStr(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        int fd = vfs_open(file_name, O_RDONLY);
        if (fd < 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_FILE_NOT_EXIST);

        struct stat st = {};
        vfs_fstat(fd, &st);
        int file_size = st.st_size;
        if (file_size > CONFIG_ATS_FS_RD_SIZE_MAX)
        {
            vfs_close(fd);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_FILE_TOO_LARGE);
        }

        char buffer[256];
        atCmdRespInfoTextBegin(cmd->engine, "+FSRDFILE: ");
        atCmdRespOutputText(cmd->engine, file_name);
        sprintf(buffer, ",%d,", file_size);
        atCmdRespOutputText(cmd->engine, buffer);

        while (file_size > 0)
        {
            int read_size = (file_size > 256) ? 256 : file_size;
            int read_len = vfs_read(fd, buffer, read_size);
            if (read_len <= 0)
                break;

            atCmdRespOutputNText(cmd->engine, buffer, read_len);
            file_size -= read_len;
        }

        vfs_close(fd);
        atCmdRespInfoTextEnd(cmd->engine, "");
        if (file_size == 0)
            atCmdRespOK(cmd->engine);
        else
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// +FSRDBLOCK: Read file block
void atCmdHandleFSRDBLOCK(atCommand_t *cmd)
{
    if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+FSRDBLOCK: filename,offset,size[,tag]");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_SET == cmd->type)
    {
        // +FSRDBLOCK: file_name,offset,size

        bool paramok = true;
        const char *file_name = atParamStr(cmd->params[0], &paramok);
        uint32_t offset = atParamUint(cmd->params[1], &paramok);
        uint32_t size = atParamUintInRange(cmd->params[2], 1, CONFIG_ATS_FS_RD_SIZE_MAX, &paramok);
        if (!paramok || cmd->param_count > 3)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        int fd = vfs_open(file_name, O_RDONLY);
        if (fd < 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_FILE_NOT_EXIST);

        struct stat st = {};
        vfs_fstat(fd, &st);
        if (offset >= st.st_size)
        {
            vfs_close(fd);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (offset + size > st.st_size)
            size = st.st_size - offset;

        char buffer[256];
        atCmdRespInfoTextBegin(cmd->engine, "+FSRDBLOCK: ");
        atCmdRespOutputText(cmd->engine, file_name);
        sprintf(buffer, ",%ld,", size);
        atCmdRespOutputText(cmd->engine, buffer);

        vfs_lseek(fd, offset, SEEK_SET);
        while (size > 0)
        {
            int read_size = (size > 256) ? 256 : size;
            int read_len = vfs_read(fd, buffer, read_size);
            if (read_len <= 0)
                break;

            atCmdRespOutputNText(cmd->engine, buffer, read_len);
            size -= read_len;
        }
        vfs_close(fd);

        atCmdRespInfoTextEnd(cmd->engine, "");
        if (size == 0)
            atCmdRespOK(cmd->engine);
        else
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// +FSDELFILE: Delete file
void atCmdHandleFSDELFILE(atCommand_t *cmd)
{
    if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespInfoText(cmd->engine, "+FSDELFILE: filename[,tag]");
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_SET == cmd->type)
    {
        // +FSDELFILE: file_name

        bool paramok = true;
        const char *file_name = atParamStr(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (vfs_unlink(file_name) < 0)
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_FILE_NOT_EXIST);
        else
            atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// +FSMKDIR: Create directory
void atCmdHandleFSMKDIR(atCommand_t *cmd)
{
    if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_SET == cmd->type)
    {
        // +FSMKDIR: dir_name

        bool paramok = true;
        const char *dir_name = atParamStr(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (vfs_mkdir(dir_name, 0) < 0)
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_DIR_CREATE_FAIL);
        else
            atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// +FSRMDIR: Remove directory
void atCmdHandleFSRMDIR(atCommand_t *cmd)
{
    if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_SET == cmd->type)
    {
        // +FSRMDIR: dir_name

        bool paramok = true;
        const char *dir_name = atParamStr(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (vfs_rmdir(dir_name) < 0)
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_DIR_NOT_EXIST);
        else
            atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// +FSLSTFILE: List file or directory
void atCmdHandleFSLSTFILE(atCommand_t *cmd)
{
    char rsp[VFS_PATH_MAX + 84];

    if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_SET == cmd->type)
    {
        // +FSLSTFILE: file_dir_name

        bool paramok = true;
        const char *name = atParamStr(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        struct stat st;
        if (vfs_stat(name, &st) == 0 && !(st.st_mode & S_IFDIR))
        {
            sprintf(rsp, "+FSLSTFILE: \"%s\",%ld", name, st.st_size);
            atCmdRespInfoText(cmd->engine, rsp);
            RETURN_OK(cmd->engine);
        }

        DIR *dir = vfs_opendir(name);
        if (dir != NULL)
        {
            size_t name_len = strlen(name);
            bool trail_slash = (name_len > 0 && name[name_len - 1] == '/');
            struct dirent *ent;
            while ((ent = vfs_readdir(dir)) != NULL)
            {
                if (ent->d_type == DT_REG)
                {
                    // borrow rsp for full_path
                    sprintf(rsp, "%s/%s", name, ent->d_name);
                    osiThreadSleep(50);
                    if (vfs_stat(rsp, &st) != 0)
                        continue;

                    if (trail_slash)
                        sprintf(rsp, "+FSLSTFILE: \"%s%s\",%ld", name, ent->d_name, st.st_size);
                    else
                        sprintf(rsp, "+FSLSTFILE: \"%s/%s\",%ld", name, ent->d_name, st.st_size);
                    atCmdRespInfoText(cmd->engine, rsp);
                }
                else if (ent->d_type == DT_DIR)
                {
                    if (trail_slash)
                        sprintf(rsp, "+FSLSTFILE: \"%s%s\"", name, ent->d_name);
                    else
                        sprintf(rsp, "+FSLSTFILE: \"%s/%s\"", name, ent->d_name);
                    atCmdRespInfoText(cmd->engine, rsp);
                }
            }
            vfs_closedir(dir);
            RETURN_OK(cmd->engine);
        }

        atCmdRespCmeError(cmd->engine, ERR_AT_CME_FILE_NOT_EXIST);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// +FSLSTPART: List partition free space
void atCmdHandleFSLSTPART(atCommand_t *cmd)
{
    if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_EXE == cmd->type)
    {
        int part_count = vfs_mount_count();
        char **mps = alloca(part_count * sizeof(char *));
        vfs_mount_points(mps, part_count);

        char rsp[VFS_PATH_MAX + 32];
        for (int n = 0; n < part_count; n++)
        {
            struct statvfs st;
            if (vfs_statvfs(mps[n], &st) != 0)
                continue;

            sprintf(rsp, "+FSLSTPART: \"%s\",%llu", mps[n], (uint64_t)st.f_bavail * st.f_bsize);
            atCmdRespInfoText(cmd->engine, rsp);
        }
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

// +FSREMOUNT: Remount as read-only or read-write
void atCmdHandleFSREMOUNT(atCommand_t *cmd)
{
    if (AT_CMD_TEST == cmd->type)
    {
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_SET == cmd->type)
    {
        // +FSREMOUNT: part_name[,flags]

        bool paramok = true;
        const char *part_name = atParamStr(cmd->params[0], &paramok);
        unsigned flags = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (vfs_remount(part_name, flags) < 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        atCmdRespOK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
#endif
