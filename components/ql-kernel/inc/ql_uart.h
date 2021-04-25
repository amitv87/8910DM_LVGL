/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_UART_H
#define QL_UART_H

#include "ql_api_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_UART_ERRCODE_BASE    (QL_COMPONENT_BSP_UART<<16)

#define QL_UART_MAX_NUM         3
/*===========================================================================
 * Enum
 ===========================================================================*/
typedef enum
{
    QL_UART_SUCCESS             = 0,
    QL_UART_EXECUTE_ERR         = 1|QL_UART_ERRCODE_BASE,
    QL_UART_MEM_ADDR_NULL_ERR,
    QL_UART_INVALID_PARAM_ERR,
    QL_UART_OPEN_REPEAT_ERR,
    QL_UART_NOT_OPEN_ERR        = 5|QL_UART_ERRCODE_BASE,
} ql_uart_errcode_e;

typedef enum 
{
	QL_UART_PORT_1,
	QL_UART_PORT_2,
	QL_UART_PORT_3,
	QL_USB_PORT_AT,
	QL_USB_PORT_MODEM,
	QL_USB_PORT_NMEA,
	QL_PORT_MAX,
}ql_uart_port_number_e;

typedef enum
{
	QL_FC_NONE = 0,
	QL_FC_HW,
}ql_uart_flowctrl_e;

typedef enum
{
    QL_UART_BAUD_2400       = 2400,
    QL_UART_BAUD_4800       = 4800,
    QL_UART_BAUD_9600       = 9600,
    QL_UART_BAUD_14400      = 14400,
    QL_UART_BAUD_19200      = 19200,
    QL_UART_BAUD_28800      = 28800,
    QL_UART_BAUD_33600      = 33600,
    QL_UART_BAUD_38400      = 38400,
    QL_UART_BAUD_57600      = 57600,
    QL_UART_BAUD_115200     = 115200,
    QL_UART_BAUD_230400     = 230400,
    QL_UART_BAUD_460800     = 460800,
    QL_UART_BAUD_921600     = 921600,
	QL_UART_BAUD_1000000	= 1000000,
	QL_UART_BAUD_4468750    = 4468750
}ql_uart_baud_e;

typedef enum
{
	QL_UART_DATABIT_7 = 7,
	QL_UART_DATABIT_8 = 8,      //8910 ARM UART hardware only support 8bit Data
}ql_uart_databit_e;

typedef enum
{
	QL_UART_STOP_1  =1,         
	QL_UART_STOP_2  =2,
}ql_uart_stopbit_e;

typedef enum
{
	QL_UART_PARITY_NONE,
    QL_UART_PARITY_ODD,
	QL_UART_PARITY_EVEN,
}ql_uart_parityit_e;

/*===========================================================================
 * Struct
 ===========================================================================*/

typedef struct
{
	ql_uart_baud_e baudrate;
	ql_uart_databit_e data_bit;
	ql_uart_stopbit_e stop_bit;
	ql_uart_parityit_e parity_bit;
	ql_uart_flowctrl_e flow_ctrl;
}ql_uart_config_s;


/*****************************************************************
* Description: UART callback
* 
* Parameters:
*   ind_type        [in]    事件类型，有UART RX接收数据和RX buffer溢出两种。 
* 	port	  		[in] 	来数据的串口 
* 	size	  		[in] 	数据大小
*
*****************************************************************/
typedef void (*ql_uart_callback)(uint32 ind_type, ql_uart_port_number_e port, uint32 size);

/*===========================================================================
 * Functions declaration
 ===========================================================================*/

/*****************************************************************
* Function: ql_uart_set_dcbconfig
*
* Description: 设置串口属性，设置完重新打开uart口生效
* 
* Parameters:
*   port        [in]    串口选择。 
* 	dcb	  		[in] 	串口属性配置结构体 
*
* Return:
* 	0			设置成功。
*	other 	    错误码。
*
*****************************************************************/
ql_uart_errcode_e ql_uart_set_dcbconfig(ql_uart_port_number_e port, ql_uart_config_s *dcb);

/*****************************************************************
* Function: ql_uart_get_dcbconfig
*
* Description: 获取串口属性
* 
* Parameters:
*   port        [in]    串口选择。 
* 	dcb	  		[out] 	串口属性配置结构体 
*
* Return:
* 	0			获取成功。
*	other 	    错误码。
*
*****************************************************************/
ql_uart_errcode_e ql_uart_get_dcbconfig(ql_uart_port_number_e port, ql_uart_config_s *dcb);

/*****************************************************************
* Function: ql_uart_open
*
* Description: 打开 UART 设备
* 
* Parameters:
*   port        [in]    串口选择。 
*
* Return:
* 	0			设置成功。
*	other 	    错误码。
*
*****************************************************************/
ql_uart_errcode_e ql_uart_open(ql_uart_port_number_e port);

/*****************************************************************
* Function: ql_uart_close
*
* Description: 关闭 UART 设备
* 
* Parameters:
*   port        [in]    串口选择。 
*
* Return:
* 	0			设置成功。
*	other 	    错误码。
*
*****************************************************************/
ql_uart_errcode_e ql_uart_close(ql_uart_port_number_e port);

/*****************************************************************
* Function: ql_uart_write
*
* Description: UART写
* 
* Parameters:
*   port        [in]    串口选择。 
* 	data	    [in] 	要写入的数据 
*   data_len    [in]    要写入的数据长度
*
* Return:
*   < 0         错误码
*	other 	    写入的实际字节长度。
*
*****************************************************************/
int ql_uart_write(ql_uart_port_number_e port, unsigned char *data, unsigned int data_len);

/*****************************************************************
* Function: ql_uart_read
*
* Description: UART读
* 
* Parameters:
*   port        [in]    串口选择。 
* 	data	    [out] 	读取的数据 
*   data_len    [in]    要读取的数据长度
*
* Return:
*   < 0         错误码
*	other 	    实际读取的数据长度
*
*****************************************************************/
int ql_uart_read(ql_uart_port_number_e port, unsigned char *data, unsigned int data_len);

/*****************************************************************
* Function: ql_uart_register_cb
*
* Description: 注册UART口读取数据回调函数
* 
* Parameters:
*   port        [in]    串口选择。 
*   uart_cb     [in]    需要注册的回调函数，若为NULL,则取消注册callback，可使用轮询读取数据
*
* Return:
* 	0			注册成功。
*	other 	    错误码。
*
*****************************************************************/
ql_uart_errcode_e ql_uart_register_cb(ql_uart_port_number_e port, ql_uart_callback uart_cb);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_UART_H */


