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

if(CONFIG_SOC_8910)
    include(${CMAKE_CURRENT_LIST_DIR}/chip_8910.cmake)
elseif(CONFIG_SOC_6760)
    include(${CMAKE_CURRENT_LIST_DIR}/chip_6760.cmake)
elseif(CONFIG_SOC_8955)
    include(${CMAKE_CURRENT_LIST_DIR}/chip_8955.cmake)
elseif(CONFIG_SOC_8909)
    include(${CMAKE_CURRENT_LIST_DIR}/chip_8909.cmake)
else()
    message(FATAL "Unknown chip")
endif()
