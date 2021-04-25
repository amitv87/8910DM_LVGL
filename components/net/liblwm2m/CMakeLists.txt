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

configure_file(include/liblwm2m_config.h.in ${out_inc_dir}/liblwm2m_config.h)

set(target lwm2m)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
#target_compile_definitions(${target} PRIVATE AT_MQTTSN_SUPPORT=1)
target_include_directories(${target} PUBLIC include)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${target} PRIVATE src/wakaama/examples/shared)
target_include_directories(${target} PRIVATE src/wakaama/examples/client)
target_include_directories(${target} PRIVATE src/wakaama/core/er-coap-13)
target_include_directories(${target} PRIVATE src/wakaama/core)
target_include_targets(${target} PRIVATE kernel driver calclib net lwip libexpat mbedtls cfw fupdate fs)

set(LIBLWM2M_DIR src)
set(LIBLWM2M_WAKAAMA_CORE_DIR src/wakaama/core)
set(LIBLWM2M_WAKAAMA_EXAMPLES_SHARED_DIR src/wakaama/examples/shared)
set(LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR src/wakaama/examples/client)
set(LIBLWM2MFILES
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/objects.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/packet.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/er-coap-13/er-coap-13.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/data.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/bootstrap.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/discover.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/lwm2m_uri.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/block1.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/lwm2m_list.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/tlv.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/registration.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/utils.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/observe.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/liblwm2m.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/management.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/json.c
    ${LIBLWM2M_WAKAAMA_CORE_DIR}/transaction.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_SHARED_DIR}/commandline.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_SHARED_DIR}/memtrace.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_SHARED_DIR}/lwm2m_platform.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_SHARED_DIR}/mbedconnection.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/object_location.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/object_access_control.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/object_server.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/lwm2mclient.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/object_connectivity_stat.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/object_security.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/system_api.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/test_object.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/object_connectivity_moni.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/object_device.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/object_firmware.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/object_temperature.c
    ${LIBLWM2M_WAKAAMA_EXAMPLES_CLIENT_DIR}/object_illuminance.c
    ${LIBLWM2M_DIR}/lwm2m_api.c
    ${LIBLWM2M_DIR}/objtable.c
    ${LIBLWM2M_DIR}/object_skel.c
    ${LIBLWM2M_DIR}/lwm2m_fota.c
    ${LIBLWM2M_DIR}/epname_encode.c
    ${LIBLWM2M_DIR}/j_aes.c
    ${LIBLWM2M_DIR}/j_base64.c
)

target_sources(${target} PRIVATE
    ${LIBLWM2MFILES}
)

relative_glob(srcs include/*.h
    src/epname_encode.c src/j_aes.c src/j_base64.c
    src/lwm2m_api.c src/lwm2m_fota.c src/object_skel.c src/objtable.c)
beautify_c_code(${target} ${srcs})
