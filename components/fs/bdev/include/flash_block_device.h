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

#ifndef _FLASH_BLOCK_DEVICE_H_
#define _FLASH_BLOCK_DEVICE_H_

#include "block_device.h"
#include "fs_config.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct drvSpiFlash;

#ifdef CONFIG_FS_FBDEV_DEFAULT_V1
#define FLASH_BLOCKDEVICE_CREATE flashBlockDeviceCreate
#define FLASH_BLOCKDEVICE_FORMAT flashBlockDeviceFormat
#endif

#ifdef CONFIG_FS_FBDEV_DEFAULT_V2
#define FLASH_BLOCKDEVICE_CREATE flashBlockDeviceCreateV2
#define FLASH_BLOCKDEVICE_FORMAT flashBlockDeviceFormatV2
#endif

/**
 * @brief create a flash block device
 *
 * There are cases fail to create flash block device:
 * - Failed to allocate memory for flash block device;
 * - Invalid parameters for flash block device;
 * - There are more than one EB doesn't match the configuration. It occurs
 *   when the parameters are not the same as the parameters at create.
 *   It should only happen at development.
 *
 * Except that, other errors will be handled silently. So, at initialization,
 * the return value should be checked, and call \a FLASH_BLOCKDEVICE_FORMAT
 * and then call \a FLASH_BLOCKDEVICE_CREATE again. If it is failed again,
 * usually \a osiPanic can be called.
 *
 * @param flash         the flash driver instance
 * @param phys_start    start offset in flash
 * @param phys_size     the region size
 * @param phys_erase_size   erase block size
 * @param block_size    block size of block device
 * @param read_only     at read only, no flash erase and program
 * @return
 *      - NULL if failed to create flash block device
 *      - instance pointer
 */
blockDevice_t *flashBlockDeviceCreate(
    struct drvSpiFlash *flash,
    uint32_t phys_start,
    uint32_t phys_size,
    uint32_t phys_erase_size,
    uint32_t block_size,
    bool read_only);

/**
 * @brief format flash region block device
 *
 * @param flash         the flash driver instance
 * @param phys_start    start offset in flash
 * @param phys_size     the region size
 * @param phys_erase_size   erase block size
 * @param block_size    block size of block device
 * @return
 *      - false if failed to format region for flash block device.
 *        Only when the configuration is invalid, it will fail.
 *      - true if the region is formatted
 */
bool flashBlockDeviceFormat(
    struct drvSpiFlash *flash,
    uint32_t phys_start,
    uint32_t phys_size,
    uint32_t phys_erase_size,
    uint32_t block_size);

/**
 * @brief create a flash block device version 2
 *
 * There are cases fail to create flash block device:
 * - Failed to allocate memory for flash block device;
 * - Invalid parameters for flash block device;
 * - There are more than one EB doesn't match the configuration. It occurs
 *   when the parameters are not the same as the parameters at create.
 *   It should only happen at development.
 *
 * Except that, other errors will be handled silently. So, at initialization,
 * the return value should be checked, and call \a FLASH_BLOCKDEVICE_FORMAT
 * and then call \a FLASH_BLOCKDEVICE_CREATE again. If it is failed again,
 * usually \a osiPanic can be called.
 *
 * \p block_size is PB of flash block device. In version 2, LB size is not
 * the same with PB size. So, always use blockDevice_t.block_size as LB size.
 *
 * @param flash         the flash driver instance
 * @param phys_start    start offset in flash
 * @param phys_size     the region size
 * @param phys_erase_size   erase block size
 * @param block_size    block size of block device
 * @param read_only     at read only, no flash erase and program
 * @return
 *      - NULL if failed to create flash block device
 *      - instance pointer
 */
blockDevice_t *flashBlockDeviceCreateV2(
    struct drvSpiFlash *flash,
    uint32_t phys_start,
    uint32_t phys_size,
    uint32_t phys_erase_size,
    uint32_t block_size,
    bool read_only);

/**
 * @brief quick create a flash block device version 2
 *
 * Comparing to \p flashBlockDeviceCreateV2, it is assumed that the flash
 * block device is *clean*, that is, there are no interrupted erase and
 * write. So, it can be faster.
 *
 * @param flash         the flash driver instance
 * @param phys_start    start offset in flash
 * @param phys_size     the region size
 * @param phys_erase_size   erase block size
 * @param block_size    block size of block device
 * @param read_only     at read only, no flash erase and program
 * @return
 *      - NULL if failed to create flash block device
 *      - instance pointer
 */
blockDevice_t *flashBlockDeviceQuickCreateV2(
    struct drvSpiFlash *flash,
    uint32_t phys_start,
    uint32_t phys_size,
    uint32_t phys_erase_size,
    uint32_t block_size,
    bool read_only);

/**
 * @brief format flash block device version 2
 *
 * @param flash         the flash driver instance
 * @param phys_start    start offset in flash
 * @param phys_size     the region size
 * @param phys_erase_size   erase block size
 * @param block_size    block size of block device
 * @return
 *      - false if failed to format region for flash block device.
 *        Only when the configuration is invalid, it will fail.
 *      - true if the region is formatted
 */
bool flashBlockDeviceFormatV2(
    struct drvSpiFlash *flash,
    uint32_t phys_start,
    uint32_t phys_size,
    uint32_t phys_erase_size,
    uint32_t block_size);

#ifdef __cplusplus
}
#endif

#endif
