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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

#include <sys/socket.h>
#include "osi_api.h"
#include "osi_log.h"

extern int mqtt_test_main();

static void prvThreadEntry(void *param)
{
    OSI_LOGI(0, "application thread enter, param 0x%x", param);

    for (;;)
    {
        if (lwip_getDefaultNetif() == NULL)
        {
            osiThreadSleep(100);
        }
        else
        {
            osiThreadSleep(1000);
            break;
        }
    }
    mqtt_test_main();
    dynreg_test_main();
    dev_sign_test_main();
    linkkit_solo_testmain();
    osiThreadExit();
}

int appimg_enter(void *param)
{
    OSI_LOGI(0, "application image enter, param 0x%x", param);

    osiThreadCreate("mythread", prvThreadEntry, NULL, OSI_PRIORITY_BELOW_NORMAL, 1024 * 20, 64);
    return 0;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}
