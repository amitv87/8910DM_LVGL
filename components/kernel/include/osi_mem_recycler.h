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

#ifndef _OSI_MEM_RECYCLER_H_
#define _OSI_MEM_RECYCLER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "kernel_config.h"
#include "osi_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief opaque data structure of memory recycler
 */
typedef struct osiMemRecycler osiMemRecycler_t;

/**
 * @brief creat memory recycler
 *
 * To avoid dynamic memory complexity, static memory with specified
 * \p depth will be used.
 *
 * @param depth     maximum pointer count
 * @return
 *      - memory recycler pointer
 *      - NULL if out of memory
 */
osiMemRecycler_t *osiMemRecyclerCreate(size_t depth);

/**
 * @brief delete the memory recycler
 *
 * When \p p is NULL, nothing will be done.
 *
 * @param d         the memory recycler
 */
void osiMemRecyclerDelete(osiMemRecycler_t *d);

/**
 * @brief put a pointer into memory recycler
 *
 * To mimic free (3), \p p can be NULL. In this case, it will be
 * ignored silently.
 *
 * When the pointer is already in the memory recycler, it can't be
 * added again.
 *
 * @param d         the memory recycler, must be valid
 * @param p         pointer to be put to memory recycler
 * @return
 *      - true on success
 *      - false on fail
 *          - invalid parameter
 *          - the pointer already in memory recycler
 */
bool osiMemRecyclerPut(osiMemRecycler_t *d, const void *p);

/**
 * @brief undo put a pointer into memory recycler
 *
 * Though it is rare to undo put, this is also provided.
 *
 * @param d         the memory recycler, must be valid
 * @param p         pointer to be extract from memory recycler
 * @return
 *      - true on success
 *      - false on fail
 *          - invalid parameter
 *          - the pointer not in memory recycler
 */
bool osiMemRecyclerUndoPut(osiMemRecycler_t *d, const void *p);

/**
 * @brief free pointers into memory recycler
 *
 * After pointers are freed, they are extracted from the memory
 * recycler also.
 *
 * @param d         the memory recycler, must be valid
 */
void osiMemRecyclerEmpty(osiMemRecycler_t *d);

#ifdef __cplusplus
}
#endif
#endif
