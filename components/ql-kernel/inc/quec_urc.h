/**  @file
  quec_urc.h

  @brief
  This file is used to define version information for different Quectel Project.

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

=================================================================*/

#ifndef _QUEC_URC_H_
#define _QUEC_URC_H_

#include "quec_common.h"
#include "at_engine.h"
#include "quec_linklist.h"
#include "quec_rtos.h"
#include "ql_api_osi.h"
#include "quec_pin_index.h"
#include "ql_uart_internal.h"


#ifndef uint8
#define uint8 unsigned char 
#endif

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#define RI_PROCESS_PRIO					 10
#define URC_PROCESS_PRIO				 (RI_PROCESS_PRIO-1)
#define RI_OUT_LEVEL_HIGH                1
#define RI_OUT_LEVEL_LOW				 0
#define QUEC_EVENT_RI_HANDLE      		 10
#define QUEC_EVENT_URC_HANDLE	 		 11
#define QUEC_EVENT_URC_TIMER_HANDLE	  	 12
#define QUEC_TIMER_STATE_RI   			 0
#define QUEC_TIMER_STATE_URC			 1
#define URC_MAX_ITEM          			 40
#define URC_STACK_SIZE        			 1024*4
#define RI_EVENT_MAX		  			 (URC_MAX_ITEM+10)  //RI timer use this task, too
#define URC_PUSH_EVENT_MAX	  			 URC_MAX_ITEM

#define URC_CMD_RC_OK 0         ///< "OK"
#define URC_CMD_RC_CONNECT 1    ///< "CONNECT"
#define URC_CMD_RC_RING 2       ///< "RING/CRING"
#define URC_CMD_RC_NOCARRIER 3  ///< "NO CARRIER"
#define URC_CMD_RC_ERROR 4      ///< "ERROR"
#define URC_CMD_RC_NOTSUPPORT 5 ///< "NOT SUPPORT"
#define URC_CMD_RC_NODIALTONE 6 ///< "NO DIALTONE"
#define URC_CMD_RC_BUSY 7       ///< "BUSY"
#define URC_CMD_RC_NOANSWER 8   ///< "NO ANSWER"
#define URC_CMD_RC_INVCMDLINE 9 ///< "INVALID COMMAND LINE"
#define URC_CMD_RC_CR 10        ///< "\r\n"
#define URC_CMD_RC_SIMDROP 11   ///< "Sim drop"

#define URC_DEV_COMMON_ALL	DEV_NONE

#define RI_OUTTYPE_STR_DEFAULT_VALUE			\
		{										\
			[RI_OUT_TYPE_OFF] 		= "off",	\
			[RI_OUT_TYPE_PULSE] 	= "pulse",	\
			[RI_OUT_TYPE_ALWAYS] 	= "always",	\
			[RI_OUT_TYPE_AUTO] 		= "auto",	\
			[RI_OUT_TYPE_WAVE] 		= "wave"	\
		}

#define URC_RESULT_TEXT_DEFAULT					\
{												\
	{URC_CMD_RC_CONNECT,	"CONNECT"},			\
	{URC_CMD_RC_RING,	    "RING"},		\
	{URC_CMD_RC_NOCARRIER,	"NO CARRIER"},		\
	{CMD_RC_NODIALTONE,     "NO DIALTONE"},		\
	{0,	NULL}									\
}//add your result code above

typedef atDispatch_t   quec_dispatch_t;
typedef atDevice_t     quec_at_device_t;
typedef atCmdEngine_t  quec_engine_t;


typedef enum
{
	URC_CFG_OPT_ENABLE,
	URC_CFG_OPT_PORT,
	URC_CFG_OPT_DELAY,
	URC_CFG_OPT_CACHE,
	URC_CFG_OPT_RI_RING,
	URC_CFG_OPT_RI_SMS,
	URC_CFG_OPT_RI_OTHERS,
	URC_CFG_OPT_RI_SIG_TYPE,
	URC_CFG_OPT_MAX
} urc_cfg_opt_e;

typedef enum
{
	RI_CATEG_RING,
	RI_CATEG_SMS,
	RI_CATEG_OTHERS,
	RI_CATEG_MAX
} ri_category_e;


typedef enum
{
	RI_OUT_TYPE_NONE = -1,
	RI_OUT_TYPE_OFF,
	RI_OUT_TYPE_PULSE,
	RI_OUT_TYPE_ALWAYS,
	RI_OUT_TYPE_AUTO,
	RI_OUT_TYPE_WAVE,
	RI_OUT_TYPE_MAX
} ri_out_type_e;

typedef enum
{
	URC_OUT_METHOD_UNICAST,
	URC_OUT_METHOD_BROADCAST
} urc_out_method_e;

typedef enum
{
	COMMON_SINGLE_CMUX_SINGLE,
	COMMON_SINGLE_CMUX_ALL,
	COMMON_ALL_CMUX_SINGLE,
	COMMON_ALL_CMUX_ALL
}urc_out_port_type_e;

typedef enum
{
	RI_SIGNAL_TYPE_RESPECTIVE,
	RI_SIGNAL_TYPE_PHYSICAL,
	RI_SIGNAL_TYPE_MAX
} ri_signal_type_e;

typedef enum 
{ 
	QUEC_CMUX_URC_ALL=0,
    QUEC_CMUX_URC_CHANNEL1 =1,
    QUEC_CMUX_URC_CHANNEL2,
    QUEC_CMUX_URC_CHANNEL3,
    QUEC_CMUX_URC_CHANNEL4,
    QUEC_CMUX_URC_MAX,      
}quec_cmux_urc_config_e;

typedef struct
{
	size_t len;
	uint8 * data;
} formated_pkt_t;

typedef struct
{
	quec_dev_e 			urc_dev;         //for urc output
	quec_dev_e          ri_pulse_dev;    //only for ri pulse, when signal type is physical, ri pulse will generate in uart1, but urc send to urc_dev  
	quec_dispatch_t     *dispatch;
	ri_category_e       ri_cate_type;    //ring , sms, or others
	uint8 				*data;
	uint				len;
	uint8               pulse_cnt;       //ri pulse count ,0~5
	ri_out_type_e		ri_out_type;     //pulse, wave, always...
	uint				ri_time;         //ri pulse duration
	uint8				urc_delay_time;  //urc delay duration
}urc_info_t;

typedef struct
{
	ri_out_type_e 	out_type;					// RI pulse output type
	uint 			duration;					// RI pulse duration
	uint			counter;					// RI pulse counter
} ri_pulse_t;

typedef struct
{
	ri_pulse_t			pulse[RI_CATEG_MAX];	// RI pulse config for each category
	ri_signal_type_e	signal_type;			// RI singnal type
	uint				ri_gpio_pin;			// RI gpio pin
} ri_config_t;


typedef struct
{
	bool					enable;				// turn on/off URC output
	quec_dev_e				atpIndex;			// URC output port
	uint					delay_time;			// when to put URC out
	bool					cache;				// XXX:
	ri_config_t				ri;					// RI config
} urc_config_t, * urc_config_ptr_t;				// need to config by AT command

typedef linklist_handler_t	urc_cache_t;

typedef struct
{
	urc_config_t			config;				// URC config
	urc_cache_t 			cache;				// URC buffer
	bool					is_urc_initialized;
	ql_mutex_t          	urc_mutex;
	ql_mutex_t				ri_mutex;
} urc_context_t, * urc_context_handler_t;

typedef struct
{
	bool					urc_delay_fi;
	quec_timer_t			*timer;
    uint8                   pusle_change_cnt; // to count the pulse number
    bool  					timer_state;  	  //timer function, used to finish urc delay or ri pulse
} ri_out_t;


typedef struct
{
	uint8 result_code;
	char *result_text;
}urc_result_text_t;


#define URC_RI_PIN 		QUEC_GPIO_MAIN_RI

#define URC_CONFIG_ATPINDEX_DEFAULT_VALUE                   DEV_USB_AT
#ifndef CONFIG_QL_OPEN_EXPORT_PKG //same as __QUEC_VER_EXPORT_OPEN_CPU__
#define URC_CONFIG_ENABLE_DEFAULT_VALUE						QUEC_ENABLE
#else
#define URC_CONFIG_ENABLE_DEFAULT_VALUE						QUEC_DISABLE
#endif
#define URC_CONFIG_ENABLE_MAX_VALUE							QUEC_ENABLE
#define URC_CONFIG_ENABLE_MIN_VALUE							QUEC_DISABLE

#define URC_CONFIG_DELAY_TIME_DEFAULT_VALUE					0
#define URC_CONFIG_DELAY_TIME_MAX_VALUE						120
#define URC_CONFIG_DELAY_TIME_MIN_VALUE						0


#define	URC_CONFIG_CACHE_DEFAULT_VALUE						FALSE
#define	URC_CONFIG_CACHE_MAX_VALUE							TRUE
#define	URC_CONFIG_CACHE_MIN_VALUE							FALSE


#define	URC_CONFIG_RI_PULSE_RING_OUTTYPE_DEFAULT_VALUE		RI_OUT_TYPE_PULSE
#define	URC_CONFIG_RI_PULSE_RING_OUTTYPE_MAX_VALUE			RI_OUT_TYPE_PULSE
#define	URC_CONFIG_RI_PULSE_RING_OUTTYPE_MIN_VALUE			RI_OUT_TYPE_OFF


#define	URC_CONFIG_RI_PULSE_RING_DURATION_DEFAULT_VALUE		120
#define	URC_CONFIG_RI_PULSE_RING_DURATION_MAX_VALUE			2000

#define	URC_CONFIG_RI_PULSE_RING_DURATION_MIN_VALUE			1

#define	URC_CONFIG_RI_PULSE_RING_COUNTER_DEFAULT_VALUE		1
#define	URC_CONFIG_RI_PULSE_RING_COUNTER_MAX_VALUE			5
#define	URC_CONFIG_RI_PULSE_RING_COUNTER_MIN_VALUE			1


#define	URC_CONFIG_RI_PULSE_SMS_OUTTYPE_DEFAULT_VALUE		RI_OUT_TYPE_PULSE
#define	URC_CONFIG_RI_PULSE_SMS_OUTTYPE_MAX_VALUE			RI_OUT_TYPE_PULSE
#define	URC_CONFIG_RI_PULSE_SMS_OUTTYPE_MIN_VALUE			RI_OUT_TYPE_OFF


#define	URC_CONFIG_RI_PULSE_SMS_DURATION_DEFAULT_VALUE		120
#define	URC_CONFIG_RI_PULSE_SMS_DURATION_MAX_VALUE			2000

#define	URC_CONFIG_RI_PULSE_SMS_DURATION_MIN_VALUE			1


#define	URC_CONFIG_RI_PULSE_SMS_COUNTER_DEFAULT_VALUE		1
#define	URC_CONFIG_RI_PULSE_SMS_COUNTER_MAX_VALUE			5
#define	URC_CONFIG_RI_PULSE_SMS_COUNTER_MIN_VALUE			1


#define	URC_CONFIG_RI_PULSE_OTHERS_OUTTYPE_DEFAULT_VALUE	RI_OUT_TYPE_PULSE
#define	URC_CONFIG_RI_PULSE_OTHERS_OUTTYPE_MAX_VALUE		RI_OUT_TYPE_PULSE
#define	URC_CONFIG_RI_PULSE_OTHERS_OUTTYPE_MIN_VALUE		RI_OUT_TYPE_OFF


#define	URC_CONFIG_RI_PULSE_OTHERS_DURATION_DEFAULT_VALUE	120
#define	URC_CONFIG_RI_PULSE_OTHERS_DURATION_MAX_VALUE		2000

#define	URC_CONFIG_RI_PULSE_OTHERS_DURATION_MIN_VALUE		1
#define	URC_CONFIG_RI_PULSE_OTHERS_COUNTER_DEFAULT_VALUE	1
#define	URC_CONFIG_RI_PULSE_OTHERS_COUNTER_MAX_VALUE		5
#define	URC_CONFIG_RI_PULSE_OTHERS_COUNTER_MIN_VALUE		1


#define URC_CONFIG_RI_SIGNAL_TYPE_DEFAULT_VALUE				RI_SIGNAL_TYPE_RESPECTIVE
#define URC_CONFIG_RI_SIGNAL_TYPE_MAX_VALUE					(RI_SIGNAL_TYPE_MAX - 1)
#define URC_CONFIG_RI_SIGNAL_TYPE_MIN_VALUE					RI_SIGNAL_TYPE_RESPECTIVE


#define URC_CONFIG_RI_GPIO_PIN_DEFAULT_VALUE				19
#define URC_CONFIG_RI_GPIO_PIN_MAX_VALUE					100
#define URC_CONFIG_RI_GPIO_PIN_MIN_VALUE					0

#define URC_CONTEXT_DEFAULT_VALUE																	\
		{																							\
			.config = 																				\
			{																						\
				.enable = URC_CONFIG_ENABLE_DEFAULT_VALUE,											\
				.atpIndex = URC_CONFIG_ATPINDEX_DEFAULT_VALUE,										\
				.delay_time = URC_CONFIG_DELAY_TIME_DEFAULT_VALUE,									\
				.cache = URC_CONFIG_CACHE_DEFAULT_VALUE,											\
				.ri = 																				\
				{																					\
					.pulse = 																		\
					{																				\
						[RI_CATEG_RING] 	= {URC_CONFIG_RI_PULSE_RING_OUTTYPE_DEFAULT_VALUE, 		\
											   URC_CONFIG_RI_PULSE_RING_DURATION_DEFAULT_VALUE, 	\
											   URC_CONFIG_RI_PULSE_RING_COUNTER_DEFAULT_VALUE},		\
						[RI_CATEG_SMS]  	= {URC_CONFIG_RI_PULSE_SMS_OUTTYPE_DEFAULT_VALUE, 		\
											   URC_CONFIG_RI_PULSE_SMS_DURATION_DEFAULT_VALUE, 		\
											   URC_CONFIG_RI_PULSE_SMS_COUNTER_DEFAULT_VALUE},		\
						[RI_CATEG_OTHERS] 	= {URC_CONFIG_RI_PULSE_OTHERS_OUTTYPE_DEFAULT_VALUE, 	\
											   URC_CONFIG_RI_PULSE_OTHERS_DURATION_DEFAULT_VALUE, 	\
											   URC_CONFIG_RI_PULSE_OTHERS_COUNTER_DEFAULT_VALUE}	\
					},																				\
					.signal_type = URC_CONFIG_RI_SIGNAL_TYPE_DEFAULT_VALUE,							\
					.ri_gpio_pin = URC_CONFIG_RI_GPIO_PIN_DEFAULT_VALUE								\
				}																					\
			},																						\
			.cache = NULL,																			\
			.is_urc_initialized = FALSE,															\
			.ri_mutex = NULL,																		\
			.urc_mutex = NULL																		\
		}


QuecOSStatus quec_urc_send(quec_dispatch_t *dispatch, char *data, uint len, urc_out_method_e opt_mthod, ri_category_e urc_cate, bool is_send_firstly);
QuecOSStatus quec_urc_flush(void);
QuecOSStatus quec_urc_init(void);
QuecOSStatus urc_config_save_nv(void);
QuecOSStatus urc_config_read_nv(void);
QuecOSStatus at_urc_config_get_opt(urc_cfg_opt_e urc_cfg_opt, void * option);
QuecOSStatus at_urc_config_set_opt(urc_cfg_opt_e urc_cfg_opt, void * option);
QuecOSStatus quec_urc_result_text(quec_dispatch_t *dispatch, unsigned char result_code, urc_out_method_e opt_mthod, ri_category_e urc_cate, bool is_send_firstly);



ri_out_type_e at_urc_ri_get_outtype_enum(char * _ri_outtype_str);

bool quec_urc_get_ri_tpye();

void urc_ri_process_handler(void *ctx);
void urc_process_handler(void *ctx);
void quec_urc_rdy(char *str);

char * at_urc_ri_get_outtype_str(ri_out_type_e ri_out_type);
QuecOSStatus quec_at_urc_ri_action(quec_dispatch_t *dispatch);
QuecOSStatus quec_cmux_set_urc_port(quec_cmux_urc_config_e urc_port);
quec_cmux_urc_config_e quec_cmux_get_urc_port(void);
quec_dispatch_t *quec_get_urc_dispatch(quec_dev_e urc_dev);



#endif

