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

set(AT_MANUFACTURER_ID "UNISOC")
set(AT_MODEL_ID "${BUILD_TARGET}-${BUILD_RELEASE_TYPE}")
set(AT_REVISION_ID "${BUILD_REVISION}-${BUILD_AUTO_REVISION}")

configure_file(include/ats_config.h.in ${out_inc_dir}/ats_config.h)

set(at_apn_table_h ${CMAKE_CURRENT_BINARY_DIR}/at_apn_table.h)
set_if(apn_csv CONFIG_AT_GLOBAL_APN_SUPPORT
    THEN apn/global_apn.csv
    OTHERWISE apn/local_apn.csv
)
add_custom_command(OUTPUT ${at_apn_table_h}
    COMMAND python3 apn/csv_parse.py ${apn_csv} ${at_apn_table_h}
    DEPENDS apn/csv_parse.py ${apn_csv}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(target ats)
add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_include_directories(${target} PUBLIC include)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_targets(${target} PRIVATE
    kernel driver ml hal calclib atr cfw fs fsmount bdev
    aworker net lwip mal diag
    nvm fupdate audio service lpa
)

if (NOT CONFIG_QUEC_PROJECT_FEATURE)
    target_include_targets(${target} PRIVATE tts)
endif()

if (CONFIG_QUEC_PROJECT_FEATURE_SSL)
target_include_targets(${target} PRIVATE mbedtls)
endif()

target_sources(${target} PRIVATE
    ${at_apn_table_h}
    cfw/at_cfw.c
    gc/at_cmd_gc.c
    id/at_cmd_id.c
    nw/at_cmd_nw.c
    cc/at_cmd_cc.c
    gprs/at_cmd_gprs.c
    pbk/at_cmd_pbk.c
    nv/at_cmd_nv.c
    ss/at_cmd_ss.c
    ss/at_ss_api.c
    sat/at_cmd_sat.c
    sim/at_cmd_sim.c
    sim/at_cmd_vsim.c
    misc/at_cmd_debug.c
    misc/at_cmd_sysnv.c
    emod/at_cmd_emod.c
    audio/at_cmd_audio.c
	tcpip/at_cmd_netif.c
    sms/at_cmd_sms.c
    battery/at_cmd_battery.c
    ssim/at_cmd_ssim.c
    simlock/at_cmd_simlock.c
    lpa/at_cmd_lpa.c
)

if (NOT CONFIG_QUEC_PROJECT_FEATURE)
target_sources(${target} PRIVATE gc/at_cmd_fupdate.c)
target_sources(${target} PRIVATE myinfo/at_cmd_myinfo.c)
target_sources(${target} PRIVATE fs/at_cmd_fs.c)
target_sources_if(CONFIG_TTS_SUPPORT THEN ${target} PRIVATE audio/at_cmd_tts.c)
target_sources(${target} PRIVATE tcpip/at_cmd_tcpip.c tcpip/at_cmd_udp.c tcpip/at_cmd_mynet.c tcpip/at_cmd_ping.c)
target_sources(${target} PRIVATE sntp/at_cmd_sntp.c)
target_sources(${target} PRIVATE alic/at_cmd_alic.c)
target_sources(${target} PRIVATE http/at_cmd_http.c)
target_sources(${target} PRIVATE coap/at_cmd_coap.c)
target_sources(${target} PRIVATE tcpip/at_cmd_ssl.c)
target_sources_if(CONFIG_AT_LWIP_MQTT_SUPPORT THEN ${target} PRIVATE mqtt/at_cmd_mqtt.c)
target_sources_if(CONFIG_AT_CISSDK_MIPL_SUPPORT THEN ${target} PRIVATE lwm2m/at_cmd_cissdk_mipl.c)
target_sources_if(CONFIG_AT_DM_LWM2M_SUPPORT THEN ${target} PRIVATE dm/at_cmd_dm_lwm2m.c)
target_sources_if(CONFIG_AT_OCEANCONNECT_SUPPORT THEN ${target} PRIVATE lwm2m/at_cmd_oceanconnect.c)
target_sources_if(CONFIG_AT_PAHO_MQTT_SUPPORT THEN ${target} PRIVATE mqtt/at_cmd_pahomqtt.c)
target_sources_if(CONFIG_AT_CAMERA_SUPPORT THEN ${target} PRIVATE cam/at_cmd_camera.c)
endif()
target_sources_if(CONFIG_SOC_8910 THEN ${target} PRIVATE bt/at_cmd_bt.c)

target_sources_if(CONFIG_AT_BLUEU_VERIFY_SUPPORT THEN ${target} PRIVATE bt/at_cmd_blueu_verify.c)
target_sources_if(CONFIG_AT_BT_APP_SUPPORT THEN ${target} PRIVATE bt/at_cmd_bt_app.c)

target_sources_if(CONFIG_AT_BT_CLASSIC_SUPPORT THEN ${target} PRIVATE bt/at_cmd_bt_classic.c)
target_sources_if(CONFIG_AT_BT_CLASSIC_SUPPORT THEN ${target} PRIVATE bt/at_cmd_btmsg.c)

target_sources_if(CONFIG_AT_WIFISCAN_SUPPORT THEN ${target} PRIVATE wifi/at_cmd_wifi.c)
target_sources_if(CONFIG_AT_FTP_SUPPORT THEN ${target} PRIVATE ftp/at_cmd_ftp.c)
target_sources_if(CONFIG_AT_NEWFTP_SUPPORT THEN ${target} PRIVATE ftp/at_cmd_newftp.c)
target_sources_if(CONFIG_AT_MYFTP_SUPPORT THEN ${target} PRIVATE ftp/at_cmd_myftp.c)
target_sources_if(CONFIG_AT_NEWMYFTP_SUPPORT THEN ${target} PRIVATE ftp/at_cmd_newMyftp.c)
target_sources_if(CONFIG_AT_U_CREATE_AT_CHANNEL THEN ${target} PRIVATE misc/at_cmd_create_atch.c)
if (NOT CONFIG_QUEC_PROJECT_FEATURE)
target_include_targets(${target} PRIVATE libcoap)
target_include_targets(${target} PRIVATE http mupnp)
endif()
target_include_targets_if(CONFIG_NET_LWM2M_SUPPORT THEN ${target} PRIVATE lwm2m)
target_include_targets_if(CONFIG_FTP_MODULE_SUPPORT THEN ${target} PRIVATE ftp)
target_include_targets_if(CONFIG_NET_CIS_SDK_SUPPORT THEN ${target} PRIVATE cis_sdk)
target_include_targets_if(CONFIG_NET_PAHO_MQTT_SUPPORT THEN ${target} PRIVATE pahomqtt)
target_include_targets_if(CONFIG_NET_ALI_SDK_SUPPORT THEN ${target} PRIVATE ali_c_sdk)
target_include_targets_if(CONFIG_BLUEU_BT_ENABLE THEN ${target} PRIVATE blueu)
target_include_targets_if(CONFIG_AT_BT_APP_SUPPORT THEN ${target} PRIVATE bt_app)

# TODO: it should be remove, avoid to access internal files directly
target_include_directories(${target} PRIVATE ../atr/src)
target_include_directories(${target} PRIVATE ../bt_app/include)
target_include_directories(${target} PRIVATE ../bt_app/bt_bsp/nv/include)
target_include_directories(${target} PRIVATE ../connectivity/bt/include/bluetooth)
target_include_directories(${target} PRIVATE ../connectivity/bt/include/test)

relative_glob(srcs */*.[ch])
beautify_c_code(${target} ${srcs})
