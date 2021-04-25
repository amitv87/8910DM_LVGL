/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef _LCDDEMO_H
#define _LCDDEMO_H

#include "ql_lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define LCD_RED     0xf800
#define LCD_GREEN   0x07e0
#define LCD_BLUE    0x001f
    
#define LCD_WHITE   0xffff
#define LCD_BLACK   0x0000
    
#define LCD_YELLOW  0xffe0
#define LCD_PURPLE  0xf81f

/*===========================================================================
 * Struct
 ===========================================================================*/

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
void ql_lcd_app_init(void);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* _LCDDEMO_H */


