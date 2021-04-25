/**  @file
  ql_boot_lcd.h

  @brief
  This file is used to define boot lcd api for different Quectel Project.

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
08/12/2020        Neo         Init version
=================================================================*/

#ifndef QL_BOOT_LCD_H
#define QL_BOOT_LCD_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "drv_lcd.h"


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_BOOT_LCD_ERRCODE_BASE (0x8209<<16)



/*===========================================================================
 * Enum
 ===========================================================================*/
typedef enum
{
    QL_BOOT_LCD_SUCCESS = 0,

    QL_BOOT_LCD_EXECUTE_ERR                 = 1|QL_BOOT_LCD_ERRCODE_BASE,
    QL_BOOT_LCD_INVALID_PARAM_ERR,
    QL_BOOT_LCD_INIT_ERR,
    QL_BOOT_LCD_BUFFER_CREATE_ERR,
    QL_BOOT_LCD_BLOCK_SET_ERR,
    QL_BOOT_LCD_OPERATION_ERR
}ql_boot_errcode_lcd;



/*===========================================================================
 * Functions declaration
 ===========================================================================*/
ql_boot_errcode_lcd ql_boot_lcd_init(void);
ql_boot_errcode_lcd ql_boot_lcd_clear_screen(uint16_t color);
ql_boot_errcode_lcd ql_boot_lcd_write(uint16_t *buffer, uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y);
ql_boot_errcode_lcd ql_boot_lcd_display_on(void);
ql_boot_errcode_lcd ql_boot_lcd_display_off(void);
ql_boot_errcode_lcd ql_boot_lcd_display_close(void);
ql_boot_errcode_lcd ql_boot_spi_lcd_write_cmd(uint8_t cmd);
ql_boot_errcode_lcd ql_boot_spi_lcd_write_data(uint8_t data);
ql_boot_errcode_lcd ql_boot_lcd_set_brightness(uint32_t level);


void ql_boot_lcd_display_demo();


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_BOOT_LCD_H */


