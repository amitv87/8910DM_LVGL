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

#ifndef _OSI_GENERIC_LIST_
#define _OSI_GENERIC_LIST_

#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

//! @cond Doxygen_Suppress
typedef SLIST_ENTRY(osiSlistItem) osiSlistIter_t;
typedef SLIST_HEAD(osiSlistHead, osiSlistItem) osiSlistHead_t;
//! @endcond

/**
 * @brief data type for generic SLIST item
 */
typedef struct osiSlistItem
{
    osiSlistIter_t iter; ///< SLIST_ENTRY iterator
} osiSlistItem_t;

//! @cond Doxygen_Suppress
typedef TAILQ_ENTRY(osiTailqItem) osiTailqIter_t;
typedef TAILQ_HEAD(osiTailqHead, osiTailqItem) osiTailqHead_t;
//! @endcond

/**
 * @brief data type for generic TAILQ item
 */
typedef struct osiTailqItem
{
    osiTailqIter_t iter; ///< TAILQ_ENTRY iterator
} osiTailqItem_t;

#ifdef __cplusplus
}
#endif
#endif
