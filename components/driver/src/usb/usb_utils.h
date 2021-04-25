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

#ifndef _USB__UTILS_H_
#define _USB__UTILS_H_

#include <stdlib.h>
#include <string.h>

#ifndef cpu_to_le32
#define cpu_to_le32(x) (x)
#endif

#ifndef cpu_to_le16
#define cpu_to_le16(x) (x)
#endif

#ifndef le16_to_cpu
#define le16_to_cpu(x) (x)
#endif

#endif // _USB__UTILS_H_
