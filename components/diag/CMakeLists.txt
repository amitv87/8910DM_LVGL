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

configure_file(include/diag_config.h.in ${out_inc_dir}/diag_config.h)

set(target diag)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC ${dummy_c_file})
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_DIAG)
target_include_directories(${target} PUBLIC include)
target_include_targets(${target} PRIVATE kernel driver audio hal fs cfw nvm calclib service)
target_include_targets_if(CONFIG_BLUEU_BT_ENABLE THEN ${target} PRIVATE bt_app blueu)

if(CONFIG_DIAG_ENABLED)
    target_sources_if(CONFIG_SOC_8910 THEN ${target} PRIVATE
        src/diag.c
        src/layer1_diag.c
        src/diag_current_test.c
        src/diag_swver.c
        src/diag_log.c
        src/diag_nvm.c
        src/diag_runmode.c
        src/diag_at.c
        src/diag_sys.c
        src/diag_ps.c
        src/diag_charge.c
        src/8910/autotest_device.c
    )
endif()

target_include_directories(${target} PRIVATE ../connectivity/bt/include/bluetooth)
target_include_directories(${target} PRIVATE ../connectivity/bt/include/test)

relative_glob(srcs include/*.h src/*.[ch] src/8910/*.c)
list(FILTER srcs EXCLUDE REGEX ".*/calibration.h")
list(FILTER srcs EXCLUDE REGEX ".*/cmddef.h")
beautify_c_code(${target} ${srcs})
