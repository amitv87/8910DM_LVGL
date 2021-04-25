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

configure_file(include/audio_config.h.in ${out_inc_dir}/audio_config.h)

set(target audio)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_AUDIO)
target_include_directories(${target} PUBLIC include)
target_include_directories(${target} PRIVATE ${out_rpc_dir})
target_include_targets(${target} PRIVATE kernel driver hal fs nanopb cfw)

target_sources(${target} PRIVATE
    src/audio_types.c
    src/audio_reader.c
    src/audio_writer.c
    src/audio_decoder.c
    src/audio_encoder.c
)
if(CONFIG_SOC_8910)
    nanopbgen(src/8910/audio_device.proto)
    target_sources(${target} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/audio_device.pb.c
        src/8910/audio_device.c
        src/8910/hal_aif.c
    )
    add_dependencies(${target} rpc)
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endif()

relative_glob(srcs include/*.[ch] src/*.[ch] src/8910/*.[ch])
beautify_c_code(${target} ${srcs})

include(core.cmake)
