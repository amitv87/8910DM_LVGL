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

#ifndef _DRV_GPO_H_
#define _DRV_GPO_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    GPO_LEVEL_INVALID = -1,
    GPO_LEVEL_LOW,
    GPO_LEVEL_HIGH
} drvGpoLevel;

struct drvGpo;

/**
 * opaque data struct for GPO instance
 */
typedef struct drvGpo drvGpo_t;

/**
 * @brief open a GPO
 *
 * IOMUX will be set to GPO mode at open.
 *
 * Each GPO sould be opened only once. When it is already opened, this
 * API will return NULL.
 *
 * The returned instance is dynamic allocated, caller should free it after
 * \a drvGpoClose is called.
 *
 * @param id    GPO id. The range may be different among chips.
 * @return
 *      - GPO instance pointer
 *      - NULL if parameter is invalid
 */
drvGpo_t *drvGpoOpen(uint32_t id);

/**
 * @brief close a GPO
 *
 * IOMUX will be kept, and the GPO will be set to default.
 *
 * @param d     GPO instance
 */
void drvGpoClose(drvGpo_t *d);

/**
 * @brief read the level of GPO
 *
 * It can be called for GPO output,
 * it is the level set by software.
 *
 * @param d     GPO instance
 * @return
 *      - GPO_LEVEL_INVALID means error
 *      - GPO_LEVEL_HIGH for level high
 *      - GPO_LEVEL_LOW for level low
 */
drvGpoLevel drvGpoRead(drvGpo_t *d);

/**
 * @brief write level for GPO
 *
 * When it is called, set the output level.
 *
 * @param d     GPO instance
 * @param level GPO level to be set,
 *              - GPO_LEVEL_HIGH for level high
 *              - GPO_LEVEL_LOW for level low
 */
void drvGpoWrite(drvGpo_t *d, drvGpoLevel level);

#ifdef __cplusplus
}
#endif
#endif
