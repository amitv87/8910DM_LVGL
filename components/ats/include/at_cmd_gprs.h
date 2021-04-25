#include "ats_config.h"

#define CONFIG_AT_GPRS_SUPPORT

#ifdef CONFIG_AT_GPRS_SUPPORT

#include "at_engine.h"
#include "at_command.h"
#include "at_apn_table.h"

typedef struct
{
    uint8_t value[40];
    uint8_t rsrp_flag25;
    uint8_t rsrp25;
    uint8_t rsrq_flag25;
    uint8_t rsrq25;
    uint8_t rsrp_flag26;
    uint8_t rsrp26;
    uint8_t rsrq_flag26;
    uint8_t rsrq26;
    uint16_t band31;
    uint16_t band32;
    uint8_t nArfcn31;
    uint8_t nArfcn32;
    uint16_t arfcn31[3];
    uint16_t arfcn32[3];
    uint8_t nArfcn36;
    uint32_t arfcn36[8];
    uint16_t band38;
    uint16_t arfcn38[3];
} SPLTEDUMMYPARA_T;
typedef struct
{
    uint8_t nEnable[15];
    uint16_t nValue1_2;
    uint16_t nValue1_3;
    uint16_t nValue2_3;
    uint16_t nValue1_5;
    uint16_t nValue1_6;
    uint16_t nValue1_7;
    uint16_t nValue1_8;
    uint16_t nValue1_9;
    uint16_t nValue1_10;
    uint16_t nValue1_11;
    uint16_t nValue1_12;
    uint16_t nValue2_12;
    uint16_t nValue1_13;
    uint16_t nValue1_14;
} RRTPARAM_T;

typedef enum
{
    AUTO_RSP_STU_OFF_PDONLY,
    AUTO_RSP_STU_ON_PDONLY,
    AUTO_RSP_STU_MOD_CAP_PDONLY,
    AUTO_RSP_STU_MOD_CAP_PD_CS,
    AUTO_RSP_STATUS_MAX,
} AT_GPRS_AUTO_RSP;

#define DUMMY_VALUE_OUT_RANGE(value, min, max) \
    if (value < min || value > max)            \
        return ERR_INVALID_PARAMETER;

#ifdef LTE_NBIOT_SUPPORT
#ifdef CONFIG_SOC_8910
uint8_t CFW_nvGetPsmEnable(uint8_t nSimID);
void CFW_nvSetPsmEnable(uint8_t psmEnable, uint8_t nSimID);
uint8_t CFW_nvGetPsmT3412(uint8_t nSimID);
void CFW_nvSetPsmT3412(uint8_t t3412Val, uint8_t nSimID);
uint8_t CFW_nvGetPsmT3324(uint8_t nSimID);
void CFW_nvSetPsmT3324(uint8_t t3324Val, uint8_t nSimID);
uint8_t CFW_nvGetEdrxEnable(uint8_t nSimID);
void CFW_nvSetEdrxEnable(uint8_t enable, uint8_t nSimID);
uint8_t CFW_nvGetEdrxValue(uint8_t nSimID);
void CFW_nvSetEdrxValue(uint8_t value, uint8_t nSimID);
uint8_t CFW_nvGetEdrxPtw(uint8_t nSimID);
void CFW_nvSetEdrxPtw(uint8_t ptw, uint8_t nSimID);
uint32_t CFW_GprsSetEdrx(uint8_t edrxEnable, uint8_t edrxType, uint8_t edrxValue, uint8_t nSimID);
void CFW_nvSetCiotNonip(uint8_t nonip, uint8_t nSimID);
void CFW_nvSetCiotCpciot(uint8_t cpciot, uint8_t nSimID);
void CFW_nvSetCiotUpciot(uint8_t upciot, uint8_t nSimID);
void CFW_nvSetCiotErwopdn(uint8_t erwopdn, uint8_t nSimID);
void CFW_nvSetCiotSmsWoCombAtt(uint8_t sms_wo_comb_att, uint8_t nSimID);
void CFW_nvSetCiotApnRateCtrl(uint8_t apn_rate_control, uint8_t nSimID);
uint8_t CFW_nvGetCiotNonip(uint8_t nSimID);
uint8_t CFW_nvGetCiotCpciot(uint8_t nSimID);
uint8_t CFW_nvGetCiotUpciot(uint8_t nSimID);
uint8_t CFW_nvGetCiotErwopdn(uint8_t nSimID);
uint8_t CFW_nvGetCiotSmsWoCombAtt(uint8_t nSimID);
uint8_t CFW_nvGetCiotApnRateCtrl(uint8_t nSimID);
#endif

uint32_t CFW_GprsSetReqQosUmts(uint8_t nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID);
uint32_t CFW_GprsGetReqQosUmts(uint8_t nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID);
uint8_t _ceregRespond(CFW_NW_STATUS_INFO *sStatus, uint8_t stat, bool reportN, atCommand_t *cmd);
bool CFW_GprsSendDataAvaliable(uint8_t nSimID);
uint8_t CFW_GprsGetSmsSeviceMode(void);
uint8_t CFW_GprsSetSmsSeviceMode(uint8_t nService);
uint32_t CFW_EmodSpCleanInfo(CFW_EmodSpCleanInfo_t para, CFW_SIM_ID nSimID);
uint32_t CFW_EmodL1Param(CFW_EmodL1Param_t para, CFW_SIM_ID nSimID);
uint32_t CFW_EmodRrtmParam(CFW_EmodRrtmParam_t para, CFW_SIM_ID nSimID);
uint32_t CFW_EmodSpLteDummyPara(CFW_EmodSpLteDummyPara_t para, CFW_SIM_ID nSimID);
uint32_t CFW_EmodGrrLteFreq(CFW_EmodGrrLteFreq_t para, CFW_SIM_ID nSimID);

#endif
#endif
