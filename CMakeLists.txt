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
set(WITH_WERROR OFF)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(cmake/extension.cmake)

# Use a global property to collect all application libraries
define_property(GLOBAL PROPERTY app_libraries
    BRIEF_DOCS "app libraries"
    FULL_DOCS "app libraries"
)

set(out_hex_dir ${CMAKE_CURRENT_BINARY_DIR}/hex)
set(out_hex_example_dir ${CMAKE_CURRENT_BINARY_DIR}/hex/examples)
set(out_hex_unittest_dir ${CMAKE_CURRENT_BINARY_DIR}/hex/unittests)
set(out_lib_dir ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(out_app_lib_dir ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(out_rpc_dir ${CMAKE_CURRENT_BINARY_DIR}/rpcgen)
set(out_inc_dir ${CMAKE_CURRENT_BINARY_DIR}/include)
set(tools_dir ${CMAKE_CURRENT_SOURCE_DIR}/tools)
set(ql_app_dir ${CMAKE_CURRENT_SOURCE_DIR}/components/ql-application)

set (QL_CCSDK_QUEC_PROJECT $ENV{ql_buildproj})
message("\n target dir: ${CMAKE_CURRENT_SOURCE_DIR}/components/ql-config/build/${QL_CCSDK_QUEC_PROJECT}/${BUILD_TARGET}\n")

set(build_target_dir ${CMAKE_CURRENT_SOURCE_DIR}/components/ql-config/build/${QL_CCSDK_QUEC_PROJECT}/${BUILD_TARGET})
set(elf2symld_py ${tools_dir}/elf2symld.py)
set(elf2incld_py ${tools_dir}/elf2incld.py)
set(pacgen_py ${tools_dir}/pacgen.py)
set(signature_py ${tools_dir}/signature/signature.py)
set(iomuxgen_py ${tools_dir}/iomuxgen.py)
set(keypadgen_py ${tools_dir}/keypadgen.py)
set(partinfogen_py ${tools_dir}/partinfogen.py)
set(modemgen_py ${tools_dir}/modemgen.py)
set(romtablegen_py ${tools_dir}/romtablegen.py)
set(symdefgen_py ${tools_dir}/symdefgen.py)
set(groupgen_py ${tools_dir}/groupgen.py)
set(norpropgen_py ${tools_dir}/norpropgen.py)
set(corestubgen dtools expgen)
set(cmd_mkuimage dtools mkuimage)
set(cmd_mksimage dtools mksimage)
set(cmd_mkappimg dtools mkappimg)
set(cmd_elf2lod dtools elf2lod)
set(cmd_elf2bin dtools elf2bin)
set(cmd_elf2romhex dtools elf2romhex)
set(cmd_lzmare2 dtools lzmare2)
set(cmd_re2c re2c)
set(partinfo_bin ${out_hex_dir}/partinfo.bin)
set(core_stub_o ${out_lib_dir}/core_stub.o)
set(dummy_c_file ${SOURCE_TOP_DIR}/cmake/dummy.c)
set(dummy_cxx_file ${SOURCE_TOP_DIR}/cmake/dummy.cpp)

set(AUTO_GENERATION_NOTICE "Auto generated. Don't edit it manually!")

find_package(Git)
if(GIT_FOUND)
    execute_process(COMMAND "${GIT_EXECUTABLE}" describe --abbrev=8 --always --dirty
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE BUILD_GIT_REVISION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

# BUILD_REVISION: A string to identify the build. When not specified, which
#       is common in development, it will be "DEVEL". CI will define it in
#       command line, such as "8915_W19.32.2"
if(NOT BUILD_REVISION)
    set(BUILD_REVISION $ENV{BUILD_REVISION})
    if(NOT BUILD_REVISION)
        set(BUILD_REVISION DEVEL)
    endif()
endif()

# BUILD_AUTO_REVISION: A string to identify the source codes revision.
#       When it is built under git working directory, it will be "g1234abcd"
#       or "g1234abcd-dirty" if the working directory is dirty. When not in
#       git working directory, it will be "unknown". And it can be overrided
#       in command line.
if(NOT BUILD_AUTO_REVISION)
    if(BUILD_GIT_REVISION)
        set(BUILD_AUTO_REVISION "g${BUILD_GIT_REVISION}")
    endif()
endif()

if(NOT BUILD_AUTO_REVISION)
    set(BUILD_AUTO_REVISION "unknown")
endif()

# BUILD_REVISION_NUMBER: A number to identify the source codes revision.
#       When it is built under git working directory, and the working
#       directory is clean, it will be "0x1234abcd". Otherwise, it is "0".
#       And it can be overrided in command line.
if(NOT BUILD_REVISION_NUMBER)
    if(BUILD_GIT_REVISION)
        set(BUILD_REVISION_NUMBER "0x${BUILD_GIT_REVISION}")
    endif()
    if(BUILD_REVISION_NUMBER MATCHES "-dirty")
        set(BUILD_REVISION_NUMBER "0")
    endif()
endif()

if(NOT BUILD_REVISION_NUMBER)
    set(BUILD_REVISION_NUMBER "0")
endif()

# BUILD_TARGET
if(NOT BUILD_TARGET)
    set(BUILD_TARGET $ENV{BUILD_TARGET})
endif()

# BUILD_RELEASE_TYPE
if (NOT BUILD_RELEASE_TYPE)
    set (BUILD_RELEASE_TYPE $ENV{BUILD_RELEASE_TYPE})
endif()

if("${BUILD_RELEASE_TYPE}" STREQUAL "release")
    set(CONFIG_WDT_ENABLE ON)
endif()

if(NOT BUILD_TARGET)
    message(FATAL_ERROR "BUILD_TARGET is empty")
endif()

include(components/ql-application/ql_target.cmake)
if (NOT CONFIG_QUEC_PROJECT_FEATURE_VOLTE)
	if ($ENV{quec_ims_feature})
		message(FATAL_ERROR "VOLTE is disabled at CCSDK!")
	endif()
endif()
if (NOT CONFIG_QUEC_PROJECT_FEATURE_QDSIM)
	if ($ENV{ql_dsim_cfg})
		message(FATAL_ERROR "DOUBLESIM is disabled at CCSDK!")
	endif()
endif()

set (QL_CCSDK_BUILD $ENV{_ccsdk_build})
set (QL_APP_BUILD_VER $ENV{ql_app_ver})
# Process and include target config
set(CONFIG_BUILD_IDENTIFY "${BUILD_TARGET}-${BUILD_RELEASE_TYPE}-${BUILD_REVISION}-${BUILD_AUTO_REVISION}")

set(SRC_CONFIG ${SOURCE_TOP_DIR}/components/ql-config/build/${QL_CCSDK_QUEC_PROJECT}/${BUILD_TARGET}/target.config)
set(TARGET_CONFIG ${BINARY_TOP_DIR}/target.config)
file(COPY ${SRC_CONFIG} DESTINATION  ${BINARY_TOP_DIR})

file(APPEND ${TARGET_CONFIG} "CONFIG_APP_RAM_OFFSET=$ENV{ap_ram_offset}\n")
file(APPEND ${TARGET_CONFIG} "CONFIG_APP_TOTAL_RAM_SIZE=$ENV{ap_ram_size}\n")
file(APPEND ${TARGET_CONFIG} "CONFIG_SUPPORT_IMS_DELTA_NV=$ENV{ims_delta_nv}\n")
file(APPEND ${TARGET_CONFIG} "CONFIG_DUAL_SIM_SUPPORT=$ENV{ql_dsim_cfg}\n")
file(APPEND ${TARGET_CONFIG} "CONFIG_BOARD_WITH_EXT_FLASH=$ENV{ql_extflash}\n")
file(APPEND ${TARGET_CONFIG} "CONFIG_QUEC_PROJECT_FEATURE_VOLTE=$ENV{quec_ims_feature}\n")

set(TARGET_CMAKE ${BINARY_TOP_DIR}/target.cmake)
execute_process(
    COMMAND python3 ${tools_dir}/cmakeconfig.py ${TARGET_CONFIG} ${TARGET_CMAKE}
    WORKING_DIRECTORY ${SOURCE_TOP_DIR}
)
include(${TARGET_CMAKE})
message("\nCurr Proj: ${CONFIG_QL_PROJECT_DEF}${CONFIG_QL_PROJECT_DEF_SUB}, QL_CCSDK_BUILD ${QL_CCSDK_BUILD}\n")
message("\n${CONFIG_NVM_VARIANTS}, ${CONFIG_PARTINFO_JSON_PATH}, ${CONFIG_APP_RAM_OFFSET}, ${CONFIG_APP_TOTAL_RAM_SIZE}, ${CONFIG_SUPPORT_IMS_DELTA_NV}, ${CONFIG_DUAL_SIM_SUPPORT}, ${CONFIG_BOARD_WITH_EXT_FLASH}\n")

# Process and include partition config
execute_process(
    COMMAND python3 ${partinfogen_py} ${CONFIG_PARTINFO_JSON_PATH} ${partinfo_bin} ${BINARY_TOP_DIR}/partinfo.cmake
    WORKING_DIRECTORY ${SOURCE_TOP_DIR}
)
include(${BINARY_TOP_DIR}/partinfo.cmake)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${partinfogen_py} ${CONFIG_PARTINFO_JSON_PATH})

# Include chip config
include(components/chip/chip.cmake)

# Process and include Kconfig dependency
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${TARGET_CONFIG})
set(KCONFIG_DEP ${BINARY_TOP_DIR}/kconfig_dep.cmake)
execute_process(
    COMMAND python3 ${tools_dir}/cmakedep.py ${KCONFIG_DEP}
    WORKING_DIRECTORY ${SOURCE_TOP_DIR}
)
include(${KCONFIG_DEP})

# Include toolchain config
include(cmake/toolchain-gcc.cmake)
include(components/ql-application/ql_app_feature_config.cmake)
set(BUILD_SHARED_LIBS OFF)

project(${BUILD_TARGET} C CXX ASM)
include_directories(${out_inc_dir})
include_directories(${SOURCE_TOP_DIR}/components/newlib/include)
include_directories(${SOURCE_TOP_DIR}/components/hal/include)
include_directories(${SOURCE_TOP_DIR}/components/fs/include)
include_directories(${SOURCE_TOP_DIR}/components/fs/fsmount/include)
include_directories(${SOURCE_TOP_DIR}/components/cfw/include)
include_directories(${SOURCE_TOP_DIR}/components/net/lwip/include)
include_directories(${SOURCE_TOP_DIR}/components/net/lwip/src/include)
include_directories(${SOURCE_TOP_DIR}/components/net/mbedtls/include)
include_directories(${SOURCE_TOP_DIR}/components/net/include)
include_directories(${SOURCE_TOP_DIR}/components/kernel/include)
include_directories(${SOURCE_TOP_DIR}/components/ql-kernel/inc)

#add_custom_target(quectel)
add_custom_target(beautify)
add_custom_target(unittests)
#add_custom_target(examples ALL)
if(CONFIG_QL_OPEN_EXPORT_PKG)
add_custom_target(ql_examples ALL)
endif()
add_custom_target(rpcgen)
add_appimg_delete()

add_definitions(-DPLATFORM_EC600)

add_subdirectory_if_exist(components)
add_subdirectory_if_exist(unittest)
#add_subdirectory_if_exist(examples)

pacvariant_gen()

file(GLOB QL_LIBS_PATH ${SOURCE_TOP_DIR}/components/ql-kernel/libs/libq*.a)
#message("\nQL_LIBS_PATH ${QL_LIBS_PATH}\n")

# "all_libs" is a target to link all app libraries
set(target all_libs)
set(all_libs_out ${out_lib_dir}/all_libs.a)
get_property(app_libraries GLOBAL PROPERTY app_libraries)
list(APPEND app_libraries ${libc_file_name} ${libm_file_name} ${libgcc_file_name} ${QL_LIBS_PATH})
add_custom_command(OUTPUT ${all_libs_out}
    COMMAND python3 ${groupgen_py} --base ${BINARY_TOP_DIR} ${all_libs_out} ${app_libraries}
    DEPENDS ${groupgen_py} ${app_libraries}
    WORKING_DIRECTORY ${BINARY_TOP_DIR}
)
add_custom_target(build_${target} DEPENDS ${all_libs_out})
add_library(${target} STATIC IMPORTED GLOBAL)
set_target_properties(${target} PROPERTIES IMPORTED_LOCATION ${all_libs_out})
add_dependencies(${target} build_${target})

