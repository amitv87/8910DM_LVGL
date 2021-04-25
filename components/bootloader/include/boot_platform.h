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

#ifndef _BOOT_PLATFORM_H_
#define _BOOT_PLATFORM_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "boot_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bootResetMode
{
    BOOT_RESET_NORMAL,         ///< normal reset
    BOOT_RESET_FORCE_DOWNLOAD, ///< reset to force download mode
} bootResetMode_t;

typedef enum
{
    BOOT_DOWNLOAD_UART1 = 0x01,
    BOOT_DOWNLOAD_UART2 = 0x02,
    BOOT_DOWNLOAD_UART3 = 0x03,
    BOOT_DOWNLOAD_USERIAL = 0x08,
} bootDownloadMode_t;

#define BOOT_DNLD_FROM_UART(mode) ((mode) >= BOOT_DOWNLOAD_UART1 && (mode) <= BOOT_DOWNLOAD_UART3)
#define BOOT_DNLD_FROM_USERIAL(mode) ((mode) == BOOT_DOWNLOAD_USERIAL)

/**
 * \brief image entry prototype
 */
typedef void (*bootJumpFunc_t)(uint32_t param);

/**
 * \brief reset the chip
 *
 * \param mode  reboot mode
 */
void bootReset(bootResetMode_t mode);

/**
 * \brief power off the chip
 */
void bootPowerOff(void);

/**
 * \brief check if the chip is boot from psm
 *
 * \return
 *      - true if from psm else false
 */
bool bootIsFromPsmSleep(void);
int bootPowerOnCause(void);

/**
 * \brief force update current version if secure boot enabled
 *
 * \param version   the version
 * \return
 *      - true on succeed else false
 */
bool bootUpdateVersion(uint32_t version);

/**
 * \brief check if secure boot enabled
 *
 * \return
 *      - true if ebable secure boot else false
 */
bool bootSecureBootEnable(void);

/**
 * \brief if second bootloader enabled, return the offset from the first
 *        bootloader start address in bytes
 *
 * \return
 *      - 0     the second bootloader not support
 *      - other the second bootloader start address from the first one.
 *              (first bootloader address + this offset = second bootloader address)
 */
uint32_t bootSecondOffsetBytes(void);

/**
 * \brief setup med configuration
 */
void bootMedConfig(void);

/**
 * \brief Enable the funtion pin reset.
 */
void bootResetPinEnable(void);

/**
 * \brief jump to image entry
 *
 * There are some housekeeping before jump. For 8910, D-cache cleanup
 * should be called outside.
 *
 * \param param jump entry parameter
 * \param entry jump entry
 */
void bootJumpImageEntry(unsigned param, bootJumpFunc_t entry);

/**
 * \brief initialize trace
 *
 * \param enabled whether trace is enabled
 */
void bootTraceInit(bool enabled);

#ifdef __cplusplus
}
#endif
#endif
