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

set(target fsmount)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC src/fs_mount.c)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_FS)
target_include_directories(${target} PUBLIC include)
target_include_targets(${target} PRIVATE kernel driver bdev fs sffs fatfs calclib)

if(CONFIG_FS_MOUNT_SDCARD)
    target_sources(${target} PRIVATE src/fs_mount_sdcard.c)
endif()

target_incbin(${target} ${partinfo_bin} gPartInfo)

relative_glob(srcs include/*.h src/*.c src/*.h)
beautify_c_code(${target} ${srcs})
