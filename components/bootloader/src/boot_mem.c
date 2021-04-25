/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "boot_mem.h"
#include "boot_platform.h"
#include "osi_log.h"
#include "osi_api.h"
#include "osi_compiler.h"

// A simple memory management for bootloader
//
// Each region to be managed is called as a heap. At malloc, create a
// block with one word at head, and one word at tail for management.
// The format of the word is the same:
//   [31]: 1/allocated, 0/free
//   [30:0]: word count
//
// At malloc, always malloc from tail. At free, adjacent free blocks
// will be merged.

typedef struct
{
    uint32_t *start;
    uint32_t *end;
    uint32_t *tail;
} bootHeap_t;

static bootHeap_t gIramHeap;
static bootHeap_t gExtRamHeap;
static bootHeap_t *gDefaultHeap;

static inline bool _is_allocated(uint32_t info)
{
    return (info & (1 << 31)) ? true : false;
}

static inline unsigned _block_wcount(uint32_t info)
{
    return info & 0x7fffffff;
}

static inline void _set_head_tail(uint32_t *head, unsigned wcount)
{
    head[0] = head[wcount + 1] = (1 << 31) | wcount;
}

static inline void _unset_head_tail(uint32_t *head, unsigned wcount)
{
    head[0] = head[wcount + 1] = wcount;
}

static void _heap_init(bootHeap_t *heap, void *start, unsigned size)
{
    uintptr_t ps = (uintptr_t)start;
    uintptr_t pe = ps + size;
    ps = (ps + 3) & ~3; // aligned to 4 bytes, moving to larger
    pe = pe & ~3;       // aligned to 4 bytes, moving to smaller
    if ((ps & 7) == 0)
        ps += 4; // can't be 8 bytes aligned

    heap->start = (uint32_t *)ps;
    heap->end = (uint32_t *)pe;
    heap->tail = heap->start;
}

static void *_heap_malloc(bootHeap_t *heap, unsigned size)
{
    size = (size + 7) & ~7; // aligned up to 8 bytes
    unsigned wcount = size / 4;
    if (heap->tail + wcount + 2 > heap->end)
        return NULL;

    uint32_t *ptr = heap->tail;
    _set_head_tail(ptr, wcount);
    heap->tail += wcount + 2;
    return (void *)&ptr[1];
}

static void *_heap_calloc(bootHeap_t *heap, unsigned nmemb, unsigned size)
{
    size *= nmemb;
    size = (size + 7) & ~7; // aligned up to 8 bytes

    uint32_t *ptr = (uint32_t *)_heap_malloc(heap, size);
    if (ptr != NULL)
    {
        for (int n = 0; n < size / 4; n++)
            ptr[n] = 0;
    }
    return ptr;
}

static void _heap_free(bootHeap_t *heap, void *ptr)
{
    uint32_t *blk = (uint32_t *)ptr - 1;
    unsigned wcount = _block_wcount(blk[0]);

    for (;;)
    {
        uint32_t *nblk = blk + (wcount + 2);
        if (nblk == heap->tail || _is_allocated(nblk[0]))
            break;
        unsigned nwcount = _block_wcount(nblk[0]);
        wcount += nwcount + 2;
        _set_head_tail(blk, wcount);
    }

    for (;;)
    {
        if (blk == heap->start || _is_allocated(blk[-1]))
            break;
        unsigned pwcount = _block_wcount(blk[-1]);
        blk -= pwcount + 2;
        wcount += pwcount + 2;
        _set_head_tail(blk, wcount);
    }

    _unset_head_tail(blk, wcount);
    if (heap->tail == blk + wcount + 2)
        heap->tail = blk;
}

static void *_heap_realloc(bootHeap_t *heap, void *ptr, unsigned size)
{
    if (ptr == NULL)
    {
        return size == 0 ? NULL : _heap_malloc(heap, size);
    }

    uint32_t *nptr = NULL;
    if (size != 0)
    {
        uint32_t *blk = (uint32_t *)ptr - 1;
        if (!_is_allocated(blk[0]))
            return NULL;

        unsigned owcount = _block_wcount(blk[0]);
        size = (size + 7) & ~7; // aligned up to 8 bytes
        nptr = (uint32_t *)_heap_malloc(heap, size);
        if (nptr == NULL)
            return NULL;

        for (unsigned n = 0; n < OSI_MIN(unsigned, size / 4, owcount); ++n)
            nptr[n] = blk[n + 1];
    }

    _heap_free(heap, ptr);
    return nptr;
}

void bootHeapInit(void *iram_start, unsigned iram_size,
                  void *ext_ram_start, unsigned ext_ram_size)
{
    _heap_init(&gIramHeap, iram_start, iram_size);
    _heap_init(&gExtRamHeap, ext_ram_start, ext_ram_size);
    gDefaultHeap = &gIramHeap;
}

void bootFree(void *ptr)
{
    if (ptr == NULL)
        return;

    uint32_t *p = (uint32_t *)ptr;
    if (p >= gIramHeap.start && p < gIramHeap.end)
        _heap_free(&gIramHeap, p);
    else if (p >= gExtRamHeap.start && p < gExtRamHeap.end)
        _heap_free(&gExtRamHeap, p);
    else
        osiPanic();
}

void bootShowMemInfo(void)
{
    OSI_LOGI(0, "heap start/%p end/%p tail/%p",
             gIramHeap.start, gIramHeap.end, gIramHeap.tail);
    OSI_LOGI(0, "heap start/%p end/%p tail/%p",
             gExtRamHeap.start, gExtRamHeap.end, gExtRamHeap.tail);
}

void bootHeapDefaultIram(void) { gDefaultHeap = &gIramHeap; }
void bootHeapDefaultExtRam(void) { gDefaultHeap = &gExtRamHeap; }
void *bootIramMalloc(unsigned size) { return _heap_malloc(&gIramHeap, size); }
void *bootIramCalloc(unsigned nmemb, unsigned size) { return _heap_calloc(&gIramHeap, nmemb, size); }
void *bootExtRamMalloc(unsigned size) { return _heap_malloc(&gExtRamHeap, size); }
void *bootExtRamCalloc(unsigned nmemb, unsigned size) { return _heap_calloc(&gExtRamHeap, nmemb, size); }
void *malloc(unsigned size) { return _heap_malloc(gDefaultHeap, size); }
void *calloc(unsigned nmemb, unsigned size) { return _heap_calloc(gDefaultHeap, nmemb, size); }
void *realloc(void *ptr, unsigned size) { return _heap_realloc(gDefaultHeap, ptr, size); }
void free(void *ptr) { bootFree(ptr); }
