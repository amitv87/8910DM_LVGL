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

cmake_minimum_required(VERSION 3.13)

set(SOURCE_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BINARY_TOP_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_C_COMPILER_FORCED   1)
set(CMAKE_CXX_COMPILER_FORCED 1)

set(out_hex_dir ${CMAKE_CURRENT_BINARY_DIR}/hex)
set(out_lib_dir ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(out_inc_dir ${CMAKE_CURRENT_BINARY_DIR}/include)
set(tools_dir ${CMAKE_CURRENT_SOURCE_DIR}/tools)

set(core_stub_o ${CMAKE_CURRENT_SOURCE_DIR}/components/lib/core_stub.o)
set(flash_ldscript ${SOURCE_TOP_DIR}/ldscripts/app_flashimg.ld)
set(file_ldscript ${SOURCE_TOP_DIR}/ldscripts/app_fileimg.ld)
set(cmd_mkappimg dtools mkappimg)
set(pacgen_py ${tools_dir}/pacgen.py)
set(modemgen_py ${tools_dir}/modemgen.py)

include(cmake/core_config.cmake)
include(cmake/toolchain-gcc.cmake)
set(BUILD_SHARED_LIBS OFF)

project(core C CXX ASM)

include_directories(components/include components/newlib/include)

function(cpp_only target file)
    add_library(${target} OBJECT ${file})
    set_source_files_properties(${file} PROPERTIES LANGUAGE C)
    target_compile_options(${target} PRIVATE -E -P -x c)
endfunction()

#quectel update
#set(sign_password 12345678)     # customer product shall replace with customer's key
#set(sign_product test)          # customer product shall replace with customer's product name
#function(sign_image src dst)
#    add_custom_command(OUTPUT ${dst}
#        COMMAND vlrsign --pw ${sign_password} --pn ${sign_product} --ha Blake2
#            --img ${src} --out ${dst}
#        DEPENDS ${src}
#    )
#endfunction()

function(add_appimg target ldscript)
    set(gen_ldscript ${target}_ldscript)
    set(target_map_file ${out_hex_dir}/${target}.map)
    set(target_img_file ${out_hex_dir}/${target}.img)
    cpp_only(${gen_ldscript} ${ldscript})
    add_executable(${target} ${ARGN} ${core_stub_o})
    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${out_hex_dir})
    target_link_libraries(${target} PRIVATE -T $<TARGET_OBJECTS:${gen_ldscript}>)
    target_link_libraries(${target} PRIVATE -Wl,-Map=${target_map_file} -nostdlib -Wl,--gc-sections)

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${cmd_mkappimg} $<TARGET_FILE:${target}> ${target_img_file}
        BYPRODUCTS ${target_img_file} ${target_map_file}
    )
endfunction()

configure_file(${SOURCE_TOP_DIR}/components/lib/fdl1.img ${out_hex_dir}/fdl1.img COPYONLY)
configure_file(${SOURCE_TOP_DIR}/components/lib/fdl2.img ${out_hex_dir}/fdl2.img COPYONLY)
macro(pac_init_fdl cmd)
    set(${cmd}
        cfg-init --pname UIX8910_MODEM --palias "APPIMG"
            --pversion "8910 MODULE" --version "BP_R1.0.0"
            --flashtype 1
        cfg-host-fdl -a ${CONFIG_FDL1_IMAGE_START} -s ${CONFIG_FDL1_IMAGE_SIZE}
            -p ${out_hex_dir}/fdl1.img
        cfg-fdl2 -a ${CONFIG_FDL2_IMAGE_START} -s ${CONFIG_FDL2_IMAGE_SIZE}
            -p ${out_hex_dir}/fdl2.img
    )
endmacro()

if(CONFIG_APPIMG_LOAD_FLASH)
    set(target appimg_flash_delete)
    set(pac_file ${out_hex_dir}/${target}.pac)
    pac_init_fdl(init_fdl)
    add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
            cfg-erase-flash -i ERASE_APPIMG
                -a ${CONFIG_APPIMG_FLASH_ADDRESS}
                -s ${CONFIG_APPIMG_FLASH_SIZE}
            pac-gen ${pac_file}
        DEPENDS ${pacgen_py}
        WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})
endif()

if(CONFIG_APPIMG_LOAD_FILE)
    set(target appimg_file_delete)
    set(pac_file ${out_hex_dir}/${target}.pac)
    pac_init_fdl(init_fdl)
    add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
            cfg-del-appimg -i DEL_APPIMG
            pac-gen ${pac_file}
        DEPENDS ${pacgen_py}
        WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})
endif()

if(CONFIG_APPIMG_LOAD_FLASH)
    set(target hello_flash)
    add_appimg(${target} ${flash_ldscript}  hello_world.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name})
    set(prepack_cpio ${out_hex_dir}/${target}_prepack.cpio)

    set(pac_file ${out_hex_dir}/${target}.pac)
    set(target_img_file ${out_hex_dir}/${target}.img)
    set(target_sign_img_file ${out_hex_dir}/${target}.sign.img)
    pac_init_fdl(init_fdl)
    add_custom_command(OUTPUT ${prepack_cpio}
            COMMAND python3 ${modemgen_py} prepackgen
                --source-top ${SOURCE_TOP_DIR}
                --binary-top ${BINARY_TOP_DIR}
                --prepackfile ${CONFIG_PACKAGE_FILE_APPIMG_JSON_PATH}
                ${prepack_cpio}
            DEPENDS ${modemgen_py}
            WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    if(CONFIG_PACKAGE_FILE_APPIMG_JSON_PATH)
        set(cfg_pack_gpio cfg-pack-cpio -i PREPACK -p ${prepack_cpio})
        set(prepack_file ${prepack_cpio})
    endif()
    sign_image(${target_img_file} ${target_sign_img_file})
    add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
            cfg-image -i APPIMG -a ${CONFIG_APPIMG_FLASH_ADDRESS}
                -s ${CONFIG_APPIMG_FLASH_SIZE}
                -p ${target_sign_img_file}
            ${cfg_pack_gpio}
            pac-gen ${pac_file}
        DEPENDS ${pacgen_py} ${target_sign_img_file} ${prepack_file}
        WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})
endif()

if(CONFIG_APPIMG_LOAD_FILE)
    set(target hello_file)
    add_appimg(${target} ${file_ldscript} hello_world.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name})
    set(prepack_cpio ${out_hex_dir}/${target}_prepack.cpio)

    set(pac_file ${out_hex_dir}/${target}.pac)
    set(target_img_file ${out_hex_dir}/${target}.img)
    set(target_sign_img_file ${out_hex_dir}/${target}.sign.img)
    pac_init_fdl(init_fdl)
    add_custom_command(OUTPUT ${prepack_cpio}
            COMMAND python3 ${modemgen_py} prepackgen
                --source-top ${SOURCE_TOP_DIR}
                --binary-top ${BINARY_TOP_DIR}
                --prepackfile ${CONFIG_PACKAGE_FILE_APPIMG_JSON_PATH}
                ${prepack_cpio}
            DEPENDS ${modemgen_py}
            WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    if(CONFIG_PACKAGE_FILE_APPIMG_JSON_PATH)
        set(cfg_pack_gpio cfg-pack-cpio -i PREPACK -p ${prepack_cpio})
        set(prepack_file ${prepack_cpio})
    endif()
    sign_image(${target_img_file} ${target_sign_img_file})
    add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
            cfg-pack-file -i APPIMG -p ${target_sign_img_file}
                -n ${CONFIG_APPIMG_LOAD_FILE_NAME}
            ${cfg_pack_gpio}
            pac-gen ${pac_file}
        DEPENDS ${pacgen_py} ${target_sign_img_file} ${prepack_file}
        WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})
endif()
