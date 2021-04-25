/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <das/hardware.h>
#include <das/platform.h>

#ifndef DAS_PLATFORM_FREERTOS
#error "realtek only supported by FreeRTOS"
#else

#define DEV_ID_LEN      32
#define FW_VER          "LinkKit-2.0.0-20200602.1111"

extern int wifi_get_mac_address(char * mac);

size_t das_hal_firmware_version(char *buf, size_t size)
{
    char *version = FW_VER;
    size_t real_len = 0;

    real_len = strlen(version);
    real_len += 1;
    if (size < real_len) {
        DAS_LOG("short buffer %d : %d\n", size, real_len);
        return 0;
    }

    memcpy(buf, version, real_len);
    return real_len;
}

size_t das_hal_device_id(char *buf, size_t size)
{
    int ret;
    char device_id[DEV_ID_LEN];

    if (size <= 2 * DEV_ID_LEN) {
        DAS_LOG("short buffer %d : %d\n", size, 2 * DEV_ID_LEN);
        return 0;
    }

    ret = wifi_get_mac_address(device_id);
    if (ret) {
        DAS_LOG("get mac addr fail\n");
        return 0;
    }

    das_hex2string(buf, size, device_id, DEV_ID_LEN);
    DAS_LOG("device_id: %s, buf: %s\n", device_id, buf);

    return (2 * DEV_ID_LEN);
}

#if (DAS_SERVICE_ROM_ENABLED)
extern uint32_t __flash_text_start__;
extern uint32_t __flash_text_end__;

extern uint32_t __ram_image2_text_start__;
extern uint32_t __ram_image2_text_end__;

int das_hal_rom_info(das_rom_bank_t banks[DAS_ROM_BANK_NUMBER])
{
    banks[0].address = &__flash_text_start__;
    banks[0].size = &__flash_text_end__ - &__flash_text_start__;
    DAS_LOG("__flash_text_start__: 0x%08X, __flash_text_end__: 0x%08X, size: 0x%08X\n", 
        &__flash_text_start__, &__flash_text_end__, banks[0].size);

    banks[1].address = &__ram_image2_text_start__;
    banks[1].size = &__ram_image2_text_end__ - &__ram_image2_text_start__;

    DAS_LOG("__ram_image2_text_start__: 0x%08X, __ram_image2_text_end__: 0x%08X, size: 0x%08X\n", 
        &__ram_image2_text_start__, &__ram_image2_text_end__, banks[1].size);

    return DAS_ROM_BANK_NUMBER;
}
#endif // end DAS_SERVICE_ROM_ENABLED

#endif // DAS_PLATFORM_FREERTOS