/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_LCD_H
#define QL_LCD_H


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_LCD_ERRCODE_BASE (QL_COMPONENT_BSP_LCD<<16)

/*===========================================================================
 * Enum
 ===========================================================================*/
/**
 * LCD errcode
 */
typedef enum
{
    QL_LCD_SUCCESS = QL_SUCCESS,

    QL_LCD_EXECUTE_ERR                 = 1|QL_LCD_ERRCODE_BASE,
    QL_LCD_INVALID_PARAM_ERR,
    QL_LCD_INIT_ERR,
    QL_LCD_BUFFER_CREATE_ERR,
    QL_LCD_BLOCK_SET_ERR,
    QL_LCD_OPERATION_ERR
}ql_errcode_lcd;

/*===========================================================================
 * Struct
 ===========================================================================*/

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
/**
 * @brief Init the LCD moudle
 * @return
 *      QL_LCD_INIT_ERR
 *      QL_LCD_BUFFER_CREATE_ERR
 *      QL_LCD_BLOCK_SET_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_init(void);

/**
 * @brief Clear screen background to a kind of color
 * @param color : background color
 * @return
 *      QL_LCD_BLOCK_SET_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_clear_screen(uint16_t color);

/**
 * @brief LCD part area set
 * @param buffer  : write-in data buffer
 * @param start_x : horizontal coordinate x start location
 * @param start_y : vertical coordinate y start location
 * @param end_x   : horizontal coordinate x end location
 * @param end_x   : vertical coordinate y end location
 * @return
 *      QL_LCD_INVALID_PARAM_ERR
 *      QL_LCD_BLOCK_SET_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_write(uint16_t *buffer, uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y);

/**
 * @brief LCD display turn on
 * @return
 *      QL_LCD_OPERATION_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_display_on(void);

/**
 * @brief LCD display turn off
 * @return
 *      QL_LCD_OPERATION_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_display_off(void);

/**
 * @brief LCD SPI CMD send interface
 * @param cmd : LCD SPI CMD param
 * @return
 *      QL_LCD_INVALID_PARAM_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_spi_lcd_write_cmd(uint8_t cmd);

/**
 * @brief LCD SPI DATA send interface
 * @param data : LCD SPI DATA param
 * @return
 *      QL_LCD_INVALID_PARAM_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_spi_lcd_write_data(uint8_t data);

/**
 * @brief LCD backlight brightness adjust function
 * @param data : LCD backlight brightness level
 * @return
 *      QL_LCD_INVALID_PARAM_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_set_brightness(uint32_t level);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_LCD_H */


