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

#ifndef _DIAG_INTERNAL_H_
#define _DIAG_INTERNAL_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "diag_config.h"
#include "diag.h"

#ifdef __cplusplus
extern "C" {
#endif

void Layer1_RegCaliDiagHandle(void);
void diagCurrentTestInit(void);
void diagSwverInit(void);
void diagLogInit(void);
void diagNvmInit(void);
void diagNvInit(void);
void diagSysInit(void);
void diagNvProductInit(void);
void diagPsInit(void);
void diagChargeInit(void);

#ifdef __cplusplus
}
#endif
#endif
