#ifndef __CTP_INTERFACE_H__
#define __CTP_INTERFACE_H__
#include "stdint.h"
#include "stdlib.h"
#include <stdio.h>
#include <string.h>

#include "ql_api_rtc.h"
#include "ql_log.h"
#include "ql_gpio.h"

#define GPIO_CTP_I2C_SDA_PIN_INTER    2
#define GPIO_CTP_I2C_SCL_PIN_INTER    1
#define	CTP_RESET_PIN_INTER           4
#define	CTP_EINT_PIN_INTER            3

#define GPIO_CTP_I2C_SDA_PIN_MODE     0
#define GPIO_CTP_I2C_SCL_PIN_MODE     0
#define	CTP_RESET_PIN_MODE            0
#define	CTP_EINT_PIN_MODE             0


#define GPIO_CTP_I2C_SDA_PIN  	GPIO_12
#define GPIO_CTP_I2C_SCL_PIN  	GPIO_9
#define	CTP_RESET_PIN		    GPIO_10
#define  CTP_EINT_PIN           GPIO_11

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define CTP_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define CTP_LOG(msg, ...)         QL_LOG(CTP_LOG_LEVEL, "CTP", msg, ##__VA_ARGS__)
#define CTP_LOG_PUSH(msg, ...)    QL_LOG_PUSH("CTP", msg, ##__VA_ARGS__)


#define MAX_TOUCH_POINT    (2)
typedef struct {
    int16_t x;
    int16_t y;
}ctp_point_t;

typedef enum {
    PEN_UP,
    PEN_DOWN
}ctp_pen_state_enum_t;

typedef struct
{
    uint8_t model;    //points
    uint32_t time_stamp;
    ctp_point_t points[MAX_TOUCH_POINT];

}multi_touch_event_t;


#define MAX_TOUCH_IC_NAME_LEN     (16)
typedef struct {
    //触摸IC名字，字符串
    uint8_t ic[MAX_TOUCH_IC_NAME_LEN];
    //当前触摸固件版本（cob项目）
    uint8_t fw_version[8];
}ctp_info_t;

typedef uint8_t (*tp_init_func)(void);
typedef void (*tp_deinit_func)(void);
typedef void (*tp_power_func)(uint8_t on);
typedef void (*tp_read_func)(multi_touch_event_t *event);
typedef ctp_info_t* (*tp_get_info)(void);

typedef struct {
     tp_init_func init;
     tp_deinit_func deinit;
     tp_power_func  power;
     tp_read_func   read;
     tp_get_info    get_info;
}ctp_funcs_t;
#endif