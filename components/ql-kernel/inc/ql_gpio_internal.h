/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_GPIO_INTERNAL_H
#define QL_GPIO_INTERNAL_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pin function seclect
 * @param pin_num  : Pin number
 * @param func_sel : seclected Pin function
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_pin_set_func_internal(uint8_t pin_num, uint8_t func_sel);


/**
 * @brief Pin function set to GPIO
 * @param pin_num  : Pin number
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_pin_set_gpio_internal(uint8_t pin_num);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_GPIO_INTERNAL_H */

