
#ifndef _BL_TS_H_ 
#define _BL_TS_H_

#include "stdint.h"
#include "stdlib.h"
#include <stdio.h>
#include <string.h>

#include "ql_api_osi.h"
#include "osi_api.h"
#include "ql_log.h"

#include "bl_chip_custom.h"
#include "ctp_interface.h"

#define  CTP_I2C_DELAY			100

#define   MDELAY(n)	    osiThreadSleep(n)	



#define CTP_GPIO_INIT           {   ql_pin_set_func(GPIO_CTP_I2C_SDA_PIN_INTER, GPIO_CTP_I2C_SDA_PIN_MODE); \
									ql_pin_set_func(GPIO_CTP_I2C_SCL_PIN_INTER, GPIO_CTP_I2C_SCL_PIN_MODE);\
									ql_pin_set_func(CTP_RESET_PIN_INTER, CTP_RESET_PIN_MODE);\
									ql_pin_set_func(CTP_EINT_PIN_INTER, CTP_EINT_PIN_MODE); \
									ql_gpio_deinit(GPIO_CTP_I2C_SDA_PIN);\
									ql_gpio_deinit(GPIO_CTP_I2C_SCL_PIN);\
									ql_gpio_deinit(CTP_RESET_PIN);\
									ql_gpio_deinit(CTP_EINT_PIN);\
									ql_gpio_init(GPIO_CTP_I2C_SDA_PIN, GPIO_INPUT, PULL_UP, 0xff);\
									ql_gpio_init(GPIO_CTP_I2C_SCL_PIN, GPIO_OUTPUT, 0xff, LVL_HIGH);\
									ql_gpio_init(CTP_RESET_PIN, GPIO_OUTPUT, 0xff, LVL_HIGH);\
									ql_gpio_init(CTP_EINT_PIN, GPIO_INPUT, PULL_UP, 0xff);}

#define CTP_SET_RESET_PIN_OUTPUT	ql_gpio_set_direction(CTP_RESET_PIN, GPIO_OUTPUT);
#define CTP_SET_RESET_PIN_HIGH		ql_gpio_set_level(CTP_RESET_PIN, LVL_HIGH);
#define CTP_SET_RESET_PIN_LOW		ql_gpio_set_level(CTP_RESET_PIN, LVL_LOW);

#define CTP_SET_I2C_EINT_OUTPUT		ql_gpio_set_direction(CTP_EINT_PIN, GPIO_OUTPUT);
#define CTP_SET_I2C_EINT_INPUT		ql_gpio_set_direction(CTP_EINT_PIN, GPIO_INPUT);
#define CTP_SET_I2C_EINT_HIGH		ql_gpio_set_level(CTP_EINT_PIN, LVL_HIGH);
#define CTP_SET_I2C_EINT_LOW		ql_gpio_set_level(CTP_EINT_PIN, LVL_LOW);


#define CTP_WRITE		  CTP_SLAVE_ADDR
#define CTP_READ		  (CTP_SLAVE_ADDR+1)
#define CTP_ACK_COUNTER	10

#define CTP_SET_I2C_CLK_OUTPUT		ql_gpio_set_direction(GPIO_CTP_I2C_SCL_PIN, GPIO_OUTPUT);
#define CTP_SET_I2C_DATA_OUTPUT		ql_gpio_set_direction(GPIO_CTP_I2C_SDA_PIN, GPIO_OUTPUT);
#define CTP_SET_I2C_DATA_INPUT		ql_gpio_set_direction(GPIO_CTP_I2C_SDA_PIN, GPIO_INPUT);
#define CTP_SET_I2C_CLK_HIGH		ql_gpio_set_level(GPIO_CTP_I2C_SCL_PIN, LVL_HIGH);
#define CTP_SET_I2C_CLK_LOW			ql_gpio_set_level(GPIO_CTP_I2C_SCL_PIN, LVL_LOW);

#define CTP_GET_I2C_CLK_BIT			bl_ctp_get_clk_level()
#define CTP_SET_I2C_CLK_INPUT		ql_gpio_set_direction(GPIO_CTP_I2C_SCL_PIN, GPIO_INPUT);

#define CTP_SET_I2C_DATA_HIGH		ql_gpio_set_level(GPIO_CTP_I2C_SDA_PIN, LVL_HIGH);
#define CTP_SET_I2C_DATA_LOW		ql_gpio_set_level(GPIO_CTP_I2C_SDA_PIN, GPIO_INPUT);

#define CTP_GET_I2C_DATA_BIT		bl_ctp_get_data_level()


#define CTP_I2C_START_BIT \
	{ \
		volatile unsigned int j; \
		CTP_SET_I2C_CLK_OUTPUT; \
		CTP_SET_I2C_DATA_OUTPUT; \
		CTP_SET_I2C_CLK_HIGH; \
		CTP_SET_I2C_DATA_HIGH; \
		for(j=0;j<CTP_I2C_DELAY;j++);\
		CTP_SET_I2C_DATA_LOW; \
		for(j=0;j<CTP_I2C_DELAY;j++);\
		CTP_SET_I2C_CLK_LOW; \
	}

	#define CTP_I2C_STOP_BIT \
	{ \
		volatile unsigned int j; \
		CTP_SET_I2C_CLK_OUTPUT; \
		CTP_SET_I2C_DATA_OUTPUT; \
		CTP_SET_I2C_CLK_LOW; \
		CTP_SET_I2C_DATA_LOW; \
		for(j=0;j<CTP_I2C_DELAY;j++);\
		CTP_SET_I2C_CLK_HIGH; \
		for(j=0;j<CTP_I2C_DELAY;j++);\
		CTP_SET_I2C_DATA_HIGH; \
		for(j=0;j<CTP_I2C_DELAY;j++);\
	}

extern char CTP_FLASH_I2C_WRITE(unsigned char i2c_addr, unsigned char *value, unsigned short len);
extern char CTP_FLASH_I2C_READ(unsigned char i2c_addr, unsigned char *value, unsigned short len);
#endif
