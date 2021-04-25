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

#ifndef _LWIPPORTS_H_
#define _LWIPPORTS_H_

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "osi_api.h"

#define RESOLV_NETWORK_ERROR 0x01

#define LWIP_RAND() ((uint32_t)rand())

#ifdef __GNUC__
#define UNUSED_PARAM __attribute__((unused))
#else /* not a GCC */
#define UNUSED_PARAM
#endif /* GCC */

#define UNUSED_P(p) ((void)p)

#include "cfw.h"
#include "cfw_event.h"

#define X8_F "02x"
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"
#define SZT_F "ld"

typedef struct
{
    uint32_t sem_id;
    uint32_t is_vail;
    uint32_t is_timeout;
    uint32_t wait_count;
} sys_sem_t, *sys_sem_ptr;

typedef struct
{
    uint32_t id;
    uint32_t is_vail;
} sys_mbox_t, *sys_mbox_ptr;

typedef struct
{
    uint32_t id;
    uint32_t user_id;
} sys_mutex_t;

#define sys_thread_t uint32_t
#define sys_prot_t uint32_t

#define sys_msleep osiThreadSleep
#define SYS_ARCH_DECL_PROTECT(x) uint32_t x
#define SYS_ARCH_PROTECT(x) x = osiEnterCritical()
#define SYS_ARCH_UNPROTECT(x) osiExitCritical(x)

typedef void (*lwip_thread_fn)(void *arg);
typedef void (*sys_timeout_handler)(void *arg);

void sys_timeout(uint32_t msecs, sys_timeout_handler handler, void *arg);
void sys_untimeout(sys_timeout_handler handler, void *arg);
void sys_settime(uint32_t sec, uint32_t frac);
uint32_t sys_get_srand();
#endif
