/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/

#ifndef QUEC_CUSTOMER_CFG_H
#define QUEC_CUSTOMER_CFG_H

#include "quec_common.h"
#include "ql_uart.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 * Macro
 ===========================================================================*/
#ifdef CONFIG_QL_PROJECT_DEF_EG500U
#define UART_PIN_MAX  (1)  //EG500U only UART2
#elif defined CONFIG_QL_PROJECT_DEF_EC600U
 #ifndef __QUEC_OEM_VER_LD__
#define UART_PIN_MAX  (1)  //EC600U only UART2
 #else
#define UART_PIN_MAX  (2)  //UART2 UART3
 #endif
#else
#define UART_PIN_MAX  (2)  //UART2 UART3
#endif
#define CAM_PIN_MAX   (8)

/*===========================================================================
 * Struct
 ===========================================================================*/
typedef struct
{
    ql_uart_port_number_e port;
    uint8_t tx_pin;
    uint8_t tx_func;
    uint8_t rx_pin;
    uint8_t rx_func;
} ql_uart_func_s;

typedef struct
{
    uint8_t pin;
    uint8_t func;
} ql_cam_func_s;

/*===========================================================================
 * Extern
 ===========================================================================*/
extern ql_uart_func_s ql_uart_pin_func[];
extern ql_cam_func_s ql_cam_pin_func[];


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_CUSTOMER_CFG_H */

