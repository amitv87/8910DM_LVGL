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
            marvin          create

=============================================================================*/

#ifndef QL_API_VOLTE_H
#define QL_API_VOLTE_H


#include <stdint.h>
#include <stdbool.h>
#include "ql_api_common.h"
#include "ql_api_osi.h"

#ifdef __cplusplus
 extern "C" {
#endif



 /*========================================================================
  *  Marco Definition
  *========================================================================*/

#define QL_VOLTE_LOG(msg, ...)	 QL_LOG(QL_LOG_LEVEL_INFO, "ql_volte", msg, ##__VA_ARGS__)

 /*========================================================================
  *  Enumeration Definition
  *========================================================================*/
typedef enum
{
	QL_VOLTE_SUCCESS					= 0,
	QL_VOLTE_ERROR					    = 1 | (QL_COMPONENT_IMS << 16),
	QL_VOLTE_PARA_ERR,
}ql_volte_errcode_e;


/*****************************************************************
* Function: ql_voice_call_volte_set
*
* Description:
*	Disable or enable VOLTE.
* 
* Parameters:
*	mode	  [in] 0:Disable VOLTE; 1:Enable VOLTE
*
* Return:ql_volte_errcode_e
*****************************************************************/
ql_volte_errcode_e ql_volte_set(uint8_t mode);

/*****************************************************************
* Function: ql_voice_call_volte_get
*
* Description:
*	get VoLTE status
* 
* Parameters:
*	mode	  [out] 0:Disable VOLTE; 1:Enable VOLTE
*
* Return:ql_volte_errcode_e
*****************************************************************/
ql_volte_errcode_e ql_volte_get(uint8_t *mode);


/*****************************************************************
* Function: ql_volte_vdp_set
*
* Description:
*	set the specified voice domain preference
* 
* Parameters:
*	setting [in] :
*   	1 CS Voice only 
*       2 CS Voice preferred, IMS PS Voice as secondary
*       3 IMS PS Voice preferred, CS Voice as secondary
*       4 IMS PS Voice only
*
* Return:ql_volte_errcode_e
*****************************************************************/
ql_volte_errcode_e ql_volte_vdp_set(uint8_t setting);


/*****************************************************************
* Function: ql_volte_vdp_get
*
* Description:
*	get the specified voice domain preference
* 
* Parameters:
* Parameters:
*	setting [out] :
*   	1 CS Voice only 
*       2 CS Voice preferred, IMS PS Voice as secondary
*       3 IMS PS Voice preferred, CS Voice as secondary
*       4 IMS PS Voice only
*
* Return:ql_volte_errcode_e
*****************************************************************/
ql_volte_errcode_e ql_volte_vdp_get(uint8_t *setting);


/*****************************************************************
* Function: ql_volte_usage_set
*
* Description:
*	set the usage of the UE
* 
* Parameters:
*	setting   [in] 0:voice centric   1:data centric
*
* Return:ql_volte_errcode_e
*****************************************************************/
ql_volte_errcode_e ql_volte_usage_set(uint8_t setting);


/*****************************************************************
* Function: ql_volte_usage_get
*
* Description:
*	get the usage of the UE
* 
* Parameters:
*	setting   [out] 0:voice centric   1:data centric
*
* Return:ql_volte_errcode_e
*****************************************************************/
ql_volte_errcode_e ql_volte_usage_get(uint8_t *setting);


#ifdef __cplusplus
 }/*"C" */
#endif


#endif

