/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification. RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _HAL_SPI_FLASH_H_
#define _HAL_SPI_FLASH_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "osi_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief flash instance
 *
 * The preperty is derived from manufacture ID. It is found that even
 * for the same manufacture ID, the property may be different, for
 * example, security register block size. In this case, user can
 * override the perperty.
 */
typedef struct halSpiFlash
{
    /** hardware controller register base */
    uintptr_t hwp;
    /** manufacture id */
    unsigned mid;
    /** capacity in bytes */
    unsigned capacity;
    /** security register block size, 0 for not support */
    uint16_t sreg_block_size;
    /** internal flash type */
    uint8_t type : 4;
    /** status register wp type */
    uint8_t wp_type : 4;
    /** read uid type */
    uint8_t uid_type : 4;
    /** whether cpid is supported */
    uint8_t cpid_type : 4;
    /** minimal security register number */
    uint8_t sreg_min_num : 4;
    /** maximum security register number */
    uint8_t sreg_max_num : 4;
    /** whether volatile sr is supported */
    bool volatile_sr_en : 1;
    /** whether suspend resume is supported */
    bool suspend_en : 1;
    /** whether 5AH to read SFDP supported */
    bool sfdp_en : 1;
    /** whether 01H can write both SR1 and SR2 */
    bool write_sr12 : 1;
    /** whether 35H can read SR2 */
    bool has_sr2 : 1;
    /** whether has GD_SR_SUS1 */
    bool has_sus1 : 1;
    /** whether has GD_SR_SUS2 */
    bool has_sus2 : 1;
} halSpiFlash_t;

/**
 * \brief write enable
 *
 * \param d hal spi flash instance
 */
void halSpiFlashWriteEnable(const halSpiFlash_t *d);

/**
 * \brief write disable
 *
 * \param d hal spi flash instance
 */
void halSpiFlashWriteDisable(const halSpiFlash_t *d);

/**
 * \brief program suspend
 *
 * \param d hal spi flash instance
 */
void halSpiFlashProgramSuspend(const halSpiFlash_t *d);

/**
 * \brief erase suspend
 *
 * \param d hal spi flash instance
 */
void halSpiFlashEraseSuspend(const halSpiFlash_t *d);

/**
 * \brief program resume
 *
 * \param d hal spi flash instance
 */
void halSpiFlashProgramResume(const halSpiFlash_t *d);

/**
 * \brief erase resume
 *
 * \param d hal spi flash instance
 */
void halSpiFlashEraseResume(const halSpiFlash_t *d);

/**
 * \brief chip erase
 *
 * \param d hal spi flash instance
 */
void halSpiFlashChipErase(const halSpiFlash_t *d);

/**
 * \brief reset enable
 *
 * \param d hal spi flash instance
 */
void halSpiFlashResetEnable(const halSpiFlash_t *d);

/**
 * \brief reset
 *
 * \param d hal spi flash instance
 */
void halSpiFlashReset(const halSpiFlash_t *d);

/**
 * \brief read status register
 *
 * When only one SR is supported, it will return the SR. When two SR are
 * supported, it will return 16bits SR, SR-1 at LSB and SR-2 at MSB.
 *
 * \param d hal spi flash instance
 * \return status register
 */
uint16_t halSpiFlashReadSR(const halSpiFlash_t *d);

/**
 * \brief write status register
 *
 * The implementation will depend on flash property, one status register,
 * two status registers with one command, or two status registers with
 * separated commands.
 *
 * Inside, it will send write enable command, and wait write finish.
 *
 * \param d hal spi flash instance
 * \param sr status register value
 */
void halSpiFlashWriteSR(const halSpiFlash_t *d, uint16_t sr);

/**
 * \brief page program
 *
 * \p data shouldn't be allocated in flash. \p size should be less than
 * hardware TX fifo size.
 *
 * It will only send program command, write enable and wait finish shall
 * be considered by caller.
 *
 * \param d hal spi flash instance
 * \param offset flash address
 * \param data data to be programmed
 * \param size data size
 */
void halSpiFlashPageProgram(const halSpiFlash_t *d, uint32_t offset, const void *data, uint32_t size);

/**
 * \brief 4K/32K/64K erase
 *
 * \p size can only be 4K, 32K or 64K. \p offset should be \p size aligned.
 *
 * \param d hal spi flash instance
 * \param offset flash address
 * \param size erase size
 */
void halSpiFlashErase(const halSpiFlash_t *d, uint32_t offset, uint32_t size);

/**
 * \brief read unique id number
 *
 * Not all flash supports unique id, and the unique id size is different even
 * supported. \p uid should be large enough.
 *
 * \param d hal spi flash instance
 * \param uid unique id
 * \return
 *      - unique id size
 *      - 0 if unique id is not supported
 */
int halSpiFlashReadUniqueId(const halSpiFlash_t *d, uint8_t *uid);

/**
 * \brief read cp id
 *
 * Not all flash support cp id.
 *
 * \param d hal spi flash instance
 * \return
 *      - cp id
 *      - 0 if not supported
 */
uint16_t halSpiFlashReadCpId(const halSpiFlash_t *d);

/**
 * \brief deep power down
 *
 * \param d hal spi flash instance
 */
void halSpiFlashDeepPowerDown(const halSpiFlash_t *d);

/**
 * \brief release from deep power down
 *
 * There are delay inside, to make sure flash is accessible at return.
 *
 * \param d hal spi flash instance
 */
void halSpiFlashReleaseDeepPowerDown(const halSpiFlash_t *d);

/**
 * \brief read SFDP
 *
 * \param d hal spi flash instance
 * \param address SFDP address
 * \param data data to be read
 * \param size read size
 * \return
 *      - true on success
 *      - false of the feature is not supported
 */
bool halSpiFlashReadSFDP(const halSpiFlash_t *d, uint32_t address, void *data, uint32_t size);

/**
 * \brief read status register and check WIP bit
 *
 * \param d hal spi flash instance
 * \return
 *      - true if WIP is not set in status register
 *      - false WIP is set in status register
 */
bool halSpiFlashIsWipFinished(const halSpiFlash_t *d);

/**
 * \brief wait WIP bit in status register unset
 *
 * \param d hal spi flash instance
 */
void halSpiFlashWaitWipFinish(const halSpiFlash_t *d);

/**
 * \brief read security register
 *
 * Not all flash supports security register, and the valid \p num and
 * \p address is different even supported.
 *
 * The maximum \p size is 4. When more bytes are needed, caller should
 * use loop for multiple read.
 *
 * \param d hal spi flash instance
 * \param num securerity register number
 * \param address security register address
 * \param data pointer to output data
 * \param size read size
 * \return
 *      - true on success
 *      - false on failed, invalid parameter or not support
 */
bool halSpiFlashReadSecurityRegister(const halSpiFlash_t *d, uint8_t num, uint16_t address, void *data, uint32_t size);

/**
 * \brief program security register
 *
 * Not all flash supports security register, and the valid \p num and
 * \p address is different even supported.
 *
 * \p data shouldn't be allocated in flash. \p size should be less than
 * (can't be equal to) hardware TX fifo size.
 *
 * \param d hal spi flash instance
 * \param num securerity register number
 * \param address security register address
 * \param data data to be programed
 * \param size program size
 * \return
 *      - true on success
 *      - false on failed, invalid parameter or not support
 */
bool halSpiFlashProgramSecurityRegister(const halSpiFlash_t *d, uint8_t num, uint16_t address, const void *data, uint32_t size);

/**
 * \brief erase security register
 *
 * Not all flash supports security register, and the valid \p num is
 * different even supported.
 *
 * \param d hal spi flash instance
 * \param num securerity register number
 * \return
 *      - true on success
 *      - false on failed, invalid parameter or not support
 */
bool halSpiFlashEraseSecurityRegister(const halSpiFlash_t *d, uint8_t num);

/**
 * \brief lock security register
 *
 * Not all flash supports security register, and the valid \p num is
 * different even supported.
 *
 * \param d hal spi flash instance
 * \param num securerity register number
 * \return
 *      - true on success
 *      - false on failed, invalid parameter or not support
 */
bool halSpiFlashLockSecurityRegister(const halSpiFlash_t *d, uint8_t num);

/**
 * \brief whether security register is locked
 *
 * Not all flash supports security register, and the valid \p num is
 * different even supported.
 *
 * \param d hal spi flash instance
 * \param num securerity register number
 * \return
 *      - true if \p num is valid and locked
 *      - false if not locked, or invalid parameter
 */
bool halSpiFlashIsSecurityRegisterLocked(const halSpiFlash_t *d, uint8_t num);

/**
 * \brief the actual write protected region
 *
 * Status register can't set write protection for arbitrary region. This returns
 * the real protected region.
 *
 * \param d hal spi flash instance
 * \param offset range start
 * \param size range size
 * \return real protected range
 */
osiUintRange_t halSpiFlashWpRange(const halSpiFlash_t *prop, uint32_t offset, uint32_t size);

/**
 * \brief handling before program/erase
 *
 * Check status register to ensure the range is not write protected, and
 * send write enable command.
 *
 * When volatile status register feature is not supported, it won't change
 * status register.
 *
 * \param d hal spi flash instance
 * \param offset range start to be program/erase
 * \param size range size to be program/erase
 */
void halSpiFlashPrepareEraseProgram(const halSpiFlash_t *prop, uint32_t offset, uint32_t size);

/**
 * \brief handling after program/erase is completed
 *
 * After program/erase is completed, status register will be set to
 * protect all.
 *
 * When volatile status register feature is not supported, it won't change
 * status register.
 *
 * \param d hal spi flash instance
 */
void halSpiFlashFinishEraseProgram(const halSpiFlash_t *d);

/**
 * \brief check status register
 *
 * Check status register:
 * - When there are suspend bits, reset
 * - QE should be 1
 * - reasonable WP setting
 *
 * \param d hal spi flash instance
 */
void halSpiFlashStatusCheck(const halSpiFlash_t *d);

/**
 * \brief initialize flash
 *
 * \p d->hwp should be valid flash controller base address. Inside, flash ID
 * will be read, and properties inside \p d will be set based on flash ID.
 *
 * If flash ID is unknown, it will panic inside.
 *
 * Also \p halSpiFlashStatusCheck will be called inside to ensure status
 * register is reasonable.
 *
 * It should be called before configure quad read in flash controller.
 *
 * \param d hal spi flash instance
 */
void halSpiFlashInit(halSpiFlash_t *d);

#ifdef __cplusplus
}
#endif
#endif
