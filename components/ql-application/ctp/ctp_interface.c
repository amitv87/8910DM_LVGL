#include "stdint.h"
#include "stdlib.h"
#include <stdio.h>
#include <string.h>

#include "ql_gpio.h"
#include "ctp_interface.h"
#include "ql_api_rtc.h"

#include "hal_chip.h"

static uint8_t ic_index;



extern  ctp_funcs_t ctp_custom_func_bl;

static ctp_funcs_t *CTPFunc_list[] =
{
    &ctp_custom_func_bl,
};


ctp_funcs_t *ctp_GetFunc(void)
{

    int i;

    //power on touch ic
   // halPmuSetPowerLevel(HAL_POWER_SIM1, SENSOR_VDD_1800MV);
   // halPmuSwitchPower(HAL_POWER_SIM1, true, false);
   
    ql_pin_set_func(61, 0); 
    ql_gpio_init(GPIO_0, GPIO_OUTPUT, 0x0, LVL_HIGH);
    ql_gpio_set_level(GPIO_0, LVL_HIGH);

    ql_pin_set_func(60, 0); 
    ql_gpio_init(GPIO_3, GPIO_OUTPUT, 0x0, LVL_HIGH);
    ql_gpio_set_level(GPIO_3, LVL_HIGH);


    for(i=0; i < sizeof(CTPFunc_list)/sizeof(ctp_funcs_t *); i++)
    {
        if(CTPFunc_list[i]->init() == 1)
        {
            CTP_LOG( "ctp_GetFunc, tp detect:%s", CTPFunc_list[i]->get_info()->fw_version); 
            ic_index = i;
            return CTPFunc_list[i];  
        }
    }

    CTP_LOG( "No supported ctp detected");    	

    return &ctp_custom_func_bl;
}



