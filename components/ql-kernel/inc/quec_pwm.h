/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QUEC_PWM_H
#define QUEC_PWM_H

#include "quec_common.h"
#include "hal_iomux.h"
#include "drv_pwm.h"
#include "drv_names.h"
#include "ql_gpio.h"
#include "quec_pin_index.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

/*===========================================================================
 * Struct
 ===========================================================================*/
typedef enum
{
    PIN_NET_STATUS = QUEC_GPIO_NET_STATUS,
    PIN_NET_MODE   = QUEC_GPIO_NET_MODE
} PWM_GPIO_NUM;

typedef enum
{
    PWM_PWT_OUT = 0,
    PWM_LPG_OUT,
    PWM_VIRT_OUT
} PWM_DEVICE_NUM;

#ifndef CONFIG_QL_OPEN_EXPORT_PKG
typedef enum
{
    QUEC_PWM_MODE_LPG,
    QUEC_PWM_MODE_VIRT,
    QUEC_PWM_MODE_LR
} QUEC_PWM_MODE;
#endif

/*===========================================================================
 * Extern
 ===========================================================================*/


/*===========================================================================
 * Functions declaration
 ===========================================================================*/
bool quec_pwm_pwt_set(uint8_t pwm_divider, uint32_t pwt_period, uint32_t pwt_duty);
bool quec_pwm_lpg_set(drvLpgPer_t lpg_period, drvLpgOntime_t highlvl_time);
bool quec_pwm_pwl0_set(uint8_t pwl_duty);
bool quec_pwm_pwl1_set(uint8_t pwl_duty);

#ifndef CONFIG_QL_OPEN_EXPORT_PKG
bool quec_pwm_virt_open(void);
bool quec_pwm_virt_close(void);
bool quec_pwm_virt_set(uint32_t pwm_period, uint32_t pwm_high_lvl_time);
#endif


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_PWM_H */


