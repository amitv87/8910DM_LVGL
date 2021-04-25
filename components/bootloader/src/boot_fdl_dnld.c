/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('F', 'D', 'L', 'D')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "boot_fdl.h"
#include "boot_platform.h"
#include "boot_spi_flash.h"
#include "hal_chip.h"
#include "boot_secure.h"
#include "boot_bsl_cmd.h"
#include "boot_simage.h"
#include "hal_config.h"
#include "osi_log.h"
#include "osi_api.h"
#include "drv_names.h"
#include "fs_mount.h"
#include "vfs.h"
#include "nvm.h"
#include "calclib/crc32.h"
#include "cpio_parser.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "quec_cust_patch.h"

// Work memory size in context
#define WORKMEM_SIZE 0x20000

#ifdef __QUEC_OEM_VER_TY__
#define PACKAGE_FILE_MAX_SIZE 0x100000
#else
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GNSS
#define PACKAGE_FILE_MAX_SIZE 0x80000
#else
#define PACKAGE_FILE_MAX_SIZE 0x10000
#endif
#endif

// Logic addresses, which should match the value used in pac file
#define ERASE_RUNNING_NV_LOGIC_ADDRESS 0xFE000001
#define PHASECHECK_LOGIC_ADDRESS 0xFE000002
#define FIXNV_LOGIC_ADDRESS 0xFE000003
#define PRE_PACK_FILE_LOGIC_ADDRESS 0xFE000004
#define DEL_APPIMG_LOGIC_ADDRESS 0xFE000005
#define FMT_FLASH_LOGIC_ADDRESS 0xFE000006

#define FLASH_ERASE_ALL 0x00000000
#define FLASH_ERASE_ALL_SIZE 0xFFFFFFFF

#define FLASH_PAGE_SIZE (256)
#define FLASH_SECTOR_SIZE (0x1000)

#define FDL_SIZE_4K (0x1000)

// 8910 encrypt
#define BOOT_SIGN_SIZE (0xbd40)
#define ENCRYPT_OFF 0x1000
#define ENCRYPT_LEN 0x400

#define DELAY_POWEROFF_TIMEOUT (2000)

#define ERR_REP_RETURN(fdl, err) OSI_DO_WHILE0(fdlEngineSendRespNoData(fdl, err); return;)
#ifdef CONFIG_BOARD_WITH_EXT_FLASH
#define IS_FLASH_ADDRESS(address) (HAL_FLASH_DEVICE_NAME(address) == DRV_NAME_SPI_FLASH || HAL_FLASH_DEVICE_NAME(address) == DRV_NAME_SPI_FLASH_EXT)
#else
#define IS_FLASH_ADDRESS(address) (HAL_FLASH_DEVICE_NAME(address) == DRV_NAME_SPI_FLASH)
#endif

#ifdef QUEC_PATCH_FACTORY_UPDATE
bool g_upgrade_by_factory = false;
#endif

static_assert(WORKMEM_SIZE >= CONFIG_NVBIN_FIXED_SIZE, "WORKMEM_SIZE is too small");
static_assert(WORKMEM_SIZE >= sizeof(phaseCheckHead_t), "WORKMEM_SIZE is too small");

enum
{
    FS_PREPARE_WRITE_PHASECHEK,
    FS_PREPARE_WRITE_FIXEDNV,
    FS_PREPARE_WRITE_PREPACK,
    FS_PREPARE_READ_PHASECHECK,
    FS_PREPARE_READ_FIXEDNV,
    FS_PREPARE_READ_RUNNINGNV,
    FS_PREPARE_ERASE_RUNNINGNV,
    FS_PREPARE_ERASE_PHASECHECK,
    FS_PREPARE_ERASE_APPIMG_FILE,
    FS_PREPARE_BACKUP_PHASE_CHECK,
};

typedef struct
{
    cpioStream_t *cpio;
    fdlDataDnld_t dnld;
    bool running_cleared;
    bool phasecheck_backed;
    bool fixednv_backed;
    unsigned init_nvbin_size;
    unsigned init_nvbin_crc;
    bool secure_boot_enable;
    bool delay_poweroff;
    bool flash_write_whole;
    unsigned device_type;
    fdlChannel_t *channel;
    unsigned max_packet_len;
    osiElapsedTimer_t connect_timer;
    phaseCheckHead_t phasecheck_back;
    char fixednv_back[CONFIG_NVBIN_FIXED_SIZE];
    char work_mem[WORKMEM_SIZE];
    char *flash_mem;
} fdlContext_t;

/**
 * When needed, hacks for nv on factory partition
 */
static bool prvFactoryNvHack(fdlContext_t *d)
{
#ifdef CONFIG_SOC_8910
    // HACK: ResearchDownload needs that nvid 2 must exist, and
    // previously this nv isn't added in 8910. Due to this nv isn't actually
    // used. So, at the beginning of FDL2, this will be written to make
    // ResearchDownload happy.
    if (nvmReadItem(NVID_CALIB_PARAM, NULL, 0) <= 0)
    {
        // Refer to CMakeLists.txt about these 2 external symbols.
        extern const unsigned gDefCalibParam[];
        extern const unsigned gDefCalibParamSize[];
        nvmWriteItem(NVID_CALIB_PARAM, gDefCalibParam, (unsigned)gDefCalibParamSize);
    }

    // HACK: GSM calibration data size is changed from 7706 to 8192. The
    // size of existed GSM calibration data may be 7706. And it is needed
    // to pad the existed GSM calibration data to 8192, to make NVEditor
    // work, and make nv backup work.
    int calib_gsm_size = nvmReadFixedItem(0x26d, NULL, 0);
    if (calib_gsm_size > 0 && calib_gsm_size < 8192)
    {
        char *calib_gsm_bin = &d->work_mem[0];
        memset(calib_gsm_bin, 0, 8192);

        nvmReadFixedItem(0x26d, calib_gsm_bin, 8192);
        nvmWriteFixedItem(0x26d, calib_gsm_bin, 8192);
    }
#endif

    return true;
}

/**
 * Mount factory partition
 */
static bool prvMountFactory(fdlContext_t *d)
{
    if (!fsMountFsPath(CONFIG_FS_FACTORY_MOUNT_POINT, false))
        return false;

    return prvFactoryNvHack(d);
}

/**
 * Mount factory partition, and format on fail
 */
static bool prvMountFormatFactory(fdlContext_t *d)
{
    if (!fsMountFsPath(CONFIG_FS_FACTORY_MOUNT_POINT, true))
        return false;

    return prvFactoryNvHack(d);
}

/**
 * Mount modem partition
 */
static bool prvMountModem(fdlContext_t *d)
{
#ifdef CONFIG_FS_MODEM_MOUNT_POINT
    return fsMountFsPath(CONFIG_FS_MODEM_MOUNT_POINT, false);
#else
    return true;
#endif
}

/**
 * Mount sys partition
 */
static bool prvMountSys(fdlContext_t *d)
{
#ifdef CONFIG_FS_SYS_MOUNT_POINT
    return fsMountFsPath(CONFIG_FS_SYS_MOUNT_POINT, false);
#else
    return true;
#endif
}

/**
 * Prepare file system for various senario
 */
static bool prvFsPrepare(fdlContext_t *d, unsigned kind)
{
    switch (kind)
    {
    case FS_PREPARE_WRITE_PHASECHEK:
    case FS_PREPARE_WRITE_FIXEDNV:
    case FS_PREPARE_WRITE_PREPACK:
    case FS_PREPARE_ERASE_RUNNINGNV:
    case FS_PREPARE_ERASE_APPIMG_FILE:
        return prvMountFormatFactory(d) && fsMountWithFormatAll();

    case FS_PREPARE_READ_PHASECHECK:
    case FS_PREPARE_ERASE_PHASECHECK:
        return prvMountFactory(d);

    case FS_PREPARE_READ_FIXEDNV:
        prvMountModem(d); // ignore error
        return prvMountFactory(d);

    case FS_PREPARE_READ_RUNNINGNV:
        return prvMountSys(d) && prvMountFactory(d);

    default:
        return false;
    }
}

/**
 * Calculate nvbin size and CRC. It will reuse \p work_mem.
 */
static void prvInitNvBinSizeCrc(fdlContext_t *d)
{
    d->init_nvbin_size = 0;

    if (!prvFsPrepare(d, FS_PREPARE_READ_FIXEDNV))
        return;

    int nvbin_size = nvmReadFixedBin(NULL, 0);
    if (nvbin_size <= 0 || nvbin_size > CONFIG_NVBIN_FIXED_SIZE)
        return;

    char *nvbin = &d->work_mem[0];
    memset(nvbin, 0xff, CONFIG_NVBIN_FIXED_SIZE);
    nvmReadFixedBin(nvbin, CONFIG_NVBIN_FIXED_SIZE);
    d->init_nvbin_crc = crc32Calc(nvbin, CONFIG_NVBIN_FIXED_SIZE);
    d->init_nvbin_size = nvbin_size;
}

/**
 * Check whether running should be cleared. It will reuse \p work_mem.
 * It is only called after write fixednv.
 */
static void prvCheckClearRunning(fdlContext_t *d)
{
#ifdef CONFIG_SOC_8910
    if (d->running_cleared)
        return;

    if (!prvFsPrepare(d, FS_PREPARE_READ_FIXEDNV) ||
        !prvFsPrepare(d, FS_PREPARE_ERASE_RUNNINGNV))
        return;

    int nvbin_size = nvmReadFixedBin(NULL, 0);
    if (d->init_nvbin_size > 0 && nvbin_size == d->init_nvbin_size)
    {
        char *nvbin = &d->work_mem[0];
        memset(nvbin, 0xff, CONFIG_NVBIN_FIXED_SIZE);
        nvmReadFixedBin(nvbin, CONFIG_NVBIN_FIXED_SIZE);

        // do nothing if the nvbin CRC matches
        if (crc32Calc(nvbin, CONFIG_NVBIN_FIXED_SIZE) == d->init_nvbin_crc)
            return;
    }

    nvmClearRunning();
    d->running_cleared = true;
#endif
}

/**
 * BSL_CMD_CONNECT, the timestamp is recorded.
 */
static void prvConnect(fdlEngine_t *fdl, fdlContext_t *d)
{
    OSI_LOGI(0, "FDL: connect");

    osiElapsedTimerStart(&d->connect_timer);
    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
}

/**
 * Flash download start. The common information start/total/received has
 * already initialized. Return response error code.
 */
static int prvFlashStart(fdlContext_t *d)
{
    fdlDataDnld_t *dn = &d->dnld;

    // Though it doesn't look reasonable, unaligned start address is supported.
    // In this case, the write address is not the same as erase address.
    dn->write_address = dn->start_address;
    dn->erase_address = OSI_ALIGN_DOWN(dn->start_address, FLASH_SECTOR_SIZE);

    unsigned faddress = HAL_FLASH_OFFSET(dn->erase_address);
    unsigned fend = HAL_FLASH_OFFSET(OSI_ALIGN_UP(dn->start_address + dn->total_size, FLASH_SECTOR_SIZE));
    int esize = bootSpiFlashEraseNoWait(dn->flash, faddress, fend - faddress);
    OSI_LOGV(0, "FDL2 flash erase no wait 0x%x/0x%x/%d", faddress, fend - faddress, esize);

    if (esize < 0)
    {
        dn->flash_stage = FLASH_STAGE_NONE;
        OSI_LOGE(0, "FDL: flash start failed 0x%x/0x%x", dn->start_address, dn->total_size);
        return -BSL_REP_DOWN_DEST_ERROR;
    }

    dn->erase_address += esize;
    dn->flash_stage = FLASH_STAGE_ERASE;
    dn->data_verify = crc32Init();
    return 0;
}

/**
 * Flash polling, called in FDL polling, midst and end. Don't access
 * \p dnld.received_size, rather the parameter should be used.
 */
static void prvFlashPolling(fdlContext_t *d, unsigned received)
{
    fdlDataDnld_t *dn = &d->dnld;

    if (dn->flash_stage == FLASH_STAGE_NONE ||
        dn->flash_stage == FLASH_STAGE_FINISH)
        return;

    if (dn->flash_stage == FLASH_STAGE_ERASE)
    {
        if (!bootSpiFlashIsDone(dn->flash))
            return;

        dn->flash_stage = FLASH_STAGE_WAIT_DATA;
    }
    else if (dn->flash_stage == FLASH_STAGE_WAIT_DATA)
    {
        int avail = received - (dn->write_address - dn->start_address);
        if (avail < FLASH_PAGE_SIZE || dn->write_address >= dn->erase_address)
            return;

        unsigned waddress = HAL_FLASH_OFFSET(dn->write_address);
        void *wdata = &d->work_mem[dn->write_address % WORKMEM_SIZE];
        int wsize = bootSpiFlashWriteNoWait(dn->flash, waddress, wdata, FLASH_PAGE_SIZE);
        OSI_LOGV(0, "FDL2 flash write no wait 0x%x/0x%x", waddress, wsize);

        dn->write_address += wsize;
        dn->flash_stage = FLASH_STAGE_WRITE;
    }
    else if (dn->flash_stage == FLASH_STAGE_WRITE)
    {
        if (!bootSpiFlashIsDone(dn->flash))
            return;

        if (dn->write_address < dn->erase_address)
        {
            dn->flash_stage = FLASH_STAGE_WAIT_DATA;
            return;
        }

        unsigned faddress = HAL_FLASH_OFFSET(dn->erase_address);
        unsigned fend = HAL_FLASH_OFFSET(OSI_ALIGN_UP(dn->start_address + dn->total_size, FLASH_SECTOR_SIZE));
        if (faddress >= fend)
        {
            dn->flash_stage = FLASH_STAGE_FINISH;
            return;
        }

        int esize = bootSpiFlashEraseNoWait(dn->flash, faddress, fend - faddress);
        OSI_LOGV(0, "FDL2 flash erase no wait 0x%x/0x%x/%d", faddress, fend - faddress, esize);
        dn->erase_address += esize;
        dn->flash_stage = FLASH_STAGE_ERASE;
    }
}

/**
 * Flash midst
 */
static void prvFlashMidst(fdlContext_t *d, const void *data, unsigned size)
{
    fdlDataDnld_t *dn = &d->dnld;

    dn->data_verify = crc32Update(dn->data_verify, data, size);

    // work_mem is reused as ring buffer. The index is address rather than
    // size. So, page write can always use linear buffer.
    unsigned mem_pos = (dn->start_address + dn->received_size) % WORKMEM_SIZE;
    unsigned tail = WORKMEM_SIZE - mem_pos;
    if (tail >= size)
    {
        memcpy(&d->work_mem[mem_pos], data, size);
    }
    else
    {
        memcpy(&d->work_mem[mem_pos], data, tail);
        memcpy(&d->work_mem[0], (const char *)data + tail, size - tail);
    }

    // Polling to make sure there are room for the next packet data.
    unsigned received = dn->received_size + size;
    unsigned received_address = dn->start_address + dn->received_size + size;
    while (received_address - dn->write_address > WORKMEM_SIZE - d->max_packet_len)
        prvFlashPolling(d, received);
}

/**
 * Flash end, write the remaining data and check crc.
 */
static int prvFlashEnd(fdlContext_t *d)
{
    fdlDataDnld_t *dn = &d->dnld;

    // size may be unaligned. So, polling to make sure the remaining data
    // less than a page.
    unsigned received_address = dn->start_address + dn->received_size;
    while (dn->write_address + FLASH_PAGE_SIZE <= received_address)
        prvFlashPolling(d, dn->received_size);

    // Write the remaining data, if exists.
    bootSpiFlashWaitDone(dn->flash);
    int remained = received_address - dn->write_address;
    if (remained > 0)
    {
        // It is possible that the remaining are located at unerased sector
        unsigned faddress = HAL_FLASH_OFFSET(dn->erase_address);
        unsigned fend = HAL_FLASH_OFFSET(OSI_ALIGN_UP(dn->start_address + dn->total_size, FLASH_SECTOR_SIZE));
        if (faddress < fend)
        {
            int esize = bootSpiFlashErase(dn->flash, faddress, fend - faddress);
            OSI_LOGV(0, "FDL2 flash erase 0x%x/0x%x/%d", faddress, fend - faddress, esize);
            dn->erase_address += esize;
        }

        unsigned waddress = HAL_FLASH_OFFSET(dn->write_address);
        void *wdata = &d->work_mem[dn->write_address % WORKMEM_SIZE];
        bootSpiFlashWrite(dn->flash, waddress, wdata, remained);
        OSI_LOGV(0, "FDL2 flash write 0x%x/0x%x", waddress, remained);
    }

    unsigned fstart = HAL_FLASH_OFFSET(dn->start_address);
    const void *fptr = bootSpiFlashMapAddress(dn->flash, fstart);
    unsigned fverify = crc32Calc(fptr, dn->total_size);

    dn->flash_stage = FLASH_STAGE_NONE;
    dn->flash = NULL;

    if (fverify != dn->data_verify)
    {
        OSI_LOGE(0, "FDL2 flash crc error, start/0x%x size/0x%x crc/0x%x calc/0x%x",
                 dn->start_address, dn->total_size, dn->data_verify, fverify);
        return -BSL_REP_VERIFY_ERROR;
    }
    return 0;
}

/**
 * Write flash after all data received.
 */
static int prvFlashWhole(fdlContext_t *d)
{
    unsigned start_addr = d->dnld.start_address;

    if (d->secure_boot_enable)
    {
#ifdef CONFIG_SOC_8910
        bool sigcheck = false;
        if (start_addr == CONFIG_BOOT_FLASH_ADDRESS)
            sigcheck = true;
        if (start_addr == CONFIG_APP_FLASH_ADDRESS)
            sigcheck = true;
#ifdef CONFIG_FDL_FLASH_SIGCHECK_BEFORE_WRITE
        if (start_addr == CONFIG_APPIMG_FLASH_ADDRESS)
            sigcheck = true;
#endif

        if (sigcheck)
        {
            // It is magic. Boot check size is fixed, and others has 512 is the cert
            // at the end and should be excluded at check.
            unsigned check_size = (start_addr == CONFIG_BOOT_FLASH_ADDRESS)
                                      ? BOOT_SIGN_SIZE
                                      : d->dnld.total_size - 512;

            OSI_LOGI(0, "FDL: image verify 0x%x size %d", (void *)start_addr, check_size);
            if (!bootSecureEmbeddedSigCheck(d->flash_mem, check_size))
            {
                OSI_LOGE(0, "FDL: bootloader secure boot verify fail");
                return -BSL_REP_VERIFY_ERROR;
            }

            if (start_addr == CONFIG_BOOT_FLASH_ADDRESS)
            {
                char *pbuf = d->flash_mem + ENCRYPT_OFF;
                bootAntiCloneEncryption(pbuf, ENCRYPT_LEN);
            }
        }
#endif

#ifdef CONFIG_SOC_8811
        bool sigcheck = false;
        if (start_addr == CONFIG_BOOT1_FLASH_ADDRESS ||
            start_addr == CONFIG_BOOT2_FLASH_ADDRESS)
            sigcheck = true;

        if (sigcheck)
        {
            OSI_LOGI(0, "FDL: image verify 0x%x size %d", (void *)start_addr, d->dnld.total_size);

            const simageHeader_t *header = (const simageHeader_t *)d->flash_mem;
            if (!bootSimageCheckSign(header))
            {
                OSI_LOGE(0, "FDL: secure boot verify fail");
                return -BSL_REP_OPERATION_FAILED;
            }

            uint32_t version = simageSecurityVersion(header);
            if (!bootUpdateVersion(version))
            {
                OSI_LOGE(0, "FDL: update security version fail %d", version);
                return -BSL_REP_OPERATION_FAILED;
            }
        }
#endif
    }

    unsigned end_addr = start_addr + d->dnld.total_size;
    unsigned erase_offset = OSI_ALIGN_DOWN(HAL_FLASH_OFFSET(start_addr), FLASH_SECTOR_SIZE);
    unsigned erase_size = OSI_ALIGN_UP(HAL_FLASH_OFFSET(end_addr) - erase_offset, FLASH_SECTOR_SIZE);
    unsigned write_offset = HAL_FLASH_OFFSET(start_addr);
    unsigned write_size = d->dnld.total_size;

    if (!bootSpiFlashErase(d->dnld.flash, erase_offset, erase_size))
        return -BSL_REP_VERIFY_ERROR;
    if (!bootSpiFlashWrite(d->dnld.flash, write_offset, d->flash_mem, write_size))
        return -BSL_REP_VERIFY_ERROR;

    // For 8910, modem won't be written as a whole. So, there are no
    // processing after flash is written.
    return 0;
}

/**
 * Post check after progressive flash write.
 */
static int prvFlashPostCheck(fdlContext_t *d)
{
    unsigned start_addr = d->dnld.start_address;
    (void)start_addr;

#ifdef CONFIG_SOC_8910
    if (start_addr == CONFIG_FS_MODEM_FLASH_ADDRESS)
    {
        // Modem file system is written by image. So, the block device and
        // file system is changed. And it is needed to umount/mount the
        // block device and file system. The easiest method is to
        // umount/mount all. It will waste some time, and the dynamic memory
        // may be considered in memory tight system.
        fsUmountAll();
        return 0;
    }

    // The followings are only for secure boot
    if (!d->secure_boot_enable)
        return 0;

    if (start_addr == CONFIG_BOOT_FLASH_ADDRESS)
    {
        OSI_LOGI(0, "FDL: boot image verify 0x%x size %d", (void *)start_addr, d->dnld.total_size);
        if (!bootSecureEmbeddedSigCheck((void *)start_addr, BOOT_SIGN_SIZE))
        {
            OSI_LOGE(0, "FDL: bootloader secure boot verify fail");
            return -BSL_REP_VERIFY_ERROR;
        }

        // reuse the working memory
        char *pbuf = &d->work_mem[0];
        memcpy(pbuf, (void *)(start_addr + ENCRYPT_OFF), FDL_SIZE_4K);
        bootAntiCloneEncryption(pbuf, ENCRYPT_LEN);

        bootSpiFlash_t *flash = bootSpiFlashOpen(DRV_NAME_SPI_FLASH);
        unsigned flash_offset = HAL_FLASH_OFFSET(start_addr);
        if (!bootSpiFlashErase(flash, flash_offset + ENCRYPT_OFF, FDL_SIZE_4K))
            return -BSL_REP_VERIFY_ERROR;
        if (!bootSpiFlashWrite(flash, flash_offset + ENCRYPT_OFF, pbuf, FDL_SIZE_4K))
            return -BSL_REP_VERIFY_ERROR;

        return 0;
    }

    if (start_addr == CONFIG_APP_FLASH_ADDRESS)
    {
        OSI_LOGI(0, "FDL: AP image verify 0x%x size %d", (void *)start_addr, d->dnld.total_size);

        // It is magic. 512 is the cert at the end and should be excluded at check.
        if (!bootSecureEmbeddedSigCheck((void *)start_addr, d->dnld.total_size - 512))
        {
            OSI_LOGE(0, "FDL: bootloader secure boot verify fail");
            return -BSL_REP_VERIFY_ERROR;
        }

        return 0;
    }

#ifdef CONFIG_APPIMG_SIGCHECK_ENABLE
    if (start_addr == CONFIG_APPIMG_FLASH_ADDRESS)
    {
        OSI_LOGI(0, "FDL: appimg verify 0x%x size %d", (void *)start_addr, d->dnld.total_size);

        // It is magic. 512 is the cert at the end and should be excluded at check.
        if (!bootSecureEmbeddedSigCheck((void *)start_addr, d->dnld.total_size - 512))
        {
            OSI_LOGE(0, "FDL: bootloader secure boot verify fail");
            return -BSL_REP_VERIFY_ERROR;
        }

        return 0;
    }
#endif
#endif

#ifdef CONFIG_SOC_8811
    // The followings are only for secure boot
    if (!d->secure_boot_enable)
        return 0;

    if (start_addr == CONFIG_BOOT1_FLASH_ADDRESS ||
        start_addr == CONFIG_BOOT2_FLASH_ADDRESS ||
        start_addr == CONFIG_APP_FLASH_ADDRESS)
    {
        OSI_LOGI(0, "FDL: image verify 0x%x size %d", (void *)start_addr, d->dnld.total_size);

        const simageHeader_t *header = (const simageHeader_t *)start_addr;
        if (!bootSimageCheckSign(header))
        {
            OSI_LOGE(0, "FDL: secure boot verify fail");
            return -BSL_REP_OPERATION_FAILED;
        }

        uint32_t version = simageSecurityVersion(header);
        if (!bootUpdateVersion(version))
        {
            OSI_LOGE(0, "FDL: update security version fail %d", version);
            return -BSL_REP_OPERATION_FAILED;
        }

        return 0;
    }
#endif

    return 0;
}

/**
 * Write one file from cpio.
 */
static void prvPackageFile(cpioFile_t *f)
{
    char name[VFS_PATH_MAX + 1] = "/";
    bool isdir = f->mode & S_IFDIR;
    memcpy(name + 1, f->name, strlen(f->name) + 1);

    OSI_LOGXD(OSI_LOGPAR_SII, 0, "package file %s (%d/%u)", name, isdir, f->data_size);
    if (isdir)
    {
        vfs_mkpath(name, 0);
    }
    else
    {
        vfs_mkfilepath(name, 0);
        vfs_file_write(name, f->data, f->data_size);
    }
}

/**
 * BSL_CMD_START_DATA
 * When file system access is needed, it should be chcked here.
 */
static void prvDataStart(fdlEngine_t *fdl, fdlPacket_t *pkt, fdlContext_t *d)
{
    int result = 0;
    uint32_t *ptr = (uint32_t *)pkt->content;
    uint32_t start_addr = OSI_FROM_BE32(*ptr++);
    uint32_t file_size = OSI_FROM_BE32(*ptr++);

    OSI_LOGI(0, "FDL: data start, start_addr/0x%x, size/0x%x", start_addr, file_size);

    d->dnld.start_address = start_addr;
    d->dnld.total_size = file_size;
    d->dnld.received_size = 0;

    if (start_addr == PHASECHECK_LOGIC_ADDRESS)
    {
#ifdef QUEC_PATCH_FACTORY_UPDATE
    	g_upgrade_by_factory = true;
#endif
        if (file_size != sizeof(phaseCheckHead_t))
        {
            OSI_LOGE(0, "FDL: invalid phasecheck data size %d", file_size);
            ERR_REP_RETURN(fdl, BSL_REP_DOWN_SIZE_ERROR);
        }

        if (!prvFsPrepare(d, FS_PREPARE_WRITE_PHASECHEK))
            ERR_REP_RETURN(fdl, BSL_REP_INCOMPATIBLE_PARTITION);
    }
    else if (start_addr == FIXNV_LOGIC_ADDRESS)
    {
        if (file_size > CONFIG_NVBIN_FIXED_SIZE)
        {
            OSI_LOGE(0, "FDL: invalid fixnv data size %d", file_size);
            ERR_REP_RETURN(fdl, BSL_REP_DOWN_SIZE_ERROR);
        }

        if (!prvFsPrepare(d, FS_PREPARE_WRITE_FIXEDNV))
            ERR_REP_RETURN(fdl, BSL_REP_INCOMPATIBLE_PARTITION);
    }
    else if (start_addr == PRE_PACK_FILE_LOGIC_ADDRESS)
    {
        if (!prvFsPrepare(d, FS_PREPARE_WRITE_PREPACK))
            ERR_REP_RETURN(fdl, BSL_REP_INCOMPATIBLE_PARTITION);

        cpioStreamCfg_t cfg = {.file_size_max = PACKAGE_FILE_MAX_SIZE, .file_path_max = VFS_PATH_MAX};
        d->cpio = cpioStreamCreate(&cfg);
        if (d->cpio == NULL)
            ERR_REP_RETURN(fdl, BSL_PHONE_NOT_ENOUGH_MEMORY);
    }
    else if (IS_FLASH_ADDRESS(start_addr))
    {
        bootSpiFlash_t *flash = bootSpiFlashOpen(HAL_FLASH_DEVICE_NAME(start_addr));
        if (flash == NULL)
            ERR_REP_RETURN(fdl, BSL_REP_OPERATION_FAILED);

        d->dnld.flash = flash;
        d->flash_write_whole = false;

#ifdef CONFIG_SOC_8910
        if (d->secure_boot_enable)
        {
            if (start_addr == CONFIG_BOOT_FLASH_ADDRESS)
                d->flash_write_whole = true;

#ifdef CONFIG_FDL_FLASH_SIGCHECK_BEFORE_WRITE
            if (start_addr == CONFIG_APP_FLASH_ADDRESS)
                d->flash_write_whole = true;
            if (start_addr == CONFIG_APPIMG_FLASH_ADDRESS)
                d->flash_write_whole = true;
#endif
        }
#endif

#ifdef CONFIG_SOC_8811
        if (d->secure_boot_enable)
        {
            if (start_addr == CONFIG_BOOT1_FLASH_ADDRESS ||
                start_addr == CONFIG_BOOT2_FLASH_ADDRESS)
                d->flash_write_whole = true;
        }
#endif

        if (d->flash_write_whole)
        {
            d->flash_mem = malloc(d->dnld.total_size);
            if (d->flash_mem == NULL)
                ERR_REP_RETURN(fdl, BSL_PHONE_NOT_ENOUGH_MEMORY);
        }
        else
        {
            if ((result = prvFlashStart(d)) < 0)
                ERR_REP_RETURN(fdl, -result);
        }
    }
    else
    {
        OSI_LOGE(0, "FDL: invalid data start address 0x%x", start_addr);
        fdlEngineSendRespNoData(fdl, BSL_REP_DOWN_DEST_ERROR);
        return;
    }

    d->dnld.stage = SYS_STAGE_START;
    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
}

/**
 * BSL_CMD_MIDST_DATA
 */
static void prvDataMidst(fdlEngine_t *fdl, fdlPacket_t *pkt, fdlContext_t *d)
{
    uint16_t data_len = pkt->size;
    uint32_t start_addr = d->dnld.start_address;

    OSI_LOGD(0, "FDL: data midst, start_addr/0x%x, size/0x%x, received/0x%x, len/0x%x, stage/0x%x",
             d->dnld.start_address, d->dnld.total_size, d->dnld.received_size,
             data_len, d->dnld.stage);

    if ((d->dnld.stage != SYS_STAGE_START) && (d->dnld.stage != SYS_STAGE_GATHER))
    {
        d->dnld.flash_stage = FLASH_STAGE_NONE;
        d->dnld.flash = NULL;
        ERR_REP_RETURN(fdl, BSL_REP_DOWN_NOT_START);
    }

    if (d->dnld.received_size + data_len > d->dnld.total_size)
    {
        d->dnld.flash_stage = FLASH_STAGE_NONE;
        d->dnld.flash = NULL;
        ERR_REP_RETURN(fdl, BSL_REP_DOWN_SIZE_ERROR);
    }

    if (start_addr == PRE_PACK_FILE_LOGIC_ADDRESS)
    {
#ifdef __QUEC_OEM_VER_TY__
        static bool isClear = false;
        if (isClear == false)
        {
            vfs_rmchildren(CONFIG_FS_SYS_MOUNT_POINT);
            isClear = true;
        }
#endif
        cpioStreamPushData(d->cpio, pkt->content, data_len);
        cpioFile_t *f;
        while ((f = cpioStreamPopFile(d->cpio)))
        {
#if defined(CONFIG_SOC_8910) && defined(CONFIG_APPIMG_LOAD_FILE) && \
    defined(CONFIG_APPIMG_LOAD_FILE_NAME) && defined(CONFIG_APPIMG_SIGCHECK_ENABLE)
            if (d->secure_boot_enable && strcmp(f->name, &CONFIG_APPIMG_LOAD_FILE_NAME[1]) == 0)
            {
                OSI_LOGI(0, "FDL: appimg file verify size %d", f->data_size);
                // It is magic. The length of bootSecureEmbeddedSigCheck is raw data plus sig.
                // However, there are 512 bytes cert in appimg.
                if (!bootSecureEmbeddedSigCheck(f->data, f->data_size - 512))
                {
                    OSI_LOGE(0, "FDL: bootloader secure boot verify fail");
                    ERR_REP_RETURN(fdl, BSL_REP_VERIFY_ERROR);
                }
            }

#endif
            prvPackageFile(f);
            cpioStreamDestroyFile(d->cpio, f);
        }
    }
    else if (IS_FLASH_ADDRESS(start_addr))
    {
        if (d->flash_write_whole)
            memcpy(&d->flash_mem[0] + d->dnld.received_size, pkt->content, data_len);
        else
            prvFlashMidst(d, pkt->content, data_len);
    }
    else if (d->dnld.received_size + data_len > WORKMEM_SIZE)
    {
        ERR_REP_RETURN(fdl, BSL_REP_DOWN_SIZE_ERROR);
    }
    else
    {
        memcpy(&d->work_mem[0] + d->dnld.received_size, pkt->content, data_len);
    }

    d->dnld.received_size += data_len;
    d->dnld.stage = SYS_STAGE_GATHER;
    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
}

/**
 * BSL_CMD_END_DATA
 */
static void prvDataEnd(fdlEngine_t *fdl, fdlContext_t *d)
{
    int result = 0;
    uint32_t start_addr = d->dnld.start_address;

    OSI_LOGI(0, "FDL: data end, start_addr/0x%x, size/0x%x, received/0x%x, stage/0x%x",
             d->dnld.start_address, d->dnld.total_size, d->dnld.received_size,
             d->dnld.stage);

    if ((d->dnld.stage != SYS_STAGE_START) && (d->dnld.stage != SYS_STAGE_GATHER))
    {
        d->dnld.flash_stage = FLASH_STAGE_NONE;
        d->dnld.flash = NULL;
        ERR_REP_RETURN(fdl, BSL_REP_DOWN_NOT_START);
    }

    if (d->dnld.received_size != d->dnld.total_size)
    {
        d->dnld.flash_stage = FLASH_STAGE_NONE;
        d->dnld.flash = NULL;
        ERR_REP_RETURN(fdl, BSL_REP_DOWN_SIZE_ERROR);
    }

    if (start_addr == PHASECHECK_LOGIC_ADDRESS)
    {
        phaseCheckHead_t *phase_check = (phaseCheckHead_t *)&d->work_mem[0];
        if (!nvmWritePhaseCheck(phase_check))
        {
            OSI_LOGE(0, "FDL: write phase check failed");
            ERR_REP_RETURN(fdl, BSL_REP_OPERATION_FAILED);
        }
    }
    else if (start_addr == FIXNV_LOGIC_ADDRESS)
    {
        char *nvbin = &d->work_mem[0];
#ifdef QUEC_PATCH_FACTORY_UPDATE
        if (nvmWriteFixedBin_v2(nvbin, d->dnld.received_size, g_upgrade_by_factory) < 0)
#else
        if (nvmWriteFixedBin(nvbin, d->dnld.received_size) < 0)
#endif
        {
            OSI_LOGE(0, "FDL: write nv bin failed");
            ERR_REP_RETURN(fdl, BSL_REP_OPERATION_FAILED);
        }

        if (d->phasecheck_backed)
        {
            d->phasecheck_backed = false;
            if (!nvmWritePhaseCheck(&d->phasecheck_back))
            {
                OSI_LOGE(0, "FDL: write phasecheck fail after erase all");
                ERR_REP_RETURN(fdl, BSL_REP_OPERATION_FAILED);
            }
        }

        d->fixednv_backed = false;
        prvCheckClearRunning(d);
    }
    else if (start_addr == PRE_PACK_FILE_LOGIC_ADDRESS)
    {
        cpioStreamDestroy(d->cpio);
        d->cpio = NULL;
    }
    else if (IS_FLASH_ADDRESS(start_addr))
    {
        if (d->flash_write_whole)
        {
            d->flash_write_whole = false;
            result = prvFlashWhole(d);
            free(d->flash_mem);
            d->flash_mem = NULL;

            if (result < 0)
                ERR_REP_RETURN(fdl, -result);
        }
        else
        {
            if ((result = prvFlashEnd(d)) < 0)
                ERR_REP_RETURN(fdl, -result);

            if ((result = prvFlashPostCheck(d)) < 0)
                ERR_REP_RETURN(fdl, -result);
        }
    }

    d->dnld.stage = SYS_STAGE_END;
    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
}

/**
 * BSL_CMD_NORMAL_RESET
 */
static void prvResetNormal(fdlEngine_t *fdl, fdlContext_t *d)
{
    OSI_LOGI(0, "FDL: reset to normal");

    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
    osiDelayUS(2000);
    bootReset(BOOT_RESET_NORMAL);
}

/**
 * Polling for delayed poweroff
 */
static void prvPowerOffPolling(fdlContext_t *d)
{
    if (!d->delay_poweroff)
        return;

    if (BOOT_DNLD_FROM_UART(d->device_type))
    {
        unsigned ms = osiElapsedTime(&d->connect_timer);
        if (ms > DELAY_POWEROFF_TIMEOUT)
        {
            OSI_LOGI(0, "FDL: poweroff timeout", ms);
            bootPowerOff();

            // ideally, we shouldn't come here
            OSI_LOGE(0, "FDL: poweroff failed");
            d->delay_poweroff = false;
        }
    }

    if (BOOT_DNLD_FROM_USERIAL(d->device_type))
    {
        if (!d->channel->connected(d->channel))
        {
            OSI_LOGI(0, "FDL: device not connected, poweroff");
            bootPowerOff();

            // ideally, we shouldn't come here
            OSI_LOGE(0, "FDL: poweroff failed");
            d->delay_poweroff = false;
        }
    }
}

/**
 * BSL_CMD_POWER_OFF
 */
static void prvPowerOff(fdlEngine_t *fdl, fdlContext_t *d)
{
    OSI_LOGI(0, "FDL: poweroff");

    d->delay_poweroff = true;
    osiElapsedTimerStart(&d->connect_timer);
    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
}

/**
 * BSL_CMD_READ_FLASH
 */
static void prvReadFlash(fdlEngine_t *fdl, fdlPacket_t *pkt, fdlContext_t *d)
{
    uint32_t *ptr = (uint32_t *)pkt->content;
    uint32_t addr = OSI_FROM_BE32(*ptr++);
    uint32_t size = OSI_FROM_BE32(*ptr++);
    uint32_t offset = OSI_FROM_BE32(*ptr++); // not all address has this parameter

    if (offset == 0)
        OSI_LOGI(0, "FDL: read flash, addr/0x%x, size/0x%x, offset/0x%x", addr, size, offset);
    else
        OSI_LOGD(0, "FDL: read flash, addr/0x%x, size/0x%x, offset/0x%x", addr, size, offset);

    if (addr == PHASECHECK_LOGIC_ADDRESS)
    {
        if (!prvFsPrepare(d, FS_PREPARE_READ_PHASECHECK))
            ERR_REP_RETURN(fdl, BSL_REP_INCOMPATIBLE_PARTITION);

        // reuse the working memory
        phaseCheckHead_t *phase_check = (phaseCheckHead_t *)&d->work_mem[0];
        memset(phase_check, 0, sizeof(phaseCheckHead_t));
        if (!nvmReadPhasecheck(phase_check))
        {
            OSI_LOGE(0, "FDL: read phasecheck failed");
            ERR_REP_RETURN(fdl, BSL_REP_OPERATION_FAILED);
        }

        fdlEngineSendRespData(fdl, BSL_REP_READ_FLASH, phase_check, sizeof(phaseCheckHead_t));
    }
    else if (addr == FIXNV_LOGIC_ADDRESS)
    {
        // Assuming the read will be started with offset 0, and continuous
        char *nvbin = &d->work_mem[0];
        if (offset == 0)
        {
            if (d->fixednv_backed)
            {
                d->fixednv_backed = false;
                memcpy(nvbin, d->fixednv_back, CONFIG_NVBIN_FIXED_SIZE);
            }
            else
            {
                if (!prvFsPrepare(d, FS_PREPARE_READ_FIXEDNV))
                    ERR_REP_RETURN(fdl, BSL_REP_INCOMPATIBLE_PARTITION);

                memset(nvbin, 0xff, CONFIG_NVBIN_FIXED_SIZE);
                if (nvmReadFixedBin(nvbin, CONFIG_NVBIN_FIXED_SIZE) < 0)
                    ERR_REP_RETURN(fdl, BSL_REP_OPERATION_FAILED);
            }
        }

        if (offset >= CONFIG_NVBIN_FIXED_SIZE)
            ERR_REP_RETURN(fdl, BSL_REP_INVALID_CMD);

        unsigned send_size = OSI_MIN(unsigned, CONFIG_NVBIN_FIXED_SIZE - offset, size);
        fdlEngineSendRespData(fdl, BSL_REP_READ_FLASH, nvbin + offset, send_size);
    }
    else if (IS_FLASH_ADDRESS(addr))
    {
        bootSpiFlash_t *flash = bootSpiFlashOpen(HAL_FLASH_DEVICE_NAME(addr));
        if (flash == NULL)
            ERR_REP_RETURN(fdl, BSL_REP_OPERATION_FAILED);

        // valid flash offset not includes the end address
        if (!bootSpiFlashMapAddressValid(flash, (void *)(addr + size - 1)))
            ERR_REP_RETURN(fdl, BSL_REP_INVALID_CMD);

        uint32_t flash_offset = HAL_FLASH_OFFSET(addr);
        const void *fdata = bootSpiFlashMapAddress(flash, flash_offset);
        fdlEngineSendRespData(fdl, BSL_REP_READ_FLASH, fdata, size);
    }
    else
    {
        fdlEngineSendRespNoData(fdl, BSL_REP_INVALID_CMD);
        return;
    }
}

/**
 * BSL_CMD_ERASE_FLASH
 */
static void prvEraseFlash(fdlEngine_t *fdl, fdlPacket_t *pkt, fdlContext_t *d)
{
    uint32_t *ptr = (uint32_t *)pkt->content;
    uint32_t addr = OSI_FROM_BE32(*ptr++);
    uint32_t size = OSI_FROM_BE32(*ptr++);

    OSI_LOGI(0, "FDL: erase flash, addr/0x%x, size/0x%x", addr, size);

    if (addr == ERASE_RUNNING_NV_LOGIC_ADDRESS)
    {
        if (!prvFsPrepare(d, FS_PREPARE_ERASE_RUNNINGNV))
            ERR_REP_RETURN(fdl, BSL_REP_INCOMPATIBLE_PARTITION);

        nvmClearRunning(); // ignore error
        d->running_cleared = true;
    }
    else if (addr == PHASECHECK_LOGIC_ADDRESS)
    {
        if (!prvFsPrepare(d, FS_PREPARE_ERASE_PHASECHECK))
            ERR_REP_RETURN(fdl, BSL_REP_INCOMPATIBLE_PARTITION);

        nvmClearPhaseCheck(); // ignore error
    }
    else if (addr == DEL_APPIMG_LOGIC_ADDRESS)
    {
#if defined(CONFIG_APPIMG_LOAD_FLASH) && defined(CONFIG_APPIMG_LOAD_FILE_NAME)
        if (!prvFsPrepare(d, FS_PREPARE_ERASE_APPIMG_FILE))
            ERR_REP_RETURN(fdl, BSL_REP_INCOMPATIBLE_PARTITION);

        vfs_unlink(CONFIG_APPIMG_LOAD_FILE_NAME); // ignore error
#endif
    }
    else if (addr == FMT_FLASH_LOGIC_ADDRESS)
    {
        // "size" is the flash block device name
        if (!fsMountFormatFlash(size))
            ERR_REP_RETURN(fdl, BSL_REP_OPERATION_FAILED);
    }
    else if (FLASH_ERASE_ALL == addr && FLASH_ERASE_ALL_SIZE == size)
    {
        if (prvFsPrepare(d, FS_PREPARE_READ_PHASECHECK) &&
            nvmReadPhasecheck(&d->phasecheck_back))
            d->phasecheck_backed = true;

        if (prvFsPrepare(d, FS_PREPARE_READ_FIXEDNV))
        {
            int nvsize = nvmReadFixedBin(d->fixednv_back, CONFIG_NVBIN_FIXED_SIZE);
            if (nvsize > 0 && nvsize <= CONFIG_NVBIN_FIXED_SIZE)
                d->fixednv_backed = true;
        }

        fsUmountAll();

        bootSpiFlash_t *flash = bootSpiFlashOpen(DRV_NAME_SPI_FLASH);
        if (flash == NULL)
            ERR_REP_RETURN(fdl, BSL_REP_OPERATION_FAILED);

        bootSpiFlashChipErase(flash);

#ifdef CONFIG_BOARD_WITH_EXT_FLASH
        bootSpiFlash_t *flash_ext = bootSpiFlashOpen(DRV_NAME_SPI_FLASH_EXT);
        if (flash_ext == NULL)
            ERR_REP_RETURN(fdl, BSL_REP_OPERATION_FAILED);

        bootSpiFlashChipErase(flash_ext);
#endif
    }
    else if (IS_FLASH_ADDRESS(addr))
    {
        bootSpiFlash_t *flash = bootSpiFlashOpen(HAL_FLASH_DEVICE_NAME(addr));
        uint32_t flash_offset = HAL_FLASH_OFFSET(addr);
        if (!bootSpiFlashErase(flash, flash_offset, size))
            ERR_REP_RETURN(fdl, BSL_REP_INVALID_CMD);
    }
    else
    {
        fdlEngineSendRespNoData(fdl, BSL_REP_INVALID_CMD);
        return;
    }

    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
}

/**
 * BSL_CMD_CHANGE_BAUD
 */
static void prvSetBaud(fdlEngine_t *fdl, fdlPacket_t *pkt, fdlContext_t *d)
{
    uint32_t *ptr = (uint32_t *)pkt->content;
    uint32_t baud = OSI_FROM_BE32(*ptr++);

    OSI_LOGD(0, "FDL: change baud %d", baud);

    // This is special, ACK must be sent in old baud rate.
    fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
    fdlEngineSetBaud(fdl, baud);
}

/**
 * Process packet, called by engine.
 */
static void prvProcessPkt(fdlEngine_t *fdl, fdlPacket_t *pkt, void *param)
{
    if (fdl == NULL || pkt == NULL)
        osiPanic();

    OSI_LOGV(0, "FDL: pkt type/0x%x, size/0x%x", pkt->type, pkt->size);

    fdlContext_t *d = (fdlContext_t *)param;
    switch (pkt->type)
    {
    case BSL_CMD_CONNECT:
        prvConnect(fdl, d);
        break;

    case BSL_CMD_START_DATA:
        prvDataStart(fdl, pkt, d);
        break;

    case BSL_CMD_MIDST_DATA:
        prvDataMidst(fdl, pkt, d);
        break;

    case BSL_CMD_END_DATA:
        prvDataEnd(fdl, d);
        break;

    case BSL_CMD_NORMAL_RESET:
        prvResetNormal(fdl, d);
        break;

    case BSL_CMD_READ_FLASH:
        prvReadFlash(fdl, pkt, d);
        break;

    case BSL_CMD_REPARTITION:
        OSI_LOGI(0, "FDL: repartition");
        fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
        break;

    case BSL_CMD_ERASE_FLASH:
        prvEraseFlash(fdl, pkt, d);
        break;

    case BSL_CMD_POWER_OFF:
        prvPowerOff(fdl, d);
        break;

    case BSL_CMD_CHANGE_BAUD:
        prvSetBaud(fdl, pkt, d);
        break;

    case BSL_CMD_ENABLE_DEBUG_MODE:
        fdlEngineSendRespNoData(fdl, BSL_REP_ACK);
        break;

    default:
        OSI_LOGE(0, "FDL: cmd not support yet 0x%x", pkt->type);
        fdlEngineSendRespNoData(fdl, BSL_REP_INVALID_CMD);
        break;
    }
}

/**
 * Polling. Called by engine
 */
static void prvPolling(fdlEngine_t *fdl, void *param)
{
    fdlContext_t *d = (fdlContext_t *)param;

    prvFlashPolling(d, d->dnld.received_size);
    prvPowerOffPolling(d);
}

/**
 * Start download
 */
bool fdlDnldStart(fdlEngine_t *fdl, unsigned devtype)
{
    fdlContext_t *d = calloc(1, sizeof(fdlContext_t));
    if (d == NULL)
        return false;

    halPmuExtFlashPowerOn();

    d->device_type = devtype;
    d->channel = fdlEngineGetChannel(fdl);
    d->max_packet_len = fdlEngineGetMaxPacketLen(fdl);
    d->secure_boot_enable = bootSecureBootEnable();

    fsMountSetScenario(FS_SCENRARIO_FDL);
    prvInitNvBinSizeCrc(d);

    if (prvFsPrepare(d, FS_PREPARE_READ_RUNNINGNV))
        nvmMigration();

    fdlEngineProcess(fdl, prvProcessPkt, prvPolling, d);
    return false;
}
