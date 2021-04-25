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

#ifndef _DRV_MD_NVM_H_
#define _DRV_MD_NVM_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct diagMsgHead;

/**
 * initialize NVM IPC driver
 *
 * NVM IPC will run on separated thread. It will register notify from IPC
 * driver.
 */
void drvNvmIpcInit(void);

void drvNvmSendCalibCmd(const struct diagMsgHead *cmd);

#ifdef __cplusplus
}
#endif

#endif
