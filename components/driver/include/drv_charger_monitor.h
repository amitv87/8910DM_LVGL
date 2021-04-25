/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _DRV_CHARGER_MON_H_
#define _DRV_CHARGER_MON_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CAL_TYPR_NO 0
#define CAL_TYPE_NV 1
#define CAL_TYPE_EFUSE 2

#define CHARGER_CURRENT_300MA 300
#define CHARGER_CURRENT_350MA 350
#define CHARGER_CURRENT_400MA 400
#define CHARGER_CURRENT_450MA 450
#define CHARGER_CURRENT_500MA 500
#define CHARGER_CURRENT_550MA 550
#define CHARGER_CURRENT_600MA 600
#define CHARGER_CURRENT_650MA 650
#define CHARGER_CURRENT_700MA 700
#define CHARGER_CURRENT_750MA 750
#define CHARGER_CURRENT_800MA 800
//#define CHARGER_CURRENT_850MA   850
#define CHARGER_CURRENT_900MA 900
//#define CHARGER_CURRENT_950MA   950
#define CHARGER_CURRENT_1000MA 1000
//#define CHARGER_CURRENT_1050MA  1050
#define CHARGER_CURRENT_1100MA 1100
#define CHARGER_CURRENT_1200MA 1200
#define CHARGER_CURRENT_MAX 1300

typedef struct
{
    uint16_t warning_vol;
    uint16_t shutdown_vol;
    uint16_t deadline_vol;
    uint16_t warning_count;
} chgDischarge_t;

typedef enum
{
    CHG_IDLE = 0,
    CHG_STARTING,
    CHG_CHARGING,
    CHG_PULSECHARGING,
    CHG_STOPPING
} chgState_e;

typedef enum
{
    CHG_INVALIDREASON = 0,
    CHG_CHARGERUNPLUG = 1,
    CHG_TIMEOUT,
    CHG_VBATEND,
    CHG_CHARGEDONE,
    CHG_OVERVOLTAGE, //add by paul for charge:over voltage
    CHG_OVERTEMP,
    CHG_PESUDO_CHARGERUNPLUG
} chgStopReason_e;

typedef enum
{
    CHG_VBAT_MONITOR_MSG = 1,
    CHG_CHARGER_MONITOR_MSG,
    CHG_CHARGER_PLUG_IN_MSG,
    CHG_CHARGER_PLUG_OUT_MSG,
    CHG_MODULE_RESET_MSG,
    CHG_CHARGER_PESUDO_PLUG_OUT_MSG,
#ifdef BATTERY_DETECT_SUPPORT
    CHG_BATTERY_OFF_MSG,
#endif
    CHG_IDLE_MSG,
    CHG_MAX_MSG
} chgMsg_t;

//
//the param of the callback.
//   typedef void (* REG_CALLBACK) (uint32 id, uint32 argc, void *argv);
//   id : the id that provided when register.
//   argc : the event; one of CHR_SVR_MSG_SERVICE_E;
//   argv : capacity. you can convert to uint32,
// The message notify to MMI
typedef enum
{
    // Charge message.
    CHR_CAP_IND = 0x1, // Notify the battery's capacity

    CHR_CHARGE_START_IND, // start the charge process.
    CHR_CHARGE_END_IND,   // the charge ended.

    CHR_WARNING_IND,  // the capacity is low, should charge.
    CHR_SHUTDOWN_IND, // the capacity is very low and must shutdown.
#ifdef BATTERY_DETECT_SUPPORT
    CHR_BATTERY_OFF_IND, // the battery is off
#endif

    CHR_CHARGE_FINISH,     // the charge has been completed.
    CHR_CHARGE_DISCONNECT, // the charge be disconnect
    CHR_CHARGE_FAULT,      // the charge fault, maybe the voltage of charge is too low.

    CHR_MSG_MAX_NUM
} CHR_SVR_MSG_SERVICE_E;

/*This enum defines the lowest switchover point between constant-current and
constant-volatage.*/
typedef enum CCtoCVSwitchoverPoint
{
    CHG_SWITPOINT_LOWEST = 0x0F,
    CHG_SWITPOINT_1 = 0x0E,
    CHG_SWITPOINT_2 = 0x0D,
    CHG_SWITPOINT_3 = 0x0C,
    CHG_SWITPOINT_4 = 0x0B,
    CHG_SWITPOINT_5 = 0x0A,
    CHG_SWITPOINT_6 = 0x09,
    CHG_SWITPOINT_7 = 0x08,
    CHG_SWITPOINT_8 = 0x07,
    CHG_SWITPOINT_9 = 0x06,
    CHG_SWITPOINT_10 = 0x05,
    CHG_SWITPOINT_11 = 0x04,
    CHG_SWITPOINT_12 = 0x03,
    CHG_SWITPOINT_13 = 0x02,
    CHG_SWITPOINT_14 = 0x01,
    CHG_SWITPOINT_15 = 0x00,
    CHG_SWITPOINT_16 = 0x10,
    CHG_SWITPOINT_17 = 0x11,
    CHG_SWITPOINT_18 = 0x12,
    CHG_SWITPOINT_19 = 0x13,
    CHG_SWITPOINT_20 = 0x14,
    CHG_SWITPOINT_21 = 0x15,
    CHG_SWITPOINT_22 = 0x16,
    CHG_SWITPOINT_23 = 0x17,
    CHG_SWITPOINT_24 = 0x18,
    CHG_SWITPOINT_25 = 0x19,
    CHG_SWITPOINT_26 = 0x1A,
    CHG_SWITPOINT_27 = 0x1B,
    CHG_SWITPOINT_28 = 0x1C,
    CHG_SWITPOINT_29 = 0x1D,
    CHG_SWITPOINT_30 = 0x1E,
    CHG_SWITPOINT_HIGHEST = 0x1F
} chgSwitPoiint_e;

typedef enum
{
    CHGMNG_ADP_UNKNOW,
    CHGMNG_ADP_STANDARD,
    CHGMNG_ADP_NONSTANDARD,
    CHGMNG_ADP_USB,
} chgAdapterType_e;

typedef struct
{
    uint16_t warning_vol;
    uint16_t shutdown_vol;
    uint16_t deadline_vol;
    uint16_t warning_count;
} chgDisChargeParam_t;

typedef struct
{
    uint16_t rechg_vol;
    uint16_t chg_end_vol;
    uint16_t bat_safety_vol;
    uint16_t standard_chg_current;
    uint16_t usb_chg_current;
    uint16_t nonstandard_chg_current;
    uint16_t chg_timeout;
} chgChargeParam_t;

typedef struct
{
    uint16_t ovp_type;
    uint16_t ovp_over_vol;
    uint16_t ovp_resume_vol;
} chgOVPparam_t;

typedef struct
{
    uint16_t otp_type; ///0:don't support,1:environment temp,2:vbat temp
    uint16_t over_low;
    uint16_t over_high;
    uint16_t resume_low;
    uint16_t resume_high;
} chgOtpParam_t;

typedef enum
{
    CHR_BATT_MIN_VOLTAGE,
    CHR_BATT_MAX_VOLTAGE,
    CHR_BATT_IN_CHARGE,
    CHR_BATT_OUT_OF_CHARGE
} chgBattState_e;

typedef enum
{
    CHG_DEFAULT_MODE = 0,
    CHG_NORMAL_ADAPTER,
    CHG_USB_ADAPTER
} chgAdapterMode_e;

#define CHR_BATTERY_NONE_S 0x1     // No battery
#define CHR_BATTERY_NORMAL_S 0x2   // battery is in normal model( not in charge )
#define CHR_BATTERY_CHARGING_S 0x3 // is charging.

#define BAT_TABLE_SIZE (13)
#define PROD_HW_CONFIG (20)
#define CHR_PARAM_RESERVE_SIZE (48 - (PROD_HW_CONFIG + 2 * BAT_TABLE_SIZE))

// All the voltage unit is mV
typedef struct
{
    struct dischg_param_t
    {
        uint16_t voltage_warning;  // when low than the voltage, send the warning msg to client.
        uint16_t voltage_shutdown; // when low than the voltage, send the shutdown msg to client.
        uint16_t voltage_deadline; //adjust_backlight;    //
        uint16_t warning_count;    // when reach the warning count, will send the voltage warning to client.
    } dischg_param;
    struct chg_param_t
    {
        uint16_t recharge_voltage;
        uint16_t charge_end_voltage;
        uint16_t bat_safety_vol;
        uint16_t standard_current_type;    //standard adapter charge current type
        uint16_t usb_current_type;         //pc charge current type
        uint16_t nonstandard_current_type; //nonstandard adapter charge current select,0:usb 1:standard adapter
        uint16_t chg_timeout;
    } chg_param;
    struct ovp_param_t
    {
        uint16_t ovp_type; //ovp type
        uint16_t ovp_over_vol;
        uint16_t ovp_resume_vol;
    } ovp_param;
    struct
    {
        uint16_t otp_type; ///0:don't support,1:environment temp,2:vbat temp
        uint16_t over_low;
        uint16_t over_high;
        uint16_t resume_low;
        uint16_t resume_high;
    } otp_param;
    uint16_t hw_switch_point; //set the lowest hardware switchover point between cc-cv
    // The table define as following:
    //  If the voltage >= the low WORD of table[n], the capacity is the high WORD of table[n].
    // The last one item of the table must be 0xFFFFFFFF
    uint16_t dischg_bat_capacity_table[BAT_TABLE_SIZE];
    uint16_t chg_bat_capacity_table[BAT_TABLE_SIZE];
    uint16_t reseved[CHR_PARAM_RESERVE_SIZE];
} chgParamProd_t; // Size is 112 bytes

#ifdef __cplusplus
}
#endif
#endif
