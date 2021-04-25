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

set(target libcoap)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
set_target_properties(${target} PROPERTIES OUTPUT_NAME coap)
target_include_directories(${target} PUBLIC include)
target_include_targets(${target} PRIVATE kernel driver calclib net lwip cfw aworker)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${target} PRIVATE src/obgm_libcoap)
target_include_directories(${target} PRIVATE src/obgm_libcoap/include/coap)
target_include_directories(${target} PRIVATE src/obgm_libcoap/examples)

if (CONFIG_QUEC_PROJECT_FEATURE_SSL)
target_include_targets(${target} PRIVATE mbedtls)
endif()

set(LIBEXDIR src/obgm_libcoap/examples)
set(EXFILES
    ${LIBEXDIR}/client.c
    ${LIBEXDIR}/coap_list.c
    src/coap_async_api.c
)
set(LIBCOAPDIR src/obgm_libcoap/src)
set(OBGMFILES
    ${LIBCOAPDIR}/coap_mem.c
    ${LIBCOAPDIR}/block.c
    ${LIBCOAPDIR}/option.c
    ${LIBCOAPDIR}/subscribe.c
    ${LIBCOAPDIR}/uri.c
    ${LIBCOAPDIR}/coap_time.c
    ${LIBCOAPDIR}/net.c
    ${LIBCOAPDIR}/resource.c
    ${LIBCOAPDIR}/pdu.c
    ${LIBCOAPDIR}/async.c
    ${LIBCOAPDIR}/coap_debug.c
    ${LIBCOAPDIR}/address.c
    ${LIBCOAPDIR}/encode.c
    ${LIBCOAPDIR}/hashkey.c
    ${LIBCOAPDIR}/str.c
    ${LIBCOAPDIR}/coap_keystore.c
    ${LIBCOAPDIR}/coap_event.c
    ${LIBCOAPDIR}/platform/posix/coap_io.c
    ${LIBCOAPDIR}/platform/posix/run.c
    ${LIBCOAPDIR}/platform/posix/prng.c
    ${LIBCOAPDIR}/coap_mbeddtls.c
)

target_sources(${target} PRIVATE
    ${EXFILES}
    ${OBGMFILES}
)

relative_glob(srcs include/*.h src/coap_async_api.c)
beautify_c_code(${target} ${srcs})
