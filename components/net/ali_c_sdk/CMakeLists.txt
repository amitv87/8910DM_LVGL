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

set(target ali_c_sdk)
add_app_libraries($<TARGET_FILE:${target}>)

set(symrename ali_sdk_symdef.h)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${symrename}
    COMMAND python3 ${symdefgen_py} ${CMAKE_CURRENT_SOURCE_DIR}/symrename
        "ali_sdk_" ${CMAKE_CURRENT_BINARY_DIR}/${symrename}
    DEPENDS ${symdefgen_py} ${CMAKE_CURRENT_SOURCE_DIR}/symrename
)
add_compile_options(-include ${CMAKE_CURRENT_BINARY_DIR}/${symrename})

add_library(${target} STATIC ${CMAKE_CURRENT_BINARY_DIR}/${symrename})
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_NET)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_targets(${target} PRIVATE kernel driver fupdate fs net lwip mbedtls)

set(ALI_C_SDK_DIR v3.0.1)
target_include_directories(${target} PUBLIC ${ALI_C_SDK_DIR}/adapter)
target_include_directories(${target} PUBLIC ${ALI_C_SDK_DIR}/src/infra)
target_include_directories(${target} PUBLIC ${ALI_C_SDK_DIR}/src/dev_sign)
target_include_directories(${target} PUBLIC ${ALI_C_SDK_DIR}/src/mqtt)
target_include_directories(${target} PUBLIC ${ALI_C_SDK_DIR}/wrappers)
target_include_directories(${target} PRIVATE ${ALI_C_SDK_DIR}/src/dev_model)
target_include_directories(${target} PRIVATE ${ALI_C_SDK_DIR}/src/dev_model/client)
target_include_directories(${target} PRIVATE ${ALI_C_SDK_DIR}/src/dev_model/server)
target_include_directories(${target} PRIVATE ${ALI_C_SDK_DIR}/src/dev_model/examples)
target_include_directories(${target} PUBLIC ${ALI_C_SDK_DIR}/src/dynamic_register)

set(ALI_C_SDK_ADAPTER ${ALI_C_SDK_DIR}/adapter/HAL_OS_osi.c
    ${ALI_C_SDK_DIR}/adapter/HAL_TCP_lwip.c
    ${ALI_C_SDK_DIR}/adapter/HAL_KV_osi.c
    ${ALI_C_SDK_DIR}/adapter/HAL_TLS_mbedtls.c
)
set(ALI_C_SDK_CERTS ${ALI_C_SDK_DIR}/certs/root_ca.c)
set(ALI_C_SDK_DEV_MODEL ${ALI_C_SDK_DIR}/src/dev_model/dm_api.c
    ${ALI_C_SDK_DIR}/src/dev_model/client/dm_client.c
    ${ALI_C_SDK_DIR}/src/dev_model/client/dm_client_adapter.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_cota.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_fota.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_ipc.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_log_report.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_manager.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_message.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_message_cache.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_msg_process.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_opt.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_ota.c
    ${ALI_C_SDK_DIR}/src/dev_model/server/dm_server.c
    ${ALI_C_SDK_DIR}/src/dev_model/server/dm_server_adapter.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_shadow.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_tsl_alink.c
    ${ALI_C_SDK_DIR}/src/dev_model/dm_utils.c
    ${ALI_C_SDK_DIR}/src/dev_model/deprecated/impl_gateway.c
    ${ALI_C_SDK_DIR}/src/dev_model/impl_linkkit.c
    ${ALI_C_SDK_DIR}/src/dev_model/deprecated/impl_ntp.c
    ${ALI_C_SDK_DIR}/src/dev_model/deprecated/impl_solo.c
    ${ALI_C_SDK_DIR}/src/dev_model/iotx_cm.c
    ${ALI_C_SDK_DIR}/src/dev_model/iotx_cm_mqtt.c
    ${ALI_C_SDK_DIR}/src/dev_model/examples/cJSON.c
)
set(ALI_C_SDK_DYNAMIC_REGISTER ${ALI_C_SDK_DIR}/src/dynamic_register/dynreg.c)
set(ALI_C_SDK_INFRA ${ALI_C_SDK_DIR}/src/infra/infra_compat.c
    ${ALI_C_SDK_DIR}/src/infra/infra_defs.c
    ${ALI_C_SDK_DIR}/src/infra/infra_log.c
    ${ALI_C_SDK_DIR}/src/infra/infra_net.c
    ${ALI_C_SDK_DIR}/src/infra/infra_report.c
    ${ALI_C_SDK_DIR}/src/infra/infra_sha256.c
    ${ALI_C_SDK_DIR}/src/infra/infra_string.c
    ${ALI_C_SDK_DIR}/src/infra/infra_timer.c
    ${ALI_C_SDK_DIR}/src/infra/infra_cjson.c
    ${ALI_C_SDK_DIR}/src/infra/infra_httpc.c
)
set(ALI_C_SDK_DEV_SIGN ${ALI_C_SDK_DIR}/src/dev_sign/dev_sign_mqtt.c)
set(ALI_C_SDK_MQTT ${ALI_C_SDK_DIR}/src/mqtt/mqtt_api.c
    ${ALI_C_SDK_DIR}/src/mqtt/impl/iotx_mqtt_client.c
    ${ALI_C_SDK_DIR}/src/mqtt/impl/MQTTConnectClient.c
    ${ALI_C_SDK_DIR}/src/mqtt/impl/MQTTDeserializePublish.c
    ${ALI_C_SDK_DIR}/src/mqtt/impl/MQTTPacket.c
    ${ALI_C_SDK_DIR}/src/mqtt/impl/MQTTSerializePublish.c
    ${ALI_C_SDK_DIR}/src/mqtt/impl/MQTTSubscribeClient.c
    ${ALI_C_SDK_DIR}/src/mqtt/impl/MQTTUnsubscribeClient.c
)

target_sources(${target} PRIVATE
    ${ALI_C_SDK_ADAPTER}
    ${ALI_C_SDK_INFRA}
    ${ALI_C_SDK_DEV_SIGN}
    ${ALI_C_SDK_MQTT}
    ${ALI_C_SDK_CERTS}
    ${ALI_C_SDK_DEV_MODEL}
    ${ALI_C_SDK_DYNAMIC_REGISTER}
    ${ALI_C_SDK_DIR}/src/mqtt/examples/mqtt_example.c
    ${ALI_C_SDK_DIR}/src/dev_sign/examples/dev_sign_example.c
    ${ALI_C_SDK_DIR}/src/dev_model/examples/linkkit_example_solo.c
    ${ALI_C_SDK_DIR}/src/dynamic_register/examples/dynreg_example.c
)

if(CONFIG_APPIMG_LOAD_FLASH AND CONFIG_NET_ALI_SDK_DYNAMIC_LOAD)
    set(target app_img_ali_c_sdk)
    add_appimg_unittest(${target} ${ALI_C_SDK_DIR}/adapter/mqtt_app_img.c)
    target_include_targets(${target} PRIVATE kernel net lwip)
    target_include_directories(${target} PRIVATE ${ALI_C_SDK_DIR}/src/infra)
    target_link_group(${target} PRIVATE ali_c_sdk ${libc_file_name})
endif()

relative_glob(srcs ${ALI_C_SDK_DIR}/adapter/*.[ch])
beautify_c_code(${target} ${srcs})
