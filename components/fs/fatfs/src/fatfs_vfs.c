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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "vfs.h"
#include "vfs_ops.h"

#define DIR FDIR
#include "ff.h"
#undef DIR

#include "fatfs_vfs.h"
#include "fatfs_portable.h"
#include "block_device.h"
#include "osi_api.h"
#include "osi_log.h"
#include "stdio.h"
#include "string.h"
#include <sys/errno.h>
#include <sys/queue.h>
#include <stdlib.h>

// set errno
#define SET_ERRNO(err) OSI_DO_WHILE0(errno = (err);)
// set errno and return
#define ERR_RETURN(err, failed) OSI_DO_WHILE0(SET_ERRNO(err); return (failed);)
// when res is negative, set errno and return -1
#define CHECK_RESULT(res) OSI_DO_WHILE0(if ((res) < 0) {errno = -(res); return -1; })
// when res is negative, set errno and return -1; otherwise return 0
#define CHECK_RESULT_RETURN(res) OSI_DO_WHILE0(if ((res) < 0) {errno = -(res); return -1; } else return (res);)
// when fr is not FR_OK, set errno and return -1
#define CHECK_FRESULT(fr) OSI_DO_WHILE0(if ((fr) != FR_OK) {errno = prvFatResToErrno(fr); return -1; })
// when fr is not FR_OK, set errno and return -1; othersize return 0
#define CHECK_FRESULT_RETURN(fr) OSI_DO_WHILE0(if ((fr) != FR_OK) {errno = prvFatResToErrno(fr); return -1; } else return 0;)
#define FD_MIN (3)

typedef SLIST_ENTRY(fatfsFile) fatfsFileIter_t;
typedef SLIST_HEAD(, fatfsFile) fatfsFileHead_t;
typedef struct fatfsFile
{
    fatfsFileIter_t iter;
    FIL fil;
    uint16_t fd;
} fatfsFile_t;

typedef struct
{
    DIR v_dir;
    FDIR fat_dir;
    struct dirent e;
} fatfsDir_t;

typedef struct
{
    FATFS fs;
    char fat_path1[VFS_PATH_MAX + 3];
    char fat_path2[VFS_PATH_MAX + 3];
    BYTE fs_dev_no;
    bool read_only;
    fatfsFileHead_t fils;
    osiMutex_t *lock;
} fatfsContext_t;

/**
 * Convert FRESULT to errno
 */
static int prvFatResToErrno(FRESULT fr)
{
    switch (fr)
    {
    case FR_OK:
        return 0;
    case FR_DISK_ERR:
    case FR_INVALID_DRIVE:
        return ENODEV;
    case FR_INT_ERR:
        return EINTR;
    case FR_NOT_READY:
        return EBUSY;
    case FR_NO_FILE:
    case FR_NO_PATH:
    case FR_INVALID_NAME:
        return ENOENT;
    case FR_TOO_MANY_OPEN_FILES:
        return ENFILE;
    case FR_NO_FILESYSTEM:
    case FR_INVALID_PARAMETER:
        return EINVAL;
    case FR_DENIED:
        return EACCES;
    case FR_EXIST:
        return EEXIST;
    case FR_WRITE_PROTECTED:
        return EROFS;
    case FR_NOT_ENOUGH_CORE:
    case FR_NOT_ENABLED:
        return ENOMEM;
    case FR_LOCKED:
        return EDEADLK;
    case FR_TIMEOUT:
        return ETIME;
    case FR_MKFS_ABORTED:
    case FR_INVALID_OBJECT:
        return EPERM;
    default:
        return EIO;
    }
}

/**
 * Convert posix mode to fatfs mode
 */
static int prvFatModeConv(int mode)
{
    int res = 0;
    int acc_mode = mode & O_ACCMODE;

    if (acc_mode == O_RDONLY)
        res |= FA_READ;
    else if (acc_mode == O_WRONLY)
        res |= FA_WRITE;
    else if (acc_mode == O_RDWR)
        res |= FA_READ | FA_WRITE;

    if (mode & O_TRUNC)
        res |= FA_CREATE_ALWAYS;
    else if (mode & O_APPEND)
        res |= FA_OPEN_APPEND;
    else if (mode & O_EXCL)
        res |= FA_CREATE_NEW;
    else if (mode & O_CREAT)
        res |= FA_OPEN_ALWAYS;

    return res;
}

/**
 * Convert name to fatfs name, start with "[0-9]:"
 */
static void prvFatNameWithId(fatfsContext_t *fs, char *dst, const char *name)
{
    int len = strlen(name);
    dst[0] = '0' + fs->fs_dev_no;
    dst[1] = ':';
    memcpy(&dst[2], name, len);
    dst[2 + len] = '\0';
}

/**
 * Get fatfsFile_t from fd. Opened files are in a list of fatfs context.
 */
static fatfsFile_t *prvFatGetFileFromFd(fatfsContext_t *fs, uint16_t fd)
{
    fatfsFile_t *fil;
    SLIST_FOREACH(fil, &fs->fils, iter)
    {
        if (fil->fd == fd)
            return fil;
    }
    return NULL;
}

/**
 * Find unused fd.
 */
static uint16_t prvFatFindNewFd(fatfsContext_t *fs)
{
    for (uint16_t fd = FD_MIN;; fd++)
    {
        if (prvFatGetFileFromFd(fs, fd) == NULL)
            return fd;
    }
    return 0; // never reach
}

/**
 * open API
 */
static int prvFatOpen(void *ctx, const char *path, int flags, int mode)
{
    if (path == NULL)
        ERR_RETURN(EINVAL, -1);

    if (path[0] == '\0')
        ERR_RETURN(ENOENT, -1);

    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    if ((flags & (O_CREAT | O_WRONLY | O_RDWR)) && fs->read_only)
        ERR_RETURN(EROFS, -1);

    fatfsFile_t *fil = malloc(sizeof(fatfsFile_t));
    if (fil == NULL)
        ERR_RETURN(ENOMEM, -1);

    osiMutexLock(fs->lock);

    fil->fd = prvFatFindNewFd(fs);

    int m_mode = prvFatModeConv(flags);
    prvFatNameWithId(fs, fs->fat_path1, path);
    FRESULT fr = f_open(&fil->fil, fs->fat_path1, m_mode);
    if (fr != FR_OK)
    {
        free(fil);
        osiMutexUnlock(fs->lock);
        ERR_RETURN(prvFatResToErrno(fr), -1);
    }

    SLIST_INSERT_HEAD(&fs->fils, fil, iter);
    osiMutexUnlock(fs->lock);
    return fil->fd;
}

/**
 * close (inside lock)
 */
static int prvFatCloseLocked(fatfsContext_t *fs, int fd)
{
    fatfsFile_t *fil = prvFatGetFileFromFd(fs, fd);
    if (fil == NULL)
        return -ENOENT;

    FRESULT fr = f_close(&fil->fil);
    if (fr != FR_OK)
        return -prvFatResToErrno(fr);

    SLIST_REMOVE(&fs->fils, fil, fatfsFile, iter);
    free(fil);
    return 0;
}

/**
 * close API
 */
static int prvFatClose(void *ctx, int fd)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    int ret = prvFatCloseLocked(fs, fd);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

/**
 * read (inside lock)
 */
static ssize_t prvFatReadLocked(fatfsContext_t *fs, int fd, void *dst, size_t size)
{
    if (size == 0)
        return 0;
    if (dst == NULL)
        return -EINVAL;

    fatfsFile_t *fil = prvFatGetFileFromFd(fs, fd);
    if (fil == NULL)
        return -ENOENT;

    UINT read_len = 0;
    FRESULT fr = f_read(&fil->fil, dst, size, &read_len);
    return (fr == FR_OK) ? read_len : -prvFatResToErrno(fr);
}

/**
 * read API
 */
static ssize_t prvFatRead(void *ctx, int fd, void *dst, size_t size)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    ssize_t ret = prvFatReadLocked(fs, fd, dst, size);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

/**
 * write (inside lock)
 */
static ssize_t prvFatWriteLocked(fatfsContext_t *fs, int fd, const void *data, size_t size)
{
    if (size == 0)
        return 0;
    if (data == NULL)
        return -EINVAL;

    fatfsFile_t *fil = prvFatGetFileFromFd(fs, fd);
    if (fil == NULL)
        return -ENOENT;

    UINT write_len = 0;
    FRESULT fr = f_write(&fil->fil, data, size, &write_len);
    return (fr == FR_OK) ? write_len : -prvFatResToErrno(fr);
}

/**
 * write API
 */
static ssize_t prvFatWrite(void *ctx, int fd, const void *data, size_t size)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    if (fs->read_only)
    {
        osiMutexUnlock(fs->lock);
        ERR_RETURN(EROFS, -1);
    }
    ssize_t ret = prvFatWriteLocked(fs, fd, data, size);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

/**
 * lseek (inside lock)
 */
static long prvFatLseekLocked(fatfsContext_t *fs, int fd, long offset, int mode)
{
    fatfsFile_t *fil = prvFatGetFileFromFd(fs, fd);
    if (fil == NULL)
        return -ENOENT;

    int64_t off = offset;
    switch (mode)
    {
    case SEEK_SET:
        break;
    case SEEK_CUR:
        off += f_tell(&fil->fil);
        break;
    case SEEK_END:
        off += f_size(&fil->fil);
        break;
    default:
        return -EINVAL;
    }

    if (off < 0)
        off = 0;
    if (off > f_size(&fil->fil))
        off = f_size(&fil->fil);

    FRESULT fr = f_lseek(&fil->fil, (FSIZE_t)off);
    return (fr == FR_OK) ? f_tell(&fil->fil) : -prvFatResToErrno(fr);
}

/**
 * lseek API
 */
static long prvFatLseek(void *ctx, int fd, long offset, int mode)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    long ret = prvFatLseekLocked(fs, fd, offset, mode);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

/**
 * fstat (inside lock)
 */
static int prvFatFstatLocked(fatfsContext_t *fs, int fd, struct stat *st)
{
    if (st == NULL)
        return -EINVAL;

    fatfsFile_t *fil = prvFatGetFileFromFd(fs, fd);
    if (fil == NULL)
        return -ENOENT;

    st->st_size = f_size(&fil->fil);
    st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO;
    if (fil->fil.obj.attr & AM_RDO)
        st->st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    st->st_mode |= (fil->fil.obj.attr & AM_DIR) ? S_IFDIR : S_IFREG;
    st->st_mtime = 0;
    st->st_atime = 0;
    st->st_ctime = 0;
    return 0;
}

/**
 * stat (inside lock)
 */
static int prvFatStatLocked(fatfsContext_t *fs, const char *path, struct stat *st)
{
    if (st == NULL || path == NULL)
        return -EINVAL;

    if (*path == '\0')
    {
        st->st_size = 0;
        st->st_mtime = 0;
        st->st_atime = 0;
        st->st_ctime = 0;
        st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO | S_IFDIR;
        return 0;
    }

    prvFatNameWithId(fs, fs->fat_path1, path);
    FILINFO fno;
    FRESULT fr = f_stat(fs->fat_path1, &fno);
    if (fr != FR_OK)
        return -prvFatResToErrno(fr);

    st->st_size = fno.fsize;
    st->st_mtime = fno.ftime;
    st->st_atime = fno.ftime;
    st->st_ctime = fno.ftime;
    st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO;
    if (fno.fattrib & AM_RDO)
        st->st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    st->st_mode |= (fno.fattrib & AM_DIR) ? S_IFDIR : S_IFREG;
    return 0;
}

/**
 * fstat API
 */
static int prvFatFstat(void *ctx, int fd, struct stat *st)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    int ret = prvFatFstatLocked(fs, fd, st);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

/**
 * stat API
 */
static int prvFatStat(void *ctx, const char *path, struct stat *st)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    int ret = prvFatStatLocked(fs, path, st);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

/**
 * fsync (inside lock)
 */
static int prvFatFsyncLocked(fatfsContext_t *fs, int fd)
{
    fatfsFile_t *fil = prvFatGetFileFromFd(fs, fd);
    if (fil == NULL)
        return -ENOENT;

    FRESULT fr = f_sync(&fil->fil);
    return (fr == FR_OK) ? 0 : -prvFatResToErrno(fr);
}

/**
 * fsync API
 */
static int prvFatFsync(void *ctx, int fd)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    int ret = prvFatFsyncLocked(fs, fd);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

/**
 * ftruncate (inside lock)
 */
static int prvFatFtruncateLocked(fatfsContext_t *fs, int fd, long length)
{
    fatfsFile_t *fil = prvFatGetFileFromFd(fs, fd);
    if (fil == NULL)
        return -ENOENT;

    FSIZE_t size = f_size(&fil->fil);
    if (length == size)
        return 0;

    if (length < size)
    {
        FRESULT fr = f_lseek(&fil->fil, length);
        if (fr != FR_OK)
            return -prvFatResToErrno(fr);

        fr = f_truncate(&fil->fil);
        if (fr != FR_OK)
            return -prvFatResToErrno(fr);

        return 0;
    }

    FRESULT fr = f_expand(&fil->fil, length, 1);
    if (fr != FR_OK)
        return -prvFatResToErrno(fr);

    return 0;
}

/**
 * ftruncate API
 */
static int prvFatFtruncate(void *ctx, int fd, long length)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    int ret = prvFatFtruncateLocked(fs, fd, length);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

/**
 * truncate API
 */
static int prvFatTruncate(void *ctx, const char *path, long length)
{
    struct stat st;
    if (prvFatStat(ctx, path, &st) < 0)
        return -1;

    int fd = prvFatOpen(ctx, path, O_WRONLY, 0);
    if (fd < 0)
        return -1;

    int ret = prvFatFtruncate(ctx, fd, length);
    prvFatClose(ctx, fd);
    return ret;
}

/**
 * unlink API
 */
static int prvFatUnlink(void *ctx, const char *path)
{
    if (path == NULL)
        ERR_RETURN(EINVAL, -1);

    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    prvFatNameWithId(fs, fs->fat_path1, path);
    FRESULT fr = f_unlink(fs->fat_path1);
    osiMutexUnlock(fs->lock);
    CHECK_FRESULT_RETURN(fr);
}

/**
 * rename API
 */
static int prvFatRename(void *ctx, const char *src, const char *dst)
{
    if (src == NULL || dst == NULL)
        ERR_RETURN(EINVAL, -1);

    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    prvFatNameWithId(fs, fs->fat_path1, src);
    prvFatNameWithId(fs, fs->fat_path2, dst);
    FRESULT fr = f_rename(fs->fat_path1, fs->fat_path2);
    osiMutexUnlock(fs->lock);
    CHECK_FRESULT_RETURN(fr);
}

/**
 * opendir API
 */
static DIR *prvFatOpendir(void *ctx, const char *name)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;

    fatfsDir_t *dir = (fatfsDir_t *)calloc(1, sizeof(fatfsDir_t));
    if (dir == NULL)
        ERR_RETURN(ENOMEM, NULL);

    osiMutexLock(fs->lock);
    prvFatNameWithId(fs, fs->fat_path1, name);
    FRESULT fr = f_opendir(&dir->fat_dir, fs->fat_path1);
    if (fr == FR_OK)
        f_readdir(&dir->fat_dir, NULL);
    osiMutexUnlock(fs->lock);

    if (fr != FR_OK)
    {
        free(dir);
        ERR_RETURN(prvFatResToErrno(fr), NULL);
    }
    return &dir->v_dir;
}

/**
 * readdir_r (inside lock)
 */
static int prvFatReaddirRLocked(fatfsContext_t *fs, DIR *dir, struct dirent *entry, struct dirent **result)
{
    if (dir == NULL || entry == NULL)
        return -EINVAL;

    fatfsDir_t *mdir = (fatfsDir_t *)OSI_CONTAINER_OF(dir, fatfsDir_t, v_dir);
    FILINFO fno;
    FRESULT fr = f_readdir(&mdir->fat_dir, &fno);
    if (fr != FR_OK || fno.fname[0] == '\0')
    {
        if (result != NULL)
            *result = NULL;
        return 0;
    }

    entry->d_ino = 0;
    entry->d_type = (fno.fattrib & AM_DIR) ? DT_DIR : DT_REG;
    strcpy(entry->d_name, (const char *)&fno.fname);
    if (result != NULL)
        *result = entry;
    return 0;
}

/**
 * readdir_r API
 */
static int prvFatReaddirR(void *ctx, DIR *dir, struct dirent *entry, struct dirent **result)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    int ret = prvFatReaddirRLocked(fs, dir, entry, result);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

/**
 * readdir API
 */
static struct dirent *prvFatReaddir(void *ctx, DIR *dir)
{
    if (dir == NULL)
        ERR_RETURN(EINVAL, NULL);

    struct dirent *result = NULL;
    fatfsDir_t *mdir = (fatfsDir_t *)OSI_CONTAINER_OF(dir, fatfsDir_t, v_dir);
    if (prvFatReaddirR(ctx, dir, &mdir->e, &result) < 0)
        return NULL;

    return result;
}

/**
 * telldir API
 */
static long prvFatTelldir(void *ctx, DIR *dir)
{
    if (dir == NULL)
        ERR_RETURN(EINVAL, -1);

    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    fatfsDir_t *mdir = (fatfsDir_t *)OSI_CONTAINER_OF(dir, fatfsDir_t, v_dir);
    osiMutexLock(fs->lock);

    // set to the head
    f_readdir(&mdir->fat_dir, NULL);

    long n = 0;
    FILINFO fno;
    while (f_readdir(&mdir->fat_dir, &fno) == FR_OK && fno.fname[0] != '\0')
        n++;

    f_readdir(&mdir->fat_dir, NULL);
    osiMutexUnlock(fs->lock);
    return n;
}

/**
 * seekdir API
 */
static void prvFatSeekdir(void *ctx, DIR *dir, long loc)
{
    if (dir == NULL)
        return;

    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    fatfsDir_t *mdir = (fatfsDir_t *)OSI_CONTAINER_OF(dir, fatfsDir_t, v_dir);
    osiMutexLock(fs->lock);

    // set to the head
    f_readdir(&mdir->fat_dir, NULL);

    FILINFO fno;
    for (int n = 0; n < loc; n++)
    {
        FRESULT fr = f_readdir(&mdir->fat_dir, &fno);
        if (fr != FR_OK || fno.fname[0] == 0)
            break;
    }

    osiMutexUnlock(fs->lock);
    return;
}

/**
 * closedir API
 */
static int prvFatClosedir(void *ctx, DIR *dir)
{
    if (dir == NULL)
        ERR_RETURN(EINVAL, -1);

    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    fatfsDir_t *mdir = (fatfsDir_t *)OSI_CONTAINER_OF(dir, fatfsDir_t, v_dir);
    osiMutexLock(fs->lock);

    // set to the head
    f_readdir(&mdir->fat_dir, NULL);
    FRESULT fr = f_closedir(&mdir->fat_dir);

    osiMutexUnlock(fs->lock);

    free(dir);
    CHECK_FRESULT_RETURN(fr);
}

/**
 * mkdir API
 */
static int prvFatMkdir(void *ctx, const char *name, int mode)
{
    if (name == NULL)
        ERR_RETURN(EINVAL, -1);

    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    prvFatNameWithId(fs, fs->fat_path1, name);
    FRESULT fr = f_mkdir(fs->fat_path1);
    osiMutexUnlock(fs->lock);
    CHECK_FRESULT_RETURN(fr);
}

/**
 * rmdir (inside lock)
 */
static int prvFatRmdirLocked(fatfsContext_t *fs, const char *name)
{
    if (name == NULL)
        return -EINVAL;

    prvFatNameWithId(fs, fs->fat_path1, name);
    FILINFO fno;
    FRESULT fr = f_stat(fs->fat_path1, &fno);
    if (fr != FR_OK)
        return -prvFatResToErrno(fr);

    if (!(fno.fattrib & AM_DIR))
        return -ENOTDIR;

    fr = f_unlink(fs->fat_path1);
    if (fr == FR_DENIED)
        return -ENOTEMPTY;

    return (fr == FR_OK) ? 0 : -prvFatResToErrno(fr);
}

/**
 * rmdir API
 */
static int prvFatRmdir(void *ctx, const char *name)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    int ret = prvFatRmdirLocked(fs, name);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

/**
 * statvfs (inside lock)
 */
static int prvFatStatvfsLocked(fatfsContext_t *fs, struct statvfs *buf)
{
    if (buf == NULL)
        return -EINVAL;

    FATFS *fatfs;
    DWORD fre_clust;
    prvFatNameWithId(fs, fs->fat_path1, "");
    FRESULT fr = f_getfree(fs->fat_path1, &fre_clust, &fatfs);
    if (fr != FR_OK)
        return -prvFatResToErrno(fr);

    memset((void *)buf, 0, sizeof(*buf));
    buf->f_bsize = 512;
    buf->f_blocks = fatfs->n_fatent * fatfs->csize;
    buf->f_bfree = fre_clust * fatfs->csize;
    buf->f_bavail = fre_clust * fatfs->csize;
    buf->f_namemax = FF_MAX_LFN;
    return 0;
}

/**
 * statvfs API
 */
static int prvFatStatvfs(void *ctx, struct statvfs *buf)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    int ret = prvFatStatvfsLocked(fs, buf);
    osiMutexUnlock(fs->lock);
    CHECK_RESULT_RETURN(ret);
}

static int prvFatRemount(void *ctx, unsigned flags)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    fs->read_only = (flags & MS_RDONLY) ? true : false;
    osiMutexUnlock(fs->lock);
    return 0;
}

static int prvFatUmount(void *ctx)
{
    fatfsContext_t *fs = (fatfsContext_t *)ctx;

    osiMutexLock(fs->lock);
    prvFatNameWithId(fs, fs->fat_path1, "");
    f_unmount(fs->fat_path1);
    fat_disk_unregister_device(fs->fs_dev_no);
    osiMutexUnlock(fs->lock);

    osiMutexDelete(fs->lock);
    free(fs);
    return 0;
}

static vfs_ops_t g_fat_vfs_ops = {
    .flags = 0,
    .umount = prvFatUmount,
    .remount = prvFatRemount,
    .open = prvFatOpen,
    .close = prvFatClose,
    .read = prvFatRead,
    .write = prvFatWrite,
    .lseek = prvFatLseek,
    .fstat = prvFatFstat,
    .stat = prvFatStat,
    .truncate = prvFatTruncate,
    .ftruncate = prvFatFtruncate,
    .unlink = prvFatUnlink,
    .rename = prvFatRename,
    .opendir = prvFatOpendir,
    .readdir = prvFatReaddir,
    .readdir_r = prvFatReaddirR,
    .telldir = prvFatTelldir,
    .seekdir = prvFatSeekdir,
    .closedir = prvFatClosedir,
    .mkdir = prvFatMkdir,
    .rmdir = prvFatRmdir,
    .fsync = prvFatFsync,
    .statvfs = prvFatStatvfs,
    .sfile_init = NULL,
    .sfile_write = NULL,
    .sfile_read = NULL,
    .sfile_size = NULL,
};

int fatfs_vfs_mount(const char *base_path, blockDevice_t *bdev)
{
    if (bdev == NULL || base_path == NULL || strlen(base_path) > VFS_PREFIX_MAX)
        ERR_RETURN(EINVAL, -1);

    BYTE idx = fat_disk_register_device(bdev);
    if (idx >= FF_VOLUMES)
        ERR_RETURN(EXDEV, -1);

    fatfsContext_t *fs = (fatfsContext_t *)calloc(1, sizeof(fatfsContext_t));
    if (fs == NULL)
        ERR_RETURN(ENOMEM, -1);

    SLIST_INIT(&fs->fils);
    fs->lock = osiMutexCreate();
    fs->fs_dev_no = idx;

    prvFatNameWithId(fs, fs->fat_path1, "");
    FRESULT fr = f_mount(&fs->fs, fs->fat_path1, 1);
    if (fr != FR_OK)
    {
        SET_ERRNO(prvFatResToErrno(fr));
        goto failed;
    }

    if (vfs_register(base_path, &g_fat_vfs_ops, fs) < 0)
    {
        f_unmount(fs->fat_path1);
        goto failed;
    }

    OSI_LOGD(0, "FAT: fs_type = %d", fs->fs.fs_type);
    OSI_LOGD(0, "FAT: n_fats = %d", fs->fs.n_fats);
    OSI_LOGD(0, "FAT: wflag = %d", fs->fs.wflag);
    OSI_LOGD(0, "FAT: fsi_flag = %d", fs->fs.fsi_flag);
    OSI_LOGD(0, "FAT: id = %d", fs->fs.id);
    OSI_LOGD(0, "FAT: n_rootdir = %d", fs->fs.n_rootdir);
    OSI_LOGD(0, "FAT: csize = %d", fs->fs.csize);
    OSI_LOGD(0, "FAT: n_fatent = %d", fs->fs.n_fatent);
    OSI_LOGD(0, "FAT: fsize = %d", fs->fs.fsize);
    OSI_LOGD(0, "FAT: volbase = %d", fs->fs.volbase);
    OSI_LOGD(0, "FAT: fatbase = %d", fs->fs.fatbase);
    OSI_LOGD(0, "FAT: dirbase = %d", fs->fs.dirbase);
    OSI_LOGD(0, "FAT: database = %d", fs->fs.database);
    OSI_LOGD(0, "FAT: winsect = %d", fs->fs.winsect);
    return 0;

failed:
    fat_disk_unregister_device(fs->fs_dev_no);

    osiMutexDelete(fs->lock);
    free(fs);
    return -1;
}

int fatfs_vfs_mkfs(blockDevice_t *bdev)
{
    BYTE work[FF_MAX_SS];

    if (bdev == NULL)
        ERR_RETURN(EINVAL, -1);

    BYTE idx = fat_disk_register_device(bdev);
    if (idx >= FF_VOLUMES)
        ERR_RETURN(EXDEV, -1);

    fatfsContext_t fs;

    memset(&fs, 0, sizeof(fs));
    SLIST_INIT(&fs.fils);
    fs.fs_dev_no = idx;

    MKFS_PARM opt = {
        .fmt = FM_FAT,
        .n_fat = 2,
        .align = 4096,
        .n_root = 0,  // use default
        .au_size = 0, // auto selection
    };
    prvFatNameWithId(&fs, fs.fat_path1, "");
    FRESULT fr = f_mkfs(fs.fat_path1, &opt, work, sizeof(work));

    fat_disk_unregister_device(fs.fs_dev_no);
    CHECK_FRESULT_RETURN(fr);
}

int fatfs_vfs_set_volume(const char *mount, const char *label)
{
    if (mount == NULL || label == NULL)
        ERR_RETURN(EINVAL, -1);

    void *ctx = vfs_mount_handle(mount);
    if (ctx == NULL)
        ERR_RETURN(ENOENT, -1);

    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    prvFatNameWithId(fs, fs->fat_path1, label);
    FRESULT fr = f_setlabel(fs->fat_path1);
    osiMutexUnlock(fs->lock);

    CHECK_FRESULT_RETURN(fr);
}

int fatfs_vfs_get_volume(const char *mount, char *label, uint32_t *sn)
{
    if (mount == NULL)
        ERR_RETURN(EINVAL, -1);

    void *ctx = vfs_mount_handle(mount);
    if (ctx == NULL)
        ERR_RETURN(ENOENT, -1);

    fatfsContext_t *fs = (fatfsContext_t *)ctx;
    osiMutexLock(fs->lock);
    prvFatNameWithId(fs, fs->fat_path1, "");
    FRESULT fr = f_getlabel(fs->fat_path1, label, (DWORD *)sn);
    osiMutexUnlock(fs->lock);

    CHECK_FRESULT_RETURN(fr);
}
