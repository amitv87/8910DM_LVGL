/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QUEC_LED_CFG_TBL_H
#define QUEC_LED_CFG_TBL_H

#include "quec_common.h"
#include "quec_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 * Enum
 ===========================================================================*/
typedef enum
{
    SIM_NOT_INSERT      = 0,
    SIM_INITIALIZING,
    SIM_PIN_LOCK,
    SIM_CPIN_READY,
    SEARCHING,
    CAMP_ON_CELL        = 5,
    REGISTERING,
    STANDBY,
    PDP_ACTIVING,
    PDP_ACTIVED,
    SOCKET_SET_UP       = 10,
    DATA_TRANSMIT,
    PDP_DEACTIVING,
    DIALING,
    NET_LED_STATUS_MAX
}quec_module_status; 

typedef enum
{
    NET_TYPE_NOT_REGISTERED,
    NET_TYPE_2G,
    NET_TYPE_3G,
    NET_TYPE_4G,
    NET_TYPE_USER_DEF,
    NET_TYPE_MAX
}quec_net_type;

/*===========================================================================
 * Struct
 ===========================================================================*/
typedef struct 
{
    uint16_t cycle;  /* pwm cycle level time */
    uint16_t ratio;  /* pwm high level time */
}quec_led_net_type_t;

typedef struct 
{
    uint8_t gpio_num;    /* number of gpio net led use*/
    uint8_t pwm_num;     /* number of pwm dev net led use */
    uint8_t priority;    /* save net led status priority */
    quec_led_net_type_t net_type[NET_TYPE_MAX]; /* save diffirent config for 2G,3G,4G or user defined */
}quec_led_cfg_t;

/*===========================================================================
 * Extern
 ===========================================================================*/
extern quec_led_cfg_t quec_led_cfg_tbl[NET_LED_STATUS_MAX];

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
void quec_led_cfg_tbl_initialize(void);
int quec_led_cfg_tbl_set(uint8_t index, uint8_t gpio_num, uint8_t pwm_num, uint8_t priority, uint8_t network_type, uint16_t cycle, uint16_t ratio);
int quec_led_cfg_tbl_get(void);
int quec_ledmode_set(uint8 mode);
int quec_ledmode_get(uint8 *p_mode);

#ifndef CONFIG_QL_OPEN_EXPORT_PKG
int quec_pwm_cfg_set(uint32_t mode);
int quec_pwm_cfg_get(uint32_t* mode);
void quec_led_cfg_tbl_initialize_lr(void);
#endif


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_LED_CFG_TBL_H */


