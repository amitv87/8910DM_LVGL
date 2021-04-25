/**  
  @file
  ql_nw_internal.h

  @brief
  This file provides the definitions for nw, and declares the 
  internal API functions.

*/
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

#ifndef QL_NW_INTERNAL_H
#define QL_NW_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "cfw.h"
#include "ql_osi_def.h"
#include "ql_api_nw.h"


#ifdef __cplusplus
extern "C" {
#endif

/*========================================================================
 *	Macro Definition
 *========================================================================*/



/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
 
 
 
 
 /*========================================================================
*  Type Definition
*========================================================================*/
 
 
 
 /*========================================================================
 *	function Definition
 *========================================================================*/
uint8_t quec_regstatus_rda2quec(uint8_t nStatus);

uint8_t quec_get_regstatus(int32_t nSim);

int quec_get_reg_status(uint8_t nSim, ql_nw_reg_status_info_s *reg_info);

time_t quec_get_nitz_time(void);

void quec_save_nw_sync_time(time_t nw_time);

void quec_pdpAddressToStr(const CFW_GPRS_PDPCONT_INFO_V2 *pdp_cont, char *str, uint8_t cid, uint8_t Sim);

bool ql_nw_send_event(uint32_t id, uint32_t nParam1, uint32_t nParam2, uint32_t nParam3);

#ifdef __cplusplus
}/*"C" */
#endif

#endif   /*QL_NW_INTERNAL_H*/
