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

#ifndef __SFFS_H__
#define __SFFS_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <vfs.h>

#ifdef __cplusplus
extern "C" {
#endif

struct blockDevice;

/**
 * opaque data structure of SFFS
 */
typedef struct sffsFs sffsFs_t;

/**
 * create a SFFS instance
 *
 * @param pdev      pointer of block device
 * @param block_cache_count     block cache count
 * @param read_only whether to mount as read-only
 * @return
 *      - SFFS pointer on success
 *      - NULL on failure, typical failure is root block mismatch
 */
sffsFs_t *sffsMount(struct blockDevice *pdev,
                    size_t block_cache_count, bool read_only);

/**
 * delete the SFFS instance
 *
 * @param fs    SFFS pointer
 * @return
 *      - 0 on success
 */
int sffsUnmount(sffsFs_t *fs);

/**
 * remount SFFS with flags
 *
 * The only supported flag is \a MS_RDONLY.
 *
 * @param fs    SFFS pointer
 * @param flags remount flags
 * @return
 *      - 0 for success
 */
int sffsRemount(sffsFs_t *fs, unsigned flags);

/**
 * format SFFS on block device
 *
 * @param pdev  block device pointer
 * @return
 *      - 0 for success
 *      - -EINVAL: invalid parameter
 *      - -ENOMEM: out of memory
 */
int sffsMakeFs(struct blockDevice *pdev);

/**
 * refer to open(2)
 *
 * It is permitted to open one file multiple times.
 *
 * It is not supported to open an existed directory.
 *
 * @param fs    SFFS pointer
 * @param path  path in file system
 * @param mode  open mode (O_CREAT, O_ACCMODE, O_TRUNC)
 * @return
 *      - file descriptor on success
 *      - -ENOENT: not exist, and without O_CREAT
 *      - -EROFS: create on read-only file system
 *      - -ENOTDIR: parent directory not exist at create
 *      - -ENOSPC: out of space
 *      - -ENAMETOOLONG: name is too long
 *      - -ENOMEM: out of memory
 */
int sffsOpen(sffsFs_t *fs, const char *path, int mode);

/**
 * refer to close(2)
 *
 * @param fs    SFFS pointer
 * @param fd    file descriptor
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer
 *      - -EBADF: invalid file descriptor
 */
int sffsClose(sffsFs_t *fs, int fd);

/**
 * refer to write(2)
 *
 * On failure, the return value is the negative errno. Written bytes won't
 * be returned.
 *
 * @param fs    SFFS pointer
 * @param fd    file descriptor
 * @param data  data pointer to be written
 * @param size  data size to be written
 * @return
 *      - written size on success
 *      - -EINVAL: invalid SFFS pointer
 *      - -EBADF: invalid file descriptor
 *      - -EROFS: write on read-only file system
 *      - -ENOSPC: out of space
 *      - -EOVERFLOW: exceed maximum large file size
 */
ssize_t sffsWrite(sffsFs_t *fs, int fd, const void *data, size_t size);

/**
 * refer to read(2)
 *
 * When there are no enough size in the file, the read size is returned.
 * On other failures, the return value is the negative errno. Read bytes
 * won't be returned.
 *
 * @param fs    SFFS pointer
 * @param fd    file descriptor
 * @param data  data pointer to be read
 * @param size  data size to be read
 * @return
 *      - read size on success
 *      - -EINVAL: invalid SFFS pointer
 *      - -EBADF: invalid file descriptor
 */
ssize_t sffsRead(sffsFs_t *fs, int fd, void *data, size_t size);

/**
 * refer to lseek(2)
 *
 * When the seek position is larger than file size, the position will be
 * set to end of file silently.
 *
 * @param fs    SFFS pointer
 * @param fd    file descriptor
 * @param offset    seek offset
 * @param mode  seek mode (SEEK_SET, SEEK_CUR, SEEK_END)
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer, or invalid seek mode
 *      - -EBADF: invalid file descriptor
 */
off_t sffsSeek(sffsFs_t *fs, int fd, off_t offset, int mode);

/**
 * refer to ftruncate(2)
 *
 * Support both increase and decrease file size. At increase file size,
 * the extended part is undefined.
 *
 * @param fs    SFFS pointer
 * @param fd    file descriptor
 * @param length    truncate new length
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer
 *      - -EBADF: invalid file descriptor
 *      - -EROFS: write on read-only file system
 *      - -EACCES: file descriptor is opened as read-only
 *      - -ENOSPC: out of space
 *      - -EOVERFLOW: exceed maximum large file size
 */
int sffsFtruncate(sffsFs_t *fs, int fd, off_t length);

/**
 * refer to fstat(2)
 *
 * @param fs    SFFS pointer
 * @param fd    file descriptor
 * @param st    output stat pointer
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer, or invalid output pointer
 *      - -EBADF: invalid file descriptor
 */
int sffsFstat(sffsFs_t *fs, int fd, struct stat *st);

/**
 * refer to unlink(2)
 *
 * @param fs    SFFS pointer
 * @param name  file path in file system
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer
 *      - -EROFS: write on read-only file system
 *      - -ENOENT: file not exist
 *      - -EBUSY: file is opened
 */
int sffsUnlink(sffsFs_t *fs, const char *name);

/**
 * refer to rename(2)
 *
 * Support 3 kinds of rename:
 * - rename file to an existed directory, but destination file not exist
 * - rename file to an existed file
 * - rename directory to an existed empty directory
 *
 * @param fs    SFFS pointer
 * @param src   source file or directory path in file system
 * @param dst   destination file or directory path in file system
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer
 *      - -EROFS: read-only file system
 *      - -ENOENT: source not exist
 *      - -EBUSY: source file or directory is opened
 *      - -ENOTDIR:
 *          - source is file, and destination in non-exist directory
 *          - source is directory, and destination is not existed directory
 *      - -EISDIR: source is file, and destination is existed directory
 *      - -ENOTEMPTY: source is directory, and destination is not empty
 *      - -ENAMETOOLONG: destination name is too long
 */
int sffsRename(sffsFs_t *fs, const char *src, const char *dst);

/**
 * refer to statvfs(3)
 *
 * @param fs    SFFS pointer
 * @param buf   output pointer
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer, or output pointer
 */
int sffsStatVfs(sffsFs_t *fs, struct statvfs *buf);

/**
 * refer to sync(2)
 *
 * @param fs    SFFS pointer
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer
 */
int sffsSync(sffsFs_t *fs);

/**
 * refer to mkdir(2)
 *
 * @param fs    SFFS pointer
 * @param name  directory path in file system
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer
 *      - -EROFS: read-only file system
 *      - -EEXIST: the path is existed
 *      - -ENOTDIR: parent of the path not exist
 *      - -ENOSPC: out of space
 *      - -ENAMETOOLONG: destination name is too long
 */
int sffsMkDir(sffsFs_t *fs, const char *name);

/**
 * refer to rmdir(2)
 *
 * @param fs    SFFS pointer
 * @param name  directory path in file system
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer
 *      - -EROFS: read-only file system
 *      - -EACCES: can't remove file system root directory
 *      - -ENOENT: path not exists
 *      - -ENOTDIR: parent of path not exists
 *      - -EBUSY: path is opened
 *      - -ENOTEMPTY: directory is not empty
 */
int sffsRmDir(sffsFs_t *fs, const char *name);

/**
 * refer to opendir(3)
 *
 * @param fs    SFFS pointer
 * @param name  directory path in file system
 * @return
 *      - DIR pointer on success
 *      - NULL on fail
 */
DIR *sffsOpenDir(sffsFs_t *fs, const char *name);

/**
 * refer to closedir(3)
 *
 * @param fs    SFFS pointer
 * @param dir   DIR pointer to be closed
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer, or DIR pointer
 */
int sffsCloseDir(sffsFs_t *fs, DIR *dir);

/**
 * refer to readdir_r(3)
 *
 * At the end of directory, the return value is 0, and \a *result will be
 * NULL.
 *
 * @param fs    SFFS pointer
 * @param dir   DIR pointer to be read
 * @param entry output dirent pointer
 * @param result    output pointer of dirent pointer
 * @return
 *      - 0 on success
 *      - -EINVAL: invalid SFFS pointer, or DIR pointer, or entry pointer
 *      - -EBADF: invalid DIR
 */
int sffsReadDirR(sffsFs_t *fs, DIR *dir, struct dirent *entry, struct dirent **result);

/**
 * refer to readdir(3)
 *
 * @param fs    SFFS pointer
 * @param dir   DIR pointer to be read
 * @return
 *      - dirent pointer on success
 *      - NULL on fail, or end of directory
 */
struct dirent *sffsReadDir(sffsFs_t *fs, DIR *dir);

/**
 * refer to telldir(3)
 *
 * @param fs    SFFS pointer
 * @param dir   DIR pointer to be read
 * @return
 *      - current location on success
 *      - -EINVAL: invalid SFFS pointer, or DIR pointer
 */
long sffsTellDir(sffsFs_t *fs, DIR *dir);

/**
 * refer to seekdir(3)
 *
 * @param fs    SFFS pointer
 * @param dir   DIR pointer to be read
 * @param loc   seek location
 */
void sffsSeekDir(sffsFs_t *fs, DIR *dir, long loc);

/**
 * write all content to a file in safe mode
 *
 * Comparing to \p sffsFileWrite, when the file already exists, and there
 * are power failure during write, either the file content to the new data,
 * or the content is kept as original.
 *
 * Due to the original file won't be deleted before the new data are
 * written, there should exist enough free blocks for the whole new data.
 *
 * @param fs    SFFS pointer
 * @param path  path in file system
 * @param data  data pointer to be written
 * @param size  data size to be written
 * @return
 *      - written size on success, it is the same as \p size parameter
 *      - -EINVAL: invalid SFFS pointer
 *      - -EROFS: create on read-only file system
 *      - -ENOTDIR: parent directory not exist at create
 *      - -EISDIR: path exists, but is directory
 *      - -ENOSPC: out of space
 *      - -ENAMETOOLONG: name is too long
 *      - -EOVERFLOW: exceed maximum large file size
 */
ssize_t sffsSfileWrite(sffsFs_t *fs, const char *path, const void *data, size_t size);

/**
 * write all content to a file
 *
 * @param fs    SFFS pointer
 * @param path  path in file system
 * @param data  data pointer to be written
 * @param size  data size to be written
 * @return
 *      - written size on success, it is the same as \p size parameter
 *      - -EINVAL: invalid SFFS pointer
 *      - -EROFS: create on read-only file system
 *      - -ENOTDIR: parent directory not exist at create
 *      - -EISDIR: path exists, but is directory
 *      - -ENOSPC: out of space
 *      - -ENAMETOOLONG: name is too long
 *      - -EOVERFLOW: exceed maximum large file size
 */
ssize_t sffsFileWrite(sffsFs_t *fs, const char *path, const void *data, size_t size);

/**
 * estimate block count needed for specified file size
 *
 * @param fs    SFFS pointer
 * @param size  file size to be estimated
 * @return
 *      - needed block count
 *      - -EOVERFLOW: exceed maximum large file size
 */
int sffsFileBlockNeeded(sffsFs_t *fs, size_t size);

/**
 * block write count for statistic
 *
 * @param fs    SFFS pointer
 * @return
 *      - block write count
 *      - -EINVAL: invalid SFFS pointer
 */
int sffsBlockWriteCount(sffsFs_t *fs);

/**
 * set reserved block count only for sffsSfileWrite
 *
 * Due to \p sffsSfileWrite will need extra blocks, this can set reserved
 * block count, which will only be used during \p sffsSfileWrite.
 *
 * In most cases, \p sffsSfileWrite is used for important information, and
 * failure is unacceptable. With proper reserved blocks, it won't fail due
 * to ENOSPC.
 *
 * Even current free block count is less than \p count, it will success.
 *
 * Another approach is to create independent partition for important files.
 *
 * @param fs    SFFS pointer
 * @param count reserved block count only for sffsSfileWrite
 * @return
 *      - 0 on success
 *      - -EINVAL on invalid parameter
 */
int sffsSetSfileReserveCount(sffsFs_t *fs, size_t count);

#ifdef __cplusplus
}
#endif
#endif //__SFFS_H__
