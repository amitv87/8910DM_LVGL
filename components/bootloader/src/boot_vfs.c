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

#include "vfs.h"
#include "vfs_ops.h"
#include "sffs.h"
#include "block_device.h"
#include "osi_compiler.h"
#include "osi_api.h"
#include "osi_log.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/queue.h>

#define VFS_FD_POS (0)
#define VFS_FD_MASK (0x3ff)
#define VFS_INDEX_POS (10)
#define VFS_INDEX_MASK (0x7c00)

// #define SET_ERRNO(err) OSI_DO_WHILE0(errno = err)
#define SET_ERRNO(err)
#define ERR_RETURN(err, ret) OSI_DO_WHILE0(SET_ERRNO(err); return (ret);)
#define CHECK_RESULT(res) OSI_DO_WHILE0(if ((res) < 0) {SET_ERRNO(-(res)); return -1; })
#define CHECK_RESULT_RETURN(res) OSI_DO_WHILE0(if ((res) < 0) { SET_ERRNO(-(res)); return -1; } else return (res);)

typedef struct bootFileInfo
{
    sffsFs_t *fs;
    const char *local_path;
    uint16_t local_fd;
    uint8_t fs_index;
} bootFileInfo_t;

typedef SLIST_ENTRY(bootVfsRegistry) bootVfsRegistryIter_t;
typedef SLIST_HEAD(bootVfsRegistryHead, bootVfsRegistry) bootVfsRegistryHead_t;
typedef struct bootVfsRegistry
{
    sffsFs_t *fs;
    char prefix[VFS_PREFIX_MAX];
    uint8_t prefix_len;
    uint8_t index;

    bootVfsRegistryIter_t iter;
} bootVfsRegistry_t;

static bootVfsRegistryHead_t gBootVfsRegList = SLIST_HEAD_INITIALIZER(&gBootVfsRegList);

static bootFileInfo_t _getFileByPath(const char *path)
{
    bootFileInfo_t fi = {};
    int match_len = 0;

    bootVfsRegistry_t *r;
    SLIST_FOREACH(r, &gBootVfsRegList, iter)
    {
        if (r->prefix_len < match_len)
            continue;
        if (memcmp(path, r->prefix, r->prefix_len) != 0)
            continue;

        if (path[r->prefix_len] == '\0') // exact match
        {
            fi.fs = r->fs;
            fi.fs_index = r->index;
            fi.local_path = path + r->prefix_len;
            break;
        }

        if (r->prefix_len == 1)
        {
            fi.fs = r->fs;
            fi.fs_index = r->index;
            fi.local_path = path + r->prefix_len;
            match_len = r->prefix_len;
            continue;
        }

        if (path[r->prefix_len] != '/')
            continue;

        fi.fs = r->fs;
        fi.fs_index = r->index;
        fi.local_path = path + r->prefix_len + 1;
        match_len = r->prefix_len;
    }
    return fi;
}

static bootFileInfo_t _getFileByFd(int fd)
{
    bootFileInfo_t fi = {};
    uint8_t fs_index = (fd & VFS_INDEX_MASK) >> VFS_INDEX_POS;

    bootVfsRegistry_t *r;
    SLIST_FOREACH(r, &gBootVfsRegList, iter)
    {
        if (r->index == fs_index)
        {
            fi.local_fd = (fd & VFS_FD_MASK) >> VFS_FD_POS;
            fi.fs_index = fs_index;
            fi.fs = r->fs;
            break;
        }
    }
    return fi;
}

bool bootVfsMount(const char *path, sffsFs_t *fs)
{
    bootVfsRegistry_t *r;
    SLIST_FOREACH(r, &gBootVfsRegList, iter)
    {
        if (memcmp(path, r->prefix, r->prefix_len + 1) == 0)
            return false; // already registered
    }

    int index = 1;
    SLIST_FOREACH(r, &gBootVfsRegList, iter)
    {
        if (r->index >= index)
            index = r->index + 1;
    }

    r = calloc(1, sizeof(bootVfsRegistry_t));
    if (r == NULL)
        return false;
    r->prefix_len = strlen(path);
    memcpy(r->prefix, path, r->prefix_len);
    r->index = index;
    r->fs = fs;
    SLIST_INSERT_HEAD(&gBootVfsRegList, r, iter);
    return true;
}

void *bootVfsMountHandle(const char *path)
{
    if (path == NULL)
        return NULL;

    size_t len = strlen(path);
    if (len == 0 || len >= VFS_PREFIX_MAX)
        return NULL;

    bootVfsRegistry_t *r;
    SLIST_FOREACH(r, &gBootVfsRegList, iter)
    {
        if (memcmp(path, r->prefix, r->prefix_len + 1) == 0)
            return r->fs;
    }

    return NULL;
}

int bootVfsUmount(const char *path)
{
    bootVfsRegistry_t *r;
    SLIST_FOREACH(r, &gBootVfsRegList, iter)
    {
        if (memcmp(path, r->prefix, r->prefix_len + 1) == 0)
        {
            sffsUnmount(r->fs);
            SLIST_REMOVE(&gBootVfsRegList, r, bootVfsRegistry, iter);
            free(r);
            return 0;
        }
    }

    ERR_RETURN(ENOENT, -1);
}

int bootVfsRemount(const char *path, unsigned flags)
{
    if (NULL == path)
        ERR_RETURN(ENOENT, -1);

    bootVfsRegistry_t *r;
    SLIST_FOREACH(r, &gBootVfsRegList, iter)
    {
        if (memcmp(path, r->prefix, r->prefix_len + 1) == 0)
        {
            sffsRemount(r->fs, flags);
            return 0;
        }
    }

    ERR_RETURN(ENOENT, -1);
}

void bootVfsUmountAll(void)
{
    while (!SLIST_EMPTY(&gBootVfsRegList))
    {
        bootVfsRegistry_t *r = SLIST_FIRST(&gBootVfsRegList);
        SLIST_REMOVE_HEAD(&gBootVfsRegList, iter);

        sffsUnmount(r->fs);
        free(r);
    }
}

int bootVfsOpen(const char *path, int flags, ...)
{
    bootFileInfo_t fi = _getFileByPath(path);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    int fd = sffsOpen(fi.fs, fi.local_path, flags);
    if (fd < 0)
        return -1;
    return fd + (fi.fs_index << VFS_INDEX_POS);
}

int bootVfsRead(int fd, void *data, size_t size)
{
    bootFileInfo_t fi = _getFileByFd(fd);
    if (fi.fs == NULL)
        ERR_RETURN(EBADF, -1);

    ssize_t res = sffsRead(fi.fs, fi.local_fd, data, size);
    CHECK_RESULT_RETURN(res);
}

int bootVfsWrite(int fd, const void *data, size_t size)
{
    bootFileInfo_t fi = _getFileByFd(fd);
    if (fi.fs == NULL)
        ERR_RETURN(EBADF, -1);

    ssize_t res = sffsWrite(fi.fs, fi.local_fd, data, size);
    CHECK_RESULT_RETURN(res);
}

long bootVfsLseek(int fd, long offset, int mode)
{
    bootFileInfo_t fi = _getFileByFd(fd);
    if (fi.fs == NULL)
        ERR_RETURN(EBADF, -1);

    off_t res = sffsSeek(fi.fs, fi.local_fd, offset, mode);
    CHECK_RESULT_RETURN(res);
}

int bootVfsFstat(int fd, struct stat *st)
{
    bootFileInfo_t fi = _getFileByFd(fd);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    int res = sffsFstat(fi.fs, fi.local_fd, st);
    CHECK_RESULT_RETURN(res);
}

int bootVfsClose(int fd)
{
    bootFileInfo_t fi = _getFileByFd(fd);
    if (fi.fs == NULL)
        ERR_RETURN(EBADF, -1);

    ssize_t res = sffsClose(fi.fs, fi.local_fd);
    CHECK_RESULT_RETURN(res);
}

int bootVfsUnlink(const char *path)
{
    bootFileInfo_t fi = _getFileByPath(path);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    ssize_t res = sffsUnlink(fi.fs, fi.local_path);
    CHECK_RESULT_RETURN(res);
}

int bootVfsRename(const char *src, const char *dest)
{
    bootFileInfo_t src_fi = _getFileByPath(src);
    bootFileInfo_t dest_fi = _getFileByPath(dest);
    if (src_fi.fs == NULL || dest_fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    if (src_fi.fs != dest_fi.fs)
        ERR_RETURN(EXDEV, -1);

    int res = sffsRename(src_fi.fs, src_fi.local_path, dest_fi.local_path);
    CHECK_RESULT_RETURN(res);
}

int bootVfsMkDir(const char *path, mode_t mode)
{
    bootFileInfo_t fi = _getFileByPath(path);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    int res = sffsMkDir(fi.fs, fi.local_path);
    CHECK_RESULT_RETURN(res);
}

int bootVfsRmDir(const char *path)
{
    bootFileInfo_t fi = _getFileByPath(path);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    int res = sffsRmDir(fi.fs, fi.local_path);
    CHECK_RESULT_RETURN(res);
}

int bootVfsStatVfs(const char *path, struct statvfs *buf)
{
    bootFileInfo_t fi = _getFileByPath(path);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    int res = sffsStatVfs(fi.fs, buf);
    CHECK_RESULT_RETURN(res);
}

int bootVfsFileSize(const char *path)
{
    int fd = bootVfsOpen(path, O_RDONLY);
    if (fd < 0)
        return -1; // errno already set

    struct stat st;
    int res = bootVfsFstat(fd, &st);
    bootVfsClose(fd);
    return (res < 0) ? -1 : st.st_size;
}

int bootVfsFileRead(const char *path, void *data, size_t size)
{
    int fd = bootVfsOpen(path, O_RDONLY);
    if (fd < 0)
        return -1; // errno already set

    int res = bootVfsRead(fd, data, size);
    bootVfsClose(fd);
    return res;
}

int bootVfsFileWrite(const char *path, const void *data, size_t size)
{
    bootFileInfo_t fi = _getFileByPath(path);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    int res = sffsSfileWrite(fi.fs, fi.local_path, data, size);
    CHECK_RESULT_RETURN(res);
}

int bootVfsSfileInit(const char *path)
{
    int fsize = bootVfsFileSize(path);
    return (fsize >= 0) ? 0 : -1;
}

int bootVfsSfileWrite(const char *path, const void *data, size_t size)
{
    bootFileInfo_t fi = _getFileByPath(path);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    int res = sffsSfileWrite(fi.fs, fi.local_path, data, size);
    CHECK_RESULT_RETURN(res);
}

int bootVfsMkPath(const char *path, mode_t mode)
{
    bootFileInfo_t fi = _getFileByPath(path);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    char *local_path = strdup(fi.local_path);
    char *sep = local_path;
    for (;;)
    {
        sep = strchr(sep, '/');
        if (sep != NULL)
            *sep = '\0';

        int res = sffsMkDir(fi.fs, local_path);
        if (res < 0 && res != -EEXIST)
        {
            SET_ERRNO(-res);
            free(local_path);
            return -1;
        }

        if (sep == NULL)
            break;

        *sep = '/';
        sep++;
    }

    free(local_path);
    return 0;
}

int bootVfsMkFilePath(const char *path, mode_t mode)
{
    bootFileInfo_t fi = _getFileByPath(path);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    char *local_path = strdup(fi.local_path);
    char *last = strrchr(local_path, '/');
    if (last == NULL) // no parent directory
    {
        free(local_path);
        return 0;
    }

    *last = '\0';
    char *sep = local_path;
    for (;;)
    {
        sep = strchr(sep, '/');
        if (sep != NULL)
            *sep = '\0';

        int res = sffsMkDir(fi.fs, local_path);
        if (res < 0 && res != -EEXIST)
        {
            SET_ERRNO(-res);
            free(local_path);
            return -1;
        }

        if (sep == NULL)
            break;

        *sep = '/';
        sep++;
    }

    free(local_path);
    return 0;
}

int bootVfsRmChildren(const char *path)
{
    bootFileInfo_t fi = _getFileByPath(path);
    if (fi.fs == NULL)
        ERR_RETURN(ENOENT, -1);

    DIR *dir = sffsOpenDir(fi.fs, fi.local_path);
    if (dir == NULL)
        ERR_RETURN(ENOENT, -1);

    int res = 0;
    struct dirent *ent = (struct dirent *)calloc(1, sizeof(struct dirent));
    struct dirent *result = NULL;
    while (sffsReadDirR(fi.fs, dir, ent, &result) >= 0)
    {
        if (result == NULL)
            break;

        if (ent->d_type == DT_REG)
        {
            char *child = strdup(fi.local_path);
            strcat(child, "/");
            strcat(child, ent->d_name);

            res = sffsUnlink(fi.fs, child);
            free(child);

            if (res < 0)
                break;
        }
        else if (ent->d_type == DT_DIR)
        {
            char *child = strdup(path);
            strcat(child, "/");
            strcat(child, ent->d_name);

            res = bootVfsRmChildren(child);
            if (res >= 0)
                res = bootVfsRmDir(child);
            free(child);

            if (res < 0)
                break;
        }
    }

    sffsCloseDir(fi.fs, dir);
    free(ent);
    return res;
}

int bootSffsMount(const char *base_path, blockDevice_t *bdev,
                  size_t cache_count, size_t sfile_reserved_lb,
                  bool read_only)
{
    sffsFs_t *fs = sffsMount(bdev, cache_count, read_only);
    if (fs == NULL)
        return -1;

    if (!bootVfsMount(base_path, fs))
    {
        sffsUnmount(fs);
        return -1;
    }

    if (sfile_reserved_lb != 0)
        sffsSetSfileReserveCount(fs, sfile_reserved_lb);

    return 0;
}

int bootSffsMkfs(blockDevice_t *bdev)
{
    int res = sffsMakeFs(bdev);
    return (res < 0) ? -1 : 0;
}

OSI_DECL_STRONG_ALIAS(bootVfsMountHandle, void *vfs_mount_handle(const char *path));
OSI_DECL_STRONG_ALIAS(bootVfsUmount, int vfs_umount(const char *path));
OSI_DECL_STRONG_ALIAS(bootVfsRemount, int vfs_remount(const char *path, unsigned flags));
OSI_DECL_STRONG_ALIAS(bootVfsUmountAll, void vfs_umount_all(void));
OSI_DECL_STRONG_ALIAS(bootVfsOpen, int vfs_open(const char *path, int flags, ...));
OSI_DECL_STRONG_ALIAS(bootVfsRead, int vfs_read(int fd, void *data, size_t size));
OSI_DECL_STRONG_ALIAS(bootVfsWrite, int vfs_write(int fd, const void *data, size_t size));
OSI_DECL_STRONG_ALIAS(bootVfsLseek, long vfs_lseek(int fd, long offset, int mode));
OSI_DECL_STRONG_ALIAS(bootVfsFstat, int vfs_fstat(int fd, struct stat *st));
OSI_DECL_STRONG_ALIAS(bootVfsClose, int vfs_close(int fd));
OSI_DECL_STRONG_ALIAS(bootVfsUnlink, int vfs_unlink(const char *path));
OSI_DECL_STRONG_ALIAS(bootVfsRename, int vfs_rename(const char *src, const char *dest));
OSI_DECL_STRONG_ALIAS(bootVfsMkDir, int vfs_mkdir(const char *path, mode_t mode));
OSI_DECL_STRONG_ALIAS(bootVfsRmDir, int vfs_rmdir(const char *path));
OSI_DECL_STRONG_ALIAS(bootVfsStatVfs, int vfs_statvfs(const char *path, struct statvfs *buf));
OSI_DECL_STRONG_ALIAS(bootVfsFileSize, int vfs_file_size(const char *path));
OSI_DECL_STRONG_ALIAS(bootVfsFileSize, int vfs_sfile_size(const char *path));
OSI_DECL_STRONG_ALIAS(bootVfsFileRead, int vfs_file_read(const char *path, void *data, size_t size));
OSI_DECL_STRONG_ALIAS(bootVfsFileRead, int vfs_sfile_read(const char *path, void *data, size_t size));
OSI_DECL_STRONG_ALIAS(bootVfsFileWrite, int vfs_file_write(const char *path, const void *data, size_t size));
OSI_DECL_STRONG_ALIAS(bootVfsSfileInit, int vfs_sfile_init(const char *path));
OSI_DECL_STRONG_ALIAS(bootVfsSfileWrite, int vfs_sfile_write(const char *path, const void *data, size_t size));
OSI_DECL_STRONG_ALIAS(bootVfsMkPath, int vfs_mkpath(const char *path, mode_t mode));
OSI_DECL_STRONG_ALIAS(bootVfsMkFilePath, int vfs_mkfilepath(const char *path, mode_t mode));
OSI_DECL_STRONG_ALIAS(bootVfsRmChildren, int vfs_rmchildren(const char *path));
OSI_DECL_STRONG_ALIAS(bootSffsMount, int sffsVfsMount(const char *base_path, blockDevice_t *bdev, size_t cache_count, size_t sfile_reserved_lb, bool read_only));
OSI_DECL_STRONG_ALIAS(bootSffsMkfs, int sffsVfsMkfs(blockDevice_t *bdev));
