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

=================================================================*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"

#include "ql_api_spi_flash.h"

#include "ql_log.h"
#include "spi_flash_demo.h"
#include "ql_gpio.h"


#define QL_SPI_NOR_LOG_LEVEL       	        QL_LOG_LEVEL_INFO
#define QL_SPI_NOR_LOG(msg, ...)			QL_LOG(QL_SPI_NOR_LOG_LEVEL, "ql_SPI_NOR_DEMO", msg, ##__VA_ARGS__)
#define QL_SPI_NOR_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_SPI_NOR_DEMO", msg, ##__VA_ARGS__)


ql_task_t spi_flash_demo_task = NULL;

#define QL_CUR_SPI_PORT             QL_SPI_PORT1 
//extern ql_errcode_gpio ql_pin_set_func(uint8_t pin_num, uint8_t func_sel);
static void ql_spi_flash_demo_task_pthread(void *ctx)
{
    QlOSStatus err = 0;
    ql_errcode_spi_nor_e ret;  
    unsigned int addr = 0x000008;
    unsigned int len = 0;
    unsigned char *data = NULL;
    int i = 0;

    QL_SPI_NOR_LOG("enter ql_spi_flash_demo_task_pthread");

    ql_pin_set_func(QL_SPI1_CS_PIN, QL_SPI1_CS_FUNC);
    ql_pin_set_func(QL_SPI1_CLK_PIN, QL_SPI1_CLK_FUNC);
    ql_pin_set_func(QL_SPI1_DO_PIN, QL_SPI1_DO_FUNC);
    ql_pin_set_func(QL_SPI1_DI_PIN, QL_SPI1_DI_FUNC);
#if 0
    ret = ql_spi_nor_init(QL_SPI_PORT2, QL_SPI_CLK_26MHZ);
#else
    ql_spi_nor_config_s nor_config = {0};
    nor_config.port = QL_CUR_SPI_PORT;
    nor_config.spiclk = QL_SPI_CLK_25MHZ;
    nor_config.input_sel = QL_SPI_DI_1;
    nor_config.transmode = QL_SPI_DIRECT_POLLING;
    nor_config.cs = QL_SPI_CS0;
    ret = ql_spi_nor_init_ext(nor_config);
#endif
    if (ret == QL_SPI_FLASH_SUCCESS)
    {
#if 1
        ret = ql_spi_nor_erase_sector(QL_CUR_SPI_PORT, addr);
        //ret = ql_spi_nor_erase_64k_block(QL_SPI_NOR_PORT2, addr);
        //ret = ql_spi_nor_erase_chip(QL_SPI_NOR_PORT2);
        if (ret != QL_SPI_FLASH_SUCCESS)
        {
            QL_SPI_NOR_LOG("erase sector error=%x", ret);
            goto QL_SPI_NOR_EXIT;
        }
#endif        
        len = 1024;
        data = malloc(len+1);
        if (data == NULL)
        {
            QL_SPI_NOR_LOG("malloc is error");
            goto QL_SPI_NOR_EXIT;
        }
        
        memset(data,0x00,len+1);
        for (i = 0; i < len; i++)
        {
            data[i] = 'i';
        }

        int p = 0;
        int q = 0;

        //while(1)
        {
            p += 1;
            ql_rtos_task_sleep_ms(100);
#if 1
            QL_SPI_NOR_LOG("write count---------p=%d", p);
            QL_SPI_NOR_LOG("write data:%s", data);
            ret = ql_spi_nor_write(QL_CUR_SPI_PORT, data, addr, len);
            if (ret != QL_SPI_FLASH_SUCCESS)
            {
                free(data);
                QL_SPI_NOR_LOG("write error=%x", ret);
                goto QL_SPI_NOR_EXIT;
            }
#endif
            memset(data,0x00,len+1);
            ql_spi_nor_read(QL_CUR_SPI_PORT, data, addr, len);
            if (ret != QL_SPI_FLASH_SUCCESS)
            {
                free(data);
                QL_SPI_NOR_LOG("read error=%x", ret);
                goto QL_SPI_NOR_EXIT;
            }
            QL_SPI_NOR_LOG("read data:%s", data);
#if 1
            if (p < 8)
            {
                addr = addr + len;
            }
            else 
            {
                p = 0;
                q ++;

                QL_SPI_NOR_LOG("erase count---------q=%d", q);
                ret = ql_spi_nor_erase_sector(QL_CUR_SPI_PORT, addr);
                //ret = ql_spi_nor_erase_64k_block(QL_SPI_NOR_PORT2, addr);
                //ret = ql_spi_nor_erase_chip(QL_SPI_NOR_PORT2);
                if (ret != QL_SPI_FLASH_SUCCESS)
                {
                    QL_SPI_NOR_LOG("erase sector error=%x", ret);
                    goto QL_SPI_NOR_EXIT;
                }
            }
#endif 
        }
        
       
        free(data);
        data = NULL;
        
    }
QL_SPI_NOR_EXIT:
    QL_SPI_NOR_LOG("ql_rtos_task_delete");
	err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_SPI_NOR_LOG("task deleted failed");
	}
}



QlOSStatus ql_spi_flash_demo_init(void)
{	
	QlOSStatus err = QL_OSI_SUCCESS;

    QL_SPI_NOR_LOG("enter ql_spi_flash_demo_init");
	err = ql_rtos_task_create(&spi_flash_demo_task, SPI_FLASH_DEMO_TASK_STACK_SIZE, SPI_FLASH_DEMO_TASK_PRIO, "ql_spi_flash", ql_spi_flash_demo_task_pthread, NULL, SPI_FLASH_DEMO_TASK_EVENT_CNT);
	if(err != QL_OSI_SUCCESS)
	{
		QL_SPI_NOR_LOG("demo_task created failed");
        return err;
	}

    return err;
}


