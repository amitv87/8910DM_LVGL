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
11/3/2020        Perlino     Init version
=================================================================*/


#ifndef QUEC_ATCMD_SMS_H
#define QUEC_ATCMD_SMS_H

#include "at_command.h"
#include "quec_atcmd_general.h"
#include "quec_common.h"
#include "quec_rtos.h"
#include "ql_api_sms.h"





/*========================================================================
 *  Macro Definition
 *========================================================================*/

#define SMS_MAX_SIZE (5 * 160)




/*========================================================================
*  Type Definition
*========================================================================*/
typedef struct 
{
	uint8_t sca[12];
	uint8_t pdu_type;
	uint8_t mr;
	uint8_t da;
	uint8_t pid;
	uint8_t dcs;
	uint8_t scts[7];
	uint8_t udl;
	uint16_t uid;
	uint8_t seg_num;
	uint8_t seg_tatal;
}QUE_SMS_PDUTYPE;

typedef struct
{
    CFW_DIALNUMBER_V2 number;
    uint8_t *data;
    unsigned pdu_length;
} cmgsAsyncContext_t;



typedef struct{
	uint8_t status;
	uint8_t index;
	char* buf;
	uint16_t buf_len;
}qat_sms_msg_s;


typedef struct
{
	uint32_t err;
	uint16_t uti;
	qat_sms_msg_s *msg;
	osiSemaphore_t *samphore;
}qat_sms_ctx_s;


/*========================================================================
 *	Variable Definition
 *========================================================================*/

extern uint8_t g_SMS_CIND_NewMsg;
extern ql_timer_t QcmmsTimer;
/*===========================================================================
* Functions
===========================================================================*/


/*
 * Convert cp936 ucs2 format number to real number
 *
 * cp936 ucs2 number 30 30 33 31 30 30 33 30 30 30 33 30 30 30 33 38 30 30 33 36
 * real number 10086
 */
char *_generateRealNumberForCmgsAndCmgwInUcs2(const char *da, size_t *daLength, uint32_t *errorCode);


/******************************************************************************************
Function            :   AT_SMS_IsValidPhoneNumber
Description     :       check number copy from pbk module is valid or not
Called By           :   by SMS module
Data Accessed       :
Data Updated        :
Input           :       uint8_t * arrNumber, uint8_t nNumberSize, bool * bIsInternational
Output          :
Return          :   bool
Others          :       //[+]2007.11.29 for check number copy from pbk module
                    //arrNumber[in]; pNumberSize[in]; bIsInternational[out]
                    //legal phone number char: 0-9,*,#,+ (+ can only be the first position)
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
bool AT_SMS_IsValidPhoneNumber(const char *s, size_t size, bool *bIsInternational);


/**
 * check the number inside PDU is prohibited by FDN feature
 */
bool _smsFDNProhibit(const uint8_t *pdu_data, uint8_t nSim);

/**
 * add sca to pdu
 */
bool SMS_AddSCA2PDU(uint8_t *InPDU, uint8_t *OutPDU, uint16_t *pSize, uint8_t nSim);

void CMGS_TextPromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size);

void CMGS_AsyncCtxDestroy(atCommand_t *cmd);

uint8_t _smsCfwStatusToAt(uint8_t status);

char *generateUrcUcs2Data(char *src, uint16_t src_length);

uint8_t _smsCfwDcsToOutDcs(uint8_t InDCS);

const char *_smsAtStatusStr(uint8_t status);

char *_smsNodeDataStr(uint8_t type, CFW_SMS_INFO_UNION *info, uint8_t nSim);

void _smsNumberStr(const uint8_t *bcd, unsigned size, unsigned type, char *s);

bool AT_SMS_CheckPDUIsValid(uint8_t *pPDUData, uint8_t *nPDULen, bool bReadOrListMsgFlag);


void QCMMS_TimmerHandler(void *param);


#endif/* QUEC_ATCMD_SMS_H */


