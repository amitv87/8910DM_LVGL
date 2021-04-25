
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

#ifndef QL_DATACALL_INTERNAL_H
#define QL_DATACALL_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>
#include "ql_api_datacall.h"


#ifdef __cplusplus
extern "C" {
#endif

/*========================================================================
 *	Macro Definition
 *========================================================================*/



/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
typedef enum
{
    QL_DATACALL_IDLE,
    QL_DATACALL_ACTIVING,
    QL_DATACALL_ACTIVED,
    QL_DATACALL_DEACTIVING,
}ql_datacall_cid_status_e;

typedef enum
{
	PDP_OP_ACT,
	PDP_OP_DACT
}ql_pdp_act_op_e;
 
 /*========================================================================
*  Type Definition
*========================================================================*/
typedef struct
{
	int ip_version;
	char apn_name[THE_APN_MAX_LEN];
	char username[THE_APN_USER_MAX_LEN];
	char password[THE_APN_PWD_MAX_LEN];
	int auth_type;
}ql_data_call_conf_s;

typedef struct
{
	 int			     pdp_cid;
	 int                 sim_cid;
	 ql_pdp_act_op_e	 op;
	 int			     status;
 }ql_pdp_act_status_t;


typedef struct
{
	uint8_t			pdp_type;
	uint8_t			apn_size;
	char 			apn_name[THE_APN_MAX_LEN];
}ql_default_pdp_info_t;

	
 /*========================================================================
 *	function Definition
 *========================================================================*/
ql_datacall_errcode_e ql_get_data_call_conf(uint8_t nSim, int profile_idx, ql_data_call_conf_s *conf);

ql_datacall_cid_status_e quec_get_datacall_status(uint8_t nSim, int profile_idx);

bool quec_set_datacall_status(uint8_t nSim, int profile_idx, ql_datacall_cid_status_e status);

void quec_datacall_fail_handler(uint8_t nSim, int profile_idx);

ql_datacall_errcode_e ql_datacall_active_pdp(uint8_t nSim, int profile_idx);

bool quec_get_default_pdn_info(CFW_SIM_ID nSimID, ql_default_pdp_info_t * dft_pdp);


#ifdef __cplusplus
}/*"C" */
#endif

#endif   /*QL_DATACALL_INTERNAL_H*/
