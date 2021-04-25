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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE

#include <stdlib.h>
#include <string.h>
#include "osi_api.h"
#include "osi_log.h"
#include "vfs.h"
#include "vfs_ops.h"
#include "sys/errno.h"
#include "block_device.h"
#include "sffs.h"
#include "sffs_vfs.h"

#define CHECK_RESULT(res) OSI_DO_WHILE0(if ((res) < 0) {errno = -(res); return -1; })
#define CHECK_RESULT_RETURN(res) OSI_DO_WHILE0(if ((res) < 0) {errno = -(res); return -1; } else return (res);)

static int _open(void *ctx, const char *path, int flags, int mode)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int fd = sffsOpen(fs, path, flags);
    if (fd < 0)
        OSI_LOGD(0, "SFFS open error: %d", fd);
    CHECK_RESULT_RETURN(fd);
}

static int _close(void *ctx, int fd)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsClose(fs, fd);
    if (res < 0)
        OSI_LOGD(0, "SFFS close error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static ssize_t _read(void *ctx, int fd, void *data, size_t size)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    ssize_t res = sffsRead(fs, fd, data, size);
    if (res < 0)
        OSI_LOGD(0, "SFFS read error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static ssize_t _write(void *ctx, int fd, const void *data, size_t size)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    ssize_t res = sffsWrite(fs, fd, data, size);
    if (res < 0)
        OSI_LOGD(0, "SFFS write error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static off_t _lseek(void *ctx, int fd, off_t offset, int mode)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    off_t res = sffsSeek(fs, fd, offset, mode);
    if (res < 0)
        OSI_LOGD(0, "SFFS seek error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static int _fstat(void *ctx, int fd, struct stat *st)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsFstat(fs, fd, st);
    if (res < 0)
        OSI_LOGD(0, "SFFS fstat error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static int _stat(void *ctx, const char *path, struct stat *st)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int fd = sffsOpen(fs, path, O_RDONLY);
    CHECK_RESULT(fd);
    int res = sffsFstat(fs, fd, st);
    sffsClose(fs, fd);
    CHECK_RESULT_RETURN(res);
}

static int _fsync(void *ctx, int fd)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsSync(fs);
    if (res < 0)
        OSI_LOGD(0, "SFFS fsync error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static int _ftruncate(void *ctx, int fd, off_t length)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsFtruncate(fs, fd, length);
    if (res < 0)
        OSI_LOGD(0, "SFFS ftruncate error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static int _truncate(void *ctx, const char *path, long length)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int fd = sffsOpen(fs, path, O_RDWR);
    CHECK_RESULT(fd);
    int res = sffsFtruncate(fs, fd, length);
    sffsClose(fs, fd);
    CHECK_RESULT_RETURN(res);
}

static int _unlink(void *ctx, const char *path)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsUnlink(fs, path);
    if (res < 0)
        OSI_LOGD(0, "SFFS unlink error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static int _rename(void *ctx, const char *src, const char *dst)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsRename(fs, src, dst);
    if (res < 0)
        OSI_LOGD(0, "SFFS rename error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static DIR *_opendir(void *ctx, const char *path)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    DIR *res = sffsOpenDir(fs, path);
    return res;
}

static struct dirent *_readdir(void *ctx, DIR *dir)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    return sffsReadDir(fs, dir);
}

static int _readdir_r(void *ctx, DIR *dir, struct dirent *entry, struct dirent **result)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsReadDirR(fs, dir, entry, result);
    CHECK_RESULT_RETURN(res);
}

static long _telldir(void *ctx, DIR *dir)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    long res = sffsTellDir(fs, dir);
    CHECK_RESULT_RETURN(res);
}

static void _seekdir(void *ctx, DIR *dir, long loc)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    sffsSeekDir(fs, dir, loc);
}

static int _closedir(void *ctx, DIR *dir)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsCloseDir(fs, dir);
    CHECK_RESULT_RETURN(res);
}

static int _mkdir(void *ctx, const char *name, int mode)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsMkDir(fs, name);
    if (res < 0)
        OSI_LOGD(0, "SFFS mkdir error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static int _rmdir(void *ctx, const char *name)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsRmDir(fs, name);
    if (res < 0)
        OSI_LOGD(0, "SFFS rmdir error: %d", res);
    CHECK_RESULT_RETURN(res);
}

static int _statvfs(void *ctx, struct statvfs *buf)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsStatVfs(fs, buf);
    CHECK_RESULT_RETURN(res);
}

static int _umount(void *ctx)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsUnmount(fs);
    CHECK_RESULT_RETURN(res);
}

static int _remount(void *ctx, unsigned flags)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsRemount(fs, flags);
    CHECK_RESULT_RETURN(res);
}

static ssize_t _sfile_write(void *ctx, const char *path, const void *data, size_t size)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsSfileWrite(fs, path, data, size);
    CHECK_RESULT_RETURN(res);
}

static ssize_t _file_write(void *ctx, const char *path, const void *data, size_t size)
{
    sffsFs_t *fs = (sffsFs_t *)ctx;
    int res = sffsFileWrite(fs, path, data, size);
    CHECK_RESULT_RETURN(res);
}

static const vfs_ops_t g_sffs_vfs_ops = {
    .flags = 0,
    .umount = _umount,
    .remount = _remount,
    .open = _open,
    .close = _close,
    .read = _read,
    .write = _write,
    .lseek = _lseek,
    .fstat = _fstat,
    .stat = _stat,
    .truncate = _truncate,
    .ftruncate = _ftruncate,
    .unlink = _unlink,
    .rename = _rename,
    .opendir = _opendir,
    .readdir = _readdir,
    .readdir_r = _readdir_r,
    .telldir = _telldir,
    .seekdir = _seekdir,
    .closedir = _closedir,
    .mkdir = _mkdir,
    .rmdir = _rmdir,
    .fsync = _fsync,
    .statvfs = _statvfs,
    .sfile_init = NULL,
    .sfile_write = _sfile_write,
    .sfile_read = NULL,
    .sfile_size = NULL,
    .file_write = _file_write,
};

int sffsVfsMount(const char *base_path, blockDevice_t *bdev,
                 size_t cache_count, size_t sfile_reserved_lb,
                 bool read_only)
{
    sffsFs_t *fs = sffsMount(bdev, cache_count, read_only);
    if (fs == NULL)
        return -1;

    if (vfs_register(base_path, &g_sffs_vfs_ops, fs) < 0)
    {
        sffsUnmount(fs);
        return -1;
    }

    if (sfile_reserved_lb != 0)
        sffsSetSfileReserveCount(fs, sfile_reserved_lb);

    return 0;
}

int sffsVfsMkfs(blockDevice_t *bdev)
{
    int res = sffsMakeFs(bdev);
    CHECK_RESULT_RETURN(res);
}
