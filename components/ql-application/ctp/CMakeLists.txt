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

#configure_file(include/lv_gui_config.h.in ${out_inc_dir}/lv_gui_config.h)

set(target ctp)
#add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_CTP)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SRC_DIR} ./ bl6133 cst716)
target_link_libraries(${target} PRIVATE lvgl kernel driver )
target_include_targets(${target} PRIVATE hal)
target_sources(${target} PRIVATE
   ctp_interface.c
   ctp_main.c

   bl6133/Tp_ts.c
   bl6133/fw_update.c
)
relative_glob(srcs include/*.h src/*.c inc/*.h)
beautify_c_code(${target} ${srcs})
