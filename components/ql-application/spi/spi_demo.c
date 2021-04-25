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

#include "ql_api_spi.h"

#include "ql_log.h"
#include "spi_demo.h"
#include "ql_gpio.h"


#define QL_SPI_DEMO_LOG_LEVEL       	        QL_LOG_LEVEL_INFO
#define QL_SPI_DEMO_LOG(msg, ...)			    QL_LOG(QL_SPI_DEMO_LOG_LEVEL, "ql_SPI_DEMO", msg, ##__VA_ARGS__)
#define QL_SPI_DEMO_LOG_PUSH(msg, ...)	        QL_LOG_PUSH("ql_SPI_DEMO", msg, ##__VA_ARGS__)


ql_task_t spi_demo_task = NULL;

//extern ql_errcode_gpio ql_pin_set_func(uint8_t pin_num, uint8_t func_sel);
static void ql_spi_demo_task_pthread(void *ctx)
{
    QlOSStatus err = 0;
    unsigned char outdata[256];
    unsigned short outlen;
    unsigned char indata[256];
    unsigned short inlen;

    ql_pin_set_func(QL_SPI2_CS_PIN, QL_SPI2_CS_FUNC);
    ql_pin_set_func(QL_SPI2_CLK_PIN, QL_SPI2_CLK_FUNC);
    ql_pin_set_func(QL_SPI2_DO_PIN, QL_SPI2_DO_FUNC);
    ql_pin_set_func(QL_SPI2_DI_PIN, QL_SPI2_DI_FUNC);
#if 0
    ql_spi_init(QL_SPI_PORT2, QL_SPI_DIRECT_POLLING, QL_SPI_CLK_26MHZ);
#else
    ql_spi_config_s spi_config;
    spi_config.input_mode = QL_SPI_INPUT_TURE;
    spi_config.port = QL_SPI_PORT2;
    spi_config.framesize = 8;
    spi_config.spiclk = QL_SPI_CLK_25MHZ;
    spi_config.cs_polarity0 = QL_SPI_CS_ACTIVE_LOW;
    spi_config.cs_polarity1 = QL_SPI_CS_ACTIVE_LOW;
    spi_config.cpol = QL_SPI_CPOL_LOW;
    spi_config.cpha = QL_SPI_CPHA_1Edge;
    spi_config.input_sel = QL_SPI_DI_1;
    spi_config.transmode = QL_SPI_DIRECT_POLLING;
    spi_config.cs = QL_SPI_CS0;
    ql_spi_init_ext(spi_config);
#endif
    //用来测试SPI CS脚可以强制拉低，CS脚恢复成系统控制后，不需要强制拉高拉低
    ql_spi_cs_low(QL_SPI_PORT2);
    ql_rtos_task_sleep_s(3);
    ql_spi_cs_auto(QL_SPI_PORT2);
    
    memcpy(outdata, "12345678", 8);
    outlen = 8;
    inlen = 8;
    ql_spi_write_read(QL_SPI_PORT2, outdata, outlen, indata, inlen);
    ql_spi_read(QL_SPI_PORT2, indata, inlen);
    ql_spi_write(QL_SPI_PORT2, outdata, outlen);
    ql_spi_release(QL_SPI_PORT2);
    QL_SPI_DEMO_LOG("ql_rtos_task_delete");
	err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_SPI_DEMO_LOG("task deleted failed");
	}
}



QlOSStatus ql_spi_demo_init(void)
{	
	QlOSStatus err = QL_OSI_SUCCESS;

    QL_SPI_DEMO_LOG("enter ql_spi_demo_init");
	err = ql_rtos_task_create(&spi_demo_task, SPI_DEMO_TASK_STACK_SIZE, SPI_DEMO_TASK_PRIO, "ql_spi_demo", ql_spi_demo_task_pthread, NULL, SPI_DEMO_TASK_EVENT_CNT);
	if(err != QL_OSI_SUCCESS)
	{
		QL_SPI_DEMO_LOG("demo_task created failed");
        return err;
	}

    return err;
}


