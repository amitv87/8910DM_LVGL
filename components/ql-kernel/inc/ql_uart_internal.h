/*============================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------

=============================================================================*/

#ifndef QL_UART_INTERNAL_H
#define QL_UART_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>
#include "quec_common.h"
#include "at_engine.h"
#include "ql_uart.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __QUEC_OEM_VER_LD__
#define QL_MIAN_UART_PORT QL_UART_PORT_3
#else
#define QL_MIAN_UART_PORT QL_UART_PORT_1
#endif

/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
typedef enum
{
	DEV_NONE = -1,
	DEV_UART1 = 	QL_UART_PORT_1,
	DEV_UART2 = 	QL_UART_PORT_2,
	DEV_UART3 = 	QL_UART_PORT_3,
	DEV_USB_AT = 	QL_USB_PORT_AT,
	DEV_USB_MODEM = QL_USB_PORT_MODEM,
	DEV_USB_NMEA  = QL_USB_PORT_NMEA,
#ifdef CONFIG_ATR_CMUX_SUPPORT
	DEV_CMUX1,
	DEV_CMUX2,
	DEV_CMUX3,
	DEV_CMUX4,
#endif
	DEV_MAX,
} quec_dev_e;

typedef enum 
{
	QL_UART_CLOSE,
	QL_UART_AT_OPEN,
	QL_UART_APP_OPEN,
}ql_uart_open_staus_e;
 /*========================================================================
*  Type Definition
*========================================================================*/

typedef struct
{
	atDevice_t				*device;
	atDispatch_t			*dispatch;
	ql_uart_open_staus_e	status;
	quec_mutex_t			*lock;
    ql_uart_callback		uart_cb;
 } ql_dev_info_t;

typedef struct
{
	char * 			port_name;
	uint32_t		drv_name;
} ql_dev_name_info_s;

/*========================================================================
*	function Definition
*========================================================================*/
atDispatch_t *quec_get_dispatch_by_dev(uint32_t dev);

quec_dev_e quec_get_dev_by_dispatch(atDispatch_t *dispatch);

bool quec_uart_at_device_create(ql_uart_port_number_e port);

uint8_t quec_get_uart_status(uint32_t dev);

bool quec_uart_is_allow_create(uint32_t drv_name);

void quec_at_device_create_success(uint32_t drv_name, atDevice_t *device, atDispatch_t *dispatch);

bool quec_uart_pin_init(ql_uart_port_number_e port);

void quec_write_to_at_dev(quec_dev_e dev,unsigned char * data_buff,unsigned int len);


#ifdef __cplusplus
}/*"C" */
#endif

#endif   /*QL_UART_INTERNAL_H*/






