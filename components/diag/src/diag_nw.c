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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "diag_config.h"
#include "diag.h"
#include "osi_api.h"
#include "osi_log.h"
#include "drv_uart.h"
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#include "cmddef.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void DiagNwStatusInd(const void *data, unsigned sizet)
{
    diagMsgHead_t head = {
        .seq_num = 0,
        .len = sizet + sizeof(diagMsgHead_t),
        .type = DIAG_NETWORK_STATUS_IND_F,
        .subtype = 0,
    };

    diagOutputPacket2(&head, data, sizet);
}

/*
void at_test_network_status(void)
{
    char test_char[8]="ttttt\0\0\0";
    
    diagMsgHead_t head = {
        .seq_num = 0,
        .len = sizeof(test_char) + sizeof(diagMsgHead_t),
        .type = DIAG_NETWORK_STATUS_IND_F,
        .subtype = 0,
    };
    

   diagOutputPacket2(&head, test_char, sizeof(test_char));
}
*/

static bool _diagNw(const diagMsgHead_t *cmd, void *ctx)
{
    char *version = malloc(512);
    char *ver = version;

    ver += sprintf(ver, "\n%s", "_diagNw sample");

    size_t ver_str_len = strlen(version);
    diagOutputPacket2(cmd, version, ver_str_len);
    free(version);
    return true;
}

void diagNwInit(void)
{
    diagRegisterCmdHandle(DIAG_NETWORK_STATUS_IND_F, _diagNw, NULL);
}
