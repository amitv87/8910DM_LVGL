/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_GPIO_H
#define QL_GPIO_H


#include "ql_api_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_GPIO_ERRCODE_BASE (QL_COMPONENT_BSP_GPIO<<16)

/*===========================================================================
 * Enum
 ===========================================================================*/
/**
 * GPIO errcode
 */
typedef enum
{
    QL_GPIO_SUCCESS = QL_SUCCESS,

    QL_GPIO_EXECUTE_ERR                 = 1|QL_GPIO_ERRCODE_BASE,
    QL_GPIO_INVALID_PARAM_ERR,
    QL_GPIO_OPEN_ERR,
    QL_GPIO_CONFIG_ERR,
    QL_GPIO_CALLBACK_ERR,
    QL_GPIO_LEVEL_TRIGGER_ERR
}ql_errcode_gpio;

/**
 * GPIO number
 */
typedef enum
{
    GPIO_0 = 0,
    GPIO_1,
    GPIO_2,
    GPIO_3,
    GPIO_4,
    GPIO_5,
    GPIO_6,
    GPIO_7,
    GPIO_8,
    GPIO_9,
    GPIO_10,
    GPIO_11,
    GPIO_12,
    GPIO_13,
    GPIO_14,
    GPIO_15,
    GPIO_16,
    GPIO_17,
    GPIO_18,
    GPIO_19,
    GPIO_20,
    GPIO_21,
    GPIO_22,
    GPIO_23,
    GPIO_24,
    GPIO_25,
    GPIO_26,
    GPIO_27,
    GPIO_28,
    GPIO_29,
    GPIO_30,
    GPIO_31,
    GPIO_MAX
}ql_GpioNum;

/**
 * GPIO direction
 */
typedef enum
{
    GPIO_INPUT,
    GPIO_OUTPUT
}ql_GpioDir;

/**
 * GPIO output level
 */
typedef enum
{
    LVL_LOW,
    LVL_HIGH
}ql_LvlMode;

/**
 * GPIO input pull mode
 */
typedef enum
{
    PULL_NONE,
    PULL_DOWN,
    PULL_UP
}ql_PullMode;

/**
 * GPIO interrupt trigger
 */
typedef enum
{
    EDGE_TRIGGER,
    LEVEL_TRIGGER
}ql_TriggerMode;

/**
 * GPIO interrupt debounce
 */
typedef enum
{
    DEBOUNCE_DIS,
    DEBOUNCE_EN
}ql_DebounceMode;

/**
 * GPIO edge-trigger mode
 */
typedef enum
{
    EDGE_RISING,
    EDGE_FALLING,
    EDGE_BOTH
}ql_EdgeMode;

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
/**
 * @brief GPIO initialize
 * @param gpio_num  : GPIO number
 * @param gpio_dir  : GPIO direction
 * @param gpio_pull : GPIO input pull
 * @param gpio_lvl  : GPIO output level
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_OPEN_ERR
 *       QL_GPIO_CONFIG_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_gpio_init(ql_GpioNum gpio_num, ql_GpioDir gpio_dir, ql_PullMode gpio_pull, ql_LvlMode gpio_lvl);

/**
 * @brief GPIO de-initialize
 * @param gpio_num  : GPIO number
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_gpio_deinit(ql_GpioNum gpio_num);

/**
 * @brief GPIO set output level
 * @param gpio_num  : GPIO number
 * @param gpio_lvl  : GPIO set output level
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_gpio_set_level(ql_GpioNum gpio_num, ql_LvlMode gpio_lvl);

/**
 * @brief GPIO get output level
 * @param gpio_num  : GPIO number
 * @param gpio_lvl  : GPIO get output level
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_gpio_get_level(ql_GpioNum gpio_num, ql_LvlMode *gpio_lvl);

/**
 * @brief GPIO set direction
 * @param gpio_num  : GPIO number
 * @param gpio_dir  : GPIO set direction
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_gpio_set_direction(ql_GpioNum gpio_num, ql_GpioDir gpio_dir);

/**
 * @brief GPIO get direction
 * @param gpio_num  : GPIO number
 * @param gpio_dir  : GPIO get direction
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_gpio_get_direction(ql_GpioNum gpio_num, ql_GpioDir *gpio_dir);

/**
 * @brief GPIO set pull
 * @param gpio_num  : GPIO number
 * @param gpio_pull : GPIO set input pull
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_CONFIG_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_gpio_set_pull(ql_GpioNum gpio_num, ql_PullMode gpio_pull);

/**
 * @brief GPIO get pull
 * @param gpio_num  : GPIO number
 * @param gpio_pull : GPIO get input pull
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_CONFIG_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_gpio_get_pull(ql_GpioNum gpio_num, ql_PullMode *gpio_pull);

/**
 * @brief GPIO interrupt register
 * @param gpio_num      : GPIO number
 * @param gpio_trigger  : GPIO interrupt trigger mode
 * @param gpio_debounce : GPIO interrupt trigger debounce enable/disable
 * @param gpio_edge     : GPIO interrupt edge/level trigger configure
 * @param gpio_pull     : GPIO get input pull
 * @param int_cb        : GPIO interrupt cb
 * @param cb_ctx        : GPIO interrupt cb_ctx
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_LEVEL_TRIGGER_ERR
 *       QL_GPIO_OPEN_ERR
 *       QL_GPIO_CONFIG_ERR
 *       QL_GPIO_CALLBACK_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_int_register(ql_GpioNum gpio_num, ql_TriggerMode gpio_trigger, ql_DebounceMode gpio_debounce,
                                    ql_EdgeMode gpio_edge, ql_PullMode gpio_pull,
                                    void *int_cb, void *cb_ctx);

/**
 * @brief GPIO interrupt enable
 * @param gpio_num : GPIO number
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_CONFIG_ERR
 *       QL_GPIO_CALLBACK_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_int_enable(ql_GpioNum gpio_num);

/**
 * @brief GPIO interrupt disable
 * @param gpio_num : GPIO number
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_int_disable(ql_GpioNum gpio_num);

/**
 * @brief Pin function seclect
 * @param pin_num  : Pin number
 * @param func_sel : seclected Pin function
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_pin_set_func(uint8_t pin_num, uint8_t func_sel);

/**
 * @brief Seclected Pin function get
 * @param pin_num  : Pin number
 * @param func_sel : seclected Pin function
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_pin_get_func(uint8_t pin_num, uint8_t *func_sel);

/**
 * @brief Pin function set to GPIO
 * @param pin_num  : Pin number
 * @return
 *       QL_GPIO_INVALID_PARAM_ERR
 *       QL_GPIO_SUCCESS
 */
ql_errcode_gpio ql_pin_set_gpio(uint8_t pin_num);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_GPIO_H */

