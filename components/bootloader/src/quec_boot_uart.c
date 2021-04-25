/**  
  @file
  quec_boot_uart.c

  @brief
  quectel boot uart interface.

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



/*===========================================================================
 * include files
 ===========================================================================*/
#include "osi_log.h"
#include "osi_api.h"
#include "quec_boot_uart.h"
#include "boot_uart.h"
#include "vfs.h"
#include "quec_boot_fota.h"


#define QUEC_CONFIG_DIR_ROOT	    "/config"
#define QUEC_UART_SETTING  QUEC_CONFIG_DIR_ROOT"/quartsetting.nv"


/*========================================================================
 *  Global Variable
 *========================================================================*/  
quec_uart_info_t quec_uart_info[QUEC_PORT_MAX] = {
	{DRV_NAME_UART1,	QUEC_UART_PORT_1,   NULL},
	{DRV_NAME_UART2,	QUEC_UART_PORT_2,   NULL},
	{DRV_NAME_UART3,	QUEC_UART_PORT_3,   NULL},
};

quec_uart_func_s quec_uart_pin_func[UART_PIN_MAX] =
{
#ifdef CONFIG_QL_PROJECT_DEF_EC200U
    {QUEC_UART_PORT_2, QUEC_PIN_UART2_TXD, 3, QUEC_PIN_UART2_RXD, 3},
    {QUEC_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4}
#elif defined CONFIG_QL_PROJECT_DEF_EC600U
#ifndef __QUEC_OEM_VER_LD__
    {QUEC_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    {QUEC_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4}
#else
    {QUEC_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    {QUEC_UART_PORT_3, QUEC_PIN_UART3_TXD, 6, QUEC_PIN_UART3_RXD, 6}
#endif
#elif defined CONFIG_QL_PROJECT_DEF_EG700U
    {QUEC_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    {QUEC_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4}
#elif defined CONFIG_QL_PROJECT_DEF_EG500U
    {QUEC_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    //{QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4}
#endif
};

extern quec_boot_pin_cfg_t quec_pin_cfg_map[];
int g_quec_boot_uart_baud = 115200;
float updata_urc_step = 0.0;
extern int updata_urc_num;
bool urc_output_first = true;



/*===========================================================================
 * Functions declaration
 *===========================================================================*/
extern int prvUartWrite(fdlChannel_t *ch, const void *data, size_t size);


/*===========================================================================
 * Functions Definition
 *===========================================================================*/
static int quec_pin_index_check(uint8_t pin_num)
{
    int index;

    for( index = 0; index < QUEC_PIN_CFG_MAX; index++ )
    {
        if( pin_num == quec_pin_cfg_map[index].pin_num )
        {
            return index;
        }
    }

    return -1;
}

static int quec_pin_get_func(uint8_t pin_num, uint8_t *func_sel)
{
    int index = 0;

    index = quec_pin_index_check(pin_num);
    if( -1 == index )
    {
        OSI_LOGI(0,"pin[%d] isn't multiple pin!", pin_num);
        return -1;
    }

    *func_sel = *(quec_pin_cfg_map[index].reg) & 0xf;
    OSI_LOGI(0,"pin[%d] func_sel is get %d!", pin_num, *func_sel);

    return 0;
}

static int quec_pin_set_func_internal(uint8_t pin_num, uint8_t func_sel)
 {
     int index = 0;
 
     index = quec_pin_index_check(pin_num);
     if( -1 == index )
     {
         OSI_LOGI(0,"pin[%d] isn't multiple pin!", pin_num);
         return -1;
     }
 
     if( func_sel > 8 )
     {
         OSI_LOGI(0,"pin[%d] haven't function[%d]!", pin_num, func_sel);
         return -1;
     }
 
     *(quec_pin_cfg_map[index].reg) = func_sel;
     OSI_LOGI(0,"pin[%d] func_sel is set %d!", pin_num, *(quec_pin_cfg_map[index].reg) & 0xf);

     return 0;
 }
 
static int quec_boot_uart_pin_init(quec_uart_port_number_e port)
{
    uint8_t index = 0;
    uint8_t tx_func_sel = 0;
    uint8_t rx_func_sel = 0;
    
    if(port >= QUEC_PORT_MAX)
    {
        return -1;
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GNSS 
    if(port == QUEC_UART_PORT_3)
    {
        return -1;
    }
#endif

    for(index = 0; index < UART_PIN_MAX; index++)
    {
        if(port == quec_uart_pin_func[index].port)
        {
            if((0 != quec_pin_get_func(quec_uart_pin_func[index].tx_pin, &tx_func_sel))
                || (0 != quec_pin_get_func(quec_uart_pin_func[index].rx_pin, &rx_func_sel)))
            {
                OSI_LOGI(0,"get pin function error");
                return -1;
            }

            if((tx_func_sel == quec_uart_pin_func[index].tx_func) && (rx_func_sel == quec_uart_pin_func[index].rx_func))
            {
                OSI_LOGI(0,"same function as before");
                return 0;
            }

            if((0 == quec_pin_set_func_internal(quec_uart_pin_func[index].tx_pin, quec_uart_pin_func[index].tx_func))
                && (0 == quec_pin_set_func_internal(quec_uart_pin_func[index].rx_pin, quec_uart_pin_func[index].rx_func)))
            {
                return 0;
            }
            else
            {
                //Restore the original function
                quec_pin_set_func_internal(quec_uart_pin_func[index].tx_pin, tx_func_sel);
                quec_pin_set_func_internal(quec_uart_pin_func[index].rx_pin, rx_func_sel);
                return -1;
            }
        }
    }
    return -1;
}

static int quec_boot_write_to_uart(quec_uart_port_number_e port, unsigned char* data_buff,unsigned int len)
{ 
    if(port >= QUEC_PORT_MAX)
    {
        return -1;
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GNSS 
    if(port == QUEC_UART_PORT_3)
    {
        return -1;
    }
#endif
    fdlChannel_t* channel_uart = quec_uart_info[port].channel;
    if(NULL == channel_uart)
    {
        OSI_LOGI(0,"this channel not init!");
        return -1;
    }

    int write_len = prvUartWrite(channel_uart, data_buff, len); 
    
    OSI_LOGI(0, "boot wrtie %d bytes data to uart%d", write_len, port + 1);
    
    return 0;
}

static int quec_boot_uart_open(quec_uart_port_number_e port, uint32_t baud)
{
    if(port >= QUEC_PORT_MAX)
    {
        return -1;
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GNSS 
    if(port == QUEC_UART_PORT_3)
    {
        return -1;
    }
#endif
	uint32_t drv_name = quec_uart_info[port].dev_name;
	if(0 == drv_name)
	{
		return -1;
	}

    fdlChannel_t* ch_uart;
    
    OSI_LOGI(0, "boot uart%d baud set %d", port+1, baud);
    if(baud <= 0)
    {
#ifndef __QUEC_OEM_VER_LD__    
        baud = 115200;
        OSI_LOGI(0, "boot uart baud set error, change to 115200");
#else
        baud = 460800;
#endif 
    }
    ch_uart = fdlOpenUart(drv_name, baud, true);
    if (ch_uart == NULL)
    {
        OSI_LOGI(0, "Failed to open %4c", port);
        return -1;
    }
    OSI_LOGI(0, "boot uart%d open success", port + 1);
    quec_uart_info[port].channel = ch_uart;
    return 0;
}

static int quec_boot_load_uart_config(quec_uart_setting_s* config)
{  
    ssize_t file_size = vfs_sfile_size(QUEC_UART_SETTING);
    
    if (file_size < 0)
    {
        OSI_LOGI(0,"boot load uart setting size error");
        return -1;
    }
    
    if(vfs_sfile_read(QUEC_UART_SETTING, config, file_size) < 0)
    {
        OSI_LOGI(0,"boot load uart setting content error");
        return -1;
    }
    return 0;
}

bool quec_verify_output(int block_count, int block)
{
    int output_value = 0, verify_value = 0, temp_count = 1;
    static int urc_count = 1;

    if(urc_output_first == true && block > 1)
    {       
        while(temp_count < block)
        {
            output_value = (int)(urc_count * updata_urc_step);
            verify_value = temp_count*100/block_count;
            if(verify_value >= output_value)
            {
                urc_count++;   
            }
            temp_count++;
        }
    }
    urc_output_first = false;
    
    if(updata_urc_num == 0)
    {
        return false;
    }
    
    if(block_count <= updata_urc_num)
    {
        return true;
    }

    output_value = (int)(urc_count * updata_urc_step);
    verify_value = block*100/block_count;
    
    if(verify_value >= output_value)
    {
        urc_count++;    
        return true;
    }
    else 
    {
        return false;
    }
}

int quec_boot_urc_send(unsigned char* buf_uart, int len)
{
    if(NULL == buf_uart || len < 1)
    {
        return -1;
    }
    quec_boot_write_to_uart(QUEC_UART_PORT_1, buf_uart, len);
    quec_boot_write_to_uart(QUEC_UART_PORT_2, buf_uart, len);
#ifndef CONFIG_QUEC_PROJECT_FEATURE_GNSS 
    quec_boot_write_to_uart(QUEC_UART_PORT_3, buf_uart, len);
#endif
    return 0;
}

int quec_boot_urc_init()
{   
    quec_uart_setting_s uart_config[QUEC_PORT_MAX] = {0};
    
    if(quec_boot_load_uart_config(uart_config) < 0)
    {
        OSI_LOGI(0,"boot load uart setting error");
    }

    if(updata_urc_num > 0)
    {
        updata_urc_step = 100.00/updata_urc_num;
    }
    
    quec_boot_uart_pin_init(QUEC_UART_PORT_2);
    quec_boot_uart_open(QUEC_UART_PORT_1, uart_config[QUEC_UART_PORT_1].baudrate);
    quec_boot_uart_open(QUEC_UART_PORT_2, uart_config[QUEC_UART_PORT_2].baudrate);
    
#ifndef CONFIG_QUEC_PROJECT_FEATURE_GNSS        
    quec_boot_uart_pin_init(QUEC_UART_PORT_3);       
    quec_boot_uart_open(QUEC_UART_PORT_3, uart_config[QUEC_UART_PORT_3].baudrate); 
#endif
    return 0;
}



