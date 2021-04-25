# Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

configure_file(include/nvm_config.h.in ${out_inc_dir}/nvm_config.h)

set(target nvm)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC ${dummy_c_file})
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_include_directories(${target} PUBLIC include)
target_include_targets(${target} PRIVATE kernel fs driver hal calclib fsmount bt_nv)
target_sources_if(CONFIG_SOC_8910 THEN ${target} PRIVATE
    src/nvm_8910.c
    src/nvm_migration.c)

relative_glob(srcs include/*.h src/*.c src/*.h include/*/*.h)
beautify_c_code(${target} ${srcs})
