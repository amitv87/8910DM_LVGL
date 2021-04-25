#ifndef _QL_CODEC_CONFIG_H_
#define _QL_CODEC_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

//CODEC ALC5616
#define RT5616_I2C_SLAVE_ADDR       0x1B
#define RT5616_VENDOR_ID	        0x10EC
#define RT5616_VENDOR_ID_REG        0XFE

/*****************************************************************************************
* Function: RT5616_INIT_REG
*
* Description: the register below will be write to codec by your code, can be write when power on
* you can add your register below : {register_addr , value,  delay}
******************************************************************************************/
#define RT5616_INIT_REG				\
{									\
	{0x02,	0x0808,	0x00},			\
	{0x03,	0x0808,	0x00},			\
	{0x05,	0x8000, 0x00},			\
	{0x0d,  0x04C0,	0x00},			\
	{0x0F,	0x0808, 0x00},			\
	{0x19,	0xAFAF, 0x00},			\
	{0x1C,	0x3F3F, 0x00},			\
	{0x1E,	0x0000, 0x00},			\
	{0x27,	0x3820, 0x00},			\
	{0x29,	0x8080, 0x00},			\
	{0x2A,	0x1250, 0x00},			\
	{0x3B,	0x0000, 0x00},			\
	{0x3C,	0x006B, 0x00},			\
	{0x3D,	0x2000, 0x00},			\
	{0x3E,	0x006B, 0x00},			\
	{0x45,	0x4000, 0x00},			\
	{0x4D,	0x0000, 0x00},			\
	{0x4E,	0x0000, 0x00},			\
	{0x4F,	0x0278, 0x00},			\
	{0x50,	0x0000, 0x00},			\
	{0x51,	0x0000, 0x00},			\
	{0x52,	0x0278, 0x00},			\
	{0x53,	0xC800, 0x00},			\
	{0x61,	0x9806, 0x00},			\
	{0x62,	0x8800,	0x00},    		\
	{0x63,	0xF8FE,	0x00},			\
	{0x64,  0x6A00, 0x00},    		\
	{0x65,	0xCC00,	0x66},			\
	{0X66,	0x3F00,	0x00},			\
	{0x6A,	0x003D, 0x00},			\
	{0x6C,	0x3600, 0x00},			\
	{0x70,  0x0000, 0x00},			\
	{0x74,	0x0C00, 0x00},			\
	{0x8E,	0x0019, 0x00},			\
	{0x8F,	0x3100, 0x00},			\
	{0x93,	0x2C00, 0x00},			\
	{0x94,	0x0200, 0x00},			\
	{0xB0,	0x2080, 0x00},			\
	{0xB1,	0x0000, 0x00},			\
	{0xFA,	0x0011, 0x00},			\
}

/*****************************************************************************************
* Function: RT5616_NV_INIT_REG
*
* Description: the register below will be write to codec by dsp when start play or record
* you can add your register below : {register_addr , value,  delay}
* Attention: The maximum number can not exceed 100 
******************************************************************************************/
#define RT5616_NV_INIT_REG		\
{								\
}

/*****************************************************************************************
* Function: RT5616_NV_CLOSE_REG
*
* Description: the register below will be write to codec by dsp when stop play or record
* you can add your register below : {register_addr , value,  delay}
* Attention: The maximum number can not exceed 50 
******************************************************************************************/
#define RT5616_NV_CLOSE_REG \
{						    \
}

/*****************************************************************************************
* Function: RT5616_NV_SAMPLE_REG
*
* Description: the register below will be write to codec by dsp when stop play or record
* you can add your register below : {register_addr , value,  delay}
* Attention: The maximum number can not exceed 50 
******************************************************************************************/
#define RT5616_NV_SAMPLE_8K_REG \
{								\
	{0x73,	0x6000,   0x00},	\
	{0x80,	0x4008,	  0x00},  	\
	{0x81,	0x2102,	  0x00},	\
	{0x82,  0x7000,   0x00},  	\
}

#define RT5616_NV_SAMPLE_12K_REG \
{							     \
}

#define RT5616_NV_SAMPLE_16K_REG \
{								 \
	{0x73,	0x4000,   0x00},	 \
	{0x80,	0x4008,	  0x00},  	 \
	{0x81,	0x2102,	  0x00},	 \
	{0x82,  0x7000,   0x00},  	 \
}

#define RT5616_NV_SAMPLE_22_05K_REG \
{									\
}

#define RT5616_NV_SAMPLE_24K_REG   \
{								   \
}

#define RT5616_NV_SAMPLE_32K_REG \
{								 \
	{0x73,	0x2000,   0x00},	 \
	{0x80,	0x4008,	  0x00},  	 \
	{0x81,	0x2102,	  0x00},	 \
	{0x82,  0x7000,   0x00},  	 \
}

#define RT5616_NV_SAMPLE_44_1K_REG \
{								   \
}

#define RT5616_NV_SAMPLE_48K_REG   \
{								   \
}



/**************************************************************************************************
* Function: RT5616_NV_INPUT_REG
*
* Description: the register below will be write to codec by dsp when choose following mic to record
* you can add your register below : {register_addr , value,  delay}
* Attention: The maximum number can not exceed 20 
***************************************************************************************************/
#define RT5616_INPUT_MAIN_MIC_REG  \
{								   \
}

#define RT5616_INPUT_AUX_MIC_REG   \
{								   \
}

#define RT5616_INPUT_DUAL_MIC_REG  \
{								   \
}

#define RT5616_INPUT_HP_MIC_L_REG  \
{								   \
}

#define RT5616_INPUT_HP_MIC_R_REG  \
{								   \
}


/*****************************************************************************************************
* Function: RT5616_NV_OUTPUT_REG
*
* Description: the register below will be write to codec by dsp when use following output type to play
* you can add your register below : {register_addr , value,  delay}
* Attention: The maximum number can not exceed 20 
******************************************************************************************************/
#define RT5616_OUTPUT_RECEIVER_REG  \
{								    \
}

#define RT5616_OUTPUT_HEADPHONE_REG \
{								    \
}

#define RT5616_OUTPUT_SPEAKER_REG   \
{								    \
}

//NOT support now
#define RT5616_OUTPUT_HEADPHONE_SPEAKER_REG  \
{								    		 \
}

#ifdef __cplusplus
} /*"C" */
#endif

#endif

