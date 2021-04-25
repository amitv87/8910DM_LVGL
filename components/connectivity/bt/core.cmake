#
# @file CMakeLists.txt
#
# @brief blueu cmake file
#
#  Copyright (C) 2020 Unisoc Communications Inc.
#  History:
#      <2020.11.06> <wcn bt>
#      Description
#

set(import_lib ${out_lib_dir}/libblueu_core.a)
configure_file(core/${CONFIG_SOC}/libblueu_core.a ${import_lib} COPYONLY)
add_app_libraries(${import_lib})
add_library(${target} STATIC IMPORTED GLOBAL)
set_target_properties(${target} PROPERTIES
    IMPORTED_LOCATION ${import_lib}
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)
