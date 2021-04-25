/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('C', 'P', 'I', 'O')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "cpio_parser.h"
#include "osi_log.h"
#include "osi_api.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#define CPIO_TRAILER "TRAILER!!!"
#define STREAM_INC(data, len, inc) \
    do                             \
    {                              \
        data += inc;               \
        len -= inc;                \
    } while (0)

typedef struct
{
    uint8_t magic[2];
    uint16_t dev_num;
    uint16_t inode_num;
    uint16_t mode;
    uint16_t user_id;
    uint16_t group_id;
    uint16_t num_link;
    uint16_t rdev_num;
    uint32_t modify_time;
    uint16_t path_len;
    uint32_t data_size;
} __attribute__((packed)) cpioOldBinLE_t;

typedef TAILQ_ENTRY(cpio_file) cpioFileEntry_t;
typedef TAILQ_HEAD(, cpio_file) cpioFileList_t;

typedef enum
{
    PHASE_NAME,
    PHASE_SKIP,
    PHASE_DATA,
} cpioFileParserPhase_t;

typedef struct cpio_file
{
    cpioFile_t file;
    cpioOldBinLE_t head;
    cpioFileParserPhase_t phase;
    cpioFileEntry_t entry;
} cpioFilePriv_t;

struct cpio_stream
{
    cpioStreamCfg_t cfg;
    cpioFileList_t store_list;
    cpioFileList_t used_list;
    cpioOldBinLE_t process_head;
    cpioFilePriv_t *process_file;
    uint32_t process_size;
    uint32_t skip_size;
    uint32_t file_count;
};

static cpioFilePriv_t *prvRawToCpioFile(cpioFile_t *raw)
{
    return (cpioFilePriv_t *)raw;
}

cpioStream_t *cpioStreamCreate(const cpioStreamCfg_t *cfg)
{
    cpioStream_t *s = calloc(1, sizeof(cpioStream_t));
    if (s == NULL)
        return NULL;
    TAILQ_INIT(&s->store_list);
    TAILQ_INIT(&s->used_list);
    s->cfg = *cfg;
    return s;
}

void cpioStreamDestroy(cpioStream_t *s)
{
    if (s == NULL)
        return;

    if (s->process_file != NULL)
    {
        free(s->process_file);
        s->process_file = NULL;
    }

    cpioFilePriv_t *f;
    while ((f = TAILQ_FIRST(&s->store_list)) != NULL)
    {
        TAILQ_REMOVE(&s->store_list, f, entry);
        free(f);
    }

    while ((f = TAILQ_FIRST(&s->used_list)) != NULL)
    {
        TAILQ_REMOVE(&s->used_list, f, entry);
        free(f);
    }
    free(s);
}

static void prvPushHead(cpioStream_t *s, uint8_t *data, uint32_t len);

static void prvPushData(cpioStream_t *s, uint8_t *data, uint32_t len)
{
    cpioFilePriv_t *f = s->process_file;
    while (len > 0)
    {
        if (f->phase == PHASE_NAME)
        {
            uint32_t fnlen = f->head.path_len - s->process_size;
            if (fnlen > len)
                fnlen = len;
            memcpy(f->file.name + s->process_size, data, fnlen);
            STREAM_INC(data, len, fnlen);
            s->process_size += fnlen;
            if (s->process_size == f->head.path_len)
            {
                s->process_size = 0;
                f->phase = f->head.path_len % 2 ? PHASE_SKIP : PHASE_DATA;
            }
        }
        else if (f->phase == PHASE_DATA)
        {
            uint32_t datalen = f->head.data_size - s->process_size;
            if (datalen > len)
                datalen = len;
            memcpy(f->file.data + s->process_size, data, datalen);
            STREAM_INC(data, len, datalen);
            s->process_size += datalen;
            if (s->process_size == f->head.data_size)
            {
                s->process_size = 0;
                if (strcmp(CPIO_TRAILER, f->file.name) == 0)
                {
                    free(s->process_file);
                    s->process_file = NULL;
                }
                else
                {
                    TAILQ_INSERT_TAIL(&s->store_list, f, entry);
                    s->process_file = NULL;
                    s->file_count += 1;
                }
                break;
            }
        }
        else if (f->phase == PHASE_SKIP)
        {
            STREAM_INC(data, len, 1);
            f->phase = PHASE_DATA;
        }
    }

    if (len > 0)
    {
        prvPushHead(s, data, len);
    }
}

static inline uint32_t prvMoLeToUint32(uint32_t val)
{
    return (((val & 0xffff) << 16) | ((val & 0xffff0000) >> 16));
}

static int prvCheckExpect(uint8_t *data, uint32_t len, uint8_t expect)
{
    bool ok = false;
    int c = 0;
    OSI_LOGV(0, "cpio check expect(%x/%u)", expect, len);
    for (c = 0; c < len; ++c)
    {
        if (data[c] == expect)
        {
            ok = true;
            break;
        }
    }
    return ok ? (c + 1) : -1;
}

static void prvSkipFile(cpioStream_t *s, cpioOldBinLE_t *h, uint8_t *data, uint32_t *len)
{
    s->skip_size = h->data_size + h->path_len + (h->data_size % 2) + (h->path_len % 2);
    uint32_t skip = s->skip_size;
    if (skip > *len)
        skip = *len;
    STREAM_INC(data, *len, skip);
    s->skip_size -= skip;
}

static void prvPushHead(cpioStream_t *s, uint8_t *data, uint32_t len)
{
    const uint8_t magic[2] = {0xc7, 0x71};
    while (len > 0)
    {
        for (; s->process_size < 2 && data != NULL && len > 0;)
        {
            int r = prvCheckExpect(data, len, magic[s->process_size]);
            if (r < 0)
                return;

            ((uint8_t *)&s->process_head)[s->process_size] = magic[s->process_size];
            s->process_size += 1;
            STREAM_INC(data, len, r);
        }

        OSI_LOGV(0, "cpio push head got maigic(%u/%u)", len, s->process_size);
        if (len > 0)
        {
            uint32_t head_expect = sizeof(cpioOldBinLE_t) - s->process_size;
            if (head_expect > len)
                head_expect = len;
            memcpy(((uint8_t *)&s->process_head) + s->process_size, data, head_expect);
            s->process_size += head_expect;
            STREAM_INC(data, len, head_expect);
        }

        if (s->process_size == sizeof(cpioOldBinLE_t))
        {
            cpioOldBinLE_t *h = &s->process_head;
            h->data_size = prvMoLeToUint32(h->data_size);
            s->process_size = 0;
            OSI_LOGV(0, "cpio parser got header(%p/%u/%u)", h, h->path_len, h->data_size);
            if (h->data_size > s->cfg.file_size_max || h->path_len > s->cfg.file_path_max)
            {
                OSI_LOGE(0, "cpio file too large. (%u/%u)", h->path_len, h->data_size);
                prvSkipFile(s, h, data, &len);
            }
            else
            {
                const unsigned alloc_size = sizeof(cpioFilePriv_t) + h->data_size + h->path_len;
                cpioFilePriv_t *f = (cpioFilePriv_t *)malloc(alloc_size);
                if (f == NULL)
                {
                    OSI_LOGE(0, "cpio allocate file fail.(%u)", alloc_size);
                    prvSkipFile(s, h, data, &len);
                }
                else
                {
                    f->head = *h;
                    f->phase = PHASE_NAME;
                    f->file.data_size = h->data_size;
                    f->file.name = (char *)f + sizeof(cpioFilePriv_t);
                    f->file.data = (uint8_t *)f->file.name + h->path_len;
                    f->file.mode = h->mode;
                    s->process_file = f;
                    break;
                }
            }
        }
    }

    if (s->process_file && len > 0)
        prvPushData(s, data, len);
}

void cpioStreamPushData(cpioStream_t *s, void *data, uint32_t len)
{
    if (s == NULL || data == NULL || len == 0)
        return;

    OSI_LOGV(0, "cpio push data(pushed %u, current %u, skip %u)", len, s->process_size, s->skip_size);
    if (s->skip_size > 0)
    {
        uint32_t skip = len;
        if (skip > s->skip_size)
            skip = s->skip_size;
        STREAM_INC(data, len, skip);
        s->skip_size -= skip;
    }

    if (len > 0)
    {
        if (s->process_file == NULL)
            prvPushHead(s, data, len);
        else
            prvPushData(s, data, len);
    }
}

uint32_t cpioStreamGetFileCount(cpioStream_t *s)
{
    if (s == NULL)
        return 0;
    return s->file_count;
}

cpioFile_t *cpioStreamPopFile(cpioStream_t *s)
{
    if (s == NULL || s->file_count == 0)
        return NULL;

    cpioFilePriv_t *f = TAILQ_FIRST(&s->store_list);
    if (f)
    {
        TAILQ_REMOVE(&s->store_list, f, entry);
        TAILQ_INSERT_TAIL(&s->used_list, f, entry);
        s->file_count -= 1;
    }
    return f ? &f->file : NULL;
}

void cpioStreamDestroyFile(cpioStream_t *s, cpioFile_t *file)
{
    if (s == NULL)
        return;
    cpioFilePriv_t *f = prvRawToCpioFile(file);
    TAILQ_REMOVE(&s->used_list, f, entry);
    free(f);
}
