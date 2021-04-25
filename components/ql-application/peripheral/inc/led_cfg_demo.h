/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef _LEDCFGDEMO_H
#define _LEDCFGDEMO_H

#include "ql_pwm.h"
#include "ql_led.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
//for LPG
#define LED_TWINKLE_SLOW_PERIOD     LPG_PER_2000MS
#define LED_TWINKLE_SLOW_ONTIME     LPG_ONTIME_200MS
    
#define LED_TWINKLE_FAST_PERIOD     LPG_PER_500MS
#define LED_TWINKLE_FAST_ONTIME     LPG_ONTIME_234MS
    
#define LED_TWINKLE_SPEED_PERIOD    LPG_PER_125MS
#define LED_TWINKLE_SPEED_ONTIME    LPG_ONTIME_62MS
    
#define LED_TWINKLE_ON_PERIOD       LPG_PER_125MS
#define LED_TWINKLE_ON_ONTIME       LPG_ONTIME_200MS
    
#define LED_TWINKLE_OFF_PERIOD      LPG_PER_2000MS
#define LED_TWINKLE_OFF_ONTIME      LPG_ONTIME_UNDEFINE

/*===========================================================================
 * Enum
 ===========================================================================*/

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
void ql_ledcfg_app_init(void);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* _LEDCFGDEMO_H */


