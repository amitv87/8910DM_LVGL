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

configure_file(include/hal_config.h.in ${out_inc_dir}/hal_config.h)

set(hal_spi_flash_prop_csv ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_spi_flash_prop.csv)
set(hal_spi_flash_prop_h ${CMAKE_CURRENT_BINARY_DIR}/hal_spi_flash_prop.h)
add_custom_command(OUTPUT ${hal_spi_flash_prop_h}
    COMMAND python3 ${norpropgen_py} ${hal_spi_flash_prop_csv} ${hal_spi_flash_prop_h}
    DEPENDS ${norpropgen_py} ${hal_spi_flash_prop_csv}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if(NOT ${CONFIG_PINMUX_JSON_PATH} STREQUAL "")
    # Put the definition in respository make it easier to read codes
    set(pin_def_h ${CMAKE_CURRENT_SOURCE_DIR}/include/${CONFIG_SOC}/hal_iomux_pin.h)
    set(pinmux_json ${SOURCE_TOP_DIR}/${CONFIG_PINMUX_JSON_PATH})
    set(iomux_prop_h ${CMAKE_CURRENT_BINARY_DIR}/hal_iomux_prop.h)
    set(iomux_prop_h_in ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_iomux_prop.h.in)
    add_custom_command(OUTPUT ${iomux_prop_h}
        COMMAND python3 ${iomuxgen_py} --chip ${CONFIG_SOC}
            ${pinmux_json} ${iomux_prop_h_in} ${iomux_prop_h} # ${pin_def_h}
        DEPENDS ${chip_iomuxgen_py} ${pinmux_json} ${iomux_prop_h_in}
    )
endif()

set(target hal)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC ${dummy_c_file})
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_include_directories(${target} PUBLIC include include/hwregs/8910 include/8910)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_targets(${target} PUBLIC kernel)
target_include_targets(${target} PRIVATE calclib fs nvm driver)

target_sources_if(CONFIG_SOC_8910 THEN ${target} PRIVATE
    src/8910/hal_clock_init.c
    src/8910/hal_chip_8910.c
    src/8910/hal_pmu_8910.c
    src/8910/hal_efuse_8910.c
    src/8910/hal_wdt_8910.c
    src/8910/hal_aes.c
    src/hal_delay.c
    src/hal_hw_map.c
    src/hal_iomux.c
    ${iomux_prop_h}
    src/hal_reg_apply.c
    src/hal_chip.c
    src/hal_mmu.c
    src/hal_shmem_region.c
    src/hal_adi_bus.c
    src/hal_hwspinlock.c
    src/hal_cp_ctrl.c
    src/hal_lzma.c
    src/hal_spi_flash.c
    ${hal_spi_flash_prop_h}
    src/hal_gouda.c
    src/hal_blue_screen.c
)

relative_glob(srcs include/*.h src/*.c src/*.h
    include/8955/*.h include/8909/*.h include/8910/*.h)
beautify_c_code(${target} ${srcs})
