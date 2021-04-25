# Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

configure_file(include/bt_app_config.h.in ${out_inc_dir}/bt_app_config.h)
add_definitions(-DBUILD_IDENTIFY=\"${BUILD_TARGET}-${BUILD_RELEASE_TYPE}-${BUILD_REVISION}-${BUILD_AUTO_REVISION}\")

add_subdirectory_if_exist(bt_nv)

if(CONFIG_BLUEU_BT_ENABLE)
set(target bt_app)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC ${dummy_c_file})
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_BT)
target_include_directories(${target} PUBLIC include)
target_include_directories(${target} PUBLIC example_ble_server/include)
target_include_directories(${target} PRIVATE
    ../diag/src
    ../kernel/freertos/include
    ../kernel/chip/8910
    ../connectivity/bt/include
    ../connectivity/bt/include/test
    ../audio/include
)

target_include_targets(${target} PRIVATE kernel hal nvm diag driver fs)
target_include_targets_if(CONFIG_BLUEU_BT_ENABLE THEN ${target} PRIVATE blueu)
target_incbin_size(${target} ${CMAKE_CURRENT_SOURCE_DIR}/bt_bsp/fw.bin psram_midi psram_midi_size)

target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/bt_adaptor)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/bt_bsp)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/bt_bsp/nv/include)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bt_bsp/hci_trace)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bt_bsp/driver)

###### example code #####
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/example_ble_client/include)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/example_bt_hfp)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/example_bt_avrcp)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/example_bt_a2dp)

target_sources(${target} PRIVATE
    bt_adaptor/bt_abs.c
    bt_bsp/nv/src/ddb.c
    bt_bsp/driver/bt_chip.c
    bt_bsp/driver/uart.c
    bt_bsp/driver/bt_hci_dev.c
    bt_bsp/driver/bt_driver.c
    bt_bsp/hci_trace/bt_hci_tra.c
)

target_sources_if(CONFIG_AT_BT_APP_SUPPORT THEN ${target} PRIVATE
    src/bt_app.c
    src/bt_app_main.c

#      example src  #
    example_ble_client/src/bt_gatt_client_demo.c
    example_ble_server/src/bt_gatt_server_demo.c
    example_bt_hfp/app_bt_hfp.c
    example_bt_avrcp/app_bt_avrcp.c
    example_bt_a2dp/app_bt_a2dp.c
)

relative_glob(srcs include/*.h src/*.c src/*.h)
beautify_c_code(${target} ${srcs})
endif()