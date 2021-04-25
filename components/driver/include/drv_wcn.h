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

#ifndef _DRV_WCN_INTERNAL_H_
#define _DRV_WCN_INTERNAL_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

typedef struct drv_wcn drvWcn_t;

typedef enum
{
    WCN_USER_WIFI = (1 << 0),
    WCN_USER_BT = (1 << 1),
} drvWcnUser_t;

/**
 * \brief open wcn
 *
 * \param user  caller user
 * \return
 *      - NULL if fail else the wcn context
 */
drvWcn_t *drvWcnOpen(drvWcnUser_t user);

/**
 * \brief close wcn
 *
 * \param wcn   the wcn context
 */
void drvWcnClose(drvWcn_t *wcn);

/**
 * \brief request wcn resource
 *
 * \param wcn   the wcn context
 * \return
 *      - true on success else fail
 */
bool drvWcnRequest(drvWcn_t *wcn);

/**
 * \brief release wcn resource
 *
 * \paran wcn   the wcn context
 */
void drvWcnRelease(drvWcn_t *wcn);

OSI_EXTERN_C_END

#endif