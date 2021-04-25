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

#ifndef _FATFS_PORTABLE_H_
#define _FATFS_PORTABLE_H_

#include "osi_compiler.h"
#include "ff.h"
#include "diskio.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief forward declaration
 */
struct blockDevice;

/**
 * \brief register a block device to fatfs
 *
 * fatfs uses BYTE as device. And the mapping between BYTE and block
 * device instance are maintained outside.
 *
 * \param dev block device
 * \return
 *      - fatfs device index
 *      - 0xFF on error
 */
BYTE fat_disk_register_device(struct blockDevice *dev);

/**
 * \brief unregister block device at specified index
 *
 * \param idx fatfs device index
 * \return
 *      - true on success
 *      - false on error, invalid parameter
 */
bool fat_disk_unregister_device(BYTE idx);

/**
 * \brief get drive status
 *
 * \param idx fatfs device index
 * \return drive status
 */
DSTATUS fat_disk_status(BYTE idx);

/**
 * \brief initialize a drive
 *
 * \param idx fatfs device index
 * \return drive status
 */
DSTATUS fat_disk_initialize(BYTE idx);

/**
 * \brief read sector or sectors
 *
 * \param idx fatfs device index
 * \param buff data buffer to store read data
 * \param sector start sector in LBA
 * \param count number of sectors to read
 * \return result
 */
DRESULT fat_disk_read(BYTE idx, BYTE *buff, DWORD sector, UINT count);

/**
 * \brief write sector or sectors
 *
 * \param idx fatfs device index
 * \param buff data to be written
 * \param sector start sector in LBA
 * \param count number of sectors to read
 * \return result
 */
DRESULT fat_disk_write(BYTE idx, const BYTE *buff, DWORD sector, UINT count);

/**
 * \brief miscellaneous functions
 *
 * \param idx fatfs device index
 * \param cmd control code
 * \param buff buffer to send/receive control data
 * \return result
 */
DRESULT fat_disk_ioctl(BYTE idx, BYTE cmd, void *buff);

OSI_EXTERN_C_END
#endif
