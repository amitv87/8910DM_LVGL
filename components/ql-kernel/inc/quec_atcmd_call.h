#ifndef QUEC_ATCMD_CALL_H
#define QUEC_ATCMD_CALL_H


/**  @file
  quec_atcmd_sms.h

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
11/19/2020        Perlino     Init version
=================================================================*/



#include "at_command.h"
#include "quec_atcmd_general.h"
#include "quec_common.h"
#include "quec_rtos.h"
#include "ql_api_voice_call.h"

/*========================================================================
 *  Macro Definition
 *========================================================================*/

#define URI_MAX_NUMBER 21
#define NUM_ECCSIM        2
#define NUM_ECCNOSIM      8


/*========================================================================
*  Type Definition
*========================================================================*/

typedef struct
{
	bool cvhu;
	uint16_t ats7;
}QUE_CALL_ATTYPE;


typedef struct
{
    uint8_t g_cc_h_cnt; // this is used to record whether the csw sent one or two rsp to AT.
} hAsyncCtx_t;


typedef enum
{
    QUEC_CALL_IDLE     = 0, // no call
    QUEC_CALL_HOLD     = 1,
    QUEC_CALL_ORIGINAL = 2,
    QUEC_CALL_CONNECT  = 3,
    QUEC_CALL_INCOMING = 4,
    QUEC_CALL_WAITING  = 5,
    QUEC_CALL_END      = 6,
    QUEC_CALL_ALERTING = 7,
}quec_dsci_call_state_e;



/******************************************************************************
*   Global Variable
******************************************************************************/

extern QUE_CALL_ATTYPE q_atcmd_par; 

extern ql_timer_t Qats7Timer;

extern quec_at_ecc_s quec_at_ecc_current;

extern uint8_t quec_dsci_flag;
extern CFW_CC_CURRENT_CALL_INFO g_callinfo[];

extern quec_at_ecc_s quec_at_ecc_default;

/*===========================================================================
* Functions
===========================================================================*/


void QATS7_TimmerHandler(void *param);


bool atGetUriCallNumber(
    const char *uri_ptr,            //[IN]
    char uriNum[URI_MAX_NUMBER + 1] //[OUT]
);

void _hAsyncContextDelete(atCommand_t *cmd);

void _chccsRspCB(atCommand_t *cmd, const osiEvent_t *event);

bool quec_at_ecc_save(void);

void quec_at_setting_ecc_default(void);

void _athRspCB(atCommand_t *cmd, const osiEvent_t *event);

void quec_dsci_urc(uint8_t sim,uint8_t index, uint8_t status);


#endif/* QUEC_ATCMD_CALL_H */

