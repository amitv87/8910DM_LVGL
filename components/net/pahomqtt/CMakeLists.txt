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

set(target pahomqtt)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
# target_include_directories(${target} PUBLIC include)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(${target} PROPERTIES OUTPUT_NAME pahomqtt)

target_include_directories(${target} PUBLIC src/paho.mqtt.c-master/src)
target_include_directories(${target} PRIVATE src/unisoc/adapter/posix/include)
target_include_directories(${target} PRIVATE src/unisoc/adapter/paho/include)
target_include_targets(${target} PRIVATE kernel net lwip)

set(EXDIR src/unisoc/adapter/posix/system)
set(EXFILES
    ${EXDIR}/Adapter.c
)

set(UNITTESTDIR src/unisoc/adapter/posix/unit_test)
set(UNITTESTFILES
    ${UNITTESTDIR}/unit_test.c
)

set(DEMODIR src/unisoc/demo)
set(DEMOFILES
    ${DEMODIR}/demo_system.c
    ${DEMODIR}/demo_client.c
)

set(TESTDIR src/unisoc/test)
set(TESTFILES
    ${TESTDIR}/paho_mqtt_lib_test.c
)

set(LIBPAHOMQTTDIR src/paho.mqtt.c-master/src)
set(PAHOMQTTFILES
    ${LIBPAHOMQTTDIR}/Thread.c
    ${LIBPAHOMQTTDIR}/LinkedList.c
    ${LIBPAHOMQTTDIR}/Base64.c
    ${LIBPAHOMQTTDIR}/SHA1.c
    ${LIBPAHOMQTTDIR}/utf-8.c
    ${LIBPAHOMQTTDIR}/SocketBuffer.c
    ${LIBPAHOMQTTDIR}/Socket.c
    ${LIBPAHOMQTTDIR}/WebSocket.c
    ${LIBPAHOMQTTDIR}/FakerLog.c
    ${LIBPAHOMQTTDIR}/MQTTProperties.c
    ${LIBPAHOMQTTDIR}/Clients.c
    ${LIBPAHOMQTTDIR}/MQTTProtocolClient.c
    ${LIBPAHOMQTTDIR}/MQTTProtocolOut.c
    ${LIBPAHOMQTTDIR}/MQTTPacket.c
    ${LIBPAHOMQTTDIR}/MQTTPacketOut.c
    ${LIBPAHOMQTTDIR}/MQTTReasonCodes.c
    ${LIBPAHOMQTTDIR}/MQTTAsync.c
)

target_sources(${target} PRIVATE
    ${EXFILES}
    ${PAHOMQTTFILES}
    ${UNITTESTFILES}
    ${DEMOFILES}
    ${TESTFILES}
)

# file(GLOB srcs include/*.h src/coap_async_api.c)
# beautify_c_code(${target} ${srcs})
