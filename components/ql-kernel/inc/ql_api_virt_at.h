/**  @file
  ql_api_virt_at.h

  @brief
  This file is used to define virt at interface for different Quectel Project.

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
26/01/2021        Neo         Init version
=================================================================*/


#ifndef _QL_API_VIRT_AT_H_
#define _QL_API_VIRT_AT_H_

#include "ql_api_common.h"

#ifdef __cplusplus
	 extern "C" {
#endif


/*===========================================================================
 * Macro Definition
 *===========================================================================*/
#define QL_VIRT_AT_ERRCODE_BASE (QL_COMPONENT_BSP_VIRT_AT<<16)


/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
/**
 * virt at errcode
 */
typedef enum
{
    QL_VIRT_AT_SUCCESS = QL_SUCCESS,

    QL_VIRT_AT_INVALID_PARAM_ERR                 = 1|QL_VIRT_AT_ERRCODE_BASE,
    QL_VIRT_AT_EXECUTE_ERR,
    QL_VIRT_AT_OPEN_REPEAT_ERR,
}ql_errcode_virt_at_e;

/**
 * virtual at channel enum
 */
typedef enum 
{
    QL_VIRT_AT_PORT_0,
	QL_VIRT_AT_PORT_1,
	QL_VIRT_AT_PORT_2,
	QL_VIRT_AT_PORT_3,
    QL_VIRT_AT_PORT_4,
    QL_VIRT_AT_PORT_5,
    QL_VIRT_AT_PORT_6,
    QL_VIRT_AT_PORT_7,
	QL_VIRT_AT_PORT_8,
	QL_VIRT_AT_PORT_9,    
	QL_VIRT_AT_PORT_MAX,
}ql_virt_at_port_number_e;

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
/*****************************************************************
* Description: VIRT AT ???????????????????????????AT????????????
* 
* Parameters:
*   ind_type    [in]    ???????????????VIRT      AT RX????????????????????? 
*   size        [in]    ????????????
*
*****************************************************************/
typedef void (*ql_virt_at_callback)(uint32 ind_type, uint32 size);


/*****************************************************************
* Function: ql_virt_at_read
*
* Description: VIRT AT?????????
* 
* Parameters:
*   port	    [in] 	??????AT?????? 
* 	data	    [out] 	??????????????? 
*   data_len    [in]    ????????????????????????
*
* Return:
*   0           ?????????????????????
*	other 	    ????????????
*
*****************************************************************/
ql_errcode_virt_at_e ql_virt_at_read(ql_virt_at_port_number_e port, unsigned char *data, unsigned int data_len);

/*****************************************************************
* Function: ql_virt_at_write
*
* Description: VIRT AT?????????
* 
* Parameters: 
*   port	    [in] 	??????AT?????? 
* 	data	    [in] 	?????????????????? 
*   data_len    [in]    ????????????????????????
*
* Return:
*   0           ?????????????????????
*	other 	    ????????????
*
*****************************************************************/
ql_errcode_virt_at_e ql_virt_at_write(ql_virt_at_port_number_e port, unsigned char *data, unsigned int data_len);


/*****************************************************************
* Function: ql_virt_at_close
*
* Description: ?????? VIRT AT??????
* 
* Parameters: 
*   port	    [in] 	??????AT?????? 
*
* Return:
* 	0			???????????????
*	other 	    ????????????
*
*****************************************************************/
ql_errcode_virt_at_e ql_virt_at_close(ql_virt_at_port_number_e port);

/*****************************************************************
* Function: ql_virt_at_open
*
* Description: ?????? VIRT AT??????,??????????????????
* 
* Parameters:
*   port	    [in] 	??????AT??????
*   virt_at_cb	[in] 	???????????? 
*
* Return:
* 	0			???????????????
*	other 	    ????????????
*
*****************************************************************/
ql_errcode_virt_at_e ql_virt_at_open(ql_virt_at_port_number_e port, ql_virt_at_callback virt_at_cb);




#ifdef __cplusplus
	 } /*"C" */
#endif
 
#endif /* _QL_API_VIRT_AT_H_ */


