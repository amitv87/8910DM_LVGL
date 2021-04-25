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

#ifndef _VFS_OPS_H_
#define _VFS_OPS_H_

#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VFS_PREFIX_MAX (15)

#define VFS_NEED_REAL_PATH (0x0001)
typedef struct
{
    int flags;
    int (*umount)(void *fs);
    int (*remount)(void *fs, unsigned flags);
    int (*open)(void *fs, const char *path, int flags, int /*mode_t*/ mode);
    int (*close)(void *fs, int fd);
    ssize_t (*read)(void *fs, int fd, void *dst, size_t size);
    ssize_t (*write)(void *fs, int fd, const void *data, size_t size);
    long /*off_t*/ (*lseek)(void *fs, int fd, long /*off_t*/ offset, int mode);
    int (*fstat)(void *fs, int fd, struct stat *st);
    int (*stat)(void *fs, const char *path, struct stat *st);
    int (*truncate)(void *fs, const char *path, long length);
    int (*ftruncate)(void *fs, int fd, long length);
    int (*unlink)(void *fs, const char *path);
    int (*rename)(void *fs, const char *src, const char *dst);

    DIR *(*opendir)(void *fs, const char *name);
    struct dirent *(*readdir)(void *fs, DIR *pdir);
    int (*readdir_r)(void *fs, DIR *pdir, struct dirent *entry, struct dirent **out_dirent);
    long (*telldir)(void *fs, DIR *pdir);
    void (*seekdir)(void *fs, DIR *pdir, long loc);
    int (*closedir)(void *fs, DIR *pdir);
    int (*mkdir)(void *fs, const char *name, int /*mode_t*/ mode);
    int (*rmdir)(void *fs, const char *name);
    int (*fsync)(void *fs, int fd);

    int (*statvfs)(void *fs, struct statvfs *buf);

    int (*sfile_init)(void *fs, const char *path);
    ssize_t (*sfile_read)(void *fs, const char *path, void *dst, size_t size);
    ssize_t (*sfile_write)(void *fs, const char *path, const void *data, size_t size);
    ssize_t (*sfile_size)(void *fs, const char *path);
    ssize_t (*file_write)(void *fs, const char *path, const void *data, size_t size);
} vfs_ops_t;

int vfs_register(const char *base_path, const vfs_ops_t *ops, void *fs);
int vfs_unregister(const char *base_path);

#ifdef __cplusplus
}
#endif

#endif // H
