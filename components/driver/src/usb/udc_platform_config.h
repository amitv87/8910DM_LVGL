/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _UDC_PLATFORM_CONFIG_H_
#define _UDC_PLATFORM_CONFIG_H_

#include "hal_config.h"

#if defined(CONFIG_SOC_8910)
#define UDC_IEP_COUNT 12
#define UDC_OEP_COUNT 12
#elif defined(CONFIG_SOC_8955)
#define UDC_IEP_COUNT 3
#define UDC_OEP_COUNT 2
#elif defined(CONFIG_SOC_8909)
#define UDC_IEP_COUNT 5
#define UDC_OEP_COUNT 5
#endif

#endif
