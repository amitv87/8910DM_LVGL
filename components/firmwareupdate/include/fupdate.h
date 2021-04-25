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

#ifndef _FUPDATE_H_
#define _FUPDATE_H_

#include "osi_compiler.h"
#include "fupdate_config.h"

OSI_EXTERN_C_BEGIN

// The following APIs are called in application

/**
 * \brief firmware update pack file name
 *
 * The default value is \p FUPDATE_PACK_FILE_NAME. And it can be defined
 * as other value to override the default value. When overrided, it
 * should be the same as bootloader.
 */
extern const char *gFupdatePackFileName;

/**
 * \brief firmware update stage file name
 *
 * The default value is \p FUPDATE_STAGE_FILE_NAME. And it can be defined
 * as other value to override the default value. When overrided, it
 * should be the same as bootloader.
 */
extern const char *gFupdateStageFileName;

/**
 * \brief firmware update temporal file name
 *
 * The default value is \p FUPDATE_TEMP_FILE_NAME. And it can be defined
 * as other value to override the default value. When overrided, it
 * should be the same as bootloader.
 */
extern const char *gFupdateTempFileName;

/**
 * \brief firmware update status
 */
typedef enum fupdateStatus
{
    FUPDATE_STATUS_NOT_READY, ///< not ready
    FUPDATE_STATUS_READY,     ///< ready
    FUPDATE_STATUS_FINISHED,  ///< update finished
} fupdateStatus_t;

/**
 * \brief progress callback function type
 *
 * During firmware update in bootloader, this callback will be called
 * after each blocked is processed.
 *
 * \param block_count   total block count
 * \param block         current finished block
 */
typedef void (*fupdateProgressCallback_t)(int block_count, int block);

/**
 * \brief get firmware update status
 *
 * The status is just get from update status file. And it won't perform
 * thorough update pack file content check.
 *
 * \return      update status
 */
fupdateStatus_t fupdateGetStatus(void);

/**
 * \brief get firmware update version information
 *
 * The old version and new version are embedded inside update pack.
 *
 * The version strings are specified at update pack creation. It is
 * suggestion just use them as trace or report information.
 *
 * \p old_version and \p new_version are allocated inside. Caller should
 * free them after used. When there are no version string inside update
 * pack, the returned pointer will be NULL.
 *
 * \param old_version   return pointer for old version
 * \param new_version   return pointer for new version
 * \return
 *      - true on success
 *      - false on error
 *          - out of memory, or invalid parameter
 *          - status is \p FUPDATE_STATUS_NONE
 */
bool fupdateGetVersion(char **old_version, char **new_version);

/**
 * \brief invalidate firmware update
 *
 * Invalidate firmware update status. \p gFupdateStageFileName will be
 * removed, \p gFupdateTempFileName will be removed, and
 * \p gFupdatePackFileName can be removed optionally.
 *
 * After previous update is finished, it is recommended to call this,
 * and remove update pack file, to release spaces in file system.
 *
 * It will be ensured that \p fupdateGetStatus will return
 * \p FUPDATE_STATUS_NONE after this is called.
 *
 * \param removePack    true to remove update pack file
 */
void fupdateInvalidate(bool removePack);

/**
 * \brief check whether firmware update file is valid
 *
 * It will perform thorough update pack file content. So, it will take
 * times.
 *
 * When \p curr_version is not NULL, it will try to check current version
 * with old version string embedded in update pack. When old version
 * string in update pack is not NULL also, and \p curr_version doesn't
 * match old version in update pack, the update pack will be regarded as
 * invalid.
 *
 * It is optional to embed version string inside update pack.
 *
 * \param curr_version  current version string, NULL for not to check
 * \return
 *      - true if the update pack file content is valid
 *      - false if not
 */
bool fupdateIsPackValid(const char *curr_version);

/**
 * \brief set firmware update to ready status
 *
 * Inside this function, the update pack file will be thoroughly checked.
 *
 * When the update pack file is valid:
 * - \p gFupdateStageFileName will be updated to indicate ready status;
 * - \p gFupdateTempFileName will be removed;
 *
 * When the update pack file is invalid:
 * - \p gFupdateStageFileName will be removed;
 * - \p gFupdateTempFileName will be removed;
 *
 * It must be called after the update pack file is written and closed. If
 * the pack file is modified after \p fupdateSetReady is called,
 * bootloader will still try to update. Though bootloader can detected
 * update pack invalid and not start real update, it will waste boot
 * time.
 *
 * No matter whether \p fupdateIsPackValid is called, the update pack file
 * will be verified inside. So, it will take even longer.
 *
 * When \p curr_version is not NULL, it will try to check current version
 * with old version string embedded in update pack. When old version
 * string in update pack is not NULL also, and \p curr_version doesn't
 * match old version in update pack, the update pack will be regarded as
 * invalid.
 *
 * It is optional to embed version string inside update pack.
 *
 * \param curr_version  current version string, NULL for not to check
 * \return
 *      - true if update pack file is valid
 *      - false if not
 */
bool fupdateSetReady(const char *curr_version);

/**
 * \brief firmware update result
 */
typedef enum fupdateResult
{
    FUPDATE_RESULT_NOT_READY,   ///< not ready
    FUPDATE_RESULT_CANNT_START, ///< failed to start
    FUPDATE_RESULT_FAILED,      ///< started and failed
    FUPDATE_RESULT_FINISHED,    ///< finished
} fupdateResult_t;

/**
 * \brief perform firmware update
 *
 * It must be called in bootloader. Before \p fupdateRun is called, file
 * system should be initializaed.
 * - file system for \p CONFIG_FS_FOTA_DATA_DIR
 * - file system to be patched
 *
 * File system layout and mount points must be the same as application.
 *
 * \p FUPDATE_RESULT_NOT_READY means that \p gFupdateStageFileName doesn't
 * exists, or not indicates \p FUPDATE_STATUS_READY. Bootloader shall boot
 * application normally.
 *
 * \p FUPDATE_RESULT_CANNT_START means that \p gFupdateStageFileName
 * indicates \p FUPDATE_STATUS_READY, however the update pack is not
 * valid or not enough memory can be allocated for upgraded. It shouldn't
 * happen, but the current firmware isn't changed. Bootloader can boot
 * application normally. And it is suggested to notify application to
 * invalidate update status.
 *
 * \p FUPDATE_RESULT_FAILED means that current firmware is changed, and
 * encounter unrecoverable error. Typical case is that power supply is
 * unstable. Due to current firmware is changed, bootloader shall **NEVER**
 * boot application normally. It is suggested that bootloader shall reboot
 * and try again.
 *
 * \p FUPDATE_RESULT_FINISHED means that current firmware is successfully
 * updated. Bootloader can boot application normally, or even better reboot.
 * At reboot, \p fupdateRun will return \p FUPDATE_RESULT_NOT_READY.
 *
 * When \p progress callback is not NULL, it will be called after each block
 * is finished.
 *
 * \param progress      progress callback
 */
fupdateResult_t fupdateRun(fupdateProgressCallback_t progress);

OSI_EXTERN_C_END
#endif
