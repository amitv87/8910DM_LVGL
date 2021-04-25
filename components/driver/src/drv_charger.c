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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('C', 'H', 'A', 'G')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_charger.h"
#include "drv_charger_monitor.h"
#include "drv_pmic_intr.h"
#include "drv_adc.h"
#include "hal_adi_bus.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hwregs.h"
#include "drv_config.h"
#include "drv_efuse_pmic.h"

#include <string.h>
#ifdef CONFIG_QUEC_PROJECT_FEATURE
#include "drv_usb.h"
#endif

#define _READ_CV_STATUS_REG_

#define CHARGER_DEBOUNCE (10)
#define CHG_BAT_MONITOR_PERIOD_ (10000)
#define CHG_STOP_VPROG 150 //100//0x70  // Isense stop point,current value

typedef struct
{
    bool plugged;
    drvChargerType_t type;
    drvChargerPlugCB_t cb;
    drvChargerNoticeCB_t notice_cb;
    osiThread_t *chgTaskId;
    osiTimer_t *batMonTimer;
    osiTimer_t *chgMonTimer;
    chgMsg_t adaptor_msg;
    chgMsg_t charge_msg;
    chgMsg_t battery_msg;
    void *plug_cb_ctx;
} drvChargerContext_t;

static drvChargerContext_t gDrvChargeCtx;

#ifdef CONFIG_SUPPORT_BATTERY_CHARGER
static void _drvChargePluginHandler(void);
#endif

static void _drvSetChargerBoardParam(void);
static uint32_t _drvChargerVoltagePercentum(uint32_t voltage, chgState_e is_charging, bool update);

//when the phone is staying in busy state(for example, talking, play games or play music, etc.),we will stop
//the state timer until it is not busy.
#define BUSYSTATE 1

#define CHARGING_ON_TIMER_INTERVAL (9000)
#define CHARGING_OFF_TIMER_INTERVAL (1000)

#define VOL_TO_CUR_PARAM (576)
#define VBAT_VOL_DIV_P1 (1000)
#define VBAT_VOL_DIV_P2 1000

#define DISCHG_VBAT_BUFF_SIZE 32
#define CHG_VBAT_BUFF_SIZE 16
#define CHG_CURRENT_BUFF_SIZE (CHG_VBAT_BUFF_SIZE >> 1)

#define VBAT_RESULT_NUM 7  //ADC sampling number
#define VPROG_RESULT_NUM 7 //ADC sampling number

#define PULSE_POS_LEVEL_WIDTH (9)
#define PULSE_NEG_LEVEL_WIDTH (1)
#define PULSE_PERIOD (PULSE_POS_LEVEL_WIDTH + PULSE_NEG_LEVEL_WIDTH)

#define CHGMNG_CALI_MODE_END_VOLT 4500
#define CHGMNG_CALI_MODE_BAT_OVP_VOLT 4500
#define CHG_NV_TEMP_OFFSET 1000 //NV value must be positive,e.g. nv = -5C + CHG_NV_TEMP_OFFSET

#define OVP_DETECT_VALID_TIMES (3)

#define _CHG_COUNTER_INIT(c, def) (c = def)
#define _CHG_COUNTER_ADD(c) (c++)
#define _CHG_COUNTER_ROUND(c, round) \
    {                                \
        if (c == round)              \
            c = 0;                   \
    }
#define _CHGMNG_COUNTER_ADD_AND_AUTO_ROUND(c, round) \
    {                                                \
        _CHG_COUNTER_ADD(c);                         \
        _CHG_COUNTER_ROUND(c, round);                \
    }

#define _CHG_COUNTER_DEC(c) (c--)

/**---------------------------------------------------------------------------*
 **                         Global Variables                                  *
 **---------------------------------------------------------------------------*/
static uint32_t g_charge_plus = 0;
static bool ischgmng_start = false;                        //charge manager start flag.
static bool isshutting_down = false;                       //shutdowe flag
static chgSwitPoiint_e hw_switch_point = CHG_SWITPOINT_15; //The lowest switchover point between cc and cv modes.
static uint16_t cv_status_counter = 0;
static uint16_t charge_endtime_counter = 0;
static uint16_t warning_counter = 0;
static bool charge_stop = false;
//static uint16_t pulse_counter = 0;
static uint16_t ovp_detect_counter = 0;
static uint16_t ovp_detect_bat_cnt = 0;
/* recent_message_flag: Record the recent message which has been send before client registes.*/
static uint32_t recent_message[10] = {0}; // Ten is enough!
uint8_t gChgTaskReady = 0;
#define BAT_CAPACITY_STEP 12

#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE
bool battery_detect = false;
uint32_t battery_detect_channel = ADC_CHANNEL_MAX;
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE
extern quec_usb_setting_s quec_usb_current_setting;
#endif

static uint16_t dischg_bat_capacity_table[BAT_CAPACITY_STEP][2] =
    {
        {4120, 100},
        {4060, 90},
        {3979, 80},
        {3900, 70},
        {3840, 60},
        {3800, 50},
        {3760, 40},
        {3730, 30},
        {3700, 20},
        {3650, 15},
        {3600, 5},
        {3501, 0},
};

static uint16_t chg_bat_capacity_table[BAT_CAPACITY_STEP][2] =
    {
        {4200, 100},
        {4180, 90},
        {4119, 80},
        {4080, 70},
        {4020, 60},
        {3970, 50},
        {3920, 40},
        {3880, 30},
        {3860, 20},
        {3830, 15},
        {3730, 5},
        {3251, 0},
};

static chgDischarge_t dischg_param =
    {
        3440, //warning_vol
        3380, //shutdown_vol
        3000, //deadline_vol
        24,   //warning_count,warning interval
};

const static int16_t vbat_temp_table[][2] =
    {
        {1451, -40},
        {1435, -39},
        {1419, -38},
        {1402, -37},
        {1384, -36},
        {1367, -35},
        {1348, -34},
        {1330, -33},
        {1311, -32},
        {1292, -31},
        {1272, -30},
        {1253, -29},
        {1233, -28},
        {1212, -27},
        {1192, -26},
        {1171, -25},
        {1150, -24},
        {1129, -23},
        {1108, -22},
        {1087, -21},
        {1066, -20},
        {1045, -19},
        {1023, -18},
        {1002, -17},
        {981, -16},
        {960, -15},
        {938, -14},
        {917, -13},
        {896, -12},
        {875, -11},
        {854, -10},
        {834, -9},
        {814, -8},
        {794, -7},
        {773, -6},
        {754, -5},
        {735, -4},
        {716, -3},
        {697, -2},
        {678, -1},
        {660, 0},
        {642, 1},
        {625, 2},
        {608, 3},
        {591, 4},
        {574, 5},
        {558, 6},
        {542, 7},
        {527, 8},
        {512, 9},
        {497, 10},
        {483, 11},
        {468, 12},
        {456, 13},
        {441, 14},
        {428, 15},
        {416, 16},
        {403, 17},
        {391, 18},
        {379, 19},
        {368, 20},
        {357, 21},
        {346, 22},
        {335, 23},
        {326, 24},
        {316, 25},
        {306, 26},
        {297, 27},
        {288, 28},
        {279, 29},
        {271, 30},
        {262, 31},
        {254, 32},
        {247, 33},
        {239, 34},
        {231, 35},
        {225, 36},
        {218, 37},
        {211, 38},
        {205, 39},
        {199, 40},
        {193, 41},
        {187, 42},
        {181, 43},
        {176, 44},
        {170, 45},
        {165, 46},
        {160, 47},
        {156, 48},
        {151, 49},
        {146, 50},
        {142, 51},
        {138, 52},
        {134, 53},
        {130, 54},
        {126, 55},
        {122, 56},
        {118, 57},
        {115, 58},
        {112, 59},
        {108, 60},
        {105, 61},
        {102, 62},
        {99, 63},
        {97, 64},
        {94, 65},
        {91, 66},
        {89, 67},
        {86, 68},
        {84, 69},
        {81, 70},
        {79, 71},
        {77, 72},
        {75, 73},
        {73, 74},
        {71, 75},
        {69, 76},
        {67, 77},
        {65, 78},
        {63, 79},
        {62, 80},
        {60, 81},
        {58, 82},
        {57, 83},
        {55, 84},
        {54, 85},
        {52, 86},
        {51, 87},
        {50, 88},
        {49, 89},
        {47, 90},
        {46, 91},
        {45, 92},
        {44, 93},
        {43, 94},
        {42, 95},
};

#define HOT_TEMP 1042

typedef struct
{
    uint16_t rechg_vol;
    uint16_t chg_end_vol;
    uint16_t bat_safety_vol;
    uint16_t standard_chg_current;
    uint16_t usb_chg_current;
    uint16_t nonstandard_chg_current;
    uint16_t chg_timeout;
} chgCharge_t;

typedef enum
{                            //this enum group are onle used to serve FSM in charger module
    CHG_FSM_EVENT_INIT = 0,  //init event
    CHG_FSM_EVENT_START_CHG, //charger plug in
    CHG_FSM_EVENT_STOP_CHG,  //charging timeout
    CHG_FSM_EVENT_MAX        //don't use
} chgFsmEvent_e;

static chgCharge_t chg_param =
    {
        4150, //rechg_vol
        4210, //chg_end_vol
        4330,
        CHARGER_CURRENT_700MA, //standard_chg_current
        CHARGER_CURRENT_500MA, //usb_chg_current
        CHARGER_CURRENT_500MA, //nonstandard_current_sel,0:usb charge current,1:normal charge current
        18000,                 //18000S
};

typedef struct
{
    uint16_t ovp_type;
    uint16_t ovp_over_vol;
    uint16_t ovp_resume_vol;
} chgOvpParam_t;

static chgOvpParam_t ovp_param =
    {
        0,    //ovp_type
        6500, //ovp_over_vol
        5800, //ovp_resume_vol
};

typedef struct
{
    chgState_e chgmng_state;       //charge module state
    uint32_t bat_statistic_vol;    //statistic voltage,
    uint32_t bat_cur_vol;          //current voltage, twinkling value
    uint32_t bat_remain_cap;       //remain battery capacity
    uint32_t charging_current;     //charging current value,reserved
    uint32_t charging_temperature; ///statistic vbat temperature.
    chgAdapterType_e adp_type;     //adapter type when chargeing,reserved
    chgStopReason_e charging_stop_reason;
    uint32_t chg_vol;
} chgStateInfo_t;

static chgStateInfo_t module_state =
    {
        CHG_IDLE,          //chgmng_state
        0,                 //bat_statistic_vol
        0,                 //bat_cur_vol
        0xff,              //bat_remain_cap
        0,                 //charging_current
        0,                 //charging_temperature
        CHGMNG_ADP_UNKNOW, //adp_type
        CHG_INVALIDREASON, //charging_stop_reason
        0,                 //chg_vol
};

static chgStateInfo_t module_state_to_app;

typedef struct
{
    uint32_t queue[DISCHG_VBAT_BUFF_SIZE];
    uint32_t pointer;
    uint32_t sum;
    uint32_t queue_len;
} vbatQueueInfo_t;
static vbatQueueInfo_t vbat_queue;

typedef struct
{
    uint32_t queue[CHG_CURRENT_BUFF_SIZE];
    uint32_t pointer;
    uint32_t sum;
} currentQueueInfo_t;
static currentQueueInfo_t current_queue;

static bool _drvGetUsbStatus(void)
{
    return false; // TODO
}

static void _updateChargerStatus(drvChargerContext_t *p)
{
    bool level = drvPmicEicGetLevel(DRV_PMIC_EIC_CHGR_INT);
    REG_RDA2720M_GLOBAL_CHGR_STATUS_T status = {halAdiBusRead(&hwp_rda2720mGlobal->chgr_status)};
    p->plugged = level;
    if (!level)
    {
        p->type = DRV_CHARGER_TYPE_NONE;
    }
    else
    {
        if (status.b.dcp_int)
            p->type = DRV_CHARGER_TYPE_DCP;
        else if (status.b.chg_det != 0 && status.b.cdp_int)
            p->type = DRV_CHARGER_TYPE_CDP;
        else if (status.b.chg_det == 0 && status.b.sdp_int)
            p->type = DRV_CHARGER_TYPE_SDP;
        else
            p->type = DRV_CHARGER_TYPE_UNKOWN;
    }

    OSI_LOGD(0, "chg: level/%d status/%x type/%d", level, status.v, p->type);

    drvPmicEicTrigger(DRV_PMIC_EIC_CHGR_INT, CHARGER_DEBOUNCE, !level);
}

static void _chargeIsrCB(void *ctx)
{
    drvChargerContext_t *p = &gDrvChargeCtx;
    _updateChargerStatus(p);

    if (p->cb != NULL)
        p->cb(p->plug_cb_ctx, p->plugged);

    if (true == _drvGetUsbStatus())
        module_state.adp_type = CHGMNG_ADP_USB;
    else
        module_state.adp_type = CHGMNG_ADP_STANDARD;

#ifdef CONFIG_SUPPORT_BATTERY_CHARGER
    _drvChargePluginHandler();
#endif
}

#define CHARGER_THREAD_PRIORITY (OSI_PRIORITY_NORMAL)
#define CHARGER_THREAD_STACK_SIZE (8192 * 4)
#define CHARGER_THREAD_MAX_EVENT (32)

static void _drvChargetPhyInit(void)
{
    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol0;
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol0, chgr_cc_en, 1),
        HAL_ADI_BUS_CHANGE_END);
}
/*****************************************************************************/
//  Description:	  This function is used to turn on the charger.
/*****************************************************************************/
static void _drvChargerTurnOn(void)
{
    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol0;
    if (charge_stop == true)
        return;
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol0, chgr_pd, 0),
        HAL_ADI_BUS_CHANGE_END);

    OSI_LOGI(0, "chg: Turn On charger");
}

static void _drvChargerTurnOff(void)
{
    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol0;
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol0, chgr_pd, 1),
        HAL_ADI_BUS_CHANGE_END);
    OSI_LOGI(0, "chg: Turn Off charger");
}

static void _drvChargerSetEndCurrent(uint32_t ma)
{
    // 0x00: 0.9full, 0x01: 0.4full, 0x2: 0.2full, 0x3: 0.1full,
#define CHGR_ITERM_0P9 9
#define CHGR_ITERM_0P4 4
#define CHGR_ITERM_0P2 2
#define CHGR_ITERM_0P1 1

    uint32_t b = 0, full, iterm;

    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol_0;
    REG_RDA2720M_GLOBAL_CHGR_CTRL1_T chargCtrol_1;
    OSI_LOGI(0, "chg: SetChargeEndCurrent %d", ma);
    chargCtrol_1.v = halAdiBusRead(&hwp_rda2720mGlobal->chgr_ctrl1);
    b = chargCtrol_1.b.chgr_cc_i;
    if (b <= 0xA)
    {
        full = b * 50 + CHARGER_CURRENT_300MA;
    }
    else if (b <= 0xF)
    {
        full = (b - 0xA) * 100 + CHARGER_CURRENT_800MA;
    }

    iterm = ma * 10 / full;

    if (iterm >= 9)
    {
        b = 0;
    }
    else if (iterm >= 4)
    {
        b = 1;
    }
    else if (iterm >= 2)
    {
        b = 2;
    }
    else
    {
        b = 3;
    }

    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol_0, chgr_iterm, b),
        HAL_ADI_BUS_CHANGE_END);
}

static void _drvChargerSetEndVolt(uint32_t mv)
{
// 0x00: 4.2v, 0x01: 4.3v, 0x02: 4.4v, 0x03: 4.5v
#define CHGR_END_V_4200 4200
#define CHGR_END_V_4500 4500

    uint32_t b;
    OSI_LOGI(0, "chg: SetChargeEndVolt %d", mv);

    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol_0;
    OSI_ASSERT((mv >= CHGR_END_V_4200) && (mv <= CHGR_END_V_4500), "mv error");
    b = (mv - CHGR_END_V_4200) / 100;

    b &= 3;
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol_0, chgr_end_v, b),
        HAL_ADI_BUS_CHANGE_END);
}

static void _drvChargerSetBatOvpVolt(uint32_t mv)
{
}
static uint32_t _drvChargerIsBatDetOk(void)
{
    REG_RDA2720M_GLOBAL_MIXED_CTRL_T bat;
    bat.v = halAdiBusRead(&hwp_rda2720mGlobal->mixed_ctrl);
    return bat.b.batdet_ok;
}

static void _drvChargerSetChangerCurrent(uint16_t current)
{
    uint32_t temp = 0;
    REG_RDA2720M_GLOBAL_CHGR_CTRL1_T cc_reg;

    OSI_LOGI(0, "chg: SetChgCurrent=%d", current);
    if ((current < CHARGER_CURRENT_300MA) || (current > CHARGER_CURRENT_MAX) || (current % 50))
    {
        OSI_ASSERT(0, "chg: current invlid.");
    }

    switch (current)
    {
    case CHARGER_CURRENT_300MA:
        temp = 0;
        break;
    case CHARGER_CURRENT_350MA:
        temp = 1;
        break;
    case CHARGER_CURRENT_400MA:
        temp = 2;
        break;
    case CHARGER_CURRENT_450MA:
        temp = 3;
        break;
    case CHARGER_CURRENT_500MA:
        temp = 4;
        break;
    case CHARGER_CURRENT_550MA:
        temp = 5;
        break;
    case CHARGER_CURRENT_600MA:
        temp = 6;
        break;
    case CHARGER_CURRENT_650MA:
        temp = 7;
        break;
    case CHARGER_CURRENT_700MA:
        temp = 8;
        break;
    case CHARGER_CURRENT_750MA:
        temp = 9;
        break;
    case CHARGER_CURRENT_800MA:
        temp = 10;
        break;
    case CHARGER_CURRENT_900MA:
        temp = 11;
        break;
    case CHARGER_CURRENT_1000MA:
        temp = 12;
        break;
    case CHARGER_CURRENT_1100MA:
        temp = 13;
        break;
    case CHARGER_CURRENT_1200MA:
        temp = 14;
        break;
    case CHARGER_CURRENT_MAX:
        temp = 15;
        break;
    default:
        OSI_ASSERT(0, "chg: current errro");
    }

    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl1, REG_FIELD_MASKVAL1(cc_reg, chgr_cc_i, temp),
        HAL_ADI_BUS_CHANGE_END);
}

/*****************************************************************************/
//  Description:	 This function sets the lowest switchover point between constant-current
// 					 and constant-voltage modes.  default 0x10
//*****************************************************************************/
static void _drvChargerSetSwitchOverPoint(uint16_t eswitchpoint)
{
    OSI_ASSERT(eswitchpoint <= 63, "chg: eswitchpoint invalid.");
    OSI_LOGI(0, "chg: SetSwitchoverPoint %d", eswitchpoint);

    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol0;
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol0, chgr_cv_v, eswitchpoint),
        HAL_ADI_BUS_CHANGE_END);
}
/*****************************************************************************/
//  Description:    send massage to charge task.

/*****************************************************************************/
static void _chargerServerThread(void *ctx);

#ifdef CONFIG_SUPPORT_BATTERY_CHARGER
static void _drvAdaptorSendMsgToChgTask(chgMsg_t chgMsg, uint32_t event_param)
{
    drvChargerContext_t *d = &gDrvChargeCtx;

    gDrvChargeCtx.adaptor_msg = chgMsg;
    if (chgMsg == CHG_CHARGER_PLUG_IN_MSG)
        OSI_LOGV(0, "chg: Send Msg CHG_CHARGER_PLUG_IN_MSG");
    if (chgMsg == CHG_CHARGER_PLUG_OUT_MSG)
        OSI_LOGV(0, "chg: Send Msg CHG_CHARGER_PLUG_OUT_MSG");

    OSI_LOGD(0, "chg: Send adaptor Msg To chg thread %d", chgMsg);
    while (false == osiThreadCallback(gDrvChargeCtx.chgTaskId, _chargerServerThread, d))
        ;
}

static void _drvMonitorSendMsgToChgTask(chgMsg_t chgMsg, uint32_t event_param)
{
    drvChargerContext_t *d = &gDrvChargeCtx;

    gDrvChargeCtx.battery_msg = chgMsg;
    if (chgMsg == CHG_VBAT_MONITOR_MSG)
        OSI_LOGV(0, "chg: Send Msg CHG_VBAT_MONITOR_MSG");

    OSI_LOGD(0, "chg: Send bat monitor Msg To chg thread %d", chgMsg);
    while (false == osiThreadCallback(gDrvChargeCtx.chgTaskId, _chargerServerThread, d))
        ;
}
#endif

static void _drvChargerSendMsgToChgTask(chgMsg_t chgMsg, uint32_t event_param)
{
    drvChargerContext_t *d = &gDrvChargeCtx;

    gDrvChargeCtx.charge_msg = chgMsg;
    if (chgMsg == CHG_VBAT_MONITOR_MSG)
        OSI_LOGV(0, "chg: Send Msg CHG_VBAT_MONITOR_MSG");
    if (chgMsg == CHG_CHARGER_MONITOR_MSG)
        OSI_LOGV(0, "chg: Send Msg CHG_CHARGER_MONITOR_MSG");
    if (chgMsg == CHG_MODULE_RESET_MSG)
        OSI_LOGV(0, "chg: Send Msg CHG_MODULE_RESET_MSG");

#ifdef BATTERY_DETECT_SUPPORT
    if (chgMsg == CHG_BATTERY_OFF_MSG)
        OSI_LOGV(0, "chg: Send Msg CHG_BATTERY_OFF_MSG");
#endif
    if (chgMsg == CHG_MAX_MSG)
        OSI_LOGV(0, "chg: Send Msg CHG_MAX_MSG");

    OSI_LOGD(0, "chg: Send Charging Msg To chg thread %d", chgMsg);
    while (false == osiThreadCallback(gDrvChargeCtx.chgTaskId, _chargerServerThread, d))
        ;
}

/*****************************************************************************/
//  Description:    timeout function of charge timer
/*****************************************************************************/
#ifdef CONFIG_SUPPORT_BATTERY_CHARGER
static void _drvChargerTimerHandler(void *state)
{
    _drvChargerSendMsgToChgTask(CHG_CHARGER_MONITOR_MSG, 0);
}
#endif

static uint32_t _drvChgGetStatus(void)
{
    return halAdiBusRead(&hwp_rda2720mGlobal->chgr_status);
}

#ifdef _READ_CV_STATUS_REG_

static bool _drvchargerGetCVStatus(void)
{

    REG_RDA2720M_GLOBAL_CHGR_STATUS_T chg_status;
    chg_status = (REG_RDA2720M_GLOBAL_CHGR_STATUS_T)_drvChgGetStatus();

    return chg_status.b.chgr_cv_status & 0x1;
}
#endif

static bool _drvChargerIsPresent(void)
{
    REG_RDA2720M_GLOBAL_CHGR_STATUS_T chg_status;
    chg_status = (REG_RDA2720M_GLOBAL_CHGR_STATUS_T)_drvChgGetStatus();

    return chg_status.b.chgr_int & 0x1;
}

static bool _drvchargerIsCalibrationResetMode(void)
{
    return false;
}

static bool _drvChargerIsDownLoadMode(void)
{
    return false;
}

/*****************************************************************************/
//  Description:    The function monitor vbat status,Be called by DoIdle_Callback.
/*****************************************************************************/
#ifdef CONFIG_SUPPORT_BATTERY_CHARGER
static void _drvChargerVbatTimerHandler(void *ctx)
{
    _drvMonitorSendMsgToChgTask(CHG_VBAT_MONITOR_MSG, 0);
}
#endif

static uint32_t _drvChargerCheckVbatPresent(void)
{
#ifndef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE
    uint32_t batdet_ok = _drvChargerIsBatDetOk();
    return 1;
    return batdet_ok;
#else
    return quec_get_battery_state();
#endif
}

/*****************************************************************************/
//  Description:    The function initializes the Vbat queue
/*****************************************************************************/
static void _drvChargerVbatQueueInit(uint32_t vbat_vol, uint32_t queue_len)
{
    uint32_t i;

    uint32_t critical = osiEnterCritical(); //must do it
    vbat_queue.sum = vbat_vol * queue_len;
    vbat_queue.pointer = 0;
    vbat_queue.queue_len = queue_len;
    osiExitCritical(critical);

    for (i = 0; i < queue_len; i++) //init vbat queue
    {
        vbat_queue.queue[i] = vbat_vol;
    }

    module_state.bat_statistic_vol = vbat_vol;
}

static uint32_t _drvChargerGetVbatVol(void);

uint16_t drvChargerGetVbatRT()
{
    return (uint16_t)_drvChargerGetVbatVol();
}

static void _drvChargerSetChargerCurrentAccordMode(chgAdapterType_e mode)
{
    switch (mode)
    {
    case CHGMNG_ADP_STANDARD:
        _drvChargerSetChangerCurrent(chg_param.standard_chg_current);
        break;
    case CHGMNG_ADP_NONSTANDARD:
        _drvChargerSetChangerCurrent(chg_param.nonstandard_chg_current);
        break;
    case CHGMNG_ADP_USB:
        _drvChargerSetChangerCurrent(chg_param.usb_chg_current);
        break;
    case CHGMNG_ADP_UNKNOW:
    default:
        _drvChargerSetChangerCurrent(CHARGER_CURRENT_700MA);
    }
}

/*****************************************************************************/
//  Description:    Charge current queue init
/*****************************************************************************/
static void _drvChargerCurrentQueueInit(uint32_t current)
{
    uint32_t i;

    current_queue.sum = current * CHG_CURRENT_BUFF_SIZE;
    current_queue.pointer = 0;

    for (i = 0; i < CHG_CURRENT_BUFF_SIZE; i++)
    {
        current_queue.queue[i] = current;
    }
    module_state.charging_current = current;
}

/*****************************************************************************/
//  Description:    This function is used to convert VProg ADC value to charge current value.
/*****************************************************************************/
static uint16_t _drvChargerAdcValueToCurrent(uint32_t voltage)
{
    uint32_t current_type = 300;
    uint32_t temp;

    if (module_state.adp_type == CHGMNG_ADP_STANDARD)
    {
        current_type = chg_param.standard_chg_current;
    }
    else if (module_state.adp_type == CHGMNG_ADP_USB)
    {
        current_type = chg_param.usb_chg_current;
    }
    else if (module_state.adp_type == CHGMNG_ADP_NONSTANDARD)
    {
        current_type = chg_param.nonstandard_chg_current;
    }
    else if (module_state.adp_type == CHGMNG_ADP_UNKNOW)
    {
        current_type = CHARGER_CURRENT_450MA;
    }

    /*
        1.internal chip voltage for ADC measure:v1---->v1 = voltage*0.241 = voltage*VOL_DIV_P1/VOL_DIV_P2
        2.current convert voltage expressions: VOL_TO_CUR_PARAM*current/curret_type = v1 = voltage*VOL_DIV_P1/VOL_DIV_P2
           ---->current = ((current_type*voltage*VOL_DIV_P1)/VOL_TO_CUR_PARAM)/VOL_DIV_P2;
    */
    temp = ((current_type * voltage * VBAT_VOL_DIV_P1) / VOL_TO_CUR_PARAM) / VBAT_VOL_DIV_P2;

    OSI_LOGD(0, "chg:  ADCValueToCurrent current_type= %d, current:%d\n", current_type, temp);
    return (uint16_t)temp;
}

/*****************************************************************************/
//  Description:    This function gets the virtual Vprog value. Because the Iprog is not steady
//                  enough when it is larger than 500mA, we have to calculate its average. The
//                  sampling times perhaps will be adjusted in the eventual project.
/*****************************************************************************/
static uint32_t _drvChargerGetVprogAdcResult(void)
{
    int i, j, temp;
    int vprog_result[VPROG_RESULT_NUM];

    for (i = 0; i < VPROG_RESULT_NUM; i++)
    {
        vprog_result[i] =
            drvAdcGetChannelVolt(ADC_CHANNEL_PROG2ADC, ADC_SCALE_5V000);
    }

    for (j = 1; j <= VPROG_RESULT_NUM - 1; j++)
    {
        for (i = 0; i < VPROG_RESULT_NUM - j; i++)
        {
            if (vprog_result[i] > vprog_result[i + 1])
            {
                temp = vprog_result[i];
                vprog_result[i] = vprog_result[i + 1];
                vprog_result[i + 1] = temp;
            }
        }
    }

    return vprog_result[VPROG_RESULT_NUM / 2];
}
/*****************************************************************************/
//  Description:    Get charging current
/*****************************************************************************/
static uint32_t _drvChargerGetChgCurrent(void)
{
    uint32_t progAdcResult = 0;
    uint32_t chgCurrent = 0;
    progAdcResult = _drvChargerGetVprogAdcResult();
    chgCurrent = _drvChargerAdcValueToCurrent(progAdcResult);
    return chgCurrent;
}

static uint32_t _drvChargerGetADCResultByChannel(uint32_t adc_channel, int32_t scale)
{
    int32_t i, j, temp;
    int32_t vbat_result[VBAT_RESULT_NUM];

    for (i = 0; i < VBAT_RESULT_NUM; i++)
    {
        vbat_result[i] = drvAdcGetChannelVolt(adc_channel, scale);
        OSI_LOGD(0, "chg: _GetADCResult i:%d adc:%d", i, vbat_result[i]);
    }

    for (j = 1; j <= VBAT_RESULT_NUM - 1; j++)
    {
        for (i = 0; i < VBAT_RESULT_NUM - j; i++)
        {
            if (vbat_result[i] > vbat_result[i + 1])
            {
                temp = vbat_result[i];
                vbat_result[i] = vbat_result[i + 1];
                vbat_result[i + 1] = temp;
            }
        }
    }

    return vbat_result[VBAT_RESULT_NUM / 2];
}

static uint32_t _drvChargerGetChannelVol(uint32_t adc_channel, int32_t scale)
{

    uint32_t vol = 0;

    vol = _drvChargerGetADCResultByChannel(adc_channel, scale);

    OSI_LOGD(0, "chg: etChannelVol adc_channel:%d vol=%d", adc_channel, vol);

    return vol;
}

static int32_t _drvChargerGetVbatTempVolValue(void)
{
    uint32_t vol = 0;
    uint32_t i = 0;
    int32_t temp = 0;
    uint32_t table_size = sizeof(vbat_temp_table) / sizeof(vbat_temp_table[0]);

    vol = _drvChargerGetChannelVol(ADC_CHANNEL_BAT_DET, ADC_SCALE_1V250);

    for (i = 0; i < table_size; i++)
    {
        if (vol >= vbat_temp_table[i][0])
        {
            break;
        }
    }

    if (i < table_size)
    {
        temp = vbat_temp_table[i][1];
    }
    else
    {
        temp = vbat_temp_table[table_size - 1][1];
    }

    OSI_LOGD(0, "chg: VbatTempVolValue temp=%d, vol=%d", temp, vol);

    return temp;
}

OSI_UNUSED static uint32_t _drvChargerGetCurrentTemp(void)
{
    int32_t result = 0;

    result = CHG_NV_TEMP_OFFSET + _drvChargerGetVbatTempVolValue();
    OSI_LOGD(0, "chg: GetCurrentTemp = %d", result);
    module_state.charging_temperature = result;
    return (uint32_t)result;
}

static void _drvChargerSendMsgToClient(CHR_SVR_MSG_SERVICE_E msg, uint32_t param)
{
    drvChargerContext_t *p = &gDrvChargeCtx;
    if (p->notice_cb)
        p->notice_cb();
}

/*****************************************************************************/
//  Description:    Enter Charge start state.
/*****************************************************************************/
static void _drvChargerStartCharge()
{
    //to detect charge unplug.
    OSI_LOGI(0, "chg: ChargeStartHandler!");

    if (_drvChargerCheckVbatPresent() == 0)
    {
        _drvChargerTurnOff();

        OSI_LOGI(0, "chg: StartCharge NO BAT");
        return;
    }
    _drvChargerVbatQueueInit(module_state.bat_statistic_vol, CHG_VBAT_BUFF_SIZE);
    _drvChargerSetChargerCurrentAccordMode(module_state.adp_type);
    _drvChargerSetEndCurrent(CHG_STOP_VPROG);
    _drvChargerCurrentQueueInit(_drvChargerGetChgCurrent());
    _drvChargerTurnOn(); //Ensure that charge module can be turned on always.
    _drvChargerSendMsgToClient(CHR_CHARGE_START_IND, 0);
    
#ifndef CONFIG_QUEC_PROJECT_FEATURE
    if (true != osiTimerStart(gDrvChargeCtx.chgMonTimer, CHARGING_ON_TIMER_INTERVAL))
#else
    if (true != osiTimerStart(gDrvChargeCtx.chgMonTimer, quec_usb_current_setting.charging_on_time))
#endif
    {
        OSI_LOGE(0, "chg: Invalid application timer pointer!");
        g_charge_plus = 0;
    }
    g_charge_plus = 1;
    module_state.charging_stop_reason = CHG_INVALIDREASON;
    
#ifndef CONFIG_QUEC_PROJECT_FEATURE    
    charge_endtime_counter = (chg_param.chg_timeout * 1000) / (CHARGING_ON_TIMER_INTERVAL + CHARGING_OFF_TIMER_INTERVAL); //set end time
#else
    charge_endtime_counter = (chg_param.chg_timeout * 1000) / (quec_usb_current_setting.charging_on_time + quec_usb_current_setting.charging_off_time); //set end time
#endif
    cv_status_counter = 0;
    ovp_detect_counter = 0;
    ovp_detect_bat_cnt = 0;

#ifndef CONFIG_QUEC_PROJECT_FEATURE
    _CHG_COUNTER_INIT(charge_endtime_counter,
                      (chg_param.chg_timeout * 1000) / (CHARGING_ON_TIMER_INTERVAL + CHARGING_OFF_TIMER_INTERVAL));
#else
    _CHG_COUNTER_INIT(charge_endtime_counter,
                      (chg_param.chg_timeout * 1000) / (quec_usb_current_setting.charging_on_time + quec_usb_current_setting.charging_off_time));
#endif
    _CHG_COUNTER_INIT(cv_status_counter, 0);

    if (module_state.bat_remain_cap == 0xff)
    {
        module_state.bat_remain_cap = _drvChargerVoltagePercentum(
            module_state.bat_statistic_vol,
            module_state.chgmng_state, false);
    }

    _drvChargerSendMsgToClient(CHR_CHARGE_START_IND, 0);
}

/*****************************************************************************/
//  Description:    Stop charge.
/*****************************************************************************/
static void _drvChargerStopCharge(chgStopReason_e reason)
{
    OSI_LOGI(0, "chg: StopCharge!");

    module_state.charging_stop_reason = reason;
    if (CHG_CHARGERUNPLUG == reason)
    {
        module_state.adp_type = CHGMNG_ADP_UNKNOW;
    }
    _drvChargerTurnOff();
    _drvChargerVbatQueueInit(module_state.bat_statistic_vol, DISCHG_VBAT_BUFF_SIZE);
    _CHG_COUNTER_INIT(warning_counter, 0);
    _drvChargerCurrentQueueInit(0);

    switch (reason)
    {
    case CHG_CHARGERUNPLUG:
    case CHG_PESUDO_CHARGERUNPLUG:
        if (true != osiTimerStop(gDrvChargeCtx.chgMonTimer))
        {
            OSI_LOGI(0, "chg: Invalid application timer pointer!");
        }

        OSI_LOGI(0, "chg: CHR_CHARGE_DISCONNECT1!!!");
        _drvChargerSendMsgToClient(CHR_CHARGE_DISCONNECT, 0);
        break;

    case CHG_TIMEOUT:
#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE        
        OSI_LOGI(0, "chg: CHR_CHARGE_TIMEOUT!!!");
        _drvChargerSendMsgToClient(CHR_CHARGE_FINISH, 0);
        break;
#endif
    
    case CHG_VBATEND:
#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE   
        OSI_LOGI(0, "chg: CHR_BATTERY_DISCONNECT!!!");
        //_drvChargerSendMsgToClient(CHR_CHARGE_DISCONNECT, 0);
        break;
#endif
    
    case CHG_CHARGEDONE:
        OSI_LOGI(0, "chg: CHR_CHARGE_FINISH!!!");
        _drvChargerSendMsgToClient(CHR_CHARGE_FINISH, 0);
        break;

    case CHG_OVERVOLTAGE:
    case CHG_OVERTEMP:
        _drvChargerSendMsgToClient(CHR_CHARGE_FAULT, 0); //add by paul
        break;
    default:
        break;
    }
}

/*****************************************************************************/
//  Description:    This function is used to process Finite State Machine of charge module.
/*****************************************************************************/
static void _drvChargerFSMProcess(chgFsmEvent_e fsm_event, uint32_t condition)
{
    OSI_LOGI(0, "chg: FSMProcess fsm_event:%d", fsm_event);

    switch (fsm_event)
    {
    case CHG_FSM_EVENT_INIT:
        module_state.chgmng_state = CHG_IDLE;
        break;

    case CHG_FSM_EVENT_START_CHG:
        _drvChargerStartCharge();
        module_state.chgmng_state = CHG_CHARGING; ///should be changed in future
        break;

    case CHG_FSM_EVENT_STOP_CHG:
        module_state.chgmng_state = CHG_IDLE;
        _drvChargerStopCharge(condition);
        break;

    default:
        OSI_LOGE(0, "chg: Error chgFsmEvent!!!");
        osiPanic();
        break;
    }
}

static void _drvChargerInfoLog(void)
{
    OSI_LOGD(0, "chg: module_state.chgmng_state:%d", module_state.chgmng_state);
    OSI_LOGD(0, "chg: .bat_statistic_vol:%d", module_state.bat_statistic_vol);
    OSI_LOGD(0, "chg: .bat_cur_vol:%d", module_state.bat_cur_vol);
    OSI_LOGD(0, "chg: .bat_remain_cap:%d", module_state.bat_remain_cap);

    if (_drvChargerIsPresent())
    {
        OSI_LOGD(0, "chg: .charging_current:%d", module_state.charging_current);
        OSI_LOGD(0, "chg: .adp_type:%d", module_state.adp_type);
        OSI_LOGD(0, "chg: .charging_stop_reason:%d", module_state.charging_stop_reason);
        OSI_LOGD(0, "chg: hw_switch_point:%d", hw_switch_point);
        OSI_LOGD(0, "chg: chg_end_vol:%d", chg_param.chg_end_vol);
    }
    else
    {
        OSI_LOGD(0, "chg: .bat_remain_cap:%d", module_state.bat_remain_cap);
        OSI_LOGD(0, "chg: dischg_param.shutdown_vol:%d", dischg_param.shutdown_vol);
        OSI_LOGD(0, "chg: .deadline_vol:%d", dischg_param.deadline_vol);
        OSI_LOGD(0, "chg: .warning_vol:%d", dischg_param.warning_vol);
    }
    OSI_LOGD(0, "chg: info vbat_queue.pointer:%d", vbat_queue.pointer);
}

/*****************************************************************************/
//  Description:    This function is used to get the result of Vcharge ADC value.
//                  Return: the Vcharge adc value.
/*****************************************************************************/
static uint32_t _drvChgGetVol(void)
{
    uint32_t vol;

    vol = (uint32_t)drvAdcGetChannelVolt(ADC_CHANNEL_VCHGSEN, ADC_SCALE_1V250);
    if (vol == -1)
    {
        osiThreadSleep(2);

        vol = (uint32_t)drvAdcGetChannelVolt(ADC_CHANNEL_VCHGSEN, ADC_SCALE_1V250);
    }
    OSI_LOGI(0, "_drvChgGetVol vol:%d ", vol);
    return vol;
}
static uint32_t _drvChargerCheckChargerIsOverVolBySoft(uint32_t chgVol)
{

    if (chgVol >= ovp_param.ovp_over_vol)
    {
        ovp_detect_counter++;
    }
    else
    {
        ovp_detect_counter = 0;
    }

    if (ovp_detect_counter >= OVP_DETECT_VALID_TIMES)
    {
        ovp_detect_counter = OVP_DETECT_VALID_TIMES;

        return 1;
    }

    return 0;
}
/*****************************************************************************/
//  Description:    Get Vbat voltage
/*****************************************************************************/
static uint32_t _drvChargerGetVbatVol(void)
{
    uint32_t vol;

    vol = (uint32_t)drvAdcGetChannelVolt(ADC_CHANNEL_VBATSENSE, ADC_SCALE_5V000);

    if (vol == -1)
    {
        osiThreadSleep(2);

        vol = (uint32_t)drvAdcGetChannelVolt(ADC_CHANNEL_VBATSENSE, ADC_SCALE_5V000);
    }

    return vol;
}
static uint32_t _drvChargerCheckIsBatOverVolBySoft(uint32_t batChargingVol)
{
    return 0;
    if (batChargingVol >= (chg_param.chg_end_vol + 30))
    {
        ovp_detect_bat_cnt++;
    }
    else
    {
        ovp_detect_bat_cnt = 0;
    }

    if (ovp_detect_bat_cnt >= OVP_DETECT_VALID_TIMES)
    {
        ovp_detect_bat_cnt = OVP_DETECT_VALID_TIMES;

        return 1;
    }

    return 0;
}

static bool _drvChargerVChgIsOV(void)
{
    REG_RDA2720M_GLOBAL_CHGR_STATUS_T reg_sta;

    reg_sta = (REG_RDA2720M_GLOBAL_CHGR_STATUS_T)_drvChgGetStatus();
    return false;
    if (reg_sta.b.vchg_ovi == 1)
        return true;
    else
        return false;
}

static bool _drvChargerVBatIsOV(void)
{
    return false;
}

/*****************************************************************************/
//  Description:    update charge current queue
/*****************************************************************************/
static void _drvChargerCurrentQueueUpdate(uint32_t current)
{
    current_queue.sum += current;
    current_queue.sum -= current_queue.queue[current_queue.pointer];
    module_state.charging_current = current_queue.sum / CHG_CURRENT_BUFF_SIZE;

    current_queue.queue[current_queue.pointer++] = current;

    if (current_queue.pointer == CHG_CURRENT_BUFF_SIZE)
    {
        current_queue.pointer = 0;
    }
}
/*****************************************************************************/
//  Description:    update vbat queue
/*****************************************************************************/
static void _drvChargerVbatQueueUpdate(uint32_t vbat)
{
    uint32_t critical = osiEnterCritical(); //must do it
    vbat_queue.sum += vbat;
    vbat_queue.sum -= vbat_queue.queue[vbat_queue.pointer];
    module_state.bat_statistic_vol = vbat_queue.sum / vbat_queue.queue_len;
    osiExitCritical(critical);

    vbat_queue.queue[vbat_queue.pointer++] = vbat;

    if (vbat_queue.pointer == vbat_queue.queue_len)
    {
        vbat_queue.pointer = 0;
    }
}

/*****************************************************************************/
//  Description:    Convert ADCVoltage to percentrum.
/*****************************************************************************/
static uint32_t _drvChargerVoltagePercentum(uint32_t voltage, chgState_e is_charging, bool update)
{
    uint16_t percentum;
    int32_t temp = 0;
    int pos = 0;
    static uint16_t pre_percentum = 0xffff;

    if (update)
    {
        pre_percentum = 0xffff;
        return 0;
    }
    for (pos = 0; pos < BAT_CAPACITY_STEP - 1; pos++)
    {
        if (voltage > dischg_bat_capacity_table[pos][0])
            break;
    }
    if (pos == 0)
    {
        percentum = 100;
    }
    else
    {
        temp = dischg_bat_capacity_table[pos][1] - dischg_bat_capacity_table[pos - 1][1];
        temp = temp * (int32_t)(voltage - dischg_bat_capacity_table[pos][0]);
        temp = temp / (dischg_bat_capacity_table[pos][0] - dischg_bat_capacity_table[pos - 1][0]);
        temp = temp + dischg_bat_capacity_table[pos][1];
        if (temp < 0)
        {
            temp = 0;
        }
        percentum = temp;
    }

    // Remove the case which doesn't make sense
    if (pre_percentum == 0xffff)
    {
        pre_percentum = percentum;
    }
    else if (pre_percentum > percentum)
    {
        if (is_charging == CHG_CHARGING)
        {
            percentum = pre_percentum;
        }
        else
        {
            pre_percentum = percentum;
        }
    }
    else
    {
        if (is_charging != CHG_CHARGING)
        {
            percentum = pre_percentum;
        }
        else
        {
            pre_percentum = percentum;
        }
    }

    OSI_LOGD(0, "chg: bat percent : %d,dischg_bat_capacity_table[2][0]:%d,dischg_bat_capacity_table[11][0]:%d",
             temp, dischg_bat_capacity_table[2][0], dischg_bat_capacity_table[11][0]);

    return percentum;
}

/*****************************************************************************/
//  Description:    The function calculates the vbat every 2 seconds.
/*****************************************************************************/

static void _drvChargerVbatMonitorRoutine()
{
    /*If we had inform the upper layer to shutdown, we will not send any other messages
    because too many messages can block the message queue.*/
    uint32_t vol;
    if (isshutting_down)
    {
        return;
    }

    if (_drvChargerCheckVbatPresent() == 0)
    {
        OSI_LOGI(0, "chg: _drvChargerVbatMonitorRoutine NO BAT");
        return;
    }

#ifdef _DETEDCT_BATTERY_TEMP_
    _drvChargerGetCurrentTemp();
    OSI_LOGI(0, "chg: charging_temperature = %d", module_state.charging_temperature);
    if (module_state.charging_temperature >= HOT_TEMP) //
    {
        _drvChargerSendMsgToClient(CHR_WARNING_IND, 0);
        if (module_state.chgmng_state == CHG_CHARGING)
        {
            _drvChargerFSMProcess(CHG_FSM_EVENT_STOP_CHG, CHG_OVERTEMP);
            module_state.charging_stop_reason = CHG_OVERTEMP;
            OSI_LOGI(0, "chg: stop charging for over temperaure");
        }
    }
    else
    {
        OSI_LOGI(0, "chg: low temperaure %d", module_state.chgmng_state);
        if (module_state.chgmng_state == CHG_IDLE)
        {
            OSI_LOGI(0, "chg: start charging for over temperaure");
            _drvChargerFSMProcess(CHG_FSM_EVENT_START_CHG, 0);
        }
    }
#endif

    if (module_state.chgmng_state == CHG_IDLE)
    {
        _drvChargerInfoLog();
        vol = _drvChargerGetVbatVol();
        if (vol != -1)
        {

            module_state.bat_cur_vol = vol;
            _drvChargerVbatQueueUpdate(module_state.bat_cur_vol);
        }
        else
        {
            OSI_LOGI(0, "chg: adc error -1");
        }

        if (module_state.bat_statistic_vol < dischg_param.warning_vol)
        {
            if (0 == warning_counter)
            {
                OSI_LOGI(0, "chg: CHR_WARNING_IND!!!");
                _drvChargerSendMsgToClient(CHR_WARNING_IND, 0);
            }
            _CHGMNG_COUNTER_ADD_AND_AUTO_ROUND(warning_counter, dischg_param.warning_count);

            if ((module_state.bat_statistic_vol < dischg_param.shutdown_vol) ||
                (module_state.bat_cur_vol < dischg_param.deadline_vol))
            {
                if (0 == warning_counter)
                {
                    OSI_LOGI(0, "chg: WARNING_IND Before shutdown!!!");
                    _drvChargerSendMsgToClient(CHR_WARNING_IND, 0);
                    _CHG_COUNTER_ADD(warning_counter);
                }
                else
                {
                    OSI_LOGI(0, "chg:  SHUTDOWN_IND!!!");
                    _drvChargerSendMsgToClient(CHR_SHUTDOWN_IND, 0);

                    if (recent_message[0] == 0)
                    { ///msg is send
                        isshutting_down = true;

                        osiTimerStartRelaxed(gDrvChargeCtx.batMonTimer, CHG_BAT_MONITOR_PERIOD_, OSI_WAIT_FOREVER);
                        return;
                    }
                }
            }
        }
        else
        {
            _CHG_COUNTER_INIT(warning_counter, 0);
        }
    }

    module_state.bat_remain_cap = _drvChargerVoltagePercentum(
        module_state.bat_statistic_vol,
        module_state.chgmng_state, false);

    _drvChargerSendMsgToClient(
        CHR_CAP_IND, module_state.bat_remain_cap);

    osiTimerStartRelaxed(gDrvChargeCtx.batMonTimer, CHG_BAT_MONITOR_PERIOD_, OSI_WAIT_FOREVER);
    OSI_LOGI(0, "chg: Vbat Monitor next ! bat capicity = %d", module_state.bat_remain_cap);
}

/*****************************************************************************/
//  Description:    When charger connect start charger monitor,Be called by charger task.
/*****************************************************************************/
static void _drvChargerMonitorChargerRoutine(void)
{
    uint32_t chgVol = 0;
    uint32_t isChgOVP = false;

    uint32_t batVol = 0;
    uint32_t isBatOVP = false;
    _drvChargerInfoLog();
    // switch positive and negative level to charge
    if (1 == g_charge_plus)
    {
#ifndef CONFIG_QUEC_PROJECT_FEATURE     
        osiTimerStart(gDrvChargeCtx.chgMonTimer, CHARGING_OFF_TIMER_INTERVAL);
#else
        osiTimerStart(gDrvChargeCtx.chgMonTimer, quec_usb_current_setting.charging_off_time);
#endif
        g_charge_plus = 0;
    }
    else
    {
#ifndef CONFIG_QUEC_PROJECT_FEATURE
        osiTimerStart(gDrvChargeCtx.chgMonTimer, CHARGING_ON_TIMER_INTERVAL);
#else
        osiTimerStart(gDrvChargeCtx.chgMonTimer, quec_usb_current_setting.charging_on_time);
#endif
        g_charge_plus = 1;
    }

    if (_drvChargerCheckVbatPresent() == 0)
    {
        OSI_LOGI(0, "chg: _drvChargerStartCharge NO BAT");
#ifndef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE            
        _drvChargerFSMProcess(CHG_FSM_EVENT_STOP_CHG, CHG_VBATEND);
#else
        if(module_state.chgmng_state == CHG_CHARGING)
        {
            _drvChargerFSMProcess(CHG_FSM_EVENT_STOP_CHG, CHG_VBATEND);
        }
#endif
        return;
    }

    chgVol = _drvChgGetVol();
    isChgOVP = _drvChargerCheckChargerIsOverVolBySoft(chgVol);

    batVol = _drvChargerGetVbatVol();
    isBatOVP = _drvChargerCheckIsBatOverVolBySoft(batVol);

    OSI_LOGI(0, "chg:  Monitor  bat_statistic_vol =%d,  batVol =%d ,chgVol=%d, rechg_vol=%d, ovp_resume_vol=%d, isChgOVP=%d, isBatOVP=%d",
             module_state.bat_statistic_vol, batVol, chgVol, chg_param.rechg_vol, ovp_param.ovp_resume_vol, isChgOVP, isBatOVP);

    // check if over voltage
    if (_drvChargerVBatIsOV() || _drvChargerVChgIsOV() || (isChgOVP == true) || (isBatOVP == true))
    {

        OSI_LOGI(0, "chg: ChargeMonitorRoutine over voltage");
        if (module_state.charging_stop_reason != CHG_OVERVOLTAGE)
        {
            _drvChargerFSMProcess(CHG_FSM_EVENT_STOP_CHG, CHG_OVERVOLTAGE);
            return;
        }
        return;
    }
    else
    {
        if (module_state.charging_stop_reason == CHG_OVERVOLTAGE)
        {
            if (chgVol < ovp_param.ovp_resume_vol)
            {

                OSI_LOGI(0, "chg: ChargeMonitorRoutine resume chgVol=%d  ", chgVol);
                _drvChargerFSMProcess(CHG_FSM_EVENT_START_CHG, 0);
            }

            return;
        }
    }

    OSI_LOGI(0, "chg: ChargeMonitorRoutine chgmng_state=%d, g_charge_plus =%d", module_state.chgmng_state, g_charge_plus);

    if (module_state.chgmng_state == CHG_IDLE)
    {
        if (module_state.charging_stop_reason == CHG_OVERTEMP)
        {
            return;
        }

        // check if need to recharge
        if (module_state.bat_statistic_vol <= chg_param.rechg_vol)
        {

            OSI_LOGI(0, "chg: ChargeMonitorRoutine recharge vot=%d  ", module_state.bat_statistic_vol);
            _drvChargerFSMProcess(CHG_FSM_EVENT_START_CHG, 0);
        }

        return;
    }
    OSI_LOGI(0, "chg: ChargeMonitorRoutine plus= %d", g_charge_plus);

#ifndef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE
    // switch positive and negative level to charge
    if (0 == g_charge_plus)
    {
        _drvChargerCurrentQueueUpdate(_drvChargerGetChgCurrent());

        _drvChargerTurnOff();
    }
    else
    {
        // module_state.bat_cur_vol = _drvChargerGetVbatVol();
        module_state.bat_cur_vol = batVol;
        OSI_LOGI(0, "chg: update batVol = %d", batVol);
        _drvChargerVbatQueueUpdate(module_state.bat_cur_vol);

        _drvChargerTurnOn();
    }
#else
    if(module_state.chgmng_state == CHG_CHARGING)
    {
        if (0 == g_charge_plus)
        {
            _drvChargerCurrentQueueUpdate(_drvChargerGetChgCurrent());

            _drvChargerTurnOff();
        }
        else
        {
            // module_state.bat_cur_vol = _drvChargerGetVbatVol();
            module_state.bat_cur_vol = batVol;
            OSI_LOGI(0, "chg: update batVol = %d", batVol);
            _drvChargerVbatQueueUpdate(module_state.bat_cur_vol);

            _drvChargerTurnOn();
        }
    }
        
#endif

#ifdef _READ_CV_STATUS_REG_
    // check cv-status
    if (_drvchargerGetCVStatus())
    {
        _CHGMNG_COUNTER_ADD_AND_AUTO_ROUND(cv_status_counter, PULSE_PERIOD);

        OSI_LOGI(0, "chg: ChargeMonitorRoutine charger full, and wating  cv_status_counter =%d ", cv_status_counter);
        if (cv_status_counter == 0)
        {
            _drvChargerFSMProcess(CHG_FSM_EVENT_STOP_CHG, CHG_TIMEOUT);
            return;
        }
    }
#else /* If the power consumption is high ,decided stop charging by soft  */

    if (module_state.bat_statistic_vol >= (chg_param.chg_end_vol - 20))
    {
        _drvChargerFSMProcess(CHG_FSM_EVENT_STOP_CHG, CHG_CHARGEDONE);
        return;
    }
#endif

#ifndef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE
    // check total charger time, set 5h charge time
    if (0 == g_charge_plus)
    {
        _CHG_COUNTER_DEC(charge_endtime_counter);
        if (charge_endtime_counter == 0)
        {
            _drvChargerFSMProcess(CHG_FSM_EVENT_STOP_CHG, CHG_TIMEOUT);
            return;
        }
    }
#else
    if(module_state.chgmng_state == CHG_CHARGING)
    {
        if (0 == g_charge_plus)
        {
            _CHG_COUNTER_DEC(charge_endtime_counter);
            if (charge_endtime_counter == 0)
            {
                _drvChargerFSMProcess(CHG_FSM_EVENT_STOP_CHG, CHG_TIMEOUT);
                return;
            }
        }
    }
#endif
}

static void _chgTaskEntry(void *param)
{
    drvChargerContext_t *p = &gDrvChargeCtx;
    p->chgTaskId = osiThreadCurrent();
    osiThreadSleep(1500); //wait for some client
    if (module_state.chgmng_state == CHG_IDLE)
    {
        if (_drvChargerIsPresent()) //If we hadn't detected charger is pluged in in interrupts, this operator can ensure charging is started.
        {
            // If in calibration mode, charger should be closed at the beginning. And then, response open and close command.
            if (_drvchargerIsCalibrationResetMode() && _drvChargerIsDownLoadMode())
            {
                _drvChargerSendMsgToChgTask(CHG_CHARGER_PESUDO_PLUG_OUT_MSG, 0);
            }
            else
            {
                _drvChargerSendMsgToChgTask(CHG_CHARGER_PLUG_IN_MSG, 0);
            }
        }
    }

    _CHG_COUNTER_INIT(warning_counter, 0);

    for (;;)
    {
        osiEvent_t event = {};
        osiEventWait(p->chgTaskId, &event);
    }
}

static void _chargerServerThreadPro(chgMsg_t *chg_msg)
{

    switch (*chg_msg)
    {
    case CHG_CHARGER_PLUG_IN_MSG:
        _drvChargerFSMProcess(CHG_FSM_EVENT_START_CHG, 0);
        break;
    case CHG_CHARGER_PLUG_OUT_MSG:
        _drvChargerFSMProcess(CHG_FSM_EVENT_STOP_CHG, CHG_CHARGERUNPLUG);
        break;
    case CHG_CHARGER_MONITOR_MSG:
        _drvChargerMonitorChargerRoutine();
        break;
    case CHG_VBAT_MONITOR_MSG:
        _drvChargerVbatMonitorRoutine();
        break;
    case CHG_CHARGER_PESUDO_PLUG_OUT_MSG:
        _drvChargerFSMProcess(CHG_FSM_EVENT_STOP_CHG, CHG_PESUDO_CHARGERUNPLUG);
        _drvChargerSetEndVolt(CHGMNG_CALI_MODE_END_VOLT);
        _drvChargerSetBatOvpVolt(CHGMNG_CALI_MODE_BAT_OVP_VOLT);
        gChgTaskReady = 1;
        break;
#ifdef BATTERY_DETECT_SUPPORT
    case CHG_BATTERY_OFF_MSG:
        _drvChargerBatteryOffRoutine();
        break;
#endif
    case CHG_IDLE_MSG:
        break;
    default:
        OSI_LOGI(0, "chg: Error msg!!!");
        break;
    }

    if (*chg_msg != CHG_IDLE_MSG)
    {
        *chg_msg = CHG_IDLE_MSG;
        OSI_LOGI(0x10007da8, "chg: chg_msg =%d, chgmng_state = %d", *chg_msg, module_state.chgmng_state);
    }
}

static void _chargerServerThread(void *ctx)
{

    drvChargerContext_t *d = (drvChargerContext_t *)ctx;

    _chargerServerThreadPro(&d->adaptor_msg);
    _chargerServerThreadPro(&d->charge_msg);
    _chargerServerThreadPro(&d->battery_msg);
}

/*****************************************************************************/
//  Description:    convert adc to Vbus voltage
/*****************************************************************************/
uint32_t _drvChargerAdcValutToChgVol(uint32_t adc)
{
    uint32_t result;

    result = drvAdcGetChannelVolt(ADC_CHANNEL_VCHGSEN, ADC_SCALE_1V250);

    OSI_LOGI(0, "_drvChargerAdcValutToChgVol result=%d", result);

    return result;
}

#ifdef BATTERY_DETECT_SUPPORT
static uint32_t _drvChargerBatteryOffRoutine()
{
    MCU_MODE_E reset_mode = NORMAL_MODE;
    if (reset_mode == NORMAL_MODE)
    {
        _drvChargerSendMsgToClient(CHR_BATTERY_OFF_IND, 0);

        if (recent_message[0] == 0)
        { ///msg is send
            isshutting_down = true;
        }
    }

    return 0;
}
#endif

/*****************************************************************************/
//  Description:    This function sets the charging parameter.
/*****************************************************************************/
static void _drvChargerSetChgParam(chgCharge_t *chg)
{
    OSI_ASSERT((chg != NULL), "chg invalid");

    chg_param = *chg;
}

static void _drvChargerSetOVPParam(chgOvpParam_t *ovp)
{
    OSI_ASSERT((ovp != NULL), "CHGMNG_SetOvpParam error"); /*assert verified*/

    ovp_param = *ovp;
}

/*****************************************************************************/
//  Description:    Set the discharge parameter.
/*****************************************************************************/
static void _drvChargerSetDischgParam(chgDischarge_t *dischg)
{
    OSI_LOGE(0, "chg: WarningVoltage Setting is too low!");
    OSI_ASSERT(dischg != NULL, "chg: dischag  is null");

    dischg_param = *dischg;
}

/*****************************************************************************/
//  Description:    Set the voltage capacity table.
/*****************************************************************************/
static void _drvChargerSetVBatCapTable(uint16_t *dischg_ptable, uint16_t *chg_ptable)
{
    uint32_t i;

    OSI_ASSERT(((dischg_ptable != NULL) && (chg_ptable != NULL)), "chg table is null");
    OSI_ASSERT(((dischg_ptable[0] > 0) && (chg_ptable[0] > 0)), "chg table too large");

    for (i = 0; i < BAT_CAPACITY_STEP; i++)
    {
        dischg_bat_capacity_table[i][0] = dischg_ptable[i];
        chg_bat_capacity_table[i][0] = chg_ptable[i];
    }

    OSI_LOGD(0, "chg: dischg_bat_capacity_table[5][0]:%d\n", dischg_bat_capacity_table[5][0]);
    OSI_LOGD(0, "chg: dischg_bat_capacity_table[6][0]:%d\n", dischg_bat_capacity_table[6][0]);
    OSI_LOGD(0, "chg: chg_bat_capacity_table[5][0]:%d\n", chg_bat_capacity_table[5][0]);
    OSI_LOGD(0, "chg: chg_bat_capacity_table[6][0]:%d\n", chg_bat_capacity_table[6][0]);
}

#ifdef CONFIG_SUPPORT_BATTERY_CHARGER
static bool _drvChgGetPowerOnForCalibrate(void)
{
    return false;
}

static bool _drvChgGetDownloaderMode(void)
{
    return false;
}
#endif

/*****************************************************************************/
//  Description:    When charger pulgin/unplug interrupt happened, this function is called.
/*****************************************************************************/
#ifdef CONFIG_SUPPORT_BATTERY_CHARGER
static void _drvChargePluginHandler(void)
{
    OSI_LOGI(0, "chg: _drvChargePluginHandler ischgmng_start=%d", ischgmng_start);

    if (!ischgmng_start)
    {
        return;
    }

    if (_drvChargerIsPresent())
    {
        // _drvChargerSetChangerCurrent(CHARGER_CURRENT_500MA);
        // If in calibration mode, charger should be closed at the beginning. And then, response open and close command.
        if (_drvChgGetPowerOnForCalibrate() && _drvChgGetDownloaderMode())
        {
            _drvAdaptorSendMsgToChgTask(CHG_CHARGER_PESUDO_PLUG_OUT_MSG, 0);
        }
        else
        {
            _drvAdaptorSendMsgToChgTask(CHG_CHARGER_PLUG_IN_MSG, 0);
        }
    }
    else
    {
        _drvAdaptorSendMsgToChgTask(CHG_CHARGER_PLUG_OUT_MSG, 0);
    }

    module_state.charging_stop_reason = CHG_INVALIDREASON; // when charge plug out ,to make sure charge can check voltage right
}
#endif

chgAdapterType_e _drvChargerCheckIdentifyAdpType(void)
{

    return CHGMNG_ADP_STANDARD;
}

/*****************************************************************************/
//  Description:    Get charge module state information.
/*****************************************************************************/
static chgStateInfo_t *_drvChargerGetModuleState(void)
{

    uint32_t critical = osiEnterCritical();
    module_state.chg_vol = _drvChgGetVol();
    module_state_to_app = module_state;
    osiExitCritical(critical);

    if (!ischgmng_start)
    {
        module_state_to_app.bat_cur_vol = _drvChargerGetVbatVol();
    }

    return &module_state_to_app;
}
static bool _drvGetBoardParm(chgParamProd_t *chr_param)
{
    return false;
}

static uint32_t _drvChgGetEffuseBits(int32_t bit_index)
{
    uint32_t val = 0;
    drvEfusePmicOpen();
    drvEfusePmicRead(bit_index, &val);
    drvEfusePmicClose();
    return val;
}
static void _drvSetChargerBoardParam(void)
{
    chgParamProd_t *chr_param = NULL;
    if (_drvGetBoardParm(chr_param) == true)
    {
        _drvChargerSetVBatCapTable(chr_param->dischg_bat_capacity_table, chr_param->chg_bat_capacity_table);
        _drvChargerSetDischgParam((chgDischarge_t *)&chr_param->dischg_param);
        _drvChargerSetChgParam((chgCharge_t *)&chr_param->chg_param);
        _drvChargerSetOVPParam((chgOvpParam_t *)&chr_param->ovp_param);
    }
}

void drvChargerGetInfo(uint8_t *nBcs, uint8_t *nBcl)
{
    if (_drvChargerIsPresent() == false)
    {
        *nBcs = 0;
    }
    else
    {
#ifndef CONFIG_QUEC_PROJECT_FEATURE
        if (module_state.chgmng_state == CHG_CHARGING)
        {
            *nBcs = 2;
        }
        else if (module_state.chgmng_state == CHG_IDLE)
        {
            *nBcs = 3;
        }
#else
        if (module_state.chgmng_state == CHG_CHARGING)
        {
            *nBcs = 1;
        }
        else if (module_state.chgmng_state == CHG_IDLE)
        {
            *nBcs = 2;
        }
#endif
    }
    if (module_state.bat_remain_cap == 0xff)
    {
        module_state.bat_remain_cap = _drvChargerVoltagePercentum(
            module_state.bat_statistic_vol,
            module_state.chgmng_state, false);
    }

    *nBcl = module_state.bat_remain_cap & 0xff;
    _drvChargerGetModuleState();
}

void drvChargerSetNoticeCB(drvChargerNoticeCB_t cb)
{
    drvChargerContext_t *p = &gDrvChargeCtx;
    p->notice_cb = cb;
}

drvChargerType_t drvChargerGetType(void)
{
    drvChargerContext_t *p = &gDrvChargeCtx;
    return (p->plugged) ? p->type : DRV_CHARGER_TYPE_NONE;
}

void drvChargerSetCB(drvChargerPlugCB_t cb, void *ctx)
{
    drvChargerContext_t *p = &gDrvChargeCtx;
    p->cb = cb;
    p->plug_cb_ctx = ctx;
    if (cb)
    {
        cb(ctx, p->plugged);
    }
}

void drvChargerInit(void)
{
    drvChargerContext_t *p = &gDrvChargeCtx;
    uint32_t cv_v;
    p->plugged = false;
    p->adaptor_msg = CHG_IDLE_MSG;
    p->charge_msg = CHG_IDLE_MSG;
    p->battery_msg = CHG_IDLE_MSG;

    REG_RDA2720M_GLOBAL_CHGR_STATUS_T chgr_status;
    OSI_LOGI(0, "chg: drvChargerInit!");

    _drvChargetPhyInit(); //Charger control initial setup
    cv_v = _drvChgGetEffuseBits(13);
    OSI_LOGI(0, "chg: get cv_v is 0x%x", cv_v);

    _drvChargerSetSwitchOverPoint(cv_v & 0x3f); //Set hardware cc-cv switch point hw_switch_point
    _drvChargerSetEndVolt(chg_param.chg_end_vol);
    _drvChargerSetChangerCurrent(CHARGER_CURRENT_700MA); // Set default current
    _drvChargerSetBatOvpVolt(chg_param.bat_safety_vol);

    //This function invalid
    _drvSetChargerBoardParam();

    p->chgTaskId = osiThreadCreate("charger", (osiCallback_t)_chgTaskEntry, NULL, CHARGER_THREAD_PRIORITY,
                                   CHARGER_THREAD_STACK_SIZE, CHARGER_THREAD_MAX_EVENT);
#ifdef CONFIG_SUPPORT_BATTERY_CHARGER
    p->chgMonTimer = osiTimerCreate(NULL, _drvChargerTimerHandler, &module_state.chgmng_state);

    p->batMonTimer = osiTimerCreate(NULL, (osiCallback_t)_drvChargerVbatTimerHandler, NULL);
    osiTimerStartRelaxed(p->batMonTimer, CHG_BAT_MONITOR_PERIOD_, OSI_WAIT_FOREVER);
#else
    _drvChargerTurnOff();
#endif

    _drvChargerFSMProcess(CHG_FSM_EVENT_INIT, 0);

    if (0 == _drvChargerCheckVbatPresent())
    {
        _drvChargerTurnOff();
    }

    module_state.bat_cur_vol = _drvChargerGetVbatVol();

    OSI_LOGI(0, "chg: drvChargerInit bat_cur_vol =%d", module_state.bat_cur_vol);
    _drvChargerVbatQueueInit(module_state.bat_cur_vol, DISCHG_VBAT_BUFF_SIZE); //init statistic program

    if (module_state.bat_cur_vol < dischg_param.shutdown_vol)
    {
        if (!_drvChargerIsPresent())
        {
            //PowerOff();
            OSI_LOGI(0, "chg: power down because low battery volt");
        }
    }

    ischgmng_start = true; //Charge management has started.
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_status,
        REG_FIELD_MASKVAL2(chgr_status, chgr_int_en, 1, dcp_switch_en, 1),
        HAL_ADI_BUS_CHANGE_END);
    drvPmicEicSetCB(DRV_PMIC_EIC_CHGR_INT, _chargeIsrCB, NULL);
    drvPmicEicTrigger(DRV_PMIC_EIC_CHGR_INT, CHARGER_DEBOUNCE, true);
    _updateChargerStatus(p);

    OSI_LOGI(0, "chg: drvChargerInit end");
}

void drvChargeEnable(void)
{
    charge_stop = false;
    _drvChargerTurnOn();
}
void drvChargeDisable(void)
{
    charge_stop = true;
    _drvChargerTurnOff();
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_CHARGE
uint32_t quec_get_vchg_vol()
{
    return _drvChgGetVol();
}

uint32_t quec_get_vbat_vol()
{
    return _drvChargerGetVbatVol();
}

uint32_t quec_get_bat_capicity()
{
    return module_state.bat_remain_cap;
}

uint32_t quec_get_vbat_cur()
{
    return _drvChargerGetChgCurrent();
}

uint32_t quec_get_chg_status()
{
    return module_state.chgmng_state;
}

bool quec_get_charger_state()
{
    bool charger_state = _drvChargerIsPresent();    
    OSI_LOGI(0,"chg:charger state = %d", charger_state);
    
    return charger_state;
}

uint32_t quec_get_battery_temp()
{
    uint32_t battery_temp;
    if(battery_detect)
    {
        if(battery_detect_channel < 5 && battery_detect_channel > 1)
        {
            battery_temp = _drvChargerGetChannelVol(battery_detect_channel, ADC_SCALE_MAX);
    
            OSI_LOGI(0,"chg:battery temperature vol = %d", battery_temp);
    
            return battery_temp;
        }
        else 
        {
            return 0;
        }        
    }
    else 
    {
        return 0;
    } 
}

#define BATTERY_DETECT_THRESHOLD 1700
bool battery_connect = false;

bool quec_get_battery_state()
{
    uint32_t battery_temp;

    static uint32_t time_count = 10000;
    
    if(battery_detect)
    {    
        battery_temp = _drvChargerGetChannelVol(battery_detect_channel, ADC_SCALE_MAX);
        OSI_LOGI(0,"chg:battery channel = %d, battery temperature vol = %d",battery_detect_channel, battery_temp);
    
        if(battery_temp < BATTERY_DETECT_THRESHOLD)
        {
            if(!battery_connect)
            {
                time_count--;
                OSI_LOGI(0,"chg:battery connect start timer count: %d!", (1000-time_count));
                osiTimerStartRelaxed(gDrvChargeCtx.batMonTimer, CHG_BAT_MONITOR_PERIOD_, OSI_WAIT_FOREVER);
            }
            battery_connect = true;
            OSI_LOGI(0,"chg:battery connected!");
            return true;
        }
        else
        {
            battery_connect = false;
            OSI_LOGI(0,"chg:battery disconnected!");
            return false;
        }
    }
    else 
    {
        uint32_t batdet_ok = _drvChargerIsBatDetOk();
        return 1;
        return batdet_ok;
    } 
}

#endif