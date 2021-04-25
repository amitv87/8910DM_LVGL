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

#ifndef _DRV_SPI_FLASH_H_
#define _DRV_SPI_FLASH_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "drv_names.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief opaque data structure for SPI flash
 */
typedef struct drvSpiFlash drvSpiFlash_t;

/**
 * \brief open SPI flash
 *
 * SPI flash instance is singleton for each flash. So, for each device name
 * the returned instance pointer is the same. And there are no \p close
 * API.
 *
 * At the first open, the flash will be initialized (for faster speed).
 *
 * When \p name is DRV_NAME_SPI_FLASH_EXT, even CONFIG_BOARD_WITH_EXT_FLASH
 * isn't defined, this will initialize external flash.
 *
 * \param name SPI flash device name
 * \return
 *      - SPI flash instance pointer
 *      - NULL if the name is invalid
 */
drvSpiFlash_t *drvSpiFlashOpen(uint32_t name);

/**
 * \brief set whether XIP is enabled
 *
 * When XIP is enabled, flash operation protect the conflict of code
 * execution on flash.
 *
 * The typical case is external flash for file system only. In this,
 * set XIP to false can make flash erase/write faster.
 *
 * By default XIP protection is enabled. The flash on primary flash
 * controller can't be changed.
 *
 * \param d SPI flash instance pointer, must be valid
 * \param enable XIP protection enable
 * \return
 *      - true on success
 *      - false on invalid prameters
 */
bool drvSpiFlashSetXipEnabled(drvSpiFlash_t *d, bool enable);

/**
 * \brief set whether to enable suspend
 *
 * By default, flash driver will suspend erase or program when there are
 * pending interrupt, and if the flash supports suspend. In case, it is
 * known that erase and program will be called in critical section, it is
 * needed to call this to disable suspend temporally. Otherwise, erase or
 * program may consume very long time, or never finish.
 *
 * In normal application, it should be avoided to call flash driver inside
 * interrupt service or critical section. So, most likely it is not needed
 * to call this. The main purpose is for debugging.
 *
 * \param d SPI flash instance pointer, must be valid
 * \param enable suspend enabled
 * \return
 *      - true on success
 *      - false on invalid prameters
 */
bool drvSpiFlashSetSuspendEnabled(drvSpiFlash_t *d, bool enable);

/**
 * \brief prohibit flash erase/program for a certain range
 *
 * By default, flash erase/program is not prohibited. If it is known that
 * certain range will never be changed, this can be called. This range will
 * be used to check erase and program parameters. When the erase or program
 * address is located in the range, erase or program API will return false.
 * For example, it the first block (usually it is bootloader) will never be
 * modified:
 *
 * \code{.cpp}
 * drvSpiFlashSetRangeWriteProhibit(flash, 0, 0x10000);
 * \endcode
 *
 * For multiple non-coninuous ranges, this can be called multple times.
 *
 * Inside, there is a flag for each block (64KB). So, the check unit is 64KB
 * block. Also, \p start will be aligned up, \p end will be aligned down.
 * So, the followings won't set any prohibit.
 *
 * \code{.cpp}
 * drvSpiFlashSetRangeWriteProhibit(flash, 0x10000, 0x18000);
 * drvSpiFlashSetRangeWriteProhibit(flash, 0x18000, 0x20000);
 * \endcode
 *
 * \param d SPI flash instance pointer, must be valid
 * \param start range start (inclusive)
 * \param end range end (exclusive)
 */
void drvSpiFlashSetRangeWriteProhibit(drvSpiFlash_t *d, uint32_t start, uint32_t end);

/**
 * \brief clear range write prohibit flag
 *
 * This is not recommended, just for completeness.
 *
 * \param d SPI flash instance pointer, must be valid
 * \param start range start (inclusive)
 * \param end range end (exclusive)
 */
void drvSpiFlashClearRangeWriteProhibit(drvSpiFlash_t *d, uint32_t start, uint32_t end);

/**
 * \brief get flash properties
 *
 * \p halSpiFlash_t is defined in \p hal_spi_flash.h
 *
 * The returned property pointer is the property used by the driver.
 * It is designed not to be const. If you really know that the default
 * property from manufacture ID doesn't match you flash, you can
 * change it.
 *
 * \param d SPI flash instance pointer, must be valid
 * \return
 *      - flash properties
 */
struct halSpiFlash *drvSpiFlashGetProp(drvSpiFlash_t *d);

/**
 * \brief get flash manufactor id
 *
 * The contents of the id:
 * - id[23:16]: capacity
 * - id[15:8]: memory type
 * - id[7:0]: manufacturer
 *
 * \param d SPI flash instance pointer, must be valid
 * \return
 *      - flash ID
 */
uint32_t drvSpiFlashGetID(drvSpiFlash_t *d);

/**
 * \brief get flash capacity in byte
 *
 * \param d SPI flash instance pointer, must be valid
 * \return
 *      - flash capacity in byte
 */
uint32_t drvSpiFlashCapacity(drvSpiFlash_t *d);

/**
 * \brief map flash offset to accessible address
 *
 * The returned pointer should be accessed as read only.
 *
 * This is incompatible with flash not on flash controller. So,
 * \p drvSpiFlashRead and \p drvSpiFlashReadCheck is recommended.
 *
 * \param d SPI flash instance pointer, must be valid
 * \param offset flash offset
 * \return
 *      - accessible pointer
 *      - NULL on error, invalid parameters
 */
const void *drvSpiFlashMapAddress(drvSpiFlash_t *d, uint32_t offset);

/**
 * \brief read data from flash
 *
 * \param d SPI flash instance pointer, must be valid
 * \param offset flash offset
 * \param data memory for read
 * \param size read size
 * \return
 *      - true on success
 *      - false on error, invalid parameters
 */
bool drvSpiFlashRead(drvSpiFlash_t *d, uint32_t offset, void *data, uint32_t size);

/**
 * \brief read data from flash, and check with provided data
 *
 * It is close to \p drvSpiFlashRead with \p memcmp. And it can return false
 * earlier at mismatch. Also, it can save another extra buffer.
 *
 * \param d SPI flash instance pointer, must be valid
 * \param offset flash offset
 * \param data memory for read
 * \param size read size
 * \return
 *      - true on success
 *      - false on error, invalid parameters
 */
bool drvSpiFlashReadCheck(drvSpiFlash_t *d, uint32_t offset, const void *data, uint32_t size);

/**
 * \brief write to flash
 *
 * This will just use flash PROGRAM command to write. Caller should ensure
 * the write sector or block is erased before. Otherwise, the data on
 * flash may be different from the input data.
 *
 * \p offset and \p size are not needed to be sector or block aligned.
 *
 * It will wait flash write finish. On success return, all data are written
 * to flash.
 *
 * It won't read back to verify the result.
 *
 * Caller should take care about the location of \p data. When \p data is on
 * flash of the driver, it may cause hang up.
 *
 * \param d SPI flash instance pointer, must be valid
 * \param offset flash offset
 * \param data memory for write
 * \param size write size
 * \return
 *      - true on success
 *      - false on invalid parameters, or soft write protected
 */
bool drvSpiFlashWrite(drvSpiFlash_t *d, uint32_t offset, const void *data, size_t size);

/**
 * \brief erase flash sectors or blocks
 *
 * Both \p offset and \p size must be sector (4KB) aligned.
 *
 * When \p size is larger than sector or block, it will loop inside until
 * all requested sector or block are erased.
 *
 * It will wait flash erase finish. On success return, the whole region is
 * erased.
 *
 * It won't read back to verify the result.
 *
 * \param d SPI flash instance pointer, must be valid
 * \param offset flash offset
 * \param size erase size
 * \return
 *      - true on success
 *      - false on invalid parameters, or soft write protected
 */
bool drvSpiFlashErase(drvSpiFlash_t *d, uint32_t offset, size_t size);

/**
 * \brief erase the whole flash chip
 *
 * In normal case, it shouldn't be used by application.
 *
 * \param d SPI flash instance pointer, must be valid
 * \return
 *      - true on success
 *      - false on invalid parameters, or soft write protected
 */
bool drvSpiFlashChipErase(drvSpiFlash_t *d);

/**
 * \brief read flash status register
 *
 * When flash support (at least) two status registers, this will return two
 * status registers. Otherwise, it will return only one status register.
 *
 * \param d SPI flash instance pointer, must be valid
 * \return
 *      - flash status register
 */
uint16_t drvSpiFlashReadSR(drvSpiFlash_t *d);

/**
 * \brief write flash status register
 *
 * When flash support (at least) two status registers, this will write two
 * status registers. Otherwise, it will write only one status register.
 *
 * \param d SPI flash instance pointer, must be valid
 * \param sr value to be written to flash status register
 */
void drvSpiFlashWriteSR(drvSpiFlash_t *d, uint16_t sr);

/**
 * \brief enter deep power down mode
 *
 * When XIP is enabled, it must be called with interrupt disabled.
 *
 * After enter deep power down mode, \p drvSpiFlashReleaseDeepPowerDown must
 * be called before access.
 *
 * \param d SPI flash instance pointer, must be valid
 */
void drvSpiFlashDeepPowerDown(drvSpiFlash_t *d);

/**
 * \brief release from deep power down mode
 *
 * When XIP is enabled, it must be called with interrupt disabled.
 *
 * \param d SPI flash instance pointer, must be valid
 */
void drvSpiFlashReleaseDeepPowerDown(drvSpiFlash_t *d);

/**
 * \brief read unique id number
 *
 * The currently known largest uid length is 16 bytes.
 *
 * \param d SPI flash instance pointer, must be valid
 * \param id unique id, enough to hold uid
 * \return
 *      - uid length
 *      - -1 on error, invalid parameters or not support
 */
int drvSpiFlashReadUniqueId(drvSpiFlash_t *d, uint8_t *id);

/**
 * \brief read cp id
 *
 * \param d SPI flash instance pointer, must be valid
 * \return
 *      - cp id
 *      - 0 if not supported
 */
uint16_t drvSpiFlashReadCpId(drvSpiFlash_t *d);

/**
 * \brief read security register
 *
 * \param d SPI flash instance pointer, must be valid
 * \param num security register number
 * \param address address inside security register
 * \param data pointer to output data
 * \param size read size
 * \return
 *      - true on success
 *      - false on invalid parameter, or not support
 */
bool drvSpiFlashReadSecurityRegister(drvSpiFlash_t *d, uint8_t num, uint16_t address, void *data, uint32_t size);

/**
 * \brief program security register
 *
 * \param d SPI flash instance pointer, must be valid
 * \param num security register number
 * \param address address inside security register
 * \param data data to be programmed
 * \param size program size
 * \return
 *      - true on success
 *      - false on invalid parameter, or not support
 */
bool drvSpiFlashProgramSecurityRegister(drvSpiFlash_t *d, uint8_t num, uint16_t address, const void *data, uint32_t size);

/**
 * \brief program security register
 *
 * \param d SPI flash instance pointer, must be valid
 * \param num security register number
 * \return
 *      - true on success
 *      - false on invalid parameter, or not support
 */
bool drvSpiFlashEraseSecurityRegister(drvSpiFlash_t *d, uint8_t num);

/**
 * \brief lock security register
 *
 * Lock are OTP bits in status register. Once it is locked, there are
 * no ways to erase/program the security register.
 *
 * \param d SPI flash instance pointer, must be valid
 * \param num security register number
 * \return
 *      - true on success
 *      - false on invalid parameter, or not support
 */
bool drvSpiFlashLockSecurityRegister(drvSpiFlash_t *d, uint8_t num);

/**
 * \brief whether security register is locked
 *
 * \param d SPI flash instance pointer, must be valid
 * \param num security register number
 * \return
 *      - true if \p num is valid and locked
 *      - false if not locked, or invalid parameter
 */
bool drvSpiFlashIsSecurityRegisterLocked(drvSpiFlash_t *d, uint8_t num);

/**
 * \brief read SFDP
 *
 * \param d SPI flash instance pointer, must be valid
 * \param address SFDP address
 * \param data data to be read
 * \param size read size
 * \return
 *      - true on success
 *      - false on fail, invalid parameters or not support
 */
bool drvSpiFlashReadSFDP(drvSpiFlash_t *d, unsigned address, void *data, unsigned size);

/**
 * \brief unique id length
 *
 * \param d SPI flash instance pointer, must be valid
 * \return unique id length, 0 for not support
 */
int drvSpiFlashUidLength(drvSpiFlash_t *d);

#ifdef __cplusplus
}
#endif
#endif
