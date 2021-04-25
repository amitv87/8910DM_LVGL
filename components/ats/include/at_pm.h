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

#ifndef __AT_PM_H__
#define __AT_PM_H__

#include "at_engine.h"
#include "at_command.h"
#include "at_response.h"

#ifdef __cplusplus
extern "C" {
#endif
void atNotcieClientPmInfo(const osiEvent_t *event);
void atPmInit(void);

#ifdef __cplusplus
}
#endif
#endif
