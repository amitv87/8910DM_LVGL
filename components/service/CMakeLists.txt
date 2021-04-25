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

configure_file(include/srv_config.h.in ${out_inc_dir}/srv_config.h)

set(target service)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC
    src/srv_wdt.c
    src/srv_dtr.c
    src/srv_sim_detect.c
)

target_sources_if(CONFIG_SOC_8910 THEN ${target} PRIVATE
    src/srv_2line_wakeup.c
    src/srv_rf_param.c
    src/srv_bt_trace.c
    src/srv_keypad.c
    src/srv_headset.c
    src/trace/srv_fs_trace.c
    src/simlock/src/simlock.c
    src/simlock/src/simlock_aescrypt.c
    src/simlock/src/srv_simlock.c
    src/simlock/src/simlock_storage.c
    src/simlock/library/source/aes.c
    src/simlock/library/source/bignum.c
    src/simlock/library/source/rsa.c
    src/simlock/library/source/sha1.c
    src/simlock/library/source/sha2.c
    src/srv_usb.c
    src/power_manager/srv_pm.c
    src/power_manager/srv_pm_main.c
)

set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_SRV)
target_compile_options(${target} PRIVATE "-Wnull-dereference")
target_include_directories(${target} PUBLIC include src/simlock/include src/simlock/library/include)
target_include_targets(${target} PRIVATE kernel driver hal fs calclib apploader nvm audio)

relative_glob(srcs include/*.h src/*.c src/*.cc src/*.cpp src/*.h
              src/trace/*.h src/trace/*.c
              src/power_manage/*.cpp src/power_manage/*.hpp
              src/simlock/include/*.h src/simlock/src/*.c)
beautify_c_code(${target} ${srcs})
