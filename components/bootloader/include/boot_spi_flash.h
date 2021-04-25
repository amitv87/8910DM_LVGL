/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _BOOT_SPI_FLASH_H_
#define _BOOT_SPI_FLASH_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * opaque data structure for SPI NOR flash
 */
typedef struct bootSpiFlash bootSpiFlash_t;

/**
 * \brief open SPI flash
 *
 * When \p name is DRV_NAME_SPI_FLASH_EXT, even CONFIG_BOARD_WITH_EXT_FLASH
 * isn't defined, this will initialize external flash.
 *
 * \param name SPI flash device name
 * \return SPI flash instance
 */
bootSpiFlash_t *bootSpiFlashOpen(unsigned name);

/**
 * \brief prohibit flash erase/program for a certain range
 *
 * By default, flash erase/program is not prohibited. If it is known that
 * certain range will never be changed, this can be called. This range will
 * be used to check erase and program parameters. When the erase or program
 * address is located in the range, erase or program API will return false.
 *
 * For multiple non-coninuous ranges, this can be called multple times.
 *
 * Inside, there is a flag for each block (64KB). So, the check unit is 64KB
 * block. Also, \p start will be aligned up, \p end will be aligned down.
 *
 * \param d SPI flash instance
 * \param start range start (inclusive)
 * \param end range end (exclusive)
 */
void bootSpiFlashSetRangeWriteProhibit(bootSpiFlash_t *d, uint32_t start, uint32_t end);

/**
 * \brief clear range write prohibit flag
 *
 * This is not recommended, just for completeness.
 *
 * \param d SPI flash instance
 * \param start range start (inclusive)
 * \param end range end (exclusive)
 */
void bootSpiFlashClearRangeWriteProhibit(bootSpiFlash_t *d, uint32_t start, uint32_t end);

/**
 * \brief check whether flash offset offset is valid
 *
 * \param d SPI flash instance
 * \param offset flash offset
 * \return
 * - true if flash offset is valid
 * - false if flash offset is invalid
 */
bool bootSpiFlashOffsetValid(bootSpiFlash_t *d, uint32_t offset);

/**
 * \brief check whether the address is valid map address
 *
 * \param d SPI flash instance
 * \param address address to be checked
 * \return
 * - true if the address is valid map address
 * - false if the address is not valid map address
 */
bool bootSpiFlashMapAddressValid(bootSpiFlash_t *d, const void *address);

/**
 * \brief mapped address for flash offset
 *
 * The mapped address is the physical address.
 *
 * \param d SPI flash instance
 * \param offset flash offset
 * \return pointer of the mapped address
 */
const void *bootSpiFlashMapAddress(bootSpiFlash_t *d, uint32_t offset);

/**
 * \brief flash ID
 *
 * The format of the ID:
 * [31:24] 0
 * [23:16] capacity bits
 * [15:8]
 * [7:0] manufacture ID
 *
 * \param d SPI flash instance
 * \return flash ID
 */
uint32_t bootSpiFlashID(bootSpiFlash_t *d);

/**
 * \brief flash capacity in bytes
 *
 * \param d SPI flash instance
 * \return capacity in bytes
 */
uint32_t bootSpiFlashCapacity(bootSpiFlash_t *d);

/**
 * \brief wait erase or program in progress finish
 *
 * \param d SPI flash instance
 */
void bootSpiFlashWaitDone(bootSpiFlash_t *d);

/**
 * \brief whether erase or program in progress finish
 *
 * \param d SPI flash instance
 * \return
 *      - true if there are no pending erase or program
 *      - false if not
 */
bool bootSpiFlashIsDone(bootSpiFlash_t *d);

/**
 * \brief flash chip erase
 *
 * This function will wait erase finish.
 *
 * \param d SPI flash instance
 */
void bootSpiFlashChipErase(bootSpiFlash_t *d);

/**
 * \brief erase block or sector of flash
 *
 * 64K/32K/4K erase will be chosen based on offset alignment and size.
 *
 * This function WON'T wait erase finish.
 *
 * \param d SPI flash instance
 * \param offset flash offset
 * \param size flash erase size
 * \return
 *      - the actual erase size
 *      - -1 for invalid parameters
 */
int bootSpiFlashEraseNoWait(bootSpiFlash_t *d, uint32_t offset, size_t size);

/**
 * \brief write data to flash
 *
 * Flash write won't cross page boundary.
 *
 * This function WON'T wait erase finish.
 *
 * \param d SPI flash instance
 * \param offset flash offset
 * \param size flash program size
 * \return
 *      - the actual program size
 *      - -1 for invalid parameters
 */
int bootSpiFlashWriteNoWait(bootSpiFlash_t *d, uint32_t offset, const void *data, size_t size);

/**
 * \brief erase region of flash
 *
 * This function will loop to erase the whole flash region, and wait erase
 * done.
 *
 * \param d SPI flash instance
 * \param offset flash offset
 * \param size flash erase size
 * \return
 *      - true on success
 *      - false on error, invalid parameters
 */
bool bootSpiFlashErase(bootSpiFlash_t *d, uint32_t offset, size_t size);

/**
 * \brief write all data to flash
 *
 * This function will loop to write the whole flash region, and wait
 * program finish. Caller should ensure \a data is not located in the
 * same flash.
 *
 * \param d SPI flash instance
 * \param offset flash offset
 * \param data data to be written
 * \param size flash erase size
 * \return
 *      - true on success
 *      - false on error, invalid parameters
 */
bool bootSpiFlashWrite(bootSpiFlash_t *d, uint32_t offset, const void *data, size_t size);

/**
 * \brief read data from flash
 *
 * \param d SPI flash instance
 * \param offset flash offset
 * \param data memory for read
 * \param size read size
 * \return
 *      - true on success
 *      - false on error, invalid parameters
 */
bool bootSpiFlashRead(bootSpiFlash_t *d, uint32_t offset, void *data, uint32_t size);

/**
 * \brief read data from flash, and check with provided data
 *
 * It is close to \p bootSpiFlashRead with \p memcmp. And it can return false
 * earlier at mismatch. Also, it can save another extra buffer.
 *
 * \param d SPI flash instance
 * \param offset flash offset
 * \param data memory for read
 * \param size read size
 * \return
 *      - true on success
 *      - false on error, invalid parameters
 */
bool bootSpiFlashReadCheck(bootSpiFlash_t *d, uint32_t offset, const void *data, uint32_t size);

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
int bootSpiFlashReadUniqueId(bootSpiFlash_t *d, uint8_t *id);

#ifdef __cplusplus
}
#endif
#endif
