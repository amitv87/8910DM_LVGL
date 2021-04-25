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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('A', 'P', 'P', 'L')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "app_loader.h"
#include "hal_config.h"
#include "osi_api.h"
#include "osi_log.h"
#include "calclib/crc32.h"
#include "vfs.h"
#include "drv_secure.h"
#include <string.h>
#include <stdio.h>

#define APP_IMAGE_MAGIC 0x41505032 // 'APP2'
#define APP_IMAGE_HEADER_SIZE (128)

#define APP_SECTION_MAX (6)
#define APP_SECTION_NONE 0
#define APP_SECTION_COPY 1
#define APP_SECTION_STUB 2
#define APP_SECTION_CLEAR 3
#define APP_SECTION_XIP 4

#define MAJOR(version) ((version) >> 16)
#define MINOR(version) ((version)&0xffff)

#if defined(CONFIG_APP_FLASHIMG_RAM_OFFSET) && defined(CONFIG_APP_FLASHIMG_RAM_SIZE)
#define FLASHIMG_RAM_START (CONFIG_RAM_PHY_ADDRESS + CONFIG_APP_FLASHIMG_RAM_OFFSET)
#define FLASHIMG_RAM_SIZE (CONFIG_APP_FLASHIMG_RAM_SIZE)
appImageHandler_t gAppImgFlash = {};
#else
#define FLASHIMG_RAM_START (0)
#define FLASHIMG_RAM_SIZE (0)
#endif

#if defined(CONFIG_APPIMG_FLASH_ADDRESS) && defined(CONFIG_APPIMG_FLASH_SIZE)
#define FLASHIMG_FLASH_START (CONFIG_APPIMG_FLASH_ADDRESS)
#define FLASHIMG_FLASH_SIZE (CONFIG_APPIMG_FLASH_SIZE)
appImageHandler_t gAppImgFile = {};
#else
#define FLASHIMG_FLASH_START (0)
#define FLASHIMG_FLASH_SIZE (0)
#endif

#if defined(CONFIG_APP_FILEIMG_RAM_OFFSET) && defined(CONFIG_APP_FILEIMG_RAM_SIZE)
#define FILEIMG_RAM_START (CONFIG_RAM_PHY_ADDRESS + CONFIG_APP_FILEIMG_RAM_OFFSET)
#define FILEIMG_RAM_SIZE (CONFIG_APP_FILEIMG_RAM_SIZE)
#else
#define FILEIMG_RAM_START (0)
#define FILEIMG_RAM_SIZE (0)
#endif

typedef struct
{
    uint32_t type;
    uint32_t offset;
    uint32_t size;
    uint32_t lma;
} appImageSection_t;

typedef struct
{
    uint32_t magic;              // 'APP2'
    uint32_t image_size;         // total size
    uint32_t image_crc;          // CRC, use 0 at calculation
    uint32_t stub_version;       // core stub version
    uint32_t enter_function;     // function address of 'app_enter'
    uint32_t exit_function;      // function address of 'app_exit'
    uint32_t get_param_function; // function address of 'app_get_param'
    uint32_t set_param_function; // function address of 'app_set_param'
    appImageSection_t sections[APP_SECTION_MAX];
} appImageHeader_t;

typedef struct
{
    uint32_t insn;
    uint32_t tag;
} stubInsn_t;

// These are implemented in core_export.o
extern unsigned gCoreExportVersion;
extern bool appImageLoadStub(const void *stub, void *dst, unsigned size);

bool appImageFromMem(const void *address, appImageHandler_t *handler)
{
#if (FLASHIMG_FLASH_SIZE == 0) || (FLASHIMG_RAM_SIZE == 0)
    return false;
#else
    OSI_LOGI(0, "apploader from mem 0x%x", address);

    if (handler == NULL)
        return false;

    if (!OSI_IS_ALIGNED(address, 4))
        return false;

    if (!OSI_IS_IN_REGION(uintptr_t, address, FLASHIMG_FLASH_START, FLASHIMG_FLASH_SIZE))
        return false;

    const appImageHeader_t *header = (const appImageHeader_t *)address;

    OSI_LOGI(0, "appimg magic/0x%x size/%d crc/0x%x enter/0x%x exit/0x%x stub/0x%x",
             header->magic, header->image_size, header->image_crc,
             header->enter_function, header->exit_function,
             header->stub_version);

    if (header->magic != APP_IMAGE_MAGIC || header->image_size <= sizeof(appImageHeader_t))
        return false;

    if (!OSI_REGION_INSIDE(uintptr_t, address, header->image_size,
                           FLASHIMG_FLASH_START, FLASHIMG_FLASH_SIZE))
        return false;

    if (MAJOR(header->stub_version) != MAJOR(gCoreExportVersion))
        return false;

    if (MINOR(header->stub_version) > MINOR(gCoreExportVersion))
        return false;

    if (header->enter_function == 0)
        return false;

    if (!OSI_IS_IN_REGION(uintptr_t, header->enter_function, address, header->image_size) &&
        !OSI_IS_IN_REGION(uintptr_t, header->enter_function, FLASHIMG_RAM_START, FLASHIMG_RAM_SIZE))
        return false;

    if (header->exit_function != 0 &&
        !OSI_IS_IN_REGION(uintptr_t, header->exit_function, address, header->image_size) &&
        !OSI_IS_IN_REGION(uintptr_t, header->exit_function, FLASHIMG_RAM_START, FLASHIMG_RAM_SIZE))
        return false;

    if (header->get_param_function != 0 &&
        !OSI_IS_IN_REGION(uintptr_t, header->get_param_function, address, header->image_size) &&
        !OSI_IS_IN_REGION(uintptr_t, header->get_param_function, FLASHIMG_RAM_START, FLASHIMG_RAM_SIZE))
        return false;

    if (header->set_param_function != 0 &&
        !OSI_IS_IN_REGION(uintptr_t, header->set_param_function, address, header->image_size) &&
        !OSI_IS_IN_REGION(uintptr_t, header->set_param_function, FLASHIMG_RAM_START, FLASHIMG_RAM_SIZE))
        return false;

#if defined(CONFIG_SOC_8910) && defined(CONFIG_APPIMG_SIGCHECK_ENABLE)
    if (drvSecureBootEnable())
    {
        OSI_LOGI(0, "apploader check signature");

        unsigned size_with_sig = header->image_size + DRV_SECURE_BOOT_SIG_SIZE_8910;
        if (!drvSecureEmbedSigCheck(header, size_with_sig))
        {
            OSI_LOGE(0, "appimg signature check failed");
            return false;
        }
    }
#endif

    unsigned zero = 0;
    unsigned crc = crc32Init();
    crc = crc32Update(crc, header, 8);
    crc = crc32Update(crc, &zero, 4);
    crc = crc32Update(crc, &header->image_crc + 1, header->image_size - 12);
    if (crc != header->image_crc)
    {
        OSI_LOGE(0, "invalid appimg crc/0x%x calc/0x%x",
                 header->image_crc, crc);
        return false;
    }

    for (int n = 0; n < APP_SECTION_MAX; n++)
    {
        const appImageSection_t *sect = &header->sections[n];

        OSI_LOGI(0, "appimg section type/%d offset/%d size/%d address/0x%x",
                 sect->type, sect->offset, sect->size, sect->lma);

        if (sect->size == 0 || sect->type == APP_SECTION_NONE)
            continue;

        switch (sect->type)
        {
        case APP_SECTION_XIP:
            // check XIP section inside image
            if (sect->offset + sect->size > header->image_size)
                return false;
            break;

        case APP_SECTION_COPY:
            // check copy section destination inside reserved range
            if (!OSI_REGION_INSIDE(uintptr_t, sect->lma, sect->size,
                                   FLASHIMG_RAM_START, FLASHIMG_RAM_SIZE))
                return false;

            // check copy section source inside image
            if (sect->offset + sect->size > header->image_size)
                return false;

            memcpy((void *)sect->lma,
                   (const char *)address + sect->offset,
                   sect->size);
            break;

        case APP_SECTION_CLEAR:
            // check clear section destination inside reserved range
            if (!OSI_REGION_INSIDE(uintptr_t, sect->lma, sect->size,
                                   FLASHIMG_RAM_START, FLASHIMG_RAM_SIZE))
                return false;

            memset((void *)sect->lma, 0, sect->size);
            break;

        case APP_SECTION_STUB:
            // check stub section destination inside reserved range
            if (!OSI_REGION_INSIDE(uintptr_t, sect->lma, sect->size,
                                   FLASHIMG_RAM_START, FLASHIMG_RAM_SIZE))
                return false;

            // check stub section source inside image
            if (sect->offset + sect->size > header->image_size)
                return false;

            if (!appImageLoadStub((const char *)address + sect->offset,
                                  (void *)sect->lma, sect->size))
                return false;
            break;

        default:
            return false;
        }
    }

    handler->enter = (appImageEnter_t)header->enter_function;
    handler->exit = (appImageExit_t)header->exit_function;
    handler->get_param = (appImageGetParam_t)header->get_param_function;
    handler->set_param = (appImageSetParam_t)header->set_param_function;
    OSI_LOGI(0, "apploader done");
    osiICacheSyncAll();
    return true;
#endif
}

bool appImageFromFile(const char *fname, appImageHandler_t *handler)
{
#if (FILEIMG_RAM_SIZE == 0)
    return false;
#else
    OSI_LOGXI(OSI_LOGPAR_S, 0, "apploader from file %s", fname);

    if (fname == NULL || handler == NULL)
        return false;

    int file_size = vfs_file_size(fname);
    if (file_size <= sizeof(appImageHeader_t))
        return false;

    if (file_size > FILEIMG_RAM_SIZE)
        return false;

    appImageHeader_t header; // 128 bytes in stack

    int fd = vfs_open(fname, O_RDONLY);
    if (fd < 0)
        return false;

    if (vfs_read(fd, &header, sizeof(header)) != sizeof(header))
        goto close_fail;

    OSI_LOGI(0, "appimg magic/0x%x size/%d crc/0x%x enter/0x%x exit/0x%x stub/0x%x",
             header.magic, header.image_size, header.image_crc,
             header.enter_function, header.exit_function,
             header.stub_version);

    if (header.magic != APP_IMAGE_MAGIC || header.image_size > file_size)
        goto close_fail;

    if (MAJOR(header.stub_version) != MAJOR(gCoreExportVersion))
        goto close_fail;

    if (MINOR(header.stub_version) > MINOR(gCoreExportVersion))
        goto close_fail;

    if (header.enter_function == 0)
        goto close_fail;

    if (!OSI_IS_IN_REGION(uintptr_t, header.enter_function, FILEIMG_RAM_START, FILEIMG_RAM_SIZE))
        goto close_fail;

    if (header.exit_function != 0 &&
        !OSI_IS_IN_REGION(uintptr_t, header.exit_function, FILEIMG_RAM_START, FILEIMG_RAM_SIZE))
        goto close_fail;

    if (header.get_param_function != 0 &&
        !OSI_IS_IN_REGION(uintptr_t, header.get_param_function, FILEIMG_RAM_START, FILEIMG_RAM_SIZE))
        goto close_fail;

    if (header.set_param_function != 0 &&
        !OSI_IS_IN_REGION(uintptr_t, header.set_param_function, FILEIMG_RAM_START, FILEIMG_RAM_SIZE))
        goto close_fail;

    unsigned zero = 0;
    unsigned crc = crc32Init();
    crc = crc32Update(crc, &header, 8);
    crc = crc32Update(crc, &zero, 4);
    crc = crc32Update(crc, &header.image_crc + 1, sizeof(header) - 12);

    // reuse header memory for file CRC
    void *pbuf = &header;
    int remain_size = header.image_size - sizeof(header);
    while (remain_size > 0)
    {
        int rsize = OSI_MIN(int, remain_size, APP_IMAGE_HEADER_SIZE);
        if (vfs_read(fd, pbuf, rsize) != rsize)
            goto close_fail;

        crc = crc32Update(crc, pbuf, rsize);
        remain_size -= rsize;
    }

#if defined(CONFIG_SOC_8910) && defined(CONFIG_APPIMG_SIGCHECK_ENABLE)
    if (drvSecureBootEnable())
    {
        OSI_LOGI(0, "apploader check signature");

        if (!drvSecureFileEmbedSigCheck(fname))
        {
            OSI_LOGE(0, "appimg file signature check failed");
            goto close_fail;
        }
    }
#endif

    // header is reused, it is needed to read again.
    vfs_lseek(fd, 0, SEEK_SET);
    if (vfs_read(fd, &header, sizeof(header)) != sizeof(header))
        goto close_fail;

    if (crc != header.image_crc)
    {
        OSI_LOGE(0, "invalid appimg crc/0x%x calc/0x%x",
                 header.image_crc, crc);
        goto close_fail;
    }

    for (int n = 0; n < APP_SECTION_MAX; n++)
    {
        const appImageSection_t *sect = &header.sections[n];

        OSI_LOGI(0, "appimg section type/%d offset/%d size/%d address/0x%x",
                 sect->type, sect->offset, sect->size, sect->lma);

        if (sect->size == 0 || sect->type == APP_SECTION_NONE)
            continue;

        switch (sect->type)
        {
        case APP_SECTION_COPY:
            // check copy section destination inside reserved range
            if (!OSI_REGION_INSIDE(uintptr_t, sect->lma, sect->size,
                                   FILEIMG_RAM_START, FILEIMG_RAM_SIZE))
                goto close_fail;

            // check copy section source inside image
            if (sect->offset + sect->size > header.image_size)
                goto close_fail;

            vfs_lseek(fd, sect->offset, SEEK_SET);
            if (vfs_read(fd, (void *)sect->lma, sect->size) != sect->size)
                goto close_fail;

            break;

        case APP_SECTION_CLEAR:
            // check clear section destination inside reserved range
            if (!OSI_REGION_INSIDE(uintptr_t, sect->lma, sect->size,
                                   FILEIMG_RAM_START, FILEIMG_RAM_SIZE))
                goto close_fail;

            memset((void *)sect->lma, 0, sect->size);
            break;

        case APP_SECTION_STUB:
            // check stub section destination inside reserved range
            if (!OSI_REGION_INSIDE(uintptr_t, sect->lma, sect->size,
                                   FILEIMG_RAM_START, FILEIMG_RAM_SIZE))
                goto close_fail;

            // check stub section source inside image
            if (sect->offset + sect->size > header.image_size)
                goto close_fail;

            vfs_lseek(fd, sect->offset, SEEK_SET);
            if (vfs_read(fd, (void *)sect->lma, sect->size) != sect->size)
                goto close_fail;

            if (!appImageLoadStub((const void *)sect->lma,
                                  (void *)sect->lma, sect->size))
                goto close_fail;
            break;

        default:
            goto close_fail;
        }
    }

    vfs_close(fd);
    handler->enter = (appImageEnter_t)header.enter_function;
    handler->exit = (appImageExit_t)header.exit_function;
    handler->get_param = (appImageGetParam_t)header.get_param_function;
    handler->set_param = (appImageSetParam_t)header.set_param_function;
    OSI_LOGI(0, "apploader done");
    osiICacheSyncAll();
    return true;

close_fail:
    vfs_close(fd);
    return false;
#endif
}
