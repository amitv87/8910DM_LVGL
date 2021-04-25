/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef _GPIOINTDEMO_H
#define _GPIOINTDEMO_H

#include "ql_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

/*===========================================================================
 * Enum
 ===========================================================================*/
typedef enum
{
    INT_CB01,
    INT_CB02
} ql_IntSel;

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
void ql_gpioint_app_init(void);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* _GPIOINTDEMO_H */


