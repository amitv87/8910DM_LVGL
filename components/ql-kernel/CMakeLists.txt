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

add_subdirectory_if_exist(common)
add_subdirectory_if_exist(dev)
add_subdirectory_if_exist(drivers)
add_subdirectory_if_exist(decoder)
if(CONFIG_QUEC_PROJECT_FEATURE_FOTA)
add_subdirectory_if_exist(fota)
endif()
if(CONFIG_QUEC_PROJECT_FEATURE_FILE)
add_subdirectory_if_exist(fs)
endif()

add_subdirectory_if_exist(network)
add_subdirectory_if_exist(osi)

if(CONFIG_QUEC_PROJECT_FEATURE_SIM)
add_subdirectory_if_exist(sim)
endif()

if(CONFIG_QUEC_PROJECT_FEATURE_FTP)
add_subdirectory_if_exist(ftp)
endif()

if(CONFIG_QUEC_PROJECT_FEATURE_AUDIO)
add_subdirectory_if_exist(audio)
endif()
if(CONFIG_QUEC_PROJECT_FEATURE_HTTP)
add_subdirectory_if_exist(http)
endif()
if(CONFIG_QUEC_PROJECT_FEATURE_MQTT)
add_subdirectory_if_exist(mqtt)
endif()
#add_subdirectory_if_exist(third_party)
if(CONFIG_QUEC_PROJECT_FEATURE_BT)
add_subdirectory_if_exist(bt)
endif()
add_subdirectory_if_exist(power)
if(CONFIG_QUEC_PROJECT_FEATURE_SMS)
add_subdirectory_if_exist(sms)
endif()
if(CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL)
add_subdirectory_if_exist(voice_call)
endif()
if(CONFIG_QUEC_PROJECT_FEATURE_VOLTE)
add_subdirectory_if_exist(volte)
endif()
if(CONFIG_QUEC_PROJECT_FEATURE_SPI)
add_subdirectory_if_exist(spi)
endif()
if(CONFIG_QUEC_PROJECT_FEATURE_SPI_FLASH)
add_subdirectory_if_exist(spi_flash)
endif()
if(CONFIG_QUEC_PROJECT_FEATURE_CAMERA)
add_subdirectory_if_exist(camera)
endif()
if(CONFIG_QUEC_PROJECT_FEATURE_RTC)
add_subdirectory_if_exist(rtc)
endif()
if(CONFIG_QUEC_PROJECT_FEATURE_VIRT_AT)
add_subdirectory_if_exist(virt_at)
endif()

