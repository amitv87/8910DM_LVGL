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

#include "at_cfg.h"
#include "cfw.h"
#include "atr_config.h"
#include "quec_proj_config.h"
#include "quec_cust_feature.h"


// /////////////////////////////////////////////////////////////////////////////////////
// At default setttings (Manufacture specific)
// /////////////////////////////////////////////////////////////////////////////////////
#define AT_ASCII_CR 13
#define AT_ASCII_LF 10
#define AT_ASCII_BS 8
#define RECEIVE_GAIN 6
#define TRANSMIT_GAIN 1
// 10
#define IND_CTRL_MASK 0
#define TE_CHEST cs_gbk
#define MT_CHSET cs_gbk
#define DISABLE_CMEE_ERROR 1
#define DURATION_OF_TONE 1
#define OPER_FORMAT_NUMERIC 2
#define AUTOMATIC_REGISTER 0
#define MONP_DISPLAY_PERIOD 10
#define MONI_DISPLAY_PERIOD 10
#define DISABLE_UPLINK_VOICE 0
#define OPERATOR_NUMBER_ORDER 100
// Random value
#define NO_INDICATOR_EVENT_REPORT 0
#define DISABLE_AUTOMATIC_ANSWERING 0
#define PHONEBOOK_MEMORY_STORAGE CFW_PBK_SIM
#define CLIR_PRESENTION_INDICATOR 0
#define DISABLE_CSSU_RESULT_CODE 0
#define DISABLE_CSSI_RESULT_CODE 0
#define DISABLE_RESULT_CODE_PRESENTION_STATUS 0

#if defined(AT_MODULE_SUPPORT_OTA)
#define AT_BEARER_TYPE_DEFUALT_VAL AT_E_DC_BEARER_CSD
#define AT_CONNECTION_MODE_DEFUALT_VAL AT_E_DC_CONNECTION_ORIENTED
#define AT_GPRS_AUTH_TYPE_DEFUALT_VAL AT_GPRS_AUTH_PAP
#define AT_PROFILE_TYPE_DEFUALT_VAL AT_E_PROF_BROWSER
#endif

const at_tcpip_setting_t gDefaultCIPSettings = {
    .cipSCONT_unSaved = 1,
    .cipMux_multiIp = 0,
    .cipHEAD_addIpHead = 0,
    .cipSHOWTP_dispTP = 0,
    .cipSRIP_showIPPort = 0,
    .cipATS_setTimer = 0,
    .cipATS_time = 1,
    .cipSPRT_sendPrompt = 1,
    .cipQSEND_quickSend = 0,
    .cipMODE_transParent = 0,
    .cipCCFG_NmRetry = 5,
    .cipCCFG_WaitTm = 2,
    .cipCCFG_SendSz = 1024,
    .cipCCFG_esc = 1,
    .cipCCFG_Rxmode = 0,
    .cipCCFG_RxSize = 1460,
    .cipCCFG_Rxtimer = 50,
    .cipDPDP_detectPDP = 0,
    .cipDPDP_interval = 0,
    .cipDPDP_timer = 3,
    .cipCSGP_setGprs = 1,
    .cipRDTIMER_rdsigtimer = 2000,
    .cipRDTIMER_rdmuxtimer = 3500,
    .cipRXGET_manualy = 0,
    .cipApn = {0},
    .cipUserName = {0},
    .cipPassWord = {0},
};

const atSetting_t gAtDefaultSetting = {
    .profile = 0,
    .ifc_rx = 0,
    .ifc_tx = 0,
    .icf_format = 3, // 8N1
    .icf_parity = 0,
#ifdef __QUEC_OEM_VER_LD__
	.ipr = 460800,
#else
    .ipr = CONFIG_ATR_DEFAULT_UART_BAUD,
#endif
    .cmer_ind = 0,
    .csclk = 0, // disbable sleep
#ifdef CONFIG_QUEC_PROJECT_FEATURE
	.cscs = cs_gsm,
#else
	.cscs = cs_gbk,
#endif
    .vtd = 1,
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
	.ctzu = 0,
    .ctzr = 0,
#else
	.ctzu = 1,
    .ctzr = 1,
#endif    
    .csdf_mode = 1,
    .csdf_auxmode = 1,
    .timezone = 0,
    .timezone_dst = 0,
    .bcchmode = 0,
    .gprsAuto = 3,
    .tempStorage = CFW_PBK_SIM,
    .drx_mode = 0,
    .callmode = 0,
    .smsmode = 0,
    .chsetting = {
        .atv = 1,
        .ate = 1,
        .atq = 0,
        .atx = 4,
        .crc = 0,
        .s3 = 0x0d,
        .s4 = 0x0a,
        .s5 = 0x08,
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        .cmee = 1,
        .andc = 1,
        .andd = 2,
#else
        .cmee = 0,
        .andc = 0,
        .andd = 1,
#endif        
    },

    .sim = {
        [0 ...(CONFIG_NUMBER_OF_SIM - 1)] = {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
            .creg = 0,
            .cgreg = 0,
            .cereg = 0,
            .cscon = 0,
            .cgerep_mode = 0,
#else
            .creg = 1,
            .cgreg = 1,
            .cereg = 1,
            .cscon = 1,
            .cgerep_mode = 1,
#endif
            .cgerep_bfr = 0,
            .clip = 0,
            .clir = 0,
            .colp = 0,
            .cssu = 0,
            .cssi = 0,
            .ccwa = 0,
            .cgauto = 3,
            .cr = 0,
            .s0 = 0,
            .ecsq = 0,
            .csdh = 0,
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
			.cmgf = 0,
			.cpms_mem1 = 1, // ME
			.cpms_mem2 = 1, // ME
			.cpms_mem3 = 1, // ME
			.cnmi_mode = 2,
			.cnmi_mt = 1,
#else
			.cmgf = 1,
			.cpms_mem1 = 2, // SM
			.cpms_mem2 = 2, // SM
			.cpms_mem3 = 2, // SM
			.cnmi_mode = 0,
			.cnmi_mt = 0,
#endif
            .cnmi_bm = 0,
            .cnmi_ds = 0,
            .cnmi_bfr = 0,
            .alphabet = 0,
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
            .cops_format = 0,
#else
            .cops_format = 2,
#endif
            .cops_mode = 0,
            .cops_oper = {},
            .cpol_format = 0,
            .cpol_index = 0,
            .cind_message = 1,
            .cpol_oper = {},
            .cops_act = 7,
            .volte = 0,
            .flight_mode = 1,
            .cfgNvFlag = 0,
        },
    },

    .g_staAtGprsCidInfo = {
        {
            {
                1,
                .nAuthProt = 1,
            },
        },
    },

    .self_register = 0,
    .psm_mode = 0,
    .mcuNotifySleepMode = 0,
    .mcuNotifySleepDelayMs = 120,
    .csta = CFW_TELNUMBER_TYPE_UNKNOWN,
    .csvm = 0,
    .detectMBS = 0,
    .virtual_sim_card = {[0 ...(CONFIG_NUMBER_OF_SIM - 1)] = 0},
    .CgClassType = {[0 ...(CONFIG_NUMBER_OF_SIM - 1)] = 0},
    .poc_user_sim = 0xFF,
};
