/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_POWER_H
#define QL_POWER_H


#include "ql_api_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_POWER_ERRCODE_BASE (QL_COMPONENT_PM<<16)

#define QL_QSCLKEX_NO_DATA_MIN	       1    //unit: second
#define QL_QSCLKEX_NO_DATA_DEFAULT	   3
#define QL_QSCLKEX_NO_DATA_MAX	       50

#define QL_QSCLKEX_PUNISH_TIME_MIN		1   //unit: minute
#define QL_QSCLKEX_PUNISH_TIME_DEFAULT	30
#define QL_QSCLKEX_PUNISH_TIME_MAX		600   

/*===========================================================================
 * Enum
 ===========================================================================*/
/**
 * POWER errcode
 */
typedef enum
{
    QL_POWER_POWD_SUCCESS  = QL_SUCCESS,
    QL_POWER_RESET_SUCCESS = QL_POWER_POWD_SUCCESS,

    QL_POWER_CFW_CTRL_ERR                   =  1|QL_POWER_ERRCODE_BASE,
    QL_POWER_CFW_CTRL_RSP_ERR,
    QL_POWER_CFW_RESET_BUSY,

    QL_POWER_SEMAPHORE_CREATE_ERR           =  5|QL_POWER_ERRCODE_BASE,
    QL_POWER_SEMAPHORE_TIMEOUT_ERR,

    QL_POWER_POWD_EXECUTE_ERR               = 11|QL_POWER_ERRCODE_BASE,
    QL_POWER_POWD_INVALID_PARAM_ERR,

    QL_POWER_RESET_EXECUTE_ERR              = 21|QL_POWER_ERRCODE_BASE,
    QL_POWER_RESET_INVALID_PARAM_ERR
}ql_errcode_power;

typedef enum
{
	QL_SLEEP_SUCCESS      					= QL_SUCCESS,
	QL_SLEEP_INVALID_PARAM					= (QL_COMPONENT_PM_SLEEP << 16) | 1000,   /*  invalid input param  	   		 */
	QL_SLEEP_LOCK_CREATE_FAIL				= (QL_COMPONENT_PM_SLEEP << 16) | 1001,   /*  failed to create wake lock  		 */
	QL_SLEEP_LOCK_DELETE_FAIL				= (QL_COMPONENT_PM_SLEEP << 16) | 1002,   /*  failed to delete wake lock		 */
	QL_SLEEP_LOCK_LOCK_FAIL					= (QL_COMPONENT_PM_SLEEP << 16) | 1003,   /*  failed to lock the wake lock 	 */
	QL_SLEEP_LOCK_UNLOCK_FAIL               = (QL_COMPONENT_PM_SLEEP << 16) | 1004,   /*  failed to unlock the wake lock 	 */
	QL_SLEEP_LOCK_AUTOSLEEP_FAIL            = (QL_COMPONENT_PM_SLEEP << 16) | 1004,   /*  failed to unlock the wake lock 	 */
	
}ql_errcode_sleep;

#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE

#define QL_CHARGE_ERRCODE_BASE (QL_COMPONENT_BSP_CHARGE<<16)
/**
 * usb charge errcode
 */
typedef enum
{
    QL_CHARGE_SUCCESS  = QL_SUCCESS,

    QL_CHARGE_INVALID_PARAM_ERR                   =  1|QL_CHARGE_ERRCODE_BASE,
    QL_CHARGE_CHARGE_STATUS_ERROR,
    QL_CHARGE_BATTERY_DETECT_ERROR
}ql_errcode_charge;

/**
 * usb charge status
 */
typedef enum
{
    //charge idle
    QL_CHG_IDLE = 0,
    //charging
    QL_CHG_CHARGING
} ql_charge_status_e;

/**
 * battery detect channel
 */
typedef enum
{
	QL_CHARGE_ADC0 = 2,						//  DEMO_ADC0
	QL_CHARGE_ADC1 = 3,						//  DEMO_ADC1
	QL_CHARGE_ADC2 = 4,						//  DEMO_ADC2
}ql_adc_charge_channel_e;

#endif

/**
 * Power down mode select
 */
typedef enum
{
    POWD_IMMDLY,
    POWD_NORMAL
}ql_PowdMode;

/**
 * Reset mode select
 */
typedef enum
{
    RESET_QUICK,
    RESET_NORMAL
}ql_ResetMode;

/**
 * Enable auto sleep
 */
typedef enum 
{
	QL_NOT_ALLOW_SLEEP = 0,
	QL_ALLOW_SLEEP,
}QL_SLEEP_FLAG_E;

typedef struct
{
	uint8_t  enable;
	uint8_t  no_data_time;
	uint16_t punish_time;
	uint16_t punish_remain_time;
}qsclkex_cfg_s;

/*===========================================================================
 * Struct
 ===========================================================================*/

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
 
 /*****************************************************************
* Function: ql_power_down
*
* Description: 关机
* 
* Parameters:
*   powd_mode  [in] 快速关机/普通关机 
*
* Return:
*   0          成功   
*	other 	   错误码
*
*****************************************************************/
ql_errcode_power ql_power_down(ql_PowdMode powd_mode);

 /*****************************************************************
* Function: ql_power_reset
*
* Description: 复位
* 
* Parameters:
*   powd_mode  [in] 快速复位/普通复位 
*
* Return:
*   0          成功   
*	other 	   错误码
*
*****************************************************************/
ql_errcode_power ql_power_reset(ql_ResetMode reset_mode);

/*****************************************************************
* Function: ql_lpm_wakelock_create
*
* Description: 用于创建休眠锁
* 
* Parameters:
*   lock_name  [in] 休眠锁名字 
	name_size  [in] 休眠锁名字长度
*
* Return:
*   >0          休眠锁id   
*	other 	    错误码。
*
*****************************************************************/
int ql_lpm_wakelock_create(char *lock_name, int name_size);

/*****************************************************************
* Function: ql_autosleep_enable
*
* Description: 用于使能普通休眠
* 
* Parameters:
*   sleep_flag  [in] 休眠选择
*
* Return:
*   0           成功  
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_sleep ql_autosleep_enable(QL_SLEEP_FLAG_E sleep_flag);

/*****************************************************************
* Function: ql_autosleepex_enable
*
* Description: 用于使能增强型休眠
* 
* Parameters:
*   sleep_flag  	[in] 休眠选择
*	no_data_time	[in] 无数据交互后释放RRC时间
*	punish_time		[in] 异常注网后不可使用增强休眠时间
*
* Return:
*   0           成功  
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_sleep ql_autosleepex_enable(QL_SLEEP_FLAG_E sleep_flag, uint8_t no_data_time, uint16_t punish_time);

/*****************************************************************
* Function: ql_lpm_wakelock_delete
*
* Description: 用于删除休眠锁
* 
* Parameters:
*   wakelock_fd  [in] 休眠锁id
*
* Return:
*   0           成功  
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_sleep ql_lpm_wakelock_delete(int wakelock_fd);

/*****************************************************************
* Function: ql_lpm_wakelock_lock
*
* Description: 锁定休眠锁，不允许系统进入休眠
* 
* Parameters:
*   wakelock_fd  [in] 休眠锁id
*
* Return:
*   0           成功  
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_sleep ql_lpm_wakelock_lock(int wakelock_fd);

/*****************************************************************
* Function: ql_lpm_wakelock_unlock
*
* Description: 释放休眠锁，所有休眠锁均释放后，系统可进入休眠
* 
* Parameters:
*   wakelock_fd  [in] 休眠锁id
*
* Return:
*   0           成功  
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_sleep ql_lpm_wakelock_unlock(int wakelock_fd);


#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE
/*****************************************************************
* Function: ql_get_vchg_vol
*
* Description: 用于获取USB端电压
* 
* Parameters:
*   vol 	    [out] 	USB端电压 
*
* Return:
*   0           获取电压成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_charge ql_get_vchg_vol(uint32_t* vol);

/*****************************************************************
* Function: ql_get_vbus_state
*
* Description: 用于获取USB在位状态
* 
* Parameters:
*   state 	    [out] 	USB在位状态,0表示未连接USB,1表示已连接USB 
*
* Return:
*   0           获取状态成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_charge ql_get_vbus_state(uint32_t* state);

/*****************************************************************
* Function: ql_get_battery_vol
*
* Description: 用于获取电池端电压
* 
* Parameters:
*   vol 	    [out] 	电池端电压 
*
* Return:
*   0           获取电压成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_charge ql_get_battery_vol(uint32_t* vol);

/*****************************************************************
* Function: ql_get_battery_level
*
* Description: 用于获取电池电量百分比
* 
* Parameters:
*   level 	    [out] 	电池电量百分比 
*
* Return:
*   0           获取电量成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_charge ql_get_battery_level(uint32_t* level);

/*****************************************************************
* Function: ql_get_charge_cur
*
* Description: 用于获取充电电流
* 
* Parameters:
*   cur 	    [out] 	充电电流 
*
* Return:
*   0           获取电流成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_charge ql_get_charge_cur(uint32_t* cur);

/*****************************************************************
* Function: ql_get_charge_status
*
* Description: 用于获取当前充电状态
* 
* Parameters:
*   status 	    [out] 	充电状态,参考ql_charge_status_e枚举 
*
* Return:
*   0           获取状态成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_charge ql_get_charge_status(ql_charge_status_e* status);

/*****************************************************************
* Function: ql_set_battery_detect_channel
*
* Description: 设置电池检测通道
* 
* Parameters:
*   ql_adc_channel_id 	    [in] 	电池检测的ADC通道,取值参考ql_adc_charge_channel_e枚举
*
* Return:
*   0           设置成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_charge ql_set_battery_detect_channel(uint32_t         ql_adc_channel_id);

/*****************************************************************
* Function: ql_battery_detect_enable
*
* Description: 用于使能电池检测,调用该函数前,需先设置电池检测通道
*              如果电池没有连接ADC,打开电池检测,会导致无法充电
* 
* Parameters:
*   on_off 	    [in] 	开关,1表示开启电池检测,0表示关闭电池检测 
*
* Return:
*   0           设置成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_charge ql_battery_detect_enable(uint32_t on_off);

/*****************************************************************
* Function: ql_get_battemp_vol_mv
*
* Description: 用于获取电池温度对应的电压,根据电池电压与温度对应关系,
*              可计算出电池温度。调用该函数前,需先使能电池检测。
*              目前只支持温度检测,不支持充电控制。
*              
* Parameters:
*   temp_vol 	[out] 	电池温度电压 
*
* Return:
*   0           获取电压成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_charge ql_get_battemp_vol_mv(uint32_t* temp_vol);

/*****************************************************************
* Function: ql_get_battery_state
*
* Description: 用于获取电池在位状态。调用该函数前，需先使能电池检测。
*              
* Parameters:
*   state 	    [out] 	电池在位状态，0表示未连接电池，1表示已连接电池 
*
* Return:
*   0           获取状态成功。
*	other 	    错误码。
*
*****************************************************************/
ql_errcode_charge ql_get_battery_state(uint32_t* state );
#endif


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_POWER_H */

