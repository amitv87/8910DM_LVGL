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

#ifndef _SFFS_VFS_H_
#define _SFFS_VFS_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct blockDevice;

/**
 * mount SFFS to VFS
 *
 * @param base_path mount point in VFS
 * @param bdev      block device pointer
 * @param cache_count   block cache count in SFFS
 * @param sfile_reserved_lb reserved logical block for sfile
 * @param read_only whether to mount as read-only
 * @return
 *      - 0 on success
 *      - -1 on fail
 */
int sffsVfsMount(const char *base_path,
                 struct blockDevice *bdev,
                 size_t cache_count,
                 size_t sfile_reserved_lb,
                 bool read_only);

/**
 * format SFFS on block device
 *
 * @param bdev  block device pointer
 * @return
 *      - 0 on success
 *      - -1 on fail
 */
int sffsVfsMkfs(struct blockDevice *bdev);

#ifdef __cplusplus
}
#endif

#endif
