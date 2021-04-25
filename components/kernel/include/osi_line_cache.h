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

#ifndef _OSI_LINE_CACHE_H_
#define _OSI_LINE_CACHE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief opaque line cache data struct
 */
typedef struct osiLineCache osiLineCache_t;

/**
 * \brief function prototype for line cache output
 *
 * This function will output line cache data.
 *
 * \param ctx       output function context
 * \param data      output data
 * \param size      output size
 */
typedef void (*osiLineCacheOutput_t)(void *ctx, const void *data, unsigned size);

/**
 * \brief create a line cache
 *
 * The output function of line cache specified at creation, and can't be
 * changed.
 *
 * \param output    output function
 * \param size      line cache size
 * \return
 *      - line cache object
 *      - NULL if invalid parameter, or out of memory
 */
osiLineCache_t *osiLineCacheCreate(osiLineCacheOutput_t output, unsigned size);

/**
 * \brief delete a line cache
 *
 * When \p d is NULL, nothing will be done.
 *
 * \param d         line cache object
 */
void osiLineCacheDelete(osiLineCache_t *d);

/**
 * \brief write data to line cache
 *
 * \p owner is the context of output function.
 *
 * When \p owner is the same as the owner of existed data, and there are
 * rooms of internal buffer, \p data will be copied to internal buffer.
 * Otherwise, the registered output function will be called.
 *
 * \param d         line cache object
 * \param owner     owner the data
 * \param data      data to be written
 * \param size      written data size
 */
void osiLineCacheWrite(osiLineCache_t *d, void *owner, const void *data, unsigned size);

/**
 * \brief flush data in internal buffer
 *
 * \param d         line cache object
 */
void osiLineCacheFlush(osiLineCache_t *d);

#ifdef __cplusplus
}
#endif
#endif
