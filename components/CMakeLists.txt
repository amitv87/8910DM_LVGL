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

add_subdirectory_if_exist(appstart)
add_subdirectory_if_exist(apploader)
add_subdirectory_if_exist(atr)
add_subdirectory_if_exist(ats)
add_subdirectory_if_exist(audio)
add_subdirectory_if_exist(aworker)
add_subdirectory_if_exist(bootloader)
add_subdirectory_if_exist(calclib)
add_subdirectory_if_exist(cfw)
add_subdirectory_if_exist(mal)
add_subdirectory_if_exist(diag)
add_subdirectory_if_exist(driver)
add_subdirectory_if_exist(firmwareupdate)
add_subdirectory_if_exist(fs)
add_subdirectory_if_exist(hal)
add_subdirectory_if_exist(kernel)
if(NOT CONFIG_QUEC_PROJECT_FEATURE)
add_subdirectory_if_exist(libjpeg-turbo)
endif()
add_subdirectory_if_exist(libs)
add_subdirectory_if_exist(ml)
add_subdirectory_if_exist(modem)
add_subdirectory_if_exist(nanopb)
add_subdirectory_if_exist(net)
add_subdirectory_if_exist(nvm)
add_subdirectory_if_exist(rpc)
add_subdirectory_if_exist(service)
add_subdirectory_if_exist(unity)
add_subdirectory_if_exist(misc)
add_subdirectory_if_exist(connectivity)
if(NOT CONFIG_QUEC_PROJECT_FEATURE)
add_subdirectory_if_exist(tts)
#add_subdirectory_if_exist(littlevgl)
endif()
add_subdirectory_if_exist(tb)
add_subdirectory_if_exist(bt_app)
add_subdirectory_if_exist(lpa)
add_subdirectory_if_exist(console)
if (NOT QL_CCSDK_BUILD)
add_subdirectory_if_exist(quectel)
else()
add_subdirectory_if_exist(ql-application)
add_subdirectory_if_exist(ql-kernel)
endif()