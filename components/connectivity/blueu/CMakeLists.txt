# Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

if(CONFIG_BLUEU_BT_ENABLE)
    set(target blueu)
    add_app_libraries($<TARGET_FILE:${target}>)

    add_library(${target} STATIC ${dummy_c_file})
    set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
    target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_BT)
    target_include_directories(${target} PUBLIC include)
    target_include_directories(${target} PRIVATE
        ../../diag/src
        ../../kernel/freertos/include
        ../../kernel/chip/8910
        ${CMAKE_CURRENT_BINARY_DIR}
        hal/freertos/log/include/log
        hal/freertos/osi/include/osi
        include/bluetooth
        include/cos
        core/stack/include
        core/vendor/8910/appi/Trace_id/inc
    )
    target_include_targets(${target} PRIVATE kernel hal nvm diag driver fs)

    target_sources(${target} PRIVATE
        hal/freertos/log/log.c
        hal/freertos/osi/osi_mutex.c
        hal/freertos/osi/osi_allocator.c
        hal/freertos/osi/osi_list.c
        hal/freertos/osi/osi_timer.c
        hal/freertos/osi/osi_thread.c
        hal/freertos/osi/osi.c
        hal/freertos/osi/osi_semaphore.c
        hal/freertos/osi/osi_fixed_queue.c
        hal/freertos/osi/osi_mempool.c
        hal/freertos/osi/osi_storage.c
        hal/freertos/osi/osi_utils.c
    )
    include(core.cmake)

    relative_glob(srcs include/*.h src/*.c src/*.h)
    beautify_c_code(${target} ${srcs})
endif()
