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
#include "osi_log.h"
#include "osi_compiler.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// #define SKIP_VFS_SYSCALL_ALIAS

#define VFS_FD_POS (0)
#define VFS_FD_MASK (0x3ff)
#define VFS_INDEX_POS (10)
#define VFS_INDEX_MASK (0x7c00)
#define VFS_COUNT_MAX (31)

#define SET_ERRNO(err) OSI_DO_WHILE0(errno = err;)
#define ERR_RETURN(err, ret) OSI_DO_WHILE0(SET_ERRNO(err); return (ret);)
#define ERR_RETURN_V(err) OSI_DO_WHILE0(SET_ERRNO(err); return;)
#define CHECK_VFS(fs, err, ret) OSI_DO_WHILE0(if ((fs) == NULL) {SET_ERRNO(err); return (ret); })
#define CHECK_API(fs, api, err, ret) OSI_DO_WHILE0(if (fs->ops.api == NULL) { SET_ERRNO(err); return (ret); })

typedef struct vfs_entry_s
{
    vfs_ops_t ops;
    void *fs;
    char prefix[VFS_PREFIX_MAX]; // canonical path
    size_t prefix_len;
    int index; // index in g_vfs
} vfs_entry_t;

static vfs_entry_t *g_vfs[VFS_COUNT_MAX] = {};
static char g_vfs_curr_dir[VFS_PATH_MAX] = "/";
static vfs_stdio_callback_t g_stdio_cb;

// Register (mounted) file system to base_path
//
// @param base_path     mount point, must be absolute path
// @param ops           file system ops
// @param fs            file system private struct
// @returns     zero on success, -1 on error
int vfs_register(const char *base_path, const vfs_ops_t *ops, void *ctx)
{
    if (base_path == NULL || ops == NULL)
        ERR_RETURN(EINVAL, -1);

    size_t len = strlen(base_path);
    if (len == 0 || len >= VFS_PREFIX_MAX)
        ERR_RETURN(EINVAL, -1);

    if (base_path[0] != '/')
        ERR_RETURN(EINVAL, -1); // must be absolute path

    char real_path[VFS_PREFIX_MAX];
    vfs_resolve_path(base_path, ".", real_path); // make canonical path
    len = strlen(real_path);

    // avoid duplicated register
    for (int n = 0; n < VFS_COUNT_MAX; ++n)
    {
        vfs_entry_t *fs = g_vfs[n];
        if (fs != NULL && len == fs->prefix_len &&
            memcmp(real_path, fs->prefix, len) == 0)
            ERR_RETURN(EEXIST, -1);
    }

    int index = -1;
    for (int n = 0; n < VFS_COUNT_MAX; ++n)
    {
        if (g_vfs[n] == NULL)
        {
            index = n;
            break;
        }
    }
    if (index < 0)
        ERR_RETURN(ENOMEM, -1);

    g_vfs[index] = (vfs_entry_t *)malloc(sizeof(vfs_entry_t));
    if (g_vfs[index] == NULL)
        ERR_RETURN(ENOMEM, -1);

    g_vfs[index]->ops = *ops;
    g_vfs[index]->fs = ctx;
    memcpy(g_vfs[index]->prefix, real_path, len);
    g_vfs[index]->prefix[len] = '\0';
    g_vfs[index]->prefix_len = len;
    g_vfs[index]->index = index + 1; // avoid stdin/out/err conflict
    return 0;
}

// Unregister (umounted) file system by base_path
//
// @param base_path     mount point, must be absolute path
// @returns     zero on success, -1 on error
int vfs_unregister(const char *base_path)
{
    if (base_path == NULL)
        ERR_RETURN(EINVAL, -1);

    size_t len = strlen(base_path);
    if (len == 0 || len >= VFS_PREFIX_MAX)
        ERR_RETURN(EINVAL, -1);

    if (base_path[0] != '/')
        ERR_RETURN(EINVAL, -1); // must be absolute path

    char real_path[VFS_PREFIX_MAX];
    vfs_resolve_path(base_path, ".", real_path); // make canonical path
    len = strlen(real_path);

    for (int n = 0; n < VFS_COUNT_MAX; ++n)
    {
        vfs_entry_t *fs = g_vfs[n];
        if (fs != NULL && len == fs->prefix_len &&
            memcmp(real_path, fs->prefix, len) == 0)
        {
            free(fs);
            g_vfs[n] = NULL;
            return 0;
        }
    }
    ERR_RETURN(ENOENT, -1);
}

void *vfs_mount_handle(const char *path)
{
    if (path == NULL)
        return NULL;

    size_t len = strlen(path);
    if (len == 0 || len >= VFS_PREFIX_MAX)
        return NULL;

    for (int n = 0; n < VFS_COUNT_MAX; ++n)
    {
        vfs_entry_t *fs = g_vfs[n];
        if (fs != NULL && len == fs->prefix_len &&
            memcmp(path, fs->prefix, len) == 0)
            return fs->fs;
    }
    return NULL;
}

// Canonical path resolver, append path to real_path from
//
// @param real_path     pointer of real_path
// @param pos           current positon of real_path to be appended
// @param path          path to be appended. It should be already checked as non-absolute path
// @returns     zero on success, -1 on error
static int vfs_resolve_add_path(char *real_path, size_t pos, const char *path)
{
    for (;;)
    {
        if (*path == '\0')
            return 0;

        if (*path == '/')
        {
            path++;
            continue;
        }

        char *slash = strchr(path, '/');
        size_t len = (slash == NULL) ? strlen(path) : slash - path;
        if (len == 2 && path[0] == '.' && path[1] == '.')
        {
            char *reverse = strrchr(real_path, '/');
            if (reverse != NULL && reverse != real_path) // not root
            {
                *reverse = '\0';
                pos = reverse - real_path;
            }
        }
        else if (!(len == 1 && path[0] == '.'))
        {
            if (pos != 1)
            {
                if (pos + 1 >= VFS_PATH_MAX)
                    ERR_RETURN(ENAMETOOLONG, -1);
                real_path[pos++] = '/';
            }

            if (pos + len >= VFS_PATH_MAX)
                ERR_RETURN(ENAMETOOLONG, -1);

            memcpy(real_path + pos, path, len);
            pos += len;
            real_path[pos] = '\0';
        }

        if (slash == NULL)
            return 0;
        path = slash + 1; // skip '/' also
    }
    return 0; // never reach
}

// return the canonicalized absolute pathname
char *vfs_realpath(const char *path, char *resolved_path)
{
    if (path == NULL || resolved_path == NULL)
        ERR_RETURN(EINVAL, NULL);

    if (*path == '\0')
        ERR_RETURN(ENOENT, NULL);

    if (path[0] == '/')
    {
        resolved_path[0] = '/';
        resolved_path[1] = '\0';
        if (vfs_resolve_add_path(resolved_path, 1, path + 1) < 0)
            return NULL;
    }
    else
    {
        vfs_getcwd(resolved_path, VFS_PATH_MAX);
        size_t len = strlen(resolved_path);
        if (vfs_resolve_add_path(resolved_path, len, path) < 0)
            return NULL;
    }

    return resolved_path;
}

// return the canonical path of 'base_path'/'path'
//
// @param base_path     base_path to be resolved, can be wither absolute or relative path
//                      At NULL, it is equivalent to return canonical path of 'path'
// @param path          relative path under base_path. When it is started with '/', it will
//                      only be interpreted as absolute path when 'base_path' is NULL.
//                      Otherwise, it is still intepreted as relative path.
// @resolved_path       the canonical to be stored. The buffer must be large enough to hold
//                      path of VFS_PATH_MAX.
// @returns the canonical path on success, NULL on error.
char *vfs_resolve_path(const char *base_path, const char *path, char *resolved_path)
{
    if (base_path == NULL)
        return vfs_realpath(path, resolved_path);

    if (path == NULL || resolved_path == NULL)
        ERR_RETURN(EINVAL, NULL);

    if (*path == '\0')
        ERR_RETURN(ENOENT, NULL);

    if (vfs_realpath(base_path, resolved_path) == NULL)
        return NULL;

    size_t len = strlen(resolved_path);
    return vfs_resolve_add_path(resolved_path, len, path) < 0 ? NULL : resolved_path;
}

// get current working directory
char *vfs_getcwd(char *buf, size_t size)
{
    size_t len = strlen(g_vfs_curr_dir);
    if (size < len + 1)
        ERR_RETURN(ERANGE, NULL);

    memcpy(buf, g_vfs_curr_dir, len);
    buf[len] = '\0';
    return buf;
}

// change working directory
int vfs_chdir(const char *path)
{
    char cwd[VFS_PATH_MAX];
    char *real_path = vfs_realpath(path, cwd);
    if (real_path == NULL)
        return -1;

    strcpy(g_vfs_curr_dir, real_path);
    return 0;
}

// return fs by (vfs) fd
static const vfs_entry_t *vfs_get_fs_by_fd(int fd)
{
    int index = (fd & VFS_INDEX_MASK) >> VFS_INDEX_POS;
    if (index <= 0 || index > VFS_COUNT_MAX)
        return NULL;
    return g_vfs[index - 1];
}

// return (fs) fd by (vfs) fd
static int vfs_get_local_fd(int fd)
{
    return (fd & VFS_FD_MASK) >> VFS_FD_POS;
}

// return fs and path inside fs by canonical path
static const vfs_entry_t *vfs_get_fs_by_path(const char *path, const char **local_path)
{
    const vfs_entry_t *best_match = NULL;
    size_t best_match_len = 0;
    size_t len = strlen(path);

    for (size_t n = 0; n < VFS_COUNT_MAX; ++n)
    {
        const vfs_entry_t *vfs = g_vfs[n];
        if (vfs == NULL)
            continue;
        if (len < vfs->prefix_len)
            continue;
        if (memcmp(path, vfs->prefix, vfs->prefix_len) != 0)
            continue;
        if (vfs->prefix_len > 1 && // only root ('/') prefixlen is 1
            len > vfs->prefix_len && path[vfs->prefix_len] != '/')
            continue;

        if (best_match_len < vfs->prefix_len)
        {
            best_match_len = vfs->prefix_len;
            best_match = vfs;
            if (vfs->ops.flags & VFS_NEED_REAL_PATH)
            {
                *local_path = path;
            }
            else
            {
                *local_path = path + vfs->prefix_len;
                if (**local_path == '/')
                    *local_path += 1;
            }
        }
    }
    return best_match;
}

// unmount filesystem
int vfs_umount(const char *path)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    if (fs == NULL || local_path == NULL || (fs->ops.flags == 0 && local_path[0] != '\0'))
        ERR_RETURN(ENOENT, -1);

    CHECK_API(fs, umount, ENOSYS, -1);
    if (fs->ops.umount(fs->fs) < 0)
        return -1;

    vfs_unregister(real_path);
    return 0;
}

// remount filesystem
int vfs_remount(const char *path, unsigned flags)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    if (fs == NULL || local_path == NULL || (fs->ops.flags == 0 && local_path[0] != '\0'))
        ERR_RETURN(ENOENT, -1);

    CHECK_API(fs, remount, ENOSYS, -1);
    return fs->ops.remount(fs->fs, flags);
}

static int vfs_open_local_path(const vfs_entry_t *fs, const char *local_path, int flags, int mode)
{
    CHECK_API(fs, open, ENOSYS, -1);

    int ret = fs->ops.open(fs->fs, local_path, flags, mode);
    if (ret < 0)
        return ret;
    return ret + (fs->index << VFS_INDEX_POS);
}

void *vfs_fs_handle(const char *path)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return NULL;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    if (fs == NULL)
        return NULL;

    return fs->fs;
}

// open and possibly create a file
int vfs_open(const char *path, int flags, ...)
{
    int mode = 0;
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);
    return vfs_open_local_path(fs, local_path, flags, mode);
}

// open and possibly create a file
int vfs_creat(const char *path, mode_t mode)
{
    return vfs_open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

// close a file descriptor
int vfs_close(int fd)
{
    const vfs_entry_t *fs = vfs_get_fs_by_fd(fd);
    CHECK_VFS(fs, EBADF, -1);
    CHECK_API(fs, close, ENOSYS, -1);

    int local_fd = vfs_get_local_fd(fd);
    return fs->ops.close(fs->fs, local_fd);
}

// read from a file descriptor
ssize_t vfs_read(int fd, void *dst, size_t size)
{
    if (fd == 0)
        return (g_stdio_cb.stdin_read == NULL) ? -1 : g_stdio_cb.stdin_read(dst, size);

    const vfs_entry_t *fs = vfs_get_fs_by_fd(fd);
    CHECK_VFS(fs, EBADF, -1);
    CHECK_API(fs, read, ENOSYS, -1);

    int local_fd = vfs_get_local_fd(fd);
    return fs->ops.read(fs->fs, local_fd, dst, size);
}

// write to a file descriptor
ssize_t vfs_write(int fd, const void *data, size_t size)
{
    if (fd == 1)
        return (g_stdio_cb.stdout_write == NULL) ? -1 : g_stdio_cb.stdout_write(data, size);
    if (fd == 2)
        return (g_stdio_cb.stderr_write == NULL) ? -1 : g_stdio_cb.stderr_write(data, size);

    const vfs_entry_t *fs = vfs_get_fs_by_fd(fd);
    CHECK_VFS(fs, EBADF, -1);
    CHECK_API(fs, write, ENOSYS, -1);

    int local_fd = vfs_get_local_fd(fd);
    return fs->ops.write(fs->fs, local_fd, data, size);
}

// reposition read/write file offset
off_t vfs_lseek(int fd, off_t offset, int mode)
{
    const vfs_entry_t *fs = vfs_get_fs_by_fd(fd);
    CHECK_VFS(fs, EBADF, -1);
    CHECK_API(fs, lseek, ENOSYS, -1);

    int local_fd = vfs_get_local_fd(fd);
    return fs->ops.lseek(fs->fs, local_fd, offset, mode);
}

// get file status
int vfs_fstat(int fd, struct stat *st)
{
    const vfs_entry_t *fs = vfs_get_fs_by_fd(fd);
    CHECK_VFS(fs, EBADF, -1);
    CHECK_API(fs, fstat, ENOSYS, -1);

    int local_fd = vfs_get_local_fd(fd);
    return fs->ops.fstat(fs->fs, local_fd, st);
}

// get file status
int vfs_stat(const char *path, struct stat *st)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);
    CHECK_API(fs, stat, ENOSYS, -1);
    return fs->ops.stat(fs->fs, local_path, st);
}

//truncate a file to a specified length
int vfs_truncate(const char *path, long length)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);
    CHECK_API(fs, truncate, ENOSYS, -1);
    return fs->ops.truncate(fs->fs, local_path, length);
}

//truncate a file to a specified length
int vfs_ftruncate(int fd, long length)
{
    const vfs_entry_t *fs = vfs_get_fs_by_fd(fd);
    CHECK_VFS(fs, EBADF, -1);
    CHECK_API(fs, ftruncate, ENOSYS, -1);

    int local_fd = vfs_get_local_fd(fd);
    return fs->ops.ftruncate(fs->fs, local_fd, length);
}

// make a new name for a file
int vfs_link(const char *oldpath, const char *newpath)
{
    ERR_RETURN(ENOSYS, -1);
}

// delete a name and possibly the file it refers to
int vfs_unlink(const char *path)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);
    CHECK_API(fs, unlink, ENOSYS, -1);
    return fs->ops.unlink(fs->fs, local_path);
}

// change the name of a file
int vfs_rename(const char *src, const char *dst)
{
    char real_src[VFS_PATH_MAX];
    if (vfs_realpath(src, real_src) == NULL)
        return -1;

    char real_dst[VFS_PATH_MAX];
    if (vfs_realpath(dst, real_dst) == NULL)
        return -1;

    const char *local_src = NULL;
    const vfs_entry_t *fs_src = vfs_get_fs_by_path(real_src, &local_src);
    CHECK_VFS(fs_src, ENOENT, -1);

    const char *local_dst = NULL;
    const vfs_entry_t *fs_dst = vfs_get_fs_by_path(real_dst, &local_dst);

    if (fs_src != fs_dst)
        ERR_RETURN(EXDEV, -1);

    CHECK_API(fs_src, rename, ENOSYS, -1);
    return fs_src->ops.rename(fs_src->fs, local_src, local_dst);
}

// open a directory
DIR *vfs_opendir(const char *name)
{
    char real_name[VFS_PATH_MAX];
    if (vfs_realpath(name, real_name) == NULL)
        return NULL;

    const char *local_name = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_name, &local_name);
    CHECK_VFS(fs, ENOENT, NULL);
    CHECK_API(fs, opendir, ENOSYS, NULL);
    DIR *dir = fs->ops.opendir(fs->fs, local_name);
    if (dir != NULL)
        dir->fs_index = fs->index << VFS_INDEX_POS;
    return dir;
}

// read a directory
struct dirent *vfs_readdir(DIR *pdir)
{
    if (pdir == NULL)
        ERR_RETURN(EINVAL, NULL);

    const vfs_entry_t *fs = vfs_get_fs_by_fd(pdir->fs_index);
    CHECK_VFS(fs, EINVAL, NULL);
    CHECK_API(fs, readdir, ENOSYS, NULL);
    return fs->ops.readdir(fs->fs, pdir);
}

// read a directory
int vfs_readdir_r(DIR *pdir, struct dirent *entry, struct dirent **out_dirent)
{
    if (pdir == NULL)
        ERR_RETURN(EINVAL, -1);

    const vfs_entry_t *fs = vfs_get_fs_by_fd(pdir->fs_index);
    CHECK_VFS(fs, EINVAL, -1);
    CHECK_API(fs, readdir_r, ENOSYS, -1);
    return fs->ops.readdir_r(fs->fs, pdir, entry, out_dirent);
}

// return current location in directory stream
long vfs_telldir(DIR *pdir)
{
    if (pdir == NULL)
        ERR_RETURN(EINVAL, -1);

    const vfs_entry_t *fs = vfs_get_fs_by_fd(pdir->fs_index);
    CHECK_VFS(fs, EINVAL, -1);
    CHECK_API(fs, telldir, ENOSYS, -1);
    return fs->ops.telldir(fs->fs, pdir);
}

// set the position of the next readdir() call in the directory stream
void vfs_seekdir(DIR *pdir, long loc)
{
    if (pdir == NULL)
        ERR_RETURN_V(EINVAL);

    const vfs_entry_t *fs = vfs_get_fs_by_fd(pdir->fs_index);
    if (fs == NULL)
        ERR_RETURN_V(EINVAL);
    if (fs->ops.seekdir == NULL)
        ERR_RETURN_V(ENOSYS);
    fs->ops.seekdir(fs->fs, pdir, loc);
}

// reset directory stream
void vfs_rewinddir(DIR *pdir)
{
    vfs_seekdir(pdir, 0);
}

// close a directory
int vfs_closedir(DIR *pdir)
{
    if (pdir == NULL)
        ERR_RETURN(EINVAL, -1);

    const vfs_entry_t *fs = vfs_get_fs_by_fd(pdir->fs_index);
    CHECK_VFS(fs, EINVAL, -1);
    CHECK_API(fs, closedir, ENOSYS, -1);
    return fs->ops.closedir(fs->fs, pdir);
}

// create a directory
int vfs_mkdir(const char *name, mode_t mode)
{
    char real_name[VFS_PATH_MAX];
    if (vfs_realpath(name, real_name) == NULL)
        return -1;

    const char *local_name = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_name, &local_name);
    CHECK_VFS(fs, ENOENT, -1);
    CHECK_API(fs, mkdir, ENOSYS, -1);
    return fs->ops.mkdir(fs->fs, local_name, mode);
}

// delete a directory
int vfs_rmdir(const char *name)
{
    char real_name[VFS_PATH_MAX];
    if (vfs_realpath(name, real_name) == NULL)
        return -1;

    const char *local_name = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_name, &local_name);
    CHECK_VFS(fs, ENOENT, -1);
    CHECK_API(fs, rmdir, ENOSYS, -1);
    return fs->ops.rmdir(fs->fs, local_name);
}

// synchronize a file's in-core state with storage device
int vfs_fsync(int fd)
{
    const vfs_entry_t *fs = vfs_get_fs_by_fd(fd);
    CHECK_VFS(fs, EBADF, -1);
    CHECK_API(fs, fsync, ENOSYS, -1);

    int local_fd = vfs_get_local_fd(fd);
    return fs->ops.fsync(fs->fs, local_fd);
}

int vfs_fcntl(int fd, int cmd, ...)
{
    ERR_RETURN(ENOSYS, -1);
}

int vfs_ioctl(int fd, unsigned long request, ...)
{
    ERR_RETURN(ENOSYS, -1);
}

// get filesystem statistics
int vfs_statvfs(const char *path, struct statvfs *buf)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);
    CHECK_API(fs, statvfs, ENOSYS, -1);
    return fs->ops.statvfs(fs->fs, buf);
}

// get filesystem statistics
int vfs_fstatvfs(int fd, struct statvfs *buf)
{
    const vfs_entry_t *fs = vfs_get_fs_by_fd(fd);
    CHECK_VFS(fs, EBADF, -1);
    CHECK_API(fs, statvfs, ENOSYS, -1);
    return fs->ops.statvfs(fs->fs, buf);
}

int vfs_sfile_init(const char *path)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);
    if (fs->ops.sfile_init == NULL)
    {
        int fd = vfs_open_local_path(fs, local_path, O_RDONLY, 0);
        if (fd < 0)
            return -1;

        struct stat st;
        int res = vfs_fstat(fd, &st);
        vfs_close(fd);
        return (res < 0) ? -1 : 0;
    }

    return fs->ops.sfile_init(fs->fs, local_path);
}

ssize_t vfs_sfile_read(const char *path, void *dst, size_t size)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);
    if (fs->ops.sfile_read == NULL)
    {
        int fd = vfs_open_local_path(fs, local_path, O_RDONLY, 0);
        if (fd < 0)
            return -1;
        ssize_t read_len = vfs_read(fd, dst, size);
        vfs_close(fd);
        return read_len;
    }

    return fs->ops.sfile_read(fs->fs, local_path, dst, size);
}

ssize_t vfs_sfile_write(const char *path, const void *data, size_t size)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);
    if (fs->ops.sfile_write == NULL)
    {
        int fd = vfs_open_local_path(fs, local_path, O_RDWR | O_CREAT | O_TRUNC, 0);
        if (fd < 0)
            return -1;

        ssize_t write_len = vfs_write(fd, data, size);
        vfs_close(fd);
        return write_len;
    }

    return fs->ops.sfile_write(fs->fs, local_path, data, size);
}

ssize_t vfs_sfile_size(const char *path)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);
    if (fs->ops.sfile_size == NULL)
    {
        int fd = vfs_open_local_path(fs, local_path, O_RDONLY, 0);
        if (fd < 0)
            return -1;

        struct stat st;
        int res = vfs_fstat(fd, &st);
        vfs_close(fd);
        return (res < 0) ? -1 : (ssize_t)st.st_size;
    }

    return fs->ops.sfile_size(fs->fs, local_path);
}

ssize_t vfs_file_read(const char *path, void *dst, size_t size)
{
    int fd = vfs_open(path, O_RDONLY);
    if (fd < 0)
        return -1;

    ssize_t read_len = vfs_read(fd, dst, size);
    vfs_close(fd);
    return read_len;
}

ssize_t vfs_file_write(const char *path, const void *data, size_t size)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        return -1;

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);
    if (fs->ops.file_write == NULL)
    {
        int fd = vfs_open_local_path(fs, local_path, O_RDWR | O_CREAT | O_TRUNC, 0);
        if (fd < 0)
            return -1;

        ssize_t write_len = vfs_write(fd, data, size);
        vfs_close(fd);
        return write_len;
    }

    return fs->ops.file_write(fs->fs, local_path, data, size);
}

ssize_t vfs_file_size(const char *path)
{
    struct stat st;
    if (vfs_stat(path, &st) < 0)
        return -1;
    return (ssize_t)st.st_size;
}

int vfs_mkpath(const char *path, mode_t mode)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        ERR_RETURN(ENOENT, -1);

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);

    char *sep = (char *)local_path;
    for (;;)
    {
        sep = strchr(sep, '/');
        if (sep != NULL)
            *sep = '\0';

        int res = vfs_mkdir(real_path, mode);
        if (res < 0 && errno != EEXIST)
            return -1;

        if (sep == NULL)
            break;

        *sep = '/';
        sep++;
    }

    return 0;
}

int vfs_mkfilepath(const char *path, mode_t mode)
{
    char real_path[VFS_PATH_MAX];
    if (vfs_realpath(path, real_path) == NULL)
        ERR_RETURN(ENOENT, -1);

    const char *local_path = NULL;
    const vfs_entry_t *fs = vfs_get_fs_by_path(real_path, &local_path);
    CHECK_VFS(fs, ENOENT, -1);

    char *sep = (char *)local_path;
    char *last = strrchr(local_path, '/');
    if (last == NULL) // no parent directory
        return 0;

    *last = '\0';
    for (;;)
    {
        sep = strchr(sep, '/');
        if (sep != NULL)
            *sep = '\0';

        int res = vfs_mkdir(real_path, mode);
        if (res < 0 && errno != EEXIST)
            return -1;

        if (sep == NULL)
            return 0;

        *sep = '/';
        sep++;
    }

    return 0; // never reach
}

int vfs_rmchildren(const char *path)
{
    DIR *dir = vfs_opendir(path);
    if (dir == NULL)
        return -1;

    struct dirent *ent = (struct dirent *)calloc(1, sizeof(struct dirent));
    char *child = (char *)malloc(VFS_PATH_MAX + 256);
    if (ent == NULL || child == NULL)
    {
        vfs_closedir(dir);
        free(ent);
        free(child);
        ERR_RETURN(ENOMEM, -1);
    }

    struct dirent *rent = NULL;
    int res = 0;
    while (vfs_readdir_r(dir, ent, &rent) >= 0 && rent != NULL)
    {
        if (rent->d_type == DT_REG)
        {
            snprintf(child, VFS_PATH_MAX + 256 - 1, "%s/%s", path, rent->d_name);
            if (vfs_unlink(child) < 0)
            {
                res = -1;
                break;
            }
        }
        else if (rent->d_type == DT_DIR)
        {
            snprintf(child, VFS_PATH_MAX + 256 - 1, "%s/%s", path, rent->d_name);
            if (vfs_rmchildren(child) < 0 || vfs_rmdir(child) < 0)
            {
                res = -1;
                break;
            }
        }
    }

    vfs_closedir(dir);
    free(ent);
    free(child);
    return res;
}

int vfs_rmdir_recursive(const char *path)
{
    int res = vfs_rmchildren(path);
    if (res < 0)
        return res;

    return vfs_rmdir(path);
}

int64_t vfs_dir_total_size(const char *path)
{
    DIR *dir = vfs_opendir(path);
    if (dir == NULL)
        return -1;

    struct dirent *ent = (struct dirent *)calloc(1, sizeof(struct dirent));
    char *child = (char *)malloc(VFS_PATH_MAX + 256);
    if (ent == NULL || child == NULL)
    {
        vfs_closedir(dir);
        free(ent);
        free(child);
        ERR_RETURN(ENOMEM, -1);
    }

    struct stat st;
    struct dirent *rent = NULL;
    int64_t total = 0;
    while (vfs_readdir_r(dir, ent, &rent) >= 0 && rent != NULL)
    {
        if (rent->d_type == DT_REG)
        {
            snprintf(child, VFS_PATH_MAX + 256 - 1, "%s/%s", path, rent->d_name);
            if (vfs_stat(child, &st) != 0)
                continue;

            total += st.st_size;
        }
        else if (rent->d_type == DT_DIR)
        {
            snprintf(child, VFS_PATH_MAX + 256 - 1, "%s/%s", path, rent->d_name);
            int64_t sub_total = vfs_dir_total_size(child);
            if (sub_total < 0)
                continue;

            total += sub_total;
        }
    }

    vfs_closedir(dir);
    free(ent);
    free(child);
    return total;
}

int vfs_mount_count(void)
{
    int count = 0;
    for (int n = 0; n < VFS_COUNT_MAX; ++n)
    {
        if (g_vfs[n] != NULL)
            count++;
    }
    return count;
}

int vfs_mount_points(char **mp, size_t count)
{
    int idx = 0;
    for (int n = 0; n < VFS_COUNT_MAX; ++n)
    {
        if (g_vfs[n] != NULL)
        {
            if (idx >= count)
                return idx + 1;

            mp[idx++] = g_vfs[n]->prefix;
        }
    }
    return idx + 1;
}

void vfs_umount_all(void)
{
    // Why it is called?
}

void vfs_set_stdio_callback(const vfs_stdio_callback_t *cb)
{
    if (cb != NULL)
        g_stdio_cb = *cb;
}

#ifndef SKIP_VFS_SYSCALL_ALIAS
OSI_DECL_STRONG_ALIAS(vfs_umount, int umount(const char *path));
OSI_DECL_STRONG_ALIAS(vfs_creat, int creat(const char *path, mode_t mode));
OSI_DECL_STRONG_ALIAS(vfs_open, int open(const char *path, int flags, ...));
OSI_DECL_STRONG_ALIAS(vfs_close, int close(int fd));
OSI_DECL_STRONG_ALIAS(vfs_read, ssize_t read(int fd, void *buf, size_t count));
OSI_DECL_STRONG_ALIAS(vfs_write, ssize_t write(int fd, const void *buf, size_t count));
OSI_DECL_STRONG_ALIAS(vfs_lseek, long lseek(int fd, long offset, int whence));
OSI_DECL_STRONG_ALIAS(vfs_fstat, int fstat(int fd, struct stat *st));
OSI_DECL_STRONG_ALIAS(vfs_stat, int stat(const char *path, struct stat *st));
OSI_DECL_STRONG_ALIAS(vfs_truncate, int truncate(const char *path, long length));
OSI_DECL_STRONG_ALIAS(vfs_ftruncate, int ftruncate(int fd, long length));
OSI_DECL_STRONG_ALIAS(vfs_link, int link(const char *from, const char *to));
OSI_DECL_STRONG_ALIAS(vfs_unlink, int unlink(const char *path));
OSI_DECL_STRONG_ALIAS(vfs_rename, int rename(const char *oldpath, const char *newpath));
OSI_DECL_STRONG_ALIAS(vfs_fsync, int fsync(int fd));
OSI_DECL_STRONG_ALIAS(vfs_fcntl, int fcntl(int fd, int cmd, ...));
OSI_DECL_STRONG_ALIAS(vfs_ioctl, int ioctl(int fd, unsigned long request, ...));
OSI_DECL_STRONG_ALIAS(vfs_opendir, DIR *opendir(const char *path));
OSI_DECL_STRONG_ALIAS(vfs_readdir, struct dirent *readdir(DIR *dirp));
OSI_DECL_STRONG_ALIAS(vfs_readdir_r, int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result));
OSI_DECL_STRONG_ALIAS(vfs_telldir, int telldir(DIR *dirp));
OSI_DECL_STRONG_ALIAS(vfs_seekdir, void seekdir(DIR *dirp, long loc));
OSI_DECL_STRONG_ALIAS(vfs_rewinddir, void rewinddir(DIR *dirp));
OSI_DECL_STRONG_ALIAS(vfs_closedir, int closedir(DIR *dirp));
OSI_DECL_STRONG_ALIAS(vfs_mkdir, int mkdir(const char *path, mode_t mode));
OSI_DECL_STRONG_ALIAS(vfs_rmdir, int rmdir(const char *path));
OSI_DECL_STRONG_ALIAS(vfs_chdir, int chdir(const char *path));
OSI_DECL_STRONG_ALIAS(vfs_getcwd, char *getcwd(char *buf, size_t size));
OSI_DECL_STRONG_ALIAS(vfs_realpath, char *realpath(const char *buf, char *resolved_path));
OSI_DECL_STRONG_ALIAS(vfs_statvfs, int statvfs(const char *path, struct statvfs *buf));
OSI_DECL_STRONG_ALIAS(vfs_fstatvfs, int fstatvfs(int fd, struct statvfs *buf));
#endif
