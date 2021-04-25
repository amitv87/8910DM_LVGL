#include "bl_chip_common.h"
#include "Tp_ts.h"
#include "ctp_interface.h"

extern int64_t ql_rtc_up_time_ms();

ctp_info_t  ctp_custom_information = 
{
	"BL6133",
	"NULL"
};

void ctp_delay_us(unsigned int time)
{
	unsigned int i,j,k;
	for(i=0;i< time;i++)
	{
		for(j=0;j<100;j++)
		{
			k=0;
		}
	}
}

//void MDELAY(unsigned short time)
//{
//	volatile unsigned int delay;
//	while(time--)	
//		for (delay =0;delay <50000;delay++) {} // 1ms
//}

void bl_ts_set_intmode(char mode)
{
	if(0 == mode)
	{//GPIO mode
		//GPIO_ModeSetup(CTP_EINT_PIN, 0);
		CTP_SET_I2C_EINT_OUTPUT;
	}
	else if(1 == mode)
	{//INT mode
		//GPIO_ModeSetup(CTP_EINT_PIN, 1);
		//GPIO_ModeSetup(CTP_EINT_PIN, CTP_EINT_PIN_M_EINT);
		CTP_SET_I2C_EINT_INPUT;
	}
}

uint8_t bl_ctp_get_clk_level(void)
{
	uint8_t level = 0;

	ql_gpio_get_level(GPIO_CTP_I2C_SCL_PIN, &level);
	return level;
}

uint8_t bl_ctp_get_data_level(void)
{
	uint8_t level = 0;

	ql_gpio_get_level(GPIO_CTP_I2C_SDA_PIN, &level);
	return level;
}

void bl_ts_set_intup(char level)
{
	CTP_SET_I2C_EINT_OUTPUT;
	if(level==1){
		CTP_SET_I2C_EINT_HIGH;
	}else{
		CTP_SET_I2C_EINT_LOW;
	}
}

#ifdef INT_PIN_WAKEUP
void bl_ts_int_wakeup(void)
{
	bl_ts_set_intmode(0);
	bl_ts_set_intup(1);
	MDELAY(20);
	bl_ts_set_intup(0);
	MDELAY(1);
	bl_ts_set_intup(1);
	MDELAY(20);
	bl_ts_set_intmode(1);
}
#endif

#ifdef  RESET_PIN_WAKEUP
void bl_ts_reset_wakeup(void)
{
	CTP_SET_RESET_PIN_OUTPUT;
	CTP_SET_RESET_PIN_HIGH;
	MDELAY(20);
	CTP_SET_RESET_PIN_LOW;
	MDELAY(20);
	CTP_SET_RESET_PIN_HIGH;
	MDELAY(20);
}
#endif

void ctp_i2c_init_sw(void)
{
	CTP_SET_I2C_CLK_OUTPUT;
	CTP_SET_I2C_CLK_HIGH;
	CTP_SET_I2C_DATA_OUTPUT;
	CTP_SET_I2C_DATA_HIGH;
}

char CTP_I2C_send_byte(char send_byte)
{
	volatile signed char i;
	volatile unsigned int j;
	volatile unsigned int k=0;
	char ack;

	for (i=7;i>=0;i--)
	{	/* data bit 7~0 */
		if (send_byte & (1<<i))
		{
			CTP_SET_I2C_DATA_HIGH;
		}
		else
		{
			CTP_SET_I2C_DATA_LOW;
		}
		for(j=0;j<CTP_I2C_DELAY;j++);
		CTP_SET_I2C_CLK_HIGH;
		for(j=0;j<CTP_I2C_DELAY;j++);
		CTP_SET_I2C_CLK_LOW;
		for(j=0;j<CTP_I2C_DELAY;j++);
	}

	CTP_SET_I2C_DATA_INPUT;
	CTP_SET_I2C_CLK_HIGH;
	for(j=0;j<CTP_I2C_DELAY;j++);

	for(k=0;k<CTP_ACK_COUNTER;k++)
	{
		if(CTP_GET_I2C_DATA_BIT == 0)
		{
			ack=1;
			break;
		}
		else
		{
			ack=0;
		}
	}

	CTP_SET_I2C_CLK_LOW;
	for(j=0;j<CTP_I2C_DELAY;j++);
	CTP_SET_I2C_DATA_OUTPUT;
	ctp_delay_us(2);
	return ack;
}	

char CTP_I2C_get_byte(void)
{
	volatile signed char i;
	volatile unsigned int j;
	unsigned char get_byte=0;

	CTP_SET_I2C_DATA_INPUT;
	for (i=7;i>=0;i--)
	{       /* data bit 7~0 */
		CTP_SET_I2C_CLK_LOW;
		for(j=0;j<CTP_I2C_DELAY;j++);
		CTP_SET_I2C_CLK_HIGH;
		for(j=0;j<CTP_I2C_DELAY;j++);
		if (CTP_GET_I2C_DATA_BIT)
			get_byte |= (1<<i);
	}
	/* don't care bit, 9th bit */

	for(j=0;j<CTP_I2C_DELAY;j++);
	CTP_SET_I2C_CLK_LOW;
	for(j=0;j<CTP_I2C_DELAY;j++);
	CTP_SET_I2C_DATA_OUTPUT;
	ctp_delay_us(2);
	return get_byte;
}	

unsigned char CTP_I2C_get_byte_with_ack(unsigned char uc_ack_lvl)
{
	volatile signed char i;
	volatile unsigned int j;
	unsigned char get_byte=0;

	CTP_SET_I2C_DATA_INPUT;
	for(j=0;j<CTP_I2C_DELAY;j++);
	for (i=7;i>=0;i--)
	{       /* data bit 7~0 */
		CTP_SET_I2C_CLK_LOW;
		for(j=0;j<CTP_I2C_DELAY;j++);
		CTP_SET_I2C_CLK_HIGH;
		for(j=0;j<CTP_I2C_DELAY;j++);
		if (CTP_GET_I2C_DATA_BIT)
			get_byte |= (1<<i);
		for(j=0;j<CTP_I2C_DELAY;j++);
	}

	/* don't care bit, 9th bit */
	for(j=0;j<CTP_I2C_DELAY;j++);
	CTP_SET_I2C_CLK_LOW;
	for(j=0;j<CTP_I2C_DELAY;j++);
	CTP_SET_I2C_DATA_OUTPUT;

	if (uc_ack_lvl == 1)
	{
		CTP_SET_I2C_DATA_HIGH;
	}
	else
	{
		CTP_SET_I2C_DATA_LOW;
	}

	for(j=0;j<CTP_I2C_DELAY;j++);
	CTP_SET_I2C_CLK_HIGH;

	for(j=0;j<CTP_I2C_DELAY;j++);
	CTP_SET_I2C_CLK_LOW;

	for(j=0;j<CTP_I2C_DELAY;j++);
	CTP_SET_I2C_DATA_LOW;
	ctp_delay_us(2);
	return get_byte;
}	

char CTP_FLASH_I2C_WRITE(unsigned char i2c_addr, unsigned char *value, unsigned short len) 
{

	volatile unsigned int j;
	unsigned char get_byte = 0;
	unsigned short i;

	CTP_I2C_START_BIT;       
	for(j=0;j<CTP_I2C_DELAY;j++);

	//if(!CTP_I2C_send_byte(reg<<1))	//FLASH_ADDR + WRITE
	if(!CTP_I2C_send_byte(i2c_addr))	//FLASH_ADDR + WRITE
		return(1);
	for(j=0;j<CTP_I2C_DELAY;j++);

	//for(i = 0; i< len - 1; i++)
	for(i = 0; i< len ; i++)
	{
		if(!CTP_I2C_send_byte(*value++)) //FLASH COMMAND
			return(1);
		for(j=0;j<CTP_I2C_DELAY;j++);
	}

	CTP_I2C_STOP_BIT;
	for(j=0;j<CTP_I2C_DELAY;j++);

	return 0;
}

char CTP_FLASH_I2C_READ(unsigned char i2c_addr, unsigned char *value, unsigned short len) 
{

	volatile unsigned int j;
	unsigned char get_byte = 0;
	unsigned short i;

	CTP_I2C_START_BIT;       
	//if(!CTP_I2C_send_byte( (reg<<1) | 1))	// FLASH READ
	if(!CTP_I2C_send_byte( i2c_addr | 0x01) )	// FLASH READ
		return(1);

	if (len <= 1)
	{
		//*value =CTP_I2C_get_byte();
		*value =CTP_I2C_get_byte_with_ack(1);
		for(j=0;j<CTP_I2C_DELAY;j++);
	}
	else
	{
		for (i = 0; i< len - 1; i++)
		{
			*value++ =CTP_I2C_get_byte_with_ack(0);
		}
		*value =CTP_I2C_get_byte_with_ack(1);
	}
	CTP_I2C_STOP_BIT;
	return 0;
}


// --------------------------------------------------------------------------
/// @Synopsis:  ctp_bl_ts_init 
///
/// @Returns:   
// ----------------------------------------------------------------------------
uint8_t ctp_bl_ts_init(void)
{
	#ifdef  BTL_CHECK_CHIPID
	unsigned char chipID =0x00;
	#endif
	char  ret = 0;	

	CTP_GPIO_INIT;

    bl_ts_set_intup(1);

	MDELAY(25);

	#ifdef  RESET_PIN_WAKEUP
	bl_ts_reset_wakeup();
	#endif

	#ifdef INT_PIN_WAKEUP
	bl_ts_int_wakeup();
	#endif
		
	ctp_i2c_init_sw();

	MDELAY(20);
    
	#ifdef  BTL_CHECK_CHIPID
    ret = bl_get_chip_id(&chipID);	
    if (ret < 0)
    {
        CTP_LOG("I2C communication ERROR!");
		
        return 0;
    }
    else if(chipID != BTL_FLASH_ID)
    {
        CTP_LOG("Please specify the IC model:0x%x!",chipID);
		return 0;
    }  
    else
    {
        CTP_LOG("I2C communication success:chipID = %x!",chipID);
    }
	MDELAY(10);
    #endif

	#ifdef  BL_AUTO_UPDATE_FARMWARE	
	bl_auto_update_fw();
	MDELAY(5);
	#endif

	//bl_get_fw_version(ctp_custom_information.fw_version);

	//eint init
	
	return 1;
}

void ctp_enter_sleep(void)
{
	uint8_t value[2] = {0xA5, 0x03};

	unsigned char data = 0;
	
	if(CTP_FLASH_I2C_WRITE(CTP_SLAVE_ADDR, &value, 1)){
		CTP_LOG("ctp_enter_sleep, write fail");
	}
	if(CTP_FLASH_I2C_READ(CTP_SLAVE_ADDR, &data, 1)){
		CTP_LOG("ctp_enter_sleep, read fail");
	}

	MDELAY(10);

	//bb=HCTP_I2C_WRITE(0xA5, 0x3);
	if(CTP_FLASH_I2C_WRITE(CTP_SLAVE_ADDR, &value, 2)){
		CTP_LOG("ctp_enter_sleep, write fail");
	}

	MDELAY(100);

	CTP_LOG( "ctp_enter_sleep");
}

void ctp_exit_sleep(void)
{

	#ifdef  RESET_PIN_WAKEUP
	bl_ts_reset_wakeup();
	#endif

	#ifdef INT_PIN_WAKEUP
	bl_ts_int_wakeup();
	#endif

    #ifdef NEED_CONFIG_EINT_RESUME

    #endif
	
	CTP_LOG( "ctp_exit_sleep");
}

void ctp_enter_low_power(void)
{

}

void ctp_exit_low_power(void)
{

}


unsigned char  ctp_read_one_point(unsigned int x_base, multi_touch_event_t* event, unsigned int ponitnum)
{
	unsigned char   values[14], i = 0;
	unsigned char    ret = 1;

	if(ponitnum == 1)
	{	
		//HCTP_I2C_READ(x_base, values, 6);
		if(CTP_FLASH_I2C_WRITE(CTP_SLAVE_ADDR, &x_base, 1)){
			CTP_LOG("ctp_read_one_point fail1");
	    }
	    if(CTP_FLASH_I2C_READ(CTP_SLAVE_ADDR, values, 6)){
			CTP_LOG("ctp_read_one_point fail2");
	    }
		
		event->points[0].x = (((unsigned short)(values[2] & 0x0f))<< 8) | values[3];
		event->points[0].y = (((unsigned short)(values[4] & 0x0f))<< 8) | values[5];		
			
		CTP_LOG( "ctp_read_one_point()(x=%d,y=%d)", event->points[0].x, event->points[0].y);
	}
	if(ponitnum == 2)    
	{	
	
		//HCTP_I2C_READ(x_base, values, 14);
		if(CTP_FLASH_I2C_WRITE(CTP_SLAVE_ADDR, &x_base, 1)){
			CTP_LOG("ctp_read_one_point fail1");
	    }
	    if(CTP_FLASH_I2C_READ(CTP_SLAVE_ADDR, values, 14)){
			CTP_LOG("ctp_read_one_point fail2");
	    }
		
		event->points[0].x = (((unsigned short)(values[2] & 0x0f))<< 8) | values[3];
		event->points[0].y = (((unsigned short)(values[4] & 0x0f))<< 8) | values[5];


		event->points[1].x = (((unsigned short)(values[8] & 0x0f))<< 8) | values[9];
		event->points[1].y = (((unsigned short)(values[10] & 0x0f))<< 8) | values[11];

	
	}
	return ret;
}

unsigned char ctp_read_all_point(multi_touch_event_t *tpes, unsigned int points)
{
	unsigned int i = 0;
	CTP_LOG( "ctp_read_all_point points=%d", points);
	
	if((points < 1) || (points > MAX_POINT_NUM))
		return 0;
	ctp_read_one_point(TD_STAT_ADDR, tpes, points);
	return 1;
}

unsigned char ctp_bl_ts_get_data(multi_touch_event_t *tpes)
{
	unsigned char lvalue[2];
	uint8_t reg = TD_STAT_ADDR;
	unsigned int counter = 0, model = 0;

	tpes->time_stamp = ql_rtc_up_time_ms();


	//HCTP_I2C_READ(TD_STAT_ADDR, lvalue, 2); //read out the count of touch points.
	if(CTP_FLASH_I2C_WRITE(CTP_SLAVE_ADDR, &reg, 1)){
		CTP_LOG("ctp_read_one_point fail1");
	}
	    if(CTP_FLASH_I2C_READ(CTP_SLAVE_ADDR, lvalue, 2)){
			CTP_LOG("ctp_read_one_point fail2");
	    }
	
	model = (unsigned int)(lvalue[1] & TD_STAT_NUMBER_TOUCH);
	CTP_LOG( "ctp_bl_ts_get_data model=%d", model);
	tpes->model = (unsigned short)model;  //read out all touch points coordinates.
	
	if((model == 0) || (model > MAX_POINT_NUM))
		return 0;

	ctp_read_all_point(tpes, model);
	return 1;
}



static void ctp_bl_ts_power(unsigned char ON)
{
	if (ON)
		ctp_exit_sleep();
	else
		ctp_enter_sleep();
	return;
}

static void ctp_bl_ts_deinit(void)
{

}


static ctp_info_t  *ctp_bl_get_info(void)
{
	return &ctp_custom_information;
}


ctp_funcs_t ctp_custom_func_bl=
{
	ctp_bl_ts_init,
	ctp_bl_ts_deinit,
	ctp_bl_ts_power,
	ctp_bl_ts_get_data,
	ctp_bl_get_info
};
