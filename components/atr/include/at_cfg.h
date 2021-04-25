/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _AT_CFG_H_
#define _AT_CFG_H_

#include <stdint.h>
#include <stdbool.h>
#include "cfw_config.h"
#include "atr_config.h"
#include "ats_config.h"
#include "osi_compiler.h"
#include "time.h"

#define AT_ALARM OSI_MAKE_TAG('A', 'T', 'A', 'L')

// ///////////////////////////////////////////////////////////////////////
// Gather definition from other ats module
// ///////////////////////////////////////////////////////////////////////

#define AT_GPRS_APN_MAX_LEN 99
#define AT_GPRS_PDPADDR_MAX_LEN 21
#define AT_GPRS_PDPTYPE_MAX_LEN 50
#define AT_GPRS_USR_MAX_LEN 64
#define AT_GPRS_PAS_MAX_LEN 64

#define AT_PDPCID_MIN 1
#define AT_PDPCID_MAX 7

typedef enum
{
    cs_gsm,
    cs_hex,
    cs_ucs2,
    cs_gbk, // equals to PCCP 936 char set
    cs_ira,
    cs_COUNT_,
    cs_INVALID_ = 0x8F,
} at_chset_t;

typedef enum
{
    AT_IND_BATTCHG, // [0,5]
    AT_IND_SIGNAL,  // [0,5]
    AT_IND_SERVICE, // [0,1]
    AT_IND_SOUNDER, // [0,1]
    AT_IND_MESSAGE, // [0,1]
    AT_IND_CALL,    // [0,1]
    AT_IND_ROAM,    // [0,1]
    AT_IND_SMSFULL, // [0,1]

    AT_IND_COUNT
} atIndicator_t;

typedef struct _AT_Gprs_CidInfo
{
    uint8_t uCid;
    uint8_t nDLCI;
    uint8_t uStateStr[15];
    uint8_t uState;

    // uint8_t   uAutoRspState;
    uint8_t nPdpType;
    uint8_t nDComp;
    uint8_t nHComp;
    uint8_t pApn[AT_GPRS_APN_MAX_LEN];
    uint8_t pPdpAddr[AT_GPRS_PDPADDR_MAX_LEN];
    uint8_t nApnSize;
    uint8_t nAuthProt;
    uint8_t nPdpAddrSize;

    uint8_t uaUsername[AT_GPRS_USR_MAX_LEN];
    uint8_t nUsernameSize;
    uint8_t uaPassword[AT_GPRS_PAS_MAX_LEN];
    uint8_t nPasswordSize;
    uint8_t szOperatorName[6 + 1];

#ifdef LTE_NBIOT_SUPPORT
    uint8_t uPCid; /*  */
    uint8_t apnacAer;
    uint8_t apnacUnit;
    uint32_t apnacRate;
#endif
} AT_Gprs_CidInfo;

typedef enum
{
    GC_RESULTCODE_PRESENTATION_ENABLE = 0,
    GC_RESULTCODE_PRESENTATION_DISABLE
} GC_RESULTCODE_PRESENTATION;

typedef enum
{
    GC_RESULTCODE_FORMAT_LIMITED = 0,
    GC_RESULTCODE_FORMAT_VERBOSE
} GC_RESULTCODE_FORMAT;

typedef enum
{
    GC_COMMANDECHO_MODE_DISABLE = 0,
    GC_COMMANDECHO_MODE_ENABLE
} GC_COMMANDECHO_MODE;

typedef struct
{
    uint16_t ind_ctrl_mask;  // indicator control mask
    uint16_t sind_ctrl_mask; // system indicator control mask, for ^SIND command
    at_chset_t te_chset;
    at_chset_t mt_chset;
} cfg_info_t;

typedef enum
{
    PBK_CHARACTER_SET_TRANSPARENT = 0,
    PBK_CHARACTER_SET_HEX
} PBK_CHARACTER_SET;

typedef enum
{
    PBK_LISTENTRY_CALLER_CPBR = 0,
    PBK_LISTENTRY_CALLER_CPBF,
    PBK_LISTENTRY_CALLER_CNUM
} PBK_LISTENTRY_CALLER;

typedef struct
{
    uint8_t nMode;
    uint8_t nMt;
    uint8_t nBm;
    uint8_t nDs;
    uint8_t nBfr;
} AT_CNMI;

enum AT_DTR_DATA_BEHAVIOR
{
    AT_DTR_DATA_IGNORE,
    AT_DTR_DATA_ESCAPE,
    AT_DTR_DATA_END
};

typedef struct
{
    AT_CNMI sCnmi;
    uint8_t nStorage1;
    uint8_t nStorage2;
    uint8_t nStorage3;

    uint8_t nTotal1;
    uint8_t nTotal2;
    uint8_t nTotal3;

    uint8_t nUsed1;
    uint8_t nUsed2;
    uint8_t nUsed3;

    uint8_t nServType;
    uint8_t nOverflowCtrl;
    uint8_t nOverflowMode;
} at_sms_info_t;

#define NW_PREFERRED_OPERATOR_FORMAT_NUMERIC 2
#define NW_GETPREFERREDOPERATORS_CALLER_READ 0
//#define GSM_SET 0
//#define UCS2_SET 1
#define ALPHA_SET 2
#define HEX_SET 3

#define INSTANCE 0
#define NONE_INSTANCE 1

#define AT_AUDIO_TEST_HANDSET 0
// 耳机
#define AT_AUDIO_TEST_EARPIECE 1
// 听筒
#define AT_AUDIO_TEST_LOUDSPEAKER 2
// 免提

// ///////////////////////////////////////////////////////////////////////////
// New at global variables
// ///////////////////////////////////////////////////////////////////////////
// sms related settings
typedef struct at_sms_setting
{
    uint8_t csmpFo;
    uint8_t csdhShow;                            // show header
    at_sms_info_t smsInfo[CONFIG_NUMBER_OF_SIM]; // CPMS命令相关的存储器信息
    uint8_t smsFormat;                           // SMS Foramt: 0: PDU 1:text
} at_sms_setting_t;

typedef enum
{
    OPERATOR_CMCC = 0,
    OPERATOR_UNICOM,
    OPERATOR_CTCC
} OPERATOR_NAME;

typedef struct at_tcpip_setting
{
    uint8_t cipSCONT_unSaved;   //AT+CIPSCONT Save TCPIP Application Context
    uint8_t cipMux_multiIp;     //AT+CIPMUX Start Up Multi-IP Connection
    uint8_t cipHEAD_addIpHead;  //AT+CIPHEAD Add an IP Head at the Beginning of a Package Received
    uint8_t cipSHOWTP_dispTP;   //AT+CIPSHOWTP Display Transfer Protocol in IP Head When Received Data
    uint8_t cipSRIP_showIPPort; //AT+CIPSRIP Show Remote IP Address and Port When Received Data
    uint8_t cipATS_setTimer;    //AT+CIPATS Set Auto Sending Timer
    uint8_t cipATS_time;
    uint8_t cipSPRT_sendPrompt;  //AT+CIPSPRT Set Prompt of > When Module Sends Data
    uint8_t cipQSEND_quickSend;  //AT+CIPQSEND Select Data Transmitting Mode
    uint8_t cipMODE_transParent; //AT+CIPMODE Select TCPIP Application Mode
    uint8_t cipCCFG_NmRetry;     //AT+CIPCCFG Configure Transparent Transfer Mode
    uint8_t cipCCFG_WaitTm;
    uint16_t cipCCFG_SendSz;
    uint8_t cipCCFG_esc;
    uint8_t cipCCFG_Rxmode;
    uint16_t cipCCFG_RxSize;
    uint16_t cipCCFG_Rxtimer;
    uint8_t cipDPDP_detectPDP; //AT+CIPDPDP Set Whether to Check State of GPRS Network Timing
    uint8_t cipDPDP_interval;
    uint8_t cipDPDP_timer;
    uint8_t cipCSGP_setGprs;        //AT+CIPCSGP Set CSD or GPRS for Connection Mode
    uint16_t cipRDTIMER_rdsigtimer; //AT+CIPRDTIMER Set Remote Delay Timer
    uint16_t cipRDTIMER_rdmuxtimer;
    uint8_t cipRXGET_manualy; //AT+CIPRXGET Get Data from Network Manually
    uint8_t cipApn[51];
    uint8_t cipUserName[51];
    uint8_t cipPassWord[51];
    uint8_t qishowla_showlocaddr;
    uint8_t qidnsip_dnsip;
} at_tcpip_setting_t;

// ///////////////////////////////////////////////////////////////////////////
// Macro definition of configuration setttings
// ///////////////////////////////////////////////////////////////////////////
// Flag of switching AT current settings
#define MANUFACTURER_DEFALUT_SETING 0
#define USER_SETTING_1 1

// /////////////////////////////////////////////////////////////////////////////////////////
// Macro to facilitate the use of Current AT settings
// /////////////////////////////////////////////////////////////////////////////////////////
// gc related
#define gATCurrentCSCLK gAtCurrentSetting.csclk
#define gATCurrenteResultcodePresentation gAtCurrentSetting.resultCodePresent
#define gATCurrenteResultcodeFormat gAtCurrentSetting.resultCodeFormat
#define gATCurrentnS3 gAtCurrentSetting.nS3
#define gATCurrentnS4 gAtCurrentSetting.nS4
#define gATCurrentnS5 gAtCurrentSetting.nS5
#define gATCurrentCgreg gAtCurrentSetting.cgreg
#ifdef LTE_NBIOT_SUPPORT
#define gATCurrentCereg gAtCurrentSetting.cereg
#endif
#define gATCurrentCrc gAtCurrentSetting.crc
#define gATCurrentRxfc gAtCurrentSetting.rxfc
#define gATCurrentTxfc gAtCurrentSetting.txfc
#define gATCurrentDtrData gAtCurrentSetting.dtrData
#define gATCurrentIcfFormat gAtCurrentSetting.icfFormat
#define gATCurrentIcfParity gAtCurrentSetting.icfParity
#define gATCurrentSecUartEnable gAtCurrentSetting.secUartEnable
#define gAtCurrentSecUartBaud gAtCurrentSetting.secUartBaud
#define gAtCurrentSecUartIcfFormat gAtCurrentSetting.secUartIcfFormat
#define gAtCurrentSecUartIcfParity gAtCurrentSetting.secUartIcfParity
#define gATCurrentuCmer_ind gAtCurrentSetting.cmerInd
// TODO..
#define gATCurrentCfgInfoInd_ctrl_mask gAtCurrentSetting.cfgInfo.ind_ctrl_mask
#define gATCurrentCfgInfoTe_chset gAtCurrentSetting.cfgInfo.te_chset
#define gATCurrentCfgInfoMt_chset gAtCurrentSetting.cfgInfo.mt_chset

// cc related
#define gATCurrentcc_VTD gAtCurrentSetting.cc_VTD
#define gATCurrentcc_s0 gAtCurrentSetting.cc_s0

// nw related
#define gATCurrentOperFormat gAtAutoSaveSetting.operFormat
#define gATCurrentCOPS_Mode_Temp gAtAutoSaveSetting.copsModeTemp
#define gATCurrentnPreferredOperFormat gAtAutoSaveSetting.preferredOperFormat
#define gATCurrentpArrPreferredOperater gAtAutoSaveSetting.preferredOperator
#define gATCurrentnPreferredOperatorIndex gAtAutoSaveSetting.preferredOperatorIndex
#define gATCurrentnGetPreferredOperatorsCaller gAtAutoSaveSetting.preferredOperatorCaller
#define gATCurrentForcedLpCtrlMode gAtAutoSaveSetting.forcedLpCtrlMode
#ifdef LTE_NBIOT_SUPPORT
#define gATCurrentCscon gAtCurrentSetting.cscon
#define gATCurrentNwEdrxPtw gAtCurrentSetting.edrxPtw
#define gATCurrentNwEdrxValue gAtCurrentSetting.edrxValue
#endif

// pbk related
#define gATCurrentpbkListEntryCaller gAtCurrentSetting.pbkListEntryCaller
#define gATCurrentpbkCharacterSet gAtCurrentSetting.pbkCharacterSet
#define gATCurrentnTempStorage gAtCurrentSetting.tempStorage

// sat related
#define gATCurrentAlphabet gAtCurrentSetting.alphabet

// sms related
#define gATCurrentAtSmsSettingSCSMP_fo gAtCurrentSetting.smsSetting.csmpFo
#define gATCurrentAtSmsSettingSCSDH_show gAtCurrentSetting.smsSetting.csdhShow
#define gATCurrentAtSmsSettingSg_SMSFormat gAtCurrentSetting.smsSetting.smsFormat
#define gATCurrentAtSMSInfo gAtCurrentSetting.smsSetting.smsInfo

// ss related
#define gATCurrentucSsCallWaitingPresentResultCode gAtCurrentSetting.callWaitingPresentResultCode
#define gATCurrentss_ussd gAtCurrentSetting.ussd
#define gATCurrentucClip gAtCurrentSetting.ucClip
#define gATCurrentucClir gAtCurrentSetting.ucClir
#define gATCurrentucColp gAtCurrentSetting.ucColp
#define gATCurrentucCSSU gAtCurrentSetting.ucCSSU
#define gATCurrentucCSSI gAtCurrentSetting.ucCSSI

// special related
#define gATCurrentu8nMONIPara gAtCurrentSetting.moniPara
#define gATCurrentu8nMONPPara gAtCurrentSetting.monpPara
#define gATCurrentnPMIndMark gAtCurrentSetting.pmIndMark
#define gATCurrentVGR_Volume gAtCurrentSetting.vgrVolume
#define gATCurrentVGT_Volume gAtCurrentSetting.vgtVolume
#define gATCurrentCMUT_mute gAtCurrentSetting.cmutMute
#define gATCurrentnAudioTestMode gAtCurrentSetting.audioTestMode

// gprs related
#define gATCurrentu8nURCAuto gAtSetting.gprsAuto

// added by yangtt at 04-18
#define gATCurrentu32nBaudRate gAtCurrentSetting.baudRate

// added by wy at 2017-11-09
#if defined(AT_MODULE_SUPPORT_OTA)
#define gATOtaDataconnProfile gAtAutoSaveSetting.g_OTA_dataconnProfile
#endif
#define g_staAtGprsCidInfo_e gAtSetting.g_staAtGprsCidInfo

#define gATCurrentRAIMode gAtCurrentSetting.RAI_mode

typedef struct
{
    bool atv;
    bool ate;
    bool atq;
    uint8_t atx; // not used now
    bool crc;
    uint8_t s3;
    uint8_t s4;
    uint8_t s5;
    uint8_t cmee;
    uint8_t andc;
    uint8_t andd;
} atChannelSetting_t;

typedef struct
{
    uint8_t creg;
    uint8_t cgreg;
    uint8_t cereg;
    uint8_t cscon;
    uint8_t cgerep_mode;
    uint8_t cgerep_bfr;
    uint8_t clip;
    uint8_t clir; // NO_SAVE
    uint8_t colp; // NO_SAVE
    uint8_t cssu;
    uint8_t cssi;
    uint8_t ccwa;
    uint8_t cgauto;
    uint8_t cr;
    uint8_t s0;
    uint8_t ecsq;
    uint8_t csdh;
    uint8_t cmgf;
    uint8_t cpms_mem1;
    uint8_t cpms_mem2;
    uint8_t cpms_mem3;
    uint8_t cnmi_mode;
    uint8_t cnmi_mt;
    uint8_t cnmi_bm;
    uint8_t cnmi_ds;
    uint8_t cnmi_bfr;
    uint8_t cind_signal;
    uint8_t cind_service;
    uint8_t cind_sounder;
    uint8_t cind_message;
    uint8_t cind_call;
    uint8_t cind_roam;
    uint8_t cind_smsfull;
    uint8_t alphabet;     // sta_cs;
    uint8_t cops_format;  // AUTO_SAVE
    uint8_t cops_mode;    // AUTO_SAVE
    uint8_t cops_oper[6]; // AUTO_SAVE
    uint8_t cpol_format;  // AUTO_SAVE
    uint8_t cpol_index;   // AUTO_SAVE
    uint8_t cpol_oper[6]; // AUTO_SAVE
    uint8_t cops_act;     // AUTO_SAVE
    uint8_t volte;
    uint8_t flight_mode; // AUTO_SAVE
    uint32_t cfgNvFlag;
} atSimSetting_t;

// &W_SAVE
typedef struct
{
    uint8_t profile;
    uint8_t ifc_rx;
    uint8_t ifc_tx;
    uint8_t icf_format;
    uint8_t icf_parity;
    uint32_t ipr;

    uint8_t cmer_mode;
    uint8_t cmer_ind;
    uint8_t csclk;
    uint8_t cscs;
    uint8_t vtd;
    uint8_t ctzu;
    uint8_t ctzr;
    uint8_t csdf_mode;
    uint8_t csdf_auxmode;
    uint8_t cind_battchg;
    int8_t timezone;                                          // AUTO_SAVE
    uint8_t timezone_dst;                                     // AUTO_SAVE
    uint8_t bcchmode;                                         // AUTO_SAVE
    AT_Gprs_CidInfo g_staAtGprsCidInfo[4][AT_PDPCID_MAX + 1]; // AUTO_SAVE

    uint8_t gprsAuto;
    uint8_t tempStorage;
    uint8_t drx_mode;
    uint8_t callmode;
    uint8_t smsmode;
    atChannelSetting_t chsetting;
    atSimSetting_t sim[CONFIG_NUMBER_OF_SIM];
    uint8_t self_register;
    uint8_t psm_mode;
    uint8_t mcuNotifySleepMode;
    uint32_t mcuNotifySleepDelayMs;
    uint8_t csta;
    uint8_t csvm;
    uint8_t detectMBS;
    uint8_t virtual_sim_card[CONFIG_NUMBER_OF_SIM];
    uint8_t CgClassType[CONFIG_NUMBER_OF_SIM];
    uint8_t poc_user_sim;
} atSetting_t;

extern atSetting_t gAtSetting;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
extern atSetting_t gAtSettingSave;
#endif


/**
 * global variable to be used in core engine
 *
 * These variables are used in core engine. To avoid multiple
 * version of core libraries, the variable rather than macros
 * are used in core library.
 */
struct atCmdDesc;
extern const bool gAtEchoCommandOnly;
extern const bool gAtCmdTplusEnable;
extern const struct atCmdDesc gAtTplusCmdDesc;
extern const bool gAtUrcBuffEnable;
extern const unsigned gAtUrcBuffSize;
extern const unsigned gAtUrcBuffCount;
extern const unsigned gAtCmdlineMax;
extern const unsigned gAtLfWaitMs;
extern const unsigned gAtCmuxOutBuffSize;
extern const unsigned gAtCmuxInBuffSize;
extern const unsigned gAtCmuxSubMinInBuffSize;
extern const unsigned gAtCmuxDlcNum;
extern const unsigned gAtDataBuffSize;
extern const unsigned gAtPppEndCheckTimeout;
extern const unsigned gAtCmdWorkerStackSize;

void atCfgInit(void);
bool atCfgSave(uint8_t profile, const atChannelSetting_t *chsetting);
bool atCfgAutoSave(void);
bool atCfgGlobalSave(void);
bool atCfgResetFactory(atChannelSetting_t *chsetting);
bool atCfgResetProfile(uint8_t profile, atChannelSetting_t *chsetting);

#endif
