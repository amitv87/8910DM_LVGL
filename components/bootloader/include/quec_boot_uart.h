/**  @file
  quec_boot_uart.h

  @brief
  This file is used to define boot uart api for different Quectel Project.

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
24/01/2021        Neo         Init version
=================================================================*/

#ifndef QUEC_BOOT_UART_H
#define QUEC_BOOT_UART_H


#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "quec_boot_pin_index.h"
#include "boot_fdl_channel.h"
#include "drv_names.h"


#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#ifndef CONFIG_QL_PROJECT_DEF_EG500U
#define UART_PIN_MAX  (2)  //UART2 UART3
#else
#define UART_PIN_MAX  (1)  //EG500U only UART2
#endif


/*===========================================================================
 * Enum
 ===========================================================================*/
typedef enum 
{
    QUEC_UART_PORT_1,
    QUEC_UART_PORT_2,
    QUEC_UART_PORT_3,
    QUEC_PORT_MAX,
}quec_uart_port_number_e;
 
typedef struct
{
    quec_uart_port_number_e port;
    uint8_t tx_pin;
    uint8_t tx_func;
    uint8_t rx_pin;
    uint8_t rx_func;
} quec_uart_func_s;

typedef struct
{
	uint32_t		            dev_name;
	quec_uart_port_number_e	    uart_port;
    fdlChannel_t*               channel;
} quec_uart_info_t;

typedef struct
{
    uint32_t baudrate;
	uint32_t icf_format;
	uint32_t icf_parity;
	uint32_t ifc_rx; 
    uint32_t ifc_tx;
} quec_uart_setting_s;

/*===========================================================================
 * Functions declaration
 ===========================================================================*/

/*****************************************************************
* Function: quec_verify_output
*
* Description:
*  验证当前block 是否urc输出.
* 
* Parameters:
*	block_count     [in]        总块数  
*   block        	[in]        当前块数
*
* Return:
* 	true			Verify OK
*	false           Verify fail	
*
*****************************************************************/
bool quec_verify_output(int block_count, int block);


/*****************************************************************
* Function: quec_boot_urc_to_uart
*
* Description:
*  urc broadcast to uart.
* 
* Parameters:
*	buf_uart        [in]        string buffer  
*   len        	    [in]        string length
*
* Return:
* 	0			    success
*	-1           	error	
*
*****************************************************************/
int quec_boot_urc_send(unsigned char* buf_uart, int len);

/*****************************************************************
* Function: quec_boot_urc_init
*
* Description:
*  boot urc init.
* 
* Parameters:
*
* Return:
* 	0			    success
*	-1           	error	
*
*****************************************************************/
int quec_boot_urc_init();


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_BOOT_UART_H */


