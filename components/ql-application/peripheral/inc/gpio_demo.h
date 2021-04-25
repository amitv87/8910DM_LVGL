/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef _GPIODEMO_H
#define _GPIODEMO_H

#include "ql_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

/*===========================================================================
 * Struct
 ===========================================================================*/
typedef struct
{
    ql_GpioNum      gpio_num;
    ql_GpioDir      gpio_dir;
    ql_PullMode     gpio_pull;    //for input only
    ql_LvlMode      gpio_lvl;     //for output only
} ql_gpio_cfg;

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
void ql_gpio_app_init(void);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* _GPIODEMO_H */


