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

#ifndef _BOOT_MEM_H_
#define _BOOT_MEM_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize heap
 *
 * Initialize internal RAM heap and external RAM heap. Both heap can be
 * empty (size is 0).
 *
 * The default heap (for \a malloc and \a calloc) is internal RAM heap.
 *
 * @param iram_start    internal RAM heap start address
 * @param iram_size     internal RAM heap size (in bytes)
 * @param ext_ram_start external RAM heap start address
 * @param ext_ram_size  external RAM heap size (in bytes)
 */
void bootHeapInit(void *iram_start, unsigned iram_size,
                  void *ext_ram_start, unsigned ext_ram_size);

/**
 * @brief set internal RAM heap as default
 */
void bootHeapDefaultIram(void);

/**
 * @brief set external RAM heap as default
 */
void bootHeapDefaultExtRam(void);

/**
 * @brief allocate from internal RAM heap
 *
 * @param size  request size (in bytes)
 * @return
 *      - allocated memory pointer
 *      - NULL is no space in internal RAM heap
 */
void *bootIramMalloc(unsigned size);

/**
 * @brief allocate from internal RAM heap and clear to zero
 *
 * @param nmemb number of blocks
 * @param size  size (in bytes) for each block
 * @return
 *      - allocated memory pointer
 *      - NULL is no space in internal RAM heap
 */
void *bootIramCalloc(unsigned nmemb, unsigned size);

/**
 * @brief allocate from external RAM heap
 *
 * @param size  request size (in bytes)
 * @return
 *      - allocated memory pointer
 *      - NULL is no space in internal RAM heap
 */
void *bootExtRamMalloc(unsigned size);

/**
 * @brief allocate from external RAM heap and clear to zero
 *
 * @param nmemb number of blocks
 * @param size  size (in bytes) for each block
 * @return
 *      - allocated memory pointer
 *      - NULL is no space in internal RAM heap
 */
void *bootExtRamCalloc(unsigned nmemb, unsigned size);

/**
 * @brief free dynamic memory
 *
 * The memory should be dynamic allocated from either internal RAM heap
 * or external RAM heap.
 *
 * If \a ptr is NULL, no operation is performed.
 *
 * @param ptr   pointer to be freed
 */
void bootFree(void *ptr);

/**
 * @brief show heap information in trace
 *
 * This is for debug purpose only.
 */
void bootShowMemInfo(void);

#ifdef __cplusplus
}
#endif
#endif
