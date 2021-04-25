/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


/*===========================================================================
 * include files
 ===========================================================================*/
#include "quec_customer_cfg.h"
#include "ql_uart.h"
#include "quec_pin_index.h"

/*===========================================================================
 * Customer set Map
 ===========================================================================*/
ql_uart_func_s ql_uart_pin_func[UART_PIN_MAX] =
{
#ifdef CONFIG_QL_PROJECT_DEF_EC200U
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 3, QUEC_PIN_UART2_RXD, 3},
    {QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4}
#elif defined CONFIG_QL_PROJECT_DEF_EC600U
 #ifndef __QUEC_OEM_VER_LD__
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    //{QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4}
 #else
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    {QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 6, QUEC_PIN_UART3_RXD, 6}
 #endif
#elif defined CONFIG_QL_PROJECT_DEF_EG700U
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    {QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4}
#elif defined CONFIG_QL_PROJECT_DEF_EG500U
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    //{QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4}
#endif
};

ql_cam_func_s ql_cam_pin_func[CAM_PIN_MAX] =
{
    {QUEC_PIN_CAM_I2C_SCL   , 0},
    {QUEC_PIN_CAM_I2C_SDA   , 0},
    {QUEC_PIN_CAM_RSTL      , 0},
    {QUEC_PIN_CAM_PWDN      , 0},
    {QUEC_PIN_CAM_REFCLK    , 0},
    {QUEC_PIN_CAM_SPI_DATA0 , 0},
    {QUEC_PIN_CAM_SPI_DATA1 , 0},
    {QUEC_PIN_CAM_SPI_SCK   , 0}
};

