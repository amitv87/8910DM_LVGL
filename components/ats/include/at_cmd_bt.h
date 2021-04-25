/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef __AT_CMD_BT_H__
#define __AT_CMD_BT_H__
#include "at_command.h"

#define ATC_MAX_BT_ADDRESS_STRING_LEN 20

#define ATC_MAX_BT_NAME_STRING_LEN 164 //BT_DEVICE_NAME_SIZE = 41

#define ATC_BT_NAME_ASCII 0 //the bt name encode in ascii
#define ATC_BT_NAME_UCS2 1  //the bt name encode in ucs2

#define ATC_SPBBTTEST_MAX_LEN 50

void atCmdHandleSPBTCTRL(atCommand_t *pParam);

#endif
