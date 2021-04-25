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

configure_file(include/tts_config.h.in ${out_inc_dir}/tts_config.h)

set(target tts)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC src/tts_player.c src/tts_dummy.c)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_TTS)
target_include_directories(${target} PUBLIC include)
target_include_directories(${target} PUBLIC aisound/include)
target_link_libraries(${target} PRIVATE kernel driver hal fs audio ml)

target_link_libraries_if(CONFIG_TTS_AISOUND_SUPPORT THEN ${target} PRIVATE aisound)

add_subdirectory_if_exist(aisound)

relative_glob(srcs */*.h */*.c)
beautify_c_code(${target} ${srcs})
