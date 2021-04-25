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

set(target fatfs_core)
add_app_libraries($<TARGET_FILE:${target}>)

set(symrename_src ${CMAKE_CURRENT_SOURCE_DIR}/symrename)
set(symrename ${CMAKE_CURRENT_BINARY_DIR}/fat_symdef.h)
add_custom_command(OUTPUT ${symrename}
    COMMAND python3 ${symdefgen_py} ${symrename_src} "fat_" ${symrename}
    DEPENDS ${symdefgen_py} ${symrename_src}
)
add_compile_options(-include ${symrename})

add_library(${target} STATIC
    ${symrename}
    source/ff.c
)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_FS)
target_include_directories(${target} PUBLIC source)
target_include_targets(${target} PRIVATE kernel driver fs fatfs ml)
