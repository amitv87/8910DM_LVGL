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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'Y', 'A', 'P')

#include "osi_log.h"
#include "osi_api.h"

static void prvInvokeGlobalCtors(void)
{
    extern void (*__init_array_start[])();
    extern void (*__init_array_end[])();

    size_t count = __init_array_end - __init_array_start;
    for (size_t i = 0; i < count; ++i)
        __init_array_start[i]();
}

static void prvThreadEntry(void *param)
{
    OSI_LOGI(0, "application thread enter, param 0x%x", param);

    for (int n = 0; n < 10; n++)
    {
        OSI_LOGI(0, "hello world %d", n);
        osiThreadSleep(500);
    }

    osiThreadExit();
}

int appimg_enter(void *param)
{
    OSI_LOGI(0, "application image enter, param 0x%x", param);

    prvInvokeGlobalCtors();

    osiThreadCreate("mythread", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024, 0);
    return 0;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}
