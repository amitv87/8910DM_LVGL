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

set(target libmqttsn)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE AT_MQTTSN_SUPPORT=1)
target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} PRIVATE kernel driver calclib net lwip cfw)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(${target} PROPERTIES OUTPUT_NAME mqttsn)

target_include_directories(${target} PRIVATE ../external/platform/include)
target_include_directories(${target} PRIVATE src/paho.mqtt-sn.embedded-c/MQTTSNPacket/src)
#target_include_directories(${target} PRIVATE ../../posix/include)
#target_include_directories(${target} PRIVATE ../lwip/src/include)

set(LIBMQTTSNDIR src/paho.mqtt-sn.embedded-c/MQTTSNPacket/src)
set(LIBMQTTSNFILES 
    ${LIBMQTTSNDIR}/MQTTSNConnectClient.c
    ${LIBMQTTSNDIR}/MQTTSNConnectServer.c
    ${LIBMQTTSNDIR}/MQTTSNDeserializePublish.c
    ${LIBMQTTSNDIR}/MQTTSNPacket.c
    ${LIBMQTTSNDIR}/MQTTSNSearchClient.c
    ${LIBMQTTSNDIR}/MQTTSNSearchServer.c
    ${LIBMQTTSNDIR}/MQTTSNSerializePublish.c
    ${LIBMQTTSNDIR}/MQTTSNSubscribeClient.c
    ${LIBMQTTSNDIR}/MQTTSNSubscribeServer.c
    ${LIBMQTTSNDIR}/MQTTSNUnsubscribeClient.c
    ${LIBMQTTSNDIR}/MQTTSNUnsubscribeServer.c
    src/MQTTSNApi.c
)

target_sources(${target} PRIVATE
    ${LIBMQTTSNFILES}
)

relative_glob(srcs include/*.h src/MQTTSNApi.c)
beautify_c_code(${target} ${srcs})
