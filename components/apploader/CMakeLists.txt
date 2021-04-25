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

configure_file(src/core_config.cmake.in ${out_lib_dir}/core_config.cmake)

set(core_export ${CMAKE_CURRENT_BINARY_DIR}/core_export.o)
set(export_list ${CMAKE_CURRENT_SOURCE_DIR}/src/core_export.list)

cpp_only(export_list_gen src/core_export.list)
add_custom_command(OUTPUT ${core_export} ${core_stub_o}
    COMMAND ${corestubgen} -p ${CONFIG_SOC} --export ${core_export}
        --stub ${core_stub_o} $<TARGET_OBJECTS:export_list_gen>
    DEPENDS $<TARGET_OBJECTS:export_list_gen>
)

set(target apploader)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC src/app_loader.c ${core_export})
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_include_directories(${target} PUBLIC include)
target_include_targets(${target} PRIVATE kernel fs calclib driver)

relative_glob(srcs include/*.h src/*.[ch])
beautify_c_code(${target} ${srcs})
