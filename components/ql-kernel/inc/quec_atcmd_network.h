/**  @file
  quec_atcmd_network.h

  @brief
  This file is used to define the network at command handler for different Quectel Project.

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
#ifndef _QUEC_ATCMD_NETWORK_H
#define _QUEC_ATCMD_NETWORK_H

#include "at_cfw.h"
#include "quec_common.h"
#include "quec_rtos.h"
#include "ql_api_osi.h"

#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW


/*========================================================================
 *	Macro Definition
 *========================================================================*/


/*========================================================================
 *	Enumeration Definition
 *========================================================================*/
typedef enum
{
    QUEC_AUTO  = 0,			/*  AUTO 	*/
    QUEC_GSM ,    			/*  GSM 	*/
    QUEC_WCDMA, 	    	/*  WCDMA 	*/
    QUEC_LTE,				/*  LTE 	*/
}quec_nw_scanmode_type_e;

typedef enum 
{
    QUEC_BAND_GSM_900=0x01 ,
    QUEC_BAND_GSM_1800=0x02,
    QUEC_BAND_GSM_850=0x04,
    QUEC_BAND_GSM_1900=0x08,
 
    QUEC_BAND_LTE_BAND1=0x01,
    QUEC_BAND_LTE_BAND3=0x04,
    QUEC_BAND_LTE_BAND5=0x10,
    QUEC_BAND_LTE_BAND7=0x40,
    QUEC_BAND_LTE_BAND8=0x80,
    QUEC_BAND_LTE_BAND20=0x80000,
    QUEC_BAND_LTE_BAND28=0x8000000, 
    QUEC_BAND_LTE_BAND34=0x200000000,
    QUEC_BAND_LTE_BAND38=0x2000000000, 
    QUEC_BAND_LTE_BAND39=0x4000000000,
    QUEC_BAND_LTE_BAND40=0x8000000000,
    QUEC_BAND_LTE_BAND41=0x10000000000
}quec_band_type_e;


typedef enum {
    RDA_BAND_GSM_450=0x01,   //#define CSW_GSM_450 (1 << 0)
    RDA_BAND_GSM_480=0x02,   //#define CSW_GSM_480 (1 << 1)
    RDA_BAND_GSM_850=0x20,   //#define CSW_GSM_850 (1 << 5)
    RDA_BAND_PGSM_900=0x04,  //#define CSW_GSM_900P (1 << 2)
    RDA_BAND_EGSM_900=0x08,  //#define CSW_GSM_900E (1 << 3)
    RDA_BAND_GSM_1800=0x40,  //#define CSW_DCS_1800 (1 << 6)
    RDA_BAND_GSM_1900=0x80,  //#define CSW_PCS_1900 (1 << 7)

    RDA_BAND_LTE_BAND1=1 ,
    RDA_BAND_LTE_BAND3=3 ,
    RDA_BAND_LTE_BAND5=5 ,
    RDA_BAND_LTE_BAND7=7 ,
    RDA_BAND_LTE_BAND8=8 ,
    RDA_BAND_LTE_BAND20=20 ,
    RDA_BAND_LTE_BAND28=28 ,
    RDA_BAND_LTE_BAND34=34 ,
    RDA_BAND_LTE_BAND38=38 ,
    RDA_BAND_LTE_BAND39=39 ,
    RDA_BAND_LTE_BAND40=40 ,
    RDA_BAND_LTE_BAND41=41
}rda_band_type_e;

typedef enum 
{
    NW_IND  = 0,			/*  CREG	*/
    GPRS_IND ,    			/*  CGREG 	*/
    EPS_IND, 	    	    /*  CEREG 	*/
}quec_nw_ind_type_e;

#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW_AT
typedef enum
{
   MODE_SYNC = 0,
   MODE_AUTO_REFRESH = 1,
   MODE_ASYN = 2,
   MODE_QUERY = 3,
}qcellinfo_e;
#endif
/*========================================================================
*  Type Definition
*========================================================================*/
typedef struct
{
    uint8_t sMcc[3];
    uint8_t sMnc[3];
    uint8_t iBandInfo;
    uint32_t nArfcn;
}quec_qnwinfo_context_s;

typedef struct
{
    uint8_t     sMcc[3];        //Mobile country code
    uint8_t     sMnc[3];        //Mobile network code
    uint16_t    sLac;           //Location area code
    uint16_t    sCellID;        //Cell identity
    uint8_t     gsmband;        //gsm band  9:BAND_GSM900    18:BAND_DCS1800    19:BAND_PCS1900    0xFF:BAND_INVALID*/
    int8_t      iRxLev;         //RX level value for base station selection
} quec_qeng_gsm_context_s;

typedef struct
{
    uint8_t lteReqType;
    uint8_t replyErrCode;
}quec_qeng_lte_context_s;

typedef struct
{
    int64_t               quec_band;
    uint8_t               rda_band;
}quectel_band_info_s;

typedef struct
{
    uint8_t nStatus;
    uint8_t nAreaCode[5];
    uint8_t nCellId[4];
    uint8_t nAct;
}nw_status_info_s;

typedef struct
{
	uint32_t interval;
	quec_timer_t *timer; 
}quec_gdcnt_autosave_cfg_s;

#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW_AT
 typedef struct
 {
 	quec_thread_t *thread;
 	ql_mutex_t mutex;
	ql_timer_t timer;
	int urc_dev;
	uint16_t inteval;
	uint8_t mode;
	uint8_t nSim;
}qcellinfo_ctx;
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_RFTEST_AT
typedef struct
{
    uint8_t    mode;
    char      *name;
    uint16_t   band;
    uint16_t   num;
    uint16_t   tx_chanel_min;
    uint16_t   tx_chanel_max;
    uint16_t   rx_chanel_min;
    uint16_t   rx_chanel_max;
}qrftest_label_type;

typedef struct
{
    uint8_t    mode;
    uint8_t    status;
}qrftest_params_type;
#endif  /* CONFIG_QUEC_PROJECT_FEATURE_RFTEST_AT */

/*========================================================================
 *	function Definition
 *========================================================================*/

#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW_AT
bool quec_save_dataCount(void);
#endif

void quec_deal_nw_gprs_status_ind(uint8_t ind_type, const CFW_EVENT *cfw_event);

void quec_deal_nw_signal_quality_ind(const CFW_EVENT *cfw_event);

void quec_deal_ratchg_ind(uint8_t forece_report, uint8_t rat, uint8_t nw_status, uint8_t nSim);

void quec_deal_qcsq_ind(const CFW_EVENT *cfw_event);

#endif  /* CONFIG_QUEC_PROJECT_FEATURE_NW*/
#endif /* QUEC_ATCMD_NETWORK_H */
