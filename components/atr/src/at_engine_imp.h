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

#ifndef _AT_ENGINE_IMP_H_
#define _AT_ENGINE_IMP_H_

#include "atr_config.h"
#include "at_engine.h"
#include "at_command.h"
#include "osi_api.h"
#include "osi_fifo.h"
#include "osi_mem_recycler.h"
#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t *buff;
    size_t size;
    size_t len;
} atByteBuff_t;

static inline void atByteBuffPop(atByteBuff_t *p)
{
    if (p->len > 0)
        p->len--;
}

static inline void atByteBuffPush(atByteBuff_t *p, uint8_t c)
{
    if (p->len < p->size)
        p->buff[p->len++] = c;
}

static inline uint8_t atByteBuffTail(atByteBuff_t *p) { return p->len > 0 ? p->buff[p->len - 1] : 0; }
static inline void atByteBuffClear(atByteBuff_t *p) { p->len = 0; }
static inline bool atByteBuffIsFull(atByteBuff_t *p) { return p->len >= p->size; }
static inline bool atByteBuffIsEmpty(atByteBuff_t *p) { return p->len == 0; }

struct atEngine
{
    osiThread_t *thread_id;
    osiEventDispatch_t *id_man; // register by event ID
    osiEventHub_t *event_hub;   // static dispatch
    osiMemRecycler_t *mem_recycler;
};

void atCmdCreateOutputLineCache(unsigned size);
void atCmdWriteFlush(atCmdEngine_t *cmd);
void atCmdWriteFlushAll(void);

#ifdef __cplusplus
}
#endif
#endif
