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

#ifndef __TB_INTERNAL_H__
#define __TB_INTERNAL_H__

#include "tb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_CODE_POINT (0xffffffff)

typedef struct
{
    uint32_t prv_rx_bytes;
    uint32_t prv_tx_bytes;
    uint32_t cur_rx_bytes;
    uint32_t cur_tx_bytes;
    uint32_t saved_rx_bytes;
    uint32_t saved_tx_bytes;
    time_t act_time;
    time_t saved_time;
} tb_DataStatistics;

extern int tb_get_signal_bar_gsm(uint8_t nCsq);
extern int tb_get_signal_bar_lte(uint8_t nRscp);
extern int tb_handle_nw_signal_quality_ind(const CFW_EVENT *cfw_event);
extern int tb_handle_nw_reg_status_ind(const CFW_EVENT *cfw_event);
extern void tb_IMSItoHomePlmn(uint8_t *InPut, uint8_t *OutPut, uint8_t *OutLen);
extern int tb_handle_Gprs_Act_Rsp(const CFW_EVENT *cfw_event);
extern int tb_handle_Gprs_Active_Ind(const CFW_EVENT *cfw_event);
extern int tb_handle_Gprs_DeActive_Ind(const CFW_EVENT *cfw_event);
extern int tb_handle_Init_ind(const CFW_EVENT *cfw_event);
extern int tb_handle_Srv_Init_ind(const CFW_EVENT *cfw_event);
extern void tb_dataStatistics_timer_handler(void *timer);
extern void tb_start_data_statistics_timer(tb_data_profile_id cid);
extern void tb_stop_data_statistics_timer(tb_data_profile_id cid);
extern uint8_t tb_data_is_disconnected(void);
extern int tb_handle_Sms_Send_Rsp(const CFW_EVENT *cfw_event);
extern int tb_handle_Sms_Delete_Rsp(const CFW_EVENT *cfw_event);
extern int tb_handle_Sms_Read_Rsp(const CFW_EVENT *cfw_event);
extern int tb_handle_New_Sms_Ind(const CFW_EVENT *pCfwEvent);
extern int tb_handle_CC_Initiate_Speech_Call_Rsp(const CFW_EVENT *cfw_event);
extern int tb_handle_CC_Accept_Call_Rsp(const CFW_EVENT *cfw_event);
extern int tb_handle_CC_HungUp_Rsp(const CFW_EVENT *cfw_event);
extern int tb_handle_CC_Speech_Call_Ind(const CFW_EVENT *pCfwEvent);
extern int tb_handle_CC_Release_Call_Ind(const CFW_EVENT *pCfwEvent);
extern int tb_handle_CC_Error_Call_Ind(const CFW_EVENT *pCfwEvent);
extern int tb_handle_CC_Alerting_Call_Ind(const CFW_EVENT *cfw_event);

#ifdef __cplusplus
}
#endif
#endif
