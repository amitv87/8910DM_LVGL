/**  @file
  quec_common.h

  @brief
  This file is used to define version information for different Quectel Project.

*/

/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/


#ifndef QUEC_COMMON_H
#define QUEC_COMMON_H

/*> include header files here*/

#include <stdio.h>
#include "osi_api.h"

#include "quec_proj_config.h"
#include "quec_log.h"
#include "ql_api_common.h"
#include "quec_cust_patch.h"
#include "quec_cust_feature.h"

#ifdef __cplusplus
extern "C" {
#endif


/*========================================================================
 *  Macro Definition
 *========================================================================*/
#define TRUE 1
#define FALSE 0

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#define kNoErr                      0       //! No error occurred.
#define kGeneralErr                -1       //! General error.
#define kInProgressErr              1       //! Operation in progress.


/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
typedef enum
{
	QUEC_DISABLE = 0,
	QUEC_ENABLE  = 1,
} quec_enable_e;

typedef enum
{
	//error code is minus, less than QUEC_GENERAL_ERROR

	QUEC_GENERAL_ERROR  = kGeneralErr,
	
	//sucess code is not minus, bigger than QUEC_GENERAL_ERROR
	QUEC_NO_ERROR = kNoErr,
	QUEC_SUCCESS = QUEC_NO_ERROR,
} quec_errcode_e;

/*========================================================================
 *	Type Definition
 *========================================================================*/
typedef int QuecOSStatus;
 

/*========================================================================
 *	Utilities Definition
 *========================================================================*/
/* Get Array Size */
#ifndef QUEC_ARR_SIZE
#define QUEC_ARR_SIZE(a)         	( sizeof(a) / sizeof(a[0]) )
#endif

/* Bit Mask Operation */
#define QUEC_BIT_MASK(bit)      		(1 << (bit))
#define QUEC_BIT_CLR(x,bit)     		((x) &= ~ BIT_MASK (bit))
#define QUEC_BIT_SET(x,bit)     		((x) |= BIT_MASK (bit))
#define QUEC_CHECK_BIT_SET(x,bit)  		((x) & BIT_MASK (bit))

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_COMMON_H */



