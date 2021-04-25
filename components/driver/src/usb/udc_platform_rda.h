/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _USB__UDC_PLATFORM_H_
#define _USB__UDC_PLATFORM_H_

#include <usb/usb_device.h>

bool udcPlatInit_rda(udc_t *udc);
void udcPlatExit_rda(udc_t *udc);

#endif // _USB__UDC_PLATFORM_H_
