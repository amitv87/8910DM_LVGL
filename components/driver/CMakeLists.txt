# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

configure_file(include/drv_config.h.in ${out_inc_dir}/drv_config.h)

if(NOT ${CONFIG_KEYPAD_CSV_PATH} STREQUAL "")
    set(keypadcfg_h ${CMAKE_CURRENT_BINARY_DIR}/drv_keypad_def.h)
    set(csv_keypad ${SOURCE_TOP_DIR}/${CONFIG_KEYPAD_CSV_PATH})
    add_custom_command(OUTPUT ${keypadcfg_h}
        COMMAND python3 ${keypadgen_py} ${csv_keypad} ${keypadcfg_h}
        DEPENDS ${keypadgen_py} ${csv_keypad}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

set(target driver)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC ${dummy_c_file} ${keypadcfg_h})
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_DRIVER)
target_compile_options(${target} PRIVATE "-Wnull-dereference")
target_include_directories(${target} PUBLIC include src)#quec update
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} src)
target_include_targets(${target} PRIVATE hal kernel fs fsmount nanopb diag)
target_include_targets(${target} PRIVATE nvm net calclib apploader audio)

nanopbgen(src/rtc/drv_rtc.proto)

target_sources_if(CONFIG_SOC_8910 THEN ${target} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/drv_rtc.pb.c
    src/spi_flash/drv_spi_flash.c
    src/uart/drv_dma_uart.c
    src/rtc/drv_rtc.c
    src/rtc/drv_rtc_2720.c
    src/debugport/drv_debug_port.c
    src/debugport/drv_debughost.c
    src/debugport/drv_debug_dma_uart.c
    src/debugport/drv_debug_userial.c
    src/debugport/drv_modem_log_buf.c
    src/hostcmd/drv_host_cmd.c
    src/hostcmd/drv_host_cmd_rw.c
    src/hostcmd/drv_host_cmd_sys.c
    src/drv_rng.c
    src/drv_ifc.c
    src/drv_wdt.c
    src/drv_md_nvm.c
    src/drv_ipc_at.c
    src/drv_axidma.c
    src/drv_pmic_intr.c
    src/drv_charger.c
    src/drv_gpio_8910.c
    src/drv_gpo_8910.c
    src/drv_serial.c
    src/drv_camera.c
    src/image_sensor/camera_gc032a.c
    src/image_sensor/camera_gc0310.c
    src/image_sensor/camera_bf30a2.c
    src/image_sensor/image_sensor.c
    src/drv_keypad.c
    src/drv_powerkey_8910.c
    src/drv_adc.c
    src/drv_i2c.c
    src/drv_spi.c
    src/drv_pwm.c
    src/drv_backlight.c
    src/drv_wcn.c
    src/drv_efuse.c
    src/drv_efuse_pmic.c
    src/drv_aes.c
    src/drv_secure.c
    src/drv_sdmmc.c
    src/drv_headset.c
    src/drv_pmic_codec.c
    src/drv_usb.cpp
    src/usb/usb_composite_device.c
    src/usb/usb_serial.c
    src/usb/usb_ether.c
    src/usb/f_serial.c
    src/usb/f_acm.c
    src/usb/f_rndis.c
    src/usb/f_ecm.c
    src/usb/f_ccid.c
    src/usb/rndis_data.c
    src/usb/rndis.c
    src/usb/ecm_data.c
    src/usb/udc.c
    src/usb/udc_dwc.c
    src/usb/udc_platform_8910.c
)

target_sources_if(CONFIG_WCN_WIFI_SCAN_SUPPORT THEN ${target} PRIVATE src/drv_wcn_wifi.c)

# quec add st7789v
target_sources_if(CONFIG_LCD_SUPPORT THEN ${target} PRIVATE
    src/drv_lcd_cfg.c
    src/drv_lcd.c
    src/lcd/drv_lcd_gc9305.c
    src/lcd/drv_lcd_st7735s.c
    src/lcd/drv_lcd_st7789v.c
    src/lcd/drv_lcd_gc9307.c
    src/lcd_v2/drv_lcd_v2.c
    src/lcd_v2/drv_panel_gc9305.c
    src/lcd_v2/drv_panel_st7789v.c
)

include(core.cmake)

relative_glob(srcs include/*.h src/*.[ch] src/*.cpp
    src/usb/*.[ch] src/image_sensor/*.[ch] core/*.[ch]
)
beautify_c_code(${target} ${srcs})
