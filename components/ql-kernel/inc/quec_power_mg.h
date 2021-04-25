#ifndef QUEC_POWER_MG_H_
#define QUEC_POWER_MG_H_

#include "quec_common.h"
#include "drv_names.h"
#include "osi_api.h"
#include "ql_gpio.h"
#include "quec_pin_index.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#if 0   //current not use task
#define QUEC_SLEEP_TASK_PRIO   		20
#define QUEC_SLEEP_TASK_EVENT_MAX   5
#define QUEC_SLEEP_TASK_STACK       1024*2
#define QUEC_SLEEP_EVENT_ID_SLEEP   15
#define QUEC_SLEEP_EVENT_ID_TIMER   16
#endif

#define TRUE 1
#define FALSE 0

#define QUEC_PRINT_SLEEP_STATE_PERIOD  3000
#define QUEC_DATA_LOCK_TIME			   5000  //at+qsclk=2时，串口来数据5秒内不可休眠

#define QUEC_SLEEP_SOURCE_MAIN_TAG  		DRV_NAME_KERNEL_SLEEP_MAIN
#define QUEC_SLEEP_WAKEUP_IN_TAG    		DRV_NAME_KERNEL_SLEEP_WAKEUP_IN

#define QUEC_MAIN_DTR_PIN  	   		QUEC_GPIO_MAIN_DTR	         
#define QUEC_SLEEP_LED_PIN     		QUEC_GPIO_SLEEP_IND
#ifndef __QUEC_OEM_VER_LD__
#define QUEC_WAKE_UP_IN_PIN         QUEC_GPIO_WAKEUP_IN
#else
#define QUEC_WAKE_UP_IN_PIN         QUEC_GPIO_DNAME_GPIO_5
#endif

#define QUEC_HAL_RESUME_SRC_PMIC 			(1 << 0)
#define QUEC_HAL_RESUME_SRC_VAD 			(1 << 1)
#define QUEC_HAL_RESUME_SRC_KEY 			(1 << 2)
#define QUEC_HAL_RESUME_SRC_GPIO1 			(1 << 3)
#define QUEC_HAL_RESUME_SRC_UART1 			(1 << 4)
#define QUEC_HAL_RESUME_SRC_UART1_RXD 		(1 << 5)
#define QUEC_HAL_RESUME_SRC_WCN2SYS 		(1 << 6)
#define QUEC_HAL_RESUME_SRC_WCN_OSC			(1 << 7)
#define QUEC_HAL_RESUME_SRC_IDLE_TIMER1		(1 << 8)
#define QUEC_HAL_RESUME_SRC_IDLE_TIMER2 	(1 << 9)
#define QUEC_HAL_RESUME_SRC_SELF		    (1 << 10)
#define QUEC_HAL_RESUME_SRC_USB_MON 		(1 << 11)
#define QUEC_HAL_RESUME_SRC_UART1_DATA 		(QUEC_HAL_RESUME_SRC_UART1_RXD | QUEC_HAL_RESUME_SRC_UART1)

#define KEYOUT5_FUN_NUM_UART_2_CTS     3    //unisoc func is RTS
#define KEYOUT5_FUN_NUM_UART_3_TXD     4

#define PMSOURCE_NAME_MAX_LEN 32

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef bool
typedef char bool;
#endif

typedef struct
{	
	uint tax;
	char *name;
}drv_index_t;

typedef struct
{
	bool active;
	bool reserve;    //reserve for further using
}pm_state_t;

typedef struct
{
	uint 			tag;
	char            name[PMSOURCE_NAME_MAX_LEN];
	osiPmSource_t 	*pmsource;
	pm_state_t      *pmstate;
}pm_info_t;

typedef enum 
{
	QUEC_NOT_ALLOW_SLEEP = 0,
	QUEC_ALLOW_SLEEP,
}QUEC_SLEEP_FLAG_E;

QuecOSStatus quec_sleep_init(void);

QuecOSStatus quec_sleep_handler(uint sleep_value);

QuecOSStatus check_sleep_condition(void);

void quec_set_sleep_led(uint level);

int pmsource_list_init(void);

int quec_pmsource_create(char *source_name, int tag, const osiPmSourceOps_t * ops, void * ctx);

int quec_pmsource_delete(int tag);

int quec_pmsource_lock(int tag);

int quec_pmsource_unlock(int tag);

int quec_pmsource_auto_sleep(QUEC_SLEEP_FLAG_E enable);

int pmsoucre_get_list(int tag, pm_info_t *pm_info, int *position);

void auto_sleep_timer_callback(void *ctx);

void quec_enter_sleep_cb(void);

void quec_exit_sleep_cb(uint source);


#ifdef CONFIG_QUEC_PROJECT_FEATURE_PSM

typedef enum
{
	QL_PSMDATA_OWNER_DATACALL,

	//==>Warning: Please add new psm data owner upper this line!!
}ql_psm_data_owner_e;


typedef struct
{
    uint8_t owner;
    uint16_t size;
} ql_psm_data_header_t;

typedef struct
{
    uint8_t *buf;      // PSM data buffer
    unsigned buf_size; // PSM data buffer size
    unsigned buf_pos;  // valid PSM data position
}ql_psm_context_t;


typedef void (*ql_psm_enter_callback)(void *ctx);

bool quec_register_psm_enter_cb(ql_psm_enter_callback cb, void *ctx);

bool quec_psm_data_save(ql_psm_data_owner_e owner, const void *buf, uint32_t size);

int quec_psm_data_restore(ql_psm_data_owner_e owner, void *buf, uint32_t size);

void quec_psm_enter_handler(void);

void quec_psm_init(void);

#endif


#endif
