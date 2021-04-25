/*============================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------

=============================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_api_nw.h"

#define QL_NW_LOG_LEVEL	                    QL_LOG_LEVEL_INFO
#define QL_NW_DEMO_LOG(msg, ...)			QL_LOG(QL_NW_LOG_LEVEL, "ql_nw_demo", msg, ##__VA_ARGS__)

ql_task_t nw_task = NULL;

void ql_nw_notify_cb(uint8_t sim_id, unsigned int ind_type, void *ind_msg_buf)
{    
    switch(ind_type)
    {
        case QUEC_NW_VOICE_REG_STATUS_IND:
        {
             ql_nw_common_reg_status_info_s  *voice_reg_status=(ql_nw_common_reg_status_info_s  *)ind_msg_buf;
             QL_NW_DEMO_LOG("Sim%d voice: state:%d; lac:0x%X; cid:0x%X; act:%d",
			 			sim_id,
                        voice_reg_status->state,
                        voice_reg_status->lac,
                        voice_reg_status->cid,
                        voice_reg_status->act);
              break;
        }
        case QUEC_NW_DATA_REG_STATUS_IND:
        {
             ql_nw_common_reg_status_info_s  *data_reg_status=(ql_nw_common_reg_status_info_s  *)ind_msg_buf;
             QL_NW_DEMO_LOG("Sim%d data: state:%d; lac:0x%X; cid:0x%X; act:%d",
			 			sim_id,
                        data_reg_status->state,
                        data_reg_status->lac,
                        data_reg_status->cid,
                        data_reg_status->act);
              break;
        }
        case QUEC_NW_SIGNAL_QUALITY_IND:
        {
             ql_nw_signal_strength_info_s  *signal_info=(ql_nw_signal_strength_info_s  *)ind_msg_buf;
             QL_NW_DEMO_LOG("Sim%d signal: rssi:%d, bitErrorRate:%d, rsrp:%d, rsrq:%d",
			 					sim_id,
                                signal_info->rssi, signal_info->bitErrorRate, 
                                signal_info->rsrp, signal_info->rsrq);
              break;
        }
        case QUEC_NW_NITZ_TIME_UPDATE_IND:
        {
             ql_nw_nitz_time_info_s  *nitz_info=(ql_nw_nitz_time_info_s  *)ind_msg_buf;
             QL_NW_DEMO_LOG("nitz update: nitz_time:%s, abs_time:%ld", nitz_info->nitz_time, nitz_info->abs_time);
             break;
        }
    }
}

static void nw_app_thread(void * arg)
{
    QlOSStatus err = 0;
    int ret = 0;
	uint8_t sim_id = 0;
    unsigned char cell_index = 0, csq = 0;
    ql_nw_mode_type_e nw_mode = 0;
    ql_nw_reg_status_info_s *reg_info = (ql_nw_reg_status_info_s *)calloc(1, sizeof(ql_nw_reg_status_info_s));
    ql_nw_signal_strength_info_s *signal_info = (ql_nw_signal_strength_info_s *)calloc(1, sizeof(ql_nw_signal_strength_info_s));
    ql_nw_operator_info_s *oper_info = (ql_nw_operator_info_s *)calloc(1, sizeof(ql_nw_operator_info_s));
    ql_nw_seclection_info_s *select_info = (ql_nw_seclection_info_s *)calloc(1, sizeof(ql_nw_seclection_info_s));
    ql_nw_nitz_time_info_s *nitz_info = (ql_nw_nitz_time_info_s *)calloc(1, sizeof(ql_nw_nitz_time_info_s));
    ql_nw_cell_info_s *cell_info = (ql_nw_cell_info_s *)calloc(1, sizeof(ql_nw_cell_info_s));

	if((NULL == reg_info) || (NULL == signal_info) || (NULL == oper_info) 
		|| (NULL == select_info) || (NULL == nitz_info) || (NULL == cell_info))
	{
		QL_NW_DEMO_LOG("calloc fail");
		goto exit;
	}

    if(ql_nw_register_cb(ql_nw_notify_cb) != QL_NW_SUCCESS)
    {
		QL_NW_DEMO_LOG("register network callback fail");
		goto exit;
	}

    while(1)
    {
        ql_rtos_task_sleep_ms(10000);

        QL_NW_DEMO_LOG("====================nw demo start===================");

        ret = ql_nw_get_mode(sim_id, &nw_mode);
        QL_NW_DEMO_LOG("ret=0x%x, nw_mode:%d", ret, nw_mode);
        
        ret = ql_nw_get_reg_status(sim_id, reg_info);
        QL_NW_DEMO_LOG("ret=0x%x", ret);
        QL_NW_DEMO_LOG("voice: state:%d, lac:0x%X, cid:0x%X, act:%d", reg_info->voice_reg.state, reg_info->voice_reg.lac, reg_info->voice_reg.cid, reg_info->voice_reg.act);
        QL_NW_DEMO_LOG("data:  state:%d, lac:0x%X, cid:0x%X, act:%d", reg_info->data_reg.state, reg_info->data_reg.lac, reg_info->data_reg.cid, reg_info->data_reg.act);

        ret = ql_nw_get_csq(sim_id, &csq);
        QL_NW_DEMO_LOG("ret=0x%x, csq:%d", ret, csq);

        ret = ql_nw_get_signal_strength(sim_id, signal_info);
        QL_NW_DEMO_LOG("ret=0x%x, rssi:%d, bitErrorRate:%d, rsrp:%d, rsrq:%d", 
                                ret, signal_info->rssi, signal_info->bitErrorRate, 
                                signal_info->rsrp, signal_info->rsrq);

        ret = ql_nw_get_operator_name(sim_id, oper_info);
        QL_NW_DEMO_LOG("ret=0x%x, long_oper_name:%s, short_oper_name:%s, mcc:%s, mnc:%s", 
                                ret, oper_info->long_oper_name, oper_info->short_oper_name, oper_info->mcc, oper_info->mnc);

        ret = ql_nw_get_selection(sim_id, select_info);
        QL_NW_DEMO_LOG("ret=0x%x, nw_selection_mode:%d, mcc:%s, mnc:%s, act:%d", 
                                ret, select_info->nw_selection_mode, select_info->mcc, select_info->mnc, select_info->act);

        ret = ql_nw_get_nitz_time_info(nitz_info);
        QL_NW_DEMO_LOG("ret=0x%x, nitz_time:%s, abs_time:%ld", ret, nitz_info->nitz_time, nitz_info->abs_time);

        ret = ql_nw_get_cell_info(sim_id, cell_info);
        QL_NW_DEMO_LOG("ret=0x%x", ret);
        if(cell_info->gsm_info_valid)
        {
            for(cell_index = 0; cell_index < cell_info->gsm_info_num; cell_index++)
            {
                QL_NW_DEMO_LOG("Cell_%d [GSM] flag:%d, cid:0x%X, mcc:%d, mnc:%02d, lac:0x%X, arfcn:%d, bsic:%d, rssi:%d",
                cell_index,
                cell_info->gsm_info[cell_index].flag,
                cell_info->gsm_info[cell_index].cid,
                cell_info->gsm_info[cell_index].mcc,
                cell_info->gsm_info[cell_index].mnc,
                cell_info->gsm_info[cell_index].lac,
                cell_info->gsm_info[cell_index].arfcn,
                cell_info->gsm_info[cell_index].bsic,
				cell_info->gsm_info[cell_index].rssi);
            }
        }
        if(cell_info->lte_info_valid)
        {
            for(cell_index = 0; cell_index < cell_info->lte_info_num; cell_index++)
            {
                QL_NW_DEMO_LOG("Cell_%d [LTE] flag:%d, cid:0x%X, mcc:%d, mnc:%02d, tac:0x%X, pci:%d, earfcn:%d, rssi:%d",
                cell_index,
                cell_info->lte_info[cell_index].flag,
                cell_info->lte_info[cell_index].cid,
                cell_info->lte_info[cell_index].mcc,
                cell_info->lte_info[cell_index].mnc,
                cell_info->lte_info[cell_index].tac,
                cell_info->lte_info[cell_index].pci,
                cell_info->lte_info[cell_index].earfcn,
				cell_info->lte_info[cell_index].rssi);
            }
        }

        QL_NW_DEMO_LOG("====================nw demo end===================");        
    }

exit:
	if(NULL != reg_info)
		free(reg_info);
	if(NULL != signal_info)
		free(signal_info);
	if(NULL != oper_info)
		free(oper_info);
	if(NULL != select_info)
		free(select_info);
	if(NULL != nitz_info)
		free(nitz_info);
	if(NULL != cell_info)
		free(cell_info);

    err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_NW_DEMO_LOG("task deleted failed");
	}
}

void ql_nw_app_init(void)
{
    QlOSStatus err = QL_OSI_SUCCESS;
    
    err = ql_rtos_task_create(&nw_task, 1024*4, APP_PRIORITY_NORMAL, "QNWDEMO", nw_app_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
	{
		QL_NW_DEMO_LOG("created task failed");
	}
}

