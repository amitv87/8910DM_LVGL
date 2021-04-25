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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_uart.h"
#include "ql_gpio.h"


/*===========================================================================
 *Definition
 ===========================================================================*/
#define QL_UART_DEMO_LOG_LEVEL			QL_LOG_LEVEL_INFO
#define QL_UART_DEMO_LOG(msg, ...)		QL_LOG(QL_UART_DEMO_LOG_LEVEL, "ql_uart_demo", msg, ##__VA_ARGS__)


#define QL_UART_TASK_STACK_SIZE     		4096
#define QL_UART_TASK_PRIO          	 	    APP_PRIORITY_NORMAL
#define QL_UART_TASK_EVENT_CNT      		5


#define QL_UART_RX_BUFF_SIZE                2048
#define QL_UART_TX_BUFF_SIZE                2048

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define QL_UART2_TX_PIN						138
#define QL_UART2_TX_FUNC					0x03

#define QL_UART2_RX_PIN						137
#define QL_UART2_RX_FUNC					0x03


/*===========================================================================
 * Variate
 ===========================================================================*/

/*===========================================================================
 * Functions
 ===========================================================================*/

void ql_uart_notify_cb(unsigned int ind_type, ql_uart_port_number_e port, unsigned int size)
{
    unsigned char *recv_buff = calloc(1, QL_UART_RX_BUFF_SIZE+1);
    unsigned int real_size = 0;
    int read_len = 0;
    
    QL_UART_DEMO_LOG("UART port %d receive ind type:0x%x, receive data size:%d", port, ind_type, size);
    switch(ind_type)
    {
        case QUEC_UART_RX_OVERFLOW_IND:  //rx buffer overflow
        case QUEC_UART_RX_RECV_DATA_IND:
        {
            while(size > 0)
            {
                memset(recv_buff, 0, QL_UART_RX_BUFF_SIZE+1);
                real_size= MIN(size, QL_UART_RX_BUFF_SIZE);
                
                read_len = ql_uart_read(port, recv_buff, real_size);
                QL_UART_DEMO_LOG("read_len=%d, recv_data=%s", read_len, recv_buff);
                if((read_len > 0) && (size >= read_len))
                {
                    size -= read_len;
                }
                else
                {
                    break;
                }
            }
            break;
        }
    }
    free(recv_buff);
    recv_buff = NULL;
}

static void ql_uart_demo_thread(void *param)
{
    int ret = 0;
	QlOSStatus err = 0;
    ql_uart_config_s uart_cfg = {0};
    int write_len = 0;
    unsigned char data[] = "hello uart demo\r\n";

    uart_cfg.baudrate = QL_UART_BAUD_115200;
    uart_cfg.flow_ctrl = QL_FC_NONE;
    uart_cfg.data_bit = QL_UART_DATABIT_8;
    uart_cfg.stop_bit = QL_UART_STOP_1;
    uart_cfg.parity_bit = QL_UART_PARITY_NONE;

    ret = ql_uart_set_dcbconfig(QL_UART_PORT_2, &uart_cfg);
    QL_UART_DEMO_LOG("ret: 0x%x", ret);
	if(QL_UART_SUCCESS != ret)
	{
		goto exit;
	}

	ret = ql_pin_set_func(QL_UART2_TX_PIN, QL_UART2_TX_FUNC);
	if(QL_GPIO_SUCCESS != ret)
	{
		goto exit;
	}
	ret = ql_pin_set_func(QL_UART2_RX_PIN, QL_UART2_RX_FUNC);
	if(QL_GPIO_SUCCESS != ret)
	{
		goto exit;
	}

    ret = ql_uart_open(QL_UART_PORT_2);
    QL_UART_DEMO_LOG("ret: 0x%x", ret);

	if(QL_UART_SUCCESS == ret)
	{
	    ret = ql_uart_register_cb(QL_UART_PORT_2, ql_uart_notify_cb);
	    QL_UART_DEMO_LOG("ret: 0x%x", ret);

	    memset(&uart_cfg, 0, sizeof(ql_uart_config_s));
	    ret = ql_uart_get_dcbconfig(QL_UART_PORT_2, &uart_cfg);
	    QL_UART_DEMO_LOG("ret: 0x%x, baudrate=%d, flow_ctrl=%d, data_bit=%d, stop_bit=%d, parity_bit=%d", 
	                        ret, uart_cfg.baudrate, uart_cfg.flow_ctrl, uart_cfg.data_bit, uart_cfg.stop_bit, uart_cfg.parity_bit);
	    
	    while(1)
	    {   
	        write_len = ql_uart_write(QL_UART_PORT_2, data, strlen((char *)data));
	        QL_UART_DEMO_LOG("write_len:%d", write_len);

	        ql_rtos_task_sleep_ms(20000);
	    }
	}

exit:
    err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_UART_DEMO_LOG("task deleted failed");
	}
}

void ql_uart_app_init(void)
{
	QlOSStatus err = 0;
	ql_task_t uart_task = NULL;
	
	err = ql_rtos_task_create(&uart_task, QL_UART_TASK_STACK_SIZE, QL_UART_TASK_PRIO, "QUARTDEMO", ql_uart_demo_thread, NULL, QL_UART_TASK_EVENT_CNT);
	if (err != QL_OSI_SUCCESS)
	{
		QL_UART_DEMO_LOG("demo task created failed");
        return;
	}
}


