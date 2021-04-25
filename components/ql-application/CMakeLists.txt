# Copyright (C) 2020 QUECTEL Technologies Limited and/or its affiliates("QUECTEL").
# All rights reserved.
#

add_subdirectory_if_exist(init)

add_subdirectory_if_exist(nw)

add_subdirectory_if_exist(peripheral)

add_subdirectory_if_exist(osi)

add_subdirectory_if_exist(dev)

add_subdirectory_if_exist(power)


if(QL_APP_FEATURE_FILE)
add_subdirectory_if_exist(fs)
endif()

if(QL_APP_FEATURE_AUDIO)
add_subdirectory_if_exist(audio)
if(QL_APP_FEATURE_AUDIO_TTS)
add_subdirectory_if_exist(tts)
endif()
endif()

if(QL_APP_FEATURE_BT)
add_subdirectory_if_exist(bt)
endif()

if(QL_APP_FEATURE_GNSS)
add_subdirectory_if_exist(gnss)
endif()

if(QL_APP_FEATURE_FTP)
add_subdirectory_if_exist(ftp)
endif()

if(QL_APP_FEATURE_HTTP)
add_subdirectory_if_exist(http)
endif()

if(QL_APP_FEATURE_MMS)
add_subdirectory_if_exist(mms)
endif()

if(QL_APP_FEATURE_MQTT)
add_subdirectory_if_exist(mqtt)
endif()

if(QL_APP_FEATURE_SSL)
add_subdirectory_if_exist(ssl)
endif()

if(QL_APP_FEATURE_PING)
add_subdirectory_if_exist(ping)
endif()

if(QL_APP_FEATURE_NTP)
add_subdirectory_if_exist(ntp)
endif()

if(QL_APP_FEATURE_LBS)
add_subdirectory_if_exist(lbs)
endif()

if(QL_APP_FEATURE_CTSREG)
add_subdirectory_if_exist(ctsreg)
endif()

if(QL_APP_FEATURE_SOCKET)
add_subdirectory_if_exist(socket)
endif()

if(QL_APP_FEATURE_LCD)
add_subdirectory_if_exist(lcd)
if(QL_APP_FEATURE_LVGL)
if(QL_APP_FEATURE_LVGL_V7)
add_subdirectory_if_exist(lvgl7_lib)
add_subdirectory_if_exist(lv_widgets)
else()
add_subdirectory_if_exist(lvgl_lib)
endif()
add_subdirectory_if_exist(lvgl)
endif()
endif()

if(QL_APP_FEATURE_CAMERA)
add_subdirectory_if_exist(camera)
endif()

if(QL_APP_FEATURE_WIFISCAN)
add_subdirectory_if_exist(wifi_scan)
endif()

if(QL_APP_FEATURE_SIM)
add_subdirectory_if_exist(sim)
endif()

if(QL_APP_FEATURE_SMS)
add_subdirectory_if_exist(sms)
endif()

if(QL_APP_FEATURE_VOICE_CALL)
add_subdirectory_if_exist(voice_call)
endif()

if(QL_APP_FEATURE_VOLTE)
add_subdirectory_if_exist(volte)
endif()

if(QL_APP_FEATURE_SPI)
add_subdirectory_if_exist(spi)
endif()

if(QL_APP_FEATURE_SPI_FLASH)
add_subdirectory_if_exist(spi_flash)
endif()

if(QL_APP_FEATURE_HTTP_FOTA)
add_subdirectory_if_exist(http_fota)
endif()

if(QL_APP_FEATURE_DECODER)
add_subdirectory_if_exist(decoder)
endif()

if(QL_APP_FEATURE_RTC)
add_subdirectory_if_exist(rtc)
endif()

if(QL_APP_FEATURE_VIRT_AT)
add_subdirectory_if_exist(virt_at)
endif()

if(QL_APP_FEATURE_ALI_LINKSDK)
add_subdirectory_if_exist(LinkSDK)
endif()

add_subdirectory_if_exist(ctp)


add_subdirectory_if_exist(function)