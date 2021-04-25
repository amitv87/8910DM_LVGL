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

set(cis_sdk_version NB_SDK2.3.0_D1.1.1)
set(target cis_sdk)
add_app_libraries($<TARGET_FILE:${target}>)

set(symrename cis_sdk_symdef.h)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${symrename}
    COMMAND python3 ${symdefgen_py} ${CMAKE_CURRENT_SOURCE_DIR}/symrename
        "cis_sdk_" ${CMAKE_CURRENT_BINARY_DIR}/${symrename}
    DEPENDS ${symdefgen_py} ${CMAKE_CURRENT_SOURCE_DIR}/symrename
)
add_compile_options(-include ${symrename})

add_library(${target} STATIC ${CMAKE_CURRENT_BINARY_DIR}/${symrename})
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_NET)
target_include_directories(${target} PUBLIC src/${cis_sdk_version}/src/ciscore)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${target} PRIVATE src/${cis_sdk_version}/src/ciscore/dm_utils)
target_include_targets(${target} PRIVATE kernel driver calclib cfw nvm fupdate fs lwip net)

set(CIS_SDK_DIR src/${cis_sdk_version}/src)
set(CIS_SDK_ADAPTER ${CIS_SDK_DIR}/adapter/win/cis_if_net.c
    ${CIS_SDK_DIR}/adapter/win/cis_if_sys.c
)
set(CIS_SDK_CORE ${CIS_SDK_DIR}/ciscore/cis_data.c
    ${CIS_SDK_DIR}/ciscore/cis_objects.c
    ${CIS_SDK_DIR}/ciscore/cis_config.c
    ${CIS_SDK_DIR}/ciscore/cis_observe.c
    ${CIS_SDK_DIR}/ciscore/cis_bootstrap.c
    ${CIS_SDK_DIR}/ciscore/cis_utils.c
    ${CIS_SDK_DIR}/ciscore/cis_management.c
    ${CIS_SDK_DIR}/ciscore/cis_transaction.c
    ${CIS_SDK_DIR}/ciscore/cis_uri.c
    ${CIS_SDK_DIR}/ciscore/cis_registration.c
    ${CIS_SDK_DIR}/ciscore/cis_tlv.c
    ${CIS_SDK_DIR}/ciscore/cis_core.c
    ${CIS_SDK_DIR}/ciscore/cis_block1.c
    ${CIS_SDK_DIR}/ciscore/cis_log.c
    ${CIS_SDK_DIR}/ciscore/cis_memtrace.c
    ${CIS_SDK_DIR}/ciscore/cis_json.c
    ${CIS_SDK_DIR}/ciscore/cis_discover.c
    ${CIS_SDK_DIR}/ciscore/cis_packet.c
    ${CIS_SDK_DIR}/ciscore/cis_list.c
)
set(CIS_SDK_DM ${CIS_SDK_DIR}/ciscore/dm_utils/dm_endpoint.c
    ${CIS_SDK_DIR}/ciscore/dm_utils/aes.c
    #${CIS_SDK_DIR}/ciscore/dm_utils/j_aes.c
    ${CIS_SDK_DIR}/ciscore/dm_utils/j_base64.c
    ${CIS_SDK_DIR}/ciscore/dm_utils/sha256.c
)
set(CIS_SDK_DTLS ${CIS_SDK_DIR}/ciscore/dtls/ccm.c
    ${CIS_SDK_DIR}/ciscore/dtls/cis_aes.c
    ${CIS_SDK_DIR}/ciscore/dtls/crypto.c
    ${CIS_SDK_DIR}/ciscore/dtls/debug.c
    ${CIS_SDK_DIR}/ciscore/dtls/dtls.c
    ${CIS_SDK_DIR}/ciscore/dtls/ecc.c
    ${CIS_SDK_DIR}/ciscore/dtls/hmac.c
    ${CIS_SDK_DIR}/ciscore/dtls/netq.c
    ${CIS_SDK_DIR}/ciscore/dtls/peer.c
    ${CIS_SDK_DIR}/ciscore/dtls/sha2.c
)
set(CIS_SDK_COAP ${CIS_SDK_DIR}/ciscore/er-coap-13/er-coap-13.c)
set(CIS_SDK_OBJECT ${CIS_SDK_DIR}/ciscore/std_object/std_object.c
    ${CIS_SDK_DIR}/ciscore/std_object/std_object_security.c
    ${CIS_SDK_DIR}/ciscore/std_object/cis_object_conn_moniter.c
    ${CIS_SDK_DIR}/ciscore/std_object/cis_object_device.c
    ${CIS_SDK_DIR}/ciscore/std_object/cis_object_firmware.c
    ${CIS_SDK_DIR}/ciscore/std_object/cis_object_cmdhdefecvalues.c
    ${CIS_SDK_DIR}/ciscore/std_object/cis_object_extend_moniter.c
    ${CIS_SDK_DIR}/ciscore/std_object/cis_object_powerup_log.c
    ${CIS_SDK_DIR}/ciscore/std_object/object_dm.c
)

target_sources(${target} PRIVATE
    ${CIS_SDK_ADAPTER}
    ${CIS_SDK_DTLS}
    ${CIS_SDK_CORE}
    ${CIS_SDK_DM}
    ${CIS_SDK_COAP}
    ${CIS_SDK_OBJECT}
)
