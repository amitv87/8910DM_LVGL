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

configure_file(include/mupnp_config.h.in ${out_inc_dir}/mupnp_config.h)

set(target mupnp)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_NET)
target_include_directories(${target} PUBLIC include src/mupnp/include)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_targets(${target} PRIVATE kernel driver calclib net lwip cfw atr)

if (CONFIG_QUEC_PROJECT_FEATURE_SSL)
target_include_targets(${target} PRIVATE mbedtls)
endif()

if (NOT CONFIG_QUEC_PROJECT_FEATURE)
target_include_targets(${target} PRIVATE http)
endif()

set(MUPMPDIR src/mupnp/src/mupnp)
set(HTTPFILES ${MUPMPDIR}/http/http.c
    ${MUPMPDIR}/http/http_date.c
    ${MUPMPDIR}/http/http_header.c
    ${MUPMPDIR}/http/http_header_list.c
    ${MUPMPDIR}/http/http_packet.c
    ${MUPMPDIR}/http/http_persistent_connection.c
    ${MUPMPDIR}/http/http_request.c
    ${MUPMPDIR}/http/http_response.c
    ${MUPMPDIR}/http/http_server.c
    ${MUPMPDIR}/http/http_server_list.c
)
set(UTILFILES ${MUPMPDIR}/util/time_wince.c
    ${MUPMPDIR}/util/mutex.c
    ${MUPMPDIR}/util/dictionary.c
    ${MUPMPDIR}/util/log.c
    ${MUPMPDIR}/util/thread_list.c
    ${MUPMPDIR}/util/time.c
    ${MUPMPDIR}/util/cond.c
    ${MUPMPDIR}/util/string_function.c
    ${MUPMPDIR}/util/list.c
    ${MUPMPDIR}/util/dictionary_elem.c
    ${MUPMPDIR}/util/string.c
    ${MUPMPDIR}/util/string_tokenizer.c
    ${MUPMPDIR}/util/thread.c
)
set(NETFILES ${MUPMPDIR}/net/interface.c
    ${MUPMPDIR}/net/socket_list.c
    ${MUPMPDIR}/net/datagram_packet.c
    ${MUPMPDIR}/net/interface_function.c
    ${MUPMPDIR}/net/mupnp_uri.c
    ${MUPMPDIR}/net/interface_list.c
    ${MUPMPDIR}/net/url.c
    ${MUPMPDIR}/net/socket.c
    ${MUPMPDIR}/net/net_function.c

)
set(XMLFILES ${MUPMPDIR}/xml/xml_node_list.c
    ${MUPMPDIR}/xml/xml_attribute_list.c
    ${MUPMPDIR}/xml/xml_parser_expat.c
    ${MUPMPDIR}/xml/xml_attribute.c
    ${MUPMPDIR}/xml/xml_parser.c
    ${MUPMPDIR}/xml/xml_parser_libxml2.c
    ${MUPMPDIR}/xml/xml_node.c
    ${MUPMPDIR}/xml/xml_function.c
)
# OTHERFILES: The files which under mupnp.
set(OTHERFILES ${MUPMPDIR}/argument.c
    ${MUPMPDIR}/io/file_list.c
    ${MUPMPDIR}/io/file.c
    ${MUPMPDIR}/upnp_function.c
    ${MUPMPDIR}/service_notify.c
    ${MUPMPDIR}/upnp_status.c
    ${MUPMPDIR}/argument_list.c
    ${MUPMPDIR}/controlpoint.c
    ${MUPMPDIR}/servicestate_table.c
    ${MUPMPDIR}/statevariable.c
    ${MUPMPDIR}/service.c
    ${MUPMPDIR}/device_advertiser.c
    ${MUPMPDIR}/action_list.c
    ${MUPMPDIR}/device.c
    ${MUPMPDIR}/icon_list.c
    ${MUPMPDIR}/device_list.c
    ${MUPMPDIR}/service_std_dcp.c
    ${MUPMPDIR}/controlpoint_event.c
    ${MUPMPDIR}/controlpoint_http_server.c
    ${MUPMPDIR}/action.c

)
# CONTROLFFILES:
set(CONTROLFFILES ${MUPMPDIR}/control/action_request.c
    ${MUPMPDIR}/control/action_ctrl.c
    ${MUPMPDIR}/control/action_response.c
    ${MUPMPDIR}/control/query_request.c
    ${MUPMPDIR}/control/query_response.c
    ${MUPMPDIR}/control/control_function.c
)
# EVENTFILES:
set(EVENTFILES ${MUPMPDIR}/event/eventlistener_list.c
    ${MUPMPDIR}/event/subscriber_list.c
    ${MUPMPDIR}/event/property.c
    ${MUPMPDIR}/event/subscription_request.c
    ${MUPMPDIR}/event/subscription.c
    ${MUPMPDIR}/event/notify_request.c
    ${MUPMPDIR}/event/property_list.c
    ${MUPMPDIR}/event/subscription_response.c
    ${MUPMPDIR}/event/subscriber.c
)
# MUPNPNOAPPSFILES: All MUPNP files without http
set(MUPNPNOAPPSFILES ${UTILFILES}
    ${NETFILES}
    ${OTHERFILES}
    ${CONTROLFFILES}
    ${EVENTFILES}
)

target_sources(${target} PRIVATE
    ${HTTPFILES}
    ${MUPNPNOAPPSFILES}
)

relative_glob(srcs include/*.h ${HTTPFILES} ${MUPNPNOAPPSFILES}
    src/mupmp/src/mupmp/*.h src/mupmp/src/mupmp/http/*.h)
beautify_c_code(${target} ${srcs})
