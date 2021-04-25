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

#ifndef _FS_MOUNT_H_
#define _FS_MOUNT_H_

#include <stdint.h>
#include <stdbool.h>
#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief file system usage scenario
 *
 * The file system mount options may be different on varioud scenario.
 */
typedef enum
{
    FS_SCENRARIO_UNKNOWN,    ///< prohibitted
    FS_SCENRARIO_APP,        ///< normal application
    FS_SCENRARIO_BOOTLOADER, ///< bootloader
    FS_SCENRARIO_FDL,        ///< FDL
} fsMountScenario_t;

/**
 * \brief set file system usage scenario
 *
 * The default scenario is \p FS_SCENRARIO_APP.
 *
 * \param scenario file system usage scenario
 */
void fsMountSetScenario(fsMountScenario_t scenario);

/**
 * \brief mount all file systems
 *
 * It will return false on the first error, such as mount fail.
 *
 * When a block device or file system is already mounted, the mounted block
 * device or file system won't be touched. So, it won't cause duplicated
 * mount.
 *
 * \return
 *      - true on success
 *      - false on fail
 */
bool fsMountAll(void);

/**
 * \brief mount all file systems, format on fail
 *
 * It is **dangerous**. Only call it at absolutely needed. Also, Don't call
 * this in application.
 *
 * \return
 *      - true on success
 *      - false on fail
 */
bool fsMountWithFormatAll(void);

/**
 * \brief format a file system
 *
 * It will perform:
 * - unmount file system;
 * - create flash block device, and format on fail;
 * - format and mount file system;
 *
 * \param path mount point
 * \return
 *      - true on success
 *      - false on fail
 */
bool fsMountFormat(const char *path);

/**
 * \brief mount file system by mount point
 *
 * When the file system is already mounted, it will do nothing and return
 * true.
 *
 * At mount fail and \p format_on_fail is true, when there is only one file
 * system on the underlay device, the device will be formatted. Otherwise,
 * when there are multiple file systems on the underlay device, due to
 * sub-partition, it will return false.
 *
 * \param path mount point
 * \param format_on_fail try to format at mount fail
 * \return
 *      - true on success
 *      - false on fail
 */
bool fsMountFsPath(const char *path, bool format_on_fail);

/**
 * \brief format all file systems on flash block device
 *
 * When sub-partition is not used, there will only exist one file system
 * on the block device. When sub-partition is used, there may exist
 * multiple sub-partitions on the flash block device, and there may exit
 * one file system on each sub-partition.
 *
 * It will perform:
 * - unmount all file systems;
 * - create flash block device, and format on fail;
 * - format and mount all file systems;
 *
 * \param name flash block device name
 * \return
 *      - true on success
 *      - false on fail
 */
bool fsMountFormatFlash(unsigned name);

/**
 * \brief umount all file system and block devices
 */
void fsUmountAll(void);

/**
 * \brief remount factory file system
 *
 * Refer to \p vfs_remount.
 *
 * \param flags file system mount flags
 */
void fsRemountFactory(unsigned flags);

/**
 * \brief find block device by device name
 *
 * \p name is the name in partition information configuration file.
 *
 * \param name block device name
 * \return
 *      - block device instance
 *      - NULL if not found
 */
struct blockDevice *fsFindBlockDeviceByName(unsigned name);

/**
 * \brief get all block device names
 *
 * \param names output for block device names
 * \param count maximum count
 * \return block device name count
 */
int fsGetBlockDeviceNames(unsigned names[], unsigned count);

OSI_EXTERN_C_END
#endif
