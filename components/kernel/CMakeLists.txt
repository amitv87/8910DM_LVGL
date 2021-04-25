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

configure_file(include/kernel_config.h.in ${out_inc_dir}/kernel_config.h)

set(target kernel)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_KERNEL)
target_include_directories(${target} PUBLIC include)
target_include_directories(${target} PUBLIC src)
target_include_directories(${target} PRIVATE freertos/include src)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_targets(${target} PRIVATE hal driver fs service nanopb)

nanopbgen(src/osi_sysnv.proto)
target_sources(${target} PRIVATE
    freertos/croutine.c
    freertos/event_groups.c
    freertos/list.c
    freertos/queue.c
    freertos/stream_buffer.c
    freertos/tasks.c
    freertos/timers.c
    src/osi_idle.c
    src/osi_clock.c
    src/osi_sysnv.c
    src/osi_cxxabi.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/osi_sysnv.pb.c
)

target_sources_if(CONFIG_SOC_8910 THEN ${target} PRIVATE
    chip/8910/startup.S
    chip/8910/vectors.S
    chip/8910/osi_boot_8910.c
    chip/8910/port.c
    chip/8910/portASM.S
    chip/8910/osi_chip_8910.c
    src/osi_blue_screen.c
    src/osi_freertos.c
    src/osi_profile.c
    src/osi_irq.c
    src/osi_time.c
    src/osi_sleep.c
)
set_if(chipdir CONFIG_SOC_8910 THEN chip chip/8910)

target_include_directories(${target} PRIVATE ${chipdir})
include(core.cmake)

set(target osi_lib)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_KERNEL)
target_include_directories(${target} PUBLIC include)
target_include_directories(${target} PRIVATE chip)
target_include_targets(${target} PRIVATE hal driver)

target_sources(${target} PRIVATE
    src/osi_mem_record.c
    src/osi_log.c
    src/osi_work.c
    src/osi_fifo.c
    src/osi_blocked_fifo.c
    src/osi_pipe.c
    src/osi_line_cache.c
    src/osi_vsmap.c
    src/osi_order_list.c
    src/osi_event_hub.c
    src/osi_mem_recycler.c
    src/osi_trace.c
    src/osi_hdlc.c
)

relative_glob(srcs include/*.h src/*.c src/*.h chip/*/osi*.[ch])
beautify_c_code(${target} ${srcs})
